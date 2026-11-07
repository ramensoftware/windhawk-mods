// ==WindhawkMod==
// @id              explorer-translucent-menus
// @name            Explorer Translucent Menus
// @description     Translucent materials and optional borderless classic context menus for File Explorer
// @version         1.0.0
// @author          alkali210
// @github          https://github.com/alkali210
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -luxtheme -lgdi32 -ladvapi32 -lcomctl32 -lmsimg32
// @license         GPL-3.0-or-later
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/alkali210/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/alkali210/windhawk-mods


// clang-format off
// ==WindhawkModReadme==
/*
# Explorer Translucent Menus

Apply translucent materials to classic Win32 popup menus in File Explorer.
Windows continues to provide menu items, layout, commands, keyboard navigation,
and shell extensions. Windows 11's modern context menu is left unchanged.

## Requirements

- Windows 11 22H2 or later (build 22621+), AMD64.
- Desktop composition and Windows transparency effects enabled.
- No other renderer modifying the same Explorer menus.

To open a classic context menu on Windows 11, select **Show more options** or
use **Shift+F10**. Select **Native** to disable all hooks, or hold **Ctrl** before
opening a menu to bypass its modifications.

## Materials

| Material | Tint opacity |
| --- | --- |
| Native | No modifications |
| Solid | Always opaque |
| Transparent | Adjustable, no blur |
| Acrylic | Adjustable tint over blur |
| Modern Acrylic | Adjustable tint color, fixed opacity |
| System Acrylic | Fixed Windows recipe |
| Mica / Mica Alt | Fixed Windows recipes |

Tint opacity changes the material, not text or icons. Theme selection controls
the light or dark palette. Hover highlight opacity can be adjusted separately.
Unrecognized saved material values use Acrylic.

## Appearance

Enable **Hide menu border** to suppress the outline on modified menus. This
preserves menu dimensions and works with rounded or square corners. Native
mode and Ctrl bypass retain the complete Windows appearance.
The option also protects the outline setting from later Shell/UxTheme updates
while the modified menu is being tracked.

Legacy icon repair converts copied RGB bitmaps to premultiplied alpha. Optional
background removal clears only matching corner-connected white, black, or
magenta regions. Disable background removal for icons with intentional borders.

## Compatibility

Unknown owner-drawn menus, custom background brushes, high contrast, remote
sessions, disabled transparency, and recognized conflicting renderers are
left unchanged. Settings changes reload the mod. Active effects suppress the
native bitmap opening animation to avoid an opaque initial snapshot.

The implementation uses version-dependent Windows interfaces: the composition
attribute exports, UxTheme ordinal 74, the DarkMode::Menu theme, menu layout
message 0x1e2, and Shell window properties. It does not read private Shell item
structures, patch instructions, download symbols, or replace the menu loop.

## Credits and license

Informed by [TranslucentFlyouts](https://github.com/ALTaleX531/TranslucentFlyouts),
[Translucent Windows](https://windhawk.net/mods/translucent-windows), and
[Custom Cached Context Menu](https://github.com/Bolt-Scripts/custom-cached-context-menu).
This implementation is distributed under GPL-3.0-or-later.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- backgroundEffect: acrylic
  $name: Background material
  $description: Applies only to classic Explorer popup menus. Native disables all modifications.
  $options:
  - none: Native
  - solid: Solid
  - transparent: Transparent
  - acrylic: Acrylic (adjustable tint)
  - modern-acrylic: Modern Acrylic
  - system: System Acrylic
  - mica: Mica
  - mica-alt: Mica Alt
- theme: auto
  $name: Menu theme
  $description: Select the palette for modified menus.
  $options:
  - auto: Follow Windows
  - light: Light
  - dark: Dark
- tintOpacity: 35
  $name: Tint opacity
  $description: 0-100. Applies to Acrylic and Transparent.
- lightTint: "#F5F5F5"
  $name: Light tint color
  $description: "#RRGGBB color for Solid, Transparent, Acrylic and Modern Acrylic in the light theme."
- darkTint: "#202020"
  $name: Dark tint color
  $description: "#RRGGBB color for Solid, Transparent, Acrylic and Modern Acrylic in the dark theme."
- highlightOpacity: 28
  $name: Hover highlight opacity
  $description: 1-100 for the custom highlight. Set to 0 to use the native highlight.
- roundedCorners: true
  $name: Rounded corners
  $description: Request small rounded corners for modified menus.
- hideMenuBorder: false
  $name: Hide menu border
  $description: Suppress the outline on modified menus without changing their size.
- repairIcons: true
  $name: Repair legacy icon transparency
  $description: Convert copied legacy RGB icons to premultiplied alpha without modifying the source bitmaps.
- removeIconBackground: true
  $name: Remove legacy icon backgrounds
  $description: Remove corner-connected white, black, or magenta backgrounds. Disable for icons with intentional solid borders.
- ctrlBypass: false
  $name: Hold Ctrl to bypass
  $description: Hold Ctrl before opening a menu to keep its complete native appearance and behavior.
- debugLogging: false
  $name: Diagnostic logging
  $description: Log preparation, geometry, and native frame state. Also enable log output in the Advanced tab. At most 64 geometry events and 4 late border overrides per popup.
*/
// ==/WindhawkModSettings==
// clang-format on

#include <commctrl.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#include <windows.h>
#include <atomic>
#include <cmath>
#include <cwchar>

#ifndef ETM_TEST
#include <windhawk_api.h>
#endif

