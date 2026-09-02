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
// @include        shutdown.exe
// @architecture   x86-64
// @compilerOptions -luser32 -lgdi32 -lmsimg32 -lpsapi -lshell32 -ldwmapi -ladvapi32 -lcrypt32
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Windows 7 Logoff Screen Restorer

## Overview

This mod restores the classic full-screen logoff/shutdown experience from Windows 7 on Windows 10 and 11. The user will see the dimmed desktop, a heading showing how many programs are still running, a list of those programs with their icons, and the familiar "Force log off" and "Cancel" buttons.

It does **not** modify any system files (like winlogon.exe or LogonUI.exe). Instead, it shows a visual prompt before Windows proceeds with the actual shutdown. Once "Force log off" is clicked, Windows continues its normal shutdown sequence.

---

## Key Features

- **Accurate Windows 7 style list**: Shows the total number of open programs (no artificial cap) and displays as many as fit on screen, with a scrollbar for the rest.
- **Adaptive layout**: The panel resizes based on the number of programs. When space is tight, it uses smaller icons and rows to fit more entries before scrolling.
- **Priority to unresponsive programs**: Programs that are not responding are detected and moved to the top, with a clear "This program is not responding." note.
- **Live updates**: The list refreshes every second, so entries disappear and the counter updates as programs close on their own.
- **Works for logoff, shutdown, and restart**: Hooks into both `ExitWindowsEx` and `InitiateShutdownW`, covering the Start menu, Win+X, Alt+F4, and Task Manager.
- **Smart skipping**: If no programs are running, the screen is bypassed entirely. If the last program closes while the screen is visible, it dismisses itself automatically.
- **Full mouse and keyboard support**: Scroll with the wheel, drag the thumb, click the track, or use arrow keys, Page Up/Down, Home, and End.
- **Two visual skins**: The mods allows to choose between **Windows 7** (with blue Aero glass) and **Windows Vista** (with a red power button). Switch anytime via settings; changes apply instantly.
- **Action-aware wording**: All text (heading, notes, buttons) adapts to logoff, shutdown, or restart, in all 21 supported languages.
- **Built-in translations**: The interface is fully localised in 21 languages. The user can also manually select a preferred language, independent of the system locale.
- **Easy on/off toggle**: The mod can be disabled entirely via the **Enable the screen** setting, without uninstalling. If disabled while active, the screen closes and the action continues.
- **Clean filtering**: System processes (lock screen, Start menu, search, etc.) are never listed, and each program appears only once regardless of how many windows it has.
- **100% reversible**: Only two user-mode hooks are installed. Disabling the mod (via Windhawk or the internal setting) removes it completely and there are not registry or system changes.

The mod has been tested on Windows 10 21H2, Windows 11 24H2 and Windows 11 25H2.
---

## Preview Shortcut

The screen can be tested without logging off by pressing `Ctrl+Alt+Shift+L` (while the mod is enabled). The shortcut can be changed or disabled in settings. Note that `Win+L` is reserved by Windows and cannot be used.

---

## Requirements

- **Windows 10 or Windows 11** (64-bit)

---

## Safety & Design

- A 60-second watchdog ensures the screen never blocks a logoff.
- All operations (enumeration, painting, hooks) are wrapped to fall back safely to normal shutdown in case of errors.
- Conservative filters skip invisible, owned, tool, or system windows, and deduplicate entries by process ID.
- A safety limit prevents unbounded icon allocation; the total count remains accurate even if the list is truncated.
- The interface is drawn off-screen and blitted in one go, eliminating flicker and keeping hit-testing perfectly aligned.

---

## Known Limitations

- Only intercepts shutdown requests via `ExitWindowsEx` or `InitiateShutdownW` in shell processes. Direct `NtShutdownSystem` calls or service-initiated shutdowns are bypassed by design.
- If the Start menu button does not trigger the screen, ensure StartMenuExperienceHost.exe is not excluded in Windhawk's process list (note: portable mode may prevent injection).
- This is a visual prompt, not a full secure-desktop replacement (that would require patching Winlogon/LogonUI).
- A program with unsaved work but still responding is listed as normal; only hung windows are flagged as blocking.

---

## Credits 
- Cips - Testing on Windows 11 25H2

---

If any issues are encountered, please report them to the mod's author.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable the screen
  $description: >-
    This setting turns the screen on or off. This setting takes effect
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
    setting, on "Automatic", follows the Windows user locale and falls back
    to English if unsupported. This setting never changes the wording's match
    to the action taking place (log off, shut down or restart).
  $options:
  - auto: Automatic (follow the Windows language)
  - en: English
  - it: Italiano (Italian)
  - es: Espanol (Spanish)
  - fr: Francais (French)
  - de: Deutsch (German)
  - pt: Portugues (Portuguese)
  - nl: Nederlands (Dutch)
  - pl: Polski (Polish)
  - cs: Cestina (Czech)
  - sv: Svenska (Swedish)
  - da: Dansk (Danish)
  - fi: Suomi (Finnish)
  - nb: Norsk bokmal (Norwegian)
  - el: Ellinika (Greek)
  - tr: Turkce (Turkish)
  - ru: Russkiy (Russian)
  - uk: Ukrayinska (Ukrainian)
  - ar: al-Arabiyya (Arabic)
  - zh: Zhongwen (Chinese, simplified)
  - ja: Nihongo (Japanese)
  - ko: Hangugeo (Korean)
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
#include <wincrypt.h>
#include <string>

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
static bool g_insideHook = false;
static UINT g_pendingFlags = 0;
static DWORD g_pendingReason = 0;
static bool g_force = false;    // add EWX_FORCEIFHUNG to the real call
// Whether the logoff/shutdown must continue once the screen closes. This is
// deliberately separate from g_force: the screen can close by itself because
// the last program went away, and in that case Windows should carry on
// normally, without the "force" flag being added behind the user's back.
static bool g_proceed = false;
// Which of the three things Windows was asked to do. Everything the screen
// says is chosen from this, so the wording can never disagree with the
// action: a restart is a shutdown as far as the logic goes, but telling the
// user the machine is shutting down when it is coming straight back up would
// be misleading, so the three are kept apart.
enum ActionKind { kActionLogoff = 0, kActionShutdown = 1, kActionRestart = 2, kActionCount = 3 };
static ActionKind g_action = kActionLogoff;

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

