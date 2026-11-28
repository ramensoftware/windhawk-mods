// ==WindhawkMod==
// @id              dense-tray-icons-padding-old-taskbar
// @name            Adjustable tray icons padding for Win10 taskbar
// @description     Makes padding of tray and tray popup icons adjustable in legacy taskbar
// @version         2.1
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lpsapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Adjustable tray icons padding for Win10 taskbar

This is a port of functionality of [7+ Taskbar Tweaker](https://github.com/m417z/7-Taskbar-Tweaker) by [m417z](https://github.com/m417z) that allows to adjust the padding of the tray icons
in classic (Windows 10) taskbar (running either under Windows 10 or Windows 11). To adjust the tray icons padding on the new Win11 taskbar,
use the mod [Taskbar tray icon spacing and grid](https://windhawk.net/mods/taskbar-notification-icon-spacing). The version 2.0 of the mod was [comprehensively reworked](https://github.com/ramensoftware/windhawk-mods/pull/6342)
by [piknockyou](https://github.com/piknockyou).

![screenshot](https://i.imgur.com/NjGhXuc.png)

* **Tray icon padding** - the spacing between the tray icons on the taskbar.
* **Hidden icons popup icon size** - the size of the icons in the popup that
  opens when you click the chevron, which also shrinks that popup.
* **Hidden icons popup cell padding** - the empty border around each icon in
  the popup. `0` packs the icons next to each other at their normal size.

The default padding value is set to 2, the same as under Windows 95-Windows XP, more dense than under Windows 7, but can be changed in mod's options.

## Screenshots

![Hidden icons popup at the Windows defaults](https://raw.githubusercontent.com/piknockyou/images/main/tray-icon-spacing-win10/readme-1-popup-defaults.png) \
*Windows defaults: 16 pixel icons in 40 pixel cells.*

![Hidden icons popup with all paddings zero](https://raw.githubusercontent.com/piknockyou/images/main/tray-icon-spacing-win10/readme-2-popup-padding-zero.png) \
*All paddings zero: the icons sit next to each other.*

![Hidden icons popup with 50 pixel icons and padding zero](https://raw.githubusercontent.com/piknockyou/images/main/tray-icon-spacing-win10/readme-3-popup-padding-zero-icon50.png) \
*Paddings zero with 50 pixel popup icons.*

## How it works

**Tray icons.** The shell reserves one fixed-width slot per tray icon and
computes its width from `GetSystemMetrics(SM_CXSMICON)` (16 pixels at 100%
scaling) plus a fixed padding, then pushes the result into the tray toolbar. The
mod never resizes the tray window itself. While the taskbar lays the tray out,
the mod

1. redirects `SM_CXSMICON` for the query the shell makes on the taskbar
   thread,
2. rewrites the horizontal padding the shell sends to the tray toolbar, and
3. after a settings change, asks Explorer to reload the notification area so
   that this work happens in the shell's own layout pass.

This update adds:

* less cases for Explorer's restart,
* the `-1` off value on every setting, which restores the previous state
  without disabling the mod,
* the hidden icons popup settings (icon size and cell padding).

**Hidden icons popup.** The popup sizes its cells from two things: the icon
images in its toolbar, and the theme's content margins around each cell
(measured on Windows 10 22H2: the shell's 16 pixel icons plus the theme's 12
pixel margins on each side give 40 pixel cells and a 120x120 popup for 9
icons). The toolbar refuses `TB_SETBUTTONSIZE` outright, so the mod replaces
the icon images with scaled copies for the icon-size setting, and answers the
shell's own theme queries (`TrayNotifyFlyout`, part 3) with the configured
cell padding instead of the theme's 12 pixels. The popup re-measures itself
every time it opens. The tray and the popup draw from one image list, and the
shell edits that list in place when a tray icon appears, disappears or is
replaced, without telling the popup. The mod therefore applies every such
edit to its scaled copy as well, scaling only the picture that changed; a copy
kept from before the edit would draw another application's picture under the
button's tooltip.

## Notes

- `-1` switches a setting off and restores what was there before, without
  disabling the mod. For the tray, disabling redirection and asking Explorer
  to reload lets the shell recompute its own spacing.
- Value ranges: tray icon padding `-1` or `0`-`32` (the Windows default is about
  `16`, `0` and `1` are the tightest); hidden icons popup icon size `-1` or
  `8`-`96` (the Windows default is `16` at 100% scaling); hidden icons popup
  cell padding `-1` or `0`-`24` (`-1` = the theme's 12 pixels per side, `0` =
  the icons sit next to each other). Values outside a range are clamped and the
  clamped value is logged.
- For the tray spacing, small values are denser than the Windows default and
  larger values wider; the icons stay evenly spaced. The exact mapping is not
  documented, so try a value and adjust from there. All settings are applied
  without disabling the mod.
- Disabling the mod removes the redirection, restores the pending flag state,
  and asks Explorer to reload, so the tray returns to its own layout.
- If a setting seems not to take effect right after enabling the mod, restart
  Explorer once; changes afterwards are applied live.
- The mod writes to Explorer's memory only to flip the two "pointer device"
  flag bytes that decide how the shell lays the tray out. They are restored
  immediately, a writable-memory check and a sanity check guard the write, and
  the mod does nothing at all while the setting is `-1`.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- padding: 2
  $name: Tray icon padding
  $description: >-
    The spacing between the tray icons on the taskbar.

    Allowed values: -1 or 0 to 32.

    -1 = Windows default (the mod restores the original tray values)

    0 or 1 = the tightest spacing. Windows 10 itself is around 16, so values
    between 1 and 16 are denser than the default.

    Larger values widen the spacing; the icons stay evenly spaced.
- overflowIconSize: -1
  $name: Hidden icons popup icon size
  $description: >-
    The size of the icons in the popup that shows the hidden tray icons.

    Allowed values: -1 or 8 to 96.

    -1 = Windows default (the mod restores the popup's own icons)

    The Windows default is 16 pixels at 100% scaling, the same small icons the
    taskbar uses; with the stock 12 pixel margins per side, the cells are 40
    pixels and the popup for 9 icons is 120x120. Smaller values shrink the
    icons together with the popup; larger values grow them, up to the popup's
    maximum size. A new size is applied the next time the popup opens.
- overflowCellPadding: -1
  $name: Hidden icons popup cell padding
  $description: >-
    The empty border around each icon in the popup that shows the hidden tray
    icons, in pixels on each side.

    Allowed values: -1 or 0 to 24.

    -1 = Windows default (the theme's 12 pixels per side; the mod leaves the
    theme values alone)

    0 = no padding: the icons sit next to each other at their normal size.
    Larger values add air around each icon and grow the popup. A new padding is
    applied the next time the popup opens (reopen it to see the change).
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <commctrl.h>
#include <psapi.h>
#include <uxtheme.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <vector>

namespace {

struct {
    int trayIconPadding;
    int overflowIconSize;
    int overflowCellPadding;
} g_settings;

std::atomic<bool> g_unloading{false};

// The range of explorer.exe in memory, used to tell the shell's own
// GetSystemMetrics calls from everybody else's.
ULONG_PTR g_explorerBase = 0;
ULONG_PTR g_explorerEnd = 0;

// ---------------------------------------------------------------------------
// Tray icons on the taskbar
// ---------------------------------------------------------------------------

// The tray mechanism follows Anixx's MIT-licensed Windhawk port of 7+ Taskbar
// Tweaker's tray padding behavior: force the shell's two pointer-device flag
// bytes while it lays the tray out, redirect its one SM_CXSMICON query, and
// rewrite the toolbar padding it sends. No toolbar is resized directly; the
// shell keeps ownership of its own window geometry.
constexpr int kTrayIconPaddingOff = -1;
constexpr int kMaxIconPadding = 32;

// Offset of the two "pointer device" flag bytes inside the object behind
// GetWindowLongPtr(TrayNotifyWnd, 0), as used by 7+ Taskbar Tweaker and by the
// mod this one is based on.
#ifdef _WIN64
constexpr int kPtrDevOffset = 0x168;
#else
constexpr int kPtrDevOffset = 0xD0;
#endif

struct TrayEntry {
    HWND hToolbar = nullptr;
    LONG_PTR lpNotify = 0;
    BYTE prevSupported = 0;
    BYTE prevValid = 0;
    bool pending = false;
    DWORD threadId = 0;
};

std::vector<TrayEntry> g_trayEntries;
CRITICAL_SECTION g_lock;
UINT g_restoreMsg = 0;

inline bool TrayPaddingActive() {
    return !g_unloading && g_settings.trayIconPadding != kTrayIconPaddingOff;
}

inline bool IsWindowInCurrentProcess(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid == GetCurrentProcessId();
}

inline bool IsExplorerAddress(const void* address) {
    auto a = reinterpret_cast<ULONG_PTR>(address);
    return a >= g_explorerBase && a < g_explorerEnd;
}

inline bool IsWritableMemory(const void* ptr, SIZE_T size) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0) {
        return false;
    }

    if (mbi.State != MEM_COMMIT) {
        return false;
    }

    if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) {
        return false;
    }

    // The two flag bytes are written, not only read: a read-only page would
    // fault instead of silently leaving the flags untouched.
    constexpr DWORD kWritable = PAGE_READWRITE | PAGE_WRITECOPY |
                                PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    if (!(mbi.Protect & kWritable)) {
        return false;
    }

    SIZE_T offset =
        reinterpret_cast<const BYTE*>(ptr) -
        reinterpret_cast<const BYTE*>(mbi.BaseAddress);
    return offset + size <= mbi.RegionSize;
}

inline bool GetFlagPointers(LONG_PTR lpNotify,
                            BYTE** ppSupported,
                            BYTE** ppValid) {
    if (lpNotify < 0x10000) {
        return false;
    }

    BYTE* pSupported = reinterpret_cast<BYTE*>(lpNotify + kPtrDevOffset);
    BYTE* pValid = pSupported + 1;
    if (!IsWritableMemory(pSupported, 2)) {
        return false;
    }

    *ppSupported = pSupported;
    *ppValid = pValid;
    return true;
}

TrayEntry* FindTrayEntry(HWND hToolbar) {
    for (auto& entry : g_trayEntries) {
        if (entry.hToolbar == hToolbar) {
            return &entry;
        }
    }

    return nullptr;
}

void RestoreTrayEntry(TrayEntry& entry) {
    if (!entry.pending || !entry.lpNotify) {
        return;
    }

    BYTE* pSupported = nullptr;
    BYTE* pValid = nullptr;
    if (!GetFlagPointers(entry.lpNotify, &pSupported, &pValid)) {
        entry.pending = false;
        entry.lpNotify = 0;
        entry.threadId = 0;
        return;
    }

    *pSupported = entry.prevSupported;
    *pValid = entry.prevValid;
    entry.pending = false;
    entry.lpNotify = 0;
    entry.threadId = 0;
}

HWND FindTrayNotifyForToolbar(HWND hToolbar) {
    for (HWND parent = GetParent(hToolbar); parent; parent = GetParent(parent)) {
        WCHAR className[64];
        if (!GetClassNameW(parent, className, ARRAYSIZE(className))) {
            return nullptr;
        }

        if (_wcsicmp(className, L"TrayNotifyWnd") == 0) {
            DWORD pid = 0;
            GetWindowThreadProcessId(parent, &pid);
            return pid == GetCurrentProcessId() ? parent : nullptr;
        }
    }

    return nullptr;
}

// Force the pointer-device flags while the shell is laying out the tray. The
// metric hook restores them when Explorer makes its SM_CXSMICON query.
void ArmTrayLayoutTakeover(HWND hToolbar) {
    if (!TrayPaddingActive()) {
        return;
    }

    HWND hTrayNotify = FindTrayNotifyForToolbar(hToolbar);
    if (!hTrayNotify) {
        return;
    }

    LONG_PTR lpNotify = GetWindowLongPtrW(hTrayNotify, 0);
    if (!lpNotify) {
        return;
    }

    BYTE* pSupported = nullptr;
    BYTE* pValid = nullptr;
    if (!GetFlagPointers(lpNotify, &pSupported, &pValid)) {
        Wh_Log(L"Invalid TrayNotify pointer %p", reinterpret_cast<void*>(lpNotify));
        return;
    }

    EnterCriticalSection(&g_lock);

    TrayEntry* entry = FindTrayEntry(hToolbar);
    if (!entry) {
        g_trayEntries.push_back(TrayEntry{hToolbar});
        entry = &g_trayEntries.back();
    }

    bool alreadyArmed = entry->pending && entry->lpNotify == lpNotify;
    if (!alreadyArmed) {
        BYTE curSupported = *pSupported;
        BYTE curValid = *pValid;
        if (curSupported > 1 || curValid > 1) {
            Wh_Log(L"Pointer device flags look wrong (%d, %d at offset 0x%X) - skipping",
                   curSupported, curValid, kPtrDevOffset);
            LeaveCriticalSection(&g_lock);
            return;
        }

        entry->prevSupported = curSupported;
        entry->prevValid = curValid;
        entry->lpNotify = lpNotify;
        entry->pending = true;
        entry->threadId = GetCurrentThreadId();
    }

    *pSupported = 1;
    *pValid = 1;

    LeaveCriticalSection(&g_lock);

    // Restore the flags if the shell turns out not to ask for the icon metric
    // during this layout pass, so that they are never left modified.
    PostMessageW(hToolbar, g_restoreMsg, 0, 0);
}

LRESULT CALLBACK TrayToolbarSubclassProc(HWND hWnd,
                                         UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam,
                                         DWORD_PTR dwRefData) {
    if (uMsg == g_restoreMsg) {
        EnterCriticalSection(&g_lock);
        if (TrayEntry* entry = FindTrayEntry(hWnd)) {
            RestoreTrayEntry(*entry);
        }
        LeaveCriticalSection(&g_lock);
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        EnterCriticalSection(&g_lock);
        if (TrayEntry* entry = FindTrayEntry(hWnd)) {
            RestoreTrayEntry(*entry);
        }
        g_trayEntries.erase(
            std::remove_if(g_trayEntries.begin(), g_trayEntries.end(),
                           [hWnd](const TrayEntry& e) {
                               return e.hToolbar == hWnd;
                           }),
            g_trayEntries.end());
        LeaveCriticalSection(&g_lock);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == TB_GETBUTTONSIZE) {
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        ArmTrayLayoutTakeover(hWnd);
        return result;
    }

    if (uMsg == TB_SETPADDING) {
        // Rewrite only the horizontal padding; Explorer supplies the vertical
        // value. The toolbar expects an even horizontal padding.
        if (TrayPaddingActive() && FindTrayNotifyForToolbar(hWnd)) {
            lParam &= ~0xFFFF;
            lParam |=
                ((g_settings.trayIconPadding / 2 * 2) & 0xFFFF);
        }

        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// The shell computes the tray slot width from SM_CXSMICON. The value is
// redirected for the single query it makes on the taskbar thread while laying
// the tray out, which is what changes the total tray width.

// Restores the tray flags armed for this thread and, when a pending layout of
// this thread was found, returns true with the redirected metric.
bool TakeArmedTrayMetric(int result, int* value) {
    DWORD currentThreadId = GetCurrentThreadId();
    bool restored = false;

    EnterCriticalSection(&g_lock);
    for (auto& entry : g_trayEntries) {
        if (entry.pending && entry.threadId == currentThreadId) {
            RestoreTrayEntry(entry);
            restored = true;
        }
    }
    LeaveCriticalSection(&g_lock);

    if (restored) {
        *value = (result + g_settings.trayIconPadding) / 2;
    }
    return restored;
}

using GetSystemMetrics_t = int(WINAPI*)(int);
GetSystemMetrics_t GetSystemMetrics_Original = nullptr;
int WINAPI GetSystemMetrics_Hook(int nIndex) {
    int result = GetSystemMetrics_Original(nIndex);

    if (nIndex == SM_CXSMICON && TrayPaddingActive() &&
        IsExplorerAddress(__builtin_return_address(0))) {
        int value = 0;
        if (TakeArmedTrayMetric(result, &value)) {
            Wh_Log(L"SM_CXSMICON %d -> %d for the tray layout", result, value);
            return value;
        }
    }

    return result;
}

using GetSystemMetricsForDpi_t = int(WINAPI*)(int, UINT);
GetSystemMetricsForDpi_t GetSystemMetricsForDpi_Original = nullptr;
int WINAPI GetSystemMetricsForDpi_Hook(int nIndex, UINT dpi) {
    int result = GetSystemMetricsForDpi_Original(nIndex, dpi);

    if (nIndex == SM_CXSMICON && TrayPaddingActive() &&
        IsExplorerAddress(__builtin_return_address(0))) {
        int value = 0;
        if (TakeArmedTrayMetric(result, &value)) {
            Wh_Log(L"SM_CXSMICON (dpi) %d -> %d for the tray layout", result,
                   value);
            return value;
        }
    }

    return result;
}

// ---------------------------------------------------------------------------
// Hidden icons popup
// ---------------------------------------------------------------------------

HWND g_overflowToolbar = nullptr;
// Read by the image-list hooks on every thread that edits any image list, so
// it is atomic; it is only written on the popup toolbar's thread and by the
// destroy hook.
std::atomic<HIMAGELIST> g_overflowOriginalImageList{nullptr};
HIMAGELIST g_overflowScaledImageList = nullptr;
// At most one full rebuild is queued at a time.
std::atomic<bool> g_overflowRescalePending{false};
int g_overflowAppliedSize = 0;
bool g_inImageListSwap = false;
UINT g_overflowRefreshMsg = 0;

// g_overflowRefreshMsg wParam: what the toolbar's own thread should do.
constexpr WPARAM kOverflowRefreshApply = 0;    // apply unless already applied
constexpr WPARAM kOverflowRefreshReapply = 1;  // apply even if already applied
constexpr WPARAM kOverflowRefreshRestore = 2;  // put the shell's list back

constexpr int kOverflowIconSizeOff = -1;
constexpr int kOverflowIconSizeMin = 8;
constexpr int kOverflowIconSizeMax = 96;

// The tray and the popup draw from one image list, and the shell edits that
// list in place when a tray icon appears, disappears, is replaced or animates;
// it sends the popup no message about it. The scaled copy therefore follows
// every such edit: the same edit is applied to the copy, with just the changed
// picture scaled, so animated icons cost one picture per frame and never a
// full rebuild. The shell makes these edits on the popup toolbar's own thread,
// which owns the copy. If an edit arrives on another thread, or the copy cannot
// follow it, one full rebuild is posted instead.
using ImageListReplaceIcon_t = int(WINAPI*)(HIMAGELIST, int, HICON);
ImageListReplaceIcon_t ImageList_ReplaceIcon_Original = nullptr;
using ImageListRemove_t = BOOL(WINAPI*)(HIMAGELIST, int);
ImageListRemove_t ImageList_Remove_Original = nullptr;
using ImageListDestroy_t = BOOL(WINAPI*)(HIMAGELIST);
ImageListDestroy_t ImageList_Destroy_Original = nullptr;

// Returns a copy of one picture of an image list at the given size, or
// nullptr. The caller destroys the returned icon.
HICON ScaleImageListIcon(HIMAGELIST source, int index, int iconSize) {
    HICON icon = ImageList_GetIcon(source, index, ILD_TRANSPARENT);
    if (!icon) {
        return nullptr;
    }

    HICON small = static_cast<HICON>(
        CopyImage(icon, IMAGE_ICON, iconSize, iconSize, LR_DEFAULTCOLOR));
    DestroyIcon(icon);
    return small;
}

bool IsOverflowSourceEdit(HIMAGELIST himl) {
    return himl && himl == g_overflowOriginalImageList && !g_unloading &&
           g_settings.overflowIconSize != kOverflowIconSizeOff;
}

// Returns the popup toolbar when the current thread owns it, else nullptr.
HWND OverflowToolbarOnThisThread() {
    HWND hToolbar = g_overflowToolbar;
    if (!hToolbar || !IsWindow(hToolbar) ||
        GetWindowThreadProcessId(hToolbar, nullptr) != GetCurrentThreadId()) {
        return nullptr;
    }
    return hToolbar;
}

void ScheduleOverflowRescale() {
    HWND hToolbar = g_overflowToolbar;
    if (hToolbar && IsWindow(hToolbar) &&
        !g_overflowRescalePending.exchange(true)) {
        // A failed post would leave the flag set and block every later
        // fallback rebuild.
        if (!PostMessageW(hToolbar, g_overflowRefreshMsg,
                          kOverflowRefreshReapply, 0)) {
            g_overflowRescalePending = false;
        }
    }
}

// Called on the toolbar's thread after the copy followed an edit: the copy is
// valid only while it has exactly as many pictures as the shell's list.
void FinishOverflowMirror(HWND hToolbar, HIMAGELIST source, bool ok) {
    if (!ok || ImageList_GetImageCount(g_overflowScaledImageList) !=
                   ImageList_GetImageCount(source)) {
        Wh_Log(L"Scaled popup icons could not follow an edit, rebuilding");
        ScheduleOverflowRescale();
        return;
    }

    if (IsWindowVisible(hToolbar)) {
        InvalidateRect(hToolbar, nullptr, FALSE);
    }
}

int WINAPI ImageList_ReplaceIcon_Hook(HIMAGELIST himl, int i, HICON hicon) {
    int result = ImageList_ReplaceIcon_Original(himl, i, hicon);
    if (result < 0 || !IsOverflowSourceEdit(himl)) {
        return result;
    }

    HWND hToolbar = OverflowToolbarOnThisThread();
    HIMAGELIST copy = hToolbar ? g_overflowScaledImageList : nullptr;
    if (!copy) {
        ScheduleOverflowRescale();
        return result;
    }

    int cx = 0;
    int cy = 0;
    bool ok = false;
    // An append (i == -1) is only mirrored when the copy is in step, so that
    // the new picture lands at the same number in both lists.
    if (ImageList_GetIconSize(copy, &cx, &cy) &&
        (i >= 0 || ImageList_GetImageCount(copy) == result)) {
        if (HICON small = ScaleImageListIcon(himl, result, cx)) {
            ok = ImageList_ReplaceIcon_Original(copy, i < 0 ? -1 : result,
                                                small) == result;
            DestroyIcon(small);
        }
    }

    FinishOverflowMirror(hToolbar, himl, ok);
    return result;
}

BOOL WINAPI ImageList_Remove_Hook(HIMAGELIST himl, int i) {
    BOOL result = ImageList_Remove_Original(himl, i);
    if (!result || !IsOverflowSourceEdit(himl)) {
        return result;
    }

    HWND hToolbar = OverflowToolbarOnThisThread();
    HIMAGELIST copy = hToolbar ? g_overflowScaledImageList : nullptr;
    if (!copy) {
        ScheduleOverflowRescale();
        return result;
    }

    // i == -1 removes every picture, in both lists alike.
    bool ok = ImageList_Remove_Original(copy, i) != FALSE;
    FinishOverflowMirror(hToolbar, himl, ok);
    return result;
}

BOOL WINAPI ImageList_Destroy_Hook(HIMAGELIST himl) {
    BOOL result = ImageList_Destroy_Original(himl);
    if (result && himl) {
        // Forget the shell's list without racing a thread that installs a
        // new one at the same time.
        HIMAGELIST expected = himl;
        g_overflowOriginalImageList.compare_exchange_strong(expected, nullptr);
    }
    return result;
}

// The popup's cell is the icon plus the visual style's content margins (class
// "TrayNotifyFlyout", part 3, state 0, TMT_CONTENTMARGINS = 3602; measured:
// 16 px icon + 12 px margins per side = 40 px cells at 100%). _UpdateTheme
// reads them through GetThemeMargins, so answering the shell's own query with
// the configured padding removes the gaps while the icon size keeps coming
// from the image list. Only the margins are substituted; the theme's
// TMT_WIDTH/TMT_HEIGHT are left alone.
constexpr int kFlyoutPart = 3;
constexpr int kFlyoutState = 0;
constexpr int kPropContentMargins = 3602;  // TMT_CONTENTMARGINS
constexpr int kOverflowCellPaddingOff = -1;
constexpr int kOverflowCellPaddingMax = 24;

HTHEME g_flyoutTheme = nullptr;
int g_lastOverflowPaddingPushed = kOverflowCellPaddingOff;

// The real theme handle is recorded, never replaced: every other use of the
// theme (painting included) keeps working, and only queries for this exact
// part, state and property are answered with the configured padding.
using OpenThemeDataForDpi_t = HTHEME(WINAPI*)(HWND, LPCWSTR, UINT);
OpenThemeDataForDpi_t OpenThemeDataForDpi_Original = nullptr;
HTHEME WINAPI OpenThemeDataForDpi_Hook(HWND hwnd,
                                       LPCWSTR pszClassList,
                                       UINT dpi) {
    HTHEME result =
        OpenThemeDataForDpi_Original(hwnd, pszClassList, dpi);
    if (result && pszClassList &&
        _wcsicmp(pszClassList, L"TrayNotifyFlyout") == 0) {
        EnterCriticalSection(&g_lock);
        g_flyoutTheme = result;
        LeaveCriticalSection(&g_lock);
        Wh_Log(L"Flyout theme opened: %p",
               reinterpret_cast<void*>(result));
    }
    return result;
}

using OpenThemeData_t = HTHEME(WINAPI*)(HWND, LPCWSTR);
OpenThemeData_t OpenThemeData_Original = nullptr;
HTHEME WINAPI OpenThemeData_Hook(HWND hwnd, LPCWSTR pszClassList) {
    HTHEME result = OpenThemeData_Original(hwnd, pszClassList);
    if (result && pszClassList &&
        _wcsicmp(pszClassList, L"TrayNotifyFlyout") == 0) {
        EnterCriticalSection(&g_lock);
        g_flyoutTheme = result;
        LeaveCriticalSection(&g_lock);
        Wh_Log(L"Flyout theme opened: %p",
               reinterpret_cast<void*>(result));
    }
    return result;
}

using GetThemeMargins_t = HRESULT(WINAPI*)(HTHEME,
                                           HDC,
                                           int,
                                           int,
                                           int,
                                           RECT*,
                                           MARGINS*);
GetThemeMargins_t GetThemeMargins_Original = nullptr;
HRESULT WINAPI GetThemeMargins_Hook(HTHEME hTheme,
                                    HDC hdc,
                                    int iPartId,
                                    int iStateId,
                                    int iPropId,
                                    RECT* prc,
                                    MARGINS* pMargins) {
    HRESULT hr = GetThemeMargins_Original(hTheme, hdc, iPartId, iStateId,
                                          iPropId, prc, pMargins);
    if (SUCCEEDED(hr) && pMargins && !g_unloading &&
        g_settings.overflowCellPadding != kOverflowCellPaddingOff &&
        iPartId == kFlyoutPart && iStateId == kFlyoutState &&
        iPropId == kPropContentMargins) {
        EnterCriticalSection(&g_lock);
        HTHEME flyout = g_flyoutTheme;
        LeaveCriticalSection(&g_lock);
        if (hTheme && hTheme == flyout) {
            int pad = g_settings.overflowCellPadding;
            pMargins->cxLeftWidth = pad;
            pMargins->cxRightWidth = pad;
            pMargins->cyTopHeight = pad;
            pMargins->cyBottomHeight = pad;
            Wh_Log(L"Flyout cell margins -> %d", pad);
        }
    }
    return hr;
}

using CloseThemeData_t = HRESULT(WINAPI*)(HTHEME);
CloseThemeData_t CloseThemeData_Original = nullptr;
HRESULT WINAPI CloseThemeData_Hook(HTHEME hTheme) {
    EnterCriticalSection(&g_lock);
    if (hTheme && hTheme == g_flyoutTheme) {
        g_flyoutTheme = nullptr;
    }
    LeaveCriticalSection(&g_lock);
    return CloseThemeData_Original(hTheme);
}

// Makes the popup re-read its theme values, which is what applies a changed
// cell padding. _UpdateTheme runs on WM_THEMECHANGED, so that message is the
// trigger; the popup re-measures itself on the next open.
void RefreshOverflowTheme() {
    HWND hOverflow = FindWindowW(L"NotifyIconOverflowWindow", nullptr);
    if (!hOverflow || !IsWindow(hOverflow)) {
        return;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hOverflow, &pid);
    if (pid != GetCurrentProcessId()) {
        return;
    }

    SendMessageW(hOverflow, WM_THEMECHANGED, 0, 0);
    Wh_Log(L"Asked the hidden icons popup to reload its theme");
}

void PushOverflowThemePadding() {
    int wanted =
        (g_unloading ||
         g_settings.overflowCellPadding == kOverflowCellPaddingOff)
            ? kOverflowCellPaddingOff
            : g_settings.overflowCellPadding;
    if (wanted == g_lastOverflowPaddingPushed) {
        return;
    }
    g_lastOverflowPaddingPushed = wanted;
    RefreshOverflowTheme();
}

HWND FindOverflowNotifyForToolbar(HWND hToolbar) {
    for (HWND parent = GetParent(hToolbar); parent; parent = GetParent(parent)) {
        WCHAR className[64];
        if (!GetClassNameW(parent, className, ARRAYSIZE(className))) {
            return nullptr;
        }

        if (_wcsicmp(className, L"NotifyIconOverflowWindow") == 0) {
            DWORD pid = 0;
            GetWindowThreadProcessId(parent, &pid);
            return pid == GetCurrentProcessId() ? parent : nullptr;
        }
    }

    return nullptr;
}

// Builds a copy of an image list with all icons scaled to the given size. The
// image list belongs to the toolbar, which sizes its cells from the images.
HIMAGELIST ScaleImageList(HIMAGELIST source, int iconSize) {
    if (!source) {
        return nullptr;
    }

    // An empty list gets an empty copy, which later appends are mirrored into.
    int count = ImageList_GetImageCount(source);
    if (count < 0) {
        return nullptr;
    }

    HIMAGELIST scaled = ImageList_Create(iconSize, iconSize,
                                         ILC_COLOR32 | ILC_MASK,
                                         std::max(count, 1), 1);
    if (!scaled) {
        return nullptr;
    }

    for (int i = 0; i < count; i++) {
        HICON small = ScaleImageListIcon(source, i, iconSize);

        // Picture numbers must stay aligned with the shell's list: a skipped
        // picture would shift every later number and draw the wrong icon under
        // the tooltip. A copy that cannot be complete is not used at all.
        if (!small) {
            Wh_Log(L"Failed to copy icon %d, keeping the shell's image list", i);
            ImageList_Destroy(scaled);
            return nullptr;
        }

        if (ImageList_ReplaceIcon(scaled, -1, small) < 0) {
            Wh_Log(L"Failed to copy icon %d to the scaled image list, keeping "
                   L"the shell's image list",
                   i);
            DestroyIcon(small);
            ImageList_Destroy(scaled);
            return nullptr;
        }

        DestroyIcon(small);
    }

    return scaled;
}

void DestroyScaledImageList() {
    if (g_overflowScaledImageList) {
        ImageList_Destroy(g_overflowScaledImageList);
        g_overflowScaledImageList = nullptr;
    }
}

// Puts the shell's own image list back, but only while the mod's scaled copy
// is still installed: the shell replaces its list when it repopulates the
// popup, and a cached handle that is no longer in use must not be re-sent.
// The cached handles are forgotten either way.
void RestoreOverflowImageList(HWND hToolbar) {
    HIMAGELIST current = reinterpret_cast<HIMAGELIST>(
        SendMessageW(hToolbar, TB_GETIMAGELIST, 0, 0));
    if (g_overflowScaledImageList && current == g_overflowScaledImageList &&
        g_overflowOriginalImageList) {
        g_inImageListSwap = true;
        SendMessageW(
            hToolbar, TB_SETIMAGELIST, 0,
            reinterpret_cast<LPARAM>(g_overflowOriginalImageList.load()));
        g_inImageListSwap = false;
        Wh_Log(L"Overflow image list restored");
    }
    DestroyScaledImageList();
    g_overflowOriginalImageList = nullptr;
    g_overflowAppliedSize = 0;
}

// Applies the configured icon size to the overflow toolbar, or restores the
// shell's own image list. With force, the image list is installed again even
// when it is already the current one, which makes the toolbar recompute its
// button size.
void ApplyOverflowIconSize(HWND hToolbar, bool force = false) {
    if (!hToolbar || !IsWindow(hToolbar)) {
        return;
    }

    if (g_settings.overflowIconSize == kOverflowIconSizeOff || g_unloading) {
        RestoreOverflowImageList(hToolbar);
        return;
    }

    HIMAGELIST current = reinterpret_cast<HIMAGELIST>(
        SendMessageW(hToolbar, TB_GETIMAGELIST, 0, 0));

    // The shell installs its own list whenever it repopulates the popup. Only
    // such a list becomes the source for scaling; never our own scaled list,
    // which would shrink the icons further on every change.
    if (current && current != g_overflowScaledImageList) {
        g_overflowOriginalImageList = current;
        g_overflowAppliedSize = 0;
    }

    if (!g_overflowOriginalImageList) {
        return;
    }

    int iconSize = g_settings.overflowIconSize;
    if (!force && current == g_overflowScaledImageList &&
        g_overflowAppliedSize == iconSize) {
        return;
    }

    HIMAGELIST scaled = ScaleImageList(g_overflowOriginalImageList, iconSize);
    if (!scaled) {
        Wh_Log(L"Failed to scale the overflow image list");
        // A copy made before the shell's last edit no longer matches the
        // buttons, so show the shell's own icons instead.
        RestoreOverflowImageList(hToolbar);
        return;
    }

    HIMAGELIST previous = g_overflowScaledImageList;
    g_overflowScaledImageList = scaled;
    g_overflowAppliedSize = iconSize;

    g_inImageListSwap = true;
    SendMessageW(hToolbar, TB_SETIMAGELIST, 0,
                 reinterpret_cast<LPARAM>(scaled));
    g_inImageListSwap = false;

    // Destroyed only after the toolbar stopped using it.
    if (previous) {
        ImageList_Destroy(previous);
    }

    Wh_Log(L"Overflow icons scaled to %d pixels (image count: %d)", iconSize,
           ImageList_GetImageCount(scaled));
}

LRESULT CALLBACK OverflowToolbarSubclassProc(HWND hWnd,
                                             UINT uMsg,
                                             WPARAM wParam,
                                             LPARAM lParam,
                                             DWORD_PTR dwRefData) {
    if (uMsg == g_overflowRefreshMsg) {
        // Cleared first, so an edit during or after this rebuild queues the
        // next one.
        g_overflowRescalePending = false;
        if (wParam == kOverflowRefreshRestore) {
            RestoreOverflowImageList(hWnd);
        } else if (!g_unloading &&
                   g_settings.overflowIconSize != kOverflowIconSizeOff) {
            ApplyOverflowIconSize(hWnd,
                                  /*force=*/wParam == kOverflowRefreshReapply);
        }
        return 0;
    }

    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case TB_SETIMAGELIST:
            // The shell installs its own list when it populates the popup.
            if (!g_inImageListSwap && !g_unloading &&
                g_settings.overflowIconSize != kOverflowIconSizeOff) {
                ApplyOverflowIconSize(hWnd);
            }
            break;

        case TB_SETBUTTONSIZE:
            // The shell re-applies the theme's button size on theme, DPI and
            // touch changes, which undoes the smaller cells. Re-apply after the
            // shell is done, from a posted message to stay out of its way.
            if (!g_inImageListSwap && !g_unloading &&
                g_settings.overflowIconSize != kOverflowIconSizeOff) {
                PostMessageW(hWnd, g_overflowRefreshMsg,
                             kOverflowRefreshReapply, 0);
            }
            break;

        case WM_NCDESTROY:
            if (hWnd == g_overflowToolbar) {
                g_overflowToolbar = nullptr;
                g_overflowOriginalImageList = nullptr;
                g_overflowAppliedSize = 0;
                // A queued rebuild dies with the window's message queue.
                g_overflowRescalePending = false;
                DestroyScaledImageList();
            }
            break;
    }

    return result;
}

// The popup's image-list state belongs to the toolbar's own thread. Other
// threads ask that thread to apply the configured icon size or to put the
// shell's own list back. The only exceptions are the image-list hooks: on
// other threads they compare against the atomic source handle, and the
// destroy hook clears it.
void RequestOverflowRefresh(bool restore) {
    HWND hToolbar = g_overflowToolbar;
    if (!hToolbar || !IsWindow(hToolbar)) {
        return;
    }

    SendMessageW(hToolbar, g_overflowRefreshMsg,
                 restore ? kOverflowRefreshRestore : kOverflowRefreshApply, 0);
}

// ---------------------------------------------------------------------------
// Discovery, applying, and cleanup
// ---------------------------------------------------------------------------

void SubclassTrayToolbar(HWND hToolbar) {
    if (!hToolbar || !IsWindow(hToolbar)) {
        return;
    }

    if (!FindTrayNotifyForToolbar(hToolbar)) {
        return;
    }

    EnterCriticalSection(&g_lock);
    bool known = FindTrayEntry(hToolbar) != nullptr;
    LeaveCriticalSection(&g_lock);
    if (known) {
        return;
    }

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hToolbar, TrayToolbarSubclassProc, 0)) {
        EnterCriticalSection(&g_lock);
        if (!FindTrayEntry(hToolbar)) {
            TrayEntry entry;
            entry.hToolbar = hToolbar;
            g_trayEntries.push_back(entry);
        }
        LeaveCriticalSection(&g_lock);
    } else {
        Wh_Log(L"Failed to subclass tray toolbar 0x%08X",
               static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hToolbar)));
    }
}

void SubclassTrayTree(HWND hTrayNotify) {
    if (!hTrayNotify || !IsWindowInCurrentProcess(hTrayNotify)) {
        return;
    }

    HWND hPager = FindWindowExW(hTrayNotify, nullptr, L"SysPager", nullptr);
    if (hPager) {
        HWND hToolbar =
            FindWindowExW(hPager, nullptr, L"ToolbarWindow32", nullptr);
        if (hToolbar) {
            SubclassTrayToolbar(hToolbar);
        }
    }

    HWND hDirectToolbar =
        FindWindowExW(hTrayNotify, nullptr, L"ToolbarWindow32", nullptr);
    if (hDirectToolbar) {
        SubclassTrayToolbar(hDirectToolbar);
    }
}

BOOL CALLBACK EnumSecondaryTrayWndProc(HWND hWnd, LPARAM lParam) {
    WCHAR className[64];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0 &&
        IsWindowInCurrentProcess(hWnd)) {
        HWND hTrayNotify =
            FindWindowExW(hWnd, nullptr, L"TrayNotifyWnd", nullptr);
        if (hTrayNotify) {
            SubclassTrayTree(hTrayNotify);
        }
    }

    return TRUE;
}

void DiscoverTrayToolbars() {
    HWND hShellTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hShellTray && IsWindowInCurrentProcess(hShellTray)) {
        HWND hTrayNotify =
            FindWindowExW(hShellTray, nullptr, L"TrayNotifyWnd", nullptr);
        if (hTrayNotify) {
            SubclassTrayTree(hTrayNotify);
        }
    }

    EnumWindows(EnumSecondaryTrayWndProc, 0);
}

void SubclassOverflowToolbar(HWND hToolbar) {
    if (!hToolbar || !IsWindow(hToolbar) || hToolbar == g_overflowToolbar) {
        return;
    }

    if (!FindOverflowNotifyForToolbar(hToolbar)) {
        return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hToolbar, OverflowToolbarSubclassProc, 0)) {
        Wh_Log(L"Failed to subclass overflow toolbar 0x%08X",
               static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hToolbar)));
        return;
    }

    g_overflowToolbar = hToolbar;

    Wh_Log(L"Overflow toolbar 0x%08X",
           static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hToolbar)));
}

