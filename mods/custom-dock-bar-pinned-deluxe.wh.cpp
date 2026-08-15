// ==WindhawkMod==
// @id              custom-dock-bar-pinned
// @name            Custom Dock Bar Deluxe
// @description     A draggable, customizable desktop dock with pinned apps, running applications, themes, labels, effects, clock, and diagonal layouts.
// @version         2.2.0
// @author          Alaricholt677
// @github          Alaricholt677
// @include         explorer.exe
// @compilerOptions -lgdi32 -lole32 -lshell32 -lpsapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Dock Bar Deluxe

Creates a customizable floating dock for Windows.

## Features

- Displays pinned taskbar applications.
- Displays applications with visible running windows.
- Provides Start, Search, Task View, Quick Settings, Desktop, and Clock buttons.
- Supports dragging and bottom-center positioning.
- Includes multiple built-in visual themes.
- Supports custom colors, opacity, spacing, rounded corners, labels, and effects.
- Supports rising and falling diagonal layouts.
- Clicking a running application activates it.
- Clicking the active application can minimize it.

## Important

This mod creates a separate floating dock. It does not replace or hide the
standard Windows taskbar.

Pinned applications are read from the current user's pinned taskbar shortcuts.
Some packaged or special applications may not expose a normal executable path
or icon.

Settings changes are applied while the mod is running. Disable the mod to close
and remove the custom dock.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: win12Glow
  $name: Theme
  $description: Choose a built-in theme, or Custom to use custom colors.
  $options:
    - minimalGlass: Minimal Glass
    - win12Glow: Win12 Glow
    - neonDock: Neon Dock
    - stealthFlat: Stealth Flat
    - midnightAcrylic: Midnight Acrylic
    - sunsetGlow: Sunset Glow
    - matrixGreen: Matrix Green
    - iceBlue: Ice Blue
    - roseQuartz: Rose Quartz
    - goldLuxury: Gold Luxury
    - vaporwave: Vaporwave
    - crimsonNight: Crimson Night
    - graphitePro: Graphite Pro
    - oceanDepth: Ocean Depth
    - royalPurple: Royal Purple
    - emeraldGlass: Emerald Glass
    - custom: Custom Theme

- width: 900
  $name: Dock width
  $description: Width in pixels. Use 0 for theme default. Ignored if Auto-fit width is enabled.

- autoFitWidth: 1
  $name: Auto-fit width
  $description: 1 = dock width grows/shrinks based on icons, 0 = use Dock width.

- height: 64
  $name: Dock height
  $description: Height in pixels. Use 0 for theme default.

- cornerRadius: 26
  $name: Dock corner radius
  $description: Dock rounded corner radius. Use 0 for theme default.

- bottomMargin: 16
  $name: Bottom margin
  $description: Distance from bottom of screen when Lock to bottom center is enabled.

- opacity: 235
  $name: Opacity
  $description: Dock opacity, 0-255. Use 0 for theme default.

- lockBottomCenter: 0
  $name: Lock to bottom center
  $description: 1 = always snap to bottom center. 0 = draggable and stays where you put it.

- diagonalMode: 0
  $name: Diagonal layout
  $description: 0 = off, 1 = rising diagonal, 2 = falling diagonal.

- diagonalAmount: 6
  $name: Diagonal amount
  $description: Vertical pixel offset between each button when diagonal layout is enabled.

- showStart: 1
  $name: Show Start button
  $description: 1 = show, 0 = hide.

- showSearch: 1
  $name: Show Search button
  $description: 1 = show, 0 = hide.

- showTaskView: 1
  $name: Show Task View button
  $description: 1 = show, 0 = hide.

- showQuickSettings: 1
  $name: Show Quick Settings button
  $description: 1 = show, 0 = hide.

- showDesktopButton: 1
  $name: Show Desktop button
  $description: 1 = show, 0 = hide.

- showClock: 1
  $name: Show Clock
  $description: 1 = show clock, 0 = hide.

- showPinnedApps: 1
  $name: Show pinned apps
  $description: 1 = show pinned taskbar shortcuts, 0 = hide.

- showRunningApps: 1
  $name: Show running apps
  $description: 1 = show visible running window apps, 0 = hide.

- minimizeOnClick: 1
  $name: Minimize active app on click
  $description: If a running app is already focused, clicking it minimizes it.

- showLabels: 0
  $name: Show app labels
  $description: 1 = show labels under icons, 0 = icons only.

- padding: 10
  $name: Inner padding
  $description: Space around dock contents. Use 0 for automatic.

- gap: 8
  $name: Icon gap
  $description: Space between buttons. Use 0 for automatic.

- buttonRadius: 14
  $name: Button radius
  $description: Rounded radius for taskbar buttons.

- iconInset: 10
  $name: Icon inset
  $description: Smaller value means larger app icons.

- customDockTop: 2634303
  $name: Custom dock top color
  $description: RGB hex as decimal. Example 2634303 = 0x28395F.

- customDockBottom: 1185843
  $name: Custom dock bottom color
  $description: RGB hex as decimal.

- customBorder: 6262015
  $name: Custom border color
  $description: RGB hex as decimal.

- customGlow: 49151
  $name: Custom glow color
  $description: RGB hex as decimal.

- customButton: 3161712
  $name: Custom button color
  $description: RGB hex as decimal.

- customButtonPressed: 2301279
  $name: Custom pressed button color
  $description: RGB hex as decimal.

- customText: 16119285
  $name: Custom text color
  $description: RGB hex as decimal.

- customRunning: 55295
  $name: Custom running indicator color
  $description: RGB hex as decimal.

- customSpecialEffect: 1
  $name: Custom special effect
  $description: 0 none, 1 glass, 2 neon, 3 scanlines, 4 flat, 5 luxury.
*/
// ==/WindhawkModSettings==


#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <psapi.h>
#include <vector>
#include <string>


int IntMin(int a, int b) {
    return a < b ? a : b;
}

int IntMax(int a, int b) {
    return a > b ? a : b;
}

int ClampInt(int value, int low, int high) {
    if (value < low)
        return low;

    if (value > high)
        return high;

    return value;
}

bool SettingOn(int value) {
    return value != 0;
}

COLORREF HexToColor(int hex) {
    hex = ClampInt(hex, 0, 0xFFFFFF);

    int r = (hex >> 16) & 0xFF;
    int g = (hex >> 8) & 0xFF;
    int b = hex & 0xFF;

    return RGB(r, g, b);
}


const wchar_t* DOCK_CLASS_NAME = L"CustomDockBarDeluxeClass";

const int ID_BTN_START = 1001;
const int ID_BTN_SEARCH = 1002;
const int ID_BTN_TASKVIEW = 1003;
const int ID_BTN_QUICK = 1004;
const int ID_BTN_DESKTOP = 1005;
const int ID_BTN_CLOCK = 1006;
const int ID_ITEM_BASE = 2000;

