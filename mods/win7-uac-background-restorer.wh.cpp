// ==WindhawkMod==
// @id              win7-uac-background-restorer
// @name            Windows 7 UAC Background Restorer
// @description     This mod restores the Windows 7 UAC background: a dimmed screenshot of the whole screen (windows, Start menu, taskbar) instead of only the wallpaper
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         consent.exe
// @include         consentux.exe
// @compilerOptions -lmsimg32 -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 UAC Background Restorer

This mod restores the Windows 7 UAC background: a dimmed screenshot of
everything you were doing, instead of only the wallpaper shown on Windows 8 and
later. The **secure desktop** stays enabled (security is unchanged).

## How it works
1. When the UAC process starts it is still on the normal desktop: the mod
   captures the whole screen (windows, Start menu, taskbar).
2. When the UAC process composes its large opaque background (before the
   switch to the secure desktop), the mod draws the screenshot instead. The
   original dim (veil) is left untouched.

The background is a still image: the secure desktop cannot show live windows.

## Robustness
- All GDI resources are owned by RAII wrappers and released on every path.
- Every hook is wrapped in try/catch and always falls back to the original
  function. (C++ exceptions are caught; hardware faults cannot be.)
- Shared state is protected by a lock; the re-entrancy guard is per thread.
- Substitution is limited to a few draws per process (`maxSubstitutions`) so a
  repeated full-screen present can never permanently hide the prompt.

## Settings
- **logOnly**: nothing is altered; large drawing calls are logged.
- **replaceBackground**: enables the substitution.
- **extraDim** (0-200): additional black veil over the screenshot. Keep 0 if
  the original dim is still applied.
- **maxSubstitutions** (1-64): substitution budget per process.

## No symbols required
This mod does not depend on symbols: it only uses public GDI functions.

## Limitations
- Works only if the background is drawn through GDI (BitBlt, StretchBlt,
  AlphaBlend). If the prompt uses XAML/DirectComposition nothing is replaced.
- If the prompt ever appears blank, the substituted draw also contained the
  dialog: use logOnly, read the log and report it. The keyboard still works on
  the hidden dialog and the prompt times out safely; elevation is never granted
  automatically.
- Multi-monitor and high-DPI setups are handled approximately.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- logOnly: false
  $name: Diagnostic mode (log only)
  $description: This setting turns on diagnostic mode; when on, nothing is altered and large drawing calls are only written to the log.
- replaceBackground: true
  $name: Replace background with a full-screen screenshot
  $description: This setting replaces the wallpaper-only UAC background with a screenshot of the whole screen.
- extraDim: 0
  $name: Extra dim (0-200)
  $description: This setting adds an extra black veil over the screenshot; keep it at 0 if the original dim is still applied.
- maxSubstitutions: 8
  $name: Maximum substituted draws (1-64)
  $description: This setting limits how many background draws are replaced per UAC prompt process.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <wchar.h>

#include <algorithm>
#include <atomic>
#include <utility>

// -----------------------------------------------------------------------------
// RAII helpers
// -----------------------------------------------------------------------------

class SrwGuard {
public:
    explicit SrwGuard(SRWLOCK& lock) : lock_(lock) {
        AcquireSRWLockExclusive(&lock_);
    }
    ~SrwGuard() {
        ReleaseSRWLockExclusive(&lock_);
    }
    SrwGuard(const SrwGuard&) = delete;
    SrwGuard& operator=(const SrwGuard&) = delete;

private:
    SRWLOCK& lock_;
};

// Per-thread re-entrancy guard: our own drawing must not re-enter the hooks.
thread_local bool t_inHook = false;

class ReentrancyGuard {
public:
    ReentrancyGuard() : active_(!t_inHook) {
        if (active_) {
            t_inHook = true;
        }
    }
    ~ReentrancyGuard() {
        if (active_) {
            t_inHook = false;
        }
    }
    ReentrancyGuard(const ReentrancyGuard&) = delete;
    ReentrancyGuard& operator=(const ReentrancyGuard&) = delete;
    bool entered() const {
        return active_;
    }

private:
    bool active_;
};

// GetDC / ReleaseDC pair.
class ScopedScreenDc {
public:
    ScopedScreenDc() : dc_(GetDC(nullptr)) {}
    ~ScopedScreenDc() {
        if (dc_) {
            ReleaseDC(nullptr, dc_);
        }
    }
    ScopedScreenDc(const ScopedScreenDc&) = delete;
    ScopedScreenDc& operator=(const ScopedScreenDc&) = delete;
    HDC get() const {
        return dc_;
    }

private:
    HDC dc_;
};

