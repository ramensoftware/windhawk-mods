// ==WindhawkMod==
// @id              cursor-idle-fade
// @name            Cursor Idle Fade
// @description     Fades the mouse cursor out after a period of inactivity and brings it back the instant the mouse moves
// @version         1.0.0
// @author          Nuzza
// @github          https://github.com/Nuzza
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32
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
using `SetSystemCursor`. Mouse activity (movement, clicks and scrolling) is
detected with raw input, so nothing is polled while the cursor is visible.
While the cursor is faded, the mouse is also checked every 50 ms (see
Limitations).
On activity the original cursors are put back with `SetSystemCursor` (which
updates the cursor in place, so apps that cache cursor handles pick it up
immediately). When the mod is unloaded the saved cursor scheme is also
reloaded with `SPI_SETCURSORS`.

The mod runs in its own dedicated `windhawk.exe` process (a Windhawk "tool
mod").

## Limitations
* Only the standard system cursors are faded (arrow, I-beam, hand, resize,
  cross, help, etc.). Apps that draw their own cursor (some games, some
  browsers/design tools) are not affected.
* The animated busy/working cursors are left alone on purpose.
* Cursors that are purely monochrome (classic inverting style) are not faded.
* The faded copies are single-size bitmaps. With mixed-DPI monitors or a
  larger pointer size, the cursor may change size while it fades and snap
  back when restored.
* Restoring puts back copies of the cursors captured right before the fade.
  A custom animated (.ani) cursor assigned to one of the faded cursor types
  may come back as a static frame until the cursor scheme is reloaded (the
  scheme is reloaded when the mod is unloaded).
* Windows doesn't deliver raw mouse input to a normal (non-elevated) process
  while an app running as administrator has focus (Task Manager, for
  example). In that case the mod falls back to checking the cursor position
  and mouse buttons (about 5 times a second): the cursor still reappears
  when you move or click, but scrolling alone isn't noticed. The mod notices
  which app has focus and switches back to raw input by itself.
* If the cursor is faded while a secure screen (UAC prompt, lock screen)
  takes the input, it is restored automatically.

## Recovery
If the mod's process is killed while the cursor is faded, the mod restores
the cursors the next time it starts. You can also re-apply your cursor scheme
in *Settings > Bluetooth & devices > Mouse > Additional mouse settings >
Pointers*.

![Cursor Fade Demo](https://i.imgur.com/lt8H6B0.gif)

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
#include <shellapi.h>
#include <stdio.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <vector>

// ---------------------------------------------------------------------------
// Settings (written by the Windhawk thread, read by the worker thread)
// ---------------------------------------------------------------------------
struct {
    std::atomic<int> idleSeconds{10};
    std::atomic<int> fadeMs{600};
    std::atomic<int> minOpacity{0};
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
    HCURSOR orig = nullptr;  // untouched copy, used to restore in place
    // true: color channels are independent of alpha (scale alpha only).
    // false: color channels are premultiplied (scale all channels).
    bool straightAlpha = false;
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
                bool straight = false;
                for (uint32_t p : s.px) {
                    uint32_t a = p >> 24;
                    if (a) anyAlpha = true;
                    // A channel brighter than alpha can't be premultiplied.
                    if (((p >> 16) & 0xFF) > a || ((p >> 8) & 0xFF) > a ||
                        (p & 0xFF) > a) {
                        straight = true;
                    }
                }
                if (!anyAlpha) {
                    // Old-style color cursor without an alpha channel:
                    // derive alpha from the AND mask (0 = opaque).
                    for (size_t i = 0; i < s.px.size(); i++) {
                        if ((mask[i] & 0xFF) == 0)
                            s.px[i] |= 0xFF000000u;
                        else
                            s.px[i] = 0;
                    }
                    straight = true;
                }
                s.straightAlpha = straight;
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
        uint32_t a = (((p >> 24) & 0xFF) * factor256) >> 8;
        if (a == 0) {
            // Fully transparent: zero everything so it can't be XOR-drawn
            // if Windows falls back to the mask path.
            dst[i] = 0;
        } else if (s.straightAlpha) {
            dst[i] = (a << 24) | (p & 0x00FFFFFFu);
        } else {
            uint32_t r = (((p >> 16) & 0xFF) * factor256) >> 8;
            uint32_t g = (((p >> 8) & 0xFF) * factor256) >> 8;
            uint32_t b = ((p & 0xFF) * factor256) >> 8;
            dst[i] = (a << 24) | (r << 16) | (g << 8) | b;
        }
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
// State machine (runs only on the worker thread)
// ---------------------------------------------------------------------------
//
// Visible: one idle timer is armed; nothing else runs.
// Fading:  a ~15 ms timer steps the opacity down.
// Faded:   a slow watchdog timer checks that the input desktop is still ours.
//
// Mouse activity arrives as WM_INPUT (raw input, sink mode).

enum class State { Visible, Fading, Faded };

enum : UINT_PTR { TIMER_IDLE = 1, TIMER_FADE = 2, TIMER_WATCH = 3 };
enum : UINT { WM_APP_SETTINGS = WM_APP + 1 };

static const wchar_t kWindowClass[] = L"CursorIdleFadeMessageWindow";
static const wchar_t kModifiedFlag[] = L"cursorsModified";

static State g_state = State::Visible;
static ULONGLONG g_lastActivity = 0;
static ULONGLONG g_fadeStart = 0;
static POINT g_lastPt = {0, 0};

static const UINT kWatchIntervalMs = 50;
static const UINT kPolledIdleIntervalMs = 200;

// Raw input bookkeeping, to notice when Windows isn't delivering it to us.
static ULONGLONG g_rawSeq = 0;           // raw mouse reports received
static ULONGLONG g_rawSeqAtLastPoll = 0;  // value at the last idle check
// True while the cursor is seen moving without any raw input arriving.
static bool g_rawMissing = false;
// True while the foreground app is probably more privileged than this
// process (Windows then doesn't deliver raw input to us).
static bool g_fgBlocked = false;
static HWINEVENTHOOK g_fgHook = nullptr;

// While either is true, the idle check runs every kPolledIdleIntervalMs
// instead of once per idle period, so the idle time stays accurate.
static bool PolledMode() {
    return g_rawMissing || g_fgBlocked;
}

static std::atomic<HWND> g_hwnd{nullptr};
static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_readyEvent = nullptr;
static bool g_workerOk = false;

static ULONGLONG IdleTimeoutMs() {
    return (ULONGLONG)g_settings.idleSeconds.load() * 1000ULL;
}

static void ArmIdleTimer(HWND hwnd, ULONGLONG ms) {
    if (PolledMode()) ms = std::min<ULONGLONG>(ms, kPolledIdleIntervalMs);
    ms = std::clamp<ULONGLONG>(ms, USER_TIMER_MINIMUM, 0x7FFFFFFFULL);
    SetTimer(hwnd, TIMER_IDLE, (UINT)ms, nullptr);
}

// Re-arms the idle timer for the time left until the idle timeout.
static void RearmIdle(HWND hwnd) {
    ULONGLONG timeout = IdleTimeoutMs();
    ULONGLONG idle = GetTickCount64() - g_lastActivity;
    ArmIdleTimer(hwnd, idle >= timeout ? 1 : timeout - idle);
}

// Raw input is not delivered to a process while a window of a more
// privileged process (e.g. an elevated Task Manager) has focus. A normal
// process can't open the token of an elevated one, which is used as the
// signal. Failing to inspect the process counts as "blocked": that only
// makes the mod check the mouse more often.
static bool ForegroundMayBlockRawInput() {
    HWND fg = GetForegroundWindow();
    if (!fg) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    if (!pid || pid == GetCurrentProcessId()) return false;

    HANDLE proc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!proc) return true;

    HANDLE token = nullptr;
    bool blocked = false;
    if (OpenProcessToken(proc, TOKEN_QUERY, &token)) {
        CloseHandle(token);
    } else {
        blocked = true;
    }
    CloseHandle(proc);
    return blocked;
}

static void CALLBACK ForegroundChanged(HWINEVENTHOOK, DWORD, HWND, LONG, LONG,
                                       DWORD, DWORD) {
    bool blocked = ForegroundMayBlockRawInput();
    if (blocked == g_fgBlocked) return;
    g_fgBlocked = blocked;

    HWND hwnd = g_hwnd.load();
    if (hwnd && g_state == State::Visible) RearmIdle(hwnd);
}

static void FreeSnaps() {
    for (CursorSnap& s : g_snaps) {
        if (s.orig) DestroyCursor(s.orig);
    }
    g_snaps.clear();
}

// Puts the original cursors back and clears the "modified" flag.
//
// The cursors are restored with SetSystemCursor, like the fade itself: it
// swaps the image in place, keeps the HCURSOR handles apps have cached valid,
// and refreshes the cursor that is currently on screen. Reloading the scheme
// with SPI_SETCURSORS alone does not refresh the on-screen cursor for apps
// that only call SetCursor when their cursor changes, so the faded cursor
// would stay invisible until another window took focus.
//
// reloadScheme additionally reloads the saved scheme (used on shutdown), and
// is the fallback if any cursor could not be put back.
static void RestoreCursorsNow(bool reloadScheme) {
    bool allRestored = true;
    for (CursorSnap& s : g_snaps) {
        if (!s.valid) continue;
        HCURSOR c = s.orig ? CopyCursor(s.orig) : nullptr;
        if (!c || !SetSystemCursor(c, s.id)) {
            if (c) DestroyCursor(c);
            allRestored = false;
        }
    }
    if (reloadScheme || !allRestored) {
        SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
    }
    Wh_SetIntValue(kModifiedFlag, 0);
    FreeSnaps();
    g_lastFactor = -1;
    g_state = State::Visible;
}

// Checks the mouse directly, without raw input. GetCursorPos and
// GetAsyncKeyState work no matter which app has focus, unlike raw input,
// which Windows doesn't deliver to us while an elevated app is in the
// foreground (UIPI).
//
// Returns false if the input desktop isn't ours (UAC prompt, lock screen).
// Otherwise returns true and sets *active if the cursor moved since the last
// check or a mouse button is down.
static void OnActivity(HWND hwnd);

static bool PollMouse(bool* active) {
    POINT pt;
    if (!GetCursorPos(&pt)) return false;

    bool a = pt.x != g_lastPt.x || pt.y != g_lastPt.y;
    g_lastPt = pt;

    static const int keys[] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON,
                               VK_XBUTTON1, VK_XBUTTON2};
    for (int k : keys) {
        if (GetAsyncKeyState(k) & 0x8000) a = true;
    }
    *active = a;
    return true;
}

static void Restore(HWND hwnd) {
    KillTimer(hwnd, TIMER_FADE);
    KillTimer(hwnd, TIMER_WATCH);
    RestoreCursorsNow(false);
    g_lastActivity = GetTickCount64();
    GetCursorPos(&g_lastPt);
    ArmIdleTimer(hwnd, IdleTimeoutMs());
}

static void StartFade(HWND hwnd) {
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

    // Remember that the cursors are modified, in case this process dies.
    Wh_SetIntValue(kModifiedFlag, 1);

    g_lastFactor = -1;
    g_fadeStart = GetTickCount64();
    g_state = State::Fading;
    SetTimer(hwnd, TIMER_FADE, 15, nullptr);
}

static void OnFadeTick(HWND hwnd) {
    bool active = false;
    if (!PollMouse(&active)) {
        // A secure desktop (UAC, lock screen) took the input.
        Restore(hwnd);
        return;
    }
    if (active) {
        OnActivity(hwnd);
        return;
    }

    int fadeMs = g_settings.fadeMs.load();
    double t = 1.0;
    if (fadeMs > 0) {
        t = std::min(1.0, (double)(GetTickCount64() - g_fadeStart) / fadeMs);
    }
    double eased = t * t * (3.0 - 2.0 * t);  // smoothstep
    double minOp = g_settings.minOpacity.load() / 100.0;
    ApplyOpacity(1.0 - eased * (1.0 - minOp));

    if (t >= 1.0) {
        KillTimer(hwnd, TIMER_FADE);
        g_state = State::Faded;
        SetTimer(hwnd, TIMER_WATCH, kWatchIntervalMs, nullptr);
    }
}

static void OnIdleTimer(HWND hwnd) {
    KillTimer(hwnd, TIMER_IDLE);
    if (g_state != State::Visible) return;

    ULONGLONG timeout = IdleTimeoutMs();

    bool active = false;
    if (!PollMouse(&active)) {
        // Input desktop isn't ours (UAC, lock screen): don't fade now.
        ArmIdleTimer(hwnd, timeout);
        return;
    }

    // Raw input doesn't reach us while an elevated app has focus. If the
    // cursor moved but no raw input arrived since the last check, track the
    // mouse by polling (at a fine interval, so the idle time stays accurate).
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
        ArmIdleTimer(hwnd, timeout - idle);  // activity happened meanwhile
        return;
    }

    StartFade(hwnd);
}

