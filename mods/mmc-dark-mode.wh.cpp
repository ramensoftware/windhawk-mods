// ==WindhawkMod==
// @id              mmc-dark-mode
// @name            MMC Dark Mode
// @description     Dark theme for the console snap-ins: Device Manager, Event Viewer, Services, Task Scheduler and Disk Management.
// @version         1.0
// @author          metanoid18
// @github          https://github.com/Metanoid18
// @license         MIT
// @include         mmc.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// Source code is published under the MIT license.

// ==WindhawkModReadme==
/*
# MMC Dark Mode

A dark theme for the Microsoft Management Console: Device Manager, Services,
Event Viewer, Task Scheduler, Disk Management and anything else hosted by
`mmc.exe`.

## What it does

* Dark title bar and rounded window corners on Windows 11.
* Dark tree and list views, including the column headers.
* Dark status bar, and the panel borders and frames around the console's panes.
* Dark property sheets, including the device properties page, its tabs, its
  group boxes and the field edges inside it.
* The icon strip is drawn without a visual style, so its separators are thin
  lines instead of light boxes, with the hover drawn by this mod.
* Dark dialogs in general, with light text.

## How the colours are done

Two different things were getting painted white, and they need two different
answers.

Code that asks Windows for a colour gets a dark answer inside `mmc.exe`. The
machine's colour table is never changed, nothing is written to the registry, and
nothing outside `mmc.exe` is affected. Turning the mod off removes the answers
and the console draws light again.

Frames that Windows draws for itself do not ask anyone: the group box around a
set of fields, and the border around a property page, are drawn from a table
inside Windows that only a machine wide change can write. Those are painted here
instead, in the same colours, so they match. That is also why an approach that
changes the machine's colours makes them go away and this one had to paint them.

The hooks exist for specific reasons:

* `GetSysColor` and `GetSysColorBrush` answer the colour lookups described above.
* `SetWindowTheme` is redirected for a few controls that come out wrong when
  themed: toolbars, whose separator boxes go light; labels, which draw their text
  through the theme engine rather than from a colour; and tab strips, which have no
  dark variant in any build. MMC re-themes its chrome after it is created, at a
  moment that varies between runs, so refusing the call is more reliable than
  removing the theme afterwards.
* `GetThemeColor` answers one question from the theme engine: the fill of a
  selected row in a list. Newer comctl32 takes that fill from the visual style and
  the dark style answers white, so the row draws as a blank white bar with light
  text on it.
* `CreateWindowExW` and the two `CreateDialog*` entry points are where windows are
  picked up as they are created, so a control that appears after the mod is loaded
  is themed before it is first painted.

## Known limits

* Pages that are web content inside the console, such as the Event Viewer
  overview, are left alone. Themed, they draw their own text black on a dark
  background, which is unreadable.
* The tab strip inside property sheets is drawn by this mod. Windows has no dark
  tab control in any build, so the tabs get a flat dark selected tab and a quiet
  grey label instead of the light boxes the control draws itself.
* Push buttons keep the visuals of Windows' own dark button, so their outline is
  lighter than a border drawn by this mod.
* The icon strip keeps its classic drawing, so its hover is drawn by this mod.
* One caption in the device properties dialog, "Setting based on:", is drawn by
  MMC itself with Windows' own renderer, in pure white and without ClearType, so it
  looks slightly brighter and softer than the labels around it. It was traced
  through four separate causes and ruled out of every one: the page theme, the
  control's own theme, dark mode on the control, and the background mode. Reaching
  it would mean taking over the drawing of a control MMC owns, which is not worth
  the risk to the rest of the dialog.
* High contrast mode is respected: the mod does nothing while it is on.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enabled
  $description: Turn the dark theme on or off without removing the mod
- darkTitleBar: true
  $name: Dark title bar
  $description: Darkens the title bar of the console window. Windows 11 only
- roundCorners: true
  $name: Rounded window corners
  $description: Rounded corners on the console window. Windows 11 only
- darkStatusBar: true
  $name: Dark status bar
  $description: Repaints the status bar at the bottom of the console in the dark colours
- hideStatusBar: false
  $name: Hide the status bar
  $description: Removes the strip at the bottom. It shows nothing in Device Manager except a resize grip
- hideToolbar: false
  $name: Hide the toolbar
  $description: Removes the strip of icons at the top of the console. The menu bar stays, and every command is in the View and Action menus
- logDetails: false
  $name: Write what was themed to the log
  $description: One line per new window class. Useful while this is new; turn it off afterwards
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <windhawk_api.h>

#include <cstddef>

// ---------------------------------------------------------------------------
// Undocumented pieces
//
// uxtheme has had an unnamed dark mode since Windows 10 1809. Those functions
// are exported by ordinal only, so they are resolved at runtime and never
// linked. The numbers have been stable since 1903 and every project in this
// space uses the same ones; if a future build moves them, ResolveApis fails and
// the mod does nothing rather than guessing.
// ---------------------------------------------------------------------------

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

// SetWindowCompositionAttribute, user32, attribute 26 darkens the frame.
constexpr DWORD kWcaUseImmersiveDarkModeColors = 26;

// DwmSetWindowAttribute, Windows 11 corner preference.
constexpr DWORD kDwmwaWindowCornerPreference = 33;
constexpr int kDwmcRound = 2;
// DWMWCP_DEFAULT: back to whatever the system and the user chose, which is what
// a window that was never touched looks like.
constexpr int kDwmcDefault = 0;
// DWMWA_COLOR_DEFAULT: the frame colour the window had before the mod set one.
constexpr DWORD kColorBorderDefault = 0xFFFFFFFF;

#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED 0x031A
#endif

// Classic controls take their colours from these messages even when they
// ignore the dark theme for their background. The console tree and the results
// list both honour them.
constexpr UINT kTvmSetBkColor = 0x111D;    // TVM_SETBKCOLOR
constexpr UINT kTvmSetTextColor = 0x111E;  // TVM_SETTEXTCOLOR
constexpr UINT kTvmSetLineColor = 0x1128;  // TVM_SETLINECOLOR
constexpr UINT kLvmSetBkColor = 0x1001;    // LVM_SETBKCOLOR
constexpr UINT kLvmSetTextColor = 0x1024;  // LVM_SETTEXTCOLOR
constexpr UINT kLvmSetTextBkColor = 0x1026;  // LVM_SETTEXTBKCOLOR

constexpr COLORREF kColorBackground = RGB(32, 32, 32);
constexpr COLORREF kColorText = RGB(240, 240, 240);
constexpr COLORREF kColorLine = RGB(70, 70, 70);
constexpr COLORREF kColorDefault = 0xFF000000;  // CLR_DEFAULT

// Status bar messages and drawing flags, from commctrl.h and winuser.h.
constexpr UINT kSbGetParts = 0x0406;       // SB_GETPARTS
constexpr UINT kSbGetRect = 0x040A;        // SB_GETRECT
constexpr UINT kSbGetTextLengthW = 0x040C;  // SB_GETTEXTLENGTHW
constexpr UINT kSbGetTextW = 0x040D;       // SB_GETTEXTW

constexpr UINT kDtLeft = 0x0000;           // DT_LEFT
constexpr UINT kDtVCenter = 0x0004;        // DT_VCENTER
constexpr UINT kDtSingleLine = 0x0020;     // DT_SINGLELINE
constexpr UINT kDtEndEllipsis = 0x8000;    // DT_END_ELLIPSIS
constexpr int kGwlWndProc = -4;            // GWLP_WNDPROC
struct TbButton {
    int iBitmap;
    int idCommand;
    BYTE fsState;
    BYTE fsStyle;
#ifdef _WIN64
    BYTE bReserved[6];
#else
    BYTE bReserved[2];
#endif
    DWORD_PTR dwData;
    LONG_PTR iString;
};

constexpr UINT kTbButtonCount = 0x0419;    // TB_BUTTONCOUNT
constexpr UINT kTbGetHotItem = 0x0447;     // TB_GETHOTITEM
constexpr UINT kTbGetItemRect = 0x041D;    // TB_GETITEMRECT
constexpr COLORREF kColorHover = RGB(58, 60, 66);
constexpr UINT kCdisHot = 0x0040;          // CDIS_HOT
constexpr UINT kTbGetButton = 0x0417;      // TB_GETBUTTON
constexpr UINT kTbGetButtonTextW = 0x044B; // TB_GETBUTTONTEXTW
constexpr int kDwmwaBorderColor = 34;      // DWMWA_BORDER_COLOR
constexpr int kGwlStyle = -16;             // GWL_STYLE
constexpr int kGwlExStyle = -20;           // GWL_EXSTYLE
constexpr LONG_PTR kWsBorder = 0x00800000; // WS_BORDER
constexpr LONG_PTR kWsExClientEdge = 0x00000200;  // WS_EX_CLIENTEDGE
constexpr LONG_PTR kBsGroupBox = 0x00000007;      // BS_GROUPBOX
constexpr int kStockNullBrush = 5;                // NULL_BRUSH
constexpr int kPenSolid = 0;                      // PS_SOLID
constexpr int kOpaque = 2;                        // OPAQUE
constexpr UINT kMsgGetFont = 0x0031;              // WM_GETFONT
constexpr UINT kSwpNoSize = 0x0001;
constexpr UINT kSwpNoMove = 0x0002;
constexpr UINT kSwpNoZOrder = 0x0004;
constexpr UINT kSwpFrameChanged = 0x0020;
constexpr UINT kTcmSetBkColor = 0x1321;     // TCM_SETBKCOLOR
constexpr UINT kTcmGetItemCount = 0x1304;   // TCM_GETITEMCOUNT
constexpr UINT kTcmGetCursel = 0x130B;      // TCM_GETCURSEL
constexpr UINT kTcmGetItemRect = 0x130A;    // TCM_GETITEMRECT
constexpr UINT kTcmGetItemW = 0x133C;       // TCM_GETITEMW
constexpr UINT kTcifText = 0x0001;          // TCIF_TEXT
constexpr LONG_PTR kTcsBottom = 0x0002;      // TCS_BOTTOM
constexpr LONG_PTR kTcsVertical = 0x0080;    // TCS_VERTICAL
constexpr LONG_PTR kTcsButtons = 0x0100;     // TCS_BUTTONS
constexpr COLORREF kColorTabText = RGB(178, 178, 178);
constexpr COLORREF kColorTabSelected = RGB(45, 45, 45);
constexpr COLORREF kColorSelection = RGB(38, 79, 120);
constexpr int kSwHide = 0;                 // SW_HIDE
constexpr int kSwShow = 5;                 // SW_SHOW

using ShouldAppsUseDarkMode_t = BOOL(WINAPI*)();
using AllowDarkModeForWindow_t = BOOL(WINAPI*)(HWND, BOOL);
using SetPreferredAppMode_t = PreferredAppMode(WINAPI*)(PreferredAppMode);
using FlushMenuThemes_t = void(WINAPI*)();
using IsDarkModeAllowedForWindow_t = BOOL(WINAPI*)(HWND);
using SetWindowTheme_t = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, const void*, DWORD);

struct WindowCompositionAttributeData {
    DWORD attribute;
    void* data;
    size_t size;
};
using SetWindowCompositionAttribute_t =
    BOOL(WINAPI*)(HWND, WindowCompositionAttributeData*);

ShouldAppsUseDarkMode_t pShouldAppsUseDarkMode = nullptr;
AllowDarkModeForWindow_t pAllowDarkModeForWindow = nullptr;
SetPreferredAppMode_t pSetPreferredAppMode = nullptr;
FlushMenuThemes_t pFlushMenuThemes = nullptr;
IsDarkModeAllowedForWindow_t pIsDarkModeAllowedForWindow = nullptr;
SetWindowTheme_t pSetWindowTheme = nullptr;

// Returns the theme a window was explicitly given, or null when it has none. It is
// how a control that is drawing itself through the theme engine is found, without
// having to know in advance which one the app decided to style.
using GetWindowTheme_t = void*(WINAPI*)(HWND);
GetWindowTheme_t pGetWindowTheme = nullptr;

// Newer comctl32 asks its visual style for the fill of a selected list item, and in
// dark mode that style answers white. The text on it stays light, so the row is
// drawn as a blank white bar. Measured on the user's drop down: a pure white
// rectangle thirteen pixels tall across the whole list with no text pixels in it.
// This answers that one question, for that one part in its selected states, and
// leaves every other theme colour alone.
constexpr int kTmtFillColor = 3802;
constexpr int kLbcpItem = 1;
constexpr int kLbpsiHot = 2;
constexpr int kLbpsiSelected = 3;
constexpr int kLbpsiHotSelected = 4;

using GetThemeColor_t = HRESULT(WINAPI*)(HTHEME, int, int, int, COLORREF*);
GetThemeColor_t pGetThemeColor = nullptr;
GetThemeColor_t pGetThemeColorHook = nullptr;

// Whether the mod is currently in effect. Declared before the hooks that read it,
// because a name that is only defined later does not compile. While this is false
// every hook below passes the call straight through, which is what makes the
// enabled setting a switch rather than a reload.
bool g_active = false;

HRESULT WINAPI GetThemeColor_Hook(HTHEME theme, int part, int state, int prop,
                                  COLORREF* colour) {
    // The one question this mod answers, and only while it is on. The answer is
    // written before the trampoline is consulted, so a call that arrives in the
    // moment the hook is installed still gets a defined colour instead of S_OK
    // with nothing written.
    if (colour && g_active && prop == kTmtFillColor && part == kLbcpItem &&
        (state == kLbpsiHot || state == kLbpsiSelected ||
         state == kLbpsiHotSelected)) {
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
    // pSetWindowTheme is repointed at the trampoline once the hook is in place,
    // and the trampoline is the original API.
    if (pSetWindowTheme) {
        pSetWindowTheme(hWnd, subApp, subId);
    }
}

// The console themes its toolbars after they are created, and the moment it does
// that moves from one run to the next. So instead of removing the theme once and
// hoping it stays removed, every attempt to theme a toolbar in this process ends
// with no theme at all.
HRESULT WINAPI SetWindowTheme_Hook(HWND hWnd, LPCWSTR subApp, LPCWSTR subId) {
    if (pSetWindowThemeHook && hWnd) {
        WCHAR className[64] = {};
        if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
            return pSetWindowThemeHook(hWnd, subApp, subId);
        }

        // While the mod is off every call goes through untouched.
        if (!g_active) {
            return pSetWindowThemeHook(hWnd, subApp, subId);
        }

        // A visual style on one of these is always wrong here, and refusing it at
        // the call is the only reliable way to catch a control that appears after
        // the last pass over the window tree:
        //
        //   toolbar        a themed toolbar draws the light separator boxes
        //   Static, Link   a themed label draws its text through the theme engine,
        //                  with a pure white colour and no ClearType, so one label
        //                  stands out as blurred against its neighbours. Measured:
        //                  0 percent colour fringing and a 255 core, against 89 to
        //                  93 percent and a 240 core for the labels beside it.
        //   tab control    no dark tab class exists, so any style makes it light
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
DwmSetWindowAttribute_t pDwmSetWindowAttribute = nullptr;
SetWindowCompositionAttribute_t pSetWindowCompositionAttribute = nullptr;

struct Settings {
    bool enabled;
    bool darkTitleBar;
    bool roundCorners;
    bool darkStatusBar;
    bool hideStatusBar;
    bool hideToolbar;
    bool logDetails;
};

Settings g_settings;

// Windows are not kept in a list. On unload the process is walked again and
// every window is put back to its stock theme, which needs no shared state and
// no locking in a process that creates windows on more than one thread.

// One log line per class, so the log stays readable. Best effort: written from
// whichever thread created the window.
struct SeenClass {
    WCHAR name[64];
};
constexpr size_t kMaxSeenClasses = 64;
SeenClass g_seenClasses[kMaxSeenClasses];
size_t g_seenClassCount = 0;

LRESULT SendToControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// The status bar paints its own text in a colour we cannot reach, so it is
// subclassed and drawn by hand. Only a handful of windows are ever involved; the
// original procedure is kept so unload can put it back.
using WndProc_t = LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM);

struct SubclassEntry {
    HWND hWnd;
    LONG_PTR oldProc;
    // Which of this mod's procedures the window was given, so a window that is
    // destroyed and whose handle is reused is not mistaken for the one before.
    WndProc_t proc;
    // Set only for a strip this mod hid, so the setting can put it back.
    bool hiddenByUs;
};

constexpr size_t kMaxSubclassed = 128;
SubclassEntry g_subclassed[kMaxSubclassed];
size_t g_subclassedCount = 0;
HBRUSH g_backgroundBrush = nullptr;
HBRUSH g_hoverBrush = nullptr;
HBRUSH g_lineBrush = nullptr;
HBRUSH g_tabSelectedBrush = nullptr;

bool g_hidingStrip = false;

// The status bar is painted by hand, and asking it about its own text while it
// is painting is the same re-entrancy that made the toolbar flicker. The content
// is cached whenever the bar changes and only read back at paint time.
struct StatusCache {
    HWND hWnd;
    int parts;
    RECT rects[16];
    WCHAR text[16][256];
    bool valid;
};
StatusCache g_statusCache[2];

void RefreshStatusCache(HWND hWnd);
void ForgetSubclass(HWND hWnd);
bool HiddenByUs(HWND hWnd);
void MarkHiddenByUs(HWND hWnd, bool hidden);


// The hovered button is worked out when the mouse moves, never while a button is
// being painted. Asking a toolbar about itself mid paint re-enters the control
// inside its own drawing, and that is what made the window flicker.
LONG_PTR OldProcOf(HWND hWnd) {
    for (size_t i = 0; i < g_subclassedCount; i++) {
        if (g_subclassed[i].hWnd == hWnd) {
            return g_subclassed[i].oldProc;
        }
    }
    return 0;
}

struct HotEntry {
    HWND toolbar;
    RECT rect;
    bool valid;
};
HotEntry g_hot[8];

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
    HotEntry* entry = HotFor(toolbar, true);
    if (!entry) {
        return false;
    }

    bool wasValid = entry->valid;
    RECT wasRect = entry->rect;

    int hot = static_cast<int>(SendToControl(toolbar, kTbGetHotItem, 0, 0));
    if (hot < 0) {
        entry->valid = false;
    } else {
        RECT rect = {};
        if (SendToControl(toolbar, kTbGetItemRect, hot,
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

LRESULT CALLBACK ToolbarProc(HWND hWnd, UINT message, WPARAM wParam,
                             LPARAM lParam) {
    LONG_PTR oldProc = OldProcOf(hWnd);
    if (!oldProc) {
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    // Inert the moment the mod is switched off, so a window that keeps this proc
    // does not keep the colours.
    if (!g_active && message != 0x0082) {  // WM_NCDESTROY
        return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
    }

    if (message == 0x0082) {  // WM_NCDESTROY
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        HotEntry* entry = HotFor(hWnd, false);
        if (entry) {
            *entry = {};
        }
        ForgetSubclass(hWnd);
        return result;
    }

    LRESULT result =
        reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);

    // The toolbar only repaints the buttons it knows about, so when the button
    // under the mouse changes, the button we painted last has to be redrawn or
    // its highlight stays on screen.
    if (message == 0x0200 || message == 0x02A0 || message == 0x02A3) {
        if (UpdateHot(hWnd)) {
            InvalidateRect(hWnd, nullptr, FALSE);
        }
    }
    return result;
}


// A strip that was hidden is hidden again whenever the console shows it back.
bool ShouldStayHidden(HWND hWnd);
BOOL CALLBACK ReapplyChild(HWND hWnd, LPARAM lParam);
void RefreshFrame(HWND hWnd);
LRESULT CALLBACK TabProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool RefreshTabEntry(HWND hWnd);
HBRUSH LineBrush();
HBRUSH TabSelectedBrush();
void DrawTextOn(HDC dc, PCWSTR text, RECT* rect, UINT flags, COLORREF colour,
                COLORREF behind);
LRESULT CALLBACK GroupBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK FieldProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void PaintGroupBox(HWND hWnd);
void PaintClientEdge(HWND hWnd);
void CoverPageFrame(HWND sheet);
void RememberGroupFont(HWND hWnd);
HFONT GroupFontOf(HWND hWnd);
void ForgetGroupFont(HWND hWnd);
bool NameContains(PCWSTR name, PCWSTR needle);

LRESULT CALLBACK StatusBarProc(HWND hWnd, UINT message, WPARAM wParam,
                               LPARAM lParam) {
    LONG_PTR oldProc = OldProcOf(hWnd);
    if (!oldProc) {
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    // Inert the moment the mod is switched off, so a window that keeps this proc
    // does not keep the colours.
    if (!g_active && message != 0x0082) {  // WM_NCDESTROY
        return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
    }

    if (message == 0x0082) {  // WM_NCDESTROY
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        for (int i = 0; i < 2; i++) {
            if (g_statusCache[i].hWnd == hWnd) {
                g_statusCache[i] = {};
            }
        }
        ForgetSubclass(hWnd);
        return result;
    }

    if (message == 0x0085) {  // WM_NCPAINT
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

            // Read from the cache only. Nothing is sent to the bar here.
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
                               kDtLeft | kDtVCenter | kDtSingleLine |
                                   kDtEndEllipsis,
                               kColorText, kColorBackground);
                }
            }
        }
        EndPaint(hWnd, &ps);
        return 0;
    }

    LRESULT result =
        reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);

    // Anything that can change the text, the part layout or the width: rebuild
    // the cache here, outside paint, then let the bar repaint itself.
    if (message == 0x0402 /* SB_SETTEXT */ || message == 0x0404 /* SB_SETPARTS */ ||
        message == 0x0005 /* WM_SIZE */ || message == 0x0401 /* SB_SETTEXTW/legacy */) {
        RefreshStatusCache(hWnd);
    }

    if (g_settings.hideStatusBar && !g_hidingStrip && ShouldStayHidden(hWnd)) {
        g_hidingStrip = true;
        ShowWindow(hWnd, kSwHide);
        g_hidingStrip = false;
        MarkHiddenByUs(hWnd, true);
    }
    return result;
}