// CreateCompatibleDC / DeleteDC pair.
class ScopedMemDc {
public:
    explicit ScopedMemDc(HDC ref) : dc_(CreateCompatibleDC(ref)) {}
    ~ScopedMemDc() {
        if (dc_) {
            DeleteDC(dc_);
        }
    }
    ScopedMemDc(const ScopedMemDc&) = delete;
    ScopedMemDc& operator=(const ScopedMemDc&) = delete;
    ScopedMemDc(ScopedMemDc&& o) noexcept : dc_(o.dc_) {
        o.dc_ = nullptr;
    }
    HDC get() const {
        return dc_;
    }
    HDC release() {
        HDC d = dc_;
        dc_ = nullptr;
        return d;
    }

private:
    HDC dc_;
};

class ScopedBitmap {
public:
    ScopedBitmap() = default;
    explicit ScopedBitmap(HBITMAP b) : bmp_(b) {}
    ~ScopedBitmap() {
        reset();
    }
    ScopedBitmap(const ScopedBitmap&) = delete;
    ScopedBitmap& operator=(const ScopedBitmap&) = delete;
    ScopedBitmap(ScopedBitmap&& o) noexcept : bmp_(o.bmp_) {
        o.bmp_ = nullptr;
    }
    ScopedBitmap& operator=(ScopedBitmap&& o) noexcept {
        if (this != &o) {
            reset();
            bmp_ = o.bmp_;
            o.bmp_ = nullptr;
        }
        return *this;
    }
    void reset() {
        if (bmp_) {
            DeleteObject(bmp_);
            bmp_ = nullptr;
        }
    }
    HBITMAP get() const {
        return bmp_;
    }
    explicit operator bool() const {
        return bmp_ != nullptr;
    }

private:
    HBITMAP bmp_ = nullptr;
};

// SelectObject with automatic restore.
class ScopedSelect {
public:
    ScopedSelect(HDC dc, HGDIOBJ obj) : dc_(dc), old_(SelectObject(dc, obj)) {}
    ~ScopedSelect() {
        if (dc_ && old_ && old_ != HGDI_ERROR) {
            SelectObject(dc_, old_);
        }
    }
    ScopedSelect(const ScopedSelect&) = delete;
    ScopedSelect& operator=(const ScopedSelect&) = delete;
    bool ok() const {
        return old_ && old_ != HGDI_ERROR;
    }

private:
    HDC dc_;
    HGDIOBJ old_;
};

// Saves/restores the DC state that the substitution changes.
class ScopedDcState {
public:
    explicit ScopedDcState(HDC dc) : dc_(dc) {
        mode_ = SetStretchBltMode(dc_, HALFTONE);
        hasOrg_ = GetBrushOrgEx(dc_, &org_) != FALSE;
        SetBrushOrgEx(dc_, 0, 0, nullptr);
    }
    ~ScopedDcState() {
        if (mode_) {
            SetStretchBltMode(dc_, mode_);
        }
        if (hasOrg_) {
            SetBrushOrgEx(dc_, org_.x, org_.y, nullptr);
        }
    }
    ScopedDcState(const ScopedDcState&) = delete;
    ScopedDcState& operator=(const ScopedDcState&) = delete;

private:
    HDC dc_;
    int mode_ = 0;
    POINT org_ = {0, 0};
    bool hasOrg_ = false;
};

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

static std::atomic<bool> g_logOnly{false};
static std::atomic<bool> g_replace{true};
static std::atomic<int> g_extraDim{0};
static std::atomic<int> g_maxSubstitutions{8};
static std::atomic<int> g_substituted{0};
static std::atomic<LONG> g_logCount{0};

static SRWLOCK g_snapLock = SRWLOCK_INIT;

// The captured screen. Everything below is protected by g_snapLock.
struct Snapshot {
    HDC dc = nullptr;           // Memory DC with the bitmap selected.
    HGDIOBJ oldBitmap = nullptr;
    ScopedBitmap bitmap;        // DIB section.
    int width = 0;
    int height = 0;
    RECT primary = {0, 0, 0, 0};  // Primary monitor rect inside the snapshot.

    void reset() {
        if (dc) {
            if (oldBitmap && oldBitmap != HGDI_ERROR) {
                SelectObject(dc, oldBitmap);
            }
            DeleteDC(dc);
        }
        dc = nullptr;
        oldBitmap = nullptr;
        bitmap.reset();
        width = height = 0;
        primary = {0, 0, 0, 0};
    }
    bool valid() const {
        return dc && bitmap;
    }
};

