// ==WindhawkMod==
// @id              mmc-dark-mode
// @name            MMC Dark Mode
// @description     Dark theme for the console snap-ins: Device Manager, Event Viewer, Services, Task Scheduler and Disk Management.
// @version         1.0
// @author          metanoid18
// @github          https://github.com/Metanoid18
// @license         MIT
// @include         mmc.exe
// @compilerOptions -lgdi32 -lcomctl32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MMC Dark Mode

A dark theme for the Microsoft Management Console: Device Manager, Services,
Event Viewer, Task Scheduler, Disk Management and anything else hosted by
`mmc.exe`.

## Screenshots

Device Manager with the mod off:

![Device Manager with the mod off](https://raw.githubusercontent.com/Metanoid18/windhawk-mods/mmc-dark-mode-shots/screenshots/mmc-dark-mode-before.png)

Device Manager with the mod on:

![Device Manager with the mod on](https://raw.githubusercontent.com/Metanoid18/windhawk-mods/mmc-dark-mode-shots/screenshots/mmc-dark-mode-after.png)

## What it does

The mod themes MMC windows in the `mmc.exe` process, including common trees,
list views, property sheets and chrome. It also contains targeted handling for
Services, Event Viewer, Task Scheduler and Disk Management. Existing menus and
toolbars remain available. Some embedded content draws itself and is not themed.

Switching the mod off requests restoration of native styles, colours and
painting. The system-wide colour table is not changed.

## Known limits

* MMC hosts many snap-ins and not every control or embedded page is covered.
  Layouts, visual styles and Windows builds may differ from those tested.
* The Actions pane caption adjustment is x64-only and uses private DirectUI
  interfaces. If the required interfaces or hooks are unavailable, the native
  appearance is retained. Other custom-drawn content may remain light.
* Disk Management's selected partition frame adjustment is x64-only and checks
  an exact native drawing-code signature. On an unknown or changed build it
  leaves that frame unchanged rather than guessing. The blue partition colour
  is adjusted only when the native renderer requests the observed stock navy.
  Any region or legend configured to that same navy will also be recoloured;
  other brush colours and native patterns are left unchanged.
* The mod checks high contrast at load and does not activate when it is already
  enabled. Switching high contrast while MMC is running is not guaranteed to
  update every themed control; restart the console or disable the mod.
* Some GDI brushes are deliberately retained until the MMC process exits because
  native controls may cache them. Repeatedly reloading the mod in the same
  process can increase GDI handle use.
* The property-sheet tab strip is custom-painted; other tab layouts keep their
  native drawing. Push buttons generally keep native visuals, with a scoped
  dark face for the Task Scheduler summary Refresh button.
* The maximized child window's menu-row buttons use scoped custom drawing.
  Other layouts may keep the native appearance. Restored child-window captions
  and their buttons can remain light.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- darkTitleBar: true
  $name: Dark title bar
  $description: Darkens the title bar of the console window. Windows 10 2004 and later
- roundCorners: true
  $name: Rounded window corners
  $description: Rounded corners on the console window. Windows 11 only
- darkStatusBar: true
  $name: Dark status bar
  $description: Repaints the status bar at the bottom of the console in the dark colours
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <windhawk_api.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <windhawk_utils.h>

// Resolve the undocumented dark-mode entry points dynamically; fail if required exports are missing.

namespace {

enum PreferredAppMode {
    PreferredAppModeDefault = 0,
    PreferredAppModeAllowDark = 1,
    PreferredAppModeForceDark = 2,
    PreferredAppModeForceLight = 3,
};

constexpr WORD kOrdinalShouldAppsUseDarkMode = 132;
constexpr WORD kOrdinalAllowDarkModeForWindow = 133;
constexpr WORD kOrdinalSetPreferredAppMode = 135;
constexpr WORD kOrdinalFlushMenuThemes = 136;
constexpr WORD kOrdinalIsDarkModeAllowedForWindow = 137;

// Try the supported immersive dark title-bar attributes in order.
constexpr DWORD kDwmwaUseImmersiveDarkMode = 20;
constexpr DWORD kDwmwaUseImmersiveDarkModeLegacy = 19;

constexpr COLORREF kColorBackground = RGB(32, 32, 32);
constexpr COLORREF kColorText = RGB(240, 240, 240);
constexpr COLORREF kColorLine = RGB(70, 70, 70);
constexpr COLORREF kColorHover = RGB(58, 60, 66);
constexpr COLORREF kColorTabText = RGB(178, 178, 178);
constexpr COLORREF kColorTabSelected = RGB(45, 45, 45);
constexpr COLORREF kColorSelection = RGB(38, 79, 120);

using ShouldAppsUseDarkMode_t = BOOL(WINAPI*)();
using AllowDarkModeForWindow_t = BOOL(WINAPI*)(HWND, BOOL);
using SetPreferredAppMode_t = PreferredAppMode(WINAPI*)(PreferredAppMode);
using FlushMenuThemes_t = void(WINAPI*)();
using IsDarkModeAllowedForWindow_t = BOOL(WINAPI*)(HWND);
using SetWindowTheme_t = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);

// Use the original GetSysColor when restoring native colours.
using GetSysColor_t = DWORD(WINAPI*)(int);
GetSysColor_t pGetSysColor = nullptr;

ShouldAppsUseDarkMode_t pShouldAppsUseDarkMode = nullptr;
AllowDarkModeForWindow_t pAllowDarkModeForWindow = nullptr;
SetPreferredAppMode_t pSetPreferredAppMode = nullptr;
FlushMenuThemes_t pFlushMenuThemes = nullptr;
IsDarkModeAllowedForWindow_t pIsDarkModeAllowedForWindow = nullptr;
SetWindowTheme_t pSetWindowTheme = nullptr;

// GetWindowTheme identifies a window's current visual style.
using GetWindowTheme_t = void*(WINAPI*)(HWND);
GetWindowTheme_t pGetWindowTheme = nullptr;

// Override the affected list-box selection fill while dark mode is active.
using GetThemeColor_t = HRESULT(WINAPI*)(HTHEME, int, int, int, COLORREF*);
GetThemeColor_t pGetThemeColor = nullptr;
GetThemeColor_t pGetThemeColorHook = nullptr;

// The active flag is read on GUI threads and cleared during unload.
std::atomic<bool> g_active{false};

HRESULT WINAPI GetThemeColor_Hook(HTHEME theme, int part, int state, int prop,
                                  COLORREF* colour) {
    if (colour && g_active && prop == TMT_FILLCOLOR && part == LBCP_ITEM &&
        state >= LBPSI_HOT && state <= LBPSI_SELECTEDNOTFOCUS) {
        *colour = kColorSelection;
        return S_OK;
    }
    if (!pGetThemeColorHook) {
        return E_FAIL;
    }
    return pGetThemeColorHook(theme, part, state, prop, colour);
}

SetWindowTheme_t pSetWindowThemeHook = nullptr;

void SetWindowThemeQuiet(HWND hWnd, PCWSTR subApp, PCWSTR subId) {
    if (pSetWindowTheme) {
        pSetWindowTheme(hWnd, subApp, subId);
    }
}

// Prevent late theme changes from reintroducing unsuitable control styles.
HRESULT WINAPI SetWindowTheme_Hook(HWND hWnd, LPCWSTR subApp, LPCWSTR subId) {
    if (pSetWindowThemeHook && hWnd) {
        WCHAR className[64] = {};
        if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
            return pSetWindowThemeHook(hWnd, subApp, subId);
        }

        if (!g_active) {
            return pSetWindowThemeHook(hWnd, subApp, subId);
        }

        // Keep toolbars, labels, links and tab strips on their classic drawing paths.
        if (wcscmp(className, L"ToolbarWindow32") == 0 ||
            wcscmp(className, L"Static") == 0 ||
            wcscmp(className, L"SysLink") == 0 ||
            wcscmp(className, L"SysTabControl32") == 0) {
            return pSetWindowThemeHook(hWnd, L"", L"");
        }
        return pSetWindowThemeHook(hWnd, subApp, subId);
    }
    return S_OK;
}
DWORD g_darkTitleBarAttribute = 0;

struct Settings {
    bool darkTitleBar;
    bool roundCorners;
    bool darkStatusBar;
};

Settings g_settings;

LRESULT SendToControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Use the comctl32 subclass chain; track windows for safe unload.
enum class WindowKind : DWORD_PTR {
    Backdrop = 1,
    Strip,
    StatusBar,
    Toolbar,
    NoBorder,
    Tab,
    Field,
    GroupBox,
    Dialog,
    MmcCustomTab,
    ActionsPane,
    TreeFrame,
    DescriptionHost,
    ActionsContent,
    WinFormsLabel,
    DiskHeader,
    WinFormsCombo,
    WinFormsComboParent,
    WinFormsRefresh,
};

bool SubclassWith(HWND hWnd, WindowKind kind);
void RemoveAllSubclasses();
void WalkEveryThread(bool waitForIt);
void WalkThreadWindows(DWORD threadId);
void ResetWindow(HWND hWnd);
void ForceRepaint(HWND hWnd);

std::mutex g_stateLock;
std::unordered_set<HWND> g_subclassed;
std::unordered_set<HWND> g_adding;

HBRUSH g_backgroundBrush = nullptr;
HBRUSH g_hoverBrush = nullptr;
HBRUSH g_lineBrush = nullptr;
HBRUSH g_tabSelectedBrush = nullptr;

// Caches are thread-local and populated on each window's owning thread.

// A registered message avoids collisions with MMC's private WM_APP messages.
UINT OwnThreadSetupMessage() {
    static const UINT message =
        RegisterWindowMessageW(L"MMC Dark Mode: fill a cache on the owning thread");
    return message;
}

bool OnOwningThread(HWND hWnd) {
    DWORD pid = 0;
    return GetWindowThreadProcessId(hWnd, &pid) == GetCurrentThreadId();
}

void AskOwningThread(HWND hWnd) {
    PostMessageW(hWnd, OwnThreadSetupMessage(), 0, 0);
}

// Cache status-bar content outside paint to avoid reentrant control queries.
struct StatusCache {
    HWND hWnd;
    int parts;
    RECT rects[16];
    WCHAR text[16][256];
    bool valid;
};
thread_local StatusCache g_statusCache[2];

void RefreshStatusCache(HWND hWnd);
void ForgetSubclass(HWND hWnd);

struct HotEntry {
    HWND toolbar;
    RECT rect;
    bool valid;
};
thread_local HotEntry g_hot[8];

HotEntry* HotFor(HWND toolbar, bool create) {
    for (int i = 0; i < 8; i++) {
        if (g_hot[i].toolbar == toolbar) {
            return &g_hot[i];
        }
    }
    if (!create) {
        return nullptr;
    }
    for (int i = 0; i < 8; i++) {
        if (!g_hot[i].toolbar) {
            g_hot[i].toolbar = toolbar;
            g_hot[i].valid = false;
            return &g_hot[i];
        }
    }
    return nullptr;
}

bool UpdateHot(HWND toolbar) {
    if (!OnOwningThread(toolbar)) {
        return false;
    }
    HotEntry* entry = HotFor(toolbar, true);
    if (!entry) {
        return false;
    }

    bool wasValid = entry->valid;
    RECT wasRect = entry->rect;

    int hot = static_cast<int>(SendToControl(toolbar, TB_GETHOTITEM, 0, 0));
    if (hot < 0) {
        entry->valid = false;
    } else {
        RECT rect = {};
        if (SendToControl(toolbar, TB_GETITEMRECT, hot,
                          reinterpret_cast<LPARAM>(&rect))) {
            entry->rect = rect;
            entry->valid = true;
        } else {
            entry->valid = false;
        }
    }

    if (wasValid != entry->valid) {
        return true;
    }
    if (!wasValid) {
        return false;
    }
    return wasRect.left != entry->rect.left || wasRect.top != entry->rect.top ||
           wasRect.right != entry->rect.right ||
           wasRect.bottom != entry->rect.bottom;
}

LRESULT HandleToolbar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    LRESULT result =
        DefSubclassProc(hWnd, message, wParam, lParam);

    // Invalidate the previous toolbar highlight when the hot item changes.
    if (message == WM_MOUSEMOVE || message == WM_NCMOUSEMOVE ||
        message == WM_MOUSELEAVE) {
        if (UpdateHot(hWnd)) {
            InvalidateRect(hWnd, nullptr, FALSE);
        }
    }
    return result;
}

BOOL CALLBACK ReapplyChild(HWND hWnd, LPARAM lParam);
void RefreshFrame(HWND hWnd);
LRESULT HandleTab(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool RefreshTabEntry(HWND hWnd);
HBRUSH LineBrush();
HBRUSH TabSelectedBrush();
void DrawTextOn(HDC dc, PCWSTR text, RECT* rect, UINT flags, COLORREF colour,
                COLORREF behind);
LRESULT HandleGroupBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT HandleField(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void PaintGroupBox(HWND hWnd);
void PaintClientEdge(HWND hWnd);
void CoverPageFrame(HWND sheet);
void RememberGroupFont(HWND hWnd);
HFONT GroupFontOf(HWND hWnd);
void ForgetGroupFont(HWND hWnd);
bool NameContains(PCWSTR name, PCWSTR needle);

LRESULT HandleStatusBar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (!g_settings.darkStatusBar) {
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }

    if (message == WM_NCPAINT) {
        return 0;
    }

    if (message == WM_ERASEBKGND) {
        return 1;
    }

    if (message == WM_PAINT) {
        PAINTSTRUCT ps = {};
        HDC dc = BeginPaint(hWnd, &ps);
        if (dc && g_backgroundBrush) {
            RECT client = {};
            GetClientRect(hWnd, &client);
            FillRect(dc, &client, g_backgroundBrush);

            for (int i = 0; i < 2; i++) {
                StatusCache& cache = g_statusCache[i];
                if (cache.hWnd != hWnd || !cache.valid) {
                    continue;
                }
                for (int part = 0; part < cache.parts && part < 16; part++) {
                    if (!cache.text[part][0]) {
                        continue;
                    }
                    RECT partRect = cache.rects[part];
                    partRect.left += 4;
                    DrawTextOn(dc, cache.text[part], &partRect,
                               DT_LEFT | DT_VCENTER | DT_SINGLELINE |
                                   DT_END_ELLIPSIS,
                               kColorText, kColorBackground);
                }
            }
        }
        EndPaint(hWnd, &ps);
        return 0;
    }

    LRESULT result =
        DefSubclassProc(hWnd, message, wParam, lParam);

    if (message == SB_SETTEXTW || message == SB_SETPARTS || message == WM_SIZE) {
        RefreshStatusCache(hWnd);
    }

    return result;
}

bool ResolveApis() {
    HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
    if (!uxtheme) {
        uxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr,
                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!uxtheme) {
        Wh_Log(L"uxtheme.dll is not loaded and cannot be loaded");
        return false;
    }

    pShouldAppsUseDarkMode = reinterpret_cast<ShouldAppsUseDarkMode_t>(
        GetProcAddress(uxtheme, (const char*)(ULONG_PTR)kOrdinalShouldAppsUseDarkMode));
    pAllowDarkModeForWindow = reinterpret_cast<AllowDarkModeForWindow_t>(
        GetProcAddress(uxtheme, (const char*)(ULONG_PTR)kOrdinalAllowDarkModeForWindow));
    pSetPreferredAppMode = reinterpret_cast<SetPreferredAppMode_t>(
        GetProcAddress(uxtheme, (const char*)(ULONG_PTR)kOrdinalSetPreferredAppMode));
    pFlushMenuThemes = reinterpret_cast<FlushMenuThemes_t>(
        GetProcAddress(uxtheme, (const char*)(ULONG_PTR)kOrdinalFlushMenuThemes));
    pIsDarkModeAllowedForWindow = reinterpret_cast<IsDarkModeAllowedForWindow_t>(
        GetProcAddress(uxtheme, (const char*)(ULONG_PTR)kOrdinalIsDarkModeAllowedForWindow));
    pSetWindowTheme =
        reinterpret_cast<SetWindowTheme_t>(GetProcAddress(uxtheme, "SetWindowTheme"));
    pGetWindowTheme =
        reinterpret_cast<GetWindowTheme_t>(GetProcAddress(uxtheme, "GetWindowTheme"));
    pGetThemeColor =
        reinterpret_cast<GetThemeColor_t>(GetProcAddress(uxtheme, "GetThemeColor"));

    if (!pAllowDarkModeForWindow || !pSetPreferredAppMode || !pSetWindowTheme) {
        Wh_Log(L"uxtheme is missing dark mode entry points "
               L"(allow=%d mode=%d theme=%d)",
               pAllowDarkModeForWindow != nullptr,
               pSetPreferredAppMode != nullptr,
               pSetWindowTheme != nullptr);
        return false;
    }

    Wh_Log(L"resolved: shouldApps=%d isAllowed=%d flush=%d",
           pShouldAppsUseDarkMode != nullptr,
           pIsDarkModeAllowedForWindow != nullptr,
           pFlushMenuThemes != nullptr);
    return true;
}

bool HighContrastIsOn() {
    HIGHCONTRASTW hc = {};
    hc.cbSize = sizeof(hc);
    if (!SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0)) {
        return false;
    }
    return (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

void ThemeDialog(HWND hWnd);
LRESULT HandleBackdrop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void SetDarkTitleBar(HWND hWnd, bool dark) {
    BOOL value = dark ? TRUE : FALSE;
    if (g_darkTitleBarAttribute) {
        DwmSetWindowAttribute(hWnd, g_darkTitleBarAttribute, &value,
                              sizeof(value));
        return;
    }
    if (SUCCEEDED(DwmSetWindowAttribute(hWnd, kDwmwaUseImmersiveDarkMode, &value,
                                        sizeof(value)))) {
        g_darkTitleBarAttribute = kDwmwaUseImmersiveDarkMode;
        return;
    }
    if (SUCCEEDED(DwmSetWindowAttribute(hWnd, kDwmwaUseImmersiveDarkModeLegacy,
                                        &value, sizeof(value)))) {
        g_darkTitleBarAttribute = kDwmwaUseImmersiveDarkModeLegacy;
    }
}

void ApplyToTopLevel(HWND hWnd) {
    if (g_settings.darkTitleBar) {
        SetDarkTitleBar(hWnd, true);
    }

    if (g_settings.roundCorners) {
        int preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference,
                              sizeof(preference));
    }

    COLORREF border = kColorBackground;
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &border, sizeof(border));

    if (!g_backgroundBrush) {
        g_backgroundBrush = CreateSolidBrush(kColorBackground);
    }
    SubclassWith(hWnd, WindowKind::Backdrop);
}