const int TIMER_REDRAW = 1;
const int TIMER_REFRESH = 2;


struct DockTheme {
    const wchar_t* id;

    int width;
    int height;
    int cornerRadius;
    int bottomMargin;
    int opacity;

    COLORREF dockTop;
    COLORREF dockBottom;
    COLORREF borderColor;
    COLORREF glowColor;
    COLORREF buttonColor;
    COLORREF buttonPressedColor;
    COLORREF textColor;
    COLORREF runningColor;

    int specialEffect;
};


DockTheme g_builtinThemes[] = {
    {
        L"minimalGlass",
        780, 52, 18, 18, 220,
        RGB(38, 42, 52),
        RGB(22, 24, 32),
        RGB(110, 120, 145),
        RGB(130, 180, 255),
        RGB(48, 52, 65),
        RGB(34, 38, 50),
        RGB(235, 238, 245),
        RGB(135, 190, 255),
        1
    },
    {
        L"win12Glow",
        900, 64, 26, 16, 235,
        RGB(42, 52, 95),
        RGB(18, 24, 55),
        RGB(95, 140, 255),
        RGB(0, 190, 255),
        RGB(48, 62, 120),
        RGB(35, 45, 95),
        RGB(245, 247, 255),
        RGB(0, 215, 255),
        2
    },
    {
        L"neonDock",
        980, 70, 30, 20, 240,
        RGB(40, 10, 65),
        RGB(12, 8, 26),
        RGB(255, 40, 210),
        RGB(0, 255, 230),
        RGB(60, 18, 90),
        RGB(35, 10, 60),
        RGB(255, 245, 255),
        RGB(0, 255, 210),
        2
    },
    {
        L"stealthFlat",
        760, 48, 12, 14, 210,
        RGB(28, 28, 30),
        RGB(20, 20, 22),
        RGB(65, 65, 70),
        RGB(80, 80, 85),
        RGB(36, 36, 39),
        RGB(24, 24, 27),
        RGB(230, 230, 235),
        RGB(210, 210, 215),
        4
    },
    {
        L"midnightAcrylic",
        920, 62, 24, 18, 225,
        RGB(18, 24, 42),
        RGB(8, 12, 25),
        RGB(80, 110, 180),
        RGB(80, 150, 255),
        RGB(28, 36, 62),
        RGB(14, 20, 42),
        RGB(235, 242, 255),
        RGB(100, 170, 255),
        1
    },
    {
        L"sunsetGlow",
        930, 66, 28, 18, 235,
        RGB(95, 35, 55),
        RGB(35, 18, 35),
        RGB(255, 130, 90),
        RGB(255, 95, 80),
        RGB(120, 50, 65),
        RGB(75, 25, 45),
        RGB(255, 245, 235),
        RGB(255, 185, 80),
        2
    },
    {
        L"matrixGreen",
        880, 58, 20, 16, 230,
        RGB(5, 35, 20),
        RGB(0, 14, 8),
        RGB(40, 220, 110),
        RGB(0, 255, 95),
        RGB(8, 50, 28),
        RGB(0, 25, 14),
        RGB(210, 255, 225),
        RGB(0, 255, 95),
        3
    },
    {
        L"iceBlue",
        900, 62, 26, 16, 225,
        RGB(185, 220, 255),
        RGB(75, 125, 180),
        RGB(225, 245, 255),
        RGB(160, 230, 255),
        RGB(135, 180, 230),
        RGB(85, 130, 180),
        RGB(15, 35, 65),
        RGB(20, 130, 255),
        1
    },
    {
        L"roseQuartz",
        900, 62, 26, 16, 225,
        RGB(115, 55, 85),
        RGB(55, 28, 48),
        RGB(255, 150, 190),
        RGB(255, 125, 205),
        RGB(135, 68, 100),
        RGB(80, 35, 60),
        RGB(255, 238, 246),
        RGB(255, 120, 185),
        1
    },
    {
        L"goldLuxury",
        940, 66, 28, 18, 235,
        RGB(72, 51, 24),
        RGB(24, 18, 10),
        RGB(255, 205, 95),
        RGB(255, 225, 130),
        RGB(95, 68, 30),
        RGB(45, 32, 14),
        RGB(255, 246, 220),
        RGB(255, 205, 75),
        5
    },
    {
        L"vaporwave",
        980, 70, 30, 20, 238,
        RGB(65, 22, 110),
        RGB(12, 20, 70),
        RGB(255, 90, 230),
        RGB(70, 230, 255),
        RGB(88, 30, 135),
        RGB(42, 18, 90),
        RGB(255, 245, 255),
        RGB(80, 245, 255),
        3
    },
    {
        L"crimsonNight",
        900, 62, 24, 16, 230,
        RGB(55, 12, 18),
        RGB(16, 8, 12),
        RGB(230, 40, 68),
        RGB(255, 60, 88),
        RGB(74, 18, 26),
        RGB(38, 10, 16),
        RGB(255, 235, 238),
        RGB(255, 70, 90),
        2
    },
    {
        L"graphitePro",
        860, 56, 16, 14, 225,
        RGB(48, 50, 54),
        RGB(22, 23, 25),
        RGB(130, 134, 142),
        RGB(170, 180, 195),
        RGB(58, 60, 65),
        RGB(32, 34, 38),
        RGB(240, 240, 242),
        RGB(190, 195, 205),
        4
    },
    {
        L"oceanDepth",
        930, 64, 26, 18, 232,
        RGB(8, 54, 82),
        RGB(4, 18, 34),
        RGB(45, 190, 230),
        RGB(50, 230, 255),
        RGB(10, 72, 105),
        RGB(4, 35, 58),
        RGB(225, 248, 255),
        RGB(30, 210, 255),
        1
    },
    {
        L"royalPurple",
        930, 66, 28, 18, 235,
        RGB(48, 22, 86),
        RGB(18, 10, 40),
        RGB(170, 110, 255),
        RGB(190, 95, 255),
        RGB(62, 32, 110),
        RGB(32, 16, 68),
        RGB(246, 238, 255),
        RGB(190, 130, 255),
        2
    },
    {
        L"emeraldGlass",
        900, 62, 24, 16, 228,
        RGB(20, 85, 65),
        RGB(8, 34, 30),
        RGB(75, 225, 165),
        RGB(40, 255, 185),
        RGB(28, 105, 82),
        RGB(12, 55, 45),
        RGB(230, 255, 246),
        RGB(50, 255, 175),
        1
    }
};


struct DockSettings {
    PCWSTR themeId;

    int width;
    int autoFitWidth;
    int height;
    int cornerRadius;
    int bottomMargin;
    int opacity;

    int lockBottomCenter;
    int diagonalMode;
    int diagonalAmount;

