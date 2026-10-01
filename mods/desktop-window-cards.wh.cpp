// ==WindhawkMod==
// @id              desktop-window-cards
// @name            Desktop Window Cards
// @description     Minimized windows fall onto the desktop as dimmed thumbnail cards you can restore or close
// @version         1.5
// @author          HaVeN80
// @github          https://github.com/haven80
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lgdi32 -lshell32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Window Cards

When you minimize a window, a copy of it "falls" onto the desktop: it drops
away, tilts back, loses its shadow and dims, until it lands as a small card
underneath all other windows.

![Showcase](https://i.imgur.com/jo794nO.gif)
[Watch the full-quality video](https://i.imgur.com/YmIrKAd.mp4)

* **Click the thumbnail:** the card rises back to the window's original
  position and the real window is restored in its place.
* **Top bar:** drag it to move the card around the desktop. With the option
  enabled, the position is remembered: the next time you minimize that window,
  its card lands where you left it. In grid layout, dragging a card over
  another one swaps them; with the same option enabled, reordered cards return
  to their place in the grid the next time their window is minimized.
* **Grid layout (optional):** instead of floating freely, the cards of each
  monitor are arranged automatically in a centered grid, similar to Task View,
  as large as the space allows. Only the windows of that monitor and of the
  current virtual desktop are shown, and the grid rearranges itself with an
  animation whenever a card arrives or leaves. Tip: Win+M minimizes all windows
  at once, turning the desktop into an overview of everything that is open.
* **X button:** closes the window (or only removes the card, depending on the
  settings).
* **Virtual desktops:** each card is shown only on the virtual desktop of its
  window (windows pinned to all desktops keep their card everywhere).
* **Smooth reveal:** on restore, a snapshot of the card stays on top of the
  window while Windows rebuilds its transparency effects (Mica/Acrylic), then
  fades out to reveal the real window, hiding the initial flicker.

Cards use DWM thumbnails, the same ones used by taskbar previews. The content
stays live as long as the app keeps drawing while minimized; many apps stop
doing so and show their last frame.

The mod runs in its own dedicated process, not inside `explorer.exe` or
`dwm.exe`, so a problem in the mod can't take down the taskbar or the desktop,
and cards survive an Explorer restart.

## Notes

* Native Windows animation: in "Off only when restoring from a card" mode
  (default), Windows animations stay enabled (maximize, snap and other mods
  relying on them keep working) and are switched off only for the moment a
  card restores its window. When minimizing, Windows' own animation towards
  the taskbar therefore remains visible as well. "Always off" disables it while
  the mod is active: no double animation, but no maximize animation either.
  These changes only apply to the current session.
* If the mod's process ends unexpectedly while the native animation is
  switched off, the mod switches it back on the next time it starts.
* Windows running as administrator can be shown as cards, but restoring or
  closing them from a card may fall back to a plain restore, since a
  non-elevated process can't send them commands.
* Apps that hide to the notification area when minimized don't get a card.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LayoutMode: floating
  $name: Layout
  $options:
  - floating: Floating cards (land near the window)
  - grid: Grid (Task View style, per monitor)
- GridMaxWidth: 560
  $name: Grid - maximum card width (px at 100% scaling)
  $description: 200-1600. Cards grow up to this width when there is room.
- ScalePercent: 25
  $name: Card size (%)
  $description: Floating layout. Size of the thumbnail relative to the original window (10-60).
- MaxWidth: 360
  $name: Maximum width (px at 100% scaling)
  $description: Floating layout. Upper limit for the card width (120-1200).
- Brightness: 60
  $name: Resting brightness (%)
  $description: 20-100. 100 = no dimming. The card brightens on mouse hover.
- AnimationMs: 480
  $name: Fall duration (ms)
  $description: 150-2000. Restoring takes three quarters of this time.
- CloseAction: window
  $name: X button
  $options:
  - window: Closes the window
  - card: Removes the card only
- IncludeExisting: true
  $name: Create cards for windows already minimized at startup
- NativeAnimationMode: restore
  $name: Native Windows animation
  $options:
  - restore: Off only when restoring from a card
  - always: Always off (maximize/snap too)
  - never: Never off
- RememberPosition: true
  $name: Remember the position of moved cards
  $description: >-
    Floating layout: the card lands where you left it. Grid layout: a card you
    reordered goes back to its place instead of the end of the grid.
- SmoothReveal: true
  $name: Smooth reveal on restore
  $description: Hides the flicker of transparent windows (Mica/Acrylic) with a fade.
- RevealDelayMs: 200
  $name: Delay before fading (ms)
  $description: >-
    0-2000. Time given to Windows to rebuild transparency. Increase it if you
    still see flicker.
- RevealFadeMs: 220
  $name: Fade duration (ms)
  $description: 0-1500. 0 removes the snapshot at once, without fading.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

extern "C" IMAGE_DOS_HEADER __ImageBase;

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

namespace {

constexpr wchar_t kCardClass[] = L"DesktopWindowCards_Card";
// Layered, click-through popups: the drop shadow and the smooth-reveal snapshot.
constexpr wchar_t kLayeredClass[] = L"DesktopWindowCards_Layered";
constexpr wchar_t kControllerClass[] = L"DesktopWindowCards_Controller";

constexpr UINT WM_APP_RELOAD = WM_APP + 1;
constexpr UINT WM_APP_WINEVENT = WM_APP + 2;
constexpr UINT_PTR TIMER_HANDOFF = 1;
constexpr UINT_PTR TIMER_CLOSE_CHECK = 2;
constexpr UINT_PTR TIMER_REVEAL = 3;
// Controller timers: virtual desktop visibility of the cards.
constexpr UINT_PTR TIMER_DESKTOP_CHECK = 10;  // coalesces a burst of cloak events
constexpr UINT_PTR TIMER_DESKTOP_POLL = 11;   // safety net for missed events
constexpr UINT kDesktopCheckDelayMs = 30;
constexpr UINT kDesktopPollMs = 1000;
constexpr UINT_PTR TIMER_RELAYOUT = 12;       // coalesces display / work area changes
constexpr int kGlideMs = 260;                 // grid rearrangement animation
constexpr double kReorderCooldownMs = 150;    // between two swaps while dragging

constexpr double kPi = 3.14159265358979323846;
constexpr int kHeaderDip = 24;
constexpr DWORD kDwmCornerPreference = 33;  // DWMWA_WINDOW_CORNER_PREFERENCE
constexpr int kDwmCornerRoundSmall = 3;     // DWMWCP_ROUNDSMALL
constexpr DWORD kDwmCloak = 13;             // DWMWA_CLOAK
// Persisted across crashes of the mod's process: set while it has Windows' min/max
// animation switched off, so a restarted instance can switch it back on.
constexpr wchar_t kAnimationDisabledValue[] = L"MinAnimateDisabledByMod";

// Defaults mirror the WindhawkModSettings block; LoadSettings overwrites them.
struct Settings {
    int scalePercent = 25;
    int maxWidth = 360;
    int brightness = 60;
    int animationMs = 480;
    bool closeWindow = true;
    bool includeExisting = true;
    enum class NativeAnimation { RestoreOnly, Always, Never } nativeAnimation =
        NativeAnimation::RestoreOnly;
    bool rememberPosition = true;
    bool smoothReveal = true;
    int revealDelayMs = 200;
    int revealFadeMs = 220;
    bool grid = false;
    int gridMaxWidth = 560;
} g_settings;

enum class CardState { Falling, Resting, Rising, Handoff, Revealing, Vanishing };

struct Card {
    HWND target = nullptr;
    HWND host = nullptr;
    HWND shadow = nullptr;
    HTHUMBNAIL thumb = nullptr;
    RECT sourceCrop = {};      // visible part of the source window
    RECT visibleRect = {};     // on-screen rect of the restored window (no invisible borders)
    RECT restThumbRect = {};   // thumbnail area of the card at rest
    int headerFullPx = 0;

    CardState state = CardState::Falling;
    RECT animFrom = {}, animTo = {};  // thumbnail-area rects
    double brightFrom = 1, brightTo = 1;
    double shadowFrom = 0, shadowTo = 0;
    double headerFrom = 0;
    double animStart = 0;  // ms, QueryPerformanceCounter-based
    int animMs = 1;

    double bright = 1;          // thumbnail opacity over black = brightness
    double shadowStrength = 0;
    double header = 0;          // 0..1 fraction of header height

    bool hover = false, hoverClose = false, pressed = false, closing = false;
    bool suppressingNativeAnimation = false;
    // The target lives on another virtual desktop: the card is hidden.
    bool offDesktop = false;

    // Grid layout: position in the grid (lower first) and thumbnail aspect ratio.
    uint64_t order = 0;
    double aspect = 1.6;
    // A resting card sliding to its new grid cell.
    bool gliding = false;
    RECT glideFrom = {}, glideTo = {};
    double glideStart = 0;
    std::wstring title;
    HICON icon = nullptr;  // our own copy, destroyed with the card
    HFONT font = nullptr;
    UINT fontDpi = 0;

    HDC shadowDC = nullptr;
    HBITMAP shadowBmp = nullptr;
    HGDIOBJ shadowOldBmp = nullptr;
    uint32_t* shadowBits = nullptr;
    int shadowCapW = 0, shadowCapH = 0;

    // Static snapshot shown above the restoring window (smooth reveal).
    HWND overlay = nullptr;
    HDC overlayDC = nullptr;
    HBITMAP overlayBmp = nullptr;
    HGDIOBJ overlayOldBmp = nullptr;
    POINT overlayPos = {};
    SIZE overlaySize = {};
};

std::vector<std::unique_ptr<Card>> g_cards;
std::atomic<HWND> g_controller{nullptr};
std::atomic<bool> g_stop{false};
HANDLE g_thread = nullptr;
HANDLE g_frameTimer = nullptr;
bool g_frameTimerArmed = false;
HWINEVENTHOOK g_hooks[4] = {};

bool g_nativeAnimationChanged = false;

uint64_t g_nextOrder = 1;
bool g_shuttingDown = false;
Card* g_draggingCard = nullptr;   // card whose top bar is being dragged
Card* g_lastSwapWith = nullptr;   // grid drag: don't swap back until the cursor leaves it
double g_lastReorderMs = 0;

HINSTANCE ModuleInstance() {
    return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

// ---------------------------------------------------------------------------
// Small helpers

// GetTickCount64 only ticks every ~15.6 ms, which would make animations stutter
// on high refresh rate displays.
double NowMs() {
    static const double msPerTick = [] {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return 1000.0 / static_cast<double>(frequency.QuadPart);
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<double>(counter.QuadPart) * msPerTick;
}

double Clamp01(double v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }
double Lerp(double a, double b, double t) { return a + (b - a) * t; }
double EaseOutCubic(double t) { double u = 1 - t; return 1 - u * u * u; }
int RectW(const RECT& r) { return r.right - r.left; }
int RectH(const RECT& r) { return r.bottom - r.top; }

RECT LerpRect(const RECT& a, const RECT& b, double t) {
    return {static_cast<LONG>(std::lround(Lerp(a.left, b.left, t))),
            static_cast<LONG>(std::lround(Lerp(a.top, b.top, t))),
            static_cast<LONG>(std::lround(Lerp(a.right, b.right, t))),
            static_cast<LONG>(std::lround(Lerp(a.bottom, b.bottom, t)))};
}

// Scale around the bottom-center point: reads as "tipping back" onto the desk.
RECT ScaleAnchorBottom(const RECT& r, double wf, double hf) {
    double cx = (r.left + r.right) / 2.0;
    double w = RectW(r) * wf;
    double h = RectH(r) * hf;
    return {static_cast<LONG>(std::lround(cx - w / 2)),
            static_cast<LONG>(std::lround(r.bottom - h)),
            static_cast<LONG>(std::lround(cx + w / 2)), r.bottom};
}

RECT ScaleAroundCenter(const RECT& r, double f) {
    double cx = (r.left + r.right) / 2.0, cy = (r.top + r.bottom) / 2.0;
    double w = RectW(r) * f / 2, h = RectH(r) * f / 2;
    return {static_cast<LONG>(std::lround(cx - w)), static_cast<LONG>(std::lround(cy - h)),
            static_cast<LONG>(std::lround(cx + w)), static_cast<LONG>(std::lround(cy + h))};
}

bool IsClassName(HWND hwnd, const wchar_t* name) {
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, name) == 0;
}

bool IsOurWindow(HWND hwnd) {
    return IsClassName(hwnd, kCardClass) || IsClassName(hwnd, kLayeredClass);
}

Card* FindCardByTarget(HWND target) {
    for (auto& c : g_cards) {
        if (c->target == target) return c.get();
    }
    return nullptr;
}

bool AnyAnimating() {
    for (auto& c : g_cards) {
        if (c->state == CardState::Falling || c->state == CardState::Rising ||
            c->state == CardState::Revealing || c->state == CardState::Vanishing ||
            c->gliding) {
            return true;
        }
    }
    return false;
}

void LoadSettings() {
    g_settings.scalePercent = std::clamp(Wh_GetIntSetting(L"ScalePercent"), 10, 60);
    g_settings.maxWidth = std::clamp(Wh_GetIntSetting(L"MaxWidth"), 120, 1200);
    g_settings.brightness = std::clamp(Wh_GetIntSetting(L"Brightness"), 20, 100);
    g_settings.animationMs = std::clamp(Wh_GetIntSetting(L"AnimationMs"), 150, 2000);
    PCWSTR closeAction = Wh_GetStringSetting(L"CloseAction");
    g_settings.closeWindow = !closeAction || wcscmp(closeAction, L"card") != 0;
    Wh_FreeStringSetting(closeAction);
    g_settings.includeExisting = Wh_GetIntSetting(L"IncludeExisting") != 0;
    PCWSTR mode = Wh_GetStringSetting(L"NativeAnimationMode");
    if (mode && wcscmp(mode, L"always") == 0) {
        g_settings.nativeAnimation = Settings::NativeAnimation::Always;
    } else if (mode && wcscmp(mode, L"never") == 0) {
        g_settings.nativeAnimation = Settings::NativeAnimation::Never;
    } else {
        g_settings.nativeAnimation = Settings::NativeAnimation::RestoreOnly;
    }
    Wh_FreeStringSetting(mode);
    g_settings.rememberPosition = Wh_GetIntSetting(L"RememberPosition") != 0;
    g_settings.smoothReveal = Wh_GetIntSetting(L"SmoothReveal") != 0;
    g_settings.revealDelayMs = std::clamp(Wh_GetIntSetting(L"RevealDelayMs"), 0, 2000);
    g_settings.revealFadeMs = std::clamp(Wh_GetIntSetting(L"RevealFadeMs"), 0, 1500);
    PCWSTR layout = Wh_GetStringSetting(L"LayoutMode");
    g_settings.grid = layout && wcscmp(layout, L"grid") == 0;
    Wh_FreeStringSetting(layout);
    g_settings.gridMaxWidth = std::clamp(Wh_GetIntSetting(L"GridMaxWidth"), 200, 1600);
}

// All changes are session-only (no SPIF_UPDATEINIFILE): signing out resets them.
bool SetMinAnimate(bool enabled) {
    ANIMATIONINFO ai = {sizeof(ai)};
    ai.iMinAnimate = enabled ? 1 : 0;
    if (!SystemParametersInfoW(SPI_SETANIMATION, sizeof(ai), &ai, 0)) return false;
    Wh_SetIntValue(kAnimationDisabledValue, enabled ? 0 : 1);
    return true;
}

// A previous instance may have died (process crash) with the animation off.
void RecoverNativeAnimationAfterCrash() {
    if (Wh_GetIntValue(kAnimationDisabledValue, 0)) {
        Wh_Log(L"Re-enabling the native min/max animation left off by a previous instance");
        SetMinAnimate(true);
    }
}

void ApplyNativeAnimationSetting(bool forceRestore) {
    bool wantDisabled =
        g_settings.nativeAnimation == Settings::NativeAnimation::Always && !forceRestore;
    ANIMATIONINFO ai = {sizeof(ai)};
    if (wantDisabled && !g_nativeAnimationChanged) {
        if (SystemParametersInfoW(SPI_GETANIMATION, sizeof(ai), &ai, 0) && ai.iMinAnimate) {
            g_nativeAnimationChanged = SetMinAnimate(false);
        }
    } else if (!wantDisabled && g_nativeAnimationChanged) {
        SetMinAnimate(true);
        g_nativeAnimationChanged = false;
    }
}

// Restore-only mode: switch Windows' min/max animation off just while a card
// hands back its window, so the window appears in place instead of flying in
// from the taskbar. Reference-counted in case several cards restore at once.
int g_transientNoAnimationCount = 0;
bool g_transientNoAnimationChanged = false;

void BeginTransientNoAnimation(Card& c) {
    if (g_settings.nativeAnimation != Settings::NativeAnimation::RestoreOnly ||
        c.suppressingNativeAnimation) {
        return;
    }
    if (g_transientNoAnimationCount == 0) {
        g_transientNoAnimationChanged = false;
        ANIMATIONINFO ai = {sizeof(ai)};
        if (SystemParametersInfoW(SPI_GETANIMATION, sizeof(ai), &ai, 0) && ai.iMinAnimate) {
            g_transientNoAnimationChanged = SetMinAnimate(false);
        }
    }
    g_transientNoAnimationCount++;
    c.suppressingNativeAnimation = true;
}

void EndTransientNoAnimation(Card& c) {
    if (!c.suppressingNativeAnimation) return;
    c.suppressingNativeAnimation = false;
    if (--g_transientNoAnimationCount == 0 && g_transientNoAnimationChanged) {
        SetMinAnimate(true);
        g_transientNoAnimationChanged = false;
    }
}

// Where the user last dragged each window's card (this session only).
struct SavedCardPosition {
    HWND target;
    POINT topLeft;  // card top-left, header included
};
std::vector<SavedCardPosition> g_savedPositions;

void SaveCardPosition(HWND target, POINT topLeft) {
    for (auto& saved : g_savedPositions) {
        if (saved.target == target) {
            saved.topLeft = topLeft;
            return;
        }
    }
    g_savedPositions.push_back({target, topLeft});
}

const SavedCardPosition* FindSavedPosition(HWND target) {
    for (auto& saved : g_savedPositions) {
        if (saved.target == target) return &saved;
    }
    return nullptr;
}

// Grid layout: the order value of each window whose card was reordered by
// dragging (this session only). Order values are unique and only ever swapped
// between existing cards, so a returning card takes back exactly its old place
// relative to the cards still on the desk.
struct SavedCardOrder {
    HWND target;
    uint64_t order;
};
std::vector<SavedCardOrder> g_savedOrders;

void SaveCardOrder(HWND target, uint64_t order) {
    for (auto& saved : g_savedOrders) {
        if (saved.target == target) {
            saved.order = order;
            return;
        }
    }
    g_savedOrders.push_back({target, order});
}

const SavedCardOrder* FindSavedOrder(HWND target) {
    for (auto& saved : g_savedOrders) {
        if (saved.target == target) return &saved;
    }
    return nullptr;
}

void ForgetSavedPosition(HWND target) {
    g_savedPositions.erase(std::remove_if(g_savedPositions.begin(), g_savedPositions.end(),
                                          [target](const SavedCardPosition& saved) {
                                              return saved.target == target;
                                          }),
                           g_savedPositions.end());
    g_savedOrders.erase(std::remove_if(g_savedOrders.begin(), g_savedOrders.end(),
                                       [target](const SavedCardOrder& saved) {
                                           return saved.target == target;
                                       }),
                        g_savedOrders.end());
}

// ---------------------------------------------------------------------------
// Target window geometry

bool IsEligibleTarget(HWND hwnd) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd) || GetAncestor(hwnd, GA_ROOT) != hwnd ||
        IsOurWindow(hwnd)) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    bool appWindow = (exStyle & WS_EX_APPWINDOW) != 0;
    if ((style & WS_CAPTION) != WS_CAPTION && !appWindow) return false;
    if (GetWindow(hwnd, GW_OWNER) && !appWindow) return false;
    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked) {
        return false;
    }
    return true;
}

