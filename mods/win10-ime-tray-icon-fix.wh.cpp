// ==WindhawkMod==
// @id              win10-ime-tray-icon-fix
// @name            Windows 10 IME Tray Icon Fix
// @description     Moves the input indicator to the left of the notification area so its width change cannot shift the tray icons. Alternative modes provided.
// @version         1.1
// @author          yumeyao
// @github          https://github.com/yumeyao
// @include         explorer.exe
// @compilerOptions -lcomctl32 -luser32
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

Forces the indicator to always occupy a fixed width. 36 = one glyph, 58 = glyph + secondary. Other values accepted too.

#### Mode 3: Hide secondary IME mode glyph (single-glyph mode)

Hides the 中/英 or あ/Ａ button inside the indicator, so the width is then fixed at 36.

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
  $description: Width in pixels of the indicator slot. Set -1 to keep the natural width. 36 = one glyph, 58 = glyph + secondary. Other values accepted too (0 = completely hidden).
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
bool g_movePosition;

// Pin the indicator's band to this width in pixels. -1 leaves the natural width
// alone, 0 hides the indicator entirely, anything above 0 is a fixed slot.
int g_fixedWidth;

// Hide the secondary IME mode glyph, the 中/英 button that sits left of the
// language glyph inside the indicator. This does not change the band width:
// CTrayInputIndicator computes that from the input profile count, not from which
// children happen to be visible. Pair it with g_fixedWidth to also shrink the
// slot.
bool g_hideImeMode;

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
    g_movePosition = Wh_GetIntSetting(L"moveToLeft") != 0;
    g_hideImeMode = Wh_GetIntSetting(L"hideImeMode") != 0;

    int width = Wh_GetIntSetting(L"fixedWidth");
    if (width < 0 && g_hideImeMode) {
        // With the mode glyph gone the slot only has to hold the language
        // glyph. Pinning it to 36 keeps the layout tight instead of leaving a
        // gap where the hidden glyph used to be.
        width = 36;
    }
    g_fixedWidth = width;
}

// Applies g_hideImeMode to the secondary IME mode glyph, in both directions.
// ShowWindow is a no-op when the window is already in the requested state, so
// this is safe to call on every layout as well as from the mod entry points.
void EnforceImeModeHidden() {
    HWND hImeMode = g_tray.hImeMode.load(std::memory_order_relaxed);
    if (hImeMode && IsWindow(hImeMode)) {
        ShowWindow(hImeMode, g_hideImeMode ? SW_HIDE : SW_SHOW);
    }
}

bool Transformable(HWND hWnd) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return false;
    }
    HWND hNotify = g_tray.hNotify.load(std::memory_order_relaxed);
    if (!hNotify || !IsWindow(hNotify) || !IsWindow(hWnd)) {
        return false;
    }
    RECT rc;
    return GetClientRect(hNotify, &rc) &&
           (rc.right - rc.left) >= (rc.bottom - rc.top);
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
        // -1 leaves the natural width alone; 0 collapses the band so the
        // indicator takes no space at all.
        if (g_fixedWidth >= 0) {
            width = g_fixedWidth;
        }
        g_tray.bandWidth.store(width, std::memory_order_relaxed);
        return MAKELONG(width, HIWORD(result));
    }

    if (msg == WM_WINDOWPOSCHANGING) {
        EnforceImeModeHidden();
        auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if (g_movePosition && !(wp->flags & SWP_NOMOVE) && Transformable(hWnd)) {
            wp->x = 0;
        }
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

// Everything that used to sit left of the indicator moves right by its width,
// which keeps the total width of the notification area unchanged.
LRESULT CALLBACK ShiftRightProc(HWND hWnd, UINT msg, WPARAM wParam,
                                LPARAM lParam, DWORD_PTR /*dwRefData*/) {
    if (msg == WM_WINDOWPOSCHANGING) {
        auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if (g_movePosition && !(wp->flags & SWP_NOMOVE) &&
            Transformable(hWnd)) {
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
        if (g_hideImeMode && !(wp->flags & SWP_NOMOVE) &&
            Transformable(hWnd)) {
            int band = g_tray.bandWidth.load(std::memory_order_relaxed);
            if (band > 0 && wp->cx > 0) {
                wp->x = band > wp->cx ? (band - wp->cx) / 2 : 0;
            }
        }
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

HWND FindTrayNotify() {
    HWND hTray = FindWindowW(kTrayWndClass, nullptr);
    return hTray ? FindWindowExW(hTray, nullptr, kTrayNotifyClass, nullptr)
                 : nullptr;
}

// Picks the windows to subclass. Subclassing has to target specific windows, so
// this classification cannot be deferred to the layout pass; everything else
// about the transform is worked out per layout.
void SyncNotify(HWND hNotify) {
    g_tray.hNotify.store(hNotify, std::memory_order_relaxed);

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

    // Seed the band width from the rect that was already read above, so this
    // costs nothing. It covers the one interleaving that the install cannot
    // rule out: this runs on the Windhawk thread, and the cross-thread
    // SendMessage that installs the subclass is dispatched at the UI thread's
    // message-check points. If one of those falls between a layout pass's
    // measuring phase and its DeferWindowPos batch, the batch would otherwise
    // see a width of zero and move nothing.
    int seed = indicatorRect.right - indicatorRect.left;
    if (g_fixedWidth >= 0) {
        seed = g_fixedWidth;
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
    if (hNotify) {
        PostMessageW(hNotify, kMsgRelayout, 0, 0);
    }
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    return TRUE;
}

void Wh_ModAfterInit() {
    HWND hNotify = FindTrayNotify();
    if (!hNotify) {
        Wh_Log(L"TrayNotifyWnd not found");
        return;
    }
    SyncNotify(hNotify);
    EnforceImeModeHidden();
    Refresh(hNotify);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnforceImeModeHidden();
    Refresh(g_tray.hNotify.load(std::memory_order_relaxed));
}

void Wh_ModUninit() {
    g_hideImeMode = false;
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