bool FirstTimeSeeingClass(HWND hWnd) {
    WCHAR name[64] = {};
    if (!GetClassNameW(hWnd, name, ARRAYSIZE(name))) {
        return false;
    }
    for (size_t i = 0; i < g_seenClassCount; i++) {
        if (wcscmp(g_seenClasses[i].name, name) == 0) {
            return false;
        }
    }
    if (g_seenClassCount < kMaxSeenClasses) {
        WCHAR* dst = g_seenClasses[g_seenClassCount].name;
        size_t i = 0;
        for (; i + 1 < ARRAYSIZE(g_seenClasses[g_seenClassCount].name) && name[i];
             i++) {
            dst[i] = name[i];
        }
        dst[i] = L'\0';
        g_seenClassCount++;
    }
    return true;
}

void LogWindowOnce(HWND hWnd, PCWSTR what) {
    if (!g_settings.logDetails || !FirstTimeSeeingClass(hWnd)) {
        return;
    }
    WCHAR className[64] = {};
    WCHAR parentClass[64] = {};
    GetClassNameW(hWnd, className, ARRAYSIZE(className));
    HWND parent = GetAncestor(hWnd, GA_PARENT);
    if (parent && parent != hWnd) {
        GetClassNameW(parent, parentClass, ARRAYSIZE(parentClass));
    }
    Wh_Log(L"%s: %s, parent %s", what, className,
           parentClass[0] ? parentClass : L"none");
}