LRESULT SendToControl(HWND hWnd, UINT message, WPARAM wParam,
                        LPARAM lParam) {
    DWORD_PTR result = 0;
    SendMessageTimeoutW(hWnd, message, wParam, lParam,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &result);
    return static_cast<LRESULT>(result);
}

void RefreshStatusCache(HWND hWnd) {
    if (!OnOwningThread(hWnd)) {
        AskOwningThread(hWnd);
        return;
    }
    StatusCache* cache = nullptr;
    for (int i = 0; i < 2; i++) {
        if (g_statusCache[i].hWnd == hWnd) {
            cache = &g_statusCache[i];
            break;
        }
    }
    if (!cache) {
        for (int i = 0; i < 2; i++) {
            if (!g_statusCache[i].hWnd) {
                cache = &g_statusCache[i];
                break;
            }
        }
    }
    if (!cache) {
        for (int i = 0; i < 2; i++) {
            if (!IsWindow(g_statusCache[i].hWnd)) {
                cache = &g_statusCache[i];
                break;
            }
        }
    }
    if (!cache) {
        return;
    }

    cache->hWnd = hWnd;
    cache->valid = false;

    int parts = static_cast<int>(SendToControl(hWnd, SB_GETPARTS, 0, 0));
    if (parts <= 0) {
        parts = 1;
    }
    if (parts > 16) {
        parts = 16;
    }
    cache->parts = parts;

    for (int part = 0; part < parts; part++) {
        cache->text[part][0] = 0;
        RECT rect = {};
        if (SendToControl(hWnd, SB_GETRECT, part,
                          reinterpret_cast<LPARAM>(&rect))) {
            cache->rects[part] = rect;
        }
        DWORD_PTR info = SendToControl(hWnd, SB_GETTEXTLENGTHW, part, 0);
        int length = static_cast<int>(info & 0xffff);
        if (length <= 0 || length >= 255) {
            continue;
        }
        WCHAR buffer[256] = {};
        if (SendToControl(hWnd, SB_GETTEXTW, part,
                          reinterpret_cast<LPARAM>(buffer))) {
            for (int c = 0; c < 255 && c < length; c++) {
                cache->text[part][c] = buffer[c];
            }
            cache->text[part][length] = 0;
        }
    }
    cache->valid = true;
}

// Use native colours during restoration, bypassing the dark-colour hook.
COLORREF RealSystemColour(int index) {
    if (pGetSysColor) {
        return pGetSysColor(index);
    }
    return GetSysColor(index);
}

// Scope WinForms list colour handling to the observed MMC host hierarchy.
bool IsHostedWinFormsList(HWND hWnd) {
    WCHAR cls[128] = {};
    if (!hWnd || !GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) ||
        wcsncmp(cls, L"WindowsForms10.SysListView32.", 29) != 0 ||
        (GetWindowLongPtrW(hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT) return false;
    HWND parent = GetParent(hWnd);
    int forms = 0;
    while (parent && forms < 8 && GetClassNameW(parent, cls, ARRAYSIZE(cls)) &&
           wcsncmp(cls, L"WindowsForms10.Window.", 22) == 0) {
        ++forms;
        parent = GetParent(parent);
    }
    if (!forms || !parent || !GetClassNameW(parent, cls, ARRAYSIZE(cls)) ||
        wcsncmp(cls, L"ATL:", 4) != 0) return false;
    parent = GetParent(parent);
    if (!parent || !GetClassNameW(parent, cls, ARRAYSIZE(cls)) ||
        wcscmp(cls, L"AtlAxWinEx") != 0) return false;
    parent = GetParent(parent);
    if (!parent || !GetClassNameW(parent, cls, ARRAYSIZE(cls)) ||
        wcscmp(cls, L"MMCOCXViewWindow") != 0) return false;
    parent = GetParent(parent);
    return parent && GetClassNameW(parent, cls, ARRAYSIZE(cls)) &&
           wcscmp(cls, L"MMCViewWindow") == 0;
}

// Only restore the observed dark palette with CLR_NONE; identical custom palettes are indistinguishable.
bool ReadHostedListColour(HWND hWnd, UINT message, COLORREF* value) {
    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(hWnd, message, 0, 0,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &result)) {
        return false;
    }
    *value = static_cast<COLORREF>(result);
    return true;
}

bool WriteHostedListColour(HWND hWnd, UINT message, COLORREF value) {
    DWORD_PTR result = 0;
    return SendMessageTimeoutW(hWnd, message, 0, static_cast<LPARAM>(value),
                               SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &result) &&
           result != 0;
}

void ChangeHostedWinFormsList(HWND hWnd, COLORREF expectedBackground,
                              COLORREF expectedText, COLORREF background,
                              COLORREF text) {
    COLORREF oldBackground = 0;
    COLORREF oldText = 0;
    COLORREF oldTextBackground = 0;
    if (!ReadHostedListColour(hWnd, LVM_GETBKCOLOR, &oldBackground) ||
        !ReadHostedListColour(hWnd, LVM_GETTEXTCOLOR, &oldText) ||
        !ReadHostedListColour(hWnd, LVM_GETTEXTBKCOLOR, &oldTextBackground) ||
        oldBackground != expectedBackground || oldText != expectedText ||
        oldTextBackground != CLR_NONE) {
        return;
    }
    // Keep transparent text; roll back the text colour if the background update fails.
    if (!WriteHostedListColour(hWnd, LVM_SETTEXTCOLOR, text)) {
        return;
    }
    if (!WriteHostedListColour(hWnd, LVM_SETBKCOLOR, background)) {
        WriteHostedListColour(hWnd, LVM_SETTEXTCOLOR, oldText);
        return;
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

void ColourClassicControl(HWND hWnd, bool dark) {
    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return;
    }

    const COLORREF background =
        dark ? kColorBackground : RealSystemColour(COLOR_WINDOW);
    const COLORREF text = dark ? kColorText : RealSystemColour(COLOR_WINDOWTEXT);

    if (IsHostedWinFormsList(hWnd)) {
        const COLORREF expectedBackground = dark
            ? RealSystemColour(COLOR_WINDOW) : kColorBackground;
        const COLORREF expectedText = dark
            ? RealSystemColour(COLOR_WINDOWTEXT) : kColorText;
        ChangeHostedWinFormsList(hWnd, expectedBackground, expectedText,
                                 background, text);
    } else if (wcscmp(className, L"SysTreeView32") == 0) {
        SendToControl(hWnd, TVM_SETBKCOLOR, 0, background);
        SendToControl(hWnd, TVM_SETTEXTCOLOR, 0, text);
        SendToControl(hWnd, TVM_SETLINECOLOR, 0,
                      dark ? kColorLine : CLR_DEFAULT);
        InvalidateRect(hWnd, nullptr, TRUE);
    } else if (wcscmp(className, L"ComboBox") == 0) {
        if (dark) {
            pSetWindowTheme(hWnd, L"DarkMode_CFD", nullptr);
        } else {
            // NULL removes the theme association; empty strings disable visual styles instead.
            pSetWindowTheme(hWnd, nullptr, nullptr);
        }
        InvalidateRect(hWnd, nullptr, TRUE);
    } else if (wcscmp(className, L"ListBox") == 0) {
        pSetWindowTheme(hWnd, dark ? L"" : nullptr, dark ? L"" : nullptr);
        InvalidateRect(hWnd, nullptr, TRUE);
    } else if (wcscmp(className, L"ScrollBar") == 0) {
        if (dark) {
            pAllowDarkModeForWindow(hWnd, TRUE);
            pSetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);
        } else {
            pSetWindowTheme(hWnd, nullptr, nullptr);
            pAllowDarkModeForWindow(hWnd, FALSE);
        }
    } else if (wcscmp(className, L"SysTabControl32") == 0) {
        if (dark) {
            pSetWindowTheme(hWnd, L"", L"");
            SubclassWith(hWnd, WindowKind::Tab);
            RefreshTabEntry(hWnd);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    } else if (wcscmp(className, L"SysListView32") == 0) {
        SendToControl(hWnd, LVM_SETBKCOLOR, 0, background);
        SendToControl(hWnd, LVM_SETTEXTCOLOR, 0, text);
        SendToControl(hWnd, LVM_SETTEXTBKCOLOR, 0, background);
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

// Record removed frame-style bits for restoration.
struct StyleFix {
    HWND hWnd;
    LONG_PTR styleBits;
    LONG_PTR exStyleBits;
};
constexpr int kMaxStyleFixes = 64;
StyleFix g_styleFixes[kMaxStyleFixes] = {};

void NoteRemovedStyles(HWND hWnd, LONG_PTR styleBits, LONG_PTR exStyleBits) {
    std::lock_guard<std::mutex> lock(g_stateLock);
    for (int i = 0; i < kMaxStyleFixes; i++) {
        if (g_styleFixes[i].hWnd == hWnd) {
            g_styleFixes[i].styleBits |= styleBits;
            g_styleFixes[i].exStyleBits |= exStyleBits;
            return;
        }
    }
    for (int i = 0; i < kMaxStyleFixes; i++) {
        if (!g_styleFixes[i].hWnd) {
            g_styleFixes[i].hWnd = hWnd;
            g_styleFixes[i].styleBits = styleBits;
            g_styleFixes[i].exStyleBits = exStyleBits;
            return;
        }
    }
}

void RestoreRemovedStyles(HWND hWnd) {
    LONG_PTR styleBits = 0;
    LONG_PTR exStyleBits = 0;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        for (int i = 0; i < kMaxStyleFixes; i++) {
            if (g_styleFixes[i].hWnd != hWnd) {
                continue;
            }
            styleBits = g_styleFixes[i].styleBits;
            exStyleBits = g_styleFixes[i].exStyleBits;
            g_styleFixes[i] = {};
            break;
        }
    }
    if (!styleBits && !exStyleBits) {
        return;
    }
    if (styleBits) {
        SetWindowLongPtrW(hWnd, GWL_STYLE,
                          GetWindowLongPtrW(hWnd, GWL_STYLE) | styleBits);
    }
    if (exStyleBits) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE,
                          GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | exStyleBits);
    }
    RefreshFrame(hWnd);
}

LRESULT CALLBACK ModSubclassProc(HWND hWnd, UINT message, WPARAM wParam,
                                 LPARAM lParam, DWORD_PTR refData);

void ForgetSubclass(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_stateLock);
    g_subclassed.erase(hWnd);
    for (int i = 0; i < kMaxStyleFixes; i++) {
        if (g_styleFixes[i].hWnd == hWnd) {
            g_styleFixes[i] = {};
        }
    }
}

bool IsSubclassed(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_stateLock);
    return g_subclassed.count(hWnd) != 0;
}

// Subclass only on the owning thread; defer work rather than blocking a busy thread.

// Track threads requiring a later owning-thread walk.
std::unordered_set<DWORD> g_pendingThreads;

void NoteWalkPendingFor(HWND hWnd) {
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        g_pendingThreads.insert(threadId);
    }
    PostMessageW(hWnd, OwnThreadSetupMessage(), 0, 0);
}

bool ThreadIsPending(DWORD threadId) {
    std::lock_guard<std::mutex> lock(g_stateLock);
    return g_pendingThreads.count(threadId) != 0;
}

void ClearPendingThread(DWORD threadId) {
    std::lock_guard<std::mutex> lock(g_stateLock);
    g_pendingThreads.erase(threadId);
}

bool SubclassWith(HWND hWnd, WindowKind kind) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    if (!OnOwningThread(hWnd)) {
        NoteWalkPendingFor(hWnd);
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        if (g_subclassed.count(hWnd) || g_adding.count(hWnd)) {
            return false;
        }
        g_adding.insert(hWnd);
    }
    const BOOL added = WindhawkUtils::SetWindowSubclassFromAnyThread(
        hWnd, ModSubclassProc, static_cast<DWORD_PTR>(kind));
    bool keep = false;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        g_adding.erase(hWnd);
        if (added && g_active) {
            g_subclassed.insert(hWnd);
            keep = true;
        }
    }
    if (added && !keep) {
        // Unload may have started while subclass installation was in progress.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ModSubclassProc);
        return false;
    }
    if (!added) {
        WCHAR className[64] = {};
        GetClassNameW(hWnd, className, ARRAYSIZE(className));
        Wh_Log(L"could not subclass a %s window", className);
    }
    return added != FALSE;
}

// Remove subclasses without holding the state lock across cross-thread calls.
void RemoveAllSubclasses() {
    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        windows.assign(g_subclassed.begin(), g_subclassed.end());
        g_subclassed.clear();
        g_adding.clear();
    }
    for (HWND hWnd : windows) {
        if (!IsWindow(hWnd)) {
            continue;
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ModSubclassProc);
    }
}
LRESULT HandleBackdrop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        RECT client = {};
        GetClientRect(hWnd, &client);
        // Report a handled erase only when the fill succeeds.
        if (dc && g_backgroundBrush && FillRect(dc, &client, g_backgroundBrush)) {
            return 1;
        }
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }

    LRESULT result =
        DefSubclassProc(hWnd, message, wParam, lParam);

    if (message == WM_PAINT) {
        CoverPageFrame(hWnd);
    }

    if (message == WM_ACTIVATE && LOWORD(wParam) == 0) {
        HWND popup = GetWindow(hWnd, GW_ENABLEDPOPUP);
        if (popup && popup != hWnd) {
            ThemeDialog(popup);
        }
    }
    return result;
}

LRESULT HandleStrip(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_NOTIFY) {
        NMHDR* header = reinterpret_cast<NMHDR*>(lParam);
        WCHAR fromClass[64] = {};
        bool fromToolbar =
            header && header->code == NM_CUSTOMDRAW &&
            GetClassNameW(header->hwndFrom, fromClass, ARRAYSIZE(fromClass)) &&
            wcscmp(fromClass, L"ToolbarWindow32") == 0;
        if (fromToolbar) {
            NMCUSTOMDRAW* custom = reinterpret_cast<NMCUSTOMDRAW*>(lParam);
            if (custom->dwDrawStage == CDDS_PREPAINT) {
                // MMC also custom-draws the maximized child's menu icon.
                // Preserve its handler and request our hover notifications too.
                return DefSubclassProc(hWnd, message, wParam, lParam) |
                       CDRF_NOTIFYITEMDRAW;
            }
            if (custom->dwDrawStage == CDDS_ITEMPREPAINT) {
                HotEntry* entry =
                    HotFor(static_cast<HWND>(header->hwndFrom), false);
                bool isHot =
                    (custom->uItemState & CDIS_HOT) != 0 ||
                    (entry && entry->valid &&
                     entry->rect.left == custom->rc.left &&
                     entry->rect.top == custom->rc.top);
                if (isHot && custom->hdc) {
                    if (!g_hoverBrush) {
                        g_hoverBrush = CreateSolidBrush(kColorHover);
                    }
                    if (g_hoverBrush) {
                        FillRect(custom->hdc, &custom->rc, g_hoverBrush);
                    }
                }
                // The native handler must see this before the toolbar draws.
                return DefSubclassProc(hWnd, message, wParam, lParam);
            }
        }
    }

    WCHAR className[64] = {};
    bool isBand = GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                  NameContains(className, L"rebar");
    if (isBand && message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        RECT client = {};
        GetClientRect(hWnd, &client);
        if (dc && g_backgroundBrush && FillRect(dc, &client, g_backgroundBrush)) {
            return 1;
        }
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }

    LRESULT result =
        DefSubclassProc(hWnd, message, wParam, lParam);

    return result;
}

