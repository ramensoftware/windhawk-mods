// ==WindhawkMod==
// @id              wheel-context-menu
// @name            Wheel Context Menu
// @description     Replaces classic context menus with a fading, rotating wheel you can spin, scroll, type into and rearrange
// @version         0.1
// @author          Johhannas Reyn
// @github          https://github.com/JohhannasReyn
// @homepage        https://thegnosys.com
// @include         explorer.exe
// @compilerOptions -ld2d1 -ldwrite -ldwmapi -lgdi32 -ladvapi32 -lshell32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Wheel Context Menu

Shows classic (Win32) context menus as a half wheel around the cursor. The
item on the centre line, or the one under the cursor, is highlighted; items
fade out toward the top and bottom of the arc.

![Wheel Context Menu in use](https://raw.githubusercontent.com/JohhannasReyn/Wheel-Context-Menu/main/images/demo.gif)

## Using it

**Mouse**
- Move above or below the centre band to spin the wheel. The further you go,
  the faster it spins.
- Scroll inside the wheel to step one item. Scroll outside it to dismiss.
- Click a label to choose it. A click on empty space inside the wheel
  chooses the highlighted item by default; that can be changed to do
  nothing or to close the wheel. Click outside to dismiss.
- Press and pull a label to rearrange. The order is remembered for that menu.
- Right-click inside the wheel for the standard menu.

**Keyboard**
- Type a name to jump to it. Up / Down step, Left / Right jump to the ends.
- Enter chooses, Backspace goes back, Esc closes.

**Wheel options** (last entry in the wheel) changes most settings on the
spot. A change made there holds until you next change the mod's settings in
Windhawk, which then take over again.

## Windows 11

Explorer's modern context menu is not a classic menu, so the wheel only
appears where the classic one would: after "Show more options", with
Shift + right-click, or everywhere if you restore the classic menu (for
example with the "Classic context menu on Windows 11" mod).

## Other programs

Only `explorer.exe` is targeted by default. Any program that shows classic
menus through `TrackPopupMenu` can be added from the mod's Advanced tab.
Programs that draw their own menus (browsers, Office, most Electron apps)
are not affected. A program that is not per-monitor DPI aware may misjudge
clicks on a scaled display.

## Other context menu mods

Mods that replace the same menus, such as Custom Cached Context Menu, hook
the same functions. With two of them enabled, whichever hook runs first
takes the menu, so use one at a time.

## Known limits

- A menu whose top level has owner-drawn items with no readable text is
  shown as the standard menu instead. Inside a submenu such items show as
  "Item N".
- Menu icons are not shown.
- Stacked submenus can run past the screen edge when a menu is deep.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- side: right
  $name: Opens toward
  $description: The wheel flips to the other side on its own when it would not fit on the screen.
  $options:
  - right: Right
  - left: Left
- opensOn: smart
  $name: Opens on
  $description: Which item is highlighted when the wheel appears.
  $options:
  - first: First item
  - last: Last choice
  - smart: Last choice, or Paste after Cut/Copy
- recentOnTop: 0
  $name: Recent choices on top
  $description: How many recent choices to move to the top of the wheel. 0 turns this off.
- submenus: replace
  $name: Submenus
  $options:
  - replace: Replace the wheel
  - stack: Stack outward as rings
  - alternate: Alternate between two rings
- theme: dark
  $name: Theme
  $options:
  - dark: Dark
  - light: Light
  - system: Match Windows
- accent: false
  $name: Use the Windows accent color
  $description: Colors the outline and the highlight with your Windows accent color.
- tilt: true
  $name: Tilt to spin
  $description: Spin the wheel by moving the mouse above or below the centre band.
- tiltSpeed: 18
  $name: Tilt speed
  $description: Items per second at full tilt.
- tiltCurve: 140
  $name: Tilt response
  $description: 100 is linear. Higher values are gentler near the band and steeper far from it.
- deadBand: 75
  $name: Dead band
  $description: Half-height of the centre band, in tenths of a degree.
- itemSpacing: 12
  $name: Item spacing
  $description: Degrees between items on the inner ring.
- wrap: false
  $name: Wrap long lists
- fadeOutAfter: 60
  $name: Fade out after
  $description: Seconds without any input before the wheel fades away.
- labelOpacity: 88
  $name: Label opacity
  $description: Opacity of the label backgrounds at the centre of the arc, 0 to 100.
- edgeOpacity: 45
  $name: Edge opacity
  $description: Opacity half way toward the ends of the arc (1:30 and 4:30), 0 to 100.
- textColor: ""
  $name: Text color
  $description: Hex color such as #F2F2F2. Leave empty to follow the theme.
- labelColor: ""
  $name: Label background color
  $description: Hex color such as #1F1F26. Leave empty to follow the theme.
- highlightColor: ""
  $name: Highlight color
  $description: Hex color such as #3D7EFF. Leave empty to follow the theme or accent.
- emptyClick: choose
  $name: Click on empty space
  $description: What a click inside the wheel does when it is not on a label, for example at the centre where the wheel opened. Choosing the highlighted item makes "right-click, click" repeat your last choice.
  $options:
  - choose: Choose the highlighted item
  - nothing: Do nothing
  - close: Close the wheel
- showDefaultEntry: true
  $name: Show "Default context options"
  $description: Adds an entry that opens the standard menu. A right-click inside the wheel always does the same.
- showOptionsEntry: true
  $name: Show "Wheel options"
- hideItems:
  - ""
  $name: Hidden items
  $description: Menu items to leave out of the wheel, by name. Use * as a wildcard, for example "Scan with*".
- customItems:
  - - name: ""
      $name: Label
    - command: ""
      $name: Command
      $description: A program, document or URL to open, with optional arguments.
  $name: Custom items
  $description: Extra entries added to the end of every wheel.
- copyNames: Cut;Copy
  $name: Copy item names
  $description: Names of the items after which Paste is highlighted next time. Separate with semicolons. Change these if Windows is not in English.
- pasteNames: Paste
  $name: Paste item names
- neverPreselect: Delete
  $name: Never highlight on open
  $description: Items that are never highlighted when the wheel opens, so a stray click cannot repeat them.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Tunables. LoadConfig() fills the user-facing ones from the mod's settings.
// ---------------------------------------------------------------------------

struct AlphaStop {
    float deg;    // clock angle: 0 = 12:00, 90 = 3:00, 180 = 6:00
    float alpha;  // 0..1
};

struct CustomItem {
    std::wstring name;
    std::wstring command;
};

struct Config {
    // Geometry, in 96-dpi pixels (scaled by monitor DPI at open time).
    float outerRadius = 340.0f;  // maximum; the rim shrinks to fit the labels
    float rimGap = 14.0f;        // space between the longest label and the rim
    float hubRadius = 96.0f;     // inner outline, just inside the labels
    float textInner = 112.0f;    // spoke labels start here...
    float textOuter = 326.0f;    // ...and end here
    float slotHeight = 26.0f;    // text box height
    float pillHeight = 21.0f;    // label background height
    float pillPad = 8.0f;        // label background padding left/right
    float labelBgOpacity = 0.88f;
    float outlineWidth = 1.5f;
    float outlineShadow = 0.55f; // dark halo so the white line shows on white
    bool innerOutline = true;
    float fontSize = 15.0f;
    const wchar_t* fontName = L"Segoe UI";

    // Angular alpha profile of the wheel, clockwise from 12:00.
    // 12:00 clear -> 1:30 semi -> 2:30..3:30 opaque -> 4:30 semi -> 6:00 clear.
    AlphaStop stops[6] = {
        {0.0f, 0.0f},   {45.0f, 0.45f},  {75.0f, 1.0f},
        {105.0f, 1.0f}, {135.0f, 0.45f}, {180.0f, 0.0f},
    };

    // Colors, 0xRRGGBB.
    uint32_t outlineColor = 0xFFFFFF;
    uint32_t haloColor = 0x000000;
    uint32_t bgColor = 0x1F1F26; // label background
    uint32_t textColor = 0xF2F2F2;
    uint32_t accentColor = 0x3D7EFF;
    uint32_t selTextColor = 0xFFFFFF;

    // Navigation.
    float stepDeg = 12.0f;       // angle between adjacent items
    float deadBandDeg = 7.5f;    // band lines sit at 3:00 +/- this (2:45, 3:15)
    float maxSpeed = 18.0f;      // items per second at full tilt
    float expo = 1.4f;           // tilt response curve; 1 = linear
    float tiltRange = 0.55f;     // fraction of the radius where tilt maxes out
    float wheelHoldPx = 14.0f;   // mouse travel that hands control back to tilt
    float snapRate = 16.0f;      // detent easing; higher = snappier
    bool tilt = true;
    bool wrap = false;           // off: stop at the first and last item
    bool guides = false;         // draw the dead-band lines

    // Look. ApplyTheme() turns these into the colours above.
    int theme = 0;               // 0 dark, 1 light, 2 follow Windows
    bool accentOutline = false;  // outline in the Windows accent colour

    // Submenus: 0 replace the wheel's contents, 1 stack outward as rings,
    // 2 alternate between an inner and an outer ring.
    int menuMode = 0;
    float ringGap = 16.0f;       // rim of one ring to the labels of the next
    float minStepDeg = 3.5f;     // finest label spacing on outer rings
    float parentDim = 0.55f;     // opacity of the rings behind the active one

    // A click inside the wheel that is not on a label: 0 chooses the
    // highlighted item, 1 does nothing, 2 closes the wheel.
    int emptyClick = 0;

    // Placement. The arc flips to the other side when it would not fit.
    bool preferLeft = false;

    // Rearranging: press a label and hold, or press and pull.
    float holdSec = 0.30f;
    float dragStartPx = 8.0f;

    // Keyboard: typed letters jump to a matching item.
    float typeResetSec = 1.0f;   // pause after which typing starts over

    // Memory.
    int preselect = 2;           // 0 first item, 1 last choice,
                                 // 2 last choice + follow-ups (Copy -> Paste)
    int mruCount = 0;            // recent choices hoisted to the top; 0 = off

    // Lifetime. The timeout counts from the last interaction, not from open.
    float idleTimeoutSec = 60.0f;
    float fadeOutSec = 0.6f;
    float fadeInSec = 0.12f;
    float dismissSec = 0.08f;    // scroll/click outside

    // Straight from the mod's settings.
    bool hasTextColor = false, hasLabelColor = false, hasHighlight = false;
    uint32_t userText = 0, userLabel = 0, userHighlight = 0;
    bool showNativeEntry = true;   // "Default context options"
    bool showOptionsEntry = true;  // "Wheel options"
    std::vector<std::wstring> hideItems;       // wildcard patterns
    std::vector<CustomItem> customItems;
    std::vector<std::wstring> copyNames = {L"cut", L"copy"};  // lower-case
    std::vector<std::wstring> pasteNames = {L"paste"};
    std::vector<std::wstring> neverPreselect = {L"delete"};
};

static Config cfg;

// ---------------------------------------------------------------------------
// Menu model
// ---------------------------------------------------------------------------

enum : int {
    ID_BACK = -1,
    ID_NATIVE = -2,
    ID_OPTIONS = -3,      // the "Wheel options" entry
    ID_OPT_SETTING = -4,  // one setting inside it
    ID_CUSTOM = -5,       // a user-defined entry that runs a command
    ID_REAL = 1,           // a real menu item; see cmd / hOwner / pos
    ID_OPT_BASE = 1000,   // radio choices: base + setting * 32 + choice
};

struct Item {
    std::wstring text;
    int id = 0;
    bool disabled = false;
    bool groupStart = false;  // a separator precedes this item
    bool hoisted = false;     // shown at the top as a recent choice
    bool radio = false;       // one of a group of exclusive choices
    bool checked = false;     // the current choice in that group

    // For real menu items.
    UINT cmd = 0;             // command ID
    HMENU hOwner = nullptr;   // the menu it sits in
    UINT pos = 0;             // its position there
    HMENU hSub = nullptr;     // its submenu, if any
    bool lazy = false;        // submenu not read yet (sub holds a stand-in)
    std::wstring command;     // for ID_CUSTOM
    std::vector<Item> sub;
};

static Item MI(const wchar_t* text, int id, bool group = false,
               bool disabled = false) {
    Item it;
    it.text = text;
    it.id = id;
    it.groupStart = group;
    it.disabled = disabled;
    return it;
}

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

enum : int { R_NONE = 0, R_COMMAND, R_NATIVE };

struct Level {
    std::vector<Item> items;
    double savedRot = 0.0;  // parent's position, restored on Back
    std::vector<float> widths;  // measured label widths, px
    float widthScale = 0.0f;
    std::vector<Item>* src = nullptr;  // where this level lives in the model
    std::wstring rootText;      // top-level entry this level hangs off
    int hoisted = 0;            // leading items that are recent choices
    float innerPx = 0.0f;       // where this level's labels start
    float rimPx = 0.0f;         // its outline radius
    float step = 0.0f;          // degrees between its items
};

struct Wheel {
    HWND hwnd = nullptr;
    bool open = false;

    // The TrackPopupMenu call being served.
    HWND owner = nullptr;
    bool notify = true;      // owner gets menu messages (no TPM_NONOTIFY)
    HWND fg = nullptr;       // foreground window while open
    std::vector<HMENU> loadedSubs;
    int result = 0;          // R_NONE / R_COMMAND / R_NATIVE
    UINT resCmd = 0;
    HMENU resMenu = nullptr;
    UINT resPos = 0;
    bool forceRender = true; // something changed that FrameKey can't see
    std::vector<MSG> pending; // hook events delivered by a nested loop
    POINT center = {};       // screen px
    int tiltY = 0;           // screen y the dead band is centred on
    float scale = 1.0f;
    int size = 0;            // window is size x size px
    float outerPx = 0.0f;    // current rim radius, px (fits this level)
    float tiltR = 0.0f;      // radius the dead band and tilt are measured on
    std::vector<float> bgRadii;
    int side = 1;            // +1 arc on the right, -1 mirrored to the left
    int bgSide = 0;

    // Left button: click to choose, hold or pull to rearrange.
    bool pressed = false;
    POINT pressPos = {};
    double pressTime = 0.0;
    int pressSlot = -1;      // label under the press, for dragging
    int pressTarget = 0;     // item the click will choose
    bool pressMoved = false; // the press travelled: a drag, not a click
    bool dragging = false;
    Item dragItem;
    float dragWidth = 0.0f;
    int dragFrom = 0;
    int dragIns = 0;
    double gapPos = 0.0;     // eased position of the empty slot

    std::wstring typed;      // type-to-find buffer
    double typedTime = 0.0;

    std::vector<Level> stack;
    double rot = 0.0;        // fractional index of the item sitting at 3:00
    double target = 0.0;     // detent the wheel eases toward when not tilting
    int wheelAccum = 0;
    bool tiltHeld = false;   // wheel/keys took over; tilt waits for movement
    POINT holdPos = {};

    double openTime = 0.0;
    double lastTick = 0.0;
    double lastInput = 0.0;
    POINT lastCursor = {};

    bool closing = false;
    bool closeCancelable = false;
    double closeStart = 0.0;
    double closeDur = 0.0;
    float opacity = 0.0f;

    // Offscreen surface for UpdateLayeredWindow.
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBmp = nullptr;
    int dibSize = 0;

    // Direct2D / DirectWrite.
    ID2D1Factory* d2d = nullptr;
    IDWriteFactory* dw = nullptr;
    ID2D1DCRenderTarget* rt = nullptr;
    ID2D1SolidColorBrush* brush = nullptr;
    ID2D1Bitmap* bg = nullptr;
    int bgSize = 0;
    IDWriteTextFormat* fmt = nullptr;
    IDWriteTextFormat* fmtRight = nullptr;
    float fmtScale = 0.0f;
};

static Wheel g;

// Shared with the mouse hook's thread (see "Input"). g_hookTarget is the
// wheel window while a wheel is taking input, null otherwise.
static std::atomic<HWND> g_hookTarget{nullptr};
static std::atomic<int> g_snapCx{0}, g_snapCy{0}, g_snapSide{1};
static std::atomic<int> g_snapOuter{0}, g_snapHub{0};

// Mouse events the hook thread posts to the wheel window.
enum : UINT {
    WM_WHEEL_LDOWN = WM_APP + 1,  // wParam, lParam: screen x, y
    WM_WHEEL_LUP,
    WM_WHEEL_RUP,
    WM_WHEEL_SCROLL,              // lParam: wheel delta
    WM_WHEEL_OUTSIDE,             // a click or scroll outside the wheel
};

static std::vector<Item> g_menu;  // the open menu's top level
static HINSTANCE g_inst = nullptr;
static bool g_classRegistered = false;

// One wheel at a time, process-wide. All wheel state is only touched by the
// thread that holds g_busy.
static std::atomic<bool> g_busy{false};
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_cfgDirty{true};
static std::atomic<bool> g_dropOverrides{false};

static const double kPi = 3.14159265358979323846;

template <class T>
static void SafeRelease(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}

static double Now() {
    static LARGE_INTEGER freq = {};
    if (!freq.QuadPart) QueryPerformanceFrequency(&freq);
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return double(t.QuadPart) / double(freq.QuadPart);
}

static int Mod(long long a, int n) {
    if (n <= 0) return 0;
    long long m = a % n;
    return int(m < 0 ? m + n : m);
}

static D2D1_COLOR_F Color(uint32_t rgb, float a = 1.0f) {
    D2D1_COLOR_F c;
    c.r = ((rgb >> 16) & 0xFF) / 255.0f;
    c.g = ((rgb >> 8) & 0xFF) / 255.0f;
    c.b = (rgb & 0xFF) / 255.0f;
    c.a = a;
    return c;
}

static D2D1_RECT_F RectF(float l, float t, float r, float b) {
    D2D1_RECT_F rc;
    rc.left = l;
    rc.top = t;
    rc.right = r;
    rc.bottom = b;
    return rc;
}

// Rotation by `deg` (clockwise on screen) about (cx, cy).
static D2D1_MATRIX_3X2_F RotationAbout(float deg, float cx, float cy) {
    const float r = float(deg * kPi / 180.0);
    const float c = std::cos(r), s = std::sin(r);
    D2D1_MATRIX_3X2_F m;
    m._11 = c;
    m._12 = s;
    m._21 = -s;
    m._22 = c;
    m._31 = cx - cx * c + cy * s;
    m._32 = cy - cx * s - cy * c;
    return m;
}

static D2D1_MATRIX_3X2_F Identity() {
    D2D1_MATRIX_3X2_F m;
    m._11 = 1; m._12 = 0;
    m._21 = 0; m._22 = 1;
    m._31 = 0; m._32 = 0;
    return m;
}

// Wheel alpha at a clock angle (degrees clockwise from 12:00).
static float Profile(float deg) {
    const int n = int(sizeof(cfg.stops) / sizeof(cfg.stops[0]));
    if (deg <= cfg.stops[0].deg || deg >= cfg.stops[n - 1].deg) return 0.0f;
    for (int i = 0; i + 1 < n; i++) {
        const AlphaStop& a = cfg.stops[i];
        const AlphaStop& b = cfg.stops[i + 1];
        if (deg <= b.deg) {
            const float t = (deg - a.deg) / (b.deg - a.deg);
            return a.alpha + (b.alpha - a.alpha) * t;
        }
    }
    return 0.0f;
}

static int ItemCount() { return int(g.stack.back().items.size()); }

// Whether level `d` is drawn, given how submenus are shown.
static bool RingVisible(size_t d) {
    const size_t n = g.stack.size();
    if (cfg.menuMode == 1) return true;
    if (cfg.menuMode == 2) return d + 2 >= n;  // the deepest and its parent
    return d + 1 == n;
}

// Degrees between items on the active level.
static float TopStep() {
    const float st = g.stack.empty() ? 0.0f : g.stack.back().step;
    return st > 0.0f ? st : cfg.stepDeg;
}

static bool Wrapping() {
    // Wrap only when the list is long enough that no item could show twice
    // in the visible half.
    return cfg.wrap && !g.dragging && ItemCount() * TopStep() >= 180.0f + TopStep();
}

static bool InsideWheel(POINT pt) {
    const double dx = double(g.side) * (pt.x - g.center.x);
    const double dy = pt.y - g.center.y;
    const double R = g.outerPx;
    if (dx * dx + dy * dy > R * R) return false;
    // The far half of the circle is empty. Only a hub's width of it counts
    // as the wheel, so a click on something over there dismisses the wheel
    // and reaches what was clicked instead of choosing the highlighted item.
    return dx > -double(cfg.hubRadius * g.scale);
}

// ---------------------------------------------------------------------------
// Graphics resources
// ---------------------------------------------------------------------------

static void DiscardDeviceResources() {
    SafeRelease(g.bg);
    SafeRelease(g.brush);
    SafeRelease(g.rt);
    g.bgSize = 0;
}

static bool EnsureSurface() {
    if (g.dib && g.dibSize == g.size) return true;
    if (!g.memDC) g.memDC = CreateCompatibleDC(nullptr);
    if (!g.memDC) return false;
    if (g.dib) {
        SelectObject(g.memDC, g.oldBmp);
        DeleteObject(g.dib);
        g.dib = nullptr;
    }
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = g.size;
    bi.bmiHeader.biHeight = -g.size;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    g.dib = CreateDIBSection(g.memDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!g.dib) return false;
    g.oldBmp = SelectObject(g.memDC, g.dib);
    g.dibSize = g.size;
    return true;
}

// ---------------------------------------------------------------------------
// Theme
// ---------------------------------------------------------------------------

static bool ReadPersonalizeDword(const wchar_t* key, const wchar_t* name,
                                 DWORD* out) {
    DWORD size = sizeof(*out);
    return RegGetValueW(HKEY_CURRENT_USER, key, name, RRF_RT_REG_DWORD,
                        nullptr, out, &size) == ERROR_SUCCESS;
}

static bool SystemUsesLightTheme() {
    DWORD v = 0;
    return ReadPersonalizeDword(
               L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
               L"Personalize",
               L"AppsUseLightTheme", &v) &&
           v != 0;
}

// The user's Windows accent colour as 0xRRGGBB.
static uint32_t SystemAccentColor() {
    DWORD abgr = 0;
    if (ReadPersonalizeDword(L"Software\\Microsoft\\Windows\\DWM",
                             L"AccentColor", &abgr))
        return ((abgr & 0xFF) << 16) | (abgr & 0xFF00) | ((abgr >> 16) & 0xFF);
    DWORD argb = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&argb, &opaque)))
        return argb & 0xFFFFFF;
    return 0x3D7EFF;
}