BOOL CALLBACK FindOverflowToolbarProc(HWND hWnd, LPARAM lParam) {
    WCHAR className[64];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"ToolbarWindow32") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE;
    }

    return TRUE;
}

void DiscoverOverflowToolbar() {
    HWND hOverflow = FindWindowW(L"NotifyIconOverflowWindow", nullptr);
    if (hOverflow) {
        HWND hOverflowToolbar = nullptr;
        EnumChildWindows(hOverflow, FindOverflowToolbarProc,
                         reinterpret_cast<LPARAM>(&hOverflowToolbar));
        SubclassOverflowToolbar(hOverflowToolbar);
    }
}

void DiscoverAndSubclass() {
    DiscoverTrayToolbars();
    DiscoverOverflowToolbar();
}

// A taskbar reload, sent to this process's taskbar windows (the primary one
// and any secondary ones) using the documented TraySettings message. It goes
// to the taskbar windows directly instead of a session-wide broadcast, so no
// other application is asked to re-read its settings; folder-window processes
// have no taskbar and simply do nothing. Because the tray subclass remains
// installed, the shell's own reload runs through the same hooks and keeps its
// window geometry and button state consistent. This is the only tray refresh
// the mod performs: it applies, restores, and unloads without artificial
// resizing.
BOOL CALLBACK ReloadTrayIconsProc(HWND hWnd, LPARAM lParam) {
    if (!IsWindowInCurrentProcess(hWnd)) {
        return TRUE;
    }

    WCHAR className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") != 0 &&
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
        return TRUE;
    }

    DWORD_PTR result = 0;
    SendMessageTimeoutW(hWnd, WM_SETTINGCHANGE, 0,
                        reinterpret_cast<LPARAM>(L"TraySettings"),
                        SMTO_ABORTIFHUNG, 1000, &result);
    *reinterpret_cast<bool*>(lParam) = true;
    return TRUE;
}

