// ==WindhawkMod==
// @id              windows-10-language-flyout-guard-on-win11-24h2
// @name            Windows 10 language flyout guard and indicator colours
// @description     This mod hides the language flyout that shows up by itself when the private Windows 10 shell starts on Windows 11 24H2+
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -luser32
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 language flyout guard and indicator colours

This mod tries to fix two issues that appear when using the private Windows 10 shell with the Windows 10 taskbar on Windows 11 24H2, both related to the tray language indicator (`ITA`, `ENG`, …):

- **The flyout that appears on its own.** When the shell starts, the language flyout (or the invisible window that catches clicks across the screen) shows up without being requested. The mod hides it automatically, but **a click on the indicator is always allowed**, so the flyout opens only when you actually ask for it.

- **The flat grey indicator.** The Windows 11 indicator paints its cell with a grey box on a Windows 10 taskbar. The mod repaints it with the real taskbar colour and draws the letters flat and horizontally, so `ITA`/`IT` stay readable on both light and dark taskbars.

The guard lasts 20 seconds by default and extends itself slightly each time it hides something. Nothing is closed or destroyed: only windows that are not ours are hidden, and no ownership is required.

Everything runs **inside the private Windows 10 shell**; the Windows 11 shell is never modified and system files are not replaced.

## Screenshots

### Before

![Before](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/before.png)

### After

![After](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/after.png)



## Settings

| Setting | What it does |
|---|---|
| `LanguageGuard` (default on) | hide the language flyout that appears by itself |
| `LanguageGuardSeconds` (default 20) | how long the guard watches for it |
| `LanguageIndicatorColours` (default on) | paint the indicator with the Windows 10 colours |
| `LogLanguageGuard` (default off) | write the logon census of every window, class by class |

## The log

With `LogLanguageGuard` on, the first windows of the session are listed one by one, so the flyout can be recognised even when it is drawn by something else. The guard also writes a line for every window it hides, and a summary line at the end.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LanguageGuard: true
  $name: Hide the language flyout at logon
  $description: >-
    Hides the flyout (and the window that catches the clicks) when it appears by itself.
    A click on the indicator always lets it through.
- LanguageGuardSeconds: 20
  $name: Guard duration (seconds)
  $description: >-
    How long the guard watches for the flyout. Every window it hides extends the guard by
    a few seconds, up to four times this duration.
- LanguageIndicatorColours: true
  $name: Windows 10 colours of the language indicator
  $description: >-
    Paints the ITA/ENG cell with the taskbar background colour and a readable letter
    colour instead of the flat Windows 11 grey box.
- LogLanguageGuard: false
  $name: Log the windows of the logon
  $description: >-
    Writes every window seen during the guard to the log, class by class: the way to find
    out who draws a flyout the guard does not recognise yet.
*/
// ==/WindhawkModSettings==
#undef INTERFACE
#include <windows.h>
#include <stdlib.h>
#include <cstdint>
#include <limits.h>
#include <string.h>
#include <stdio.h>      // _snwprintf_s
#include <string>
#include <atomic>
#include <memory>
#include <commctrl.h>   // window subclassing (SetWindowSubclass/DefSubclassProc)
#include <windhawk_api.h>
#include <windhawk_utils.h>

// The shell this module lives in. In the monolith the flag lived among the globals of the
// shell-services section; here it is the only flag of that kind.
static std::atomic<bool> g_unloading{false};

// The monolith's detailed diagnostic switch: off here, this module logs what it does.
static std::atomic<bool> g_verboseDiagnostics{false};

// Options of this module.
static bool g_langGuardEnabled = true;
static DWORD g_langGuardMs = 20000;        // the guard duration, from the settings
static bool g_indicatorColours = true;
static bool g_langCensusLog = false;

// The real path of the image, captured before any hook of this mod runs.
static wchar_t g_realExePath[MAX_PATH] = {};

static bool ImageNameIs(const wchar_t* path, const wchar_t* expected) {
    if (!path || !expected) return false;
    const wchar_t* base = wcsrchr(path, L'\\');
    return _wcsicmp(base ? base + 1 : path, expected) == 0;
}

// The private Windows 10 shell is an explorer.exe that is *not* the one of %SystemRoot%.
// The Windows 11 shell is left completely alone.
static bool IsPrivateShellProcess() {
    wchar_t path[MAX_PATH] = {};
    if (!GetModuleFileNameW(nullptr, path, _countof(path))) return false;
    if (g_realExePath[0] == 0) wcscpy_s(g_realExePath, path);
    if (!ImageNameIs(path, L"explorer.exe")) return false;
    wchar_t systemRoot[MAX_PATH] = {};
    const DWORD rootLength =
        GetEnvironmentVariableW(L"SystemRoot", systemRoot, _countof(systemRoot));
    if (!rootLength || rootLength >= _countof(systemRoot)) return true;
    const size_t rootChars = wcslen(systemRoot);
    const size_t pathChars = wcslen(path);
    if (pathChars == rootChars + 13 && _wcsnicmp(path, systemRoot, rootChars) == 0 &&
        _wcsicmp(path + rootChars, L"\\explorer.exe") == 0) {
        return false;   // this is the system shell: nothing to do here
    }
    return true;
}

// https://stackoverflow.com/a/51274008
template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};
using string_setting_unique_ptr =
    std::unique_ptr<const WCHAR[], deleter_from_fn<Wh_FreeStringSetting>>;

class ScopedGdiObj {
public:
    explicit ScopedGdiObj(HGDIOBJ o = nullptr) : m_o(o) {}
    ~ScopedGdiObj() { if (m_o) DeleteObject(m_o); }
    ScopedGdiObj(const ScopedGdiObj&) = delete;
    ScopedGdiObj& operator=(const ScopedGdiObj&) = delete;
    HGDIOBJ get() const { return m_o; }
private:
    HGDIOBJ m_o;
};

// Restores the previously selected GDI object (SelectObject is sticky).
class ScopedSelectedObject {
public:
    ScopedSelectedObject(HDC hdc, HGDIOBJ obj) : m_hdc(hdc), m_old(nullptr) {
        if (obj) m_old = SelectObject(hdc, obj);
    }
    ~ScopedSelectedObject() { if (m_old && m_old != HGDI_ERROR) SelectObject(m_hdc, m_old); }
    ScopedSelectedObject(const ScopedSelectedObject&) = delete;
    ScopedSelectedObject& operator=(const ScopedSelectedObject&) = delete;
private:
    HDC m_hdc;
    HGDIOBJ m_old;
};

static wchar_t g_text[8] = {};
static wchar_t g_text2[8] = {};   // second native line (layout code, e.g. "IT" under "ITA")
static wchar_t g_lastDrawnText[8] = {};
static LOGFONTW g_font = {};
static int g_srcW = 0, g_srcH = 0;
static bool g_pending = false, g_firstBltDone = false, g_alreadyDrawn = false;
static DWORD g_threadId = 0;
static COLORREF g_lastGoodBg = CLR_INVALID;

// --- background colour used until the cell is painted by the mod -----------
// The original mod's trick (redraw the text upright instead of the rotated one)
// forces the background to be filled. If the colour is wrong a rectangle shows:
// that is the artefact reported on 2026-10-01 (a light grey square on the blue
// taskbar with unreadable text, because GetPixel on the destination DC did not
// answer and the code fell back to the system grey COLOR_BTNFACE).
// Order of the attempts:
//   1. colour sampled from the SCREEN just outside the indicator window
//      (TrayInputIndicatorWClass): that is the real taskbar background;
//   2. corners of the destination DC (as before, when they answer);
//   3. last good colour seen recently;
//   4. COLOR_BTNFACE, last resort only.
static COLORREF g_bgCache = CLR_INVALID;
static DWORD g_bgCacheTick = 0;
static int g_bgLogs = 0;
static DWORD g_pendingTick = 0;
static int  g_indicatorTextLogs = 0;

class ScopedReleaseDc {
public:
    explicit ScopedReleaseDc(HDC dc) : m_dc(dc) {}
    ~ScopedReleaseDc() { if (m_dc) ReleaseDC(nullptr, m_dc); }
    ScopedReleaseDc(const ScopedReleaseDc&) = delete;
    ScopedReleaseDc& operator=(const ScopedReleaseDc&) = delete;
    HDC get() const { return m_dc; }
    bool valid() const { return m_dc != nullptr; }
private:
    HDC m_dc;
};

// The indicator cell is a CHILD of the taskbar (Shell_TrayWnd > TrayNotifyWnd >
// cell), and FindWindowW only looks at top-level windows: it never found it. That
// is why SampleTaskbarColor always failed and PaintIndicatorCell left the cell
// alone without a word. The descendants of the taskbar are searched instead.
static BOOL CALLBACK FindIndicatorChildProc(HWND w, LPARAM param) {
    wchar_t cls[64] = {};
    if (GetClassNameW(w, cls, _countof(cls)) &&
        (_wcsicmp(cls, L"TrayInputIndicatorWClass") == 0 ||
         _wcsicmp(cls, L"InputIndicatorWClass") == 0)) {
        *(HWND*)param = w;
        return FALSE;
    }
    return TRUE;
}

static HWND FindInputIndicatorWindow() {
    HWND found = nullptr;
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) EnumChildWindows(tray, FindIndicatorChildProc, (LPARAM)&found);
    if (found) return found;
    const wchar_t* classes[] = { L"TrayInputIndicatorWClass", L"InputIndicatorWClass" };
    for (const wchar_t* cls : classes) {
        HWND w = FindWindowW(cls, nullptr);
        if (w) return w;
    }
    return nullptr;
}