// Resolves the theme and outline settings into actual colours.
static void ApplyTheme() {
    const bool light =
        cfg.theme == 1 || (cfg.theme == 2 && SystemUsesLightTheme());
    if (light) {
        cfg.bgColor = 0xF4F5F8;
        cfg.textColor = 0x1B1C20;
        cfg.outlineColor = 0x2B2D33;
        cfg.haloColor = 0xFFFFFF;
    } else {
        cfg.bgColor = 0x1F1F26;
        cfg.textColor = 0xF2F2F2;
        cfg.outlineColor = 0xFFFFFF;
        cfg.haloColor = 0x000000;
    }
    // With accent on, the outline and the selection highlight both take the
    // Windows accent colour; otherwise the highlight is the built-in blue.
    cfg.accentColor = 0x3D7EFF;
    if (cfg.accentOutline) {
        cfg.outlineColor = SystemAccentColor();
        cfg.accentColor = cfg.outlineColor;
    }
    // Colours set explicitly in the mod's settings win over all of that.
    if (cfg.hasTextColor) cfg.textColor = cfg.userText;
    if (cfg.hasLabelColor) cfg.bgColor = cfg.userLabel;
    if (cfg.hasHighlight) cfg.accentColor = cfg.userHighlight;
    // Keep the highlighted label readable on pale accents.
    const float lum = 0.299f * float((cfg.accentColor >> 16) & 0xFF) +
                      0.587f * float((cfg.accentColor >> 8) & 0xFF) +
                      0.114f * float(cfg.accentColor & 0xFF);
    cfg.selTextColor = lum > 160.0f ? 0x101114 : 0xFFFFFF;
    g.bgSize = 0;  // redraw the outlines
}

// The outlines never rotate (only the items do), so they are rendered once
// per layout into a premultiplied bitmap: the hub ring plus one rim per ring
// on show, each fading around the clock by Profile().
static bool EnsureBackground() {
    std::vector<float> radii;
    if (cfg.innerOutline) radii.push_back(cfg.hubRadius * g.scale);
    for (size_t d = 0; d < g.stack.size(); d++)
        if (RingVisible(d)) radii.push_back(g.stack[d].rimPx);
    if (g.bg && g.bgSize == g.size && g.bgSide == g.side &&
        g.bgRadii == radii)
        return true;
    SafeRelease(g.bg);

    const int S = g.size;
    const float c = S * 0.5f;
    const float half = cfg.outlineWidth * g.scale * 0.5f;
    float maxR = 0.0f;
    for (float r : radii) maxR = std::max(maxR, r);
    const float reach2 = (maxR + half + 3.0f) * (maxR + half + 3.0f);
    const float lr = float((cfg.outlineColor >> 16) & 0xFF);
    const float lg = float((cfg.outlineColor >> 8) & 0xFF);
    const float lb = float(cfg.outlineColor & 0xFF);
    const float hr = float((cfg.haloColor >> 16) & 0xFF);
    const float hg = float((cfg.haloColor >> 8) & 0xFF);
    const float hb = float(cfg.haloColor & 0xFF);

    // A thin line with a soft contrasting halo so it reads on any backdrop.
    std::vector<uint32_t> px(size_t(S) * S, 0);
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            const float dx = (x + 0.5f - c) * float(g.side);
            const float dy = y + 0.5f - c;
            if (dx <= 0.0f) continue;  // the far half is fully transparent
            const float r2 = dx * dx + dy * dy;
            if (r2 > reach2) continue;
            const float r = std::sqrt(r2);
            float d = 1e9f;
            for (float ri : radii) d = std::min(d, std::fabs(r - ri));
            if (d > half + 2.5f) continue;
            const float deg = float(std::atan2(dx, -dy) * 180.0 / kPi);
            const float p = Profile(deg);
            if (p <= 0.0f) continue;
            const float line = std::clamp(half + 0.5f - d, 0.0f, 1.0f);
            const float halo = std::clamp(half + 2.0f - d, 0.0f, 1.0f) *
                               cfg.outlineShadow * (1.0f - line);
            const uint32_t A = uint32_t((line + halo) * p * 255.0f + 0.5f);
            if (!A) continue;
            const float kl = line * p, kh = halo * p;  // premultiplied
            px[size_t(y) * S + x] =
                (A << 24) | (uint32_t(lr * kl + hr * kh + 0.5f) << 16) |
                (uint32_t(lg * kl + hg * kh + 0.5f) << 8) |
                uint32_t(lb * kl + hb * kh + 0.5f);
        }
    }

    D2D1_BITMAP_PROPERTIES bp = {};
    bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bp.dpiX = bp.dpiY = 96.0f;
    D2D1_SIZE_U sz;
    sz.width = sz.height = UINT32(S);
    if (FAILED(g.rt->CreateBitmap(sz, px.data(), UINT32(S) * 4, &bp, &g.bg)))
        return false;
    g.bgSize = S;
    g.bgSide = g.side;
    g.bgRadii = radii;
    return true;
}

