// ==WindhawkMod==
// @id              cursor-idle-fade
// @name            Cursor Idle Fade
// @description     Fades the mouse cursor out after a period of inactivity and brings it back the instant the mouse moves
// @version         1.0.0
// @author          Nuzza
// @github          https://github.com/Nuzza
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Cursor Idle Fade

When the mouse hasn't moved (or been clicked / scrolled) for the configured
number of seconds, the system cursors are smoothly faded out. The moment any
mouse activity happens, the cursors are restored immediately.

By default, the cursor will fade out completely after 10 seconds of inactivity.

## How it works
Windows has no "cursor opacity" setting, so the mod captures the current
system cursors, then swaps them for progressively more transparent copies
using `SetSystemCursor`. The mouse is polled (position + buttons) about every 15 ms; on activity (and
when the mod is unloaded) the original cursors are put back.

## Limitations
* Only the standard system cursors are faded (arrow, I-beam, hand, resize,
  cross, help, etc.). Apps that draw their own cursor (some games, some
  browsers/design tools) are not affected.
* The animated busy/working cursors are left alone on purpose.
* Mouse wheel scrolling on its own does not count as activity (movement and
  button presses do).
* Cursors that are purely monochrome (classic inverting style) are not faded.
* Restoring reloads your saved cursor scheme, so cursors changed temporarily
  by another tool via `SetSystemCursor` will revert to the saved scheme.

![Cursor Idle Fade Demo](https://i.imgur.com/g5EWkMU.gif)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- idleTimeout: 10
  $name: Idle time before fading starts (seconds)
- fadeDuration: 600
  $name: Fade duration (milliseconds)
- minOpacity: 0
  $name: Final opacity (percent, 0 = fully invisible)
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <algorithm>
#include <cstdint>
#include <vector>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
struct {
    int idleSeconds = 10;
    int fadeMs = 600;
    int minOpacity = 0;
} g_settings;

static void LoadSettings() {
    g_settings.idleSeconds = std::max(1, (int)Wh_GetIntSetting(L"idleTimeout"));
    g_settings.fadeMs = std::max(0, (int)Wh_GetIntSetting(L"fadeDuration"));
    g_settings.minOpacity =
        std::clamp((int)Wh_GetIntSetting(L"minOpacity"), 0, 100);
}

// ---------------------------------------------------------------------------
// Cursor snapshots
// ---------------------------------------------------------------------------

// OCR_* ids accepted by SetSystemCursor. WAIT (32514) and APPSTARTING (32650)
// are intentionally omitted because they are animated.
static const DWORD kCursorIds[] = {
    32512,  // OCR_NORMAL
    32513,  // OCR_IBEAM
    32515,  // OCR_CROSS
    32516,  // OCR_UP
    32642,  // OCR_SIZENWSE
    32643,  // OCR_SIZENESW
    32644,  // OCR_SIZEWE
    32645,  // OCR_SIZENS
    32646,  // OCR_SIZEALL
    32648,  // OCR_NO
    32649,  // OCR_HAND
    32651,  // OCR_HELP
};

struct CursorSnap {
    DWORD id = 0;
    bool valid = false;
    HCURSOR orig = nullptr;  // untouched copy, used to restore
    int w = 0, h = 0;
    DWORD xHot = 0, yHot = 0;
    std::vector<uint32_t> px;  // 32-bit BGRA, top-down
};

static std::vector<CursorSnap> g_snaps;

static BITMAPINFO MakeBitmapInfo(int w, int h) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
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
        if (GetObjectW(ii.hbmColor, sizeof(bm), &bm) && bm.bmWidth > 0 &&
            bm.bmHeight > 0) {
            int w = bm.bmWidth, h = bm.bmHeight;
            s.px.assign((size_t)w * h, 0);
            std::vector<uint32_t> mask((size_t)w * h, 0);

            HDC dc = GetDC(nullptr);
            BITMAPINFO bi1 = MakeBitmapInfo(w, h);
            BITMAPINFO bi2 = MakeBitmapInfo(w, h);
            int r1 = GetDIBits(dc, ii.hbmColor, 0, h, s.px.data(), &bi1,
                               DIB_RGB_COLORS);
            int r2 = GetDIBits(dc, ii.hbmMask, 0, h, mask.data(), &bi2,
                               DIB_RGB_COLORS);
            ReleaseDC(nullptr, dc);

            if (r1 == h && r2 == h) {
                bool anyAlpha = false;
                for (uint32_t p : s.px) {
                    if (p >> 24) {
                        anyAlpha = true;
                        break;
                    }
                }
                // Old-style color cursor without an alpha channel:
                // derive alpha from the AND mask (0 = opaque, 1 = transparent).
                if (!anyAlpha) {
                    for (size_t i = 0; i < s.px.size(); i++) {
                        if ((mask[i] & 0xFF) == 0)
                            s.px[i] |= 0xFF000000u;
                        else
                            s.px[i] = 0;
                    }
                }
                s.id = id;
                s.w = w;
                s.h = h;
                s.xHot = ii.xHotspot;
                s.yHot = ii.yHotspot;
                s.orig = CopyCursor(cur);
                ok = true;
            }
        }
    }

    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return ok;
}