WCHAR LowerAscii(WCHAR c) {
    return (c >= L'A' && c <= L'Z') ? static_cast<WCHAR>(c + 32) : c;
}

bool NameContains(PCWSTR name, PCWSTR needle) {
    size_t needleLength = wcslen(needle);
    if (!needleLength) {
        return false;
    }
    for (size_t i = 0; name[i]; i++) {
        size_t j = 0;
        while (j < needleLength && name[i + j] &&
               LowerAscii(name[i + j]) == LowerAscii(needle[j])) {
            j++;
        }
        if (j == needleLength) {
            return true;
        }
    }
    return false;
}

void RefreshFrame(HWND hWnd) {
    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
                 SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

bool IsBorderHost(PCWSTR name) {
    return NameContains(name, L"mdiclient") ||
           NameContains(name, L"mmcchildfrm") ||
           NameContains(name, L"mmcviewwindow") ||
           NameContains(name, L"mmcocxviewwindow") ||
           NameContains(name, L"afxframeorview") ||
           NameContains(name, L"afxolecontrol");
}

LRESULT HandleNoBorder(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_NCPAINT) {
        return 0;
    }
    return DefSubclassProc(hWnd, message, wParam, lParam);
}

void StripFrame(HWND hWnd) {
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    bool changed = false;

    if (exStyle & WS_EX_CLIENTEDGE) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_CLIENTEDGE);
        NoteRemovedStyles(hWnd, 0, WS_EX_CLIENTEDGE);
        changed = true;
    }
    if (style & WS_BORDER) {
        SetWindowLongPtrW(hWnd, GWL_STYLE, style & ~WS_BORDER);
        NoteRemovedStyles(hWnd, WS_BORDER, 0);
        changed = true;
    }
    if (changed) {
        RefreshFrame(hWnd);
        SubclassWith(hWnd, WindowKind::NoBorder);
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

bool RemoveClientEdge(HWND hWnd) {
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_CLIENTEDGE)) {
        return false;
    }
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_CLIENTEDGE);
    NoteRemovedStyles(hWnd, 0, WS_EX_CLIENTEDGE);
    RefreshFrame(hWnd);
    InvalidateRect(hWnd, nullptr, TRUE);
    return true;
}

// Paint supported top tabs from native geometry; fall back if labels are unavailable.
constexpr int kMaxTabs = 16;
constexpr int kTabLabelChars = 48;

struct TabEntry {
    HWND hWnd;
    int count;
    int selected;
    bool usable;
    HFONT font;
    RECT items[kMaxTabs];
    WCHAR labels[kMaxTabs][kTabLabelChars];
};
thread_local TabEntry g_tabEntries[4] = {};

TabEntry* TabEntryFor(HWND hWnd, bool create) {
    const int slots = static_cast<int>(ARRAYSIZE(g_tabEntries));
    for (int i = 0; i < slots; i++) {
        if (g_tabEntries[i].hWnd == hWnd) {
            return &g_tabEntries[i];
        }
    }
    if (!create) {
        return nullptr;
    }
    for (int i = 0; i < slots; i++) {
        if (!g_tabEntries[i].hWnd) {
            g_tabEntries[i] = {};
            g_tabEntries[i].hWnd = hWnd;
            return &g_tabEntries[i];
        }
    }
    return nullptr;
}

bool RefreshTabEntry(HWND hWnd) {
    if (!OnOwningThread(hWnd)) {
        AskOwningThread(hWnd);
        return false;
    }
    TabEntry* entry = TabEntryFor(hWnd, true);
    if (!entry) {
        return false;
    }
    entry->usable = false;
    // Leave bottom, vertical and button-style tabs to the native control.
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & (TCS_BOTTOM | TCS_VERTICAL | TCS_BUTTONS)) {
        entry->count = 0;
        return false;
    }
    const int count = static_cast<int>(SendToControl(hWnd, TCM_GETITEMCOUNT, 0, 0));
    if (count <= 0 || count > kMaxTabs) {
        entry->count = 0;
        return false;
    }
    entry->count = count;
    for (int i = 0; i < count; i++) {
        entry->items[i] = {};
        if (!SendToControl(hWnd, TCM_GETITEMRECT, static_cast<WPARAM>(i),
                           reinterpret_cast<LPARAM>(&entry->items[i]))) {
            entry->count = 0;
            return false;
        }
        TCITEMW item = {};
        item.mask = TCIF_TEXT;
        item.pszText = entry->labels[i];
        item.cchTextMax = kTabLabelChars - 1;
        entry->labels[i][0] = 0;
        SendToControl(hWnd, TCM_GETITEMW, static_cast<WPARAM>(i),
                      reinterpret_cast<LPARAM>(&item));
    }
    entry->selected = static_cast<int>(SendToControl(hWnd, TCM_GETCURSEL, 0, 0));
    entry->font = reinterpret_cast<HFONT>(SendToControl(hWnd, WM_GETFONT, 0, 0));
    for (int i = 0; i < count; i++) {
        if (entry->labels[i][0]) {
            entry->usable = true;
            break;
        }
    }
    return entry->usable;
}

void PaintTabStrip(HWND hWnd) {
    PAINTSTRUCT ps = {};
    HDC dc = BeginPaint(hWnd, &ps);
    if (!dc) {
        return;
    }
    RECT client = {};
    GetClientRect(hWnd, &client);
    if (g_backgroundBrush) {
        FillRect(dc, &client, g_backgroundBrush);
    }

    TabEntry* entry = TabEntryFor(hWnd, false);
    HFONT font = entry ? entry->font : nullptr;
    HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;

    int selectedLeft = 0;
    int selectedRight = 0;
    if (entry) {
        for (int i = 0; i < entry->count; i++) {
            RECT tab = entry->items[i];
            const bool selected = (i == entry->selected);
            if (selected) {
                HBRUSH brush = TabSelectedBrush();
                if (brush) {
                    FillRect(dc, &tab, brush);
                }
                selectedLeft = tab.left;
                selectedRight = tab.right;
            }
            RECT label = tab;
            label.left += 6;
            label.right -= 6;
            DrawTextOn(dc, entry->labels[i], &label,
                       DT_CENTER | DT_SINGLELINE | DT_VCENTER,
                       selected ? kColorText : kColorTabText,
                       selected ? kColorTabSelected : kColorBackground);
        }
    }

    HBRUSH line = LineBrush();
    if (line && client.bottom > client.top) {
        RECT left = {client.left, client.bottom - 1, selectedLeft, client.bottom};
        RECT right = {selectedRight, client.bottom - 1, client.right, client.bottom};
        if (left.right > left.left) {
            FillRect(dc, &left, line);
        }
        if (right.right > right.left) {
            FillRect(dc, &right, line);
        }
    }

    if (oldFont) {
        SelectObject(dc, oldFont);
    }
    EndPaint(hWnd, &ps);
}