// Read once per session, when the screen is about to be shown, so switching
// the setting in Windhawk takes effect on the next screen without a reload.
// Empty means "follow the Windows user locale". Anything else is one of the
// locale prefixes in kTexts, validated when it is used rather than here, so an
// unknown value degrades to the automatic behaviour instead of failing.
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

    // The shortcut is only re-read here; changing it takes effect after the
    // mod is reloaded, because the registration lives in the hotkey thread.
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
static bool g_hoverForce = false;
static bool g_hoverCancel = false;
static ULONGLONG g_dialogStart = 0;
static ULONGLONG g_captureFailedAt = 0;
static HWND g_hotkeyWindow = nullptr;
static HANDLE g_hotkeyThread = nullptr;
static DWORD g_hotkeyThreadId = 0;

// ---------------------------------------------------------------------------
// DPI scaling
//
// Every metric the painter uses (fonts, rows, icons, paddings, the 800 px
// panel, the button sizes, ...) is written in 96-DPI reference pixels, i.e.
// the size the UI has at 100% scaling. The shell processes this mod runs
// inside (explorer.exe, StartMenuExperienceHost.exe, ...) are DPI-aware, so
// at 125% or 150% scaling Windows gives them physical pixels and scales its
// own UI up by the same factor -- while hardcoded metrics would stay put and
// the whole screen would come out too small next to the rest of the shell.
// Every metric is therefore multiplied by the process's actual DPI (96, 120,
// 144, ...) through DpiScale() before it is used.
//
// GetDeviceCaps(LOGPIXELSX) on a screen DC is deliberately used instead of
// GetDpiForSystem(): it returns the DPI the process *actually renders at*.
// For a DPI-aware process that is the real scaling (120 at 125%); for a
// DPI-unaware process (e.g. shutdown.exe, whose windows Windows scales and
// stretches for it) it returns 96, so the metrics stay unscaled and DWM's
// own stretch factor does the rest -- the two must never be combined.
// ---------------------------------------------------------------------------
static int g_dpi = 96;   // process DPI (dots per inch); 96 == 100% scaling

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
static void LogCritical(const wchar_t* message) {
    wchar_t path[MAX_PATH]{};
    DWORD n = GetTempPathW(ARRAYSIZE(path), path);
    if (!n || n >= ARRAYSIZE(path) - 32) return;
    wcscat_s(path, L"Win7LogoffRestorer.log");
    HANDLE f = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                           OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    SYSTEMTIME t{}; GetLocalTime(&t);
    wchar_t line[512]{};
    swprintf_s(line, L"%04u-%02u-%02u %02u:%02u:%02u: %s\r\n",
               t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,message);
    DWORD written=0; WriteFile(f,line,(DWORD)(wcslen(line)*sizeof(wchar_t)),&written,nullptr);
    CloseHandle(f);
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

static HBITMAP CaptureDesktop() {
    HWND desktop = GetDesktopWindow();
    HDC src = GetDC(desktop);
    if (!src) return nullptr;
    RECT r{};
    GetWindowRect(desktop, &r);
    int w = r.right - r.left;
    int h = r.bottom - r.top;
    DcGuard mem(CreateCompatibleDC(src));
    BitmapGuard bmp(CreateCompatibleBitmap(src, w, h));
    if (mem.get() && bmp.get()) {
        HGDIOBJ old = SelectObject(mem.get(), bmp.get());
        BitBlt(mem.get(), 0, 0, w, h, src, 0, 0, SRCCOPY);
        SelectObject(mem.get(), old);
        HBITMAP result = bmp.release();
        ReleaseDC(desktop, src);
        return result;
    }
    ReleaseDC(desktop, src);
    return nullptr;
}

// Every string the screen shows depends on which action was actually
// requested: promising a "Force log off" while the machine is about to shut
// down, or saying a program "is preventing Windows from logging off" during a
// restart, would simply be wrong. So each action-dependent string exists in
// all three variants -- log off, shut down, restart -- for every language,
// indexed by ActionKind, and no locale silently falls back to English.
//
// cancel is action-neutral (the button cancels whatever was requested).
// stillOne/stillMany are the full suffix shown after the numeric count
// ("<N> <suffix>"), already agreeing in number (and, where the language
// needs it, in verb form) so no language ever shows a mismatched count
// like "3 programma" -- singular suffix is used only when count == 1. They
// are action-neutral too: the heading only counts programs, it does not name
// the action.
// notResponding is the line shown under a program that has stopped answering
// messages; it describes the program, not the action, so it is single-valued.
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
static void FreeProgramIcons() {
    for (auto& p : g_openPrograms) if (p.icon) DestroyIcon(p.icon);
}
static HICON CloneIcon(HICON icon, int cx, int cy) {
    if (!icon) return nullptr;
    return (HICON)CopyImage(icon, IMAGE_ICON, cx, cy, LR_COPYFROMRESOURCE | LR_DEFAULTSIZE);
}

static HICON GetProgramIcon(HWND window, const wchar_t* exePath) {
    HICON source = (HICON)SendMessageW(window, WM_GETICON, ICON_BIG, 0);
    if (!source) source = (HICON)SendMessageW(window, WM_GETICON, ICON_SMALL2, 0);
    if (!source) source = (HICON)SendMessageW(window, WM_GETICON, ICON_SMALL, 0);
    if (!source) source = (HICON)GetClassLongPtrW(window, GCLP_HICON);
    if (!source) source = (HICON)GetClassLongPtrW(window, GCLP_HICONSM);

    HICON copy = CloneIcon(source, 32, 32);
    if (copy) return copy;

    // More reliable fallback: extract the icon from the executable itself.
    if (exePath && *exePath) {
        HICON large = nullptr, small = nullptr;
        if (ExtractIconExW(exePath, 0, &large, &small, 1) > 0) {
            HICON result = CloneIcon(large ? large : small, 32, 32);
            if (large) DestroyIcon(large);
            if (small) DestroyIcon(small);
            if (result) return result;
        }
    }

    if (exePath && *exePath) {
        SHFILEINFOW fi{};
        if (SHGetFileInfoW(exePath, 0, &fi, sizeof(fi), SHGFI_ICON | SHGFI_LARGEICON) && fi.hIcon) {
            HICON result = CloneIcon(fi.hIcon, 32, 32);
            DestroyIcon(fi.hIcon);
            if (result) return result;
        }
    }
    // Last-resort generic Windows application icon.
    return CloneIcon(LoadIconW(nullptr, IDI_APPLICATION), 32, 32);
}

// Background/system host processes that own hidden or auxiliary top-level
// windows (input method hosts, the lock screen, the shell frame host,
// search, etc.). These are never something the user consciously "closes",
// so they must never show up in the shutdown list even if EnumWindows sees
// their window as visible.
static bool IsSystemHostProcess(const wchar_t* exeName) {
    static const wchar_t* const kHosts[] = {
        L"textinputhost.exe", L"lockapp.exe", L"applicationframehost.exe",
        L"shellexperiencehost.exe", L"startmenuexperiencehost.exe",
        L"searchhost.exe", L"searchui.exe", L"searchapp.exe",
        L"systemsettings.exe", L"sihost.exe", L"ctfmon.exe",
        L"dwm.exe", L"taskhostw.exe", L"explorer.exe",
        L"windowsinternal.composableshell.experiences.textinput.inputapp.exe",
    };
    for (const wchar_t* h : kHosts) if (_wcsicmp(exeName, h) == 0) return true;
    return false;
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
        if (!GetWindowTextW(w, title, ARRAYSIZE(title)) || !title[0]) return TRUE;
        // The Start menu is an Explorer-owned shell surface, not a program
        // that must be closed. Never show it in the shutdown list.
        if (_wcsicmp(title, L"Start menu") == 0 ||
            _wcsicmp(title, L"Menu Start") == 0 ||
            _wcsicmp(title, L"Start") == 0 ||
            _wcsicmp(title, L"Start menu host") == 0) return TRUE;
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
        // One line per process: a program with several top-level windows
        // (or several invisible worker windows) must not appear twice.
        for (const auto& p : g_openPrograms) if (p.pid == pid) return TRUE;
        std::wstring x=title;
        if (!exeName.empty()) x += L" (" + exeName + L")";
        // A window that no longer pumps messages is what actually holds the
        // shutdown back, so remember it: those entries are listed first.
        bool blocking = IsHungAppWindow(w) != FALSE;
        g_openPrograms.push_back({x, GetProgramIcon(w, path), pid, blocking});
    } catch (...) {}
    return TRUE;
}
static int RefreshOpenPrograms(){
 try {
  std::wstring before=ProgramListSignature();
  size_t beforeCount=g_totalPrograms;
  FreeProgramIcons(); g_openPrograms.clear();
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
  // and reachable by scrolling. Only the generous safety bound is enforced,
  // and the icons of the entries it drops are destroyed so no HICON leaks.
  if(g_openPrograms.size()>kMaxListedPrograms){
      for(size_t i=kMaxListedPrograms;i<g_openPrograms.size();++i)
          if(g_openPrograms[i].icon) DestroyIcon(g_openPrograms[i].icon);
      g_openPrograms.resize(kMaxListedPrograms);
  }
  if (g_totalPrograms!=beforeCount) return kListLayoutChanged;
  return before!=ProgramListSignature() ? kListContentChanged : kListUnchanged;
 } catch (...) { FreeProgramIcons(); g_openPrograms.clear(); g_totalPrograms=0; LogCritical(L"Program enumeration failed"); return kListLayoutChanged; }
}