static IDWriteTextFormat* MakeFormat(DWRITE_TEXT_ALIGNMENT align) {
    IDWriteTextFormat* f = nullptr;
    if (FAILED(g.dw->CreateTextFormat(
            cfg.fontName, nullptr, DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            cfg.fontSize * g.scale, L"", &f)))
        return nullptr;
    f->SetTextAlignment(align);
    f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    IDWriteInlineObject* ellipsis = nullptr;
    if (SUCCEEDED(g.dw->CreateEllipsisTrimmingSign(f, &ellipsis))) {
        DWRITE_TRIMMING trim = {};
        trim.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
        f->SetTrimming(&trim, ellipsis);
        ellipsis->Release();
    }
    return f;
}

static bool EnsureText() {
    if (!g.dw &&
        FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                   __uuidof(IDWriteFactory),
                                   reinterpret_cast<IUnknown**>(&g.dw))))
        return false;

    if (!g.fmt || g.fmtScale != g.scale) {
        SafeRelease(g.fmt);
        SafeRelease(g.fmtRight);
        g.fmt = MakeFormat(DWRITE_TEXT_ALIGNMENT_LEADING);
        g.fmtRight = MakeFormat(DWRITE_TEXT_ALIGNMENT_TRAILING);
        if (!g.fmt || !g.fmtRight) return false;
        g.fmtScale = g.scale;
    }
    return true;
}

static void MeasureLevel(Level& lvl) {
    const float s = g.scale;
    if (lvl.widths.size() == lvl.items.size() && lvl.widthScale == s) return;
    const float span = (cfg.textOuter - cfg.textInner) * s;
    lvl.widths.assign(lvl.items.size(), 0.0f);
    for (size_t i = 0; i < lvl.items.size(); i++) {
        const Item& it = lvl.items[i];
        const float maxW = span - (it.sub.empty() ? 0 : 18 * s) -
                           (it.radio ? 18 * s : 0);
        float w = maxW;
        IDWriteTextLayout* layout = nullptr;
        if (SUCCEEDED(g.dw->CreateTextLayout(
                it.text.c_str(), UINT32(it.text.size()), g.fmt, maxW,
                cfg.slotHeight * s, &layout))) {
            DWRITE_TEXT_METRICS tm = {};
            if (SUCCEEDED(layout->GetMetrics(&tm)))
                w = std::min(maxW, tm.widthIncludingTrailingWhitespace);
            layout->Release();
        }
        lvl.widths[i] = w;
    }
    lvl.widthScale = s;
}

// Lays out the levels: where each one's labels start, how far apart they
// sit, and a rim pulled in to fit the longest label.
//   Replace:   every level sits in the same spot; only the deepest shows.
//   Stack:     each deeper level is a new ring outside its parent.
//   Alternate: two spots only. Levels take turns, inner then outer then
//              inner again, so the deepest level and its parent are on show
//              and the wheel never grows past two rings.
static void LayoutLevel() {
    const float s = g.scale;
    g.outerPx = g.tiltR = cfg.outerRadius * s;
    if (g.stack.empty()) return;
    const bool canMeasure = EnsureText();
    const float base = cfg.textInner * s;
    const float span = (cfg.textOuter - cfg.textInner) * s;
    const float gap = cfg.ringGap * s;
    const size_t top = g.stack.size() - 1;

    // Places one level with its labels starting at `inner`; returns its rim.
    auto place = [&](Level& lvl, float inner) {
        lvl.innerPx = inner;
        // Outer rings use a finer angle so neighbouring labels stay the
        // same distance apart instead of fanning out.
        lvl.step = inner > base ? std::max(cfg.minStepDeg,
                                           cfg.stepDeg * base / inner)
                                : cfg.stepDeg;
        float longest = span;
        if (canMeasure) {
            MeasureLevel(lvl);
            longest = 0.0f;
            for (size_t i = 0; i < lvl.items.size(); i++)
                longest = std::max(
                    longest, lvl.widths[i] +
                                 (lvl.items[i].sub.empty() ? 0 : 18 * s) +
                                 (lvl.items[i].radio ? 18 * s : 0));
        }
        const float cap = inner + (cfg.outerRadius - cfg.textInner) * s;
        lvl.rimPx = std::min(
            cap, std::ceil(inner + longest + (cfg.pillPad + cfg.rimGap) * s));
        return lvl.rimPx;
    };

    // tiltR is always the innermost ring on show, so the dead band and tilt
    // feel the same however many rings sit outside it.
    if (cfg.menuMode == 1) {
        float rim = 0.0f;
        for (size_t d = 0; d <= top; d++)
            rim = place(g.stack[d], d == 0 ? base : rim + gap);
        g.outerPx = rim;
        g.tiltR = g.stack.front().rimPx;
    } else if (cfg.menuMode == 2 && top >= 1) {
        for (size_t d = 0; d + 1 < top; d++) place(g.stack[d], base);  // hidden
        const bool topInner = top % 2 == 0;
        Level& in = g.stack[topInner ? top : top - 1];
        Level& out = g.stack[topInner ? top - 1 : top];
        g.tiltR = place(in, base);
        g.outerPx = place(out, g.tiltR + gap);
    } else {
        for (size_t d = 0; d <= top; d++) place(g.stack[d], base);
        g.outerPx = g.tiltR = g.stack.back().rimPx;
    }

    // Window: the rings plus a margin for a label dragged past the rim.
    const float need = std::max(cfg.outerRadius * s, g.outerPx) + 120 * s;
    g.size = int(std::ceil(need)) * 2;
}

static bool EnsureResources() {
    if (!g.d2d &&
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
                                 __uuidof(ID2D1Factory), nullptr,
                                 reinterpret_cast<void**>(&g.d2d))))
        return false;
    if (!EnsureText()) return false;

    if (!g.rt) {
        D2D1_RENDER_TARGET_PROPERTIES props = {};
        props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
        props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        props.dpiX = props.dpiY = 96.0f;  // draw in raw pixels
        props.usage = D2D1_RENDER_TARGET_USAGE_NONE;
        props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
        if (FAILED(g.d2d->CreateDCRenderTarget(&props, &g.rt))) return false;
        // ClearType cannot blend onto a transparent surface.
        g.rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    }
    if (!g.brush) {
        const D2D1_COLOR_F white = Color(0xFFFFFF);
        if (FAILED(g.rt->CreateSolidColorBrush(&white, nullptr, &g.brush)))
            return false;
    }
    return EnsureSurface() && EnsureBackground();
}

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------

// While an item is being dragged there is one extra (empty) slot in the list.
static int HighlightIndexAt(POINT pt);

static int SlotCount() { return ItemCount() + (g.dragging ? 1 : 0); }

// Radial span [a, b] out from the hub, as an unrotated rect on the active
// side (right of centre normally, left of centre when mirrored).
static D2D1_RECT_F SpanRect(float c, float a, float b, float halfH) {
    return g.side > 0 ? RectF(c + a, c - halfH, c + b, c + halfH)
                      : RectF(c - b, c - halfH, c - a, c + halfH);
}

static void Render() {
    bool stale = g.stack.back().rimPx <= 0.0f;
    for (const Level& l : g.stack)
        if (l.widths.size() != l.items.size()) stale = true;
    if (stale) LayoutLevel();
    if (!EnsureResources()) return;

    const float c = g.size * 0.5f;
    const float s = g.scale;
    const float side = float(g.side);
    const float span = (cfg.textOuter - cfg.textInner) * s;  // label length cap
    const float halfH = cfg.slotHeight * s * 0.5f;
    const float pillHalf = cfg.pillHeight * s * 0.5f;
    const float pad = cfg.pillPad * s;
    const float chev = 18 * s;
    const D2D1_MATRIX_3X2_F identity = Identity();

    RECT rc = {0, 0, g.size, g.size};
    g.rt->BindDC(g.memDC, &rc);
    g.rt->BeginDraw();
    const D2D1_COLOR_F clear = Color(0, 0.0f);
    g.rt->Clear(&clear);
    g.rt->SetTransform(&identity);

    const D2D1_RECT_F full = RectF(0, 0, float(g.size), float(g.size));
    g.rt->DrawBitmap(g.bg, &full, 1.0f,
                     D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, nullptr);

    if (cfg.guides) {
        const D2D1_COLOR_F col = Color(0xFFCC33);
        g.brush->SetColor(&col);
        g.brush->SetOpacity(0.7f);
        // The band's two edges, on the line the wheel was opened from.
        const float band =
            g.tiltR * float(std::sin(cfg.deadBandDeg * kPi / 180.0));
        const float mid = c + float(g.tiltY - g.center.y);
        for (int sign = -1; sign <= 1; sign += 2) {
            D2D1_POINT_2F a, b;
            a.x = c - g.tiltR; a.y = mid + float(sign) * band;
            b.x = c + g.tiltR; b.y = a.y;
            g.rt->DrawLine(a, b, g.brush, 1.0f, nullptr);
        }
    }

    // Mirrored, labels are right-aligned so they still start at the hub.
    IDWriteTextFormat* labelFmt = g.side > 0 ? g.fmt : g.fmtRight;
    IDWriteTextFormat* chevFmt = g.side > 0 ? g.fmtRight : g.fmt;
    const wchar_t* chevGlyph = g.side > 0 ? L"\x203A" : L"\x2039";

    // Draws one ring of items. `rot` is the list position sitting on the
    // 3:00 (or 9:00) line; `dim` fades the rings behind the active one.
    auto drawLevel = [&](const Level& lvl, double rot, bool top, float dim) {
        const std::vector<Item>& items = lvl.items;
        const std::vector<float>& widths = lvl.widths;
        const int n = int(items.size());
        const float r0 = lvl.innerPx, r1 = r0 + span, R = lvl.rimPx;
        const float step = lvl.step;
        const bool dragging = top && g.dragging;
        const int hi = (top && !dragging) ? HighlightIndexAt(g.lastCursor) : -1;

        auto drawSlot = [&](int idx, double pos, bool selected, bool seam) {
            const Item& it = items[size_t(idx)];
            const float phi = float(pos - rot) * step;
            const float theta = 90.0f + phi;
            if (theta <= -step || theta >= 180.0f + step) return;

            // Separator tick hanging inward off the rim, before this item.
            const bool sep = seam || (lvl.hoisted > 0 && idx == lvl.hoisted) ||
                             (it.groupStart && !it.hoisted && idx != 0);
            if (sep) {
                const float td = theta - step * 0.5f;
                const float ta = Profile(td) * dim;
                if (ta > 0.01f) {
                    const D2D1_MATRIX_3X2_F m =
                        RotationAbout(side * (td - 90.0f), c, c);
                    g.rt->SetTransform(&m);
                    const D2D1_COLOR_F col = Color(cfg.outlineColor);
                    g.brush->SetColor(&col);
                    g.brush->SetOpacity(ta);
                    D2D1_POINT_2F a, b;
                    a.x = c + side * (R - (seam ? 60 : 16) * s); a.y = c;
                    b.x = c + side * R; b.y = c;
                    g.rt->DrawLine(a, b, g.brush, cfg.outlineWidth * s,
                                   nullptr);
                }
            }

            const float a = Profile(theta) * dim;
            if (a <= 0.01f) return;
            const D2D1_MATRIX_3X2_F m = RotationAbout(side * phi, c, c);
            g.rt->SetTransform(&m);
            const bool hasSub = !it.sub.empty();
            const float w = widths[size_t(idx)];
            const float chk = it.radio ? chev : 0;  // check-mark column

            // Label background: theme colour, accent for the selected item.
            D2D1_ROUNDED_RECT rr;
            rr.rect = SpanRect(c, r0 - pad,
                               r0 + chk + w + (hasSub ? chev : 0) + pad,
                               pillHalf);
            rr.radiusX = rr.radiusY = 5 * s;
            const D2D1_COLOR_F bgc =
                Color(selected ? cfg.accentColor : cfg.bgColor);
            g.brush->SetColor(&bgc);
            g.brush->SetOpacity(a * (selected ? 1.0f : cfg.labelBgOpacity));
            g.rt->FillRoundedRectangle(&rr, g.brush);

            const D2D1_COLOR_F col =
                Color(selected ? cfg.selTextColor : cfg.textColor);
            g.brush->SetColor(&col);
            g.brush->SetOpacity(a * (it.disabled ? 0.4f : 1.0f));
            if (it.checked) {
                const D2D1_RECT_F kr = SpanRect(c, r0, r0 + chk, halfH);
                g.rt->DrawText(L"\x2713", 1, labelFmt, &kr, g.brush,
                               D2D1_DRAW_TEXT_OPTIONS_NONE,
                               DWRITE_MEASURING_MODE_NATURAL);
            }
            const D2D1_RECT_F tr =
                SpanRect(c, r0 + chk, r1 - (hasSub ? chev : 0), halfH);
            g.rt->DrawText(it.text.c_str(), UINT32(it.text.size()), labelFmt,
                           &tr, g.brush, D2D1_DRAW_TEXT_OPTIONS_CLIP,
                           DWRITE_MEASURING_MODE_NATURAL);
            if (hasSub) {
                const D2D1_RECT_F cr =
                    SpanRect(c, r0 + chk + w, r0 + chk + w + chev, halfH);
                g.rt->DrawText(chevGlyph, 1, chevFmt, &cr, g.brush,
                               D2D1_DRAW_TEXT_OPTIONS_NONE,
                               DWRITE_MEASURING_MODE_NATURAL);
            }
        };

        if (top && Wrapping()) {
            const double reach = 90.0 / step;
            const long long kMin = (long long)std::floor(rot - reach) - 1;
            const long long kMax = (long long)std::ceil(rot + reach) + 1;
            for (long long k = kMin; k <= kMax; k++) {
                const int idx = Mod(k, n);
                drawSlot(idx, double(k), idx == hi, idx == 0);
            }
        } else {
            for (int i = 0; i < n; i++) {
                double pos = double(i);
                // Items at or past the gap slide down a slot to make room.
                if (dragging)
                    pos += std::clamp(double(i) - g.gapPos + 1.0, 0.0, 1.0);
                drawSlot(i, pos,
                         top ? i == hi
                             : std::fabs(pos - rot) < 0.5,
                         false);
            }
        }

        if (dragging) {
            // Where the carried item will land: an outlined empty slot.
            const float dw = g.dragWidth + (g.dragItem.sub.empty() ? 0 : chev);
            const float phi = float(g.gapPos - rot) * step;
            const float a = Profile(90.0f + phi);
            if (a > 0.01f) {
                const D2D1_MATRIX_3X2_F m = RotationAbout(side * phi, c, c);
                g.rt->SetTransform(&m);
                D2D1_ROUNDED_RECT rr;
                rr.rect = SpanRect(c, r0 - pad, r0 + dw + pad, pillHalf);
                rr.radiusX = rr.radiusY = 5 * s;
                const D2D1_COLOR_F accent = Color(cfg.accentColor);
                g.brush->SetColor(&accent);
                g.brush->SetOpacity(std::max(a, 0.5f));
                g.rt->DrawRoundedRectangle(&rr, g.brush, 1.5f * s, nullptr);
            }
        }
    };

    // The deepest level is the active ring. Any ancestors on show are drawn
    // dimmed and frozen where you left them.
    const size_t topIdx = g.stack.size() - 1;
    for (size_t d = 0; d < topIdx; d++)
        if (RingVisible(d))
            drawLevel(g.stack[d], g.stack[d + 1].savedRot, false,
                      cfg.parentDim);
    drawLevel(g.stack[topIdx], g.rot, true, 1.0f);

    if (g.dragging) {
        // The carried item rides on the cursor.
        g.rt->SetTransform(&identity);
        const float px = float(g.lastCursor.x - g.center.x) + c;
        const float py = float(g.lastCursor.y - g.center.y) + c;
        D2D1_ROUNDED_RECT rr;
        rr.rect = RectF(px - g.dragWidth * 0.5f - pad, py - pillHalf,
                        px + g.dragWidth * 0.5f + pad, py + pillHalf);
        rr.radiusX = rr.radiusY = 5 * s;
        const D2D1_COLOR_F accent = Color(cfg.accentColor);
        g.brush->SetColor(&accent);
        g.brush->SetOpacity(0.95f);
        g.rt->FillRoundedRectangle(&rr, g.brush);
        const D2D1_COLOR_F col = Color(cfg.selTextColor);
        g.brush->SetColor(&col);
        g.brush->SetOpacity(1.0f);
        const D2D1_RECT_F tr =
            RectF(px - g.dragWidth * 0.5f, py - halfH,
                  px + g.dragWidth * 0.5f + 4 * s, py + halfH);
        g.rt->DrawText(g.dragItem.text.c_str(),
                       UINT32(g.dragItem.text.size()), g.fmt, &tr, g.brush,
                       D2D1_DRAW_TEXT_OPTIONS_NONE,
                       DWRITE_MEASURING_MODE_NATURAL);
    }

    // What has been typed so far, shown just above the cursor.
    if (!g.typed.empty()) {
        g.rt->SetTransform(&identity);
        float tw = 40 * s;
        IDWriteTextLayout* layout = nullptr;
        if (SUCCEEDED(g.dw->CreateTextLayout(
                g.typed.c_str(), UINT32(g.typed.size()), g.fmt, 400 * s,
                halfH * 2, &layout))) {
            DWRITE_TEXT_METRICS tm = {};
            if (SUCCEEDED(layout->GetMetrics(&tm)))
                tw = tm.widthIncludingTrailingWhitespace;
            layout->Release();
        }
        const float ty = c - 40 * s;
        D2D1_ROUNDED_RECT rr;
        rr.rect = RectF(c - tw * 0.5f - pad, ty - pillHalf,
                        c + tw * 0.5f + pad, ty + pillHalf);
        rr.radiusX = rr.radiusY = 5 * s;
        const D2D1_COLOR_F bgc = Color(cfg.bgColor);
        g.brush->SetColor(&bgc);
        g.brush->SetOpacity(0.95f);
        g.rt->FillRoundedRectangle(&rr, g.brush);
        const D2D1_COLOR_F ac = Color(cfg.accentColor);
        g.brush->SetColor(&ac);
        g.rt->DrawRoundedRectangle(&rr, g.brush, 1.5f * s, nullptr);
        const D2D1_COLOR_F col = Color(cfg.textColor);
        g.brush->SetColor(&col);
        g.brush->SetOpacity(1.0f);
        const D2D1_RECT_F tr = RectF(c - tw * 0.5f, ty - halfH,
                                     c + tw * 0.5f + 4 * s, ty + halfH);
        g.rt->DrawText(g.typed.c_str(), UINT32(g.typed.size()), g.fmt, &tr,
                       g.brush, D2D1_DRAW_TEXT_OPTIONS_NONE,
                       DWRITE_MEASURING_MODE_NATURAL);
    }

    g.rt->SetTransform(&identity);
    const HRESULT hr = g.rt->EndDraw(nullptr, nullptr);
    if (hr == HRESULT(D2DERR_RECREATE_TARGET)) {
        DiscardDeviceResources();
        g.forceRender = true;  // this frame was lost; draw it again
        return;
    }

    POINT dst = {g.center.x - g.size / 2, g.center.y - g.size / 2};
    SIZE sz = {g.size, g.size};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha =
        BYTE(std::clamp(g.opacity, 0.0f, 1.0f) * 255.0f + 0.5f);
    bf.AlphaFormat = AC_SRC_ALPHA;
    UpdateLayeredWindow(g.hwnd, nullptr, &dst, &sz, g.memDC, &src, 0, &bf,
                        ULW_ALPHA);
}