// ---------------------------------------------------------------------------
// Resolution
// ---------------------------------------------------------------------------

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

    HMODULE dwmapi = GetModuleHandleW(L"dwmapi.dll");
    if (!dwmapi) {
        dwmapi = LoadLibraryExW(L"dwmapi.dll", nullptr,
                                LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (dwmapi) {
        pDwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttribute_t>(
            GetProcAddress(dwmapi, "DwmSetWindowAttribute"));
    }

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        pSetWindowCompositionAttribute =
            reinterpret_cast<SetWindowCompositionAttribute_t>(
                GetProcAddress(user32, "SetWindowCompositionAttribute"));
    }

    Wh_Log(L"resolved: shouldApps=%d isAllowed=%d flush=%d dwm=%d swca=%d",
           pShouldAppsUseDarkMode != nullptr,
           pIsDarkModeAllowedForWindow != nullptr,
           pFlushMenuThemes != nullptr, pDwmSetWindowAttribute != nullptr,
           pSetWindowCompositionAttribute != nullptr);
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

// ---------------------------------------------------------------------------
// Theming
// ---------------------------------------------------------------------------

bool SubclassWith(HWND hWnd, WndProc_t proc);
void ThemeDialog(HWND hWnd);
LRESULT CALLBACK BackdropProc(HWND hWnd, UINT message, WPARAM wParam,
                              LPARAM lParam);

void ApplyToTopLevel(HWND hWnd) {
    if (g_settings.darkTitleBar && pSetWindowCompositionAttribute) {
        BOOL dark = TRUE;
        WindowCompositionAttributeData data = {};
        data.attribute = kWcaUseImmersiveDarkModeColors;
        data.data = &dark;
        data.size = sizeof(dark);
        pSetWindowCompositionAttribute(hWnd, &data);
    }

    if (g_settings.roundCorners && pDwmSetWindowAttribute) {
        int preference = kDwmcRound;
        pDwmSetWindowAttribute(hWnd, kDwmwaWindowCornerPreference, &preference,
                               sizeof(preference));
    }

    if (pDwmSetWindowAttribute) {
        // The one pixel frame Windows draws around the window keeps the light
        // system colour unless it is told otherwise.
        COLORREF border = kColorBackground;
        pDwmSetWindowAttribute(hWnd, kDwmwaBorderColor, &border,
                               sizeof(border));
    }

    if (!g_backgroundBrush) {
        g_backgroundBrush = CreateSolidBrush(kColorBackground);
    }
    SubclassWith(hWnd, BackdropProc);
}

LRESULT SendToControl(HWND hWnd, UINT message, WPARAM wParam,
                        LPARAM lParam) {
    DWORD_PTR result = 0;
    SendMessageTimeoutW(hWnd, message, wParam, lParam,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &result);
    return static_cast<LRESULT>(result);
}

void RefreshStatusCache(HWND hWnd) {
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
    // A third bar recycles the slot of one that is gone, so it still gets its text
    // drawn rather than an empty strip.
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

    int parts = static_cast<int>(SendToControl(hWnd, kSbGetParts, 0, 0));
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
        if (SendToControl(hWnd, kSbGetRect, part,
                          reinterpret_cast<LPARAM>(&rect))) {
            cache->rects[part] = rect;
        }
        DWORD_PTR info = SendToControl(hWnd, kSbGetTextLengthW, part, 0);
        int length = static_cast<int>(info & 0xffff);
        if (length <= 0 || length >= 255) {
            continue;
        }
        WCHAR buffer[256] = {};
        if (SendToControl(hWnd, kSbGetTextW, part,
                          reinterpret_cast<LPARAM>(buffer))) {
            for (int c = 0; c < 255 && c < length; c++) {
                cache->text[part][c] = buffer[c];
            }
            cache->text[part][length] = 0;
        }
    }
    cache->valid = true;
}