// All four accessors are now a plain lookup: the table already holds the
// wording for the exact action in progress, so nothing has to be patched up
// at draw time and no string can contradict what is about to happen.
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
// It ships as two stacked 64x64 8-bit coverage masks (first the dark disc,
// then the white ring and bar) rather than being drawn with GDI arcs, because
// Arc() and Ellipse() do not antialias: at the ~17 px this is rendered at, a
// pen-drawn ring comes out visibly jagged and the bar inside it turns into a
// smudge. The masks were rasterised 8x oversampled and box filtered, so
// scaling them down stays smooth at any button height. This is also safer
// than a glyph font -- Segoe MDL2 Assets may be missing or metrically
// different across the Windows versions this mod supports.
//
// Encoding: base64 of a run-length stream of (value, count) byte pairs
// holding one coverage mask each.
//
// Two masks ship here, pixel-aligned with each other:
//
//   kPowerGlyphMaskRle  the ring and the bar, drawn in the label colour.
//   kPowerGlyphDiscRle  the ring's full interior, ring included. It is not
//                       painted by default; it exists only for the flat-fill
//                       option below, which floods everything inside the
//                       ring with one uniform tone so the button gradient
//                       cannot show a lighter top and a darker bottom
//                       through the middle of the symbol.
//
// With the flat fill off, the ring mask alone is composited and every pixel
// that is not ring or bar stays fully transparent, so the button's own red
// gradient shows through the middle of the symbol.
static const char* const kPowerGlyphMaskRle =
    "AFkEAgAKBAIALwQBAAUFAREBJAEnAiQBEQEFAQAFBAEAKQMBAAQsAWMBmQHCAd0B7QH8Af8C/AHtAd0BwgGZAWMBLAEABAMB"
    "ACQDAQADMAGFAdEB/xDRAYUBMAEAAwMBACEDAQACUQG4Af8E+wL8Af8I/AH7Av8EuAFRAQACAwEAHgMBAAJGAccB/wP7Af8S"
    "+wH/A8cBRgEAAgMBABsEAQABGAGgAf8C/AL/B/wB+wb8Af8H/AL/AqABGAEAAQQBABkDAQABSAHkAf8I/AH7Af8M+wH8Af8I"
    "5AFIAQABAwEAGXoB/wL7Af8F+wH/BOcBxAGkAYwBgAKMAaQBxAHnAf8E+wH/BfsB/wJ6AQAZlwH/B/wB/wPCAXcBNgENAQAI"
    "DQE2AXcBwgH/A/wB/weXAQAVAwEAAacB/wH6Af8H8wGYATQBAAUDAQQEAwEABTQBmAHzAf8H+gH/AacBAAEDAQARBAEAAZgB"
    "/wH5Af8E/AH/AqUBJgEAAwQBAAwEAQADJgGlAf8C/AH/BPkB/wGYAQABBAEADwQBAAF5Af8B+gH/BPsB/wHkAU8BAAIEAQAS"
    "BAEAAk8B5AH/AfsB/wT6Af8BeQEAAQQBAA0DAQABRAH/AfsB/wT7Af8BuwEbAQABBAEAFgQBAAEbAbsB/wH7Af8E+wH/AUQB"
    "AAEDAQANFgHnAf8F+wH/AacBAAIEAQAYBAEAAqcB/wH7Af8F5wEWAQAMAwEAAaQB/wH7Af8D+wH/AacBACCnAf8B+wH/A/sB"
    "/wGkAQABAwEACQMBAAFBAf8B/AH/A/wB/wG+AQAivgH/AfwB/wP8Af8BQQEAAQMBAArMAf8B/AH/BOcBFwEAIhcB5wH/BPwB"
    "/wHMAQAJBAEAAU0B/wH8Af8D/AH/AUwBAAEEAQAOAwEAAgQBAA4EAQABTAH/AfwB/wP8Af8BTQEAAQQBAAi9Af8B/AH/AvwB"
    "/wGqAQABBAEAIgQBAAGqAf8B/AH/AvwB/wG9AQAJLAH/BvMBIAEAEXYBzQHSAYUBBwEAECAB8wH/BiwBAAYEAQABhQH/AfsB"
    "/wL7Af8BmQEAAQQBAA0DAQABkwH/BLEBAA8EAQABmQH/AfsB/wL7Af8BhQEAAQQBAAbUAf8GMAEAAQMBAA4lAf8B+wH8Af8B"
    "+gH/AT8BAAEDAQAMAwEAATAB/wbUAQAHKwH/BPwB/wHEAQAPAwEAAUUB/wT7Af8BYwEAAQQBAA/EAf8B/AH/BCsBAAQEAQAB"
    "ZQH/AfsB/wL7Af8BdQEAAQQBAA0DAQABQQH/BPsB/wFfAQABBAEADQQBAAF1Af8B+wH/AvsB/wFlAQABBAEAAgQBAAGZAf8B"
    "+wH/BDQBAA8DAQABQgH/BPsB/wFgAQABBAEADzQB/wT7Af8BmQEAAQQBAATCAf8B/AH/A+gBCgEADwMBAAFCAf8E+wH/AWAB"
    "AAEEAQAPCgHoAf8D/AH/AcIBAAUEAd8B/wP8Af8BwgEAEAMBAAFCAf8E+wH/AWABAAEEAQAQwgH/AfwB/wPfAQQBAAQSAe8B"
    "/wP7Af8BpQEAAQQBAA4DAQABQgH/BPsB/wFgAQABBAEADgQBAAGlAf8B+wH/A+8BEgEABCMB/AH/A/sB/wGKAQABBAEADgMB"
    "AAFCAf8E+wH/AWABAAEEAQAOBAEAAYoB/wH7Af8D/AEjAQAEKAH/BPsB/wF+AQABBAEADgMBAAFCAf8E+wH/AWABAAEEAQAO"
    "BAEAAX4B/wH7Af8EKAEABCgB/wT7Af8BfgEAAQQBAA4DAQABQgH/BPsB/wFgAQABBAEADgQBAAF+Af8B+wH/BCgBAAQjAfwB"
    "/wP7Af8BigEAAQQBAA4DAQABQgH/BPsB/wFgAQABBAEADgQBAAGKAf8B+wH/A/wBIwEABBIB7wH/A/sB/wGlAQABBAEADgMB"
    "AAFCAf8E+wH/AWABAAEEAQAOBAEAAaUB/wH7Af8D7wESAQAEBAHfAf8D/AH/AcIBABADAQABQgH/BPsB/wFgAQABBAEAEMIB"
    "/wH8Af8D3wEEAQAFwgH/AfwB/wPoAQoBAA8DAQABQgH/BPsB/wFgAQABBAEADwoB6AH/A/wB/wHCAQAEBAEAAZkB/wH7Af8E"
    "NAEADwMBAAFCAf8E+wH/AWABAAEEAQAPNAH/BPsB/wGZAQABBAEAAgQBAAFlAf8B+wH/AvsB/wF1AQABBAEADQMBAAFBAf8E"
    "+wH/AV8BAAEEAQANBAEAAXUB/wH7Af8C+wH/AWUBAAEEAQAEKwH/BPwB/wHEAQAPAwEAAUUB/wT7Af8BYwEAAQQBAA/EAf8B"
    "/AH/BCsBAAfUAf8GMAEAAQMBAA4rAf8B+wH/AvoB/wFHAQABAwEADAMBAAEwAf8G1AEABgQBAAGFAf8B+wH/AvsB/wGZAQAB"
    "BAEAD6oB/wTGAQYBAA4EAQABmQH/AfsB/wL7Af8BhQEAAQQBAAYsAf8G8wEgAQAQCgGUAeQB6AGjARQBABAgAfMB/wYsAQAJ"
    "vQH/AfwB/wL8Af8BqgEAAQQBABALAQ4BABAEAQABqgH/AfwB/wL8Af8BvQEACAQBAAFNAf8B/AH/A/wB/wFMAQABBAEADgMB"
    "AAIDAQAOBAEAAUwB/wH8Af8D/AH/AU0BAAEEAQAJzAH/AfwB/wTnARcBACIXAecB/wT8Af8BzAEACgMBAAFBAf8B/AH/A/wB"
    "/wG+AQAivgH/AfwB/wP8Af8BQQEAAQMBAAkDAQABpAH/AfsB/wP7Af8BpwEAIKcB/wH7Af8D+wH/AaQBAAEDAQAMFgHnAf8F"
    "+wH/AacBAAIEAQAYBAEAAqcB/wH7Af8F5wEWAQANAwEAAUQB/wH7Af8E+wH/AbsBGwEAAQQBABYEAQABGwG7Af8B+wH/BPsB"
    "/wFEAQABAwEADQQBAAF5Af8B+gH/BPsB/wHkAU8BAAIEAQASBAEAAk8B5AH/AfsB/wT6Af8BeQEAAQQBAA8EAQABmAH/AfkB"
    "/wT8Af8CpQEmAQADBAEADAQBAAMmAaUB/wL8Af8E+QH/AZgBAAEEAQARAwEAAacB/wH6Af8H8wGYATQBAAUDAQQEAwEABTQB"
    "mAHzAf8H+gH/AacBAAEDAQAVlwH/B/wB/wPCAXcBNgENAQAIDQE2AXcBwgH/A/wB/weXAQAZegH/AvsB/wX7Af8E5wHEAaQB"
    "jAGAAowBpAHEAecB/wT7Af8F+wH/AnoBABkDAQABSAHkAf8I/AH7Af8M+wH8Af8I5AFIAQABAwEAGQQBAAEYAaAB/wL8Av8H"
    "/AH7BvwB/wf8Av8CoAEYAQABBAEAGwMBAAJGAccB/wP7Af8S+wH/A8cBRgEAAgMBAB4DAQACUQG4Af8E+wL8Af8I/AH7Av8E"
    "uAFRAQACAwEAIQMBAAMwAYUB0QH/ENEBhQEwAQADAwEAJAMBAAQsAWMBmQHCAd0B7QH8Af8C/AHtAd0BwgGZAWMBLAEABAMB"
    "ACkEAQAFBQERASQBJwIkAREBBQEABQQBAC8EAgAKBAIAWQ==";

