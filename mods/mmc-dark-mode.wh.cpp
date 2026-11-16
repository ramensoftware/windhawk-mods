// ==WindhawkMod==
// @id              mmc-dark-mode
// @name            MMC Dark Mode
// @description     Dark theme for the console snap-ins: Device Manager, Event Viewer, Services, Task Scheduler and Disk Management.
// @version         1.1.1
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
`mmc.exe`. Version 1.1 builds on work contributed by FadeMind.

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

Switching the mod off restores native styles, colours and painting without
altering the system-wide colour table.

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
  native drawing. Some native and WinForms push buttons have scoped dark faces
  and text. Custom images, drawing methods or unavailable hooks can affect
  their appearance; controls outside the scoped paths keep native visuals.
* The maximized child window's menu-row buttons use scoped custom drawing.
  Other layouts may keep the native appearance. Restored child-window captions
  and their buttons can remain light.

## Changelog

### 1.1.1

* Dialog check boxes and radio buttons paint their own dark glyph and caption.
* Wizard pages (including Insert ActiveX Control): page labels, divider/header
  lines and blank 49-pixel header bitmaps are drawn in the dark palette; wizard
  detection works whether or not a hidden tab control is present.
* Result-view host windows use the dark class brush, MMC's empty background
  placeholder static paints dark, `FillRect` swaps `COLOR_xxx+1` pseudo-brushes
  for the dark palette, `Shell Embedding` and `Shell DocObject View` paint empty
  areas dark, and new embedded web views stay clipped behind an empty window
  region for up to 800 ms until their page loads.
* Result list panes and Actions captions drop their white 3D frames, and the
  Actions group header drops its pale gradient-caption bottom row.
* Windows created with `CreateWindowExA` (including MMC's OCX view host) are
  themed at creation.
* Message boxes and user32 dialogs are themed through a per-thread
  `WM_INITDIALOG` watcher rather than wrapping modal calls, and unhandled
  messages pass straight through the subclass proc so open modals cannot stall
  unloading.
* Unloading waits for busy UI threads instead of skipping them, ignores
  non-UI helper windows, and restores class brushes, window styles, list
  palettes and web-view regions.

### 1.1

* Scoped handling covers read-only WinForms fields, hosted lists, and button
  faces and text. Some controls and transitions may still keep native colours.
* Edit fields in property sheets use classic painting: no light themed frame,
  no light fill for read-only fields. Editable fields get a slightly raised fill.
* Hosted WinForms separator lines (thin sunken labels) are drawn as a single
  line in the border colour instead of the system's white 3D edge.
* Task Scheduler: the General tab (the first page, which WinForms builds before
  the view is attached) is themed like the other tabs. Hosted WinForms windows
  are recognised by class name instead of by their parent chain.
* Task Scheduler: combo boxes on the Conditions and Settings tabs (including the
  editable ones such as idle time) are dark and are no longer limited to a fixed
  nesting depth under the summary pane.
* Task Scheduler: the details pane of a selected task paints its page body and
  group-box frames in the dark palette, captions use light text, and hosted
  WinForms tab strips share the dark tab painter.
* Tab strips refresh as soon as a tab is selected, added or renamed from code,
  rather than waiting for a mouse click or key press.
* Palette: all colours are named constants. Hover and pressed states are neutral
  greys, borders and captions use explicit colours, list rows distinguish hot,
  selected and inactive selection states, and accent blue marks the active tab,
  focused edit field and default push button.
* Status bar text uses the control's own font instead of the raster system font,
  and panes are separated by thin dividers.
* Toolbar buttons show a pressed state.
* Fixed the first property-sheet tab strip paint falling back to native light
  drawing.
* Fixed `Dark title bar` and `Rounded window corners` not reverting when switched
  off while the mod is running.
* Group box captions with `&` mnemonics measure accurately, disabled captions
  are dimmed, and offsets scale with window DPI.
* Removed frame styles are tracked in a map instead of a fixed 64-slot table.
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
#include <oleidl.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <windhawk_api.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <windhawk_utils.h>

// Resolve the undocumented dark-mode entry points dynamically; fail if required exports are missing.

namespace MmcFormHost {
// The host exposes IFormHost and IOleWindow. It need not expose IPersist.
static const GUID kFormHostIid = {
    0xb5f3b64d, 0xb22e, 0x4ea4, {0x9b, 0x98, 0x53, 0x01, 0x6f, 0x1a, 0x7a, 0xae}};
static const GUID kOleWindowIid = {
    0x00000114, 0, 0, {0xc0, 0, 0, 0, 0, 0, 0, 0x46}};
thread_local bool g_inProbe = false;

struct ProbeGuard {
    ProbeGuard() { g_inProbe = true; }
    ~ProbeGuard() { g_inProbe = false; }
};

static bool ClassEquals(HWND window, const wchar_t* expected) {
    wchar_t name[128] = {};
    return window && GetClassNameW(window, name, 128) && wcscmp(name, expected) == 0;
}

static bool IsCandidate(HWND window, HMODULE formModule) {
    if (!window || !formModule || !IsWindow(window) ||
        reinterpret_cast<HMODULE>(GetClassLongPtrW(window, GCLP_HMODULE)) != formModule ||
        !(GetWindowLongPtrW(window, GWL_STYLE) & WS_CLIPCHILDREN)) {
        return false;
    }
    wchar_t name[128] = {};
    if (!GetClassNameW(window, name, 128) || wcsncmp(name, L"ATL:", 4)) {
        return false;
    }
    HWND parent = GetParent(window);
    HWND ocx = parent ? GetParent(parent) : nullptr;
    HWND view = ocx ? GetParent(ocx) : nullptr;
    DWORD pid = 0;
    DWORD thread = GetWindowThreadProcessId(window, &pid);
    return thread == GetCurrentThreadId() &&
           GetWindowThreadProcessId(parent, nullptr) == thread &&
           pid == GetCurrentProcessId() && ClassEquals(parent, L"AtlAxWinEx") &&
           ClassEquals(ocx, L"MMCOCXViewWindow") && ClassEquals(view, L"MMCViewWindow");
}

// Query the hosted object only on its owning thread. ATL returns an AddRef'd
// IUnknown; both queried interfaces and the original reference are released.
static bool VerifyFormHost(HWND window, HMODULE formModule) {
    if (g_inProbe || !IsCandidate(window, formModule) || GetWindow(window, GW_CHILD)) {
        return false;
    }
    ProbeGuard guard;
    HWND parent = GetParent(window);
    const UINT message = RegisterWindowMessageW(L"WM_ATLGETCONTROL");
    DWORD_PTR result = 0;
    if (!message || !SendMessageTimeoutW(parent, message, 0, 0,
                                          SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &result)) {
        return false;
    }
    auto* unknown = reinterpret_cast<IUnknown*>(result);
    if (!unknown) {
        return false;
    }
    IUnknown* formHost = nullptr;
    IOleWindow* oleWindow = nullptr;
    HRESULT formResult = unknown->QueryInterface(kFormHostIid, reinterpret_cast<void**>(&formHost));
    HRESULT windowResult = unknown->QueryInterface(kOleWindowIid, reinterpret_cast<void**>(&oleWindow));
    HWND actual = nullptr;
    bool valid = SUCCEEDED(formResult) && formHost && SUCCEEDED(windowResult) && oleWindow &&
                 SUCCEEDED(oleWindow->GetWindow(&actual)) && actual == window;
    if (oleWindow) oleWindow->Release();
    if (formHost) formHost->Release();
    unknown->Release();
    return valid && IsCandidate(window, formModule) && GetParent(window) == parent &&
           !GetWindow(window, GW_CHILD);
}

static bool EraseEmptyFormHost(HWND window, HDC dc, HBRUSH brush, HMODULE formModule,
                               bool (*isActive)()) {
    if (!isActive || !isActive() || !dc || !brush ||
        !VerifyFormHost(window, formModule) || !isActive()) {
        return false;
    }
    RECT rect = {};
    return GetClientRect(window, &rect) && rect.right > rect.left && rect.bottom > rect.top &&
           FillRect(dc, &rect, brush) != 0;
}
}  // namespace MmcFormHost


namespace {

enum PreferredAppMode {
    PreferredAppModeDefault = 0,
    PreferredAppModeAllowDark = 1,
    PreferredAppModeForceDark = 2,
    PreferredAppModeForceLight = 3,
};

constexpr WORD kOrdinalAllowDarkModeForWindow = 133;
constexpr WORD kOrdinalSetPreferredAppMode = 135;
constexpr WORD kOrdinalFlushMenuThemes = 136;

// Try the supported immersive dark title-bar attributes in order.
constexpr DWORD kDwmwaUseImmersiveDarkMode = 20;
constexpr DWORD kDwmwaUseImmersiveDarkModeLegacy = 19;
// Windows 11 (build 22000+); older builds reject them harmlessly.
constexpr DWORD kDwmwaCaptionColor = 35;
constexpr DWORD kDwmwaTextColor = 36;

// Palette: neutral greys in the Windows 11 dark layering, with blue reserved for
// selection and the accent. Text contrast against the background is 4.8:1 or
// better for every text colour.
constexpr COLORREF kColorBackground = RGB(32, 32, 32);    // windows, dialogs, lists
constexpr COLORREF kColorSurface = RGB(43, 43, 43);       // raised: buttons, selected tab
constexpr COLORREF kColorHover = RGB(56, 56, 56);
constexpr COLORREF kColorPressed = RGB(66, 66, 66);
constexpr COLORREF kColorLine = RGB(72, 72, 72);          // borders and dividers
constexpr COLORREF kColorAccent = RGB(60, 140, 230);      // focus and "current" markers
constexpr COLORREF kColorText = RGB(240, 240, 240);
constexpr COLORREF kColorTextMuted = RGB(178, 178, 178);  // inactive tabs
constexpr COLORREF kColorTextDisabled = RGB(140, 140, 140);
constexpr COLORREF kColorSelection = RGB(38, 79, 120);
constexpr COLORREF kColorSelectionHot = RGB(46, 58, 76);
constexpr COLORREF kColorSelectionInactive = RGB(56, 62, 72);
constexpr COLORREF kColorTabText = kColorTextMuted;
constexpr COLORREF kColorTabSelected = kColorSurface;

// Window-control glyph buttons (the standard Windows 11 close-button reds).
constexpr COLORREF kColorCloseHot = RGB(196, 43, 28);
constexpr COLORREF kColorCloseDown = RGB(145, 32, 23);

// Actions pane header gradient, as grey levels (top, middle, bottom).
constexpr int kActionsShadeTop = 46;
constexpr int kActionsShadeMiddle = 42;
constexpr int kActionsShadeBottom = 38;

// Disk Management: the stock navy hatch is recoloured to the accent, and the
// selected partition frame uses a lighter steel blue.
constexpr COLORREF kColorDiskStockNavy = RGB(0, 0, 128);
constexpr COLORREF kColorDiskSelected = RGB(80, 120, 160);

constexpr COLORREF kNotMapped = 0xffffffff;

using AllowDarkModeForWindow_t = BOOL(WINAPI*)(HWND, BOOL);
using SetPreferredAppMode_t = PreferredAppMode(WINAPI*)(PreferredAppMode);
using FlushMenuThemes_t = void(WINAPI*)();
using SetWindowTheme_t = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);

// Use the original GetSysColor when restoring native colours.
using GetSysColor_t = DWORD(WINAPI*)(int);
GetSysColor_t pGetSysColor = nullptr;

AllowDarkModeForWindow_t pAllowDarkModeForWindow = nullptr;
SetPreferredAppMode_t pSetPreferredAppMode = nullptr;
FlushMenuThemes_t pFlushMenuThemes = nullptr;
SetWindowTheme_t pSetWindowTheme = nullptr;

// GetWindowTheme identifies a window's current visual style.
using GetWindowTheme_t = void*(WINAPI*)(HWND);
GetWindowTheme_t pGetWindowTheme = nullptr;

// Override the affected list-box selection fill while dark mode is active.
using GetThemeColor_t = HRESULT(WINAPI*)(HTHEME, int, int, int, COLORREF*);
GetThemeColor_t pGetThemeColor = nullptr;
GetThemeColor_t pGetThemeColorHook = nullptr;

// The optional private export identifies the resolved theme class. Unknown
// classes retain native drawing instead of relying on overlapping part IDs.
using GetThemeClass_t = HRESULT(WINAPI*)(HTHEME, LPWSTR, int);
GetThemeClass_t pGetThemeClass = nullptr;

bool ThemeClassIs(HTHEME theme, PCWSTR expected) {
    if (!theme || !expected || !pGetThemeClass) {
        return false;
    }
    WCHAR name[128] = {};
    if (FAILED(pGetThemeClass(theme, name, ARRAYSIZE(name)))) {
        return false;
    }
    name[ARRAYSIZE(name) - 1] = L'\0';
    PCWSTR base = name;
    for (PCWSTR p = name; p[0] && p[1]; ++p) {
        if (p[0] == L':' && p[1] == L':') {
            base = p + 2;
            ++p;
        }
    }
    return _wcsicmp(base, expected) == 0;
}

// The active flag is read on GUI threads and cleared during unload.
std::atomic<bool> g_active{false};
std::atomic<bool> g_unloading{false};
std::atomic<int> g_activeThreadHooks{0};

struct ActiveThreadHookGuard {
    ActiveThreadHookGuard() { g_activeThreadHooks.fetch_add(1, std::memory_order_acq_rel); }
    ~ActiveThreadHookGuard() { g_activeThreadHooks.fetch_sub(1, std::memory_order_acq_rel); }
};

// Non-zero while a hosted WinForms window is handling a paint or erase message
// on this thread. It scopes the theme hooks that recolour WinForms pages.
thread_local int g_formsPaintDepth = 0;
// Counts subclass paint dispatch independently of theme-colour scoping.
thread_local int g_subclassPaintDepth = 0;
// Avoid caching mod-owned brushes in the read-only Forms colour handler.
thread_local bool g_nativeFormsBrushScope = false;

// Native tab control, or the WinForms wrapper class around it.
bool IsTabControlClass(PCWSTR className) {
    static const WCHAR kFormsTabPrefix[] = L"WindowsForms10.SysTabControl32.";
    return wcscmp(className, L"SysTabControl32") == 0 ||
           wcsncmp(className, kFormsTabPrefix, ARRAYSIZE(kFormsTabPrefix) - 1) == 0;
}

HRESULT WINAPI GetThemeColor_Hook(HTHEME theme, int part, int state, int prop,
                                  COLORREF* colour) {
    // WinForms group boxes take their caption colour from the light theme.
    if (colour && g_active && g_formsPaintDepth > 0 && prop == TMT_TEXTCOLOR &&
        part == BP_GROUPBOX && ThemeClassIs(theme, L"Button")) {
        *colour = state == GBS_DISABLED ? kColorTextDisabled : kColorText;
        return S_OK;
    }
    if (colour && g_active && prop == TMT_FILLCOLOR && part == LBCP_ITEM) {
        switch (state) {
        case LBPSI_HOT:
            *colour = kColorSelectionHot;
            return S_OK;
        case LBPSI_HOTSELECTED:
        case LBPSI_SELECTED:
            *colour = kColorSelection;
            return S_OK;
        case LBPSI_SELECTEDNOTFOCUS:
            *colour = kColorSelectionInactive;
            return S_OK;
        }
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
    if (!pSetWindowThemeHook) {
        return E_FAIL;
    }
    if (!hWnd || !g_active) {
        return pSetWindowThemeHook(hWnd, subApp, subId);
    }

    // Keep toolbars, labels, links and tab strips on their classic drawing paths.
    WCHAR className[64] = {};
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
        (wcscmp(className, L"ToolbarWindow32") == 0 ||
         wcscmp(className, L"Static") == 0 ||
         wcscmp(className, L"SysLink") == 0 ||
         IsTabControlClass(className))) {
        return pSetWindowThemeHook(hWnd, L"", L"");
    }
    return pSetWindowThemeHook(hWnd, subApp, subId);
}
DWORD g_darkTitleBarAttribute = 0;

struct Settings {
    bool darkTitleBar;
    bool roundCorners;
    bool darkStatusBar;
};

Settings g_settings;

LRESULT SendToControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Scale a 96-DPI pixel value for the window's DPI.
int ScaleForWindow(HWND hWnd, int pixels) {
    const UINT dpi = hWnd ? GetDpiForWindow(hWnd) : 0;
    return dpi ? MulDiv(pixels, static_cast<int>(dpi), 96) : pixels;
}

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
    WinFormsSurface,
    WinFormsBackdrop,
    PushButton,
    ChoiceButton,
    FormHost,
};

constexpr UINT_PTR kModSubclassId = 0x4D4D4331u;
constexpr UINT_PTR kAuxSubclassId = 0x4D4D4332u;

bool SubclassWith(HWND hWnd, WindowKind kind);
void RemoveAllSubclasses();
void WalkEveryThread(bool waitForIt);
void WalkThreadWindows(DWORD threadId);
void ResetWindowState(HWND hWnd);

std::mutex g_stateLock;
std::unordered_set<HWND> g_subclassed;
std::unordered_set<HWND> g_adding;

HBRUSH g_backgroundBrush = nullptr;
HBRUSH g_hoverBrush = nullptr;
HBRUSH g_lineBrush = nullptr;
HBRUSH g_tabSelectedBrush = nullptr;
HBRUSH g_pressedBrush = nullptr;
HBRUSH g_accentBrush = nullptr;

// Caches are thread-local and populated on each window's owning thread.

UINT g_ownThreadSetupMessage = 0;

// A registered message avoids collisions with MMC's private WM_APP messages.
UINT OwnThreadSetupMessage() {
    if (!g_ownThreadSetupMessage) {
        g_ownThreadSetupMessage =
            RegisterWindowMessageW(L"MMC Dark Mode: fill a cache on the owning thread");
    }
    return g_ownThreadSetupMessage;
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
    HFONT font;
    bool valid;
};
thread_local StatusCache g_statusCache[8] = {};

void RefreshStatusCache(HWND hWnd);
void ForgetSubclass(HWND hWnd);

struct HotEntry {
    HWND toolbar;
    RECT rect;
    bool valid;
};
thread_local HotEntry g_hot[16] = {};

HotEntry* HotFor(HWND toolbar, bool create) {
    const int slots = static_cast<int>(ARRAYSIZE(g_hot));
    for (int i = 0; i < slots; i++) {
        if (g_hot[i].toolbar == toolbar) {
            return &g_hot[i];
        }
    }
    if (!create) {
        return nullptr;
    }
    for (int i = 0; i < slots; i++) {
        if (!g_hot[i].toolbar || !IsWindow(g_hot[i].toolbar)) {
            g_hot[i] = {};
            g_hot[i].toolbar = toolbar;
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

    // Invalidate the previous hot toolbar item when the hovered button changes.
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
HBRUSH PressedBrush();
HBRUSH AccentBrush();
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

            HBRUSH line = LineBrush();
            if (line) {
                RECT top = {client.left, client.top, client.right, client.top + 1};
                FillRect(dc, &top, line);
            }

            const int padding = ScaleForWindow(hWnd, 6);
            const int dividerInset = ScaleForWindow(hWnd, 4);
            for (int i = 0; i < static_cast<int>(ARRAYSIZE(g_statusCache)); i++) {
                StatusCache& cache = g_statusCache[i];
                if (cache.hWnd != hWnd || !cache.valid) {
                    continue;
                }
                // Without this the DC keeps the raster system font.
                HGDIOBJ oldFont = SelectObject(
                    dc, cache.font ? static_cast<HGDIOBJ>(cache.font)
                              : GetStockObject(DEFAULT_GUI_FONT));
                for (int part = 0; part < cache.parts && part < 16; part++) {
                    RECT partRect = cache.rects[part];
                    if (line && part + 1 < cache.parts) {
                        RECT divider = {partRect.right, partRect.top + dividerInset,
                                        partRect.right + 1,
                                        partRect.bottom - dividerInset};
                        if (divider.bottom > divider.top) {
                            FillRect(dc, &divider, line);
                        }
                    }
                    if (!cache.text[part][0]) {
                        continue;
                    }
                    partRect.left += padding;
                    DrawTextOn(dc, cache.text[part], &partRect,
                               DT_LEFT | DT_VCENTER | DT_SINGLELINE |
                                   DT_END_ELLIPSIS | DT_NOPREFIX,
                               kColorText, kColorBackground);
                }
                if (oldFont) {
                    SelectObject(dc, oldFont);
                }
            }
        }
        EndPaint(hWnd, &ps);
        return 0;
    }

    LRESULT result =
        DefSubclassProc(hWnd, message, wParam, lParam);

    if (message == SB_SETTEXTW || message == SB_SETPARTS || message == WM_SIZE ||
        message == WM_SETFONT) {
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

    pAllowDarkModeForWindow = reinterpret_cast<AllowDarkModeForWindow_t>(
        GetProcAddress(uxtheme, (const char*)(ULONG_PTR)kOrdinalAllowDarkModeForWindow));
    pSetPreferredAppMode = reinterpret_cast<SetPreferredAppMode_t>(
        GetProcAddress(uxtheme, (const char*)(ULONG_PTR)kOrdinalSetPreferredAppMode));
    pFlushMenuThemes = reinterpret_cast<FlushMenuThemes_t>(
        GetProcAddress(uxtheme, (const char*)(ULONG_PTR)kOrdinalFlushMenuThemes));
    pSetWindowTheme =
        reinterpret_cast<SetWindowTheme_t>(GetProcAddress(uxtheme, "SetWindowTheme"));
    pGetWindowTheme =
        reinterpret_cast<GetWindowTheme_t>(GetProcAddress(uxtheme, "GetWindowTheme"));
    pGetThemeColor =
        reinterpret_cast<GetThemeColor_t>(GetProcAddress(uxtheme, "GetThemeColor"));
    pGetThemeClass = reinterpret_cast<GetThemeClass_t>(
        GetProcAddress(uxtheme, (const char*)(ULONG_PTR)74));
    if (!pGetThemeClass) {
        Wh_Log(L"theme class lookup unavailable; WinForms page drawing stays native");
    }

    if (!pAllowDarkModeForWindow || !pSetPreferredAppMode || !pSetWindowTheme) {
        Wh_Log(L"uxtheme is missing dark mode entry points "
               L"(allow=%d mode=%d theme=%d)",
               pAllowDarkModeForWindow != nullptr,
               pSetPreferredAppMode != nullptr,
               pSetWindowTheme != nullptr);
        return false;
    }

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

// Caption colours make the title bar blend with the client area; the default
// value hands control back to the system.
void SetCaptionColours(HWND hWnd, bool dark) {
    COLORREF caption = dark ? kColorBackground : DWMWA_COLOR_DEFAULT;
    COLORREF text = dark ? kColorText : DWMWA_COLOR_DEFAULT;
    DwmSetWindowAttribute(hWnd, kDwmwaCaptionColor, &caption, sizeof(caption));
    DwmSetWindowAttribute(hWnd, kDwmwaTextColor, &text, sizeof(text));
}

void ApplyToTopLevel(HWND hWnd) {
    // Both settings are applied in both directions: they can be switched off
    // while the mod is loaded, and the window must follow.
    SetDarkTitleBar(hWnd, g_settings.darkTitleBar);
    SetCaptionColours(hWnd, g_settings.darkTitleBar);

    int preference = g_settings.roundCorners ? DWMWCP_ROUND : DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference,
                          sizeof(preference));

    // A visible border keeps the window separated from other dark windows.
    COLORREF border = kColorLine;
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &border, sizeof(border));

    if (!g_backgroundBrush) {
        g_backgroundBrush = CreateSolidBrush(kColorBackground);
    }
    SubclassWith(hWnd, WindowKind::Backdrop);
    if (GetMenu(hWnd)) {
        DrawMenuBar(hWnd);
    }
    if (IsWindowVisible(hWnd)) {
        RedrawWindow(hWnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
    }
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
    const int slots = static_cast<int>(ARRAYSIZE(g_statusCache));
    for (int i = 0; i < slots; i++) {
        if (g_statusCache[i].hWnd == hWnd) {
            cache = &g_statusCache[i];
            break;
        }
    }
    if (!cache) {
        for (int i = 0; i < slots; i++) {
            if (!g_statusCache[i].hWnd || !IsWindow(g_statusCache[i].hWnd)) {
                cache = &g_statusCache[i];
                *cache = {};
                break;
            }
        }
    }
    if (!cache) {
        return;
    }

    cache->hWnd = hWnd;
    cache->valid = false;
    cache->font = reinterpret_cast<HFONT>(SendToControl(hWnd, WM_GETFONT, 0, 0));

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
    int forms = 0, tabs = 0, depth = 0;
    while (parent && depth < 20 && GetClassNameW(parent, cls, ARRAYSIZE(cls))) {
        if (wcsncmp(cls, L"WindowsForms10.Window.", 22) == 0) {
            ++forms;
        } else if (wcsncmp(cls, L"WindowsForms10.SysTabControl32.", 31) == 0) {
            ++tabs;
            if (tabs > 2) return false;
        } else {
            break;
        }
        ++depth;
        parent = GetParent(parent);
    }
    if (!forms || depth == 20 || !parent ||
        !GetClassNameW(parent, cls, ARRAYSIZE(cls)) ||
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

bool IsSubclassed(HWND hWnd);

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

bool ChangeHostedWinFormsList(HWND hWnd, COLORREF expectedBackground,
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
        return false;
    }
    // Keep transparent text; roll back the text colour if the background update fails.
    if (!WriteHostedListColour(hWnd, LVM_SETTEXTCOLOR, text)) {
        return false;
    }
    if (!WriteHostedListColour(hWnd, LVM_SETBKCOLOR, background)) {
        WriteHostedListColour(hWnd, LVM_SETTEXTCOLOR, oldText);
        return false;
    }
    InvalidateRect(hWnd, nullptr, TRUE);
    return true;
}

// Mark only palettes that this paint-time retry actually changed. A list may
// later be parked outside the host tree, so unload also checks the marker.
constexpr PCWSTR kPaintListProperty = L"MMC Dark Mode: paint list {57E42678-DA28-4E67-BAD6-15AA3E713D7B}";

void RetryHostedListPalette(HWND hWnd) {
    if (!IsHostedWinFormsList(hWnd)) return;
    const COLORREF nativeBackground = RealSystemColour(COLOR_WINDOW);
    const COLORREF nativeText = RealSystemColour(COLOR_WINDOWTEXT);
    const bool marked = GetPropW(hWnd, kPaintListProperty) != nullptr;
    if (!marked) {
        COLORREF background = 0, text = 0, textBackground = 0;
        if (!ReadHostedListColour(hWnd, LVM_GETBKCOLOR, &background) ||
            !ReadHostedListColour(hWnd, LVM_GETTEXTCOLOR, &text) ||
            !ReadHostedListColour(hWnd, LVM_GETTEXTBKCOLOR, &textBackground) ||
            background != nativeBackground || text != nativeText ||
            textBackground != CLR_NONE ||
            !SetPropW(hWnd, kPaintListProperty, reinterpret_cast<HANDLE>(1))) return;
    }
    const bool changed = ChangeHostedWinFormsList(
        hWnd, nativeBackground, nativeText, kColorBackground, kColorText);
    if (!changed && !marked) RemovePropW(hWnd, kPaintListProperty);
}

void RestorePaintListPalette(HWND hWnd) {
    if (!GetPropW(hWnd, kPaintListProperty)) return;
    ChangeHostedWinFormsList(hWnd, kColorBackground, kColorText,
                             RealSystemColour(COLOR_WINDOW),
                             RealSystemColour(COLOR_WINDOWTEXT));
    RemovePropW(hWnd, kPaintListProperty);
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
        if (dark) {
            // Track every palette change on a subclassed window, even if the
            // list is later parked outside its original host tree.
            if (g_active &&
                (IsSubclassed(hWnd) || SubclassWith(hWnd, WindowKind::WinFormsSurface)) &&
                g_active) {
                RetryHostedListPalette(hWnd);
            }
        } else {
            RestorePaintListPalette(hWnd);
            ChangeHostedWinFormsList(hWnd, kColorBackground, kColorText,
                                     background, text);
        }
    } else if (wcscmp(className, L"SysTreeView32") == 0) {
        SendToControl(hWnd, TVM_SETBKCOLOR, 0,
                      dark ? kColorBackground
                           : static_cast<LPARAM>(static_cast<COLORREF>(-1)));
        SendToControl(hWnd, TVM_SETTEXTCOLOR, 0,
                      dark ? kColorText
                           : static_cast<LPARAM>(static_cast<COLORREF>(-1)));
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
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    } else if (IsTabControlClass(className)) {
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

// Record removed frame-style bits for restoration (guarded by g_stateLock).
struct StyleFix {
    LONG_PTR styleBits;
    LONG_PTR exStyleBits;
};
std::unordered_map<HWND, StyleFix> g_styleFixes;

void NoteRemovedStyles(HWND hWnd, LONG_PTR styleBits, LONG_PTR exStyleBits) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_stateLock);
    if (g_styleFixes.size() > 64) {
        for (auto it = g_styleFixes.begin(); it != g_styleFixes.end();) {
            if (!IsWindow(it->first)) {
                it = g_styleFixes.erase(it);
            } else {
                ++it;
            }
        }
    }
    StyleFix& fix = g_styleFixes[hWnd];
    fix.styleBits |= styleBits;
    fix.exStyleBits |= exStyleBits;
}

void RestoreRemovedStyles(HWND hWnd) {
    LONG_PTR styleBits = 0;
    LONG_PTR exStyleBits = 0;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        const auto it = g_styleFixes.find(hWnd);
        if (it == g_styleFixes.end()) {
            return;
        }
        styleBits = it->second.styleBits;
        exStyleBits = it->second.exStyleBits;
        g_styleFixes.erase(it);
    }
    if (!IsWindow(hWnd)) {
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
    if (styleBits || exStyleBits) {
        RefreshFrame(hWnd);
    }
}

void FixActionsPaleRows(HWND hWnd, HDC providedDc);
LRESULT CALLBACK ModSubclassProc(HWND hWnd, UINT message, WPARAM wParam,
                                 LPARAM lParam, UINT_PTR uIdSubclass,
                                 DWORD_PTR refData);

void ForgetSubclass(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_stateLock);
    g_subclassed.erase(hWnd);
    g_styleFixes.erase(hWnd);
}

