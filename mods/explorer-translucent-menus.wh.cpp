// ==WindhawkMod==
// @id              explorer-translucent-menus
// @name            Explorer Translucent Menus
// @name:zh-CN      Explorer 上下文菜单半透明
// @description     Translucent materials and optional borderless classic context menus for File Explorer
// @description:zh-CN 对 Explorer 经典上下文菜单应用半透明材质和可选无边框
// @version         1.0.2
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
// https://github.com/ramensoftware/windhawk-mods/issues
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

This is an Explorer-only, menu-focused alternative to the archived
[TranslucentFlyouts](https://github.com/ALTaleX531/TranslucentFlyouts) project.
It covers classic popup menus hosted by `explorer.exe`, including desktop and
File Explorer context menus. It leaves the main Explorer window background,
ordinary controls, and other processes unchanged.

## Screenshots

Materials in light and dark themes

![materials](https://raw.githubusercontent.com/alkali210/my-windhawk-mods/main/explorer-translucent-menus/screenshots/01-materials-light-dark.png)

Tint and transparency adjustments

![tint](https://raw.githubusercontent.com/alkali210/my-windhawk-mods/main/explorer-translucent-menus/screenshots/02-tint-transparency.png)

Borders and submenus

![borders](https://raw.githubusercontent.com/alkali210/my-windhawk-mods/main/explorer-translucent-menus/screenshots/03-border-submenus.png)

## Requirements

- Windows 11 22H2 or later (build 22621+), 64-bit.
- Desktop composition and Windows transparency effects enabled.
- No other renderer modifying the same Explorer menus.

To open a classic context menu on Windows 11, select **Show more options** or
use **Shift+F10**. Select **Native** to disable all hooks. For a temporary bypass,
enable **Hold Ctrl to bypass** in the settings, then hold **Ctrl** before opening
a menu.

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
Transfers that declare per-pixel alpha, and bitmaps with ambiguous partial
alpha, retain the original GDI behavior. The mod does not guess whether an
existing alpha bitmap is straight or premultiplied.

## Scope and rendering approach

This mod keeps the Shell's immersive owner-draw enabled and preserves native
menu layout, commands, and shell extensions. Effects are scoped to supported
classic popups while a menu is tracked. Preparation validates resources before
applying effects and exposes the popup to drawing hooks only after commit.
Only successful state changes are recorded for cleanup. Unassociated memory
DCs are left to Windows; owner-draw memory DCs require an exact synchronous
WM_DRAWITEM match.
GDI and UxTheme hooks are installed in Explorer, but forward to Windows outside
a tracked menu. The scope is the popup effect, rather than a replacement for
Explorer's overall interface rendering.

[Translucent Windows](https://windhawk.net/mods/translucent-windows) and
[Win32 UI Modernizer](https://windhawk.net/mods/win32-ui-modernizer) already
offer translucent classic menus. Translucent Windows shares background-effect
configuration between windows and flyouts, with menu drawing integrated into
its broader theme-rendering path. Win32 UI Modernizer lets users disable its
Explorer section and other general features, but its modern menu modes disable
immersive owner-draw and also modernize ComboBox dropdowns.

The purpose of this separate implementation is to provide a dedicated popup
renderer that retains the Shell's owner-draw path and can accompany a separate
Explorer background renderer. Restricting process targets or adding material
presets alone does not provide that rendering behavior. An equivalent mode in
a broader UI mod would need to isolate menu rendering and preserve immersive
owner-draw as well.

## Compatibility

Menu effects from these mods conflict when enabled on the same Explorer popups:

- [Translucent Windows](https://windhawk.net/mods/translucent-windows):
  **Flyout effects** also renders classic menus, dropdowns, and tooltips.
  Disable that option to remove its flyout effects.
- [Win32 UI Modernizer](https://windhawk.net/mods/win32-ui-modernizer): set
  **Menus and dropdowns** to **Native (no modernization)**. Both modern modes
  apply their own acrylic rendering and disable immersive owner-draw.

These settings turn off their direct menu features. Their remaining Explorer
effects and shared drawing hooks depend on other settings, so compatibility
must be evaluated with the configuration in use. To avoid their remaining hooks
in Explorer while continuing to use them in other applications, exclude
`explorer.exe` from their targets in Windhawk's Advanced tab.

This mod is also compatible with
[ExplorerBlurMica](https://github.com/Maplespe/ExplorerBlurMica), which supplies
the main Explorer window backdrop. This mod keeps its effects confined to classic
popup menus and leaves the main window backdrop to ExplorerBlurMica.

Unknown owner-drawn menus, custom background brushes, high contrast, remote
sessions, disabled transparency, and recognized conflicting renderers are
left unchanged. Settings changes reload the mod. Menu opening animations follow
the Windows settings and the caller's flags, including when effects are active.

Menus whose previous system-backdrop value cannot be read stay native. Unreadable
optional dark-mode and corner preferences are left unchanged. Recorded values
are restored on live-window cleanup; failed restores remain pending for retry.
There are no public getters for prior blur regions or extended-frame margins,
and the Accent getter is unavailable on some builds. Cleanup of those states is
best-effort, not a lossless restoration guarantee for an unknown renderer. A
missing border-color snapshot likewise uses the Windows default on cleanup.
Destroyed popup HWNDs release their window-owned effects without extra resets.

The implementation uses version-dependent Windows interfaces: the composition
attribute exports, UxTheme ordinal 74, the DarkMode::Menu theme, menu layout
message 0x1e2, and Shell window properties. It does not read private Shell item
structures, patch instructions, download symbols, or replace the menu loop.

## Troubleshooting

Enable log output in Windhawk's Advanced tab to record menu preparation,
material and border application results, native fallback reasons, and cleanup.
There is no separate logging setting in this mod. Native mode registers no
menu hooks, so it reports initialization rather than individual menu openings.

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

#include <windhawk_utils.h>

// Keep logging transparent to callers of the hooked Windows APIs. Wh_Log
// supplies Windhawk's enable switch and lazy argument evaluation.
#define ETM_LOG(...)                       \
    do {                                   \
        DWORD savedError = GetLastError(); \
        Wh_Log(__VA_ARGS__);               \
        SetLastError(savedError);          \
    } while (false)

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

PCWSTR MaterialName(Material material) {
    constexpr PCWSTR names[] = {
        L"native",         L"solid",          L"transparent", L"acrylic",
        L"modern-acrylic", L"system-acrylic", L"mica",        L"mica-alt"};
    unsigned index = static_cast<unsigned>(material);
    return index < ARRAYSIZE(names) ? names[index] : L"unknown";
}

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
    bool blurChanged;
    bool frameChanged;
    bool backdropChanged;
    bool cornerChanged;
    bool darkChanged;
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
    AccentPolicy oldAccent;
    bool haveAccent;
    bool accentChanged;
    bool applying;
    bool subclassed;
    bool preparing;
    bool layingOut;
};

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

bool EffectsAllowed(PCWSTR* reason = nullptr) {
    auto reject = [reason](PCWSTR value) {
        if (reason)
            *reason = value;
        return false;
    };
    if (reason)
        *reason = L"allowed";
    if (g_settings.material == Material::Native)
        return reject(L"native material");
    if (g_stopping.load(std::memory_order_relaxed))
        return reject(L"mod stopping");
    if (ConflictingModuleLoaded())
        return reject(L"conflicting renderer");
    if (g_settings.ctrlBypass && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
        return reject(L"Ctrl bypass");
    HIGHCONTRASTW hc{sizeof(hc)};
    BOOL composition = FALSE;
    if (!SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0))
        return reject(L"high-contrast query failed");
    if (hc.dwFlags & HCF_HIGHCONTRASTON)
        return reject(L"high contrast");
    if (GetSystemMetrics(SM_REMOTESESSION))
        return reject(L"remote session");
    if (FAILED(DwmIsCompositionEnabled(&composition)) || !composition)
        return reject(L"composition unavailable");
    if (!IsAppThemed())
        return reject(L"visual styles unavailable");

    DWORD enabled = 1;
    DWORD size = sizeof(enabled);
    // Missing value means Windows default (enabled). Other read errors bypass.
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &enabled, &size);
    if (status == ERROR_FILE_NOT_FOUND)
        return true;
    if (status != ERROR_SUCCESS)
        return reject(L"transparency setting unreadable");
    return enabled ? true : reject(L"transparency disabled");
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
    if (popup.haveDark) {
        HRESULT darkResult = DwmSetWindowAttribute(
            popup.hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
        popup.darkChanged |= SUCCEEDED(darkResult);
        ETM_LOG(L"Theme hwnd=%p dark=%d hr=0x%08lx", popup.hwnd, dark,
                darkResult);
    } else {
        ETM_LOG(
            L"Theme hwnd=%p: previous value unreadable; keeping native state",
            popup.hwnd);
    }
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
        HRESULT frameResult =
            DwmExtendFrameIntoClientArea(popup.hwnd, &margins);
        bool extended = SUCCEEDED(frameResult);
        popup.frameChanged |= extended;
        HRESULT backdropResult = S_FALSE;
        if (extended) {
            backdropResult =
                DwmSetWindowAttribute(popup.hwnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                      &backdrop, sizeof(backdrop));
            success = SUCCEEDED(backdropResult);
            popup.backdropChanged |= success;
        }
        ETM_LOG(
            L"Material hwnd=%p material=%s backdrop=%lu frameHr=0x%08lx "
            L"backdropAttempted=%d backdropHr=0x%08lx applied=%d",
            popup.hwnd, MaterialName(g_settings.material), backdrop,
            frameResult, extended, backdropResult, success);
        // Backdrop/alpha first, Accent disabled last. A menu never activates,
        // so notify DWM of the active appearance without requesting another
        // window-position/frame calculation after native menu layout.
        if (success && g_setComposition) {
            AccentPolicy disabled{};
            CompositionData data{19, &disabled, sizeof(disabled)};
            BOOL applied = g_setComposition(popup.hwnd, &data);
            DWORD error = applied ? ERROR_SUCCESS : GetLastError();
            if (applied)
                popup.accentChanged = true;
            ETM_LOG(L"AccentDisabled hwnd=%p applied=%d lastError=%lu",
                    popup.hwnd, applied, error);
        }
        if (success) {
            DefWindowProcW(popup.hwnd, WM_NCACTIVATE, TRUE, 0);
        }
    } else if (g_setComposition) {
        // Do not stack the fixed, bright system Acrylic recipe with Accent.
        DWORD backdrop = DWMSBT_NONE;
        HRESULT clearResult = DwmSetWindowAttribute(
            popup.hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
        popup.backdropChanged |= SUCCEEDED(clearResult);
        if (FAILED(clearResult)) {
            ETM_LOG(L"BackdropClear hwnd=%p hr=0x%08lx", popup.hwnd,
                    clearResult);
            popup.applying = false;
            return false;
        }
        int opacity = g_settings.material == Material::Solid ? 100
                      : g_settings.material == Material::ModernAcrylic
                          ? 35  // Fixed recipe; do not vary its input alpha.
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
        DWORD error = success ? ERROR_SUCCESS : GetLastError();
        if (success)
            popup.accentChanged = true;
        ETM_LOG(
            L"Material hwnd=%p material=%s state=%d flags=0x%x "
            L"color=0x%08lx applied=%d lastError=%lu",
            popup.hwnd, MaterialName(g_settings.material), state, flags,
            accent.color, success, error);
    } else {
        ETM_LOG(
            L"Material hwnd=%p material=%s unavailable: composition export "
            L"missing",
            popup.hwnd, MaterialName(g_settings.material));
    }
    if (!success) {
        popup.applying = false;
        return false;
    }
    DWORD corner =
        g_settings.roundedCorners ? DWMWCP_ROUNDSMALL : DWMWCP_DONOTROUND;
    if (popup.haveCorner) {
        HRESULT cornerResult =
            DwmSetWindowAttribute(popup.hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                                  &corner, sizeof(corner));
        popup.cornerChanged |= SUCCEEDED(cornerResult);
        ETM_LOG(L"Corner hwnd=%p rounded=%d hr=0x%08lx", popup.hwnd,
                g_settings.roundedCorners, cornerResult);
    } else {
        ETM_LOG(
            L"Corner hwnd=%p: previous value unreadable; keeping native state",
            popup.hwnd);
    }
    if (g_settings.hideMenuBorder) {
        COLORREF border = DWMWA_COLOR_NONE;
        HRESULT borderResult = DwmSetWindowAttribute(
            popup.hwnd, DWMWA_BORDER_COLOR, &border, sizeof(border));
        popup.borderColorChanged = SUCCEEDED(borderResult);
        ETM_LOG(L"Border hwnd=%p action=hide hr=0x%08lx", popup.hwnd,
                borderResult);
    } else {
        ETM_LOG(L"Border hwnd=%p action=keep-native", popup.hwnd);
    }
    popup.applying = false;
    return success;
}

bool HasPopupChanges(const Popup& popup) {
    return popup.brushChanged || popup.blurChanged || popup.frameChanged ||
           popup.accentChanged || popup.backdropChanged ||
           popup.cornerChanged || popup.darkChanged || popup.borderColorChanged;
}

void RestorePopup(Popup& popup, bool destroying = false) {
    if (!popup.active && !popup.palette && !HasPopupChanges(popup))
        return;
    popup.active = false;  // Rendering stays inert throughout rollback.
    if (popup.palette) {
        CloseThemeData(popup.palette);
        popup.palette = nullptr;
    }
    if (popup.brushChanged) {
        if (!IsMenu(popup.menu)) {
            popup.brushChanged = false;
        } else {
            MENUINFO info{sizeof(info)};
            info.fMask = MIM_BACKGROUND;
            if (GetMenuInfo(popup.menu, &info)) {
                if (info.hbrBack != GetStockObject(BLACK_BRUSH)) {
                    popup.brushChanged = false;  // Another writer owns it now.
                } else {
                    info.hbrBack = popup.oldBrush;
                    if (SetMenuInfo(popup.menu, &info))
                        popup.brushChanged = false;
                }
            }
        }
    }
    // Window-owned effects die with the HWND. Do not expose a native/bright
    // last frame by resetting attributes during WM_DESTROY/WM_NCDESTROY.
    if (destroying || !IsWindow(popup.hwnd)) {
        popup.blurChanged = popup.frameChanged = popup.accentChanged = false;
        popup.backdropChanged = popup.cornerChanged = popup.darkChanged = false;
        popup.borderColorChanged = false;
        ETM_LOG(L"Cleanup hwnd=%p menu=%p window-destroyed", popup.hwnd,
                popup.menu);
        return;
    }
    if (popup.accentChanged && g_setComposition) {
        AccentPolicy accent =
            popup.haveAccent ? popup.oldAccent : AccentPolicy{};
        CompositionData data{19, &accent, sizeof(accent)};
        if (g_setComposition(popup.hwnd, &data))
            popup.accentChanged = false;
        if (!popup.haveAccent)
            ETM_LOG(
                L"Cleanup hwnd=%p accent reset is best-effort: previous policy "
                L"unreadable",
                popup.hwnd);
    }
    // No public getters exist for the prior frame margins or blur region.
    // Undo only our successful writes; these two inverses are best-effort,
    // not restoration of an unknown third-party configuration.
    if (popup.frameChanged) {
        MARGINS margins{};
        if (SUCCEEDED(DwmExtendFrameIntoClientArea(popup.hwnd, &margins)))
            popup.frameChanged = false;
        ETM_LOG(L"Cleanup hwnd=%p frame reset is best-effort", popup.hwnd);
    }
    if (popup.blurChanged) {
        DWM_BLURBEHIND blur{};
        blur.dwFlags = DWM_BB_ENABLE;
        if (SUCCEEDED(DwmEnableBlurBehindWindow(popup.hwnd, &blur)))
            popup.blurChanged = false;
        ETM_LOG(L"Cleanup hwnd=%p blur reset is best-effort", popup.hwnd);
    }
    if (popup.backdropChanged) {
        DWORD backdrop = popup.haveBackdrop ? popup.oldBackdrop : DWMSBT_AUTO;
        if (SUCCEEDED(DwmSetWindowAttribute(popup.hwnd,
                                            DWMWA_SYSTEMBACKDROP_TYPE,
                                            &backdrop, sizeof(backdrop))))
            popup.backdropChanged = false;
        if (!popup.haveBackdrop)
            ETM_LOG(
                L"Cleanup hwnd=%p backdrop reset is best-effort: previous "
                L"value unreadable",
                popup.hwnd);
    }
    if (popup.cornerChanged && popup.haveCorner &&
        SUCCEEDED(
            DwmSetWindowAttribute(popup.hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                                  &popup.oldCorner, sizeof(popup.oldCorner))))
        popup.cornerChanged = false;
    if (popup.darkChanged && popup.haveDark &&
        SUCCEEDED(DwmSetWindowAttribute(popup.hwnd,
                                        DWMWA_USE_IMMERSIVE_DARK_MODE,
                                        &popup.oldDark, sizeof(popup.oldDark))))
        popup.darkChanged = false;
    if (popup.borderColorChanged) {
        COLORREF border =
            popup.haveBorderColor ? popup.oldBorderColor : DWMWA_COLOR_DEFAULT;
        HRESULT result = DwmSetWindowAttribute(popup.hwnd, DWMWA_BORDER_COLOR,
                                               &border, sizeof(border));
        if (SUCCEEDED(result))
            popup.borderColorChanged = false;
        ETM_LOG(
            L"Border hwnd=%p action=restore color=0x%08lx hr=0x%08lx saved=%d",
            popup.hwnd, border, result, popup.haveBorderColor);
    }
    ETM_LOG(
        L"Cleanup hwnd=%p menu=%p pending brush=%d blur=%d frame=%d accent=%d "
        L"backdrop=%d corner=%d dark=%d border=%d",
        popup.hwnd, popup.menu, popup.brushChanged, popup.blurChanged,
        popup.frameChanged, popup.accentChanged, popup.backdropChanged,
        popup.cornerChanged, popup.darkChanged, popup.borderColorChanged);
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
            DWORD error = GetLastError();
            ETM_LOG(L"Could not observe this menu thread: %lu", error);
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
            DWORD error = GetLastError();
            ETM_LOG(L"Menu callbacks unavailable owner=%p lastError=%lu", owner,
                    error);
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
        }
        if (!popup->subclassed)
            popup->subclassed =
                SetWindowSubclass(hwnd, MenuPopupSubclass,
                                  reinterpret_cast<UINT_PTR>(this),
                                  reinterpret_cast<DWORD_PTR>(this)) != FALSE;
        if (!popup->subclassed) {
            DWORD error = GetLastError();
            ETM_LOG(L"Menu subclass failed hwnd=%p lastError=%lu", hwnd, error);
        }
        return popup->subclassed ? popup : nullptr;
    }

    void Prepare(HWND hwnd) {
        Popup* reusable = Watch(hwnd);
        if (!reusable || reusable->active || reusable->preparing ||
            reusable->layingOut)
            return;
        if (HasPopupChanges(*reusable) || reusable->palette) {
            RestorePopup(*reusable);
            if (HasPopupChanges(*reusable)) {
                ETM_LOG(
                    L"Preparation deferred hwnd=%p: previous cleanup is still "
                    L"pending",
                    hwnd);
                return;
            }
        }
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
            GetMenuItemCount(bar.hMenu) <= 0) {
            ETM_LOG(L"Preparation pending hwnd=%p: menu handle/items not ready",
                    hwnd);
            return;
        }
        if (!IsSupportedMenu(bar.hMenu, owner, &shellOwnerDraw)) {
            ETM_LOG(
                L"Native fallback hwnd=%p menu=%p: unsupported menu/owner-draw",
                hwnd, bar.hMenu);
            return;
        }
        MENUINFO info{sizeof(info)};
        info.fMask = MIM_BACKGROUND;
        if (!GetMenuInfo(bar.hMenu, &info)) {
            DWORD error = GetLastError();
            ETM_LOG(
                L"Preparation failed hwnd=%p menu=%p stage=GetMenuInfo "
                L"lastError=%lu",
                hwnd, bar.hMenu, error);
            return;
        }
        if (!shellOwnerDraw && info.hbrBack &&
            info.hbrBack != GetSysColorBrush(COLOR_MENU) &&
            info.hbrBack != reinterpret_cast<HBRUSH>(COLOR_MENU + 1)) {
            ETM_LOG(L"Native fallback hwnd=%p menu=%p: custom background brush",
                    hwnd, bar.hMenu);
            return;
        }
        Popup& popup = *reusable;
        bool subclassed = popup.subclassed;
        popup = {};
        popup.subclassed = subclassed;
        popup.preparing = true;
        popup.hwnd = hwnd;
        popup.menu = bar.hMenu;
        popup.oldBrush = info.hbrBack;
        popup.dark = UsesDarkMenus();
        ETM_LOG(
            L"Preparing hwnd=%p menu=%p material=%s theme=%s shellOwnerDraw=%d",
            hwnd, popup.menu, MaterialName(g_settings.material),
            popup.dark ? L"dark" : L"light", shellOwnerDraw);
        popup.haveBackdrop = SUCCEEDED(DwmGetWindowAttribute(
            hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &popup.oldBackdrop,
            sizeof(popup.oldBackdrop)));
        if (!popup.haveBackdrop) {
            ETM_LOG(L"Native fallback hwnd=%p: previous backdrop is unreadable",
                    hwnd);
            return;
        }
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
        popup.palette =
            OpenThemeData(hwnd, popup.dark ? L"DarkMode::Menu" : L"Menu");
        if (!popup.palette) {
            DWORD error = GetLastError();
            ETM_LOG(
                L"Preparation failed hwnd=%p stage=OpenThemeData lastError=%lu",
                hwnd, error);
            RestorePopup(popup);
            return;
        }
        HRGN region = CreateRectRgn(0, 0, -1, -1);
        if (!region) {
            DWORD error = GetLastError();
            ETM_LOG(
                L"Preparation failed hwnd=%p stage=CreateRectRgn lastError=%lu",
                hwnd, error);
            RestorePopup(popup);
            return;
        }
        info.hbrBack = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        if (!SetMenuInfo(popup.menu, &info)) {
            DWORD error = GetLastError();
            ETM_LOG(
                L"Preparation failed hwnd=%p stage=SetMenuInfo lastError=%lu",
                hwnd, error);
            DeleteObject(region);
            RestorePopup(popup);
            return;
        }
        popup.brushChanged = true;
        DWM_BLURBEHIND blur{DWM_BB_ENABLE | DWM_BB_BLURREGION, TRUE, region,
                            FALSE};
        HRESULT result = DwmEnableBlurBehindWindow(hwnd, &blur);
        DeleteObject(region);
        if (SUCCEEDED(result))
            popup.blurChanged = true;
        if (FAILED(result)) {
            ETM_LOG(
                L"Preparation failed hwnd=%p stage=DwmEnableBlurBehindWindow "
                L"hr=0x%08lx",
                hwnd, result);
            RestorePopup(popup);
            return;
        }
        if (!ApplyBackdrop(popup)) {
            ETM_LOG(L"Preparation failed hwnd=%p stage=material", hwnd);
            RestorePopup(popup);
            return;
        }
        if (!IsClassicPopup(hwnd) || !popup.subclassed ||
            g_stopping.load(std::memory_order_relaxed)) {
            RestorePopup(popup);
            return;
        }
        popup.active = true;  // Commit only a complete, live popup.
        ETM_LOG(
            L"Prepared hwnd=%p menu=%p material=%s hideBorder=%d "
            L"borderApplied=%d",
            hwnd, popup.menu, MaterialName(g_settings.material),
            g_settings.hideMenuBorder, popup.borderColorChanged);
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
                scope->Prepare(message.hwnd);
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
    if (!t_scope) {
        [[clang::musttail]] return g_dwmSetWindowAttribute(hwnd, attribute,
                                                           value, size);
    }
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
    HRESULT result =
        g_dwmSetWindowAttribute(hwnd, attribute, popup ? &hidden : value, size);
    if (popup && SUCCEEDED(result))
        popup->borderColorChanged = true;
    // Initial application is logged by ApplyBackdrop. Record subsequent native
    // writes here without tracing paints or querying window geometry.
    if (popup && !popup->applying)
        ETM_LOG(L"Border hwnd=%p action=protect effective=0x%08lx hr=0x%08lx",
                hwnd, hidden, result);
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
            scope->Prepare(hwnd);
        return result;
    }
    if (message == WM_WINDOWPOSCHANGING && lParam &&
        (reinterpret_cast<const WINDOWPOS*>(lParam)->flags & SWP_SHOWWINDOW))
        scope->Prepare(hwnd);
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

// Require a positive DC association, never the last observed popup. A memory
// DC is accepted only as the exact synchronous WM_DRAWITEM target.
Popup* DrawingPopupForDC(HDC dc) {
    MenuScope* scope = t_scope;
    if (!scope || !scope->allowed || g_stopping.load(std::memory_order_relaxed))
        return nullptr;
    // Shell may paint an owner-drawn item using a DC associated with its owner.
    // Accept that DC only when it is exactly the verified WM_DRAWITEM DC.
    HWND hwnd = scope->drawingItem && dc == scope->drawingItem->hDC
                    ? scope->paintingWindow
                    : WindowFromDC(dc);
    if (!hwnd || !IsClassicPopup(hwnd))
        return nullptr;
    Popup* popup = scope->Find(hwnd);
    return popup && popup->active && !popup->preparing && !popup->applying
               ? popup
               : nullptr;
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

struct IconSurface {
    HBITMAP bitmap = nullptr;
    HDC dc = nullptr;
    HGDIOBJ old = nullptr;
    ~IconSurface() {
        if (old && old != HGDI_ERROR)
            SelectObject(dc, old);
        if (dc)
            DeleteDC(dc);
        if (bitmap)
            DeleteObject(bitmap);
    }
};

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
    // NULL blend function commits raw ARGB by copy. Repainting a hot item
    // replaces its alpha instead of accumulating it over the previous frame.
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
    int width = rect->right - rect->left, height = rect->bottom - rect->top;
    if (SUCCEEDED(result) && (!bits || stride < width))
        result = E_FAIL;
    if (SUCCEEDED(result)) {
        UINT dpi = GetDpiForWindow(popup.hwnd);
        if (!dpi)
            dpi = 96;
        float inset = static_cast<float>(MulDiv(2, dpi, 96));
        float radius = static_cast<float>(MulDiv(4, dpi, 96));
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
    if (FAILED(result)) {
        EndBufferedPaint(buffer, FALSE);
        RestoreDC(dc, saved);
        return false;
    }
    // Keep a rollback image before the sole target-writing operation. Even a
    // partially failed commit can then be undone without an exposed clear.
    IconSurface backup;
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB};
    void* backupBits = nullptr;
    backup.bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS,
                                     &backupBits, nullptr, 0);
    backup.dc = CreateCompatibleDC(dc);
    if (backup.bitmap && backup.dc)
        backup.old = SelectObject(backup.dc, backup.bitmap);
    bool captured = backupBits && backup.old && backup.old != HGDI_ERROR &&
                    g_bitBlt(backup.dc, 0, 0, width, height, dc, rect->left,
                             rect->top, SRCCOPY);
    if (!captured) {
        EndBufferedPaint(buffer, FALSE);
        RestoreDC(dc, saved);
        return false;
    }
    HRESULT end = EndBufferedPaint(buffer, TRUE);
    if (FAILED(end)) {
        BOOL restored = g_bitBlt(dc, rect->left, rect->top, width, height,
                                 backup.dc, 0, 0, SRCCOPY);
        ETM_LOG(L"Highlight commit failed hwnd=%p hr=0x%08lx rollback=%d",
                popup.hwnd, end, restored);
        if (!restored)
            RedrawWindow(popup.hwnd, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_ERASE | RDW_NOCHILDREN);
    }
    RestoreDC(dc, saved);
    return SUCCEEDED(end);
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
    if (!t_scope) {
        [[clang::musttail]] return g_drawThemeBackground(theme, dc, part, state,
                                                         rect, clip);
    }
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
    if (!t_scope) {
        [[clang::musttail]] return g_drawThemeBackgroundEx(theme, dc, part,
                                                           state, rect, opts);
    }
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
    if (!t_scope) {
        [[clang::musttail]] return g_drawThemeText(theme, dc, part, state, text,
                                                   length, flags, flags2, rect);
    }
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
    if (!t_scope) {
        [[clang::musttail]] return g_drawThemeTextEx(
            theme, dc, part, state, text, length, flags, rect, options);
    }
    CallGuard call;
    HRESULT result;
    if (DrawCompositedText(theme, dc, part, state, text, length, flags, rect,
                           options, &result))
        return result;
    return g_drawThemeTextEx(theme, dc, part, state, text, length, flags, rect,
                             options);
}

// Work only on an owned copy obtained through public GDI. Never modify a menu
// bitmap, the selected source bitmap, or Shell's private item-data structures.
bool PrepareIconBitmap(HDC source,
                       HBITMAP original,
                       const BITMAP& sourceInfo,
                       POINT origin,
                       int width,
                       int height,
                       IconSurface& target) {
    DIBSECTION originalSection{};
    bool originalDib =
        GetObjectW(original, sizeof(originalSection), &originalSection) ==
            sizeof(originalSection) &&
        originalSection.dsBm.bmBits;
    HBITMAP copy = static_cast<HBITMAP>(CopyImage(
        original, IMAGE_BITMAP, 0, 0, originalDib ? LR_CREATEDIBSECTION : 0));
    if (!copy || copy == original)
        return false;
    DIBSECTION section{};
    int objectSize = GetObjectW(copy, sizeof(section), &section);
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB};
    RGBQUAD* pixels = nullptr;
    target.bitmap =
        CreateDIBSection(nullptr, &info, DIB_RGB_COLORS,
                         reinterpret_cast<void**>(&pixels), nullptr, 0);
    bool read = false;
    GdiFlush();  // Synchronize GDI before CPU reads from the owned copy.
    if (target.bitmap && pixels && objectSize == sizeof(DIBSECTION) &&
        section.dsBm.bmBits) {
        const BITMAP& bitmap = section.dsBm;
        int bytes = bitmap.bmBitsPixel / 8;
        bool rgb = section.dsBmih.biCompression == BI_RGB ||
                   (section.dsBmih.biCompression == BI_BITFIELDS &&
                    bytes == 4 && section.dsBitfields[0] == 0x00ff0000 &&
                    section.dsBitfields[1] == 0x0000ff00 &&
                    section.dsBitfields[2] == 0x000000ff);
        if (rgb && bitmap.bmPlanes == 1 && (bytes == 3 || bytes == 4) &&
            bitmap.bmWidth == sourceInfo.bmWidth &&
            bitmap.bmHeight == sourceInfo.bmHeight &&
            bitmap.bmWidthBytes >= bitmap.bmWidth * bytes &&
            (section.dsBmih.biHeight == bitmap.bmHeight ||
             section.dsBmih.biHeight == -bitmap.bmHeight)) {
            const auto data = static_cast<const BYTE*>(bitmap.bmBits);
            for (int y = 0; y < height; ++y) {
                int row = section.dsBmih.biHeight < 0
                              ? origin.y + y
                              : bitmap.bmHeight - 1 - origin.y - y;
                for (int x = 0; x < width; ++x) {
                    const BYTE* pixel = data + row * bitmap.bmWidthBytes +
                                        (origin.x + x) * bytes;
                    pixels[y * width + x] = {pixel[0], pixel[1], pixel[2],
                                             bytes == 4 ? pixel[3] : BYTE{0}};
                }
            }
            read = true;
        }
    } else if (target.bitmap && pixels && objectSize == sizeof(BITMAP) &&
               !section.dsBm.bmBits) {
        // Only an unselected DDB is passed to GetDIBits. DIB sections above
        // are read directly, using their explicit orientation and stride.
        auto sourcePixels = static_cast<RGBQUAD*>(HeapAlloc(
            GetProcessHeap(), 0,
            sourceInfo.bmWidth * sourceInfo.bmHeight * sizeof(RGBQUAD)));
        info.bmiHeader.biWidth = sourceInfo.bmWidth;
        info.bmiHeader.biHeight = -sourceInfo.bmHeight;
        read = sourcePixels &&
               GetDIBits(source, copy, 0, sourceInfo.bmHeight, sourcePixels,
                         &info, DIB_RGB_COLORS) == sourceInfo.bmHeight;
        if (read)
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x)
                    pixels[y * width + x] =
                        sourcePixels[(origin.y + y) * sourceInfo.bmWidth +
                                     origin.x + x];
        if (sourcePixels)
            HeapFree(GetProcessHeap(), 0, sourcePixels);
    }
    DeleteObject(copy);
    if (!read)
        return false;
    // Neither DIB metadata nor channel values identify straight vs.
    // premultiplied alpha. Only RGB transfers with uniformly absent/opaque
    // alpha can be repaired; preserve all other representations natively.
    if (sourceInfo.bmBitsPixel == 32) {
        bool anyAlpha = false, allOpaque = true;
        for (int i = 0; i < width * height; ++i) {
            anyAlpha |= pixels[i].rgbReserved != 0;
            allOpaque &= pixels[i].rgbReserved == 255;
        }
        if (anyAlpha && !allOpaque)
            return false;
    }
    for (int i = 0; i < width * height; ++i)
        pixels[i].rgbReserved = 255;

    // Remove only a connected corner background, never interior white/black
    // details. Existing semi-transparent assets keep their original alpha.
    if (g_settings.removeIconBackground) {
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
    for (int i = 0; i < width * height; ++i)
        if (!pixels[i].rgbReserved)
            pixels[i] = {};
    target.dc = CreateCompatibleDC(source);
    if (!target.dc)
        return false;
    target.old = SelectObject(target.dc, target.bitmap);
    return target.old && target.old != HGDI_ERROR;
}

bool PrepareIcon(HDC source,
                 int sx,
                 int sy,
                 int width,
                 int height,
                 IconSurface& target) {
    HBITMAP original =
        static_cast<HBITMAP>(GetCurrentObject(source, OBJ_BITMAP));
    BITMAP sourceInfo{};
    if (!original ||
        GetObjectW(original, sizeof(sourceInfo), &sourceInfo) !=
            sizeof(sourceInfo) ||
        sourceInfo.bmBitsPixel <= 1 || sourceInfo.bmWidth < 2 ||
        sourceInfo.bmHeight < 2 || sourceInfo.bmWidth > 256 ||
        sourceInfo.bmHeight > 256 || width < 2 || height < 2 ||
        GetMapMode(source) != MM_TEXT)
        return false;
    // Restrict repair to an unscaled source rectangle. Leave transformed and
    // mirrored transfers to GDI instead of guessing which bitmap pixels apply.
    if (GetGraphicsMode(source) == GM_ADVANCED) {
        XFORM transform{};
        if (!GetWorldTransform(source, &transform) || transform.eM11 != 1 ||
            transform.eM22 != 1 || transform.eM12 != 0 || transform.eM21 != 0 ||
            transform.eDx != 0 || transform.eDy != 0)
            return false;
    }
    POINT origin{sx, sy};
    if ((GetLayout(source) & LAYOUT_RTL) || !LPtoDP(source, &origin, 1) ||
        origin.x < 0 || origin.y < 0 || width > sourceInfo.bmWidth ||
        height > sourceInfo.bmHeight || origin.x > sourceInfo.bmWidth - width ||
        origin.y > sourceInfo.bmHeight - height)
        return false;
    return PrepareIconBitmap(source, original, sourceInfo, origin, width,
                             height, target);
}

bool IsIconTransfer(HDC destination, HDC source, int width, int height) {
    return !t_bitmapDepth && !t_textDepth && g_settings.repairIcons &&
           t_scope && t_scope->drawingItem && destination != source &&
           destination == t_scope->drawingItem->hDC && width >= 4 &&
           height >= 4 && width <= 128 && height <= 128 &&
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
    if (!t_scope) {
        [[clang::musttail]] return g_bitBlt(destination, x, y, width, height,
                                            source, sx, sy, operation);
    }
    CallGuard call;
    if (operation == SRCCOPY &&
        IsIconTransfer(destination, source, width, height)) {
        BitmapGuard guard;
        IconSurface icon;
        if (PrepareIcon(source, sx, sy, width, height, icon)) {
            BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            if (g_alphaBlend(destination, x, y, width, height, icon.dc, 0, 0,
                             width, height, blend)) {
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
    if (!t_scope) {
        [[clang::musttail]] return g_stretchBlt(destination, x, y, width,
                                                height, source, sx, sy, sw, sh,
                                                operation);
    }
    CallGuard call;
    if (operation == SRCCOPY &&
        IsIconTransfer(destination, source, width, height)) {
        BitmapGuard guard;
        IconSurface icon;
        if (PrepareIcon(source, sx, sy, sw, sh, icon)) {
            BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            if (g_alphaBlend(destination, x, y, width, height, icon.dc, 0, 0,
                             sw, sh, blend)) {
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
    if (!t_scope) {
        [[clang::musttail]] return g_alphaBlend(
            destination, x, y, width, height, source, sx, sy, sw, sh, blend);
    }
    CallGuard call;
    if (IsIconTransfer(destination, source, width, height) &&
        blend.BlendOp == AC_SRC_OVER && !blend.BlendFlags &&
        !blend.AlphaFormat) {
        BitmapGuard guard;
        IconSurface icon;
        if (PrepareIcon(source, sx, sy, sw, sh, icon)) {
            BLENDFUNCTION repairedBlend = blend;
            repairedBlend.AlphaFormat = AC_SRC_ALPHA;
            if (g_alphaBlend(destination, x, y, width, height, icon.dc, 0, 0,
                             sw, sh, repairedBlend)) {
                return TRUE;
            }
        }
    }
    return g_alphaBlend(destination, x, y, width, height, source, sx, sy, sw,
                        sh, blend);
}

int WINAPI FillRectHook(HDC dc, const RECT* rect, HBRUSH brush) {
    if (!t_scope) {
        [[clang::musttail]] return g_fillRect(dc, rect, brush);
    }
    CallGuard call;
    // An unrelated double-buffered control can repaint during the menu loop.
    // Only accept the DC from the verified, synchronous WM_DRAWITEM context.
    Popup* popup = !t_textDepth && rect && t_scope && t_scope->drawingItem &&
                           t_scope->drawingItem->hDC == dc
                       ? DrawingPopupForDC(dc)
                       : nullptr;
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
    if (!t_scope) {
        [[clang::musttail]] return g_drawText(dc, text, length, rect, flags);
    }
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
    return height;
}

template <typename Invoke>
BOOL TrackMenu(HWND owner, UINT flags, Invoke invoke) {
    CallGuard call;
    DWORD incomingError = GetLastError();
    PCWSTR reason = L"nested bypass";
    bool requestEffects = !t_bypassDepth && EffectsAllowed(&reason);
    bool buffered = false;
    BOOL result;
    DWORD resultError;
    {
        MenuScope scope(owner, requestEffects);
        // BufferedPaint must be initialized on each rendering thread. Pending
        // callbacks remain inert so initialization cannot modify a popup.
        if (scope.Ready()) {
            HRESULT paintResult = BufferedPaintInit();
            buffered = SUCCEEDED(paintResult);
            if (!buffered) {
                reason = L"buffered painting unavailable";
                ETM_LOG(L"BufferedPaintInit owner=%p hr=0x%08lx", owner,
                        paintResult);
            }
        } else if (requestEffects) {
            reason = L"menu callbacks unavailable";
        }
        if (buffered && EffectsAllowed(&reason))
            scope.allowed = true;
        ETM_LOG(
            L"Tracking owner=%p material=%s effects=%d reason=%s flags=0x%x",
            owner, MaterialName(g_settings.material), scope.allowed, reason,
            flags);
        if (scope.allowed) {
            SetLastError(incomingError);
            result = invoke(flags);
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
    ETM_LOG(L"Tracking ended owner=%p result=%d lastError=%lu", owner, result,
            resultError);
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
    // DWMWA_SYSTEMBACKDROP_TYPE requires Windows 11 22H2. RtlGetVersion avoids
    // manifest-dependent version virtualization; no symbol lookup is involved.
    using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
    auto versionFn = reinterpret_cast<RtlGetVersion_t>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion"));
    OSVERSIONINFOW version{sizeof(version)};
    if (!versionFn || versionFn(&version) < 0 ||
        version.dwBuildNumber < 22621) {
        ETM_LOG(L"Requires Windows 11 22H2, build 22621 or newer");
        return FALSE;
    }
    g_settings.material =
        ParseMaterial(WindhawkUtils::StringSetting::make(L"backgroundEffect"));
    if (g_settings.material == Material::Native) {
        ETM_LOG(L"Native mode: no hooks registered, no window effects changed");
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
        ETM_LOG(L"Menu theme identification unavailable");
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
        ETM_LOG(L"Adjustable Acrylic is unavailable; choose System Acrylic");
        return FALSE;
    }
    g_settings.lightTint = ParseTint(
        WindhawkUtils::StringSetting::make(L"lightTint"), RGB(245, 245, 245));
    g_settings.darkTint = ParseTint(
        WindhawkUtils::StringSetting::make(L"darkTint"), RGB(32, 32, 32));
    auto themeSetting = WindhawkUtils::StringSetting::make(L"theme");
    g_settings.theme = !_wcsicmp(themeSetting, L"dark")    ? 2
                       : !_wcsicmp(themeSetting, L"light") ? 1
                                                           : 0;
    g_settings.highlightOpacity =
        ClampPercent(Wh_GetIntSetting(L"highlightOpacity"));
    g_settings.repairIcons = Wh_GetIntSetting(L"repairIcons") != 0;
    g_settings.removeIconBackground =
        Wh_GetIntSetting(L"removeIconBackground") != 0;
    g_settings.roundedCorners = Wh_GetIntSetting(L"roundedCorners") != 0;
    g_settings.hideMenuBorder = Wh_GetIntSetting(L"hideMenuBorder") != 0;
    g_settings.ctrlBypass = Wh_GetIntSetting(L"ctrlBypass") != 0;
    g_stopping.store(false, std::memory_order_relaxed);

    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenuHook,
                                        &g_trackPopupMenu) ||
        !WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuExHook,
                                        &g_trackPopupMenuEx) ||
        !WindhawkUtils::SetFunctionHook(DrawThemeBackground,
                                        DrawThemeBackgroundHook,
                                        &g_drawThemeBackground) ||
        !WindhawkUtils::SetFunctionHook(DrawThemeBackgroundEx,
                                        DrawThemeBackgroundExHook,
                                        &g_drawThemeBackgroundEx) ||
        !WindhawkUtils::SetFunctionHook(DrawThemeText, DrawThemeTextHook,
                                        &g_drawThemeText) ||
        !WindhawkUtils::SetFunctionHook(DrawThemeTextEx, DrawThemeTextExHook,
                                        &g_drawThemeTextEx) ||
        !WindhawkUtils::SetFunctionHook(DrawTextW, DrawTextHook, &g_drawText) ||
        !WindhawkUtils::SetFunctionHook(FillRect, FillRectHook, &g_fillRect) ||
        !WindhawkUtils::SetFunctionHook(BitBlt, BitBltHook, &g_bitBlt) ||
        !WindhawkUtils::SetFunctionHook(StretchBlt, StretchBltHook,
                                        &g_stretchBlt) ||
        !WindhawkUtils::SetFunctionHook(AlphaBlend, AlphaBlendHook,
                                        &g_alphaBlend))
        return FALSE;
    if (g_settings.hideMenuBorder &&
        !WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute,
                                        DwmSetWindowAttributeHook,
                                        &g_dwmSetWindowAttribute))
        return FALSE;
    ETM_LOG(L"Initialized on Windows build %lu; only Explorer Win32 popups",
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

#undef ETM_LOG
