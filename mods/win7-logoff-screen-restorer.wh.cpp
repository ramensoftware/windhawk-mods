// ==WindhawkMod==
// @id             win7-logoff-screen-restorer
// @name           Windows Vista/7 Logoff Screen Restorer
// @description    This mod restores the classic Windows Vista/7 full-screen "programs still need to close" logoff and shutdown screen for Windows 10 and 11
// @version        1.0.0
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @include        StartMenuExperienceHost.exe
// @include        ShellExperienceHost.exe
// @include        ShellHost.exe
// @include        SearchHost.exe
// @include        Taskmgr.exe
// @architecture   x86-64
// @compilerOptions -luser32 -lgdi32 -lmsimg32 -lpsapi -lshell32 -ldwmapi -ladvapi32
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Windows Vista/7 Logoff Screen Restorer

This mod restores the classic Windows Vista/7 screen that shows up when logging off,
shut down, or restart while programs are still open (the one with the dimmed
desktop, the list of running programs, and the "Force log off"/"Cancel"
buttons)

The files on the system are not modified. The mod just shows this screen right
before Windows does the actual shutdown/restart/logoff. After clicking "Force log off"/"Force shutdown", Windows continues as normal.

## Screenshots

![Windows 7 Logoff](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win7logoff.PNG)

![Windows Vista Logoff](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/winvistalogoff.PNG)

## Features

- The mod shows every open program in a scrollable list, with icons just like Windows Vista/7 did.
- If nothing is open, the screen doesn't show up at all and the system goes straight to shutdown.
- The mod includes two skins to choose in the settings: **Windows 7** (blue Aero) or **Windows Vista** (red button) and the changes apply instantly.
- The mod is translated into 21 languages, matched automatically the system language (or pick one manually in settings).

The mod has been tested on Windows 10 21H2, Windows 11 24H2, and Windows 11 25H2.

---

## Try it without logging off

To try the mod without logging off, it is recommended to press `Ctrl+Alt+Shift+L` any time (while the mod is enabled) to preview the screen. You can change or disable this shortcut in settings. `Win+L` can't be used — Windows reserves it for locking the PC.

---

## Requirements

- Windows 10 or Windows 11 (64 bit)

---

## Safety Notes

- If the screen is ever left open too long, the mod closes it automatically after 60 seconds and lets the logoff/shutdown continue and it never forces programs to close on its own and never destroys unsaved work.
- The mod only shows a visual screen and it doesn't touch any system files or settings, and doesn't write anything to the registry.
- Turning the mod off (from Windhawk or from its own settings) removes it completely.

---

## Good to know

- This only catches shutdowns started the normal way (Start menu, Alt+F4, Task Manager, etc.). It won't appear for shutdowns triggered directly by Windows internals or by another program/service.
- If clicking Shut Down from the Start menu doesn't show the screen, make sure `StartMenuExperienceHost.exe` isn't excluded in Windhawk's process list.
- This screen is a visual step before shutdown, not a security screen — it doesn't replace the Windows lock/login screen.
- "Force log off" only force-closes programs that have actually stopped responding. A program that's still running but has unsaved work is left alone, exactly like in the original Windows 7 behavior, so unsaved work should never be lost.

---

## Credits
- Cips — testing on Windows 11 25H2

---

For any suggestions or problems it is recommended to contact the author of this modification.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable the custom screen
  $description: >-
    This setting turns the custom screen created by the mod on or off. This setting takes effect
    immediately, dismissing a screen already showing. This setting, when off,
    leaves log off, shut down and restart unchanged from stock Windows.
- skin: win7
  $name: Skin
  $description: >-
    This setting picks the screen's appearance. This setting does not change
    the information shown or how the screen behaves.
  $options:
  - win7: Windows 7 (blue glass buttons)
  - vista: Windows Vista (red shut down button)
- language: auto
  $name: Language
  $description: >-
    This setting picks the language of every string on the screen. This
    setting, on "Automatic", the mod follows the Windows user locale and falls back
    to English if unsupported. This setting never changes the wording's match
    to the action taking place (log off, shut down or restart).
  $options:
  - auto: Automatic (follow the Windows language)
  - en: English
  - it: Italiano
  - es: Español
  - fr: Français
  - de: Deutsch
  - pt: Português
  - nl: Nederlands
  - pl: Polski
  - cs: Čeština
  - sv: Svenska
  - da: Dansk
  - fi: Suomi
  - nb: Norsk bokmål
  - el: Ελληνικά (Greek)
  - tr: Türkçe
  - ru: Русский (Russian)
  - uk: Українська (Ukrainian)
  - ar: العربية (Arabic)
  - zh: 中文（简体）(Chinese, simplified)
  - ja: 日本語 (Japanese)
  - ko: 한국어 (Korean)
- previewHotkey: ctrlaltshiftl
  $name: Preview shortcut
  $description: >-
    This setting picks the shortcut that previews the screen without logging
    off or shutting anything down. This setting cannot offer Win+L, which
    Windows reserves for locking the workstation. This setting registers on
    explorer.exe, so a conflict with another program fails silently except
    for a note in the mod's log. This setting, set to "None", disables the
    preview.
  $options:
  - ctrlaltshiftl: Ctrl+Alt+Shift+L
  - ctrlshiftl: Ctrl+Shift+L
  - ctrlaltl: Ctrl+Alt+L
  - winshiftl: Win+Shift+L
  - winaltl: Win+Alt+L
  - ctrlaltshiftq: Ctrl+Alt+Shift+Q
  - none: None (disable the preview)
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <vector>
#include <cmath>
#include <string>

#include <unordered_map>
#include <atomic>
#include <mutex>
#include <new>
using ExitWindowsEx_t = BOOL (WINAPI*)(UINT, DWORD);
static ExitWindowsEx_t ExitWindowsEx_Original = nullptr;
// Second entry point. The Start menu power button and several shell surfaces
// go through InitiateShutdownW (advapi32) instead of ExitWindowsEx, so a mod
// that only hooks the latter never sees a shutdown started from the Start
// menu. Declared by hand because the SDK headers guard it behind a newer
// _WIN32_WINNT than Windhawk mods are compiled with.
using InitiateShutdownW_t = DWORD (WINAPI*)(LPWSTR, LPWSTR, DWORD, DWORD, DWORD);
static InitiateShutdownW_t InitiateShutdownW_Original = nullptr;

#ifndef SHUTDOWN_RESTART
#define SHUTDOWN_FORCE_OTHERS   0x00000001
#define SHUTDOWN_FORCE_SELF     0x00000002
#define SHUTDOWN_RESTART        0x00000004
#define SHUTDOWN_POWEROFF       0x00000008
#define SHUTDOWN_NOREBOOT       0x00000010
#define SHUTDOWN_GRACE_OVERRIDE 0x00000020
#define SHUTDOWN_INSTALL_UPDATES 0x00000040
#define SHUTDOWN_RESTARTAPPS    0x00000080
#define SHUTDOWN_HYBRID         0x00000200
#endif

static HWND g_dialog = nullptr;
static HBITMAP g_desktop = nullptr;
static HBITMAP g_backdrop = nullptr;   // cached desktop + veil, rebuilt on change
// thread_local: the nested pump in ShowWin7LogoffDialog lets a shell thread
// re-enter *Ex_Hook on itself while a screen is up, and independently a
// second thread/process can call in concurrently. A process-wide flag would
// let a second thread's hook see the first thread's "already inside" state
// (skipping its own screen) and would let whichever of two overlapping hooks
// finishes first clear the flag out from under the other.
static thread_local bool g_insideHook = false;
// Which of the three things Windows was asked to do, for the *currently
// displayed* screen. Only one screen is ever visible at a time (WM_APP_SHOW
// fast-paths a second concurrent request instead of opening another), so
// this being process-wide is fine for painting/wording purposes -- unlike
// the per-request outcome below, nothing here is read back by a hook thread.
enum ActionKind { kActionLogoff = 0, kActionShutdown = 1, kActionRestart = 2, kActionCount = 3 };
static ActionKind g_action = kActionLogoff;

// One of these is created (on the stack of the hook thread that issues the
// request) per shutdown/logoff attempt, and its address is handed to the UI
// thread as the WM_APP_SHOW lParam. The outcome fields are written only by
// the UI thread and only up until it signals `reply`; the requesting hook
// thread reads them only after its wait on `reply` returns. Keeping this
// per-request rather than in process-wide globals is what stops a second,
// concurrent request's "screen already up, proceed" fast path from landing
// between an in-progress screen's WM_DESTROY and the first request's own
// thread reading the result -- previously a shared g_proceed/g_force meant
// that race could turn a user's Cancel into a proceed.
struct ShutdownRequest {
    HANDLE reply = nullptr;
    ActionKind action = kActionLogoff;
    bool force = false;      // add EWX_FORCEIFHUNG to the real call
    // Whether the logoff/shutdown must continue once the screen closes. This
    // is deliberately separate from force: the screen can close by itself
    // because the last program went away, and in that case Windows should
    // carry on normally, without the "force" flag being added behind the
    // user's back.
    bool proceed = false;
};

// Requests that arrived while a screen was already up (the WM_APP_SHOW fast
// path below). They do not get their own screen; instead they are made to
// follow the outcome of the screen that IS currently visible, so a second
// caller can never start the real shutdown/logoff behind the first user's
// back while that user's Cancel is still possible. Signalled and cleared
// from WM_DESTROY, right after the visible screen's own request is.
static std::vector<ShutdownRequest*> g_waiters;

// ---------------------------------------------------------------------------
// Skins
//
// The two skins differ only in how the screen is painted: the program list,
// the filters, the counting, the scrolling and the wording are shared, so a
// skin can never change what the screen reports or what the buttons do. Every
// colour, alignment and ornament the painter needs lives in this one struct,
// which means adding a third skin later is a matter of adding another entry
// rather than of touching the drawing code.
// ---------------------------------------------------------------------------
struct ButtonPalette {
    COLORREF top, mid, bot;      // three-stop vertical gradient
    COLORREF topHot, midHot, botHot;
    COLORREF border;
    COLORREF text;
};

struct Skin {
    const wchar_t* id;
    COLORREF veil; BYTE veilAlpha;
    COLORREF titleText;
    bool     centreText;         // Vista centres the heading and the body
    COLORREF nameText;
    COLORREF noteText, blockingNoteText;
    COLORREF separator;
    BYTE     rowBandAlpha;       // 0 disables the alternating band
    COLORREF trackFill, trackBorder;
    COLORREF thumbTop, thumbMid, thumbBot, thumbBorder;
    ButtonPalette force, cancel;
    bool     powerGlyph;         // draw the round power symbol on the confirm button
    // Floods the inside of that symbol with one flat tone, taken from the
    // middle stop of the button's own gradient, instead of letting the
    // gradient run through it. Vista's symbol reads as a single solid red
    // disc rather than a lighter-at-the-top, darker-at-the-bottom one.
    bool     glyphFlatFill;
    int      cornerRadius;
    // Button metrics. Vista's buttons are noticeably tighter than the Windows 7
    // ones: measured off the reference screenshot they come out around 139x23
    // and 87x25 device pixels, i.e. a snug fit around the label rather than a
    // wide slab, so the padding and the floor width are per skin.
    int      buttonPadding;      // total horizontal padding around the label
    int      buttonMinWidth;
    // Cancel is sized separately from the action button. Both skins give it
    // the compact Vista proportions -- a snug 96 px floor rather than the wide
    // 160 px slab -- so the secondary button looks the same in either skin.
    // Only the width differs: the height stays the skin's own, so the two
    // buttons of a pair still line up.
    int      cancelPadding;
    int      cancelMinWidth;
    int      buttonHeight;
    int      buttonGap;
};

// Windows 7: cool blue glass on a blue-grey veil, everything left-aligned.
static const Skin kSkinWin7 = {
    L"win7",
    RGB(22,22,26), 200,
    RGB(242,242,242),
    false,
    RGB(228,231,235),
    RGB(200,170,120), RGB(245,185,80),
    RGB(80,95,108),
    10,
    RGB(40,52,64), RGB(70,86,100),
    RGB(150,200,240), RGB(70,140,205), RGB(30,80,150), RGB(150,185,215),
    // force
    {RGB(110,195,250), RGB(20,110,210), RGB(4,40,120),
     RGB(150,220,255), RGB(40,140,230), RGB(8,60,150),
     RGB(150,185,215), RGB(255,255,255)},
    // cancel
    {RGB(110,195,250), RGB(20,110,210), RGB(4,40,120),
     RGB(150,220,255), RGB(40,140,230), RGB(8,60,150),
     RGB(150,185,215), RGB(255,255,255)},
    false,
    false,
    6,
    34, 160,
    22, 96,
    31, 20
};

// Windows Vista, with the colours sampled off the reference screenshot rather
// than guessed:
//   veil      the desktop is dimmed almost to black, ~RGB(19,20,22)
//   title     near white, ~RGB(243,243,243), and centred
//   body      dimmer than the title, ~RGB(181,181,181)
//   notes     a warm tan, ~RGB(211,169,121) at its brightest -- Vista does not
//             use the saturated amber the Windows 7 skin does here
//   rules     flat grey, ~RGB(65,65,65)
// Cancel keeps the blue glass of the Windows 7 skin, so the red reads as the
// one button that carries consequences.
static const Skin kSkinVista = {
    L"vista",
    RGB(4,5,6), 205,
    RGB(243,243,243),
    true,
    RGB(226,226,226),
    RGB(168,136,100), RGB(206,166,118),
    RGB(70,70,70),
    0,
    RGB(44,44,44), RGB(82,82,82),
    RGB(190,190,190), RGB(134,134,134), RGB(88,88,88), RGB(184,184,184),
    // force: glossy red. Top anchor verified against a real Windows Vista
    // screenshot: a clean, letter-free band right at the button's top edge
    // measured consistently around (240-255, 215-240, 210-235), a near-white
    // warm pink -- much brighter than a saturated red. Mid/bot are left as
    // before: the rest of that button (only ~14px tall in the reference) is
    // covered almost entirely by the icon and label text, so there was no
    // clean fill pixel to verify them against.
    {RGB(250,228,222), RGB(196,38,30), RGB(122,10,8),
     RGB(255,240,236), RGB(220,58,46), RGB(150,16,12),
     RGB(150,60,52), RGB(255,255,255)},
    // cancel: the same blue glass as the Windows 7 skin
    {RGB(110,195,250), RGB(20,110,210), RGB(4,40,120),
     RGB(150,220,255), RGB(40,140,230), RGB(8,60,150),
     RGB(150,185,215), RGB(255,255,255)},
    true,
    true,
    3,
    22, 96,
    22, 96,
    27, 14
};

static const Skin* g_skin = &kSkinWin7;

// Master switch. When false the mod keeps its hooks installed but never shows
// the screen, so every shutdown path behaves exactly as it would without the
// mod. Kept as a plain bool read from the settings rather than by unhooking,
// because unhooking and re-hooking on every Save would be far riskier than
// one branch on a path that runs at most once per shutdown.
static bool g_enabled = true;

// Read on the UI thread, when the screen is about to be shown (and again on
// every settings Save), so switching the setting in Windhawk takes effect on
// the next screen without a reload. Empty means "follow the Windows user
// locale". Anything else is one of the locale prefixes in kTexts, validated
// when it is used rather than here, so an unknown value degrades to the
// automatic behaviour instead of failing.
static wchar_t g_forcedLocale[16] = L"";

static constexpr int kHotkeyId = 0x574C;

// Preview shortcut, resolved from the settings. 0 modifiers means the preview
// is switched off. Win+L is deliberately absent from the choices: the system
// consumes it for the secure lock screen before any application sees it, and
// RegisterHotKey rejects it, so offering it would only ever produce a
// shortcut that silently does nothing.
static UINT g_hotkeyMods = MOD_CONTROL | MOD_ALT | MOD_SHIFT;
static UINT g_hotkeyVk   = 'L';

static void LoadSkinSetting() {
    g_skin = &kSkinWin7;
    PCWSTR value = Wh_GetStringSetting(L"skin");
    if (value) {
        if (_wcsicmp(value, L"vista") == 0) g_skin = &kSkinVista;
        Wh_FreeStringSetting(value);
    }

    g_enabled = Wh_GetIntSetting(L"enabled") != 0;

    g_forcedLocale[0] = L'\0';
    PCWSTR lang = Wh_GetStringSetting(L"language");
    if (lang) {
        // "auto" and the empty string both mean: use the system locale.
        if (*lang && _wcsicmp(lang, L"auto") != 0) {
            wcsncpy_s(g_forcedLocale, lang, ARRAYSIZE(g_forcedLocale) - 1);
        }
        Wh_FreeStringSetting(lang);
    }

    // The shortcut lives on the UI thread's message-only window; a changed
    // setting is applied from Wh_ModSettingsChanged via WM_APP_APPLYSETTINGS,
    // so the new combination (or "None") takes effect without a mod reload.
    g_hotkeyMods = MOD_CONTROL | MOD_ALT | MOD_SHIFT;
    g_hotkeyVk   = 'L';
    PCWSTR hk = Wh_GetStringSetting(L"previewHotkey");
    if (hk) {
        struct { const wchar_t* id; UINT mods; UINT vk; } kCombos[] = {
            {L"ctrlaltshiftl", MOD_CONTROL | MOD_ALT | MOD_SHIFT, 'L'},
            {L"ctrlshiftl",    MOD_CONTROL | MOD_SHIFT,           'L'},
            {L"ctrlaltl",      MOD_CONTROL | MOD_ALT,             'L'},
            {L"winshiftl",     MOD_WIN | MOD_SHIFT,               'L'},
            {L"winaltl",       MOD_WIN | MOD_ALT,                 'L'},
            {L"ctrlaltshiftq", MOD_CONTROL | MOD_ALT | MOD_SHIFT, 'Q'},
            {L"none",          0,                                 0},
        };
        for (const auto& c : kCombos) {
            if (_wcsicmp(hk, c.id) == 0) { g_hotkeyMods = c.mods; g_hotkeyVk = c.vk; break; }
        }
        Wh_FreeStringSetting(hk);
    }
}

