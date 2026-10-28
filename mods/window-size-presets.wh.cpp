// ==WindhawkMod==
// @id              window-size-presets
// @name            Window Size Presets
// @description     Resize any window to a preset size from a hotkey or by hovering/right-clicking a title bar button (inspired by Sizer)
// @version         1.0.0
// @author          Nuzza
// @github          https://github.com/Nuzza
// @include         windhawk.exe
// @compilerOptions -ldwmapi -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Size Presets

Resize any window to a preset size, natively, in the spirit of
[Sizer](https://www.brianapps.net/sizer4/).

![Window Size Presets Demo](https://i.imgur.com/DZwtRRD.gif)

## Ways to open the preset menu

* **Title bar button** - right-click and/or hover the minimise (or maximise)
  button. Configurable in the settings; right-click is the default.
* **Hotkey** - opens the menu for the foreground window at the mouse cursor.

## Presets

Add as many as you like in the settings. Each preset has:

* **Name** (optional) - label shown in the menu.
* **Width / Height** - in pixels or as a percentage of the monitor's work area.
  `0` keeps the current value. At least one of them must be non-zero, and the
  first preset with both set to 0 ends the list.
* **Position** - keep, centre, or snap to a corner of the work area.

A tick is shown next to the preset matching the window's current size and
position.

## Notes

* This is a tool mod: it runs in its own dedicated `windhawk.exe` process and
  doesn't inject into any other process.
* Windows running as administrator can't be resized unless Windhawk's tool
  process is also elevated (UIPI).
* On Windows 11, hovering the maximise button also opens the Snap Layouts
  flyout, so the minimise button is the better choice for hover.
* The [Winstant](https://windhawk.net/mods/winstant) mod also reacts to
  hovering the minimise button. If you use both, give them different buttons
  or triggers (for example, right-click for this mod).
* Changing any setting is applied immediately, without restarting the mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- presets:
  - - name: XGA
      $name: Name
      $description: Optional label shown in the menu
    - width: 1024
      $name: Width
      $description: Pixels or percent (see Unit). 0 keeps the current width.
    - height: 768
      $name: Height
      $description: Pixels or percent (see Unit). 0 keeps the current height.
    - unit: pixels
      $name: Unit
      $options:
      - pixels: Pixels
      - percent: Percent of monitor work area
    - position: keep
      $name: Position
      $options:
      - keep: Keep current top-left corner
      - center: Centre on monitor
      - topleft: Top-left of work area
      - topright: Top-right of work area
      - bottomleft: Bottom-left of work area
      - bottomright: Bottom-right of work area
  - - name: HD 720p
    - width: 1280
    - height: 720
    - unit: pixels
    - position: keep
  - - name: HD+ 900p
    - width: 1600
    - height: 900
    - unit: pixels
    - position: keep
  - - name: Full HD
    - width: 1920
    - height: 1080
    - unit: pixels
    - position: keep
  - - name: Left half
    - width: 50
    - height: 100
    - unit: percent
    - position: topleft
  - - name: Right half
    - width: 50
    - height: 100
    - unit: percent
    - position: topright
  - - name: Centred 80%
    - width: 80
    - height: 80
    - unit: percent
    - position: center
  $name: Size presets
  $description: Sizes offered in the preset menu.
- sizeMode: visible
  $name: What the preset size refers to
  $options:
  - visible: Visible window (ignores the invisible resize borders)
  - window: Whole window rectangle
  - client: Client area (excludes title bar and borders)
- keepOnScreen: true
  $name: Keep resized windows inside the work area
- titlebarButton: minimize
  $name: Title bar button that opens the menu
  $description: >-
    On Windows 11, hovering the maximise button also opens the Snap Layouts
    flyout.
  $options:
  - minimize: Minimise button
  - maximize: Maximise button
  - none: Disabled
- titlebarTrigger: rightclick
  $name: Title bar trigger
  $description: >-
    With hover, pausing on the button for the delay below opens the menu, so
    a slow click on the button may only dismiss the menu.
  $options:
  - rightclick: Right-click
  - hover: Hover
  - both: Hover or right-click
- hoverDelay: 450
  $name: Hover delay (ms)
- hotkey: Win+Alt+S
  $name: Hotkey
  $description: 'Examples: Ctrl+Alt+S, Win+Shift+F9. Leave empty to disable.'
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

enum class Unit { Pixels, Percent };
enum class Pos { Keep, Center, TopLeft, TopRight, BottomLeft, BottomRight };
enum class SizeMode { Visible, Window, Client };
enum class Button { None, Minimize, Maximize };
enum class Trigger { Hover, RightClick, Both };

struct Preset {
    std::wstring name;
    int width = 0;
    int height = 0;
    Unit unit = Unit::Pixels;
    Pos pos = Pos::Keep;
};

struct Settings {
    std::vector<Preset> presets;
    SizeMode sizeMode = SizeMode::Visible;
    bool keepOnScreen = true;
    Button button = Button::Minimize;
    Trigger trigger = Trigger::RightClick;
    int hoverDelay = 450;
    std::wstring hotkey;
};

// Only ever read and written on the host thread (see HostThreadProc), after
// the initial load, so no locking is needed.
static Settings g_settings;

static void LoadSettings() {
    Settings s;

    for (int i = 0; i < 255; i++) {
        int w = Wh_GetIntSetting(L"presets[%d].width", i);
        int h = Wh_GetIntSetting(L"presets[%d].height", i);
        if (w <= 0 && h <= 0) break;

        Preset p;
        p.name = WindhawkUtils::StringSetting::make(L"presets[%d].name", i).get();
        p.width = std::max(w, 0);
        p.height = std::max(h, 0);

        std::wstring unit =
            WindhawkUtils::StringSetting::make(L"presets[%d].unit", i).get();
        p.unit = unit == L"percent" ? Unit::Percent : Unit::Pixels;

        std::wstring pos =
            WindhawkUtils::StringSetting::make(L"presets[%d].position", i).get();
        if (pos == L"center") p.pos = Pos::Center;
        else if (pos == L"topleft") p.pos = Pos::TopLeft;
        else if (pos == L"topright") p.pos = Pos::TopRight;
        else if (pos == L"bottomleft") p.pos = Pos::BottomLeft;
        else if (pos == L"bottomright") p.pos = Pos::BottomRight;
        else p.pos = Pos::Keep;

        s.presets.push_back(std::move(p));
    }

    std::wstring mode = WindhawkUtils::StringSetting::make(L"sizeMode").get();
    s.sizeMode = mode == L"window"   ? SizeMode::Window
                 : mode == L"client" ? SizeMode::Client
                                     : SizeMode::Visible;

    s.keepOnScreen = Wh_GetIntSetting(L"keepOnScreen") != 0;

    std::wstring btn =
        WindhawkUtils::StringSetting::make(L"titlebarButton").get();
    s.button = btn == L"none"       ? Button::None
               : btn == L"maximize" ? Button::Maximize
                                    : Button::Minimize;

    std::wstring trig =
        WindhawkUtils::StringSetting::make(L"titlebarTrigger").get();
    s.trigger = trig == L"hover"  ? Trigger::Hover
                : trig == L"both" ? Trigger::Both
                                  : Trigger::RightClick;

    s.hoverDelay = std::clamp(Wh_GetIntSetting(L"hoverDelay"), 0, 5000);
    s.hotkey = WindhawkUtils::StringSetting::make(L"hotkey").get();

    g_settings = std::move(s);
}

// ---------------------------------------------------------------------------
// Sizing logic
// ---------------------------------------------------------------------------

struct Target {
    RECT outer{};              // rectangle to pass to SetWindowPos
    int dw = 0, dh = 0;        // requested size, in the measured rectangle
    int curW = 0, curH = 0;    // current size, in the measured rectangle
    RECT cur{};                // current window rectangle (position matching)
};

static bool ComputeTarget(HWND hwnd, const Preset& p, const Settings& s,
                          Target* t) {
    RECT wr, cr;
    if (!GetWindowRect(hwnd, &wr) || !GetClientRect(hwnd, &cr)) return false;

    RECT efb = wr;
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &efb,
                                     sizeof(efb)))) {
        efb = wr;
    }

    int mL = 0, mT = 0, mR = 0, mB = 0;  // invisible border margins
    if (s.sizeMode == SizeMode::Visible) {
        mL = efb.left - wr.left;
        mT = efb.top - wr.top;
        mR = wr.right - efb.right;
        mB = wr.bottom - efb.bottom;
    }

    HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!mon || !GetMonitorInfoW(mon, &mi)) return false;
    const RECT& wa = mi.rcWork;
    int waW = wa.right - wa.left;
    int waH = wa.bottom - wa.top;

    t->cur = wr;
    int wrW = wr.right - wr.left, wrH = wr.bottom - wr.top;
    int crW = cr.right - cr.left, crH = cr.bottom - cr.top;

    switch (s.sizeMode) {
        case SizeMode::Visible:
            t->curW = efb.right - efb.left;
            t->curH = efb.bottom - efb.top;
            break;
        case SizeMode::Window:
            t->curW = wrW;
            t->curH = wrH;
            break;
        case SizeMode::Client:
            t->curW = crW;
            t->curH = crH;
            break;
    }

    auto resolve = [&](int value, int workArea, int current) {
        if (value <= 0) return current;
        return p.unit == Unit::Percent ? workArea * value / 100 : value;
    };
    t->dw = resolve(p.width, waW, t->curW);
    t->dh = resolve(p.height, waH, t->curH);

    int ow, oh;
    switch (s.sizeMode) {
        case SizeMode::Visible:
            ow = t->dw + mL + mR;
            oh = t->dh + mT + mB;
            break;
        case SizeMode::Window:
            ow = t->dw;
            oh = t->dh;
            break;
        default:  // Client
            ow = t->dw + (wrW - crW);
            oh = t->dh + (wrH - crH);
            break;
    }

    // Work in terms of the visible rectangle for positioning.
    int vw = ow - mL - mR;
    int vh = oh - mT - mB;
    int vx, vy;
    switch (p.pos) {
        case Pos::Center:
            vx = wa.left + (waW - vw) / 2;
            vy = wa.top + (waH - vh) / 2;
            break;
        case Pos::TopLeft:
            vx = wa.left;
            vy = wa.top;
            break;
        case Pos::TopRight:
            vx = wa.right - vw;
            vy = wa.top;
            break;
        case Pos::BottomLeft:
            vx = wa.left;
            vy = wa.bottom - vh;
            break;
        case Pos::BottomRight:
            vx = wa.right - vw;
            vy = wa.bottom - vh;
            break;
        default:  // Keep
            vx = wr.left + mL;
            vy = wr.top + mT;
            break;
    }

    if (s.keepOnScreen) {
        if (vx + vw > wa.right) vx = wa.right - vw;
        if (vy + vh > wa.bottom) vy = wa.bottom - vh;
        if (vx < wa.left) vx = wa.left;
        if (vy < wa.top) vy = wa.top;
    }

    t->outer.left = vx - mL;
    t->outer.top = vy - mT;
    t->outer.right = t->outer.left + ow;
    t->outer.bottom = t->outer.top + oh;
    return true;
}