LRESULT HandleTab(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    // WM_SETFONT uses wParam for the font; lParam is the redraw flag.
    if (message == WM_SETFONT) {
        LRESULT result =
            DefSubclassProc(hWnd, message, wParam, lParam);
        TabEntry* entry = TabEntryFor(hWnd, false);
        if (entry) {
            entry->font = reinterpret_cast<HFONT>(wParam);
        }
        return result;
    }

    if (message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        RECT client = {};
        GetClientRect(hWnd, &client);
        // Report a handled erase only when the fill succeeds.
        if (dc && g_backgroundBrush && FillRect(dc, &client, g_backgroundBrush)) {
            return 1;
        }
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }

    if (message == WM_PAINT) {
        TabEntry* entry = TabEntryFor(hWnd, false);
        if (!entry || !entry->usable) {
            if (!RefreshTabEntry(hWnd) || !entry) {
                return DefSubclassProc(hWnd, message, wParam, lParam);
            }
        }
        PaintTabStrip(hWnd);
        return 0;
    }

    if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP ||
        message == WM_KEYUP) {
        LRESULT result =
            DefSubclassProc(hWnd, message, wParam, lParam);
        RefreshTabEntry(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
        return result;
    }

    if (message == WM_THEMECHANGED) {
        LRESULT result =
            DefSubclassProc(hWnd, message, wParam, lParam);
        RefreshTabEntry(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
        return result;
    }

    if (message == WM_SIZE || message == WM_SHOWWINDOW ||
        message == WM_WINDOWPOSCHANGED) {
        LRESULT result =
            DefSubclassProc(hWnd, message, wParam, lParam);
        TabEntry* entry = TabEntryFor(hWnd, false);
        if (!entry || !entry->usable) {
            RefreshTabEntry(hWnd);
        }
        return result;
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

// Keep MMC custom-tab drawing and input native; replace only its background erase.
LRESULT HandleMmcCustomTab(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        RECT client = {};
        if (dc && g_backgroundBrush && GetClientRect(hWnd, &client) &&
            FillRect(dc, &client, g_backgroundBrush)) {
            return 1;
        }
    }
    return DefSubclassProc(hWnd, message, wParam, lParam);
}

bool IsMmcActionsPane(HWND hWnd) {
    WCHAR cls[64] = {};
    WCHAR parentCls[64] = {};
    HWND parent = GetParent(hWnd);
    return hWnd && parent &&
           GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
           wcscmp(cls, L"AfxFrameOrView42u") == 0 &&
           GetClassNameW(parent, parentCls, ARRAYSIZE(parentCls)) &&
           wcscmp(parentCls, L"MMCViewWindow") == 0 &&
           FindWindowExW(hWnd, nullptr, L"NativeHWNDHost", nullptr) != nullptr;
}

// Scope the Actions caption fill adjustment to its native erase call and HDC.
using DrawEdge_t = decltype(&DrawEdge);
DrawEdge_t pDrawEdge = nullptr;
thread_local HWND g_drawingDiskHeader = nullptr;
thread_local HDC g_actionsEraseDc = nullptr;
thread_local bool g_drawingMmcDescription = false;
thread_local bool g_drawingActionsContent = false;
thread_local bool g_drawingSummaryTitle = false;
thread_local HWND g_drawingSummaryButton = nullptr;

BOOL WINAPI DrawEdge_Hook(HDC dc, LPRECT rect, UINT edge, UINT flags) {
    if (!pDrawEdge) {
        return FALSE;
    }
    // Replace only the observed unthemed Disk header edge variants; preserve native content and input.
    RECT headerClient = {};
    if (g_active && g_drawingDiskHeader && dc && rect && pGetWindowTheme &&
        !pGetWindowTheme(g_drawingDiskHeader) &&
        GetClientRect(g_drawingDiskHeader, &headerClient) &&
        rect->top == headerClient.top && rect->bottom == headerClient.bottom &&
        ((edge == EDGE_RAISED && flags == (BF_RECT | BF_SOFT)) ||
         (edge == EDGE_SUNKEN && flags == (BF_RECT | BF_SOFT | BF_FLAT))) &&
        rect->right - rect->left >= 4 && rect->bottom - rect->top >= 4 &&
        g_lineBrush && g_backgroundBrush && g_hoverBrush) {
        RECT inner = *rect;
        InflateRect(&inner, -1, -1);
        HBRUSH innerBrush = edge == EDGE_SUNKEN ? g_hoverBrush : g_backgroundBrush;
        if (FrameRect(dc, rect, g_lineBrush) && FrameRect(dc, &inner, innerBrush)) {
            return TRUE;
        }
    }
    if (g_active && dc && dc == g_actionsEraseDc && rect &&
        (flags & BF_MIDDLE) && g_backgroundBrush &&
        FillRect(dc, rect, g_backgroundBrush)) {
        return pDrawEdge(dc, rect, edge, flags & ~static_cast<UINT>(BF_MIDDLE));
    }
    return pDrawEdge(dc, rect, edge, flags);
}

void DressExtraChrome(HWND hWnd, bool dark) {
    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return;
    }

    if (IsBorderHost(className)) {
        if (dark) {
            // Preserve the MDI child caption: WS_BORDER is part of WS_CAPTION.
            if (lstrcmpiW(className, L"MMCChildFrm") != 0) {
                StripFrame(hWnd);
            }
            if (IsMmcActionsPane(hWnd)) {
                SubclassWith(hWnd, WindowKind::ActionsPane);
            }
        }
        if (!NameContains(className, L"mdiclient")) {
            return;
        }
    }

    if (NameContains(className, L"toolbar")) {
        if (dark) {
            SubclassWith(hWnd, WindowKind::Toolbar);
        }
        return;
    }

    bool isStatusBar = NameContains(className, L"status");
    bool isStrip = NameContains(className, L"sizeablerebar") ||
                   NameContains(className, L"rebar");
    if (!isStatusBar && !isStrip) {
        return;
    }

    if (!dark) {
        return;
    }

    if (!g_backgroundBrush) {
        g_backgroundBrush = CreateSolidBrush(kColorBackground);
    }

    SubclassWith(hWnd, isStatusBar ? WindowKind::StatusBar : WindowKind::Strip);
    if (isStatusBar) {
        RefreshStatusCache(hWnd);
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

// Leave self-rendered web content to its host.
bool IsHtmlHost(HWND hWnd) {
    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    if (wcscmp(className, L"Shell Embedding") == 0) {
        return true;
    }
    return className[0] == L'A' && className[1] == L'T' && className[2] == L'L' &&
           className[3] == L':';
}

bool WindowClassStarts(HWND hWnd, PCWSTR prefix) {
    WCHAR name[128] = {};
    return hWnd && GetClassNameW(hWnd, name, ARRAYSIZE(name)) &&
           wcsncmp(name, prefix, wcslen(prefix)) == 0;
}

bool IsDiskManagementHeader(HWND hWnd) {
    HWND list = GetParent(hWnd);
    HWND view = list ? GetParent(list) : nullptr;
    HWND ocx = view ? GetParent(view) : nullptr;
    HWND atl = ocx ? GetParent(ocx) : nullptr;
    HWND mmc = atl ? GetParent(atl) : nullptr;
    return WindowClassStarts(hWnd, L"SysHeader32") &&
           WindowClassStarts(list, L"SysListView32") &&
           WindowClassStarts(view, L"AfxWnd42u") &&
           WindowClassStarts(ocx, L"AfxOleControl42u") &&
           WindowClassStarts(atl, L"AtlAxWinEx") &&
           WindowClassStarts(mmc, L"MMCOCXViewWindow");
}

bool IsMmcMdiButtonStrip(HWND hWnd);

// Scope the non-owner-drawn Scheduler combo to its observed host hierarchy.
bool IsMmcSummaryCombo(HWND hWnd) {
    if (!WindowClassStarts(hWnd, L"WindowsForms10.COMBOBOX.") ||
        (GetWindowLongPtrW(hWnd, GWL_STYLE) & 3) != CBS_DROPDOWNLIST ||
        (GetWindowLongPtrW(hWnd, GWL_STYLE) &
            (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE))) {
        return false;
    }
    HWND parent = hWnd;
    for (int i = 0; i < 7; ++i) {
        parent = GetParent(parent);
        if (!WindowClassStarts(parent, L"WindowsForms10.Window.")) {
            return false;
        }
    }
    HWND atl = GetParent(parent);
    HWND ax = atl ? GetParent(atl) : nullptr;
    HWND ocx = ax ? GetParent(ax) : nullptr;
    HWND mmcView = ocx ? GetParent(ocx) : nullptr;
    return WindowClassStarts(atl, L"ATL:") &&
           WindowClassStarts(ax, L"AtlAxWinEx") &&
           WindowClassStarts(ocx, L"MMCOCXViewWindow") &&
           WindowClassStarts(mmcView, L"MMCViewWindow");
}

bool IsMmcSummaryComboPart(HWND combo, HWND part) {
    if (!part || !IsMmcSummaryCombo(combo)) {
        return false;
    }
    COMBOBOXINFO info = {};
    info.cbSize = sizeof(info);
    return GetComboBoxInfo(combo, &info) &&
           (part == combo || part == info.hwndItem || part == info.hwndList);
}

// Scope the Scheduler Refresh button to its observed hierarchy and geometry.
bool IsMmcSummaryRefreshCandidate(HWND hWnd) {
    if (!WindowClassStarts(hWnd, L"WindowsForms10.BUTTON.") ||
        (GetWindowLongPtrW(hWnd, GWL_STYLE) & BS_TYPEMASK) != BS_OWNERDRAW) {
        return false;
    }
    HWND panel = GetParent(hWnd);
    HWND form = panel ? GetParent(panel) : nullptr;
    HWND container = form ? GetParent(form) : nullptr;
    HWND atl = container ? GetParent(container) : nullptr;
    HWND ax = atl ? GetParent(atl) : nullptr;
    HWND ocx = ax ? GetParent(ax) : nullptr;
    if (!WindowClassStarts(panel, L"WindowsForms10.Window.") ||
        !WindowClassStarts(form, L"WindowsForms10.Window.") ||
        !WindowClassStarts(container, L"WindowsForms10.Window.") ||
        !WindowClassStarts(atl, L"ATL:") ||
        !WindowClassStarts(ax, L"AtlAxWinEx") ||
        !WindowClassStarts(ocx, L"MMCOCXViewWindow")) {
        return false;
    }
    return true;
}

bool IsMmcSummaryRefresh(HWND hWnd) {
    if (!IsMmcSummaryRefreshCandidate(hWnd)) {
        return false;
    }
    HWND panel = GetParent(hWnd);
    HWND form = GetParent(panel);
    RECT p = {}, f = {}, b = {};
    return GetWindowRect(panel, &p) && GetWindowRect(form, &f) &&
           GetWindowRect(hWnd, &b) && p.left == f.left && p.right == f.right &&
           p.bottom == f.bottom && p.top > f.top && b.left >= p.left &&
           b.right <= p.right && b.top >= p.top && b.bottom <= p.bottom &&
           b.bottom > b.top && b.right - b.left > 2 * (b.bottom - b.top);
}

// Scope the Scheduler title adjustment to its observed WinForms label hierarchy.
bool IsMmcWinFormsLabel(HWND hWnd) {
    HWND panel = GetParent(hWnd);
    HWND form = panel ? GetParent(panel) : nullptr;
    HWND container = form ? GetParent(form) : nullptr;
    HWND atl = container ? GetParent(container) : nullptr;
    HWND atlAx = atl ? GetParent(atl) : nullptr;
    HWND ocx = atlAx ? GetParent(atlAx) : nullptr;
    return WindowClassStarts(hWnd, L"WindowsForms10.STATIC.") &&
           (GetWindowLongPtrW(hWnd, GWL_STYLE) & SS_TYPEMASK) == SS_OWNERDRAW &&
           WindowClassStarts(panel, L"WindowsForms10.Window.") &&
           WindowClassStarts(form, L"WindowsForms10.Window.") &&
           WindowClassStarts(container, L"WindowsForms10.Window.") &&
           WindowClassStarts(atl, L"ATL:") &&
           WindowClassStarts(atlAx, L"AtlAxWinEx") &&
           WindowClassStarts(ocx, L"MMCOCXViewWindow");
}

bool IsMmcSummaryTitle(HWND hWnd) {
    if (!IsMmcWinFormsLabel(hWnd)) {
        return false;
    }
    HWND panel = GetParent(hWnd);
    HWND form = GetParent(panel);
    RECT p = {}, f = {};
    if (!GetWindowRect(panel, &p) || !GetWindowRect(form, &f)) {
        return false;
    }
    return IsWindowVisible(hWnd) && p.top == f.top && p.left == f.left &&
           p.right == f.right && p.bottom > p.top && f.bottom > p.bottom;
}

#if defined(_WIN64)
void TryInstallActionsElementHook(HWND actionsWindow);
#endif

void ApplyToWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }

    // Apply window changes on the owning thread; defer foreign-thread work.
    if (!OnOwningThread(hWnd)) {
        NoteWalkPendingFor(hWnd);
        return;
    }

    if (IsHtmlHost(hWnd)) {
        return;
    }

    // Limit MMC custom-tab handling to the console view.
    WCHAR classForMmcTab[64] = {};
    WCHAR parentForMmcTab[64] = {};
    HWND parent = GetParent(hWnd);
    if (GetClassNameW(hWnd, classForMmcTab, ARRAYSIZE(classForMmcTab)) &&
        wcscmp(classForMmcTab, L"AMCCustomTab") == 0 && parent &&
        GetClassNameW(parent, parentForMmcTab, ARRAYSIZE(parentForMmcTab)) &&
        wcscmp(parentForMmcTab, L"MMCViewWindow") == 0) {
        if (SubclassWith(hWnd, WindowKind::MmcCustomTab)) {
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    }

    // Repaint the console tree frame without changing its style or hit testing.
    WCHAR treeClass[64] = {};
    WCHAR treeParentClass[64] = {};
    HWND treeParent = GetParent(hWnd);
    if (GetClassNameW(hWnd, treeClass, ARRAYSIZE(treeClass)) &&
        wcscmp(treeClass, L"SysTreeView32") == 0 && treeParent &&
        GetClassNameW(treeParent, treeParentClass, ARRAYSIZE(treeParentClass)) &&
        wcscmp(treeParentClass, L"MMCViewWindow") == 0 &&
        ((GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_CLIENTEDGE) ||
         (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_BORDER))) {
        if (SubclassWith(hWnd, WindowKind::TreeFrame)) {
            RedrawWindow(hWnd, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_FRAME);
        }
    }

    if (IsDiskManagementHeader(hWnd)) {
        if (SubclassWith(hWnd, WindowKind::DiskHeader)) {
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    }

    if (IsMmcSummaryRefreshCandidate(hWnd)) {
        if (SubclassWith(hWnd, WindowKind::WinFormsRefresh)) {
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    }

    if (IsMmcSummaryCombo(hWnd)) {
        // The combo and its parent may both answer colour requests.
        SubclassWith(hWnd, WindowKind::WinFormsCombo);
        SubclassWith(GetParent(hWnd), WindowKind::WinFormsComboParent);
    }

    if (IsMmcWinFormsLabel(hWnd)) {
        if (SubclassWith(hWnd, WindowKind::WinFormsLabel)) {
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    }

    WCHAR currentClass[64] = {};
    if (GetClassNameW(hWnd, currentClass, ARRAYSIZE(currentClass)) &&
        wcscmp(currentClass, L"NativeHWNDHost") == 0) {
        HWND actions = GetParent(hWnd);
        if (IsMmcActionsPane(actions) && SubclassWith(actions, WindowKind::ActionsPane)) {
            InvalidateRect(actions, nullptr, TRUE);
        }
    }

    // The Actions sections are windowless DirectUI elements under NativeHWNDHost.
    WCHAR actionsContentClass[64] = {}, actionsParentClass[64] = {};
    HWND actionsContentParent = GetParent(hWnd);
    if (GetClassNameW(hWnd, actionsContentClass, ARRAYSIZE(actionsContentClass)) &&
        wcscmp(actionsContentClass, L"DirectUIHWND") == 0 &&
        actionsContentParent &&
        GetClassNameW(actionsContentParent, actionsParentClass, ARRAYSIZE(actionsParentClass)) &&
        wcscmp(actionsParentClass, L"NativeHWNDHost") == 0 &&
        IsMmcActionsPane(GetParent(actionsContentParent))) {
#if defined(_WIN64)
        TryInstallActionsElementHook(hWnd);
#endif
        if (SubclassWith(hWnd, WindowKind::ActionsContent)) {
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    }

    pAllowDarkModeForWindow(hWnd, TRUE);
    pSetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);

    // Use classic toolbar drawing and handle its hot item separately.
    WCHAR toolbarClass[64] = {};
    if (GetClassNameW(hWnd, toolbarClass, ARRAYSIZE(toolbarClass)) &&
        wcscmp(toolbarClass, L"ToolbarWindow32") == 0) {
        pSetWindowTheme(hWnd, L"", L"");
        SubclassWith(hWnd, WindowKind::Toolbar);
    }

    if (IsMmcSummaryCombo(hWnd)) {
        pSetWindowTheme(hWnd, L"DarkMode_CFD", nullptr);
    }

    // Keep labels and links classic so their text uses the control colour path.
    WCHAR labelClass[64] = {};
    if (GetClassNameW(hWnd, labelClass, ARRAYSIZE(labelClass)) &&
        (wcscmp(labelClass, L"Static") == 0 ||
         wcscmp(labelClass, L"SysLink") == 0)) {
        pAllowDarkModeForWindow(hWnd, FALSE);
        pSetWindowTheme(hWnd, L"", L"");
    }

    // Headers use a separate visual-style name from their parent views.
    WCHAR headerClass[64] = {};
    if (GetClassNameW(hWnd, headerClass, ARRAYSIZE(headerClass)) &&
        wcscmp(headerClass, L"SysHeader32") == 0) {
        pSetWindowTheme(hWnd, L"DarkMode_ItemsView", nullptr);
    }
    DWORD_PTR ignored = 0;
    SendMessageTimeoutW(hWnd, WM_THEMECHANGED, 0, 0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &ignored);
    ColourClassicControl(hWnd, true);
    DressExtraChrome(hWnd, true);
    if (IsMmcMdiButtonStrip(hWnd)) {
        InvalidateRect(hWnd, nullptr, TRUE);
    }

    // Keep existing subclasses when handling the native owner-drawn description.
    WCHAR descriptionHostClass[64] = {};
    if (GetClassNameW(hWnd, descriptionHostClass, ARRAYSIZE(descriptionHostClass)) &&
        wcscmp(descriptionHostClass, L"MMCViewWindow") == 0) {
        SubclassWith(hWnd, WindowKind::DescriptionHost);
    }

    if (GetAncestor(hWnd, GA_ROOT) == hWnd) {
        ApplyToTopLevel(hWnd);
    }
}

// Set the text background for opaque drawing on dark surfaces.
void DrawTextOn(HDC dc, PCWSTR text, RECT* rect, UINT flags, COLORREF colour,
                COLORREF behind) {
    if (!dc || !text || !rect) {
        return;
    }
    SetBkMode(dc, OPAQUE);
    SetBkColor(dc, behind);
    SetTextColor(dc, colour);
    DrawTextW(dc, text, -1, rect, flags);
}

HBRUSH LineBrush() {
    if (!g_lineBrush) {
        g_lineBrush = CreateSolidBrush(kColorLine);
    }
    return g_lineBrush;
}

HBRUSH TabSelectedBrush() {
    if (!g_tabSelectedBrush) {
        g_tabSelectedBrush = CreateSolidBrush(kColorTabSelected);
    }
    return g_tabSelectedBrush;
}

// Repaint only the tree border; retain native geometry and input.
LRESULT HandleTreeFrame(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
    if (message == WM_NCPAINT || message == WM_PAINT) {
        RECT window = {};
        HBRUSH line = LineBrush();
        if (line && GetWindowRect(hWnd, &window)) {
            RECT edge = {0, 0, window.right - window.left,
                         window.bottom - window.top};
            HDC dc = GetWindowDC(hWnd);
            if (dc) {
                FrameRect(dc, &edge, line);
                ReleaseDC(hWnd, dc);
            }
        }
    }
    return result;
}

// Repaint only the unused header tail; retain native columns and hit targets.
LRESULT HandleDiskHeader(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HWND previousHeader = g_drawingDiskHeader;
    if (message == WM_PAINT || message == WM_PRINTCLIENT) {
        g_drawingDiskHeader = hWnd;
    }
    LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
    g_drawingDiskHeader = previousHeader;
    if (message != WM_PAINT || !g_backgroundBrush ||
        (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)) {
        return result;
    }
    RECT client = {};
    if (!GetClientRect(hWnd, &client) || client.right <= client.left) {
        return result;
    }
    LRESULT count = SendToControl(hWnd, HDM_GETITEMCOUNT, 0, 0);
    if (count < 0 || count > 256) {
        return result;
    }
    LONG rightmost = client.left;
    for (LRESULT i = 0; i < count; ++i) {
        RECT item = {};
        if (!SendToControl(hWnd, HDM_GETITEMRECT, static_cast<WPARAM>(i),
                           reinterpret_cast<LPARAM>(&item))) {
            return result;
        }
        if (item.right > rightmost) {
            rightmost = item.right;
        }
    }
    if (rightmost < client.right) {
        RECT blank = {rightmost, client.top, client.right, client.bottom};
        HDC dc = GetDC(hWnd);
        if (dc) {
            FillRect(dc, &blank, g_backgroundBrush);
            ReleaseDC(hWnd, dc);
        }
    }
    return result;
}

// Add a subtle edge after the field's native paint.
LRESULT HandleField(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    if (message == WM_PAINT) {
        LRESULT result =
            DefSubclassProc(hWnd, message, wParam, lParam);
        HBRUSH brush = LineBrush();
        HDC dc = GetDC(hWnd);
        if (brush && dc) {
            RECT client = {};
            GetClientRect(hWnd, &client);
            FrameRect(dc, &client, brush);
            ReleaseDC(hWnd, dc);
        }
        return result;
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

// Cache the group-box font outside paint.
struct GroupFont {
    HWND hWnd;
    HFONT font;
};
thread_local GroupFont g_groupFonts[32] = {};

void RememberGroupFont(HWND hWnd) {
    if (!OnOwningThread(hWnd)) {
        AskOwningThread(hWnd);
        return;
    }
    HFONT font = reinterpret_cast<HFONT>(SendToControl(hWnd, WM_GETFONT, 0, 0));
    if (!font) {
        return;
    }
    const int slots = static_cast<int>(ARRAYSIZE(g_groupFonts));
    int freeSlot = -1;
    for (int i = 0; i < slots; i++) {
        if (g_groupFonts[i].hWnd == hWnd) {
            g_groupFonts[i].font = font;
            return;
        }
        if (!g_groupFonts[i].hWnd && freeSlot < 0) {
            freeSlot = i;
        }
    }
    if (freeSlot >= 0) {
        g_groupFonts[freeSlot].hWnd = hWnd;
        g_groupFonts[freeSlot].font = font;
    }
}

HFONT GroupFontOf(HWND hWnd) {
    const int slots = static_cast<int>(ARRAYSIZE(g_groupFonts));
    for (int i = 0; i < slots; i++) {
        if (g_groupFonts[i].hWnd == hWnd) {
            return g_groupFonts[i].font;
        }
    }
    return nullptr;
}

void ForgetGroupFont(HWND hWnd) {
    const int slots = static_cast<int>(ARRAYSIZE(g_groupFonts));
    for (int i = 0; i < slots; i++) {
        if (g_groupFonts[i].hWnd == hWnd) {
            g_groupFonts[i].hWnd = nullptr;
            g_groupFonts[i].font = nullptr;
        }
    }
}

// Draw the group-box frame with the mod palette while retaining its native caption.
void PaintGroupBox(HWND hWnd) {
    PAINTSTRUCT ps = {};
    HDC dc = BeginPaint(hWnd, &ps);
    if (!dc) {
        return;
    }

    RECT client = {};
    GetClientRect(hWnd, &client);
    if (g_backgroundBrush) {
        FillRect(dc, &client, g_backgroundBrush);
    }

    HFONT font = GroupFontOf(hWnd);
    HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;

    WCHAR caption[128] = {};
    GetWindowTextW(hWnd, caption, ARRAYSIZE(caption) - 1);

    SIZE textSize = {};
    if (caption[0]) {
        GetTextExtentPoint32W(dc, caption, lstrlenW(caption), &textSize);
    }

    const int lineY = client.top + textSize.cy / 2;

    HGDIOBJ pen = CreatePen(PS_SOLID, 1, kColorLine);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));

    Rectangle(dc, client.left, lineY, client.right, client.bottom);

    if (caption[0] && g_backgroundBrush) {
        RECT captionRect = {client.left + 6, client.top, 0,
                            client.top + textSize.cy + 2};
        captionRect.right = captionRect.left + textSize.cx + 8;
        FillRect(dc, &captionRect, g_backgroundBrush);
        captionRect.left += 4;
        DrawTextOn(dc, caption, &captionRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER,
                   kColorText, kColorBackground);
    }

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    if (oldFont) {
        SelectObject(dc, oldFont);
    }
    DeleteObject(pen);
    EndPaint(hWnd, &ps);
}

LRESULT HandleGroupBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    if (message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        RECT client = {};
        GetClientRect(hWnd, &client);
        if (dc && g_backgroundBrush && FillRect(dc, &client, g_backgroundBrush)) {
            return 1;
        }
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }

    if (message == WM_PAINT) {
        PaintGroupBox(hWnd);
        return 0;
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

// Repaint the property-sheet frame without covering tabs or page content.
void ToClientOf(HWND hWnd, RECT* rc) {
    POINT origin = {0, 0};
    ClientToScreen(hWnd, &origin);
    OffsetRect(rc, -origin.x, -origin.y);
}

void CoverPageFrame(HWND sheet) {
    if (!g_backgroundBrush) {
        return;
    }
    HWND tab = FindWindowExW(sheet, nullptr, L"SysTabControl32", nullptr);
    if (!tab || !IsWindowVisible(tab)) {
        return;
    }
    HWND page = nullptr;
    HWND child = nullptr;
    while ((child = FindWindowExW(sheet, child, L"#32770", nullptr)) != nullptr) {
        if (IsWindowVisible(child)) {
            page = child;
            break;
        }
    }
    if (!page) {
        return;
    }

    RECT pageRect = {};
    RECT tabRect = {};
    GetWindowRect(page, &pageRect);
    GetWindowRect(tab, &tabRect);
    ToClientOf(sheet, &pageRect);
    ToClientOf(sheet, &tabRect);

    HDC dc = GetDC(sheet);
    if (!dc) {
        return;
    }
    const RECT outer = {pageRect.left - 2, tabRect.top - 2, pageRect.right + 2,
                        pageRect.bottom + 2};
    const RECT leftStrip = {outer.left, outer.top, outer.left + 2, outer.bottom};
    const RECT rightStrip = {outer.right - 2, outer.top, outer.right, outer.bottom};
    const RECT bottomStrip = {outer.left, outer.bottom - 2, outer.right,
                              outer.bottom};
    const RECT topLeft = {outer.left, outer.top, tabRect.left, outer.top + 2};
    const RECT topRight = {tabRect.right, outer.top, outer.right, outer.top + 2};

    if (pageRect.right <= pageRect.left || pageRect.bottom <= pageRect.top ||
        tabRect.bottom <= tabRect.top) {
        ReleaseDC(sheet, dc);
        return;
    }
    FillRect(dc, &leftStrip, g_backgroundBrush);
    FillRect(dc, &rightStrip, g_backgroundBrush);
    FillRect(dc, &bottomStrip, g_backgroundBrush);
    if (topLeft.right > topLeft.left) {
        FillRect(dc, &topLeft, g_backgroundBrush);
    }
    if (topRight.right > topRight.left) {
        FillRect(dc, &topRight, g_backgroundBrush);
    }
    ReleaseDC(sheet, dc);
}

void PaintClientEdge(HWND hWnd) {
    if (!g_backgroundBrush) {
        return;
    }
    if (!(GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_CLIENTEDGE)) {
        return;
    }
    HDC dc = GetWindowDC(hWnd);
    if (!dc) {
        return;
    }
    RECT window = {};
    GetWindowRect(hWnd, &window);
    RECT edge = {0, 0, window.right - window.left, window.bottom - window.top};
    FrameRect(dc, &edge, g_backgroundBrush);
    InflateRect(&edge, -1, -1);
    FrameRect(dc, &edge, g_backgroundBrush);
    ReleaseDC(hWnd, dc);
}

// Populate per-window caches and perform deferred walks on the owning thread.
LRESULT HandleOwnThreadSetup(HWND hWnd, WindowKind kind) {
    // A posted walk runs on the thread that owns the receiving window.
    const DWORD threadId = GetCurrentThreadId();
    if (ThreadIsPending(threadId)) {
        ClearPendingThread(threadId);
        WalkThreadWindows(threadId);
    }

    switch (kind) {
        case WindowKind::StatusBar:
            RefreshStatusCache(hWnd);
            break;
        case WindowKind::Tab:
            RefreshTabEntry(hWnd);
            break;
        case WindowKind::GroupBox:
            RememberGroupFont(hWnd);
            break;
        case WindowKind::Toolbar:
            if (UpdateHot(hWnd)) {
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            break;
        default:
            break;
    }
    return 0;
}

LRESULT HandleDialog(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // Keep native dialog painting; replace the background erase when possible.
    if (message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        RECT client = {};
        GetClientRect(hWnd, &client);
        if (dc && g_backgroundBrush && FillRect(dc, &client, g_backgroundBrush)) {
            return 1;
        }
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }

    if (message == WM_SHOWWINDOW && wParam) {
        EnumChildWindows(hWnd, ReapplyChild, 0);
    }

    if (message == WM_PAINT) {
        LRESULT result =
            DefSubclassProc(hWnd, message, wParam, lParam);
        CoverPageFrame(hWnd);
        return result;
    }

    if (message == WM_NCPAINT) {
        LRESULT result =
            DefSubclassProc(hWnd, message, wParam, lParam);
        PaintClientEdge(hWnd);
        return result;
    }

    // Read-only and writable fields use different colour notifications.
    if (message == WM_CTLCOLORDLG || message == WM_CTLCOLORSTATIC ||
        message == WM_CTLCOLORBTN || message == WM_CTLCOLOREDIT ||
        message == WM_CTLCOLORLISTBOX || message == WM_CTLCOLORSCROLLBAR ||
        message == WM_CTLCOLORMSGBOX) {
        if (wParam) {
            HDC dc = reinterpret_cast<HDC>(wParam);
            // Opaque text backgrounds provide a known colour for ClearType blending.
            SetBkMode(dc, OPAQUE);
            SetTextColor(dc, kColorText);
            SetBkColor(dc, kColorBackground);
        }
        return reinterpret_cast<LRESULT>(g_backgroundBrush);
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

BOOL CALLBACK ReapplyChild(HWND hWnd, LPARAM) {
    // Defer controls owned by another thread.
    if (!OnOwningThread(hWnd)) {
        NoteWalkPendingFor(hWnd);
        return TRUE;
    }
    ApplyToWindow(hWnd);

    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    // Remove unsuitable visual styles before applying classic-control colours.
    if (pGetWindowTheme && pGetWindowTheme(hWnd) != nullptr &&
        (wcscmp(className, L"Static") == 0 || wcscmp(className, L"SysLink") == 0 ||
         wcscmp(className, L"SysTabControl32") == 0)) {
        pSetWindowTheme(hWnd, L"", L"");
        if (wcscmp(className, L"SysTabControl32") == 0) {
            RefreshTabEntry(hWnd);
        }
        InvalidateRect(hWnd, nullptr, TRUE);
    }

    if (wcscmp(className, L"Button") == 0) {
        LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
        if ((style & BS_TYPEMASK) == BS_GROUPBOX) {
            SetWindowThemeQuiet(hWnd, L"", L"");
            SubclassWith(hWnd, WindowKind::GroupBox);
            RememberGroupFont(hWnd);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    }

    if (wcscmp(className, L"Edit") == 0) {
        if (RemoveClientEdge(hWnd)) {
            SubclassWith(hWnd, WindowKind::Field);
        }
    }

    if (wcscmp(className, L"#32770") == 0) {
        SubclassWith(hWnd, WindowKind::Dialog);
        SetWindowThemeQuiet(hWnd, L"", L"");
        InvalidateRect(hWnd, nullptr, TRUE);
    }

    if (wcscmp(className, L"SysTabControl32") == 0) {
        SetWindowThemeQuiet(hWnd, L"", L"");
        SubclassWith(hWnd, WindowKind::Tab);
        RefreshTabEntry(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
    }
    return TRUE;
}

void ThemeDialog(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || IsSubclassed(hWnd)) {
        return;
    }
    if (!OnOwningThread(hWnd)) {
        NoteWalkPendingFor(hWnd);
        return;
    }

    for (int depth = 0; depth < 3; depth++) {
        HWND parent = GetParent(hWnd);
        if (!parent || IsSubclassed(parent)) {
            break;
        }
        WCHAR parentClass[64] = {};
        if (!GetClassNameW(parent, parentClass, ARRAYSIZE(parentClass)) ||
            wcscmp(parentClass, L"#32770") != 0) {
            break;
        }
        SubclassWith(parent, WindowKind::Dialog);
        ApplyToWindow(parent);
        EnumChildWindows(parent, ReapplyChild, 0);
        InvalidateRect(parent, nullptr, TRUE);
        hWnd = parent;
    }
    if (!g_backgroundBrush) {
        g_backgroundBrush = CreateSolidBrush(kColorBackground);
    }
    SubclassWith(hWnd, WindowKind::Dialog);
    ApplyToWindow(hWnd);
    EnumChildWindows(hWnd, ReapplyChild, 0);
    InvalidateRect(hWnd, nullptr, TRUE);
}

void ResetWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }
    // Disable dark mode before removing each window's theme association.
    if (pAllowDarkModeForWindow) {
        pAllowDarkModeForWindow(hWnd, FALSE);
    }
    // Use NULL to remove the association rather than disabling visual styles.
    pSetWindowTheme(hWnd, nullptr, nullptr);
    RestoreRemovedStyles(hWnd);
    ColourClassicControl(hWnd, false);
    DressExtraChrome(hWnd, false);
    DWORD_PTR ignored = 0;
    SendMessageTimeoutW(hWnd, WM_THEMECHANGED, 0, 0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &ignored);
    if (GetAncestor(hWnd, GA_ROOT) == hWnd) {
        SetDarkTitleBar(hWnd, false);
        int preference = DWMWCP_DEFAULT;
        DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference,
                              sizeof(preference));
        // Restore the default DWM border colour during unload.
        DWORD border = DWMWA_COLOR_DEFAULT;
        DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &border, sizeof(border));
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

struct WalkResult {
    int topLevel;
    int children;
};

void ForceRepaint(HWND hWnd);

BOOL CALLBACK WalkChild(HWND hWnd, LPARAM lParam) {
    WalkResult* result = reinterpret_cast<WalkResult*>(lParam);
    if (!hWnd || !IsWindow(hWnd)) {
        return TRUE;
    }
    // Only reset windows owned by the current thread.
    if (!OnOwningThread(hWnd)) {
        return TRUE;
    }
    if (g_active) {
        ApplyToWindow(hWnd);
    } else {
        ResetWindow(hWnd);
    }
    result->children++;
    return TRUE;
}

BOOL CALLBACK WalkTop(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }
    WalkResult* result = reinterpret_cast<WalkResult*>(lParam);
    if (!OnOwningThread(hWnd)) {
        return TRUE;
    }
    if (g_active) {
        ApplyToWindow(hWnd);
    } else {
        ResetWindow(hWnd);
        ForceRepaint(hWnd);
    }
    EnumChildWindows(hWnd, WalkChild, lParam);
    result->topLevel++;
    return TRUE;
}

// Invalidate restored windows and their children to remove stale dark pixels.
void ForceRepaint(HWND hWnd) {
    RedrawWindow(hWnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
}

// Only one walk runs per window-owning thread at a time.
thread_local bool g_inWalk = false;

BOOL CALLBACK WalkOwnedChild(HWND hWnd, LPARAM lParam) {
    if (OnOwningThread(hWnd)) {
        return WalkChild(hWnd, lParam);
    }
    return TRUE;
}

// Walk the process tree but modify only windows owned by this thread.
BOOL CALLBACK WalkOwnedInTree(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }
    if (OnOwningThread(hWnd)) {
        return WalkTop(hWnd, lParam);
    }
    EnumChildWindows(hWnd, WalkOwnedChild, lParam);
    return TRUE;
}

void WalkThreadWindows(DWORD threadId) {
    if (threadId != GetCurrentThreadId() || g_inWalk) {
        return;
    }
    g_inWalk = true;
    WalkResult result = {};
    EnumWindows(WalkOwnedInTree, reinterpret_cast<LPARAM>(&result));
    g_inWalk = false;
    Wh_Log(L"thread %u: %s %d windows, %d children", threadId,
           g_active ? L"themed" : L"restored", result.topLevel, result.children);
}

// Dispatch walks to owning threads. Unload waits for completion before removing hooks.

// The dispatched function has no per-call state and remains valid while the mod is loaded.
using RunOnWindowThreadProc = void (*)();

void WalkThisThread() {
    WalkThreadWindows(GetCurrentThreadId());
}

UINT RemoteCallMessage() {
    static const UINT message =
        RegisterWindowMessageW(L"MMC Dark Mode: run this on the window's thread");
    return message;
}

LRESULT CALLBACK RemoteCallHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* info = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (info && info->message == RemoteCallMessage()) {
            RunOnWindowThreadProc proc =
                reinterpret_cast<RunOnWindowThreadProc>(info->lParam);
            if (proc) {
                proc();
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Timed walks may be deferred until a busy thread creates another window.
constexpr UINT kRemoteCallTimeoutMs = 2000;

// Unload waits for the target thread; normal walks use a bounded timeout.
bool RunOnWindowThread(HWND hWnd, RunOnWindowThreadProc proc, bool waitForIt) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    const DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        proc();
        return true;
    }
    HHOOK hook =
        SetWindowsHookExW(WH_CALLWNDPROC, RemoteCallHook, nullptr, threadId);
    if (!hook) {
        Wh_Log(L"could not hook thread %u to walk its windows", threadId);
        return false;
    }

    bool delivered = true;
    if (waitForIt) {
        // Unload must wait for the target thread before the mod can be removed.
        SendMessageW(hWnd, RemoteCallMessage(), 0,
                     reinterpret_cast<LPARAM>(proc));
    } else {
        DWORD_PTR ignored = 0;
        if (!SendMessageTimeoutW(hWnd, RemoteCallMessage(), 0,
                                 reinterpret_cast<LPARAM>(proc),
                                 SMTO_ABORTIFHUNG | SMTO_BLOCK,
                                 kRemoteCallTimeoutMs, &ignored)) {
            delivered = false;
            Wh_Log(L"thread %u did not answer within %u ms", threadId,
                   kRemoteCallTimeoutMs);
        }
    }

    // Remove the hook after the send; a late message must not call unloaded code.
    UnhookWindowsHookEx(hook);
    return delivered;
}

void AddThreadOf(HWND hWnd, std::unordered_map<DWORD, HWND>* threads) {
    DWORD pid = 0;
    const DWORD threadId = GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId() && threadId) {
        threads->emplace(threadId, hWnd);
    }
}

BOOL CALLBACK CollectChildThread(HWND hWnd, LPARAM lParam) {
    AddThreadOf(hWnd, reinterpret_cast<std::unordered_map<DWORD, HWND>*>(lParam));
    return TRUE;
}

BOOL CALLBACK CollectThread(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }
    AddThreadOf(hWnd, reinterpret_cast<std::unordered_map<DWORD, HWND>*>(lParam));
    EnumChildWindows(hWnd, CollectChildThread, lParam);
    return TRUE;
}

// Collect window-owning threads and dispatch one walk per thread.
void WalkEveryThread(bool waitForIt) {
    std::unordered_map<DWORD, HWND> threads;
    EnumWindows(CollectThread, reinterpret_cast<LPARAM>(&threads));
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        for (HWND hWnd : g_subclassed) {
            if (IsWindow(hWnd)) {
                AddThreadOf(hWnd, &threads);
            }
        }
    }
    Wh_Log(L"%s %zu threads%s", g_active ? L"theming" : L"restoring",
           threads.size(), waitForIt ? L", waiting for each" : L"");
    int done = 0;
    int deferred = 0;
    for (const auto& entry : threads) {
        if (RunOnWindowThread(entry.second, WalkThisThread, waitForIt)) {
            done++;
            continue;
        }
        // Leave busy threads pending during activation; unload uses a blocking walk.
        if (!waitForIt) {
            NoteWalkPendingFor(entry.second);
            deferred++;
        }
    }
    Wh_Log(L"%s done on %d of %zu threads, %d deferred", g_active ? L"theme"
           : L"restore", done, threads.size(), deferred);
}

constexpr int kColScrollbar = 0;
// Extended View uses ActiveCaption and CaptionText for its HTML and icon.
constexpr int kColActiveCaption = 2;
constexpr int kColCaptionText = 9;
constexpr int kColGradientActiveCaption = 27;
constexpr int kSysColourCount = 23;
constexpr int kColHighlight = 13;
constexpr int kColHighlightText = 14;
constexpr int kColWindowFrame = 6;
constexpr int kColWindow = 5;
constexpr int kColWindowText = 8;
constexpr int kColMenuText = 7;
constexpr int kColBtnFace = 15;
constexpr int kColBtnShadow = 16;
constexpr int kColGrayText = 17;
constexpr int kColBtnText = 18;
constexpr int kColBtnHighlight = 20;
constexpr int kCol3DDkShadow = 21;
constexpr int kCol3DLight = 22;

HBRUSH g_sysBrushes[kSysColourCount];

COLORREF MapSystemColour(int index) {
    switch (index) {
    case kColWindow:
    case kColScrollbar:
    case kColBtnFace:
    case kColBtnHighlight:
    case kCol3DLight:
        return kColorBackground;
    case kColWindowText:
    case kColMenuText:
    case kColBtnText:
        return kColorText;
    case kColBtnShadow:
    case kColWindowFrame:
    case kCol3DDkShadow:
        return kColorLine;
    case kColHighlight:
        return kColorSelection;
    case kColHighlightText:
        return kColorText;
    case kColGrayText:
        return RGB(140, 140, 140);
    default:
        return 0xffffffff;  // not remapped
    }
}

// Scope Extended View caption colours to its renderer and node manager.
bool IsExtendedViewColourCaller(void* address) {
    HMODULE caller = nullptr;
    if (!address || !GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(address), &caller)) {
        return false;
    }
    HMODULE html = GetModuleHandleW(L"mshtml.dll");
    HMODULE nodeManager = GetModuleHandleW(L"mmcndmgr.dll");
    return caller && (caller == html || caller == nodeManager);
}

bool IsDirectUiCaller(void* address) {
    HMODULE caller = nullptr;
    return address && GetModuleHandleExW(
               GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
               reinterpret_cast<LPCWSTR>(address), &caller) &&
           caller && caller == GetModuleHandleW(L"dui70.dll");
}

// Scope the splitter replacement to the small dmview memory-DC frame.
using DrawFrameControl_t = decltype(&DrawFrameControl);
DrawFrameControl_t pDrawFrameControl = nullptr;

BOOL WINAPI DrawFrameControl_Hook(HDC dc, LPRECT rect, UINT type, UINT state) {
    HMODULE caller = nullptr;
    if (g_active && dc && rect && type == DFC_BUTTON && state == DFCS_BUTTONPUSH &&
        rect->right - rect->left >= 64 && rect->bottom - rect->top > 0 &&
        rect->bottom - rect->top <= 12 && GetObjectType(dc) == OBJ_MEMDC &&
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(__builtin_return_address(0)),
                           &caller) && caller &&
        caller == GetModuleHandleW(L"dmview.ocx") && g_backgroundBrush &&
        FillRect(dc, rect, g_backgroundBrush)) {
        RECT line = *rect;
        line.top += (line.bottom - line.top) / 2;
        line.bottom = line.top + 1;
        if (HBRUSH brush = LineBrush()) {
            FillRect(dc, &line, brush);
        }
        return TRUE;
    }
    return pDrawFrameControl ? pDrawFrameControl(dc, rect, type, state) : FALSE;
}

// Recolour only the observed navy hatch from dmview; preserve patterns and other colours.
using CreateHatchBrush_t = decltype(&CreateHatchBrush);
CreateHatchBrush_t pCreateHatchBrush = nullptr;
HBRUSH WINAPI CreateHatchBrush_Hook(int hatch, COLORREF color) {
    if (!pCreateHatchBrush) return nullptr;
    HMODULE caller = nullptr;
    if (g_active && color == RGB(0, 0, 128) &&
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(__builtin_return_address(0)),
                           &caller) && caller && caller == GetModuleHandleW(L"dmview.ocx")) {
        HBRUSH result = pCreateHatchBrush(hatch, RGB(55, 125, 230));
        if (result) return result;
    }
    return pCreateHatchBrush(hatch, color);
}

// Identify MMC's small MDI-button strip without changing its input handling.
bool IsMmcMdiButtonStrip(HWND hWnd) {
    WCHAR className[64] = {}, rootClass[64] = {};
    if (!hWnd || !GetClassNameW(hWnd, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"AfxWnd42u") != 0) {
        return false;
    }
    HWND root = GetAncestor(hWnd, GA_ROOT);
    if (!root || root == hWnd ||
        !GetClassNameW(root, rootClass, ARRAYSIZE(rootClass)) ||
        wcscmp(rootClass, L"MMCMainFrame") != 0) {
        return false;
    }
    RECT window = {}, frame = {};
    if (!GetWindowRect(hWnd, &window) || !GetWindowRect(root, &frame)) {
        return false;
    }
    int width = window.right - window.left;
    int height = window.bottom - window.top;
    int buttonWidth = GetSystemMetrics(SM_CXSMSIZE);
    int buttonHeight = GetSystemMetrics(SM_CYSMSIZE);
    return buttonWidth > 0 && buttonHeight > 0 &&
           width >= buttonWidth && width <= buttonWidth * 5 &&
           height >= buttonHeight / 2 && height <= buttonHeight * 2 &&
           window.left >= frame.left && window.right <= frame.right &&
           window.right >= frame.left + (frame.right - frame.left) * 2 / 3 &&
           window.top >= frame.top &&
           window.top <= frame.top + GetSystemMetrics(SM_CYSIZE) * 3;
}

bool IsMmcMdiButtonPaint(HDC dc, LPCRECT bounds) {
    HWND hWnd = dc ? WindowFromDC(dc) : nullptr;
    RECT client = {};
    return bounds && IsMmcMdiButtonStrip(hWnd) && GetClientRect(hWnd, &client) &&
           bounds->left >= client.left && bounds->top >= client.top &&
           bounds->right <= client.right && bounds->bottom <= client.bottom;
}

// Draw only the face and glyph; native MMC still handles every button action.
bool DrawMmcMdiButton(HDC dc, int part, int state, LPCRECT bounds, LPCRECT clip) {
    if (!dc || !bounds || (part != WP_MDIMINBUTTON && part != WP_MAXBUTTON &&
                           part != WP_MDICLOSEBUTTON && part != WP_MDIRESTOREBUTTON) ||
        state < 1 || state > 4) {
        return false;
    }
    int width = bounds->right - bounds->left;
    int height = bounds->bottom - bounds->top;
    if (width < 9 || height < 9 || width > 128 || height > 128) {
        return false;
    }
    int saved = SaveDC(dc);
    if (!saved) return false;
    if (IntersectClipRect(dc, bounds->left, bounds->top, bounds->right,
                          bounds->bottom) == ERROR ||
        (clip && IntersectClipRect(dc, clip->left, clip->top, clip->right,
                                   clip->bottom) == ERROR)) {
        RestoreDC(dc, saved);
        return false;
    }
    COLORREF face = RGB(45, 45, 45);
    if (state == 2) face = part == WP_MDICLOSEBUTTON ? RGB(196, 43, 28) : kColorHover;
    if (state == 3) face = part == WP_MDICLOSEBUTTON ? RGB(145, 32, 23) : RGB(76, 78, 84);
    COLORREF edge = part == WP_MDICLOSEBUTTON && (state == 2 || state == 3)
                        ? face : RGB(75, 75, 75);
    COLORREF ink = state == 4 ? RGB(130, 130, 130) : kColorText;
    HBRUSH brush = reinterpret_cast<HBRUSH>(GetStockObject(DC_BRUSH));
    HPEN pen = CreatePen(PS_SOLID, height >= 32 ? 2 : 1, ink);
    if (!brush || !pen || SetDCBrushColor(dc, face) == CLR_INVALID) {
        RestoreDC(dc, saved);
        if (pen) DeleteObject(pen);
        return false;
    }
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    bool ok = oldPen && oldPen != HGDI_ERROR && oldBrush && oldBrush != HGDI_ERROR &&
              FillRect(dc, bounds, brush);
    if (ok) {
        ok = SetDCBrushColor(dc, edge) != CLR_INVALID && FrameRect(dc, bounds, brush);
    }
    int side = (width < height ? width : height) * 3 / 5;
    if (side < 5) side = 5;
    int x = bounds->left + (width - side) / 2;
    int y = bounds->top + (height - side) / 2;
    auto line = [&](int x1, int y1, int x2, int y2) {
        return MoveToEx(dc, x1, y1, nullptr) && LineTo(dc, x2, y2);
    };
    if (ok) {
        if (part == WP_MDIMINBUTTON) {
            ok = line(x, y + side - 1, x + side, y + side - 1);
        } else if (part == WP_MDICLOSEBUTTON) {
            ok = line(x, y, x + side, y + side) &&
                 line(x + side - 1, y, x - 1, y + side);
        } else if (part == WP_MDIRESTOREBUTTON) {
            int shift = side / 3;
            if (shift < 1) shift = 1;
            ok = line(x + shift, y, x + side, y) &&
                 line(x + side - 1, y, x + side - 1, y + side - shift) &&
                 Rectangle(dc, x, y + shift, x + side - shift, y + side);
        } else {
            ok = Rectangle(dc, x, y, x + side, y + side);
        }
    }
    RestoreDC(dc, saved);
    DeleteObject(pen);
    return ok;
}

// Replace only the scoped Scheduler button face; retain its native text and input.
using DrawThemeBackground_t = decltype(&DrawThemeBackground);
DrawThemeBackground_t pDrawThemeBackground = nullptr;
HRESULT WINAPI DrawThemeBackground_Hook(HTHEME theme, HDC dc, int part, int state,
                                        LPCRECT bounds, LPCRECT clip) {
    RECT client = {};
    HMODULE caller = nullptr;
    if (g_active && dc && bounds &&
        (part == WP_MDIMINBUTTON || part == WP_MAXBUTTON ||
         part == WP_MDICLOSEBUTTON || part == WP_MDIRESTOREBUTTON) &&
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(__builtin_return_address(0)),
                           &caller) && caller == GetModuleHandleW(nullptr) &&
        IsMmcMdiButtonPaint(dc, bounds) &&
        DrawMmcMdiButton(dc, part, state, bounds, clip)) {
        return S_OK;
    }
    if (g_active && g_drawingSummaryButton && dc && bounds &&
        part == BP_PUSHBUTTON && state >= PBS_NORMAL && state <= PBS_DEFAULTED &&
        GetClientRect(g_drawingSummaryButton, &client) &&
        bounds->right - bounds->left == client.right - client.left &&
        bounds->bottom - bounds->top == client.bottom - client.top) {
        HBRUSH fill = state == PBS_DISABLED ? g_backgroundBrush : g_tabSelectedBrush;
        if (state == PBS_HOT || state == PBS_PRESSED) {
            fill = g_hoverBrush;
        }
        HBRUSH edge = g_lineBrush;
        int saved = SaveDC(dc);
        if (saved && fill && edge) {
            if (clip) {
                IntersectClipRect(dc, clip->left, clip->top, clip->right, clip->bottom);
            }
            BOOL filled = FillRect(dc, bounds, fill);
            BOOL framed = FrameRect(dc, bounds, edge);
            RestoreDC(dc, saved);
            if (filled && framed) {
                return S_OK;
            }
        } else if (saved) {
            RestoreDC(dc, saved);
        }
    }
    return pDrawThemeBackground ? pDrawThemeBackground(theme, dc, part, state, bounds, clip)
                                : E_FAIL;
}

