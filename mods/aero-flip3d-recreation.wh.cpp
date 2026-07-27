// ==WindhawkMod==
// @id              aero-flip3d-recreation
// @name            Aero Flip 3D Recreation
// @description     This mod recreates the Windows Vista/7 Aero Flip 3D for modern Windows versions
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -luser32 -lgdi32 -lmsimg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Aero Flip 3D Recreation

This mod recreates the Windows Vista/7 **Flip 3D** window switcher on Windows 10/11.
The mod is based on live DWM thumbnails and only public, documented Windows APIs and system files are not modified.
Note: This is a first version so it might not be 100% accurate but it will improve over time with feedback.
The mod has been tested on Windows 10 1809.

## Keyboard shortcuts

| Shortcut | Action |
| --- | --- |
| `Win+Tab` | Open Flip 3D. Press `Tab` to cycle through windows; keep `Win` held to auto-cycle like the original. **Releasing `Win` activates** the selected window. |
| `Ctrl+Win+Tab` | Open Flip 3D in **persistent mode**: the switcher stays open after you release the keys. |
| `Ctrl+G` or `Ctrl+Alt+F12` | Toggle the switcher on/off (persistent mode). Useful fallback if `Win+Tab` is taken by another app. |

### While the switcher is open

- **Navigate**: `Tab`, the arrow keys, or the mouse wheel.
- **Confirm**: `Enter`, `Space`, or click.
- **Cancel**: `Esc`.
- **Emergency exit**: `Ctrl+Shift+Esc` always reaches Task Manager.

## Settings

- **Realistic perspective cards** (`usePerspective`, on by default): deform each
  card into a perspective trapezoid (Windows 7-like). Uncheck it for the
  original **flat** 2.5D card cascade. The change applies live if the switcher
  is open.

## Requirements

- Windows 10 (64 bit) or Windows 11 (64 bit)
- DWM composition enabled (on by default)

## Notes

- The switcher is drawn on the **primary monitor**.
- The native `Alt+Tab` switcher is never intercepted.
- While the mod is enabled it owns the `Win+Tab` hotkey, replacing the modern
  Task View. Disable or uninstall the mod to get Task View back.


*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- usePerspective: true
  $name: Realistic perspective cards
  $description: If this setting is enabled, the mod deforms each card into a perspective trapezoid using live thumbnail strips (Windows 7-like). If disabled, the mod uses the original simplified flat 2.5D card cascade (scale, offset and opacity, no perspective).
- verboseLogging: false
  $name: Verbose debug logging
  $description: This setting logs detailed step-by-step diagnostics (window enumeration, thumbnail registration, hook install steps, layout computation) to the Windhawk log. Disable it for normal use; turn it on only when troubleshooting since it is more verbose than the default logging.
*/
// ==/WindhawkModSettings==

// -----------------------------------------------------------------------------
// Defensive rewrite (hardened build)
//
// This mod runs inside explorer.exe, a critical process: any unhandled fault
// restarts the shell. The code follows an aggressive defensive strategy:
//
//  1. RAII everywhere: HDC, HBITMAP, HBRUSH, HHOOK, HANDLE, HTHUMBNAIL and
//     BeginPaint/EndPaint are owned by small move-only wrapper classes, so
//     resources are released on every exit path, including stack unwinding.
//  2. Aggressive validation: every HWND is checked with IsWindow() before use,
//     every pointer with a null check, every Win32/DWM return value with an
//     explicit test before its result is trusted.
//  3. No exception escapes a Windows boundary: every callback invoked by the
//     system (WndProcs, low-level hooks, thread proc, EnumWindows callbacks,
//     Wh_ModInit/Wh_ModUninit) is wrapped in try/catch. On failure the mod
//     logs, cleans up and degrades gracefully instead of crashing Explorer.
//  4. Minimal logging: Wh_Log is used only for init/uninit, unrecoverable
//     failures and caught exceptions — never for per-frame or per-key events.
//  5. Non-blocking init: Wh_ModInit starts a background hook thread and waits
//     at most 200 ms, so Explorer's watchdog can never restart the shell.
//  6. Isolated components with fallbacks: if the desktop snapshot, the
//     wallpaper bitmap or DWM thumbnails fail, the backdrop degrades to a
//     plain Aero gradient and the mod keeps working.
//  7. Resource-pressure guards: GDI object budget and free physical memory
//     are checked before allocating full-screen bitmaps; the window deck is
//     capped; thumbnails whose source window zombifies are dropped after a
//     small number of failed DWM updates; failure logs are rate-limited; the
//     overlay has a safe-destroy fallback.
//
// Only public, documented APIs are used: DwmRegisterThumbnail,
// DwmUpdateThumbnailProperties, DwmUnregisterThumbnail, EnumWindows,
// RegisterHotKey and low-level input hooks.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// How the REAL Flip 3D worked (from the decompiled Windows 7 RTM dwmapi.dll)
//
// dwmapi.dll was only an RPC client: the Flip 3D engine lived inside the DWM
// compositor process (dwm.exe, managed by the uxsms service).
//
// - Toggling Flip 3D: exported ordinal #105 (the famous
//   `rundll32 dwmapi #105` trick) simply sends ALPC message 0x4000002B to
//   dwm.exe, with no window arguments. dwm.exe then rendered the 3D card
//   stack itself, with true perspective transforms — which is exactly why it
//   cannot be replicated 1:1 with public APIs: DWM thumbnails only support a
//   destination rect + opacity, so this mod draws a pseudo-3D stack instead.
// - Thumbnail APIs used by this mod and their ALPC message IDs:
//     DwmRegisterThumbnail          -> 0x4000001D (destination must be a
//                                        top-level window owned by the
//                                        calling process — our overlay —
//                                        source may be any top-level window;
//                                        HTHUMBNAIL is a client-local ID)
//     DwmUpdateThumbnailProperties  -> 0x4000001E (45-byte property payload;
//                                        the real DWM rejects flags == 0 and
//                                        inverted rects — right < left or
//                                        bottom < top — with E_INVALIDARG;
//                                        ApplyThumbnailProperties mirrors
//                                        this validation exactly)
//     DwmUnregisterThumbnail        -> 0x4000001F
//     DwmQueryThumbnailSourceSize   -> 0x40000021
// - Every thumbnail update is one LPC round-trip to dwm.exe: that is why
//   updates are batched once per animation frame and z-order rebuilds happen
//   only when the selection changes, never per-frame.
// - The Flip 3D policy attribute was DWMWA_FLIP3D_POLICY (public id 11,
//   internal DWM attribute 17 per the attribute translation table).
//   DWMWA_CLOAKED (14) did not exist on Windows 7 — it is a Windows 8+
//   attribute, used here only to skip cloaked UWP windows on modern systems.
//
// The Windows 10/11 DWM no longer implements message 0x4000002B (and
// Windhawk itself requires Windows 10+), so this mod re-implements the
// switcher inside explorer.exe using only the still-supported thumbnail
// messages listed above.
//
// Perspective approximation: since a thumbnail can only be scaled/moved
// (no affine or perspective transforms), each card's live content is
// registered up to kPerspectiveStripCount times (fewer for cards deep in
// the stack, none beyond depth 3), each thumbnail cropped to a vertical
// source strip via DWM_TNP_RECTSOURCE and mapped onto a destination strip
// whose height follows the trapezoid of a card rotated around a vertical
// axis. The tilt amount is animated and grows with stack depth (front
// card nearly flat, deep cards angled — CoverFlow-style), so the morphs
// are seamless and the large front card shows no visible strip
// staircase. If DWM rejects the strips, the card falls back to a single
// flat thumbnail. All of this stays within public, documented APIs.
// -----------------------------------------------------------------------------

#include <windows.h>
#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdarg>
#include <utility>
#include <setjmp.h>
#include <vector>

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif

#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

#ifndef CAPTUREBLT
#define CAPTUREBLT 0x40000000
#endif

#ifndef WPF_RESTORETOMAXIMIZED
#define WPF_RESTORETOMAXIMIZED 0x0002
#endif

#ifndef GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#endif

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

// -----------------------------------------------------------------------------
// Minimal, essential logging (Wh_Log)
//
// Rule: log only init/uninit, unrecoverable failures and caught exceptions.
// Never log per-frame animation, per-key input or per-thumbnail updates.
// -----------------------------------------------------------------------------

static void LogNote(PCWSTR what) {
    Wh_Log(L"[AeroFlip3D] %s", what);
}

// Failure logs are rate-limited to at most one per second: a recurring fault
// (e.g. an exception in a WM_PAINT handler) must not flood the log or burn
// CPU inside a low-level hook, where the system enforces a response timeout.
static std::atomic<ULONGLONG> g_lastFailureLogTick{0};

static void LogFailure(PCWSTR what) {
    ULONGLONG now = GetTickCount64();
    ULONGLONG last = g_lastFailureLogTick.load(std::memory_order_relaxed);
    if (last && now - last < 1000) {
        return;  // Throttled.
    }
    g_lastFailureLogTick.store(now, std::memory_order_relaxed);
    Wh_Log(L"[AeroFlip3D] %s failed", what);
}

static void LogFailureCode(PCWSTR what, DWORD code) {
    Wh_Log(L"[AeroFlip3D] %s failed (code %u)", what, static_cast<unsigned>(code));
}

// Verbose, opt-in diagnostics (the "verboseLogging" setting). These are the
// logs to turn on when troubleshooting a specific problem (mod not
// activating, hook install failing, thumbnails not registering, layout
// looking wrong, etc.) — they are deliberately noisier than LogNote/
// LogFailure and are off by default so normal use stays quiet.
static std::atomic<bool> g_verboseLogging{false};

static void LogDebug(PCWSTR what) {
    if (!g_verboseLogging.load(std::memory_order_relaxed)) {
        return;
    }
    Wh_Log(L"[AeroFlip3D][debug] %s", what);
}

static void LogDebugFmt(PCWSTR fmt, ...) {
    if (!g_verboseLogging.load(std::memory_order_relaxed)) {
        return;
    }
    wchar_t buf[512];
    va_list args;
    va_start(args, fmt);
    _vsnwprintf_s(buf, _countof(buf), _TRUNCATE, fmt, args);
    va_end(args);
    Wh_Log(L"[AeroFlip3D][debug] %s", buf);
}

// -----------------------------------------------------------------------------
// RAII wrappers for every system resource the mod touches
// -----------------------------------------------------------------------------

// DWM thumbnail handle. Unregisters the thumbnail on destruction.
struct ThumbnailHandle {
    HTHUMBNAIL value = nullptr;

    ThumbnailHandle() = default;
    ~ThumbnailHandle() {
        reset();
    }

    ThumbnailHandle(const ThumbnailHandle&) = delete;
    ThumbnailHandle& operator=(const ThumbnailHandle&) = delete;

    ThumbnailHandle(ThumbnailHandle&& other) noexcept : value(other.value) {
        other.value = nullptr;
    }

    ThumbnailHandle& operator=(ThumbnailHandle&& other) noexcept {
        if (this != &other) {
            reset();
            value = other.value;
            other.value = nullptr;
        }
        return *this;
    }

    void reset(HTHUMBNAIL newValue = nullptr) {
        if (value) {
            DwmUnregisterThumbnail(value);
        }
        value = newValue;
    }

    HTHUMBNAIL get() const {
        return value;
    }

    HTHUMBNAIL* put() {
        reset();
        return &value;
    }

    explicit operator bool() const {
        return value != nullptr;
    }
};

// HBITMAP owner. Deletes the bitmap on destruction.
class ScopedBitmap {
public:
    ScopedBitmap() = default;
    explicit ScopedBitmap(HBITMAP bitmap) : bitmap_(bitmap) {}

    ~ScopedBitmap() {
        reset();
    }

    ScopedBitmap(const ScopedBitmap&) = delete;
    ScopedBitmap& operator=(const ScopedBitmap&) = delete;

    ScopedBitmap(ScopedBitmap&& other) noexcept : bitmap_(other.bitmap_) {
        other.bitmap_ = nullptr;
    }

    ScopedBitmap& operator=(ScopedBitmap&& other) noexcept {
        if (this != &other) {
            reset();
            bitmap_ = other.bitmap_;
            other.bitmap_ = nullptr;
        }
        return *this;
    }

    void reset(HBITMAP newBitmap = nullptr) {
        if (bitmap_) {
            DeleteObject(bitmap_);
        }
        bitmap_ = newBitmap;
    }

    HBITMAP get() const {
        return bitmap_;
    }

    HBITMAP* put() {
        reset();
        return &bitmap_;
    }

    // Gives up ownership (caller becomes responsible for DeleteObject).
    HBITMAP release() {
        HBITMAP bitmap = bitmap_;
        bitmap_ = nullptr;
        return bitmap;
    }

    explicit operator bool() const {
        return bitmap_ != nullptr;
    }

private:
    HBITMAP bitmap_ = nullptr;
};

// Device context owner. Knows whether it must ReleaseDC (GetDC) or
// DeleteDC (CreateCompatibleDC) and does the right thing on destruction.
class ScopedDc {
public:
    ScopedDc() = default;

    ~ScopedDc() {
        reset();
    }

    ScopedDc(const ScopedDc&) = delete;
    ScopedDc& operator=(const ScopedDc&) = delete;

    ScopedDc(ScopedDc&& other) noexcept
        : dc_(other.dc_), owner_(other.owner_), kind_(other.kind_) {
        other.dc_ = nullptr;
        other.owner_ = nullptr;
    }

    ScopedDc& operator=(ScopedDc&& other) noexcept {
        if (this != &other) {
            reset();
            dc_ = other.dc_;
            owner_ = other.owner_;
            kind_ = other.kind_;
            other.dc_ = nullptr;
            other.owner_ = nullptr;
        }
        return *this;
    }

    // Wraps GetDC(hwnd). Pass nullptr for the whole screen DC.
    static ScopedDc fromScreen(HWND hwnd = nullptr) {
        ScopedDc dc;
        dc.owner_ = hwnd;
        dc.kind_ = Kind::Release;
        dc.dc_ = GetDC(hwnd);
        return dc;
    }

    // Wraps CreateCompatibleDC(reference).
    static ScopedDc compatible(HDC reference) {
        ScopedDc dc;
        dc.kind_ = Kind::Delete;
        dc.dc_ = CreateCompatibleDC(reference);
        return dc;
    }

    void reset() {
        if (!dc_) {
            return;
        }
        if (kind_ == Kind::Delete) {
            DeleteDC(dc_);
        } else {
            ReleaseDC(owner_, dc_);
        }
        dc_ = nullptr;
        owner_ = nullptr;
    }

    HDC get() const {
        return dc_;
    }

    explicit operator bool() const {
        return dc_ != nullptr;
    }

private:
    enum class Kind { Release, Delete };

    HDC dc_ = nullptr;
    HWND owner_ = nullptr;
    Kind kind_ = Kind::Release;
};

// Restores the previous GDI object selected into a DC when leaving scope.
template <typename T>
class ScopedSelectObject {
public:
    ScopedSelectObject(HDC hdc, T object)
        : hdc_(hdc), previous_(SelectObject(hdc, object)) {}

    ~ScopedSelectObject() {
        if (hdc_ && previous_ && previous_ != HGDI_ERROR) {
            SelectObject(hdc_, previous_);
        }
    }

    ScopedSelectObject(const ScopedSelectObject&) = delete;
    ScopedSelectObject& operator=(const ScopedSelectObject&) = delete;

    bool succeeded() const {
        return previous_ != nullptr && previous_ != HGDI_ERROR;
    }

private:
    HDC hdc_;
    HGDIOBJ previous_;
};

// Solid brush owner.
class ScopedSolidBrush {
public:
    explicit ScopedSolidBrush(COLORREF color) : brush_(CreateSolidBrush(color)) {}

    ~ScopedSolidBrush() {
        if (brush_) {
            DeleteObject(brush_);
        }
    }

    ScopedSolidBrush(const ScopedSolidBrush&) = delete;
    ScopedSolidBrush& operator=(const ScopedSolidBrush&) = delete;

    HBRUSH get() const {
        return brush_;
    }

    explicit operator bool() const {
        return brush_ != nullptr;
    }

private:
    HBRUSH brush_ = nullptr;
};

// WH_KEYBOARD_LL / WH_MOUSE_LL hook owner.
class ScopedHook {
public:
    ScopedHook() = default;

    ~ScopedHook() {
        reset();
    }

    ScopedHook(const ScopedHook&) = delete;
    ScopedHook& operator=(const ScopedHook&) = delete;

    void reset(HHOOK newHook = nullptr) {
        if (hook_) {
            UnhookWindowsHookEx(hook_);
        }
        hook_ = newHook;
    }

    HHOOK get() const {
        return hook_;
    }

    explicit operator bool() const {
        return hook_ != nullptr;
    }

private:
    HHOOK hook_ = nullptr;
};

// Generic kernel HANDLE owner (events, threads).
class ScopedHandle {
public:
    ScopedHandle() = default;
    explicit ScopedHandle(HANDLE handle) : handle_(handle) {}

    ~ScopedHandle() {
        reset();
    }

    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;