// Restored window rect in screen coordinates, plus the invisible resize borders
// that Windows 10/11 keep around most windows.
bool GetRestoredGeometry(HWND hwnd, RECT& windowRect, RECT& insets, bool& maximized) {
    WINDOWPLACEMENT wp = {sizeof(wp)};
    if (!GetWindowPlacement(hwnd, &wp)) return false;
    RECT r = wp.rcNormalPosition;
    HMONITOR monitor = MonitorFromRect(&r, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(monitor, &mi)) return false;
    // rcNormalPosition uses workspace coordinates for normal top-level windows.
    OffsetRect(&r, mi.rcWork.left - mi.rcMonitor.left, mi.rcWork.top - mi.rcMonitor.top);

    UINT dpi = GetDpiForWindow(hwnd);
    if (!dpi) dpi = 96;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    int frame = 0;
    if (style & WS_THICKFRAME) {
        frame = GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi) +
                GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
    } else if ((style & WS_CAPTION) == WS_CAPTION) {
        frame = GetSystemMetricsForDpi(SM_CXFIXEDFRAME, dpi) +
                GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
    }

    maximized = (wp.flags & WPF_RESTORETOMAXIMIZED) != 0;
    if (maximized) {
        HMONITOR m = MonitorFromRect(&r, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mmi = {sizeof(mmi)};
        GetMonitorInfoW(m, &mmi);
        windowRect = mmi.rcWork;
        InflateRect(&windowRect, frame, frame);
        insets = {frame, frame, frame, frame};
    } else {
        windowRect = r;
        insets = {frame, 0, frame, frame};
    }
    return RectW(windowRect) > 0 && RectH(windowRect) > 0;
}