// Adjust only the scoped Scheduler summary title foreground.
using DrawTextExW_t = decltype(&DrawTextExW);
DrawTextExW_t pDrawTextExW = nullptr;

int WINAPI DrawTextExW_Hook(HDC dc, LPWSTR text, int count, LPRECT rect,
                            UINT format, LPDRAWTEXTPARAMS params) {
    if (!pDrawTextExW) {
        return 0;
    }
    if (g_active && g_drawingSummaryTitle && dc && !(format & DT_CALCRECT) &&
        GetTextColor(dc) == kColorBackground) {
        COLORREF previous = SetTextColor(dc, kColorText);
        if (previous != CLR_INVALID) {
            int result = pDrawTextExW(dc, text, count, rect, format, params);
            SetTextColor(dc, previous);
            return result;
        }
    }
    return pDrawTextExW(dc, text, count, rect, format, params);
}

bool IsActionsCaptionColour(int index) {
    return index == kColActiveCaption || index == kColCaptionText ||
           index == kColGradientActiveCaption;
}

DWORD WINAPI GetSysColor_Hook(int nIndex) {
    // The native description uses COLOR_WINDOW for text on a dark background.
    if (g_active && g_drawingMmcDescription && nIndex == kColWindow) {
        return kColorText;
    }
    if (g_active && g_drawingActionsContent && IsActionsCaptionColour(nIndex) &&
        IsDirectUiCaller(__builtin_return_address(0))) {
        return nIndex == kColCaptionText ? kColorText : kColorBackground;
    }
    if (g_active && (nIndex == kColActiveCaption || nIndex == kColCaptionText) &&
        IsExtendedViewColourCaller(__builtin_return_address(0))) {
        return nIndex == kColCaptionText ? kColorText : kColorBackground;
    }
    COLORREF mapped = g_active ? MapSystemColour(nIndex) : 0xffffffff;
    if (mapped != 0xffffffff) {
        return mapped;
    }
    return pGetSysColor(nIndex);
}

