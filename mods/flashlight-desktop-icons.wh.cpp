// ==WindhawkMod==
// @id              flashlight-desktop-icons
// @name            Flashlight Desktop Icons
// @description     Make desktop icons semi-transparent or invisible when idle, and reveal ONLY the icons near the mouse cursor within a customizable radius. Works flawlessly with Wallpaper Engine.
// @version         1.0.0
// @author          dantepicachu
// @github          https://github.com/dantepicachu11-coder
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -luser32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Flashlight Desktop Icons

Makes desktop icons semi-transparent (or completely invisible) when idle. When
the mouse cursor moves over the desktop, only the icons within the configured
pixel radius smoothly reveal themselves, and they fade back to their idle
opacity once the mouse stops moving.

![Flashlight Desktop Icons demo](https://i.imgur.com/UvM4nxK.gif)

* No overlay windows: each icon's alpha is applied at draw time with GDI custom
  draw, so nothing blocks clicks and no wallpaper layers are duplicated.
  Compatible with Wallpaper Engine and other live wallpapers.
* No background threads and no polling: repainting is driven by mouse events
  and a fade timer running on the desktop's own thread, so the mod does no work
  at all while the mouse is away from the desktop.
* Labels fade together with their icons. Enable the "Hide Icon Labels" setting
  to remove the text under desktop icons entirely.
* Dragging a file over the desktop reveals the icons around the drag cursor,
  so you can see what you are dropping onto.
* Double-clicking empty desktop space hides all icons; double-clicking again
  returns to flashlight mode (rather than making all icons fully visible).

## How this differs from similar mods

* [Transparent Desktop Icons with Spotlight](https://windhawk.net/mods/transparent-desktop-icons-spotlight)
  reveals icons around the cursor using a DirectComposition overlay mask. This
  mod instead computes each icon's alpha from its distance to the cursor and
  applies it per icon at draw time (GDI custom draw), with no overlay or
  wallpaper layers.
* [Desktop Icons Transparency](https://windhawk.net/mods/desktop-icons-transparency)
  applies one fixed opacity to all desktop icons. This mod's opacity is
  per-icon and follows the cursor.
* [Desktop Icon Section Auto-Hide & Fluent Hover Reveal](https://windhawk.net/mods/desktop-icon-section-autohide)
  and [ZenDesktop](https://windhawk.net/mods/zen-desktop-toggle-icons) hide and
  reveal icons in bulk (per section or on toggle). This mod's default
  experience is a continuous visibility gradient around the cursor.

## Known limitations

* Faded labels are drawn in the mod's own style (white text with a glow)
  rather than Explorer's shadow style, so a label changes slightly when its
  icon reaches full opacity and Explorer takes over drawing it.

## Credits

* [desktop-icons-transparency](https://windhawk.net/mods/desktop-icons-transparency)
  by zed712969-crypto: desktop window discovery and subclassing patterns.
* [Smooth Desktop Icons Auto-Hide](https://windhawk.net/mods/smooth-desktop-icons-auto-hide)
  by HeyOkay: the drag-and-drop reveal approach (hooking the desktop's
  IDropTarget vtable methods).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- idleOpacity: 0
  $name: Idle Opacity (0-255)
  $description: Opacity of icons when idle/far away from cursor (0 = invisible, 255 = fully opaque)
- activeOpacity: 255
  $name: Active Opacity (0-255)
  $description: Opacity of icons directly under or near the mouse cursor (255 = fully visible)
- spotlightRadius: 200
  $name: Mouse Spotlight Radius (pixels)
  $description: Radial distance in pixels from cursor where icons begin to fade into active opacity.
- idleDelay: 500
  $name: Mouse Idle Timeout (ms)
  $description: Delay in milliseconds before icons fade back to idle opacity after mouse stops moving
- spotlightFadeOutDuration: 800
  $name: Fade Out Duration (ms)
  $description: How long it takes for icons to fade out after the idle delay
- hideLabels: false
  $name: Hide Icon Labels
  $description: Never draw the text labels under desktop icons. Selected icons still show their name, since Explorer draws them.
- toggleOnDoubleClick: true
  $name: Toggle Icons on Double-Click
  $description: Double-click on empty desktop background to hide all icons; double-click again to return to flashlight mode.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <ole2.h>
#include <Uxtheme.h>
#include <vssym32.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <mutex>
#include <vector>

#include <windhawk_utils.h>

////////////////////////////////////////////////////////////////////////////////
// Settings

struct Settings {
    int idleOpacity;
    int activeOpacity;
    int spotlightRadius;
    int idleDelay;
    int spotlightFadeOutDuration;
    bool hideLabels;
    bool toggleOnDoubleClick;
};

// Written by the Windhawk engine thread (LoadSettings), read by the desktop
// thread, so all access is guarded by a mutex.
Settings g_settings;
std::mutex g_settingsMutex;

Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

////////////////////////////////////////////////////////////////////////////////
// Global state

// Set from the engine thread (Wh_ModAfterInit / Wh_ModUninit) or from the
// desktop thread (CreateWindowExW hook), read by the desktop thread.
std::atomic<HWND> g_listViewWnd{nullptr};   // Desktop SysListView32 (subclassed)
std::atomic<HWND> g_shellViewWnd{nullptr};  // Parent SHELLDLL_DefView (subclassed)

// Set at the start of Wh_ModBeforeUninit (and again in Wh_ModUninit) so that
// the fade timer can't be re-armed by a WM_MOUSEMOVE or a queued WM_TIMER
// processed before the subclass is removed.
std::atomic<bool> g_unloading{false};

// The desktop's own IDropTarget object, found via the window property that
// RegisterDragDrop sets. The hooked vtable methods may be shared with other
// shell drop targets, so the hooks only react to this exact object.
std::atomic<IDropTarget*> g_desktopDropTarget{nullptr};

// The hooks are installed at most once: the attempt is claimed atomically,
// so EnsureDropTargetHook can't race itself when called from two threads,
// and a failed SetFunctionHook isn't retried on every mouse move.
std::atomic<bool> g_dropTargetHookAttempted{false};

// Posted to the desktop list view on unload so that the fade timer is killed
// on its owning thread, while the subclass that handles it is still installed.
UINT g_msgKillFadeTimer = 0;

// Posted to the desktop list view by the drop target hooks, carrying the
// drag cursor position (screen coordinates) during an OLE drag.
UINT g_msgDragPosition = 0;

// All of the state below is only ever touched by the desktop's UI thread (the
// subclass procs, the fade timer and the custom draw handler).
HTHEME g_hTheme = nullptr;                  // Cached theme handle
POINT g_mousePos = {-32000, -32000};        // Last cursor pos, list view client coordinates
ULONGLONG g_lastMouseMoveTime = 0;          // GetTickCount64() of the last WM_MOUSEMOVE
bool g_trackingMouseLeave = false;          // TrackMouseEvent(TME_LEAVE) is pending
bool g_fadeTicking = false;                 // Fade timer is in its ~60 FPS tick phase
bool g_iconsHidden = false;                 // Set by the double-click toggle
std::vector<BYTE> g_lastItemAlphas;         // Per-item alpha, for targeted invalidation

constexpr UINT_PTR kFadeTimerId = 0xF1A5;  // Unlikely to collide with comctl32 internal timers
constexpr UINT kFadeTickIntervalMs = 16;   // ~60 FPS while a fade is running

////////////////////////////////////////////////////////////////////////////////
// Alpha math

float GetDistanceToRect(const POINT& pt, const RECT& rc) {
    // Squaring in float: with the off-screen sentinel position, dx*dx + dy*dy
    // would overflow a 32-bit int.
    float dx = (float)(std::max)({(int)(rc.left - pt.x), 0, (int)(pt.x - rc.right + 1)});
    float dy = (float)(std::max)({(int)(rc.top - pt.y), 0, (int)(pt.y - rc.bottom + 1)});
    return std::sqrt(dx * dx + dy * dy);
}

BYTE CalculateIconAlpha(HWND hListView, int iItem) {
    if (g_iconsHidden) {
        return 0;
    }

    Settings settings = GetSettings();

    RECT rcBounds = {LVIR_BOUNDS};
    if (!SendMessageW(hListView, LVM_GETITEMRECT, iItem, (LPARAM)&rcBounds)) {
        return (BYTE)settings.idleOpacity;
    }

    float dist = GetDistanceToRect(g_mousePos, rcBounds);
    float radius = (float)(std::max)(settings.spotlightRadius, 1);

    float spotlightFade = 0.0f;
    if (dist <= radius) {
        spotlightFade = 1.0f - dist / radius;
    }

    ULONGLONG elapsed = GetTickCount64() - g_lastMouseMoveTime;
    float timeFade = 1.0f;
    if (elapsed > (ULONGLONG)settings.idleDelay) {
        float fadeTime = (float)(elapsed - settings.idleDelay);
        if (settings.spotlightFadeOutDuration > 0) {
            timeFade = 1.0f - fadeTime / settings.spotlightFadeOutDuration;
            timeFade = std::clamp(timeFade, 0.0f, 1.0f);
        } else {
            timeFade = 0.0f;
        }
    }

    float finalFade = spotlightFade * timeFade;
    int alpha = settings.idleOpacity +
                (int)((settings.activeOpacity - settings.idleOpacity) * finalFade);
    return (BYTE)std::clamp(alpha, 0, 255);
}

////////////////////////////////////////////////////////////////////////////////
// Repainting: invalidate only the icons whose alpha actually changed

void UpdateIconVisibility(HWND hListView) {
    int count = (int)SendMessageW(hListView, LVM_GETITEMCOUNT, 0, 0);
    if (count < 0) {
        return;
    }

    bool refreshAll = ((int)g_lastItemAlphas.size() != count);
    if (refreshAll) {
        g_lastItemAlphas.assign(count, 0);
        InvalidateRect(hListView, nullptr, FALSE);
    }

    for (int iItem = 0; iItem < count; iItem++) {
        BYTE alpha = CalculateIconAlpha(hListView, iItem);
        if (g_lastItemAlphas[iItem] == alpha) {
            continue;
        }
        g_lastItemAlphas[iItem] = alpha;
        if (refreshAll) {
            continue;  // Already invalidated the whole client area above.
        }

        RECT rc = {LVIR_BOUNDS};
        if (SendMessageW(hListView, LVM_GETITEMRECT, iItem, (LPARAM)&rc)) {
            // Small margin for the label glow and selection outline.
            InflateRect(&rc, 4, 4);
            InvalidateRect(hListView, &rc, FALSE);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Fade timer: runs on the desktop thread, only between a mouse move and the
// end of the fade-out. Killed as soon as the fade completes.

void ScheduleFadeTimer(HWND hListView) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    if (g_fadeTicking) {
        // Already ticking; OnFadeTimer re-evaluates the state on every tick.
        return;
    }

    Settings settings = GetSettings();
    ULONGLONG elapsed = GetTickCount64() - g_lastMouseMoveTime;
    ULONGLONG remaining =
        elapsed >= (ULONGLONG)settings.idleDelay
            ? 1
            : (ULONGLONG)settings.idleDelay - elapsed + 1;
    SetTimer(hListView, kFadeTimerId, (UINT)remaining, nullptr);
}

void OnFadeTimer(HWND hListView) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        // Unload in progress: don't touch the timer state, the fade timer has
        // been (or is about to be) killed via the registered message.
        KillTimer(hListView, kFadeTimerId);
        g_fadeTicking = false;
        return;
    }

    Settings settings = GetSettings();
    ULONGLONG elapsed = GetTickCount64() - g_lastMouseMoveTime;

    if (elapsed < (ULONGLONG)settings.idleDelay) {
        // The mouse moved (or the settings changed) since the timer was set;
        // wait for the remaining delay.
        g_fadeTicking = false;
        SetTimer(hListView, kFadeTimerId,
                 (UINT)((ULONGLONG)settings.idleDelay - elapsed + 1), nullptr);
        return;
    }

    UpdateIconVisibility(hListView);

    ULONGLONG fadeEnd = (ULONGLONG)settings.idleDelay +
                        (ULONGLONG)settings.spotlightFadeOutDuration;
    if (elapsed >= fadeEnd) {
        // Fade complete, everything is at idle opacity now. Stop the timer.
        KillTimer(hListView, kFadeTimerId);
        g_fadeTicking = false;
    } else if (!g_fadeTicking) {
        g_fadeTicking = true;
        SetTimer(hListView, kFadeTimerId, kFadeTickIntervalMs, nullptr);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Custom draw: draws one item (icon + label) with the given alpha

void DrawIconWithAlpha(HWND hListView, HDC hdc, int iItem,
                       const LVITEMW& lvi, BYTE alpha) {
    HIMAGELIST himl =
        (HIMAGELIST)SendMessageW(hListView, LVM_GETIMAGELIST, LVSIL_NORMAL, 0);
    if (!himl) {
        return;
    }

    RECT rcIcon = {LVIR_ICON};
    if (!SendMessageW(hListView, LVM_GETITEMRECT, iItem, (LPARAM)&rcIcon)) {
        return;
    }

    // Query the live icon size so that custom sizes (Ctrl+mouse wheel on the
    // desktop) are respected.
    int iconWidth = 32, iconHeight = 32;
    ImageList_GetIconSize(himl, &iconWidth, &iconHeight);

    int drawX = rcIcon.left + ((rcIcon.right - rcIcon.left) - iconWidth) / 2;
    int drawY = rcIcon.top + ((rcIcon.bottom - rcIcon.top) - iconHeight) / 2;

    IMAGELISTDRAWPARAMS imldp = {};
    imldp.cbSize = sizeof(imldp);
    imldp.himl = himl;
    imldp.i = lvi.iImage;
    imldp.hdcDst = hdc;
    imldp.x = drawX;
    imldp.y = drawY;
    imldp.cx = iconWidth;
    imldp.cy = iconHeight;
    // The LVIS_OVERLAYMASK state bits use the same bit positions as the
    // overlay index in fStyle, so shortcut arrows, cloud/sync badges etc.
    // keep being drawn at partial alpha.
    imldp.fStyle = ILD_TRANSPARENT | (lvi.state & LVIS_OVERLAYMASK);
    imldp.fState = ILS_ALPHA;
    imldp.Frame = alpha;

    ImageList_DrawIndirect(&imldp);
}

void DrawLabelWithAlpha(HWND hListView, HDC hdc, int iItem, BYTE alpha) {
    RECT rcLabel = {LVIR_LABEL};
    if (!SendMessageW(hListView, LVM_GETITEMRECT, iItem, (LPARAM)&rcLabel)) {
        return;
    }
    if (rcLabel.right <= rcLabel.left || rcLabel.bottom <= rcLabel.top) {
        return;
    }

    WCHAR szText[256] = {};
    LVITEMW lvi = {};
    lvi.mask = LVIF_TEXT;
    lvi.iItem = iItem;
    lvi.pszText = szText;
    lvi.cchTextMax = ARRAYSIZE(szText);
    SendMessageW(hListView, LVM_GETITEMW, 0, (LPARAM)&lvi);
    if (!szText[0]) {
        return;
    }

    HDC hdcBuffer = nullptr;
    BP_PAINTPARAMS bpParams = {sizeof(bpParams)};
    bpParams.dwFlags = BPPF_ERASE;
    // EndBufferedPaint(TRUE) must alpha-blend the (premultiplied) buffer onto
    // the wallpaper. Without a blend function the buffer, which is erased to
    // transparent black, would be copied as-is and paint an opaque black box.
    BLENDFUNCTION blendFunction = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    bpParams.pBlendFunction = &blendFunction;

    HPAINTBUFFER hbp =
        BeginBufferedPaint(hdc, &rcLabel, BPBF_TOPDOWNDIB, &bpParams, &hdcBuffer);
    if (!hbp) {
        return;
    }

    // Note: the buffer DC uses the same coordinate space as the target DC
    // (its window origin is set to the target rect), so drawing uses rcLabel.
    HFONT hFont = (HFONT)SendMessageW(hListView, WM_GETFONT, 0, 0);
    HFONT hOldFont = (HFONT)SelectObject(hdcBuffer, hFont);

    if (g_hTheme) {
        DTTOPTS dttOpts = {sizeof(dttOpts)};
        dttOpts.dwFlags = DTT_TEXTCOLOR | DTT_COMPOSITED | DTT_GLOWSIZE;
        dttOpts.crText = RGB(255, 255, 255);
        dttOpts.iGlowSize = 5;
        DrawThemeTextEx(g_hTheme, hdcBuffer, LVP_LISTITEM, 0, szText, -1,
                        DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX,
                        &rcLabel, &dttOpts);
    } else {
        SetBkMode(hdcBuffer, TRANSPARENT);
        SetTextColor(hdcBuffer, RGB(255, 255, 255));
        DrawTextW(hdcBuffer, szText, -1, &rcLabel,
                  DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
    }

    SelectObject(hdcBuffer, hOldFont);

    // Scale the per-pixel alpha (and premultiply the colors accordingly) so
    // that the label fades together with the icon.
    RGBQUAD* pBits = nullptr;
    int cxRow = 0;
    if (SUCCEEDED(GetBufferedPaintBits(hbp, &pBits, &cxRow)) && pBits) {
        int width = rcLabel.right - rcLabel.left;
        int height = rcLabel.bottom - rcLabel.top;
        for (int y = 0; y < height; y++) {
            RGBQUAD* pRow = pBits + (y * cxRow);
            for (int x = 0; x < width; x++) {
                if (!g_hTheme && pRow[x].rgbReserved == 0) {
                    // Plain GDI text doesn't set the alpha channel. Estimate
                    // the coverage of the white text from its color so that
                    // anti-aliased edge pixels don't get a dark fringe.
                    BYTE coverage = (std::max)({pRow[x].rgbRed, pRow[x].rgbGreen,
                                                pRow[x].rgbBlue});
                    pRow[x].rgbReserved = coverage;
                }
                pRow[x].rgbReserved = (BYTE)(pRow[x].rgbReserved * alpha / 255);
                pRow[x].rgbRed = (BYTE)(pRow[x].rgbRed * alpha / 255);
                pRow[x].rgbGreen = (BYTE)(pRow[x].rgbGreen * alpha / 255);
                pRow[x].rgbBlue = (BYTE)(pRow[x].rgbBlue * alpha / 255);
            }
        }
    }

    EndBufferedPaint(hbp, TRUE);
}

// result is what DefView and any previously installed subclasses returned
// for this notification; it must be passed through for everything the mod
// doesn't draw or hide itself.
LRESULT HandleCustomDraw(LPNMLVCUSTOMDRAW pLVCD, LRESULT result) {
    HWND hListView = pLVCD->nmcd.hdr.hwndFrom;

    switch (pLVCD->nmcd.dwDrawStage) {
        case CDDS_PREPAINT:
            if (!g_hTheme) {
                g_hTheme = OpenThemeData(hListView, L"Explorer::ListView");
                if (!g_hTheme) {
                    g_hTheme = OpenThemeData(hListView, L"ListView");
                }
            }
            return result | CDRF_NOTIFYITEMDRAW;

        case CDDS_ITEMPREPAINT: {
            int iItem = (int)pLVCD->nmcd.dwItemSpec;

            LVITEMW lvi = {};
            lvi.mask = LVIF_STATE | LVIF_IMAGE;
            lvi.iItem = iItem;
            lvi.stateMask = LVIS_SELECTED | LVIS_CUT | LVIS_OVERLAYMASK;
            SendMessageW(hListView, LVM_GETITEMW, 0, (LPARAM)&lvi);

            // Selected and cut items are transient, highlighted states; let
            // Explorer draw them natively at full opacity.
            if (lvi.state & (LVIS_SELECTED | LVIS_CUT)) {
                return result;
            }

            Settings settings = GetSettings();
            BYTE alpha = CalculateIconAlpha(hListView, iItem);

            // At full opacity Explorer draws the item better than we do, so
            // hand it back - unless labels are hidden, since Explorer would
            // draw the label too.
            if (alpha == 255 && !settings.hideLabels) {
                return result;
            }
            if (alpha == 0) {
                return CDRF_SKIPDEFAULT;
            }

            DrawIconWithAlpha(hListView, pLVCD->nmcd.hdc, iItem, lvi, alpha);
            if (!settings.hideLabels) {
                DrawLabelWithAlpha(hListView, pLVCD->nmcd.hdc, iItem, alpha);
            }
            return CDRF_SKIPDEFAULT;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Subclasses (both installed on the desktop's UI thread)

void EnsureDropTargetHook();

LRESULT CALLBACK ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_MOUSEMOVE:
            // The desktop may register its drop target after the list view is
            // created, and a recreated desktop registers a new one; rerun the
            // (bounded) lookup until the current target is known.
            if (!g_desktopDropTarget.load(std::memory_order_relaxed)) {
                EnsureDropTargetHook();
            }

            g_mousePos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            g_lastMouseMoveTime = GetTickCount64();

            if (!g_trackingMouseLeave) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hWnd, 0};
                if (TrackMouseEvent(&tme)) {
                    g_trackingMouseLeave = true;
                }
            }

            UpdateIconVisibility(hWnd);
            ScheduleFadeTimer(hWnd);
            break;

        case WM_MOUSELEAVE:
            g_trackingMouseLeave = false;
            break;

        case WM_TIMER:
            if (wParam == kFadeTimerId) {
                OnFadeTimer(hWnd);
                return 0;
            }
            break;

        case WM_LBUTTONDBLCLK: {
            Settings settings = GetSettings();
            if (settings.toggleOnDoubleClick) {
                LVHITTESTINFO hti = {};
                hti.pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&hti);
                if (hti.flags & LVHT_NOWHERE) {
                    g_iconsHidden = !g_iconsHidden;
                    UpdateIconVisibility(hWnd);
                }
            }
            break;
        }

        case WM_NCDESTROY:
            if (hWnd == g_listViewWnd.load()) {
                g_listViewWnd = nullptr;
                g_fadeTicking = false;
                g_trackingMouseLeave = false;
                g_lastItemAlphas.clear();
                // The desktop's drop target is revoked together with the
                // desktop; a recreated desktop registers a new object, which
                // EnsureDropTargetHook re-discovers via the WM_MOUSEMOVE retry.
                g_desktopDropTarget = nullptr;
            }
            break;

        default:
            if (g_msgKillFadeTimer && uMsg == g_msgKillFadeTimer) {
                KillTimer(hWnd, kFadeTimerId);
                g_fadeTicking = false;
                return 0;
            }
            if (g_msgDragPosition && uMsg == g_msgDragPosition) {
                // Screen coordinates of the drag cursor, posted by the
                // desktop's IDropTarget hooks during an OLE drag.
                POINT pt = {(int)(INT_PTR)wParam, (int)(INT_PTR)lParam};
                ScreenToClient(hWnd, &pt);
                g_mousePos = pt;
                g_lastMouseMoveTime = GetTickCount64();
                UpdateIconVisibility(hWnd);
                ScheduleFadeTimer(hWnd);
                return 0;
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ShellViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == WM_NOTIFY) {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh && pnmh->hwndFrom && pnmh->hwndFrom == g_listViewWnd.load() &&
            pnmh->code == NM_CUSTOMDRAW) {
            // Let DefView (which e.g. colors compressed/encrypted file names)
            // and any previously installed subclasses handle the notification
            // first, and build on their result.
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            return HandleCustomDraw((LPNMLVCUSTOMDRAW)lParam, result);
        }
    } else if (uMsg == WM_THEMECHANGED) {
        // Refresh the cached theme on theme or high-contrast switches.
        if (g_hTheme) {
            CloseThemeData(g_hTheme);
            g_hTheme = nullptr;
        }
    } else if (uMsg == WM_NCDESTROY) {
        if (hWnd == g_shellViewWnd.load()) {
            g_shellViewWnd = nullptr;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Desktop window discovery

bool IsDesktopParent(HWND hWnd) {
    if (!hWnd) {
        return false;
    }
    if (hWnd == GetShellWindow()) {
        return true;
    }

    WCHAR className[64] = {};
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        if (_wcsicmp(className, L"Progman") == 0 ||
            _wcsicmp(className, L"WorkerW") == 0) {
            return true;
        }
    }

    return false;
}

// Checks that hWnd is a desktop SysListView32 belonging to this
// explorer.exe process (e.g. not the shell's desktop seen from a
// "separate process" folder window).
bool IsDesktopListView(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return false;
    }

    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)) ||
        _wcsicmp(className, L"SysListView32") != 0) {
        return false;
    }

    HWND hDefView = GetParent(hWnd);
    if (!hDefView ||
        !GetClassNameW(hDefView, className, ARRAYSIZE(className)) ||
        _wcsicmp(className, L"SHELLDLL_DefView") != 0) {
        return false;
    }

    return IsDesktopParent(GetAncestor(hDefView, GA_PARENT));
}

// Finds an already-existing desktop list view of the current process, if any.
HWND FindDesktopListView() {
    HWND hDefView = nullptr;

    HWND hShell = GetShellWindow();
    if (hShell) {
        hDefView = FindWindowExW(hShell, nullptr, L"SHELLDLL_DefView", nullptr);
    }

    if (!hDefView) {
        HWND hProgman = FindWindowW(L"Progman", L"Program Manager");
        if (hProgman && hProgman != hShell) {
            hDefView = FindWindowExW(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);
        }
    }

    if (hDefView) {
        HWND hListView =
            FindWindowExW(hDefView, nullptr, L"SysListView32", nullptr);
        if (IsDesktopListView(hListView)) {
            return hListView;
        }
    }

    // With some wallpaper setups the DefView lives under a top-level WorkerW.
    // WorkerW is a generic class that other processes can use too, so verify
    // each candidate instead of stopping at the first one.
    HWND hWorkerW = nullptr;
    while ((hWorkerW =
                FindWindowExW(nullptr, hWorkerW, L"WorkerW", nullptr)) !=
           nullptr) {
        HWND hDefViewW =
            FindWindowExW(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
        if (!hDefViewW) {
            continue;
        }

        HWND hListView =
            FindWindowExW(hDefViewW, nullptr, L"SysListView32", nullptr);
        if (IsDesktopListView(hListView)) {
            return hListView;
        }
    }

    return nullptr;
}

// Subclasses the desktop's SHELLDLL_DefView, which routes NM_CUSTOMDRAW to
// the custom draw handler. Idempotent: returns true if already subclassed.
bool SubclassShellView(HWND hShellView) {
    if (!hShellView || hShellView == g_shellViewWnd.load()) {
        return hShellView != nullptr;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hShellView,
                                                       ShellViewSubclassProc,
                                                       0)) {
        Wh_Log(L"Failed to subclass the desktop DefView");
        return false;
    }

    g_shellViewWnd = hShellView;
    return true;
}

void SubclassDesktop(HWND hListView) {
    if (!hListView || hListView == g_listViewWnd.load()) {
        return;
    }

    HWND hShellView = GetParent(hListView);
    if (!hShellView) {
        return;
    }

    // Defensive: if we still track different windows that are alive (normally
    // they are destroyed first and cleared in WM_NCDESTROY), remove our
    // subclasses from them so that no subclassed window can outlive the mod.
    HWND hOldListView = g_listViewWnd.load();
    if (hOldListView && IsWindow(hOldListView)) {
        if (g_msgKillFadeTimer) {
            SendMessageW(hOldListView, g_msgKillFadeTimer, 0, 0);
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hOldListView,
                                                         ListViewSubclassProc);
    }
    HWND hOldShellView = g_shellViewWnd.load();
    if (hOldShellView && hOldShellView != hShellView &&
        IsWindow(hOldShellView)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hOldShellView,
                                                         ShellViewSubclassProc);
        g_shellViewWnd = nullptr;
    }

    g_lastItemAlphas.clear();

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hListView,
                                                       ListViewSubclassProc, 0)) {
        Wh_Log(L"Failed to subclass the desktop list view");
        return;
    }

    if (!SubclassShellView(hShellView)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hListView,
                                                         ListViewSubclassProc);
        return;
    }

    g_listViewWnd = hListView;
    Wh_Log(L"Subclassed the desktop list view");

    RedrawWindow(hListView, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN);

    // The drop target may be registered by now (if not, the list view
    // subclass retries until it is).
    EnsureDropTargetHook();
}

////////////////////////////////////////////////////////////////////////////////
// CreateWindowExW hook: catches the desktop windows when they are created
// (boot, desktop recreation, Explorer restart of the view, etc.)

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

// Nesting depth of CreateWindowExW calls on the current thread. Window
// creation nests: the desktop list view is created inside its DefView's
// WM_CREATE, which in turn creates tooltips and other children. Windows
// become discoverable (and subclassable "successfully") while still being
// constructed, but a subclass installed at that point does not function.
// Attaching is therefore deferred until the depth returns to zero, i.e. no
// creation is in flight on this thread anymore.
static thread_local int g_createWindowHookDepth = 0;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName, DWORD dwStyle, int X,
                                 int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu,
                                 HINSTANCE hInstance, LPVOID lpParam) {
    g_createWindowHookDepth++;
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    g_createWindowHookDepth--;

    // True only when this hook body runs with no window creation still in
    // flight on this thread, i.e. every existing window is fully constructed.
    bool safeToAttach = (g_createWindowHookDepth == 0);

    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (_wcsicmp(lpClassName, L"SHELLDLL_DefView") == 0) {
            // The desktop's DefView is created before (or while creating) its
            // list view child, so this is the primary boot-time attach point.
            // Folder windows and common dialogs create DefViews too, so check
            // the parent. Pattern follows smooth-desktop-icons-auto-hide.
            if (safeToAttach && IsDesktopParent(GetParent(hWnd))) {
                SubclassShellView(hWnd);
                // The list view usually already exists at this point (it's
                // created during the DefView's WM_CREATE); if not, the
                // SysListView32 branch or the probe below catches it later.
                HWND hListView =
                    FindWindowExW(hWnd, nullptr, L"SysListView32", nullptr);
                if (hListView && IsDesktopListView(hListView)) {
                    SubclassDesktop(hListView);
                }
            }
        } else if (_wcsicmp(lpClassName, L"SysListView32") == 0 &&
                   safeToAttach && IsDesktopListView(hWnd)) {
            SubclassDesktop(hWnd);
        }
    }

    // Boot-time catch-up: if the desktop existed before the mod was loaded,
    // or its creation was missed above, keep probing on subsequent window
    // creations until the desktop is attached. Only attach from the desktop
    // window's own thread at depth zero, so that the windows are fully
    // constructed and no cross-thread creation race is possible.
    if (safeToAttach && !g_listViewWnd.load(std::memory_order_relaxed)) {
        // Explorer processes that don't host the desktop (e.g. folder windows
        // with "Launch folder windows in a separate process") can never pass
        // the desktop checks; skip the probe once the shell window belongs to
        // another process. Before the shell window exists this process may
        // still be the shell, and the search there is trivial anyway.
        HWND hShellWnd = GetShellWindow();
        DWORD shellPid = 0;
        bool shellInOtherProcess =
            hShellWnd && GetWindowThreadProcessId(hShellWnd, &shellPid) &&
            shellPid != GetCurrentProcessId();
        if (!shellInOtherProcess) {
            HWND hDesktop = FindDesktopListView();
            if (hDesktop &&
                GetWindowThreadProcessId(hDesktop, nullptr) ==
                    GetCurrentThreadId()) {
                SubclassDesktop(hDesktop);
            }
        }
    }

    return hWnd;
}

////////////////////////////////////////////////////////////////////////////////
// Drag & drop reveal
//
// During an OLE drag the list view gets no WM_MOUSEMOVE, so the icons would
// stay at idle opacity exactly while the user is aiming at them. The desktop
// registers an IDropTarget with OLE, and every drag over the desktop, from
// any process, ends up calling that object's DragEnter/DragOver/Drop on the
// desktop thread. Hooking those vtable methods tracks the drag cursor with
// no polling. Pattern follows smooth-desktop-icons-auto-hide.

using DropTargetDragEnter_t = HRESULT(STDMETHODCALLTYPE*)(
    IDropTarget*, IDataObject*, DWORD, POINTL, DWORD*);
using DropTargetDragOver_t = HRESULT(STDMETHODCALLTYPE*)(
    IDropTarget*, DWORD, POINTL, DWORD*);
using DropTargetDrop_t = HRESULT(STDMETHODCALLTYPE*)(
    IDropTarget*, IDataObject*, DWORD, POINTL, DWORD*);

DropTargetDragEnter_t DropTargetDragEnter_Original = nullptr;
DropTargetDragOver_t DropTargetDragOver_Original = nullptr;
DropTargetDrop_t DropTargetDrop_Original = nullptr;

void NotifyDragPosition(const IDropTarget* self, const POINTL& pt) {
    if (self != g_desktopDropTarget.load() || !g_msgDragPosition ||
        g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    HWND hListView = g_listViewWnd.load();
    if (hListView) {
        PostMessageW(hListView, g_msgDragPosition,
                     (WPARAM)(INT_PTR)pt.x, (LPARAM)(INT_PTR)pt.y);
    }
}

HRESULT STDMETHODCALLTYPE DropTargetDragEnter_Hook(
    IDropTarget* self, IDataObject* pDataObj, DWORD grfKeyState, POINTL pt,
    DWORD* pdwEffect) {
    NotifyDragPosition(self, pt);
    return DropTargetDragEnter_Original(self, pDataObj, grfKeyState, pt,
                                        pdwEffect);
}

HRESULT STDMETHODCALLTYPE DropTargetDragOver_Hook(
    IDropTarget* self, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) {
    NotifyDragPosition(self, pt);
    return DropTargetDragOver_Original(self, grfKeyState, pt, pdwEffect);
}

HRESULT STDMETHODCALLTYPE DropTargetDrop_Hook(
    IDropTarget* self, IDataObject* pDataObj, DWORD grfKeyState, POINTL pt,
    DWORD* pdwEffect) {
    NotifyDragPosition(self, pt);
    return DropTargetDrop_Original(self, pDataObj, grfKeyState, pt, pdwEffect);
}

bool IsAddressInLoadedModule(void* p) {
    HMODULE module = nullptr;
    return p &&
           GetModuleHandleExW(
               GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
               (LPCWSTR)p, &module) &&
           module;
}

IDropTarget* FindDesktopDropTarget() {
    HWND hShellView = g_shellViewWnd.load();
    HWND candidates[] = {
        g_listViewWnd.load(),
        hShellView,
        hShellView ? GetParent(hShellView) : nullptr,
    };

    for (HWND hwnd : candidates) {
        if (!hwnd) {
            continue;
        }

        // RegisterDragDrop stores the in-process IDropTarget in this window
        // property.
        auto* target = static_cast<IDropTarget*>(
            GetPropW(hwnd, L"OleDropTargetInterface"));
        if (!target) {
            continue;
        }

        // IDropTarget vtable: QueryInterface, AddRef, Release, DragEnter,
        // DragOver, DragLeave, Drop.
        void** vtbl = *reinterpret_cast<void***>(target);
        if (IsAddressInLoadedModule(vtbl[3]) &&
            IsAddressInLoadedModule(vtbl[4]) &&
            IsAddressInLoadedModule(vtbl[6])) {
            return target;
        }
    }

    return nullptr;
}

// Runs on the desktop thread (subclass proc, CreateWindowExW hook) or on the
// engine thread (Wh_ModAfterInit), always after Wh_ModInit.
void EnsureDropTargetHook() {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    // Always publish the lookup result, including nullptr: a stale pointer
    // must not survive a desktop recreation (the hooks only react to this
    // exact object, so nullptr simply means "no drag reveal for now").
    IDropTarget* target = FindDesktopDropTarget();
    g_desktopDropTarget = target;

    // Claim the installation attempt atomically: only one thread (desktop
    // or engine) may install the hooks, and a failure is not retried.
    if (!target ||
        g_dropTargetHookAttempted.exchange(true, std::memory_order_relaxed)) {
        return;
    }

    void** vtbl = *reinterpret_cast<void***>(target);
    bool ok =
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<DropTargetDragEnter_t>(vtbl[3]),
            DropTargetDragEnter_Hook, &DropTargetDragEnter_Original) &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<DropTargetDragOver_t>(vtbl[4]),
            DropTargetDragOver_Hook, &DropTargetDragOver_Original) &&
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<DropTargetDrop_t>(vtbl[6]),
            DropTargetDrop_Hook, &DropTargetDrop_Original);
    if (!ok) {
        Wh_Log(L"Drop target hooks failed");
        return;
    }

    // This runs after Wh_ModInit, so the hooks must be applied explicitly.
    Wh_ApplyHookOperations();

    Wh_Log(L"Drop target hooks set");
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