std::wstring GetTitle(HWND hwnd) {
    wchar_t buffer[256] = {};
    // Never sends WM_GETTEXT, so a hung target can't block the worker thread.
    InternalGetWindowText(hwnd, buffer, ARRAYSIZE(buffer));
    return buffer;
}

// The returned icon belongs to the target app; callers copy it.
HICON GetWindowIcon(HWND hwnd) {
    DWORD_PTR result = 0;
    const WPARAM kinds[] = {ICON_SMALL2, ICON_SMALL, ICON_BIG};
    for (WPARAM kind : kinds) {
        if (SendMessageTimeoutW(hwnd, WM_GETICON, kind, 0, SMTO_ABORTIFHUNG, 100, &result) &&
            result) {
            return reinterpret_cast<HICON>(result);
        }
    }
    HICON icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICONSM));
    if (!icon) icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON));
    return icon;
}

// ---------------------------------------------------------------------------
// Z-order: resting cards sit right above the desktop (Progman / WorkerW).

// Topmost of the desktop's own windows (Progman and the WorkerW above it).
HWND GetDesktopTop() {
    HWND progman = GetShellWindow();
    if (!progman) return nullptr;
    HWND desktopTop = progman;
    for (HWND prev = GetWindow(progman, GW_HWNDPREV); prev; prev = GetWindow(prev, GW_HWNDPREV)) {
        if (IsClassName(prev, L"WorkerW") || IsClassName(prev, L"Progman")) {
            desktopTop = prev;
        } else {
            break;
        }
    }
    return desktopTop;
}