// Interior of the ring, ring edge included: a solid disc used only when the
// flat-fill option is on. Derived from the ring mask by flood-filling the
// area it encloses, so its outline matches the ring exactly at every size.
static const char* const kPowerGlyphDiscRle =
    "AFkEAgAKBAIALwQBAAUFAREBJAEnAiQBEQEFAQAFBAEAKQMBAAQsAWMB/wxjASwBAAQDAQAkAwEAAzAB/xQwAQADAwEAIQMB"
    "AAJRAf8YUQEAAgMBAB4DAQACRgH/HEYBAAIDAQAbBAEAARgB/yAYAQABBAEAGQMBAAFIAf8iSAEAAQMBABl6Af8kegEAGf8o"
    "ABUDAQAB/yoAAQMBABEEAQAB/ywAAQQBAA8EAQABeQH/LHkBAAEEAQANAwEAAUQB/y5EAQABAwEADRYB/zAWAQAMAwEAAf8y"
    "AAEDAQAJAwEAAUEB/zJBAQABAwEACv80AAkEAQABTQH/NE0BAAEEAQAI/zYACSwB/zYsAQAGBAEAAf84AAEEAQAG/zgABysB"
    "/zgrAQAEBAEAAWUB/zhlAQABBAEAAgQBAAH/OgABBAEABP86AAUEAf86BAEABBIB/zoSAQAEIwH/OiMBAAQoAf86KAEABCgB"
    "/zooAQAEIwH/OiMBAAQSAf86EgEABAQB/zoEAQAF/zoABAQBAAH/OgABBAEAAgQBAAFlAf84ZQEAAQQBAAQrAf84KwEAB/84"
    "AAYEAQAB/zgAAQQBAAYsAf82LAEACf82AAgEAQABTQH/NE0BAAEEAQAJ/zQACgMBAAFBAf8yQQEAAQMBAAkDAQAB/zIAAQMB"
    "AAwWAf8wFgEADQMBAAFEAf8uRAEAAQMBAA0EAQABeQH/LHkBAAEEAQAPBAEAAf8sAAEEAQARAwEAAf8qAAEDAQAV/ygAGXoB"
    "/yR6AQAZAwEAAUgB/yJIAQABAwEAGQQBAAEYAf8gGAEAAQQBABsDAQACRgH/HEYBAAIDAQAeAwEAAlEB/xhRAQACAwEAIQMB"
    "AAMwAf8UMAEAAwMBACQDAQAELAFjAf8MYwEsAQAEAwEAKQQBAAUFAREBJAEnAiQBEQEFAQAFBAEALwQCAAoEAgBZ";