void LoadSettings() {
    Settings s;
    s.idleOpacity = std::clamp(Wh_GetIntSetting(L"idleOpacity"), 0, 255);
    s.activeOpacity = std::clamp(Wh_GetIntSetting(L"activeOpacity"), 0, 255);
    s.spotlightRadius = (std::max)(Wh_GetIntSetting(L"spotlightRadius"), 0);
    s.idleDelay = (std::max)(Wh_GetIntSetting(L"idleDelay"), 0);
    s.spotlightFadeOutDuration =
        (std::max)(Wh_GetIntSetting(L"spotlightFadeOutDuration"), 0);
    s.hideLabels = Wh_GetIntSetting(L"hideLabels") != 0;
    s.toggleOnDoubleClick = Wh_GetIntSetting(L"toggleOnDoubleClick") != 0;

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = s;
    }

    HWND hListView = g_listViewWnd.load();
    if (hListView && IsWindow(hListView)) {
        RedrawWindow(hListView, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ALLCHILDREN);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    g_msgKillFadeTimer =
        RegisterWindowMessageW(L"FlashlightDesktopIcons_KillFadeTimer");
    g_msgDragPosition =
        RegisterWindowMessageW(L"FlashlightDesktopIcons_DragPosition");

    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    HWND hListView = FindDesktopListView();
    if (hListView) {
        SubclassDesktop(hListView);
    } else {
        // Normal at boot: the desktop doesn't exist yet; the CreateWindowExW
        // hook subclasses it as soon as it's created.
        Wh_Log(L"No desktop list view yet, waiting for it to be created");
    }
}