void ReloadTrayIcons() {
    bool delivered = false;
    EnumWindows(ReloadTrayIconsProc, reinterpret_cast<LPARAM>(&delivered));
    if (delivered) {
        Wh_Log(L"Asked the shell to reload the tray");
    }
}

// Releases any pending tray-flag state before a natural reload. The restore
// message runs through the toolbar subclass, so the bytes are returned to the
// values Explorer had before the current layout attempt began.
void ClearPendingTrayFlags() {
    std::vector<HWND> toolbars;
    EnterCriticalSection(&g_lock);
    for (const auto& entry : g_trayEntries) {
        if (entry.hToolbar) {
            toolbars.push_back(entry.hToolbar);
        }
    }
    LeaveCriticalSection(&g_lock);

    for (HWND hToolbar : toolbars) {
        if (hToolbar && IsWindow(hToolbar)) {
            SendMessageW(hToolbar, g_restoreMsg, 0, 0);
        }
    }
}

// Applies the current settings. The tray is only asked to reload when the
// tray padding itself changed: the TraySettings message makes the taskbar
// re-read all of its settings, which a popup-only change does not need.
void ApplyAll(bool reloadTray) {
    DiscoverAndSubclass();

    // With tray redirection disabled, clear any pending flag state first so
    // the natural reload sees Explorer's own values. Either way, the shell's
    // own reload carries the current padding through the installed hooks.
    if (!TrayPaddingActive()) {
        ClearPendingTrayFlags();
    }
    if (reloadTray) {
        ReloadTrayIcons();
    }

    RequestOverflowRefresh(g_settings.overflowIconSize == kOverflowIconSizeOff);
    PushOverflowThemePadding();
}