static constexpr int kPowerGlyphMaskSize = 64;

// Decodes one RLE stream into a 64x64 coverage mask. Returns nullptr on a
// short or corrupt stream, in which case the caller simply skips that layer.
static bool DecodePowerGlyphMask(const char* rleBase64, std::vector<BYTE>& mask) {
    const int pixels = kPowerGlyphMaskSize * kPowerGlyphMaskSize;
    DWORD rleSize = 0;
    if (!CryptStringToBinaryA(rleBase64, 0, CRYPT_STRING_BASE64,
                              nullptr, &rleSize, nullptr, nullptr) || rleSize == 0) {
        return false;
    }
    std::vector<BYTE> rle(rleSize);
    if (!CryptStringToBinaryA(rleBase64, 0, CRYPT_STRING_BASE64,
                              rle.data(), &rleSize, nullptr, nullptr)) {
        return false;
    }

    std::vector<BYTE> out;
    out.reserve(pixels);
    for (DWORD i = 0; i + 1 < rleSize; i += 2) {
        const BYTE value = rle[i];
        int count = rle[i + 1];
        if ((int)out.size() + count > pixels) count = pixels - (int)out.size();
        out.insert(out.end(), count, value);
        if ((int)out.size() >= pixels) break;
    }
    if ((int)out.size() != pixels) return false;   // corrupt stream: skip the layer
    mask.swap(out);
    return true;
}