// The taskbar is uniform: several samples are taken just outside the indicator and
// the most frequent one is chosen. Never inside the window (that is where the
// rectangle we draw is).
// The taskbar is uniform: several samples are taken just outside the indicator
// (never inside the cell, that is what the mod paints) and the most frequent
// colour wins. `how` carries the source into the log.
static bool SampleTaskbarColor(COLORREF* out, const wchar_t** how) {
    if (how) *how = L"none";
    HWND w = FindInputIndicatorWindow();
    if (!w) {
        static int saidNoWindow = 0;
        if (saidNoWindow < 2) { saidNoWindow++; Wh_Log(L"[lang] sample: indicator window not found"); }
        return false;
    }
    RECT rc = {};
    if (!GetWindowRect(w, &rc)) return false;
    if (rc.right <= rc.left || rc.bottom <= rc.top) return false;

    ScopedReleaseDc screen(GetDC(nullptr));
    if (!screen.valid()) return false;

    // Four points just OUTSIDE the cell, on its top and bottom edges: the tray
    // icons sit in the middle band (and are white), so those rows are bare
    // taskbar. Averaged, because the taskbar is a gradient.
    const POINT pts[4] = {
        { rc.left - 2,  rc.top + 4 },
        { rc.left - 2,  rc.bottom - 5 },
        { rc.right + 2, rc.top + 4 },
        { rc.right + 2, rc.bottom - 5 },
    };
    int r = 0, g = 0, b = 0, count = 0;
    for (int i = 0; i < 4; i++) {
        if (pts[i].x < 0 || pts[i].y < 0) continue;
        const COLORREF c = GetPixel(screen.get(), pts[i].x, pts[i].y);
        if (c == CLR_INVALID || c == 0x000000) continue;
        r += GetRValue(c); g += GetGValue(c); b += GetBValue(c);
        count++;
    }
    if (count == 0) {
        static int saidNoSamples = 0;
        if (saidNoSamples < 2) {
            saidNoSamples++;
            Wh_Log(L"[lang] no usable taskbar sample around the indicator, cell left alone");
        }
        return false;
    }
    *out = RGB(r / count, g / count, b / count);
    if (how) *how = L"screen (taskbar)";
    return true;
}

static bool CornersBackground(HDC hdc, int w, int h, COLORREF* out) {
    const int xs[] = { 0, w - 1 };
    const int ys[] = { 0, h - 1 };
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            COLORREF c = GetPixel(hdc, xs[i], ys[j]);
            if (c != CLR_INVALID && c != 0x000000) {
                *out = c;
                return true;
            }
        }
    }
    return false;
}

static void ChooseIndicatorBackground(HDC hdc, int w, int h, COLORREF* bg, const wchar_t** how) {
    // The taskbar is translucent glass: no flat colour can match it (it changes
    // with the wallpaper behind). On glass, GDI black is transparent, so the cell
    // and the box simply show the taskbar. Letters stay white (IsDarkColor).
    (void)hdc; (void)w; (void)h;
    g_lastGoodBg = RGB(0, 0, 0);
    *bg = RGB(0, 0, 0);
    *how = L"black (glass transparent)";
    return;
    COLORREF c = CLR_INVALID;
    if (g_bgCache != CLR_INVALID && (GetTickCount() - g_bgCacheTick) < 2000) {
        c = g_bgCache;
        *how = L"short cache";
    } else if (SampleTaskbarColor(&c, nullptr)) {
        g_bgCache = c;
        g_bgCacheTick = GetTickCount();
        *how = L"screen (taskbar)";
    } else if (hdc && CornersBackground(hdc, w, h, &c)) {
        g_bgCache = c;
        g_bgCacheTick = GetTickCount();
        *how = L"DC corners";
    } else if (g_lastGoodBg != CLR_INVALID) {
        c = g_lastGoodBg;
        *how = L"last good colour";
    } else {
        c = GetSysColor(COLOR_BTNFACE);
        *how = L"COLOR_BTNFACE (fallback)";
    }
    if (c != CLR_INVALID) g_lastGoodBg = c;
    *bg = c;
}
/*
static bool IsDarkColor(COLORREF c) {
    const int luma = (GetRValue(c) * 299 + GetGValue(c) * 587 + GetBValue(c) * 114) / 1000;
    return luma < 110;
}
*/
// The text is chosen for contrast: COLOR_BTNTEXT was used before, which on a
// dark theme is white and vanished on the grey rectangle.
// The cell background is black (= transparent on the glass taskbar), so its
// colour says nothing about the theme. The letters follow the system theme
// instead, as Windows 10 does: dark taskbar -> white, light taskbar -> dark.
// SystemUsesLightTheme is cached for a second (this runs on every paint).
static bool TaskbarUsesLightTheme() {
    static DWORD lastTick = 0;
    static bool light = false;
    const DWORD now = GetTickCount();
    if (lastTick && now - lastTick < 1000) return light;
    lastTick = now;
    DWORD value = 0, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS)
        light = (value != 0);
    else
        light = false;
    return light;
}

static COLORREF IndicatorTextColor(COLORREF bg) {
    (void)bg;
    // NEVER pure black: on the glass taskbar GDI black (0,0,0) is the colour that
    // means "transparent", so black letters on the black cell are invisible (that
    // is why the light theme showed nothing). A near-black stays opaque.
    return TaskbarUsesLightTheme() ? RGB(28, 28, 28) : RGB(255, 255, 255);
}

static void LogIndicatorColors(COLORREF bg, const wchar_t* how) {
    if (g_bgLogs >= 6) return;
    g_bgLogs++;
    COLORREF fg = IndicatorTextColor(bg);
    Wh_Log(L"[lang] indicator background 0x%06X (from %s), text 0x%06X",
           (unsigned)bg & 0xFFFFFFu, how, (unsigned)fg & 0xFFFFFFu);
}

static bool LooksLikeLayoutText(LPCWSTR s, int len) {
    if (!s || len < 2 || len > 4) return false;
    for (int i = 0; i < len; i++)
        if (!IsCharAlphaW(s[i])) return false;
    return true;
}

static bool IsRotatedFont(HDC hdc, LOGFONTW* lf) {
    HFONT font = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    if (!font) return false;
    if (GetObjectW(font, sizeof(*lf), lf) == 0) return false;
    return (lf->lfEscapement != 0 || lf->lfOrientation != 0);
}

typedef BOOL(WINAPI* ExtTextOutW_t)(HDC, int, int, UINT, const RECT*, LPCWSTR, UINT, const INT*);
static ExtTextOutW_t ExtTextOutW_Original = nullptr;

static void DrawHorizontalText(HDC hdc, const RECT* rc, COLORREF color) {
    LOGFONTW lf = g_font;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    HFONT font = CreateFontIndirectW(&lf);
    if (!font) return;
    ScopedGdiObj fontObj((HGDIOBJ)font);
    ScopedSelectedObject sel(hdc, (HGDIOBJ)font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    TEXTMETRICW tm = {};
    GetTextMetricsW(hdc, &tm);
    const int lineH = tm.tmHeight > 0 ? tm.tmHeight : 12;
    const int n = g_text2[0] ? 2 : 1;
    const int y = rc->top + ((rc->bottom - rc->top) - lineH * n) / 2;
    RECT r1 = { rc->left, y, rc->right, y + lineH };
    DrawTextW(hdc, g_text, -1, &r1, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX);
    if (n == 2) {
        RECT r2 = { rc->left, y + lineH, rc->right, y + 2 * lineH };
        DrawTextW(hdc, g_text2, -1, &r2, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX);
    }
}

static BOOL WINAPI ExtTextOutW_Hook(HDC hdc, int x, int y, UINT options, const RECT* lprect,
                                    LPCWSTR lpString, UINT c, const INT* lpDx) {
    try {
        if (!g_indicatorColours)
            return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
        const int len = (int)c;
        LOGFONTW lf = {};
        if (LooksLikeLayoutText(lpString, len) && IsRotatedFont(hdc, &lf)) {
            wchar_t newText[8] = {};
            wcsncpy_s(newText, _countof(newText), lpString, len);
            if (wcscmp(newText, g_lastDrawnText) != 0) g_alreadyDrawn = false;

            // The native indicator draws two rotated lines per paint: the
            // language ("ITA") and the layout ("IT"). Only ROTATED layout text is
            // taken (any other 2-4 letter string explorer draws - "LVR", "HUN" -
            // is not the indicator). The longer string goes on top.
            if (!g_pending) { g_text[0] = 0; g_text2[0] = 0; }
            if (!g_text[0]) wcsncpy_s(g_text, _countof(g_text), newText, _TRUNCATE);
            else if (!g_text2[0] && wcscmp(g_text, newText) != 0)
                wcsncpy_s(g_text2, _countof(g_text2), newText, _TRUNCATE);
            if (g_text2[0] && wcslen(g_text) < wcslen(g_text2)) {
                wchar_t tmp[8] = {};
                wcscpy_s(tmp, g_text); wcscpy_s(g_text, g_text2); wcscpy_s(g_text2, tmp);
            }
            if (g_indicatorTextLogs < 4) {
                g_indicatorTextLogs++;
                Wh_Log(L"[lang] indicator text seen (rotated): \"%s\" / \"%s\"", g_text, g_text2);
            }

            HBITMAP bitmap = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
            BITMAP bm = {};
            if (bitmap && GetObject(bitmap, sizeof(bm), &bm)) {
                if (!g_pending) {
                    g_font = lf;
                    g_srcW = bm.bmWidth;
                    g_srcH = bm.bmHeight;
                    g_pending = true;
                    g_pendingTick = GetTickCount();
                    g_firstBltDone = false;
                    g_threadId = GetCurrentThreadId();
                }
                COLORREF fill = CLR_INVALID;
                const wchar_t* how = L"none";
                ChooseIndicatorBackground(nullptr, 0, 0, &fill, &how);
                LogIndicatorColors(fill, how);
                // RAII: the brush is deleted on exit even if FillRect
                // were to raise an exception (outer try/catch).
                ScopedGdiObj brushObj((HGDIOBJ)CreateSolidBrush(fill));
                if (brushObj.get()) {
                    RECT rc = { 0, 0, bm.bmWidth, bm.bmHeight };
                    FillRect(hdc, &rc, (HBRUSH)brushObj.get());
                }
                g_pendingTick = GetTickCount();
                return TRUE;
            }
        }
    } catch (...) {
        Wh_Log(L"[lang] exception in ExtTextOutW");
    }
    return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c, lpDx);
}

typedef BOOL(WINAPI* BitBlt_t)(HDC, int, int, int, int, HDC, int, int, DWORD);
static BitBlt_t BitBlt_Original = nullptr;