// ---------------------------------------------------------------------------
// Memory: recent choices, pre-selection, saved order
// ---------------------------------------------------------------------------
//
// Real menus have no stable IDs we can rely on, so everything here goes by
// the item's text.

static std::vector<std::wstring> g_history;  // top-level labels, newest first
static std::wstring g_lastText;              // the last item actually chosen
static bool g_historyLoaded = false;
static DWORD g_clipSeq = 0;                  // clipboard sequence at that time

static std::wstring ToLower(std::wstring s) {
    for (wchar_t& ch : s) ch = wchar_t(std::towlower(ch));
    return s;
}

static std::vector<std::wstring> Split(const std::wstring& s, wchar_t sep) {
    std::vector<std::wstring> out;
    size_t a = 0;
    while (a <= s.size()) {
        size_t b = s.find(sep, a);
        if (b == std::wstring::npos) b = s.size();
        size_t i = a, j = b;
        while (i < j && s[i] == L' ') i++;
        while (j > i && s[j - 1] == L' ') j--;
        if (j > i) out.push_back(s.substr(i, j - i));
        a = b + 1;
    }
    return out;
}

static std::wstring Join(const std::vector<std::wstring>& v, wchar_t sep) {
    std::wstring out;
    for (size_t i = 0; i < v.size(); i++) {
        if (i) out += sep;
        out += v[i];
    }
    return out;
}

// `names` is already lower-case.
static bool NameIn(const std::vector<std::wstring>& names,
                   const std::wstring& text) {
    const std::wstring t = ToLower(text);
    for (const std::wstring& n : names)
        if (n == t) return true;
    return false;
}

static std::wstring ReadStored(PCWSTR key) {
    std::vector<wchar_t> buf(16384);
    const size_t n = Wh_GetStringValue(key, buf.data(), buf.size());
    return std::wstring(buf.data(), std::min(n, buf.size() - 1));
}

static void LoadHistory() {
    if (g_historyLoaded) return;
    g_historyLoaded = true;
    g_history = Split(ReadStored(L"history"), L'\t');
    g_lastText = ReadStored(L"last");
    g_clipSeq = GetClipboardSequenceNumber();
}

static Item* FindText(std::vector<Item>& v, const std::wstring& text) {
    for (Item& it : v)
        if (it.text == text) return &it;
    return nullptr;
}

// The model's copy of a displayed item.
static Item* FindSame(std::vector<Item>& v, const Item& like) {
    for (Item& it : v)
        if (it.id == like.id && it.cmd == like.cmd &&
            it.hOwner == like.hOwner && it.pos == like.pos &&
            it.text == like.text)
            return &it;
    return nullptr;
}

// Stored lists are tab-separated: labels never contain a tab (CleanMenuText
// cuts them at the first one), and unlike a line break a tab survives every
// storage backend.

// Remembers a choice by the top-level entry it was reached through.
static void RecordChoice(const std::wstring& rootText,
                         const std::wstring& leafText) {
    g_lastText = leafText;
    g_history.erase(std::remove(g_history.begin(), g_history.end(), rootText),
                    g_history.end());
    g_history.insert(g_history.begin(), rootText);
    if (g_history.size() > 8) g_history.resize(8);
    g_clipSeq = GetClipboardSequenceNumber();
    Wh_SetStringValue(L"history", Join(g_history, L'\t').c_str());
    Wh_SetStringValue(L"last", g_lastText.c_str());
}

static int PreselectIndex(const std::vector<Item>& items) {
    if (cfg.preselect == 0) return 0;
    if (cfg.preselect == 2) {
        // Paste follows a Cut or Copy, whether it was chosen here or done
        // some other way (Ctrl+C, a toolbar button) since our last choice.
        if (NameIn(cfg.copyNames, g_lastText) ||
            GetClipboardSequenceNumber() != g_clipSeq)
            for (size_t i = 0; i < items.size(); i++)
                if (!items[i].disabled && NameIn(cfg.pasteNames, items[i].text))
                    return int(i);
    }
    if (g_history.empty()) return 0;
    // Never park on something destructive.
    if (NameIn(cfg.neverPreselect, g_lastText)) return 0;
    for (size_t i = 0; i < items.size(); i++)
        if (items[i].text == g_history[0] && !items[i].disabled) return int(i);
    return 0;
}

// Top level as displayed: optionally the most recent choices first, then the
// rest in the user's own order.
static Level BuildRootLevel() {
    Level lvl;
    lvl.src = &g_menu;
    const size_t want = size_t(std::max(0, cfg.mruCount));
    for (size_t h = 0; h < g_history.size() && h < want; h++) {
        if (const Item* it = FindText(g_menu, g_history[h])) {
            lvl.items.push_back(*it);
            lvl.items.back().hoisted = true;
        }
    }
    lvl.hoisted = int(lvl.items.size());
    for (const Item& it : g_menu) {
        bool taken = false;
        for (int i = 0; i < lvl.hoisted && !taken; i++)
            taken = lvl.items[size_t(i)].text == it.text;
        if (!taken) lvl.items.push_back(it);
    }
    return lvl;
}

// A rearranged menu is remembered under a key made from the set of names in
// it, so each distinct menu (files, folders, the desktop...) keeps its own
// order, and a menu with different entries starts fresh.
static std::wstring OrderKey(const std::vector<Item>& items) {
    std::vector<std::wstring> names;
    for (const Item& it : items)
        if (it.id != ID_BACK) names.push_back(it.text);
    std::sort(names.begin(), names.end());
    uint64_t h = 1469598103934665603ull;  // FNV-1a
    for (const std::wstring& n : names) {
        for (wchar_t ch : n) {
            h ^= uint64_t(ch);
            h *= 1099511628211ull;
        }
        h ^= 0x0A;
        h *= 1099511628211ull;
    }
    wchar_t key[40];
    swprintf(key, 40, L"order_%08x%08x", unsigned(h >> 32),
             unsigned(h & 0xFFFFFFFFu));
    return key;
}

static void SaveOrder(const std::vector<Item>& items) {
    std::vector<std::wstring> names;
    for (const Item& it : items)
        if (it.id != ID_BACK) names.push_back(it.text);
    Wh_SetStringValue(OrderKey(items).c_str(), Join(names, L'\t').c_str());
}

static void ApplyStoredOrder(std::vector<Item>& items) {
    const std::vector<std::wstring> order =
        Split(ReadStored(OrderKey(items).c_str()), L'\t');
    if (order.empty()) return;
    std::vector<Item> out;
    std::vector<bool> used(items.size(), false);
    for (const std::wstring& name : order) {
        for (size_t i = 0; i < items.size(); i++) {
            if (!used[i] && items[i].text == name) {
                out.push_back(items[i]);
                used[i] = true;
                break;
            }
        }
    }
    for (size_t i = 0; i < items.size(); i++)
        if (!used[i]) out.push_back(items[i]);
    items.swap(out);
}

// ---------------------------------------------------------------------------
// Options, edited from inside the wheel
// ---------------------------------------------------------------------------
//
// "Wheel options" is an ordinary entry whose submenu lists the settings; each
// setting opens a group of exclusive choices with a check mark on the current
// one. Choosing one applies it on the spot and leaves the wheel open.

struct Choice {
    const wchar_t* label;
    float value;
};

struct Setting {
    const wchar_t* name;
    float (*get)();
    void (*set)(float);
    std::vector<Choice> choices;
};

