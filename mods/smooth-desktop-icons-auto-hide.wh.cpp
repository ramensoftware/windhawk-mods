// ==WindhawkMod==
// @id              smooth-desktop-icons-auto-hide
// @name            Smooth Desktop Icons Auto-Hide
// @description     Smoothly auto-hide Windows desktop icons with click-to-show, double-click-to-hide, drag reveal and configurable fade animation.
// @version         0.14.2
// @author          HeyOkay
// @github           https://github.com/HeyOkay
// @license          MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -luxtheme -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Smooth Desktop Icons Auto-Hide

Automatically hides desktop icons when you're not using the desktop and smoothly fades them back in when you are.

![Demo](https://raw.githubusercontent.com/HeyOkay/SmoothDesktopAutoHide/main/assets/demo.gif)

*Dragging a file onto the desktop reveals the icons with a smooth fade-in; they fade out once the desktop loses focus.*

## Features

- **Auto-hide.** Icons fade out a few seconds after you leave the desktop. The countdown is paused while the desktop is the active surface, so icons never disappear while you're looking at them.
- **Click to show.** A single click on empty desktop reveals the icons. Mouse movement alone never does.
- **Double-click to hide.** Double-click empty desktop to hide the icons right away.
- **Drag & drop.** Dragging a file onto the desktop reveals the icons before you drop it.
- **Win+D and Show desktop.** Showing the desktop with `Win+D`, the taskbar's **Show desktop** button, or by minimizing the last window reveals the icons; leaving it starts a fresh countdown.
- **Smooth fade.** Configurable duration, no wallpaper dimming, works on all Virtual Desktops.
- **Safe while hidden.** Hidden icons can't be clicked and are deselected. Pressing a key on the desktop (including Delete, F2, Ctrl+A) reveals them instead of acting on invisible files.

## How this differs from similar mods

- **[ZenDesktop: Desktop Icon Toggle and Auto-Hide](https://windhawk.net/mods/zen-desktop-toggle-icons)** toggles icons by double-click and hides them after N seconds without any input anywhere in the system (`GetLastInputInfo()`), restoring them on any input. This mod instead ties auto-hide to the desktop itself: the countdown starts when the desktop stops being the active surface and is paused while you're on it. It also adds a smooth fade, reveals the icons with a *single* click on empty desktop, and reveals them when a file is dragged onto the desktop.
- **[Desktop Icon Section Auto-Hide & Fluent Hover Reveal](https://windhawk.net/mods/desktop-icon-section-autohide)** reveals icons on hover and offers modes, per-app pinning and click-and-hold peek. This mod deliberately does **not** react to mouse movement: icons appear only on an explicit action (click, drag, desktop activation) and stay while the desktop is in use. It has no modes or whitelist and only three settings.

## How it works

The desktop icon list is never hidden or made transparent as a window. Instead, the mod blends the icons, labels and selection highlights onto the wallpaper with the current opacity while Explorer paints them. Showing and hiding react to events (clicks, drag & drop, the desktop becoming the active window) rather than constant polling, so the mod does nothing while idle. When the mod is disabled, everything is restored to Explorer's normal state.

## Settings

- **Enable Auto-hide** — hide icons automatically after leaving the desktop. When off, icons start visible and are hidden only by double-clicking empty desktop.
- **Hide after seconds** — delay before icons are hidden (1–60, default 5).
- **Animation duration (ms)** — fade duration (50–1000, default 250).

## Credits

- The paint-time opacity technique (blending icon and label drawing instead of making the ListView layered) follows [desktop-icon-section-autohide](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/desktop-icon-section-autohide.wh.cpp) by Piyush Das, which builds on [desktop-icons-transparency](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/desktop-icons-transparency.wh.cpp) by zed712969-crypto.
- Inspired by [ZenDesktop: Desktop Icon Toggle and Auto-Hide](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/zen-desktop-toggle-icons.wh.cpp) by Lanbo, including its desktop window discovery approach (CreateWindowExW hook, `Progman`/`WorkerW` enumeration, subclassing `SHELLDLL_DefView` and `SysListView32`).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- autoHideEnabled: true
  $name: Enable Auto-hide
  $description: "Automatically hide desktop icons after inactivity. When off, icons start visible and are hidden only by double-clicking empty desktop."
- autoHideDelay: 5
  $name: Hide after seconds
  $description: "Inactivity time before icons are hidden. Range: 1-60."
- animationDuration: 250
  $name: Animation duration (ms)
  $description: "Fade duration. Range: 50-1000 ms."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <oleidl.h>
#include <uxtheme.h>
#include <mmsystem.h>
#include <atomic>
#include <windhawk_utils.h>

static constexpr UINT_PTR kTimerAutoHide = 0x53444901;
static constexpr UINT_PTR kTimerAnimation = 0x53444902;
// Runs only while a desktop context menu is open, to notice the menu closing
// when WM_EXITMENULOOP doesn't reach the desktop view.
static constexpr UINT_PTR kTimerMenuCheck = 0x53444903;
static constexpr UINT kMenuCheckMs = 150;
// Ignore "menu not visible yet" for this long after the menu was requested;
// building a shell context menu can take a moment.
static constexpr DWORD kMenuGraceMs = 400;
// Safety cap: never treat a menu as open for longer than this, so a stray
// popup window can't block auto-hide indefinitely.
static constexpr DWORD kMenuMaxMs = 120000;
// Animation frame interval. Progress is time based, so this only sets how
// often a new frame is drawn.
static constexpr UINT kAnimFrameMs = 8;
static UINT g_msgRefresh = 0;
static UINT g_msgShow = 0;
static UINT g_msgHide = 0;
static UINT g_msgUninit = 0;
static UINT g_msgDragEnter = 0;
static UINT g_msgDragLeave = 0;

struct Settings {
    bool autoHideEnabled = true;
    int autoHideDelay = 5;
    int animationDuration = 250;
} g_settings;

enum class IconState {
    Hidden,
    Showing,
    Visible,
    Hiding,
};

struct DesktopState {
    HWND shell = nullptr;
    HWND list = nullptr;

    IconState state = IconState::Hidden;

    // Per-desktop alpha animation state.
    BYTE alpha = 0;
    BYTE animFrom = 0;
    BYTE animTo = 0;
    LONGLONG animStart = 0;
    float animDurationMs = 0.0f;
    bool precisionHeld = false;

    bool dragActive = false;
    bool interactionActive = false;
    bool suppressDesktopFocusUntilClick = false;
    bool contextMenuActive = false;
    // Once the user clicks the desktop, keep auto-hide paused while the
    // desktop remains the active/focused surface. This lets the user inspect
    // icons without the list disappearing underneath their eyes.
    bool desktopFocusActive = false;
    // Prevent repeated StartAutoHide() calls from restarting the countdown.
    bool autoHideTimerArmed = false;

    DWORD menuStartTick = 0;

    // Whether the desktop was the foreground surface at the previous
    // EVENT_SYSTEM_FOREGROUND. Used to tell a real activation of the desktop
    // (Win+D, last window minimized) from focus returning after a menu.
    bool lastForegroundDesktop = false;

    // EVENT_SYSTEM_FOREGROUND hook, installed and removed on the desktop
    // thread.
    HWINEVENTHOOK foregroundHook = nullptr;

    // If a show request came from a real desktop click, keyboard focus must
    // move to the ListView only after the asynchronous show has completed.
    bool focusListAfterShow = false;

    // Manual double-click fallback for window classes that don't advertise
    // CS_DBLCLKS (notably some SHELLDLL_DefView configurations).
    bool manualDoubleClickArmed = false;
    DWORD manualClickTime = 0;
    POINT manualClickPoint{};
    HWND manualClickWindow = nullptr;

};

// All DesktopState entries are created, changed and read on the desktop
// window's own thread. Only the desktop SHELLDLL_DefView is ever subclassed.
static DesktopState g_states[4]{};
static int g_stateCount = 0;

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t g_CreateWindowExW = nullptr;

static DesktopState* FindStateByShell(HWND shell);
static void CancelAutoHide(DesktopState* state);
static void StartAutoHide(DesktopState* state);
static void ShowIcons(DesktopState* state);
static void HideIcons(DesktopState* state);
static void RequestShowIcons(DesktopState* state, bool focusList = false);
static void RequestHideIcons(DesktopState* state);
static bool IsEmptyListPoint(HWND list, LPARAM lParam);
static bool IsEmptyDesktopPoint(DesktopState* state, LPARAM shellLParam);

static DesktopState* FindStateByShell(HWND shell) {
    if (!shell)
        return nullptr;
    for (int i = 0; i < g_stateCount; ++i) {
        if (g_states[i].shell == shell)
            return &g_states[i];
    }
    return nullptr;
}

static DesktopState* FindStateByList(HWND list) {
    if (!list)
        return nullptr;
    for (int i = 0; i < g_stateCount; ++i) {
        if (g_states[i].list == list)
            return &g_states[i];
    }
    return nullptr;
}

static DesktopState* GetOrCreateState(HWND shell) {
    if (auto* existing = FindStateByShell(shell))
        return existing;

    // Reuse a slot whose DefView has been destroyed.
    DesktopState* state = nullptr;
    for (int i = 0; i < g_stateCount; ++i) {
        if (!g_states[i].shell) {
            state = &g_states[i];
            break;
        }
    }

    if (!state) {
        if (g_stateCount >= static_cast<int>(_countof(g_states)))
            return nullptr;
        state = &g_states[g_stateCount++];
    }

    *state = {};
    state->shell = shell;
    return state;
}

static bool IsClass(HWND hwnd, const wchar_t* className) {
    if (!hwnd)
        return false;

    wchar_t actual[128]{};

    return GetClassNameW(
               hwnd,
               actual,
               _countof(actual)) &&
           wcscmp(actual, className) == 0;
}

static HWND FindDesktopList(HWND shell) {
    if (!IsClass(shell, L"SHELLDLL_DefView"))
        return nullptr;

    return FindWindowExW(
        shell,
        nullptr,
        L"SysListView32",
        nullptr);
}


static bool IsWindowOwnedByCurrentProcess(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return false;

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    return processId != 0 &&
           processId == GetCurrentProcessId();
}

static bool IsExpectedDesktopShell(HWND shell) {
    return IsWindowOwnedByCurrentProcess(shell) &&
           IsClass(shell, L"SHELLDLL_DefView");
}

static bool SupportsNativeDoubleClick(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return false;

    const ULONG_PTR classStyle =
        static_cast<ULONG_PTR>(
            GetClassLongPtrW(
                hwnd,
                GCL_STYLE));

    return (classStyle & CS_DBLCLKS) != 0;
}

static bool IsManualDoubleClick(
    DesktopState* state,
    HWND hwnd,
    LPARAM lParam,
    bool pointIsShellCoordinates) {

    if (!state || !hwnd)
        return false;

    // Native WM_LBUTTONDBLCLK is preferred whenever the class supports it.
    if (SupportsNativeDoubleClick(hwnd)) {
        if (state->manualClickWindow == hwnd)
            state->manualDoubleClickArmed = false;
        return false;
    }

    const bool emptyPoint =
        pointIsShellCoordinates
            ? IsEmptyDesktopPoint(state, lParam)
            : IsEmptyListPoint(hwnd, lParam);

    if (!emptyPoint) {
        state->manualDoubleClickArmed = false;
        state->manualClickWindow = nullptr;
        return false;
    }

    POINT screenPoint{
        GET_X_LPARAM(lParam),
        GET_Y_LPARAM(lParam)
    };

    if (!ClientToScreen(hwnd, &screenPoint)) {
        state->manualDoubleClickArmed = false;
        state->manualClickWindow = nullptr;
        return false;
    }

    const DWORD now = GetTickCount();
    const DWORD elapsed = now - state->manualClickTime;
    const int maxDx = GetSystemMetrics(SM_CXDOUBLECLK) / 2;
    const int maxDy = GetSystemMetrics(SM_CYDOUBLECLK) / 2;

    int deltaX =
        screenPoint.x -
        state->manualClickPoint.x;
    if (deltaX < 0)
        deltaX = -deltaX;

    int deltaY =
        screenPoint.y -
        state->manualClickPoint.y;
    if (deltaY < 0)
        deltaY = -deltaY;

    const bool isDoubleClick =
        state->manualDoubleClickArmed &&
        state->manualClickWindow == hwnd &&
        elapsed <= GetDoubleClickTime() &&
        deltaX <= maxDx &&
        deltaY <= maxDy;

    if (isDoubleClick) {
        state->manualDoubleClickArmed = false;
        state->manualClickWindow = nullptr;
        return true;
    }

    state->manualDoubleClickArmed = true;
    state->manualClickTime = now;
    state->manualClickPoint = screenPoint;
    state->manualClickWindow = hwnd;
    return false;
}

static void LoadSettings() {
    g_settings.autoHideEnabled =
        Wh_GetIntSetting(L"autoHideEnabled") != 0;

    g_settings.autoHideDelay =
        Wh_GetIntSetting(L"autoHideDelay");

    if (g_settings.autoHideDelay < 1)
        g_settings.autoHideDelay = 1;

    if (g_settings.autoHideDelay > 60)
        g_settings.autoHideDelay = 60;

    g_settings.animationDuration =
        Wh_GetIntSetting(L"animationDuration");

    if (g_settings.animationDuration < 50)
        g_settings.animationDuration = 50;

    if (g_settings.animationDuration > 1000)
        g_settings.animationDuration = 1000;

    Wh_Log(
        L"Settings: autoHide=%d delay=%d animation=%d",
        static_cast<int>(g_settings.autoHideEnabled),
        g_settings.autoHideDelay,
        g_settings.animationDuration);
}

static void CancelAutoHide(DesktopState* state) {
    if (state && state->shell) {
        KillTimer(
            state->shell,
            kTimerAutoHide);
        state->autoHideTimerArmed = false;
    }
}

static void StartAutoHide(DesktopState* state) {
    if (!state ||
        !state->shell ||
        !g_settings.autoHideEnabled)
        return;

    // Keep the timer alive even while Desktop is active or a context menu is
    // open. The timer callback decides whether it is currently allowed to hide.
    // Explorer doesn't always deliver every menu-loop transition to the
    // desktop view, so timer creation must not depend on those messages alone.
    // Callers invoke this on many events; never restart a running
    // countdown, or a 5-second timer would effectively never expire.
    if (state->autoHideTimerArmed)
        return;

    SetTimer(
        state->shell,
        kTimerAutoHide,
        static_cast<UINT>(
            g_settings.autoHideDelay * 1000),
        nullptr);

    state->autoHideTimerArmed = true;
}

// ---------------------------------------------------------------------------
// Opacity rendering
//
// The desktop ListView is never hidden with ShowWindow and never made
// WS_EX_LAYERED. While it paints, icon, label and highlight drawing calls are
// intercepted and blended onto the already painted wallpaper with the current
// opacity. At opacity 0 nothing is drawn, so the ListView is simply
// transparent. Without a layered surface and without show/hide transitions
// there is nothing Explorer can expose as a black background during Virtual
// Desktop creation or switching.
// ---------------------------------------------------------------------------

static std::atomic<bool> g_unloading{false};

// Opacity the paint hooks apply on the current thread; -1 means "not inside a
// desktop ListView paint", so every hook passes straight through.
static thread_local int tl_paintAlpha = -1;
// Nesting depth of our own wrapped drawing. Calls made by the original
// function (e.g. ImageList -> GdiAlphaBlend, DrawShadowText -> ExtTextOutW)
// draw into the offscreen DC unmodified so alpha is applied exactly once.
static thread_local int tl_drawDepth = 0;

using GdiAlphaBlend_t = BOOL(WINAPI*)(
    HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
static GdiAlphaBlend_t g_GdiAlphaBlend = nullptr;

using ExtTextOutW_t = decltype(&ExtTextOutW);
static ExtTextOutW_t g_ExtTextOutW = nullptr;

using DrawThemeBackground_t = decltype(&DrawThemeBackground);
static DrawThemeBackground_t g_DrawThemeBackground = nullptr;

using DrawShadowText_t = int(WINAPI*)(
    HDC, LPCWSTR, UINT, RECT*, DWORD, COLORREF, COLORREF, int, int);
static DrawShadowText_t g_DrawShadowText = nullptr;

using ImageList_DrawIndirect_t = BOOL(WINAPI*)(IMAGELISTDRAWPARAMS*);
static ImageList_DrawIndirect_t g_ImageList_DrawIndirect = nullptr;

static std::atomic<bool> g_comctlHooked{false};
static std::atomic<bool> g_initDone{false};

// One offscreen buffer, used only on the desktop thread while it paints.
// Plain struct (no destructor) so nothing runs after the mod DLL unloads.
struct OffscreenDC {
    HDC dc;
    HBITMAP bmp;
    HBITMAP oldBmp;
    int w;
    int h;
};
static OffscreenDC g_offscreen{};

static void FreeOffscreen() {
    if (g_offscreen.dc) {
        if (g_offscreen.oldBmp)
            SelectObject(g_offscreen.dc, g_offscreen.oldBmp);
        if (g_offscreen.bmp)
            DeleteObject(g_offscreen.bmp);
        DeleteDC(g_offscreen.dc);
    }
    g_offscreen = {};
}

static HDC GetOffscreen(HDC ref, int w, int h) {
    if (!g_offscreen.dc) {
        g_offscreen.dc = CreateCompatibleDC(ref);
        if (!g_offscreen.dc)
            return nullptr;
    }

    if (!g_offscreen.bmp || w > g_offscreen.w || h > g_offscreen.h) {
        const int newW = w > g_offscreen.w ? w : g_offscreen.w;
        const int newH = h > g_offscreen.h ? h : g_offscreen.h;

        HBITMAP bmp = CreateCompatibleBitmap(
            ref, newW < 256 ? 256 : newW, newH < 128 ? 128 : newH);
        if (!bmp)
            return nullptr;

        HBITMAP old =
            static_cast<HBITMAP>(SelectObject(g_offscreen.dc, bmp));
        if (g_offscreen.bmp)
            DeleteObject(g_offscreen.bmp);
        else
            g_offscreen.oldBmp = old;

        g_offscreen.bmp = bmp;
        g_offscreen.w = newW < 256 ? 256 : newW;
        g_offscreen.h = newH < 128 ? 128 : newH;
    }

    return g_offscreen.dc;
}

static BOOL RealAlphaBlend(
    HDC dst, int x, int y, int w, int h,
    HDC src, int sx, int sy, int sw, int sh,
    BLENDFUNCTION bf) {
    return (g_GdiAlphaBlend ? g_GdiAlphaBlend : GdiAlphaBlend)(
        dst, x, y, w, h, src, sx, sy, sw, sh, bf);
}

static bool ShouldIntercept() {
    return tl_paintAlpha >= 0 && tl_drawDepth == 0 && !g_unloading;
}

// Copy what is already painted under rc into an offscreen DC, let draw()
// render into it using the same logical coordinates, then blend the result
// back with the requested constant alpha.
template <typename Draw>
static bool DrawWithOpacity(HDC hdc, RECT rc, int alpha, Draw&& draw) {
    const int w = rc.right - rc.left;
    const int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0 || w > 4096 || h > 4096)
        return false;

    HDC mem = GetOffscreen(hdc, w, h);
    if (!mem)
        return false;

    BitBlt(mem, 0, 0, w, h, hdc, rc.left, rc.top, SRCCOPY);

    POINT oldOrg{};
    SetViewportOrgEx(mem, -rc.left, -rc.top, &oldOrg);
    HGDIOBJ oldFont = SelectObject(mem, GetCurrentObject(hdc, OBJ_FONT));
    const COLORREF oldText = SetTextColor(mem, GetTextColor(hdc));
    const COLORREF oldBk = SetBkColor(mem, GetBkColor(hdc));
    const int oldBkMode = SetBkMode(mem, GetBkMode(hdc));
    const UINT oldAlign = SetTextAlign(mem, GetTextAlign(hdc));

    ++tl_drawDepth;
    draw(mem);
    --tl_drawDepth;

    SetTextAlign(mem, oldAlign);
    SetBkMode(mem, oldBkMode);
    SetBkColor(mem, oldBk);
    SetTextColor(mem, oldText);
    SelectObject(mem, oldFont);
    SetViewportOrgEx(mem, oldOrg.x, oldOrg.y, nullptr);

    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = static_cast<BYTE>(alpha);
    RealAlphaBlend(hdc, rc.left, rc.top, w, h, mem, 0, 0, w, h, bf);
    return true;
}

static BOOL WINAPI HookGdiAlphaBlend(
    HDC dst, int x, int y, int w, int h,
    HDC src, int sx, int sy, int sw, int sh,
    BLENDFUNCTION bf) {

    if (ShouldIntercept()) {
        const int a = tl_paintAlpha;
        if (a <= 0)
            return TRUE;
        bf.SourceConstantAlpha =
            static_cast<BYTE>((bf.SourceConstantAlpha * a + 127) / 255);
    }

    return g_GdiAlphaBlend(dst, x, y, w, h, src, sx, sy, sw, sh, bf);
}

static BOOL WINAPI HookImageList_DrawIndirect(IMAGELISTDRAWPARAMS* p) {
    if (!p || !ShouldIntercept())
        return g_ImageList_DrawIndirect(p);

    const int a = tl_paintAlpha;
    if (a <= 0)
        return TRUE;

    if (p->cbSize < sizeof(IMAGELISTDRAWPARAMS)) {
        // Old structure without fState/Frame: draw unmodified.
        return g_ImageList_DrawIndirect(p);
    }

    IMAGELISTDRAWPARAMS params = *p;
    const DWORD baseAlpha = (params.fState & ILS_ALPHA) ? params.Frame : 255;
    params.fState |= ILS_ALPHA;
    params.Frame = (baseAlpha * static_cast<DWORD>(a) + 127) / 255;

    ++tl_drawDepth;
    const BOOL result = g_ImageList_DrawIndirect(&params);
    --tl_drawDepth;
    return result;
}

static int WINAPI HookDrawShadowText(
    HDC hdc, LPCWSTR text, UINT cch, RECT* prc, DWORD flags,
    COLORREF crText, COLORREF crShadow, int ox, int oy) {

    if (!ShouldIntercept() || !prc || (flags & DT_CALCRECT))
        return g_DrawShadowText(
            hdc, text, cch, prc, flags, crText, crShadow, ox, oy);

    const int a = tl_paintAlpha;
    if (a <= 0)
        return 1;

    const int padX = (ox < 0 ? -ox : ox) + 4;
    const int padY = (oy < 0 ? -oy : oy) + 4;
    RECT rc{prc->left - padX, prc->top - padY,
            prc->right + padX, prc->bottom + padY};

    int result = 0;
    if (DrawWithOpacity(hdc, rc, a, [&](HDC mem) {
            result = g_DrawShadowText(
                mem, text, cch, prc, flags, crText, crShadow, ox, oy);
        }))
        return result;

    return g_DrawShadowText(
        hdc, text, cch, prc, flags, crText, crShadow, ox, oy);
}

static BOOL WINAPI HookExtTextOutW(
    HDC hdc, int x, int y, UINT options, const RECT* lprect,
    LPCWSTR str, UINT c, const INT* dx) {

    if (!ShouldIntercept())
        return g_ExtTextOutW(hdc, x, y, options, lprect, str, c, dx);

    const int a = tl_paintAlpha;
    if (a <= 0)
        return TRUE;

    const UINT align = GetTextAlign(hdc);
    if (align & TA_UPDATECP)
        return g_ExtTextOutW(hdc, x, y, options, lprect, str, c, dx);

    SIZE sz{};
    if (str && c) {
        if (options & ETO_GLYPH_INDEX)
            GetTextExtentPointI(
                hdc, reinterpret_cast<LPWORD>(const_cast<LPWSTR>(str)),
                static_cast<int>(c), &sz);
        else
            GetTextExtentPoint32W(hdc, str, static_cast<int>(c), &sz);
    }

    if (dx && c) {
        const UINT step = (options & ETO_PDY) ? 2 : 1;
        LONG sum = 0;
        for (UINT i = 0; i < c; ++i)
            sum += dx[i * step];
        if (sum > sz.cx)
            sz.cx = sum;
    }

    TEXTMETRICW tm{};
    GetTextMetricsW(hdc, &tm);
    if (tm.tmHeight > sz.cy)
        sz.cy = tm.tmHeight;

    int left = x;
    if ((align & TA_CENTER) == TA_CENTER)
        left = x - sz.cx / 2;
    else if (align & TA_RIGHT)
        left = x - sz.cx;

    int top = y;
    if ((align & TA_BASELINE) == TA_BASELINE)
        top = y - tm.tmAscent;
    else if (align & TA_BOTTOM)
        top = y - sz.cy;

    RECT rc{left - 3, top - 3,
            left + sz.cx + tm.tmOverhang + tm.tmMaxCharWidth / 4 + 3,
            top + sz.cy + 3};

    if (lprect) {
        if (options & ETO_OPAQUE)
            UnionRect(&rc, &rc, lprect);
        if (options & ETO_CLIPPED)
            IntersectRect(&rc, &rc, lprect);
    }

    BOOL result = TRUE;
    if (DrawWithOpacity(hdc, rc, a, [&](HDC mem) {
            result = g_ExtTextOutW(mem, x, y, options, lprect, str, c, dx);
        }))
        return result;

    return g_ExtTextOutW(hdc, x, y, options, lprect, str, c, dx);
}

static HRESULT WINAPI HookDrawThemeBackground(
    HTHEME theme, HDC hdc, int part, int stateId,
    LPCRECT pRect, LPCRECT pClip) {

    if (!ShouldIntercept() || !pRect)
        return g_DrawThemeBackground(theme, hdc, part, stateId, pRect, pClip);

    const int a = tl_paintAlpha;
    if (a <= 0)
        return S_OK;

    RECT rc = *pRect;
    if (pClip)
        IntersectRect(&rc, &rc, pClip);

    HRESULT result = S_OK;
    if (DrawWithOpacity(hdc, rc, a, [&](HDC mem) {
            result = g_DrawThemeBackground(
                theme, mem, part, stateId, pRect, pClip);
        }))
        return result;

    return g_DrawThemeBackground(theme, hdc, part, stateId, pRect, pClip);
}

// Hook the comctl32 that actually owns SysListView32 (the v6 side-by-side
// copy), not whichever comctl32 this DLL happens to be linked against.
static bool HookComctl32For(HWND list, bool applyNow) {
    if (g_comctlHooked)
        return true;

    // No new hooks once uninit has started.
    if (g_unloading)
        return false;

    HMODULE comctl = nullptr;
    if (list && IsWindow(list)) {
        comctl = reinterpret_cast<HMODULE>(
            GetClassLongPtrW(list, GCLP_HMODULE));
    }
    if (!comctl)
        return false;

    auto drawIndirect = reinterpret_cast<ImageList_DrawIndirect_t>(
        GetProcAddress(comctl, "ImageList_DrawIndirect"));
    auto drawShadow = reinterpret_cast<DrawShadowText_t>(
        GetProcAddress(comctl, "DrawShadowText"));

    bool any = false;
    if (drawIndirect &&
        WindhawkUtils::SetFunctionHook(
            drawIndirect, HookImageList_DrawIndirect,
            &g_ImageList_DrawIndirect))
        any = true;

    if (drawShadow &&
        WindhawkUtils::SetFunctionHook(
            drawShadow, HookDrawShadowText, &g_DrawShadowText))
        any = true;

    if (!any)
        return false;

    g_comctlHooked = true;
    if (applyNow)
        Wh_ApplyHookOperations();

    Wh_Log(L"comctl32 hooks set (module %p)", comctl);
    return true;
}

// Resolve GDI exports from gdi32full.dll, where they are implemented, so
// internal calls (e.g. DrawTextW -> ExtTextOutW for labels drawn without a
// shadow) also go through the hook. Fall back to gdi32.dll.
static FARPROC GetGdiProc(const char* name) {
    static const wchar_t* const kModules[] = {L"gdi32full.dll", L"gdi32.dll"};
    for (const wchar_t* module : kModules) {
        HMODULE gdi = GetModuleHandleW(module);
        FARPROC proc = gdi ? GetProcAddress(gdi, name) : nullptr;
        if (proc)
            return proc;
    }
    return nullptr;
}

static bool InstallGdiHooks() {
    auto alphaBlend =
        reinterpret_cast<GdiAlphaBlend_t>(GetGdiProc("GdiAlphaBlend"));
    auto extTextOut =
        reinterpret_cast<ExtTextOutW_t>(GetGdiProc("ExtTextOutW"));
    if (!extTextOut)
        extTextOut = ExtTextOutW;

    bool ok = true;

    if (!alphaBlend ||
        !WindhawkUtils::SetFunctionHook(
            alphaBlend, HookGdiAlphaBlend, &g_GdiAlphaBlend))
        ok = false;

    if (!WindhawkUtils::SetFunctionHook(
            extTextOut, HookExtTextOutW, &g_ExtTextOutW))
        ok = false;

    if (!WindhawkUtils::SetFunctionHook(
            DrawThemeBackground, HookDrawThemeBackground,
            &g_DrawThemeBackground))
        ok = false;

    return ok;
}

static void RepaintList(DesktopState* state) {
    if (!state || !state->list || !IsWindow(state->list))
        return;

    InvalidateRect(state->list, nullptr, FALSE);
    UpdateWindow(state->list);
}

static bool IsLabelEditActive(DesktopState* state) {
    if (!state || !state->list || !IsWindow(state->list))
        return false;

    HWND edit = reinterpret_cast<HWND>(
        SendMessageW(state->list, LVM_GETEDITCONTROL, 0, 0));
    return edit && IsWindowVisible(edit);
}

static void BeginTimerPrecision(DesktopState* state) {
    if (state && !state->precisionHeld) {
        timeBeginPeriod(1);
        state->precisionHeld = true;
    }
}

static void EndTimerPrecision(DesktopState* state) {
    if (state && state->precisionHeld) {
        timeEndPeriod(1);
        state->precisionHeld = false;
    }
}

// Hidden icons must not stay selected: a key or accelerator reaching the
// view (Delete, F2, Ctrl+X, Alt+Enter) would act on files the user can't see.
static void ClearSelection(DesktopState* state) {
    if (!state || !state->list || !IsWindow(state->list))
        return;

    LVITEMW item{};
    item.stateMask = LVIS_SELECTED;
    item.state = 0;
    SendMessageW(state->list, LVM_SETITEMSTATE, static_cast<WPARAM>(-1),
                 reinterpret_cast<LPARAM>(&item));
}

static void SetAlpha(
    DesktopState* state,
    BYTE alpha) {

    if (!state || !state->list)
        return;

    if (state->alpha == alpha)
        return;

    state->alpha = alpha;
    RepaintList(state);
}

static LONGLONG QpcNow() {
    LARGE_INTEGER v{};
    QueryPerformanceCounter(&v);
    return v.QuadPart;
}

static double QpcToMs(LONGLONG ticks) {
    static LONGLONG freq = 0;
    if (!freq) {
        LARGE_INTEGER f{};
        QueryPerformanceFrequency(&f);
        freq = f.QuadPart ? f.QuadPart : 1;
    }
    return static_cast<double>(ticks) * 1000.0 / static_cast<double>(freq);
}

static BYTE EaseAlpha(
    BYTE from,
    BYTE to,
    float t) {

    if (t <= 0.0f)
        return from;

    if (t >= 1.0f)
        return to;

    // Smoothstep easing.
    float eased =
        t * t * (3.0f - 2.0f * t);

    float value =
        static_cast<float>(from) +
        (static_cast<float>(to) -
         static_cast<float>(from)) * eased;

    if (value < 0.0f)
        value = 0.0f;

    if (value > 255.0f)
        value = 255.0f;

    return static_cast<BYTE>(
        value + 0.5f);
}

static void FinishAnimation(
    DesktopState* state) {

    if (!state)
        return;

    if (state->shell)
        KillTimer(state->shell, kTimerAnimation);
    EndTimerPrecision(state);

    SetAlpha(state, state->animTo);

    state->state =
        state->animTo == 0 ? IconState::Hidden : IconState::Visible;

    if (state->state == IconState::Hidden)
        ClearSelection(state);
}

static void TickAnimation(
    DesktopState* state) {

    if (!state || !state->list)
        return;

    const double elapsed = QpcToMs(QpcNow() - state->animStart);
    const float progress =
        state->animDurationMs > 0.0f
            ? static_cast<float>(elapsed / state->animDurationMs)
            : 1.0f;

    if (progress >= 1.0f) {
        FinishAnimation(state);
        return;
    }

    SetAlpha(
        state,
        EaseAlpha(
            state->animFrom,
            state->animTo,
            progress));
}

static void AnimateTo(
    DesktopState* state,
    BYTE targetAlpha) {

    if (!state || !state->list)
        return;

    KillTimer(
        state->shell,
        kTimerAnimation);

    if (targetAlpha == state->alpha) {
        // Already there (possibly an interrupted fade that just reached the
        // target): settle the state machine without animating.
        state->animTo = targetAlpha;
        FinishAnimation(state);
        return;
    }

    const int delta = targetAlpha > state->alpha
                          ? targetAlpha - state->alpha
                          : state->alpha - targetAlpha;

    state->animFrom = state->alpha;
    state->animTo = targetAlpha;
    state->animStart = QpcNow();
    // An interrupted fade only covers part of the range; scale its duration
    // so the perceived speed stays constant.
    state->animDurationMs =
        static_cast<float>(g_settings.animationDuration) *
        static_cast<float>(delta) / 255.0f;
    if (state->animDurationMs < 40.0f)
        state->animDurationMs = 40.0f;

    state->state =
        targetAlpha > state->alpha
            ? IconState::Showing
            : IconState::Hiding;

    BeginTimerPrecision(state);
    SetTimer(
        state->shell,
        kTimerAnimation,
        kAnimFrameMs,
        nullptr);
}

static void ShowIcons(
    DesktopState* state) {

    if (!state || !state->list)
        return;

    CancelAutoHide(state);

    if (state->state == IconState::Visible ||
        state->state == IconState::Showing) {

        StartAutoHide(state);
        return;
    }

    AnimateTo(
        state,
        255);

    StartAutoHide(state);
}

static void HideIcons(
    DesktopState* state) {

    if (!state || !state->list)
        return;

    if (state->dragActive ||
        state->interactionActive ||
        state->contextMenuActive ||
        IsLabelEditActive(state))
        return;

    CancelAutoHide(state);

    AnimateTo(
        state,
        0);
}


static void RequestShowIcons(
    DesktopState* state,
    bool focusList) {

    if (!state ||
        !state->shell ||
        !IsWindow(state->shell))
        return;

    if (focusList)
        state->focusListAfterShow = true;

    PostMessageW(
        state->shell,
        g_msgShow,
        0,
        0);
}

static void RequestHideIcons(
    DesktopState* state) {

    if (!state ||
        !state->shell ||
        !IsWindow(state->shell))
        return;

    PostMessageW(
        state->shell,
        g_msgHide,
        0,
        0);
}

static bool IsEmptyListPoint(
    HWND list,
    LPARAM lParam) {

    LVHITTESTINFO hit{};

    hit.pt.x =
        GET_X_LPARAM(lParam);

    hit.pt.y =
        GET_Y_LPARAM(lParam);

    LRESULT result =
        SendMessageW(
            list,
            LVM_HITTEST,
            0,
            reinterpret_cast<LPARAM>(&hit));

    return result == -1;
}

static bool IsEmptyDesktopPoint(DesktopState* state, LPARAM shellLParam) {
    if (!state || !state->list)
        return false;

    POINT pt{};
    pt.x = GET_X_LPARAM(shellLParam);
    pt.y = GET_Y_LPARAM(shellLParam);

    // Convert the SHELLDLL_DefView client coordinates to screen coordinates,
    // then to SysListView32 client coordinates before performing the hit test.
    if (!ClientToScreen(state->shell, &pt))
        return false;
    if (!ScreenToClient(state->list, &pt))
        return false;

    LPARAM listLParam = MAKELPARAM(
        static_cast<short>(pt.x),
        static_cast<short>(pt.y));

    return IsEmptyListPoint(state->list, listLParam);
}

// Classic menus use the #32768 window class. The Windows 11 desktop context
// menu is a XAML popup hosted in a Xaml_WindowedPopupClass window.
static bool IsMenuWindowClass(HWND hwnd) {
    if (!hwnd)
        return false;

    wchar_t cls[64]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)))
        return false;

    return wcscmp(cls, L"#32768") == 0 ||
           wcscmp(cls, L"Xaml_WindowedPopupClass") == 0;
}

