// ==WindhawkMod==
// @id              win10-1507-start-menu-recreation
// @name            Windows 10 1507 Start Menu Recreation
// @description     This mod recreates the Windows 10 RTM (1507) Start menu on Windows 10 and Windows 11
// @version         1.1.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         GPL-3.0
// @include         explorer.exe
// @compilerOptions -lole32 -luser32 -lgdi32 -ldwmapi -lshell32 -loleaut32 -luuid -lshcore -lcomctl32 -lshlwapi -ladvapi32 -lgdiplus -lpowrprof -lsecur32 -lmsimg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 1507 (RTM) Start Menu Recreation

This mod is a  best-effort recreation of the Windows 10 RTM (1507) Start menu for Windows 10
and Windows 11. The native Start menu is neither themed nor patched: this mod
builds its own window inside Explorer and opens it in place of the system one.

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win10startmenu.png)

This modification has been tested on Windows 10 21H2, Windows 11 24H2 and Windows 11 25H2.

## Getting started

* Enable the mod in Windhawk. The new menu becomes active immediately, with no
  need to restart Explorer or sign out.
* The menu opens with the Win key or with a click on the Start button, exactly
  like the system one.
* The window can be resized by dragging its top or right edge, and the chosen
  size is remembered.
* Accent colour, menu width, animations, profile picture and privacy options are
  available in the mod settings.

## What the menu provides

* **App lists** - Most used, Recently added, All apps (A-Z, with folders) and
  type-to-search. The lists refresh automatically when shortcuts are added to or
  removed from the Start menu folders, and periodically for Store apps.
* **Tiles** - default groups are built from the applications actually installed.
  A tile lifts under the pointer while it is dragged, stays inside the tile area
  and the tiles it pushes aside glide to their new cell, with a collision-free
  reflow; on release the tile settles into the chosen cell. An application can
  be pinned only once: pinning it again keeps the existing tile. Tiles can also
  be unpinned and set to four sizes, and an optional live-tile flip is
  available, off by default.
* **Account and power commands** - account picture and name, lock, sign out,
  sleep, hibernate (when available), shut down, restart, plus the pinned places
  of File Explorer.
* **Presentation** - resizable window, reversible slide animation, 95% opacity
  and per-monitor DPI awareness.
* **Storage** - tile layout, window size and launch counts are kept in Windhawk's
  mod storage. Nothing is written outside it.

## Keyboard

* **Win** - opens or closes the menu.
* **Any character** - starts a search straight away; the best match is selected
  automatically.
* **Up / Down** - move the selection through the app list, then on to the account
  tile above it or the bottom bar (File Explorer, Settings, Power, All apps)
  below it.
* **Right / Left** - enter the tile grid and return to the left column. Inside
  the grid, the arrow keys move between neighbouring tiles and across groups.
* **Tab / Shift+Tab** - cycle through the four areas of the menu.
* **Page Up / Page Down** - scroll the list one screen at a time.
* **Home / End** - jump to the first or last entry of the list, or of the focused
  tile group.
* **Enter** - opens the selected item; on a folder of All apps it expands or
  collapses it, and on the account tile, Power or All apps it performs the same
  action as a click.
* **Menu key or Shift+F10** - opens the context menu of the selected item (pin,
  unpin, resize, run as administrator, open file location).
* **Esc** - clears an active search, otherwise closes the menu.
* **Ctrl+A** - selects the whole search text so that it can be replaced.

Keyboard focus and mouse hover share a single highlight: moving the pointer hands
the highlight back to the mouse, and the next arrow key takes it over again.

## Notes

* Default tile groups only contain applications found on the system. A missing
  legacy app makes a group smaller, or leaves it out entirely; this is expected
  behaviour rather than a defect.
* The menu is opened by intercepting the Win key and clicks on the Start button
  through low-level hooks that run on a dedicated thread. Clicks are intercepted
  only when a taskbar window is actually under the pointer, so full-screen
  applications covering the taskbar keep receiving them.
* Tile movements are animated by a 60 Hz timer that only runs while something
  is actually moving. When the timer cannot be created, the tiles are placed
  directly in their cell and everything else keeps working.
* Windhawk must already be running with this mod enabled at sign-in. Portable
  installations need a Startup-folder shortcut to `windhawk.exe -tray-only`.

## Known limitations

* Live tiles are limited: Windows does not provide a documented API that lets a
  third-party Start menu read another app's live tile content. Only the Calendar
  tile is animated, showing today's weekday and date computed locally; all other
  tiles stay static. Should Microsoft document such an API in the future, the mod
  can be updated to support it.
* The open and close animation approximates the original motion with a standard
  Win32 slide. It is not the private DWM transition used by Windows, so timing
  and easing may differ slightly.

## Credits

* AdmnistratoX - Testing on Windows 11 25H2 and providing technical references
* m417z - Windows 11 Start Menu Styler (technical reference)
* bbmaster123 - Windows 10 theme for Windows 11 Start Menu Styler (technical reference)
* Carl Fredricksen [dwm] - Bug Report
* ac - Bug Report
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- accentMode: auto
  $name: Accent color
  $description: This setting controls which accent color is used to tint Start menu highlights, tiles, and interactive elements. "System accent" follows the color configured in Windows personalisation settings, while "Windows 10 1507 default" restores the fixed blue accent used by the original 2015 release for a more authentic retro appearance.
  $options:
  - auto: System accent
  - classic: Windows 10 1507 default
- liveTileAnimation: false
  $name: Calendar tile flip animation
  $description: This setting enables the periodic flip animation on the Calendar tile only, which then alternates between its icon and today's weekday and date, computed locally. It is disabled by default. Microsoft documents tile notifications as a write-only API for an app's own tile (TileUpdateManager.CreateTileUpdaterForApplication), so there is no supported way for a third-party Start menu to read the live content of Mail, Photos, Weather or other apps; those tiles therefore stay static instead of showing made-up content.
- menuAnimation: true
  $name: Menu open/close slide animation
  $description: This setting enables a nonblocking, reversible 120 ms eased Win32 slide when the Start menu opens or closes. It is intended to approximate the original Start menu motion but is not the original private DWM transition, so timing and easing may differ slightly from the genuine Windows 10 1507 behaviour.
- appTileColors: false
  $name: Use app-specific tile colors (sampled from the app icon)
  $description: This setting makes each Start menu tile use a color sampled directly from the corresponding application icon instead of the global accent color. This produces a more colorful and varied tile grid reminiscent of the original Windows 10 Start screen, at the cost of a slightly longer tile rendering pass when the menu is first shown.
- liveTileIntervalMs: 4000
  $name: Live tile flip interval (ms)
  $description: This setting defines how many milliseconds elapse between each Calendar tile flip when the flip animation is enabled. Lower values make the tiles cycle more rapidly and feel busier, while higher values produce a calmer, more subdued effect. The default of 4000 ms matches the pacing used by the original Windows 10 live tiles.
- menuWidthOverride: 0
  $name: Menu width override in px (0 = default)
  $description: This setting overrides the width of the Start menu in pixels (measured at 100% scaling). A value of 0 keeps the default width, or the width the menu was last resized to. Any positive value forces the menu to that width, even if the menu was resized by dragging its edge. This is useful for replicating the narrower Start menu proportions seen in early Windows 10 builds.
- profilePicturePath: ""
  $name: Profile picture file path (empty = use Windows account picture, then bundled avatar fallback)
  $description: This setting specifies a custom file path to an image that will be used as the profile picture shown in the Start menu. When left empty, the mod falls back to the Windows account picture, and if that is unavailable it uses the bundled fallback avatar shipped with the mod, ensuring a picture is always displayed.
- privacyMode: false
  $name: Privacy mode (hide account name)
  $description: This setting hides the Windows account name in the Start menu and replaces it with a generic placeholder label, which is useful when taking screenshots, recording videos, or streaming without exposing personal information. When enabled, the account name is replaced with a localized generic label (for example "User" on English Windows).
- tileLayout: ""
  $name: Import tile layout (one-time, empty = none)
  $description: This setting allows to paste a serialized tile layout here (a "WHSM2#..." string) to import it. The layout is applied once, the first time the value changes; after that, your own tile drags, resizes and pins take precedence again. The current layout is otherwise saved automatically in Windhawk's mod storage, so most users can leave this empty.
*/
// ==/WindhawkModSettings==
/*
 Overview of how the mod works
 ----------------------------
 * Anchoring: the Start button rectangle is read from the Win32 "Start" child
   window (Windows 10), from UI Automation (AutomationId "StartButton", Windows 11
   XAML taskbar) or, as a last resort, derived from the taskbar rectangle. Primary
   and secondary taskbars are both handled. The menu is placed against the
   taskbar edge, clamped to the monitor work area, and laid out at that monitor's
   DPI (GetDpiForMonitor / WM_DPICHANGED).
 * Opening the menu: a WH_KEYBOARD_LL hook turns a bare Win tap into a toggle of
   the custom menu (an inert key, VK 0xE8, is injected before the Win release so
   the native menu is not opened; the release itself is passed through). A
   WH_MOUSE_LL hook swallows a left click on the Start button only when the
   window under the cursor belongs to a taskbar. Both hooks live on a dedicated
   thread that does nothing but pump messages, so slow work on the UI thread can
   never trip LowLevelHooksTimeout. WH_GETMESSAGE hooks on the Progman and
   taskbar threads drop WM_SYSCOMMAND/SC_TASKLIST. The cached Start rectangles are
   recomputed on the UI thread when the mouse enters a taskbar, when the menu
   opens, and on display/work-area changes; there is no periodic UIA polling.
 * Keyboard: the menu window takes the focus when it opens, so arrow keys,
   Tab, Home/End, Page Up/Down, Enter and the context-menu key are handled by
   its own window procedure. The keyboard highlight reuses the hover state, so
   only one element is ever highlighted; moving the mouse hands the highlight
   back to the pointer.
 * Tiles: front face = icon + name. Only the Calendar tile has a back face
   (weekday + date from GetDateFormatEx), because that is the only live content
   that can be computed locally: the documented tile APIs (TileUpdateManager /
   TileUpdater) are write-only and scoped to the calling app, so the real
   content of other apps' live tiles cannot be read. The flip is off by default
   and is a cosine deformation drawn into a GDI double buffer.
 * Lifetime: the menu window, catalog, GDI+ objects and hooks are created and
   destroyed on the mod's own threads. Globals with non-trivial teardown are
   marked [[clang::no_destroy]] and released explicitly when the UI thread exits,
   so nothing runs under the loader lock when Explorer exits.
*/

#ifndef NOMINMAX
#define NOMINMAX
#endif
#define SECURITY_WIN32

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <knownfolders.h>
#include <sddl.h>
#include <uiautomation.h>
#include <uxtheme.h>
#include <shellscalingapi.h>
#include <powrprof.h>
#include <security.h>
#include <windhawk_utils.h>
#include <wrl/client.h>

#include <algorithm>
#include <cwctype>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <climits>
#include <exception>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using Microsoft::WRL::ComPtr;
using namespace Gdiplus;

static ULONG_PTR g_gdiplusToken = 0;
static HICON g_stockFolderIcon = nullptr;
static HICON g_stockAppIcon = nullptr;
// Populated and used on the menu UI thread only. Cleared before catalog icon
// handles are replaced, so a recycled HICON value cannot return a stale bitmap.
// [[clang::no_destroy]]: the GDI+ bitmaps are released explicitly by the UI
// thread before GdiplusShutdown; the automatic destructor must not run at
// process exit (see the Windhawk wiki, "Global objects and process shutdown").
[[clang::no_destroy]] static std::optional<
    std::unordered_map<HICON, std::unique_ptr<Gdiplus::Bitmap>>>
    g_iconBitmapCache{std::in_place};
static void DrawAppIconOrFallback(HDC dc, HICON icon, int x, int y, int size);

constexpr UINT WM_APP_TOGGLE = WM_APP + 101;
constexpr UINT WM_APP_HIDE = WM_APP + 102;
constexpr UINT WM_APP_REFRESH = WM_APP + 103;
constexpr UINT WM_APP_PROFILE_PHOTO_READY = WM_APP + 104;
constexpr UINT WM_APP_REFRESH_ANCHOR = WM_APP + 105;
constexpr UINT_PTR TIMER_LIVE_FLIP = 2001;
constexpr UINT_PTR TIMER_LIVE_TICK = 2002;
constexpr UINT_PTR TIMER_MENU_ANIMATION = 2003;
constexpr UINT_PTR TIMER_ALL_APPS_SCROLL_REPEAT = 2004;
constexpr UINT_PTR TIMER_TILE_ANIM = 2005;
constexpr int TILE_PRESSED_SCALE_PERCENT = 94; // subtle press feedback; inspired by the 8045 reference mod.

// Tile movement animations, ported from the Windows 8 (8045) Start screen mod:
// a tile keeps an animated offset from its slot and eases into it exponentially
// instead of jumping, the pressed tile shrinks a little and the dragged one is
// lifted above the grid, slightly enlarged and semi-transparent. Only painting
// is animated: hit testing and the saved layout keep using the slot rectangles.
constexpr UINT TILE_ANIM_INTERVAL_MS = 16;      // ~60 frames per second
constexpr float TILE_ANIM_GLIDE_RATE = 16.0f;   // k = 1 - exp(-dt * rate)
constexpr float TILE_ANIM_SCALE_RATE = 22.0f;   // press/lift scale easing
constexpr float TILE_ANIM_SETTLE_PX = 0.3f;     // below this the glide is over
constexpr float TILE_ANIM_SETTLE_SCALE = 0.001f;
constexpr float TILE_PRESSED_SCALE = TILE_PRESSED_SCALE_PERCENT / 100.0f;
constexpr float TILE_DRAG_SCALE = 1.05f;        // the lifted tile grows by 5%
constexpr BYTE TILE_DRAG_ALPHA = 235;           // ~92% opacity, as in the reference
constexpr BYTE kMenuOpacity95Percent = 242; // 242/255 ~= 95% opacity
// The original 1507 Showing/Hiding paths pass 0x50 to private DWM calls.
// Use a nonblocking, documented Win32 geometry update with a slightly eased 120 ms slide.
constexpr UINT kMenuSlideDurationMs = 120;
constexpr UINT kMenuAnimationTimerMs = 10;
static RECT OffsetTowardTaskbar(RECT rect, UINT taskbarEdge, LONG distance) {
    LONG dx = 0, dy = 0;
    switch (taskbarEdge) {
        case ABE_TOP:    dy = -distance; break;
        case ABE_LEFT:   dx = -distance; break;
        case ABE_RIGHT:  dx = distance; break;
        case ABE_BOTTOM:
        default:         dy = distance; break;
    }
    OffsetRect(&rect, dx, dy);
    return rect;
}

// Context menu command identifiers
constexpr UINT IDM_OPEN_APP         = 3001;
constexpr UINT IDM_RUNAS_ADMIN      = 3002;
constexpr UINT IDM_OPEN_LOCATION    = 3003;
constexpr UINT IDM_PIN_TO_START     = 3004;
constexpr UINT IDM_UNPIN_FROM_START = 3005;
constexpr UINT IDM_RESIZE_SMALL     = 3006;
constexpr UINT IDM_RESIZE_MEDIUM    = 3007;
constexpr UINT IDM_RESIZE_WIDE      = 3008;
constexpr UINT IDM_RESIZE_LARGE     = 3009;
constexpr UINT IDM_TOGGLE_LIVE      = 3010;

constexpr UINT IDM_USER_SETTINGS    = 3101;
constexpr UINT IDM_USER_LOCK        = 3102;
constexpr UINT IDM_USER_SIGNOUT     = 3103;

constexpr UINT IDM_POWER_SLEEP      = 3201;
constexpr UINT IDM_POWER_SHUTDOWN   = 3202;
constexpr UINT IDM_POWER_RESTART    = 3203;
constexpr UINT IDM_POWER_HIBERNATE  = 3204;
constexpr UINT IDM_EXPLORER_DESKTOP = 3301;
constexpr UINT IDM_EXPLORER_DOWNLOADS = 3302;
constexpr UINT IDM_EXPLORER_DOCUMENTS = 3303;
constexpr UINT IDM_EXPLORER_PICTURES = 3304;

// ===================== ERROR HANDLING =====================
#define GUARDED(label, ...)                                                     \
    try {                                                                        \
        __VA_ARGS__                                                              \
    } catch (const std::exception& _e) {                                         \
        Wh_Log(L"[%s] C++ Exception: %S", label, _e.what());                     \
    } catch (...) {                                                              \
        Wh_Log(L"[%s] Unknown exception caught", label);                         \
    }

// ===================== RAII WRAPPERS =====================
template <typename T, typename Deleter>
class UniqueRes {
    T h_{};
public:
    UniqueRes() = default;
    explicit UniqueRes(T h) : h_(h) {}
    UniqueRes(const UniqueRes&) = delete;
    UniqueRes& operator=(const UniqueRes&) = delete;
    UniqueRes(UniqueRes&& o) noexcept : h_(o.h_) { o.h_ = T{}; }
    UniqueRes& operator=(UniqueRes&& o) noexcept {
        if (this != &o) reset(o.release());
        return *this;
    }
    ~UniqueRes() { reset(); }

    T get() const { return h_; }
    T release() { T t = h_; h_ = T{}; return t; }
    void reset(T h = T{}) {
        if (h_) Deleter()(h_);
        h_ = h;
    }
    explicit operator bool() const { return h_ != T{}; }
};

struct DelObj  { void operator()(HGDIOBJ h) const { DeleteObject(h); } };
struct DelIcon { void operator()(HICON h)   const { DestroyIcon(h); } };
struct DelWnd  { void operator()(HWND h)    const { DestroyWindow(h); } };
struct DelHnd  { void operator()(HANDLE h)  const { CloseHandle(h); } };
struct DelDC   { void operator()(HDC h)     const { DeleteDC(h); } };

using UniqueBitmap = UniqueRes<HBITMAP, DelObj>;
using UniqueFont   = UniqueRes<HFONT, DelObj>;
using UniqueBrush  = UniqueRes<HBRUSH, DelObj>;
using UniqueRegion = UniqueRes<HRGN, DelObj>;
using UniquePen    = UniqueRes<HPEN, DelObj>;
using UniqueIcon   = UniqueRes<HICON, DelIcon>;
using UniqueHwnd   = UniqueRes<HWND, DelWnd>;
using UniqueHandle = UniqueRes<HANDLE, DelHnd>;
using UniqueMemDC  = UniqueRes<HDC, DelDC>;

// FromHICON can lose semitransparent edge pixels on some legacy Shell icons.
// Copy the documented ICONINFO color bitmap and, when its alpha plane is empty,
// reconstruct transparency from the documented AND mask before handing pixels
// to GDI+ as straight 32-bit ARGB.
static std::unique_ptr<Gdiplus::Bitmap> CreateGdiplusBitmapFromIcon(HICON icon) {
    ICONINFO iconInfo{};
    if (!icon || !GetIconInfo(icon, &iconInfo)) return {};
    UniqueBitmap color(iconInfo.hbmColor);
    UniqueBitmap mask(iconInfo.hbmMask);
    if (!color) return {}; // monochrome icon; caller uses the native fallback

    BITMAP sourceInfo{};
    if (GetObjectW(color.get(), sizeof(sourceInfo), &sourceInfo) != sizeof(sourceInfo) ||
        sourceInfo.bmWidth <= 0 || sourceInfo.bmHeight <= 0 ||
        sourceInfo.bmWidth > 512 || sourceInfo.bmHeight > 512) return {};
    const int width = sourceInfo.bmWidth;
    const int height = sourceInfo.bmHeight;
    const size_t pixelBytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    std::vector<BYTE> pixels(pixelBytes);

    BITMAPINFO colorInfo{};
    colorInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    colorInfo.bmiHeader.biWidth = width;
    colorInfo.bmiHeader.biHeight = -height; // top-down rows
    colorInfo.bmiHeader.biPlanes = 1;
    colorInfo.bmiHeader.biBitCount = 32;
    colorInfo.bmiHeader.biCompression = BI_RGB;
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) return {};
    const int colorLines = GetDIBits(screenDc, color.get(), 0, static_cast<UINT>(height),
                                    pixels.data(), &colorInfo, DIB_RGB_COLORS);
    ReleaseDC(nullptr, screenDc);
    if (colorLines != height) return {};

    bool hasAlpha = false;
    bool hasPartialAlpha = false;
    bool alphaAllOpaque = true;
    for (size_t i = 3; i < pixels.size(); i += 4) {
        const BYTE a = pixels[i];
        hasAlpha = hasAlpha || a != 0;
        hasPartialAlpha = hasPartialAlpha || (a != 0 && a != 255);
        alphaAllOpaque = alphaAllOpaque && a == 255;
    }

    std::vector<BYTE> maskBits;
    size_t maskStride = ((static_cast<size_t>(width) + 31u) / 32u) * 4u;
    bool hasTransparentMaskPixel = false;
    if (mask) {
        maskBits.resize(maskStride * static_cast<size_t>(height));
        struct MaskBitmapInfo {
            BITMAPINFOHEADER header;
            RGBQUAD colors[2];
        } maskInfo{};
        maskInfo.header.biSize = sizeof(BITMAPINFOHEADER);
        maskInfo.header.biWidth = width;
        maskInfo.header.biHeight = -height;
        maskInfo.header.biPlanes = 1;
        maskInfo.header.biBitCount = 1;
        maskInfo.header.biCompression = BI_RGB;
        maskInfo.colors[0] = RGBQUAD{0, 0, 0, 0};
        maskInfo.colors[1] = RGBQUAD{255, 255, 255, 0};
        screenDc = GetDC(nullptr);
        if (screenDc) {
            const int maskLines = GetDIBits(screenDc, mask.get(), 0, static_cast<UINT>(height),
                                            maskBits.data(), reinterpret_cast<BITMAPINFO*>(&maskInfo), DIB_RGB_COLORS);
            ReleaseDC(nullptr, screenDc);
            if (maskLines == height) {
                for (int y = 0; y < height && !hasTransparentMaskPixel; ++y) {
                    const BYTE* row = maskBits.data() + static_cast<size_t>(y) * maskStride;
                    for (int x = 0; x < width; ++x) {
                        if ((row[x / 8] & (0x80u >> (x % 8))) != 0) {
                            hasTransparentMaskPixel = true;
                            break;
                        }
                    }
                }
            } else {
                maskBits.clear();
            }
        } else {
            maskBits.clear();
        }
    }

    // 32-bit icons with genuine partial alpha use that channel. Older icons
    // commonly have an all-zero alpha plane (or an opaque plane plus AND mask).
    // If neither a useful alpha plane nor a readable AND mask exists, let
    // DrawIconEx render the HICON rather than caching a potentially black box.
    if (!hasAlpha && maskBits.empty()) return {};
    const bool useMask = !hasPartialAlpha &&
                         ((!hasAlpha && !maskBits.empty()) ||
                          (alphaAllOpaque && hasTransparentMaskPixel));
    auto bitmap = std::make_unique<Gdiplus::Bitmap>(width, height, PixelFormat32bppARGB);
    if (bitmap->GetLastStatus() != Gdiplus::Ok) return {};
    Gdiplus::Rect bounds(0, 0, width, height);
    Gdiplus::BitmapData dst{};
    if (bitmap->LockBits(&bounds, Gdiplus::ImageLockModeWrite,
                         PixelFormat32bppARGB, &dst) != Gdiplus::Ok) return {};
    for (int y = 0; y < height; ++y) {
        const BYTE* src = pixels.data() + static_cast<size_t>(y) * static_cast<size_t>(width) * 4;
        BYTE* out = static_cast<BYTE*>(dst.Scan0) + static_cast<ptrdiff_t>(y) * dst.Stride;
        const BYTE* maskRow = maskBits.empty() ? nullptr : maskBits.data() + static_cast<size_t>(y) * maskStride;
        for (int x = 0; x < width; ++x) {
            BYTE alpha = src[x * 4 + 3];
            if (useMask && maskRow) {
                alpha = (maskRow[x / 8] & (0x80u >> (x % 8))) ? 0 : 255;
            } else if (!hasAlpha && !maskRow) {
                alpha = 255;
            }
            out[x * 4 + 0] = alpha ? src[x * 4 + 0] : 0;
            out[x * 4 + 1] = alpha ? src[x * 4 + 1] : 0;
            out[x * 4 + 2] = alpha ? src[x * 4 + 2] : 0;
            out[x * 4 + 3] = alpha;
        }
    }
    bitmap->UnlockBits(&dst);
    return bitmap;
}

// Derive a stable brand-like tile fill from the icon's dominant saturated hue.
// Called when a tile is attached/refreshed, never from the paint loop.
static COLORREF GetIconAccentColor(HICON icon, COLORREF fallback = RGB(0, 120, 215)) {
    try {
        auto bitmap = CreateGdiplusBitmapFromIcon(icon);
        if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) return fallback;
        const int width = static_cast<int>(bitmap->GetWidth());
        const int height = static_cast<int>(bitmap->GetHeight());
        if (width <= 0 || height <= 0) return fallback;

        Gdiplus::Rect bounds(0, 0, width, height);
        Gdiplus::BitmapData data{};
        if (bitmap->LockBits(&bounds, Gdiplus::ImageLockModeRead,
                             PixelFormat32bppARGB, &data) != Gdiplus::Ok) return fallback;
        double weight[24]{};
        double sumR[24]{}, sumG[24]{}, sumB[24]{};
        const int stepX = std::max(1, width / 32);
        const int stepY = std::max(1, height / 32);
        for (int y = 0; y < height; y += stepY) {
            const BYTE* row = static_cast<const BYTE*>(data.Scan0) + static_cast<ptrdiff_t>(y) * data.Stride;
            for (int x = 0; x < width; x += stepX) {
                const BYTE* px = row + static_cast<ptrdiff_t>(x) * 4;
                const double b = px[0] / 255.0, g = px[1] / 255.0, r = px[2] / 255.0;
                const double alpha = px[3] / 255.0;
                if (alpha < 0.38) continue;
                const double hi = std::max(r, std::max(g, b));
                const double lo = std::min(r, std::min(g, b));
                const double delta = hi - lo;
                if (hi < 0.18 || delta < 0.12) continue; // ignore shadows and neutral pixels
                double hue = 0.0;
                if (hi == r) hue = 60.0 * std::fmod((g - b) / delta, 6.0);
                else if (hi == g) hue = 60.0 * ((b - r) / delta + 2.0);
                else hue = 60.0 * ((r - g) / delta + 4.0);
                if (hue < 0.0) hue += 360.0;
                const int bin = std::min(23, static_cast<int>(hue / 15.0));
                const double w = alpha * (delta / std::max(0.01, hi));
                weight[bin] += w;
                sumR[bin] += r * w;
                sumG[bin] += g * w;
                sumB[bin] += b * w;
            }
        }
        bitmap->UnlockBits(&data);
        int best = 0;
        for (int i = 1; i < 24; ++i) if (weight[i] > weight[best]) best = i;
        if (weight[best] < 0.5) return fallback;
        const auto toByte = [](double value) {
            return static_cast<BYTE>((std::max)(0.0, (std::min)(255.0, value * 255.0)));
        };
        return RGB(toByte(sumR[best] / weight[best]),
                   toByte(sumG[best] / weight[best]),
                   toByte(sumB[best] / weight[best]));
    } catch (...) {
        return fallback;
    }
}

static void DrawAppIconOrFallback(HDC dc, HICON icon, int x, int y, int size) {
    if (!dc || size <= 0) return;
    RECT clip{}, iconRect{x, y, x + size, y + size}, visible{};
    if (GetClipBox(dc, &clip) == ERROR || !IntersectRect(&visible, &clip, &iconRect)) return;
    if (!icon && !g_stockAppIcon) {
        SHSTOCKICONINFO stock{}; stock.cbSize=sizeof(stock);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_APPLICATION,SHGSI_ICON|SHGSI_LARGEICON,&stock)))
            g_stockAppIcon=stock.hIcon;
    }
    if (!icon) icon=g_stockAppIcon;
    if (!icon) return;

    if (g_gdiplusToken && g_iconBitmapCache) {
        auto it = g_iconBitmapCache->find(icon);
        if (it == g_iconBitmapCache->end()) {
            auto bitmap = CreateGdiplusBitmapFromIcon(icon);
            if (bitmap) it = g_iconBitmapCache->emplace(icon, std::move(bitmap)).first;
        }
        if (it != g_iconBitmapCache->end()) {
            Gdiplus::Graphics graphics(dc);
            if (graphics.GetLastStatus() == Gdiplus::Ok) {
                graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBilinear);
                graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
                if (graphics.DrawImage(it->second.get(), x, y, size, size) == Gdiplus::Ok)
                    return;
            }
        }
    }
    // Native fallback for monochrome/unsupported icons or GDI+ errors.
    DrawIconEx(dc, x, y, icon, size, size, 0, nullptr, DI_NORMAL);
}

// The byte buffer handed over by the worker is protected by a mutex; the bitmap
// and its stream stay confined to the UI thread. The destruction order keeps the
// stream alive for as long as GDI+ uses the image.
static std::mutex g_profilePhotoMutex;
static std::mutex g_profilePhotoPathMutex;
static std::wstring g_profilePhotoPath;
static std::vector<BYTE> g_profilePhotoPending;
// Account display name resolved by the worker (GetUserNameExW may contact a
// domain controller, so it never runs on the UI thread). Guarded by
// g_profilePhotoMutex; g_accountNamePendingValid marks a fresh value.
static std::wstring g_accountNamePending;
static bool g_accountNamePendingValid = false;
static ComPtr<IStream> g_profilePhotoStream;
// Released explicitly by the UI thread before GdiplusShutdown; never at process exit.
[[clang::no_destroy]] static std::unique_ptr<Gdiplus::Bitmap> g_profilePhoto;
// Bundled copy of the supplied neutral profile-avatar PNG. Keeping its stream alive is
// required for the lifetime of the GDI+ Bitmap created from it.
static const BYTE g_defaultProfileAvatarPng[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x94, 0x00, 0x00, 0x00, 0x94, 0x08, 0x02, 0x00, 0x00, 0x00, 0xFA, 0x5E, 0x97,
    0x83, 0x00, 0x00, 0x10, 0x00, 0x49, 0x44, 0x41, 0x54, 0x78, 0x01, 0xEC, 0xDD, 0x09, 0x88, 0x96,
    0x55, 0xDB, 0x07, 0xF0, 0x98, 0x37, 0xDA, 0x43, 0x5B, 0xB4, 0xA8, 0x28, 0xDA, 0x24, 0x49, 0x24,
    0x4C, 0x22, 0x68, 0xCF, 0xB0, 0x98, 0xA2, 0x85, 0xA4, 0xC8, 0x8A, 0x0A, 0x4A, 0x5A, 0x6C, 0x37,
    0xA9, 0x14, 0x23, 0xD3, 0x16, 0x93, 0x4C, 0x9A, 0x0A, 0x6C, 0x33, 0x0A, 0x25, 0x29, 0x5A, 0xA8,
    0xA1, 0x24, 0xDB, 0xF7, 0xD2, 0x42, 0x02, 0xA5, 0xA0, 0x22, 0xEA, 0x6B, 0x99, 0x16, 0xA5, 0x68,
    0xB1, 0x82, 0xEF, 0x37, 0xDE, 0x76, 0x3C, 0xDE, 0xF3, 0xCC, 0xCC, 0x33, 0xCF, 0xDC, 0xDB, 0xE4,
    0x33, 0x9C, 0xEF, 0xFE, 0xAE, 0x73, 0xDD, 0xE7, 0x5C, 0xD7, 0xFF, 0xFA, 0xFF, 0xCF, 0xB9, 0xB7,
    0xE7, 0x7D, 0x7D, 0x5B, 0x36, 0x19, 0x98, 0x7F, 0x23, 0x47, 0x8E, 0x1C, 0x37, 0x6E, 0xDC, 0x05,
    0x17, 0x5C, 0x70, 0xFD, 0xF5, 0xD7, 0xCF, 0x99, 0x33, 0x67, 0xC1, 0x82, 0x05, 0xCF, 0x3F, 0xFF,
    0xFC, 0x1B, 0x6F, 0xBC, 0xF1, 0xF1, 0xC7, 0x1F, 0xAF, 0x58, 0xB1, 0xE2, 0x8B, 0x2F, 0xBE, 0xF8,
    0xBF, 0xE8, 0x4F, 0x97, 0xD3, 0x29, 0x03, 0x0C, 0x33, 0xD8, 0x14, 0x13, 0x4D, 0x17, 0x44, 0xA8,
    0x81, 0xC9, 0xC1, 0x26, 0x03, 0x49, 0xBC, 0xE1, 0xC3, 0x87, 0x63, 0xFC, 0x89, 0x27, 0x9E, 0xA0,
    0x01, 0x01, 0x66, 0xCF, 0x9E, 0x3D, 0x63, 0xC6, 0x8C, 0xC9, 0x93, 0x27, 0x9F, 0x77, 0xDE, 0x79,
    0x63, 0xC7, 0x8E, 0x1D, 0x3D, 0x7A, 0xF4, 0xBE, 0xFB, 0xEE, 0x3B, 0x64, 0xC8, 0x90, 0x41, 0x83,
    0x06, 0x6D, 0xB6, 0xD9, 0x66, 0xB1, 0x1E, 0xBA, 0x9C, 0x4E, 0x19, 0x60, 0x98, 0xC1, 0xA6, 0x98,
    0x68, 0xBA, 0x20, 0x42, 0x09, 0x28, 0xAC, 0xE0, 0x52, 0xC4, 0x13, 0x2B, 0x6E, 0x57, 0x5D, 0x3C,
    0x3B, 0x03, 0xC5, 0xB6, 0x8B, 0x8D, 0xB4, 0x74, 0xE9, 0x52, 0x8C, 0x1F, 0x71, 0xC4, 0x11, 0x34,
    0xA0, 0x04, 0x49, 0xFE, 0xF9, 0xE7, 0x9F, 0xDF, 0xD6, 0xFE, 0xAD, 0x59, 0xFB, 0xA7, 0xAB, 0x75,
    0xC7, 0xB8, 0x53, 0xDA, 0xDA, 0x81, 0x6B, 0xD6, 0x4E, 0xFA, 0x4D, 0x57, 0x10, 0xA1, 0x04, 0x14,
    0x56, 0x70, 0x29, 0x24, 0x92, 0x4E, 0x52, 0xA9, 0xBB, 0x0B, 0x55, 0x11, 0x7F, 0x45, 0xC5, 0x3B,
    0xF2, 0xC8, 0x23, 0xEF, 0xBF, 0xFF, 0xFE, 0x65, 0xCB, 0x96, 0xD9, 0x19, 0x13, 0x27, 0x4E, 0xB4,
    0x5D, 0xF0, 0xB5, 0x6A, 0xD5, 0x2A, 0xA4, 0x63, 0x1F, 0xE9, 0x1A, 0x4F, 0x26, 0x4D, 0x28, 0x4D,
    0x58, 0xC1, 0xA5, 0x10, 0x53, 0x3A, 0x49, 0xA5, 0x06, 0x00, 0x0C, 0x60, 0x38, 0x2B, 0xD8, 0xAA,
    0x25, 0xDE, 0x09, 0x27, 0x9C, 0x70, 0xCF, 0x3D, 0xF7, 0xB8, 0x39, 0xB5, 0xB7, 0xB7, 0x5B, 0xF8,
    0xBB, 0xEE, 0xBA, 0xAB, 0x9D, 0x81, 0x53, 0xCC, 0x16, 0xC9, 0x9D, 0x74, 0x92, 0x4A, 0x0D, 0x00,
    0x18, 0xC0, 0x80, 0x04, 0x18, 0x78, 0x45, 0xC2, 0xE8, 0x35, 0x57, 0x55, 0xC4, 0x3B, 0xE7, 0x9C,
    0x73, 0xDC, 0x78, 0xDA, 0xDA, 0xDA, 0xC6, 0x8F, 0x1F, 0xEF, 0x3A, 0x66, 0x07, 0xA0, 0xCF, 0x86,
    0xE8, 0xB5, 0x80, 0x5C, 0x07, 0x00, 0x00, 0x06, 0x30, 0x20, 0x01, 0x06, 0x1E, 0x90, 0xA0, 0xE6,
    0x9A, 0xB4, 0xFE, 0xE0, 0x25, 0x8B, 0x77, 0xD0, 0x41, 0x07, 0x79, 0xF0, 0xFB, 0xE4, 0x93, 0x4F,
    0xEE, 0xBD, 0xF7, 0x5E, 0x37, 0x1E, 0x8B, 0x1D, 0x59, 0x28, 0xAB, 0xBF, 0x80, 0x62, 0x46, 0x82,
    0x04, 0x18, 0x78, 0x40, 0x82, 0x0A, 0x30, 0xD8, 0xC0, 0x17, 0x93, 0xBD, 0xBB, 0x2C, 0xA5, 0x89,
    0xE7, 0xB9, 0xCE, 0x85, 0xE8, 0xD1, 0x47, 0x1F, 0x3D, 0xFB, 0xEC, 0xB3, 0x3D, 0x0A, 0x5A, 0xDD,
    0x08, 0xEA, 0x0E, 0x65, 0x75, 0xFC, 0x40, 0x82, 0x0A, 0x30, 0xD8, 0xC0, 0x2B, 0x41, 0x21, 0x65,
    0xC1, 0x2B, 0x41, 0x3C, 0xD5, 0xAA, 0x79, 0xC9, 0x92, 0x25, 0x2E, 0x44, 0xDB, 0x6C, 0xB3, 0x8D,
    0x1B, 0x0C, 0x46, 0xCA, 0xAA, 0xBF, 0xB1, 0xBC, 0x00, 0x83, 0x0D, 0xBC, 0x12, 0x14, 0xA2, 0x1C,
    0x45, 0x35, 0x16, 0xAA, 0x3F, 0xB3, 0x72, 0x14, 0xAF, 0x2B, 0xAC, 0xC1, 0x83, 0x07, 0x7B, 0x97,
    0x7A, 0xFA, 0xE9, 0xA7, 0x4F, 0x3B, 0xED, 0xB4, 0xFF, 0xFD, 0xEF, 0x7F, 0x2E, 0x44, 0x5D, 0xC7,
    0x0C, 0x2C, 0x8F, 0x12, 0x14, 0xA2, 0x1C, 0x45, 0x29, 0x4D, 0x81, 0x45, 0xE2, 0x2F, 0x4E, 0x3C,
    0xF7, 0xF9, 0x97, 0x5E, 0x7A, 0x69, 0xEA, 0xD4, 0xA9, 0xAE, 0x39, 0x96, 0xAD, 0xC5, 0x5B, 0x64,
    0x9D, 0xF9, 0xE5, 0x52, 0x88, 0x72, 0x14, 0xA5, 0x34, 0x05, 0x2A, 0x33, 0xBF, 0x5C, 0xA9, 0xC8,
    0x45, 0x88, 0xB7, 0xE7, 0x9E, 0x7B, 0xFA, 0x8A, 0x31, 0x6B, 0xD6, 0x2C, 0x4F, 0xDE, 0x6E, 0x18,
    0xAA, 0x4D, 0x81, 0xF8, 0x0F, 0x74, 0x15, 0xA5, 0x34, 0x05, 0x2A, 0x53, 0xB1, 0x4A, 0x2E, 0xA0,
    0xA8, 0xDC, 0xC5, 0xBB, 0xE4, 0x92, 0x4B, 0x5E, 0x78, 0xE1, 0x85, 0xD6, 0xD6, 0x56, 0xE5, 0x69,
    0x05, 0x94, 0x54, 0x62, 0x0A, 0x05, 0x6A, 0x8A, 0x55, 0xB2, 0xC2, 0xF3, 0x46, 0x92, 0xA3, 0x78,
    0x56, 0x9F, 0x0F, 0x86, 0xD3, 0xA6, 0x4D, 0x73, 0x49, 0x71, 0x6F, 0xC8, 0xBB, 0x92, 0xEA, 0xC4,
    0x57, 0xAC, 0x92, 0x15, 0xAE, 0x7C, 0x24, 0xE4, 0x07, 0x2C, 0x2F, 0xF1, 0x7C, 0x8C, 0x00, 0x7D,
    0xCC, 0x98, 0x31, 0x56, 0xA2, 0x96, 0x5F, 0x01, 0xD5, 0x8C, 0xAC, 0x64, 0x4D, 0xF9, 0x48, 0x40,
    0x45, 0x4E, 0x20, 0x73, 0x11, 0xCF, 0x73, 0xD7, 0xFC, 0xF9, 0xF3, 0xDD, 0x00, 0xAC, 0xC1, 0x9C,
    0x70, 0x0F, 0x88, 0xB0, 0xCA, 0x47, 0x02, 0x2A, 0x10, 0x92, 0x07, 0xE0, 0xEC, 0xC5, 0xB3, 0xD6,
    0xAE, 0xBA, 0xEA, 0xAA, 0x35, 0x6B, 0xD6, 0x58, 0x7A, 0x79, 0x20, 0x1E, 0x58, 0x31, 0x91, 0x80,
    0x0A, 0x84, 0xA0, 0x25, 0x73, 0xE4, 0x59, 0x8A, 0xE7, 0x57, 0x4D, 0x9F, 0xFE, 0xFC, 0xB6, 0x02,
    0x6E, 0xE6, 0x40, 0x07, 0x74, 0x40, 0x84, 0xA0, 0x05, 0x39, 0x28, 0xCA, 0xB0, 0x90, 0xCC, 0xC4,
    0x73, 0x67, 0x7E, 0xEC, 0xB1, 0xC7, 0xF6, 0xDB, 0x6F, 0x3F, 0x40, 0x33, 0xC4, 0x57, 0x3B, 0xD4,
    0x00, 0xF4, 0xA2, 0x05, 0x39, 0x28, 0x42, 0x54, 0x56, 0xF0, 0xB3, 0x11, 0xCF, 0x9B, 0xA9, 0x87,
    0xE3, 0xED, 0xB7, 0xDF, 0x1E, 0xC4, 0xAC, 0x90, 0xFD, 0xF7, 0xE2, 0x20, 0x07, 0x45, 0x88, 0x42,
    0x57, 0x26, 0xD5, 0x65, 0x20, 0x9E, 0xA7, 0xA9, 0x5B, 0x6E, 0xB9, 0xC5, 0xC3, 0xB1, 0xEB, 0x7B,
    0x26, 0x98, 0xFE, 0xC3, 0x41, 0x50, 0x84, 0x28, 0x74, 0x21, 0xAD, 0xFF, 0x65, 0xF6, 0x57, 0x3C,
    0xAF, 0xA2, 0xF3, 0xE6, 0xCD, 0xF3, 0x7D, 0x0F, 0xAC, 0xFE, 0xA3, 0xD9, 0x18, 0x22, 0x20, 0x0A,
    0x5D, 0x48, 0x43, 0x5D, 0x3F, 0xEB, 0xED, 0x97, 0x78, 0x96, 0xCF, 0x94, 0x29, 0x53, 0x40, 0x01,
    0xA8, 0x9F, 0x38, 0x36, 0xAA, 0xE9, 0xE8, 0x42, 0x1A, 0xEA, 0x10, 0xD8, 0x9F, 0xC2, 0x1B, 0x17,
    0xCF, 0x85, 0xDB, 0xF2, 0x01, 0x02, 0x94, 0xFE, 0x20, 0xD8, 0x38, 0xE7, 0x22, 0x0D, 0x75, 0x08,
    0x44, 0x63, 0xC3, 0x0C, 0x34, 0x28, 0x9E, 0x47, 0x26, 0x0B, 0x47, 0x7A, 0x20, 0x1A, 0xCE, 0xBD,
    0x91, 0x4F, 0x44, 0x1D, 0x02, 0xD1, 0x88, 0xCC, 0xC6, 0xA8, 0x68, 0x44, 0x3C, 0x2F, 0x2B, 0xED,
    0xED, 0xED, 0x6E, 0xBC, 0xD2, 0x37, 0x96, 0xB5, 0x39, 0x2B, 0x61, 0x00, 0x81, 0x68, 0x44, 0x26,
    0x4A, 0x13, 0x4F, 0x9F, 0x8E, 0x8D, 0x88, 0xE7, 0x87, 0xE3, 0x21, 0x43, 0x86, 0x48, 0xDC, 0xA7,
    0x4C, 0xCD, 0xC1, 0x35, 0x19, 0x40, 0x23, 0x32, 0x51, 0x5A, 0xF3, 0x6C, 0xCF, 0xCE, 0x3E, 0x8B,
    0xE7, 0x33, 0x8F, 0x97, 0x4D, 0xAF, 0x2C, 0x3D, 0xC7, 0x1D, 0xB0, 0x67, 0x4B, 0x00, 0x8E, 0x4C,
    0x94, 0x22, 0xB6, 0xAF, 0xB9, 0xFB, 0x26, 0x9E, 0x0F, 0xAC, 0x3E, 0xF3, 0x48, 0xD6, 0xD7, 0x34,
    0xCD, 0xF1, 0x3D, 0x33, 0x80, 0x52, 0xC4, 0xA2, 0xB7, 0xE7, 0x61, 0xA9, 0xB3, 0x7D, 0x10, 0xCF,
    0x73, 0xAD, 0x0F, 0xAC, 0xD2, 0xA4, 0x42, 0x34, 0xBB, 0x99, 0x30, 0x80, 0x58, 0xF4, 0x22, 0xB9,
    0xFE, 0x68, 0xF5, 0x8A, 0xE7, 0x89, 0x68, 0xFA, 0xF4, 0xE9, 0x2E, 0xD0, 0xF5, 0x87, 0x6E, 0x8E,
    0xEC, 0x2B, 0x03, 0xE8, 0x45, 0x32, 0xAA, 0xEB, 0x9C, 0x58, 0xAF, 0x78, 0xB3, 0x67, 0xCF, 0xDE,
    0x6B, 0xAF, 0xBD, 0x44, 0xAF, 0x33, 0x6E, 0x73, 0x58, 0x03, 0x0C, 0xA0, 0x17, 0xC9, 0xA8, 0xAE,
    0x73, 0x6E, 0x5D, 0xE2, 0xF9, 0x90, 0x73, 0xE8, 0xA1, 0x87, 0xFA, 0x69, 0xB1, 0xCE, 0xA0, 0xCD,
    0x61, 0x0D, 0x33, 0x80, 0x64, 0x54, 0x23, 0xBC, 0x9E, 0x08, 0xBD, 0x8B, 0x67, 0x17, 0x5F, 0x7E,
    0xF9, 0xE5, 0xF5, 0xC4, 0x6A, 0x8E, 0xC9, 0x8A, 0x01, 0x84, 0xA3, 0xBD, 0xD7, 0x68, 0xBD, 0x8B,
    0x37, 0x73, 0xE6, 0xCC, 0xA1, 0x43, 0x87, 0xDA, 0xD1, 0xBD, 0xC6, 0x6A, 0x0E, 0xC8, 0x84, 0x01,
    0x54, 0x23, 0x1C, 0xED, 0xBD, 0x46, 0xEB, 0x45, 0x3C, 0x5F, 0xDE, 0xC6, 0x8C, 0x19, 0x63, 0x2F,
    0xF7, 0x1A, 0xA8, 0x39, 0x20, 0x43, 0x06, 0x10, 0x8E, 0x76, 0xE4, 0xF7, 0x1C, 0xB3, 0x27, 0xF1,
    0x06, 0x0F, 0x1E, 0x6C, 0xFF, 0xF6, 0x3C, 0xBF, 0x3A, 0x67, 0x7D, 0x27, 0xDC, 0x6C, 0xB3, 0xCD,
    0x1C, 0x13, 0x48, 0x9E, 0xBC, 0x43, 0x4B, 0x3C, 0x4E, 0xC5, 0x03, 0x12, 0x67, 0x91, 0xC7, 0xBE,
    0xE6, 0x42, 0x3E, 0x09, 0x7A, 0x98, 0xD5, 0x93, 0x78, 0x17, 0x5F, 0x7C, 0xB1, 0x37, 0x7F, 0xBB,
    0xB8, 0x87, 0xF9, 0x55, 0x38, 0x45, 0x92, 0xAD, 0xB6, 0xDA, 0xAA, 0xA3, 0xA3, 0xE3, 0x83, 0x0F,
    0x3E, 0xF0, 0x9D, 0xA2, 0xAD, 0xAD, 0xED, 0xBA, 0xEB, 0xAE, 0x9B, 0x18, 0xFD, 0xE9, 0x72, 0x3A,
    0x65, 0x80, 0x61, 0x06, 0x9B, 0x52, 0x05, 0xE4, 0x3D, 0x60, 0x40, 0x3B, 0xF2, 0x49, 0xD0, 0xC3,
    0x98, 0x6E, 0xC5, 0x1B, 0x3E, 0x7C, 0xF8, 0xB9, 0xE7, 0x9E, 0xFB, 0xCB, 0x2F, 0xBF, 0xF4, 0x30,
    0xB9, 0xDC, 0x53, 0x76, 0x92, 0x66, 0x7B, 0xBD, 0xFA, 0xEA, 0xAB, 0x16, 0x29, 0xB1, 0x6E, 0xBC,
    0xF1, 0xC6, 0xF9, 0xF3, 0xE7, 0xBF, 0xFC, 0xF2, 0xCB, 0x9F, 0x7D, 0xF6, 0xD9, 0xEA, 0xE8, 0x4F,
    0x97, 0xD3, 0x29, 0x03, 0x0C, 0x33, 0xD8, 0x14, 0x13, 0x4D, 0xD7, 0xCA, 0xAD, 0xA2, 0x87, 0xEC,
    0xC8, 0x27, 0x01, 0x21, 0xBA, 0x1B, 0xD3, 0xAD, 0x78, 0x97, 0x5E, 0x7A, 0xE9, 0xF6, 0xDB, 0x6F,
    0x4F, 0xFF, 0xEE, 0x66, 0x96, 0xEB, 0xB7, 0x75, 0x7E, 0xFF, 0xFD, 0x77, 0x9B, 0x89, 0x18, 0x53,
    0xA7, 0x4E, 0x25, 0x0F, 0x3C, 0x9B, 0x6F, 0xBE, 0xB9, 0x5D, 0xE5, 0x48, 0x92, 0x54, 0xE3, 0x4C,
    0x4E, 0x19, 0x66, 0xB0, 0x29, 0x26, 0x9A, 0x2E, 0x88, 0x50, 0x9C, 0x15, 0x6C, 0xC8, 0x27, 0x01,
    0x21, 0xBA, 0xC3, 0x56, 0x5B, 0x3C, 0x6A, 0x9F, 0x7C, 0xF2, 0xC9, 0x26, 0x77, 0x37, 0xAD, 0x44,
    0x3F, 0x55, 0x64, 0x5F, 0xBC, 0x78, 0xB1, 0x55, 0x69, 0x33, 0xD9, 0x60, 0x3B, 0xED, 0xB4, 0x53,
    0xE2, 0xE4, 0xAF, 0xA7, 0x19, 0x6C, 0x8A, 0x89, 0xA6, 0x0B, 0x22, 0x94, 0x59, 0x9C, 0x8E, 0x55,
    0x6B, 0x24, 0x20, 0x04, 0x39, 0x6A, 0x02, 0xAB, 0x2D, 0x1E, 0xB5, 0xAD, 0x53, 0x33, 0x6B, 0xCE,
    0x29, 0xD1, 0x69, 0x97, 0x20, 0xDD, 0x63, 0xF4, 0xAD, 0xB7, 0xDE, 0x0A, 0x1E, 0x90, 0x35, 0x49,
    0x77, 0x2A, 0xD5, 0xBA, 0x62, 0x36, 0xD1, 0x74, 0xC3, 0x84, 0x12, 0x50, 0x58, 0xC1, 0xBB, 0x0E,
    0x2B, 0xD7, 0x03, 0x1E, 0x90, 0xE4, 0xA8, 0x09, 0xA3, 0x86, 0x78, 0x07, 0x1D, 0x74, 0xD0, 0x31,
    0xC7, 0x1C, 0xE3, 0x69, 0xB5, 0xE6, 0x84, 0x12, 0x9D, 0xC8, 0x5D, 0xB9, 0x72, 0xE5, 0x84, 0x09,
    0x13, 0x96, 0x2D, 0x5B, 0xE6, 0x7A, 0x82, 0xFD, 0x14, 0x98, 0x3F, 0xFF, 0xFC, 0xF3, 0xA7, 0x9F,
    0x7E, 0x72, 0xF4, 0x0B, 0xA7, 0xBD, 0x15, 0x9A, 0x2E, 0x67, 0x72, 0x2A, 0x35, 0x45, 0x10, 0xA1,
    0x04, 0x14, 0x56, 0x70, 0x29, 0x52, 0x03, 0x4A, 0xEF, 0x12, 0x82, 0x1C, 0x44, 0xE9, 0x8A, 0xA4,
    0x86, 0x78, 0x67, 0x9C, 0x71, 0x86, 0x7A, 0xBA, 0x0E, 0x2D, 0xD7, 0x63, 0x01, 0x7A, 0x56, 0xF4,
    0xDD, 0xDD, 0x62, 0x74, 0x03, 0x8B, 0xC1, 0xF0, 0xA8, 0xF0, 0xBB, 0xEF, 0xBE, 0x1B, 0x36, 0x6C,
    0xD8, 0xB5, 0xD7, 0x5E, 0x7B, 0xFB, 0xED, 0xB7, 0xDB, 0x4C, 0x37, 0xDF, 0x7C, 0xB3, 0x8F, 0x84,
    0x1A, 0x43, 0x97, 0xD3, 0x29, 0x03, 0x0C, 0x33, 0xD8, 0x94, 0x38, 0x82, 0x80, 0x3C, 0x82, 0x4B,
    0x21, 0x51, 0x7C, 0xAA, 0x0A, 0x36, 0x39, 0x88, 0xD2, 0x15, 0x49, 0x0D, 0xF1, 0x8E, 0x3F, 0xFE,
    0x78, 0x95, 0x74, 0x1D, 0x5A, 0xA2, 0xC7, 0x86, 0x78, 0xE7, 0x9D, 0x77, 0xA6, 0x4D, 0x9B, 0x86,
    0x65, 0x7B, 0x25, 0x46, 0x42, 0x09, 0x1B, 0xEB, 0xC4, 0x13, 0x4F, 0x7C, 0xFA, 0xE9, 0xA7, 0xE7,
    0xCE, 0x9D, 0x3B, 0x76, 0xEC, 0xD8, 0x7D, 0xF7, 0xDD, 0xD7, 0x6F, 0xD3, 0x9C, 0x66, 0x69, 0x0C,
    0x5D, 0x4E, 0xA7, 0x0C, 0x30, 0xCC, 0x60, 0x4E, 0x13, 0xE3, 0x38, 0xC2, 0x0A, 0x2E, 0x85, 0x44,
    0x66, 0xC5, 0xA7, 0x4A, 0xB7, 0xC9, 0x41, 0x94, 0xAE, 0x30, 0xD2, 0xE2, 0x79, 0xAB, 0xDF, 0x6D,
    0xB7, 0xDD, 0x8C, 0xEE, 0x3A, 0xB4, 0x2C, 0x0F, 0x2A, 0xBF, 0xFD, 0xF6, 0x5B, 0xB4, 0x22, 0x17,
    0xC5, 0x31, 0x0C, 0xDB, 0xC8, 0x25, 0xC5, 0xC6, 0xBA, 0xF0, 0xC2, 0x0B, 0x13, 0x3D, 0xBC, 0x00,
    0x00, 0x9F, 0xB4, 0x64, 0x64, 0x62, 0x3B, 0x3A, 0x45, 0x30, 0xC3, 0x0C, 0x36, 0xC5, 0x44, 0xD3,
    0x93, 0x31, 0xC9, 0x51, 0x70, 0x29, 0x24, 0x92, 0x4E, 0xD2, 0xC4, 0x59, 0x85, 0x23, 0xF0, 0x44,
    0x21, 0x4D, 0x0A, 0xCC, 0x7A, 0xF1, 0x92, 0x13, 0x17, 0x5C, 0x70, 0x81, 0xD7, 0x8B, 0xC4, 0xAE,
    0xC2, 0x11, 0xA1, 0xEE, 0x55, 0xAE, 0x78, 0x68, 0x65, 0x07, 0x48, 0xEA, 0x61, 0xCF, 0x99, 0x33,
    0x67, 0xF2, 0xE4, 0xC9, 0x36, 0x16, 0x61, 0x12, 0x0F, 0x67, 0xCF, 0xCD, 0x30, 0x83, 0x4D, 0x31,
    0xD1, 0x74, 0x83, 0x79, 0x1C, 0x93, 0x26, 0x85, 0x44, 0xD2, 0x49, 0xCA, 0x4E, 0x9C, 0x55, 0x38,
    0x12, 0x85, 0x34, 0x29, 0x24, 0x1B, 0x88, 0xE7, 0x67, 0x5C, 0x0A, 0xC7, 0xC5, 0xA4, 0x46, 0x17,
    0xDF, 0x45, 0xE5, 0xA2, 0x45, 0x8B, 0xBE, 0xFF, 0xFE, 0xFB, 0x98, 0xCA, 0x04, 0xE1, 0x8C, 0x19,
    0x33, 0x46, 0x8F, 0x1E, 0x6D, 0x33, 0x25, 0xDD, 0x3E, 0x61, 0x33, 0xC5, 0x44, 0xD3, 0x05, 0x31,
    0x51, 0xD7, 0x31, 0x69, 0x12, 0x49, 0x27, 0xA9, 0xD4, 0x89, 0xA7, 0x0A, 0x47, 0x08, 0x49, 0x43,
    0xA0, 0x18, 0xCC, 0x06, 0xE2, 0xB5, 0xB6, 0xB6, 0xF6, 0xFC, 0x31, 0x2D, 0x9E, 0x59, 0x80, 0xED,
    0xDA, 0xE5, 0x0E, 0xF4, 0xC0, 0x03, 0x0F, 0xC4, 0x0F, 0x11, 0xCA, 0x70, 0xE9, 0x9B, 0x37, 0x6F,
    0x9E, 0xAF, 0x47, 0xF6, 0x50, 0x7F, 0x60, 0x98, 0x2E, 0x88, 0x50, 0x02, 0x0A, 0x1B, 0x42, 0x49,
    0x27, 0xA9, 0xD4, 0x00, 0x04, 0x67, 0xE9, 0x06, 0x69, 0x08, 0x14, 0xC3, 0xD8, 0x40, 0xBC, 0xC3,
    0x0F, 0x3F, 0xDC, 0x23, 0x75, 0x7C, 0xBA, 0x5C, 0x1B, 0xB9, 0x3E, 0x82, 0x58, 0x71, 0x31, 0x0C,
    0x08, 0x5D, 0xD6, 0x5C, 0xF7, 0x9C, 0x8D, 0xFD, 0x8D, 0xD9, 0x82, 0x08, 0x25, 0xA0, 0xB0, 0x71,
    0x04, 0x49, 0xA5, 0x76, 0x36, 0x76, 0x96, 0x6B, 0x43, 0x48, 0xA0, 0x18, 0xC3, 0x7A, 0xF1, 0x8E,
    0x3C, 0xF2, 0x48, 0xBF, 0xC1, 0xC7, 0x0B, 0x30, 0x1E, 0x57, 0xBC, 0xED, 0xF2, 0xF5, 0xF6, 0xDB,
    0x6F, 0xBF, 0xFB, 0xEE, 0xBB, 0x8C, 0x90, 0xDD, 0xAD, 0xE8, 0x8A, 0x2B, 0xAE, 0x18, 0x31, 0x62,
    0x44, 0x86, 0xB4, 0x0A, 0x25, 0xA0, 0xB0, 0x82, 0x87, 0x44, 0x92, 0x4A, 0x0D, 0x00, 0x23, 0x38,
    0xCB, 0x35, 0x48, 0x43, 0x20, 0x32, 0x05, 0x18, 0xEB, 0xC5, 0x3B, 0xEB, 0xAC, 0xB3, 0x94, 0x11,
    0x4E, 0x94, 0x6E, 0xB8, 0xE5, 0x2C, 0x5C, 0xB8, 0x70, 0xEB, 0xAD, 0xB7, 0x0E, 0x48, 0x2C, 0xBD,
    0xC3, 0x0E, 0x3B, 0x0C, 0x7A, 0xB7, 0xAB, 0xE0, 0xCC, 0xC4, 0x10, 0x50, 0x58, 0xC1, 0xA5, 0x08,
    0x01, 0xA5, 0x06, 0x00, 0x8C, 0xE0, 0x29, 0xDD, 0x20, 0x10, 0x99, 0x02, 0x8C, 0xF5, 0xE2, 0x8D,
    0x1A, 0x35, 0x2A, 0x86, 0x1E, 0x46, 0x94, 0x62, 0x58, 0xEF, 0x3E, 0x1F, 0x2F, 0x5D, 0xBA, 0x34,
    0xE6, 0xCE, 0xD2, 0xF3, 0xAE, 0x9A, 0xD3, 0x7D, 0x48, 0x58, 0xC1, 0xA5, 0x08, 0xF5, 0x4A, 0x0D,
    0x00, 0x18, 0xC0, 0x04, 0x67, 0xB9, 0x06, 0x81, 0xC8, 0x14, 0x30, 0xAC, 0x13, 0x6F, 0xDC, 0xB8,
    0x71, 0x5E, 0xE3, 0x63, 0xE8, 0x61, 0x44, 0x29, 0x06, 0xE2, 0xFC, 0x6A, 0xE3, 0x6E, 0x14, 0xB2,
    0xC3, 0xED, 0xFB, 0xAC, 0xEB, 0x9B, 0xD5, 0x17, 0x9C, 0x19, 0x1A, 0xC2, 0x0A, 0x2E, 0x85, 0x44,
    0x21, 0x2C, 0x00, 0x60, 0x00, 0x13, 0x3C, 0xE5, 0x1A, 0x04, 0x22, 0x13, 0xB1, 0x12, 0x18, 0xEB,
    0xC4, 0x3B, 0xE0, 0x80, 0x03, 0x3C, 0xCC, 0x24, 0xAE, 0x2A, 0x1C, 0x57, 0xAD, 0x5A, 0xF5, 0xD1,
    0x47, 0x1F, 0xC5, 0xAC, 0xF1, 0xD8, 0x19, 0x7F, 0xFF, 0xFD, 0x77, 0x7E, 0xF0, 0x04, 0x97, 0x42,
    0xA2, 0x90, 0x02, 0x00, 0x30, 0x62, 0x4F, 0x38, 0x55, 0x96, 0x41, 0x26, 0x62, 0x25, 0xD9, 0xD7,
    0x89, 0xE7, 0x8D, 0x07, 0xF4, 0xC4, 0x95, 0xFD, 0xB1, 0x8F, 0x11, 0x5D, 0xA6, 0x3C, 0x3B, 0xFC,
    0xF8, 0xE3, 0x8F, 0x61, 0x9E, 0x15, 0xB7, 0xF7, 0xDE, 0x7B, 0xBB, 0x5D, 0xDB, 0x1F, 0xC1, 0x99,
    0xB9, 0x21, 0xB8, 0x14, 0x7B, 0xEF, 0xBD, 0xB7, 0x74, 0x21, 0x38, 0x18, 0xC0, 0x80, 0x14, 0x3C,
    0xE5, 0x1A, 0x64, 0x22, 0x56, 0x82, 0x61, 0x9D, 0x78, 0x23, 0x47, 0x8E, 0x04, 0x3D, 0x71, 0x55,
    0xE1, 0xE8, 0x35, 0xF9, 0xCB, 0x2F, 0xBF, 0x0C, 0x94, 0x41, 0x0C, 0xE1, 0x36, 0xDB, 0x6C, 0x93,
    0x37, 0x36, 0x29, 0x24, 0x92, 0x2E, 0x49, 0x04, 0x00, 0x18, 0xC0, 0x24, 0xDD, 0x2A, 0x1C, 0xC9,
    0x04, 0x61, 0x82, 0xA4, 0x53, 0x3C, 0x17, 0x7A, 0x9B, 0x31, 0xE9, 0x57, 0xE1, 0x88, 0x32, 0x8B,
    0x3D, 0x46, 0x62, 0x2B, 0x78, 0xF1, 0xF2, 0x4C, 0x11, 0x3B, 0xF3, 0xB0, 0xA5, 0x90, 0x48, 0xBA,
    0x38, 0x38, 0x30, 0x20, 0xC5, 0x9E, 0x72, 0x6D, 0x62, 0x91, 0x0C, 0x86, 0x4E, 0xF1, 0x4E, 0x39,
    0xE5, 0x14, 0x7A, 0xEA, 0x54, 0xA7, 0xF9, 0x64, 0xBC, 0xC5, 0x16, 0x5B, 0xC4, 0x78, 0xFC, 0x38,
    0x17, 0x77, 0xF3, 0xB3, 0x53, 0x89, 0xC0, 0x00, 0x26, 0xBF, 0x74, 0x0D, 0x44, 0x26, 0x16, 0xC9,
    0x4C, 0xEC, 0x14, 0xEF, 0xC0, 0x03, 0x0F, 0x0C, 0x17, 0x0A, 0xAE, 0xD2, 0xDB, 0xA6, 0x9B, 0x6E,
    0xDA, 0xD1, 0xD1, 0x91, 0x82, 0xE1, 0xAD, 0x2B, 0xE5, 0xC9, 0xA9, 0xDB, 0x35, 0x11, 0x30, 0x20,
    0xE5, 0x94, 0xAE, 0x81, 0xB0, 0xC4, 0x22, 0x99, 0x89, 0x9D, 0xE2, 0x59, 0x6B, 0xA9, 0x0B, 0x85,
    0x13, 0x55, 0x6B, 0xEE, 0x46, 0x05, 0x80, 0x94, 0x42, 0xA2, 0xAA, 0xD5, 0x9E, 0xC2, 0x03, 0x24,
    0xC9, 0x38, 0x5B, 0xDC, 0xFD, 0x5C, 0x43, 0x59, 0xCD, 0x36, 0x80, 0x18, 0x20, 0x19, 0xE1, 0x5A,
    0x86, 0x0D, 0x1B, 0x66, 0xAD, 0x11, 0xB3, 0x52, 0xD0, 0x7D, 0xD7, 0x4F, 0xE1, 0xF9, 0xF5, 0xD7,
    0x5F, 0x0B, 0x78, 0x6A, 0x90, 0x42, 0xA2, 0x54, 0xEA, 0xAE, 0x60, 0x52, 0x03, 0x0A, 0xEE, 0x12,
    0x8B, 0x64, 0x84, 0x6B, 0xD9, 0x6E, 0xBB, 0xED, 0xB6, 0xDC, 0x72, 0xCB, 0x82, 0xD3, 0xF7, 0x9C,
    0xCE, 0x35, 0x7D, 0xE7, 0x9D, 0x77, 0xFE, 0xE3, 0x8F, 0x3F, 0xE2, 0x61, 0x5F, 0x7D, 0xF5, 0x55,
    0xDC, 0xCD, 0xCF, 0x4E, 0x25, 0x02, 0x03, 0x18, 0x90, 0xF2, 0xCB, 0xD8, 0x40, 0x64, 0x92, 0x11,
    0xAE, 0xC5, 0x17, 0x20, 0xDF, 0x11, 0x1A, 0x98, 0x9F, 0xDF, 0x14, 0x2B, 0xCB, 0xF3, 0x7A, 0x2A,
    0xFE, 0xB7, 0xDF, 0x7E, 0xCB, 0x9F, 0x72, 0x6E, 0xB2, 0x49, 0xC6, 0x0E, 0x29, 0x24, 0x4A, 0x05,
    0x05, 0x86, 0x3F, 0xE5, 0x2C, 0xB7, 0x4B, 0x32, 0xC2, 0x75, 0x8A, 0x57, 0x2E, 0x8E, 0x9A, 0xD9,
    0x87, 0x0E, 0x1D, 0xEA, 0x07, 0xD2, 0x70, 0x0A, 0xD6, 0x4F, 0x3F, 0xFD, 0xF4, 0xF7, 0xDF, 0x7F,
    0x0F, 0x9E, 0x9C, 0x0C, 0x29, 0x24, 0x92, 0x2E, 0xC4, 0x07, 0x03, 0x98, 0xD0, 0xAD, 0x8E, 0xD1,
    0x29, 0x9E, 0xE7, 0x96, 0xAA, 0x5D, 0x13, 0x2C, 0x73, 0xD7, 0x74, 0xC0, 0x18, 0x09, 0x59, 0x6E,
    0x45, 0x1F, 0x7E, 0xF8, 0x61, 0xDE, 0x2F, 0xCB, 0xB2, 0x48, 0x21, 0x11, 0x23, 0xC9, 0x0B, 0x00,
    0x18, 0xC0, 0x30, 0x12, 0x4F, 0x45, 0x8E, 0x24, 0x03, 0xAC, 0xC5, 0x57, 0xEA, 0x8A, 0x00, 0x8A,
    0x61, 0x40, 0xE5, 0x86, 0x0C, 0x62, 0x70, 0x7A, 0x59, 0x6E, 0x6F, 0x6F, 0x8F, 0xF7, 0x44, 0x38,
    0x95, 0x95, 0x21, 0xB8, 0x14, 0x12, 0x85, 0x80, 0x00, 0x80, 0x01, 0x4C, 0xF0, 0x54, 0xC7, 0x80,
    0xAA, 0xA5, 0x82, 0xCB, 0x2A, 0x21, 0xC8, 0x4F, 0xFE, 0xF1, 0xE7, 0x7C, 0x8F, 0x7C, 0x2F, 0xBC,
    0xF0, 0x82, 0xCF, 0x8C, 0x61, 0x5B, 0x24, 0xC3, 0xB2, 0x3A, 0x0A, 0x2B, 0xB8, 0x14, 0x12, 0x85,
    0x98, 0x00, 0x80, 0x11, 0xBA, 0xD5, 0x31, 0x5C, 0x09, 0x08, 0xD7, 0xE2, 0x8D, 0xA1, 0x3A, 0x98,
    0x02, 0x12, 0x5F, 0x80, 0x0E, 0x3E, 0xF8, 0xE0, 0x6D, 0xB7, 0xDD, 0x36, 0x78, 0x18, 0x7E, 0x69,
    0x5B, 0xB4, 0x68, 0x11, 0x96, 0xD9, 0x99, 0x37, 0x61, 0x05, 0x97, 0x22, 0x8E, 0x0C, 0x00, 0x18,
    0xC0, 0xC4, 0xCE, 0x8A, 0xD8, 0x84, 0x6B, 0x01, 0x9A, 0x8C, 0x15, 0x01, 0x14, 0xC3, 0x70, 0xC9,
    0xF2, 0xEB, 0x9A, 0x9B, 0x50, 0x70, 0xDA, 0x13, 0xCF, 0x3C, 0xF3, 0x8C, 0xCF, 0xFC, 0x3E, 0x1F,
    0x07, 0x67, 0x26, 0x86, 0x80, 0xC2, 0x0A, 0x2E, 0x45, 0x08, 0x28, 0x35, 0x00, 0x60, 0x04, 0x4F,
    0x75, 0x0C, 0x92, 0x11, 0xAE, 0xC5, 0x1B, 0x43, 0x75, 0x30, 0xC5, 0x48, 0xE0, 0x3B, 0xEE, 0xB8,
    0xE3, 0x76, 0xDF, 0x7D, 0x77, 0x46, 0xF0, 0xBB, 0x2D, 0x4D, 0x9B, 0x36, 0xCD, 0x7B, 0x34, 0xE8,
    0xC1, 0xD9, 0x4F, 0x43, 0x28, 0x01, 0x85, 0x15, 0x3C, 0x84, 0x92, 0x54, 0x6A, 0x00, 0x18, 0xC1,
    0x59, 0x29, 0x83, 0x70, 0x2D, 0x16, 0x5D, 0xA5, 0x30, 0x05, 0x30, 0x58, 0xF3, 0x34, 0xDC, 0xDA,
    0xDA, 0xBA, 0x7A, 0xF5, 0xEA, 0xE0, 0x44, 0xB4, 0x3B, 0xD3, 0xBC, 0xB5, 0xFF, 0x30, 0x72, 0x70,
    0xF6, 0xD3, 0x10, 0x53, 0x40, 0x61, 0x19, 0x21, 0x94, 0xA4, 0x52, 0x03, 0x00, 0x46, 0x70, 0x56,
    0xCA, 0x20, 0x5C, 0xE7, 0x87, 0xE9, 0x4A, 0x61, 0x8A, 0xC1, 0xB8, 0xD9, 0x9C, 0x76, 0xDA, 0x69,
    0x9E, 0xF7, 0xE2, 0x5B, 0x91, 0x2B, 0xDB, 0xB3, 0xCF, 0x3E, 0x7B, 0xC3, 0x0D, 0x37, 0x18, 0x19,
    0xD3, 0xAD, 0xDB, 0xD7, 0x96, 0x4C, 0x17, 0x4A, 0x40, 0x61, 0xC3, 0x74, 0xE9, 0x86, 0x0D, 0x1B,
    0x26, 0x35, 0x00, 0xC1, 0x59, 0x41, 0xA3, 0xD2, 0xE2, 0xE1, 0xCB, 0xC2, 0xBF, 0xE2, 0x8A, 0x2B,
    0x18, 0x71, 0xF3, 0x94, 0xFC, 0xFE, 0xFB, 0xEF, 0xCF, 0x9C, 0x39, 0xD3, 0x59, 0x0B, 0x30, 0x3E,
    0x55, 0xBF, 0x6D, 0xA2, 0xE9, 0x82, 0x08, 0x25, 0x60, 0x6A, 0xA2, 0xA4, 0xCE, 0xA6, 0x9C, 0x55,
    0xEB, 0x0E, 0x00, 0xF1, 0x46, 0x8C, 0x18, 0x31, 0x69, 0xD2, 0xA4, 0xD4, 0x2F, 0xA2, 0x36, 0x0A,
    0xD2, 0x4F, 0x3D, 0xF5, 0xD4, 0x0F, 0x3E, 0xF8, 0xC0, 0x06, 0xA2, 0x44, 0xFD, 0xCC, 0x1A, 0x6C,
    0x8A, 0x89, 0xA6, 0x0B, 0x22, 0x54, 0x3C, 0x57, 0x22, 0xE9, 0x24, 0x1D, 0x00, 0xE2, 0x55, 0xFC,
    0xCA, 0x80, 0xD6, 0xDF, 0x7E, 0xFB, 0xED, 0x88, 0x23, 0x8E, 0x38, 0xF3, 0xCC, 0x33, 0x3D, 0xFE,
    0xE9, 0x86, 0x86, 0x74, 0xBF, 0x91, 0x7A, 0xD0, 0x98, 0x3B, 0x77, 0xAE, 0x67, 0x45, 0xDD, 0x44,
    0x95, 0x30, 0x20, 0x65, 0x10, 0xCC, 0x00, 0xC3, 0x0C, 0x36, 0xC5, 0x44, 0xD3, 0x75, 0xE3, 0x61,
    0x52, 0x48, 0x24, 0x9D, 0xA4, 0xB1, 0xBF, 0x82, 0x36, 0xE1, 0x5A, 0x7C, 0xCD, 0xAB, 0x20, 0xB2,
    0x14, 0x24, 0x40, 0x2F, 0xBC, 0xF0, 0xC2, 0xF3, 0xCE, 0x3B, 0xCF, 0xB6, 0x88, 0x4F, 0xD1, 0x03,
    0xFB, 0x2F, 0xBF, 0xFC, 0xB2, 0x3D, 0x74, 0xF9, 0xE5, 0x97, 0x2F, 0x5E, 0xBC, 0xF8, 0xEB, 0xAF,
    0xBF, 0x36, 0x38, 0x11, 0xC9, 0xA9, 0xA4, 0xE9, 0x72, 0x3A, 0x65, 0x80, 0x61, 0x06, 0x9B, 0xE2,
    0x94, 0xE9, 0x71, 0x34, 0xC1, 0xA5, 0x90, 0xC8, 0xE0, 0xD8, 0x5F, 0x4D, 0x9B, 0x70, 0x2D, 0x2E,
    0x0E, 0xA9, 0x1A, 0xAA, 0x89, 0x15, 0xA1, 0xE3, 0xC7, 0x8F, 0x9F, 0x30, 0x61, 0x82, 0xCD, 0x01,
    0x73, 0x0C, 0xD2, 0x23, 0xFE, 0x1E, 0x7B, 0xEC, 0xF1, 0xC9, 0x27, 0x9F, 0xDC, 0x7A, 0xEB, 0xAD,
    0xF6, 0x93, 0x07, 0x10, 0xAD, 0xAD, 0xAD, 0xED, 0xA1, 0xB5, 0x7F, 0xB3, 0x66, 0xCD, 0xD2, 0xD5,
    0x9C, 0x32, 0xC0, 0x30, 0x83, 0x4D, 0x89, 0x23, 0x08, 0x28, 0xAC, 0xE0, 0x52, 0x48, 0x14, 0x9F,
    0xAA, 0xA6, 0x4D, 0x32, 0x98, 0x5B, 0x7C, 0x01, 0xAA, 0x26, 0xBE, 0xAE, 0xA8, 0xC0, 0xB5, 0x33,
    0x88, 0x01, 0xBA, 0x07, 0xC2, 0xD4, 0x00, 0x7A, 0x78, 0xEE, 0xF0, 0xC6, 0xF6, 0xD9, 0x67, 0x9F,
    0x2D, 0x5F, 0xBE, 0xDC, 0x1B, 0xF7, 0xC2, 0xB5, 0x7F, 0x2F, 0xBE, 0xF8, 0xA2, 0x2E, 0xA7, 0x53,
    0x06, 0x18, 0x96, 0x9A, 0x28, 0x94, 0x80, 0xC2, 0x0A, 0x2E, 0x45, 0xEA, 0x6C, 0x65, 0xBB, 0x84,
    0x6B, 0x51, 0x12, 0xE8, 0x95, 0x85, 0x98, 0x02, 0xE6, 0x56, 0xE4, 0x7B, 0x95, 0xD7, 0xB2, 0x1E,
    0x1E, 0x28, 0x94, 0xA3, 0xB9, 0x2A, 0x86, 0xA6, 0xAB, 0xA5, 0x42, 0x25, 0x5D, 0x6A, 0x09, 0x25,
    0xA0, 0xB0, 0x82, 0x27, 0xCE, 0xEA, 0x1F, 0x95, 0x43, 0xB8, 0x16, 0x97, 0x8B, 0xEA, 0x63, 0x4D,
    0x10, 0x42, 0x4C, 0x0F, 0x7B, 0xE8, 0xB9, 0xE7, 0x9E, 0xFB, 0xE6, 0x9B, 0x6F, 0x74, 0x13, 0x7F,
    0x7F, 0x8E, 0x82, 0x08, 0x25, 0xA0, 0xB0, 0x82, 0xEB, 0xF6, 0x27, 0x5A, 0x91, 0x73, 0x09, 0xD7,
    0xE2, 0x2E, 0xED, 0xA1, 0xAB, 0xC8, 0xAC, 0x8D, 0xE5, 0xF2, 0xD0, 0xE1, 0x16, 0xED, 0x4E, 0x76,
    0xD5, 0x55, 0x57, 0xB9, 0x1C, 0xFA, 0x02, 0xD2, 0x58, 0x9C, 0xAE, 0xB3, 0x84, 0x12, 0x50, 0x58,
    0xC1, 0xA5, 0x90, 0xA8, 0xEB, 0x98, 0x8C, 0x3D, 0xFD, 0x0E, 0x47, 0x32, 0xC2, 0xB5, 0x74, 0x74,
    0xF9, 0x4F, 0x48, 0xF6, 0x3B, 0x72, 0xC6, 0x01, 0xEC, 0x06, 0x84, 0x7A, 0x50, 0x3C, 0xFB, 0xEC,
    0xB3, 0xDD, 0xC9, 0x44, 0xEF, 0x6E, 0x8B, 0xB8, 0x06, 0x6A, 0xAE, 0x7E, 0x56, 0xA5, 0xDA, 0x42,
    0xD3, 0xE5, 0x74, 0x4A, 0x33, 0x3D, 0xD5, 0xC4, 0x17, 0x90, 0x53, 0x70, 0x29, 0x24, 0x92, 0x8E,
    0x93, 0xA7, 0xCA, 0x8D, 0x70, 0x9D, 0xE2, 0xB9, 0x63, 0x57, 0x16, 0x25, 0x1E, 0x51, 0xEF, 0x59,
    0xF1, 0xCE, 0x3B, 0xEF, 0x04, 0x32, 0x61, 0x99, 0x11, 0x37, 0xF8, 0x69, 0xC3, 0x33, 0x68, 0xD0,
    0xA0, 0xFD, 0xF6, 0xDB, 0xEF, 0xC4, 0x13, 0x4F, 0xBC, 0xEC, 0xB2, 0xCB, 0x66, 0xCC, 0x98, 0x31,
    0x67, 0xED, 0x1F, 0x43, 0x97, 0xD3, 0x29, 0x03, 0x0C, 0x33, 0xD8, 0x14, 0x46, 0xAA, 0x25, 0xC1,
    0x25, 0x92, 0x4E, 0x52, 0xA9, 0x53, 0x03, 0xAA, 0xD3, 0x85, 0xBF, 0x53, 0xBC, 0x9F, 0x7F, 0xFE,
    0xD9, 0xB5, 0xA2, 0x3A, 0xB0, 0x62, 0x24, 0xD8, 0x5C, 0xB9, 0xF6, 0xDF, 0xAB, 0x4A, 0xBE, 0x83,
    0xA4, 0x76, 0x83, 0x6D, 0x84, 0x62, 0xED, 0xA8, 0xA3, 0x8E, 0xF2, 0x35, 0xEB, 0x8E, 0x3B, 0xEE,
    0xF0, 0xDC, 0x71, 0xFB, 0xED, 0xB7, 0x4F, 0x9C, 0x38, 0x71, 0xDC, 0xB8, 0x71, 0x5E, 0xB4, 0x47,
    0xAF, 0xFD, 0x63, 0xE8, 0x72, 0x3A, 0x65, 0x80, 0x61, 0x06, 0x9B, 0x62, 0xA2, 0x26, 0x48, 0x9C,
    0x51, 0x0A, 0x49, 0xA5, 0xF3, 0xDA, 0x20, 0x35, 0x3B, 0x3E, 0x5B, 0x1D, 0x9B, 0x64, 0x84, 0x6B,
    0xF1, 0xDE, 0xE3, 0xB9, 0x05, 0xE8, 0xEA, 0x20, 0x4B, 0x90, 0x58, 0xF8, 0xED, 0xED, 0xED, 0xD7,
    0x5C, 0x73, 0x8D, 0x6E, 0x8A, 0x44, 0x8C, 0xDB, 0x3D, 0xB6, 0x91, 0x17, 0xEA, 0xC7, 0x1F, 0x7F,
    0x9C, 0x30, 0x63, 0xC7, 0x8E, 0xDD, 0x75, 0xD7, 0x5D, 0x8D, 0xE4, 0x4F, 0x9A, 0xD7, 0xB5, 0xD0,
    0x12, 0x8F, 0xA3, 0x01, 0x86, 0x19, 0x6C, 0x8A, 0x89, 0xA6, 0x0B, 0xC2, 0x2F, 0xA0, 0x53, 0xA1,
    0x25, 0xE9, 0xA4, 0x06, 0x00, 0x8C, 0xE0, 0xAF, 0x88, 0x41, 0x2C, 0x92, 0x11, 0xAE, 0xC5, 0x3B,
    0x90, 0x37, 0x86, 0x8A, 0xC0, 0x0A, 0x30, 0xD0, 0xE7, 0xDE, 0x33, 0x65, 0xCA, 0x14, 0xAF, 0x65,
    0xB0, 0x06, 0x3F, 0x03, 0xD7, 0x8E, 0x76, 0x8F, 0xE7, 0x0B, 0xEF, 0xD4, 0xD8, 0xD7, 0xA5, 0x53,
    0x4A, 0x00, 0xCE, 0x9A, 0xCD, 0x30, 0x83, 0x9D, 0x32, 0xD1, 0x74, 0x41, 0x84, 0xD2, 0x4D, 0xC2,
    0x32, 0x92, 0x26, 0xA9, 0xD4, 0x00, 0x80, 0x01, 0x4C, 0xE2, 0xAC, 0xCE, 0x91, 0x64, 0x84, 0xEB,
    0xFC, 0x30, 0xED, 0xC6, 0x0E, 0x6B, 0x75, 0x90, 0x21, 0x6B, 0xC1, 0x82, 0x05, 0xEE, 0x3D, 0x3E,
    0x85, 0xC4, 0xA8, 0xF0, 0x8E, 0xE2, 0x63, 0x8E, 0x39, 0xE6, 0x81, 0x07, 0x1E, 0xB0, 0x7B, 0xFC,
    0x1A, 0xA9, 0xCB, 0x19, 0x8F, 0xA9, 0xDF, 0x36, 0xD1, 0x74, 0x41, 0x84, 0x12, 0x50, 0x58, 0x5D,
    0xCE, 0x38, 0x02, 0x00, 0x60, 0x00, 0x03, 0x52, 0xEC, 0x2F, 0xD7, 0x26, 0x16, 0xC9, 0x60, 0xE8,
    0x14, 0x6F, 0xE9, 0xD2, 0xA5, 0x1E, 0x3D, 0x75, 0xAA, 0xD0, 0xD0, 0xB4, 0x64, 0xC9, 0x12, 0xF7,
    0x27, 0x46, 0x8C, 0xC7, 0x2D, 0x5A, 0x9B, 0x3E, 0x7D, 0xBA, 0x67, 0x7A, 0x9B, 0xC6, 0xEE, 0x49,
    0x11, 0x1D, 0x0F, 0xAE, 0xDF, 0x16, 0x44, 0x28, 0x01, 0x85, 0x15, 0x5C, 0x0A, 0x2D, 0x9E, 0x0E,
    0x06, 0x30, 0x20, 0x31, 0x62, 0x7F, 0x89, 0x36, 0xB1, 0x05, 0x5A, 0x7F, 0xB9, 0x00, 0x00, 0x08,
    0xDD, 0x49, 0x44, 0x41, 0x54, 0x48, 0x06, 0x40, 0xA7, 0x78, 0x4F, 0x3E, 0xF9, 0x64, 0x45, 0xAE,
    0xEC, 0x60, 0xBC, 0xF3, 0xCE, 0x3B, 0xB7, 0xDD, 0x76, 0x9B, 0x25, 0x0F, 0x5C, 0x68, 0xF6, 0x84,
    0x7B, 0xD5, 0x83, 0x0F, 0x3E, 0xE8, 0x11, 0x04, 0xD7, 0x18, 0x0F, 0xA7, 0x32, 0x31, 0x04, 0x14,
    0x56, 0x70, 0x29, 0x24, 0x92, 0x2E, 0x0E, 0x0B, 0x0C, 0x48, 0x80, 0x81, 0x17, 0xFB, 0xCB, 0xB2,
    0xC1, 0x20, 0x99, 0xEC, 0x9D, 0xE2, 0xAD, 0x58, 0xB1, 0xC2, 0x35, 0x54, 0xA7, 0xDC, 0xE6, 0x6A,
    0xE0, 0x3E, 0xEC, 0xF3, 0xB1, 0x9B, 0x4D, 0x8C, 0xC4, 0x33, 0xE1, 0xF0, 0xE1, 0xC3, 0x67, 0xCF,
    0x9E, 0x3D, 0x64, 0xC8, 0x10, 0x14, 0xC7, 0xA7, 0xB2, 0xB5, 0x05, 0x97, 0x42, 0x22, 0xE9, 0x24,
    0x8D, 0x83, 0x83, 0x74, 0xE3, 0x8D, 0x37, 0x82, 0x07, 0x64, 0xEC, 0x2F, 0xC5, 0x26, 0x16, 0xC9,
    0xA4, 0xEE, 0x14, 0xCF, 0xFF, 0x5B, 0xBE, 0x7C, 0x39, 0x3D, 0x19, 0x9D, 0xAD, 0xA4, 0xFF, 0xB3,
    0xFC, 0x5D, 0xA0, 0xB0, 0xA3, 0x05, 0x08, 0x36, 0xC1, 0x61, 0x87, 0x1D, 0x86, 0x38, 0x37, 0x27,
    0x03, 0x82, 0x3F, 0x27, 0x43, 0x0A, 0x89, 0xA4, 0x93, 0x54, 0xEA, 0x90, 0x25, 0x81, 0x04, 0x9E,
    0x01, 0xC1, 0x59, 0x8A, 0x41, 0x26, 0x62, 0x25, 0xA9, 0xD7, 0x89, 0xE7, 0x67, 0x65, 0x57, 0xD2,
    0xC4, 0x55, 0xCA, 0xD1, 0x1D, 0xE5, 0x95, 0x57, 0x5E, 0x79, 0xFD, 0xF5, 0xD7, 0xAD, 0xF1, 0x00,
    0xC0, 0xED, 0x67, 0xE8, 0xD0, 0xA1, 0x1E, 0xD9, 0x11, 0x5A, 0x18, 0x6B, 0x12, 0x49, 0x27, 0xA9,
    0xD4, 0x00, 0x04, 0x30, 0x80, 0x81, 0x07, 0x24, 0xA8, 0xC1, 0x59, 0xBC, 0x41, 0x26, 0x62, 0x25,
    0x79, 0xD7, 0x89, 0x57, 0xEE, 0xBF, 0x36, 0x62, 0x5D, 0xBB, 0x14, 0xDC, 0x74, 0xD3, 0x4D, 0x7E,
    0xB2, 0x49, 0x60, 0x39, 0x22, 0x71, 0x87, 0x1D, 0x76, 0xF0, 0x0B, 0x1C, 0x2A, 0xD9, 0x3C, 0x85,
    0x35, 0xE9, 0x24, 0x95, 0x1A, 0x00, 0x76, 0xC8, 0x0B, 0x1E, 0x90, 0xA0, 0x02, 0x1C, 0x9C, 0x05,
    0x1B, 0xB2, 0x13, 0x2B, 0x49, 0xBA, 0x4E, 0x3C, 0x6F, 0xAC, 0xAE, 0xF2, 0x65, 0x61, 0x92, 0xF7,
    0xD1, 0x47, 0x1F, 0xDD, 0x76, 0xC3, 0xFF, 0x7C, 0xF4, 0x0F, 0x3F, 0xFC, 0xE0, 0x55, 0x7A, 0xE7,
    0x9D, 0x77, 0x8E, 0xE9, 0x4B, 0x40, 0x17, 0x70, 0x94, 0x54, 0x6A, 0x00, 0xC0, 0x88, 0xD3, 0x01,
    0x09, 0x2A, 0xC0, 0xB1, 0xB3, 0x30, 0x5B, 0x5E, 0x32, 0x11, 0x2B, 0xC9, 0xB8, 0x4E, 0x3C, 0x9D,
    0x65, 0xCB, 0x96, 0xB9, 0x32, 0x30, 0x0A, 0x6E, 0x00, 0x7D, 0xF9, 0xE5, 0x97, 0x7E, 0x94, 0x89,
    0x2F, 0x47, 0xEE, 0x37, 0xA7, 0x9C, 0x72, 0xCA, 0xE1, 0x87, 0x1F, 0xEE, 0x21, 0xA2, 0x60, 0x3C,
    0x21, 0x9D, 0xD4, 0x00, 0x80, 0x01, 0x4C, 0x70, 0x02, 0x09, 0x2A, 0xC0, 0x60, 0x07, 0x67, 0x61,
    0x06, 0x81, 0xC8, 0x14, 0xD2, 0xAD, 0x17, 0xCF, 0x82, 0x72, 0x33, 0x0C, 0x27, 0x0A, 0x33, 0x00,
    0xF2, 0x15, 0xCA, 0x4A, 0x0F, 0x19, 0xD9, 0x38, 0xF2, 0xF9, 0xE3, 0x97, 0xB2, 0xFF, 0xC7, 0x70,
    0x00, 0x00, 0x03, 0x18, 0x90, 0x62, 0x78, 0x00, 0x83, 0x1D, 0x3C, 0x85, 0x19, 0x04, 0x22, 0x53,
    0x48, 0xB7, 0x5E, 0x3C, 0xB7, 0x62, 0x3F, 0x48, 0x16, 0xBC, 0xA0, 0xA4, 0x73, 0x11, 0x5F, 0xB4,
    0x68, 0x11, 0x82, 0x02, 0x26, 0x3F, 0xB0, 0xF9, 0x2E, 0x5C, 0x85, 0xFF, 0x3E, 0x2A, 0xCD, 0xC0,
    0x00, 0x06, 0xA4, 0x00, 0x0F, 0x54, 0x80, 0xC1, 0x06, 0x3E, 0x38, 0x0B, 0x30, 0xA4, 0x23, 0x10,
    0x99, 0x42, 0xAE, 0xF5, 0xE2, 0x71, 0xBD, 0xF6, 0xDA, 0x6B, 0x05, 0x2F, 0x28, 0x80, 0xDE, 0x7A,
    0xEB, 0xAD, 0xF8, 0xBF, 0x12, 0x07, 0xC6, 0x5F, 0x7F, 0xFD, 0xE5, 0x93, 0x55, 0x7C, 0xB1, 0xE2,
    0x2C, 0xAB, 0x81, 0x01, 0x0C, 0x48, 0x31, 0x00, 0x80, 0xC1, 0x06, 0x3E, 0x76, 0xE6, 0x6D, 0x93,
    0x86, 0x40, 0x71, 0x96, 0x0D, 0xC4, 0x73, 0x35, 0xB0, 0xA0, 0xE2, 0xD3, 0x79, 0xDB, 0xEE, 0x2B,
    0x6F, 0xBE, 0xF9, 0x66, 0xCC, 0x82, 0x1B, 0xF2, 0xD5, 0x57, 0x5F, 0xED, 0x81, 0x38, 0xEF, 0xD4,
    0xF5, 0xC7, 0x07, 0x06, 0x24, 0xC0, 0xC2, 0x14, 0x80, 0xC1, 0x06, 0x3E, 0x78, 0x0A, 0x30, 0x48,
    0x43, 0xA0, 0x38, 0xD1, 0x06, 0xE2, 0x3D, 0xFB, 0xEC, 0xB3, 0x5F, 0x7D, 0xF5, 0x15, 0x64, 0xF1,
    0x88, 0x4C, 0xED, 0x0D, 0x82, 0x49, 0x84, 0x11, 0x3F, 0x9E, 0x59, 0x53, 0xE1, 0x84, 0xC7, 0xF1,
    0x51, 0x55, 0xFA, 0x67, 0x5B, 0x01, 0xF3, 0xB6, 0x07, 0x12, 0x60, 0xEC, 0xA4, 0x01, 0x0C, 0x36,
    0xF0, 0x4A, 0x48, 0x3C, 0x79, 0x1F, 0x25, 0x22, 0x0D, 0x81, 0xE2, 0x44, 0x1B, 0x88, 0xE7, 0xC4,
    0x7D, 0xF7, 0xDD, 0xE7, 0x69, 0x98, 0x51, 0x40, 0x03, 0xC8, 0xC7, 0x02, 0x0F, 0x05, 0x21, 0x97,
    0x7B, 0xCC, 0xFE, 0xFB, 0xEF, 0xEF, 0xEB, 0x22, 0x23, 0x38, 0x4B, 0x37, 0x80, 0x01, 0x09, 0x30,
    0x46, 0x00, 0x03, 0x36, 0xF0, 0x4A, 0x08, 0x9E, 0x5C, 0x0D, 0xA2, 0x90, 0x26, 0x95, 0x22, 0x2D,
    0xDE, 0xC3, 0x0F, 0x3F, 0x4C, 0xE1, 0x62, 0x30, 0xB9, 0x1C, 0x7D, 0xFC, 0xF1, 0xC7, 0x60, 0x05,
    0x4C, 0x9E, 0x0B, 0x0E, 0x39, 0xE4, 0x90, 0x62, 0xB2, 0x87, 0xA4, 0xF5, 0x18, 0x20, 0x01, 0x06,
    0x5E, 0x18, 0x0C, 0x36, 0xF0, 0x4A, 0x08, 0x9E, 0xFC, 0x0C, 0xD9, 0x89, 0x42, 0x9A, 0x54, 0x8A,
    0xB4, 0x78, 0x4E, 0x7B, 0x8F, 0x31, 0x9A, 0x51, 0x40, 0x73, 0xDB, 0xF7, 0xF0, 0x16, 0x12, 0x59,
    0xCE, 0x3E, 0xED, 0xC7, 0x0B, 0x3C, 0x9C, 0x2A, 0xD7, 0x00, 0x09, 0x30, 0xF0, 0x02, 0x0C, 0xB0,
    0x81, 0x0F, 0xDD, 0x5C, 0x0D, 0x72, 0x10, 0xA5, 0x6B, 0x8A, 0x1A, 0xE2, 0x2D, 0x5C, 0xB8, 0xD0,
    0xD5, 0xBC, 0xEB, 0xD0, 0x6C, 0x3D, 0x00, 0x7D, 0xFF, 0xFD, 0xF7, 0x6E, 0x27, 0x71, 0x58, 0xCB,
    0xD9, 0x17, 0x45, 0x4C, 0xC5, 0xCE, 0x2A, 0xD8, 0x20, 0x01, 0x06, 0x5E, 0x0C, 0x06, 0x78, 0x25,
    0x28, 0x24, 0x76, 0xE6, 0x61, 0x93, 0x83, 0x28, 0x5D, 0x23, 0xD7, 0x10, 0xEF, 0xBD, 0xF7, 0xDE,
    0x7B, 0xF1, 0xC5, 0x17, 0xAD, 0xAC, 0xAE, 0xA3, 0xB3, 0xF5, 0xA8, 0x3C, 0x0E, 0x88, 0x0B, 0xDF,
    0xF2, 0x63, 0x4F, 0xD5, 0x6C, 0xF0, 0x80, 0x8C, 0x51, 0xA5, 0x4A, 0x88, 0x4F, 0x65, 0x65, 0x13,
    0x82, 0x1C, 0x44, 0xE9, 0x1A, 0xB0, 0x86, 0x78, 0x06, 0xDD, 0x75, 0xD7, 0x5D, 0xDE, 0x6F, 0x72,
    0x5D, 0x53, 0x82, 0xFB, 0x2D, 0xDF, 0x8A, 0x96, 0x2E, 0x69, 0xEC, 0x7D, 0xF6, 0xD9, 0xA7, 0x9A,
    0xFF, 0x05, 0x7E, 0x08, 0x01, 0x03, 0x0F, 0x48, 0x76, 0xD2, 0xD8, 0x4A, 0x50, 0x48, 0xD2, 0xCD,
    0xE3, 0x28, 0x38, 0x21, 0xC8, 0x51, 0x33, 0x78, 0x6D, 0xF1, 0xFC, 0xD6, 0xF7, 0xD4, 0x53, 0x4F,
    0x99, 0x59, 0x73, 0x4E, 0x56, 0xCE, 0x9F, 0x7F, 0xFE, 0x39, 0x0E, 0xF5, 0xC7, 0x1F, 0x7F, 0xEC,
    0xB4, 0xD3, 0x4E, 0xB1, 0xA7, 0x6A, 0x36, 0x78, 0x40, 0xC6, 0xA8, 0x52, 0x25, 0xC4, 0xA7, 0x32,
    0xB1, 0x49, 0x40, 0x08, 0x72, 0xD4, 0x8C, 0x56, 0x5B, 0x3C, 0x43, 0xA9, 0xED, 0x52, 0x6B, 0x32,
    0x3B, 0xA7, 0x96, 0xBC, 0xE4, 0x5A, 0xBF, 0x49, 0xF3, 0x15, 0xC3, 0xFB, 0x13, 0x3B, 0xA7, 0x74,
    0xFD, 0x0C, 0x0B, 0x18, 0x78, 0x40, 0x32, 0x92, 0x26, 0x60, 0x52, 0x02, 0x23, 0x8F, 0x86, 0x7C,
    0x12, 0x10, 0xA2, 0xBB, 0xE0, 0xDD, 0x8A, 0x47, 0xED, 0xF9, 0xF3, 0xE7, 0xA7, 0x6E, 0xD1, 0xDD,
    0x45, 0x69, 0xD8, 0xEF, 0x82, 0x3E, 0xE8, 0xDF, 0xBF, 0x1D, 0x77, 0xDC, 0xB1, 0xEB, 0x3F, 0x11,
    0x5C, 0x7F, 0xE4, 0x02, 0x46, 0x82, 0x07, 0xE4, 0xBF, 0x78, 0x07, 0x01, 0x9F, 0x6B, 0x52, 0xE4,
    0x93, 0x80, 0x10, 0xDD, 0x65, 0xE9, 0x56, 0x3C, 0x13, 0xEE, 0xB9, 0xE7, 0x9E, 0x95, 0x2B, 0x57,
    0xD2, 0x9F, 0x9D, 0x79, 0xB3, 0x66, 0x5B, 0x5B, 0x5B, 0x81, 0x6B, 0xFB, 0xF7, 0xEF, 0x91, 0x47,
    0x1E, 0x19, 0x39, 0x72, 0xA4, 0x45, 0x9D, 0x79, 0xAE, 0x4C, 0x02, 0x02, 0x06, 0x1E, 0x90, 0xFF,
    0xE2, 0x6D, 0x03, 0x5E, 0x09, 0x0A, 0xC9, 0x24, 0x7E, 0x2A, 0x08, 0xDA, 0x91, 0x4F, 0x82, 0x94,
    0x3F, 0xEE, 0xF6, 0x24, 0x9E, 0x8F, 0x69, 0x73, 0xE7, 0xCE, 0x8D, 0x47, 0x67, 0x6B, 0xFB, 0x81,
    0x23, 0xD5, 0xB2, 0x8D, 0x9F, 0x47, 0xB4, 0x14, 0x60, 0xDD, 0x3C, 0xB2, 0x24, 0x31, 0x91, 0x4F,
    0x82, 0xC4, 0xAE, 0x79, 0xEC, 0x49, 0x3C, 0x13, 0xBC, 0xD5, 0x57, 0xEA, 0x3F, 0xB2, 0x08, 0xD2,
    0xC6, 0xD0, 0x5C, 0x90, 0xD1, 0x8E, 0xFC, 0x9E, 0x8B, 0xED, 0x45, 0x3C, 0x93, 0xA7, 0x4C, 0x99,
    0xE2, 0x6D, 0xC6, 0x2E, 0x66, 0x37, 0x5B, 0x01, 0x0C, 0xA0, 0x1A, 0xE1, 0x68, 0xEF, 0x35, 0x57,
    0xEF, 0xE2, 0x7D, 0xFE, 0xF9, 0xE7, 0xF6, 0x6F, 0xAF, 0x81, 0x9A, 0x03, 0x32, 0x64, 0x00, 0xE1,
    0x68, 0xEF, 0x35, 0x60, 0xEF, 0xE2, 0x09, 0x71, 0xF7, 0xDD, 0x77, 0xBF, 0xF1, 0xC6, 0x1B, 0xF6,
    0x32, 0xBB, 0xD9, 0x72, 0x65, 0x00, 0xC9, 0xA8, 0x46, 0x78, 0x3D, 0x59, 0xEA, 0x12, 0x4F, 0xA0,
    0x49, 0x93, 0x26, 0xF9, 0x0D, 0xDE, 0x8E, 0x66, 0x37, 0x5B, 0x4E, 0x0C, 0xA0, 0x17, 0xC9, 0xA8,
    0xAE, 0x33, 0x7E, 0xBD, 0xE2, 0xD9, 0xC5, 0xD3, 0xA6, 0x4D, 0x13, 0xBD, 0xCE, 0xB8, 0xCD, 0x61,
    0x0D, 0x30, 0x80, 0x5E, 0x24, 0xA3, 0xBA, 0xCE, 0xB9, 0xF5, 0x8A, 0x27, 0x9C, 0x9F, 0x71, 0xEF,
    0xB8, 0xE3, 0x8E, 0x5C, 0x1F, 0x8E, 0x65, 0xA9, 0x62, 0x2B, 0x04, 0x13, 0x62, 0xD1, 0x8B, 0xE4,
    0xFA, 0xB3, 0xF5, 0x41, 0x3C, 0x41, 0x6F, 0xBE, 0xF9, 0xE6, 0x57, 0x5F, 0x7D, 0x55, 0x1A, 0x76,
    0xB3, 0x65, 0xC8, 0x00, 0x4A, 0x11, 0x8B, 0xDE, 0x3E, 0xC5, 0xEC, 0x9B, 0x78, 0x42, 0x9F, 0x7A,
    0xEA, 0xA9, 0xDE, 0xFC, 0x25, 0x63, 0x37, 0x5B, 0x26, 0x0C, 0x20, 0x13, 0xA5, 0x88, 0xED, 0x6B,
    0xB4, 0x3E, 0x8B, 0x27, 0xC1, 0xC5, 0x17, 0x5F, 0xDC, 0xD1, 0xD1, 0xE1, 0x02, 0xCD, 0x6E, 0xB6,
    0x7E, 0x32, 0x80, 0x46, 0x64, 0xA2, 0xB4, 0x81, 0x38, 0x8D, 0x88, 0xB7, 0x7C, 0xF9, 0x72, 0xDF,
    0xF4, 0x56, 0xAF, 0x5E, 0x2D, 0x71, 0x03, 0x29, 0x9B, 0x53, 0x02, 0x03, 0x08, 0x44, 0x23, 0x32,
    0x51, 0x1A, 0x9C, 0xF5, 0x1B, 0x8D, 0x88, 0x27, 0xBA, 0x27, 0xA2, 0x99, 0x6B, 0xFF, 0xA1, 0x59,
    0xE9, 0x75, 0x9B, 0xAD, 0x01, 0x06, 0x50, 0xE7, 0x63, 0x37, 0x1A, 0x91, 0xD9, 0xC0, 0x74, 0x53,
    0x1A, 0x14, 0xCF, 0x4C, 0x5F, 0xDE, 0x26, 0x4C, 0x98, 0x20, 0x3D, 0x10, 0xBA, 0xCD, 0xD6, 0x27,
    0x06, 0x90, 0x86, 0x3A, 0x04, 0xA2, 0xB1, 0x4F, 0x13, 0xE3, 0xC1, 0x8D, 0x8B, 0x27, 0x8A, 0xE7,
    0x5A, 0x0B, 0x07, 0x08, 0x50, 0x74, 0x9B, 0xAD, 0x4E, 0x06, 0xD0, 0x85, 0x34, 0xD4, 0x21, 0xB0,
    0xCE, 0x29, 0x35, 0x87, 0xF5, 0x4B, 0x3C, 0x11, 0x7D, 0xC8, 0xB1, 0x7C, 0x40, 0x01, 0x48, 0xB7,
    0xD9, 0x7A, 0x65, 0x00, 0x51, 0xE8, 0x42, 0x1A, 0xEA, 0x7A, 0x1D, 0xDC, 0xF3, 0x80, 0xFE, 0x8A,
    0x27, 0xBA, 0xE5, 0x73, 0xDD, 0x75, 0xD7, 0xB9, 0xF1, 0x82, 0xA5, 0xDB, 0x6C, 0x3D, 0x30, 0x80,
    0x22, 0x44, 0xA1, 0x0B, 0x69, 0x6B, 0x87, 0xF5, 0xEB, 0x90, 0x81, 0x78, 0xF2, 0xBB, 0x70, 0x1F,
    0x7B, 0xEC, 0xB1, 0x3F, 0xFD, 0xF4, 0x93, 0x57, 0x16, 0xDD, 0x66, 0xAB, 0xC9, 0x00, 0x72, 0x50,
    0x84, 0x28, 0x74, 0xD5, 0x1C, 0xD0, 0x57, 0x67, 0x36, 0xE2, 0xC9, 0xEA, 0x91, 0xE9, 0xF4, 0xD3,
    0x4F, 0xF7, 0xB2, 0x09, 0xA2, 0x6E, 0xB3, 0xA5, 0x18, 0x40, 0x0B, 0x72, 0x50, 0x84, 0xA8, 0xD4,
    0xA9, 0x86, 0xBB, 0x99, 0x89, 0x07, 0x81, 0x97, 0x95, 0x43, 0x0F, 0x3D, 0xD4, 0x67, 0x1E, 0x40,
    0x75, 0x9B, 0x2D, 0x30, 0x80, 0x10, 0xB4, 0x20, 0x07, 0x45, 0xC1, 0xD9, 0x7F, 0x23, 0x4B, 0xF1,
    0x12, 0x34, 0x3E, 0xF3, 0xF8, 0xC0, 0x0A, 0xAE, 0xEB, 0x7B, 0xE2, 0xD9, 0x98, 0x8F, 0x48, 0x40,
    0x05, 0x42, 0xD0, 0x92, 0x39, 0x0F, 0xD9, 0x8B, 0x07, 0xA2, 0x0F, 0xAC, 0xE7, 0x9E, 0x7B, 0xEE,
    0xD7, 0x5F, 0x7F, 0xED, 0xA7, 0x45, 0xDD, 0x8D, 0xB6, 0x29, 0x1F, 0x09, 0xA8, 0x40, 0x48, 0x1E,
    0x24, 0xE4, 0x22, 0x1E, 0xA0, 0x9E, 0xA6, 0xAC, 0xB5, 0x25, 0x4B, 0x96, 0x58, 0x7A, 0x1A, 0xCF,
    0x46, 0xD5, 0x94, 0xAC, 0x29, 0x1F, 0x09, 0xA8, 0xC8, 0xA9, 0xF6, 0xBC, 0xC4, 0x03, 0xD7, 0x9D,
    0x19, 0xF4, 0xE9, 0xD3, 0xA7, 0x7B, 0x38, 0xB6, 0x06, 0x79, 0x36, 0x92, 0xA6, 0x58, 0x25, 0x2B,
    0x5C, 0xF9, 0x48, 0xC8, 0xAF, 0xEA, 0x1C, 0xC5, 0x4B, 0x40, 0x7B, 0x15, 0xF5, 0x70, 0xDC, 0xDE,
    0xDE, 0x6E, 0x25, 0x6A, 0x89, 0xF3, 0xBF, 0x7A, 0x54, 0xA0, 0xA6, 0x58, 0x25, 0x2B, 0x3C, 0xEF,
    0x32, 0x73, 0x17, 0x4F, 0x01, 0x56, 0xDF, 0xF8, 0xF1, 0xE3, 0x27, 0x4F, 0x9E, 0xEC, 0x06, 0x30,
    0x78, 0xF0, 0x60, 0xE5, 0x71, 0xFE, 0xC7, 0x9A, 0xA2, 0x94, 0xA6, 0x40, 0x65, 0x2A, 0x56, 0xC9,
    0x05, 0x14, 0xD8, 0xB3, 0x78, 0x59, 0x02, 0xF0, 0x66, 0x7A, 0xF4, 0xD1, 0x47, 0xCF, 0x98, 0x31,
    0xC3, 0x25, 0xC5, 0x03, 0x98, 0x6A, 0xB3, 0x8C, 0x5E, 0x5E, 0x2C, 0x85, 0x28, 0x47, 0x51, 0x4A,
    0x53, 0xA0, 0x32, 0x0B, 0xC3, 0x52, 0x9C, 0x78, 0x4A, 0x5A, 0xB5, 0x6A, 0x95, 0xE7, 0xAE, 0x93,
    0x4E, 0x3A, 0x69, 0xD1, 0xA2, 0x45, 0xBE, 0xEF, 0xB9, 0x37, 0x70, 0x0E, 0xE8, 0xA6, 0x04, 0x85,
    0x28, 0x47, 0x51, 0x4A, 0x53, 0x60, 0x91, 0xE5, 0x14, 0x2A, 0x5E, 0x52, 0xD8, 0x8A, 0x15, 0x2B,
    0xFC, 0x70, 0x3C, 0x66, 0xCC, 0x98, 0x05, 0x0B, 0x16, 0xFC, 0xFA, 0xEB, 0xAF, 0x96, 0xAD, 0xC5,
    0x9B, 0x9C, 0x1A, 0x28, 0x47, 0x80, 0xC1, 0x06, 0x5E, 0x09, 0x0A, 0x51, 0x8E, 0xA2, 0x8A, 0x07,
    0x5F, 0x82, 0x78, 0x49, 0x91, 0xAA, 0x55, 0xF3, 0x59, 0x67, 0x9D, 0xF5, 0xC8, 0x23, 0x8F, 0xB8,
    0xE6, 0xB8, 0x61, 0x60, 0x24, 0x39, 0x55, 0xE5, 0x23, 0x90, 0xA0, 0x02, 0x0C, 0x36, 0xF0, 0x4A,
    0x50, 0x48, 0x59, 0x80, 0x4B, 0x13, 0x2F, 0x29, 0xF8, 0xBD, 0xF7, 0xDE, 0xBB, 0xF2, 0xCA, 0x2B,
    0x87, 0x0D, 0x1B, 0x76, 0xD1, 0x45, 0x17, 0x7D, 0xFA, 0xE9, 0xA7, 0x6B, 0xD6, 0xAC, 0x71, 0x21,
    0x42, 0x50, 0x72, 0xB6, 0x3A, 0x47, 0x90, 0x00, 0x03, 0x0F, 0x48, 0x50, 0x01, 0x06, 0x1B, 0xF8,
    0x72, 0x11, 0x96, 0x2C, 0x5E, 0x28, 0xDE, 0x7D, 0xDE, 0xA7, 0xBF, 0x89, 0x13, 0x27, 0xBA, 0x10,
    0x75, 0x74, 0x74, 0x58, 0xDD, 0xC8, 0x42, 0x59, 0x18, 0x50, 0x8A, 0x01, 0x00, 0x18, 0xC0, 0x80,
    0x04, 0x18, 0x78, 0x40, 0x82, 0x5A, 0x0A, 0x98, 0xAE, 0x49, 0xAB, 0x22, 0x5E, 0x82, 0xCC, 0xC7,
    0x08, 0x17, 0xA2, 0x11, 0x23, 0x46, 0xB4, 0xB6, 0xB6, 0x3E, 0xFE, 0xF8, 0xE3, 0x9E, 0xBC, 0x2D,
    0x76, 0xF4, 0xB9, 0xC1, 0x24, 0x03, 0x8A, 0x39, 0x4A, 0x27, 0xA9, 0xD4, 0x00, 0x80, 0x01, 0x0C,
    0x48, 0x80, 0x81, 0x57, 0x0C, 0x80, 0x3A, 0xB3, 0x54, 0x4B, 0xBC, 0x00, 0xFA, 0x95, 0x57, 0x5E,
    0x39, 0xFF, 0xFC, 0xF3, 0x47, 0x8D, 0x1A, 0x35, 0x69, 0xD2, 0xA4, 0xB6, 0xB6, 0xB6, 0xE4, 0x5F,
    0x55, 0xB6, 0x03, 0x70, 0x8A, 0x59, 0x1B, 0x42, 0x0B, 0x83, 0xFB, 0x69, 0x08, 0xA5, 0x09, 0x2B,
    0xB8, 0x14, 0xA2, 0x49, 0x27, 0xA9, 0xD4, 0x00, 0x80, 0x01, 0x0C, 0x67, 0x05, 0x5B, 0x45, 0xC5,
    0x0B, 0x4C, 0x59, 0xF8, 0x53, 0xA7, 0x4E, 0x3D, 0xEE, 0xB8, 0xE3, 0x76, 0xD9, 0x65, 0x97, 0x03,
    0x0F, 0x3C, 0x70, 0xD6, 0xAC, 0x59, 0x7E, 0x5B, 0x71, 0xE3, 0x71, 0x1D, 0xB3, 0x33, 0x90, 0x8E,
    0x71, 0x0D, 0xF5, 0x9A, 0xAE, 0x16, 0xE6, 0xA6, 0x0C, 0xA7, 0x34, 0xC3, 0x34, 0x53, 0x34, 0x5D,
    0x41, 0x84, 0x12, 0x50, 0x58, 0xC1, 0xA5, 0x90, 0x48, 0x3A, 0x49, 0xA5, 0x4E, 0x45, 0xA8, 0x5A,
    0xB7, 0x34, 0xF1, 0x1A, 0x20, 0xC2, 0x73, 0x9D, 0x77, 0x29, 0x1F, 0x0C, 0xDD, 0x78, 0x7C, 0xC5,
    0xB0, 0x33, 0x50, 0x8C, 0xF1, 0x87, 0x1E, 0x7A, 0x68, 0xF1, 0xE2, 0xC5, 0xB6, 0x0B, 0x0D, 0x28,
    0xE1, 0x51, 0x90, 0x24, 0x71, 0x7C, 0x5D, 0x4E, 0xA7, 0x0C, 0x30, 0xCC, 0x60, 0x53, 0x4C, 0x34,
    0x5D, 0x10, 0xA1, 0x04, 0x14, 0x56, 0x70, 0x29, 0xE2, 0x89, 0x15, 0xB7, 0xFF, 0x1F, 0x00, 0x00,
    0xFF, 0xFF, 0x46, 0xEA, 0x19, 0x9B, 0x00, 0x00, 0x00, 0x06, 0x49, 0x44, 0x41, 0x54, 0x03, 0x00,
    0x83, 0xBA, 0xC0, 0x62, 0xE9, 0x01, 0xDE, 0x47, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44,
    0xAE, 0x42, 0x60, 0x82,
};
static ComPtr<IStream> g_defaultProfileAvatarStream;
[[clang::no_destroy]] static std::unique_ptr<Gdiplus::Bitmap> g_defaultProfileAvatar;
static UniqueHandle g_profilePhotoThread;
static std::atomic<bool> g_profilePhotoStop{ false };

class ScopedCom {
    bool ok_;
public:
    ScopedCom() : ok_(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {}
    ~ScopedCom() { if (ok_) CoUninitialize(); }
    bool IsOk() const { return ok_; }
};

class ScopedSelectObject {
    HDC hdc_;
    HGDIOBJ old_;
public:
    ScopedSelectObject(HDC hdc, HGDIOBJ obj) : hdc_(hdc), old_(SelectObject(hdc, obj)) {}
    ~ScopedSelectObject() { if (hdc_ && old_ && old_ != HGDI_ERROR) SelectObject(hdc_, old_); }
    bool IsValid() const { return old_ && old_ != HGDI_ERROR; }
    ScopedSelectObject(const ScopedSelectObject&) = delete;
    ScopedSelectObject& operator=(const ScopedSelectObject&) = delete;
};

class ScopedSaveDC {
    HDC dc_ = nullptr;
    int saved_ = 0;
public:
    explicit ScopedSaveDC(HDC dc) : dc_(dc), saved_(dc ? SaveDC(dc) : 0) {}
    ScopedSaveDC(const ScopedSaveDC&) = delete;
    ScopedSaveDC& operator=(const ScopedSaveDC&) = delete;
    ~ScopedSaveDC() { if (dc_ && saved_) RestoreDC(dc_, saved_); }
    explicit operator bool() const { return saved_ != 0; }
};

class ScopedDC {
    HWND hwnd_;
    HDC hdc_;
public:
    explicit ScopedDC(HWND hwnd) : hwnd_(hwnd), hdc_(GetDC(hwnd)) {}
    ~ScopedDC() { if (hwnd_ && hdc_) ReleaseDC(hwnd_, hdc_); }
    HDC get() const { return hdc_; }
    explicit operator bool() const { return hdc_ != nullptr; }
    ScopedDC(const ScopedDC&) = delete;
    ScopedDC& operator=(const ScopedDC&) = delete;
};

class ScopedPaint {
    HWND hwnd_;
    PAINTSTRUCT ps_{};
    HDC hdc_;
public:
    explicit ScopedPaint(HWND hwnd) : hwnd_(hwnd), hdc_(BeginPaint(hwnd, &ps_)) {}
    ~ScopedPaint() { if (hdc_) EndPaint(hwnd_, &ps_); }
    HDC get() const { return hdc_; }
    const RECT& PaintRect() const { return ps_.rcPaint; }
    ScopedPaint(const ScopedPaint&) = delete;
    ScopedPaint& operator=(const ScopedPaint&) = delete;
};

class ScopedTextColor {
    HDC hdc_;
    COLORREF old_;
public:
    ScopedTextColor(HDC hdc, COLORREF col) : hdc_(hdc), old_(SetTextColor(hdc, col)) {}
    ~ScopedTextColor() { if (hdc_) SetTextColor(hdc_, old_); }
    ScopedTextColor(const ScopedTextColor&) = delete;
    ScopedTextColor& operator=(const ScopedTextColor&) = delete;
};

class ScopedBkMode {
    HDC hdc_;
    int old_;
public:
    ScopedBkMode(HDC hdc, int mode) : hdc_(hdc), old_(SetBkMode(hdc, mode)) {}
    ~ScopedBkMode() { if (hdc_) SetBkMode(hdc_, old_); }
    ScopedBkMode(const ScopedBkMode&) = delete;
    ScopedBkMode& operator=(const ScopedBkMode&) = delete;
};


// ===================== SETTINGS =====================
// Windhawk string values are limited only by the backing store; the layout of
// a very large tile grid stays well below this buffer.
constexpr size_t kStorageStringMaxChars = 256 * 1024;

static std::wstring ReadStorageString(const wchar_t* name) {
    std::vector<wchar_t> buffer(kStorageStringMaxChars, L'\0');
    const size_t chars = Wh_GetStringValue(name, buffer.data(), buffer.size());
    if (chars == 0 || chars >= buffer.size()) return {};
    return std::wstring(buffer.data(), chars);
}

struct Settings {
    bool classicAccent = false;
    bool liveTiles = false;
    bool menuAnimation = true;
    bool appTileColors = false;
    int  liveTileIntervalMs = 4000;
    int  widthOverride = 0;
    bool privacyMode = false;
    std::wstring profilePicturePath;
    // Layout to apply to the tile grid. Either the stored layout from mod
    // storage, or a layout pasted in the tileLayout setting that has not been
    // imported yet (see tileLayoutImportPending).
    std::wstring tileLayout;
    bool tileLayoutImportPending = false;

    void Load() {
        using WindhawkUtils::StringSetting;
        classicAccent = _wcsicmp(StringSetting::make(L"accentMode").get(), L"classic") == 0;

        liveTiles = Wh_GetIntSetting(L"liveTileAnimation") != 0;
        menuAnimation = Wh_GetIntSetting(L"menuAnimation") != 0;
        appTileColors = Wh_GetIntSetting(L"appTileColors") != 0;

        int interval = Wh_GetIntSetting(L"liveTileIntervalMs");
        liveTileIntervalMs = (interval >= 1000 && interval <= 60000) ? interval : 4000;

        widthOverride = Wh_GetIntSetting(L"menuWidthOverride");
        if (widthOverride < 0 || widthOverride > 3000) widthOverride = 0;
        privacyMode = Wh_GetIntSetting(L"privacyMode") != 0;

        profilePicturePath = StringSetting::make(L"profilePicturePath").get();
        { std::lock_guard<std::mutex> lock(g_profilePhotoPathMutex); g_profilePhotoPath = profilePicturePath; }

        // The tileLayout setting is a one-time import: it is applied only when its
        // value differs from the last value that was imported, so changing an
        // unrelated option never discards the user's own tile edits.
        const std::wstring importLayout = StringSetting::make(L"tileLayout").get();
        tileLayoutImportPending = false;
        if (!importLayout.empty() && importLayout != ReadStorageString(L"tileLayoutImported")) {
            tileLayout = importLayout;
            tileLayoutImportPending = true;
        } else {
            tileLayout = ReadStorageString(L"startMenuLayout");
        }

        Wh_Log(L"[Settings] Loaded: classicAccent=%d, liveTiles=%d, menuAnimation=%d, appTileColors=%d, privacyMode=%d, interval=%d ms, widthOverride=%d, layoutImport=%d",
               classicAccent ? 1 : 0, liveTiles ? 1 : 0,
               menuAnimation ? 1 : 0, appTileColors ? 1 : 0, privacyMode ? 1 : 0,
               liveTileIntervalMs, widthOverride, tileLayoutImportPending ? 1 : 0);
    }
};

// ===================== DPI AND LAYOUT METRICS =====================
struct Metrics {
    UINT dpi = 96;

    int Scale(int v) const {
        return MulDiv(v, (int)dpi, 96);
    }

    // Base geometry constants (at 96 DPI)
    static constexpr int BASE_LEFT_COL_W        = 224;
    static constexpr int BASE_USER_HEADER_H     = 52;
    static constexpr int BASE_PLACE_ITEM_H      = 38;
    static constexpr int BASE_APP_ITEM_H        = 36;
    static constexpr int BASE_SECTION_HEADER_H  = 26;
    static constexpr int BASE_ICON_SIZE         = 24;
    static constexpr int BASE_PLACE_ICON_SIZE   = 20;

    // Tile grid metrics (96 DPI)
    static constexpr int BASE_TILE_SMALL        = 48;
    static constexpr int BASE_TILE_GAP          = 4;
    static constexpr int BASE_TILE_MEDIUM       = 100;
    static constexpr int BASE_TILE_WIDE         = 204;
    static constexpr int BASE_TILE_LARGE        = 204;
    static constexpr int BASE_GROUP_HEADER_H    = 28;
    static constexpr int BASE_GROUP_GAP         = 16;
    static constexpr int BASE_GROUP_PAD_X       = 12;
    static constexpr int BASE_GROUP_PAD_Y       = 10;
    static constexpr int BASE_COLUMNS_PER_GROUP = 3;

    // Overall menu size (96 DPI)
    // 2 group columns = 2 * (3 * 100 + 2 * 4) + 16 + 24 = 656 px for the tiles
    // Whole menu: 224 (left column) + 656 = 880 px
    static constexpr int BASE_DEFAULT_MENU_W    = 880;
    static constexpr int BASE_DEFAULT_MENU_H    = 620;
    static constexpr int BASE_SCROLLBAR_W       = 8;
};

// The original 1507 layout computes its actual pane size against available
// rectangles before positioning. Keep the custom window inside the monitor's
// work area while preserving its minimum size when the work area permits it.
static RECT FitRectToWorkArea(const RECT& candidate, const RECT& workArea,
                              LONG desiredMinWidth, LONG desiredMinHeight) {
    const LONG workW = (std::max)(1L, workArea.right - workArea.left);
    const LONG workH = (std::max)(1L, workArea.bottom - workArea.top);
    const LONG minW = (std::min)((std::max)(1L, desiredMinWidth), workW);
    const LONG minH = (std::min)((std::max)(1L, desiredMinHeight), workH);
    const int64_t requestedW = static_cast<int64_t>(candidate.right) - candidate.left;
    const int64_t requestedH = static_cast<int64_t>(candidate.bottom) - candidate.top;
    const LONG width = static_cast<LONG>((std::max<int64_t>)(minW,
        (std::min<int64_t>)(requestedW, workW)));
    const LONG height = static_cast<LONG>((std::max<int64_t>)(minH,
        (std::min<int64_t>)(requestedH, workH)));
    const LONG left = (std::max)(workArea.left,
        (std::min)(candidate.left, workArea.right - width));
    const LONG top = (std::max)(workArea.top,
        (std::min)(candidate.top, workArea.bottom - height));
    return RECT{ left, top, left + width, top + height };
}

// ===================== COLOR AND DWM HELPERS =====================
static COLORREF GetSystemAccentColor(bool forceClassic) {
    if (forceClassic) {
        return RGB(0, 120, 215); // Classic Windows 10 1507 default blue
    }
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        return RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    }
    return RGB(0, 120, 215);
}

// ===================== APP AND TILE DATA MODEL =====================
enum class TileSize { Small, Medium, Wide, Large };

struct AppEntry {
    std::wstring name;
    std::wstring path;
    std::wstring aumid;
    std::wstring folder;
    std::wstring target; // raw shortcut target (shortcut entries only), used for identity matching
    FILETIME lastModified{};
    int launchCount = 0;
    HICON hIcon = nullptr; // Reference to the icon in the catalog cache
};

struct FolderGroup {
    std::wstring name;
    bool expanded = false;
    std::vector<AppEntry> apps;
};

struct LetterSection {
    wchar_t letter = 0;
    std::vector<AppEntry> apps;
    std::vector<FolderGroup> folders;
};

struct Tile {
    std::wstring name;
    std::wstring aumid;
    std::wstring path;
    TileSize size = TileSize::Medium;
    int col = 0; // column in tile units (0, 1, 2)
    int row = 0; // row in tile units (0, 1, 2, ...)
    HICON hIcon = nullptr;
    COLORREF appColor = RGB(0, 120, 215); // sampled from the icon once, outside the paint loop
    bool available = true; // runtime state, not serialized: persisted tiles are kept even when stale

    // Live tile flag. Only tiles for which HasLocalLiveContent() is true get a
    // back face (the Calendar tile: weekday + date computed locally); the live*
    // strings are kept for layout (de)serialization compatibility and are not
    // displayed.
    bool isLive = false;
    std::wstring liveTitle;
    std::wstring liveSubtitle;
    std::wstring liveDetail;
    float flipProgress = 0.0f; // 0.0f (fronte) a 1.0f (retro)
    bool flipping = false;
    bool showBack = false;

    // Animation state, never serialized: offset between the position the tile
    // is painted at and its slot, and the eased press/lift scale. Both are
    // driven by TIMER_TILE_ANIM and reset whenever the menu closes.
    float animOffsetX = 0.0f;
    float animOffsetY = 0.0f;
    float animScale = 1.0f;
};

// The only live content a third-party Start menu can legitimately produce is
// the Calendar tile's weekday and date. Microsoft's tile APIs only let an app
// update its own tile, so other apps' live content is not readable and those
// tiles are never flipped.
static bool CanHaveLocalLiveContent(const Tile& tile) {
    std::wstring aumid = tile.aumid;
    for (auto& ch : aumid) ch = static_cast<wchar_t>(towlower(ch));
    return aumid.find(L"windowslive.calendar") != std::wstring::npos ||
           _wcsicmp(tile.name.c_str(), L"Calendario") == 0 ||
           _wcsicmp(tile.name.c_str(), L"Calendar") == 0;
}

static bool HasLocalLiveContent(const Tile& tile) {
    return tile.isLive && CanHaveLocalLiveContent(tile);
}

struct TileGroup {
    std::wstring title;
    std::vector<Tile> tiles;
};

// Resolve a shortcut's own icon resource only when its Shell image-list entry
// is the generic .lnk glyph. This follows the same target/icon-location idea used
// by OpenStartScreen, but uses native documented Shell COM and HICON ownership.
// Loads the shortcut and returns its icon (never the composite .lnk image with
// the arrow overlay). The raw, unresolved target path is returned in
// `targetOut` so callers can identify the app without a slow IShellLink::Resolve.
static HICON GetShortcutIconNoOverlay(LPCWSTR shortcut, std::wstring* targetOut = nullptr) {
    if (!shortcut || !*shortcut) return nullptr;
    ComPtr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&link)))) return nullptr;
    ComPtr<IPersistFile> file;
    if (FAILED(link.As(&file)) || FAILED(file->Load(shortcut,STGM_READ))) return nullptr;
    if (targetOut) {
        wchar_t rawTarget[MAX_PATH]={};
        if (SUCCEEDED(link->GetPath(rawTarget,ARRAYSIZE(rawTarget),nullptr,SLGP_RAWPATH))) *targetOut=rawTarget;
    }
    wchar_t iconPath[MAX_PATH]={}; int iconIndex=0;
    if (SUCCEEDED(link->GetIconLocation(iconPath,ARRAYSIZE(iconPath),&iconIndex))&&iconPath[0]) {
        wchar_t expanded[MAX_PATH]={}; DWORD n=ExpandEnvironmentStringsW(iconPath,expanded,ARRAYSIZE(expanded));
        LPCWSTR path=(n&&n<=ARRAYSIZE(expanded))?expanded:iconPath;
        HICON large=nullptr,small=nullptr;
        if (ExtractIconExW(path,iconIndex,&large,&small,1)>0) {
            if (small) DestroyIcon(small);
            if (large) return large;
        } else { if(large)DestroyIcon(large);if(small)DestroyIcon(small); }
    }
    wchar_t target[MAX_PATH]={}; WIN32_FIND_DATAW fd{};
    if (SUCCEEDED(link->GetPath(target,ARRAYSIZE(target),&fd,SLGP_RAWPATH))&&target[0]) {
        wchar_t expanded[MAX_PATH]={}; DWORD n=ExpandEnvironmentStringsW(target,expanded,ARRAYSIZE(expanded));
        LPCWSTR path=(n&&n<=ARRAYSIZE(expanded))?expanded:target;
        SHFILEINFOW sfi{};
        if (SHGetFileInfoW(path,0,&sfi,sizeof(sfi),SHGFI_ICON|SHGFI_SHELLICONSIZE)&&sfi.hIcon)
            return sfi.hIcon;
    }
    return nullptr;
}
// ===================== APP CATALOG =====================
// Extracts a Shell icon through the item's own icon provider instead of relying
// solely on the SHGetFileInfo cache for virtual AppsFolder items.
static HICON ExtractShellIconFromPidl(PCIDLIST_ABSOLUTE pidl) {
    if (!pidl) return nullptr;
    ComPtr<IShellFolder> parent;
    PCUITEMID_CHILD child = nullptr;
    if (FAILED(SHBindToParent(pidl, IID_PPV_ARGS(&parent), &child)) || !parent || !child)
        return nullptr;

    ComPtr<IExtractIconW> extractor;
    if (FAILED(parent->GetUIObjectOf(nullptr, 1, &child, IID_IExtractIconW, nullptr,
                                    reinterpret_cast<void**>(extractor.GetAddressOf()))) || !extractor)
        return nullptr;

    wchar_t location[MAX_PATH]{};
    int iconIndex = 0;
    UINT flags = 0;
    if (FAILED(extractor->GetIconLocation(GIL_FORSHELL, location, ARRAYSIZE(location),
                                          &iconIndex, &flags))) return nullptr;

    HICON largeRaw = nullptr, smallRaw = nullptr;
    const HRESULT hr = extractor->Extract(location, iconIndex, &largeRaw, &smallRaw,
                                           MAKELONG(48, 48));
    UniqueIcon large(largeRaw), small(smallRaw);
    if (SUCCEEDED(hr) && large) return large.release();
    if (small) return small.release();

    SHFILEINFOW info{};
    if (SHGetFileInfoW(reinterpret_cast<LPCWSTR>(pidl), 0, &info, sizeof(info),
                       SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON | SHGFI_SHELLICONSIZE))
        return info.hIcon;
    return nullptr;
}

static bool GetKnownFolderPathString(REFKNOWNFOLDERID folderId, std::wstring& result) {
    PWSTR rawPath = nullptr;
    if (FAILED(SHGetKnownFolderPath(folderId, KF_FLAG_DEFAULT, nullptr, &rawPath)) || !rawPath)
        return false;
    struct CoTaskMemPathDeleter { void operator()(wchar_t* p) const { CoTaskMemFree(p); } };
    std::unique_ptr<wchar_t, CoTaskMemPathDeleter> path(rawPath);
    result.assign(path.get());
    return !result.empty();
}

class AppCatalog {
public:
    void Refresh() {
        apps_.clear();
        iconCache_.clear();
        std::set<std::wstring> seenPaths;

        // 1. User shortcuts (documented Known Folder API).
        std::wstring userPrograms;
        if (GetKnownFolderPathString(FOLDERID_Programs, userPrograms)) {
            ScanDirectory(userPrograms, L"", seenPaths);
        }

        // 2. Common (all users) shortcuts.
        std::wstring commonPrograms;
        if (GetKnownFolderPathString(FOLDERID_CommonPrograms, commonPrograms)) {
            ScanDirectory(commonPrograms, L"", seenPaths);
        }

        // 3. UWP / Store apps from FOLDERID_AppsFolder
        ScanAppsFolder(seenPaths);

        // Sort alphabetically
        std::sort(apps_.begin(), apps_.end(), [](const AppEntry& a, const AppEntry& b) {
            return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
        });

        ApplyLaunchCounts();
        BuildSections();
        Wh_Log(L"[Catalog] Refreshed: total %u apps catalogued", (unsigned)apps_.size());
    }

    const std::vector<AppEntry>& Apps() const { return apps_; }
    const std::vector<LetterSection>& Sections() const { return sections_; }
    const std::vector<AppEntry>& MostUsed() const { return mostUsed_; }
    const std::vector<AppEntry>& RecentlyAdded() const { return recentlyAdded_; }

    const AppEntry* FindApp(const std::wstring& term) const {
        for (const auto& a : apps_) {
            if (_wcsicmp(a.name.c_str(), term.c_str()) == 0 ||
                (!a.aumid.empty() && _wcsicmp(a.aumid.c_str(), term.c_str()) == 0)) {
                return &a;
            }
        }
        return nullptr;
    }

    // Launch counts are keyed by path (shortcut entries) or AUMID (AppsFolder
    // entries) and persisted in mod storage, so Most used survives Explorer
    // restarts. BuildSections() preserves the expanded state of folders.
    void RecordLaunch(const std::wstring& pathOrAumid) {
        if (pathOrAumid.empty()) return;
        int& count = launchCounts_[pathOrAumid];
        ++count;
        for (auto& a : apps_) {
            if (a.path == pathOrAumid || (!a.aumid.empty() && a.aumid == pathOrAumid)) {
                a.launchCount = count;
                break;
            }
        }
        SaveLaunchCounts();
        BuildSections();
    }

    // Folder headers in All apps are toggled through the catalog so the
    // expanded state can be preserved when the sections are rebuilt.
    void ToggleFolder(size_t sectionIndex, size_t folderIndex) {
        if (sectionIndex >= sections_.size()) return;
        auto& folders = sections_[sectionIndex].folders;
        if (folderIndex >= folders.size()) return;
        FolderGroup& folder = folders[folderIndex];
        folder.expanded = !folder.expanded;
        const std::wstring key = FolderKey(sections_[sectionIndex].letter, folder.name);
        if (folder.expanded) expandedFolders_.insert(key);
        else expandedFolders_.erase(key);
    }

private:
    std::vector<AppEntry> apps_;
    std::vector<LetterSection> sections_;
    std::vector<AppEntry> mostUsed_;
    std::vector<AppEntry> recentlyAdded_;
    std::unordered_map<std::wstring, UniqueIcon> iconCache_;
    std::unordered_map<std::wstring, int> launchCounts_;
    std::set<std::wstring> expandedFolders_;
    bool launchCountsLoaded_ = false;

    // Case-insensitive match of a token against the AUMID (prefix, e.g. the
    // package family name) or against the shortcut's target/file name.
    static bool MatchesIdentityToken(const AppEntry& a, const wchar_t* token) {
        const size_t tokenLen = wcslen(token);
        if (!a.aumid.empty() && _wcsnicmp(a.aumid.c_str(), token, tokenLen) == 0) return true;
        if (a.aumid.empty() && !a.path.empty()) {
            if (StrStrIW(a.path.c_str(), token)) return true;
            if (!a.target.empty() && StrStrIW(a.target.c_str(), token)) return true;
        }
        return false;
    }

    static std::wstring FolderKey(wchar_t letter, const std::wstring& name) {
        std::wstring key(1, letter);
        key += L'|';
        key += name;
        return key;
    }

    // Storage format: "<key>=<count>\n" per entry; keys never contain '\n'
    // and the '=' separator is searched from the end because paths may contain it.
    void LoadLaunchCounts() {
        if (launchCountsLoaded_) return;
        launchCountsLoaded_ = true;
        launchCounts_.clear();
        const std::wstring stored = ReadStorageString(L"startMenuLaunchCounts");
        size_t pos = 0;
        while (pos < stored.size()) {
            size_t lineEnd = stored.find(L'\n', pos);
            if (lineEnd == std::wstring::npos) lineEnd = stored.size();
            const std::wstring line = stored.substr(pos, lineEnd - pos);
            pos = lineEnd + 1;
            const size_t sep = line.rfind(L'=');
            if (sep == std::wstring::npos || sep == 0) continue;
            const int count = _wtoi(line.c_str() + sep + 1);
            if (count > 0) launchCounts_[line.substr(0, sep)] = count;
        }
    }

    void SaveLaunchCounts() {
        // Keep the store bounded: only the 64 most launched entries matter.
        std::vector<std::pair<std::wstring, int>> entries(launchCounts_.begin(), launchCounts_.end());
        std::sort(entries.begin(), entries.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        if (entries.size() > 64) entries.resize(64);
        std::wstring out;
        for (const auto& [key, count] : entries) {
            out += key;
            out += L'=';
            out += std::to_wstring(count);
            out += L'\n';
        }
        if (!Wh_SetStringValue(L"startMenuLaunchCounts", out.c_str()))
            Wh_Log(L"[Catalog] failed to persist launch counts");
    }

    void ApplyLaunchCounts() {
        LoadLaunchCounts();
        for (auto& a : apps_) {
            auto it = launchCounts_.find(a.path);
            if (it == launchCounts_.end() && !a.aumid.empty()) it = launchCounts_.find(a.aumid);
            a.launchCount = it != launchCounts_.end() ? it->second : 0;
        }
    }

    HICON CacheIcon(const std::wstring& key, HICON hIcon) {
        if (!hIcon) return nullptr;
        auto it = iconCache_.find(key);
        if (it != iconCache_.end()) {
            DestroyIcon(hIcon);
            return it->second.get();
        }
        iconCache_[key] = UniqueIcon(hIcon);
        return iconCache_[key].get();
    }

    void ScanDirectory(const std::wstring& rootDir, const std::wstring& subFolder, std::set<std::wstring>& seen) {
        std::wstring searchPattern = rootDir + (subFolder.empty() ? L"" : (L"\\" + subFolder)) + L"\\*";
        WIN32_FIND_DATAW fd{};
        HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &fd);
        if (hFind == INVALID_HANDLE_VALUE) return;
        struct FindCloseGuard { HANDLE h; ~FindCloseGuard() { if (h != INVALID_HANDLE_VALUE) FindClose(h); } } findGuard{hFind};

        do {
            if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;

            std::wstring curRel = subFolder.empty() ? fd.cFileName : (subFolder + L"\\" + fd.cFileName);
            std::wstring fullPath = rootDir + L"\\" + curRel;

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ScanDirectory(rootDir, curRel, seen);
            } else {
                const wchar_t* ext = PathFindExtensionW(fd.cFileName);
                if (ext && _wcsicmp(ext, L".lnk") == 0) {
                    wchar_t baseName[MAX_PATH] = {};
                    wcscpy_s(baseName, fd.cFileName);
                    PathRemoveExtensionW(baseName);

                    std::wstring lowerName = baseName;
                    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::towlower);
                    if (seen.find(lowerName) != seen.end()) continue;
                    seen.insert(lowerName);

                    // Resolve the shortcut's explicit IconLocation or target icon.
                    // Do not ask the Shell for the .lnk's composite image, which
                    // may include the shortcut-arrow overlay.
                    std::wstring target;
                    HICON hIcon = GetShortcutIconNoOverlay(fullPath.c_str(), &target);
                    if (hIcon) hIcon=CacheIcon(fullPath,hIcon);

                    AppEntry entry;
                    entry.name = baseName;
                    entry.path = fullPath;
                    entry.target = std::move(target);
                    entry.folder = subFolder;
                    entry.lastModified = fd.ftLastWriteTime;
                    entry.hIcon = hIcon;
                    apps_.push_back(std::move(entry));
                }
            }
        } while (FindNextFileW(hFind, &fd));
    }

    void ScanAppsFolder(std::set<std::wstring>& seen) {
        PIDLIST_ABSOLUTE rawPidlApps = nullptr;
        if (FAILED(SHGetKnownFolderIDList(FOLDERID_AppsFolder, 0, nullptr, &rawPidlApps)) || !rawPidlApps) return;
        struct PidlScope { PIDLIST_ABSOLUTE p; ~PidlScope(){ if (p) ILFree(p); } } pidlAppsScope{rawPidlApps};
        PIDLIST_ABSOLUTE pidlApps = rawPidlApps;

        ComPtr<IShellFolder> desktop;
        if (SUCCEEDED(SHGetDesktopFolder(&desktop)) && desktop) {
            ComPtr<IShellFolder> appsFolder;
            if (SUCCEEDED(desktop->BindToObject(pidlApps, nullptr, IID_PPV_ARGS(&appsFolder))) && appsFolder) {
                ComPtr<IEnumIDList> enumList;
                if (SUCCEEDED(appsFolder->EnumObjects(nullptr, SHCONTF_NONFOLDERS | SHCONTF_FOLDERS, &enumList)) && enumList) {
                    PITEMID_CHILD child = nullptr;
                    ULONG fetched = 0;
                    while (enumList->Next(1, &child, &fetched) == S_OK && child) {
                        struct ChildPidlScope { PITEMID_CHILD p; ~ChildPidlScope(){ if (p) CoTaskMemFree(p); } } childScope{child};
                        wchar_t nameBuf[MAX_PATH] = {};
                        STRRET strret{};
                        if (SUCCEEDED(appsFolder->GetDisplayNameOf(child, SHGDN_NORMAL, &strret))) {
                            StrRetToBufW(&strret, child, nameBuf, ARRAYSIZE(nameBuf));
                        }

                        wchar_t aumidBuf[512] = {};
                        strret = {};
                        if (SUCCEEDED(appsFolder->GetDisplayNameOf(child, SHGDN_FORPARSING, &strret))) {
                            StrRetToBufW(&strret, child, aumidBuf, ARRAYSIZE(aumidBuf));
                        }

                        if (nameBuf[0] && aumidBuf[0]) {
                            std::wstring lower = nameBuf;
                            std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
                            if (seen.find(lower) == seen.end()) {
                                seen.insert(lower);
                                std::wstring shellPath = std::wstring(L"shell:AppsFolder\\") + aumidBuf;

                                HICON hIcon = nullptr;
                                PIDLIST_ABSOLUTE parsedPidl = nullptr;
                                if (SUCCEEDED(SHParseDisplayName(shellPath.c_str(), nullptr, &parsedPidl, 0, nullptr)) && parsedPidl) {
                                    // SHParseDisplayName returns COM memory; it is released on
                                    // every path, including intermediate errors and early exits.
                                    struct PidlScope { PIDLIST_ABSOLUTE p; ~PidlScope(){ if (p) CoTaskMemFree(p); } } pidlScope{parsedPidl};
                                    hIcon = CacheIcon(shellPath, ExtractShellIconFromPidl(parsedPidl));
                                }

                                AppEntry entry;
                                entry.name = nameBuf;
                                entry.aumid = aumidBuf;
                                entry.path = shellPath;
                                entry.hIcon = hIcon;
                                apps_.push_back(std::move(entry));
                            }
                        }
                    }
                }
            }
        }
    }

    void BuildSections() {
        sections_.clear();
        std::map<wchar_t, std::vector<AppEntry>> groups;
        std::map<wchar_t, std::map<std::wstring, std::vector<AppEntry>>> folderGroups;

        for (const auto& a : apps_) {
            wchar_t initial = L'#';
            if (!a.name.empty()) {
                wchar_t ch = (wchar_t)::towupper(a.name[0]);
                if (ch >= L'A' && ch <= L'Z') initial = ch;
                else if (a.name[0] >= L'0' && a.name[0] <= L'9') initial = L'0';
                else if (a.name[0] > 0x7f) {
                    WORD charType = 0;
                    if (GetStringTypeW(CT_CTYPE1, a.name.data(), 1, &charType) && (charType & C1_ALPHA)) initial = L'~';
                    else initial = L'#';
                }
            }
            if (a.folder.empty()) {
                groups[initial].push_back(a);
            } else {
                folderGroups[initial][a.folder].push_back(a);
            }
        }

        // Add 0-9 only when a real app/folder starts with a digit, then A-Z;
        // punctuation stays in a separate # bucket and never creates a fake numeric section.
        std::vector<wchar_t> sectionOrder{ L'0' };
        for (wchar_t ch = L'A'; ch <= L'Z'; ++ch) sectionOrder.push_back(ch);
        sectionOrder.push_back(L'#');
        sectionOrder.push_back(L'~');
        for (wchar_t ch : sectionOrder) {
            auto appsIt = groups.find(ch);
            auto foldersIt = folderGroups.find(ch);
            bool hasApps = appsIt != groups.end() && !appsIt->second.empty();
            bool hasFolders = foldersIt != folderGroups.end() && !foldersIt->second.empty();
            if (!hasApps && !hasFolders) continue;
            LetterSection sec;
            sec.letter = ch;
            if (hasApps) sec.apps = appsIt->second;
            if (hasFolders) {
                for (auto& fg : foldersIt->second) {
                    FolderGroup group;
                    group.name = fg.first;
                    group.apps = fg.second;
                    group.expanded = expandedFolders_.count(FolderKey(ch, group.name)) != 0;
                    sec.folders.push_back(std::move(group));
                }
            }
            sections_.push_back(std::move(sec));
        }

        // Most used: top 6 by launch count.
        std::vector<AppEntry> sorted = apps_;
        std::sort(sorted.begin(), sorted.end(), [](const AppEntry& a, const AppEntry& b) {
            return a.launchCount > b.launchCount;
        });
        mostUsed_.clear();
        for (size_t i = 0; i < sorted.size() && i < 6; ++i) {
            if (sorted[i].launchCount <= 0) break;
            mostUsed_.push_back(sorted[i]);
        }

        // Until enough launches were recorded, fill Most used with well-known
        // system apps, matched by AUMID / executable name so the list does not
        // depend on the display language.
        if (mostUsed_.size() < 6) {
            struct DefaultApp { const wchar_t* tokens[4]; };
            static const DefaultApp defaults[] = {
                {{L"MSEdge", L"Microsoft.MicrosoftEdge", L"msedge.exe", nullptr}},
                {{L"Microsoft.Windows.Explorer", L"explorer.exe", nullptr, nullptr}},
                {{L"windows.immersivecontrolpanel", nullptr, nullptr, nullptr}},
                {{L"Microsoft.WindowsCalculator", L"calc.exe", nullptr, nullptr}},
                {{L"Microsoft.Windows.Photos", nullptr, nullptr, nullptr}},
                {{L"Microsoft.WindowsNotepad", L"Microsoft.Notepad", L"notepad.exe", nullptr}},
            };
            auto alreadyListed = [this](const AppEntry& a) {
                for (const auto& m : mostUsed_)
                    if (m.path == a.path && m.aumid == a.aumid) return true;
                return false;
            };
            for (const auto& def : defaults) {
                if (mostUsed_.size() >= 6) break;
                const AppEntry* match = nullptr;
                for (const wchar_t* token : def.tokens) {
                    if (!token) break;
                    for (const auto& a : apps_) {
                        if (MatchesIdentityToken(a, token)) { match = &a; break; }
                    }
                    if (match) break;
                }
                if (match && !alreadyListed(*match)) mostUsed_.push_back(*match);
            }
        }

        // Recently added: most recently modified entries.
        std::sort(sorted.begin(), sorted.end(), [](const AppEntry& a, const AppEntry& b) {
            return CompareFileTime(&a.lastModified, &b.lastModified) > 0;
        });
        recentlyAdded_.clear();
        for (size_t i = 0; i < sorted.size() && i < 3; ++i) {
            recentlyAdded_.push_back(sorted[i]);
        }
    }
};

// Validates classic tile targets. The presence of the .lnk is not enough: the
// shortcut is resolved silently and its target checked; namespace shortcuts are
// accepted when they carry a PIDL.
static bool IsUsableTileFileTarget(const std::wstring& path) {
    if (path.empty()) return false;
    const DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) return false;
    const wchar_t* extension = PathFindExtensionW(path.c_str());
    if (!extension || _wcsicmp(extension, L".lnk") != 0) return true;

    ScopedCom com;
    if (!com.IsOk()) return false;
    ComPtr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link))) || !link) return false;
    ComPtr<IPersistFile> persist;
    if (FAILED(link.As(&persist)) || !persist || FAILED(persist->Load(path.c_str(), STGM_READ))) return false;
    const DWORD resolveFlags = SLR_NO_UI | SLR_NOUPDATE | SLR_NOSEARCH | SLR_NOTRACK | (400u << 16);
    if (FAILED(link->Resolve(nullptr, resolveFlags))) return false;

    wchar_t target[32768]{};
    WIN32_FIND_DATAW findData{};
    if (SUCCEEDED(link->GetPath(target, ARRAYSIZE(target), &findData, SLGP_RAWPATH)) && target[0]) {
        wchar_t expanded[32768]{};
        const DWORD n = ExpandEnvironmentStringsW(target, expanded, ARRAYSIZE(expanded));
        const wchar_t* resolved = (n && n <= ARRAYSIZE(expanded)) ? expanded : target;
        return GetFileAttributesW(resolved) != INVALID_FILE_ATTRIBUTES;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    const HRESULT pidlHr = link->GetIDList(&pidl);
    struct PidlCleanup { PIDLIST_ABSOLUTE p; ~PidlCleanup(){ if (p) CoTaskMemFree(p); } } cleanup{pidl};
    return SUCCEEDED(pidlHr) && pidl != nullptr;
}

static bool IsUsableCatalogApp(const AppEntry& app) {
    // AppsFolder entries are enumerated at refresh time and carry an AUMID;
    // shortcut entries must also pass the target check above.
    if (!app.aumid.empty()) return true;
    return IsUsableTileFileTarget(app.path);
}

// ===================== TILE LAYOUT =====================
class TileLayout {
public:
    std::vector<TileGroup> groups;

    void LoadDefault(const AppCatalog& catalog) {
        groups.clear();

        // Group 1: "Apps" (Life at a glance)
        TileGroup g1;
        g1.title = L"Applicazioni";

        Tile tMail;
        tMail.name = L"Posta";
        tMail.size = TileSize::Medium;
        tMail.col = 0; tMail.row = 0;

        Tile tCal;
        tCal.name = L"Calendario";
        tCal.size = TileSize::Wide;
        tCal.col = 1; tCal.row = 0;
        tCal.isLive = true;

        Tile tEdge;
        tEdge.name = L"Microsoft Edge";
        tEdge.size = TileSize::Medium;
        tEdge.col = 0; tEdge.row = 1;

        Tile tPhotos;
        tPhotos.name = L"Foto";
        tPhotos.size = TileSize::Medium;
        tPhotos.col = 1; tPhotos.row = 1;

        Tile tWeather;
        tWeather.name = L"Meteo";
        tWeather.size = TileSize::Wide;
        tWeather.col = 1; tWeather.row = 2;

        // A default tile is added only when the app exists in the installed
        // catalog (AppsFolder or Start shortcuts): no fake or unlaunchable tiles.
        if (AttachAppDetails(tMail, catalog)) g1.tiles.push_back(tMail);
        if (AttachAppDetails(tCal, catalog)) g1.tiles.push_back(tCal);
        if (AttachAppDetails(tEdge, catalog)) g1.tiles.push_back(tEdge);
        if (AttachAppDetails(tPhotos, catalog)) g1.tiles.push_back(tPhotos);
        if (AttachAppDetails(tWeather, catalog)) g1.tiles.push_back(tWeather);

        // Group 2: "Play and explore"
        TileGroup g2;
        g2.title = L"Tempo libero";

        Tile tXbox; tXbox.name = L"Xbox"; tXbox.size = TileSize::Medium; tXbox.col = 0; tXbox.row = 0;
        Tile tMusic; tMusic.name = L"Groove Musica"; tMusic.size = TileSize::Medium; tMusic.col = 1; tMusic.row = 0;
        Tile tVideo; tVideo.name = L"Film e TV"; tVideo.size = TileSize::Medium; tVideo.col = 2; tVideo.row = 0;
        Tile tSol; tSol.name = L"Solitaire"; tSol.size = TileSize::Medium; tSol.col = 1; tSol.row = 1;
        Tile tCalc; tCalc.name = L"Calcolatrice"; tCalc.size = TileSize::Medium; tCalc.col = 2; tCalc.row = 1;

        if (AttachAppDetails(tXbox, catalog)) g2.tiles.push_back(tXbox);
        if (AttachAppDetails(tMusic, catalog)) g2.tiles.push_back(tMusic);
        if (AttachAppDetails(tVideo, catalog)) g2.tiles.push_back(tVideo);
        if (AttachAppDetails(tSol, catalog)) g2.tiles.push_back(tSol);
        if (AttachAppDetails(tCalc, catalog)) g2.tiles.push_back(tCalc);

        if (!g1.tiles.empty()) groups.push_back(std::move(g1));
        if (!g2.tiles.empty()) groups.push_back(std::move(g2));
    }

    bool AttachAppDetails(Tile& t, const AppCatalog& cat) {
        // Prefer exact name/AUMID matches, then known localized/legacy names and
        // package identity tokens. Tile captions are localized independently of AppsFolder.
        const AppEntry* app = cat.FindApp(t.name);
        if (!app && !t.aumid.empty()) app = cat.FindApp(t.aumid);
        if (!app) {
            struct AliasSet { const wchar_t* tile; const wchar_t* aliases[8]; };
            static const AliasSet sets[] = {
                {L"Posta", {L"Mail", L"Outlook", L"Microsoft.OutlookForWindows", nullptr}},
                {L"Calendario", {L"Calendar", L"Outlook Calendar", L"WindowsCommunicationsApps", nullptr}},
                {L"Microsoft Edge", {L"Edge", L"MSEdge", L"MicrosoftEdge", nullptr}},
                {L"Foto", {L"Photos", L"Microsoft.Windows.Photos", L"Microsoft Photos", nullptr}},
                {L"Meteo", {L"Weather", L"MSN Weather", L"BingWeather", nullptr}},
                {L"Xbox", {L"Xbox", L"XboxApp", L"Gaming", nullptr}},
                {L"Groove Musica", {L"Groove Music", L"ZuneMusic", L"Media Player", nullptr}},
                {L"Film e TV", {L"Movies & TV", L"Movies & Television", L"ZuneVideo", nullptr}},
                {L"Store", {L"Microsoft Store", L"WindowsStore", L"WinStore", nullptr}},
                {L"Solitaire", {L"Microsoft Solitaire Collection", L"SolitaireCollection", nullptr}},
                {L"Calcolatrice", {L"Calculator", L"WindowsCalculator", nullptr}},
            };
            const AliasSet* aliasSet = nullptr;
            for (const auto& set : sets) if (_wcsicmp(set.tile, t.name.c_str()) == 0) { aliasSet = &set; break; }
            if (aliasSet) {
                // First perform case-insensitive exact aliases; then package-token matching.
                for (const wchar_t* alias : aliasSet->aliases) {
                    if (!alias) break;
                    if ((app = cat.FindApp(alias))) break;
                    std::wstring needle(alias);
                    std::transform(needle.begin(), needle.end(), needle.begin(), ::towlower);
                    for (const auto& candidate : cat.Apps()) {
                        std::wstring identity = candidate.name + L" " + candidate.aumid + L" " + candidate.path;
                        std::transform(identity.begin(), identity.end(), identity.begin(), ::towlower);
                        if (identity.find(needle) != std::wstring::npos) { app = &candidate; break; }
                    }
                    if (app) break;
                }
            }
        }
        if (app && !IsUsableCatalogApp(*app)) app = nullptr;
        if (app) {
            t.path = app->path;
            t.aumid = app->aumid;
            t.hIcon = app->hIcon;
            t.appColor = GetIconAccentColor(t.hIcon);
            t.available = true;
            return true;
        }
        // Keep saved custom tiles rather than deleting them after an app refresh.
        // They are rendered with a fallback icon and are not launched until a
        // later refresh resolves the AUMID/path again.
        t.hIcon = nullptr; // the old HICON may belong to the catalog being replaced
        t.available = IsUsableTileFileTarget(t.path);
        return t.available;
    }

    // Refresh launchability/icons without deleting persisted groups or tiles.
    // Missing targets remain visible and editable, but cannot be launched until
    // a later catalog refresh resolves them again.
    void RefreshInstalledApps(const AppCatalog& cat) {
        for (auto& group : groups) {
            for (auto& tile : group.tiles)
                AttachAppDetails(tile, cat);
        }
    }

    static void AppendNumber(std::wstring& out, size_t value, wchar_t terminator) {
        out += std::to_wstring(value);
        out.push_back(terminator);
    }

    static void AppendField(std::wstring& out, const std::wstring& value) {
        AppendNumber(out, value.size(), L':');
        out += value;
    }

    static bool ReadNumber(const std::wstring& input, size_t& pos, wchar_t terminator,
                           size_t maximum, size_t& value) {
        if (pos >= input.size()) return false;
        size_t parsed = 0;
        bool haveDigit = false;
        while (pos < input.size() && input[pos] >= L'0' && input[pos] <= L'9') {
            const size_t digit = static_cast<size_t>(input[pos] - L'0');
            if (digit > maximum || parsed > (maximum - digit) / 10) return false;
            parsed = parsed * 10 + digit;
            ++pos;
            haveDigit = true;
        }
        if (!haveDigit || pos >= input.size() || input[pos] != terminator) return false;
        ++pos;
        value = parsed;
        return true;
    }

    static bool ReadField(const std::wstring& input, size_t& pos, std::wstring& value) {
        size_t length = 0;
        if (!ReadNumber(input, pos, L':', 32768, length) || length > input.size() - pos)
            return false;
        value.assign(input, pos, length);
        pos += length;
        return true;
    }

    static bool ParseBoundedInt(const std::wstring& text, int maximum, int& value) {
        if (text.empty()) return false;
        size_t parsed = 0;
        for (wchar_t ch : text) {
            if (ch < L'0' || ch > L'9') return false;
            const size_t digit = static_cast<size_t>(ch - L'0');
            if (digit > static_cast<size_t>(maximum) ||
                parsed > (static_cast<size_t>(maximum) - digit) / 10) return false;
            parsed = parsed * 10 + digit;
        }
        value = static_cast<int>(parsed);
        return true;
    }

    bool ParseV2(const std::wstring& str, const AppCatalog& cat) {
        static constexpr wchar_t kHeader[] = L"WHSM2#";
        constexpr size_t kHeaderLength = ARRAYSIZE(kHeader) - 1;
        if (str.compare(0, kHeaderLength, kHeader) != 0 || str.size() > 16 * 1024 * 1024)
            return false;

        size_t pos = kHeaderLength;
        size_t groupCount = 0;
        if (!ReadNumber(str, pos, L'#', 128, groupCount)) return false;
        std::vector<TileGroup> newGroups;
        newGroups.reserve(groupCount);
        size_t totalTiles = 0;

        for (size_t gi = 0; gi < groupCount; ++gi) {
            TileGroup group;
            if (!ReadField(str, pos, group.title)) return false;
            size_t tileCount = 0;
            if (!ReadNumber(str, pos, L'#', 4096, tileCount) ||
                tileCount > 8192 - totalTiles) return false;
            totalTiles += tileCount;
            group.tiles.reserve(tileCount);

            for (size_t ti = 0; ti < tileCount; ++ti) {
                std::wstring fields[10];
                for (auto& field : fields) {
                    if (!ReadField(str, pos, field)) return false;
                }

                int size = 0, col = 0, row = 0, live = 0;
                if (!ParseBoundedInt(fields[3], 3, size) ||
                    !ParseBoundedInt(fields[4], Metrics::BASE_COLUMNS_PER_GROUP - 1, col) ||
                    !ParseBoundedInt(fields[5], 100000, row) ||
                    !ParseBoundedInt(fields[6], 1, live)) return false;

                Tile tile;
                tile.name = std::move(fields[0]);
                tile.aumid = std::move(fields[1]);
                tile.path = std::move(fields[2]);
                tile.size = static_cast<TileSize>(size);
                tile.col = col;
                tile.row = row;
                tile.isLive = live != 0;
                tile.liveTitle = std::move(fields[7]);
                tile.liveSubtitle = std::move(fields[8]);
                tile.liveDetail = std::move(fields[9]);
                tile.available = AttachAppDetails(tile, cat);

                // Retain saved custom tiles even if their app is temporarily
                // absent. They remain removable/editable and are not launched
                // until a later catalog refresh resolves their target.
                if (!tile.name.empty() || !tile.path.empty() || !tile.aumid.empty())
                    group.tiles.push_back(std::move(tile));
            }
            newGroups.push_back(std::move(group));
        }
        if (pos != str.size()) return false;
        groups = std::move(newGroups); // an explicitly empty v2 layout is valid
        return true;
    }

    bool ParseLegacy(const std::wstring& str, const AppCatalog& cat) {
        try {
            std::vector<TileGroup> newGroups;
            size_t start = 0;
            while (start < str.size()) {
                size_t endGroup = str.find(L"||", start);
                if (endGroup == std::wstring::npos) endGroup = str.size();
                const std::wstring groupChunk = str.substr(start, endGroup - start);
                start = endGroup == str.size() ? str.size() : endGroup + 2;

                const size_t sep = groupChunk.find(L"@@");
                if (sep == std::wstring::npos) continue;
                TileGroup group;
                group.title = groupChunk.substr(0, sep);
                const std::wstring tilesChunk = groupChunk.substr(sep + 2);
                size_t tileStart = 0;
                while (tileStart < tilesChunk.size()) {
                    size_t tileEnd = tilesChunk.find(L";", tileStart);
                    if (tileEnd == std::wstring::npos) tileEnd = tilesChunk.size();
                    const std::wstring tileStr = tilesChunk.substr(tileStart, tileEnd - tileStart);
                    tileStart = tileEnd == tilesChunk.size() ? tilesChunk.size() : tileEnd + 1;

                    // The historical format has ten comma-separated fields.
                    // Split only the first nine separators so an empty final
                    // field remains present (the old parser silently lost it).
                    std::vector<std::wstring> fields;
                    size_t fieldStart = 0;
                    for (int field = 0; field < 9; ++field) {
                        const size_t comma = tileStr.find(L",", fieldStart);
                        if (comma == std::wstring::npos) break;
                        fields.push_back(tileStr.substr(fieldStart, comma - fieldStart));
                        fieldStart = comma + 1;
                    }
                    if (fields.size() == 9) fields.push_back(tileStr.substr(fieldStart));
                    if (fields.size() != 10) continue;

                    int size = 0, col = 0, row = 0, live = 0;
                    if (!ParseBoundedInt(fields[3], 3, size) ||
                        !ParseBoundedInt(fields[4], Metrics::BASE_COLUMNS_PER_GROUP - 1, col) ||
                        !ParseBoundedInt(fields[5], 100000, row) ||
                        !ParseBoundedInt(fields[6], 1, live)) continue;

                    Tile tile;
                    tile.name = std::move(fields[0]);
                    tile.aumid = std::move(fields[1]);
                    tile.path = std::move(fields[2]);
                    tile.size = static_cast<TileSize>(size);
                    tile.col = col;
                    tile.row = row;
                    tile.isLive = live != 0;
                    tile.liveTitle = std::move(fields[7]);
                    tile.liveSubtitle = std::move(fields[8]);
                    tile.liveDetail = std::move(fields[9]);
                    tile.available = AttachAppDetails(tile, cat);
                    if (!tile.name.empty() || !tile.path.empty() || !tile.aumid.empty())
                        group.tiles.push_back(std::move(tile));
                }
                if (!group.tiles.empty()) newGroups.push_back(std::move(group));
            }
            if (newGroups.empty()) return false;
            groups = std::move(newGroups);
            return true;
        } catch (...) {
            return false;
        }
    }

    std::wstring Serialize() const {
        // Length-prefixed fields avoid delimiter collisions in paths, names,
        // localized text, or custom tile content. The header also represents
        // an intentionally empty layout without falling back to defaults.
        std::wstring out = L"WHSM2#";
        AppendNumber(out, groups.size(), L'#');
        for (const auto& group : groups) {
            AppendField(out, group.title);
            AppendNumber(out, group.tiles.size(), L'#');
            for (const auto& tile : group.tiles) {
                AppendField(out, tile.name);
                AppendField(out, tile.aumid);
                AppendField(out, tile.path);
                AppendField(out, std::to_wstring(static_cast<int>(tile.size)));
                AppendField(out, std::to_wstring(tile.col));
                AppendField(out, std::to_wstring(tile.row));
                AppendField(out, tile.isLive ? L"1" : L"0");
                AppendField(out, tile.liveTitle);
                AppendField(out, tile.liveSubtitle);
                AppendField(out, tile.liveDetail);
            }
        }
        return out;
    }

    bool Deserialize(const std::wstring& str, const AppCatalog& cat) {
        if (str.empty()) return false;
        if (str.rfind(L"WHSM2#", 0) == 0) return ParseV2(str, cat);
        return ParseLegacy(str, cat);
    }

};

// ===================== ANCHORING AND POSITIONING =====================
struct StartAnchor {
    RECT startButton{};
    UINT edge = ABE_BOTTOM;
    HMONITOR monitor = nullptr;
    int method = 0; // 1=Win32 "Start" child, 2=UI Automation, 3=geometric fallback
    RECT taskbarRect{};
    HWND taskbarHwnd = nullptr;
    RECT workArea{};
};

static bool IsTaskbarClassName(const wchar_t* cls) {
    return _wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

static bool IsTaskbarWindow(HWND hwnd) {
    if (!hwnd) return false;
    wchar_t cls[64]{};
    return GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0 && IsTaskbarClassName(cls);
}

// Secondary taskbars are not reported by ABM_GETTASKBARPOS: derive the edge from
// the taskbar rectangle relative to its monitor.
static UINT DeriveTaskbarEdge(const RECT& taskbar, const RECT& monitor) {
    const LONG w = taskbar.right - taskbar.left;
    const LONG h = taskbar.bottom - taskbar.top;
    if (w >= h) return (taskbar.top - monitor.top) <= (monitor.bottom - taskbar.bottom) ? ABE_TOP : ABE_BOTTOM;
    return (taskbar.left - monitor.left) <= (monitor.right - taskbar.right) ? ABE_LEFT : ABE_RIGHT;
}

// Locates the Start button of `taskbar` (primary taskbar when null). Called on
// the UI thread only; may use UI Automation on Windows 11.
static bool GetStartAnchor(StartAnchor& out, HWND taskbar = nullptr) {
    HWND hTray = taskbar ? taskbar : FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTray && !IsWindow(hTray)) hTray = nullptr;
    out.taskbarHwnd = hTray;
    if (!hTray) {
        POINT pt{ 0, 0 };
        out.monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi{ sizeof(mi) };
        GetMonitorInfoW(out.monitor, &mi);
        out.workArea = mi.rcWork;
        out.edge = ABE_BOTTOM;
        out.taskbarRect = { mi.rcMonitor.left, mi.rcWork.bottom, mi.rcMonitor.right, mi.rcMonitor.bottom };
        out.startButton = { mi.rcMonitor.left, mi.rcWork.bottom, mi.rcMonitor.left + 48, mi.rcMonitor.bottom };
        out.method = 3;
        return false;
    }

    // 1. Taskbar rectangle and edge
    GetWindowRect(hTray, &out.taskbarRect);
    wchar_t cls[64]{};
    GetClassNameW(hTray, cls, ARRAYSIZE(cls));
    APPBARDATA abd{ sizeof(abd) };
    abd.hWnd = hTray;
    if (_wcsicmp(cls, L"Shell_TrayWnd") == 0 && SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) {
        out.edge = abd.uEdge;
        out.taskbarRect = abd.rc;
    } else {
        HMONITOR mon = MonitorFromWindow(hTray, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{ sizeof(mi) };
        GetMonitorInfoW(mon, &mi);
        out.edge = DeriveTaskbarEdge(out.taskbarRect, mi.rcMonitor);
    }

    // 2. Start button rectangle
    bool foundButton = false;

    // Method A: Win32 child window (Windows 10 / classic taskbar)
    HWND hStart = FindWindowExW(hTray, nullptr, L"Start", nullptr);
    if (!hStart) {
        HWND hChild = nullptr;
        while ((hChild = FindWindowExW(hTray, hChild, nullptr, nullptr)) != nullptr) {
            wchar_t childCls[64] = {};
            GetClassNameW(hChild, childCls, ARRAYSIZE(childCls));
            if (_wcsicmp(childCls, L"Start") == 0 || _wcsicmp(childCls, L"Button") == 0) {
                hStart = hChild;
                break;
            }
        }
    }
    if (hStart && IsWindowVisible(hStart)) {
        RECT rc{};
        if (GetWindowRect(hStart, &rc) && (rc.right > rc.left) && (rc.bottom > rc.top)) {
            out.startButton = rc; out.method = 1;
            foundButton = true;
        }
    }

    // Method B: UI Automation (Windows 11 XAML taskbar)
    if (!foundButton) {
        ComPtr<IUIAutomation> uia;
        if (SUCCEEDED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&uia))) && uia) {
            ComPtr<IUIAutomationElement> trayElem;
            if (SUCCEEDED(uia->ElementFromHandle(hTray, &trayElem)) && trayElem) {
                VARIANT varId;
                varId.vt = VT_BSTR;
                varId.bstrVal = SysAllocString(L"StartButton");
                ComPtr<IUIAutomationCondition> cond;
                if (SUCCEEDED(uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, varId, &cond)) && cond) {
                    ComPtr<IUIAutomationElement> btnElem;
                    if (SUCCEEDED(trayElem->FindFirst(TreeScope_Descendants, cond.Get(), &btnElem)) && btnElem) {
                        RECT uiaRc{};
                        if (SUCCEEDED(btnElem->get_CurrentBoundingRectangle(&uiaRc)) && (uiaRc.right > uiaRc.left)) {
                            out.startButton = uiaRc; out.method = 2;
                            foundButton = true;
                        }
                    }
                }
                SysFreeString(varId.bstrVal);
            }
        }
    }

    // Method C: geometric fallback (square at the taskbar's leading end)
    if (!foundButton) {
        out.method = 3;
        switch (out.edge) {
            case ABE_BOTTOM:
            case ABE_TOP:
                out.startButton.left = out.taskbarRect.left;
                out.startButton.top = out.taskbarRect.top;
                out.startButton.right = out.taskbarRect.left + (out.taskbarRect.bottom - out.taskbarRect.top);
                out.startButton.bottom = out.taskbarRect.bottom;
                break;
            case ABE_LEFT:
            case ABE_RIGHT:
                out.startButton.left = out.taskbarRect.left;
                out.startButton.top = out.taskbarRect.top;
                out.startButton.right = out.taskbarRect.right;
                out.startButton.bottom = out.taskbarRect.top + (out.taskbarRect.right - out.taskbarRect.left);
                break;
        }
    }

    // Monitor and work area
    out.monitor = MonitorFromRect(&out.startButton, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    GetMonitorInfoW(out.monitor, &mi);
    out.workArea = mi.rcWork;
    return true;
}

// Effective DPI of the monitor the menu will be shown on. Explorer keeps the
// system DPI it started with, so GetDeviceCaps(LOGPIXELSX) must not be used.
static UINT GetAnchorDpi(const StartAnchor& anchor) {
    UINT dpiX = 0, dpiY = 0;
    if (anchor.monitor && SUCCEEDED(GetDpiForMonitor(anchor.monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiX > 0)
        return dpiX;
    HDC screen = GetDC(nullptr);
    UINT dpi = 96;
    if (screen) {
        const int logical = GetDeviceCaps(screen, LOGPIXELSX);
        if (logical > 0) dpi = static_cast<UINT>(logical);
        ReleaseDC(nullptr, screen);
    }
    return dpi;
}

static POINT ComputeMenuOrigin(const StartAnchor& anchor, SIZE menuSize) {
    POINT pt{};
    switch (anchor.edge) {
        case ABE_BOTTOM:
            pt.x = anchor.startButton.left;
            pt.y = anchor.taskbarRect.top - menuSize.cy;
            break;
        case ABE_TOP:
            pt.x = anchor.startButton.left;
            pt.y = anchor.taskbarRect.bottom;
            break;
        case ABE_LEFT:
            pt.x = anchor.taskbarRect.right;
            pt.y = anchor.startButton.top;
            break;
        case ABE_RIGHT:
            pt.x = anchor.taskbarRect.left - menuSize.cx;
            pt.y = anchor.startButton.top;
            break;
    }

    // Clamp the origin to the monitor work area
    if (pt.x + menuSize.cx > anchor.workArea.right) {
        pt.x = anchor.workArea.right - menuSize.cx;
    }
    if (pt.x < anchor.workArea.left) {
        pt.x = anchor.workArea.left;
    }
    if (pt.y + menuSize.cy > anchor.workArea.bottom) {
        pt.y = anchor.workArea.bottom - menuSize.cy;
    }
    if (pt.y < anchor.workArea.top) {
        pt.y = anchor.workArea.top;
    }

    return pt;
}

// Items of the context menus (owner-drawn HMENU, see ImmersiveStartMenu).
struct MetroPopupItem { std::wstring text; UINT command; bool separator=false; bool heading=false; };

// ===================== START BUTTON CACHE (shared with the hook thread) =====================
// The low-level mouse hook must decide synchronously, without COM/UIA, whether
// a click lands on a Start button. The UI thread computes the rectangles of the
// Start buttons of every taskbar and publishes them here. They are refreshed
// when the mouse enters a taskbar, when the menu is shown, and on display/work
// area changes (no periodic polling).
struct CachedStartButton {
    HWND taskbar = nullptr;
    RECT taskbarRect{};
    RECT startRect{};
    bool reliable = false; // false when the geometric fallback was used
};
static SRWLOCK g_startCacheLock = SRWLOCK_INIT;
static std::vector<CachedStartButton> g_startCache; // guarded by g_startCacheLock
static std::atomic<ULONGLONG> g_startCacheTick{ 0 };

static void RefreshStartRects(bool logIt);

// ===================== APP LAUNCH AND COMMANDS =====================
static bool LaunchApplication(const std::wstring& path, const std::wstring& aumid) {
    if (!aumid.empty()) {
        ComPtr<IApplicationActivationManager> aam;
        HRESULT hr = CoCreateInstance(CLSID_ApplicationActivationManager, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&aam));
        if (SUCCEEDED(hr) && aam) {
            DWORD pid = 0;
            hr = aam->ActivateApplication(aumid.c_str(), nullptr, AO_NONE, &pid);
            if (SUCCEEDED(hr)) return true;
        }
    }
    if (!path.empty()) {
        HINSTANCE hInst = ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return ((INT_PTR)hInst > 32);
    }
    return false;
}

static bool LaunchAdmin(const std::wstring& path) {
    if (!path.empty()) {
        HINSTANCE hInst = ShellExecuteW(nullptr, L"runas", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return ((INT_PTR)hInst > 32);
    }
    return false;
}

static bool OpenExplorerLocation(const std::wstring& path) {
    if (!path.empty()) {
        std::wstring param = L"/select,\"" + path + L"\"";
        HINSTANCE hInst = ShellExecuteW(nullptr, L"open", L"explorer.exe", param.c_str(), nullptr, SW_SHOWNORMAL);
        return ((INT_PTR)hInst > 32);
    }
    return false;
}

static void ExecutePowerAction(int cmd) {
    HANDLE token=nullptr; TOKEN_PRIVILEGES previous{}; DWORD previousSize=sizeof(previous);
    bool restore=false;
    if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&token)){
        TOKEN_PRIVILEGES enabled{}; enabled.PrivilegeCount=1;
        if(LookupPrivilegeValueW(nullptr,L"SeShutdownPrivilege",&enabled.Privileges[0].Luid)){
            enabled.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED; SetLastError(ERROR_SUCCESS);
            if(AdjustTokenPrivileges(token,FALSE,&enabled,sizeof(previous),&previous,&previousSize)&&GetLastError()!=ERROR_NOT_ALL_ASSIGNED)restore=true;
        }
    }
    if(cmd==IDM_POWER_SLEEP||cmd==IDM_POWER_HIBERNATE){
        if(!SetSuspendState(cmd==IDM_POWER_HIBERNATE,FALSE,FALSE))Wh_Log(L"[Power] SetSuspendState failed (error=%u)",GetLastError());
    }else if(cmd==IDM_POWER_SHUTDOWN||cmd==IDM_POWER_RESTART){
        UINT flags=cmd==IDM_POWER_RESTART?EWX_REBOOT:EWX_SHUTDOWN;
        if(!ExitWindowsEx(flags,SHTDN_REASON_MAJOR_APPLICATION|SHTDN_REASON_MINOR_OTHER|SHTDN_REASON_FLAG_PLANNED))Wh_Log(L"[Power] ExitWindowsEx failed (error=%u)",GetLastError());
    }
    if(token){if(restore&&previousSize)AdjustTokenPrivileges(token,FALSE,&previous,0,nullptr,nullptr);CloseHandle(token);}
}

// Which power entries make sense on this machine (like the native menu).
// IsPwrSuspendAllowed/IsPwrHibernateAllowed (powrprof) already account for the
// supported S-states, modern standby and the presence of the hibernation file.
struct PowerMenuAvailability { bool sleep = true; bool hibernate = true; };
static PowerMenuAvailability QueryPowerMenuAvailability() {
    PowerMenuAvailability result;
    result.sleep = IsPwrSuspendAllowed() != FALSE;
    result.hibernate = IsPwrHibernateAllowed() != FALSE;
    return result;
}

// ===================== START MENU WINDOW =====================
// Owner-drawn opaque context-menu port, structurally following the supplied
// ImmersiveMenu (LoadApi/OpenMenuTheme/Begin/End/Prepare/Measure/Draw/OwnerProc/Track).
// Unlike the Start window, these popup menus are not layered and are always dark.
namespace ImmersiveStartMenu {
using ThemeHandle = void*;
static constexpr int kPartBackground=9, kPartItem=14, kStateNormal=1, kStateHot=2,
                     kPropFillColor=3802,
                     kPropFont=210;
static constexpr UINT_PTR kSubclassId=0x49534D31;
struct Api {
    ThemeHandle (WINAPI* openTheme)(HWND,LPCWSTR)=nullptr;
    ThemeHandle (WINAPI* openThemeForDpi)(HWND,LPCWSTR,UINT)=nullptr;
    HRESULT (WINAPI* closeTheme)(ThemeHandle)=nullptr;
    HRESULT (WINAPI* drawBackground)(ThemeHandle,HDC,int,int,const RECT*,const RECT*)=nullptr;
    HRESULT (WINAPI* getFont)(ThemeHandle,HDC,int,int,int,LOGFONTW*)=nullptr;
    HRESULT (WINAPI* getColor)(ThemeHandle,int,int,int,COLORREF*)=nullptr;
    bool ready=false;
};
static Api g_api;
static bool LoadApi() {
    if(g_api.ready) return true;
    HMODULE m=LoadLibraryExW(L"uxtheme.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!m) return false;
    g_api.openTheme=reinterpret_cast<decltype(g_api.openTheme)>(GetProcAddress(m,"OpenThemeData"));
    g_api.openThemeForDpi=reinterpret_cast<decltype(g_api.openThemeForDpi)>(GetProcAddress(m,"OpenThemeDataForDpi"));
    g_api.closeTheme=reinterpret_cast<decltype(g_api.closeTheme)>(GetProcAddress(m,"CloseThemeData"));
    g_api.drawBackground=reinterpret_cast<decltype(g_api.drawBackground)>(GetProcAddress(m,"DrawThemeBackground"));
    g_api.getFont=reinterpret_cast<decltype(g_api.getFont)>(GetProcAddress(m,"GetThemeFont"));
    g_api.getColor=reinterpret_cast<decltype(g_api.getColor)>(GetProcAddress(m,"GetThemeColor"));
    g_api.ready=g_api.openTheme&&g_api.closeTheme;
    return g_api.ready;
}
static bool IsDark(COLORREF c) { return (GetRValue(c)+GetGValue(c)+GetBValue(c))/3 < 110; }
static ThemeHandle OpenMenuTheme(HWND owner,int dpi) {
    const wchar_t* names[]={L"DarkMode_ImmersiveStart::Menu",L"ImmersiveStart::Menu",L"Menu"};
    for(const wchar_t* name:names) {
        ThemeHandle h=nullptr;
        if(g_api.openThemeForDpi) h=g_api.openThemeForDpi(owner,name,static_cast<UINT>(dpi));
        if(!h&&g_api.openTheme) h=g_api.openTheme(owner,name);
        if(h) return h;
    }
    return nullptr;
}
struct ItemData { wchar_t text[128]{}; bool separator=false,submenu=false,disabled=false; };
struct Session {
    HWND owner=nullptr; ThemeHandle theme=nullptr; int dpi=96; LOGFONTW font{}; HFONT hfont=nullptr;
    HBRUSH brush=nullptr; COLORREF fill=RGB(43,43,43), textNormal=RGB(255,255,255),
        textDisabled=RGB(160,160,160), separator=RGB(105,105,105), hotFill=RGB(65,65,65);
    int itemHeight=0,padLeft=32,padRight=10,chevronInset=4,separatorInset=8,minWidth=242;
    bool themeDark=false; std::vector<ItemData*> items;
};
static Session* g_session=nullptr;
static size_t CountItems(HMENU menu);
static void End(Session& s) {
    for(ItemData* d:s.items) delete d; s.items.clear();
    if(s.brush){DeleteObject(s.brush);s.brush=nullptr;}
    if(s.hfont){DeleteObject(s.hfont);s.hfont=nullptr;}
    if(s.theme&&g_api.closeTheme){g_api.closeTheme(s.theme);s.theme=nullptr;}
}
static void Prepare(Session& s,HMENU menu) {
    const int count=GetMenuItemCount(menu);
    for(int i=0;i<count;++i) {
        wchar_t text[128]{}; MENUITEMINFOW info{}; info.cbSize=sizeof(info);
        info.fMask=MIIM_FTYPE|MIIM_SUBMENU|MIIM_STATE;
        if(!GetMenuItemInfoW(menu,i,TRUE,&info)) continue;
        const bool separator=(info.fType&MFT_SEPARATOR)!=0;
        if(!separator){
            MENUITEMINFOW textInfo{}; textInfo.cbSize=sizeof(textInfo); textInfo.fMask=MIIM_STRING;
            textInfo.dwTypeData=text; textInfo.cch=static_cast<UINT>(_countof(text)-1);
            if(!GetMenuItemInfoW(menu,i,TRUE,&textInfo)) continue;
        }
        ItemData* d=new(std::nothrow) ItemData{}; if(!d) continue;
        d->separator=separator; d->submenu=info.hSubMenu!=nullptr;
        d->disabled=(info.fState&(MFS_DISABLED|MFS_GRAYED))!=0;
        wcsncpy_s(d->text,text,_TRUNCATE); s.items.push_back(d);
        MENUITEMINFOW set{}; set.cbSize=sizeof(set); set.fMask=MIIM_FTYPE|MIIM_DATA;
        set.fType=info.fType|MFT_OWNERDRAW; set.dwItemData=reinterpret_cast<ULONG_PTR>(d);
        SetMenuItemInfoW(menu,i,TRUE,&set);
        if(info.hSubMenu) Prepare(s,info.hSubMenu);
    }
    MENUINFO mi{}; mi.cbSize=sizeof(mi); mi.fMask=MIM_BACKGROUND|MIM_APPLYTOSUBMENUS; mi.hbrBack=s.brush;
    SetMenuInfo(menu,&mi);
}
static bool Begin(Session& s,HMENU menu,HWND owner,UINT dpiHint,bool compact) {
    if(!owner) return false;
    s.owner=owner; s.dpi=(dpiHint>=96&&dpiHint<=480)?static_cast<int>(dpiHint):96;
    s.items.reserve(CountItems(menu)); // allocate before Prepare mutates any HMENU entries
    s.padLeft=MulDiv(compact?14:32,s.dpi,96); s.minWidth=MulDiv(compact?150:242,s.dpi,96);
    if(LoadApi()) s.theme=OpenMenuTheme(owner,s.dpi);
    HDC dc=GetDC(owner);
    bool gotFont=false;
    if(s.theme&&g_api.getFont&&dc&&SUCCEEDED(g_api.getFont(s.theme,dc,kPartItem,kStateNormal,kPropFont,&s.font))&&s.font.lfHeight) gotFont=true;
    if(!gotFont) {
        NONCLIENTMETRICSW ncm{}; ncm.cbSize=sizeof(ncm);
        if(SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,sizeof(ncm),&ncm,0)) s.font=ncm.lfMenuFont;
        if(!s.font.lfHeight) s.font.lfHeight=-MulDiv(9,s.dpi,72);
        wcscpy_s(s.font.lfFaceName,LF_FACESIZE,L"Segoe UI");
    }
    s.themeDark=false;
    if(s.theme&&g_api.getColor) { COLORREF c=0; if(g_api.getColor(s.theme,kPartBackground,0,kPropFillColor,&c)==S_OK&&IsDark(c)) s.themeDark=true; }
    int px=abs(s.font.lfHeight);
    s.hfont=CreateFontIndirectW(&s.font);
    if(s.hfont&&dc){HGDIOBJ old=SelectObject(dc,s.hfont);TEXTMETRICW tm{};if(GetTextMetricsW(dc,&tm))px=tm.tmHeight;if(old)SelectObject(dc,old);}
    if(dc) ReleaseDC(owner,dc);
    s.itemHeight=std::max(MulDiv(32,s.dpi,96),px+MulDiv(8,s.dpi,96));
    s.brush=CreateSolidBrush(s.fill);
    if(!s.hfont||!s.brush) return false;
    Prepare(s,menu); return true;
}
static size_t CountItems(HMENU menu) {
    size_t total=0;
    for(int i=0;i<GetMenuItemCount(menu);++i){MENUITEMINFOW m{};m.cbSize=sizeof(m);m.fMask=MIIM_SUBMENU;if(GetMenuItemInfoW(menu,i,TRUE,&m)){++total;if(m.hSubMenu)total+=CountItems(m.hSubMenu);}}
    return total;
}
static bool Owns(const Session* s,const ItemData* d) { if(!s||!d)return false;for(auto* x:s->items)if(x==d)return true;return false; }
static void Measure(Session* s,HWND hwnd,MEASUREITEMSTRUCT* m,const ItemData* d) {
    if(d->separator){m->itemWidth=1;m->itemHeight=MulDiv(9,s->dpi,96);return;}
    HDC dc=GetDC(hwnd); int w=0;
    if(dc){HGDIOBJ old=SelectObject(dc,s->hfont);RECT r{};DrawTextW(dc,d->text,-1,&r,DT_CALCRECT|DT_SINGLELINE|DT_LEFT);w=r.right-r.left;if(old)SelectObject(dc,old);ReleaseDC(hwnd,dc);}
    w+=s->padLeft+MulDiv(10,s->dpi,96);
    if(d->submenu)w+=MulDiv(18,s->dpi,96);
    const int maxW=MulDiv(520,s->dpi,96); w=(std::min)(w,maxW);
    m->itemWidth=static_cast<UINT>((std::max)(s->minWidth,w));m->itemHeight=static_cast<UINT>(s->itemHeight);
}
static void Draw(Session* s,DRAWITEMSTRUCT* di,const ItemData* d) {
    HDC dc=di->hDC; RECT r=di->rcItem; SelectClipRgn(dc,nullptr); FillRect(dc,&r,s->brush);
    if(d->separator){const int y=(r.top+r.bottom)/2;RECT line{r.left+s->separatorInset,y,r.right-s->separatorInset,y+1};UniqueBrush b(CreateSolidBrush(s->separator));if(b)FillRect(dc,&line,b.get());return;}
    const bool disabled=d->disabled||(di->itemState&(ODS_DISABLED|ODS_GRAYED));
    const bool hot=(di->itemState&ODS_SELECTED)&&!disabled;
    if(hot){bool painted=false;if(s->themeDark&&s->theme&&g_api.drawBackground)painted=SUCCEEDED(g_api.drawBackground(s->theme,dc,kPartItem,kStateHot,&r,nullptr));if(!painted){UniqueBrush b(CreateSolidBrush(s->hotFill));if(b)FillRect(dc,&r,b.get());}}
    HGDIOBJ old=SelectObject(dc,s->hfont);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,disabled?s->textDisabled:s->textNormal);
    RECT tr=r;tr.left+=s->padLeft;tr.right-=s->padRight;DrawTextW(dc,d->text,-1,&tr,DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_HIDEPREFIX);
    if(d->submenu){RECT ar=r;ar.right-=s->chevronInset;HFONT arrow=CreateFontW(s->font.lfHeight,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,L"Segoe MDL2 Assets");if(arrow){SelectObject(dc,arrow);SetTextColor(dc,disabled?s->textDisabled:s->textNormal);DrawTextW(dc,L"\xE76C",-1,&ar,DT_SINGLELINE|DT_VCENTER|DT_RIGHT);SelectObject(dc,s->hfont);DeleteObject(arrow);}}
    if(old)SelectObject(dc,old);
    if(d->submenu)ExcludeClipRect(dc,r.left,r.top,r.right,r.bottom);
}
static bool MatchChar(Session* s,HMENU menu,wchar_t ch,UINT& pos) {
    if(!menu||!ch)return false;int found=-1,matches=0;
    for(int i=0;i<GetMenuItemCount(menu);++i){MENUITEMINFOW m{};m.cbSize=sizeof(m);m.fMask=MIIM_FTYPE|MIIM_SUBMENU|MIIM_DATA;if(!GetMenuItemInfoW(menu,i,TRUE,&m)||(m.fType&MFT_SEPARATOR)||m.hSubMenu)continue;auto* d=reinterpret_cast<ItemData*>(m.dwItemData);if(!Owns(s,d)||d->disabled||!d->text[0]||towupper(d->text[0])!=towupper(ch))continue;found=i;if(++matches>1)return false;}
    if(matches!=1)return false;pos=static_cast<UINT>(found);return true;
}
static LRESULT CALLBACK OwnerProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp,UINT_PTR,DWORD_PTR) {
    Session* s=g_session;
    if(s&&msg==WM_MEASUREITEM&&lp){auto* m=reinterpret_cast<MEASUREITEMSTRUCT*>(lp);auto* d=reinterpret_cast<ItemData*>(m->itemData);if(m->CtlType==ODT_MENU&&Owns(s,d)){GUARDED(L"ImmersiveMenu.Measure",{Measure(s,hwnd,m,d);})return TRUE;}}
    if(s&&msg==WM_DRAWITEM&&lp){auto* dstruct=reinterpret_cast<DRAWITEMSTRUCT*>(lp);auto* d=reinterpret_cast<ItemData*>(dstruct->itemData);if(dstruct->CtlType==ODT_MENU&&Owns(s,d)){GUARDED(L"ImmersiveMenu.Draw",{Draw(s,dstruct,d);})return TRUE;}}
    if(s&&msg==WM_MENUCHAR){UINT pos=0;if(MatchChar(s,reinterpret_cast<HMENU>(lp),static_cast<wchar_t>(LOWORD(wp)),pos))return MAKELRESULT(pos,MNC_EXECUTE);return MAKELRESULT(0,MNC_IGNORE);}
    return DefSubclassProc(hwnd,msg,wp,lp);
}
static UINT Track(HMENU menu,HWND owner,int x,int y,UINT flags,UINT dpi,bool compact=false) {
    if(!menu||!owner||GetWindowThreadProcessId(owner,nullptr)!=GetCurrentThreadId())return 0;
    Session s{}; bool subclassed=SetWindowSubclass(owner,OwnerProc,kSubclassId,0)!=FALSE;
    bool began=false;
    try { if(subclassed&&!g_session) began=Begin(s,menu,owner,dpi,compact); }
    catch(...) { Wh_Log(L"[ImmersiveMenu] initialization failed before tracking"); }
    if(!began){
        if(subclassed)RemoveWindowSubclass(owner,OwnerProc,kSubclassId);End(s);
        return static_cast<UINT>(TrackPopupMenuEx(menu,(flags&~TPM_NONOTIFY)|TPM_RETURNCMD,x,y,owner,nullptr));
    }
    g_session=&s; UINT result=0;
    GUARDED(L"ImmersiveMenu.Track",{result=static_cast<UINT>(TrackPopupMenuEx(menu,(flags&~TPM_NONOTIFY)|TPM_RETURNCMD,x,y,owner,nullptr));})
    RemoveWindowSubclass(owner,OwnerProc,kSubclassId);g_session=nullptr;End(s);return result;
}
} // namespace ImmersiveStartMenu

struct DelMenu { void operator()(HMENU h) const { if(h) DestroyMenu(h); } };
using UniqueMenu = UniqueRes<HMENU,DelMenu>;


enum class ViewMode { Normal, AllApps, AllAppsAlphabet, Search };

static std::wstring GetKnownFolderLocalizedDisplayName(const KNOWNFOLDERID& folderId, const wchar_t* fallback) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHGetKnownFolderIDList(folderId, KF_FLAG_DEFAULT, nullptr, &pidl)) || !pidl)
        return fallback ? fallback : L"";
    IShellItem* rawItem = nullptr;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&rawItem));
    CoTaskMemFree(pidl);
    if (FAILED(hr) || !rawItem) return fallback ? fallback : L"";
    struct ShellItemScope { IShellItem* p; ~ShellItemScope(){ if(p) p->Release(); } } item{rawItem};
    PWSTR name = nullptr;
    if (FAILED(rawItem->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) || !name || !name[0]) {
        if (name) CoTaskMemFree(name);
        return fallback ? fallback : L"";
    }
    std::wstring result(name);
    CoTaskMemFree(name);
    return result;
}


enum class UiText : size_t {
    Explorer, Settings, Power, AllApps, Back, MostUsed, RecentlyAdded, SearchPrefix,
    DefaultUser, GroupApps, GroupLeisure, Pinned, Unpin, Resize, Small, Medium, Wide,
    Large, EnableLive, DisableLive, RunAsAdmin, OpenLocation, Open, PinToStart,
    EditAccount, Lock, SignOut, Sleep, Shutdown, Restart, Hibernate,
    TileMail, TileCalendar, TilePhotos, TileWeather, TileGrooveMusic, TileMoviesTV,
    TileCalculator, Count
};

// Translation columns follow the requested order: Italian, English, Spanish, French,
// Persian, Arabic, Portuguese, Simplified Chinese, Japanese, Polish, Russian.
static int GetUiLanguageIndex() {
    switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
        case 0x10: return 0; // Italian
        case 0x09: return 1; // English
        case 0x0A: return 2; // Spanish
        case 0x0C: return 3; // French
        case 0x29: return 4; // Persian
        case 0x01: return 5; // Arabic
        case 0x16: return 6; // Portuguese
        case 0x04: return 7; // Chinese
        case 0x11: return 8; // Japanese
        case 0x15: return 9; // Polish
        case 0x19: return 10; // Russian
        default: return 1;
    }
}

static const wchar_t* Ui(UiText text) {
    static const wchar_t* const strings[][11] = {
        {L"Esplora file",L"File Explorer",L"Explorador de archivos",L"Explorateur de fichiers",L"\u0641\u0627\u06CC\u0644 \u0627\u06A9\u0633\u067E\u0644\u0648\u0631",L"\u0645\u0633\u062A\u0643\u0634\u0641 \u0627\u0644\u0645\u0644\u0641\u0627\u062A",L"Explorador de Arquivos",L"\u6587\u4EF6\u8D44\u6E90\u7BA1\u7406\u5668",L"\u30D5\u30A1\u30A4\u30EB \u30A8\u30AF\u30B9\u30D7\u30ED\u30FC\u30E9\u30FC",L"Eksplorator plik\u00F3w",L"\u041F\u0440\u043E\u0432\u043E\u0434\u043D\u0438\u043A"},
        {L"Impostazioni",L"Settings",L"Configuraci\u00F3n",L"Param\u00E8tres",L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A",L"\u0627\u0644\u0625\u0639\u062F\u0627\u062F\u0627\u062A",L"Configura\u00E7\u00F5es",L"\u8BBE\u7F6E",L"\u8A2D\u5B9A",L"Ustawienia",L"\u041F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B"},
        {L"Alimentazione",L"Power",L"Inicio/apagado",L"Marche/arr\u00EAt",L"\u062A\u0648\u0627\u0646",L"\u0627\u0644\u0637\u0627\u0642\u0629",L"Ligar/desligar",L"\u7535\u6E90",L"\u96FB\u6E90",L"Zasilanie",L"\u041F\u0438\u0442\u0430\u043D\u0438\u0435"},
        {L"Tutte le app",L"All apps",L"Todas las aplicaciones",L"Toutes les applications",L"\u0647\u0645\u0647 \u0628\u0631\u0646\u0627\u0645\u0647\u200C\u0647\u0627",L"\u0643\u0644 \u0627\u0644\u062A\u0637\u0628\u064A\u0642\u0627\u062A",L"Todas as aplica\u00E7\u00F5es",L"\u6240\u6709\u5E94\u7528",L"\u3059\u3079\u3066\u306E\u30A2\u30D7\u30EA",L"Wszystkie aplikacje",L"Все приложения"},
        {L"Indietro",L"Back",L"Atr\u00E1s",L"Retour",L"\u0628\u0627\u0632\u06AF\u0634\u062A",L"\u0631\u062C\u0648\u0639",L"Voltar",L"\u8FD4\u56DE",L"\u623B\u308B",L"Wstecz",L"\u041D\u0430\u0437\u0430\u0434"},
        {L"Pi\u00F9 usate",L"Most used",L"M\u00E1s usadas",L"Les plus utilis\u00E9es",L"\u067E\u0631\u06A9\u0627\u0631\u0628\u0631\u062F\u062A\u0631\u06CC\u0646",L"\u0627\u0644\u0623\u0643\u062B\u0631 \u0627\u0633\u062A\u062E\u062F\u0627\u0645\u064B\u0627",L"Mais usadas",L"\u6700\u5E38\u7528",L"\u3088\u304F\u4F7F\u3046\u30A2\u30D7\u30EA",L"Najcz\u0119\u015Bciej u\u017Cywane",L"\u0427\u0430\u0441\u0442\u043E \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u0443\u0435\u043C\u044B\u0435"},
        {L"Aggiunte di recente",L"Recently added",L"Agregadas recientemente",L"R\u00E9cemment ajout\u00E9es",L"\u0627\u0641\u0632\u0648\u062F\u0647\u200C\u0634\u062F\u0647\u200C\u0647\u0627\u06CC \u0627\u062E\u06CC\u0631",L"\u0627\u0644\u0645\u0636\u0627\u0641\u0629 \u0645\u0624\u062E\u0631\u064B\u0627",L"Adicionadas recentemente",L"\u6700\u8FD1\u6DFB\u52A0",L"\u6700\u8FD1\u8FFD\u52A0\u3057\u305F\u30A2\u30D7\u30EA",L"Ostatnio dodane",L"\u041D\u0435\u0434\u0430\u0432\u043D\u043E \u0434\u043E\u0431\u0430\u0432\u043B\u0435\u043D\u043D\u044B\u0435"},
        {L"Ricerca: ",L"Search: ",L"Buscar: ",L"Rechercher : ",L"\u062C\u0633\u062A\u062C\u0648: ",L"\u0628\u062D\u062B: ",L"Pesquisar: ",L"\u641C\u7D22: ",L"\u691C\u7D22: ",L"Szukaj: ",L"\u041F\u043E\u0438\u0441\u043A: "},
        {L"Utente",L"User",L"Usuario",L"Utilisateur",L"\u06A9\u0627\u0631\u0628\u0631",L"\u0645\u0633\u062A\u062E\u062F\u0645",L"Usu\u00E1rio",L"\u7528\u6237",L"\u30E6\u30FC\u30B6\u30FC",L"U\u017Cytkownik",L"\u041F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u0435\u043B\u044C"},
        {L"Applicazioni",L"Apps",L"Aplicaciones",L"Applications",L"\u0628\u0631\u0646\u0627\u0645\u0647\u200C\u0647\u0627",L"\u0627\u0644\u062A\u0637\u0628\u064A\u0642\u0627\u062A",L"Aplica\u00E7\u00F5es",L"\u5E94\u7528",L"\u30A2\u30D7\u30EA",L"Aplikacje",L"\u041F\u0440\u0438\u043B\u043E\u0436\u0435\u043D\u0438\u044F"},
        {L"Tempo libero",L"Play and explore",L"Jugar y explorar",L"Divertissement",L"\u0633\u0631\u06AF\u0631\u0645\u06CC \u0648 \u06A9\u0634\u0641",L"\u0627\u0644\u0644\u0639\u0628 \u0648\u0627\u0644\u0627\u0633\u062A\u0643\u0634\u0627\u0641",L"Divers\u00E3o e descoberta",L"\u5A31\u4E50\u4E0E\u63A2\u7D22",L"\u904A\u3073\u3068\u63A2\u7D22",L"Rozrywka i odkrywanie",L"\u0418\u0433\u0440\u044B \u0438 \u043E\u0442\u043A\u0440\u044B\u0442\u0438\u044F"},
        {L"Fissati",L"Pinned",L"Ancladas",L"\u00C9pingl\u00E9es",L"\u0633\u0646\u062C\u0627\u0642\u200C\u0634\u062F\u0647",L"\u0645\u062B\u0628\u062A",L"Fixados",L"\u5DF2\u56FA\u5B9A",L"\u30D4\u30F3\u7559\u3081\u6E08\u307F",L"Przypi\u0119te",L"Закрепленные"},
        {L"Rimuovi da Start",L"Unpin from Start",L"Desanclar de Inicio",L"D\u00E9tacher de l\u2019\u00E9cran de d\u00E9marrage",L"\u0628\u0631\u062F\u0627\u0634\u062A\u0646 \u0627\u0632 \u0645\u0646\u0648\u06CC \u0634\u0631\u0648\u0639",L"\u0625\u0644\u063A\u0627\u0621 \u0627\u0644\u062A\u062B\u0628\u064A\u062A \u0645\u0646 \u0627\u0628\u062F\u0623",L"Desafixar do Iniciar",L"\u4ECE\u201C\u5F00\u59CB\u201D\u53D6\u6D88\u56FA\u5B9A",L"\u30B9\u30BF\u30FC\u30C8\u304B\u3089\u30D4\u30F3\u7559\u3081\u3092\u5916\u3059",L"Odepnij od menu Start",L"Открепить от начального экрана"},
        {L"Ridimensiona",L"Resize",L"Cambiar tama\u00F1o",L"Redimensionner",L"\u062A\u063A\u06CC\u06CC\u0631 \u0627\u0646\u062F\u0627\u0632\u0647",L"\u062A\u063A\u064A\u064A\u0631 \u0627\u0644\u062D\u062C\u0645",L"Redimensionar",L"\u8C03\u6574\u5927\u5C0F",L"\u30B5\u30A4\u30BA\u5909\u66F4",L"Zmie\u0144 rozmiar",L"Изменить размер"},
        {L"Piccola",L"Small",L"Peque\u00F1a",L"Petite",L"\u06A9\u0648\u0686\u06A9",L"\u0635\u063A\u064A\u0631",L"Pequena",L"\u5C0F",L"\u5C0F\u3055\u3044",L"Ma\u0142y",L"Маленькая"},
        {L"Media",L"Medium",L"Mediana",L"Moyenne",L"\u0645\u062A\u0648\u0633\u0637",L"\u0645\u062A\u0648\u0633\u0637",L"M\u00E9dia",L"\u4E2D",L"\u4E2D",L"\u015Aredni",L"Средняя"},
        {L"Larga",L"Wide",L"Ancha",L"Large",L"\u0639\u0631\u06CC\u0636",L"\u0639\u0631\u064A\u0636",L"Larga",L"\u5BBD",L"\u5E45\u5E83",L"Szeroki",L"Широкая"},
        {L"Grande",L"Large",L"Grande",L"Grande",L"\u0628\u0632\u0631\u06AF",L"\u0643\u0628\u064A\u0631",L"Grande",L"\u5927",L"\u5927\u304D\u3044",L"Du\u017Cy",L"Большая"},
        {L"Attiva riquadro animato",L"Turn live tile on",L"Activar icono din\u00E1mico",L"Activer la vignette dynamique",L"\u0641\u0639\u0627\u0644\u200C\u0633\u0627\u0632\u06CC \u067E\u0627\u06CC\u0647 \u0632\u0646\u062F\u0647",L"\u062A\u0641\u0639\u064A\u0644 \u0627\u0644\u0628\u0644\u0627\u0637\u0629 \u0627\u0644\u062D\u064A\u0629",L"Ativar bloco din\u00E2mico",L"\u542F\u7528\u52A8\u6001\u78C1\u8D34",L"\u30E9\u30A4\u30D6\u30BF\u30A4\u30EB\u3092\u6709\u52B9\u306B\u3059\u308B",L"W\u0142\u0105cz kafelek dynamiczny",L"Включить живую плитку"},
        {L"Disattiva riquadro animato",L"Turn live tile off",L"Desactivar icono din\u00E1mico",L"D\u00E9sactiver la vignette dynamique",L"\u063A\u06CC\u0631\u0641\u0639\u0627\u0644\u200C\u0633\u0627\u0632\u06CC \u067E\u0627\u06CC\u0647 \u0632\u0646\u062F\u0647",L"\u062A\u0639\u0637\u064A\u0644 \u0627\u0644\u0628\u0644\u0627\u0637\u0629 \u0627\u0644\u062D\u064A\u0629",L"Desativar bloco din\u00E2mico",L"\u5173\u95ED\u52A8\u6001\u78C1\u8D34",L"\u30E9\u30A4\u30D6\u30BF\u30A4\u30EB\u3092\u7121\u52B9\u306B\u3059\u308B",L"Wy\u0142\u0105cz kafelek dynamiczny",L"Выключить живую плитку"},
        {L"Esegui come amministratore",L"Run as administrator",L"Ejecutar como administrador",L"Ex\u00E9cuter en tant qu\u2019administrateur",L"\u0627\u062C\u0631\u0627 \u0628\u0647\u200C\u0639\u0646\u0648\u0627\u0646 \u0645\u062F\u06CC\u0631",L"\u062A\u0634\u063A\u064A\u0644 \u0643\u0645\u0633\u0624\u0648\u0644",L"Executar como administrador",L"\u4EE5\u7BA1\u7406\u5458\u8EAB\u4EFD\u8FD0\u884C",L"\u7BA1\u7406\u8005\u3068\u3057\u3066\u5B9F\u884C",L"Uruchom jako administrator",L"Запуск от имени администратора"},
        {L"Apri percorso file",L"Open file location",L"Abrir ubicaci\u00F3n del archivo",L"Ouvrir l\u2019emplacement du fichier",L"\u06AF\u0634\u0648\u062F\u0646 \u0645\u062D\u0644 \u0641\u0627\u06CC\u0644",L"\u0641\u062A\u062D \u0645\u0648\u0642\u0639 \u0627\u0644\u0645\u0644\u0641",L"Abrir local do ficheiro",L"\u6253\u5F00\u6587\u4EF6\u4F4D\u7F6E",L"\u30D5\u30A1\u30A4\u30EB\u306E\u5834\u6240\u3092\u958B\u304F",L"Otw\u00F3rz lokalizacj\u0119 pliku",L"Открыть расположение файла"},
        {L"Apri",L"Open",L"Abrir",L"Ouvrir",L"\u06AF\u0634\u0648\u062F\u0646",L"\u0641\u062A\u062D",L"Abrir",L"\u6253\u5F00",L"\u958B\u304F",L"Otw\u00F3rz",L"Открыть"},
        {L"Aggiungi a Start",L"Pin to Start",L"Anclar a Inicio",L"\u00C9pingler \u00E0 l\u2019\u00E9cran de d\u00E9marrage",L"\u0633\u0646\u062C\u0627\u0642\u200C\u06A9\u0631\u062F\u0646 \u0628\u0647 \u0645\u0646\u0648\u06CC \u0634\u0631\u0648\u0639",L"\u062A\u062B\u0628\u064A\u062A \u0641\u064A \u0627\u0628\u062F\u0623",L"Afixar ao Iniciar",L"\u56FA\u5B9A\u5230\u201C\u5F00\u59CB\u201D",L"\u30B9\u30BF\u30FC\u30C8\u306B\u30D4\u30F3\u7559\u3081",L"Przypnij do menu Start",L"Закрепить на начальном экране"},
        {L"Modifica impostazioni account",L"Change account settings",L"Cambiar configuración de la cuenta",L"Modifier les paramètres du compte",L"\u062A\u063A\u06CC\u06CC\u0631 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u062D\u0633\u0627\u0628",L"\u062A\u063A\u064A\u064A\u0631 \u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0627\u0644\u062D\u0633\u0627\u0628",L"Alterar definições da conta",L"\u66F4\u6539\u8D26\u6237\u8BBE\u7F6E",L"\u30A2\u30AB\u30A6\u30F3\u30C8\u8A2D\u5B9A\u306E\u5909\u66F4",L"Zmie\u0144 ustawienia konta",L"Изменить параметры учетной записи"},
        {L"Blocca",L"Lock",L"Bloquear",L"Verrouiller",L"\u0642\u0641\u0644",L"\u0642\u0641\u0644",L"Bloquear",L"\u9501\u5B9A",L"\u30ED\u30C3\u30AF",L"Zablokuj",L"Заблокировать"},
        {L"Disconnetti",L"Sign out",L"Cerrar sesión",L"Se déconnecter",L"\u062E\u0631\u0648\u062C",L"\u062A\u0633\u062C\u064A\u0644 \u0627\u0644\u062E\u0631\u0648\u062C",L"Terminar sessão",L"\u6CE8\u9500",L"\u30B5\u30A4\u30F3\u30A2\u30A6\u30C8",L"Wyloguj",L"Выйти"},
        {L"Sospendi",L"Sleep",L"Suspender",L"Mettre en veille",L"\u0627\u0633\u062A\u0646\u0628\u0627\u062A",L"\u0627\u0633\u062A\u0639\u062F\u0627\u062F",L"Suspender",L"\u7761\u7720",L"\u30B9\u30EA\u30FC\u30D7",L"U\u015Bpij",L"Спящий режим"},
        {L"Arresta il sistema",L"Shut down",L"Apagar",L"Arrêter",L"\u062E\u0627\u0645\u0648\u0634 \u06A9\u0631\u062F\u0646",L"\u0625\u064A\u0642\u0627\u0641 \u0627\u0644\u062A\u0634\u063A\u064A\u0644",L"Encerrar",L"\u5173\u673A",L"\u30B7\u30E3\u30C3\u30C8\u30C0\u30A6\u30F3",L"Zamknij system",L"Завершение работы"},
        {L"Riavvia il sistema",L"Restart",L"Reiniciar",L"Redémarrer",L"\u0628\u0627\u0632\u0646\u0634\u0627\u0646\u06CC",L"\u0625\u0639\u0627\u062F\u0629 \u0627\u0644\u062A\u0634\u063A\u064A\u0644",L"Reiniciar",L"\u91CD\u542F",L"\u518D\u8D77\u52D5",L"Uruchom ponownie",L"Перезагрузка"},
        {L"Iberna",L"Hibernate",L"Hibernar",L"Mettre en veille prolongée",L"\u0632\u0645\u0633\u062A\u0627\u0646\u06CC",L"\u0625\u0633\u0628\u0627\u062A",L"Hibernar",L"\u4F11\u7720",L"\u4F11\u6B62\u72B6\u614B",L"Hibernuj",L"Гибернация"},
        {L"Posta",L"Mail",L"Correo",L"Courrier",L"\u067E\u0633\u062A",L"\u0627\u0644\u0628\u0631\u064A\u062F",L"Correio",L"\u90AE\u4EF6",L"\u30E1\u30FC\u30EB",L"Poczta",L"Почта"},
        {L"Calendario",L"Calendar",L"Calendario",L"Calendrier",L"تقویم",L"\u0627\u0644\u062A\u0642\u0648\u064A\u0645",L"Calendário",L"\u65E5\u5386",L"\u30AB\u30EC\u30F3\u30C0\u30FC",L"Kalendarz",L"Календарь"},
        {L"Foto",L"Photos",L"Fotos",L"Photos",L"\u0639\u06A9\u0633",L"\u0627\u0644\u0635\u0648\u0631",L"Fotografias",L"\u7167\u7247",L"\u5199\u771F",L"Zdjęcia",L"Фотографии"},
        {L"Meteo",L"Weather",L"El tiempo",L"Météo",L"\u0622\u0628\u200C\u0648\u0647\u0648\u0627",L"\u0627\u0644\u0637\u0642\u0633",L"Meteorologia",L"\u5929\u6C14",L"\u5929\u6C17",L"Pogoda",L"Погода"},
        {L"Groove Musica",L"Groove Music",L"Groove Música",L"Groove Musique",L"Groove Music",L"Groove Music",L"Groove Música",L"Groove \u97F3\u4E50",L"Groove \u30DF\u30E5\u30FC\u30B8\u30C3\u30AF",L"Groove Muzyka",L"Groove Музыка"},
        {L"Film e TV",L"Movies & TV",L"Películas y TV",L"Films et TV",L"\u0641\u06CC\u0644\u0645 \u0648 \u062A\u0644\u0648\u06CC\u0632\u06CC\u0648\u0646",L"\u0627\u0644\u0623\u0641\u0644\u0627\u0645 \u0648\u0627\u0644\u062A\u0644\u0641\u0632\u064A\u0648\u0646",L"Filmes e TV",L"\u7535\u5F71\u548C\u7535\u89C6",L"\u6620\u753B\u3068\u30C6\u30EC\u30D3",L"Filmy i TV",L"Фильмы и ТВ"},
        {L"Calcolatrice",L"Calculator",L"Calculadora",L"Calculatrice",L"\u062D\u0627\u0633\u0628",L"\u0627\u0644\u062D\u0627\u0633\u0628\u0629",L"Calculadora",L"\u8BA1\u7B97\u5668",L"\u96FB\u5353",L"Kalkulator",L"Калькулятор"},
    };
    const size_t index = static_cast<size_t>(text);
    return index < _countof(strings) ? strings[index][GetUiLanguageIndex()] : L"";
}

static const wchar_t* TranslateTileCaption(const std::wstring& caption) {
    if (_wcsicmp(caption.c_str(), L"Posta") == 0) return Ui(UiText::TileMail);
    if (_wcsicmp(caption.c_str(), L"Calendario") == 0) return Ui(UiText::TileCalendar);
    if (_wcsicmp(caption.c_str(), L"Foto") == 0) return Ui(UiText::TilePhotos);
    if (_wcsicmp(caption.c_str(), L"Meteo") == 0) return Ui(UiText::TileWeather);
    if (_wcsicmp(caption.c_str(), L"Groove Musica") == 0) return Ui(UiText::TileGrooveMusic);
    if (_wcsicmp(caption.c_str(), L"Film e TV") == 0) return Ui(UiText::TileMoviesTV);
    if (_wcsicmp(caption.c_str(), L"Calcolatrice") == 0) return Ui(UiText::TileCalculator);
    return caption.c_str();
}


class StartMenuWindow {
public:
    StartMenuWindow() = default;
    ~StartMenuWindow() {
        Destroy();
        if (classRegistered_ && hInst_) {
            UnregisterClassW(kWindowClassName, hInst_);
            classRegistered_ = false;
        }
    }

    static constexpr const wchar_t* kWindowClassName = L"Win10_1507_StartMenuRestorer_WndClass";

    // The class is registered under the mod's own module handle, so a class left
    // behind by a previous instance (same name, different module) is never
    // reused with a dangling WndProc. Registration failure fails Create().
    bool Create(HINSTANCE hInstance) {
        hInst_ = hInstance;
        WNDCLASSW wc{};
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInst_;
        wc.lpszClassName = kWindowClassName;
        wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
        if (!RegisterClassW(&wc)) {
            DWORD error = GetLastError();
            if (error == ERROR_CLASS_ALREADY_EXISTS) {
                // Stale registration from a previous instance of this module
                // (e.g. after a crash): drop it and register the current WndProc.
                UnregisterClassW(kWindowClassName, hInst_);
                if (RegisterClassW(&wc)) error = ERROR_SUCCESS;
                else error = GetLastError();
            }
            if (error != ERROR_SUCCESS) {
                Wh_Log(L"[StartMenu] RegisterClass failed: %u", error);
                return false;
            }
        }
        classRegistered_ = true;

        // Keep Start above ordinary applications. A per-frame window region clips
        // the moving edge so the taskbar remains visually in front during the slide.
        HWND h = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
            wc.lpszClassName,
            L"Windows 10 Start Menu",
            WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            0, 0, 100, 100,
            nullptr, nullptr, hInst_, this
        );

        if (!h) {
            Wh_Log(L"[StartMenu] CreateWindowEx failed: %u", GetLastError());
            return false;
        }

        hwnd_.reset(h);
        if (!ApplyWindowOpacity()) {
            // Do not expose a Start window if its fixed alpha could not be set.
            hwnd_.reset();
            return false;
        }
        InitFonts();
        catalog_.Refresh();
        lastCatalogScanTick_ = GetTickCount64();
        layout_.LoadDefault(catalog_);
        NormalizeTileLayout();

        const size_t savedSizeChars = Wh_GetStringValue(L"startMenuWindowSize", savedWindowSize_, ARRAYSIZE(savedWindowSize_));
        int savedWidth = 0, savedHeight = 0;
        if (savedSizeChars > 0 && savedSizeChars < ARRAYSIZE(savedWindowSize_) &&
            swscanf_s(savedWindowSize_, L"%d,%d", &savedWidth, &savedHeight) == 2 &&
            savedWidth >= 540 && savedHeight >= 420 && savedWidth <= 5000 && savedHeight <= 5000) {
            savedWidth96_ = savedWidth;
            savedHeight96_ = savedHeight;
            hasUserSized_ = true;
        }

        Wh_Log(L"[StartMenu] Window created successfully (HWND=%p)", hwnd_.get());
        return true;
    }

    void Destroy() {
        if (hwnd_) {
            StopAllAppsArrowRepeat(true);
            KillTimer(hwnd_.get(), TIMER_LIVE_FLIP);
            KillTimer(hwnd_.get(), TIMER_LIVE_TICK);
            KillTimer(hwnd_.get(), TIMER_MENU_ANIMATION);
            KillTimer(hwnd_.get(), TIMER_TILE_ANIM);
            tileAnimActive_ = false;
            menuAnimationActive_ = false;
            HWND h = hwnd_.release();
            if (h && IsWindow(h)) {
                DestroyWindow(h);
            }
        }
    }

    HWND GetHwnd() const { return hwnd_.get(); }
    bool IsVisible() const { return visible_; }

    // `taskbar` is the taskbar whose Start button was activated (nullptr = primary).
    void Toggle(HWND taskbar = nullptr) {
        if (visible_) Hide();
        else Show(taskbar);
    }

    void Show(HWND taskbar = nullptr) {
        if (!hwnd_) return;
        GUARDED(L"Show", {
            StartAnchor anchor;
            GetStartAnchor(anchor, taskbar);
            requestedTaskbar_ = anchor.taskbarHwnd;

            // Lay out at the DPI of the monitor the menu is shown on.
            const UINT targetDpi = GetAnchorDpi(anchor);
            if (targetDpi > 0) metrics_.dpi = targetDpi;
            ApplyWindowOpacity();
            InitFonts();

            // The explicit width override wins over a saved drag-resize width;
            // the saved height is kept.
            int menuW = (settings_.widthOverride > 0) ? metrics_.Scale(settings_.widthOverride)
                      : hasUserSized_ ? metrics_.Scale(savedWidth96_)
                      : metrics_.Scale(Metrics::BASE_DEFAULT_MENU_W);
            int menuH = hasUserSized_ ? metrics_.Scale(savedHeight96_)
                                      : metrics_.Scale(Metrics::BASE_DEFAULT_MENU_H);

            // Clamp to the work area without altering tile sizes (which are
            // controlled exclusively by TileSize and the context menu).
            const int maxW = (std::max)(1L, anchor.workArea.right - anchor.workArea.left);
            const int maxH = (std::max)(1L, anchor.workArea.bottom - anchor.workArea.top);
            const int minW = (std::min)(metrics_.Scale(540), maxW);
            const int minH = (std::min)(metrics_.Scale(420), maxH);
            menuW = (std::max)(minW, (std::min)(menuW, maxW));
            menuH = (std::max)(minH, (std::min)(menuH, maxH));

            SIZE sz{ menuW, menuH };
            POINT pt = ComputeMenuOrigin(anchor, sz);

            // Publish fresh Start rectangles for the hook thread, unless the
            // mouse hook just had them refreshed.
            if (GetTickCount64() - g_startCacheTick.load() > 1000) RefreshStartRects(false);
            showTick_ = GetTickCount64();
            Wh_Log(L"[StartMenu] Show: pos=(%ld,%ld) size=%dx%d anchorMethod=%d edge=%u", pt.x, pt.y, menuW, menuH, anchor.method, (unsigned)anchor.edge);
            visible_ = true;
            visibleAnchorEdge_ = anchor.edge;
            visibleTaskbarHwnd_ = anchor.taskbarHwnd;
            visibleTaskbarRect_ = anchor.taskbarRect;
            RECT targetRect{ pt.x, pt.y, pt.x + menuW, pt.y + menuH };
            RECT fromRect = OffsetTowardTaskbar(targetRect, visibleAnchorEdge_, metrics_.Scale(18));
            if (menuAnimationActive_ && IsWindowVisible(hwnd_.get())) {
                // Reopening during a closing transition starts at the exact current
                // rectangle, reversing smoothly rather than restarting off-screen.
                GetWindowRect(hwnd_.get(), &fromRect);
            }
            programmaticResize_ = true;
            if (settings_.menuAnimation) {
                SetWindowPos(hwnd_.get(), HWND_TOPMOST, fromRect.left, fromRect.top,
                             fromRect.right - fromRect.left, fromRect.bottom - fromRect.top,
                             SWP_NOACTIVATE);
                UpdateTaskbarWindowRegion();
                ShowWindow(hwnd_.get(), SW_SHOWNOACTIVATE);
            } else {
                SetWindowPos(hwnd_.get(), HWND_TOPMOST, targetRect.left, targetRect.top,
                             menuW, menuH, SWP_NOACTIVATE | SWP_SHOWWINDOW);
                UpdateTaskbarWindowRegion();
            }
            programmaticResize_ = false;
            if (settings_.menuAnimation) BeginMenuAnimation(fromRect, targetRect, false);
            else if (menuAnimationActive_) KillTimer(hwnd_.get(), TIMER_MENU_ANIMATION), menuAnimationActive_ = false;
            viewMode_ = ViewMode::Normal;
            searchQuery_.clear();
            searchAllSelected_ = false;
            allAppsScrollY_ = 0;
            hoverArea_ = -1;
            hoverIndex_ = -1;
            hoverGroup_ = -1;
            ResetKeyboardNavigation();

            if (settings_.liveTiles) {
                SetTimer(hwnd_.get(), TIMER_LIVE_FLIP, settings_.liveTileIntervalMs, nullptr);
            }

            BOOL fgOk = SetForegroundWindow(hwnd_.get());
            SetFocus(hwnd_.get());
            Wh_Log(L"[StartMenu] SetForegroundWindow=%d, foreground is menu=%d", fgOk ? 1 : 0, GetForegroundWindow() == hwnd_.get() ? 1 : 0);
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
        })
    }

    void Hide() {
        if (!visible_ || !hwnd_) return;
        GUARDED(L"Hide", {
            StopAllAppsArrowRepeat(true);
            KillTimer(hwnd_.get(), TIMER_LIVE_FLIP);
            KillTimer(hwnd_.get(), TIMER_LIVE_TICK);
            // A closed menu reopens with the tiles already in place.
            SettleTileAnimations();
            visible_ = false;
            ResetKeyboardNavigation();
            if (settings_.menuAnimation && IsWindowVisible(hwnd_.get())) {
                RECT fromRect{};
                GetWindowRect(hwnd_.get(), &fromRect);
                RECT toRect = OffsetTowardTaskbar(fromRect, visibleAnchorEdge_, metrics_.Scale(18));
                BeginMenuAnimation(fromRect, toRect, true);
            } else {
                if (menuAnimationActive_) KillTimer(hwnd_.get(), TIMER_MENU_ANIMATION);
                menuAnimationActive_ = false;
                ShowWindow(hwnd_.get(), SW_HIDE);
                ClearTaskbarWindowRegion();
            }
            Wh_Log(L"[StartMenu] Hide");
            // Refresh the app list once the menu is out of the way: either a
            // change notification arrived while it was open, or the catalog is
            // old enough that Store apps may have been installed or removed.
            constexpr ULONGLONG kStaleCatalogMs = 15ull * 60 * 1000;
            if (catalogRefreshPending_ || GetTickCount64() - lastCatalogScanTick_ > kStaleCatalogMs)
                PostMessageW(hwnd_.get(), WM_APP_REFRESH, 0, 0);
        })
    }

    void ApplySettings(const Settings& s) {
        settings_ = s;
        if (!settings_.menuAnimation && menuAnimationActive_) CompleteMenuAnimation();
        // Apply a layout only on first creation (restore from storage) or when a
        // new layout was pasted into the tileLayout setting. Other settings
        // changes leave the user's current tile arrangement untouched.
        if (!settings_.tileLayout.empty() && (!layoutApplied_ || settings_.tileLayoutImportPending)) {
            if (layout_.Deserialize(settings_.tileLayout, catalog_)) {
                if (settings_.tileLayoutImportPending) {
                    Wh_Log(L"[TileLayout] imported layout from the tileLayout setting");
                    Wh_SetStringValue(L"tileLayoutImported", settings_.tileLayout.c_str());
                }
            } else {
                Wh_Log(L"[TileLayout] stored/imported layout could not be parsed; keeping current layout");
            }
        }
        layoutApplied_ = true;
        NormalizeTileLayout();
        PersistTileLayout();
        if (!settings_.liveTiles) {
            for (auto& group : layout_.groups) {
                for (auto& tile : group.tiles) {
                    tile.flipping = false;
                    tile.flipProgress = 0.0f;
                    tile.showBack = false;
                }
            }
        }
        if (hwnd_) ApplyWindowOpacity();
        if (hwnd_) {
            if (visible_ && settings_.liveTiles) {
                SetTimer(hwnd_.get(), TIMER_LIVE_FLIP, settings_.liveTileIntervalMs, nullptr);
            } else {
                KillTimer(hwnd_.get(), TIMER_LIVE_FLIP);
                KillTimer(hwnd_.get(), TIMER_LIVE_TICK);
            }
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
        }
    }

    void InvalidateTileRect(int groupIndex, int tileIndex) {
        if (!hwnd_ || groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return;
        const auto& group = layout_.groups[groupIndex];
        if (tileIndex < 0 || tileIndex >= static_cast<int>(group.tiles.size())) return;
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        const int startX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X);
        const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_MEDIUM) +
            (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
        const int groupGap = metrics_.Scale(Metrics::BASE_GROUP_GAP);
        const int groupX = startX + groupIndex * (groupW + groupGap);
        const int tileY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
            metrics_.Scale(Metrics::BASE_GROUP_HEADER_H) + metrics_.Scale(4);
        RECT rect = ComputeTileRect(groupX, tileY, group.tiles[tileIndex]);
        InvalidateRect(hwnd_.get(), &rect, FALSE);
    }

    void OnLiveTimer() {
        if (!settings_.liveTiles) return;
        size_t liveCount = 0;
        for (const auto& group : layout_.groups)
            for (const auto& tile : group.tiles)
                if (HasLocalLiveContent(tile) && !tile.flipping) ++liveCount;
        if (!liveCount) return;

        static size_t nextLiveIndex = 0;
        size_t selected = nextLiveIndex++ % liveCount;
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            auto& group = layout_.groups[gi];
            for (size_t ti = 0; ti < group.tiles.size(); ++ti) {
                auto& tile = group.tiles[ti];
                if (!HasLocalLiveContent(tile) || tile.flipping) continue;
                if (selected-- == 0) {
                    tile.flipping = true;
                    tile.flipProgress = 0.0f;
                    InvalidateTileRect(static_cast<int>(gi), static_cast<int>(ti));
                    SetTimer(hwnd_.get(), TIMER_LIVE_TICK, 25, nullptr);
                    return;
                }
            }
        }
    }

    void OnTick() {
        bool stillFlipping = false;
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            auto& group = layout_.groups[gi];
            for (size_t ti = 0; ti < group.tiles.size(); ++ti) {
                auto& tile = group.tiles[ti];
                if (!tile.flipping) continue;
                tile.flipProgress += 0.06f;
                if (tile.flipProgress >= 1.0f) {
                    tile.flipProgress = 0.0f;
                    tile.flipping = false;
                    tile.showBack = !tile.showBack;
                } else {
                    stillFlipping = true;
                }
                InvalidateTileRect(static_cast<int>(gi), static_cast<int>(ti));
            }
        }
        if (!stillFlipping) KillTimer(hwnd_.get(), TIMER_LIVE_TICK);
    }

    void PersistWindowSize() {
        if (!hasUserSized_) return;
        wchar_t value[64]{};
        swprintf_s(value, L"%d,%d", savedWidth96_, savedHeight96_);
        if (!Wh_SetStringValue(L"startMenuWindowSize", value))
            Wh_Log(L"[StartMenu] failed to persist custom window size");
    }

    // Full rescan of the app catalog (Programs folders + AppsFolder). Launch
    // counts come from storage and expanded folders are preserved by the catalog.
    void RefreshCatalog() {
        catalogRefreshPending_ = false;
        lastCatalogScanTick_ = GetTickCount64();
        if (g_iconBitmapCache) g_iconBitmapCache->clear();
        catalog_.Refresh();
        layout_.RefreshInstalledApps(catalog_);
        searchResults_.clear();
        if (!searchQuery_.empty()) FilterSearch();
        // Row and tile indices held by the keyboard focus refer to the catalog
        // that has just been replaced.
        ClearKeyboardFocus();
        keyboardTileGroup_ = -1;
        keyboardTileIndex_ = -1;
        if (hwnd_) InvalidateRect(hwnd_.get(), nullptr, FALSE);
    }

    void PersistTileLayout() {
        const std::wstring serialized = layout_.Serialize();
        if (!Wh_SetStringValue(L"startMenuLayout", serialized.c_str()))
            Wh_Log(L"[TileLayout] failed to save layout to Windhawk mod storage");
    }

    const AppEntry* GetListAppByIndex(int index) const {
        if (index < 0) return nullptr;
        if (viewMode_ == ViewMode::Normal) {
            const int frequent = static_cast<int>(catalog_.MostUsed().size());
            if (index < frequent) return &catalog_.MostUsed()[index];
            const int recent = index - frequent;
            return recent >= 0 && recent < static_cast<int>(catalog_.RecentlyAdded().size())
                ? &catalog_.RecentlyAdded()[recent] : nullptr;
        }
        if (viewMode_ == ViewMode::Search)
            return index < static_cast<int>(searchResults_.size()) ? &searchResults_[index] : nullptr;
        if (viewMode_ == ViewMode::AllApps) return FindAllAppsAppByIndex(index);
        return nullptr;
    }

    static void GetTileFootprint(TileSize size, int& columns, int& rows) {
        columns = (size == TileSize::Wide || size == TileSize::Large) ? 2 : 1;
        rows = size == TileSize::Large ? 2 : 1;
    }

    bool IsTilePlacementFree(int groupIndex, int exceptTile, int col, int row,
                             TileSize size, bool onlyPriorTiles = false) const {
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size()) || row < 0)
            return false;
        int candidateW = 1, candidateH = 1;
        GetTileFootprint(size, candidateW, candidateH);
        if (col < 0 || col + candidateW > Metrics::BASE_COLUMNS_PER_GROUP) return false;
        const auto& tiles = layout_.groups[groupIndex].tiles;
        for (size_t i = 0; i < tiles.size(); ++i) {
            if (static_cast<int>(i) == exceptTile || (onlyPriorTiles && static_cast<int>(i) >= exceptTile))
                continue;
            const Tile& other = tiles[i];
            int otherW = 1, otherH = 1;
            GetTileFootprint(other.size, otherW, otherH);
            if (col < other.col + otherW && other.col < col + candidateW &&
                row < other.row + otherH && other.row < row + candidateH)
                return false;
        }
        return true;
    }

    bool FindTilePlacement(int groupIndex, int exceptTile, int preferredCol, int preferredRow,
                           TileSize size, bool onlyPriorTiles, int& resultCol, int& resultRow) const {
        int tileW = 1, tileH = 1;
        GetTileFootprint(size, tileW, tileH);
        const int lastCol = Metrics::BASE_COLUMNS_PER_GROUP - tileW;
        if (lastCol < 0) return false;
        preferredCol = std::max(0, std::min(lastCol, preferredCol));
        preferredRow = std::max(0, std::min(100000 - tileH + 1, preferredRow));
        for (int row = preferredRow; row <= 100000 - tileH + 1; ++row) {
            for (int delta = 0; delta <= lastCol; ++delta) {
                const int left = preferredCol - delta;
                if (left >= 0 && IsTilePlacementFree(groupIndex, exceptTile, left, row, size, onlyPriorTiles)) {
                    resultCol = left; resultRow = row; return true;
                }
                const int right = preferredCol + delta;
                if (delta && right <= lastCol &&
                    IsTilePlacementFree(groupIndex, exceptTile, right, row, size, onlyPriorTiles)) {
                    resultCol = right; resultRow = row; return true;
                }
            }
        }
        return false;
    }

    bool ApplyDraggedTilePlacement(int groupIndex, int movingIndex, int wantedCol, int wantedRow) {
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return false;
        auto& tiles = layout_.groups[groupIndex].tiles;
        if (movingIndex < 0 || movingIndex >= static_cast<int>(tiles.size()) ||
            dragOriginalTilePositions_.size() != tiles.size()) return false;

        auto restoreOriginalPositions = [&]() {
            for (size_t i = 0; i < tiles.size(); ++i) {
                tiles[i].col = dragOriginalTilePositions_[i].x;
                tiles[i].row = dragOriginalTilePositions_[i].y;
            }
        };
        restoreOriginalPositions();

        int movingW = 1, movingH = 1;
        GetTileFootprint(tiles[movingIndex].size, movingW, movingH);
        wantedCol = std::max(0, std::min(Metrics::BASE_COLUMNS_PER_GROUP - movingW, wantedCol));
        wantedRow = std::max(0, std::min(100000 - movingH + 1, wantedRow));
        tiles[movingIndex].col = wantedCol;
        tiles[movingIndex].row = wantedRow;

        std::vector<int> displaced;
        for (int i = 0; i < static_cast<int>(tiles.size()); ++i) {
            if (i == movingIndex) continue;
            int otherW = 1, otherH = 1;
            GetTileFootprint(tiles[i].size, otherW, otherH);
            const bool overlaps = wantedCol < tiles[i].col + otherW &&
                tiles[i].col < wantedCol + movingW &&
                wantedRow < tiles[i].row + otherH &&
                tiles[i].row < wantedRow + movingH;
            if (overlaps) displaced.push_back(i);
        }

        // Park only the colliding neighbors beyond the legal grid while finding
        // new cells; the search range ends at row 100000, so these sentinels can
        // never be selected or collide with a valid tile.
        for (int i : displaced) tiles[i].row = 200000 + i;
        for (int i : displaced) {
            int newCol = 0, newRow = 0;
            const POINT original = dragOriginalTilePositions_[i];
            if (!FindTilePlacement(groupIndex, i, original.x, original.y,
                                   tiles[i].size, false, newCol, newRow)) {
                restoreOriginalPositions();
                return false;
            }
            tiles[i].col = newCol;
            tiles[i].row = newRow;
        }
        return true;
    }

    // ---- Tile movement animations -----------------------------------------

    static int TileAnimOffset(float value) {
        return static_cast<int>(value >= 0.0f ? value + 0.5f : value - 0.5f);
    }

    bool IsTileLifted(int groupIndex, int tileIndex) const {
        return tileDragging_ && groupIndex >= 0 && tileIndex >= 0 &&
               groupIndex == dragGroup_ && tileIndex == dragTile_;
    }

    // Rectangle the tile is painted at: its slot shifted by the running glide,
    // or the position under the pointer while the tile is being dragged.
    bool GetTileDrawRect(int groupIndex, int tileIndex, RECT& out) const {
        if (!GetTileClientRect(groupIndex, tileIndex, out)) return false;
        if (IsTileLifted(groupIndex, tileIndex)) {
            const LONG width = out.right - out.left;
            const LONG height = out.bottom - out.top;
            out.left = dragPointer_.x - dragTileGrabOffset_.x;
            out.top = dragPointer_.y - dragTileGrabOffset_.y;
            out.right = out.left + width;
            out.bottom = out.top + height;
            ClampTileToGrid(out);
            return true;
        }
        const Tile& tile = layout_.groups[groupIndex].tiles[tileIndex];
        OffsetRect(&out, TileAnimOffset(tile.animOffsetX), TileAnimOffset(tile.animOffsetY));
        return true;
    }

    // Keeps the dragged tile inside the tile area, whatever the pointer does:
    // it can never cover the app list or the group titles, and it cannot leave
    // the menu when the pointer is dragged outside the window.
    void ClampTileToGrid(RECT& rect) const {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return;
        const int width = static_cast<int>(rect.right - rect.left);
        const int height = static_cast<int>(rect.bottom - rect.top);
        if (width <= 0 || height <= 0) return;
        // The lifted tile is painted slightly larger than its slot, so that
        // growth is kept inside the grid as well.
        const int grow = static_cast<int>((TILE_DRAG_SCALE - 1.0f) * std::max(width, height) / 2.0f) + 1;
        const int minLeft = metrics_.Scale(Metrics::BASE_LEFT_COL_W) +
                            metrics_.Scale(Metrics::BASE_GROUP_PAD_X) + grow;
        const int minTop = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
                           metrics_.Scale(Metrics::BASE_GROUP_HEADER_H) + metrics_.Scale(4) + grow;
        const int maxLeft = std::max(minLeft, static_cast<int>(client.right) -
                                     metrics_.Scale(Metrics::BASE_GROUP_PAD_X) - grow - width);
        const int maxTop = std::max(minTop, static_cast<int>(client.bottom) -
                                    metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) - grow - height);
        const int left = std::max(minLeft, std::min(maxLeft, static_cast<int>(rect.left)));
        const int top = std::max(minTop, std::min(maxTop, static_cast<int>(rect.top)));
        rect.left = left;
        rect.top = top;
        rect.right = left + width;
        rect.bottom = top + height;
    }

    // Where a group's tiles are painted right now. The result is handed to
    // StartTileGlide once the layout has changed.
    std::vector<POINT> CaptureTileRenderPositions(int groupIndex) const {
        std::vector<POINT> positions;
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return positions;
        const auto& tiles = layout_.groups[groupIndex].tiles;
        positions.reserve(tiles.size());
        for (int i = 0; i < static_cast<int>(tiles.size()); ++i) {
            RECT rect{};
            if (!GetTileDrawRect(groupIndex, i, rect)) rect = RECT{};
            positions.push_back(POINT{rect.left, rect.top});
        }
        return positions;
    }

    // Turns a layout change into a movement: every tile keeps the position it
    // was painted at and eases from there into its new slot.
    void StartTileGlide(int groupIndex, const std::vector<POINT>& before) {
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return;
        auto& tiles = layout_.groups[groupIndex].tiles;
        if (before.size() != tiles.size()) return;
        bool moved = false;
        for (size_t i = 0; i < tiles.size(); ++i) {
            // A lifted tile follows the pointer, so it is never glided.
            if (IsTileLifted(groupIndex, static_cast<int>(i))) {
                tiles[i].animOffsetX = 0.0f;
                tiles[i].animOffsetY = 0.0f;
                continue;
            }
            RECT slot{};
            if (!GetTileClientRect(groupIndex, static_cast<int>(i), slot)) continue;
            const float dx = static_cast<float>(before[i].x - slot.left);
            const float dy = static_cast<float>(before[i].y - slot.top);
            if (fabsf(dx) < TILE_ANIM_SETTLE_PX && fabsf(dy) < TILE_ANIM_SETTLE_PX) continue;
            tiles[i].animOffsetX = dx;
            tiles[i].animOffsetY = dy;
            moved = true;
        }
        if (moved) EnsureTileAnimationTimer();
    }

    void EnsureTileAnimationTimer() {
        if (!hwnd_ || tileAnimActive_) return;
        tileAnimLastTick_ = GetTickCount64();
        if (!SetTimer(hwnd_.get(), TIMER_TILE_ANIM, TILE_ANIM_INTERVAL_MS, nullptr)) {
            Wh_Log(L"[TileAnim] timer unavailable (%u): tiles move without animation", GetLastError());
            SettleTileAnimations();
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
            return;
        }
        tileAnimActive_ = true;
    }

    void StopTileAnimationTimer() {
        if (hwnd_ && tileAnimActive_) KillTimer(hwnd_.get(), TIMER_TILE_ANIM);
        tileAnimActive_ = false;
    }

    // Drops every animation to its final state: used when the menu closes and
    // whenever the timer cannot be created.
    void SettleTileAnimations() {
        for (auto& group : layout_.groups) {
            for (auto& tile : group.tiles) {
                tile.animOffsetX = 0.0f;
                tile.animOffsetY = 0.0f;
                tile.animScale = 1.0f;
                tile.flipping = false;
                tile.flipProgress = 0.0f;
            }
        }
        StopTileAnimationTimer();
    }

    // Scale the tile is easing towards: reduced while pressed, enlarged while
    // lifted by a drag, otherwise its natural size.
    float WantedTileScale(int groupIndex, int tileIndex) const {
        if (IsTileLifted(groupIndex, tileIndex)) return TILE_DRAG_SCALE;
        if (tileDragCandidate_ && !tileDragging_ &&
            groupIndex == dragGroup_ && tileIndex == dragTile_) return TILE_PRESSED_SCALE;
        return 1.0f;
    }

    // One animation frame: advances offsets and scales, then repaints only the
    // rectangles that actually changed.
    void OnTileAnimationTick() {
        const ULONGLONG now = GetTickCount64();
        const ULONGLONG elapsed = now > tileAnimLastTick_ ? now - tileAnimLastTick_ : 0;
        tileAnimLastTick_ = now;
        // The step is clamped so that a stalled UI thread cannot make the tiles
        // jump across the grid in a single frame.
        const float dt = std::min(0.05f, std::max(0.001f, static_cast<float>(elapsed) / 1000.0f));
        const float glide = 1.0f - expf(-dt * TILE_ANIM_GLIDE_RATE);
        const float scaleStep = std::min(1.0f, dt * TILE_ANIM_SCALE_RATE);

        bool animating = false;
        bool dirty = false;
        RECT dirtyRect{};
        const auto addDirty = [&dirty, &dirtyRect](const RECT& rect) {
            if (!dirty) {
                dirtyRect = rect;
                dirty = true;
                return;
            }
            RECT merged{};
            if (UnionRect(&merged, &dirtyRect, &rect)) dirtyRect = merged;
        };

        for (int gi = 0; gi < static_cast<int>(layout_.groups.size()); ++gi) {
            auto& tiles = layout_.groups[gi].tiles;
            for (int ti = 0; ti < static_cast<int>(tiles.size()); ++ti) {
                Tile& tile = tiles[ti];
                const bool lifted = IsTileLifted(gi, ti);
                const float wantedScale = WantedTileScale(gi, ti);
                const bool glidePending = !lifted &&
                    (tile.animOffsetX != 0.0f || tile.animOffsetY != 0.0f);
                const bool scalePending = fabsf(tile.animScale - wantedScale) > TILE_ANIM_SETTLE_SCALE;
                if (!glidePending && !scalePending) continue;

                RECT before{};
                const bool hadBefore = GetTileDrawRect(gi, ti, before);
                if (glidePending) {
                    if (fabsf(tile.animOffsetX) < TILE_ANIM_SETTLE_PX &&
                        fabsf(tile.animOffsetY) < TILE_ANIM_SETTLE_PX) {
                        tile.animOffsetX = 0.0f;
                        tile.animOffsetY = 0.0f;
                    } else {
                        tile.animOffsetX -= tile.animOffsetX * glide;
                        tile.animOffsetY -= tile.animOffsetY * glide;
                        animating = true;
                    }
                }
                if (scalePending) {
                    tile.animScale += (wantedScale - tile.animScale) * scaleStep;
                    if (fabsf(tile.animScale - wantedScale) <= TILE_ANIM_SETTLE_SCALE)
                        tile.animScale = wantedScale;
                    else
                        animating = true;
                }
                if (hadBefore) addDirty(before);
                RECT after{};
                if (GetTileDrawRect(gi, ti, after)) addDirty(after);
            }
        }

        if (dirty && hwnd_) {
            // The scale makes a tile slightly larger than its slot.
            InflateRect(&dirtyRect, metrics_.Scale(8), metrics_.Scale(8));
            InvalidateRect(hwnd_.get(), &dirtyRect, FALSE);
        }
        // While a tile is lifted the timer stays alive: the pointer can start a
        // new glide at any moment. Otherwise it stops as soon as everything has
        // settled and is restarted on demand.
        if (!animating && !tileDragging_) StopTileAnimationTimer();
    }

    void NormalizeTileLayout() {
        // Indices must stay stable while a tile is being dragged, so the
        // duplicate cleanup is skipped until the drag is over.
        bool changed = !tileDragCandidate_ && RemoveDuplicateTiles();
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            auto& tiles = layout_.groups[gi].tiles;
            for (size_t ti = 0; ti < tiles.size(); ++ti) {
                auto& tile = tiles[ti];
                int tileW = 1, tileH = 1;
                GetTileFootprint(tile.size, tileW, tileH);
                const int wantedCol = std::max(0, std::min(Metrics::BASE_COLUMNS_PER_GROUP - tileW, tile.col));
                const int wantedRow = std::max(0, std::min(100000 - tileH + 1, tile.row));
                int placedCol = wantedCol, placedRow = wantedRow;
                if (IsTilePlacementFree(static_cast<int>(gi), static_cast<int>(ti),
                                        wantedCol, wantedRow, tile.size, true)) {
                    if (tile.col != wantedCol || tile.row != wantedRow) changed = true;
                    tile.col = wantedCol; tile.row = wantedRow;
                    continue;
                }
                if (FindTilePlacement(static_cast<int>(gi), static_cast<int>(ti), wantedCol,
                                      wantedRow, tile.size, true, placedCol, placedRow)) {
                    tile.col = placedCol; tile.row = placedRow; changed = true;
                    Wh_Log(L"[TileLayout] moved overlapping/out-of-bounds tile to free grid cell (group=%u, tile=%u)",
                           static_cast<unsigned>(gi), static_cast<unsigned>(ti));
                }
            }
        }
        if (changed) PersistTileLayout();
    }

    // Two tiles are the same application when they start the same target: the
    // AUMID is compared first, the shortcut path when either side has none.
    static bool SameTileTarget(const std::wstring& pathA, const std::wstring& aumidA,
                               const std::wstring& pathB, const std::wstring& aumidB) {
        if (!aumidA.empty() && !aumidB.empty())
            return _wcsicmp(aumidA.c_str(), aumidB.c_str()) == 0;
        if (!pathA.empty() && !pathB.empty())
            return _wcsicmp(pathA.c_str(), pathB.c_str()) == 0;
        return false;
    }

    bool FindPinnedTile(const std::wstring& path, const std::wstring& aumid,
                        int& groupOut, int& tileOut) const {
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            const auto& tiles = layout_.groups[gi].tiles;
            for (size_t ti = 0; ti < tiles.size(); ++ti) {
                if (SameTileTarget(path, aumid, tiles[ti].path, tiles[ti].aumid)) {
                    groupOut = static_cast<int>(gi);
                    tileOut = static_cast<int>(ti);
                    return true;
                }
            }
        }
        return false;
    }

    // A layout saved by an earlier build can hold the same application twice;
    // only the first tile of each application is kept.
    bool RemoveDuplicateTiles() {
        bool removed = false;
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            auto& tiles = layout_.groups[gi].tiles;
            for (size_t ti = 0; ti < tiles.size();) {
                int firstGroup = -1, firstTile = -1;
                if (FindPinnedTile(tiles[ti].path, tiles[ti].aumid, firstGroup, firstTile) &&
                    (firstGroup != static_cast<int>(gi) || firstTile != static_cast<int>(ti))) {
                    Wh_Log(L"[TileLayout] duplicate tile '%s' removed (already pinned in group %d)",
                           tiles[ti].name.c_str(), firstGroup);
                    tiles.erase(tiles.begin() + ti);
                    removed = true;
                    continue;
                }
                ++ti;
            }
        }
        return removed;
    }

    bool PinAppInGroup(const AppEntry& app, int groupIndex, int wantedCol, int wantedRow) {
        if (!IsUsableCatalogApp(app) || groupIndex < 0 ||
            groupIndex >= static_cast<int>(layout_.groups.size())) return false;

        // An application is pinned only once: a second pin would create two
        // tiles starting the same target, so the existing one is kept.
        int pinnedGroup = -1, pinnedTile = -1;
        if (FindPinnedTile(app.path, app.aumid, pinnedGroup, pinnedTile)) {
            Wh_Log(L"[TileLayout] '%s' is already pinned (group %d, tile %d); no duplicate added",
                   app.name.c_str(), pinnedGroup, pinnedTile);
            InvalidateTileRect(pinnedGroup, pinnedTile);
            return true;
        }
        auto& tiles = layout_.groups[groupIndex].tiles;
        wantedCol = std::max(0, std::min(Metrics::BASE_COLUMNS_PER_GROUP - 1, wantedCol));
        wantedRow = std::max(0, std::min(100000, wantedRow));

        auto cellFree = [&tiles](int col, int row) {
            for (const auto& existing : tiles) {
                const int existingW = (existing.size == TileSize::Wide || existing.size == TileSize::Large) ? 2 : 1;
                const int existingH = existing.size == TileSize::Large ? 2 : 1;
                if (col < existing.col + existingW && col + 1 > existing.col &&
                    row < existing.row + existingH && row + 1 > existing.row) return false;
            }
            return true;
        };
        int chosenCol = -1, chosenRow = -1;
        for (int distance = 0; distance < 64 && chosenCol < 0; ++distance) {
            const int firstRow = std::max(0, wantedRow - distance);
            const int lastRow = std::min(100000, wantedRow + distance);
            for (int row = firstRow; row <= lastRow && chosenCol < 0; ++row) {
                for (int col = 0; col < Metrics::BASE_COLUMNS_PER_GROUP; ++col) {
                    if (std::abs(col - wantedCol) + std::abs(row - wantedRow) == distance && cellFree(col, row)) {
                        chosenCol = col; chosenRow = row; break;
                    }
                }
            }
        }
        if (chosenCol < 0) return false;

        Tile tile;
        tile.name = app.name;
        tile.path = app.path;
        tile.aumid = app.aumid;
        tile.hIcon = app.hIcon;
        tile.appColor = GetIconAccentColor(app.hIcon);
        tile.size = TileSize::Medium;
        tile.col = chosenCol;
        tile.row = chosenRow;
        tile.available = true;
        tiles.push_back(std::move(tile));
        PersistTileLayout();
        InvalidateTileRect(groupIndex, static_cast<int>(tiles.size()) - 1);
        return true;
    }

    bool PinAppAtPoint(const AppEntry& app, POINT point) {
        if (!IsUsableCatalogApp(app) || point.x < metrics_.Scale(Metrics::BASE_LEFT_COL_W)) return false;
        if (layout_.groups.empty()) {
            TileGroup group;
            group.title = L"Applicazioni";
            layout_.groups.push_back(std::move(group));
        }
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        const int groupStartX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X);
        const int cellW = metrics_.Scale(Metrics::BASE_TILE_MEDIUM) + metrics_.Scale(Metrics::BASE_TILE_GAP);
        const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_MEDIUM) +
            (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
        const int groupStep = groupW + metrics_.Scale(Metrics::BASE_GROUP_GAP);
        const int offsetX = point.x - groupStartX;
        const int groupIndex = offsetX >= 0 ? offsetX / std::max(1, groupStep) : -1;
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size()) ||
            offsetX % std::max(1, groupStep) >= groupW) return false;

        const int tileStartY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
            metrics_.Scale(Metrics::BASE_GROUP_HEADER_H) + metrics_.Scale(4);
        if (point.y < tileStartY) return false;
        const int pointX = static_cast<int>(point.x);
        const int pointY = static_cast<int>(point.y);
        const int wantedCol = std::max(0, std::min(Metrics::BASE_COLUMNS_PER_GROUP - 1,
            (pointX - (groupStartX + groupIndex * groupStep)) / std::max(1, cellW)));
        const int wantedRow = std::max(0, (pointY - tileStartY) / std::max(1, cellW));
        return PinAppInGroup(app, groupIndex, wantedCol, wantedRow);
    }

private:
    HINSTANCE hInst_ = nullptr;
    UniqueHwnd hwnd_;
    UniqueMemDC renderDc_;
    UniqueBitmap renderBitmap_;
    int renderBufferWidth_ = 0;
    int renderBufferHeight_ = 0;
    Metrics metrics_;
    Settings settings_;
    AppCatalog catalog_;
    TileLayout layout_;
    enum class PendingContext { None, Tile, App, User, Power, ExplorerPlaces };
    PendingContext pendingContext_ = PendingContext::None;
    int pendingGroup_ = -1;
    int pendingTile_ = -1;
    std::wstring pendingAppName_;
    std::wstring pendingAppPath_;
    std::wstring pendingAppAumid_;
    std::wstring accountDisplayName_; // cached; resolved off the UI thread

    bool visible_ = false;
    bool programmaticResize_ = false;
    bool customResizeActive_ = false;
    bool customResizeTop_ = false;
    bool customResizeRight_ = false;
    POINT customResizeStartPoint_{};
    RECT customResizeStartRect_{};
    bool hasUserSized_ = false;
    bool classRegistered_ = false;
    bool layoutApplied_ = false;      // stored/imported layout applied once per window
    bool catalogRefreshPending_ = false; // refresh requested while the menu was open
    ULONGLONG lastCatalogScanTick_ = 0;
    HWND requestedTaskbar_ = nullptr; // taskbar whose Start button was clicked (0 = primary)
    int savedWidth96_ = Metrics::BASE_DEFAULT_MENU_W;
    int savedHeight96_ = Metrics::BASE_DEFAULT_MENU_H;
    wchar_t savedWindowSize_[64]{};
    ULONGLONG showTick_ = 0;
    UINT visibleAnchorEdge_ = ABE_BOTTOM;
    bool menuAnimationActive_ = false;
    bool menuAnimationHidesAtEnd_ = false;
    HWND visibleTaskbarHwnd_ = nullptr;
    RECT visibleTaskbarRect_{};
    bool taskbarClipActive_ = false;
    ULONGLONG menuAnimationStart_ = 0;
    RECT menuAnimationFrom_{};
    RECT menuAnimationTo_{};

    ViewMode viewMode_ = ViewMode::Normal;
    std::wstring searchQuery_;
    bool searchAllSelected_ = false;
    RECT dragPreviewRect_{};
    POINT dragPreviewPoint_{};
    std::vector<AppEntry> searchResults_;

    int mainListScrollY_ = 0;
    int totalMainListH_ = 0;
    bool mainListScrollDragging_ = false;
    int allAppsScrollY_ = 0;
    int totalAllAppsH_ = 0;
    bool allAppsScrollDragging_ = false;
    bool allAppsScrollArrowPressed_ = false;
    int allAppsScrollRepeatDirection_ = 0;
    int scrollDragTrackTop_ = 0;
    int scrollDragTrackHeight_ = 0;
    int scrollDragThumbHeight_ = 0;
    int scrollDragThumbOffset_ = 0;
    int scrollDragMaxScroll_ = 0;

    int hoverArea_ = -1;  // 0=User, 1=Places, 2=AppList, 3=Tile, 4=GroupHeader
    int hoverIndex_ = -1;
    int hoverGroup_ = -1;

    // Keyboard navigation. The focused element is stored in the hover fields
    // above, so a pointer hover and a keyboard focus can never be displayed at
    // the same time; keyboardNav_ only records which of the two owns the
    // highlight and whether the focus ring has to be drawn.
    bool keyboardNav_ = false;
    int keyboardTileGroup_ = -1; // tile the keyboard left the grid from
    int keyboardTileIndex_ = -1;
    bool tileDragCandidate_ = false;
    bool tileDragging_ = false;
    bool appDragCandidate_ = false;
    bool appDragging_ = false;
    AppEntry draggedApp_;
    int dragGroup_ = -1, dragTile_ = -1;
    std::vector<POINT> dragOriginalTilePositions_;
    POINT dragStart_{};
    POINT dragTileGrabOffset_{}; // pointer position within tile, retained while snapping to grid.
    POINT dragPointer_{};        // last pointer position while a tile is lifted.
    bool tileAnimActive_ = false;      // TIMER_TILE_ANIM currently running
    ULONGLONG tileAnimLastTick_ = 0;   // tick of the previous animation frame

    UniqueFont fontNormal_;
    UniqueFont fontBold_;
    UniqueFont fontTitle_;
    UniqueFont fontSmall_;
    UniqueFont fontHeader_;

    void ClearTaskbarWindowRegion() {
        if (!hwnd_ || !taskbarClipActive_) return;
        if (SetWindowRgn(hwnd_.get(), nullptr, TRUE)) taskbarClipActive_ = false;
        else Wh_Log(L"[StartMenu] clearing taskbar clip failed: %u", GetLastError());
    }

    void UpdateTaskbarWindowRegion() {
        if (!hwnd_) return;
        if (!visibleTaskbarHwnd_ || !IsWindow(visibleTaskbarHwnd_) ||
            !IsWindowVisible(visibleTaskbarHwnd_)) {
            ClearTaskbarWindowRegion();
            return;
        }
        RECT windowRect{}, taskbarRect{};
        if (!GetWindowRect(hwnd_.get(), &windowRect)) return;
        if (!GetWindowRect(visibleTaskbarHwnd_, &taskbarRect)) taskbarRect = visibleTaskbarRect_;
        RECT overlap{};
        if (!IntersectRect(&overlap, &windowRect, &taskbarRect)) {
            ClearTaskbarWindowRegion();
            return;
        }
        const int width = windowRect.right - windowRect.left;
        const int height = windowRect.bottom - windowRect.top;
        UniqueRegion windowRegion(CreateRectRgn(0, 0, width, height));
        UniqueRegion taskbarCut(CreateRectRgn(overlap.left - windowRect.left,
            overlap.top - windowRect.top, overlap.right - windowRect.left,
            overlap.bottom - windowRect.top));
        if (!windowRegion || !taskbarCut ||
            CombineRgn(windowRegion.get(), windowRegion.get(), taskbarCut.get(), RGN_DIFF) == ERROR) {
            Wh_Log(L"[StartMenu] failed to create taskbar clip region");
            return;
        }
        if (SetWindowRgn(hwnd_.get(), windowRegion.get(), TRUE)) {
            windowRegion.release(); // ownership transfers to the system on success
            taskbarClipActive_ = true;
        } else {
            Wh_Log(L"[StartMenu] SetWindowRgn failed: %u", GetLastError());
        }
    }

    void SetAnimatedWindowRect(const RECT& rc) {
        if (!hwnd_) return;
        programmaticResize_ = true;
        SetWindowPos(hwnd_.get(), nullptr, rc.left, rc.top,
                     rc.right - rc.left, rc.bottom - rc.top,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
        UpdateTaskbarWindowRegion();
        programmaticResize_ = false;
    }

    void CompleteMenuAnimation() {
        if (!menuAnimationActive_) return;
        KillTimer(hwnd_.get(), TIMER_MENU_ANIMATION);
        const bool hideAtEnd = menuAnimationHidesAtEnd_;
        menuAnimationActive_ = false;
        SetAnimatedWindowRect(menuAnimationTo_);
        if (hideAtEnd && !visible_) {
            ShowWindow(hwnd_.get(), SW_HIDE);
            ClearTaskbarWindowRegion();
        }
    }

    void BeginMenuAnimation(const RECT& from, const RECT& to, bool hideAtEnd) {
        if (menuAnimationActive_) KillTimer(hwnd_.get(), TIMER_MENU_ANIMATION);
        menuAnimationFrom_ = from;
        menuAnimationTo_ = to;
        menuAnimationHidesAtEnd_ = hideAtEnd;
        menuAnimationStart_ = GetTickCount64();
        menuAnimationActive_ = true;
        if (!SetTimer(hwnd_.get(), TIMER_MENU_ANIMATION, kMenuAnimationTimerMs, nullptr)) {
            Wh_Log(L"[StartMenu] animation timer unavailable; completing transition immediately");
            CompleteMenuAnimation();
        }
    }

    void UpdateMenuAnimation() {
        if (!menuAnimationActive_) return;
        const ULONGLONG elapsed = GetTickCount64() - menuAnimationStart_;
        const double t = (std::min)(1.0, static_cast<double>(elapsed) / kMenuSlideDurationMs);
        // Smoothstep easing; per-frame position updates keep the UI thread responsive
        // so a repeated toggle can reverse from the current rectangle without a jump.
        const double eased = t * t * (3.0 - 2.0 * t);
        RECT rc{};
        auto lerp = [eased](LONG a, LONG b) {
            return static_cast<LONG>(std::lround(a + (b - a) * eased));
        };
        rc.left = lerp(menuAnimationFrom_.left, menuAnimationTo_.left);
        rc.top = lerp(menuAnimationFrom_.top, menuAnimationTo_.top);
        rc.right = lerp(menuAnimationFrom_.right, menuAnimationTo_.right);
        rc.bottom = lerp(menuAnimationFrom_.bottom, menuAnimationTo_.bottom);
        SetAnimatedWindowRect(rc);
        if (t >= 1.0) CompleteMenuAnimation();
    }

    bool ApplyWindowOpacity() {
        if (!hwnd_) return false;
        // SetLayeredWindowAttributes uses a documented alpha byte: 255 opaque,
        // 0 transparent. 242/255 is approximately 95% opacity.
        if (!SetLayeredWindowAttributes(hwnd_.get(), 0, kMenuOpacity95Percent, LWA_ALPHA)) {
            Wh_Log(L"[StartMenu] SetLayeredWindowAttributes failed: %u", GetLastError());
            return false;
        }
        return true;
    }

    RECT ClampRectToWorkArea(const RECT& candidate) const {
        HMONITOR monitor = MonitorFromRect(&candidate, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info{ sizeof(info) };
        if (!monitor || !GetMonitorInfoW(monitor, &info)) return candidate;
        return FitRectToWorkArea(candidate, info.rcWork,
                                 static_cast<LONG>(metrics_.Scale(540)),
                                 static_cast<LONG>(metrics_.Scale(420)));
    }

    bool BeginCustomResize(int x, int y) {
        if (!hwnd_ || !visible_) return false;
        RECT client{};
        if (!GetClientRect(hwnd_.get(), &client)) return false;
        const int border = std::max(5, static_cast<int>(metrics_.dpi ? metrics_.dpi : 96) / 12);
        customResizeTop_ = y >= 0 && y < border;
        customResizeRight_ = x >= client.right - border && x < client.right;
        if (!customResizeTop_ && !customResizeRight_) return false;
        if (!GetWindowRect(hwnd_.get(), &customResizeStartRect_)) return false;
        if (!GetCursorPos(&customResizeStartPoint_)) return false;
        customResizeActive_ = true;
        SetCapture(hwnd_.get());
        return true;
    }

    void ApplyCustomResize() {
        if (!customResizeActive_ || !hwnd_) return;
        POINT point{};
        if (!GetCursorPos(&point)) return;
        const LONG dx = point.x - customResizeStartPoint_.x;
        const LONG dy = point.y - customResizeStartPoint_.y;
        RECT r = customResizeStartRect_;
        MONITORINFO mi{sizeof(mi)};
        HMONITOR monitor = MonitorFromWindow(hwnd_.get(), MONITOR_DEFAULTTONEAREST);
        const bool haveWorkArea = monitor && GetMonitorInfoW(monitor, &mi);
        const LONG workW = haveWorkArea ? (std::max)(1L, mi.rcWork.right - mi.rcWork.left) : LONG_MAX;
        const LONG workH = haveWorkArea ? (std::max)(1L, mi.rcWork.bottom - mi.rcWork.top) : LONG_MAX;
        const LONG minHeight = (std::min)(static_cast<LONG>(metrics_.Scale(420)), workH);
        const LONG minWidth = (std::min)(static_cast<LONG>(metrics_.Scale(540)), workW);
        if (customResizeTop_) {
            const LONG topLimit = haveWorkArea ? mi.rcWork.top : LONG_MIN;
            r.top = (std::max)(topLimit, (std::min)(r.bottom - minHeight, r.top + dy));
        }
        if (customResizeRight_) {
            const LONG rightLimit = haveWorkArea ? mi.rcWork.right : LONG_MAX;
            r.right = (std::max)(r.left + minWidth, (std::min)(rightLimit, r.right + dx));
        }
        r = ClampRectToWorkArea(r);
        SetWindowPos(hwnd_.get(), nullptr, r.left, r.top, r.right - r.left, r.bottom - r.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void InitFonts() {
        fontNormal_.reset(CreateFontW(-metrics_.Scale(12), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                      ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"));
        fontBold_.reset(CreateFontW(-metrics_.Scale(13), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"));
        fontTitle_.reset(CreateFontW(-metrics_.Scale(16), 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"));
        fontSmall_.reset(CreateFontW(-metrics_.Scale(11), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"));
        fontHeader_.reset(CreateFontW(-metrics_.Scale(14), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                      ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"));
    }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        StartMenuWindow* pThis = nullptr;
        if (uMsg == WM_NCCREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            pThis = reinterpret_cast<StartMenuWindow*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        } else {
            pThis = reinterpret_cast<StartMenuWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }
        if (pThis) {
            try {
                return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
            } catch (const std::exception& e) {
                Wh_Log(L"[WndProc] C++ exception in message %u: %S", uMsg, e.what());
            } catch (...) {
                Wh_Log(L"[WndProc] unknown C++ exception in message %u", uMsg);
            }
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

    LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_SETCURSOR: {
                if (LOWORD(lParam) == HTCLIENT && hwnd_ && visible_) {
                    POINT p{}; GetCursorPos(&p); ScreenToClient(hwnd_.get(), &p);
                    RECT c{}; GetClientRect(hwnd_.get(), &c);
                    const int edge=std::max(5, static_cast<int>(metrics_.dpi ? metrics_.dpi : 96)/12);
                    if (p.y >= 0 && p.y < edge) { SetCursor(LoadCursorW(nullptr, IDC_SIZENS)); return TRUE; }
                    if (p.x >= c.right-edge && p.x < c.right) { SetCursor(LoadCursorW(nullptr, IDC_SIZEWE)); return TRUE; }
                }
                break;
            }
            case WM_APP_PROFILE_PHOTO_READY: {
                std::vector<BYTE> bytes;
                {
                    std::lock_guard<std::mutex> lock(g_profilePhotoMutex);
                    bytes.swap(g_profilePhotoPending);
                    if (g_accountNamePendingValid) {
                        accountDisplayName_ = g_accountNamePending;
                        g_accountNamePendingValid = false;
                        InvalidateRect(hwnd, nullptr, FALSE);
                    }
                }
                if (!bytes.empty() && bytes.size() <= 32u * 1024u * 1024u) {
                    ComPtr<IStream> stream;
                    stream.Attach(SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size())));
                    if (stream) {
                        auto bitmap = std::make_unique<Gdiplus::Bitmap>(stream.Get(), FALSE);
                        if (bitmap->GetLastStatus() == Gdiplus::Ok && bitmap->GetWidth() && bitmap->GetHeight()) {
                            g_profilePhoto.reset();
                            g_profilePhotoStream = std::move(stream);
                            g_profilePhoto = std::move(bitmap);
                            InvalidateRect(hwnd, nullptr, FALSE);
                        } else {
                            Wh_Log(L"[Profile] GDI+ could not decode account image (status=%d)",
                                   bitmap ? static_cast<int>(bitmap->GetLastStatus()) : -1);
                        }
                    } else {
                        Wh_Log(L"[Profile] SHCreateMemStream failed for account image");
                    }
                }
                return 0;
            }
            case WM_PAINT: {
                ScopedPaint paint(hwnd);
                if (paint.get()) Render(paint.get(), paint.PaintRect());
                return 0;
            }
            case WM_PRINT:
            case WM_PRINTCLIENT: {
                HDC printDc = reinterpret_cast<HDC>(wParam);
                RECT client{};
                if (printDc && GetClientRect(hwnd, &client)) Render(printDc, client);
                return 0;
            }
            case WM_ERASEBKGND:
                return 1;

            case WM_ACTIVATE:
                if (LOWORD(wParam) == WA_INACTIVE) {
                    // a context popup can steal the focus for an instant
                    if (GetTickCount64() - showTick_ > 300) Hide();
                    else Wh_Log(L"[StartMenu] WM_ACTIVATE(inactive) ignored during grace period");
                }
                return 0;

            case WM_KILLFOCUS:
                if (GetTickCount64() - showTick_ > 300) Hide();
                return 0;

            case WM_KEYDOWN:
                OnKeyDown((UINT)wParam);
                return 0;

            case WM_SYSKEYDOWN:
                // F10 and Shift+F10 arrive as system keys; every other system
                // key combination keeps its default handling.
                if (wParam == VK_F10) {
                    OnKeyDown((UINT)wParam);
                    return 0;
                }
                break;

            case WM_CLOSE:
                // Alt+F4 / SC_CLOSE only dismisses the menu; the window lives until unload.
                Hide();
                return 0;

            case WM_CHAR:
                OnChar((wchar_t)wParam);
                return 0;

            case WM_MOUSEMOVE:
                if (customResizeActive_) {
                    ApplyCustomResize();
                    return 0;
                }
                OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                return 0;

            case WM_MOUSELEAVE:
                if (!tileDragging_ && !allAppsScrollDragging_ && !mainListScrollDragging_) {
                    const int oldArea = hoverArea_, oldIndex = hoverIndex_, oldGroup = hoverGroup_;
                    hoverArea_ = hoverIndex_ = hoverGroup_ = -1;
                    InvalidateHoverTarget(oldArea, oldIndex, oldGroup);
                }
                return 0;

            case WM_LBUTTONDOWN: {
                int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
                if (BeginCustomResize(x, y)) return 0;
                if (viewMode_ == ViewMode::AllApps && BeginAllAppsScroll(x, y)) return 0;
                if ((viewMode_ == ViewMode::Normal || viewMode_ == ViewMode::Search) && BeginMainListScroll(x, y)) return 0;
                int area=-1, idx=-1, grp=-1; HitTest(POINT{x,y}, area, idx, grp);
                if (area == 3 && grp >= 0 && grp < static_cast<int>(layout_.groups.size()) &&
                    idx >= 0 && idx < static_cast<int>(layout_.groups[grp].tiles.size())) {
                    dragOriginalTilePositions_.clear();
                    dragOriginalTilePositions_.reserve(layout_.groups[grp].tiles.size());
                    for (const auto& tile : layout_.groups[grp].tiles)
                        dragOriginalTilePositions_.push_back(POINT{tile.col, tile.row});
                    tileDragCandidate_ = true;
                    tileDragging_ = false;
                    dragGroup_ = grp;
                    dragTile_ = idx;
                    dragStart_ = {x, y};
                    const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
                    const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_MEDIUM) +
                        (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
                    const int groupStep = groupW + metrics_.Scale(Metrics::BASE_GROUP_GAP);
                    const int groupX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X) + grp * groupStep;
                    const int tileY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
                        metrics_.Scale(Metrics::BASE_GROUP_HEADER_H) + metrics_.Scale(4);
                    const RECT grabbedTile = ComputeTileRect(groupX, tileY, layout_.groups[grp].tiles[idx]);
                    dragTileGrabOffset_ = {x - grabbedTile.left, y - grabbedTile.top};
                    dragPointer_ = POINT{x, y};
                    // Eases the press-in effect of the tile under the pointer.
                    EnsureTileAnimationTimer();
                    SetCapture(hwnd);
                } else if (area == 2) {
                    const AppEntry* app = GetListAppByIndex(idx);
                    if (app) {
                        draggedApp_ = *app;
                        appDragCandidate_ = true;
                        appDragging_ = false;
                        dragStart_ = {x, y};
                        SetCapture(hwnd);
                    } else {
                        OnLButtonDown(x,y); // e.g. expand/collapse an All Apps folder
                    }
                } else OnLButtonDown(x,y);
                return 0;
            }
            case WM_LBUTTONUP: {
                if (allAppsScrollArrowPressed_) {
                    StopAllAppsArrowRepeat(true);
                    return 0;
                }
                if (customResizeActive_) {
                    customResizeActive_ = false;
                    if (GetCapture() == hwnd) ReleaseCapture();
                    PersistWindowSize();
                    return 0;
                }
                if (appDragCandidate_) {
                    const bool wasDragging = appDragging_;
                    AppEntry app = std::move(draggedApp_);
                    const POINT clickStart = dragStart_;
                    appDragCandidate_ = appDragging_ = false;
                    ClearDragPreview();
                    draggedApp_ = AppEntry{};
                    if (GetCapture() == hwnd) ReleaseCapture();
                    if (wasDragging) {
                        POINT dropPoint{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                        if (!PinAppAtPoint(app, dropPoint))
                            Wh_Log(L"[TileLayout] app drag ended outside an available tile group");
                    } else {
                        OnLButtonDown(clickStart.x, clickStart.y);
                    }
                    return 0;
                }
                if (allAppsScrollDragging_ || mainListScrollDragging_) {
                    allAppsScrollDragging_ = false;
                    mainListScrollDragging_ = false;
                    if (GetCapture() == hwnd) ReleaseCapture();
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
                if (tileDragCandidate_) {
                    bool dragged=tileDragging_; int grp=dragGroup_, idx=dragTile_;
                    // Captured while the tile is still lifted: once the drag
                    // state is cleared it glides from the pointer into its slot
                    // instead of snapping there.
                    const std::vector<POINT> droppedFrom =
                        dragged ? CaptureTileRenderPositions(grp) : std::vector<POINT>();
                    tileDragCandidate_=tileDragging_=false; dragGroup_=dragTile_=-1;
                    if (dragged) StartTileGlide(grp, droppedFrom);
                    // Also lets the press-in effect ease back out.
                    EnsureTileAnimationTimer();
                    ClearDragPreview();
                    dragOriginalTilePositions_.clear();
                    if (GetCapture()==hwnd) ReleaseCapture();
                    if (!dragged && grp>=0 && grp<(int)layout_.groups.size() && idx>=0 && idx<(int)layout_.groups[grp].tiles.size()) {
                        auto& tile=layout_.groups[grp].tiles[idx];
                        if (tile.available) {
                            Hide(); catalog_.RecordLaunch(tile.path.empty()?tile.aumid:tile.path);
                            LaunchApplication(tile.path,tile.aumid);
                        } else {
                            Wh_Log(L"[TileLayout] retained tile target is currently unavailable; click ignored");
                        }
                    } else if (dragged) {
                        PersistTileLayout();
                    }
                    InvalidateRect(hwnd,nullptr,FALSE); return 0;
                }
                return 0;
            }

            case WM_RBUTTONUP:
                OnRButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                return 0;

            case WM_MOUSEWHEEL: {
                POINT wheelPt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                ScreenToClient(hwnd, &wheelPt);
                OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), wheelPt);
                return 0;
            }
            case WM_CAPTURECHANGED:
            case WM_CANCELMODE:
                StopAllAppsArrowRepeat(true);
                customResizeActive_ = false;
                ClearDragPreview();
                if (appDragCandidate_) {
                    appDragCandidate_ = appDragging_ = false;
                    ClearDragPreview();
                    draggedApp_ = AppEntry{};
                }
                if (tileDragCandidate_) {
                    const int cancelledGroup = dragGroup_;
                    const std::vector<POINT> cancelledFrom = CaptureTileRenderPositions(cancelledGroup);
                    if (dragGroup_ >= 0 && dragGroup_ < static_cast<int>(layout_.groups.size()) &&
                        dragOriginalTilePositions_.size() == layout_.groups[dragGroup_].tiles.size()) {
                        auto& tiles = layout_.groups[dragGroup_].tiles;
                        for (size_t i = 0; i < tiles.size(); ++i) {
                            tiles[i].col = dragOriginalTilePositions_[i].x;
                            tiles[i].row = dragOriginalTilePositions_[i].y;
                        }
                    }
                    dragOriginalTilePositions_.clear();
                    tileDragCandidate_ = tileDragging_ = false;
                    dragGroup_ = dragTile_ = -1;
                    // A cancelled drag puts the tiles back by gliding them.
                    StartTileGlide(cancelledGroup, cancelledFrom);
                    EnsureTileAnimationTimer();
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                if (allAppsScrollDragging_ || mainListScrollDragging_) {
                    allAppsScrollDragging_ = false;
                    mainListScrollDragging_ = false;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;

            case WM_TIMER:
                if (wParam == TIMER_ALL_APPS_SCROLL_REPEAT) OnAllAppsArrowRepeatTimer();
                else if (wParam == TIMER_MENU_ANIMATION) UpdateMenuAnimation();
                else if (wParam == TIMER_LIVE_FLIP) OnLiveTimer();
                else if (wParam == TIMER_LIVE_TICK) OnTick();
                else if (wParam == TIMER_TILE_ANIM) OnTileAnimationTick();
                return 0;

            case WM_DPICHANGED: {
                metrics_.dpi = HIWORD(wParam);
                InitFonts();
                RECT* prc = reinterpret_cast<RECT*>(lParam);
                const RECT suggested = ClampRectToWorkArea(*prc);
                programmaticResize_ = true;
                SetWindowPos(hwnd, nullptr, suggested.left, suggested.top,
                             suggested.right - suggested.left, suggested.bottom - suggested.top,
                             SWP_NOZORDER | SWP_NOACTIVATE);
                programmaticResize_ = false;
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            case WM_NCCALCSIZE:
                // WS_POPUP has no native frame; keep all pixels in our client area.
                return 0;
            case WM_NCPAINT:
                return 0;
            case WM_NCACTIVATE:
                return TRUE;
            case WM_NCHITTEST:
                // Resizing is handled by our client-area drag code, never by Windows chrome.
                return HTCLIENT;
            case WM_GETMINMAXINFO: {
                auto* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
                MONITORINFO mi{ sizeof(mi) };
                HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                if (!GetMonitorInfoW(monitor, &mi)) SystemParametersInfoW(SPI_GETWORKAREA, 0, &mi.rcWork, 0);
                const LONG workW = (std::max)(1L, mi.rcWork.right - mi.rcWork.left);
                const LONG workH = (std::max)(1L, mi.rcWork.bottom - mi.rcWork.top);
                // Match the original layout's monitor-bounded sizing behavior:
                // the minimum must never exceed the available work area.
                mmi->ptMinTrackSize.x = (std::min)(static_cast<LONG>(metrics_.Scale(540)), workW);
                mmi->ptMinTrackSize.y = (std::min)(static_cast<LONG>(metrics_.Scale(420)), workH);
                mmi->ptMaxTrackSize.x = workW;
                mmi->ptMaxTrackSize.y = workH;
                return 0;
            }
            case WM_SIZE:
                if (visible_ && !programmaticResize_ && wParam != SIZE_MINIMIZED) {
                    RECT client{};
                    if (GetClientRect(hwnd, &client)) {
                        const int width = client.right - client.left;
                        const int height = client.bottom - client.top;
                        if (width > 0 && height > 0) {
                            savedWidth96_ = MulDiv(width, 96, static_cast<int>(metrics_.dpi));
                            savedHeight96_ = MulDiv(height, 96, static_cast<int>(metrics_.dpi));
                            hasUserSized_ = true;
                        }
                    }
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;

            case WM_NCDESTROY:
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);

            case WM_APP_TOGGLE:
                Wh_Log(L"[StartMenu] WM_APP_TOGGLE received (visible=%d)", visible_ ? 1 : 0);
                Toggle(reinterpret_cast<HWND>(lParam));
                return 0;

            case WM_APP_HIDE:
                Hide();
                return 0;

            case WM_APP_REFRESH:
                // Posted by the Programs-folder watcher and by the stale-catalog
                // check. Defer while the menu is open so the lists don't change
                // under the user's cursor.
                if (visible_ || menuAnimationActive_) { catalogRefreshPending_ = true; return 0; }
                RefreshCatalog();
                return 0;

            case WM_APP_REFRESH_ANCHOR:
                RefreshStartRects(false);
                return 0;

            case WM_DISPLAYCHANGE:
                PostMessageW(hwnd, WM_APP_REFRESH_ANCHOR, 0, 0);
                return 0;

            case WM_SETTINGCHANGE:
                if (wParam == SPI_SETWORKAREA) PostMessageW(hwnd, WM_APP_REFRESH_ANCHOR, 0, 0);
                return 0;
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

    int BottomPlaceItemCount() const {
        return (viewMode_ == ViewMode::AllApps || viewMode_ == ViewMode::AllAppsAlphabet) ? 1 : 4;
    }

    // Maps an All-apps section index to its cell in the alphabet grid
    // (layout: #, 0-9, A-Z, globe; 5 columns), in client coordinates.
    bool AlphabetCellRectForSection(int section, int leftW, int userH, RECT& out) const {
        if (section < 0) return false;
        const int pad = metrics_.Scale(10);
        const int cellW = std::max(1, (leftW - 2 * pad) / 5);
        const int cellH = metrics_.Scale(48);
        const int top = userH + metrics_.Scale(10);
        for (int n = 0; n < 29; ++n) {
            wchar_t key = 0;
            if (n == 0) key = L'#';
            else if (n == 1) key = L'0';
            else if (n < 28) key = static_cast<wchar_t>(L'A' + n - 2);
            else key = L'~';
            if (FindAllAppsSection(key) != section) continue;
            const int col = n % 5, row = n / 5;
            out = RECT{pad + col * cellW, top + row * cellH,
                       pad + (col + 1) * cellW, top + (row + 1) * cellH};
            return true;
        }
        return false;
    }

    void InvalidateHoverTarget(int area, int index, int groupIndex) {
        if (!hwnd_) return;
        RECT client{};
        if (!GetClientRect(hwnd_.get(), &client)) return;
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        const int userH = metrics_.Scale(Metrics::BASE_USER_HEADER_H);
        const int placeH = metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        const int bottomY = static_cast<int>(client.bottom) - BottomPlaceItemCount() * placeH;
        RECT dirty{};
        switch (area) {
            case 0: dirty = RECT{0, 0, leftW, userH}; break;
            case 1:
                if (index < 0 || index >= 4) return;
                if (BottomPlaceItemCount() == 1) {
                    if (index != 3) return;
                    dirty = RECT{0, bottomY, leftW, bottomY + placeH};
                } else {
                    dirty = RECT{0, bottomY + index * placeH, leftW, bottomY + (index + 1) * placeH};
                }
                break;
            case 2: dirty = RECT{0, userH, leftW, bottomY}; break;
            case 3:
                if (!GetTileClientRect(groupIndex, index, dirty)) return;
                break;
            case 5: dirty = RECT{0, userH, leftW, bottomY}; break;
            case 6: {
                // Alphabet grid: repaint only the hovered cell instead of the
                // whole left column (or nothing at all, which left the highlight stale).
                if (!AlphabetCellRectForSection(groupIndex, leftW, userH, dirty)) {
                    dirty = RECT{0, userH, leftW, bottomY};
                }
                break;
            }
            case 4: {
                if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return;
                const int startX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X);
                const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_MEDIUM) +
                    (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
                const int groupX = startX + groupIndex * (groupW + metrics_.Scale(Metrics::BASE_GROUP_GAP));
                dirty = RECT{groupX, metrics_.Scale(Metrics::BASE_GROUP_PAD_Y),
                    groupX + groupW, metrics_.Scale(Metrics::BASE_GROUP_PAD_Y + Metrics::BASE_GROUP_HEADER_H)};
                break;
            }
            default: return;
        }
        InvalidateRect(hwnd_.get(), &dirty, FALSE);
    }

    void OnMouseMove(int x, int y) {
        if (allAppsScrollDragging_ || mainListScrollDragging_) {
            const int travel = std::max(1, scrollDragTrackHeight_ - scrollDragThumbHeight_);
            const int thumbTop = std::max(scrollDragTrackTop_,
                std::min(y - scrollDragThumbOffset_, scrollDragTrackTop_ + travel));
            const int offset = thumbTop - scrollDragTrackTop_;
            const int newScroll = scrollDragMaxScroll_ > 0
                ? (offset * scrollDragMaxScroll_ + travel / 2) / travel : 0;
            if (mainListScrollDragging_) mainListScrollY_ = newScroll;
            else allAppsScrollY_ = newScroll;
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
            return;
        }
        if (appDragCandidate_ && !appDragging_ &&
            (std::abs(x - dragStart_.x) > metrics_.Scale(6) || std::abs(y - dragStart_.y) > metrics_.Scale(6))) {
            appDragging_ = true;
        }
        if (appDragging_) {
            SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(32649)));
            UpdateDragPreview(x, y);
        }

        if (tileDragCandidate_ && dragGroup_ >= 0 && dragGroup_ < static_cast<int>(layout_.groups.size()) &&
            dragTile_ >= 0 && dragTile_ < static_cast<int>(layout_.groups[dragGroup_].tiles.size())) {
            const int dragThreshold = metrics_.Scale(6);
            if (!tileDragging_ &&
                (std::abs(x - dragStart_.x) > dragThreshold || std::abs(y - dragStart_.y) > dragThreshold))
                tileDragging_ = true;
            if (tileDragging_) {
                // The lifted tile follows the pointer: repaint the area it is
                // leaving and the one it now covers, not the whole grid.
                RECT liftedBefore{};
                const bool hadLifted = GetTileDrawRect(dragGroup_, dragTile_, liftedBefore);
                dragPointer_ = POINT{x, y};
                EnsureTileAnimationTimer();
                RECT liftedNow{};
                const bool haveLifted = GetTileDrawRect(dragGroup_, dragTile_, liftedNow);
                if (hadLifted && haveLifted && hwnd_) {
                    RECT dirty{};
                    if (UnionRect(&dirty, &liftedBefore, &liftedNow)) {
                        InflateRect(&dirty, metrics_.Scale(10), metrics_.Scale(10));
                        InvalidateRect(hwnd_.get(), &dirty, FALSE);
                    }
                }
                const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
                const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_MEDIUM) +
                    (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
                const int cell = std::max(1, metrics_.Scale(Metrics::BASE_TILE_MEDIUM) +
                                             metrics_.Scale(Metrics::BASE_TILE_GAP));
                const int groupX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X) +
                    dragGroup_ * (groupW + metrics_.Scale(Metrics::BASE_GROUP_GAP));
                const int tilesTop = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
                    metrics_.Scale(Metrics::BASE_GROUP_HEADER_H) + metrics_.Scale(4);
                // The target cell follows the tile as it is drawn, which the
                // clamp keeps inside the grid, rather than the bare pointer.
                const int wantedLeft = haveLifted ? static_cast<int>(liftedNow.left)
                                                  : x - dragTileGrabOffset_.x;
                const int wantedTop = haveLifted ? static_cast<int>(liftedNow.top)
                                                 : y - dragTileGrabOffset_.y;
                int movingW = 1, movingH = 1;
                GetTileFootprint(layout_.groups[dragGroup_].tiles[dragTile_].size, movingW, movingH);
                const int lastCol = Metrics::BASE_COLUMNS_PER_GROUP - movingW;
                // Ignore positions outside this group's snap area instead of forcing
                // the tile to an edge while the pointer crosses the menu or group gap.
                const int minSnapX = groupX - cell / 2;
                const int maxSnapX = groupX + lastCol * cell + cell / 2;
                if (lastCol >= 0 && wantedLeft >= minSnapX && wantedLeft <= maxSnapX &&
                    wantedTop >= tilesTop - cell / 2) {
                    const int wantedCol = std::max(0, std::min(lastCol,
                        (wantedLeft - groupX + cell / 2) / cell));
                    const int wantedRow = std::max(0, (wantedTop - tilesTop + cell / 2) / cell);
                    const Tile& moving = layout_.groups[dragGroup_].tiles[dragTile_];
                    if (moving.col != wantedCol || moving.row != wantedRow) {
                        // Remember where the tiles are painted so that the
                        // neighbours pushed aside by the drop glide to their new
                        // slot instead of appearing there.
                        const std::vector<POINT> beforeReflow = CaptureTileRenderPositions(dragGroup_);
                        if (ApplyDraggedTilePlacement(dragGroup_, dragTile_, wantedCol, wantedRow))
                            StartTileGlide(dragGroup_, beforeReflow);
                        RECT client{};
                        if (GetClientRect(hwnd_.get(), &client)) {
                            RECT tileViewport{leftW, 0, client.right, client.bottom};
                            InvalidateRect(hwnd_.get(), &tileViewport, FALSE);
                        }
                    }
                }
                return;
            }
        }
        if (keyboardNav_) {
            // The pointer wins over the keyboard: drop the focus ring even when
            // the pointer lands on the element that was focused.
            keyboardNav_ = false;
            InvalidateHoverTarget(hoverArea_, hoverIndex_, hoverGroup_);
        }

        int oldArea = hoverArea_;
        int oldIdx = hoverIndex_;
        int oldGrp = hoverGroup_;

        POINT pt{ x, y };
        HitTest(pt, hoverArea_, hoverIndex_, hoverGroup_);

        if (oldArea != hoverArea_ || oldIdx != hoverIndex_ || oldGrp != hoverGroup_) {
            InvalidateHoverTarget(oldArea, oldIdx, oldGrp);
            InvalidateHoverTarget(hoverArea_, hoverIndex_, hoverGroup_);
        }

        TRACKMOUSEEVENT tme{ sizeof(tme), TME_LEAVE, hwnd_.get(), 0 };
        TrackMouseEvent(&tme);
    }

    void OnLButtonDown(int x, int y) {
        POINT pt{ x, y };
        int area = -1, idx = -1, grp = -1;
        HitTest(pt, area, idx, grp);
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        // Only the far-right chevron of the Explorer row opens its pinned places.
        const bool explorerChevron = (area == 1 && idx == 0 &&
                                      x >= leftW - metrics_.Scale(38));
        ActivateItem(area, idx, grp, pt, explorerChevron);
    }

    // Runs an element of the menu. Shared by the pointer (click) and by the
    // keyboard (Enter): `pt` only positions the flyouts, and `explorerChevron`
    // selects the Explorer jump list instead of an Explorer window.
    void ActivateItem(int area, int idx, int grp, POINT pt, bool explorerChevron) {
        if (area == 0) { // user header -> account menu
            ShowUserMenu(pt);
        } else if (area == 1) { // File Explorer, Settings, Power, All apps
            if (idx == 0) { // File Explorer, or its pinned places
                if (explorerChevron) {
                    ShowExplorerJumpList();
                } else {
                    Hide();
                    ShellExecuteW(nullptr, L"open", L"explorer.exe", nullptr, nullptr, SW_SHOWNORMAL);
                }
            } else if (idx == 1) { // Settings
                Hide();
                ShellExecuteW(nullptr, L"open", L"ms-settings:", nullptr, nullptr, SW_SHOWNORMAL);
            } else if (idx == 2) { // Power
                ShowPowerMenu(pt);
            } else if (idx == 3) { // All apps / Back
                if (viewMode_ == ViewMode::Normal) viewMode_ = ViewMode::AllApps;
                else if (viewMode_ == ViewMode::AllAppsAlphabet) viewMode_ = ViewMode::AllApps;
                else viewMode_ = ViewMode::Normal;
                // The list below changed: keep the focus on the row just used,
                // whose index is valid in both views.
                if (keyboardNav_) SetKeyboardFocus(1, 3, -1);
                InvalidateRect(hwnd_.get(), nullptr, FALSE);
            }
        } else if (area == 2) { // App list / folders
            if (viewMode_ == ViewMode::Normal) {
                if (idx >= 0 && idx < (int)catalog_.MostUsed().size()) {
                    LaunchCatalogEntry(catalog_.MostUsed()[idx]);
                } else if (idx >= (int)catalog_.MostUsed().size() &&
                           idx < (int)(catalog_.MostUsed().size() + catalog_.RecentlyAdded().size())) {
                    int recentIdx = idx - (int)catalog_.MostUsed().size();
                    LaunchCatalogEntry(catalog_.RecentlyAdded()[recentIdx]);
                }
            } else if (viewMode_ == ViewMode::Search) {
                if (idx >= 0 && idx < (int)searchResults_.size()) {
                    LaunchCatalogEntry(searchResults_[idx]);
                }
            } else if (viewMode_ == ViewMode::AllApps) {
                HandleAllAppsClick(idx);
            }
        } else if (area == 5 && viewMode_ == ViewMode::AllApps) {
            viewMode_ = ViewMode::AllAppsAlphabet;
            ClearKeyboardFocus();
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
        } else if (area == 6 && viewMode_ == ViewMode::AllAppsAlphabet && grp >= 0) {
            const bool fromKeyboard = keyboardNav_;
            viewMode_ = ViewMode::AllApps;
            ScrollToAllAppsSection(grp);
            // Keyboard jumps continue on the first entry of the chosen letter.
            if (fromKeyboard) {
                const int row = FirstRowOfSection(grp);
                if (row >= 0) SetKeyboardFocus(2, row, -1);
                else ClearKeyboardFocus();
            }
        } else if (area == 3) { // Click on a tile
            if (grp >= 0 && grp < (int)layout_.groups.size()) {
                auto& group = layout_.groups[grp];
                if (idx >= 0 && idx < (int)group.tiles.size()) {
                    const auto& tile = group.tiles[idx];
                    Hide();
                    catalog_.RecordLaunch(tile.path.empty() ? tile.aumid : tile.path);
                    LaunchApplication(tile.path, tile.aumid);
                }
            }
        }
    }

    void OnRButtonUp(int x, int y) {
        POINT pt{ x, y };
        int area = -1, idx = -1, grp = -1;
        HitTest(pt, area, idx, grp);

        if (area == 0) { // Account header context menu
            ShowUserMenu(pt);
        } else if (area == 3 && grp >= 0 && idx >= 0) { // Tile context menu
            ShowTileContextMenu(pt, grp, idx);
        } else if (area == 2 && idx >= 0) { // App context menu
            ShowAppContextMenu(pt, idx);
        }
    }

    // Geometry is derived from the live client area, not the default menu size;
    // this keeps scrolling correct with DPI changes, short work areas and resizing.
    bool GetMainListScrollGeometry(RECT& track, RECT& thumb, int& maxScroll) const {
        track = {}; thumb = {}; maxScroll = 0;
        if ((viewMode_ != ViewMode::Normal && viewMode_ != ViewMode::Search) || !hwnd_) return false;
        RECT client{}; if (!GetClientRect(hwnd_.get(), &client)) return false;
        const int top = metrics_.Scale(Metrics::BASE_USER_HEADER_H);
        const int bottom = client.bottom - BottomPlaceItemCount() * metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        const int visible = bottom - top;
        if (visible <= 0 || totalMainListH_ <= visible) return false;
        const int edge = std::min(static_cast<int>(client.right), metrics_.Scale(Metrics::BASE_LEFT_COL_W));
        const int width = std::max(4, metrics_.Scale(6));
        track = {std::max(0,edge-width),top,edge,bottom};
        const int trackH = track.bottom-track.top;
        const int thumbH = std::min(trackH,std::max(metrics_.Scale(20),trackH*visible/totalMainListH_));
        maxScroll=totalMainListH_-visible;
        const int travel=trackH-thumbH;
        const int thumbY=track.top+(maxScroll ? travel*std::max(0,std::min(mainListScrollY_,maxScroll))/maxScroll : 0);
        thumb={track.left,thumbY,track.right,thumbY+thumbH};
        return true;
    }

    // Scrollbar palette (dark, like the 1507 All apps list).
    static constexpr COLORREF kScrollRail      = RGB(31, 31, 31);
    static constexpr COLORREF kScrollThumb     = RGB(118, 118, 118);
    static constexpr COLORREF kScrollThumbHot  = RGB(190, 190, 190);
    static constexpr COLORREF kScrollArrow     = RGB(190, 190, 190);
    static constexpr COLORREF kScrollArrowDim  = RGB(85, 85, 85);
    static constexpr COLORREF kScrollArrowHot  = RGB(255, 255, 255);

    // DrawFrameControl(DFC_SCROLL) always paints with the system 3D colors
    // (COLOR_BTNFACE, embossed glyph for DFCS_INACTIVE) and ignores the DC
    // colors, which produced light boxes and doubled arrows on the dark menu.
    // Draw a small antialiased triangle instead, with the menu's own palette.
    void DrawScrollArrow(HDC hdc, const RECT& button, bool up, COLORREF color) {
        const int w = button.right - button.left, h = button.bottom - button.top;
        if (w <= 0 || h <= 0 || !g_gdiplusToken) return;
        const float arrowW = static_cast<float>(std::max(5, std::min(w - 4, metrics_.Scale(7))));
        const float arrowH = arrowW / 2.0f;
        const float cx = button.left + w / 2.0f;
        const float cy = button.top + h / 2.0f;
        Gdiplus::PointF pts[3];
        if (up) {
            pts[0] = { cx - arrowW / 2, cy + arrowH / 2 };
            pts[1] = { cx + arrowW / 2, cy + arrowH / 2 };
            pts[2] = { cx, cy - arrowH / 2 };
        } else {
            pts[0] = { cx - arrowW / 2, cy - arrowH / 2 };
            pts[1] = { cx + arrowW / 2, cy - arrowH / 2 };
            pts[2] = { cx, cy + arrowH / 2 };
        }
        Gdiplus::Graphics g(hdc);
        if (g.GetLastStatus() != Gdiplus::Ok) return;
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));
        g.FillPolygon(&brush, pts, 3);
    }

    void DrawScrollThumb(HDC hdc, RECT thumb, bool hot) {
        // Leave a 1 px gutter on both sides so the thumb never touches the
        // column border or the list text.
        const int gutter = std::max(1, metrics_.Scale(1));
        InflateRect(&thumb, -gutter, 0);
        if (thumb.right <= thumb.left || thumb.bottom <= thumb.top) return;
        UniqueBrush brush(CreateSolidBrush(hot ? kScrollThumbHot : kScrollThumb));
        FillRect(hdc, &thumb, brush.get());
    }

    void DrawMainListScrollbar(HDC hdc, const RECT&) {
        RECT track{},thumb{}; int maxScroll=0;
        if (!GetMainListScrollGeometry(track,thumb,maxScroll)) return;
        UniqueBrush rail(CreateSolidBrush(kScrollRail));
        FillRect(hdc,&track,rail.get());
        DrawScrollThumb(hdc, thumb, mainListScrollDragging_);
    }

    bool BeginMainListScroll(int x, int y) {
        RECT track{},thumb{}; int maxScroll=0;
        if (!GetMainListScrollGeometry(track,thumb,maxScroll)) return false;
        if (x < track.left-metrics_.Scale(10) || x >= track.right+metrics_.Scale(2) || y < track.top || y >= track.bottom) return false;
        if (y >= thumb.top && y < thumb.bottom) {
            mainListScrollDragging_=true; scrollDragTrackTop_=track.top;
            scrollDragTrackHeight_=track.bottom-track.top; scrollDragThumbHeight_=thumb.bottom-thumb.top;
            scrollDragThumbOffset_=y-thumb.top; scrollDragMaxScroll_=maxScroll; SetCapture(hwnd_.get());
        } else {
            const int page=std::max(metrics_.Scale(32),static_cast<int>(track.bottom-track.top));
            mainListScrollY_=std::max(0,std::min(mainListScrollY_+(y<thumb.top?-page:page),maxScroll));
            InvalidateRect(hwnd_.get(),nullptr,FALSE);
        }
        return true;
    }

    bool GetAllAppsScrollGeometry(RECT& track, RECT& thumb, int& maxScroll,
                                  RECT* upButton = nullptr, RECT* downButton = nullptr) const {
        track = {}; thumb = {}; maxScroll = 0;
        if (upButton) *upButton = {};
        if (downButton) *downButton = {};
        if (viewMode_ != ViewMode::AllApps || !hwnd_) return false;
        RECT client{};
        if (!GetClientRect(hwnd_.get(), &client)) return false;
        const int userH = metrics_.Scale(Metrics::BASE_USER_HEADER_H);
        const int bottomY = client.bottom - BottomPlaceItemCount() * metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        const int visibleH = bottomY - userH;
        const int contentH = std::max(0, totalAllAppsH_);
        if (visibleH <= 0 || contentH <= visibleH) return false;
        const int buttonW = std::max(14, metrics_.Scale(14));
        const int sbW = buttonW; // rail, thumb and arrow buttons share the same width.
        const int buttonH = std::min(std::max(12, metrics_.Scale(14)), std::max(1, visibleH / 4));
        const int leftEdge = std::min(static_cast<int>(client.right), metrics_.Scale(Metrics::BASE_LEFT_COL_W));
        const int trackTop = userH + buttonH;
        const int trackBottom = bottomY - buttonH;
        track = { std::max(0, leftEdge - sbW), trackTop, leftEdge, trackBottom };
        const int trackH = track.bottom - track.top;
        if (trackH <= 0) return false;
        if (upButton) *upButton = { std::max(0, leftEdge - buttonW), userH, leftEdge, trackTop };
        if (downButton) *downButton = { std::max(0, leftEdge - buttonW), trackBottom, leftEdge, bottomY };
        const int thumbH = std::min(trackH, std::max(metrics_.Scale(20), trackH * visibleH / contentH));
        maxScroll = contentH - visibleH;
        const int boundedScroll = std::max(0, std::min(allAppsScrollY_, maxScroll));
        const int travel = trackH - thumbH;
        const int thumbY = track.top + (maxScroll > 0 ? (travel * boundedScroll / maxScroll) : 0);
        thumb = { track.left, thumbY, track.right, thumbY + thumbH };
        return true;
    }

    void StopAllAppsArrowRepeat(bool releaseCapture) {
        if (hwnd_) KillTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT);
        const bool wasPressed = allAppsScrollArrowPressed_;
        allAppsScrollRepeatDirection_ = 0;
        allAppsScrollArrowPressed_ = false;
        if (releaseCapture && hwnd_ && GetCapture() == hwnd_.get()) ReleaseCapture();
        if (wasPressed && hwnd_) InvalidateRect(hwnd_.get(), nullptr, FALSE); // clear the pressed arrow highlight
    }

    void ScrollAllAppsArrowStep() {
        if (!allAppsScrollRepeatDirection_) return;
        RECT track{}, thumb{}; int maxScroll = 0;
        if (!GetAllAppsScrollGeometry(track, thumb, maxScroll)) {
            KillTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT);
            allAppsScrollRepeatDirection_ = 0;
            return;
        }
        const int oldScroll = allAppsScrollY_;
        const int step = std::max(1, metrics_.Scale(32));
        if (allAppsScrollRepeatDirection_ < 0)
            allAppsScrollY_ = std::max(0, allAppsScrollY_ - step);
        else
            allAppsScrollY_ = std::min(maxScroll, allAppsScrollY_ + step);
        if (allAppsScrollY_ == oldScroll) {
            KillTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT);
            allAppsScrollRepeatDirection_ = 0;
            return;
        }
        InvalidateRect(hwnd_.get(), nullptr, FALSE);
    }

    void BeginAllAppsArrowRepeat(int direction) {
        allAppsScrollArrowPressed_ = true;
        allAppsScrollRepeatDirection_ = direction;
        SetCapture(hwnd_.get());
        ScrollAllAppsArrowStep(); // immediate first line, followed by standard held-button repeat
        if (allAppsScrollRepeatDirection_ &&
            !SetTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT, 400, nullptr)) {
            Wh_Log(L"[AllApps] scrollbar arrow repeat timer unavailable; held repeat disabled");
            allAppsScrollRepeatDirection_ = 0;
        }
    }

    void OnAllAppsArrowRepeatTimer() {
        if (!allAppsScrollArrowPressed_ || !allAppsScrollRepeatDirection_) {
            KillTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT);
            return;
        }
        ScrollAllAppsArrowStep();
        if (allAppsScrollRepeatDirection_ &&
            !SetTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT, 50, nullptr)) {
            Wh_Log(L"[AllApps] scrollbar arrow repeat timer stopped");
            allAppsScrollRepeatDirection_ = 0;
        }
    }

    bool BeginAllAppsScroll(int x, int y) {
        RECT track{}, thumb{}, upButton{}, downButton{}; int maxScroll = 0;
        if (!GetAllAppsScrollGeometry(track, thumb, maxScroll, &upButton, &downButton)) return false;
        const int hitLeft = std::min(static_cast<int>(upButton.left), static_cast<int>(track.left)) - metrics_.Scale(10);
        if (x < hitLeft || x >= track.right + metrics_.Scale(2)) return false;
        const POINT point{ x, y };
        if (PtInRect(&upButton, point)) {
            BeginAllAppsArrowRepeat(-1);
            return true;
        }
        if (PtInRect(&downButton, point)) {
            BeginAllAppsArrowRepeat(1);
            return true;
        }
        if (y < track.top || y >= track.bottom) return false;
        if (y >= thumb.top && y < thumb.bottom) {
            allAppsScrollDragging_ = true;
            scrollDragTrackTop_ = track.top;
            scrollDragTrackHeight_ = track.bottom - track.top;
            scrollDragThumbHeight_ = thumb.bottom - thumb.top;
            scrollDragThumbOffset_ = y - thumb.top;
            scrollDragMaxScroll_ = maxScroll;
            SetCapture(hwnd_.get());
        } else {
            const int page = std::max(metrics_.Scale(32), static_cast<int>(track.bottom - track.top));
            allAppsScrollY_ = std::max(0, std::min(allAppsScrollY_ + (y < thumb.top ? -page : page), maxScroll));
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
        }
        return true;
    }

    void OnMouseWheel(int delta, POINT clientPoint) {
        if (viewMode_ != ViewMode::AllApps && viewMode_ != ViewMode::Normal && viewMode_ != ViewMode::Search) return;
        RECT client{};
        if (!GetClientRect(hwnd_.get(), &client)) return;
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        const int userH = metrics_.Scale(Metrics::BASE_USER_HEADER_H);
        const int bottomY = client.bottom - BottomPlaceItemCount() * metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        if (clientPoint.x < 0 || clientPoint.x >= leftW || clientPoint.y < userH || clientPoint.y >= bottomY) return;
        const int visibleH = bottomY - userH;
        const bool allApps = viewMode_ == ViewMode::AllApps;
        const int maxScroll = std::max(0, (allApps ? totalAllAppsH_ : totalMainListH_) - visibleH);
        if (maxScroll == 0) return;
        const int notches = delta / WHEEL_DELTA;
        if (notches == 0) return;
        if (allApps) allAppsScrollY_ = std::max(0, std::min(allAppsScrollY_ - notches * metrics_.Scale(40), maxScroll));
        else mainListScrollY_ = std::max(0, std::min(mainListScrollY_ - notches * metrics_.Scale(40), maxScroll));
        InvalidateRect(hwnd_.get(), nullptr, FALSE);
    }

    void OnChar(wchar_t ch) {
        if (ch >= 32 && ch != 127) {
            if (searchAllSelected_) searchQuery_.clear();
            searchAllSelected_ = false;
            searchQuery_.push_back(ch);
            mainListScrollY_ = 0;
            viewMode_ = ViewMode::Search;
            FilterSearch();
            // As in the original menu, the best match is preselected so that
            // Enter launches it and the arrow keys walk the other results.
            FocusFirstSearchResult();
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
        }
    }

    void OnKeyDown(UINT vk) {
        const bool control = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

        if (vk == 'A' && control && !searchQuery_.empty()) {
            viewMode_ = ViewMode::Search;
            searchAllSelected_ = true;
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
            return;
        }

        switch (vk) {
            case VK_ESCAPE:
                searchAllSelected_ = false;
                ClearKeyboardFocus();
                if (viewMode_ == ViewMode::Search) {
                    searchQuery_.clear();
                    searchResults_.clear();
                    viewMode_ = ViewMode::Normal;
                    mainListScrollY_ = 0;
                    InvalidateRect(hwnd_.get(), nullptr, FALSE);
                } else {
                    Hide();
                }
                return;

            case VK_BACK:
                if (viewMode_ == ViewMode::Search && !searchQuery_.empty()) {
                    if (searchAllSelected_) {
                        searchQuery_.clear();
                        searchResults_.clear();
                        searchAllSelected_ = false;
                        viewMode_ = ViewMode::Normal;
                        ClearKeyboardFocus();
                        InvalidateRect(hwnd_.get(), nullptr, FALSE);
                        return;
                    }
                    searchQuery_.pop_back();
                    searchAllSelected_ = false;
                    if (searchQuery_.empty()) {
                        viewMode_ = ViewMode::Normal;
                        ClearKeyboardFocus();
                    } else {
                        FilterSearch();
                        FocusFirstSearchResult();
                    }
                    InvalidateRect(hwnd_.get(), nullptr, FALSE);
                }
                return;

            case VK_UP:    MoveFocusVertical(-1); return;
            case VK_DOWN:  MoveFocusVertical(1); return;
            case VK_LEFT:  MoveFocusHorizontal(-1); return;
            case VK_RIGHT: MoveFocusHorizontal(1); return;
            case VK_PRIOR: MoveFocusPage(-1); return;
            case VK_NEXT:  MoveFocusPage(1); return;
            case VK_HOME:  MoveFocusToEdge(false); return;
            case VK_END:   MoveFocusToEdge(true); return;
            case VK_TAB:   MoveFocusToNextArea(shift ? -1 : 1); return;

            case VK_RETURN:
                ActivateKeyboardFocus();
                return;

            case VK_APPS:
                ShowKeyboardContextMenu();
                return;

            case VK_F10:
                if (shift) ShowKeyboardContextMenu();
                return;

            default:
                return;
        }
    }

    // ===================== KEYBOARD NAVIGATION =====================
    // Arrow keys, Tab, Home/End and Page Up/Down move a single highlight over
    // the account header (area 0), the bottom bar (area 1), the app list
    // (area 2), the tile grid (area 3) and the A-Z grid of All apps (area 6).
    // The highlight is stored in the hover fields, so the existing painting
    // code highlights a focused element exactly like a hovered one; only the
    // focus ring is added on top of it.

    int ListItemHeight() const { return metrics_.Scale(Metrics::BASE_APP_ITEM_H); }

    // Walks the rows of the left column that the keyboard can reach. The
    // callback receives (index, top), where `top` is measured from the top of
    // the scrollable area before the scroll offset is applied, and returns
    // false to stop the walk. One extra call is made past the last row, so the
    // same walk also yields the row count and the total content height.
    template <typename Callback>
    void ForEachListRow(Callback callback) const {
        const int itemH = ListItemHeight();
        int index = 0;
        int top = 0;
        if (viewMode_ == ViewMode::Normal) {
            top = metrics_.Scale(8) + metrics_.Scale(24);
            for (size_t i = 0; i < catalog_.MostUsed().size(); ++i) {
                if (!callback(index++, top)) return;
                top += itemH;
            }
            if (!catalog_.RecentlyAdded().empty()) {
                top += metrics_.Scale(8) + metrics_.Scale(24);
                for (size_t i = 0; i < catalog_.RecentlyAdded().size(); ++i) {
                    if (!callback(index++, top)) return;
                    top += itemH;
                }
            }
        } else if (viewMode_ == ViewMode::Search) {
            top = metrics_.Scale(8) + metrics_.Scale(26);
            for (size_t i = 0; i < searchResults_.size(); ++i) {
                if (!callback(index++, top)) return;
                top += itemH;
            }
        } else if (viewMode_ == ViewMode::AllApps) {
            const int headH = metrics_.Scale(Metrics::BASE_SECTION_HEADER_H);
            for (const auto& section : catalog_.Sections()) {
                top += headH;
                for (size_t a = 0; a < section.apps.size(); ++a) {
                    if (!callback(index++, top)) return;
                    top += itemH;
                }
                for (const auto& folder : section.folders) {
                    if (!callback(index++, top)) return; // the folder row itself
                    top += itemH;
                    if (!folder.expanded) continue;
                    for (size_t sub = 0; sub < folder.apps.size(); ++sub) {
                        if (!callback(index++, top)) return;
                        top += itemH;
                    }
                }
            }
        }
        callback(index, top); // one past the last row
    }

    int ListItemCount() const {
        int count = 0;
        ForEachListRow([&](int index, int) { count = index; return true; });
        return count;
    }

    int ListContentHeight() const {
        int height = 0;
        ForEachListRow([&](int, int top) { height = top; return true; });
        return height;
    }

    int ListItemOffset(int index) const {
        int offset = 0;
        ForEachListRow([&](int current, int top) {
            if (current != index) return true;
            offset = top;
            return false;
        });
        return offset;
    }

    // Row closest to a position expressed in content coordinates.
    int ListIndexAtOffset(int contentY) const {
        const int itemH = ListItemHeight();
        const int count = ListItemCount();
        int best = -1;
        int bestDistance = INT_MAX;
        ForEachListRow([&](int index, int top) {
            const int distance = std::abs(top + itemH / 2 - contentY);
            if (distance < bestDistance) { bestDistance = distance; best = index; }
            return true;
        });
        if (best >= count) best = count - 1; // the past-the-end call
        return best;
    }

    // Index of the first row of an All apps section, or -1 when it is empty.
    int FirstRowOfSection(int sectionIndex) const {
        if (sectionIndex < 0) return -1;
        int index = 0;
        int current = 0;
        for (const auto& section : catalog_.Sections()) {
            if (current++ == sectionIndex)
                return (section.apps.empty() && section.folders.empty()) ? -1 : index;
            index += static_cast<int>(section.apps.size());
            for (const auto& folder : section.folders) {
                ++index;
                if (folder.expanded) index += static_cast<int>(folder.apps.size());
            }
        }
        return -1;
    }

    bool ListViewportRect(RECT& out) const {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return false;
        const int leftW = std::min(static_cast<int>(client.right),
                                   metrics_.Scale(Metrics::BASE_LEFT_COL_W));
        const int userH = metrics_.Scale(Metrics::BASE_USER_HEADER_H);
        const int bottomY = static_cast<int>(client.bottom) -
                            BottomPlaceItemCount() * metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        if (leftW <= 0 || bottomY <= userH) return false;
        out = RECT{0, userH, leftW, bottomY};
        return true;
    }

    int CurrentListScroll() const {
        return viewMode_ == ViewMode::AllApps ? allAppsScrollY_ : mainListScrollY_;
    }

    void SetCurrentListScroll(int value) {
        if (viewMode_ == ViewMode::AllApps) allAppsScrollY_ = value;
        else mainListScrollY_ = value;
    }

    bool ListItemRect(int index, RECT& out) const {
        RECT viewport{};
        if (index < 0 || index >= ListItemCount() || !ListViewportRect(viewport)) return false;
        const int top = static_cast<int>(viewport.top) + ListItemOffset(index) - CurrentListScroll();
        out = RECT{viewport.left, top, viewport.right, top + ListItemHeight()};
        return true;
    }

    // Scrolls by the smallest amount that brings a row fully into view.
    void EnsureListItemVisible(int index) {
        RECT viewport{};
        if (index < 0 || !ListViewportRect(viewport)) return;
        const int visibleH = static_cast<int>(viewport.bottom - viewport.top);
        const int maxScroll = std::max(0, ListContentHeight() - visibleH);
        const int top = ListItemOffset(index);
        const int bottom = top + ListItemHeight();
        const int current = CurrentListScroll();
        int scroll = current;
        if (top < scroll) {
            // Scrolling upwards also reveals the heading that sits above the
            // row ("Most used", "Recently added", or the A-Z letter).
            scroll = index > 0 ? ListItemOffset(index - 1) + ListItemHeight() : 0;
        } else if (bottom > scroll + visibleH) {
            scroll = bottom - visibleH;
        }
        scroll = std::max(0, std::min(scroll, maxScroll));
        if (scroll != current) {
            SetCurrentListScroll(scroll);
            if (hwnd_) InvalidateRect(hwnd_.get(), nullptr, FALSE);
        }
    }

    bool PlaceItemRect(int index, RECT& out) const {
        RECT client{};
        if (index < 0 || index > 3 || !hwnd_ || !GetClientRect(hwnd_.get(), &client)) return false;
        const int leftW = std::min(static_cast<int>(client.right),
                                   metrics_.Scale(Metrics::BASE_LEFT_COL_W));
        const int placeH = metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        const int bottomY = static_cast<int>(client.bottom) - BottomPlaceItemCount() * placeH;
        if (BottomPlaceItemCount() == 1) {
            if (index != 3) return false; // only "Back" is visible in All apps
            out = RECT{0, bottomY, leftW, bottomY + placeH};
            return true;
        }
        out = RECT{0, bottomY + index * placeH, leftW, bottomY + (index + 1) * placeH};
        return true;
    }

    // Client rectangle of a tile, shared by hit testing, repainting and
    // keyboard navigation so that all three always agree.
    bool GetTileClientRect(int groupIndex, int tileIndex, RECT& out) const {
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return false;
        const auto& tiles = layout_.groups[groupIndex].tiles;
        if (tileIndex < 0 || tileIndex >= static_cast<int>(tiles.size())) return false;
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_MEDIUM) +
            (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
        const int groupX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X) +
            groupIndex * (groupW + metrics_.Scale(Metrics::BASE_GROUP_GAP));
        const int tilesY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
            metrics_.Scale(Metrics::BASE_GROUP_HEADER_H) + metrics_.Scale(4);
        out = ComputeTileRect(groupX, tilesY, tiles[tileIndex]);
        return true;
    }

    // A tile scrolled out of the grid viewport must not receive the focus.
    bool IsTileRectReachable(const RECT& tile) const {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return false;
        RECT viewport{metrics_.Scale(Metrics::BASE_LEFT_COL_W), 0, client.right, client.bottom};
        RECT intersection{};
        return IntersectRect(&intersection, &tile, &viewport) != FALSE;
    }

    bool FocusTargetRect(int area, int index, int group, RECT& out) const {
        RECT client{};
        switch (area) {
            case 0:
                if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return false;
                out = RECT{0, 0,
                           std::min(static_cast<int>(client.right),
                                    metrics_.Scale(Metrics::BASE_LEFT_COL_W)),
                           metrics_.Scale(Metrics::BASE_USER_HEADER_H)};
                return true;
            case 1: return PlaceItemRect(index, out);
            case 2: return ListItemRect(index, out);
            case 3: return GetTileClientRect(group, index, out);
            case 6: return AlphabetCellRectForSection(group,
                        metrics_.Scale(Metrics::BASE_LEFT_COL_W),
                        metrics_.Scale(Metrics::BASE_USER_HEADER_H), out);
            default: return false;
        }
    }

    void SetKeyboardFocus(int area, int index, int group) {
        const int oldArea = hoverArea_, oldIndex = hoverIndex_, oldGroup = hoverGroup_;
        hoverArea_ = area;
        hoverIndex_ = index;
        hoverGroup_ = group;
        keyboardNav_ = area >= 0;
        if (area == 3) {
            keyboardTileGroup_ = group;
            keyboardTileIndex_ = index;
        }
        if (area == 2) EnsureListItemVisible(index);
        InvalidateHoverTarget(oldArea, oldIndex, oldGroup);
        InvalidateHoverTarget(area, index, group);
    }

    void ClearKeyboardFocus() {
        if (!keyboardNav_) return;
        const int oldArea = hoverArea_, oldIndex = hoverIndex_, oldGroup = hoverGroup_;
        keyboardNav_ = false;
        hoverArea_ = hoverIndex_ = hoverGroup_ = -1;
        InvalidateHoverTarget(oldArea, oldIndex, oldGroup);
    }

    void ResetKeyboardNavigation() {
        keyboardNav_ = false;
        keyboardTileGroup_ = -1;
        keyboardTileIndex_ = -1;
    }

    void FocusFirstSearchResult() {
        if (!searchResults_.empty()) SetKeyboardFocus(2, 0, -1);
        else ClearKeyboardFocus();
    }

    int FirstPlaceIndex() const { return BottomPlaceItemCount() == 1 ? 3 : 0; }

    bool FocusPlace(int index) {
        if (BottomPlaceItemCount() == 1) index = 3; // All apps view: only "Back"
        if (index < 0 || index > 3) return false;
        SetKeyboardFocus(1, index, -1);
        return true;
    }

    bool FocusListRow(int index) {
        const int count = ListItemCount();
        if (count <= 0) return false;
        SetKeyboardFocus(2, std::max(0, std::min(index, count - 1)), -1);
        return true;
    }

    // ---- A-Z grid of All apps (5 columns: #, 0-9, A to Z, globe) ----
    static constexpr int kAlphabetColumns = 5;
    static constexpr int kAlphabetCells = 29;

    static wchar_t AlphabetCellKey(int cell) {
        if (cell == 0) return L'#';
        if (cell == 1) return L'0';
        if (cell >= 2 && cell < 28) return static_cast<wchar_t>(L'A' + cell - 2);
        if (cell == 28) return L'~';
        return 0;
    }

    int AlphabetCellSection(int cell) const {
        const wchar_t key = AlphabetCellKey(cell);
        return key ? FindAllAppsSection(key) : -1;
    }

    int AlphabetCellForSection(int section) const {
        if (section < 0) return -1;
        for (int cell = 0; cell < kAlphabetCells; ++cell)
            if (AlphabetCellSection(cell) == section) return cell;
        return -1;
    }

    bool FocusAlphabetCell(int cell) {
        const int section = AlphabetCellSection(cell);
        if (section < 0) return false; // letters without apps stay unreachable
        SetKeyboardFocus(6, -1, section);
        return true;
    }

    bool FocusFirstAlphabetCell(bool fromEnd = false) {
        for (int i = 0; i < kAlphabetCells; ++i) {
            const int cell = fromEnd ? kAlphabetCells - 1 - i : i;
            if (FocusAlphabetCell(cell)) return true;
        }
        return false;
    }

    bool MoveAlphabetFocus(int dx, int dy) {
        const int step = dx != 0 ? dx : dy * kAlphabetColumns;
        if (step == 0) return false;
        const int current = (hoverArea_ == 6) ? AlphabetCellForSection(hoverGroup_) : -1;
        if (current < 0) return FocusFirstAlphabetCell(step < 0);
        for (int cell = current + step; cell >= 0 && cell < kAlphabetCells; cell += step) {
            // A horizontal move stays on its row.
            if (dx != 0 && cell / kAlphabetColumns != current / kAlphabetColumns) break;
            if (FocusAlphabetCell(cell)) return true;
        }
        return false;
    }

    // ---- tile grid ----
    // Picks the reachable tile that best continues the requested direction:
    // the closest one along the axis of travel, with lateral drift penalised.
    bool MoveTileFocus(int dx, int dy) {
        RECT currentRect{};
        if (hoverArea_ != 3 || !GetTileClientRect(hoverGroup_, hoverIndex_, currentRect))
            return FocusTileFromColumn();
        const int fromX = static_cast<int>(currentRect.left + currentRect.right) / 2;
        const int fromY = static_cast<int>(currentRect.top + currentRect.bottom) / 2;
        int bestGroup = -1, bestTile = -1;
        long long bestScore = 0;
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            const auto& tiles = layout_.groups[gi].tiles;
            for (size_t ti = 0; ti < tiles.size(); ++ti) {
                if (static_cast<int>(gi) == hoverGroup_ && static_cast<int>(ti) == hoverIndex_) continue;
                RECT rect{};
                if (!GetTileClientRect(static_cast<int>(gi), static_cast<int>(ti), rect)) continue;
                if (!IsTileRectReachable(rect)) continue;
                const int centerX = static_cast<int>(rect.left + rect.right) / 2;
                const int centerY = static_cast<int>(rect.top + rect.bottom) / 2;
                const long long along = dx != 0 ? static_cast<long long>(centerX - fromX) * dx
                                                : static_cast<long long>(centerY - fromY) * dy;
                if (along <= 0) continue; // not in the requested direction
                const long long across = dx != 0 ? std::abs(centerY - fromY) : std::abs(centerX - fromX);
                const long long score = along + across * 3;
                if (bestTile < 0 || score < bestScore) {
                    bestScore = score;
                    bestGroup = static_cast<int>(gi);
                    bestTile = static_cast<int>(ti);
                }
            }
        }
        if (bestTile >= 0) {
            SetKeyboardFocus(3, bestTile, bestGroup);
            return true;
        }
        if (dx < 0) return FocusLeftColumnFromTile(); // leaving the grid on its left
        return false;
    }

    // Enters the tile grid from the left column, keeping the vertical position
    // or, when available, the tile the grid was last left from.
    bool FocusTileFromColumn() {
        RECT remembered{};
        if (GetTileClientRect(keyboardTileGroup_, keyboardTileIndex_, remembered) &&
            IsTileRectReachable(remembered)) {
            SetKeyboardFocus(3, keyboardTileIndex_, keyboardTileGroup_);
            return true;
        }
        RECT origin{};
        const int wantedY = FocusTargetRect(hoverArea_, hoverIndex_, hoverGroup_, origin)
            ? static_cast<int>(origin.top + origin.bottom) / 2 : 0;
        int bestGroup = -1, bestTile = -1;
        long long bestScore = 0;
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            const auto& tiles = layout_.groups[gi].tiles;
            for (size_t ti = 0; ti < tiles.size(); ++ti) {
                RECT rect{};
                if (!GetTileClientRect(static_cast<int>(gi), static_cast<int>(ti), rect)) continue;
                if (!IsTileRectReachable(rect)) continue;
                const int centerY = static_cast<int>(rect.top + rect.bottom) / 2;
                const long long score =
                    static_cast<long long>(std::abs(centerY - wantedY)) * 4 + rect.left;
                if (bestTile < 0 || score < bestScore) {
                    bestScore = score;
                    bestGroup = static_cast<int>(gi);
                    bestTile = static_cast<int>(ti);
                }
            }
        }
        if (bestTile < 0) return false;
        SetKeyboardFocus(3, bestTile, bestGroup);
        return true;
    }

    // Returns from the tile grid to the row of the left column at the same height.
    bool FocusLeftColumnFromTile() {
        if (viewMode_ == ViewMode::AllAppsAlphabet) return FocusFirstAlphabetCell();
        RECT tileRect{};
        int wantedY = -1;
        if (GetTileClientRect(hoverGroup_, hoverIndex_, tileRect))
            wantedY = static_cast<int>(tileRect.top + tileRect.bottom) / 2;
        RECT viewport{};
        if (ListItemCount() > 0 && ListViewportRect(viewport)) {
            int index = 0;
            if (wantedY >= 0) {
                const int clampedY = std::max(static_cast<int>(viewport.top),
                    std::min(wantedY, static_cast<int>(viewport.bottom) - 1));
                index = std::max(0, ListIndexAtOffset(clampedY - static_cast<int>(viewport.top) +
                                                      CurrentListScroll()));
            }
            return FocusListRow(index);
        }
        return FocusPlace(FirstPlaceIndex());
    }

    // ---- movement ----
    // First target when navigation starts without a highlight.
    void FocusFirstTarget(int direction) {
        if (viewMode_ == ViewMode::AllAppsAlphabet) {
            if (FocusFirstAlphabetCell(direction < 0)) return;
        }
        const int count = ListItemCount();
        if (count > 0) {
            SetKeyboardFocus(2, direction < 0 ? count - 1 : 0, -1);
            return;
        }
        if (!FocusPlace(direction < 0 ? 3 : FirstPlaceIndex())) FocusTileFromColumn();
    }

    void MoveFocusVertical(int delta) {
        if (!keyboardNav_ || hoverArea_ < 0) {
            FocusFirstTarget(delta);
            return;
        }
        switch (hoverArea_) {
            case 0: // account header: only downwards
                if (delta > 0 && !FocusListRow(0)) FocusPlace(FirstPlaceIndex());
                return;
            case 1: {
                const int next = hoverIndex_ + delta;
                if (BottomPlaceItemCount() > 1 && next >= 0 && next <= 3) {
                    SetKeyboardFocus(1, next, -1);
                    return;
                }
                if (delta < 0) {
                    const int count = ListItemCount();
                    if (count > 0) FocusListRow(count - 1);
                    else SetKeyboardFocus(0, -1, -1);
                }
                return;
            }
            case 2: {
                const int count = ListItemCount();
                const int next = hoverIndex_ + delta;
                if (next >= 0 && next < count) {
                    SetKeyboardFocus(2, next, -1);
                    return;
                }
                if (delta < 0) SetKeyboardFocus(0, -1, -1); // above the list: account header
                else FocusPlace(FirstPlaceIndex());         // below the list: bottom bar
                return;
            }
            case 3: MoveTileFocus(0, delta); return;
            case 6: MoveAlphabetFocus(0, delta); return;
            default: FocusFirstTarget(delta); return;
        }
    }

    void MoveFocusHorizontal(int delta) {
        if (!keyboardNav_ || hoverArea_ < 0) {
            if (delta > 0 && FocusTileFromColumn()) return;
            FocusFirstTarget(1);
            return;
        }
        switch (hoverArea_) {
            case 0:
            case 1:
            case 2:
                if (delta > 0) FocusTileFromColumn(); // step into the tile grid
                return;
            case 3: MoveTileFocus(delta, 0); return;
            case 6: MoveAlphabetFocus(delta, 0); return;
            default: return;
        }
    }

    void MoveFocusPage(int delta) {
        if (keyboardNav_ && hoverArea_ == 3) { // in the grid a page is one row
            MoveTileFocus(0, delta);
            return;
        }
        RECT viewport{};
        const int count = ListItemCount();
        if (count <= 0 || !ListViewportRect(viewport)) {
            MoveFocusVertical(delta);
            return;
        }
        const int rows = std::max(1, static_cast<int>(viewport.bottom - viewport.top) /
                                         std::max(1, ListItemHeight()) - 1);
        const int current = (keyboardNav_ && hoverArea_ == 2) ? hoverIndex_
                                                              : (delta > 0 ? -1 : count);
        const int next = std::max(0, std::min(current + delta * rows, count - 1));
        SetKeyboardFocus(2, next, -1);
    }

    void MoveFocusToEdge(bool last) {
        if (keyboardNav_ && hoverArea_ == 3 && hoverGroup_ >= 0 &&
            hoverGroup_ < static_cast<int>(layout_.groups.size())) {
            const auto& tiles = layout_.groups[hoverGroup_].tiles;
            int best = -1;
            for (size_t ti = 0; ti < tiles.size(); ++ti) {
                if (best < 0) { best = static_cast<int>(ti); continue; }
                const Tile& candidate = tiles[ti];
                const Tile& current = tiles[best];
                const bool better = last
                    ? (candidate.row > current.row ||
                       (candidate.row == current.row && candidate.col > current.col))
                    : (candidate.row < current.row ||
                       (candidate.row == current.row && candidate.col < current.col));
                if (better) best = static_cast<int>(ti);
            }
            if (best >= 0) {
                SetKeyboardFocus(3, best, hoverGroup_);
                return;
            }
        }
        const int count = ListItemCount();
        if (count > 0) {
            SetKeyboardFocus(2, last ? count - 1 : 0, -1);
            return;
        }
        FocusPlace(last ? 3 : FirstPlaceIndex());
    }

    // Tab and Shift+Tab walk the areas of the menu in a fixed order.
    void MoveFocusToNextArea(int delta) {
        constexpr int kStops = 4; // account header, list, bottom bar, tiles
        int current;
        switch (keyboardNav_ ? hoverArea_ : -1) {
            case 0: current = 0; break;
            case 2:
            case 6: current = 1; break;
            case 1: current = 2; break;
            case 3: current = 3; break;
            default: current = delta > 0 ? 0 : kStops; break;
        }
        for (int step = 1; step <= kStops; ++step) {
            const int stop = ((current + delta * step) % kStops + kStops) % kStops;
            switch (stop) {
                case 0:
                    SetKeyboardFocus(0, -1, -1);
                    return;
                case 1:
                    if (viewMode_ == ViewMode::AllAppsAlphabet) {
                        if (FocusFirstAlphabetCell()) return;
                    } else if (FocusListRow(0)) {
                        return;
                    }
                    break;
                case 2:
                    if (FocusPlace(FirstPlaceIndex())) return;
                    break;
                default:
                    if (FocusTileFromColumn()) return;
                    break;
            }
        }
    }

    // ---- activation ----
    // Flyouts opened from the keyboard are anchored to the focused element.
    POINT FocusAnchorPoint() const {
        RECT rect{};
        if (!FocusTargetRect(hoverArea_, hoverIndex_, hoverGroup_, rect)) return POINT{0, 0};
        return POINT{rect.left + (rect.right - rect.left) / 2,
                     rect.top + (rect.bottom - rect.top) / 2};
    }

    void ActivateKeyboardFocus() {
        if (!keyboardNav_ || hoverArea_ < 0) {
            // No highlight: Enter keeps launching the best search match.
            if (viewMode_ == ViewMode::Search && !searchResults_.empty())
                LaunchCatalogEntry(searchResults_[0]);
            return;
        }
        ActivateItem(hoverArea_, hoverIndex_, hoverGroup_, FocusAnchorPoint(), false);
    }

    void ShowKeyboardContextMenu() {
        if (!keyboardNav_ || hoverArea_ < 0) return;
        const POINT anchor = FocusAnchorPoint();
        switch (hoverArea_) {
            case 0: ShowUserMenu(anchor); return;
            case 1:
                if (hoverIndex_ == 0) ShowExplorerJumpList();
                else if (hoverIndex_ == 2) ShowPowerMenu(anchor);
                return;
            case 2:
                if (hoverIndex_ >= 0) ShowAppContextMenu(anchor, hoverIndex_);
                return;
            case 3:
                if (hoverGroup_ >= 0 && hoverIndex_ >= 0)
                    ShowTileContextMenu(anchor, hoverGroup_, hoverIndex_);
                return;
            default: return;
        }
    }

    // ---- focus ring ----
    // Tiles already get the white border of the hover state, which is also the
    // focus look of the original menu; the other areas receive a thin outline
    // on top of their highlight.
    void DrawKeyboardFocusRing(HDC hdc) {
        if (!keyboardNav_ || hoverArea_ < 0 || hoverArea_ == 3) return;
        RECT focus{};
        if (!FocusTargetRect(hoverArea_, hoverIndex_, hoverGroup_, focus)) return;
        if (hoverArea_ == 2 || hoverArea_ == 6) {
            // Rows and A-Z cells scroll: clip the outline to the visible area.
            RECT viewport{}, visible{};
            if (!ListViewportRect(viewport)) return;
            if (!IntersectRect(&visible, &focus, &viewport)) return;
            focus = visible;
        }
        const int thickness = std::max(1, metrics_.Scale(1));
        if (focus.right - focus.left <= 2 * thickness ||
            focus.bottom - focus.top <= 2 * thickness) return;
        UniquePen pen(CreatePen(PS_SOLID, thickness, RGB(255, 255, 255)));
        if (!pen) return;
        HBRUSH hollow = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
        ScopedSelectObject selectPen(hdc, pen.get());
        ScopedSelectObject selectBrush(hdc, hollow);
        const int inset = thickness; // keep the outline inside the element
        Rectangle(hdc, focus.left + inset, focus.top + inset,
                  focus.right - inset, focus.bottom - inset);
    }

    void FilterSearch() {
        searchResults_.clear();
        if (searchQuery_.empty()) return;
        std::wstring q = searchQuery_;
        std::transform(q.begin(), q.end(), q.begin(), ::towlower);

        for (const auto& a : catalog_.Apps()) {
            std::wstring name = a.name;
            std::transform(name.begin(), name.end(), name.begin(), ::towlower);
            if (name.find(q) != std::wstring::npos) {
                searchResults_.push_back(a);
            }
        }
    }

    // ===================== DOUBLE-BUFFERED RENDERING =====================
    void Render(HDC hdc, const RECT& rcPaint) {
        RECT rcClient{};
        GetClientRect(hwnd_.get(), &rcClient);

        const int width = static_cast<int>(rcClient.right);
        const int height = static_cast<int>(rcClient.bottom);
        if (width <= 0 || height <= 0) return;
        if (!renderDc_) renderDc_.reset(CreateCompatibleDC(hdc));
        if (!renderDc_) return;
        if (!renderBitmap_ || renderBufferWidth_ != width || renderBufferHeight_ != height) {
            renderBitmap_.reset(CreateCompatibleBitmap(hdc, width, height));
            if (!renderBitmap_) { renderBufferWidth_ = renderBufferHeight_ = 0; return; }
            renderBufferWidth_ = width;
            renderBufferHeight_ = height;
        }
        ScopedSelectObject sBmp(renderDc_.get(), renderBitmap_.get());
        ScopedSaveDC savedDc(renderDc_.get());
        IntersectClipRect(renderDc_.get(), rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);

        // GDI solid fills are cheaper than creating GDI+ Graphics objects for
        // each background on every hover/live-tile frame.
        UniqueBrush background(CreateSolidBrush(RGB(24, 24, 24)));
        UniqueBrush leftBackground(CreateSolidBrush(RGB(24, 24, 24)));
        UniqueBrush borderBrush(CreateSolidBrush(RGB(48, 48, 48)));
        FillRect(renderDc_.get(), &rcClient, background.get());

        int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        RECT rcLeft = { 0, 0, leftW, rcClient.bottom };
        FillRect(renderDc_.get(), &rcLeft, leftBackground.get());
        FrameRect(renderDc_.get(), &rcClient, borderBrush.get());

        ScopedBkMode sBk(renderDc_.get(), TRANSPARENT);
        DrawLeftColumn(renderDc_.get(), rcLeft);

        RECT rcRight = { leftW, 0, rcClient.right, rcClient.bottom };
        DrawTiles(renderDc_.get(), rcRight);
        DrawKeyboardFocusRing(renderDc_.get());
        DrawDragPreview(renderDc_.get());
        if (!savedDc) SelectClipRgn(renderDc_.get(), nullptr);

        BitBlt(hdc, rcPaint.left, rcPaint.top,
               rcPaint.right - rcPaint.left, rcPaint.bottom - rcPaint.top,
               renderDc_.get(), rcPaint.left, rcPaint.top, SRCCOPY);
    }

    bool DrawCircularProfilePhoto(HDC hdc, Gdiplus::Bitmap* source,
                                  int x, int y, int size) const {
        if (!hdc || !source || source->GetLastStatus() != Gdiplus::Ok || size <= 0) return false;
        const UINT imageW = source->GetWidth(), imageH = source->GetHeight();
        if (!imageW || !imageH || imageW > static_cast<UINT>(INT_MAX) ||
            imageH > static_cast<UINT>(INT_MAX)) return false;
        constexpr int kSupersample = 4;
        const int highSize = size * kSupersample;
        Gdiplus::Bitmap avatar(highSize, highSize, PixelFormat32bppARGB);
        if (avatar.GetLastStatus() != Gdiplus::Ok) return false;
        Gdiplus::Graphics graphics(&avatar);
        Gdiplus::GraphicsPath clip;
        if (graphics.GetLastStatus() != Gdiplus::Ok ||
            graphics.Clear(Gdiplus::Color(0, 0, 0, 0)) != Gdiplus::Ok) return false;
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        if (clip.AddEllipse(0.5f, 0.5f, static_cast<REAL>(highSize - 1),
                            static_cast<REAL>(highSize - 1)) != Gdiplus::Ok ||
            graphics.SetClip(&clip, Gdiplus::CombineModeReplace) != Gdiplus::Ok) return false;
        const UINT cropSide = (std::min)(imageW, imageH);
        const INT cropX = static_cast<INT>((imageW - cropSide) / 2);
        const INT cropY = static_cast<INT>((imageH - cropSide) / 2);
        const bool copied = graphics.DrawImage(source, Gdiplus::Rect(0, 0, highSize, highSize),
            cropX, cropY, static_cast<INT>(cropSide), static_cast<INT>(cropSide),
            Gdiplus::UnitPixel) == Gdiplus::Ok;
        graphics.ResetClip();
        if (!copied) return false;
        Gdiplus::Pen border(Gdiplus::Color(220, 255, 255, 255),
            (std::max<REAL>)(1.0f, static_cast<REAL>(metrics_.dpi) / 96.0f) * kSupersample);
        if (graphics.DrawEllipse(&border, 2.0f, 2.0f,
                                 static_cast<REAL>(highSize - 4),
                                 static_cast<REAL>(highSize - 4)) != Gdiplus::Ok) return false;
        Gdiplus::Graphics target(hdc);
        if (target.GetLastStatus() != Gdiplus::Ok) return false;
        target.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        target.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        return target.DrawImage(&avatar, Gdiplus::Rect(x, y, size, size),
            0, 0, highSize, highSize, Gdiplus::UnitPixel) == Gdiplus::Ok;
    }

    void DrawLeftColumn(HDC hdc, const RECT& rcLeft) {
        int leftW = rcLeft.right - rcLeft.left;
        int userH = metrics_.Scale(Metrics::BASE_USER_HEADER_H);

        // 1. User header (top)
        RECT rcUser = { 0, 0, leftW, userH };
        RECT paintClip{}, headerIntersection{};
        const bool headerNeedsPaint = GetClipBox(hdc, &paintClip) != ERROR &&
                                      IntersectRect(&headerIntersection, &paintClip, &rcUser);
        if (hoverArea_ == 0) {
            UniqueBrush brHov(CreateSolidBrush(RGB(40, 40, 40)));
            FillRect(hdc, &rcUser, brHov.get());
        }

        // Real account photo (loaded off the UI thread). If the Shell exposes no
        // valid photo, the bundled fallback is kept without blocking the menu.
        const int avSize = metrics_.Scale(32);
        const int avX = metrics_.Scale(16);
        const int avY = (userH - avSize) / 2;
        if (headerNeedsPaint) {
            bool drewProfilePhoto = false;
            if (g_profilePhoto && g_profilePhoto->GetLastStatus() == Gdiplus::Ok)
                drewProfilePhoto = DrawCircularProfilePhoto(hdc, g_profilePhoto.get(), avX, avY, avSize);
            if (!drewProfilePhoto && g_defaultProfileAvatar)
                drewProfilePhoto = DrawCircularProfilePhoto(hdc, g_defaultProfileAvatar.get(), avX, avY, avSize);
            // If GDI+ cannot render either source, leave the avatar area blank rather
            // than substitute the old hand-drawn approximation.
        }

        // User name: resolved once by ProfilePhotoWorker (a domain lookup must
        // never run inside WM_PAINT); only the cached string is drawn here.
        // Privacy mode avoids even reading the real name.
        const wchar_t* userNameToDisplay = Ui(UiText::DefaultUser);
        if (!settings_.privacyMode && !accountDisplayName_.empty())
            userNameToDisplay = accountDisplayName_.c_str();

        RECT rcName = { avX + avSize + metrics_.Scale(12), 0, leftW - metrics_.Scale(8), userH };
        bool drewUserName = false;
        if (headerNeedsPaint) {
        {
            Gdiplus::Graphics graphics(hdc);
            Gdiplus::FontFamily family(L"Segoe UI");
            Gdiplus::Font font(&family, static_cast<REAL>(metrics_.Scale(13)),
                               Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
            Gdiplus::StringFormat format;
            Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
            format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
            format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
            graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
            if (graphics.GetLastStatus() == Gdiplus::Ok && family.GetLastStatus() == Gdiplus::Ok &&
                font.GetLastStatus() == Gdiplus::Ok) {
                Gdiplus::RectF layout(static_cast<REAL>(rcName.left), 0.0f,
                    static_cast<REAL>(rcName.right - rcName.left), static_cast<REAL>(userH));
                drewUserName = graphics.DrawString(userNameToDisplay, -1, &font, layout, &format, &textBrush) == Gdiplus::Ok;
            }
        }
        if (!drewUserName) {
            ScopedSelectObject sFont(hdc, fontBold_.get());
            ScopedTextColor sCol(hdc, RGB(255, 255, 255));
            DrawTextW(hdc, userNameToDisplay, -1, &rcName, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
        }

        // 2. Bottom bar: 1507 order (Explorer, Settings, Power, All apps).
        int placeH = metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        int bottomH = BottomPlaceItemCount() * placeH;
        int bottomY = rcLeft.bottom - bottomH;

        const wchar_t* places[] = {
            Ui(UiText::Explorer),
            Ui(UiText::Settings),
            Ui(UiText::Power),
            ((viewMode_ == ViewMode::AllApps || viewMode_ == ViewMode::AllAppsAlphabet) ? Ui(UiText::Back) : Ui(UiText::AllApps))
        };

        const bool allAppsView = viewMode_ == ViewMode::AllApps || viewMode_ == ViewMode::AllAppsAlphabet;
        for (int i = 0; i < 4; ++i) {
            if (allAppsView && i < 3) continue;
            const int rowIndex = allAppsView ? 0 : i;
            RECT rcItem = { 0, bottomY + rowIndex * placeH, leftW, bottomY + (rowIndex + 1) * placeH };
            if (hoverArea_ == 1 && hoverIndex_ == i) {
                UniqueBrush brHov(CreateSolidBrush(RGB(48, 48, 48)));
                FillRect(hdc, &rcItem, brHov.get());
            }

            // Draw the stylized glyph of the place
            DrawPlaceGlyph(hdc, i, rcItem);

            RECT rcText = { metrics_.Scale(52), rcItem.top,
                leftW - metrics_.Scale(i == 0 ? 30 : 8), rcItem.bottom };
            ScopedSelectObject sF(hdc, fontNormal_.get());
            ScopedTextColor sC(hdc, RGB(255, 255, 255));
            DrawTextW(hdc, places[i], -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            if (i == 0)
                DrawExplorerJumpChevron(hdc, rcItem);
        }

        // 3. Scrollable middle section (Most used / All apps / Search)
        RECT rcScroll = { 0, userH, leftW, bottomY };
        UniqueRegion rgnClip(CreateRectRgn(rcScroll.left, rcScroll.top, rcScroll.right, rcScroll.bottom));
        SelectClipRgn(hdc, rgnClip.get());

        if (viewMode_ == ViewMode::Normal) {
            DrawNormalAppList(hdc, rcScroll);
        } else if (viewMode_ == ViewMode::AllApps) {
            DrawAllApps(hdc, rcScroll);
        } else if (viewMode_ == ViewMode::AllAppsAlphabet) {
            DrawAllAppsAlphabet(hdc, rcScroll);
        } else if (viewMode_ == ViewMode::Search) {
            DrawSearchResults(hdc, rcScroll);
        }

        SelectClipRgn(hdc, nullptr);
    }

    void DrawExplorerJumpChevron(HDC hdc, const RECT& rcItem) {
        Gdiplus::Graphics graphics(hdc);
        if (graphics.GetLastStatus() != Gdiplus::Ok) return;
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::Pen pen(Gdiplus::Color(255, 215, 215, 215),
                         std::max<REAL>(1.25f, static_cast<REAL>(metrics_.dpi) / 96.0f));
        pen.SetStartCap(Gdiplus::LineCapRound);
        pen.SetEndCap(Gdiplus::LineCapRound);
        const REAL x = static_cast<REAL>(rcItem.right - metrics_.Scale(15));
        const REAL y = static_cast<REAL>(rcItem.top) +
                       static_cast<REAL>(rcItem.bottom - rcItem.top) * 0.5f;
        const REAL u = static_cast<REAL>(metrics_.Scale(16)) / 16.0f;
        Gdiplus::PointF points[] = {
            {x - 2.5f * u, y - 4.0f * u},
            {x + 1.5f * u, y},
            {x - 2.5f * u, y + 4.0f * u}
        };
        graphics.DrawLines(&pen, points, ARRAYSIZE(points));
    }

    void DrawPlaceGlyph(HDC hdc, int index, const RECT& rcItem) {
        const REAL x = static_cast<REAL>(metrics_.Scale(18));
        const REAL sz = static_cast<REAL>(metrics_.Scale(16));
        const REAL u = sz / 16.0f;
        const REAL rowHeight = static_cast<REAL>(rcItem.bottom - rcItem.top);
        const REAL y = static_cast<REAL>(rcItem.top) + (rowHeight - sz) / 2.0f;
        Gdiplus::Graphics graphics(hdc);
        if (graphics.GetLastStatus() != Gdiplus::Ok) return;
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        Gdiplus::Pen pen(Gdiplus::Color(255, 230, 230, 230),
            std::max<REAL>(1.25f, static_cast<REAL>(metrics_.dpi) / 96.0f));
        pen.SetStartCap(Gdiplus::LineCapRound);
        pen.SetEndCap(Gdiplus::LineCapRound);
        pen.SetLineJoin(Gdiplus::LineJoinRound);

        if (index == 3) { // All apps (list glyph) or Back (arrow glyph only)
            if (viewMode_ == ViewMode::AllApps || viewMode_ == ViewMode::AllAppsAlphabet) {
                // Every coordinate is expressed on the same 16 px grid as the glyph.
                const REAL midY = y + 8.0f * u;
                graphics.DrawLine(&pen, x + 14.0f * u, midY, x + 2.25f * u, midY);
                graphics.DrawLine(&pen, x + 2.25f * u, midY, x + 7.7f * u, y + 2.9f * u);
                graphics.DrawLine(&pen, x + 2.25f * u, midY, x + 7.7f * u, y + 13.1f * u);
            } else {
                Gdiplus::SolidBrush bullets(Gdiplus::Color(255, 230, 230, 230));
                for (int row = 0; row < 3; ++row) {
                    const REAL ly = y + 2.0f * u + row * (sz / 2.0f - u);
                    graphics.FillEllipse(&bullets, x + 0.5f * u, ly, 2.5f * u, 2.5f * u);
                    graphics.DrawLine(&pen, x + 6.0f * u, ly + 1.25f * u,
                                      x + sz, ly + 1.25f * u);
                }
            }
        } else if (index == 2) { // Power
            graphics.DrawArc(&pen, x + 2.0f * u, y + 2.0f * u,
                             sz - 4.0f * u, sz - 4.0f * u, 315.0f, 270.0f);
            graphics.DrawLine(&pen, x + sz / 2.0f, y + 1.0f * u,
                              x + sz / 2.0f, y + sz / 2.0f + 0.5f * u);
        } else if (index == 1) { // Settings: geometric gear
            Gdiplus::PointF teeth[16];
            const REAL cx = x + sz / 2.0f, cy = y + sz / 2.0f;
            for (int i = 0; i < 16; ++i) {
                const REAL angle = static_cast<REAL>(i) * 3.14159265f / 8.0f - 1.5707963f;
                const REAL radius = (i % 2 == 0) ? sz * 0.46f : sz * 0.34f;
                teeth[i] = {cx + std::cos(angle) * radius, cy + std::sin(angle) * radius};
            }
            Gdiplus::GraphicsPath gear;
            gear.AddPolygon(teeth, ARRAYSIZE(teeth));
            graphics.DrawPath(&pen, &gear);
            graphics.DrawEllipse(&pen, cx - sz * 0.13f, cy - sz * 0.13f, sz * 0.26f, sz * 0.26f);
        } else if (index == 0) { // File Explorer: folder with front flap
            Gdiplus::PointF folderPts[] = {
                {x + 0.5f * u,  y + 1.5f * u},
                {x + 6.5f * u,  y + 1.5f * u},
                {x + 8.0f * u,  y + 3.0f * u},
                {x + 15.5f * u, y + 3.0f * u},
                {x + 15.5f * u, y + 14.5f * u},
                {x + 0.5f * u,  y + 14.5f * u}
            };
            Gdiplus::GraphicsPath folder;
            folder.AddPolygon(folderPts, ARRAYSIZE(folderPts));
            graphics.DrawPath(&pen, &folder);

            Gdiplus::PointF archPts[] = {
                {x + 2.7f * u,  y + 8.0f * u},
                {x + 13.3f * u, y + 8.0f * u},
                {x + 13.3f * u, y + 14.5f * u},
                {x + 11.5f * u, y + 14.5f * u},
                {x + 11.5f * u, y + 9.8f * u},
                {x + 4.5f * u,  y + 9.8f * u},
                {x + 5.3f * u,  y + 14.5f * u},
                {x + 2.7f * u,  y + 14.5f * u}
            };
            Gdiplus::GraphicsPath arch;
            arch.AddPolygon(archPts, ARRAYSIZE(archPts));
            Gdiplus::SolidBrush archBrush(Gdiplus::Color(255, 230, 230, 230));
            graphics.FillPath(&archBrush, &arch);
        }
    }

    void DrawNormalAppList(HDC hdc, const RECT& rcArea) {
        const int itemH = metrics_.Scale(Metrics::BASE_APP_ITEM_H);
        int curY = rcArea.top + metrics_.Scale(8) - mainListScrollY_;
        int idx = 0;
        auto drawHeader = [&](const wchar_t* text) {
            RECT r{metrics_.Scale(16), curY, rcArea.right, curY + metrics_.Scale(22)};
            if (curY + metrics_.Scale(22) >= rcArea.top && curY <= rcArea.bottom) {
                ScopedSelectObject sf(hdc, fontSmall_.get());
                ScopedTextColor sc(hdc, RGB(160,160,160));
                DrawTextW(hdc, text, -1, &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            }
            curY += metrics_.Scale(24);
        };
        auto drawApp = [&](const AppEntry& app) {
            if (curY + itemH >= rcArea.top && curY <= rcArea.bottom) {
                RECT item{0, curY, rcArea.right, curY + itemH};
                if (hoverArea_ == 2 && hoverIndex_ == idx) {
                    UniqueBrush hover(CreateSolidBrush(RGB(40,40,40))); FillRect(hdc,&item,hover.get());
                }
                const int iconSize=metrics_.Scale(Metrics::BASE_ICON_SIZE);
                DrawAppIconOrFallback(hdc,app.hIcon,metrics_.Scale(16),curY+(itemH-iconSize)/2,iconSize);
                RECT text{metrics_.Scale(52),curY,rcArea.right-metrics_.Scale(8),curY+itemH};
                ScopedSelectObject sf(hdc,fontNormal_.get()); ScopedTextColor sc(hdc,RGB(255,255,255));
                DrawTextW(hdc,app.name.c_str(),-1,&text,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
            }
            curY += itemH; ++idx;
        };
        drawHeader(Ui(UiText::MostUsed));
        for (const auto& app : catalog_.MostUsed()) drawApp(app);
        if (!catalog_.RecentlyAdded().empty()) {
            curY += metrics_.Scale(8);
            drawHeader(Ui(UiText::RecentlyAdded));
            for (const auto& app : catalog_.RecentlyAdded()) drawApp(app);
        }
        const int areaTop = static_cast<int>(rcArea.top);
        const int visibleHeight = static_cast<int>(rcArea.bottom - rcArea.top);
        totalMainListH_ = std::max(0, curY + mainListScrollY_ - areaTop);
        const int maxScroll = std::max(0, totalMainListH_ - visibleHeight);
        mainListScrollY_ = std::max(0,std::min(mainListScrollY_,maxScroll));
        DrawMainListScrollbar(hdc, rcArea);
    }

    int FindAllAppsSection(wchar_t key) const {
        int i = 0;
        for (const auto& sec : catalog_.Sections()) { if (sec.letter == key) return i; ++i; }
        return -1;
    }

    void DrawAllAppsAlphabet(HDC hdc, const RECT& rcArea) {
        const int pad = metrics_.Scale(10);
        const int cellW = std::max(1, (static_cast<int>(rcArea.right) - 2 * pad) / 5);
        const int cellH = metrics_.Scale(48);
        const int top = static_cast<int>(rcArea.top) + metrics_.Scale(10);
        const int digitSection = FindAllAppsSection(L'0');
        const int hashSection = FindAllAppsSection(L'#');
        const int globeSection = FindAllAppsSection(L'~');
        auto drawCell = [&](int n, const wchar_t* label, int target, bool globe = false) {
            const int col = n % 5, row = n / 5;
            RECT r{pad + col * cellW, top + row * cellH, pad + (col + 1) * cellW, top + (row + 1) * cellH};
            if (!RectVisible(hdc, &r)) return;
            const bool active = target >= 0;
            if (hoverArea_ == 6 && hoverGroup_ == target && active) {
                UniqueBrush b(CreateSolidBrush(RGB(52,52,52))); FillRect(hdc, &r, b.get());
            }
            ScopedSelectObject f(hdc, fontHeader_.get());
            ScopedTextColor c(hdc, active ? RGB(255,255,255) : RGB(100,100,100));
            if (globe) {
                Gdiplus::Graphics g(hdc); g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                Gdiplus::Pen pen(Gdiplus::Color(255,active?255:100,active?255:100,active?255:100), std::max<REAL>(1.0f, static_cast<REAL>(metrics_.dpi) / 96.0f));
                REAL d = static_cast<REAL>(metrics_.Scale(20));
                REAL x = static_cast<REAL>(r.left + (cellW - d) / 2), y = static_cast<REAL>(r.top + (cellH - d) / 2);
                g.DrawEllipse(&pen,x,y,d,d); g.DrawEllipse(&pen,x+d*0.32f,y,d*0.36f,d);
                g.DrawLine(&pen,x+1,y+d*0.5f,x+d-1,y+d*0.5f);
            } else DrawTextW(hdc, label, -1, &r, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        };
        int n = 0;
        drawCell(n++, L"#", hashSection);
        drawCell(n++, L"0-9", digitSection);
        for (wchar_t ch=L'A'; ch<=L'Z'; ++ch) { wchar_t text[2]{ch,0}; drawCell(n++,text,FindAllAppsSection(ch)); }
        drawCell(n++, L"", globeSection, true);
    }

    void DrawAllApps(HDC hdc, const RECT& rcArea) {
        int curY = rcArea.top - allAppsScrollY_;
        int itemH = metrics_.Scale(Metrics::BASE_APP_ITEM_H);
        int totalH = 0;
        int renderIdx = 0;
        int sectionIndex = 0;

        for (const auto& sec : catalog_.Sections()) {
            // Alpha/numeric section headings stay white; a subtle row highlight supplies hover feedback.
            int headH = metrics_.Scale(Metrics::BASE_SECTION_HEADER_H);
            if (curY + headH >= rcArea.top && curY <= rcArea.bottom) {
                RECT rcH = { 0, curY, rcArea.right, curY + headH };
                if (hoverArea_ == 5 && hoverGroup_ == sectionIndex) {
                    UniqueBrush brHover(CreateSolidBrush(RGB(40, 40, 40)));
                    FillRect(hdc, &rcH, brHover.get());
                }
                rcH.left = metrics_.Scale(16);
                const wchar_t* label = sec.letter == L'0' ? L"0-9" :
                                       (sec.letter == L'#' ? L"#" : (sec.letter == L'~' ? L"◎" : nullptr));
                wchar_t letterText[2] = { sec.letter, 0 };
                ScopedSelectObject sF(hdc, fontHeader_.get());
                ScopedTextColor sC(hdc, RGB(255, 255, 255));
                DrawTextW(hdc, label ? label : letterText, -1, &rcH, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            }
            curY += headH;
            totalH += headH;

            // Loose apps
            for (const auto& a : sec.apps) {
                if (curY + itemH >= rcArea.top && curY <= rcArea.bottom) {
                    RECT rcItem = { 0, curY, rcArea.right, curY + itemH };
                    if (hoverArea_ == 2 && hoverIndex_ == renderIdx) {
                        UniqueBrush brHov(CreateSolidBrush(RGB(40, 40, 40)));
                        FillRect(hdc, &rcItem, brHov.get());
                    }
                    int icSz = metrics_.Scale(Metrics::BASE_ICON_SIZE);
                    int icY = curY + (itemH - icSz) / 2;
                    DrawAppIconOrFallback(hdc,a.hIcon,metrics_.Scale(16),icY,icSz);
                    RECT rcText = { metrics_.Scale(52), curY, rcArea.right - metrics_.Scale(8), curY + itemH };
                    ScopedSelectObject sF(hdc, fontNormal_.get());
                    ScopedTextColor sC(hdc, RGB(255, 255, 255));
                    DrawTextW(hdc, a.name.c_str(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                }
                curY += itemH;
                totalH += itemH;
                renderIdx++;
            }

            // Folders
            for (const auto& f : sec.folders) {
                if (curY + itemH >= rcArea.top && curY <= rcArea.bottom) {
                    RECT rcF = { 0, curY, rcArea.right, curY + itemH };
                    if (hoverArea_ == 2 && hoverIndex_ == renderIdx) {
                        UniqueBrush brHov(CreateSolidBrush(RGB(40, 40, 40)));
                        FillRect(hdc, &rcF, brHov.get());
                    }
                    // Use the stock Shell folder glyph rather than an empty outline.
                    int icSz = metrics_.Scale(Metrics::BASE_ICON_SIZE);
                    int icY = curY + (itemH - icSz) / 2;
                    if (!g_stockFolderIcon) { SHSTOCKICONINFO si{}; si.cbSize=sizeof(si); if (SUCCEEDED(SHGetStockIconInfo(SIID_FOLDER,SHGSI_ICON|SHGSI_LARGEICON,&si))) g_stockFolderIcon=si.hIcon; }
                    if (g_stockFolderIcon) DrawIconEx(hdc,metrics_.Scale(16),icY,g_stockFolderIcon,icSz,icSz,0,nullptr,DI_NORMAL);

                    RECT rcText = { metrics_.Scale(52), curY, rcArea.right - metrics_.Scale(24), curY + itemH };
                    ScopedSelectObject sF(hdc, fontNormal_.get());
                    ScopedTextColor sC(hdc, RGB(255, 255, 255));
                    DrawTextW(hdc, f.name.c_str(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

                    // Chevron (▾ when expanded, ▸ when collapsed)
                    RECT rcChev = { rcArea.right - metrics_.Scale(20), curY, rcArea.right - metrics_.Scale(4), curY + itemH };
                    DrawTextW(hdc, f.expanded ? L"\u25BE" : L"\u25B8", -1, &rcChev, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                curY += itemH;
                totalH += itemH;
                renderIdx++;

                // Child apps when the folder is expanded
                if (f.expanded) {
                    for (const auto& sub : f.apps) {
                        if (curY + itemH >= rcArea.top && curY <= rcArea.bottom) {
                            RECT rcSub = { 0, curY, rcArea.right, curY + itemH };
                            if (hoverArea_ == 2 && hoverIndex_ == renderIdx) {
                                UniqueBrush brHov(CreateSolidBrush(RGB(40, 40, 40)));
                                FillRect(hdc, &rcSub, brHov.get());
                            }
                            int icSz = metrics_.Scale(20);
                            int icY = curY + (itemH - icSz) / 2;
                            DrawAppIconOrFallback(hdc,sub.hIcon,metrics_.Scale(28),icY,icSz);
                            RECT rcText = { metrics_.Scale(58), curY, rcArea.right - metrics_.Scale(8), curY + itemH };
                            ScopedSelectObject sF(hdc, fontNormal_.get());
                            ScopedTextColor sC(hdc, RGB(255, 255, 255));
                            DrawTextW(hdc, sub.name.c_str(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                        }
                        curY += itemH;
                        totalH += itemH;
                        renderIdx++;
                    }
                }
            }
            ++sectionIndex;
        }
        totalAllAppsH_ = totalH;

        // Scrollbar with a proportional thumb, clickable paging and dragging.
        // Updated from the column's actual viewport, also after resize/DPI changes.
        const int visibleH = rcArea.bottom - rcArea.top;
        const int currentMaxScroll = std::max(0, totalAllAppsH_ - visibleH);
        allAppsScrollY_ = std::max(0, std::min(allAppsScrollY_, currentMaxScroll));
        RECT rcTrack{}, rcThumb{}, rcUpButton{}, rcDownButton{}; int maxScroll = 0;
        if (GetAllAppsScrollGeometry(rcTrack, rcThumb, maxScroll, &rcUpButton, &rcDownButton)) {
            // Rail and buttons share one flat dark background: no seams between
            // the arrow buttons and the track.
            UniqueBrush rail(CreateSolidBrush(kScrollRail));
            RECT rcBar{ rcTrack.left, rcUpButton.top, rcTrack.right, rcDownButton.bottom };
            FillRect(hdc, &rcBar, rail.get());
            const bool upDisabled = allAppsScrollY_ <= 0;
            const bool downDisabled = allAppsScrollY_ >= maxScroll;
            const bool upPressed = allAppsScrollArrowPressed_ && allAppsScrollRepeatDirection_ < 0;
            const bool downPressed = allAppsScrollArrowPressed_ && allAppsScrollRepeatDirection_ > 0;
            DrawScrollArrow(hdc, rcUpButton, true,
                upDisabled ? kScrollArrowDim : upPressed ? kScrollArrowHot : kScrollArrow);
            DrawScrollArrow(hdc, rcDownButton, false,
                downDisabled ? kScrollArrowDim : downPressed ? kScrollArrowHot : kScrollArrow);
            DrawScrollThumb(hdc, rcThumb, allAppsScrollDragging_);
        }
    }

    void UpdateDragPreview(int x, int y) {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return;
        const int width = metrics_.Scale(190), height = metrics_.Scale(58), gap = metrics_.Scale(12);
        RECT next{x + gap, y + gap, x + gap + width, y + gap + height};
        if (next.right > client.right) OffsetRect(&next, -(next.right - client.right), 0);
        if (next.bottom > client.bottom) OffsetRect(&next, 0, -(next.bottom - client.bottom));
        if (next.left < 0) OffsetRect(&next, -next.left, 0);
        if (next.top < 0) OffsetRect(&next, 0, -next.top);
        if (EqualRect(&next, &dragPreviewRect_)) return;
        RECT old = dragPreviewRect_;
        dragPreviewRect_ = next;
        dragPreviewPoint_ = {x, y};
        if (old.right > old.left && old.bottom > old.top) InvalidateRect(hwnd_.get(), &old, FALSE);
        InvalidateRect(hwnd_.get(), &dragPreviewRect_, FALSE);
    }

    void ClearDragPreview() {
        if (dragPreviewRect_.right > dragPreviewRect_.left && dragPreviewRect_.bottom > dragPreviewRect_.top && hwnd_)
            InvalidateRect(hwnd_.get(), &dragPreviewRect_, FALSE);
        dragPreviewRect_ = {};
    }

    void DrawDragPreview(HDC hdc) {
        // Only an app dragged out of the list still needs the floating card: a
        // tile drag now lifts the tile itself.
        if (dragPreviewRect_.right <= dragPreviewRect_.left || dragPreviewRect_.bottom <= dragPreviewRect_.top ||
            !appDragging_) return;
        const int radius = metrics_.Scale(8);
        UniqueBrush fill(CreateSolidBrush(RGB(38, 43, 50)));
        UniquePen outline(CreatePen(PS_SOLID, std::max(1, metrics_.Scale(1)), RGB(100, 165, 220)));
        ScopedSelectObject sBrush(hdc, fill.get());
        ScopedSelectObject sPen(hdc, outline.get());
        RoundRect(hdc, dragPreviewRect_.left, dragPreviewRect_.top, dragPreviewRect_.right,
            dragPreviewRect_.bottom, radius, radius);
        HICON icon = draggedApp_.hIcon;
        const std::wstring* label = &draggedApp_.name;
        const int iconSize = metrics_.Scale(36);
        const int pad = metrics_.Scale(10);
        if (icon) DrawIconEx(hdc, dragPreviewRect_.left + pad,
            dragPreviewRect_.top + (dragPreviewRect_.bottom - dragPreviewRect_.top - iconSize) / 2,
            icon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
        RECT textRect{dragPreviewRect_.left + pad + iconSize + metrics_.Scale(8), dragPreviewRect_.top,
            dragPreviewRect_.right - pad, dragPreviewRect_.bottom};
        DrawTextW(hdc, label->c_str(), -1, &textRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    }

    void DrawSearchResults(HDC hdc, const RECT& rcArea) {
        int curY = rcArea.top + metrics_.Scale(8) - mainListScrollY_;
        int itemH = metrics_.Scale(Metrics::BASE_APP_ITEM_H);

        RECT rcPrompt = { metrics_.Scale(16), curY, rcArea.right, curY + metrics_.Scale(22) };
        const std::wstring prefix = Ui(UiText::SearchPrefix);
        ScopedSelectObject sF(hdc, fontBold_.get());
        ScopedTextColor sC(hdc, RGB(255, 255, 255));
        if (searchAllSelected_ && !searchQuery_.empty()) {
            SIZE prefixSize{}, querySize{};
            GetTextExtentPoint32W(hdc, prefix.c_str(), static_cast<int>(prefix.size()), &prefixSize);
            GetTextExtentPoint32W(hdc, searchQuery_.c_str(), static_cast<int>(searchQuery_.size()), &querySize);
            DrawTextW(hdc, prefix.c_str(), -1, &rcPrompt, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            RECT rcSelected{rcPrompt.left + prefixSize.cx, rcPrompt.top + 1,
                std::min(rcPrompt.right, rcPrompt.left + prefixSize.cx + querySize.cx + metrics_.Scale(2)), rcPrompt.bottom - 1};
            UniqueBrush selectionBrush(CreateSolidBrush(GetSystemAccentColor(settings_.classicAccent)));
            FillRect(hdc, &rcSelected, selectionBrush.get());
            RECT rcSelectedText = rcSelected;
            rcSelectedText.left += metrics_.Scale(1);
            DrawTextW(hdc, searchQuery_.c_str(), -1, &rcSelectedText,
                DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
        } else {
            std::wstring prompt = prefix + searchQuery_;
            DrawTextW(hdc, prompt.c_str(), -1, &rcPrompt, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
        }
        curY += metrics_.Scale(26);

        int idx = 0;
        for (const auto& a : searchResults_) {
            if (curY + itemH >= rcArea.top && curY <= rcArea.bottom) {
            RECT rcItem = { 0, curY, rcArea.right, curY + itemH };
            if (hoverArea_ == 2 && hoverIndex_ == idx) {
                UniqueBrush brHov(CreateSolidBrush(RGB(40, 40, 40)));
                FillRect(hdc, &rcItem, brHov.get());
            }
            int icSz = metrics_.Scale(Metrics::BASE_ICON_SIZE);
            int icY = curY + (itemH - icSz) / 2;
            DrawAppIconOrFallback(hdc,a.hIcon,metrics_.Scale(16),icY,icSz);
            RECT rcText = { metrics_.Scale(52), curY, rcArea.right - metrics_.Scale(8), curY + itemH };
            ScopedSelectObject sFont(hdc, fontNormal_.get());
            ScopedTextColor sCol(hdc, RGB(255, 255, 255));
            DrawTextW(hdc, a.name.c_str(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            curY += itemH;
            idx++;
        }
        totalMainListH_ = metrics_.Scale(34) + static_cast<int>(searchResults_.size()) * itemH;
        const int visibleHeight = static_cast<int>(rcArea.bottom - rcArea.top);
        const int maxScroll = std::max(0, totalMainListH_ - visibleHeight);
        mainListScrollY_ = std::max(0, std::min(mainListScrollY_, maxScroll));
        DrawMainListScrollbar(hdc, rcArea);
    }

    void DrawTiles(HDC hdc, const RECT& rcRight) {
        // CStartMenuGridPane::OnSize (explorer.exe 10240.16384, RVA 0x91f50)
        // resizes both the child window and its DirectUI grid element to the
        // available client rectangle. Mirror that viewport boundary here without
        // resizing user tiles: off-viewport groups/tiles are clipped and skipped.
        ScopedSaveDC savedGridDc(hdc);
        if (!savedGridDc) return;
        const int clipResult = IntersectClipRect(hdc, rcRight.left, rcRight.top,
                                                  rcRight.right, rcRight.bottom);
        if (clipResult == ERROR || clipResult == NULLREGION) return;

        const int startX = rcRight.left + metrics_.Scale(Metrics::BASE_GROUP_PAD_X);
        const int startY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y);
        const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_MEDIUM) +
                           (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
        const int groupGap = metrics_.Scale(Metrics::BASE_GROUP_GAP);
        const int groupStep = groupW + groupGap;
        const int headerH = metrics_.Scale(Metrics::BASE_GROUP_HEADER_H);
        const int tileGap = metrics_.Scale(4);
        const COLORREF systemAccent = GetSystemAccentColor(settings_.classicAccent);

        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            // Group origins increase monotonically; stop once the grid viewport is passed.
            const int64_t gx64 = static_cast<int64_t>(startX) +
                                 static_cast<int64_t>(gi) * groupStep;
            if (gx64 >= rcRight.right) break;
            if (gx64 + groupW <= rcRight.left || gx64 > INT_MAX) continue;
            const int gx = static_cast<int>(gx64);
            auto& g = layout_.groups[gi];
            const int gy = startY;

            RECT rcHead{ gx, gy, gx + groupW, gy + headerH };
            RECT visibleHeader{};
            if (IntersectRect(&visibleHeader, &rcHead, &rcRight)) {
                ScopedSelectObject sFont(hdc, fontTitle_.get());
                ScopedTextColor sCol(hdc, (hoverArea_ == 4 && hoverGroup_ == static_cast<int>(gi))
                    ? RGB(255, 255, 255) : RGB(215, 215, 215));
                DrawTextW(hdc, (_wcsicmp(g.title.c_str(), L"Applicazioni") == 0 ? Ui(UiText::GroupApps) :
                    (_wcsicmp(g.title.c_str(), L"Tempo libero") == 0 ? Ui(UiText::GroupLeisure) : g.title.c_str())),
                    -1, &rcHead, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            }

            const int tilesStartY = gy + headerH + tileGap;
            for (size_t ti = 0; ti < g.tiles.size(); ++ti) {
                // The dragged tile is painted after every group, so that it
                // stays above the whole grid while it follows the pointer.
                if (IsTileLifted(static_cast<int>(gi), static_cast<int>(ti))) continue;
                auto& tile = g.tiles[ti];
                RECT rcTile = ComputeTileRect(gx, tilesStartY, tile);
                OffsetRect(&rcTile, TileAnimOffset(tile.animOffsetX), TileAnimOffset(tile.animOffsetY));
                RECT visibleTile{};
                if (!IntersectRect(&visibleTile, &rcTile, &rcRight)) continue;
                const bool isHovered = hoverArea_ == 3 && hoverGroup_ == static_cast<int>(gi) &&
                                       hoverIndex_ == static_cast<int>(ti);
                const COLORREF fill = tile.available
                    ? (settings_.appTileColors ? tile.appColor : systemAccent)
                    : RGB(72, 72, 72);
                DrawSingleTile(hdc, tile, rcTile, isHovered, fill, tile.animScale);
            }
        }

        DrawLiftedTile(hdc, rcRight, systemAccent);
    }

    // The dragged tile floats above the grid: enlarged, slightly transparent
    // and painted last, the way the 8045 reference mod lifts its tiles.
    void DrawLiftedTile(HDC hdc, const RECT& rcRight, COLORREF systemAccent) {
        if (!tileDragging_ || dragGroup_ < 0 ||
            dragGroup_ >= static_cast<int>(layout_.groups.size())) return;
        auto& tiles = layout_.groups[dragGroup_].tiles;
        if (dragTile_ < 0 || dragTile_ >= static_cast<int>(tiles.size())) return;
        RECT rcTile{};
        if (!GetTileDrawRect(dragGroup_, dragTile_, rcTile)) return;
        RECT visibleTile{};
        if (!IntersectRect(&visibleTile, &rcTile, &rcRight)) return;

        Tile& tile = tiles[dragTile_];
        const COLORREF fill = tile.available
            ? (settings_.appTileColors ? tile.appColor : systemAccent)
            : RGB(72, 72, 72);
        const int width = rcTile.right - rcTile.left;
        const int height = rcTile.bottom - rcTile.top;
        if (width <= 0 || height <= 0) return;

        // GDI has no per-primitive alpha: the tile is painted once on a scratch
        // surface seeded with the background and blended back over it. The
        // margin leaves room for the 5% the lifted tile grows by.
        const int margin = metrics_.Scale(8);
        const int surfaceW = width + margin * 2;
        const int surfaceH = height + margin * 2;
        UniqueMemDC scratchDc(CreateCompatibleDC(hdc));
        UniqueBitmap scratchBitmap(scratchDc ? CreateCompatibleBitmap(hdc, surfaceW, surfaceH) : nullptr);
        if (scratchDc && scratchBitmap) {
            ScopedSelectObject selectScratch(scratchDc.get(), scratchBitmap.get());
            ScopedBkMode scratchBkMode(scratchDc.get(), TRANSPARENT);
            if (BitBlt(scratchDc.get(), 0, 0, surfaceW, surfaceH, hdc,
                       rcTile.left - margin, rcTile.top - margin, SRCCOPY)) {
                RECT rcScratchTile{margin, margin, margin + width, margin + height};
                DrawSingleTile(scratchDc.get(), tile, rcScratchTile, true, fill, tile.animScale);
                BLENDFUNCTION blend{AC_SRC_OVER, 0, TILE_DRAG_ALPHA, 0};
                if (AlphaBlend(hdc, rcTile.left - margin, rcTile.top - margin, surfaceW, surfaceH,
                               scratchDc.get(), 0, 0, surfaceW, surfaceH, blend))
                    return;
            }
        }
        // Without the scratch surface the lifted tile is simply drawn opaque.
        DrawSingleTile(hdc, tile, rcTile, true, fill, tile.animScale);
    }

    RECT ComputeTileRect(int groupOriginX, int groupOriginY, const Tile& t) const {
        int gap = metrics_.Scale(Metrics::BASE_TILE_GAP);
        int med = metrics_.Scale(Metrics::BASE_TILE_MEDIUM);
        int sm = metrics_.Scale(Metrics::BASE_TILE_SMALL);
        int wd = metrics_.Scale(Metrics::BASE_TILE_WIDE);
        int lg = metrics_.Scale(Metrics::BASE_TILE_LARGE);

        int x = groupOriginX + t.col * (med + gap);
        int y = groupOriginY + t.row * (med + gap);

        int w = med, h = med;
        switch (t.size) {
            case TileSize::Small:  w = sm; h = sm; break;
            case TileSize::Medium: w = med; h = med; break;
            case TileSize::Wide:   w = wd;  h = med; break;
            case TileSize::Large:  w = lg;  h = lg;  break;
        }
        return RECT{ x, y, x + w, y + h };
    }

    // `scale` is the animated press/lift factor: the tile is painted around
    // the centre of its rectangle, which itself never changes, so hit testing
    // and the layout are unaffected.
    void DrawSingleTile(HDC hdc, Tile& tile, const RECT& rcTile, bool hovered,
                        COLORREF accent, float scale) {

        RECT drawRc = rcTile;
        bool drawBack = tile.showBack;

        // While the tile flips, apply the vertical cosine deformation
        if (tile.flipping) {
            float p = tile.flipProgress;
            float scaleY = 1.0f;
            if (p < 0.5f) {
                scaleY = cosf(p * 3.14159265f);
            } else {
                scaleY = cosf((1.0f - p) * 3.14159265f);
                drawBack = !tile.showBack;
            }
            if (scaleY < 0.05f) scaleY = 0.05f;

            int fullH = rcTile.bottom - rcTile.top;
            int squeezedH = (int)(fullH * scaleY);
            int midY = (rcTile.top + rcTile.bottom) / 2;
            drawRc.top = midY - squeezedH / 2;
            drawRc.bottom = midY + squeezedH / 2;
        }

        // Centered press-in and lift effects, inspired by AdmnistratoX's 8045
        // mod: only the painted rectangle changes, never the layout one.
        if (scale > 0.0f && fabsf(scale - 1.0f) > TILE_ANIM_SETTLE_SCALE) {
            const int width = drawRc.right - drawRc.left;
            const int height = drawRc.bottom - drawRc.top;
            const int scaledW = (std::max)(1, static_cast<int>(width * scale + 0.5f));
            const int scaledH = (std::max)(1, static_cast<int>(height * scale + 0.5f));
            drawRc.left -= (scaledW - width) / 2;
            drawRc.right = drawRc.left + scaledW;
            drawRc.top -= (scaledH - height) / 2;
            drawRc.bottom = drawRc.top + scaledH;
        }

        // Fill solid via GDI (faster than creating a GDI+ Graphics per tile/frame).
        UniqueBrush tileBrush(CreateSolidBrush(accent));
        if (tileBrush) FillRect(hdc, &drawRc, tileBrush.get());

        // 2 px white border on hover
        if (hovered) {
            UniquePen pWhite(CreatePen(PS_SOLID, 2, RGB(255, 255, 255)));
            HBRUSH brNull = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
            {
                ScopedSelectObject sPen(hdc, pWhite.get());
                ScopedSelectObject sB(hdc, brNull);
                Rectangle(hdc, drawRc.left, drawRc.top, drawRc.right, drawRc.bottom);
            }
        }

        if (drawBack && HasLocalLiveContent(tile)) {
            // Back face of the Calendar tile: weekday and date computed locally
            // with GetDateFormatEx (day number large, as in Windows 10).
            ScopedTextColor sCol(hdc, RGB(255, 255, 255));
            wchar_t dayText[16]{}, weekdayText[64]{}, monthText[64]{};
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, nullptr, L"d", dayText, ARRAYSIZE(dayText), nullptr);
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, nullptr, L"dddd", weekdayText, ARRAYSIZE(weekdayText), nullptr);
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, nullptr, L"MMMM yyyy", monthText, ARRAYSIZE(monthText), nullptr);

            RECT rcDay = { drawRc.left + metrics_.Scale(8), drawRc.top + metrics_.Scale(6), drawRc.right - metrics_.Scale(8), drawRc.top + metrics_.Scale(40) };
            ScopedSelectObject sFT(hdc, fontTitle_.get());
            DrawTextW(hdc, dayText, -1, &rcDay, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);

            RECT rcWeekday = { drawRc.left + metrics_.Scale(8), drawRc.top + metrics_.Scale(40), drawRc.right - metrics_.Scale(8), drawRc.top + metrics_.Scale(58) };
            ScopedSelectObject sFB(hdc, fontBold_.get());
            DrawTextW(hdc, weekdayText, -1, &rcWeekday, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);

            RECT rcMonth = { drawRc.left + metrics_.Scale(8), drawRc.bottom - metrics_.Scale(22), drawRc.right - metrics_.Scale(8), drawRc.bottom - metrics_.Scale(4) };
            ScopedSelectObject sFS(hdc, fontSmall_.get());
            DrawTextW(hdc, monthText, -1, &rcMonth, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
        } else {
            // Standard front face (centered/left icon, name at the bottom left)
            int icSz = metrics_.Scale(tile.size == TileSize::Small ? 20 : 32);
            int icX = drawRc.left + ((drawRc.right - drawRc.left) - icSz) / 2;
            int icY = drawRc.top + ((drawRc.bottom - drawRc.top) - icSz) / 2;

            if (tile.size == TileSize::Wide) {
                icX = drawRc.left + metrics_.Scale(16);
            } else if (tile.size == TileSize::Medium || tile.size == TileSize::Large) {
                icY -= metrics_.Scale(8);
            }

            DrawAppIconOrFallback(hdc,tile.hIcon,icX,icY,icSz);

            if (tile.size != TileSize::Small) {
                RECT rcLabel = { drawRc.left + metrics_.Scale(8), drawRc.bottom - metrics_.Scale(22),
                                 drawRc.right - metrics_.Scale(6), drawRc.bottom - metrics_.Scale(4) };
                ScopedSelectObject sF(hdc, fontSmall_.get());
                ScopedTextColor sCol(hdc, RGB(255, 255, 255));
                DrawTextW(hdc, TranslateTileCaption(tile.name), -1, &rcLabel, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
        }
    }

    int HitTestNormal(int relY) const {
        int curY = metrics_.Scale(8) + metrics_.Scale(24) - mainListScrollY_;
        int itemH = metrics_.Scale(Metrics::BASE_APP_ITEM_H);
        for (int i = 0; i < (int)catalog_.MostUsed().size(); ++i) {
            if (relY >= curY && relY < curY + itemH) return i;
            curY += itemH;
        }
        if (!catalog_.RecentlyAdded().empty()) {
            curY += metrics_.Scale(8) + metrics_.Scale(24);
            for (int j = 0; j < (int)catalog_.RecentlyAdded().size(); ++j) {
                if (relY >= curY && relY < curY + itemH) return (int)catalog_.MostUsed().size() + j;
                curY += itemH;
            }
        }
        return -1;
    }

    int GetAllAppsSectionOffset(int sectionIndex) const {
        if (sectionIndex < 0) return 0;
        int offset = 0;
        const int itemH = metrics_.Scale(Metrics::BASE_APP_ITEM_H);
        const int headH = metrics_.Scale(Metrics::BASE_SECTION_HEADER_H);
        int current = 0;
        for (const auto& sec : catalog_.Sections()) {
            if (current++ == sectionIndex) return offset;
            offset += headH + static_cast<int>(sec.apps.size()) * itemH;
            for (const auto& folder : sec.folders) {
                offset += itemH;
                if (folder.expanded) offset += static_cast<int>(folder.apps.size()) * itemH;
            }
        }
        return offset;
    }

    void ScrollToAllAppsSection(int sectionIndex) {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return;
        const int viewport = static_cast<int>(client.bottom) - metrics_.Scale(Metrics::BASE_USER_HEADER_H) -
                             BottomPlaceItemCount() * metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        const int maxScroll = std::max(0, totalAllAppsH_ - viewport);
        allAppsScrollY_ = std::max(0, std::min(GetAllAppsSectionOffset(sectionIndex), maxScroll));
        InvalidateRect(hwnd_.get(), nullptr, FALSE);
    }

    int HitTestAllAppsHeader(int relY) const {
        int curY = -allAppsScrollY_;
        const int itemH = metrics_.Scale(Metrics::BASE_APP_ITEM_H);
        const int headH = metrics_.Scale(Metrics::BASE_SECTION_HEADER_H);
        int sectionIndex = 0;
        for (const auto& sec : catalog_.Sections()) {
            if (relY >= curY && relY < curY + headH) return sectionIndex;
            curY += headH + static_cast<int>(sec.apps.size()) * itemH;
            for (const auto& folder : sec.folders) {
                curY += itemH;
                if (folder.expanded) curY += static_cast<int>(folder.apps.size()) * itemH;
            }
            ++sectionIndex;
        }
        return -1;
    }

    int HitTestAllApps(int relY) const {
        int curY = -allAppsScrollY_;
        int itemH = metrics_.Scale(Metrics::BASE_APP_ITEM_H);
        int headH = metrics_.Scale(Metrics::BASE_SECTION_HEADER_H);
        int idx = 0;
        for (const auto& sec : catalog_.Sections()) {
            curY += headH;
            for (size_t a = 0; a < sec.apps.size(); ++a) {
                if (relY >= curY && relY < curY + itemH) return idx;
                curY += itemH;
                idx++;
            }
            for (size_t f = 0; f < sec.folders.size(); ++f) {
                if (relY >= curY && relY < curY + itemH) return idx;
                curY += itemH;
                idx++;
                if (sec.folders[f].expanded) {
                    for (size_t sub = 0; sub < sec.folders[f].apps.size(); ++sub) {
                        if (relY >= curY && relY < curY + itemH) return idx;
                        curY += itemH;
                        idx++;
                    }
                }
            }
        }
        return -1;
    }

    // ===================== HIT TESTING =====================
    void HitTest(POINT pt, int& outArea, int& outIndex, int& outGroup) const {
        outArea = -1; outIndex = -1; outGroup = -1;
        RECT rcClient{};
        GetClientRect(hwnd_.get(), &rcClient);

        int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        int userH = metrics_.Scale(Metrics::BASE_USER_HEADER_H);
        int placeH = metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        int bottomH = BottomPlaceItemCount() * placeH;
        int bottomY = rcClient.bottom - bottomH;

        if (pt.x < leftW) {
            if (pt.y < userH) {
                outArea = 0; // User
                return;
            }
            if (pt.y >= bottomY) {
                const bool allAppsView = viewMode_ == ViewMode::AllApps || viewMode_ == ViewMode::AllAppsAlphabet;
                outArea = 1; // Places
                outIndex = allAppsView ? 3 : (pt.y - bottomY) / placeH;
                return;
            }
            outArea = 2; // App list
            int relY = pt.y - userH;
            if (viewMode_ == ViewMode::Normal) {
                outIndex = HitTestNormal(relY);
            } else if (viewMode_ == ViewMode::AllApps) {
                outIndex = HitTestAllApps(relY);
                if (outIndex < 0) {
                    const int section = HitTestAllAppsHeader(relY);
                    if (section >= 0) { outArea = 5; outGroup = section; }
                }
            } else if (viewMode_ == ViewMode::AllAppsAlphabet) {
                const int pad = metrics_.Scale(10), cellW = std::max(1,(leftW-2*pad)/5), cellH = metrics_.Scale(48);
                const int row = (relY-metrics_.Scale(10))/cellH, col = (pt.x-pad)/cellW;
                if (row >= 0 && col >= 0 && col < 5) {
                    const int n = row*5+col; wchar_t key = 0;
                    if (n == 0) key=L'#'; else if (n == 1) key=L'0'; else if (n >= 2 && n < 28) key=static_cast<wchar_t>(L'A'+n-2); else if (n == 28) key=L'~';
                    const int section=FindAllAppsSection(key);
                    if (key && section >= 0) { outArea=6; outGroup=section; }
                }
            } else if (viewMode_ == ViewMode::Search) {
                { const int searchY = relY + mainListScrollY_ - metrics_.Scale(34); outIndex = searchY < 0 ? -1 : searchY / metrics_.Scale(Metrics::BASE_APP_ITEM_H); }
            }
            return;
        }

        // Tile column
        int startX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X);
        int startY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y);
        int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_MEDIUM) +
                     (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
        int groupGap = metrics_.Scale(Metrics::BASE_GROUP_GAP);

        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            int gx = startX + (int)gi * (groupW + groupGap);
            int gy = startY;

            RECT rcHead = { gx, gy, gx + groupW, gy + metrics_.Scale(Metrics::BASE_GROUP_HEADER_H) };
            if (PtInRect(&rcHead, pt)) {
                outArea = 4; outGroup = (int)gi;
                return;
            }

            int tilesStartY = gy + metrics_.Scale(Metrics::BASE_GROUP_HEADER_H) + metrics_.Scale(4);
            const auto& g = layout_.groups[gi];
            for (size_t ti = 0; ti < g.tiles.size(); ++ti) {
                RECT rcTile = ComputeTileRect(gx, tilesStartY, g.tiles[ti]);
                if (PtInRect(&rcTile, pt)) {
                    outArea = 3; outGroup = (int)gi; outIndex = (int)ti;
                    return;
                }
            }
        }
    }

    // Copies the identity of the entry first: RecordLaunch() rebuilds the
    // catalog sections, which would invalidate a reference into them.
    void LaunchCatalogEntry(const AppEntry& entry) {
        const std::wstring path = entry.path, aumid = entry.aumid;
        Hide();
        catalog_.RecordLaunch(path.empty() ? aumid : path);
        LaunchApplication(path, aumid);
    }

    void HandleAllAppsClick(int clickIdx) {
        int idx = 0;
        const auto& sections = catalog_.Sections();
        for (size_t si = 0; si < sections.size(); ++si) {
            const auto& sec = sections[si];
            for (const auto& a : sec.apps) {
                if (idx == clickIdx) { LaunchCatalogEntry(a); return; }
                idx++;
            }
            for (size_t fi = 0; fi < sec.folders.size(); ++fi) {
                const auto& f = sec.folders[fi];
                if (idx == clickIdx) {
                    catalog_.ToggleFolder(si, fi);
                    InvalidateRect(hwnd_.get(), nullptr, FALSE);
                    return;
                }
                idx++;
                if (f.expanded) {
                    for (const auto& sub : f.apps) {
                        if (idx == clickIdx) { LaunchCatalogEntry(sub); return; }
                        idx++;
                    }
                }
            }
        }
    }

    // ===================== CONTEXT MENUS: owner-drawn HMENU (ImmersiveStartMenu) =====================
    bool ShowMetroMenuAt(POINT pt,const std::vector<MetroPopupItem>& items) {
        UniqueMenu menu(CreatePopupMenu());
        if(!menu) return false;
        for(const auto& item:items) {
            if(item.separator) { if(!AppendMenuW(menu.get(),MF_SEPARATOR,0,nullptr)) return false; }
            else {
                UINT flags=MF_STRING;
                if(item.heading || item.command==0) flags|=MF_GRAYED;
                if(!AppendMenuW(menu.get(),flags,item.command,item.text.c_str())) return false;
            }
        }
        const bool compact=(pendingContext_==PendingContext::ExplorerPlaces);
        TrackImmersiveMenu(menu.get(),pt,compact);
        return true;
    }

    void TrackImmersiveMenu(HMENU menu,POINT pt,bool compact=false) {
        if(!hwnd_||!menu) { HandleMetroPopupResult(0,1); return; }
        ClientToScreen(hwnd_.get(),&pt);
        const UINT command=ImmersiveStartMenu::Track(menu,hwnd_.get(),pt.x,pt.y,
            TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON,metrics_.dpi,compact);
        HandleMetroPopupResult(command,1);
    }

    void ShowExplorerJumpList() {
        std::vector<MetroPopupItem> menu = {
            {Ui(UiText::Pinned), 0, false, true},
            {GetKnownFolderLocalizedDisplayName(FOLDERID_Desktop, L"Desktop"), IDM_EXPLORER_DESKTOP},
            {GetKnownFolderLocalizedDisplayName(FOLDERID_Downloads, L"Downloads"), IDM_EXPLORER_DOWNLOADS},
            {GetKnownFolderLocalizedDisplayName(FOLDERID_Documents, L"Documents"), IDM_EXPLORER_DOCUMENTS},
            {GetKnownFolderLocalizedDisplayName(FOLDERID_Pictures, L"Pictures"), IDM_EXPLORER_PICTURES},
        };
        RECT client{};
        if (!GetClientRect(hwnd_.get(), &client)) return;
        const int popupHeight = 2 * metrics_.Scale(6) + static_cast<int>(menu.size()) * metrics_.Scale(34);
        // Attach the flyout beside the Explorer chevron and align its bottom with
        // the menu's bottom edge, like the pinned-places panel in the reference.
        POINT popupPoint{metrics_.Scale(Metrics::BASE_LEFT_COL_W) - metrics_.Scale(5),
                         static_cast<int>(client.bottom) - popupHeight};
        pendingContext_ = PendingContext::ExplorerPlaces;
        if (!ShowMetroMenuAt(popupPoint, menu)) ClearPendingContext();
    }

    void ClearPendingContext() {
        pendingContext_ = PendingContext::None;
        pendingGroup_ = pendingTile_ = -1;
        pendingAppName_.clear();
        pendingAppPath_.clear();
        pendingAppAumid_.clear();
    }

    void ShowTileContextMenu(POINT pt,int grp,int idx) {
        if(grp<0||grp>=static_cast<int>(layout_.groups.size())) return;
        auto& group=layout_.groups[grp];
        if(idx<0||idx>=static_cast<int>(group.tiles.size())) return;
        const auto& tile=group.tiles[idx];
        UniqueMenu menu(CreatePopupMenu());
        if(!menu) return;
        AppendMenuW(menu.get(),MF_STRING,IDM_UNPIN_FROM_START,Ui(UiText::Unpin));
        HMENU resizeRaw=CreatePopupMenu();
        if(resizeRaw) {
            UniqueMenu resize(resizeRaw);
            AppendMenuW(resize.get(),MF_STRING,IDM_RESIZE_SMALL,Ui(UiText::Small));
            AppendMenuW(resize.get(),MF_STRING,IDM_RESIZE_MEDIUM,Ui(UiText::Medium));
            AppendMenuW(resize.get(),MF_STRING,IDM_RESIZE_WIDE,Ui(UiText::Wide));
            AppendMenuW(resize.get(),MF_STRING,IDM_RESIZE_LARGE,Ui(UiText::Large));
            if(AppendMenuW(menu.get(),MF_POPUP|MF_STRING,
                reinterpret_cast<UINT_PTR>(resize.get()),Ui(UiText::Resize))) resize.release();
        }
        // Offered only for tiles that have real local content (Calendar);
        // no other app's live tile can be read through documented APIs.
        if (CanHaveLocalLiveContent(tile))
            AppendMenuW(menu.get(),MF_STRING,IDM_TOGGLE_LIVE,
                tile.isLive?Ui(UiText::DisableLive):Ui(UiText::EnableLive));
        if (IsFileBackedEntry(tile.path)) {
            AppendMenuW(menu.get(),MF_SEPARATOR,0,nullptr);
            AppendMenuW(menu.get(),MF_STRING,IDM_RUNAS_ADMIN,Ui(UiText::RunAsAdmin));
            AppendMenuW(menu.get(),MF_STRING,IDM_OPEN_LOCATION,Ui(UiText::OpenLocation));
        }
        pendingContext_=PendingContext::Tile; pendingGroup_=grp; pendingTile_=idx;
        TrackImmersiveMenu(menu.get(),pt,false);
    }

    const AppEntry* FindAllAppsAppByIndex(int targetIndex) const {
        if (targetIndex < 0) return nullptr;
        int index = 0;
        for (const auto& section : catalog_.Sections()) {
            for (const auto& app : section.apps) {
                if (index++ == targetIndex) return &app;
            }
            for (const auto& folder : section.folders) {
                ++index; // folder header occupies a hit-test row, not an app
                if (folder.expanded) {
                    for (const auto& app : folder.apps) {
                        if (index++ == targetIndex) return &app;
                    }
                }
            }
        }
        return nullptr;
    }

    void ShowAppContextMenu(POINT pt,int idx) {
        const AppEntry* app = nullptr;
        if (viewMode_ == ViewMode::Normal) {
            const int frequentCount = static_cast<int>(catalog_.MostUsed().size());
            if (idx >= 0 && idx < frequentCount) app = &catalog_.MostUsed()[idx];
            else {
                const int recentIndex = idx - frequentCount;
                if (recentIndex >= 0 && recentIndex < static_cast<int>(catalog_.RecentlyAdded().size()))
                    app = &catalog_.RecentlyAdded()[recentIndex];
            }
        } else if (viewMode_ == ViewMode::Search && idx >= 0 && idx < static_cast<int>(searchResults_.size())) {
            app = &searchResults_[idx];
        } else if (viewMode_ == ViewMode::AllApps) {
            app = FindAllAppsAppByIndex(idx);
        }
        if (!app) return;
        std::vector<MetroPopupItem> menu = {
            {Ui(UiText::Open), IDM_OPEN_APP},
            {Ui(UiText::PinToStart), IDM_PIN_TO_START}};
        // "Run as administrator" / "Open file location" only make sense for
        // shortcut entries; AppsFolder (shell:AppsFolder\<AUMID>) items have no
        // file to elevate or to select in Explorer.
        if (IsFileBackedEntry(app->path)) {
            menu.push_back({Ui(UiText::RunAsAdmin), IDM_RUNAS_ADMIN});
            menu.push_back({Ui(UiText::OpenLocation), IDM_OPEN_LOCATION});
        }
        pendingContext_ = PendingContext::App;
        pendingAppName_ = app->name;
        pendingAppPath_ = app->path;
        pendingAppAumid_ = app->aumid;
        if (!ShowMetroMenuAt(pt, menu)) ClearPendingContext();
    }

    static bool IsFileBackedEntry(const std::wstring& path) {
        return !path.empty() && _wcsnicmp(path.c_str(), L"shell:", 6) != 0;
    }

    void ShowUserMenu(POINT pt) {
        std::vector<MetroPopupItem> menu={{Ui(UiText::EditAccount),IDM_USER_SETTINGS},{Ui(UiText::Lock),IDM_USER_LOCK},{Ui(UiText::SignOut),IDM_USER_SIGNOUT}};
        pendingContext_=PendingContext::User;
        if(!ShowMetroMenuAt(pt,menu))ClearPendingContext();
    }

    void ShowPowerMenu(POINT pt) {
        const PowerMenuAvailability power = QueryPowerMenuAvailability();
        std::vector<MetroPopupItem> menu;
        if (power.sleep) menu.push_back({Ui(UiText::Sleep),IDM_POWER_SLEEP});
        menu.push_back({Ui(UiText::Shutdown),IDM_POWER_SHUTDOWN});
        menu.push_back({Ui(UiText::Restart),IDM_POWER_RESTART});
        if (power.hibernate) menu.push_back({Ui(UiText::Hibernate),IDM_POWER_HIBERNATE});
        pendingContext_=PendingContext::Power;
        if(!ShowMetroMenuAt(pt,menu))ClearPendingContext();
    }

    void HandleMetroPopupResult(UINT command,int reason) {
        if (reason == 2) { // outside the owner and flyout: dismiss Start as well
            ClearPendingContext();
            Hide();
            return;
        }
        const PendingContext context = pendingContext_;
        const int groupIndex = pendingGroup_;
        const int tileIndex = pendingTile_;
        const std::wstring appName = pendingAppName_;
        const std::wstring appPath = pendingAppPath_;
        const std::wstring appAumid = pendingAppAumid_;
        ClearPendingContext();
        if (!command || reason == 3) return;

        if (context == PendingContext::ExplorerPlaces) {
            const KNOWNFOLDERID* folderId = nullptr;
            switch (command) {
                case IDM_EXPLORER_DESKTOP: folderId = &FOLDERID_Desktop; break;
                case IDM_EXPLORER_DOWNLOADS: folderId = &FOLDERID_Downloads; break;
                case IDM_EXPLORER_DOCUMENTS: folderId = &FOLDERID_Documents; break;
                case IDM_EXPLORER_PICTURES: folderId = &FOLDERID_Pictures; break;
                default: return;
            }
            PWSTR rawPath = nullptr;
            const HRESULT hr = SHGetKnownFolderPath(*folderId, KF_FLAG_DEFAULT, nullptr, &rawPath);
            if (FAILED(hr) || !rawPath || !rawPath[0]) {
                Wh_Log(L"[ExplorerPlaces] SHGetKnownFolderPath returned no usable path: 0x%08X", static_cast<unsigned>(hr));
                if (rawPath) CoTaskMemFree(rawPath);
                return;
            }
            struct CoTaskMemPathScope { PWSTR path; ~CoTaskMemPathScope() { if (path) CoTaskMemFree(path); } } pathScope{rawPath};
            Hide();
            const HINSTANCE result = ShellExecuteW(nullptr, L"open", pathScope.path, nullptr, nullptr, SW_SHOWNORMAL);
            if (reinterpret_cast<INT_PTR>(result) <= 32)
                Wh_Log(L"[ExplorerPlaces] opening pinned known folder failed: %lld",
                       static_cast<long long>(reinterpret_cast<INT_PTR>(result)));
            return;
        }

        if (context == PendingContext::Tile) {
            if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return;
            auto& tiles = layout_.groups[groupIndex].tiles;
            if (tileIndex < 0 || tileIndex >= static_cast<int>(tiles.size())) return;
            auto& tile = tiles[tileIndex];
            if (command == IDM_UNPIN_FROM_START) {
                tiles.erase(tiles.begin() + tileIndex);
                PersistTileLayout();
                InvalidateRect(hwnd_.get(), nullptr, FALSE);
            } else if (command >= IDM_RESIZE_SMALL && command <= IDM_RESIZE_LARGE) {
                const TileSize requestedSize = command == IDM_RESIZE_SMALL ? TileSize::Small :
                                               command == IDM_RESIZE_MEDIUM ? TileSize::Medium :
                                               command == IDM_RESIZE_WIDE ? TileSize::Wide : TileSize::Large;
                int placedCol = tile.col, placedRow = tile.row;
                if (!FindTilePlacement(groupIndex, tileIndex, tile.col, tile.row, requestedSize,
                                       false, placedCol, placedRow)) {
                    Wh_Log(L"[TileLayout] resize rejected because no collision-free cell is available");
                    return;
                }
                const std::vector<POINT> beforeResize = CaptureTileRenderPositions(groupIndex);
                tile.size = requestedSize;
                tile.col = placedCol;
                tile.row = placedRow;
                // The resized tile reaches its new cell with a short glide.
                StartTileGlide(groupIndex, beforeResize);
                PersistTileLayout();
                InvalidateRect(hwnd_.get(), nullptr, FALSE);
            } else if (command == IDM_TOGGLE_LIVE && CanHaveLocalLiveContent(tile)) {
                tile.isLive = !tile.isLive; tile.flipping = false; tile.showBack = false;
                PersistTileLayout();
                InvalidateRect(hwnd_.get(), nullptr, FALSE);
            } else if (command == IDM_RUNAS_ADMIN) {
                if (tile.available && !tile.path.empty()) {
                    const std::wstring path = tile.path; Hide(); LaunchAdmin(path);
                }
            } else if (command == IDM_OPEN_LOCATION) {
                if (!tile.path.empty()) {
                    const std::wstring path = tile.path; Hide(); OpenExplorerLocation(path);
                }
            }
            return;
        }

        if (context == PendingContext::App) {
            if (command == IDM_OPEN_APP) {
                Hide();
                catalog_.RecordLaunch(appPath.empty() ? appAumid : appPath);
                LaunchApplication(appPath, appAumid);
            } else if (command == IDM_PIN_TO_START) {
                const AppEntry* app = nullptr;
                for (const auto& candidate : catalog_.Apps()) {
                    if ((!appPath.empty() && candidate.path == appPath) ||
                        (!appAumid.empty() && candidate.aumid == appAumid)) { app = &candidate; break; }
                }
                if (app && IsUsableCatalogApp(*app)) {
                    if (layout_.groups.empty()) {
                        TileGroup group;
                        group.title = L"Applicazioni";
                        layout_.groups.push_back(std::move(group));
                    }
                    // Reuse the collision-aware placement path used by drag-to-pin.
                    PinAppInGroup(*app, 0, 0, 0);
                }
            } else if (command == IDM_RUNAS_ADMIN) {
                Hide(); LaunchAdmin(appPath);
            } else if (command == IDM_OPEN_LOCATION) {
                Hide(); OpenExplorerLocation(appPath);
            }
            return;
        }

        if (context == PendingContext::User) {
            if (command == IDM_USER_SETTINGS) { Hide(); ShellExecuteW(nullptr,L"open",L"ms-settings:yourinfo",nullptr,nullptr,SW_SHOWNORMAL); }
            else if (command == IDM_USER_LOCK) { Hide(); LockWorkStation(); }
            else if (command == IDM_USER_SIGNOUT) { Hide(); ExitWindowsEx(EWX_LOGOFF,0); }
            return;
        }

        if (context == PendingContext::Power && command >= IDM_POWER_SLEEP && command <= IDM_POWER_HIBERNATE) {
            Hide(); ExecutePowerAction(static_cast<int>(command));
        }
    }

};

// ===================== UI THREAD, HOOKS AND NATIVE-MENU SUPPRESSION =====================
// Threads owned by the mod:
//  - UI thread: owns the menu window, the catalog, the icon cache and GDI+. All
//    slow work (catalog scans, .lnk resolution, UI Automation) happens here.
//  - Hook thread: installs WH_KEYBOARD_LL / WH_MOUSE_LL and only pumps messages,
//    so the hook procedures always return quickly (LowLevelHooksTimeout).
//  - Watcher thread: waits on change notifications for the Programs folders and
//    asks the UI thread to rescan the catalog.
//  - Profile-photo worker: loads the account picture in the background.
//  - WH_GETMESSAGE hooks on the Progman / taskbar threads drop
//    WM_SYSCOMMAND/SC_TASKLIST (classic Start activation) and toggle our menu.
//  - A 250 ms debounce avoids double toggles across sources.
// g_menu has a non-trivial destructor (DestroyWindow, UnregisterClass, GDI
// objects) and is released explicitly by the UI thread; see the Windhawk wiki
// page "Global objects and process shutdown".
[[clang::no_destroy]] static std::unique_ptr<StartMenuWindow> g_menu; // UI thread only
static Settings g_settings;

constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 110;
constexpr ULONG_PTR KEY_MARKER = 0x57484B31;
constexpr WORD VK_DUMMY = 0xE8; // unassigned virtual key

static bool g_winPressed = false;   // keyboard hook state (hook thread only)
static bool g_winChorded = false;
static bool g_swallowMouseUp = false; // mouse hook state (hook thread only)
static ULONGLONG g_lastAnchorRequestTick = 0; // hook thread only

static std::atomic<ULONGLONG> g_lastToggleTick{ 0 };
static std::atomic<DWORD> g_uiThreadId{ 0 };
static std::atomic<DWORD> g_hookThreadId{ 0 };
static std::atomic<HWND> g_menuHwnd{ nullptr };
static std::atomic<bool> g_stopRequested{ false };
static std::atomic<bool> g_settingsChangedPending{ false };

extern "C" IMAGE_DOS_HEADER __ImageBase;
static HINSTANCE GetModInstance() { return reinterpret_cast<HINSTANCE>(&__ImageBase); }

struct FindHandleScope {
    HANDLE h = INVALID_HANDLE_VALUE;
    ~FindHandleScope() { if (h != INVALID_HANDLE_VALUE) FindClose(h); }
};
struct LocalFreeScope {
    PWSTR p = nullptr;
    ~LocalFreeScope() { if (p) CoTaskMemFree(p); }
};

static bool ReadPictureFile(const std::wstring& path, std::vector<BYTE>& bytes) {
    UniqueHandle file(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                   nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
    if (!file) return false;
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file.get(), &size) || size.QuadPart <= 0 || size.QuadPart > 32ll * 1024 * 1024) return false;
    bytes.resize(static_cast<size_t>(size.QuadPart));
    size_t offset = 0;
    while (offset < bytes.size()) {
        DWORD got = 0;
        DWORD chunk = static_cast<DWORD>(std::min<size_t>(bytes.size() - offset, 1u << 20));
        if (!ReadFile(file.get(), bytes.data() + offset, chunk, &got, nullptr) || !got) { bytes.clear(); return false; }
        offset += got;
    }
    return true;
}

// Compatibility fallback based on the Win7Taskbar approach: Windows currently
// stores per-SID account-picture file paths here, but this registry contract is
// undocumented and may change. Keep the fully user-configurable file path first.
static bool LoadRegistryAccountPhoto(std::vector<BYTE>& result) {
    HANDLE rawToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &rawToken)) {
        Wh_Log(L"[Profile] OpenProcessToken failed: %u", GetLastError());
        return false;
    }
    UniqueHandle token(rawToken);
    DWORD tokenBytes = 0;
    GetTokenInformation(token.get(), TokenUser, nullptr, 0, &tokenBytes);
    if (!tokenBytes) return false;
    std::vector<BYTE> tokenData(tokenBytes);
    if (!GetTokenInformation(token.get(), TokenUser, tokenData.data(), tokenBytes, &tokenBytes)) {
        Wh_Log(L"[Profile] GetTokenInformation(TokenUser) failed: %u", GetLastError());
        return false;
    }
    auto* tokenUser = reinterpret_cast<TOKEN_USER*>(tokenData.data());
    LPWSTR rawSid = nullptr;
    if (!ConvertSidToStringSidW(tokenUser->User.Sid, &rawSid) || !rawSid) {
        Wh_Log(L"[Profile] ConvertSidToStringSid failed: %u", GetLastError());
        return false;
    }
    struct LocalStringDeleter { void operator()(wchar_t* p) const { if (p) LocalFree(p); } };
    std::unique_ptr<wchar_t, LocalStringDeleter> sid(rawSid);

    std::wstring subkey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AccountPicture\\Users\\";
    subkey += sid.get();
    HKEY rawKey = nullptr;
    LONG status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, subkey.c_str(), 0,
                                KEY_QUERY_VALUE | KEY_WOW64_64KEY, &rawKey);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"[Profile] AccountPicture registry key unavailable (status=%ld)", status);
        return false;
    }
    struct RegistryKeyScope { HKEY h; ~RegistryKeyScope() { if (h) RegCloseKey(h); } };
    RegistryKeyScope key{rawKey};

    static const wchar_t* values[] = {L"Image1080", L"Image448", L"Image240", L"Image96", L"Image64", L"Image48"};
    for (const wchar_t* valueName : values) {
        DWORD type = 0, bytes = 0;
        status = RegQueryValueExW(key.h, valueName, nullptr, &type, nullptr, &bytes);
        if (status != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ) || bytes < sizeof(wchar_t))
            continue;
        std::vector<wchar_t> value((bytes / sizeof(wchar_t)) + 2, wchar_t{});
        status = RegQueryValueExW(key.h, valueName, nullptr, &type,
                                  reinterpret_cast<BYTE*>(value.data()), &bytes);
        if (status != ERROR_SUCCESS || !value[0]) continue;
        std::wstring path(value.data());
        if (type == REG_EXPAND_SZ) {
            DWORD needed = ExpandEnvironmentStringsW(path.c_str(), nullptr, 0);
            if (needed > 1 && needed < 32768) {
                std::vector<wchar_t> expanded(needed);
                if (ExpandEnvironmentStringsW(path.c_str(), expanded.data(), needed)) path.assign(expanded.data());
            }
        }
        if (ReadPictureFile(path, result)) {
            Wh_Log(L"[Profile] Loaded account picture from registry value %s (%u bytes)",
                   valueName, static_cast<unsigned>(result.size()));
            return true;
        }
    }
    Wh_Log(L"[Profile] AccountPicture registry key has no readable image values");
    return false;
}

static bool LoadImageFilesFromDirectory(const std::wstring& directory, std::vector<BYTE>& result) {
    const std::wstring pattern = directory + L"\\*";
    WIN32_FIND_DATAW data{};
    FindHandleScope find{FindFirstFileW(pattern.c_str(), &data)};
    if (find.h == INVALID_HANDLE_VALUE) return false;
    struct Candidate { std::wstring path; FILETIME time; };
    std::vector<Candidate> files;
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const wchar_t* dot = wcsrchr(data.cFileName, L'.');
        if (!dot || (_wcsicmp(dot, L".jpg") && _wcsicmp(dot, L".jpeg") &&
                     _wcsicmp(dot, L".png") && _wcsicmp(dot, L".bmp"))) continue;
        files.push_back({directory + L"\\" + data.cFileName, data.ftLastWriteTime});
    } while (FindNextFileW(find.h, &data));
    std::sort(files.begin(), files.end(), [](const Candidate& a, const Candidate& b) {
        return CompareFileTime(&a.time, &b.time) > 0;
    });
    if (files.size() > 32) files.resize(32);
    for (const auto& candidate : files) {
        if (ReadPictureFile(candidate.path, result)) {
            Wh_Log(L"[Profile] Loaded account picture from roaming folder (%u bytes)",
                   static_cast<unsigned>(result.size()));
            return true;
        }
    }
    return false;
}

static std::vector<BYTE> ExtractAccountPicturePayload(const std::vector<BYTE>& file) {
    // .accountpicture-ms files usually contain two concatenated images. Look for
    // JPEG/PNG markers and validate the final decode with GDI+.
    std::vector<BYTE> best;
    const BYTE pngSig[] = {0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a};
    for (size_t i = 0; i + 8 < file.size(); ++i) {
        size_t end = 0;
        if (std::equal(std::begin(pngSig), std::end(pngSig), file.begin() + i)) {
            for (size_t j = i + 8; j + 8 <= file.size(); ++j) {
                if (file[j] == 'I' && file[j+1] == 'E' && file[j+2] == 'N' && file[j+3] == 'D') { end = j + 8; break; }
            }
        } else if (file[i] == 0xff && file[i+1] == 0xd8 && file[i+2] == 0xff) {
            for (size_t j = i + 3; j + 1 < file.size(); ++j) if (file[j] == 0xff && file[j+1] == 0xd9) { end = j + 2; break; }
        }
        if (end > i && end - i > best.size()) best.assign(file.begin() + i, file.begin() + end);
    }
    return best;
}

static bool LoadShellAccountPhoto(std::vector<BYTE>& result) {
    // CoInitializeEx is documented as a prerequisite for the known-folder APIs.
    ScopedCom com;
    if (!com.IsOk()) return false;
    PWSTR rawFolder = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_AccountPictures, KF_FLAG_DEFAULT, nullptr, &rawFolder)) || !rawFolder) return false;
    LocalFreeScope folder{rawFolder};
    std::wstring pattern = std::wstring(rawFolder) + L"\\*";
    WIN32_FIND_DATAW data{};
    FindHandleScope find{FindFirstFileW(pattern.c_str(), &data)};
    if (find.h == INVALID_HANDLE_VALUE) return false;
    struct Candidate { std::wstring path; FILETIME time; };
    std::vector<Candidate> files;
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const wchar_t* dot = wcsrchr(data.cFileName, L'.');
        if (!dot || (_wcsicmp(dot, L".accountpicture-ms") && _wcsicmp(dot, L".png") &&
                     _wcsicmp(dot, L".jpg") && _wcsicmp(dot, L".jpeg"))) continue;
        files.push_back({std::wstring(rawFolder) + L"\\" + data.cFileName, data.ftLastWriteTime});
    } while (FindNextFileW(find.h, &data));
    std::sort(files.begin(), files.end(), [](const Candidate& a, const Candidate& b) {
        return CompareFileTime(&a.time, &b.time) > 0;
    });
    if (files.size() > 16) files.resize(16);
    for (const auto& candidate : files) {
        std::vector<BYTE> raw;
        if (!ReadPictureFile(candidate.path, raw)) continue;
        std::vector<BYTE> image;
        if (PathMatchSpecW(candidate.path.c_str(), L"*.accountpicture-ms")) image = ExtractAccountPicturePayload(raw);
        else image = std::move(raw);
        if (!image.empty()) { result = std::move(image); return true; }
    }
    return false;
}


static bool InitializeDefaultProfileAvatar() {
    if (!g_gdiplusToken) return false;
    if (g_defaultProfileAvatar && g_defaultProfileAvatarStream) return true;
    g_defaultProfileAvatar.reset();
    g_defaultProfileAvatarStream.Reset();
    g_defaultProfileAvatarStream.Attach(SHCreateMemStream(
        g_defaultProfileAvatarPng, static_cast<UINT>(sizeof(g_defaultProfileAvatarPng))));
    if (!g_defaultProfileAvatarStream) {
        Wh_Log(L"[Profile] Could not create a stream for the bundled fallback avatar");
        return false;
    }
    // GDI+ objects must be allocated with GdiplusBase::operator new (GdipAlloc):
    // a global `::new` would pair a CRT allocation with GdipFree on delete and
    // crash when the bitmap is released at unload.
    g_defaultProfileAvatar.reset(new Gdiplus::Bitmap(g_defaultProfileAvatarStream.Get(), FALSE));
    if (!g_defaultProfileAvatar || g_defaultProfileAvatar->GetLastStatus() != Gdiplus::Ok ||
        !g_defaultProfileAvatar->GetWidth() || !g_defaultProfileAvatar->GetHeight()) {
        Wh_Log(L"[Profile] GDI+ could not decode the bundled fallback avatar");
        g_defaultProfileAvatar.reset();
        g_defaultProfileAvatarStream.Reset();
        return false;
    }
    return true;
}

// Like the native menu, prefer the display name over the logon name.
static std::wstring ResolveAccountDisplayName() {
    wchar_t name[256] = {};
    ULONG displayLen = ARRAYSIZE(name);
    if (GetUserNameExW(NameDisplay, name, &displayLen) && name[0]) return name;
    DWORD nameLen = ARRAYSIZE(name);
    name[0] = 0;
    if (GetUserNameW(name, &nameLen) && name[0]) return name;
    return {};
}

static DWORD WINAPI ProfilePhotoWorker(void*) {
    try {
    // Account name first: it is cheap locally but may wait for a domain
    // controller, which is exactly why it runs here and not on the UI thread.
    if (!g_settings.privacyMode) {
        std::wstring name = ResolveAccountDisplayName();
        if (!g_profilePhotoStop.load()) {
            {
                std::lock_guard<std::mutex> lock(g_profilePhotoMutex);
                g_accountNamePending = std::move(name);
                g_accountNamePendingValid = true;
            }
            if (HWND hwnd = g_menuHwnd.load()) PostMessageW(hwnd, WM_APP_PROFILE_PHOTO_READY, 0, 0);
        }
    }

    std::vector<BYTE> bytes;
    std::wstring explicitPath;
    { std::lock_guard<std::mutex> lock(g_profilePhotoPathMutex); explicitPath = g_profilePhotoPath; }
    if (!explicitPath.empty() && ReadPictureFile(explicitPath, bytes)) {
        Wh_Log(L"[Profile] Loaded configured picture file (%u bytes)", static_cast<unsigned>(bytes.size()));
    } else {
        if (!explicitPath.empty())
            Wh_Log(L"[Profile] Configured profilePicturePath could not be read; trying Windows account sources");
        // First try the per-user registry image paths, then the roaming
        // AccountPictures directory and the known-folder fallback.
        if (!LoadRegistryAccountPhoto(bytes)) {
            std::wstring roaming;
            bool loadedRoaming = GetKnownFolderPathString(FOLDERID_RoamingAppData, roaming) &&
                LoadImageFilesFromDirectory(roaming + L"\\Microsoft\\Windows\\AccountPictures", bytes);
            if (!loadedRoaming && !LoadShellAccountPhoto(bytes))
                Wh_Log(L"[Profile] No account photo source yielded an image; avatar remains fallback");
        }
    }
    if (!bytes.empty() && !g_profilePhotoStop.load()) {
        { std::lock_guard<std::mutex> lock(g_profilePhotoMutex); g_profilePhotoPending = std::move(bytes); }
        HWND hwnd = g_menuHwnd.load();
        if (hwnd && !g_profilePhotoStop.load()) PostMessageW(hwnd, WM_APP_PROFILE_PHOTO_READY, 0, 0);
    }
    return 0;
    } catch (const std::exception& e) {
        Wh_Log(L"[Profile] C++ exception in worker: %S", e.what());
    } catch (...) {
        Wh_Log(L"[Profile] unknown C++ exception in worker");
    }
    return 0;
}

static void RestartProfilePhotoWorker() {
    g_profilePhotoStop = true;
    if (g_profilePhotoThread) {
        WaitForSingleObject(g_profilePhotoThread.get(), INFINITE);
        g_profilePhotoThread.reset();
    }
    g_profilePhoto.reset();
    g_profilePhotoStream.Reset();
    { std::lock_guard<std::mutex> lock(g_profilePhotoMutex); g_profilePhotoPending.clear(); g_accountNamePending.clear(); g_accountNamePendingValid = false; }
    g_profilePhotoStop = false;
    g_profilePhotoThread.reset(CreateThread(nullptr, 0, ProfilePhotoWorker, nullptr, 0, nullptr));
    HWND hwnd = g_menuHwnd.load();
    if (hwnd) InvalidateRect(hwnd, nullptr, FALSE);
}

static UniqueHandle g_hUiThread;
static UniqueHandle g_hHookThread;
static UniqueHandle g_hWatcherThread;
static UniqueHandle g_hookReadyEvent;
static UniqueHandle g_watcherStopEvent;

static HHOOK g_hKbdHook = nullptr;   // hook thread
static HHOOK g_hMouseHook = nullptr; // hook thread
static HHOOK g_hProgHook = nullptr;  // UI thread
static HHOOK g_hTrayHook = nullptr;  // UI thread

static void RequestToggle(const wchar_t* source, HWND taskbar = nullptr) {
    ULONGLONG now = GetTickCount64();
    ULONGLONG last = g_lastToggleTick.load();
    if (now - last < 250) {
        Wh_Log(L"[Toggle] %s ignored (debounce, %u ms)", source, (unsigned)(now - last));
        return;
    }
    g_lastToggleTick = now;
    HWND h = g_menuHwnd.load();
    if (!h) {
        Wh_Log(L"[Toggle] %s: menu window not available", source);
        return;
    }
    Wh_Log(L"[Toggle] request from %s", source);
    PostMessageW(h, WM_APP_TOGGLE, 0, reinterpret_cast<LPARAM>(taskbar));
}

// UI thread: recompute the Start button rectangle of every taskbar and publish
// the result for the mouse hook.
static void RefreshStartRects(bool logIt) {
    std::vector<CachedStartButton> fresh;
    std::vector<HWND> taskbars;
    if (HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr)) taskbars.push_back(primary);
    HWND secondary = nullptr;
    while ((secondary = FindWindowExW(nullptr, secondary, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr)
        taskbars.push_back(secondary);

    for (HWND taskbar : taskbars) {
        StartAnchor a;
        if (!GetStartAnchor(a, taskbar)) continue;
        CachedStartButton entry;
        entry.taskbar = taskbar;
        entry.taskbarRect = a.taskbarRect;
        entry.startRect = a.startButton;
        entry.reliable = a.method != 3; // the geometric fallback is not trusted for click capture
        fresh.push_back(entry);
        if (logIt)
            Wh_Log(L"[Anchor] taskbar=%p method=%d rect=(%ld,%ld,%ld,%ld) edge=%u valid=%d",
                   taskbar, a.method, a.startButton.left, a.startButton.top, a.startButton.right,
                   a.startButton.bottom, (unsigned)a.edge, entry.reliable ? 1 : 0);
        if (!entry.reliable && logIt)
            Wh_Log(L"[Anchor] Start button NOT found reliably on taskbar %p; its clicks will not be intercepted, only the Win key", taskbar);
    }

    AcquireSRWLockExclusive(&g_startCacheLock);
    g_startCache.swap(fresh);
    ReleaseSRWLockExclusive(&g_startCacheLock);
    g_startCacheTick = GetTickCount64();
}

// Hook thread helpers: read-only lookups in the published cache.
static bool PointInAnyTaskbar(POINT pt) {
    AcquireSRWLockShared(&g_startCacheLock);
    bool inside = false;
    for (const auto& e : g_startCache) {
        if (PtInRect(&e.taskbarRect, pt)) { inside = true; break; }
    }
    ReleaseSRWLockShared(&g_startCacheLock);
    return inside;
}

static bool FindStartButtonAt(POINT pt, HWND& taskbarOut) {
    AcquireSRWLockShared(&g_startCacheLock);
    bool hit = false;
    for (const auto& e : g_startCache) {
        if (e.reliable && PtInRect(&e.startRect, pt)) { taskbarOut = e.taskbar; hit = true; break; }
    }
    ReleaseSRWLockShared(&g_startCacheLock);
    return hit;
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (kbd->dwExtraInfo == KEY_MARKER) {
            return CallNextHookEx(nullptr, nCode, wParam, lParam); // event injected by us
        }
        bool isWin = (kbd->vkCode == VK_LWIN || kbd->vkCode == VK_RWIN);
        bool down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool up = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (down) {
            if (isWin) {
                if (!g_winPressed) { // ignore auto-repeat
                    g_winPressed = true;
                    g_winChorded = false;
                }
            } else if (g_winPressed) {
                g_winChorded = true; // Win+R, Win+E, ...
            }
        } else if (up && isWin) {
            bool tap = g_winPressed && !g_winChorded;
            g_winPressed = false;
            g_winChorded = false;
            if (tap) {
                Wh_Log(L"[Key] isolated Win tap detected");
                // Inert key BEFORE the Win release: cancels the native menu.
                INPUT inp[2]{};
                inp[0].type = INPUT_KEYBOARD;
                inp[0].ki.wVk = VK_DUMMY;
                inp[0].ki.dwExtraInfo = KEY_MARKER;
                inp[1].type = INPUT_KEYBOARD;
                inp[1].ki.wVk = VK_DUMMY;
                inp[1].ki.dwFlags = KEYEVENTF_KEYUP;
                inp[1].ki.dwExtraInfo = KEY_MARKER;
                SendInput(2, inp, sizeof(INPUT));
                RequestToggle(L"Win key");
                // the Win release passes through normally (no stuck key)
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        const POINT pt = ms->pt;
        if (wParam == WM_MOUSEMOVE) {
            // Ask the UI thread for fresh Start rectangles while the pointer is over
            // a taskbar (throttled), so a centered Windows 11 taskbar whose icons
            // shifted is measured again before the click arrives.
            if (PointInAnyTaskbar(pt)) {
                const ULONGLONG now = GetTickCount64();
                if (now - g_lastAnchorRequestTick > 600 && now - g_startCacheTick.load() > 600) {
                    g_lastAnchorRequestTick = now;
                    if (HWND h = g_menuHwnd.load()) PostMessageW(h, WM_APP_REFRESH_ANCHOR, 0, 0);
                }
            }
        } else if (wParam == WM_LBUTTONDOWN) {
            HWND taskbar = nullptr;
            if (FindStartButtonAt(pt, taskbar)) {
                // Only when a taskbar is really under the cursor: a fullscreen app
                // covering that corner must keep receiving its clicks.
                HWND root = GetAncestor(WindowFromPoint(pt), GA_ROOT);
                if (root && (root == taskbar || IsTaskbarWindow(root))) {
                    g_swallowMouseUp = true;
                    Wh_Log(L"[Mouse] click on Start button at (%ld,%ld)", pt.x, pt.y);
                    RequestToggle(L"Start click", taskbar);
                    return 1; // keeps the native menu from opening
                }
            }
            HWND menu = g_menuHwnd.load();
            if (menu && IsWindowVisible(menu)) {
                RECT rcMenu{};
                if (GetWindowRect(menu, &rcMenu) && !PtInRect(&rcMenu, pt)) {
                    PostMessageW(menu, WM_APP_HIDE, 0, 0); // light dismiss
                }
            }
        } else if (wParam == WM_LBUTTONUP && g_swallowMouseUp) {
            g_swallowMouseUp = false;
            return 1;
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Runs on the Progman / Shell_TrayWnd thread: PostMessage only in here.
static LRESULT CALLBACK ShellThreadGetMsgProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && wParam == PM_REMOVE) {
        MSG* msg = reinterpret_cast<MSG*>(lParam);
        if (msg->message == WM_SYSCOMMAND && (msg->wParam & 0xFFF0) == SC_TASKLIST) {
            msg->message = WM_NULL; // suppress the native activation
            RequestToggle(L"SC_TASKLIST");
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Dedicated thread for the low-level hooks: it only pumps messages, so the
// hook procedures are never delayed by catalog scans or UI Automation.
static DWORD WINAPI HookThreadProc(LPVOID) {
    MSG queueInit{};
    PeekMessageW(&queueInit, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_hookThreadId = GetCurrentThreadId();

    const HINSTANCE hMod = GetModInstance();
    g_hKbdHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);
    Wh_Log(L"[Hooks] WH_KEYBOARD_LL: %s (err=%u)", g_hKbdHook ? L"ok" : L"FAILED", g_hKbdHook ? 0 : GetLastError());
    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, hMod, 0);
    Wh_Log(L"[Hooks] WH_MOUSE_LL: %s (err=%u)", g_hMouseHook ? L"ok" : L"FAILED", g_hMouseHook ? 0 : GetLastError());
    if (g_hookReadyEvent) SetEvent(g_hookReadyEvent.get());

    if (g_hKbdHook || g_hMouseHook) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (g_hKbdHook) { UnhookWindowsHookEx(g_hKbdHook); g_hKbdHook = nullptr; }
    if (g_hMouseHook) { UnhookWindowsHookEx(g_hMouseHook); g_hMouseHook = nullptr; }
    g_hookThreadId = 0;
    Wh_Log(L"[Hooks] hook thread finished");
    return 0;
}

static void InstallShellMessageHooks() {
    HWND prog = FindWindowW(L"Progman", nullptr);
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD tp = prog ? GetWindowThreadProcessId(prog, nullptr) : 0;
    DWORD tt = tray ? GetWindowThreadProcessId(tray, nullptr) : 0;
    if (tp) {
        g_hProgHook = SetWindowsHookExW(WH_GETMESSAGE, ShellThreadGetMsgProc, nullptr, tp);
        Wh_Log(L"[Hooks] WH_GETMESSAGE on Progman thread %u: %s", (unsigned)tp, g_hProgHook ? L"ok" : L"FAILED");
    }
    if (tt && tt != tp) {
        g_hTrayHook = SetWindowsHookExW(WH_GETMESSAGE, ShellThreadGetMsgProc, nullptr, tt);
        Wh_Log(L"[Hooks] WH_GETMESSAGE on taskbar thread %u: %s", (unsigned)tt, g_hTrayHook ? L"ok" : L"FAILED");
    }
}

static void RemoveShellMessageHooks() {
    if (g_hProgHook) { UnhookWindowsHookEx(g_hProgHook); g_hProgHook = nullptr; }
    if (g_hTrayHook) { UnhookWindowsHookEx(g_hTrayHook); g_hTrayHook = nullptr; }
}

static bool StartHookThread() {
    g_hookReadyEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    g_hHookThread.reset(CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, nullptr));
    if (!g_hHookThread) {
        Wh_Log(L"[Hooks] CreateThread failed: %u", GetLastError());
        return false;
    }
    if (g_hookReadyEvent) WaitForSingleObject(g_hookReadyEvent.get(), 5000);
    return g_hKbdHook != nullptr && g_hMouseHook != nullptr;
}

static void StopHookThread() {
    const DWORD tid = g_hookThreadId.load();
    if (tid) PostThreadMessageW(tid, WM_QUIT, 0, 0);
    if (g_hHookThread) {
        WaitForSingleObject(g_hHookThread.get(), INFINITE);
        g_hHookThread.reset();
    }
    g_hookReadyEvent.reset();
}

// Watches the user and common Programs folders; a change (install/uninstall of
// a Win32 app) asks the UI thread to rescan after a short quiet period.
static DWORD WINAPI ProgramsWatcherProc(LPVOID) {
    std::vector<HANDLE> waitHandles;
    waitHandles.push_back(g_watcherStopEvent.get());
    const KNOWNFOLDERID folders[] = { FOLDERID_Programs, FOLDERID_CommonPrograms };
    std::vector<HANDLE> notifications;
    for (const auto& id : folders) {
        std::wstring path;
        if (!GetKnownFolderPathString(id, path)) continue;
        HANDLE h = FindFirstChangeNotificationW(path.c_str(), TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE);
        if (h != INVALID_HANDLE_VALUE) { notifications.push_back(h); waitHandles.push_back(h); }
    }
    struct Cleanup { std::vector<HANDLE>& h; ~Cleanup() { for (HANDLE x : h) FindCloseChangeNotification(x); } } cleanup{notifications};
    if (notifications.empty()) {
        Wh_Log(L"[Watcher] no Programs folder could be watched");
        return 0;
    }

    bool pending = false;
    while (true) {
        const DWORD wait = WaitForMultipleObjects(static_cast<DWORD>(waitHandles.size()), waitHandles.data(),
                                                  FALSE, pending ? 2000 : INFINITE);
        if (wait == WAIT_OBJECT_0) break; // stop
        if (wait == WAIT_TIMEOUT) {
            pending = false;
            if (HWND h = g_menuHwnd.load()) PostMessageW(h, WM_APP_REFRESH, 0, 0);
            continue;
        }
        const DWORD index = wait - WAIT_OBJECT_0;
        if (index >= 1 && index < waitHandles.size()) {
            pending = true; // debounce: wait for 2 s of quiet before refreshing
            if (!FindNextChangeNotification(waitHandles[index])) break;
        } else {
            break; // WAIT_FAILED / abandoned
        }
    }
    return 0;
}

static void StartProgramsWatcher() {
    g_watcherStopEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!g_watcherStopEvent) return;
    g_hWatcherThread.reset(CreateThread(nullptr, 0, ProgramsWatcherProc, nullptr, 0, nullptr));
}

static void StopProgramsWatcher() {
    if (g_watcherStopEvent) SetEvent(g_watcherStopEvent.get());
    if (g_hWatcherThread) {
        WaitForSingleObject(g_hWatcherThread.get(), INFINITE);
        g_hWatcherThread.reset();
    }
    g_watcherStopEvent.reset();
}

static bool WaitForMainExplorerReady();

static DWORD WINAPI UiThreadProc(LPVOID) {
    // Create the thread message queue before publishing its ID, so settings
    // changes or unload requests can be posted while Explorer is initializing.
    MSG queueInit{};
    PeekMessageW(&queueInit, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_uiThreadId = GetCurrentThreadId();
    ScopedCom com;
    Wh_Log(L"[UI] bootstrap thread started (id=%u, COM=%d)", (unsigned)g_uiThreadId.load(), com.IsOk() ? 1 : 0);
    if (!WaitForMainExplorerReady()) {
        Wh_Log(L"[UI] shell did not become ready in this Explorer process; no UI hooks installed");
        g_uiThreadId = 0;
        return 0;
    }
    if (g_settingsChangedPending.exchange(false)) g_settings.Load();

    GdiplusStartupInput gdipInput;
    if (GdiplusStartup(&g_gdiplusToken, &gdipInput, nullptr) != Ok) {
        g_gdiplusToken = 0;
        Wh_Log(L"[GDI+] startup failed");
    }
    if (g_gdiplusToken && !InitializeDefaultProfileAvatar())
        Wh_Log(L"[Profile] bundled avatar unavailable; no drawn substitute will be used");
    Wh_Log(L"[UI] primary Explorer shell confirmed; initializing menu");

    bool ok = false;
    GUARDED(L"UiInit", {
        g_menu = std::make_unique<StartMenuWindow>();
        if (g_menu->Create(GetModInstance())) {
            g_menu->ApplySettings(g_settings);
            g_menuHwnd = g_menu->GetHwnd();
            g_profilePhotoStop = false;
            g_profilePhotoThread.reset(CreateThread(nullptr, 0, ProfilePhotoWorker, nullptr, 0, nullptr));
            RefreshStartRects(true);
            InstallShellMessageHooks();
            ok = StartHookThread();
            if (ok) StartProgramsWatcher();
        }
    })

    if (ok) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            if (!msg.hwnd && msg.message == WM_APP_SETTINGS_CHANGED) {
                GUARDED(L"Settings", {
                    g_settingsChangedPending = false;
                    g_settings.Load();
                    if (g_menu) g_menu->ApplySettings(g_settings);
                    RestartProfilePhotoWorker();
                })
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else {
        Wh_Log(L"[UI] initialization failed; the native Start menu stays active");
    }

    // Teardown, in dependency order, on the thread that owns the objects.
    // Each step is logged so a failure during unload can be located.
    Wh_Log(L"[UI] teardown: stopping hook thread");
    StopHookThread();
    Wh_Log(L"[UI] teardown: stopping Programs watcher");
    StopProgramsWatcher();
    Wh_Log(L"[UI] teardown: removing shell message hooks");
    RemoveShellMessageHooks();
    g_profilePhotoStop = true;
    if (g_profilePhotoThread) {
        WaitForSingleObject(g_profilePhotoThread.get(), INFINITE);
        g_profilePhotoThread.reset();
    }
    g_menuHwnd = nullptr;
    Wh_Log(L"[UI] teardown: destroying the Start window");
    GUARDED(L"Teardown", {
        g_menu.reset(); // destroys the window on the thread that created it
    })
    Wh_Log(L"[UI] teardown: releasing icon bitmap cache (%u entries)",
           g_iconBitmapCache ? (unsigned)g_iconBitmapCache->size() : 0u);
    g_iconBitmapCache.reset(); // GDI+ bitmaps destroyed before GdiplusShutdown
    Wh_Log(L"[UI] teardown: releasing stock icons");
    if (g_stockFolderIcon) { DestroyIcon(g_stockFolderIcon); g_stockFolderIcon=nullptr; }
    if (g_stockAppIcon) { DestroyIcon(g_stockAppIcon); g_stockAppIcon=nullptr; }
    Wh_Log(L"[UI] teardown: releasing profile photo");
    g_profilePhoto.reset();
    g_profilePhotoStream.Reset();
    Wh_Log(L"[UI] teardown: releasing default avatar");
    g_defaultProfileAvatar.reset();
    g_defaultProfileAvatarStream.Reset();
    { std::lock_guard<std::mutex> lock(g_profilePhotoMutex); g_profilePhotoPending.clear(); g_accountNamePending.clear(); g_accountNamePendingValid = false; }
    Wh_Log(L"[UI] teardown: GdiplusShutdown");
    if (g_gdiplusToken) { GdiplusShutdown(g_gdiplusToken); g_gdiplusToken=0; }
    Wh_Log(L"[UI] teardown: GDI+ shut down");
    g_uiThreadId = 0;
    Wh_Log(L"[UI] thread finished");
    return 0;
}

// Evaluated on the worker after Explorer started creating the shell. In
// Wh_ModInit the Shell_TrayWnd / GetShellWindow windows may not exist yet.
static bool IsMainExplorerProcess() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(tray, &pid);
    return pid == GetCurrentProcessId();
}

static bool WaitForMainExplorerReady() {
    const DWORD currentPid = GetCurrentProcessId();
    const ULONGLONG start = GetTickCount64();
    constexpr ULONGLONG kShellStartupTimeoutMs = 120000;

    while (!g_stopRequested.load()) {
        if (IsMainExplorerProcess()) return true;

        // A different process already owns the shell: this Explorer is a folder
        // window process, not the desktop/taskbar host, so do not install hooks.
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        HWND shell = GetShellWindow();
        for (int i = 0; i < 2; ++i) {
            HWND candidate = i == 0 ? tray : shell;
            if (!candidate) continue;
            DWORD ownerPid = 0;
            GetWindowThreadProcessId(candidate, &ownerPid);
            if (ownerPid && ownerPid != currentPid) {
                Wh_Log(L"[Init] shell window belongs to Explorer PID %u, not PID %u",
                       (unsigned)ownerPid, (unsigned)currentPid);
                return false;
            }
        }

        MSG msg{};
        while (PeekMessageW(&msg, nullptr, WM_APP_SETTINGS_CHANGED,
                            WM_APP_SETTINGS_CHANGED, PM_REMOVE)) {
            g_settingsChangedPending = false;
            g_settings.Load();
        }
        if (GetTickCount64() - start >= kShellStartupTimeoutMs) {
            Wh_Log(L"[Init] timed out waiting for this Explorer process to own Shell_TrayWnd");
            return false;
        }
        Sleep(100);
    }
    return false;
}

// ===================== WINDHAWK ENTRY POINTS =====================
BOOL Wh_ModInit() {
    // Do not block Explorer's startup while its shell windows are being created.
    // The worker waits asynchronously for the process that owns Shell_TrayWnd.
    g_stopRequested = false;
    g_settingsChangedPending = false;
    g_settings.Load();
    HANDLE t = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (!t) {
        Wh_Log(L"[Init] CreateThread failed: %u", GetLastError());
        return FALSE;
    }
    g_hUiThread.reset(t);
    Wh_Log(L"[Init] bootstrap worker started; waiting asynchronously for the primary Explorer shell");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    g_settingsChangedPending = true;
    DWORD tid = g_uiThreadId.load();
    if (tid && !PostThreadMessageW(tid, WM_APP_SETTINGS_CHANGED, 0, 0))
        Wh_Log(L"[Settings] PostThreadMessage failed: %u", GetLastError());
}

void Wh_ModUninit() {
    Wh_Log(L"[Uninit] unloading");
    g_stopRequested = true;
    DWORD tid = g_uiThreadId.load();
    if (tid && !PostThreadMessageW(tid, WM_QUIT, 0, 0))
        Wh_Log(L"[Uninit] PostThreadMessage(WM_QUIT) failed: %u", GetLastError());
    if (g_hUiThread) {
        // The module must not unload while its worker is still executing code in it.
        WaitForSingleObject(g_hUiThread.get(), INFINITE);
        g_hUiThread.reset();
        Wh_Log(L"[Uninit] UI thread joined");
    }
    g_uiThreadId = 0;
    Wh_Log(L"[Uninit] Mod successfully uninitialized.");
}