static const std::vector<Setting>& Settings() {
    static const std::vector<Setting> v = {
        {L"Opens toward",
         [] { return cfg.preferLeft ? 1.0f : 0.0f; },
         [](float x) { cfg.preferLeft = x > 0.5f; },
         {{L"Right", 0}, {L"Left", 1}}},
        {L"Opens on",
         [] { return float(cfg.preselect); },
         [](float x) { cfg.preselect = int(x); },
         {{L"First item", 0}, {L"Last choice", 1}, {L"Last, or Paste", 2}}},
        {L"Recent on top",
         [] { return float(cfg.mruCount); },
         [](float x) { cfg.mruCount = int(x); },
         {{L"Off", 0}, {L"Last 3", 3}, {L"Last 5", 5}}},
        {L"Tilt to spin",
         [] { return cfg.tilt ? 1.0f : 0.0f; },
         [](float x) { cfg.tilt = x > 0.5f; },
         {{L"On", 1}, {L"Off", 0}}},
        {L"Tilt speed",
         [] { return cfg.maxSpeed; },
         [](float x) { cfg.maxSpeed = x; },
         {{L"Slow", 10}, {L"Normal", 18}, {L"Fast", 26}, {L"Very fast", 36}}},
        {L"Tilt response",
         [] { return cfg.expo; },
         [](float x) { cfg.expo = x; },
         {{L"Linear", 1.0f}, {L"Gentle", 1.4f}, {L"Curved", 2.0f},
          {L"Steep", 3.0f}}},
        {L"Dead band",
         [] { return cfg.deadBandDeg; },
         [](float x) { cfg.deadBandDeg = x; },
         {{L"Narrow", 5.0f}, {L"Normal", 7.5f}, {L"Wide", 11.0f}}},
        {L"Item spacing",
         [] { return cfg.stepDeg; },
         [](float x) { cfg.stepDeg = x; },
         {{L"Tight", 10}, {L"Normal", 12}, {L"Loose", 15}}},
        {L"Wrap long lists",
         [] { return cfg.wrap ? 1.0f : 0.0f; },
         [](float x) { cfg.wrap = x > 0.5f; },
         {{L"Off", 0}, {L"On", 1}}},
        {L"Guide lines",
         [] { return cfg.guides ? 1.0f : 0.0f; },
         [](float x) { cfg.guides = x > 0.5f; },
         {{L"Off", 0}, {L"On", 1}}},
        {L"Fade out after",
         [] { return cfg.idleTimeoutSec; },
         [](float x) { cfg.idleTimeoutSec = x; },
         {{L"10 seconds", 10}, {L"30 seconds", 30}, {L"1 minute", 60},
          {L"2 minutes", 120}}},
        {L"Theme",
         [] { return float(cfg.theme); },
         [](float x) { cfg.theme = int(x); },
         {{L"Dark", 0}, {L"Light", 1}, {L"Match Windows", 2}}},
        {L"Outline",
         [] { return cfg.accentOutline ? 1.0f : 0.0f; },
         [](float x) { cfg.accentOutline = x > 0.5f; },
         {{L"Plain", 0}, {L"Windows accent", 1}}},
        {L"Submenus",
         [] { return float(cfg.menuMode); },
         [](float x) { cfg.menuMode = int(x); },
         {{L"Replace the wheel", 0}, {L"Stack outward", 1},
          {L"Alternate", 2}}},
        {L"Empty click",
         [] { return float(cfg.emptyClick); },
         [](float x) { cfg.emptyClick = int(x); },
         {{L"Chooses", 0}, {L"Does nothing", 1}, {L"Closes the wheel", 2}}},
    };
    return v;
}

// Storage key for a choice made in "Wheel options".
static std::wstring OverrideKey(const Setting& st) {
    std::wstring key = L"opt_";
    for (const wchar_t* p = st.name; *p; p++)
        if (std::iswalnum(*p)) key += *p;
    return key;
}

// The whole options subtree, reflecting cfg as it stands right now.
static Item BuildOptionsItem() {
    Item root;
    root.text = L"Wheel options";
    root.id = ID_OPTIONS;
    root.groupStart = true;
    const std::vector<Setting>& all = Settings();
    for (size_t k = 0; k < all.size(); k++) {
        const Setting& st = all[k];
        const float cur = st.get();
        Item entry;
        entry.id = ID_OPT_SETTING;
        std::wstring current = L"custom";
        for (size_t c = 0; c < st.choices.size(); c++) {
            Item ch;
            ch.text = st.choices[c].label;
            ch.id = ID_OPT_BASE + int(k) * 32 + int(c);
            ch.radio = true;
            ch.checked = std::fabs(st.choices[c].value - cur) < 0.001f;
            if (ch.checked) current = ch.text;
            entry.sub.push_back(std::move(ch));
        }
        entry.text = std::wstring(st.name) + L": " + current;
        root.sub.push_back(std::move(entry));
    }
    return root;
}

// Rebuilds the options entry in the menu model so it matches cfg.
static Item* RefreshOptionsItem() {
    for (Item& it : g_menu) {
        if (it.id == ID_OPTIONS) {
            it = BuildOptionsItem();
            return &it;
        }
    }
    return nullptr;
}

// Applies one radio choice, then refreshes the check marks and the value
// shown on the parent entry without leaving the menu.
static void ApplyOption(int id) {
    const std::vector<Setting>& all = Settings();
    const size_t k = size_t((id - ID_OPT_BASE) / 32);
    const size_t c = size_t((id - ID_OPT_BASE) % 32);
    if (k >= all.size() || c >= all[k].choices.size()) return;
    all[k].set(all[k].choices[c].value);
    if (k == 0) g.side = cfg.preferLeft ? -1 : 1;  // show a side change now
    g.forceRender = true;
    ApplyTheme();

    Item* opt = RefreshOptionsItem();
    if (!opt) return;

    // Stack while choosing: [top level, settings, choices].
    if (g.stack.size() == 3 && k < opt->sub.size()) {
        const Item back = MI(L"\x2039 Back", ID_BACK);
        Level& settings = g.stack[1];
        settings.src = &opt->sub;
        settings.items.assign(1, back);
        settings.items.insert(settings.items.end(), opt->sub.begin(),
                              opt->sub.end());
        settings.widths.clear();
        Level& choices = g.stack[2];
        choices.src = &opt->sub[k].sub;
        choices.items.assign(1, back);
        choices.items.insert(choices.items.end(), opt->sub[k].sub.begin(),
                             opt->sub[k].sub.end());
        choices.widths.clear();
        LayoutLevel();
    }
    // Remember it; see LoadConfig for how this meets the mod's settings.
    Wh_SetIntValue(OverrideKey(all[k]).c_str(),
                   int(std::lround(all[k].choices[c].value * 100.0f)));
}

// ---------------------------------------------------------------------------
// Menu model, read from the real HMENU
// ---------------------------------------------------------------------------

// "&Open\tCtrl+O" -> "Open". Also drops the "(&O)" suffix some languages use.
static std::wstring CleanMenuText(const wchar_t* raw) {
    std::wstring out;
    for (const wchar_t* p = raw; *p && *p != L'\t'; p++) {
        if (p[0] == L'(' && p[1] == L'&' && p[2] && p[3] == L')') {
            p += 3;
            continue;
        }
        if (*p == L'&') {
            if (p[1] == L'&') {
                out += L'&';
                p++;
            }
            continue;
        }
        out += *p;
    }
    while (!out.empty() && out.back() == L' ') out.pop_back();
    return out;
}

// Case-insensitive match where * stands for any run of characters.
static bool WildMatch(const wchar_t* pat, const wchar_t* str) {
    while (*pat) {
        if (*pat == L'*') {
            while (*pat == L'*') pat++;
            if (!*pat) return true;
            for (; *str; str++)
                if (WildMatch(pat, str)) return true;
            return false;
        }
        if (!*str || std::towlower(*pat) != std::towlower(*str)) return false;
        pat++;
        str++;
    }
    return !*str;
}

static bool IsHidden(const std::wstring& text) {
    for (const std::wstring& pat : cfg.hideItems)
        if (WildMatch(pat.c_str(), text.c_str())) return true;
    return false;
}

// Set by ReadMenu when an item had no readable text.
static bool g_sawUnreadable = false;

// One level of a menu as wheel items. Submenus are not read here: their
// owners often fill them only when they are about to open, so they are
// marked lazy and read on the way in (LoadSubmenu).
static std::vector<Item> ReadMenu(HMENU hMenu) {
    std::vector<Item> out;
    const int n = GetMenuItemCount(hMenu);
    bool sepPending = false, anyCheck = false;
    for (int pos = 0; pos < n; pos++) {
        wchar_t buf[512];
        buf[0] = 0;
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU |
                    MIIM_STRING;
        mii.dwTypeData = buf;
        mii.cch = 511;
        if (!GetMenuItemInfoW(hMenu, UINT(pos), TRUE, &mii)) continue;
        if (mii.fType & MFT_SEPARATOR) {
            sepPending = true;
            continue;
        }
        Item it;
        it.text = CleanMenuText(buf);
        if (it.text.empty()) {
            // Owner-drawn with no text we can read.
            g_sawUnreadable = true;
            wchar_t name[32];
            swprintf(name, 32, L"Item %d", pos + 1);
            it.text = name;
        }
        if (IsHidden(it.text)) continue;
        it.id = ID_REAL;
        it.cmd = mii.wID;
        it.hOwner = hMenu;
        it.pos = UINT(pos);
        it.disabled = (mii.fState & (MFS_DISABLED | MFS_GRAYED)) != 0;
        it.checked = (mii.fState & MFS_CHECKED) != 0;
        if (it.checked || (mii.fType & MFT_RADIOCHECK)) anyCheck = true;
        it.groupStart = sepPending && !out.empty();
        sepPending = false;
        if (mii.hSubMenu) {
            it.hSub = mii.hSubMenu;
            it.lazy = true;
            Item ph;  // stands in until the submenu is read
            ph.text = L"\x2026";
            ph.disabled = true;
            it.sub.push_back(ph);
        }
        out.push_back(std::move(it));
    }
    // Give every item the check-mark column if any of them uses it.
    if (anyCheck)
        for (Item& it : out) it.radio = true;
    return out;
}

// Reads a submenu the first time it is entered, after telling its owner it
// is about to open, exactly as the system would.
static void LoadSubmenu(Item& item) {
    if (!item.lazy || !item.hSub) return;
    item.lazy = false;
    if (g.notify)
        SendMessageW(g.owner, WM_INITMENUPOPUP, WPARAM(item.hSub),
                     MAKELPARAM(item.pos, FALSE));
    g.loadedSubs.push_back(item.hSub);
    item.sub = ReadMenu(item.hSub);
    if (item.sub.empty()) {
        Item none;
        none.text = L"(empty)";
        none.disabled = true;
        item.sub.push_back(none);
    }
    ApplyStoredOrder(item.sub);
}

// The top level: the real items, then custom entries and our own two.
static std::vector<Item> BuildRootModel(HMENU hMenu) {
    g_sawUnreadable = false;
    std::vector<Item> items = ReadMenu(hMenu);
    // A top level with entries we cannot label is better left to the real
    // menu than chosen from blind.
    if (g_sawUnreadable) items.clear();
    if (items.empty()) return items;
    bool first = true;
    for (const CustomItem& ci : cfg.customItems) {
        Item it;
        it.text = ci.name;
        it.id = ID_CUSTOM;
        it.command = ci.command;
        it.groupStart = first;
        first = false;
        items.push_back(std::move(it));
    }
    if (cfg.showNativeEntry)
        items.push_back(MI(L"Default context options", ID_NATIVE, true));
    if (cfg.showOptionsEntry) items.push_back(BuildOptionsItem());
    ApplyStoredOrder(items);
    return items;
}

// Runs a custom item's command: first token is the target, the rest its
// arguments. Quotes around the target are honoured.
static void RunCustom(const std::wstring& command) {
    std::wstring file, args;
    size_t i = 0;
    while (i < command.size() && command[i] == L' ') i++;
    if (i < command.size() && command[i] == L'"') {
        const size_t e = command.find(L'"', i + 1);
        file = command.substr(i + 1, e == std::wstring::npos ? e : e - i - 1);
        i = e == std::wstring::npos ? command.size() : e + 1;
    } else {
        const size_t e = command.find(L' ', i);
        file = command.substr(i, e == std::wstring::npos ? e : e - i);
        i = e == std::wstring::npos ? command.size() : e;
    }
    while (i < command.size() && command[i] == L' ') i++;
    args = command.substr(i);
    if (file.empty()) return;
    ShellExecuteW(nullptr, nullptr, file.c_str(),
                  args.empty() ? nullptr : args.c_str(), nullptr,
                  SW_SHOWNORMAL);
}

// ---------------------------------------------------------------------------
// Wheel behaviour
// ---------------------------------------------------------------------------

static void ClampPosition();
static void GoBack();

static void Touch() {
    g.lastInput = Now();
    if (g.closing && g.closeCancelable) g.closing = false;
}

// Fractional list position under the cursor's angle.
static double CursorSlot(POINT pt) {
    const double dx = double(g.side) * (pt.x - g.center.x);
    const double dy = double(pt.y - g.center.y);
    const double phi =
        std::clamp(std::atan2(dy, std::max(dx, 1.0)) * 180.0 / kPi, -88.0, 88.0);
    return g.rot + phi / TopStep();
}

// Index of the label under the point, or -1.
static int HitTestLabel(POINT pt) {
    if (g.dragging || g.stack.empty()) return -1;
    const Level& lvl = g.stack.back();
    if (lvl.widths.size() != lvl.items.size()) return -1;
    const double dx = double(g.side) * (pt.x - g.center.x);
    const double dy = double(pt.y - g.center.y);
    if (dx <= 0.0) return -1;
    const double r = std::sqrt(dx * dx + dy * dy);
    const double q = g.rot + std::atan2(dy, dx) * 180.0 / kPi / TopStep();
    const long long k = std::llround(q);
    if (k < 0 || k >= (long long)lvl.items.size()) return -1;
    const double s = g.scale;
    const double lateral =
        std::fabs(q - double(k)) * TopStep() * kPi / 180.0 * r;
    if (lateral > cfg.pillHeight * s * 0.5 + 2.0 * s) return -1;
    const Item& it = lvl.items[size_t(k)];
    const double w = lvl.widths[size_t(k)] +
                     (it.sub.empty() ? 0.0 : 18.0 * s) +
                     (it.radio ? 18.0 * s : 0.0);
    const double r0 = lvl.innerPx, pad = cfg.pillPad * s;
    if (r < r0 - pad || r > r0 + w + pad) return -1;
    return int(k);
}