    ScopedHandle(ScopedHandle&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    ScopedHandle& operator=(ScopedHandle&& other) noexcept {
        if (this != &other) {
            reset();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    void reset(HANDLE newHandle = nullptr) {
        if (handle_ && handle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(handle_);
        }
        handle_ = newHandle;
    }

    HANDLE get() const {
        return handle_;
    }

    HANDLE* put() {
        reset();
        return &handle_;
    }

    explicit operator bool() const {
        return handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE;
    }

private:
    HANDLE handle_ = nullptr;
};

// BeginPaint/EndPaint pair. Guarantees EndPaint runs even if painting throws,
// so a fault can never leave the window in a WM_PAINT storm.
class ScopedPaint {
public:
    explicit ScopedPaint(HWND hwnd) : hwnd_(hwnd) {
        hdc_ = BeginPaint(hwnd_, &ps_);
    }

    ~ScopedPaint() {
        if (hdc_) {
            EndPaint(hwnd_, &ps_);
        }
    }

    ScopedPaint(const ScopedPaint&) = delete;
    ScopedPaint& operator=(const ScopedPaint&) = delete;

    HDC get() const {
        return hdc_;
    }

    explicit operator bool() const {
        return hdc_ != nullptr;
    }

private:
    HWND hwnd_;
    PAINTSTRUCT ps_ = {};
    HDC hdc_ = nullptr;
};

// Runs a cleanup callable exactly once when leaving scope, even on exception
// unwind. The cleanup itself is protected so it can never throw out.
template <class Fn>
class ScopeGuard {
public:
    explicit ScopeGuard(Fn fn) : fn_(std::move(fn)) {}

    ~ScopeGuard() {
        try {
            fn_();
        } catch (...) {
            // Cleanup must never propagate.
        }
    }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

private:
    Fn fn_;
};

template <class Fn>
static ScopeGuard<Fn> MakeScopeGuard(Fn fn) {
    return ScopeGuard<Fn>(std::move(fn));
}

// Runs fn(); if any exception escapes, logs `what` and returns failValue.
// Used to keep exceptions from crossing system boundaries.
template <typename R, typename Fn>
static R SafeExec(PCWSTR what, R failValue, Fn&& fn) {
    try {
        return fn();
    } catch (...) {
        LogFailure(what);
        return failValue;
    }
}

// -----------------------------------------------------------------------------
// Anti-crash "fault fence" (public Win32 APIs: AddVectoredExceptionHandler,
// TlsAlloc/GetValue/SetValue, longjmp, kernel32 file I/O).
//
// Why this exists: the mod runs inside explorer.exe, so any UNHANDLED hardware
// fault (access violation, illegal instruction, divide-by-zero, ...) on a
// thread we own, or inside a Windows callback we installed, terminates the
// shell. The C++ try/catch blocks elsewhere only catch *thrown* exceptions;
// whether they also catch hardware faults depends on the compiler's exception
// model, which we cannot control here. A Vectored Exception Handler combined
// with setjmp/longjmp catches hardware faults regardless of that model.
//
// Design (kept deliberately conservative so it can never disturb explorer or
// other mods):
//   - A per-thread stack of "fences" (setjmp buffers + a label) is kept in TLS.
//   - Each system boundary (WndProcs, low-level hooks, thread proc, the
//     ActivateFlip3D path, EnumWindows callbacks, Wh_ModInit/Uninit) opens a
//     fence with FLIP3D_FENCE_BEGIN / FLIP3D_FENCE_ELSE / FLIP3D_FENCE_END.
//   - The VEH returns EXCEPTION_CONTINUE_SEARCH for everything EXCEPT a small
//     set of hardware fault codes, AND only when the faulting thread currently
//     has an active fence. In that case it appends a one-line record to
//     %TEMP%\AeroFlip3D_crash.log (the fence label + exception code + RIP, so
//     the offending boundary is identified) and longjmps back to the fence's
//     recovery block, which returns a safe value. explorer.exe keeps running.
//   - C++ thrown exceptions (0xE06D7363), breakpoints and stack overflows are
//     NEVER intercepted here: the former are handled by the existing try/catch
//     (so RAII destructors still run on those paths), the latter cannot be
//     recovered from safely.
//   - The VEH does no heap allocation and only reads the TLS slot, so it is
//     safe to run on any thread at any time.
//
// Residual, accepted risk: if a fault happens *inside* a Win32/DWM call while
// that call holds one of its own internal locks, the longjmp will not release
// it; the worst case is that *our* hook thread later hangs (Flip 3D stops
// working) while the rest of explorer stays alive -- which is strictly better
// than a shell restart, and the crash log records the event.
// -----------------------------------------------------------------------------

struct FenceFrame {
    jmp_buf* jb;
    const char* label;
};

struct FenceStack {
    FenceFrame frames[16];
    int top;
};

static DWORD g_fenceTls = TLS_OUT_OF_INDEXES;
static PVOID g_veh = nullptr;

static FenceStack* GetFenceStack(bool create) {
    if (g_fenceTls == TLS_OUT_OF_INDEXES) {
        return nullptr;
    }
    FenceStack* fs = static_cast<FenceStack*>(TlsGetValue(g_fenceTls));
    if (!fs && create) {
        fs = static_cast<FenceStack*>(calloc(1, sizeof(FenceStack)));
        if (fs) {
            TlsSetValue(g_fenceTls, fs);
        }
    }
    return fs;
}

static void Flip3DFencePush(jmp_buf* jb, const char* label) {
    FenceStack* fs = GetFenceStack(true);
    if (fs && fs->top < 16) {
        fs->frames[fs->top].jb = jb;
        fs->frames[fs->top].label = label;
        fs->top++;
    }
}

static void Flip3DFencePop() {
    FenceStack* fs = GetFenceStack(false);
    if (fs && fs->top > 0) {
        fs->top--;
    }
}

// Auto-pop on the normal (non-recovery) return path, so a boundary with many
// `return` statements cannot leak a fence level.
class Flip3DFenceAutoPop {
public:
    ~Flip3DFenceAutoPop() {
        Flip3DFencePop();
    }
};

// Appends a single diagnostic line using only kernel32/user32 (no CRT heap),
// so it is safe to call from the VEH fault path.
static void WriteCrashLog(DWORD code, PEXCEPTION_POINTERS ep, const char* label) {
    wchar_t path[MAX_PATH] = L"";
    DWORD len = GetTempPathW(MAX_PATH, path);
    if (len == 0 || len >= MAX_PATH) {
        return;
    }
    if (lstrcatW(path, L"AeroFlip3D_crash.log") == nullptr) {
        return;
    }

#if defined(_M_ARM64) || defined(_M_ARM64EC)
    ULONG_PTR rip = ep && ep->ContextRecord ? static_cast<ULONG_PTR>(ep->ContextRecord->Pc) : 0;
#elif defined(_M_IX86)
    ULONG_PTR rip = ep && ep->ContextRecord ? static_cast<ULONG_PTR>(ep->ContextRecord->Eip) : 0;
#else
    ULONG_PTR rip = ep && ep->ContextRecord ? static_cast<ULONG_PTR>(ep->ContextRecord->Rip) : 0;
#endif
    wchar_t line[384];
    wsprintfW(line,
              L"[AeroFlip3D] fence recovered | fence=%S | code=0x%08X | rip=0x%016IX | tick=%u\r\n",
              label ? label : "?",
              code,
              rip,
              static_cast<unsigned>(GetTickCount64() & 0xFFFFFFFFu));

    HANDLE h = CreateFileW(path, FILE_APPEND_DATA,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        DWORD bytes = static_cast<DWORD>(wcslen(line) * sizeof(wchar_t));
        WriteFile(h, line, bytes, &written, nullptr);
        CloseHandle(h);
    }
}

static LONG NTAPI Flip3D_Veh(PEXCEPTION_POINTERS ep) {
    if (!ep || !ep->ExceptionRecord) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    const DWORD code = ep->ExceptionRecord->ExceptionCode;

    // Only recover from hardware faults we can survive. Explicitly exclude C++
    // throws, breakpoints and stack overflows.
    if (code != EXCEPTION_ACCESS_VIOLATION &&
        code != EXCEPTION_ILLEGAL_INSTRUCTION &&
        code != EXCEPTION_INT_DIVIDE_BY_ZERO &&
        code != EXCEPTION_INT_OVERFLOW &&
        code != EXCEPTION_DATATYPE_MISALIGNMENT &&
        code != EXCEPTION_PRIV_INSTRUCTION &&
        code != EXCEPTION_ARRAY_BOUNDS_EXCEEDED &&
        code != EXCEPTION_FLT_DENORMAL_OPERAND &&
        code != EXCEPTION_FLT_DIVIDE_BY_ZERO &&
        code != EXCEPTION_FLT_INVALID_OPERATION) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    FenceStack* fs = GetFenceStack(false);
    if (!fs || fs->top <= 0) {
        return EXCEPTION_CONTINUE_SEARCH;  // Not our guarded code: leave it alone.
    }

    const char* label = fs->frames[fs->top - 1].label;
    jmp_buf* jb = fs->frames[fs->top - 1].jb;
    if (!jb) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    WriteCrashLog(code, ep, label);
    longjmp(*jb, 1);  // Transfer control to the boundary's recovery block.
}

// The fence must be opened with these three macros INSIDE the boundary
// function (setjmp has to live in the frame we longjmp back to). The recovery
// block (between BEGIN and ELSE) must end with a `return`. The normal body
// (between ELSE and END) may return freely; its fence level is auto-popped.
#define FLIP3D_FENCE_BEGIN(label)        \
    jmp_buf _flip3d_fjb;                 \
    Flip3DFencePush(&_flip3d_fjb, label); \
    if (setjmp(_flip3d_fjb)) {           \
        Flip3DFencePop();
#define FLIP3D_FENCE_ELSE                \
    } else {                             \
        Flip3DFenceAutoPop _flip3d_fpop;
#define FLIP3D_FENCE_END }

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

struct FlipWindowEntry {
    HWND hwnd = nullptr;
    ThumbnailHandle thumbnail;          // Used only in flat-fallback mode.
    std::vector<ThumbnailHandle> slices;  // Perspective strips (one live thumbnail each).
    int stripCount = 0;                   // 0 = this card uses the flat single thumbnail.
    bool stripsHidden = false;            // True once hidden strips were told to hide (skip per-frame work).
    SIZE sourceSize = {0, 0};
    RECT sourceRect = {0, 0, 0, 0};

    RECT currentRect = {0, 0, 0, 0};
    RECT startRect = {0, 0, 0, 0};
    RECT targetRect = {0, 0, 0, 0};

    BYTE currentOpacity = 0;
    BYTE startOpacity = 0;
    BYTE targetOpacity = 0;
    int updateFailures = 0;  // Consecutive DWM update failures (zombie-window tripwire).

    // Perspective tilt amount (0 = flat card facing the viewer, 1 = full
    // Flip 3D rotation). Animated like the rect/opacity so cards morph
    // flat -> tilted on entry and tilted -> flat on exit.
    double currentTilt = 0.0;
    double startTilt = 0.0;
    double targetTilt = 0.0;
};

enum class TriggerModifier : int {
    None = 0,
    Win = 1,
    Alt = 2,
};

enum class FlipAnimationKind : int {
    None = 0,
    Entry,
    Layout,
    Exit,
};

static HWND g_hControllerWnd = nullptr;  // message-only window on hook/UI thread
static HWND g_hOverlayWnd = nullptr;     // visible fullscreen stack overlay
static HWND g_hDesktopThumbnailSource = nullptr;
static HTHUMBNAIL g_desktopThumbnail = nullptr;
static ScopedBitmap g_desktopSnapshotBitmap;    // real desktop captured via GDI
static SIZE g_desktopSnapshotSize = {0, 0};
static ScopedBitmap g_backdropCompositeBitmap;  // snapshot + veil, pre-blended once
static SIZE g_backdropCompositeSize = {0, 0};
static DWORD g_hookThreadId = 0;
static DWORD g_overlayThreadId = 0;
static ScopedHandle g_hookThread;
static ScopedHandle g_hookReadyEvent;
static std::atomic<bool> g_hookInstallOk{false};
static std::atomic<bool> g_hookSessionActive{false};

static ScopedHook g_keyboardHook;
static ScopedHook g_mouseHook;

static std::vector<FlipWindowEntry> g_windows;
static int g_selectedIndex = 0;
static bool g_desktopSelected = false;  // true = front slot empty, desktop showing (Win7-style entry)
static bool g_persistentMode = false;   // true = Ctrl+Win+Tab or Ctrl+G: switcher stays open after key release
static int g_activeWindowIndex = -1;    // index of the foreground window at activation time
static HWND g_initialForegroundHwnd = nullptr;
static bool g_isActive = false;
static bool g_animationInProgress = false;
static FlipAnimationKind g_animationKind = FlipAnimationKind::None;
static DWORD g_activeAnimationDurationMs = 240;
static bool g_exitInProgress = false;
static HWND g_pendingActivateHwnd = nullptr;
static ULONGLONG g_animationStartTick = 0;
static UINT_PTR g_animationTimerId = 0;
static TriggerModifier g_triggerModifier = TriggerModifier::None;
static TriggerModifier g_suppressReleaseModifier = TriggerModifier::None;
static bool g_suppressNextModifierRelease = false;

static int32_t g_wheelAccum = 0;
static ULONGLONG g_lastWheelPostMs = 0;
static std::atomic<ULONGLONG> g_lastNavigationTick{0};
static int g_lastWheelDir = 0;

static int g_moduleAddressAnchor = 0;

// -----------------------------------------------------------------------------
// Settings (read from the Windhawk UI block above)
// -----------------------------------------------------------------------------

struct Flip3DSettings {
    bool perspective = true;  // true = realistic strips; false = flat cascade.
    bool verboseLogging = false;  // true = emit LogDebug diagnostics.
};

static Flip3DSettings g_settings;

static void LoadSettings() {
    // usePerspective is a boolean setting; Wh_GetIntSetting returns 1 (on) or
    // 0 (off) -- the canonical boolean read used by official Windhawk mods.
    g_settings.perspective = Wh_GetIntSetting(L"usePerspective") != 0;
    g_settings.verboseLogging = Wh_GetIntSetting(L"verboseLogging") != 0;
    g_verboseLogging.store(g_settings.verboseLogging, std::memory_order_relaxed);
    LogDebugFmt(L"LoadSettings: perspective=%d verboseLogging=%d",
                g_settings.perspective ? 1 : 0, g_settings.verboseLogging ? 1 : 0);
}

static constexpr UINT_PTR kAnimationTimerId = 1;
static constexpr UINT_PTR kFailsafeTimerId = 2;
static constexpr UINT_PTR kAutoCycleTimerId = 3;
// Runs on the CONTROLLER window (not the overlay). Windows intermittently
// "reclaims" the Win+Tab / Ctrl+Win+Tab global grab (e.g. after Task View,
// Xbox Game Bar, or Ink Workspace re-registers it, or after a DWM/session
// reinit on sleep/lock). Periodically re-issuing RegisterHotKey is the
// documented way to win it back without restarting the mod.
static constexpr UINT_PTR kHotkeyReclaimTimerId = 4;
static constexpr UINT kHotkeyReclaimIntervalMs = 4000;
static constexpr UINT kAutoCycleIntervalMs = 250;  // Win7-style: cycles by itself while Win is held
static constexpr UINT kAnimationTimerIntervalMs = 16;  // ~60 fps: smoother animation without increasing load.
static constexpr DWORD kEntryAnimationDurationMs = 460;
static constexpr DWORD kLayoutAnimationDurationMs = 400;
static constexpr DWORD kExitAnimationDurationMs = 320;
static constexpr ULONGLONG kWheelDebounceMs = 70;
static constexpr ULONGLONG kNavigationDebounceMs = 85;
static constexpr UINT kFailsafeAutoCloseMs = 60000;
static constexpr WORD kVkDummy = 0xFF;
static constexpr int kStickyHotkeyId = 0x3D3D;
static constexpr int kToggleHotkeyId = 0x3D3E;
static constexpr int kHotkeyWinTab = 0x4001;      // Win+Tab (transient mode)
static constexpr int kHotkeyCtrlWinTab = 0x4002;  // Ctrl+Win+Tab (persistent mode)

// Tracks which combos RegisterHotKey currently owns. When a combo IS owned,
// WM_HOTKEY alone must handle it: the LL hook below must NOT also trigger
// ActivateFlip3D/NavigateSelection for the same physical keypress, or both
// paths fire back-to-back (WM_HOTKEY activates with Desktop selected, then
// the LL hook immediately navigates away from it) - this was why Win+Tab
// intermittently skipped the Win7-style "Desktop" entry and landed on the
// next window instead. When NOT owned (registration failed/lost), the LL
// hook remains the sole fallback trigger, so functionality never regresses.
static std::atomic<bool> g_stickyHotkeyOwned{false};
static std::atomic<bool> g_winTabHotkeyOwned{false};
static std::atomic<bool> g_ctrlWinTabHotkeyOwned{false};

// Crash-safe default: disabled.
// DWM thumbnails of Progman/WorkerW/SHELLDLL_DefView desktop hosts can be
// unstable on some Windows 10/11 Explorer builds. The GDI snapshot below is
// the safe backplate; this flag only re-enables the risky DWM path for tests.
static constexpr bool kEnableDesktopDwmBackdrop = false;
static constexpr bool kEnableLowLevelInputHooks = true;
static constexpr bool kEnableMinimalHotkeyHook = true;

static constexpr UINT WM_FLIP3D_ACTIVATE = WM_APP + 0x520;
static constexpr UINT WM_FLIP3D_NAVIGATE = WM_APP + 0x521;
static constexpr UINT WM_FLIP3D_CLOSE = WM_APP + 0x522;
static constexpr UINT WM_FLIP3D_UPDATE_TIMER = WM_APP + 0x523;
static constexpr UINT WM_FLIP3D_REBUILD = WM_APP + 0x524;  // Re-style the open deck (settings changed).

// Maximum windows shown in the deck: protects against pathological systems
// (hundreds of windows) that would make the stack unusable and expensive.
static constexpr size_t kMaxFlipWindows = 128;

// A thumbnail that rejects this many consecutive property updates (source
// window became a zombie/cloaked/hung) is dropped from the deck.
static constexpr int kMaxThumbnailUpdateFailures = 5;

// Perspective deformation: public DWM thumbnail APIs only accept destination
// and source RECTs (no affine/perspective transforms — the real Flip 3D
// perspective was computed inside dwm.exe). The closest public-API
// approximation is to register the same live window several times, each
// thumbnail cropped to a vertical source strip (DWM_TNP_RECTSOURCE) and
// mapped onto a destination strip whose height follows the projected
// trapezoid of a card rotated around a vertical axis.
//
// Performance note: each strip update is one ALPC round-trip to dwm.exe, and
// the hook thread (which owns the overlay) drives the animation — so the
// strip budget is kept small and concentrated on the front cards (where the
// perspective is actually visible). Deep cards render flat. Cards whose
// opacity is 0 are hidden with a single call and skipped per-frame. The
// animation timer is also slowed a touch (50 fps) to stay well within the
// system's low-level-hook responsiveness budget.
// targetTilt (see the per-card projection below) already ranges only
// 0.10-0.55, never reaching 1.0 — so this was being applied as a SECOND,
// compounding damping factor on top of that cap, capping real far-edge
// height loss at ~11% (0.55*0.20) even for the deepest cards: nearly flat.
// Raised so the back of the deck actually reads as a rotated Win7-style
// trapezoid instead of a flat cascade with a faint taper.
// Deliberately not pushed further: the per-strip height jump between two
// adjacent strips is roughly h*tilt/stripCount. Strength and strip count
// have to be tuned together - raising this without enough strips is what
// produced visible "steps" (part of a window reading as a different height
// than the rest) instead of a smooth taper. See StripCountForDepth below.
static constexpr double kPerspectiveStrength = 0.55;  // Far-edge height loss at full tilt.

// Per-depth strip budget: the front card gets the most strips (it is large
// and on-screen, so it needs smooth edges) while deep cards get fewer (they
// are small/dim and their perspective is barely visible). 0 = flat thumbnail.
inline int StripCountForDepth(int depth) {
    if (!g_settings.perspective) {
        return 0;  // Flat mode: every card uses a single flat thumbnail.
    }
    switch (depth) {
        case 0: return 14;  // was 18, then over-cut to 11 - restored for a smooth taper
        case 1: return 10;  // was 12, then over-cut to 7
        case 2: return 7;   // was 8, then over-cut to 5
        case 3: return 6;   // was 6, then over-cut to 3 (the worst offender for visible steps)
        default: return 0;  // Flat single thumbnail.
    }
}

// Maximum time Wh_ModInit may wait for the background hook thread. Explorer
// must never be blocked longer than this (point 5: no blocking init).

static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static bool TryRegisterHotkey(int id, UINT modifiers, UINT vk, PCWSTR label);
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
static void FinishExitAfterAnimation();
inline int StackDepthForIndex(int index, int selectedIndex, int count);
inline std::vector<int> BuildBackToFrontOrder();

// -----------------------------------------------------------------------------
// Utility helpers
// -----------------------------------------------------------------------------

static HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&g_moduleAddressAnchor),
                            &module) ||
        !module) {
        return GetModuleHandleW(nullptr);
    }
    return module;
}