[[clang::no_destroy]] static Snapshot g_snap;

using StretchBlt_t = decltype(&StretchBlt);
using BitBlt_t = decltype(&BitBlt);
using AlphaBlend_t = decltype(&AlphaBlend);
using CreateWindowExW_t = decltype(&CreateWindowExW);
static StretchBlt_t StretchBlt_Original;
static BitBlt_t BitBlt_Original;
static AlphaBlend_t AlphaBlend_Original;
static CreateWindowExW_t CreateWindowExW_Original;

static bool ShouldLog() {
    return g_logCount.fetch_add(1, std::memory_order_relaxed) < 80;
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static bool IsOnDefaultDesktop() {
    HDESK desk = GetThreadDesktop(GetCurrentThreadId());
    wchar_t name[64] = L"";
    DWORD needed = 0;
    if (!desk || !GetUserObjectInformationW(desk, UOI_NAME, name, sizeof(name),
                                            &needed)) {
        return false;
    }
    return _wcsicmp(name, L"Default") == 0;
}

static bool IsLarge(int w, int h) {
    const int sw = GetSystemMetrics(SM_CXSCREEN);
    const int sh = GetSystemMetrics(SM_CYSCREEN);
    if (sw <= 0 || sh <= 0) {
        return false;
    }
    w = w < 0 ? -w : w;
    h = h < 0 ? -h : h;
    return w * 2 >= sw && h * 2 >= sh;
}

static bool Near(int a, int b) {
    const int d = a > b ? a - b : b - a;
    return d * 50 <= (b > 0 ? b : 1);  // Within 2%.
}

// Captures the whole virtual screen into g_snap. Runs while the process is still
// on the normal desktop, before the switch to the secure desktop.
static void CaptureScreen() {
    const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vw <= 0 || vh <= 0 || vw > 16384 || vh > 16384) {
        Wh_Log(L"CaptureScreen: invalid virtual screen size");
        return;
    }

    ScopedScreenDc screen;
    if (!screen.get()) {
        Wh_Log(L"CaptureScreen: GetDC failed (%lu)", GetLastError());
        return;
    }

    // A DIB section does not depend on the display device state, unlike a
    // device-dependent bitmap.
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = vw;
    bi.bmiHeader.biHeight = -vh;  // Top-down.
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    ScopedBitmap bitmap(
        CreateDIBSection(screen.get(), &bi, DIB_RGB_COLORS, &bits, nullptr, 0));
    if (!bitmap) {
        Wh_Log(L"CaptureScreen: CreateDIBSection failed (%lu)", GetLastError());
        return;
    }

    ScopedMemDc mem(screen.get());
    if (!mem.get()) {
        Wh_Log(L"CaptureScreen: CreateCompatibleDC failed");
        return;
    }

    HGDIOBJ old = SelectObject(mem.get(), bitmap.get());
    if (!old || old == HGDI_ERROR) {
        Wh_Log(L"CaptureScreen: SelectObject failed");
        return;
    }

    if (!BitBlt(mem.get(), 0, 0, vw, vh, screen.get(), vx, vy,
                SRCCOPY | CAPTUREBLT)) {
        SelectObject(mem.get(), old);
        Wh_Log(L"CaptureScreen: BitBlt failed (%lu)", GetLastError());
        return;
    }

    SrwGuard lock(g_snapLock);
    g_snap.reset();
    g_snap.dc = mem.release();
    g_snap.oldBitmap = old;
    g_snap.bitmap = std::move(bitmap);
    g_snap.width = vw;
    g_snap.height = vh;
    g_snap.primary = {-vx, -vy, -vx + GetSystemMetrics(SM_CXSCREEN),
                      -vy + GetSystemMetrics(SM_CYSCREEN)};
    Wh_Log(L"CaptureScreen: captured %dx%d", vw, vh);
}