    int showStart;
    int showSearch;
    int showTaskView;
    int showQuickSettings;
    int showDesktopButton;
    int showClock;
    int showPinnedApps;
    int showRunningApps;
    int minimizeOnClick;
    int showLabels;

    int padding;
    int gap;
    int buttonRadius;
    int iconInset;

    DockTheme theme;
};

DockSettings g_settings;


struct DockItem {
    std::wstring exePath;
    std::wstring displayName;
    HICON icon;
    HWND hButton;
    HWND hAppWindow;
    int id;
    bool pinned;
    bool running;
};

struct RunningWindow {
    HWND hwnd;
    DWORD pid;
    std::wstring exePath;
    std::wstring title;
};


HWND g_dockWnd = nullptr;
HWND g_btnStart = nullptr;
HWND g_btnSearch = nullptr;
HWND g_btnTaskView = nullptr;
HWND g_btnQuick = nullptr;
HWND g_btnDesktop = nullptr;
HWND g_btnClock = nullptr;

std::vector<DockItem> g_items;

bool g_isLayingOut = false;
bool g_allowMoveTracking = false;
bool g_hasManualPosition = false;
int g_manualX = 0;
int g_manualY = 0;


DockTheme FindTheme(PCWSTR id);
void LoadSettings();
void ApplyDockRegion(HWND hWnd);
void DestroyDockItems();
void RefreshItems(HWND parent, HINSTANCE hInst);
void LayoutDock(HWND hWnd);
DockItem* FindItemById(int id);
void InvalidateAllButtons();


COLORREF BlendColor(COLORREF a, COLORREF b, int percentB) {
    percentB = ClampInt(percentB, 0, 100);
    int percentA = 100 - percentB;

    int ar = GetRValue(a);
    int ag = GetGValue(a);
    int ab = GetBValue(a);

    int br = GetRValue(b);
    int bg = GetGValue(b);
    int bb = GetBValue(b);

    return RGB(
        (ar * percentA + br * percentB) / 100,
        (ag * percentA + bg * percentB) / 100,
        (ab * percentA + bb * percentB) / 100
    );
}

COLORREF PulseColor(COLORREF a, COLORREF b) {
    DWORD tick = GetTickCount();
    int phase = (int)((tick / 20) % 100);

    if (phase > 50)
        phase = 100 - phase;

    return BlendColor(a, b, phase * 2);
}

void FillGradientVertical(HDC hdc, RECT rc, COLORREF top, COLORREF bottom) {
    int height = (int)(rc.bottom - rc.top);

    if (height <= 0)
        return;

    for (int y = 0; y < height; y++) {
        int percent = (y * 100) / height;
        COLORREF c = BlendColor(top, bottom, percent);

        HBRUSH brush = CreateSolidBrush(c);

        RECT line;
        line.left = rc.left;
        line.top = rc.top + y;
        line.right = rc.right;
        line.bottom = rc.top + y + 1;

        FillRect(hdc, &line, brush);
        DeleteObject(brush);
    }
}

void DrawRoundedOutline(HDC hdc, RECT rc, int radius, COLORREF color, int thickness) {
    thickness = ClampInt(thickness, 1, 6);

    for (int i = 0; i < thickness; i++) {
        RECT r = rc;
        InflateRect(&r, -i, -i);

        HPEN pen = CreatePen(PS_SOLID, 1, color);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);

        RoundRect(
            hdc,
            r.left,
            r.top,
            r.right - 1,
            r.bottom - 1,
            radius * 2,
            radius * 2
        );

        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(pen);
    }
}