// The theme alone left these two white: mmc draws their contents itself. The
// control messages are the part the control itself still obeys.
void ColourClassicControl(HWND hWnd, bool dark) {
    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return;
    }

    const COLORREF background = dark ? kColorBackground : kColorDefault;
    const COLORREF text = dark ? kColorText : kColorDefault;

    if (wcscmp(className, L"SysTreeView32") == 0) {
        SendToControl(hWnd, kTvmSetBkColor, 0, background);
        SendToControl(hWnd, kTvmSetTextColor, 0, text);
        SendToControl(hWnd, kTvmSetLineColor, 0, dark ? kColorLine : kColorDefault);
        InvalidateRect(hWnd, nullptr, TRUE);
    } else if (wcscmp(className, L"ComboBox") == 0) {
        // Measured before this fix: an Explorer themed combo box field is
        // 204,228,247, a pale blue that reads as white on a dark page. The combo
        // theme darkens the closed field and the dropped list in one go.
        if (dark) {
            pSetWindowTheme(hWnd, L"DarkMode_CFD", nullptr);
        } else {
            pSetWindowTheme(hWnd, L"", L"");
        }
        InvalidateRect(hWnd, nullptr, TRUE);
    } else if (wcscmp(className, L"ListBox") == 0) {
        // A list box has no dark theme. Classic drawing asks the parent for a
        // brush, and the parent answers dark.
        pSetWindowTheme(hWnd, L"", L"");
        InvalidateRect(hWnd, nullptr, TRUE);
    } else if (wcscmp(className, L"ScrollBar") == 0) {
        // A classic scrollbar is drawn from the system colour table, which a colour
        // answer cannot reach. Themed, it follows the dark style instead.
        if (dark) {
            pAllowDarkModeForWindow(hWnd, TRUE);
            pSetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);
        } else {
            pSetWindowTheme(hWnd, L"", L"");
        }
    } else if (wcscmp(className, L"SysTabControl32") == 0) {
        // Reached here for every window in the process, which matters because a
        // property sheet's tab strip is a child of the sheet, and the sheet is a
        // top level window that never goes through the dialog walk.
        if (dark) {
            pSetWindowTheme(hWnd, L"", L"");
            SendToControl(hWnd, kTcmSetBkColor, 0,
                          static_cast<LPARAM>(kColorBackground));
            SubclassWith(hWnd, TabProc);
            RefreshTabEntry(hWnd);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    } else if (wcscmp(className, L"SysListView32") == 0) {
        SendToControl(hWnd, kLvmSetBkColor, 0, background);
        SendToControl(hWnd, kLvmSetTextColor, 0, text);
        SendToControl(hWnd, kLvmSetTextBkColor, 0, background);
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

// Drops the table entry for a window. Used when the window is going away, and
// when its handle turns out to belong to a different window now.
void ForgetSubclass(HWND hWnd) {
    for (size_t i = 0; i < g_subclassedCount; i++) {
        if (g_subclassed[i].hWnd != hWnd) {
            continue;
        }
        for (size_t j = i + 1; j < g_subclassedCount; j++) {
            g_subclassed[j - 1] = g_subclassed[j];
        }
        g_subclassedCount--;
        return;
    }
}

SubclassEntry* FindSubclass(HWND hWnd) {
    for (size_t i = 0; i < g_subclassedCount; i++) {
        if (g_subclassed[i].hWnd == hWnd) {
            return &g_subclassed[i];
        }
    }
    return nullptr;
}

// The mark is set on a window when this mod first takes it over, and lives
// exactly as long as the window does. It answers "is this the same window", which
// the procedure pointer cannot: a destroyed window's handle is handed to the next
// window created here, and another component may have subclassed on top of us.
constexpr PCWSTR kSubclassMark = L"DarkConsole.Subclass";

bool IsMarkedSubclass(HWND hWnd, WndProc_t proc) {
    return hWnd &&
           GetPropW(hWnd, kSubclassMark) == reinterpret_cast<HANDLE>(proc);
}

// Whether the procedure at the top of the window's chain is still this mod's,
// which is the only case in which the original may be written back.
bool CarriesProc(SubclassEntry* entry) {
    return entry && entry->hWnd &&
           GetWindowLongPtrW(entry->hWnd, kGwlWndProc) ==
               reinterpret_cast<LONG_PTR>(entry->proc);
}

bool HiddenByUs(HWND hWnd) {
    SubclassEntry* entry = FindSubclass(hWnd);
    return entry && entry->hiddenByUs;
}

void MarkHiddenByUs(HWND hWnd, bool hidden) {
    SubclassEntry* entry = FindSubclass(hWnd);
    if (entry) {
        entry->hiddenByUs = hidden;
    }
}

bool SubclassWith(HWND hWnd, WndProc_t proc) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    SubclassEntry* existing = FindSubclass(hWnd);
    if (existing) {
        if (IsMarkedSubclass(hWnd, existing->proc)) {
            // The same window, already carrying one of ours. A window can only
            // wear one of them, so this is the normal repeated call.
            return false;
        }
        // The handle is in the table but the window does not carry the mark: the
        // window was destroyed and its handle reused. Drop the stale entry.
        ForgetSubclass(hWnd);
    }
    if (g_subclassedCount >= kMaxSubclassed) {
        Wh_Log(L"out of subclass slots, %d in use", static_cast<int>(g_subclassedCount));
        return false;
    }
    // Without the mark, the window could never be told apart from a future window
    // that reuses its handle, so it is left alone rather than half tracked.
    if (!SetPropW(hWnd, kSubclassMark, reinterpret_cast<HANDLE>(proc))) {
        return false;
    }
    LONG_PTR oldProc = SetWindowLongPtrW(
        hWnd, kGwlWndProc, reinterpret_cast<LONG_PTR>(proc));
    if (!oldProc) {
        RemovePropW(hWnd, kSubclassMark);
        return false;
    }
    g_subclassed[g_subclassedCount].hWnd = hWnd;
    g_subclassed[g_subclassedCount].oldProc = oldProc;
    g_subclassed[g_subclassedCount].proc = proc;
    g_subclassed[g_subclassedCount].hiddenByUs = false;
    g_subclassedCount++;
    return true;
}

bool ShouldStayHidden(HWND hWnd) {
    return IsWindowVisible(hWnd);
}

// Last line of defence for the console frame itself. If the console keeps the
// space where a hidden strip used to be, that space shows this background
// instead of a white band.
LRESULT CALLBACK BackdropProc(HWND hWnd, UINT message, WPARAM wParam,
                              LPARAM lParam) {
    LONG_PTR oldProc = OldProcOf(hWnd);
    if (!oldProc) {
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    // Inert the moment the mod is switched off, so a window that keeps this proc
    // does not keep the colours.
    if (!g_active && message != 0x0082) {  // WM_NCDESTROY
        return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
    }

    if (message == 0x0082) {  // WM_NCDESTROY
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        ForgetSubclass(hWnd);
        return result;
    }

    // The colour goes down in the erase, not in a paint of our own: a paint
    // that fills the whole client covers whatever the console drew into its own
    // child windows, and the console does not always draw it again.
    if (message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        if (dc && g_backgroundBrush) {
            RECT client = {};
            GetClientRect(hWnd, &client);
            FillRect(dc, &client, g_backgroundBrush);
        }
        return 1;
    }

    LRESULT result =
        reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);

    // A property sheet is a top level window, so it carries this proc rather than
    // the dialog one. Its body frame is drawn just before this and covered here.
    if (message == WM_PAINT) {
        CoverPageFrame(hWnd);
    }

    // Dialogs the console opens are owned windows of this frame, and some of
    // them are built inside user32 where the CreateWindowExW hook cannot see
    // them. When the frame goes inactive, one of its dialogs has just opened.
    if (message == WM_ACTIVATE && LOWORD(wParam) == 0) {
        HWND popup = GetWindow(hWnd, GW_ENABLEDPOPUP);
        if (popup && popup != hWnd) {
            ThemeDialog(popup);
        }
    }
    return result;
}

// The band at the top of the console holds the menu bar and the icon strip in
// one rebar. Nothing here paints it: the theme is asked to do it, and this proc
// only exists so the band can still be removed if the user turns that on.
LRESULT CALLBACK StripProc(HWND hWnd, UINT message, WPARAM wParam,
                           LPARAM lParam) {
    LONG_PTR oldProc = OldProcOf(hWnd);
    if (!oldProc) {
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    // Inert the moment the mod is switched off, so a window that keeps this proc
    // does not keep the colours.
    if (!g_active && message != 0x0082) {  // WM_NCDESTROY
        return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
    }

    if (message == 0x0082) {  // WM_NCDESTROY
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        ForgetSubclass(hWnd);
        return result;
    }

    // The console's toolbars draw a hovered button as a two tone border, which on
    // a dark band shows up as a thin corner. The fill is painted here instead.
    // Everything else about the button is still drawn by the control.
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
                // The per button stage only arrives if this is asked for.
                return CDRF_NOTIFYITEMDRAW;
            }
            if (custom->dwDrawStage == CDDS_ITEMPREPAINT) {
                HotEntry* entry =
                    HotFor(static_cast<HWND>(header->hwndFrom), false);
                bool isHot =
                    (custom->uItemState & kCdisHot) != 0 ||
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
                return CDRF_DODEFAULT;
            }
        }
    }

    // Only the band around the menu and icons is coloured. The toolbar and the
    // menu text are drawn by the console on top of it and are left to draw.
    WCHAR className[64] = {};
    bool isBand = GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                  NameContains(className, L"rebar");
    if (isBand && message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        if (dc && g_backgroundBrush) {
            RECT client = {};
            GetClientRect(hWnd, &client);
            FillRect(dc, &client, g_backgroundBrush);
        }
        return 1;
    }

    LRESULT result =
        reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);

    // Hiding from inside the window's own proc can come straight back as
    // WM_SHOWWINDOW. The guard makes the recursion impossible.
    if (g_settings.hideToolbar && !g_hidingStrip && ShouldStayHidden(hWnd)) {
        g_hidingStrip = true;
        ShowWindow(hWnd, kSwHide);
        g_hidingStrip = false;
        MarkHiddenByUs(hWnd, true);
    }
    return result;
}