// ---------------------------------------------------------------------------
// Threading and teardown
//
// Every window the mod creates (the hotkey's message-only window and the
// full-screen dialog) belongs to one dedicated UI thread per injected
// process. That thread owns its window classes, its message loops and the
// RegisterHotKey call; nothing else ever calls CreateWindow/DestroyWindow/
// RegisterClass against them. This fixes the cross-thread teardown hazards a
// "dialog created on the caller's thread, hotkeys on another" design has:
//
//   * DestroyWindow cannot touch a window owned by another thread, so the
//     dialog is always closed with a posted message and torn down by the
//     thread that owns it.
//   * A window class is only ever unregistered from the thread that owns its
//     windows, after they are gone, and always on every exit path.
//   * The classes are registered with the mod's own module handle, never
//     with explorer.exe's, so a stale class can never point at an unmapped
//     WndProc.
//   * Wh_ModUninit asks the UI thread to wind up and joins it INFINITE: the
//     DLL image is never freed while any thread can still be executing mod
//     code or blocked in GetMessage with a return address inside it.
//
// The shutdown hooks run on the caller's (shell) thread; they hand the
// request to the UI thread and block until it signals the outcome.
// ---------------------------------------------------------------------------

// Module handle of the mod DLL itself. GetModuleHandleW(nullptr) would return
// the host executable (explorer.exe, ...), whose HINSTANCE is the wrong value
// for a class registered by the mod: on unload the class entry survives with
// lpfnWndProc pointing into freed memory.
static HMODULE GetModModuleHandle() {
    HMODULE hModule = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&GetModModuleHandle), &hModule);
    return hModule;
}

static const wchar_t kCtrlClassName[]  = L"WindhawkWin7LogoffRestorerCtrl";

// Private messages for the UI thread's message-only control window.
static constexpr UINT WM_APP_SHOW         = WM_APP + 1; // lParam: HANDLE reply event
static constexpr UINT WM_APP_APPLYSETTINGS = WM_APP + 2;
static constexpr UINT WM_APP_QUITUI       = WM_APP + 3;

// Written by the UI thread (including on cleanup), read by hook threads and
// by Wh_ModUninit -- made atomic so those cross-thread reads are never torn
// or reordered relative to the writes.
static std::atomic<HWND>  g_hotkeyWindow{nullptr};  // message-only control window (UI thread)
static std::atomic<DWORD> g_uiThreadId{0};
static std::atomic<HANDLE> g_uiThread{nullptr};   // written from StartUiThreadOnce/Wh_ModUninit, read from SignalUiThreadQuit
static bool   g_isExplorer = false;       // only explorer registers the preview hotkey

// Set while the control window exists; the UI thread clears it just before it
// exits. Uninit waits on it so the FreeLibrary that Windhawk does on return
// can never happen while the thread is still inside mod code.
static HANDLE g_uiThreadDone = nullptr;
// Fired once the control window has been created (or creation failed), so a
// shutdown that arrives immediately after init posts to a valid window.
static HANDLE g_uiReady = nullptr;
// Signalled while no hook is inside the mod's own code. Uninit waits on it
// before unhooking, for the same "never free the image under live code"
// reason as the UI-thread join.
static HANDLE g_hooksIdle = nullptr;

// Nest-safe: two overlapping hook invocations on the same thread (the nested
// pump in ShowWin7LogoffDialog lets ExitWindowsEx_Hook re-enter itself) must
// not let the inner frame's destructor signal g_hooksIdle while the outer
// frame is still executing mod code. A counter, rather than a plain
// reset/set pair, only signals once the last frame unwinds.
static std::atomic<int> g_hooksInFlight{0};
struct InFlightHook {
    InFlightHook()  { if (g_hooksInFlight.fetch_add(1, std::memory_order_acq_rel) == 0) ResetEvent(g_hooksIdle); }
    ~InFlightHook() { if (g_hooksInFlight.fetch_sub(1, std::memory_order_acq_rel) == 1) SetEvent(g_hooksIdle); }
};
static bool g_hoverForce = false;
static bool g_hoverCancel = false;
static ULONGLONG g_dialogStart = 0;
static ULONGLONG g_captureFailedAt = 0;

// ---------------------------------------------------------------------------
// DPI scaling
//
// Every metric the painter uses (fonts, rows, icons, paddings, the 800 px
// panel, the button sizes, ...) is written in 96-DPI reference pixels, i.e.
// the size the UI has at 100% scaling. The shell processes this mod runs
// inside (explorer.exe, StartMenuExperienceHost.exe, ...) are per-monitor
// DPI-aware, so on a mixed-DPI multi-monitor setup a single process-wide DPI
// is wrong for every monitor but the first. The DPI is therefore taken from
// the actual screen window (GetDpiForWindow, available on Windows 10 1607+,
// resolved dynamically so the mod still loads everywhere) and refreshed in
// WM_DPICHANGED when the window moves between monitors. GetProcessDpi() only
// remains as a pre-creation fallback; for a DPI-unaware host it returns 96, so
// the metrics stay unscaled and DWM's own stretch factor does the rest -- the
// two must never be combined.
// ---------------------------------------------------------------------------
static int g_dpi = 96;   // DPI of the monitor the screen is on; 96 == 100%

typedef UINT (WINAPI *GetDpiForWindow_t)(HWND);
static int GetProcessDpi();

static int GetWindowDpi(HWND hwnd) {
    int dpi = 0;
    static const GetDpiForWindow_t pGetDpiForWindow = []() -> GetDpiForWindow_t {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        return user32 ? reinterpret_cast<GetDpiForWindow_t>(
                            GetProcAddress(user32, "GetDpiForWindow"))
                      : nullptr;
    }();
    if (pGetDpiForWindow) dpi = (int)pGetDpiForWindow(hwnd);
    if (dpi < 96 || dpi > 480) dpi = GetProcessDpi();
    return dpi;
}

static int GetProcessDpi() {
    int dpi = 96;
    if (HDC dc = GetDC(nullptr)) {
        int d = GetDeviceCaps(dc, LOGPIXELSX);
        if (d >= 96 && d <= 480) dpi = d;
        ReleaseDC(nullptr, dc);
    }
    return dpi;
}

// Scales a 96-DPI reference value to the current DPI, rounding to the
// nearest whole pixel so 1 px at 125% still rounds to something sane.
static int DpiScale(int v) { return (v * g_dpi + 48) / 96; }

// ---------------------------------------------------------------------------
// Target monitor
//
// The full-screen dialog spans the whole virtual desktop, so centring the panel
// in "the" client rect would put it halfway across the bezel on a multi-monitor
// setup, scale it with a DPI that is meaningful for no particular monitor, and
// decide layout against the combined height of every monitor. The panel is
// therefore centred on (and metric-scaled to) a single target monitor: the one
// holding the foreground window, with the cursor and then the primary monitor
// as fallbacks.
// ---------------------------------------------------------------------------
static HMONITOR g_monitor = nullptr;
static RECT     g_monitorClient = {0, 0, 0, 0};   // monitor rect in client coords

typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
static int GetMonitorDpi(HMONITOR mon) {
    static const GetDpiForMonitor_t p = []() -> GetDpiForMonitor_t {
        HMODULE shcore = GetModuleHandleW(L"shcore.dll");
        if (!shcore) shcore = LoadLibraryW(L"shcore.dll");
        return shcore ? reinterpret_cast<GetDpiForMonitor_t>(
                            GetProcAddress(shcore, "GetDpiForMonitor"))
                      : nullptr;
    }();
    if (p && mon) {
        UINT dx = 0, dy = 0;
        if (SUCCEEDED(p(mon, 0, &dx, &dy)))     // MDT_EFFECTIVE_DPI == 0
            return (int)dx;
    }
    return 0;
}

static HMONITOR PickTargetMonitor() {
    HMONITOR mon = nullptr;
    if (HWND fg = GetForegroundWindow())
        mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    if (!mon) {
        POINT pt{};
        if (GetCursorPos(&pt)) mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    }
    if (!mon) mon = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    return mon;
}

static void SetTargetMonitor(HMONITOR mon) {
    g_monitor = mon;
    g_monitorClient = RECT{0, 0, 0, 0};
    if (!mon) { g_dpi = GetProcessDpi(); return; }
    MONITORINFO mi{}; mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(mon, &mi)) {
        const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
        const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
        g_monitorClient = RECT{mi.rcMonitor.left - vx,
                               mi.rcMonitor.top - vy,
                               mi.rcMonitor.right - vx,
                               mi.rcMonitor.bottom - vy};
    }
    int dpi = GetMonitorDpi(mon);
    if (dpi < 96 || dpi > 480) dpi = GetWindowDpi(g_dialog ? g_dialog : nullptr);
    if (dpi < 96 || dpi > 480) dpi = GetProcessDpi();
    g_dpi = dpi;
}
// Windows 7 never capped this list at a fixed number of entries: the heading
// reported the true total and the list showed as many programs as the screen
// resolution allowed, adding a scrollbar (and smaller icons) for the rest.
// The constants below are therefore only sanity bounds, not a feature limit:
//   kMaxListedPrograms - upper bound on enumerated entries, so a runaway
//                        enumeration can never allocate unbounded icons.
//                        The heading still reports the real total.
//   kMaxVisibleRows    - rows drawn at once with normal metrics (Windows 7
//                        typically showed 4-6); the rest is scrollable.
//   kMaxVisibleRowsCompact - rows drawn once the list switches to the
//                        compact metrics used when there are many programs.
static constexpr size_t kMaxListedPrograms = 64;
static constexpr int kMaxVisibleRows = 6;
static constexpr int kMaxVisibleRowsCompact = 8;
static int g_listScroll = 0;          // index of the first row currently drawn
static size_t g_totalPrograms = 0;    // real total, independent of the list cap
static bool g_draggingThumb = false;  // scrollbar thumb is being dragged
static int g_dragOffset = 0;          // grab point inside the thumb

// Result of RefreshOpenPrograms(): nothing changed, only the text/icons
// changed (repaint the list only), or the entry count changed (the panel is
// re-laid out, so everything must be repainted).
enum { kListUnchanged = 0, kListContentChanged = 1, kListLayoutChanged = 2 };

static const wchar_t kClassName[] = L"WindhawkWin7LogoffRestorer";

// Program icons are keyed by PID and survive the one-second list refreshes.
// Previously every refresh destroyed every icon and re-ran WM_GETICON /
// ExtractIconExW / SHGetFileInfoW for every program every second; the latter
// two can hit the disk and the shell icon cache and block for a long time on
// a slow or network path. The map keeps the CloneIcon'd 32x32 bitmap owned by
// the mod (window icons belong to the window's process and can vanish), and
// entries for PIDs no longer present are pruned each refresh.
static std::unordered_map<DWORD, HICON> g_iconCache;

static void ClearIconCache() {
    for (auto& kv : g_iconCache) if (kv.second) DestroyIcon(kv.second);
    g_iconCache.clear();
}


class BitmapGuard {
public:
    explicit BitmapGuard(HBITMAP value = nullptr) : value_(value) {}
    ~BitmapGuard() { if (value_) DeleteObject(value_); }
    BitmapGuard(const BitmapGuard&) = delete;
    BitmapGuard& operator=(const BitmapGuard&) = delete;
    HBITMAP get() const { return value_; }
    HBITMAP release() { HBITMAP v = value_; value_ = nullptr; return v; }
private:
    HBITMAP value_;
};

class DcGuard {
public:
    explicit DcGuard(HDC value = nullptr) : value_(value) {}
    ~DcGuard() { if (value_) DeleteDC(value_); }
    DcGuard(const DcGuard&) = delete;
    DcGuard& operator=(const DcGuard&) = delete;
    HDC get() const { return value_; }
private:
    HDC value_;
};

static void FreeDesktopBitmap() {
    if (g_desktop) {
        DeleteObject(g_desktop);
        g_desktop = nullptr;
    }
}

// Grabs the whole virtual desktop (all monitors) so the backdrop covers the
// same area the full-screen window spans.
static HBITMAP CaptureDesktop() {
    HDC src = GetDC(nullptr);
    if (!src) return nullptr;
    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (w <= 0 || h <= 0) { ReleaseDC(nullptr, src); return nullptr; }
    DcGuard mem(CreateCompatibleDC(src));
    BitmapGuard bmp(CreateCompatibleBitmap(src, w, h));
    if (mem.get() && bmp.get()) {
        HGDIOBJ old = SelectObject(mem.get(), bmp.get());
        BitBlt(mem.get(), 0, 0, w, h, src, x, y, SRCCOPY);
        SelectObject(mem.get(), old);
        HBITMAP result = bmp.release();
        ReleaseDC(nullptr, src);
        return result;
    }
    ReleaseDC(nullptr, src);
    return nullptr;
}

static void FreeBackdrop() {
    if (g_backdrop) { DeleteObject(g_backdrop); g_backdrop = nullptr; }
}