bool IsSubclassed(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_stateLock);
    return g_subclassed.count(hWnd) != 0;
}

// Subclass only on the owning thread; defer work rather than blocking a busy thread.

// Track threads requiring a later owning-thread walk.
std::unordered_set<DWORD> g_pendingThreads;

void NoteWalkPendingFor(HWND hWnd) {
    if (g_unloading || !g_active) {
        return;
    }
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
    if (!hWnd || !IsWindow(hWnd) || g_unloading || !g_active) {
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
    const BOOL added = SetWindowSubclass(hWnd, ModSubclassProc, kModSubclassId,
                                         static_cast<DWORD_PTR>(kind));
    bool keep = false;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        g_adding.erase(hWnd);
        if (added && g_active && !g_unloading) {
            g_subclassed.insert(hWnd);
            keep = true;
        }
    }
    if (added && !keep) {
        // Unload may have started while subclass installation was in progress.
        RemoveWindowSubclass(hWnd, ModSubclassProc, kModSubclassId);
        return false;
    }
    if (!added) {
        WCHAR className[64] = {};
        GetClassNameW(hWnd, className, ARRAYSIZE(className));
        Wh_Log(L"could not subclass a %s window", className);
    }
    return added != FALSE;
}

// A Wizard97 exterior page can install a colour handler after the mod. Install
// a second, tracked handler only after the modal loop is idle so native input,
// bitmap and non-text controls remain unchanged.
std::unordered_set<HWND> g_wizardColourPages;
std::unordered_set<HWND> g_wizardColourAdding;

// A comctl32 wizard sheet owns the standard button/divider IDs. The hidden tab
// control is not guaranteed to exist on every wizard, so it is not required.
bool IsWizardSheetWindow(HWND sheet) {
    WCHAR sc[16] = {};
    if (!sheet || !GetClassNameW(sheet, sc, ARRAYSIZE(sc)) || wcscmp(sc, L"#32770")) return false;
    HWND separator = GetDlgItem(sheet, 12326);
    if (separator && GetParent(separator) == sheet) return true;
    for (int id = 12323; id <= 12325; ++id) {  // Back, Next, Finish
        HWND button = GetDlgItem(sheet, id);
        if (button && GetParent(button) == sheet) return true;
    }
    return false;
}

bool IsWizardColourPage(HWND page, HWND sheet) {
    WCHAR pc[16] = {};
    return page && sheet && GetParent(page) == sheet &&
           (GetWindowLongPtrW(page, GWL_STYLE) & WS_CHILD) &&
           GetClassNameW(page, pc, ARRAYSIZE(pc)) && !wcscmp(pc, L"#32770") &&
           IsWizardSheetWindow(sheet);
}

bool IsNeutralColour(COLORREF c) {
    return GetRValue(c) == GetGValue(c) && GetGValue(c) == GetBValue(c);
}

// A Wizard97 watermark brush can contain artwork elsewhere. Sample the entire
// text control's mapped background and replace it only when it is a
// uniform standard page colour. Artwork or custom colours remain untouched.
bool IsBlankWizardPattern(HWND child, HDC dc, HBRUSH brush, const LOGBRUSH& info) {
    if (!child || !dc || !brush ||
        (info.lbStyle != BS_PATTERN && info.lbStyle != BS_DIBPATTERN &&
         info.lbStyle != BS_DIBPATTERNPT) || GetMapMode(dc) != MM_TEXT ||
        GetLayout(dc) != 0 || WindowFromDC(dc) != child) return false;
    POINT viewport = {}, window = {}, brushOrigin = {};
    RECT bounds = {};
    if (!GetViewportOrgEx(dc, &viewport) || !GetWindowOrgEx(dc, &window) ||
        !GetBrushOrgEx(dc, &brushOrigin) || viewport.x || viewport.y ||
        window.x || window.y || !GetClientRect(child, &bounds) ||
        bounds.left || bounds.top) return false;
    const int width = bounds.right, height = bounds.bottom;
    if (width <= 0 || height <= 0 || width > 1024 || height > 512 ||
        static_cast<unsigned long long>(width) * height > 524288) return false;
    BITMAPINFO bitmapInfo = {};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    HDC memory = CreateCompatibleDC(dc);
    if (!memory) return false;
    HBITMAP bitmap = CreateDIBSection(dc, &bitmapInfo, DIB_RGB_COLORS, &pixels, nullptr, 0);
    bool white = false;
    if (bitmap && pixels) {
        HGDIOBJ previous = SelectObject(memory, bitmap);
        if (previous && previous != HGDI_ERROR &&
            SetBrushOrgEx(memory, brushOrigin.x, brushOrigin.y, nullptr) &&
            FillRect(memory, &bounds, brush)) {
            white = GdiFlush() != FALSE;
            auto data = static_cast<const DWORD*>(pixels);
            const size_t count = static_cast<size_t>(width) * static_cast<size_t>(height);
            const DWORD first = data[0] & 0x00ffffffu;
            // A 32-bit DIB stores RGB as 0x00RRGGBB; COLORREF uses BGR.
            const auto dibColour = [](COLORREF colour) -> DWORD {
                return (static_cast<DWORD>(GetRValue(colour)) << 16) |
                       (static_cast<DWORD>(GetGValue(colour)) << 8) |
                       static_cast<DWORD>(GetBValue(colour));
            };
            white = white && (first == 0x00ffffffu ||
                              first == dibColour(RealSystemColour(COLOR_WINDOW)) ||
                              first == dibColour(RealSystemColour(COLOR_BTNFACE)));
            for (size_t i = 0; white && i < count; ++i) {
                if ((data[i] & 0x00ffffffu) != first) { white = false; break; }
            }
        }
        if (previous && previous != HGDI_ERROR) SelectObject(memory, previous);
    }
    if (bitmap) DeleteObject(bitmap);
    DeleteDC(memory);
    return white;
}

LRESULT CALLBACK WizardColourSubclass(HWND page, UINT message, WPARAM wParam,
                                      LPARAM lParam, UINT_PTR uIdSubclass,
                                      DWORD_PTR) {
    if (message == WM_NCDESTROY) {
        { std::lock_guard<std::mutex> lock(g_stateLock); g_wizardColourPages.erase(page); }
        RemoveWindowSubclass(page, WizardColourSubclass, uIdSubclass);
        return DefSubclassProc(page, message, wParam, lParam);
    }
    if (!g_active || message != WM_CTLCOLORSTATIC || !wParam || !lParam || !g_backgroundBrush) {
        return DefSubclassProc(page, message, wParam, lParam);
    }
    HWND child = reinterpret_cast<HWND>(lParam);
    WCHAR cls[16] = {};
    LONG_PTR style = IsWindow(child) ? GetWindowLongPtrW(child, GWL_STYLE) : 0;
    const LONG_PTR kind = style & SS_TYPEMASK;
    const bool textLabel = GetParent(child) == page &&
        GetClassNameW(child, cls, ARRAYSIZE(cls)) && !wcscmp(cls, L"Static") &&
        (kind == SS_LEFT || kind == SS_CENTER || kind == SS_RIGHT ||
         kind == SS_SIMPLE || kind == SS_LEFTNOWORDWRAP);
    LRESULT result = DefSubclassProc(page, message, wParam, lParam);
    if (!textLabel || !g_active || !IsWindow(page) || !IsWindow(child) ||
        !IsWizardColourPage(page, GetParent(page))) return result;
    LOGBRUSH brush = {};
    HDC dc = reinterpret_cast<HDC>(wParam);
    const COLORREF colour = GetTextColor(dc);
    const bool gotBrush = result &&
        GetObjectW(reinterpret_cast<HBRUSH>(result), sizeof(brush), &brush) == sizeof(brush);
    // Native, hook-mapped and dark text colours are all greys. Coloured text
    // is a deliberate design choice and stays untouched.
    if (!gotBrush || colour == CLR_INVALID || !IsNeutralColour(colour)) return result;
    const bool whiteSolid = brush.lbStyle == BS_SOLID && IsNeutralColour(brush.lbColor);
    if (!whiteSolid && !IsBlankWizardPattern(child, dc, reinterpret_cast<HBRUSH>(result), brush)) return result;
    SetBkMode(dc, OPAQUE);
    SetBkColor(dc, kColorBackground);
    SetTextColor(dc, IsWindowEnabled(child) ? kColorText : kColorTextDisabled);
    return reinterpret_cast<LRESULT>(g_backgroundBrush);
}

// Wizard97 exterior pages let comctl32 answer WM_CTLCOLORSTATIC, so the page
// level override can lose. Draw the text labels of wizard pages ourselves
// instead: this does not depend on the order of any colour handler.
std::unordered_set<HWND> g_wizardLabels;

bool IsWizardTextLabel(HWND label) {
    WCHAR cls[16] = {};
    if (!label || !IsWindow(label) ||
        !GetClassNameW(label, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"Static")) return false;
    const LONG_PTR kind = GetWindowLongPtrW(label, GWL_STYLE) & SS_TYPEMASK;
    if (kind != SS_LEFT && kind != SS_CENTER && kind != SS_RIGHT &&
        kind != SS_SIMPLE && kind != SS_LEFTNOWORDWRAP) return false;
    HWND page = GetParent(label);
    return page && IsWizardColourPage(page, GetParent(page));
}

LRESULT CALLBACK WizardLabelSubclass(HWND hWnd, UINT message, WPARAM wParam,
                                     LPARAM lParam, UINT_PTR uIdSubclass,
                                     DWORD_PTR) {
    if (message == WM_NCDESTROY) {
        { std::lock_guard<std::mutex> lock(g_stateLock); g_wizardLabels.erase(hWnd); }
        RemoveWindowSubclass(hWnd, WizardLabelSubclass, uIdSubclass);
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }
    if (!g_active || !g_backgroundBrush ||
        (message != WM_ERASEBKGND && message != WM_PAINT && message != WM_PRINTCLIENT)) {
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }
    RECT rc = {};
    GetClientRect(hWnd, &rc);
    if (message == WM_ERASEBKGND) {
        return FillRect(reinterpret_cast<HDC>(wParam), &rc, g_backgroundBrush) ? 1 : 0;
    }
    PAINTSTRUCT ps = {};
    HDC dc = message == WM_PAINT ? BeginPaint(hWnd, &ps) : reinterpret_cast<HDC>(wParam);
    if (!dc) return 0;
    FillRect(dc, &rc, g_backgroundBrush);
    WCHAR text[1024] = {};
    GetWindowTextW(hWnd, text, ARRAYSIZE(text));
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR kind = style & SS_TYPEMASK;
    UINT format = DT_EXPANDTABS;
    format |= kind == SS_CENTER ? DT_CENTER : kind == SS_RIGHT ? DT_RIGHT : DT_LEFT;
    format |= (kind == SS_SIMPLE || kind == SS_LEFTNOWORDWRAP) ? DT_SINGLELINE : DT_WORDBREAK;
    if (style & SS_NOPREFIX) format |= DT_NOPREFIX;
    if (style & SS_ENDELLIPSIS) format |= DT_END_ELLIPSIS;
    if (style & SS_PATHELLIPSIS) format |= DT_PATH_ELLIPSIS;
    if (style & SS_WORDELLIPSIS) format |= DT_WORD_ELLIPSIS;
    if (SendMessageW(hWnd, WM_QUERYUISTATE, 0, 0) & UISF_HIDEACCEL) format |= DT_HIDEPREFIX;
    HFONT font = reinterpret_cast<HFONT>(SendMessageW(hWnd, WM_GETFONT, 0, 0));
    HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;
    const int oldMode = SetBkMode(dc, TRANSPARENT);
    const COLORREF oldColour = SetTextColor(
        dc, IsWindowEnabled(hWnd) ? kColorText : kColorTextDisabled);
    DrawTextW(dc, text, -1, &rc, format);
    SetTextColor(dc, oldColour);
    SetBkMode(dc, oldMode);
    if (oldFont) SelectObject(dc, oldFont);
    if (message == WM_PAINT) EndPaint(hWnd, &ps);
    return 0;
}

void ThemeWizardLabel(HWND label) {
    if (g_unloading || !g_active || !OnOwningThread(label) || !IsWizardTextLabel(label)) return;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        if (g_wizardLabels.count(label)) return;
        g_wizardLabels.insert(label);
    }
    if (SetWindowSubclass(label, WizardLabelSubclass, kAuxSubclassId, 0)) {
        InvalidateRect(label, nullptr, TRUE);
    } else {
        std::lock_guard<std::mutex> lock(g_stateLock);
        g_wizardLabels.erase(label);
    }
}

// While an embedded web view loads, MMC shows "Shell Embedding" and its child
// "Shell DocObject View" with nothing in them yet. DocObject View paints that
// empty area itself in light grey (240), not with its class brush, through a
// path the colour hooks never see. Paint the empty area dark ourselves and leave
// any child that is really showing (the browser) alone.
bool IsInsideMmcView(HWND hWnd);
constexpr UINT_PTR kWebCurtainTimer = 0x4D43;
constexpr UINT kWebCurtainMs = 800;
std::unordered_set<HWND> g_shellViews;
std::unordered_set<HWND> g_shellDocCurtains;
void ShowWebView(HWND hWnd);

void FillExcludingChildren(HWND hWnd, HDC dc, const RECT* area) {
    const int saved = SaveDC(dc);
    for (HWND c = GetWindow(hWnd, GW_CHILD); c; c = GetWindow(c, GW_HWNDNEXT)) {
        if (!IsWindowVisible(c)) continue;
        HRGN rgn = CreateRectRgn(0, 0, 0, 0);
        const int type = rgn ? GetWindowRgn(c, rgn) : 0;
        if (rgn) DeleteObject(rgn);
        if (type == NULLREGION) continue;  // clipped to nothing, so nothing to protect
        RECT r = {};
        GetWindowRect(c, &r);
        MapWindowPoints(nullptr, hWnd, reinterpret_cast<POINT*>(&r), 2);
        ExcludeClipRect(dc, r.left, r.top, r.right, r.bottom);
    }
    FillRect(dc, area, g_backgroundBrush);
    if (saved) RestoreDC(dc, saved);
}

LRESULT CALLBACK ShellViewSubclass(HWND hWnd, UINT message, WPARAM wParam,
                                   LPARAM lParam, UINT_PTR uIdSubclass,
                                   DWORD_PTR) {
    if (message == WM_NCDESTROY) {
        KillTimer(hWnd, kWebCurtainTimer);
        {
            std::lock_guard<std::mutex> lock(g_stateLock);
            g_shellViews.erase(hWnd);
            g_shellDocCurtains.erase(hWnd);
        }
        RemoveWindowSubclass(hWnd, ShellViewSubclass, uIdSubclass);
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }
    if (message == WM_TIMER && wParam == kWebCurtainTimer) {
        KillTimer(hWnd, kWebCurtainTimer);
        {
            std::lock_guard<std::mutex> lock(g_stateLock);
            g_shellDocCurtains.erase(hWnd);
        }
        ShowWebView(hWnd);
        return 0;
    }
    if (!g_active || !g_backgroundBrush) return DefSubclassProc(hWnd, message, wParam, lParam);
    if (message == WM_ERASEBKGND) {
        RECT rc = {};
        GetClientRect(hWnd, &rc);
        FillExcludingChildren(hWnd, reinterpret_cast<HDC>(wParam), &rc);
        return 1;
    }
    if (message == WM_PAINT) {
        PAINTSTRUCT ps = {};
        HDC dc = BeginPaint(hWnd, &ps);
        if (dc) FillExcludingChildren(hWnd, dc, &ps.rcPaint);
        EndPaint(hWnd, &ps);
        return 0;
    }
    if (message == WM_PRINTCLIENT) {
        RECT rc = {};
        GetClientRect(hWnd, &rc);
        FillExcludingChildren(hWnd, reinterpret_cast<HDC>(wParam), &rc);
        return 0;
    }
    return DefSubclassProc(hWnd, message, wParam, lParam);
}

void ThemeShellView(HWND hWnd) {
    WCHAR cls[32] = {};
    if (g_unloading || !g_active || !hWnd || !OnOwningThread(hWnd) ||
        !GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) ||
        (wcscmp(cls, L"Shell DocObject View") && wcscmp(cls, L"Shell Embedding")) ||
        !IsInsideMmcView(hWnd)) return;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        if (g_shellViews.count(hWnd)) return;
        g_shellViews.insert(hWnd);
    }
    if (SetWindowSubclass(hWnd, ShellViewSubclass, kAuxSubclassId, 0)) {
        if (wcscmp(cls, L"Shell Embedding") == 0) {
            RedrawWindow(hWnd, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
        } else {
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    } else {
        std::lock_guard<std::mutex> lock(g_stateLock);
        g_shellViews.erase(hWnd);
    }
}

// MMC keeps a plain, empty Static the size of the result pane under its views. It
// is what shows while one view is hidden and the next is not up yet, and its
// colour comes from the parent's default handling, i.e. light grey. Paint it dark.
bool WindowClassEquals(HWND hWnd, PCWSTR expected);

void ThemeViewPlaceholder(HWND hWnd) {
    WCHAR cls[16] = {};
    if (g_unloading || !g_active || !hWnd || !OnOwningThread(hWnd) ||
        !GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"Static")) return;
    HWND parent = GetParent(hWnd);
    WCHAR text[4] = {};
    GetWindowTextW(hWnd, text, ARRAYSIZE(text));
    if (!parent || text[0] || !WindowClassEquals(parent, L"MMCViewWindow")) return;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        if (g_wizardLabels.count(hWnd)) return;
        g_wizardLabels.insert(hWnd);
    }
    if (SetWindowSubclass(hWnd, WizardLabelSubclass, kAuxSubclassId, 0)) {
        InvalidateRect(hWnd, nullptr, TRUE);
    } else {
        std::lock_guard<std::mutex> lock(g_stateLock);
        g_wizardLabels.erase(hWnd);
    }
}

BOOL CALLBACK ThemeWizardLabelChild(HWND child, LPARAM) {
    ThemeWizardLabel(child);
    return TRUE;
}

// The wizard's divider (above the buttons) and header line are etched statics
// that draw with native 3D colours. Draw them as one line in the border colour.
std::unordered_set<HWND> g_wizardDividers;

LRESULT CALLBACK WizardDividerSubclass(HWND hWnd, UINT message, WPARAM wParam,
                                       LPARAM lParam, UINT_PTR uIdSubclass,
                                       DWORD_PTR) {
    if (message == WM_NCDESTROY) {
        { std::lock_guard<std::mutex> lock(g_stateLock); g_wizardDividers.erase(hWnd); }
        RemoveWindowSubclass(hWnd, WizardDividerSubclass, uIdSubclass);
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }
    if (!g_active || !g_backgroundBrush ||
        (message != WM_ERASEBKGND && message != WM_PAINT && message != WM_PRINTCLIENT &&
         message != WM_NCPAINT)) {
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }
    RECT rc = {};
    GetClientRect(hWnd, &rc);
    const bool clientHasArea = rc.right > 0 && rc.bottom > 0;
    if (message == WM_NCPAINT) {
        // A sunken/etched static draws its 3D edge in the non-client area, and the
        // client can be empty. Paint the whole window ourselves in that case.
        if (clientHasArea) return 0;
        RECT w = {};
        GetWindowRect(hWnd, &w);
        HDC wdc = GetWindowDC(hWnd);
        if (!wdc) return 0;
        const RECT all = {0, 0, w.right - w.left, w.bottom - w.top};
        FillRect(wdc, &all, g_backgroundBrush);
        const int y = all.top + (all.bottom - all.top) / 2;
        const RECT line = {all.left, y, all.right, y + 1};
        FillRect(wdc, &line, LineBrush());
        ReleaseDC(hWnd, wdc);
        return 0;
    }
    if (!clientHasArea) {
        // Everything was drawn in WM_NCPAINT. A pending WM_PAINT must still be
        // validated, or Windows keeps re-sending it to this window and starves
        // the paint of the windows after it (the wizard footer buttons).
        if (message == WM_PAINT) {
            PAINTSTRUCT empty = {};
            BeginPaint(hWnd, &empty);
            EndPaint(hWnd, &empty);
        }
        return message == WM_ERASEBKGND ? 1 : 0;
    }
    if (message == WM_ERASEBKGND) {
        return FillRect(reinterpret_cast<HDC>(wParam), &rc, g_backgroundBrush) ? 1 : 0;
    }
    PAINTSTRUCT ps = {};
    HDC dc = message == WM_PAINT ? BeginPaint(hWnd, &ps) : reinterpret_cast<HDC>(wParam);
    if (!dc) return 0;
    FillRect(dc, &rc, g_backgroundBrush);
    const int mid = rc.top + (rc.bottom - rc.top) / 2;
    const RECT line = {rc.left, mid, rc.right, mid + 1};
    FillRect(dc, &line, LineBrush());
    if (message == WM_PAINT) EndPaint(hWnd, &ps);
    return 0;
}

void ThemeWizardDivider(HWND line) {
    WCHAR cls[16] = {};
    if (g_unloading || !g_active || !line || !OnOwningThread(line) || !IsWindow(line)) return;
    HWND sheet = GetParent(line);
    if (!sheet || !GetClassNameW(line, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"Static") ||
        !IsWizardSheetWindow(sheet)) return;
    // Match by shape: a thin, wide, text-less static directly on the sheet.
    RECT r = {};
    WCHAR text[4] = {};
    GetWindowTextW(line, text, ARRAYSIZE(text));
    const LONG_PTR kind = GetWindowLongPtrW(line, GWL_STYLE) & SS_TYPEMASK;
    if (!GetWindowRect(line, &r) || text[0] ||
        r.bottom - r.top > 4 || r.right - r.left < 40 ||
        kind == SS_ICON || kind == SS_BITMAP || kind == SS_OWNERDRAW) return;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        if (g_wizardDividers.count(line)) return;
        g_wizardDividers.insert(line);
    }
    if (SetWindowSubclass(line, WizardDividerSubclass, kAuxSubclassId, 0)) {
        RedrawWindow(line, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME);
        // The sheet does not clip its children. Repaint it and every child in
        // order once, so nothing drawn earlier (the footer buttons) stays covered.
        RedrawWindow(sheet, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
    } else {
        std::lock_guard<std::mutex> lock(g_stateLock);
        g_wizardDividers.erase(line);
    }
}

BOOL CALLBACK ThemeWizardDividerChild(HWND child, LPARAM sheetParam) {
    if (GetParent(child) != reinterpret_cast<HWND>(sheetParam)) return TRUE;
    ThemeWizardDivider(child);
    return TRUE;
}

// A newly created embedded web view paints a blank white page before its
// document has loaded. It has no class brush to swap, so it is made fully
// transparent (the dark host shows through) for a moment and then revealed.
bool IsInsideMmcView(HWND hWnd);
bool WindowClassEquals(HWND hWnd, PCWSTR expected);
std::unordered_map<HWND, bool> g_webCurtains;  // web views that are hidden for now

// Show a web view again after it was hidden with an empty window region.
void ShowWebView(HWND hWnd) {
    SetWindowRgn(hWnd, nullptr, TRUE);
    RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}

void RevealParentShellDocView(HWND ieWindow) {
    HWND parent = ieWindow ? GetParent(ieWindow) : nullptr;
    if (!parent || !IsWindow(parent)) return;
    bool hidden = false;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        hidden = g_shellDocCurtains.erase(parent) != 0;
    }
    if (hidden) {
        KillTimer(parent, kWebCurtainTimer);
        SetWindowRgn(parent, nullptr, TRUE);
    }
}

LRESULT CALLBACK WebCurtainSubclass(HWND hWnd, UINT message, WPARAM wParam,
                                    LPARAM lParam, UINT_PTR uIdSubclass,
                                    DWORD_PTR) {
    if (message == WM_NCDESTROY) {
        KillTimer(hWnd, kWebCurtainTimer);
        { std::lock_guard<std::mutex> lock(g_stateLock); g_webCurtains.erase(hWnd); }
        RemoveWindowSubclass(hWnd, WebCurtainSubclass, uIdSubclass);
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }
    if (message == WM_TIMER && wParam == kWebCurtainTimer) {
        KillTimer(hWnd, kWebCurtainTimer);
        RevealParentShellDocView(hWnd);
        ShowWebView(hWnd);
        return 0;
    }
    return DefSubclassProc(hWnd, message, wParam, lParam);
}