static BOOL CALLBACK FindVisibleMenuProc(HWND hwnd, LPARAM lParam) {
    RECT rc{};
    if (IsWindowVisible(hwnd) && IsMenuWindowClass(hwnd) &&
        IsWindowOwnedByCurrentProcess(hwnd) && GetWindowRect(hwnd, &rc) &&
        !IsRectEmpty(&rc)) {
        *reinterpret_cast<bool*>(lParam) = true;
        return FALSE;
    }
    return TRUE;
}

static bool IsExplorerMenuLoopStillActive() {
    // WM_EXITMENULOOP is not guaranteed to reach our SHELLDLL_DefView/ListView
    // subclass on every Explorer code path, and the Windows 11 menu doesn't
    // run a classic menu loop at all, so use the live menu window as the
    // source of truth. Only called while a desktop menu is open.
    if (IsMenuWindowClass(GetCapture()) ||
        IsMenuWindowClass(GetForegroundWindow()))
        return true;

    bool found = false;
    EnumWindows(FindVisibleMenuProc, reinterpret_cast<LPARAM>(&found));
    return found;
}

static bool IsDesktopFocusStillActive(DesktopState* state) {
    if (!state || !state->shell || !IsWindow(state->shell))
        return false;

    HWND foreground = GetForegroundWindow();
    if (!foreground)
        return false;

    HWND desktopRoot = GetAncestor(state->shell, GA_ROOT);
    HWND foregroundRoot = GetAncestor(foreground, GA_ROOT);

    // The foreground/root-window relationship is the authoritative signal
    // for whether the user is currently on the desktop. GetFocus() is
    // thread-local and isn't suitable for this cross-window check.
    if (desktopRoot && foregroundRoot == desktopRoot)
        return true;

    // Win+D and the taskbar's "Show desktop" button may activate a different
    // top-level desktop window than the one hosting SHELLDLL_DefView (e.g.
    // WorkerW vs Progman, depending on the Windows build and wallpaper
    // setup). Any Progman/WorkerW owned by this Explorer counts as the
    // desktop.
    return foregroundRoot &&
           IsWindowOwnedByCurrentProcess(foregroundRoot) &&
           (IsClass(foregroundRoot, L"Progman") ||
            IsClass(foregroundRoot, L"WorkerW"));
}