namespace etm {

// MENU_POPUPITEM_FOCUSABLE from the Windows 11 SDK. Windhawk 1.7.3 ships
// an older vsstyle.h that stops at MENU_SYSTEMRESTORE.
constexpr int kPopupItemFocusable = 27;
constexpr int kPopupItemKeyboardFocus = 26;
enum class Material {
    Native,
    Solid,
    Transparent,
    Acrylic,
    ModernAcrylic,
    SystemAcrylic,
    Mica,
    MicaAlt
};

Material ParseMaterial(PCWSTR value) {
    if (!value)
        return Material::Acrylic;
    struct Choice {
        PCWSTR name;
        Material value;
    };
    constexpr Choice choices[] = {{L"none", Material::Native},
                                  {L"solid", Material::Solid},
                                  {L"transparent", Material::Transparent},
                                  {L"acrylic", Material::Acrylic},
                                  {L"modern-acrylic", Material::ModernAcrylic},
                                  {L"system", Material::SystemAcrylic},
                                  {L"mica", Material::Mica},
                                  {L"mica-alt", Material::MicaAlt}};
    for (auto choice : choices)
        if (!_wcsicmp(value, choice.name))
            return choice.value;
    return Material::Acrylic;
}
bool IsSystemMaterial(Material value) {
    return value == Material::SystemAcrylic || value == Material::Mica ||
           value == Material::MicaAlt;
}
int ClampPercent(int value) {
    return value < 0 ? 0 : value > 100 ? 100 : value;
}
COLORREF ParseTint(PCWSTR value, COLORREF fallback) {
    if (!value)
        return fallback;
    if (*value == L'#')
        ++value;
    if (wcslen(value) != 6)
        return fallback;
    for (unsigned i = 0; i < 6; ++i)
        if (!((value[i] >= L'0' && value[i] <= L'9') ||
              (value[i] >= L'a' && value[i] <= L'f') ||
              (value[i] >= L'A' && value[i] <= L'F')))
            return fallback;
    unsigned rgb = wcstoul(value, nullptr, 16);
    return RGB((rgb >> 16) & 255, (rgb >> 8) & 255, rgb & 255);
}

struct Settings {
    bool roundedCorners;
    bool ctrlBypass;
    bool debugLogging;
    Material material = Material::Acrylic;
    int tintOpacity = 35;
    COLORREF lightTint = RGB(245, 245, 245);
    COLORREF darkTint = RGB(32, 32, 32);
    int theme = 0;  // 0=auto, 1=light, 2=dark
    int highlightOpacity = 28;
    bool repairIcons = true;
    bool removeIconBackground = true;
    bool hideMenuBorder = false;
};
Settings
    g_settings{};  // Immutable while hooks are active; settings cause reload.
std::atomic<bool> g_stopping{false};
std::atomic<unsigned> g_calls{0};
struct CallGuard {
    CallGuard() { g_calls.fetch_add(1, std::memory_order_acquire); }
    ~CallGuard() { g_calls.fetch_sub(1, std::memory_order_release); }
};
using GetThemeClass_t = HRESULT(WINAPI*)(HTHEME, LPWSTR, int);
GetThemeClass_t g_getThemeClass = nullptr;
decltype(&TrackPopupMenu) g_trackPopupMenu = nullptr;
decltype(&TrackPopupMenuEx) g_trackPopupMenuEx = nullptr;
decltype(&DrawThemeBackground) g_drawThemeBackground = nullptr;
decltype(&DrawThemeBackgroundEx) g_drawThemeBackgroundEx = nullptr;
decltype(&DrawThemeText) g_drawThemeText = nullptr;
decltype(&DrawThemeTextEx) g_drawThemeTextEx = nullptr;
decltype(&DrawTextW) g_drawText = nullptr;
decltype(&FillRect) g_fillRect = nullptr;
decltype(&BitBlt) g_bitBlt = nullptr;
decltype(&StretchBlt) g_stretchBlt = nullptr;
decltype(&AlphaBlend) g_alphaBlend = nullptr;
decltype(&DwmSetWindowAttribute) g_dwmSetWindowAttribute = nullptr;

// Optional user32 composition exports. These are undocumented, unlike the
// system-backdrop alternative; no offsets or symbol lookups are used.
struct AccentPolicy {
    int state;
    int flags;
    DWORD
    color;  // AABBGGRR, not COLORREF with an opacity applied to the window.
    int animation;
};
struct CompositionData {
    int attribute;
    void* data;
    SIZE_T size;
};
using CompositionAttribute_t = BOOL(WINAPI*)(HWND, CompositionData*);
CompositionAttribute_t g_setComposition = nullptr;
CompositionAttribute_t g_getComposition = nullptr;
constexpr UINT kMenuSizeWindow = 0x01e2;
thread_local unsigned t_textDepth = 0;
thread_local unsigned t_observerDepth = 0;
thread_local unsigned t_bitmapDepth = 0;
thread_local unsigned t_bypassDepth = 0;
struct BitmapGuard {
    BitmapGuard() { ++t_bitmapDepth; }
    ~BitmapGuard() { --t_bitmapDepth; }
};
struct ObserverGuard {
    ObserverGuard() { ++t_observerDepth; }
    ~ObserverGuard() { --t_observerDepth; }
};
struct TextGuard {
    TextGuard() { ++t_textDepth; }
    ~TextGuard() { --t_textDepth; }
};

// POD TLS only. In particular, nothing runs a destructor at process shutdown.
struct Popup {
    HWND hwnd;
    HMENU menu;
    HBRUSH oldBrush;
    HTHEME palette;
    bool brushChanged;
    bool active;
    bool dark;
    bool haveBackdrop;
    bool haveCorner;
    bool haveDark;
    DWORD oldBackdrop;
    DWORD oldCorner;
    BOOL oldDark;
    COLORREF oldBorderColor;
    bool haveBorderColor;
    bool borderColorChanged;
    unsigned borderOverrides;
    AccentPolicy oldAccent;
    bool haveAccent;
    bool accentChanged;
    bool frameExtended;
    bool applying;
    bool subclassed;
    bool preparing;
    bool layingOut;
    unsigned geometryEvents;
    ULONGLONG observedAt;
    unsigned backgroundDraws;
    unsigned shellTextDraws;
    unsigned themeTextDraws;
    unsigned iconRepairs;
    unsigned highlights;
};

void TracePopupGeometry(Popup& popup,
                        PCWSTR phase,
                        const WINDOWPOS* position = nullptr) {
    if (!g_settings.debugLogging || !popup.active ||
        !IsSystemMaterial(g_settings.material) || popup.geometryEvents >= 64)
        return;
    ++popup.geometryEvents;
    RECT window{}, frame{};
    BOOL haveWindow = GetWindowRect(popup.hwnd, &window);
    HRESULT haveFrame = DwmGetWindowAttribute(
        popup.hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(frame));
    BOOL ncEnabled = FALSE;
    UINT border = 0;
    HRESULT haveNc = DwmGetWindowAttribute(
        popup.hwnd, DWMWA_NCRENDERING_ENABLED, &ncEnabled, sizeof(ncEnabled));
    HRESULT haveBorder =
        DwmGetWindowAttribute(popup.hwnd, DWMWA_VISIBLE_FRAME_BORDER_THICKNESS,
                              &border, sizeof(border));
    Wh_Log(
        L"Geometry %p +%llu ms %s visible=%d dpi=%u winOk=%d "
        L"win=(%ld,%ld,%ld,%ld) frameHr=0x%08lx frame=(%ld,%ld,%ld,%ld) "
        L"ncHr=0x%08lx nc=%d borderHr=0x%08lx border=%u flags=0x%x "
        L"proposed=(%d,%d,%d,%d)",
        popup.hwnd, GetTickCount64() - popup.observedAt, phase,
        IsWindowVisible(popup.hwnd), GetDpiForWindow(popup.hwnd), haveWindow,
        window.left, window.top, window.right, window.bottom,
        static_cast<ULONG>(haveFrame), frame.left, frame.top, frame.right,
        frame.bottom, static_cast<ULONG>(haveNc), ncEnabled,
        static_cast<ULONG>(haveBorder), border, position ? position->flags : 0,
        position ? position->x : 0, position ? position->y : 0,
        position ? position->cx : 0, position ? position->cy : 0);
}

struct MenuScope;
thread_local MenuScope* t_scope = nullptr;
SRWLOCK g_scopesLock = SRWLOCK_INIT;
MenuScope* g_scopes = nullptr;

bool IsClassicPopup(HWND hwnd) {
    return hwnd && GetClassLongPtrW(hwnd, GCW_ATOM) == 32768;
}

bool ConflictingModuleLoaded() {
    constexpr PCWSTR names[] = {
        L"StartAllBackX64.dll", L"StartIsBack64.dll",
        L"StartIsBack32.dll",   L"TFMain64.dll",
        L"TFMain32.dll",        L"TranslucentFlyouts.dll"};
    for (PCWSTR name : names) {
        if (GetModuleHandleW(name))
            return true;
    }
    return false;
}

bool EffectsAllowed() {
    if (g_settings.material == Material::Native)
        return false;
    if (g_stopping.load(std::memory_order_relaxed) || ConflictingModuleLoaded())
        return false;
    if (g_settings.ctrlBypass && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
        return false;
    HIGHCONTRASTW hc{sizeof(hc)};
    BOOL composition = FALSE;
    if (!SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) ||
        (hc.dwFlags & HCF_HIGHCONTRASTON) ||
        GetSystemMetrics(SM_REMOTESESSION) ||
        FAILED(DwmIsCompositionEnabled(&composition)) || !composition ||
        !IsAppThemed())
        return false;

    DWORD enabled = 1;
    DWORD size = sizeof(enabled);
    // Missing value means Windows default (enabled). Other read errors bypass.
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &enabled, &size);
    return status == ERROR_FILE_NOT_FOUND ||
           (status == ERROR_SUCCESS && enabled);
}

bool UsesDarkMenus() {
    if (g_settings.theme)
        return g_settings.theme == 2;
    DWORD light = 1;
    DWORD size = sizeof(light);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);
    return !light;
}

// Only recognize the shell's own owner-draw PROPERTY. Never dereference its
// private item-data structure (its layout changes across Windows releases).
bool IsSupportedMenu(HMENU menu, HWND owner, bool* shellOwnerDraw = nullptr) {
    if (shellOwnerDraw)
        *shellOwnerDraw = false;
    int count = GetMenuItemCount(menu);
    if (count <= 0 || count > 4096)
        return false;
    for (int i = 0; i < count; ++i) {
        MENUITEMINFOW item{sizeof(item)};
        item.fMask = MIIM_FTYPE | MIIM_DATA | MIIM_ID;
        if (!GetMenuItemInfoW(menu, i, TRUE, &item))
            return false;
        if (item.fType & MFT_OWNERDRAW) {
            wchar_t property[96];
            swprintf_s(property, L"ImmersiveContextMenuArray_%lu-%lu",
                       static_cast<ULONG>(item.dwItemData),
                       static_cast<ULONG>(item.wID));
            if (!owner || !GetPropW(owner, property))
                return false;
            if (shellOwnerDraw)
                *shellOwnerDraw = true;
        }
    }
    return true;
}

bool ApplyBackdrop(Popup& popup) {
    if (popup.applying)
        return false;
    popup.applying = true;
    bool success = false;
    BOOL dark = popup.dark;
    // Apply theme before the material; changing it afterwards can invalidate
    // the DWM recipe that was just configured.
    DwmSetWindowAttribute(popup.hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
                          sizeof(dark));
    if (IsSystemMaterial(g_settings.material)) {
        // Preserve the menu's native non-client policy. Forcing DWMNCRP_ENABLED
        // adds a delayed DWM frame: at 144 DPI an owned child contracted by one
        // physical pixel about 170 ms after showing, despite a fixed HWND rect.
        // The previous version never extended the frame over the client area.
        // An accepted backdrop attribute alone doesn't establish that surface.
        MARGINS margins{-1, -1, -1, -1};
        DWORD backdrop =
            g_settings.material == Material::Mica      ? DWMSBT_MAINWINDOW
            : g_settings.material == Material::MicaAlt ? DWMSBT_TABBEDWINDOW
                                                       : DWMSBT_TRANSIENTWINDOW;
        popup.frameExtended =
            SUCCEEDED(DwmExtendFrameIntoClientArea(popup.hwnd, &margins));
        success = popup.frameExtended &&
                  SUCCEEDED(DwmSetWindowAttribute(popup.hwnd,
                                                  DWMWA_SYSTEMBACKDROP_TYPE,
                                                  &backdrop, sizeof(backdrop)));
        // Backdrop/alpha first, Accent disabled last. A menu never activates,
        // so notify DWM of the active appearance without requesting another
        // window-position/frame calculation after native menu layout.
        if (g_setComposition) {
            AccentPolicy disabled{};
            CompositionData data{19, &disabled, sizeof(disabled)};
            if (g_setComposition(popup.hwnd, &data))
                popup.accentChanged = true;
        }
        if (success) {
            DefWindowProcW(popup.hwnd, WM_NCACTIVATE, TRUE, 0);
        }
    } else if (g_setComposition) {
        // Do not stack the fixed, bright system Acrylic recipe with Accent.
        DWORD backdrop = DWMSBT_NONE;
        DwmSetWindowAttribute(popup.hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop,
                              sizeof(backdrop));
        int opacity = g_settings.material == Material::Solid
                          ? 100
                          : ClampPercent(g_settings.tintOpacity);
        // Alpha=0 has special semantics on some builds, so keep it nonzero.
        unsigned alpha = (opacity * 255 + 50) / 100;
        if (!alpha)
            alpha = 1;
        COLORREF tint = popup.dark ? g_settings.darkTint : g_settings.lightTint;
        int state = 4, flags = 0;
        switch (g_settings.material) {
            case Material::Solid:
                state = 1;
                flags = 2;
                break;
            case Material::Transparent:
                state = 2;
                flags = 2;
                break;
            case Material::ModernAcrylic:
                flags = 2;
                break;
            default:
                break;
        }
        AccentPolicy accent{state, flags, (alpha << 24) | tint, 0};
        CompositionData data{19, &accent, sizeof(accent)};
        success = g_setComposition(popup.hwnd, &data) != FALSE;
        if (success)
            popup.accentChanged = true;
    }
    DWORD corner =
        g_settings.roundedCorners ? DWMWCP_ROUNDSMALL : DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(popup.hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner,
                          sizeof(corner));
    if (g_settings.hideMenuBorder) {
        COLORREF border = DWMWA_COLOR_NONE;
        popup.borderColorChanged = SUCCEEDED(DwmSetWindowAttribute(
            popup.hwnd, DWMWA_BORDER_COLOR, &border, sizeof(border)));
    }
    popup.applying = false;
    return success;
}