// Both masks are decoded once on first use and cached for the life of the
// process.
static const BYTE* GetPowerGlyphMask() {
    static std::vector<BYTE> mask;
    static bool tried = false;
    if (!tried) { tried = true; DecodePowerGlyphMask(kPowerGlyphMaskRle, mask); }
    return mask.empty() ? nullptr : mask.data();
}

static const BYTE* GetPowerGlyphDisc() {
    static std::vector<BYTE> mask;
    static bool tried = false;
    if (!tried) { tried = true; DecodePowerGlyphMask(kPowerGlyphDiscRle, mask); }
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

static DialogLayout ComputeLayout(int clientW, int clientH, size_t programCount) {
    DialogLayout L;
    L.width = DpiScale(kPanelWidth);

    // Vertical space the panel needs above and below the list itself.
    // Every value below is a 96-DPI reference measurement, scaled to the
    // current DPI (100% = 96 DPI, 125% = 120 DPI, 150% = 144 DPI, ...).
    const int chromeAbove = DpiScale(130) + DpiScale(kListPadTop);      // panel top -> first row
    const int chromeBelow = DpiScale(kListPadBot) + DpiScale(20) + DpiScale(68)   // pad + gap + body text
                          + DpiScale(20) + DpiScale(g_skin->buttonHeight) + DpiScale(70);  // gap + buttons + margin
    const int room = clientH - chromeAbove - chromeBelow - DpiScale(24);
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

    L.ox = (clientW - L.width) / 2;
    L.oy = (clientH - L.height) / 2;
    if (L.ox < 0) L.ox = 0;
    if (L.oy < 0) L.oy = 0;

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
    return ComputeLayout(w, h, g_openPrograms.size());
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

static bool ShowWin7LogoffDialog(UINT flags, DWORD reason);

static LRESULT CALLBACK HotkeyProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_HOTKEY && wp == kHotkeyId && !g_dialog && g_enabled) {
        // Preview only: the return value is deliberately ignored, so no
        // logoff is ever performed. With no program open the screen skips
        // itself just like it would during a real logoff.
        try {
            ShowWin7LogoffDialog(EWX_LOGOFF, 0);
            if (g_totalPrograms == 0)
                Wh_Log(L"Preview skipped: no program is currently open");
        }
        catch (...) { Wh_Log(L"Simulation failed: exception contained"); }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI HotkeyThreadProc(LPVOID) {
    try {
        WNDCLASSW wc{}; wc.hInstance=GetModuleHandleW(nullptr); wc.lpfnWndProc=HotkeyProc;
        wc.lpszClassName=L"WindhawkWin7LogoffHotkey";
        RegisterClassW(&wc);
        g_hotkeyWindow=CreateWindowExW(0,wc.lpszClassName,L"",0,0,0,0,0,
                                       HWND_MESSAGE,nullptr,wc.hInstance,nullptr);
        if (!g_hotkeyWindow) { LogCritical(L"Hotkey window could not be created"); return 1; }
        if (!RegisterHotKey(g_hotkeyWindow, kHotkeyId, g_hotkeyMods, g_hotkeyVk)) {
            // Almost always ERROR_HOTKEY_ALREADY_REGISTERED: something else,
            // or Windows itself, already owns the combination. Naming the
            // error saves the user guessing why nothing happens on the keys.
            const DWORD err = GetLastError();
            if (err == ERROR_HOTKEY_ALREADY_REGISTERED)
                LogCritical(L"The preview shortcut is already taken by another "
                            L"program; choose a different one in the settings");
            else
                LogCritical(L"RegisterHotKey failed");
            Wh_Log(L"RegisterHotKey error %u (mods 0x%X, vk 0x%X)", err, g_hotkeyMods, g_hotkeyVk);
            DestroyWindow(g_hotkeyWindow); g_hotkeyWindow = nullptr;
            return 1;
        }
        Wh_Log(L"Preview shortcut registered (mods 0x%X, vk 0x%X)", g_hotkeyMods, g_hotkeyVk);
        MSG m{};
        while (GetMessageW(&m,nullptr,0,0)>0) {
            TranslateMessage(&m); DispatchMessageW(&m);
        }
        UnregisterHotKey(g_hotkeyWindow,kHotkeyId);
        DestroyWindow(g_hotkeyWindow); g_hotkeyWindow=nullptr;
        UnregisterClassW(wc.lpszClassName,wc.hInstance);
    } catch (...) { LogCritical(L"Hotkey thread exception"); }
    return 0;
}

static LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        SetTimer(hwnd, 1, 1000, nullptr);
        return 0;
    case WM_TIMER:
        // Safety watchdog only: it never redraws. Rendering is event-driven.
        // The dialog is closed after 60 seconds so logoff cannot be blocked.
        if (GetTickCount64() - g_dialogStart >= 60000) {
            g_force = true; g_proceed = true;
            DestroyWindow(hwnd);
        } else {
            const int changed = RefreshOpenPrograms();
            // Programs close by themselves in the background, so the list can
            // empty out while the screen is up. Showing "0 programs still
            // need to close:" would be nonsense, and there is nothing left to
            // wait for either: close the screen and let Windows carry on
            // normally (g_force stays false -- nothing had to be forced).
            if (g_totalPrograms == 0) {
                g_force = false; g_proceed = true;
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
        }
        if (!g_desktop && GetTickCount64() - g_captureFailedAt >= 1000) {
            FreeDesktopBitmap();
            g_desktop = CaptureDesktop();
            g_captureFailedAt = g_desktop ? 0 : GetTickCount64();
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) { g_force = false; g_proceed = false; DestroyWindow(hwnd); return 0; }
        if (wp == VK_RETURN) { g_force = true;  g_proceed = true;  DestroyWindow(hwnd); return 0; }
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
    case WM_LBUTTONUP: {
        if (g_draggingThumb) { g_draggingThumb = false; ReleaseCapture(); return 0; }
        const int x = GET_X_LPARAM(lp), y = GET_Y_LPARAM(lp);
        DialogLayout L = LayoutForWindow(hwnd);
        RECT force = L.force, cancel = L.cancel;
        if (PtInRect(&force, POINT{x,y})) { g_force = true; g_proceed = true; DestroyWindow(hwnd); }
        else if (PtInRect(&cancel, POINT{x,y})) { g_force = false; g_proceed = false; DestroyWindow(hwnd); }
        return 0;
    }
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
        // Everything below is drawn into an off-screen buffer and blitted
        // to the window in one shot at the end. Without this, each hover
        // repaint (BitBlt + alpha-blended veil + per-row gradients) was
        // visible mid-draw as a brief flash/disappearance of the buttons.
        HDC dc = CreateCompatibleDC(screenDc);
        HBITMAP backBmp = CreateCompatibleBitmap(screenDc, cr.right, cr.bottom);
        HGDIOBJ oldBackBmp = backBmp ? SelectObject(dc, backBmp) : nullptr;
        if (!backBmp) dc = screenDc; // Fallback: draw directly if allocation failed.
        if (g_desktop) {
            HDC src = CreateCompatibleDC(dc);
            HGDIOBJ old = SelectObject(src, g_desktop);
            BitBlt(dc, 0, 0, cr.right, cr.bottom, src, 0, 0, SRCCOPY);
            SelectObject(src, old); DeleteDC(src);
        } else {
            HBRUSH fallback = CreateSolidBrush(RGB(0x1a,0x2a,0x3a));
            if (fallback) { FillRect(dc, &cr, fallback); DeleteObject(fallback); }
        }
        // Windows 7-like dark veil.
        // Windows 7-style translucent dark veil. The desktop remains visible.
        HDC veilDc = CreateCompatibleDC(dc);
        HBITMAP veilBmp = CreateCompatibleBitmap(dc, cr.right, cr.bottom);
        if (veilDc && veilBmp) {
            HGDIOBJ old = SelectObject(veilDc, veilBmp);
            HBRUSH veilBrush = CreateSolidBrush(g_skin->veil);
            if (veilBrush) { FillRect(veilDc, &cr, veilBrush); DeleteObject(veilBrush); }
            BLENDFUNCTION bf{AC_SRC_OVER, 0, g_skin->veilAlpha, 0};
            AlphaBlend(dc, 0, 0, cr.right, cr.bottom, veilDc, 0, 0,
                       cr.right, cr.bottom, bf);
            SelectObject(veilDc, old);
            DeleteObject(veilBmp);
        }
        if (veilDc) DeleteDC(veilDc);
        // Single source of truth for every coordinate below; the panel is
        // sized from the real number of programs instead of assuming three.
        DialogLayout L = ComputeLayout(cr.right, cr.bottom, g_openPrograms.size());
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
            BitBlt(screenDc, 0, 0, cr.right, cr.bottom, dc, 0, 0, SRCCOPY);
            SelectObject(dc, oldBackBmp);
            DeleteObject(backBmp);
        }
        if (dc != screenDc) DeleteDC(dc);
        EndPaint(hwnd,&ps); return 0;
    }
    case WM_DESTROY: KillTimer(hwnd,1); PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}