void DrawScanlines(HDC hdc, RECT rc, COLORREF color) {
    COLORREF lineColor = BlendColor(color, RGB(0, 0, 0), 45);

    HPEN pen = CreatePen(PS_SOLID, 1, lineColor);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    for (int y = rc.top + 2; y < rc.bottom; y += 4) {
        MoveToEx(hdc, rc.left, y, nullptr);
        LineTo(hdc, rc.right, y);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void DrawGlassShine(HDC hdc, RECT rc) {
    int h = (int)(rc.bottom - rc.top);
    int shineH = h / 2;

    if (shineH <= 0)
        return;

    for (int y = 0; y < shineH; y++) {
        int strength = 22 - (y * 22 / shineH);
        COLORREF c = RGB(strength, strength, strength + 8);

        HBRUSH brush = CreateSolidBrush(c);

        RECT line;
        line.left = rc.left;
        line.right = rc.right;
        line.top = rc.top + y;
        line.bottom = rc.top + y + 1;

        FillRect(hdc, &line, brush);
        DeleteObject(brush);
    }
}

void DrawLuxuryLine(HDC hdc, RECT rc, COLORREF color) {
    RECT top = rc;
    top.bottom = top.top + 2;

    HBRUSH brush = CreateSolidBrush(color);
    FillRect(hdc, &top, brush);
    DeleteObject(brush);

    RECT bottom = rc;
    bottom.top = bottom.bottom - 2;

    brush = CreateSolidBrush(BlendColor(color, RGB(0, 0, 0), 30));
    FillRect(hdc, &bottom, brush);
    DeleteObject(brush);
}


DockTheme FindTheme(PCWSTR id) {
    int count = (int)(sizeof(g_builtinThemes) / sizeof(g_builtinThemes[0]));

    for (int i = 0; i < count; i++) {
        if (id && wcscmp(g_builtinThemes[i].id, id) == 0)
            return g_builtinThemes[i];
    }

    return g_builtinThemes[1];
}

bool IsCustomTheme(PCWSTR id) {
    return id && wcscmp(id, L"custom") == 0;
}

void LoadSettings() {
    g_settings.themeId = Wh_GetStringSetting(L"theme");

    DockTheme selected = FindTheme(g_settings.themeId);

    if (IsCustomTheme(g_settings.themeId)) {
        selected.id = L"custom";
        selected.width = 900;
        selected.height = 64;
        selected.cornerRadius = 26;
        selected.bottomMargin = 16;
        selected.opacity = 235;

        selected.dockTop = HexToColor(Wh_GetIntSetting(L"customDockTop"));
        selected.dockBottom = HexToColor(Wh_GetIntSetting(L"customDockBottom"));
        selected.borderColor = HexToColor(Wh_GetIntSetting(L"customBorder"));
        selected.glowColor = HexToColor(Wh_GetIntSetting(L"customGlow"));
        selected.buttonColor = HexToColor(Wh_GetIntSetting(L"customButton"));
        selected.buttonPressedColor = HexToColor(Wh_GetIntSetting(L"customButtonPressed"));
        selected.textColor = HexToColor(Wh_GetIntSetting(L"customText"));
        selected.runningColor = HexToColor(Wh_GetIntSetting(L"customRunning"));
        selected.specialEffect = ClampInt(Wh_GetIntSetting(L"customSpecialEffect"), 0, 5);
    }

    g_settings.theme = selected;

    int width = Wh_GetIntSetting(L"width");
    int height = Wh_GetIntSetting(L"height");
    int cornerRadius = Wh_GetIntSetting(L"cornerRadius");
    int bottomMargin = Wh_GetIntSetting(L"bottomMargin");
    int opacity = Wh_GetIntSetting(L"opacity");

    g_settings.width = width > 0 ? width : selected.width;
    g_settings.height = height > 0 ? height : selected.height;
    g_settings.cornerRadius = cornerRadius > 0 ? cornerRadius : selected.cornerRadius;
    g_settings.bottomMargin = bottomMargin > 0 ? bottomMargin : selected.bottomMargin;
    g_settings.opacity = opacity > 0 ? opacity : selected.opacity;

    g_settings.autoFitWidth = Wh_GetIntSetting(L"autoFitWidth");
    g_settings.lockBottomCenter = Wh_GetIntSetting(L"lockBottomCenter");
    g_settings.diagonalMode = Wh_GetIntSetting(L"diagonalMode");
    g_settings.diagonalAmount = Wh_GetIntSetting(L"diagonalAmount");

    g_settings.showStart = Wh_GetIntSetting(L"showStart");
    g_settings.showSearch = Wh_GetIntSetting(L"showSearch");
    g_settings.showTaskView = Wh_GetIntSetting(L"showTaskView");
    g_settings.showQuickSettings = Wh_GetIntSetting(L"showQuickSettings");
    g_settings.showDesktopButton = Wh_GetIntSetting(L"showDesktopButton");
    g_settings.showClock = Wh_GetIntSetting(L"showClock");
    g_settings.showPinnedApps = Wh_GetIntSetting(L"showPinnedApps");
    g_settings.showRunningApps = Wh_GetIntSetting(L"showRunningApps");
    g_settings.minimizeOnClick = Wh_GetIntSetting(L"minimizeOnClick");
    g_settings.showLabels = Wh_GetIntSetting(L"showLabels");

    g_settings.padding = Wh_GetIntSetting(L"padding");
    g_settings.gap = Wh_GetIntSetting(L"gap");
    g_settings.buttonRadius = Wh_GetIntSetting(L"buttonRadius");
    g_settings.iconInset = Wh_GetIntSetting(L"iconInset");

    g_settings.width = ClampInt(g_settings.width, 240, 3200);
    g_settings.height = ClampInt(g_settings.height, 36, 220);
    g_settings.cornerRadius = ClampInt(g_settings.cornerRadius, 0, 120);
    g_settings.bottomMargin = ClampInt(g_settings.bottomMargin, 0, 400);
    g_settings.opacity = ClampInt(g_settings.opacity, 0, 255);

    g_settings.diagonalMode = ClampInt(g_settings.diagonalMode, 0, 2);
    g_settings.diagonalAmount = ClampInt(g_settings.diagonalAmount, 0, 40);

    g_settings.padding = ClampInt(g_settings.padding, 0, 50);
    g_settings.gap = ClampInt(g_settings.gap, 0, 50);
    g_settings.buttonRadius = ClampInt(g_settings.buttonRadius, 0, 50);
    g_settings.iconInset = ClampInt(g_settings.iconInset, 2, 32);

    if (SettingOn(g_settings.lockBottomCenter))
        g_hasManualPosition = false;
}


void ApplyDockRegion(HWND hWnd) {
    if (!hWnd)
        return;

    RECT rect;

    if (!GetClientRect(hWnd, &rect))
        return;

    int width = (int)(rect.right - rect.left);
    int height = (int)(rect.bottom - rect.top);

    if (width <= 0 || height <= 0)
        return;

    int radius = g_settings.cornerRadius;

    if (radius <= 0) {
        SetWindowRgn(hWnd, nullptr, TRUE);
        return;
    }

    HRGN hRgn = CreateRoundRectRgn(
        0,
        0,
        width,
        height,
        radius * 2,
        radius * 2
    );

    if (hRgn)
        SetWindowRgn(hWnd, hRgn, TRUE);
}


HWND CreateDockButton(HWND parent, HINSTANCE hInst, int id, const wchar_t* text) {
    return CreateWindowExW(
        0,
        L"BUTTON",
        text,
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        0,
        0,
        32,
        32,
        parent,
        (HMENU)(INT_PTR)id,
        hInst,
        nullptr
    );
}

void DestroySpecialButtons() {
    if (g_btnStart) {
        DestroyWindow(g_btnStart);
        g_btnStart = nullptr;
    }

    if (g_btnSearch) {
        DestroyWindow(g_btnSearch);
        g_btnSearch = nullptr;
    }

    if (g_btnTaskView) {
        DestroyWindow(g_btnTaskView);
        g_btnTaskView = nullptr;
    }

    if (g_btnQuick) {
        DestroyWindow(g_btnQuick);
        g_btnQuick = nullptr;
    }

    if (g_btnDesktop) {
        DestroyWindow(g_btnDesktop);
        g_btnDesktop = nullptr;
    }

    if (g_btnClock) {
        DestroyWindow(g_btnClock);
        g_btnClock = nullptr;
    }
}

void CreateSpecialButtons(HWND parent, HINSTANCE hInst) {
    DestroySpecialButtons();

    if (SettingOn(g_settings.showStart))
        g_btnStart = CreateDockButton(parent, hInst, ID_BTN_START, L"Start");

    if (SettingOn(g_settings.showSearch))
        g_btnSearch = CreateDockButton(parent, hInst, ID_BTN_SEARCH, L"Search");

    if (SettingOn(g_settings.showTaskView))
        g_btnTaskView = CreateDockButton(parent, hInst, ID_BTN_TASKVIEW, L"Task View");

    if (SettingOn(g_settings.showQuickSettings))
        g_btnQuick = CreateDockButton(parent, hInst, ID_BTN_QUICK, L"Quick Settings");

    if (SettingOn(g_settings.showDesktopButton))
        g_btnDesktop = CreateDockButton(parent, hInst, ID_BTN_DESKTOP, L"Desktop");

    if (SettingOn(g_settings.showClock))
        g_btnClock = CreateDockButton(parent, hInst, ID_BTN_CLOCK, L"Clock");
}


std::wstring GetProcessPathFromPid(DWORD pid) {
    std::wstring result;

    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

    if (!h)
        return result;

    wchar_t path[MAX_PATH] = {};
    DWORD size = MAX_PATH;

    if (QueryFullProcessImageNameW(h, 0, path, &size))
        result = path;

    CloseHandle(h);

    return result;
}

std::wstring GetWindowTitleString(HWND hwnd) {
    std::wstring title;

    int len = GetWindowTextLengthW(hwnd);

    if (len <= 0)
        return title;

    std::vector<wchar_t> buffer;
    buffer.resize((size_t)len + 1);

    GetWindowTextW(hwnd, &buffer[0], len + 1);
    title = &buffer[0];

    return title;
}

bool IsUsefulWindow(HWND hwnd) {
    if (!hwnd)
        return false;

    if (!IsWindowVisible(hwnd))
        return false;

    if (hwnd == g_dockWnd)
        return false;

    if (GetWindow(hwnd, GW_OWNER))
        return false;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);

    if ((style & WS_CHILD) != 0)
        return false;

    RECT rc;

    if (!GetWindowRect(hwnd, &rc))
        return false;

    int w = (int)(rc.right - rc.left);
    int h = (int)(rc.bottom - rc.top);

    if (w <= 1 || h <= 1)
        return false;

    std::wstring title = GetWindowTitleString(hwnd);

    if (title.empty())
        return false;

    return true;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    std::vector<RunningWindow>* windows = (std::vector<RunningWindow>*)lParam;

    if (!windows)
        return TRUE;

    if (!IsUsefulWindow(hwnd))
        return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    if (pid == 0)
        return TRUE;

    std::wstring path = GetProcessPathFromPid(pid);

    if (path.empty())
        return TRUE;

    RunningWindow rw;
    rw.hwnd = hwnd;
    rw.pid = pid;
    rw.exePath = path;
    rw.title = GetWindowTitleString(hwnd);

    windows->push_back(rw);

    return TRUE;
}

void GetVisibleRunningWindows(std::vector<RunningWindow>& windows) {
    windows.clear();
    EnumWindows(EnumWindowsProc, (LPARAM)&windows);
}


void DestroyDockItems() {
    for (size_t i = 0; i < g_items.size(); i++) {
        if (g_items[i].hButton)
            DestroyWindow(g_items[i].hButton);

        if (g_items[i].icon)
            DestroyIcon(g_items[i].icon);
    }

    g_items.clear();
}

std::wstring GetPinnedFolderPath() {
    wchar_t appData[MAX_PATH] = {};

    if (FAILED(SHGetFolderPathW(
        nullptr,
        CSIDL_APPDATA,
        nullptr,
        SHGFP_TYPE_CURRENT,
        appData
    ))) {
        return L"";
    }

    std::wstring path = appData;
    path += L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar";
    return path;
}

HICON GetLargeIconForPath(const std::wstring& path) {
    if (path.empty())
        return nullptr;

    SHFILEINFOW sfi = {};

    if (SHGetFileInfoW(
        path.c_str(),
        0,
        &sfi,
        sizeof(sfi),
        SHGFI_ICON | SHGFI_LARGEICON
    )) {
        return sfi.hIcon;
    }

    return nullptr;
}

std::wstring FileNameWithoutExtension(const std::wstring& path) {
    std::wstring name = path;

    size_t slash = name.find_last_of(L"\\/");

    if (slash != std::wstring::npos)
        name = name.substr(slash + 1);

    size_t dot = name.rfind(L'.');

    if (dot != std::wstring::npos)
        name = name.substr(0, dot);

    return name;
}

bool ItemExistsForPath(const std::wstring& exePath, DockItem** found) {
    if (found)
        *found = nullptr;

    if (exePath.empty())
        return false;

    for (size_t i = 0; i < g_items.size(); i++) {
        if (_wcsicmp(g_items[i].exePath.c_str(), exePath.c_str()) == 0) {
            if (found)
                *found = &g_items[i];

            return true;
        }
    }

    return false;
}

DockItem* FindItemById(int id) {
    for (size_t i = 0; i < g_items.size(); i++) {
        if (g_items[i].id == id)
            return &g_items[i];
    }

    return nullptr;
}

void LoadPinnedItems(HWND parent, HINSTANCE hInst) {
    if (!SettingOn(g_settings.showPinnedApps))
        return;

    std::wstring folder = GetPinnedFolderPath();

    if (folder.empty())
        return;

    std::wstring search = folder + L"\\*.lnk";

    WIN32_FIND_DATAW fd = {};
    HANDLE hFind = FindFirstFileW(search.c_str(), &fd);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    int nextId = ID_ITEM_BASE;

    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        std::wstring lnkPath = folder + L"\\" + fd.cFileName;

        IShellLinkW* pLink = nullptr;

        if (FAILED(CoCreateInstance(
            CLSID_ShellLink,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&pLink)
        ))) {
            continue;
        }

        IPersistFile* pPersist = nullptr;

        if (FAILED(pLink->QueryInterface(IID_PPV_ARGS(&pPersist)))) {
            pLink->Release();
            continue;
        }

        if (FAILED(pPersist->Load(lnkPath.c_str(), STGM_READ))) {
            pPersist->Release();
            pLink->Release();
            continue;
        }

        WCHAR exePath[MAX_PATH] = {};
        pLink->GetPath(exePath, MAX_PATH, nullptr, SLGP_UNCPRIORITY);

        WCHAR iconPath[MAX_PATH] = {};
        int iconIndex = 0;
        pLink->GetIconLocation(iconPath, MAX_PATH, &iconIndex);

        HICON hIcon = nullptr;

        if (wcslen(iconPath) > 0)
            hIcon = GetLargeIconForPath(iconPath);

        if (!hIcon && wcslen(exePath) > 0)
            hIcon = GetLargeIconForPath(exePath);

        std::wstring displayName = fd.cFileName;
        size_t dot = displayName.rfind(L'.');

        if (dot != std::wstring::npos)
            displayName = displayName.substr(0, dot);

        HWND hBtn = CreateDockButton(parent, hInst, nextId, displayName.c_str());

        DockItem item = {};
        item.exePath = exePath;
        item.displayName = displayName;
        item.icon = hIcon;
        item.hButton = hBtn;
        item.hAppWindow = nullptr;
        item.id = nextId;
        item.pinned = true;
        item.running = false;

        g_items.push_back(item);

        pPersist->Release();
        pLink->Release();

        nextId++;

    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
}