static void UpdateDesktopFocusState(DesktopState* state) {
    if (!state)
        return;

    // A context menu or deliberate double-click temporarily suppresses
    // desktop-focus reconciliation. A real left-click on the desktop clears
    // this suppression.
    const bool desktopActive = IsDesktopFocusStillActive(state);
    if (state->suppressDesktopFocusUntilClick)
        return;

    // The desktop can become the active surface without sending a mouse
    // message to our ShellView/ListView. Win+D is the important example:
    // Windows dismisses the foreground application and activates the desktop
    // directly. This is called from EVENT_SYSTEM_FOREGROUND, so it sees every
    // such transition without any key or mouse polling.

    if (desktopActive) {
        if (!state->desktopFocusActive) {
            state->desktopFocusActive = true;
            CancelAutoHide(state);

            // Win+D can activate the desktop without sending a mouse message
            // to SHELLDLL_DefView, so desktop activation itself can request a
            // show when the ListView is hidden.
            if (state->state == IconState::Hidden ||
                state->state == IconState::Hiding) {
                RequestShowIcons(state);
            }
        }
        return;
    }

    if (state->desktopFocusActive) {
        // Losing Desktop focus is the exact start of a new inactivity period.
        // Always discard any timer that may have been armed while Desktop was
        // active and start a completely fresh full countdown from this
        // transition. This makes the behavior identical whether the user
        // leaves Desktop after 1 second or after 30 seconds.
        state->desktopFocusActive = false;
        CancelAutoHide(state);
        StartAutoHide(state);
    }
}