static bool ShowWin7LogoffDialog(UINT flags, DWORD reason) {
    try {
    if (g_dialog || g_insideHook) return true;
    // Master switch. Read before anything else is touched so that a disabled
    // mod does not even enumerate windows or capture the desktop: the caller
    // is told to proceed, unforced, exactly as if the mod were not installed.
    // Both hooks and the preview hotkey funnel through here, so this single
    // test is enough to make the whole feature inert.
    LoadSkinSetting();
    // Re-read the DPI every time the screen is shown: a scaling change made
    // while the mod was loaded is picked up here, and the value is what the
    // whole layout multiplies its reference metrics by.
    g_dpi = GetProcessDpi();
    if (!g_enabled) return true;
    g_pendingFlags=flags; g_pendingReason=reason; g_force=false; g_proceed=false;
    // Which wording the screen uses is decided here, once. EWX_REBOOT is
    // tested first because a restart also carries the shutdown semantics, and
    // EWX_POWEROFF is treated as a shutdown because "Shut down" from the Start
    // menu passes it rather than a bare EWX_SHUTDOWN -- without it the screen
    // would offer to "force log off" while the machine is powering off.
    g_action = (flags & EWX_REBOOT)                     ? kActionRestart
             : (flags & (EWX_SHUTDOWN | EWX_POWEROFF))  ? kActionShutdown
             : kActionLogoff;
    g_listScroll = 0; g_draggingThumb = false; // always open at the top of the list
    // The settings were already re-read above, so a skin or language changed
    // in Windhawk applies to this very screen without a reload.
    RefreshOpenPrograms();

    // Nothing is holding the logoff back, so there is nothing to report and
    // nothing to decide: staying out of the way is what Windows 7 did too.
    // Putting up a screen that reads "0 programs still need to close:" would
    // be awkward and would only delay a logoff that can just go ahead.
    if (g_totalPrograms == 0) {
        FreeProgramIcons(); g_openPrograms.clear();
        return true; // continue with the logoff/shutdown, unforced
    }
    g_dialogStart = GetTickCount64();
    FreeDesktopBitmap(); g_desktop=CaptureDesktop();
    g_captureFailedAt = g_desktop ? 0 : g_dialogStart;
    WNDCLASSW wc{}; wc.hInstance=GetModuleHandleW(nullptr); wc.lpfnWndProc=DialogProc;
    wc.lpszClassName=kClassName; wc.hCursor=LoadCursor(nullptr,IDC_ARROW);
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1); RegisterClassW(&wc);
    int w=GetSystemMetrics(SM_CXSCREEN), h=GetSystemMetrics(SM_CYSCREEN); int x=0, y=0;
    g_dialog=CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW,kClassName,L"Windows",WS_POPUP|WS_VISIBLE,x,y,w,h,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    if (g_dialog) { SetForegroundWindow(g_dialog); SetFocus(g_dialog); }
    if (!g_dialog) { LogCritical(L"CreateWindowExW failed"); FreeDesktopBitmap(); return true; }
    MSG m{}; while (g_dialog && GetMessageW(&m,nullptr,0,0)>0) { TranslateMessage(&m); DispatchMessageW(&m); }
    g_dialog=nullptr; FreeDesktopBitmap(); return g_proceed;
    } catch (...) {
        g_dialog = nullptr; FreeDesktopBitmap();
        Wh_Log(L"Logoff dialog exception contained");
        return true;
    }
}