inline int RectWidth(const RECT& rc) {
    return rc.right - rc.left;
}

inline int RectHeight(const RECT& rc) {
    return rc.bottom - rc.top;
}

inline int ClampInt(int value, int minValue, int maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

inline BYTE ClampByte(int value) {
    return static_cast<BYTE>(ClampInt(value, 0, 255));
}

inline int ScaleForDpi(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi ? dpi : 96), 96);
}

static UINT GetDpiForOverlayWindow(HWND hwnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);

    static GetDpiForWindow_t pGetDpiForWindow = []() -> GetDpiForWindow_t {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        return user32 ? reinterpret_cast<GetDpiForWindow_t>(
                            GetProcAddress(user32, "GetDpiForWindow"))
                      : nullptr;
    }();

    if (pGetDpiForWindow && hwnd && IsWindow(hwnd)) {
        UINT dpi = pGetDpiForWindow(hwnd);
        if (dpi) {
            return dpi;
        }
    }

    // Fallback: screen DC caps. fromScreen(nullptr) is RAII, no leak possible.
    ScopedDc hdc = ScopedDc::fromScreen(nullptr);
    UINT dpi = hdc ? static_cast<UINT>(GetDeviceCaps(hdc.get(), LOGPIXELSX)) : 96;
    return dpi ? dpi : 96;
}

static RECT GetPrimaryMonitorRect() {
    POINT origin = {0, 0};
    HMONITOR monitor = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (monitor && GetMonitorInfoW(monitor, &mi)) {
        return mi.rcMonitor;
    }

    return {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
}

inline LONG LerpLong(LONG from, LONG to, double t) {
    return static_cast<LONG>(std::lround(from + (to - from) * t));
}

inline RECT LerpRect(const RECT& from, const RECT& to, double t) {
    return {
        LerpLong(from.left, to.left, t),
        LerpLong(from.top, to.top, t),
        LerpLong(from.right, to.right, t),
        LerpLong(from.bottom, to.bottom, t),
    };
}

inline bool IntersectsRect(const RECT& a, const RECT& b) {
    return a.left < b.right && a.right > b.left &&
           a.top < b.bottom && a.bottom > b.top;
}

inline RECT MakeOverlayRelativeRect(const RECT& screenRect) {
    if (!g_hOverlayWnd || !IsWindow(g_hOverlayWnd)) {
        return screenRect;
    }

    RECT overlayRect = {};
    if (!GetWindowRect(g_hOverlayWnd, &overlayRect)) {
        return screenRect;
    }

    return {
        screenRect.left - overlayRect.left,
        screenRect.top - overlayRect.top,
        screenRect.right - overlayRect.left,
        screenRect.bottom - overlayRect.top,
    };
}

inline RECT CenterRectWithSameSize(const RECT& templateRect, int cx, int cy) {
    int w = std::max(1, RectWidth(templateRect));
    int h = std::max(1, RectHeight(templateRect));
    return {cx - w / 2, cy - h / 2, cx + w / 2, cy + h / 2};
}

inline BYTE LerpOpacity(BYTE from, BYTE to, double t) {
    return ClampByte(static_cast<int>(std::lround(from + (to - from) * t)));
}

static double EaseOutCubic(double t) {
    t = std::max(0.0, std::min(1.0, t));
    double inv = 1.0 - t;
    return 1.0 - inv * inv * inv;
}

static bool IsWinKey(DWORD vk) {
    return vk == VK_LWIN || vk == VK_RWIN;
}

static bool IsAltKey(DWORD vk) {
    return vk == VK_MENU || vk == VK_LMENU || vk == VK_RMENU;
}

static bool IsCtrlKey(DWORD vk) {
    return vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL;
}

static bool IsShiftKey(DWORD vk) {
    return vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT;
}

static bool IsAnyModifierKey(DWORD vk) {
    return IsWinKey(vk) || IsAltKey(vk) || IsCtrlKey(vk) || IsShiftKey(vk);
}

static bool IsTriggerModifierKey(TriggerModifier trigger, DWORD vk) {
    return (trigger == TriggerModifier::Win && IsWinKey(vk)) ||
           (trigger == TriggerModifier::Alt && IsAltKey(vk));
}

static bool IsKeyDownMessage(WPARAM wParam) {
    return wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
}

static bool IsKeyUpMessage(WPARAM wParam) {
    return wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
}

static bool PairStillDown(DWORD releasedVk) {
    auto down = [](int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; };

    switch (releasedVk) {
        case VK_LWIN:
            return down(VK_RWIN);
        case VK_RWIN:
            return down(VK_LWIN);
        case VK_LMENU:
            return down(VK_RMENU);
        case VK_RMENU:
            return down(VK_LMENU);
        case VK_MENU:
            return down(VK_LMENU) || down(VK_RMENU);
        default:
            return false;
    }
}

static bool TriggerModifierStillPhysicallyDown(TriggerModifier trigger) {
    if (trigger == TriggerModifier::Win) {
        return (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 ||
               (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
    }
    if (trigger == TriggerModifier::Alt) {
        return (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ||
               (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0 ||
               (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    }
    return false;
}

static void ResetWheelState() {
    g_wheelAccum = 0;
    g_lastWheelPostMs = 0;
    g_lastNavigationTick.store(0, std::memory_order_relaxed);
    g_lastWheelDir = 0;
}

static void ClearPendingModifierReleaseSuppression() {
    g_suppressNextModifierRelease = false;
    g_suppressReleaseModifier = TriggerModifier::None;
}

static void PreparePendingModifierReleaseSuppression() {
    g_suppressReleaseModifier = g_triggerModifier;
    g_suppressNextModifierRelease = TriggerModifierStillPhysicallyDown(g_suppressReleaseModifier);
    if (!g_suppressNextModifierRelease) {
        g_suppressReleaseModifier = TriggerModifier::None;
    }
}

static void SwallowModifierRelease(DWORD vk) {
    // Defuse Start-menu/menu-bar side effects after swallowing Win/Alt combos:
    // make Windows see an unrelated key press between modifier down and up,
    // then replay the modifier release as injected input.
    INPUT inputs[3] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = kVkDummy;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = kVkDummy;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = static_cast<WORD>(vk);
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(3, inputs, sizeof(INPUT));
}

static void PostToController(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) {
    HWND controller = g_hControllerWnd;
    if (controller && IsWindow(controller)) {
        PostMessageW(controller, msg, wParam, lParam);
    } else if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, msg, wParam, lParam);
    }
}

// -----------------------------------------------------------------------------
// Desktop backdrop: real snapshot -> wallpaper bitmap -> Aero gradient.
// Each layer is RAII-based and isolated: a failure in one layer degrades to
// the next instead of taking the mod (or Explorer) down.
// -----------------------------------------------------------------------------

static void ReleaseDesktopSnapshot() {
    g_desktopSnapshotBitmap.reset();
    g_desktopSnapshotSize.cx = 0;
    g_desktopSnapshotSize.cy = 0;
    g_backdropCompositeBitmap.reset();
    g_backdropCompositeSize.cy = 0;
    g_backdropCompositeSize.cx = 0;
}

// Resource-pressure guards, checked before allocating full-screen GDI
// bitmaps. If the system is under pressure the snapshot is simply skipped
// and the backdrop pipeline degrades to the Aero gradient (layer 3).
static bool GdiBudgetAllowsBitmaps(int needed = 8) {
    // Per-process GDI object cap is 10,000 by default. If we are already
    // close to it (leak elsewhere, unusual system state), don't make it worse.
    DWORD used = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    return used == 0 || used + static_cast<DWORD>(needed) < 8000;
}

static bool MemoryPressureAllowsSnapshot() {
    MEMORYSTATUSEX status = {};
    status.dwLength = sizeof(status);
    if (!GlobalMemoryStatusEx(&status)) {
        return true;  // Cannot tell -> allow.
    }
    return status.ullAvailPhys > 100ULL * 1024 * 1024;  // 100 MB free physical RAM.
}

// Finds the real desktop window that hosts the icons/wallpaper
// (Progman or one of the WorkerW windows containing SHELLDLL_DefView).
static HWND FindPlainDesktopWindow() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (!progman || !IsWindow(progman)) {
        return nullptr;
    }

    HWND defView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    if (defView && IsWindow(defView)) {
        return progman;
    }

    HWND workerW = FindWindowExW(nullptr, nullptr, L"WorkerW", nullptr);
    while (workerW && IsWindow(workerW)) {
        HWND defViewInWorker = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr);
        if (defViewInWorker && IsWindow(defViewInWorker)) {
            return workerW;
        }
        workerW = FindWindowExW(nullptr, workerW, L"WorkerW", nullptr);
    }

    return progman;
}

// Captures the real, currently-visible desktop (wallpaper + icons + taskbar)
// into a GDI bitmap. All DCs and bitmaps are RAII-owned, so no early return
// can leak. Must be called BEFORE the overlay window becomes visible,
// otherwise the overlay itself would be captured.
static void CaptureDesktopSnapshot() {
    // Se già abbiamo un bitmap della stessa dimensione, lo riusiamo
    bool reuseExisting = false;
    if (g_desktopSnapshotBitmap) {
        BITMAP bm = {};
        if (GetObjectW(g_desktopSnapshotBitmap.get(), sizeof(bm), &bm) == sizeof(bm)) {
            if (bm.bmWidth > 0 && bm.bmHeight > 0) {
                reuseExisting = true;
            }
        }
    }

    // Se non abbiamo un bitmap da riusare, libera e ricrea
    if (!reuseExisting) {
        ReleaseDesktopSnapshot();
    }

    HWND desktopHwnd = FindPlainDesktopWindow();
    if (!desktopHwnd || !IsWindow(desktopHwnd)) {
        desktopHwnd = GetDesktopWindow();
    }
    if (!desktopHwnd || !IsWindow(desktopHwnd)) {
        LogFailure(L"CaptureDesktopSnapshot: no desktop window");
        return;
    }

    if (!GdiBudgetAllowsBitmaps() || !MemoryPressureAllowsSnapshot()) {
        LogFailure(L"CaptureDesktopSnapshot: resource pressure");
        return;
    }

    RECT desktopRect = {};
    if (!GetWindowRect(desktopHwnd, &desktopRect) ||
        RectWidth(desktopRect) <= 0 || RectHeight(desktopRect) <= 0) {
        desktopRect = GetPrimaryMonitorRect();
    }

    const int width = RectWidth(desktopRect);
    const int height = RectHeight(desktopRect);
    if (width <= 0 || height <= 0) {
        LogFailure(L"CaptureDesktopSnapshot: invalid size");
        return;
    }

    // Se stiamo riusando un bitmap esistente, verifica che abbia le dimensioni giuste
    if (reuseExisting) {
        BITMAP bm = {};
        if (GetObjectW(g_desktopSnapshotBitmap.get(), sizeof(bm), &bm) == sizeof(bm)) {
            if (bm.bmWidth != width || bm.bmHeight != height) {
                // Dimensioni diverse: ricrea
                ReleaseDesktopSnapshot();
                reuseExisting = false;
            }
        } else {
            reuseExisting = false;
            ReleaseDesktopSnapshot();
        }
    }

    // Se non abbiamo un bitmap valido, creane uno nuovo
    if (!reuseExisting) {
        ScopedDc screenDc = ScopedDc::fromScreen(nullptr);
        if (!screenDc) {
            LogFailureCode(L"CaptureDesktopSnapshot: GetDC", GetLastError());
            return;
        }

        ScopedBitmap bitmap(CreateCompatibleBitmap(screenDc.get(), width, height));
        if (!bitmap) {
            LogFailure(L"CaptureDesktopSnapshot: CreateCompatibleBitmap");
            return;
        }
        g_desktopSnapshotBitmap = std::move(bitmap);
        g_desktopSnapshotSize.cx = width;
        g_desktopSnapshotSize.cy = height;
    }

    // Ora disegna il contenuto nel bitmap (nuovo o riutilizzato)
    ScopedDc screenDc = ScopedDc::fromScreen(nullptr);
    if (!screenDc) {
        LogFailureCode(L"CaptureDesktopSnapshot: GetDC(2)", GetLastError());
        return;
    }

    ScopedDc memDc = ScopedDc::compatible(screenDc.get());
    if (!memDc) {
        LogFailure(L"CaptureDesktopSnapshot: CreateCompatibleDC");
        return;
    }

    ScopedSelectObject<HBITMAP> select(memDc.get(), g_desktopSnapshotBitmap.get());
    if (!select.succeeded()) {
        LogFailure(L"CaptureDesktopSnapshot: SelectObject");
        return;
    }

    auto isBlankCapture = [&]() -> bool {
        static constexpr int kSampleCount = 9;
        int hits = 0;
        for (int i = 0; i < kSampleCount; ++i) {
            const int sx = (width * (i + 1)) / (kSampleCount + 1);
            const int sy = (height * (i + 1)) / (kSampleCount + 1);
            COLORREF c = GetPixel(memDc.get(), sx, sy);
            if (c == CLR_INVALID || c == RGB(0, 0, 0)) {
                ++hits;
            }
        }
        return hits >= kSampleCount;
    };

    BOOL ok = PrintWindow(desktopHwnd, memDc.get(), PW_RENDERFULLCONTENT);
    if (ok && isBlankCapture()) {
        ok = FALSE;
    }
    if (!ok) {
        ok = PrintWindow(desktopHwnd, memDc.get(), 0);
        if (ok && isBlankCapture()) {
            ok = FALSE;
        }
    }
    if (!ok) {
        ok = BitBlt(memDc.get(), 0, 0, width, height, screenDc.get(),
                    desktopRect.left, desktopRect.top, SRCCOPY);
        if (ok && isBlankCapture()) {
            ok = FALSE;
        }
    }

    if (ok) {
        HWND taskbarHwnd = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (taskbarHwnd && IsWindow(taskbarHwnd)) {
            RECT taskbarRect = {};
            if (GetWindowRect(taskbarHwnd, &taskbarRect) &&
                RectWidth(taskbarRect) > 0 && RectHeight(taskbarRect) > 0) {
                BitBlt(memDc.get(),
                       taskbarRect.left - desktopRect.left,
                       taskbarRect.top - desktopRect.top,
                       RectWidth(taskbarRect),
                       RectHeight(taskbarRect),
                       screenDc.get(),
                       taskbarRect.left,
                       taskbarRect.top,
                       SRCCOPY | CAPTUREBLT);
            }
        }

        HWND startButtonHwnd = FindWindowW(L"Button", L"Start");
        if (!startButtonHwnd && taskbarHwnd && IsWindow(taskbarHwnd)) {
            startButtonHwnd = FindWindowExW(taskbarHwnd, nullptr, L"Start", nullptr);
        }
        if (startButtonHwnd && IsWindow(startButtonHwnd)) {
            RECT startRect = {};
            if (GetWindowRect(startButtonHwnd, &startRect) &&
                RectWidth(startRect) > 0 && RectHeight(startRect) > 0) {
                BitBlt(memDc.get(),
                       startRect.left - desktopRect.left,
                       startRect.top - desktopRect.top,
                       RectWidth(startRect),
                       RectHeight(startRect),
                       screenDc.get(),
                       startRect.left,
                       startRect.top,
                       SRCCOPY | CAPTUREBLT);
            }
        }
    }

    if (!ok) {
        LogFailure(L"CaptureDesktopSnapshot: capture");
    }
}

static bool EnsureBackdropComposite(HDC refDc, const RECT& rc) {
    if (!refDc) {
        return false;
    }

    const int w = RectWidth(rc);
    const int h = RectHeight(rc);
    if (w <= 0 || h <= 0 || !g_desktopSnapshotBitmap) {
        return false;
    }

    // Controllo cache con tolleranza DPI (±1 pixel)
    if (g_backdropCompositeBitmap) {
        if (abs(g_backdropCompositeSize.cx - w) <= 1 &&
            abs(g_backdropCompositeSize.cy - h) <= 1) {
            return true;  // Cache valida
        }
    }

    // Ricostruisci il composite
    g_backdropCompositeBitmap.reset();
    g_backdropCompositeSize.cx = 0;
    g_backdropCompositeSize.cy = 0;

    ScopedDc compositeDc = ScopedDc::compatible(refDc);
    if (!compositeDc) {
        return false;
    }

    ScopedBitmap compositeBitmap(CreateCompatibleBitmap(refDc, w, h));
    if (!compositeBitmap) {
        return false;
    }

    ScopedSelectObject<HBITMAP> selectComposite(compositeDc.get(), compositeBitmap.get());
    if (!selectComposite.succeeded()) {
        return false;
    }

    ScopedDc snapshotDc = ScopedDc::compatible(refDc);
    if (snapshotDc) {
        ScopedSelectObject<HBITMAP> selectSnapshot(snapshotDc.get(),
                                                   g_desktopSnapshotBitmap.get());
        if (selectSnapshot.succeeded()) {
            SetStretchBltMode(compositeDc.get(), HALFTONE);
            StretchBlt(compositeDc.get(), 0, 0, w, h, snapshotDc.get(), 0, 0,
                       g_desktopSnapshotSize.cx, g_desktopSnapshotSize.cy, SRCCOPY);
        }
    }

    static HMODULE msimg32 = LoadLibraryW(L"msimg32.dll");
    using AlphaBlend_t = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
    static AlphaBlend_t pAlphaBlend =
        msimg32 ? reinterpret_cast<AlphaBlend_t>(GetProcAddress(msimg32, "AlphaBlend"))
                : nullptr;

    if (pAlphaBlend) {
        ScopedDc veilDc = ScopedDc::compatible(refDc);
        if (veilDc) {
            ScopedBitmap veilBitmap(CreateCompatibleBitmap(refDc, w, h));
            if (veilBitmap) {
                ScopedSelectObject<HBITMAP> selectVeil(veilDc.get(), veilBitmap.get());
                if (selectVeil.succeeded()) {
                    RECT veilRc = {0, 0, w, h};
                    FillRect(veilDc.get(), &veilRc,
                             static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

                    BLENDFUNCTION blend = {};
                    blend.BlendOp = AC_SRC_OVER;
                    blend.BlendFlags = 0;
                    blend.SourceConstantAlpha = 80;
                    blend.AlphaFormat = 0;

                    pAlphaBlend(compositeDc.get(), 0, 0, w, h, veilDc.get(), 0, 0, w, h, blend);
                }
            }
        }
    }

    g_backdropCompositeBitmap = std::move(compositeBitmap);
    g_backdropCompositeSize.cx = w;
    g_backdropCompositeSize.cy = h;
    return true;
}

// Draws the cached desktop+veil composite as the backplate (Vista/7 look).
static bool PaintDesktopSnapshotWithVeil(HDC hdc, const RECT& rc) {
    if (!hdc || !EnsureBackdropComposite(hdc, rc)) {
        return false;
    }

    ScopedDc memDc = ScopedDc::compatible(hdc);
    if (!memDc) {
        return false;
    }

    ScopedSelectObject<HBITMAP> select(memDc.get(), g_backdropCompositeBitmap.get());
    if (!select.succeeded()) {
        return false;
    }

    return BitBlt(hdc, rc.left, rc.top, RectWidth(rc), RectHeight(rc),
                  memDc.get(), 0, 0, SRCCOPY) != FALSE;
}

// Backdrop pipeline with cascading fallbacks (point 6: isolated components):
//   1. real desktop snapshot + veil,
//   2. current wallpaper (BMP only, plain GDI),
//   3. Aero-style gradient drawn with GDI only (always succeeds).
static void PaintStaticWallpaperBackdrop(HWND hwnd, HDC hdc) {
    if (!hwnd || !IsWindow(hwnd) || !hdc) {
        return;
    }

    RECT rc = {};
    if (!GetClientRect(hwnd, &rc) || RectWidth(rc) <= 0 || RectHeight(rc) <= 0) {
        return;
    }

    // Layer 1: real desktop snapshot + dark veil.
    if (PaintDesktopSnapshotWithVeil(hdc, rc)) {
        return;
    }

    // Layer 2: current wallpaper via plain GDI. Only bitmap-compatible
    // wallpapers work here; JPEG/PNG would need GDI+/WIC, which is
    // intentionally avoided inside explorer.exe.
    wchar_t wallpaperPath[MAX_PATH] = L"";
    if (SystemParametersInfoW(SPI_GETDESKWALLPAPER, ARRAYSIZE(wallpaperPath),
                              wallpaperPath, 0) &&
        wallpaperPath[0] != L'\0') {
        ScopedBitmap wallpaper(reinterpret_cast<HBITMAP>(
            LoadImageW(nullptr, wallpaperPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)));
        if (wallpaper) {
            BITMAP bitmap = {};
            if (GetObjectW(wallpaper.get(), sizeof(bitmap), &bitmap) == sizeof(bitmap) &&
                bitmap.bmWidth > 0 && bitmap.bmHeight > 0) {
                ScopedDc memDc = ScopedDc::compatible(hdc);
                if (memDc) {
                    ScopedSelectObject<HBITMAP> select(memDc.get(), wallpaper.get());
                    if (select.succeeded()) {
                        SetStretchBltMode(hdc, HALFTONE);
                        if (StretchBlt(hdc, rc.left, rc.top, RectWidth(rc), RectHeight(rc),
                                       memDc.get(), 0, 0, bitmap.bmWidth, bitmap.bmHeight,
                                       SRCCOPY)) {
                            return;
                        }
                    }
                }
            }
        }
    }

    // Layer 3: crash-safe Aero gradient, plain GDI only. No GDI+/WIC/
    // PrintWindow/DwmRegisterThumbnail here — this must always work.
    const int h = std::max(1, RectHeight(rc));
    const int bands = 96;
    for (int i = 0; i < bands; ++i) {
        double t = static_cast<double>(i) / static_cast<double>(bands - 1);
        int r = static_cast<int>(6 + 4 * (1.0 - t));
        int g = static_cast<int>(42 + 48 * (1.0 - t));
        int b = static_cast<int>(70 + 82 * (1.0 - t));

        double glow = 1.0 - std::min(1.0, std::abs(t - 0.38) / 0.38);
        g += static_cast<int>(42 * glow);
        b += static_cast<int>(26 * glow);

        RECT band = rc;
        band.top = rc.top + MulDiv(i, h, bands);
        band.bottom = rc.top + MulDiv(i + 1, h, bands);

        ScopedSolidBrush brush(RGB(std::min(255, std::max(0, r)),
                                   std::min(255, std::max(0, g)),
                                   std::min(255, std::max(0, b))));
        if (brush) {
            FillRect(hdc, &band, brush.get());
        }
    }
}

static void DrawFaux3DFrames(HWND, HDC) {
    // Intentionally empty: Windows 7 drew no artificial GDI borders/shadows.
}

// -----------------------------------------------------------------------------
// Desktop host search for the (optional, disabled by default) DWM backplate
// -----------------------------------------------------------------------------

static HWND g_desktopHostSearchResult = nullptr;

static BOOL CALLBACK FindDesktopHostProc(HWND hwnd, LPARAM) {
    // Guarded: an exception (or hardware fault) here must not cross the
    // EnumWindows boundary.
    FLIP3D_FENCE_BEGIN("FindDesktopHostProc")
        return FALSE;
    FLIP3D_FENCE_ELSE
    try {
        if (!hwnd || !IsWindow(hwnd)) {
            return TRUE;  // Skip, keep enumerating.
        }
        HWND defView = FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr);
        if (defView && IsWindow(defView)) {
            g_desktopHostSearchResult = hwnd;
            return FALSE;  // Stop.
        }
    } catch (...) {
        LogFailure(L"FindDesktopHostProc");
        return FALSE;  // Stop enumeration safely.
    }
    FLIP3D_FENCE_END
    return TRUE;
}

static HWND FindDesktopThumbnailSourceWindow() {
    g_desktopHostSearchResult = nullptr;
    EnumWindows(FindDesktopHostProc, 0);
    if (g_desktopHostSearchResult && IsWindow(g_desktopHostSearchResult)) {
        return g_desktopHostSearchResult;
    }

    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman && IsWindow(progman)) {
        return progman;
    }

    return GetShellWindow();
}

// -----------------------------------------------------------------------------
// Alt-Tab-like window enumeration, Win10/Win11 compatible
// -----------------------------------------------------------------------------

static bool IsClassBlacklisted(HWND hwnd) {
    wchar_t cls[128] = L"";
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return true;
    }

    // Finestre del mod stesso
    if (lstrcmpW(cls, L"Flip3DOverlayWndClass") == 0) return true;
    if (lstrcmpW(cls, L"Flip3DControllerWndClass") == 0) return true;
    
    // Finestre di sistema
    if (lstrcmpW(cls, L"Shell_TrayWnd") == 0) return true;
    if (lstrcmpW(cls, L"Shell_SecondaryTrayWnd") == 0) return true;
    if (lstrcmpW(cls, L"Progman") == 0) return true;
    if (lstrcmpW(cls, L"WorkerW") == 0) return true;
    if (lstrcmpW(cls, L"Shell_InputSwitchTopLevelWindow") == 0) return true;
    if (lstrcmpW(cls, L"Windows.UI.Core.CoreWindow") == 0) return true;
    if (lstrcmpW(cls, L"ApplicationManager_ImmersiveShellWindow") == 0) return true;
    
    // --- SOFTWARE DI REGISTRAZIONE SCHERMO ---
    // Bandicam
    if (lstrcmpW(cls, L"Bandicam") == 0) return true;
    if (lstrcmpW(cls, L"BANDICAM") == 0) return true;
    if (lstrcmpW(cls, L"Bandicam Window") == 0) return true;
    
    // Fraps
    if (lstrcmpW(cls, L"FRAPS") == 0) return true;
    if (lstrcmpW(cls, L"Fraps") == 0) return true;
    if (lstrcmpW(cls, L"FRAPS_CAPTURE") == 0) return true;
    
    // OBS Studio
    if (lstrcmpW(cls, L"Qt5QWindowIcon") == 0) return true;  // OBS usa Qt
    if (lstrcmpW(cls, L"OBS") == 0) return true;
    if (lstrcmpW(cls, L"obs") == 0) return true;
    
    // NVIDIA ShadowPlay / GeForce Experience
    if (lstrcmpW(cls, L"NVIDIA Share") == 0) return true;
    if (lstrcmpW(cls, L"ShadowPlay") == 0) return true;
    if (lstrcmpW(cls, L"GeForce Experience") == 0) return true;
    if (lstrcmpW(cls, L"NVContainer") == 0) return true;
    
    // AMD ReLive
    if (lstrcmpW(cls, L"AMD Radeon Settings") == 0) return true;
    if (lstrcmpW(cls, L"Radeon") == 0) return true;
    if (lstrcmpW(cls, L"AMDRSServ") == 0) return true;
    
    // Xbox Game Bar / Xbox Game DVR
    if (lstrcmpW(cls, L"Xbox Game Bar") == 0) return true;
    if (lstrcmpW(cls, L"GameDVR") == 0) return true;
    if (lstrcmpW(cls, L"Windows.Gaming.GameBar") == 0) return true;
    
    // Windows Game DVR
    if (lstrcmpW(cls, L"Windows.Gaming.GameBar.PopupWindow") == 0) return true;
    if (lstrcmpW(cls, L"Microsoft.Windows.Gaming.GameBar") == 0) return true;
    
    // Steam overlay
    if (lstrcmpW(cls, L"Steam") == 0) return true;
    if (lstrcmpW(cls, L"STEAM") == 0) return true;
    if (lstrcmpW(cls, L"GameOverlayUI") == 0) return true;
    if (lstrcmpW(cls, L"SteamOverlay") == 0) return true;
    
    
    // Action! (Mirillis)
    if (lstrcmpW(cls, L"Action") == 0) return true;
    if (lstrcmpW(cls, L"Mirillis") == 0) return true;
    if (lstrcmpW(cls, L"ActionOverlay") == 0) return true;
    
    // D3DGear
    if (lstrcmpW(cls, L"D3DGear") == 0) return true;
    if (lstrcmpW(cls, L"d3dgear") == 0) return true;
    
    // PlayClaw
    if (lstrcmpW(cls, L"PlayClaw") == 0) return true;
    if (lstrcmpW(cls, L"Playclaw") == 0) return true;
    
    // Overwolf
    if (lstrcmpW(cls, L"Overwolf") == 0) return true;
    if (lstrcmpW(cls, L"OverwolfBrowser") == 0) return true;
    
    // Mediatracker / Any other recording software
    if (lstrcmpW(cls, L"MediaTracker") == 0) return true;
    // ------------------------------------------

    return false;
}