// Runs while faded. Raw input normally restores the cursor; this covers the
// cases where it can't (elevated foreground app, secure desktop).
static void OnWatchTimer(HWND hwnd) {
    bool active = false;
    if (!PollMouse(&active)) {
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
    if (GetRawInputData(hri, RID_INPUT, &ri, &size, sizeof(RAWINPUTHEADER)) !=
        (UINT)-1) {
        if (ri.header.dwType == RIM_TYPEMOUSE) {
            const RAWMOUSE& m = ri.data.mouse;
            bool empty = !(m.usFlags & MOUSE_MOVE_ABSOLUTE) && m.lLastX == 0 &&
                         m.lLastY == 0 && m.usButtonFlags == 0;
            if (empty) return;  // no-op report, not real activity
        }
    }
    OnActivity(hwnd);
}

static void OnSettingsMessage(HWND hwnd) {
    switch (g_state) {
        case State::Visible:
            RearmIdle(hwnd);
            break;
        case State::Faded:
            g_lastFactor = -1;
            ApplyOpacity(g_settings.minOpacity.load() / 100.0);
            break;
        case State::Fading:
            break;  // the next tick uses the new values
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
    switch (msg) {
        case WM_INPUT:
            OnRawInput(hwnd, (HRAWINPUT)lParam);
            break;  // DefWindowProc must still run for WM_INPUT cleanup

        case WM_TIMER:
            switch (wParam) {
                case TIMER_IDLE:
                    OnIdleTimer(hwnd);
                    break;
                case TIMER_FADE:
                    OnFadeTick(hwnd);
                    break;
                case TIMER_WATCH:
                    OnWatchTimer(hwnd);
                    break;
            }
            return 0;

        case WM_APP_SETTINGS:
            OnSettingsMessage(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Worker thread
// ---------------------------------------------------------------------------
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
        hwnd = CreateWindowExW(0, kWindowClass, L"", 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, inst, nullptr);
    }
    if (hwnd) {
        RAWINPUTDEVICE rid{};
        rid.usUsagePage = 0x01;  // generic desktop
        rid.usUsage = 0x02;      // mouse
        rid.dwFlags = RIDEV_INPUTSINK;
        rid.hwndTarget = hwnd;
        rawInputRegistered = RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }

    if (!hwnd || !rawInputRegistered) {
        Wh_Log(L"Setup failed: class=%d hwnd=%d rawinput=%d (error %u)",
               (int)classRegistered, hwnd != nullptr, (int)rawInputRegistered,
               GetLastError());
        g_workerOk = false;
        if (hwnd) DestroyWindow(hwnd);
        if (classRegistered) UnregisterClassW(kWindowClass, inst);
        SetEvent(g_readyEvent);
        return 0;
    }

    g_lastActivity = GetTickCount64();
    GetCursorPos(&g_lastPt);
    g_fgBlocked = ForegroundMayBlockRawInput();
    g_fgHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                               nullptr, ForegroundChanged, 0, 0,
                               WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_fgHook) Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
    ArmIdleTimer(hwnd, IdleTimeoutMs());
    g_hwnd = hwnd;
    g_workerOk = true;
    SetEvent(g_readyEvent);

    for (;;) {
        DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE,
                                            QS_ALLINPUT | QS_RAWINPUT);
        if (r != WAIT_OBJECT_0 + 1) break;  // stop event (or failure)

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
    if (g_state != State::Visible) RestoreCursorsNow(true);
    FreeSnaps();

    DestroyWindow(hwnd);
    UnregisterClassW(kWindowClass, inst);
    return 0;
}

// ---------------------------------------------------------------------------
// Tool mod callbacks
// ---------------------------------------------------------------------------
BOOL WhTool_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    // Recover from an unclean exit that left the cursors faded.
    if (Wh_GetIntValue(kModifiedFlag, 0)) {
        SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
        Wh_SetIntValue(kModifiedFlag, 0);
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

    // The worker always signals this event, on success and on failure.
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

void WhTool_ModSettingsChanged() {
    LoadSettings();
    HWND hwnd = g_hwnd.load();
    if (hwnd) PostMessageW(hwnd, WM_APP_SETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L">");
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