// Composits the captured desktop and the dark veil into one cached bitmap the
// size of the virtual desktop. It changes only on capture, monitor, DPI or
// skin changes, so WM_PAINT can blit a region of it instead of re-copying the
// desktop and alpha-blending a full-screen veil on every repaint.
static void RebuildBackdrop() {
    if (g_backdrop) { DeleteObject(g_backdrop); g_backdrop = nullptr; }
    const int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (w <= 0 || h <= 0) return;
    HDC screen = GetDC(nullptr);
    DcGuard mem(CreateCompatibleDC(screen));
    BitmapGuard bmp(screen ? CreateCompatibleBitmap(screen, w, h) : nullptr);
    if (screen && mem.get() && bmp.get()) {
        HGDIOBJ old = SelectObject(mem.get(), bmp.get());
        if (g_desktop) {
            HDC src = CreateCompatibleDC(mem.get());
            if (src) {
                HGDIOBJ o = SelectObject(src, g_desktop);
                BitBlt(mem.get(), 0, 0, w, h, src, 0, 0, SRCCOPY);
                SelectObject(src, o); DeleteDC(src);
            }
        } else {
            HBRUSH fallback = CreateSolidBrush(RGB(0x1a, 0x2a, 0x3a));
            RECT r{0, 0, w, h};
            if (fallback) { FillRect(mem.get(), &r, fallback); DeleteObject(fallback); }
        }
        // Cast the veil over the captured desktop.
        HDC veilDc = CreateCompatibleDC(mem.get());
        HBITMAP veilBmp = CreateCompatibleBitmap(mem.get(), w, h);
        if (veilDc && veilBmp) {
            HGDIOBJ oldVeil = SelectObject(veilDc, veilBmp);
            HBRUSH veilBrush = CreateSolidBrush(g_skin->veil);
            RECT r{0, 0, w, h};
            if (veilBrush) { FillRect(veilDc, &r, veilBrush); DeleteObject(veilBrush); }
            BLENDFUNCTION bf{AC_SRC_OVER, 0, g_skin->veilAlpha, 0};
            AlphaBlend(mem.get(), 0, 0, w, h, veilDc, 0, 0, w, h, bf);
            SelectObject(veilDc, oldVeil);
            DeleteObject(veilBmp);
        }
        if (veilDc) DeleteDC(veilDc);
        SelectObject(mem.get(), old);
        g_backdrop = bmp.release();
    }
    if (screen) ReleaseDC(nullptr, screen);
}
struct UiText {
    const wchar_t* locale;
    const wchar_t* cancel;
    const wchar_t* stillOne;
    const wchar_t* stillMany;
    const wchar_t* notResponding;
    const wchar_t* body[kActionCount];     // indexed by ActionKind
    const wchar_t* force[kActionCount];    // label of the confirm button
    const wchar_t* blocked[kActionCount];  // per-program explanatory line
};
static const UiText kTexts[] = {
 {L"it", L"Annulla",
  L"programma deve ancora chiudersi:",
  L"programmi devono ancora chiudersi:",
  L"Questo programma non risponde.",
  {L"Per chiudere il programma che impedisce la disconnessione, fare clic su Annulla e chiudere il programma.",
   L"Per chiudere il programma che impedisce l'arresto del sistema, fare clic su Annulla e chiudere il programma.",
   L"Per chiudere il programma che impedisce il riavvio del sistema, fare clic su Annulla e chiudere il programma."},
  {L"Forza disconnessione", L"Forza arresto", L"Forza riavvio"},
  {L"Questo programma impedisce la disconnessione di Windows.",
   L"Questo programma impedisce l'arresto di Windows.",
   L"Questo programma impedisce il riavvio di Windows."}},
 {L"es", L"Cancelar",
  L"programa aún debe cerrarse:",
  L"programas aún deben cerrarse:",
  L"Este programa no responde.",
  {L"Para cerrar el programa que impide cerrar la sesión, haga clic en Cancelar y cierre el programa.",
   L"Para cerrar el programa que impide apagar el equipo, haga clic en Cancelar y cierre el programa.",
   L"Para cerrar el programa que impide reiniciar el equipo, haga clic en Cancelar y cierre el programa."},
  {L"Forzar cierre de sesión", L"Forzar apagado", L"Forzar reinicio"},
  {L"Este programa impide que Windows cierre la sesión.",
   L"Este programa impide que Windows se apague.",
   L"Este programa impide que Windows se reinicie."}},
 {L"fr", L"Annuler",
  L"programme doit encore être fermé:",
  L"programmes doivent encore être fermés:",
  L"Ce programme ne répond pas.",
  {L"Pour fermer le programme qui empêche la fermeture de session, cliquez sur Annuler, puis fermez le programme.",
   L"Pour fermer le programme qui empêche l'arrêt de Windows, cliquez sur Annuler, puis fermez le programme.",
   L"Pour fermer le programme qui empêche le redémarrage de Windows, cliquez sur Annuler, puis fermez le programme."},
  {L"Forcer la fermeture", L"Forcer l'arrêt", L"Forcer le redémarrage"},
  {L"Ce programme empêche la fermeture de session de Windows.",
   L"Ce programme empêche l'arrêt de Windows.",
   L"Ce programme empêche le redémarrage de Windows."}},
 {L"de", L"Abbrechen",
  L"Programm muss noch geschlossen werden:",
  L"Programme müssen noch geschlossen werden:",
  L"Dieses Programm reagiert nicht.",
  {L"Klicken Sie auf Abbrechen und schließen Sie das Programm, das die Abmeldung verhindert.",
   L"Klicken Sie auf Abbrechen und schließen Sie das Programm, das das Herunterfahren verhindert.",
   L"Klicken Sie auf Abbrechen und schließen Sie das Programm, das den Neustart verhindert."},
  {L"Abmeldung erzwingen", L"Herunterfahren erzwingen", L"Neustart erzwingen"},
  {L"Dieses Programm verhindert die Abmeldung von Windows.",
   L"Dieses Programm verhindert das Herunterfahren von Windows.",
   L"Dieses Programm verhindert den Neustart von Windows."}},
 {L"tr", L"İptal",
  L"program kapatılmayı bekliyor:",
  L"program kapatılmayı bekliyor:",
  L"Bu program yanıt vermiyor.",
  {L"Oturumun kapatılmasını engelleyen programı kapatmak için İptal'e tıklayın ve programı kapatın.",
   L"Bilgisayarın kapatılmasını engelleyen programı kapatmak için İptal'e tıklayın ve programı kapatın.",
   L"Bilgisayarın yeniden başlatılmasını engelleyen programı kapatmak için İptal'e tıklayın ve programı kapatın."},
  {L"Oturumu kapatmaya zorla", L"Kapatmaya zorla", L"Yeniden başlatmaya zorla"},
  {L"Bu program Windows'un oturumu kapatmasını engelliyor.",
   L"Bu program Windows'un kapatılmasını engelliyor.",
   L"Bu program Windows'un yeniden başlatılmasını engelliyor."}},
 {L"zh", L"取消",
  L"个程序仍需要关闭:",
  L"个程序仍需要关闭:",
  L"此程序未响应。",
  {L"若要关闭阻止注销的程序，请单击“取消”，然后关闭该程序。",
   L"若要关闭阻止关机的程序，请单击“取消”，然后关闭该程序。",
   L"若要关闭阻止重启的程序，请单击“取消”，然后关闭该程序。"},
  {L"强制注销", L"强制关机", L"强制重启"},
  {L"此程序正在阻止 Windows 注销。",
   L"此程序正在阻止 Windows 关机。",
   L"此程序正在阻止 Windows 重启。"}},
 {L"pt", L"Cancelar",
  L"programa ainda precisa ser fechado:",
  L"programas ainda precisam ser fechados:",
  L"Este programa não está a responder.",
  {L"Para fechar o programa que impede a sessão de terminar, clique em Cancelar e feche o programa.",
   L"Para fechar o programa que impede o encerramento do Windows, clique em Cancelar e feche o programa.",
   L"Para fechar o programa que impede o reinício do Windows, clique em Cancelar e feche o programa."},
  {L"Forçar saída", L"Forçar encerramento", L"Forçar reinício"},
  {L"Este programa está a impedir que o Windows termine a sessão.",
   L"Este programa está a impedir que o Windows se desligue.",
   L"Este programa está a impedir que o Windows reinicie."}},
 {L"nl", L"Annuleren",
  L"programma moet nog worden gesloten:",
  L"programma's moeten nog worden gesloten:",
  L"Dit programma reageert niet.",
  {L"Klik op Annuleren en sluit het programma dat het afmelden verhindert.",
   L"Klik op Annuleren en sluit het programma dat het afsluiten verhindert.",
   L"Klik op Annuleren en sluit het programma dat het opnieuw opstarten verhindert."},
  {L"Afmelden forceren", L"Afsluiten forceren", L"Opnieuw opstarten forceren"},
  {L"Dit programma voorkomt dat Windows wordt afgemeld.",
   L"Dit programma voorkomt dat Windows wordt afgesloten.",
   L"Dit programma voorkomt dat Windows opnieuw wordt opgestart."}},
 {L"pl", L"Anuluj",
  L"program nadal wymaga zamknięcia:",
  L"programy nadal wymagają zamknięcia:",
  L"Ten program nie odpowiada.",
  {L"Aby zamknąć program blokujący wylogowanie, kliknij Anuluj i zamknij program.",
   L"Aby zamknąć program blokujący zamykanie systemu, kliknij Anuluj i zamknij program.",
   L"Aby zamknąć program blokujący ponowne uruchamianie, kliknij Anuluj i zamknij program."},
  {L"Wymuś wylogowanie", L"Wymuś zamknięcie", L"Wymuś ponowne uruchomienie"},
  {L"Ten program uniemożliwia wylogowanie z systemu Windows.",
   L"Ten program uniemożliwia zamknięcie systemu Windows.",
   L"Ten program uniemożliwia ponowne uruchomienie systemu Windows."}},
 {L"ru", L"Отмена",
  L"программа всё ещё должна закрыться:",
  L"программы всё ещё должны закрыться:",
  L"Эта программа не отвечает.",
  {L"Чтобы закрыть программу, препятствующую выходу, нажмите «Отмена» и закройте программу.",
   L"Чтобы закрыть программу, препятствующую завершению работы, нажмите «Отмена» и закройте программу.",
   L"Чтобы закрыть программу, препятствующую перезагрузке, нажмите «Отмена» и закройте программу."},
  {L"Принудительный выход", L"Завершить работу принудительно", L"Перезагрузить принудительно"},
  {L"Эта программа мешает выходу Windows из системы.",
   L"Эта программа мешает завершению работы Windows.",
   L"Эта программа мешает перезагрузке Windows."}},
 {L"uk", L"Скасувати",
  L"програма ще має закритися:",
  L"програми ще мають закритися:",
  L"Ця програма не відповідає.",
  {L"Натисніть «Скасувати» та закрийте програму, яка заважає вийти.",
   L"Натисніть «Скасувати» та закрийте програму, яка заважає завершити роботу.",
   L"Натисніть «Скасувати» та закрийте програму, яка заважає перезавантажити комп'ютер."},
  {L"Примусово вийти", L"Примусово завершити роботу", L"Примусово перезавантажити"},
  {L"Ця програма заважає виходу Windows із системи.",
   L"Ця програма заважає завершенню роботи Windows.",
   L"Ця програма заважає перезавантаженню Windows."}},
 {L"cs", L"Zrušit",
  L"program je stále nutné zavřít:",
  L"programy je stále nutné zavřít:",
  L"Tento program neodpovídá.",
  {L"Klikněte na Zrušit a zavřete program, který brání odhlášení.",
   L"Klikněte na Zrušit a zavřete program, který brání vypnutí.",
   L"Klikněte na Zrušit a zavřete program, který brání restartování."},
  {L"Vynutit odhlášení", L"Vynutit vypnutí", L"Vynutit restart"},
  {L"Tento program brání odhlášení systému Windows.",
   L"Tento program brání vypnutí systému Windows.",
   L"Tento program brání restartování systému Windows."}},
 {L"sv", L"Avbryt",
  L"program måste fortfarande stängas:",
  L"program måste fortfarande stängas:",
  L"Det här programmet svarar inte.",
  {L"Klicka på Avbryt och stäng programmet som hindrar utloggningen.",
   L"Klicka på Avbryt och stäng programmet som hindrar avstängningen.",
   L"Klicka på Avbryt och stäng programmet som hindrar omstarten."},
  {L"Tvinga utloggning", L"Tvinga avstängning", L"Tvinga omstart"},
  {L"Det här programmet förhindrar att Windows loggar ut.",
   L"Det här programmet förhindrar att Windows stängs av.",
   L"Det här programmet förhindrar att Windows startas om."}},
 {L"da", L"Annuller",
  L"program skal stadig lukkes:",
  L"programmer skal stadig lukkes:",
  L"Dette program svarer ikke.",
  {L"Klik på Annuller, og luk programmet, der forhindrer, at du logger af.",
   L"Klik på Annuller, og luk programmet, der forhindrer nedlukningen.",
   L"Klik på Annuller, og luk programmet, der forhindrer genstarten."},
  {L"Gennemtving log af", L"Gennemtving nedlukning", L"Gennemtving genstart"},
  {L"Dette program forhindrer, at Windows logger af.",
   L"Dette program forhindrer, at Windows lukkes ned.",
   L"Dette program forhindrer, at Windows genstarter."}},
 {L"fi", L"Peruuta",
  L"ohjelma on vielä suljettava:",
  L"ohjelmaa on vielä suljettava:",
  L"Tämä ohjelma ei vastaa.",
  {L"Napsauta Peruuta ja sulje uloskirjautumisen estävä ohjelma.",
   L"Napsauta Peruuta ja sulje sammuttamisen estävä ohjelma.",
   L"Napsauta Peruuta ja sulje uudelleenkäynnistyksen estävä ohjelma."},
  {L"Pakota uloskirjautuminen", L"Pakota sammutus", L"Pakota uudelleenkäynnistys"},
  {L"Tämä ohjelma estää Windowsin uloskirjautumisen.",
   L"Tämä ohjelma estää Windowsin sammuttamisen.",
   L"Tämä ohjelma estää Windowsin uudelleenkäynnistyksen."}},
 {L"nb", L"Avbryt",
  L"program må fortsatt lukkes:",
  L"programmer må fortsatt lukkes:",
  L"Dette programmet svarer ikke.",
  {L"Klikk Avbryt og lukk programmet som hindrer avlogging.",
   L"Klikk Avbryt og lukk programmet som hindrer avslutning.",
   L"Klikk Avbryt og lukk programmet som hindrer omstart."},
  {L"Tving avlogging", L"Tving avslutning", L"Tving omstart"},
  {L"Dette programmet hindrer Windows i å logge av.",
   L"Dette programmet hindrer Windows i å slå seg av.",
   L"Dette programmet hindrer Windows i å starte på nytt."}},
 {L"el", L"Ακύρωση",
  L"πρόγραμμα πρέπει ακόμη να κλείσει:",
  L"προγράμματα πρέπει ακόμη να κλείσουν:",
  L"Αυτό το πρόγραμμα δεν ανταποκρίνεται.",
  {L"Κάντε κλικ στην Ακύρωση και κλείστε το πρόγραμμα που εμποδίζει την αποσύνδεση.",
   L"Κάντε κλικ στην Ακύρωση και κλείστε το πρόγραμμα που εμποδίζει τον τερματισμό λειτουργίας.",
   L"Κάντε κλικ στην Ακύρωση και κλείστε το πρόγραμμα που εμποδίζει την επανεκκίνηση."},
  {L"Εξαναγκασμός αποσύνδεσης", L"Εξαναγκασμός τερματισμού", L"Εξαναγκασμός επανεκκίνησης"},
  {L"Αυτό το πρόγραμμα εμποδίζει την αποσύνδεση των Windows.",
   L"Αυτό το πρόγραμμα εμποδίζει τον τερματισμό λειτουργίας των Windows.",
   L"Αυτό το πρόγραμμα εμποδίζει την επανεκκίνηση των Windows."}},
 {L"ko", L"취소",
  L"개의 프로그램을 닫아야 합니다:",
  L"개의 프로그램을 닫아야 합니다:",
  L"이 프로그램이 응답하지 않습니다.",
  {L"로그오프를 방해하는 프로그램을 닫으려면 취소를 클릭한 다음 프로그램을 닫으세요.",
   L"시스템 종료를 방해하는 프로그램을 닫으려면 취소를 클릭한 다음 프로그램을 닫으세요.",
   L"다시 시작을 방해하는 프로그램을 닫으려면 취소를 클릭한 다음 프로그램을 닫으세요."},
  {L"강제 로그오프", L"강제 종료", L"강제 다시 시작"},
  {L"이 프로그램이 Windows 로그오프를 막고 있습니다.",
   L"이 프로그램이 Windows 종료를 막고 있습니다.",
   L"이 프로그램이 Windows 다시 시작을 막고 있습니다."}},
 {L"ja", L"キャンセル",
  L"個のプログラムを閉じる必要があります:",
  L"個のプログラムを閉じる必要があります:",
  L"このプログラムは応答していません。",
  {L"ログオフを妨げているプログラムを閉じるには、キャンセルをクリックしてからプログラムを閉じてください。",
   L"シャットダウンを妨げているプログラムを閉じるには、キャンセルをクリックしてからプログラムを閉じてください。",
   L"再起動を妨げているプログラムを閉じるには、キャンセルをクリックしてからプログラムを閉じてください。"},
  {L"強制ログオフ", L"強制シャットダウン", L"強制再起動"},
  {L"このプログラムが Windows のログオフを妨げています。",
   L"このプログラムが Windows のシャットダウンを妨げています。",
   L"このプログラムが Windows の再起動を妨げています。"}},
 {L"ar", L"إلغاء",
  L"برنامج لا يزال بحاجة إلى الإغلاق:",
  L"برامج لا تزال بحاجة إلى الإغلاق:",
  L"هذا البرنامج لا يستجيب.",
  {L"لإغلاق البرنامج الذي يمنع تسجيل الخروج، انقر فوق إلغاء ثم أغلق البرنامج.",
   L"لإغلاق البرنامج الذي يمنع إيقاف التشغيل، انقر فوق إلغاء ثم أغلق البرنامج.",
   L"لإغلاق البرنامج الذي يمنع إعادة التشغيل، انقر فوق إلغاء ثم أغلق البرنامج."},
  {L"فرض تسجيل الخروج", L"فرض إيقاف التشغيل", L"فرض إعادة التشغيل"},
  {L"يمنع هذا البرنامج تسجيل خروج Windows.",
   L"يمنع هذا البرنامج إيقاف تشغيل Windows.",
   L"يمنع هذا البرنامج إعادة تشغيل Windows."}},
 {L"en", L"Cancel",
  L"program still needs to close:",
  L"programs still need to close:",
  L"This program is not responding.",
  {L"To close the program that is preventing Windows from logging off, click Cancel, and then close the program.",
   L"To close the program that is preventing Windows from shutting down, click Cancel, and then close the program.",
   L"To close the program that is preventing Windows from restarting, click Cancel, and then close the program."},
  {L"Force log off", L"Force shut down", L"Force restart"},
  {L"This program is preventing Windows from logging off.",
   L"This program is preventing Windows from shutting down.",
   L"This program is preventing Windows from restarting."}}
};
// The English row is the fallback, so it must stay last; a mistake here would
// silently hand every unknown locale the wrong language.
static_assert(ARRAYSIZE(kTexts) == 21, "one row per supported locale");

// The language setting wins when it names one of the supported locales;
// otherwise the Windows user locale is matched by prefix, exactly as before.
// An unrecognised forced value falls through to the automatic path rather
// than to English, so a typo in the setting cannot silently change the
// language away from the one the system asks for.
static const UiText* GetUiText() {
    if (g_forcedLocale[0]) {
        for (const auto& t : kTexts)
            if (_wcsicmp(g_forcedLocale, t.locale) == 0) return &t;
    }
    wchar_t l[LOCALE_NAME_MAX_LENGTH]{};
    GetUserDefaultLocaleName(l, ARRAYSIZE(l));
    for (const auto& t : kTexts)
        if (_wcsnicmp(l, t.locale, wcslen(t.locale)) == 0) return &t;
    return &kTexts[ARRAYSIZE(kTexts) - 1]; /* English is the last row */
}
struct OpenProgram {
    std::wstring name;
    HICON icon = nullptr;
    DWORD pid = 0;
    // True when the window has stopped answering messages. Windows 7 listed
    // the programs that were actively blocking the shutdown first, so this
    // flag drives both the sort order and the per-row note.
    bool blocking = false;
};
static std::vector<OpenProgram> g_openPrograms;
static std::wstring ProgramListSignature() {
    std::wstring r;
    for (const auto& p : g_openPrograms) { r += p.blocking ? L"!" : L"-"; r += p.name; r += L"\n"; }
    return r;
}

static HICON CloneIcon(HICON icon, int cx, int cy) {
    if (!icon) return nullptr;
    return (HICON)CopyImage(icon, IMAGE_ICON, cx, cy, LR_COPYFROMRESOURCE | LR_DEFAULTSIZE);
}

// A directed SendMessage to a window of another thread is delivered only
// when that thread pumps messages; a hung window -- exactly the app this mod
// exists to report -- never pumps, so a plain SendMessageW(WM_GETICON) blocks
// the calling thread forever, freezing the shell UI thread that triggered
// the logoff. SendMessageTimeoutW with SMTO_ABORTIFHUNG gives up instead, and
// SMTO_BLOCK stops our own message queue being re-entered while we wait.
static HICON QueryWindowIcon(HWND window, int type) {
    DWORD_PTR result = 0;
    LRESULT ok = SendMessageTimeoutW(window, WM_GETICON, (WPARAM)type, 0,
                                     SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, &result);
    return ok ? (HICON)result : nullptr;
}