// The vtable methods may be shared with File Explorer folder views, so the
// hooks only react to the desktop's own drop target object.
static std::atomic<IDropTarget*> g_desktopDropTarget{nullptr};
static std::atomic<HWND> g_desktopShell{nullptr};

static bool IsMenuWindow(HWND hwnd) {
    return IsMenuWindowClass(hwnd);
}

static bool AnyMouseButtonDown() {
    return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
           (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
}

// Explorer doesn't always deliver a matching button-up/menu-loop message to
// the desktop view (e.g. capture taken by another window). Called only at
// event boundaries, never periodically.
static void RecoverInteractionState(DesktopState* state) {
    if (!state || AnyMouseButtonDown())
        return;

    if (state->interactionActive && !state->contextMenuActive)
        state->interactionActive = false;

    // An OLE drag always holds a mouse button; with none held, a missed
    // DragLeave/Drop must not block auto-hide forever.
    state->dragActive = false;
}

static void BeginContextMenu(DesktopState* state) {
    if (!state || !state->shell)
        return;

    state->contextMenuActive = true;
    state->interactionActive = false;
    state->desktopFocusActive = false;
    state->suppressDesktopFocusUntilClick = true;
    state->menuStartTick = GetTickCount();
    CancelAutoHide(state);

    // Bounded check, only while the menu is open.
    SetTimer(state->shell, kTimerMenuCheck, kMenuCheckMs, nullptr);
}

static void EndContextMenu(DesktopState* state) {
    if (!state || !state->shell)
        return;

    KillTimer(state->shell, kTimerMenuCheck);
    state->contextMenuActive = false;
    RecoverInteractionState(state);
    StartAutoHide(state);
}

static void EnsureDropTargetHook(DesktopState* state, bool applyNow);

static void OnForegroundChanged(DesktopState* state, HWND foreground) {
    if (!state || !state->shell || !IsWindow(state->shell))
        return;

    // The desktop may register its drop target after our first refresh.
    if (g_initDone && !g_desktopDropTarget.load())
        EnsureDropTargetHook(state, true);

    RecoverInteractionState(state);

    // A menu taking the foreground says nothing about the desktop.
    if (IsMenuWindow(foreground))
        return;

    const bool desktopNow = IsDesktopFocusStillActive(state);

    // Switching *to* the desktop from another window is a deliberate
    // activation (Win+D, the taskbar's Show desktop button, minimizing the
    // last window, clicking the desktop),
    // so it lifts the suppression left by a context menu or double-click
    // hide. Focus merely returning to the desktop after its own menu does
    // not, because the desktop was already the previous foreground.
    if (desktopNow && !state->lastForegroundDesktop)
        state->suppressDesktopFocusUntilClick = false;

    state->lastForegroundDesktop = desktopNow;

    UpdateDesktopFocusState(state);
}

static void CALLBACK ForegroundWinEventProc(
    HWINEVENTHOOK hook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD,
    DWORD) {

    if (g_unloading || event != EVENT_SYSTEM_FOREGROUND)
        return;

    // Out-of-context events are delivered on the thread that installed the
    // hook, which is the desktop thread.
    for (int i = 0; i < g_stateCount; ++i) {
        if (g_states[i].shell && g_states[i].foregroundHook == hook) {
            OnForegroundChanged(&g_states[i], hwnd);
            break;
        }
    }
}

static void InstallForegroundHook(DesktopState* state) {
    if (!state || state->foregroundHook)
        return;

    state->foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
        nullptr,
        ForegroundWinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT);

    if (!state->foregroundHook)
        Wh_Log(L"SetWinEventHook failed: %lu", GetLastError());

    state->lastForegroundDesktop = IsDesktopFocusStillActive(state);
}