static void ApplyPreset(HWND hwnd, const Preset& p, const Settings& s) {
    if (IsIconic(hwnd) || IsZoomed(hwnd)) {
        DWORD_PTR ignored;
        SendMessageTimeoutW(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0,
                            SMTO_ABORTIFHUNG, 500, &ignored);
    }

    Target t;
    if (!ComputeTarget(hwnd, p, s, &t)) return;

    // Async so a busy app can never block the host thread.
    SetWindowPos(hwnd, nullptr, t.outer.left, t.outer.top,
                 t.outer.right - t.outer.left, t.outer.bottom - t.outer.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

static std::wstring EscapeMenuText(const std::wstring& in) {
    std::wstring out;
    for (wchar_t c : in) {
        out += c;
        if (c == L'&') out += L'&';
    }
    return out;
}

// Builds a popup menu whose command IDs are 1 + preset index.
static HMENU BuildPresetMenu(HWND target, const Settings& s) {
    HMENU menu = CreatePopupMenu();
    if (!menu) return nullptr;

    if (s.presets.empty()) {
        AppendMenuW(menu, MF_STRING | MF_GRAYED, 0,
                    L"No presets configured");
        return menu;
    }

    // ApplyPreset restores minimized/maximized windows first, so for those the
    // current size is not what a "keep" dimension would end up as.
    bool restoredSizeUnknown = IsIconic(target) || IsZoomed(target);
    bool ticked = false;  // tick at most one preset

    for (size_t i = 0; i < s.presets.size(); i++) {
        const Preset& p = s.presets[i];
        Target t;
        bool ok = ComputeTarget(target, p, s, &t);

        std::wstring label = EscapeMenuText(p.name);
        if (ok) {
            if (!label.empty()) label += L'\t';
            label += (p.width <= 0 && restoredSizeUnknown)
                         ? std::wstring(L"\u2013")
                         : std::to_wstring(t.dw);
            label += L" \u00D7 ";
            label += (p.height <= 0 && restoredSizeUnknown)
                         ? std::wstring(L"\u2013")
                         : std::to_wstring(t.dh);
        } else if (label.empty()) {
            label = L"?";
        }

        UINT flags = MF_STRING;
        // A preset is "current" only if both size and position match what
        // applying it would produce, so e.g. "Left half" and "Right half"
        // are never both ticked.
        if (ok && !restoredSizeUnknown && !ticked &&
            std::abs(t.outer.left - t.cur.left) <= 2 &&
            std::abs(t.outer.top - t.cur.top) <= 2 &&
            std::abs(t.outer.right - t.cur.right) <= 2 &&
            std::abs(t.outer.bottom - t.cur.bottom) <= 2) {
            flags |= MF_CHECKED;
            ticked = true;
        }
        AppendMenuW(menu, flags, 1 + (UINT)i, label.c_str());
    }
    return menu;
}

// ---------------------------------------------------------------------------
// Host thread: title bar button, hotkey and menu
// ---------------------------------------------------------------------------

static const UINT WM_APP_MOUSEMOVE = WM_APP + 1;
static const UINT WM_APP_RBUTTON = WM_APP + 2;
static const UINT WM_APP_QUIT = WM_APP + 3;
static const UINT WM_APP_MENUCLICK = WM_APP + 4;
static const UINT WM_APP_RELOAD = WM_APP + 5;
static const UINT_PTR kHoverTimer = 1;
static const int kHotkeyId = 1;

static HANDLE g_hostThread = nullptr;
static HANDLE g_hostReady = nullptr;
static HWND g_hostWnd = nullptr;
static HHOOK g_mouseHook = nullptr;
static bool g_hotkeyRegistered = false;

static std::atomic<bool> g_movePending{false};
static std::atomic<bool> g_menuOpen{false};
static std::atomic<LONG> g_ptX{0}, g_ptY{0};
static std::atomic<HWND> g_overButton{nullptr};
static bool g_wantRightClick = false;
static bool g_rightSwallowed = false;
static HWND g_pendingHover = nullptr;
static HWND g_suppressHover = nullptr;

static std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t");
    return s.substr(a, b - a + 1);
}

static UINT KeyFromName(const std::wstring& tok) {
    if (tok.size() == 1) {
        wchar_t c = (wchar_t)towupper(tok[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) return c;
        return 0;
    }
    if (tok[0] == L'f') {
        int n = _wtoi(tok.c_str() + 1);
        if (n >= 1 && n <= 24) return VK_F1 + (n - 1);
    }
    struct {
        const wchar_t* name;
        UINT vk;
    } static const names[] = {
        {L"space", VK_SPACE},   {L"tab", VK_TAB},      {L"enter", VK_RETURN},
        {L"return", VK_RETURN}, {L"esc", VK_ESCAPE},   {L"escape", VK_ESCAPE},
        {L"up", VK_UP},         {L"down", VK_DOWN},    {L"left", VK_LEFT},
        {L"right", VK_RIGHT},   {L"home", VK_HOME},    {L"end", VK_END},
        {L"pageup", VK_PRIOR},  {L"pgup", VK_PRIOR},   {L"pagedown", VK_NEXT},
        {L"pgdn", VK_NEXT},     {L"insert", VK_INSERT}, {L"delete", VK_DELETE},
        {L"del", VK_DELETE},    {L"backspace", VK_BACK},
    };
    for (auto& n : names) {
        if (tok == n.name) return n.vk;
    }
    return 0;
}

static bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;
    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find(L'+', start);
        if (end == std::wstring::npos) end = text.size();
        std::wstring tok = Lower(Trim(text.substr(start, end - start)));
        start = end + 1;
        if (tok.empty()) continue;

        if (tok == L"ctrl" || tok == L"control") *mods |= MOD_CONTROL;
        else if (tok == L"alt") *mods |= MOD_ALT;
        else if (tok == L"shift") *mods |= MOD_SHIFT;
        else if (tok == L"win" || tok == L"windows" || tok == L"super" ||
                 tok == L"meta")
            *mods |= MOD_WIN;
        else {
            if (*vk) return false;
            *vk = KeyFromName(tok);
            if (!*vk) return false;
        }
    }
    return *vk != 0;
}