void CoverWebView(HWND hWnd) {
    WCHAR cls[32] = {};
    if (g_unloading || !g_active || !hWnd || !OnOwningThread(hWnd) ||
        !GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) || !IsInsideMmcView(hWnd)) return;
    if (wcscmp(cls, L"Shell DocObject View") == 0) {
        // Hide the newly created host until Internet Explorer_Server is ready (or
        // failsafe expires) so its 240,240,240 empty surface never flashes before
        // IE attaches.
        if (FindWindowExW(hWnd, nullptr, L"Internet Explorer_Server", nullptr)) return;
        {
            std::lock_guard<std::mutex> lock(g_stateLock);
            if (!g_shellViews.count(hWnd) || g_shellDocCurtains.count(hWnd)) return;
            g_shellDocCurtains.insert(hWnd);
        }
        HRGN empty = CreateRectRgn(0, 0, 0, 0);
        if (empty && !SetWindowRgn(hWnd, empty, TRUE)) DeleteObject(empty);
        SetTimer(hWnd, kWebCurtainTimer, kWebCurtainMs + 400, nullptr);
        if (HWND parent = GetParent(hWnd)) {
            RedrawWindow(parent, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
        }
        return;
    }
    if (wcscmp(cls, L"Internet Explorer_Server") != 0) return;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        if (g_webCurtains.count(hWnd)) return;
        g_webCurtains[hWnd] = true;
    }
    if (SetWindowSubclass(hWnd, WebCurtainSubclass, kAuxSubclassId, 0)) {
        // The embedded browser paints a blank white page before its document
        // loads. Clip it to an empty region (it still paints, but nothing is
        // shown, so the dark host remains visible) and reveal it after a moment.
        // A window region works for any window, unlike layered alpha on a child.
        HRGN empty = CreateRectRgn(0, 0, 0, 0);
        if (empty && !SetWindowRgn(hWnd, empty, FALSE)) DeleteObject(empty);  // the system owns it only on success
        SetTimer(hWnd, kWebCurtainTimer, kWebCurtainMs, nullptr);
    } else {
        std::lock_guard<std::mutex> lock(g_stateLock);
        g_webCurtains.erase(hWnd);
    }
}

BOOL CALLBACK ThemeWizardColourPage(HWND page, LPARAM sheetParam) {
    HWND sheet = reinterpret_cast<HWND>(sheetParam);
    if (g_unloading || !g_active || !OnOwningThread(page) || !IsWizardColourPage(page, sheet)) return TRUE;
    EnumChildWindows(page, ThemeWizardLabelChild, 0);
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        if (g_wizardColourPages.count(page) || g_wizardColourAdding.count(page)) return TRUE;
        g_wizardColourAdding.insert(page);
    }
    const BOOL added = SetWindowSubclass(page, WizardColourSubclass, kAuxSubclassId, 0);
    bool keep = false;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        g_wizardColourAdding.erase(page);
        if (added && g_active && !g_unloading && IsWindow(page)) { g_wizardColourPages.insert(page); keep = true; }
    }
    if (added && !keep && IsWindow(page)) {
        RemoveWindowSubclass(page, WizardColourSubclass, kAuxSubclassId);
    } else if (keep) {
        RedrawWindow(page, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }
    return TRUE;
}

void ThemeWizardColourPages(HWND sheet) {
    if (!g_unloading && g_active && sheet && OnOwningThread(sheet)) {
        EnumChildWindows(sheet, ThemeWizardDividerChild, reinterpret_cast<LPARAM>(sheet));
        EnumChildWindows(sheet, ThemeWizardColourPage, reinterpret_cast<LPARAM>(sheet));
    }
}

// Remove subclasses without holding the state lock across cross-thread calls.
constexpr wchar_t kOwnedModalThemeProperty[] = L"MMC.DarkMode.OwnedModalTheme";
constexpr wchar_t kTpmDialogPending[] = L"MMC Dark Mode: TPM dialog pending {61155E5A-B28E-4BA4-8E2D-32D9E5B3D982}";
constexpr PCWSTR kFormsFrameProperty =
    L"MMC Dark Mode: Forms frame {897DAD77-8CF1-4FA3-A246-306FBE37DDE9}";
constexpr PCWSTR kEventFrameRetryProperty =
    L"MMC Dark Mode: event frame retry {92D73E6D-03F4-4A64-9600-0FBA04C21D18}";
constexpr PCWSTR kSummaryPaintRetryProperty =
    L"MMC Dark Mode: summary paint guard {61654469-2476-4417-97E5-EE932A67C578}";

void ClearAllTrackedProps(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    RemovePropW(hWnd, kPaintListProperty);
    RemovePropW(hWnd, kOwnedModalThemeProperty);
    RemovePropW(hWnd, kTpmDialogPending);
    RemovePropW(hWnd, kFormsFrameProperty);
    RemovePropW(hWnd, kEventFrameRetryProperty);
    RemovePropW(hWnd, kSummaryPaintRetryProperty);
}

void RemoveSubclassesOnCurrentThread(const std::vector<HWND>& visitedWindows) {
    const DWORD tid = GetCurrentThreadId();
    const auto removeSet = [tid](std::unordered_set<HWND>& set, SUBCLASSPROC proc,
                                 bool webTeardown) {
        std::vector<HWND> mine;
        {
            std::lock_guard<std::mutex> lock(g_stateLock);
            for (auto it = set.begin(); it != set.end();) {
                HWND w = *it;
                if (!IsWindow(w)) {
                    it = set.erase(it);
                } else if (GetWindowThreadProcessId(w, nullptr) == tid) {
                    mine.push_back(w);
                    it = set.erase(it);
                } else {
                    ++it;
                }
            }
        }
        for (HWND w : mine) {
            if (webTeardown) {
                KillTimer(w, kWebCurtainTimer);
                {
                    std::lock_guard<std::mutex> lock(g_stateLock);
                    g_shellDocCurtains.erase(w);
                }
                ShowWebView(w);
            }
            RemoveWindowSubclass(w, proc, kAuxSubclassId);
            ClearAllTrackedProps(w);
        }
    };

    removeSet(g_shellViews, ShellViewSubclass, true);
    {
        std::vector<HWND> curtains;
        {
            std::lock_guard<std::mutex> lock(g_stateLock);
            for (auto it = g_webCurtains.begin(); it != g_webCurtains.end();) {
                HWND w = it->first;
                if (!IsWindow(w)) {
                    it = g_webCurtains.erase(it);
                } else if (GetWindowThreadProcessId(w, nullptr) == tid) {
                    curtains.push_back(w);
                    it = g_webCurtains.erase(it);
                } else {
                    ++it;
                }
            }
        }
        for (HWND w : curtains) {
            KillTimer(w, kWebCurtainTimer);
            RevealParentShellDocView(w);
            ShowWebView(w);
            RemoveWindowSubclass(w, WebCurtainSubclass, kAuxSubclassId);
            ClearAllTrackedProps(w);
        }
    }
    removeSet(g_wizardDividers, WizardDividerSubclass, false);
    removeSet(g_wizardLabels, WizardLabelSubclass, false);
    removeSet(g_wizardColourPages, WizardColourSubclass, false);

    std::vector<HWND> mine;
    std::vector<HWND> styleWindows;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        for (auto it = g_subclassed.begin(); it != g_subclassed.end();) {
            HWND w = *it;
            if (!IsWindow(w)) {
                g_styleFixes.erase(w);
                it = g_subclassed.erase(it);
            } else if (GetWindowThreadProcessId(w, nullptr) == tid) {
                mine.push_back(w);
                it = g_subclassed.erase(it);
            } else {
                ++it;
            }
        }
        for (HWND w : visitedWindows) {
            if (IsWindow(w) && GetWindowThreadProcessId(w, nullptr) == tid) {
                g_subclassed.erase(w);
            }
        }
        for (auto it = g_styleFixes.begin(); it != g_styleFixes.end();) {
            HWND w = it->first;
            if (!IsWindow(w)) {
                it = g_styleFixes.erase(it);
            } else {
                if (GetWindowThreadProcessId(w, nullptr) == tid) {
                    styleWindows.push_back(w);
                }
                ++it;
            }
        }
    }
    for (HWND w : styleWindows) {
        RestoreRemovedStyles(w);
    }
    for (HWND w : mine) {
        RemoveWindowSubclass(w, ModSubclassProc, kModSubclassId);
        ClearAllTrackedProps(w);
    }
    for (HWND w : visitedWindows) {
        if (IsWindow(w) && GetWindowThreadProcessId(w, nullptr) == tid) {
            RemoveWindowSubclass(w, ModSubclassProc, kModSubclassId);
            ClearAllTrackedProps(w);
        }
    }
}

void RestoreRemovedStyles(HWND hWnd);

void RemoveAllSubclasses() {
    std::unordered_set<HWND> shellViews;
    std::unordered_set<HWND> docCurtains;
    std::unordered_map<HWND, bool> webCurtains;
    std::unordered_set<HWND> dividers;
    std::unordered_set<HWND> labels;
    std::unordered_set<HWND> colourPages;
    std::unordered_set<HWND> subclassed;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        shellViews.swap(g_shellViews);
        docCurtains.swap(g_shellDocCurtains);
        webCurtains.swap(g_webCurtains);
        dividers.swap(g_wizardDividers);
        labels.swap(g_wizardLabels);
        colourPages.swap(g_wizardColourPages);
        g_wizardColourAdding.clear();
        subclassed.swap(g_subclassed);
        g_adding.clear();
        g_styleFixes.clear();
        g_pendingThreads.clear();
    }
    const DWORD tid = GetCurrentThreadId();
    for (HWND w : shellViews) {
        if (!IsWindow(w)) continue;
        KillTimer(w, kWebCurtainTimer);
        ShowWebView(w);
        if (GetWindowThreadProcessId(w, nullptr) == tid) {
            RemoveWindowSubclass(w, ShellViewSubclass, kAuxSubclassId);
        }
        ClearAllTrackedProps(w);
    }
    for (HWND w : docCurtains) {
        if (!IsWindow(w)) continue;
        KillTimer(w, kWebCurtainTimer);
        ShowWebView(w);
    }
    for (const auto& entry : webCurtains) {
        HWND w = entry.first;
        if (!IsWindow(w)) continue;
        KillTimer(w, kWebCurtainTimer);
        RevealParentShellDocView(w);
        ShowWebView(w);
        if (GetWindowThreadProcessId(w, nullptr) == tid) {
            RemoveWindowSubclass(w, WebCurtainSubclass, kAuxSubclassId);
        }
        ClearAllTrackedProps(w);
    }
    const auto cleanupAux = [tid](const std::unordered_set<HWND>& set, SUBCLASSPROC proc) {
        for (HWND w : set) {
            if (!IsWindow(w)) continue;
            if (GetWindowThreadProcessId(w, nullptr) == tid) {
                RemoveWindowSubclass(w, proc, kAuxSubclassId);
            }
            ClearAllTrackedProps(w);
        }
    };
    cleanupAux(dividers, WizardDividerSubclass);
    cleanupAux(labels, WizardLabelSubclass);
    cleanupAux(colourPages, WizardColourSubclass);
    for (HWND w : subclassed) {
        if (!IsWindow(w)) continue;
        RestoreRemovedStyles(w);
        if (GetWindowThreadProcessId(w, nullptr) == tid) {
            RemoveWindowSubclass(w, ModSubclassProc, kModSubclassId);
        }
        ClearAllTrackedProps(w);
    }
}

// A modal dialog can be created by a path that bypasses the modeless hooks.
// WM_ENTERIDLE supplies the modal HWND once it has finished initialization.
void ThemeOwnedModalOnIdle(HWND owner, WPARAM reason, LPARAM detail) {
    if (!g_active || reason != MSGF_DIALOGBOX || !detail) return;
    HWND modal = reinterpret_cast<HWND>(detail);
    if (modal == owner || !IsWindow(modal) || !OnOwningThread(modal) ||
        GetWindow(modal, GW_OWNER) != owner) return;
    DWORD pid = 0;
    GetWindowThreadProcessId(modal, &pid);
    WCHAR cls[16] = {};
    if (pid != GetCurrentProcessId() ||
        !GetClassNameW(modal, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"#32770")) return;
    HANDLE marker = GetPropW(modal, kOwnedModalThemeProperty);
    if (marker) {
        if (marker == reinterpret_cast<HANDLE>(2)) ThemeWizardColourPages(modal);
        return;
    }
    // Distinguish in-progress theming from a completed pass during reentry.
    if (!SetPropW(modal, kOwnedModalThemeProperty, reinterpret_cast<HANDLE>(1))) return;
    ThemeDialog(modal);
    if (IsWindow(modal)) {
        if (!g_active || !IsSubclassed(modal) ||
            !SetPropW(modal, kOwnedModalThemeProperty, reinterpret_cast<HANDLE>(2))) {
            RemovePropW(modal, kOwnedModalThemeProperty);
        } else {
            ThemeWizardColourPages(modal);
        }
    }
}

// Only replace a completely blank 49-pixel Wizard97 header bitmap;
// preserve every bitmap that contains visible artwork.
thread_local HWND g_wizardPaintWindow = nullptr;
// Restrict the footer brush to the native message-box painter and dialog DC.
// If private symbols or unwind metadata are unavailable, keep native drawing.
#if defined(_WIN64)
using FooterRectangleFn = BOOL (WINAPI*)(HDC, int, int, int, int);
using FooterLookupFn = PRUNTIME_FUNCTION (WINAPI*)(DWORD64, PDWORD64, PUNWIND_HISTORY_TABLE);
static FooterRectangleFn g_footerRectangleOriginal = nullptr;
static FooterLookupFn g_footerLookup = nullptr;
static void* g_footerPainterStart = nullptr;
static thread_local HWND g_footerPaintDialog = nullptr;

static bool FooterCallerMatches(void* caller) {
    if (!caller || !g_footerPainterStart || !g_footerLookup) return false;
    DWORD64 base = 0;
    PRUNTIME_FUNCTION fn = g_footerLookup(reinterpret_cast<DWORD64>(caller), &base, nullptr);
    return fn && base + fn->BeginAddress == reinterpret_cast<DWORD64>(g_footerPainterStart);
}

static BOOL WINAPI FooterRectangleHook(HDC dc, int left, int top, int right, int bottom) {
    const DWORD incomingError = GetLastError();
    HWND dialog = g_footerPaintDialog;
    if (g_active && g_backgroundBrush && dc && dialog &&
        WindowFromDC(dc) == dialog && FooterCallerMatches(__builtin_return_address(0))) {
        LOGBRUSH nativeBrush = {};
        HGDIOBJ currentBrush = GetCurrentObject(dc, OBJ_BRUSH);
        if (currentBrush && GetObjectW(currentBrush, sizeof(nativeBrush), &nativeBrush) == sizeof(nativeBrush) &&
            nativeBrush.lbStyle == BS_SOLID &&
            (nativeBrush.lbColor == RealSystemColour(COLOR_BTNFACE) ||
             nativeBrush.lbColor == RealSystemColour(COLOR_WINDOW) ||
             nativeBrush.lbColor == RealSystemColour(COLOR_MENU))) {
            HGDIOBJ oldBrush = SelectObject(dc, g_backgroundBrush);
            if (oldBrush && oldBrush != HGDI_ERROR) {
                SetLastError(incomingError);
                BOOL result = g_footerRectangleOriginal(dc, left, top, right, bottom);
                DWORD resultError = GetLastError();
                SelectObject(dc, oldBrush);
                SetLastError(resultError);
                return result;
            }
        }
    }
    SetLastError(incomingError);
    return g_footerRectangleOriginal(dc, left, top, right, bottom);
}
#endif

using BitBlt_t = decltype(&BitBlt);
BitBlt_t pBitBlt = nullptr;

bool IsCommonControlsCaller(void* address) {
    HMODULE module = nullptr;
    WCHAR path[MAX_PATH] = {};
    if (!address || !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(address), &module)) return false;
    const DWORD len = GetModuleFileNameW(module, path, ARRAYSIZE(path));
    if (!len || len >= ARRAYSIZE(path)) return false;
    const WCHAR* base = wcsrchr(path, L'\\');
    return _wcsicmp(base ? base + 1 : path, L"comctl32.dll") == 0;
}

bool IsBlankWizardHeaderBlit(HDC dst, int x, int y, int width, int height,
                             HDC src, int srcX, int srcY, DWORD rop) {
    HWND sheet = g_wizardPaintWindow;
    if (!g_active || !sheet || !IsWindow(sheet) || !g_backgroundBrush ||
        !dst || !src || width != 49 || height != 49 || srcX || srcY ||
        rop != SRCCOPY || GetObjectType(src) != OBJ_MEMDC ||
        WindowFromDC(dst) != sheet || GetMapMode(dst) != MM_TEXT ||
        GetMapMode(src) != MM_TEXT || GetGraphicsMode(dst) != GM_COMPATIBLE ||
        GetGraphicsMode(src) != GM_COMPATIBLE) return false;
    POINT origin = {}, viewport = {}, sourceOrigin = {}, sourceViewport = {};
    if (!GetWindowOrgEx(dst, &origin) || !GetViewportOrgEx(dst, &viewport) ||
        !GetWindowOrgEx(src, &sourceOrigin) || !GetViewportOrgEx(src, &sourceViewport) ||
        origin.x || origin.y || viewport.x || viewport.y || sourceOrigin.x ||
        sourceOrigin.y || sourceViewport.x || sourceViewport.y) return false;
    WCHAR sheetClass[16] = {}, lineClass[16] = {};
    HWND line = GetDlgItem(sheet, 12327);
    if (!line || !IsWindowVisible(line) || GetParent(line) != sheet ||
        !GetClassNameW(sheet, sheetClass, ARRAYSIZE(sheetClass)) ||
        wcscmp(sheetClass, L"#32770") ||
        !GetClassNameW(line, lineClass, ARRAYSIZE(lineClass)) ||
        wcscmp(lineClass, L"Static")) return false;
    RECT client = {}, lr = {};
    if (!GetClientRect(sheet, &client) || !GetWindowRect(line, &lr)) return false;
    POINT linePoint = {lr.left, lr.top};
    if (!ScreenToClient(sheet, &linePoint)) return false;
    // The square must fit entirely in the right-hand header above its separator.
    if (client.right <= 0 || x < client.right * 2 / 3 || y < 0 ||
        x > client.right - width || y > linePoint.y - height) return false;
    BITMAP bm = {};
    HGDIOBJ bitmap = GetCurrentObject(src, OBJ_BITMAP);
    if (!bitmap || GetObjectW(bitmap, sizeof(bm), &bm) != sizeof(bm) ||
        bm.bmWidth != width || bm.bmHeight != height) return false;
    for (int py = 0; py < height; ++py) {
        for (int px = 0; px < width; ++px) {
            if (GetPixel(src, px, py) != RGB(255, 255, 255)) return false;
        }
    }
    return g_active && IsWindow(sheet);
}