// The item that is highlighted, and that a click or Enter will choose: the
// label under the cursor if there is one, otherwise the one on the centre
// line. After wheel or keyboard stepping the centre line wins until the
// mouse moves again, so the highlight never jumps away from where you
// stepped to.
static int HighlightIndexAt(POINT pt) {
    if (g.pressed && !g.dragging) return g.pressTarget;
    if (!g.tiltHeld && !g.dragging) {
        const int h = HitTestLabel(pt);
        if (h >= 0) return h;
    }
    return Mod(std::llround(g.rot), ItemCount());
}

static void BeginDrag(int idx) {
    Level& lvl = g.stack.back();
    // A lone item has nowhere to go, and lifting it would empty the list.
    if (lvl.items.size() < 2 || idx < 0 || idx >= int(lvl.items.size()) ||
        lvl.widths.size() != lvl.items.size() ||
        lvl.items[size_t(idx)].id == ID_BACK ||
        lvl.items[size_t(idx)].id == ID_OPT_SETTING ||
        lvl.items[size_t(idx)].radio)  // these stay put
        return;
    g.dragItem = lvl.items[size_t(idx)];
    g.dragWidth = lvl.widths[size_t(idx)];
    lvl.items.erase(lvl.items.begin() + idx);
    lvl.widths.erase(lvl.widths.begin() + idx);
    g.dragFrom = g.dragIns = idx;
    g.gapPos = double(idx);
    g.dragging = true;
    g.tiltHeld = false;  // tilt keeps working so you can carry it up or down
}

// Puts the carried item back into the list at `at`. With commit, the new
// order is written back to the menu model so it sticks for later opens.
static void EndDrag(int at, bool commit) {
    if (!g.dragging) return;
    Level& lvl = g.stack.back();
    at = std::clamp(at, 0, int(lvl.items.size()));
    lvl.items.insert(lvl.items.begin() + at, std::move(g.dragItem));
    lvl.widths.insert(lvl.widths.begin() + at, g.dragWidth);
    g.dragItem = Item();
    g.dragging = false;
    if (commit && at != g.dragFrom && lvl.src) {
        // A manual order replaces the recent-first arrangement.
        for (Item& it : lvl.items) it.hoisted = false;
        lvl.hoisted = 0;
        std::vector<Item> out;
        for (const Item& it : lvl.items)
            if (it.id != ID_BACK) out.push_back(it);
        *lvl.src = std::move(out);
        if (g.stack.size() == 1) RefreshOptionsItem();
        SaveOrder(*lvl.src);
    }
    ClampPosition();
}

static void CloseWheel() {
    if (!g.open) return;
    EndDrag(g.dragFrom, false);
    g.pressed = false;
    g.open = false;
    g.closing = false;
    g_hookTarget = nullptr;  // the hook stops taking events
    ShowWindow(g.hwnd, SW_HIDE);
}

static void StartClose(double seconds, bool cancelable) {
    if (!g.open || g.closing) return;
    g.closing = true;
    g.closeCancelable = cancelable;
    g.closeStart = Now();
    g.closeDur = std::max(seconds, 0.001);
}

static void ClampPosition() {
    const int N = SlotCount();
    if (Wrapping()) {
        while (g.rot >= N) { g.rot -= N; g.target -= N; }
        while (g.rot < 0)  { g.rot += N; g.target += N; }
    } else {
        g.rot = std::clamp(g.rot, 0.0, double(N - 1));
        g.target = std::clamp(g.target, 0.0, double(N - 1));
    }
}

static void StepBy(int n) {
    if (!g.open) return;
    Touch();
    // Stepping overrides tilt until the mouse actually moves again.
    g.tiltHeld = true;
    GetCursorPos(&g.holdPos);
    g.target = std::round(g.target) + n;
    ClampPosition();
}

// Moves the highlight straight to an item; like StepBy, this parks tilt.
static void JumpTo(int idx) {
    if (!g.open || g.dragging) return;
    Touch();
    g.tiltHeld = true;
    GetCursorPos(&g.holdPos);
    g.target = double(idx);
    ClampPosition();
}

static int CurrentIndex() {
    return std::clamp(int(std::llround(g.target)), 0, ItemCount() - 1);
}

// Up / Down: one item at a time, wrapping from one end to the other.
static void KeyStep(int dir) {
    if (!g.open || g.dragging) return;
    const int n = ItemCount();
    JumpTo(Mod(CurrentIndex() + dir, n));
}

// Left / Right (and Home / End): first or last item of the list.
static void KeyJumpEnd(bool last) {
    if (!g.open) return;
    JumpTo(last ? ItemCount() - 1 : 0);
}

static bool MatchAt(const std::wstring& text, size_t at,
                    const std::wstring& needle) {
    if (at + needle.size() > text.size()) return false;
    for (size_t i = 0; i < needle.size(); i++)
        if (std::towlower(text[at + i]) != std::towlower(needle[i]))
            return false;
    return true;
}

// First usable item matching `needle`, searching from `from` and wrapping.
// Names that start with it win over names that merely contain it.
static int FindMatch(const std::wstring& needle, int from) {
    const std::vector<Item>& items = g.stack.back().items;
    const int n = int(items.size());
    for (int pass = 0; pass < 2; pass++) {
        for (int j = 0; j < n; j++) {
            const int i = Mod(from + j, n);
            const Item& it = items[size_t(i)];
            if (it.disabled || it.id == ID_BACK) continue;
            if (pass == 0) {
                if (MatchAt(it.text, 0, needle)) return i;
            } else {
                for (size_t at = 1; at < it.text.size(); at++)
                    if (MatchAt(it.text, at, needle)) return i;
            }
        }
    }
    return -1;
}

// Type-to-find. Letters accumulate into a search ("pro" -> Properties);
// tapping the same letter again steps through the items that start with it.
static void OnChar(wchar_t ch) {
    if (!g.open || g.dragging || ch < 32) return;
    const double now = Now();
    if (now - g.typedTime > cfg.typeResetSec) g.typed.clear();

    const std::wstring longer = g.typed + ch;
    int idx = FindMatch(longer, 0);
    if (idx >= 0 && !(g.typed.size() == 1 && g.typed[0] == ch &&
                      !MatchAt(g.stack.back().items[size_t(idx)].text, 0,
                               longer))) {
        g.typed = longer;
    } else if (g.typed.size() == 1 &&
               std::towlower(g.typed[0]) == std::towlower(ch)) {
        idx = FindMatch(g.typed, CurrentIndex() + 1);  // same letter: next one
    } else {
        idx = -1;  // nothing matches; ignore the keystroke
    }
    if (idx < 0) {
        Touch();
        return;
    }
    g.typedTime = now;
    JumpTo(idx);
}

// Backspace edits the search if there is one, otherwise goes up a level.
static void OnBackspace() {
    if (!g.open) return;
    if (!g.typed.empty() && Now() - g.typedTime <= cfg.typeResetSec) {
        g.typed.pop_back();
        g.typedTime = Now();
        if (!g.typed.empty()) {
            const int idx = FindMatch(g.typed, 0);
            if (idx >= 0) JumpTo(idx);
        }
        Touch();
        return;
    }
    GoBack();
}

static void OpenWheel(POINT pt) {
    // Scale for the monitor the wheel opens on, which is not always the
    // owner window's.
    UINT dpi = 0, dpiY = 0;
    if (FAILED(GetDpiForMonitor(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST),
                                MDT_EFFECTIVE_DPI, &dpi, &dpiY)))
        dpi = g.owner ? GetDpiForWindow(g.owner) : 0;
    g.scale = float(dpi ? dpi : 96) / 96.0f;
    g.forceRender = true;
    ApplyTheme();
    g.center = pt;
    g.tiltY = pt.y;
    g.pressed = false;
    g.dragging = false;
    g.typed.clear();
    g.stack.clear();
    RefreshOptionsItem();
    g.stack.push_back(BuildRootLevel());
    LayoutLevel();

    // Open on the preferred side unless that would run off the screen.
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    g.side = cfg.preferLeft ? -1 : 1;
    if (GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi)) {
        const int reach = int(g.outerPx);
        const bool fitsRight = pt.x + reach <= mi.rcWork.right;
        const bool fitsLeft = pt.x - reach >= mi.rcWork.left;
        if (g.side > 0 && !fitsRight && fitsLeft) g.side = -1;
        if (g.side < 0 && !fitsLeft && fitsRight) g.side = 1;

        // Near the top or bottom of the screen, slide the wheel along the
        // edge until the whole arc fits. The dead band stays on the line the
        // cursor is on (tiltY), so the wheel does not open already spinning.
        const int top = mi.rcMonitor.top + reach;
        const int bottom = mi.rcMonitor.bottom - reach;
        if (top <= bottom) g.center.y = std::clamp(int(pt.y), top, bottom);
    }

    g.rot = g.target = double(PreselectIndex(g.stack.back().items));
    g.wheelAccum = 0;
    g.tiltHeld = false;
    g.closing = false;
    g.opacity = 0.0f;
    g.openTime = g.lastTick = g.lastInput = Now();
    g.lastCursor = pt;
    g.open = true;
    Render();
    ShowWindow(g.hwnd, SW_SHOWNOACTIVATE);
}

static void GoBack() {
    if (g.stack.size() < 2 || g.dragging) return;
    const double restored = g.stack.back().savedRot;
    g.stack.pop_back();
    g.typed.clear();
    LayoutLevel();
    g.rot = g.target = restored;
    ClampPosition();
    Touch();
}

static void ActivateIndex(int idx) {
    if (!g.open || g.dragging || (g.closing && !g.closeCancelable)) return;
    Touch();
    if (idx < 0 || idx >= ItemCount()) return;
    const Item it = g.stack.back().items[size_t(idx)];
    if (it.disabled) return;
    if (it.id == ID_BACK) {
        GoBack();
        return;
    }
    const bool atRoot = g.stack.size() == 1;
    const std::wstring rootText = atRoot ? it.text : g.stack.back().rootText;
    if (!it.sub.empty()) {
        // Build the submenu from the model, not from our display copy, so a
        // rearranged submenu shows its saved order.
        std::vector<Item>* parentSrc = g.stack.back().src;
        Item* srcItem = parentSrc ? FindSame(*parentSrc, it) : nullptr;
        if (srcItem) LoadSubmenu(*srcItem);
        Level lvl;
        lvl.savedRot = double(idx);  // parent rests on the entry we came through
        lvl.src = srcItem ? &srcItem->sub : nullptr;
        lvl.rootText = rootText;
        const std::vector<Item>& kids = srcItem ? srcItem->sub : it.sub;
        lvl.items.push_back(MI(L"\x2039 Back", ID_BACK));
        lvl.items.insert(lvl.items.end(), kids.begin(), kids.end());
        g.stack.push_back(std::move(lvl));
        g.typed.clear();
        LayoutLevel();
        g.rot = g.target = 1.0;  // first real entry
        return;
    }
    if (it.id == ID_NATIVE) {
        g.result = R_NATIVE;
        CloseWheel();
        return;
    }
    if (it.id >= ID_OPT_BASE) {
        ApplyOption(it.id);
        return;
    }
    if (it.id == ID_CUSTOM) {
        // Close first, so the wheel is not in the way of whatever opens.
        CloseWheel();
        RunCustom(it.command);
        return;
    }
    RecordChoice(rootText, it.text);
    g.result = R_COMMAND;
    g.resCmd = it.cmd;
    g.resMenu = it.hOwner;
    g.resPos = it.pos;
    CloseWheel();
}

// Enter, or anything else that means "choose what is highlighted".
static void Activate() { ActivateIndex(HighlightIndexAt(g.lastCursor)); }

// Left button released after a press that started inside the wheel.
static void OnLeftUp() {
    if (!g.open) return;
    const int target = g.pressTarget;
    if (g.dragging && g.pressMoved) {
        EndDrag(g.dragIns, true);  // carried somewhere: drop it
    } else if (g.dragging) {
        // Held but never moved: a slow click, not a rearrange.
        EndDrag(g.dragFrom, false);
        g.pressed = false;
        ActivateIndex(target);
    } else if (g.pressed) {
        g.pressed = false;
        // A click on a label always chooses it. A click on empty space (the
        // hub, or between labels) does what the user has set.
        if (g.pressSlot >= 0 || cfg.emptyClick == 0)
            ActivateIndex(target);
        else if (cfg.emptyClick == 2)
            CloseWheel();
    }
    g.pressed = false;
}

static void OnEscape() {
    if (g.dragging) {
        EndDrag(g.dragFrom, false);
        g.pressed = false;
    } else {
        CloseWheel();
    }
}