static HICON GetProgramIcon(HWND window, DWORD pid, const wchar_t* exePath) {
    auto cached = g_iconCache.find(pid);
    if (cached != g_iconCache.end() && cached->second) return cached->second;

    HICON source = QueryWindowIcon(window, ICON_BIG);
    if (!source) source = QueryWindowIcon(window, ICON_SMALL2);
    if (!source) source = QueryWindowIcon(window, ICON_SMALL);
    // GCLP_HICON is a direct read of the window's class word, no message is
    // sent, so it cannot block.
    if (!source) source = (HICON)GetClassLongPtrW(window, GCLP_HICON);
    if (!source) source = (HICON)GetClassLongPtrW(window, GCLP_HICONSM);

    HICON copy = CloneIcon(source, 32, 32);
    if (copy) { g_iconCache[pid] = copy; return copy; }

    // More reliable fallback: extract the icon from the executable itself.
    if (exePath && *exePath) {
        HICON large = nullptr, small = nullptr;
        if (ExtractIconExW(exePath, 0, &large, &small, 1) > 0) {
            HICON result = CloneIcon(large ? large : small, 32, 32);
            if (large) DestroyIcon(large);
            if (small) DestroyIcon(small);
            if (result) { g_iconCache[pid] = result; return result; }
        }
    }

    if (exePath && *exePath) {
        SHFILEINFOW fi{};
        if (SHGetFileInfoW(exePath, 0, &fi, sizeof(fi), SHGFI_ICON | SHGFI_LARGEICON) && fi.hIcon) {
            HICON result = CloneIcon(fi.hIcon, 32, 32);
            DestroyIcon(fi.hIcon);
            if (result) { g_iconCache[pid] = result; return result; }
        }
    }
    // Last-resort generic Windows application icon.
    HICON fallback = CloneIcon(LoadIconW(nullptr, IDI_APPLICATION), 32, 32);
    if (fallback) g_iconCache[pid] = fallback;
    return fallback;
}

// Background/system host processes that own hidden or auxiliary top-level
// windows (input method hosts, the lock screen, the shell frame host,
// search, etc.). These are never something the user consciously "closes",
// so they must never show up in the shutdown list even if EnumWindows sees
// their window as visible.
// applicationframehost.exe and systemsettings.exe are deliberately NOT in
// this list: the former hosts every classic UWP/Store app (Calculator,
// Photos, Mail, ...) and the latter is Settings itself, so blanket-excluding
// them dropped a whole class of running programs from the list and the
// headline count. explorer.exe is excluded here even though it also hosts
// File Explorer windows -- those are let back in below by resolving each
// window's *real* owning process instead of trusting the top-level PID, the
// same way an ApplicationFrameWindow's real owner is resolved.
static bool IsSystemHostProcess(const wchar_t* exeName) {
    static const wchar_t* const kHosts[] = {
        L"textinputhost.exe", L"lockapp.exe",
        L"shellexperiencehost.exe", L"startmenuexperiencehost.exe",
        L"searchhost.exe", L"searchui.exe", L"searchapp.exe",
        L"sihost.exe", L"ctfmon.exe",
        L"dwm.exe", L"taskhostw.exe",
        L"windowsinternal.composableshell.experiences.textinput.inputapp.exe",
    };
    for (const wchar_t* h : kHosts) if (_wcsicmp(exeName, h) == 0) return true;
    return false;
}

// Shell surfaces that must never be listed as "programs still need to close".
// Matched by window class because captions are localized; the host processes
// are separately excluded by IsSystemHostProcess.
static bool IsShellSurfaceClass(HWND w) {
    wchar_t cls[128]{};
    GetClassNameW(w, cls, ARRAYSIZE(cls));
    if (!cls[0]) return false;
    // UWP/composited shell surfaces: the modern Start menu, Action Center, etc.
    if (_wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) return true;
    // The classic Start menu host window.
    if (_wcsicmp(cls, L"DV2ControlHost") == 0) return true;
    // The taskbar and its secondary (second-monitor) instances.
    if (_wcsicmp(cls, L"Shell_TrayWnd") == 0) return true;
    if (_wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) return true;
    // The desktop itself (Progman, and the WorkerW instances Explorer
    // creates alongside it). explorer.exe is no longer blanket-excluded by
    // process name, so without this the desktop would show up as a "program".
    if (_wcsicmp(cls, L"Progman") == 0) return true;
    if (_wcsicmp(cls, L"WorkerW") == 0) return true;
    return false;
}

// A classic UWP/Store app's visible top-level window belongs to
// ApplicationFrameHost.exe, not to the app itself: the app's own content
// lives in a child Windows.UI.Core.CoreWindow owned by a different process.
// Resolving that child's PID gives the entry the app's own name/icon instead
// of ApplicationFrameHost's, and lets EnumChildWindows-based dedup work per
// app rather than collapsing every open Store app into one frame-host entry.
static BOOL CALLBACK FindCoreWindowChild(HWND child, LPARAM lp) {
    wchar_t cls[64]{};
    GetClassNameW(child, cls, ARRAYSIZE(cls));
    if (_wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        *reinterpret_cast<HWND*>(lp) = child;
        return FALSE;   // found it, stop enumerating
    }
    return TRUE;
}

static DWORD ResolveRealOwnerPid(HWND w, DWORD framePid, const wchar_t* frameExe) {
    if (!frameExe || _wcsicmp(frameExe, L"applicationframehost.exe") != 0)
        return framePid;
    HWND core = nullptr;
    EnumChildWindows(w, FindCoreWindowChild, reinterpret_cast<LPARAM>(&core));
    if (!core) return framePid;
    DWORD corePid = 0;
    GetWindowThreadProcessId(core, &corePid);
    return corePid ? corePid : framePid;
}

static BOOL CALLBACK CollectVisibleWindows(HWND w, LPARAM) {
    try {
        if (!IsWindowVisible(w) || GetWindow(w, GW_OWNER)) return TRUE;
        // Tool windows (e.g. floating utility panels) are never full
        // programs a user would expect to see in this list.
        if (GetWindowLongPtrW(w, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
        // UWP/shell surfaces are frequently kept alive off-screen and
        // reported as "cloaked" by DWM instead of being truly hidden.
        BOOL cloaked = FALSE;
        if (SUCCEEDED(DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked)
            return TRUE;
        wchar_t title[256]{};
        // GetWindowTextW on a foreign window already applies USER32's
        // internal hang timeout, so it cannot deadlock the way a plain
        // SendMessage would.
        if (!GetWindowTextW(w, title, ARRAYSIZE(title)) || !title[0]) return TRUE;
        // Shell surfaces (the Start menu, the taskbar, composited UWP hosts)
        // are not programs that must be closed. Match them by window class,
        // never by caption: a caption like "Start" is localized, so a German
        // or Japanese system would otherwise list the Start menu here, and any
        // user window literally titled "Start" would be silently dropped. The
        // host processes are covered by IsSystemHostProcess below; this catches
        // the remaining explorer-owned surfaces.
        if (IsShellSurfaceClass(w)) return TRUE;
        DWORD pid=0; GetWindowThreadProcessId(w, &pid);
        wchar_t path[MAX_PATH]{}; DWORD n=ARRAYSIZE(path);
        HANDLE h=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        std::wstring exeName;
        if (h) {
            if (QueryFullProcessImageNameW(h, 0, path, &n)) {
                const wchar_t* b=wcsrchr(path, L'\\');
                if (b) exeName = b + 1;
            }
            CloseHandle(h);
        }
        if (!exeName.empty() && IsSystemHostProcess(exeName.c_str())) return TRUE;
        // For a UWP/Store app, swap the frame host's PID/path for the app's
        // own, so the name, icon and per-process dedup below all reflect the
        // actual app rather than ApplicationFrameHost.exe.
        DWORD realPid = ResolveRealOwnerPid(w, pid, exeName.c_str());
        if (realPid != pid) {
            wchar_t realPath[MAX_PATH]{}; n = ARRAYSIZE(realPath);
            HANDLE rh = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, realPid);
            if (rh) {
                if (QueryFullProcessImageNameW(rh, 0, realPath, &n)) {
                    const wchar_t* b = wcsrchr(realPath, L'\\');
                    exeName = b ? b + 1 : realPath;
                    wcscpy_s(path, realPath);
                }
                CloseHandle(rh);
            }
            pid = realPid;
        }
        // One line per process: a program with several top-level windows
        // (or several invisible worker windows) must not appear twice.
        for (const auto& p : g_openPrograms) if (p.pid == pid) return TRUE;
        std::wstring x=title;
        if (!exeName.empty()) x += L" (" + exeName + L")";
        // A window that no longer pumps messages is what actually holds the
        // shutdown back, so remember it: those entries are listed first.
        bool blocking = IsHungAppWindow(w) != FALSE;
        g_openPrograms.push_back({x, GetProgramIcon(w, pid, path), pid, blocking});
    } catch (...) {}
    return TRUE;
}

// Drops cached icons of processes that are no longer in the list, so a
// closed program does not leak its HICON for the lifetime of the process.
static void PruneIconCache() {
    for (auto it = g_iconCache.begin(); it != g_iconCache.end();) {
        bool stillOpen = false;
        for (const auto& p : g_openPrograms) {
            if (p.pid == it->first) { stillOpen = true; break; }
        }
        if (stillOpen) { ++it; continue; }
        if (it->second) DestroyIcon(it->second);
        it = g_iconCache.erase(it);
    }
}

static int RefreshOpenPrograms(){
 try {
  std::wstring before=ProgramListSignature();
  size_t beforeCount=g_totalPrograms;
  g_openPrograms.clear();
  EnumWindows(CollectVisibleWindows,0);

  // Windows 7 showed the programs that were actively blocking the shutdown
  // at the top of the list. stable_sort keeps the enumeration (z-order)
  // sequence within each group, so entries don't jump around between the
  // one-second refreshes.
  std::stable_sort(g_openPrograms.begin(), g_openPrograms.end(),
                   [](const OpenProgram& a, const OpenProgram& b){
                       return a.blocking && !b.blocking;
                   });

  // The heading always reports the true total, exactly like Windows 7 did,
  // even in the pathological case where the safety bound below kicks in.
  g_totalPrograms = g_openPrograms.size();

  // There is no artificial three-item truncation any more: every program
  // returned by the (unchanged) filters in CollectVisibleWindows is listed
  // and reachable by scrolling. Only the generous safety bound is enforced;
  // icons of dropped entries stay in the PID cache and are pruned below.
  if(g_openPrograms.size()>kMaxListedPrograms){
      g_openPrograms.resize(kMaxListedPrograms);
  }
  PruneIconCache();
  if (g_totalPrograms!=beforeCount) return kListLayoutChanged;
  return before!=ProgramListSignature() ? kListContentChanged : kListUnchanged;
 } catch (...) { g_openPrograms.clear(); g_totalPrograms=0; PruneIconCache(); Wh_Log(L"Program enumeration failed"); return kListLayoutChanged; }
}
static const wchar_t* GetActionText()  { return GetUiText()->force[g_action]; }
static const wchar_t* GetActionBody()  { return GetUiText()->body[g_action]; }
static const wchar_t* GetBlockedNote() { return GetUiText()->blocked[g_action]; }
static const wchar_t* GetNotRespondingNote() { return GetUiText()->notResponding; }

// Picks the grammatically-agreeing suffix for the given count, so the
// heading never reads like "3 programma" -- singular is used only for 1.
static const wchar_t* GetCountSuffix(unsigned count) {
    const UiText* t = GetUiText();
    return count == 1 ? t->stillOne : t->stillMany;
}

static HFONT CreateSystemFont(int height, LONG weight = FW_NORMAL, int scalePercent = 100) {
    NONCLIENTMETRICSW ncm{}; ncm.cbSize=sizeof(ncm);
    LOGFONTW lf{};
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,sizeof(ncm),&ncm,0)) lf=ncm.lfMessageFont;
    else wcscpy_s(lf.lfFaceName,L"Segoe UI");
    lf.lfHeight=-((height * scalePercent + 50) / 100); lf.lfWeight=weight; lf.lfQuality=CLEARTYPE_QUALITY;
    return CreateFontIndirectW(&lf);
}

// The button font is defined once so that the code that measures the labels
// and the code that draws them can never disagree.
static HFONT CreateButtonFont() { return CreateSystemFont(DpiScale(13), FW_NORMAL, 103); }

// Width the given label needs inside a button, padding included. The action
// labels differ wildly in length between languages and between log off / shut
// down / restart ("Force log off" against "Завершить работу принудительно"),
// so the buttons are measured rather than fixed at 160 px: a clipped or
// ellipsised label would defeat the point of getting the wording right.
// Geometry of the leading power symbol, derived from the button height so
// measuring and drawing always agree. On the reference screenshot the symbol
// is a touch smaller than the button's inner height and sits close to the
// left edge.
static constexpr int kGlyphMarginLeft = 6;
static constexpr int kGlyphGapToText  = 6;
static int GlyphSizeFor(int buttonHeight) { return std::max(8, buttonHeight - 10); }
static int GlyphRoom(int buttonHeight) {
    return DpiScale(kGlyphMarginLeft) + GlyphSizeFor(buttonHeight) + DpiScale(kGlyphGapToText);
}

static int MeasureButtonWidth(const wchar_t* text, bool isConfirm = false) {
    const int kPadding  = DpiScale(isConfirm ? g_skin->buttonPadding  : g_skin->cancelPadding);
    const int kMinWidth = DpiScale(isConfirm ? g_skin->buttonMinWidth : g_skin->cancelMinWidth);
    int width = kMinWidth;
    // The Vista confirm button spends part of its width on the leading power
    // symbol, so that space is reserved here rather than stolen from the
    // label: the glyph itself, its left margin and the gap before the text.
    const int glyphRoom = (isConfirm && g_skin->powerGlyph) ? GlyphRoom(DpiScale(g_skin->buttonHeight)) : 0;
    if (HDC dc = GetDC(nullptr)) {
        if (HFONT font = CreateButtonFont()) {
            HGDIOBJ old = SelectObject(dc, font);
            SIZE sz{};
            if (GetTextExtentPoint32W(dc, text, static_cast<int>(wcslen(text)), &sz)) {
                int needed = sz.cx + kPadding + glyphRoom;
                if (needed > width) width = needed;
            }
            SelectObject(dc, old); DeleteObject(font);
        }
        ReleaseDC(nullptr, dc);
    }
    return width;
}

static COLORREF LerpColor(COLORREF a, COLORREF b, int t /*0..255*/) {
    return RGB(GetRValue(a) + (GetRValue(b) - GetRValue(a)) * t / 255,
               GetGValue(a) + (GetGValue(b) - GetGValue(a)) * t / 255,
               GetBValue(a) + (GetBValue(b) - GetBValue(a)) * t / 255);
}

// isDefault is kept in the signature for call-site clarity but no longer
// changes the palette -- both buttons now share the same glossy blue style.
// The symbol Vista stamps on its red button, taken from the reference
// screenshot: a dark disc, with a white ring and a white vertical bar drawn
// over it -- not a plain outline, and not the Windows 7 power symbol whose
// ring is broken at the top.
//
// The shape is generated analytically rather than shipped as a pre-rasterised
// blob, so a reviewer (or a user reading the source before enabling the mod in
// explorer.exe) can tell exactly what it draws: a ring plus a vertical bar, a
// few lines of distance/rect coverage math with 4x4 supersampling for a smooth
// edge. No Arc/Ellipse jaggies, no glyph-font dependency, no base64 payload.
static constexpr int kPowerGlyphMaskSize = 64;

// Builds one 64x64 eight-bit coverage mask. includeDisc selects between the
// two layers the painter composites:
//   false  -> the ring (annulus) and the vertical bar in the label colour;
//            everything else stays transparent, so the button gradient shows
//            through the middle of the symbol.
//   true   -> a solid disc (the whole ring interior, ring edge included),
//            used only by the flat-fill skin option as a uniform backdrop.
static bool BuildPowerGlyphMask(std::vector<BYTE>& mask, bool includeDisc) {
    constexpr int S  = kPowerGlyphMaskSize;
    constexpr int SS = 4;                 // coverage samples per axis
    mask.assign(S * S, 0);
    const float cx  = (S - 1) * 0.5f;     // 31.5
    const float cy  = (S - 1) * 0.5f;
    const float R      = S * 0.40f;       // ring mid radius
    const float halfTh = S * 0.045f;      // half thickness -> ~5.8 px ring
    const float ROut   = R + halfTh;      // ring outer radius / disc edge
    const float barHW  = S * 0.05f;       // vertical bar half width
    const float barTop = cy - ROut - S * 0.015f;  // a touch above the ring
    const float barBot = cy;                       // down to the centre

    for (int py = 0; py < S; ++py) {
        for (int px = 0; px < S; ++px) {
            int hit = 0;
            for (int sy = 0; sy < SS; ++sy) {
                const float fy = py + (sy + 0.5f) / SS;
                for (int sx = 0; sx < SS; ++sx) {
                    const float fx = px + (sx + 0.5f) / SS;
                    const float dx = fx - cx;
                    const float dy = fy - cy;
                    const float dist = std::sqrt(dx * dx + dy * dy);
                    bool on = false;
                    if (includeDisc) {
                        on = dist <= ROut;
                    } else {
                        if (std::fabs(dist - R) <= halfTh) on = true;      // ring
                        if (std::fabs(fx - cx) <= barHW &&                  // bar
                            fy >= barTop && fy <= barBot) on = true;
                    }
                    if (on) ++hit;
                }
            }
            mask[py * S + px] = static_cast<BYTE>((hit * 255) / (SS * SS));
        }
    }
    return true;
}

static const BYTE* GetPowerGlyphMask() {
    static std::vector<BYTE> mask;
    static bool tried = false;
    if (!tried) { tried = true; BuildPowerGlyphMask(mask, false); }
    return mask.empty() ? nullptr : mask.data();
}

static const BYTE* GetPowerGlyphDisc() {
    static std::vector<BYTE> mask;
    static bool tried = false;
    if (!tried) { tried = true; BuildPowerGlyphMask(mask, true); }
    return mask.empty() ? nullptr : mask.data();
}