using GetSysColorBrush_t = HBRUSH(WINAPI*)(int);
GetSysColorBrush_t pGetSysColorBrush = nullptr;

HBRUSH WINAPI GetSysColorBrush_Hook(int nIndex) {
    if (g_active && g_drawingActionsContent && IsActionsCaptionColour(nIndex) &&
        IsDirectUiCaller(__builtin_return_address(0))) {
        HBRUSH brush = g_sysBrushes[nIndex == kColCaptionText ? kColWindowText : kColWindow];
        if (brush) {
            return brush;
        }
    }
    if (g_active && (nIndex == kColActiveCaption || nIndex == kColCaptionText) &&
        IsExtendedViewColourCaller(__builtin_return_address(0))) {
        HBRUSH brush = g_sysBrushes[nIndex == kColCaptionText ? kColWindowText : kColWindow];
        if (brush) {
            return brush;
        }
    }
    if (g_active && nIndex >= 0 && nIndex < kSysColourCount && g_sysBrushes[nIndex]) {
        return g_sysBrushes[nIndex];
    }
    return pGetSysColorBrush(nIndex);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pCreateWindowExW = nullptr;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, PCWSTR lpClassName,
                                 PCWSTR lpWindowName, DWORD dwStyle, int x,
                                 int y, int nWidth, int nHeight, HWND hWndParent,
                                 HMENU hMenu, HINSTANCE hInstance,
                                 void* lpParam) {
    HWND hWnd = pCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                 x, y, nWidth, nHeight, hWndParent, hMenu,
                                 hInstance, lpParam);
    if (hWnd && g_active) {
        ApplyToWindow(hWnd);
        // Handle only windows owned by the current thread.
        const DWORD threadId = GetCurrentThreadId();
        if (ThreadIsPending(threadId)) {
            ClearPendingThread(threadId);
            WalkThreadWindows(threadId);
        }
    }
    return hWnd;
}

using CreateDialogParamW_t = decltype(&CreateDialogParamW);
CreateDialogParamW_t pCreateDialogParamW = nullptr;

HWND WINAPI CreateDialogParamW_Hook(HINSTANCE hInstance, LPCWSTR lpTemplateName,
                                    HWND hWndParent, DLGPROC lpDialogFunc,
                                    LPARAM dwInitParam) {
    HWND hWnd = pCreateDialogParamW(hInstance, lpTemplateName, hWndParent,
                                    lpDialogFunc, dwInitParam);
    if (hWnd && g_active) {
        ThemeDialog(hWnd);
    }
    return hWnd;
}

using CreateDialogIndirectParamW_t = decltype(&CreateDialogIndirectParamW);
CreateDialogIndirectParamW_t pCreateDialogIndirectParamW = nullptr;

HWND WINAPI CreateDialogIndirectParamW_Hook(HINSTANCE hInstance,
                                            LPCDLGTEMPLATEW lpTemplate,
                                            HWND hWndParent,
                                            DLGPROC lpDialogFunc,
                                            LPARAM dwInitParam) {
    HWND hWnd = pCreateDialogIndirectParamW(hInstance, lpTemplate, hWndParent,
                                            lpDialogFunc, dwInitParam);
    if (hWnd && g_active) {
        ThemeDialog(hWnd);
    }
    return hWnd;
}

BOOL CALLBACK SendSysColorChange(HWND hWnd, LPARAM);

BOOL CALLBACK WalkTopSysColor(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }
    SendSysColorChange(hWnd, 0);
    EnumChildWindows(hWnd, SendSysColorChange, 0);
    int* count = reinterpret_cast<int*>(lParam);
    if (count) {
        (*count)++;
    }
    return TRUE;
}

BOOL CALLBACK SendSysColorChange(HWND hWnd, LPARAM) {
    DWORD_PTR ignored = 0;
    SendMessageTimeoutW(hWnd, WM_SYSCOLORCHANGE, 0, 0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &ignored);
    return TRUE;
}

// Notify controls of system-colour changes after theme transitions.
void BroadcastSysColorChange() {
    int counted = 0;
    EnumWindows(WalkTopSysColor, reinterpret_cast<LPARAM>(&counted));
    Wh_Log(L"asked %d top level windows for their colours again", counted);
}

