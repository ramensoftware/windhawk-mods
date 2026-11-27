// ==WindhawkMod==
// @id              clean-desktop-fade
// @name            Clean Desktop & Cursor Fade
// @description     Auto-hide desktop icons and mouse cursor on inactivity with smooth fade animations and double-click toggle.
// @version         1.0.3
// @author          Marcelo
// @github          https://github.com/mrcelov
// @homepage        https://github.com/mrcelov/clean-desktop-fade
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lgdi32 -luxtheme -lwinmm -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Clean Desktop & Cursor Fade

Combines desktop icon management and cursor auto-hiding into a single lightweight module:
- **Desktop Icons:** Double-click an empty area on the desktop to toggle icons, or let them fade out automatically after inactivity. Moving the mouse or clicking restores them smoothly.
- **Mouse Cursor:** Automatically fades out the system cursor after a configurable idle timeout and restores it instantly upon any movement or click.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableIconAutoHide: true
  $name: Enable Desktop Icons Auto-Hide
- iconAutoHideDelay: 5
  $name: Icons Auto-Hide Delay (seconds)
- showIconsOnAnyInput: true
  $name: Restore Icons on Any Input
- iconFadeDuration: 250
  $name: Icons Fade Duration (ms)
- enableCursorAutoHide: true
  $name: Enable Cursor Auto-Hide
- cursorIdleTimeout: 10
  $name: Cursor Auto-Hide Delay (seconds)
- cursorFadeDuration: 600
  $name: Cursor Fade Duration (ms)
- cursorMinOpacity: 0
  $name: Cursor Minimum Opacity (%)
- excludedPrograms:
  - ""
  $name: Excluded Programs for Cursor Fade (e.g. game.exe)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <mmsystem.h>
#include <stdio.h>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <string>
#include <vector>
#include <windhawk_utils.h>

// ============================================================================
// SHARED SETTINGS
// ============================================================================
struct GlobalSettings {
    // Desktop Icons
    std::atomic<bool> enableIconAutoHide{true};
    std::atomic<int>  iconAutoHideDelay{5};
    std::atomic<bool> showIconsOnAnyInput{true};
    std::atomic<int>  iconFadeDuration{250};

    // Cursor
    std::atomic<bool> enableCursorAutoHide{true};
    std::atomic<int>  cursorIdleSeconds{10};
    std::atomic<int>  cursorFadeMs{600};
    std::atomic<int>  cursorMinOpacity{0};
} g_settings;

static void LoadGlobalSettings() {
    g_settings.enableIconAutoHide   = Wh_GetIntSetting(L"enableIconAutoHide") != 0;
    g_settings.iconAutoHideDelay    = std::clamp((int)Wh_GetIntSetting(L"iconAutoHideDelay"), 1, 60);
    g_settings.showIconsOnAnyInput  = Wh_GetIntSetting(L"showIconsOnAnyInput") != 0;
    g_settings.iconFadeDuration     = std::clamp((int)Wh_GetIntSetting(L"iconFadeDuration"), 50, 1000);

    g_settings.enableCursorAutoHide = Wh_GetIntSetting(L"enableCursorAutoHide") != 0;
    g_settings.cursorIdleSeconds    = std::max(1, (int)Wh_GetIntSetting(L"cursorIdleTimeout"));
    g_settings.cursorFadeMs         = std::max(0, (int)Wh_GetIntSetting(L"cursorFadeDuration"));
    g_settings.cursorMinOpacity     = std::clamp((int)Wh_GetIntSetting(L"cursorMinOpacity"), 0, 100);
}

// ============================================================================
// MODULE 1: DESKTOP ICONS FADE & TOGGLE
// ============================================================================
namespace DesktopModule {

constexpr UINT_PTR TIMER_ANIMATION     = 1002;
constexpr UINT     IDLE_POLL_MS        = 150;
constexpr UINT     ANIM_FRAME_MS       = 8;

static UINT g_msgManualToggle = 0;
static UINT g_msgAutoHide     = 0;
static UINT g_msgAutoRestore  = 0;

LRESULT CALLBACK ListViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);
LRESULT CALLBACK ShellViewSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t Real_CreateWindowExW = nullptr;

struct DesktopController {
    BYTE alpha = 255;
    BYTE animFrom = 255;
    BYTE animTo = 255;
    LONGLONG animStart = 0;
    float animDurationMs = 0.0f;

    std::atomic<bool> isVisible{true};
    std::atomic<bool> isAutoHidden{false};
    std::atomic<bool> manuallyHidden{false};

    std::atomic<HWND> activeShell{nullptr};
    std::atomic<ULONGLONG> lastActivityTick{0};
    POINT lastMousePos = {0, 0};
} g_ctrl;

static std::atomic<bool> g_unloading{false};
static HANDLE g_monitorThread = nullptr;
static HANDLE g_stopEvent = nullptr;

static thread_local int tl_paintAlpha = -1;
static thread_local int tl_drawDepth = 0;

using GdiAlphaBlend_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
static GdiAlphaBlend_t g_GdiAlphaBlend = nullptr;
using ExtTextOutW_t = decltype(&ExtTextOutW);
static ExtTextOutW_t g_ExtTextOutW = nullptr;
using DrawThemeBackground_t = decltype(&DrawThemeBackground);
static DrawThemeBackground_t g_DrawThemeBackground = nullptr;
using DrawShadowText_t = int(WINAPI*)(HDC, LPCWSTR, UINT, RECT*, DWORD, COLORREF, COLORREF, int, int);
static DrawShadowText_t g_DrawShadowText = nullptr;
using ImageList_DrawIndirect_t = BOOL(WINAPI*)(IMAGELISTDRAWPARAMS*);
static ImageList_DrawIndirect_t g_ImageList_DrawIndirect = nullptr;

struct OffscreenDC {
    HDC dc; HBITMAP bmp; HBITMAP oldBmp; int w; int h;
} g_offscreen{};

static void FreeOffscreen() {
    if (g_offscreen.dc) {
        if (g_offscreen.oldBmp) SelectObject(g_offscreen.dc, g_offscreen.oldBmp);
        if (g_offscreen.bmp) DeleteObject(g_offscreen.bmp);
        DeleteDC(g_offscreen.dc);
    }
    g_offscreen = {};
}