// Box-filters one mask down to the requested size at pixel (x, y).
static int SamplePowerGlyphMask(const BYTE* mask, int x, int y, int size) {
    const int src = kPowerGlyphMaskSize;
    const int sy0 = y * src / size, sy1 = std::max(sy0 + 1, (y + 1) * src / size);
    const int sx0 = x * src / size, sx1 = std::max(sx0 + 1, (x + 1) * src / size);
    int sum = 0, n = 0;
    for (int sy = sy0; sy < sy1; ++sy)
        for (int sx = sx0; sx < sx1; ++sx) { sum += mask[sy * src + sx]; ++n; }
    return n ? sum / n : 0;
}

// Composites the cached masks over whatever the button gradient already put on
// the DC.
//
// Two layers, bottom to top:
//
//   1. the flat fill, only when the skin asks for it: the ring's interior is
//      flooded with one uniform tone so the middle of the symbol cannot show
//      the gradient's lighter top and darker bottom. Without it the interior
//      is left untouched and the gradient continues through the symbol.
//   2. the ring and bar, in the label colour.
//
// Everything outside the ring stays fully transparent either way, so the
// symbol never sits on a dark square.
static void DrawPowerGlyph(HDC dc, int left, int top, int size, COLORREF colour,
                           bool flatFill, COLORREF fillColour) {
    const BYTE* ring = GetPowerGlyphMask();
    if (!ring || size <= 0) return;
    const BYTE* disc = flatFill ? GetPowerGlyphDisc() : nullptr;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;          // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) { if (bmp) DeleteObject(bmp); return; }

    const int rr = GetRValue(colour), rg = GetGValue(colour), rb = GetBValue(colour);
    const int fr = GetRValue(fillColour), fg = GetGValue(fillColour), fb = GetBValue(fillColour);
    BYTE* dst = static_cast<BYTE*>(bits);
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            // Source-over of the fill first, then the ring on top of it, all
            // premultiplied for AlphaBlend. Coverage 0 in both layers leaves
            // the button gradient untouched.
            const int a = SamplePowerGlyphMask(ring, x, y, size);
            int r = rr * a / 255, g = rg * a / 255, b = rb * a / 255, outA = a;
            if (disc) {
                const int da = SamplePowerGlyphMask(disc, x, y, size);
                if (da > 0) {
                    // Premultiplied fill, then the ring composited over it.
                    const int inv = 255 - a;
                    r += fr * da / 255 * inv / 255;
                    g += fg * da / 255 * inv / 255;
                    b += fb * da / 255 * inv / 255;
                    outA = a + da * inv / 255;
                }
            }
            BYTE* px = dst + (y * size + x) * 4;
            px[0] = (BYTE)std::min(255, b);
            px[1] = (BYTE)std::min(255, g);
            px[2] = (BYTE)std::min(255, r);
            px[3] = (BYTE)std::min(255, outA);
        }
    }

    HDC mem = CreateCompatibleDC(dc);
    if (mem) {
        HGDIOBJ oldBmp = SelectObject(mem, bmp);
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        AlphaBlend(dc, left, top, size, size, mem, 0, 0, size, size, bf);
        SelectObject(mem, oldBmp);
        DeleteDC(mem);
    }
    DeleteObject(bmp);
}

// isConfirm distinguishes the action button from Cancel: the two use separate
// palettes (identical under the Windows 7 skin, red against grey under Vista)
// and only the action button carries the power glyph.
static void DrawButton(HDC dc, const RECT& r, const wchar_t* text, bool hot, bool isConfirm) {
    const Skin& sk = *g_skin;
    const ButtonPalette& pal = isConfirm ? sk.force : sk.cancel;

    // Vertical gradient shaped as a real Aero glass "capsule": bright top
    // sheen, a quick drop to the body tone, a dark trough, then a long smooth
    // climb back to a bright band at the very bottom edge. The stop positions
    // below were fit directly to pixel samples from a real Windows 7
    // screenshot (both a full-screen capture and an isolated button crop
    // agreed almost exactly): the darkest row sits at t=0.536 of the button's
    // height, not at the halfway or two-thirds mark, and the climb back up
    // reaches its peak brightness only right at the bottom edge -- a single
    // continuous curve, not two flat segments bolted together.
    COLORREF topC = hot ? pal.topHot : pal.top;
    COLORREF midC = hot ? pal.midHot : pal.mid;
    COLORREF botC = hot ? pal.botHot : pal.bot;
    COLORREF edgeC = LerpColor(midC, topC, 200); // bright band right at the bottom edge
    struct Stop { float t; COLORREF c; };
    const Stop stops[] = {
        {0.00f, topC},
        {0.13f, midC},
        {0.536f, botC},
        {1.00f, edgeC},
    };
    int height = std::max(1, static_cast<int>(r.bottom - r.top - 1));
    for (int y = r.top; y < r.bottom; ++y) {
        float t = float(y - r.top) / float(height);
        int seg = 0;
        while (seg < 2 && t > stops[seg + 1].t) ++seg;
        float span = stops[seg + 1].t - stops[seg].t;
        int frac = span > 0.f ? int(std::clamp((t - stops[seg].t) / span, 0.f, 1.f) * 255) : 255;
        COLORREF c = LerpColor(stops[seg].c, stops[seg + 1].c, frac);
        HBRUSH b = CreateSolidBrush(c);
        if (b) { RECT row{r.left, y, r.right, y + 1}; FillRect(dc, &row, b); DeleteObject(b); }
    }

    // Soft specular highlight strip near the top, alpha-blended for a glassy sheen.
    int hiH = std::max(2, height / 4);
    RECT hi{r.left + DpiScale(2), r.top + DpiScale(1), r.right - DpiScale(2), r.top + DpiScale(1) + hiH};
    HDC hiDc = CreateCompatibleDC(dc);
    int hw = hi.right - hi.left, hh = hi.bottom - hi.top;
    if (hw > 0 && hh > 0) {
        HBITMAP hiBmp = CreateCompatibleBitmap(dc, hw, hh);
        if (hiDc && hiBmp) {
            HGDIOBJ oldBmp = SelectObject(hiDc, hiBmp);
            HBRUSH hiBrush = CreateSolidBrush(RGB(255, 255, 255));
            RECT local{0, 0, hw, hh};
            if (hiBrush) { FillRect(hiDc, &local, hiBrush); DeleteObject(hiBrush); }
            BLENDFUNCTION bf{AC_SRC_OVER, 0, 60, 0};
            AlphaBlend(dc, hi.left, hi.top, hw, hh, hiDc, 0, 0, hw, hh, bf);
            SelectObject(hiDc, oldBmp);
            DeleteObject(hiBmp);
        }
    }
    if (hiDc) DeleteDC(hiDc);

    // Rounded outline, in the skin's own border tone.
    HPEN border = CreatePen(PS_SOLID, 1, pal.border);
    if (border) {
        HGDIOBJ oldPen = SelectObject(dc, border);
        HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
        RoundRect(dc, r.left, r.top, r.right, r.bottom,
                  DpiScale(sk.cornerRadius), DpiScale(sk.cornerRadius));
        SelectObject(dc, oldPen); SelectObject(dc, oldBrush);
        DeleteObject(border);
    }

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, pal.text);
    HFONT buttonFont=CreateButtonFont();
    HGDIOBJ oldFont=buttonFont ? SelectObject(dc,buttonFont) : nullptr;
    RECT textRect = r;
    // Vista puts the symbol before the label, at the left end of the button,
    // so the confirm button reserves room there and the text is centred in
    // what is left. The label and the glyph therefore never overlap, however
    // long the translation is.
    if (isConfirm && sk.powerGlyph) {
        const int glyphSize = GlyphSizeFor((int)(r.bottom - r.top));
        const int glyphLeft = (int)r.left + DpiScale(kGlyphMarginLeft);
        const int glyphTop = (int)(r.top + r.bottom - glyphSize) / 2;
        // The flat fill follows the hover state, so the inside of the symbol
        // stays the same tone as the button body it sits on in both states.
        DrawPowerGlyph(dc, glyphLeft, glyphTop, glyphSize, pal.text,
                       sk.glyphFlatFill, hot ? pal.midHot : pal.mid);
        textRect.left = glyphLeft + glyphSize + DpiScale(kGlyphGapToText);
    }
    // DT_END_ELLIPSIS is only a last resort: the button was already measured
    // to fit its label, and it can bite only if the pair had to be clamped to
    // the content column on a very narrow panel.
    DrawTextW(dc, text, -1, &textRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
    if (buttonFont) { SelectObject(dc,oldFont); DeleteObject(buttonFont); }
}

// ---------------------------------------------------------------------------
// Layout
//
// Everything used to be hardcoded for exactly three rows (the list region was
// the fixed band oy+130 .. oy+316 inside a fixed 800x580 panel). The panel is
// now measured from the actual number of programs, so ten open programs are
// listed as ten rows instead of silently dropping seven of them. All message
// handlers (WM_PAINT, WM_TIMER, WM_MOUSEMOVE, WM_LBUTTONUP, WM_MOUSEWHEEL)
// ask this single function for their coordinates, so the hit-testing can
// never drift away from what is painted.
// ---------------------------------------------------------------------------
struct DialogLayout {
    int ox = 0, oy = 0;          // panel origin, in client coordinates
    int width = 800, height = 580;
    int contentLeft = 0, contentRight = 0;
    RECT title{};
    int listTop = 0;             // upper separator
    int rowsTop = 0;             // first row
    int rowH = 56;
    int iconSize = 32;
    int nameFont = 19, noteFont = 14;
    bool compact = false;        // list switched to reduced metrics
    int visibleRows = 0;         // rows actually drawn
    int hiddenRows = 0;          // rows reachable only by scrolling
    int listBottom = 0;          // lower separator
    RECT listRegion{};           // area invalidated when the list changes
    RECT track{};                // scrollbar track (empty when not scrolling)
    RECT thumb{};                // scrollbar thumb
    RECT body{};
    RECT force{};
    RECT cancel{};
};

static const int kPanelWidth = 800;
static const int kListPadTop = 18;
static const int kListPadBot = 14;

// Normal metrics: the roomy Windows 7 row, 32px icon and two lines of text.
static const int kRowHeight = 56, kIconSize = 32;
static const int kNameFont  = 19, kNoteFont = 14;
// Compact metrics: used when there are more programs than rows available, so
// more of them fit before the list has to scroll (Windows 7 likewise shrank
// the entries rather than hiding them behind a fixed cap).
static const int kRowHeightCompact = 40, kIconSizeCompact = 24;
static const int kNameFontCompact  = 16, kNoteFontCompact = 12;

static DialogLayout ComputeLayout(int clientW, int clientH, const RECT& monitorClient,
                                  size_t programCount) {
    DialogLayout L;
    L.width = DpiScale(kPanelWidth);

    // The panel is placed and sized within the target monitor rather than the
    // whole virtual desktop, so on a multi-monitor setup it sits wholly on one
    // screen instead of straddling a bezel. A degenerate rect falls back to the
    // full window (single-monitor / unusual geometry).
    const int monW = monitorClient.right - monitorClient.left;
    const int monH = monitorClient.bottom - monitorClient.top;
    const bool haveMon = monW > 0 && monH > 0;
    const int useMonW = haveMon ? monW : clientW;
    const int useMonH = haveMon ? monH : clientH;
    const int monLeft = haveMon ? monitorClient.left : 0;
    const int monTop  = haveMon ? monitorClient.top  : 0;

    // Vertical space the panel needs above and below the list itself.
    // Every value below is a 96-DPI reference measurement, scaled to the
    // current DPI (100% = 96 DPI, 125% = 120 DPI, 150% = 144 DPI, ...).
    const int chromeAbove = DpiScale(130) + DpiScale(kListPadTop);      // panel top -> first row
    const int chromeBelow = DpiScale(kListPadBot) + DpiScale(20) + DpiScale(68)   // pad + gap + body text
                          + DpiScale(20) + DpiScale(g_skin->buttonHeight) + DpiScale(70);  // gap + buttons + margin
    const int room = useMonH - chromeAbove - chromeBelow - DpiScale(24);
    const int count = static_cast<int>(programCount);

    auto rowsThatFit = [&](int rowH, int cap) {
        int n = room / rowH;
        if (n < 1) n = 1;
        if (n > cap) n = cap;
        return n;
    };

    // Start with the roomy Windows 7 row. If the programs don't all fit, try
    // the compact metrics before giving up and scrolling.
    int fit = rowsThatFit(DpiScale(kRowHeight), kMaxVisibleRows);
    L.compact = false;
    if (count > fit) {
        int fitCompact = rowsThatFit(DpiScale(kRowHeightCompact), kMaxVisibleRowsCompact);
        if (fitCompact > fit) { L.compact = true; fit = fitCompact; }
    }

    L.rowH     = L.compact ? DpiScale(kRowHeightCompact) : DpiScale(kRowHeight);
    L.iconSize = L.compact ? DpiScale(kIconSizeCompact)  : DpiScale(kIconSize);
    L.nameFont = L.compact ? DpiScale(kNameFontCompact)  : DpiScale(kNameFont);
    L.noteFont = L.compact ? DpiScale(kNoteFontCompact)  : DpiScale(kNoteFont);

    L.visibleRows = count < fit ? count : fit;
    if (L.visibleRows < 1) L.visibleRows = 1;       // never collapse the band
    L.hiddenRows = count - L.visibleRows;
    if (L.hiddenRows < 0) L.hiddenRows = 0;

    const int listContent = L.visibleRows * L.rowH;

    L.listTop    = DpiScale(130);
    L.rowsTop    = L.listTop + DpiScale(kListPadTop);
    L.listBottom = L.rowsTop + listContent + DpiScale(kListPadBot);

    const int bodyTop    = L.listBottom + DpiScale(20);
    const int buttonsTop = bodyTop + DpiScale(68) + DpiScale(20);
    L.height             = buttonsTop + DpiScale(g_skin->buttonHeight) + DpiScale(70);

    L.ox = monLeft + (useMonW - L.width) / 2;
    L.oy = monTop + (useMonH - L.height) / 2;
    if (L.ox < monLeft) L.ox = monLeft;
    if (L.oy < monTop)  L.oy = monTop;

    const int ox = L.ox, oy = L.oy;
    L.contentLeft  = ox + DpiScale(90);
    L.contentRight = ox + DpiScale(700);
    L.title  = RECT{ox + DpiScale(90), oy + DpiScale(78), ox + DpiScale(700), oy + DpiScale(118)};
    L.listTop    += oy;
    L.rowsTop    += oy;
    L.listBottom += oy;
    L.body   = RECT{ox + DpiScale(90), oy + bodyTop, ox + DpiScale(700), oy + bodyTop + DpiScale(68)};
    // Both buttons are measured from the text they will actually show and the
    // pair is centred in the panel, so a long localised label neither gets cut
    // off nor pushes the buttons out of alignment.
    const int gap = DpiScale(g_skin->buttonGap);
    const int btnH = DpiScale(g_skin->buttonHeight);
    int forceW  = MeasureButtonWidth(GetActionText(), /*isConfirm=*/true);
    int cancelW = MeasureButtonWidth(GetUiText()->cancel);
    const int maxPairW = L.width - 2 * DpiScale(90);   // stay inside the content column
    if (forceW + cancelW + gap > maxPairW) {
        const int share = (maxPairW - gap) / 2;
        if (forceW  > share) forceW  = share;
        if (cancelW > share) cancelW = share;
    }
    const int pairLeft = ox + (L.width - (forceW + gap + cancelW)) / 2;
    L.force  = RECT{pairLeft, oy + buttonsTop, pairLeft + forceW, oy + buttonsTop + btnH};
    L.cancel = RECT{L.force.right + gap, oy + buttonsTop,
                    L.force.right + gap + cancelW, oy + buttonsTop + btnH};

    // Vertical scrollbar, shown only when some programs are off-list. Its
    // geometry lives here so painting and mouse handling agree exactly.
    if (L.hiddenRows > 0 && count > 0) {
        const int trackTop = L.rowsTop, trackBot = L.rowsTop + listContent;
        L.track = RECT{ox + DpiScale(706), trackTop, ox + DpiScale(717), trackBot};
        const int trackH = trackBot - trackTop;
        int thumbH = trackH * L.visibleRows / count;
        if (thumbH < DpiScale(26)) thumbH = DpiScale(26);
        if (thumbH > trackH) thumbH = trackH;
        int scroll = g_listScroll;
        if (scroll > L.hiddenRows) scroll = L.hiddenRows;
        if (scroll < 0) scroll = 0;
        const int span = trackH - thumbH;
        const int thumbTop = trackTop + (L.hiddenRows > 0 ? span * scroll / L.hiddenRows : 0);
        L.thumb = RECT{L.track.left, thumbTop, L.track.right, thumbTop + thumbH};
    }

    L.listRegion = RECT{ox + DpiScale(84), L.listTop, ox + DpiScale(722), L.listBottom + 1};
    return L;
}
static DialogLayout LayoutForWindow(HWND hwnd) {
    RECT cr{}; GetClientRect(hwnd, &cr);
    int w = cr.right - cr.left, h = cr.bottom - cr.top;
    if (w <= 0) w = GetSystemMetrics(SM_CXSCREEN);
    if (h <= 0) h = GetSystemMetrics(SM_CYSCREEN);
    RECT mon = g_monitorClient;
    if (mon.right <= mon.left || mon.bottom <= mon.top) mon = RECT{0, 0, w, h};
    return ComputeLayout(w, h, mon, g_openPrograms.size());
}

// Keeps the scroll offset inside [0, hidden rows] after the list changes.
static bool ClampListScroll(const DialogLayout& L) {
    const int old = g_listScroll;
    if (g_listScroll > L.hiddenRows) g_listScroll = L.hiddenRows;
    if (g_listScroll < 0) g_listScroll = 0;
    return old != g_listScroll;
}

static bool ScrollList(HWND hwnd, int delta) {
    DialogLayout L = LayoutForWindow(hwnd);
    if (L.hiddenRows <= 0) return false;
    const int old = g_listScroll;
    g_listScroll += delta;
    ClampListScroll(L);
    if (g_listScroll == old) return false;
    InvalidateRect(hwnd, &L.listRegion, FALSE);
    return true;
}