// Returns the top-level window if the point is over the given caption button.
static HWND HitTestCaptionButton(POINT pt, Button button) {
    HWND h = WindowFromPoint(pt);
    if (!h) return nullptr;
    HWND root = GetAncestor(h, GA_ROOT);
    if (!root || root == g_hostWnd) return nullptr;

    RECT wr;
    if (!GetWindowRect(root, &wr)) return nullptr;
    if (pt.y < wr.top || pt.y - wr.top > 150) return nullptr;  // cheap filter

    LONG_PTR style = GetWindowLongPtrW(root, GWL_STYLE);
    if (!(style & WS_THICKFRAME)) return nullptr;
    if (IsHungAppWindow(root)) return nullptr;

    // No SMTO_BLOCK: this thread owns the low-level mouse hook, whose
    // callbacks must still be delivered while we wait on a busy app.
    DWORD_PTR res = 0;
    if (!SendMessageTimeoutW(root, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y),
                             SMTO_ABORTIFHUNG, 50, &res)) {
        return nullptr;
    }

    LRESULT want = button == Button::Minimize ? HTMINBUTTON : HTMAXBUTTON;
    return (LRESULT)res == want ? root : nullptr;
}

// Popup menus only dismiss on an outside click / Esc if their owner window is
// the foreground window. Windows allows SetForegroundWindow when the calling
// process received the last input event; sending an empty input first makes
// that true, without attaching to another app's input queue.
static void ForceForeground(HWND hwnd) {
    if (GetForegroundWindow() == hwnd) return;

    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));
    SetForegroundWindow(hwnd);
}