static BOOL WINAPI BitBlt_Hook(HDC hdcDest, int xDest, int yDest, int w, int h,
                               HDC hdcSrc, int xSrc, int ySrc, DWORD rop) {
    BOOL result = BitBlt_Original(hdcDest, xDest, yDest, w, h, hdcSrc, xSrc, ySrc, rop);

    if (!g_indicatorColours) return result;
    if (!g_pending || g_srcW <= 0 || g_srcH <= 0) return result;
    // If too much time passed between the text and this blit, the "pending" is
    // stale (the indicator did not repaint any more): better to touch nothing
    // than to fill a random rectangle.
    if (GetTickCount() - g_pendingTick > 1500) {
        g_pending = false;
        return result;
    }
    if (GetCurrentThreadId() != g_threadId) return result;
    if (xDest != 0 || yDest != 0) return result;

    if (w == g_srcW && h == g_srcH) {         // first blit: the rotated bitmap only
        g_firstBltDone = true;
        return result;
    }
    if (!g_firstBltDone) return result;
    if (!((w > g_srcW || h > g_srcH) && w >= g_srcH)) return result;   // not the indicator

    RECT rc = { 0, 0, w, h };

    if (g_alreadyDrawn) {                      // hover: repaint without touching the background
        DrawHorizontalText(hdcDest, &rc, IndicatorTextColor(g_lastGoodBg));
        g_pending = false;
        return result;
    }

    // The real taskbar colour: first the screen just outside the indicator,
    // then the corners of the DC, then the last good one, and only at the end the
    // system grey (which was the cause of the grey rectangle on the coloured taskbar).
    COLORREF bg = CLR_INVALID;
    const wchar_t* how = L"none";
    ChooseIndicatorBackground(hdcDest, w, h, &bg, &how);
    LogIndicatorColors(bg, how);

    ScopedGdiObj brushObj((HGDIOBJ)CreateSolidBrush(bg));
    if (brushObj.get()) FillRect(hdcDest, &rc, (HBRUSH)brushObj.get());

    DrawHorizontalText(hdcDest, &rc, IndicatorTextColor(bg));

    wcscpy_s(g_lastDrawnText, g_text);
    g_alreadyDrawn = true;
    g_pending = false;
    return result;
}

static bool ContainsNoCase(const wchar_t* haystack, const wchar_t* needle) {
    if (!haystack || !needle || !*needle) return false;
    size_t n = wcslen(needle);
    for (const wchar_t* p = haystack; *p; p++) {
        if (_wcsnicmp(p, needle, n) == 0) return true;
    }
    return false;
}

// Who asked for the menu, so that the show desktop button and the clock can be
// told apart from every other window of the tray.
static bool IsShowDesktopClass(const wchar_t* cls) {
    return cls && _wcsicmp(cls, L"TrayShowDesktopButtonWClass") == 0;
}
static bool IsClockClass(const wchar_t* cls) {
    return cls && (_wcsicmp(cls, L"TrayClockWClass") == 0 || _wcsicmp(cls, L"ClockButton") == 0);
}

static bool g_explorerClockMenuShown = false;

// --- 9) language flyout: census and suppression in the first 5 seconds ------
// The indicator fix (from the official mod) only draws text and bitmaps:
// it opens no windows. The flyout that appears at logon, therefore, is either
// opened by the legacy shell itself (class "Shell_InputSwitchTopLevelWindow", with the
// dismissal window "Shell_InputSwitchDismissOverlay"), or recreated by
// another mod: on this machine there is "win7-language-switcher-restorer",
// which hooks exactly those two classes and shows in their place a window
// of class "Windhawk_Win78LanguageFlyout". All three are in the list.
// In the first 5 seconds of the shell's life:
//  - every visible window of this process goes into the log once
//    (census: it is needed to recognise the culprit if it is not that class);
//  - windows that are a language flyout are hidden at once, both by
//    preventing ShowWindow from showing them and by hiding them afterwards.
//
// On the explorer restart performed by the mod (not on the first start) the flyout
// appeared anyway. The two possible explanations, both covered here:
//  a) the window is created already visible (no ShowWindow to block) or
//     shown with ShowWindowAsync: now all three roads are taken, plus
//     the very moment of creation (CreateWindowExW);
//  b) the visible flyout is not ours: it is drawn by another process
//     (ctfmon, TextInputHost, the system shell still alive for an instant).
//     The census, for the flyout classes, is no longer limited to our
//     process: it looks for them everywhere and hides them (hiding a window does not
//     require owning it).
// Moreover WM_CLOSE is no longer sent to the other mod's window: once closed,
// that window is recreated, and the recreation can restore the
// original flyout. It is simply hidden (that was the suspicion "the guard makes it
// reappear")).
static DWORD g_langGuardMsFromSettings = 20000;  // set from the settings
static DWORD g_langGuardUntil = 0;
static DWORD g_langGuardStart = 0;
static DWORD g_langCensus = 0;
static int  g_langSuppressions = 0;    // guard interventions (one log line each)
static int  g_langLogs = 0;            // lines already written (cap: no walls of text)
static HWND g_langHidden[8] = {};      // windows already hidden (so the log is not repeated)
static int  g_langHiddenCount = 0;
static bool g_langEndLogged = false;
static DWORD g_langManualUntil = 0;    // manual open: the guard steps aside
static int   g_langManualLogs = 0;

typedef BOOL(WINAPI* ShowWindow_t)(HWND, int);
static ShowWindow_t ShowWindow_Original = nullptr;
typedef BOOL(WINAPI* ShowWindowAsync_t)(HWND, int);
static ShowWindowAsync_t ShowWindowAsync_Original = nullptr;

static bool LooksLikeLanguageFlyout(HWND hwnd) {
    wchar_t cls[128] = {};
    if (!GetClassNameW(hwnd, cls, _countof(cls))) return false;
    if (_wcsicmp(cls, L"TrayInputIndicatorWClass") == 0) return false;   // the button, not the flyout

    // Known classes of the Windows 10 flyout (the same ones the other mod
    // hooks to replace it with its own) and of its dismissal window:
    // if it stays visible on its own, it eats the clicks of the whole screen.
    if (_wcsicmp(cls, L"Shell_InputSwitchTopLevelWindow") == 0) return true;
    if (_wcsicmp(cls, L"Shell_InputSwitchDismissOverlay") == 0) return true;
    if (_wcsicmp(cls, L"Windhawk_Win78LanguageFlyout") == 0) return true;   // the other mod's flyout

    return ContainsNoCase(cls, L"LanguageFlyout") || ContainsNoCase(cls, L"LangSwitcher") ||
           ContainsNoCase(cls, L"InputSwitch");
}

// If something is suppressed, the guard is extended: it is needed to catch the
// dismissal window as well, which can appear right after the flyout. The cap is
// four times the configured duration.
static void ExtendLanguageGuard() {
    DWORD now = GetTickCount();
    // The extension is a fixed 15 s per intervention, and it never goes past the
    // cap above: the guard cannot grow without an end.
    DWORD extended = now + 15000;
    const DWORD cap = g_langGuardMsFromSettings * 4;
    if (extended > g_langGuardStart + cap) extended = g_langGuardStart + cap;
    if (extended > g_langGuardUntil) g_langGuardUntil = extended;
}

// Name of the process that owns the window: if the flyout is not ours
// one must know who draws it (that was the piece missing from the log).
static void AppendWindowOwner(HWND hwnd, wchar_t* buf, size_t count) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    wchar_t exe[MAX_PATH] = {};
    HANDLE proc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (proc) {
        DWORD len = (DWORD)_countof(exe);
        if (!QueryFullProcessImageNameW(proc, 0, exe, &len)) exe[0] = 0;
        CloseHandle(proc);
    }
    const wchar_t* base = exe[0] ? exe : L"(unknown)";
    const wchar_t* slash = wcsrchr(base, L'\\');
    if (slash) base = slash + 1;
    _snwprintf_s(buf, count, _TRUNCATE, L"%s, pid %lu", base, pid);
}

static void LogLanguageEvent(const wchar_t* how, HWND hwnd) {
    if (g_langLogs >= 12) return;                 // beyond the cap: final summary only
    g_langLogs++;
    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, _countof(cls));
    wchar_t owner[160] = {};
    AppendWindowOwner(hwnd, owner, _countof(owner));
    Wh_Log(L"[language] %s: class %s (%s)", how, cls, owner);
}

// Hides the window and extends the guard. No WM_CLOSE is sent: once
// the other mod's window is closed it gets recreated, and the recreation can
// bring the original flyout back. It is hidden even if it belongs to another
// process: hiding a window does not require owning it.
static void SuppressLanguageWindow(HWND hwnd, const wchar_t* how) {
    for (int i = 0; i < g_langHiddenCount; i++) {
        if (g_langHidden[i] == hwnd) {
            if (ShowWindow_Original) ShowWindow_Original(hwnd, SW_HIDE);
            ExtendLanguageGuard();
            return;                               // already reported: the log is not repeated
        }
    }
    if (g_langHiddenCount < (int)_countof(g_langHidden)) g_langHidden[g_langHiddenCount++] = hwnd;
    g_langSuppressions++;
    LogLanguageEvent(how, hwnd);
    if (ShowWindow_Original) ShowWindow_Original(hwnd, SW_HIDE);
    else ShowWindow(hwnd, SW_HIDE);
    ExtendLanguageGuard();
}

// The guard covers what appears by itself at logon. If the user clicks
// the indicator, the appearance is intended: for 5 seconds the guard lets it
// through (that was the request: "it suppresses what appears at logon but
// allows opening it by hand").
static bool LanguageManualOpenActive() {
    return g_langManualUntil != 0 && GetTickCount() < g_langManualUntil;
}

static void NoteManualLanguageOpen(const wchar_t* how) {
    g_langManualUntil = GetTickCount() + 5000;
    if (g_langManualLogs < 4) {
        g_langManualLogs++;
        Wh_Log(L"[language] %s: the guard lets the flyout through for five seconds", how);
    }
}