// Maps a vertical mouse position on the scrollbar to a scroll offset.
static void ScrollToThumbPosition(HWND hwnd, int y) {
    DialogLayout L = LayoutForWindow(hwnd);
    if (L.hiddenRows <= 0) return;
    const int thumbH = L.thumb.bottom - L.thumb.top;
    const int span = (L.track.bottom - L.track.top) - thumbH;
    if (span <= 0) return;
    int pos = y - L.track.top - g_dragOffset;
    if (pos < 0) pos = 0;
    if (pos > span) pos = span;
    const int target = (pos * L.hiddenRows + span / 2) / span;
    if (target != g_listScroll) {
        g_listScroll = target;
        ClampListScroll(L);
        InvalidateRect(hwnd, &L.listRegion, FALSE);
    }
}

// ---------------------------------------------------------------------------
// The full-screen dialog and the UI thread that owns it
// ---------------------------------------------------------------------------

static DWORD WINAPI UiThreadProc(LPVOID);

// Runs on the UI thread. Creates the dialog; the outcome is written into the
// request (via GWLP_USERDATA) and reported when the window is destroyed
// (WM_DESTROY signals its reply event). It never runs a nested modal loop: the dialog is modeless on this
// thread, so the same loop also keeps the hotkey window alive (the preview
// hotkey can therefore never fire while a screen is already up) and a quit
// message posted to the thread is always reached.
// req may be null for the preview hotkey path, which is fire-and-forget and
// has nothing waiting on an outcome.
static void ShowScreenOnUiThread(ShutdownRequest* req, ActionKind action) {
    HANDLE replyEvent = req ? req->reply : nullptr;
    // Re-read every setting on the thread that consumes it.
    LoadSkinSetting();
    if (!g_enabled) {
        if (req) { req->force = false; req->proceed = true; }
        if (replyEvent) SetEvent(replyEvent);
        return;
    }

    if (req) { req->force = false; req->proceed = false; }
    g_action = action;
    g_listScroll = 0; g_draggingThumb = false; // always open at the top of the list
    RefreshOpenPrograms();

    // Nothing is holding the logoff back, so there is nothing to report and
    // nothing to decide: staying out of the way is what Windows 7 did too.
    // Putting up a screen that reads "0 programs still need to close:" would
    // be awkward and would only delay a logoff that can just go ahead.
    if (g_totalPrograms == 0) {
        g_openPrograms.clear();
        if (req) { req->force = false; req->proceed = true; }
        if (replyEvent) SetEvent(replyEvent);
        return;
    }

    g_dialogStart = GetTickCount64();
    // Pick the target monitor before the dialog exists: once the full-screen
    // window is created (and made foreground) it would become its own
    // foreground window, making a MonitorFromWindow on it meaningless.
    SetTargetMonitor(PickTargetMonitor());
    FreeDesktopBitmap(); g_desktop = CaptureDesktop();
    g_captureFailedAt = g_desktop ? 0 : g_dialogStart;
    RebuildBackdrop();

    HINSTANCE hMod = GetModModuleHandle();
    // Span the virtual desktop so every monitor is covered and the screen is
    // modal on a multi-monitor setup.
    const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    HWND dlg = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, kClassName, L"Windows",
                               WS_POPUP | WS_VISIBLE, vx, vy, vw, vh,
                               nullptr, nullptr, hMod, nullptr);
    if (!dlg) {
        Wh_Log(L"CreateWindowExW for the logoff screen failed");
        FreeDesktopBitmap();
        if (req) { req->force = false; req->proceed = true; }   // fail open: never block a logoff
        if (replyEvent) SetEvent(replyEvent);
        return;
    }
    g_dialog = dlg;
    // The request is stashed on the window so WM_DESTROY (and the other
    // outcome-setting handlers) can reach it and signal its own reply event.
    SetWindowLongPtrW(dlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(req));
    // The metrics are scaled to the target monitor's DPI (already set above).
    SetForegroundWindow(dlg);
    SetFocus(dlg);
}

static LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

static LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        SetTimer(hwnd, 1, 1000, nullptr);
        return 0;
    case WM_TIMER: {
        // Safety watchdog only: it never redraws. Rendering is event-driven.
        // The dialog is dismissed after 60 seconds so a logoff can never be
        // blocked forever -- but it proceeds UNFORCED, exactly like the
        // "last program closed by itself" path: force-closing apps on a timer
        // would throw away unsaved work for a user who simply walked away,
        // while stock Windows would instead show its own "app is preventing
        // shutdown" screen and wait.
        if (GetTickCount64() - g_dialogStart >= 60000) {
            if (auto* req = reinterpret_cast<ShutdownRequest*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)))
                { req->force = false; req->proceed = true; }
            DestroyWindow(hwnd);
            return 0;
        }
        const int changed = RefreshOpenPrograms();
        // Programs close by themselves in the background, so the list can
        // empty out while the screen is up. Showing "0 programs still
        // need to close:" would be nonsense, and there is nothing left to
        // wait for either: close the screen and let Windows carry on
        // normally (force stays false -- nothing had to be forced).
        if (g_totalPrograms == 0) {
            if (auto* req = reinterpret_cast<ShutdownRequest*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)))
                { req->force = false; req->proceed = true; }
            DestroyWindow(hwnd);
            return 0;
        }
        if (changed != kListUnchanged) {
            DialogLayout L = LayoutForWindow(hwnd);
            ClampListScroll(L);
            // When the number of entries changes the whole panel is
            // re-laid out (it grows/shrinks with the list), so a partial
            // invalidate would leave stale pixels behind.
            if (changed == kListLayoutChanged) InvalidateRect(hwnd, nullptr, FALSE);
            else InvalidateRect(hwnd, &L.listRegion, FALSE);
        }
        if (!g_desktop && GetTickCount64() - g_captureFailedAt >= 1000) {
            // CaptureDesktop() grabs whatever GetDC(nullptr) currently shows,
            // and this window is covering the whole virtual desktop, so
            // capturing without hiding it first would feed the dimmed
            // backdrop + panel back into itself, and RebuildBackdrop() would
            // alpha-blend the veil over that a second time. Hide the window
            // for the capture only; nothing else about it changes.
            ShowWindow(hwnd, SW_HIDE);
            FreeDesktopBitmap();
            g_desktop = CaptureDesktop();
            ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            g_captureFailedAt = g_desktop ? 0 : GetTickCount64();
            RebuildBackdrop();
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) {
            if (auto* req = reinterpret_cast<ShutdownRequest*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)))
                { req->force = false; req->proceed = false; }
            DestroyWindow(hwnd); return 0;
        }
        if (wp == VK_RETURN) {
            if (auto* req = reinterpret_cast<ShutdownRequest*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)))
                { req->force = true; req->proceed = true; }
            DestroyWindow(hwnd); return 0;
        }
        // Keyboard access to the part of the list that does not fit.
        if (wp == VK_DOWN)  { ScrollList(hwnd,  1); return 0; }
        if (wp == VK_UP)    { ScrollList(hwnd, -1); return 0; }
        if (wp == VK_NEXT)  { ScrollList(hwnd,  kMaxVisibleRows); return 0; }
        if (wp == VK_PRIOR) { ScrollList(hwnd, -kMaxVisibleRows); return 0; }
        if (wp == VK_HOME)  { ScrollList(hwnd, -(int)kMaxListedPrograms); return 0; }
        if (wp == VK_END)   { ScrollList(hwnd,  (int)kMaxListedPrograms); return 0; }
        return 0;
    case WM_MOUSEWHEEL: {
        const int notches = GET_WHEEL_DELTA_WPARAM(wp) / WHEEL_DELTA;
        if (notches) ScrollList(hwnd, -notches);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        // Dragging the scrollbar thumb, or clicking the track to page.
        DialogLayout L = LayoutForWindow(hwnd);
        if (L.hiddenRows <= 0) return 0;
        POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
        if (PtInRect(&L.thumb, pt)) {
            g_draggingThumb = true;
            g_dragOffset = pt.y - L.thumb.top;
            SetCapture(hwnd);
        } else if (PtInRect(&L.track, pt)) {
            ScrollList(hwnd, pt.y < L.thumb.top ? -L.visibleRows : L.visibleRows);
        }
        return 0;
    }
    case WM_MOUSEMOVE: {
        // Coordinates come from the same layout used by WM_PAINT, so the
        // buttons stay hit-testable no matter how tall the list has grown.
        DialogLayout L = LayoutForWindow(hwnd);
        if (g_draggingThumb) { ScrollToThumbPosition(hwnd, GET_Y_LPARAM(lp)); return 0; }
        RECT force = L.force, cancel = L.cancel;
        POINT pt{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
        bool nf=PtInRect(&force,pt), nc=PtInRect(&cancel,pt);
        if (nf != g_hoverForce) { g_hoverForce=nf; InvalidateRect(hwnd,&force,FALSE); }
        if (nc != g_hoverCancel) { g_hoverCancel=nc; InvalidateRect(hwnd,&cancel,FALSE); }
        return 0;
    }
    case WM_SIZE: {
        // A resolution change alters how many rows fit, so the scroll offset
        // has to be re-clamped before the next paint.
        DialogLayout L = LayoutForWindow(hwnd);
        ClampListScroll(L);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_DISPLAYCHANGE: {
        // Monitor added/removed or resolution changed: re-span the virtual
        // desktop and refresh the backdrop.
        const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
        const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
        const int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        const int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        SetWindowPos(hwnd, HWND_TOPMOST, vx, vy, vw, vh,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        // Hide around the capture -- see the WM_TIMER retry above for why.
        ShowWindow(hwnd, SW_HIDE);
        FreeDesktopBitmap();
        g_desktop = CaptureDesktop();
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        g_captureFailedAt = g_desktop ? 0 : GetTickCount64();
        SetTargetMonitor(PickTargetMonitor());
        RebuildBackdrop();
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_DPICHANGED: {
        // A monitor's scaling changed. Re-read the target monitor's DPI (the
        // spanning window's own reported DPI is ambiguous), re-capture and
        // repaint with the new metrics.
        SetTargetMonitor(g_monitor ? g_monitor : PickTargetMonitor());
        const RECT* suggested = reinterpret_cast<const RECT*>(lp);
        if (suggested && suggested->right > suggested->left) {
            SetWindowPos(hwnd, nullptr, suggested->left, suggested->top,
                         suggested->right - suggested->left,
                         suggested->bottom - suggested->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }
        // Hide around the capture -- see the WM_TIMER retry above for why.
        ShowWindow(hwnd, SW_HIDE);
        FreeDesktopBitmap();
        g_desktop = CaptureDesktop();
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        RebuildBackdrop();
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_LBUTTONUP: {
        if (g_draggingThumb) { g_draggingThumb = false; ReleaseCapture(); return 0; }
        const int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
        DialogLayout L = LayoutForWindow(hwnd);
        RECT force = L.force, cancel = L.cancel;
        if (PtInRect(&force, POINT{x,y})) {
            if (auto* req = reinterpret_cast<ShutdownRequest*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)))
                { req->force = true; req->proceed = true; }
            DestroyWindow(hwnd);
        } else if (PtInRect(&cancel, POINT{x,y})) {
            if (auto* req = reinterpret_cast<ShutdownRequest*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)))
                { req->force = false; req->proceed = false; }
            DestroyWindow(hwnd);
        }
        return 0;
    }
    case WM_CLOSE:
        // Alt+F4 / system close == Cancel.
        if (auto* req = reinterpret_cast<ShutdownRequest*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)))
            { req->force = false; req->proceed = false; }
        DestroyWindow(hwnd);
        return 0;
    case WM_ERASEBKGND:
        // The whole client area is always fully repainted in WM_PAINT (via
        // an off-screen buffer), so letting DefWindowProc erase it first
        // with the class background brush would just be a wasted, visibly
        // flickering paint pass.
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC screenDc = BeginPaint(hwnd, &ps);
        RECT cr{}; GetClientRect(hwnd, &cr);
        // Repaint only the region GDI reports as dirty. During hot tracking
        // that is just one button rect, so moving the mouse over a button edge
        // re-renders a few pixels instead of re-compositing the whole virtual
        // desktop on every move. An empty region (nothing invalidated) paints
        // the full window.
        RECT dirty = ps.rcPaint;
        if (!IntersectRect(&dirty, &dirty, &cr) ||
            dirty.right <= dirty.left || dirty.bottom <= dirty.top) {
            dirty = cr;
        }
        const int dw = dirty.right - dirty.left;
        const int dh = dirty.bottom - dirty.top;
        // Everything below is drawn into an off-screen buffer and blitted
        // to the window in one shot at the end, still flicker-free.
        HDC dc = CreateCompatibleDC(screenDc);
        HBITMAP backBmp = CreateCompatibleBitmap(screenDc, dw, dh);
        HGDIOBJ oldBackBmp = backBmp ? SelectObject(dc, backBmp) : nullptr;
        if (!backBmp) dc = screenDc; // Fallback: draw directly if allocation failed.
        if (backBmp) {
            // The cached backdrop (desktop + veil) is blitted as one BitBlt of
            // the dirty region; the per-pixel AlphaBlend of the veil and the
            // second desktop copy are gone from the repaint path.
            HDC src = CreateCompatibleDC(dc);
            if (src && g_backdrop) {
                HGDIOBJ old = SelectObject(src, g_backdrop);
                BitBlt(dc, 0, 0, dw, dh, src, dirty.left, dirty.top, SRCCOPY);
                SelectObject(src, old);
            } else {
                HBRUSH fallback = CreateSolidBrush(RGB(0x1a,0x2a,0x3a));
                RECT r{0, 0, dw, dh};
                if (fallback) { FillRect(dc, &r, fallback); DeleteObject(fallback); }
            }
            if (src) DeleteDC(src);
            // Map client-coordinate drawing into the smaller buffer: panel
            // coordinates stay absolute and GDI clips them to the bitmap.
            SetWindowOrgEx(dc, dirty.left, dirty.top, nullptr);
        }
        // Single source of truth for every coordinate below; the panel is
        // sized from the real number of programs instead of assuming three.
        DialogLayout L = ComputeLayout(cr.right, cr.bottom, g_monitorClient, g_openPrograms.size());
        ClampListScroll(L);

        SetBkMode(dc, TRANSPARENT);

        HFONT title = CreateSystemFont(DpiScale(27));
        SetTextColor(dc, g_skin->titleText);
        HGDIOBJ oldFont = SelectObject(dc, title);
        // Like Windows 7, the heading reports the total number of programs
        // still open -- never just the number of rows that fit on screen.
        const unsigned total = (unsigned)g_totalPrograms;
        wchar_t titleText[300]{};
        swprintf_s(titleText, L"%u %s", total, GetCountSuffix(total));
        RECT titleR = L.title;
        // Windows 7 left-aligns the heading against the list; Vista centres
        // it over the panel, as in the reference screen.
        DrawTextW(dc, titleText, -1, &titleR,
                  (g_skin->centreText ? DT_CENTER : DT_LEFT) | DT_SINGLELINE);
        SelectObject(dc, oldFont); DeleteObject(title);

        HFONT itemFont=CreateSystemFont(L.nameFont);
        HFONT noteFont=CreateSystemFont(L.noteFont);
        HFONT blockFont=CreateSystemFont(L.noteFont, FW_SEMIBOLD);
        if (itemFont) {
            HGDIOBJ oldItem=SelectObject(dc,itemFont);
            // One row per program: icon (32px, or 24px once the list goes
            // compact), the program name, and a smaller note underneath
            // explaining why it's blocking, matching the reference UI.
            // The list is not capped: it shows as many rows as the screen
            // allows and scrolls through the rest.
            const int rowH = L.rowH;
            int first = g_listScroll;
            int last  = first + L.visibleRows;
            if (last > (int)g_openPrograms.size()) last = (int)g_openPrograms.size();
            for (int i = first; i < last; ++i) {
                const int rowTop = L.rowsTop + (i - first) * rowH;
                // Very faint alternating band, like the authui program list.
                if (g_skin->rowBandAlpha > 0 && ((i - first) & 1) == 0) {
                    HDC bandDc = CreateCompatibleDC(dc);
                    const int bw = L.contentRight - L.contentLeft + 8, bh = rowH;
                    HBITMAP bandBmp = (bandDc && bw > 0) ? CreateCompatibleBitmap(dc, bw, bh) : nullptr;
                    if (bandDc && bandBmp) {
                        HGDIOBJ oldBand = SelectObject(bandDc, bandBmp);
                        HBRUSH bb = CreateSolidBrush(RGB(255,255,255));
                        RECT lr{0,0,bw,bh};
                        if (bb) { FillRect(bandDc,&lr,bb); DeleteObject(bb); }
                        BLENDFUNCTION bf{AC_SRC_OVER,0,g_skin->rowBandAlpha,0};
                        AlphaBlend(dc, L.contentLeft - DpiScale(4), rowTop - DpiScale(2),
                                   bw, bh, bandDc, 0, 0, bw, bh, bf);
                        SelectObject(bandDc, oldBand); DeleteObject(bandBmp);
                    }
                    if (bandDc) DeleteDC(bandDc);
                }
                const int iconSz = L.iconSize;
                if (g_openPrograms[i].icon)
                    DrawIconEx(dc, L.contentLeft, rowTop + DpiScale(2), g_openPrograms[i].icon,
                               iconSz, iconSz, 0, nullptr, DI_NORMAL);
                const int textLeft = L.contentLeft + iconSz + DpiScale(12);
                const int nameH = L.compact ? DpiScale(19) : DpiScale(24);
                SetTextColor(dc, g_skin->nameText);
                SelectObject(dc, itemFont);
                RECT nameR{textLeft, rowTop, L.contentRight, rowTop+nameH};
                DrawTextW(dc, g_openPrograms[i].name.c_str(), -1, &nameR, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
                // A program that stopped responding is what actually holds
                // the shutdown back, so it gets its own emphasised note.
                const bool blocking = g_openPrograms[i].blocking;
                HFONT rowNote = blocking ? blockFont : noteFont;
                if (rowNote) {
                    SetTextColor(dc, blocking ? g_skin->blockingNoteText : g_skin->noteText);
                    SelectObject(dc, rowNote);
                    RECT noteR{textLeft, rowTop + nameH, L.contentRight,
                              rowTop + nameH + (L.compact ? DpiScale(17) : DpiScale(22))};
                    DrawTextW(dc, blocking ? GetNotRespondingNote() : GetBlockedNote(),
                              -1, &noteR, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
                }
            }
            // Vertical scrollbar: shown only when some programs are off-list,
            // so all of them stay reachable instead of being hidden.
            if (L.hiddenRows > 0) {
                HBRUSH track = CreateSolidBrush(g_skin->trackFill);
                if (track) { RECT tr=L.track; FillRect(dc,&tr,track); DeleteObject(track); }
                HPEN tp = CreatePen(PS_SOLID, 1, g_skin->trackBorder);
                if (tp) {
                    HGDIOBJ op=SelectObject(dc,tp), ob=SelectObject(dc,GetStockObject(NULL_BRUSH));
                    Rectangle(dc, L.track.left, L.track.top, L.track.right, L.track.bottom);
                    SelectObject(dc,op); SelectObject(dc,ob); DeleteObject(tp);
                }
                // Glossy thumb, in the same Aero palette as the buttons.
                const int th = L.thumb.bottom - L.thumb.top;
                for (int y = L.thumb.top; y < L.thumb.bottom; ++y) {
                    const int t = th > 1 ? (y - L.thumb.top) * 255 / (th - 1) : 0;
                    COLORREF c = t < 128
                        ? LerpColor(g_skin->thumbTop, g_skin->thumbMid, t*2)
                        : LerpColor(g_skin->thumbMid, g_skin->thumbBot, (t-128)*2);
                    HBRUSH b = CreateSolidBrush(c);
                    if (b) { RECT row{L.thumb.left, y, L.thumb.right, y+1}; FillRect(dc,&row,b); DeleteObject(b); }
                }
                HPEN bp = CreatePen(PS_SOLID, 1, g_skin->thumbBorder);
                if (bp) {
                    HGDIOBJ op=SelectObject(dc,bp), ob=SelectObject(dc,GetStockObject(NULL_BRUSH));
                    RoundRect(dc, L.thumb.left, L.thumb.top, L.thumb.right, L.thumb.bottom,
                              DpiScale(4), DpiScale(4));
                    SelectObject(dc,op); SelectObject(dc,ob); DeleteObject(bp);
                }
            }
            SelectObject(dc,oldItem); DeleteObject(itemFont);
        }
        if (blockFont) DeleteObject(blockFont);
        if (noteFont) DeleteObject(noteFont);

        SetTextColor(dc, RGB(255,255,255));
        HPEN line = CreatePen(PS_SOLID, 1, g_skin->separator); oldFont = SelectObject(dc,line);
        MoveToEx(dc, L.contentLeft, L.listTop, nullptr);    LineTo(dc, L.contentRight, L.listTop);
        MoveToEx(dc, L.contentLeft, L.listBottom, nullptr); LineTo(dc, L.contentRight, L.listBottom);
        SelectObject(dc,oldFont); DeleteObject(line);

        HFONT body = CreateSystemFont(DpiScale(20));
        SetTextColor(dc, g_skin->titleText);
        oldFont=SelectObject(dc,body); RECT text = L.body;
        DrawTextW(dc,GetActionBody(),-1,&text,
                  (g_skin->centreText ? DT_CENTER : DT_LEFT) | DT_WORDBREAK);
        SelectObject(dc,oldFont); DeleteObject(body);
        // Buttons are ~4% flatter than a plain square-ish rect, matching the
        // slightly squashed proportions of the real Windows 7 dialog buttons.
        DrawButton(dc, L.force,  GetActionText(),      g_hoverForce,  /*isConfirm=*/true);
        DrawButton(dc, L.cancel, GetUiText()->cancel, g_hoverCancel, /*isConfirm=*/false);
        if (backBmp) {
            // Undo the client-coordinate mapping installed above before the
            // final blit. BitBlt takes its *source* corner in logical units,
            // so while the window origin is still (dirty.left, dirty.top) a
            // source of (0,0) resolves to device pixel (-dirty.left,
            // -dirty.top) -- outside the buffer. On a full-window repaint the
            // origin is (0,0) and it happened to work, but every partial
            // repaint (i.e. exactly the button rects invalidated by hot
            // tracking) blitted the wrong pixels, so the hover state never
            // reached the screen.
            SetWindowOrgEx(dc, 0, 0, nullptr);
            BitBlt(screenDc, dirty.left, dirty.top, dw, dh, dc, 0, 0, SRCCOPY);
            SelectObject(dc, oldBackBmp);
            DeleteObject(backBmp);
        }
        if (dc != screenDc) DeleteDC(dc);
        EndPaint(hwnd,&ps); return 0;
    }

    // No PostQuitMessage here: the dialog is modeless on the UI thread,
    // which also owns the hotkey window and keeps pumping after the screen
    // closes. Report the outcome by signalling the reply event a real hook
    // caller is waiting on; for a preview null was passed, so nothing is
    // signalled (and nothing needs closing).
    case WM_DESTROY: {
        KillTimer(hwnd, 1);
        g_dialog = nullptr;
        FreeDesktopBitmap();
        FreeBackdrop();
        g_openPrograms.clear();
        g_hoverForce = g_hoverCancel = false;
        // The outcome (force/proceed) was already written into *req by
        // whichever handler triggered this DestroyWindow; signalling its own
        // reply event last is what lets that outcome be read back safely by
        // the one hook thread that owns this specific request.
        auto* req = reinterpret_cast<ShutdownRequest*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (req && req->reply) SetEvent(req->reply);
        // Any request that arrived while this screen was up follows its
        // outcome exactly -- same force/proceed the visible screen just
        // decided (whichever handler set them into *req above, including the
        // "0 programs" and failed-CreateWindow fail-open paths, which never
        // reach WM_DESTROY and so never populate g_waiters in the first
        // place; only a request that actually got a screen on-screen does).
        for (auto* w : g_waiters) {
            if (req) { w->force = req->force; w->proceed = req->proceed; }
            else     { w->force = false;      w->proceed = true; }  // preview: nothing to decide, proceed
            if (w->reply) SetEvent(w->reply);
        }
        g_waiters.clear();
        return 0;
    }
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

// Applies (or clears) the preview hotkey on the thread that owns the window.
static void ApplyHotkey() {
    if (!g_isExplorer || !g_hotkeyWindow) return;
    UnregisterHotKey(g_hotkeyWindow, kHotkeyId);   // harmless if not registered
    if (g_hotkeyVk == 0) return;                   // "None": preview disabled
    if (!RegisterHotKey(g_hotkeyWindow, kHotkeyId, g_hotkeyMods, g_hotkeyVk)) {
        const DWORD err = GetLastError();
        if (err == ERROR_HOTKEY_ALREADY_REGISTERED)
            Wh_Log(L"The preview shortcut is already taken by another "
                   L"program; choose a different one in the settings");
        else
            Wh_Log(L"RegisterHotKey failed (error %u)", err);
        return;
    }
    Wh_Log(L"Preview shortcut registered (mods 0x%X, vk 0x%X)", g_hotkeyMods, g_hotkeyVk);
}

// Registers a window class for the mod, retrying once after unregistering a
// stale class left behind by an earlier mod image (its WndProc pointer is
// dangling, so the class must never be reused).
static ATOM RegisterModClass(WNDCLASSW* wc) {
    ATOM atom = RegisterClassW(wc);
    if (!atom) {
        UnregisterClassW(wc->lpszClassName, wc->hInstance);
        atom = RegisterClassW(wc);
    }
    return atom;
}

static LRESULT CALLBACK ControlProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_APP_SHOW: {
        // A shutdown hook is asking for the screen. The action (logoff/
        // shutdown/restart) is carried in wParam so it is decided by the hook
        // thread and never read by this thread while another request changes it.
        // lParam is that hook thread's own ShutdownRequest*, so writing the
        // outcome here can never be clobbered by any other request.
        auto* req = reinterpret_cast<ShutdownRequest*>(lp);
        if (g_dialog) {
            // A screen is already up. Do not pre-approve this second request
            // -- that would let it start the real shutdown while the first
            // user's screen is still on display and undecided, making their
            // subsequent Cancel do nothing. Instead park it: WM_DESTROY
            // gives it the same outcome (force/proceed) as the request whose
            // screen is actually showing, once that one is decided.
            g_waiters.push_back(req);
            return 0;
        }
        try {
            ShowScreenOnUiThread(req, (ActionKind)wp);
        } catch (...) {
            Wh_Log(L"Logoff screen failed: exception contained");
            req->force = false; req->proceed = true;
            g_dialog = nullptr; FreeDesktopBitmap(); g_openPrograms.clear();
            SetEvent(req->reply);
        }
        return 0;
    }
    case WM_HOTKEY:
        if (wp == kHotkeyId && !g_dialog && g_enabled) {
            // Preview only: pass nullptr for the request, so WM_DESTROY finds
            // nothing to signal (nothing is waiting on it). With no program
            // open the screen skips itself like a real logoff.
            try {
                ShowScreenOnUiThread(nullptr, kActionLogoff);
                if (g_totalPrograms == 0)
                    Wh_Log(L"Preview skipped: no program is currently open");
            } catch (...) {
                // If a dialog made it into existence before the throw, close
                // it; nothing is waiting on it.
                Wh_Log(L"Preview failed: exception contained");
                if (g_dialog) DestroyWindow(g_dialog);
            }
        }
        return 0;
    case WM_APP_APPLYSETTINGS:
        LoadSkinSetting();
        ApplyHotkey();   // re-registration picks up the changed shortcut
        if (g_dialog) {
            if (!g_enabled) {
                // Turned off while the screen was up: dismiss it and let the
                // pending action continue unforced, as if never intercepted.
                if (auto* req = reinterpret_cast<ShutdownRequest*>(GetWindowLongPtrW(g_dialog, GWLP_USERDATA)))
                    { req->force = false; req->proceed = true; }
                DestroyWindow(g_dialog);
            } else {
                RebuildBackdrop();   // re-bake the veil if the skin changed
                InvalidateRect(g_dialog, nullptr, TRUE);
            }
        }
        return 0;
    // WM_APP_QUITUI used to be handled here as a window message posted to
    // g_hotkeyWindow. It is now sent as a *thread* message (PostThreadMessageW
    // against the thread id, which is valid the instant CreateThread returns
    // -- see Wh_ModBeforeUninit/Wh_ModUninit), because g_hotkeyWindow may
    // never get published (StartUiThread's 5 s wait can time out, or window
    // creation can simply fail) while the thread keeps running; waiting on a
    // handle that might never exist was leaving the host hung forever on
    // unload. GetMessageW still returns thread messages (with msg.hwnd ==
    // NULL), so UiThreadProc's own loop intercepts it directly -- see
    // DrainOnQuit below.
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// Shared by UiThreadProc's message loop (the normal path) and its post-loop
// cleanup (the belt-and-suspenders re-drain -- see UiThreadProc). Closes any
// on-screen dialog first (on this thread, so DestroyWindow is legal) so a
// hook blocked on it wakes up, then drains and answers any WM_APP_SHOW still
// queued behind it -- otherwise a request that arrived in the same instant
// would be discarded along with the window, and its hook thread would block
// on `reply` forever (see the bounded wait in ShowWin7LogoffDialog for the
// other half of that fix). Both leave proceed TRUE: the mod must never
// silently cancel a real action it only intercepted, matching the "disable
// the mod while the screen is up" path in WM_APP_APPLYSETTINGS.
static void DrainOnQuit() {
    if (g_dialog) {
        if (auto* req = reinterpret_cast<ShutdownRequest*>(GetWindowLongPtrW(g_dialog, GWLP_USERDATA)))
            { req->force = false; req->proceed = true; }
        DestroyWindow(g_dialog);
    }
    MSG leftover;
    while (PeekMessageW(&leftover, nullptr, WM_APP_SHOW, WM_APP_SHOW, PM_REMOVE)) {
        if (auto* req = reinterpret_cast<ShutdownRequest*>(leftover.lParam)) {
            req->force = false; req->proceed = true;
            if (req->reply) SetEvent(req->reply);
        }
    }
}

static DWORD WINAPI UiThreadProc(LPVOID) {
    HINSTANCE hMod = GetModModuleHandle();

    // Force this thread's message queue to exist before anything else runs,
    // so PostThreadMessageW(g_uiThreadId, ...) from another thread (Wh_ModInit
    // returns before this line could plausibly run, but Wh_ModBeforeUninit/
    // Wh_ModUninit could race it) can never fail with ERROR_INVALID_THREAD_ID
    // for lack of a queue. Harmless no-op peek: there is nothing in the queue
    // yet and PM_NOREMOVE leaves it that way.
    MSG primeQueue;
    PeekMessageW(&primeQueue, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    auto cleanup = [&]() {
        if (g_hotkeyWindow) { UnregisterHotKey(g_hotkeyWindow, kHotkeyId);
                              DestroyWindow(g_hotkeyWindow); g_hotkeyWindow = nullptr; }
        UnregisterClassW(kCtrlClassName, hMod);
        UnregisterClassW(kClassName, hMod);
        ClearIconCache();
        FreeDesktopBitmap();
        FreeBackdrop();
        g_openPrograms.clear();
        g_uiThreadId = 0;
        if (g_uiThreadDone) SetEvent(g_uiThreadDone);
        if (g_uiReady) SetEvent(g_uiReady);
    };

    try {
        WNDCLASSW ctrl{};
        ctrl.hInstance = hMod;
        ctrl.lpfnWndProc = ControlProc;
        ctrl.lpszClassName = kCtrlClassName;
        if (!RegisterModClass(&ctrl)) { Wh_Log(L"Control window class could not be registered"); cleanup(); return 1; }

        WNDCLASSW dlg{};
        dlg.hInstance = hMod;
        dlg.lpfnWndProc = DialogProc;
        dlg.lpszClassName = kClassName;
        dlg.hCursor = LoadCursor(nullptr, IDC_ARROW);
        dlg.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        if (!RegisterModClass(&dlg)) { Wh_Log(L"Dialog window class could not be registered"); cleanup(); return 1; }

        g_hotkeyWindow = CreateWindowExW(0, kCtrlClassName, L"", 0, 0, 0, 0, 0,
                                         HWND_MESSAGE, nullptr, hMod, nullptr);
        if (!g_hotkeyWindow) { Wh_Log(L"Control window could not be created"); cleanup(); return 1; }

        // The hotkey only makes sense in the process that owns the desktop;
        // registering it in every injected shell host would make all but the
        // first registration fail and log spurious errors.
        ApplyHotkey();

        SetEvent(g_uiReady);   // init may now post requests

        MSG m{};
        while (GetMessageW(&m, nullptr, 0, 0) > 0) {
            // Thread message (PostThreadMessageW), not a window message --
            // msg.hwnd is NULL for these, so DispatchMessageW would not have
            // delivered it to ControlProc anyway. This is the authoritative
            // unload signal from Wh_ModBeforeUninit/Wh_ModUninit; see the
            // comment on WM_APP_QUITUI in ControlProc for why it moved here.
            if (!m.hwnd && m.message == WM_APP_QUITUI) {
                DrainOnQuit();
                break;
            }
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }

        // Belt-and-suspenders re-drain: a WM_APP_SHOW posted after the
        // WM_APP_QUITUI above was dequeued but before this thread actually
        // gets here would otherwise sit unanswered forever (DrainOnQuit's own
        // drain already ran), leaving its hook thread parked for the full
        // 65 s bound in ShowWin7LogoffDialog -- and Wh_ModUninit blocked on
        // g_hooksIdle for that whole time.
        if (g_dialog) DestroyWindow(g_dialog);
        MSG leftover;
        while (PeekMessageW(&leftover, nullptr, WM_APP_SHOW, WM_APP_SHOW, PM_REMOVE)) {
            if (auto* req = reinterpret_cast<ShutdownRequest*>(leftover.lParam)) {
                req->force = false; req->proceed = true;
                if (req->reply) SetEvent(req->reply);
            }
        }
        if (g_hotkeyWindow) UnregisterHotKey(g_hotkeyWindow, kHotkeyId);
        if (g_hotkeyWindow) { DestroyWindow(g_hotkeyWindow); g_hotkeyWindow = nullptr; }
        UnregisterClassW(kCtrlClassName, hMod);
        UnregisterClassW(kClassName, hMod);
    } catch (...) {
        Wh_Log(L"UI thread exception");
    }
    cleanup();
    return 0;
}

// Hook-side entry: hand the request to the UI thread and block until the
// screen reports its decision. Returns true when Windows should proceed.
// Forward declaration: used below (lazy-start on the first request from a
// non-explorer host) but defined further down, next to Wh_ModInit's own
// (eager, explorer-only) call.
static bool StartUiThread();

// Deliberately synchronous, not post-and-return: ExitWindowsEx/
// InitiateShutdownW's return value is exactly what tells the caller (the
// shell's own shutdown state machine) whether to proceed with tearing itself
// down. Returning immediately would report "started" before the user has
// even chosen an option, which would let the caller start closing windows,
// killing the process, etc. while the screen is still up and undecided --
// trading the current re-entrancy risk for a worse one. Keeping this
// synchronous is what lets a plain `return TRUE/FALSE` here still mean what
// the caller expects it to mean.
static bool ShowWin7LogoffDialog(UINT flags, DWORD reason, bool* outForce) {
    *outForce = false;
    if (g_insideHook) return true;
    // Lazy start for every host except explorer.exe, which already started
    // the thread eagerly in Wh_ModInit for the preview hotkey. Everywhere
    // else this fires at most once per process lifetime (the first real
    // shutdown/logoff attempt), so there is no reason to keep a thread and
    // two window classes resident from startup for a feature that may never
    // trigger in that process. StartUiThread()'s own `if (g_uiThread) return
    // true;` guard makes calling it here on every request harmless.
    if (!g_uiThreadId && !StartUiThread()) return true;   // fail open
    if (!g_uiThreadId || !g_hotkeyWindow) return true;

    // Heap-allocated, not stack: on the (last-resort) timeout/WM_QUIT paths
    // below, this function gives up on waiting while the UI thread may still
    // hold this pointer (e.g. in a dialog's GWLP_USERDATA) and could still
    // write to it or SetEvent() its reply later. A stack object would make
    // that a use-after-free/invalid-handle; leaking this one small object in
    // that (should-never-happen) case is the safe trade. The normal path
    // frees it itself once it is provably done being touched.
    auto* req = new (std::nothrow) ShutdownRequest();
    if (!req) return true;   // fail open: never block a logoff
    req->reply = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!req->reply) { delete req; return true; }   // fail open: never block a logoff

    // Which wording the screen uses is decided here, once. EWX_REBOOT is
    // tested first because a restart also carries the shutdown semantics, and
    // EWX_POWEROFF is treated as a shutdown because "Shut down" from the Start
    // menu passes it rather than a bare EWX_SHUTDOWN -- without it the screen
    // would offer to "force log off" while the machine is powering off.
    req->action = (flags & EWX_REBOOT)                     ? kActionRestart
                : (flags & (EWX_SHUTDOWN | EWX_POWEROFF))  ? kActionShutdown
                : kActionLogoff;

    if (!PostMessageW(g_hotkeyWindow, WM_APP_SHOW, (WPARAM)req->action,
                      reinterpret_cast<LPARAM>(req))) {
        CloseHandle(req->reply);
        delete req;
        return true;
    }

    // Wait for the screen to close. The UI thread always signals the event
    // (every ShowScreenOnUiThread path, WM_DESTROY, and the exception path),
    // and Wh_ModBeforeUninit/WM_APP_QUITUI tears the dialog down (or, if it
    // arrived too late for that, drains and answers the still-queued
    // WM_APP_SHOW directly) before the UI thread's loop exits -- so this
    // should never actually hit its bound. The bound exists only as a last
    // resort against some other, unforeseen way the reply could go
    // unsignalled: without it, this thread (the shell's own) would block
    // forever inside ExitWindowsEx/InitiateShutdownW, and Wh_ModUninit would
    // in turn block forever on g_hooksIdle, hanging the shell on unload.
    // 65 s clears the screen's own 60 s watchdog with room to spare.
    constexpr DWORD kWaitBoundMs = 65000;
    const ULONGLONG deadline = GetTickCount64() + kWaitBoundMs;
    bool proceed;
    bool gaveUp = false;
    for (;;) {
        ULONGLONG now = GetTickCount64();
        DWORD remaining = (now >= deadline) ? 0 : (DWORD)(deadline - now);
        DWORD wait = MsgWaitForMultipleObjects(1, &req->reply, FALSE, remaining, QS_SENDMESSAGE);
        if (wait == WAIT_TIMEOUT) {
            Wh_Log(L"Timed out waiting for the logoff screen to close; proceeding unforced");
            proceed = true; gaveUp = true;   // fail open
            break;
        }
        if (WaitForSingleObject(req->reply, 0) == WAIT_OBJECT_0) { proceed = req->proceed; *outForce = req->force; break; }
        // This is the shell's (or Task Manager's, etc.) own UI thread, deep
        // inside ExitWindowsEx/InitiateShutdownW, re-entrantly waiting on the
        // mod's *own* UI thread to answer. It must keep answering messages
        // *sent* to it (e.g. DWM's cross-thread SendMessage queries) so those
        // callers can't deadlock -- that's all PM_QS_SENDMESSAGE dispatches
        // here. Everything else (posted input, timers, WM_COMMANDs, and any
        // WM_QUIT belonging to this thread's own real loop) is deliberately
        // left on the queue untouched: previously this pumped and dispatched
        // the *entire* posted-message queue with PM_REMOVE, which ran
        // arbitrary shell code re-entrantly, on the caller's own thread, in
        // the middle of its shutdown sequence, for up to the full 65 s bound.
        // A WM_QUIT sitting in the queue needs no special handling either --
        // left alone here, it is simply the first thing the thread's real
        // loop sees once this function returns.
        MSG m{};
        PeekMessageW(&m, nullptr, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
    }

    if (!gaveUp) { CloseHandle(req->reply); delete req; }
    return proceed;
}

static BOOL WINAPI ExitWindowsEx_Hook(UINT flags, DWORD reason) {
    InFlightHook guard;
    try {
        if (!ExitWindowsEx_Original) return FALSE;
        if (g_insideHook) return ExitWindowsEx_Original(flags, reason);
        bool force = false;
        if (!ShowWin7LogoffDialog(flags, reason, &force)) return TRUE;
        g_insideHook=true;
        BOOL result=ExitWindowsEx_Original(flags | (force ? EWX_FORCEIFHUNG : 0), reason);
        g_insideHook=false; return result;
    } catch (...) {
        g_insideHook=false;
        return ExitWindowsEx_Original ? ExitWindowsEx_Original(flags, reason) : FALSE;
    }
}

// Second entry point: this is the one the Start menu power button actually
// uses on Windows 10/11 for Shut down and Restart, which is why hooking only
// ExitWindowsEx made the screen appear for sign-out but not for shutdown.
// The shutdown-type flags are different from the EWX_* ones, so they are
// translated to the equivalent EWX_* set for the dialog's own logic.
static DWORD WINAPI InitiateShutdownW_Hook(LPWSTR machineName, LPWSTR message,
                                           DWORD gracePeriod, DWORD shutdownFlags,
                                           DWORD reason) {
    InFlightHook guard;
    try {
        if (!InitiateShutdownW_Original) return ERROR_PROC_NOT_FOUND;
        // Only local, interactive requests get the screen. A remote shutdown or a
        // re-entrant call must pass straight through.
        if (g_insideHook || (machineName && *machineName))
            return InitiateShutdownW_Original(machineName, message, gracePeriod, shutdownFlags, reason);

        UINT ewx = (shutdownFlags & SHUTDOWN_RESTART) ? EWX_REBOOT
                 : (shutdownFlags & SHUTDOWN_POWEROFF) ? EWX_POWEROFF
                 : EWX_SHUTDOWN;
        bool force = false;
        if (!ShowWin7LogoffDialog(ewx, reason, &force)) return ERROR_SUCCESS; // cancelled by the user

        g_insideHook=true;
        // Deliberately no SHUTDOWN_FORCE_* flags. Those are the equivalent of
        // EWX_FORCE (they terminate applications without letting them save),
        // whereas the ExitWindowsEx path uses only EWX_FORCEIFHUNG. Matching
        // that keeps "Force" the same conservative action whichever shell
        // surface started the shutdown: an app that vetoes WM_QUERYENDSESSION
        // is left to Windows' own "app is preventing shutdown" screen, and
        // unsaved work is never destroyed behind the user's back.
        DWORD result=InitiateShutdownW_Original(machineName, message, gracePeriod,
                                                shutdownFlags, reason);
        g_insideHook=false; return result;
    } catch (...) {
        g_insideHook=false;
        return InitiateShutdownW_Original
            ? InitiateShutdownW_Original(machineName, message, gracePeriod, shutdownFlags, reason)
            : ERROR_PROC_NOT_FOUND;
    }
}

// Brings the UI thread up. Returns false if it could not be started.
//
// g_uiReady/g_uiThreadDone/g_hooksIdle are created unconditionally in
// Wh_ModInit, before any hook is installed, so they always exist by the time
// this (or a hook) runs -- this function only ever starts the thread itself.
//
// Guarded by std::call_once rather than the old "if (g_uiThread) return
// true;" check: two shell threads racing into ShowWin7LogoffDialog on a
// non-explorer host could both pass a plain pointer check before either had
// written g_uiThread, each creating its own thread and pair of window
// classes. call_once makes exactly one caller actually run
// StartUiThreadOnce; everyone else blocks until it finishes and then observes
// its result.
static std::once_flag g_uiThreadOnce;
static bool g_uiThreadStartOk = false;

static void StartUiThreadOnce() {
    DWORD tid = 0;
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &tid);
    if (!g_uiThread) {
        Wh_Log(L"CreateThread for the UI thread failed");
        g_uiThreadStartOk = false;
        return;
    }
    g_uiThreadId.store(tid, std::memory_order_release);
    // Do not post anything to the control window before the thread has
    // actually created it.
    WaitForSingleObject(g_uiReady, 5000);
    g_uiThreadStartOk = (g_hotkeyWindow.load(std::memory_order_acquire) != nullptr);
}

static bool StartUiThread() {
    std::call_once(g_uiThreadOnce, StartUiThreadOnce);
    return g_uiThreadStartOk;
}

BOOL Wh_ModInit() {
    wchar_t exePath[MAX_PATH]{};
    if (GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath))) {
        const wchar_t* exe = wcsrchr(exePath, L'\\');
        exe = exe ? exe + 1 : exePath;
        g_isExplorer = (_wcsicmp(exe, L"explorer.exe") == 0);
    }

    LoadSkinSetting();

    // Created here, before any hook is installed, so the in-flight-hook
    // guard and the unload waits are always meaningful: InFlightHook's
    // constructor can run the instant SetFunctionHook below returns, on any
    // host (not just explorer.exe, which used to be the only one where the
    // UI thread -- and therefore these events -- existed this early). If
    // g_hooksIdle didn't exist yet, ResetEvent(nullptr) would silently no-op
    // and Wh_ModUninit's later WaitForSingleObject(g_hooksIdle, INFINITE)
    // would return immediately while a hook was still executing.
    g_uiReady      = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiThreadDone = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hooksIdle    = CreateEventW(nullptr, TRUE, TRUE, nullptr);
    if (!g_uiReady || !g_uiThreadDone || !g_hooksIdle) {
        Wh_Log(L"Failed to create synchronization events");
        if (g_uiReady)      CloseHandle(g_uiReady);
        if (g_uiThreadDone) CloseHandle(g_uiThreadDone);
        if (g_hooksIdle)    CloseHandle(g_hooksIdle);
        g_uiReady = g_uiThreadDone = g_hooksIdle = nullptr;
        return FALSE;
    }

    // Both entry points are hooked. Which one a given shell surface uses
    // varies with the Windows version and with the action (sign out vs shut
    // down vs restart), so requiring both to be present would make the mod
    // fail on systems where only one exists: succeeding if *either* hook was
    // installed is what makes the screen appear consistently.
    bool anyHook = false;

    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        if (auto target=(ExitWindowsEx_t)(void*)GetProcAddress(user32,"ExitWindowsEx")) {
            if (WindhawkUtils::SetFunctionHook(target, ExitWindowsEx_Hook, &ExitWindowsEx_Original))
                anyHook = true;
            else Wh_Log(L"Hooking ExitWindowsEx failed");
        } else Wh_Log(L"ExitWindowsEx not found");
    }

    // advapi32 is not always loaded yet in the shell host processes, so it is
    // loaded explicitly rather than merely looked up. The reference is
    // intentionally never released: the hook must outlive this function.
    if (HMODULE advapi32 = LoadLibraryW(L"advapi32.dll")) {
        if (auto target=(InitiateShutdownW_t)(void*)GetProcAddress(advapi32,"InitiateShutdownW")) {
            if (WindhawkUtils::SetFunctionHook(target, InitiateShutdownW_Hook, &InitiateShutdownW_Original))
                anyHook = true;
            else Wh_Log(L"Hooking InitiateShutdownW failed");
        } else Wh_Log(L"InitiateShutdownW not found");
    }

    if (!anyHook) {
        Wh_Log(L"No shutdown entry point could be hooked");
        // No hook is live and Windhawk will not call Wh_ModUninit after a
        // FALSE return, so these must be released here or they leak.
        CloseHandle(g_uiReady);      g_uiReady = nullptr;
        CloseHandle(g_uiThreadDone); g_uiThreadDone = nullptr;
        CloseHandle(g_hooksIdle);    g_hooksIdle = nullptr;
        return FALSE;
    }

    // Only start the UI thread itself once at least one hook is live (the
    // synchronization events it uses already exist from above). Starting the
    // thread before the hooks meant the !anyHook path above returned FALSE
    // (so Windhawk frees the image and never calls Wh_ModUninit) with the UI
    // thread still sitting in GetMessage -- a dangling-window-class crash.
    // Started here, a shutdown arriving before the thread is ready fails open
    // (ShowWin7LogoffDialog returns its callers' true), which is safe.
    //
    // Only explorer.exe needs the thread up front, for the preview hotkey
    // (g_isExplorer gates ApplyHotkey/WM_HOTKEY already). Every other host
    // only ever needs it for an actual shutdown/logoff attempt, which is rare
    // and often never happens in that process's lifetime at all -- so there
    // it is started lazily, from ShowWin7LogoffDialog on the first request,
    // instead of sitting in GetMessage with two registered window classes
    // for the whole session.
    if (g_isExplorer && !StartUiThread()) {
        Wh_Log(L"UI thread unavailable; the mod will stay out of the way");
    }

    return TRUE;
}

