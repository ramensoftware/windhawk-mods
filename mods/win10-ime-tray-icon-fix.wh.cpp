// ==WindhawkMod==
// @id              win10-ime-tray-icon-fix
// @name            Windows 10 IME Tray Icon Fix
// @description     Moves the input indicator to the left of the notification area so its width change cannot shift the tray icons. Alternative modes provided.
// @version         1.1
// @author          yumeyao
// @github          https://github.com/yumeyao
// @include         explorer.exe
// @compilerOptions -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 IME Tray Icon Fix

## Default Stock Layout And The Bug

![Before Fix](https://raw.githubusercontent.com/yumeyao/windhawk-ime-tray-fix/refs/heads/master/doc/before-fix.gif)

```text
  Screen Left  <-----------------------------------------------------  Screen Right Edge
  [Chevron ^]  |  [SysPager (Tray Icons)]  |  [IME Indicator]  |  [Clock]  |  [Show Desktop]
```

The notification area (`TrayNotifyWnd`) is **anchored to the right edge** of the screen, while child windows are laid out left-to-right. Notice that the **IME Indicator sits right in the middle**, between the third-party tray icons (`SysPager`) and the clock.

When **"Let me set a different input method for each app window"** is enabled in a multilingual setup (e.g., English, Chinese, Japanese):
1. While typing in an app, the indicator often displays **2 glyphs** (e.g., `CH` + `中` or `JP` + `あ`), occupying a wider slot.
2. When you click a tray icon (such as Steam or WiFi), `WM_LBUTTONDOWN` immediately shifts focus to `explorer.exe`.
3. The shell defaults to English, shrinking the indicator down to **1 glyph** (`ENG`).
4. Because the clock on the right is pinned to the right edge, the shrunk width forces **everything to the left of the indicator (`SysPager` and `Chevron ^`) to slide rightward**.
5. When `WM_LBUTTONUP` fires, the icon you aimed at has moved, and your click lands on whichever neighboring icon slid under your cursor.

## The Fix

Each can solve the bug alone, but you can customize the taskbar by enabling multiple modes!

#### Mode 1: Move the input indicator to the leftmost (recommended)

![After Fix](https://raw.githubusercontent.com/yumeyao/windhawk-ime-tray-fix/refs/heads/master/doc/after-fix.gif)

```text
[IME Indicator]  |  [Chevron ^]  |  [SysPager (Tray Icons)]  |  [Clock]  |  [Show Desktop]
```

The indicator takes the first slot and everything that used to sit left of it moves right by the indicator's width. Thus, every band is now to the right of the indicator, sits at a fixed distance from the right edge and never moves.

##### Mimicking Docked Language Bar?

Yes. But the docked language bar is broken in win10 (unstable, and won't work in remote desktop - even if you enable this option in both sides, and the latest MSPY doesn't support it unless you turn on "Use the old version of MSPY") and is completely removed in win11.

#### Mode 2: Fixed width

Forces the indicator to always occupy a fixed width. The value is in **pixels at 100% display scaling** and is scaled by the monitor DPI. 36 = one glyph, 58 = glyph + secondary. Other values accepted too.

#### Mode 3: Hide secondary IME mode glyph (single-glyph mode)

Hides the 中/英 or あ/Ａ button inside the indicator.

## Notes and known limitations

- Source code and reverse engineering credit goes to AI bot (deepseek v4.1 flash).
- Heavily guided and reviewed by human (the author) to ensure the minimal path and minimal overhead. (The bots used extra threads, timers...)
- Tested on Windows 10 21H2. Windows 10 20H2 and above (including LTSC 2021, Builds 19042–19045) share the exact same binary codebase, so it should work flawlessly. Earlier builds (1903/1909 and prior) should theoretically work as well, but remain untested.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- moveToLeft: true
  $name: Move the input indicator to the leftmost
  $description: Puts the indicator in the first slot of the notification area and shifts everything that used to sit left of it to the right.
- fixedWidth: -1
  $name: Fixed width
  $description: Width of the indicator slot, in pixels at 100% display scaling (it is scaled by the monitor DPI). Set -1 to keep the natural width. 36 = one glyph, 58 = glyph + secondary. Other values accepted too (0 = completely hidden).
- hideImeMode: false
  $name: Hide secondary IME mode glyph
  $description: Hides the 中/英 or あ/Ａ button inside the indicator.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <atomic>

namespace {

// Move the input indicator into the first slot and push everything that used to
// sit left of it to the right.
std::atomic<bool> g_movePosition{false};

// Pin the indicator's band to this width, in pixels at 96 DPI. -1 leaves the
// natural width alone, 0 hides the indicator entirely, anything above 0 is a
// fixed slot scaled by the monitor DPI.
std::atomic<int> g_fixedWidth{-1};

// Hide the secondary IME mode glyph, the 中/英 button that sits left of the
// language glyph inside the indicator. This does not change the band width on
// its own: CTrayInputIndicator computes that from the input profile count, not
// from which children happen to be visible. It is therefore paired with the main
// glyph width below.
std::atomic<bool> g_hideImeMode{false};

// Width of the main IME glyph in physical pixels, as laid out by
// CTrayInputIndicator::_ResizeChildWindows. Used as the slot width when the mode
// glyph is hidden and no explicit width was requested. Zero until the first
// measurement, in which case the 96 DPI constant is scaled instead.
std::atomic<int> g_mainIconWidth{0};

// ---------------------------------------------------------------------------

constexpr wchar_t kTrayWndClass[] = L"Shell_TrayWnd";
constexpr wchar_t kTrayNotifyClass[] = L"TrayNotifyWnd";
constexpr wchar_t kIndicatorClass[] = L"TrayInputIndicatorWClass";
constexpr wchar_t kImeModeClass[] = L"IMEModeButton";
constexpr wchar_t kMainIconClass[] = L"InputIndicatorButton";
constexpr wchar_t kSysPagerClass[] = L"SysPager";

// CTrayChevron::RuntimeClassInitialize creates the chevron with (HMENU)0x5DE.
constexpr int kChevronCtrlId = 0x05DE;

// CTrayNotify::_SizeWindows asks every band for its minimum size with this
// private WM_USER-range message. TrayInputIndicatorWClass answers it from
// CTrayInputIndicator::v_WndProc (message case 1124).
constexpr UINT kMsgGetMinSize = 0x464;

// CTrayNotify::v_WndProc (case 1045) runs _ResizeOnIconChange, which notifies
// the host through _OnSizeChanged before it lays the children out. That is the
// only message-driven path that makes the taskbar re-measure and reposition the
// notification area, so it is what keeps the container in sync when the band
// width changes.
constexpr UINT kMsgRelayout = 0x415;

constexpr int kMaxShift = 8;

struct Tray {
    std::atomic<HWND> hNotify{nullptr};
    std::atomic<HWND> hIndicator{nullptr};
    std::atomic<HWND> hImeMode{nullptr};
    std::atomic<HWND> hMainIcon{nullptr};
    std::atomic<HWND> shift[kMaxShift];
    std::atomic<int> shiftCount{0};
    std::atomic<int> bandWidth{0};
};

Tray g_tray;
std::atomic<bool> g_unloading{false};

void LoadSettings() {
    g_movePosition.store(Wh_GetIntSetting(L"moveToLeft") != 0,
                         std::memory_order_relaxed);
    g_hideImeMode.store(Wh_GetIntSetting(L"hideImeMode") != 0,
                        std::memory_order_relaxed);
    g_fixedWidth.store(Wh_GetIntSetting(L"fixedWidth"),
                       std::memory_order_relaxed);
}

// True only for a window this process owns. Cached handles are re-checked with
// this before every use: a handle whose window was destroyed can be recycled by
// another process, and acting on it would touch a foreign window.
inline bool IsOwnedByCurrentProcess(HWND hWnd) {
    DWORD pid = 0;
    return hWnd && GetWindowThreadProcessId(hWnd, &pid) &&
           pid == GetCurrentProcessId();
}

// Applies g_hideImeMode to the secondary IME mode glyph, in both directions.
// ShowWindow is a no-op when the window is already in the requested state, so
// this is safe to call on every layout as well as from the mod entry points.
void EnforceImeModeHidden() {
    HWND hImeMode = g_tray.hImeMode.load(std::memory_order_relaxed);
    if (IsOwnedByCurrentProcess(hImeMode)) {
        ShowWindow(hImeMode, g_hideImeMode.load(std::memory_order_relaxed)
                                 ? SW_HIDE
                                 : SW_SHOW);
    }
}

bool Transformable(HWND hWnd) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return false;
    }
    HWND hNotify = g_tray.hNotify.load(std::memory_order_relaxed);
    if (!IsOwnedByCurrentProcess(hNotify) || !IsWindow(hWnd)) {
        return false;
    }
    RECT rc;
    return GetClientRect(hNotify, &rc) &&
           (rc.right - rc.left) >= (rc.bottom - rc.top);
}

inline UINT DpiOf(HWND hWnd) {
    UINT dpi = hWnd ? GetDpiForWindow(hWnd) : 0;
    return dpi ? dpi : 96;
}

int OverrideBandWidth(HWND hWnd) {
    int width = g_fixedWidth.load(std::memory_order_relaxed);
    if (width < 0) {
        if (!g_hideImeMode.load(std::memory_order_relaxed)) {
            return -1;
        }
        int glyph = g_mainIconWidth.load(std::memory_order_relaxed);
        if (glyph) {
            return glyph;
        }
        // Explorer's own value is 2 * SM_CYBORDER + 34, which is 36 at 96 DPI
        // and a few pixels more once the system DPI reaches 150%, because
        // GetSystemMetrics already scales that border. The measured width
        // replaces this after the first pass, so close is enough here.
        width = 36;
    }
    return MulDiv(width, static_cast<int>(DpiOf(hWnd)), 96);
}

// Takes the first slot.
LRESULT CALLBACK IndicatorProc(HWND hWnd, UINT msg, WPARAM wParam,
                               LPARAM lParam, DWORD_PTR /*dwRefData*/) {
    // CTrayNotify::_SizeWindows asks every band for its minimum size right
    // before it chains the bands, so recording the reply is enough: by the time
    // the children get their WM_WINDOWPOSCHANGING, the width is already known.
    // No query of our own is needed.
    if (msg == kMsgGetMinSize) {
        LRESULT result = DefSubclassProc(hWnd, msg, wParam, lParam);
        int width = LOWORD(result);
        // A zero reply means the indicator is hidden; forcing a width there
        // would leave an empty slot behind, so it is passed through untouched.
        if (width > 0) {
            int override = OverrideBandWidth(hWnd);
            if (override >= 0) {
                width = override;
            }
        }
        g_tray.bandWidth.store(width, std::memory_order_relaxed);
        return MAKELONG(width, HIWORD(result));
    }

    if (msg == WM_WINDOWPOSCHANGING) {
        EnforceImeModeHidden();
        auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if (g_movePosition.load(std::memory_order_relaxed) &&
            !(wp->flags & SWP_NOMOVE) && Transformable(hWnd)) {
            wp->x = 0;
        }
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

// Everything that used to sit left of the indicator moves right by its width,
// which keeps the total width of the notification area unchanged.
//
// This adds to the incoming position instead of placing the window absolutely,
// which is only correct because every caller passes the *untransformed* x:
// CTrayNotify::_SizeWindows is the sole producer of band moves, and it chains
// each rectangle from the band widths starting at x = 0 without ever reading a
// child's current geometry. A caller that echoed the current position back would
// be shifted a second time. The other two procs do not have this caveat because
// they assign absolute positions.
LRESULT CALLBACK ShiftRightProc(HWND hWnd, UINT msg, WPARAM wParam,
                                LPARAM lParam, DWORD_PTR /*dwRefData*/) {
    if (msg == WM_WINDOWPOSCHANGING) {
        auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if (g_movePosition.load(std::memory_order_relaxed) &&
            !(wp->flags & SWP_NOMOVE) && Transformable(hWnd)) {
            wp->x += g_tray.bandWidth.load(std::memory_order_relaxed);
        }
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

// CTrayInputIndicator::_ResizeChildWindows places the main glyph at the right
// edge of the mode glyph's computed rect, so with the mode glyph hidden and the
// slot narrowed the main glyph ends up past the right edge and gets clipped.
// Centre it in the slot instead.
LRESULT CALLBACK MainIconProc(HWND hWnd, UINT msg, WPARAM wParam,
                              LPARAM lParam, DWORD_PTR /*dwRefData*/) {
    if (msg == WM_WINDOWPOSCHANGING) {
        auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        // _ResizeChildWindows sizes this window to the glyph width, which makes
        // wp->cx the authoritative physical width of a single glyph at the
        // current DPI. Recording it keeps the slot correct at any scaling
        // factor without hard-coding one.
        if (!(wp->flags & SWP_NOSIZE) && wp->cx > 0) {
            g_mainIconWidth.store(wp->cx, std::memory_order_relaxed);
        }
        if (g_hideImeMode.load(std::memory_order_relaxed) &&
            !(wp->flags & SWP_NOMOVE) && Transformable(hWnd)) {
            int band = g_tray.bandWidth.load(std::memory_order_relaxed);
            if (band > 0 && wp->cx > 0) {
                wp->x = band > wp->cx ? (band - wp->cx) / 2 : 0;
            }
        }
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

// Mirrors FindCurrentProcessTaskbarWindows from taskbar-volume-control, minus
// the secondary taskbars: only the primary one has a notification area, and
// the secondaries are owned by the same process anyway.
//
// explorer.exe also runs as a plain file-browser process when "Launch folder
// windows in a separate process" is enabled. That process owns no taskbar, and
// the taskbar of the desktop process cannot be subclassed from here, so the
// search is restricted to windows owned by the current process.
BOOL CALLBACK FindTaskbarProc(HWND hWnd, LPARAM lParam) {
    if (!IsOwnedByCurrentProcess(hWnd)) {
        return TRUE;
    }

    wchar_t className[32];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }
    if (wcscmp(className, kTrayWndClass)) {
        return TRUE;
    }

    *reinterpret_cast<HWND*>(lParam) = hWnd;
    return FALSE;
}

inline HWND FindCurrentProcessTaskbarWindow() {
    HWND hTray = nullptr;
    EnumWindows(FindTaskbarProc, reinterpret_cast<LPARAM>(&hTray));
    return hTray;
}

// The notification area is a direct child of the taskbar, looked up the same way
// windhawk-mods does it. The parent has already been proven to belong to this
// process, which bounds the result to this process as well.
inline HWND FindTrayNotify(HWND hTray) {
    return hTray ? FindWindowExW(hTray, nullptr, kTrayNotifyClass, nullptr)
                 : nullptr;
}

// explorer registers Shell_TrayWnd in the process that hosts the taskbar. A
// file-browser explorer.exe never does, so this tells the two apart without
// waiting for the window itself to exist.
inline bool IsShellProcess() {
    WNDCLASS wc;
    return GetClassInfoW(GetModuleHandleW(nullptr), kTrayWndClass, &wc) != 0;
}

// Picks the windows to subclass. Subclassing has to target specific windows, so
// this classification cannot be deferred to the layout pass; everything else
// about the transform is worked out per layout.
void SyncNotify(HWND hNotify) {
    if (!IsOwnedByCurrentProcess(hNotify)) {
        return;
    }
    g_tray.hNotify.store(hNotify, std::memory_order_relaxed);

    // Direct child of the taskbar, which was just verified to be ours.
    HWND hIndicator = FindWindowExW(hNotify, nullptr, kIndicatorClass, nullptr);
    RECT indicatorRect;
    if (!hIndicator || !GetWindowRect(hIndicator, &indicatorRect)) {
        return;
    }

    // Everything that sits left of the indicator. The chevron and SysPager are
    // also matched by identity, so the classification still holds if this ever
    // runs while the transform is already in place.
    HWND shift[kMaxShift];
    int shiftCount = 0;

    auto addShift = [&](HWND hwnd) {
        if (!hwnd || hwnd == hIndicator || shiftCount >= kMaxShift) {
            return;
        }
        for (int i = 0; i < shiftCount; ++i) {
            if (shift[i] == hwnd) {
                return;
            }
        }
        shift[shiftCount++] = hwnd;
    };

    for (HWND child = GetWindow(hNotify, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        RECT rc;
        if (!GetWindowRect(child, &rc)) {
            continue;
        }
        if (rc.left < indicatorRect.left ||
            GetDlgCtrlID(child) == kChevronCtrlId) {
            addShift(child);
        }
    }
    addShift(FindWindowExW(hNotify, nullptr, kSysPagerClass, nullptr));

    g_tray.hIndicator.store(hIndicator, std::memory_order_relaxed);
    g_tray.hImeMode.store(
        FindWindowExW(hIndicator, nullptr, kImeModeClass, nullptr),
        std::memory_order_relaxed);
    HWND hMainIcon =
        FindWindowExW(hIndicator, nullptr, kMainIconClass, nullptr);
    g_tray.hMainIcon.store(hMainIcon, std::memory_order_relaxed);
    g_tray.shiftCount.store(shiftCount, std::memory_order_relaxed);
    for (int i = 0; i < kMaxShift; ++i) {
        g_tray.shift[i].store(i < shiftCount ? shift[i] : nullptr,
                              std::memory_order_relaxed);
    }

    // The glyph width is already there whenever the indicator was laid out at
    // least once, so take it now instead of waiting for the next layout pass.
    if (hMainIcon) {
        RECT mainRect;
        if (GetWindowRect(hMainIcon, &mainRect) &&
            mainRect.right > mainRect.left) {
            g_mainIconWidth.store(mainRect.right - mainRect.left,
                                  std::memory_order_relaxed);
        }
    }

    // Seed the band width from the rect that was already read above, so this
    // costs nothing. It covers the one interleaving that the install cannot
    // rule out: this runs on the Windhawk thread, and the cross-thread
    // SendMessage that installs the subclass is dispatched at the UI thread's
    // message-check points. If one of those falls between a layout pass's
    // measuring phase and its DeferWindowPos batch, the batch would otherwise
    // see a width of zero and move nothing.
    //
    // A hidden indicator answers the size query with 0 (see
    // CTrayInputIndicator::v_WndProc, case 1124), so its stale rectangle must
    // not leak into the seed: that would shift the bands for one pass by a width
    // the layout did not allocate.
    int seed = IsWindowVisible(hIndicator)
                   ? indicatorRect.right - indicatorRect.left
                   : 0;
    int override = OverrideBandWidth(hNotify);
    if (override >= 0 && seed > 0) {
        seed = override;
    }
    g_tray.bandWidth.store(seed, std::memory_order_relaxed);

    WindhawkUtils::SetWindowSubclassFromAnyThread(hIndicator, IndicatorProc, 0);
    if (hMainIcon) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hMainIcon, MainIconProc,
                                                      0);
    }
    for (int i = 0; i < shiftCount; ++i) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(shift[i], ShiftRightProc,
                                                      0);
    }
}

// Re-runs the whole notification area layout. kMsgRelayout reaches
// CTrayNotify::_ResizeOnIconChange, which notifies the host through
// _OnSizeChanged first and only then lays the children out, so the container is
// already resized by the time the children are placed. A plain WM_SIZE would
// only re-lay out the children inside the current rectangle, which is wrong
// whenever the band width changed.
void Refresh(HWND hNotify) {
    // PostMessage crosses process boundaries, so the target is verified first.
    if (IsOwnedByCurrentProcess(hNotify)) {
        PostMessageW(hNotify, kMsgRelayout, 0, 0);
    }
}

// The one place that wires the mod up. Both entry points call it, and each can
// reach it at most once, so it carries no guard of its own: SyncNotify
// re-subclasses idempotently (SetWindowSubclass replaces a subclass that has the
// same procedure), EnforceImeModeHidden is a no-op when the window is already in
// the requested state, and Refresh only posts a message.
void HandleIdentifiedTrayNotify(HWND hNotify) {
    SyncNotify(hNotify);
    EnforceImeModeHidden();
    Refresh(hNotify);
}

// Catch-up for the case where explorer has not built the notification area yet
// when Wh_ModAfterInit runs. CTrayNotify::TrayNotifyCreate creates it through
// CreateWindowExW with the class name passed as a string literal, so the textual
// comparison below is sufficient and the atom case never reaches it.
void HandleCreatedWindow(HWND hWnd, LPCWSTR className) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    // Same test windhawk-mods uses: lpClassName is either a string pointer or an
    // integer atom, and an atom must never be dereferenced.
    BOOL textualClassName =
        (reinterpret_cast<ULONG_PTR>(className) &
         ~static_cast<ULONG_PTR>(0xffff)) != 0;
    if (!textualClassName || _wcsicmp(className, kTrayNotifyClass)) {
        return;
    }

    Wh_Log(L"TrayNotifyWnd created, wiring up");
    HandleIdentifiedTrayNotify(hWnd);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className,
                                 LPCWSTR windowName, DWORD style, int x, int y,
                                 int cx, int cy, HWND parent, HMENU menu,
                                 HINSTANCE instance, LPVOID param) {
    HWND hWnd = CreateWindowExW_Original(exStyle, className, windowName, style,
                                         x, y, cx, cy, parent, menu, instance,
                                         param);
    if (hWnd) {
        HandleCreatedWindow(hWnd, className);
    }
    return hWnd;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    // Only a catch-up, so failing to install it must not stop the mod.
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW, relying on Wh_ModAfterInit");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // explorer registers Shell_TrayWnd in the process that hosts the taskbar; a
    // file-browser explorer.exe never does. The check also fails when the shell
    // process has not got that far yet, in which case the creation hook picks the
    // notification area up later.
    if (!IsShellProcess()) {
        return;
    }

    HWND hNotify = FindTrayNotify(FindCurrentProcessTaskbarWindow());
    if (!hNotify) {
        Wh_Log(L"Notification area not created yet, waiting for the hook");
        return;
    }

    HandleIdentifiedTrayNotify(hNotify);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnforceImeModeHidden();
    Refresh(g_tray.hNotify.load(std::memory_order_relaxed));
}

void Wh_ModUninit() {
    g_hideImeMode.store(false, std::memory_order_relaxed);
    EnforceImeModeHidden();

    g_unloading.store(true, std::memory_order_relaxed);

    // Remove synchronously: the DLL is about to be unloaded and a lingering
    // subclass proc would be a dangling pointer.
    HWND hIndicator = g_tray.hIndicator.load(std::memory_order_relaxed);
    if (hIndicator) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hIndicator,
                                                        IndicatorProc);
    }
    int shiftCount = g_tray.shiftCount.load(std::memory_order_relaxed);
    for (int i = 0; i < shiftCount; ++i) {
        HWND hwnd = g_tray.shift[i].load(std::memory_order_relaxed);
        if (hwnd) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd,
                                                            ShiftRightProc);
        }
    }
    HWND hMainIcon = g_tray.hMainIcon.load(std::memory_order_relaxed);
    if (hMainIcon) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hMainIcon,
                                                        MainIconProc);
    }

    // Restore the stock order right away.
    Refresh(g_tray.hNotify.load(std::memory_order_relaxed));
}