static BOOL WINAPI ShowWindow_Hook(HWND hwnd, int cmd) {
    try {
        if (cmd != SW_HIDE && !LanguageManualOpenActive() && g_langGuardUntil &&
            GetTickCount() < g_langGuardUntil && LooksLikeLanguageFlyout(hwnd)) {
            SuppressLanguageWindow(hwnd, L"show blocked (ShowWindow)");
            return TRUE;
        }
    } catch (...) {
    }
    return ShowWindow_Original(hwnd, cmd);
}

static BOOL WINAPI ShowWindowAsync_Hook(HWND hwnd, int cmd) {
    try {
        if (cmd != SW_HIDE && !LanguageManualOpenActive() && g_langGuardUntil &&
            GetTickCount() < g_langGuardUntil && LooksLikeLanguageFlyout(hwnd)) {
            SuppressLanguageWindow(hwnd, L"show blocked (ShowWindowAsync)");
            return TRUE;
        }
    } catch (...) {
    }
    return ShowWindowAsync_Original(hwnd, cmd);
}

// Third way a flyout can appear: SetWindowPos with SWP_SHOWWINDOW.
// (In the 2026-10-01 log the flyout kept coming back: the guard only covered
// ShowWindow and creation.)
typedef BOOL(WINAPI* SetWindowPos_t)(HWND, HWND, int, int, int, int, UINT);
static SetWindowPos_t SetWindowPos_Original = nullptr;

static BOOL WINAPI SetWindowPos_Hook(HWND hwnd, HWND after, int x, int y, int cx, int cy, UINT flags) {
    try {
        if ((flags & SWP_SHOWWINDOW) && !LanguageManualOpenActive() && g_langGuardUntil &&
            GetTickCount() < g_langGuardUntil && LooksLikeLanguageFlyout(hwnd)) {
            SuppressLanguageWindow(hwnd, L"show blocked (SetWindowPos)");
            flags &= ~SWP_SHOWWINDOW;
            if (!flags) return TRUE;
        }
    } catch (...) {
    }
    return SetWindowPos_Original(hwnd, after, x, y, cx, cy, flags);
}

// A window created already visible does not go through ShowWindow: here it is caught
// at creation time. Outside the guard's seconds the hook exits
// at once (one read and one comparison), so it does not weigh on the shell.
typedef HWND(WINAPI* CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
                                        HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;

static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR cls, LPCWSTR title, DWORD style,
                                        int x, int y, int w, int h, HWND parent, HMENU menu,
                                        HINSTANCE inst, LPVOID param) {
    HWND created = CreateWindowExW_Original(exStyle, cls, title, style, x, y, w, h, parent, menu, inst, param);
    try {
        if (created && !LanguageManualOpenActive() && g_langGuardUntil &&
            GetTickCount() < g_langGuardUntil && LooksLikeLanguageFlyout(created))
            SuppressLanguageWindow(created, L"window created during the guard");
    } catch (...) {
    }
    return created;
}

static BOOL CALLBACK LangCensusProc(HWND hwnd, LPARAM) {
    if (g_langCensusLog) {
        wchar_t cls[128] = {};
        wchar_t title[128] = {};
        GetClassNameW(hwnd, cls, _countof(cls));
        GetWindowTextW(hwnd, title, _countof(title));
        if (cls[0] && g_langLogs < 40) {
            g_langLogs++;
            Wh_Log(L"[language] window at logon: class %s title \"%s\"", cls, title);
        }
    }
    try {
        if (!IsWindowVisible(hwnd)) return TRUE;

        // Flyout windows are looked for in ANY process: if it is
        // ctfmon/TextInputHost that draws it (or the system shell still alive
        // for an instant), inside explorer it would never be seen. The filter on the
        // pid stays only for the generic census, which concerns us.
        if (LooksLikeLanguageFlyout(hwnd)) {
            SuppressLanguageWindow(hwnd, L"flyout visible during the guard");
            return TRUE;
        }

        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != GetCurrentProcessId()) return TRUE;

        wchar_t cls[128] = {};
        wchar_t title[128] = {};
        GetClassNameW(hwnd, cls, _countof(cls));
        GetWindowTextW(hwnd, title, _countof(title));

        // The list is there to recognise the culprit, not to tell the story of the
        // taskbar: known shell windows are not written.
        static const wchar_t* chrome[] = {
            L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", L"Progman", L"WorkerW",
            L"ApplicationManager_ImmersiveShellWindow", L"DummyDWMListenerWindow",
            L"EdgeUiInputWndClass", L"EdgeUiInputTopWndClass", L"UserOOBEWindowClass",
            L"ApplicationFrameWindow",
        };
        for (const wchar_t* c : chrome) {
            if (_wcsicmp(cls, c) == 0) return TRUE;
        }

        if (g_langCensus < 10) {
            g_langCensus++;
            Wh_Log(L"[language] window at logon: class %s title \"%s\"", cls, title);
        }
    } catch (...) {
    }
    return TRUE;
}

// --- 10-bis) the language indicator: manual clicks, and the grey rectangle --
// Two jobs in the same subclass of the indicator window:
//   1. mouse clicks are the only way for the user to open the flyout by hand:
//      when one arrives the logon guard (section 10) is parked for a few
//      seconds, so a deliberate click is never mistaken for the automatic
//      appearance;
//   2. the cell background is a THEME part, not the class brush (see the note
//      above PaintIndicatorCell): on Windows 11 the theme call leaves the
//      classic face grey behind, so the mod composes the cell itself - taskbar
//      colour as the background, layout abbreviation drawn upright - and blits
//      it in one go. Geometry comes from the window's own client rect.
class ScopedWindowDc {
public:
    ScopedWindowDc(HWND w, HDC dc) : m_w(w), m_dc(dc) {}
    ~ScopedWindowDc() { if (m_dc) ReleaseDC(m_w, m_dc); }
    ScopedWindowDc(const ScopedWindowDc&) = delete;
    ScopedWindowDc& operator=(const ScopedWindowDc&) = delete;
    HDC get() const { return m_dc; }
    bool valid() const { return m_dc != nullptr; }
private:
    HWND m_w;
    HDC m_dc;
};

static const UINT_PTR kIndicatorSubclassId = 78;