// Draws the screenshot over the destination instead of a large opaque
// background. Returns true when the substitution was drawn.
static bool TrySubstitute(HDC hdcDest, int x, int y, int w, int h) {
    if (!hdcDest || g_logOnly.load() || !g_replace.load() ||
        !StretchBlt_Original) {
        return false;
    }
    if (g_substituted.load() >= g_maxSubstitutions.load()) {
        return false;
    }
    // No desktop check: the background is composed into a memory bitmap while
    // the process is still on the normal desktop (verified in the log).
    ReentrancyGuard guard;
    if (!guard.entered()) {
        return false;
    }

    const int aw = w < 0 ? -w : w;
    const int ah = h < 0 ? -h : h;

    SrwGuard lock(g_snapLock);
    if (!g_snap.valid()) {
        return false;
    }

    int sx, sy, sw, sh;
    if (Near(aw, g_snap.width) && Near(ah, g_snap.height)) {
        sx = 0;
        sy = 0;
        sw = g_snap.width;
        sh = g_snap.height;
    } else if (Near(aw, g_snap.primary.right - g_snap.primary.left) &&
               Near(ah, g_snap.primary.bottom - g_snap.primary.top)) {
        sx = g_snap.primary.left;
        sy = g_snap.primary.top;
        sw = g_snap.primary.right - g_snap.primary.left;
        sh = g_snap.primary.bottom - g_snap.primary.top;
    } else {
        return false;
    }

    BOOL ok;
    {
        ScopedDcState state(hdcDest);
        ok = StretchBlt_Original(hdcDest, x, y, w, h, g_snap.dc, sx, sy, sw, sh,
                                 SRCCOPY);
    }
    if (!ok) {
        return false;
    }

    const int dim = std::max(0, std::min(200, g_extraDim.load()));
    if (dim > 0 && AlphaBlend_Original) {
        ScopedMemDc veil(hdcDest);
        if (veil.get()) {
            ScopedBitmap vb(CreateCompatibleBitmap(hdcDest, 1, 1));
            if (vb) {
                ScopedSelect sel(veil.get(), vb.get());
                if (sel.ok()) {
                    RECT one = {0, 0, 1, 1};
                    FillRect(veil.get(), &one,
                             static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
                    BLENDFUNCTION bf = {AC_SRC_OVER, 0,
                                        static_cast<BYTE>(dim), 0};
                    AlphaBlend_Original(hdcDest, x, y, w, h, veil.get(), 0, 0, 1,
                                        1, bf);
                }
            }
        }
    }

    const int count = g_substituted.fetch_add(1) + 1;
    if (ShouldLog()) {
        Wh_Log(L"Substituted background %dx%d (%d)", aw, ah, count);
    }
    return true;
}

// -----------------------------------------------------------------------------
// Hooks: every one falls back to the original function on any failure.
// -----------------------------------------------------------------------------

static BOOL WINAPI StretchBlt_Hook(HDC hdcDest, int xDest, int yDest, int wDest,
                                   int hDest, HDC hdcSrc, int xSrc, int ySrc,
                                   int wSrc, int hSrc, DWORD rop) {
    try {
        if (!t_inHook && IsLarge(wDest, hDest)) {
            if (ShouldLog()) {
                Wh_Log(L"StretchBlt %dx%d rop=0x%08X", wDest, hDest, rop);
            }
            if (rop == SRCCOPY &&
                TrySubstitute(hdcDest, xDest, yDest, wDest, hDest)) {
                return TRUE;
            }
        }
    } catch (...) {
        Wh_Log(L"StretchBlt_Hook: exception");
    }
    return StretchBlt_Original(hdcDest, xDest, yDest, wDest, hDest, hdcSrc, xSrc,
                               ySrc, wSrc, hSrc, rop);
}

static BOOL WINAPI BitBlt_Hook(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc,
                               int x1, int y1, DWORD rop) {
    try {
        if (!t_inHook && IsLarge(cx, cy)) {
            if (ShouldLog()) {
                Wh_Log(L"BitBlt %dx%d rop=0x%08X", cx, cy, rop);
            }
            if (rop == SRCCOPY && TrySubstitute(hdc, x, y, cx, cy)) {
                return TRUE;
            }
        }
    } catch (...) {
        Wh_Log(L"BitBlt_Hook: exception");
    }
    return BitBlt_Original(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop);
}

static BOOL WINAPI AlphaBlend_Hook(HDC hdcDest, int xDest, int yDest, int wDest,
                                   int hDest, HDC hdcSrc, int xSrc, int ySrc,
                                   int wSrc, int hSrc, BLENDFUNCTION ftn) {
    try {
        if (!t_inHook && IsLarge(wDest, hDest)) {
            if (ShouldLog()) {
                Wh_Log(L"AlphaBlend %dx%d alpha=%u fmt=%u", wDest, hDest,
                       ftn.SourceConstantAlpha, ftn.AlphaFormat);
            }
            // Only an opaque blit can be the wallpaper; a partial alpha is the
            // veil and is deliberately left untouched.
            if (ftn.SourceConstantAlpha == 255 && ftn.AlphaFormat == 0 &&
                TrySubstitute(hdcDest, xDest, yDest, wDest, hDest)) {
                return TRUE;
            }
        }
    } catch (...) {
        Wh_Log(L"AlphaBlend_Hook: exception");
    }
    return AlphaBlend_Original(hdcDest, xDest, yDest, wDest, hDest, hdcSrc, xSrc,
                               ySrc, wSrc, hSrc, ftn);
}

static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR cls, LPCWSTR name,
                                        DWORD style, int x, int y, int w, int h,
                                        HWND parent, HMENU menu, HINSTANCE inst,
                                        LPVOID param) {
    try {
        if (IsLarge(w, h) && ShouldLog()) {
            // Class atoms (< 0x10000) are not strings: never dereference them.
            if (cls && reinterpret_cast<ULONG_PTR>(cls) >= 0x10000) {
                Wh_Log(L"CreateWindowExW class=%s %dx%d ex=0x%08X", cls, w, h,
                       exStyle);
            } else {
                Wh_Log(L"CreateWindowExW (atom) %dx%d ex=0x%08X", w, h,
                       exStyle);
            }
        }
    } catch (...) {
        Wh_Log(L"CreateWindowExW_Hook: exception");
    }
    return CreateWindowExW_Original(exStyle, cls, name, style, x, y, w, h, parent,
                                    menu, inst, param);
}