// Queue a native Disk header repaint after colour notifications.
BOOL CALLBACK QueueDiskHeaderChildRepaint(HWND hWnd, LPARAM) {
    if (IsDiskManagementHeader(hWnd) && IsWindowVisible(hWnd)) {
        RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE);
    }
    return TRUE;
}
BOOL CALLBACK QueueDiskHeaderTopRepaint(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        EnumChildWindows(hWnd, QueueDiskHeaderChildRepaint, 0);
    }
    return TRUE;
}
void QueueDiskHeaderRepaint() {
    EnumWindows(QueueDiskHeaderTopRepaint, 0);
}

// Queue a hosted WinForms repaint after colour notifications; the managed notification may arrive later.
BOOL CALLBACK FindHostedFormsRepaintChild(HWND hWnd, LPARAM param) {
    if (!IsHostedWinFormsList(hWnd)) {
        return TRUE;
    }
    HWND parent = GetParent(hWnd);
    WCHAR name[128] = {};
    while (parent && GetClassNameW(parent, name, ARRAYSIZE(name)) &&
           wcsncmp(name, L"WindowsForms10.Window.", 22) == 0) {
        parent = GetParent(parent);
    }
    if (parent) {
        auto* hosts = reinterpret_cast<std::unordered_set<HWND>*>(param);
        hosts->insert(parent);
    }
    return TRUE;
}

BOOL CALLBACK FindHostedFormsRepaintTop(HWND hWnd, LPARAM param) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        EnumChildWindows(hWnd, FindHostedFormsRepaintChild, param);
    }
    return TRUE;
}

void QueueHostedFormsRepaint() {
    std::unordered_set<HWND> hosts;
    EnumWindows(FindHostedFormsRepaintTop, reinterpret_cast<LPARAM>(&hosts));
    for (HWND hWnd : hosts) {
        if (IsWindow(hWnd)) {
            RedrawWindow(hWnd, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
        }
    }
}

// Queue a repaint of the exact MMC view to refresh narrow pane gaps.
BOOL CALLBACK QueueMmcViewChildRepaint(HWND hWnd, LPARAM) {
    WCHAR name[64] = {};
    if (!GetClassNameW(hWnd, name, ARRAYSIZE(name)) ||
        wcscmp(name, L"MMCViewWindow") != 0) {
        return TRUE;
    }
    HWND childFrame = GetParent(hWnd);
    HWND mdi = childFrame ? GetParent(childFrame) : nullptr;
    HWND frame = mdi ? GetParent(mdi) : nullptr;
    WCHAR childName[64] = {}, mdiName[64] = {}, frameName[64] = {};
    if (!childFrame || !mdi || !frame ||
        !GetClassNameW(childFrame, childName, ARRAYSIZE(childName)) ||
        !GetClassNameW(mdi, mdiName, ARRAYSIZE(mdiName)) ||
        !GetClassNameW(frame, frameName, ARRAYSIZE(frameName)) ||
        wcscmp(childName, L"MMCChildFrm") != 0 ||
        wcscmp(mdiName, L"MDIClient") != 0 ||
        wcscmp(frameName, L"MMCMainFrame") != 0) {
        return TRUE;
    }
    RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE);
    return TRUE;
}

BOOL CALLBACK QueueMmcViewTopRepaint(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        EnumChildWindows(hWnd, QueueMmcViewChildRepaint, 0);
    }
    return TRUE;
}

void QueueMmcViewRepaint() {
    EnumWindows(QueueMmcViewTopRepaint, 0);
}

// Subclass reference data identifies the handler; WM_NCDESTROY removes bookkeeping.
LRESULT CALLBACK ModSubclassProc(HWND hWnd, UINT message, WPARAM wParam,
                                 LPARAM lParam, DWORD_PTR refData) {
    const WindowKind kind = static_cast<WindowKind>(refData);

    if (message == WM_NCDESTROY) {
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
        ForgetSubclass(hWnd);
        switch (kind) {
        case WindowKind::Toolbar: {
            HotEntry* entry = HotFor(hWnd, false);
            if (entry) {
                *entry = {};
            }
            break;
        }
        case WindowKind::StatusBar: {
            for (int i = 0; i < 2; i++) {
                if (g_statusCache[i].hWnd == hWnd) {
                    g_statusCache[i] = {};
                }
            }
            break;
        }
        case WindowKind::Tab: {
            TabEntry* entry = TabEntryFor(hWnd, false);
            if (entry) {
                *entry = {};
            }
            break;
        }
        case WindowKind::GroupBox:
            ForgetGroupFont(hWnd);
            break;
        default:
            break;
        }
        return result;
    }

    if (!g_active) {
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }

    // Let the native combo handler run before adjusting the scoped colour response.
    if ((kind == WindowKind::WinFormsCombo ||
         kind == WindowKind::WinFormsComboParent) &&
        (message == WM_CTLCOLORSTATIC || message == WM_CTLCOLOREDIT ||
         message == WM_CTLCOLORLISTBOX) && wParam && lParam &&
        g_backgroundBrush) {
        HWND part = reinterpret_cast<HWND>(lParam);
        HWND combo = kind == WindowKind::WinFormsCombo ? hWnd : part;
        if (kind == WindowKind::WinFormsComboParent &&
            !IsMmcSummaryCombo(combo)) {
            combo = GetParent(part);
        }
        if (IsMmcSummaryComboPart(combo, part)) {
            DefSubclassProc(hWnd, message, wParam, lParam);
            HDC dc = reinterpret_cast<HDC>(wParam);
            SetTextColor(dc, IsWindowEnabled(combo) ? kColorText : RGB(140, 140, 140));
            SetBkColor(dc, kColorBackground);
            return reinterpret_cast<LRESULT>(g_backgroundBrush);
        }
    }

    // Keep native owner drawing and scope the description text colour adjustment.
    if (message == WM_DRAWITEM && lParam) {
        const DRAWITEMSTRUCT* item = reinterpret_cast<const DRAWITEMSTRUCT*>(lParam);
        WCHAR hostClass[64] = {}, itemClass[64] = {};
        if (item->CtlType == ODT_STATIC && item->hwndItem &&
            GetParent(item->hwndItem) == hWnd &&
            GetClassNameW(hWnd, hostClass, ARRAYSIZE(hostClass)) &&
            wcscmp(hostClass, L"MMCViewWindow") == 0 &&
            GetClassNameW(item->hwndItem, itemClass, ARRAYSIZE(itemClass)) &&
            wcscmp(itemClass, L"Static") == 0 &&
            (GetWindowLongPtrW(item->hwndItem, GWL_STYLE) & SS_TYPEMASK) == SS_OWNERDRAW) {
            const bool previous = g_drawingMmcDescription;
            g_drawingMmcDescription = true;
            LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
            g_drawingMmcDescription = previous;
            return result;
        }
    }

    // Scope DrawEdge to the Actions caption erase.
    if (message == WM_ERASEBKGND && IsMmcActionsPane(hWnd)) {
        HDC previous = g_actionsEraseDc;
        g_actionsEraseDc = reinterpret_cast<HDC>(wParam);
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
        g_actionsEraseDc = previous;
        return result;
    }

    if (kind == WindowKind::WinFormsRefresh &&
        (message == WM_PAINT || message == WM_PRINTCLIENT) &&
        IsMmcSummaryRefresh(hWnd)) {
        HWND previous = g_drawingSummaryButton;
        g_drawingSummaryButton = hWnd;
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
        g_drawingSummaryButton = previous;
        return result;
    }

    if (kind == WindowKind::WinFormsLabel &&
        (message == WM_PAINT || message == WM_PRINTCLIENT) &&
        IsMmcSummaryTitle(hWnd)) {
        const bool previous = g_drawingSummaryTitle;
        g_drawingSummaryTitle = true;
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
        g_drawingSummaryTitle = previous;
        return result;
    }

    if (kind == WindowKind::ActionsContent &&
        (message == WM_PAINT || message == WM_PRINTCLIENT || message == WM_ERASEBKGND)) {
        const bool previous = g_drawingActionsContent;
        g_drawingActionsContent = true;
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
        g_drawingActionsContent = previous;
        return result;
    }

    if (message == OwnThreadSetupMessage()) {
        return HandleOwnThreadSetup(hWnd, kind);
    }

    switch (kind) {
    case WindowKind::Backdrop:
        return HandleBackdrop(hWnd, message, wParam, lParam);
    case WindowKind::Strip:
        return HandleStrip(hWnd, message, wParam, lParam);
    case WindowKind::StatusBar:
        return HandleStatusBar(hWnd, message, wParam, lParam);
    case WindowKind::Toolbar:
        return HandleToolbar(hWnd, message, wParam, lParam);
    case WindowKind::NoBorder:
        return HandleNoBorder(hWnd, message, wParam, lParam);
    case WindowKind::Tab:
        return HandleTab(hWnd, message, wParam, lParam);
    case WindowKind::Field:
        return HandleField(hWnd, message, wParam, lParam);
    case WindowKind::GroupBox:
        return HandleGroupBox(hWnd, message, wParam, lParam);
    case WindowKind::Dialog:
        return HandleDialog(hWnd, message, wParam, lParam);
    case WindowKind::MmcCustomTab:
        return HandleMmcCustomTab(hWnd, message, wParam, lParam);
    case WindowKind::ActionsPane:
        return DefSubclassProc(hWnd, message, wParam, lParam);
    case WindowKind::TreeFrame:
        return HandleTreeFrame(hWnd, message, wParam, lParam);
    case WindowKind::DescriptionHost:
    case WindowKind::ActionsContent:
    case WindowKind::WinFormsLabel:
    case WindowKind::WinFormsCombo:
    case WindowKind::WinFormsComboParent:
    case WindowKind::WinFormsRefresh:
        return DefSubclassProc(hWnd, message, wParam, lParam);
    case WindowKind::DiskHeader:
        return HandleDiskHeader(hWnd, message, wParam, lParam);
    }
    return DefSubclassProc(hWnd, message, wParam, lParam);
}

void LoadSettings() {
    g_settings.darkTitleBar = Wh_GetIntSetting(L"darkTitleBar") != 0;
    g_settings.roundCorners = Wh_GetIntSetting(L"roundCorners") != 0;
    g_settings.darkStatusBar = Wh_GetIntSetting(L"darkStatusBar") != 0;
}

// Hook installation is typed; inactive hooks pass through.
#if defined(_WIN64)
// Scope Actions rendering by its DirectUI element and verified root window.
using DuiElementPaint_t = void (*)(void*, HDC, const RECT*, const RECT*, RECT*, RECT*);
using DuiElementGetRoot_t = void* (*)(void*);
using DuiElementGetParent_t = void* (*)(void*);
using DuiElementGetId_t = unsigned short (*)(void*);
using DuiHwndElementGetHwnd_t = HWND (*)(void*);
using ActionsGradient_t = decltype(&GdiGradientFill);
using ActionsDrawText_t = decltype(&DrawTextW);
using ActionsExtText_t = decltype(&ExtTextOutW);
DuiElementPaint_t pDuiElementPaint = nullptr;
DuiElementGetRoot_t pDuiElementGetRoot = nullptr;
DuiElementGetParent_t pDuiElementGetParent = nullptr;
DuiElementGetId_t pDuiElementGetId = nullptr;
DuiHwndElementGetHwnd_t pDuiHwndElementGetHwnd = nullptr;
void* pDuiHwndElementClassInfoW = nullptr;
ActionsGradient_t pActionsGradient = nullptr;
ActionsDrawText_t pActionsDrawText = nullptr;
ActionsExtText_t pActionsExtText = nullptr;
std::atomic<bool> g_actionsElementHooksReady{false};
std::atomic<bool> g_actionsInstallAttempted{false};
std::mutex g_actionsInstallMutex;
bool g_actionsStopping = false;  // guarded by g_actionsInstallMutex
bool g_actionsGdiHooksReady = false;  // set once before Windhawk enables hooks
HMODULE g_actionsDuiReference = nullptr;  // released after Windhawk removes hooks
struct ActionsElementFrame { HDC dc; bool headerText; bool gradientReplaced; };
thread_local ActionsElementFrame* g_actionsElementFrame = nullptr;
struct ActionsFrameRestore {
    ActionsElementFrame* previous;
    ~ActionsFrameRestore() { g_actionsElementFrame = previous; }
};

bool IsMmcActionsContentWindow(HWND window) {
    WCHAR cls[64] = {}, parentCls[64] = {};
    HWND parent = GetParent(window);
    return window && parent && GetClassNameW(window, cls, ARRAYSIZE(cls)) &&
           wcscmp(cls, L"DirectUIHWND") == 0 &&
           GetClassNameW(parent, parentCls, ARRAYSIZE(parentCls)) &&
           wcscmp(parentCls, L"NativeHWNDHost") == 0 &&
           IsMmcActionsPane(GetParent(parent));
}

// Verify the exact HWNDElement class-info slot before calling its HWND method.
bool IsActionsDuiElement(void* element) {
    if (!element || !pDuiElementGetRoot || !pDuiHwndElementGetHwnd ||
        !pDuiHwndElementClassInfoW) return false;
    void* root = pDuiElementGetRoot(element);
    if (!root) return false;
    void** table = *static_cast<void***>(root);
    if (!table) return false;
    const void* const* slot = table + 35;
    MEMORY_BASIC_INFORMATION mbi = {};
    if (!VirtualQuery(slot, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT ||
        (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))) return false;
    const DWORD access = mbi.Protect & 0xff;
    if (access != PAGE_READONLY && access != PAGE_READWRITE &&
        access != PAGE_WRITECOPY && access != PAGE_EXECUTE_READ &&
        access != PAGE_EXECUTE_READWRITE && access != PAGE_EXECUTE_WRITECOPY)
        return false;
    const auto regionBegin = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
    const auto address = reinterpret_cast<uintptr_t>(slot);
    if (address < regionBegin || mbi.RegionSize < sizeof(void*) ||
        address - regionBegin > mbi.RegionSize - sizeof(void*) ||
        *slot != pDuiHwndElementClassInfoW) return false;
    return IsMmcActionsContentWindow(pDuiHwndElementGetHwnd(root));
}

// Do not paint a new dark Actions caption when high contrast is active or cannot be queried.
bool ActionsColoursAllowedNow() {
    HIGHCONTRASTW hc = {};
    hc.cbSize = sizeof(hc);
    return SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) &&
           !(hc.dwFlags & HCF_HIGHCONTRASTON);
}

void DuiElementPaint_Hook(void* element, HDC dc, const RECT* r1, const RECT* r2,
                          RECT* r3, RECT* r4) {
    ActionsElementFrame frame = {};
    ActionsFrameRestore restore{g_actionsElementFrame};
    // Do not let nested non-header elements inherit a header paint scope.
    g_actionsElementFrame = nullptr;
    if (g_active && g_actionsElementHooksReady && element && dc &&
        pDuiElementGetId && pDuiElementGetParent) {
        const ATOM header = FindAtomW(L"header");
        const ATOM title = FindAtomW(L"title");
        const ATOM arrow = FindAtomW(L"arrow");
        const auto id = pDuiElementGetId(element);
        bool candidate = header && id == header;
        bool headerText = false;
        if (!candidate && header && ((title && id == title) || (arrow && id == arrow))) {
            void* parent = pDuiElementGetParent(element);
            candidate = parent && pDuiElementGetId(parent) == header;
            headerText = candidate;
        }
        if (candidate && IsActionsDuiElement(element) && ActionsColoursAllowedNow()) {
            frame = {dc, headerText, false};
            g_actionsElementFrame = &frame;
        }
    }
    pDuiElementPaint(element, dc, r1, r2, r3, r4);
}

bool MatchesActionsVertex(const TRIVERTEX& v, COLORREF c) {
    return (v.Red >> 8) == GetRValue(c) && (v.Green >> 8) == GetGValue(c) &&
           (v.Blue >> 8) == GetBValue(c);
}