void AddVisibleRunningApps(HWND parent, HINSTANCE hInst) {
    if (!SettingOn(g_settings.showRunningApps))
        return;

    std::vector<RunningWindow> windows;
    GetVisibleRunningWindows(windows);

    for (size_t i = 0; i < windows.size(); i++) {
        DockItem* existing = nullptr;

        if (ItemExistsForPath(windows[i].exePath, &existing)) {
            if (existing) {
                existing->running = true;
                existing->hAppWindow = windows[i].hwnd;

                if (existing->displayName.empty())
                    existing->displayName = windows[i].title;
            }

            continue;
        }

        HICON hIcon = GetLargeIconForPath(windows[i].exePath);

        int id = ID_ITEM_BASE + 1000 + (int)g_items.size();

        HWND hBtn = CreateDockButton(parent, hInst, id, windows[i].title.c_str());

        DockItem item = {};
        item.exePath = windows[i].exePath;
        item.displayName = windows[i].title.empty()
            ? FileNameWithoutExtension(windows[i].exePath)
            : windows[i].title;
        item.icon = hIcon;
        item.hButton = hBtn;
        item.hAppWindow = windows[i].hwnd;
        item.id = id;
        item.pinned = false;
        item.running = true;

        g_items.push_back(item);
    }
}

void RefreshItems(HWND parent, HINSTANCE hInst) {
    DestroyDockItems();
    LoadPinnedItems(parent, hInst);
    AddVisibleRunningApps(parent, hInst);
}