void Wh_ModBeforeUninit() {
    // Called before Windhawk removes the hooks: stop the drop target hooks
    // from posting new messages, and make sure no new hooks are registered
    // from now on.
    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    // Already set in Wh_ModBeforeUninit; kept here so that the fade timer
    // can't be re-armed by a message processed before the subclass is
    // removed, even if this ever runs without the BeforeUninit step.
    g_unloading = true;

    HWND hListView = g_listViewWnd.load();
    HWND hShellView = g_shellViewWnd.load();
    g_listViewWnd = nullptr;
    g_shellViewWnd = nullptr;

    if (hListView && IsWindow(hListView)) {
        // Kill the fade timer on the thread that owns it, while the subclass
        // that handles the message is still installed.
        if (g_msgKillFadeTimer) {
            SendMessageW(hListView, g_msgKillFadeTimer, 0, 0);
        }
        // Removal is synchronous to the window's owning thread, so the
        // subclass proc is guaranteed not to be running once this returns.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hListView,
                                                         ListViewSubclassProc);
    }

    if (hShellView && IsWindow(hShellView)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hShellView,
                                                         ShellViewSubclassProc);
    }

    if (g_hTheme) {
        CloseThemeData(g_hTheme);
        g_hTheme = nullptr;
    }

    g_lastItemAlphas.clear();

    // Repaint so that the icons return to Explorer's default drawing.
    if (hListView && IsWindow(hListView)) {
        RedrawWindow(hListView, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
}