static bool HasUwpContent(HWND hwnd) {
    wchar_t cls[128] = L"";
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    if (lstrcmpW(cls, L"ApplicationFrameWindow") != 0) {
        return true;
    }

    HWND core = FindWindowExW(hwnd, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
    return core != nullptr && IsWindow(core);
}

static bool GetRepresentativeWindowRect(HWND hwnd, RECT* outRect) {
    if (!outRect || !hwnd || !IsWindow(hwnd)) {
        return false;
    }

    RECT rc = {0, 0, 0, 0};

    if (IsIconic(hwnd)) {
        WINDOWPLACEMENT wp = {};
        wp.length = sizeof(wp);
        if (GetWindowPlacement(hwnd, &wp)) {
            if (wp.showCmd == SW_SHOWMAXIMIZED || (wp.flags & WPF_RESTORETOMAXIMIZED)) {
                HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = {};
                mi.cbSize = sizeof(mi);
                if (monitor && GetMonitorInfoW(monitor, &mi)) {
                    rc = mi.rcWork;
                } else {
                    rc = GetPrimaryMonitorRect();
                }
            } else {
                rc = wp.rcNormalPosition;
            }
        } else {
            rc = {0, 0, 800, 450};
        }
    } else {
        RECT dwmRect = {};
        HRESULT hr = DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                           &dwmRect, sizeof(dwmRect));
        if (SUCCEEDED(hr) && RectWidth(dwmRect) > 0 && RectHeight(dwmRect) > 0) {
            rc = dwmRect;
        } else if (!GetWindowRect(hwnd, &rc)) {
            return false;
        }
    }

    if (RectWidth(rc) <= 0 || RectHeight(rc) <= 0) {
        return false;
    }

    *outRect = rc;
    return true;
}

static bool IsFlipEligibleWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    if (hwnd == g_hOverlayWnd || hwnd == g_hControllerWnd) {
        return false;
    }
    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    HWND owner = GetWindow(hwnd, GW_OWNER);

    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }
    if (exStyle & WS_EX_NOACTIVATE) {
        return false;
    }
    if ((exStyle & WS_EX_TRANSPARENT) && (exStyle & WS_EX_LAYERED)) {
        return false;
    }
    if (owner && IsWindow(owner) && !(exStyle & WS_EX_APPWINDOW)) {
        return false;
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked != 0) {
        return false;
    }

    if (IsClassBlacklisted(hwnd) || !HasUwpContent(hwnd)) {
        return false;
    }

    // Many shell/helper top-level windows have no title. Real app windows with
    // WS_EX_APPWINDOW are allowed even if the title is empty.
    if (GetWindowTextLengthW(hwnd) <= 0 && !(exStyle & WS_EX_APPWINDOW)) {
        return false;
    }

    RECT rc = {};
    if (!GetRepresentativeWindowRect(hwnd, &rc)) {
        return false;
    }

    return true;
}

static BOOL CALLBACK EnumWindowsForFlipProc(HWND hwnd, LPARAM lParam) {
    // Guarded: this callback is invoked by the system for every top-level
    // window; a fault here must stop enumeration, not crash Explorer.
    FLIP3D_FENCE_BEGIN("EnumWindowsForFlipProc")
        return FALSE;
    FLIP3D_FENCE_ELSE
    try {
        auto* result = reinterpret_cast<std::vector<HWND>*>(lParam);
        if (result && hwnd && IsFlipEligibleWindow(hwnd)) {
            if (result->size() >= kMaxFlipWindows) {
                return FALSE;  // Deck cap reached: stop enumerating (DoS guard).
            }
            result->push_back(hwnd);
        }
    } catch (...) {
        LogFailure(L"EnumWindowsForFlipProc");
        return FALSE;  // Stop enumeration; caller validates the result.
    }
    FLIP3D_FENCE_END
    return TRUE;
}

static std::vector<HWND> EnumerateFlipEligibleWindows() {
    std::vector<HWND> result;
    try {
        result.reserve(32);
    } catch (...) {
        // Non-fatal: vector grows on demand.
    }
    if (!EnumWindows(EnumWindowsForFlipProc, reinterpret_cast<LPARAM>(&result))) {
        // Enumeration stopped early (exception in callback). The partial list
        // may be incomplete; treat it as unusable to avoid a half-broken UI.
        result.clear();
    }
    return result;
}

static bool CanStartFlip3DSession() {
    // Guarded: called from the low-level keyboard hook.
    return SafeExec<bool>(L"CanStartFlip3DSession", false, [] {
        BOOL compositionEnabled = FALSE;
        if (FAILED(DwmIsCompositionEnabled(&compositionEnabled)) || !compositionEnabled) {
            return false;
        }
        std::vector<HWND> eligible = EnumerateFlipEligibleWindows();
        return eligible.size() >= 2;
    });
}

// -----------------------------------------------------------------------------
// Thumbnail application and animation
// -----------------------------------------------------------------------------

inline int StackDepthForIndex(int index, int selectedIndex, int count) {
    if (count <= 1) {
        return 0;
    }

    // Stable deck order: selected window is depth 0/front, the next window is
    // depth 1, then depth 2, etc. Deliberately not a signed/circular
    // left-right layout (the old one looked chaotic when cycling fast).
    return (index - selectedIndex + count) % count;
}

// Flat (single-thumbnail) property application. Used as a fallback when a
// card cannot keep its perspective strips.
static void ApplySingleThumbnailProperties(FlipWindowEntry& entry) {
    if (!entry.thumbnail) {
        return;
    }
    if (!entry.hwnd || !IsWindow(entry.hwnd)) {
        return;
    }

    DWM_THUMBNAIL_PROPERTIES props = {};
    props.dwFlags = DWM_TNP_RECTDESTINATION |
                    DWM_TNP_VISIBLE |
                    DWM_TNP_OPACITY |
                    DWM_TNP_SOURCECLIENTAREAONLY;
    props.rcDestination = entry.currentRect;
    props.opacity = entry.currentOpacity;
    props.fVisible = entry.currentOpacity > 0 &&
                     RectWidth(entry.currentRect) > 0 &&
                     RectHeight(entry.currentRect) > 0;
    props.fSourceClientAreaOnly = FALSE;

    // Mirror the exact validation the real DWM performs inside
    // DwmUpdateThumbnailProperties (Windows 7 dwmapi.dll decompilation):
    // flags must be non-zero and, when DWM_TNP_RECTDESTINATION is set,
    // right >= left and bottom >= top — otherwise DWM rejects the update
    // with E_INVALIDARG. Never send a degenerate rect: hide the thumbnail
    // instead, so an in-flight animation lerp can't waste an LPC round-trip
    // or trip the updateFailures tripwire below.
    if (props.rcDestination.right < props.rcDestination.left ||
        props.rcDestination.bottom < props.rcDestination.top) {
        props.rcDestination = {0, 0, 0, 0};
        props.fVisible = FALSE;
    }

    HRESULT hr = DwmUpdateThumbnailProperties(entry.thumbnail.get(), &props);
    if (SUCCEEDED(hr)) {
        entry.updateFailures = 0;
    } else if (++entry.updateFailures >= kMaxThumbnailUpdateFailures) {
        // Consequence-based invalidation: a thumbnail whose source window
        // turned into a zombie (crashed/cloaked/hung) is dropped from the
        // deck instead of hammering DWM every frame. Not logged per-call.
        entry.thumbnail.reset();
    }
}