int CountSpecialButtons() {
    int count = 0;

    if (g_btnStart)
        count++;

    if (g_btnSearch)
        count++;

    if (g_btnTaskView)
        count++;

    if (g_btnQuick)
        count++;

    if (g_btnDesktop)
        count++;

    if (g_btnClock)
        count++;

    return count;
}

int GetClockWidth(int btnH) {
    return IntMax(btnH + 32, 92);
}

int CalculateDockWidth(int padding, int gap, int btnH) {
    int width = padding * 2;

    int special = CountSpecialButtons();

    if (g_btnClock && special > 0)
        special--;

    width += special * btnH;

    if (special > 0)
        width += special * gap;

    if (!g_items.empty()) {
        width += (int)g_items.size() * btnH;
        width += (int)g_items.size() * gap;
    }

    if (g_btnClock) {
        width += GetClockWidth(btnH);
        width += gap;
    }

    return ClampInt(width, 240, 3200);
}

int TotalButtonCountForDiagonal() {
    int count = CountSpecialButtons();
    count += (int)g_items.size();
    return count;
}

int GetDiagonalExtraHeight() {
    if (g_settings.diagonalMode == 0)
        return 0;

    int count = TotalButtonCountForDiagonal();

    if (count <= 1)
        return 0;

    return ClampInt((count - 1) * g_settings.diagonalAmount, 0, 500);
}

void PositionButton(HWND btn, int x, int y, int w, int h) {
    if (!btn)
        return;

    SetWindowPos(
        btn,
        nullptr,
        x,
        y,
        w,
        h,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW
    );
}

int DiagonalY(int baseY, int index, int extraHeight) {
    if (g_settings.diagonalMode == 0)
        return baseY;

    int amount = g_settings.diagonalAmount;

    if (g_settings.diagonalMode == 1)
        return baseY + extraHeight - index * amount;

    if (g_settings.diagonalMode == 2)
        return baseY + index * amount;

    return baseY;
}

void RecordCurrentDockPosition() {
    if (!g_dockWnd)
        return;

    RECT rc;

    if (!GetWindowRect(g_dockWnd, &rc))
        return;

    g_manualX = rc.left;
    g_manualY = rc.top;
    g_hasManualPosition = true;
}

void LayoutDock(HWND hWnd) {
    if (!hWnd)
        return;

    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfo(hMon, &mi))
        return;

    int screenWidth = (int)(mi.rcWork.right - mi.rcWork.left);

    int baseDockH = g_settings.height;
    int extraHeight = GetDiagonalExtraHeight();
    int dockH = baseDockH + extraHeight;

    int padding = g_settings.padding > 0 ? g_settings.padding : IntMax(6, baseDockH / 7);
    int gap = g_settings.gap > 0 ? g_settings.gap : IntMax(6, baseDockH / 8);

    int btnH = baseDockH - padding * 2;

    if (btnH < 18)
        btnH = 18;

    int dockW = g_settings.width;

    if (SettingOn(g_settings.autoFitWidth))
        dockW = CalculateDockWidth(padding, gap, btnH);

    int x = mi.rcWork.left + (screenWidth - dockW) / 2;
    int y = mi.rcWork.bottom - g_settings.bottomMargin - dockH;

    if (!SettingOn(g_settings.lockBottomCenter) && g_hasManualPosition) {
        x = g_manualX;
        y = g_manualY;
    }

    g_isLayingOut = true;

    SetWindowPos(
        hWnd,
        HWND_TOPMOST,
        x,
        y,
        dockW,
        dockH,
        SWP_SHOWWINDOW | SWP_NOACTIVATE
    );

    g_isLayingOut = false;

    ApplyDockRegion(hWnd);

    int curX = padding;
    int baseY = padding;
    int index = 0;

    if (g_settings.diagonalMode == 0)
        baseY = (dockH - btnH) / 2;

    if (g_btnStart) {
        PositionButton(g_btnStart, curX, DiagonalY(baseY, index, extraHeight), btnH, btnH);
        curX += btnH + gap;
        index++;
    }

    if (g_btnSearch) {
        PositionButton(g_btnSearch, curX, DiagonalY(baseY, index, extraHeight), btnH, btnH);
        curX += btnH + gap;
        index++;
    }

    if (g_btnTaskView) {
        PositionButton(g_btnTaskView, curX, DiagonalY(baseY, index, extraHeight), btnH, btnH);
        curX += btnH + gap;
        index++;
    }

    if (g_btnQuick) {
        PositionButton(g_btnQuick, curX, DiagonalY(baseY, index, extraHeight), btnH, btnH);
        curX += btnH + gap;
        index++;
    }

    if (g_btnDesktop) {
        PositionButton(g_btnDesktop, curX, DiagonalY(baseY, index, extraHeight), btnH, btnH);
        curX += btnH + gap * 2;
        index++;
    }

    for (size_t i = 0; i < g_items.size(); i++) {
        if (!g_items[i].hButton)
            continue;

        PositionButton(
            g_items[i].hButton,
            curX,
            DiagonalY(baseY, index, extraHeight),
            btnH,
            btnH
        );

        curX += btnH + gap;
        index++;
    }

    if (g_btnClock) {
        int clockW = GetClockWidth(btnH);

        PositionButton(
            g_btnClock,
            curX,
            DiagonalY(baseY, index, extraHeight),
            clockW,
            btnH
        );

        curX += clockW + gap;
        index++;
    }

    InvalidateRect(hWnd, nullptr, TRUE);
    InvalidateAllButtons();
}


void SendKeyCombo(WORD key1, WORD key2) {
    INPUT inputs[4] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key1;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key2;

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = key2;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = key1;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(4, inputs, sizeof(INPUT));
}

void SendSingleKey(WORD key) {
    INPUT inputs[2] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
}