static HDC GetOffscreen(HDC ref, int w, int h) {
    if (!g_offscreen.dc) {
        g_offscreen.dc = CreateCompatibleDC(ref);
        if (!g_offscreen.dc) return nullptr;
    }
    if (!g_offscreen.bmp || w > g_offscreen.w || h > g_offscreen.h) {
        int newW = w > g_offscreen.w ? w : g_offscreen.w;
        int newH = h > g_offscreen.h ? h : g_offscreen.h;
        HBITMAP bmp = CreateCompatibleBitmap(ref, newW < 256 ? 256 : newW, newH < 128 ? 128 : newH);
        if (!bmp) return nullptr;
        HBITMAP old = (HBITMAP)SelectObject(g_offscreen.dc, bmp);
        if (g_offscreen.bmp) DeleteObject(g_offscreen.bmp);
        else g_offscreen.oldBmp = old;
        g_offscreen.bmp = bmp;
        g_offscreen.w = newW < 256 ? 256 : newW;
        g_offscreen.h = newH < 128 ? 128 : newH;
    }
    return g_offscreen.dc;
}

static BOOL RealAlphaBlend(HDC dst, int x, int y, int w, int h, HDC src, int sx, int sy, int sw, int sh, BLENDFUNCTION bf) {
    return (g_GdiAlphaBlend ? g_GdiAlphaBlend : GdiAlphaBlend)(dst, x, y, w, h, src, sx, sy, sw, sh, bf);
}

static bool ShouldIntercept() { return tl_paintAlpha >= 0 && tl_drawDepth == 0 && !g_unloading; }

template <typename Draw>
static bool DrawWithOpacity(HDC hdc, RECT rc, int alpha, Draw&& draw) {
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0 || w > 4096 || h > 4096) return false;
    HDC mem = GetOffscreen(hdc, w, h);
    if (!mem) return false;
    BitBlt(mem, 0, 0, w, h, hdc, rc.left, rc.top, SRCCOPY);
    POINT oldOrg{}; SetViewportOrgEx(mem, -rc.left, -rc.top, &oldOrg);
    HGDIOBJ oldFont = SelectObject(mem, GetCurrentObject(hdc, OBJ_FONT));
    COLORREF oldText = SetTextColor(mem, GetTextColor(hdc));
    COLORREF oldBk = SetBkColor(mem, GetBkColor(hdc));
    int oldBkMode = SetBkMode(mem, GetBkMode(hdc));
    UINT oldAlign = SetTextAlign(mem, GetTextAlign(hdc));

    ++tl_drawDepth; draw(mem); --tl_drawDepth;

    SetTextAlign(mem, oldAlign); SetBkMode(mem, oldBkMode);
    SetBkColor(mem, oldBk); SetTextColor(mem, oldText);
    SelectObject(mem, oldFont); SetViewportOrgEx(mem, oldOrg.x, oldOrg.y, nullptr);
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)alpha, 0 };
    RealAlphaBlend(hdc, rc.left, rc.top, w, h, mem, 0, 0, w, h, bf);
    return true;
}

static BOOL WINAPI HookGdiAlphaBlend(HDC dst, int x, int y, int w, int h, HDC src, int sx, int sy, int sw, int sh, BLENDFUNCTION bf) {
    if (ShouldIntercept()) {
        if (tl_paintAlpha <= 0) return TRUE;
        bf.SourceConstantAlpha = (BYTE)((bf.SourceConstantAlpha * tl_paintAlpha + 127) / 255);
    }
    return g_GdiAlphaBlend(dst, x, y, w, h, src, sx, sy, sw, sh, bf);
}

static BOOL WINAPI HookImageList_DrawIndirect(IMAGELISTDRAWPARAMS* p) {
    if (!p || !ShouldIntercept()) return g_ImageList_DrawIndirect(p);
    if (tl_paintAlpha <= 0) return TRUE;
    if (p->cbSize < sizeof(IMAGELISTDRAWPARAMS)) return g_ImageList_DrawIndirect(p);
    IMAGELISTDRAWPARAMS params = *p;
    DWORD baseAlpha = (params.fState & ILS_ALPHA) ? params.Frame : 255;
    params.fState |= ILS_ALPHA;
    params.Frame = (baseAlpha * (DWORD)tl_paintAlpha + 127) / 255;
    ++tl_drawDepth; BOOL res = g_ImageList_DrawIndirect(&params); --tl_drawDepth;
    return res;
}

static int WINAPI HookDrawShadowText(HDC hdc, LPCWSTR text, UINT cch, RECT* prc, DWORD flags, COLORREF crText, COLORREF crShadow, int ox, int oy) {
    if (!ShouldIntercept() || !prc || (flags & DT_CALCRECT)) return g_DrawShadowText(hdc, text, cch, prc, flags, crText, crShadow, ox, oy);
    if (tl_paintAlpha <= 0) return 1;
    int padX = (ox < 0 ? -ox : ox) + 4, padY = (oy < 0 ? -oy : oy) + 4;
    RECT rc{prc->left - padX, prc->top - padY, prc->right + padX, prc->bottom + padY};
    int result = 0;
    if (DrawWithOpacity(hdc, rc, tl_paintAlpha, [&](HDC mem) {
        result = g_DrawShadowText(mem, text, cch, prc, flags, crText, crShadow, ox, oy);
    })) return result;
    return g_DrawShadowText(hdc, text, cch, prc, flags, crText, crShadow, ox, oy);
}

static BOOL WINAPI HookExtTextOutW(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR str, UINT c, const INT* dx) {
    if (!ShouldIntercept()) return g_ExtTextOutW(hdc, x, y, options, lprect, str, c, dx);
    if (tl_paintAlpha <= 0) return TRUE;
    UINT align = GetTextAlign(hdc);
    if (align & TA_UPDATECP) return g_ExtTextOutW(hdc, x, y, options, lprect, str, c, dx);
    SIZE sz{};
    if (str && c) {
        if (options & ETO_GLYPH_INDEX) GetTextExtentPointI(hdc, (LPWORD)str, c, &sz);
        else GetTextExtentPoint32W(hdc, str, c, &sz);
    }
    if (dx && c) {
        UINT step = (options & ETO_PDY) ? 2 : 1; LONG sum = 0;
        for (UINT i = 0; i < c; ++i) sum += dx[i * step];
        if (sum > sz.cx) sz.cx = sum;
    }
    TEXTMETRICW tm{}; GetTextMetricsW(hdc, &tm);
    if (tm.tmHeight > sz.cy) sz.cy = tm.tmHeight;
    int left = x; if ((align & TA_CENTER) == TA_CENTER) left = x - sz.cx / 2; else if (align & TA_RIGHT) left = x - sz.cx;
    int top = y; if ((align & TA_BASELINE) == TA_BASELINE) top = y - tm.tmAscent; else if (align & TA_BOTTOM) top = y - sz.cy;
    RECT rc{left - 3, top - 3, left + sz.cx + tm.tmOverhang + tm.tmMaxCharWidth / 4 + 3, top + sz.cy + 3};
    if (lprect) {
        if (options & ETO_OPAQUE) UnionRect(&rc, &rc, lprect);
        if (options & ETO_CLIPPED) IntersectRect(&rc, &rc, lprect);
    }
    BOOL res = TRUE;
    if (DrawWithOpacity(hdc, rc, tl_paintAlpha, [&](HDC mem) {
        res = g_ExtTextOutW(mem, x, y, options, lprect, str, c, dx);
    })) return res;
    return g_ExtTextOutW(hdc, x, y, options, lprect, str, c, dx);
}