// Degrades one card from perspective strips to a single flat thumbnail.
// Called when DWM refuses the strip registrations/updates. The freshly
// registered thumbnail lands on top of the current registration order, but
// this path is rare (DWM trouble) and self-heals on the next rebuild.
static void FallbackToSingleThumbnail(FlipWindowEntry& entry) {
    entry.slices.clear();
    if (!g_hOverlayWnd || !IsWindow(g_hOverlayWnd) || !entry.hwnd || !IsWindow(entry.hwnd)) {
        return;
    }

    HRESULT hr = DwmRegisterThumbnail(g_hOverlayWnd, entry.hwnd, entry.thumbnail.put());
    if (SUCCEEDED(hr) && entry.thumbnail) {
        DwmQueryThumbnailSourceSize(entry.thumbnail.get(), &entry.sourceSize);
        ApplySingleThumbnailProperties(entry);
    }
    LogFailure(L"Perspective strips unavailable: card fell back to flat thumbnail");
}

// Applies one card's live content to DWM. Perspective mode: the card rect +
// tilt are projected into kPerspectiveStripCount vertical strips; each strip
// maps a source slice of the window onto a destination slice whose height
// follows the trapezoid of a card rotated around a vertical axis (far/left
// edge shorter). Tilt 0 degenerates exactly into the flat card rect, so
// entry/exit morphs are seamless.
static void ApplyCardThumbnails(FlipWindowEntry& entry) {
    if (!entry.hwnd || !IsWindow(entry.hwnd)) {
        return;
    }

    if (entry.slices.empty()) {
        ApplySingleThumbnailProperties(entry);
        return;
    }

    const RECT& box = entry.currentRect;
    const int w = RectWidth(box);
    const int h = RectHeight(box);
    const bool cardVisible = entry.currentOpacity > 0 && w > 0 && h > 0;

    // Cheap fast-path for hidden cards: tell the strips to hide exactly once,
    // then skip all per-frame geometry/ALPC work until the card reappears.
    if (!cardVisible) {
        if (!entry.stripsHidden) {
            for (auto& th : entry.slices) {
                if (!th) {
                    continue;
                }
                DWM_THUMBNAIL_PROPERTIES hide = {};
                hide.dwFlags = DWM_TNP_VISIBLE;
                hide.fVisible = FALSE;
                DwmUpdateThumbnailProperties(th.get(), &hide);
            }
            entry.stripsHidden = true;
        }
        return;
    }
    entry.stripsHidden = false;

    SIZE src = entry.sourceSize;
    if (src.cx <= 0 || src.cy <= 0) {
        src = {1, 1};  // Defensive: never build zero-sized source rects.
    }

    const double tilt = std::max(0.0, std::min(1.0, entry.currentTilt)) * kPerspectiveStrength;
    const double cy = (box.top + box.bottom) / 2.0;
    const int M = static_cast<int>(entry.slices.size());

    int failedStrips = 0;
    for (int j = 0; j < M; ++j) {
        if (!entry.slices[j]) {
            continue;
        }

        // Strip geometry. s = horizontal position of the strip centre:
        // s=0 is the near (left) edge, s=1 the far (right) edge — matching
        // the Win7 front-left deck orientation.
        const double s = (j + 0.5) / static_cast<double>(M);
        const int stripH = std::max(1, static_cast<int>(std::lround(h * (1.0 - tilt * s))));

        DWM_THUMBNAIL_PROPERTIES props = {};
        props.dwFlags = DWM_TNP_RECTDESTINATION |
                        DWM_TNP_RECTSOURCE |
                        DWM_TNP_VISIBLE |
                        DWM_TNP_OPACITY |
                        DWM_TNP_SOURCECLIENTAREAONLY;
        // Destination strip. Integer division + 1px overlap on the right
        // edge so rounding can never open 1px seams between strips.
        props.rcDestination.left = box.left + (j * w) / M;
        props.rcDestination.right = box.left + ((j + 1) * w) / M + 1;
        props.rcDestination.top = static_cast<LONG>(std::lround(cy - stripH / 2.0));
        props.rcDestination.bottom = props.rcDestination.top + stripH;
        // Source strip (full height). Same 1px overlap trick.
        props.rcSource.left = (j * src.cx) / M;
        props.rcSource.right = ((j + 1) * src.cx) / M + 1;
        props.rcSource.top = 0;
        props.rcSource.bottom = src.cy;
        props.opacity = entry.currentOpacity;
        props.fVisible = cardVisible;
        props.fSourceClientAreaOnly = FALSE;

        // Mirror the real DWM validation (see the Win7 decompilation notes):
        // never send inverted rects; hide the strip instead.
        if (props.rcDestination.right < props.rcDestination.left ||
            props.rcDestination.bottom < props.rcDestination.top ||
            props.rcSource.right < props.rcSource.left ||
            props.rcSource.bottom < props.rcSource.top) {
            props.rcDestination = {0, 0, 0, 0};
            props.rcSource = {0, 0, 0, 0};
            props.fVisible = FALSE;
        }

        if (FAILED(DwmUpdateThumbnailProperties(entry.slices[j].get(), &props))) {
            ++failedStrips;
        }
    }

    if (failedStrips == 0) {
        entry.updateFailures = 0;
    } else if (failedStrips == M && ++entry.updateFailures >= kMaxThumbnailUpdateFailures) {
        // Every strip of the card is failing: the source window is likely a
        // zombie. Abandon strips and fall back to a single flat thumbnail
        // (which has its own invalidation path).
        entry.updateFailures = 0;
        FallbackToSingleThumbnail(entry);
    }
}

static void ApplyDesktopThumbnailProperties() {
    if (!g_hOverlayWnd || !IsWindow(g_hOverlayWnd) || !g_desktopThumbnail) {
        return;
    }

    RECT clientRect = {};
    if (!GetClientRect(g_hOverlayWnd, &clientRect)) {
        return;
    }

    DWM_THUMBNAIL_PROPERTIES props = {};
    props.dwFlags = DWM_TNP_RECTDESTINATION |
                    DWM_TNP_VISIBLE |
                    DWM_TNP_OPACITY |
                    DWM_TNP_SOURCECLIENTAREAONLY;
    props.rcDestination = clientRect;
    props.opacity = 255;
    props.fVisible = TRUE;
    props.fSourceClientAreaOnly = FALSE;

    DwmUpdateThumbnailProperties(g_desktopThumbnail, &props);
}

static void UnregisterDesktopThumbnail() {
    if (g_desktopThumbnail) {
        DwmUnregisterThumbnail(g_desktopThumbnail);
        g_desktopThumbnail = nullptr;
    }
    g_hDesktopThumbnailSource = nullptr;
}

static void RegisterDesktopThumbnail() {
    UnregisterDesktopThumbnail();

    if (!kEnableDesktopDwmBackdrop) {
        return;
    }
    if (!g_hOverlayWnd || !IsWindow(g_hOverlayWnd)) {
        return;
    }

    HWND desktopSource = FindDesktopThumbnailSourceWindow();
    if (!desktopSource || !IsWindow(desktopSource) || desktopSource == g_hOverlayWnd) {
        return;
    }

    HTHUMBNAIL thumbnail = nullptr;
    HRESULT hr = DwmRegisterThumbnail(g_hOverlayWnd, desktopSource, &thumbnail);
    if (SUCCEEDED(hr) && thumbnail) {
        g_desktopThumbnail = thumbnail;
        g_hDesktopThumbnailSource = desktopSource;
        ApplyDesktopThumbnailProperties();
    }
}

inline std::vector<int> BuildBackToFrontOrder() {
    const int count = static_cast<int>(g_windows.size());
    std::vector<int> order;
    try {
        order.reserve(count > 0 ? count : 1);
    } catch (...) {
        // Non-fatal.
    }
    for (int i = 0; i < count; ++i) {
        order.push_back(i);
    }

    std::sort(order.begin(), order.end(), [count](int a, int b) {
        int da = StackDepthForIndex(a, g_selectedIndex, count);
        int db = StackDepthForIndex(b, g_selectedIndex, count);
        if (da != db) {
            return da > db;  // Deepest first, selected/front window last.
        }
        return a < b;
    });

    return order;
}

static void ApplyAllThumbnailProperties() {
    if (g_windows.empty()) {
        return;
    }
    std::vector<int> order = BuildBackToFrontOrder();
    const int winCount = static_cast<int>(g_windows.size());
    for (int index : order) {
        if (index < 0 || index >= winCount) {
            continue;  // Defensive bounds check.
        }
        ApplyCardThumbnails(g_windows[index]);
    }
}

static void RebuildThumbnailZOrder() {
    if (!g_hOverlayWnd || !IsWindow(g_hOverlayWnd) || g_windows.empty()) {
        return;
    }

    // DWM thumbnails don't expose an explicit z-order API. Updating properties
    // isn't always enough on Win10/11: re-register only when the selection
    // changes, in back-to-front order, so the selected card's strips are the
    // last registered ones. Within a card, strips are registered left ->
    // right so the near (left) edge renders on top. Each card's strip count
    // depends on its stack depth (front cards get perspective, deep cards go
    // flat) to keep the per-frame ALPC budget low.
    std::vector<int> order = BuildBackToFrontOrder();

    const int count = static_cast<int>(g_windows.size());

    for (auto& entry : g_windows) {
        entry.thumbnail.reset();
        entry.slices.clear();
        entry.stripCount = 0;
        entry.stripsHidden = false;
    }

    for (int index : order) {
        if (index < 0 || index >= count) {
            continue;
        }
        auto& entry = g_windows[index];
        if (!entry.hwnd || !IsWindow(entry.hwnd)) {
            continue;
        }

        const int depth = StackDepthForIndex(index, g_selectedIndex, count);
        const int stripsWanted = StripCountForDepth(depth);

        // Each card is isolated: a fault while registering one card's strips
        // must not abort the whole rebuild (it runs on the hook thread).
        try {
            if (stripsWanted > 0) {
                bool allStripsOk = true;
                std::vector<ThumbnailHandle> strips;
                try {
                    strips.reserve(stripsWanted);
                } catch (...) {
                    // Non-fatal.
                }

                for (int j = 0; j < stripsWanted; ++j) {
                    ThumbnailHandle th;
                    HRESULT hr = DwmRegisterThumbnail(g_hOverlayWnd, entry.hwnd, th.put());
                    if (FAILED(hr) || !th) {
                        allStripsOk = false;
                        break;  // This card will use the flat fallback below.
                    }
                    strips.push_back(std::move(th));
                }

                if (allStripsOk && static_cast<int>(strips.size()) == stripsWanted) {
                    entry.slices = std::move(strips);
                    entry.stripCount = stripsWanted;
                    DwmQueryThumbnailSourceSize(entry.slices[0].get(), &entry.sourceSize);
                    ApplyCardThumbnails(entry);
                    continue;
                }
                strips.clear();  // Drop partial strips -> flat fallback.
            }

            // Flat single-thumbnail path (deep cards or failed strip setup).
            entry.stripCount = 0;
            HRESULT hr = DwmRegisterThumbnail(g_hOverlayWnd, entry.hwnd, entry.thumbnail.put());
            if (SUCCEEDED(hr) && entry.thumbnail) {
                DwmQueryThumbnailSourceSize(entry.thumbnail.get(), &entry.sourceSize);
                ApplySingleThumbnailProperties(entry);
            }
        } catch (...) {
            // A single misbehaving window must never take the rebuild down.
            LogFailure(L"RebuildThumbnailZOrder");
            entry.slices.clear();
            entry.thumbnail.reset();
            entry.stripCount = 0;
        }
        // Registration failure for a single window is non-fatal: the card
        // is simply skipped (or runs flat). No per-call logging.
    }
}

static void UpdateRefreshTimerNow(bool enable) {
    if (!g_hOverlayWnd || !IsWindow(g_hOverlayWnd)) {
        return;
    }

    if (enable) {
        if (!g_animationTimerId) {
            g_animationTimerId = SetTimer(g_hOverlayWnd, kAnimationTimerId,
                                          kAnimationTimerIntervalMs, nullptr);
            if (!g_animationTimerId) {
                LogFailureCode(L"SetTimer(animation)", GetLastError());
            }
        }
    } else if (g_animationTimerId) {
        KillTimer(g_hOverlayWnd, g_animationTimerId);
        g_animationTimerId = 0;
    }
}

static void RequestUpdateRefreshTimer(bool enable) {
    if (!g_hOverlayWnd || !IsWindow(g_hOverlayWnd)) {
        return;
    }

    if (GetCurrentThreadId() == g_overlayThreadId) {
        UpdateRefreshTimerNow(enable);
    } else {
        PostMessageW(g_hOverlayWnd, WM_FLIP3D_UPDATE_TIMER, enable ? 1 : 0, 0);
    }
}

static void UpdateAnimationFrame() {
    if (!g_animationInProgress) {
        return;
    }

    ULONGLONG now = GetTickCount64();
    ULONGLONG elapsed = now >= g_animationStartTick ? now - g_animationStartTick : 0;
    DWORD duration = std::max<DWORD>(1, g_activeAnimationDurationMs);
    double linear = static_cast<double>(elapsed) / static_cast<double>(duration);
    bool done = linear >= 1.0;
    double eased = done ? 1.0 : EaseOutCubic(linear);

    for (auto& entry : g_windows) {
        entry.currentRect = done ? entry.targetRect : LerpRect(entry.startRect, entry.targetRect, eased);
        entry.currentOpacity = done ? entry.targetOpacity : LerpOpacity(entry.startOpacity, entry.targetOpacity, eased);
        entry.currentTilt = done ? entry.targetTilt
                                 : entry.startTilt + (entry.targetTilt - entry.startTilt) * eased;
    }

    ApplyAllThumbnailProperties();
    if (g_hOverlayWnd && IsWindow(g_hOverlayWnd)) {
        InvalidateRect(g_hOverlayWnd, nullptr, FALSE);
    }

    if (done) {
        FlipAnimationKind finishedKind = g_animationKind;
        g_animationInProgress = false;
        g_animationKind = FlipAnimationKind::None;
        RequestUpdateRefreshTimer(false);

        if (finishedKind == FlipAnimationKind::Exit) {
            FinishExitAfterAnimation();
        }
    }
}

static void BeginTransitionFromCurrent(FlipAnimationKind kind, DWORD durationMs) {
    for (auto& entry : g_windows) {
        entry.startRect = entry.currentRect;
        entry.startOpacity = entry.currentOpacity;
        entry.startTilt = entry.currentTilt;
    }

    g_animationKind = kind;
    g_activeAnimationDurationMs = durationMs;
    g_animationStartTick = GetTickCount64();
    g_animationInProgress = true;
    RequestUpdateRefreshTimer(true);
}

static void CleanupThumbnails() {
    for (auto& entry : g_windows) {
        entry.thumbnail.reset();
        entry.slices.clear();  // Each ThumbnailHandle unregisters its strip.
    }
    g_windows.clear();
}

// -----------------------------------------------------------------------------
// Windows 7 Flip 3D geometry
//
// Real Flip 3D laid the thumbnails out as cards in an actual 3D scene viewed
// by a perspective camera: the deck recedes away from the viewer along -Z
// and arcs toward the upper-left, with the selected card swung forward to
// face the camera almost head-on in the lower-right foreground.
//
// Rather than approximate that with hand-tuned pixel offsets per depth
// (which tends to either clump cards in the middle or fan them out
// unnaturally at the edges), each card is placed at a 3D position and run
// through a real perspective projection, the same way DWM's own compositor
// would have. The on-screen offsets fall out of the projection math instead
// of being guessed per depth level, so the foreshortening (cards shrink
// faster the further back they are, spacing compresses toward the vanishing
// point) matches a real camera instead of a linear/exponential ramp.
// -----------------------------------------------------------------------------

namespace Flip3DGeometry {

// Virtual camera tuned to match the field of view / card density visible in
// real Windows 7 Flip 3D captures: a fairly tight FOV so the perspective
// convergence is clearly visible (cards noticeably shrink and tilt as they
// recede) without becoming a fisheye distortion.
constexpr double kFovYDegrees = 30.0;
constexpr double kCameraDistance = 3.0;   // Distance from camera to Z=0 plane.
constexpr double kCardDepthStep = 0.62;   // World-Z distance between adjacent cards.
constexpr double kCardArcStep = 0.80;     // World-X drift per depth step (toward upper-left).
constexpr double kCardRiseStep = 0.30;    // World-Y drift per depth step (toward the top).
constexpr double kFrontCardWorldHeight = 1.55;  // World height of the front (selected) card.
constexpr double kMaxDepthForFalloff = 7.0;      // Depths beyond this fully fade out.

// Perspective projection of a point in camera space (x right, y up, z forward
// into the screen, camera at the origin looking down +Z) onto normalized
// device coordinates.
struct Projected {
    double ndcX;
    double ndcY;
    double perspectiveScale;  // 1 / z, i.e. how much a unit length shrinks here.
};

inline Projected Project(double worldX, double worldY, double worldZ, double fovYRadians) {
    double z = std::max(0.05, worldZ);  // Guard against divide-by-zero/behind-camera.
    double tanHalfFov = std::tan(fovYRadians * 0.5);
    Projected p;
    p.ndcY = worldY / (z * tanHalfFov);
    p.ndcX = worldX / (z * tanHalfFov);  // Square-ish projection window for the deck area.
    p.perspectiveScale = 1.0 / z;
    return p;
}

}  // namespace Flip3DGeometry