void ActivateOrLaunchItem(DockItem* item) {
    if (!item)
        return;

    if (item->running && item->hAppWindow && IsWindow(item->hAppWindow)) {
        HWND foreground = GetForegroundWindow();

        if (foreground == item->hAppWindow && SettingOn(g_settings.minimizeOnClick)) {
            ShowWindow(item->hAppWindow, SW_MINIMIZE);
            return;
        }

        if (IsIconic(item->hAppWindow))
            ShowWindow(item->hAppWindow, SW_RESTORE);
        else
            ShowWindow(item->hAppWindow, SW_SHOW);

        SetForegroundWindow(item->hAppWindow);
        return;
    }

    if (!item->exePath.empty()) {
        ShellExecuteW(
            nullptr,
            L"open",
            item->exePath.c_str(),
            nullptr,
            nullptr,
            SW_SHOWNORMAL
        );
    }
}


void DrawDockBackground(HWND hWnd, HDC hdc) {
    RECT rc = {};
    GetClientRect(hWnd, &rc);

    FillGradientVertical(
        hdc,
        rc,
        g_settings.theme.dockTop,
        g_settings.theme.dockBottom
    );

    int effect = g_settings.theme.specialEffect;

    if (effect == 1)
        DrawGlassShine(hdc, rc);

    if (effect == 2) {
        COLORREF pulse = PulseColor(g_settings.theme.borderColor, g_settings.theme.glowColor);
        DrawRoundedOutline(hdc, rc, g_settings.cornerRadius, pulse, 2);
    }

    if (effect == 3)
        DrawScanlines(hdc, rc, g_settings.theme.glowColor);

    if (effect == 5)
        DrawLuxuryLine(hdc, rc, g_settings.theme.borderColor);

    DrawRoundedOutline(
        hdc,
        rc,
        g_settings.cornerRadius,
        g_settings.theme.borderColor,
        effect == 5 ? 2 : 1
    );
}

void DrawButtonFrame(HDC hdc, RECT rc, bool pressed, bool active) {
    COLORREF bgColor = pressed
        ? g_settings.theme.buttonPressedColor
        : g_settings.theme.buttonColor;

    if (active)
        bgColor = BlendColor(bgColor, g_settings.theme.runningColor, 18);

    HBRUSH bg = CreateSolidBrush(bgColor);
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    COLORREF border = active
        ? g_settings.theme.runningColor
        : BlendColor(g_settings.theme.borderColor, bgColor, 45);

    if (g_settings.theme.specialEffect == 2 && active)
        border = PulseColor(g_settings.theme.runningColor, g_settings.theme.glowColor);

    DrawRoundedOutline(
        hdc,
        rc,
        g_settings.buttonRadius,
        border,
        active && g_settings.theme.specialEffect == 2 ? 2 : 1
    );
}

void DrawCenteredText(HDC hdc, RECT rc, const wchar_t* text, int fontSize, bool bold) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, g_settings.theme.textColor);

    HFONT font = CreateFontW(
        fontSize,
        0,
        0,
        0,
        bold ? FW_SEMIBOLD : FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );

    HFONT oldFont = (HFONT)SelectObject(hdc, font);

    DrawTextW(
        hdc,
        text,
        -1,
        &rc,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS
    );

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

void DrawGlyphButton(LPDRAWITEMSTRUCT dis, const wchar_t* glyph) {
    if (!dis)
        return;

    bool pressed = (dis->itemState & ODS_SELECTED) != 0;

    RECT rc = dis->rcItem;

    DrawButtonFrame(dis->hDC, rc, pressed, false);

    int itemH = (int)(rc.bottom - rc.top);
    int fontSize = -IntMax(15, itemH / 2);

    DrawCenteredText(dis->hDC, rc, glyph, fontSize, true);
}

void DrawClockButton(LPDRAWITEMSTRUCT dis) {
    if (!dis)
        return;

    bool pressed = (dis->itemState & ODS_SELECTED) != 0;

    RECT rc = dis->rcItem;

    DrawButtonFrame(dis->hDC, rc, pressed, false);

    SYSTEMTIME st = {};
    GetLocalTime(&st);

    wchar_t buffer[64] = {};
    wsprintfW(
        buffer,
        L"%02d:%02d",
        st.wHour,
        st.wMinute
    );

    DrawCenteredText(dis->hDC, rc, buffer, -16, true);
}

void DrawRunningIndicator(HDC hdc, RECT rc, bool pinned) {
    int itemW = (int)(rc.right - rc.left);
    int itemH = (int)(rc.bottom - rc.top);

    int underlineHeight = IntMax(3, itemH / 13);
    int inset = pinned ? IntMax(9, itemW / 4) : IntMax(6, itemW / 5);

    RECT underline = rc;
    underline.left += inset;
    underline.right -= inset;
    underline.top = underline.bottom - underlineHeight - 2;
    underline.bottom -= 2;

    if (underline.right <= underline.left || underline.bottom <= underline.top)
        return;

    COLORREF color = g_settings.theme.runningColor;

    if (g_settings.theme.specialEffect == 2)
        color = PulseColor(g_settings.theme.runningColor, g_settings.theme.glowColor);

    HBRUSH ul = CreateSolidBrush(color);
    FillRect(hdc, &underline, ul);
    DeleteObject(ul);
}

void DrawAppLabel(HDC hdc, RECT rc, const std::wstring& label) {
    if (!SettingOn(g_settings.showLabels))
        return;

    RECT labelRc = rc;
    labelRc.top = rc.bottom - 15;
    labelRc.bottom = rc.bottom - 2;
    labelRc.left += 2;
    labelRc.right -= 2;

    DrawCenteredText(hdc, labelRc, label.c_str(), -10, false);
}

void DrawAppButton(LPDRAWITEMSTRUCT dis, DockItem* item) {
    if (!dis || !item)
        return;

    bool pressed = (dis->itemState & ODS_SELECTED) != 0;

    RECT rc = dis->rcItem;

    int itemW = (int)(rc.right - rc.left);
    int itemH = (int)(rc.bottom - rc.top);

    if (itemW <= 0 || itemH <= 0)
        return;

    bool active = item->running;

    DrawButtonFrame(dis->hDC, rc, pressed, active);

    int iconAreaBottom = SettingOn(g_settings.showLabels)
        ? rc.bottom - 13
        : rc.bottom;

    if (item->icon) {
        int smallest = IntMin(itemW, iconAreaBottom - rc.top);
        int iconSize = smallest - g_settings.iconInset;

        if (iconSize < 12)
            iconSize = 12;

        int x = rc.left + (itemW - iconSize) / 2;
        int y = rc.top + ((iconAreaBottom - rc.top) - iconSize) / 2;

        DrawIconEx(
            dis->hDC,
            x,
            y,
            item->icon,
            iconSize,
            iconSize,
            0,
            nullptr,
            DI_NORMAL
        );
    } else {
        RECT textRc = rc;
        textRc.bottom = iconAreaBottom;
        DrawCenteredText(dis->hDC, textRc, item->displayName.c_str(), -12, false);
    }

    if (item->running)
        DrawRunningIndicator(dis->hDC, rc, item->pinned);

    DrawAppLabel(dis->hDC, rc, item->displayName);
}