static BOOL WINAPI ExitWindowsEx_Hook(UINT flags, DWORD reason) {
    try {
    if (!ExitWindowsEx_Original) return FALSE;
    if (g_insideHook) return ExitWindowsEx_Original(flags, reason);
    if (!ShowWin7LogoffDialog(flags, reason)) return TRUE;
    g_insideHook=true;
    BOOL result=ExitWindowsEx_Original(flags | (g_force ? EWX_FORCEIFHUNG : 0), reason);
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
    try {
    if (!InitiateShutdownW_Original) return ERROR_PROC_NOT_FOUND;
    // Only local, interactive requests get the screen. A remote shutdown or a
    // re-entrant call must pass straight through.
    if (g_insideHook || (machineName && *machineName))
        return InitiateShutdownW_Original(machineName, message, gracePeriod, shutdownFlags, reason);

    UINT ewx = (shutdownFlags & SHUTDOWN_RESTART) ? EWX_REBOOT
             : (shutdownFlags & SHUTDOWN_POWEROFF) ? EWX_POWEROFF
             : EWX_SHUTDOWN;
    if (!ShowWin7LogoffDialog(ewx, reason)) return ERROR_SUCCESS; // cancelled by the user

    g_insideHook=true;
    DWORD extra = g_force ? (SHUTDOWN_FORCE_OTHERS | SHUTDOWN_FORCE_SELF) : 0;
    DWORD result=InitiateShutdownW_Original(machineName, message, gracePeriod,
                                            shutdownFlags | extra, reason);
    g_insideHook=false; return result;
    } catch (...) {
        g_insideHook=false;
        return InitiateShutdownW_Original
            ? InitiateShutdownW_Original(machineName, message, gracePeriod, shutdownFlags, reason)
            : ERROR_PROC_NOT_FOUND;
    }
}

BOOL Wh_ModInit() {
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
            else LogCritical(L"Hooking ExitWindowsEx failed");
        } else LogCritical(L"ExitWindowsEx not found");
    }

    // advapi32 is not always loaded yet in the shell host processes, so it is
    // loaded explicitly rather than merely looked up. The reference is
    // intentionally never released: the hook must outlive this function.
    if (HMODULE advapi32 = LoadLibraryW(L"advapi32.dll")) {
        if (auto target=(InitiateShutdownW_t)(void*)GetProcAddress(advapi32,"InitiateShutdownW")) {
            if (WindhawkUtils::SetFunctionHook(target, InitiateShutdownW_Hook, &InitiateShutdownW_Original))
                anyHook = true;
            else LogCritical(L"Hooking InitiateShutdownW failed");
        } else LogCritical(L"InitiateShutdownW not found");
    }

    if (!anyHook) { LogCritical(L"No shutdown entry point could be hooked"); return FALSE; }

    LoadSkinSetting();

    // The preview hotkey only makes sense in the process that owns the
    // desktop; registering the same hotkey in every shell host would make all
    // but the first registration fail and log a spurious error.
    wchar_t exePath[MAX_PATH]{};
    if (GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath))) {
        const wchar_t* exe = wcsrchr(exePath, L'\\');
        exe = exe ? exe + 1 : exePath;
        if (_wcsicmp(exe, L"explorer.exe") == 0 && g_hotkeyVk != 0) {
            g_hotkeyThread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0, &g_hotkeyThreadId);
            if (!g_hotkeyThread) LogCritical(L"CreateThread for hotkey failed");
        }
    }
    return TRUE;
}

// Windhawk calls this when the user presses Save in the settings. Skin and
// language are picked up immediately; if the screen happens to be on display
// it is repainted, so a language change is visible on the spot rather than
// only on the next shutdown.
void Wh_ModSettingsChanged() {
    LoadSkinSetting();
    if (!g_dialog) return;
    if (!g_enabled) {
        // Turned off while the screen was up. Letting it stand would leave a
        // full-screen window belonging to a feature the user just disabled,
        // so it is dismissed and the pending action is allowed to continue
        // unforced -- the same outcome as if the mod had never intercepted it.
        g_force = false; g_proceed = true;
        DestroyWindow(g_dialog);
        return;
    }
    InvalidateRect(g_dialog, nullptr, TRUE);
}

void Wh_ModUninit() {
    if (g_hotkeyThreadId) PostThreadMessageW(g_hotkeyThreadId, WM_QUIT, 0, 0);
    if (g_hotkeyThread) { WaitForSingleObject(g_hotkeyThread, 3000); CloseHandle(g_hotkeyThread); g_hotkeyThread=nullptr; }
    g_hotkeyThreadId=0;
    if (g_dialog) { g_force=false; g_proceed=false; DestroyWindow(g_dialog); g_dialog=nullptr; }
    FreeProgramIcons(); g_openPrograms.clear(); g_listScroll = 0; g_totalPrograms = 0;
    g_draggingThumb = false; FreeDesktopBitmap(); UnregisterClassW(kClassName,GetModuleHandleW(nullptr));
}