// While a card is dragged it stays at desktop level but above the other cards,
// so it doesn't slide underneath them.
HWND GetAboveCardsInsertAfter(HWND self) {
    HWND top = GetDesktopTop();
    if (!top) return nullptr;
    for (HWND prev = GetWindow(top, GW_HWNDPREV); prev && (prev == self || IsOurWindow(prev));
         prev = GetWindow(prev, GW_HWNDPREV)) {
        top = prev;
    }
    HWND above = GetWindow(top, GW_HWNDPREV);
    if (!above || (GetWindowLongPtrW(above, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        return HWND_NOTOPMOST;
    }
    return above;
}

HWND GetDesktopInsertAfter(HWND self) {
    HWND desktopTop = GetDesktopTop();
    if (!desktopTop) return nullptr;
    HWND above = GetWindow(desktopTop, GW_HWNDPREV);
    if (above == self) return nullptr;  // already in place
    // Inserting after a topmost window (e.g. the taskbar, when every normal
    // window is gone) would make the card topmost too. In that case nothing
    // normal is above the desktop, so the top of the normal band is correct.
    if (!above || (GetWindowLongPtrW(above, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        return HWND_NOTOPMOST;
    }
    return above;
}

void SendCardToDesktopLevel(Card& c) {
    // WM_WINDOWPOSCHANGING redirects HWND_BOTTOM to "just above the desktop".
    SetWindowPos(c.host, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// ---------------------------------------------------------------------------
// Virtual desktops
//
// Cards are tool windows, which Windows shows on every virtual desktop. The
// shell cloaks the windows of the other desktops (minimized ones included), so
// a card follows its target: hidden while the target is shell-cloaked. Windows
// pinned to all desktops are never cloaked, so their cards stay everywhere.

void UpdateThumbnail(Card& c);

bool IsOnOtherVirtualDesktop(HWND target) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(target, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           (cloaked & DWM_CLOAKED_SHELL);
}

void RelayoutGrid(bool animate);

void UpdateDesktopVisibility() {
    std::vector<Card*> shown;
    bool changed = false;
    for (auto& p : g_cards) {
        Card& c = *p;
        // Only resting cards: a card is always born on the current desktop, and
        // a card that just landed is checked by OnAnimationFinished.
        if (c.state != CardState::Resting || !c.host || !IsWindow(c.target)) continue;
        bool off = IsOnOtherVirtualDesktop(c.target);
        if (off == c.offDesktop) continue;
        changed = true;
        c.offDesktop = off;
        // Cloak first in both directions: hiding leaves no trace, and showing
        // doesn't flash a black rectangle before the thumbnail is composed.
        BOOL cloak = TRUE;
        DwmSetWindowAttribute(c.host, kDwmCloak, &cloak, sizeof(cloak));
        if (off) {
            if (GetCapture() == c.host) SendMessageW(c.host, WM_CANCELMODE, 0, 0);
            ShowWindow(c.host, SW_HIDE);
            c.hover = c.hoverClose = c.pressed = false;
            UpdateThumbnail(c);
        } else {
            shown.push_back(&c);
        }
    }
    // Grid: arrange this desktop's cards before any of them reappears. Cards
    // that just left only make the others slide into the gap.
    if (changed) RelayoutGrid(shown.empty());
    for (Card* c : shown) {
        SetWindowPos(c->host, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        SendCardToDesktopLevel(*c);
        UpdateWindow(c->host);
    }
    if (shown.empty()) return;
    DwmFlush();  // one wait for all the cards that reappear together
    BOOL uncloak = FALSE;
    for (Card* c : shown) DwmSetWindowAttribute(c->host, kDwmCloak, &uncloak, sizeof(uncloak));
}

void ScheduleDesktopCheck() {
    // Re-arming resets the delay, so a whole desktop switch is handled at once.
    if (HWND controller = g_controller.load()) {
        SetTimer(controller, TIMER_DESKTOP_CHECK, kDesktopCheckDelayMs, nullptr);
    }
}

// ---------------------------------------------------------------------------
// Layout

int HeaderPx(const Card& c) {
    return static_cast<int>(std::lround(c.headerFullPx * c.header));
}

RECT CloseButtonRect(const Card& c, int width, int headerPx) {
    UINT dpi = GetDpiForWindow(c.host);
    int w = MulDiv(34, dpi ? dpi : 96, 96);
    return {width - w, 0, width, headerPx};
}

RECT ComputeRestThumbRect(const Card& self, const RECT& visible, double aspect, UINT dpi) {
    HMONITOR monitor = MonitorFromRect(&visible, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    const RECT& work = mi.rcWork;
    double scale = dpi / 96.0;

    int tw = static_cast<int>(RectW(visible) * g_settings.scalePercent / 100.0);
    tw = std::min(tw, static_cast<int>(g_settings.maxWidth * scale));
    tw = std::max(tw, static_cast<int>(140 * scale));
    int th = std::max(1, static_cast<int>(tw / aspect));
    int header = self.headerFullPx;

    // "Falls" a bit below where the window was, as if dropping onto the desk.
    int cx = (visible.left + visible.right) / 2;
    int cy = (visible.top + visible.bottom) / 2 + (RectH(visible) - th) / 4;
    RECT card = {cx - tw / 2, cy - (th + header) / 2, 0, 0};
    card.right = card.left + tw;
    card.bottom = card.top + th + header;

    int margin = static_cast<int>(12 * scale);
    auto clampToWork = [&](RECT& r) {
        int dx = 0, dy = 0;
        if (r.right > work.right - margin) dx = work.right - margin - r.right;
        if (r.left + dx < work.left + margin) dx = work.left + margin - r.left;
        if (r.bottom > work.bottom - margin) dy = work.bottom - margin - r.bottom;
        if (r.top + dy < work.top + margin) dy = work.top + margin - r.top;
        OffsetRect(&r, dx, dy);
    };
    clampToWork(card);

    // Cascade away from cards that are already on the desk.
    int step = static_cast<int>(28 * scale);
    for (int attempt = 0; attempt < 32; attempt++) {
        bool overlaps = false;
        for (auto& other : g_cards) {
            if (other.get() == &self || other->offDesktop ||
                other->state == CardState::Vanishing ||
                other->state == CardState::Handoff || other->state == CardState::Revealing) {
                continue;
            }
            RECT otherCard = other->restThumbRect;
            otherCard.top -= other->headerFullPx;
            RECT tmp;
            if (IntersectRect(&tmp, &card, &otherCard)) {
                overlaps = true;
                break;
            }
        }
        if (!overlaps) break;
        OffsetRect(&card, step, step);
        if (card.right > work.right - margin) OffsetRect(&card, work.left + margin - card.left, 0);
        if (card.bottom > work.bottom - margin) OffsetRect(&card, 0, work.top + margin - card.top);
    }

    card.top += header;  // return the thumbnail area only
    return card;
}

// ---------------------------------------------------------------------------
// Grid layout
//
// Each monitor gets its own grid with the cards of its windows (on the current
// virtual desktop). Cards are arranged in rows of equal height, keeping each
// window's aspect ratio, centered in the work area, like Task View. Every
// possible number of rows is tried and the one giving the largest cards wins.

void ArmFrameTimer();

HMONITOR CardMonitor(const Card& c) {
    return MonitorFromRect(&c.visibleRect, MONITOR_DEFAULTTONEAREST);
}

bool TakesGridSlot(const Card& c) {
    return !c.offDesktop && (c.state == CardState::Resting || c.state == CardState::Falling);
}

void StartGlide(Card& c, const RECT& to) {
    RECT from;
    GetWindowRect(c.host, &from);
    from.top += c.headerFullPx;  // resting cards always show the full header
    if (EqualRect(&from, &to)) {
        c.gliding = false;
        return;
    }
    c.glideFrom = from;
    c.glideTo = to;
    c.glideStart = NowMs();
    c.gliding = true;
    ArmFrameTimer();
}

void ApplyFrame(Card& c, RECT thumbRect);

void LayoutMonitorGrid(HMONITOR monitor, std::vector<Card*>& group, bool animate) {
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(monitor, &mi)) return;
    const RECT& work = mi.rcWork;
    int header = 0;
    double maxAspect = 0.1;
    for (Card* c : group) {
        header = std::max(header, c->headerFullPx);
        maxAspect = std::max(maxAspect, c->aspect);
    }
    double scale = std::max(1, header) / static_cast<double>(kHeaderDip);
    double pad = 32 * scale, gap = 20 * scale;
    double areaW = RectW(work) - 2 * pad, areaH = RectH(work) - 2 * pad;
    int n = static_cast<int>(group.size());

    // Rows are consecutive runs of the ordered cards, as even as possible.
    auto rowSizes = [n](int rows) {
        std::vector<int> sizes(rows, n / rows);
        for (int i = 0; i < n % rows; i++) sizes[i]++;
        return sizes;
    };
    double bestH = -1;
    int bestRows = 1;
    for (int rows = 1; rows <= n; rows++) {
        double h = (areaH - gap * (rows - 1)) / rows - header;
        int index = 0;
        for (int size : rowSizes(rows)) {
            double aspectSum = 0;
            for (int i = 0; i < size; i++) aspectSum += group[index++]->aspect;
            h = std::min(h, (areaW - gap * (size - 1)) / aspectSum);
        }
        h = std::min(h, g_settings.gridMaxWidth * scale / maxAspect);
        if (h > bestH) {
            bestH = h;
            bestRows = rows;
        }
    }
    double h = std::max(bestH, 24 * scale);

    double totalH = bestRows * (h + header) + gap * (bestRows - 1);
    double y = work.top + pad + (areaH - totalH) / 2;
    int index = 0;
    for (int size : rowSizes(bestRows)) {
        double rowW = gap * (size - 1);
        for (int i = 0; i < size; i++) rowW += group[index + i]->aspect * h;
        double x = work.left + pad + (areaW - rowW) / 2;
        for (int i = 0; i < size; i++) {
            Card& c = *group[index + i];
            double w = c.aspect * h;
            RECT thumb = {static_cast<LONG>(std::lround(x)),
                          static_cast<LONG>(std::lround(y + header)),
                          static_cast<LONG>(std::lround(x + w)),
                          static_cast<LONG>(std::lround(y + header + h))};
            c.restThumbRect = thumb;
            if (c.state == CardState::Falling) {
                c.animTo = thumb;  // redirected mid-air if the grid changed
            } else if (&c != g_draggingCard) {
                if (animate) {
                    StartGlide(c, thumb);
                } else {
                    c.gliding = false;
                    ApplyFrame(c, thumb);
                }
            }
            x += w + gap;
        }
        index += size;
        y += h + header + gap;
    }
}

void RelayoutGrid(bool animate) {
    if (!g_settings.grid || g_shuttingDown) return;
    std::vector<std::pair<HMONITOR, std::vector<Card*>>> groups;
    for (auto& p : g_cards) {
        Card* c = p.get();
        if (!TakesGridSlot(*c) || !c->host) continue;
        HMONITOR monitor = CardMonitor(*c);
        auto it = std::find_if(groups.begin(), groups.end(),
                               [monitor](const auto& g) { return g.first == monitor; });
        if (it == groups.end()) {
            groups.push_back({monitor, {}});
            it = groups.end() - 1;
        }
        it->second.push_back(c);
    }
    for (auto& [monitor, group] : groups) {
        std::sort(group.begin(), group.end(),
                  [](const Card* a, const Card* b) { return a->order < b->order; });
        LayoutMonitorGrid(monitor, group, animate);
    }
}

void ScheduleRelayout() {
    if (HWND controller = g_controller.load()) {
        SetTimer(controller, TIMER_RELAYOUT, 150, nullptr);
    }
}

// Grid drag: swap with the card whose cell is under the cursor.
void OnGridDrag(Card& dragged) {
    POINT pt;
    GetCursorPos(&pt);
    double now = NowMs();
    HMONITOR monitor = CardMonitor(dragged);
    for (auto& p : g_cards) {
        Card* other = p.get();
        if (other == &dragged || other->state != CardState::Resting || other->offDesktop ||
            CardMonitor(*other) != monitor) {
            continue;
        }
        RECT cell = other->restThumbRect;
        cell.top -= other->headerFullPx;
        if (!PtInRect(&cell, pt)) continue;
        if (other == g_lastSwapWith || now - g_lastReorderMs < kReorderCooldownMs) return;
        std::swap(dragged.order, other->order);
        SaveCardOrder(dragged.target, dragged.order);
        SaveCardOrder(other->target, other->order);
        g_lastSwapWith = other;
        g_lastReorderMs = now;
        RelayoutGrid(true);
        return;
    }
    g_lastSwapWith = nullptr;  // the cursor left every other cell
}

// ---------------------------------------------------------------------------
// Rendering

double DisplayBrightness(const Card& c) {
    double b = c.bright;
    if (c.state == CardState::Resting) {
        if (c.closing) {
            b *= 0.5;
        } else if (c.hover) {
            b = std::min(1.0, b + 0.3);
        }
    }
    return b;
}

void UpdateThumbnail(Card& c) {
    if (!c.thumb) return;
    RECT client;
    GetClientRect(c.host, &client);
    DWM_THUMBNAIL_PROPERTIES p = {};
    p.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE | DWM_TNP_OPACITY |
                DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;
    p.rcDestination = {0, std::min(HeaderPx(c), static_cast<int>(client.bottom)), client.right,
                       client.bottom};
    p.rcSource = c.sourceCrop;
    p.opacity = static_cast<BYTE>(std::lround(Clamp01(DisplayBrightness(c)) * 255));
    p.fVisible = TRUE;
    p.fSourceClientAreaOnly = FALSE;
    DwmUpdateThumbnailProperties(c.thumb, &p);
}

void PaintCard(Card& c, HDC hdc) {
    RECT rc;
    GetClientRect(c.host, &rc);
    if (rc.right <= 0 || rc.bottom <= 0) return;
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
    HGDIOBJ oldBmp = SelectObject(mem, bmp);
    FillRect(mem, &rc, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

    int hh = HeaderPx(c);
    if (hh > 0) {
        UINT dpi = GetDpiForWindow(c.host);
        if (!dpi) dpi = 96;
        auto S = [dpi](int v) { return MulDiv(v, dpi, 96); };
        RECT header = {0, 0, rc.right, hh};
        HBRUSH bg = CreateSolidBrush(c.hover ? RGB(46, 46, 46) : RGB(28, 28, 28));
        FillRect(mem, &header, bg);
        DeleteObject(bg);

        if (hh >= c.headerFullPx * 0.7) {
            int x = S(7);
            if (c.icon) {
                int iconSize = std::min(S(16), hh - S(6));
                DrawIconEx(mem, x, (hh - iconSize) / 2, c.icon, iconSize, iconSize, 0, nullptr,
                           DI_NORMAL);
                x += iconSize + S(6);
            }
            RECT closeRect = CloseButtonRect(c, rc.right, hh);
            if (c.hoverClose) {
                HBRUSH red = CreateSolidBrush(RGB(196, 43, 28));
                FillRect(mem, &closeRect, red);
                DeleteObject(red);
            }
            HPEN pen = CreatePen(PS_SOLID, std::max(1, S(1)), RGB(230, 230, 230));
            HGDIOBJ oldPen = SelectObject(mem, pen);
            int cx = (closeRect.left + closeRect.right) / 2, cy = hh / 2, k = S(4);
            MoveToEx(mem, cx - k, cy - k, nullptr);
            LineTo(mem, cx + k + 1, cy + k + 1);
            MoveToEx(mem, cx + k, cy - k, nullptr);
            LineTo(mem, cx - k - 1, cy + k + 1);
            SelectObject(mem, oldPen);
            DeleteObject(pen);

            if (!c.font || c.fontDpi != dpi) {
                if (c.font) DeleteObject(c.font);
                NONCLIENTMETRICSW ncm = {sizeof(ncm)};
                if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0,
                                               dpi)) {
                    c.font = CreateFontIndirectW(&ncm.lfCaptionFont);
                } else {
                    c.font = CreateFontW(-MulDiv(9, dpi, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE,
                                         FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                         CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
                                         L"Segoe UI");
                }
                c.fontDpi = dpi;
            }
            HGDIOBJ oldFont = SelectObject(mem, c.font);
            SetBkMode(mem, TRANSPARENT);
            SetTextColor(mem, c.hover ? RGB(240, 240, 240) : RGB(170, 170, 170));
            RECT textRect = {x, 0, closeRect.left - S(4), hh};
            DrawTextW(mem, c.title.c_str(), -1, &textRect,
                      DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
            SelectObject(mem, oldFont);
        }
    }

    BitBlt(hdc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);
}

// Soft shadow drawn in a separate per-pixel-alpha window just below the card.
// Strong while the window is "in the air", gone once it lies on the desk.
void HideShadow(Card& c) {
    if (c.shadow) ShowWindow(c.shadow, SW_HIDE);
}

// The shadow bitmap grows to the largest size it was drawn at (up to the whole
// window at the start of the fall): free it once the card lands. UpdateShadow
// recreates the window and the bitmap on demand when the card rises.
void ReleaseShadow(Card& c) {
    if (c.shadow) DestroyWindow(c.shadow);
    c.shadow = nullptr;
    if (c.shadowDC) {
        SelectObject(c.shadowDC, c.shadowOldBmp);
        DeleteObject(c.shadowBmp);
        DeleteDC(c.shadowDC);
    }
    c.shadowDC = nullptr;
    c.shadowBmp = nullptr;
    c.shadowOldBmp = nullptr;
    c.shadowBits = nullptr;
    c.shadowCapW = c.shadowCapH = 0;
}

void UpdateShadow(Card& c, const RECT& cardRect) {
    if (c.shadowStrength < 0.02) {
        HideShadow(c);
        return;
    }
    UINT dpi = GetDpiForWindow(c.host);
    double scale = (dpi ? dpi : 96) / 96.0;
    double s = c.shadowStrength;
    int blur = std::max(2, static_cast<int>((8 + 26 * s) * scale));
    int offsetY = static_cast<int>((3 + 16 * s) * scale);
    double alphaMax = 0.55 * s;

    RECT r = cardRect;
    InflateRect(&r, blur, blur);
    OffsetRect(&r, 0, offsetY);
    int w = std::min(RectW(r), 8192), h = std::min(RectH(r), 8192);
    if (w <= 0 || h <= 0) return;

    if (!c.shadow) {
        c.shadow = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kLayeredClass,
            L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, ModuleInstance(), nullptr);
        if (!c.shadow) return;
    }
    if (w > c.shadowCapW || h > c.shadowCapH) {
        if (c.shadowDC) {
            SelectObject(c.shadowDC, c.shadowOldBmp);
            DeleteObject(c.shadowBmp);
            DeleteDC(c.shadowDC);
        }
        c.shadowCapW = std::max(w, c.shadowCapW);
        c.shadowCapH = std::max(h, c.shadowCapH);
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = c.shadowCapW;
        bi.bmiHeader.biHeight = -c.shadowCapH;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        c.shadowDC = CreateCompatibleDC(nullptr);
        c.shadowBmp = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!c.shadowDC || !c.shadowBmp) {
            if (c.shadowBmp) DeleteObject(c.shadowBmp);
            if (c.shadowDC) DeleteDC(c.shadowDC);
            c.shadowDC = nullptr;
            c.shadowBmp = nullptr;
            c.shadowBits = nullptr;
            c.shadowCapW = c.shadowCapH = 0;
            return;
        }
        c.shadowBits = static_cast<uint32_t*>(bits);
        c.shadowOldBmp = SelectObject(c.shadowDC, c.shadowBmp);
    }

    // Separable falloff: cheap and smooth enough for a soft drop shadow.
    auto edge = [blur](int i, int n) {
        double d = std::min(i, n - 1 - i) / (2.0 * blur);
        d = Clamp01(d);
        return d * d * (3 - 2 * d);
    };
    std::vector<double> fx(w);
    for (int x = 0; x < w; x++) fx[x] = edge(x, w);
    // Rows away from the top/bottom falloff are all identical: build one and
    // copy it, instead of recomputing millions of pixels per frame.
    std::vector<uint32_t> flatRow(w);
    for (int x = 0; x < w; x++) {
        flatRow[x] = static_cast<uint32_t>(fx[x] * alphaMax * 255.0) << 24;
    }
    for (int y = 0; y < h; y++) {
        uint32_t* row = c.shadowBits + static_cast<size_t>(y) * c.shadowCapW;
        double ey = edge(y, h);
        if (ey >= 1.0) {
            memcpy(row, flatRow.data(), static_cast<size_t>(w) * sizeof(uint32_t));
            continue;
        }
        double fy = ey * alphaMax * 255.0;
        for (int x = 0; x < w; x++) {
            row[x] = static_cast<uint32_t>(fx[x] * fy) << 24;  // premultiplied black
        }
    }

    POINT dst = {r.left, r.top};
    SIZE size = {w, h};
    POINT src = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(c.shadow, nullptr, &dst, &size, c.shadowDC, &src, 0, &blend, ULW_ALPHA);
    SetWindowPos(c.shadow, c.host, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void ApplyFrame(Card& c, RECT thumbRect) {
    RECT cardRect = thumbRect;
    cardRect.top -= HeaderPx(c);
    int w = std::max(1, RectW(cardRect)), h = std::max(1, RectH(cardRect));
    SetWindowPos(c.host, nullptr, cardRect.left, cardRect.top, w, h,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOCOPYBITS);
    UpdateThumbnail(c);
    InvalidateRect(c.host, nullptr, FALSE);
    UpdateShadow(c, cardRect);
}

RECT CurrentThumbRect(const Card& c) {
    RECT r;
    GetWindowRect(c.host, &r);
    r.top += HeaderPx(c);
    return r;
}

// ---------------------------------------------------------------------------
// Smooth reveal: a frozen screenshot of the card, topmost and click-through.
// The live thumbnail would mirror the window's own flicker while Windows
// rebuilds its Mica/Acrylic backdrop, so a static image is used instead.

void SetOverlayAlpha(Card& c, BYTE alpha) {
    if (!c.overlay) return;
    POINT src = {0, 0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    UpdateLayeredWindow(c.overlay, nullptr, &c.overlayPos, &c.overlaySize, c.overlayDC, &src, 0,
                        &blend, ULW_ALPHA);
}

bool CreateRevealOverlay(Card& c) {
    RECT r;
    GetWindowRect(c.host, &r);
    int w = RectW(r), h = RectH(r);
    if (w <= 0 || h <= 0) return false;

    DwmFlush();  // make sure the card's final frame is on screen before copying it

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bmp = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dc || !bmp) {
        if (bmp) DeleteObject(bmp);
        if (dc) DeleteDC(dc);
        return false;
    }
    HGDIOBJ old = SelectObject(dc, bmp);
    HDC screen = GetDC(nullptr);
    // No CAPTUREBLT: it makes layered windows and the cursor flicker.
    BOOL copied = BitBlt(dc, 0, 0, w, h, screen, r.left, r.top, SRCCOPY);
    ReleaseDC(nullptr, screen);
    if (!copied) {
        SelectObject(dc, old);
        DeleteObject(bmp);
        DeleteDC(dc);
        return false;
    }
    GdiFlush();
    // GDI leaves alpha at 0: make the snapshot fully opaque.
    uint32_t* px = static_cast<uint32_t*>(bits);
    for (size_t i = 0, n = static_cast<size_t>(w) * h; i < n; i++) px[i] |= 0xFF000000u;

    c.overlay = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                                    WS_EX_NOACTIVATE | WS_EX_TOPMOST,
                                kLayeredClass, L"", WS_POPUP, r.left, r.top, w, h, nullptr, nullptr,
                                ModuleInstance(), nullptr);
    if (!c.overlay) {
        SelectObject(dc, old);
        DeleteObject(bmp);
        DeleteDC(dc);
        return false;
    }
    c.overlayDC = dc;
    c.overlayBmp = bmp;
    c.overlayOldBmp = old;
    c.overlayPos = {r.left, r.top};
    c.overlaySize = {w, h};
    SetOverlayAlpha(c, 255);
    SetWindowPos(c.overlay, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    return true;
}

void DestroyRevealOverlay(Card& c) {
    if (c.overlay) DestroyWindow(c.overlay);
    c.overlay = nullptr;
    if (c.overlayDC) {
        SelectObject(c.overlayDC, c.overlayOldBmp);
        DeleteObject(c.overlayBmp);
        DeleteDC(c.overlayDC);
    }
    c.overlayDC = nullptr;
    c.overlayBmp = nullptr;
}

// ---------------------------------------------------------------------------
// Animation clock (high-resolution waitable timer at the display refresh rate)

void ArmFrameTimer() {
    if (g_frameTimerArmed || !g_frameTimer) return;
    double refresh = 60.0;
    DWM_TIMING_INFO ti = {sizeof(ti)};
    if (SUCCEEDED(DwmGetCompositionTimingInfo(nullptr, &ti)) && ti.rateRefresh.uiDenominator &&
        ti.rateRefresh.uiNumerator) {
        refresh = static_cast<double>(ti.rateRefresh.uiNumerator) / ti.rateRefresh.uiDenominator;
    }
    refresh = std::clamp(refresh, 30.0, 240.0);
    LARGE_INTEGER due;
    due.QuadPart = -static_cast<LONGLONG>(10'000'000.0 / refresh);
    if (SetWaitableTimer(g_frameTimer, &due, 0, nullptr, nullptr, FALSE)) {
        g_frameTimerArmed = true;
    }
}

void StartAnimation(Card& c, CardState state, int durationMs) {
    c.state = state;
    c.animStart = NowMs();
    c.animMs = std::max(1, durationMs);
    ArmFrameTimer();
}

void DestroyCard(Card* card);

void StartVanishing(Card& c) {
    KillTimer(c.host, TIMER_HANDOFF);
    KillTimer(c.host, TIMER_CLOSE_CHECK);
    c.animFrom = CurrentThumbRect(c);
    c.animTo = ScaleAroundCenter(c.animFrom, 0.75);
    c.brightFrom = DisplayBrightness(c);
    c.brightTo = 0;
    c.headerFrom = c.header;
    c.shadowStrength = 0;
    c.gliding = false;
    HideShadow(c);
    StartAnimation(c, CardState::Vanishing, 180);
    RelayoutGrid(true);  // the others close the gap
}

void StartRising(Card& c) {
    if (c.state != CardState::Resting) return;
    RECT windowRect, insets;
    bool maximized = false;
    if (GetRestoredGeometry(c.target, windowRect, insets, maximized)) {
        c.visibleRect = {windowRect.left + insets.left, windowRect.top + insets.top,
                         windowRect.right - insets.right, windowRect.bottom - insets.bottom};
    }
    // Current position rather than the resting one: the card may be gliding.
    c.gliding = false;
    c.animFrom = CurrentThumbRect(c);
    c.animTo = c.visibleRect;
    c.brightFrom = DisplayBrightness(c);
    c.brightTo = 1.0;
    c.shadowFrom = 0;
    c.shadowTo = 1.0;
    c.headerFrom = c.header;
    c.hover = c.hoverClose = false;
    // Change state first: while Resting, WM_WINDOWPOSCHANGING pins the card to the desk.
    StartAnimation(c, CardState::Rising, std::max(200, g_settings.animationMs * 3 / 4));
    SetWindowPos(c.host, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    RelayoutGrid(true);
}

void OnAnimationFinished(Card& c) {
    switch (c.state) {
        case CardState::Falling:
            c.state = CardState::Resting;
            c.header = 1;
            c.bright = c.brightTo;
            c.shadowStrength = 0;
            ApplyFrame(c, c.restThumbRect);
            ReleaseShadow(c);
            SendCardToDesktopLevel(c);
            ScheduleDesktopCheck();  // the desktop may have changed during the fall
            break;
        case CardState::Rising: {
            c.state = CardState::Handoff;
            ReleaseShadow(c);
            if (g_settings.smoothReveal && CreateRevealOverlay(c)) {
                // The snapshot now covers everything: the live card is no longer needed.
                if (c.thumb) {
                    DwmUnregisterThumbnail(c.thumb);
                    c.thumb = nullptr;
                }
                DwmFlush();  // snapshot on screen before the live card goes away
                ShowWindow(c.host, SW_HIDE);
            }
            // Let the app restore itself through its own WM_SYSCOMMAND handling,
            // like the taskbar does. Forcing foreground onto a still-minimized
            // window and restoring it from outside confuses Chromium/Electron
            // apps, which then minimize themselves again a few seconds later.
            DWORD pid = 0;
            GetWindowThreadProcessId(c.target, &pid);
            AllowSetForegroundWindow(pid);
            BeginTransientNoAnimation(c);
            if (!PostMessageW(c.target, WM_SYSCOMMAND, SC_RESTORE, 0)) {
                // e.g. an elevated window: UIPI blocks posted messages.
                Wh_Log(L"SC_RESTORE post failed (%lu), falling back to ShowWindowAsync",
                       GetLastError());
                ShowWindowAsync(c.target, SW_RESTORE);
            }
            // Normally replaced on EVENT_SYSTEM_MINIMIZEEND; this is the fallback.
            SetTimer(c.host, c.overlay ? TIMER_REVEAL : TIMER_HANDOFF, 1500, nullptr);
            break;
        }
        case CardState::Revealing:
        case CardState::Vanishing:
            DestroyCard(&c);
            break;
        default:
            break;
    }
}

bool TickAnimations() {
    double now = NowMs();
    std::vector<Card*> finished;
    for (auto& up : g_cards) {
        Card& c = *up;
        if (c.state == CardState::Resting && c.gliding) {
            double t = Clamp01((now - c.glideStart) / kGlideMs);
            ApplyFrame(c, LerpRect(c.glideFrom, c.glideTo, EaseOutCubic(t)));
            if (t >= 1.0) c.gliding = false;
            continue;
        }
        if (c.state != CardState::Falling && c.state != CardState::Rising &&
            c.state != CardState::Revealing && c.state != CardState::Vanishing) {
            continue;
        }
        double t = Clamp01((now - c.animStart) / c.animMs);
        if (c.state == CardState::Revealing) {
            // Ease-in: the snapshot lingers a moment, then melts into the window.
            SetOverlayAlpha(c, static_cast<BYTE>(std::lround(255 * (1 - t * t))));
            if (t >= 1.0) finished.push_back(&c);
            continue;
        }
        RECT r = {};
        if (c.state == CardState::Falling) {
            // Gravity-like acceleration, a slight tip backwards while falling,
            // and a small squash when it lands.
            const double land = 0.78;
            double p = t < land ? t / land : 1.0;
            double e = p * p;
            r = LerpRect(c.animFrom, c.animTo, e);
            double wf = 1.0, hf = 1.0 - 0.12 * std::sin(kPi * e);
            double headerT = 0;
            if (t > land) {
                double u = (t - land) / (1 - land);
                double squash = std::sin(kPi * u);
                wf = 1.0 + 0.035 * squash;
                hf *= 1.0 - 0.06 * squash;
                headerT = u;
            }
            r = ScaleAnchorBottom(r, wf, hf);
            c.bright = Lerp(c.brightFrom, c.brightTo, e);
            c.shadowStrength = Lerp(c.shadowFrom, c.shadowTo, e);
            c.header = headerT;
        } else if (c.state == CardState::Rising) {
            double e = EaseOutCubic(t);
            r = LerpRect(c.animFrom, c.animTo, e);
            r = ScaleAnchorBottom(r, 1.0, 1.0 - 0.08 * std::sin(kPi * e));
            c.bright = Lerp(c.brightFrom, c.brightTo, e);
            c.shadowStrength = Lerp(c.shadowFrom, c.shadowTo, e);
            c.header = c.headerFrom * (1.0 - Clamp01(t / 0.2));
        } else {
            r = LerpRect(c.animFrom, c.animTo, t * t);
            c.bright = Lerp(c.brightFrom, c.brightTo, t);
            c.header = c.headerFrom * (1.0 - t);
        }
        ApplyFrame(c, r);
        if (t >= 1.0) finished.push_back(&c);
    }
    for (Card* c : finished) OnAnimationFinished(*c);
    return AnyAnimating();
}

// ---------------------------------------------------------------------------
// Card lifetime

void DestroyCard(Card* card) {
    auto it = std::find_if(g_cards.begin(), g_cards.end(),
                           [card](const std::unique_ptr<Card>& p) { return p.get() == card; });
    if (it == g_cards.end()) return;
    std::unique_ptr<Card> owned = std::move(*it);
    g_cards.erase(it);
    if (g_draggingCard == owned.get()) g_draggingCard = nullptr;
    if (g_lastSwapWith == owned.get()) g_lastSwapWith = nullptr;
    RelayoutGrid(true);
    EndTransientNoAnimation(*owned);
    if (owned->thumb) DwmUnregisterThumbnail(owned->thumb);
    DestroyRevealOverlay(*owned);
    if (owned->host) DestroyWindow(owned->host);
    ReleaseShadow(*owned);
    if (owned->icon) DestroyIcon(owned->icon);
    if (owned->font) DeleteObject(owned->font);
}

void CreateCard(HWND target, bool animate) {
    if (Card* existing = FindCardByTarget(target)) {
        if (existing->state == CardState::Handoff || existing->state == CardState::Revealing ||
            existing->state == CardState::Vanishing) {
            DestroyCard(existing);
        } else {
            return;
        }
    }
    RECT windowRect, insets;
    bool maximized = false;
    if (!GetRestoredGeometry(target, windowRect, insets, maximized)) return;
    if (RectW(windowRect) < 100 || RectH(windowRect) < 80) return;

    auto card = std::make_unique<Card>();
    Card& c = *card;
    c.target = target;
    c.visibleRect = {windowRect.left + insets.left, windowRect.top + insets.top,
                     windowRect.right - insets.right, windowRect.bottom - insets.bottom};

    c.host = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kCardClass, L"", WS_POPUP,
                             c.visibleRect.left, c.visibleRect.top, RectW(c.visibleRect),
                             RectH(c.visibleRect), nullptr, nullptr, ModuleInstance(), &c);
    if (!c.host) return;
    int corner = kDwmCornerRoundSmall;
    DwmSetWindowAttribute(c.host, kDwmCornerPreference, &corner, sizeof(corner));
    // Keep the card cloaked until its first real frame (black background +
    // thumbnail) has been composed, otherwise a bare black rectangle flashes.
    BOOL cloak = TRUE;
    bool cloaked =
        SUCCEEDED(DwmSetWindowAttribute(c.host, kDwmCloak, &cloak, sizeof(cloak)));
    auto uncloakWhenReady = [&]() {
        if (!cloaked) return;
        UpdateWindow(c.host);
        DwmFlush();
        BOOL off = FALSE;
        DwmSetWindowAttribute(c.host, kDwmCloak, &off, sizeof(off));
    };

    HRESULT thumbnailResult = DwmRegisterThumbnail(c.host, target, &c.thumb);
    if (FAILED(thumbnailResult)) {
        Wh_Log(L"DwmRegisterThumbnail failed for %p: 0x%08X", target,
               static_cast<unsigned>(thumbnailResult));
        DestroyWindow(c.host);
        return;
    }

    // Map the invisible borders into thumbnail-source coordinates.
    SIZE source = {};
    if (FAILED(DwmQueryThumbnailSourceSize(c.thumb, &source)) || source.cx < 50 || source.cy < 50) {
        source = {RectW(windowRect), RectH(windowRect)};
    }
    double sx = static_cast<double>(source.cx) / RectW(windowRect);
    double sy = static_cast<double>(source.cy) / RectH(windowRect);
    c.sourceCrop = {static_cast<LONG>(insets.left * sx), static_cast<LONG>(insets.top * sy),
                    static_cast<LONG>(source.cx - insets.right * sx),
                    static_cast<LONG>(source.cy - insets.bottom * sy)};
    double aspect = static_cast<double>(RectW(c.sourceCrop)) / std::max(1, RectH(c.sourceCrop));
    c.aspect = std::clamp(aspect, 0.25, 4.0);
    const SavedCardOrder* savedOrder = FindSavedOrder(target);
    c.order = g_settings.rememberPosition && savedOrder ? savedOrder->order : g_nextOrder++;
    Wh_Log(L"Card for %p: window %dx%d, source %dx%d, maximized=%d", target, RectW(windowRect),
           RectH(windowRect), source.cx, source.cy, maximized);

    UINT dpi = GetDpiForWindow(c.host);
    if (!dpi) dpi = 96;
    c.headerFullPx = MulDiv(kHeaderDip, dpi, 96);
    c.title = GetTitle(target);
    if (HICON icon = GetWindowIcon(target)) c.icon = CopyIcon(icon);
    // GetWindowIcon may have waited on the target: make sure it still exists.
    if (!IsWindow(target)) {
        if (c.icon) DestroyIcon(c.icon);
        DwmUnregisterThumbnail(c.thumb);
        DestroyWindow(c.host);
        return;
    }
    c.restThumbRect = ComputeRestThumbRect(c, c.visibleRect, aspect, dpi);
    if (g_settings.rememberPosition && !g_settings.grid) {
        if (const SavedCardPosition* saved = FindSavedPosition(target)) {
            // Same size as computed, placed where the user left it last time,
            // kept on screen in case monitors changed.
            RECT card = c.restThumbRect;
            card.top -= c.headerFullPx;
            OffsetRect(&card, saved->topLeft.x - card.left, saved->topLeft.y - card.top);
            HMONITOR monitor = MonitorFromPoint(saved->topLeft, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = {sizeof(mi)};
            if (GetMonitorInfoW(monitor, &mi)) {
                const RECT& work = mi.rcWork;
                int dx = 0, dy = 0;
                if (card.right > work.right) dx = work.right - card.right;
                if (card.left + dx < work.left) dx = work.left - card.left;
                if (card.bottom > work.bottom) dy = work.bottom - card.bottom;
                if (card.top + dy < work.top) dy = work.top - card.top;
                OffsetRect(&card, dx, dy);
            }
            card.top += c.headerFullPx;
            c.restThumbRect = card;
        }
    }

    double restBrightness = g_settings.brightness / 100.0;
    g_cards.push_back(std::move(card));

    if (animate) {
        c.animFrom = c.visibleRect;
        c.animTo = c.restThumbRect;
        c.brightFrom = 1.0;
        c.brightTo = restBrightness;
        c.shadowFrom = 1.0;
        c.shadowTo = 0.0;
        c.bright = 1.0;
        // The shadow window is not cloaked: draw it from the first animation
        // frame on, once the card itself is visible, or it flashes alone.
        c.shadowStrength = 0.0;
        c.header = 0;
        ApplyFrame(c, c.animFrom);
        SetWindowPos(c.host, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        uncloakWhenReady();
        StartAnimation(c, CardState::Falling, g_settings.animationMs);
        RelayoutGrid(true);  // picks this card's cell and makes room for it
    } else {
        c.state = CardState::Resting;
        c.bright = restBrightness;
        c.brightTo = restBrightness;
        c.header = 1;
        RelayoutGrid(false);
        ApplyFrame(c, c.restThumbRect);
        ShowWindow(c.host, SW_SHOWNOACTIVATE);
        SendCardToDesktopLevel(c);
        uncloakWhenReady();
    }
}

void RequestClose(Card& c) {
    if (!g_settings.closeWindow) {
        StartVanishing(c);
        return;
    }
    if (!PostMessageW(c.target, WM_SYSCOMMAND, SC_CLOSE, 0)) {
        // Typically an elevated window (UIPI): bring it back so the user can close it.
        Wh_Log(L"SC_CLOSE post failed (%lu), restoring the window instead", GetLastError());
        StartRising(c);
        return;
    }
    c.closing = true;
    UpdateThumbnail(c);
    InvalidateRect(c.host, nullptr, FALSE);
    // If the app is still alive after a moment it is probably asking something
    // (e.g. "save changes?"); bring it back so the prompt is visible.
    SetTimer(c.host, TIMER_CLOSE_CHECK, 1500, nullptr);
}

void OnTargetRestored(Card& c) {
    if (c.state == CardState::Handoff) {
        EndTransientNoAnimation(c);
        // Activate only now that the window is really restored.
        if (GetForegroundWindow() != c.target) SetForegroundWindow(c.target);
        if (c.overlay) {
            // Keep the snapshot on top while the backdrop is being rebuilt.
            KillTimer(c.host, TIMER_REVEAL);
            SetTimer(c.host, TIMER_REVEAL, std::max(1, g_settings.revealDelayMs), nullptr);
        } else {
            SetTimer(c.host, TIMER_HANDOFF, 120, nullptr);
        }
    } else if (c.state != CardState::Vanishing && c.state != CardState::Revealing) {
        StartVanishing(c);
    }
}

void SetHover(Card& c, bool hover, bool hoverClose) {
    if (c.hover == hover && c.hoverClose == hoverClose) return;
    c.hover = hover;
    c.hoverClose = hoverClose;
    UpdateThumbnail(c);
    InvalidateRect(c.host, nullptr, FALSE);
}

// ---------------------------------------------------------------------------
// Window procedures

LRESULT CALLBACK CardWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    Card* c = reinterpret_cast<Card*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (!c) return DefWindowProcW(hwnd, msg, wParam, lParam);

    switch (msg) {
        case WM_NCDESTROY:
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            break;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            PaintCard(*c, dc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_WINDOWPOSCHANGING: {
            auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
            if (c->state == CardState::Resting && !(wp->flags & SWP_NOZORDER)) {
                HWND after = g_draggingCard == c ? GetAboveCardsInsertAfter(hwnd)
                                                 : GetDesktopInsertAfter(hwnd);
                if (after) {
                    wp->hwndInsertAfter = after;
                } else {
                    wp->flags |= SWP_NOZORDER;
                }
            }
            break;
        }
        case WM_ENTERSIZEMOVE:
            if (c->state == CardState::Resting) {
                g_draggingCard = c;
                g_lastSwapWith = nullptr;
                c->gliding = false;
                // Redirected by WM_WINDOWPOSCHANGING to just above the other cards.
                SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
            return 0;
        case WM_MOVING:
            if (g_settings.grid && g_draggingCard == c && c->state == CardState::Resting) {
                OnGridDrag(*c);
            }
            break;
        case WM_EXITSIZEMOVE: {
            g_draggingCard = nullptr;
            g_lastSwapWith = nullptr;
            SendCardToDesktopLevel(*c);
            if (g_settings.grid) {
                RelayoutGrid(true);  // slide into its (possibly new) cell
                return 0;
            }
            RECT r;
            GetWindowRect(hwnd, &r);
            SaveCardPosition(c->target, {r.left, r.top});
            r.top += c->headerFullPx;
            c->restThumbRect = r;
            return 0;
        }
        case WM_NCHITTEST: {
            if (c->state != CardState::Resting) return HTCLIENT;
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hwnd, &pt);
            RECT rc;
            GetClientRect(hwnd, &rc);
            int hh = HeaderPx(*c);
            RECT closeRect = CloseButtonRect(*c, rc.right, hh);
            if (pt.y < hh && !PtInRect(&closeRect, pt)) return HTCAPTION;
            return HTCLIENT;
        }
        case WM_NCLBUTTONDBLCLK:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONUP:
        case WM_CONTEXTMENU:
            return 0;
        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                bool onThumb = pt.y >= HeaderPx(*c) && c->state == CardState::Resting;
                SetCursor(LoadCursorW(nullptr, onThumb ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            break;
        case WM_MOUSEMOVE: {
            if (c->state != CardState::Resting) return 0;
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            RECT rc;
            GetClientRect(hwnd, &rc);
            int hh = HeaderPx(*c);
            RECT closeRect = CloseButtonRect(*c, rc.right, hh);
            SetHover(*c, true, PtInRect(&closeRect, pt) != FALSE);
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_NCMOUSEMOVE: {
            if (c->state == CardState::Resting) {
                SetHover(*c, true, false);
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE | TME_NONCLIENT, hwnd, 0};
                TrackMouseEvent(&tme);
            }
            break;
        }
        case WM_MOUSELEAVE:
        case WM_NCMOUSELEAVE: {
            POINT pt;
            GetCursorPos(&pt);
            if (WindowFromPoint(pt) != hwnd) SetHover(*c, false, false);
            return 0;
        }
        case WM_LBUTTONDOWN:
            if (c->state == CardState::Resting) {
                c->pressed = true;
                SetCapture(hwnd);
            }
            return 0;
        case WM_LBUTTONUP: {
            if (!c->pressed) return 0;
            c->pressed = false;
            ReleaseCapture();
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (c->state != CardState::Resting || !PtInRect(&rc, pt)) return 0;
            int hh = HeaderPx(*c);
            RECT closeRect = CloseButtonRect(*c, rc.right, hh);
            if (PtInRect(&closeRect, pt)) {
                RequestClose(*c);
            } else if (pt.y >= hh) {
                StartRising(*c);
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
            c->pressed = false;
            return 0;
        case WM_TIMER:
            KillTimer(hwnd, wParam);
            if (wParam == TIMER_HANDOFF || wParam == TIMER_REVEAL) {
                EndTransientNoAnimation(*c);  // fallback if MINIMIZEEND never came
            }
            if (wParam == TIMER_HANDOFF) {
                DestroyCard(c);  // c is gone after this
            } else if (wParam == TIMER_REVEAL && c->state == CardState::Handoff) {
                StartAnimation(*c, CardState::Revealing, g_settings.revealFadeMs);
            } else if (wParam == TIMER_CLOSE_CHECK && c->state == CardState::Resting) {
                c->closing = false;
                if (IsWindow(c->target) && IsIconic(c->target) && !c->offDesktop) {
                    StartRising(*c);
                } else {
                    UpdateThumbnail(*c);
                }
            }
            return 0;
        case WM_DPICHANGED:
            return 0;  // keep the card size stable when dragged across monitors
        // Broadcasts reach top-level windows only, not the message-only
        // controller: any card forwards them (coalesced by the timer).
        case WM_DISPLAYCHANGE:
            ScheduleRelayout();
            break;
        case WM_SETTINGCHANGE:
            if (wParam == SPI_SETWORKAREA) ScheduleRelayout();
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void HandleWinEvent(DWORD event, HWND hwnd);

LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_RELOAD:
            LoadSettings();
            ApplyNativeAnimationSetting(false);
            for (auto& c : g_cards) {
                if (c->state == CardState::Resting) {
                    c->bright = g_settings.brightness / 100.0;
                    UpdateThumbnail(*c);
                }
            }
            // Switching to grid arranges the cards already on the desk.
            // Switching back to floating leaves them where they are.
            RelayoutGrid(true);
            return 0;
        case WM_APP_WINEVENT:
            HandleWinEvent(static_cast<DWORD>(wParam), reinterpret_cast<HWND>(lParam));
            return 0;
        case WM_TIMER:
            if (wParam == TIMER_RELAYOUT) {
                KillTimer(hwnd, wParam);
                RelayoutGrid(true);
                return 0;
            }
            if (wParam == TIMER_DESKTOP_CHECK) KillTimer(hwnd, wParam);
            if (wParam == TIMER_DESKTOP_CHECK || wParam == TIMER_DESKTOP_POLL) {
                UpdateDesktopVisibility();
            }
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void HandleWinEvent(DWORD event, HWND hwnd) {
    switch (event) {
        case EVENT_SYSTEM_MINIMIZESTART:
            if (IsEligibleTarget(hwnd)) CreateCard(hwnd, true);
            break;
        case EVENT_SYSTEM_MINIMIZEEND:
            if (Card* c = FindCardByTarget(hwnd)) OnTargetRestored(*c);
            break;
        case EVENT_OBJECT_DESTROY:
            ForgetSavedPosition(hwnd);
            if (Card* c = FindCardByTarget(hwnd)) DestroyCard(c);
            break;
        case EVENT_OBJECT_HIDE:
            // e.g. apps that hide to the notification area after minimizing.
            if (Card* c = FindCardByTarget(hwnd)) {
                if (c->state != CardState::Handoff && c->state != CardState::Revealing &&
                    c->state != CardState::Vanishing) {
                    StartVanishing(*c);
                }
            }
            break;
        case EVENT_OBJECT_CLOAKED:
        case EVENT_OBJECT_UNCLOAKED:
            // Virtual desktop switch, or a window moved to another desktop.
            ScheduleDesktopCheck();
            break;
        case EVENT_OBJECT_NAMECHANGE:
            if (Card* c = FindCardByTarget(hwnd)) {
                c->title = GetTitle(hwnd);
                InvalidateRect(c->host, nullptr, FALSE);
            }
            break;
    }
}

// Out-of-context WinEvents can be delivered re-entrantly while this thread
// waits inside a cross-process call (e.g. SendMessageTimeout in CreateCard).
// Posting them to the controller serializes all card state changes through
// the message loop.
void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild,
                           DWORD, DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (event == EVENT_OBJECT_SHOW) return;  // only part of the hooked range
    if (event == EVENT_OBJECT_DESTROY || event == EVENT_OBJECT_HIDE ||
        event == EVENT_OBJECT_NAMECHANGE || event == EVENT_OBJECT_CLOAKED ||
        event == EVENT_OBJECT_UNCLOAKED) {
        // Very frequent system-wide: only forward windows we care about.
        if (!FindCardByTarget(hwnd) && !FindSavedPosition(hwnd) && !FindSavedOrder(hwnd)) {
            return;
        }
    }
    if (HWND controller = g_controller.load()) {
        PostMessageW(controller, WM_APP_WINEVENT, event, reinterpret_cast<LPARAM>(hwnd));
    }
}

BOOL CALLBACK CollectMinimizedWindow(HWND hwnd, LPARAM lParam) {
    if (IsWindowVisible(hwnd) && IsIconic(hwnd) && IsEligibleTarget(hwnd)) {
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hwnd);
    }
    return TRUE;
}

// ---------------------------------------------------------------------------
// Worker thread

DWORD WINAPI WorkerThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    LoadSettings();

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.hInstance = ModuleInstance();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpfnWndProc = CardWndProc;
    wc.lpszClassName = kCardClass;
    RegisterClassExW(&wc);
    wc.lpfnWndProc = DefWindowProcW;
    wc.lpszClassName = kLayeredClass;
    RegisterClassExW(&wc);
    wc.lpfnWndProc = ControllerWndProc;
    wc.lpszClassName = kControllerClass;
    RegisterClassExW(&wc);

    g_frameTimer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                          TIMER_ALL_ACCESS);
    if (!g_frameTimer) g_frameTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);

    HWND controller = CreateWindowExW(0, kControllerClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                                      nullptr, ModuleInstance(), nullptr);
    g_controller = controller;

    if (controller && !g_stop.load()) {
        const DWORD flags = WINEVENT_OUTOFCONTEXT;
        g_hooks[0] = SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, nullptr,
                                     WinEventProc, 0, 0, flags);
        // DESTROY..HIDE (SHOW in between is filtered out by the callback).
        g_hooks[1] = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, nullptr,
                                     WinEventProc, 0, 0, flags);
        g_hooks[2] = SetWinEventHook(EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE, nullptr,
                                     WinEventProc, 0, 0, flags);
        g_hooks[3] = SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                                     WinEventProc, 0, 0, flags);
        SetTimer(controller, TIMER_DESKTOP_POLL, kDesktopPollMs, nullptr);
        RecoverNativeAnimationAfterCrash();
        ApplyNativeAnimationSetting(false);

        if (g_settings.includeExisting) {
            std::vector<HWND> minimized;
            EnumWindows(CollectMinimizedWindow, reinterpret_cast<LPARAM>(&minimized));
            for (HWND hwnd : minimized) CreateCard(hwnd, false);
        }
        Wh_Log(L"Desktop Window Cards running, %zu existing cards", g_cards.size());

        bool quit = false;
        while (!quit) {
            DWORD r = MsgWaitForMultipleObjectsEx(1, &g_frameTimer, INFINITE, QS_ALLINPUT,
                                                  MWMO_INPUTAVAILABLE);
            if (r == WAIT_OBJECT_0) {
                g_frameTimerArmed = false;
                if (TickAnimations()) ArmFrameTimer();
            }
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    quit = true;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    for (HWINEVENTHOOK& hook : g_hooks) {
        if (hook) UnhookWinEvent(hook);
        hook = nullptr;
    }
    g_shuttingDown = true;
    while (!g_cards.empty()) DestroyCard(g_cards.back().get());
    ApplyNativeAnimationSetting(true);
    g_controller = nullptr;
    if (IsWindow(controller)) DestroyWindow(controller);
    if (g_frameTimer) CloseHandle(g_frameTimer);
    g_frameTimer = nullptr;
    UnregisterClassW(kCardClass, ModuleInstance());
    UnregisterClassW(kLayeredClass, ModuleInstance());
    UnregisterClassW(kControllerClass, ModuleInstance());
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");
    g_stop = false;
    g_shuttingDown = false;
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    g_stop = true;
    if (HWND controller = g_controller.load()) PostMessageW(controller, WM_CLOSE, 0, 0);
    if (g_thread) {
        // The module is unloaded right after this returns: the worker must be gone.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    if (HWND controller = g_controller.load()) PostMessageW(controller, WM_APP_RELOAD, 0, 0);
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