static void ComputeStackLayout(std::vector<FlipWindowEntry>& windows,
                               int selectedIndex,
                               bool desktopSelected,
                               int clientW,
                               int clientH,
                               UINT dpi) {
    using namespace Flip3DGeometry;

    const int count = static_cast<int>(windows.size());
    if (count <= 0 || clientW <= 0 || clientH <= 0) {
        return;
    }

    selectedIndex = ClampInt(selectedIndex, 0, count - 1);
    const int maxVisibleDepth = std::min(count, 8) + (desktopSelected ? 1 : 0);

    // Screen-space anchor for the projected origin: Flip 3D keeps the
    // selected card low and to the right, with the deck receding to the
    // upper-left, so the projection's neutral point is placed accordingly
    // rather than dead-centre.
    const int originX = clientW * 58 / 100;
    const int originY = clientH * 58 / 100;

    const double fovYRadians = kFovYDegrees * (3.14159265358979323846 / 180.0);
    const double tanHalfFov = std::tan(fovYRadians * 0.5);
    const double frontZ = kCameraDistance;

    // desiredFrontHeightPx ties the virtual camera's scale to the actual
    // overlay size, so the front card lands at a comfortable, DPI-scaled
    // fraction of the screen (real Flip 3D used roughly 40-48% of width for
    // the foreground card).
    const int desiredFrontHeightPx = std::max(ScaleForDpi(380, dpi), clientH * 55 / 100);
    const double pixelsPerNdcYAtFront = desiredFrontHeightPx /
        (kFrontCardWorldHeight / (frontZ * tanHalfFov));
    const int minW = ScaleForDpi(130, dpi);

    for (int i = 0; i < count; ++i) {
        int depth = StackDepthForIndex(i, selectedIndex, count);
        if (desktopSelected) {
            depth += 1;
        }
        int visualDepth = std::min(depth, maxVisibleDepth - 1);
        double d = static_cast<double>(visualDepth);

        // 1. Place this card in 3D world space: it recedes along +Z and
        // drifts toward the upper-left, matching the real fan-back of the
        // deck.
        double worldX = -d * kCardArcStep;
        double worldY = d * kCardRiseStep;
        double worldZ = kCameraDistance + d * kCardDepthStep;

        Projected proj = Project(worldX, worldY, worldZ, fovYRadians);

        // 2. Aspect ratio from the real source window, so cards keep their
        // true window proportions instead of being forced to 16:9.
        double aspect = 16.0 / 9.0;
        if (windows[i].sourceSize.cx > 0 && windows[i].sourceSize.cy > 0) {
            aspect = static_cast<double>(windows[i].sourceSize.cx) /
                     static_cast<double>(windows[i].sourceSize.cy);
        }

        // World height shrinks slightly for genuinely small/thin source
        // windows so tiny dialogs don't get blown up to the same size as a
        // maximized window sitting at the same depth.
        double worldHeight = kFrontCardWorldHeight;
        if (windows[i].sourceSize.cx > 0 && windows[i].sourceSize.cx < 700) {
            double sizeRatio = static_cast<double>(windows[i].sourceSize.cx) / 700.0;
            worldHeight *= 0.72 + 0.28 * sizeRatio;
        }

        // 3. Project world height to a pixel height using the perspective
        // scale at this card's depth: this is what makes deeper cards shrink
        // like a real camera view (faster falloff up close, gentler far
        // away) instead of a flat exponential per depth step.
        double ndcHeight = worldHeight * proj.perspectiveScale / tanHalfFov;
        int h = std::max(minW, static_cast<int>(std::lround(ndcHeight * pixelsPerNdcYAtFront)));
        int w = static_cast<int>(std::lround(h * aspect));
        w = std::max(minW, w);

        int cx = originX + static_cast<int>(std::lround(proj.ndcX * pixelsPerNdcYAtFront));
        int cy = originY - static_cast<int>(std::lround(proj.ndcY * pixelsPerNdcYAtFront));

        // 4. Target 2D rectangle for the DWM thumbnail.
        windows[i].targetRect = {
            cx - w / 2,
            cy - h / 2,
            cx + w / 2,
            cy + h / 2,
        };

        // 5. Perspective tilt: derived from the same depth-driven curve
        // rather than a separate hand-tuned ramp, so the card's apparent
        // rotation stays consistent with how far back the projection placed
        // it. The front card stays almost face-on; deeper cards swing to
        // roughly 30-35 degrees, matching the real fan. Disabled in flat
        // mode.
        windows[i].targetTilt = g_settings.perspective
                                    ? std::min(0.55, 0.10 + 0.075 * d)
                                    : 0.0;

        // 6. Per-card opacity gradient: fades out smoothly toward
        // kMaxDepthForFalloff instead of a hard cutoff, closer to the real
        // Flip 3D deck fading into haze at the back.
        if (depth >= maxVisibleDepth) {
            windows[i].targetOpacity = 0;
        } else {
            double falloff = std::min(1.0, std::max(0.0, 1.0 - d / kMaxDepthForFalloff));
            windows[i].targetOpacity = ClampByte(static_cast<int>(std::lround(180 + 75.0 * falloff)));
        }
    }
}

static void RecomputeTargetsForCurrentSelection() {
    if (!g_hOverlayWnd || !IsWindow(g_hOverlayWnd) || g_windows.empty()) {
        return;
    }

    RECT clientRect = {};
    if (!GetClientRect(g_hOverlayWnd, &clientRect)) {
        LogDebug(L"RecomputeTargetsForCurrentSelection: GetClientRect failed");
        return;
    }

    UINT dpi = GetDpiForOverlayWindow(g_hOverlayWnd);
    LogDebugFmt(L"RecomputeTargetsForCurrentSelection: selectedIndex=%d desktopSelected=%d "
                L"clientRect=%dx%d dpi=%u cardCount=%u",
                g_selectedIndex, g_desktopSelected ? 1 : 0,
                RectWidth(clientRect), RectHeight(clientRect), dpi, static_cast<unsigned>(g_windows.size()));

    ComputeStackLayout(g_windows,
                       g_selectedIndex,
                       g_desktopSelected,
                       RectWidth(clientRect),
                       RectHeight(clientRect),
                       dpi);
}

static void NavigateSelection(int delta) {
    const int count = static_cast<int>(g_windows.size());
    if (count <= 0 || delta == 0 || g_exitInProgress) {
        return;
    }

    ULONGLONG now = GetTickCount64();
    ULONGLONG lastNavigationTick = g_lastNavigationTick.load(std::memory_order_relaxed);
    if (lastNavigationTick && now - lastNavigationTick < kNavigationDebounceMs) {
        return;
    }
    g_lastNavigationTick.store(now, std::memory_order_relaxed);

    if (g_animationInProgress) {
        UpdateAnimationFrame();
    }

    delta %= count;
    if (g_desktopSelected) {
        g_desktopSelected = false;
        if (delta > 0) {
            // Select the window AFTER the active one, not index 0.
            if (g_activeWindowIndex >= 0 && g_activeWindowIndex < count) {
                g_selectedIndex = (g_activeWindowIndex + 1) % count;
            } else {
                g_selectedIndex = 0;
            }
        } else {
            g_selectedIndex = count - 1;
        }
    } else {
        g_selectedIndex = (g_selectedIndex + delta + count) % count;
    }

    RecomputeTargetsForCurrentSelection();
    RebuildThumbnailZOrder();
    BeginTransitionFromCurrent(FlipAnimationKind::Layout, kLayoutAnimationDurationMs);
}

// -----------------------------------------------------------------------------
// Overlay lifecycle
// -----------------------------------------------------------------------------

static HWND CreateOverlayWindow() {
    static bool classRegistered = false;
    const wchar_t* className = L"Flip3DOverlayWndClass";
    HINSTANCE hInstance = GetCurrentModuleHandle();
    if (!hInstance) {
        return nullptr;
    }

    if (!classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = OverlayWndProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wc.lpszClassName = className;

        if (RegisterClassExW(&wc) || GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
            classRegistered = true;
        } else {
            LogFailureCode(L"RegisterClassExW(overlay)", GetLastError());
            return nullptr;
        }
    }

    RECT monitorRect = GetPrimaryMonitorRect();
    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                                className,
                                L"Aero Flip 3D Recreation",
                                WS_POPUP,
                                monitorRect.left,
                                monitorRect.top,
                                std::max(1, RectWidth(monitorRect)),
                                std::max(1, RectHeight(monitorRect)),
                                nullptr,
                                nullptr,
                                hInstance,
                                nullptr);
    if (!hwnd) {
        LogFailureCode(L"CreateWindowExW(overlay)", GetLastError());
        return nullptr;
    }

    // Not layered: a normal opaque popup repaints more reliably. Earlier
    // layered/GDI+/desktop-thumbnail experiments caused flashes or Explorer
    // restarts on some Windows 10/11 systems.
    return hwnd;
}

static void ActivateSelectedWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }

    HWND foreground = GetForegroundWindow();
    DWORD currentThread = GetCurrentThreadId();
    DWORD foregroundThread = (foreground && IsWindow(foreground))
                                 ? GetWindowThreadProcessId(foreground, nullptr)
                                 : 0;
    DWORD targetThread = GetWindowThreadProcessId(hwnd, nullptr);

    bool attachedForeground = false;
    bool attachedTarget = false;

    if (foregroundThread && foregroundThread != currentThread) {
        attachedForeground = AttachThreadInput(currentThread, foregroundThread, TRUE) != FALSE;
    }
    if (targetThread && targetThread != currentThread && targetThread != foregroundThread) {
        attachedTarget = AttachThreadInput(currentThread, targetThread, TRUE) != FALSE;
    }

    BringWindowToTop(hwnd);
    SetForegroundWindow(hwnd);

    if (attachedTarget) {
        AttachThreadInput(currentThread, targetThread, FALSE);
    }
    if (attachedForeground) {
        AttachThreadInput(currentThread, foregroundThread, FALSE);
    }
}

static void CleanupFlipResourcesOnOverlayThread() {
    UpdateRefreshTimerNow(false);
    if (g_hOverlayWnd && IsWindow(g_hOverlayWnd)) {
        KillTimer(g_hOverlayWnd, kFailsafeTimerId);
        KillTimer(g_hOverlayWnd, kAutoCycleTimerId);
    }
    g_animationInProgress = false;
    g_animationKind = FlipAnimationKind::None;
    g_exitInProgress = false;
    CleanupThumbnails();
    UnregisterDesktopThumbnail();
    ReleaseDesktopSnapshot();
    g_selectedIndex = 0;
    g_desktopSelected = false;
    g_initialForegroundHwnd = nullptr;
    g_isActive = false;
    g_triggerModifier = TriggerModifier::None;
    g_pendingActivateHwnd = nullptr;
    g_hookSessionActive.store(false, std::memory_order_relaxed);
    ResetWheelState();
    g_persistentMode = false;
    g_activeWindowIndex = -1;
}

// Destroys the overlay with a fallback. If DestroyWindow unexpectedly fails
// we hide the window so the screen is restored; we NEVER hand-post WM_DESTROY
// (that message must only be generated by the system from DestroyWindow, or
// the window would leak and cleanup state could be processed twice).
static void SafeDestroyOverlayWindow(HWND overlay) {
    if (!overlay || !IsWindow(overlay)) {
        return;
    }
    if (!DestroyWindow(overlay)) {
        LogFailureCode(L"DestroyWindow(overlay)", GetLastError());
        ShowWindow(overlay, SW_HIDE);
    }
}

static void FinishExitAfterAnimation() {
    HWND selectedHwnd = g_pendingActivateHwnd;
    HWND overlay = g_hOverlayWnd;

    CleanupFlipResourcesOnOverlayThread();

    SafeDestroyOverlayWindow(overlay);

    g_hOverlayWnd = nullptr;
    g_overlayThreadId = 0;

    if (selectedHwnd && IsWindow(selectedHwnd)) {
        ActivateSelectedWindow(selectedHwnd);
    }
}

static void DeactivateFlip3D(bool activateSelected) {
    if (g_hOverlayWnd && IsWindow(g_hOverlayWnd) &&
        GetCurrentThreadId() != g_overlayThreadId) {
        SendMessageW(g_hOverlayWnd, WM_FLIP3D_CLOSE, activateSelected ? 1 : 0, 0);
        return;
    }

    if (!g_hOverlayWnd || !IsWindow(g_hOverlayWnd)) {
        g_hookSessionActive.store(false, std::memory_order_relaxed);
        return;
    }

    if (g_exitInProgress) {
        return;
    }

    LogDebugFmt(L"DeactivateFlip3D: closing (activateSelected=%d)", activateSelected ? 1 : 0);

    HWND selectedHwnd = nullptr;
    if (activateSelected) {
        if (g_desktopSelected) {
            selectedHwnd = g_initialForegroundHwnd;
        } else if (g_selectedIndex >= 0 && g_selectedIndex < static_cast<int>(g_windows.size())) {
            selectedHwnd = g_windows[g_selectedIndex].hwnd;
        }
    }
    if (selectedHwnd && !IsWindow(selectedHwnd)) {
        selectedHwnd = nullptr;
    }

    if (g_windows.empty()) {
        g_pendingActivateHwnd = selectedHwnd;
        FinishExitAfterAnimation();
        return;
    }

    // Exit morph: stack -> original desktop rects while fading out. The real
    // desktop is hidden by the opaque overlay until the final destroy.
    if (g_animationInProgress) {
        UpdateAnimationFrame();
    }

    RECT clientRect = {};
    if (!GetClientRect(g_hOverlayWnd, &clientRect)) {
        clientRect = {0, 0, 1, 1};
    }

    for (auto& entry : g_windows) {
        RECT dst = MakeOverlayRelativeRect(entry.sourceRect);
        if (!IntersectsRect(dst, clientRect)) {
            int cx = (entry.currentRect.left + entry.currentRect.right) / 2;
            int cy = (entry.currentRect.top + entry.currentRect.bottom) / 2;
            dst = CenterRectWithSameSize(entry.currentRect, cx, cy);
        }

        entry.targetRect = dst;
        entry.targetOpacity = 0;
        entry.targetTilt = 0.0;  // Cards flatten while morphing back home.
    }

    g_pendingActivateHwnd = selectedHwnd;
    g_exitInProgress = true;
    g_isActive = false;
    g_hookSessionActive.store(false, std::memory_order_relaxed);
    BeginTransitionFromCurrent(FlipAnimationKind::Exit, kExitAnimationDurationMs);
}