// The subclass procedure type Windhawk wants: five parameters, the last one is
// the reference data. It is NOT the six-parameter comctl32 SUBCLASSPROC used by
// SetWindowSubclass - passing the wrong one is a compile error, and mixing the
// two is what broke 0.3.7. Everything that subclasses across threads must use
// this type; SetWindowSubclass/DefSubclassProc stay for the same-thread cases.
typedef LRESULT (*IndicatorSubclassFn)(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

// Windows of the tray indicator the mod supervises. The names come from the
// shipped binaries: the cell is "TrayInputIndicatorWClass" (and
// "InputIndicatorWClass" on older builds), the button explorer creates inside
// the tray is "InputIndicatorButton", its sibling is "IMEModeButton".
static const wchar_t* const kIndicatorClasses[] = {
    L"TrayInputIndicatorWClass",
    L"InputIndicatorWClass",
    L"InputIndicatorButton",
    L"IMEModeButton",
};
static const int kIndicatorClassCount = (int)(sizeof(kIndicatorClasses) / sizeof(kIndicatorClasses[0]));

// One entry per supervised window. The flag says whether the subclass is really
// installed: GetWindowSubclass cannot be used here (it wants the six-parameter
// type), so the mod remembers the outcome of the install call itself.
struct IndicatorTarget {
    HWND wnd;
    bool subclassed;
};
static IndicatorTarget g_indicatorTargets[8] = {};
static int g_indicatorTargetCount = 0;

// Defined with the supervision code below, used here to report whether the
// subclass is installed (the measurement runs before that code in the file).
static int TrackedIndicatorIndex(HWND w);

// Read-back of the paint: the screen pixel at the centre of the indicator is
// read a moment after the blit, because a blit is not on the screen yet when the
// function returns. Bounded, and only for the first times.
struct IndicatorLayerCheck {
    POINT centre;
    COLORREF expected;
    DWORD due;
    HWND layer;
    wchar_t cls[64];
    bool pending;
};
static IndicatorLayerCheck g_indicatorCheck = {};
static int g_indicatorCheckLogs = 0;
// At most two lines: one for the first check (so a log always shows the layer the cell
// ended up in) and one for a real surprise. The old code wrote six colour mismatches
// that the transparent background made inevitable.
static const int kIndicatorCheckLimit = 2;

static DWORD g_lastIndicatorArm = 0;
static DWORD g_lastIndicatorDiag = 0;      // when the measurement was last written
static int g_indicatorPaints = 0;          // WM_PAINT seen on the indicator
static int g_indicatorErases = 0;          // WM_ERASEBKGND seen on the indicator
static int g_indicatorCellPaints = 0;      // times the cell was painted by the mod
static int g_indicatorCellLogs = 0;
static int g_indicatorDiagLogs = 0;

static int ColorDistance(COLORREF a, COLORREF b) {
    int d = (int)GetRValue(a) - (int)GetRValue(b);
    if (d < 0) d = -d;
    int e = (int)GetGValue(a) - (int)GetGValue(b);
    if (e < 0) e = -e;
    int f = (int)GetBValue(a) - (int)GetBValue(b);
    if (f < 0) f = -f;
    return d + e + f;
}


// Why the grey box appears, read from the shipped binaries (no guessing):
// the Windows 10 tray input indicator is a child window of the taskbar created
// by explorer.exe with class "TrayInputIndicatorWClass" and implemented by
// CTrayInputIndicator (source file pcshell\shell\explorer\trayindicator.cpp).
// Its cell background is not the class brush: the paint path is
// CTrayInputIndicator::_OnPaintInputIndicator ->
// CTrayInputIndicator::_DoPaintInputIndicator, which first calls
// CTrayInputIndicator::DrawThemeBackgroundWrapper (uxtheme!DrawThemeBackground
// with the theme handle opened by CTrayInputIndicator::_OpenThemeData) and then
// draws the rotated glyphs on top.
// On Windows 11 that theme part is gone, so the theme call leaves the cell with
// the classic face grey (COLOR_3DFACE, 0xF0F0F0) while the taskbar itself is
// painted with the accent colour: that is the light grey rectangle with
// unreadable glyphs. Nothing can be "erased harder" - the cell has to be
// painted here, with the real taskbar colour and the window's own geometry.
//
// The cell is not the only window involved, and the log of 2026-10-01 18:14
// says why the first attempt changed nothing:
//   * "[lang] measure: TrayInputIndicatorWClass 45x50 ... our subclass no":
//     the subclass had never been attached (the old code called
//     SetWindowSubclass from the services thread: it does not subclass a window
//     that belongs to another thread);
//   * "under (1775,1032) there is 0x12048C InputIndicatorButton": the point
//     inside the cell belongs to a CHILD window. explorer registers that class
//     itself (RegisterClassW of "InputIndicatorButton" at 0x140063189, then
//     CreateWindowExW at 0x14006320F, exStyle 0x54000000, cbWndExtra 8, next to
//     its sibling "IMEModeButton") and paints it with BeginBufferedPaint +
//     IsCompositionActive (uxtheme): on Windows 11 the theme part is missing, so
//     the button fills its own client area with the classic face grey, ON TOP of
//     the cell. Painting only the outer cell can therefore change nothing
//     visible, no matter how the colour is sampled.
// The mod now supervises the cell AND its buttons, and it paints whichever of
// them the tray decides to use.


static bool CursorOverWindow(HWND hwnd) {
    POINT p = {};
    RECT rc = {};
    if (!GetCursorPos(&p) || !GetWindowRect(hwnd, &rc)) return false;
    return p.x >= rc.left && p.x < rc.right && p.y >= rc.top && p.y < rc.bottom;
}

// The layout abbreviation: the exact string explorer wanted to draw when the
// ExtTextOutW/BitBlt hooks caught it, otherwise the one Windows reports for the
// active keyboard layout (LOCALE_SABBREVLANGNAME: ITA, ENG, DEU ...).
static bool IndicatorText(HWND hwnd, wchar_t* out, size_t count) {
    if (!out || count < 4) return false;
    out[0] = 0;
    if (g_text[0]) {
        wcsncpy_s(out, count, g_text, _TRUNCATE);
        return out[0] != 0;
    }
    const DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    const HKL hkl = GetKeyboardLayout(tid);
    if (!hkl) return false;
    const LANGID lang = (LANGID)LOWORD((UINT_PTR)hkl);
    wchar_t abbrev[16] = {};
    if (!GetLocaleInfoW(MAKELCID(lang, SORT_DEFAULT), LOCALE_SABBREVLANGNAME,
                        abbrev, _countof(abbrev)) || !abbrev[0])
        return false;
    wcsncpy_s(out, count, abbrev, _TRUNCATE);
    return out[0] != 0;
}

// Second line: the native one when it was seen, otherwise the two-letter country
// code of the active layout (what Windows 10 shows under "ITA").
static bool IndicatorText2(HWND hwnd, wchar_t* out, size_t count) {
    if (!out || count < 4) return false;
    out[0] = 0;
    if (g_text[0]) {
        if (g_text2[0]) wcsncpy_s(out, count, g_text2, _TRUNCATE);
        return out[0] != 0;
    }
    const DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    const HKL hkl = GetKeyboardLayout(tid);
    if (!hkl) return false;
    const LANGID lang = (LANGID)LOWORD((UINT_PTR)hkl);
    if (!GetLocaleInfoW(MAKELCID(lang, SORT_DEFAULT), LOCALE_SISO3166CTRYNAME, out, (int)count))
        out[0] = 0;
    return out[0] != 0;
}

class ScopedMemDc {
public:
    explicit ScopedMemDc(HDC src) : m_dc(CreateCompatibleDC(src)) {}
    ~ScopedMemDc() { if (m_dc) DeleteDC(m_dc); }
    ScopedMemDc(const ScopedMemDc&) = delete;
    ScopedMemDc& operator=(const ScopedMemDc&) = delete;
    HDC get() const { return m_dc; }
    bool valid() const { return m_dc != nullptr; }
private:
    HDC m_dc;
};

// The deepest supervised window at a screen point: the window under the point is
// walked up until one of the supervised ones is met. That is the layer the user
// actually sees there, and the only one that should carry the letters.
static HWND TopmostTrackedAt(POINT p) {
    HWND w = WindowFromPoint(p);
    int guard = 0;
    while (w && guard++ < 32) {
        if (TrackedIndicatorIndex(w) >= 0) return w;
        w = GetParent(w);
    }
    return nullptr;
}

// The area of the indicator: the cell if it is there, otherwise the first
// supervised window. The centre of this area is the point the user looks at.
static bool PrimaryIndicatorRect(RECT* out) {
    if (!out) return false;
    for (int i = 0; i < g_indicatorTargetCount; i++) {
        HWND w = g_indicatorTargets[i].wnd;
        if (!w || !IsWindow(w) || !IsWindowVisible(w)) continue;
        wchar_t cls[64] = {};
        if (!GetClassNameW(w, cls, _countof(cls))) continue;
        if (_wcsicmp(cls, L"TrayInputIndicatorWClass") != 0 &&
            _wcsicmp(cls, L"InputIndicatorWClass") != 0) continue;
        if (GetWindowRect(w, out)) return true;
    }
    for (int i = 0; i < g_indicatorTargetCount; i++) {
        HWND w = g_indicatorTargets[i].wnd;
        if (!w || !IsWindow(w) || !IsWindowVisible(w)) continue;
        if (GetWindowRect(w, out)) return true;
    }
    return false;
}

// Is this paint run the one that must carry the letters? True for the topmost
// supervised layer at the centre of the indicator (the one that is not covered
// by anything of ours), false for the layers underneath - so on a stack of
// windows the abbreviation appears once, at the right size.
static bool ThisLayerShowsText(HWND hwnd) {
    RECT area = {};
    if (!PrimaryIndicatorRect(&area)) return true;          // no reference: keep the old behaviour
    POINT centre = { (area.left + area.right) / 2, (area.top + area.bottom) / 2 };
    HWND top = TopmostTrackedAt(centre);
    if (!top || !IsWindow(top)) return true;                // nothing of ours there: keep the old behaviour
    return top == hwnd;
}

// Paints one window of the tray indicator: background = the real taskbar colour,
// text = the layout abbreviation drawn upright in a contrast colour. Everything
// is composed in a memory DC and blitted once, so the window never flickers and
// the grey background plus the rotated glyphs disappear in a single operation.
// Geometry comes from the window's own client rect, so a wrong offset is
// impossible - and the same routine serves the cell and the button on top of it.
static void PaintIndicatorCell(HWND hwnd, const wchar_t* why) {
    try {
        if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd)) return;

        // Only the windows the tray scan recognised, so a recycled handle can
        // never make the mod paint something that is not the indicator. The
        // scan recognises the cell, its buttons and every child of them, so the
        // paint follows the stack instead of trusting one class name.
        wchar_t cls[64] = {};
        if (!GetClassNameW(hwnd, cls, _countof(cls))) return;
        if (TrackedIndicatorIndex(hwnd) < 0) return;

        RECT rc = {};
        if (!GetClientRect(hwnd, &rc)) return;
        const int w = rc.right;
        const int h = rc.bottom;
        if (w < 4 || h < 4 || w > 256 || h > 256) return;   // not a tray cell

        const wchar_t* how = L"black (glass transparent)";
        const bool hover = CursorOverWindow(hwnd);
        const COLORREF bg = RGB(0, 0, 0);

        const bool showsText = ThisLayerShowsText(hwnd);

        wchar_t text[16] = {};
        if (showsText && !IndicatorText(hwnd, text, _countof(text))) return;
        wchar_t text2[16] = {};
        if (showsText) IndicatorText2(hwnd, text2, _countof(text2));

        ScopedWindowDc win(hwnd, GetDC(hwnd));
        if (!win.valid()) return;
        ScopedMemDc mem(win.get());
        if (!mem.valid()) return;
        // The cell is composed in a 32-bit top-down DIB with a REAL alpha channel,
        // then copied with BitBlt SRCCOPY: per Microsoft's glass guidance, GDI
        // ignores alpha and BitBlt(SRCCOPY) is the one call that keeps it. Zero
        // pixels (alpha 0) are the transparent background, so the glass shows
        // through; the letters get their own alpha, which is what lets DARK text
        // be visible on a light taskbar (GDI dark text alone has alpha 0 and
        // vanishes, white only survives because it adds light).
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;                          // top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        ScopedGdiObj bmp((HGDIOBJ)CreateDIBSection(win.get(), &bi, DIB_RGB_COLORS, &bits, nullptr, 0));
        if (!bmp.get() || !bits) return;
        ScopedSelectedObject bmpSel(mem.get(), bmp.get());
        memset(bits, 0, (size_t)w * (size_t)h * 4);

        LOGFONTW lf = {};
        lf.lfHeight = -((h * 28) / 100);                    // two lines: language over layout
        lf.lfWeight = FW_NORMAL;
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfQuality = ANTIALIASED_QUALITY;                 // grey smoothing: coverage = alpha
        lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
        wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Segoe UI");
        if (showsText) {
            ScopedGdiObj font((HGDIOBJ)CreateFontIndirectW(&lf));
            if (font.get()) {
                ScopedSelectedObject fontSel(mem.get(), font.get());
                SetBkMode(mem.get(), TRANSPARENT);
                SetTextColor(mem.get(), RGB(255, 255, 255));   // mask only: coverage ends up in the channels
                TEXTMETRICW tm = {};
                GetTextMetricsW(mem.get(), &tm);
                const int lineH = tm.tmHeight > 0 ? tm.tmHeight : 12;
                const int nLines = text2[0] ? 2 : 1;
                const int y0 = rc.top + (h - lineH * nLines) / 2;
                RECT r1 = { rc.left, y0, rc.right, y0 + lineH };
                DrawTextW(mem.get(), text, -1, &r1, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX);
                if (nLines == 2) {
                    RECT r2 = { rc.left, y0 + lineH, rc.right, y0 + 2 * lineH };
                    DrawTextW(mem.get(), text2, -1, &r2, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX);
                }
            }
            // Mask -> premultiplied ARGB in the theme colour (dark on the light
            // theme, white on the dark one).
            const COLORREF fg = IndicatorTextColor(bg);
            const unsigned fr = GetRValue(fg), fgr = GetGValue(fg), fb = GetBValue(fg);
            DWORD* px = (DWORD*)bits;
            const size_t count = (size_t)w * (size_t)h;
            for (size_t i = 0; i < count; i++) {
                const DWORD v = px[i];
                const unsigned r = (v >> 16) & 0xFF, g = (v >> 8) & 0xFF, b = v & 0xFF;
                unsigned a = r > g ? r : g;
                if (b > a) a = b;
                if (a == 0) { px[i] = 0; continue; }
                px[i] = ((DWORD)a << 24) |
                        ((DWORD)((fr * a + 127) / 255) << 16) |
                        ((DWORD)((fgr * a + 127) / 255) << 8) |
                        (DWORD)((fb * a + 127) / 255);
            }
        }

        BitBlt(win.get(), 0, 0, w, h, mem.get(), 0, 0, SRCCOPY);

        // Schedule the read-back: it says whether this paint is what the screen
        // really shows, or whether another window sits on top of this one.
        if (showsText && g_indicatorCheckLogs < kIndicatorCheckLimit) {
            RECT wr = {};
            if (GetWindowRect(hwnd, &wr)) {
                g_indicatorCheck.centre.x = (wr.left + wr.right) / 2;
                g_indicatorCheck.centre.y = (wr.top + wr.bottom) / 2;
                g_indicatorCheck.expected = bg;
                g_indicatorCheck.due = GetTickCount() + 250;
                g_indicatorCheck.layer = hwnd;
                wcsncpy_s(g_indicatorCheck.cls, _countof(g_indicatorCheck.cls), cls, _TRUNCATE);
                g_indicatorCheck.pending = true;
            }
        }

        g_indicatorCellPaints++;
        if (g_indicatorCellLogs < 8) {
            g_indicatorCellLogs++;
            RECT wr = {};
            GetWindowRect(hwnd, &wr);
            Wh_Log(L"[lang] cell repainted (%s): %s 0x%p %dx%d at (%d,%d), background 0x%06X from %s, text %s, hover %s",
                   why, cls, (void*)hwnd, w, h, wr.left, wr.top,
                   (unsigned)bg & 0xFFFFFFu, how,
                   showsText ? text : L"(none: a layer above carries it)", hover ? L"yes" : L"no");
        }
    } catch (...) {
        Wh_Log(L"[lang] exception while painting the indicator cell");
    }
}