static HRESULT WINAPI HookDrawThemeBackground(HTHEME theme, HDC hdc, int part, int stateId, LPCRECT pRect, LPCRECT pClip) {
    if (!ShouldIntercept() || !pRect) return g_DrawThemeBackground(theme, hdc, part, stateId, pRect, pClip);
    if (tl_paintAlpha <= 0) return S_OK;
    RECT rc = *pRect; if (pClip) IntersectRect(&rc, &rc, pClip);
    HRESULT res = S_OK;
    if (DrawWithOpacity(hdc, rc, tl_paintAlpha, [&](HDC mem) {
        res = g_DrawThemeBackground(theme, mem, part, stateId, pRect, pClip);
    })) return res;
    return g_DrawThemeBackground(theme, hdc, part, stateId, pRect, pClip);
}

static FARPROC GetGdiProc(const char* name) {
    const wchar_t* kModules[] = {L"gdi32full.dll", L"gdi32.dll"};
    for (const wchar_t* mod : kModules) {
        if (HMODULE gdi = GetModuleHandleW(mod))
            if (FARPROC proc = GetProcAddress(gdi, name)) return proc;
    }
    return nullptr;
}

static bool InstallGdiHooks() {
    auto alphaBlend = (GdiAlphaBlend_t)GetGdiProc("GdiAlphaBlend");
    auto extTextOut = (ExtTextOutW_t)GetGdiProc("ExtTextOutW");
    if (!extTextOut) extTextOut = ExtTextOutW;
    bool ok = true;
    if (!alphaBlend || !WindhawkUtils::SetFunctionHook(alphaBlend, HookGdiAlphaBlend, &g_GdiAlphaBlend)) ok = false;
    if (!WindhawkUtils::SetFunctionHook(extTextOut, HookExtTextOutW, &g_ExtTextOutW)) ok = false;
    if (!WindhawkUtils::SetFunctionHook(DrawThemeBackground, HookDrawThemeBackground, &g_DrawThemeBackground)) ok = false;
    return ok;
}

static void HookComctl32For(HWND list) {
    static bool hooked = false;
    if (hooked || !list) return;
    HMODULE comctl = (HMODULE)GetClassLongPtrW(list, GCLP_HMODULE);
    if (!comctl) return;
    auto drawInd = (ImageList_DrawIndirect_t)GetProcAddress(comctl, "ImageList_DrawIndirect");
    auto drawShad = (DrawShadowText_t)GetProcAddress(comctl, "DrawShadowText");
    if (drawInd) WindhawkUtils::SetFunctionHook(drawInd, HookImageList_DrawIndirect, &g_ImageList_DrawIndirect);
    if (drawShad) WindhawkUtils::SetFunctionHook(drawShad, HookDrawShadowText, &g_DrawShadowText);
    hooked = true;
    Wh_ApplyHookOperations();
}

static void RepaintList(HWND list) {
    if (list && IsWindow(list)) {
        InvalidateRect(list, nullptr, FALSE);
        UpdateWindow(list);
    }
}

static LONGLONG QpcNow() { LARGE_INTEGER v{}; QueryPerformanceCounter(&v); return v.QuadPart; }
static double QpcToMs(LONGLONG ticks) {
    static LONGLONG freq = 0;
    if (!freq) { LARGE_INTEGER f{}; QueryPerformanceFrequency(&f); freq = f.QuadPart ? f.QuadPart : 1; }
    return (double)ticks * 1000.0 / (double)freq;
}
static BYTE EaseAlpha(BYTE from, BYTE to, float t) {
    if (t <= 0.0f) return from; if (t >= 1.0f) return to;
    float eased = t * t * (3.0f - 2.0f * t);
    float val = (float)from + ((float)to - (float)from) * eased;
    return (BYTE)(val < 0.0f ? 0.0f : (val > 255.0f ? 255.0f : val + 0.5f));
}

static void SetAlpha(HWND list, BYTE alpha) {
    if (g_ctrl.alpha != alpha) {
        g_ctrl.alpha = alpha;
        RepaintList(list);
    }
}

static void TickAnimation(HWND shell, HWND list) {
    double elapsed = QpcToMs(QpcNow() - g_ctrl.animStart);
    float progress = g_ctrl.animDurationMs > 0.0f ? (float)(elapsed / g_ctrl.animDurationMs) : 1.0f;
    if (progress >= 1.0f) {
        KillTimer(shell, TIMER_ANIMATION);
        SetAlpha(list, g_ctrl.animTo);
    } else {
        SetAlpha(list, EaseAlpha(g_ctrl.animFrom, g_ctrl.animTo, progress));
    }
}

static void AnimateTo(HWND shell, HWND list, BYTE targetAlpha) {
    if (!shell || !list) return;
    KillTimer(shell, TIMER_ANIMATION);
    if (targetAlpha == g_ctrl.alpha) {
        g_ctrl.animTo = targetAlpha;
        SetAlpha(list, targetAlpha);
        return;
    }
    int delta = targetAlpha > g_ctrl.alpha ? targetAlpha - g_ctrl.alpha : g_ctrl.alpha - targetAlpha;
    g_ctrl.animFrom = g_ctrl.alpha;
    g_ctrl.animTo = targetAlpha;
    g_ctrl.animStart = QpcNow();
    g_ctrl.animDurationMs = (float)g_settings.iconFadeDuration.load() * (float)delta / 255.0f;
    if (g_ctrl.animDurationMs < 40.0f) g_ctrl.animDurationMs = 40.0f;

    g_ctrl.isVisible = (targetAlpha > 0);
    SetTimer(shell, TIMER_ANIMATION, ANIM_FRAME_MS, NULL);
}