static bool ActivateFlip3DImpl(TriggerModifier triggerModifier, int initialDelta) {
    if (g_isActive) {
        NavigateSelection(initialDelta == 0 ? 1 : initialDelta);
        return true;
    }

    if (g_exitInProgress || (g_hOverlayWnd && IsWindow(g_hOverlayWnd))) {
        LogDebug(L"ActivateFlip3D: skipped (exit in progress or overlay already exists)");
        return true;
    }

    BOOL compositionEnabled = FALSE;
    if (FAILED(DwmIsCompositionEnabled(&compositionEnabled)) || !compositionEnabled) {
        g_hookSessionActive.store(false, std::memory_order_relaxed);
        LogFailure(L"ActivateFlip3D: DWM composition disabled");
        return false;
    }

    HWND foregroundBefore = GetForegroundWindow();
    std::vector<HWND> eligible = EnumerateFlipEligibleWindows();
    LogDebugFmt(L"ActivateFlip3D: %u eligible top-level window(s) found", static_cast<unsigned>(eligible.size()));
    if (eligible.size() < 2) {
        g_hookSessionActive.store(false, std::memory_order_relaxed);
        LogFailure(L"ActivateFlip3D: not enough eligible windows (need >= 2)");
        return false;
    }

    // Do NOT minimize the foreground window: the opaque overlay + desktop
    // snapshot provide the backdrop (no unwanted minimize animation).
    g_hOverlayWnd = CreateOverlayWindow();
    if (!g_hOverlayWnd) {
        g_hookSessionActive.store(false, std::memory_order_relaxed);
        LogFailure(L"ActivateFlip3D: CreateOverlayWindow returned null");
        return false;
    }

    // Overlay exists but is not visible yet: grab the real desktop now.
    CaptureDesktopSnapshot();

    g_overlayThreadId = GetCurrentThreadId();
    g_isActive = true;
    g_triggerModifier = triggerModifier;
    g_hookSessionActive.store(true, std::memory_order_relaxed);
    ClearPendingModifierReleaseSuppression();
    ResetWheelState();

    RegisterDesktopThumbnail();

    g_windows.clear();
    int skippedNoRect = 0;
    int skippedThumbnailFailed = 0;
    for (HWND hwnd : eligible) {
        if (!hwnd || !IsWindow(hwnd) || !IsFlipEligibleWindow(hwnd)) {
            continue;
        }

        FlipWindowEntry entry;
        entry.hwnd = hwnd;
        if (!GetRepresentativeWindowRect(hwnd, &entry.sourceRect)) {
            ++skippedNoRect;
            continue;
        }

        HRESULT hr = DwmRegisterThumbnail(g_hOverlayWnd, hwnd, entry.thumbnail.put());
        if (FAILED(hr) || !entry.thumbnail) {
            ++skippedThumbnailFailed;
            LogDebugFmt(L"ActivateFlip3D: DwmRegisterThumbnail failed for hwnd=0x%p hr=0x%08X",
                        hwnd, static_cast<unsigned int>(hr));
            continue;  // Skip this window; non-fatal.
        }

        DwmQueryThumbnailSourceSize(entry.thumbnail.get(), &entry.sourceSize);
        g_windows.push_back(std::move(entry));
    }

    LogDebugFmt(L"ActivateFlip3D: %u thumbnail(s) registered (skipped: %d no-rect, %d thumbnail-failed)",
                static_cast<unsigned>(g_windows.size()), skippedNoRect, skippedThumbnailFailed);

    if (g_windows.empty()) {
        LogFailure(L"ActivateFlip3D: no thumbnails registered");
        DeactivateFlip3D(false);
        return false;
    }

    int foregroundIndex = 0;
    for (int i = 0; i < static_cast<int>(g_windows.size()); ++i) {
        if (g_windows[i].hwnd == foregroundBefore) {
            foregroundIndex = i;
            break;
        }
    }

    // Win7-style entry: Flip 3D opens showing the desktop itself in front.
    g_initialForegroundHwnd = foregroundBefore;
    g_activeWindowIndex = foregroundIndex;
    g_desktopSelected = true;
    g_selectedIndex = 0;

    RecomputeTargetsForCurrentSelection();

    RECT clientRect = {};
    if (!GetClientRect(g_hOverlayWnd, &clientRect)) {
        clientRect = {0, 0, 1, 1};
    }

    for (auto& entry : g_windows) {
        RECT startRect = MakeOverlayRelativeRect(entry.sourceRect);
        if (!IntersectsRect(startRect, clientRect)) {
            int cx = (entry.targetRect.left + entry.targetRect.right) / 2;
            int cy = (entry.targetRect.top + entry.targetRect.bottom) / 2;
            startRect = CenterRectWithSameSize(entry.targetRect, cx, cy);
        }

        entry.currentRect = startRect;
        entry.startRect = startRect;
        entry.currentOpacity = 0;
        entry.startOpacity = 0;
        // Entry morph: cards start flat and rotate into the deck while
        // flying to their stack slots. ComputeStackLayout already set
        // targetTilt = 1.0 above; only the starting pose is flat here.
        entry.currentTilt = 0.0;
        entry.startTilt = 0.0;
        entry.stripsHidden = false;
        entry.stripCount = 0;  // Set properly by RebuildThumbnailZOrder() below.
    }

    ShowWindow(g_hOverlayWnd, SW_SHOW);
    UpdateWindow(g_hOverlayWnd);

    if (!SetTimer(g_hOverlayWnd, kFailsafeTimerId, kFailsafeAutoCloseMs, nullptr)) {
        LogFailureCode(L"SetTimer(failsafe)", GetLastError());
    }
    if (!g_persistentMode) {
        // Win7-style: auto-cycle while Win stays held.
        if (!SetTimer(g_hOverlayWnd, kAutoCycleTimerId, kAutoCycleIntervalMs, nullptr)) {
            LogFailureCode(L"SetTimer(autocycle)", GetLastError());
        }
    }

    SetWindowPos(g_hOverlayWnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetForegroundWindow(g_hOverlayWnd);
    SetFocus(g_hOverlayWnd);

    RebuildThumbnailZOrder();
    ApplyAllThumbnailProperties();
    BeginTransitionFromCurrent(FlipAnimationKind::Entry, kEntryAnimationDurationMs);
    LogDebugFmt(L"ActivateFlip3D: switcher opened with %u card(s), persistent=%d",
                static_cast<unsigned>(g_windows.size()), g_persistentMode ? 1 : 0);
    return true;
}

// Guarded activation: ActivateFlip3DImpl enumerates arbitrary top-level
// windows and registers DWM thumbnails for each of them — exactly the kind of
// cross-process interaction that can fail in unexpected ways. Any exception
// is caught here, logged once, cleaned up, and the switcher simply does not
// open; explorer.exe keeps running.
static bool ActivateFlip3D(TriggerModifier triggerModifier, int initialDelta) {
    FLIP3D_FENCE_BEGIN("ActivateFlip3D")
        try {
            if (g_hOverlayWnd && IsWindow(g_hOverlayWnd) &&
                GetCurrentThreadId() == g_overlayThreadId) {
                CleanupFlipResourcesOnOverlayThread();
                SafeDestroyOverlayWindow(g_hOverlayWnd);
            }
        } catch (...) {
        }
        g_hOverlayWnd = nullptr;
        g_overlayThreadId = 0;
        g_isActive = false;
        g_hookSessionActive.store(false, std::memory_order_relaxed);
        return false;
    FLIP3D_FENCE_ELSE
    try {
        return ActivateFlip3DImpl(triggerModifier, initialDelta);
    } catch (...) {
        LogFailure(L"ActivateFlip3D");
        try {
            if (g_hOverlayWnd && IsWindow(g_hOverlayWnd) &&
                GetCurrentThreadId() == g_overlayThreadId) {
                CleanupFlipResourcesOnOverlayThread();
                SafeDestroyOverlayWindow(g_hOverlayWnd);
            }
        } catch (...) {
        }
        g_hOverlayWnd = nullptr;
        g_overlayThreadId = 0;
        g_isActive = false;
        g_hookSessionActive.store(false, std::memory_order_relaxed);
        return false;
    }
    FLIP3D_FENCE_END
}

// -----------------------------------------------------------------------------
// Overlay window procedure (guarded boundary: no exception may escape)
// -----------------------------------------------------------------------------

static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    FLIP3D_FENCE_BEGIN("OverlayWndProc")
        if (msg == WM_PAINT) {
            ValidateRect(hwnd, nullptr);  // Prevent a WM_PAINT storm after a fault.
            return 0;
        }
        if (msg == WM_NCDESTROY || msg == WM_DESTROY) {
            if (hwnd == g_hOverlayWnd) {
                g_hOverlayWnd = nullptr;
                g_overlayThreadId = 0;
            }
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    FLIP3D_FENCE_ELSE
        try {
            switch (msg) {
            case WM_ERASEBKGND:
                PaintStaticWallpaperBackdrop(hwnd, reinterpret_cast<HDC>(wParam));
                DrawFaux3DFrames(hwnd, reinterpret_cast<HDC>(wParam));
                return 1;

            case WM_PAINT: {
                // ScopedPaint guarantees EndPaint even if painting throws.
                ScopedPaint paint(hwnd);
                if (HDC hdc = paint.get()) {
                    PaintStaticWallpaperBackdrop(hwnd, hdc);
                    DrawFaux3DFrames(hwnd, hdc);
                }
                return 0;
            }

            case WM_SHOWWINDOW:
            case WM_SIZE:
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;

            case WM_MOUSEWHEEL: {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                NavigateSelection(delta > 0 ? -1 : 1);
                return 0;
            }

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN: {
                switch (wParam) {
                    case VK_ESCAPE:
                        PreparePendingModifierReleaseSuppression();
                        DeactivateFlip3D(false);
                        return 0;

                    case VK_RETURN:
                    case VK_SPACE:
                        PreparePendingModifierReleaseSuppression();
                        DeactivateFlip3D(true);
                        return 0;

                    case VK_LEFT:
                    case VK_UP:
                        NavigateSelection(-1);
                        return 0;

                    case VK_RIGHT:
                    case VK_DOWN:
                    case VK_TAB:
                        NavigateSelection(1);
                        return 0;
                }
                break;
            }

            case WM_FLIP3D_NAVIGATE:
                NavigateSelection(static_cast<int>(static_cast<INT_PTR>(wParam)));
                return 0;

            case WM_FLIP3D_CLOSE:
                DeactivateFlip3D(wParam != 0);
                return 0;

            case WM_FLIP3D_UPDATE_TIMER:
                UpdateRefreshTimerNow(wParam != 0);
                return 0;

            case WM_FLIP3D_REBUILD:
                // Live re-style after a settings change: recompute the deck
                // (new tilt/strip budget) and rebuild thumbnails on this
                // thread, then play a short layout transition.
                RecomputeTargetsForCurrentSelection();
                RebuildThumbnailZOrder();
                BeginTransitionFromCurrent(FlipAnimationKind::Layout, kLayoutAnimationDurationMs);
                return 0;

            case WM_TIMER:
                if (wParam == kAnimationTimerId || wParam == g_animationTimerId) {
                    UpdateAnimationFrame();
                    return 0;
                }
                if (wParam == kFailsafeTimerId) {
                    DeactivateFlip3D(false);
                    return 0;
                }
                if (wParam == kAutoCycleTimerId) {
                    if (g_isActive && !g_persistentMode) {
                        NavigateSelection(1);
                    } else if (g_hOverlayWnd && IsWindow(g_hOverlayWnd)) {
                        KillTimer(g_hOverlayWnd, kAutoCycleTimerId);
                    }
                    return 0;
                }
                break;

            case WM_DESTROY:
                CleanupFlipResourcesOnOverlayThread();
                return 0;

            case WM_NCDESTROY:
                if (hwnd == g_hOverlayWnd) {
                    g_hOverlayWnd = nullptr;
                    g_overlayThreadId = 0;
                }
                return 0;
        }
    } catch (...) {
        // Boundary guard: log once, neutralize the message, never crash.
        LogFailure(L"OverlayWndProc");
        if (msg == WM_PAINT) {
            ValidateRect(hwnd, nullptr);  // Prevent a WM_PAINT storm.
            return 0;
        }
        if (msg == WM_NCDESTROY) {
            if (hwnd == g_hOverlayWnd) {
                g_hOverlayWnd = nullptr;
                g_overlayThreadId = 0;
            }
            return 0;
        }
        if (msg == WM_DESTROY) {
            return 0;
        }
    }
    FLIP3D_FENCE_END

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// -----------------------------------------------------------------------------
// Controller window (hotkey owner) and its guarded procedure
// -----------------------------------------------------------------------------

static LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    FLIP3D_FENCE_BEGIN("ControllerWndProc")
        if (msg == WM_DESTROY) {
            if (hwnd == g_hControllerWnd) {
                g_hControllerWnd = nullptr;
            }
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    FLIP3D_FENCE_ELSE
        try {
            switch (msg) {
            case WM_TIMER:
                if (wParam == kHotkeyReclaimTimerId) {
                    // Idempotent: re-registering an already-owned hotkey is a
                    // harmless no-op. If Windows silently took Win+Tab back
                    // (Task View / Game Bar / a session reinit), this wins it
                    // back within kHotkeyReclaimIntervalMs instead of leaving
                    // the mod stuck relying only on the racy LL-hook fallback.
                    UnregisterHotKey(g_hControllerWnd, kHotkeyWinTab);
                    UnregisterHotKey(g_hControllerWnd, kHotkeyCtrlWinTab);
                    g_winTabHotkeyOwned.store(
                        TryRegisterHotkey(kHotkeyWinTab, MOD_WIN | MOD_NOREPEAT, VK_TAB,
                                          L"RegisterHotKey(Win+Tab) [reclaim]"),
                        std::memory_order_relaxed);
                    g_ctrlWinTabHotkeyOwned.store(
                        TryRegisterHotkey(kHotkeyCtrlWinTab, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_TAB,
                                          L"RegisterHotKey(Ctrl+Win+Tab) [reclaim]"),
                        std::memory_order_relaxed);
                    return 0;
                }
                break;

            case WM_HOTKEY:
                switch (static_cast<int>(wParam)) {
                    case kHotkeyWinTab:
                        // Win+Tab: transient mode. Release Win = confirm.
                        if (g_isActive) {
                            if (g_hOverlayWnd && IsWindow(g_hOverlayWnd)) {
                                KillTimer(g_hOverlayWnd, kAutoCycleTimerId);
                            }
                            NavigateSelection(1);
                        } else {
                            g_persistentMode = false;
                            ActivateFlip3D(TriggerModifier::Win, 1);
                        }
                        return 0;

                    case kHotkeyCtrlWinTab:
                        // Ctrl+Win+Tab: persistent mode.
                        if (g_isActive) {
                            NavigateSelection(1);
                        } else {
                            g_persistentMode = true;
                            ActivateFlip3D(TriggerModifier::Win, 1);
                        }
                        return 0;

                    case kStickyHotkeyId:  // Ctrl+Alt+F12
                    case kToggleHotkeyId:  // Ctrl+G fallback toggle
                        if (g_isActive || (g_hOverlayWnd && IsWindow(g_hOverlayWnd))) {
                            DeactivateFlip3D(false);
                        } else {
                            g_persistentMode = true;
                            ActivateFlip3D(TriggerModifier::None, 0);
                        }
                        return 0;
                }
                break;

            case WM_FLIP3D_ACTIVATE:
                ActivateFlip3D(static_cast<TriggerModifier>(static_cast<int>(wParam)),
                               static_cast<int>(static_cast<INT_PTR>(lParam)));
                return 0;

            case WM_FLIP3D_NAVIGATE:
                if (g_isActive) {
                    NavigateSelection(static_cast<int>(static_cast<INT_PTR>(wParam)));
                }
                return 0;

            case WM_FLIP3D_CLOSE:
                DeactivateFlip3D(wParam != 0);
                return 0;

            case WM_DESTROY:
                if (hwnd == g_hControllerWnd) {
                    g_hControllerWnd = nullptr;
                }
                return 0;
        }
    } catch (...) {
        LogFailure(L"ControllerWndProc");
        if (msg == WM_DESTROY) {
            if (hwnd == g_hControllerWnd) {
                g_hControllerWnd = nullptr;
            }
            return 0;
        }
    }
    FLIP3D_FENCE_END

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static HWND CreateControllerWindow() {
    const wchar_t* className = L"Flip3DControllerWndClass";
    HINSTANCE hInstance = GetCurrentModuleHandle();
    if (!hInstance) {
        return nullptr;
    }

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ControllerWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;

    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        LogFailureCode(L"RegisterClassExW(controller)", GetLastError());
        return nullptr;
    }

    HWND hwnd = CreateWindowExW(0, className, L"", 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, hInstance, nullptr);
    if (!hwnd) {
        LogFailureCode(L"CreateWindowExW(controller)", GetLastError());
    }
    return hwnd;
}

// -----------------------------------------------------------------------------
// Low-level input hooks (guarded boundaries: must ALWAYS forward on failure)
// -----------------------------------------------------------------------------

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(g_keyboardHook.get(), nCode, wParam, lParam);
    }

    FLIP3D_FENCE_BEGIN("LowLevelKeyboardProc")
        return CallNextHookEx(g_keyboardHook.get(), nCode, wParam, lParam);
    FLIP3D_FENCE_ELSE
    try {
        const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        if (!kb) {
            return CallNextHookEx(g_keyboardHook.get(), nCode, wParam, lParam);
        }
        if (kb->flags & LLKHF_INJECTED) {
            return CallNextHookEx(g_keyboardHook.get(), nCode, wParam, lParam);
        }

        const DWORD vk = kb->vkCode;
        const bool keyDown = IsKeyDownMessage(wParam);
        const bool keyUp = IsKeyUpMessage(wParam);
        const bool winDown = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 ||
                             (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
        const bool altDown = (kb->flags & LLKHF_ALTDOWN) != 0 ||
                             (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ||
                             (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0 ||
                             (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
        const bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 ||
                              (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0 ||
                              (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
        const bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0 ||
                               (GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0 ||
                               (GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0;
        const bool active = g_hookSessionActive.load(std::memory_order_relaxed);

        if (active && keyDown && ctrlDown && shiftDown && vk == VK_ESCAPE) {
            // Emergency escape hatch: never block Task Manager.
            g_hookSessionActive.store(false, std::memory_order_relaxed);
            ClearPendingModifierReleaseSuppression();
            ResetWheelState();
            PostToController(WM_FLIP3D_CLOSE, 0, 0);
            return CallNextHookEx(g_keyboardHook.get(), nCode, wParam, lParam);
        }

        if (active && keyDown && (IsCtrlKey(vk) || IsShiftKey(vk))) {
            // Let Ctrl/Shift key-down through so system shortcuts keep working.
            return CallNextHookEx(g_keyboardHook.get(), nCode, wParam, lParam);
        }

        // Safe/manual shortcuts. Ctrl+Alt+F12 is the recommended one.
        // Ctrl+Win+F is consumed because Windows reserves it for
        // "Find Computers". Each combo below (except Ctrl+Win+F, which has
        // no RegisterHotKey counterpart) is skipped here entirely when
        // RegisterHotKey currently owns it: letting both fire for the same
        // physical keypress caused a double-activate (WM_HOTKEY activates
        // with Desktop selected, then this path immediately re-triggers and
        // navigates away from it before the user ever sees the desktop).
        const bool stickyOwnedElsewhere = g_stickyHotkeyOwned.load(std::memory_order_relaxed);
        const bool ctrlWinTabOwnedElsewhere = g_ctrlWinTabHotkeyOwned.load(std::memory_order_relaxed);
        const bool manualShortcut =
            (vk == VK_F12 && ctrlDown && altDown && !winDown && !stickyOwnedElsewhere) ||
            (vk == VK_TAB && ctrlDown && winDown && !ctrlWinTabOwnedElsewhere) ||
            (vk == 'F' && ctrlDown && winDown);
        if (manualShortcut && keyDown) {
            TriggerModifier manualModifier = winDown ? TriggerModifier::Win
                                                     : (altDown ? TriggerModifier::Alt
                                                                : TriggerModifier::None);
            if (active) {
                g_hookSessionActive.store(false, std::memory_order_relaxed);
                g_suppressReleaseModifier = manualModifier;
                g_suppressNextModifierRelease = TriggerModifierStillPhysicallyDown(manualModifier);
                ResetWheelState();
                PostToController(WM_FLIP3D_CLOSE, 0, 0);
            } else {
                if (!CanStartFlip3DSession()) {
                    // Don't let Windows open Find Computers if we can't start.
                    return 1;
                }

                g_triggerModifier = TriggerModifier::None;
                ClearPendingModifierReleaseSuppression();
                g_hookSessionActive.store(true, std::memory_order_relaxed);
                ResetWheelState();
                PostToController(WM_FLIP3D_ACTIVATE,
                                 static_cast<WPARAM>(TriggerModifier::None), 0);
            }
            return 1;
        }

        if (active && vk == VK_TAB && winDown) {
            // Overlay open via Ctrl+G / Ctrl+Alt+F12: swallow Win+Tab so Task
            // View can't pop up on top of it.
            return 1;
        }

        if (!kEnableLowLevelInputHooks) {
            // Safe mode: hook only backs the manual hotkey if RegisterHotKey
            // was stolen. Never intercept Win+Tab/Alt+Tab/arrows here.
            return CallNextHookEx(g_keyboardHook.get(), nCode, wParam, lParam);
        }

        // Only Win+Tab activates Flip3D via the LL hook (fallback path if
        // RegisterHotKey failed/was lost for this exact combo). Alt+Tab is
        // NEVER intercepted. !ctrlDown excludes Ctrl+Win+Tab, which is its
        // own combo (handled above / by its own WM_HOTKEY) - without this,
        // Ctrl+Win+Tab could fall through here and get double-activated in
        // the wrong (transient) mode whenever Win+Tab happens to be owned
        // while Ctrl+Win+Tab is not, or vice versa.
        const bool winTabOwnedElsewhere = g_winTabHotkeyOwned.load(std::memory_order_relaxed);
        if (vk == VK_TAB && keyDown && winDown && !altDown && !ctrlDown && !winTabOwnedElsewhere) {
            if (!active) {
                if (!CanStartFlip3DSession()) {
                    return CallNextHookEx(g_keyboardHook.get(), nCode, wParam, lParam);
                }

                TriggerModifier trigger = TriggerModifier::Win;
                int initialDelta = shiftDown ? -1 : 1;
                g_triggerModifier = trigger;
                ClearPendingModifierReleaseSuppression();
                g_hookSessionActive.store(true, std::memory_order_relaxed);
                ResetWheelState();
                PostToController(WM_FLIP3D_ACTIVATE,
                                 static_cast<WPARAM>(trigger),
                                 static_cast<LPARAM>(initialDelta));
                return 1;
            }

            PostToController(WM_FLIP3D_NAVIGATE,
                             static_cast<WPARAM>(static_cast<INT_PTR>(shiftDown ? -1 : 1)), 0);
            return 1;
        }

        if (active) {
            if (keyUp && IsTriggerModifierKey(g_triggerModifier, vk) && !PairStillDown(vk)) {
                if (g_persistentMode) {
                    // Persistent mode: do NOT close on key release.
                    return 1;
                }
                // Transient mode (Win+Tab): release confirms selection.
                g_hookSessionActive.store(false, std::memory_order_relaxed);
                ClearPendingModifierReleaseSuppression();
                ResetWheelState();
                PostToController(WM_FLIP3D_CLOSE, 1, 0);
                if (IsWinKey(vk) || IsAltKey(vk)) {
                    SwallowModifierRelease(vk);
                }
                return 1;
            }

            if (keyUp && IsAnyModifierKey(vk)) {
                if (IsWinKey(vk) || IsAltKey(vk)) {
                    SwallowModifierRelease(vk);
                    return 1;
                }
                return CallNextHookEx(g_keyboardHook.get(), nCode, wParam, lParam);
            }

            if (vk == VK_TAB) {
                return 1;
            }

            if (keyDown) {
                switch (vk) {
                    case VK_ESCAPE:
                        g_hookSessionActive.store(false, std::memory_order_relaxed);
                        PreparePendingModifierReleaseSuppression();
                        ResetWheelState();
                        PostToController(WM_FLIP3D_CLOSE, 0, 0);
                        return 1;

                    case VK_RETURN:
                    case VK_SPACE:
                        g_hookSessionActive.store(false, std::memory_order_relaxed);
                        PreparePendingModifierReleaseSuppression();
                        ResetWheelState();
                        PostToController(WM_FLIP3D_CLOSE, 1, 0);
                        return 1;

                    case VK_LEFT:
                    case VK_UP:
                        PostToController(WM_FLIP3D_NAVIGATE,
                                         static_cast<WPARAM>(static_cast<INT_PTR>(-1)), 0);
                        return 1;

                    case VK_RIGHT:
                    case VK_DOWN:
                        PostToController(WM_FLIP3D_NAVIGATE,
                                         static_cast<WPARAM>(static_cast<INT_PTR>(1)), 0);
                        return 1;
                }
            }

            // While the switcher is visible, don't leak keystrokes to apps or
            // to the native Alt+Tab UI.
            return 1;
        }

        if (g_suppressNextModifierRelease && keyUp &&
            IsTriggerModifierKey(g_suppressReleaseModifier, vk)) {
            ClearPendingModifierReleaseSuppression();
            if (IsWinKey(vk) || IsAltKey(vk)) {
                SwallowModifierRelease(vk);
            }
            return 1;
        }
    } catch (...) {
        // Boundary guard: on ANY failure, forward the event to Windows so the
        // input chain is never broken.
        LogFailure(L"LowLevelKeyboardProc");
    }
    FLIP3D_FENCE_END

    return CallNextHookEx(g_keyboardHook.get(), nCode, wParam, lParam);
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(g_mouseHook.get(), nCode, wParam, lParam);
    }

    FLIP3D_FENCE_BEGIN("LowLevelMouseProc")
        return CallNextHookEx(g_mouseHook.get(), nCode, wParam, lParam);
    FLIP3D_FENCE_ELSE

    if (!g_hookSessionActive.load(std::memory_order_relaxed)) {
        return CallNextHookEx(g_mouseHook.get(), nCode, wParam, lParam);
    }

    try {
        const auto* ms = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        if (!ms || (ms->flags & LLMHF_INJECTED)) {
            return CallNextHookEx(g_mouseHook.get(), nCode, wParam, lParam);
        }

        if (wParam == WM_MOUSEWHEEL) {
            short delta = static_cast<short>(HIWORD(ms->mouseData));
            if (delta == 0) {
                return 1;
            }

            int eventDir = delta > 0 ? +1 : -1;
            ULONGLONG now = GetTickCount64();
            if (g_lastWheelDir != 0 && eventDir != g_lastWheelDir &&
                (now - g_lastWheelPostMs) < kWheelDebounceMs) {
                return 1;
            }

            if (eventDir != g_lastWheelDir) {
                g_wheelAccum = 0;
            }

            g_wheelAccum += delta;
            while (g_wheelAccum >= WHEEL_DELTA) {
                PostToController(WM_FLIP3D_NAVIGATE,
                                 static_cast<WPARAM>(static_cast<INT_PTR>(-1)), 0);
                g_wheelAccum -= WHEEL_DELTA;
                g_lastWheelPostMs = now;
                g_lastWheelDir = +1;
            }
            while (g_wheelAccum <= -WHEEL_DELTA) {
                PostToController(WM_FLIP3D_NAVIGATE,
                                 static_cast<WPARAM>(static_cast<INT_PTR>(1)), 0);
                g_wheelAccum += WHEEL_DELTA;
                g_lastWheelPostMs = now;
                g_lastWheelDir = -1;
            }

            return 1;
        }
    } catch (...) {
        LogFailure(L"LowLevelMouseProc");
    }
    FLIP3D_FENCE_END

    return CallNextHookEx(g_mouseHook.get(), nCode, wParam, lParam);
}

// -----------------------------------------------------------------------------
// Hook thread: owns the controller window, hotkeys and LL hooks.
// A ScopeGuard guarantees full cleanup on every exit path, normal or
// exceptional, so nothing is ever leaked or left dangling.
// -----------------------------------------------------------------------------

static void UnregisterAllHotkeys() {
    g_stickyHotkeyOwned.store(false, std::memory_order_relaxed);
    g_winTabHotkeyOwned.store(false, std::memory_order_relaxed);
    g_ctrlWinTabHotkeyOwned.store(false, std::memory_order_relaxed);
    if (!g_hControllerWnd || !IsWindow(g_hControllerWnd)) {
        return;
    }
    UnregisterHotKey(g_hControllerWnd, kStickyHotkeyId);
    UnregisterHotKey(g_hControllerWnd, kToggleHotkeyId);
    UnregisterHotKey(g_hControllerWnd, kHotkeyWinTab);
    UnregisterHotKey(g_hControllerWnd, kHotkeyCtrlWinTab);
}

static bool TryRegisterHotkey(int id, UINT modifiers, UINT vk, PCWSTR label) {
    if (!RegisterHotKey(g_hControllerWnd, id, modifiers, vk)) {
        // Non-fatal: the LL hook still backs Win+Tab / manual shortcuts.
        LogFailureCode(label, GetLastError());
        return false;
    }
    LogDebugFmt(L"TryRegisterHotkey: %s registered (id=%d)", label, id);
    return true;
}

static DWORD HookThreadProcImpl(LPVOID) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

    MSG dummy;
    PeekMessageW(&dummy, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_hookThreadId = GetCurrentThreadId();
    LogDebugFmt(L"HookThread: started, tid=%lu", g_hookThreadId);

    // Single cleanup point for normal exit AND exception unwind.
    auto cleanup = MakeScopeGuard([] {
        LogDebug(L"HookThread: cleanup running");
        g_pendingActivateHwnd = nullptr;
        if (g_hOverlayWnd && IsWindow(g_hOverlayWnd)) {
            FinishExitAfterAnimation();
        }
        g_mouseHook.reset();
        g_keyboardHook.reset();
        UnregisterAllHotkeys();
        if (g_hControllerWnd && IsWindow(g_hControllerWnd)) {
            if (!DestroyWindow(g_hControllerWnd)) {
                LogFailureCode(L"DestroyWindow(controller)", GetLastError());
            }
        }
        g_hControllerWnd = nullptr;
        g_hookThreadId = 0;
        g_hookSessionActive.store(false, std::memory_order_relaxed);
    });

    g_hControllerWnd = CreateControllerWindow();
    if (g_hControllerWnd) {
        LogDebugFmt(L"HookThread: controller window created, hwnd=%p", g_hControllerWnd);
        g_stickyHotkeyOwned.store(
            TryRegisterHotkey(kStickyHotkeyId, MOD_CONTROL | MOD_ALT, VK_F12,
                              L"RegisterHotKey(Ctrl+Alt+F12)"),
            std::memory_order_relaxed);
        TryRegisterHotkey(kToggleHotkeyId, MOD_CONTROL, 'G',
                          L"RegisterHotKey(Ctrl+G)");
        // Win+Tab / Ctrl+Win+Tab via RegisterHotKey (more stable than LL
        // hooks). While registered, this process owns the hotkey and Task
        // View does not appear; UnregisterHotKey on unload restores it.
        g_winTabHotkeyOwned.store(
            TryRegisterHotkey(kHotkeyWinTab, MOD_WIN | MOD_NOREPEAT, VK_TAB,
                              L"RegisterHotKey(Win+Tab)"),
            std::memory_order_relaxed);
        g_ctrlWinTabHotkeyOwned.store(
            TryRegisterHotkey(kHotkeyCtrlWinTab, MOD_CONTROL | MOD_WIN | MOD_NOREPEAT, VK_TAB,
                              L"RegisterHotKey(Ctrl+Win+Tab)"),
            std::memory_order_relaxed);
        if (!SetTimer(g_hControllerWnd, kHotkeyReclaimTimerId, kHotkeyReclaimIntervalMs, nullptr)) {
            LogFailureCode(L"SetTimer(hotkeyReclaim)", GetLastError());
        }
    } else {
        LogFailure(L"HookThread: controller window");
    }

    if (kEnableLowLevelInputHooks || kEnableMinimalHotkeyHook) {
        HMODULE module = GetCurrentModuleHandle();
        g_keyboardHook.reset(SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, module, 0));
        if (!g_keyboardHook) {
            LogFailureCode(L"SetWindowsHookExW(keyboard)", GetLastError());
        } else {
            LogDebug(L"HookThread: keyboard LL hook installed");
        }
    }

    if (kEnableLowLevelInputHooks) {
        HMODULE module = GetCurrentModuleHandle();
        g_mouseHook.reset(SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, module, 0));
        if (!g_mouseHook) {
            LogFailureCode(L"SetWindowsHookExW(mouse)", GetLastError());
        } else {
            LogDebug(L"HookThread: mouse LL hook installed");
        }
    }

    bool ok = g_hControllerWnd != nullptr &&
              (!(kEnableLowLevelInputHooks || kEnableMinimalHotkeyHook) ||
               static_cast<bool>(g_keyboardHook));
    g_hookInstallOk.store(ok, std::memory_order_release);
    if (g_hookReadyEvent) {
        SetEvent(g_hookReadyEvent.get());
    }

    LogDebugFmt(L"HookThread: install %s, entering message loop", ok ? L"OK" : L"FAILED");

    if (!ok) {
        LogFailure(L"HookThread: install");
        return 1;  // ScopeGuard cleans everything up.
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    LogDebug(L"HookThread: message loop exited (WM_QUIT)");
    return 0;  // ScopeGuard cleans everything up.
}

// Guarded thread entry: an unhandled exception here must fail this thread
// alone, never take explorer.exe down. The ready event is ALWAYS signaled so
// Wh_ModInit can never hang waiting for us.
static DWORD WINAPI HookThreadProc(LPVOID param) {
    FLIP3D_FENCE_BEGIN("HookThreadProc")
        LogFailure(L"HookThreadProc (hardware fault)");
        g_mouseHook.reset();
        g_keyboardHook.reset();
        UnregisterAllHotkeys();
        if (g_hControllerWnd && IsWindow(g_hControllerWnd)) {
            DestroyWindow(g_hControllerWnd);
        }
        g_hControllerWnd = nullptr;
        g_hookThreadId = 0;
        g_hookSessionActive.store(false, std::memory_order_relaxed);
        g_hookInstallOk.store(false, std::memory_order_release);
        if (g_hookReadyEvent) {
            SetEvent(g_hookReadyEvent.get());
        }
        return 1;
    FLIP3D_FENCE_ELSE
    try {
        return HookThreadProcImpl(param);
    } catch (...) {
        LogFailure(L"HookThreadProc");
        try {
            g_mouseHook.reset();
            g_keyboardHook.reset();
            UnregisterAllHotkeys();
            if (g_hControllerWnd && IsWindow(g_hControllerWnd)) {
                if (!DestroyWindow(g_hControllerWnd)) {
                    LogFailureCode(L"DestroyWindow(controller)", GetLastError());
                }
            }
            g_hControllerWnd = nullptr;
            g_hookThreadId = 0;
            g_hookSessionActive.store(false, std::memory_order_relaxed);
        } catch (...) {
        }
        g_hookInstallOk.store(false, std::memory_order_release);
        if (g_hookReadyEvent) {
            SetEvent(g_hookReadyEvent.get());
        }
        return 1;
    }
    FLIP3D_FENCE_END
}

// -----------------------------------------------------------------------------
// Preload symbols before hook installation. Resolving addresses up front
// avoids lazy binding while hooks are being installed (a known source of
// load-time faults inside explorer.exe).
// -----------------------------------------------------------------------------

static bool PreloadSymbols() {
    // These DLLs are already mapped into explorer.exe (the mod links them
    // implicitly via @compilerOptions), so we only resolve their handles —
    // never LoadLibraryW here. Loading a DLL from the mod init path can run
    // under the loader lock during injection and is a classic cause of
    // explorer.exe deadlocks/restarts at startup.
    HMODULE dwmapi = GetModuleHandleW(L"dwmapi.dll");
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
    HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    HMODULE msimg32 = GetModuleHandleW(L"msimg32.dll");

    LogDebugFmt(L"PreloadSymbols: dwmapi=%p user32=%p kernelbase=%p gdi32=%p msimg32=%p",
                dwmapi, user32, kernelbase, gdi32, msimg32);

    if (!dwmapi || !user32 || !kernelbase) {
        LogFailure(L"PreloadSymbols: GetModuleHandle");
        return false;
    }

    // Touch the IAT entries we use so the first real call doesn't pay a
    // lazy-bind cost on a hot path. gdi32/msimg32 are optional here.
    GetProcAddress(dwmapi, "DwmRegisterThumbnail");
    GetProcAddress(dwmapi, "DwmUpdateThumbnailProperties");
    GetProcAddress(dwmapi, "DwmUnregisterThumbnail");
    GetProcAddress(dwmapi, "DwmQueryThumbnailSourceSize");
    GetProcAddress(dwmapi, "DwmGetWindowAttribute");
    GetProcAddress(dwmapi, "DwmIsCompositionEnabled");
    if (kernelbase) {
        GetProcAddress(kernelbase, "RegQueryValueExW");
    }
    GetProcAddress(user32, "RegisterHotKey");
    GetProcAddress(user32, "UnregisterHotKey");
    GetProcAddress(user32, "SetWindowsHookExW");
    if (gdi32) {
        GetProcAddress(gdi32, "GetDeviceCaps");
    }
    if (msimg32) {
        GetProcAddress(msimg32, "AlphaBlend");
    }

    LogDebug(L"PreloadSymbols: OK");
    return true;
}

// -----------------------------------------------------------------------------
// Windhawk entry points (guarded; init is non-blocking)
// -----------------------------------------------------------------------------

static BOOL Wh_ModInitImpl() {
    // Read user settings first (the perspective/flat choice drives both the
    // strip budget and the tilt computed later).
    LoadSettings();
    LogDebug(L"Wh_ModInit: settings loaded");

    // Install the anti-crash fault fence BEFORE anything else runs, so every
    // subsequent boundary (including the hook thread) is protected. The VEH
    // stays inert (returns EXCEPTION_CONTINUE_SEARCH) until a thread opens a
    // fence, so it cannot affect explorer or other mods.
    if (g_fenceTls == TLS_OUT_OF_INDEXES) {
        g_fenceTls = TlsAlloc();
        LogDebugFmt(L"Wh_ModInit: TlsAlloc -> %lu", g_fenceTls);
    }
    if (!g_veh) {
        g_veh = AddVectoredExceptionHandler(1, Flip3D_Veh);
        if (!g_veh) {
            LogFailureCode(L"Wh_ModInit: AddVectoredExceptionHandler", GetLastError());
        } else {
            LogDebug(L"Wh_ModInit: VEH installed");
        }
    }

    // Step 1: preload symbols BEFORE any hook installation (no DLL loading).
    if (!PreloadSymbols()) {
        LogFailure(L"Wh_ModInit: PreloadSymbols");
        return FALSE;
    }

    // Step 2: fire-and-forget. The hook thread does all the real work in the
    // background. We do NOT wait on it here: waiting would stall explorer's
    // thread during injection (and, in the worst case, deadlock against the
    // loader lock), which is the most common cause of an Explorer restart at
    // mod load. The mod simply becomes fully operational a few milliseconds
    // later, once the thread signals g_hookInstallOk.
    g_hookInstallOk.store(false, std::memory_order_relaxed);
    g_hookThread.reset(CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, nullptr));
    if (!g_hookThread) {
        LogFailureCode(L"Wh_ModInit: CreateThread", GetLastError());
        return FALSE;
    }

    LogDebugFmt(L"Wh_ModInit: hook thread created (tid pending), handle=%p", g_hookThread.get());
    LogNote(L"initialized (hook thread starting in background)");
    return TRUE;
}

static void Wh_ModUninitImpl() {
    LogDebug(L"Wh_ModUninit: begin");
    if (g_hookThread) {
        if (g_hookThreadId) {
            PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        }

        // Bounded wait: never block uninit indefinitely.
        DWORD waitResult = WaitForSingleObject(g_hookThread.get(), 5000);
        LogDebugFmt(L"Wh_ModUninit: hook thread wait result=%lu", waitResult);
        if (waitResult == WAIT_TIMEOUT) {
            LogFailure(L"Wh_ModUninit: hook thread did not exit within 5s");
        }
        g_hookThread.reset();
    } else {
        DeactivateFlip3D(false);
        g_keyboardHook.reset();
        g_mouseHook.reset();
    }

    g_hookThreadId = 0;
    g_hookSessionActive.store(false, std::memory_order_relaxed);

    // Tear down the fault fence last (after the hook thread has joined, so it
    // can no longer be inside a guarded region). The per-thread fence stack on
    // other threads (if any) is abandoned; this only runs at mod unload.
    if (g_veh) {
        RemoveVectoredExceptionHandler(g_veh);
        g_veh = nullptr;
    }
    if (g_fenceTls != TLS_OUT_OF_INDEXES) {
        FenceStack* fs = static_cast<FenceStack*>(TlsGetValue(g_fenceTls));
        if (fs) {
            free(fs);
        }
        TlsFree(g_fenceTls);
        g_fenceTls = TLS_OUT_OF_INDEXES;
    }
}

// Guarded mod entry points: a fault during load or unload fails the mod
// cleanly instead of crashing explorer.exe.
BOOL Wh_ModInit() {
    BOOL ok = FALSE;
    FLIP3D_FENCE_BEGIN("Wh_ModInit")
        LogFailure(L"Wh_ModInit (hardware fault)");
        return FALSE;
    FLIP3D_FENCE_ELSE
    ok = SafeExec<BOOL>(L"Wh_ModInit", FALSE, [] { return Wh_ModInitImpl(); });
    FLIP3D_FENCE_END
    return ok;
}

void Wh_ModUninit() {
    FLIP3D_FENCE_BEGIN("Wh_ModUninit")
        LogFailure(L"Wh_ModUninit (hardware fault)");
        return;
    FLIP3D_FENCE_ELSE
    SafeExec<bool>(L"Wh_ModUninit", false, [] {
        Wh_ModUninitImpl();
        return true;
    });
    LogNote(L"uninitialized");
    FLIP3D_FENCE_END
}

// Called by the Windhawk engine when the user changes a setting in the UI.
// Settings are reloaded immediately; if the switcher is open, the overlay
// thread rebuilds the deck so the new card style (realistic/flat) is applied
// live without having to close and reopen the switcher.
void Wh_ModSettingsChanged() {
    FLIP3D_FENCE_BEGIN("Wh_ModSettingsChanged")
        LogFailure(L"Wh_ModSettingsChanged (hardware fault)");
        return;
    FLIP3D_FENCE_ELSE
    LoadSettings();
    LogDebug(L"Wh_ModSettingsChanged: settings reloaded");
    if (g_isActive && g_hOverlayWnd && IsWindow(g_hOverlayWnd)) {
        LogDebug(L"Wh_ModSettingsChanged: switcher is open, requesting live rebuild");
        PostMessageW(g_hOverlayWnd, WM_FLIP3D_REBUILD, 0, 0);
    }
    FLIP3D_FENCE_END
}