void UnsubclassStatusBar(HWND hWnd) {
    SubclassEntry* entry = FindSubclass(hWnd);
    if (!entry) {
        return;
    }
    const bool top = CarriesProc(entry);
    LONG_PTR oldProc = entry->oldProc;
    ForgetSubclass(hWnd);
    if (!top) {
        // Something else is on top of the chain, or the handle belongs to another
        // window now. Writing here would break whichever it is.
        return;
    }
    SetWindowLongPtrW(hWnd, kGwlWndProc, oldProc);
    RemovePropW(hWnd, kSubclassMark);
    InvalidateRect(hWnd, nullptr, TRUE);
}

WCHAR LowerAscii(WCHAR c) {
    return (c >= L'A' && c <= L'Z') ? static_cast<WCHAR>(c + 32) : c;
}

// The console does not use the control class names the rest of Windows uses for
// its chrome, so matching is by substring rather than by exact class.
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
                 kSwpNoSize | kSwpNoMove | kSwpNoZOrder | kSwpFrameChanged);
}

// The console frames its panes with a sunken edge painted in the light system
// colours. That is the white rectangle around the tree. The edge bits are
// cleared and any remaining non client painting of it is refused.
bool IsBorderHost(PCWSTR name) {
    return NameContains(name, L"mdiclient") ||
           NameContains(name, L"mmcchildfrm") ||
           NameContains(name, L"mmcviewwindow") ||
           NameContains(name, L"mmcocxviewwindow") ||
           NameContains(name, L"afxframeorview") ||
           NameContains(name, L"afxolecontrol");
}

LRESULT CALLBACK NoBorderProc(HWND hWnd, UINT message, WPARAM wParam,
                              LPARAM lParam) {
    LONG_PTR oldProc = OldProcOf(hWnd);
    if (!oldProc) {
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    // Inert the moment the mod is switched off, so a window that keeps this proc
    // does not keep the colours.
    if (!g_active && message != 0x0082) {  // WM_NCDESTROY
        return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
    }

    if (message == 0x0082) {  // WM_NCDESTROY
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        ForgetSubclass(hWnd);
        return result;
    }

    if (message == 0x0085) {  // WM_NCPAINT
        return 0;
    }
    return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
}

void StripFrame(HWND hWnd) {
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, kGwlExStyle);
    LONG_PTR style = GetWindowLongPtrW(hWnd, kGwlStyle);
    bool changed = false;

    if (exStyle & kWsExClientEdge) {
        SetWindowLongPtrW(hWnd, kGwlExStyle, exStyle & ~kWsExClientEdge);
        changed = true;
    }
    if (style & kWsBorder) {
        SetWindowLongPtrW(hWnd, kGwlStyle, style & ~kWsBorder);
        changed = true;
    }
    if (changed) {
        RefreshFrame(hWnd);
        SubclassWith(hWnd, NoBorderProc);
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

// An edit field keeps a two pixel sunken edge drawn in the system 3D colours,
// which stays white on a dark page however the rest is coloured. The edge is
// non client area, so the only reliable answer is to remove the style.
bool RemoveClientEdge(HWND hWnd) {
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, kGwlExStyle);
    if (!(exStyle & kWsExClientEdge)) {
        return false;
    }
    SetWindowLongPtrW(hWnd, kGwlExStyle, exStyle & ~kWsExClientEdge);
    RefreshFrame(hWnd);
    InvalidateRect(hWnd, nullptr, TRUE);
    return true;
}

// SysTabControl32 has no dark theme class in any shipping Windows, so unthemed it
// draws every tab as a light box from the colour table. The control cannot be
// recoloured, only painted, so the tab strip is drawn here from the rects the
// control laid out. If the labels cannot be read, the strip falls back to the
// control's own drawing rather than showing blank tabs.
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
TabEntry g_tabEntries[4] = {};

// As the SDK declares TCITEMW. The pointer makes it 40 bytes on 64 bit and 28 on
// 32 bit, which is what the control expects on each.
struct TabItemW {
    UINT mask;
    DWORD dwState;
    DWORD dwStateMask;
    LPWSTR pszText;
    int cchText;
    int iImage;
    LPARAM lParam;
};

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
    TabEntry* entry = TabEntryFor(hWnd, true);
    if (!entry) {
        return false;
    }
    entry->usable = false;
    // Only plain tabs along the top are drawn here. Button tabs, vertical tabs
    // and bottom tabs keep the control's own drawing, which is what the console
    // expects there.
    const LONG_PTR style = GetWindowLongPtrW(hWnd, kGwlStyle);
    if (style & (kTcsBottom | kTcsVertical | kTcsButtons)) {
        entry->count = 0;
        return false;
    }
    const int count = static_cast<int>(SendToControl(hWnd, kTcmGetItemCount, 0, 0));
    if (count <= 0 || count > kMaxTabs) {
        entry->count = 0;
        return false;
    }
    entry->count = count;
    for (int i = 0; i < count; i++) {
        entry->items[i] = {};
        if (!SendToControl(hWnd, kTcmGetItemRect, static_cast<WPARAM>(i),
                           reinterpret_cast<LPARAM>(&entry->items[i]))) {
            entry->count = 0;
            return false;
        }
        TabItemW item = {};
        item.mask = kTcifText;
        item.pszText = entry->labels[i];
        item.cchText = kTabLabelChars - 1;
        entry->labels[i][0] = 0;
        SendToControl(hWnd, kTcmGetItemW, static_cast<WPARAM>(i),
                      reinterpret_cast<LPARAM>(&item));
    }
    entry->selected = static_cast<int>(SendToControl(hWnd, kTcmGetCursel, 0, 0));
    entry->font = reinterpret_cast<HFONT>(SendToControl(hWnd, kMsgGetFont, 0, 0));
    // Nothing to go on means the item layout did not match; the control then draws
    // itself, which is what shipped before.
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

    // One rule along the bottom of the strip, with a gap where the page meets the
    // selected tab, which is how a tab strip reads as connected.
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

LRESULT CALLBACK TabProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LONG_PTR oldProc = OldProcOf(hWnd);
    if (!oldProc) {
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    // Inert the moment the mod is switched off, so a window that keeps this proc
    // does not keep the colours.
    if (!g_active && message != 0x0082) {  // WM_NCDESTROY
        return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
    }


    if (message == 0x0082) {  // WM_NCDESTROY
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        TabEntry* entry = TabEntryFor(hWnd, false);
        if (entry) {
            *entry = {};
        }
        UnsubclassStatusBar(hWnd);
        return result;
    }

    // The cached font is what the strip is painted with. Reading it back when the
    // app hands the control a new one keeps a deleted font out of the paint path.
    if (message == WM_SETFONT) {
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        TabEntry* entry = TabEntryFor(hWnd, false);
        if (entry) {
            entry->font = reinterpret_cast<HFONT>(lParam);
        }
        return result;
    }

    if (message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        if (dc && g_backgroundBrush) {
            RECT client = {};
            GetClientRect(hWnd, &client);
            FillRect(dc, &client, g_backgroundBrush);
        }
        return 1;
    }

    if (message == WM_PAINT) {
        TabEntry* entry = TabEntryFor(hWnd, false);
        // A tab control only has items once the app has inserted them, and the app
        // may do that after the control is created and after it is laid out. So a
        // cache that came up empty is retried on the first paint that follows, and
        // stops retrying the moment it reads successfully.
        if (!entry || !entry->usable) {
            if (!RefreshTabEntry(hWnd) || !entry) {
                return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam,
                                                            lParam);
            }
        }
        PaintTabStrip(hWnd);
        return 0;
    }

    // The control changes the selected tab inside its own click handler, so the
    // cache is read back after the message rather than guessed from the mouse.
    if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP ||
        message == WM_KEYUP) {
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        RefreshTabEntry(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
        return result;
    }

    // A re-theme is what a late control gets when the app decides to restyle it,
    // and it is the moment its items exist. Read the layout back here rather than
    // during a paint.
    if (message == WM_THEMECHANGED) {
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        RefreshTabEntry(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
        return result;
    }

    if (message == WM_SIZE || message == WM_SHOWWINDOW ||
        message == WM_WINDOWPOSCHANGED) {
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        TabEntry* entry = TabEntryFor(hWnd, false);
        if (!entry || !entry->usable) {
            RefreshTabEntry(hWnd);
        }
        return result;
    }

    return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
}

// The strips above and below the console tree. The status bar gets painted by
// hand because its text colour cannot be set any other way. The band that holds
// the icon strips is painted dark as well: its icons are dark glyphs, so a dark
// background erases them, and it is removed rather than recoloured when the
// user asks for that.
// The console has two toolbars in that band: the menu bar, whose buttons carry
// text, and the strip of icons, whose buttons carry no text at all. Only the
// icon strip is ever hidden, so the menus always stay.
bool IsIconStrip(HWND hWnd);

bool IsIconStrip(HWND hWnd) {
    int count = static_cast<int>(SendToControl(hWnd, kTbButtonCount, 0, 0));
    if (count <= 0) {
        return false;
    }
    for (int i = 0; i < count; i++) {
        TbButton button = {};
        if (!SendToControl(hWnd, kTbGetButton, i,
                           reinterpret_cast<LPARAM>(&button))) {
            continue;
        }
        WCHAR text[64] = {};
        SendToControl(hWnd, kTbGetButtonTextW, button.idCommand,
                      reinterpret_cast<LPARAM>(text));
        if (text[0]) {
            return false;
        }
    }
    return true;
}

void DressExtraChrome(HWND hWnd, bool dark) {
    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return;
    }

    if (IsBorderHost(className)) {
        if (dark) {
            StripFrame(hWnd);
        }
        if (!NameContains(className, L"mdiclient")) {
            return;
        }
    }

    if (NameContains(className, L"toolbar")) {
        // The icon strip's squares are painted into the console's own button
        // images, so no colour or theme setting reaches them. Removing the strip
        // is the only reliable answer, and every one of its commands is also in
        // the menus above it.
        bool iconStrip = dark && IsIconStrip(hWnd);
        if (!dark) {
            ShowWindow(hWnd, kSwShow);
            return;
        }
        ShowWindow(hWnd, iconStrip && g_settings.hideToolbar ? kSwHide : kSwShow);
        return;
    }

    bool isStatusBar = NameContains(className, L"status");
    bool isStrip = NameContains(className, L"sizeablerebar") ||
                   NameContains(className, L"rebar");
    if (!isStatusBar && !isStrip) {
        return;
    }

    if (!dark) {
        bool wasOurs = OldProcOf(hWnd) != 0;
        UnsubclassStatusBar(hWnd);
        if (wasOurs) {
            ShowWindow(hWnd, kSwShow);
        }
        return;
    }

    if (!g_backgroundBrush) {
        g_backgroundBrush = CreateSolidBrush(kColorBackground);
    }

    SubclassWith(hWnd, isStatusBar ? StatusBarProc : StripProc);
    if (isStatusBar) {
        RefreshStatusCache(hWnd);
    }
    InvalidateRect(hWnd, nullptr, TRUE);

    bool hide = isStatusBar ? g_settings.hideStatusBar : g_settings.hideToolbar;
    if (hide) {
        ShowWindow(hWnd, kSwHide);
        MarkHiddenByUs(hWnd, true);
    } else {
        // A strip this mod hid is put back the moment the setting is turned off.
        // Without this the band could only come back after a restart.
        if (HiddenByUs(hWnd)) {
            ShowWindow(hWnd, kSwShow);
            MarkHiddenByUs(hWnd, false);
        }
        if (isStatusBar && !g_settings.darkStatusBar) {
            UnsubclassStatusBar(hWnd);
        }
    }
}