static void ResetIdleTracking() {
    g_ctrl.lastActivityTick = GetTickCount64();
    GetCursorPos(&g_ctrl.lastMousePos);
}

static void ManualToggleIcons(HWND hwndShellView) {
    HWND hwndListView = FindWindowExW(hwndShellView, NULL, L"SysListView32", NULL);
    if (!hwndListView) return;

    if (g_ctrl.isVisible.load()) {
        g_ctrl.manuallyHidden = true;
        g_ctrl.isAutoHidden = false;
        AnimateTo(hwndShellView, hwndListView, 0);
    } else {
        g_ctrl.manuallyHidden = false;
        g_ctrl.isAutoHidden = false;
        ResetIdleTracking();
        AnimateTo(hwndShellView, hwndListView, 255);
    }
}

static DWORD WINAPI IdleMonitorThread(LPVOID) {
    ResetIdleTracking();

    while (WaitForSingleObject(g_stopEvent, IDLE_POLL_MS) == WAIT_TIMEOUT) {
        if (g_unloading.load()) break;

        HWND shell = g_ctrl.activeShell.load();
        if (!shell || !IsWindow(shell)) continue;

        if (!g_settings.enableIconAutoHide.load()) {
            if (g_ctrl.isAutoHidden.load()) {
                PostMessageW(shell, g_msgAutoRestore, 0, 0);
            }
            ResetIdleTracking();
            continue;
        }

        bool active = false;
        POINT pt = {};
        if (GetCursorPos(&pt)) {
            if (pt.x != g_ctrl.lastMousePos.x || pt.y != g_ctrl.lastMousePos.y) {
                g_ctrl.lastMousePos = pt;
                active = true;
            }
        }

        for (int vk = 1; vk < 255; ++vk) {
            if (vk == VK_PACKET || vk == 0xFF) continue;
            if (GetAsyncKeyState(vk) & 0x8000) {
                active = true;
                break;
            }
        }

        ULONGLONG now = GetTickCount64();
        if (active) {
            g_ctrl.lastActivityTick = now;
        }

        if (g_ctrl.isVisible.load()) {
            ULONGLONG idleMs = now - g_ctrl.lastActivityTick.load();
            ULONGLONG thresholdMs = (ULONGLONG)g_settings.iconAutoHideDelay.load() * 1000ULL;
            if (idleMs >= thresholdMs) {
                PostMessageW(shell, g_msgAutoHide, 0, 0);
            }
        } else if (g_ctrl.isAutoHidden.load() && !g_ctrl.manuallyHidden.load() && g_settings.showIconsOnAnyInput.load()) {
            if (active) {
                PostMessageW(shell, g_msgAutoRestore, 0, 0);
            }
        }
    }
    return 0;
}

LRESULT CALLBACK ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (uMsg == WM_NCHITTEST) {
        if (!g_ctrl.isVisible.load() && !g_unloading) return HTTRANSPARENT;
    }

    if (uMsg == WM_PAINT || uMsg == WM_PRINTCLIENT) {
        if (g_ctrl.alpha < 255 && !g_unloading) {
            int prevAlpha = tl_paintAlpha;
            tl_paintAlpha = g_ctrl.alpha;
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            tl_paintAlpha = prevAlpha;
            return result;
        }
    }

    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONDOWN) {
        bool hasDblClkStyle = (GetClassLongPtrW(hWnd, GCL_STYLE) & CS_DBLCLKS) != 0;
        bool isDblClick = false;
        if (hasDblClkStyle) {
            isDblClick = (uMsg == WM_LBUTTONDBLCLK);
        } else {
            if (uMsg == WM_LBUTTONDBLCLK) isDblClick = true;
            else if (uMsg == WM_LBUTTONDOWN) {
                static DWORD lastClickTime = 0; static POINT lastClickPt = {};
                DWORD now = GetTickCount(); DWORD dblClickTime = GetDoubleClickTime();
                POINT pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
                if (now - lastClickTime <= dblClickTime &&
                    abs(pt.x - lastClickPt.x) <= GetSystemMetrics(SM_CXDOUBLECLK) / 2 &&
                    abs(pt.y - lastClickPt.y) <= GetSystemMetrics(SM_CYDOUBLECLK) / 2) {
                    isDblClick = true;
                } else {
                    lastClickTime = now;
                    lastClickPt = pt;
                }
            }
        }
        if (isDblClick) {
            LVHITTESTINFO ht = {};
            ht.pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
            SendMessageW(hWnd, LVM_HITTEST, 0, (LPARAM)&ht);
            if (ht.iItem == -1) {
                HWND hwndParent = GetParent(hWnd);
                if (hwndParent) PostMessageW(hwndParent, g_msgManualToggle, 0, 0);
                return 0;
            }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ShellViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (uMsg == g_msgManualToggle) {
        ManualToggleIcons(hWnd);
        return 0;
    }

    if (uMsg == g_msgAutoHide) {
        HWND lv = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (lv && g_ctrl.isVisible.load()) {
            HookComctl32For(lv);
            g_ctrl.isAutoHidden = true;
            g_ctrl.manuallyHidden = false;
            AnimateTo(hWnd, lv, 0);
        }
        return 0;
    }

    if (uMsg == g_msgAutoRestore) {
        HWND lv = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        if (lv && !g_ctrl.isVisible.load()) {
            g_ctrl.isAutoHidden = false;
            ResetIdleTracking();
            AnimateTo(hWnd, lv, 255);
        }
        return 0;
    }

    if (uMsg == WM_TIMER && wParam == TIMER_ANIMATION) {
        HWND lv = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
        TickAnimation(hWnd, lv);
        return 0;
    }

    if (g_ctrl.isAutoHidden.load() && !g_ctrl.manuallyHidden.load() && g_settings.showIconsOnAnyInput.load()) {
        if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MOUSEMOVE) {
            HWND lv = FindWindowExW(hWnd, NULL, L"SysListView32", NULL);
            if (lv) {
                g_ctrl.isAutoHidden = false;
                ResetIdleTracking();
                AnimateTo(hWnd, lv, 255);
            }
        }
    }

    if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONDOWN) {
        bool hasDblClkStyle = (GetClassLongPtrW(hWnd, GCL_STYLE) & CS_DBLCLKS) != 0;
        bool isDblClick = false;
        if (hasDblClkStyle) isDblClick = (uMsg == WM_LBUTTONDBLCLK);
        else {
            if (uMsg == WM_LBUTTONDBLCLK) isDblClick = true;
            else if (uMsg == WM_LBUTTONDOWN) {
                static DWORD lastClickTime = 0; static POINT lastClickPt = {};
                DWORD now = GetTickCount(); DWORD dblClickTime = GetDoubleClickTime();
                POINT pt = { (short)GET_X_LPARAM(lParam), (short)GET_Y_LPARAM(lParam) };
                if (now - lastClickTime <= dblClickTime &&
                    abs(pt.x - lastClickPt.x) <= GetSystemMetrics(SM_CXDOUBLECLK) / 2 &&
                    abs(pt.y - lastClickPt.y) <= GetSystemMetrics(SM_CYDOUBLECLK) / 2) {
                    isDblClick = true;
                } else {
                    lastClickTime = now;
                    lastClickPt = pt;
                }
            }
        }
        if (isDblClick) {
            PostMessageW(hWnd, g_msgManualToggle, 0, 0);
            return 0;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM) {
    WCHAR className[256] = {};
    if (!GetClassNameW(hWnd, className, 256)) return TRUE;
    if (wcscmp(className, L"WorkerW") == 0 || wcscmp(className, L"Progman") == 0) {
        HWND hwndShell = FindWindowExW(hWnd, NULL, L"SHELLDLL_DefView", NULL);
        if (hwndShell) {
            g_ctrl.activeShell = hwndShell;
            WindhawkUtils::SetWindowSubclassFromAnyThread(hwndShell, ShellViewSubclassProc, 0);
            HWND hwndListView = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
            if (hwndListView) {
                WindhawkUtils::SetWindowSubclassFromAnyThread(hwndListView, ListViewSubclassProc, 0);
                HookComctl32For(hwndListView);
            }
        }
    }
    return TRUE;
}

HWND WINAPI Hook_CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = Real_CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && lpClassName && !IS_INTRESOURCE(lpClassName)) {
        if (wcscmp(lpClassName, L"SHELLDLL_DefView") == 0) {
            WCHAR parentClass[256] = {};
            if (hWndParent && GetClassNameW(hWndParent, parentClass, 256) &&
                (wcscmp(parentClass, L"Progman") == 0 || wcscmp(parentClass, L"WorkerW") == 0)) {
                g_ctrl.activeShell = hWnd;
                WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ShellViewSubclassProc, 0);
            }
        } else if (wcscmp(lpClassName, L"SysListView32") == 0) {
            WCHAR parentClass[256] = {};
            if (hWndParent && GetClassNameW(hWndParent, parentClass, 256) && wcscmp(parentClass, L"SHELLDLL_DefView") == 0) {
                WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ListViewSubclassProc, 0);
                HookComctl32For(hWnd);
            }
        }
    }
    return hWnd;
}