void RestorePopup(Popup& popup, bool destroying = false) {
    if (!popup.active)
        return;
    if (g_settings.debugLogging)
        Wh_Log(
            L"Restore %p backgrounds=%u shellText=%u themeText=%u icons=%u "
            L"highlights=%u",
            popup.hwnd, popup.backgroundDraws, popup.shellTextDraws,
            popup.themeTextDraws, popup.iconRepairs, popup.highlights);
    popup.active =
        false;  // Do not intercept reentrant painting during restoration.
    if (popup.palette) {
        CloseThemeData(popup.palette);
        popup.palette = nullptr;
    }
    if (popup.brushChanged && IsMenu(popup.menu)) {
        MENUINFO info{sizeof(info)};
        info.fMask = MIM_BACKGROUND;
        if (GetMenuInfo(popup.menu, &info) &&
            info.hbrBack == GetStockObject(BLACK_BRUSH)) {
            info.hbrBack = popup.oldBrush;
            SetMenuInfo(popup.menu, &info);
        }
    }
    // A dying popup loses its DWM state with the HWND. Restoring bright/native
    // attributes while it is still visible can expose a final flash.
    if (destroying || !IsWindow(popup.hwnd))
        return;
    if (popup.accentChanged && g_setComposition) {
        AccentPolicy accent =
            popup.haveAccent ? popup.oldAccent : AccentPolicy{};
        CompositionData data{19, &accent, sizeof(accent)};
        g_setComposition(popup.hwnd, &data);
    }
    if (popup.frameExtended) {
        MARGINS margins{};
        DwmExtendFrameIntoClientArea(popup.hwnd, &margins);
    }
    DWM_BLURBEHIND blur{};
    blur.dwFlags = DWM_BB_ENABLE;
    DwmEnableBlurBehindWindow(popup.hwnd, &blur);
    DWORD backdrop = popup.haveBackdrop ? popup.oldBackdrop : DWMSBT_AUTO;
    DwmSetWindowAttribute(popup.hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop,
                          sizeof(backdrop));
    if (popup.haveCorner)
        DwmSetWindowAttribute(popup.hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                              &popup.oldCorner, sizeof(popup.oldCorner));
    if (popup.haveDark)
        DwmSetWindowAttribute(popup.hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE,
                              &popup.oldDark, sizeof(popup.oldDark));
    if (popup.borderColorChanged) {
        COLORREF border =
            popup.haveBorderColor ? popup.oldBorderColor : DWMWA_COLOR_DEFAULT;
        DwmSetWindowAttribute(popup.hwnd, DWMWA_BORDER_COLOR, &border,
                              sizeof(border));
    }
}

LRESULT CALLBACK MenuMessages(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MenuOwnerSubclass(HWND hwnd,
                                   UINT message,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   UINT_PTR id,
                                   DWORD_PTR data);
LRESULT CALLBACK MenuPopupSubclass(HWND hwnd,
                                   UINT message,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   UINT_PTR id,
                                   DWORD_PTR data);

struct MenuScope {
    MenuScope* previous = nullptr;
    MenuScope* next = nullptr;
    DWORD threadId = GetCurrentThreadId();
    HHOOK messageHook = nullptr;
    bool ownerSubclass = false;
    HWND owner = nullptr;
    Popup popups[32]{};
    unsigned count = 0;
    HWND paintingWindow = nullptr;
    const DRAWITEMSTRUCT* drawingItem = nullptr;
    bool allowed = false;

    explicit MenuScope(HWND menuOwner, bool requestEffects)
        : previous(t_scope), owner(menuOwner) {
        // Even a bypassed nested call must hide the outer call's drawing scope.
        t_scope = this;
        AcquireSRWLockExclusive(&g_scopesLock);
        next = g_scopes;
        g_scopes = this;
        ReleaseSRWLockExclusive(&g_scopesLock);
        if (!requestEffects)
            return;
        messageHook = SetWindowsHookExW(WH_CALLWNDPROC, MenuMessages, nullptr,
                                        GetCurrentThreadId());
        if (!messageHook) {
            Wh_Log(L"Could not observe this menu thread: %lu", GetLastError());
            return;
        }
        // The owner is on this UI thread while its menu is tracked. A real
        // subclass gives WM_DRAWITEM a synchronous, valid parameter lifetime;
        // menu-window CWP/return notifications are not guaranteed to be paired.
        if (!owner ||
            GetWindowThreadProcessId(owner, nullptr) != GetCurrentThreadId() ||
            !SetWindowSubclass(owner, MenuOwnerSubclass,
                               reinterpret_cast<UINT_PTR>(this),
                               reinterpret_cast<DWORD_PTR>(this))) {
            UnhookWindowsHookEx(messageHook);
            messageHook = nullptr;
            return;
        }
        ownerSubclass = true;
        // Stay inert through setup. The tracking wrapper enables rendering
        // only after buffered painting and its final bypass check succeed.
    }

    bool Ready() const { return messageHook && ownerSubclass; }

    void DetachCallbacks() {
        allowed = false;
        if (messageHook) {
            UnhookWindowsHookEx(messageHook);
            messageHook = nullptr;
        }
        if (ownerSubclass) {
            RemoveWindowSubclass(owner, MenuOwnerSubclass,
                                 reinterpret_cast<UINT_PTR>(this));
            ownerSubclass = false;
        }
    }

    ~MenuScope() {
        // Keep the scope on TLS until the callbacks are unregistered.
        DetachCallbacks();
        for (unsigned i = 0; i < count; ++i) {
            if (popups[i].subclassed)
                RemoveWindowSubclass(popups[i].hwnd, MenuPopupSubclass,
                                     reinterpret_cast<UINT_PTR>(this));
            RestorePopup(popups[i]);
        }
        t_scope = previous;
        AcquireSRWLockExclusive(&g_scopesLock);
        MenuScope** cursor = &g_scopes;
        while (*cursor && *cursor != this)
            cursor = &(*cursor)->next;
        if (*cursor)
            *cursor = next;
        ReleaseSRWLockExclusive(&g_scopesLock);
    }

    Popup* Find(HWND hwnd) {
        for (unsigned i = 0; i < count; ++i)
            if (popups[i].hwnd == hwnd)
                return &popups[i];
        return nullptr;
    }

    Popup* Watch(HWND hwnd) {
        if (!allowed || g_stopping.load(std::memory_order_relaxed))
            return nullptr;
        Popup* popup = Find(hwnd);
        if (!popup) {
            if (count == ARRAYSIZE(popups) || !IsClassicPopup(hwnd))
                return nullptr;
            popup = &popups[count++];
            *popup = {};
            popup->hwnd = hwnd;
            popup->observedAt = GetTickCount64();
        }
        if (!popup->subclassed)
            popup->subclassed =
                SetWindowSubclass(hwnd, MenuPopupSubclass,
                                  reinterpret_cast<UINT_PTR>(this),
                                  reinterpret_cast<DWORD_PTR>(this)) != FALSE;
        return popup->subclassed ? popup : nullptr;
    }

    void Prepare(HWND hwnd, PCWSTR phase = L"owner-draw-fallback") {
        Popup* reusable = Watch(hwnd);
        if (!reusable || reusable->active || reusable->preparing ||
            reusable->layingOut)
            return;
        struct Preparation {
            Popup* popup;
            explicit Preparation(Popup* value) : popup(value) {
                popup->preparing = true;
            }
            ~Preparation() { popup->preparing = false; }
        } preparation(reusable);
        MENUBARINFO bar{sizeof(bar)};
        bool shellOwnerDraw = false;
        // During early layout the HWND can exist before it has a valid HMENU.
        // Do not cache that transient failure as a permanently skipped popup.
        if (!GetMenuBarInfo(hwnd, OBJID_CLIENT, 0, &bar) || !bar.hMenu ||
            GetMenuItemCount(bar.hMenu) <= 0)
            return;
        if (!IsSupportedMenu(bar.hMenu, owner, &shellOwnerDraw)) {
            if (g_settings.debugLogging)
                Wh_Log(L"Native fallback: unsupported menu %p", hwnd);
            return;
        }
        MENUINFO info{sizeof(info)};
        info.fMask = MIM_BACKGROUND;
        if (!GetMenuInfo(bar.hMenu, &info))
            return;
        if (!shellOwnerDraw && info.hbrBack &&
            info.hbrBack != GetSysColorBrush(COLOR_MENU) &&
            info.hbrBack != reinterpret_cast<HBRUSH>(COLOR_MENU + 1)) {
            if (g_settings.debugLogging)
                Wh_Log(L"Native fallback: custom brush %p", hwnd);
            return;
        }
        Popup& popup = *reusable;
        bool subclassed = popup.subclassed;
        ULONGLONG observedAt = popup.observedAt;
        popup = {};
        popup.subclassed = subclassed;
        popup.observedAt = observedAt;
        popup.preparing = true;
        popup.hwnd = hwnd;
        popup.menu = bar.hMenu;
        popup.oldBrush = info.hbrBack;
        popup.dark = UsesDarkMenus();
        popup.haveBackdrop = SUCCEEDED(DwmGetWindowAttribute(
            hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &popup.oldBackdrop,
            sizeof(popup.oldBackdrop)));
        popup.haveCorner = SUCCEEDED(
            DwmGetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                                  &popup.oldCorner, sizeof(popup.oldCorner)));
        popup.haveDark = SUCCEEDED(
            DwmGetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE,
                                  &popup.oldDark, sizeof(popup.oldDark)));
        if (g_settings.hideMenuBorder)
            popup.haveBorderColor = SUCCEEDED(DwmGetWindowAttribute(
                hwnd, DWMWA_BORDER_COLOR, &popup.oldBorderColor,
                sizeof(popup.oldBorderColor)));
        if (g_getComposition) {
            CompositionData data{19, &popup.oldAccent, sizeof(popup.oldAccent)};
            popup.haveAccent = g_getComposition(hwnd, &data) != FALSE;
        }
        popup.active = true;
        popup.palette =
            OpenThemeData(hwnd, popup.dark ? L"DarkMode::Menu" : L"Menu");
        if (!popup.palette) {
            RestorePopup(popup);
            return;
        }
        HRGN region = CreateRectRgn(0, 0, -1, -1);
        if (!region) {
            RestorePopup(popup);
            return;
        }
        DWM_BLURBEHIND blur{DWM_BB_ENABLE | DWM_BB_BLURREGION, TRUE, region,
                            FALSE};
        HRESULT result = DwmEnableBlurBehindWindow(hwnd, &blur);
        DeleteObject(region);
        if (FAILED(result)) {
            RestorePopup(popup);
            return;
        }
        info.hbrBack = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        if (!SetMenuInfo(popup.menu, &info)) {
            RestorePopup(popup);
            return;
        }
        popup.brushChanged = true;
        if (!ApplyBackdrop(popup)) {
            RestorePopup(popup);
            return;
        }
        if (g_settings.debugLogging)
            Wh_Log(
                L"Prepared %p menu=%p material=%d shell=%d tint=%d "
                L"phase=%s visible=%d hideBorder=%d "
                L"borderSet=%d",
                hwnd, popup.menu, static_cast<int>(g_settings.material),
                shellOwnerDraw, g_settings.tintOpacity, phase,
                IsWindowVisible(hwnd), g_settings.hideMenuBorder,
                popup.borderColorChanged);
        TracePopupGeometry(popup, phase);
    }

    void PrepareMenu(HMENU menu) {
        for (unsigned i = 0; i < count; ++i)
            if (popups[i].menu == menu && popups[i].active)
                return;
        struct Search {
            MenuScope* scope;
            HMENU menu;
        } search{this, menu};
        EnumThreadWindows(
            GetCurrentThreadId(),
            [](HWND hwnd, LPARAM param) -> BOOL {
                auto& search = *reinterpret_cast<Search*>(param);
                if (IsClassicPopup(hwnd)) {
                    MENUBARINFO bar{sizeof(bar)};
                    if (GetMenuBarInfo(hwnd, OBJID_CLIENT, 0, &bar) &&
                        bar.hMenu == search.menu) {
                        search.scope->Prepare(hwnd);
                        return FALSE;
                    }
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&search));
    }
};