void InvalidateAllButtons() {
    if (g_btnStart)
        InvalidateRect(g_btnStart, nullptr, TRUE);

    if (g_btnSearch)
        InvalidateRect(g_btnSearch, nullptr, TRUE);

    if (g_btnTaskView)
        InvalidateRect(g_btnTaskView, nullptr, TRUE);

    if (g_btnQuick)
        InvalidateRect(g_btnQuick, nullptr, TRUE);

    if (g_btnDesktop)
        InvalidateRect(g_btnDesktop, nullptr, TRUE);

    if (g_btnClock)
        InvalidateRect(g_btnClock, nullptr, TRUE);

    for (size_t i = 0; i < g_items.size(); i++) {
        if (g_items[i].hButton)
            InvalidateRect(g_items[i].hButton, nullptr, TRUE);
    }
}


LRESULT CALLBACK DockWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN:
        ReleaseCapture();
        SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        return 0;

    case WM_EXITSIZEMOVE:
        if (!SettingOn(g_settings.lockBottomCenter)) {
            RecordCurrentDockPosition();
        }
        return 0;

    case WM_WINDOWPOSCHANGED:
        if (g_allowMoveTracking && !g_isLayingOut && !SettingOn(g_settings.lockBottomCenter)) {
            WINDOWPOS* wp = (WINDOWPOS*)lParam;

            if (wp && !(wp->flags & SWP_NOMOVE)) {
                g_manualX = wp->x;
                g_manualY = wp->y;
                g_hasManualPosition = true;
            }
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps = {};
        HDC hdc = BeginPaint(hWnd, &ps);

        DrawDockBackground(hWnd, hdc);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_SIZE:
        ApplyDockRegion(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
        return 0;

    case WM_TIMER:
        if (wParam == TIMER_REDRAW) {
            InvalidateRect(hWnd, nullptr, TRUE);
            InvalidateAllButtons();
            return 0;
        }

        if (wParam == TIMER_REFRESH) {
            HINSTANCE hInst = GetModuleHandle(nullptr);
            RefreshItems(hWnd, hInst);
            LayoutDock(hWnd);
            return 0;
        }

        return 0;

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;

        if (!dis)
            return TRUE;

        int id = (int)dis->CtlID;

        if (id == ID_BTN_START) {
            DrawGlyphButton(dis, L"\x229E");
            return TRUE;
        }

        if (id == ID_BTN_SEARCH) {
            DrawGlyphButton(dis, L"S");
            return TRUE;
        }

        if (id == ID_BTN_TASKVIEW) {
            DrawGlyphButton(dis, L"\x25A3");
            return TRUE;
        }

        if (id == ID_BTN_QUICK) {
            DrawGlyphButton(dis, L"\x2699");
            return TRUE;
        }

        if (id == ID_BTN_DESKTOP) {
            DrawGlyphButton(dis, L"D");
            return TRUE;
        }

        if (id == ID_BTN_CLOCK) {
            DrawClockButton(dis);
            return TRUE;
        }

        DockItem* item = FindItemById(id);

        if (item) {
            DrawAppButton(dis, item);
            return TRUE;
        }

        return TRUE;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);

        if (id == ID_BTN_START) {
            SendSingleKey(VK_LWIN);
            return 0;
        }

        if (id == ID_BTN_SEARCH) {
            SendKeyCombo(VK_LWIN, 'S');
            return 0;
        }

        if (id == ID_BTN_TASKVIEW) {
            SendKeyCombo(VK_LWIN, VK_TAB);
            return 0;
        }

        if (id == ID_BTN_QUICK) {
            SendKeyCombo(VK_LWIN, 'A');
            return 0;
        }

        if (id == ID_BTN_DESKTOP) {
            SendKeyCombo(VK_LWIN, 'D');
            return 0;
        }

        if (id == ID_BTN_CLOCK) {
            ShellExecuteW(
                nullptr,
                L"open",
                L"ms-clock:",
                nullptr,
                nullptr,
                SW_SHOWNORMAL
            );
            return 0;
        }

        if (id >= ID_ITEM_BASE) {
            DockItem* item = FindItemById(id);
            ActivateOrLaunchItem(item);
            return 0;
        }

        break;
    }

    case WM_DESTROY:
        KillTimer(hWnd, TIMER_REDRAW);
        KillTimer(hWnd, TIMER_REFRESH);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}


bool CreateDockWindow() {
    HINSTANCE hInst = GetModuleHandle(nullptr);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = DockWndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = DOCK_CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;

    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"Failed to register dock window class");
        return false;
    }

    g_dockWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        DOCK_CLASS_NAME,
        L"",
        WS_POPUP,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        g_settings.width,
        g_settings.height,
        nullptr,
        nullptr,
        hInst,
        nullptr
    );

    if (!g_dockWnd) {
        Wh_Log(L"Failed to create dock window");
        return false;
    }

    SetLayeredWindowAttributes(
        g_dockWnd,
        0,
        (BYTE)g_settings.opacity,
        LWA_ALPHA
    );

    CreateSpecialButtons(g_dockWnd, hInst);
    RefreshItems(g_dockWnd, hInst);

    g_allowMoveTracking = false;
    LayoutDock(g_dockWnd);
    g_allowMoveTracking = true;

    SetTimer(g_dockWnd, TIMER_REDRAW, 1000, nullptr);
    SetTimer(g_dockWnd, TIMER_REFRESH, 4000, nullptr);

    ShowWindow(g_dockWnd, SW_SHOWNOACTIVATE);
    UpdateWindow(g_dockWnd);

    return true;
}

void DestroyDockWindow() {
    DestroyDockItems();
    DestroySpecialButtons();

    if (g_dockWnd) {
        DestroyWindow(g_dockWnd);
        g_dockWnd = nullptr;
    }
}


BOOL Wh_ModInit() {
    Wh_Log(L"Custom Dock Bar Deluxe init");

    CoInitialize(nullptr);

    LoadSettings();

    if (!CreateDockWindow())
        Wh_Log(L"Failed to create Custom Dock Bar Deluxe");

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Custom Dock Bar Deluxe uninit");

    DestroyDockWindow();

    CoUninitialize();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Custom Dock Bar Deluxe settings changed");

    LoadSettings();

    if (g_dockWnd) {
        SetLayeredWindowAttributes(
            g_dockWnd,
            0,
            (BYTE)g_settings.opacity,
            LWA_ALPHA
        );

        HINSTANCE hInst = GetModuleHandle(nullptr);

        CreateSpecialButtons(g_dockWnd, hInst);
        RefreshItems(g_dockWnd, hInst);
        LayoutDock(g_dockWnd);

        InvalidateRect(g_dockWnd, nullptr, TRUE);
        InvalidateAllButtons();
    }
}