static void RemoveForegroundHook(DesktopState* state) {
    if (state && state->foregroundHook) {
        UnhookWinEvent(state->foregroundHook);
        state->foregroundHook = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Drag reveal
//
// The desktop registers an IDropTarget with OLE. Every drag over the desktop,
// from any process, ends up calling that object's DragEnter / DragLeave / Drop
// on the desktop thread. Hooking those three methods reveals the icons exactly
// when a drag enters the desktop, with no mouse polling at all.
// ---------------------------------------------------------------------------

using DropTargetDragEnter_t = HRESULT(STDMETHODCALLTYPE*)(
    IDropTarget*, IDataObject*, DWORD, POINTL, DWORD*);
using DropTargetDragLeave_t = HRESULT(STDMETHODCALLTYPE*)(IDropTarget*);
using DropTargetDrop_t = HRESULT(STDMETHODCALLTYPE*)(
    IDropTarget*, IDataObject*, DWORD, POINTL, DWORD*);

static DropTargetDragEnter_t g_DropTargetDragEnter = nullptr;
static DropTargetDragLeave_t g_DropTargetDragLeave = nullptr;
static DropTargetDrop_t g_DropTargetDrop = nullptr;
static std::atomic<bool> g_dropHooked{false};


static void NotifyDesktopDrag(IDropTarget* self, UINT msg) {
    if (g_unloading || !self || self != g_desktopDropTarget.load())
        return;

    HWND shell = g_desktopShell.load();
    if (shell)
        PostMessageW(shell, msg, 0, 0);
}

static HRESULT STDMETHODCALLTYPE HookDropTargetDragEnter(
    IDropTarget* self, IDataObject* data, DWORD keys, POINTL pt,
    DWORD* effect) {
    NotifyDesktopDrag(self, g_msgDragEnter);
    return g_DropTargetDragEnter(self, data, keys, pt, effect);
}

static HRESULT STDMETHODCALLTYPE HookDropTargetDragLeave(IDropTarget* self) {
    NotifyDesktopDrag(self, g_msgDragLeave);
    return g_DropTargetDragLeave(self);
}

static HRESULT STDMETHODCALLTYPE HookDropTargetDrop(
    IDropTarget* self, IDataObject* data, DWORD keys, POINTL pt,
    DWORD* effect) {
    NotifyDesktopDrag(self, g_msgDragLeave);
    return g_DropTargetDrop(self, data, keys, pt, effect);
}

static bool IsCodeInLoadedModule(void* p) {
    HMODULE module = nullptr;
    return p &&
           GetModuleHandleExW(
               GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
               static_cast<LPCWSTR>(p), &module) &&
           module;
}

static IDropTarget* FindDesktopDropTarget(DesktopState* state) {
    HWND candidates[] = {
        state->list,
        state->shell,
        state->shell ? GetParent(state->shell) : nullptr,
    };

    for (HWND hwnd : candidates) {
        if (!hwnd)
            continue;

        // RegisterDragDrop stores the in-process IDropTarget in this window
        // property.
        auto* target = static_cast<IDropTarget*>(
            GetPropW(hwnd, L"OleDropTargetInterface"));
        if (!target)
            continue;

        void** vtbl = *reinterpret_cast<void***>(target);
        if (IsCodeInLoadedModule(vtbl[3]) && IsCodeInLoadedModule(vtbl[5]) &&
            IsCodeInLoadedModule(vtbl[6]))
            return target;
    }

    return nullptr;
}

static void EnsureDropTargetHook(DesktopState* state, bool applyNow) {
    if (!state || g_unloading)
        return;

    IDropTarget* target = FindDesktopDropTarget(state);
    g_desktopShell = state->shell;
    g_desktopDropTarget = target;

    if (!target) {
        Wh_Log(L"Desktop drop target not found yet");
        return;
    }

    if (g_dropHooked)
        return;

    // IDropTarget vtable: QueryInterface, AddRef, Release, DragEnter,
    // DragOver, DragLeave, Drop.
    void** vtbl = *reinterpret_cast<void***>(target);

    const bool ok =
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<DropTargetDragEnter_t>(vtbl[3]),
            HookDropTargetDragEnter, &g_DropTargetDragEnter) &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<DropTargetDragLeave_t>(vtbl[5]),
            HookDropTargetDragLeave, &g_DropTargetDragLeave) &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<DropTargetDrop_t>(vtbl[6]),
            HookDropTargetDrop, &g_DropTargetDrop);

    if (!ok) {
        Wh_Log(L"Drop target hooks failed");
        return;
    }

    g_dropHooked = true;
    if (applyNow)
        Wh_ApplyHookOperations();

    Wh_Log(L"Drop target hooks set");
}