// Windhawk calls this when the user presses Save in the settings. Skin,
// language and hotkey are picked up on the UI thread; if the screen happens
// to be on display it is repainted (or, when the master switch was turned
// off, dismissed) on the spot, rather than only on the next shutdown.
void Wh_ModSettingsChanged() {
    if (g_hotkeyWindow)
        PostMessageW(g_hotkeyWindow, WM_APP_APPLYSETTINGS, 0, 0);
}

// Signals the UI thread to wind up, using its thread id (valid the instant
// CreateThread returned) rather than g_hotkeyWindow. The window handle is not
// a safe stop signal: StartUiThread gives up waiting for it after 5 s and
// returns false while leaving the thread running, and window creation can
// simply fail -- either way g_hotkeyWindow would stay null forever and
// nothing would ever be posted, hanging the host in the INFINITE join below.
// Waiting on g_uiReady first (only when the thread was actually started --
// otherwise nobody will ever signal it) guarantees the thread has reached its
// message loop and PostThreadMessageW cannot fail for lack of a queue (see
// the priming PeekMessageW at the top of UiThreadProc).
static void SignalUiThreadQuit() {
    if (!g_uiThread.load(std::memory_order_acquire)) return;   // never started (or never even attempted) -- nothing to signal
    WaitForSingleObject(g_uiReady, INFINITE);
    DWORD tid = g_uiThreadId.load(std::memory_order_acquire);
    if (tid) PostThreadMessageW(tid, WM_APP_QUITUI, 0, 0);
}

// Windhawk calls this while the hooks are still installed, before it removes
// them and unloads the mod. Tearing the screen down here unblocks a hook
// thread that is sitting in ShowWin7LogoffDialog, so disabling/updating the
// mod during a live logoff cannot stall the unload until the screen closes or
// the watchdog fires (Windhawk has to wait for the hook thread to leave the
// mod's code before it can free the image).
void Wh_ModBeforeUninit() {
    SignalUiThreadQuit();
}

void Wh_ModUninit() {
    // 1. Ask the UI thread to wind up: it closes the full-screen dialog on
    //    its own thread (DestroyWindow from any other thread fails with
    //    ERROR_ACCESS_DENIED and leaves the window alive with a WndProc about
    //    to be unmapped), unregisters the hotkey and both window classes, and
    //    frees the icon cache.
    SignalUiThreadQuit();

    // 2. Do not let any hook thread keep executing mod code after the DLL is
    //    freed. During a real logoff the screen being torn down wakes the
    //    hook and it proceeds immediately.
    WaitForSingleObject(g_hooksIdle, INFINITE);

    // 2b. A hook thread that was still in flight when step 1 ran may have
    //     called StartUiThreadOnce (and created g_uiThread) *after* that
    //     first SignalUiThreadQuit already saw g_uiThread == nullptr and gave
    //     up. g_hooksIdle is now signalled, so no hook can be in that window
    //     any more and g_uiThread (if it exists) has its final value -- signal
    //     once more so a UI thread created that late still gets its quit
    //     message instead of parking in GetMessageW forever.
    SignalUiThreadQuit();

    // 3. Join the UI thread INFINITE. A bounded wait that gives up and lets
    //    Windhawk FreeLibrary the image while the thread is still inside mod
    //    code (or blocked in GetMessage with a return address in it) crashes
    //    the host process.
    if (HANDLE uiThread = g_uiThread.load(std::memory_order_acquire)) {
        WaitForSingleObject(uiThread, INFINITE);
        CloseHandle(uiThread);
        g_uiThread = nullptr;
    }
    g_hotkeyWindow = nullptr;
    g_uiThreadId = 0;

    // 4. Only now, with every thread that could touch this state gone, is it
    //    safe to release it.
    g_dialog = nullptr;
    ClearIconCache();
    g_openPrograms.clear();
    g_listScroll = 0;
    g_totalPrograms = 0;
    g_draggingThumb = false;
    FreeDesktopBitmap();
    FreeBackdrop();

    if (g_uiReady)      { CloseHandle(g_uiReady);      g_uiReady = nullptr; }
    if (g_uiThreadDone) { CloseHandle(g_uiThreadDone); g_uiThreadDone = nullptr; }
    if (g_hooksIdle)    { CloseHandle(g_hooksIdle);    g_hooksIdle = nullptr; }
}