static void Tick(double dt) {
    const double now = Now();

    // The menu's owner went away, or another window came forward.
    if (!IsWindow(g.owner)) StartClose(cfg.dismissSec, false);
    // (Activation can still be settling just after the click, so the first
    // quarter second only records which window that is.)
    const HWND fgNow = GetForegroundWindow();
    if (now - g.openTime < 0.25) {
        if (fgNow) g.fg = fgNow;
    } else if (fgNow && g.fg && fgNow != g.fg) {
        StartClose(cfg.dismissSec, false);
    }

    POINT p;
    GetCursorPos(&p);
    const bool inside = InsideWheel(p);
    if ((p.x != g.lastCursor.x || p.y != g.lastCursor.y) && inside) Touch();
    g.lastCursor = p;

    if (!g.typed.empty() && now - g.typedTime > cfg.typeResetSec)
        g.typed.clear();

    // Press on a label, then hold or pull: pick it up.
    if (g.pressed) {
        const double thresh = cfg.dragStartPx * g.scale;
        const double mx = p.x - g.pressPos.x, my = p.y - g.pressPos.y;
        if (mx * mx + my * my > thresh * thresh) g.pressMoved = true;
    }
    if (g.pressed && !g.dragging) {
        Touch();
        if (g.pressSlot >= 0 &&
            (g.pressMoved || now - g.pressTime >= cfg.holdSec))
            BeginDrag(g.pressSlot);
    }
    if (g.dragging) {
        Touch();
        const Level& lvl = g.stack.back();
        const int n = int(lvl.items.size());
        const int minIns = (n > 0 && lvl.items[0].id == ID_BACK) ? 1 : 0;
        g.dragIns = std::clamp(int(std::llround(CursorSlot(p))), minIns, n);
        g.gapPos += (double(g.dragIns) - g.gapPos) * (1.0 - std::exp(-dt * 22.0));
    }

    // Rate control: distance past the dead band sets the spin speed.
    // Below the band = further down the list, on either side.
    const double R = g.tiltR;
    const double band = R * std::sin(cfg.deadBandDeg * kPi / 180.0);
    const double dy = double(p.y) - double(g.tiltY);
    double v = 0.0;
    const bool dismissing = g.closing && !g.closeCancelable;
    const bool pressing = g.pressed && !g.dragging;  // hold still while aiming
    if (g.tiltHeld) {
        const double hold = cfg.wheelHoldPx * g.scale;
        const double mx = p.x - g.holdPos.x, my = p.y - g.holdPos.y;
        if (mx * mx + my * my > hold * hold) g.tiltHeld = false;
    }
    if (cfg.tilt && inside && !dismissing && !g.tiltHeld && !pressing) {
        const double d = std::fabs(dy) - band;
        if (d > 0.0) {
            const double range = std::max(1.0, (R - band) * cfg.tiltRange);
            const double n = std::min(1.0, d / range);
            v = cfg.maxSpeed * std::pow(n, double(cfg.expo)) *
                (dy > 0 ? 1.0 : -1.0);
        }
    }

    if (v != 0.0) {
        g.rot += v * dt;
        // Settle on the next detent in the direction of travel.
        g.target = v > 0 ? std::ceil(g.rot - 0.25) : std::floor(g.rot + 0.25);
        g.lastInput = now;
    } else {
        g.rot += (g.target - g.rot) * (1.0 - std::exp(-dt * cfg.snapRate));
        if (std::fabs(g.target - g.rot) < 0.001) g.rot = g.target;
    }
    ClampPosition();

    if (!g.closing && now - g.lastInput > cfg.idleTimeoutSec)
        StartClose(cfg.fadeOutSec, true);

    float o = float(std::min(1.0, (now - g.openTime) / cfg.fadeInSec));
    if (g.closing) {
        const double t = (now - g.closeStart) / g.closeDur;
        if (t >= 1.0) {
            CloseWheel();
            return;
        }
        o *= float(1.0 - t);
    }
    g.opacity = o;
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------

// The wheel never takes focus or capture, so a low-level mouse hook is what
// lets it see scrolls and clicks. A low-level hook holds up the mouse for the
// whole system until its callback returns, and it runs on the thread that
// installed it, so it lives on a small thread of its own that never blocks.
// The callback only decides whether an event belongs to the wheel and posts
// it to the wheel window; the wheel's thread does the work.
//
// What the callback needs to know is published in these atomics.

static bool SnapInside(POINT pt) {
    const double dx = double(g_snapSide.load()) * (pt.x - g_snapCx.load());
    const double dy = double(pt.y - g_snapCy.load());
    const double R = double(g_snapOuter.load());
    if (dx * dx + dy * dy > R * R) return false;
    return dx > -double(g_snapHub.load());  // see InsideWheel
}

// Called by the wheel's thread whenever the geometry may have changed.
static void PublishGeometry() {
    g_snapCx = g.center.x;
    g_snapCy = g.center.y;
    g_snapSide = g.side;
    g_snapOuter = int(g.outerPx);
    g_snapHub = int(cfg.hubRadius * g.scale);
}

// Only touched by the hook thread.
static bool s_swallowLUp = false;
static bool s_swallowRUp = false;

static LRESULT CALLBACK MouseHook(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        const MSLLHOOKSTRUCT* m = reinterpret_cast<const MSLLHOOKSTRUCT*>(lp);
        const HWND wheel = g_hookTarget.load();
        // Button-ups are matched to the downs we took, so nothing ever sees
        // half a click.
        if (wp == WM_LBUTTONUP && s_swallowLUp) {
            s_swallowLUp = false;
            if (wheel) PostMessageW(wheel, WM_WHEEL_LUP, 0, 0);
            return 1;
        }
        if (wp == WM_RBUTTONUP && s_swallowRUp) {
            s_swallowRUp = false;
            if (wheel) PostMessageW(wheel, WM_WHEEL_RUP, 0, 0);
            return 1;
        }
        if (wheel) {
            const bool inside = SnapInside(m->pt);
            bool outside = false;
            switch (wp) {
                case WM_MOUSEWHEEL:
                    if (inside) {
                        PostMessageW(wheel, WM_WHEEL_SCROLL, 0,
                                     LPARAM(short(HIWORD(m->mouseData))));
                        return 1;
                    }
                    outside = true;
                    break;
                case WM_LBUTTONDOWN:
                    if (inside) {
                        s_swallowLUp = true;
                        PostMessageW(wheel, WM_WHEEL_LDOWN,
                                     WPARAM(INT_PTR(m->pt.x)),
                                     LPARAM(m->pt.y));
                        return 1;
                    }
                    outside = true;
                    break;
                case WM_RBUTTONDOWN:
                    // Inside: asks for the standard menu, acted on at
                    // release. Outside: dismisses, like any other click.
                    if (inside) {
                        s_swallowRUp = true;
                        return 1;
                    }
                    outside = true;
                    break;
                case WM_MBUTTONDOWN:
                case WM_XBUTTONDOWN:
                    outside = !inside;
                    break;
            }
            if (outside) {
                // The wheel is over. Stop taking events right away, tell the
                // wheel, and let this one through to whatever it was aimed at.
                g_hookTarget = nullptr;
                PostMessageW(wheel, WM_WHEEL_OUTSIDE, 0, 0);
            }
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

struct HookThread {
    HANDLE thread = nullptr;
    DWORD id = 0;
    HANDLE ready = nullptr;
    bool installed = false;
};

static DWORD WINAPI HookThreadProc(LPVOID param) {
    HookThread* ht = static_cast<HookThread*>(param);
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // make a queue
    s_swallowLUp = s_swallowRUp = false;
    HHOOK hook = SetWindowsHookExW(WH_MOUSE_LL, MouseHook, g_inst, 0);
    ht->installed = hook != nullptr;
    SetEvent(ht->ready);
    if (!hook) return 0;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) DispatchMessageW(&msg);
    UnhookWindowsHookEx(hook);
    return 0;
}

static bool StartHookThread(HookThread* ht) {
    ht->ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ht->ready) return false;
    ht->thread = CreateThread(nullptr, 0, HookThreadProc, ht, 0, &ht->id);
    if (ht->thread) WaitForSingleObject(ht->ready, INFINITE);
    CloseHandle(ht->ready);
    ht->ready = nullptr;
    if (ht->thread && !ht->installed) {
        WaitForSingleObject(ht->thread, INFINITE);
        CloseHandle(ht->thread);
        ht->thread = nullptr;
    }
    return ht->thread != nullptr;
}

static void StopHookThread(HookThread* ht) {
    if (!ht->thread) return;
    PostThreadMessageW(ht->id, WM_QUIT, 0, 0);
    WaitForSingleObject(ht->thread, INFINITE);
    CloseHandle(ht->thread);
    ht->thread = nullptr;
}

// Mouse events the hook thread passed on, handled on the wheel's thread.
// Returns true if the message was one of ours.
static bool HandleWheelEvent(const MSG& msg) {
    if (msg.hwnd != g.hwnd || !g.hwnd) return false;
    switch (msg.message) {
        case WM_WHEEL_LDOWN: {
            if (!g.open) return true;
            // Acts on release: a click chooses, a hold or pull on a label
            // picks it up instead.
            const POINT pt = {int(INT_PTR(msg.wParam)), int(msg.lParam)};
            // A press on a label is for that label, even just after a scroll
            // or key step has parked the highlight on the centre line.
            g.pressSlot = HitTestLabel(pt);
            g.pressTarget =
                g.pressSlot >= 0 ? g.pressSlot : HighlightIndexAt(pt);
            g.pressMoved = false;
            g.pressed = true;
            g.pressPos = pt;
            g.pressTime = Now();
            Touch();
            return true;
        }
        case WM_WHEEL_LUP:
            OnLeftUp();
            return true;
        case WM_WHEEL_RUP:
            if (!g.open) return true;
            if (g.dragging) {
                OnEscape();  // put the carried item back
            } else {
                g.result = R_NATIVE;
                CloseWheel();
            }
            return true;
        case WM_WHEEL_SCROLL: {
            g.wheelAccum += int(msg.lParam);
            const int notches = g.wheelAccum / WHEEL_DELTA;
            g.wheelAccum -= notches * WHEEL_DELTA;
            if (notches) StepBy(-notches);  // scroll down = next
            return true;
        }
        case WM_WHEEL_OUTSIDE:
            CloseWheel();  // at once; the click itself is still on its way
            return true;
    }
    return false;
}

// A button press aimed at some other window. Clicks inside the wheel never
// reach a queue (the hook takes them), so any that shows up is outside.
static bool IsMouseDown(UINT message) {
    switch (message) {
        case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN: case WM_LBUTTONDBLCLK: case WM_RBUTTONDBLCLK:
        case WM_NCLBUTTONDOWN: case WM_NCRBUTTONDOWN: case WM_NCMBUTTONDOWN:
        case WM_NCXBUTTONDOWN:
            return true;
    }
    return false;
}

// Keys reach us through the thread's own queue: the wheel never activates, so
// the menu's owner keeps the focus, and RunWheel takes key messages before
// they are dispatched, as a menu loop does. Typed characters arrive
// separately as WM_CHAR.
static void OnKey(UINT vk) {
    switch (vk) {
        case VK_ESCAPE: OnEscape(); break;
        case VK_UP:     KeyStep(-1); break;
        case VK_DOWN:   KeyStep(+1); break;
        case VK_HOME:
        case VK_LEFT:   KeyJumpEnd(false); break;
        case VK_END:
        case VK_RIGHT:  KeyJumpEnd(true); break;
        case VK_RETURN: Activate(); break;
        case VK_BACK:   OnBackspace(); break;
    }
    if (g.open) ClampPosition();
}

static LRESULT CALLBACK WheelProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    // RunWheel takes the hook's events straight from the queue. If some other
    // message loop is running inside ours (a message box, a shell extension
    // that pumps messages) it dispatches them here instead, so keep them for
    // RunWheel to handle when it is back in control.
    if (msg >= WM_WHEEL_LDOWN && msg <= WM_WHEEL_OUTSIDE && h == g.hwnd) {
        MSG held = {};
        held.hwnd = h;
        held.message = msg;
        held.wParam = wp;
        held.lParam = lp;
        g.pending.push_back(held);
        return 0;
    }
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_ARROW));
            return TRUE;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

static std::wstring StrSetting(PCWSTR name, int index = -1) {
    return std::wstring(
        index < 0 ? WindhawkUtils::StringSetting::make(name).get()
                  : WindhawkUtils::StringSetting::make(name, index).get());
}

// "#RRGGBB" or "RRGGBB".
static bool ParseColor(const std::wstring& text, uint32_t* out) {
    std::wstring t = text;
    while (!t.empty() && (t.front() == L' ' || t.front() == L'#'))
        t.erase(t.begin());
    while (!t.empty() && t.back() == L' ') t.pop_back();
    if (t.size() != 6) return false;
    uint32_t v = 0;
    for (wchar_t ch : t) {
        int d;
        if (ch >= L'0' && ch <= L'9') d = ch - L'0';
        else if (ch >= L'a' && ch <= L'f') d = ch - L'a' + 10;
        else if (ch >= L'A' && ch <= L'F') d = ch - L'A' + 10;
        else return false;
        v = (v << 4) | uint32_t(d);
    }
    *out = v;
    return true;
}