BOOL Init() {
    g_msgManualToggle = RegisterWindowMessageW(L"Windhawk.CleanDesktopFade.Toggle");
    g_msgAutoHide     = RegisterWindowMessageW(L"Windhawk.CleanDesktopFade.AutoHide");
    g_msgAutoRestore  = RegisterWindowMessageW(L"Windhawk.CleanDesktopFade.AutoRestore");

    if (!InstallGdiHooks()) return FALSE;
    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)Hook_CreateWindowExW, (void**)&Real_CreateWindowExW)) return FALSE;

    EnumWindows(EnumWindowsProc, 0);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent) {
        g_monitorThread = CreateThread(nullptr, 0, IdleMonitorThread, nullptr, 0, nullptr);
    }
    return TRUE;
}

void Uninit() {
    g_unloading = true;
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
        if (g_monitorThread) {
            WaitForSingleObject(g_monitorThread, INFINITE);
            CloseHandle(g_monitorThread);
            g_monitorThread = nullptr;
        }
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    auto Cleanup = [](HWND hwndShell) {
        if (!hwndShell) return;
        KillTimer(hwndShell, TIMER_ANIMATION);

        HWND lv = FindWindowExW(hwndShell, NULL, L"SysListView32", NULL);
        if (lv) {
            InvalidateRect(lv, nullptr, TRUE);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(lv, ListViewSubclassProc);
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwndShell, ShellViewSubclassProc);
    };

    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman) Cleanup(FindWindowExW(hwndProgman, NULL, L"SHELLDLL_DefView", NULL));

    HWND hwndWorkerW = NULL;
    while ((hwndWorkerW = FindWindowExW(NULL, hwndWorkerW, L"WorkerW", NULL)) != NULL)
        Cleanup(FindWindowExW(hwndWorkerW, NULL, L"SHELLDLL_DefView", NULL));

    FreeOffscreen();
}

void SettingsChanged() {
    ResetIdleTracking();
}

} // namespace DesktopModule

// ============================================================================
// MODULE 2: CURSOR IDLE FADE (Runs directly inside explorer.exe user session)
// ============================================================================
namespace CursorModule {

static const DWORD kCursorIds[] = {
    32512, 32513, 32515, 32516, 32642, 32643, 32644, 32645,
    32646, 32648, 32649, 32651, 32631, 32671, 32672
};

struct CursorSnap {
    DWORD id = 0;
    bool valid = false;
    HCURSOR orig = nullptr;
    bool mono = false;
    bool monoBlanked = false;
    int w = 0, h = 0;
    DWORD xHot = 0, yHot = 0;
    std::vector<uint32_t> px;
};

static std::vector<CursorSnap> g_snaps;
static int g_lastFactor = -1;

static BITMAPINFO MakeBitmapInfo(int w, int h) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    return bi;
}