// Safety net: if a click lands outside the menu, close the menu ourselves.
static void OnMenuClick() {
    if (!g_menuOpen) return;
    POINT pt = {g_ptX.load(), g_ptY.load()};
    HWND h = WindowFromPoint(pt);
    wchar_t cls[16] = {};
    if (h) GetClassNameW(h, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"#32768") != 0) {  // not a menu window
        EndMenu();
    }
}

static void ShowMenuFor(HWND target, POINT anchor, bool fromTitlebar) {
    if (g_menuOpen.exchange(true)) return;

    // Copy: a settings reload is processed on this thread while the menu's
    // modal loop runs, and must not invalidate the presets used below.
    const Settings s = g_settings;

    HMENU menu = BuildPresetMenu(target, s);
    if (!menu) {
        g_menuOpen = false;
        return;
    }

    int x = anchor.x;
    int y = anchor.y;
    UINT flags = TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON | TPM_TOPALIGN;

    if (fromTitlebar) {
        // Drop the menu just below the caption buttons, right-aligned to them.
        RECT bounds = {}, wr = {};
        if (SUCCEEDED(DwmGetWindowAttribute(target,
                                            DWMWA_CAPTION_BUTTON_BOUNDS,
                                            &bounds, sizeof(bounds))) &&
            bounds.bottom > 0 && bounds.bottom < 300 &&
            GetWindowRect(target, &wr)) {
            x = wr.left + bounds.right;
            y = wr.top + bounds.bottom;
        } else {
            y = anchor.y + 16;
        }
        flags |= TPM_RIGHTALIGN;
    } else {
        flags |= TPM_LEFTALIGN;
    }

    HWND prevForeground = GetForegroundWindow();
    ForceForeground(g_hostWnd);

    int cmd = TrackPopupMenu(menu, flags, x, y, 0, g_hostWnd, nullptr);
    PostMessageW(g_hostWnd, WM_NULL, 0, 0);
    DestroyMenu(menu);
    g_menuOpen = false;

    if (prevForeground && prevForeground != g_hostWnd &&
        IsWindow(prevForeground)) {
        SetForegroundWindow(prevForeground);
    }

    if (cmd > 0 && (size_t)cmd <= s.presets.size() && IsWindow(target)) {
        ApplyPreset(target, s.presets[cmd - 1], s);
    }
}