// A web page drawn inside the console reads the theme and switches its own text
// to black, which is unreadable on our background. Those hosts are left alone.
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

void ApplyToWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }

    if (IsHtmlHost(hWnd)) {
        return;
    }

    pAllowDarkModeForWindow(hWnd, TRUE);
    pSetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);

    // The icon strip gets no theme at all. Unthemed, comctl32 draws each button
    // in the classic way, from the system colour table, which this mod already
    // answers with dark values. If the light squares are a themed button face
    // they disappear here; if they survive, they are pixels in the icons.
    WCHAR toolbarClass[64] = {};
    if (GetClassNameW(hWnd, toolbarClass, ARRAYSIZE(toolbarClass)) &&
        wcscmp(toolbarClass, L"ToolbarWindow32") == 0) {
        pSetWindowTheme(hWnd, L"", L"");
        SubclassWith(hWnd, ToolbarProc);
    }

    // A label carries no visual style of its own, and handing it one makes some
    // of them draw their text through the theme engine instead of GDI. The two
    // render differently: the theme engine's text has no ClearType fringing and
    // comes out at full white, so a single label stands out as blurred against
    // the rest of the page. Labels and links are left classic, where the colours
    // answered above and ClearType both apply.
    WCHAR labelClass[64] = {};
    if (GetClassNameW(hWnd, labelClass, ARRAYSIZE(labelClass)) &&
        (wcscmp(labelClass, L"Static") == 0 ||
         wcscmp(labelClass, L"SysLink") == 0)) {
        // Dark mode is turned back off for a label. With it on, Windows draws the
        // text itself in pure white without ClearType, which is the doubled look
        // measured on one label while every label beside it came through the
        // colour answer at 240. Classic mode makes the label ask its parent for
        // the colours above, so every label on a page renders the same way.
        pAllowDarkModeForWindow(hWnd, FALSE);
        pSetWindowTheme(hWnd, L"", L"");
    }

    // List and tree headers draw from a different theme name than the views
    // they belong to, and the Explorer name leaves them light.
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
    LogWindowOnce(hWnd, L"themed");

    if (GetAncestor(hWnd, GA_ROOT) == hWnd) {
        ApplyToTopLevel(hWnd);
    }
}