static bool CaptureCursor(DWORD id, CursorSnap& s) {
    HCURSOR cur = LoadCursorW(nullptr, MAKEINTRESOURCEW(id));
    if (!cur) return false;

    ICONINFO ii{};
    if (!GetIconInfo(cur, &ii)) return false;

    bool ok = false;
    if (ii.hbmColor && ii.hbmMask) {
        BITMAP bm{};
        if (GetObjectW(ii.hbmColor, sizeof(bm), &bm) && bm.bmWidth > 0 && bm.bmHeight > 0) {
            int w = bm.bmWidth, h = bm.bmHeight;
            s.px.assign((size_t)w * h, 0);
            std::vector<uint32_t> mask((size_t)w * h, 0);

            HDC dc = GetDC(nullptr);
            BITMAPINFO bi1 = MakeBitmapInfo(w, h);
            BITMAPINFO bi2 = MakeBitmapInfo(w, h);
            int r1 = GetDIBits(dc, ii.hbmColor, 0, h, s.px.data(), &bi1, DIB_RGB_COLORS);
            int r2 = GetDIBits(dc, ii.hbmMask, 0, h, mask.data(), &bi2, DIB_RGB_COLORS);
            ReleaseDC(nullptr, dc);

            if (r1 == h && r2 == h) {
                bool anyAlpha = false;
                for (uint32_t p : s.px) {
                    if (p >> 24) { anyAlpha = true; break; }
                }
                if (!anyAlpha) {
                    for (size_t i = 0; i < s.px.size(); i++) {
                        s.px[i] = ((mask[i] & 0xFF) == 0) ? (s.px[i] | 0xFF000000u) : 0;
                    }
                }
                s.id = id;
                s.w = w; s.h = h;
                s.xHot = ii.xHotspot; s.yHot = ii.yHotspot;
                s.orig = CopyCursor(cur);
                ok = true;
            }
        }
    } else if (!ii.hbmColor && ii.hbmMask) {
        BITMAP bm{};
        if (GetObjectW(ii.hbmMask, sizeof(bm), &bm) && bm.bmWidth > 0 && bm.bmHeight >= 2) {
            s.mono = true;
            s.id = id;
            s.w = bm.bmWidth; s.h = bm.bmHeight / 2;
            s.xHot = ii.xHotspot; s.yHot = ii.yHotspot;
            s.orig = CopyCursor(cur);
            ok = true;
        }
    }

    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return ok;
}

static HCURSOR BuildCursor(const CursorSnap& s, int factor256) {
    BITMAPINFO bi = MakeBitmapInfo(s.w, s.h);
    void* bits = nullptr;
    HBITMAP color = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!color || !bits) {
        if (color) DeleteObject(color);
        return nullptr;
    }

    uint32_t* dst = (uint32_t*)bits;
    for (size_t i = 0; i < s.px.size(); i++) {
        uint32_t p = s.px[i];
        uint32_t a = (((p >> 24) & 0xFF) * factor256) >> 8;
        dst[i] = (a == 0) ? 0 : ((a << 24) | (p & 0x00FFFFFFu));
    }

    int rowBytes = ((s.w + 15) / 16) * 2;
    std::vector<BYTE> maskBits((size_t)rowBytes * s.h, 0xFF);
    HBITMAP mask = CreateBitmap(s.w, s.h, 1, 1, maskBits.data());

    HCURSOR result = nullptr;
    if (mask) {
        ICONINFO ii{};
        ii.fIcon = FALSE;
        ii.xHotspot = s.xHot; ii.yHotspot = s.yHot;
        ii.hbmMask = mask; ii.hbmColor = color;
        result = (HCURSOR)CreateIconIndirect(&ii);
        DeleteObject(mask);
    }
    DeleteObject(color);
    return result;
}

static HCURSOR BuildBlankMonoCursor(const CursorSnap& s) {
    int rowBytes = ((s.w + 15) / 16) * 2;
    size_t half = (size_t)rowBytes * s.h;
    std::vector<BYTE> bits(half * 2, 0x00);
    std::fill(bits.begin(), bits.begin() + half, 0xFF);
    HBITMAP mask = CreateBitmap(s.w, s.h * 2, 1, 1, bits.data());
    if (!mask) return nullptr;

    ICONINFO ii{};
    ii.fIcon = FALSE;
    ii.xHotspot = s.xHot; ii.yHotspot = s.yHot;
    ii.hbmMask = mask; ii.hbmColor = nullptr;
    HCURSOR result = (HCURSOR)CreateIconIndirect(&ii);
    DeleteObject(mask);
    return result;
}

static void InstallCursor(HCURSOR c, DWORD id) {
    if (c && !SetSystemCursor(c, id)) DestroyCursor(c);
}

static void ApplyOpacity(double opacity) {
    int f = std::clamp((int)(opacity * 256.0 + 0.5), 0, 256);
    if (f == g_lastFactor) return;
    g_lastFactor = f;
    for (CursorSnap& s : g_snaps) {
        if (!s.valid) continue;
        if (s.mono) {
            bool shouldBlank = f < 128;
            if (shouldBlank && !s.monoBlanked) {
                InstallCursor(BuildBlankMonoCursor(s), s.id);
                s.monoBlanked = true;
            } else if (!shouldBlank && s.monoBlanked && s.orig) {
                InstallCursor(CopyCursor(s.orig), s.id);
                s.monoBlanked = false;
            }
            continue;
        }
        InstallCursor(BuildCursor(s, f), s.id);
    }
}

enum class State { Visible, Fading, Faded };
enum : UINT_PTR { TIMER_IDLE = 1, TIMER_FADE = 2, TIMER_WATCH = 3 };
enum : UINT { WM_APP_SETTINGS = WM_APP + 1 };

static const wchar_t kWindowClass[] = L"CleanDesktopCursorWorkerWnd";
static WCHAR g_modifiedFlag[64] = L"cleanDesktopCursorsModified";
static State g_state = State::Visible;
static ULONGLONG g_lastActivity = 0;
static ULONGLONG g_fadeStart = 0;
static POINT g_lastPt = {0, 0};

constexpr UINT kWatchIntervalMs       = 250;
constexpr UINT kWatchPolledIntervalMs = 50;
constexpr UINT kPolledIdleIntervalMs  = 200;

static ULONGLONG g_rawSeq = 0;
static ULONGLONG g_rawSeqAtLastPoll = 0;
static bool g_rawMissing = false;
static bool g_fgBlocked = false;
static bool g_fgExcluded = false;
static std::vector<std::wstring> g_excluded;
static HWINEVENTHOOK g_fgHook = nullptr;

static bool PolledMode() { return g_rawMissing || g_fgBlocked; }

static std::atomic<HWND> g_hwnd{nullptr};
static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_readyEvent = nullptr;
static bool g_workerOk = false;

static ULONGLONG IdleTimeoutMs() {
    return (ULONGLONG)g_settings.cursorIdleSeconds.load() * 1000ULL;
}

static void ArmIdleTimer(HWND hwnd, ULONGLONG ms) {
    if (!g_settings.enableCursorAutoHide.load()) {
        KillTimer(hwnd, TIMER_IDLE);
        return;
    }
    if (PolledMode()) ms = std::min<ULONGLONG>(ms, kPolledIdleIntervalMs);
    ms = std::clamp<ULONGLONG>(ms, USER_TIMER_MINIMUM, 0x7FFFFFFFULL);
    SetTimer(hwnd, TIMER_IDLE, (UINT)ms, nullptr);
}