static void OnMouseMove(HWND host) {
    if (g_menuOpen) return;
    const Settings& s = g_settings;
    if (s.button == Button::None) return;

    POINT pt = {g_ptX.load(), g_ptY.load()};
    HWND root = HitTestCaptionButton(pt, s.button);
    g_overButton = root;

    if (!root) {
        KillTimer(host, kHoverTimer);
        g_pendingHover = nullptr;
        g_suppressHover = nullptr;
        return;
    }

    if (s.trigger == Trigger::RightClick) return;
    if (root == g_suppressHover) return;  // menu was just dismissed here

    if (root != g_pendingHover) {
        g_pendingHover = root;
        SetTimer(host, kHoverTimer, (UINT)std::max(s.hoverDelay, 1), nullptr);
    }
}

static void OnHoverTimer(HWND host) {
    KillTimer(host, kHoverTimer);
    HWND pending = g_pendingHover;
    g_pendingHover = nullptr;
    if (!pending || g_menuOpen) return;

    POINT pt;
    GetCursorPos(&pt);
    if (HitTestCaptionButton(pt, g_settings.button) != pending) return;

    ShowMenuFor(pending, pt, true);
    g_suppressHover = pending;
}

static void OnRightClick() {
    HWND root = g_overButton.load();
    if (!root || g_menuOpen) return;
    POINT pt;
    GetCursorPos(&pt);
    ShowMenuFor(root, pt, true);
    g_suppressHover = root;
}