BOOL WINAPI ActionsGradient_Hook(HDC dc, PTRIVERTEX vertices, ULONG count,
                                 PVOID mesh, ULONG meshes, ULONG mode) {
    if (pActionsGradient && g_active && g_actionsElementHooksReady &&
        g_actionsElementFrame && dc && dc == g_actionsElementFrame->dc && vertices &&
        count == 4 && mesh && (meshes == 1 || meshes == 2) && pGetSysColor &&
        (mode == GRADIENT_FILL_RECT_H || mode == GRADIENT_FILL_RECT_V) &&
        IsDirectUiCaller(__builtin_return_address(0))) {
        const COLORREF first = pGetSysColor(kColGradientActiveCaption);
        const COLORREF last = pGetSysColor(kColActiveCaption);
        const COLORREF middle = RGB((GetRValue(first) + GetRValue(last)) / 2,
                                    (GetGValue(first) + GetGValue(last)) / 2,
                                    (GetBValue(first) + GetBValue(last)) / 2);
        bool used[4] = {}, match = first != last, sawFirst = false, sawLast = false;
        const auto* rectangles = static_cast<const GRADIENT_RECT*>(mesh);
        for (ULONG i = 0; i < meshes; ++i) {
            if (rectangles[i].UpperLeft >= count || rectangles[i].LowerRight >= count) {
                match = false; break;
            }
            used[rectangles[i].UpperLeft] = used[rectangles[i].LowerRight] = true;
        }
        TRIVERTEX copy[4] = {};
        for (unsigned i = 0; i < 4; ++i) {
            if (!used[i]) continue;
            copy[i] = vertices[i];
            const bool isFirst = MatchesActionsVertex(vertices[i], first);
            const bool isLast = MatchesActionsVertex(vertices[i], last);
            const bool isMid = MatchesActionsVertex(vertices[i], middle);
            sawFirst |= isFirst; sawLast |= isLast;
            match &= isFirst || isLast || isMid;
            const COLOR16 shade = static_cast<COLOR16>((isFirst ? 46 : isLast ? 38 : 42) << 8);
            copy[i].Red = copy[i].Green = copy[i].Blue = shade;
        }
        if (match && sawFirst && sawLast &&
            pActionsGradient(dc, copy, count, mesh, meshes, mode)) {
            g_actionsElementFrame->gradientReplaced = true;
            return TRUE;
        }
    }
    return pActionsGradient ? pActionsGradient(dc, vertices, count, mesh, meshes, mode) : FALSE;
}

bool IsDarkActionsHeaderText(HDC dc) {
    return g_active && g_actionsElementHooksReady && g_actionsElementFrame && dc &&
           dc == g_actionsElementFrame->dc && g_actionsElementFrame->headerText &&
           g_actionsElementFrame->gradientReplaced && pGetSysColor &&
           (GetTextColor(dc) & 0x00ffffffu) == (pGetSysColor(kColCaptionText) & 0x00ffffffu);
}

int WINAPI ActionsDrawText_Hook(HDC dc, LPCWSTR text, int length, LPRECT rect, UINT format) {
    if (!pActionsDrawText) return 0;
    if (rect && !(format & DT_CALCRECT) && IsDarkActionsHeaderText(dc) &&
        IsDirectUiCaller(__builtin_return_address(0))) {
        COLORREF old = SetTextColor(dc, kColorText);
        if (old != CLR_INVALID) {
            int result = pActionsDrawText(dc, text, length, rect, format);
            SetTextColor(dc, old); return result;
        }
    }
    return pActionsDrawText(dc, text, length, rect, format);
}

// Match the selected-region fill by a verified x64 dmview instruction sequence; unmatched builds pass through.
bool IsKnownDiskSelectedFillCaller(void* returnAddress, HMODULE dmview) {
    static const unsigned char signature[] = {
        0x80,0x7b,0x39,0x00,0x75,0x06,0x80,0x7b,0x3a,0x00,0x74,0x10,
        0x40,0x80,0xfe,0x34,0x76,0x08,0x81,0xc6,0xcc,0xcb,0xcb,0xff,
        0xeb,0x02,0x33,0xf6,0x44,0x8b,0xc6,0x48,0x8d,0x55,0xe0,0x48,
        0x8b,0xcf,0x48,0xff,0x15,0x5b,0x83,0x00,0x00
    };
    auto address = reinterpret_cast<uintptr_t>(returnAddress);
    if (!dmview || address < sizeof(signature)) return false;
    const auto start = address - sizeof(signature);
    MEMORY_BASIC_INFORMATION mbi = {};
    if (!VirtualQuery(reinterpret_cast<const void*>(start), &mbi, sizeof(mbi)) ||
        mbi.State != MEM_COMMIT || mbi.Type != MEM_IMAGE || mbi.AllocationBase != dmview ||
        (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))) return false;
    const DWORD protection = mbi.Protect & 0xff;
    if (protection != PAGE_EXECUTE_READ && protection != PAGE_EXECUTE_READWRITE &&
        protection != PAGE_EXECUTE_WRITECOPY && protection != PAGE_READONLY &&
        protection != PAGE_READWRITE && protection != PAGE_WRITECOPY) return false;
    const auto base = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
    if (start < base || mbi.RegionSize < sizeof(signature) ||
        start - base > mbi.RegionSize - sizeof(signature)) return false;
    return memcmp(reinterpret_cast<const void*>(start), signature, sizeof(signature)) == 0;
}

bool IsDiskSelectedFillOnStack() {
    HMODULE dmview = GetModuleHandleW(L"dmview.ocx");
    if (!dmview) return false;
    void* frames[12] = {};
    USHORT count = CaptureStackBackTrace(0, ARRAYSIZE(frames), frames, nullptr);
    for (USHORT i = 0; i < count; ++i) {
        if (IsKnownDiskSelectedFillCaller(frames[i], dmview)) return true;
    }
    return false;
}

BOOL WINAPI ActionsExtText_Hook(HDC dc, int x, int y, UINT options, const RECT* rect,
                                LPCWSTR text, UINT count, const INT* dx) {
    if (!pActionsExtText) return FALSE;
    // Restore the DC background after the scoped selected-frame fill.
    if (g_active && dc && rect && !text && count == 0 &&
        options == ETO_OPAQUE && GetBkColor(dc) == RGB(0, 0, 0) &&
        IsDiskSelectedFillOnStack()) {
        COLORREF old = SetBkColor(dc, RGB(80, 120, 160));
        if (old != CLR_INVALID) {
            BOOL result = pActionsExtText(dc, x, y, options, rect, text, count, dx);
            SetBkColor(dc, old);
            return result;
        }
    }
    if (text && count && IsDarkActionsHeaderText(dc) &&
        IsDirectUiCaller(__builtin_return_address(0))) {
        COLORREF old = SetTextColor(dc, kColorText);
        if (old != CLR_INVALID) {
            BOOL result = pActionsExtText(dc, x, y, options, rect, text, count, dx);
            SetTextColor(dc, old); return result;
        }
    }
    return pActionsExtText(dc, x, y, options, rect, text, count, dx);
}

// Install the GDI hooks before enabling the late DirectUI element hook.
void InstallActionsGdiHooks() {
    bool gradient = WindhawkUtils::SetFunctionHook(GdiGradientFill, ActionsGradient_Hook, &pActionsGradient);
    bool text = WindhawkUtils::SetFunctionHook(DrawTextW, ActionsDrawText_Hook, &pActionsDrawText);
    bool ext = WindhawkUtils::SetFunctionHook(ExtTextOutW, ActionsExtText_Hook, &pActionsExtText);
    g_actionsGdiHooksReady = gradient && text && ext;
    if (!g_actionsGdiHooksReady)
        Wh_Log(L"Actions caption hooks unavailable; native rendering preserved");
}

// Register the DirectUI hook only for an existing host and synchronize with unload.
void TryInstallActionsElementHook(HWND actionsWindow) {
    if (!g_active || !g_actionsGdiHooksReady ||
        (g_actionsInstallAttempted.load() && g_actionsElementHooksReady.load())) return;
    bool installed = false;
    {
        std::lock_guard<std::mutex> lock(g_actionsInstallMutex);
        if (g_actionsStopping) return;
        if (g_actionsInstallAttempted.load()) {
            installed = g_actionsElementHooksReady.load();
        } else {
            HMODULE library = nullptr;
            // Take a reference only to an already loaded module; retry if absent.
            if (!GetModuleHandleExW(0, L"dui70.dll", &library)) return;
            g_actionsInstallAttempted.store(true);
            auto paint = reinterpret_cast<DuiElementPaint_t>(GetProcAddress(
                library, "?Paint@Element@DirectUI@@UEAAXPEAUHDC__@@PEBUtagRECT@@1PEAU4@2@Z"));
            pDuiElementGetRoot = reinterpret_cast<DuiElementGetRoot_t>(GetProcAddress(
                library, "?GetRoot@Element@DirectUI@@QEAAPEAV12@XZ"));
            pDuiElementGetParent = reinterpret_cast<DuiElementGetParent_t>(GetProcAddress(
                library, "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ"));
            pDuiElementGetId = reinterpret_cast<DuiElementGetId_t>(GetProcAddress(
                library, "?GetID@Element@DirectUI@@QEAAGXZ"));
            pDuiHwndElementGetHwnd = reinterpret_cast<DuiHwndElementGetHwnd_t>(GetProcAddress(
                library, "?GetHWND@HWNDElement@DirectUI@@UEAAPEAUHWND__@@XZ"));
            pDuiHwndElementClassInfoW = reinterpret_cast<void*>(GetProcAddress(
                library, "?GetClassInfoW@HWNDElement@DirectUI@@UEAAPEAUIClassInfo@2@XZ"));
            if (!paint || !pDuiElementGetRoot || !pDuiElementGetParent ||
                !pDuiElementGetId || !pDuiHwndElementGetHwnd || !pDuiHwndElementClassInfoW) {
                FreeLibrary(library);
                Wh_Log(L"Actions elements: DirectUI exports unavailable; native rendering preserved");
                return;
            }
            // Keep the module reference until Windhawk removes any queued hook.
            g_actionsDuiReference = library;
            if (WindhawkUtils::SetFunctionHook(paint, DuiElementPaint_Hook, &pDuiElementPaint) &&
                Wh_ApplyHookOperations()) {
                g_actionsElementHooksReady.store(true);
                installed = true;
            } else {
                Wh_Log(L"Actions elements: hook unavailable; native rendering preserved");
            }
        }
    }
    // Do not send messages or redraw while holding the installation lock.
    if (installed) InvalidateRect(actionsWindow, nullptr, TRUE);
}
#endif

void InstallHooks() {
    HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
    auto drawThemeBackground = uxtheme ? reinterpret_cast<DrawThemeBackground_t>(
        GetProcAddress(uxtheme, "DrawThemeBackground")) : nullptr;
    if (!drawThemeBackground || !WindhawkUtils::SetFunctionHook(
            drawThemeBackground, DrawThemeBackground_Hook, &pDrawThemeBackground)) {
        Wh_Log(L"could not hook DrawThemeBackground; Task Scheduler button keeps its original face");
    }
    if (!WindhawkUtils::SetFunctionHook(DrawTextExW, DrawTextExW_Hook,
                                        &pDrawTextExW)) {
        Wh_Log(L"could not hook DrawTextExW; the Task Scheduler summary title may remain dim");
    }
    if (!WindhawkUtils::SetFunctionHook(DrawFrameControl, DrawFrameControl_Hook,
                                        &pDrawFrameControl)) {
        Wh_Log(L"could not hook DrawFrameControl; Disk Management keeps its original splitter");
    }
    if (!WindhawkUtils::SetFunctionHook(CreateHatchBrush, CreateHatchBrush_Hook,
                                        &pCreateHatchBrush)) {
        Wh_Log(L"could not hook CreateHatchBrush; Disk Management keeps its original partition colours");
    }
    if (!WindhawkUtils::SetFunctionHook(DrawEdge, DrawEdge_Hook, &pDrawEdge)) {
        Wh_Log(L"could not hook DrawEdge; Actions captions and Disk header edges keep their native appearance");
    }
    if (pSetWindowTheme) {
        if (!WindhawkUtils::SetFunctionHook(pSetWindowTheme, SetWindowTheme_Hook,
                                            &pSetWindowThemeHook)) {
            Wh_Log(L"could not hook SetWindowTheme");
        } else {
            pSetWindowTheme = pSetWindowThemeHook;
        }
    }

    if (pGetThemeColor) {
        if (!WindhawkUtils::SetFunctionHook(pGetThemeColor, GetThemeColor_Hook,
                                            &pGetThemeColorHook)) {
            Wh_Log(L"could not hook GetThemeColor; selected list rows may stay white");
        }
    } else {
        Wh_Log(L"GetThemeColor not found; selected list rows may stay white");
    }

    if (!WindhawkUtils::SetFunctionHook(GetSysColor, GetSysColor_Hook,
                                        &pGetSysColor)) {
        Wh_Log(L"could not hook GetSysColor");
    }
    if (!WindhawkUtils::SetFunctionHook(GetSysColorBrush, GetSysColorBrush_Hook,
                                        &pGetSysColorBrush)) {
        Wh_Log(L"could not hook GetSysColorBrush");
    }
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &pCreateWindowExW)) {
        Wh_Log(L"could not hook CreateWindowExW; new windows will not be themed");
    }
    if (!WindhawkUtils::SetFunctionHook(CreateDialogParamW, CreateDialogParamW_Hook,
                                        &pCreateDialogParamW)) {
        Wh_Log(L"could not hook CreateDialogParamW");
    }
    if (!WindhawkUtils::SetFunctionHook(CreateDialogIndirectParamW,
                                        CreateDialogIndirectParamW_Hook,
                                        &pCreateDialogIndirectParamW)) {
        Wh_Log(L"could not hook CreateDialogIndirectParamW");
    }
}

void Activate() {
    if (!g_backgroundBrush) {
        g_backgroundBrush = CreateSolidBrush(kColorBackground);
    }
    for (int i = 0; i < kSysColourCount; i++) {
        COLORREF mapped = MapSystemColour(i);
        if (mapped != 0xffffffff && !g_sysBrushes[i]) {
            g_sysBrushes[i] = CreateSolidBrush(mapped);
        }
    }

    // Create shared paint brushes before concurrent GUI paints.
    if (!g_hoverBrush) {
        g_hoverBrush = CreateSolidBrush(kColorHover);
    }
    LineBrush();
    TabSelectedBrush();

    Wh_Log(L"init: brushes ready");
    pSetPreferredAppMode(PreferredAppModeForceDark);
    if (pFlushMenuThemes) {
        pFlushMenuThemes();
    }

    g_active = true;

    // Wait for Wh_ModAfterInit: Windhawk applies registered hooks after Wh_ModInit returns.
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"MMC Dark Mode 1.0");

    LoadSettings();
    Wh_Log(L"init: settings read");

    if (!ResolveApis()) {
        return FALSE;
    }

    Wh_Log(L"init: apis resolved");
    InstallHooks();
    Wh_Log(L"init: hooks installed");

    if (pShouldAppsUseDarkMode) {
        Wh_Log(L"system dark mode is %d", pShouldAppsUseDarkMode() ? 1 : 0);
    }

    if (HighContrastIsOn()) {
        Wh_Log(L"high contrast is on, doing nothing");
        return TRUE;
    }

#if defined(_WIN64)
    InstallActionsGdiHooks();
#endif
    Wh_Log(L"init: activating");
    Activate();
    Wh_Log(L"init: activated");

    Wh_Log(L"dark status bar=%d", g_settings.darkStatusBar);
    Wh_Log(L"ready");
    return TRUE;
}

// Refresh open windows after hooks are active.
void Wh_ModAfterInit() {
    if (!g_active) {
        return;
    }
    WalkEveryThread(false);
    BroadcastSysColorChange();
    QueueHostedFormsRepaint();
    QueueMmcViewRepaint();
    QueueDiskHeaderRepaint();
    Wh_Log(L"after init: windows and colours refreshed");
}

void Wh_ModBeforeUninit() {
#if defined(_WIN64)
    // Prevent new late-hook registration before unload begins.
    std::lock_guard<std::mutex> lock(g_actionsInstallMutex);
    g_actionsStopping = true;
#endif
}

void Wh_ModUninit() {
    Wh_Log(L"uninit: start");
    const bool wasActive = g_active;
    g_active = false;

    // Restore app mode before retheming windows so controls resolve native colours.
    if (pSetPreferredAppMode) {
        pSetPreferredAppMode(PreferredAppModeDefault);
    }
    if (pFlushMenuThemes) {
        pFlushMenuThemes();
    }
    Wh_Log(L"uninit: app mode restored");

    // Restore windows on their owning threads before removing subclasses.
    if (wasActive) {
        // Unload waits until window-thread work has completed.
        WalkEveryThread(true);
        BroadcastSysColorChange();
        QueueHostedFormsRepaint();
    }
    // Remove all remaining subclasses before unloading the mod.
    RemoveAllSubclasses();
    Wh_Log(L"uninit: windows restored");
#if defined(_WIN64)
    if (g_actionsDuiReference) {
        FreeLibrary(g_actionsDuiReference);
        g_actionsDuiReference = nullptr;
    }
#endif

    // Retain brushes until process exit because native controls can cache their handles.
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"settings changed: darkTitleBar=%d roundCorners=%d darkStatusBar=%d",
           g_settings.darkTitleBar, g_settings.roundCorners,
           g_settings.darkStatusBar);
    if (!g_active) {
        return;
    }
    WalkEveryThread(false);
}