static void RearmIdle(HWND hwnd) {
    ULONGLONG timeout = IdleTimeoutMs();
    ULONGLONG idle = GetTickCount64() - g_lastActivity;
    ArmIdleTimer(hwnd, idle >= timeout ? 1 : timeout - idle);
}

static void ArmWatchTimer(HWND hwnd) {
    SetTimer(hwnd, TIMER_WATCH, PolledMode() ? kWatchPolledIntervalMs : kWatchIntervalMs, nullptr);
}

static void LoadExcludedPrograms() {
    g_excluded.clear();
    for (int i = 0; i < 64; i++) {
        auto value = WindhawkUtils::StringSetting::make(L"excludedPrograms[%d]", i);
        if (!*value.get()) break;
        g_excluded.push_back(value.get());
    }
}

static void QueryForeground(bool* blocked, bool* excluded) {
    *blocked = false;
    *excluded = false;

    HWND fg = GetForegroundWindow();
    if (!fg) return;

    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    if (!pid || pid == GetCurrentProcessId()) return;

    HANDLE proc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!proc) {
        *blocked = true;
        return;
    }

    if (!g_excluded.empty()) {
        WCHAR path[MAX_PATH * 2];
        DWORD len = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(proc, 0, path, &len)) {
            const wchar_t* base = wcsrchr(path, L'\\');
            const wchar_t* name = base ? base + 1 : path;
            for (const std::wstring& e : g_excluded) {
                if (CompareStringOrdinal(e.c_str(), -1, name, -1, TRUE) == CSTR_EQUAL) {
                    *excluded = true;
                    break;
                }
            }
        }
    }

    HANDLE token = nullptr;
    if (OpenProcessToken(proc, TOKEN_QUERY, &token)) {
        CloseHandle(token);
    } else {
        *blocked = true;
    }
    CloseHandle(proc);
}

static void Restore(HWND hwnd);

static void ResetActivityTracking() {
    g_lastActivity = GetTickCount64();
    GetCursorPos(&g_lastPt);
    g_rawSeqAtLastPoll = g_rawSeq;
}

static void CALLBACK ForegroundChanged(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) {
    bool blocked, excluded;
    QueryForeground(&blocked, &excluded);
    bool changed = blocked != g_fgBlocked || excluded != g_fgExcluded;
    if (g_fgExcluded && !excluded) ResetActivityTracking();
    g_fgBlocked = blocked;
    g_fgExcluded = excluded;

    HWND hwnd = g_hwnd.load();
    if (!hwnd) return;

    if (excluded && g_state != State::Visible) {
        Restore(hwnd);
        return;
    }
    if (!changed) return;

    if (g_state == State::Visible) RearmIdle(hwnd);
    else if (g_state == State::Faded) ArmWatchTimer(hwnd);
}

static void FreeSnaps() {
    for (CursorSnap& s : g_snaps) {
        if (s.orig) DestroyCursor(s.orig);
    }
    g_snaps.clear();
}

static void RestoreCursorsNow() {
    for (CursorSnap& s : g_snaps) {
        if (!s.valid) continue;
        HCURSOR c = s.orig ? CopyCursor(s.orig) : nullptr;
        if (!c || !SetSystemCursor(c, s.id)) {
            if (c) DestroyCursor(c);
        }
    }
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
    Wh_SetIntValue(g_modifiedFlag, 0);
    FreeSnaps();
    g_lastFactor = -1;
    g_state = State::Visible;
}

static void OnActivity(HWND hwnd);

static bool PollMouse(bool* active) {
    POINT pt;
    if (!GetCursorPos(&pt)) return false;

    bool a = pt.x != g_lastPt.x || pt.y != g_lastPt.y;
    g_lastPt = pt;

    static const int keys[] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2};
    for (int k : keys) {
        if (GetAsyncKeyState(k) & 0x8000) a = true;
    }
    *active = a;
    return true;
}

static void Restore(HWND hwnd) {
    KillTimer(hwnd, TIMER_FADE);
    KillTimer(hwnd, TIMER_WATCH);
    RestoreCursorsNow();
    g_lastActivity = GetTickCount64();
    GetCursorPos(&g_lastPt);
    ArmIdleTimer(hwnd, IdleTimeoutMs());
}

static void StartFade(HWND hwnd) {
    if (!g_settings.enableCursorAutoHide.load()) return;

    FreeSnaps();
    bool any = false;
    for (DWORD id : kCursorIds) {
        CursorSnap s;
        s.valid = CaptureCursor(id, s);
        any = any || s.valid;
        g_snaps.push_back(std::move(s));
    }
    if (!any) {
        FreeSnaps();
        ArmIdleTimer(hwnd, IdleTimeoutMs());
        return;
    }

    Wh_SetIntValue(g_modifiedFlag, 1);

    g_lastFactor = -1;
    g_fadeStart = GetTickCount64();
    g_state = State::Fading;
    SetTimer(hwnd, TIMER_FADE, 15, nullptr);
}

static void OnFadeTick(HWND hwnd) {
    bool active = false;
    if (!PollMouse(&active) || !g_settings.enableCursorAutoHide.load()) {
        Restore(hwnd);
        return;
    }
    if (active) {
        OnActivity(hwnd);
        return;
    }

    int fadeMs = g_settings.cursorFadeMs.load();
    double t = 1.0;
    if (fadeMs > 0) {
        t = std::min(1.0, (double)(GetTickCount64() - g_fadeStart) / fadeMs);
    }
    double eased = t * t * (3.0 - 2.0 * t);
    double minOp = g_settings.cursorMinOpacity.load() / 100.0;
    ApplyOpacity(1.0 - eased * (1.0 - minOp));

    if (t >= 1.0) {
        KillTimer(hwnd, TIMER_FADE);
        g_state = State::Faded;
        ArmWatchTimer(hwnd);
    }
}

static void OnIdleTimer(HWND hwnd) {
    KillTimer(hwnd, TIMER_IDLE);
    if (g_state != State::Visible || !g_settings.enableCursorAutoHide.load()) return;

    ULONGLONG timeout = IdleTimeoutMs();

    if (g_fgExcluded) {
        SetTimer(hwnd, TIMER_IDLE, (UINT)std::min<ULONGLONG>(timeout, 0x7FFFFFFFULL), nullptr);
        return;
    }

    bool active = false;
    if (!PollMouse(&active)) {
        ArmIdleTimer(hwnd, timeout);
        return;
    }

    bool rawSeen = g_rawSeq != g_rawSeqAtLastPoll;
    g_rawSeqAtLastPoll = g_rawSeq;
    if (rawSeen) {
        g_rawMissing = false;
    } else if (active) {
        g_rawMissing = true;
        g_lastActivity = GetTickCount64();
    }

    ULONGLONG idle = GetTickCount64() - g_lastActivity;
    if (idle < timeout) {
        ArmIdleTimer(hwnd, timeout - idle);
        return;
    }

    StartFade(hwnd);
}