static void OnHotkey() {
    if (g_menuOpen) return;
    HWND fg = GetForegroundWindow();
    if (!fg) return;
    HWND root = GetAncestor(fg, GA_ROOT);
    if (!root || root == g_hostWnd || !IsWindowVisible(root)) return;
    if (root == GetShellWindow() || root == GetDesktopWindow()) return;

    wchar_t cls[64] = {};
    GetClassNameW(root, cls, ARRAYSIZE(cls));
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
        !wcscmp(cls, L"Shell_TrayWnd") ||
        !wcscmp(cls, L"Shell_SecondaryTrayWnd")) {
        return;
    }

    POINT pt;
    GetCursorPos(&pt);
    ShowMenuFor(root, pt, false);
}

static LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam,
                                          LPARAM lParam) {
    if (code == HC_ACTION) {
        const MSLLHOOKSTRUCT* ms = (const MSLLHOOKSTRUCT*)lParam;
        if (g_menuOpen) {
            // Let the menu's own loop handle everything, but notify the host
            // about clicks so it can dismiss the menu on an outside click.
            if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
                wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN) {
                g_ptX = ms->pt.x;
                g_ptY = ms->pt.y;
                PostMessageW(g_hostWnd, WM_APP_MENUCLICK, 0, 0);
            }
        } else if (wParam == WM_MOUSEMOVE) {
            g_ptX = ms->pt.x;
            g_ptY = ms->pt.y;
            if (!g_movePending.exchange(true)) {
                PostMessageW(g_hostWnd, WM_APP_MOUSEMOVE, 0, 0);
            }
        } else if (g_wantRightClick) {
            if (wParam == WM_RBUTTONDOWN && g_overButton.load()) {
                g_rightSwallowed = true;
                return 1;
            }
            if (wParam == WM_RBUTTONUP && g_rightSwallowed) {
                g_rightSwallowed = false;
                PostMessageW(g_hostWnd, WM_APP_RBUTTON, 0, 0);
                return 1;
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// (Re)loads the settings and re-creates everything that depends on them: the
// hotkey registration and the mouse hook. Runs only on the host thread.
static void ApplyHostSettings(HWND host) {
    KillTimer(host, kHoverTimer);
    g_pendingHover = nullptr;
    g_suppressHover = nullptr;
    g_overButton = nullptr;
    g_rightSwallowed = false;

    if (g_hotkeyRegistered) {
        UnregisterHotKey(host, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }

    LoadSettings();
    const Settings& s = g_settings;

    if (!s.hotkey.empty()) {
        UINT mods, vk;
        if (!ParseHotkey(s.hotkey, &mods, &vk)) {
            Wh_Log(L"Could not parse hotkey \"%s\"", s.hotkey.c_str());
        } else if (!RegisterHotKey(host, kHotkeyId, mods | MOD_NOREPEAT, vk)) {
            Wh_Log(L"RegisterHotKey failed (%u); is the combination in use?",
                   GetLastError());
        } else {
            g_hotkeyRegistered = true;
        }
    }

    g_wantRightClick = s.button != Button::None &&
                       (s.trigger == Trigger::RightClick ||
                        s.trigger == Trigger::Both);
    if (s.button != Button::None) {
        g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc,
                                        (HINSTANCE)&__ImageBase, 0);
        if (!g_mouseHook) Wh_Log(L"Failed to install mouse hook");
    }
}

static LRESULT CALLBACK HostWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam) {
    switch (msg) {
        case WM_APP_MOUSEMOVE:
            g_movePending = false;
            OnMouseMove(hwnd);
            return 0;
        case WM_APP_RBUTTON:
            OnRightClick();
            return 0;
        case WM_APP_MENUCLICK:
            OnMenuClick();
            return 0;
        case WM_APP_RELOAD:
            ApplyHostSettings(hwnd);
            return 0;
        case WM_TIMER:
            if (wParam == kHoverTimer) OnHoverTimer(hwnd);
            return 0;
        case WM_HOTKEY:
            if (wParam == kHotkeyId) OnHotkey();
            return 0;
        case WM_APP_QUIT:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static DWORD WINAPI HostThreadProc(LPVOID) {
    // Real pixels for all coordinates, regardless of the process' awareness.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HINSTANCE inst = (HINSTANCE)&__ImageBase;
    const wchar_t kClass[] = L"WindhawkWindowSizePresetsHost";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = HostWndProc;
    wc.hInstance = inst;
    wc.lpszClassName = kClass;
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClass failed (%u)", GetLastError());
        SetEvent(g_hostReady);
        return 1;
    }

    HWND wnd = CreateWindowExW(WS_EX_TOOLWINDOW, kClass, L"", WS_POPUP, 0, 0,
                               0, 0, nullptr, nullptr, inst, nullptr);
    if (!wnd) {
        Wh_Log(L"Failed to create host window (%u)", GetLastError());
        UnregisterClassW(kClass, inst);
        SetEvent(g_hostReady);
        return 1;
    }
    g_hostWnd = wnd;

    ApplyHostSettings(wnd);
    SetEvent(g_hostReady);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    if (g_hotkeyRegistered) {
        UnregisterHotKey(wnd, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    g_hostWnd = nullptr;
    UnregisterClassW(kClass, inst);
    return 0;
}

static bool StartHost() {
    g_hostReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hostReady) return false;

    g_hostThread = CreateThread(nullptr, 0, HostThreadProc, nullptr, 0, nullptr);
    if (!g_hostThread) return false;

    // The thread signals this on success and on failure.
    WaitForSingleObject(g_hostReady, INFINITE);
    return g_hostWnd != nullptr;
}

static void StopHost() {
    if (g_hostThread) {
        if (g_hostWnd) PostMessageW(g_hostWnd, WM_APP_QUIT, 0, 0);
        WaitForSingleObject(g_hostThread, INFINITE);
        CloseHandle(g_hostThread);
        g_hostThread = nullptr;
    }
    if (g_hostReady) {
        CloseHandle(g_hostReady);
        g_hostReady = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Tool mod callbacks
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    if (!StartHost()) {
        StopHost();
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    // Called on a different thread; the host thread owns g_settings, the
    // hotkey and the mouse hook, so let it do the reload.
    HWND wnd = g_hostWnd;
    if (wnd) PostMessageW(wnd, WM_APP_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    StopHost();
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