// -----------------------------------------------------------------------------
// Mod lifecycle
// -----------------------------------------------------------------------------

static void LoadSettings() {
    g_logOnly = Wh_GetIntSetting(L"logOnly") != 0;
    g_replace = Wh_GetIntSetting(L"replaceBackground") != 0;
    g_extraDim = Wh_GetIntSetting(L"extraDim");
    int maxSubs = Wh_GetIntSetting(L"maxSubstitutions");
    g_maxSubstitutions = std::max(1, std::min(64, maxSubs));
}

BOOL Wh_ModInit() {
    try {
        LoadSettings();

        const bool onDefault = IsOnDefaultDesktop();
        Wh_Log(L"Started: logOnly=%d replace=%d extraDim=%d max=%d onDefault=%d",
               g_logOnly.load() ? 1 : 0, g_replace.load() ? 1 : 0,
               g_extraDim.load(), g_maxSubstitutions.load(), onDefault ? 1 : 0);

        // The screenshot is only possible before the switch to the secure
        // desktop.
        if (onDefault && g_replace.load() && !g_logOnly.load()) {
            CaptureScreen();
        } else if (!onDefault) {
            Wh_Log(L"Already on a non-default desktop: no screenshot available");
        }

        Wh_SetFunctionHook(reinterpret_cast<void*>(&StretchBlt),
                           reinterpret_cast<void*>(StretchBlt_Hook),
                           reinterpret_cast<void**>(&StretchBlt_Original));
        Wh_SetFunctionHook(reinterpret_cast<void*>(&BitBlt),
                           reinterpret_cast<void*>(BitBlt_Hook),
                           reinterpret_cast<void**>(&BitBlt_Original));
        Wh_SetFunctionHook(reinterpret_cast<void*>(&AlphaBlend),
                           reinterpret_cast<void*>(AlphaBlend_Hook),
                           reinterpret_cast<void**>(&AlphaBlend_Original));
        Wh_SetFunctionHook(reinterpret_cast<void*>(&CreateWindowExW),
                           reinterpret_cast<void*>(CreateWindowExW_Hook),
                           reinterpret_cast<void**>(&CreateWindowExW_Original));
        return TRUE;
    } catch (...) {
        Wh_Log(L"Wh_ModInit: exception");
        return FALSE;
    }
}

void Wh_ModSettingsChanged() {
    try {
        LoadSettings();
        Wh_Log(L"Settings changed: logOnly=%d replace=%d extraDim=%d max=%d",
               g_logOnly.load() ? 1 : 0, g_replace.load() ? 1 : 0,
               g_extraDim.load(), g_maxSubstitutions.load());
    } catch (...) {
        Wh_Log(L"Wh_ModSettingsChanged: exception");
    }
}

void Wh_ModUninit() {
    try {
        SrwGuard lock(g_snapLock);
        g_snap.reset();
        Wh_Log(L"Unloaded");
    } catch (...) {
    }
}