static void HandleTimer(
    DesktopState* state,
    WPARAM timerId) {

    if (!state)
        return;

    if (timerId == kTimerAnimation) {
        TickAnimation(state);
        return;
    }

    if (timerId == kTimerMenuCheck) {
        if (!state->contextMenuActive) {
            KillTimer(state->shell, kTimerMenuCheck);
            return;
        }

        const DWORD menuAge = GetTickCount() - state->menuStartTick;
        if ((menuAge >= kMenuGraceMs && !IsExplorerMenuLoopStillActive()) ||
            menuAge >= kMenuMaxMs) {
            EndContextMenu(state);
        }
        return;
    }

    if (timerId == kTimerAutoHide) {
        KillTimer(
            state->shell,
            kTimerAutoHide);
        state->autoHideTimerArmed = false;

        // If the countdown expires while Desktop is active, it has no meaning:
        // the user is allowed to stay on Desktop indefinitely. Do NOT turn it
        // into a 100 ms retry timer. When focus later leaves Desktop,
        // UpdateDesktopFocusState() will cancel any stale timer and start a
        // fresh full countdown from that exact transition.
        if (state->desktopFocusActive) {
            return;
        }

        RecoverInteractionState(state);

        // Dragging or a context menu can end without a foreground transition,
        // so keep a short retry while those states remain active.
        if (state->dragActive ||
            state->interactionActive ||
            state->contextMenuActive ||
            IsLabelEditActive(state)) {
            SetTimer(
                state->shell,
                kTimerAutoHide,
                100,
                nullptr);
            state->autoHideTimerArmed = true;
            return;
        }

        if (state->state == IconState::Visible) {
            RequestHideIcons(state);
        }

        return;
    }

}

static bool IsModifierKey(WPARAM vk) {
    switch (vk) {
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT:
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL:
    case VK_MENU:
    case VK_LMENU:
    case VK_RMENU:
    case VK_LWIN:
    case VK_RWIN:
        return true;
    }
    return false;
}