BOOL WINAPI WizardBitBltHook(HDC dst, int x, int y, int width, int height,
                             HDC src, int srcX, int srcY, DWORD rop) {
    if (g_wizardPaintWindow && width == 49 && height == 49 &&
        IsCommonControlsCaller(__builtin_return_address(0)) &&
        IsBlankWizardHeaderBlit(dst, x, y, width, height, src, srcX, srcY, rop)) {
        const RECT target = {x, y, x + width, y + height};
        if (FillRect(dst, &target, g_backgroundBrush)) return TRUE;
    }
    return pBitBlt ? pBitBlt(dst, x, y, width, height, src, srcX, srcY, rop) : FALSE;
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
    if (message == WM_ENTERIDLE) {
        ThemeOwnedModalOnIdle(hWnd, wParam, lParam);
    }

    if (message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE && IsWindow(hWnd)) {
        HWND popup = GetWindow(hWnd, GW_ENABLEDPOPUP);
        if (popup && popup != hWnd) {
            ThemeDialog(popup);
        }
        // GW_ENABLEDPOPUP can still be null while the incoming dialog activates.
        HWND incoming = reinterpret_cast<HWND>(lParam);
        if (incoming && incoming != popup) {
            ThemeOwnedModalOnIdle(hWnd, MSGF_DIALOGBOX, lParam);
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
                const bool isDown =
                    (custom->uItemState & (CDIS_SELECTED | CDIS_CHECKED)) != 0;
                if ((isHot || isDown) && custom->hdc) {
                    if (!g_hoverBrush) {
                        g_hoverBrush = CreateSolidBrush(kColorHover);
                    }
                    HBRUSH fill = isDown ? PressedBrush() : g_hoverBrush;
                    if (fill) {
                        FillRect(custom->hdc, &custom->rc, fill);
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
thread_local TabEntry g_tabEntries[8] = {};

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
        if (!g_tabEntries[i].hWnd || !IsWindow(g_tabEntries[i].hWnd)) {
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
                if (HBRUSH accent = AccentBrush()) {
                    RECT bar = {tab.left, tab.top, tab.right,
                                tab.top + ScaleForWindow(hWnd, 2)};
                    FillRect(dc, &bar, accent);
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
            // `entry` may have been null before the refresh; PaintTabStrip looks it up again.
            if (!RefreshTabEntry(hWnd)) {
                return DefSubclassProc(hWnd, message, wParam, lParam);
            }
        }
        PaintTabStrip(hWnd);
        return 0;
    }

    // Mouse and keyboard input, plus selection and item changes made from code.
    if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP ||
        message == WM_KEYUP || message == TCM_SETCURSEL ||
        message == TCM_INSERTITEMW || message == TCM_INSERTITEMA ||
        message == TCM_DELETEITEM || message == TCM_DELETEALLITEMS ||
        message == TCM_SETITEMW || message == TCM_SETITEMA) {
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
thread_local HWND g_actionsPanePaint = nullptr;
thread_local bool g_drawingMmcDescription = false;
thread_local bool g_drawingActionsContent = false;
thread_local bool g_drawingSummaryTitle = false;
thread_local HWND g_drawingSummaryButton = nullptr;
thread_local HWND g_drawingPushButton = nullptr;
thread_local HWND g_drawingChoiceButton = nullptr;
thread_local bool g_pushButtonFaceDrawn = false;
thread_local bool g_pushButtonFaceDisabled = false;
// Set before activation; never enable partial push-button recolouring.
bool g_pushButtonThemeHooksReady = false;

struct FormsButtonPaint {
    HWND window;
    bool faceDrawn;
    bool disabled;
};
thread_local FormsButtonPaint* g_formsButtonPaint = nullptr;
bool g_formsButtonHooksReady = false;

// Pair a native disabled button edge and face without replacing its text or input.
struct ClassicPushPair {
    HWND button;
    HDC dc;
    RECT interior;
    RECT exterior;
    bool armed;
    bool framePending;
};

bool IsDisabledDialogPush(HWND h, HDC dc, const RECT& original) {
    if (!h || !dc || !IsWindow(h) || IsWindowEnabled(h) || WindowFromDC(dc) != h) return false;
    WCHAR cls[16] = {}, parentCls[16] = {};
    HWND parent = GetParent(h);
    LONG_PTR style = GetWindowLongPtrW(h, GWL_STYLE);
    LONG_PTR kind = style & BS_TYPEMASK;
    RECT client = {};
    POINT vp = {}, org = {};
    return parent && (style & WS_CHILD) &&
        (kind == BS_PUSHBUTTON || kind == BS_DEFPUSHBUTTON) &&
        !(style & (BS_ICON | BS_BITMAP)) &&
        GetClassNameW(h, cls, ARRAYSIZE(cls)) && !wcscmp(cls, L"Button") &&
        GetClassNameW(parent, parentCls, ARRAYSIZE(parentCls)) && !wcscmp(parentCls, L"#32770") &&
        GetClientRect(h, &client) && EqualRect(&client, &original) &&
        client.right >= 8 && client.bottom >= 8 && client.right <= 4096 && client.bottom <= 4096 &&
        GetMapMode(dc) == MM_TEXT && GetLayout(dc) == 0 &&
        GetViewportOrgEx(dc, &vp) && !vp.x && !vp.y &&
        GetWindowOrgEx(dc, &org) && !org.x && !org.y;
}

// Called after the native DrawEdge. Does not change its result or adjusted RECT.
void RecordClassicPushEdge(ClassicPushPair& pair, HDC dc, RECT original, RECT adjusted,
                           UINT edge, UINT flags, bool nativeSucceeded, bool callerIsUser32) {
    pair.armed = false;
    pair.framePending = false;
    constexpr UINT required = BF_RECT | BF_ADJUST;
    constexpr UINT supported = required | BF_SOFT | BF_FLAT | BF_MONO;
    if (!nativeSucceeded || !callerIsUser32 || !g_active || !g_lineBrush ||
        (edge != EDGE_RAISED && edge != EDGE_SUNKEN) ||
        (flags & required) != required || (flags & ~supported) ||
        !IsDisabledDialogPush(pair.button, dc, original)) return;
    const LONG left = adjusted.left - original.left, top = adjusted.top - original.top;
    const LONG right = original.right - adjusted.right, bottom = original.bottom - adjusted.bottom;
    if (left < 1 || top < 1 || right < 1 || bottom < 1 ||
        left > 16 || top > 16 || right > 16 || bottom > 16 ||
        adjusted.left >= adjusted.right || adjusted.top >= adjusted.bottom) return;
    // Do not paint until the matching native fill actually occurs.
    pair.dc = dc;
    pair.interior = adjusted;
    pair.exterior = original;
    pair.armed = true;
}

// Called immediately before a FillRect. Valid only for the specific native pair.
HBRUSH ClassicPushFillBrush(ClassicPushPair& pair, HDC dc, const RECT* rect,
                            HBRUSH original, bool callerIsUser32) {
    if (!pair.armed || !rect || !callerIsUser32 || !g_active || !g_backgroundBrush ||
        pair.dc != dc || !EqualRect(rect, &pair.interior) ||
        !IsWindow(pair.button) || IsWindowEnabled(pair.button) || WindowFromDC(dc) != pair.button)
        return original;
    pair.armed = false;
    pair.framePending = true;
    return g_backgroundBrush;
}

void FinishClassicPushFill(ClassicPushPair& pair, bool nativeSucceeded) {
    const bool pending = pair.framePending;
    pair.framePending = false;
    if (!pending || !nativeSucceeded || !g_active || !g_lineBrush ||
        !IsDisabledDialogPush(pair.button, pair.dc, pair.exterior)) return;
    HRGN outer = CreateRectRgnIndirect(&pair.exterior);
    HRGN inner = CreateRectRgnIndirect(&pair.interior);
    if (outer && inner && CombineRgn(outer, outer, inner, RGN_DIFF) != ERROR)
        FillRgn(pair.dc, outer, g_lineBrush);
    if (inner) DeleteObject(inner);
    if (outer) DeleteObject(outer);
}

thread_local ClassicPushPair* g_classicPushPair = nullptr;
using FillRect_t = decltype(&FillRect);
FillRect_t pFillRect = nullptr;
bool g_classicPushHooksReady = false;

bool IsUser32PaintCaller(void* address) {
    HMODULE caller = nullptr;
    return address && GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
               reinterpret_cast<LPCWSTR>(address), &caller) &&
           caller == GetModuleHandleW(L"user32.dll");
}

HBRUSH MappedPseudoBrush(HBRUSH brush);
bool IsNativePaleBrush(HDC dc, HGDIOBJ brush);
bool IsLightClassBrush(ULONG_PTR brush);

bool IsActionsPaneDc(HDC dc) {
    if (!dc || !g_actionsPanePaint) return false;
    HWND owner = WindowFromDC(dc);
    return !owner || owner == g_actionsPanePaint;
}

bool IsLightSolidColour(COLORREF colour) {
    if (colour == CLR_INVALID) return false;
    const int r = GetRValue(colour), g = GetGValue(colour), b = GetBValue(colour);
    const COLORREF pure = colour & 0x00ffffffu;
    return (r >= 200 && g >= 200 && b >= 200) ||
           pure == (RealSystemColour(5) & 0x00ffffffu) ||   // COLOR_WINDOW
           pure == (RealSystemColour(15) & 0x00ffffffu);    // COLOR_BTNFACE
}

bool IsLightSolidBrush(HDC dc, HGDIOBJ brush) {
    if (!brush) return false;
    const ULONG_PTR raw = reinterpret_cast<ULONG_PTR>(brush);
    if (raw <= 32) {
        return IsLightClassBrush(raw);
    }
    if (brush == GetStockObject(WHITE_BRUSH) ||
        brush == GetStockObject(LTGRAY_BRUSH)) {
        return true;
    }
    COLORREF colour = CLR_INVALID;
    if (brush == GetStockObject(DC_BRUSH)) {
        colour = GetDCBrushColor(dc);
    } else {
        LOGBRUSH lb = {};
        if (GetObjectW(brush, sizeof(lb), &lb) == sizeof(lb) &&
            lb.lbStyle == BS_SOLID) {
            colour = lb.lbColor;
        }
    }
    return IsLightSolidColour(colour);
}

int WINAPI FillRect_Hook(HDC dc, const RECT* rect, HBRUSH brush) {
    if (!pFillRect) return 0;
    if (g_active && g_lineBrush && dc && IsNativePaleBrush(dc, brush)) {
        return pFillRect(dc, rect, g_lineBrush);
    }
    if (g_active && g_backgroundBrush && IsActionsPaneDc(dc) &&
        IsLightSolidBrush(dc, brush)) {
        return pFillRect(dc, rect, g_backgroundBrush);
    }
    ClassicPushPair* pair = g_classicPushPair;
    if (!g_classicPushHooksReady || !pair || !pair->armed) {
        // Callers often pass (COLOR_WINDOW + 1) instead of a brush. Windows resolves
        // that internally, past GetSysColorBrush, so it stays light unless mapped.
        HBRUSH use = MappedPseudoBrush(brush);
        if (use == brush && g_active && g_backgroundBrush && dc &&
            brush == static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH))) {
            // A host painting its wait state with the stock white brush.
            HWND owner = WindowFromDC(dc);
            if (owner && IsInsideMmcView(owner)) use = g_backgroundBrush;
        }
        return pFillRect(dc, rect, use);
    }
    DWORD previousError = GetLastError();
    HBRUSH replacement = ClassicPushFillBrush(*pair, dc, rect, brush,
        IsUser32PaintCaller(__builtin_return_address(0)));
    SetLastError(previousError);
    int result = pFillRect(dc, rect, replacement);
    DWORD nativeError = GetLastError();
    if (pair->framePending) FinishClassicPushFill(*pair, result != 0);
    SetLastError(nativeError);
    return result;
}

BOOL WINAPI DrawEdge_Hook(HDC dc, LPRECT rect, UINT edge, UINT flags) {
    if (!pDrawEdge) {
        return FALSE;
    }
    if (g_classicPushHooksReady && g_classicPushPair && dc && rect) {
        DWORD previousError = GetLastError();
        bool eligible = IsUser32PaintCaller(__builtin_return_address(0)) &&
                        IsDisabledDialogPush(g_classicPushPair->button, dc, *rect);
        SetLastError(previousError);
        if (eligible) {
            RECT original = *rect;
            BOOL result = pDrawEdge(dc, rect, edge, flags);
            DWORD nativeError = GetLastError();
            RecordClassicPushEdge(*g_classicPushPair, dc, original, *rect,
                                  edge, flags, result != FALSE, true);
            SetLastError(nativeError);
            return result;
        }
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
    if (g_active && dc &&
        (dc == g_actionsEraseDc || IsActionsPaneDc(dc)) &&
        rect && g_backgroundBrush && g_lineBrush) {
        // The Actions caption is outlined with a 3D edge, which shows as white and
        // grey lines. Draw one dark line on each requested side instead.
        RECT r = *rect;
        if (flags & BF_MIDDLE) FillRect(dc, &r, g_backgroundBrush);
        const RECT sides[4] = {
            {r.left, r.top, r.left + 1, r.bottom},        // BF_LEFT
            {r.left, r.top, r.right, r.top + 1},          // BF_TOP
            {r.right - 1, r.top, r.right, r.bottom},      // BF_RIGHT
            {r.left, r.bottom - 1, r.right, r.bottom}};   // BF_BOTTOM
        const UINT bits[4] = {BF_LEFT, BF_TOP, BF_RIGHT, BF_BOTTOM};
        for (int i = 0; i < 4; ++i) {
            if ((flags & bits[i]) == bits[i]) {
                FillRect(dc, &sides[i], g_lineBrush);
                // Cover the second pixel of a two-pixel 3D edge.
                RECT inner = sides[i];
                if (i == 0) { inner.left += 1; inner.right += 1; }
                else if (i == 1) { inner.top += 1; inner.bottom += 1; }
                else if (i == 2) { inner.left -= 1; inner.right -= 1; }
                else { inner.top -= 1; inner.bottom -= 1; }
                FillRect(dc, &inner, g_backgroundBrush);
            }
        }
        if (flags & BF_ADJUST) InflateRect(rect, -2, -2);
        return TRUE;
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
                if (SubclassWith(hWnd, WindowKind::ActionsPane)) {
                    InvalidateRect(hWnd, nullptr, TRUE);
                }
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

// Leave self-rendered web content and hosts to their own handlers.
bool IsHtmlHost(HWND hWnd) {
    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    if (wcscmp(className, L"Shell Embedding") == 0 ||
        wcscmp(className, L"Shell DocObject View") == 0) {
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

bool WindowClassEquals(HWND hWnd, PCWSTR expected) {
    WCHAR name[128] = {};
    return hWnd && GetClassNameW(hWnd, name, ARRAYSIZE(name)) &&
           wcscmp(name, expected) == 0;
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

// WinForms combo boxes (Task Scheduler pages) that use the native drawing:
// drop-down lists and editable drop-downs, but not owner-drawn ones. The nesting
// depth differs from page to page and is not checked.
bool IsMmcSummaryCombo(HWND hWnd) {
    if (!WindowClassStarts(hWnd, L"WindowsForms10.COMBOBOX.")) {
        return false;
    }
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR type = style & 3;
    return (type == CBS_DROPDOWNLIST || type == CBS_DROPDOWN) &&
           !(style & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE));
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

// Any WinForms window in this process. The parent chain cannot be used: the
// page that is selected first is built under a parking window and only moved
// into the snap-in view afterwards.
bool IsHostedWinFormsWindow(HWND hWnd) {
    return WindowClassStarts(hWnd, L"WindowsForms10.");
}

// Plain text push buttons: native, or the WinForms wrapper. Owner-drawn buttons
// and the other button types keep their own drawing.
bool IsTextPushButton(HWND hWnd) {
    WCHAR cls[32] = {};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    if (wcscmp(cls, L"Button") != 0 &&
        !WindowClassStarts(hWnd, L"WindowsForms10.BUTTON.")) {
        return false;
    }
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR type = style & BS_TYPEMASK;
    return (type == BS_PUSHBUTTON || type == BS_DEFPUSHBUTTON) &&
           !(style & (BS_ICON | BS_BITMAP));
}

bool IsDialogChoiceButton(HWND hWnd) {
    WCHAR cls[16] = {}, parentClass[16] = {};
    HWND parent = GetParent(hWnd);
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR type = style & BS_TYPEMASK;
    return (style & WS_CHILD) && parent &&
           GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && !wcscmp(cls, L"Button") &&
           GetClassNameW(parent, parentClass, ARRAYSIZE(parentClass)) &&
           !wcscmp(parentClass, L"#32770") &&
           !(style & (BS_PUSHLIKE | BS_ICON | BS_BITMAP)) &&
           (type == BS_CHECKBOX || type == BS_AUTOCHECKBOX || type == BS_3STATE ||
            type == BS_AUTO3STATE || type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON);
}

// WinForms uses owner-draw for push buttons and other controls alike. The
// visual-style part, not this window style alone, identifies a push face.
bool IsFormsOwnerDrawButtonCandidate(HWND hWnd) {
    if (!WindowClassStarts(hWnd, L"WindowsForms10.BUTTON.") ||
        (GetWindowLongPtrW(hWnd, GWL_STYLE) & BS_TYPEMASK) != BS_OWNERDRAW) {
        return false;
    }
    HWND parent = GetParent(hWnd);
    for (int depth = 0; parent && depth < 24; ++depth) {
        const bool formsWindow = WindowClassStarts(parent, L"WindowsForms10.Window.");
        if (formsWindow && GetAncestor(parent, GA_ROOT) == parent) {
            return true;
        }
        if (!formsWindow && !WindowClassStarts(parent, L"WindowsForms10.SysTabControl32.")) {
            return false;
        }
        parent = GetParent(parent);
    }
    return false;
}

// The embedded task preview is nested inside a Forms tab and an MMC OCX
// host, rather than a top-level Forms dialog. Fail closed on other ancestry.
bool IsHostedFormsReadOnlyEdit(HWND edit) {
    if (!WindowClassEquals(GetAncestor(edit, GA_ROOT), L"MMCMainFrame")) return false;
    HWND parent = GetParent(edit);
    int windows = 0, tabs = 0, depth = 0;
    while (parent && depth < 20) {
        if (WindowClassStarts(parent, L"WindowsForms10.Window.")) {
            ++windows;
        } else if (WindowClassStarts(parent, L"WindowsForms10.SysTabControl32.")) {
            ++tabs;
        } else {
            break;
        }
        ++depth;
        parent = GetParent(parent);
    }
    if (depth == 20 || windows < 2 || tabs != 1 ||
        !WindowClassStarts(parent, L"ATL:")) return false;
    parent = GetParent(parent);
    if (!WindowClassEquals(parent, L"AtlAxWinEx")) return false;
    parent = GetParent(parent);
    if (!WindowClassEquals(parent, L"MMCOCXViewWindow")) return false;
    return WindowClassEquals(GetParent(parent), L"MMCViewWindow");
}

// WinForms TextBox uses a native EDIT window and forwards its colour request
// through the Forms parent. Restrict the override to enabled read-only
// fields in a Forms dialog or the narrow hosted preview, not password edits.
bool IsFormsReadOnlyEdit(HWND parent, HWND edit) {
    if (!parent || !edit || GetParent(edit) != parent || !IsWindowEnabled(edit) ||
        !WindowClassStarts(parent, L"WindowsForms10.Window.") ||
        !WindowClassStarts(edit, L"WindowsForms10.EDIT.")) {
        return false;
    }
    const LONG_PTR style = GetWindowLongPtrW(edit, GWL_STYLE);
    HWND root = GetAncestor(edit, GA_ROOT);
    return (style & ES_READONLY) && !(style & ES_PASSWORD) && root &&
           (WindowClassStarts(root, L"WindowsForms10.Window.") ||
            IsHostedFormsReadOnlyEdit(edit));
}

// Preserve custom brushes and unusual foreground colours; only replace the
// standard light WinForms edit palette after its own colour handler has run.
bool ColourFormsReadOnlyEdit(HDC dc, HBRUSH nativeBrush) {
    if (!dc || !nativeBrush || !g_backgroundBrush) return false;
    LOGBRUSH brush = {};
    if (GetObjectW(nativeBrush, sizeof(brush), &brush) != static_cast<int>(sizeof(brush)) ||
        brush.lbStyle != BS_SOLID) return false;
    const COLORREF window = RealSystemColour(COLOR_WINDOW);
    const COLORREF face = RealSystemColour(COLOR_BTNFACE);
    if (brush.lbColor != window && brush.lbColor != face) return false;
    const COLORREF text = GetTextColor(dc);
    if (text == CLR_INVALID ||
        (text != RealSystemColour(COLOR_WINDOWTEXT) &&
         text != RealSystemColour(COLOR_BTNTEXT) && text != kColorText)) return false;
    const COLORREF oldText = SetTextColor(dc, kColorText);
    if (oldText == CLR_INVALID) return false;
    const COLORREF oldBk = SetBkColor(dc, kColorBackground);
    if (oldBk == CLR_INVALID) {
        SetTextColor(dc, oldText);
        return false;
    }
    if (!SetBkMode(dc, OPAQUE)) {
        SetTextColor(dc, oldText);
        SetBkColor(dc, oldBk);
        return false;
    }
    return true;
}

// An icon or bitmap can be assigned after creation; its glyph is drawn dark and
// would vanish on the dark face, so such buttons are left alone.
bool ButtonHasImage(HWND hWnd) {
    return SendToControl(hWnd, BM_GETIMAGE, IMAGE_ICON, 0) != 0 ||
           SendToControl(hWnd, BM_GETIMAGE, IMAGE_BITMAP, 0) != 0;
}

// Limit writable-field handling to the multiline Forms edit in a Forms dialog.
// Do not alter password, read-only, custom-colour or unrelated native edits.
bool IsFormsWritableMultilineEdit(HWND parent, HWND edit) {
    if (!parent || !edit || GetParent(edit) != parent || !IsWindowEnabled(edit) ||
        !WindowClassStarts(parent, L"WindowsForms10.Window.") ||
        !WindowClassStarts(edit, L"WindowsForms10.EDIT.")) return false;
    const LONG_PTR style = GetWindowLongPtrW(edit, GWL_STYLE);
    HWND root = GetAncestor(edit, GA_ROOT);
    return root && WindowClassStarts(root, L"WindowsForms10.Window.") &&
           (style & ES_MULTILINE) && !(style & (ES_PASSWORD | ES_READONLY));
}

// Apply the same native-first, stock-colour handling to single-line fields.
// Keep passwords and custom palettes native and do not change control styles.
bool IsFormsWritableSingleLineEdit(HWND parent, HWND edit) {
    if (!parent || !edit || GetParent(edit) != parent || !IsWindowEnabled(edit) ||
        !WindowClassStarts(parent, L"WindowsForms10.Window.") ||
        !WindowClassStarts(edit, L"WindowsForms10.EDIT.")) return false;
    const LONG_PTR style = GetWindowLongPtrW(edit, GWL_STYLE);
    HWND root = GetAncestor(edit, GA_ROOT);
    return root && WindowClassStarts(root, L"WindowsForms10.Window.") &&
           !(style & (ES_MULTILINE | ES_PASSWORD | ES_READONLY));
}

// A disabled native edit uses system gray text even if the parent sets a
// different text colour. Only replace a stock light background when that
// system gray remains legible against this fixed dark palette.
bool IsFormsDisabledEdit(HWND parent, HWND edit) {
    if (!parent || !edit || GetParent(edit) != parent || IsWindowEnabled(edit) ||
        !WindowClassStarts(parent, L"WindowsForms10.Window.") ||
        !WindowClassStarts(edit, L"WindowsForms10.EDIT.")) return false;
    HWND root = GetAncestor(edit, GA_ROOT);
    return root && WindowClassStarts(root, L"WindowsForms10.Window.") &&
           !(GetWindowLongPtrW(edit, GWL_STYLE) & ES_PASSWORD);
}

bool ColourFormsDisabledEdit(HDC dc, HBRUSH nativeBrush) {
    if (!dc || !nativeBrush || !g_backgroundBrush) return false;
    LOGBRUSH brush = {};
    if (GetObjectW(nativeBrush, sizeof(brush), &brush) != static_cast<int>(sizeof(brush)) ||
        brush.lbStyle != BS_SOLID ||
        (brush.lbColor != RealSystemColour(COLOR_WINDOW) &&
         brush.lbColor != RealSystemColour(COLOR_BTNFACE))) return false;
    const COLORREF gray = RealSystemColour(COLOR_GRAYTEXT);
    // RGB(106,106,106) has at least 3:1 contrast against RGB(32,32,32).
    // Require each component to meet the bound, including unusual colours.
    if (GetRValue(gray) < 106 || GetGValue(gray) < 106 || GetBValue(gray) < 106) return false;
    const COLORREF oldBk = SetBkColor(dc, kColorBackground);
    if (oldBk == CLR_INVALID) return false;
    if (!SetBkMode(dc, OPAQUE)) {
        SetBkColor(dc, oldBk);
        return false;
    }
    return true;
}

#if defined(_WIN64)
void TryInstallActionsElementHook(HWND actionsWindow);
#endif

bool IsFormsEditFrameCandidate(HWND hWnd);
bool RefreshEventDetailFramesFor(HWND hWnd);
void RefreshFormsSummaryFramesFor(HWND hWnd);
void RefreshFormsSummaryFramesFor(HWND hWnd, bool includeTracked, bool fromPaint);

// MMC creates the result view and its host windows with a light class brush, so
// a newly shown view is erased white before its content arrives (a white flash
// when switching snap-ins). Swap that brush for the dark one on the view chain
// and restore the original when the mod unloads.
struct HostClassBrush { HWND window; ULONG_PTR original; };
std::unordered_map<ATOM, HostClassBrush> g_hostClassBrushes;

bool IsHelperClassWindow(HWND hWnd) {
    WCHAR cls[64] = {};
    if (!hWnd || !GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    return wcscmp(cls, L"IME") == 0 ||
           wcscmp(cls, L"MSCTFIME UI") == 0 ||
           wcscmp(cls, L"OleMainThreadWndClass") == 0 ||
           wcscmp(cls, L"GDI+ Hook Window Class") == 0 ||
           wcsncmp(cls, L".NET-BroadcastEventWindow", 25) == 0;
}

bool IsInsideMmcView(HWND hWnd) {
    HWND p = hWnd;
    for (int depth = 0; p && depth < 10; ++depth, p = GetParent(p)) {
        if (WindowClassEquals(p, L"MMCViewWindow")) return true;
    }
    return false;
}

// A class brush is light when it is a COLOR_xxx+1 pseudo handle for a light
// system colour, or a light solid brush. Null and hollow brushes paint nothing.
bool IsLightClassBrush(ULONG_PTR brush) {
    if (!brush) return false;
    COLORREF colour = 0;
    if (brush <= 32) {
        colour = RealSystemColour(static_cast<int>(brush) - 1);
    } else {
        LOGBRUSH lb = {};
        if (GetObjectW(reinterpret_cast<HBRUSH>(brush), sizeof(lb), &lb) != sizeof(lb) ||
            lb.lbStyle != BS_SOLID) return false;
        colour = lb.lbColor;
    }
    return (GetRValue(colour) + GetGValue(colour) + GetBValue(colour)) / 3 > 140;
}

// The console view and every window it creates for a result pane.
bool IsViewChainClass(HWND hWnd) {
    WCHAR name[64] = {};
    if (!GetClassNameW(hWnd, name, ARRAYSIZE(name))) return false;
    if (!wcscmp(name, L"#32770")) return false;
    // MMC can create its own host windows before parenting them under the view,
    // so the view test alone misses them at creation time. Match them by name.
    static const WCHAR* const kMmcHostClasses[] = {
        L"MMCViewWindow", L"MMCChildFrm", L"MMCOCXViewWindow", L"AtlAxWinEx",
        L"AfxOleControl42u", L"AfxWnd42u", L"AfxFrameOrView42u"};
    for (const WCHAR* host : kMmcHostClasses) {
        if (!wcscmp(name, host)) return true;
    }
    return IsInsideMmcView(hWnd);
}

void DarkenHostClass(HWND hWnd) {
    if (!g_active || !g_backgroundBrush || !OnOwningThread(hWnd) ||
        !IsViewChainClass(hWnd)) return;
    const ATOM atom = static_cast<ATOM>(GetClassLongPtrW(hWnd, GCW_ATOM));
    const ULONG_PTR current = GetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND);
    if (!atom || current == reinterpret_cast<ULONG_PTR>(g_backgroundBrush)) return;
    if (!IsLightClassBrush(current)) return;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        if (g_hostClassBrushes.count(atom)) return;
        g_hostClassBrushes[atom] = {hWnd, current};
    }
    SetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(g_backgroundBrush));
    InvalidateRect(hWnd, nullptr, TRUE);
}

struct ClassFinder { ATOM atom; HWND found; };
BOOL CALLBACK FindClassWindow(HWND hWnd, LPARAM param) {
    auto* f = reinterpret_cast<ClassFinder*>(param);
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId() &&
        static_cast<ATOM>(GetClassLongPtrW(hWnd, GCW_ATOM)) == f->atom) {
        f->found = hWnd;
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK FindClassWindowTop(HWND top, LPARAM param) {
    auto* f = reinterpret_cast<ClassFinder*>(param);
    if (FindClassWindow(top, param)) EnumChildWindows(top, FindClassWindow, param);
    return f->found ? FALSE : TRUE;
}

void RestoreHostClassBrushes() {
    std::unordered_map<ATOM, HostClassBrush> saved;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        saved.swap(g_hostClassBrushes);
    }
    for (auto& entry : saved) {
        HWND window = entry.second.window;
        if (!window || !IsWindow(window) ||
            static_cast<ATOM>(GetClassLongPtrW(window, GCW_ATOM)) != entry.first) {
            // The window that first showed the class may be gone; look for any
            // live window of the class anywhere in the process.
            ClassFinder finder = {entry.first, nullptr};
            EnumWindows(FindClassWindowTop, reinterpret_cast<LPARAM>(&finder));
            window = finder.found;
        }
        if (!window) {
            Wh_Log(L"could not find a window to restore class brush atom %u", (unsigned)entry.first);
            continue;
        }
        if (GetClassLongPtrW(window, GCLP_HBRBACKGROUND) ==
            reinterpret_cast<ULONG_PTR>(g_backgroundBrush)) {
            SetClassLongPtrW(window, GCLP_HBRBACKGROUND, static_cast<LONG_PTR>(entry.second.original));
        }
    }
}

void ApplyToWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || IsHelperClassWindow(hWnd)) {
        return;
    }

    // Apply window changes on the owning thread; defer foreign-thread work.
    if (!OnOwningThread(hWnd)) {
        NoteWalkPendingFor(hWnd);
        return;
    }

    DarkenHostClass(hWnd);
    ThemeShellView(hWnd);
    ThemeViewPlaceholder(hWnd);

    if (IsHtmlHost(hWnd)) {
        // Identify a narrow native host and recheck its COM identity on erase.
        HMODULE formModule = GetModuleHandleW(L"mmcndmgr.dll");
        if (g_active && MmcFormHost::IsCandidate(hWnd, formModule) &&
            SubclassWith(hWnd, WindowKind::FormHost)) {
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        return;
    }

    // Dialogs need their colour-message handler before the generic backdrop
    // claims the single subclass slot on a top-level window.
    WCHAR dialogClass[16] = {};
    if (GetClassNameW(hWnd, dialogClass, ARRAYSIZE(dialogClass)) &&
        wcscmp(dialogClass, L"#32770") == 0) {
        SubclassWith(hWnd, WindowKind::Dialog);
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
        (wcscmp(treeClass, L"SysTreeView32") == 0 ||
         wcscmp(treeClass, L"SysListView32") == 0) && treeParent &&
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
    if (IsMmcActionsPane(hWnd) && SubclassWith(hWnd, WindowKind::ActionsPane)) {
        InvalidateRect(hWnd, nullptr, TRUE);
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
        if (wcscmp(labelClass, L"Static") == 0) {
            ThemeWizardLabel(hWnd);
            ThemeWizardDivider(hWnd);
        }
    }

    if (wcscmp(labelClass, L"Button") == 0 &&
        (GetWindowLongPtrW(hWnd, GWL_STYLE) & BS_TYPEMASK) == BS_GROUPBOX) {
        SetWindowThemeQuiet(hWnd, L"", L"");
        SubclassWith(hWnd, WindowKind::GroupBox);
        RememberGroupFont(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
    }

    if (wcscmp(labelClass, L"Edit") == 0) {
        WCHAR parentClass[32] = {};
        HWND editParent = GetParent(hWnd);
        if (editParent && GetClassNameW(editParent, parentClass, ARRAYSIZE(parentClass)) &&
            wcscmp(parentClass, L"#32770") == 0) {
            SetWindowThemeQuiet(hWnd, L"", L"");
            if (RemoveClientEdge(hWnd)) {
                SubclassWith(hWnd, WindowKind::Field);
            }
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    }

    // Preserve the native control and input handling; the scoped drawing hook
    // changes only a matching classic choice glyph.
    if (IsDialogChoiceButton(hWnd)) {
        SetWindowThemeQuiet(hWnd, L"", L"");
        if (SubclassWith(hWnd, WindowKind::ChoiceButton)) {
            InvalidateRect(hWnd, nullptr, TRUE);
        }
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

    if (IsTextPushButton(hWnd) && SubclassWith(hWnd, WindowKind::PushButton)) {
        InvalidateRect(hWnd, nullptr, TRUE);
    }

    // Last on purpose: a window takes only one subclass, so the specific
    // handlers above (tab strip, summary label, combo, button) win.
    if (IsHostedWinFormsWindow(hWnd)) {
        const bool topLevel = GetAncestor(hWnd, GA_ROOT) == hWnd;
        if (SubclassWith(hWnd, topLevel ? WindowKind::WinFormsBackdrop
                                      : WindowKind::WinFormsSurface)) {
            InvalidateRect(hWnd, nullptr, TRUE);
            if (IsFormsEditFrameCandidate(hWnd)) {
                RedrawWindow(hWnd, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
            }
        }
    }

    RefreshEventDetailFramesFor(hWnd);
    RefreshFormsSummaryFramesFor(hWnd);

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

HBRUSH PressedBrush() {
    if (!g_pressedBrush) {
        g_pressedBrush = CreateSolidBrush(kColorPressed);
    }
    return g_pressedBrush;
}

HBRUSH AccentBrush() {
    if (!g_accentBrush) {
        g_accentBrush = CreateSolidBrush(kColorAccent);
    }
    return g_accentBrush;
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
                // A client edge is two pixels thick; hide the inner 3D line too.
                if ((GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_CLIENTEDGE) &&
                    g_backgroundBrush) {
                    InflateRect(&edge, -1, -1);
                    FrameRect(dc, &edge, g_backgroundBrush);
                }
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

// Add a subtle edge after the field's native paint; the focused field also
// gets an accent underline.
LRESULT HandleField(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    if (message == WM_SETFOCUS || message == WM_KILLFOCUS) {
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
        RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME);
        return result;
    }

    if (message == WM_PAINT) {
        LRESULT result =
            DefSubclassProc(hWnd, message, wParam, lParam);
        HBRUSH brush = LineBrush();
        HDC dc = GetDC(hWnd);
        if (brush && dc) {
            RECT client = {};
            GetClientRect(hWnd, &client);
            FrameRect(dc, &client, brush);
            HBRUSH accent = AccentBrush();
            if (accent && GetFocus() == hWnd) {
                int thickness = ScaleForWindow(hWnd, 2);
                if (thickness < 1) thickness = 1;
                if (thickness > client.bottom - client.top) {
                    thickness = client.bottom - client.top;
                }
                RECT underline = {client.left, client.bottom - thickness, client.right,
                                  client.bottom};
                FillRect(dc, &underline, accent);
            }
        }
        if (dc) {
            ReleaseDC(hWnd, dc);
        }
        return result;
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

// Event detail uses two bordered splitter panels: a virtual report list in one
// and a read-only Rich Edit in the other. Generic Forms panels do not qualify.
bool IsFormsEventRichEdit(HWND hWnd) {
    WCHAR name[128] = {};
    if (!GetClassNameW(hWnd, name, ARRAYSIZE(name))) return false;
    constexpr PCWSTR prefix20 = L"WindowsForms10.RICHEDIT20W.";
    constexpr PCWSTR prefix50 = L"WindowsForms10.RICHEDIT50W.";
    return (_wcsnicmp(name, prefix20, wcslen(prefix20)) == 0 ||
            _wcsnicmp(name, prefix50, wcslen(prefix50)) == 0) &&
           (GetWindowLongPtrW(hWnd, GWL_STYLE) & ES_READONLY);
}

bool IsFormsEventList(HWND hWnd) {
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    return WindowClassStarts(hWnd, L"WindowsForms10.SysListView32.") &&
           (style & LVS_TYPEMASK) == LVS_REPORT && (style & LVS_OWNERDATA);
}

// This scan only examines existing HWNDs, never sends messages or creates
// managed handles. Any excessive tree or ambiguity fails closed.
int EventDetailDescendants(HWND panel, HWND* list = nullptr, HWND* rich = nullptr) {
    if (list) *list = nullptr;
    if (rich) *rich = nullptr;
    HWND stack[128] = {};
    int count = 0, visited = 0, found = 0;
    HWND child = GetWindow(panel, GW_CHILD);
    while (child) {
        if (count == static_cast<int>(ARRAYSIZE(stack))) return 0;
        stack[count++] = child;
        child = GetWindow(child, GW_HWNDNEXT);
    }
    while (count) {
        HWND current = stack[--count];
        if (++visited > 256) return 0;
        if (IsFormsEventList(current)) {
            if (found & 1) return 0;
            found |= 1;
            if (list) *list = current;
        }
        if (IsFormsEventRichEdit(current)) {
            if (found & 2) return 0;
            found |= 2;
            if (rich) *rich = current;
        }
        child = GetWindow(current, GW_CHILD);
        while (child) {
            if (count == static_cast<int>(ARRAYSIZE(stack))) return 0;
            stack[count++] = child;
            child = GetWindow(child, GW_HWNDNEXT);
        }
    }
    return found;
}

bool IsFormsEventPanelStyle(HWND hWnd) {
    if (!WindowClassStarts(hWnd, L"WindowsForms10.Window.") ||
        !IsWindowVisible(hWnd) || !IsWindowEnabled(hWnd)) return false;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    return (style & (WS_CHILD | WS_CLIPCHILDREN)) == (WS_CHILD | WS_CLIPCHILDREN) &&
           !(style & (WS_VSCROLL | WS_HSCROLL)) &&
           (exStyle & (WS_EX_CLIENTEDGE | WS_EX_CONTROLPARENT)) ==
               (WS_EX_CLIENTEDGE | WS_EX_CONTROLPARENT);
}

HWND FindEventDetailSibling(HWND panel) {
    if (!IsFormsEventPanelStyle(panel)) return nullptr;
    HWND parent = GetParent(panel);
    HWND root = GetAncestor(panel, GA_ROOT);
    if (!WindowClassStarts(parent, L"WindowsForms10.Window.") ||
        !(WindowClassStarts(root, L"WindowsForms10.Window.") ||
          WindowClassEquals(root, L"MMCMainFrame"))) return nullptr;
    HWND other = nullptr, child = GetWindow(parent, GW_CHILD);
    int count = 0, matches = 0;
    for (; child && count < 128; ++count, child = GetWindow(child, GW_HWNDNEXT)) {
        if (IsFormsEventPanelStyle(child)) {
            ++matches;
            if (child != panel) other = child;
        }
    }
    if (child || matches != 2 || !other) return nullptr;
    RECT a = {}, b = {};
    if (!GetWindowRect(panel, &a) || !GetWindowRect(other, &b) ||
        a.right <= a.left || a.bottom <= a.top ||
        b.right <= b.left || b.bottom <= b.top) return nullptr;
    int tolerance = ScaleForWindow(panel, 2);
    int maxGap = ScaleForWindow(panel, 32);
    if (tolerance < 1) tolerance = 1;
    const auto withinTolerance = [tolerance](LONG x, LONG y) {
        const long long delta = static_cast<long long>(x) - y;
        return delta >= -tolerance && delta <= tolerance;
    };
    const auto gap = [maxGap](LONG x, LONG y) {
        const long long delta = static_cast<long long>(x) - y;
        return delta >= 0 && delta <= maxGap;
    };
    if (!withinTolerance(a.left, b.left) || !withinTolerance(a.right, b.right) ||
        !(gap(a.top, b.bottom) || gap(b.top, a.bottom))) return nullptr;
    const int first = EventDetailDescendants(panel);
    const int second = EventDetailDescendants(other);
    return ((first == 1 && second == 2) || (first == 2 && second == 1))
        ? other : nullptr;
}

// A task preview contains a report list and either a tab or the blank panel
// shown when no task is selected. The inactive control may still have an HWND.
bool IsEmptyFormsSummaryPlaceholder(HWND panel, HWND child) {
    if (!WindowClassStarts(child, L"WindowsForms10.Window.") ||
        (GetWindowLongPtrW(child, GWL_STYLE) &
         (WS_BORDER | WS_VSCROLL | WS_HSCROLL)) ||
        (GetWindowLongPtrW(child, GWL_EXSTYLE) &
         (WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME)) ||
        GetWindow(child, GW_CHILD)) return false;
    RECT parentRect = {}, childRect = {};
    if (!GetClientRect(panel, &parentRect) || !GetWindowRect(child, &childRect)) return false;
    POINT origin = {0, 0};
    if (!ClientToScreen(panel, &origin)) return false;
    OffsetRect(&parentRect, origin.x, origin.y);
    int tolerance = ScaleForWindow(panel, 6);
    if (tolerance < 1) tolerance = 1;
    const auto close = [tolerance](LONG a, LONG b) {
        const long long delta = static_cast<long long>(a) - b;
        return delta >= -tolerance && delta <= tolerance;
    };
    // An inactive fill-docked placeholder can retain its earlier size after
    // a resize. Keep its origin and nonempty geometry checks; the visible
    // alternative must still be the unique tab control.
    return parentRect.right > parentRect.left && parentRect.bottom > parentRect.top &&
           childRect.right > childRect.left && childRect.bottom > childRect.top &&
           close(parentRect.left, childRect.left) && close(parentRect.top, childRect.top) &&
           (!IsWindowVisible(child) ||
            (close(parentRect.right, childRect.right) && close(parentRect.bottom, childRect.bottom)));
}

int FormsSummaryPanelContent(HWND panel) {
    HWND list = nullptr, tab = nullptr, placeholder = nullptr;
    HWND child = GetWindow(panel, GW_CHILD);
    int count = 0;
    for (; child && count < 2; ++count, child = GetWindow(child, GW_HWNDNEXT)) {
        if (WindowClassStarts(child, L"WindowsForms10.SysListView32.")) {
            if (list) return 0;
            list = child;
        } else if (WindowClassStarts(child, L"WindowsForms10.SysTabControl32.")) {
            if (tab) return 0;
            tab = child;
        } else if (IsEmptyFormsSummaryPlaceholder(panel, child)) {
            if (placeholder) return 0;
            placeholder = child;
        } else {
            return 0;
        }
    }
    if (child || !count) return 0;
    if (list) {
        const LONG_PTR style = GetWindowLongPtrW(list, GWL_STYLE);
        return count == 1 && IsWindowVisible(list) && IsWindowEnabled(list) &&
               (style & LVS_TYPEMASK) == LVS_REPORT && !(style & LVS_OWNERDATA) ? 1 : 0;
    }
    if (tab && !IsWindowEnabled(tab)) return 0;
    if (placeholder && !IsWindowEnabled(placeholder)) return 0;
    const bool tabVisible = tab && IsWindowVisible(tab);
    const bool blankVisible = placeholder && IsWindowVisible(placeholder);
    return tabVisible != blankVisible ? 2 : 0;
}

HWND FindFormsSummarySibling(HWND panel) {
    if (!IsFormsEventPanelStyle(panel) ||
        !WindowClassEquals(GetAncestor(panel, GA_ROOT), L"MMCMainFrame")) return nullptr;
    HWND inner = GetParent(panel), outer = GetParent(inner), container = GetParent(outer);
    if (!WindowClassStarts(inner, L"WindowsForms10.Window.") ||
        !IsFormsEventPanelStyle(outer) || GetWindow(outer, GW_CHILD) != inner ||
        GetWindow(inner, GW_HWNDNEXT) ||
        !WindowClassStarts(container, L"WindowsForms10.Window.")) return nullptr;
    HWND host = GetParent(container);
    if (!WindowClassStarts(host, L"ATL:") ||
        !WindowClassEquals(GetParent(host), L"AtlAxWinEx")) return nullptr;
    host = GetParent(GetParent(host));
    if (!WindowClassEquals(host, L"MMCOCXViewWindow") ||
        !WindowClassEquals(GetParent(host), L"MMCViewWindow")) return nullptr;
    HWND other = nullptr, child = GetWindow(inner, GW_CHILD);
    int count = 0, matches = 0;
    for (; child && count < 128; ++count, child = GetWindow(child, GW_HWNDNEXT)) {
        if (IsFormsEventPanelStyle(child)) {
            ++matches;
            if (child != panel) other = child;
        }
    }
    if (child || matches != 2 || !other) return nullptr;
    const int aType = FormsSummaryPanelContent(panel), bType = FormsSummaryPanelContent(other);
    if (!((aType == 1 && bType == 2) || (aType == 2 && bType == 1))) return nullptr;
    RECT a = {}, b = {};
    if (!GetWindowRect(panel, &a) || !GetWindowRect(other, &b) ||
        a.right <= a.left || a.bottom <= a.top || b.right <= b.left || b.bottom <= b.top) return nullptr;
    int tolerance = ScaleForWindow(panel, 2), maxGap = ScaleForWindow(panel, 32);
    if (tolerance < 1) tolerance = 1;
    const auto close = [tolerance](LONG x, LONG y) {
        const long long delta = static_cast<long long>(x) - y;
        return delta >= -tolerance && delta <= tolerance;
    };
    const auto gap = [maxGap](LONG x, LONG y) {
        const long long delta = static_cast<long long>(x) - y;
        return delta >= 0 && delta <= maxGap;
    };
    return close(a.left, b.left) && close(a.right, b.right) &&
           (gap(a.top, b.bottom) || gap(b.top, a.bottom)) ? other : nullptr;
}

bool IsFormsSummaryOuter(HWND hWnd) {
    HWND container = GetParent(hWnd);
    if (!IsFormsEventPanelStyle(hWnd) ||
        !WindowClassStarts(container, L"WindowsForms10.Window.") ||
        !WindowClassStarts(GetParent(container), L"ATL:")) return false;
    HWND inner = GetWindow(hWnd, GW_CHILD);
    if (!inner || GetWindow(inner, GW_HWNDNEXT)) return false;
    HWND panel = GetWindow(inner, GW_CHILD);
    for (int count = 0; panel && count < 128; ++count, panel = GetWindow(panel, GW_HWNDNEXT)) {
        if (FindFormsSummarySibling(panel)) return true;
    }
    return false;
}

// The panel pairing above is required before touching either control.
// Native borders and scrollbars retain their geometry and input behavior.
bool IsFormsEventDetailLeaf(HWND hWnd) {
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    const bool eventList = IsFormsEventList(hWnd);
    const bool eventRich = IsFormsEventRichEdit(hWnd);
    const LONG_PTR otherEdges = WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME;
    const bool singleEdge = (style & WS_BORDER) && !(exStyle & WS_EX_CLIENTEDGE);
    // Framework RichTextBox maps FixedSingle to the native client edge.
    const bool richClientEdge = eventRich && !(style & WS_BORDER) &&
                                (exStyle & WS_EX_CLIENTEDGE);
    if (!IsWindowEnabled(hWnd) || (!eventList && !eventRich) ||
        (exStyle & otherEdges) || (!singleEdge && !richClientEdge)) return false;
    HWND parent = GetParent(hWnd);
    for (int depth = 0; parent && depth < 16; ++depth, parent = GetParent(parent)) {
        if (FindEventDetailSibling(parent)) return true;
    }
    return false;
}

bool RefreshEventDetailFramesFor(HWND hWnd) {
    if (!IsFormsEventList(hWnd) && !IsFormsEventRichEdit(hWnd)) return false;
    HWND parent = GetParent(hWnd);
    for (int depth = 0; parent && depth < 16; ++depth, parent = GetParent(parent)) {
        HWND sibling = FindEventDetailSibling(parent);
        if (sibling) {
            HWND list = nullptr, rich = nullptr;
            EventDetailDescendants(parent, &list, &rich);
            HWND first = list ? list : rich;
            list = rich = nullptr;
            EventDetailDescendants(sibling, &list, &rich);
            HWND second = list ? list : rich;
            for (HWND frame : {parent, sibling, first, second}) {
                if (frame && (GetFocus() != frame || IsFormsEventRichEdit(frame)) &&
                    !GetPropW(frame, kFormsFrameProperty)) {
                    RedrawWindow(frame, nullptr, nullptr,
                                 RDW_FRAME | RDW_INVALIDATE | RDW_NOCHILDREN);
                }
            }
            return true;
        }
    }
    return false;
}

// Keep the separator adjustment to small, captionless Forms labels.
bool IsFormsSeparator(HWND hWnd) {
    if (!WindowClassStarts(hWnd, L"WindowsForms10.STATIC.") ||
        !(GetWindowLongPtrW(hWnd, GWL_EXSTYLE) &
          (WS_EX_CLIENTEDGE | WS_EX_STATICEDGE))) {
        return false;
    }
    DWORD_PTR length = 0;
    if (!SendMessageTimeoutW(hWnd, WM_GETTEXTLENGTH, 0, 0,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &length) || length) {
        return false;
    }
    RECT window = {};
    if (!GetWindowRect(hWnd, &window)) {
        return false;
    }
    const int width = window.right - window.left;
    const int height = window.bottom - window.top;
    int threshold = ScaleForWindow(hWnd, 6);
    if (threshold < 1) threshold = 1;
    return width > 0 && height > 0 && (height <= threshold || width <= threshold);
}

// Preserve input geometry and scrollbars; a focused Forms edit gets an accent frame.
bool IsFormsEditFrameCandidate(HWND hWnd) {
    HWND parent = GetParent(hWnd);
    HWND root = GetAncestor(hWnd, GA_ROOT);
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    return WindowClassStarts(hWnd, L"WindowsForms10.EDIT.") &&
           WindowClassStarts(parent, L"WindowsForms10.Window.") &&
           (WindowClassStarts(root, L"WindowsForms10.Window.") ||
            ((style & ES_READONLY) && IsHostedFormsReadOnlyEdit(hWnd))) &&
           (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_CLIENTEDGE) &&
           !(style & (WS_VSCROLL | WS_HSCROLL | ES_PASSWORD));
}

bool IsFormsFrame(HWND hWnd) {
    return IsFormsEditFrameCandidate(hWnd) ||
           (IsFormsEventDetailLeaf(hWnd) &&
            (GetFocus() != hWnd || IsFormsEventRichEdit(hWnd))) ||
           ((FindEventDetailSibling(hWnd) || FindFormsSummarySibling(hWnd) ||
             IsFormsSummaryOuter(hWnd)) && GetFocus() != hWnd);
}

// DefSubclassProc paints first. Duplicate the update region before passing it
// on; GetDCEx owns the copy on success. Never paint inside the client area.
LRESULT HandleFormsFrame(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HRGN update = nullptr;
    if (wParam != 1) {
        update = CreateRectRgn(0, 0, 0, 0);
        if (!update || CombineRgn(update, reinterpret_cast<HRGN>(wParam),
                                  nullptr, RGN_COPY) == ERROR) {
            if (update) DeleteObject(update);
            return DefSubclassProc(hWnd, message, wParam, lParam);
        }
    }
    LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
    RECT window = {}, client = {};
    POINT origin = {0, 0};
    HBRUSH line = GetFocus() == hWnd &&
                          (IsFormsEditFrameCandidate(hWnd) ||
                           (IsFormsEventRichEdit(hWnd) && IsFormsEventDetailLeaf(hWnd)))
        ? AccentBrush() : LineBrush();
    if (!g_active || !IsWindow(hWnd) ||
        (!IsFormsFrame(hWnd) && !IsFormsSeparator(hWnd)) || !line ||
        !GetWindowRect(hWnd, &window) || !GetClientRect(hWnd, &client) ||
        !ClientToScreen(hWnd, &origin)) {
        if (update) DeleteObject(update);
        return result;
    }
    const int width = window.right - window.left;
    const int height = window.bottom - window.top;
    OffsetRect(&client, origin.x - window.left, origin.y - window.top);
    if (width <= 0 || height <= 0 || client.left <= 0 || client.top <= 0 ||
        client.right >= width || client.bottom >= height) {
        if (update) DeleteObject(update);
        return result;
    }
    DWORD flags = DCX_WINDOW | DCX_CACHE | DCX_CLIPSIBLINGS | DCX_CLIPCHILDREN;
    if (update) flags |= DCX_INTERSECTRGN;
    HDC dc = GetDCEx(hWnd, update, flags);
    if (!dc) {
        if (update) DeleteObject(update);
        return result;
    }
    const bool marked = GetPropW(hWnd, kFormsFrameProperty) != nullptr;
    const bool canMark = marked || SetPropW(hWnd, kFormsFrameProperty,
                                            reinterpret_cast<HANDLE>(1));
    bool painted = false;
    if (canMark && ExcludeClipRect(dc, client.left, client.top, client.right, client.bottom) != ERROR) {
        RECT edge = {0, 0, width, height};
        const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
        const LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        const bool single = (style & WS_BORDER) && !(exStyle & WS_EX_CLIENTEDGE);
        int count = ScaleForWindow(hWnd, single ? 1 : 2);
        if (count < 1) count = 1;
        if (count > client.left) count = client.left;
        if (count > client.top) count = client.top;
        if (count > width - client.right) count = width - client.right;
        if (count > height - client.bottom) count = height - client.bottom;
        for (int i = 0; i < count && edge.right > edge.left && edge.bottom > edge.top; ++i) {
            if (!FrameRect(dc, &edge, line)) break;
            painted = true;
            InflateRect(&edge, -1, -1);
        }
    }
    if (!painted && canMark && !marked) RemovePropW(hWnd, kFormsFrameProperty);
    ReleaseDC(hWnd, dc);
    return result;
}

void RestoreFormsFrame(HWND hWnd) {
    RemovePropW(hWnd, kEventFrameRetryProperty);
    RemovePropW(hWnd, kSummaryPaintRetryProperty);
    if (!GetPropW(hWnd, kFormsFrameProperty)) return;
    RemovePropW(hWnd, kFormsFrameProperty);
    RedrawWindow(hWnd, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
}

LRESULT HandleFormsSurface(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_NCPAINT && g_active &&
        (IsFormsFrame(hWnd) || IsFormsSeparator(hWnd))) {
        return HandleFormsFrame(hWnd, message, wParam, lParam);
    }
    if (g_active && (message == WM_SETFOCUS || message == WM_KILLFOCUS) &&
        (IsFormsEditFrameCandidate(hWnd) || FindEventDetailSibling(hWnd) ||
         IsFormsEventDetailLeaf(hWnd))) {
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
        if (IsWindow(hWnd)) RedrawWindow(hWnd, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
        return result;
    }
    return DefSubclassProc(hWnd, message, wParam, lParam);
}

// Cache the group-box font outside paint.
struct GroupFont {
    HWND hWnd;
    HFONT font;
};
thread_local GroupFont g_groupFonts[64] = {};

void RememberGroupFont(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }
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
        if (g_groupFonts[i].hWnd && !IsWindow(g_groupFonts[i].hWnd)) {
            g_groupFonts[i] = {};
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
    if (!hWnd || !IsWindow(hWnd)) {
        return nullptr;
    }
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

    // DrawText measures '&' mnemonics correctly; GetTextExtentPoint32W does not.
    SIZE textSize = {};
    if (caption[0]) {
        RECT measure = {};
        DrawTextW(dc, caption, -1, &measure, DT_CALCRECT | DT_SINGLELINE | DT_LEFT);
        textSize.cx = measure.right - measure.left;
        textSize.cy = measure.bottom - measure.top;
    }

    const int lineY = client.top + textSize.cy / 2;

    HGDIOBJ pen = CreatePen(PS_SOLID, 1, kColorLine);
    if (!pen) {
        if (oldFont) {
            SelectObject(dc, oldFont);
        }
        EndPaint(hWnd, &ps);
        return;
    }
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));

    Rectangle(dc, client.left, lineY, client.right, client.bottom);

    if (caption[0] && g_backgroundBrush) {
        const int inset = ScaleForWindow(hWnd, 6);
        const int gap = ScaleForWindow(hWnd, 4);
        RECT captionRect = {client.left + inset, client.top, 0,
                            client.top + textSize.cy + ScaleForWindow(hWnd, 2)};
        captionRect.right = captionRect.left + textSize.cx + 2 * gap;
        FillRect(dc, &captionRect, g_backgroundBrush);
        captionRect.left += gap;
        DrawTextOn(dc, caption, &captionRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER,
                   IsWindowEnabled(hWnd) ? kColorText : kColorTextDisabled,
                   kColorBackground);
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
        case WindowKind::Dialog:
            if (RemovePropW(hWnd, kTpmDialogPending) && g_active && IsWindow(hWnd)) {
                ThemeDialog(hWnd);
            }
            break;
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
    if (message == WM_ENTERIDLE) {
        ThemeOwnedModalOnIdle(hWnd, wParam, lParam);
    }
    if (message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE &&
        GetAncestor(hWnd, GA_ROOT) == hWnd) {
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
        HWND popup = IsWindow(hWnd) ? GetWindow(hWnd, GW_ENABLEDPOPUP) : nullptr;
        if (popup && popup != hWnd) {
            ThemeDialog(popup);
        }
        HWND incoming = reinterpret_cast<HWND>(lParam);
        if (IsWindow(hWnd) && incoming && incoming != popup) {
            ThemeOwnedModalOnIdle(hWnd, MSGF_DIALOGBOX, lParam);
        }
        return result;
    }
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

    if (message == WM_PAINT || message == WM_PRINTCLIENT) {
        // A wizard can become visible without its owner receiving WM_ENTERIDLE.
        // Retry after page creation, before native paint, without a repaint loop.
        static thread_local bool checkingWizardPages = false;
        if (g_active && message == WM_PAINT && !checkingWizardPages) {
            HWND root = GetAncestor(hWnd, GA_ROOT);
            HWND parent = GetParent(hWnd);
            if (root == hWnd && IsWizardSheetWindow(hWnd)) {
                checkingWizardPages = true;
                ThemeWizardColourPages(hWnd);
                checkingWizardPages = false;
            } else if (parent && IsWizardColourPage(hWnd, parent)) {
                checkingWizardPages = true;
                ThemeWizardColourPage(hWnd, reinterpret_cast<LPARAM>(parent));
                checkingWizardPages = false;
            }
        }
        HWND previousWizard = g_wizardPaintWindow;
        g_wizardPaintWindow = hWnd;
#if defined(_WIN64)
        HWND previousFooterDialog = g_footerPaintDialog;
        if (message == WM_PAINT) g_footerPaintDialog = hWnd;
#endif
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
#if defined(_WIN64)
        g_footerPaintDialog = previousFooterDialog;
#endif
        g_wizardPaintWindow = previousWizard;
        if (message == WM_PAINT) CoverPageFrame(hWnd);
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
        // Editable fields that sit directly on the dialog get a raised fill so
        // they stand out from the page; everything else keeps the page colour.
        const bool field = message == WM_CTLCOLOREDIT && lParam &&
                           GetParent(reinterpret_cast<HWND>(lParam)) == hWnd;
        HBRUSH brush = field ? TabSelectedBrush() : g_backgroundBrush;
        if (wParam) {
            HDC dc = reinterpret_cast<HDC>(wParam);
            // Opaque text backgrounds provide a known colour for ClearType blending.
            SetBkMode(dc, OPAQUE);
            SetTextColor(dc, kColorText);
            SetBkColor(dc, field ? kColorSurface : kColorBackground);
        }
        return reinterpret_cast<LRESULT>(brush);
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

    if (wcscmp(className, L"Static") == 0) {
        ThemeWizardLabel(hWnd);
        ThemeWizardDivider(hWnd);
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
        // The edit inside a combo box keeps the combo's own drawing.
        WCHAR parentClass[32] = {};
        HWND editParent = GetParent(hWnd);
        const bool inCombo =
            editParent &&
            GetClassNameW(editParent, parentClass, ARRAYSIZE(parentClass)) &&
            (wcscmp(parentClass, L"ComboBox") == 0 ||
             wcsncmp(parentClass, L"ComboBoxEx", 10) == 0);
        if (!inCombo) {
            // The themed edit draws a light frame and a light read-only fill;
            // classic painting follows the mod's colour messages instead.
            SetWindowThemeQuiet(hWnd, L"", L"");
            if (RemoveClientEdge(hWnd)) {
                SubclassWith(hWnd, WindowKind::Field);
            }
            InvalidateRect(hWnd, nullptr, TRUE);
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
    if (!hWnd || !IsWindow(hWnd)) {
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

thread_local std::vector<HWND>* t_resetPaintList = nullptr;
thread_local std::vector<HWND>* t_resetVisitedList = nullptr;

void ResetWindowState(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || IsHelperClassWindow(hWnd)) {
        return;
    }
    // Disable dark mode before removing each window's theme association.
    if (pAllowDarkModeForWindow) {
        pAllowDarkModeForWindow(hWnd, FALSE);
    }
    // Reset Forms combo boxes to the non-dark counterpart of the CFD theme.
    // Removing the association alone may leave their themed drop-down chrome dark.
    // If that theme cannot be applied, fall back to the normal association.
    if (!IsMmcSummaryCombo(hWnd) || FAILED(pSetWindowTheme(hWnd, L"CFD", nullptr))) {
        pSetWindowTheme(hWnd, nullptr, nullptr);
    }
    RestoreRemovedStyles(hWnd);
    ColourClassicControl(hWnd, false);
    DressExtraChrome(hWnd, false);
    if (GetAncestor(hWnd, GA_ROOT) == hWnd) {
        SetDarkTitleBar(hWnd, false);
        int preference = DWMWCP_DEFAULT;
        DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference,
                              sizeof(preference));
        // Restore the default DWM border colour during unload.
        DWORD border = DWMWA_COLOR_DEFAULT;
        DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &border, sizeof(border));
        SetCaptionColours(hWnd, false);
    }
}

BOOL CALLBACK WalkChild(HWND hWnd, LPARAM) {
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
        if (t_resetVisitedList) {
            t_resetVisitedList->push_back(hWnd);
        }
        ResetWindowState(hWnd);
        if (t_resetPaintList && IsWindowVisible(hWnd)) {
            t_resetPaintList->push_back(hWnd);
        }
    }
    return TRUE;
}

BOOL CALLBACK WalkTop(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }
    if (!OnOwningThread(hWnd)) {
        return TRUE;
    }
    if (g_active) {
        ApplyToWindow(hWnd);
        EnumChildWindows(hWnd, WalkChild, 0);
    } else {
        if (t_resetVisitedList) {
            t_resetVisitedList->push_back(hWnd);
        }
        ResetWindowState(hWnd);
        if (t_resetPaintList && IsWindowVisible(hWnd)) {
            t_resetPaintList->push_back(hWnd);
        }
        EnumChildWindows(hWnd, WalkChild, 0);
    }
    return TRUE;
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
    std::vector<HWND> paintList;
    std::vector<HWND> visitedList;
    if (!g_active) {
        t_resetPaintList = &paintList;
        t_resetVisitedList = &visitedList;
    }
    EnumWindows(WalkOwnedInTree, 0);
    t_resetPaintList = nullptr;
    t_resetVisitedList = nullptr;
    if (!g_active) {
        std::vector<HWND> windows;
        {
            std::lock_guard<std::mutex> lock(g_stateLock);
            windows.assign(g_subclassed.begin(), g_subclassed.end());
        }
        // Some Forms trees are parked under message-only windows, which
        // EnumWindows does not visit. Restore marked lists on their owner.
        for (HWND hWnd : windows) {
            if (IsWindow(hWnd) && OnOwningThread(hWnd)) {
                RestoreRemovedStyles(hWnd);
                RestorePaintListPalette(hWnd);
                RestoreFormsFrame(hWnd);
            }
        }
        // Remove all subclasses on the owning thread BEFORE sending change
        // notifications and synchronously repainting in native light mode.
        RemoveSubclassesOnCurrentThread(visitedList);
        // Notify bottom-up (children before parents) so parent views like
        // SysListView32, dialogs, and MMCMainFrame see their light children.
        for (auto it = visitedList.rbegin(); it != visitedList.rend(); ++it) {
            HWND w = *it;
            if (!w || !IsWindow(w) || IsHelperClassWindow(w)) {
                continue;
            }
            DWORD_PTR ignored = 0;
            SendMessageTimeoutW(w, WM_THEMECHANGED, 0, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &ignored);
            SendMessageTimeoutW(w, WM_SYSCOLORCHANGE, 0, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &ignored);
            if (GetAncestor(w, GA_ROOT) == w && GetMenu(w)) {
                DrawMenuBar(w);
            }
        }
        std::vector<HWND> rootsToFlush;
        std::unordered_set<HWND> seenRoots;
        for (HWND w : paintList) {
            if (!IsWindow(w) || !IsWindowVisible(w) || IsHelperClassWindow(w)) {
                continue;
            }
            RedrawWindow(w, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_NOCHILDREN);
            // Cross-thread hosted subtrees (such as WinForms panes) have no
            // GA_ROOT window on this thread; climb to the highest visible
            // same-thread child ancestor without crossing into an owner popup.
            HWND flushRoot = w;
            while ((GetWindowLongPtrW(flushRoot, GWL_STYLE) & WS_CHILD) != 0) {
                HWND parent = GetAncestor(flushRoot, GA_PARENT);
                if (!parent || !IsWindow(parent) || !IsWindowVisible(parent) ||
                    !OnOwningThread(parent)) {
                    break;
                }
                flushRoot = parent;
            }
            if (seenRoots.insert(flushRoot).second) {
                rootsToFlush.push_back(flushRoot);
            }
        }
        for (HWND root : rootsToFlush) {
            if (IsWindow(root) && IsWindowVisible(root)) {
                RedrawWindow(root, nullptr, nullptr,
                             RDW_ERASENOW | RDW_UPDATENOW | RDW_ALLCHILDREN);
            }
        }
    }
    g_inWalk = false;
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
        if (info && info->message == RemoteCallMessage() &&
            info->lParam == reinterpret_cast<LPARAM>(&WalkThisThread)) {
            ActiveThreadHookGuard hookGuard;
            if (!g_unloading.load(std::memory_order_acquire) ||
                !g_active.load(std::memory_order_acquire)) {
                WalkThisThread();
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Timed walks may be deferred until a busy thread creates another window.
constexpr UINT kRemoteCallTimeoutMs = 2000;

// Unload uses a blocking send on UI/tracked threads so busy snap-in threads
// always remove their own subclasses cleanly before the DLL unloads.
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
        SetLastError(ERROR_SUCCESS);
        SendMessageW(hWnd, RemoteCallMessage(), 0,
                     reinterpret_cast<LPARAM>(proc));
        if (!IsWindow(hWnd) && GetLastError() == ERROR_INVALID_WINDOW_HANDLE) {
            delivered = false;
        }
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

struct ThreadEntry {
    std::vector<HWND> samples;
    bool hasUiOrTracked = false;
};

void AddThreadOf(HWND hWnd, std::unordered_map<DWORD, ThreadEntry>* threads, bool forceTracked) {
    DWORD pid = 0;
    const DWORD threadId = GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId() && threadId) {
        const bool meaningful = forceTracked || !IsHelperClassWindow(hWnd);
        auto& entry = (*threads)[threadId];
        if (forceTracked || entry.samples.size() < 4) {
            entry.samples.push_back(hWnd);
        }
        if (meaningful) {
            entry.hasUiOrTracked = true;
        }
    }
}

BOOL CALLBACK CollectChildThread(HWND hWnd, LPARAM lParam) {
    AddThreadOf(hWnd, reinterpret_cast<std::unordered_map<DWORD, ThreadEntry>*>(lParam), false);
    return TRUE;
}

BOOL CALLBACK CollectThread(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }
    AddThreadOf(hWnd, reinterpret_cast<std::unordered_map<DWORD, ThreadEntry>*>(lParam), false);
    EnumChildWindows(hWnd, CollectChildThread, lParam);
    return TRUE;
}

// Collect window-owning threads and dispatch one walk per thread.
void WalkEveryThread(bool waitForIt) {
    std::unordered_map<DWORD, ThreadEntry> threads;
    EnumWindows(CollectThread, reinterpret_cast<LPARAM>(&threads));
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        const auto addSet = [&threads](const std::unordered_set<HWND>& set) {
            for (HWND hWnd : set) {
                if (IsWindow(hWnd)) {
                    AddThreadOf(hWnd, &threads, true);
                }
            }
        };
        addSet(g_subclassed);
        addSet(g_shellViews);
        addSet(g_wizardDividers);
        addSet(g_wizardLabels);
        addSet(g_wizardColourPages);
        for (const auto& entry : g_webCurtains) {
            if (IsWindow(entry.first)) {
                AddThreadOf(entry.first, &threads, true);
            }
        }
    }
    for (const auto& entry : threads) {
        if (waitForIt && !entry.second.hasUiOrTracked) {
            continue;
        }
        bool walked = false;
        HWND fallbackSample = nullptr;
        for (HWND sample : entry.second.samples) {
            if (!IsWindow(sample)) {
                continue;
            }
            if (!fallbackSample) {
                fallbackSample = sample;
            }
            if (RunOnWindowThread(sample, WalkThisThread, waitForIt)) {
                walked = true;
                break;
            }
        }
        // Leave busy threads pending during activation; unload uses a blocking walk.
        if (!walked && !waitForIt && fallbackSample) {
            NoteWalkPendingFor(fallbackSample);
        }
    }
    if (waitForIt) {
        // Retry any remaining live tracked windows in case an earlier sample died mid-send.
        for (int pass = 0; pass < 3; ++pass) {
            std::unordered_map<DWORD, ThreadEntry> remaining;
            {
                std::lock_guard<std::mutex> lock(g_stateLock);
                const auto addSet = [&remaining](const std::unordered_set<HWND>& set) {
                    for (HWND hWnd : set) {
                        if (IsWindow(hWnd)) {
                            AddThreadOf(hWnd, &remaining, true);
                        }
                    }
                };
                addSet(g_subclassed);
                addSet(g_shellViews);
                addSet(g_wizardDividers);
                addSet(g_wizardLabels);
                addSet(g_wizardColourPages);
                for (const auto& entry : g_webCurtains) {
                    if (IsWindow(entry.first)) {
                        AddThreadOf(entry.first, &remaining, true);
                    }
                }
            }
            if (remaining.empty()) {
                break;
            }
            bool progressed = false;
            for (const auto& entry : remaining) {
                for (HWND sample : entry.second.samples) {
                    if (IsWindow(sample) && RunOnWindowThread(sample, WalkThisThread, true)) {
                        progressed = true;
                        break;
                    }
                }
            }
            if (!progressed) {
                break;
            }
        }
    }
}

constexpr int kColScrollbar = 0;
// Extended View uses ActiveCaption and CaptionText for its HTML and icon.
constexpr int kColActiveCaption = 2;
constexpr int kColCaptionText = 9;
constexpr int kColGradientActiveCaption = 27;

// The Actions group header edge is drawn in the native gradient-caption colour
// through an API the mod does not otherwise hook. Recognise that exact colour.
bool IsNativePaleBrush(HDC dc, HGDIOBJ brush) {
    if (!brush || !pGetSysColor) return false;
    const COLORREF native = pGetSysColor(kColGradientActiveCaption) & 0x00ffffffu;
    COLORREF colour = CLR_INVALID;
    if (brush == GetStockObject(DC_BRUSH)) {
        colour = GetDCBrushColor(dc);
    } else {
        LOGBRUSH lb = {};
        if (GetObjectW(brush, sizeof(lb), &lb) == sizeof(lb) && lb.lbStyle == BS_SOLID) {
            colour = lb.lbColor;
        }
    }
    return colour != CLR_INVALID && (colour & 0x00ffffffu) == native;
}

using PatBlt_t = decltype(&PatBlt);
PatBlt_t pPatBlt = nullptr;
BOOL WINAPI PatBlt_Hook(HDC dc, int x, int y, int w, int h, DWORD rop) {
    if (g_active && g_backgroundBrush && IsActionsPaneDc(dc)) {
        if (rop == WHITENESS ||
            (rop == PATCOPY && IsLightSolidBrush(dc, GetCurrentObject(dc, OBJ_BRUSH)))) {
            HGDIOBJ old = SelectObject(dc, g_backgroundBrush);
            BOOL result = pPatBlt(dc, x, y, w, h, PATCOPY);
            SelectObject(dc, old);
            return result;
        }
    }
    if (g_active && g_lineBrush && dc && rop == PATCOPY) {
        HGDIOBJ current = GetCurrentObject(dc, OBJ_BRUSH);
        if (IsNativePaleBrush(dc, current)) {
            HGDIOBJ old = SelectObject(dc, g_lineBrush);
            BOOL result = pPatBlt(dc, x, y, w, h, rop);
            SelectObject(dc, old);
            return result;
        }
    }
    return pPatBlt(dc, x, y, w, h, rop);
}
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

HBRUSH MappedPseudoBrush(HBRUSH brush) {
    const ULONG_PTR value = reinterpret_cast<ULONG_PTR>(brush);
    if (!g_active || g_nativeFormsBrushScope || value == 0 ||
        value > static_cast<ULONG_PTR>(kSysColourCount)) {
        return brush;
    }
    HBRUSH mapped = g_sysBrushes[value - 1];
    return mapped ? mapped : brush;
}

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
        return kColorTextDisabled;
    default:
        return kNotMapped;
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

// The native BUTTON procedure retains layout, focus, input and accessibility.
// Replace only the classic checkbox/radio glyph inside its owning paint scope.
bool DrawDarkChoiceGlyph(HDC dc, const RECT* bounds, UINT type, UINT state) {
    if (!g_active || !g_drawingChoiceButton || !dc || !bounds || type != DFC_BUTTON ||
        WindowFromDC(dc) != g_drawingChoiceButton || !IsWindow(g_drawingChoiceButton)) return false;
    const UINT part = state & 0x0f;
    const UINT supported = 0x0f | DFCS_CHECKED | DFCS_INACTIVE | DFCS_PUSHED | DFCS_FLAT | DFCS_MONO;
    if ((state & ~supported) || (part != DFCS_BUTTONCHECK && part != DFCS_BUTTON3STATE &&
                                 part != DFCS_BUTTONRADIO)) return false;
    const int width = bounds->right - bounds->left, height = bounds->bottom - bounds->top;
    if (width < 7 || height < 7 || width > 64 || height > 64 ||
        width > height * 2 || height > width * 2) return false;
    const bool disabled = (state & DFCS_INACTIVE) != 0;
    const COLORREF border = disabled ? RGB(102, 102, 102) : RGB(170, 170, 170);
    const COLORREF fill = (state & DFCS_PUSHED) ? kColorHover : kColorSurface;
    const COLORREF mark = disabled ? kColorTextDisabled : kColorText;
    const int saved = SaveDC(dc);
    if (!saved) return false;
    HPEN edge = CreatePen(PS_SOLID, 1, border);
    const int side = width < height ? width : height;
    HPEN stroke = CreatePen(PS_SOLID, side >= 18 ? 2 : 1, mark);
    bool ok = edge && stroke && SelectObject(dc, edge) &&
              SelectObject(dc, GetStockObject(DC_BRUSH)) &&
              SetDCBrushColor(dc, fill) != CLR_INVALID;
    if (ok) ok = part == DFCS_BUTTONRADIO ?
        Ellipse(dc, bounds->left, bounds->top, bounds->right, bounds->bottom) != FALSE :
        Rectangle(dc, bounds->left, bounds->top, bounds->right, bounds->bottom) != FALSE;
    if (ok && (state & DFCS_CHECKED)) {
        if (part == DFCS_BUTTONRADIO) {
            const int padX = width / 3, padY = height / 3;
            ok = SelectObject(dc, stroke) && SetDCBrushColor(dc, mark) != CLR_INVALID &&
                 Ellipse(dc, bounds->left + padX, bounds->top + padY,
                         bounds->right - padX, bounds->bottom - padY);
        } else if (part == DFCS_BUTTON3STATE) {
            RECT bar = {bounds->left + width / 4, bounds->top + height / 2,
                        bounds->right - width / 4, bounds->top + height / 2 + (height >= 18 ? 2 : 1)};
            ok = SetDCBrushColor(dc, mark) != CLR_INVALID && FillRect(dc, &bar, (HBRUSH)GetStockObject(DC_BRUSH));
        } else {
            ok = SelectObject(dc, stroke) &&
                 MoveToEx(dc, bounds->left + width / 5, bounds->top + height / 2, nullptr) &&
                 LineTo(dc, bounds->left + width * 2 / 5, bounds->bottom - height / 4) &&
                 LineTo(dc, bounds->right - width / 5, bounds->top + height / 4);
        }
    }
    RestoreDC(dc, saved);
    if (edge) DeleteObject(edge);
    if (stroke) DeleteObject(stroke);
    return ok;
}

// Paint dialog check boxes and radio buttons directly. The classic glyph is not
// always drawn through DrawFrameControl, so a glyph-only hook can miss it.
void DrawChoiceGlyphDirect(HDC dc, int left, int top, int size, bool radio,
                           bool checked, bool indeterminate, bool disabled, bool pushed) {
    const COLORREF border = disabled ? RGB(102, 102, 102) : RGB(170, 170, 170);
    const COLORREF fill = pushed ? kColorHover : kColorSurface;
    const COLORREF mark = disabled ? kColorTextDisabled : kColorText;
    const int saved = SaveDC(dc);
    if (!saved) return;
    HPEN edge = CreatePen(PS_SOLID, 1, border);
    HPEN stroke = CreatePen(PS_SOLID, size >= 18 ? 2 : 1, mark);
    const int right = left + size, bottom = top + size;
    if (edge && stroke) {
        SelectObject(dc, edge);
        SelectObject(dc, GetStockObject(DC_BRUSH));
        SetDCBrushColor(dc, fill);
        if (radio) Ellipse(dc, left, top, right, bottom);
        else Rectangle(dc, left, top, right, bottom);
        if (checked || indeterminate) {
            SelectObject(dc, stroke);
            if (radio) {
                SetDCBrushColor(dc, mark);
                const int pad = size / 3;
                Ellipse(dc, left + pad, top + pad, right - pad, bottom - pad);
            } else if (indeterminate) {
                SetDCBrushColor(dc, mark);
                const RECT bar = {left + size / 4, top + size / 2, right - size / 4,
                                  top + size / 2 + (size >= 18 ? 2 : 1)};
                FillRect(dc, &bar, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
            } else {
                MoveToEx(dc, left + size / 5, top + size / 2, nullptr);
                LineTo(dc, left + size * 2 / 5, bottom - size / 4);
                LineTo(dc, right - size / 5, top + size / 4);
            }
        }
    }
    RestoreDC(dc, saved);
    if (edge) DeleteObject(edge);
    if (stroke) DeleteObject(stroke);
}

bool PaintChoiceButton(HWND hWnd, UINT message, WPARAM wParam) {
    if (!g_active || !g_backgroundBrush) return false;
    RECT rc = {};
    if (!GetClientRect(hWnd, &rc) || rc.right <= 0 || rc.bottom <= 0) return false;
    PAINTSTRUCT ps = {};
    HDC dc = message == WM_PAINT ? BeginPaint(hWnd, &ps) : reinterpret_cast<HDC>(wParam);
    if (!dc) return false;
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR type = style & BS_TYPEMASK;
    const bool radio = type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON;
    const bool tri = type == BS_3STATE || type == BS_AUTO3STATE;
    const LRESULT check = SendMessageW(hWnd, BM_GETCHECK, 0, 0);
    const LRESULT state = SendMessageW(hWnd, BM_GETSTATE, 0, 0);
    const bool disabled = !IsWindowEnabled(hWnd);
    const LRESULT uiState = SendMessageW(hWnd, WM_QUERYUISTATE, 0, 0);
    FillRect(dc, &rc, g_backgroundBrush);

    int dpi = GetDeviceCaps(dc, LOGPIXELSX);
    if (dpi <= 0) dpi = 96;
    const int size = MulDiv(13, dpi, 96), gap = MulDiv(4, dpi, 96);
    const bool textLeft = (style & BS_LEFTTEXT) != 0;
    RECT area = rc;
    if (textLeft) area.right -= size + gap; else area.left += size + gap;

    WCHAR text[1024] = {};
    GetWindowTextW(hWnd, text, ARRAYSIZE(text));
    HFONT font = reinterpret_cast<HFONT>(SendMessageW(hWnd, WM_GETFONT, 0, 0));
    HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;
    TEXTMETRICW tm = {};
    GetTextMetricsW(dc, &tm);
    UINT format = DT_EXPANDTABS | ((style & BS_MULTILINE) ? DT_WORDBREAK : DT_SINGLELINE);
    switch (style & BS_CENTER) {
        case BS_RIGHT: format |= DT_RIGHT; break;
        case BS_CENTER: format |= DT_CENTER; break;
        default: format |= DT_LEFT; break;
    }
    if (uiState & UISF_HIDEACCEL) format |= DT_HIDEPREFIX;
    RECT calc = area;
    DrawTextW(dc, text, -1, &calc, format | DT_CALCRECT);
    const int textHeight = calc.bottom - calc.top;
    int top;
    switch (style & BS_VCENTER) {
        case BS_TOP: top = area.top; break;
        case BS_BOTTOM: top = area.bottom - textHeight; break;
        default: top = area.top + (area.bottom - area.top - textHeight) / 2; break;
    }
    RECT textRect = {area.left, top, area.right, top + textHeight};
    const int lineHeight = tm.tmHeight > 0 ? tm.tmHeight : textHeight;
    const int glyphTop = top + (lineHeight - size) / 2;
    const int glyphLeft = textLeft ? rc.right - size : rc.left;
    DrawChoiceGlyphDirect(dc, glyphLeft, glyphTop, size, radio,
                          check == BST_CHECKED, tri && check == BST_INDETERMINATE,
                          disabled, (state & BST_PUSHED) != 0);
    SetBkMode(dc, TRANSPARENT);
    const COLORREF oldColour = SetTextColor(dc, disabled ? kColorTextDisabled : kColorText);
    DrawTextW(dc, text, -1, &textRect, format);
    SetTextColor(dc, oldColour);
    if ((state & BST_FOCUS) && !(uiState & UISF_HIDEFOCUS) && text[0]) {
        RECT focus = {textRect.left - 1, textRect.top - 1,
                      textRect.left + (calc.right - calc.left) + 1, textRect.bottom + 1};
        if (focus.right > rc.right) focus.right = rc.right;
        DrawFocusRect(dc, &focus);
    }
    if (oldFont) SelectObject(dc, oldFont);
    if (message == WM_PAINT) EndPaint(hWnd, &ps);
    return true;
}

// Some classic disabled push buttons do not call DrawThemeBackground.
// Keep the native caption and input, and replace only a matching full face.
bool DrawDarkDisabledClassicPush(HDC dc, const RECT* bounds, UINT type, UINT state) {
    if (!g_active || !g_drawingPushButton || !dc || !bounds ||
        type != DFC_BUTTON || (state & 0x1f) != DFCS_BUTTONPUSH ||
        WindowFromDC(dc) != g_drawingPushButton ||
        !IsWindow(g_drawingPushButton) || IsWindowEnabled(g_drawingPushButton) ||
        !g_backgroundBrush || !g_lineBrush) return false;
    const UINT supported = DFCS_BUTTONPUSH | DFCS_INACTIVE | DFCS_FLAT | DFCS_MONO;
    if (state & ~supported) return false;
    WCHAR cls[16] = {}, parentClass[16] = {};
    HWND parent = GetParent(g_drawingPushButton);
    RECT client = {};
    if (!parent || !GetClassNameW(g_drawingPushButton, cls, ARRAYSIZE(cls)) ||
        wcscmp(cls, L"Button") || !GetClassNameW(parent, parentClass, ARRAYSIZE(parentClass)) ||
        wcscmp(parentClass, L"#32770") || !GetClientRect(g_drawingPushButton, &client) ||
        !EqualRect(bounds, &client) || client.right < 8 || client.bottom < 8 ||
        client.right > 4096 || client.bottom > 4096) return false;
    POINT viewport = {}, window = {};
    if (GetMapMode(dc) != MM_TEXT || GetLayout(dc) != 0 ||
        !GetViewportOrgEx(dc, &viewport) || viewport.x || viewport.y ||
        !GetWindowOrgEx(dc, &window) || window.x || window.y) return false;
    // Prepare both parts off-screen; a failed frame must not leave a partial face.
    HDC memory = CreateCompatibleDC(dc);
    HBITMAP bitmap = memory ? CreateCompatibleBitmap(dc, client.right, client.bottom) : nullptr;
    HGDIOBJ previous = bitmap ? SelectObject(memory, bitmap) : nullptr;
    const bool selected = previous && previous != HGDI_ERROR;
    const bool prepared = selected && FillRect(memory, &client, g_backgroundBrush) &&
                          FrameRect(memory, &client, g_lineBrush);
    const bool drawn = prepared && BitBlt(dc, 0, 0, client.right, client.bottom,
                                         memory, 0, 0, SRCCOPY);
    if (selected) SelectObject(memory, previous);
    if (bitmap) DeleteObject(bitmap);
    if (memory) DeleteDC(memory);
    return drawn;
}

BOOL WINAPI DrawFrameControl_Hook(HDC dc, LPRECT rect, UINT type, UINT state) {
    if (DrawDarkDisabledClassicPush(dc, rect, type, state)) return TRUE;
    if (DrawDarkChoiceGlyph(dc, rect, type, state)) return TRUE;
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
    if (g_active && color == kColorDiskStockNavy &&
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(__builtin_return_address(0)),
                           &caller) && caller && caller == GetModuleHandleW(L"dmview.ocx")) {
        HBRUSH result = pCreateHatchBrush(hatch, kColorAccent);
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
    COLORREF face = kColorSurface;
    if (state == 2) face = part == WP_MDICLOSEBUTTON ? kColorCloseHot : kColorHover;
    if (state == 3) face = part == WP_MDICLOSEBUTTON ? kColorCloseDown : kColorPressed;
    COLORREF edge = part == WP_MDICLOSEBUTTON && (state == 2 || state == 3)
                        ? face : kColorLine;
    COLORREF ink = state == 4 ? kColorTextDisabled : kColorText;
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

// Paint scoped push-button faces; retain native input and focus handling.
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
    // Hosted WinForms pages take their body and group-box frame from the light
    // theme; draw them in the mod palette while one is painting.
    if (g_active && g_formsPaintDepth > 0 && dc && bounds) {
        constexpr int kTabPane = 9, kTabBody = 10, kTabAeroWizardBody = 11;
        const bool pagePart = part == kTabPane || part == kTabBody ||
                              part == kTabAeroWizardBody;
        const bool groupPart =
            part == BP_GROUPBOX && (state == GBS_NORMAL || state == GBS_DISABLED);
        const bool pageBody = pagePart && ThemeClassIs(theme, L"Tab");
        const bool groupFrame = groupPart && ThemeClassIs(theme, L"Button");
        HBRUSH brush = pageBody ? g_backgroundBrush : groupFrame ? g_lineBrush : nullptr;
        const int saved = brush ? SaveDC(dc) : 0;
        if (saved) {
            const bool clipOk = !clip || IntersectClipRect(
                dc, clip->left, clip->top, clip->right, clip->bottom) != ERROR;
            const BOOL drawn = clipOk && (pageBody ? FillRect(dc, bounds, brush)
                                                   : FrameRect(dc, bounds, brush));
            RestoreDC(dc, saved);
            if (drawn) {
                return S_OK;
            }
        }
    }
    // PBS_DEFAULTED_ANIMATING (6) is the pulsing frame of the default button.
    constexpr int kPushDefaultedAnimating = 6;
    HWND ownerDrawButton = g_formsButtonPaint && g_formsButtonHooksReady &&
                                   ThemeClassIs(theme, L"Button")
                               ? g_formsButtonPaint->window : nullptr;
    HWND button = g_drawingPushButton
                      ? (g_pushButtonThemeHooksReady && ThemeClassIs(theme, L"Button")
                             ? g_drawingPushButton : nullptr)
                      : g_drawingSummaryButton ? g_drawingSummaryButton : ownerDrawButton;
    if (g_active && button && dc && bounds &&
        part == BP_PUSHBUTTON && state >= PBS_NORMAL &&
        state <= kPushDefaultedAnimating &&
        GetClientRect(button, &client) &&
        bounds->right - bounds->left == client.right - client.left &&
        bounds->bottom - bounds->top == client.bottom - client.top) {
        HBRUSH fill = g_tabSelectedBrush;
        if (state == PBS_DISABLED) {
            fill = g_backgroundBrush;
        } else if (state == PBS_PRESSED) {
            fill = g_pressedBrush;
        } else if (state == PBS_HOT) {
            fill = g_hoverBrush;
        }
        // The default button keeps an accent frame.
        HBRUSH edge = (state == PBS_DEFAULTED || state == kPushDefaultedAnimating)
                          ? g_accentBrush : g_lineBrush;
        int saved = SaveDC(dc);
        if (saved && fill && edge) {
            const bool clipOk = !clip || IntersectClipRect(
                dc, clip->left, clip->top, clip->right, clip->bottom) != ERROR;
            BOOL filled = clipOk && FillRect(dc, bounds, fill);
            BOOL framed = clipOk && FrameRect(dc, bounds, edge);
            RestoreDC(dc, saved);
            if (filled && framed) {
                if (ownerDrawButton && button == ownerDrawButton) {
                    g_formsButtonPaint->faceDrawn = true;
                    g_formsButtonPaint->disabled = state == PBS_DISABLED;
                } else if (g_drawingPushButton && button == g_drawingPushButton) {
                    g_pushButtonFaceDrawn = true;
                    g_pushButtonFaceDisabled = state == PBS_DISABLED;
                }
                return S_OK;
            }
        } else if (saved) {
            RestoreDC(dc, saved);
        }
    }
    return pDrawThemeBackground ? pDrawThemeBackground(theme, dc, part, state, bounds, clip)
                                : E_FAIL;
}

// Push button caption: the theme text colour is black, so while a scoped button
// paints, its caption is drawn in the mod palette.
using DrawThemeTextEx_t = decltype(&DrawThemeTextEx);
using DrawThemeText_t = decltype(&DrawThemeText);
DrawThemeTextEx_t pDrawThemeTextEx = nullptr;
DrawThemeText_t pDrawThemeText = nullptr;

bool PushButtonTextColour(HTHEME theme, HDC dc, int part, int state, COLORREF* colour) {
    if (!g_active || !dc || part != BP_PUSHBUTTON) return false;
    const bool nativePush = g_pushButtonThemeHooksReady && g_drawingPushButton &&
                            g_pushButtonFaceDrawn;
    const bool formsPush = g_formsButtonHooksReady && g_formsButtonPaint &&
                           g_formsButtonPaint->faceDrawn;
    if ((!nativePush && !formsPush) || !ThemeClassIs(theme, L"Button")) return false;
    const bool disabled = nativePush ? state == PBS_DISABLED
                                     : g_formsButtonPaint->disabled;
    *colour = disabled ? kColorTextDisabled : kColorText;
    return true;
}

HRESULT WINAPI DrawThemeTextEx_Hook(HTHEME theme, HDC dc, int part, int state,
                                    LPCWSTR text, int length, DWORD flags,
                                    LPRECT rect, const DTTOPTS* options) {
    if (!pDrawThemeTextEx) {
        return E_FAIL;
    }
    COLORREF colour = 0;
    if (PushButtonTextColour(theme, dc, part, state, &colour)) {
        DTTOPTS merged = {};
        if (options) {
            memcpy(&merged, options,
                   options->dwSize < sizeof(merged) ? options->dwSize : sizeof(merged));
        }
        merged.dwSize = sizeof(merged);
        merged.dwFlags |= DTT_TEXTCOLOR;
        merged.crText = colour;
        return pDrawThemeTextEx(theme, dc, part, state, text, length, flags, rect,
                                &merged);
    }
    return pDrawThemeTextEx(theme, dc, part, state, text, length, flags, rect, options);
}

HRESULT WINAPI DrawThemeText_Hook(HTHEME theme, HDC dc, int part, int state,
                                  LPCWSTR text, int length, DWORD flags,
                                  DWORD flags2, LPCRECT rect) {
    if (!pDrawThemeText) {
        return E_FAIL;
    }
    COLORREF colour = 0;
    if (pDrawThemeTextEx && rect && !flags2 &&
        PushButtonTextColour(theme, dc, part, state, &colour)) {
        DTTOPTS options = {};
        options.dwSize = sizeof(options);
        options.dwFlags = DTT_TEXTCOLOR;
        options.crText = colour;
        RECT copy = *rect;
        return pDrawThemeTextEx(theme, dc, part, state, text, length, flags, &copy,
                                &options);
    }
    return pDrawThemeText(theme, dc, part, state, text, length, flags, flags2, rect);
}

// Adjust only the scoped Scheduler summary title foreground.
using DrawTextExW_t = decltype(&DrawTextExW);
DrawTextExW_t pDrawTextExW = nullptr;

int WINAPI DrawTextExW_Hook(HDC dc, LPWSTR text, int count, LPRECT rect,
                            UINT format, LPDRAWTEXTPARAMS params) {
    if (!pDrawTextExW) {
        return 0;
    }
    if (g_active && g_formsButtonHooksReady && g_formsButtonPaint &&
        g_formsButtonPaint->faceDrawn && dc && !(format & DT_CALCRECT)) {
        COLORREF previous = SetTextColor(
            dc, g_formsButtonPaint->disabled ? kColorTextDisabled : kColorText);
        if (previous != CLR_INVALID) {
            int result = pDrawTextExW(dc, text, count, rect, format, params);
            SetTextColor(dc, previous);
            return result;
        }
    }
    if (g_active && g_pushButtonThemeHooksReady && g_drawingPushButton &&
        g_pushButtonFaceDrawn && dc && !(format & DT_CALCRECT)) {
        COLORREF previous = SetTextColor(
            dc, g_pushButtonFaceDisabled ? kColorTextDisabled : kColorText);
        if (previous != CLR_INVALID) {
            int result = pDrawTextExW(dc, text, count, rect, format, params);
            SetTextColor(dc, previous);
            return result;
        }
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
    if (g_active && !(format & DT_CALCRECT) && IsActionsPaneDc(dc)) {
        const COLORREF textColour = GetTextColor(dc);
        const bool darkText = textColour != CLR_INVALID &&
                              GetRValue(textColour) < 80 &&
                              GetGValue(textColour) < 80 &&
                              GetBValue(textColour) < 80;
        const bool lightBk = GetBkMode(dc) == OPAQUE && IsLightSolidColour(GetBkColor(dc));
        if (darkText || lightBk) {
            COLORREF oldText = darkText ? SetTextColor(dc, kColorText) : CLR_INVALID;
            COLORREF oldBk = lightBk ? SetBkColor(dc, kColorBackground) : CLR_INVALID;
            int result = pDrawTextExW(dc, text, count, rect, format, params);
            if (oldText != CLR_INVALID) SetTextColor(dc, oldText);
            if (oldBk != CLR_INVALID) SetBkColor(dc, oldBk);
            return result;
        }
    }
    return pDrawTextExW(dc, text, count, rect, format, params);
}

bool IsActionsCaptionColour(int index) {
    return index == kColActiveCaption || index == kColCaptionText ||
           index == kColGradientActiveCaption || index == 3 || index == 19 || index == 28;
}

DWORD WINAPI GetSysColor_Hook(int nIndex) {
    // The native description uses COLOR_WINDOW for text on a dark background.
    if (g_active && g_drawingMmcDescription && nIndex == kColWindow) {
        return kColorText;
    }
    if (g_active && g_actionsPanePaint && IsActionsCaptionColour(nIndex)) {
        return (nIndex == kColCaptionText || nIndex == 19) ? kColorText : kColorBackground;
    }
    if (g_active && g_drawingActionsContent && IsActionsCaptionColour(nIndex) &&
        IsDirectUiCaller(__builtin_return_address(0))) {
        return nIndex == kColCaptionText ? kColorText : kColorBackground;
    }
    if (g_active && (nIndex == kColActiveCaption || nIndex == kColCaptionText) &&
        IsExtendedViewColourCaller(__builtin_return_address(0))) {
        return nIndex == kColCaptionText ? kColorText : kColorBackground;
    }
    COLORREF mapped = g_active ? MapSystemColour(nIndex) : kNotMapped;
    if (mapped != kNotMapped) {
        return mapped;
    }
    return pGetSysColor(nIndex);
}

using GetSysColorBrush_t = HBRUSH(WINAPI*)(int);
GetSysColorBrush_t pGetSysColorBrush = nullptr;

HBRUSH WINAPI GetSysColorBrush_Hook(int nIndex) {
    if (g_nativeFormsBrushScope) return pGetSysColorBrush(nIndex);
    if (g_active && g_actionsPanePaint && IsActionsCaptionColour(nIndex)) {
        HBRUSH brush = g_sysBrushes[(nIndex == kColCaptionText || nIndex == 19) ? kColWindowText : kColWindow];
        if (brush) {
            return brush;
        }
    }
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

// Dialogs built inside user32 (message boxes, common dialogs) never reach the
// creation hooks above. Watch each thread that creates windows for the
// dialog's WM_INITDIALOG instead. This hook is not on the stack while a modal
// dialog runs, so it cannot hold up unloading the mod.
LRESULT CALLBACK DialogInitHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        ActiveThreadHookGuard hookGuard;
        if (g_active.load(std::memory_order_acquire) &&
            !g_unloading.load(std::memory_order_acquire)) {
            const CWPRETSTRUCT* info = reinterpret_cast<const CWPRETSTRUCT*>(lParam);
            if (info && info->message == WM_INITDIALOG && info->hwnd) {
                WCHAR cls[16] = {};
                if (GetClassNameW(info->hwnd, cls, ARRAYSIZE(cls)) && !wcscmp(cls, L"#32770")) {
                    ThemeDialog(info->hwnd);
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

std::unordered_map<DWORD, HHOOK> g_dialogInitHooks;

void EnsureDialogInitHook() {
    static thread_local bool t_installed = false;
    if (t_installed || !g_active || g_unloading) return;
    const DWORD threadId = GetCurrentThreadId();
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROCRET, DialogInitHook, nullptr, threadId);
    if (hook) {
        std::lock_guard<std::mutex> lock(g_stateLock);
        if (g_unloading) {
            UnhookWindowsHookEx(hook);
            return;
        }
        g_dialogInitHooks[threadId] = hook;
        t_installed = true;
    }
}

void RemoveDialogInitHooks() {
    std::unordered_map<DWORD, HHOOK> hooks;
    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        hooks.swap(g_dialogInitHooks);
    }
    for (auto& entry : hooks) UnhookWindowsHookEx(entry.second);
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
        const DWORD error = GetLastError();
        EnsureDialogInitHook();
        ApplyToWindow(hWnd);
        CoverWebView(hWnd);
        // Handle only windows owned by the current thread.
        const DWORD threadId = GetCurrentThreadId();
        if (ThreadIsPending(threadId)) {
            ClearPendingThread(threadId);
            WalkThreadWindows(threadId);
        }
        SetLastError(error);
    }
    return hWnd;
}

// Some MMC windows (the OCX view host) are created through the ANSI entry point,
// which the wide hook never sees. Treat them the same way.
using CreateWindowExA_t = decltype(&CreateWindowExA);
CreateWindowExA_t pCreateWindowExA = nullptr;

HWND WINAPI CreateWindowExA_Hook(DWORD dwExStyle, LPCSTR lpClassName,
                                 LPCSTR lpWindowName, DWORD dwStyle, int x,
                                 int y, int nWidth, int nHeight, HWND hWndParent,
                                 HMENU hMenu, HINSTANCE hInstance,
                                 void* lpParam) {
    HWND hWnd = pCreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                 x, y, nWidth, nHeight, hWndParent, hMenu,
                                 hInstance, lpParam);
    if (hWnd && g_active) {
        const DWORD error = GetLastError();
        EnsureDialogInitHook();
        ApplyToWindow(hWnd);
        CoverWebView(hWnd);
        SetLastError(error);
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
        const DWORD error = GetLastError();
        ThemeDialog(hWnd);
        SetLastError(error);
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
        const DWORD error = GetLastError();
        ThemeDialog(hWnd);
        SetLastError(error);
    }
    return hWnd;
}

// The TPM chooser calls SetWindowLongPtr(DWLP_USER) during WM_INITDIALOG.
// Register the dialog now and post the full theming pass after initialization;
// unlike a DialogBox hook, this call does not stay on the stack for the modal loop.
using SetWindowLongPtrW_t = decltype(&SetWindowLongPtrW);
SetWindowLongPtrW_t pSetWindowLongPtrW = nullptr;

bool IsTpmChooserInit(HWND hWnd, int index, void* address) {
    HMODULE caller = nullptr;
    WCHAR cls[16] = {};
    if (!g_active || !hWnd || index != DWLP_USER || !OnOwningThread(hWnd) ||
        !GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"#32770") ||
        !address || !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(address), &caller) ||
        !caller || caller != GetModuleHandleW(L"tpmcompc.dll")) return false;
    HWND local = GetDlgItem(hWnd, 5001), remote = GetDlgItem(hWnd, 5002);
    HWND edit = GetDlgItem(hWnd, 5003), browse = GetDlgItem(hWnd, 5004);
    WCHAR a[16] = {}, b[16] = {}, c[16] = {}, d[16] = {};
    return local && remote && edit && browse &&
        GetClassNameW(local, a, ARRAYSIZE(a)) && !wcscmp(a, L"Button") &&
        GetClassNameW(remote, b, ARRAYSIZE(b)) && !wcscmp(b, L"Button") &&
        GetClassNameW(edit, c, ARRAYSIZE(c)) && !wcscmp(c, L"Edit") &&
        GetClassNameW(browse, d, ARRAYSIZE(d)) && !wcscmp(d, L"Button");
}

LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND hWnd, int index, LONG_PTR value) {
    if (!pSetWindowLongPtrW) return 0;
    LONG_PTR result = pSetWindowLongPtrW(hWnd, index, value);
    const DWORD error = GetLastError();
    if (IsTpmChooserInit(hWnd, index, __builtin_return_address(0)) &&
        SubclassWith(hWnd, WindowKind::Dialog) &&
        SetPropW(hWnd, kTpmDialogPending, reinterpret_cast<HANDLE>(1))) {
        if (!PostMessageW(hWnd, OwnThreadSetupMessage(), 0, 0)) {
            RemovePropW(hWnd, kTpmDialogPending);
        }
    }
    SetLastError(error);
    return result;
}

BOOL CALLBACK SendSysColorChange(HWND hWnd, LPARAM);

BOOL CALLBACK WalkTopSysColor(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }
    SendSysColorChange(hWnd, 0);
    EnumChildWindows(hWnd, SendSysColorChange, 0);
    return TRUE;
}

thread_local std::unordered_set<DWORD>* t_silentThreads = nullptr;

BOOL CALLBACK SendSysColorChange(HWND hWnd, LPARAM) {
    const DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    // A thread that did not answer once will not answer for its other windows
    // either; do not wait 200 ms for every one of them.
    if (t_silentThreads && t_silentThreads->count(threadId)) return TRUE;
    DWORD_PTR ignored = 0;
    if (!SendMessageTimeoutW(hWnd, WM_SYSCOLORCHANGE, 0, 0,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &ignored) &&
        t_silentThreads) {
        t_silentThreads->insert(threadId);
    }
    return TRUE;
}

// Notify controls of system-colour changes after theme transitions.
void BroadcastSysColorChange() {
    std::unordered_set<DWORD> silent;
    t_silentThreads = &silent;
    EnumWindows(WalkTopSysColor, 0);
    t_silentThreads = nullptr;
}

BOOL CALLBACK WalkDotNetSysColor(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    const DWORD threadId = GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId() || !threadId) {
        return TRUE;
    }
    WCHAR cls[64] = {};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) ||
        wcsncmp(cls, L".NET-BroadcastEventWindow", 25) != 0) {
        return TRUE;
    }
    if (t_silentThreads && t_silentThreads->count(threadId)) {
        return TRUE;
    }
    DWORD_PTR ignored = 0;
    if (!SendMessageTimeoutW(hWnd, WM_SYSCOLORCHANGE, 0, 0,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &ignored) &&
        t_silentThreads) {
        t_silentThreads->insert(threadId);
    }
    return TRUE;
}

// Refresh .NET SystemEvents colour caches before thread walks repaint WinForms panes.
void NotifyDotNetSysColorChange() {
    std::unordered_set<DWORD> silent;
    t_silentThreads = &silent;
    EnumWindows(WalkDotNetSysColor, 0);
    t_silentThreads = nullptr;
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
    RedrawWindow(hWnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
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

// Recognise thin Forms labels without caching a dark system
// brush. The control may paint before the rest of its page has been created.
bool IsFormsThinStaticSeparator(HWND parent, HWND child) {
    if (!parent || !child || GetParent(child) != parent ||
        !WindowClassStarts(parent, L"WindowsForms10.Window.") ||
        !WindowClassStarts(child, L"WindowsForms10.STATIC.") ||
        (GetWindowLongPtrW(child, GWL_STYLE) & SS_TYPEMASK) != SS_LEFT) {
        return false;
    }
    DWORD_PTR length = 0;
    RECT bounds = {};
    if (!SendMessageTimeoutW(child, WM_GETTEXTLENGTH, 0, 0,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &length) || length ||
        !GetWindowRect(child, &bounds)) {
        return false;
    }
    const long long width = static_cast<long long>(bounds.right) - bounds.left;
    const long long height = static_cast<long long>(bounds.bottom) - bounds.top;
    int threshold = ScaleForWindow(child, 6);
    if (threshold < 1) threshold = 1;
    if (width <= 0 || height <= 0 || height > threshold || width <= height * 4) {
        return false;
    }
    return true;
}

// Subclass reference data identifies the handler; WM_NCDESTROY removes bookkeeping.
LRESULT ModSubclassInner(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,
                         UINT_PTR uIdSubclass, DWORD_PTR refData) {
    const WindowKind kind = static_cast<WindowKind>(refData);

    if (message == WM_NCDESTROY) {
        RemovePropW(hWnd, kOwnedModalThemeProperty);
        RemovePropW(hWnd, kTpmDialogPending);
        RemovePropW(hWnd, kFormsFrameProperty);
        RemovePropW(hWnd, kEventFrameRetryProperty);
        RemovePropW(hWnd, kSummaryPaintRetryProperty);
        RemoveWindowSubclass(hWnd, ModSubclassProc, uIdSubclass);
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
            for (int i = 0; i < static_cast<int>(ARRAYSIZE(g_statusCache)); i++) {
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

    if (kind == WindowKind::FormHost && message == WM_ERASEBKGND &&
        MmcFormHost::EraseEmptyFormHost(
            hWnd, reinterpret_cast<HDC>(wParam), g_backgroundBrush,
            GetModuleHandleW(L"mmcndmgr.dll"), []() -> bool { return g_active.load(); })) {
        return TRUE;
    }

    if (kind == WindowKind::WinFormsSurface && message == WM_WINDOWPOSCHANGED) {
        RemovePropW(hWnd, kEventFrameRetryProperty);
    }
    if ((kind == WindowKind::WinFormsSurface || kind == WindowKind::Tab) &&
        message == WM_WINDOWPOSCHANGED) {
        RemovePropW(hWnd, kSummaryPaintRetryProperty);
    }
    if ((kind == WindowKind::WinFormsSurface || kind == WindowKind::Tab) &&
        message == WM_PAINT) {
        RefreshFormsSummaryFramesFor(hWnd, false, true);
        if (!GetPropW(hWnd, kFormsFrameProperty) &&
            !GetPropW(hWnd, kEventFrameRetryProperty) &&
            (GetFocus() != hWnd || IsFormsEventRichEdit(hWnd)) &&
            (IsFormsEventList(hWnd) || IsFormsEventRichEdit(hWnd)) &&
            SetPropW(hWnd, kEventFrameRetryProperty, reinterpret_cast<HANDLE>(1))) {
            if (!RefreshEventDetailFramesFor(hWnd)) {
                RemovePropW(hWnd, kEventFrameRetryProperty);
            }
        }
    }

    // A WinForms list can be created under a parking window and reparented
    // afterwards. Recheck its final host before native painting begins.
    if (kind == WindowKind::WinFormsSurface &&
        (message == WM_PAINT || message == WM_ERASEBKGND ||
         message == WM_PRINTCLIENT)) {
        RetryHostedListPalette(hWnd);
    }

    // Let the native combo handler run before adjusting the scoped colour response.
    if ((kind == WindowKind::WinFormsCombo ||
         kind == WindowKind::WinFormsComboParent ||
         kind == WindowKind::WinFormsSurface ||
         kind == WindowKind::WinFormsBackdrop) &&
        (message == WM_CTLCOLORSTATIC || message == WM_CTLCOLOREDIT ||
         message == WM_CTLCOLORLISTBOX) && wParam && lParam &&
        g_backgroundBrush) {
        HWND part = reinterpret_cast<HWND>(lParam);
        HWND combo = kind == WindowKind::WinFormsCombo ? hWnd : part;
        if (kind != WindowKind::WinFormsCombo && !IsMmcSummaryCombo(combo)) {
            combo = GetParent(part);
        }
        if (IsMmcSummaryComboPart(combo, part)) {
            const bool previousBrushScope = g_nativeFormsBrushScope;
            g_nativeFormsBrushScope = true;
            LRESULT native = DefSubclassProc(hWnd, message, wParam, lParam);
            g_nativeFormsBrushScope = previousBrushScope;
            if (!g_active || !IsWindow(hWnd) || !IsMmcSummaryComboPart(combo, part)) {
                return native;
            }
            HDC dc = reinterpret_cast<HDC>(wParam);
            SetTextColor(dc, IsWindowEnabled(combo) ? kColorText : kColorTextDisabled);
            SetBkColor(dc, kColorBackground);
            return reinterpret_cast<LRESULT>(g_backgroundBrush);
        }
    }

    // Keep stock brushes cached by WinForms native; substitute the equivalent
    // dark brush only for this paint-time colour response.
    if ((kind == WindowKind::WinFormsSurface || kind == WindowKind::WinFormsBackdrop ||
         kind == WindowKind::WinFormsComboParent) && message == WM_CTLCOLORSTATIC &&
        wParam && lParam && pGetSysColorBrush &&
        IsFormsThinStaticSeparator(hWnd, reinterpret_cast<HWND>(lParam))) {
        const bool previousBrushScope = g_nativeFormsBrushScope;
        g_nativeFormsBrushScope = true;
        LRESULT native = DefSubclassProc(hWnd, message, wParam, lParam);
        g_nativeFormsBrushScope = previousBrushScope;
        if (g_active && IsWindow(hWnd) &&
            IsFormsThinStaticSeparator(hWnd, reinterpret_cast<HWND>(lParam))) {
            HBRUSH nativeBrush = reinterpret_cast<HBRUSH>(native);
            HBRUSH replacement = nullptr;
            COLORREF replacementColour = kNotMapped;
            for (int index = 0; nativeBrush && index < kSysColourCount; ++index) {
                if (g_sysBrushes[index] &&
                    nativeBrush == pGetSysColorBrush(index)) {
                    COLORREF colour = MapSystemColour(index);
                    if (replacement && colour != replacementColour) {
                        replacement = nullptr;
                        break;
                    }
                    replacement = g_sysBrushes[index];
                    replacementColour = colour;
                }
            }
            if (replacement &&
                SetBkColor(reinterpret_cast<HDC>(wParam), replacementColour) != CLR_INVALID) {
                return reinterpret_cast<LRESULT>(replacement);
            }
        }
        return native;
    }

    // Prevent a system-colour brush from being cached as the dark brush by
    // an editable Forms multiline field; colour only the current response.
    if ((kind == WindowKind::WinFormsSurface || kind == WindowKind::WinFormsBackdrop ||
         kind == WindowKind::WinFormsComboParent) && message == WM_CTLCOLOREDIT &&
        wParam && lParam && g_backgroundBrush &&
        IsFormsWritableMultilineEdit(hWnd, reinterpret_cast<HWND>(lParam))) {
        const bool previousBrushScope = g_nativeFormsBrushScope;
        g_nativeFormsBrushScope = true;
        LRESULT native = DefSubclassProc(hWnd, message, wParam, lParam);
        g_nativeFormsBrushScope = previousBrushScope;
        if (g_active && IsWindow(hWnd) &&
            IsFormsWritableMultilineEdit(hWnd, reinterpret_cast<HWND>(lParam)) &&
            ColourFormsReadOnlyEdit(reinterpret_cast<HDC>(wParam),
                                    reinterpret_cast<HBRUSH>(native))) {
            return reinterpret_cast<LRESULT>(g_backgroundBrush);
        }
        return native;
    }

    // Single-line fields have the same cache risk as multiline Forms edits.
    if ((kind == WindowKind::WinFormsSurface || kind == WindowKind::WinFormsBackdrop ||
         kind == WindowKind::WinFormsComboParent) && message == WM_CTLCOLOREDIT &&
        wParam && lParam && g_backgroundBrush &&
        IsFormsWritableSingleLineEdit(hWnd, reinterpret_cast<HWND>(lParam))) {
        const bool previousBrushScope = g_nativeFormsBrushScope;
        g_nativeFormsBrushScope = true;
        LRESULT native = DefSubclassProc(hWnd, message, wParam, lParam);
        g_nativeFormsBrushScope = previousBrushScope;
        if (g_active && IsWindow(hWnd) &&
            IsFormsWritableSingleLineEdit(hWnd, reinterpret_cast<HWND>(lParam)) &&
            ColourFormsReadOnlyEdit(reinterpret_cast<HDC>(wParam),
                                    reinterpret_cast<HBRUSH>(native))) {
            return reinterpret_cast<LRESULT>(g_backgroundBrush);
        }
        return native;
    }

    // A WinForms read-only EDIT reports WM_CTLCOLORSTATIC to its parent.
    // Call the Forms handler first so it can reflect the message to the child.
    if ((kind == WindowKind::WinFormsSurface || kind == WindowKind::WinFormsBackdrop ||
         kind == WindowKind::WinFormsComboParent) && message == WM_CTLCOLORSTATIC &&
        wParam && lParam && g_backgroundBrush &&
        IsFormsReadOnlyEdit(hWnd, reinterpret_cast<HWND>(lParam))) {
        const bool previousBrushScope = g_nativeFormsBrushScope;
        g_nativeFormsBrushScope = true;
        LRESULT native = DefSubclassProc(hWnd, message, wParam, lParam);
        g_nativeFormsBrushScope = previousBrushScope;
        if (g_active && IsWindow(hWnd) &&
            IsFormsReadOnlyEdit(hWnd, reinterpret_cast<HWND>(lParam)) &&
            ColourFormsReadOnlyEdit(reinterpret_cast<HDC>(wParam),
                                    reinterpret_cast<HBRUSH>(native))) {
            return reinterpret_cast<LRESULT>(g_backgroundBrush);
        }
        return native;
    }

    // Disabled edits always use the system's gray foreground. Keep their
    // disabled state and native text rendering; replace only a legible stock face.
    if ((kind == WindowKind::WinFormsSurface || kind == WindowKind::WinFormsBackdrop ||
         kind == WindowKind::WinFormsComboParent) && message == WM_CTLCOLORSTATIC &&
        wParam && lParam && g_backgroundBrush &&
        IsFormsDisabledEdit(hWnd, reinterpret_cast<HWND>(lParam))) {
        const bool previousBrushScope = g_nativeFormsBrushScope;
        g_nativeFormsBrushScope = true;
        LRESULT native = DefSubclassProc(hWnd, message, wParam, lParam);
        g_nativeFormsBrushScope = previousBrushScope;
        if (g_active && IsWindow(hWnd) &&
            IsFormsDisabledEdit(hWnd, reinterpret_cast<HWND>(lParam)) &&
            ColourFormsDisabledEdit(reinterpret_cast<HDC>(wParam),
                                    reinterpret_cast<HBRUSH>(native))) {
            return reinterpret_cast<LRESULT>(g_backgroundBrush);
        }
        return native;
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

    // Scope DrawEdge and light fills to the Actions caption erase and paint.
    if ((kind == WindowKind::ActionsPane ||
         (message == WM_ERASEBKGND && IsMmcActionsPane(hWnd))) &&
        (message == WM_ERASEBKGND || message == WM_PAINT ||
         message == WM_PRINTCLIENT)) {
        HWND previousPane = g_actionsPanePaint;
        HDC previous = g_actionsEraseDc;
        g_actionsPanePaint = hWnd;
        if (message == WM_ERASEBKGND || message == WM_PRINTCLIENT) {
            g_actionsEraseDc = reinterpret_cast<HDC>(wParam);
        }
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
        if (message == WM_ERASEBKGND) {
            FixActionsPaleRows(hWnd, g_actionsEraseDc);
        }
        g_actionsEraseDc = previous;
        g_actionsPanePaint = previousPane;
        return result;
    }

    if (kind == WindowKind::PushButton &&
        (message == WM_PAINT || message == WM_PRINTCLIENT ||
         message == WM_ENABLE || message == WM_SETFOCUS ||
         message == WM_KILLFOCUS || message == BM_SETSTATE) &&
        !ButtonHasImage(hWnd)) {
        HWND previous = g_drawingPushButton;
        const bool previousFace = g_pushButtonFaceDrawn;
        const bool previousDisabled = g_pushButtonFaceDisabled;
        ClassicPushPair pair = {hWnd, nullptr, {}, {}, false, false};
        ClassicPushPair* previousClassicPair = g_classicPushPair;
        g_classicPushPair = &pair;
        g_drawingPushButton = hWnd;
        g_pushButtonFaceDrawn = false;
        g_pushButtonFaceDisabled = false;
        LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
        g_classicPushPair = previousClassicPair;
        g_drawingPushButton = previous;
        g_pushButtonFaceDrawn = previousFace;
        g_pushButtonFaceDisabled = previousDisabled;
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
        if (message == WM_PAINT) FixActionsPaleRows(hWnd, nullptr);
        return result;
    }

    if (message == OwnThreadSetupMessage()) {
        return HandleOwnThreadSetup(hWnd, kind);
    }

    // Native buttons sometimes paint directly during state changes, not just WM_PAINT.
    if (kind == WindowKind::ChoiceButton) {
        if (g_active && message == WM_ERASEBKGND && g_backgroundBrush) {
            RECT client = {};
            GetClientRect(hWnd, &client);
            if (FillRect(reinterpret_cast<HDC>(wParam), &client, g_backgroundBrush)) return 1;
        }
        if (g_active && (message == WM_PAINT || message == WM_PRINTCLIENT) &&
            PaintChoiceButton(hWnd, message, wParam)) {
            return 0;
        }
        if (message == WM_PAINT || message == WM_PRINTCLIENT ||
            message == WM_ENABLE || message == WM_SETFOCUS ||
            message == WM_KILLFOCUS || message == BM_SETSTATE ||
            message == BM_SETCHECK) {
            HWND previous = g_drawingChoiceButton;
            g_drawingChoiceButton = hWnd;
            LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
            g_drawingChoiceButton = previous;
            return result;
        }
        return DefSubclassProc(hWnd, message, wParam, lParam);
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
    case WindowKind::PushButton:
    case WindowKind::ChoiceButton:
    case WindowKind::FormHost:
        return DefSubclassProc(hWnd, message, wParam, lParam);
    case WindowKind::WinFormsSurface:
        return HandleFormsSurface(hWnd, message, wParam, lParam);
    case WindowKind::WinFormsBackdrop:
        return HandleBackdrop(hWnd, message, wParam, lParam);
    case WindowKind::DiskHeader:
        return HandleDiskHeader(hWnd, message, wParam, lParam);
    }
    return DefSubclassProc(hWnd, message, wParam, lParam);
}

bool IsHostedWinFormsKind(WindowKind kind) {
    return kind == WindowKind::WinFormsSurface || kind == WindowKind::WinFormsBackdrop ||
           kind == WindowKind::WinFormsLabel ||
           kind == WindowKind::WinFormsCombo ||
           kind == WindowKind::WinFormsComboParent ||
           kind == WindowKind::WinFormsRefresh;
}

// Marks the paint and erase messages of hosted WinForms windows so the theme
// hooks can tell a Task Scheduler page from every other themed drawing.
// The Actions group header leaves a one-pixel row in the native gradient-caption
// colour. Find those rows after the pane paints and redraw them in the line colour.
void FixActionsPaleRows(HWND hWnd, HDC providedDc) {
    if (!g_active || !g_lineBrush) return;
    RECT client = {};
    if (!GetClientRect(hWnd, &client) || client.right < 16 || client.bottom < 2) return;
    const COLORREF native = (pGetSysColor ? pGetSysColor(kColGradientActiveCaption)
                                          : RGB(185, 209, 234)) & 0x00ffffffu;
    HDC dc = providedDc ? providedDc : GetDC(hWnd);
    if (!dc) return;
    const int rows = client.bottom < 400 ? client.bottom : 400;
    for (int y = 0; y < rows; ++y) {
        if ((GetPixel(dc, client.right / 2, y) & 0x00ffffffu) == native &&
            (GetPixel(dc, client.right / 4, y) & 0x00ffffffu) == native &&
            (GetPixel(dc, client.right * 3 / 4, y) & 0x00ffffffu) == native) {
            const RECT row = {0, y, client.right, y + 1};
            FillRect(dc, &row, g_lineBrush);
        }
    }
    if (!providedDc) ReleaseDC(hWnd, dc);
}

bool IsSubclassMessageRelevant(WindowKind kind, UINT message, LPARAM lParam) {
    if (g_ownThreadSetupMessage && message == g_ownThreadSetupMessage) {
        return true;
    }
    switch (kind) {
    case WindowKind::Backdrop:
        return message == WM_ERASEBKGND || message == WM_PAINT ||
               message == WM_ENTERIDLE || message == WM_ACTIVATE;
    case WindowKind::Strip:
        if (message == WM_ERASEBKGND) return true;
        if (message == WM_NOTIFY && lParam) {
            return reinterpret_cast<const NMHDR*>(lParam)->code == NM_CUSTOMDRAW;
        }
        return false;
    case WindowKind::StatusBar:
        return message == WM_NCPAINT || message == WM_ERASEBKGND ||
               message == WM_PAINT || message == SB_SETTEXTW ||
               message == SB_SETPARTS || message == WM_SIZE ||
               message == WM_SETFONT;
    case WindowKind::Toolbar:
        return message == WM_MOUSEMOVE || message == WM_NCMOUSEMOVE ||
               message == WM_MOUSELEAVE;
    case WindowKind::NoBorder:
        return message == WM_NCPAINT;
    case WindowKind::Tab:
        return message == WM_SETFONT || message == WM_ERASEBKGND ||
               message == WM_PAINT || message == WM_LBUTTONDOWN ||
               message == WM_LBUTTONUP || message == WM_KEYUP ||
               message == TCM_SETCURSEL || message == TCM_INSERTITEMW ||
               message == TCM_INSERTITEMA || message == TCM_DELETEITEM ||
               message == TCM_DELETEALLITEMS || message == TCM_SETITEMW ||
               message == TCM_SETITEMA || message == WM_THEMECHANGED ||
               message == WM_SIZE || message == WM_SHOWWINDOW ||
               message == WM_WINDOWPOSCHANGED;
    case WindowKind::Field:
        return message == WM_SETFOCUS || message == WM_KILLFOCUS ||
               message == WM_PAINT;
    case WindowKind::GroupBox:
        return message == WM_ERASEBKGND || message == WM_PAINT;
    case WindowKind::Dialog:
        return message == WM_ENTERIDLE || message == WM_ACTIVATE ||
               message == WM_ERASEBKGND || message == WM_SHOWWINDOW ||
               message == WM_PAINT || message == WM_PRINTCLIENT ||
               message == WM_NCPAINT || message == WM_CTLCOLORDLG ||
               message == WM_CTLCOLORSTATIC || message == WM_CTLCOLORBTN ||
               message == WM_CTLCOLOREDIT || message == WM_CTLCOLORLISTBOX ||
               message == WM_CTLCOLORSCROLLBAR || message == WM_CTLCOLORMSGBOX;
    case WindowKind::MmcCustomTab:
        return message == WM_ERASEBKGND;
    case WindowKind::ActionsPane:
        return message == WM_ERASEBKGND || message == WM_PAINT ||
               message == WM_PRINTCLIENT;
    case WindowKind::TreeFrame:
        return message == WM_NCPAINT || message == WM_PAINT;
    case WindowKind::DescriptionHost:
        return message == WM_DRAWITEM;
    case WindowKind::ActionsContent:
        return message == WM_PAINT || message == WM_PRINTCLIENT ||
               message == WM_ERASEBKGND;
    case WindowKind::WinFormsLabel:
    case WindowKind::WinFormsRefresh:
        return message == WM_PAINT || message == WM_PRINTCLIENT ||
               message == WM_PRINT || message == WM_ERASEBKGND;
    case WindowKind::DiskHeader:
        return message == WM_PAINT || message == WM_PRINTCLIENT;
    case WindowKind::WinFormsCombo:
    case WindowKind::WinFormsComboParent:
        return message == WM_PAINT || message == WM_PRINTCLIENT ||
               message == WM_PRINT || message == WM_ERASEBKGND ||
               message == WM_CTLCOLORSTATIC || message == WM_CTLCOLOREDIT ||
               message == WM_CTLCOLORLISTBOX;
    case WindowKind::WinFormsSurface:
        return message == WM_WINDOWPOSCHANGED || message == WM_PAINT ||
               message == WM_PRINTCLIENT || message == WM_PRINT ||
               message == WM_ERASEBKGND || message == WM_NCPAINT ||
               message == WM_SETFOCUS || message == WM_KILLFOCUS ||
               message == WM_CTLCOLORSTATIC || message == WM_CTLCOLOREDIT ||
               message == WM_CTLCOLORLISTBOX;
    case WindowKind::WinFormsBackdrop:
        return message == WM_ERASEBKGND || message == WM_PAINT ||
               message == WM_PRINTCLIENT || message == WM_PRINT ||
               message == WM_ENTERIDLE || message == WM_ACTIVATE ||
               message == WM_CTLCOLORSTATIC || message == WM_CTLCOLOREDIT ||
               message == WM_CTLCOLORLISTBOX;
    case WindowKind::PushButton:
        return message == WM_PAINT || message == WM_PRINTCLIENT ||
               message == WM_ENABLE || message == WM_SETFOCUS ||
               message == WM_KILLFOCUS || message == BM_SETSTATE;
    case WindowKind::ChoiceButton:
        return message == WM_ERASEBKGND || message == WM_PAINT ||
               message == WM_PRINTCLIENT || message == WM_ENABLE ||
               message == WM_SETFOCUS || message == WM_KILLFOCUS ||
               message == BM_SETSTATE || message == BM_SETCHECK;
    case WindowKind::FormHost:
        return message == WM_ERASEBKGND;
    }
    return false;
}

// Keep all tracked state and RAII guards out of ModSubclassProc so unhandled
// messages (commands, notifications, clicks, menus, private snap-in messages)
// remain pure sibling tail calls into DefSubclassProc with zero stack frame.
__attribute__((noinline))
LRESULT ModSubclassHandled(HWND hWnd, UINT message, WPARAM wParam,
                           LPARAM lParam, UINT_PTR uIdSubclass,
                           DWORD_PTR refData) {
    const bool previousBrushScope = g_nativeFormsBrushScope;
    g_nativeFormsBrushScope = false;
    const bool scoped =
        g_active && IsHostedWinFormsKind(static_cast<WindowKind>(refData)) &&
        (message == WM_PAINT || message == WM_ERASEBKGND ||
         message == WM_PRINTCLIENT || message == WM_PRINT);
    if (scoped) {
        ++g_formsPaintDepth;
    }
    FormsButtonPaint paint = {hWnd, false, false};
    FormsButtonPaint* previousButton = g_formsButtonPaint;
    const bool ownerButton = scoped && g_formsButtonHooksReady &&
        static_cast<WindowKind>(refData) == WindowKind::WinFormsSurface &&
        (message == WM_PAINT || message == WM_PRINTCLIENT) &&
        IsFormsOwnerDrawButtonCandidate(hWnd) && !ButtonHasImage(hWnd);
    const bool painting = message == WM_PAINT || message == WM_PRINTCLIENT ||
                          message == WM_PRINT || message == WM_ERASEBKGND;
    HWND previousWizard = g_wizardPaintWindow;
    if (painting) g_wizardPaintWindow = nullptr;
    HWND previousChoice = g_drawingChoiceButton;
    HWND previousPush = g_drawingPushButton;
    const bool previousPushFace = g_pushButtonFaceDrawn;
    const bool previousPushDisabled = g_pushButtonFaceDisabled;
    const bool framePainting = painting || message == WM_NCPAINT;
    if (framePainting) ++g_subclassPaintDepth;
    if (painting) {
        g_formsButtonPaint = ownerButton ? &paint : nullptr;
        // A nested paint of another subclassed window must not inherit the
        // outer push button's completed face or text colour.
        g_drawingChoiceButton = nullptr;
        g_drawingPushButton = nullptr;
        g_pushButtonFaceDrawn = false;
        g_pushButtonFaceDisabled = false;
    }
    const LRESULT result = ModSubclassInner(hWnd, message, wParam, lParam,
                                            uIdSubclass, refData);
    if (g_active && message == WM_WINDOWPOSCHANGED && IsWindow(hWnd) &&
        (static_cast<WindowKind>(refData) == WindowKind::WinFormsSurface ||
         static_cast<WindowKind>(refData) == WindowKind::Tab)) {
        // Do not invalidate frames from a position change nested in Forms
        // painting: application layout can otherwise feed a repaint loop.
        if (g_subclassPaintDepth == 0) {
            RefreshFormsSummaryFramesFor(hWnd, true, false);
        }
    }
    if (framePainting) --g_subclassPaintDepth;
    if (painting) {
        g_wizardPaintWindow = previousWizard;
        g_formsButtonPaint = previousButton;
        g_drawingChoiceButton = previousChoice;
        g_drawingPushButton = previousPush;
        g_pushButtonFaceDrawn = previousPushFace;
        g_pushButtonFaceDisabled = previousPushDisabled;
    }
    if (scoped) {
        --g_formsPaintDepth;
    }
    g_nativeFormsBrushScope = previousBrushScope;
    return result;
}

LRESULT CALLBACK ModSubclassProc(HWND hWnd, UINT message, WPARAM wParam,
                                 LPARAM lParam, UINT_PTR uIdSubclass,
                                 DWORD_PTR refData) {
    if (message != WM_NCDESTROY &&
        (!g_active.load(std::memory_order_relaxed) ||
         !IsSubclassMessageRelevant(static_cast<WindowKind>(refData), message,
                                     lParam))) {
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }
    return ModSubclassHandled(hWnd, message, wParam, lParam, uIdSubclass,
                              refData);
}

void LoadSettings() {
    g_settings.darkTitleBar = Wh_GetIntSetting(L"darkTitleBar") != 0;
    g_settings.roundCorners = Wh_GetIntSetting(L"roundCorners") != 0;
    g_settings.darkStatusBar = Wh_GetIntSetting(L"darkStatusBar") != 0;
}

// A hosted preview can be attached after its children were created. Retry
// frame invalidation on their paint, without invalidating their client areas.
void RefreshFormsSummaryFramesFor(HWND hWnd, bool includeTracked, bool fromPaint) {
    HWND panel = nullptr;
    if (WindowClassStarts(hWnd, L"WindowsForms10.SysListView32.") ||
        WindowClassStarts(hWnd, L"WindowsForms10.SysTabControl32.") ||
        WindowClassStarts(hWnd, L"WindowsForms10.Window.")) {
        panel = GetParent(hWnd);
        if (!FindFormsSummarySibling(panel) &&
            WindowClassStarts(hWnd, L"WindowsForms10.Window.")) panel = hWnd;
    }
    HWND sibling = FindFormsSummarySibling(panel);
    if (!sibling) return;
    HWND outer = GetParent(GetParent(panel));
    // A frame repaint must not indefinitely invalidate itself or its sibling
    // if the overlay cannot be drawn. Position changes rearm this guard.
    if (fromPaint && (hWnd == panel || hWnd == sibling || hWnd == outer)) {
        if (GetPropW(hWnd, kSummaryPaintRetryProperty) ||
            !SetPropW(hWnd, kSummaryPaintRetryProperty, reinterpret_cast<HANDLE>(1))) return;
    }
    for (HWND frame : {panel, sibling, outer}) {
        if (includeTracked || !GetPropW(frame, kFormsFrameProperty)) {
            RedrawWindow(frame, nullptr, nullptr,
                         RDW_FRAME | RDW_INVALIDATE | RDW_NOCHILDREN);
        }
    }
}

void RefreshFormsSummaryFramesFor(HWND hWnd) {
    RefreshFormsSummaryFramesFor(hWnd, false, false);
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
            const COLOR16 shade = static_cast<COLOR16>(
                (isFirst ? kActionsShadeTop : isLast ? kActionsShadeBottom
                                                      : kActionsShadeMiddle) << 8);
            copy[i].Red = copy[i].Green = copy[i].Blue = shade;
        }
        if (match && sawFirst && sawLast &&
            pActionsGradient(dc, copy, count, mesh, meshes, mode)) {
            g_actionsElementFrame->gradientReplaced = true;
            return TRUE;
        }
    }
    if (pActionsGradient && g_active && IsActionsPaneDc(dc) &&
        !g_actionsElementFrame && vertices && count > 0 && count <= 8 &&
        mesh && meshes > 0 &&
        (mode == GRADIENT_FILL_RECT_H || mode == GRADIENT_FILL_RECT_V)) {
        bool allLight = true;
        TRIVERTEX copy[8] = {};
        const COLOR16 bgR = static_cast<COLOR16>(GetRValue(kColorBackground) << 8);
        const COLOR16 bgG = static_cast<COLOR16>(GetGValue(kColorBackground) << 8);
        const COLOR16 bgB = static_cast<COLOR16>(GetBValue(kColorBackground) << 8);
        for (ULONG i = 0; i < count; ++i) {
            if ((vertices[i].Red >> 8) < 190 || (vertices[i].Green >> 8) < 190 ||
                (vertices[i].Blue >> 8) < 190) {
                allLight = false;
                break;
            }
            copy[i] = vertices[i];
            copy[i].Red = bgR;
            copy[i].Green = bgG;
            copy[i].Blue = bgB;
        }
        if (allLight) {
            return pActionsGradient(dc, copy, count, mesh, meshes, mode);
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
    if (rect && !(format & DT_CALCRECT) && g_active && dc) {
        if (IsDarkActionsHeaderText(dc) && IsDirectUiCaller(__builtin_return_address(0))) {
            COLORREF old = SetTextColor(dc, kColorText);
            if (old != CLR_INVALID) {
                int result = pActionsDrawText(dc, text, length, rect, format);
                SetTextColor(dc, old); return result;
            }
        }
        if (IsActionsPaneDc(dc)) {
            const COLORREF textColour = GetTextColor(dc);
            const bool darkText = textColour != CLR_INVALID &&
                                  GetRValue(textColour) < 80 &&
                                  GetGValue(textColour) < 80 &&
                                  GetBValue(textColour) < 80;
            const bool lightBk = GetBkMode(dc) == OPAQUE && IsLightSolidColour(GetBkColor(dc));
            if (darkText || lightBk) {
                COLORREF oldText = darkText ? SetTextColor(dc, kColorText) : CLR_INVALID;
                COLORREF oldBk = lightBk ? SetBkColor(dc, kColorBackground) : CLR_INVALID;
                int result = pActionsDrawText(dc, text, length, rect, format);
                if (oldText != CLR_INVALID) SetTextColor(dc, oldText);
                if (oldBk != CLR_INVALID) SetBkColor(dc, oldBk);
                return result;
            }
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
        COLORREF old = SetBkColor(dc, kColorDiskSelected);
        if (old != CLR_INVALID) {
            BOOL result = pActionsExtText(dc, x, y, options, rect, text, count, dx);
            SetBkColor(dc, old);
            return result;
        }
    }
    if (g_active && g_lineBrush && dc && (options & ETO_OPAQUE) && pGetSysColor &&
        (GetBkColor(dc) & 0x00ffffffu) ==
            (pGetSysColor(kColGradientActiveCaption) & 0x00ffffffu)) {
        COLORREF old = SetBkColor(dc, kColorLine);
        if (old != CLR_INVALID) {
            BOOL result = pActionsExtText(dc, x, y, options, rect, text, count, dx);
            SetBkColor(dc, old);
            return result;
        }
    }
    if (g_active && IsActionsPaneDc(dc)) {
        const bool lightBk = ((options & ETO_OPAQUE) || GetBkMode(dc) == OPAQUE) &&
                             IsLightSolidColour(GetBkColor(dc));
        const COLORREF textColour = GetTextColor(dc);
        const bool darkText = text && count && textColour != CLR_INVALID &&
                              GetRValue(textColour) < 80 &&
                              GetGValue(textColour) < 80 &&
                              GetBValue(textColour) < 80;
        if (lightBk || darkText) {
            COLORREF oldBk = lightBk ? SetBkColor(dc, kColorBackground) : CLR_INVALID;
            COLORREF oldText = darkText ? SetTextColor(dc, kColorText) : CLR_INVALID;
            BOOL result = pActionsExtText(dc, x, y, options, rect, text, count, dx);
            if (oldText != CLR_INVALID) SetTextColor(dc, oldText);
            if (oldBk != CLR_INVALID) SetBkColor(dc, oldBk);
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

#if defined(_WIN64)
void InstallMessageBoxFooterHook() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    g_footerLookup = ntdll ? reinterpret_cast<FooterLookupFn>(
        GetProcAddress(ntdll, "RtlLookupFunctionEntry")) : nullptr;
    using DrawCommandRectangleFn = void (WINAPI*)(HWND);
    DrawCommandRectangleFn painter = nullptr;
    WindhawkUtils::SYMBOL_HOOK user32DllHooks[] = {
        {{L"void __cdecl DrawCommandRectangle(struct HWND__ *)"}, &painter, nullptr, true},
    };
    if (!user32 || !g_footerLookup ||
        !WindhawkUtils::HookSymbols(user32, user32DllHooks, ARRAYSIZE(user32DllHooks)) || !painter) {
        Wh_Log(L"message-box footer symbol unavailable; native footer retained");
        return;
    }
    DWORD64 base = 0;
    auto entry = g_footerLookup(reinterpret_cast<DWORD64>(painter), &base, nullptr);
    if (!entry || base != reinterpret_cast<DWORD64>(user32) ||
        base + entry->BeginAddress != reinterpret_cast<DWORD64>(painter)) {
        Wh_Log(L"message-box footer metadata unavailable; native footer retained");
        return;
    }
    g_footerPainterStart = reinterpret_cast<void*>(painter);
    if (!WindhawkUtils::SetFunctionHook(Rectangle, FooterRectangleHook,
                                         &g_footerRectangleOriginal)) {
        g_footerPainterStart = nullptr;
        Wh_Log(L"message-box footer hook unavailable; native footer retained");
    }
}
#endif

void InstallHooks() {
    HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
    auto drawThemeBackground = uxtheme ? reinterpret_cast<DrawThemeBackground_t>(
        GetProcAddress(uxtheme, "DrawThemeBackground")) : nullptr;
    const bool backgroundHooked = drawThemeBackground && WindhawkUtils::SetFunctionHook(
        drawThemeBackground, DrawThemeBackground_Hook, &pDrawThemeBackground);
    if (!backgroundHooked) {
        Wh_Log(L"could not hook DrawThemeBackground; native button drawing retained");
    }
    auto drawThemeTextEx = uxtheme ? reinterpret_cast<DrawThemeTextEx_t>(
        GetProcAddress(uxtheme, "DrawThemeTextEx")) : nullptr;
    auto drawThemeText = uxtheme ? reinterpret_cast<DrawThemeText_t>(
        GetProcAddress(uxtheme, "DrawThemeText")) : nullptr;
    const bool textExHooked = drawThemeTextEx && WindhawkUtils::SetFunctionHook(
        drawThemeTextEx, DrawThemeTextEx_Hook, &pDrawThemeTextEx);
    if (!textExHooked) {
        Wh_Log(L"could not hook DrawThemeTextEx; native push button drawing retained");
    }
    const bool textHooked = drawThemeText && WindhawkUtils::SetFunctionHook(
        drawThemeText, DrawThemeText_Hook, &pDrawThemeText);
    if (!textHooked) {
        Wh_Log(L"could not hook DrawThemeText; native push button drawing retained");
    }
    const bool drawTextExHooked = WindhawkUtils::SetFunctionHook(
        DrawTextExW, DrawTextExW_Hook, &pDrawTextExW);
    if (!drawTextExHooked) {
        Wh_Log(L"could not hook DrawTextExW; WinForms text stays native");
    }
    g_pushButtonThemeHooksReady = backgroundHooked && textExHooked && textHooked &&
                                  drawTextExHooked;
    g_formsButtonHooksReady = g_pushButtonThemeHooksReady;
    if (!WindhawkUtils::SetFunctionHook(DrawFrameControl, DrawFrameControl_Hook,
                                        &pDrawFrameControl)) {
        Wh_Log(L"could not hook DrawFrameControl; Disk Management keeps its original splitter");
    }
    if (!WindhawkUtils::SetFunctionHook(BitBlt, WizardBitBltHook, &pBitBlt)) {
        Wh_Log(L"could not hook BitBlt; wizard header images remain native");
    }
    if (!WindhawkUtils::SetFunctionHook(CreateHatchBrush, CreateHatchBrush_Hook,
                                        &pCreateHatchBrush)) {
        Wh_Log(L"could not hook CreateHatchBrush; Disk Management keeps its original partition colours");
    }
    const bool edgeHooked = WindhawkUtils::SetFunctionHook(DrawEdge, DrawEdge_Hook, &pDrawEdge);
    if (!edgeHooked) {
        Wh_Log(L"could not hook DrawEdge; Actions captions and Disk header edges keep their native appearance");
    }
    const bool fillHooked = WindhawkUtils::SetFunctionHook(FillRect, FillRect_Hook, &pFillRect);
    g_classicPushHooksReady = edgeHooked && fillHooked;
    if (!g_classicPushHooksReady) {
        Wh_Log(L"classic disabled push button adjustment unavailable");
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
    if (!WindhawkUtils::SetFunctionHook(PatBlt, PatBlt_Hook, &pPatBlt)) {
        Wh_Log(L"could not hook PatBlt");
    }
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateWindowExA_Hook,
                                        &pCreateWindowExA)) {
        Wh_Log(L"could not hook CreateWindowExA; ANSI-created windows are themed late");
    }
    if (!WindhawkUtils::SetFunctionHook(SetWindowLongPtrW, SetWindowLongPtrW_Hook,
                                        &pSetWindowLongPtrW)) {
        Wh_Log(L"could not hook SetWindowLongPtrW; TPM chooser may remain light");
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
        if (mapped != kNotMapped && !g_sysBrushes[i]) {
            g_sysBrushes[i] = CreateSolidBrush(mapped);
        }
    }

    // Create shared paint brushes before concurrent GUI paints.
    if (!g_hoverBrush) {
        g_hoverBrush = CreateSolidBrush(kColorHover);
    }
    LineBrush();
    TabSelectedBrush();
    PressedBrush();
    AccentBrush();

    pSetPreferredAppMode(PreferredAppModeForceDark);
    if (pFlushMenuThemes) {
        pFlushMenuThemes();
    }

    g_active = true;

    // Wait for Wh_ModAfterInit: Windhawk applies registered hooks after Wh_ModInit returns.
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"MMC Dark Mode " WH_MOD_VERSION L" (pid=%u)", GetCurrentProcessId());
    OwnThreadSetupMessage();

    LoadSettings();

    if (!ResolveApis()) {
        return FALSE;
    }

    InstallHooks();

    if (HighContrastIsOn()) {
        Wh_Log(L"high contrast is on, doing nothing");
        return TRUE;
    }

#if defined(_WIN64)
    InstallActionsGdiHooks();
    InstallMessageBoxFooterHook();
#endif
    Activate();

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
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    RemoveDialogInitHooks();
#if defined(_WIN64)
    // Prevent new late-hook registration before unload begins.
    std::lock_guard<std::mutex> lock(g_actionsInstallMutex);
    g_actionsStopping = true;
#endif
}

void Wh_ModUninit() {
    g_unloading = true;
    const bool wasActive = g_active;
    g_active = false;

    // Restore app mode before retheming windows so controls resolve native colours.
    if (pSetPreferredAppMode) {
        pSetPreferredAppMode(PreferredAppModeDefault);
    }
    if (pFlushMenuThemes) {
        pFlushMenuThemes();
    }
    RemoveDialogInitHooks();
    while (g_activeThreadHooks.load(std::memory_order_acquire) > 0) {
        Sleep(10);
    }

    // Restore class brushes first so on-thread resets and repaints erase cleanly to light.
    RestoreHostClassBrushes();

    if (wasActive) {
        NotifyDotNetSysColorChange();
        WalkEveryThread(true);
    }
    RemoveAllSubclasses();
    while (g_activeThreadHooks.load(std::memory_order_acquire) > 0) {
        Sleep(10);
    }
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