// New toolbars are subclassed as they appear, so the mod also survives a tray
// toolbar being recreated.
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }

    WCHAR className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return hWnd;
    }

    if (_wcsicmp(className, L"ToolbarWindow32") == 0) {
        if (FindTrayNotifyForToolbar(hWnd)) {
            SubclassTrayToolbar(hWnd);
        } else if (FindOverflowNotifyForToolbar(hWnd)) {
            SubclassOverflowToolbar(hWnd);
        }
    }

    return hWnd;
}

void LoadSettings() {
    g_settings.trayIconPadding = Wh_GetIntSetting(L"padding");
    if (g_settings.trayIconPadding > kMaxIconPadding) {
        Wh_Log(L"Tray padding %d is above the maximum, clamping to %d",
               g_settings.trayIconPadding, kMaxIconPadding);
        g_settings.trayIconPadding = kMaxIconPadding;
    } else if (g_settings.trayIconPadding < kTrayIconPaddingOff) {
        g_settings.trayIconPadding = kTrayIconPaddingOff;
    }

    g_settings.overflowIconSize = Wh_GetIntSetting(L"overflowIconSize");
    if (g_settings.overflowIconSize > kOverflowIconSizeMax) {
        Wh_Log(L"Overflow icon size %d is above the maximum, clamping to %d",
               g_settings.overflowIconSize, kOverflowIconSizeMax);
        g_settings.overflowIconSize = kOverflowIconSizeMax;
    } else if (g_settings.overflowIconSize != kOverflowIconSizeOff &&
               g_settings.overflowIconSize < kOverflowIconSizeMin) {
        g_settings.overflowIconSize = kOverflowIconSizeMin;
    }

    g_settings.overflowCellPadding = Wh_GetIntSetting(L"overflowCellPadding");
    if (g_settings.overflowCellPadding > kOverflowCellPaddingMax) {
        Wh_Log(L"Overflow cell padding %d is above the maximum, clamping to %d",
               g_settings.overflowCellPadding, kOverflowCellPaddingMax);
        g_settings.overflowCellPadding = kOverflowCellPaddingMax;
    } else if (g_settings.overflowCellPadding < kOverflowCellPaddingOff) {
        g_settings.overflowCellPadding = kOverflowCellPaddingOff;
    }

    Wh_Log(L"Settings: trayIconPadding=%d overflowIconSize=%d "
           L"overflowCellPadding=%d",
           g_settings.trayIconPadding, g_settings.overflowIconSize,
           g_settings.overflowCellPadding);
}

}  // namespace