LRESULT CALLBACK DesktopListSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR) {

    DesktopState* state = FindStateByList(hWnd);
    if (!state)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
    case WM_NCHITTEST:
        // Hidden icons must not be clickable: let the click fall through to
        // SHELLDLL_DefView, exactly as if the ListView were hidden.
        if (state->state == IconState::Hidden && !g_unloading)
            return HTTRANSPARENT;
        break;

    case WM_PAINT:
    case WM_PRINTCLIENT: {
        if (state->alpha >= 255 || g_unloading)
            break;

        const int prevAlpha = tl_paintAlpha;
        tl_paintAlpha = state->alpha;
        const LRESULT result =
            DefSubclassProc(hWnd, uMsg, wParam, lParam);
        tl_paintAlpha = prevAlpha;
        return result;
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        // While hidden, the first real key press reveals the icons instead
        // of acting on invisible items (type-to-select, Enter, Delete, F2,
        // Alt+Enter...). Modifier keys alone pass through, so shortcuts like
        // Alt+Tab or Win+D keep working.
        if (state->state == IconState::Hidden && !g_unloading) {
            // Alt+F4 doesn't act on any item; let it open the shutdown
            // dialog on the first press.
            if (IsModifierKey(wParam) ||
                (uMsg == WM_SYSKEYDOWN && wParam == VK_F4))
                break;
            if (!(lParam & (1 << 30)))  // not an auto-repeat
                RequestShowIcons(state);
            return 0;
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
    case WM_CHAR:
    case WM_SYSCHAR:
        if (state->state == IconState::Hidden && !g_unloading &&
            !((uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) &&
              (IsModifierKey(wParam) || wParam == VK_F4)))
            return 0;
        break;

    case WM_LBUTTONDOWN:
        // A click while the icons are fading out brings them back.
        if (state->state == IconState::Hiding)
            RequestShowIcons(state);

        // A real left-click is the explicit signal that the user resumed
        // interacting with the desktop after a context menu/double-click.
        state->suppressDesktopFocusUntilClick = false;
        state->desktopFocusActive = true;
        state->interactionActive = true;
        CancelAutoHide(state);
        break;

    case WM_LBUTTONUP:
        state->interactionActive = false;
        state->dragActive = false;
        StartAutoHide(state);
        break;

    case WM_RBUTTONDOWN:
        state->interactionActive = true;
        // Opening the desktop context menu ends the active-desktop state.
        // Keep focus reconciliation suppressed until the next real desktop
        // left-click.
        state->desktopFocusActive = false;
        state->suppressDesktopFocusUntilClick = true;
        CancelAutoHide(state);
        break;

    case WM_RBUTTONUP:
        state->interactionActive = false;
        StartAutoHide(state);
        break;

    case WM_CONTEXTMENU:
        // Explorer may route WM_CONTEXTMENU differently from the mouse/menu
        // loop messages. Treat it as the authoritative beginning of a desktop
        // context-menu interaction as well.
        BeginContextMenu(state);
        break;

    case WM_ENTERMENULOOP:
        // The menu loop means the desktop is no longer an active interaction
        // surface. Suspend desktop-focus tracking for this menu invocation so
        // it cannot immediately re-block the auto-hide timer after the menu
        // closes. A subsequent left-click on the desktop re-enables it.
        BeginContextMenu(state);
        break;

    case WM_EXITMENULOOP:
        EndContextMenu(state);
        break;

    case WM_LBUTTONDBLCLK:
        // Prefer the native double-click message when the window class
        // advertises CS_DBLCLKS.
        if (SupportsNativeDoubleClick(hWnd) &&
            IsEmptyListPoint(hWnd, lParam)) {
            // WM_LBUTTONDOWN arrives immediately before WM_LBUTTONDBLCLK and
            // marks the interaction as active. Clear that transient flag (and
            // any stale menu state) so the deliberate double-click can hide
            // the icons.
            state->contextMenuActive = false;
            state->interactionActive = false;
            state->dragActive = false;
            state->suppressDesktopFocusUntilClick = true;
            state->desktopFocusActive = false;
            RequestHideIcons(state);
            return 0;
        }
        break;

    case WM_KILLFOCUS:
        state->desktopFocusActive = false;
        CancelAutoHide(state);
        StartAutoHide(state);
        break;

    case WM_NCDESTROY:
        // The ListView is going away; forget it so nothing touches a dead
        // HWND. A new one is picked up through CreateWindowExW.
        state->list = nullptr;
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DesktopShellSubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR) {

    // The state is created only by the initial refresh, which runs on this
    // (desktop) thread. Every other message just looks it up.
    DesktopState* state =
        (uMsg == g_msgRefresh && wParam == 0)
            ? GetOrCreateState(hWnd)
            : FindStateByShell(hWnd);
    if (!state)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (uMsg == g_msgRefresh && wParam == 1) {
        // Settings changed: keep the current visual state, only restart the
        // auto-hide countdown with the new delay.
        CancelAutoHide(state);
        if (!g_settings.autoHideEnabled && state->state != IconState::Visible &&
            state->state != IconState::Showing)
            RequestShowIcons(state);
        else
            StartAutoHide(state);
        return 0;
    }

    if (uMsg == g_msgRefresh && wParam == 2) {
        // Hooks that need live desktop objects, applied after Wh_ModInit.
        HookComctl32For(state->list, true);
        EnsureDropTargetHook(state, true);
        return 0;
    }

    if (uMsg == g_msgDragEnter) {
        state->dragActive = true;
        CancelAutoHide(state);
        if (state->state == IconState::Hidden ||
            state->state == IconState::Hiding) {
            ShowIcons(state);
        }
        return 0;
    }

    if (uMsg == g_msgDragLeave) {
        state->dragActive = false;
        RecoverInteractionState(state);
        StartAutoHide(state);
        return 0;
    }

    if (uMsg == g_msgRefresh) {
        HWND newList = FindDesktopList(hWnd);
        if (newList && newList != state->list) {
            state->list = newList;
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                state->list, DesktopListSubclassProc, 0);
        } else if (!newList) {
            state->list = nullptr;
        }

        if (state->list) {
            KillTimer(hWnd, kTimerAutoHide);
            KillTimer(hWnd, kTimerAnimation);
            KillTimer(hWnd, kTimerMenuCheck);

            InstallForegroundHook(state);
            if (g_initDone) {
                HookComctl32For(state->list, true);
                EnsureDropTargetHook(state, true);
            }

            // Undo a layered style left behind by older versions of the mod.
            const LONG_PTR exStyle =
                GetWindowLongPtrW(state->list, GWL_EXSTYLE);
            if (exStyle & WS_EX_LAYERED) {
                SetWindowLongPtrW(
                    state->list, GWL_EXSTYLE,
                    exStyle & ~static_cast<LONG_PTR>(WS_EX_LAYERED));
            }

            // Startup state: with auto-hide enabled the icons start hidden
            // (opacity 0; the ListView itself stays visible and simply paints
            // nothing). With auto-hide disabled they start visible.
            EndTimerPrecision(state);
            state->alpha = 255;
            if (g_settings.autoHideEnabled) {
                SetAlpha(state, 0);
                state->state = IconState::Hidden;
            } else {
                RepaintList(state);
                state->state = IconState::Visible;
            }
            state->dragActive = false;
            state->interactionActive = false;
            state->suppressDesktopFocusUntilClick = false;
            state->contextMenuActive = false;
            state->desktopFocusActive = false;
            state->autoHideTimerArmed = false;
            state->focusListAfterShow = false;
            state->manualDoubleClickArmed = false;
            state->manualClickTime = 0;
            state->manualClickPoint = {};
            state->manualClickWindow = nullptr;
            state->menuStartTick = 0;
            state->lastForegroundDesktop = IsDesktopFocusStillActive(state);
        }

        return 0;
    }

    if (uMsg == g_msgShow) {
        ShowIcons(state);

        if (state->focusListAfterShow) {
            state->focusListAfterShow = false;

            if (state->list &&
                IsWindow(state->list) &&
                IsWindowVisible(state->list)) {
                SetFocus(state->list);
            }
        }

        return 0;
    }

    if (uMsg == g_msgHide) {
        HideIcons(state);
        return 0;
    }

    if (uMsg == g_msgUninit) {
        // Runs on the desktop thread, so timers and GDI objects created here
        // can be released reliably.
        KillTimer(hWnd, kTimerAutoHide);
        KillTimer(hWnd, kTimerAnimation);
        KillTimer(hWnd, kTimerMenuCheck);
        EndTimerPrecision(state);
        RemoveForegroundHook(state);
        g_desktopDropTarget = nullptr;
        g_desktopShell = nullptr;

        state->state = IconState::Visible;
        state->alpha = 255;

        if (state->list && IsWindow(state->list)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                state->list, DesktopListSubclassProc);
            InvalidateRect(state->list, nullptr, TRUE);
        }

        FreeOffscreen();
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hWnd, DesktopShellSubclassProc);
        state->shell = nullptr;
        state->list = nullptr;
        return 1;
    }

    switch (uMsg) {
    case WM_COMMAND:
        // Keyboard accelerators (Ctrl+A, Ctrl+X, Delete, F2, ...) are
        // translated by the desktop's message loop and sent to the view as
        // WM_COMMAND with HIWORD(wParam) == 1, never reaching the ListView.
        // While hidden, reveal the icons instead of running the command on
        // items the user can't see.
        if (HIWORD(wParam) == 1 && lParam == 0 &&
            state->state == IconState::Hidden && !g_unloading) {
            RequestShowIcons(state);
            return 0;
        }
        break;

    case WM_CONTEXTMENU:
        BeginContextMenu(state);
        break;

    case WM_LBUTTONDBLCLK:
        // Use the native path only when the class actually supports
        // WM_LBUTTONDBLCLK generation; otherwise WM_LBUTTONDOWN uses the
        // interval/distance fallback above.
        if (SupportsNativeDoubleClick(hWnd) &&
            state->list &&
            IsEmptyDesktopPoint(state, lParam)) {
            state->interactionActive = false;
            state->dragActive = false;
            state->suppressDesktopFocusUntilClick = true;
            state->desktopFocusActive = false;
            RequestHideIcons(state);
            return 0;
        }
        break;

    case WM_LBUTTONDOWN:
        if (IsManualDoubleClick(state, hWnd, lParam, true) &&
            (state->state == IconState::Visible ||
             state->state == IconState::Showing)) {
            state->interactionActive = false;
            state->dragActive = false;
            state->suppressDesktopFocusUntilClick = true;
            state->desktopFocusActive = false;
            RequestHideIcons(state);
            return 0;
        }

        // A real left-click is the explicit signal that the user resumed
        // interacting with the desktop after a context menu/double-click.
        state->suppressDesktopFocusUntilClick = false;
        state->desktopFocusActive = true;
        // When SysListView32 is hidden, SHELLDLL_DefView receives the click.
        // A single click reveals the icons. The next click of a double-click
        // is then handled by SysListView32 and can hide them again.
        if (state->state == IconState::Hidden ||
            state->state == IconState::Hiding) {
            // The actual show is queued so the current Explorer mouse-message
            // stack can finish first. Focus is applied from g_msgShow after
            // the ListView has become visible.
            RequestShowIcons(state, true);
        }
        state->interactionActive = true;
        CancelAutoHide(state);
        break;

    case WM_LBUTTONUP:
        state->interactionActive = false;
        StartAutoHide(state);
        break;

    case WM_RBUTTONDOWN:
        state->interactionActive = true;
        // Opening the desktop context menu ends the active-desktop state.
        // Keep focus reconciliation suppressed until the next real desktop
        // left-click.
        state->desktopFocusActive = false;
        state->suppressDesktopFocusUntilClick = true;
        CancelAutoHide(state);
        break;

    case WM_RBUTTONUP:
        state->interactionActive = false;
        StartAutoHide(state);
        break;

    case WM_ENTERMENULOOP:
        // The menu loop means the desktop is no longer an active interaction
        // surface. Suspend desktop-focus tracking for this menu invocation so
        // it cannot immediately re-block the auto-hide timer after the menu
        // closes. A subsequent left-click on the desktop re-enables it.
        BeginContextMenu(state);
        break;

    case WM_EXITMENULOOP:
        EndContextMenu(state);
        break;

    case WM_TIMER:
        HandleTimer(state, wParam);
        if (wParam == kTimerAnimation ||
            wParam == kTimerAutoHide ||
            wParam == kTimerMenuCheck)
            return 0;
        break;

    case WM_KILLFOCUS:
        state->desktopFocusActive = false;
        CancelAutoHide(state);
        StartAutoHide(state);
        break;

    case WM_NCDESTROY:
        KillTimer(hWnd, kTimerAnimation);
        KillTimer(hWnd, kTimerAutoHide);
        KillTimer(hWnd, kTimerMenuCheck);
        EndTimerPrecision(state);
        RemoveForegroundHook(state);
        if (g_desktopShell.load() == hWnd) {
            g_desktopDropTarget = nullptr;
            g_desktopShell = nullptr;
        }
        state->shell = nullptr;
        state->list = nullptr;
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Progman or WorkerW owned by this explorer.exe.
static bool IsDesktopHostWindow(HWND hwnd) {
    return hwnd && IsWindowOwnedByCurrentProcess(hwnd) &&
           (IsClass(hwnd, L"Progman") || IsClass(hwnd, L"WorkerW"));
}

static bool IsDesktopShellView(HWND shell) {
    // Only the desktop's DefView (under Progman or a WorkerW). File Explorer
    // folder windows and common dialogs create SHELLDLL_DefView too and must
    // be left alone.
    return shell && IsDesktopHostWindow(GetParent(shell));
}

static void SubclassDesktopShell(
    HWND shell) {

    if (!shell || !IsDesktopShellView(shell))
        return;

    if (!WindhawkUtils::
            SetWindowSubclassFromAnyThread(
                shell,
                DesktopShellSubclassProc,
                0)) {

        Wh_Log(
            L"Failed to subclass ShellView %p",
            shell);

        return;
    }

    PostMessageW(
        shell,
        g_msgRefresh,
        0,
        0);
}

static BOOL CALLBACK EnumWindowsProc(
    HWND hWnd,
    LPARAM) {

    // WorkerW is a generic class that other processes create too; only
    // this Explorer's own desktop windows are of interest.
    if (!IsDesktopHostWindow(hWnd))
        return TRUE;

    HWND shell =
        FindWindowExW(
            hWnd,
            nullptr,
            L"SHELLDLL_DefView",
            nullptr);

    if (shell)
        SubclassDesktopShell(shell);

    return TRUE;
}

static void DiscoverExistingDesktop() {
    EnumWindows(
        EnumWindowsProc,
        0);
}

static HWND WINAPI HookCreateWindowExW(
    DWORD exStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD style,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    LPVOID param) {

    HWND hWnd =
        g_CreateWindowExW(
            exStyle,
            className,
            windowName,
            style,
            x,
            y,
            width,
            height,
            parent,
            menu,
            instance,
            param);

    if (!hWnd ||
        !className ||
        IS_INTRESOURCE(className))
        return hWnd;

    if (wcscmp(
            className,
            L"SHELLDLL_DefView") == 0) {

        SubclassDesktopShell(hWnd);

    } else if (
        wcscmp(
            className,
            L"SysListView32") == 0 &&
        parent &&
        IsClass(
            parent,
            L"SHELLDLL_DefView") &&
        IsDesktopShellView(parent)) {

        // Don't touch DesktopState here; the refresh handler picks up and
        // subclasses the new ListView on the desktop thread.
        PostMessageW(
            parent,
            g_msgRefresh,
            0,
            0);
    }

    return hWnd;
}

static void Cleanup() {

    for (int i = 0;
         i < g_stateCount;
         ++i) {

        DesktopState* state =
            &g_states[i];

        if (!IsExpectedDesktopShell(state->shell))
            continue;

        // Runs the g_msgUninit handler on the desktop thread: unhooks the
        // WinEvent hook, kills the timers, releases the timer resolution
        // request, frees GDI objects and removes both subclasses. All of
        // these are thread-affine, so there is no cross-thread fallback.
        SendMessageW(state->shell, g_msgUninit, 0, 0);
    }

    g_stateCount = 0;
}

static HWND FindExistingDesktopList() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND shell = IsDesktopHostWindow(progman)
        ? FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr)
        : nullptr;

    // With some wallpaper setups the DefView lives under a WorkerW.
    for (HWND worker = nullptr; !shell;) {
        worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr);
        if (!worker)
            break;
        if (!IsDesktopHostWindow(worker))
            continue;
        shell = FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr);
    }

    return shell ? FindDesktopList(shell) : nullptr;
}