// factor256: 0 = fully transparent, 256 = original.
static HCURSOR BuildCursor(const CursorSnap& s, int factor256) {
    BITMAPINFO bi = MakeBitmapInfo(s.w, s.h);
    void* bits = nullptr;
    HBITMAP color =
        CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!color || !bits) {
        if (color) DeleteObject(color);
        return nullptr;
    }

    uint32_t* dst = (uint32_t*)bits;
    for (size_t i = 0; i < s.px.size(); i++) {
        uint32_t p = s.px[i];
        // Scale every channel (cursor bitmaps are premultiplied).
        uint32_t a = (((p >> 24) & 0xFF) * factor256) >> 8;
        uint32_t r = (((p >> 16) & 0xFF) * factor256) >> 8;
        uint32_t g = (((p >> 8) & 0xFF) * factor256) >> 8;
        uint32_t b = ((p & 0xFF) * factor256) >> 8;
        dst[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }

    // All-ones AND mask: if Windows ignores alpha (everything fully
    // transparent), the mask path still renders the cursor as invisible.
    int rowBytes = ((s.w + 15) / 16) * 2;
    std::vector<BYTE> maskBits((size_t)rowBytes * s.h, 0xFF);
    HBITMAP mask = CreateBitmap(s.w, s.h, 1, 1, maskBits.data());

    HCURSOR result = nullptr;
    if (mask) {
        ICONINFO ii{};
        ii.fIcon = FALSE;
        ii.xHotspot = s.xHot;
        ii.yHotspot = s.yHot;
        ii.hbmMask = mask;
        ii.hbmColor = color;
        result = (HCURSOR)CreateIconIndirect(&ii);
        DeleteObject(mask);
    }
    DeleteObject(color);
    return result;
}

static int g_lastFactor = -1;

static void ApplyOpacity(double opacity) {
    int f = (int)(opacity * 256.0 + 0.5);
    f = std::clamp(f, 0, 256);
    if (f == g_lastFactor) return;
    g_lastFactor = f;
    for (const CursorSnap& s : g_snaps) {
        if (!s.valid) continue;
        HCURSOR c = BuildCursor(s, f);
        if (!c) continue;
        // On success the system takes ownership of the handle.
        if (!SetSystemCursor(c, s.id)) DestroyCursor(c);
    }
}

// ---------------------------------------------------------------------------
// State machine / worker thread
// ---------------------------------------------------------------------------
enum class State { Visible, Fading, Faded };

static State g_state = State::Visible;
static ULONGLONG g_lastActivity = 0;
static POINT g_lastPt = {0, 0};

static HANDLE g_thread = nullptr;
static HANDLE g_readyEvent = nullptr;
static DWORD g_threadId = 0;

static void FreeSnaps() {
    for (CursorSnap& s : g_snaps) {
        if (s.orig) DestroyCursor(s.orig);
    }
    g_snaps.clear();
}

static void RestoreCursors() {
    // Instant restore from the copies taken before fading...
    for (CursorSnap& s : g_snaps) {
        if (!s.valid || !s.orig) continue;
        HCURSOR c = CopyCursor(s.orig);
        if (c && !SetSystemCursor(c, s.id)) DestroyCursor(c);
    }
    // ...then reload the user's configured scheme to be safe.
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
    g_lastFactor = -1;
    g_state = State::Visible;
}

// Polls the mouse: true if it moved or any button is held.
static bool MouseActive() {
    bool active = false;
    POINT pt;
    if (GetCursorPos(&pt)) {
        if (pt.x != g_lastPt.x || pt.y != g_lastPt.y) active = true;
        g_lastPt = pt;
    }
    static const int keys[] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON,
                               VK_XBUTTON1, VK_XBUTTON2};
    for (int k : keys) {
        if (GetAsyncKeyState(k) & 0x8000) active = true;
    }
    return active;
}

static void OnTimer() {
    ULONGLONG now = GetTickCount64();

    if (MouseActive()) {
        g_lastActivity = now;
        if (g_state != State::Visible) RestoreCursors();
        return;
    }

    ULONGLONG idle = now - g_lastActivity;
    ULONGLONG timeout = (ULONGLONG)g_settings.idleSeconds * 1000ULL;
    if (idle < timeout) return;

    if (g_state == State::Visible) {
        FreeSnaps();
        for (DWORD id : kCursorIds) {
            CursorSnap s;
            s.valid = CaptureCursor(id, s);
            g_snaps.push_back(std::move(s));
        }
        g_lastFactor = -1;
        g_state = State::Fading;
    }

    if (g_state == State::Fading) {
        double t = 1.0;
        if (g_settings.fadeMs > 0) {
            t = std::min(1.0, (double)(idle - timeout) / g_settings.fadeMs);
        }
        double eased = t * t * (3.0 - 2.0 * t);  // smoothstep
        double minOp = g_settings.minOpacity / 100.0;
        ApplyOpacity(1.0 - eased * (1.0 - minOp));
        if (t >= 1.0) g_state = State::Faded;
    }
}

static DWORD WINAPI WorkerThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // make queue
    g_threadId = GetCurrentThreadId();

    GetCursorPos(&g_lastPt);
    g_lastActivity = GetTickCount64();

    UINT_PTR timer = SetTimer(nullptr, 0, 15, nullptr);
    if (!timer) Wh_Log(L"SetTimer failed: %u", GetLastError());
    SetEvent(g_readyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_TIMER) {
            OnTimer();
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (timer) KillTimer(nullptr, timer);
    if (g_state != State::Visible) RestoreCursors();
    FreeSnaps();
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"Cursor Idle Fade: init");
    LoadSettings();

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread || !g_readyEvent) return FALSE;

    WaitForSingleObject(g_readyEvent, 5000);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Cursor Idle Fade: uninit");
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}