// Reads the cell back: is it still the classic face grey? Used only by the
// safety net, never to decide the colour to paint.
static bool IndicatorCellShowsGrey(HWND hwnd) {
    RECT rc = {};
    if (!GetClientRect(hwnd, &rc)) return false;
    const int w = rc.right;
    const int h = rc.bottom;
    if (w < 6 || h < 6 || w > 96 || h > 96) return false;
    ScopedWindowDc dc(hwnd, GetDC(hwnd));
    if (!dc.valid()) return false;
    const COLORREF face = GetSysColor(COLOR_3DFACE);
    const POINT pts[5] = { { 1, 1 }, { w - 2, 1 }, { 1, h - 2 }, { w - 2, h - 2 }, { w / 2, h / 2 } };
    int grey = 0;
    for (int i = 0; i < 5; i++) {
        const COLORREF c = GetPixel(dc.get(), pts[i].x, pts[i].y);
        if (c == CLR_INVALID) continue;
        if (ColorDistance(c, face) <= 60) grey++;
    }
    return grey >= 3;
}

// Five parameters, as WindhawkUtils::SetWindowSubclassFromAnyThread expects
// (the old six-parameter form of 0.3.7 did not compile). The reference data is
// the subclass id the mod passed when it installed this procedure.
static LRESULT IndicatorSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                    DWORD_PTR refData) {
    (void)refData;
    try {
        switch (msg) {
        case WM_LBUTTONDOWN: case WM_LBUTTONUP:
        case WM_MBUTTONUP:
            NoteManualLanguageOpen(L"click on the indicator");
            break;

        case WM_RBUTTONDOWN:
            // The right click on the indicator must not open the small indicator
            // menu: it is forwarded to the Shell_TrayWnd, which shows its OWN native
            // context menu (the same one the taskbar shows).
            NoteManualLanguageOpen(L"right click on the indicator");
            {
                HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
                if (tray) {
                    POINT pt = {};
                    GetCursorPos(&pt);   // the cursor is on the indicator, inside the taskbar
                    LPARAM screenPoint = MAKELPARAM((SHORT)pt.x, (SHORT)pt.y);
                    Wh_Log(L"[lang] right click on the indicator: routing to the taskbar menu at (%d,%d)",
                           (int)pt.x, (int)pt.y);
                    // PostMessage: the indicator's thread is not blocked.
                    // WM_CONTEXTMENU with wParam = Shell_TrayWnd itself.
                    PostMessageW(tray, WM_CONTEXTMENU, (WPARAM)tray, screenPoint);
                } else {
                    Wh_Log(L"[lang] Shell_TrayWnd not found: falling back to the native indicator menu");
                    // Fallback: let the shell show the indicator's own menu.
                    return DefSubclassProc(hwnd, msg, wParam, lParam);
                }
            }
            return 0;   // the indicator's own menu must not appear

        case WM_RBUTTONUP:
            // The down was already handled: nothing to do, the menu is already on its way
            // from the message forwarded to the taskbar.
            return 0;

        case WM_ERASEBKGND:
            // The grey is born here (class brush, or the theme part the child
            // window asks for) - claim the message. The paint below always
            // fills the whole client area, so nothing shows through and the
            // window never flickers.
            g_indicatorErases++;
            return 1;
        case WM_PAINT:
        case WM_PRINTCLIENT: {
            // The original first (explorer draws its glyphs, and the button
            // window blits its buffered paint), then the mod's cell on top.
            const LRESULT r = DefSubclassProc(hwnd, msg, wParam, lParam);
            g_indicatorPaints++;
            PaintIndicatorCell(hwnd, msg == WM_PAINT ? L"paint" : L"printclient");
            return r;
        }
        default:
            break;
        }
    } catch (...) {
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);   // nessun messaggio assorbito
}
// --- 10-ter) measuring the grey band ----------------------------------------
// It is there to understand WHO draws the rectangle: the real colour of the pixels
// inside and around the indicator window, which window is under each point
// (it may be another one: another mod's flyout, the taskbar, the tray) and from
// which module the window procedure of the class comes. A few lines are enough,
// and only once every 12 seconds.
static void ModuleOfAddress(const void* addr, wchar_t* buf, size_t count) {
    if (!buf || count == 0) return;
    buf[0] = 0;
    HMODULE m = nullptr;
    if (addr && GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   (LPCWSTR)addr, &m) && m)
        GetModuleFileNameW(m, buf, (DWORD)count);
}

static void LogIndicatorDiagnosis(HWND hwnd) {
    const DWORD now = GetTickCount();
    if (g_indicatorDiagLogs >= 8) return;
    if (g_lastIndicatorDiag && now - g_lastIndicatorDiag < 12000) return;
    g_lastIndicatorDiag = now;
    g_indicatorDiagLogs++;

    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, _countof(cls));
    RECT rc = {};
    GetWindowRect(hwnd, &rc);
    RECT cr = {};
    GetClientRect(hwnd, &cr);

    const int idx = TrackedIndicatorIndex(hwnd);
    const bool ours = idx >= 0 && g_indicatorTargets[idx].subclassed;

    wchar_t proc[MAX_PATH] = {};
    ModuleOfAddress((const void*)GetClassLongPtrW(hwnd, GCLP_WNDPROC), proc, _countof(proc));
    const wchar_t* procName = wcsrchr(proc, L'\\');

    Wh_Log(L"[lang] measure: %s 0x%p %dx%d at (%d,%d), client %dx%d, visible %s, our subclass %s, class proc from %s",
           cls[0] ? cls : L"window", (void*)hwnd,
           rc.right - rc.left, rc.bottom - rc.top, rc.left, rc.top,
           cr.right - cr.left, cr.bottom - cr.top, IsWindowVisible(hwnd) ? L"yes" : L"no",
           ours ? L"yes" : L"no", procName ? procName + 1 : L"unknown module");

    POINT punti[5] = { { rc.left + 2, rc.top + 2 }, { rc.right - 3, rc.top + 2 },
                       { rc.left + 2, rc.bottom - 3 }, { rc.right - 3, rc.bottom - 3 },
                       { (rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2 } };
    ScopedReleaseDc screen(GetDC(nullptr));
    if (!screen.valid()) return;

    unsigned colors[5] = { 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu };
    for (int i = 0; i < 5; i++) {
        const COLORREF c = GetPixel(screen.get(), punti[i].x, punti[i].y);
        if (c != CLR_INVALID) colors[i] = (unsigned)c & 0xFFFFFFu;
    }
    Wh_Log(L"[lang] measure: pixels 0x%06X 0x%06X 0x%06X 0x%06X, centre 0x%06X (paints %d, erases %d)",
           colors[0], colors[1], colors[2], colors[3], colors[4],
           g_indicatorPaints, g_indicatorErases);

    // What is underneath: the centre and the top-left corner are enough to tell
    // whether the grey belongs to the indicator window or to another window.
    const int quali[2] = { 0, 4 };
    for (int k = 0; k < 2; k++) {
        const POINT p = punti[quali[k]];
        HWND under = WindowFromPoint(p);
        wchar_t ucls[64] = {};
        wchar_t umod[MAX_PATH] = {};
        if (under) {
            GetClassNameW(under, ucls, _countof(ucls));
            ModuleOfAddress((const void*)GetClassLongPtrW(under, GCLP_WNDPROC), umod, _countof(umod));
        }
        const wchar_t* uName = wcsrchr(umod, L'\\');
        Wh_Log(L"[lang] measure: under (%d,%d) there is 0x%p %s from %s", p.x, p.y, (void*)under,
               ucls[0] ? ucls : L"no class", uName ? uName + 1 : L"unknown module");
    }
}