BOOL Wh_ModInit() {

    Wh_Log(
        L"Init");

    LoadSettings();

    g_msgRefresh =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Refresh");

    g_msgShow =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Show");

    g_msgHide =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Hide");

    g_msgUninit =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.Uninit");

    g_msgDragEnter =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.DragEnter");

    g_msgDragLeave =
        RegisterWindowMessageW(
            L"Windhawk.SmoothDesktop.DragLeave");

    if (!g_msgRefresh ||
        !g_msgShow ||
        !g_msgHide ||
        !g_msgUninit ||
        !g_msgDragEnter ||
        !g_msgDragLeave) {

        Wh_Log(
            L"Failed to register messages");

        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
            CreateWindowExW,
            HookCreateWindowExW,
            &g_CreateWindowExW)) {

        Wh_Log(
            L"CreateWindowExW hook failed");

        return FALSE;
    }

    if (!InstallGdiHooks()) {
        Wh_Log(
            L"GDI paint hooks failed");

        return FALSE;
    }

    DiscoverExistingDesktop();

    Wh_Log(
        L"Init complete");

    return TRUE;
}

void Wh_ModAfterInit() {
    g_initDone = true;

    // Hooks that depend on live desktop objects (the comctl32 copy owning the
    // ListView, the desktop's IDropTarget) are installed on the desktop
    // thread. Later desktops get them from their own refresh.
    HWND list = FindExistingDesktopList();
    if (list)
        PostMessageW(GetParent(list), g_msgRefresh, 2, 0);
}

void Wh_ModBeforeUninit() {
    // Windhawk removes the mod's hooks after this returns. Stop installing
    // new hooks and make the paint hooks, subclasses and WinEvent handler
    // inert from this point on.
    g_unloading = true;
}

void Wh_ModUninit() {

    Wh_Log(
        L"Uninit");

    g_unloading = true;

    Cleanup();
}

void Wh_ModSettingsChanged() {

    Wh_Log(
        L"Settings changed");

    LoadSettings();

    // Let the desktop thread apply the new settings to its own state.
    HWND list = FindExistingDesktopList();
    if (list)
        PostMessageW(GetParent(list), g_msgRefresh, 1, 0);
}