LRESULT CALLBACK MenuMessages(int code, WPARAM wParam, LPARAM lParam) {
    if (t_observerDepth)
        return CallNextHookEx(nullptr, code, wParam, lParam);
    ObserverGuard observer;
    MenuScope* scope = t_scope;
    if (code >= 0 && scope && scope->allowed) {
        const auto& message = *reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (IsClassicPopup(message.hwnd)) {
            if (message.message == kMenuSizeWindow ||
                message.message == WM_WINDOWPOSCHANGING)
                scope->Watch(message.hwnd);
            // Compatibility fallback if a popup did not expose the layout
            // message. Never initialize a backdrop from a non-client message.
            if (message.message == WM_PAINT ||
                message.message == WM_ERASEBKGND ||
                message.message == WM_PRINT ||
                message.message == WM_PRINTCLIENT)
                scope->Prepare(message.hwnd, L"client-fallback");
            // Keep the actual owner-draw context intact across internal menu
            // queries performed during a WM_DRAWITEM call.
            if (!scope->drawingItem)
                scope->paintingWindow = message.hwnd;
            if (message.message == WM_DESTROY) {
                if (Popup* popup = scope->Find(message.hwnd))
                    RestorePopup(*popup, true);
                if (scope->paintingWindow == message.hwnd)
                    scope->paintingWindow = nullptr;
            }
            if (message.message == WM_NCDESTROY) {
                // The popup subclass removes itself before clearing the HWND.
                // Clearing it here would make its synchronous cleanup miss it.
                if (Popup* popup = scope->Find(message.hwnd);
                    popup && !popup->subclassed)
                    popup->hwnd = nullptr;
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

HRESULT WINAPI DwmSetWindowAttributeHook(HWND hwnd,
                                         DWORD attribute,
                                         LPCVOID value,
                                         DWORD size) {
    CallGuard call;
    COLORREF hidden = DWMWA_COLOR_NONE;
    Popup* popup = nullptr;
    if (attribute == DWMWA_BORDER_COLOR && value && size == sizeof(COLORREF) &&
        g_settings.hideMenuBorder && t_scope && t_scope->allowed &&
        !g_stopping.load(std::memory_order_relaxed)) {
        popup = t_scope->Find(hwnd);
        if (!popup || !popup->active)
            popup = nullptr;
    }
    // Shell/UxTheme can set the border again after post-layout preparation,
    // including when showing a child. Protect only an already modified HWND;
    // native/bypassed/unsupported menus and unrelated windows pass through.
    COLORREF requested = popup ? *static_cast<const COLORREF*>(value) : 0;
    HRESULT result =
        g_dwmSetWindowAttribute(hwnd, attribute, popup ? &hidden : value, size);
    if (popup && SUCCEEDED(result)) {
        popup->borderColorChanged = true;
        if (requested != hidden && popup->borderOverrides++ < 4 &&
            g_settings.debugLogging) {
            DWORD error = GetLastError();
            Wh_Log(
                L"BorderOverride %p requested=0x%08lx effective=0x%08lx "
                L"result=0x%08lx visible=%d",
                hwnd, requested, hidden, result, IsWindowVisible(hwnd));
            SetLastError(error);
        }
    }
    return result;
}

bool ClearBorderPixels(HDC dc, const RECT& outer, const RECT& client) {
    int saved = SaveDC(dc);
    if (!saved)
        return false;
    bool success = ExcludeClipRect(dc, client.left, client.top, client.right,
                                   client.bottom) != ERROR &&
                   PatBlt(dc, outer.left, outer.top, outer.right - outer.left,
                          outer.bottom - outer.top, BLACKNESS);
    RestoreDC(dc, saved);
    return success;
}

bool GetBorderRects(HWND hwnd, RECT& outer, RECT& client) {
    if (!GetWindowRect(hwnd, &outer) || !GetClientRect(hwnd, &client))
        return false;
    SetLastError(ERROR_SUCCESS);
    if (!MapWindowPoints(hwnd, HWND_DESKTOP, reinterpret_cast<POINT*>(&client),
                         2) &&
        GetLastError())
        return false;
    OffsetRect(&client, -outer.left, -outer.top);
    OffsetRect(&outer, -outer.left, -outer.top);
    return true;
}

bool PaintBorderlessFrame(HWND hwnd, WPARAM updateRegion) {
    RECT outer{}, client{};
    if (!GetBorderRects(hwnd, outer, client))
        return false;
    HRGN clip = nullptr;
    if (updateRegion > 1) {
        clip = CreateRectRgn(0, 0, 0, 0);
        if (!clip)
            return false;
        if (CombineRgn(clip, reinterpret_cast<HRGN>(updateRegion), nullptr,
                       RGN_COPY) == ERROR) {
            DeleteObject(clip);
            return false;
        }
    }
    HDC dc = GetDCEx(hwnd, clip,
                     DCX_WINDOW | DCX_CACHE | (clip ? DCX_INTERSECTRGN : 0));
    if (!dc) {
        if (clip)
            DeleteObject(clip);
        return false;
    }
    // GetDCEx owns the copied region after success. The message's original
    // region remains valid if rendering fails and native processing is needed.
    bool success = ClearBorderPixels(dc, outer, client);
    ReleaseDC(hwnd, dc);
    return success;
}

LRESULT CALLBACK MenuPopupSubclass(HWND hwnd,
                                   UINT message,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   UINT_PTR id,
                                   DWORD_PTR data) {
    auto scope = reinterpret_cast<MenuScope*>(data);
    Popup* popup = scope->Find(hwnd);
    if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, MenuPopupSubclass, id);
        if (popup) {
            RestorePopup(*popup, true);
            popup->subclassed = false;
            popup->hwnd = nullptr;
        }
        return DefSubclassProc(hwnd, message, wParam, lParam);
    }
    if (!popup || scope != t_scope || !scope->allowed ||
        g_stopping.load(std::memory_order_relaxed))
        return DefSubclassProc(hwnd, message, wParam, lParam);
    if (message == WM_NCPAINT && popup->active && g_settings.hideMenuBorder &&
        PaintBorderlessFrame(hwnd, wParam))
        return 0;
    if (message == WM_PRINT && popup->active && g_settings.hideMenuBorder &&
        (lParam & PRF_NONCLIENT)) {
        LRESULT result = DefSubclassProc(hwnd, message, wParam, lParam);
        RECT outer{}, client{};
        if (GetBorderRects(hwnd, outer, client))
            ClearBorderPixels(reinterpret_cast<HDC>(wParam), outer, client);
        return result;
    }
    if (message == kMenuSizeWindow) {
        // Unlike CWP return notifications, a real subclass guarantees a
        // synchronous after-layout point. Windows has finished initializing
        // the popup, but hasn't presented its first frame yet.
        bool wasLayingOut = popup->layingOut;
        popup->layingOut = true;
        LRESULT result = DefSubclassProc(hwnd, message, wParam, lParam);
        popup->layingOut = wasLayingOut;
        if (!wasLayingOut)
            scope->Prepare(hwnd, L"post-layout");
        return result;
    }
    if (message == WM_WINDOWPOSCHANGING && lParam &&
        (reinterpret_cast<const WINDOWPOS*>(lParam)->flags & SWP_SHOWWINDOW))
        scope->Prepare(hwnd, L"pre-show");
    if (message == WM_WINDOWPOSCHANGED && lParam) {
        WINDOWPOS position = *reinterpret_cast<const WINDOWPOS*>(lParam);
        bool showing = (position.flags & SWP_SHOWWINDOW) != 0;
        TracePopupGeometry(
            *popup, showing ? L"show-before-native" : L"position-before-native",
            &position);
        LRESULT result = DefSubclassProc(hwnd, message, wParam, lParam);
        TracePopupGeometry(
            *popup, showing ? L"show-after-native" : L"position-after-native",
            &position);
        return result;
    } else if (message == WM_NCCALCSIZE) {
        TracePopupGeometry(*popup, L"nc-calc");
    } else if (message == WM_NCACTIVATE) {
        TracePopupGeometry(*popup, wParam ? L"nc-active" : L"nc-inactive");
    } else if (message == WM_PAINT) {
        TracePopupGeometry(*popup, L"paint");
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK MenuOwnerSubclass(HWND hwnd,
                                   UINT message,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   UINT_PTR id,
                                   DWORD_PTR data) {
    auto scope = reinterpret_cast<MenuScope*>(data);
    if (scope != t_scope || !scope->allowed)
        return DefSubclassProc(hwnd, message, wParam, lParam);
    struct DrawingContext {
        MenuScope* scope;
        HWND oldWindow;
        const DRAWITEMSTRUCT* oldItem;
        ~DrawingContext() {
            scope->paintingWindow = oldWindow;
            scope->drawingItem = oldItem;
        }
    } context{scope, scope->paintingWindow, scope->drawingItem};
    scope->paintingWindow = nullptr;
    scope->drawingItem = nullptr;
    if (message == WM_DRAWITEM && lParam) {
        const auto& item = *reinterpret_cast<const DRAWITEMSTRUCT*>(lParam);
        if (item.CtlType == ODT_MENU) {
            scope->PrepareMenu(reinterpret_cast<HMENU>(item.hwndItem));
            for (unsigned i = 0; i < scope->count; ++i) {
                Popup& popup = scope->popups[i];
                if (popup.menu == reinterpret_cast<HMENU>(item.hwndItem) &&
                    popup.active) {
                    scope->paintingWindow = popup.hwnd;
                    scope->drawingItem =
                        &item;  // Valid until DefSubclassProc returns.
                    break;
                }
            }
        }
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

// Gate every drawing hook by BOTH a live classic popup and its theme class.
// A DC belonging to another window must never fall back to the memory-DC path.
Popup* DrawingPopupForDC(HDC dc) {
    MenuScope* scope = t_scope;
    if (!scope || !scope->allowed || g_stopping.load(std::memory_order_relaxed))
        return nullptr;
    // Shell may paint an owner-drawn item using a DC associated with its owner.
    // Accept that DC only when it is exactly the verified WM_DRAWITEM DC.
    HWND hwnd = scope->drawingItem && dc == scope->drawingItem->hDC
                    ? scope->paintingWindow
                    : WindowFromDC(dc);
    if (hwnd && !IsClassicPopup(hwnd))
        return nullptr;
    if (!hwnd)
        hwnd = scope->paintingWindow;
    Popup* popup = scope->Find(hwnd);
    return popup && popup->active ? popup : nullptr;
}

Popup* DrawingPopup(HTHEME theme, HDC dc) {
    Popup* popup = DrawingPopupForDC(dc);
    if (!popup)
        return nullptr;
    wchar_t name[64]{};
    if (FAILED(g_getThemeClass(theme, name, ARRAYSIZE(name))) ||
        _wcsicmp(name, L"Menu") != 0)
        return nullptr;
    return popup;
}

bool DrawHighlight(Popup& popup, HDC dc, const RECT* rect, const RECT* clip) {
    if (!rect || IsRectEmpty(rect) || !g_settings.highlightOpacity)
        return false;
    TextGuard guard;
    int saved = SaveDC(dc);
    if (!saved)
        return false;
    if (clip)
        IntersectClipRect(dc, clip->left, clip->top, clip->right, clip->bottom);
    BP_PAINTPARAMS params{sizeof(params)};
    params.dwFlags = BPPF_ERASE;
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    params.pBlendFunction = &blend;
    HDC bufferDC = nullptr;
    HPAINTBUFFER buffer =
        BeginBufferedPaint(dc, rect, BPBF_TOPDOWNDIB, &params, &bufferDC);
    if (!buffer) {
        RestoreDC(dc, saved);
        return false;
    }
    RGBQUAD* bits = nullptr;
    int stride = 0;
    HRESULT result = GetBufferedPaintBits(buffer, &bits, &stride);
    if (SUCCEEDED(result) && bits) {
        UINT dpi = GetDpiForWindow(popup.hwnd);
        if (!dpi)
            dpi = 96;
        float inset = static_cast<float>(MulDiv(2, dpi, 96));
        float radius = static_cast<float>(MulDiv(4, dpi, 96));
        int width = rect->right - rect->left, height = rect->bottom - rect->top;
        COLORREF color = popup.dark ? RGB(128, 192, 255) : RGB(0, 105, 190);
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x) {
                float px = x + 0.5f, py = y + 0.5f;
                float cx = px < inset + radius ? inset + radius
                           : px > width - inset - radius
                               ? width - inset - radius
                               : px;
                float cy = py < 1 + radius            ? 1 + radius
                           : py > height - 1 - radius ? height - 1 - radius
                                                      : py;
                float distance =
                    std::sqrt((px - cx) * (px - cx) + (py - cy) * (py - cy));
                float coverage = radius + 0.5f - distance;
                if (coverage < 0)
                    coverage = 0;
                if (coverage > 1)
                    coverage = 1;
                unsigned alpha = static_cast<unsigned>(
                    coverage * ClampPercent(g_settings.highlightOpacity) *
                        2.55f +
                    0.5f);
                bits[y * stride + x] = {
                    static_cast<BYTE>((GetBValue(color) * alpha + 127) / 255),
                    static_cast<BYTE>((GetGValue(color) * alpha + 127) / 255),
                    static_cast<BYTE>((GetRValue(color) * alpha + 127) / 255),
                    static_cast<BYTE>(alpha)};
            }
    }
    // Replace the background rather than repeatedly accumulating hover alpha.
    if (SUCCEEDED(result))
        PatBlt(dc, rect->left, rect->top, rect->right - rect->left,
               rect->bottom - rect->top, BLACKNESS);
    HRESULT end = EndBufferedPaint(buffer, SUCCEEDED(result));
    RestoreDC(dc, saved);
    if (SUCCEEDED(result) && SUCCEEDED(end)) {
        ++popup.highlights;
        return true;
    }
    return false;
}

bool PaintHotItem(HTHEME theme,
                  HDC dc,
                  int part,
                  int state,
                  const RECT* rect,
                  const RECT* clip) {
    bool keyboardFocus = part == kPopupItemKeyboardFocus;
    if (!keyboardFocus &&
        ((part != MENU_POPUPITEM && part != kPopupItemFocusable) ||
         (state != MPI_HOT && state != MPI_DISABLEDHOT)))
        return false;
    Popup* popup = DrawingPopup(theme, dc);
    return popup && DrawHighlight(*popup, dc, rect, clip);
}

bool ClearBackground(HTHEME theme,
                     HDC dc,
                     int part,
                     int state,
                     const RECT* rect,
                     const RECT* clip) {
    if (!rect || IsRectEmpty(rect))
        return false;
    switch (part) {
        case MENU_POPUPBACKGROUND:
        case MENU_POPUPBORDERS:
        case MENU_POPUPGUTTER:
            break;
        case MENU_POPUPITEM:
        case kPopupItemFocusable:
            if (state == MPI_HOT || state == MPI_DISABLEDHOT)
                return false;
            break;
        default:
            return false;  // Keep native highlights, checks, arrows and
                           // separators.
    }
    Popup* popup = DrawingPopup(theme, dc);
    if (!popup)
        return false;
    ++popup->backgroundDraws;
    RECT area = *rect;
    if (clip && !IntersectRect(&area, rect, clip))
        return true;
    // BLACKNESS writes zero-alpha pixels to a composited GDI menu surface.
    return PatBlt(dc, area.left, area.top, area.right - area.left,
                  area.bottom - area.top, BLACKNESS) != FALSE;
}

HRESULT WINAPI DrawThemeBackgroundHook(HTHEME theme,
                                       HDC dc,
                                       int part,
                                       int state,
                                       const RECT* rect,
                                       const RECT* clip) {
    CallGuard call;
    if (PaintHotItem(theme, dc, part, state, rect, clip))
        return S_OK;
    if (ClearBackground(theme, dc, part, state, rect, clip))
        return S_OK;
    if (part >= MENU_POPUPCHECK && part <= kPopupItemFocusable) {
        if (Popup* popup = DrawingPopup(theme, dc))
            theme = popup->palette;
    }
    return g_drawThemeBackground(theme, dc, part, state, rect, clip);
}

HRESULT WINAPI DrawThemeBackgroundExHook(HTHEME theme,
                                         HDC dc,
                                         int part,
                                         int state,
                                         const RECT* rect,
                                         const DTBGOPTS* opts) {
    CallGuard call;
    // Preserve uncommon omit-border/content and mirrored drawing options.
    if ((!opts || (opts->dwSize == sizeof(*opts) &&
                   !(opts->dwFlags & ~DTBG_CLIPRECT))) &&
        PaintHotItem(
            theme, dc, part, state, rect,
            opts && (opts->dwFlags & DTBG_CLIPRECT) ? &opts->rcClip : nullptr))
        return S_OK;
    if ((!opts || (opts->dwSize == sizeof(*opts) &&
                   !(opts->dwFlags & ~DTBG_CLIPRECT))) &&
        ClearBackground(
            theme, dc, part, state, rect,
            opts && (opts->dwFlags & DTBG_CLIPRECT) ? &opts->rcClip : nullptr))
        return S_OK;
    if (part >= MENU_POPUPCHECK && part <= kPopupItemFocusable) {
        if (Popup* popup = DrawingPopup(theme, dc))
            theme = popup->palette;
    }
    return g_drawThemeBackgroundEx(theme, dc, part, state, rect, opts);
}

bool DrawCompositedText(HTHEME theme,
                        HDC dc,
                        int part,
                        int state,
                        PCWSTR text,
                        int length,
                        DWORD flags,
                        RECT* rect,
                        const DTTOPTS* options,
                        HRESULT* result) {
    if (!rect || IsRectEmpty(rect) || (flags & DT_CALCRECT) ||
        (part != MENU_POPUPITEM && part != kPopupItemFocusable) ||
        (options && (options->dwSize != sizeof(*options) ||
                     (options->dwFlags & (DTT_CALCRECT | DTT_CALLBACK)))))
        return false;
    Popup* popup = DrawingPopup(theme, dc);
    if (!popup)
        return false;
    TextGuard textGuard;
    // Top-down DIB is required by DTT_COMPOSITED. Clear only the temporary
    // buffer; EndBufferedPaint alpha-blends the glyphs over the existing menu.
    BP_PAINTPARAMS paint{sizeof(paint)};
    paint.dwFlags = BPPF_ERASE;
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    paint.pBlendFunction = &blend;
    HDC bufferDC = nullptr;
    HPAINTBUFFER buffer =
        BeginBufferedPaint(dc, rect, BPBF_TOPDOWNDIB, &paint, &bufferDC);
    if (!buffer)
        return false;
    DTTOPTS opts{};
    if (options)
        opts = *options;
    opts.dwSize = sizeof(opts);
    opts.dwFlags |= DTT_COMPOSITED | DTT_TEXTCOLOR;
    bool disabled = state == MPI_DISABLED || state == MPI_DISABLEDHOT;
    opts.crText = popup->dark
                      ? (disabled ? RGB(155, 155, 155) : RGB(245, 245, 245))
                      : (disabled ? RGB(120, 120, 120) : RGB(25, 25, 25));
    HGDIOBJ font = GetCurrentObject(dc, OBJ_FONT);
    if (font)
        SelectObject(bufferDC, font);
    RECT bounds = *rect;
    *result = g_drawThemeTextEx(theme, bufferDC, part, state, text, length,
                                flags, &bounds, &opts);
    HRESULT end = EndBufferedPaint(buffer, SUCCEEDED(*result));
    if (SUCCEEDED(*result) && SUCCEEDED(end))
        ++popup->themeTextDraws;
    return SUCCEEDED(*result) && SUCCEEDED(end);
}

HRESULT WINAPI DrawThemeTextHook(HTHEME theme,
                                 HDC dc,
                                 int part,
                                 int state,
                                 PCWSTR text,
                                 int length,
                                 DWORD flags,
                                 DWORD flags2,
                                 const RECT* rect) {
    CallGuard call;
    RECT bounds = rect ? *rect : RECT{};
    HRESULT result;
    if (!flags2 && DrawCompositedText(theme, dc, part, state, text, length,
                                      flags, &bounds, nullptr, &result))
        return result;
    return g_drawThemeText(theme, dc, part, state, text, length, flags, flags2,
                           rect);
}

HRESULT WINAPI DrawThemeTextExHook(HTHEME theme,
                                   HDC dc,
                                   int part,
                                   int state,
                                   PCWSTR text,
                                   int length,
                                   DWORD flags,
                                   RECT* rect,
                                   const DTTOPTS* options) {
    CallGuard call;
    HRESULT result;
    if (DrawCompositedText(theme, dc, part, state, text, length, flags, rect,
                           options, &result))
        return result;
    return g_drawThemeTextEx(theme, dc, part, state, text, length, flags, rect,
                             options);
}

struct IconSurface {
    HBITMAP bitmap = nullptr;
    HDC dc = nullptr;
    HGDIOBJ old = nullptr;
    ~IconSurface() {
        if (old)
            SelectObject(dc, old);
        if (dc)
            DeleteDC(dc);
        if (bitmap)
            DeleteObject(bitmap);
    }
};

// Work only on an owned copy obtained through public GDI. Never modify a menu
// bitmap, the selected source bitmap, or Shell's private item-data structures.
bool PrepareIcon(HDC source, IconSurface& target) {
    HBITMAP original =
        static_cast<HBITMAP>(GetCurrentObject(source, OBJ_BITMAP));
    BITMAP sourceInfo{};
    if (!original ||
        GetObjectW(original, sizeof(sourceInfo), &sourceInfo) !=
            sizeof(sourceInfo) ||
        sourceInfo.bmBitsPixel <= 1 || sourceInfo.bmWidth < 2 ||
        sourceInfo.bmHeight < 2 || sourceInfo.bmWidth > 256 ||
        sourceInfo.bmHeight > 256)
        return false;
    HBITMAP copy = static_cast<HBITMAP>(
        CopyImage(original, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
    if (!copy)
        return false;
    int width = sourceInfo.bmWidth, height = sourceInfo.bmHeight;
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB};
    RGBQUAD* pixels = nullptr;
    target.bitmap =
        CreateDIBSection(nullptr, &info, DIB_RGB_COLORS,
                         reinterpret_cast<void**>(&pixels), nullptr, 0);
    // The copied bitmap is not selected into any DC (GetDIBits requirement).
    bool read = target.bitmap && pixels &&
                GetDIBits(source, copy, 0, height, pixels, &info,
                          DIB_RGB_COLORS) == height;
    DeleteObject(copy);
    if (!read)
        return false;
    bool anyAlpha = false, allOpaque = true, straightAlpha = false;
    for (int i = 0; i < width * height; ++i) {
        const auto& p = pixels[i];
        anyAlpha |= p.rgbReserved != 0;
        allOpaque &= p.rgbReserved == 255;
        if (p.rgbReserved && p.rgbReserved < 255 &&
            (p.rgbRed > p.rgbReserved || p.rgbGreen > p.rgbReserved ||
             p.rgbBlue > p.rgbReserved))
            straightAlpha = true;
    }
    bool legacy = sourceInfo.bmBitsPixel < 32 || !anyAlpha;
    if (legacy)
        for (int i = 0; i < width * height; ++i)
            pixels[i].rgbReserved = 255;

    // Remove only a connected corner background, never interior white/black
    // details. Existing semi-transparent assets keep their original alpha.
    if (g_settings.removeIconBackground && (legacy || allOpaque)) {
        RGBQUAD key = pixels[0];
        auto matchesKey = [key](const RGBQUAD& p) {
            return std::abs(int(p.rgbRed) - key.rgbRed) <= 8 &&
                   std::abs(int(p.rgbGreen) - key.rgbGreen) <= 8 &&
                   std::abs(int(p.rgbBlue) - key.rgbBlue) <= 8;
        };
        bool neutral =
            (key.rgbRed > 235 && key.rgbGreen > 235 && key.rgbBlue > 235) ||
            (key.rgbRed < 16 && key.rgbGreen < 16 && key.rgbBlue < 16) ||
            (key.rgbRed > 235 && key.rgbBlue > 235 && key.rgbGreen < 16);
        unsigned corners = matchesKey(pixels[width - 1]) +
                           matchesKey(pixels[(height - 1) * width]) +
                           matchesKey(pixels[width * height - 1]);
        if (neutral && corners >= 2) {
            auto queue = static_cast<unsigned*>(HeapAlloc(
                GetProcessHeap(), 0, width * height * sizeof(unsigned)));
            if (queue) {
                unsigned head = 0, tail = 0;
                auto add = [&](int index) {
                    if (pixels[index].rgbReserved &&
                        matchesKey(pixels[index])) {
                        pixels[index] = {};
                        queue[tail++] = static_cast<unsigned>(index);
                    }
                };
                add(0);
                add(width - 1);
                add((height - 1) * width);
                add(width * height - 1);
                while (head < tail) {
                    unsigned index = queue[head++];
                    int x = index % width, y = index / width;
                    if (x)
                        add(index - 1);
                    if (x + 1 < width)
                        add(index + 1);
                    if (y)
                        add(index - width);
                    if (y + 1 < height)
                        add(index + width);
                }
                HeapFree(GetProcessHeap(), 0, queue);
            }
        }
    }
    if (straightAlpha && !legacy)
        for (int i = 0; i < width * height; ++i) {
            auto& p = pixels[i];
            p.rgbRed = (p.rgbRed * p.rgbReserved + 127) / 255;
            p.rgbGreen = (p.rgbGreen * p.rgbReserved + 127) / 255;
            p.rgbBlue = (p.rgbBlue * p.rgbReserved + 127) / 255;
        }
    for (int i = 0; i < width * height; ++i)
        if (!pixels[i].rgbReserved)
            pixels[i] = {};
    target.dc = CreateCompatibleDC(source);
    if (!target.dc)
        return false;
    target.old = SelectObject(target.dc, target.bitmap);
    return target.old && target.old != HGDI_ERROR;
}

bool IsIconTransfer(HDC destination, HDC source, int width, int height) {
    return !t_bitmapDepth && !t_textDepth && g_settings.repairIcons &&
           t_scope && t_scope->drawingItem && destination != source &&
           width >= 4 && height >= 4 && width <= 128 && height <= 128 &&
           width * 2 >= height && height * 2 >= width &&
           DrawingPopupForDC(destination);
}

BOOL WINAPI BitBltHook(HDC destination,
                       int x,
                       int y,
                       int width,
                       int height,
                       HDC source,
                       int sx,
                       int sy,
                       DWORD operation) {
    CallGuard call;
    if (operation == SRCCOPY &&
        IsIconTransfer(destination, source, width, height)) {
        BitmapGuard guard;
        IconSurface icon;
        if (PrepareIcon(source, icon)) {
            BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            if (g_alphaBlend(destination, x, y, width, height, icon.dc, sx, sy,
                             width, height, blend)) {
                if (Popup* popup = DrawingPopupForDC(destination))
                    ++popup->iconRepairs;
                return TRUE;
            }
        }
    }
    return g_bitBlt(destination, x, y, width, height, source, sx, sy,
                    operation);
}

BOOL WINAPI StretchBltHook(HDC destination,
                           int x,
                           int y,
                           int width,
                           int height,
                           HDC source,
                           int sx,
                           int sy,
                           int sw,
                           int sh,
                           DWORD operation) {
    CallGuard call;
    if (operation == SRCCOPY &&
        IsIconTransfer(destination, source, width, height)) {
        BitmapGuard guard;
        IconSurface icon;
        if (PrepareIcon(source, icon)) {
            BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            if (g_alphaBlend(destination, x, y, width, height, icon.dc, sx, sy,
                             sw, sh, blend)) {
                if (Popup* popup = DrawingPopupForDC(destination))
                    ++popup->iconRepairs;
                return TRUE;
            }
        }
    }
    return g_stretchBlt(destination, x, y, width, height, source, sx, sy, sw,
                        sh, operation);
}

BOOL WINAPI AlphaBlendHook(HDC destination,
                           int x,
                           int y,
                           int width,
                           int height,
                           HDC source,
                           int sx,
                           int sy,
                           int sw,
                           int sh,
                           BLENDFUNCTION blend) {
    CallGuard call;
    if (IsIconTransfer(destination, source, width, height) &&
        blend.BlendOp == AC_SRC_OVER && !blend.BlendFlags) {
        BitmapGuard guard;
        IconSurface icon;
        if (PrepareIcon(source, icon)) {
            BLENDFUNCTION repairedBlend = blend;
            repairedBlend.AlphaFormat = AC_SRC_ALPHA;
            if (g_alphaBlend(destination, x, y, width, height, icon.dc, sx, sy,
                             sw, sh, repairedBlend)) {
                if (Popup* popup = DrawingPopupForDC(destination))
                    ++popup->iconRepairs;
                return TRUE;
            }
        }
    }
    return g_alphaBlend(destination, x, y, width, height, source, sx, sy, sw,
                        sh, blend);
}

int WINAPI FillRectHook(HDC dc, const RECT* rect, HBRUSH brush) {
    CallGuard call;
    Popup* popup = !t_textDepth && rect ? DrawingPopupForDC(dc) : nullptr;
    if (popup && brush) {
        const DRAWITEMSTRUCT* item = t_scope->drawingItem;
        bool selected = item && (item->itemState & ODS_SELECTED);
        bool fullRow = item &&
                       rect->right - rect->left >=
                           item->rcItem.right - item->rcItem.left - 12 &&
                       rect->bottom - rect->top >=
                           item->rcItem.bottom - item->rcItem.top - 2;
        bool background = brush == popup->oldBrush ||
                          brush == GetSysColorBrush(COLOR_MENU) ||
                          brush == reinterpret_cast<HBRUSH>(COLOR_MENU + 1) ||
                          brush == GetStockObject(BLACK_BRUSH);
        LOGBRUSH info{};
        if (!background &&
            GetObjectW(brush, sizeof(info), &info) == sizeof(info) &&
            info.lbStyle == BS_SOLID && info.lbColor == GetSysColor(COLOR_MENU))
            background = true;
        if (selected && fullRow && DrawHighlight(*popup, dc, rect, nullptr))
            return 1;
        if (background && !selected) {
            ++popup->backgroundDraws;
            return PatBlt(dc, rect->left, rect->top, rect->right - rect->left,
                          rect->bottom - rect->top, BLACKNESS)
                       ? 1
                       : 0;
        }
    }
    return g_fillRect(dc, rect, brush);
}

int WINAPI
DrawTextHook(HDC dc, PCWSTR text, int length, RECT* rect, UINT flags) {
    CallGuard call;
    Popup* popup =
        !t_textDepth && rect && !IsRectEmpty(rect) &&
                !(flags & (DT_CALCRECT | DT_MODIFYSTRING | DT_INTERNAL)) &&
                t_scope && t_scope->drawingItem &&
                t_scope->drawingItem->hDC == dc
            ? DrawingPopupForDC(dc)
            : nullptr;
    if (!popup)
        return g_drawText(dc, text, length, rect, flags);

    TextGuard textGuard;
    BP_PAINTPARAMS params{sizeof(params)};
    params.dwFlags = BPPF_ERASE;
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    params.pBlendFunction = &blend;
    HDC bufferDC = nullptr;
    HPAINTBUFFER buffer =
        BeginBufferedPaint(dc, rect, BPBF_TOPDOWNDIB, &params, &bufferDC);
    if (!buffer)
        return g_drawText(dc, text, length, rect, flags);
    if (HGDIOBJ font = GetCurrentObject(dc, OBJ_FONT))
        SelectObject(bufferDC, font);
    SetBkMode(bufferDC, TRANSPARENT);
    bool disabled =
        t_scope->drawingItem->itemState & (ODS_DISABLED | ODS_GRAYED);
    COLORREF color = popup->dark
                         ? (disabled ? RGB(155, 155, 155) : RGB(245, 245, 245))
                         : (disabled ? RGB(120, 120, 120) : RGB(25, 25, 25));
    SetTextColor(bufferDC, color);
    int height = 0;
    DTTOPTS opts{sizeof(opts)};
    opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_CALLBACK;
    opts.crText = color;
    opts.lParam = reinterpret_cast<LPARAM>(&height);
    opts.pfnDrawTextCallback = [](HDC target, LPWSTR content, int count,
                                  LPRECT bounds, UINT format,
                                  LPARAM param) -> int {
        int result = g_drawText(target, content, count, bounds, format);
        *reinterpret_cast<int*>(param) = result;
        return result;
    };
    RECT bounds = *rect;
    HRESULT result = g_drawThemeTextEx(popup->palette, bufferDC, MENU_POPUPITEM,
                                       disabled ? MPI_DISABLED : MPI_NORMAL,
                                       text, length, flags, &bounds, &opts);
    HRESULT end = EndBufferedPaint(buffer, SUCCEEDED(result));
    if (FAILED(result) || FAILED(end))
        return g_drawText(dc, text, length, rect, flags);
    ++popup->shellTextDraws;
    return height;
}

template <typename Invoke>
BOOL TrackMenu(HWND owner, UINT flags, Invoke invoke) {
    CallGuard call;
    DWORD incomingError = GetLastError();
    bool requestEffects = !t_bypassDepth && EffectsAllowed();
    bool buffered = false;
    BOOL result;
    DWORD resultError;
    {
        MenuScope scope(owner, requestEffects);
        // BufferedPaint must be initialized on each rendering thread. Pending
        // callbacks remain inert so initialization cannot modify a popup.
        if (scope.Ready())
            buffered = SUCCEEDED(BufferedPaintInit());
        if (buffered && EffectsAllowed())
            scope.allowed = true;
        if (scope.allowed) {
            SetLastError(incomingError);
            result = invoke(flags | TPM_NOANIMATION);
            resultError = GetLastError();
        } else {
            // Ctrl may have become pressed during setup, or setup may have
            // failed. Finish all rendering cleanup BEFORE invoking Windows,
            // and forward the original flags through every fallback path.
            scope.DetachCallbacks();
            if (buffered) {
                BufferedPaintUnInit();
                buffered = false;
            }
            struct Bypass {
                Bypass() { ++t_bypassDepth; }
                ~Bypass() { --t_bypassDepth; }
            } bypass;
            SetLastError(incomingError);
            result = invoke(flags);
            resultError = GetLastError();
        }
    }
    if (buffered)
        BufferedPaintUnInit();
    SetLastError(resultError);
    return result;
}

BOOL WINAPI TrackPopupMenuHook(HMENU menu,
                               UINT flags,
                               int x,
                               int y,
                               int reserved,
                               HWND owner,
                               const RECT* exclude) {
    return TrackMenu(owner, flags, [&](UINT actualFlags) {
        return g_trackPopupMenu(menu, actualFlags, x, y, reserved, owner,
                                exclude);
    });
}

BOOL WINAPI TrackPopupMenuExHook(HMENU menu,
                                 UINT flags,
                                 int x,
                                 int y,
                                 HWND owner,
                                 LPTPMPARAMS params) {
    return TrackMenu(owner, flags, [&](UINT actualFlags) {
        return g_trackPopupMenuEx(menu, actualFlags, x, y, owner, params);
    });
}

}  // namespace etm

BOOL Wh_ModInit() {
    using namespace etm;
    wchar_t path[MAX_PATH]{};
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path)))
        return FALSE;
    const wchar_t* name = wcsrchr(path, L'\\');
    if (_wcsicmp(name ? name + 1 : path, L"explorer.exe") != 0)
        return FALSE;
    // DWMWA_SYSTEMBACKDROP_TYPE requires Windows 11 22H2. RtlGetVersion avoids
    // manifest-dependent version virtualization; no symbol lookup is involved.
    using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
    auto versionFn = reinterpret_cast<RtlGetVersion_t>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion"));
    OSVERSIONINFOW version{sizeof(version)};
    if (!versionFn || versionFn(&version) < 0 ||
        version.dwBuildNumber < 22621) {
        Wh_Log(L"Requires Windows 11 22H2, build 22621 or newer");
        return FALSE;
    }
    PCWSTR backend = Wh_GetStringSetting(L"backgroundEffect");
    g_settings.material = ParseMaterial(backend);
    if (backend)
        Wh_FreeStringSetting(backend);
    if (g_settings.material == Material::Native) {
        Wh_Log(L"Native mode: no hooks registered, no window effects changed");
        return TRUE;
    }
    HMODULE theme = GetModuleHandleW(L"uxtheme.dll");
    g_getThemeClass = reinterpret_cast<GetThemeClass_t>(
        GetProcAddress(theme, MAKEINTRESOURCEA(74)));
    if (!g_getThemeClass)
        return FALSE;
    // Validate the undocumented export before registering any hooks.
    HTHEME probe = OpenThemeData(nullptr, L"Menu");
    wchar_t themeName[64]{};
    bool valid =
        probe &&
        SUCCEEDED(g_getThemeClass(probe, themeName, ARRAYSIZE(themeName))) &&
        _wcsicmp(themeName, L"Menu") == 0;
    if (probe)
        CloseThemeData(probe);
    if (!valid) {
        Wh_Log(L"Menu theme identification unavailable");
        return FALSE;
    }
    g_settings.tintOpacity = Wh_GetIntSetting(L"tintOpacity");
    HMODULE user = GetModuleHandleW(L"user32.dll");
    g_setComposition = reinterpret_cast<CompositionAttribute_t>(
        GetProcAddress(user, "SetWindowCompositionAttribute"));
    g_getComposition = reinterpret_cast<CompositionAttribute_t>(
        GetProcAddress(user, "GetWindowCompositionAttribute"));
    if (!IsSystemMaterial(g_settings.material) &&
        g_settings.material != Material::Native && !g_setComposition) {
        Wh_Log(L"Adjustable Acrylic is unavailable; choose System Acrylic");
        return FALSE;
    }
    PCWSTR lightTint = Wh_GetStringSetting(L"lightTint");
    g_settings.lightTint = ParseTint(lightTint, RGB(245, 245, 245));
    if (lightTint)
        Wh_FreeStringSetting(lightTint);
    PCWSTR darkTint = Wh_GetStringSetting(L"darkTint");
    g_settings.darkTint = ParseTint(darkTint, RGB(32, 32, 32));
    if (darkTint)
        Wh_FreeStringSetting(darkTint);
    PCWSTR themeSetting = Wh_GetStringSetting(L"theme");
    g_settings.theme = themeSetting && !_wcsicmp(themeSetting, L"dark")    ? 2
                       : themeSetting && !_wcsicmp(themeSetting, L"light") ? 1
                                                                           : 0;
    if (themeSetting)
        Wh_FreeStringSetting(themeSetting);
    g_settings.highlightOpacity =
        ClampPercent(Wh_GetIntSetting(L"highlightOpacity"));
    g_settings.repairIcons = Wh_GetIntSetting(L"repairIcons") != 0;
    g_settings.removeIconBackground =
        Wh_GetIntSetting(L"removeIconBackground") != 0;
    g_settings.roundedCorners = Wh_GetIntSetting(L"roundedCorners") != 0;
    g_settings.hideMenuBorder = Wh_GetIntSetting(L"hideMenuBorder") != 0;
    g_settings.ctrlBypass = Wh_GetIntSetting(L"ctrlBypass") != 0;
    g_settings.debugLogging = Wh_GetIntSetting(L"debugLogging") != 0;
    g_stopping.store(false, std::memory_order_relaxed);

#define ETM_HOOK(api, hook, original)                             \
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(api),         \
                            reinterpret_cast<void*>(hook),        \
                            reinterpret_cast<void**>(&original))) \
    return FALSE
    ETM_HOOK(TrackPopupMenu, TrackPopupMenuHook, g_trackPopupMenu);
    ETM_HOOK(TrackPopupMenuEx, TrackPopupMenuExHook, g_trackPopupMenuEx);
    ETM_HOOK(DrawThemeBackground, DrawThemeBackgroundHook,
             g_drawThemeBackground);
    ETM_HOOK(DrawThemeBackgroundEx, DrawThemeBackgroundExHook,
             g_drawThemeBackgroundEx);
    ETM_HOOK(DrawThemeText, DrawThemeTextHook, g_drawThemeText);
    ETM_HOOK(DrawThemeTextEx, DrawThemeTextExHook, g_drawThemeTextEx);
    ETM_HOOK(DrawTextW, DrawTextHook, g_drawText);
    ETM_HOOK(FillRect, FillRectHook, g_fillRect);
    ETM_HOOK(BitBlt, BitBltHook, g_bitBlt);
    ETM_HOOK(StretchBlt, StretchBltHook, g_stretchBlt);
    ETM_HOOK(AlphaBlend, AlphaBlendHook, g_alphaBlend);
    if (g_settings.hideMenuBorder) {
        ETM_HOOK(DwmSetWindowAttribute, DwmSetWindowAttributeHook,
                 g_dwmSetWindowAttribute);
    }
#undef ETM_HOOK
    Wh_Log(L"Initialized on Windows build %lu; only Explorer Win32 popups",
           version.dwBuildNumber);
    return TRUE;
}

void Wh_ModBeforeUninit() {
    etm::g_stopping.store(true, std::memory_order_relaxed);
    // Cancel active loops on their owning UI threads. Do not synchronously send
    // across threads or hold a lock while waiting for shell extensions.
    AcquireSRWLockShared(&etm::g_scopesLock);
    for (etm::MenuScope* scope = etm::g_scopes; scope; scope = scope->next) {
        if (scope->owner)
            PostMessageW(scope->owner, WM_CANCELMODE, 0, 0);
        PostThreadMessageW(scope->threadId, WM_CANCELMODE, 0, 0);
    }
    ReleaseSRWLockShared(&etm::g_scopesLock);
}

void Wh_ModUninit() {
    // Windhawk has now removed the detours, so no new calls can enter. A thread
    // may still be inside TrackPopupMenu or a drawing callback: drain
    // explicitly. There is deliberately no timeout that would leave a dangling
    // callback.
    while (etm::g_calls.load(std::memory_order_acquire))
        Sleep(10);
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload =
        TRUE;  // Never mutate settings concurrently with Explorer painting.
    return TRUE;
}