static bool IsIndicatorClass(const wchar_t* cls) {
    if (!cls || !cls[0]) return false;
    for (int i = 0; i < kIndicatorClassCount; i++)
        if (_wcsicmp(cls, kIndicatorClasses[i]) == 0) return true;
    return false;
}

static int TrackedIndicatorIndex(HWND w) {
    for (int i = 0; i < g_indicatorTargetCount; i++)
        if (g_indicatorTargets[i].wnd == w) return i;
    return -1;
}

static void UntrackIndicator(int idx) {
    if (idx < 0 || idx >= g_indicatorTargetCount) return;
    for (int i = idx; i + 1 < g_indicatorTargetCount; i++)
        g_indicatorTargets[i] = g_indicatorTargets[i + 1];
    g_indicatorTargets[--g_indicatorTargetCount] = IndicatorTarget{};
}

struct IndicatorEnumCtx {
    HWND found[8];
    int count;
};

static BOOL CALLBACK IndicatorEnumProc(HWND w, LPARAM param) {
    IndicatorEnumCtx* ctx = (IndicatorEnumCtx*)param;
    if (!ctx || ctx->count >= (int)(sizeof(ctx->found) / sizeof(ctx->found[0]))) return FALSE;
    wchar_t cls[64] = {};
    GetClassNameW(w, cls, _countof(cls));
    if (IsIndicatorClass(cls)) ctx->found[ctx->count++] = w;
    return TRUE;
}

// RAII around one subclass slot: the slot is cleared now (so the window ends up
// with a single copy of our procedure) and put back in the destructor, which
// runs even if an exception escapes in between. That is how the mod stays the
// last pass to paint even when another mod subclasses the same window later.
// Both calls use Windhawk's own procedure type (five parameters) and can be made
// from this thread even though the window belongs to the shell's thread; the
// outcome of the install is handed back to the caller, because there is no
// GetWindowSubclass that accepts this type.
class ScopedWindowSubclass {
public:
    ScopedWindowSubclass(HWND w, IndicatorSubclassFn proc, DWORD_PTR ref, bool* installed)
        : m_w(w), m_proc(proc), m_ref(ref), m_installed(installed) {
        if (m_installed) *m_installed = false;
        if (m_w) WindhawkUtils::RemoveWindowSubclassFromAnyThread(m_w, m_proc);
    }
    ~ScopedWindowSubclass() {
        if (!m_w) return;
        const bool ok = WindhawkUtils::SetWindowSubclassFromAnyThread(m_w, m_proc, m_ref);
        if (m_installed) *m_installed = ok;
    }
    ScopedWindowSubclass(const ScopedWindowSubclass&) = delete;
    ScopedWindowSubclass& operator=(const ScopedWindowSubclass&) = delete;
private:
    HWND m_w;
    IndicatorSubclassFn m_proc;
    DWORD_PTR m_ref;
    bool* m_installed;
};