// Reads the mod's settings. Choices made in "Wheel options" are kept in the
// mod's storage and laid over the top, until the settings are next changed
// in Windhawk, at which point Windhawk's values win again.
static void LoadConfig() {
    const std::wstring side = StrSetting(L"side");
    const std::wstring opensOn = StrSetting(L"opensOn");
    const std::wstring submenus = StrSetting(L"submenus");
    const std::wstring theme = StrSetting(L"theme");
    cfg.preferLeft = side == L"left";
    cfg.preselect = opensOn == L"first" ? 0 : opensOn == L"last" ? 1 : 2;
    cfg.menuMode = submenus == L"stack" ? 1 : submenus == L"alternate" ? 2 : 0;
    cfg.theme = theme == L"light" ? 1 : theme == L"system" ? 2 : 0;
    cfg.mruCount = std::clamp(Wh_GetIntSetting(L"recentOnTop"), 0, 8);
    cfg.accentOutline = Wh_GetIntSetting(L"accent") != 0;
    cfg.tilt = Wh_GetIntSetting(L"tilt") != 0;
    cfg.maxSpeed = float(std::clamp(Wh_GetIntSetting(L"tiltSpeed"), 2, 80));
    cfg.expo =
        float(std::clamp(Wh_GetIntSetting(L"tiltCurve"), 50, 500)) / 100.0f;
    cfg.deadBandDeg =
        float(std::clamp(Wh_GetIntSetting(L"deadBand"), 10, 400)) / 10.0f;
    cfg.stepDeg = float(std::clamp(Wh_GetIntSetting(L"itemSpacing"), 6, 30));
    cfg.wrap = Wh_GetIntSetting(L"wrap") != 0;
    cfg.idleTimeoutSec =
        float(std::clamp(Wh_GetIntSetting(L"fadeOutAfter"), 3, 3600));
    cfg.labelBgOpacity =
        float(std::clamp(Wh_GetIntSetting(L"labelOpacity"), 0, 100)) / 100.0f;
    const float edge =
        float(std::clamp(Wh_GetIntSetting(L"edgeOpacity"), 0, 100)) / 100.0f;
    cfg.stops[1].alpha = cfg.stops[4].alpha = edge;
    cfg.hasTextColor = ParseColor(StrSetting(L"textColor"), &cfg.userText);
    cfg.hasLabelColor = ParseColor(StrSetting(L"labelColor"), &cfg.userLabel);
    cfg.hasHighlight =
        ParseColor(StrSetting(L"highlightColor"), &cfg.userHighlight);
    const std::wstring emptyClick = StrSetting(L"emptyClick");
    cfg.emptyClick = emptyClick == L"nothing" ? 1 : emptyClick == L"close" ? 2 : 0;
    cfg.showNativeEntry = Wh_GetIntSetting(L"showDefaultEntry") != 0;
    cfg.showOptionsEntry = Wh_GetIntSetting(L"showOptionsEntry") != 0;

    cfg.hideItems.clear();
    for (int i = 0; i < 200; i++) {
        const std::wstring pat = StrSetting(L"hideItems[%d]", i);
        if (pat.empty()) break;
        cfg.hideItems.push_back(pat);
    }
    cfg.customItems.clear();
    for (int i = 0; i < 50; i++) {
        CustomItem ci;
        ci.name = StrSetting(L"customItems[%d].name", i);
        ci.command = StrSetting(L"customItems[%d].command", i);
        if (ci.name.empty()) break;
        if (!ci.command.empty()) cfg.customItems.push_back(ci);
    }
    cfg.copyNames = Split(ToLower(StrSetting(L"copyNames")), L';');
    cfg.pasteNames = Split(ToLower(StrSetting(L"pasteNames")), L';');
    cfg.neverPreselect = Split(ToLower(StrSetting(L"neverPreselect")), L';');

    const bool drop = g_dropOverrides.exchange(false);
    for (const Setting& st : Settings()) {
        const std::wstring key = OverrideKey(st);
        if (drop) {
            Wh_DeleteValue(key.c_str());
            continue;
        }
        const int v = Wh_GetIntValue(key.c_str(), INT_MIN);
        if (v != INT_MIN) st.set(float(v) / 100.0f);
    }
}

// ---------------------------------------------------------------------------
// Mod glue: hosting the wheel inside a TrackPopupMenu call
// ---------------------------------------------------------------------------

static const wchar_t kWheelClass[] = L"WheelContextMenu.Wheel";

static bool EnsureWindowClass() {
    if (g_classRegistered) return true;
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = g_inst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpfnWndProc = WheelProc;
    wc.lpszClassName = kWheelClass;
    // If the class is somehow already there, it was left by code that is no
    // longer loaded and its window procedure is gone, so don't reuse it. The
    // caller falls back to the real menu.
    if (!RegisterClassExW(&wc)) return false;
    g_classRegistered = true;
    return true;
}

// Direct2D, DirectWrite and GDI objects. Only ever called with no wheel on
// screen: by the wheel's own thread on its way out, or after it has finished.
static void ReleaseGraphics() {
    DiscardDeviceResources();
    SafeRelease(g.fmt);
    SafeRelease(g.fmtRight);
    SafeRelease(g.dw);
    SafeRelease(g.d2d);
    if (g.dib) {
        SelectObject(g.memDC, g.oldBmp);
        DeleteObject(g.dib);
        g.dib = nullptr;
        g.dibSize = 0;
    }
    if (g.memDC) {
        DeleteDC(g.memDC);
        g.memDC = nullptr;
    }
}

// What a frame depends on. An idle wheel redraws nothing.
struct FrameKey {
    double rot = 0, gap = 0;
    float opacity = -1, outer = 0;
    POINT cursor = {};
    size_t depth = 0;
    int size = 0, side = 0;
    bool dragging = false;
    int highlight = -1;
    std::wstring typed;
};

static bool NeedsRender(FrameKey* last) {
    FrameKey now;
    now.rot = g.rot;
    now.gap = g.gapPos;
    now.opacity = g.opacity;
    now.outer = g.outerPx;
    now.cursor = g.lastCursor;
    now.depth = g.stack.size();
    now.size = g.size;
    now.side = g.side;
    now.dragging = g.dragging;
    now.highlight = g.dragging ? -1 : HighlightIndexAt(g.lastCursor);
    now.typed = g.typed;
    const bool same =
        !g.forceRender && now.rot == last->rot && now.gap == last->gap &&
        now.opacity == last->opacity && now.outer == last->outer &&
        now.cursor.x == last->cursor.x && now.cursor.y == last->cursor.y &&
        now.depth == last->depth && now.size == last->size &&
        now.side == last->side && now.dragging == last->dragging &&
        now.highlight == last->highlight &&
        now.typed == last->typed;
    g.forceRender = false;
    *last = now;
    return !same;
}

struct WheelResult {
    int kind = R_NONE;
    UINT cmd = 0;
    HMENU menu = nullptr;
    UINT pos = 0;
};

// True on the thread that is currently running a wheel.
static thread_local bool t_inWheel = false;

// Shows the wheel for this menu and runs it to completion on the calling
// thread, like TrackPopupMenu does. Returns false if the wheel could not
// take the menu, in which case the caller falls back to the real one.
static bool RunWheel(HMENU hMenu, UINT flags, int x, int y, HWND owner,
                     WheelResult* result) {
    bool expected = false;
    if (!g_busy.compare_exchange_strong(expected, true)) return false;
    struct Release {
        ~Release() {
            t_inWheel = false;
            g_busy = false;
        }
    } release;
    if (g_unloading) return false;
    t_inWheel = true;

    if (g_cfgDirty.exchange(false)) LoadConfig();
    LoadHistory();

    g.owner = owner;
    g.notify = !(flags & TPM_NONOTIFY);
    g.result = R_NONE;
    g.loadedSubs.clear();
    g.pending.clear();

    // The messages a real popup sends before it appears. Owners use them to
    // fill in or enable items, so they have to come before we read anything.
    if (g.notify) {
        SendMessageW(owner, WM_ENTERMENULOOP, TRUE, 0);
        SendMessageW(owner, WM_INITMENU, WPARAM(hMenu), 0);
        SendMessageW(owner, WM_INITMENUPOPUP, WPARAM(hMenu), 0);
    }
    auto endNotifications = [&] {
        if (!g.notify) return;
        for (size_t i = g.loadedSubs.size(); i-- > 0;)
            SendMessageW(owner, WM_UNINITMENUPOPUP, WPARAM(g.loadedSubs[i]), 0);
        SendMessageW(owner, WM_UNINITMENUPOPUP, WPARAM(hMenu), 0);
        SendMessageW(owner, WM_MENUSELECT, MAKEWPARAM(0, 0xFFFF), 0);
        SendMessageW(owner, WM_EXITMENULOOP, TRUE, 0);
    };

    g_menu = BuildRootModel(hMenu);
    if (g_menu.empty() || !EnsureWindowClass()) {
        endNotifications();
        g_menu.clear();
        return false;
    }
    g.hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kWheelClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g_inst,
        nullptr);
    HookThread hookThread;
    if (!g.hwnd || !StartHookThread(&hookThread)) {
        // Without the hook the wheel could not be clicked: use the real menu.
        if (g.hwnd) DestroyWindow(g.hwnd);
        g.hwnd = nullptr;
        endNotifications();
        g_menu.clear();
        return false;
    }
    g.fg = GetForegroundWindow();

    POINT pt = {x, y};
    OpenWheel(pt);
    PublishGeometry();
    g_hookTarget = g.hwnd;

    FrameKey lastFrame;
    NeedsRender(&lastFrame);  // OpenWheel drew the first frame
    while (g.open && !g_unloading) {
        MSG msg;
        while (g.open && PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE)) {
            // A click on something else ends the wheel, and stays in the
            // queue so its target handles it after this call has returned,
            // not from inside it.
            if (msg.hwnd != g.hwnd && IsMouseDown(msg.message)) {
                CloseWheel();
                break;
            }
            if (!PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) break;
            if (msg.message == WM_QUIT) {
                PostQuitMessage(int(msg.wParam));  // not ours to swallow
                CloseWheel();
                break;
            }
            if (HandleWheelEvent(msg)) continue;
            // Keys are the wheel's while it is open.
            if (msg.message == WM_KEYDOWN) {
                // Translating queues a WM_CHAR for type-to-find. Not for
                // the keys that can end the wheel, or their character would
                // be left behind for the owner to receive afterwards.
                if (msg.wParam != VK_RETURN && msg.wParam != VK_ESCAPE &&
                    msg.wParam != VK_BACK)
                    TranslateMessage(&msg);
                OnKey(UINT(msg.wParam));
                continue;
            }
            if (msg.message == WM_CHAR) {
                OnChar(wchar_t(std::towlower(wchar_t(msg.wParam))));
                continue;
            }
            if (msg.message == WM_SYSKEYDOWN) {
                StartClose(cfg.dismissSec, false);  // Alt or F10: step aside
                continue;
            }
            if (msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP ||
                msg.message == WM_SYSCHAR || msg.message == WM_DEADCHAR ||
                msg.message == WM_SYSDEADCHAR)
                continue;
            DispatchMessageW(&msg);
        }
        // Events a nested loop handed to the window procedure.
        while (g.open && !g.pending.empty()) {
            const MSG held = g.pending.front();
            g.pending.erase(g.pending.begin());
            HandleWheelEvent(held);
        }
        if (!g.open) break;

        const double t = Now();
        const double dt = std::min(t - g.lastTick, 0.05);
        g.lastTick = t;
        Tick(dt);
        if (!g.open) break;
        PublishGeometry();
        if (NeedsRender(&lastFrame)) Render();
        if (FAILED(DwmFlush())) Sleep(8);  // pace to the compositor
    }
    if (g.open) CloseWheel();

    StopHookThread(&hookThread);
    DestroyWindow(g.hwnd);
    g.hwnd = nullptr;
    endNotifications();

    result->kind = g.result;
    result->cmd = g.resCmd;
    result->menu = g.resMenu;
    result->pos = g.resPos;
    g.stack.clear();
    g_menu.clear();
    // On unload, let go of everything here, on the thread that was using it.
    if (g_unloading) ReleaseGraphics();
    return true;
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
static TrackPopupMenuEx_t TrackPopupMenuEx_Original;
using TrackPopupMenu_t = decltype(&TrackPopupMenu);
static TrackPopupMenu_t TrackPopupMenu_Original;

// Set while we are deliberately calling the real function, so a nested call
// from inside user32 is not taken over as well.
static thread_local int t_passThrough = 0;

static bool ShouldTake(HMENU hMenu, HWND hWnd) {
    if (t_inWheel) {
        // Something asked for another menu from inside the wheel's own loop.
        // The wheel gets out of the way and the real menu shows.
        if (g.open) CloseWheel();
        return false;
    }
    return !t_passThrough && !g_unloading && hMenu && IsMenu(hMenu) &&
           GetMenuItemCount(hMenu) > 0 && hWnd && IsWindow(hWnd);
}

// Turns the wheel's outcome into what TrackPopupMenu would have returned and
// sent. Returns false when the real menu should be shown instead.
static bool Deliver(const WheelResult& r, UINT flags, HWND hWnd, BOOL* ret) {
    if (r.kind == R_NATIVE) return false;
    if (r.kind == R_COMMAND) {
        if (flags & TPM_RETURNCMD) {
            *ret = BOOL(r.cmd);
            return true;
        }
        if (!(flags & TPM_NONOTIFY)) {
            MENUINFO mi = {};
            mi.cbSize = sizeof(mi);
            mi.fMask = MIM_STYLE;
            if (GetMenuInfo(r.menu, &mi) && (mi.dwStyle & MNS_NOTIFYBYPOS))
                PostMessageW(hWnd, WM_MENUCOMMAND, r.pos, LPARAM(r.menu));
            else
                PostMessageW(hWnd, WM_COMMAND, MAKEWPARAM(r.cmd, 0), 0);
        }
        *ret = TRUE;
        return true;
    }
    // Dismissed without choosing.
    *ret = (flags & TPM_RETURNCMD) ? 0 : TRUE;
    return true;
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                  HWND hWnd, LPTPMPARAMS lptpm) {
    if (ShouldTake(hMenu, hWnd)) {
        WheelResult r;
        BOOL ret = FALSE;
        if (RunWheel(hMenu, uFlags, x, y, hWnd, &r) &&
            Deliver(r, uFlags, hWnd, &ret))
            return ret;
    }
    t_passThrough++;
    const BOOL ret =
        TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
    t_passThrough--;
    return ret;
}

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                int nReserved, HWND hWnd,
                                const RECT* prcRect) {
    if (ShouldTake(hMenu, hWnd)) {
        WheelResult r;
        BOOL ret = FALSE;
        if (RunWheel(hMenu, uFlags, x, y, hWnd, &r) &&
            Deliver(r, uFlags, hWnd, &ret))
            return ret;
    }
    t_passThrough++;
    const BOOL ret = TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved,
                                             hWnd, prcRect);
    t_passThrough--;
    return ret;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&WheelProc), &g_inst);
    WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook,
                                   &TrackPopupMenuEx_Original);
    WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenu_Hook,
                                   &TrackPopupMenu_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    // Picked up by the next wheel, on its own thread. Windhawk's values
    // replace anything chosen in "Wheel options".
    g_dropOverrides = true;
    g_cfgDirty = true;
}

void Wh_ModBeforeUninit() {
    g_unloading = true;  // an open wheel closes on its next frame
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    // Our code must not be running when the mod is unloaded, however long a
    // wheel takes to notice (a slow submenu handler, a message box). A wheel
    // that was open releases its own graphics as it leaves.
    while (g_busy) Sleep(10);
    ReleaseGraphics();
    if (g_classRegistered) UnregisterClassW(kWheelClass, g_inst);
}