// The callback functions must stay outside the anonymous namespace: Windhawk
// looks them up in the loaded library by their mangled global names.

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE hExplorer = GetModuleHandleW(nullptr);
    MODULEINFO moduleInfo{};
    if (GetModuleInformation(GetCurrentProcess(), hExplorer, &moduleInfo,
                             sizeof(moduleInfo))) {
        g_explorerBase = reinterpret_cast<ULONG_PTR>(moduleInfo.lpBaseOfDll);
        g_explorerEnd = g_explorerBase + moduleInfo.SizeOfImage;
    }

    InitializeCriticalSection(&g_lock);
    g_restoreMsg =
        RegisterWindowMessageW(L"Windhawk_TrayIconSpacing_Restore_" WH_MOD_ID);
    g_overflowRefreshMsg = RegisterWindowMessageW(
        L"Windhawk_TrayIconSpacing_OverflowRefresh_" WH_MOD_ID);

    LoadSettings();

    WindhawkUtils::SetFunctionHook(GetSystemMetrics, GetSystemMetrics_Hook,
                                   &GetSystemMetrics_Original);
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    // Tray icon changes edit the shared image list in place.
    WindhawkUtils::SetFunctionHook(ImageList_ReplaceIcon,
                                   ImageList_ReplaceIcon_Hook,
                                   &ImageList_ReplaceIcon_Original);
    WindhawkUtils::SetFunctionHook(ImageList_Remove, ImageList_Remove_Hook,
                                   &ImageList_Remove_Original);
    WindhawkUtils::SetFunctionHook(ImageList_Destroy, ImageList_Destroy_Hook,
                                   &ImageList_Destroy_Original);

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        auto getSystemMetricsForDpi = reinterpret_cast<GetSystemMetricsForDpi_t>(
            GetProcAddress(hUser32, "GetSystemMetricsForDpi"));
        if (getSystemMetricsForDpi) {
            WindhawkUtils::SetFunctionHook(getSystemMetricsForDpi,
                                           GetSystemMetricsForDpi_Hook,
                                           &GetSystemMetricsForDpi_Original);
        }
    }

    // Resolved dynamically so the mod needs no uxtheme link dependency. It is
    // normally already loaded; the fallback loads it from System32 only, so
    // the load cannot be redirected to a foreign DLL.
    HMODULE hUxTheme = GetModuleHandleW(L"uxtheme.dll");
    if (!hUxTheme) {
        hUxTheme = LoadLibraryExW(L"uxtheme.dll", nullptr,
                                  LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (hUxTheme) {
        auto openThemeDataForDpi =
            reinterpret_cast<OpenThemeDataForDpi_t>(
                GetProcAddress(hUxTheme, "OpenThemeDataForDpi"));
        if (openThemeDataForDpi) {
            WindhawkUtils::SetFunctionHook(openThemeDataForDpi,
                                           OpenThemeDataForDpi_Hook,
                                           &OpenThemeDataForDpi_Original);
        }
        auto openThemeData = reinterpret_cast<OpenThemeData_t>(
            GetProcAddress(hUxTheme, "OpenThemeData"));
        if (openThemeData) {
            WindhawkUtils::SetFunctionHook(openThemeData,
                                           OpenThemeData_Hook,
                                           &OpenThemeData_Original);
        }
        auto getThemeMargins = reinterpret_cast<GetThemeMargins_t>(
            GetProcAddress(hUxTheme, "GetThemeMargins"));
        if (getThemeMargins) {
            WindhawkUtils::SetFunctionHook(getThemeMargins,
                                           GetThemeMargins_Hook,
                                           &GetThemeMargins_Original);
        }
        auto closeThemeData = reinterpret_cast<CloseThemeData_t>(
            GetProcAddress(hUxTheme, "CloseThemeData"));
        if (closeThemeData) {
            WindhawkUtils::SetFunctionHook(closeThemeData,
                                           CloseThemeData_Hook,
                                           &CloseThemeData_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // The reload applies the tray padding live when the mod is enabled in a
    // running Explorer; there is nothing to reload with the tray value off.
    ApplyAll(TrayPaddingActive());
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    int previousTrayIconPadding = g_settings.trayIconPadding;
    LoadSettings();
    ApplyAll(previousTrayIconPadding != g_settings.trayIconPadding);
}

void RemoveTraySubclasses() {
    std::vector<HWND> toolbars;
    EnterCriticalSection(&g_lock);
    for (const auto& entry : g_trayEntries) {
        if (entry.hToolbar) {
            toolbars.push_back(entry.hToolbar);
        }
    }
    LeaveCriticalSection(&g_lock);

    for (HWND hToolbar : toolbars) {
        if (hToolbar && IsWindow(hToolbar)) {
            SendMessageW(hToolbar, g_restoreMsg, 0, 0);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hToolbar, TrayToolbarSubclassProc);
        }
    }

    EnterCriticalSection(&g_lock);
    g_trayEntries.clear();
    LeaveCriticalSection(&g_lock);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    bool trayPaddingWasActive = TrayPaddingActive();

    g_unloading = true;

    // Disable redirection first, then let the shell recompute its own tray
    // geometry. No artificial button, padding, or window values are written.
    ClearPendingTrayFlags();

    // Let go of the hidden icons popup.
    RequestOverflowRefresh(/*restore=*/true);
    PushOverflowThemePadding();

    if (trayPaddingWasActive) {
        ReloadTrayIcons();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    RemoveTraySubclasses();

    HWND hOverflowToolbar = g_overflowToolbar;
    if (hOverflowToolbar && IsWindow(hOverflowToolbar)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hOverflowToolbar, OverflowToolbarSubclassProc);
    }
    g_overflowToolbar = nullptr;

    DeleteCriticalSection(&g_lock);
}