// explorer creates the cell and its buttons at different times (the 18:14 log
// shows a first, empty instance of the cell before the real one, and the button
// appearing afterwards), so the scan runs on every tick and reports what it
// finds; while nothing is there it says so, so an absent cell never looks like a
// silent failure. The subclass is refreshed every ten seconds rather than every
// tick: enough to stay on top, cheap on the shell.
static void ArmIndicatorSubclass() {
    try {
        IndicatorEnumCtx ctx = {};
        HWND cell = FindInputIndicatorWindow();
        if (cell) ctx.found[ctx.count++] = cell;
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray) {
            // Children and grandchildren: the cell and its buttons are both
            // somewhere under the taskbar.
            EnumChildWindows(tray, IndicatorEnumProc, (LPARAM)&ctx);
        }
        if (!ctx.count) {
            static DWORD lastMiss = 0;
            const DWORD now0 = GetTickCount();
            if (!lastMiss || now0 - lastMiss > 30000) {
                lastMiss = now0;
                Wh_Log(L"[lang] input indicator window not found yet (class TrayInputIndicatorWClass)");
            }
            return;
        }

        bool added = false;

        // The tray indicator is a stack of windows, not a single one: explorer
        // creates the cell and, inside it or on top of it, other small windows
        // (the 18:14 log of 2026-10-01 shows a light grey 18x16 box with the big
        // "ITA" over a cell the mod had already painted blue). Those children
        // are the ones the user sees, so they are supervised too - whatever
        // their class is called, because the name is not what matters here, the
        // place is: a visible child, small, inside a supervised window.
        for (int i = 0; i < ctx.count; i++) {
            HWND parent = ctx.found[i];
            if (!parent || !IsWindow(parent)) continue;
            HWND child = nullptr;
            while ((child = FindWindowExW(parent, child, nullptr, nullptr)) != nullptr) {
                if (g_indicatorTargetCount >= (int)(sizeof(g_indicatorTargets) / sizeof(g_indicatorTargets[0])))
                    break;
                if (!IsWindowVisible(child)) continue;
                if (TrackedIndicatorIndex(child) >= 0) continue;
                RECT cr = {};
                if (!GetWindowRect(child, &cr)) continue;
                const int cw = cr.right - cr.left;
                const int chh = cr.bottom - cr.top;
                if (cw < 2 || chh < 2 || cw > 160 || chh > 160) continue;
                wchar_t ccls[64] = {};
                GetClassNameW(child, ccls, _countof(ccls));
                g_indicatorTargets[g_indicatorTargetCount].wnd = child;
                g_indicatorTargets[g_indicatorTargetCount].subclassed = false;
                g_indicatorTargetCount++;
                added = true;
                Wh_Log(L"[lang] indicator layer found (%s 0x%p, %dx%d): it is painted with the taskbar colour too",
                       ccls[0] ? ccls : L"window", (void*)child, cw, chh);
            }
        }

        for (int i = 0; i < ctx.count; i++) {
            HWND w = ctx.found[i];
            if (!w || !IsWindow(w)) continue;
            wchar_t cls[64] = {};
            GetClassNameW(w, cls, _countof(cls));
            if (!IsIndicatorClass(cls)) continue;            // recycled handle
            if (TrackedIndicatorIndex(w) >= 0) continue;
            if (g_indicatorTargetCount >= (int)(sizeof(g_indicatorTargets) / sizeof(g_indicatorTargets[0])))
                continue;
            g_indicatorTargets[g_indicatorTargetCount].wnd = w;
            g_indicatorTargets[g_indicatorTargetCount].subclassed = false;
            g_indicatorTargetCount++;
            added = true;
            Wh_Log(L"[lang] indicator window found (%s 0x%p): the mod paints it and its clicks pass through",
                   cls, (void*)w);
        }

        // Windows that went away are dropped, so the list never grows stale.
        for (int i = g_indicatorTargetCount - 1; i >= 0; i--) {
            wchar_t cls[64] = {};
            HWND w = g_indicatorTargets[i].wnd;
            if (!w || !IsWindow(w) || !GetClassNameW(w, cls, _countof(cls)) || !IsIndicatorClass(cls))
                UntrackIndicator(i);
        }

        const DWORD now = GetTickCount();
        // A window discovered just now is subclassed in the same tick, so the
        // next paint already comes from the mod.
        if (!added && now - g_lastIndicatorArm < 10000) return;
        g_lastIndicatorArm = now;

        for (int i = 0; i < g_indicatorTargetCount; i++) {
            HWND w = g_indicatorTargets[i].wnd;
            if (!w || !IsWindow(w)) continue;
            wchar_t cls[64] = {};
            GetClassNameW(w, cls, _countof(cls));
            bool installed = false;
            {
                // The destructor puts the subclass back and leaves the outcome
                // in "installed"; nothing else is done with the window here.
                ScopedWindowSubclass slot(w, IndicatorSubclassProc, kIndicatorSubclassId, &installed);
            }
            const bool first = !g_indicatorTargets[i].subclassed;
            g_indicatorTargets[i].subclassed = installed;
            if (installed && first) {
                Wh_Log(L"[lang] subclass installed on %s 0x%p (paint and clicks pass through here)",
                       cls, (void*)w);
            } else if (!installed) {
                static DWORD lastReport = 0;
                if (!lastReport || now - lastReport > 30000) {
                    lastReport = now;
                    Wh_Log(L"[lang] the shell did not accept the subclass on %s 0x%p, retrying",
                           cls, (void*)w);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[lang] exception while supervising the indicator windows");
    }
}

// The read-back scheduled by PaintIndicatorCell: what is on the screen at the
// centre of the indicator a moment after the paint.
static void IndicatorLayerCheckTick() {
    if (!g_indicatorCheck.pending || GetTickCount() < g_indicatorCheck.due) return;
    g_indicatorCheck.pending = false;
    ++g_indicatorCheckLogs;
    try {
        ScopedReleaseDc screen(GetDC(nullptr));
        HWND top = WindowFromPoint(g_indicatorCheck.centre);
        wchar_t tcls[64] = {};
        if (top) GetClassNameW(top, tcls, _countof(tcls));

        // The verdict is about the LAYER, not about the colour: the cell is painted with
        // a transparent background on purpose (see PaintIndicatorCell), so the pixel on
        // the screen is the taskbar, not the paint of the mod, and comparing the two
        // reported a mismatch every time even when nothing at all was wrong.
        const bool ours = top && (top == g_indicatorCheck.layer ||
                                  (g_indicatorCheck.layer && IsChild(g_indicatorCheck.layer, top)) ||
                                  TrackedIndicatorIndex(top) >= 0);
        const COLORREF found = screen.valid()
                                   ? GetPixel(screen.get(), g_indicatorCheck.centre.x,
                                              g_indicatorCheck.centre.y)
                                   : CLR_INVALID;

        // One line for the first check of the session; after that only a real surprise
        // (something that is not the indicator on top of it) is written, and at most
        // kIndicatorCheckLimit lines in total.
        const bool surprise = !ours;
        if (g_indicatorCheckLogs > 1 && !surprise) return;
        if (g_indicatorCheckLogs > kIndicatorCheckLimit) return;
        Wh_Log(L"[lang] layer check: %s 0x%p at (%d,%d): screen 0x%06X, %s; %s 0x%p is on top",
               g_indicatorCheck.cls, (void*)g_indicatorCheck.layer,
               g_indicatorCheck.centre.x, g_indicatorCheck.centre.y,
               (unsigned)found & 0xFFFFFFu,
               ours ? L"the transparent background of the cell belongs to the taskbar below it"
                    : L"our paint is not the window on top",
               tcls[0] ? tcls : L"nothing", (void*)top);
    } catch (...) {
        Wh_Log(L"[lang] exception while checking the painted layer");
    }
}

// The stray box: a small visible layer inside the language cell (not the cell,
// not InputIndicatorButton which carries the real letters) is hidden. Rects are
// logged so the layer can be identified if the box is still there.
static void HideStrayIndicatorLayers() {
    try {
        HWND cellWnd = FindInputIndicatorWindow();
        RECT cell = {};
        if (!cellWnd || !GetWindowRect(cellWnd, &cell)) return;
        const int cellArea = (cell.right - cell.left) * (cell.bottom - cell.top);
        for (int i = 0; i < g_indicatorTargetCount; i++) {
            HWND w = g_indicatorTargets[i].wnd;
            if (!w || w == cellWnd || !IsWindow(w)) continue;
            RECT r = {};
            if (!GetWindowRect(w, &r)) continue;
            wchar_t cls[64] = {};
            GetClassNameW(w, cls, _countof(cls));
            const bool vis = IsWindowVisible(w) != 0;
            const int area = (r.right - r.left) * (r.bottom - r.top);
            static int logs = 0;
            if (logs < 12) {
                logs++;
                Wh_Log(L"[lang] layer %s 0x%p rect (%d,%d)-(%d,%d) area %d of cell %d, visible %s",
                       cls, (void*)w, (int)r.left, (int)r.top, (int)r.right, (int)r.bottom,
                       area, cellArea, vis ? L"yes" : L"no");
            }
            if (_wcsicmp(cls, L"InputIndicatorButton") == 0) continue;   // carries the letters
            if (vis && area > 0 && area * 2 < cellArea) {
                ShowWindow(w, SW_HIDE);
                static int hides = 0;
                if (hides < 4) {
                    hides++;
                    Wh_Log(L"[lang] stray layer hidden: %s 0x%p", cls, (void*)w);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[lang] exception while hiding stray layers");
    }
}

static void LanguageGuardTick() {
    ArmIndicatorSubclass();
    IndicatorLayerCheckTick();
    HideStrayIndicatorLayers();
    for (int i = 0; i < g_indicatorTargetCount; i++) {
        HWND w = g_indicatorTargets[i].wnd;
        if (!w || !IsWindow(w)) continue;
        // Safety net for the case where no WM_PAINT ever reaches the subclass
        // (the tray paints the cell through the parent): repaint, but only when
        // the window still shows the face grey. There is no blind periodic
        // repaint and no forced RedrawWindow any more: a repaint firing on its
        // own was one more way to end up with the wrong colour in the cell.
        if (g_indicatorPaints == 0 || IndicatorCellShowsGrey(w))
            PaintIndicatorCell(w, L"safety net");
        LogIndicatorDiagnosis(w);
    }
    if (!g_langGuardUntil) return;
    if (GetTickCount() < g_langGuardUntil) {
        EnumWindows(LangCensusProc, 0);
        return;
    }
    // Final round at the end of the guard: if something is still
    // visible it is hidden now, and a summary line is written (the user
    // needs it to know that the guard worked and how much).
    if (!g_langEndLogged) {
        g_langEndLogged = true;
        EnumWindows(LangCensusProc, 0);
        Wh_Log(L"[language] guard finished: %d interventions", g_langSuppressions);
    }
}

// ---------------------------------------------------------------------------
// This module's own thread
//
// Every hook is installed from here, this thread does nothing else than tick, and
// the shell's own threads are never held up by the work of the module.
// ---------------------------------------------------------------------------
static HANDLE g_stopEvent = nullptr;
static HANDLE g_languageThread = nullptr;

// The guard: the four entry points by which a window can be shown.
static void InstallLanguageGuardHooks() {
    bool hookedShow = Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook,
                                        (void**)&ShowWindow_Original);
    bool hookedAsync = Wh_SetFunctionHook((void*)ShowWindowAsync, (void*)ShowWindowAsync_Hook,
                                         (void**)&ShowWindowAsync_Original);
    bool hookedCreate = Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                                          (void**)&CreateWindowExW_Original);
    bool hookedPos = Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook,
                                       (void**)&SetWindowPos_Original);
    Wh_Log(L"[language] guard hooks: ShowWindow %s, ShowWindowAsync %s, CreateWindowExW %s, "
           L"SetWindowPos %s",
           hookedShow ? L"installed" : L"not installed", hookedAsync ? L"installed" : L"not installed",
           hookedCreate ? L"installed" : L"not installed", hookedPos ? L"installed" : L"not installed");
}

// The indicator colours: the text pass and the bitmap pass of the tray.
static void InstallIndicatorHooks() {
    const bool text = Wh_SetFunctionHook((void*)ExtTextOutW, (void*)ExtTextOutW_Hook,
                                         (void**)&ExtTextOutW_Original);
    const bool bitmap = Wh_SetFunctionHook((void*)BitBlt, (void*)BitBlt_Hook,
                                           (void**)&BitBlt_Original);
    Wh_Log(L"[lang] indicator colour hooks: ExtTextOutW %s, BitBlt %s",
           text ? L"installed" : L"not installed", bitmap ? L"installed" : L"not installed");
}

static void ArmLanguageGuard() {
    g_langGuardStart = GetTickCount();
    g_langGuardUntil = g_langGuardStart + g_langGuardMs;
    g_langEndLogged = false;
    Wh_Log(L"[language] guard armed for %u seconds", (unsigned)(g_langGuardMs / 1000));
}

static void DisarmLanguageGuard() {
    g_langGuardUntil = 0;
    Wh_Log(L"[language] guard off (the indicator colours stay on)");
}

static DWORD WINAPI LanguageServicesThread(LPVOID) {
    MSG queueInit = {};
    PeekMessageW(&queueInit, nullptr, 0, 0, PM_NOREMOVE);
    for (;;) {
        if (g_unloading.load(std::memory_order_acquire)) break;
        if (WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) break;
        try {
            LanguageGuardTick();
        } catch (...) {
            Wh_Log(L"[lang] exception in the services thread: the shell is unaffected");
        }
    }
    return 0;
}

static void LoadLanguageSettings() {
    g_langGuardEnabled = Wh_GetIntSetting(L"LanguageGuard") != 0;
    g_langGuardMs = (DWORD)Wh_GetIntSetting(L"LanguageGuardSeconds") * 1000;
    if (g_langGuardMs < 2000) g_langGuardMs = 2000;          // below two seconds it is useless
    if (g_langGuardMs > 600000) g_langGuardMs = 600000;      // and ten minutes is the ceiling
    g_langGuardMsFromSettings = g_langGuardMs;
    g_indicatorColours = Wh_GetIntSetting(L"LanguageIndicatorColours") != 0;
    g_langCensusLog = Wh_GetIntSetting(L"LogLanguageGuard") != 0;
}

BOOL Wh_ModInit() {
    try {
        wchar_t exePath[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, exePath, _countof(exePath));
        if (g_realExePath[0] == 0) wcscpy_s(g_realExePath, exePath);

        Wh_Log(L"[lang] init: Windows 10 language flyout guard and indicator colours");
        Wh_Log(L"[lang] process: %s", exePath);

        if (!IsPrivateShellProcess()) {
            Wh_Log(L"[lang] this is not the private Windows 10 shell: nothing to do here");
            return TRUE;
        }

        LoadLanguageSettings();
        Wh_Log(L"[lang] settings: guard=%s (%u s), indicator colours=%s, logon census=%s",
               g_langGuardEnabled ? L"on" : L"off", (unsigned)(g_langGuardMs / 1000),
               g_indicatorColours ? L"on" : L"off", g_langCensusLog ? L"on" : L"off");

        // The guard hooks are installed even when the setting is off: they check it at
        // every call, so the guard can be switched while the shell runs.
        InstallLanguageGuardHooks();
        if (g_indicatorColours) InstallIndicatorHooks();
        else Wh_Log(L"[lang] the indicator colours are off: no drawing hook is installed");
        if (g_langGuardEnabled) ArmLanguageGuard();

        g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_stopEvent) {
            Wh_Log(L"[lang] the stop event could not be created");
            return TRUE;
        }
        g_languageThread = CreateThread(nullptr, 0, LanguageServicesThread, nullptr, 0, nullptr);
        if (!g_languageThread)
            Wh_Log(L"[lang] the services thread could not be created (%lu)", GetLastError());
        return TRUE;
    } catch (...) {
        Wh_Log(L"[lang] init exception: the shell is left as it is");
        return TRUE;
    }
}

void Wh_ModBeforeUninit() {
    // The thread is stopped here and the hooks are removed by Windhawk.
    g_unloading.store(true, std::memory_order_release);
    if (g_stopEvent) SetEvent(g_stopEvent);
}

void Wh_ModUninit() {
    if (g_languageThread) {
        WaitForSingleObject(g_languageThread, 3000);
        CloseHandle(g_languageThread);
        g_languageThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    Wh_Log(L"[lang] unloaded");
}

void Wh_ModSettingsChanged() {
    LoadLanguageSettings();
    if (g_langGuardEnabled) {
        if (!g_langGuardUntil) ArmLanguageGuard();
    } else if (g_langGuardUntil) {
        DisarmLanguageGuard();
    }
    Wh_Log(L"[lang] settings reloaded: guard=%s (%u s), indicator colours=%s, logon census=%s",
           g_langGuardEnabled ? L"on" : L"off", (unsigned)(g_langGuardMs / 1000),
           g_indicatorColours ? L"on" : L"off", g_langCensusLog ? L"on" : L"off");
}