static void OnWatchTimer(HWND hwnd) {
    bool active = false;
    if (!PollMouse(&active) || !g_settings.enableCursorAutoHide.load()) {
        Restore(hwnd);
    } else if (active) {
        OnActivity(hwnd);
    }
}

static void OnActivity(HWND hwnd) {
    g_lastActivity = GetTickCount64();
    GetCursorPos(&g_lastPt);
    if (g_state != State::Visible) Restore(hwnd);
}

static void OnRawInput(HWND hwnd, HRAWINPUT hri) {
    g_rawSeq++;
    RAWINPUT ri{};
    UINT size = sizeof(ri);
    if (GetRawInputData(hri, RID_INPUT, &ri, &size, sizeof(RAWINPUTHEADER)) != (UINT)-1) {
        if (ri.header.dwType == RIM_TYPEMOUSE) {
            const RAWMOUSE& m = ri.data.mouse;
            bool empty = !(m.usFlags & MOUSE_MOVE_ABSOLUTE) && m.lLastX == 0 &&
                         m.lLastY == 0 && m.usButtonFlags == 0;
            if (empty) return;
        }
    }
    OnActivity(hwnd);
}

static void OnSettingsMessage(HWND hwnd) {
    bool wasExcluded = g_fgExcluded;
    LoadExcludedPrograms();
    QueryForeground(&g_fgBlocked, &g_fgExcluded);
    if (wasExcluded && !g_fgExcluded) ResetActivityTracking();

    if (!g_settings.enableCursorAutoHide.load() || (g_fgExcluded && g_state != State::Visible)) {
        if (g_state != State::Visible) Restore(hwnd);
        else KillTimer(hwnd, TIMER_IDLE);
        return;
    }

    switch (g_state) {
        case State::Visible:
            RearmIdle(hwnd);
            break;
        case State::Faded:
            g_lastFactor = -1;
            ApplyOpacity(g_settings.cursorMinOpacity.load() / 100.0);
            break;
        case State::Fading:
            break;
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INPUT:
            OnRawInput(hwnd, (HRAWINPUT)lParam);
            break;

        case WM_TIMER:
            switch (wParam) {
                case TIMER_IDLE:  OnIdleTimer(hwnd); break;
                case TIMER_FADE:  OnFadeTick(hwnd); break;
                case TIMER_WATCH: OnWatchTimer(hwnd); break;
            }
            return 0;

        case WM_APP_SETTINGS:
            OnSettingsMessage(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static DWORD WINAPI WorkerThread(LPVOID) {
    HINSTANCE inst = GetModuleHandleW(nullptr);

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.lpszClassName = kWindowClass;
    bool classRegistered = RegisterClassW(&wc) != 0;

    HWND hwnd = nullptr;
    bool rawInputRegistered = false;

    if (classRegistered) {
        hwnd = CreateWindowExW(0, kWindowClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, inst, nullptr);
    }
    if (hwnd) {
        RAWINPUTDEVICE rid{};
        rid.usUsagePage = 0x01;
        rid.usUsage = 0x02;
        rid.dwFlags = RIDEV_INPUTSINK;
        rid.hwndTarget = hwnd;
        rawInputRegistered = RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }

    if (!hwnd || !rawInputRegistered) {
        g_workerOk = false;
        if (hwnd) DestroyWindow(hwnd);
        if (classRegistered) UnregisterClassW(kWindowClass, inst);
        SetEvent(g_readyEvent);
        return 0;
    }

    g_lastActivity = GetTickCount64();
    GetCursorPos(&g_lastPt);
    LoadExcludedPrograms();
    QueryForeground(&g_fgBlocked, &g_fgExcluded);
    g_fgHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, ForegroundChanged, 0, 0,
                               WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    ArmIdleTimer(hwnd, IdleTimeoutMs());
    g_hwnd = hwnd;
    g_workerOk = true;
    SetEvent(g_readyEvent);

    for (;;) {
        DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT | QS_RAWINPUT);
        if (r != WAIT_OBJECT_0 + 1) break;

        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    g_hwnd = nullptr;
    if (g_fgHook) {
        UnhookWinEvent(g_fgHook);
        g_fgHook = nullptr;
    }

    RAWINPUTDEVICE rid{};
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_REMOVE;
    rid.hwndTarget = nullptr;
    RegisterRawInputDevices(&rid, 1, sizeof(rid));

    KillTimer(hwnd, TIMER_IDLE);
    KillTimer(hwnd, TIMER_FADE);
    KillTimer(hwnd, TIMER_WATCH);
    if (g_state != State::Visible) RestoreCursorsNow();
    FreeSnaps();

    DestroyWindow(hwnd);
    UnregisterClassW(kWindowClass, inst);
    return 0;
}

BOOL Init() {
    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId)) {
        swprintf_s(g_modifiedFlag, L"cleanDesktopCursorsModified_%lu", sessionId);
    }

    if (Wh_GetIntValue(g_modifiedFlag, 0)) {
        SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
        Wh_SetIntValue(g_modifiedFlag, 0);
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_readyEvent) {
        if (g_stopEvent) CloseHandle(g_stopEvent);
        if (g_readyEvent) CloseHandle(g_readyEvent);
        g_stopEvent = g_readyEvent = nullptr;
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        CloseHandle(g_readyEvent);
        g_stopEvent = g_readyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, INFINITE);
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;

    if (!g_workerOk) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        CloseHandle(g_stopEvent);
        g_thread = g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void SettingsChanged() {
    HWND hwnd = g_hwnd.load();
    if (hwnd) PostMessageW(hwnd, WM_APP_SETTINGS, 0, 0);
}

void Uninit() {
    if (g_thread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

} // namespace CursorModule

// ============================================================================
// WINDHAWK LIFECYCLE
// ============================================================================
BOOL Wh_ModInit() {
    LoadGlobalSettings();

    if (!DesktopModule::Init()) {
        return FALSE;
    }

    CursorModule::Init();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadGlobalSettings();
    DesktopModule::SettingsChanged();
    CursorModule::SettingsChanged();
}

void Wh_ModUninit() {
    CursorModule::Uninit();
    DesktopModule::Uninit();
}