// Text drawn in opaque mode paints its own background, and the background a DC
// starts with is white. Drawing a label without saying which colour sits behind it
// therefore paints a white box the size of the label over a dark strip. Measured:
// one white box around each tab label, and a white box around the caption of a
// group box, both created by this mod rather than by Windows.
void DrawTextOn(HDC dc, PCWSTR text, RECT* rect, UINT flags, COLORREF colour,
                COLORREF behind) {
    if (!dc || !text || !rect) {
        return;
    }
    SetBkMode(dc, kOpaque);
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

// Removing the white edge from a field leaves it with no boundary at all and the
// text appears to float on the page. This draws a quiet one pixel edge of its own,
// after the field has painted its content.
LRESULT CALLBACK FieldProc(HWND hWnd, UINT message, WPARAM wParam,
                           LPARAM lParam) {
    LONG_PTR oldProc = OldProcOf(hWnd);
    if (!oldProc) {
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    // Inert the moment the mod is switched off, so a window that keeps this proc
    // does not keep the colours.
    if (!g_active && message != 0x0082) {  // WM_NCDESTROY
        return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
    }


    if (message == 0x0082) {  // WM_NCDESTROY
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        UnsubclassStatusBar(hWnd);
        return result;
    }

    if (message == WM_PAINT) {
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
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

    return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
}

// A painted caption has to use the control's own font, or it comes out in the
// stock system font and reads smaller than the labels next to it. The font is
// read once, when the control is found, never during paint.
struct GroupFont {
    HWND hWnd;
    HFONT font;
};
GroupFont g_groupFonts[32] = {};

void RememberGroupFont(HWND hWnd) {
    HFONT font = reinterpret_cast<HFONT>(SendToControl(hWnd, kMsgGetFont, 0, 0));
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

// The system does not call GetSysColor to draw its own frames; it reads a colour
// table inside user32 that only SetSysColors can write. So a hooked answer never
// reaches these, and both are painted here instead. Measured before the fix on a
// dark device properties page: a group box frame and a page border, both exactly
// 255,255,255 and 160,160,160, the untouched defaults.
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

    // The frame starts at the caption's midline and leaves a gap for the text.
    const int lineY = client.top + textSize.cy / 2;

    HGDIOBJ pen = CreatePen(kPenSolid, 1, kColorLine);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(kStockNullBrush));

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

LRESULT CALLBACK GroupBoxProc(HWND hWnd, UINT message, WPARAM wParam,
                              LPARAM lParam) {
    LONG_PTR oldProc = OldProcOf(hWnd);
    if (!oldProc) {
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    // Inert the moment the mod is switched off, so a window that keeps this proc
    // does not keep the colours.
    if (!g_active && message != 0x0082) {  // WM_NCDESTROY
        return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
    }


    if (message == 0x0082) {  // WM_NCDESTROY
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        ForgetGroupFont(hWnd);
        UnsubclassStatusBar(hWnd);
        return result;
    }

    if (message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        if (dc && g_backgroundBrush) {
            RECT client = {};
            GetClientRect(hWnd, &client);
            FillRect(dc, &client, g_backgroundBrush);
        }
        return 1;
    }

    if (message == WM_PAINT) {
        PaintGroupBox(hWnd);
        return 0;
    }

    return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
}

// The property sheet draws a raised two pixel frame around its body, the tabs and
// the page, from the system colour table, so it comes back white around a dark
// page. Measured on a device properties page: 255,255,255 and 160,160,160 on a
// page that is otherwise 32,32,32. It is covered here, just outside the page and
// just above the tabs, after the sheet has painted.
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
    // The top edge is drawn by the sheet only where the tabs do not cover it, so
    // it is painted in two pieces and never over a tab.
    const RECT topLeft = {outer.left, outer.top, tabRect.left, outer.top + 2};
    const RECT topRight = {tabRect.right, outer.top, outer.right, outer.top + 2};

    // A page that has not been placed yet gives an empty rect, and strips worked
    // out from it would be meaningless. Nothing is drawn in that case; the cover
    // runs again on the next paint.
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

// A dialog carrying a client edge has that edge drawn from the same internal
// table, so it comes back white around a dark page. The edge is two pixels of
// non client area, which is why it is repainted here after the dialog drew it.
void PaintClientEdge(HWND hWnd) {
    if (!g_backgroundBrush) {
        return;
    }
    if (!(GetWindowLongPtrW(hWnd, kGwlExStyle) & kWsExClientEdge)) {
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

// A classic dialog, including every property sheet the console opens, paints its
// own background from the system colours. The theme call alone does not reach it,
// so the colours are answered directly.
LRESULT CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam) {
    LONG_PTR oldProc = OldProcOf(hWnd);
    if (!oldProc) {
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    // Inert the moment the mod is switched off, so a window that keeps this proc
    // does not keep the colours.
    if (!g_active && message != 0x0082) {  // WM_NCDESTROY
        return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
    }


    // A destroyed window leaving its handle in the table is how a later window
    // that reuses the handle inherits the wrong proc.
    if (message == 0x0082) {  // WM_NCDESTROY
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        UnsubclassStatusBar(hWnd);
        return result;
    }

    // The colour goes down in the erase and the paint still reaches the dialog's
    // own proc. Painting the client here and returning early skips the dialog
    // manager, and a property sheet page then draws nothing at all.
    if (message == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        if (dc && g_backgroundBrush) {
            RECT client = {};
            GetClientRect(hWnd, &client);
            FillRect(dc, &client, g_backgroundBrush);
        }
        return 1;
    }

    // A page switched to after the sheet was themed brings children that were
    // never through a pass. Both this page and the sheet get one when they are
    // shown, which is the moment before those children are first painted.
    if (message == WM_SHOWWINDOW && wParam) {
        EnumChildWindows(hWnd, ReapplyChild, 0);
    }

    // The sheet has just painted, including its body frame; cover it.
    if (message == WM_PAINT) {
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        CoverPageFrame(hWnd);
        return result;
    }

    // The dialog has just drawn its own border; cover it so the page has no
    // white outline left.
    if (message == WM_NCPAINT) {
        LRESULT result =
            reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
        PaintClientEdge(hWnd);
        return result;
    }

    // A read only field asks for WM_CTLCOLORSTATIC, a writable one asks for
    // WM_CTLCOLOREDIT. Both have to be answered or the writable one stays white.
    // A list box, and the list a combo box drops down, ask for their colours here.
    // Without an answer they stay white however the rest of the page is painted.
    if (message == WM_CTLCOLORDLG || message == WM_CTLCOLORSTATIC ||
        message == WM_CTLCOLORBTN || message == WM_CTLCOLOREDIT ||
        message == WM_CTLCOLORLISTBOX || message == WM_CTLCOLORSCROLLBAR ||
        message == WM_CTLCOLORMSGBOX) {
        if (wParam) {
            HDC dc = reinterpret_cast<HDC>(wParam);
            // ClearType needs a known background to blend against. Left in
            // transparent mode, text is either drawn grey, or, if it is ever
            // painted twice, merges its own edge pixels with itself, which reads
            // as doubled and blurred. Measured on a device properties page: one
            // label had no colour fringing at all while every other label did.
            SetBkMode(dc, kOpaque);
            SetTextColor(dc, kColorText);
            SetBkColor(dc, kColorBackground);
        }
        return reinterpret_cast<LRESULT>(g_backgroundBrush);
    }

    return reinterpret_cast<WndProc_t>(oldProc)(hWnd, message, wParam, lParam);
}

BOOL CALLBACK ReapplyChild(HWND hWnd, LPARAM) {
    ApplyToWindow(hWnd);

    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    // Any control that carries a theme of its own is drawing itself through the
    // theme engine. For the ones whose text or border comes from that engine and
    // not from a colour, that is always wrong here, so the theme is removed at the
    // control. This does not depend on the window being seen at the moment the app
    // styled it.
    if (pGetWindowTheme && pGetWindowTheme(hWnd) != nullptr &&
        (wcscmp(className, L"Static") == 0 || wcscmp(className, L"SysLink") == 0 ||
         wcscmp(className, L"SysTabControl32") == 0)) {
        pSetWindowTheme(hWnd, L"", L"");
        if (wcscmp(className, L"SysTabControl32") == 0) {
            RefreshTabEntry(hWnd);
        }
        InvalidateRect(hWnd, nullptr, TRUE);
    }

    // A group box is a button control. Themed, it draws its own frame and
    // caption through the theme engine and ignores every colour set here.
    if (wcscmp(className, L"Button") == 0) {
        LONG_PTR style = GetWindowLongPtrW(hWnd, kGwlStyle);
        if ((style & 0x0f) == kBsGroupBox) {
            // Unthemed so nothing themed draws it, and painted here because the
            // classic frame comes from the table above.
            SetWindowThemeQuiet(hWnd, L"", L"");
            SubclassWith(hWnd, GroupBoxProc);
            RememberGroupFont(hWnd);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
    }

    if (wcscmp(className, L"Edit") == 0) {
        if (RemoveClientEdge(hWnd)) {
            SubclassWith(hWnd, FieldProc);
        }
    }

    if (wcscmp(className, L"#32770") == 0) {
        // Windows 11 ignores the brush a property page returns for its own
        // background, so the page has to be painted during the erase instead.
        // That only happens if this dialog proc is on it, and a page found by
        // walking down from the sheet has not been through ThemeDialog.
        SubclassWith(hWnd, DialogProc);
        SetWindowThemeQuiet(hWnd, L"", L"");
        InvalidateRect(hWnd, nullptr, TRUE);
    }

    if (wcscmp(className, L"SysTabControl32") == 0) {
        // Tab controls have no dark variant in any shipping Windows. Unthemed,
        // they draw their tabs from the colour table, which this mod answers.
        SetWindowThemeQuiet(hWnd, L"", L"");
        SendToControl(hWnd, kTcmSetBkColor, 0,
                      static_cast<LPARAM>(kColorBackground));
        SubclassWith(hWnd, TabProc);
        RefreshTabEntry(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
    }
    return TRUE;
}

void ThemeDialog(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || OldProcOf(hWnd)) {
        return;
    }

    // A property sheet is two dialogs: the page, and the frame that holds the
    // tabs and the buttons. Only the page reaches the dialog hooks, so the frame
    // is found here and themed the same way.
    for (int depth = 0; depth < 3; depth++) {
        HWND parent = GetParent(hWnd);
        if (!parent || OldProcOf(parent)) {
            break;
        }
        WCHAR parentClass[64] = {};
        if (!GetClassNameW(parent, parentClass, ARRAYSIZE(parentClass)) ||
            wcscmp(parentClass, L"#32770") != 0) {
            break;
        }
        SubclassWith(parent, DialogProc);
        ApplyToWindow(parent);
        EnumChildWindows(parent, ReapplyChild, 0);
        InvalidateRect(parent, nullptr, TRUE);
        hWnd = parent;
    }
    if (!g_backgroundBrush) {
        g_backgroundBrush = CreateSolidBrush(kColorBackground);
    }
    // Subclass first: ApplyToWindow would give a top level window the frame
    // backdrop proc, and a window can only carry one of ours.
    SubclassWith(hWnd, DialogProc);
    ApplyToWindow(hWnd);
    // The controls were created and themed before the dialog itself was, so
    // they were resolving their theme against a light parent. Now that the
    // dialog is in, they get a second pass.
    EnumChildWindows(hWnd, ReapplyChild, 0);
    InvalidateRect(hWnd, nullptr, TRUE);
}

void ResetWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }
    pSetWindowTheme(hWnd, L"", L"");
    ColourClassicControl(hWnd, false);
    DressExtraChrome(hWnd, false);
    DWORD_PTR ignored = 0;
    SendMessageTimeoutW(hWnd, WM_THEMECHANGED, 0, 0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &ignored);
    if (GetAncestor(hWnd, GA_ROOT) == hWnd) {
        if (pSetWindowCompositionAttribute) {
            BOOL dark = FALSE;
            WindowCompositionAttributeData data = {};
            data.attribute = kWcaUseImmersiveDarkModeColors;
            data.data = &dark;
            data.size = sizeof(dark);
            pSetWindowCompositionAttribute(hWnd, &data);
        }
        if (pDwmSetWindowAttribute) {
            int preference = kDwmcDefault;
            pDwmSetWindowAttribute(hWnd, kDwmwaWindowCornerPreference,
                                   &preference, sizeof(preference));
            // The one pixel frame was set to the background colour when the window
            // was themed. Left behind, it draws a dark outline around a light
            // window after the mod is switched off.
            DWORD border = kColorBorderDefault;
            pDwmSetWindowAttribute(hWnd, kDwmwaBorderColor, &border,
                                   sizeof(border));
        }
        UnsubclassStatusBar(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

struct WalkResult {
    int topLevel;
    int children;
};

void LogChromeClass(HWND hWnd);

BOOL CALLBACK WalkChild(HWND hWnd, LPARAM lParam) {
    WalkResult* result = reinterpret_cast<WalkResult*>(lParam);
    if (!hWnd || !IsWindow(hWnd)) {
        return TRUE;
    }
    LogChromeClass(hWnd);
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
    if (g_active) {
        ApplyToWindow(hWnd);
    } else {
        ResetWindow(hWnd);
    }
    EnumChildWindows(hWnd, WalkChild, lParam);
    result->topLevel++;
    return TRUE;
}

const wchar_t* const kChromeNames[] = {L"status", L"toolbar", L"rebar",
                                      L"header", L"tab", L"mmc", L"afx"};

void LogChromeClass(HWND hWnd) {
    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return;
    }
    bool interesting = false;
    for (size_t i = 0; i < ARRAYSIZE(kChromeNames); i++) {
        if (NameContains(className, kChromeNames[i])) {
            interesting = true;
        }
    }
    if (!interesting || !FirstTimeSeeingClass(hWnd)) {
        return;
    }
    WCHAR parentClass[64] = {};
    HWND parent = GetAncestor(hWnd, GA_PARENT);
    if (parent && parent != hWnd) {
        GetClassNameW(parent, parentClass, ARRAYSIZE(parentClass));
    }
    RECT rect = {};
    GetWindowRect(hWnd, &rect);
    Wh_Log(L"chrome: %s, parent %s, %ldx%ld", className,
           parentClass[0] ? parentClass : L"none", rect.right - rect.left,
           rect.bottom - rect.top);
}

void WalkProcessWindows() {
    WalkResult result = {};
    EnumWindows(WalkTop, reinterpret_cast<LPARAM>(&result));
    Wh_Log(L"%s %d top level windows, %d child windows",
           g_active ? L"themed" : L"restored", result.topLevel,
           result.children);
}

// ---------------------------------------------------------------------------
// Hook
// ---------------------------------------------------------------------------

// Windows draws a lot of this console with the system colour table: menu text,
// button faces, the lines that frame every pane, the status bar border. Asking
// it to draw dark is therefore a matter of answering those lookups, inside this
// process only. Nothing outside mmc.exe is affected.
constexpr int kColScrollbar = 0;
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

HBRUSH g_sysBrushes[23];

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

using GetSysColor_t = DWORD(WINAPI*)(int);
GetSysColor_t pGetSysColor = nullptr;

DWORD WINAPI GetSysColor_Hook(int nIndex) {
    // Switched off means the real system colours, not a dark answer with the
    // theme removed around it: the two have to go together or the console ends
    // up half dark with nothing painting the other half.
    COLORREF mapped = g_active ? MapSystemColour(nIndex) : 0xffffffff;
    if (mapped != 0xffffffff) {
        return mapped;
    }
    return pGetSysColor(nIndex);
}

using GetSysColorBrush_t = HBRUSH(WINAPI*)(int);
GetSysColorBrush_t pGetSysColorBrush = nullptr;

HBRUSH WINAPI GetSysColorBrush_Hook(int nIndex) {
    if (g_active && nIndex >= 0 && nIndex < 23 && g_sysBrushes[nIndex]) {
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

// ---------------------------------------------------------------------------
// Settings and lifecycle
// ---------------------------------------------------------------------------

void LoadSettings() {
    g_settings.enabled = Wh_GetIntSetting(L"enabled") != 0;
    g_settings.darkTitleBar = Wh_GetIntSetting(L"darkTitleBar") != 0;
    g_settings.roundCorners = Wh_GetIntSetting(L"roundCorners") != 0;
    g_settings.darkStatusBar = Wh_GetIntSetting(L"darkStatusBar") != 0;
    g_settings.hideStatusBar = Wh_GetIntSetting(L"hideStatusBar") != 0;
    g_settings.hideToolbar = Wh_GetIntSetting(L"hideToolbar") != 0;
    g_settings.logDetails = Wh_GetIntSetting(L"logDetails") != 0;
}

// Installed once, at load, even when the mod starts switched off. Every hook
// below passes the call straight through while g_active is false, which is what
// makes the enabled setting work in both directions without reloading the mod.
void InstallHooks() {
    if (pSetWindowTheme) {
        if (!Wh_SetFunctionHook((void*)pSetWindowTheme,
                                (void*)SetWindowTheme_Hook,
                                (void**)&pSetWindowThemeHook)) {
            Wh_Log(L"could not hook SetWindowTheme");
        } else {
            // Everything else keeps calling through the hook from now on.
            pSetWindowTheme = pSetWindowThemeHook;
        }
    }

    if (pGetThemeColor) {
        if (!Wh_SetFunctionHook((void*)pGetThemeColor, (void*)GetThemeColor_Hook,
                                (void**)&pGetThemeColorHook)) {
            Wh_Log(L"could not hook GetThemeColor; selected list rows may stay white");
        }
    } else {
        Wh_Log(L"GetThemeColor not found; selected list rows may stay white");
    }

    if (!Wh_SetFunctionHook((void*)GetSysColor, (void*)GetSysColor_Hook,
                            (void**)&pGetSysColor)) {
        Wh_Log(L"could not hook GetSysColor");
    }

    if (!Wh_SetFunctionHook((void*)GetSysColorBrush,
                            (void*)GetSysColorBrush_Hook,
                            (void**)&pGetSysColorBrush)) {
        Wh_Log(L"could not hook GetSysColorBrush");
    }

    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                            (void**)&pCreateWindowExW)) {
        Wh_Log(L"could not hook CreateWindowExW; existing windows are themed, "
               L"new ones will not be");
    }

    if (!Wh_SetFunctionHook((void*)CreateDialogParamW,
                            (void*)CreateDialogParamW_Hook,
                            (void**)&pCreateDialogParamW)) {
        Wh_Log(L"could not hook CreateDialogParamW");
    }

    if (!Wh_SetFunctionHook((void*)CreateDialogIndirectParamW,
                            (void*)CreateDialogIndirectParamW_Hook,
                            (void**)&pCreateDialogIndirectParamW)) {
        Wh_Log(L"could not hook CreateDialogIndirectParamW");
    }
}

// Every window this mod took over gets its own procedure back before the DLL is
// unloaded. Windows are not obliged to be destroyed first: a property sheet can
// still be open when the mod is turned off and unloaded, and a window procedure
// that points into an unloaded DLL is an access violation waiting for the next
// message. The table is checked window by window, because a handle that has been
// reused must not have a stranger's procedure written into it.
void RestoreSubclassedWindows() {
    for (size_t i = 0; i < g_subclassedCount; i++) {
        if (CarriesProc(&g_subclassed[i])) {
            SetWindowLongPtrW(g_subclassed[i].hWnd, kGwlWndProc,
                              g_subclassed[i].oldProc);
            RemovePropW(g_subclassed[i].hWnd, kSubclassMark);
        }
    }
    g_subclassedCount = 0;
}

// Everything that has to be true for the theme to be in effect. Called at load,
// and again when the enabled setting is turned back on.
void Activate() {
    pSetPreferredAppMode(PreferredAppModeForceDark);
    if (pFlushMenuThemes) {
        pFlushMenuThemes();
    }

    g_active = true;
    if (!g_backgroundBrush) {
        g_backgroundBrush = CreateSolidBrush(kColorBackground);
    }

    // Windows that already exist were created before the hook was in place.
    WalkProcessWindows();

    for (int i = 0; i < 23; i++) {
        COLORREF mapped = MapSystemColour(i);
        if (mapped != 0xffffffff && !g_sysBrushes[i]) {
            g_sysBrushes[i] = CreateSolidBrush(mapped);
        }
    }
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"MMC Dark Mode 1.0");

    LoadSettings();

    if (!ResolveApis()) {
        // A mod that is switched off must never fail to load; one that is on says
        // so in the log instead of half working.
        return g_settings.enabled ? FALSE : TRUE;
    }

    InstallHooks();

    if (pShouldAppsUseDarkMode) {
        Wh_Log(L"system dark mode is %d", pShouldAppsUseDarkMode() ? 1 : 0);
    }

    if (!g_settings.enabled) {
        Wh_Log(L"disabled in settings, hooks installed but inert");
        return TRUE;
    }

    if (HighContrastIsOn()) {
        Wh_Log(L"high contrast is on, doing nothing");
        return TRUE;
    }

    Activate();

    Wh_Log(L"status bar setting=%d, hide toolbar setting=%d",
           g_settings.darkStatusBar, g_settings.hideToolbar);
    Wh_Log(L"ready");
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_active) {
        return;
    }
    g_active = false;
    WalkProcessWindows();
    RestoreSubclassedWindows();
    if (pSetPreferredAppMode) {
        pSetPreferredAppMode(PreferredAppModeDefault);
    }
    if (pFlushMenuThemes) {
        pFlushMenuThemes();
    }
    if (g_backgroundBrush) {
        DeleteObject(g_backgroundBrush);
        g_backgroundBrush = nullptr;
    }
    if (g_hoverBrush) {
        DeleteObject(g_hoverBrush);
        g_hoverBrush = nullptr;
    }
    if (g_lineBrush) {
        DeleteObject(g_lineBrush);
        g_lineBrush = nullptr;
    }
    if (g_tabSelectedBrush) {
        DeleteObject(g_tabSelectedBrush);
        g_tabSelectedBrush = nullptr;
    }
    for (int i = 0; i < 23; i++) {
        if (g_sysBrushes[i]) {
            DeleteObject(g_sysBrushes[i]);
            g_sysBrushes[i] = nullptr;
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"settings changed: enabled=%d darkTitleBar=%d roundCorners=%d",
           g_settings.enabled, g_settings.darkTitleBar, g_settings.roundCorners);

    if (g_settings.enabled && !g_active) {
        if (HighContrastIsOn()) {
            Wh_Log(L"high contrast is on, leaving the console alone");
            return;
        }
        if (!pSetPreferredAppMode) {
            // The mod loaded while uxtheme would not cooperate. Try once more, so
            // the switch is not silently dead.
            if (!ResolveApis()) {
                return;
            }
            InstallHooks();
        }
        Activate();
    } else if (!g_settings.enabled && g_active) {
        g_active = false;
        WalkProcessWindows();
        if (pSetPreferredAppMode) {
            pSetPreferredAppMode(PreferredAppModeDefault);
        }
    } else if (g_active) {
        // Title bar and corner switches changed while running: reapply.
        WalkProcessWindows();
    }
}
