// ==WindhawkMod==
// @id              win7-uac-recreation
// @name            Windows 7/8.1 UAC Recreation
// @description     This mod recreates the Windows 7/8.1 UAC prompt on Windows 10/11 without changing UAC behavior or disabling the secure desktop
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         consent.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32 -lole32 -loleaut32 -luuid -lshell32 -lcomctl32 -lpsapi -luxtheme -lcrypt32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7/8.1 UAC Recreation

This mod recreates the Windows 7 and 8.1 classic UAC interface on Windows 10 and 11. It changes only the presentation: Windows retains control of consent, authentication, and the secure desktop.

## Features

- Four themes: Windows 7 Basic, custom Aero, Windows 8.1, and Windows 10 (1507–1607).
- It restores the native prompt if the recreation fails; never makes an elevation decision.
- It leaves system files, settings, and credential prompts untouched.
- It matches the banner and shield to Windows' publisher verdict.
- Ithows the requesting program's icon and native prompt details when available.

## Emergency shortcut

Press `Ctrl+Shift+F10` to close the recreated dialog and return to the native prompt without approving or denying the request.

## Themes

### Windows 7 (Basic)
![Windows 7 UAC prompt](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win7uac.PNG)

### Windows 7 Aero
![Windows 7 Aero UAC prompt](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win7uacaero.PNG)

### Windows 8.1
![Windows 8.1 UAC prompt](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win81uac.PNG)

### Windows 10 (1507–1607)
![Windows 10 UAC prompt](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win10uac.PNG)

## Installation

Windhawk excludes `consent.exe` by default. To enable the mod:

1. Open **Settings** → **Advanced settings** → **More advanced settings**.
2. Add `consent.exe` to the **Process inclusion list**.
3. Install and enable the mod.

Without this change, the mod has no effect.

## Settings

| Setting | Description |
|---|---|
| **Dialog skin** | Choose one of the four themes. |
| **Replace the UAC prompt** | Replace the prompt or observe it and log events. |
| **Which apps get the Classic UAC prompt** | Apply to all apps, only listed apps, or all except listed apps. |
| **App list** | Semicolon-separated application names for the filter above. |
| **Button style** | Use the system button or the mod's Aero-style button. |
| **Which shield is shown** | Choose automatic, verified, question, or exclamation. |
| **Banner colour** | Choose automatic, blue, yellow, or red; manual choices can only make the prompt more cautious. |
| **The Windows prompt, while the Windows 7 one is on screen** | Move the native prompt off-screen or hide it. |
| **Read the elevation request** | Read request details to show the app icon and "File origin". |
| **How long to wait for the prompt** | Wait time, in milliseconds, before leaving the native prompt unchanged. |
| **Watchdog** | Close the recreated dialog after this many seconds without a response. |
| **Panic hotkey** | Shortcut to restore the native prompt; default: `Ctrl+Shift+F10`. |
| **Extra Yes/No key** | Optional Alt+letter shortcut forwarded to the original prompt. |
| **Debug log** | Record mod activity. |
| **Write a diagnostics dump** | Write diagnostics; the mod runs in observation mode. |
| **Write the raw elevation request to the log** | Log the raw request, which may include file and user names. |
| **How the prompt is found** | Choose desktop watchers (recommended), polling, or hook only. |

## Behavior and limitations

- The banner follows the native publisher row. The shield follows the selected theme and Windows' verdict; the mod never upgrades an unknown or unverified publisher to verified.
- Publisher checks use the file's Authenticode signature or its Windows catalog when applicable. The native verdict is not overridden.
- Regedit is shown by its localized name only after Windows confirms the system file's signature.
- Details that cannot be read from the native prompt are omitted, not guessed.
- A XAML candidate without a Win32 edit control may be moved off-screen briefly during UI Automation classification.
- Logs and diagnostic files can contain prompt text, command lines, file names, and user names. Enable them only when needed.

## Tested systems

The modification has been tested on the following systems:
- Windows 10 21H2
- Windows 11 24H2
- Windows 11 25H2

## Credits

- **AdministratoX** — Windows 11 24H2/25H2 testing
- **Microsoft** — Documentation
- **m417z** — Technical reference and review

## Reporting issues

If there are anomalies, please report them to the author. It is recommended to include the debug log and, if needed, the diagnostic file.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- windowTheme: win7Basic
  $name: Dialog skin
  $description: "This setting selects the window frame and shield artwork. Windows 7 Aero is an additional custom theme with the complete Windows 7 shield set; consent behavior is unchanged."
  $options:
  - win7Basic: Windows 7 (Basic Theme)
  - win7Aero: Windows 7 Aero (Custom Theme)
  - win81Native: Windows 8.1 (Native System Theme)
  - win10Flat: Windows 10 (versions 1507-1607, Flat Icons)
- replaceUAC: true
  $name: Replace the UAC prompt
  $description: "This setting chooses between replacing the UAC prompt and only observing it. When it is off, the Windows prompt is left exactly as it is and every decision is written to the log."
- appsScope: all
  $name: Which apps get the Classic UAC prompt
  $description: "This setting selects which applications receive the Classic UAC prompt. The app list below applies when the selected scope uses the list."
  $options:
  - all: All apps (recommended)
  - selected: Only the apps in the list below
  - except: All apps except the ones in the list
- appsList: notepad.exe
  $name: App list
  $description: "This setting holds the application names used by the option above. Entries are separated with a semicolon; a partial, case-insensitive name is enough."
- buttonStyle: system
  $name: Button style
  $description: "This setting chooses how the Yes and No buttons are drawn. The Windows-drawn buttons follow the current theme, the Aero ones are painted to the Windows 7 reference."
  $options:
  - system: Drawn by Windows (recommended)
  - aero: The Windows 7 Aero buttons, painted by the mod
- bannerShield: auto
  $name: Which shield is shown
  $description: "This setting chooses the shield for non-flat themes. An orange banner always uses its matching warning shield; Flat Icons always uses the blue Windows shield. A manual choice that is more cautious than the automatic one is always applied; the verified shield can be asked for only when the operating system has verified the publisher, so that the prompt never shows more trust than Windows itself grants."
  $options:
  - auto: Automatic (recommended)
  - verified: Verified shield
  - question: Blue question mark
  - exclamation: Amber exclamation mark
- bannerColour: auto
  $name: Banner colour
  $description: "This setting chooses the banner colour. Automatic mode follows the publisher and the native Yes-button state; manual choices can only make the banner more cautious (blue, then yellow, then red), never more trusted, and never approve or deny a request."
  $options:
  - auto: Automatic (recommended)
  - blue: Always blue
  - yellow: Always yellow
  - red: Always red
- promptWindow: offscreen
  $name: The Windows prompt, while the Windows 7 one is on screen
  $description: "This setting chooses where the Windows prompt is kept while the Windows 7 one is on screen. That prompt has to keep running, because it is the one that finishes the request."
  $options:
  - offscreen: Moved off screen (recommended)
  - hidden: Hidden
- readRequestBlock: true
  $name: Read the elevation request
  $description: "This setting allows the elevation request to be read, read-only, so that the real program icon and the File origin line can be shown. When it is off, the stock icon is used and that line is left out."
- promptTimeoutMs: 10000
  $name: How long to wait for the prompt (milliseconds)
  $description: "This setting is how long the mod waits for the prompt window to exist before leaving the Windows prompt alone. A slower machine needs longer than a fast one."
- watchdogSeconds: 20
  $name: Watchdog (seconds)
  $description: "This setting is how long the Windows 7 dialog may stay on screen without a usable way to answer before it closes itself and the Windows prompt comes back. Zero disables the watchdog."
- panicHotkey: Ctrl+Shift+F10
  $name: Panic hotkey
  $description: "This setting is the combination that closes the Windows 7 dialog and gives the Windows prompt back without deciding anything. Ctrl, Shift, Alt or Win with one key; empty disables it."
- yesHotkey: ""
  $name: Extra Yes key (advanced)
  $description: "This setting is a single letter, sent as Alt+letter to the original prompt when no other way of answering works. Empty disables it."
- noHotkey: ""
  $name: Extra No key (advanced)
  $description: "This setting is a single letter, sent as Alt+letter to the original prompt when no other way of answering works. Empty disables it."
- debugLog: false
  $name: Debug log
  $description: "This setting writes a detailed log of what the mod does, to the Windhawk log and to DebugView. It is intended for diagnosing a problem."
- dumpDiagnostics: false
  $name: Write a diagnostics dump (nothing is touched)
  $description: "This setting writes a text file with the desktops, the windows found and the prompt's own content. While it is on, the mod only observes: the Windows prompt is never replaced."
- dumpRequestBlock: false
  $name: Write the raw elevation request to the log (diagnosing only)
  $description: "This setting writes the raw elevation request to the log. The request can contain file names and user names, so it is intended for diagnosis only."
- promptDetection: watchers
  $name: How the prompt is found (advanced)
  $description: "This setting chooses how the prompt is found. The default watches both desktops; the other two exist for diagnosing."
  $options:
  - watchers: Desktop watchers (recommended)
  - polling: Look only when a prompt is requested
  - hookOnly: Hook only, no user interface (diagnosing)

*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <objbase.h>
#include <oleauto.h>
#include <shellapi.h>
#include <new>
// initguid.h turns the GUIDs that the headers below merely declare (IID_IImageList,
// WINTRUST_ACTION_GENERIC_VERIFY_V2) into definitions of this translation unit, which is what the
// mod needs: it refers to them by name and must not depend on a library that happens to carry them.
#include <initguid.h>
#include <commctrl.h>
// The shell's image lists (IImageList), and the header that declares SHGetImageList and the
// SHIL_* constants: they are what carry an icon at 48 px (SHIL_EXTRALARGE) and at 256 px
// (SHIL_JUMBO), and both are documented shell APIs.
#include <commoncontrols.h>
// The theme manager, for the "Show details" expander: the row draws the Windows 7 art embedded in
// this file, and this is where the fallback of a system whose theme carries no such art comes from
// - the part and the size it is drawn at, instead of assembling a circle and a triangle by hand.
#include <uxtheme.h>
// The task dialog parts and states (TASKDIALOGPARTS / EXPANDOBUTTONSTATES) come from vssym32.h
// where the toolchain carries it; the ids below are the documented ones (TDLG_EXPANDOBUTTON = 13;
// TDLGEBS_NORMAL = 1, TDLGEBS_HOVER = 2, TDLGEBS_PRESSED = 3, TDLGEBS_EXPANDEDNORMAL = 4,
// TDLGEBS_EXPANDEDHOVER = 5, TDLGEBS_EXPANDEDPRESSED = 6), and the theme is opened with the
// documented class string "TASKDIALOG", which is what VSCLASS_TASKDIALOG names. The guards below
// make both paths agree instead of depending on which of the two headers is present.
#if defined(__has_include)
#  if __has_include(<vssym32.h>)
#    include <vssym32.h>
#  endif
#endif
#include <shlobj.h>
#ifndef TDLG_EXPANDOBUTTON
#define TDLG_EXPANDOBUTTON 13
#endif
#ifndef TDLGEBS_NORMAL
#define TDLGEBS_NORMAL 1
#define TDLGEBS_HOVER 2
#define TDLGEBS_PRESSED 3
#define TDLGEBS_EXPANDEDNORMAL 4
#define TDLGEBS_EXPANDEDHOVER 5
#define TDLGEBS_EXPANDEDPRESSED 6
#endif
#include <wintrust.h>
#include <wincrypt.h>
#include <softpub.h>
#include <windowsx.h>
#include <psapi.h>
#include <stdio.h>
#include <wchar.h>
#include <algorithm>
#include <string>
#include <vector>

#include <windhawk_api.h>


#if __has_include(<windhawk_utils.h>)
#include <windhawk_utils.h>
#define HAVE_UTILS 1
#else
#define HAVE_UTILS 0
#endif

#if __has_include(<uiautomation.h>)
#include <uiautomation.h>
#define HAVE_UIA 1
#else
#define HAVE_UIA 0
// Only used as tags for the Win32 control enumeration, so plain values are enough. Guarded one by
// one: on a configuration where uiautomationclient.h is reachable after this point, its own macros
// arrive later and an unguarded definition here is a redefinition warning in every build log.
typedef int CONTROLTYPEID;
#ifndef UIA_ButtonControlTypeId
#define UIA_ButtonControlTypeId 50000
#endif
#ifndef UIA_EditControlTypeId
#define UIA_EditControlTypeId   50004
#endif
#ifndef UIA_TextControlTypeId
#define UIA_TextControlTypeId   50020
#endif
#ifndef UIA_HyperlinkControlTypeId
#define UIA_HyperlinkControlTypeId 50005
#endif
#endif

#include <utility>
#include <exception>

// C++ exceptions are used only as a last net, at the boundaries Windows calls into and around the
// two long stretches of reading (UiSessionRun, HarvestDetailsSafely, UiWndProc, WinEventProc and
// the three thread entry points). If this compiler is built without them, the macro is 0 and those
// blocks disappear: the same code then runs on plain return values alone. No structured exception
// handling (no __try/__except), and nothing is ever caught silently.
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
#define W7UAC_HAS_EXCEPTIONS 1
#else
#define W7UAC_HAS_EXCEPTIONS 0
#endif

//------------------------------------------------------------------------------
// Settings
//------------------------------------------------------------------------------

// The names in comments are the ones the settings UI shows ("Which apps get the Classic UAC
// prompt", "Dialog skin"...); the numbers are the ones this file speaks.
enum WindowThemePreset {
    WindowThemeWin7Basic = 0,
    WindowThemeWin7Aero = 1,
    WindowThemeWin81Native = 2,
    WindowThemeWin10Flat = 3,
};

struct Settings {
    int relayMode;          // Replace the UAC prompt: 1 on, 0 off (observe only)
    int whitelistMode;      // Which apps: 0 only the list, 1 all apps, 2 all except the list
    std::wstring appsWhitelist;   // App list
    int windowTheme;        // 0 Win7 Basic, 1 custom Win7 Aero, 2 Win8.1 native, 3 Win10 flat
    int style;              // Button style: 0 drawn by Windows, 1 Windows 7 Aero buttons
    BOOL readRequestBlock;
    BOOL dumpRequestBlock;
    int bannerShield;        // 0 auto, 1 verified shield, 2 question-mark shield, 3 exclamation
    int nativeWindowMode;   // 0 offscreen, 1 hidden; legacy inPlace is migrated to offscreen
    int detectTimeoutMs;    // How long to wait for the prompt (milliseconds)
    int detection;         // How the prompt is found: 0 watchers, 1 polling, 2 hook only
    int bannerOverride;     // Banner colour: 0 auto, 1 blue, 2 yellow, 3 red
    WCHAR panicHotkey[64];
    ULONG panicMods;        // MOD_CONTROL | MOD_SHIFT | ... (0 = nothing registered)
    UINT panicVk;           // 0 = disabled
    int watchdogSeconds;
    WCHAR yesHotkey[8];
    WCHAR noHotkey[8];
    BOOL debugLog;
    BOOL dumpUiTree;        // Write a diagnostics dump (dumpDiagnostics)
};

// The settings are published as one immutable snapshot and read without a lock: the interface
// thread, the two watchers, the identity thread and the hook all read them while Windhawk may be
// replacing them, and a std::wstring copied into the live struct while another thread reads it is a
// crash. A new snapshot is built on the side (see LoadSettings) and swapped in with a single store,
// so every reader sees a whole set of values. The superseded snapshot is left to the process, which
// lives for one prompt, and the snapshot itself never runs its destructor under the loader lock.
[[clang::no_destroy]] static Settings g_settingsInitial = {};
static Settings* g_settingsPublished = &g_settingsInitial;
#define g_settings (*g_settingsPublished)

// "Ctrl+Shift+F10" -> MOD_CONTROL | MOD_SHIFT + VK_F10. Anything that is not recognised leaves
// the hotkey disabled, so a typo cannot register something the user did not ask for.
static void ParseHotkey(const WCHAR* text, ULONG* mods, UINT* vk) {
    ULONG m = 0;
    UINT key = 0;
    std::wstring s = text ? text : L"";
    size_t pos = 0;
    while (pos <= s.size()) {
        size_t plus = s.find(L'+', pos);
        std::wstring part = s.substr(
            pos, (plus == std::wstring::npos) ? std::wstring::npos : plus - pos);
        size_t b = part.find_first_not_of(L" \t");
        size_t e = part.find_last_not_of(L" \t");
        part = (b == std::wstring::npos) ? std::wstring() : part.substr(b, e - b + 1);
        if (!part.empty()) {
            if (!_wcsicmp(part.c_str(), L"ctrl") || !_wcsicmp(part.c_str(), L"control"))
                m |= MOD_CONTROL;
            else if (!_wcsicmp(part.c_str(), L"shift")) m |= MOD_SHIFT;
            else if (!_wcsicmp(part.c_str(), L"alt")) m |= MOD_ALT;
            else if (!_wcsicmp(part.c_str(), L"win")) m |= MOD_WIN;
            else if (part.size() == 1) key = (UINT)towupper(part[0]);
            else if ((part[0] == L'F' || part[0] == L'f') && part.size() <= 3) {
                int n = 0;
                bool digits = true;
                for (size_t i = 1; i < part.size(); i++) {
                    if (part[i] < L'0' || part[i] > L'9') { digits = false; break; }
                    n = n * 10 + (int)(part[i] - L'0');
                }
                if (digits && n >= 1 && n <= 24) key = 0x6F + (UINT)n;   // VK_F1 = 0x70
            } else if (!_wcsicmp(part.c_str(), L"pause")) key = VK_PAUSE;
            else if (!_wcsicmp(part.c_str(), L"insert")) key = VK_INSERT;
            else if (!_wcsicmp(part.c_str(), L"delete")) key = VK_DELETE;
            else if (!_wcsicmp(part.c_str(), L"home")) key = VK_HOME;
            else if (!_wcsicmp(part.c_str(), L"end")) key = VK_END;
            else if (!_wcsicmp(part.c_str(), L"esc") || !_wcsicmp(part.c_str(), L"escape"))
                key = VK_ESCAPE;
        }
        if (plus == std::wstring::npos) break;
        pos = plus + 1;
    }
    *mods = m;
    *vk = key;
}

// Reads a setting whose YAML block lists named choices and gives back the index of the chosen
// one: the settings UI shows words, the rest of this file speaks numbers. A value that is missing
// or unknown falls back to the shipped default, which is what a first run sees.
static int ReadChoice(const WCHAR* key, const WCHAR* const* ids, int count, int fallback) {
    PCWSTR s = Wh_GetStringSetting(key);
    int r = fallback;
    for (int i = 0; i < count; i++) {
        if (_wcsicmp(s, ids[i]) == 0) {
            r = i;
            break;
        }
    }
    Wh_FreeStringSetting(s);
    return r;
}

// Reads every setting into a fresh snapshot. It stays the one place where a setting is read, so a
// reload can never miss a limit or a fallback that the first load applies (see the clamps below and
// in the tail of the function): what comes back is always a validated set of values.
static void FillSettings(Settings& out) {
    out.relayMode = Wh_GetIntSetting(L"replaceUAC") ? 1 : 0;
    static const WCHAR* kWindowThemeIds[] = {
        L"win7Basic", L"win7Aero", L"win81Native", L"win10Flat"};
    out.windowTheme = ReadChoice(L"windowTheme", kWindowThemeIds, ARRAYSIZE(kWindowThemeIds), WindowThemeWin7Basic);
    out.debugLog = Wh_GetIntSetting(L"debugLog");
    out.dumpUiTree = Wh_GetIntSetting(L"dumpDiagnostics");

    static const WCHAR* kScopeIds[] = { L"selected", L"all", L"except" };
    out.whitelistMode = ReadChoice(L"appsScope", kScopeIds, ARRAYSIZE(kScopeIds), 1);
    static const WCHAR* kButtonIds[] = { L"system", L"aero" };
    out.style = ReadChoice(L"buttonStyle", kButtonIds, ARRAYSIZE(kButtonIds), 0);
    static const WCHAR* kShieldIds[] = { L"auto", L"verified", L"question", L"exclamation" };
    out.readRequestBlock = Wh_GetIntSetting(L"readRequestBlock");
    out.dumpRequestBlock = Wh_GetIntSetting(L"dumpRequestBlock");
    out.bannerShield = ReadChoice(L"bannerShield", kShieldIds, ARRAYSIZE(kShieldIds), 0);
    static const WCHAR* kColourIds[] = { L"auto", L"blue", L"yellow", L"red" };
    out.bannerOverride = ReadChoice(L"bannerColour", kColourIds, ARRAYSIZE(kColourIds), 0);
    static const WCHAR* kWindowIds[] = { L"offscreen", L"hidden", L"inPlace" };
    out.nativeWindowMode = ReadChoice(L"promptWindow", kWindowIds, ARRAYSIZE(kWindowIds), 0);
    if (out.nativeWindowMode == 2) out.nativeWindowMode = 0; // never display both prompts
    static const WCHAR* kDetectionIds[] = { L"watchers", L"polling", L"hookOnly" };
    out.detection = ReadChoice(L"promptDetection", kDetectionIds, ARRAYSIZE(kDetectionIds), 0);

    out.detectTimeoutMs = Wh_GetIntSetting(L"promptTimeoutMs");
    if (out.detectTimeoutMs < 500) out.detectTimeoutMs = 500;
    if (out.detectTimeoutMs > 30000) out.detectTimeoutMs = 30000;

    // Wh_GetStringSetting returns a PCWSTR which must be freed with Wh_FreeStringSetting.
    // It never returns NULL: it gives back L"" when the value is missing.
    PCWSTR s = Wh_GetStringSetting(L"appsList");
    out.appsWhitelist = s;
    Wh_FreeStringSetting(s);

    s = Wh_GetStringSetting(L"panicHotkey");
    wcsncpy_s(out.panicHotkey, *s ? s : L"", _TRUNCATE);
    Wh_FreeStringSetting(s);
    ParseHotkey(out.panicHotkey, &out.panicMods, &out.panicVk);

    out.watchdogSeconds = Wh_GetIntSetting(L"watchdogSeconds");
    if (out.watchdogSeconds < 0) out.watchdogSeconds = 0;
    if (out.watchdogSeconds > 300) out.watchdogSeconds = 300;

    s = Wh_GetStringSetting(L"yesHotkey");
    wcsncpy_s(out.yesHotkey, *s ? s : L"", _TRUNCATE);
    Wh_FreeStringSetting(s);

    s = Wh_GetStringSetting(L"noHotkey");
    wcsncpy_s(out.noHotkey, *s ? s : L"", _TRUNCATE);
    Wh_FreeStringSetting(s);

}

//------------------------------------------------------------------------------
// Logging
//------------------------------------------------------------------------------

static void LogV(const WCHAR* fmt, va_list args, BOOL toDebugger) {
    WCHAR buf[1024];
    _vsnwprintf_s(buf, ARRAYSIZE(buf), _TRUNCATE, fmt, args);
    Wh_Log(L"[win7uac] %s", buf);
    if (toDebugger) {
        // One call and not three: every OutputDebugString is an event that holds this process
        // until whoever is listening has taken it, and inside `consent.exe` that is time taken
        // away from a prompt that is waiting to be shown.
        WCHAR line[1152];
        _snwprintf_s(line, ARRAYSIZE(line), _TRUNCATE, L"[win7uac] %s\n", buf);
        OutputDebugStringW(line);
    }
}

static void Dbg(const WCHAR* fmt, ...) {
    va_list a; va_start(a, fmt);
    LogV(fmt, a, g_settings.debugLog);
    va_end(a);
}

static void Log(const WCHAR* fmt, ...) {
    va_list a; va_start(a, fmt);
    LogV(fmt, a, TRUE);
    va_end(a);
}

// The same log without the debug output. The exception handler uses it, because a logger that
// raised the exception it is reporting would go on reporting itself for ever.
static void LogQuiet(const WCHAR* fmt, ...) {
    va_list a; va_start(a, fmt);
    LogV(fmt, a, FALSE);
    va_end(a);
}

// The snapshot that is currently in use. Built on the heap and published with a single store: the
// threads that are reading the previous one keep reading a whole struct, never a half-written one.
static void LoadSettings() {
    Settings* next = new (std::nothrow) Settings();
    if (!next) {
        Log(L"out of memory: the settings could not be read - the previous snapshot stays in use");
        return;
    }
    FillSettings(*next);
    InterlockedExchangePointer((PVOID volatile*)&g_settingsPublished, (PVOID)next);
}

//------------------------------------------------------------------------------
// Small helpers
//------------------------------------------------------------------------------

static std::wstring ToLower(const std::wstring& s) {
    std::wstring r = s;
    for (auto& c : r) c = towlower(c);
    return r;
}

static bool ContainsI(const std::wstring& haystack, const std::wstring& needle) {
    if (needle.empty()) return false;
    return ToLower(haystack).find(ToLower(needle)) != std::wstring::npos;
}

static bool EqualsI(const std::wstring& a, const std::wstring& b) {
    return !a.empty() && _wcsicmp(a.c_str(), b.c_str()) == 0;
}

static std::wstring Trim(const std::wstring& s) {
    size_t b = s.find_first_not_of(L" \t\r\n");
    size_t e = s.find_last_not_of(L" \t\r\n");
    if (b == std::wstring::npos) return L"";
    return s.substr(b, e - b + 1);
}

static std::wstring Basename(const std::wstring& path) {
    size_t p = path.find_last_of(L"\\/");
    return (p == std::wstring::npos) ? path : path.substr(p + 1);
}

//------------------------------------------------------------------------------
// Localization of the chrome (the prompt content comes from the native UI)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Crash telemetry (LOGGING ONLY)
//
// A vectored exception handler is installed when debugLog is on. It writes the exception
// code, the faulting module+RVA and a short stack trace to the log and then lets the
// exception continue its normal course: it never handles it, never swallows it and changes
// nothing about the consent flow. It exists so a crash can be located from the log alone.
//
// Two rules keep it honest and cheap. Informational exceptions - the ones Windows itself uses
// as a channel rather than as a fault - are counted and never logged; and a fault is reported
// once per place it happens, not once per occurrence.
//------------------------------------------------------------------------------

static const WCHAR* g_step = L"init";

static void FormatAddress(void* addr, WCHAR* out, size_t cch) {
    HMODULE mods[512];
    DWORD needed = 0;
    if (EnumProcessModules(GetCurrentProcess(), mods, sizeof(mods), &needed)) {
        int n = (int)(needed / sizeof(HMODULE));
        for (int i = 0; i < n && i < (int)ARRAYSIZE(mods); i++) {
            MODULEINFO mi = {};
            if (!GetModuleInformation(GetCurrentProcess(), mods[i], &mi, sizeof(mi))) continue;
            BYTE* base = (BYTE*)mi.lpBaseOfDll;
            BYTE* a = (BYTE*)addr;
            if (a >= base && a < base + mi.SizeOfImage) {
                WCHAR name[MAX_PATH] = L"";
                GetModuleFileNameW(mods[i], name, ARRAYSIZE(name));
                swprintf_s(out, cch, L"%s+0x%llX", Basename(name).c_str(),
                           (unsigned long long)(a - base));
                return;
            }
        }
    }
    swprintf_s(out, cch, L"%p (module unknown)", addr);
}

static USHORT(WINAPI* g_captureStack)(ULONG, ULONG, PVOID*, PULONG) = nullptr;

// The exception codes that are not crashes. Windows raises an exception to hand the text of
// `OutputDebugString` to whoever is listening (0x40010006 and 0x4001000A) - and this mod's own
// log goes that way on every single line; the C++ runtime raises 0xE06D7363 for every `throw`
// it is about to handle; a debugger's breakpoint and single step are 0x80000003 and 0x80000004
// (an RPC call that failed is unwound with an exception too, and gets its own one-line report,
// because that failure is exactly what a report about a prompt that never appeared needs).
// Reporting those as
// crashes is not a diagnosis, it is noise: on one machine it turned a log of 24 real lines into
// 584 lines that said `exception`.
static bool ExceptionIsInformational(DWORD code) {
    if (code >= 0x40010000u && code <= 0x400100FFu) return true;   // OutputDebugString's own
    if (code == 0x406D1388u) return true;                          // SetThreadName
    if (code == 0xE06D7363u) return true;                          // a C++ exception
    if (code == 0x80000003u || code == 0x80000004u) return true;   // breakpoint, single step
    return false;
}

// One report per fault *site*, not per fault: a fault that repeats fifty times is one sentence of
// information, and fifty identical stack traces are none. Sixteen distinct places is more than
// any diagnosis of this mod has needed; past that the logger falls silent rather than fill the
// log again.
static bool FaultSiteAlreadyReported(DWORD code, void* addr) {
    static struct { DWORD code; void* addr; } seen[16] = {};
    static int count = 0;
    for (int i = 0; i < count; i++)
        if (seen[i].code == code && seen[i].addr == addr) return true;
    if (count >= (int)ARRAYSIZE(seen)) return true;
    seen[count].code = code;
    seen[count].addr = addr;
    count++;
    return false;
}

// The names Windows itself gives to the codes that can end up in this log: a line saying
// `0x000006C6` is a puzzle, the same line saying `RPC_S_INVALID_BOUND` is a diagnosis - and it is
// the same string the error dialog the user is looking at carries.
static const WCHAR* ErrorCodeName(DWORD code) {
    switch (code) {
    case 0x00000000u: return L"ERROR_SUCCESS";
    case 0x00000057u: return L"ERROR_INVALID_PARAMETER";
    case 0x000004C7u: return L"ERROR_CANCELLED (the prompt was answered with No, or closed)";
    case 0x000006A4u: return L"RPC_S_INVALID_STRING_BINDING";
    case 0x000006B5u: return L"RPC_S_CALL_FAILED";
    case 0x000006B9u: return L"RPC_S_CALL_PENDING";
    case 0x000006BAu: return L"RPC_S_SERVER_UNAVAILABLE";
    case 0x000006C6u: return L"RPC_S_INVALID_BOUND";
    case 0x000006D1u: return L"RPC_S_UNKNOWN_IF";
    case 0x00000706u: return L"RPC_S_INVALID_OBJECT";
    case 0x80004005u: return L"E_FAIL";
    case 0x80070005u: return L"E_ACCESSDENIED";
    case 0x80070057u: return L"E_INVALIDARG";
    case 0x40010006u: return L"DBG_PRINTEXCEPTION_C (OutputDebugString)";
    case 0x4001000Au: return L"DBG_PRINTEXCEPTION_WIDE_C (OutputDebugString)";
    case 0x406D1388u: return L"MS_VC_EXCEPTION (SetThreadName)";
    case 0xE06D7363u: return L"a C++ exception being thrown";
    case 0xC0000005u: return L"EXCEPTION_ACCESS_VIOLATION";
    case 0xC000001Du: return L"EXCEPTION_ILLEGAL_INSTRUCTION";
    case 0xC000008Cu: return L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
    case 0xC0000094u: return L"EXCEPTION_INT_DIVIDE_BY_ZERO";
    case 0xC00000FDu: return L"EXCEPTION_STACK_OVERFLOW";
    case 0xC0000374u: return L"STATUS_HEAP_CORRUPTION";
    case 0xC0000409u: return L"STATUS_STACK_BUFFER_OVERRUN";
    default: return L"";
    }
}

// The RPC error range (1700..1800). A call that fails is unwound by the RPC runtime with an
// exception: it is a failure of a call, never a crash of the process, and the log says which.
static bool ExceptionCodeIsRpc(DWORD code) {
    return code >= 0x000006A4u && code <= 0x0000070Fu;
}

static volatile LONG g_exceptionsIgnored = 0;

static LONG CALLBACK ExceptionLogger(EXCEPTION_POINTERS* ep) {
    static volatile LONG busy = 0;
    if (InterlockedCompareExchange(&busy, 1, 0) != 0) return EXCEPTION_CONTINUE_SEARCH;
    if (ep && ep->ExceptionRecord) {
        const DWORD code = ep->ExceptionRecord->ExceptionCode;
        void* addr = ep->ExceptionRecord->ExceptionAddress;
        if (ExceptionIsInformational(code)) {
            // By far the common path - every line this mod logs arrives here - so nothing is done
            // on it but the count: no module walk, no stack, no logging, no debug output.
            InterlockedIncrement(&g_exceptionsIgnored);
        } else if (!FaultSiteAlreadyReported(code, addr)) {
            WCHAR where[320] = L"";
            FormatAddress(addr, where, ARRAYSIZE(where));
            const WCHAR* name = ErrorCodeName(code);
            const long ignored = (long)InterlockedCompareExchange(&g_exceptionsIgnored, 0, 0);
            // LogQuiet, never Log: OutputDebugString from inside this handler would raise the
            // very exception being reported. An RPC call that failed is not a crash and is not
            // labelled as one - it is the line that names the error the user may be seeing.
            if (ExceptionCodeIsRpc(code))
                LogQuiet(L"[rpc] the RPC runtime unwound a failed call: 0x%08X %s at %s, last "
                         L"step: %s (%ld informational exceptions passed over)", code,
                         *name ? name : L"(no name for this code)", where, g_step, ignored);
            else
                LogQuiet(L"[crash] exception 0x%08X %s at %s, last step: %s (%ld informational "
                         L"exceptions passed over before this one)", code,
                         *name ? name : L"(no name for this code)", where, g_step, ignored);

            if (!g_captureStack) {
                HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
                if (ntdll)
                    g_captureStack = (USHORT(WINAPI*)(ULONG, ULONG, PVOID*, PULONG))
                        GetProcAddress(ntdll, "RtlCaptureStackBackTrace");
            }
            if (g_captureStack) {
                void* frames[16] = {};
                USHORT n = g_captureStack(0, ARRAYSIZE(frames), frames, nullptr);
                for (USHORT i = 0; i < n && i < 8; i++) {
                    WCHAR line[320] = L"";
                    FormatAddress(frames[i], line, ARRAYSIZE(line));
                    LogQuiet(L"[crash]   frame %u: %s", (unsigned)i, line);
                }
            }
        }
    }
    InterlockedExchange(&busy, 0);
    return EXCEPTION_CONTINUE_SEARCH;
}

static PVOID g_excLogger = nullptr;

static void InstallExceptionLogger() {
    if (!g_excLogger) g_excLogger = AddVectoredExceptionHandler(1, ExceptionLogger);
}

static void RemoveExceptionLogger() {
    if (g_excLogger) {
        RemoveVectoredExceptionHandler(g_excLogger);
        g_excLogger = nullptr;
    }
}

struct LocStrings {
    const WCHAR* lang;
    const WCHAR* title;
    const WCHAR* programName;
    const WCHAR* publisher;
    const WCHAR* origin;
    const WCHAR* showDetails;
    const WCHAR* hideDetails;
    const WCHAR* helpDecide;
    const WCHAR* changeNotifications;
    const WCHAR* question;
    const WCHAR* programLocation;   // the row that carries the path inside the details pane
    const WCHAR* noExtraInfo;       // shown when the prompt has no further information at all
};

static const LocStrings kLoc[] = {
    {L"en", L"User Account Control",
     L"Program name:", L"Verified publisher:", L"File origin:",
     L"Show details", L"Hide details",
     L"Help me decide", L"Change when these notifications appear",
     L"Do you want to allow the following program to make changes to this computer?",
     L"Program location:", L"No further information is available for this request."},
    {L"it", L"Controllo account utente",
     L"Nome programma:", L"Editore verificato:", L"Origine file:",
     L"Mostra dettagli", L"Nascondi dettagli",
     L"Aiutami a decidere", L"Modifica quando vengono visualizzate le notifiche",
     L"Consentire a questo programma di apportare modifiche al computer?",
     L"Percorso del programma:", L"Per questa richiesta non sono disponibili altre informazioni."},
    {L"de", L"Benutzerkontensteuerung",
     L"Programmname:", L"Verifizierter Herausgeber:", L"Dateiursprung:",
     L"Details anzeigen", L"Details ausblenden",
     L"Entscheidungshilfe", L"Aendern, wann Benachrichtigungen angezeigt werden",
     L"Moechten Sie zulassen, dass dieses Programm Aenderungen an diesem Computer vornimmt?",
     L"Programmspeicherort:", L"F\u00fcr diese Anforderung sind keine weiteren Informationen verf\u00fcgbar."},
    {L"fr", L"Contr\u00f4le de compte d'utilisateur",
     L"Nom du programme :", L"\u00c9diteur v\u00e9rifi\u00e9 :", L"Origine du fichier :",
     L"Afficher les d\u00e9tails", L"Masquer les d\u00e9tails",
     L"Aide-moi \u00e0 d\u00e9cider", L"Modifier le moment o\u00f9 ces notifications apparaissent",
     L"Voulez-vous autoriser ce programme \u00e0 apporter des modifications \u00e0 cet ordinateur ?",
     L"Emplacement du programme :", L"Aucune information suppl\u00e9mentaire n'est disponible pour cette demande."},
    {L"es", L"Control de cuentas de usuario",
     L"Nombre del programa:", L"Editor comprobado:", L"Origen del archivo:",
     L"Mostrar detalles", L"Ocultar detalles",
     L"Ay\u00fademe a decidir", L"Cambiar cu\u00e1ndo aparecen estas notificaciones",
     L"\u00bfDesea permitir que este programa haga cambios en este equipo?",
     L"Ubicaci\u00f3n del programa:", L"No hay informaci\u00f3n adicional disponible para esta solicitud."},
    {L"pt", L"Controlo de conta de utilizador",
     L"Nome do programa:", L"Editor verificado:", L"Origem do ficheiro:",
     L"Mostrar detalhes", L"Ocultar detalhes",
     L"Ajudar-me a decidir", L"Alterar quando estas notifica\u00e7\u00f5es aparecem",
     L"Pretende permitir que este programa fa\u00e7a altera\u00e7\u00f5es ao computador?",
     L"Localiza\u00e7\u00e3o do programa:", L"N\u00e3o est\u00e3o dispon\u00edveis mais informa\u00e7\u00f5es para este pedido."},
    {L"nl", L"Gebruikersaccountbeheer",
     L"Programmanaam:", L"Geverifieerde uitgever:", L"Herkomst van bestand:",
     L"Details weergeven", L"Details verbergen",
     L"Help mij beslissen", L"Wijzigen wanneer deze meldingen worden weergegeven",
     L"Wilt u toestaan dat dit programma wijzigingen aanbrengt op deze computer?",
     L"Locatie van programma:", L"Er zijn geen verdere gegevens beschikbaar voor deze aanvraag."},
    {L"pl", L"Kontrola konta u\u017cytkownika",
     L"Nazwa programu:", L"Zweryfikowany wydawca:", L"Pochodzenie pliku:",
     L"Poka\u017c szczeg\u00f3\u0142y", L"Ukryj szczeg\u00f3\u0142y",
     L"Pomoc w podj\u0119ciu decyzji", L"Zmie\u0144, kiedy te powiadomienia maj\u0105 by\u0107 wy\u015bwietlane",
     L"Czy chcesz zezwoli\u0107 temu programowi na wprowadzanie zmian na tym komputerze?",
     L"Lokalizacja programu:", L"Brak dodatkowych informacji dla tego \u017c\u0105dania."},
    {L"ru", L"\u041a\u043e\u043d\u0442\u0440\u043e\u043b\u044c \u0443\u0447\u0451\u0442\u043d\u044b\u0445 \u0437\u0430\u043f\u0438\u0441\u0435\u0439",
     L"\u0418\u043c\u044f \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u044b:", L"\u041f\u0440\u043e\u0432\u0435\u0440\u0435\u043d\u043d\u044b\u0439 \u0438\u0437\u0434\u0430\u0442\u0435\u043b\u044c:", L"\u0418\u0441\u0442\u043e\u0447\u043d\u0438\u043a \u0444\u0430\u0439\u043b\u0430:",
     L"\u041f\u043e\u043a\u0430\u0437\u0430\u0442\u044c \u043f\u043e\u0434\u0440\u043e\u0431\u043d\u043e\u0441\u0442\u0438", L"\u0421\u043a\u0440\u044b\u0442\u044c \u043f\u043e\u0434\u0440\u043e\u0431\u043d\u043e\u0441\u0442\u0438",
     L"\u041f\u043e\u043c\u043e\u0447\u044c \u043c\u043d\u0435 \u043f\u0440\u0438\u043d\u044f\u0442\u044c \u0440\u0435\u0448\u0435\u043d\u0438\u0435", L"\u0418\u0437\u043c\u0435\u043d\u0438\u0442\u044c \u0432\u0440\u0435\u043c\u044f \u043f\u043e\u044f\u0432\u043b\u0435\u043d\u0438\u044f \u044d\u0442\u0438\u0445 \u0443\u0432\u0435\u0434\u043e\u043c\u043b\u0435\u043d\u0438\u0439",
     L"\u0420\u0430\u0437\u0440\u0435\u0448\u0438\u0442\u044c \u044d\u0442\u043e\u043c\u0443 \u043f\u0440\u0438\u043b\u043e\u0436\u0435\u043d\u0438\u044e \u0432\u043d\u043e\u0441\u0438\u0442\u044c \u0438\u0437\u043c\u0435\u043d\u0435\u043d\u0438\u044f \u043d\u0430 \u044d\u0442\u043e\u043c \u043a\u043e\u043c\u043f\u044c\u044e\u0442\u0435\u0440\u0435?",
     L"\u0420\u0430\u0441\u043f\u043e\u043b\u043e\u0436\u0435\u043d\u0438\u0435 \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u044b:", L"\u0414\u043e\u043f\u043e\u043b\u043d\u0438\u0442\u0435\u043b\u044c\u043d\u044b\u0435 \u0441\u0432\u0435\u0434\u0435\u043d\u0438\u044f \u0434\u043b\u044f \u044d\u0442\u043e\u0433\u043e \u0437\u0430\u043f\u0440\u043e\u0441\u0430 \u043e\u0442\u0441\u0443\u0442\u0441\u0442\u0432\u0443\u044e\u0442."},
    // fa
    {L"fa", L"\u06a9\u0646\u062a\u0631\u0644 \u062d\u0633\u0627\u0628 \u06a9\u0627\u0631\u0628\u0631\u06cc",
     L"\u0646\u0627\u0645 \u0628\u0631\u0646\u0627\u0645\u0647:", L"\u0646\u0627\u0634\u0631 \u062a\u0623\u06cc\u06cc\u062f\u0634\u062f\u0647:", L"\u0645\u0628\u062f\u0623 \u0641\u0627\u06cc\u0644:",
     L"\u0646\u0645\u0627\u06cc\u0634 \u062c\u0632\u0626\u06cc\u0627\u062a", L"\u067e\u0646\u0647\u0627\u0646 \u06a9\u0631\u062f\u0646 \u062c\u0632\u0626\u06cc\u0627\u062a",
     L"\u0628\u0647 \u0645\u0646 \u062f\u0631 \u062a\u0635\u0645\u06cc\u0645\u200c\u06af\u06cc\u0631\u06cc \u06a9\u0645\u06a9 \u06a9\u0646", L"\u062a\u063a\u06cc\u06cc\u0631 \u0632\u0645\u0627\u0646 \u0646\u0645\u0627\u06cc\u0634 \u0627\u06cc\u0646 \u0627\u0639\u0644\u0627\u0646\u200c\u0647\u0627",
     L"\u0622\u06cc\u0627 \u0645\u06cc\u200c\u062e\u0648\u0627\u0647\u06cc\u062f \u0628\u0647 \u0627\u06cc\u0646 \u0628\u0631\u0646\u0627\u0645\u0647 \u0627\u062c\u0627\u0632\u0647 \u062f\u0647\u06cc\u062f \u062a\u063a\u06cc\u06cc\u0631\u0627\u062a\u06cc \u062f\u0631 \u0627\u06cc\u0646 \u0631\u0627\u06cc\u0627\u0646\u0647 \u0627\u06cc\u062c\u0627\u062f \u06a9\u0646\u062f\u061f",
     L"\u0645\u062d\u0644 \u0628\u0631\u0646\u0627\u0645\u0647:", L"\u0627\u0637\u0644\u0627\u0639\u0627\u062a \u062f\u06cc\u06af\u0631\u06cc \u0628\u0631\u0627\u06cc \u0627\u06cc\u0646 \u062f\u0631\u062e\u0648\u0627\u0633\u062a \u0645\u0648\u062c\u0648\u062f \u0646\u06cc\u0633\u062a."},
    // ar
    {L"ar", L"\u0627\u0644\u062a\u062d\u0643\u0645 \u0641\u064a \u062d\u0633\u0627\u0628 \u0627\u0644\u0645\u0633\u062a\u062e\u062f\u0645",
     L"\u0627\u0633\u0645 \u0627\u0644\u0628\u0631\u0646\u0627\u0645\u062c:", L"\u0627\u0644\u0646\u0627\u0634\u0631 \u0627\u0644\u0645\u0639\u062a\u0645\u062f:", L"\u0645\u0635\u062f\u0631 \u0627\u0644\u0645\u0644\u0641:",
     L"\u0625\u0638\u0647\u0627\u0631 \u0627\u0644\u062a\u0641\u0627\u0635\u064a\u0644", L"\u0625\u062e\u0641\u0627\u0621 \u0627\u0644\u062a\u0641\u0627\u0635\u064a\u0644",
     L"\u0645\u0633\u0627\u0639\u062f\u062a\u064a \u0641\u064a \u0627\u062a\u062e\u0627\u0630 \u0627\u0644\u0642\u0631\u0627\u0631", L"\u062a\u063a\u064a\u064a\u0631 \u0648\u0642\u062a \u0638\u0647\u0648\u0631 \u0647\u0630\u0647 \u0627\u0644\u0625\u0634\u0639\u0627\u0631\u0627\u062a",
     L"\u0647\u0644 \u062a\u0631\u064a\u062f \u0627\u0644\u0633\u0645\u0627\u062d \u0644\u0647\u0630\u0627 \u0627\u0644\u0628\u0631\u0646\u0627\u0645\u062c \u0628\u0625\u062c\u0631\u0627\u0621 \u062a\u063a\u064a\u064a\u0631\u0627\u062a \u0639\u0644\u0649 \u0647\u0630\u0627 \u0627\u0644\u0643\u0645\u0628\u064a\u0648\u062a\u0631\u061f",
     L"\u0645\u0648\u0642\u0639 \u0627\u0644\u0628\u0631\u0646\u0627\u0645\u062c:", L"\u0644\u0627 \u062a\u062a\u0648\u0641\u0631 \u0645\u0639\u0644\u0648\u0645\u0627\u062a \u0625\u0636\u0627\u0641\u064a\u0629 \u0644\u0647\u0630\u0627 \u0627\u0644\u0637\u0644\u0628."},
    // tr
    {L"tr", L"Kullan\u0131c\u0131 Hesab\u0131 Denetimi",
     L"Program ad\u0131:", L"Do\u011frulanm\u0131\u015f yay\u0131mc\u0131:", L"Dosya kayna\u011f\u0131:",
     L"Ayr\u0131nt\u0131lar\u0131 g\u00f6ster", L"Ayr\u0131nt\u0131lar\u0131 gizle",
     L"Karar vermemde yard\u0131mc\u0131 ol", L"Bu bildirimlerin ne zaman g\u00f6r\u00fcnt\u00fclenece\u011fini de\u011fi\u015ftir",
     L"Bu program\u0131n bu bilgisayarda de\u011fi\u015fiklik yapmas\u0131na izin vermek istiyor musunuz?",
     L"Program konumu:", L"Bu iste\u011fe ili\u015fkin ba\u015fka bilgi yok."},
    // ja
    {L"ja", L"\u30e6\u30fc\u30b6\u30fc \u30a2\u30ab\u30a6\u30f3\u30c8\u5236\u5fa1",
     L"\u30d7\u30ed\u30b0\u30e9\u30e0\u540d:", L"\u78ba\u8a8d\u6e08\u307f\u306e\u767a\u884c\u5143:", L"\u30d5\u30a1\u30a4\u30eb\u306e\u767a\u884c\u5143:",
     L"\u8a73\u7d30\u3092\u8868\u793a", L"\u8a73\u7d30\u3092\u975e\u8868\u793a",
     L"\u5224\u65ad\u306e\u30d8\u30eb\u30d7", L"\u901a\u77e5\u3092\u8868\u793a\u3059\u308b\u30bf\u30a4\u30df\u30f3\u30b0\u306e\u5909\u66f4",
     L"\u3053\u306e\u30d7\u30ed\u30b0\u30e9\u30e0\u304c\u3053\u306e\u30b3\u30f3\u30d4\u30e5\u30fc\u30bf\u30fc\u306b\u5909\u66f4\u3092\u52a0\u3048\u308b\u3053\u3068\u3092\u8a31\u53ef\u3057\u307e\u3059\u304b?",
     L"\u30d7\u30ed\u30b0\u30e9\u30e0\u306e\u5834\u6240:", L"\u3053\u306e\u8981\u6c42\u306b\u95a2\u3059\u308b\u8a73\u7d30\u60c5\u5831\u306f\u3042\u308a\u307e\u305b\u3093\u3002"},
    // ko
    {L"ko", L"\uc0ac\uc6a9\uc790 \uacc4\uc815 \ucee8\ud2b8\ub864",
     L"\ud504\ub85c\uadf8\ub7a8 \uc774\ub984:", L"\ud655\uc778\ub41c \uac8c\uc2dc\uc790:", L"\ud30c\uc77c \uc6d0\ubcf8:",
     L"\uc790\uc138\ud788 \ud45c\uc2dc", L"\uc790\uc138\ud788 \uc228\uae30\uae30",
     L"\uacb0\uc815\ud558\ub294 \ub370 \ub3c4\uc6c0 \ubc1b\uae30", L"\uc774 \uc54c\ub9bc\uc774 \ud45c\uc2dc\ub418\ub294 \uc2dc\uc810 \ubcc0\uacbd",
     L"\uc774 \ud504\ub85c\uadf8\ub7a8\uc774 \ucef4\ud4e8\ud130\ub97c \ubcc0\uacbd\ud558\ub3c4\ub85d \ud5c8\uc6a9\ud558\uc2dc\uaca0\uc2b5\ub2c8\uae4c?",
     L"\ud504\ub85c\uadf8\ub7a8 \uc704\uce58:", L"\uc774 \uc694\uccad\uc5d0 \ub300\ud55c \ucd94\uac00 \uc815\ubcf4\uac00 \uc5c6\uc2b5\ub2c8\ub2e4."},
    // hi
    {L"hi", L"\u0909\u092a\u092f\u094b\u0917\u0915\u0930\u094d\u0924\u093e \u0916\u093e\u0924\u093e \u0928\u093f\u092f\u0902\u0924\u094d\u0930\u0923",
     L"\u092a\u094d\u0930\u094b\u0917\u094d\u0930\u093e\u092e \u0928\u093e\u092e:", L"\u0938\u0924\u094d\u092f\u093e\u092a\u093f\u0924 \u092a\u094d\u0930\u0915\u093e\u0936\u0915:", L"\u092b\u093c\u093e\u0907\u0932 \u0938\u094d\u0930\u094b\u0924:",
     L"\u0935\u093f\u0935\u0930\u0923 \u0926\u093f\u0916\u093e\u090f\u0901", L"\u0935\u093f\u0935\u0930\u0923 \u091b\u093f\u092a\u093e\u090f\u0901",
     L"\u092e\u0941\u091d\u0947 \u0928\u093f\u0930\u094d\u0923\u092f \u0932\u0947\u0928\u0947 \u092e\u0947\u0902 \u0938\u0939\u093e\u092f\u0924\u093e \u0915\u0930\u0947\u0902", L"\u092c\u0926\u0932\u0947\u0902 \u0915\u093f \u092f\u0947 \u0938\u0942\u091a\u0928\u093e\u090f\u0901 \u0915\u092c \u0926\u093f\u0916\u093e\u0908 \u0926\u0947\u0902",
     L"\u0915\u094d\u092f\u093e \u0906\u092a \u0907\u0938 \u092a\u094d\u0930\u094b\u0917\u094d\u0930\u093e\u092e \u0915\u094b \u0907\u0938 \u0915\u0902\u092a\u094d\u092f\u0942\u091f\u0930 \u092e\u0947\u0902 \u092a\u0930\u093f\u0935\u0930\u094d\u0924\u0928 \u0915\u0930\u0928\u0947 \u0915\u0940 \u0905\u0928\u0941\u092e\u0924\u093f \u0926\u0947\u0928\u093e \u091a\u093e\u0939\u0924\u0947 \u0939\u0948\u0902?",
     L"\u092a\u094d\u0930\u094b\u0917\u094d\u0930\u093e\u092e \u0938\u094d\u0925\u093e\u0928:", L"\u0907\u0938 \u0905\u0928\u0941\u0930\u094b\u0927 \u0915\u0947 \u0932\u093f\u090f \u0915\u094b\u0908 \u0905\u0924\u093f\u0930\u093f\u0915\u094d\u0924 \u091c\u093e\u0928\u0915\u093e\u0930\u0940 \u0909\u092a\u0932\u092c\u094d\u0927 \u0928\u0939\u0940\u0902 \u0939\u0948\u0964"},
};

static const LocStrings* g_loc = &kLoc[0];

// Persian and Arabic are written right to left. The texts are drawn with the reading direction
// they expect; the dialog itself keeps the left-to-right layout of the Windows 7 original, which
// the readme says out loud rather than pretending to mirror it.
static bool LocIsRtl() {
    if (!g_loc) return false;
    return _wcsicmp(g_loc->lang, L"fa") == 0 || _wcsicmp(g_loc->lang, L"ar") == 0 ||
           _wcsicmp(g_loc->lang, L"he") == 0 || _wcsicmp(g_loc->lang, L"ur") == 0;
}

// The alignment a block of text is drawn with in the language of the dialog.
static UINT LocTextFlags(UINT flags) {
    if (!LocIsRtl()) return flags;
    return (flags & ~((UINT)DT_LEFT)) | DT_RIGHT | DT_RTLREADING;
}

static void InitLocale() {
    WCHAR primary[8] = L"";
    if (GetLocaleInfoW(GetUserDefaultUILanguage(), LOCALE_SISO639LANGNAME, primary,
                       ARRAYSIZE(primary))) {
        for (const auto& l : kLoc) {
            if (_wcsicmp(l.lang, primary) == 0) { g_loc = &l; break; }
        }
    }
    Dbg(L"UI language: %s", g_loc->lang);
}

// "Verified publisher:" only when Windows' own row says so; otherwise the plain "Publisher:".
static const WCHAR* PublisherLabelFor(bool verified) {
    if (!g_loc) return L"Publisher:";
    if (verified) return g_loc->publisher;
    const WCHAR* l = g_loc->lang;
    if (!_wcsicmp(l, L"it")) return L"Editore:";
    if (!_wcsicmp(l, L"de")) return L"Herausgeber:";
    if (!_wcsicmp(l, L"fr")) return L"\u00c9diteur :";
    if (!_wcsicmp(l, L"es")) return L"Editor:";
    if (!_wcsicmp(l, L"pt")) return L"Editor:";
    if (!_wcsicmp(l, L"nl")) return L"Uitgever:";
    if (!_wcsicmp(l, L"pl")) return L"Wydawca:";
    if (!_wcsicmp(l, L"ru")) return L"\u0418\u0437\u0434\u0430\u0442\u0435\u043b\u044c:";
    if (!_wcsicmp(l, L"fa")) return L"\u0646\u0627\u0634\u0631:";
    if (!_wcsicmp(l, L"ar")) return L"\u0627\u0644\u0646\u0627\u0634\u0631:";
    if (!_wcsicmp(l, L"tr")) return L"Yay\u0131mc\u0131:";
    if (!_wcsicmp(l, L"ja")) return L"\u767a\u884c\u5143:";
    if (!_wcsicmp(l, L"ko")) return L"\uac8c\uc2dc\uc790:";
    if (!_wcsicmp(l, L"hi")) return L"\u092a\u094d\u0930\u0915\u093e\u0936\u0915:";
    return L"Publisher:";
}

//------------------------------------------------------------------------------
// Desktop helpers
//------------------------------------------------------------------------------

static const WCHAR* kDesktopDefault = L"Default";
static const WCHAR* kDesktopWinlogon = L"Winlogon";

// Windows only: the desktop is opened for objects, windows and hooks. The journal and
// SwitchDesktop rights the first version asked for are gone - nothing here records or
// replays input, and nothing here switches desktops (the prompt's own thread does that).
#define DESKTOP_MOD_ACCESS (DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW | DESKTOP_CREATEMENU | \
                            DESKTOP_HOOKCONTROL | DESKTOP_ENUMERATE | \
                            DESKTOP_WRITEOBJECTS | STANDARD_RIGHTS_REQUIRED)

static void GetDesktopName(HDESK hDesk, WCHAR* out, size_t cch) {
    out[0] = 0;
    if (!hDesk) return;
    DWORD need = 0;
    GetUserObjectInformationW(hDesk, UOI_NAME, out, (DWORD)(cch * sizeof(WCHAR)), &need);
}

static HDESK OpenDesktopByName(const WCHAR* name) {
    HDESK desk = OpenDesktopW(name, 0, FALSE, DESKTOP_MOD_ACCESS);
    if (!desk) Dbg(L"OpenDesktop('%s') failed: %u", name, GetLastError());
    return desk;
}

static std::wstring GetProcessImageName(DWORD pid) {
    std::wstring name;
    if (!pid) return name;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return name;
    WCHAR buf[MAX_PATH] = L"";
    DWORD size = ARRAYSIZE(buf);
    if (QueryFullProcessImageNameW(h, 0, buf, &size)) name = Basename(buf);
    CloseHandle(h);
    return name;
}

//------------------------------------------------------------------------------
// Shared state
//------------------------------------------------------------------------------

#define WIN7UAC_CLASS L"Win7UacRecreationWnd"
#define WM_PROMPT_STARTED (WM_APP + 1)
// Posted to the dialog by the event hook that follows the native prompt (see PromptStateProc). A
// message and not a direct call: the check then runs on the thread that owns the window.
#define WM_W7UAC_STATECHANGED (WM_APP + 2)

struct PromptCandidate {
    HWND hwnd;
    DWORD pid;
    BOOL ours;              // window of consent.exe itself (the only kind we may drive)
    WCHAR cls[128];
    WCHAR title[256];
    HDESK desktop;
    WCHAR desktopName[32];
};

struct WatcherCtx {
    const WCHAR* name;
    HDESK desktop;
    DWORD tid;
    HANDLE thread;
    HWINEVENTHOOK hook;
};

static WatcherCtx g_watchers[2] = { {kDesktopDefault}, {kDesktopWinlogon} };

static CRITICAL_SECTION g_cs;
// These three are written by the watcher and read by the session under g_cs, and they are emptied
// in the unload path (see ReleaseGlobalState): their destructors must not run at process exit,
// where static destruction happens under the loader lock.
[[clang::no_destroy]] static std::vector<PromptCandidate> g_candidates;
[[clang::no_destroy]] static std::wstring g_userDesktopApp;        // main window of the user desktop
[[clang::no_destroy]] static std::wstring g_userDesktopAppClass;
static volatile LONG g_userDesktopAppReady = 0;

static HANDLE g_stopEvent = nullptr;
static bool StopRequested() {
    return g_stopEvent && WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
}
static bool StopWait(DWORD ms) {
    return g_stopEvent && WaitForSingleObject(g_stopEvent, ms) == WAIT_OBJECT_0;
}

// Every thread this mod creates is registered here, handle and all, and waited for before the DLL
// can be unloaded: a thread that outlives the code it runs is the one crash this mod must never
// have. g_threadsActive is the count of threads that have not returned yet - the unload path
// reports it when it is not zero, which turns a silent unload bug into a log line.
[[clang::no_destroy]] static std::vector<HANDLE> g_modThreads;
static volatile LONG g_threadsActive = 0;

static void TrackModThread(HANDLE thread, const WCHAR*) {
    if (!thread) return;
    EnterCriticalSection(&g_cs);
    g_modThreads.push_back(thread);
    LeaveCriticalSection(&g_cs);
}

static void WaitForModThreads() {
    // Windhawk removes the hooks before Uninit; the stop gate prevents new sessions.
    std::vector<HANDLE> threads;
    EnterCriticalSection(&g_cs);
    threads.swap(g_modThreads);
    LeaveCriticalSection(&g_cs);
    for (HANDLE h : threads) {
        if (WaitForSingleObject(h, INFINITE) != WAIT_OBJECT_0)
            Log(L"thread join failed: %u", GetLastError());
        CloseHandle(h);
    }
}

static CRITICAL_SECTION g_nativeCs;
struct NativeLock {
    NativeLock() { EnterCriticalSection(&g_nativeCs); }
    ~NativeLock() { LeaveCriticalSection(&g_nativeCs); }
    NativeLock(const NativeLock&) = delete;
    NativeLock& operator=(const NativeLock&) = delete;
};

static volatile LONG g_inHook = 0;             // a consent prompt is in flight
static volatile LONG g_originalReturned = 0;   // the hooked API returned
static volatile LONG g_promptGivenBack = 0;
// True as soon as the native call has returned: from then on there is no prompt to replace,
// whatever the reason (the user decided, or the prompt failed to appear at all).
static bool NativeCallFinished() {
    return InterlockedCompareExchange(&g_originalReturned, 0, 0) != 0;
}

// Windows that consent.exe creates for its own plumbing: COM/OLE marshaling and the text
// input framework. They are never the prompt, and hiding or moving one of them can break the
// very call this mod is hooked into (they belong to the RPC/OLE machinery in flight), so
// they are filtered out the moment they are seen.
static bool IsInternalHelperClass(const WCHAR* cls) {
    if (!cls || !cls[0]) return false;
    static const WCHAR* kInternal[] = {
        L"CicMarshalWndClass", L"CicMarshalWndClassEx", L"OleMainThreadWndClass",
        L"OleDDEWndClass", L"MSCTFIME UI", L"Default IME", L"IME",
        L"MSCTF.AsmListCache.FMP", L"ForegroundStaging", L"TaskListThumbnailWnd",
    };
    for (const WCHAR* k : kInternal) {
        if (wcscmp(cls, k) == 0) return true;
    }
    return wcsstr(cls, L"MSCTFIME") != nullptr;
}

// The Windows 11 consent prompt is not a classic dialog: since 22H2 it is a XAML island
// hosted in a window of class "Credential Dialog Xaml Host", and it has no Win32 child
// windows at all. Knowing the class lets the log say plainly when the real prompt appeared.
static bool IsKnownPromptClass(const WCHAR* cls) {
    if (!cls || !cls[0]) return false;
    static const WCHAR* kKnown[] = {
        L"Credential Dialog Xaml Host", L"CredDialogClass", L"#32770",
        L"Windows.UI.Core.CoreWindow",
    };
    for (const WCHAR* k : kKnown) {
        if (wcscmp(cls, k) == 0) return true;
    }
    return false;
}

// The narrower list the *instant ejection* works from. The generic classes above are there so that
// the log and the harvest recognise every shape a prompt has had; moving a window on the strength
// of one of them is a different matter, because this process also owns windows that are not the
// prompt at all - a message box is a `#32770`, and the XAML stack can carry other `CoreWindow`s.
// These two are the consent prompt's own hosts and nothing else: the XAML island of Windows 10 and
// 11 (both the 21H2 and the 24H2 builds of consent.exe use it) and the classic consent dialog.
static bool IsConsentHostClass(const WCHAR* cls) {
    return cls && (wcscmp(cls, L"Credential Dialog Xaml Host") == 0 ||
                   wcscmp(cls, L"CredDialogClass") == 0);
}

static thread_local int g_curWatcher = -1;

//------------------------------------------------------------------------------
// Prompt discovery: one WinEvent watcher per desktop
//------------------------------------------------------------------------------

// The candidate log: one line per window, at most 48 windows for one prompt. The prompt's own
// window is moved while it is taken over and every move is another event, so an ungated line here
// used to write hundreds of repetitions of the same handle - noise that hides the lines that
// matter in exactly the log a user sends in.
static HWND g_loggedCandidates[48] = {};
static int g_loggedCandidateCount = 0;

static void ResetCandidateLog() {
    g_loggedCandidateCount = 0;
    ZeroMemory(g_loggedCandidates, sizeof(g_loggedCandidates));
}

static void AddCandidate(HWND hwnd, DWORD pid, const WCHAR* cls) {
    if (g_curWatcher < 0) return;              // not one of our watcher threads
    if (!IsWindow(hwnd)) return;
    WatcherCtx& w = g_watchers[g_curWatcher];

    // The title is read before the lock is taken: GetWindowTextW can send a message to the thread
    // that owns the window (the prompt's own thread, busy in its modal loop), and no lock here is
    // ever held across a call that can wait for another thread.
    WCHAR title[256] = L"";      // the same size as PromptCandidate::title
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));

    EnterCriticalSection(&g_cs);
    if (g_candidates.size() < 32) {
        PromptCandidate c = {};
        c.hwnd = hwnd;
        c.pid = pid;
        c.ours = (pid == GetCurrentProcessId()) ? TRUE : FALSE;
        wcsncpy_s(c.cls, cls, _TRUNCATE);
        wcsncpy_s(c.title, title, _TRUNCATE);
        c.desktop = w.desktop;
        wcsncpy_s(c.desktopName, w.name, _TRUNCATE);
        g_candidates.push_back(c);
    }
    // The line is written once per window, and only while the log is not yet full: the dedup runs
    // under the same lock as the list, the printing does not. The lock taken above is still held
    // here on purpose - a CRITICAL_SECTION is recursive, so a second Enter would have gone through
    // and the single Leave below would have left the lock taken for good, with every other thread
    // waiting on it forever.
    bool logIt = false;
    bool known = false;
    for (int i = 0; i < g_loggedCandidateCount; i++) {
        if (g_loggedCandidates[i] == hwnd) {
            known = true;
            break;
        }
    }
    if (!known && g_loggedCandidateCount < (int)ARRAYSIZE(g_loggedCandidates)) {
        g_loggedCandidates[g_loggedCandidateCount++] = hwnd;
        logIt = true;
    }
    LeaveCriticalSection(&g_cs);
    if (logIt) Dbg(L"watcher[%s]: candidate hwnd=%p pid=%lu class='%s'", w.name, hwnd, pid, cls);
}

// Windows that are not ours but appeared while a prompt was in flight. They are only
// *recorded* (never enumerated, never driven) so that the dump shows whether the prompt of
// this build really lives inside consent.exe.
[[clang::no_destroy]] static std::vector<std::wstring> g_rejectLog;

static void NoteForeignWindow(HWND hwnd, DWORD pid, const WCHAR* cls) {
    EnterCriticalSection(&g_cs);
    if (g_rejectLog.size() < 40) {
        WCHAR line[320];
        swprintf_s(line, L"hwnd=%p pid=%lu class='%s'", hwnd, pid, cls);
        g_rejectLog.push_back(line);
    }
    LeaveCriticalSection(&g_cs);
}

// Same parking lot as NoteForeignWindow, for the plumbing windows of consent.exe itself.
static void NoteInternalWindow(HWND hwnd, DWORD pid, const WCHAR* cls) {
    EnterCriticalSection(&g_cs);
    if (g_rejectLog.size() < 40) {
        WCHAR line[320];
        swprintf_s(line, L"internal plumbing, never used: hwnd=%p pid=%lu class='%s'", hwnd, pid,
                   cls);
        if (std::find(g_rejectLog.begin(), g_rejectLog.end(), line) == g_rejectLog.end())
            g_rejectLog.push_back(line);
    }
    LeaveCriticalSection(&g_cs);
    Dbg(L"ignoring the consent process's own plumbing window class='%s'", cls);
}

// Prints the parked windows into the log, so that a pasted log alone shows what the consent
// flow put on its desktops: its own plumbing, and the windows of every other process.
static void LogSeenWindows(const WCHAR* what) {
    std::vector<std::wstring> copy;
    EnterCriticalSection(&g_cs);
    copy = g_rejectLog;
    LeaveCriticalSection(&g_cs);
    if (copy.empty()) return;
    size_t shown = copy.size() < 12 ? copy.size() : 12;
    Log(L"%s: %u window(s) parked, first %u shown", what, (unsigned)copy.size(),
        (unsigned)shown);
    for (size_t i = 0; i < shown; i++) Log(L"  %s", copy[i].c_str());
}

// The instant ejection. It is defined with the session bookkeeping below and declared here
// because the watcher is where this process's own prompt is first seen (EjectTheNativePromptAtOnce).
static void EjectTheNativePromptAtOnce(HWND hwnd);

static void CALLBACK WinEventProcBody(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                      LONG idChild, DWORD, DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW) return;
    if (InterlockedCompareExchange(&g_inHook, 0, 0) == 0) return;
    if (g_curWatcher < 0) return;              // not one of our watcher threads
    if (event != EVENT_OBJECT_CREATE && event != EVENT_OBJECT_SHOW &&
        event != EVENT_OBJECT_LOCATIONCHANGE)
        return;

    WCHAR cls[128] = L"";
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (cls[0] == L'\0' || wcscmp(cls, WIN7UAC_CLASS) == 0) return;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return;

    if (IsInternalHelperClass(cls)) {
        NoteInternalWindow(hwnd, pid, cls);
        return;
    }

    if (pid == GetCurrentProcessId()) {
        // This process's own prompt window leaves the screen in the very event that announced it:
        // nothing else has to run first (see EjectTheNativePromptAtOnce). Only the prompt's own
        // host class is ejected on sight, never a window this process happens to own.
        if (IsConsentHostClass(cls)) EjectTheNativePromptAtOnce(hwnd);
        AddCandidate(hwnd, pid, cls);
    } else {
        // Identity gate: a foreign window is never harvested and never clicked. It is noted
        // for the dump only (the shell creates plenty of windows while a prompt is pending,
        // the Start menu among them).
        NoteForeignWindow(hwnd, pid, cls);
    }
}

// The event callback the system calls into, with the same last net: an exception here must not
// travel through the event dispatch of user32 either.
static void CALLBACK WinEventProc(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject,
                                  LONG idChild, DWORD thread, DWORD time) {
#if W7UAC_HAS_EXCEPTIONS
    try {
        WinEventProcBody(hook, event, hwnd, idObject, idChild, thread, time);
    } catch (...) {
        Log(L"exception in the event watcher (event 0x%04X): the event is dropped", event);
    }
#else
    WinEventProcBody(hook, event, hwnd, idObject, idChild, thread, time);
#endif
}

// Records the app the user was working with, from the Default desktop.
struct UserAppCtx {
    long bestArea;
    std::wstring exe;
    std::wstring cls;
};

static BOOL CALLBACK UserDesktopEnumProc(HWND h, LPARAM lParam) {
    UserAppCtx* ctx = (UserAppCtx*)lParam;
    if (!IsWindowVisible(h)) return TRUE;
    if ((GetWindowLongW(h, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) != 0) return TRUE;

    WCHAR cls[128] = L"";
    GetClassNameW(h, cls, ARRAYSIZE(cls));
    static const WCHAR* kSkip[] = {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", L"Progman",
                                   L"WorkerW", L"Windows.UI.Core.CoreWindow",
                                   L"Win7UacRecreationWnd", L"TaskListThumbnailWnd",
                                   L"XamlExplorerHostIslandWindow"};
    for (auto* s : kSkip)
        if (_wcsicmp(cls, s) == 0) return TRUE;

    WCHAR title[256] = L"";
    GetWindowTextW(h, title, ARRAYSIZE(title));
    if (title[0] == L'\0') return TRUE;

    RECT rc = {};
    if (!GetWindowRect(h, &rc)) return TRUE;
    long area = (long)(rc.right - rc.left) * (long)(rc.bottom - rc.top);
    if (area <= ctx->bestArea) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid || pid == GetCurrentProcessId()) return TRUE;

    ctx->bestArea = area;
    ctx->exe = GetProcessImageName(pid);
    ctx->cls = cls;
    return TRUE;
}

static void CaptureUserDesktopApp() {
    std::wstring exe, cls;

    HWND fg = GetForegroundWindow();
    if (fg) {
        DWORD pid = 0;
        GetWindowThreadProcessId(fg, &pid);
        if (pid && pid != GetCurrentProcessId()) {
            exe = GetProcessImageName(pid);
            WCHAR c[128] = L"";
            GetClassNameW(fg, c, ARRAYSIZE(c));
            cls = c;
        }
    }

    UserAppCtx ctx = {0};
    HDESK desk = GetThreadDesktop(GetCurrentThreadId());
    if (EnumDesktopWindows(desk, UserDesktopEnumProc, (LPARAM)&ctx) && exe.empty() &&
        !ctx.exe.empty()) {
        exe = ctx.exe;
        cls = ctx.cls;
    }

    EnterCriticalSection(&g_cs);
    g_userDesktopApp = exe;
    g_userDesktopAppClass = cls;
    LeaveCriticalSection(&g_cs);
    InterlockedExchange(&g_userDesktopAppReady, 1);
    Dbg(L"user desktop app: '%s' (class '%s')", exe.c_str(), cls.c_str());
}

// Used in polling mode, where no watcher thread exists to capture the user desktop app.
static DWORD WINAPI IdentityThreadProcBody(LPVOID) {
    CaptureUserDesktopApp();
    return 0;
}

static DWORD WINAPI IdentityThreadProc(LPVOID param) {
#if W7UAC_HAS_EXCEPTIONS
    try {
        DWORD r = IdentityThreadProcBody(param);
        InterlockedDecrement(&g_threadsActive);
        return r;
    } catch (...) {
        Log(L"exception while capturing the user desktop app: nothing else is affected");
        InterlockedDecrement(&g_threadsActive);
        return 0;
    }
#else
    DWORD r = IdentityThreadProcBody(param);
    InterlockedDecrement(&g_threadsActive);
    return r;
#endif
}

// Prompt discovery is event-driven. The in-process CreateWindowExW hook parks the native host
// before the caller can show it, while this WinEvent hook covers the later SHOW/LOCATIONCHANGE
// events in case consent repositions the host. There is deliberately no desktop sweep here: a
// prompt can remain pending for minutes, and a timer would turn that wait into needless polling.

static DWORD WINAPI WatcherProcBody(LPVOID param) {
    WatcherCtx* w = (WatcherCtx*)param;
    g_curWatcher = (int)(w - g_watchers);

    HDESK desk = OpenDesktopByName(w->name);
    if (!desk) return 0;
    w->desktop = desk;

    // SetThreadDesktop only works while this thread has no window and no installed hook.
    if (!SetThreadDesktop(desk)) {
        Log(L"watcher[%s]: SetThreadDesktop failed: %u", w->name, GetLastError());
        CloseDesktop(desk);
        w->desktop = nullptr;
        return 0;
    }

    // The range carries the events the prompt announces itself with - created, shown - and every
    // move. The CreateWindowExW hook normally handles the first placement synchronously; this
    // event path handles a later move without waking up on a timer.
    HWINEVENTHOOK hook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                                         WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!hook) {
        Log(L"watcher[%s]: SetWinEventHook failed: %u", w->name, GetLastError());
        CloseDesktop(desk);
        w->desktop = nullptr;
        return 0;
    }

    w->hook = hook;
    w->tid = GetCurrentThreadId();
    Log(L"watcher[%s]: active (desktop handle %p, tid=%lu)", w->name, (void*)desk, w->tid);

    MSG msg;
    while (!StopRequested()) {
        DWORD wait = MsgWaitForMultipleObjectsEx(1, &g_stopEvent, INFINITE, QS_ALLINPUT,
                                                 MWMO_INPUTAVAILABLE);
        if (wait != WAIT_OBJECT_0 + 1) break;
        if (!PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) continue;
        if (msg.message == WM_QUIT) break;
        if (msg.hwnd == nullptr && msg.message == WM_PROMPT_STARTED) {
            CaptureUserDesktopApp();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnhookWinEvent(hook);
    return 0;
}

// The wrapper of the watcher thread: the body above it, and the count of running threads of this
// mod decremented on every way out of it, the normal one included.
static DWORD WINAPI WatcherProc(LPVOID param) {
#if W7UAC_HAS_EXCEPTIONS
    try {
        DWORD r = WatcherProcBody(param);
        InterlockedDecrement(&g_threadsActive);
        return r;
    } catch (...) {
        Log(L"exception in the watcher thread: the watcher stops, the Windows prompt is untouched");
        InterlockedDecrement(&g_threadsActive);
        return 0;
    }
#else
    DWORD r = WatcherProcBody(param);
    InterlockedDecrement(&g_threadsActive);
    return r;
#endif
}

static void StartWatchers() {
    for (auto& w : g_watchers) {
        InterlockedIncrement(&g_threadsActive);
        w.thread = CreateThread(nullptr, 0, WatcherProc, &w, 0, nullptr);
        if (!w.thread) {
            InterlockedDecrement(&g_threadsActive);
            Log(L"watcher[%s]: CreateThread failed: %u", w.name, GetLastError());
            continue;
        }
        TrackModThread(w.thread, w.name);
    }
}

static void StopWatchers() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    for (auto& w : g_watchers) {
        if (w.thread) {
            if (WaitForSingleObject(w.thread, INFINITE) != WAIT_OBJECT_0)
                Log(L"watcher[%s]: did not stop in time", w.name);
            // Waited for, not closed: this handle is one of the tracked ones and WaitForModThreads
            // closes it. Both places closing it was a double close on the same value.
            w.thread = nullptr;
            w.tid = 0;
        }
        w.hook = nullptr;
        // The desktop handle opened at the start of the thread lives as long as the mod does (the
        // session enumerates the two desktops through these handles); it is closed here, with the
        // last thread that could have used it already waited for.
        if (w.desktop) {
            CloseDesktop(w.desktop);
            w.desktop = nullptr;
        }
    }
}

//------------------------------------------------------------------------------
// Prompt content
//------------------------------------------------------------------------------

struct UiElement {
    std::wstring name;
    std::wstring automationId;
    std::wstring className;
    CONTROLTYPEID type = 0;
    BOOL enabled = TRUE;
    BOOL isPassword = FALSE;
    RECT rect = {};
#if HAVE_UIA
    IUIAutomationElement* element = nullptr;   // non-owning, refs are tracked in g_uiaRefs
#endif
};

struct PromptInfo {
    HWND hNative = nullptr;
    HWND hDefaultButton = nullptr;
    std::wstring windowTitle;
    std::wstring questionText;    // the prompt's own wording, mapped onto the Win7 rows
    std::wstring appText;
    std::wstring publisherText;
    std::vector<UiElement> elements;
    std::vector<std::wstring> texts;
    std::vector<std::wstring> detailTexts;   // what the native "Show details" pane reveals
    std::vector<std::wstring> paths;
    std::wstring appPath;      // the file being elevated, when the prompt says which one
    std::wstring originText;   // Windows 7's "File origin" row
    UiElement* yes = nullptr;
    UiElement* no = nullptr;
    bool buttonsConfident = false;
    bool yesEnabled = false;
    bool credentialVariant = false;
    bool systemFileConfirmed = false;        // the prompt itself showed %SystemRoot%\regedit.exe; signature is still checked
    bool publisherVerifiedByWindows = false; // native verified row, or the signature-backed Regedit exception
    bool nativeWindowsComponent = false;     // the native row says "verified" and the publisher is "Microsoft Windows"
    bool publisherRowLabelSeen = false;      // a labelled publisher row was present
    bool publisherRowValueReadable = false;   // its value was actually mapped, not fallback text
    bool publisherRowLabelRejected = false;  // its label was rejected by LabelSaysVerified
    bool sawUnknownWording = false;          // explicit unknown publisher/author wording in native text
    bool regeditDetected = false;            // native text identifies Registry Editor; presentation is deferred
    bool regeditOverrideApplied = false;     // the narrow path/signature-backed presentation is active
    bool sawUnknownAuthor = false;   // the prompt says the author is unknown, not just the publisher
    bool publisherFromSignature = false; // original compatibility recovery behavior
    int signatureState = -1;
    int shieldVariant = 0;     // 0 verified, 1 blue question mark, 2 amber exclamation
    int bannerState = 1;      // 1 blue, 2 yellow, 3 red
    const WCHAR* harvestSource = L"none";
};

static const WCHAR* kYesNames[] = {L"yes", L"s\u00ec", L"si", L"ja", L"oui", L"s\u00ed",
                                   L"sim", L"tak", L"igen", L"j\u00e1", L"\u0434\u0430",
                                   L"\u306f\u3044", L"\uc608", L"\u662f", L"ano",
                                   L"evet", L"\u0646\u0639\u0645", L"\u0628\u0644\u0647", L"\u0939\u093e\u0901"};
static const WCHAR* kNoNames[]  = {L"no", L"nein", L"non", L"n\u00e3o", L"nao", L"niet",
                                   L"nie", L"n\u0113", L"nej", L"\u043d\u0435\u0442",
                                   L"\u3044\u3044\u3048", L"\uc544\ub2c8\uc694", L"\u5426",
                                   L"hay\u0131r", L"\u0644\u0627", L"\u062e\u06cc\u0631", L"\u0928\u0939\u0940\u0902"};

static bool NameIsYes(const std::wstring& name) {
    std::wstring n = ToLower(Trim(name));
    for (auto* t : kYesNames) if (n == t) return true;
    return false;
}

static bool NameIsNo(const std::wstring& name) {
    std::wstring n = ToLower(Trim(name));
    for (auto* t : kNoNames) if (n == t) return true;
    return false;
}

// -- Source 1: plain Win32 child enumeration (no COM, works across processes) -----

static BOOL CALLBACK ChildEnumProc(HWND hChild, LPARAM lParam) {
    PromptInfo* info = (PromptInfo*)lParam;

    WCHAR cls[64] = L"";
    GetClassNameW(hChild, cls, ARRAYSIZE(cls));
    WCHAR text[1024] = L"";
    GetWindowTextW(hChild, text, ARRAYSIZE(text));

    UiElement e;
    e.className = cls;
    e.name = text;
    e.enabled = IsWindowEnabled(hChild);
    GetWindowRect(hChild, &e.rect);

    std::wstring lower = ToLower(cls);
    if (lower == L"button") e.type = UIA_ButtonControlTypeId;
    else if (lower == L"edit" || lower == L"comboboxex32") {
        e.type = UIA_EditControlTypeId;
        e.isPassword = TRUE;
    } else if (lower == L"static" || lower == L"syslink") e.type = UIA_TextControlTypeId;

    info->elements.push_back(e);
    return TRUE;
}

static bool HarvestWin32(HWND hWnd, PromptInfo& info) {
    EnumChildWindows(hWnd, ChildEnumProc, (LPARAM)&info);
    if (info.elements.empty()) return false;

    DWORD_PTR res = 0;
    if (SendMessageTimeoutW(hWnd, DM_GETDEFID, 0, 0, SMTO_ABORTIFHUNG, 500, &res) && res) {
        DWORD defId = (DWORD)res;
        if (HIWORD(defId) == DC_HASDEFID) info.hDefaultButton = GetDlgItem(hWnd, LOWORD(defId));
    }

    std::vector<UiElement*> buttons;
    for (auto& e : info.elements) {
        if (e.type == UIA_EditControlTypeId) info.credentialVariant = true;
        if (e.type == UIA_ButtonControlTypeId && !e.name.empty()) buttons.push_back(&e);
        else if (!e.name.empty()) info.texts.push_back(e.name);
    }

    // The two buttons are matched by their own text (ResolveButtons does the naming on the
    // finished set). There is deliberately no positional fallback: a guessed button could turn a
    // click on "No" into an approval on the native prompt.
    info.harvestSource = L"win32";
    return true;
}

// -- Source 2: UI Automation (richer text, works also for non-Win32 prompts) ------

#if HAVE_UIA
static IUIAutomation* g_uia = nullptr;
[[clang::no_destroy]] static std::vector<IUIAutomationElement*> g_uiaRefs;

static void UiaRelease() {
    for (auto* e : g_uiaRefs) if (e) e->Release();
    g_uiaRefs.clear();
    if (g_uia) { g_uia->Release(); g_uia = nullptr; }
}

static bool UiaInit() {
    if (g_uia) return true;
    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation8, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&g_uia));
    if (FAILED(hr) || !g_uia) {
        Log(L"UIA: CoCreateInstance failed hr=0x%08X", hr);
        g_uia = nullptr;
        return false;
    }
    IUIAutomation2* bounded = nullptr;
    HRESULT limits = g_uia->QueryInterface(IID_PPV_ARGS(&bounded));
    if (SUCCEEDED(limits) && bounded) {
        limits = bounded->put_ConnectionTimeout(250);
        if (SUCCEEDED(limits)) limits = bounded->put_TransactionTimeout(250);
        bounded->Release();
    } else {
        limits = E_NOINTERFACE;
    }
    if (FAILED(limits)) {
        Log(L"UIA timeouts unavailable -> native fallback");
        g_uia->Release();
        g_uia = nullptr;
        return false;
    }
    return !StopRequested();
}

static void ElementToInfo(IUIAutomationElement* el, UiElement& out) {
    if (!el) return;
    BSTR b = nullptr;
    if (SUCCEEDED(el->get_CurrentName(&b)) && b) { out.name = b; SysFreeString(b); }
    b = nullptr;
    if (SUCCEEDED(el->get_CurrentAutomationId(&b)) && b) { out.automationId = b; SysFreeString(b); }
    b = nullptr;
    if (SUCCEEDED(el->get_CurrentClassName(&b)) && b) { out.className = b; SysFreeString(b); }

    CONTROLTYPEID t = 0;
    el->get_CurrentControlType(&t);
    out.type = t;

    BOOL v = TRUE;
    el->get_CurrentIsEnabled(&v);
    out.enabled = v;

    // Note: UI Automation has no "IsDefault" property (UIA_IsDefaultPropertyId does not
    // exist). The default button is taken from DM_GETDEFID, from the localized button name
    // or from the position of the buttons.
    VARIANT var;
    VariantInit(&var);
    if (SUCCEEDED(el->GetCurrentPropertyValue(UIA_IsPasswordPropertyId, &var)) &&
        var.vt == VT_BOOL && var.boolVal != VARIANT_FALSE)
        out.isPassword = TRUE;
    VariantClear(&var);

    RECT rc = {};
    if (SUCCEEDED(el->get_CurrentBoundingRectangle(&rc))) out.rect = rc;

    el->AddRef();
    g_uiaRefs.push_back(el);
    out.element = el;
}

static void WalkTree(IUIAutomationTreeWalker* walker, IUIAutomationElement* el,
                     std::vector<UiElement>& out, int depth, int* budget) {
    if (StopRequested() || !el || depth > 6 || *budget <= 0) return;
    IUIAutomationElement* child = nullptr;
    if (FAILED(walker->GetFirstChildElement(el, &child)) || !child) return;
    while (child && *budget > 0 && !StopRequested()) {
        (*budget)--;
        UiElement e;
        ElementToInfo(child, e);
        out.push_back(e);
        WalkTree(walker, child, out, depth + 1, budget);
        IUIAutomationElement* next = nullptr;
        if (FAILED(walker->GetNextSiblingElement(child, &next))) next = nullptr;
        child = next;
    }
    if (child) child->Release();
}

static bool HarvestUia(HWND hWnd, PromptInfo& info) {
    if (StopRequested() || !UiaInit()) return false;
    IUIAutomationElement* root = nullptr;
    if (FAILED(g_uia->ElementFromHandle(hWnd, &root)) || !root) return false;

    UiElement rootInfo;
    ElementToInfo(root, rootInfo);
    info.elements.push_back(rootInfo);

    IUIAutomationTreeWalker* walker = nullptr;
    if (FAILED(g_uia->get_RawViewWalker(&walker)) || !walker) return false;

    int budget = 250;
    WalkTree(walker, root, info.elements, 0, &budget);
    walker->Release();

    std::vector<UiElement*> buttons;
    for (auto& e : info.elements) {
        if (e.type == UIA_EditControlTypeId || e.isPassword) info.credentialVariant = true;
        if (e.type == UIA_ButtonControlTypeId && !e.name.empty()) buttons.push_back(&e);
        else if (!e.name.empty()) info.texts.push_back(e.name);
    }

    // Names, never positions (see ResolveButtons, which runs on the merged set).

    info.harvestSource = L"uia";
    return true;
}
#else
static bool HarvestUia(HWND, PromptInfo&) { return false; }
static void UiaRelease() {}
#endif

// "Editore verificato: ...", "Autore verificato: ...", "Verified publisher: ...",
// "Editore sconosciuto" and friends:
// the row that carries the publisher is recognised by its own words, not by its position.
static bool LooksLikePublisher(const std::wstring& t) {
    static const WCHAR* kWords[] = {
        L"publisher", L"editore", L"autore", L"author", L"autor", L"auteur",
        L"verlag", L"\u00e9diteur", L"editor", L"verificato",
        L"verified", L"signiert", L"firmado", L"unknown", L"sconosciuto", L"unbekannt",
        L"inconnu", L"desconocido", L"desconhecido",
    };
    std::wstring l = ToLower(t);
    for (const WCHAR* w : kWords) {
        if (l.find(w) != std::wstring::npos) return true;
    }
    return false;
}

// A row like "Editore verificato: Microsoft Windows" carries two things: a label and a
// value. Only the value belongs in the Windows 7 dialog. Paths ("C:\...") contain a colon
// too and must not be split.
static void SplitLabelValue(const std::wstring& t, std::wstring& label, std::wstring& value) {
    label.clear();
    value = t;
    if (t.size() > 2 && t[1] == L':' &&
        ((t[0] >= L'A' && t[0] <= L'Z') || (t[0] >= L'a' && t[0] <= L'z')))
        return;                                   // drive letter, not a label
    size_t p = t.find(L':');
    if (p == std::wstring::npos) return;
    label = t.substr(0, p);
    value = t.substr(p + 1);
    while (!label.empty() && (label.back() == L' ' || label.back() == L'\t')) label.pop_back();
    while (!value.empty() && (value.front() == L' ' || value.front() == L'\t')) value.erase(0, 1);
    while (!value.empty() && (value.back() == L' ' || value.back() == L'\t')) value.pop_back();
}

// The prompt's own chrome: its window title. It is a legitimate string of the prompt, but it
// is never the name of the program asking for elevation.
static bool IsChromeText(const std::wstring& t) {
    static const WCHAR* kChrome[] = {
        L"user account control", L"controllo dell'account utente", L"controllo account utente",
        L"benutzerkontensteuerung", L"contr\u00f4le de compte d'utilisateur",
        L"control de cuentas de usuario", L"controle de conta de usu\u00e1rio",
        L"\u0441\u043b\u0443\u0436\u0431\u0430 \u043a\u043e\u043d\u0442\u0440\u043e\u043b\u044f",
        L"\u30e6\u30fc\u30b6\u30fc \u30a2\u30ab\u30a6\u30f3\u30c8 \u5236\u5fa1",
    };
    std::wstring l = ToLower(t);
    for (const WCHAR* c : kChrome) {
        if (l == c) return true;
    }
    return false;
}

static bool IsPathLike(const std::wstring& t) {
    return t.find(L":\\") != std::wstring::npos || t.find(L"\\\\") != std::wstring::npos ||
           (!t.empty() && t[0] == L'\\');
}

static std::wstring BaseNameOf(const std::wstring& t) {
    size_t p = t.find_last_of(L"\\/");
    return (p == std::wstring::npos) ? t : t.substr(p + 1);
}

static volatile LONG g_mappingLogged = 0;

// --- mapping the prompt's own strings onto the Windows 7 rows -----------------------------

static bool IsAlphaNumChar(WCHAR c) {
    return (c >= L'0' && c <= L'9') || (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') ||
           c > 127;
}

// "?", "-", "***": the help button of the modern prompt is a text of its own and is not the
// name of a program.
static bool IsJunkText(const std::wstring& t) {
    int letters = 0;
    for (WCHAR c : t) {
        if (IsAlphaNumChar(c)) letters++;
    }
    return letters < 2;
}

static bool TextHasAnyWord(const std::wstring& t, const WCHAR* const* words, size_t count) {
    std::wstring l = ToLower(t);
    for (size_t i = 0; i < count; i++) {
        if (l.find(words[i]) != std::wstring::npos) return true;
    }
    return false;
}

static const WCHAR* kAppRowWords[] = {
    L"nome programma", L"program name", L"programmname", L"nom du programme",
    L"nombre del programa", L"nome do programa", L"nazwa programu",
};
static const WCHAR* kPubRowWords[] = {
    L"editore", L"autore", L"author", L"autor", L"auteur", L"publisher",
    L"herausgeber", L"\u00e9diteur", L"editeur", L"editor",
    L"uitgever", L"wydawca",
};
static const WCHAR* kLinkWords[] = {
    L"show details", L"hide details", L"mostra dettagli", L"nascondi dettagli", L"dettagli",
    L"details", L"d\u00e9tails", L"detalles", L"detalhes", L"help me decide",
    L"aiutami a decidere", L"change when", L"modifica quando",
};
// "Unknown author" and not just "unknown publisher": the prompt uses one or the other, and the
// two are shown with different shields. These are the words of the first case.
static const WCHAR* kAuthorWords[] = {
    L"autore", L"author", L"autor", L"auteur", L"\u0430\u0432\u0442\u043e\u0440",
    L"auktor", L"autor", L"tekij\u00e4",
};

static const WCHAR* kUnknownWords[] = {
    L"unknown publisher", L"unknown author", L"unknown", L"editore sconosciuto", L"sconosciuto",
    L"unbekannter herausgeber", L"unbekannt", L"\u00e9diteur inconnu", L"inconnu",
    L"editor desconocido", L"desconocido", L"editor desconhecido", L"desconhecido",
    L"nieznany wydawca", L"nieznany", L"onbekende uitgever", L"onbekend",
    L"\u043d\u0435\u0438\u0437\u0432\u0435\u0441\u0442\u043d\u044b\u0439",
    L"\u4e0d\u660e\u306a\u767a\u884c\u5143", L"\u4e0d\u660e", L"\u672a\u77e5\u53d1\u5e03\u8005", L"\u672a\u77e5",
    L"bilinmeyen yay\u0131mc\u0131", L"bilinmeyen", L"\u0646\u0627\u0634\u0631 \u063a\u064a\u0631 \u0645\u0639\u0631\u0648\u0641", L"\u063a\u064a\u0631 \u0645\u0639\u0631\u0648\u0641", L"\u0646\u0627\u0634\u0631 \u0646\u0627\u0634\u0646\u0627\u0633", L"\u0646\u0627\u0634\u0646\u0627\u0633", L"\u0905\u091c\u094d\u091e\u093e\u0924 \u092a\u094d\u0930\u0915\u093e\u0936\u0915", L"\u0905\u091c\u094d\u091e\u093e\u0924", L"\uc54c \uc218 \uc5c6\ub294 \uac8c\uc2dc\uc790", L"\uc54c \uc218 \uc5c6\ub294", L"\uc54c \uc218 \uc5c6\uc74c",
};

static bool IsUnknownPublisherValue(const std::wstring& text) {
    std::wstring value = ToLower(Trim(text));
    while (!value.empty() && (value.back() == L'.' || value.back() == L':' ||
                              value.back() == L';' || value.back() == L'!'))
        value.pop_back();
    if (value.empty() || value == L"-" || IsJunkText(value)) return true;
    for (const WCHAR* term : kUnknownWords) {
        // Multiword phrases identify a complete unknown-publisher/author statement; one-word
        // values match only as the value itself, not as part of a legitimate company name.
        if (wcschr(term, L' ')) {
            if (value.find(term) != std::wstring::npos) return true;
        } else if (value == term) {
            return true;
        }
    }
    return false;
}

// The legacy blue question-mark shield is for a trusted third-party publisher in the Windows 7
// consent UI. Keep the plain Windows shield when the publisher identifies Microsoft Windows.
static bool IsWindowsPublisherText(const std::wstring& text) {
    std::wstring value = ToLower(Trim(text));
    return value == L"windows" || value.find(L"microsoft windows") != std::wstring::npos;
}

static const WCHAR* kVerifiedWords[] = {L"verified", L"verificato", L"verifiziert", L"v\u00e9rifi\u00e9", L"comprobado", L"verificado", L"geverifieerd", L"zweryfikowany", L"\u043f\u0440\u043e\u0432\u0435\u0440\u0435\u043d\u043d", L"\u78ba\u8a8d\u6e08\u307f", L"\ud655\uc778\ub41c", L"do\u011frulanm\u0131\u015f", L"\u062a\u0623\u06cc\u06cc\u062f", L"\u0627\u0644\u0645\u0639\u062a\u0645\u062f", L"\u0938\u0924\u094d\u092f\u093e\u092a\u093f\u0924"};
static const WCHAR* kNotVerifiedWords[] = {L"unverified", L"non verificato", L"nicht verifiziert", L"non v\u00e9rifi\u00e9", L"no verificado", L"no comprobado", L"n\u00e3o verificado", L"nao verificado", L"niet geverifieerd", L"niet-geverifieerd", L"niezweryfikowany", L"\u043d\u0435\u043f\u0440\u043e\u0432\u0435\u0440\u0435\u043d\u043d", L"\u043d\u0435\u043f\u043e\u0434\u0442\u0432\u0435\u0440\u0436\u0434", L"\u672a\u78ba\u8a8d", L"\ud655\uc778\ub418\uc9c0 \uc54a\uc740", L"do\u011frulanmam\u0131\u015f", L"\u063a\u064a\u0631 \u0645\u0639\u062a\u0645\u062f", L"\u062a\u0623\u06cc\u06cc\u062f \u0646\u0634\u062f\u0647"};

// True only when the label of the native publisher row says "verified" in the prompt's language.
static bool LabelSaysVerified(const std::wstring& label) {
    if (TextHasAnyWord(label, kNotVerifiedWords, ARRAYSIZE(kNotVerifiedWords))) return false;
    return TextHasAnyWord(label, kVerifiedWords, ARRAYSIZE(kVerifiedWords));
}

// A details expansion can expose the label and value as separate strings, so retain the small
// set of unambiguous standalone publisher labels as evidence even without row geometry.
static bool IsStandalonePublisherLabel(const std::wstring& text) {
    std::wstring value = ToLower(Trim(text));
    static const WCHAR* kLabels[] = {L"publisher", L"editore", L"herausgeber",
                                     L"éditeur", L"editeur", L"uitgever", L"wydawca",
                                     L"editor"};
    for (const WCHAR* label : kLabels)
        if (value == label) return true;
    return TextHasAnyWord(value, kNotVerifiedWords, ARRAYSIZE(kNotVerifiedWords));
}

// The two bottom links of the prompt, in the languages this mod ships strings for. They are
// used to find the native element whose click has to be forwarded.
static const WCHAR* kHelpLinkWords[] = {
    L"aiutami a decidere", L"help me decide", L"entscheidungshilfe", L"aide-moi",
    L"ay\u00fademe a decidir",
};
static const WCHAR* kChangeLinkWords[] = {
    L"modifica quando", L"change when", L"aendern, wann", L"modifier le moment",
    L"cambiar cu",
};

struct RowText {
    std::wstring t;
    RECT rc;
};

// The prompt is a XAML tree whose rows are made of a label and a value. The value either sits
// inside the same element ("Editore verificato: Microsoft Windows") or in the element to the
// right of the label, on the same line. That is what this function follows.
static void MapPromptRows(PromptInfo& info) {
    std::vector<RowText> rows;
    for (const auto& e : info.elements) {
        if (e.name.empty() || e.type == UIA_ButtonControlTypeId) continue;
        RowText r;
        r.t = e.name;
        r.rc = e.rect;
        rows.push_back(r);
    }

    // The question is the longest text that asks something: the help button "?" is a text of
    // its own and must not win.
    std::wstring question;
    for (const auto& r : rows) {
        if (IsJunkText(r.t) || r.t.find(L'?') == std::wstring::npos) continue;
        if (question.size() < r.t.size()) question = r.t;
    }
    info.questionText = question;

    auto valueOf = [&](const RowText& label, std::wstring& out) -> bool {
        std::wstring lbl, val;
        SplitLabelValue(label.t, lbl, val);
        if (!lbl.empty() && !val.empty()) {
            out = val;
            return true;
        }
        const RowText* best = nullptr;
        LONG bestDx = 0;
        for (const auto& r : rows) {
            if (&r == &label) continue;
            if (r.t == question || IsJunkText(r.t)) continue;
            if (TextHasAnyWord(r.t, kAppRowWords, ARRAYSIZE(kAppRowWords))) continue;
            if (TextHasAnyWord(r.t, kPubRowWords, ARRAYSIZE(kPubRowWords))) continue;
            LONG dy = (r.rc.top > label.rc.top) ? (r.rc.top - label.rc.top)
                                                : (label.rc.top - r.rc.top);
            if (dy > 10) continue;                 // another line
            LONG dx = r.rc.left - label.rc.left;
            if (dx <= 0) continue;                 // not to the right
            if (!best || dx < bestDx) {
                best = &r;
                bestDx = dx;
            }
        }
        if (best) {
            out = best->t;
            return true;
        }
        return false;
    };

    const RowText* appLabel = nullptr;
    const RowText* pubLabel = nullptr;
    std::wstring pubValue;
    bool publisherLabelSeen = false;
    bool publisherLabelRejected = false;
    for (const auto& r : rows) {
        if (IsJunkText(r.t) || r.t == question) continue;
        bool isAppRow = TextHasAnyWord(r.t, kAppRowWords, ARRAYSIZE(kAppRowWords));
        if (!appLabel && isAppRow) appLabel = &r;
        // A publisher row is accepted only when a value is found for it ("Registry Editor" has
        // no colon and nothing to its right, so it is skipped and scanning goes on). A colon-labelled
        // row is recorded even when its value is unreadable: it is still explicit native evidence.
        if (!isAppRow &&
            (TextHasAnyWord(r.t, kPubRowWords, ARRAYSIZE(kPubRowWords)) || LooksLikePublisher(r.t))) {
            std::wstring lbl, ignoredValue;
            SplitLabelValue(r.t, lbl, ignoredValue);
            std::wstring v;
            bool hasValue = valueOf(r, v);
            // Do not mistake an unlabeled program name such as "Registry Editor" for a
            // publisher row. A no-colon row is evidence only when it yielded a value or is an
            // unambiguous standalone publisher label.
            if (hasValue || !lbl.empty() || IsStandalonePublisherLabel(r.t)) {
                const std::wstring label = lbl.empty() ? r.t : lbl;
                publisherLabelSeen = true;
                if (!LabelSaysVerified(label)) publisherLabelRejected = true;
            }
            if (!pubLabel && hasValue) { pubLabel = &r; pubValue = v; }
        }
    }
    info.publisherRowLabelSeen = publisherLabelSeen;
    info.publisherRowValueReadable = (pubLabel != nullptr);
    info.publisherRowLabelRejected = publisherLabelRejected;
    if (appLabel) valueOf(*appLabel, info.appText);
    if (pubLabel) {
        info.publisherText = pubValue;
        std::wstring lbl, val;
        SplitLabelValue(pubLabel->t, lbl, val);
        info.publisherVerifiedByWindows =
            LabelSaysVerified(lbl.empty() ? pubLabel->t : lbl) && !IsUnknownPublisherValue(pubValue);
    }

    // Fallback: this prompt exposes no labels at all, only values. Then the rows are taken in
    // reading order, skipping the question, the chrome, the links and the help button.
    if (info.appText.empty() || info.publisherText.empty()) {
        std::vector<const RowText*> rest, restAll;
        for (const auto& r : rows) {
            if (r.t == question || IsJunkText(r.t)) continue;
            if (TextHasAnyWord(r.t, kAppRowWords, ARRAYSIZE(kAppRowWords))) continue;
            if (TextHasAnyWord(r.t, kPubRowWords, ARRAYSIZE(kPubRowWords))) continue;
            if (TextHasAnyWord(r.t, kLinkWords, ARRAYSIZE(kLinkWords))) continue;
            restAll.push_back(&r);
            if (IsChromeText(r.t)) continue;
            if (!info.windowTitle.empty() && r.t == info.windowTitle) continue;
            rest.push_back(&r);
        }
        auto byPosition = [](const RowText* a, const RowText* b) {
            if (a->rc.top != b->rc.top) return a->rc.top < b->rc.top;
            return a->rc.left < b->rc.left;
        };
        std::sort(rest.begin(), rest.end(), byPosition);
        std::sort(restAll.begin(), restAll.end(), byPosition);
        auto fill = [&](const std::vector<const RowText*>& v) {
            if (info.appText.empty() && !v.empty()) info.appText = v[0]->t;
            if (info.publisherText.empty() && v.size() >= 2) info.publisherText = v[1]->t;
        };
        fill(rest);
        fill(restAll);          // last resort: the title itself is better than an empty row
    }
    if (IsPathLike(info.appText)) info.appText = BaseNameOf(info.appText);

    // The prompt's own texts are only logged with debugLog on: the always-on lines never
    // repeat what the dialog says.
    if (InterlockedCompareExchange(&g_mappingLogged, 1, 0) == 0)
        Dbg(L"text mapping: question='%s' app='%s' publisher='%s'", info.questionText.c_str(),
            info.appText.c_str(), info.publisherText.c_str());
}

static bool EndsWithExecExt(const std::wstring& t) {
    size_t dot = t.find_last_of(L'.');
    if (dot == std::wstring::npos) return false;
    std::wstring ext = ToLower(t.substr(dot));
    return ext == L".exe" || ext == L".msc" || ext == L".cpl" || ext == L".bat" ||
           ext == L".cmd";
}

// AppInfo starts consent.exe as "consent.exe <parent pid> <size> <pointer>" and the block that
// pointer refers to is mapped into this process as well: it is the request being displayed, and
// the file to elevate is in it. Its *layout* is not documented, so nothing is assumed about it:
// the block is only scanned for a raw UTF-16 path that ends in .exe and exists on disk. The
// region is checked with VirtualQuery first and the scan is bounded, so an unreadable or foreign
// block is simply ignored - never followed blindly.
static std::wstring ScanBlockForExecutablePath(const WCHAR* block, size_t cch) {
    for (size_t i = 0; i + 4 < cch; i++) {
        bool drive = ((block[i] >= L'A' && block[i] <= L'Z') ||
                      (block[i] >= L'a' && block[i] <= L'z')) &&
                     block[i + 1] == L':' && (block[i + 2] == L'\\' || block[i + 2] == L'/');
        bool unc = (block[i] == L'\\' && block[i + 1] == L'\\');
        if (!drive && !unc) continue;

        WCHAR cand[320] = L"";
        size_t n = 0;
        while (i + n < cch && n < ARRAYSIZE(cand) - 1) {
            WCHAR c = block[i + n];
            if (c < 0x20 || c == L'"') break;       // end of the string / of the field
            cand[n++] = c;
        }
        if (n < 8) continue;
        cand[n] = 0;
        std::wstring s = cand;
        if (!EndsWithExecExt(s)) {
            // The run may carry trailing junk: cut it after the last ".exe".
            size_t dot = ToLower(s).rfind(L".exe");
            if (dot == std::wstring::npos) continue;
            s = s.substr(0, dot + 4);
        }
        if (GetFileAttributesW(s.c_str()) != INVALID_FILE_ATTRIBUTES) return s;
    }
    return std::wstring();
}

// The raw bytes of the request block, for the diagnosis of a path that is not found in it. Off
// by default: the block belongs to an elevation request and can carry file names and user names.
static void DumpRequestBlock(const BYTE* data, size_t n) {
    WCHAR tmp[8];
    std::wstring line;
    for (size_t i = 0; i < n; i += 16) {
        line.clear();
        for (size_t j = i; j < i + 16 && j < n; j++) {
            swprintf_s(tmp, ARRAYSIZE(tmp), L"%02X ", (unsigned)data[j]);
            line += tmp;
        }
        Dbg(L"request block +%04u: %s", (unsigned)i, line.c_str());
    }
}

// The same search for the 8 bit encodings: a run of printable bytes that looks like a path is
// converted and checked, trying UTF-8 first and the ANSI code page of the machine after it.
// Only a path that really exists on disk is accepted, as in the UTF-16 scan.
static std::wstring ScanBlockForExecutablePathAnsi(const BYTE* data, size_t n) {
    char cand[320];
    for (size_t i = 0; i + 8 < n; i++) {
        BYTE c = data[i];
        bool drive = ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) && data[i + 1] == ':' &&
                     (data[i + 2] == '\\' || data[i + 2] == '/');
        bool unc = (c == '\\' && data[i + 1] == '\\');
        if (!drive && !unc) continue;

        size_t m = 0;
        while (i + m < n && m < ARRAYSIZE(cand) - 1) {
            BYTE b = data[i + m];
            if (b < 0x20 || b > 0x7E || b == '"') break;
            cand[m++] = (char)b;
        }
        if (m < 8) continue;
        cand[m] = 0;
        std::string a(cand);
        std::string lower = a;
        for (auto& ch : lower) ch = (char)tolower((unsigned char)ch);
        size_t dot = lower.rfind(".exe");
        if (dot == std::string::npos) continue;
        if (dot + 4 != a.size()) a = a.substr(0, dot + 4);   // trailing junk after the .exe

        const UINT cps[2] = {CP_UTF8, CP_ACP};
        for (int k = 0; k < 2; k++) {
            WCHAR wide[320] = L"";
            int w = MultiByteToWideChar(cps[k], cps[k] == CP_UTF8 ? MB_ERR_INVALID_CHARS : 0,
                                        a.c_str(), -1, wide, ARRAYSIZE(wide));
            if (w <= 1) continue;
            if (!EndsWithExecExt(wide)) continue;
            if (GetFileAttributesW(wide) != INVALID_FILE_ATTRIBUTES) return std::wstring(wide);
        }
    }
    return std::wstring();
}

// The two SIDs are compared by hand: same length, same bytes. Nothing here depends on a helper
// that might not be declared, and nothing is assumed about the token beyond TokenUser.
static size_t SidLength(PSID s) {
    if (!s) return 0;
    return 8 + (size_t)((const BYTE*)s)[1] * 4;
}

static bool SameSid(PSID a, PSID b) {
    size_t la = SidLength(a), lb = SidLength(b);
    return la != 0 && la == lb && memcmp(a, b, la) == 0;
}

// Memory of another process is only ever read when that process belongs to the same user as this
// one. The query is done with PROCESS_QUERY_LIMITED_INFORMATION, the least access that answers
// the question; if it cannot be answered, the answer is no.
static bool SameUserProcess(DWORD pid) {
    if (!pid || pid == GetCurrentProcessId()) return false;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) {
        Dbg(L"request block: OpenProcess(%lu, QUERY_LIMITED) failed: %u", (unsigned long)pid,
            GetLastError());
        return false;
    }
    HANDLE mine = nullptr, theirs = nullptr;
    bool same = false;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &mine) &&
        OpenProcessToken(h, TOKEN_QUERY, &theirs)) {
        BYTE b1[256] = {}, b2[256] = {};
        DWORD n1 = 0, n2 = 0;
        if (GetTokenInformation(mine, TokenUser, b1, sizeof(b1), &n1) &&
            GetTokenInformation(theirs, TokenUser, b2, sizeof(b2), &n2))
            same = SameSid(((TOKEN_USER*)b1)->User.Sid, ((TOKEN_USER*)b2)->User.Sid);
    }
    if (mine) CloseHandle(mine);
    if (theirs) CloseHandle(theirs);
    CloseHandle(h);
    return same;
}

// One bounded read of the request block. ReadProcessMemory is the documented way: a stale or
// foreign pointer gives FALSE and an error code, never an access violation inside consent.exe.
// Nothing is written and nothing is executed - this is a read of a structure that describes an
// elevation request, and the same read consent.exe itself performs.
static bool ReadBlockBounded(DWORD pid, const void* base, size_t size, std::vector<BYTE>& out,
                             const WCHAR** where) {
    bool self = (pid == GetCurrentProcessId());
    if (self) {
        MEMORY_BASIC_INFORMATION mbi = {};
        if (VirtualQuery(base, &mbi, sizeof(mbi)) != sizeof(mbi)) return false;
        if (mbi.State != MEM_COMMIT || (mbi.Protect & PAGE_GUARD) ||
            (mbi.Protect & PAGE_NOACCESS))
            return false;
    }
    HANDLE h = self ? GetCurrentProcess() : OpenProcess(PROCESS_VM_READ, FALSE, pid);
    if (!h) {
        Dbg(L"request block: OpenProcess(%lu, PROCESS_VM_READ) failed: %u", (unsigned long)pid,
            GetLastError());
        return false;
    }
    out.assign(size, 0);
    SIZE_T got = 0;
    BOOL ok = ReadProcessMemory(h, base, out.data(), size, &got);
    DWORD err = ok ? 0u : GetLastError();
    if (!self) CloseHandle(h);
    if (!ok || !got) {
        Dbg(L"request block: ReadProcessMemory in pid %lu at %p failed: %u", (unsigned long)pid,
            base, err);
        out.clear();
        return false;
    }
    out.resize((size_t)got);
    *where = self ? L"this process" : L"the requesting process";
    return true;
}

static std::wstring TargetPathFromRequestBlock() {
    if (!g_settings.readRequestBlock) {
        Dbg(L"AppInfo request block: reading is switched off in the settings");
        return std::wstring();
    }
    const WCHAR* cl = GetCommandLineW();
    if (!cl) return std::wstring();

    std::vector<std::wstring> args;
    std::wstring cmd = cl;
    size_t pos = 0;
    while (pos < cmd.size()) {
        std::wstring token;
        if (cmd[pos] == L' ') { pos++; continue; }
        if (cmd[pos] == L'"') {
            size_t end = cmd.find(L'"', pos + 1);
            if (end == std::wstring::npos) break;
            token = cmd.substr(pos + 1, end - pos - 1);
            pos = end + 1;
        } else {
            size_t sp = cmd.find(L' ', pos);
            token = cmd.substr(pos,
                               (sp == std::wstring::npos) ? std::wstring::npos : sp - pos);
            pos = (sp == std::wstring::npos) ? cmd.size() : sp + 1;
        }
        args.push_back(token);
    }
    // <exe> <pid> <size> <pointer>
    if (args.size() < 4) {
        Dbg(L"AppInfo request block: no <size> <pointer> arguments on the command line");
        return std::wstring();
    }

    ULONGLONG size = wcstoull(args[2].c_str(), nullptr, 10);
    ULONGLONG addr = wcstoull(args[3].c_str(), nullptr, 16);
    if (!size || size < 16 || size > 0x10000 || !addr) {
        Dbg(L"AppInfo request block: unusable arguments on the command line (size=%llu addr=%p)",
            (unsigned long long)size, (const void*)(ULONG_PTR)addr);
        return std::wstring();
    }

    const void* base = (const void*)(ULONG_PTR)addr;
    DWORD requesterPid = (DWORD)wcstoul(args[1].c_str(), nullptr, 10);
    size_t want = (size_t)((size > 4096) ? 4096 : size);

    // The pointer on the command line does not have to be ours: in the runs seen so far it points
    // into the process that asked for the elevation (the pid right there on the command line,
    // before the size), and in consent.exe's own address space that page is free. So the block is
    // looked for here first and then in that process - same user only, read-only, bounded.
    std::vector<BYTE> block;
    const WCHAR* where = L"?";
    bool have = ReadBlockBounded(GetCurrentProcessId(), base, want, block, &where);
    if (!have && requesterPid && requesterPid != GetCurrentProcessId()) {
        Dbg(L"AppInfo request block: not in this process -> trying the requesting pid %lu",
            (unsigned long)requesterPid);
        if (SameUserProcess(requesterPid))
            have = ReadBlockBounded(requesterPid, base, want, block, &where);
        else
            Dbg(L"AppInfo request block: pid %lu is not a process of this user - not read",
                (unsigned long)requesterPid);
    }
    if (!have) {
        Dbg(L"AppInfo request block: unreadable at %p (size %llu, requester pid %lu)", base,
            (unsigned long long)size, (unsigned long)requesterPid);
        return std::wstring();
    }
    Dbg(L"AppInfo request block: %u bytes read at %p (size on the command line %llu) from %s",
        (unsigned)block.size(), base, (unsigned long long)size, where);
    if (g_settings.dumpRequestBlock) DumpRequestBlock(block.data(), block.size());

    // The layout of the block is not documented by Microsoft, so nothing is assumed about it: it
    // is only scanned as text, in both the encodings a structure of this kind can carry, and
    // only a path that exists on disk is accepted.
    std::wstring found = ScanBlockForExecutablePath((const WCHAR*)block.data(), block.size() / 2);
    if (found.empty()) found = ScanBlockForExecutablePathAnsi(block.data(), block.size());
    if (found.empty())
        Dbg(L"AppInfo request block: no existing path found in it (turn dumpRequestBlock on to "
            L"see the bytes)");
    else
        Dbg(L"AppInfo request block: path '%s'", found.c_str());
    return found;
}

// ---------------------------------------------------------------------------------------------
// The digital signature of the elevated file, read with WinVerifyTrust - the documented API for
// exactly this question. wintrust.dll is loaded at run time, so nothing is added to the linker
// flags, and the answer only decides which shield the banner shows: nothing here approves,
// refuses or changes anything.
// ---------------------------------------------------------------------------------------------
enum SignatureState {
    SigUnknown = -1,   // no file to read, or wintrust is not usable
    SigTrusted = 0,    // signed, and the chain is trusted
    SigUnsigned = 1,   // no signature at all
    SigInvalid = 2,    // a signature is there and is not valid, not trusted, or distrusted
};

typedef LONG (WINAPI *WinVerifyTrustFn)(HWND, GUID*, LPVOID);

typedef CRYPT_PROVIDER_DATA* (WINAPI *WTHelperProvDataFromStateDataFn)(HANDLE);
typedef CRYPT_PROVIDER_SGNR* (WINAPI *WTHelperGetProvSignerFromChainFn)(
    CRYPT_PROVIDER_DATA*, DWORD, BOOL, DWORD);
typedef CRYPT_PROVIDER_CERT* (WINAPI *WTHelperGetProvCertFromChainFn)(
    CRYPT_PROVIDER_SGNR*, DWORD);

// Read the signer identity from the state belonging to a successful WinVerifyTrust call. These
// documented WinTrust helpers are resolved dynamically; if a build does not publish them, the
// native "Unknown" value is kept. The certificate is never trusted by this helper alone: this
// function is called only after the Authenticode provider has returned success.
static std::wstring TrustedSignerNameFromState(HMODULE wintrust, HANDLE state) {
    if (!wintrust || !state) return L"";
    auto getProvider = (WTHelperProvDataFromStateDataFn)GetProcAddress(
        wintrust, "WTHelperProvDataFromStateData");
    auto getSigner = (WTHelperGetProvSignerFromChainFn)GetProcAddress(
        wintrust, "WTHelperGetProvSignerFromChain");
    auto getCert = (WTHelperGetProvCertFromChainFn)GetProcAddress(
        wintrust, "WTHelperGetProvCertFromChain");
    if (!getProvider || !getSigner || !getCert) return L"";

    CRYPT_PROVIDER_DATA* provider = getProvider(state);
    if (!provider) return L"";
    CRYPT_PROVIDER_SGNR* signer = getSigner(provider, 0, FALSE, 0);
    if (!signer || signer->dwError != ERROR_SUCCESS) return L"";
    CRYPT_PROVIDER_CERT* cert = getCert(signer, 0);
    if (!cert || cert->dwError != ERROR_SUCCESS || !cert->pCert) return L"";

    WCHAR name[256] = {};
    DWORD required = CertGetNameStringW(cert->pCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr,
                                        nullptr, 0);
    if (required < 2 || required > ARRAYSIZE(name)) return L"";
    DWORD written = CertGetNameStringW(cert->pCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr,
                                       name, required);
    if (written < 2 || written > required) return L"";
    std::wstring result = Trim(name);
    if (result.empty() || result.size() > 255 || IsJunkText(result)) return L"";
    for (WCHAR ch : result) {
        if (ch < 32) return L"";
    }
    return result;
}

// wintrust.dll, loaded on demand: the mod does not link against it, and the library is only
// needed when a signature is really read.
static HMODULE WinTrustDll() {
    static HMODULE dll = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        dll = LoadLibraryExW(L"wintrust.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    return dll;
}

// WinVerifyTrust, resolved once and kept: the answer only decides which shield the banner
// shows, and the prompt must not wait for a library to be loaded twice.
static WinVerifyTrustFn WinVerifyTrustApi() {
    static WinVerifyTrustFn fn = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        HMODULE dll = WinTrustDll();
        if (dll) fn = (WinVerifyTrustFn)GetProcAddress(dll, "WinVerifyTrust");
        if (!fn) Dbg(L"signature: wintrust.dll is not usable - the state stays unknown");
    }
    return fn;
}

// ---------------------------------------------------------------------------------------------
// The security catalog: where the signature of a component of Windows usually is.
//
// Most of the binaries in %SystemRoot%\System32 (notepad.exe, regedit.exe, taskmgr.exe and
// hundreds of others) carry no embedded Authenticode signature: what makes them signed is their
// hash, listed in a signed catalog file of the system (C:\Windows\System32\CatRoot).
// WinVerifyTrust reads the embedded signature only, so for those files it answers
// TRUST_E_NOSIGNATURE - "no signature" - even though Windows considers the file signed and
// shows "Verified publisher: Microsoft Windows" for it.
//
// The catalog administrator (the documented CryptCATAdmin* family of wintrust.dll) answers the
// real question - "is this file a member of a catalog?" - and the member is then verified with
// WinVerifyTrust exactly like any other signed object (dwUnionChoice = WTD_CHOICE_CATALOG), so
// a file that is not really listed in a trusted catalog cannot pass.
//
// Everything here is read-only: the file is opened for reading, the catalogs are read from the
// catalog database of the system, and the answer only decides which shield the banner shows.
// Nothing approves, refuses or changes anything.
// ---------------------------------------------------------------------------------------------

// The handles of the catalog administrator. The SDK headers do not declare them in every
// version, and both of them are pointers in every version: they are spelled out here so that
// the mod does not depend on the headers of the machine it happens to be built on.
typedef HANDLE CatAdminHandle;   // HCATADMIN
typedef HANDLE CatInfoHandle;    // HCATINFO

// The layout of CATALOG_INFO. The only field this mod reads is wszCatalogFile, and that one is
// a wide string in every version of the structure; the three members that follow it belong to
// the member information, which is not asked for here - only the basic information of the
// catalog is.
struct CatalogInfoLocal {
    DWORD cbStruct;
    WCHAR wszCatalogFile[MAX_PATH];
    void* szMemberTag;
    void* szMemberFilePath;
    void* szMemberFile;
    FILETIME ftMemberFile;
};

// The layout of WINTRUST_CATALOG_INFO. The hCatAdmin member is the one Windows 8 and later
// need for a hash that is not a SHA-1 one; the structure is written by this mod and read by
// the trust provider of the system, which is told how much of it there is through cbStruct.
struct CatalogTrustInfoLocal {
    DWORD cbStruct;
    DWORD dwCatalogVersion;
    LPCWSTR pcwszCatalogFilePath;
    LPCWSTR pcwszMemberTag;
    LPCWSTR pcwszMemberFilePath;
    HANDLE hMemberFile;
    BYTE* pbCalculatedFileHash;
    DWORD cbCalculatedFileHash;
    void* pcCatalogContext;
    HANDLE hCatAdmin;
};

typedef BOOL (WINAPI* CatAcquireContextFn)(CatAdminHandle*, const GUID*, DWORD);
typedef BOOL (WINAPI* CatAcquireContext2Fn)(CatAdminHandle*, const GUID*, LPCWSTR, void*, DWORD);
typedef BOOL (WINAPI* CatCalcHashFn)(HANDLE, DWORD*, BYTE*, DWORD);
typedef BOOL (WINAPI* CatCalcHash2Fn)(CatAdminHandle, HANDLE, DWORD*, BYTE*, DWORD);
typedef CatInfoHandle (WINAPI* CatEnumCatalogsFn)(CatAdminHandle, BYTE*, DWORD, DWORD,
                                                  CatInfoHandle*);
typedef BOOL (WINAPI* CatCatalogInfoFn)(CatInfoHandle, CatalogInfoLocal*, DWORD);
typedef BOOL (WINAPI* CatReleaseCatalogFn)(CatAdminHandle, CatInfoHandle, DWORD);
typedef BOOL (WINAPI* CatReleaseContextFn)(CatAdminHandle, DWORD);

// The catalog administrator of wintrust.dll, resolved once and never linked against. A build of
// Windows that does not have one of these functions simply does not get that particular way of
// asking: the file is then in no catalog as far as the mod is concerned, which is exactly what
// the mod believed before this existed.
struct CatalogApi {
    HMODULE dll = nullptr;
    CatAcquireContextFn acquire = nullptr;
    CatAcquireContext2Fn acquire2 = nullptr;
    CatCalcHashFn calcHash = nullptr;
    CatCalcHash2Fn calcHash2 = nullptr;
    CatEnumCatalogsFn enumCatalogs = nullptr;
    CatCatalogInfoFn catalogInfo = nullptr;
    CatReleaseCatalogFn releaseCatalog = nullptr;
    CatReleaseContextFn releaseContext = nullptr;
    bool usable = false;
};

static const CatalogApi& CatalogApis() {
    static CatalogApi api = []() -> CatalogApi {
        CatalogApi a;
        a.dll = WinTrustDll();
        if (!a.dll) return a;
        a.acquire = (CatAcquireContextFn)GetProcAddress(a.dll, "CryptCATAdminAcquireContext");
        a.acquire2 = (CatAcquireContext2Fn)GetProcAddress(a.dll, "CryptCATAdminAcquireContext2");
        a.calcHash = (CatCalcHashFn)GetProcAddress(a.dll, "CryptCATAdminCalcHashFromFileHandle");
        a.calcHash2 = (CatCalcHash2Fn)GetProcAddress(a.dll, "CryptCATAdminCalcHashFromFileHandle2");
        a.enumCatalogs = (CatEnumCatalogsFn)GetProcAddress(a.dll, "CryptCATAdminEnumCatalogFromHash");
        a.catalogInfo = (CatCatalogInfoFn)GetProcAddress(a.dll, "CryptCATCatalogInfoFromContext");
        a.releaseCatalog =
            (CatReleaseCatalogFn)GetProcAddress(a.dll, "CryptCATAdminReleaseCatalogContext");
        a.releaseContext = (CatReleaseContextFn)GetProcAddress(a.dll, "CryptCATAdminReleaseContext");
        a.usable = a.acquire && a.calcHash && a.enumCatalogs && a.catalogInfo &&
                   a.releaseCatalog && a.releaseContext;
        if (!a.usable)
            Dbg(L"catalog: the catalog administrator of this build of Windows is not complete - "
                L"the catalogs of the system cannot be read");
        return a;
    }();
    return api;
}

// The tag of a catalog member is the hash of the file, written as hexadecimal.
static std::wstring CatalogMemberTag(const BYTE* hash, DWORD len) {
    static const WCHAR kHex[] = L"0123456789ABCDEF";
    std::wstring tag;
    tag.reserve((size_t)len * 2);
    for (DWORD i = 0; i < len; i++) {
        tag.push_back(kHex[(hash[i] >> 4) & 0x0F]);
        tag.push_back(kHex[hash[i] & 0x0F]);
    }
    return tag;
}

// The membership of the file in the catalogs of the system, with a hash that has already been
// computed. The catalog that lists it is then verified by the trust provider of the system,
// exactly like any other signed object: a catalog that is not trusted, or that does not really
// list the file, does not pass.
static int VerifyCatalogMember(const std::wstring& path, const std::wstring& memberTag,
                                const BYTE* hash, DWORD hashLen, CatAdminHandle admin,
                                bool handOverTheAdminContext, std::wstring* catalogFile) {
    const CatalogApi& api = CatalogApis();
    if (catalogFile) catalogFile->clear();
    WinVerifyTrustFn verify = WinVerifyTrustApi();
    if (!verify || !api.enumCatalogs || !api.catalogInfo || !admin) return SigUnknown;

    int result = SigUnknown;
    CatInfoHandle previous = nullptr;
    bool held = false;               // the handle in `previous` is still ours to release
    // A bounded number of catalogs: the first one that verifies is the answer, and a file that
    // is in no catalog at all is found in none of them.
    for (int attempt = 0; attempt < 8; attempt++) {
        CatInfoHandle info = api.enumCatalogs(admin, const_cast<BYTE*>(hash), hashLen, 0,
                                              held ? &previous : nullptr);
        if (!info) {
            // The enumeration is over: the catalog administrator releases the handle of the
            // previous catalog by itself, and releasing it here as well would release it twice.
            held = false;
            break;
        }
        previous = info;
        held = true;

        CatalogInfoLocal infoStruct = {};
        infoStruct.cbStruct = sizeof(infoStruct);
        if (!api.catalogInfo(info, &infoStruct, 0)) continue;
        std::wstring catalog = infoStruct.wszCatalogFile;
        if (catalog.empty()) continue;
        if (GetFileAttributesW(catalog.c_str()) == INVALID_FILE_ATTRIBUTES) continue;

        CatalogTrustInfoLocal catalogInfo = {};
        catalogInfo.cbStruct = sizeof(catalogInfo);
        catalogInfo.pcwszCatalogFilePath = catalog.c_str();
        catalogInfo.pcwszMemberTag = memberTag.c_str();
        catalogInfo.pcwszMemberFilePath = path.c_str();
        // The context has to be handed over whenever the hash is not a SHA-1 one: Windows 8 and
        // later need it to know which catalog database the member comes from.
        catalogInfo.hCatAdmin = handOverTheAdminContext ? (HANDLE)admin : nullptr;

        WINTRUST_DATA data = {};
        data.cbStruct = sizeof(data);
        data.dwUIChoice = WTD_UI_NONE;                 // never a dialog of its own
        data.fdwRevocationChecks = WTD_REVOKE_NONE;    // no network round trip
        data.dwUnionChoice = WTD_CHOICE_CATALOG;
        data.pCatalog = reinterpret_cast<WINTRUST_CATALOG_INFO*>(&catalogInfo);
        data.dwStateAction = WTD_STATEACTION_VERIFY;
        data.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL;

        GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
        LONG status = verify(nullptr, &action, &data);
        data.dwStateAction = WTD_STATEACTION_CLOSE;
        verify(nullptr, &action, &data);

        if (status == ERROR_SUCCESS) {
            result = SigTrusted;
            if (catalogFile) *catalogFile = catalog;
            break;
        }
        // The file is listed in a catalog, but that catalog does not verify: the file is not
        // trusted, and saying so is more honest than saying "no signature".
        result = SigInvalid;
        if (catalogFile) *catalogFile = catalog;
    }
    // The handle of the catalog that was still open when the loop ended: only in that case it
    // has not been released by the enumeration itself.
    if (held && previous && api.releaseCatalog) api.releaseCatalog(admin, previous, 0);
    return result;
}

// The signature of the file in the security catalog of the system. Two hashes are tried, in
// this order: the SHA-256 one that Windows 8 and later use for the files of the system, and
// the SHA-1 one that the older catalogs carry. The answer is SigTrusted, SigInvalid or
// SigUnknown - never "unsigned": a file that is in no catalog at all is not a file of the
// system, and the embedded signature is the one that decides about it.
static int VerifyCatalogSignature(const std::wstring& path, std::wstring* catalogFile) {
    if (catalogFile) catalogFile->clear();
    if (path.empty()) return SigUnknown;
    const CatalogApi& api = CatalogApis();
    if (!api.usable) return SigUnknown;

    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                               nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Dbg(L"catalog: the file could not be opened for reading: %u", GetLastError());
        return SigUnknown;
    }

    // The catalog database of the system: the one the operating system uses for its own
    // components. DRIVER_ACTION_VERIFY is the documented identifier of that database.
    static const GUID kSystemCatalogs = {
        0xF750E6C3, 0x38EE, 0x11D1, {0x85, 0xE5, 0x00, 0xC0, 0x4F, 0xC2, 0x95, 0xEE}};

    int result = SigUnknown;
    std::wstring foundCatalog;

    // The modern way: the context chooses the algorithm, and the hash of the file is computed
    // with it. Windows 8 and later need that context for the verification itself.
    if (api.acquire2 && api.calcHash2) {
        CatAdminHandle admin = nullptr;
        if (api.acquire2(&admin, &kSystemCatalogs, L"SHA256", nullptr, 0) && admin) {
            DWORD hashLen = 0;
            if (api.calcHash2(admin, file, &hashLen, nullptr, 0) && hashLen && hashLen <= 64) {
                std::vector<BYTE> hash(hashLen);
                if (api.calcHash2(admin, file, &hashLen, hash.data(), 0)) {
                    int r = VerifyCatalogMember(path, CatalogMemberTag(hash.data(), hashLen),
                                                hash.data(), hashLen, admin, true, &foundCatalog);
                    if (r == SigTrusted) result = SigTrusted;
                    else if (r == SigInvalid && result != SigTrusted) result = SigInvalid;
                }
            }
            if (api.releaseContext) api.releaseContext(admin, 0);
        }
    }

    // The older way: the SHA-1 hash of the file, and the context of the same database. The
    // systems that still carry the older catalogs answer this one.
    if (result != SigTrusted && api.acquire && api.calcHash) {
        CatAdminHandle admin = nullptr;
        if (api.acquire(&admin, &kSystemCatalogs, 0) && admin) {
            DWORD hashLen = 0;
            if (api.calcHash(file, &hashLen, nullptr, 0) && hashLen && hashLen <= 64) {
                std::vector<BYTE> hash(hashLen);
                if (api.calcHash(file, &hashLen, hash.data(), 0)) {
                    int r = VerifyCatalogMember(path, CatalogMemberTag(hash.data(), hashLen),
                                                hash.data(), hashLen, admin, false, &foundCatalog);
                    if (r == SigTrusted) result = SigTrusted;
                    else if (r == SigInvalid && result != SigTrusted) result = SigInvalid;
                }
            }
            if (api.releaseContext) api.releaseContext(admin, 0);
        }
    }

    CloseHandle(file);
    if (result == SigTrusted || result == SigInvalid) {
        if (catalogFile) *catalogFile = foundCatalog;
        Dbg(L"catalog: the file is listed in '%s' (%s)", foundCatalog.c_str(),
            result == SigTrusted ? L"the catalog is trusted" : L"the catalog is not trusted");
    } else {
        Dbg(L"catalog: the file is in no catalog of the system");
    }
    return result;
}

static int VerifySignatureState(const std::wstring& path, std::wstring* verifiedSignerName,
                                 bool askTheCatalog) {
    if (verifiedSignerName) verifiedSignerName->clear();
    if (path.empty()) return SigUnknown;
    HMODULE wintrust = WinTrustDll();
    WinVerifyTrustFn fn = WinVerifyTrustApi();
    if (!fn) return SigUnknown;

    WINTRUST_FILE_INFO file = {};
    file.cbStruct = sizeof(file);
    file.pcwszFilePath = path.c_str();

    WINTRUST_DATA data = {};
    data.cbStruct = sizeof(data);
    data.dwUIChoice = WTD_UI_NONE;                 // never a dialog of its own
    data.fdwRevocationChecks = WTD_REVOKE_NONE;    // no network round trip: the prompt must not wait
    data.dwUnionChoice = WTD_CHOICE_FILE;
    data.pFile = &file;
    data.dwStateAction = WTD_STATEACTION_VERIFY;
    data.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL;   // documented: the cache only, never the wire

    GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    LONG status = fn(nullptr, &action, &data);

    if (status == ERROR_SUCCESS && verifiedSignerName) {
#if W7UAC_HAS_EXCEPTIONS
        try {
            *verifiedSignerName = TrustedSignerNameFromState(wintrust, data.hWVTStateData);
        } catch (...) {
            verifiedSignerName->clear();
            Dbg(L"signature: the verified signer name could not be copied; the prompt text is kept");
        }
#else
        *verifiedSignerName = TrustedSignerNameFromState(wintrust, data.hWVTStateData);
#endif
    }

    // Release provider state only after the optional, read-only signer lookup above.
    data.dwStateAction = WTD_STATEACTION_CLOSE;
    fn(nullptr, &action, &data);

    if (status == ERROR_SUCCESS) return SigTrusted;
    DWORD code = (DWORD)status;
    if (code == (DWORD)TRUST_E_NOSIGNATURE) {
        Dbg(L"signature: WinVerifyTrust reports no signature (0x%08X)", (unsigned)code);
        // A component of Windows carries its signature in the security catalog of the system,
        // and WinVerifyTrust does not read the catalogs: the catalog is asked instead. The
        // question is asked only when it can change the answer, that is when the native prompt
        // says the publisher is verified and the file has no signature of its own.
        if (askTheCatalog) {
            std::wstring catalog;
            int catalogState = VerifyCatalogSignature(path, &catalog);
            if (catalogState == SigTrusted) {
                Dbg(L"signature: the file is a member of the signed catalog '%s'", catalog.c_str());
                return SigTrusted;
            }
            if (catalogState == SigInvalid) {
                Dbg(L"signature: the file is listed in '%s', but that catalog is not trusted",
                    catalog.c_str());
                return SigInvalid;
            }
        }
        return SigUnsigned;
    }
    if (code == (DWORD)TRUST_E_SUBJECT_FORM_UNKNOWN ||
        code == (DWORD)TRUST_E_PROVIDER_UNKNOWN ||
        code == (DWORD)TRUST_E_ACTION_UNKNOWN) {
        Dbg(L"signature: WinVerifyTrust cannot evaluate this subject/provider (0x%08X); state "
            L"unknown, not unsigned", (unsigned)code);
        return SigUnknown;
    }
    return SigInvalid;
}

// The file that is being elevated, from the places that actually carry it: the block AppInfo
// hands to consent.exe, and the strings the prompt publishes (unfolded or not). A path that does
// not exist is only accepted when nothing better is there, and nothing is ever invented.
static std::wstring FindTargetPath(const PromptInfo& info, const WCHAR** source) {
    *source = L"none";
    for (const auto& p : info.paths) {
        if (EndsWithExecExt(p) && GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES) {
            *source = L"a string of the prompt";
            return p;
        }
    }
    for (const auto& p : info.paths) {
        if (EndsWithExecExt(p)) {
            *source = L"a string of the prompt (not readable on disk)";
            return p;
        }
    }

    std::wstring fromBlock = TargetPathFromRequestBlock();
    if (!fromBlock.empty()) {
        *source = L"the AppInfo request block";
        return fromBlock;
    }

    // Last resort: a path on consent.exe's own command line. Current builds pass the request as
    // a pointer instead of as text, so this usually finds nothing but consent.exe itself.
    const WCHAR* cl = GetCommandLineW();
    std::wstring cmd = cl ? cl : L"";
    size_t pos = 0;
    while (pos < cmd.size()) {
        std::wstring token;
        if (cmd[pos] == L' ') { pos++; continue; }
        if (cmd[pos] == L'"') {
            size_t end = cmd.find(L'"', pos + 1);
            if (end == std::wstring::npos) break;
            token = cmd.substr(pos + 1, end - pos - 1);
            pos = end + 1;
        } else {
            size_t sp = cmd.find(L' ', pos);
            token = cmd.substr(pos,
                               (sp == std::wstring::npos) ? std::wstring::npos : sp - pos);
            pos = (sp == std::wstring::npos) ? cmd.size() : sp + 1;
        }
        if (!EndsWithExecExt(token)) continue;
        if (ContainsI(token, L"consent.exe")) continue;      // that one is us
        if (GetFileAttributesW(token.c_str()) != INVALID_FILE_ATTRIBUTES) {
            *source = L"the command line";
            return token;
        }
    }
    return std::wstring();
}

// An explicit unknown value is evidence too, even when the row mapper could not attach it to a
// label. This is kept separate from the presentation fields because the Regedit exception is only
// allowed after all native text currently available has been checked.
static bool TextListHasUnknownWording(const std::vector<std::wstring>& texts) {
    for (const auto& t : texts)
        if (TextHasAnyWord(t, kUnknownWords, ARRAYSIZE(kUnknownWords))) return true;
    return false;
}

static bool TextListHasRejectedPublisherLabel(const std::vector<std::wstring>& texts) {
    for (const auto& t : texts) {
        std::wstring label, value;
        SplitLabelValue(t, label, value);
        // Without a colon there is no reliable way to tell a standalone program name such as
        // "Registry Editor" from a row label. MapPromptRows has the geometry for that case;
        // details have only text, so require an explicit label/value form or an unambiguous
        // standalone publisher label here.
        if (label.empty()) {
            if (!IsStandalonePublisherLabel(t)) continue;
            label = Trim(t);
        }
        if (!TextHasAnyWord(label, kPubRowWords, ARRAYSIZE(kPubRowWords)) &&
            !LooksLikePublisher(label) && !IsStandalonePublisherLabel(label))
            continue;
        if (!LabelSaysVerified(label)) return true;
    }
    return false;
}

static bool PromptHasUnknownWording(const PromptInfo& info) {
    if (TextListHasUnknownWording(info.texts) ||
        TextListHasUnknownWording(info.detailTexts))
        return true;
    if (!info.publisherText.empty() &&
        (TextHasAnyWord(info.publisherText, kUnknownWords, ARRAYSIZE(kUnknownWords)) ||
         IsUnknownPublisherValue(info.publisherText)))
        return true;
    return false;
}

static std::wstring NormalizePathForCompare(std::wstring path) {
    path = Trim(path);
    while (!path.empty() && (path.front() == L'"' || path.front() == L'\'')) path.erase(0, 1);
    while (!path.empty() && (path.back() == L'"' || path.back() == L'\'' ||
                             path.back() == L' ' || path.back() == L'\t'))
        path.pop_back();

    std::wstring lower = ToLower(path);
    if (lower.compare(0, 12, L"%systemroot%") == 0) {
        WCHAR expanded[32768] = L"";
        DWORD n = ExpandEnvironmentStringsW(path.c_str(), expanded, ARRAYSIZE(expanded));
        if (n > 1 && n < ARRAYSIZE(expanded)) path.assign(expanded, n - 1);
    }
    if (path.compare(0, 4, L"\\\\?\\") == 0 || path.compare(0, 4, L"\\??\\") == 0)
        path.erase(0, 4);
    for (auto& c : path) if (c == L'/') c = L'\\';
    while (path.size() > 1 && path.back() == L'\\') path.pop_back();
    return ToLower(path);
}

// Extracts a real path from a row that may carry a localized label before it, then compares it
// with the actual Windows-directory copy of regedit.exe. Merely seeing the word "regedit" is not
// enough: the native prompt must expose this exact system path.
static bool PathTextIsSystemRegedit(const std::wstring& text, const std::wstring& expected,
                                    std::wstring* shown) {
    std::wstring lower = ToLower(text);
    size_t search = 0;
    while (search < text.size()) {
        size_t start = std::wstring::npos;
        auto take = [&](size_t candidate) {
            if (candidate != std::wstring::npos &&
                (start == std::wstring::npos || candidate < start))
                start = candidate;
        };
        take(lower.find(L"%systemroot%", search));
        take(lower.find(L"\\\\", search));
        for (size_t i = search; i + 2 < text.size(); i++) {
            WCHAR c = text[i];
            if (((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z')) &&
                text[i + 1] == L':' && (text[i + 2] == L'\\' || text[i + 2] == L'/')) {
                take(i);
                break;
            }
        }
        if (start == std::wstring::npos) return false;
        size_t dot = lower.find(L".exe", start);
        if (dot == std::wstring::npos) return false;
        std::wstring candidate = text.substr(start, dot + 4 - start);
        if (NormalizePathForCompare(candidate) == expected) {
            if (shown) *shown = candidate;
            return true;
        }
        search = start + 1;
    }
    return false;
}

static bool PromptShowsSystemRegeditPath(const PromptInfo& info, std::wstring* path) {
    WCHAR win[MAX_PATH] = L"";
    if (!GetWindowsDirectoryW(win, ARRAYSIZE(win))) return false;
    std::wstring expected = NormalizePathForCompare(std::wstring(win) + L"\\regedit.exe");
    const std::vector<std::wstring>* sources[] = {&info.paths, &info.texts, &info.detailTexts};
    for (const auto* source : sources) {
        for (const auto& text : *source) {
            std::wstring shown;
            if (PathTextIsSystemRegedit(text, expected, &shown)) {
                if (path) *path = std::wstring(win) + L"\\regedit.exe";
                return true;
            }
        }
    }
    return false;
}

// The native tree can map an adjacent value to the program row (the observed result was
// "Verticale" for C:\Windows\regedit.exe). That is an unacceptable presentation failure: the
// program-name row is not a trust decision, so normalize it for every supported mod language as
// soon as the request is identified as Registry Editor. The publisher/signature override remains
// separately gated below.
static const WCHAR* LocalizedRegeditName() {
    if (!_wcsicmp(g_loc->lang, L"it")) return L"Editor del Registro";
    if (!_wcsicmp(g_loc->lang, L"de")) return L"Registrierungs-Editor";
    if (!_wcsicmp(g_loc->lang, L"fr")) return L"\u00c9diteur du Registre";
    if (!_wcsicmp(g_loc->lang, L"es")) return L"Editor del Registro";
    if (!_wcsicmp(g_loc->lang, L"pt")) return L"Editor do Registro";
    if (!_wcsicmp(g_loc->lang, L"nl")) return L"Register-editor";
    if (!_wcsicmp(g_loc->lang, L"pl")) return L"Edytor rejestru";
    if (!_wcsicmp(g_loc->lang, L"ru")) return L"\u0420\u0435\u0434\u0430\u043a\u0442\u043e\u0440 \u0440\u0435\u0435\u0441\u0442\u0440\u0430";
    if (!_wcsicmp(g_loc->lang, L"ja")) return L"\u30ec\u30b8\u30b9\u30c8\u30ea \u30a8\u30c7\u30a3\u30bf\u30fc";
    if (!_wcsicmp(g_loc->lang, L"ko")) return L"\ub808\uc9c0\uc2a4\ud2b8\ub9ac \ud3b8\uc9d1\uae30";
    if (!_wcsicmp(g_loc->lang, L"hi")) return L"\u0930\u091c\u093f\u0938\u094d\u091f\u094d\u0930\u0940 \u0938\u0902\u092a\u093e\u0926\u0915";
    if (!_wcsicmp(g_loc->lang, L"tr")) return L"Kay\u0131t Defteri D\u00fczenleyicisi";
    if (!_wcsicmp(g_loc->lang, L"fa")) return L"\u0648\u06cc\u0631\u0627\u06cc\u0634\u06af\u0631 \u0631\u062c\u06cc\u0633\u062a\u0631\u06cc";
    if (!_wcsicmp(g_loc->lang, L"ar")) return L"\u0645\u062d\u0631\u0631 \u0627\u0644\u0633\u062c\u0644";
    return L"Registry Editor";
}

static void ApplyRegeditNameOverride(PromptInfo& info) {
    if (info.regeditDetected) info.appText = LocalizedRegeditName();
}

static bool ApplyRegeditOverrideIfSafe(PromptInfo& info) {
    if (!info.regeditDetected || info.regeditOverrideApplied) return false;

    info.sawUnknownWording = PromptHasUnknownWording(info);
    bool rejectedLabel = info.publisherRowLabelRejected ||
                         TextListHasRejectedPublisherLabel(info.detailTexts);
    // A value without a readable publisher label is treated as unreadable. A labelled row is not
    // overridden unless it is absent, empty, or otherwise unavailable; a label rejected by
    // LabelSaysVerified is explicit contrary evidence and always blocks the exception.
    bool publisherRowMissingOrUnreadable = !info.publisherRowLabelSeen ||
                                             !info.publisherRowValueReadable;
    if (!publisherRowMissingOrUnreadable || info.sawUnknownWording || rejectedLabel) {
        Dbg(L"regedit presentation not applied: native publisher evidence is present, unknown, "
            L"or explicitly unverified (row=%d rejected=%d unknown=%d)",
            info.publisherRowLabelSeen ? 1 : 0, rejectedLabel ? 1 : 0,
            info.sawUnknownWording ? 1 : 0);
        return false;
    }

    std::wstring systemPath;
    if (!PromptShowsSystemRegeditPath(info, &systemPath)) {
        Dbg(L"regedit text found, but the prompt did not show the system path -> no override");
        return false;
    }

    // The name-only normalization above is also repeated here so the gated path keeps the same
    // localized value when it supplies the publisher/path presentation.
    info.appText = LocalizedRegeditName();
    info.publisherText = L"Microsoft Windows";
    info.appPath = systemPath;
    info.originText = systemPath;
    info.systemFileConfirmed = true;       // path identity only; VerifySignatureState still runs
    info.regeditOverrideApplied = true;
    info.publisherVerifiedByWindows = false; // becomes true only after a trusted signature result
    info.publisherFromSignature = false;
    info.sawUnknownAuthor = false;
    info.signatureState = SigUnknown;
    info.bannerState = (info.yes && !info.yesEnabled) ? 3 : 2;
    Dbg(L"regedit presentation allowed: prompt showed '%s'; publisher row was missing or unreadable; "
        L"the system file will still be verified", systemPath.c_str());
    return true;
}

// "Yes" and "No" are resolved here, on the finished element set, from the buttons' own text.
// Nothing is carried across a vector that has been appended to, and a button that cannot be
// recognised by its own text leaves the pair incomplete: the mod then fails open instead of
// guessing a position. The automation id of the classic dialog template is only accepted for
// "No" when no name matched at all, never for "Yes".
static void ResolveButtons(PromptInfo& info) {
    info.yes = nullptr;
    info.no = nullptr;
    for (auto& e : info.elements) {
        if (e.type != UIA_ButtonControlTypeId || e.name.empty()) continue;
        if (!info.yes && NameIsYes(e.name)) info.yes = &e;
        else if (!info.no && NameIsNo(e.name)) info.no = &e;
    }
    if (!info.yes && !info.no) {
        for (auto& e : info.elements) {
            if (e.type != UIA_ButtonControlTypeId) continue;
            if (ContainsI(e.automationId, L"cancel")) { info.no = &e; break; }
        }
    }
    info.buttonsConfident = (info.yes && info.no && info.yes != info.no);
    info.yesEnabled = info.yes ? (info.yes->enabled != FALSE) : false;

    if (info.no && info.hDefaultButton) {
        RECT dr = {};
        bool samePlace = false;
        if (GetWindowRect(info.hDefaultButton, &dr))
            samePlace = (info.no->rect.left == dr.left && info.no->rect.top == dr.top);
        if (!samePlace)
            Dbg(L"buttons: '%s' is not the native default button - the identity check disagrees",
                info.no->name.c_str());
    }
    if (!info.yes || !info.no)
        Log(L"buttons: the native pair could not be identified by name (yes=%d no=%d) - the mod "
            L"fails open rather than guessing", info.yes ? 1 : 0, info.no ? 1 : 0);
}

// The publisher shown is the one in the native publisher row. The only presentation exception is
// the signature-backed Regedit path, and it is applied only when the native publisher row is
// missing or unreadable and no explicit unknown/unverified wording is present.
static void FinishHarvest(PromptInfo& info) {
    ResolveButtons(info);

    for (const auto& t : info.texts) {
        if (t.find(L":\\") != std::wstring::npos || t.find(L"\\\\") != std::wstring::npos)
            info.paths.push_back(t);
    }

    MapPromptRows(info);

    // ------------------------------------------------------------------------
    // REGEDIT DETECTION (hardcoded, across all 15 languages of the mod).
    // ------------------------------------------------------------------------
    // This section is fundamental. After multiple attempts of trying to avoid it, there
    // have been various regressions on the author's side, such as the name of regedit being shown 
    // as "Verticale". This is not acceptable for a mod that tries to recreate a feature. For this reason,
    // this override is kept unless someone who knows a proper method (and has tested it on Windows 10 or Windows 11)
    // provides it.
    auto isRegeditText = [](const std::wstring& t) {
        return ContainsI(t, L"regedit")
            || ContainsI(t, L"Registry Editor")          // en
            || ContainsI(t, L"Editor del Registro")      // it
            || ContainsI(t, L"Editor del registro")      // it (alt)
            || ContainsI(t, L"Registrierungs-Editor")    // de
            || ContainsI(t, L"Registrierungseditor")     // de (without hyphen, tolerance)
            || ContainsI(t, L"\u00c9diteur du Registre") // fr
            || ContainsI(t, L"Editor del Registro")      // es
            || ContainsI(t, L"Editor do Registro")       // pt
            || ContainsI(t, L"Register-editor")          // nl
            || ContainsI(t, L"Edytor rejestru")          // pl
            || ContainsI(t, L"\u0420\u0435\u0434\u0430\u043a\u0442\u043e\u0440 \u0440\u0435\u0435\u0441\u0442\u0440\u0430") // ru
            || ContainsI(t, L"\u30ec\u30b8\u30b9\u30c8\u30ea \u30a8\u30c7\u30a3\u30bf\u30fc") // ja
            || ContainsI(t, L"\ub808\uc9c0\uc2a4\ud2b8\ub9ac \ud3b8\uc9d1\uae30")             // ko
            || ContainsI(t, L"\u0930\u091c\u093f\u0938\u094d\u091f\u094d\u0930\u0940 \u0938\u0902\u092a\u093e\u0926\u0915") // hi
            || ContainsI(t, L"Kay\u0131t Defteri D\u00fczenleyicisi") // tr
            || ContainsI(t, L"\u0648\u06cc\u0631\u0627\u06cc\u0634\u06af\u0631 \u0631\u062c\u06cc\u0633\u062a\u0631\u06cc") // fa
            || ContainsI(t, L"\u0645\u062d\u0631\u0631 \u0627\u0644\u0633\u062c\u0644") // ar
            ;
    };

    bool isRegedit = isRegeditText(info.appText) || isRegeditText(info.windowTitle);
    if (!isRegedit)
        for (const auto& t : info.texts) if (isRegeditText(t)) { isRegedit = true; break; }
    if (!isRegedit)
        for (const auto& t : info.detailTexts) if (isRegeditText(t)) { isRegedit = true; break; }
    if (!isRegedit)
        for (const auto& p : info.paths) if (isRegeditText(p)) { isRegedit = true; break; }
    if (!isRegedit && !info.publisherText.empty() && isRegeditText(info.publisherText))
        isRegedit = true;

    info.regeditDetected = isRegedit;
    ApplyRegeditNameOverride(info);
    info.sawUnknownWording = PromptHasUnknownWording(info);
    // The Regedit presentation is deliberately deferred until the native details have been read.
    // That prevents a later-expanded "unknown" or unverified publisher row from being hidden by
    // the compatibility name match.

    // ------------------------------------------------------------------------

    // Which file is being elevated. It feeds the program icon, the "File origin" line and the
    // details pane, so it is looked for in two places and never invented.
    if (!info.regeditOverrideApplied) {
        const WCHAR* pathSource = L"none";
        info.appPath = FindTargetPath(info, &pathSource);
        info.originText = info.appPath;
        if (info.appPath.empty()) {
            const WCHAR* cl = GetCommandLineW();
            Dbg(L"elevated file: none of the readable sources carry it (request block reading=%d, "
                L"command line='%s')", g_settings.readRequestBlock ? 1 : 0, cl ? cl : L"");
        } else {
            Dbg(L"elevated file: source=%s path='%s'", pathSource, info.appPath.c_str());
        }
    }

    // Windows 7 paints the banner from the publisher: blue when the publisher is verified,
    // amber (with black text) when it is not. The decision is therefore taken from the publisher
    // row that was really mapped - a stray "unknown publisher" elsewhere in the tree must not
    // repaint a signed program in amber.
    bool sawUnknownWording = info.sawUnknownWording;
    info.sawUnknownAuthor = TextHasAnyWord(info.publisherText, kAuthorWords,
                                           ARRAYSIZE(kAuthorWords));
    bool publisherKnown = info.publisherVerifiedByWindows && !info.sawUnknownAuthor && !IsUnknownPublisherValue(info.publisherText);

    // The native row says that the publisher is verified and names "Microsoft Windows": this is
    // a component of Windows, and Windows 7 gives it the full shield. The verdict is the
    // operating system's own, and the mod's own signature check must never contradict it: it
    // can only make the prompt more cautious, never more trusted. The check reads the embedded
    // Authenticode signature and, when there is none, the security catalog of the system, so
    // that a component of Windows is not mistaken for an unsigned file.
    info.nativeWindowsComponent = publisherKnown && IsWindowsPublisherText(info.publisherText);

    // Blue only when Windows' own publisher row says "verified"; everything else is amber.
    if (info.yes && !info.yesEnabled) {
        info.bannerState = 3;                       // the native Yes button is disabled: blocked
    } else {
        info.bannerState = publisherKnown ? 1 : 2;
    }
    // A manual choice can only make the prompt look more cautious (1 blue < 2 amber < 3 red).
    if (g_settings.bannerOverride >= 1 && g_settings.bannerOverride <= 3 &&
        g_settings.bannerOverride > info.bannerState)
        info.bannerState = g_settings.bannerOverride;

    // A prompt that asks for a password is a credential prompt whatever else it looks like.
    static const WCHAR* kPasswordWords[] = {L"password", L"passwort", L"mot de passe",
                                            L"contrase\u00f1a", L"wachtwoord", L"senha",
                                            L"has\u0142o",
                                            L"\u043f\u0430\u0440\u043e\u043b\u044c"};
    for (const auto& t : info.texts) {
        if (TextHasAnyWord(t, kPasswordWords, ARRAYSIZE(kPasswordWords))) {
            info.credentialVariant = true;
            break;
        }
    }

    Dbg(L"harvest(%s): publisher='%s' known=%d unknownWording=%d path='%s'",
        info.harvestSource, info.publisherText.c_str(), publisherKnown ? 1 : 0,
        sawUnknownWording ? 1 : 0, info.appPath.c_str());
    Log(L"harvest(%s): elements=%d texts=%d confident=%d yesEnabled=%d banner=%d cred=%d "
        L"(publisher mapped=%d, unknown wording seen=%d, file path found=%d)",
        info.harvestSource, (int)info.elements.size(), (int)info.texts.size(),
        info.buttonsConfident, info.yesEnabled, info.bannerState, info.credentialVariant,
        publisherKnown ? 1 : 0, sawUnknownWording ? 1 : 0, info.appPath.empty() ? 0 : 1);
}

//------------------------------------------------------------------------------
// Identity of the requesting application (for the whitelist)
//------------------------------------------------------------------------------

struct Identity {
    std::wstring userDesktopApp;
    std::wstring userDesktopAppClass;
    std::vector<std::wstring> cmdLineTokens;
};

static void CollectIdentity(Identity& id) {
    EnterCriticalSection(&g_cs);
    id.userDesktopApp = g_userDesktopApp;
    id.userDesktopAppClass = g_userDesktopAppClass;
    LeaveCriticalSection(&g_cs);

    const WCHAR* cl = GetCommandLineW();
    std::wstring cmd = cl ? cl : L"";
    size_t pos = 0;
    while (pos < cmd.size()) {
        size_t quote = cmd.find(L'"', pos);
        if (quote != std::wstring::npos) {
            size_t end = cmd.find(L'"', quote + 1);
            if (end == std::wstring::npos) break;
            std::wstring token = cmd.substr(quote + 1, end - quote - 1);
            if (ContainsI(token, L".exe") || ContainsI(token, L".msc") ||
                ContainsI(token, L".cpl") || ContainsI(token, L".bat") ||
                ContainsI(token, L".cmd"))
                id.cmdLineTokens.push_back(token);
            pos = end + 1;
            continue;
        }
        size_t sp = cmd.find(L' ', pos);
        std::wstring token =
            cmd.substr(pos, (sp == std::wstring::npos) ? std::wstring::npos : sp - pos);
        if (ContainsI(token, L".exe") || ContainsI(token, L".msc") || ContainsI(token, L".cpl"))
            id.cmdLineTokens.push_back(token);
        if (sp == std::wstring::npos) break;
        pos = sp + 1;
    }

    Dbg(L"identity: user desktop app='%s' (class '%s'), cmd line: %s", id.userDesktopApp.c_str(),
        id.userDesktopAppClass.c_str(), cmd.c_str());
}

// 1 matched, 0 not matched, -1 undecidable.
static int MatchWhitelist(const Identity& id, const PromptInfo& info, std::wstring* matchedBy) {
    const std::wstring& list = g_settings.appsWhitelist;
    if (list.find(L'*') != std::wstring::npos) { *matchedBy = L"wildcard"; return 1; }

    std::vector<std::wstring> entries;
    size_t pos = 0;
    while (pos <= list.size()) {
        size_t sep = list.find(L';', pos);
        std::wstring e =
            Trim(list.substr(pos, (sep == std::wstring::npos) ? std::wstring::npos : sep - pos));
        if (!e.empty()) entries.push_back(e);
        if (sep == std::wstring::npos) break;
        pos = sep + 1;
    }

    auto stem = [](const std::wstring& e) {
        size_t dot = e.find_last_of(L'.');
        return (dot == std::wstring::npos) ? e : e.substr(0, dot);
    };

    for (const auto& e : entries) {
        if (EqualsI(id.userDesktopApp, e)) {
            *matchedBy = L"app in the foreground: " + id.userDesktopApp;
            return 1;
        }
    }
    for (const auto& e : entries) {
        for (const auto& t : id.cmdLineTokens) {
            if (ContainsI(Basename(t), e)) {
                *matchedBy = L"consent.exe command line: " + t;
                return 1;
            }
        }
    }
    if (!id.userDesktopApp.empty()) {
        for (const auto& e : entries) {
            std::wstring s = stem(e);
            if (s.size() < 3) continue;
            for (const auto& t : info.texts) {
                if (ContainsI(t, s)) {
                    *matchedBy = L"prompt text: " + t;
                    return 1;
                }
            }
        }
    }

    if (id.userDesktopApp.empty() && id.cmdLineTokens.empty()) {
        // Nothing to decide with. Try the prompt texts alone before giving up.
        for (const auto& e : entries) {
            std::wstring s = stem(e);
            if (s.size() < 3) continue;
            for (const auto& t : info.texts)
                if (ContainsI(t, s)) { *matchedBy = L"prompt text: " + t; return 1; }
        }
        return -1;
    }
    return 0;
}

//------------------------------------------------------------------------------
// Relay: perform the decision on the *real* prompt
//------------------------------------------------------------------------------

struct RelayCtx {
    const std::wstring* label;
    HWND found;
};

static BOOL CALLBACK FindButtonByTextProc(HWND hChild, LPARAM lParam) {
    RelayCtx* ctx = (RelayCtx*)lParam;
    WCHAR cls[64] = L"";
    GetClassNameW(hChild, cls, ARRAYSIZE(cls));
    if (_wcsicmp(cls, L"Button") != 0) return TRUE;

    WCHAR text[256] = L"";
    GetWindowTextW(hChild, text, ARRAYSIZE(text));
    if (Trim(text) == Trim(*ctx->label)) {
        ctx->found = hChild;
        return FALSE;
    }
    return TRUE;
}

static bool RelayViaWin32(HWND hNative, const std::wstring& label) {
    if (!hNative || !IsWindow(hNative) || label.empty()) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hNative, &pid);
    if (pid != GetCurrentProcessId()) {
        Log(L"relay: the prompt window is not ours -> refusing to click");
        return false;
    }
    RelayCtx ctx = {&label, nullptr};
    EnumChildWindows(hNative, FindButtonByTextProc, (LPARAM)&ctx);
    if (!ctx.found) {
        Dbg(L"relay: no control with the text '%s'", label.c_str());
        return false;
    }
    DWORD bpid = 0;
    GetWindowThreadProcessId(ctx.found, &bpid);
    if (bpid != GetCurrentProcessId() ||
        GetAncestor(ctx.found, GA_ROOT) != GetAncestor(hNative, GA_ROOT)) {
        Log(L"relay: the control '%s' does not belong to the prompt -> refusing", label.c_str());
        return false;
    }
    if (!IsWindowEnabled(ctx.found)) {
        Dbg(L"relay: control '%s' is disabled", label.c_str());
        return false;
    }
    // BM_CLICK does exactly what a mouse click on the real button does.
    DWORD_PTR res = 0;
    SendMessageTimeoutW(ctx.found, BM_CLICK, 0, 0, SMTO_ABORTIFHUNG, 3000, &res);
    Log(L"relay: BM_CLICK sent to the native control '%s'", label.c_str());
    return true;
}

#if HAVE_UIA
static bool RelayViaUia(UiElement* e) {
    if (!e || !e->element) return false;
    IUIAutomationInvokePattern* invoke = nullptr;
    if (FAILED(e->element->GetCurrentPatternAs(UIA_InvokePatternId, IID_PPV_ARGS(&invoke))) ||
        !invoke) {
        IUnknown* unk = nullptr;
        if (SUCCEEDED(e->element->GetCurrentPattern(UIA_InvokePatternId, &unk)) && unk) {
            unk->QueryInterface(IID_PPV_ARGS(&invoke));
            unk->Release();
        }
    }
    if (!invoke) {
        Dbg(L"relay: no Invoke pattern for '%s'", e->name.c_str());
        return false;
    }
    HRESULT hr = invoke->Invoke();
    invoke->Release();
    Log(L"relay: UIA Invoke('%s') hr=0x%08X", e->name.c_str(), hr);
    return SUCCEEDED(hr);
}
#else
static bool RelayViaUia(UiElement*) { return false; }
#endif

//------------------------------------------------------------------------------
// Windows 7 style UI
//------------------------------------------------------------------------------

// The native bottom link and its relay path are intentionally removed from this recreation.
#define W7UAC_BOTTOM_LINKS 0

#define IDC_W7UAC_YES     1001
#define IDC_W7UAC_NO      1002
#define IDC_W7UAC_DETAILS 1003
#define IDC_W7UAC_PANIC   1004
// The value the dialog carries when the mod decided nothing at all: the panic hotkey, the
// watchdog and the consent flow ending by itself must never approve or deny anything, so they
// leave the decision to the native prompt (which is restored, or already finished).
#define RESULT_NO_DECISION 0

// The system buttons of the Windows 7 Aero caption (96 dpi).


// Layout constants at 96 dpi.

#define BANNER_H            66
#define PAD                 16
#define BTN_W               78
#define BTN_H               24
#define ROW_H               22
#define DETAILS_MAX_ROWS    8
// The banner shield is drawn at 96% of the art's own pixel size - the proportion the Windows 7
// prompt shows next to the question: 48 px of art become 46 px on the dialog.
#define BANNER_SHIELD_PERCENT 96

// The embedded expander art: one icon per state, per size tier (the states are held in the order
// down, up, hover down, hover up, pressed down, pressed up).
#define CHEVRON_STATE_COUNT 6
#define CHEVRON_TIER_COUNT  3

//------------------------------------------------------------------------------
// GDI+, loaded by hand
//
// Two pieces of the dialog are bitmaps that the screen can ask for at a size other than the one the
// art has: the expander button of the "Show details" row (the row draws it a hair smaller than the
// art, 98.7%, and the display scale moves every measurement again) and the program icon (32 pixels
// at 100%, 40 at 125%, 48 at 150%, 80 at 250%). `DrawIconEx` stretches a bitmap with the stretch
// mode of the device context, which is nearest neighbour by default, so at any scale in between the
// edges of the art come out stepped. GDI+ is the documented way to resample a bitmap with a real
// filter, and it is loaded by hand - no import library, no static dependency - so a system where it
// cannot start simply keeps the Win32 path it had before, and the log says so once.
//------------------------------------------------------------------------------

typedef void GpImage;
typedef void GpBitmap;
typedef void GpGraphics;
struct GpRect { INT X, Y, Width, Height; };
struct GpBitmapData { INT Width, Height, Stride, PixelFormat; VOID* Scan0; UINT_PTR Reserved; };
// GdiplusStartupInput, exactly as the header declares it: the version, the debug callback and two
// BOOLs. Only the version is ever written here.
struct GpStartupInput {
    UINT32 GdiplusVersion;
    VOID* DebugEventCallback;
    BOOL SuppressBackgroundThread;
    BOOL SuppressExternalCodecs;
};

typedef UINT (WINAPI *GdiplusStartup_t)(ULONG_PTR*, const GpStartupInput*, VOID*);
typedef VOID (WINAPI *GdiplusShutdown_t)(ULONG_PTR);
typedef UINT (WINAPI *GdipCreateBitmapFromStream_t)(IStream*, GpBitmap**);
typedef UINT (WINAPI *GdipCreateBitmapFromScan0_t)(INT, INT, INT, INT, BYTE*, GpBitmap**);
typedef UINT (WINAPI *GdipBitmapLockBits_t)(GpBitmap*, const GpRect*, UINT, INT, GpBitmapData*);
typedef UINT (WINAPI *GdipBitmapUnlockBits_t)(GpBitmap*, GpBitmapData*);
typedef UINT (WINAPI *GdipCreateFromHDC_t)(HDC, GpGraphics**);
typedef UINT (WINAPI *GdipDeleteGraphics_t)(GpGraphics*);
typedef UINT (WINAPI *GdipDrawImageRectI_t)(GpGraphics*, GpImage*, INT, INT, INT, INT);
typedef UINT (WINAPI *GdipSetInterpolationMode_t)(GpGraphics*, INT);
typedef UINT (WINAPI *GdipSetPixelOffsetMode_t)(GpGraphics*, INT);
typedef UINT (WINAPI *GdipDisposeImage_t)(GpImage*);

static struct GdiplusApi {
    HMODULE lib;
    ULONG_PTR token;
    BOOL started;
    BOOL tried;
    // The thread that started GDI+: it is the only one that shuts it down again, because the
    // documentation of GdiplusStartup and GdiplusShutdown asks for a start and a stop on one
    // thread. Nothing here starts it outside the interface thread that draws the dialog.
    DWORD startThread;
    GdiplusStartup_t Startup;
    GdiplusShutdown_t Shutdown;
    GdipCreateBitmapFromStream_t CreateBitmapFromStream;
    GdipCreateBitmapFromScan0_t CreateBitmapFromScan0;
    GdipBitmapLockBits_t BitmapLockBits;
    GdipBitmapUnlockBits_t BitmapUnlockBits;
    GdipCreateFromHDC_t CreateFromHDC;
    GdipDeleteGraphics_t DeleteGraphics;
    GdipDrawImageRectI_t DrawImageRectI;
    GdipSetInterpolationMode_t SetInterpolationMode;
    GdipSetPixelOffsetMode_t SetPixelOffsetMode;
    GdipDisposeImage_t DisposeImage;
} g_gdiplus = {};

// The flat API's own numbering: the high quality cubic filter, the high quality pixel offset, a
// straight (non premultiplied) 32 bit pixel, and a write lock.
#define W7UAC_GP_INTERP_HQ_BICUBIC 7
#define W7UAC_GP_OFFSET_HQ 2
#define W7UAC_GP_PF_32BPP_ARGB 0x0026200A
#define W7UAC_GP_LOCK_WRITE 2

// Everything or nothing: a gdiplus.dll that carries only part of these is not used at all, so no
// drawing path can end up half way through the API.
static BOOL GdiplusStart() {
    if (g_gdiplus.started) return TRUE;
    if (g_gdiplus.tried) return FALSE;
    g_gdiplus.tried = TRUE;

    g_gdiplus.lib = LoadLibraryExW(L"gdiplus.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_gdiplus.lib) {
        Log(L"GDI+ is not available (gdiplus.dll did not load): the art is drawn with the plain "
            L"Win32 path");
        return FALSE;
    }

    BOOL ok = TRUE;
#define W7UAC_GP(api, name)                                          \
    g_gdiplus.api = (name##_t)GetProcAddress(g_gdiplus.lib, #name);  \
    if (!g_gdiplus.api) ok = FALSE;
    W7UAC_GP(Startup, GdiplusStartup)
    W7UAC_GP(Shutdown, GdiplusShutdown)
    W7UAC_GP(CreateBitmapFromStream, GdipCreateBitmapFromStream)
    W7UAC_GP(CreateBitmapFromScan0, GdipCreateBitmapFromScan0)
    W7UAC_GP(BitmapLockBits, GdipBitmapLockBits)
    W7UAC_GP(BitmapUnlockBits, GdipBitmapUnlockBits)
    W7UAC_GP(CreateFromHDC, GdipCreateFromHDC)
    W7UAC_GP(DeleteGraphics, GdipDeleteGraphics)
    W7UAC_GP(DrawImageRectI, GdipDrawImageRectI)
    W7UAC_GP(SetInterpolationMode, GdipSetInterpolationMode)
    W7UAC_GP(SetPixelOffsetMode, GdipSetPixelOffsetMode)
    W7UAC_GP(DisposeImage, GdipDisposeImage)
#undef W7UAC_GP
    if (!ok) {
        FreeLibrary(g_gdiplus.lib);
        g_gdiplus.lib = nullptr;
        Log(L"GDI+ is not usable (an entry point is missing): the art is drawn with the plain "
            L"Win32 path");
        return FALSE;
    }

    GpStartupInput in = {};
    in.GdiplusVersion = 1;
    if (g_gdiplus.Startup(&g_gdiplus.token, &in, nullptr) != 0) {
        FreeLibrary(g_gdiplus.lib);
        g_gdiplus.lib = nullptr;
        Log(L"GDI+ refused to start: the art is drawn with the plain Win32 path");
        return FALSE;
    }
    g_gdiplus.started = TRUE;
    g_gdiplus.startThread = GetCurrentThreadId();
    Dbg(L"GDI+ ready: the expander and the program icon are resampled with the high quality "
        L"cubic filter");
    return TRUE;
}

// Called once with everything the session built already freed (FreeUiResources), so no GDI+ object
// can outlive the library.
static void GdiplusStop() {
    if (!g_gdiplus.started) return;
    if (GetCurrentThreadId() != g_gdiplus.startThread) {
        // Never a shutdown from a thread GDI+ was not started on: the library stays up until the
        // process goes, which is a leak of nothing - the dialog, and every GDI+ object it made, is
        // already gone by the time this can happen.
        Dbg(L"GDI+ is left to the process: this is not the thread it was started on");
        return;
    }
    g_gdiplus.Shutdown(g_gdiplus.token);
    g_gdiplus.started = FALSE;
    g_gdiplus.token = 0;
    if (g_gdiplus.lib) {
        FreeLibrary(g_gdiplus.lib);
        g_gdiplus.lib = nullptr;
    }
}

static void GdiplusDisposeBitmap(GpBitmap*& bmp) {
    if (bmp && g_gdiplus.DisposeImage) g_gdiplus.DisposeImage((GpImage*)bmp);
    bmp = nullptr;
}

// Draws a bitmap into an exact rectangle with the high quality filter. The graphics object is made
// for this one call and deleted with it: nothing of GDI+ outlives the paint.
static BOOL GdiplusDrawBitmap(HDC dc, GpBitmap* bmp, int x, int y, int w, int h) {
    if (!bmp || !g_gdiplus.started || w <= 0 || h <= 0) return FALSE;
    GpGraphics* g = nullptr;
    if (g_gdiplus.CreateFromHDC(dc, &g) != 0 || !g) return FALSE;
    g_gdiplus.SetInterpolationMode(g, W7UAC_GP_INTERP_HQ_BICUBIC);
    g_gdiplus.SetPixelOffsetMode(g, W7UAC_GP_OFFSET_HQ);
    const UINT st = g_gdiplus.DrawImageRectI(g, (GpImage*)bmp, x, y, w, h);
    g_gdiplus.DeleteGraphics(g);
    return st == 0;
}

// A PNG held in memory, decoded with GDI+ alone: the alpha channel arrives exactly as the art has
// it. GDI+ reads the stream lazily, so the stream is handed back to the caller, which keeps it for
// as long as the bitmap lives.
static GpBitmap* GdiplusBitmapFromPng(const std::vector<BYTE>& png, IStream** keep) {
    if (keep) *keep = nullptr;
    if (!GdiplusStart() || png.empty()) return nullptr;
    IStream* stream = nullptr;
    if (FAILED(CreateStreamOnHGlobal(nullptr, TRUE, &stream)) || !stream) return nullptr;
    GpBitmap* bmp = nullptr;
    ULONG written = 0;
    if (SUCCEEDED(stream->Write(png.data(), (ULONG)png.size(), &written)) &&
        written == (ULONG)png.size()) {
        LARGE_INTEGER zero = {};
        if (SUCCEEDED(stream->Seek(zero, STREAM_SEEK_SET, nullptr)) &&
            g_gdiplus.CreateBitmapFromStream(stream, &bmp) != 0)
            bmp = nullptr;
    }
    if (!bmp) {
        stream->Release();
        return nullptr;
    }
    if (keep) *keep = stream;
    else stream->Release();
    return bmp;
}

// The pixels of an icon with their alpha - the documented way around the trap. `GdipCreateBitmapFromHICON`
// is the call that throws an icon's alpha away and leaves the picture on a black square; the colour
// bitmap of a modern icon is 32 bits per pixel and carries the alpha channel itself. Icons old
// enough to have only an AND mask get an alpha channel built from that mask here, so no icon can
// come out as a black rectangle either.
static GpBitmap* GdiplusBitmapFromHIcon(HICON icon, int* outW, int* outH) {
    if (!icon || !GdiplusStart()) return nullptr;
    ICONINFO ii = {};
    if (!GetIconInfo(icon, &ii)) return nullptr;

    BITMAP bm = {};
    GpBitmap* result = nullptr;
    int w = 0, h = 0;
    if (ii.hbmColor && GetObject(ii.hbmColor, sizeof(bm), &bm) && bm.bmWidth > 0 &&
        bm.bmHeight > 0 && bm.bmWidth <= 1024 && bm.bmHeight <= 1024) {
        w = bm.bmWidth;
        h = bm.bmHeight;
        std::vector<BYTE> px((size_t)w * (size_t)h * 4, 0);
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;              // top-down: the order GDI+ reads scanlines in
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        HDC dc = GetDC(nullptr);
        const int got = dc ? GetDIBits(dc, ii.hbmColor, 0, (UINT)h, px.data(), &bi, DIB_RGB_COLORS)
                           : 0;
        if (dc) ReleaseDC(nullptr, dc);

        if (got == h) {
            BOOL hasAlpha = FALSE;
            for (size_t i = 3; i < px.size(); i += 4) {
                if (px[i]) {
                    hasAlpha = TRUE;
                    break;
                }
            }
            // Where the alpha channel cannot be trusted, the icon is left to DrawIconEx: an
            // invented opacity is exactly what paints a black square behind an icon.
            BOOL pixelsUsable = TRUE;
            if (!hasAlpha && ii.hbmMask) {
                // An icon from before 32 bit alpha: what is transparent is the AND mask, one bit
                // per pixel, every row padded to four bytes.
                const int stride = ((w + 31) / 32) * 4;
                std::vector<BYTE> mask((size_t)stride * (size_t)h, 0);
                BITMAPINFO mi = {};
                mi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                mi.bmiHeader.biWidth = w;
                mi.bmiHeader.biHeight = -h;
                mi.bmiHeader.biPlanes = 1;
                mi.bmiHeader.biBitCount = 1;
                mi.bmiHeader.biCompression = BI_RGB;
                HDC dc2 = GetDC(nullptr);
                const int gotMask =
                    dc2 ? GetDIBits(dc2, ii.hbmMask, 0, (UINT)h, mask.data(), &mi, DIB_RGB_COLORS)
                        : 0;
                if (dc2) ReleaseDC(nullptr, dc2);
                if (gotMask == h) {
                    for (int y = 0; y < h; y++) {
                        for (int x = 0; x < w; x++) {
                            px[((size_t)y * (size_t)w + (size_t)x) * 4 + 3] =
                                (mask[(size_t)y * (size_t)stride + (size_t)(x / 8)] &
                                 (0x80 >> (x & 7)))
                                    ? 0
                                    : 255;
                        }
                    }
                } else {
                    pixelsUsable = FALSE;
                }
            } else if (!hasAlpha) {
                pixelsUsable = FALSE;   // no alpha channel at all: DrawIconEx knows what to do
            }

            // The pixels go in through LockBits, which copies them: the vector is free to die here,
            // which is what the GDI+ constructor that takes a buffer does not promise.
            GpBitmap* bmp = nullptr;
            if (pixelsUsable &&
                g_gdiplus.CreateBitmapFromScan0(w, h, 0, W7UAC_GP_PF_32BPP_ARGB, nullptr, &bmp) ==
                    0 &&
                bmp) {
                GpRect all = {0, 0, w, h};
                GpBitmapData data = {};
                if (g_gdiplus.BitmapLockBits(bmp, &all, W7UAC_GP_LOCK_WRITE, W7UAC_GP_PF_32BPP_ARGB,
                                             &data) == 0 &&
                    data.Scan0) {
                    for (int y = 0; y < h; y++)
                        memcpy((BYTE*)data.Scan0 + (size_t)y * (size_t)data.Stride,
                               px.data() + (size_t)y * (size_t)w * 4, (size_t)w * 4);
                    g_gdiplus.BitmapUnlockBits(bmp, &data);
                    result = bmp;
                } else {
                    g_gdiplus.DisposeImage((GpImage*)bmp);
                }
            } else if (!pixelsUsable) {
                Dbg(L"the icon has no alpha this code can trust: it is drawn the Win32 way");
            }
        }
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    if (result) {
        if (outW) *outW = w;
        if (outH) *outH = h;
    }
    return result;
}
// Declared above the interface state that keeps these bitmaps, and above every function
// that draws with them: no name in this file is used before it exists.

struct UiState {
    HWND hwnd = nullptr;
    HWND hYes = nullptr;
    HWND hNo = nullptr;
    HWND hDetails = nullptr;
    HWINEVENTHOOK hStateHook = nullptr;   // the event hook that follows the native prompt
    PromptInfo* info = nullptr;
    HFONT fontNormal = nullptr;
    HFONT fontBold = nullptr;
    HFONT fontBanner = nullptr;
    HICON shieldSmall = nullptr;
    HICON shieldBig = nullptr;
    HICON appIcon = nullptr;
    // The theme manager's handle for the task dialog class, opened once for the dialog and closed
    // with it: it is the fallback that draws the "Show details" expander when the embedded Windows
    // 7 art cannot be decoded (see ExpandoTheme and ChevronArtIcon). The flag records that the
    // attempt was made, so a system without that theme part says so once in the log instead of
    // once per repaint.
    HTHEME hExpando = nullptr;
    bool expandoTried = false;
    // The embedded expander art, one icon per state and size tier, decoded when the tier is first
    // asked for (see EnsureChevronTier). chevTried records, per tier, that the decode was
    // attempted, so a failure is reported once instead of once per repaint.
    HICON chevArt[CHEVRON_TIER_COUNT][CHEVRON_STATE_COUNT] = {};
    bool chevTried[CHEVRON_TIER_COUNT] = {};
    // The same art through GDI+ (see the GDI+ section): what the row is drawn from when the size it
    // asks for is not the size of the art. Each decoded bitmap keeps its stream, because GDI+ reads
    // an image out of the stream it was given, lazily.
    GpBitmap* chevBmp[CHEVRON_TIER_COUNT][CHEVRON_STATE_COUNT] = {};
    IStream* chevStream[CHEVRON_TIER_COUNT][CHEVRON_STATE_COUNT] = {};
    bool chevBmpTried[CHEVRON_TIER_COUNT] = {};
    // The program icon as a bitmap of its own, for the same reason: the dialog lays it out for this
    // display scale (32 px at 100%, 80 px at 250%), and that is rarely the size the shell published.
    GpBitmap* appIconBmp = nullptr;
    // True while the cursor is over the "Show details" row (the row is a child control, so its
    // cursor message is relayed to the dialog: see the WM_SETCURSOR handler).
    bool detailsHot = false;
    // The pixel size of the art behind those two icons. The program icon is drawn 1:1 - an icon
    // asked for at the art size is never resampled, which is what keeps it sharp - and the banner
    // shield at BANNER_SHIELD_PERCENT of it.
    int shieldSize = 0;
    int appIconSize = 0;
    int dpi = 96;
    int baseWidth = 0;
    int baseHeight = 0;
    bool detailsExpanded = false;
    // Set once the prompt's own details have been read; detailsAttempts is the hard cap on how
    // many times the native window is touched for them (once while the dialog is built, and at
    // most once more if the user expands the pane and there was nothing in it).
    bool detailsFromNative = false;
    int detailsAttempts = 0;
    // Shown in the button row when a bottom link could not be forwarded to the native prompt: the
    // click is then a no-op, and a no-op that says nothing looks like a broken dialog.
    std::wstring notice;
    DWORD noticeUntil = 0;
    DWORD shownAt = 0;
    int hotkeyId = 0;
    int result = 0;
    bool done = false;
    int hoverId = 0;   // 1 help link, 2 change link
    std::vector<std::wstring> details;
};

// Holds two containers of its own (the notice text and the details pane), so it is part of the
// same rule: emptied in the unload path, never destroyed under the loader lock.
[[clang::no_destroy]] static UiState g_ui;

static int ScaleFor(int v, int dpi) { return MulDiv(v, dpi, 96); }

// The banner shield is the one icon that is not drawn 1:1: the art is taken at its own pixel size
// and drawn at BANNER_SHIELD_PERCENT of it, rounded to the nearest pixel, so the same proportion
// holds for every art (16/48/128 px) and at every display scale.
static int BannerShieldPixels(int artPixels) {
    return (artPixels * BANNER_SHIELD_PERCENT + 50) / 100;
}

static UINT GetSystemDpiValue() {
    typedef UINT (WINAPI *GetDpiForSystem_t)(void);
    if (HMODULE u32 = GetModuleHandleW(L"user32.dll")) {
        auto fn = (GetDpiForSystem_t)GetProcAddress(u32, "GetDpiForSystem");
        if (fn) return fn();
    }
    HDC dc = GetDC(nullptr);
    UINT dpi = dc ? (UINT)GetDeviceCaps(dc, LOGPIXELSY) : 96;
    if (dc) ReleaseDC(nullptr, dc);
    return dpi ? dpi : 96;
}

struct Layout {
    int W, H;
    int capH, bannerH;
    int iconX, iconY, iconSize;
    int textX, textY, textW;
    int rowY;
    int detailsY;
    int sepY;
    int btnY;
    int linkY;
};

// The dialog uses the native Win32 title bar, which lives in the non-client area: the client
// layout starts at the top of the window and nothing has to be reserved for a caption the mod
// does not draw any more.
static int CaptionHeight(int) {
    return 0;
}

// The "Show details" row belongs to the dialog, not to its content: on Windows 7 it is always
// there, and the information appears when it is expanded. The pane itself never stays empty
// (BuildDetailsPane guarantees it), so this row is never a hole either.
static bool HasDetailsRow() {
    return true;
}

// The "Show details" pane is measured from its content - one line per row - and the dialog is
// given exactly that much room (see ResizeUiToContent). A fixed guess at the height was what let
// the pane come out as an empty rectangle; a measured one cannot.
static int DetailsRowHeight(int dpi) { return ScaleFor(19, dpi); }

static int DetailsPaneHeight(int dpi) {
    int rows = (int)g_ui.details.size();
    if (rows < 1) rows = 1;
    if (rows > DETAILS_MAX_ROWS) rows = DETAILS_MAX_ROWS;
    return ScaleFor(10, dpi) + rows * DetailsRowHeight(dpi) + ScaleFor(8, dpi);
}

// Windows 7 prints a third line - the origin of the file - when the prompt says which file is
// being elevated. It is the information the modern prompt keeps behind "show details", and it is
// the same file the program icon is taken from.
static int RowCount() {
    return (g_ui.info && !g_ui.info->originText.empty()) ? 3 : 2;
}

static int BaseClientHeight(int dpi) {
    // caption + banner + rows + ("show details" row) + separator + buttons + margins
    int content = BANNER_H + 188;
#if W7UAC_BOTTOM_LINKS
    ;                       // the row of the link is drawn, and it takes its 32 px
#else
    content -= 32;          // no link row: the buttons take the space it used to take
#endif
    if (RowCount() == 3) content += ROW_H;        // the extra "File origin" line
    return CaptionHeight(dpi) + ScaleFor(content, dpi);
}

static Layout ComputeLayout(int clientW, int clientH, int dpi, bool expanded) {
    Layout L = {};
    L.W = clientW;
    L.H = clientH;
    L.capH = CaptionHeight(dpi);
    L.bannerH = ScaleFor(BANNER_H, dpi);
    // The banner shield: the Windows 7 art drawn at 96% of its own pixel size, the proportion
    // the original prompt shows next to the question's two lines.
    L.iconSize = BannerShieldPixels(g_ui.shieldSize > 0 ? g_ui.shieldSize : ScaleFor(44, dpi));
    L.iconX = ScaleFor(PAD, dpi);
    L.iconY = L.capH + (L.bannerH - L.iconSize) / 2;
    L.textX = L.iconX + L.iconSize + ScaleFor(12, dpi);
    L.textY = L.capH + ScaleFor(9, dpi);
    L.textW = L.W - L.textX - ScaleFor(PAD, dpi);
    L.rowY = L.capH + L.bannerH + ScaleFor(16, dpi);
    L.detailsY = L.rowY + ScaleFor(RowCount() == 3 ? 58 + ROW_H : 58, dpi);

    int rowH = ScaleFor(24, dpi);
#if W7UAC_BOTTOM_LINKS
    L.linkY = L.H - ScaleFor(12, dpi) - rowH;
    L.btnY = L.linkY - ScaleFor(12, dpi) - rowH;
#else
    // No link row: the buttons take its place, and the link coordinate is put out of reach so that
    // nothing can be drawn or clicked where the link used to be.
    L.linkY = L.H + 4096;
    L.btnY = L.H - ScaleFor(13, dpi) - rowH;
#endif
    L.sepY = L.btnY - ScaleFor(10, dpi);
    return L;
}

// The three banner states of the real prompt, straight from the DirectUI markup of authui.dll
// (UIFILE 14000): UAC-Signed, UAC-Unsigned and UAC-Killed.
// One flat colour per state: no gradient any more, the banner is a solid band. The light blue
// ("azzurro") is the state of a known publisher, which is the normal case for a signed program;
// amber is the unsigned publisher, red the blocked one.
static COLORREF BannerColour(int state) {
    switch (state) {
        case 2:  return RGB(242, 177, 0);    // unsigned publisher
        case 3:  return RGB(172, 1, 0);      // blocked
        default: return RGB(15, 111, 168);   // signed publisher: fixed azzurro
    }
}

// The vertical ramp that the Aero buttons are painted with.
static void FillGradientV(HDC dc, const RECT& r, COLORREF top, COLORREF bottom) {
    int h = r.bottom - r.top;
    if (h <= 0) return;
    for (int y = 0; y < h; y++) {
        int t = MulDiv(y, 255, h);
        COLORREF c = RGB(GetRValue(top) + (GetRValue(bottom) - GetRValue(top)) * t / 255,
                         GetGValue(top) + (GetGValue(bottom) - GetGValue(top)) * t / 255,
                         GetBValue(top) + (GetBValue(bottom) - GetBValue(top)) * t / 255);
        HBRUSH br = CreateSolidBrush(c);
        RECT line = {r.left, r.top + y, r.right, r.top + y + 1};
        FillRect(dc, &line, br);
        DeleteObject(br);
    }
}


// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------
// The Windows 7 shield, embedded as PNG in base64. It is not an approximation and not a crop
// of a screenshot: it is icon resource 78 of the Windows 7 imageres.dll, at three of its own
// native sizes - 16, 48 and 128 pixels - taken from the 32 bit-per-pixel entries of that icon
// (the 4 and 8 bpp entries of the same resource are the low-colour variants and look wrong on
// a modern desktop). This is the very image the classic prompt uses; the modern UIFILE asks
// for the same family of icons by their imageres index.
//
// No image library is needed: since Windows Vista a PNG image is a legal icon image resource,
// so the decoded bytes go straight to CreateIconFromResourceEx ("the sneaky way to load a PNG
// as an icon"; the icon directory of an .ico file is *not* accepted, which is what the first
// attempt of this mod got wrong).
// ---------------------------------------------------------------------------------------------

static const WCHAR kShield16B64[] = L"iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAC9klEQVR4nG2TTWgcdRjGf//5yM7sxP2o2aRGa5rDVgUjEgpV0UOF"
    L"zUG9RBGhohfxIAiFTXPTi0IRY2YuoifFU1i9LEgMIhK9iEKMhZWUYmqaQJqEWdPsrNmZnZ2Pv4fYmMa+l/fyPD/e93l5BXeparU6"
    L"Ojg4+LkQ4hFASdO00Ww2X7Fte/e4Vhwzni6Xy9NCiJfGx8dP5HI5VUop2+12vLS0dEtV1a9XV1c/sG17/Q7A1NTUC6VS6V1FUUYm"
    L"JiaKQgjt0o++0sLgo+Kb5PIFvPw7vf7+/nRxcdFL03Sl2Ww6s7Oz8xqAaZqXK5XKw5Zlac9/qwo96HDh/Bm+/HmL+/Mup8abXLty"
    L"sa+zM0ylcllN0/SZer0+AswrAIqiWIVCQat8lxPnRi0uPPsgV3clcRDRly0h9JcZO6dzcnidcOktPZvNKqqqZgCU26tIKens74Ou"
    L"88vNHldXW+itAEUN0NQNpHyckbJPTHJHiNq/PRXiIM9fNzyinsDa9oiDGEXvcmv9CqqmoWYstEwKgJTyP4CU0hdCyDj4W4SxDt0O"
    L"fhSj6DoyhtBTSBH0ejEiUgFSKeXeISBJkmXf9898cuL1zPkn+skWMwd7CcnoF/OEsaBPE+QzCqf2b/Dx2TBJkuT3wwziOP7Ndd3w"
    L"sYESGjpG9h6MooEQCgiFxBzCGBhB3jdIsziA67qtOI6XjwK+aTQaW/rZT6O224PIgOhgCkWAmTUxCymFtXXef7ovWllZ2Y6i6KtD"
    L"gG3ba67rbgoh0r/CIfwgQaZZEKBpguEHUiy/w70Zj0dLauK67jXHcTaPnpEgCF6bm5vbKox92LtxXefmnz7hXkoGyDX+YGBjGefF"
    L"fFir1Ta73W71tu8Q4DjOju/7b9dqtZb10Hu9be8pfvo+Ymj7OmOnd3FePRnW6/W27/sXHcfZueszAUxPTz9nmuZnk5OTRcMwNEB6"
    L"npcsLCzsBUHwxszMzMJR/f8AANVqtWwYhq3r+pOAGkXRD91u95Jt22vHtf8AU4dOukbuXlEAAAAASUVORK5CYII=";
static const WCHAR kShield48B64[] = L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAPXklEQVR4nMWaa7BdVX3Af+ux97nnPnJJbl6QoIASQIJAUFEDrTjU"
    L"5kNlEGIRn7XWVpnRD9WKlsc4Tp2gFi1MdQJtudMWqXZqCTBpBigk1tChkQDekJcxEO4z595zX+fcc/Zjvfphn5ukJDcv4vQ/s+fe"
    L"fc7svf6/9X+utY7gtyDr168XIQRCCNx2223htzHGrIgz9aIHH3zwPO/9V0IIq4ErQyj09t6/bK3d2mg0fnrHHXc8DzjgjEGdEYD1"
    L"69ffIoT4ydvf/nYuv/xyyuUypVKJer3O6OgoQ0ND7Ny5k2azuf7222//CpAC/kyM/aYBHnjggWeB69asWUNPTw9R1EYcq0Pfe58w"
    L"NTnDwOAwO3bsoFqtUqlUbrrnnns2AoY3aY3TBli/fv21UsoNcRwvWLt2LXFcYuv2Azyzo8lI04FN+fLVa2mYlLJ8L+ev/BH1mZwX"
    L"XnyFAwcO0Gg0Hrnrrrs+z5u0xmkBPPDAAw8LIT5x8cUXc80111CrNXhi6+vsnYgxImN0WhPbKb73J/dTH51gZCTBzRjmnfPnnL38"
    L"d9n3mwP86ld9VKtVPzY29rF77733MU7TGicN0Nvbq621PwohfL5UKnHzzWtpby+z77WD/NfuJla3IWVOI/eMTedMTzX57uf+BTfe"
    L"IMsczk4yNj6Cza/ivJW3E4Jm85ZfsG/fPowxI5OTk1/6wQ9+8MSpghwXoLe3d4lz7kPAx4UQa+I45sMf/jDd3d2kacbPXxqmKbrQ"
    L"0kEMOI8xMDje5PWRae753E/RM21kuSUYQ31qlOn6IFk9EC36IsvP+yAHK6Nsf7GPPXv24L0fTdP0n9M0/dl99923jcK1jgtzFMBD"
    L"Dz20RghxN/A+gHK5zEUXXcTKlSvRWpNlOc/vqtA/GbF8aSeRMkgPQXhCgPGZnMGphP7BOus++zPkTDGEzRXBWmxapz49QmVskDRb"
    L"Qs8FX2Lpssup1er0Dwyzc9du9u/fj3MO7/14mqbfuv/++9fPZRn9xg+klJsuvPBCVq1aRVtbGwDeB6brTV749Qi/GvCcfXYnixd5"
    L"ynGOFgrvwAlBPbFkzhMhUEoQKJ7He7QGRBmFRskYRRdpOsb4wDfZ27+Mnrd+mpUrr+YdF53HdL3B8MgoO17Z3bN79+77gH8FJoHs"
    L"hAAhBN7//vfTaOa8uLfK0GTCntenGDXt9HS10dMN7SojVprgISBABoxzmOBwHhACEAii1lsduEBwDiElOirTvWAp7Wk3pXiSrDHN"
    L"+IF1jOyEefNXoBdcywXnr2bp4vns2rWLhQsXLqtWq42TApiV53dWeXZgHnFbjuw8ix6R0RXndJQitCzcIrWWWAVMFjA+YHKHcYHc"
    L"BaxzgCyM7oHgEAiCtwghUEoh2joQMkbLbhDzSUWd6ugI6dgPySvPsPjC21BKsXjx4p5qtaopXP7/uNGcABMzGe2lhM6SxVpHhCQS"
    L"Ct9qEWaajlKsyJ3HGk9uHc3ckaSeJPMkmUMG+YbxCsvgPcEHhIA4iqEssFaSmzbay/Mo5VAbr7LkQo9Wio6Ojs7DD58kgPPgfVFf"
    L"tFYY68Da1iOWUqQw1uAQZM5h8kCSO2Zyx0xmaWYOggAfiisUlwiicDuK74ILyCCJlaKsJLakcEZStznWa3SkCSGIwpxHy1EAs0o7"
    L"B9Z5MuMAsNYjFBjrSJHkNuBDIDcBEzzGetLc0cgKgFqaH1YeQBTjh9k5DK35lCCkJdYxVjk6GSYvSWQWQBYtSQvgmDIngBAC4zy5"
    L"Ke4zY/Ey4LQiNYIQILOu8HdbxEBuHUnuaeSWmTRvBbMsFHUWvCGYDIwluGJivAngPSK1xCEl2Bny1BO8A6Gw1jLb2Z4UwKwEASaH"
    L"TBYDmdzhgVx4PAJjPVkL0IWA8Z4sLwI4946mcZA3CI0mwTbBJoQswRtDMI7gAziJDwHhwWUWnxtEOkGUWYSYj3OGEAJCiJO3wGFa"
    L"iffgbWEB6wIOj/OQ20BmHM3cY53DBYENHm8h855m5qlMWTjYh51IEd4UigpBcJrgNaCAgAiFhaSOCB6UkkSlCJcJhIyYXRidsgWk"
    L"AhMC2hUP5y4gBRjjSY2jnjoaxmO8Ay+wPlBPHWOJY7Jh8BMJJGMQOkBpBOJQ/hBCEHyrVihB8CAUCKUpxZpAQOa6mEDvjwQ4yhJz"
    L"WkBKRZYHlCzuXfD4EHDek5lAmvsiRnxgOnVUZiz1ZgrJZCt4SyBjhJKHRg2zyrfCYrbgIQW4gIwAJwjKo+I2nPc0mwnOOXPEK04M"
    L"EEKguwy5gVi3AFrBbQJYAAlJFhis5VRrSaF4EEWyixUYeWhEKYok5IMoTOtbSgcBQSKEABFwLhS1Twp0PA9rDWmaUq1Wp4+l/FwA"
    L"1RDCwq7YIIQma7lQcEX+KwCh2nDsGU8x0wfBBVAtpY7MPFK0lBMgQEiJQOK9QAQFQbYuUcRIFPAmkHlP1L6ALC+CeHh4eILDyfeE"
    L"AMPNZnNhTwfYmsedVXwuAYnACcFIw7JnPMfUxkDKQvlwRL4XArwGrZBKFSVACoTUgEQ5BUgCErwEBNI7hHM4meGlQ7e/hdyIWZc+"
    L"1JCcEMBa+/Lw8PDKeZ2d8veXbeTH+1cREJQih/eOsUbKa9UEM2PAlIrZP1SVKP5KIIsgbESatuJeKRAxiAikBq8QQRG8JPhAMAGy"
    L"Ot6M4zPwbX/D5GSCMWZkrtk/JoBz7uXBwcGPXnHFFeVFnb/mOzc8xDkdK+hW3WglQZZbrcEk2CmwTQiu5TqzZhTg/oOu7+6lLc4o"
    L"lS1aR0RaEkeKWGukFGghCaJMaqDRmEYkDdz0OKY+zTPfXkZzeDdJkuymCLtjrpuPAqjX60+NjIzc9a53v6dcis8lrzeAFNFxFiHI"
    L"1lsE0sXgYrwxvLFNCQh0XoKyYV5zB5HTxJGiLVKU4ohYKyIpEapEwynq0xrXzHHNCcyMY+XCBCkU4+Pj1Ov1XUBOsZ90lBzVIN19"
    L"9917JiYmJr2zLJx/PllFE6wmBI9AooJF4CAoQCOQiCAOtYoCEC2DKyFQUqKURGmFjjRaa3Sk0HEJJUug2ilFEZEGRSDPLVetmIcx"
    L"OZVKhdHR0RdPCQBwxphfjo+P07PoXKanOzC2AcHhvQEswh2qSIeatEPiBaBARChRKK+VIlIKrRVaF/dSa3It8UKDjJA6wgMdus4H"
    L"Vi0lSepUKhVeeumllykWMvZkAUiSZMuWLVvo7HkLqu0yppJxbO7AU/QwwoEIhBCBaK0zvACvinuhAIVUEqkVKtZEsUZqidISpRQI"
    L"hVdtOKeK8A8JqYs4v6vOOy5cxvTUFAMDA7P+nzNHDBwT4M4773yoVqtRb6Sce9HNDA/OkNlpfHAI4RHBgg+F+wg52wccTqHMGkgQ"
    L"aY1WEq0kkWqlVa2xQhKMxWmwJORNj8hybvrgBTSbDTZt2sTQ0NA/Umx8zbnVckwAwCRJ8tDOnbtYsHQFdXMl440Kzua0CnIBIqCo"
    L"/1FhhSM7FR9QWqGUQiuFUAKpFEpKkBIvFQ6BNAbpEzLjWawrrLnmCqYmJxgaGqo/99xzTwONlgWOKXMBhK1bt/7F9u3b8ULxlks+"
    L"TqVSp2GmCZgibVJkmxA0Quii4r6h1xKicJnC7yVKCYRWWAQ2gMfjTYOklhGSnJuuPgspHBs2PEq1Wv1Ja/YbzOH/xwNg06ZNU8aY"
    L"Z7Zt+yXLlq9gbOJSas1BnEkPF11RVFehIkAfZWStJFpKpCr8Xqgic3kv8dYT8hzjEtIclqt9fPLGaxkbPUh/fz9PP/30PwAzLYg5"
    L"++k5AQD/8MMP39zX10eSWy55z5/R/5qlYaaw3mNdYYGiq9QgosIKh+g8SguULFKpUAohJVnwWKFxNpDlOTMTFp+m3HLdBSjh2bDh"
    L"USqVyn0t5esU/j+nHA+AvXv3ziRJ8sOn//MZOuYvo8n1jE2NkeUJxjusD7igQZQQuoyQMUIc3lpXUhX5X2u0VrigsF6SW4sLljRN"
    L"yXPJpd0T3HrDNez/zV4GBwfN5s2bfwzUgCYn2Lk+LgDg1q1b99WhoSFeHxjmyms/y759C6j5aZyz+CCRkSaoCEQZVAdClBGiSK9S"
    L"SOK4UN47sEVzDc6TpznNmYguO8lfffF3SGZq9Pb28uqrr95J4fd1jhO8JwsAkPf3939g48aNpE5y6ao/Zv9rnlrIyF1Obimas6gE"
    L"ogOidqTuhLa3obWiraSRFOtmPDibY1LLRLNMlii+8KH5nHfOfB599N9oNBq/2LZt29PAVGv2T7hLfTIAvre397+TJHnk8Sc20rbo"
    L"nXSX3s3IWJMpB03jya3CyRgflSEqQ9wB6hykFmSiCx8csQIVDN5BtTmPWq2HtSun+MQN72NH38ts377dPf74498AplvXcX3/VAAA"
    L"7Pe///3PHzhw4PXntr3Eue/6JJWB86nWMmp5RMMETJB4GeFlG0GXaar5SC0J3lHWilhYQhDU0g7q6Tlc2/NrvnXb77F//z4efPBB"
    L"+vr6/pRiA3eSE2SeI2XORf0bJADpU0899V4hxEhPTw+rb/gqW/79W+Rva2Lnd+JjR7uSrS5TIVVEhKTc2UUZQ9L01EwHJpvP6q5t"
    L"/P3XrqE6Nsq9f/09RkdH/3b37t3bgQmK7HPMxu1YcrIWAPCvvPLK2P79+1c/9thjvNY/zAdu+iZTgxGDA1OM11ImkoymNSSZI/El"
    L"tPTE3T3koo2DjRhTD1zdsZ2/+9pqkjThjjv+klqttmHz5s29LeWnOU7ROpaczhlZdMstt6xdsmTJI3/0mc/wtgveyqvbf0K18iyL"
    L"V5TpkjFxFOHMJj71/DbafY200k80c5DrVpT58qfXUB2f4Btfv52ZmZlnnnjiiduBcWAMSDjFA7/TPaUs3XjjjWvOPvvsDR/5yEdY"
    L"deVlkFXp2/odGjMDeJfSTA3f2/l1lnbB8oUdfOq6JbzvXVewc9cu7lm3jqmpqUc3bdr0bYqZr1KkzlM+rTxdAAGUVq9evfKSSy55"
    L"8qqrrlrwhx/9KKUIpsZfZ/++X+KcY/fUxfR0d3HlpecR+YSf//xZnnzySQ4ePHjfli1b/qml/PjpKv9mAGafjYDuW2+99f5FixZ9"
    L"7LLLLuOKy9/J/AU9zG5n1mo1BgYGeOSRR7DW9r/wwgtfOXDgwF4OZ5xTdpszBTArGujs6OhYfP3113+hXC7/gdZ6+eyWuLW20Wg0"
    L"/qe/v//Rvr6+FygK1BRFq5Dx/3VS/waRFAetZaATaKcAm32/43Br3Gz9f0rZZi45Y79WaYmkcKvZ7ecjjzMsRXU9o79W+V9g9n+d"
    L"ye+n8QAAAABJRU5ErkJggg==";
static const WCHAR kShield128B64[] = L"iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAAA++UlEQVR4nO29Z7gl1Xnn+1urqnY4sU8HOtKBHCWDQEgg6QoFW5It"
    L"S9aMNPYE+3GUn/vBd+Y+z9iyfa/neq7DHVtjezSWLQmU0EhgAQKBQpMaECI0UdCBhqZzn5x2TlVrrfthrVVV+9DWSPQBGoYF1Xuf"
    L"vatq713v/33f/xvWKnhjvDHeGG+MN8Yb443xxnhjvDHeGG+M1/X47N//4+ZX+zucSkO82l/gFR7i+uu/+cv1eu0jmuCdQrDBGNBa"
    L"o5JkMu61t2utv3fk8OG7//vff6YGGHec+XEnfS2P/yUA8PVvfHMkkOLjjUbj34RR9LYztm0tr127FgCtDUoldLs95ubmeW7/gXaj"
    L"1Xw4MMnX9j///O3XfvGaCqDdqV53QHhdA+DP/uwvwzPPPOs3W+3W/3namjXnbNy4gcHBQXq9mCRJMDlxCiEQQqC0olKpMDM9xeTk"
    L"1LPtdvOzDzzwg69u3769jQXC6woEr1sA3Hrr7T+zsLDw9+vWnXbVli1bUUrTarVRSlGIQqJCAYwBAQJBnCR02j16SZdiVCIqhDSb"
    L"TSYnJxgfP3Z/vVb/4//8//7po4DCguB1AYTXIwDEN77xT/9WSvHfzjvvvDEpA+r1BoUoolAIqddr1KoVOt2EqFAEo9FJm6hQYvXq"
    L"NQwNj9DrxSwsVpFBSBhIqrUqRw4fXJybm/2TP/mT//sLWBC8LqzB6woA/99f/lW0cdPG/7xy5cpPnXHGGVSrNQSCwcEyk5PjzM0v"
    L"MLDidGR5DbVegUYrYbpR5VMfNxw/9CCH9ynGFwZZvfZ0zjjzTFrtLvPzi0RRgTjuMj5xPDl+7OjnnnryyT+96eabFnkdgOB1A4Bv"
    L"fvNbm9vt5j9s27r151euWsXCQpUVK0ZoN+u8cOAAqzacjx7YwLGZFgu1Np12j1hp9kzNcev/NQDxPVAo0Y0Fzz8Nj/+owBlnv5kt"
    L"W8/g2LFx4kSBMCzML3Dw4AvbZ2dn/ujTn/7r3bzGXcLrAQDi1lu/83ONRuWzF5x/4RlCBjQaTU5bs4rDhw5QbSWsPeOtHJnpMV9p"
    L"0O706ClFomJibXh2cpFv/9EgdO4AJIgACkViAh7YnjBV3ci7r34v45Mz1Ko1okJItVbj0MEDx2dmpv/wz//8z74JJLxGQfCaBsBn"
    L"P/v5aOXKFf+pXC596rxzzwtq9SZGK8ZWjPLMrqcZXrWZYPQsDk5UaHU6dHsJPZ2QKE2sNLEyPDu5wO1/OATt7aAlYEAIkCGUyux5"
    L"rsoP71vNB37+ozSaXSanpikWi/R6XY4ePdIdHz/2+QMHDvzlV77y5Vlegy5Bvtpf4CUO8a1bbnvL8MjgPevXr//jc889L5hfqFCI"
    L"QgYGSux8dCfrtl6MGdrG0akKcdxDmwQjDUhQAhJhSFBoHwsKA8LJz2hIYmjVufCMQT780XluuemLJHGbs8/cRpIkBGHItjPOLF5w"
    L"wYW/d8EFF377D/7gU+8BQuw1fc0o1mvmi/rx2c9+LlqzZs3/geBPzjv3nGEM1OstVq1awcz0FEePj3PBpVczWYWFSot2HNPTCbEy"
    L"dBNFJ1HEStHpKpSKeX6yznf/eAha3wMj7RXxxlwDQkKhSEcH3Hpjg6E1b+PSSy/j6PEJms0WxWKRZqvBwYMHajPTk589ePDgZ667"
    L"7rrXjDV4LQFA3HDDP71ba/PnG9ave/vGTZuoVGoIYGR4kD17dkM4wBkXvo2j0y0anS7dOCFWil6i6CTaCj9O6Mb2MdHKAuCPRqD1"
    L"XWv6MQ4AErR7joQogsGQ791RY3ryTN7z3p+j0WwzNT1LFBUwaOZmpzl06OATc7Ozf/pf/uq/bOc1QBBfEwD45o3f2qyS3u8PDw9/"
    L"8uyzzwkFUKvXGRoYpNmss2v3Ls6+6HIGVmxiYr5Ju9MjUYqu0vQSRVcp2j1FHCt6StFzIFDa8PxEje/+0bC1AEJboQMgwAgnQmH/"
    L"lgGUixyZaHHbrZK3/2+/yJo16zk2PkGv16M0UKZRr3P40KHu1NTxr8/Pz3/mM5/5zB4ya3DKAeGUBsDXvnb9aKlU+KTB/Ptzzzl7"
    L"/cqxlTSaTTAQRSH79u2l0ezxpsvfRb0bUG12LdFTCbHS9JSh04vpKU03TuglmjjW9v3Y0EsSnp+s8Z0/HLEAQINR9sO1cOIS7rmx"
    L"VkFKKBbpCLjt5nlU4XLe8Y53Uak1mJmZo1goIgPJ/PwsBw8emJmfn/2HmZmZaz//+c9PcQq6hVMSAH//9/9QXLt27S8boz+1bevW"
    L"8zZt2ki326Pb7VIqlZifm+XpXbs4+/yf4bSNZzFf69LuxsRKOZNvWX43UXRiRZwoerGimyjiRNvNvb5vssrtnxqB1nZL/lCgdT8H"
    L"8LqbGocAghAGSux9dpF77h3gbe/8EKetXc/4xAydTpvSQBmdKKamJjh8+NBz8/Nz/3jkyOFvXH/99QucQkA4pQDwzRtvXh2F4Sfi"
    L"Xvd3T998+sVbt2wBoNvtEoYhnXaHZ57+EQkhb77sSnqUaLZcXK81sVIoZehqK+BerOi6x15i3UFPOSuQaOIk4dnxPAAMkIByrkA4"
    L"zQer/V5kGkBAIKFQphUq7v5Oh0rnbK686p1oI5mcmsEYw8BAiU6ny8TEcY4cObK3Vl384vz8wo2f/8LnJzgFqoynAgDELbfe/maj"
    L"1a9qoz+xbeuWjZs2biQIA7qdLkEQoJKEPXv3MjU9y0U/81ZGV2+k3uyRqIREaxJt7KPS9Jzf78WZpncSba2A0/6eswg9pXn2+CK3"
    L"/cEotO50FiABrZwFyKn9UgCk1kBCGEG5yMGJFndt12w640ouvOhNNJptZmfniKKQYrFIq91memqCo0eOHq5UFm5stZrf/Nu/+7un"
    L"eBWB8GoBQFx//Y2rBwbLv5Qk8b8YGhx4z7ZtW8M1a9YA0Ov2CKREKcX+559n/4GDbD3zAracdT7NriZRCq0NBoM2GmUMsTL0EmUT"
    L"PE7wvUTTi/tB0csBIY4Ve44v8O0/GIX23U7rE1DKRQAOACeibx4EXnRhAFERSiE7H62we98azjz/crZuPYNarcHC4iJBEFIuF+kl"
    L"CXMzsxw9drQ5Pzezo91ufbtWrd7xj5/73ETuk14RMLxiAPjKV64bGh4ZOTOQwbsM6v1CyHdt27pldP26dQwNDZIkCUopgiCg2+nw"
    L"wv4XeG7/C5y2fgtnnHsxCRHdXoLA5NJXBmUMShuX2XOCTYVtBR6nQMiBIlYkSrP72AK3/v4KaN/lNF5bS6D1i0Hgpa1yP6zPGgjL"
    L"DUpFuqHkoR/UOHB0DWef/xa2bj2DRrPN3PwCUkpK5RJCQLVSZWZmiomJyflGvbqj0+nclyTxD2q12uEvXHNNM/2hL9N4WQDwpS99"
    L"bWRgsLQmCqPLDWZbIMTlIpCXDA4ObNy4YUO0evVqRkeGAUiUSr9IZbHCs/v2cXx8gjVrN7P5rAtQskinGwOGQIIUloMJIRBgzX9O"
    L"+InygrdCTk2+0/puohwHsK/tPrbArf9xDFp32y/vhW/8ZqyQdS5T6EPFvGXQJrucUkIUQrFES8HupzW7nx/gjHMvYcuWbSRKs7hY"
    L"pd1uUywVbW8ChsriIosLC0zPTLfrtfqRTqf9eKLivcaYfc1mc0+9Vhv/0pe/3CL79JMeywaAf/zHa0vrN6z/TYH6JSG4KCqUhk9b"
    L"s3pgeHiYlSvHGB4eIooijNZp+lUA7Vab4+Pj7N2zh3qry8bN53Lapm30VEC7F2OMJgggkIIgEEgpCKR9BAsgpTSxNtbveyugdEr+"
    L"4kRbMpjkwZEQx5rdxxe49ffHoLXDCVRbEugB4AVucvbe75fmDHQODII+IAQhlEt0teCpp+vsfW6EVWvP5axzzmVwYIhWu81ipUai"
    L"EsqlEoVChBCSTrdDo96g0WhQqSyaSqXSajTrU+1m87b5+fnPfO7znzvGMnCH8KUemB9f+vLXymvXrvkfZ56x7WMbNqxjYGAQIQRS"
    L"vrjUICS0G03Gx8c5cOAAM3OLlAZXsGHbm9kyvJpGRzFVjRH0kAEEaWo2sOl6Y+yvNu4yG4FBYIxBCPfaElgbk3sxz+s8yfeJHr9J"
    L"YwtDXtBgawVG9mu9t//5/XwCyWAtidIQJxTDkLddNMAVl2n2P/8ke/Y8RbO1nrUbz2LTxk2UBwbo9RIajSatdhswDA8PMzY2xtat"
    L"W4TWZrBSqZw5NT39H3709DOnA78KxJxkSLksAFi1cuzSVSvHPnbuuee86D2jNY16ncXFBSYnJjk2Pk6j2aUwuJLTNmzj7C1vJTaS"
    L"Rjdhca6FCDQSkIEg0MLafAQCjTICoa0gjTAIBMoYjDEYI2yuxm1WaUUmH2Pse4Ah2x9wKWCRAcFIKzzhnqNJiYdx/xhhXzOqn7Z5"
    L"ziDc+bSTj9IQx4huyDmnR5yzLaIdz3Po6AwHdkmqzTGGV29i86bNrF+7EikDEIJWu0O71aZWryOEoFwusXrVyM9/5CMfPffb3771"
    L"OSwIXnKWcVkAUCxGG8vlcvp3bXGRvXv2MD0zS7sb01MgwgHGVq9n9PRLWVVeQU9Bs91jYb5jr60xCGEIjJW5FLYwY7RBaIkApBAY"
    L"4QUpAI3WhkSD0ppEa5S2pFArYzt+tcFokwOFSS26yXeFIrK434hMgAYgcBqurZHQHiyeHHow6BwYvExE9rcCEgNxArJLOQi5YHPE"
    L"BWeGJKLOxMTTHDryBAefLWLEaYhoiIGhUbadcRbr153GvucOEAQBoyMrysPDQ5cAh7FnzdPSn2osCwDiOBkpFKL07/t+8ACdYCUr"
    L"N7yVwbCACCN6CjpxQjOOSTodQKOd75RGI4QVqdHW32Mg0YJICpTWSCFRxiC0QAmDFFaQXsh+S/J/O2H7SEE7gGhtw0dtsHzEkMv4"
    L"OaEJnJAh02hpiaDI+RAhM67g3RW5RxzaPKDQjlRqSBTEMSAIw4DNYwGb15QgDNBygVpzmmoF7rpjBx/46O8xPDxIp9smiiK01muA"
    L"ItAja0j5qceyAEBrMxiGFgBaKRJtWHH6xTR7kqSTEJseSts+Sm00WisQAqMVQhiUASmskI10pXllkFhXLIywfEsZEulMvzPjymiU"
    L"Mjnt16jEAUBp+7cyaJMHhQWKdm7BCgdSweOe+wed+7EmL2XPG3wjiXvMgyHzO/3HCWdplANHTwOxC3MMMgpYEYas2FRiw7oq9VqN"
    L"IAwQUhBYbjUMFDjJ/oNlAYAxekOxWABAJQndRCFaCUJGJFqhhUKhMEaTaFshNS7ONljyFiAw0ppYpaT1+VIgdIAUzpQLgzYCgTfj"
    L"NgmkveCVcaGgDQ0TZRNEPlRU7jFJsuepBUBmwgJHrTwIcuTCC/NF+iYyi59aEv/6koNS0umBkpOhNvbztIIkAa0ZLJfpdjtIWUIg"
    L"EVJSLg+OkTWgvOSxTBZArwyC0AoiUfQSQ6gNUigUmlhbrVdCo5VCGY1AYxzPsgooCDFIEaCdeZdaYKRxPEqjpbuYIvPpqQvwQtU5"
    L"ALj0cOxSxLHKrEWibNZQe5PvBZWSeZHz6XnB6SW/PmcR0qc5juBRkYInByRw+utIZh+BNLZ1CRgeDOm0GpSGi7YEEQQMDQ2OAgGn"
    L"hgUwq8IwRGtNHMfERiJkaM092vpeo1FKoYyyvlxZ7ZdCEGB/ikFYdxoaF9pZASdYy2i0QbjJHNaPu/cd6Uucb7eabzU9VtpZA51m"
    L"CxPnppQjhy+y6H0yzsf4Sxz8UrOeEkJtxaLzFiS3qyeTzq2l5zJuX68VAhCSoZLkeLNDadgCQ8iQYrG0jkz4ry4AgNAYg0oSlEpo"
    L"dbqslFhTq7Q11dqmXrXSKDTCONtlDEYIdy0ty9famXmhMUoSaJPSXIMlbtqAVsb9bTU5SXLmPi0MGavtSZYgihPjqofWaoB5sWLn"
    L"BZZqrD7xfkv3J7NUKShkfr+ctfEIVLjQ0zgegE15SkUUQtzr2mgVO4VNBkGZkxQ+LBMAlFLDQgqUUnQ7XYZHVyCl/32uYOP9tLGa"
    L"DN712oshjUA7QaTJHIV1IylJy4RvciROOa03BqvpOTcQJ7Za2FOGntIkriTs30+bQj0IXiT07O2+S93n51kCAFyEwJIXPU9weYbU"
    L"LYisB0E5zZDGbipAxJper+e+lj1fGARD9GWvXtpYHheAEdpdXCkl7WaTFdoJ31ftfIiWKLTRBE7rBTavr5VGSGm7djU2WSOs2gi8"
    L"pjuBe/NvtP0MlRWF0nBQ5VyBqw94vx/7x8R+L/sjThTD58y/8BlBXkwA+3JxeVDkyF7ezaR1A0OadMq7Ga2t+wgMBCrNgPphJ7IS"
    L"cKpYAKOMsDG2QmnF7Nw8G84SDtwmi72VfW4tniEQoDEIKa0yKIPUBiNtxk+44otVFE/83Lm0AwTGab/jAD4ppHSuUKT6QZB46+C/"
    L"T84FeJfQJ2Qf6jlHlOaQ80we+ggfS473pDI9H/SjSeSaUB0AXGIkCKDTbfed0QH3pIQPyxUFGB1ZDqDsbw8jK3xnBVBWh5XRGKXR"
    L"GKRju0YLTKiRWoDUaEBpgXQduib9DOPiee9WbIIHn+QxVvBaW0AorUmMIU5csSjRzgWYXBOJcfUckwknLf+anFD9t/BJH5Ht449J"
    L"k0OaLIEEWVKJLOWM29+7CAHW94msj1honL+kVCoSSne4/ybypGUPLF8UILW2LVZeY61L1amv1samZBP3qNCEUlplUiCx2msLSBrh"
    L"fq1JBW4F67N3eS5ALrHjs4HakU6fC1DOIsTaRgBxygFyk0Ey1OaEK/oVVeb8derjc4BZquR93EBmgs+Dp8/1YC2BzFkio5FBaOsZ"
    L"6VUX2dOTGMsEAFBKI0isZufKvTonGGW0zdF73+osgTTC8h4hQGmHdJ3yLOXP4Uy9T/EajKvK+s/Qabo3USZ9TDU+zQ044bvSv20E"
    L"VWTxet68LyWDTovT5JBvHFGukyixm+8rSHybmbbv58vNytjXlHZtaP6zQ4gKtqcgEqAlpi+MyD+e3FimMND5YKWInFbYdi1bulXG"
    L"oJW1BgbSxI4xlslobX1kILEXQoi0eqfxft0J2AHCWxjtOYXJ3vPJHp/yVVqnIaLSxoanRtvr7rXMCzrNzNnfkWWqnLBVD1Qb4jYk"
    L"beh17YbL7bvsXZrdy9cT/PDhnweIcZ3IOPchAzAxyCLoEIwvRoFwpkUIsSw+YLnyAMInZ0wgclbPXjgPbO+3tbFgEDiy66qB2nMs"
    L"J1iDQDuNVQa0Vin7184VGxdaasc3knzhx1mN2NcCVM46ONdiTqRQ/tpqDUkHek3oNaBbg14LlOsb9NVCz8d84scTubSE7ECkcx+S"
    L"Vhoh7TPw9YH02pFZBZ3YXXM+4CVLKzeWKQzEcRmrQsJl8Qw+XavRRqC0SMM2A0isFbAeQdvjcG4j1XxFrBxv8AkgXInXWRTbxa1T"
    L"guiLPkr7zzYp4/fFIO26SjL37OJP1YFOHToL0K5bgWtnulPf7YUnM4Hlc/8+kZHzJP6tfsstM1cuXKQhcLkRXNnZ9SOYl1zx/bFj"
    L"eSyAY33GISEFtrdqxsfsmRvwxA7htVGn5WDlhKxcelepHIdwgvMEUKXhoQeA5ww65zocIDwYnXWJE0O7m8DifVDbD80adGqZwPPC"
    L"9FahT6CeqMnsb5Mz+3m3opcobOplfInZnVc6SyLd8/Qa2++Ta7I6dUggkPiEijG2XOkzVt5Cejav8KbXZCTXAcM34irtwsG0eJMr"
    L"7xof/9uTarLIIP2MXJ1AudcTrdBaECtNrZ2w0O7SiKGzUIHJXVA/Yn1uKvAfU2RL3bq3Bs7c+6hAi/790selOQKRVpLtMd4VSNcL"
    L"B0jRhyP/rzFmCaJe2liuMLCvuybutTHOBKbE1ph0P+X9v9GW4bpCmHIM32hNrC0A4lxCx4MD6O/uSb+DF7rLGxgwWqGMoNpKWGj2"
    L"qHUVrURnGm3IiNdL4lUmcw16SYxvXLTQlwug3yWAI5oye0P6v23EkcQ2c5nlyMmlsE9uLJcF0F4AhUIBo5QFhMiZe+0IoPaxtx/K"
    L"/q+cNivru5V2iRtX0Em7ebDFIuOygJZnmDSM12T1gk5PMd/ssdCKqfd0ajXwj2KJpv7PhheqF5QglwbOdLRf6LnPSAVoMnB4LuFT"
    L"zd7dCOFcgKTd61EoDrjZaqngTx0XIIRoJImd1GGHSS+CcSbAXyevpTaU9mTR/eefm8zvJ8oK32fufB7AWxztQzl3fq0NjU7CTKPH"
    L"Qiuhq5aW7/qtsMinc3/sj+TF+6Sa7Ey79ByA/p1l7mDt3ksthCsdqxxQpAeAPbcQgiAIbeSU1kdOrTAw1q5vT0prz3VOQEqLNMmW"
    L"lr9dwQfjtdj7b2cp8GBx5Fz3+/T0/Ib072o7ZqbZpdJWxGlefYnbdcMqo3cDToBBXoC591OlNzlC57XcWwNJ5sNz58gL3wvVmyoB"
    L"aSQQuDc8EfSuCRv+GkMaJVmLqjs/rZBONJbLAiwmSWxRiqFYiJxxFi4aclVBk/nqvEw8APxmvLAdy9e4CMCzf5M7Rhtq7ZjJRodK"
    L"S6GWFGNOKPwXKY8nEmTmG/o1Pu/DU6KYO87vLN3zpaw/+3B7gCRzD55vapP5fhE4MghSFvEtd9J9d6WSLv2X8SWNZeoH0LU4jimV"
    L"ygghCQQkSYwxQVoQSk28WUpgcuGjM+2Jj9WN7/fzx2WkUhlDvZMwWW+z0NS2Tmfy5zvx+LEJNC9MmQNB38G++TP/AUuacoTLDaQm"
    L"fimK/Gsi/b6pSfDCl24OXGDP1+hqEKG1fqRZ10buW7/ksVwW4Fin02FkZBRjDOVCRDeOKYSWWdtJGN58aStEx5w9gfaXIY0UHAD8"
    L"fz7M00bTiTXj1TazzYQ47aqx458T/okEn7mBJQf1NXOIf+Y9d4a088UJ1r8f5l7z7/cBxbmDlBzlJqOITPhIQbMlKAwWMNrlUYym"
    L"2+nM54/+sQL6MWN5wkCtanEcuwmbksFSSNxtUYhc15KbyGlSoZJ2BQEO2aS1g5QsnmCbqnUZr3Rpe3KX++n/rPB/kteWkrw0hCMT"
    L"pHcPfr6aj91Tc74UTIIsSZQDB0tCPiBdksYJ3c4rtI/1NpRXllHKdVbHXer1+hQnIXg/lisV3Ihjn6s2DJQiZrothoZWIzCuJEyW"
    L"HHKtdcJdQ+P8ft63p4bRHdfsxRyebbPQVX2R3I8198BSspwpr0gZdsrK82Ga38d4k5w/2vvxnEk3WCafmv5cMcgL3Qu7zxq4XypM"
    L"to+UmRuQAbVWwFhpgFasXPu7otPpVnBRLycBhOWaGDLb63URQpAoxcjwCEerLUtY0sIKaJ/rdxTRukCTcrCl2u6twGSlw9Fql3ai"
    L"T8AhTjyWmnzR/2b/a17wgSdvOUELmZG0PDmUuaydJ3yBEyLCgkEubdrNZRf7yKOAQJFGBFJAENjkVBCQUKBQKtHoNjHGEMcxUop2"
    L"/iwvdSwLALrdzmy32+1KKYtxkjA6NER3qg7g4mxhNV6AIbCNIobULQgjcqbfpC6gEysOzbWZrHdJPCJ+gvHP0jyRfaP+1127jV6S"
    L"/s2HcP544X6If0zTufnJo858+wKPj0x0bv+0scjJ0E+EFdIKX2SWxzBAqVTGVBoYren1YrTWTU4VDlCv1+dGRkerYE6L45jh4SG6"
    L"7RkMEIaB7fFz5lAb23jhBW3n/WaxvU8b19sx++daVDs/XRVsqdnvM/n0g0N4IXqul+/lz+/t38sOyp3J8wNv7vM75/MCYklOwB9u"
    L"7IRRTwtkDjwyIMHQ7hUpFItg7HoI7XY7rlRri5yk+Yc+m/TSx8zMTC2O47pS1kcNDQ2hug2M7V+3M30BEKm229k9OVfgLqoBZuod"
    L"Dix2aCfeafSPE+iwfT3vcsi59BOY/NT3p284TfXJzLywZP483u87P730UQY5P+4eRW5/r+X+PaRNAgUBhO65dO4oCGj3esSqRBhG"
    L"IAxKKRrNZv3okUM+CjgpECwLAG6++aamUmqy2+2itaY8MABJB2UUobRz2UTaKGLDJt+No1L/byOE8cU2x2uJnT20RJj/nOCBE+yb"
    L"/yuncEL0nSfDgSNiwl18v0+Q38nH6UGOqEnSdQOD0ArS+28vdC9c6c28yNxOSkJFyvqzY0LaPcXA4CiJSmwyTWviuFs/ePDgIsuw"
    L"FO2yAODQoYMGYw51Oh2MMURRgVKg6CaJXdIFiXAXL22icQUi+1wQK8OxxQ5zbZVOFQd8D/yPHUuFnz4Rog8MLyKGqYkwGWo8MZM4"
    L"U+zVX2Z+WQRuC0E6oYsw0/4gp9VBXrAiS/D4WX0y9z5hv7UIQyp1xdDIKrqdXqooSplKt9vtkVmAlzyWqxZglFJHWq025fIgYRAw"
    L"UorodFoMD4ymWmdZf+Yz08yfMhxbbFPtqj5N7xfciUF+YuGfwOQv1Xr3hnXZTjjGzbXwXRepa8h/ggOCF5bvCvJ+3qMorRqKDFQp"
    L"5xDkWDAvqj+krqTA9HyX0bF1LvxLbBTQ602QLQzx6kcBgOl1e3vq9RqnrV1LnCjWr1rBwdocw4MrkFJihMTl/dLuHAn0Ys3xSptG"
    L"z038dHtlm31dYFzmMEsdCwciWCJ8Y3Li9sI36ds+B+9b2EzcIG430Kbrqm15X58/kdVOkQpI4lcrQ0h3bg8en9P3UYD/cP9V/G81"
    L"mRd31T9UADIBU2JhvsuWTaN0uz2UStCqx9TUxAHsohCnRhQAmF6ve6jVaiWBlGGn02XThrWsjJ9gx9EyC/U2lVaHXpIQK3vHDoNB"
    L"K0Wv26PdcQtcuASAUBoRK2SiELFGJAqRaDuj2CUHUi32DD5H6MRP8XxlqcMXvttgtr4KZQIMAiFknjumbsjnNayblkhhJ7AEUjhL"
    L"Lty+gXv0ILAWUAiR1ZvSDKDuMzJCCqSQFAohwyvKPH1gLRe/exVzC1WSRNFsNul2u0dZpjuXLRcAmJicOLpi1coJg9ncixNWr1rF"
    L"4ecn+PSvHYS4BapGWuM10vrOOISO96cyF0vnlmo1id1UG3SXdClXv6Aj0Dev76e8Ho/e8x1+59n/RLuTWKE5kuaV0QraczRp+Z6U"
    L"CCGdSxeW6HpXLmxLnHQgk0ISYM9rQSVSrukTYRqDQPrFQQhFQFkNUkpKFIO7WTE6yszMAkmiqDcaSa+XzNoLkYLgJY9lA8CnP/3X"
    L"85//wrUHO+32ZikDxsZW8sAEkBy2rdXxOGlZTwRW+I0QiEAW7Ffxmqz9BIsemA7oNugOqC6Ynr2di3CNmz6HzNJE0f8ECAZMopmp"
    L"CZTSjM830zBSCLtOkXAabmmBIBASGUDowrgwcBZASBe92b9DEThaIZAIdy5rMYSz9UL4DKhxyTDpDIEkkhFjw5qSaLJp7ShxrGzE"
    L"lCTMz81NP/rozoMskwVYlijAfQmdxPHj1WoVsK1hMlhJp90FUwZdsJk2I+3CZk2TS5K4R+mpsaPJKdOO7CNuC/z7MtsPF4vz02wC"
    L"lS/W4KMw4bRX4HMGQopUqKRa7rt1BML9HQjhIjzbHBMGEhnYMBgHEB/5GUcyZRBipEwTZkEQUAwj4uYUF5y9kXavQ7fX8zOfZmZm"
    L"putkHOCkxnIBAMA0GvUfzM/PE0UFOr2YdRu2cvSFBogCUCS9A0fb+Xs/vcrzmPTuHE6Y3jWQE7RY6jI8WcsHfD+5UlgybrVPysxf"
    L"W8LvTLdbmVRKaR8D7xZEn6Ww+0oCKQlDt1Cmdxe4mU+BRAb532ilaHNCAUJIoiCgWCqgmuNs3bqFdqtDksQolTA7O/ckVoW8Czgl"
    L"LACAmZ6e2l2t1KalFLRaHTadfjrP7ouxDRaDVmBdCYlzdvnlWPO3Zkm/Wi5g9okThHtNLNn/RED4SYYPBXOhozP5lvh5Uy5TIiik"
    L"cxGOsAkHjiCQyFAgAhspBIF/7riFyy6atJZAahU8EkQgKBQLlCLBUNhk5YoxGs0mnU6XyuIctVp1F9nScKeMCwAwf/M3fzPRajd3"
    L"NZsNenGPdWvXMTk/iE4SEGVIArcsGmTRi7MCJvdb+vLtPkES9ms/QbZfStfdP2lU8BMAwQlBYIUuna+2b2XabTO6/m+ZCk5KSwKD"
    L"QBCEMuUJUkjrRoxn9iI9PqsCgpHCTYyxIaOUIcWoCJ05ztmyGiMkvV5CkiRMTU9PPfbYY/voB8BJjeUEAIDudFrbZ2dnrLkMQkoD"
    L"65mYnAbKIIpOWE7Qxk950i42ygPaCz/IcuZe+N4t+HTrizRf/ESy98OG9z5Ucy9I78u9ppMKXProQDoNF84CIJEOBN4diMAFnNIv"
    L"mIFdE0nY8jiuM9rackEhDCmXC7QXXuCySy+i2WrR7XZJkoR6rfHCkSOH58hcwElpPyyzBQD0wkLl4bm5hXYUhTRbHc459zyeeEo6"
    L"Sz5IWkoF0ps1GkU2zcpbAVI/bI8JXKYucL5/qeC9i/AH+4cTICFnfPIt/Jnfzxi9cCGeNeEmBwhh1zN2IJCuiCNdVtG7CuETR+67"
    L"2tRPNovS5PMEgaBYKDJUjCgks2w+fTO1urWmrWaVqanJ+4EuFgAn7f9h+S2Auf++e/dUqotPdzodWu0WmzedzuEjAVokIAcc04c0"
    L"/+4nEPq4Pg3rICuthmQk0FkA6TmCyKxA+tz/tBwgvND1ksfc8Fk9HwFYc2/6TL+Qlt1b7fb5ABc6BjIN/TzrFw5ZNtvrC2K2QyrB"
    L"kKBTSUYioFQsYNoznLd1jEKxQLfbpdfrcfz4eHX//uefBjpkq4Sf9Fh2ADzwwwfajXrju9PTswghCaKIladtZfxoDcIiBCX62brz"
    L"/9q7tKVuIM+YZfY8dQ35tZJyfKDvW+W2nIn31kg6E+7Nf1/mjxzTz0UCgRQEInCvidTne4IoHX/wn+Gzv74vUiHcUncO80IQhiFD"
    L"5RLN2b286+2XU6836XZjkjhmdnZ23+OPP36EzAKcNAGElwEAgJ6Znt4+OTVZKxYL1GpNLrzgQnY+3IOCgWCIrFUKZ4OVy/g5V9DX"
    L"7JcXbj4/4MJAsRQEZMWc/OXxwjDOhbichDDO/LrPEn2m20cD0ml/gAy8mSe9cYXMhYt2KVfhPJIjeu6L9Bsf1/bu0sNSBgyWSgwV"
    L"JAN6hk1bNlOt1Wl32rRadaampu4H2pzk4tBLx3IDAEB/+r/+9d7K4vwParUanV6X9Rs2Ml4ZoNVpWAAEkTP//hB/Wfy0bG8Z/Ps5"
    L"K9AXFgYZmHJNJynb8qAwIuMNHgiuMSOr9jqh2WpAarqtuc8JWjjS5+5eIgOZZgmtwTAWAC5sNJLU/GvcqjDGL4Xnp8YJCkHAULlE"
    L"e34f77r8PDCCXi8mSWIOHjywuHfv3p1Y899jmfw/vDwAMEAyOzv7P44dO2oKhSJxrDj/gjfxyL3zUAghGMZX7SwX8BzAr9Xjlwox"
    L"mSCX+v50Bm0+Axhkwvb8weSaN5ZmCnNFGnsxskhAuGyfZ/hpEihPEAUI425fk8v+pSEeuIUshGt3823xIu2H0MYQyIBysczIQJnu"
    L"7F7eevnlVGtVOt0u3W6X+fmFp3bteuY4/f7/lAaAvu/ee++dmJzYo7WiUqtz7nnn8czhQRK6EA3bYpAQkC7GqNzm19jxoSFkIWGO"
    L"/Xuz73vzyXXh4COGfBbRC50+b+EjAOHZuEvreo7gEzle6N4SBIEPA7P4XuTayEQaXmSAUOCmuPtwkDTzN1QuIjvTXLRthOGREVrt"
    L"Lu1Wk4mJ4/HBgwdvA5pY/78s8b8fLwcAAPQPH/xhvbJYuf748eNIKSgUSqxZeyaHnm3b++sFA6S99mkI6EmgZ0b5UndO0J4L+FyA"
    L"F7wJsEmj3Osyr/H+PEuIovAaTC7xI3MET0CQpXozQft6gbccBimFW/vYtcC7dYz95Fbf9Kq1XSNRIhgoFlg9NED12E7e++6raHds"
    L"7r/X63H40MEDd91111NY/9/lJO4OcqLxcgHAAMn+/c996/CRw1NRFLGwWOWyt1zK/U8pjDQQrXCCc7t7MugJIQl9dQIg9ecpD8jX"
    L"CvLJoiXhoswRjlT2IvtXZKGf9/nChXo+nk/fw3MBb/IdaCRgXNNLGnUaDDK11wKJFrkuDgFRGDJaHqBkmmwei9m0cRPNVptms02l"
    L"ssDExNR3ut1OHQuAZYv//Xg5AaC/+tWvHluYn//m5OQkiVaMrliJDNdx+FANorJ1BX1kf4kVEEvJrshlBb1fXxIZ+Apin7tIP2DJ"
    L"V3Sn9DwgJXzSpe0z0y49H0i/hiSQwk38WcL6XWThO6D65jrmDFsoJMOlMmPDgyweeZQPveddKGPodrt0Oi327Nlz/L777r2XfvO/"
    L"bMKHlw8AYKUY73/+ua8cOPBCpVwqsVCpcsVbr2DHg8pZ6xW2kxZHCNOQ0KeI3XOdQ4nIabr37b71WuYiBBlm/rev/ipyriC7Cmlq"
    L"l3z+3z0nd7/CQKbk0EcHuOOMcL9B5GY2ue5nv2Cm8wAIISlGESPlIkMyZk1hljPPOZtWq0213qDZbDE5OXn3+PjxGaDFMrP/3E9/"
    L"WYf68le+vH9iYuLrExMTJIli1erTIFrHkSMtmxiKhnPK6SMClxPQzhLkf7NYYgVEjgvkw7y+QlHu9ROAQLiyrHUDpHF9nvgJ/9Hu"
    L"icxpv8BpvbD5BRvUmnTLLIIXvm0fGy6WWDM8xOKhH/KxX3gfUkp7m7hmk2f3PjP72GOPfQ+r/cua/cuPlxMA3nnHe/fuvnb/88/N"
    L"l8pFFhaqXHnlldz9wx6mYKCwIosI8kBIw8Jc5/NSoeJJnweBzBHCXCjoj/FS7Buu+cNbgLQymMX/wnUA9QPD8oEsu+h8vtDZdHdv"
    L"BeziuNb0C5AioFwsMjY0QJku60qznHPuebTbXaq1BvVGk8OHD9+ze/euo2Tmf9m1H15+C2AAdcMNNxyYmJz4+vix4yitWLlqNUFx"
    L"HS8cEFAoQmHU7p0KP5cY8lFPuqyI1+hcUqi/0Z4MSb5WkAcOS6wAma8Xma/P2sFEDhgyxZHJHeMbSLyJtwtd+mWA7V1Psm41m18Y"
    L"KZZYNTTEwgt38/GPfAgB1Bt1ms0G+57dNfPggw99C8iTv2XXfnhlAKCBeNczu6597vnnpoulArOzC7zjHe/gjvvbJLLnIgLXMdR3"
    L"WK5AdKIagecDRpKGf2mdIA+UfNC/lAyKNFAQXtC5Bs5U611ewLiQEaGxFt+ey657JdOZTv6WdX5+gHGfFQjJQKHAqqFBCr1ZzloT"
    L"s+3MM2i22zQaTWq1GvtfOHDH3r178tq/rLF/frzcAAAXEt78rZsOj48fv/bQocMYYHBwmA0bt/HIAzGUC1AYy4Vu5DhBLjxMVxmH"
    L"tCwsckI2QY4D+Lh/yT5e/n5ugIv9lxaBxAm4AHmNd9ZI4CRjvJhtk6lxK5ol2HWOcAApBBEryoOsHhlkbv8d/IuPfhhtDI1GnUaz"
    L"zq5nnhzfsWOH135P/l4W4bur87IPr869hx588KvPPffs/iCQzMwv8Pa3vZ3H9wlanRhKIxANZmlb6c210/x8UqiP3ecIHsKCIk0w"
    L"5cz+i6xAbjhhZynevNl3M5pSPuC5pHSEzoLBr2mg3Iqnicvx2/yWXSInkAFDxQKrRwbpze7lvZdtZePGTbTbber1NouLFQ4cOHjb"
    L"4UMHJ4AGlvy9bNoPrwwAwFnFHffumDx8+PB/e/75faYQFej0Ei677AruvNPAYADFVTYszFsCST+j8lnDVMuFI4E5hm9yoPBNpkCf"
    L"6XdWIO368aY/r/GQ9f25JWMsIGyTp43xSZcA8svZWotg1yv2dWApBMUwZGxwkNWDReLph/nZ978fpRPq9Sb1Ro1Hdz5y4Hvf++7t"
    L"ZNr/svl+P15JAGgg/ta3bv7Wvn37drTbLSrVGuedfwGT1YiDz7agPOhcgROuDBwIvBWAvjLxi1j+UoZPZvJT399vEXx3rzf/+ckh"
    L"wtf1pXAfYT/P/p0tZw92Mcv0JllureP0dnhIQhkwUhpg3YoRZp+/g3/14asZGhqmVm9Sq9cZP34s3rVr1zdmZ2dnsdr/svp+P14p"
    L"AIDjAkeOHKkdfOHA3+3Zs6tZLpeZnpnnfe95L3fcHxILbQHgXQESjAsRpQ8NvSXI/YK+JlGZ5VnTpdyXfJO8IegDQNbEIV1q2GLH"
    L"ZC3j0t123pl+Y0Q63yVbzl646NVaI4lkoFhkzegQYXeObSNVLrn0Ujq9HrVancXFRR566OEHv/Od2+8BavQnfl7W8UoDQAPxNV+8"
    L"5sEX9u//2sTEOIlSDA6NsmnbNu7f3oOBIhTWQFDIFXNC903zIPDTwnIhnid+fXf9zieRlnwjkTV75rVfpskmLMXwbsSI9KO0yX0M"
    L"pItWklv5xKcISlHEyqEh1g4PMP/87fzKxz9GEAbUG3Xq9Tq7nvnR4mOPPXodUKXf9y973L90vJIA8EMBncefePxzz+x6Zl8YSqam"
    L"Z3nb297GvinJ1FwCxWEorMQvkpSCIPAsPmcJ+rJ6PkPoJeRfXzJM9rJ0hFH4BpEUDMZxUfuacbc6TWcm47We7N5DBpSQZCu5SyIZ"
    L"Mlous2FshMrh+/hXH3w769evp9lsUVmsMjc/xwMP/PCWRx/d+RyZ9r/svt+PVxoA3gok3//+9w/tf27fX+3atbtXHiwzN1/lZ9/3"
    L"fm66cZGedISwNGKzhFFuToCPDvLuPNXQXAbQmWhbR3DbCeYOpt29wk3kyFkBiU8Q2N0tOTT4yed+BdT0XoaIdAKs0YIQwWCpxLrR"
    L"UQrdWTaXZ7jqqquIVcLCQpVKpcp9O+7Zs2PHPTeTaf/LlvU70Xg1LIAP7Ls3/NMN392zZ9c3pqcm6cUxg0OjnHXexdxx+wIMRtYV"
    L"RGUggjACEZE2fvhTeYInRW5lrvwEEocSr0/pRFL3t8gVdshNAPGpYOfvpRC2X8UtBmF7VoS70YXJRRzW7wdCUoqKrB4eYs1IkdoL"
    L"t/HvfuXjhGFIpVKlVq/x3HN7Wz988IfX1Gq1Gaz250u+r8h4NQAAjhAuLCw0H3rowb974vEnnpFSMDUzx2WXvZWJWomnn0mgNADF"
    L"NRAWbHgYhQ4Izi3kiZ6XaNopnE8Ny8wi5NIJuKpcKN1kz3zZ11f5ZNYyJnx+QVkfYtC2cUlIcoVAO78/CFk1NMSmsRXM7ruNX/v4"
    L"Bzht7TqarTZz84vMzc3y4IMP3bxz584nsNrf5BUifvnxagJAA70dO3YcfmbXj/7sySefaAwOlpmYmuXnP/gh7rp7isXZHhRXQHGl"
    L"I4VFtxWsNQiWpnzdJiV2KpmbVexnFot8uthaByFD6wLcJI9QCjujNzf7V+aSQGBseGdyax36RK+bBFoIQlYMDrFx5ShqYTfvPn+E"
    L"yy59C3HSY3p6lsriInffdcczN9104zeACjbuf8WIX368WgDwQwHd66677t4fPfXkZw4cOEAQSDo9xfve9z6uv2mSWApLCIujVvvD"
    L"km0pC3PTxqVP/uS0Xgb2/aBgASNLEBT7t0IJGRTsbF4ZEIYSKQOCIMjKwDILDXGpX9+sKgkQ0i4AIUVAgKAgQ0aKZTaOjjCgKgw1"
    L"nuCjH/0lhBTMzi5QqVZ4+umnqo88svMfOp32HP3E7xXVfnh1AeCNcQK0b7n1lmsee2zn9+r1Oo1mk7XrNrDt3Au56etzmKKBcA1E"
    L"Q+5umiUIB9xEEwcE3wCSbwmT/nlkW9GDAoRlCMoQDkIwgAgLRGFAEEmCICCKAjvpI3STP6Rb/CEtB/t1ACAIBNLYOQOBEIQyYKg4"
    L"wNrREdYMhdSev5n//bd/nYHBQeYXq8zOzjIzM63vvPPOr+zc+churPbnkz6v+Hi1LYAnhL0jR44s7Nz5yF88svPh56MwYHJqhre8"
    L"5XK6MuL++xdhUEK0ygo+iKzww7LbvJZ7MET95l+6NfxkEXCuJCxCWECEkRV8aIUvAzfTVwYEMrSLO8jAzRqWWbdQYDU+DCWBDCjI"
    L"kMHINnhsXDXC/HO38Lv/7mNs2LiReqPBxMQkc/Pz3LH9+3fcccf224BFXkXT78erDQDIRQXbt29/ds/uXX/x5FNPNgYGBzg+Mc0H"
    L"PvBBfrS7wd5HWjBYsCAIylabo6KzBmX7GBQdABwggtzmQSFLbrN8IpAhxUJAseBAEAgHBkkUWoIYuU1KQURAGIQUgsC6DRFSlAED"
    L"UYnVI9bvT+++mV961/lccuklNFstDh85xvz8Avfde/e+m2+++RpgAUv8XjXT78epAADIuYKbbrrpzj17d3/uwMEDqlAoMDe/yMc+"
    L"+kvcfv8Rju3pQbkI0Zjz4wXLCcKyay8r2bAxLFi+ICMraFEASg4UUc5SFJAyJApCojCkUAgoRAFREBAF0hFCq/FBIImEB4XdoiCi"
    L"ICPKBdvcsXFsBa3xH/Ku84d43/vfRy+JOT4+wcL8ArueeXJhx457/3Z+fm4ca/rztf5XbSzbIlEnOdI0caVSqX/39tuvGRoY2joy"
    L"PPwvR0ZW0O4m/OJHfpHrv30b/3rwfDadPmjpd9wAoTK/r0P7t4nsZFMVu0kminQaOlgS52oIIggolAJKbUsetdE2B+B208YdqgUi"
    L"cFba+C4g6zaGi2XWjQ4j6ns5c3CSj3/8t0BIjh05xuTEFEeOHOjcfPO3Pvvkk0/s4RQx/X6cKhYAcqHh7j17ph5++MG/fezRnQ8p"
    L"FdNstYkKZT7wwQ9ww83PUVloQmHYtpLJYsb2U1fgSeIgRAPuuXsvdFZClkCUkEFIMZREUUQUBZQKhcy8B5KitDN3CgVBoRBSCCMK"
    L"QUhRRAwWCowNDLJxbATRPsRg9XF+/dd+jWKpxPj4BMfHJ5ibn9N3333PdY888vAOrOn3CZ+XtdHjJx2nigXww7uC7ve+//1nB4eG"
    L"/rJQKv3F1e9+98WVao2VY2O88x3v5EtfvZ/f/OSFjA4PWLYfN0AmTl0Da+J1Yh/TKWe5dQiUsI+iawFQCCkVNEL4exK7m1obg29I"
    L"MEoQ2M4RhLYuYigqsnJoENU4TDh5D7/1yd9mYGiIiclpjh47zsLCPHfduf2W66//xo3APP0Jn1dd+HBqAgAcH7jxxhsfHhgo/3W5"
    L"VP5/rrzyyjNm5xZYs3YDV77jXVz7hQf41Y++jTVnY5m+bkCSgJ8QKqS9C4cO7GnT7mJj7+xhDCiJlBHFMKRUVnYpewNGaUAiXEs3"
    L"CkwgkMYufB3KkKGoyGi5RLfyLHL8fj7527/BirExpqdnOXToCJXFeW695aYdX/va164B5rCmP5/rPyXGqQYAyPEBoP3Vr163PYoK"
    L"w+Vy6T9ecsmlW2fnF1i7fhNXv+d9fOmmu/jwR3+GCy4oQHcMRANEx/n9wGq+9Ld+z7eV6bSnQIYhpSBioKgRRtiWLmWrgcK4ti8l"
    L"QAuEDCiIgKGoyFC5wOLEE5TmHuW3P/mbjIyMMjU9y+EjR6ksLnLHHd9/+JZbbvnv9Av/lPD7+XEqAgByfAAQ11577c1SirIMwv/w"
    L"povfvHFmdp7Vq1bxCx/+eb5963cQ9Qs5/4oVEI9YPpB0rPB1kGl+4Aps6S1aLAhkEFCKQsoFX9QxCC3Q7lauRoEMA0IExSCiHBWI"
    L"Asn0gXtZ1d7Pr/7Wb1AolpicmuHI0aNUq1XuvON7j1x//fWfbjQa4/T7/ZhTSPhw6gIAckkioPmFL1zzdYEoBFL+3kUXXbRufqHC"
    L"itFhPvHxj/Pt227j2NwY7/nAOYTBCIgiJC0Qbo1Co3NTznPrdBhNEAYUiiHlBNfHByiJiWy6Vyi7eEMpCCnKAonqML33Ns4c7vGx"
    L"f/0bKAVT07OMj49Tq9a5++7v7/zGN77x1/V6/ZQXPpzaAIBckgjgS1/+8leDIChizO9cfPGb1tfqTUrFAp/4+Ce4++67uO4Le/jE"
    L"r2xmaMVKCwLTsusLa+cK/H1pjbF9htoQBCEDpYhOItI1qlHCtiFKSVEElMMQjKReneTAQ1/ng++4hHe84100Wi0WFipMTU9Rrze4"
    L"687vPXLDDTf8Vyf8OWy8/7K3dp/MONUBANmF68ZxXL/m2mu/0Ov12q1263euuOLt29rtrpiZW+TnPvAhnn7qCT73uaf5yNXnc/YV"
    L"q0APYxNAHdBdEIlrGtHWPQTarssbRJQjiXEeQ0aGogwpyQCJpJsYpg8+TOvQQ3zyVz7C6Vu2slitsbBYYWZmlmajxm233XL3jTfe"
    L"+NlutztJxvhbWPCeksKH1wYAILuAnTiOufaLX7yu1W7XWq3mJ6+88qoLgyAKj41Pcv4FF3P65tPZfu99PLF3gg9+9EJGx4qgC5D0"
    L"LAh0D0ziXEFIVAgoBCGlyBZ0ClIQSYk0ECdQXZxkeu89nD5o+OBv/wZChkzPzFGtVZmbnaNSmdc33njjLbfd9u0vYbU+H+6d0sKH"
    L"EzbMndLDd3sUgaEP/8IvXHXlVVf97jve+e4r16xeNdBoNCkWSoytGGLPnl3s3b2Xizet5K0/u4XhlQXQkZ2wp2NQCqM7zB3cxUPR"
    L"v7eE0AiMhnaiadTmGN/7AMXKMd77nnezdduZLFRqtNstFiu2nevI4QONW2+95av33Xfv7VjB53P8p7zw4bUHAMhAUAAGL7/ssnOv"
    L"vvo9v3XlVVe997wLLtzQ6/REHPcYGRkmkPDM0z/i8MFDbFm7mrdcMsppZ66iWBJgIkwcUz2ym4fMr9OJNZ1eh/rccapH9iAqE1x+"
    L"yZu58KKLabU7VGsNenHM/PwC9XqVxx979NDtt9/2ub179zyBFb5P8Z7SPn/peC0CALJerwIwsPa009Z88EMf+peXX/bWj731iivO"
    L"GxsbKzWbbbRWjAwPo1XCkSOHOHz4MEIljAxHrB4qsmKkR6PZ5DneRHthlqQ6x6qRAc4+80xO37qNJNEsVmtoldBqd5ifX6CyOGvu"
    L"vPPOHdu3f/9LCwsL41jhV8ji/NeM8OG1CwAgnf8dASVg+D1XX/2Wyy6//JcvueTSt1944cUbBwYGZLPZQilFsVikVIxI4h6tZpP5"
    L"uTmajToYQ2lggNWr1zA6NkYQFeh0ejSb9g4iiUqYnZ2n3W7xxOM7j9x7744bdu7c+QOsuV/Ehnn5RRxOuVDvx43XMgAgmwwQYnnB"
    L"4IoVK1a95+qr33/hRRd98M1vvuRnzj3n3FVDw8PSrrtjV90OwwApZdrj76d0JUlCEASEUUjci5mbn6fZbPH8c3vmH3zwh/fcd999"
    L"tzQajWmsxlfpN/mnVIbvJx2vdQBANkPA84IyMLR+3bp1V1xxxdUXXnjR+846+5zzzzvvvNNWrVoli8WisCtwJmitUcqka/71ejG1"
    L"eoNqtUKrWWfPnt3TTz/9owd37tz5/cnJyaNYodfI+vjyK3e+JsfrAQCQ/Q63ZgxFHBBWrly5+oLzz7908+Ytl51//vlvPn3zljNW"
    L"rVpVHB4aLhTLZSmAdrtDq93W1cq8Pn78eHX37t17Dx488NSuXbserVarc1hNr2P9fJP+NXtfM/7+ROP1AgA/8tYgIgPCQKFQGD77"
    L"7LPPXzE6evbYypVbhgYHTysUiiNhFA1qrUWjUZ+dmJjYX6vVxl944YW9nU7H+3a/tckEv6zLtb6a4/UGAD88N/BAcD1hlLCAKLvn"
    L"EdZiuAYBYqyAO2QC75ARPLeC5Wtf8H68XgEA2W/zQHA3KaTgtshtfv6YX5Uqdls393xZ7tB1Ko7XMwDyw7uGPBj8/DH/ni9B+6XJ"
    L"/GN+MtnrbvyvAgA/8r9X8uLfb5Zsb4w3xhvjjfE6Hv8/Qz5bFCL1ruQAAAAASUVORK5CYII=";

static const WCHAR kShieldQ48B64[] = L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAR3klEQVR42sWaebBcd5XfP+f3+92lu9++SLYleZdly/YIW+DCMBDL"
    L"DB5MKpmkBr8iYKrGVBB2QYpUFmfGMZGASYAZJwxhAqMZBmqAYbFiFosZMM7EkjFeBhC2kJdY2NqepKe36C293L73/pb8cfuphBG2"
    L"xnYqt+pX/V53V9/zPed79iu8StcDDzxgNm3aZG+66ab45ptvfk8Ux+/QStY450NZuj3tdvPL73rXu74PcPfdd+uJiQn3atxXXukP"
    L"hBBk+/btamJiwn396/e8fmxs8E/7GulGEcF5RQiOOBI6nYL5heZ3JicPfeiDH/zgwR5gB4T/bwBCCFpEHMB3vvPdzSPDA5+p1eJ4"
    L"ekHc8aUBmVtAnLVhsNZm9Vhbzl8zLIcnp48ePnzovZs3b75PRPjGN77xiqwhL1NwtX37dpmYmHCf/exnh9euXfvfhoeHfi/rZMxl"
    L"425yvl9Pz7Zodbrk3RKNpq8Oa89ZsBsuqZn5xU44enTqo+95z7v/ELAhBA14EfkHW8P8Q758991365tuuollrX/v/vt/Z6Bev6te"
    L"Sy5eWOq6tl+jZtuxbnUWcZRYX5KXOTYvyNpCq9UwC82Ov/pSLVdcfsmWe+759lsPHHju34rIowBbtmwxgP/IRz7iXxULLPN7fHxc"
    L"rr/+ehtCpaDv3nffa9acddadURT9bqu5RDs3zifn6tkFy0Kzw2LbsriUs9Qq6LQrK4h3mOCIkwYrRwNXrSvchecO6uPT8/bYsePb"
    L"nnpq7x998pOfPLRs4a1bt6oemPBifnI6C0gIQZ1i0pP8fOihh940Oj7+vtiYdwohmjw67eP6WdTr4/rEUhsRi9YKJWC0ItLq5P/O"
    L"ByyC7yxypEhYaqV68viiv2JtbK56zWUfGB0d/hfr1l32xYMH939BRJ4C/KmU3bp1K6ezzIta4DOf+avRa6/dcNnIyNB19Xr6T7SS"
    L"a7x37D94hNLXXP/wao0Y2llGlnuW2iVLrd5plzSbOa12TqedY4sSnKNiuUUFiJI6o8MSLjnP+XUX1HRfI+HI0anixMLSA4vzi/ce"
    L"Ozbzo3vv/eEv9uy5v/2SFAohCMDOnTsbF1544daRkZE3C/4CE5kxrRTHj89ycPJ4yMrUp33jqlZriFCiFJTW0+xYmm3LUtuy1Cpp"
    L"tgqWWgWtdk67VQEIzhG8Q0KAEMCXKNFESY2xEQkXrhZ/wZpIjw3XcM4yMzNLu5MdLgr7zOLi4h233nrrT7Zs2aJOtYS8MCTu2bPn"
    L"7VdeeeXfzM2d4OjULPNL3dDKxFnqyiQDypgIpSyJCdQSg9FCUXraXUu7Y1lsWRaaBfNLBUvNnHa7oNst8KWF4KtX7wnBgw+EUIEC"
    L"TRQn9PeZsGJU+bNWaMZHjB4eMKxcMcQjjzyy/ZZbbpk4NXT/kg/s3LlTAJKkNgbOPfSTg64jqyMdKQlKmeAtZCWRKkgTjVIaXXpK"
    L"C9Z5sq6j2bZkuYMAjZpGiBECeW5pdx3eWuqxIEJP+ID3ELzgXEnezWg3RWamI/3Mvoi4NhDOHs/sO27Uylq3oieqP60TX3fddZVJ"
    L"RI16Z7VX/cHrRArbxFkHPmBEwAhGC2UhBBfwIZB1LVoJF6+pMzJoqCeKSEMIgcI6Wu2SqZmMH++Z48dPTCPenfwcKjp577E2YK0l"
    L"hLwSajEXI7HudBIlooYAo5SyPeaE00ahsizq3W5Otygp45KitNjCowG0oEVRlp7gq8iW5Y5V4ymvuaRBbATnPc5VAnkfSE0g7hdG"
    L"+lLWX3AOG9c3+NL//AXNliU29H4nVFYhEJwjLyyl9aCg2fLkeYExqh+IQwj2VHnVKRQCIM9zk2UdrLV4FyhyR144Suux1lMUjk5W"
    L"Oer8Us7sfM7ogCHW0MksRVFp0vnKTxENYiidop05Ll87wHsnzieEgrL0+J4/KAJGgdaCVoHgHUVh6eYOay1KKXWqvL82D2R5prrd"
    L"nBDAeU9Reorc4ZXgFOSVqnCuAtTqWFpZgXMG6xxJbEhijVKKEDydrCQEIYoMWiu6heWKS1fy5muOseP+KQYaSU9ZHmcrocvckeUW"
    L"FwzWVoqo5D+TRObBWUsIYF3AOk9ROmwA1XM+5wNF6bGuApDnlhA8aayYnp3j+YNzzC3lDPSlXLNhDUkc080ryysJ5LnnsouH+MSf"
    L"PYmoPhS+CochYEtHkZdY62j0xYydJT3fFDld3voVANZafK9kCM7jetQJruK9c4GidOS5o3SBpVaBtZ5ms8mXvvUzHtt7goV2RFZo"
    L"5pZK1qw4wJ/8/msZ7tNY14s8TnHWigHSFGYXPI1EKuoIaCMYX7FbKQ2iEZEzL+a89zZ4X90IcLYC4V3A2cofOl1L1rWUpadbOL7+"
    L"/QPkrSl+ccQSzApmFkpm5guyQti3d5GvX32UD02sotN1RJFBlKKWxiQRBBQohQiIVEEiRAKiMT3aaa3wPpy2JvoVAM65jg8erQVf"
    L"VBHFuQqAdZ6ydOS5pZOVFIVDKThwJGd6IWV63nL8xDzeBhDBxApVM3QL6GYZeV5xII4MC0sZ882SKI5BAqKkOiGgUSAOrSPiKCKK"
    L"IkSwvfqM5aLytACKopz13iOhFO+qKLEMwDl/EoT3HpHA8bmc549mzC6WVdmnBTGCiFRAGimvWxcxNX0CHTWAwNBAjd17jzDbNIyP"
    L"JkiwiBKUkh7JPSIKbWLiKJDEGuvcIpD/WgvMzMwEgNzmJ6y1GOXwPlSnF9crAA4lgW5ueWp/i8PTOXgQLWCWQ6cgAVxX8e/+5Tmc"
    L"1Zhndl4YHFAorcm6Xb7xN/tIGuPoyKC8qkKoSI9KvnovSqmnLsSxwVm3ADjvvZza+PyKBVqdzlSWdYl0UMF7COBDzwreowWOn8j5"
    L"ydNLLLYqzaF7gkNFAw/ea25/32reesUiUzNdGo067U6XleNDfO07P+ORZxRj42OIK4hE0BIqACHgxBO8IzIxg/2WJI7odLtzp9Rv"
    L"4VcS2ZNPPhkAjs/NTXW73aweI0pcEMD7QAgeLYEDxzKeeK5NtwwYI71CkOVQhwqg0Hz0ttXccMUCUzNtlBK6eUl/I2HnY8/xX786"
    L"ycjKi4giTZLUiOOEOEowUYwxMVrHGJMQJwlDgxK01rTbncOnawFOAti6dWsAeGzn9HFn3VSjplC4IEpB8BA8+45k7J/KMVowp2h9"
    L"WXitFS6D971jFW9e1+ToTEYcG5wXhgbqPPr4JP9p2350/2U06ilJnJAkMXGSEMUJURSjeyeKE9I0ZsVoLEVpOXHixAGA7du3n94H"
    L"RCSEEJSIFJs33/fsSL+5QHwRBI33nn1Huhybs6SxIusKQq+qDD3NK4UtPKvPH+Adb4o4caKkv6+GdYGhwRo/3D3Fn36zSX1kPY1a"
    L"ghYhVoIGFAEJvqKP84DHKEVfX8TZ40Y1Wx2OHJ589nQAfik/79y5U1V+0H00TRSRdANoJudKji9Yol500VpQuvp7+WglUMBvbhhg"
    L"uBFIag3iJGVsdJDHn53n0/e0qI+tp1FPMdqQxDFxFBOdcoypThRFJGmD4QEXVq1MZW5ucfFHP3rw//QGC/7XAliORNNTsw9kWUZf"
    L"0tULHaGVayKjCIBWUvW6y2FPVJUpRUApzltpAI0xCUnawHrNV38wTzxwMfUkwmhDHEXEJiKKDHFcvRoToY1BG0MURaS1GueM5358"
    L"tMHs7PwTBw4cmAwhyAtHL78EYGJiwgP89KeP/XhhoXl41biWEKxv1FO0FpAqyhijiKJTQVSUQmlG+iCNA7VUMzyQ0s09J7J+Go0G"
    L"WmkiExEZQxRVghqjMcagjUZpjdaaOI6o1RVrz7Ugiuf2P39/b7jwktVo6LVsnRtueNu9l14y9oEVjbafTRsqjpuUZZW8jBYiozHG"
    L"43xVciCCMsJDe5rEvkOzbamlKZPTGV5SUmMwRmF0JXClAIUKoWo1nVT+BMRJynB/K6y/KNEHDh4rH/i7+79/aqB50VJi2Un2HXj+"
    L"i6vOHrvtohVBH5ofpFFL6OYO5zxaK+JIERmNtY7ghYAQpYq/fazNjp0ZiEbHJf2NGsNDI2ilMNpgjCaKNNpojIAQCK6KaKICSkGS"
    L"JFywctKft3qV+sHfPfzwww8//ESPPv4lAUxMTLheNPrpt771rV3rzlu1ae9k2y32D+hWp4stFUF74kgTR4HSBrwFRFXdIYbfuGIN"
    L"q8cU3RIOz2gKa4iMwhhT0UXrk/MiQjjZ3aGEOEqpp22uvjRIJ7Py6N8/+kWg7NEnnNFocfv27QJw6NCRPzp39VmbLjm7y2x2Ds1m"
    L"QlFUrWIUaeIkUNiAC4EQhMIKt/2zMX7nGkuWLeG8YqaZ8LUHU6aXUiKjT1aXSqlK+0AQIYigFERJjTWjB/3VV66Rx36y99m//PM/"
    L"v/d0zntaJ36hFT70oQ/ed+DQsR9eeWFNr+xruuGhEWpp5XBaC2msiSNNEmmy3HPthiHefZ1jbm6OdjdQOsNFq1LevQkilSPLmteq"
    L"KkHk1EweiOOUWrTEmzZIUDqVh3700P8A5l9YPrwkgFOsEJ75xb4tZZFz5eolRgYShgYa1GqVQ8axJk01cawIaNatiXDWYZIa/X0N"
    L"hocHiWqDnL9mkKF6RkBV+aMXuRDplSkBJRDFCReMHfSv33iRfvTvdz/96U996ssvpv0XnU6f4gsPnPvlr96z8arLfnf/zIzLyhXa"
    L"OktAETJL6gUfHPU0sP+opa+/j8HSYOIaaa3BivFRDh5ZpNWxNEY0oqoJRCAQvMd6D8GRJg36oilufEMjtDPHjnu//fGe9tULZ0Gn"
    L"Xvqlptc7d+5k27Y/2716zXm3rB7X8bGFBK9q4oPFexCEEBTGCM8fK0kj4Y0bBhkfG2LF2BBF6fn0V/ZytDVGX1+9V/dXDu+cwztH"
    L"pDW1VPPGtc+5f3zD1frbO7533x9+7GN3hkr9L7ozeFEAu3btCpdffrm+/fbb5zZcdXX2G+vX3ujzebfoVipU1XX5UFGi6mEVjzyV"
    L"8/izTQ4fW+Lhx6f53PbneHZmhNHRcURA66p99M7jnUfjSesDnD/4ZLj1nevY99zR5ofv/I/vmZmZORJCkF27doVXtOCYmJjwvaXc"
    L"fx/78ld++5qrrnjbbPuwk+girc0sJjJEzQJjHMY4kiTm6WOwd9IRGc3g0IWMjNUQ5dHGINJrkrxHgiOtDTAUH+Ldbx/2tb5R/dd/"
    L"/ZmP7tmz52c9+r7kouOlKATA+vXr5cEHH/SEsOv8i9a+89Jz08Gpua43fWeL0eXJEsBEVZSp1yKGBhoMDvZRS6scEEW6KviotC/e"
    L"kSZ16qbJTb85667f9Eb9V1/6yo4P33nHv+9R54y2NGe8I1tejd7x4Q/f+PYb3vrdvHTh0clzVB6Py9LSAostS7Nl6XQ83QK8F0D3"
    L"kpZBKQEP3gWwjsQk1Ixj06VPuVt/70b9g//1v/f99lvf8lta60PWWjnTfdkZ78gmJiZcD8T3RgaH/+C33vKPPrmhe8juXYp0umJM"
    L"0nSRet3RyRzdAkorOFfNokJQ1SDYBgKeOK6R6MDGc37uN998vd69+/ETd/z+7beIyKE777xTnan2X9aWctkSn9u27VNvesPr//X+"
    L"Q8ftvuxCzcBqaTUX6XQ9eRkoCigsOEs1dS4DofTEuo4JXS4f/pl/37terw4cnsnu+IP/cPOOHTu+eaa8f6VrVlkurD7/+S9se8O1"
    L"r9s8efS429dcofT4ZVKUBZ0sr0CUUBSBsvDgNEpSaE9x2dDT7p+/7TV6/6Hp7ic+/p/fe88993zt5Qj/SvbEsjyv/Ny2bX98zcaN"
    L"/6bIM56ZCc6NrNdSG6a01VC4LAVnFUU7Qy0+Ezacs+iufd2VZvcTT81++k/uev+OHX/7zZcr/Cvd1EsIAREJd9111+aNG1/7ibNX"
    L"jgwfmJwJ07bPu9pKcaomtlsQ2rNhVM+H9RcM6eGRcXY9+NDuj3/8v3zg5z//+aOvRPhX41mJk3R6//vfv2HTprd89PzzVv/T4cF+"
    L"OlmODQLB019PSGt19j13YOnBB3/4Fx/72Ef+GDj+SoV/VR72eOHTJ7fd9q9u2Pi6q945PDj02igyIwDNZuvwU888vesLn//83ceO"
    L"HdutlMI5Jy/n0YL/Z9eWLVvUsm/0rmHg4t4ZeIH/yKt13/8LPN2wd2FL/UwAAAAASUVORK5CYII=";
static const WCHAR kShieldQ128B64[] = L"iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAABRKElEQVR42u29e7xfVXUv+h1zrtfvtX/7vXd2QhKSEDABQniJIAW0"
    L"Vqy21qPxVnusp7Uej70993za23t7q9dCTrX3VFu9t6231fYIAj4gInARRRAhPMorPBJICHknO/v9/L1/v7XWnPP+Medca+2ArSDB"
    L"4On28zObnf3KGmOO8R3f8R1jEn5B35RStG3bNvbBD35Q2I9973vfG26325cBWCOl5AAIAIQQynXdSSnlUwsLC3s/8YlPRABwzTXX"
    L"sGuvvRZEJH9RnxP9Iv6jbrnlFm4N//3vf7+r3W6/j4g+IKW8iHM+VCqVwBgDkf7nCyHQbDbRbrfnhRAvArgziqI7PvKRj+yx32/L"
    L"li2SiNS/OcAp/u+55ZZb2Ac/+EFxyy23FD3P+7hS6g+KxeIazjkAQEoJADGAxAGUUgDAOeckhEAURahWq3NxHN8phPi/f/u3f3vn"
    L"iY71bw5wCoZ8Y1R12223/SoRfb5YLG40Rhbmc5jjuPB9n4gISikQEaI4QqfdgZRCMcYUY0xJKXkURahUKvUoiv42juPP/87v/M7i"
    L"Nddcw7Zu3Sr/zQFO0ZB/xx13fNbzvD/1fZ8ppWKlFHNdl+ULBXiui067hTBsgUAAAVIKuK6HfL4EYhy1Wh31eg0AlOu6SgjBOp0O"
    L"FhYWHg/D8I9+93d/958NNlC/CCmBflGMf8sttwzkcrmvlUql90RRpDjnkjHGu7t7oJTA1OQEZmcXoFgBHZEHcR8AQ6PRBGQDPlXR"
    L"XQ6watUKdPcOoFqpYWFhHo7jKMaYiqKIzc7OTrRarU9/7GMfu86CxDd6NHjDOkAW5d96660XBUFwfalU2hDHsVBK8lKpC/l8HocP"
    L"HcD0TA357pXId69BrIpoRw7qzRBRLCElgTMFKZoQzTG0KvvRnW9j07lvwtDwcoyNjaHVasHzPBnHMZufn4+azeYXGo3G//UHf/AH"
    L"9Tc6LqA38qkHgNtuu+0Tnuf9dbFYLEgphVKKDw4OoVGvYtdzu1EeOAP9I5uw2HQxNVtDpdJEs9lBHAuIWEIB4IzBD3wUCnkUcj7i"
    L"1iRmjj+CNcsdXHXllVio1DA1OQnP85RSiiqVCiqVyvdbrdaf/P7v//7zb+SUQG+0U3/ttdfyrVu3xt/4xjd6yuXyX+Xz+d81gE4y"
    L"xtny5SM4cvggDh2dwvpzfhkh+nH0+BwWqy10OhHCSEAIASEUYiEhYgElNRhkjMH3XZS7u1AuFTAztgOs9Rz+3XvfCcfN4fDhQ3Ac"
    L"RxEROp0OTU1NHWi1Wp/+5Cc/ecsbtUqgN+ipv9zzvK92dXWdJYSQUgoKcnkaHBjAM08/hU7s4Yxz34ljUx1MTVXQDgXCToRQxIhj"
    L"qQ2fcQAZSygpoaQCYwyu4yKX8zA4NIROcwILx+7Ge66+GKtWr8f+/fsQxzE8z5NhGLLZ2dl2rVb78vz8/Of+9E//dOGNFg3ojXDq"
    L"ba6///77nVar9SnO+adyuZyvlIrjOHZ6+/rguS4ee/RR9A6dgeVr34L9hxdRrTURRjHakUAUCcRSIYoF4lgbP4p0GhBxDCkFSCgA"
    L"EgQC5w58z0XfwACCQGJi/124+Nx+XP5LV+LIkWOoVBYRBIGM45jVajXMzc090Wq1/uQ//+f//MAbCSDyU/3Un3322XLbtm3qjjvu"
    L"uEQp9a2urq7fZow5UkoJEF++YjkWF+bx5BM7cOa5l6N32SbsPzyHZquNWAjEUkIqBWFesVSIhUIcSwghEAsFISWEVJAKABSUVJAi"
    L"RhzFaDVbALkYXnkB9h84iqP7n8T5529CsdiF+fl54pyrXC6nisXiiiiK3vuOd7yj661vfevOP/uzP2saboJt375d/VsEeIWG37Jl"
    L"iyIi+d3vfrcvl8v9H47j/GGpVOJhGIo4jlmxWKSenh7s2rUTC5UWLrr0V1Hr5DE6No8witEJBcJYIhL61IexRCcSCEOJKJKI4xhx"
    L"JDUYFBJCSqhYggggKDAlQUqBAXAcD8WuAgaGhtFY3Ie48hh+7V2XY3jZShw6fAhRGCZVQqVSwfz8/JO1Wm3rH/3RH90FAFu2bOG3"
    L"3HLLKUkln1IOcM0117CNGzeSzfV33XXX+3O53J/39PS8KYoiCP3G+/r60G638OSTOzA4vAZnbfoljE+3MDvXQBxLdOIIkTF+GEuE"
    L"kX0JdCKBOBSIY4EwkpCxQGxAoRQSSikwpcAIICVBEGBg4NxBkAswtGwEnDUwe+ReXHBOPy677HJMz8xhZmYarusqxpgKw5BNTk62"
    L"arXajZVK5Uuf/vSn92Yc+5RyBDpVDH/ttdcSEVnDX5bL5T7d1dX1Lt/3EUVRHEURD4KAisUiXtizB8fHJrH5oqvQM7gGR44voNkK"
    L"EUX6pMexQBgrhEIiDPXJj4wDhJHQzhFLiEhoUBgLSKFTghQKBAUCwKDAYHEBwInB83309PWhp7eMmdFH0OON4eqrr0SpqxdHjhxB"
    L"FIXwfV9KKVm9XsfU1NTxRqPx981m8yuf+tSn5k41R6BTINQnD+K73/3um8rl8qcKhcKHent7eafTkXEcg4ixcrkLi4uLeGrHDpR7"
    L"l+GCi9+GesfF1MyiNm4sEoAXCnPiYx3uO9booUTHhP3YOEEcC8SRrg6U0CUiFABFIEhwBpBSIOiUQERwXQ+FrhIGBocQdaZQm3gQ"
    L"bz7/NFx08SWYm1vA9PQ0OGfKcRwVRRFbWFjA3Nzcs41G42/Hxsa+9aUvfal1qqSG190BlFL0wAMP8Kuuuiq2H7vjjjs2FovF3y8W"
    L"ix8dHBwsRFGEOI6FlJIXCgUopfDsM09jemYe5198BZadth4TU3XUm21IofSpFhKxUIhiiU6sT759PwoFolhpJ4kk4sgYPxKIhTQO"
    L"oCsCKXVJqJ1AgZGNBtBOAAmYKiEIAvQODKBQ8LAw8TjK/gSuuuLNGBpegbGxcdTrNUslo9Pp0NzcHBYWFp6o1WpfHh8fv/1v//Zv"
    L"qzYCAsDPo2qg1zO3W2Bnvf/jH//4L/u+/x+CIHj/yMiIa1qxIo5jlsvlyHVd7N+/Dy/s2YsVq9fjvAveinbEMTtfgxBSn2IhIcyf"
    L"OvwDHZPzLfgLQ/25YayNHxrDRzYaRAIi1h+TQvMCIAUoBShtfEb6fZ0ehHYKxuG4LgpdXegfGICK5lCdegzrVwW49LK3gDkexsfG"
    L"0Ol0bFqgKIpodnYW8/PzzzQajeur1eqtf/EXfzGWjYq7d+9WW7duVdBu+IZzALrmmmto48aNyffOsmN33nnn8lwu9758Pv/RUql0"
    L"YX9/P+I4RhiGQkrJcrkceZ6H0WPH8PQzz8APunDxW65CqWcAc3MNtNsRFHQdH0t96oVQOtTHElGsTEqQCSYII4k41tEhjPTHdQow"
    L"gFAIXREIEwWE/nVJSQAKNkCTfZFOB1AKDATmcPiBj+7efnSVS6jO7QG19+Ci807HxnM2od0OMTU1iTiO4bquVEpRGIa0sLCAhYWF"
    L"0Var9d1ms3nLZz7zmX9OHiIRbr75Zr5t2zZs2LDhpDkE/azh/Nprr6Vrr70W27Zto4GBAbryyivFiTntzjvvXO44zgVdXV0fLRQK"
    L"lw8NDQ24rgsb6oUQzPcDcl0Hx4+PYufOXQgjhU3nvwWnrVqPaj1EvdHSP9PW9VIhksb4xugW3KWGNu/HuhSMkshgTn+kcYNI0oDU"
    L"YFBKQCgoJUFQ5rFrg5N5aqQkiJRxBIAYg+M4yBUK6O0fhOdK1Od3osDGceHm9Vi3/kw0Wx3MTE+j0+nA8zxJRBRFETWbTczMzDQb"
    L"jcbOdrv9nXq9ft9nP/vZnS+HmQBg9+7dyqSMn9kp6LXK49m3H/zgB8vCMFzV3d19YRAEVwdBcP7Q0NCyIAi0AEOjegaAfN8npRRG"
    L"R49h167n0GzH2HD2BVizbiPaEVCtNnQoJp2TFRS0fQyhI8ypj63B0/fD7J+xzHyOiQIWC8SxSQW6LBTCYABI/TOlApH+0zwBMP0L"
    L"aUxgogERwDjXILFUQk/fABjqqM08i558BedvPgNr1qxDJxKYmZ5Bu91CRoDCwjBEs9nE3Nxcs9lsPh2G4UP1ev1+xtjBBx544Nj2"
    L"7dvjf4kif10cQClF9oR/5Stfcfv7+4eEEF0rVqy4nHP+JsbY+kKhcE4QBCu6u7tRKBQgpUQYhkI3YQRzXZe446DRqOPg/v3Yt/8A"
    L"hHKw9oxzcPq6jYgkQ7Vah4glOIchZ/SfCoCUJgIIlQC/eImBFcJIpdEgY3iLDeJImNSwFAcIAyYhrWGNEyhz2qESfwS00QGAQXMI"
    L"ULaxxOH5PkpdZZR7eyDjClqVvSgH8zhnwyqsXbsO3PGwsFhBtbKIOI4V51wxxpQQgkspUa/XUavVUK/Xp8MwfCGO4wNCiN3NZvPZ"
    L"MAwP/+Vf/uVRbZLUJifVAewPevjhh0uc8/+lUCi8J5fLncsYy5fLZZTLZTiOAwPmFAAphICUkhzHYZxzdDodTE6M48UXX8Tx8Snk"
    L"8j1Yd+a5GF6xFlGsT7wQAowBjAGcAGIExlINnzTUbSQUokjpCiCW+n0b8qMT0kEsEEUqwQaJs0S6IRTFGQeIRQIArQNASShTGRDS"
    L"SABFSYRiUFAQGjOYj3HuGEfoQalcBlQDrep+5NkU1qzqwfr1a9DbN4AwjLG4WEG9XocQsWKMSSIipRRJKUlKiSiKUK/XUa/XEYZh"
    L"M4qix+r1+lc/85nP3PxqneCVOAAppfDAAw+Ue3p6vrNp06a3W2MY55BSSiWlBBERQEwLMRVarRYmJ8Zw5OgxHBsdQxQT+gdXYvXa"
    L"DSiWB9DsxKhWG5BSghtjc6bAiOBwAkscgIFI6QaOsqdfGfRvHSAD9l4WD5iyMFMFWFBoHUDEIs370p7sTEpQKR5I/848TSXNQ00d"
    L"hxED4w5c30ehWEKp3A2HC0StMajOGPq7Y6xZPYTTThtBsVSGlECj0US9Xken04YQQpljroiIAaA4jsEYw9jYWP3FF1+85POf//zu"
    L"V9OAcl4BacOISDz88MPvXL169dsBRGEYMgCMc06cc8Y5RywEWs0GatUKJiYncfz4GGZmFxAJju6+ZViz4Qr09A2DmI9qtYXZ4wtQ"
    L"kHAYmXAqASKQ0v8tE+9DEn4VyBgCUBalq3/Fs+knnwHKfpLNN+Z7qn/hmxg1MfSvo0ASUIqgSIHM91VgEFJCqhBxHKHTaqG6uIBc"
    L"voBi1xBy3auwGNfw2HPj2PHcc+gpSZw20o3h4QH09/XC94cgpKIoiqnZbKHZbKDVbIJzDtd1w+Hh4eLk5ORHAfzv2crrNXcA+yaE"
    L"WGPEkgSASykxPT2F8fExLC4uol5rYG6hhkgy5IIiij0jWH/uRciXegHmo9HqYGy6hTiqaUKFSTAGxNCnXClo4ysFUgRmQJhklJRj"
    L"urunsYCSia2goJG7UioN0tlonURtSiyrjWgLPGWAJqUeRSbEZ1yBMu/YvyLz8+1fKpVNFASSOhpIIRDHEcJ2G/VqBa4XoFAoIF88"
    L"HfDOwELUxMy+edDecRSDw8gHCoMDJfT1llDIBVixYjmIOA4dOoQwDB3f91U+n78UgLd79+74pDsAY6zsOA4BgOM4ePbZZ7H9ocfR"
    L"078KzCuhUFqOM1YNwA26wLmPSEg0WhHmp1uIopomUsgQK6SgJIEpTbc6YCAChJQgMIAkSOpTqaBZOQWCEqa9a6oB7Qi6OpCK9PtK"
    L"v5R5SZj/hvlvBfN3pN+H/VjGWWyZl4kIONH4SCNT4nDm59ivIdJRIPElKSGgHSEKO2g16+Dzc/A8D34ujyDXC89fhiYBrU6EudEm"
    L"1OEqwmgOrtyJ//BbV6O/fwDHjh2F53mklOoHUN66detc6smvsQNs2bLFTk/kOOeJpn5+bhbL17wFK884D23DuNU7HYTNEGHUhDQl"
    L"FFH6ZMmWTAQokpBEcADEJExIZpCkQ6rQRTdY0pKRpgqA7uGbiiA270upncMaPlPCQ5nPlcZBtAZAwsYLYgSSSEtORSYiWetm0usJ"
    L"DpEGCmXSkklRSjsmoH8uMikMBN12FjHiOEQUttFq1sGYozGD58HxAgS5IvKFAfT29+H5p27BkSOHcdrKNbpvod/8wcHB0vT09LzB"
    L"CiclAihz6vts6BRCAMQQw8fETIhO2IQ07BmgBZfEJJSS+iGY40VkmFYAiikwRhCKwKRm2iWZzyECpOZilTAP0Z5eCS3rMmSQEOmJ"
    L"l0ohjhU6HYl2R6ATawmY/j2U5vuVNFFDIQwlWu0I7U4MKAWHAw4j3RImGAMaKyeEkEqiABESjoBMelFZh6E0Stlnp7UiMsU4ZIgl"
    L"EmAUAeBotzsgaoK7dTheDp1IwvcCRFEMxhlMhQUi8oeGhgrT09M4aSnAlhic8yHzgxmgEIYRJDyApEmlBiWbuljKFDUnD48IOqjT"
    L"EgBNZE+NzqFS2u9H0Oc0Nb5UCiI2xreGjCTaHW1kxoChPh/FvAPPZSjmOEoFF56jnQw2BQCoN2LMLnYwv9jG/GIHR45XMT5VR7Md"
    L"QwmFXOAkjKA2LKVhXyGDH5bGXkWZPJFEBmkiUTaCWOfROgQkw0wSRBLMVCz1ahOKPNTrdVuNkBACnPPcsmXLSs8995y69tprT0oK"
    L"IADqmmuucYioWwhhvFii1eqAci6kFEk4lbAnTBpjyhTOK5sSAEUKkBotM1JQwkQABpA0zmCcyoIsKUyuN6FeCqVFn5FCPsexfCDA"
    L"yECA7pKLvM/hOrqBI5Uyyp/U7Wya4AM+OAvAnV5ISWh1JGqNGLv3L+DRZ6awZ/8swo5EIXBMJlA286d8AS1FmmQcNrGvPvDGgfXn"
    L"SZGmquTTDNesoEDEwZgCExLEdDkNwdFoNJLyW0oJznmuXC53ZX7SSQOBDoAu7QAAlIACgTm+KXUklBKQkJBKaDJNGk49AUQa3RMU"
    L"iBGUQf5S6ipAKkAKAFKBmILI+rNKDa8kdFdPSvSUfKxdkcdQr4+cTwmZE4YxWm0bgXRyV1l8YNKFlMI4ln6ojkNwHYZLzyvj8gv6"
    L"cXC0jrt+fARP7pwEwOF7DFKkqUAZpyBrumxlYLuIisBIQahMxUFp1SJlWtYqqTTZzCUYOIgrcC4RhiEc7qHZWkwOphBCBUHgl0ql"
    L"PgBq48aN7DV3AAv4isUil1I6cRzD+mmtVodX1GWblMpkfgPUhD39lk/XHs6h0lyZoHFtfFKAkBpBkNQ5Nf17m+eBKJQol1y8aU0X"
    L"hvs8MCXRiWLUm9JiOF212B+KlEXMOgIHoBSD5BYjWHIoQqXWBiOFgXIOn/yt9bj0/EF88/a9GJ9qoZjzIGxzyHARyp56lTn59uhT"
    L"egDACBBqyfNVkOYgII2msZ5a4pIgBUcUxYACOu12ykEAijFGANxXQ+2/ogggpWRKqaIQAgogToaIIQYpZZqnDdoWUj9wkraBAiOr"
    L"MOBK6QcnlamT7SknHR1IGSiZaQIppRDGCgPdHi47rw8ek2i12waMkTZ4Jq+mXIChZRjLPDw7IWxDry5DHQeQkoEx/e+q1lpYrDSw"
    L"fmUef/KfzsMNt+7FE8/MoFjMGYeywDZjeKVOgM8qoZ0UZGIpynyOdUohpClJCYrrJ8ckEIUCnJH59ybfXxYKBcY5zwNQu3fvPnkp"
    L"IAgC0ickhgLAHG5KG5VQpMqWV0IbXwoYOlQ/bNvVUVJBMfMnAGHwkiITHYQFVfb06zk+pTTYWz1SgMclas0OHMY0eMrU4z+hiEmR"
    L"etLM0aUeY/r0aaMDjEmjAwI8z4GUCovVJlyX4/d+8yx4rsL2x6dQLuWTlCWRpoHU6CYtK/mS38emD0s+ZjkFZQgpYdrfnOn3hYmC"
    L"JwB0KKVelcT/FTmA67rcqFrAGEOkJGq1GvKmWkvrbROuDehKw5zm+IUyz1ZqkMdNKSVJJSfYEikqSS0wci0gFtKUZ+nnSwm8XPmb"
    L"Pit6GUdAEjlsOWf7KToCaCewDuF5LuJYbxP59+9bj2qlgaf3VFAuFnU0S2DfCQZS2jWULYORKXMhE1BKGZCsD4Z5QMQgkgjIEIVx"
    L"gleWUNIn2wFarRZJKR1bBRAxcO7qE30CuBIZ0gUZ4gMmBcKedmZZPkpwhf1HyWxYNMgfQNL+tQZ+Ce3705W15vOVYTjJADECEctE"
    L"BJVQu4CE65IeKIlifPg3zsLo+KOYrzaRD3yT0lIamijLQiFtJGVOOzMF31JmSTuJFLp4Iq7AzPMEODqdUANr88w459YhaGJi4hWl"
    L"gFeEGLu6uqCUIiugSAwgU4rVGk4KS9BYkYUpw0SKD2KhO3tWrmW7dWEkja5PoB1KdDqptr8Tam2/tF06vNLXTz41RGRezPT07Uvj"
    L"Ac651gE6DoQE+nrzeN/VZ6DTquh/p5kukkl1IRP2UagEIb2EolYZ57DdRQ0IpXb82E4rSRP+acnv77quZQVPLghst9skhKAoisC5"
    L"hFJO6rPS/tImfKmUmlXK1PmKUqLILmgyBEgaoHV/wLJ2mq7VEcRWGbFV7xpSRZeS6l887f8KxZF2BinFBlIyAxxlQk5pgovBdQmd"
    L"UOCi81ZgwyMH8Pz+RXSXuw0zl4lktu9hrWOxEAiStLyUDJAmoecQ0rRmwj5lkoR6KctjHEed9BTgeR7pByMToJKGR5Up1+ysnQlb"
    L"tjIiBUWkwx5J0zJdSlzZMk9mOHvrRNKcjiiWWrYF/bMgs+GcXhYAWkyRdYY0D6f/nUmtJi3Y6kD/Hjrc6pIRAHI5D7/0ltXYtfdJ"
    L"SFHQQV2qhP4WQkFJCWFAcYLwk96FNB9XCRchpUyeA+O6KnE9DiF/Msg1OoGT6wCO45A1upQi8WRb9UjzSg1oSB0rniAkUzd2yEI7"
    L"lMpQq2mrV7N3maaK+SFWCmZHuiVZGtmeUCTlHWMMjsPhcKZLywxXr4UhsSlM2MsCyKyDcG7BYlqSttoR1p3ejygKsXv/LJhTMA5q"
    L"nE6mZahUGTyAbJSUyWiaEFqYCigwzuB5hFxAKOQUnDyQlxoUnhjdHMfhAGjZsmXqpDnA0pDz0o9lq4AloFBa1owyJU8qpETiOCaH"
    L"2q6ekMmDtqdfQiGOZMKaSSXBQObzdFpgjMFzHSil0Gx1MLdQx8JiHZV6G612BM4ZeruLWH1aP4YHygZfxOCMTjhVqSNkT6sli+yJ"
    L"zudc9PUW8PhzCyiVfQ1qTQRhTAMtzrQTMJat+ylTEUgIIzWVmQ1mRJSBxub4qFdE97+mZSBJKeml6camAu3u0oo4ZJoWIHXvi5Y2"
    L"1ZKTLy0fIG0ozHT4pGUASaeXOBMVhIIiZThxQuC7qDfbeHb3YTzz3FG8cGASU3NtNFoCoSAIyaCIwXUY+rtzeOsFK/Gh956P/p4i"
    L"6o12QhRZkGmNvISzT2p6baBczsPmswfxw4cmwUgCxMCswCijHSCWxWmU9EPIFEiMmSpEmbkEYqYiYYABpj8Jz0gpTz4GqNfrUErJ"
    L"VEWTPSZIJNTWmFkgiIT1S8tCmanflvTpzcy+FLbnjwTxK6MFJFJ6oCOOQQTkcj6arTZ+/PDzuO+Rvdh3eAHt2IUblOAFQ/C6fHjM"
    L"1Q+UNAnRjCJsu3cCDz79Q3z699+Ks9f1Yn6hvoQttM2ZNNyqhDHURRoDZxynLe+G60hEIobj+IYUMiRTkv4oAwZNGcgoSx+BmblE"
    L"qQiMWKKEJktXEkueeVq1pHzixMQEO2kO0N/fr8zuvVQOTdCBi1I8AFsaJs0gZQxuRX4yqbkBDZCErRyERCxhciFS0YaCIUaAMNaP"
    L"13cdtNwYxXyAJ545gBtvfQQvHFwA98sollejKyhBkZeETTJNgjCSqDVjLNQ5Ko0ynt5fwd4jD+Puf3on8i6h1Y7hOGxJlZA+ZMpw"
    L"Byx5f6C3BN9T6IgYcIIsiDCukIqIMy5lGwTmpEtDl3AwqYdNGLeOoEtS/Xk8dSbK9jhOcgSo1Wro7e1NTwcI7XYLSglYUXRibJkh"
    L"c2SW686UiyZiCJPvVQbhCwVd9wrDAioN+vSCJoEfPjyK0wbWoNWJceOt9+F79z6PjsyhZ+gMOH5qeDIAI4wEKo0Is5UI89UY9bb+"
    L"3gDgFgo4fKSBL974Iv76jzahPjYHxp2katCvly8rGdNhIgg8cCahYpHpMTDYlE3pXNmSFEAq3UxCkgzxJE0fiUCMg3Fm0gEHQUHE"
    L"kdGvkh20waudC3ilvYDYDCJASgnHceAaehSMJ4aWhnSRGVlUEhWsIyRyLnPapUQsoPX5UpoKwqBjWHmWLq8YAU/unseLByfRqk3h"
    L"6PEZFMsj6Cv1g1igDa8AIQWq9QgzixFmqzGqTWGMTiAGMJ7W25T38MNHZ/GZmgDnQBzFYIzrGp6xTEMJSyJC4iAZutvmb9hmWXLe"
    L"M+/ZkECU6A5tRNF4wXADBmeQOf1ShsjnfRBYhsZWiOP45DuAlFJIKZspt6+Sdqv9py81esoMYgnLlYkExpnsmFdsJnMS4yfRxAxm"
    L"mDKPiGG+zhHHvRhY3g+QAzAOAtDuxJirhpicjzBXjaGfTcbo6oSGnVEKN0OFxXoMnzTOcAAoYmAm15/IG2QjQieMtdiEURJ5NFpg"
    L"CVZKIpI93aCUj5VmEQVRUuURSLdcTZeTmUjkel6qY6SEqaST7gCVSkUODQ1FJgUoIiPukgLkuEiVXzb/ZxwhUxEs6e2LpWlBSgVp"
    L"VrnJDIgUEkvIJo0lGRyvYEQYCo1mjKm5EBMLIWpNU4AzAjP1+xLl7tJujcEvzNC2MURsPsJUwpgzIk1kGZBo+wmcEaZmaxo75J2E"
    L"uWNG30VmTsBAffO+SvFF4gQs8/P09+BEYMQB4iCTCmxqMjQ1RVEEACEAdVJ4AOv1g4ODoVKqYeYDIIVEsVRIu2CW8JEZnb4BcSlh"
    L"tJTYsXJtXV9n2T9bAciEYLLjWTaiMEaAEqjUY4zPdTA1H6HVkclpR6ZTiJ/UMDIhXCkB3+XI+4RmNYKQBCK+tPwzYA2QJlynBdC+"
    L"g9MIBcFz3AS9p/MDzIR0WlI+EpLyAIlkyiyl0Aoj/T/GdeTS4E+hUCgYOEEgImq32wDQPum9gN27d8eXXHLJ4pJmipTotJvwi7lM"
    L"7W+oTkMLWy28ysizbZWQGl2mf6rsf6dA0joZM6epUgtxfLqDyYUIUaT7wS8b4l/eq9OKnACKFc5YVULBl5hutOF5riGVODjX+ZZz"
    L"ZoZXsmUiod2O8OwLYwAFYMzVp9UU9ynuYxn62WoA0oqCzESRVVRpGhMgzsDAwYiBEwch0i11s9SSiOwdB+HJdAA7gSrvuuuuGUPf"
    L"KgWgkA8wG7URgCUnMwnVMrVE0hY+cTAjcQT739BpwaYMgUSISUxHmGojwrGpFibnY234E8L8v9YWOTGPkxGovO+qEdSqC6g3Wigo"
    L"aArZUeYxiZfoDYgIge/hyPE57Nk/Cy/oBnE/Od1k8zhL7JngiGTPgBElKJMWyAorzCwkI0MCMQ7OHUA2kct5mgHVo+XUaDTCVqtV"
    L"SfVnJyECWLmxlHKKKFXdMQaEnYahuTJNG8q0OGXCFaaVgC0ZrccnzqDSxq1KVHbgHGi0YhybamNsJkIYGsM7P73hU2ou/W/OCXEz"
    L"xvoNfbj6zWUcPfgiGGOI42jp/CABQiBB7TaVuQ7Dw08cxMRMB6W+MsAcg+pTGpCsZD4b+k90BMt0ZtaRMOJmSpqSAVOoNgLfTfYb"
    L"c84Rx3Fjenq6kspsToID2MFDIURDC0J0qRYEPsLZmsnHmZZlwgZmuobS6vvSDl2CBRJjp310LYXSO3+OTbdxdKqDRkvoUP+KDL80"
    L"5C/J/UI3Xb7wX85Bff446s028jkfcSaFZHUCTEoN8pQ2TK3Zxg/u3wviBTCnACIOy+0mUI54mrNZdtVMtipgiWPZlGD5B8YcnYoc"
    L"Fyyqo1gchhBpBIjjuLJ3795ZAHzr1q3RSW0GSSmrFrApJREEHkTU1g/TzgNk1b6W1sjO3JmoAIVEHAGlMl+rDE0KzFVC7BttYa6q"
    L"lcjcIaOcfWXqnxOjADNt6agZ40vXXIwzhxax8/lx5PMBtOpZpxvB0i6i1guaDqYCynkf92x/AY/vnECxvBrk+Pqkmg0iutGjlcdk"
    L"fmaWD0ocQKU6IGJGYawoOf3EGMgIUtCqw/d9s0OB2Yokmp+fb5/0MtA8zMkwDAEzo54LAji8AzvTpyDS0kYZfX+m24ekvy9TNjCF"
    L"DpBSgjFCFAkcHG/i8HgHkdCllko4gZ/6l33JqbflHAGIqiG2/m8X4n2XAA8/9iJygTY+544xkwAo0qGWGCQXkJKBSMJxOBarTfzT"
    L"tx9HTEUU8r0g5puy3i6b1GJVu3SSmOYFrEDExuulo2WptJTMzWaMtPE553A9Qi4XwEwEIY5jdDqdCEBrxYoVOH78+CsqA1+xiEAp"
    L"Nd5qtZRSinc6ocrnc1Cirbdt6/ZWkuPt/9uOXvrRrBYaabcPOuRXaiGe2VfHwbEOhNRbQn7qUP+yxk9039qRpELcivEXn7oYH/1l"
    L"Fw/987PwXHfJ7KCUItP+1Tp9aRjLKJbI+Q5uvvNZPP78LEq9IwArJhWDPpkcnDGdvzkDM3IyTeyYDh/j+nPt3xMzRtcvzjT4Y4zB"
    L"dV1wLuE5Evl8YYkDCCGqAMK1a9eqkxYB7GYqpdR4GIatfD6f74QdFPIFyLABJbRSmGXUrQrIzAVmUL6SS8K9UulEzehkGy8eb6HR"
    L"1u1dEatX3vnOGj8T/h3OEHW0tv7v/+sluHpzjAcfeVqXfKahkpZpClIwSGaZSqnbtZFAV8nHzr3j+O+3PI1ccQiO3wfiXrpUUtnl"
    L"knoaiME08YwgljJMIenhB/OXDEyZaSno02+dgDs+GMUIfIFCoYj2fA2MMbRaLTSbzQkAYmxs7BU/qlccAcIwrFg6OAoj5AsFSNmB"
    L"FJEWVFB2VC5tG7/kN5NplcAIiCKBvcea2DvaQRQDDnuFJ/4nnHxbinHOELVidBc9bPvS5Xj72S088PAz8DzXAFSR0NtZCZrlI6wc"
    L"nTsMi7UW/vIrD2Gu7iJfXg7mFMzJ5+CMJ6edcd0qZoylXTzGDRjUH+fETY7nBhwwgJwkioBpCth1XECGyAdAvlCEEAKO46gwDLG4"
    L"uDiaiItPlgNce+21CgCiKJoRQswRkd5s5Tgo5LhxAGbAD5I8ltLCaRrIYEJwBtSbEXYfaWFiLgLnZm+AOnF05qcDe0tOvqmxGCfE"
    L"rRgjQwV8/+/firXlCTzy2PPIBcFLBkSUFYIkwhaNX/SCBz0j8N/+4UE8uXsBXX2rQG43OHfBmGPytN4sbnO2De/cOIQN+WQcBsR0"
    L"uWcchCXSNZ0OXO6AcQ7H8wHVRjHvgIjbsXCYbWLjAITneScvBeiFVYqIqHHbbbcdBnCmlEIBDKWCh06jglx5SAMdyjZZ0uHMRAFk"
    L"/o6TwsRiiH3H26g1JRxOEEIttd+rQfqUdt8YI4hQoL83j1u/+Gagvg879o2hWMgZZY9KOoKWu7fLwcDS9+33+psbHsMPHjqO7oE1"
    L"4MEAmONr+RfSysIifWYVzwktkKp/KDkftleiKwAoBW4eou0IOo4H3w8ANYneniKiKLJDpdRqtVCpVCYAyFwud3JTgCGDEMfxmGlB"
    L"QimFYiFAoz4Pzp00x5FK4oCipcsbyEy/TMyGODDeQRgrOAxLqFK8KuNTkgJs80VJBU4c1332YvjRUbywfwylojH+CWofmfAYlLZz"
    L"TYfPcRj+4VtP4js/PIRy/2rw3DC4mwNnDA5PUbp+OXAcDu5kPmaYPMbTkG/TgiZ8MgCQ649xcnT55+jLrFS0iGXDfVZvoIiIzc/P"
    L"R51OZw6AWrNmjTypDpAhg/bZ6aA4jtHd3YU4rBvhQqq8pSUymOzglMLodBuHpkJIpTteliX7yazdT2P8jNKG9HVwohHj2j84B2cv"
    L"r2LXnqMoFfOZCEOp5Ctp9ereO4zQlDsOCMDf3fQUtv3wCEr9q8Hzy8DdgjYS52AOB3ecJMynlQBLjKvf1zk/wQVcGx0WI5jqgExP"
    L"g3EOxziT5zlQqo7BwQFEUQzOuSIi1Gq16fHx8RkAbMOGDScvBWTf4jjeE4YhOOfUbLbQ39sD0dln9gFlxU4p7ZuVQx+d6mB8NtZy"
    L"MtLbP9LGiWXAVLodVL08rUuEE4yfQfyMIWxEuPCiIXzs1/rx8EOPopDPZb6G4yVbn5LeOocEkPddNFsxvrrtWTzw1BzKfafDyQ+D"
    L"O3lzcnVziMP06u1NI0YqRypJKGnYNz0RUhl6nKlkCjq7S0BXihy+H4C7hMBtoljIo6nXxCkAaDabk3v27JlYtmwZM7uDT74DKKXG"
    L"6/V6s7u7O99oNFRvby8p2YEQcaJiUTD6P6WBitWLH50OMTEf61NmVaEslboTnZj/KVkd85KBnp8QLIilsuo//u31mB4/BiEVAt+B"
    L"EDLdOWB+OJnFTYw0+aLAUMz7mJxr4W9u2oU9R9roHlgLJzcI5uTAGWndHiNwgnaELOEDSp3AOoAFPqYcVKSngJTRSTKSRt2sHwI3"
    L"WIpzDs8PQDJEVy7CwMAAxidm4TgOOp0O2u32HIDqyMgIJiYmTq4DbNmyRQKgZrO5N5fLHXRd95ww7CjPH6C8F6HTqMBxCwB1EmpT"
    L"JoMkEqMzHYzPC3Bje4XMSc90zSxASlW4/zLgoxOiACNC1Ixx/nmDeOtGD08/PYN8PkjKUgKS30vv89NpinEGpQjFvI/jUw18/mtP"
    L"4dgMR/fgGSC3D8wJ0nlBI8jgxIxSRzsDnTj/YKaGKVkdgyW76JRSugFkliQyZr6eaQ7AcVwEuQBSVNDX7aBQ7EIsJpELAnvvwH4A"
    L"cmBg4OSWgbYSuOaaa+gTn/hEUyl1RO+vjRXnLgb7CqjX5sC5q/8BaikNO7EQY3JRkzvA0oaIljxl9XUGD2QAYfZzl4T+JRpbSouA"
    L"WOJDv7IcrfoCFBhcx4FjXow7BqClH3Nc3XApWOP/96dxfNZFuX8tyBsAdwNwTkm9r2t+J+XpTY5nphS0+dy+Tyan2/JOcwIaB/Dk"
    L"6zVPwDN8guNy5PJ5xJ0prFxeQhjGtmFE9XodBw4ceByAnJmZkSfdAUwlAABot9sP2XKk3e5gaGgAjcpYQmPqIUhtkrlqjMlF3cWz"
    L"MwPZUs+uY2PGEazhmY0MRC/P8GW19hmRphBArieHizcWsVipwfPcBI07Dgd3ORzu6nrdSd8PAh9zlRhf+NrTOD7noNS/BuT3gjte"
    L"AuI0l58azeHMNGzIGNaSPqaFa5A+J26iRpYQ0i8wDmXQvyWGuKNH2nzfg+85kJ3jWHP6SnQ6oY5gUtLi4mJncXFxHIB4NRXAq3KA"
    L"bdu2kQEfjzabTSilWK1Ww8iyZRCtacTShn6CJKDSijFZ0RwovWSWKNsXpySUMrNEMlkomF2gkLRyX2p8mLAs2gJnry9j5QBHrRHB"
    L"8xxD0OiT5ZjTZaMA4w5cxwGI4avbnsORKaDUfzqY16tJHtOQ0Vy/JXtYQuzo75uJAhb1J05hHSRDBmXGz8mMoOsSMO0bMMeBH3hg"
    L"LEbBmcGyZcvQaDTAGJOcc0xMTDx38ODBo8uWLWPbtm17fRzA4ADUarW9tVrtRc/zqF6vyXJPH3JeiHplFsQDKMXQCYH5us2PtGSI"
    L"iE5g72zuty1QZPKolUW/fLhf0vLRH1fAQJcLhhBKIT3tlp0zM/5JXc45isUc7v3nUfzzznmU+1eCnF4QcxKDOuZrszW8Ld04MbOm"
    L"xkSGxEEoaQbp0o8SHQJYKvWmrNNwDse8PBP+w9Yklg04KHf3ol6vwfM81Wq1UK1WD1Wr1amRkZGXZdtPigMYRpD98R//8WwcxzuV"
    L"UiqKIsW5i1XLe1BfnACYD6EY6h0GCQ6HsSX6+WSSDBn6m2V65OauAMrW5hmETz+hB5CUA4ywaiSPKArTk8XScKs7b9oBOOPwXQ/V"
    L"Roy7Hz4Kv9AP7veDHC/pzjFrcBjQBxve0z9BWsJNzE7ysDQFUCZFUIbq1bovK/0E49qROCNwh8HzHeQLeUSNQ1h3+iCUIkRRDNd1"
    L"2ejoKPbv3/9jAFizZk2MV/n2qmbKLQ7odDp3NxoNklJSrVbD6tWr0Vw8CBBHqDgUOXDMg2E8VUiqE/n6zIwby4BA+6e9LIKwlPRJ"
    L"pPAZaZWdV1sx6CEKQ2S3eFqyScus7ZwdR+C7ePHwPMZmYuSKgwDPmXsLsjw9ZUK7Pt3ciEX03+k0QMTSkG5bwvZ7ECWMHyNKiCBm"
    L"v4+RkjOTrnzfg+cqIDqMDRvOQqVSBdcrYmlmZqa+Y8eOXeYcidfVAexbo9F4tFqtLjLG2OLiolq+YgXyTg3tRhWOE8DlHK7DwYm0"
    L"VzODok+o3U/M/fpz9XIExjLn/wSyBy9JCpTIq0oBzBrb7OLodFgl9T1thEOjVXSED8cv6fwLi/azRs8Y3BoUpndPtCSv21IRLBMZ"
    L"bL63EQGU9BASTGFeruugUMgh6sxguCfEyMgK1Os1uK4rpJQ4cuTIw7VabXRoaEi92vz/qh3AMk5zc3OHOp3OY7ov3ZSel8dpy4po"
    L"Lh5BLpeHwxkcx3DlxviJyczGEEpUOlr7nn1QdlcP7LUxlE0DWQxAL+kHROb6NyWFEXfE5sYxJKvWktkFaN0hc/Jg3NfpIjnxJxg8"
    L"E9qJEZiTgsGk7LMpj6Un3srK9O/J0iqGuKGC0zTBOYfrc+RLRbQqe3HmugF4fg6tVguu69L4+Dimp6efbLVai6effrrEz7As4NVG"
    L"AGWuJwlbrdb3ze5aVqlWsOGsMxFWX4TL9ZIGlztwuVXHmAfBaMlO3EzzOLNYIWXD0uHMpeg/AVSEl6QC3yWEUYgwjBCGInNfoL4g"
    L"UphNYyLWLV/uOBr0MVOuEU9KPnZiWZd1Bpvz7SQvOwHU2SrAgJzE0LAEkm42JRUFIzgORy7w4DkS1N6HzZvOxvz8IpgZGzp27Fjj"
    L"6aeffhj61rXoZ4niP1MKAICFhYV7FhcX5znnND83r05buRo9uRri9hxyuZzR1psanJPJrWaeMEGCGQxgnIPbxohZAskyuGAJ/Wsd"
    L"whqfaWp177E2urvy6Cq6yAcMOZ8h8AiBR8h5BN/XH8sHHELEGJtMm1nJ6WbIGDmDSxJ8wJJ5Pf25psPHKfM1GTl4MuSZ0p3JYbDM"
    L"IiN4HkehUECnMYblAyFGVqxCpbKQoP/R0dHH9+7du29gYEBt375d/Cz2c17tF27dulUppeiDH/zggf7+/nuI6DdbrZZSxOjMNcPY"
    L"cXgP8ssuh1+toWXQbWzSgBAqXRNi5VFWMm0BH9ONFV0WsuTiCcukMnOv94mVgFKA6zPcdPckWo0AvbkqGvVGslTKzIwl6Js7HMen"
    L"mnj0uQXk8yt1b4AydTnLzOcnOCU1JqP0XoF0ZwIlIxo605m7BWx/xLbJzeYIZiIbI4A7HJ7HkSvksHB0B97xtnVQYAjDDvL5Ao4e"
    L"PaqOHDnyIID5tWvXipmZGfVzcQBjPbZt2zZx2WWX3TI/P//BcrlMc7NzOPvsc7Dj+Xvg0CUIAh9eO4LrxogEA2fSLFsivQ7OjkOb"
    L"B8AYgXNASJuD9ZZOxgEl0iZPOlipBzazO3cZIzRbCv/99llALAKiqfV2lJZiWaLGYRz54mlw/N4E+NmoYk8xZfM6TwEfZS6WADI4"
    L"Jbk8SpNgyaxMMhaut6Yjw4Nwh+C5DnL5AEpUUWCHcN55/x5zc3Pg3FFKKXb48OHFHTt23A8AP2v4/1kdIFlK8Mgjj9xbLpd39vX1"
    L"bV5cXJBnnnkmO30kj+MLB1AsnoF6owXP4Yi4g4hLcKHMmrhkFUJmjaoNvQTGFbjQS5MYESRJ00WkJStXWPKVSE4240CxuwtACWSG"
    L"OTml+ViDLUqcAaS5AeJOWpVY4Ma4afmyJY2gJamBspWNaW8rgt2Hr+z0T1K36NVzijKnn3O4LkOhWERj/lFsPqMXXeU+zB46CNd1"
    L"Va1Wo2PHjt1/5MiRAwMDA/JnDf+vBQZQSim2bdu2er1ev6nZbEJKqRYrVWzatBHtuaeR813kAg+u48B1OTzHPHhbUmXyKmUeBuOa"
    L"PbSfx5kuD5MTmZF8IukhZFcumwESxaDIA+ABpF+KeQDzAPh6kwi5IHIM4ZPqAhIwh0x1kqD11JHAMxxGFhdY3oOQtsiznc7M4ifG"
    L"GFyHI8i5cB0JWX8Kl1xyEWq1GgAozjkdPny4s3PnztsB1MvlcoTXYFXYzwwCbRSYnJy8Y2pqaoJzzmZnZuTq089Af6GBqH4MpVIJ"
    L"vu/CNc0Yx0nLQsre00empCKWgEVbI6djUplKYAkvkKGIM9WBwxk46VHzTiTRifS1srGRmzNTaSRIfolRKDuHn8EBzBCOGWdk2Uig"
    L"WT6V7Wwiu3DKMpYqAbic6+2jhWIRzcW9WLNMYdWqdahUKwiCQEVRRPv373/hsccee6ynp4cOHDgQ4zV4Y6/B91BKKfrc5z53sF6v"
    L"bwvDkMKwo1qtDi7cfDYaU0+iWMghH7imLNRLGx3DDpJ5mJnDYYSltjSykcAaAelJzPbdM9WAfdiMEar1CO1QoafLwxkri3jT6Xms"
    L"P83Hsn4PrsPRDoFY2kZORkie4ezBMlGKWcEmM0KWpQOfS14AQOrllSuElPxyDPXrOfADF52FR3HZpZvRCTV34boujh07hl27dt0S"
    L"huFsEATRz8L+vWYY4ASxqFpYWLhxenr6d0ZGRoqTkxPqzLPOpkef2AnRmECpVEazHSGMYnhS1+KxVJDcTBNJJAINbTwFrmzVIA1o"
    L"00RONt8q0NKrW830Txhrsue9Vy7Hr1/eg+V9MVjcQLvd1NfLCoapRcK+MY7HXlCYmBco5LwE1NmVLJbhI9vIMc5JWSdkNu3b3J/2"
    L"OdQJqFkl8jdKnJ0zBs9lyJeKaNcOY6R7EWeddQ4q1Qpc11VxHNPu3bsPPvPMM/cBEOvXrw8nJiZeC9OBvxbfZPv27UopRVdcccXE"
    L"29/+9vV9fX2bhRCyu7uHuTzGiy/uQ8+y89DptBHGItl6rSQgTrggISsXSfcIILkyVsHuJs5YnFLdAGeEMJTo7vLxlU+di4+/Owfe"
    L"OY7awhQa9Qo67TakiMBUiHI+wvoRgc1ncEhJODIl4Dqujgb8RDJHp6Wk9IPlAlKiyjKUijJaWEXJxRfZy0gT/QMjeA5HLuegq1xC"
    L"7fhteNdV6zCyfDUajTo8z8PRo0dp+/btNz788MN3bNiwIdqxY0cHr9Ebe62+kY0Ck5OT/+/ExESDMcYmxsfVOeduRolPQrQmUCoV"
    L"NSB0XbiOJYZYJsciUxbpjzm27LK3iTOWNGqyhoeJ1nEs0d3l4ebPnYNzl03j4Uefwej4AtqhhAQDMQdKEWJF6ESERlsh4CF+8wqF"
    L"X3uzRBQ2dYpJBKosg/QzNHWGwEnb2kubWKQyl18g3RlspwcsvuEuQ75QRFg/gmVd09h03kVoNGp6GhjAnj17Jp944on/D0AMswvo"
    L"lHOArVu3SqUU/fmf//mO+fn5m4UQ1Gq3pBDAmy88B5XjD6FcKiIfeAg87QCuy+E6ZKoCU0/r7kiGS9c3iGtp1VIgyBhlwqn+mjBS"
    L"+Mv/sgHD+Wk8teswcoEPz3OSiWB7vw8nfTOY63IQdxHGwNUX+ti8JkK71VwCPJGtALL4INPHsGhfZcM9pYshlUqFLPaiDOKa9g18"
    L"jlzBR2PyXlxx+fnwgzzCMEQQBHJiYoKef/757+3bt2/3unXr5J49eyK8hm8Mr/2bGh0d/fL4+HiNc87Gx8fUOZsuQNmZRlQ/inK5"
    L"C4HvwvdceI4Dz9EdQyuVStaoJEDQiCMTPR5lVqNlqGPO0GwKXH7BAK7axLFn/xi6SoWXCogJS9RAjqPVQNz14LgO3nlxDnmnCinj"
    L"BPmD0soi3fNHmQkfWoL7lq6gSy/ItMrnZIqIaeRfLBXRWnwRK3rnsfn8N6Ne16dfKUU7d+6cfuihh74NIDxw4ED7tTz9r7kDGNEo"
    L"+8IXvvDs9PT0NzqdDrXbbdlstXHpJedj8dj96CoGKBZ8BL6neQHXlIa2KmAsNRohub6F2VSQ8PWZ/oABZ1IqvOuyfohOFUJkSsaM"
    L"7CoRgTquEYq6cF0PnqubQacNFXBaf4yw01wiVIGVDzC2RLBKyZ5EemmPWr10Q4qNXJwAx2EIfAe+76A9/SO8/cpLwLmLsNNBEATy"
    L"+PHj9Oyzz9566NCh59etWycARK/1aX3NI4AZIpUTExN/f/z48QXOORs7PqrO2nAuhruaaM3tQbncjZzvIvA8eK4Hzwgg7QnnDktX"
    L"rTMYgQTBsenAQZKDLV+gJODnHKwe9rGwUIXnOUtEmloQqo3vuS48z4Xr+fA9H57nwvM8OI6LUjGHwV6OOGroq1yBdOSdnbDsOdV+"
    L"IyP6TuJA9lp7ylwtb6eVPZdQ7CqjPvsM1g43seHs81Cr1cA5V3Ec044dO47dd9993wYQlcvl1mt9+k+KA9go8Fd/9Ve7ZmZm/rHZ"
    L"bFIcx2phYRFXXXkl6mPbUfCBYiFA4HvwPT325HkOfI/D9Yy+LtMiZYY0chztBNw4hN3VZxE1ZwQGaa9TNQBTO5bLHTiuC9d14Lie"
    L"vqrdd+H5Ljzfg+97cD0XfuDCdwAlQ4NJ0n5+0ktiL9Um2nrfWshOFOseQDL5kPAb3GHI5TxwFkLM34df+ZWrAOjlVJ7nqf3799NT"
    L"Tz31nePHj+/t6+uLnnrqqfgkpOuTggESwcjevXu/cvTo0cOcczY1NSFHTluN9au7UB17FD09PcgFLnzXQ+C68D0HrqvpUO4YYaUJ"
    L"7dxEA2v4BA9wDSBtSFaK0A4lPM8zw5imDe1q3b/nuvDcAL7nwfW8xPC+78HzXCPBcs3lFDIjV1NLhKtZsarKlq0nrMtFcg9CZuLZ"
    L"iEY8l6HYVUZ1/H5cfG4P1q7bgEajDsdxVLPZZDt27Dhw7733frtUKom5ubn2yTj9J80BrGDkuuuuOzQ1NfXF2dlZAISx46PqbW/7"
    L"FcjKU3DlPMpdRfi+D993EbjcOAFPJmsT9i+5scuc/gQUWsWtdopOKLH/eBulUtGcMg3wXMeUnq4D19cLrn378u37HnzPg1SEuYX2"
    L"kpXsduUXZXM9paSOrvFlxvgq2TOQNP/MDgJOetK4UMxDhjMoyqfxtrf9MjqdDpRS8H1fvvDCC+rJJ5/8+tzc3GHP80JT/uGN5ABJ"
    L"Wfjggw9+a3R09AGlFFtcXJTc8fCWi8/F7MG70dNdQqFgH76TGMTzuCkTHbhOiva5wQkOZ5k0kHL1nsdwz2NzaMcu8oFvumsm37se"
    L"PM/Xod+zp95LnIM7DvI5H1PzTRwYrSCXyxnmz7J/DOkUcUbOkLkdbMleRENcgbLAT+ObnM9RKBZQPXY73nbZRvT2DyMM2/A8Ty0s"
    L"LPDHH3/8sXvuuec7XV1dcm5urnWyTv9JdQD7dt99982Njo5+8dixYw3HcejIkUPq/AsvwWChgnD+OfR29yDnu/D8AL7rwHcdBIEL"
    L"z9d5kjsMjmve55QY30leBgQSUCo4eO5AA9/ZXsHwYBnEOBzX08Y3YV4DwEwl4Lq6FHRd5AIXd/5oLyoNghcUTbcu02VkJ8hQiZK1"
    L"jEolW28SvWEy7WsqEc61k5a6u9Cc24G1g3O4+C2/hFazYRZBOPLxxx+v/+hHP/qbMAxnC4VC+2Se/pPuAHaryHe+8517x8fHv1Gv"
    L"11kURWpudha/8s6rUT12H4peiK6uPHK+xgO+z+G7KSi0JaLDdeh0XEocInEC12zUIiCXc/A3t0zgnh0NjAyU4Htm/MumA9e87zpw"
    L"XG5u5nLR31vA7fc+j+9tP4KuniGQk1sycQSWhnGVbUZQeg9CEv4TKRCWdBBdl6FQ8MDRgpr/Id7znqvhuh6EEAiCQB46dIg/8sgj"
    L"333mmWceWrlypZiYmOic7AN60iMAABw9erR98ODBvzl8+PA+zjmbnJyUvX2DuOi8tZg7dDf6e7tRyBlE7rjwPa3V930Hvq9FElpd"
    L"rNOA6xJcRzuCVR07TkqsKHKx9WvjuPGH03A9F73lHPI514yIOUlJGPgeurvyIAB/f9Oj+NJ1TyBXGoKbGwBxL8Xume2OydILpFLz"
    L"ZBuaWX6tP1VlOH/9OwY+Q1e5jNrxO3HVW1bj9DVnotNpwXEc1Wq16OGHHz78/e9//6vFYrHjeV4Tr3Dt68+tGfSvYAFcc8017Etf"
    L"+tLMpk2bmr7v/2pPTw9VFhfpvM0X4sXnHoJAgKC8GmHUMdSpNK1WW+alc+MpE8cyt3Cx5IoVKIA7DBIO/nlXBc/unYaSEcolD/mc"
    L"j2IhgO97UBKYW2zigccP4Qv/9Ai+t/0YcuXlyJdXgPtdZujDTPXa+X+WlnM2LdgVd8hsPEW69sAAVIYg4Ojp7UFn4VmsyD2FD2z5"
    L"TdgtK77vq0cffVTddttt/+255577cbFY7IyNjZ3U3P+atoN/WkB44YUXfqerq+uq/v7+3/J9X87MzLBfffe7ccM3bsWyzaeju1SC"
    L"UjXDmMUgMi1vEob+jRETgZgE4vT2bXs6mZCIzQSy6zrw3R48N9rAczdOYrj7OEZ6GboKHET6VpHpuTYm5kOQU0TP8Flwc31gfs6w"
    L"jSkdbe/9YZlr57MXT6d3AcslxtcbPrXKt1jMwUEd8cL38L6P/Qb8IEC71bJ8P3vwwQfvvPfee28dGRkR4+PjzdfD+K+bAySACahs"
    L"3Ljx88Vi8YJzzz33rKmpSXnmmWexKy49Dw/u2Iblm38PYdTWzZIOJZtD9Px9DAp1aGWxBGMSkdm8SKS3fsQmRNur5qCAUqkEpbqw"
    L"0GljerQBGXfAIOFwH0GuC6WBErygCMZz4I67tLW7dIbD7DSkRPMHaa+ATcl/vekrHXW3p79YKmBh31fwvrefgzVrN6DZrMF1XURR"
    L"RPfcc8/Y7bff/v8Ui8UaY6z5Wok9TikHQHrnwPP9/f3XdnV1XX/66af7Bw8eUBdcfCkdOfpNLBy7DwOnvR1yWvMGuryLwJkwvXgG"
    L"zgRCRqCYpc5ApOVVHGCxRBTD3EFggRqH65bAWDkVnGbWsBK3W7qy9/OZkJ/R8BGlV9ImN3wrqUFfhhu06cLlHL7P0N3Ti8bEj3De"
    L"mg6ufNu7EIb6gkrf98VDDz0k77777r8eHx9/fs2aNeGhQ4c6r9fpf91A4AlVgfriF79456FDh/5+YWGBlFLq6JGj+LVffz+iuSeg"
    L"avvQ09uNvJ/yAr7vIggc3TgJHAQBh+czeD6H5zvwfP13nqGVfU9v1nBcTSs7HJn2LgcxJzG+FnoYttGQS/b9dODD3uKVdvT0Onx7"
    L"H14aFOxkk8v0vb/l7jJE4wDK8hH8T7/54UT3kM/nxcGDB/ndd999x/3333/7smXLxKFDhxqvp/FfdwfIiEjbjz/++Bf37dt3vxCC"
    L"1es10Wg08N73/gam9t6GAq+hVC5qgxvGLvBd5POOpo991/wdg+daozumr8B1q9nXH3M9R4tPXGiHsEZOXkinfRPjm04iz9b/aWM/"
    L"XSkrjeHT2QBmQJ/rMpSKeQROE/Hkt/Bbv/kb6O0bhJQCnuepSqXC77rrrn3btm37YqFQaExMTNRN6H9dHYDj5/R24MCB+vLlyw96"
    L"nvf24eHhnkqlIk87bTXlfcKuJ+/HsrWXIJLZW8ApYf6IKB24tAazQx+cMhs4uDEsT6Zv0+UNPKGVuek/sCVjXSwzBXRCl9feX5jZ"
    L"ScSIkpUxrsNRzHvo6Slicf8/4P3vOhtvufQqtNtNvaOIMXnHHXe0v/nNb/7JkSNHniqVSq1Wq9V6vY3/c3UAAPTss89OnnXWWYu+"
    L"7/9KX1+fMzc3i3PO3UyVmSMYPbIXg6svQhSFeoUbS2lZzjMz+BnZtp3TZ8YpEqMzs3TRNJlYhkTijp4MSlMASyVodlwtI/iwpV5y"
    L"i3Ay0aR3E7oOQy7noK+vB5UjN+PSs4H3feC3EHY64JwhCIL4wQcf5DfccMMXt2/f/u2RkZFwenq6/noCv1PFAQBANpvNgyMjI93F"
    L"YvHNpVJJLSzM00WXXIoDzz+MWrWCnhXnIorapu5XmR38+j4dKxuD3azBKJlE1rMH3LCGlJz+ZDYhGV1n4A7pF09nEqzDES3da2Jv"
    L"edMnX+la3w52eAx9A/1oT9+HNeU9+NjvfTL5uiAIxJ49e5yvf/3r37v55ps/Pzg42JqcnKycDKHHG8IBlFL45Cc/GZ555pkHGWMb"
    L"ent71xCR7HTadNHFl+DpR+4EdwIUBs5AFJoOnRGG2qFRLfYw4doYk4ywRBuZJytXEvqYaQbRfn4SGazyl1KRasIEJpd5WTJI6eYO"
    L"18b3OIfvAb39A6DGTpRad+OT/+n3USx1QYgYvu/L6elpfsMNN+z+6le/+r96nje9uLhYhRZ5/tzenJ/nDzdVAYho3x/+4R9+NgiC"
    L"kQ0bNpxVrVZlEATsgx/6EG78+o1YFuRR7t6IWrUCplwIR5jmUAzuEKJIG9eNJaJIwREKscvMvcSAFArCKDSTM6xequY1hH/aBlaU"
    L"8PzgZjpZmiEvTuBI27ueQ+jp6YEXH0A49W187H/+OPoHBtFpt+B6nqrVamzbtm3TN9100//ZarVG+/r6Gs1ms/PzyPunUgrA1q1b"
    L"oZSiq6++euz8889fFEJcOjQ0VKpUKqrcVaaz3nQWHrvvZvQOrYZTHIaMwyTfw0zpclvDW5BndP2u2VCSnPKkb2BTQdph1AARJgKk"
    L"y6jIroA3AytkR7lIl3quw+F7hO7ubuRoBu0j/4j/+LEPY/1ZG9Fut+A4DsIwVLfeemvr29/+9qefe+65H69cubI1OTlZfz24/lPe"
    L"AYwTEACRz+cPl0qlmIguHBwczM3NzamBgSFatfI0PPbjbegfXgOnOAQhwiRH8xOkY0s+7qQaQ70QkqVqI9NdZInDpF9nRac21MPO"
    L"DxrDa36fwzPGL5e7kedzqO7/Mj7yoV/Dps0XodVsGBELi3/wgx+wb3zjG39+//333zI8PNwZGxur/rxA3ynpAAYP0Ec+8pFOqVR6"
    L"MZ/P53zf39zX1+fNzs6okRUraGR4GI//eBv6h1fC6xqGiKNERMqyCJ5S9O9yfbGjXudO6anPAj6eAY7JVK+d/DECTiPmcIww1XWY"
    L"5h1cffILfBrzL/wdPrzlHbjozW9FvVY1AhUvfuihh5wbbrjhy3feeec/rFq1KhwbG1s0PX51Kjx351RxgMyNJLOc87/1PK/b87x/"
    L"PzQ0lDty+LBavfp0+nfv+zXcfvt1WHnRh1EeOBv1WlWfZKHgCIFYKngOQxQrxPbyaaHg2mtfhBlHU5TeR2zuIJTJncYwG84VpCIo"
    L"Dn2dKxGY0unA5QwuJ3SVe+CER1DZ91X89od+FRdceAmqlUXbbhZPPvmk881vfvNbt95669/19fVFR48eXTSIX50qz/2UiQDZ1vFX"
    L"vvKVyuDg4F7P8waKxeKZXV1d7uzsjFq+fAWtXbcWj/3oZuSCPMrL1kHEIiGIkho+OdmW5KGkLWvXsDmJuJQyQtN0+CQhdTKbzlyH"
    L"IfA4Apehp68fqO1C/cA/4SMffj/O3XQhFhcX7MmXu3bt4tdff/13r7/++s/19fUtzs3NLQLonErGP+UcAEgHTT/wgQ/M9fb27vU8"
    L"b6RYLK7r6upyZmZm0N8/iE3nnosnH7gdstPE4KoNiJXet28ZPi0cYSndawFg9mNuuujZccgMpixlCxMHMAOcvscQBA76+nrRGL8P"
    L"0fh38B8++hGsWbse8/OzICL4vi/27NnDv/a1r9163XXX/Xkul5urVqsV6Ovd5an2vE85B8hGgi9/+cszfX19exzHOb2rq2tNV1cX"
    L"n5ubRS5fwMVvfjNeePoBzBx7ASNrNwFuDpDxEs1gakgY9K8lYI6ra3eeqRJSZyEd4pnmC1xO8D0O3yUUCgX0lHKYeeEbKIun8Xu/"
    L"9x/RPzCE+flZMMYQBIHYt28fv/7662+77rrr/msul5s1t3qfksY/ZR0gEwnYli1bpoeHh/cS0apCobC8XC571WpFCSHpLZe+FYvT"
    L"R7Dn8XvQv+w0FPoHIYQEZwrMSeXi2bCfOEdmwMQOmVi5uaWJPc6ShlO53A1XzGH0yS/jTSsEPvLR34MCbM5XnufJF198kd94443f"
    L"+drXvvbZ3t7euSAIKs1ms3WqIP43lAOYSKC2bNnCv/Wtb43ncrlnHcdZViwWV3R1deXa7Taq1YrafMHFVMp7ePqB26DiDgZOWwe4"
    L"PkiJNPzzdMzcYRnkz1MnsLneNc0czyG4HMjl8yiX8qgceQCzu2/Au3/5Qlz9q+/F4uICms0mHMdRruti586d7Otf//o/3nTTTZ/v"
    L"7e2dn5+fXzzVjX/KOwAA7NmzR23ZsoXfdttt07lc7mkpZSmfz5/W3d2dV0qx6ekptXLlajrnnHNwePdjOPb84+gdGEShfxDEHEDF"
    L"SzqJdto42zNwzcnXoV9HjcD3USrnIOvjOPbk9ejFIXzkIx/GmnVnYXJyAnEcw/M8yRhjDz30UHjTTTf99W233fZ3vb299fn5+UUA"
    L"p7zx3xAOYJ3gmmuuYf/4j/84X61Wn/R9X7iuu6Knp6fs+z6fm5tTANFFF1+CQsDwwuP3YmH8MLp6+lHo6QV39G3g6eJJLK3tyQye"
    L"Ohy5XIB8zoVqzeP4ztvROvpDvP2t5+Jd73kvojjGzMw0iAj5fF5EUcTvuuuu2a9//et/dv/993+zt7e3PT8/v2CML98Iz5bwBnoz"
    L"+4kVgODjH//4BzZv3vyf3vSmN52dz+e7zC3aanh4GXm+h11PP4XdL+yF0zWCvnUXoGt4LeDnIAGI2PQIlOnq2evtwwi1qcOYObwD"
    L"snIImzauxSWX/hKIOKanJ6GUgud5MpfLsbGxMfzgBz946vrrr//cxMTEM/39/Z3Z2dkF09yRb5Rn+oZyAPs7b9myhW3btg3vf//7"
    L"L73gggv+48aNG68cHh4e5pw7zWYTrutiaHgYUgjsf/EFHDxwEPVQId87Are8HE6+DPAAShFEHCJuVNFaGEVncRxFL8a6tafj7HPP"
    L"QxDkMTMzbbd0o1AoSKUU27Fjh7z77ru/fdNNN/0NgMnh4eHm5OSk7eypN9TDxBvzja644gq+fft2sWHDhtPe8Y53fOjss8/+wNq1"
    L"a9d3d3cXpZSs1dJduJ7uHnI9F9XFBSzMz2F2dhZxHMP1fEAphGEHruOgu6cbg0PD6O7pg1QKlcVFWGfK5XLS8zw2Pj6O7du377/z"
    L"zju/vGPHjrt7enqaUspapVJpnEr07v8IDgAA2LJlC9+2bRsAuO9+97sv2bx584fPPPPMK1etWrWyVCp5URRRp9MBQCqXzyGXy5Hv"
    L"+cmmr2SLi1QIwxDtdhvtdgtSSjiOo/L5PFzXpUqlgieffLK6ffv2W7Zt23YDgGP9/f3h7Oxs1dT44o36DN/QDnBCNJAA+n/913/9"
    L"lzZt2vQb69evv3zVqlXDpVLJY4yh0+kgDEMlpQQAUsnNIYkARLmuq4IgIMdxlBlhw969e2tPPPHEvT/60Y++dezYsZ1BELQ9z2tU"
    L"q9WG4fXlG/rh4RfkzUQDq3Iees973nPZ+vXr37l+/fo3j4yMnDYwMJAvl8vc3nhORMp1XWHkXoxzzoQQmJ6exuzsLPbu3bt/9+7d"
    L"Tzz44IN3jo6O7gTQGBwcDKenp2sm17/uCt5/c4CfPhrYtT5da9asOXPdunXnnXHGGW/t7e09vaenp1wsFnvy+XwOQCClRLvd7jSb"
    L"zdmZmZn5Y8eOvfj8888/umvXrueEEMcARIODg6FSqjEzM9P+RTH8L6oDLHGEsbExfuDAAQDwzWtkeHh4xPf9Muc857pu0XVdklJ2"
    L"RkdHR2u12jHojl0LgBgeHo6FEK2ZmZnOGxXk/Y/qAEtKxt27d3PHcdxdu3YBSBZ/2XRh1/o6a9asoXa7LTqdTjg3N2fDvPhFNPz/"
    L"KA5w4hu74oor2NjYGI+iiIQQyb8/CAJ54MABYUCd+kU2evbt/weOZ5UROhuNxgAAAABJRU5ErkJggg==";
static const WCHAR kShieldY48B64[] = L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAARbklEQVR42sWaaaxd13Xff2vvM93hjXzvcRJlyZIcWfREWBagVkko"
    L"R0GkCpYquSSaoEAsB4hapLGDBGjgJvUlXaAIjCBugPqD234w0NgJSJSxlFgyOphyU0uo40KEJYqJJWqiyDe/d+d7zzl779UP5z6K"
    L"lqzRbvKAg4d7cM++//8a/mvttY/wU/o7ffp0dPvtt7uvfe1rC0tL879lTPwrpZNrrI3UGr/mitG32t3BHxw9evRvW62WATh+/Hj4"
    L"SX9XftIFWq2WOXbsGCISHn300XubjdqXonT2Wu9LYl3HuRGFLpLVd5EP1jqDYe8Ld999zx8BnDhxwh49etT/vRHYsTrAY499+/NJ"
    L"kh2XqMG0/MAv6V+YZPSUhHxAx+/WZf14KKfvt816jfW1C6c63cE/P3r06Prp06ejw4cPexHRvzMCJ06csEeOHAkiog899NC1s7PT"
    L"X56enr9rMCrCNVOPs1SeNK63jisLNB9iyjFqPC/5m3Vt+l/5uV3XRZtrz58fjop/ee+9935rxxjvhsg7ISCqagAVkQDw+ON/9WuR"
    L"jf9AbLagfuBumP9+1Mz/N/moQMsuOlzFj7bw4xHqLInkbJj9vNT8rK8tfdyWwzU625tf3tjq/P4DDzzQnoRkdOzYsbdNxLwZ4Far"
    L"ZVTVnj59OpoA9yISnnjiicNnzz797cXFvf85aLTQTDv+IwfORU2zSmmvwyQLCBlIBhqDD4gfMR5HzA2Wef/W562++EfBqdG9V9/4"
    L"G/v2Lj750EP/9be+8Y1vTB0/ftyJiKqqabVakaoaVZW37YHJl42I/Ehy3XTTTcnXv/5ffqlRb/6LJK3dNc4dve6av3q3M4tT2+JG"
    L"PUKZo/kqYXQRHS0TRiv4wRp+sEkYjVCvBB9hnMXEBavpTazNf8oniz9vY/FsrF14sd/r/fFglP/ZAw88sPIaXGbH8283hOx3vvXn"
    L"+xavvub9082Zu5KscVcU135mOMxZX3tJZxsDvWpPYiIb4UsDfkjIV9HxJcL4EmF4kTBcJgxX8YMNwmhAyD2hCGgQ1MXE4inShOXm"
    L"bdpf+GRI5g9ZA2yuvdLOx/3HxsP+Nww81RmW5x988MHOm3pgx01PPvnkwr59+744NT39D73jQJLWsgBsrK3Q3Xo2TKUrujTjbFaf"
    L"J8heiHaBFmixTshXCaNlwujihMAlwnAFP9zED/tI6dA84Itw2Ruihihy5FmTzamPheGuX1RmDtm4tkBZOLrtDVwxWO31ev9zfX39"
    L"05/5zGcKEQFQgOhKi4uIO3/+/D/ZvXv3py5deoVh7yI+vxhSfSk0o2Wzu+lMHM/h9Rqcq2GSGHCgDjQgBGJToiZH7fiKa0SIc0Y+"
    L"xhuDGE9wHoJHg2dcRNjhmD3d08ZvfJdh7YAOpw4GM3OQmewam8zfsHvXroVfiWP5ExF59Mr6Eb3WJbVa5H3Ad17+D/6a+qOxITM2"
    L"So2aJYK7msI0MVGCaIn6AYQCDQXi+7hiyPMXG7Tbexh2hUG3xqg3x7DfJSk3uP3qF8iMIYjFWCEEJXjFBE/w4HMDg0DDviBTG89b"
    L"afx3iqkaL+z5vSJeuMX60l0PsLi4eDlyoh+TxDcWJbYZLWsiPcl9E08TQxMxERI8hBzcCJ0InfocEzr0un2eWd7L1pZlux2x3W7S"
    L"6c7T6w0oyveyP77Aof0FhVoEgwgYAyqKqgfnUB/Ig4BGmKEhzkv81KbRGW977fbotXhfR2A8HM+PGgUu76OxRUOJCQXgUatVqPgC"
    L"pQuuj2qJujEm9PDjMfkoIx9uUY7auFEbP+zgR31iaygKj/oAwUCQSRQLKgKmurRwaO4IRYnmgjEp5XBIrMr03OyNVdV/7PUEdm7m"
    L"ZeFGwz7BdaocdzkqDqwiIUAoCK6HuCEaSghjgisIOiCMcvq9WTrdPt1en25vQK8/ZDgqyBJLhAen4D14Qb2BAIKpCAiIAVGtkjxX"
    L"cODKkhglSdKFt+GBXLLaGBEFVTR4oKisbsZAD9wYDR4NQ3AVAa8jZFjQ7+1hu92m3+3Q7Xbp93vkhSc2lsQ4NATwioYKvHqpZNUL"
    L"qqbyiiqiAQKgAsFXngL3lgS8Bpx3RNU6EBRwqBsB/cp6KOpHaNkDnxNciYQcHRZ029eytdVh0O8wHI5wpaMoHWVeEDtPGEIYBdQJ"
    L"6nQCvAJa1YfqPt5CJIiCCIgI8mPq1usIBOfRy9mpoBMivgSGIEUVOkWHUHZQV6CuxGhBGBR0O1v0BznDwYB2u0N7e5u1jW0OLNYx"
    L"1wuOQHCABnTHwoRJSZrkgjVIZMFYxEgVXW9Qcl9PANUQlKCTJNsp3gqEEtUcLftovokW7QkBh8dB6WhvrfPDH66wtnyBdqeLcyXO"
    L"BXZP78EaBVt5VXyY5LBcrqey051Zg0QRiEWMQYwFhBCCviWBqvsSjInQsGP+CQNVCA7cEC261eVzgg/o2MOW5YfnzvGDMy8QRwbE"
    L"EEURJrJkSYQQJnYWMFVYqEr1eYeCCmIVRUEsWEOUJBhjMNbGb9mNOlc+pyq4UEeYWCmEKhdUIYQqgdWDBEIecCuB/GWQjpKagJi4"
    L"Am4MSRyT1eo0s4jIagXSGiSqLmMNYgWsIFYw1mDiKoRMbAmRxds61gjj0fg8wOHDh9/YA/lwsIYYCt9ANVRKhCKECYFJXGmgXFfK"
    L"NdCikr/IBuoJ7HS/SZKQZSkSVwRiA0QWE0wlnWoIahC1VLFlq4UCIAGxESFOKE2TyAgCl94yhGxku670qJmrdhRaJdxEKlCjaKEU"
    L"FxS3WSW52Oor1kBiq7iO44gkTcmyDJs1qaVKFBnERhgzEXws5grwShXrGkBMwMYRLk2IarOoerJaFr0hgfX1dQXoD3vnm8UYWDJB"
    L"LaJMND9ArIQuFK9AGEyAu0miKxiU1CjYiDRJSNKUWq1G3GhSS4dYa8BGIFViilggQifeUExFwCtqhSix9JIGJpux+XhEMSrOXYn1"
    L"RwicPXu2EoW8WC3y0Si2+2pOEzUgwQfEOnxbcetUbYCRiQS+qrgGpZEEojgmTmKyLKNWy8iaTaZrBTayEMcYsUgUIWJRscgEvEzW"
    L"UxcI3mDrEa4+Q1ybJR+PXC3LNt8wib/whS8EVZXzFzdXymK4KslV5G5aBQH1uC2P2/BXaJ2ZqMarWwtBqUUQxwlxHJNOPFCr1amn"
    L"hiiJkSjBJCkmTZEsxWQpJkswWYxkMZLESBJBlmDrlry2W7P6rOT5oG1rtZcBjhw5El5HQFU5efKk+exnP5urG5+16V564z1q1OLa"
    L"gtt0k95fqxAQ82rxuewBoR5r5YE4JkkS0jQly2rUU4ONY0ySYNIESSckJkQkSyf3IySLsVkC9Zhx7dqQpnW8c8/fdddd/VarZa7c"
    L"8P+IjO702cNx51smqtMub1AtYkI/qqTGv0rgsmLwag4gkFjFWEsURZdJJElCYoUoSZBkAnpymSxBsgQzsbzEFkki4npMntQo0hvU"
    L"4lWDfk9EwuHDh80b1oHDhw8HgMFg+9uDftsNog/aUVGrYlc9+JyJRICx1eM68YJUBckKREaIjBBHhjiyJHFUPZIkSFJZX5IKuKRX"
    L"gp+0EJElaVi2ZRE7db2Mhh1JsvSxtxyriEhotVrm6NFfOzfsbp9JZj/Atl8IcSooivoCQlnFu8QI1cZkpx0QhJp1jMkoyMg1oyBl"
    L"6CxlWWDTFOIEieMKdBwj8cTqkUEmBc7EEVIzrMvPaL25YNfWVrq7d+/7zsTI/k0r8cRFOui3v5rWZmXdfkhJBLGm6oV8MQmjCExc"
    L"bamoGq6xt/zie/r8xqEOd76nzeGlC3yk/izv1yf42P4+pPUKXBzDFeCJbLX+ZFMTZxFdahT1j/ksEc3z/M9vvvnmjRMnTtjXDryi"
    L"H0PAA9Lpr/zZ1vrF4/HsbfOdjcdDM90yhSsQV4AVsBYx8aT4OESEIsCBaeH3/kEPhzA2GXFiiRsNXNaEKK7kM4ogrhq1nY6xikRB"
    L"jSFpGP6m9x4aV91oup0NaTSm/mQi9fKWvZCIaKvVsr/6q7+52e9ufb05d51clA9rVJNJV1heDiNMhJi4ansRUIMBlkc1/v2593Hs"
    L"yQ/yx88c5IedBvXMojZC7E64VI3azk5sp5eLkoiBRHTiW/2u+VlZWV75/p133nm61WqZ48ePu7c1Wjx27JhXVXnhpQv/tr213Mnn"
    L"f8lsy5zGtah6wheIVr0KNkGIQIVIhL6L+J3/tYf/+N0R/+e5Lt99pcEffv99PL0sZKlBjXkNcLks4wGh1jQ83zvA/FW3aTHuSJyk"
    L"XxYR/1r1eVMCIqKTmrDe3V796tT8e+WCvTVEdTtpBUKlSjYCk2AkQYmpxYG/Wp3nu8/2mTYDpuoJs6lD0mm+/eIufPCVyshkhyIV"
    L"cPUeVEnTiE6I2YpuDwvzs/bFF186f+jQoROqKrfffrt/R8PdI0eOBFWVZ/7mueNb6xe2dOluWTEHQjadQZqiptqMm7hSFkzVIqz0"
    L"K8mt1RukaUZSqzPdrNEtM/p5wFhTTSEub5I8+IAiJE3DM9s3sP+9P6fbW8uS58Xv7N+/f3jy5MmdZHn7BHa88LnPfW57e3v93zWm"
    L"583F9K7gm1NE9SlskiJGEBsjUYbYCC8pH5gfMj09RZzWmGrWmZ2eIa3PsLvWIYktulO9NYD3qPf4oNSaMRcGDfzcvX5uJrMvv/zK"
    L"4/fdd9/Dqmre7BTHvsVBBgcPHrRbW9d9z/LSP57d95G9m531sH+qLc5FiAZUFTMZUhUerp52qFjOjxeZnl2gOTPPgn+Oj1+zwp7d"
    L"00xm35VBXTUnsnFMyAxnOnfw/o/8I33x+XMo8snrr7/+0sGDB83Jkyf1XR9w7MwhT506dcvi4uLjCrp/+Kf2KrMug80hDHvoeEQo"
    L"xmgxQoIDUf56NeZvuzXmGoYPHAhce/UccT2GyFQyrJX1VQ2NOcsTyzew98O/79K4iM6c+cEX77nnnt9VVfvaMf87OeAA4OjRo77V"
    L"akX333//91ZXlz83M7MrekF/wfezJeqLS5jZ3ZipeWx9BlOfgayJpk1uuz7jUzfDPR+OueHAPFEUT+Y+ppoLlYEQDM3ZmGc2Zmle"
    L"+6DfszQVPfXU0z/4xCc+8fkTJ07YK0YK757A5DjUt1qt6JOfPPKHF1589n8s7LspOtP5qAuzu0gW92Hn9mJnl7DTi9ipBUx9hjKe"
    L"QdM5JJ1GTQo2BZOiWFQNQSIa0ykv92I6U58OH/rgh+TJJ//vME2zXxaRfCcPfyoEdvRCRPDKP9tcv7A6tefm6Hsr14d4fo54aT9m"
    L"11XY+X3Y2b1EM7uxU7swjRkkm0LSJkR1MClIikpGfbrBRhnznDuqt/7snf6Zs2fM1lb71++4445n3snx6zs6pdxZ+OGHH751bnbm"
    L"v9lkql5u/zW3Xtc15J6iN4BiiJY5hBJRjxJAqr20SMCjNKeU9XHJmfYvcNsdny7XV16Inz577ov33HPP7155dPv/5Zh15wceeeSR"
    L"f7q4uPCnpbNhvP0Ut9zQMw0Lo+4Iyhx8iQZX/VeHSkCMp9FwvLRVcrb3c3z01nvLXnslfurpc1+6//77fvudgn/X58Rf+cpX4gcf"
    L"fLB85JFHPj03O/OfbNIw22vP+pv2b9irFixh7CjHBcGV4AugJI4cKiOeuWhZk5/V9934Ud9tr0bnn3/xS/fdd99vX3n2/HdyUt9q"
    L"taLjx4+7U6dO/fzCwvyp+V175i9dfNlNxxftdXsLmasFTHCoLyjKnNW244WtRdL5j/mFhTmztbEqm1vbX7r77rvfNfif+FWDK2rE"
    L"dbOzM1/du3ffbf1BQWd7OdSiLtOZE++9dEaxarJX5+Z3ExlnLl28VIoxv3nHHXd85ScB/1N52eMKxTDf/OY3f31qqvGvm82ZA0hE"
    L"UTo0KHFsERzdboeiKP9yaWnp3xw6dOjM3/vLHle+sbLz6sypU6d2ZVl21Frzy965g8baWVVdM0a+Mze36+u33HLLwz+tN1UA/h+r"
    L"mSgIUXffXwAAAABJRU5ErkJggg==";
static const WCHAR kShieldY128B64[] = L"iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAABPJElEQVR42u29abRc13Ue+O1z7lDzmzHPI0GAI0CQAikRlChKlCVZ"
    L"jk0ojtRp2e7lXnZsd5y10p3upAMwWRmULMexkji2nGVbkR3JoGNHliNLJCVQIiGSFkiKJCiOgDA9AG8ea7r3nLP7xznn3lsPoCRK"
    L"BAe1a61C1Ss8FKrunr/97X0IP6a3w4cPSwA4cOCABoDnnnvuzoWFhRsnJyd3lkqlv7uwsKABSAAwxnAURRRF0czCwsK/Xr9+Pfbs"
    L"2fM7RDTv3+uee+4xRMQ/bteJfty+EDPToUOH6N577zUA8KUvfen2MAz/PhHdHUVRnCQJ0jRFGIYgIhARmBlKKRhjUCqVAABpmp6u"
    L"VCq/labpZ++4445J996CiMyP0/UKfpy+zMGDB72A+MEHH9ybJMkhKeXdYRgiTVMkSaKISMRxLJgZRADAABhBEHhlUFprAFivlPp3"
    L"SZL82gMPPPDp1atX/yERXfhxU4IfGw9w+PBheeDAAX3ixIm+s2fP/u9RFH2y0+nAGMNCCANAWDcfIIpiCCHBbC+BEAJKddHtttFu"
    L"d6GUhpSSAWhjTCClRKvVOjs0NPRTt9xyyxNLw8vfKMCbfDty5Ehwxx13qG9+85tbpJRfabfbm9I0VWEYkjFGVKsVKpfLaDY7aLa6"
    L"WFycB8Egjghp2kW73YUMGxgaXo2hgRKE0Jiemka700UYhsYYw91uV5bLZQghfvP222//B0Wl+xsFePPivbjvvvvowIED+siRI1uY"
    L"+QEhxAallBJCBKVSCX19DUxNzWBycgblKMGqkSaW176LSJ2A0LNg00WSLGB0ZhCnx4cx3d2J+tBV2L59G+JQ4/yFizCGEQSBSZKE"
    L"iUgGQfD7q1ev/o/btm176tixY+GePXvSv1GAN0H4PhZ/61vf+s12u/1zSqk+IYRmZjk4OAilFF555SQG+2Ncu2UBy+InwfN/jWT+"
    L"IkyyCFYGYAaJAGEEiFIJM90GXji/Es+c3YHhNe/GTXuuwfT0OCYnpxHHMdI01UEQSGZeCMPwjne+851PHDx4UBw6dIjfjlXC21IB"
    L"Dh8+HB04cCA5efLk+jNnzvwfpVLp11utFnysX7lyBc6fP4/xsXHs2zOMNX2PQV28H93Z8zCKAc1gGMAYQGuw0WADMBHCmFBpxOiI"
    L"Gr5+fAQvTt+JO99/AOVI48TJU4jiGGxM2u12Q6XU3LJly/5k7969v0RE5u0YEuhtJvgs+fr617++N47jLyulBpRSipllFEW0fPky"
    L"fOc7z0MI4M7byoin/wiL498GK4AMAJ0CrADWMEaDdQooDdYGYNjEUAjIkkRjuIRXxuv4iyd248Z3/RK2blqGl196GdqGBG61WlSt"
    L"VrG4uPi54eHhf3DzzTdf/BsPcIU+55EjR+Qdd9yhAODRRx/9f+fn5/9+qVQaBKCMMUGj0UC1WsVTTzyB9RvW4B3XLSA5/Wmki5Mg"
    L"I6ygWYNZASYFTAo2KVilYK3AWgEa1hMwAyQAIVEbjNBEBZ9/dAeGNv9veNdtN+LUqe9iYWERURRxkiQmTVMZx/FipVK559Zbb/0y"
    L"M79tEkR6O1i9v5DPPPPM7XNzc79SrVZ/Zn5+HrBem4aHR9But3D8+Hdwy827sH3NCbS/+0fgpAswW+EaDYIGmy5YJe6xC+jEKYIC"
    L"awNiAht7BwMsJOJ6iLCvjC8+uRoT9Ldx9wc/jObCJEZHLyKOYzCzbrVa0hiDWq32wOrVq39m27Zt80eOHAn279+v38q5wVtWAZiZ"
    L"AARElF64cKF65syZn223279TLpdlq9VKiSiIooiWLRvBiVdOYHp6Fu9773UYEEfQHj1i8R2trdUbA3AKmC5YdcDKPeqOVQCdWg+h"
    L"NZgZzAJkCEbby8MQCMoBGssreOzkAB45/W7c8b5PYMVIBa+c+C6MMQjDEJ1OB0mSII7j0SiK/un+/ft//60OJb8lFcDX9QBw9OjR"
    L"XUmS/Pvly5e/Z2pqyhARG61lo68PURThmaefxuDQEO5451rImS+gM/0dCAqgtQIxA6xt3DcdGNUBqzZYtQHdBXTXveaUwKQAGzAL"
    L"wBDAAqwJ0AQIAsIAfSMxLnZr+LMnr8O6XZ/A3j27cP78WczMzCGOYxhjdJqmcn5+Ho1G439WKpX/5Z3vfOfM0u/1NwrwfQT/+OOP"
    L"DzHzb6Vp+rFarYY0TZXWWgohaHBwEBcvXsTJkyfxjpuvx9ZNHaQXvgzdmQGBrMtntsmeSQDVgdEdsG6D0zagWzCqC+gOOHUewaQw"
    L"OgW0AhggJhgIK3wtABCYCZASlcEIXK3iS8+swjj9Ldx514cQiC5OnzkHAAjD0HQ6HQghxOLi4vNRFP3uqlWrPnPDDTfMvtUUgd4i"
    L"rl4AMETEhw8fjkZGRv5upVL51ZGRkWtnZ2cNM8MYI+r1OgDG8ePHEUUx7rj9WtTCF9AZPwYyBmRsZm8t3yZ6RnXAugXoNlh1AN10"
    L"Qm+DTRdInXKoFJwmYJMAhkEM6wkYVhGMBGuyF0xIBNUQjWUlPDXah4dP3YKrb/oYdl29AedHRzEzm3sDrbV0DagT9Xr9P7373e/+"
    L"z0TUeauEBnoTBZ+heAUP8Ik4jv/PoaGhHVJKtNvtVGsdRlGESqWCM2fO4NR3T+GGG67Brp194Nm/Rtq8CCLhrN6A2YCMAkwH7Czf"
    L"Cr8F6Jb1AqoNuByA0zZYdWF0F5x2s7LQfkaADAEkwIZgDAFGAEyAEEAg0TcSYTGo4i+f2YBW9SPYf8ddiAKFc6PnkaYKURQZrTV3"
    L"Oh1pjIHW+uzw8PC/3rdv36eJSL3ZikBvtLUfOnSI9u/fLwoukI4ePfpRIvqny5Yt2yGlRJIkxjZkhKhWq5idncXx48fR12jgXe+8"
    L"HtXSONLZ5wGdgAyBoQDDTvgJWHetoHXbKUITSNtg0wY7JYDLB6A6MKoNk3YBlcDoBFDu/Qys27fggM0JGGAtbKLIBIQCcS1EdbiE"
    L"Jy8M4dEze7Bh10/h+ut2YGF+GmNjExBCIAgCTpLEtFotKYQAgBMrV6785O7du/+YiFoA8Lu/+7vh+fPn9RuJKr4RCkDMTE888YQs"
    L"YuZHjx7dpbXe1d/f/w8bjcaNUkoopVSapoKIRKVSQafTwfHjx9FqtnDzzddj46YGsPgS0s4sCOSLdjAzyGjnvp1lZ27fWb1uAco/"
    L"tuzvZElhJ1cAlQA6AbRxwmaACQybF7ArE2EIrAVABBYSMpKoj0ToRFV8/ZVVON3Zjxvf8RPYsGYY4+NjmJ6ZRRAEkFJyp9PR7XY7"
    L"EEKAiL5TqVT+4+Dg4Od8juCVYWBgwBw4cMDA1jRvHwVwcV3aOp0yF//Zz362sWrVqpWlUumf1Ov1jw8MDCCOY7TbbaWUygSfpile"
    L"eulFjJ47j+3bt2Dv7k0AxpHOX7AkDkh7RQyDwDmoY3xp54SfufyWe61lBe4ekVpFsa6/68pDrwTGeQF3ZwI058kgbDhgQzZXIAGS"
    L"AmEtQH24hDPNGr5+Yit48G7cuOdWDPaVMDZ2EQuLTUgpIaU0nU7HdLvdAAAmJyefHx4efqJcLn/qzjvvPElEU/663XPPPfKXf/mX"
    L"CQBe7+SRflRBP/TQQxIA9u/fn2lq0X396q/+anzPPffs6e/v/4DW+kCj0dgyNDSEdrvNzGy01iAiGccxkiTByy+/jHPnzmHNmpXY"
    L"e+M2lOtt6PlRGMMQogQwgchn5QYwytb3puuE3wHrZqYA1gs0wcZ5ANUqKEDbKUgHxmMDyqKD0AqsYfMKzYABjHu0X57AEA44cooA"
    L"AkkBCgNU+gMEfWW8MDWEx0evRnXFe3D9jXtRrwaYnBjD3NwCSAiEYWjSNOU0TaXWGrOzs4ii6GKlUvn9kZGRh/fu3fuQTxqLMjt4"
    L"8CABEDt37uQfBXGkH0Xwr6aNjzzyyKpqtfqhTqdTq1Qq/6hcLg9s2LBBtlotaK2htVaeaBGGARYWFnDilRM4f+ECViwfxp4bt6De"
    L"l4AXz0OlCYSsAlQCKABR6D+Eg3Vd5q5tjIdyj7rlLL/tFMIL3t298HXHKoJyHkAnFhdQCmxgQ4Gx3oY1wxinBOw/hrClJyxmwEZA"
    L"CJskUilAfSiErlTwzIUhHJ+8GvXV78HOa25AfyPGzPQUZufmnHILw8wmTdOg1WqhXC7j4sWLKJfL0wsLC/+hUqlcuOqqq5Ldu3f/"
    L"weXQ0ueee449De6KKsDBgweF/4++8Y1vjAwMDNwyOjqarl279uOVSmXP4uKirtfrw6tXr14GIOPapWmaaq0DIQSFYQClNMbHx/DK"
    L"yycwv7CANWuWYdeO5ejr6wLNc0i7XYigClANkCUQlQERASQdi0tZ4CaL+y7h80meE7iN9033mCsAfHmYtjNU0OgukCYw3gu4RNAK"
    L"H7ZhZJwSuMYRMQEGYLJJok0UbZggQTBCIKxI1IZKUFGM56cG8OzkTkTDt2H7jhuxfKQf3e4iZqZn0Gy1ABCklGyMUVrrMEkSh07a"
    L"+/nz519Yvnw5giA48cILL/z2Nddcoz7wgQ/cX5AnXzEF8D34I0eOXDU0NPTb1Wp145o1azakaYooiiCEgJQSzIxOp5MyMxljpM2C"
    L"JWltMDs7i9OnT2P03HkICWzeuBzbN9cRRxPgxXNIkxQk6yBZA4kySNYBqlglEBEAAQY7wdu4b62/W3D7rZ7HTOj+5zTPB+CQQasE"
    L"CdgpgO0RAOwt3tiEkLXNCaDY6iEjCwuWYubKRAiHIxBYAJASUSVAdSCCLkV4ZaYfz09vRie+ESs37sa6detRrQRoLc5jbm4OnU4X"
    L"2tLZIITQSilOkoSCIAg6nQ7SNIXDR5AkydevuuqqX9m7d+/xooH+ILfgtVi+68Rdu379+i+tXLlytbNwVSqVwMyktSatFYSQiKMo"
    L"BAHdbhfT0zM4e24U42MT6LRbWLasir27V2Dt8hTQ34VeGEWiGCTrELIOIAQQObcfASIASNqYS9LCuxC5DhMV1NklaCigd0Vd9/bB"
    L"WSgH+3/jkjvyr7m35ow8Su5t2ApV+1AEzy21OQMIDAbBACwBZUHJRBkkiwpBOcHWRhfbNsxiPH0RL5+6H8de3I5w4HqsWLsDI8tW"
    L"YEUlhEq7tLg4j3arHTAbRFEEZjbVahVExMYYTpJEVyqV21955ZVPMvPf+vSnP21ylXwdPQAzSyLSx48ff2Tnzp23KqUSY0wghBBS"
    L"ShAxjDZotduYmZnF3Nw8xscnMDk1g1Ik0KhLbFpXxYqBFqJwDGifQdKcheEAQjZAogaWdWf9dZCsArIMEt71x4CQtrhgY2M/u5LN"
    L"tK0nMB7nz7P/ovuH9wJpM8MJrHfoAqprEULXGLJ3l/wxbAVQzAWyKsF7CcoVIdMXcsolbMXArqcAAUiCjAKUahKlRoQkiDG6WMPZ"
    L"hSFMputB9V3oG9mE4WVr0OhrIAoIRBpJp41Wu42FhUUkSQJXPqdzc3Oyr69v5/vf//4XXksr+jXTwpMkMVYfWAIQi4uLOHX6LMbH"
    L"J9Fud2B0AuYuyrHE6hUS12wiDNSmEcgxoHka6dQc2opBsgoSNZAoOWuPISjO4zxZa2SwhXZJF/yt98nGm2GGCXhTNGxfY8POSh1m"
    L"oI2Np8Y5Ek02u9demAbGCZZ8SHXCLJoNkQATA5fDawoewX524zyCbzMzWBN0YtBsCbSmU4SlLtbUWtgwNIVUnsJ08iguXGxg4uwa"
    L"nORlkNX1KNXXISj3YdnICHbs2IFz585hYmICQRBIKaXodrt3AnjhtcjzNSsA2QtBlldPePjhryNtj2LbphiVFQkalQSD9RYEpoDu"
    L"eejOLNKLTaRGgKkCIaoQsgpG7Ny7u1OQJ3hwcZa0BXiEtmweks5pGTBr1+xRthpg46oCBTYaoTCQkfXAULDdPQkgEIAWYEUgJSyY"
    L"oyRYy+yRtQEriYWWhrFBweoWLfGdRCCvBIItocRpADMXlAAAORWgHFm0+suANki6GsliavOouIu+isRwZR6i/yLSIEKbK5hOYrS7"
    L"fXj++BAuXvVR7N27B2NjY1nZXa1Wfx7Af7yiCiCldIoAdLspWHfw3n0LqIs/A7ebMN0WuvMJmCWIYuu6qQyiEohKAMVgRICwCkAI"
    L"QZAAEwwziA3IGEBqZ6LKZtZwrV2QtXrX42eTuMZPYjEBVghEionJJhYWFmFUCyppwqRt6GQRWrWhkyZ02oJJ2tBpGzq1rWKTdmFU"
    L"FzpNEQmD6zZVEQUREmOHSDxvMEsFKAM+7OcTlnZmemKBCyE+P8i8ibEehtghi8KWnCSQJhqqSWhJAREFkHGKUrWL9bUS6ssSRHoG"
    L"D73yHN7xjluy6oCIMD8/P/da5fmaFSAIgizlTZIUUcjg9stIOqegVAgiaS2dAgABWEQ2qXOWThQCFNqSyZkU2/TZCt+5czLGun0o"
    L"6/Ez6we8/2YveHa9fJOCkOLpE4yLi5uhOEa300WSJuh2EyRJgm6SoNvtIk1TPy2EJLHPlVJI0xTGGBiT4ovPP4hfuX0cA7UIxphC"
    L"m9hpgLvw7H904V240AXNYHLKww48MuwCi3s/MEhQ9jOTMwZhkUfWgEoZJgWSdhcmBaSOISn1QQXGGBARkiSRr1We4rWWgUEQZDlx"
    L"kmqw6SCkRQABhBAQkBBCZJw6osBl8Vk6XXzDLHbbjNn/bMBQYOP4e+zLsq6Fe03Hgj2mk70G04WkFNMzixjvrEGlPgLiFFIyAsmI"
    L"QiAKgVJIKMciu1digWpJoFKyj7VKgHIMLB8ewMXqT+ILj3dRDhlpyuAstyhUH9mDRyhF9gghQLBAEeUlh0MwjbV4pYBEA6kCUp2B"
    L"UJRqUKpgUgVWGiYxMG0N1dQos4FOFtHuJCAiq5w2P8MV8QBHjhwJiEg9++yz71u2bNnNzWZTBYEM0jRFqpoIqAWtXfQTLtsl2Pho"
    L"kRQLmhA7C3Ax02TtIjAMiA2MMRBSW3cunOu3cDGywpu9+88VhHUXJFMkSQcAodWcx+zcPLTW6Ha7ztJf/e49glIKSik0KwsYaqxE"
    L"a0yg2Ta5G8/qx0uLKSpAMSTIfnWybt56C7Y/+/yVjasylK0ifF7hSkkIAUjjcl4BYwiqpRDVNJJOC0mSZMOtQggkSULMTPfdd9+V"
    L"8QDNZrMshAi9m9TaIBQMgWbm4oqZOhvrniwnz2fp1j2yq58542I7JYBl7jIr191LLNbv+/oezNFtwLRcCdjOGkGsOlDu87FTKB8n"
    L"C56s5+6T2967gNIK3a5Bq8u2UmAPA3DBeVEBFvZFAxXwCLIoIQkwXPfQeQhmkem0gSsvEw2TKJhEgbsKnCiYrgZ3FLhroDuMEhtA"
    L"tZEkKvMAUko0m031WtvIrykHUEoZpfL/NEkSNJvzEJzaXjy5jIe0i3cGIANiBgmLnJFxn08K70GthRgGhFMUox2Ww9bKqQC5sU0A"
    L"7WCHypg/rFNAWgaQUilAQJpqaK18/yGL8d7K/ev+73pfMwhhy8xUGYTCxXAncGKXvIHyeYJimeB+LkqDBGCMsN+XyJqfaziR6zrC"
    L"MLSrcoUxoICASMNAgsiAQgYZRpomUDqTBRljuFQq1ScmJurDw8NNZqYfRBlekwJ0u90MfrT1sgGR8EmthcJtiuTKNAFiYRXCsAPn"
    L"HNrGeYkNV+cL390TbAEfreF9hs0LTJYAWkXQjsipLIwLDTIdaNUFkx381Npkyd5S4S9VCN+30O7/NSaFJGf9RcEam89QlsAWUEbj"
    L"C1VkHsNXCiiWkgQQC+v22RmOYbDWtjRkA8MCIhUgMmDSNlQqhmSCSu33IWFJat1uV61YsWJ3kiR3EtGfHzlyJHAF8OunAN46mBlR"
    L"KAGSMGoW0Iuu/e+/n7HNELbuF4KzTNmbhPGOkp35GAMjlEX6tbTmUnDZ9j1tiIDRVsGMzmp/uMEP0h0k3RYQ2J0AXgFe7b5UCayC"
    L"a8sBUClC6fTTKSpxAewrQsCGbGIHh/7x5cFW4X41e5mp53cIwgrfIAuX0AxSDiZhS1NnF9qy/MN5ZWZWV7QM9EpgOEar1UWnOQag"
    L"5co0d6WMZdHAGBDcB2WL6Hlc3X7tgjW7KoHhyj/jS0RtS0L40k/nXsC4537Shw2MbkGlXRDpHgUoCjov/5Ie6/fKrZSGYQNtUgQu"
    L"h/NgI3wVkH1HcmijB4CQlYs+7cgiGFGGoWSyh4OHfULseQVsFd+QdYg2n+YMWGLnwai3SnvNDb7XHALc9gwYrcFsEIUia7z4r2u7"
    L"dboA4LDLbK3t2zwgC3RWgLBZMkMWupoezrWoH7OHfT0Q5D1BCqM1jNQQum1BHaGRJAmM4Uuy/aLQi57AJ4xWEQCjEoTSQdE2nck4"
    L"guwFwbYaNxkEzHkrAOSUv5gbUG/XVgj7PuSSZCKnPA45NOSUzZqNMB6NtUospMiAICK6MmVg8WZc7Neu9mRrFg7rLuLgLsMmDWKd"
    L"sXcENFh4y1fZxbFGb3zF7C6kF7Bxv+uwf/aVhc6UAy7Zi0QbujsDLZYhSZJLcoButwul1GVLP18VqDQFhIROFlCNYQkbgRMpO3oY"
    L"58LNYV/KS0XjK1yCyTMZlzRyjiFk/tCHPAvqmCxPyhtNotDCFN7iOUdoHRiEKx4ClFLQSrmPrpdkvktbo8YhfNaEmHPh+xEskGvW"
    L"ZN0zLgg7dUL2wjYFKLjgHbQd/yXdhuq2oEOFpNuFNtyD+l1O+GlWNnL2HY02gFIIS/kXKsZ8cjxBXwoSs+MpIkPouJgHMFvLBmf5"
    L"YFYGkcOQBYG0sMAn2fcT2WWlnmRzqev/Yaz/h1IAv1FLab2E+psrd5GcQsZY9wbjPIEVGPnxK2f+7OK+B4/YGPhGOjsL9ygh4Ob9"
    L"jG0AEWuLqkGDVAtad5GmCt0kATNeFfDxz71X8x7Alo62HA0og+6tUNi5ZFP4jpnDd2CO9xIO6yEuYAQ+nWG4HkDelYT72d4LiYJj"
    L"HaGnxcCvC1X4h94SZi+YT+R8t6yAiDDbVpwt/sFuOhfka3fKCBYWKHHDGBlopFxip7Imj1UeP+/ncgqjwGzAWsGwQSTaSFszSCIN"
    L"lSZQmnsEf7ns35e2HlJVSoGEgEqaKIdO+JJ70D/jaemGHBfQJWkajilc6FwjxxB4iTAzeMPYETTWxiqDqw/9ZV2K2Anq7dAWga4r"
    L"7gF8Vt9b5eTtT74kJHDewvXunDQgUvcPKA+N7H9PAZyAdZJ3Bb1yuCEQS8hIXZy03kVyB93WLFRV2aaPMj3WXrz7rN8L33sBqxSA"
    L"USkC6aoWP2qY2l4+p+5jGQsIsV7CNCpaK+V/d2lLxOcCAkzGQcgFrASUscyogDrS5dv0b5wCcE+w7y3yfRabIyK+8vddPovzw7hV"
    L"LeRTP86Ez47zB6cAdvrH1ucZTceYnorAaAOh29AqsSGg24U2yGL/UuF7gS+Fi7XW6HQ7QJqAuoBeAJQopCVcVHbKEmAuCJ9AYME9"
    L"lyiXGl3KyxLIaWlCgDTnZSJR3lByj6/X4NCP5AHosiQ7zjB/Xxv5H0kDENqWeuzGt2HyfjqbvMFjkoIHUA740HlPQRdRQQPWGsYw"
    L"ItFB2l1EkioHl+ISt++F77+PMQadTgetVgvtdhtTk5PoaoHl8QKCBDBtBiJkSk7+8xbyH856Prky2zBXIB/SpZcraxWTc+XkOImC"
    L"CpZtk2MWZCnnmcHk/67YVr9iChAEQc9/QkQQIsj0sueycCFz9dZCvu3ru1/skC9f7qh8pNt03d1BvcZZujE5FqA5rwiMhWzDKEVn"
    L"cRrdhi33UpXnAN7lExG01mi1WpiftyzcZrOZQd1pmqKbpECoEUn/Xa3/tgIuwr+MHM8tFkRU8A3oiY2WO0A93ME82JPjSmToEUgI"
    L"SzZBHkKkJAgiGL4ECLpyClCsla2rNOh0UmfBJr8wvIQWVeiT5p/R4wfaZbo6T/yMXd5gJ3482SMv/ax1+UqB3fyeASsDDrpIkwRJ"
    L"miLpJkg1Z9autcbi4iKmp6cxOzuLhYWFDAByc3qOy+Cs0IWxDJV2P7MjcjDl/QDy84PwFQD3Bn/Q0rDfixR6lFCwo5Oz4xiQa5+4"
    L"PMm9po3lJngAyMvGM7aueAhQSqFUKkE5eJR8Vwz+OedlUZYAUS9zMiucfSx3sV93AN0FGxcCTJpjAL4Pa3zDxL2PtgQLqATESU+C"
    L"Nz8/j6mpKUxOTmJubi7j1HuhL/VsYHYhx/RgnPBu3nt0hwVYZcgxAP+nddXuijBf0ibO3qyoG055SDiED3ANNztoIlxo0BzYtTRd"
    L"q7xviAeI47gnbtpBkDDjQ1kGL2V4efaBqIjuFQ2DC9CuW9nmxrvhljixSey8f5bsFVi+WTWAjPFLrBDLBJOdNi5cvIDzF8YxOTmJ"
    L"TqeTCd1bif98l4Q1sm43IAMp0Evm8BZrfJTjDAG0OIH1GMyUGwN8qdtrAMSUIT3knGDWMdKWKkbsaj7hcgFh3zeMIkRhiJa2chBC"
    L"ZJ7uiilAFEW9HD4iaI28rnEACPHSxLAYBtCrDFlHyxXNxtX9xnH+OQE7pm9WAroQ4FuoWeRhA5No6AQ48corOHbsCRimzLX7evnV"
    L"LMVfyCCwtDZB3JO9F2N6xgE0hWTOuedsqMSRBH04IRQSQZ/cUd5GhrBegbRFBZmo0Ir0LGTPLHYAsxA5RFwg7V4RBZBSMrurp7SC"
    L"DATKlapNRIolH+Ey3L8l+GURMDCcAzwFvIBd6xce7zd5Z9Fu+8w9CncBPQ+kKUM3gVa3Da00wrjUk/G/KjXKCV9KCTYWVzfcW+fk"
    L"zZ4lglwS2Tz/r5gH0+WGB4pGUaykJYGFcK1yyy20/4/NBBUDYRQhDAIgp4MhiiJ2W86vjALU6/UwiiJqtVow2kCGgFIBlCoAHFyo"
    L"ANjX/57Uwb3YQU8Y4FwR/M9cwBqK/1znDTXuAnoO0C22Q0IxoRGnCFuEKI6zptX3mnPwwvf3MAwRhiGqIkFQ9NxZ8lZo5GSh3NdB"
    L"OfGDKHcVvGSwxHIDc345ZdzI3mtIruwTQoIkQUjXKAri7IP5HCAMQxJChK87K3hiYoKZmaSUY+12e05KSUprjsIAJGvopAQh8uw4"
    L"m59zgiPiS3DspR4in+ApCL/QWu0xXgGYDpCOA8l5hp6zEKytKAn9oYIggyguuW7l97Z6b/n+HscxgiBARXat8WXYK/kmbwbeEAEs"
    L"hS3ViKyRCuEEJ3Kkj7yLz+M5HPdQEIGkzTtICvtcuvcQApAiQwNFINBWhJRLfkwWQggmIlJKzQVBMAaAJiYm+HVTADdnJq655pqj"
    L"4+PjT1YqFam1NkIIKBMhSW3i1GOlS0ells5N8VJr58uMeBWnb20DnhOGmmAkowwz65oycin2UCCa8vd3+VLK7HkQBO70ECt5IVxS"
    L"WIR1hYvDwrtnkQEy5F5j4co34ZO4vJAnN4AqpDWcSxWj4PKF9QIsrSIISegYAosSpBQgG5pNuVyWFy9efHJoaOjo4cOHxQ86G/gD"
    L"s4Lvu+8+v/olNMbY+BoKMNXQ7krrAS4X933Z1gMdX/ojoWD1xX/HljTCygn+LKCmXRiQPZBCFpZLgW0/CyFxuXH5ovCXWn+PJxBs"
    L"ZeNDfSZI6onLvmogSZkX8FZt9YMgiCFA8I5BCCooTS5zEViBWy/iPItwHoEIQSjRTAUoqkMKyrAApRTq9XrIzPS6h4BCvGRjDDMz"
    L"tDEIJMGggnZXgAT3tim5h9eD3rUaSxXFe9c8RyikeDDzQHoOUJM2F8yK88t4GwahL7RkTrvavbdX7mP+UuGHYeiXOCGOIxARqrKL"
    L"QIqiBnjpubjsheceiTJjRiZgKzhQ7tLJl3VeuEKApLSPRPn7OS/ilYAFQYYCC4kABWVIdw18DhDHMV9RWrizHvZjUswGpXIDmgOQ"
    L"6PYmb7Q0ieMsYbq0UnA8e+berlnKSCcAPVtYCcCvFlry1wLhODa0hJZNlAn+ckrgn4dhBCGls1bKXXRWgrm0zQ2H9pBAhch6+CRc"
    L"ZszCjX/5lo4TtIeMvCJnv8/5RRDIcgJIgSAgdA0hrNSsiTjlVkohDMPXKs7XrgBxHEfG2HFnrRKUyg100ghE7RzuJc5Hq1FoHfeM"
    L"y/ISORYwdWLoOSAZY3CrUIvz5SMJlmTOgrz/oO+Z7bv9fZfkAGFoX5MglwMIF+Y9XOtrfVEAt0TmVG0O7Hu4IkMOBIl8wxgVllL4"
    L"RNN4WNl3l8i5f2kTwUAAoUTCAaq1ukUqnfUnSYJarcZXXAHK5fJF3y9P0wSVah9mZ6PcOooazYUyjwpYgMfUl5qyYCAF0glATVne"
    L"CCR69118jwTTz9zWAkDAIAhDN7BCl038vPCL9yCwp4qBGSWZQArpjN/NP6AgQFfi2feXrg8gMuInC7LNrqxm9GCU30TSu7UkA5bY"
    L"uzrrOXxu4beTLqQR6vX+bDjVcxmq1eqVHQ4FgHPnzv0z10AhpTWq1Tpm52OAXAzr4QSiEAZMIdtfQmjyF6ADdC8CeuYyYeL7Vhf5"
    L"LRQ2qRQuM/Wuf6mrL4YDa/kh/OYyIsqgYPJZfyFby34W0q6tEXZhpC3bJCAlhPRlnX9NgIS0rwUESMoTS+GFLXrKQQ8Kwf0blhJz"
    L"SYj+gYEMjvdU/XK53Hoj+AAio06lCtVaDWdVJZt3s25RXz7bpyWiLyCEegFQ4wC3/fTE9xf05T9gTtv3rn/pfangi+WffS200LHI"
    L"J3x9DpC3bgmUVRkit2h2CWJm6SI3b59PevC2uHmEi5hX3lUlQVb4kiBDCRYC80mE/kYDaZpCSgmtNbnvdvCKK8Dg4GBWmXeTLirl"
    L"ASRYjiQ9DQokWLlhkCXcKM7KOh8COLuYahpQs8jAnFz49ANbfSZ8BmIJBEidkOiyYE/x7i0/CAIIBwQl2oCEhhQlV/uLHPQXPqv3"
    L"KLywkZ8KSyy9oB1BhDI00CWmXBgZL8yS2emnnPtjvQcBgV1HmwoBHTQQRSG6nXm4pdoIggCDg4OdKx4CVq5cGVYqFaG1RtJNUC7H"
    L"SPQQmm2ClPkFQWEypre+N7kCaIaatFAuFSFSr/1LxqYu22dY4mE0A40S0C8XEUblyyZ+l1MG7wECKVAqV1AJFfpKbDeTeYDGAz9C"
    L"uBIwr9UpkFkpB+/yhXBhQNq45MKBECLL6iGdQvn6n/KwQTL//0gSZCQxnxBEPIhqpYyuWxLVbrcRhqEZGRmhK6YA99xzjzl8+LCs"
    L"1WovTU1NPVypVKibJDqKQsiwgcW2gAyk3eRFxVFqP9Tp+/jGIXo22TPN4qeg1xTrXy0p9GcCCyEgv48CeMF7HCAMQsggRBwQokBk"
    L"oEzR6oUQ7ntKkAhc3M7jPwnpsHsvZJcriMLPXsiByGFfKS69B/Z3EAgEcYj5LlCuj6BarUBrDSEEms0mBgYGRBRF4RVTACLimZkZ"
    L"sX79+pmFhYWXgyAgYwyzSVCpr8LUfAUyEJbv57Z8cXH617gJYWJwl6EmAO4UtmwVm0hFDgFy+PSS8Wt+dZ3ws3hLy7/LJYJLlUFK"
    L"J1xXOWRWngna370AZW71PQK0ySBJ5wkCmWH7wiuBWJIkeo8gnVdwSoJQIqiEmO0IVBvD0CrxcjFBEFC73X4yDMPv+rMHrlgIYGbq"
    L"7+8f0o6EmSRt9A+tw/iMqwTgFiF4CrMnazhQ33Ss8E3CvYypnsa7uPQ14AdahMrMiCVQFR0EQWQvfEEJis+XKkKeEIYQuo1KyTd4"
    L"iph/jgb6jD0XnsvyRW7dQhQVhCAy4VNPCOhBEH2oyCoBARFIyDjERCvC4PBqtFtNEAkYY4yUkgA8RUTjMzMz4rWgga9JAbZt28ZE"
    L"xHEcP95qtUBE1G53MDA4iPlmBWwCt/FLFNy5o1fBwHQtns9pNml5GYHS5fvsS8vLy3kpZ/mhBGJKIYIQgkSP8F9N6D33IEQoGHHo"
    L"FEDajJ88DOzeg2TBEwgJ4awdgcxCAYLcG9AS70Ci+OgUwjV/svAQSFAgISIJLSWmWjFWr90AbTSEkNDaTkEPDg4OvtY+wGtWgIce"
    L"esgAwCOPPPI7c3Nzhohks9nC4EANk62NaLcJQoYAW1IFtLCsb7Yxvyh8P1nLxemJQqlFS/lzP0gusGR2n8C2KfM9wsDSUGDDgITM"
    L"kju3npZyly2ckHNrl7kbD3z8t1bPBe9QDAk5vOvcvZAFwdvfYSnBLpcIShE6TFhQQ+hrNNDtdBGGATqdDhlj0N/f/yIR8bZt265s"
    L"LwAA7r777v75+XmRpim6HYFatQxZWonZecJILULqFzj5KVoN6BkCpwwK3KIoMq60oiUkGXJ7dKinIigyZ3KdIfTwot1u4FAQYpGA"
    L"ZGChXFfCfb9qIKODyQAwCUqR2/qVATWi0PKVhXatWNIryLn7/k8/Pehp5Bmz37iFWeSbppwTRHyHMRSIKiFGFxlUXY9atYTJ8Q6C"
    L"IECr1ZJpmpqrr7763xWN9Ip4gHvvvZcPHz4sr7vuusnFxcWvRVEEpbU2uovG4Aa8MtqHMJCADgAO3Ll7gJqT4K6dI2R/mGMRGfQQ"
    L"sSjSaPI1LPkyaHr1CoALOUAkUBUdyCC0TZ0fsBTMFSGA4ATV2C6oFuQTQZm7/cytF7J/cn/nQ4CzeJv0Fb2FLLh7kSd9WZlIEIHz"
    L"EoEAwgBhJcIrF4HBkY0IAwGlLAg0NzeHZcuWCQDVH8aYX2sSyJs2bRJEtMDMD0dRBGOMabeaWLFqA85PVWA4AETohC/AzQDctXNv"
    L"zMU9vwUa2NJhEiomf/T9y78lb0E56FjgU/aigT4nuAQhdMlXIKWr9Slz/VkL1ylBVqfLoFe4vlTMBN0b41Fs8/qs37+eMYDc+wUS"
    L"IgwgohBnZ8pYs24LFhfm7a5iZsPMKJfL3wIwy8zi0KFDfCUVALt371ZuA9V/uXDhwkwQBMHs3DyvWTWMseZqLDQFpLRbQqFCmCRw"
    L"+Zyf6XMDHUaD2CzZv9u7th20xCMsVRReEg4Kws6UYYmgL1cNLPUKggS0VpD+Z+njfV7GkQwcSJNbdp7wLcH+nUewll4oFb3QJfUQ"
    L"TqiAAUBKROUQsx1gvDWEDevXYG5+DmEYot1uq1KphFqtdj8Rzd53333Ba+UDvGYFcBeS77777rEwDMtKKep0uqhVY5T7N2N00oIq"
    L"bEK77ElQYYOMXRWTbQPN5v2X4ABZMigubZ0yXQoRF4TPIARSIBQq59td5n5ZwQuBQFgMI+20UIoCO6PnY77Ms/vM4oOCwLImjrSN"
    L"HpmDPEQ5zy9n/BRLTHedJPUCRaFEXCvhzASj3LcZtWqMdruLIAiwsLAgpJRq+fLlk8xMIyMj5kqHAADgw4cP+y7IZ1wzQne7baxe"
    L"uxXPn6shDAOAQzDJrEVshyn8IIfO6V4ZBzAfhMzYlr0nNhT3cRYZKpd4ECJ7rhiDLyv876UQnswpXRwGudW3krJyL/MEhYYRCmVh"
    L"JnhfNnqF8b+/NBQUSaEFBREOAArLEY6fATZtvTbbeyiE4Lm5uQDA4rp16/4DEfH+/fv1G6EAuOeee4iIVBAEjwghlDGGZ2ZmsGnz"
    L"RpyfH8RCWyIIfd2cW7A1aDfH57eDFRQgEyJ51szlPMAPDFihSAh+VYG/Sm7AbnWLyGK37HXPRQSvUAqSFPZwqwL4k8G8jtblm1S5"
    L"d6QcABIiv25BgKgcYz4hnJsbxPbtV2F2dsYfEoHFxUVeu3ZtB0CMH/L2QykAESlmprvuuutPJyYmxsMwDBcWFs1AXwVU3Y7TExJR"
    L"KbR9bM+Jy8LAZRLBYu+XUCD9ee9Bl5mxv+RD5UQMBqohg6Agg+CyYezVnvt9/u1my1pgBvT4HsASyyeHCWQIoTuXwLN5ikTSbDEU"
    L"9Xo4txvA8wEhnfeREnE9xskxA5Q3Y9nIABYWFhHHMaamptTIyAj19fX9FhG1mDn4YU4bFT+s5hw6dIiIqBMEwaNJkrDWhjvtFjZv"
    L"vwbPnKkgKAUgnx1TcXwK2TrYXi9geoYnM+tHrxegpZDx0gQQtt9UCRnCKFvTvwov4HJ3DzQkWjtXT7lwl5SAXjmEyC0YS5i8cKPd"
    L"KMT9bBNeJnjkoYF8SShBkURQjvHkK4wdu25Gt9OCUtb9z87OUqVSme/r63vyh2BN/OgKsGrVKummUf5LGIZkjOap6Wls27IRY62V"
    L"mG1JhHFgvUAh0fGduvzMFpMjgsyFKWLnPTIFWAIaFWM/CnmCm8KthQaSGOU4so2TgqKRG2Unt5uwyPuMowDGGATCQq1EeULmaVnk"
    L"UTsvdMoh3cx9F4CiDN51zOGcUOrDoiORIFcaigSiSoyZJnB2dgV2Xn0VpqenEQQBkiThZrMpq9XqzPLly++3+k36DVWAX/zFX1QH"
    L"Dx4Umzdvfm5+fv6slIFcXGyaaiVAbfm1eO5cgFLVnfjlGjI5kFdcAmm3h9lOYS/okzNxRG914IXPvV+BAEhBaKWEWzcA6dTLEPXV"
    L"WL5mM/qXrUVjeBVqgytQbowgqg4gKPdBxg0grEKLEhRCLCQSf/3YN7FvnYIIQwv7Um/Slyd3vUmgtd4CvaswM5B/dp8H5EShjAqe"
    L"5R0SEAFK9TKOvZSif/l1GOivYrHZQhzHmJiYMAMDA7RixYo/PnjwoGDmH1qOP/SWMCLiw4cPi61bt5793Oc+9xtRFP37KIrM7Mws"
    L"rrv2Wvz1A4/g5i1diDiA0dJuBCXLBcgwAUYPnSo/qbtAwPDuFwU6Vg8WwAWCplUiZYCBaoB/sX8eX/jOE1hAFR0FGE1Aip7eAxUU"
    L"TguBJO3gA2sM9l3V7452lT1QMAqAEHlhQmRru/xEkafG+/4MiRzqzejgfrTcbyJ37p8CARGH0CLAE68EuP2D78D83JzfbmLm5uZE"
    L"X1/fyb6+vv/0o7j/H0kBiu3hZ5555ssXLlxAmqY0PTODjZs249HyVXhlbBqbGhGanQQktN2Z7w5ZspYPtzPIJ3q9ZwFyYUyKhF3H"
    L"BojC2lm+bE9QCKCdMm5YE2HnCsbo3CJaiqD8eb+ikLD14PmMUqmCUjlCGArUKwGCIJ/7gxAFcmgBus5Yw1z4+JTtv7jcfFo2I5Ed"
    L"OZNzACAlSvUyXjqXIgmuxsb1q3D+/DmEYYhWq8XGGDEyMnJqZGTkvD/O701RgAMHDugjR44E+/fvP3v27NmvlEql93a7ieq2W8FV"
    L"u3bj8aefxvZbu6CwAygFkk7wZPcD5bmAXRBNPVUeuaPZvBeQyMeC0btbx79aSAYlCMoAigWG6gEaRrizfoTLSyir5637zTH6IBQo"
    L"l0OUYgkZFIY0C8lCBg0jj+/ZjKB3Yuxe94ueCyvjhd+0RnmCykLYvCMMEVQiPPxMimuuvxVsFJTSKJfLOHXqFDUaDdqyZctvvNbT"
    L"QV7XHMDf9u/fb4ioFYbhP2+324KZ5fjEJHZetRkXuxtwbi5CqRq71mYh4SF/0KLb+esJkZlLp8LRbMKd/3Yp6HPZvoGxniIQhEpI"
    L"aMSEgYpAf1lgoCqxrBFgZX+EFQMhljVC9FcDDNQCDNUkhhshBushqiWJILCUMh+GqIcQktfzXkF6wkHPXoDeXUFEl7a/2SeKgUSp"
    L"FuPshMG5+TW44bqrMT0zgzAMobXmqakprFy5cr5SqbzgMBl+UxWAiMzBgweDu+6669Hp6em/kFJSs9nUBIWtV9+Mx18uo1wrgYKg"
    L"d+TZzwJmXEFXEWTTQ0sOYMriq8iVgqjX+qlIJrFhRbgZPikIw/UAA2VGp9PGzNwipmcWkXbbWFYXGKiFkKFEGPqWsCgcfiUuGQ/z"
    L"x8oUX8+rnHwzSN6Vys9R4mxYBjkS6CaAOAgQ1kv4xtMaW3bsQ6kUottNUCqVMD4+rgcHB8XQ0NAflsvlk0888URAROZHkd+PnAMA"
    L"wIc+9CFyh0o/ppT6CSmlvnBxXN543Q58/tm1uLjYRl81RjtNQdJkq2QEu01XDGSnLtJSXmBeElquoS5QqT0DlPPdutkJXcj2BQRS"
    L"ohoZ/PlTczg6uRxB32qkogqQQBmLSGbP4G/tauJd1w5hIZGWnV5088XqQ4gsJ81HwItrY/L9wX5P0OX2A7LbAJrvALLkk6gSY2IO"
    L"eP78CD723huxMD+frX05d+5csGbNmmTTpk3/xrF/1I8qO/F6KMDu3bsVANq/f/+nW62WUkpFi4uLXIolNl51Mx55sYxSowIKQwug"
    L"kMgIH0TFExkKtHFTqAYISyw/xwYybj0KKJtTEMOEUhSg3engU4+X8Eczd2Js6D04a9bjfLcfE3oQ05VrMLf6I/i9k3vxb/9qEc12"
    L"F3EYuAS08L7U+9iDbmUgpPsutKRBtUT42dwfF76XFOAgQNxXxpEnU6zd9A4MDdbR6Sa+9EsrlQqvXr36dwCcd9bPbwkFICJ2LeIp"
    L"IvrFTqeTSinN6PkL2HvTdXh5ei3GmmXElZKjUlmShU3EONsSmm8CM/lxSxn4soSUWYz3GQiUx1YrfIEzky38+pHleEzcgXZzAc8/"
    L"8TCef+YYTr70HE698gJOvPAMZs6/hMbwajxJ78P//T8UJufbCL0SiCULG/yQCImeriUVFwkUdiXmocmdOwgujBcW2sGBRFwpYXqB"
    L"8OzZYdx22y1otlp+eonPnDkT1Ov19s6dOz9JRPzFL35Rvx6ye10UoJALiA996EP/dXJy8ojWWiwsNE0pkti661Y89HwFlb4qEISW"
    L"6y6K8dxOFOdNIpOdz+NWaCzZxmFPESfO5/SoyCuEXcCQJgn+1bdGMNN3I57+64fxwgsvIFEGQRghikuIS2UEYYQkNTh96iQaYRfp"
    L"yvfgX/5lG2mqskmebL6fCsMrS/PPHkILF/YjuhZ4AenM8xw//i0z6//K412s3XwbVq4YQrvdQRRFmJ6e5jRNad26dacBXGRmuvfe"
    L"e/ktpQAAcOHCBcnMYmBg4FNKKWI25vz5i7jlputxcnYdzi2UUWqUgTAAh0WuncdE3NEynijiFyj1AEJ+Vs8miMSF9dtOMJoJ9Zjw"
    L"F88T5gZuwpnnv4XFZhulUmkJ7u8SoSBAuVzBzNwChusCZ+h6/NWTswgDAY28bZt18YgKLn9JVl8cU0NhLsLvVy4kfkw5A6hUK+HC"
    L"FOP42eW4/V37sLjYzD7byy+/rLds2aJHRkb+mXtL+aOAP1dMAVauXKkPHTqE3bt3v5QkyZhSSiwuLhpihV033IYHnq2g1KiDosg2"
    L"ipa61uxmsuPe85XpMu8LQIDs9L5j7KLHHINAYnq+hUdbV2F+bhZjY+OIoqhnO+hSIghgF2HOLyxi/do1eOxcDQutxK2KoyUJaSEX"
    L"KCheln2ywzmY8+1opjDwiXzrJwUSCCNEtTL+6jGFbbv2Y3iwgXa7gziOMTk5aZIkkQMDAy+sWLHisO/Gvl4ye10V4N577zUAxNq1"
    L"a1+u1Wq/mySJAMBnR0ex98ZdGEu24YWxMiqNCiBDsLTAh3evfuVqdk4g5RmynbqV7hxikXcFqffQKmZCJIFTMwKzNIKJsVEL8ADf"
    L"czOInwwWRJCSMMMr8cLZRQhB+SaOpSvflx4pf8lENBeWZqIwFOs8myOZVBplnDiv8dLF1bht3x4sFKz/hRde0Nddd50YHh7+50Rk"
    L"HnroIfl6yux1VQCnBOrw4cPR7bfffnB+fv5+AJQkqWq1m7jtXXfggWcboEodohyBohAIA1AQAGFoE8SAwNJv5zYOMURhIZPI5vE9"
    L"rZyyElHAMBBLg+dmyuhQBdMTFxHHpe+5HqY4HCJEgDgkLKoKvjuRgpnsUvIl4FS+ursXvfQezH9sf8gmZzuQ0LPsQcQRKI7xhYcN"
    L"9r7jLtTrFXS7XURRhHPnzhkAYb1e/2yapn9x+PBh+cOwft5QBXDolAKA/v7+fzY7OyuUUuL8+QvYsW0DqO96PHaijvpgDYhiqwBO"
    L"CcitZyOSblMmZWfzCOe2WQY9s3lZYgh3Hi+sFc92JRIjkHS7lwg+mwQuzAJmzwP7nGSIVEukSmerX3uxicJIMl86ncJs8t2Hxh73"
    L"mp0w7mp+iADV/ioef66LRVyLW26+HouLLR+S+MUXX8TatWsXtm7d+o/WrVvX9hXXW14BHCgUfOADH3iUmR9M01QYY9TFixdx13vf"
    L"jYdfGcGMGURULYGCEAg8ecS1jl3p5ZtE2Zh41oot8PI8TIscl9cssKKiIHULtXpfz5LopcshLlUECSlDlEN7Ilpa2HJChaXYOXEF"
    L"S0aSkJ2ECmPsKnuy69y4aP1SIqjEaOkAXzlWwR3vuQsEu4U9jmOcPHlSV6tVsXLlyj8EcOHYsWPhD7r7701XAMBuFyUic+211x5K"
    L"koSTJKHJqWkM9FWw5ep9+NKTZVSHBkBxCSIMrQKEoXsuHJ+CCr1yssOnJB0lO1/NUlyuKEgg0QLbBxJQsoD+oWGwMT2Wno2Cu5Ww"
    L"xRHxKIrAJFGiFkYaEqmmHroZinuCitbvV9ibIsWZ8yMECwxgO+wRojxYxRcf7mBw1a24aut6tNp22idNU7z00kti1apVydq1az9J"
    L"ROzANrxtFODAgQOamcWWLVuOlsvlB1qtliQi/d1TZ3D7O/ditLUR3xnrQ22gDoQxEIVOsBEgo2zuPjsey4+RZa3ZwCWEsrBp07KQ"
    L"UyOwdiBApTuKev8QgkBe4u698P09iiLEcYxSHEMzIeycwYqhsutevvqMis/2bevXHY7lTwLnnAQrPLklEEAQotyo4OQ5xlOnVuOu"
    L"9+5Hx51WEscxnn766XTHjh1i7dq1v1upVM4fO3YsfL1d/xVXAH99Dh48KNavX/9hrfVou91Gp9PhxYV53HnXXfjLJ6pI4mEE1Qoo"
    L"ioEwBgURKAhtaJByCRZfQOZg2bdWEWRPr14boK9WxQfXjqPdSbBixXJopXqSveJzrwRCANV6H8bOvoLrVy6iFIWIAsf5K6CPdJli"
    L"n/wqHH+sTfZXPu7DjXkHWeJ339cM9r3rgxgabCBJUkRRhImJCTM1NRUODQ09VS6X/wUAvF6o3xuuAI6rLrZt29Zdvnz5bwohpFJK"
    L"j56/gI3rVmH5hpvw5W+XUB0eBKKyxQeiEBRGEGEEGYa2fxDIwuFIonc1i5B2FM0NdMCNgy8mjLuvruC9Q69AlAcxPDxoLfFVEsEg"
    L"EOjrH8TU9AzW4SlsW11FFElEkXBOaAkFhYunh7mj3/35BQbZOQbZMTRSgkIJI0NUh2q4//EE1LgFt+y9Bq12N8tLnnrqKXX11Ven"
    L"AwMD/2TFihVjAKQrr/F29AB46KGHzOHDh+W+ffs+EwTBt+bn54mIzMnvnsL779qPF8ZX4jsX+1Eb7gdFJSCMQFEMimKIMAZJqwQU"
    L"hAValnTzeLZsy7Z2UD7DL5yH+KW9wDuqLyClMgZHVqJeryEK7S6gMJAolWL0Neqo1Idw8eIYqhP345ZNAo1qjHo5QBiIHPQpjqIV"
    L"DrzgbA8CZwTXfBEUFdbBhqj1lXH6gsHXn1uOD3/wJ+wJpQAqlQpefPFFXS6Xo+Hh4eNr1679kmP7qCtqpHgDbswsiMicPn1658sv"
    L"v3y80+moer0erFq1EvMLbXzlL/4Iv/ZhhmiNQTU7QJICqQKnCkgT6DQFlAJrdmPnNs6SVoC2ZwqzUYWDKfPkS5NAK1G47+kE35hY"
    L"Bepbj/6BAYRxGaU4RqUUod1ahJk7iXXBCdywIcRgfwXL+iP0NSLEkXQlm+PuZVwGZK1o8mcp6hzzz04IEXbGT4QBZClG0NfAv/2s"
    L"wfW3/TzesfdaNFsdSCnRbDbN1772Nd67d+/EmjVr9h85cuSV5557jq+k9b9ufIAfpCxk5gDAi6Ojo//X/Pz8J1WaqvPnLwQ7rtqO"
    L"TVffisMPP4yfe+8yLOpxSwWDpYMxSQgKYEQCUtqSOo2xM/Veh/3Js6ZwdI1he94PCHEY4eN7I9wyPoFnxsbx4rjE2CKjJQhBQ2LT"
    L"ELB+ucDyoSrKlRhDjRCNSogwWHrOT4GE6k8O85R2U8AACuRQuwhCgoMQ5cEaPn9/B7UV78W+W65H27n+MAxx9OhRs3nz5qC/v//3"
    L"RkZGXjxy5Ehw77336isuG7xBN9cuBjOLr371q6dardaavr4+E0WR2Lp1C/7gDz+HmzeN451XLWJxYhbU7YITBaMMoFJwmsIoBaQp"
    L"WKWuznaWbwygU0C7djJUHp+F5QUkTFAgdDWjrRhdBaRMYBKQYYQwDlEuSdSrAcrlEHEks10+JIWFqKW4dPY8O8DKZCeJWvqCc/2B"
    L"BIcB6iMNfPsE4b6jG/HLf+/voRRHSFOFcrmMJ554Qs3OzgY7d+584Kqrrrrr+PHj0a5du5I3Qi7ijVIAzxk4dOgQ79u37zat9YmF"
    L"hQVOkoQvnL+An/npD+HBbwc4NT2ASn8DHFUsRhDFQFiGiMqQUQkUu3voqgUZgWQIyMi1miP7mgiAIACkzfbLkUQlkqiVQvRVSxio"
    L"lzDcKGOZc/fL+0IM90WolQNEYWHxgwek6DIbyrRz+9o1gFAc83bCD0LEtQqmFgIcPlLFPQf+Dhr1GrQ2iOMYY2Nj/Pzzz2Pjxo2T"
    L"jUbjHx88eFDs3LlTvVFyCfAG3nwoIKLTjz/++P+zsLDwJ+12O5kGokajjve9//343AP/A7/208sQ18eRtCSIUkhhYIIAlFoUz5AE"
    L"hAZUApACGW3RNiPd6aPCnlFcONKLhP2yUhBKTqicbfOya2FEWKCAF9faM+zCC+P4/X4VvvFhwNH7yVHC/EKJQEKWQohyGf/ljzVu"
    L"ftfPYMdVm9BstrPV7t/4xjfUvn37wsHBwU+uWrXqW67mT98omQi8wTciUkeOHAn27t3736WUv91utyNmVqfPnMXWLRux5eqb8dn7"
    L"FxH2L4csVUDlCqhkqwLEVaBUA5XKoDAGhSVQWAKCGAhLFkNwWIL3BnB4gl3UEEDIEEIGCMIQYRQiCEIE0q6K9dUEeQZwceNZcTs4"
    L"+4Mg7SZRdwAueraJBhIIIlQG6/ijv0pQW3Un3vfed2Vxv1wu4+jRo6qvr0/UarU/Wb9+/aeYOdizZ0/6hsoDb8LNVQXcbrfXHzt2"
    L"7Inx8fHBkZERllLS9u3b8Mf/7b9jXf8YPvLOAO3JKUBpsFIwibbP0wScJjY3UPYRxsBolxuwAmvtVtJYU81c+FJev5/1C0ThNA8/"
    L"qGmHNrlnMMmRVYwudIUZ0h0CBinsUucgRG2kjiNPaHzjxV349V//lWytW7lcxnPPPWe+/e1vi1tuueXM1Vdfvb6QJ/EbKQvxZiiA"
    L"CwVULpdPrVq16v1RFE3Ozs4qZuZTp07jowd+EsfPxHjkGUJ5ZBgclEBxBaJUtnlBXIYoVawniKuguAJEJYioDIpKIJcziLBsj1fz"
    L"eUEQg0TsMIUQkKGbXcxxBEiLLjKJHFyCzDkHLMDGThjBbf0mIhj3+zIIwDJAbbCCZ04w/uqJVfiFX/gE4jiCMYxSqYSLFy/y0aNH"
    L"9XXXXdeuVqu/wszkjeKNlsWbogBeCY4fPx5t3rz5Wxs2bPjP9Xo9XFhYMK1WC5OTU/g7P3sPvvzXHXzndIzKcD8QxBBRCSKOQWUr"
    L"dBFXQXEZiCuguAKKy6CoDArdPbZKIaKKhZkdoETSKoGQIUQQgii0sLIMHLwsbBIJCaZcCZhz/oFtWRME2dYxBRIiCMAyRKWvjPOz"
    L"MT77QAMf+/jPY/nyYXS7CmEYoNvt4ktf+pK66aabwmXLlv3d9evXf/Ghhx6SPyq//20VAoq3Y8eOhbt376ajR49+Znx8/J7+/n5I"
    L"KeWaNWswNzePP73vMH7xp1dj/UAT7YUWSNtVc6w0oBSQdmFUClbKgkRKAdo9N5yNhduTQ/yBVAQIe0CzJ3Hma19yDqDJ4OdCd88R"
    L"O6y8GJDud9xC51I1wryp4d/+kcJ7P/gLuP1dt6DZbGeQ85/92Z+Zvr4+sWPHjq9OTU19GEC6e/du9WZY/1tCAXyeLYTA17/+9Zmx"
    L"sbH+ZcuWGWYWGzasx9mz5/GVL/9P/OpH12GkNo/2YhfCaHtUvFMCVqlTAnfMvFZgpUCsXFfOTh15UmYG1mbMHi6MfOfTvBn509PU"
    L"BcMQgdi45iS7xRAMDgRK1RBpUMO/+azC9e84gA/9xJ1otjsQRIjjGA8++KCemJjAzTff/A0hxN1bt25NrgTJ47Xc5FtAAXD48GG5"
    L"Y8cOuu222541xvz0+Ph40Gg0eHp6hrZs2YRypY4///K3ce2OlajXDJSyg51M+X4eIQp8waxD6Ikm0rnpwPUQQrfTJ3R/F7hd/7Kw"
    L"1En2DKLY/8/yDYS0gy2+hORAIq6G4LiB3/p8B5t2fRg/9ZH3o9lsQxChVCrhscceMydOnBA333xzd8eOHbuGh4cTAOKOO+7gN9v6"
    L"3hI33y945plnPjA+Pv7n3W436u/v51Qp2rZ1C5586hkce/xh/OrHtmKwvIBOM7EUcm0sApha92+MrRhgVLaVNF9BY1zt7oZQyB3V"
    L"QfnsVj60w26XQeG58xYs2BGRrAcIKyE4ruE3/tsclm28Gx//2x9Bq92BEAKVSgXHjx/no0ePmrvuuksKIX5h27Ztv/+jjnX/2CmA"
    L"zwf27NmTPvroox+YnZ39PBGVG42GTJKUtm3bgm8/fRyPf/Nh/OJHt2DVYBudhY7twxttt0Io1xjSrgw0Fhomo9yx8fZ6kzu0wq+n"
    L"yc84zCeSmEw+6gWrNCTYDR4bCMFgIVCqBmhTBf/+802s3PQe/OyBD6HV7mYdvtOnT/PXvvY1vW/fvqDRaPz8li1b/uCtIvy3nAIA"
    L"wEsvvRRv27at+8QTT/yWUurXZmZmkr6+vihJEmzbthXPP/8SvvrgA/i5n96MzWsNOvMtKyNtMQKwAXyiqN2R80Zb4Mahg35lLWWd"
    L"Ozew6mp8yhK+ws9usQWEVQRNhFpfhOlOCb/53xaw44a78ZEP34W5uQUQEcrlMi5cuMAPPviguemmm+Ty5ct/buPGjX/IzG8o0ve2"
    L"yAGKt0996lNm1apV4Qc/+MFHxsbGbtFab26322mlUpHj4xPYunUzVqxcic//+WPo7+vDuvV1mFQVljHKnufF5c427hfKPJcrULb3"
    L"t7AKVsgsL2C/4VPaMS4jBGpDFZyeDPEbf7yAm/b9JD7w/v2Ynp4Fs631L1y4wF/96lfN7t275cjIyCc2b978mWPHjoWrV69O30rX"
    L"+y3nAYqIGDMHTz311FdmZmbezcxptVoNkyTBhg3rsTC/gMP3/Tn2XjuAn7hjBKYzD5Voew6Fp2Vpt3xCWy/gp4/ZuNfZl4mceQbP"
    L"+RVunxHDgMlAkM0hKCSUByp47JkUf/Jggg9/5KO48fqrMTE1A+Esf2pqih966CGze/duOTQ09Int27d/xoe3t9q1fksqQBEuZubo"
    L"2LFj/3Nubu49RNSpVCqlJEmwfPlyxHGM+/70C+ivdPCxD29EpdRB0uyADAHQtktnrGBZu6ljpwD5ylru3WKe5QXGLbPWgLATS6Wy"
    L"AJUq+NMHpvDkiT587OMfw4rlw5icms6y/ampKf7mN79pbrjhBjkyMvLz27Zt+wPXAFNvxev8llWAohKMjo6Wz58//4UkSe7sdDqm"
    L"XC5TqhT1NRpYtWolvnL/13Di5efw0Q9uw1VbIphWEyq1cd/Hfit0tl4B3LOvmI3fVqrzHIEMiAwMbM1f6ivh4gTjD75wEXH/Lvzs"
    L"3/4pMDNmZ+dsu7lcxrlz5/jJJ5/Uu3fvDpYtW+YTvrdUzH9bKUCxPGTmyuOPP/73RkdH/9XQ0JCMosgYY4SUEhs2rMeJE9/F/fd/"
    L"FTs21/DhO9ehUupAt9swChYAMjrbScR+cCMLA8WllTqjeoM0ytUQEBHuf2wGXz3WwTtvfz9uu3UPpqZmsgMby+UyXnzxRX7xxRfN"
    L"rbfeKmu12s9t3779D9/qwn9bKEAhJwAA/trXvva+NE2/SERBrVYzzCy11li5YgXCKMJXv/Z1jF04g/03r8TeaxuQQRem1YVSzqUX"
    L"mLzsFlIwtMUMYJ8LAcSlAAhCHH95EX/5jSlE9c340Affj4H+Bi6OjYOZEUURgiDAU089pcfHx3H77bfLMAzf0jH/bakADi7Fs88+"
    L"G+3atSt55JFHPqC1/jOtdVwqlRQRyTRNqVwuY9WqlbhwcQyPPPwodDqLfTcux3VX1VEuGUAl4KQLrTlbXc+woUCCEYQW0zda4jvf"
    L"XcSRb01irjOAd73rDuy6egumZ2axsLCQufwkSfD444/rKIrkLbfcgjRNP3H11Ve/bYT/tlKAImx84MAB/fTTT28/c+bMPx0eHv47"
    L"aZoiDEMGQEopDAwMYHBwACdPnsaTTz6NbmcGG1eXsGNzH9Ysi1CvEogVAJUd4tRqG4xNtvHCqSZeONWCon7ccONNuO7aHeh02piY"
    L"mMxKvFKphNHRURw7dqx7/fXXxytXrvyqUuofXHvttc/4kPW2MSy8DW8eSWNm8cgjj3ym1Wr9dBzH5VKppIQQMk1TAoCBgQHUalVM"
    L"Ts3g5ZdPYGpqGtAthDJBoxZBCoYxBgvNLhIVItElDA2PYOuWzVizZgWUUpianEKqFMIwRLVaRZqm+Pa3v62np6fpzjvvFOVy+cGp"
    L"qamf3LNnT+vIkSPBHXfcod5O1/JtqQDeE3je/LFjx26cn5//H1rrteVyGXEcKyFE0Ol0wMyo1aqo1WqQQqLTTdBs2TPqiQQMGxCA"
    L"cilCpRKDiNBqtbCwsAhjDMIwRLlsD6E+deoUv/jii7x582axatUqvWzZsn88Njb2H96uwn9bK4C/HTx4UNx7771mdHS08uyzz/5S"
    L"t9v9lb6+vg1aa9VoNIiIKE1TobWF3u0coN0Gkq1zMAZKaSRpmo2Se1fPzLhw4QK/9NJLRkopr7/+eqxZs+ZLrVbrU1u3bv2KP63z"
    L"zWzp/v9aAYpKAACnT5/efP78+d+YnJz8SSEEarUayuWyiuNYACCtNSVJAq8QALIB0SiK7IFXzFhcXDSjo6M4c+aMieM42LVrF5Yv"
    L"Xz7V19d3z8jIyBEgb169na/dj4UC+FvRDZ84cWLv6dOnD01NTW0ql8vbtdaIoghRFKlyuYwoimSpVCIhRHb+7vz8PCYmJtTU1BSI"
    L"KGg0Gti+fTv6+/vH4jj+r4uLi7+zffv2k8ws7rvvProSCxv+RgFeB2+wc+fOTDjMXH7uuec+Pjo6+g8nJiZW1mq1WrPZRLPZxOLi"
    L"Iuwp6AZRFKFer2N4eBhDQ0Oo1WpnS6XSS+vWrfvt8fHxJzdu3HiqCEz9uFyv4MdNAXwocEfbgYjaAH4vCILfa7Vae+bm5u48ceKE"
    L"IqL/tVQq7Wo2m0YIQeVymcIwnCmVSv86CAKsXbv2d4hovlh5HDp0iH+chA8A/x814S/KSRvKzwAAAABJRU5ErkJggg==";

// The banner shield of each state of the request, all three embedded at the pixel sizes the
// dialog asks for:
//   0  the Windows 7 shield - the publisher is verified;
//   1  the blue shield with the question mark - the publisher could not be confirmed (a file with
//      no signature at all, or with one that is present and not valid);
//   2  the amber shield with the exclamation mark - the prompt itself speaks of an unknown author.
// The art is used exactly as it is, tone included, and the 16 px tier keeps the verified shield:
// that is the one the native caption shows.
// The user-supplied Windows 8.1 shield, with the white background removed and alpha preserved.
// Three icon-resource sizes match the existing 16/48/128-pixel layout tiers.
static const WCHAR kShield81_16B64[] = L"iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAADE0lEQVR42nWTW2wUZRTH/+f7ZmZ3dvbSblnaKmUhUrLwQPrgBRI1"
    L"buINIcTELIlG5QETE17AF4lPDYkhmhhfGkkwfcJ421FMlGgRzG6AEMlGgpKltkCRQlfptkst7ezOzsx3fNhuU2+/t5Pv/M93cs7/"
    L"ACtgZioUClo7/rZUeq5UvPhUOy4UCtrg4KBYqaFlMUACYAbwwchI39Zw+Ejkzm87S7E6eq2NXwzILftTj6Yq+AfU/pmI+I3JSfOl"
    L"Wm1PhxSHNtT/XH3r/Hfq/eQYuuL9ImNsme6zNrzXK7q/TD+WnljSMjFAdj4v7rPCz/cKei1maM+k1q8jrzoTVK9ckMMdV3CtWgmk"
    L"H5IPJDdhXXxjI8GrPtm1Y9deZhaCALZzOSSY9q2dmX5Wu1pWC+Nl9m5cld4ft+HAgRk3ZSNwuHjtlDty/Xh4zrn7UKt9Yg0AtgKG"
    L"p4Lu2th4wFxHMF0hcfMW7s3cxq9dHu4FTTTnXJqtzmvufJPdDtdpz0ADgCjgE5EuvKb0FueUNz4BujiKxYiPyW1RwAxB0wEXdXiB"
    L"T2Y4vLwJwQx6ncjzNWNRNyVoaor9n0cBpw4jGkXEjMKKWAjFLWiWjo5YEvFwbBYA8nZOaMViQQJZfzZklj8znxy4Hu1n94ntUApY"
    L"n/Yx/PhJSJ3ALsPpdlmE+rgSTo4CQCq1j5ZN05MIvqrHOl++FBKkxRvwiBDtcZDpjAG6BjgumpYSk1o/OXr6BABUq1XWstlswMwE"
    L"2CcenuDRcqfMzN4NAkEkUffhKgO6r6DqC0EjtFb8jkfKv6j0mSXvKAGAbdsWRLubA/fj4PYHE6SZlgqRDsURQHZDsAHyfTWffJVc"
    L"Y9PBQ1nybdsWLQMvkc/nJQB8f+qH4Xe+HuMXhy65b318QTWm31U8dcBtVPL8Y6n8YSuX5b9vgZls2xa5zZDFWs83ta7NT/uGwgtr"
    L"zqLprMLlicTpmzfGdwAIcrmcIiL+W4GVN8FHj+rnMpm3vWjslU7paIsLxqc/ff7Rm/uHhtx2Dv6P1kBbHD98uOvYkZOr/+utzV8A"
    L"nmfI5552dwAAAABJRU5ErkJggg==";
static const WCHAR kShield81_48B64[] = L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAPmklEQVR42s2aeYxdV33HP79zzn37zLzxLMZ2YsdJnMVGCQQXJRSS"
    L"cRdSo6ykHqmogNJWdUtRI0GFkFA1tlrxR1GFSqtWieiGQKXjtCLBNIS2jKPCH4RQkWYhzoodr7O892bevOXee8759Y83TiFNYrsx"
    L"hfN09aR7r+4539/6Pb/fgQs8VFXm5ubca91TVeFneczOztpX3TKAfRUYcyHndBfqQ3Nzc27Xrl3+wQcfHB8fH/94mue39Xq9JBFL"
    L"UiwuZFl/9v6X7v9LEcnPvHsh5pU3bzIz5r7vnbR7d96Xf+vfH35vKFTuqw0NbYmrDYgZSwaqSYXhZJRGa/F7HTq/d8sv3PKd2Sdn"
    L"Cnt27MtFRH9aAGRubs6ekeQ3v/WdT5US+8cSewyfnPdjR4+bE9ki/7rVUbJFnSxtildt3J4sL3WWG6uNj77/9tu/CDAzM+P2798f"
    L"AP3/AaCIokZEAsADDzywtT458blqpXZLvngqjq82GDt+ypiFRQ4nK3zl8gAaKEiFDcnWcPVb3mZLpszqcuMfu2nr47feOn0ckNnZ"
    L"WTM9PR1+IgAG0eOAgadUZH8EmNmzp3DbJz+11xj5o+D9CO2G3+zbrtJcJG21yRtLHPPLHNjWJ419JFqMlplILta3TlwbLx692Dab"
    L"rVPzS/N/2Ly0+fd7d+7Nz/jS1NRUBPRczEveeNHw6o984yvf2rjpyk23FYrym2T5zlZjkZqEsMU5a5YXSTtN8uYSsrDEsdDgwJU9"
    L"2r0uIc+JwVGIVYaTUS4ZvTxcPf5W65ICi/ON7zeWGp9era9+be9te7tn5pqZmTE7duyQN9LMWTUwNztbG7vkkkmPvzgaNg2Pj30w"
    L"C7KpdWqxWJdsw8Wj9WrNlIx2V/HdNvnKErG5CItLvBwaPHB1n2Z7hX63S65K8OAoUqLKpF2vl01erZdNXmnIDYuNheeCCQ+1Vltf"
    L"Xjy5+NQ999yzct4aUFUREX344Ye3FivFqzr9/Fh9bGxoqbWYFPvZlfWi/eVyHq/dOFzfOjxed9ZZ1ChZ1odWF20uElqLsNTkWGjw"
    L"4FWrtFbb9Lpd8jyQZ56ggqpioqFGncmhjXHz2OVsnthihm2d5eYK3X7nhA/+2SzLvnrXXXd99rWs4TXzgIgwMzNjXKm6bf7k4ovj"
    L"hfTmvNPZvrHEz9Wdu269K1AsApIRO6vIcB1EMGqIQDSCiqJEEMWIwVqDSUAiiBFiHvDBgyppzGg1m+bl5SM8c2oirq+8Ja6vXuRG"
    L"6us2bqps3LiyvDJ14IEDX5++Y/ppVTUiEs+WyPTd73738Ni6kSH/7A9uetc7rv2z2Fgl7bfxKw31qIZqIm6oJlaEsGowzmHSFE17"
    L"aN5HfI74HKOeGANqAlgFp2hQiKBRid4TgkfJ6UmXlaxpjjdfMkOlUcoLQ7pz8obs4vqWRKJeDzx96NAhA5wVAOPOyXKa1szE8BVh"
    L"dcV3F477rNcvJFlugkUkOqwGUIEQCVYwWQadLtLtEXs9pNcHScljRh5zoonoKyAiGpRoIyEqIXhCDKh2sdbQz3qMuBFZ6TdtYLNJ"
    L"+3nhvKjEE489ll+x6xe39Vu+kBdSl/f6ZJ0VE/spiRNcLBIFvLWIzwAh5Bna6eJXVwgry9jlZdJkla7PSWNG0EAUJYoiFsT8jxeq"
    L"KhINGgNZyLDe0I89+pU+MUba7bY5ZwBrTtx94qmnb8hHa0Pt1RY2TY1rd7Bpn1gQ1EbUJXi7jOmARiBN0W6P2GkT2svYRpu00mUl"
    L"j+QxgCoxKqBYEbAGnAVVUEO0AfWCiRYvkWXfpdvrYbG8EYt9PTKnvV6vqtaMxtxjMy+x24O0i9UESczAfDJLDBGyDE37aLeHdjpo"
    L"p0NstvEhI40FggFRiOoJBLwPpGlGr5+SpjlZGvB9j/ceRSmYIrXiMH4oX6O05vzZaIgh+hC8qGKygKQZ9PsonmiUmOcEUehlkPaI"
    L"/R50e2ivT0hTkpUeUlQ6uaeTpWT9Pr2QkqYpvh+IaSCmOVkWyVIPGfg8oBopmhK2XsAYc9ZM9boANERVFaMo4vuYPCVqQDMFIkEs"
    L"MWTIapfQ76L9PiZN8VmGtnukp1dorKQ8c9Lig8dGwFoEwTgQNVgtkhARtWRkCCDRYDCIgDGD9+OPB55z3A+YgZupQAwBDRFCRATU"
    L"QwwZoddFmyuIzzCrPeJSi7zRwi93sGkgFIrkoYwYGVxiEAQbQa2iDkwUjD/zfOASA6kLzlmMNeengX379skakcrHJyY2x+X5gIjV"
    L"GDFxkIxMjESfY7IMk2ZwaonVk6fQ5RVMVArG4pxgnSAyyLoiBmMMIgYroCghiYPklhhcsGge8WvSts7inENEKJfLr8uF/he8qakp"
    L"A9But5+tDFVLMagKAYmDD6souQai5piVVbrPH2X52ReJrTZODJpYfGKIzqLWoDowBWPsICsbA84gzmASh0kMJlGME8QNvF2sxWIp"
    L"2CJJYhgbHzt/E3KFguaANW4Qq2XA6yUOImDWbJG/8DLm1BIWRa3gB6LD2gSraxnaWpx12DUNGGMQI6hRjAAxEoPHODBWEDvQUmJL"
    L"1Kt1k/b7nF48fRRgYWFBzxmAz8Mz0TjyxDHsikQRnFFKUUhPN8mOz0O3hxgIEVRkTbpucIlBCgUKa6ZQMBYRGQAA1AiBCOogRiQH"
    L"nEACRQqUbImh8rCkndw3m81jAHv27Dk7gKmpKQVIO+EHad+TlAqmVEowNifJe/iFFsnKCqW0T+57aOxR8BEBrDWoBYNQJKESc3ox"
    L"wUaDRwY/lQEABI8hBEPmhegjmQ/kPoBRkmJJR2oj0l3sLh8+cvjYK/vBc9CAAuT58vO5r/n50qj9r0KkP1Zl6cgCUkwpblC6oz3S"
    L"0Q6xlyIKgiImYp2lUCqgoUppa4M/2P4sUcLA2WSghbWdKRpA0yL5akK2WiLr5oR+oFqpU6hv0Fp5RBaZP/HOq363/Xn9vJwTnR6w"
    L"CZWHPvf7R2Tyzpd6pfq2f+kNxaPZFrNomrjxAjaBEAydiT69PBvwG6M4JxSto1w0aDDceNnL/NX2BfA5kIARkIH8iQp5hDRAO4Fe"
    L"St5zhDRSKuY8qSOxn9RMTuGFvXt35idOzDjAnzUKiYhy4IB53z1/nrZ8/tiWcaMFWYox7VAkUsgzStFTIqUcU6qaMZx3qOcd6qHL"
    L"aOxRNT3KSUqRlOD7eO8JXgle8DnkGeRZxOeeLPV0Mk+rD40eLPfh+UZGy1wuwXfQbvqNNdM+tzAKcOAMnVhafbCMk63VihQlUrAO"
    L"VcV7A1FxYrAKYg0kdpBpE0MRITEJ1hQwUsKaEsZWsKaEMyUSU8ZJCasFrKxFKCJKRtHkNNiixZEr7MrJk3mrfeTrAIcOHYrnDGB6"
    L"z3RUkLmm/drp1uKJay8r2lqZiKusUV9FVRCxYCyKQ0gQcUQ7+HcyiPuiBmGQgXnlelWdhgEjNUAfT7+0PZbqG3S1mx+q1x89Oju7"
    L"x+7fv//cASAoc3P2M791R7vXXfmHbeuH2DKOFsoKzqIxf2X7aUQwxqJqQO3afkkH9BP9Ebt/nR25ghLJxZOIYT4bwozeSOwH8b73"
    L"xenpA2Fi4iNyzpn4lbHwl6qqstBe/Vxnqb3y85dWzGjS02LJQIQwCP5Yk4DaNTkKUQd8JkQhRvMjgU1/fOUqg8VHJUQlAqJK226P"
    L"o2/ZaU6dPnz8os0cBJVdu6bCeQOQ6QPh0KFD9sPTHz56fGn+r6/dUpdtG10oF0u4glsTneA0QcSgKkQVYpRBbA+GPDqgMChQi/DK"
    L"tmSNU6kPg1AaHFWJzOdl3ObbY7VoBdxX3vWu6cbc3CG7ps7z1MCa46iqnDjR+uxys9V5z1XOTBRRW7O4RBAZxH4nDqsGciXmSvBK"
    L"9OC9AiWQMkpxAEYNUXNCSIkxouiAdojhqLtBRyfebV4+8nSrXB76U1WV13PeM8O+0cNHHnlEAfeJT3ysdfPtd5Z2bN481erMh5O9"
    L"ihEMqgMz0ugGUmVQNkmsEE2BTcMdbtlxbE0DRUQFCMSYEUOK+ojPlEoCLyyvx278SBiuFuzR+SOffu8v7X5gamrK3X333W9YLz1r"
    L"s2Hfvn1BVc1TWf9PThw7+vR7rtlkt474UK0UKFcMxSShkAhJYnDWUZIC1iUkYjGmAqYGUkLEgThUHZYCloAJgnVlWowwX7wzbrjo"
    L"CvfM4Se/u2X7tr+YnZ21U1NTZy32nhWAiOiBAwdk//T0atpf+uQIqdx0ZVEnhiLDtQ2UapZaVRiqOoZqCbVKgWo5oVpyFAsFoqmA"
    L"LYMpgi0OzCmWCbEOlElKNZ5ovEMv2bGbH/7wmf5KP3zymi3XNF+vEvd/6tBMT0+Hta7KVx86+MDfXrt1693zrdP+e4vBVYplsswT"
    L"AiCKxWELio8DMGrLqBiIAcjBWKIa1NdIRpXvv7iO9Vd82BdKJlmYX/j89F13ffPee+9Npqen8wvaHzhTKX5q+03lG48//u11k5PX"
    L"HHzydDidTdosgoYcVQsYXKKE4Ni2fpmP3fgoEnuIDvbWZIrPAi6JvHS6QjPZHbZvf7v9j0f+7bu1odGbjh07lu3Zsyeea+fmvBoc"
    L"Z2qTX/jC7OVbNo8/RqE6/O0jPV0tbzAxKBIFMQHrHBotm9d1+I23PYrENhIt+EDst7GlGicWHceam/Wq636Fx7/zaLuzfOqdu++Y"
    L"Pvxa9c835QOv8oc4MzPjPvSh6ecb7f4HakUrN2xyOhbmdaxeZnSszLqxIqNjFdaNVxmql4i2jLoq6ioEO4Stb+BEq8YLp7bqpTve"
    L"p0888ag0F5p37r5j+vCMzpzX4s8aRl8ntMZ77703+fUP/NrhW9//qycm69Xb15dybbRXtTBSl0qtSKGQUColrKsGLhs6DQRcUsaU"
    L"a7z0Q+HI8fF4yZU79dTR52xnsbV39x233j8zN+P2b93/k2kxvbZPzLn9+3f5Bw8e/OCG8fG/s9aZJxZWfLruIlddNw4Fx2ixx+6x"
    L"x8F06LYdTz23QpZvCOsnttpG8yStRuN3bt59y71vpu36ptqsZyZ+6KFv3jxWTf5mZHJ043MLrbAcHDI0aoaGEt5ee1GWW11tLiSx"
    L"Ul0fq7XR5OVjRxZdsbd36j27//nN9owvQJ94zons8gfvP3jp6Fj1M9VK9f2FQpXlfocgEUI/Jq5oKpUqaZqx2l59pFQu3XP99dc/"
    L"rqr2TLfzpwYA4LHHfjvZufO+HODLX77/xnXrRj5aLLqbsiydqJTr0u93VwLd/xwbHfmn6647eZ/IdHahuvUX7PDFzMyM2bcPzrRh"
    L"v/Slg6NpOr91cnLdSJq2n7nrrg+efHU4/pk98PFaBzpmZmbM4MQKP9snVn60UaKqZg3QT2zR/w0lRniAER6IIwAAAABJRU5ErkJg"
    L"gg==";
static const WCHAR kShield81_128B64[] = L"iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAABMkElEQVR42u29ebRc13Xe+dvn3KGq3oSHeQYJAhwAzoBESaRMQIM1"
    L"WLQiW4DTsdux40RM25bdnbiTTq/VBpHlxJ220mnZcdqk47YSuxMLSOy4LcvWYBGMZgogxVEUZ4KY8eahqu6955zdf5xb9Qog5Tgi"
    L"xUHh5bp8A96relVnn72//e1v7wNvXG9cb1xvXG9cb1zf39eBAwfM3XffnaiqfbF74N/kjXfr9XmZgc+l/tqoqhw6dMj+1zzQ3Xff"
    L"ndS/IwOPx4t8/rq/ku+j16L14mjv8zvvvNOKSAX4b33rW+vXr1+/+6tf/ap58sknh4qiEGstxhhTFIWuWrWquPnmm4v777//yN69"
    L"e2cB9u3blwH+8OHD/qLn6j3P6/6S70MPEN773vfmIyMjevjw4XJiYmLDuXPn/rZz7m8VRbG50+lgjMF7jzFCmmYURYGq0mw2mZyc"
    L"fGLFihVPdrvdX7n11lu/AvDxj388/8Vf/MWyXvS0/ujeMIDX4Ov5yEc+ktx1110VwMMPP/xPZmdn/5aqru10OnQ6HZckCWmakqYp"
    L"IQS88yCgqoQQaDQaCYBzjuHh4T/sdDq/unfv3qOqKu973/uyP//zP6+A8H3zhr1e/25VEFlyw7t27Uq3bt3aOnz48OzTT3/7B44f"
    L"P/0LzWbrR+fm5vDB+zSx0mw2TZomJEmChkBQRayl2+3ivUeAqqyCMVbb7TZjo8vswsLCeZvwT7/4xS//q4MHD5b79u3LDh8+XPbj"
    L"jsb3cPBvecMAvteI/m6S2/Ygx26HU3fdqnfceoQj99zBXg66+7/8hR/qiv3jogp2oeNda6hlG5mVLIGyM4/rLuLLLsY5OgamjJCl"
    L"GcNDQww3hhhKmyzOdSgWHEONUdctO0mXecbHx05suWzLT60d3/gXhx5+ONu/87ADuJuDZk/8s/zr0QhenwagJF/5PfIP76C8fP5u"
    L"3bt3r1PVxhPffvL/OX/+9I90y3bmsb41NJoMZTnF7DSdqUn84iwjmWEotWi3wympOD6WE1ygaHcQrywfXs5lm7cznI0xdW6GsizU"
    L"264rqyodX7ayqAr/4b17f+BTB1QNR/aYPXvu4Q0DeIWvQ4o9zbZkHT+u++VgeffXHt84bLufSDP7zumZKQ2hYnRsWFLg+Ue/zbgY"
    L"LhkbJ1tsU85N056bRsuCU2OWb65OESsMN4cwqhTzBfPTHTau3sybdr2Vk8dP4DoFiU1Du1NKljaKbtt/+Ic+uPfPBQm/9nvXtrK3"
    L"POh+YTvlGwbwCl0f/fS2/DeaT3r24p48e/anJ06euXPi3ESaZYnPs9SMj43J4tx5zjzxKFeu3ch622Th209i5hZIfIULJbiKx0Y9"
    L"X9mcIokhOEeeZIwOjTCUDXP2xBSuUt558zsZCcNMnJ2iXZZaFhU2TSTL0rt9KPa9+93vnvzK8X3Nt27aUYgcDG8YwPcy0VeVY8eO"
    L"JbvftLs6G55Ze/rpxZ8+f3rqnxaLJXmW+UaW2mVDLc6dPs3MyWe4adtmmp2K9tPP01osSYsuGko6WlGUHb49rnxpS4InkIgQnCcx"
    L"lpSU1Ss3MHl+GukYfmDHHlaOr+aJp54hS3NCUO0WHRkaaUysWDH0j6675qZ/HYGpGhEJ9fsaLk5P3yCCXsK1Y8eOTERKoPrYxz62"
    L"8sRDU5+zSX61L13I89QMZZldMz7Ct+47RgrsufYawunnKSanyVyJ0QKsx4mjqyXt1NFOAt4EvHp88CAeHyzeZJyYfI6hoWGSLOMv"
    L"vvYXXHXFTm68bhfPP3eaxfm2jIyOel8VK5975vRv33ffsX2dTve/F5FzH/3oR/MHH3zQ33PPPb1FN6/ltPH14AFk37595vDhw356"
    L"enrZiRMn/vbU9NTtQdnmvK/yLE+Hh5rkwfHUgw9w6eoVbF61ksWTJ5CZKUxZIu0C6zzqK0qtmA9dClfw5KhydAM4E9DgQD2oYCUh"
    L"sTlGE5rJMCvztZx49hSrx9ey523vpFioOPP8adQFTY3R1lDLVME/vmHDht+6/PLL/8UbIeBlcvdHjhyxR44c4eDBg+7ee++9YWRk"
    L"5I/a7faWqvJ02t1gU2vWrF7ByWef5szTT3DL1TtYmRnmThyHxTZNFcQHtKrwZUnZXaRyJYUr8EXJE2OBL29WKuPBO1Q9SkCwiCYI"
    L"CYlkNNNhxodXMj+xiF8Ubtn1A6xdtobnnzpOM2mgQf1i1bZihDzPf2d8fPw3r7vuuoeOHDnC+fPndd++fUFEDOD/mzeA/xKJrqrC"
    L"YYzsl/6bdfTo0d9JkvTHi6LIq6r0wYuMLxs3wVc8/9xThO4iN1+7g7w9x+KJ51hmFd8uCBWIKGWnoGgv4ooOUjl8USCdksfGA39x"
    L"aQwLhEDAoaqogqoBtWiAoNBIhlk3vpGkzDn7zCRvve6t7Nx6FROnzjM/O480TFBUO52uXbZsrOh2u//wXe9618d7r+HOO+9Mb7/9"
    L"9gpFeA1lC/LK7mqSY8eQXbvwgCIKYO7imPkIu/jqvq8mnzn8L/xBDpf66cfzB1bO7Jes8feMsdcXpSMEr0mayvBQg9PHn2bi1Gmu"
    L"2rKFbWtX0z5/DjrzSHcRExzqHb4s4+7vdAlFQSi64EqqoiB0Sp5Y7vnP2wJlcDhX4ZxDUVQF1RANwQv4lNw2aKZDDOejrBhZxamn"
    L"TrNqbCXXX38jTdti4Uwb9YoYUzn16enTp88tW77s2Q1b1v769Tt3HRKR6u9/8yeG5lfN6BcWP+XHthO2Ho7YYMc+9KC8OjjhFTWA"
    L"u+8mebCD/dLnMOfuo9pzjwZuvSO7beE2P39sXvey133uf//c2Or3rPtQM09/Ns2zNzkXKIrCA6bRaEi70+bbjzzAcGbYvfNqhhCK"
    L"yQky5zCuJLgC50rUl2hZEIoC7XaRosSXXUJVUFUFvlPwxHLPl7cpXV/hqmgAIYTaC8SbIIjPMWJJJccSP64aXwNOOHdmgl3X7Ob6"
    L"9TcwNz3P1NwM1hgVa6UoSmxiyPL83jRLf/Mtb3rLv43ugPR/Gt+XTK06p+3z91SHfwxf+wRbA0b9fvUABjCf+MSWZGjozWHfqp8N"
    L"snevq11/46tfP/rjw6NjH83z7DoNnrGREdrz84h61HueefJJzp4/y45t27n8kk2Edodydo48eKx3mKrCuy7el4SqhKpDKEq024Ui"
    L"GkOoSsqqi+8WPLm84ivbhcJVVGWF8+5FjEBAU/CCBhAvJJoy3BxjpDXKUGuE6VNTjLgxbr7p7axYvoKzZ89Sdr2qinY73SBqkunp"
    L"abwPv3nFjisffNObbvzXdbrIR3btSqe3bpW1t6yVM1/6DXf48CuLE+SVWfiosjn8f+5vbL11XHbvvqvd+7evH/v67qzR+nnB/ODq"
    L"1avXqbVMz8wx0mxqe372jzIJV3bm5nd8+6EHwtoVK8wNu3aRimH+7DmoHENZinEllBXqSrwrCK4iVAWUbUJR4rtdpGcArqAsowd4"
    L"cqXja9uJBlB7gLIs8d4PGAAENaACKkgwSDAkJqeRNmg0mow1ltEshjj57Cku3XIpb77hJlw3MDM5hyUltbmfm5kVETGqSuWqx1Ys"
    L"X/XP165e/9DWnZu+3qe4D+3LHj2MP3ToUIgFJtHXqgH0fk8vxnaqKiLCgQMH5I477jBHjhxhb73Le9c99/zZurVrN96e5q398wvz"
    L"l6/ftNH6EJiZmztmSC+1JnXT01P/x+TJZ29YnJ68dSjL1199+TZZv3q1LExPYknIbIYWBaKKhAotS4Kv8FVBqCq06kLRJpQloVsg"
    L"ZUkoYggoyy6uW/LUSsfXtxFDQFlRuYqiKCIWqA3AqxIUpBYZGQyiBlcFUpPGl+4sqxurGG6MUnRKfFvZtmU7O7ZdTcM0mTg7jSs8"
    L"Q80hNz83b+bbC6bRzJmcme5cfvlljzlxv33l9m3fHBtb8dULQ+bdyeOPPy4At99+u1PVl90oXjYD+OhHP5r9xm/8RvHisf/ubXme"
    L"j46Ojvzc8PDwZXNzM8Zac8TatNFopntWrV4zOjlz7tMLi+2ztpLw0NFHVovxf2vd6hXLN65Zw4plIxjvCEWBQclMipEEvMM7B+pQ"
    L"5yLgq0p8UaJVB8oOWpZozwDKAq1qD9Dt8tQKx9e3Q9eVVAMG4L3Hex/1AiHga2AYfCB4T2IyEpuh9asXNeQhp5m2GM5HycipFj3W"
    L"ZWzbsp0rL7uKxKZMnZ2hLCp8cBo0uLKq0rIqEIShoSG6ZfdPH/zmg7969dVXn7r22mv10ksvffbi9/3WW2+199xzj3+5cIK8DL+v"
    L"qmpFYtr2xS9+cXO32+XMmTNbd+7c+c65ubnGpk2b/u7p06f/bZZlcza1zT/4d7/xm7/2a5944ujXP/ueiZNPTb/nQ3/34ce/+bWP"
    L"nzl7dsOTD3x7/JJNm968deulZtPG9aHbXhQNQawJWJEIIsQiYvDBEbyH4FDnoXQx5tcGoFUHLUooimgIRYHvh4AOT69w3Lsdur6I"
    L"GMC5JQMIPi54CHiVWjDi8V4xYkGkz/KKgvGGPMnRClrZMGPNcWzIKBcrcttg/dpN7Ni2E2tTisUO83MLVGWlBqPq0U6nK61m05yf"
    L"OE+SJOSNvHr++PO/tWXLlu6aNWsmfuVXfuVf/tqv/Zq96qqr5muw6F9tAxCi6DKIiH7hc5/78fGVK3++KIqviIjvdDozVVX9yfT0"
    L"tE3TdPJDH/rQyUFK9POf/r0d111z865Hvv7V5S7J/tdzp07LpVu2rNq+fiMrmi1AaXc6BJQ0T8EYJLFEHR8ENC6UVqgPUHqkdNAt"
    L"oaji4pdttIoegKJEyxJfFZRVB98peGaF4+vbQ40BSqoBAwgh1FgAglc0KD4EQojqoZ77CwJGDARPe6FNI2sxlI8gztLMhlg1vgpL"
    L"wuT5aXxH2HrJZey4fCcjrVFcu8J3A2Wnoio86oPDqq1cJZ1OhyRJSGzC/MI809PTE5s3b07a7fbf+fCHP/wfPvnJT9r9+/e/ZCN4"
    L"SbWAAwcOiIjoAw888KvTs7PzQ1n2szfccMP93+nn/+XZu4ff7y67+fFHHr251Wj8zOOPP7k+aY0yYi3X3XorCXgzMyfl3JwJRsBa"
    L"bBqrdRhBxCI2BRtQfMRl3kQVaO0hpN6ZikD9tUZAhdZfixgQ0//3WtMTKzfGLC2wxoWPjxeiz5XaEOIXCOC1ous6ZK0cYw3z5RwS"
    L"LG3XYXpxilZzmGVjyxgZH+Hs3BlOfvkk481xNq3ezLoV6xhbNoYNlqJTJovFAmIEay3OOacadGhoyKxatXrluXNnGRoa+uTDDz98"
    L"5Y4dO54cKD69Kh7AAv7QoUOrLr300j8YG9v9/ssvl6I2Kqd6NIWtV37tnqPdkyfP7121du1tzz3+4Nj2Sy99uy9LhkzC8marGm21"
    L"pGESW3U7dLsdyawhtQaTZpg0gyRBshSSFEkzTJKCVVQDPvi4+71HyypmAmWJlg4tO1AuEGrXL0WJL4qaB+hGDLDcc2y7RAzgSrxz"
    L"lFUZ08Hg8N4RgqLeorVH8EFjaKhTxBCUoB6IoNB5Hw1DEowK1iagBgIMpyMMNUYYGRpGS3AdT6IpDdNk1fJVrF65lvHGKoZGR8iG"
    L"E4puQbVYsjDTpmyXmrVSP9eeTtK08Xtf+eJXfgowBw8eFKJAVV9pD6B1zGdkZOzeN71Jig/8f0dbv7xB3tc+d+aW//uff/bSK7df"
    L"8cG5mTmGG02GEW657gZMteCGRkckcUHoFikTk3SDw6iSAdLMCFkO4jHiQQwaPBIM6j1BBFGDooiv7xDDgdb5e0Ajp68hRh2N3EpQ"
    L"H/l+jWme1PF7yQloHdqjsrxPBpn6EYOiRmvfT2QLewyOCD44RKVmE+PfGnxAEATDQjFDt2oz35mmkTVp5S0SK3TdPMdn53h++lka"
    L"5TKyRpPGspRWs8Wa4TVs33Q5zz19XLrtRckbOe2FzvUHDx4Mhw4dGpTBv7Ih4NChQ7J//35+8id/8m3OVVtUVYxI+/bf/cQ71q9b"
    L"/bN7b9gNRaXbN49LluZB1ISyMy+uqJKZzhymcmQYMmNJEsEkliSx8dVUZUzBRCJbXHNjpjY7NfF1a3BoCJjaCxA8uJgR4AOqAUIk"
    L"8zUETAh4jQsoqogGAkLQ0F9MlUCMHnW4QFGRaBwmEkEqscYgUmNBJJaTI9kVQ4Y6FMGIqcn/QMDgQ6AqS9rdDvNmnobNSW1KI2uS"
    L"Jw1cXiCZUpaBqcUJHjj6TRp7mqxdv5aHH3tIEm8C6Ngzzzxz5Sc+8YnH69TwFccAsnXrVgP40dHR24pud20vP5177tmpqzZvqDaO"
    L"j/vZM+cadqFDWcyYslsZbxxpCiNisSZFvMcEj3jBGEECBAmoq1+QGFSjEQSNoU5CwFgbt6z39cIrOI84V3/PQXBICITg+z8nQTEh"
    L"ICEgQaNh9HY6IRqCgBiJKym11YX6c2HpNkuZsAatQ0LPc3hCkPgQEnc/CMFEozIiGGPx6ijKDgZLUiQYLCONZWS2QZ7ljC9fzvxk"
    L"wsTcedauWk9qM1MWnWp8+YrNi4uL7zx48OBjt912W/pS9AYvWRDSbrcXkyRxAJ1vfubSbzw99xNlt5NOnS0SbS8iARoYWo2UoqoI"
    L"XYcVh40pBD441Aili0DP5EkEfrUB9MCYaARjEmrXLoIGj3H1wnoHzhG8Q3wF3kUD83H3S4gLoyFgVKH3OAhIqL166C1pj/iLLj/0"
    L"PEIMD6K9na5LRkBARVHfYz9DbTemrwvpuAIrEeAZqV+jUtcXUtIkoSMLdNwitmPBBELqaZftCHq9Ym1Ct9vV9kK7A/D000/LSxGd"
    L"JC8l/td0pVGRBOC+xc4yaWSXIIq1iMkspvK4qkS9gqtI6p0Ygq+RdECsQTHxYxl3rYQMq8TFzWKpTBAIluA9KoqEgIba/deuX1yF"
    L"1IuP92ioPUJtIOI94qLnIXicJ6L6Oq326vHqURRjBBUTY3+vLiBKqBc3iESPJSAWCAZsxAoYqQ0lEDyoOsTGQBDxSokRQ2ITgnh8"
    L"cJSV4IIjtxlSGdplA2NyFhZncd5hEktZleQiUlSFAExPT78kDGBeqgdwzmkIwaqqTDQy3+50PJiaSPH4UOGoqEKJc1Xk6Z1DXeTu"
    L"4+JUGOcwVYlUJbj6rncyrkJc1f+euhKqMmKFqoLeY7mqvh2hqgiVQ8sqPldV1Wyhjx9dLDAFfPwvRGlYqCFkHRRANe5sAa09RWSj"
    L"QgSLRmrQKIjRelNwYVFPQgwhSl1qjiEl4KlCSaUlpZaUocTX7Gao2UjVgKtc/dcEVFR8CDgX4+Tll1/+6oDA2vVQVZX44IOI6OeO"
    L"fkXL7qQxTdAygAsEFw3BhQCuJLjoeo33EAKJNTWSt5BErj0i9rjj6/cNrEW8R61HJYIqQs8LxEXFeUJV4Z3rGwlVhS8rTFXWNYKe"
    L"EVY4H/BeIsNX33F3h+jSa0xwYbZcL2wvJNTYoJcdxI8SU0QCivRNQYL02AboeRECahWRCEh95QhiUav4pMK4lG6328cpiOC9Z3Fx"
    L"0fIyXC8ZA8zOzs6/+c1vuf4P/uDuq1c+JPNnRSTzFlMWSBnTNOr0zAeHDy4CMR+wPmDUIEZq6sYQaswsIgRX1aDJREBlarmtUCN8"
    L"XcIEPmr+QlV7mBB3PFXc7a4ODeo9IThC5QguoCT4etF7HoD+4gc0yAtKHxEPRM/Qsw8RlggnlCXLHfhcTfySgKpB1eM1xNBiFItF"
    L"qxA9RgIhjZilqqo6PEWcUfMf8qoawOnTp6OjC6E7Njay7PmnH2+uW3Sz1SUNSl/QMBrp6uCgiq7cFgXGOyQo4iMa95aI6rGICImk"
    L"GBHEBNQE1FQRG2jc+eJqhg9BfS3m9AFcHecrj3gHLpaAvasIZYktK2y3xJcF6kuCc1RBaTtHqS6GAomYot5r0V0j0eB6jkCkhwAx"
    L"0QVE9KUSM4vaxYsKovFne5+LaD+89NK3pJdWaHySkApl4gk4slCRCHSLLt55rErESgiLi/MGoK4Wftft6t+1Adx8882hR512u129"
    L"Ze/173z+d//ff2c3XUUVKtRCEIdoDcyqCls6EuciMRN6ebrExTUWxCCSYCXm48F41Gi96EubUMTEMOB9FH063wd56uIul6qAshvF"
    L"nkURawVFFYUhrsAbcAG63lPha8qXmhOo2aHIAkQSOK5tz3v3aVQxivE9l18veO3qZQBoae0x+prDXlVXlmCYCvhEUeNwPtAKjqCh"
    L"L1IREnpTK7plKa+JEOC9t7Ozs3LZZZf96ob/4W8vfPHIX8ymSTomIdbUJSxxVcFHPNAzAKndnxgTFTdGIvihAmNrlbaJObbU4bbO"
    L"rYMSGUIfogfwFVSOUAMoqcroAdQTyhJfOEwVS8bBe4JEdB7pgMgJxIWu8/l+Xt9jd+JO1kCt6azDQR3nMZFcqrPVmjvoWcDS4/X+"
    L"QxjgDpYMyruKoJEw6imUegKVGGpe3jaD79oAjh071jMAiSoaxWkYD875ntUHX7vU2t3jPVq5GGN9LGlbE116MIoRRbHxTayRswRD"
    L"MBfOYoj8O33mzwSPVtELhJoLMJWDskC9I5Ql0q1iVa8sCb6M6Vhl8MHgGZCA9TxpbRgR5/UInr4t1JQk1BYR00GUYOLPaI9PUNDQ"
    L"e9yLbr1Q/qcqOO8RjToE55YMIITQL1aFoK++AQx4AAneoyp0qir0nCdIn22ToOC13q2uFlvGAoqKgWCxIcT3wrvazUeqVa25YFEi"
    L"8NA6jw99wgcX6thfRXxRxSzAOE8oSqRy8fkrB76sjTDBY3HEHRY0YIypUV3M5yX0if8aB0jNFC6hP+nRsRqZSoJEF9+LzPWmDZVe"
    L"IDi92AOg4NTXzKfifTSE4EP/55bo51fZAE6dOiU9iyyrKjJVVjwmonZXdEh77tN5fFmROE9wDl9VGBRro65BRBEfX5ixHlPLsn1w"
    L"kVBJbHSptSKHeveHHu3bY/x8TP/UxWYQyiriAVehzuOLCvUVgqMouhBG8AquVqnHNCumn6FG3DHzqGN6DfK8i0IRNHojDYF2t4vv"
    L"F6WUbregLCtCULyvK4hV/LcQQn9H92sONbBtpcPYxJJlOc5WmMT0ZDd1RZKXVTT8vesNrNsjpd5Fpk7Z8JGjj4A6TuXAEBdKauRd"
    L"EyyCidSqd0iIP9MLJ1Knk8FV4D2urPogMFQOGwLWK6EmmqRySBkBIhJi2PAV3cpRmbhJRep+gN57XBM68Y0PVFUMdVW3pCodrnJU"
    L"RVQSuaoi+NA3AjSmrnEMTa0fcHER+wagdd2hbwgGyS1JkoAIlXUE8X1NQ9z6L69O9GUxAB3gSrRGs9T8vdQ0agwHdRHG+5jrY9BQ"
    L"kyAmvtnGUGcAQIhFHx8C+Pr3vCLqwJXxzawrf5Fb6BFCPVIoRGbNR+Og8LF2bzXSxRqwSYY3MbnqGW7wnqIsKMsy6gR9gasCVVng"
    L"q5p+ruVAGqJcBCt9F23E4H1UDwU04gOFNElBtR/TYypYh8vaA1RVhaqSplldvtaYFg+KV17GUYYviwHIRZMRzJJYcAm51yVYDXWJ"
    L"FkBq+TVgjEPFoloR6iqgEhARqrKO6z7UHIIjuKIfEsT7WD/wvmb6HMbFn/caCFU0Ct+NoSrkJi5+/YY6V8Wd7B2Li4tUVdWXhHkC"
    L"ZIYQfJ2+hahQ0rqX1CgaojqmD1W09mB1tUfEoL4eSBWoiSftcw513MEgJBKNy1hDkiTYJIEQK4j9YUT6GvMA3jkQS6iiu3IhxNSO"
    L"AHiMrzC1G9bgMTUREu2grra5WJ+n5gN6UnxQEtfb3VUN9gLiq5gGOoc4j3dlTAdrbKBiorfplPFnAKuQe0sZlKosmZqc5ulTHQr1"
    L"+MpFKthFHJBYizVR/Fm6ir4AzAo2ihLxREyjgJUk/u0ERCW2+ASIAkYXjcDG7KYHcPv0IXWJuCZ5et5g6XP6U6iEi4Djq2QActtt"
    L"t+nBgwfjzD3nEAtO1YgYKu8RIzWV6mMMrgs/pubXQ42CjYm7UVxd6zeRXIlmHvrlW6ll31SxymeCi7X+mgRK6nQzuBKco6sBDcqy"
    L"kGDV4DQQSk+Y69JpLzBdLDC5uslMF5xRjBhMKrG4o4qT0CeApCZrpJ+GOmq8SJJYUCEUcIFgoDc+TBWjElE9sYrZ0x4M1uOWlEnS"
    L"9yI9rGCMqbMM7WcrvetVKwZdFAPig9nEIlpnSbpEgS8lcnVF7ELJVZ9Hj9RyxA2939EIvowLUFYxtoeqNqyAdw7jPbZmBU2v5Iti"
    L"FKTdYXF+gbmiQ3dhkaztIguYKoZmVPT20rzQy/n/S6/X0OvP6O9GY/rpoInUX714QhDBGBMVSbUnMSbiB9X/gjBTlSSN2MFYC6YG"
    L"ma/ZLGCApAoasP126yVA2KdDGXgTtdeV66P0ujadoB5XFNgqkkhSOUJwfQOg1hqEymF9IFHFxPYrOjOLdKfmaC92KGzkJHKTInlG"
    L"O3SpqhLV7EID/SsoZXuuubd6UjOBtWRlqfpXVzOxUnM+dWd4X7kcBtONF82kQggkeRLDjDVRsvZaywJ6mrQw8Ab1ZdU96kx77jC+"
    L"2X2WLWh/NxnRWNoNvd2vEByhrJDKI1WFVh4JLhaBqqgIsi6QI2i3opifp+x0aS+2Ce0uo6SMiCUVxeSGrFKKqsRmQp5l/Xp/bxH+"
    L"qvi6B8Z7r7VHDgWjvVdfN5aG2E9YjzuQGvDpBaON//IMK/IldSjq4YdX2QD6f/Xy5ctDXRWk2+n61vCQ9hbeGNMHOqpLwkzo8eYD"
    L"wkoNMaUSW6tqXFTB+EAjSSKJU8TqGF7RymNVwSlSVhQLHcr5eYr5BUJVYRQytaReCaI18AT18e8yg+77Ao+rS4bQT71qMNbT+GmP"
    L"3l/yYBJqLUAd8qjL16iAiXyHMXVdI/haZtYrBkmfVAoSSNKknwUIQp7nWBul6U5jx9KyZcsU4MiRIy9J1POSFUFDQ0OJtZZOp+PW"
    L"bVz380OtodFu0a1j63cXQ4z2yJ6YMhofIlXsPa7TxXQrhoOh6YQwu8D0iTNMPH+ChYlJtFOQq5AbizVao/IQ9QY1s2dMLel+sXgv"
    L"F7Fz/WaTge8b6f9cPw02Ua3cQ/dqtEb8xIyo/vleTm/Mxe2VdcrYCx0I1lhUA4lNBiqU8W/MskwB9uzZE17VEJDn+XRRFKEsS80a"
    L"jXXNoRad6UlkfLTv5gfdpuiFr3tAVFPXz2sxRr3w6h2uW2BDoGFtBHadDt3ZeRbm5mjPz+OLghRIMSQGTAhUKBjT02DUKVhU82r9"
    L"0Rh5QQTup179hR8wAAY9wQsNJ2aHpia5lv5vtK4lG+mj+iWdQXhBOO2ngUbwZSDLMrz3sVsoVDSbTUaGR9yAB/iuS4TftQfYvXu3"
    L"r3UBd505c3oysTbtdDohTVKKsqjFEt9NcInpj3rthw1fF5C8c3QWFpk5O8Hk8ZMsnJvElJ7xxhBNm9JIUvIaMQf1eKPxtjEMxF0Z"
    L"S8sMKHr0ooympwDupepc7AkYqA/IEgYw9S+ZnnfotZ8ZEFtnAhLv3uO9aNJR6w5EolzNJlEIW59voLG+4Ac9wKsiCxeANE298x6P"
    L"4tVL1syZrTyIrTX9vT6+WOaMW24AdfcFl7W2jhDVs97jy4IsKDkGN7dAZ2KK9uwcZrFLHt0gIQSKoouvKkJiI0ljDMFq3GH1oqiJ"
    L"KxPr9aYWcsaYPJi/i5g+PdsTa5iLAW/NyeuA4kdrQsf2094Q00LT0xdE5hMT+wh64hIxUrOhdc+isRiTRHZRErQSWvlQTRIpiqPV"
    L"aDE2Nh4ARkZGXhVZeN+JX5qmQRANBlzwNIeGcM5DMBiV6P5wEArQCum3btVvUm8P1osU1cQViQRya7DdLmF6gfLMBOX5SbIqSrq9"
    L"dxcc6GJSE8sFGmoSyiA1s4YxcS2NiTIDY2o1b2w7iwsxuDuXcoKeO764codIbCypw5fxpg8Co3ikLoVLz/uEvjAkiOKJf6dNhKoK"
    L"aK19FGMxNtK/KRnqDGNDyyI4xuNCQZYvI0/SFGDXrl2vLhH0pg99KGR5HJZQOUczywmqsa5tDGriQtR+cUAu3UsPtf+GSTCxzauq"
    L"yNRig1IutFk4dx4W2pHoCbrUtXMxwpCLFykKTvugq3bHpiZyRAxilpi2HsDrhS8z0DE8mO72vlYzQMv2FL+RBcKEGH4EU2+CJWWQ"
    L"qesHDIaXnveJ6QBJo4GIIckSGo2cEALWWJwL4rwr8zyfBuTIkSMvqT78UrMA+fVf/3WfZ3lQoKgq8maTSpUKRW1SVwctIrZWztQs"
    L"YX/RPSbEhbVeyYIwahr4hQ4Tz5+kPTUDRUWiSlbH+cS8MHZejNpNvZhiY0gQMTEGm9iA0rvj96T/b32UPvjzMvDvA8ay9HX9sb77"
    L"z1E/bv97/Z+VC57rglSTSBplSYYxhizJGG4NE3wgsanP0iw5+fyJb23dvvU/7NixI927d+9LygLMS8QAAoQszxJFcC6QZDkuyjwI"
    L"Rgg9d1ujYpVQEy8xNzca+/VsUBIfaCiUM7PMnT2HX+xAWSEusnxWQ93wEV60IfKC7w2AtrpGu3RLT1nTh3IDxmOW2sAGDOvFDG3p"
    L"64t+zvSfoCaLlvSfF2QW/WLQYGCNxp2aFFEhsSmNRgOCkliLLx1iYvvy4uKi4SVOeTEv4ff0Ix/5iDXGhGefe/bjqViq0gWbZiSN"
    L"nK53mDxHkqRPdCzFWulz9RYlFUPiFeMDnakZFienSSpHQwXrauFniCmhhlpTqPqCu+eejTEYa/rpXtyNUUlskoQA1CH0grBBHSri"
    L"Y/SmkSx5gO9kDKYGnmp6yD6Wg401kb9PLGJtH2D2jMZI/BlrDYmNQpA0zeLAScDWncV51iA4pSodqLBp0+YMkPe85z3+pSpEzUsB"
    L"gLt27UJVKTruuTRJFZFQacDmOXOddhzsYA1ikj4xpKYWe2oUgVoxJAqpCovTM3RmZ8kCpGrq0BCJIQbUtPoiRM3gbjc9dq2eCoKR"
    L"ASxQS6z6rtnGzuQBkua7ubH14g88F73nGuCYX/z3Tf+jFUNiEqxJ8T6wbGSMLEnxzmPU4JzHGnMW0DvvvPNVGxGjg7rA7du3Dw0P"
    L"D8vZs22KqmJodJS5qQnMsjG8SaL1myS6xjCgEAmQiiGUBVOTU5SdDk2kFn5EbFCX6GIHbw9RK98xBPRyd2MkAlA1S+xdLyPQwbge"
    L"UGOw/SxgqfHkgjz/oo+Db0Qs+xqCDuRj/c7jXolZlrqGLoihS+yeqY03SzPyNMOXgdUb1yJYvAvY3KJe+cxnPvuxv7yK9AplAadP"
    L"n1YRYc2alfcdPzMxVTo3VpZOx1Ysl7PnzqA9F2gM1iS4ulJmtKfBg/biInOTkxjnSYxQt9L2K4YsyTUJpm6uMFzQUHFxjq4Coc5A"
    L"jC65537lTgSS3t9lwdAHZXGgwwuJnr6BmRd/zzVEo+vNA6GWigVZGijWnw0gUTUsffuo+30k1kKyNCNNc0LpGRsZxVcOKwZfedIk"
    L"1SuuuOJlKwm+JAO46667fD2o6N5PfvKPn167Zs1uX1Rh+ei4nA11QdQavBXUKokNmNg2gEVxnQ4LU7PktSEnQFa3jRkfZdHqQuy0"
    L"UcGqQaVWCLsLaWZj6zewbuCwJjZbWg2R8wkJIkk8EUQCrbKkWVlM0sDbPkJbagaVi7HuIDV4sTsU1Nb8f1CMh6AuTgnxkQgDD+JQ"
    L"Kz0tWa8YXj/sUkZi00CWB6oFodVcRlU50jzQnp+l2WrJ2uUb5bVgAApwxx13WFXlC58+YpvG4gunI8uWUZiUdtZARsdodws8gtUK"
    L"MRlF1+HLiqIMeLuMVITUBqyBpHIgFZWUVJQEU+JtFSVjtTjUVFHxO9AoQI95NSbuwJAIPjEYAzZJEGkSyHCquLQAP0tQodmeJemJ"
    L"QiUqfExfzlbn8WL/Um+rsUAZPZZnqRciKL6q27tdIFRlnGLuosrYu0Bw8bVYa8lthiGQpG0ybZGl69iy7gqqhS5Zy2l76jzGrF4Y"
    L"Hl49N2CZ+qp5AEB37typIhK+duze9tzCApnJaY2sZKI1whdOTdFKW0wXI1ShBa01uBTmdI75xXmSlkCuWJREFCMBZ4VKFF/G8a3B"
    L"eVxRxnc4RBGo6QO9ngHE+oGIRtwl9bQREYwFaxNEUtKkRekqNO1i04qr3hL4X970OJXv9hnA0BesDkzBFfuiiy4XfF2Pl/P1r1ZK"
    L"cD4utnMEp7i2xI/19yVIv0/AiiVJMtQsY3zDOio7zn3HFhjLmkxVHbI885rmydmp6S984AOXfPnAgQO9I3TMqy8JA6bOPvqPm8Pr"
    L"/7yqlEKVfPV6Hn36PK3xFUwl47S94IKh8J6pcpbFsEBqMpLcIhK1gFYCZWoojCF4h3NL7dGhp/kLHqxBa2XuwB6sMV5cyARLgmCt"
    L"1iNZLKlp4H1BlhUEW3H1pW3etfEElFoLOlnq/NQlPiGI+Y77X3vVzFDL3HycA6AuEJwhuICvBHVgQgMNgq+i+FSDElwgeMUYS5pm"
    L"kA6TrljGk9M5E8Mpw3mD01UgpJb5wrHu0kvpFYHqEXHyanqA/nXVug1lp2rJ+Zl5mGuzYXwND3bOUQw16JoWcyoU3Tad+XlmOgZN"
    L"llEgpElST+CqCycBQlWLKGv5dCmOIAFv45QM36OB64wAUZLELOnxRAkk8cQPGwiJYMXgnGKNwWYJne40Wim60EardsxSALH1GFhd"
    L"8rDGfOct1hN+auVrBVNdA/Am6hSdx1Sh7oiai5igHpwhClYN+EjzJppiigmSRsrTDy6y/bIPUjiPM0K3cAg561evbQK9wyp1qXb4"
    L"yheDANi3b184dOiQXbZs2zefOnbsSyvXrr3Zzc/59eNrbEMrKBYRb3CFYbFcoO3mqWxFCLEmXklUFIsFYwOZs/FN6SlnVWMzp6kN"
    L"QuKwJ3zoZ1ixlhBJE9NPEZVgHZoEEhsBgjGQJAGbdmhoCaZEGq3ITJpkaYRLvxIQU0lvLk799AIFUVQHx7S21wYeRaYuGqx6vNj4"
    L"FOJweIIYvAuo07peAqWrSEyHxC8yuVBy2ebLOD07Cblh5uycNZqEzqz7l7UL6HEAL4kLeMmKIBHRrVu3mvFLL53xq9fMtlNktloI"
    L"+QgsX5Wy0D1DsPMUzFCFEhXTb/kOECdv1qNXgwqVNZSZoUyEygplAmUChYUyjbevEXdU4cSiTJAop/Impl4hCfgEXApFpvFOA2UK"
    L"XePxCVQmgHFgHCoejEdNlG5jFSy1Zwr1TKB4x0KS1h3MoS7zBsQ4kHp6uVSo+Pp2tcAzI4QmIeSE0ERDk6ANVJsQWpQuxQ0Pc2LR"
    L"sWjHaa7cyELVwWYwPTkhG1avM1NPnX/sRaLQq4sB/mTXLj2gavzjjx05c/yp92fW29W02bJ1Hd+693F0aBxnFBsSct8gOEPUM9TU"
    L"qJf43hrQxOBFahAucdxqSOIbHnq1Ax/3Z09li2I1PpapCZdELTYYCKYOE3VBBjASqxUJbaBA6CK4etf3BDZJf38k+mL5z4WKZqm9"
    L"la2nk2pfFe0RKoIKIWSoN1hvY1bjou3ELshozSEf5/FnS9asux6jw4ifRHU+ZIkxrdbQo+s3D80cOnTI8jINCTAvCwC4g3DQmHBq"
    L"gX83ipFGtzBuYprL121iSDJCJ5CbFsFYnIk7ONgo1wq9nvy6l9BowBALP707JZCFQKO+Uzy2buy2vc/Fk4qSSCAxgTwEWuppBE+O"
    L"J6PONuo5G4kYbN3EVpel4i1JfVuQBEjjkTGDt6RABqRIfaO2fgaLkqL114EUJSWqEj1BSoIUqCnqz+PXji5qAt1OysQJuP7yvbTP"
    L"VQxLQnvqnI61srLqyse23bzt3PTWafNyHRyRvDzrD+gvm59Zc6W9d+rkRBmSle3zc7rpqm2ytjlKZ84xljdYMHNUaX04U62ZD8bF"
    L"AkwSefmkHgwRTD1CRhWJzEp/9m+oXa4O8DJiFYyvC0ESBzWIgFWsSBw3g4INUaVroghDJceLQySr+b8klq61N7JGXtTB9nsa6iFQ"
    L"IlU9p3hJoeVrJZQXwYsDKfDicEKcR2S70ZP4BmIspIGzsxkuGWPFqrU8c3qOvGW0Pdex1jTm/uf/9X//Tzv27chu3/WRCm7nNeMB"
    L"5KCEH//08lQ2yvNzif0/i9aIX7SJK1zBzu2bkO4ko7ZgWEvGVBkNCcM+pamGPEASFHGGUGWEKoFKEG8wwWI0wZCCJCgWp4ZAgmiO"
    L"kGNMjrUZGhI0JKAZaIqzKd0k7j7jUhKXkGqGlRQjMTzYYJFgMRqBZ0L8aHSgGfMiMrDHO8jgXTfBGlVs7cUEj1WPqUOAVU/mhcQL"
    L"GjKCyzFakWpJUo2ShuXkw0M8NjPCyFVvZi6fpmMn6DrPYtvq+o1bF975ztH0kUOPukOHxbxc2dvL9kDb3/eLFcATK5LfWuh0C6xJ"
    L"zk+c1cu3b8ZoBxMWyTOLSTKM6TVSxr66fi5zgYr4gsG8/dgs0vv8wpfROycg7lhbv7QlJY/oIMO/VKf53p2bpRcZCjX9G3uHRCqM"
    L"ZBBSbOqoTEHXNTk/6dl++XUsdkrSNGdycsYtG18u84vFvzx48LfOnTr1kcYjj7x8f/TLZgCI6gFVc8k1t7SXLR+/3/tSFhamw1DL"
    L"suPyjZSd8wyPNJEsgzQl2AQvltCrsqnWu0VfJNPgAhEHS29pTANeUGblv/BxSfL9Sl5KfQSdVIgpCC7BJkOUZh5pBY6fCwy1NrB+"
    L"7VYW5hwiuc7OLphWa6i7atXKbx44gPHrp7VHArymDOAO7hDu+ET2fpGiMTb0j4WwWFaLOjt9hpt27yDRWYytSFKLJLavFvLSO90j"
    L"xMXXAZJVX0yFIwP1GLlARy21DiDW182AgveFekG54HG+t1ePsA8iKLZ+rSXWZHQLcGmFDmU8cdyxbdtNzM2UqE+pSlVrrcnzxuev"
    L"uebyz86NbsznOOzv2PPyjQl72QxgJ4/Kowc/Xe1STd+y44bPBg1n8sTa6fOnwrKWZdPqUYxbJMsUH7qR9evn8b0+Qn9Bx86Sdl4v"
    L"8Abx+1Fho/XotsGQcbFK6GIlz8Xu5QKl71/FKC6a1KQMdIp8J6F/DfyC9GTisXAcJMHZlFnXYNatYetlN9JZcKRpi/Pnz+uqVSvF"
    L"FeYuVZX/4bYPBR55eY3zZTOAfRwKhzmkw0eetQCtseHPWLFiQtDu3Cw37rgKtzjJWCMwNmRo5kKeJVi7tHP7XUH9YY0vLvxg4Gyg"
    L"uOD0qdtey/XFM/76OfuLLs93EQ7+iu6j97eoBhxxPJ2EFNGcECrS3JIPreMr951nw7abaY2uiNkPGqanp8Um5ok1q8Yfv+MOZPv2"
    L"t7udO3nZjox7eTHAAZFbD+wxz14C+/SQXfRzH0Nl0gRDMb8Yrth8KataCZmfYeWwJTculoBrTZwY6beE99cuaD1DSC5s2Xphwxn9"
    L"casDizloAxccBNUb2Pg9PJfzgs7onp8wFSoVaI4JLXzoYjJPu1zG5Nw6LrtmD+enprAJTE6d962hjBUrxh6/6rqt396373Cyf/9+"
    L"+pHkNWUAAtyGvWL9gjx3ySX+3CO3Nt/zlg8+k1v7++qxxUKhrl1w03VXYYppmrYgMw4JFWlqsYnUpVgGZvMv5dm94UgXgLfeiQ79"
    L"l3EhHujN7P3L7u856Lvg+QIqLhqB5qjPsdaRNZVHHz3Plq3vZOWGK+iUHaz1HH/+qeSSSzcYL1O/o6qyc+cj4fBheLlPk33ZPMAj"
    L"TyPrTh0TQB/+06/KgQMHzGWp/bX5+cUFDWpnp6d0x7Z1jKUlDRyjQzlpIj1lVj/Z6y3qhQsVqVIjpj9H5y9PSOTCmZy61Hcb6lHu"
    L"qt9p2NJF3at/WZrX06ToC8kiqVvFVGMzRBwNH2sICQarKUkzY7pbcWbGcu3172VmZoHh4RYzMzN+dHRE8sx++djZf/1ncFcCBz3c"
    L"KnCHvPYMQGHnPqqDBykRcZP/8K8t7Nmzx6x46ztPked3+TSEuc6EG0kXuPbSFXSnz7FiZIQ0Nait4k5IhKbJsCbpv5chKOpipc9o"
    L"ggkJiWZYEtSYetZQPW+objSLAqw45j0e12L73L5Xi9MER4ojJZDWYo8eBqkPgLD9In/k9SVcOJmtd2KkxoOkRaU+lyhEtZdX1Av4"
    L"uhMmWDSkFL5BanMa2qWRKQyt44EzGcOX3szIqo249jxGRU+fPi9r12zoTE3Nf+yn9/6bLozXbv8eBwfDa9IDXBSXdO/evUFEdMUl"
    L"q35tZna2U7kiOfXcCb15927GRxLws4wOJTTTDGvyyPQl9XpcHOIviPWDwM0sgcSey18aUrc0iqYeyzuICUJYOuHrgruXlvQfRy56"
    L"m5aGQPXL8QPj4XTAMwxeVoSmEbSrdKsFOqbLbDnC82eG2XHDzai0aWZN5ufazM8vmOXLV/i//td//HMxifmx79mR8uZ7GQLvvPPO"
    L"9KarbzozNDRyZ5bl0lmsHM6z5+3XUi4+y+plltG8SW6HYz0+UyTxLyBu4vu+NFhZROi1fpr6v/6uDHVdPkjEFD70J3aGoBfedZWw"
    L"L0iQZMAjDN4XgksuAJS+X7NgcAbwoGaAKFZNXX1Y1rDFrhjjoWccaze9l42br6LTnaCRNTj+3HHduHEDzWbz9G/8xv8mNX6Q16MB"
    L"MD4+HgDZuHH9xzudzozRzJx6/oRu27KCTastdM8ylqdkpJEizWMNfqnActEUjbqfoKeiHSR++ju8f9xXbzGkP5VzcPG1nvuLpAML"
    L"bwd2f/31CwBnzwbqfoVer/PAASEvBBfxuRo+pZEZZKTBmU7KY89nXL3rR5idCwQc8wvTWhSlbtmyZfq+++577/79P9+O9i76uvQA"
    L"+/fvD4cOHTJXX331cVX9vSxvWlcUvrtwnrfesI1y5iTjuTKcG4YaKaRgUiFPU6wd6KpZQlv9gXr9oYqYAW1ofYZffcSrhjosBAh1"
    L"zO4RRUEVH4h1AzWo2ro4mtSG0LsTBNs/+w9dOjFUe+cMhap/JN3g5LMLR9Ab1GWUWiIjy/nawx02bn8ny1dvob2oNLJlPProfX7L"
    L"lg3WOf9vPvzhDz999913m+/l4n/PPQCgn//8542q2jVrNn+88n6i6x0TZ87qto3r2LllNWHhFKtGYWTY9OfpZHlKkhh68rxBkm6p"
    L"NCTxvKFalBNnEdTizPp7F7zM/sDHaAg946Cu4UPSrzgqNo6trW96h02rucADxFm+FaqudvtciAd0AK+oILZFPraabz/vOTWzmre/"
    L"88PMzk/SzFucPT2nnXKejZvWnpudnf1NVbV16zevZwPgrrvuCkeOHJHdu695SrPk39tmI+l2vJ88NcG7bn4zTZmiaadImGMob5Da"
    L"tCaHbPyY1KeKCv3GCanPD5beiFWJO1TUstTUNQDqLhBxy9JkLpL682QAB9gLv/+iWGCgvCM6cCTcRYdOECeQWmvj5K/hEVy+mfse"
    L"Ud5yy08gkiOhQyKOY8eO6o7rdibn50/8zbe97W1PAnrw4MuL+F8NAzCA7tmzx995553p4i03/c9nZqY/Z8xQsjhb+uVDLW7YsY65"
    L"6cfYsGaE3GY08yGsFdI0IUlS0tRievN1TK+Bc2CmrtiBBsslo7iQQh5E84O9FEI8rMr0j5QXiYKQnjAkikPkwiLVYLIvA8fBDB4G"
    L"0R8HD4lNEAvJWML932qTNt7ENdf/AN3uAqnAqece98tX5KE5NvL5p+bO3KOqyYDrf92GAKlRlIoIp06dsu8XKa64esf9wUnZWXB6"
    L"5uRJdt1wJRvXDkM1z0huyazF1Ls/SQRbt3xJv7euZsP6Z/IuLbDIUlrXr/73VL71+yk94qaWgkXlZy/uJwMg8OKdbwZm4Q8OlawN"
    L"avD4l3jmbCSdiO3httFgsrLc92ibm275MdrdEtWKhITHH3uEH3zPDyQmbX5z/9v2dw4fPvw998yvCAhk6Vx7PXjwYLHvK8ebu7Zd"
    L"/o8np07+M0kW7ez8RFW0K9518610zz/DlvE2440Ki6WRJSSJJ80djTyQJUpiBKsJVi3WBowJWKNYExsso+CznrGnCXlI46AlE0e5"
    L"JiJEGikFaaCmCdJEyYH8ojRwMPvo4QKDqomqQkmwmtQklWC1wKqLolTNQVoEO4RLhimwMLyFT31jA+uv/e9YsX4N3fYcedbQBx77"
    L"lo6sWudOnJz87c9/9Yv/XFVl3759Ff1D6l/fIeACcuj+b3SDCO0Tp779r4JW56uqa86eOqkrR0e46bqrqGZOsn5Vi1ZmEI11ApEE"
    L"Y1NMksQOn8SSJGAt/Y/WSo0bpP95auLQhT6eqOcAWDFYSTAmiTq8Ov27AOz1pknWDaMMpJz9Vt+aYRStO4pMFVmJ0MCELKariaNC"
    L"aSzfxrefg4VqFbvf+g66VZeRkWFm5+fDYrcrmy657HPXXHPjR35+/09Pioh+r5H/KwoCB68nr93u/+Yz5P/gHxw8s27V8l9vNho2"
    L"VIWfPPM8b77uKkbSgGufZ8W40MygmbXIkmFsMkyatUjylCRT0gyy1JKlljxLyDNb30n/6yyzJIkhTQ1pYklTS2IkGkUSwaW1vUVP"
    L"loggBokhcyE4lLQeBGeiClgTVFOUJI7CIUe0hdGE1CpqHUMr1zBTrecLX51h79730MgTEpuQpilHv/EN3bxp09Tc3OzvqKpV1cAr"
    L"fL2iBsAe9MsVeuCAJte++YZ/3cizR4rOAq4zH8LiNO97+5spZo4zPuRYPprSSCyNfIgkaWJtFnFB5klSJU0MaWJIrPTv3tdp0hu7"
    L"Er1CksbvWxuLT4kRkt6kULFxlkDPAExaH1qR9G/tgUUSVOJM0pghpAgJaI7XYYLk9XhYh00zTL6cgnX8p888zpadP8TmS69gfn6O"
    L"PG9w//33U1XVgrX2k81mMxGRcOTIEfn+NgCQJ7fjd+5ERUbOjjD306tWrEymzp9hYeosG1eNcsuNV9CeeIa1YxmtFJpZQppEV24S"
    L"RZKAtYEkEZIkhoEkhSwzpKks3Un0Emli4+InQpZajIUkiUYStQXJCziAPgvYYwdNMsAFRM1/CAkaUoKmhNBCWEvlUzRz+DRQhCGa"
    L"I9fzn782h29cxtve/dfodAuGh4Y4ffq0m5ub0927d39mfHz8T1etWvUnBw4ckL179/rvdwMIQNj/Y+IffvhQdsXbbrs/y8w/HRkZ"
    L"MbMz0/7E8WfZffVlrBsWutMnWLMspZU6Wg2h1UxoNRJaeUqeJ2SZkGWGLIuhoNFI+neeWfJGQp4ZGllCnljyNKGRp3U4MDRyW4tQ"
    L"YiPI0u5P+kagalAiDokGEkNF0N4dQ4AwjC/GMMkQPg34RoNkdDtPHR/h+KkR9rx7f92GJgQf9LHHHjMrVqyoHnrooX922WWX/en2"
    L"7dvLOud/xZWqr6wBSF3RCSqHDz/iRMRNf/qXD4ytWHVOTWInZ2bD5Mnn2feePTT8HKaaZNlwYKhZMdQIDOexXbqV5zTytL4TGnnS"
    L"xwRZGuN/niZkaUKWWdI8zhDOUhNxQ/1vaVLL000Szy7uuX6ToSYahNZZwRIrmMRuIEljM4mkiFqy1jJsYwyfjOEbG1hgI3/82ce5"
    L"ee9+1qzdTAhdGnnGN44dDStXrjTr1q2b/dSnPvXcnXfemV4EmvUVXZJX8sl6sn8RUTTIgfqwrb1fefQaa+b/ZOLsuTUrhht286Yt"
    L"craj/PZ//Azrd76JiUWhdAmh0njuj5gYt+vG0heblyRYDBZshVpFxGIlRXwgtSXed3jfjV1+dOc38VUbY2zU6/dOCiUeTx9PLoun"
    L"lKrTelyd9sfUaPCImjgLoJXj0iZpayu/9W++yMZL9vKB236CjmvTbBoeefBb4YknnjTXXXf9xMmTJ9/7rne965j33ojIKw7+XjUM"
    L"sMSoIAeBR59/Pt1z8877u6b1DzdsvjRpzy2U508+z/LhBj/0rluYOv0Uy0cMIy1hqJEy3GzRzPO4y/P6rlH/0h09QppasjSGgKy+"
    L"k9SSJglJmpAkSb2zBwDfIAA0ca4QtbBEeru/Hk4ffzer+aEu1rRoDV/JH/3pw4wsv4L3//CPsNCdp5EaJk6f0wcfeNBfddXVUxMT"
    L"E+9/xzve8aovPnwvTw59cdFIuCAcHDjA4Z07y6Oq6S74w0ce+/a/7S4f/8mphTnvzhy3b7nyKubOn+bBpx9k4/brORVK5tVi0pRs"
    L"QLRTHzgSiZpeSVc8IlU9fCmNdK9CYuOYuCo04rgyGY3H1KnEIU4S2zipZWOCB+0gXsDFx/ZVhc1S1JYojm5ZYNNlNEe28o1vnmVi"
    L"eoif+JmfpdPNSCygji98/jPhxhvfkhZFcfjmm2/+xu/+7u826hEv/DdjAC+4Dh5UQG7ftYtjx451gJ8+cuSIKQN/oygL/+xT37a3"
    L"7L6O85PTnD/xJJuv2MXTZ2cwmcX7+oQvjQtLLe7QHpUrICbUPH8Sy8chkKSRpjVBMDaLu1myenwtcbx9/wwAUx9y3StHx/MCk0Ye"
    L"zzWWBiY1GBzNlVfw8MOn+NLXv8WP/o2/R9Ycpt12NLOUv/jsn/s1a9YamyX/7q//yI/+Ys31d3kNXOZVfn4F/LFjx0I9bi7MzQ3f"
    L"PrZsebnQ7trFxUU/Oz3JB3/oPTSoWDj3HJevGWM4gWYjp9FokDea8W42yfOcZiOllac0GymNZk6jkdNs5DSbef11RiOP38/zdInl"
    L"Mxbp3YMEkElBcrBpRCxpwIVuLAmkLZy2aK3eyVNPV/zJp77Bj/71n2HD5i2R7RtL+epXvxzmZuftlk2Xdf/Hn/qZjz755JOFMcbx"
    L"Qq3Zf5MG0MMFQUT45V/+5ey223YVkg79yPDwSOG9s7MzU3r+5HH2f+AHWTzzDOXEM6waNuSpjYucZ3ExG1m9uAmNhtBoGBqNlGYr"
    L"o9HMaDRTGo2UvJGRNxIajZQsj2hfbIz1cZppUhtDDxdkqOSoZJBYNBG8AdIGhc/JR7Zw5nTgk//hKO/70O1s3nojMwsdGq2cY8eO"
    L"6eOPP+Z377qpa1z6E//oPb86d/fddyeq+ooD8NdEFvAdLjuQ/sg+VTks4r/0pa++35WLnyzKMm80WsnI+EqxWZM//JNPMbJlJ/ma"
    L"7Sy0u2AySh8Qa7D1EGpLPVbWgpF6cVUQVaxVBIcLyu5L2tyy5mFwi3WF0cVWb2JoiYdD1xNGtCS4NiIBHyySLidNVnHyzCL//g8+"
    L"z9t/8O9y7Y03Mr84Sz6UcOrECf+Ne+9l2yXbHtDZ/BcvW7vZrrtx1X+ObQ6vLvB77WCAeA2yX3oY5Hef0cYtl8qn77336J8tS82+"
    L"s2fPVMn8TDoyqvzIe9/Bf/z8V2iqsGbjJUwvdBFr0XrAozWKrcfS+3popKlLx4kIIp4kySid68f1mAX0dGW+NkVbn/Zl65NM676C"
    L"ACQtsuY6Tp5s8wf/8W7e8gMf5Nrrd3H23DzjK4aYmjjD3Xd/Ltz8lptTX+qv737/7i8pmtYp8Gtm8V8rIeAF1yeexX3805/Ot75p"
    L"199O0sZftIaG06LouMmJs6jr8tP7bmPhxLeZPv4Yl6wdZaRhaKQD1cEEbBL1BGlmSbJIDydpXRdIIElj1bDHBPbSQZW0jvsxJTRJ"
    L"QhChcCB2jMBy8rHLeOp4m9//g8+x5137uPr6tzE5Mcv4WItzp0/xhc9+zr11903pUKt5ZP7p+f+oRzXlEEEQfa291689A7gDuecI"
    L"4d/+p9Vhpcjc7/7Z0Q8mafM/2yRPKueqhfkFZs4e5+/sfz/V9PM88eDXWbWsSZ4Kab3gWRYZwCxParo4Jc0Tsjwly5L4eZaQpkl9"
    L"UnnSB4NiUqSmhJGUgKCJYJIc7AiN5ZfxwH3P84nf+WPef9t/z5bLrmehUzEyIsxNn+DL93zO7bru2mR8eOyLf3Tnx374HfvfsbD/"
    L"nxFk/+HX4l57bQCRF163Jhw4EvbtIzm8E/fuX/ql5q/82N/4M/Bvn5udqYYzTYdbOa3VW/jUPd9gskq45OobCEmDMoTYbmZjF2WU"
    L"dSWYWtsrJg6tLp3nqlXzXD/yKBKKyBvgqE9njZpDTVCUUisSGcayjHuOPMj99z3LBz70k4yMr2NiZoGx5WN0507zhc9/zt1009uS"
    L"5WPLv/jz2y//oU8h8/5AMHJkj+HnViv7D/s3DOCv/ncpAvuC2kMQ7rjrrub7r7/hU8bK3oXZaVp5jkky1m26hC/dex/fOn6Cbdfs"
    L"YnjtBuado1Mq1jbIG1mcL2xiOdhY0FARRLhi2QI3jj6JVvMEuihlVBclTdQJ6i0hGYJ0jLkF5Q8P/RmEYf7aD/84pROmZmYZHmky"
    L"NTPFg/ffV1199TXpqlUr7/nHv/j3bvudP/7jxYGY/5KHOn8vEfhr+nr04EF99NGddlXjb7rJyav/8JJLNt8wN99e7wKaGWsWZ6Zk"
    L"5xWXsXJslKNH72V+YZ6169aQ5c367MJ4qkovpTeJ4NWj1jKel6y3U0CINSETKd+yEpQWSTaGyVdy9P7n+Pd/8Cm2bLmGd/3gh5hb"
    L"qJhf7DAyOsTpM8d59OGH3HXX706XLVtx5F/8X//XB+/6/d+f3/HII+bq/fvDa/39fc0bAMCjjx42cKt5xzveUb7rXe/+vZ/6qb/V"
    L"3LB+w975+blgRJibn5fRsTF2XHMNx08c5/HHHmFoZJihsdiA6n1Jmluw8WiXIAFNUkazLqtkIk4TT3KCpCgNDCOkQ5uYnSg5/MlP"
    L"8dC3nuGHb9vHdde8iTOnJilLx/j4OE899YQ+9u1H3C233JxmWf7lX/qlX/rA7//+789/+MMfzg4ePuxeD+/t68IAAH3uuefCjh07"
    L"ZM+ePXL11W95oDmcjM5Mz1ybxMN1fKe9aJwrufbqKxkfafLYo48wOTnJSCNl1fIxmlkCwSNGCN6BGJalFWvsPGnWxCQtQshJkmVM"
    L"zyj3fOEoX/rSw6xadSkfuG0fWTrC8eNnaDRGsNbywAMP6Jmzp3jPe99tFf/F2z7w9vd/5SsPzO/YcVV2zz33VK/h8Pq6wAAvUkHs"
    L"8wW9zCXcd+9971Nf/rOFhYVrhlrNoBbjURpDTZKhZTzx/HkeeewxkkaT1WvXsnL9epqj40iSEEzCuqEOV49MAcrM3CynT09w8vg5"
    L"nn/+PKPDa7jhurfQaA5xbvI8YixDQ6NMT83w0EMPFps2b8hXrBj7xpXXXPULv3nXTz37D37+8NkDB1QOHnxl9PzfT0TQX6leMPB1"
    L"pI1/67daN775xj/77X/0T+679n23PjA/v7gmSQ3NoYbvzs9aWehyxao1XL76Bzh59iwnTp3hxLlzOGPBGmzeYqJVcVzOcu7cWcoi"
    L"0GyOsHrlBt781usYaa1kenqBiekJRseGaTZbPPnkk/7xxx/X6667Nl+9ZpWePXvqV4ayoa8denhHpnpAXmskz/eLB3ihRSiy+3aS"
    L"XXyEO++8U39z/x2Nm/7HD/zfQYufKMsOeSP1zWwIX4j1quStFlmjSRkc3aqi8C6eLRw6hGqKodYwjUYTa1KSJGN+rk2nXZLlTRqN"
    L"BvNzczz40EO6atVKufGGG0iT5I9OnDr9r9785hs/r/pwBle7C0rdbxjA99wA7LFjmLt272J639Fw6DBBSPTJx47fPHH+uX/QrRZ/"
    L"2KohOHR02biqBnHeidMQ5eFZSqinkRlJ4vk93uFcRZaltIZaqHpmZ2f07NlzeurkmXDllVclGzau/7Ox0bGHN2zY9A8BPXr0znT3"
    L"7tvd0rkhr78reT3+0UeOIHv2wLoDx/Sug8eM8DTg2HalfBn44De+ceyfdjvtWycmz7+tE5wMtZpkjcw1sswaiXMKvXeCphhp0UhT"
    L"TA6I6tzcFGdOnQynTh8PIj7dtGmzvOtdP25Onz712fNTT/2LHVd98HNHT36gtWv9+uqOO273cEBeTzH/+8UDxJMDDV51iWNRVXP7"
    L"7XfZu+66vQL40te+9oNVp/P3nHPvttaamZlpjIVmo0mWpVibOlcZqqqk3V60i+1FMQbGl42xYuUyGs3s/Ojo6FMbNm/52IMP3fvE"
    L"jdf8xcPPM5pv4u+XvMzj2t7wAP81VntBvB2cIhqnOqmq+YVf+IX0lre85bPAZ7/85S9/oNFo7Q4h/B1VVk5NTbqyKFt5I0+894wt"
    L"G2N4pMXYspH2unXr0oWFhc+sXr32y5/61P/3//7cz/3c873HP3DggDl48O93+D66hO/jq1YZ9aXWd95557qtW7c2h4aGirIs33n5"
    L"5dveMzMzV65cuTI5efLk15944ok/MsbkH/7wh08CRf3+yKFDh5JHHnkkHDx48GWd0vnG9coYuBw6dMgePXo0/a8hwI4ePZqqqnwH"
    L"TkLeeGtfP1e/0f/AgQNGVUVV+0ahqkl927o1u9cbNjgg4OLHe8MA3rjeuN643rjeuN643rjeuN64Xr/X/w8cuczUUknjTgAAAABJ"
    L"RU5ErkJggg==";

// The supplied orange exclamation shield, with its white backdrop removed and alpha preserved.
// Its sizes match the existing icon layout tiers.
static const WCHAR kShieldOrange_16B64[] = L"iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAADCUlEQVR4nG1TTWxUVRT+zr33PeeH+WFe27SWSWtINRhEQBRSWXQU"
    L"wWBMCIquJC5MCCsT48LExWxJ1MSokdS4wBii6dgNBTWhkGK1JFhCUaz8FKamNGMJ05kOM7437717j4sySKTf6pyc7zvJd36A+8DM"
    L"YnJy0mrlI6eKu0fOzO9q5YODg1Y+nxf3awgr4PCRie6n19U/y4oTe/5ZquCm9XphOr777QNbqPR/7n8NmGm2it7GQnFfTC2925uZ"
    L"adczn5pm8Sf4cqOYib2xUE49/0Ex9uTwwU00e1fLxMwEgBbmp0ai+vxziXg6Qon1aFbmdOnPY9ItnkTmzjUtwohsOv2oZbZ6c3LT"
    L"0RdfeuUtZhaCiPgIYDfLo9uStU8iQfWqb25NcOmvS/LwaBc+H+9DSHHpBzbj4km/beLjiPBu58BMRGQUAPx2FrSNam6oFDfdK0qE"
    L"J6h6HRj/OYnV/mVQ1oNuRijUjtKuYjIctJwrAEjehOZuZcgYMu4ch6Up3L5gcPYMYfujCsoQNDOkRbDjcSJlhRAWA4BiBhHZ/t7x"
    L"Q2WCzOqyZm+WoDyDpONgbW8KVvJvsEgAAVilOyCi6TI4BOfzQoyN5SUQoIHs70F1NetbHoMULMGwk+1IOx2wHAdWdyfszjiLnj42"
    L"0c4rADA2MCAEMAAAWDQbhqumj6SpkU+r0LUKOLi5gWd77kC2d0Bm0lApm6qpzeTKruP3ZpDL5TQz04Hz+D7jbpzekPx1nam7mugh"
    L"Ob0YxRq0QSUMQh0YTvWKEj9xzU0/8iMvb0ELAIxCQXyxhYJKbPv7tZ49FEtHjZEJWHYUbEVhDEBhXdfaXqXAfuy919aTDxSWtS0M"
    L"DQ1JABg9fford+pD9r7Z0ax82W+WvttpGj/0N+vTH/G5C5ePLnNZPnDKzEyFQkE0GvustX1jx57JLr4gF/9A6FXQTD2O6/7WX+ZD"
    L"Z+fLTz3sAWAi4gee6a4vzueH7NyO7CEnVn8z1CwbpuPbG1cX3tm/f1ejxVnpCe81acVfD19cc/zUjZ6Vai38C3GuaXgcRTvxAAAA"
    L"AElFTkSuQmCC";
static const WCHAR kShieldOrange_48B64[] = L"iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAPf0lEQVR4nM2aa4xdV3XHf2vv87iPuTNjz8N2Epr4EUiwiSGBhBCK"
    L"x2pJgklAUDlULTQRlNIPPFSoglqpmoyEqqpUqPlAVWhQAqjQjpWQAG1SUnVGpEBStwHkhDYvx/FrbM/zPs+955y9Vz+cO8RqPLET"
    L"TNstjebOmXPP/v/XWv+11t77wHkeqiozMzPBma6pqpzv+c7rUFVz+t93331r6e67by2dfm1yctJwHsd5s8jMzEywe/fufHp6emjD"
    L"2LpPZy54r9NgVIwQ0jsE2d8nPf/lPXv29FbvPR/z/sIEJicnzR133CEi4h5++MGJKCh9uTq86VLyRSI9TJo6umYLpfJ6GkuHH0+6"
    L"7dve/e73HZienrZ79+71IqL/VwRkZmbGrlpyZuZ7Hw/DgTujuGJG7b9kI+n91nSew6UZdX+BHpX3qx37jSBtLzaTzvInb3jXe+8p"
    L"vveLeeMVE1BVmZ2d/Tnwe++996KxkfV/UR5Y9wGXJ7q19s863H3I9NotfK+NT+qYrIsT5aC5ybVGP2XjsESzfvzvlPAz119//XFV"
    L"lX379plX45Hg7LcUoPft22fGxsZERHIgv/POO+Ndu97+e87xJ2KisYAlt3X9fhN1D5pUNkNwAu1mqIdepkgesJUH7PHuQT018gd+"
    L"eOyq32wuH3nHt79976SI3AU4KDwyMTHhAT0XMmt6oB/bBvAi4levf/7zn6necMMH3xfH5U9HceVNy0sLjA423YWDR6xPFsjTBE2O"
    L"4jtH8O3juNZJXGsZzRzeRYSa0QoGOTr0285tuMWGQZmVxSOP9pLmFySo/8N73vOxzukYtm/fLrfccot7xQROH1//+p2DV17x9jcO"
    L"jYzcIBJ9IAjLW1dWltDspLtwHDNQ9pJ3MzRroMkRfHIE3zmMbx/DteZwrUV8t4dPPT63GGeQIGe+skOXxj7og7Fd1ntYWTj2TLO+"
    L"8Ldpnjw0Kt3/3POhTzVOw3lGb7yEgKqKiHDgwIEt4+OjH69WB9+Y5VwmJtwoYlicP4TrPOtGB9syPDxs1G7CUwVXR7vHcZ1j+OQw"
    L"vn0Y1z6Kb83hWgv4bgI9h+t51IHmAYE4sihgZfBNvjF6E7L+GmPiEeorK/SS9hzae3pxYeGbt91225f6uF5C4kwasEC+fv36Px4f"
    L"3/jhE3NHyLonSVvPaolDbiReMQNDg5boEvK8jDExYkC1iLLQOtRkqO2htovaLj5MyJwnyUKMyXGZR31O6gz0lJHWv5nhhcdJBn7F"
    L"J4M7vRneaX1ty6a4tnlTKS7t+sY3vvGvIvLk5OSkmZqa8mcjoABxqTK2snwyz1/4iB8pL4WBKYkJ1geei8nyDRgbYYIMzesggmZN"
    L"NG/x/MmIU/MbaNc97XqJTmOYRv0iLh88yBVjK/QkQKxDvKJOUedIc4MkSrlxyFRPHTSUH8QNVvSFC25PyyPXhbVOYzfw5MTExDkR"
    L"8ABZ5i/K3WIwFB11gUSS+xLiaojEGPXgu2jeApeimiGuTd5Z4Km5cQ6fCFhegsWlEisrNRabyvFBz+sHH0OpgBhEwBjweFCPphmp"
    L"U9QZTBOiblfcwEnrh7zJs7RypvhfiwAA3W7qJK8TZw5vFFwPyMEq6h24BNVFRB3qeqjr4JMVum1L0kpIO/NknWXyzhKSCdYlZKnD"
    L"mEIDeAEVMAEYjwqQ5fgkxQtYE5N1e+AdA7XBbQCzs7MvwblmY9VJEul166CZ4Byap+A9+BxcB02X0eQYvn0I334e3z6Mtg/TaS6x"
    L"vNxgeaXBSr1Jo9mm3ekhPsV6hdyhuUO9B1UEEGMQGyAmALVoVtjK5RmIYMRWACYmJs7dA71uj6JmgXotQiVPwLRRLyDNgkjehrxX"
    L"EOw1aNfXs7CotBqL1OtN0l5C6iGkh8GRpw7NBM0pspET1Bs0t2imiLPF81UQLZKOivq1cK5JIHe5qO1/TwEPmveAFpgMNEfTlb54"
    L"U8hzJG3QWN7E0kpE2m6RJAmtxgqnGgukwy10Rch6HnJBvaAeUAWhAO0tSIAIiEjhHdYoAGcj4L2iZhW8FvMoqOsWeshaaG8BnzbQ"
    L"PEPzHOtadJoLzM1ZFuae49T8Iu1WgyQPyTaXkFzAK4KiKgiCmn4pMkAgiLOF9Y1BrAER8IUrzqSBtXshRRFbPNlrQUT7FvMpmtVx"
    L"3UU0bYDPcKkn6GUcee4gP/j+cUQ7KIbACmEcEwQgUlgbUcQoCogKSPFbMWhYeEOsIQwjvDGoNWviXFPEYrz3RHgCBS1E1xce3qN5"
    L"DlqEkmvkpEdz3JxC0sE5RxiGhGFIHMdUq1UqISCCGCCwSGAx1iCBgCkAm8BgQouJAjS0OFsiMIYsS4/DmUX8EgKzs7MWoNvtPGts"
    L"jdwHKjhQRVkloGAcmnrSY0p6BFxbMCi1koCxCBDHMeVKhWq1QiXsZ5sgwIQWG1okNJig+Gyi/k8YFATiiNTUCANLnrm5tQy9pmus"
    L"0FWp4KgidEGLggOKGsU1oHcIfBtW1SYKkVUQIYwiSqUSlXKJqFqlFC5iggDjA8QIEGDU9iEYlCL2sR4jBhdH2MowqKM6UF1zwfMy"
    L"Ik6fVRmhp6OILKCusDrGky9CNgfkfR86fp4qytYjQUAcW0qlEtVqldJAlTg0YAPEhhhriwKG7QM3BQkP3joCa+mVq0hpncmzlDTP"
    L"ngWYn58/ezM3MTGhAJp3n8udoes3iZqnUFUkz8mXPa4OGCliuq/vVQLV0BNGEVG0SmCA6kCFaiwQRohEmDAsCIil2MiQIis5Ra3B"
    L"RkJeHdagtE563Y6vVofmAPbu3Xt2AnfccYcC1FtzP7Xljbn4LdblP1RylWwlR1PXV46hUKS8mKg9lAIlCkOisCBQrlSolEvEsUUi"
    L"gzElTBiA7RNAQA2qoLnDBGDL0KtsIiqvk+bSieULtmx+HoracFYCU1NTCtDt/uMzve5rn4/Dyy/tdmreNDrimzkSZ6AhIpYizfYf"
    L"qqAI5UAJwpAgtMRxTByXKMchpVWR2hiJwoKAsUUt8CBe8dZgVJCqp1XaolFUFkSf2rJly0p/PfCSinymNKqqam+5ZZ9Tl/zIVLdo"
    L"vXeht2pRcsh7RToVW5DAFOIrTERgIDRCFFiiwBJHAWFgCCKLxCUkjpFSjCnFmFKExCEmDpDIIpElLAf0oip5ebu34tUG4Y9U9efZ"
    L"8VwIMDs7KwDd9vJ9uQ9licuNhEW+VtcDnxXhI+GLYSSFEEPxJBrR1ZgeMZmpkGSCisH0CZgoRuIIicLCG6EFaxBriSqGRXshVDab"
    L"dmtJKpWBB+FFbZ4TgYmJCaeqsv/x7/5Tq754JBu6Vtqm6oPAgM9Ql4KCmLDoIMVggF5ueONYwkeuaLL7NS3eNnKcS+3TvCE+wKbR"
    L"GBdEmCiEPnCJAiQoWgYxgoQBvmRZCnb6cmXItFrNF6699tof9OP0jA3dGdOoiOjMzEwwNfXV7kMP7f1mNHLp7fOLl7lLSvtN3lUk"
    L"74Ip9YUYFXVCPLkKY1Xhc++o0/EhbSlRLTeJB2u4Ug01ARKGSBgUfQ5FO4FzqAhRybLgBvC1q31ochNF0ddFJJmZmQlktTU+FwL9"
    L"MPIK8pVnnrwzLq37/bD2a7Uke0JtlIjv5UUYYRAbFsCcR1UIBH62UuOup19DXYfYNgq/dcU8m4ctmQ2w1kJgiuWY+v5uEGAMtgTH"
    L"2ldobfgSuzh/qDs0vP6uVSxr4VyzF5qamvKomt/9xGeP15dO/E157Eo5EV7h4oqgIuDTIv8EEZgYNCAQaOYxt8+O8K3/WOHgXINH"
    L"Twzzxccvpp4oQWRR2we/mhLVo16Jy5aT6QB+3a/7aklEkXuvuuqqF6anp+3/XAefEwEoaoKqypFjz36hvjzfaAzfZJrRsIblsEg+"
    L"eMSEGBujJqIUCj9eqPHEsTZjFU9toML4ADQY5cCJkCiiIN/vSNV7cA4RwcRwKHuLjoxtk/lTx/MNGzb+6cthOycCU1NTfnZ21n7i"
    L"E5893m0vfak68npzJNrlwqESUqpgjEEkQKISEoQgEbFxhGFEuVyhUi5TGxxkoFrBeYfSt3x/jYFzOKeUq5aD7VFKG27yoUlNJ+n9"
    L"9Y4dO342PT1tX25X7qwEoMhIk5OTxtj4z5bmD5/Ix/aYk9FrfXldDSlXkNAiQYyNynSJedN4yju3CWk0QmVwhKA6zqbgKNtGHbkW"
    L"XSqqaO7wuSeMQlpiOWlv8hs2XmQOH3nhxKZNmyYnJyfN3r171wydcyYgIrp9+3a58cYbl9K09ekorpij8c2+W9tAPDKGqQ5hojIm"
    L"HkDDCnEcM/mrHW7dvsjFwRFe53/AnksOsWG0hnO+D96juQMsURV+1riSza/f45srJ0TE/uGOHTuWtm/ffsaduJfgO9sNq2NmcjLY"
    L"PTWVP/jgd7+64YKtv9Odm8mvHPj3IF3u4poraLuJ73XwaYI1niCAZg5htUxpeAiiAAlNoR2f4xUGhgwHljZQuexz+eBAFDzxxIFv"
    L"XX/9je9XVSsiLxs6r5jA6tnWzTffXGs0Vn5cro5ujusPu50jx2x7IUFby2inie8leJciBmwU9ItWAJEFKyAO7x3VmuVQO6I5/kd+"
    L"67Yd8pPH989dfMnmq+65555T0M+C55MAwKqoHn744beUy6VHnAbh+vyH7BhvmPZignbq+KSNZgloXjSsViAMCvDG4/FUB5S5xHM0"
    L"+qjufPMN/qc/eUzy3O/etWvX989FuK+aAMDMzGSwe/dU/r3vPfihdetGv9bu9Nx6HjdvuCiVbr1H3m4hWYK6DNG8WIaKR41HUaoD"
    L"juOtnIP+fXrFm9/tDr/wX0G93vrorl277no1x02v6oxsdaLvfOf+D2/ceOFXOknmK/5Jdl7cMzb39FrdYq/IZahPUZ9jA0ccd3lu"
    L"UTmqN+jW173VrSwdD1rt5M+vu+66z77as7JXfci3OuH999//4QsvuOArmTdkjafdZRc17XhN0TTHZxm4FJEu7V7CU6dq9Krv8COj"
    L"G2k1l4z3fP6aa665vf+s0xam/wsETidx3333vWvjhvGvDQyOjC7MH3NVe0I2DiWmGmakacpC07KSXegr617r41CDpaVFyuXKx6++"
    L"+uov/qLHrefjnDiYmprKH3jggW2jI+v+aqA29M4sF9rtBqFVpxhViWy5FAu+R6vdeWJ8fPyTl19++cwrFewvhQC8mJ0AHnnkkVut"
    L"4WNZ7t9aKpVEREiSNt7pM4NDQ1+78sor/1JEWq8k1/+vjMnJSXP6yxyPPvrozscee+zW/fv3f+TAgQNvU9Vo9X/T09NnXB7+vxgv"
    L"B05V7fl+Y+WX9vqLqprZ2VkDxYbU+Xgv4kzjvwE7NSC6hTA88gAAAABJRU5ErkJggg==";
static const WCHAR kShieldOrange_128B64[] = L"iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAYAAADDPmHLAABGPklEQVR4nO29ebQlxX3n+YmIXO76ltqpogqoha1AFCpAIEuikIVW"
    L"azd4bNySF1rudren+4z7jHtm3C1on6Pp8XjU0/b00eJW21a3+1iFNZIstCCDqpCQZCEWi01AQUHt9V699b67ZmZEzB8Rkfe+Amls"
    L"qqBAJuDWzZt3eZn5276/7+8XkfDqeHW8Ol4dr45/oEOc6QN4KYe1Vtx///3R/fff/5z3du7cyf79+80NN9xghBD2DBzeq+PFGNZa"
    L"cd9998V/n+/s2bMn2r17t3qxjunlNH6qPYC1VgohTHj9/e9/f10cx7969OjRpNvtIqVEay2EEGbbtm3SWnvv5Zdf/i0hRNt/RVhr"
    L"xehv/LSNn0oF8ELzm3bVgw8++JHp6enrlFLXJElSV8oZt5QSAK01SikWFhYQQpyYmJj4kw0bNuzZtGnT1wHuu++++IorrsjP1Pm8"
    L"mOOnTgF2796tbrzxRg2wd+/e96dp+sdFUawsioKiKAAKIYQVQiAEWAvWliE/0lqLarVKURQ0m807Vq5c+fHNmzd/w1orAH7a8MFP"
    L"jQIIITDGREKI4rOf/eyaNWvW/Emapu/0Qs+jSMk4jmUURSKOE4QQWCuQUmJMQZb16PczikJbIYTWWss4jmW/39cTExMfu+aaa35P"
    L"CJFba5UQQp/p8z1dIzrTB3A6hrVW3HLLLUIIUXz/+9//uTzPPzsYDCaNMVYpRa1WjdM0pdPNaLdzev0Fkgik1PR6PbRJaI6fxYrJ"
    L"SSJViIWF+SjLLEIILYQQSql/c8cdd1x1xx13fEgIMW2tjbwSvOK9wSveA+zZsye67rrrCmtt9MADD/yb6enp/61arSpjTJEkSdRs"
    L"Npifb9FqdRirdVg/OcVE/BhRfgihu2jdZq6dcmD2LKa7FxE3XsPmLedTTQ1TU1NYBNbaIs/zqFarza9evfpDF1988e0f/ehH5S23"
    L"3GJf6SHhFa0AwR1/5CMfiX/913/9C2mavmt2dtYAYmJiQoDlwIHDrF6huPScQ4zb71IsPEzRnUfnA9AGgSSKJFFF0bUN9s+u5cED"
    L"26mvewc7LruUuZmjLC51SOJYDwYDlee5Xrdu3cd27tz5e0KIPCjgmb4WL3S8IhXAAzIhhDAPPPDA9QsLC/9rmqa7+v1+JqVMVq9e"
    L"xcLCAtPHT/C611ZYn36DwfFvkS21QAuwBms1aI3VBVYbjBVEEVTGY0irfO/JNTw2/3Nc+7O/gCkWOXZsmjiObb/ft+Pj47LX630t"
    L"juMPv+lNbzqBSxdfkQBRnukD+PuO3bt3K4/izRe+8IUPttvtr1Wr1V2DwUAnSZKsW7eOI0eO0l5q867rFOuK/0L72b3kXYEQDYRK"
    L"EDJCSgVSOhMQFmkNeqBpT/XpHl/i2s2HeNuWP+Obt/8+rY5ky+ZzKYpcVCoVubS0VPT7/XdorZ949NFH3+uPx74SyaNXjAfYvXu3"
    L"CjTtH//xH6/dvHnzZ5IkeVdRFMZaa+v1umo2Gzz2o8dZv24l11wyw+DQn1B02ggrsDoHNNZkUAywJsPqAbbI3XuFwRqBNYAVWCFp"
    L"TCR0VIUvPvgaVm77p7x2x0UcePYpBplGKam73a5KkoR6vX77ueee+2vr169/xXmDl70CBP4+EDF33nnn+yuVymeMMZNaGwtWrFq1"
    L"kn6/z1NPP8vOHeeybd2j9A7+FRQaYS1GF1hbgMmg6GH1AFP0oeiBzrwSFFgjnPC1cMogBEk9Jl1R4Y5HNjId/SOuu+5ttFvTTJ+Y"
    L"I0kS2+v1KIpCjI+Pz61cufLXLrnkki9Za7nvvvvinTt3Fi93RXjZKoC1Vuzdu1cFgPXYY4+9tdvt/naWZW8FKIpCK6XUihUrOHz4"
    L"EEtLXa59w2ZWiLvoHb8PUKA1YEDnYPqYooctetiiC0UfW/SxxQCrM+cFjMEYgUBhtAAN1kpUGjG2NuWHxyb43tF3ctWb/hErJyIO"
    L"HDyEEBIhhG632yqOY9I0/crGjRt/bevWrdOwnJh6OY6XpQKMXrTHH3981dTU1G8ppX53fHxcdjodo7UWzWZTWGvY9+RTrFu3mp+5"
    L"ooGZ+waD1mEkCms0WA0m98LuonUPii626Djh532s9wK6yEAXLrO3EouEQoJxhJFVivE1KTO6ztceu5QVW3+Ny3dcwsz0ERYWl0iS"
    L"xPb7ffI8F8aY+Q0bNvw/IVOw1orbbrtNvhwV4WWjAB7ZK0ALIeyePXuiNWvW/G673f6tiYmJFZ1OB621juNY1eo1jhw+wuzMHFdc"
    L"cT7nrJ8jm/oOJu+7eG80wmowGbboY4ou6K63/A4273pP0PfPA0zuMAEuqrgHAmMUaIk1YKWiNh4TTaTs3XcWB/X7ed0b3ketojl8"
    L"+AggUErpTqejqtUqnU5nz+Tk5B+86U1v+qoxBkDs3r1bvpxKzmdcAU529QB79+59R71e/1dr165989LSElrrAlCNRkO0l9o8ue9J"
    L"Vq9awc+87hxU/ijZwn6EkBijEcZ4yx9g8x5W97C6MxR+0cXqrsMCeReb9zDFAJsPMMUACg0WrAGBxCLAOEWwBkChUsXE2oRnOk3u"
    L"evpKzrrwJi65+AIWF6aZm18gjhNbuBEPBgNqtdo31qxZ839ffvnlX/OKwJ49e6Jdu3bpM60IZ0IBhLUW7xINnk598sknV/d6vX/c"
    L"arWuW7t27VsqlQq9Xi83xkTValVordm3bx/tdocrdm7nnE0WvfAoJusgrHR5vbFgczDZMqu3uu1df3jdca4/72HzHjrvQT4oswKM"
    L"dUAwFImscuDQCKxWgMAqSX0yQTQrfPvZDRzM38YlV/wc61Y3mJo6RrfbJ4oik2WZKIpC5HlOtVr9aynlf3zLW97ybSFEC5winDhx"
    L"wp4pr/BSKIAIF3IUzYdx++23r5uYmPjNer3+z9esWTMphKDb7Vprja1UqlIIePbZAxw+dIRt285hx6UbkOYweXsaIVQo54ExYHKs"
    L"GWC1t3LdweYdrO446w8KUXS8B/BeoMQDGabIPEFk3U8bARYEXgGMxBoJSIwQxJWIsdUJ07rB3qcvwK58PzuueD1pVDA9NUWWFyil"
    L"zGAwEMYYsbS0RKPRmGk2m59Yt27dH51//vknwrX46Ec/Gr373e8WO3fuLMAVuHiR6w2nWwHESGlV3X///eJkgQsheOCBByYOHz78"
    L"+q1bt/4LpdQ1Y2NjTWMMRVEUxhiRJLESQnL48CEOHDjI5OQEV+3cSmOsh24dwxqLELE7fGu9y/d5ve554Xuwp4MH8EpQdDC6A5nH"
    L"BCEEFD0fBjLQBqut8wQGrLEuI3CskfMIRmCMCxFCKapjEemKhB/NTvLg7OsY3/hOLrp4O5IBMydOkGU5Kop0URSy3W6LOI5ZXFyc"
    L"S5LkM5OTk3suu+yye1avXr108gX91Kc+FX/kIx+xOGwEp1khTpsC/KR0Z2ZmZuwzn/lM4/rrr/8VIcR1Y2NjV1lrx9asWUOWZRRF"
    L"UVhrVZLEosgLDh85wrPPHmBsrM6O15zLqtVg28coCo1UdSB21g9gtSd1MjBdj+qDtXf8thO+0R3InRLYvFOmhDbvY3KXElJknh4G"
    L"tMUa67yB9wjC4jIEK1y24LMEhELGivrKBNuo8LfH1/Lk0hWs3vJ2tm49n0hkzM6eoN/PEFJarbXOsiyKooiZmRmUUjNLS0t/un37"
    L"9vlHH330MzfffHMvjuOWL2f/na7zCxmnPQQ8+eSTY7fddlv6gQ984DcGg4ECfmbFihWv6/V6YsOGDU0hBFprjNEYY7SUSkopxNLS"
    L"EgcOHGRq6gTjY1W2X7SONWsMdI+TZzlCNRGqBqKKEAkI6YVfOMCn+04BdNfFe68EzurboNsYL3yK7hATjGYE3gPYohgRPt4bGIy2"
    L"DiBaED5TsCinANYphZUSVVE0VyYMkioPn1jFs/0raZ79s2zZciHViqDdmqe11C57D4wxKssyEUURWZZx4sSJpfXr19t2u/39OI6/"
    L"c8455xQHDx78z+9973unRuR2WjzBKSuAtVbu27cvPnr06M+effbZ/7JarV7Z6XTU2Wef3axUKmityfOcOI7IsrwQIIQUUiDoD/ri"
    L"2LEpDh48RLfbZd3aMS7a2mS82YL+FPkgCH4MVBMha6CqIBL3x4PwTR90D6v7WBME3PZeoI3VbWwe9nWweRure5B7BdA9jwF8OlgU"
    L"3v17168t1hhs4UOCxeEC42kD6zABVmCtCwtWSpJaRGNlTBanPD63iqc7lxKtfD0bzn0Nq1dNYPWApdYi3W4XrY21oH2WEHW7XQCS"
    L"JCHLMpaWlpaiKLrXGPN/3XTTTV+/5ZZbxK233nrKvYqnpADW2gjQDz744L+78MILf7darZb9dYXzXRaQUgpprcVaKzqdDsePn+D4"
    L"8Snm5+epVgTnnt3gvLMtSXQc2z5GnluEGkNEk07wqoFQDZA1kClCuiZfqzOwmbN+3cWaXmn1wfU7d9/2eMDjgLzt08IuJg8p4cCx"
    L"gsXA08I+9huLLYISWKw2niG0DiAa66+F8F7JPYwVIARCKqKqoj4ZYdOEQ/1x9rc2sRhdxtja17J63XmMjzVQQtPrtun1OgwGmdXa"
    L"YKw11lpjjBHW2khKyezs7AA4+6abbprxvY+n5AlecEeQ/+PFpz71qfFdu3b982q1aoui0NZapZRAShHpoqDd6TI3t8DCwiInZmbJ"
    L"Bz2qVVi/RnH5+Zax6gnIHySfn6dvIqSaQETjICpA4t195C4uFvClXMtwu/SGduQxtFTAO02LsAbr4Zxd9mbYFAw9rHCfEA68Wox7"
    L"FqN/SlDiXmNAWPf7QoCRWGMplgwLHY1KctaPDTh35Twd8QSH5r7BsSPrORhdRDJ5MROrzmF8cg1jk0rEkSDP+qo/6KnWYouiKGyS"
    L"JNnY2FjS7Xb/hbX2337605+OgFNqVj3llrCHHnoouu6662JrrTDGqMFgIB5+eD/T0zMMsoxI5sRRzljdsP2cgpVjS9SSE5AdpejM"
    L"0V8ARB0RjTuAJ6sgKgiZgozw9Vpchc2TPOUVLxxjYwusNWAMwnkajHEPjBkCuTKm+7hegNE4C/fWrrVz+7IUslc8YX3/IA4JIgDj"
    L"CurGP1w8QFjtgKKwDitgITfoXLLU1cgoJ65mbGt2uGByip54hJluhZmn1nLEnEURb0Q2zkXEK1i7biMXXHAB+/fvF71eLzHGiGq1"
    L"+s8WFxc//hu/8Rvzp+oFTlkBHn/8cUKXbZIk/PCHD3Fo/3fYeRnUZYt62qGatKCYxfRnyecG9LRwQpdNRNQsXbsVqUvvhCNb8MLE"
    L"aARFeVHd+9ZLrnB8v829IrjnSBaoKAfpHyoHVbiHLqAoICoc8RO7aiCFe220pdNX5Nq6zmHvFSzWeSJhEdL6LGDoTeyoAypfCKyw"
    L"jkewxnsFwyAzZEuuKVVVItbWemyoLkL6DEWc0rYphRjnW3evon/Nv+Sstat46un9AAwGg/See+4ZA+ZPVX6nrAA7d+6sKqWk42I0"
    L"WZ6z86IO5zS+TN5awPQNnVwgRIoQVZBVhKiBrCBUBRvcPDEC5dMrB7oEBoTGWXp4T/tYa73AC7AeC5gMdI4k59jMgMPTgkE/Is8i"
    L"8kFM1o/Js4R8UJAPNPnAUGSWPBPoTKILRZEn1KM+77i0x8pmSpZbn/pBCA0CgXE9SV4h8c/lBzHggaJDik6JQIiQNRiskRgMptDk"
    L"bQmRQiaaqFrQaBasOMtyYKzHzIkpNp69Fq210Frn4+Pj1W3btv0qcAuufvKCW9JesALcf//9EZB/+MMf/vCaNWuqRZHlIGJdFNji"
    L"OHppiTwTSKrIKEEQYUUM+JguY0D5fF54q3EuHGMR0uAhOMJoZ8XaDrGAte594+r81rgmj1hmHDyW8djUZlTSZBAPGJiczOZkNmMg"
    L"MjKRkcmcPCrIkpwsy8mLnCIvnBIXhvvv+AL/9vqnaNSqLgOw/hiFBGFc0cnhPKwEYb13ksZFBmu9IxBDptKCcM0iWOmxhhCglfuu"
    L"cV6ksBJd5CyJAYmW6KLvKpKuGIExRvT7/eoLld3oOGUPoLWuCNdID0gGgz7J2JITZjhB70KFkJQnbT0Is87CMMGCjLcOF1iFKLBG"
    L"OjgmtM+97dADmBxCNlAMEFGPE4uWSm2SuROHGGQZg8GAQb9PFrYHA7/t9mVZRp47Bej3e5y1fiML9e3sP/owl22togPdHMCit3wx"
    L"ci4IXCaolTtFQr6I92gWt8v4c3C/YxEgNAIJShFQpzWColNQoaDbaaG1CZNYhNaaoihOy3S1U1aAXq9nrLVobZwy64xE9bBaI2zs"
    L"vaYB4YRq0QhhEMI6pGWsQ9cSX3xxSB+Mi/tGeaDlFAy84hjjMUA29AJ6AGKALTQD+hSFRmsH7IwVaMNJD4E2AmMlxkq0FRgUWaGR"
    L"ZkC7D4PcIK37y07xPOq3XhmCjAVgJUJ6oCkkSK/QgTnEeKxg3G4z/L5TAO0SHi0QiaToGaqRZtDvYDweklJSFAWdTudURQecBgXw"
    L"DRAYo1FK0O/3XQ4uA0vi4rg1skzZhDAgPbUKPJeOcMDPxf/ccfNlGkiJ/LEGjPcC1tPB9ChyixaavCjI87x8hOlhP/l1jtYGJSxZ"
    L"DnlhSaUpy8Jlv4AXXgm/g0Lg08TRVFIa96xD7QJCEcto6/XdIiXYxPUyCGswqSGSmkz3McZ7FCEoigKtdSVMVzuVccpdwUVR4Ao5"
    L"GmMMeZ4jhWuuNFCmZ9Zoh+atdhYwkqOLkfNwZV3f0KEzMAPP9vVA93xVzxM7ntlDD4keq13blxNqVgo2z/PS3f84RQjnYoxF2NwJ"
    L"WJtSWEF4wj+XwjfChbEQKUp6QSCkJ4gQzsqJnKfQXiE02MIpgskMDDS2MJjCYguLtIJ+r4/x8V8ppRYXF1m5cuWH7r333hVCiOJU"
    L"FOGUPYCv4lEUOVGUeEuccy3XQKjWWQTCKqcQIZvz8c64BM8TKS5MYDNAI3QxpHkCPig9gAeBJRbIwfaxBWR6QJEXDAaDUvij3uDk"
    L"13meY4xBFxptDNgcFZyOF7qXQfnsuo3dOVrwWYovHY+w9RJ81uDTQQ8YhDWORg6ZgvVJTm5cKDE4atm/D56QshZjTEUpdcoe4JQV"
    L"oCgKUVpUoekszSLMAsJPwUbgT0D7cBCAX7hoAQRqkF6gQmIxLt+3wnMuGutdY2BzrM8QXMdvgS0yrB1gCkuuc7KThD2qDKPgL3gB"
    L"h2UKtHZeSAUhmuGxug3pQoHvFbBlC9mo4w8Y0FPCmKAfDjhK/CSVcJFsOVNZGEHAkBKLLjKnlCPitiElOMVxOjAAHpUSG0ukfK7L"
    L"MGYBPna62C9KK/ZKsUyguTcZ33kT+rO8EllbeCrYCcn471vjW7sZoIwmzwbkeeGRfr5M4M9n/S6LAW20r/b1iRUuHMkgUA8AA6LH"
    L"Ogv2aL8UHgwxQekJfNonQxHJgVohjFcur18ubpbwSQmJLjKMdjS0EAKlFMYYZmdnTzmEn5bZwSGeVqxD9J4GcQrrT8iFAce7CmOw"
    L"wuf3wngyR+FoV+PiLj71w5aCD14gCD8okDXah4DCcQE2Z9DvUBQF/b7LBka9wPNZf+jV00WBNpDqHokayUxMyOeDdQbuYgjqBNLr"
    L"gq82LMMEwv9vHaAV1mmCP0+LccHB/7ZPnMGKYAYuvAjHHo403pzSOC0KEEq+LncfHlg46MDuuStnsNKDQRyKd+yYQhgwVgHaL94Q"
    L"vlNgbebApC18v78uPYMNOEB7T+AFO8gy+v0BWutlbn9Z7u8QNb5aSVFoCl2QWhcCnBLb8vBFWQ62TlB26BkcVxGUY4Q3eL5hQ3cR"
    L"7llIAqPv8NKyC+j3nx6hj44XrAA7d+604FgpcEqgrdPi4LXdCGYAgb2zBOvPXcHH+DzZWwTCceyl8E2Ba/bMnRcwniU0oRHUh4VC"
    L"Y1WBMn0G/R55XpANBhQnKcAoEHSo3z0cBtBoYxEmI1G435fWW2GwRTk8NUM5pax038anjAzfo3xfOKXV0pcyDGjhQ4xFSEFYkSgI"
    L"PBSmxIvQwnlaPID13P1zFNRbwTJPIHAXQIbYX2B1mKSpfaHHW7/VhC5fR/eG4k94z5MqOvyO8wzKtOm0F9FG0ev1MNYui/9hO1i+"
    L"61ByV92FA4s0PYcB8LWAEHVysIULBbbw1m6sR+sMY395DSjDQWkLviBU4oKT2ZDAI5wELF+M8YIVYO/evQCl5YRwuOxQQ+pC0Obg"
    L"8ixherawDvUjcXHQ5sPAaXJX3dMDP72rcDjC+NjvemicdHQxxAN5zwlaSwbZAK1dqhqygOWo34G/EAKs5zJMobEDMB0orBd8sOTh"
    L"yZaArVSUUlrBm4VqYVD+sNvNPRRKDFNBcOYuKMNBqDmFIcTp9QKnxQMEJQB4Tmk6lHRDlc/vFsY6htBX+4T2rwFGkH1JBOncK4Qu"
    L"+QCM43SFn+9vtMYqgzQdBoM+hYkZDJybDwRQlmWl0MPIsoxut0u322Vxfo6eTZlM+4guaGuxMd4FGyc0YBgUfGHIU77CBuwSpCiG"
    L"NmEZKoO0QyUQgAz1AT8ZxZNIQo5K/2WkALt27QJ4DiIVQvm8dyQnDm4QhhcjoGBTuKKI8HyAFZ4NDPRuH0zfCd8UnlE0MKIE1npc"
    L"oF1KJXWHpdYCcXUFg34fC6XgwwXs9Xq0Wi0WFxdZWlpyocJb/+T6PirVRMoJyhVt/CGPQDHBcH95cmUI8NmBGGEGhmyx4wCkfyEd"
    L"r+CsXZTrFrjiGe44wjV9uShAGGUIsK55YjBwMdSxKKYkRQhM2ogeDC+cyx7KEBKaPEwfdN+3fA98B3Dh8YYtQwG+2maNJ1uKPoN+"
    L"hlUD8iJ3i0EA/X6f+fl5ZmdnabVa9PuOYw+plZSSSAWoZZFBRqWTH4ZnYYcNIGXkC/0BduQ5uHKxXPetVwxXGvZcgsBTxyOCFoKi"
    L"MF740mcqp29FmlPpBxAAUkprrZuDL4QgilNXGUwCiAl5MsuD2Qj4AXw5OCD7wjd39H3D58BP3MyHrKJv4QLtFM7i0kArkCZHkmEM"
    L"DAYDFhdbTE9PMzc3R7fbJVTVhBBEUbQ8p7ZgrPWE7TAQlyma8MdankXoFPJfJgh7lAhabrXuwlFmCgiPfb0RCSHLEGCsRSVVlA8R"
    L"gQo+XeOUPUAcxw5MGQdxhVSlultwzNgoiR6aLoP5uPwHf+U9kCtcIUj7+F/0nEKYYoT+9d/THn+E3j8hUBLyQY8Dh/fx0N/eR7fb"
    L"K91/EHy4iMvDl2PZlBDEMhyrB3Elozk89wBuhShVxZ8nOKQnlqE4acVoi6H7TembYMLfCDSxxOd/EMdR+cNKqdMaBk5ZAYIHCP9F"
    L"KsZY6Rk+KG0hXCGPgofGP5oflXkSZaFHZyNNH44NNMZ3Autho0X4I6Yn0XOwuNjmRz/az8LCImmaLrP0ky1olGK1wpau1r0ZDtMF"
    L"gLC6aIjpwdrFKPMnhAPDo3iAMp5QAkMJQ07BW4zvFBJI5wEYKm2ggK21xHGM1vqUXcEpK0C1WvXA3aKLgjStYq1EjHY7hK2QHlmz"
    L"XClGA6m35lD1GzaJ2DLeY2z5LIQDZaYLRctiBxbaoCiIk5hQMPtxbnM0/ispESpGSYilXZ5+hcMtrXoE0Zvhh0TZVr68IjhUpOXf"
    L"J2QA1mUFQkr3UF4BhKBSrYIQSClK65dSynDtT2WcjhBgglYKAVJV6PQUaxoOqAyvoS2BDuAZtpEfCjnyiICtHQo6gMSTBak7oBfA"
    L"dB3I1JGlmsKY6pJW6j8xXo5aVnCtaRITR4JmnJeeYXlK42gL96uOwRRSeP7eubYhKISyJc5PHwsCDDHfCuvrCz5kSMcNIEBGgn6h"
    L"0FHijEY4D6CUQkrZT9P0lCuCp6wAK1eurOZ5jtGuKyiKq/QzhfSucjktYE+yCO/uUSeFADu0fDsq/OH3TM9SzFlMxw7Dio+vTgwj"
    L"BannGUHwpfUrhZKSKI79PueIxYgVC5+2OZAvsMgSH4TFJByN67SkDA/WQUohQwgJqZBwwlch7AlQEisFQkmkknQygarWABOWt9fj"
    L"4+NRq9X67DXXXDPvl619wWnBCy4n7t+/3wDMzs5+e35+PhdSSmu0jeI6S71k2LwLwywAYFloCNDanvQZGCHXCdIXwmL7lvwYZIfB"
    L"hMnUcuTnLCgJkSjK9O85Jy0lURSVgo+iqHzEUUwkBYl0vLzEC15505c+T5fORQdXHYSLkggFQllKnyAESgrfGCSccgWDDzl+cPvC"
    L"N4dKSRQrloqIKG2AsSUGSJKEJEn6p2NBiResADfccIMBuPrqq7+1uLg4kFIobQripEq3H3kCBfDgcJkVh31lg4Xb95zH8AtgLHrG"
    L"kh0EvXjS0Y8kFNpCGkFd9InSynKlglLopdWPKIBUiiRNkcLSiDQw0tIlwsKSzo0L6WNyeBb4+I177UGcUxYBygk5KJBQqlQe9zvO"
    L"YwglkEpglUDGgtZAEadVjNVl9pIkCZVK5bSkAqccAv7gD/4gjePYTcXSmrRSoyeqw5hZgrhRRHwSHoDh+/YkVyDA9Cz5FJi2f3tU"
    L"be1ztx2uss9Jl36i8D0IjJME6VNJKUMqJ/zFd9X7gA6HC0aAo4EDWBh+T9rQCh9SHzE8VgkBKQa6QCjnCZR0axhnRA7L+NivtbY+"
    L"fJ2y9cMpeIAwdu3alSulrDGWLM9IkwqZbro4V55gOPURl16OocWXW2VGAPmMZXDYYrosTx+fEzJGTkqCIsxLcPuWof3nEX4URURx"
    L"TJIkKCVRyll0AIpCgJIuRjvtGIYEIRVSSQfegpdQEqkUIhLDz4dQoTzQU9J7Bu8hvPBdDJOYSFKICvVa3XddKwaDAVEUkSTJaWkJ"
    L"O2UF2LlzJ2maCmM0eZZTrVXoDOoUhdNigJISDdIdaQ4JBSIncyd0IcFmkB2DYoYRa+H5BT/y2liIFVREhordOgI/Tvij23EcE0WK"
    L"JEnBamrKt2BJfHyXXriBrJGIyLlylA8RQjnXPrpfKYi8oL2QkaNKMfJe5H9TSmQkMVLSNTWazSZaO/YyyzLq9TpxHL88ZgYBRko5"
    L"KIqikRcFY9UK/bxJYZT3ej5FHDF8V/gZCdzLzBfMImRTLrcXcogF/65DeA8SnEVQgOegfv9wwo+QUhBFMVJob8j+F2SY2OH2Wd+z"
    L"4Lp/hVulDK8kNuTqjsQRPj0s5wpYhm3wQngKXJYEkpXOk8hIkqPo2hpjY026rRNIqawxRuZ5nrVarW+F6//3kNVzxgv2AGExR6A9"
    L"PT39XxqNBrrQRaQEMl1Nv3AgJ+TKpZ37vH7YQ2CXCbeYh2waQptAGWN/gstfti9U1TAY69rN5UmCH3X7QfhuOyFNYkdRCBBCDqlt"
    L"KUAGK/fuXimEdFYvY4WKFCL2Vq98+IgkInJhQkjhw0L4nrd8HxakEshIICNFlEZkUqJVkyRSaONK2FJKVRTF4DOf+czdP+FK/J3H"
    L"KYWAZrMphBC20WgshX460KhkPYtLEVEUSsMBKI2i+5NYPW0pZkDPM0KxBq8hl8f+5xuj+4VgbXVALC1RnCBgGfh73vTPK0KlWqMi"
    L"MqqxKFM9kMtctZQSISPv7iNEpJCRwqohupdKQaRKZRjGd69M0UhW4N+TkXL7lEClknYhSOqrEcJNvQu9l2NjY/bKK6+snIrswjgt"
    L"DSFr165tZFlGoV2xJq1OstiWnLNW4YSnSxxgPTskQjdP5GbAFLNiCPRCa1WJ6sOaHqPc6shYJnzn/FNp3HWO42Vc/6jwTwaDSklU"
    L"nFBLLUnkvZenalEBDA6T+LJqF4if8hgFwzqAIPQN+LYnH/Wsn10cGkoDHpCISBFXYhbagubEOtI0BjuchVWr1epRFJ15EBjIIGBP"
    L"q9UaSCllPhjYsfHVzHfGXLtTcKEjJi2MdsIXBptZ8mmw/eDy8QL1F7KsHvr3Ts5+n0cfBCOBUTi3Oxr/T8YAwQMkcUykFFYEingk"
    L"ZxeepJHOqmXkAZ9SoCIXDlSweAf8lP/c0AOMIP0yBATUL7CRQEQKYoWqJbSyiMb4Koo8QypFlmW22WwCfEsIseRvjHnmQkAggz71"
    L"qU/t6Xa7OQjV63UYn1zDfHvC17sVFrUsC7DGgjCYgaWYBps5rzDEhaFVKhzeSDo3mkeP0gsjw1hBMy6IpStXO9A+FP7zWf+QDIpJ"
    L"GJAmnu1TvjgzwtC5EDAUdknqeBQvyxDghCtL96/KtE/6NLP8G+FzkUDEEpko5gY1JibX0ut2iaKIXq+nJycnAfa85z3vCf7ylMYp"
    L"/wDAH/7hH1YbjcZAa01/MKDRqNHNVzDIIpSIXNOj9RUUbbAYTN+Sn7CY3Hjhn8wPMPQEo57/J82DdHqERVBRFiksSkXPSQNPxgDL"
    L"tqOINII4UiCFJ4PkMsCHirDeckfTPqGUo5+DVXvFIAqfkUMvIYfvC+/23ecUMokolGIxb7DurA3kRU4URXS7XWq1Go1GY9XpkBuc"
    L"ogKMZAKLMzMzf9JoNBgM8qJWUWRqG60liVIJmMgtq6o9g5ZDMWMhMwhh/DIqozB/6P7FyaEAntftP+fYvIsIcftk938yJ1CSQZFC"
    L"igD2HOofBXJyBOSFMEB4lm7bqqgUulMc6ZXHETyMkkORE75TAAcsk2pCVwt64ixq1ZQszxBC2CzLVBRFWafT2eNP85RxwCl7gJAJ"
    L"rFq1al5rbYvCTeRojp/NsZkKkYzADCd/2EKg5wQ2h7BoxHPoXxjW1cWo8EetfwiylqeIrs16LNFEwmUBoY7+4zzBqEdASCIxIE0i"
    L"IMRriZRq6Palj/ve2oV0Ai9dfhB8SPkiWSqJSy2HmKBULimRUkCsSOoJxxYtlfHNpElEkRdhBpOMoij/+Mc/flpSQDg9TGAB8KMf"
    L"/ehPZmZmelKKuNdbsivXnMOBEyuQSmFtDEaBFuiWwg7A3cDJTwrFDNNB8AKXZXfMMrcfFOLktHCEV7JAJXKrkCilXFXuJCLo+cOB"
    L"c+GptMRKOVJGnuS6g1BLFx/51G0ocCJVfieAvLJiqDyb6L8vlQebHkMEBTgwm7By7WaKYoCUjgKWUjI5OTl4y1veEp+q3MI4LRgA"
    L"4LrrruukadorCk273WHt2lVMt1fT60dIGQERup9gM78g0rLe/pG1/8oMIBzeSe4/fMbyXDxQdtJ6sFjOVZDP6wGeyxAqlBeQVBIV"
    L"ObRe3mJOjXiAwAOEGF8qifMMThHECO3rOYBQBxCeaQwhJJIQS1QSUUjF4cUxNp5zHq3FRfzK4nr9+vUA//UDH/jACd8HcOY9QMAB"
    L"27ZtW1pcXPyzWq1Gr9cvmo0KVC/gxIJrsrA6Bq08r+4TtTDbFz/Xrxye/LejwvdgzD6PIoxU6FxEkG5eH66Chs+3R5VglBcI23EU"
    L"OX3M+iSxKotByKgEbHIEtQ+9giwFScn3D4tCwuf3iOU9BEhKpQiAMqmlzHWgKzayesUEHZ8BtFotu3LlShvH8dzpvLHEafEAAQds"
    L"2bJlVmut87wgz3qsXr+NJ4/VSZIYY2LKifZhPT1rKZfuDO3eI2FNiKHgR0u7Ypnljwg/vEYQKxdSQio5qgAnK8NoaABBJC0qVo7z"
    L"HxGw8IpQ7pOjIUGVWEGOKEgAh2UVcKTl2ymUQPgiErGi2kx58rhl1fpLiJRFF5o8z22WZbFSqved73zns/5ET8uS8adFAQIOmJmZ"
    L"+dOlpSWstXFrccGec+5mDs6uJC8cT04ghUJqJ4YzeyhxAAzBny+3lv1eo0zbjzmYkCgE1hHXoAHP7wWW7Q9dPb7aF8BcyP2D0F3F"
    L"LoA3L2CfxwcgKHxRh7LNO4QDf4xy+edEJFFpjI0TnjxeZev5l7DUXiKKY1qtFkmSMDEx0W23263TIbMwTosCCCGstVZef/31M8C3"
    L"oihiqd0xk+M1svR8ji8okor3AFKW/RKuHBTm+fvnkNl4RREjzRVDNujHZAXeYxgrSBRUZI5UUTjGH+sBRqlirQ39fh8V+VjumUCk"
    L"Ki0eMRr3Qx9A6BgSZcuY4w9EGfbKzCb0BoQik1eAuJYy07PMZZvYePZ6Wq0WaZpy4sSJYsuWLSildt98883zpyv+w2kEgffff78S"
    L"QmTNZvNrURSR5bkpsi5nn/caHjtSIa1GjvVaxqFDmOJlvfBDldANxyQOhR08wcgYif3DvgODFIJU+pm/Jkwf88/WlMuu2bL93O2T"
    L"UpBp4zkAz9SVADDEbl/kCsCufFYjBR6GLj/E+5HuIudxlAeCEqKI6liVR541rN20EyUNuU//Op2OHBsbK9rt9h1e8C8vDADDMNDt"
    L"dv98bm5uViDi2bl5u2XLeRxoraenFTKJvQWNpnJmxAMM5/o5mjdkAWrEE/g/GFyre8Fwy6INjFUjqqZFVB2n0WiQVqvDR6VKpeof"
    L"FfdI0yrN5jjzCwtMVAzICKmELweHSqAqQV2pFEISJsRSlohFqCf7upEo8U9oDHXt35QZg0xjtIx45NAYF1+yg8WFeZIkYX5+3iZJ"
    L"oqrV6vzv/M7vnDYCKIzTpgAhG3jrW996dDAY7K5UKnQ63aJRj0kmL+PJ4xG1RuJYMjUaBmC48KNfH3C0X9CzgVZKrwTOC4hlIWC4"
    L"LQBtBfVKzM9tmsJ0pli5YgWNWo1Kmrg5dlqT9Xu020ssLswzc+IEJ6aP8fCjj9F++m7efFGFHOlxy0jsD82hgcApi0PemsXQwwUv"
    L"t0zZywjm28AYov9qo8JTxwry9GLOPmsVraUlkiRhamqqOP/8820cx3/5pS99qXs63T+cpnJwGLt27bIAa9eu/Wqr1bo5SRK5tLjA"
    L"Rdt38ND37uHSDZnLj3OFWxtAIERYdUG4FcKDwEtSSGKFW1TaSuEKTALAb5cVJL/fWpSAdmZ520Ux500e5fHpo7QySeGlYJcJZgg2"
    L"0wi2XF5n/aoKlWSoACIIu4zp0v2GCnHd5/QjyU3Z2mZGlNkfdlhjAOlp4TgibqT84F7DRZdcTZ73yomt7XZbrl27ViwtLd0hhNDW"
    L"2ufvdX+B47QqAN41vfGNb7zzzjvvbBdFMTk3t2DPOfc88YC8iEPzs6ytJfQytzaQFW72jVsCz60VYCXL6urCgya3jEzIIiTPaQIo"
    L"N922FJAXgnNWVVg7AX0tKexykFYieeGEHLj9WlVRq0a+6hdceQBuAeANM4chDnHlSbcMnF3OaZXJS1Bgp0xSKZJ6hZmW5dDCRn7m"
    L"wm3Mzc1QrVY4fPiwXr9+vbLW7t2+ffvtvvx7Wu8/fNpCAJTZQATkjUbjE3EcM8iyIht02XLRFfzgmQZpowIqcsWWsrEiXK6QFvoZ"
    L"s95aQswEhQigMBiWGApg9F+sIBKSSFpqsWS86h4T9YhVzZg14wmrJ2Im6xET9YgVjZjJZsSq8YixekwSu3y+RPOjz5KS2l3eKCqG"
    L"yUgQvB3ZFwpcwVtJl01Uxip87zHN2VuuoZpKisJ1AB87dsycf/75KKXu8II/rfLixfhB3A0ONfCH3W53wRgTTU/P2AvPP4+j/a3M"
    L"9FPSeuIaRdSIdZULPri2MremIMOrWfbVB3cshs/AyFUm9OkjcFO9lSCNJOvHFauqGnSPfNDBDHqsqBnOmkyoVhTVVJEmCuU7c4Pb"
    L"L9vCSiHL4XOJ7N0xiNLCR+N+ALXhdHzRSCmiako7Ezx6ZA2vvfxyFhcXqVQqzM7O2iRJ4jRNF3/wgx/8d78a22m/+/jpDgElGLzm"
    L"mmumbr/99k9EUfS/9PqD3JosPuf81/G9p57kPZcMGLQHCK39cnDuFizuhs1eESSU+hnarUSI3wpEgfMEkrLFrLypQ9k24jiBWJGq"
    L"gs//bZ8fts/G1taQi5REGkRvmksnT3DD1WMUKmVgrGtnlyNW6l+7tA6HRUIWItzfsyN3klre1uA6og243whKLSU2ktTGqtz1sGbi"
    L"rKtYtbLJ8eNTVKtVDh06pC+66CJhrf3EjTfeePBU5wD+uHHaFQBg165dGmAwGPxRnue/2Wg0xqanTtjXXHqR+OLjW5gdPEK1npIV"
    L"BaIwWGFKEBXuvFWunhGKPkKUZIyw2tG0IVsIS7gDZSEIgbGCahox3+3xuafW8O3e5aikhm51EQKq1RqV5oUc7i7x8Ncf5Jdfu8TW"
    L"cybpF34SSJjXENI2L2z8xM/lCD8gQH8Q3upDS/xICuCVSRGlCX2ruPfJMd72vqvpdDokScLi4qLt9/tqcnKy2LNnzyf9ZT3t1g8v"
    L"TggovcAHP/jBY1LKTwghRKfb1YKczRe+nu/sa1Abq2GjyOXVofkyLBw1wgwKq4egadQFh4kYFn+7loAAnCIZoJJIjs33+Nff2cg3"
    L"269l6vCzPPG33+WZfY9y5ODTHD7wJMeeeQShM443d3HrN1fy+KEWlUpU3vevBHyIEgcswx5lrC8xIOV9hPwwAbdi3Wn4CmJtos7f"
    L"PJYztu51nL1+Fb1enzRNeeaZZ8yWLVuo1+s/WFxcnD4dvX8/brwoCgBDL2Ct/aNWq7VojFFTx6ft5Tu2s7+1lRODGmm9gvX181Aw"
    L"KWNmubRmCKmhKDSSkqHAKi+H5d1DQgiEyfnkI2s4HJ3PYw98m2PHj4OMiJOUJElJkgoySphfWKRYOsr4ua/j/7wrYb7Vc4WfEdbu"
    L"5FJEGSJGahd2mTLYIaHlLkT5OZQgqiZ0jeA7P5rkjW94E/1+jyiKaLfbzM/Ps3XrVnH8+PHf++3f/u0ey9Tp9I4XTQGCF3jf+953"
    L"VEr5SSGE6PZ62ugB51/6BvY+Xqc22YAkLqdDBaDnWsD9WoH+CjolCMybRFjXcu625YjrdTNy6onkuwfh6ehSju77IVnhZtWeXAOQ"
    L"UpKmKbkRRKZLPr6Dux7pEUe4mT3LEH7wVCNQf3QdP+yQv7Bh4ezwlZGO4CiiNlHj7gc1qze+kQ1nraTT7VGpVHjqqaf0BRdcIIHv"
    L"7ty5887du3er0536jY4XTQFg6AWAP2y1WotFUahjx47byy45n8P9iziwWKPWrDp2MOTmwonQDV8nCESPHLp+fKeuEKHKKEs7ccu2"
    L"5Nw/v5a5Ts7szDRJ4uYJPl9PoBCCOI7J85yJiQkeOjHBfKsPI/TzsBztLlnQg9F+tPBvsH7hbyYRvu+UQJHWqsx3Bfc+tZo3vOH1"
    L"tDsdYl/1m52dZevWrWJ+fv73hBDFDTfc8GKK6MVVgFEvkKbpJ4UQot8f6G5niSuuvo67Hh0nbjaRaYKIo5HGSLdKhgvtvmVcDidg"
    L"OFpWDTn4UCTyNHESCWaXNEfMOhZmjiN9Z/DoXICT5wUo369XTSOOtBscmumPALwABhlxxiMNrIKhu7cM1zYardsIIJLYKCKdqPL1"
    L"7xece8F1rF7ZpNfrE8cxjz32WHHhhRdKrfV3d+zYcefnPve5F9X64UVWABh6gfHx8T/s9/uLg0GmpqZP2PO3nE1Wv4KHjjRorKhj"
    L"4wQRxYg4hjh2EyqkXy7F4idRiuVWHypzoTiDwiJQUrE0MJwomnSXWuXcwJOFHiaEhGlhKoqIY0VfJ3QGUBg7ZO6WIX5x0pI1ABYR"
    L"FqscWbW0xBC+pFwbq/HMUc2+E1u59o3X0FrqkKYp09PTLC4uigsvvFDMzc29JNYPL4ECBHbwDW94w9EVK1Z8QkohBoOBnp6a4tpd"
    L"13L3UxvoR5NEtQrEsZ9rF/uGSefywzJswkI5YVO6Dp0hGzesGBoLlURRt0sklRpSDDt/RieFjE4MdcvDRERRTC2Bwphy/WYx8u8y"
    L"yy/nOQbr90ymCWHIf074CSNJgqqnfOVvYq685u1UK6q82/qjjz6qL730UtXv97+2c+fOO40xp532fb7xoiuAHxpgw4YN/zGO44VO"
    L"pyPnFhbteKPChm0/w52PNRhbPY5InBeQUYSME8fPn9xQUXL5kccOwxtMhywh17CqEbGaaRrjK1BSPEfwo48oikjihDRNMCjG0x71"
    L"iqRYZuGjGGBI+gy7mcN9jt1rGzIFJdxiDyqmsarO9x7J6Uav5fLLLmSp3aVarXLgwAErhBAbNmxY+OQnP/lha0/qjXsRx0uiAMEL"
    L"nHfeecc3bNjwicnJSdnv9YrDh4/w+qsvZ39rG88uTlKbaEKSYuMEVAwqcW3XoZU6uHqhhs2aInbPI4s3WSRpUmFrbRYpJc1mEynE"
    L"MmtPkqRUgDRNiZOYWq1Gr99npZpiolFd7uMFJ22zbFk7q22ZtgbPIMIxRxFpvcJiX3Hngyt4y1vehi5ypJTkec6jjz6qd+3aJQeD"
    L"wac/9rGPzQDqxcr7Tx4vlQcA0NZauWrVqn8P/E1RFKrX7+v2Uos3XHs9X/nbcURzFapWRcQpIk6dF4hcWCBazsohpVuvp5ykEZWp"
    L"mZSCTq54x/mCDRxibM0mP01sdDWQ5dPCk1gRV8foTT3CjrO12xc5r2N9zC9H+cKzfcbT137FUhvSRxkme8ZUJmp88VsFWy95O5s2"
    L"rqHXH1CtVnnkkUf0xo0bJfA3d91117+/7bbbJC8S6/d84yVTAK/RYtWqVa2zzjrr346Pj8tWq8X0iRNs3LCK8Q1Xs+dHNZqrVyDS"
    L"qgsHSYpMKk4R4hhiZ+llNuDn60nhFWCkacMAY7UK/9MVc6yU80ysPYdqJXHd2UJ4XCCII0WtVqE+tpqZQz9i5+RTbFjdpJIqt77B"
    L"yMwT64v9FspFLoZrHQwbm0O6J5QLU40VdX64T3O0eznXXXs1nU6XSqXC9PQ0R44c4fLLL5fHjh37d7/5m785f8MNN/BSWT+8tB6A"
    L"0NCwbdu2uyYnJ79qrZWDQaYPHjzMm3e9noeOb+KZhUnqK8YhrSD8Q6YVVJQi4hih4pGp2JEPEZHfH7l9PlQMCsFF6+v8q8unWKkP"
    L"I9JxGpPrmFixiokVK1mxcg318dVkOuLovu9xgfwe289pMF6PqVciVw84iYSzPiNx8d6nfNorh78xZNkHGCnSeoV2FvGl747z9ne8"
    L"x60XKBwgfeCBB/Rll10m+/3+16+55pq/frFJn+cbL0ox6P9nGABr7S+fddZZRw4ePJjGcWIWFxfkm3/2bXzpjj/nn759FXHDort9"
    L"IHfTy1GoTGIoQBTYovCpnydqtKODrfC3l9MWpaCwgkvPrnNLc5Gv7XuY/f2VLHTHmNUR47WIFdWMFXKGnWt6bFjTZOVYwkQjdu6/"
    L"PGRfqBJmuO3j/Cjz55hBfJdvhIhj0vEaf/Yly8WvfS+bz1tPq9WmXq/z4IMPmlqtJjZt2tT//d///Q954PeSjxeNY/5Jw7c1mX37"
    L"9n3w2LFjt83NzRUrV66Mtmw+l+/d+xD59N38D9dq2lMnEP0MkxfYXEOeYbIMWxSQ5/5mURpb+PsI6NxPMytcl7EFJBRGoIWksIZj"
    L"rYwj8wXt3CKkZKKRMDlRp1pLadZjxhoJ1WrkegIix0OE5d2GiYAHftaM3DvADgtGnvAZWzfO3gc0Dxx7A7/2q/+Ifn9AHMfMzMyw"
    L"Z88e/fa3v11NTU390ute97q/AF6StO/kcUYUAMBXuMwTTzzx1f37979DCKHHmk21Zetmdv/lX7Fzw0Gu2dalPb2AyDNMZrB5jsgz"
    L"dJ5jiwxyJ3Br/D0DvQII6/fh1vx3K7dLcgQ5Ao0itwLj+w2jRFKtRqSpIkpjxwqGKVteAWQoN5Q1fx/0PU3tCpae8Iki6pN1Di3G"
    L"/OmdG/n1f/wvqddTisKt9vmlL31JX3zxxXLdunV3bN68+Z3WWiFEuFncSzteUgxw0rDWWnX06NEPTE5Ofj/LMtUfDPSRI0f5uXe+"
    L"jbsfq3NocZza5Di2UkOmKTKtQrWOrFaQSRWR1hBJiohcxiCjFJIEogQRuf2oGKIEGUekSUQ9jWhWFOM1xWTdtYCtaMbUKzFJHKHE"
    L"MKw45XEKFEghawFt/ZoGEDgIOzI/MK5X6ImU//aNKu989y+xauUYWhvq9Tr33nuvGRsbE5s2ber/h//wHz5kreWWW245QyI4gx4A"
    L"XCgQQuhDhw69+eDBg18+fPhwvG7dWdG6tavFUmfAN79+G//8fVVUf5a8O0AUBaYosFmBzQfYzHkBW2RQuDuIu7uN+NvOlBU5O9LA"
    L"6ev75bRt6YBjqEiWEz3ksOwbZvMCWHe/YnfhTJkmEklkFCGSmPqqJv/p84YNF/8K73rHtSwtdajVajz55JN897vfLd797ndHR48e"
    L"/cUrr7zyc5wh1x/GmfQACCH0fffdF2/cuPGbK1as+I+bN2+O5+dni6kTM6xZPcFFO67lv/51j3hiDapahbTqUsS0hkwbyLSOqNSQ"
    L"ac1nDDVkXEUk/hFXEFEFEScQpaA8wRTF7ln6ZzVMI5ERVkaOWpYRVnqK2Qb+301XG7amOctXSiHimPqqBl/Ym6Mmrudd77iWbrdP"
    L"pVJhcXGRe+65R7/5zW+OiqL4+lVXXfUX7hKcOeHDGVYAcDOK9uzZE61evfrfNxqNrxtjo163qw8ePMQVr91OZcUl3LanR2X1GkRS"
    L"RVZqyErVCb5aR1ZqiEoDKo2RsFBDJHVkGhShiohciHChwSmCiGKsinzzR4SV8Qif4FcLQ7m+Q4RTAl95FPj5AUohVYRRMfVVNe75"
    L"oeGxmddy0y/fQJ4V5a1qvvrVrxaXXnqpqlQq3//Yxz72QQ+EX7J8/8eNM64AQgi7d+9es2rVqtbTTz/9S1u2bBm0Wi2V54V55pkD"
    L"vPX6aznWPYu/vjenunYVxCkyrSCqVag2EJWmV4oaVBqISt0pQVpFJDVkUkPGNWRShdgJ32GGFKESpExAOmXAK4IQMUYoXJYsXRpq"
    L"/aIQoRfBK5CMY2wU0VxZ50cHIr7xw3P51V/5Fb+6pyFNU+666y7baDTURRddZKampn7305/+dDec+5m9+mcYA4yOkBoePXr0vUeO"
    L"HPnLAwcO2LVr16pqpSLWnbWOP//vt/GGSyw/c5mkN7eE1Mbd3jV3KaHJM4cFdOFuI18UWF24jCDMOcTf77Ps0fMdxaGsHNrUBeXN"
    L"vQl1fS8rKV3fopUglMAoSXOywsG5Cp/6SpMP/9r/yObzNtLr9anX63zve9+zTzzxRHH99dfnBw8e/JU3vvGNt+3Zsye67rrrTnuH"
    L"7wsZLxsFgCEovO+++27o9/u7p6am8jVr1sb1eo0VK1bw5/99N++6OuGyCwS9+TbSWGxQgqLAFhmmyEsFQBflXANrzbJOo/Jef6M9"
    L"fuU6Au4fWzZ0uGZOGTo/vfC1MjQmUma6Ff7TF1Led+M/49JLttHp9KjX6zzyyCN8+9vfzm666abk+PHj/8f27dv/9SOPPJJccskl"
    L"2Zm4vs83zgQT+GNHAIVXXHHFbfv27fsq8M75+flcCGIpJT//8+/jc3/xlyCbXHbBJL35NkJakAUiMogihiJ3wo8KhC7KhaiEnwLu"
    L"5K69J3DsXfDEpdUDCIv0n7FeN6w0IC1SWowUNMdrnOjF/NHnBW97z69yycVbaLXaNBoNnnnmGe655x79rne9K2m1Wn999OjR/z3M"
    L"mjozV/f5x8vKAwBYWzpo9eSTT35pamrq7b1eL6/VavH4+BhxnPL5z3+B668e46rXJPTnW36JIY0otHf9GqsLf6tZ4xShVADrPUG4"
    L"UXUIwwbhvYIVo50gppyUZIRBKYsR0JhIOLqY8okvGn72nR/i6isvo7XkaN65uTm+/OUv6yuuuEKsWbPmzt/6rd96/+233971hM8Z"
    L"j/uj42XlAaDsHTCAefrpp9+3bt26vwLeOhgM8sXFVjw+PsYv/uKN/MXnPk+3r9l19QqyxSWsjkBpVxgyBnSBLtwt5Sk0MiwEUbab"
    L"G7/PDjt4y5sauJlKjj8Ibt+ipLs3cWNllf1HBf/5q5L3vP9DXLp9G3PzC9RqNebm5vjKV75iLr74YnX22Wf3b7311ptuv/327pko"
    L"9PxdxsvOA4RhrbvJ6p/+6Z+mF1988Zf6/f5bjTF5FEVxs9lkcnKC3bd9ka3rC9731rPQ3RZ5plHGYnW4/axbgs7o5YtPuLAQOjjd"
    L"/lI5RLiLd8AJBikKkGClpbaqxv0/yti9N+XGX/gQm887m4WFJer1Gt1ulzvuuMNs3ryZHTt2cPTo0Rsvu+yyL+zdu1e+XEDfyeNl"
    L"qwCwXAnOP//8L2VZ9lYgj6IortWqrFu3ji/ffgeqmOaX33culajHoDNAGpywtfE3lNZYPVyX0Jqh0N0aRbb0Cha3xjBojLBIYbBo"
    L"olSSjFW5/VttfvDUKm666SYmJhrMzy/SaDTo9Xrcdddd5rzzzmPHjh0cO3bshh07dvy/u3fvVjfeeOPLzvLDeFkrACxXgnPPPfev"
    L"gOuFEHkURbFSik0bN/L9ex/gqSd+yI3v3MQ5m2KKVhujLcJX62wQujHLbksrTOFuOE1YnCqsTmIQaKzQWAG1ZkwnV/y3r8zSiy/l"
    L"F37+fRQ6Z2mpQ71eo91u8+1vf7u0/CB8D2hfVqDv5PGyVwAYKsHHP/7xylVXXfVFrfVbrbWDJElSYwznnnsOhw8f45t33cXOS5q8"
    L"9fVrEPTIugPfmm996diWdxl34UC7VUm8J7DWYN1sDowtSCuSqF7hoSfafGFvl0tfez27rr2a2dk5siyj0Whw4sQJ7r333uKCCy6Q"
    L"l156KYcPH75x586dn38lCB9eIQoAQyXYu3dv2mg0vmitfVur1TK1Wk3keS7WrFlNnCR885vfpr90nHddt4HN5yaQ9cj7BVZ7QYcm"
    L"DhtWKjUlDjDWlZDjSBA3EuYXNF++e5ajrTW8813vYv26lRyfOoEQgnq9zrPPPsvDDz+sX//616tNmzbZQ4cO/fzOnTtfEZYfxitG"
    L"AWDYQ/Dd7363Gsfxvzl+/Pj/XK1WVaPRKIqiiOI4Zv36dTx74DD3/eB+Vo1lXHvVajZuSMBk2H5Gkbs1CaxfpdRalykoYYkTIIlZ"
    L"WCy454FZHtofsf2y13P1VTvotJdYWGxRrVZJkoRHHnmEw4cPF295y1uiZrP5rW63+7Ft27bd8WLN43+xxitKAcDxBLfccou49dZb"
    L"zd133/1BIcRtgFBKaaWUyvOcyclJmo0Gjz2+j8d/9BgTjYxLtjbZsrHKZEOCcmkiISvTll6v4NCxHg891ebAdMTZ52zn6tftJI4F"
    L"U1MnAGg2m3S7Xe677z6rlDLXX3+9yrLsr9/4xje+9/Dhw72XE8X7dx2vOAUApwSPPvpofMkll2T79u17+9TU1L/WWl9bFIUZGxsT"
    L"RVEIYwyrVq1EqYhDh46y/5lnyQdtGpWcZs3QqCqEMPT6Be0uLLQlRE02bDyXC7ZtplpNmJmZpd93nH6apjz77LM88cQTesuWLery"
    L"yy9Ha/21f/JP/snPf/nLX+7hevlfUcKHV6gChBEs7lOf+lTtmmuu+fzi4uLbl5aWaDabRa1WU1mWCWMMY2NNqtUqea5Zanfp9fqo"
    L"yC25r4uCJIlpNmvUqhV0kbPYWiLPMqq1GpVKhYWFBR599FGrtebKK68U69atW5ibm/uN7du37/7oRz8qAW699dYz0tJ1quMVrQDg"
    L"lGDv3r3m1ltvNfv27bthaWnp04uLixPtdpuJiQldr9dlnueiKAqEECRJUt4WxvqJHYW/H5+bpxdRq9XCEu3s37/fzs7O6q1bt0av"
    L"ec1rGAwGX//mN7/56zfffPNRT+3Cy6Cu/0LHK14BwIWEIIh77rln/fj4+G8tLi7+TqfTEZ1Oh4mJiWJsbEylaSqKwq2/a/3ULSkl"
    L"cRyXawb0ej1mZmbs4cOHTafTsZs2bYp27NhBpVJZmJmZ+Y3t27fv9n/zZUnt/n3HT4UChBGyBIDp6el3FEXxr44ePXrVwsJCY3Z2"
    L"ljzPbbVaLarVqkzTVPrbsZPnOd1ul9nZWT0YDGg0GtF5553Hli1b0FovZFn2ybvvvvuPbr755qOA8MrzirX60fFTpQDgvMH9998f"
    L"+TxcPv3002cXRfGbwM1zc3MrjTFMTU3RarXo9/tY65aOWbFiBZs2bWJsbIxKpdIpiuL7aZp+8+677/7sL/7iLx7yvy3PVPv2izV+"
    L"6hQgDM/Bl/cdf/rpp8ellD+7adOm1xw+fPgNjUbjdZ1OxwBCSimstYN+v/9ZIcTiww8//Gfvf//7D+Enafo6vv5psfrR8VOrAGGE"
    L"/oKT4rXas2fP5EMPPSSyLBMrVqywzWZT33jjjXMjnxG+Te2nUvD/4Ia1Vlhr5X333fdjb7lmrY3846feMML4B3OiJw1RTuoc3fmq"
    L"pb86Xh2vjlfHq+PV8er4BzL+P4Sf1Gp+cG9mAAAAAElFTkSuQmCC";

struct ShieldArtSet {
    const WCHAR* small;
    const WCHAR* medium;
    const WCHAR* large;
};

static const ShieldArtSet kShieldArtSets[3] = {
    {kShield16B64, kShield48B64, kShield128B64},
    {kShield16B64, kShieldQ48B64, kShieldQ128B64},
    {kShield16B64, kShieldY48B64, kShieldY128B64},
};

static bool Base64Decode(const WCHAR* in, std::vector<BYTE>& out) {
    auto value = [](WCHAR c) -> int {
        if (c >= L'A' && c <= L'Z') return c - L'A';
        if (c >= L'a' && c <= L'z') return c - L'a' + 26;
        if (c >= L'0' && c <= L'9') return c - L'0' + 52;
        if (c == L'+') return 62;
        if (c == L'/') return 63;
        return -1;
    };
    out.clear();
    int buf = 0, bits = 0;
    for (const WCHAR* p = in; p && *p; p++) {
        if (*p == L'=') break;
        int v = value(*p);
        if (v < 0) continue;                       // whitespace and line breaks are ignored
        buf = (buf << 6) | v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((BYTE)((buf >> bits) & 0xFF));
        }
    }
    return out.size() > 8;
}

static HICON IconFromPngBytes(const std::vector<BYTE>& png) {
    if (png.size() < 26) return nullptr;

    // A PNG image is a legal icon image resource since Windows Vista, so the PNG is handed to
    // CreateIconFromResourceEx exactly as it is. The buffer has to be DWORD aligned: the
    // decoded vector is, but the check is cheap insurance.
    std::vector<BYTE> aligned;
    const BYTE* bits = png.data();
    if (((ULONG_PTR)bits) & 3) {
        aligned = png;
        bits = aligned.data();
    }
    HICON icon = (HICON)CreateIconFromResourceEx((PBYTE)bits, (DWORD)png.size(), TRUE, 0x00030000,
                                                 0, 0, LR_DEFAULTCOLOR);
    if (icon) return icon;

    // Second chance: the same PNG inside an icon directory, for the case where a future build
    // insists on the file container format. Never reached on Windows 10/11.
    DWORD w = ((DWORD)png[16] << 24) | ((DWORD)png[17] << 16) | ((DWORD)png[18] << 8) | png[19];
    DWORD h = ((DWORD)png[20] << 24) | ((DWORD)png[21] << 16) | ((DWORD)png[22] << 8) | png[23];
    if (!w || !h || w > 256 || h > 256) return nullptr;

    BYTE dir[22] = {};                             // ICONDIR + one ICONDIRENTRY
    dir[2] = 1;                                    // type: icon
    dir[4] = 1;                                    // one image
    dir[6] = (BYTE)w;                              // width  (0 means 256)
    dir[7] = (BYTE)h;                              // height
    dir[10] = 1;                                   // colour planes
    dir[12] = 32;                                  // bits per pixel
    DWORD len = (DWORD)png.size();
    memcpy(dir + 14, &len, 4);                     // bytes of the image
    DWORD offset = 22;
    memcpy(dir + 18, &offset, 4);                  // where the image starts

    std::vector<BYTE> blob(dir, dir + 22);
    blob.insert(blob.end(), png.begin(), png.end());
    return (HICON)CreateIconFromResourceEx(blob.data(), (DWORD)blob.size(), TRUE, 0x00030000,
                                           (int)w, (int)h, LR_DEFAULTCOLOR);
}

static HICON LoadB64Icon(const WCHAR* b64) {
    std::vector<BYTE> png;
    if (!Base64Decode(b64, png)) {
        Log(L"shield icon: the embedded base64 data could not be decoded");
        return nullptr;
    }
    HICON h = IconFromPngBytes(png);
    if (!h)
        Log(L"shield icon: CreateIconFromResourceEx failed on a %u byte PNG (error %u)",
            (unsigned)png.size(), (unsigned)GetLastError());
    return h;
}

// The three pixel sizes the Windows 7 shield is embedded at (16, 48, 128). The icon is always
// asked for at one of those and drawn 1:1: only a very large display scale pushes it past 48 px.
static int ShieldArtSize(int want) { return (want <= 22) ? 16 : (want <= 70 ? 48 : 128); }

static HICON LoadShieldIcon(int size, int variant) {
    // The shield of the state this request is in, at the native size that suits what is about to be
    // drawn; the stock icon of Windows 10/11 is a different shape.
    if (variant < 0 || variant > 2) variant = 0;
    const ShieldArtSet& set = kShieldArtSets[variant];
    const WCHAR* art = (size <= 20) ? set.small : (size <= 64 ? set.medium : set.large);
    HICON embedded = LoadB64Icon(art);
    if (embedded) return embedded;

    SHSTOCKICONINFO sii = {};
    sii.cbSize = sizeof(sii);
    if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD,
            SHGSI_ICON | (size > 16 ? SHGSI_LARGEICON : SHGSI_SMALLICON), &sii)))
        return sii.hIcon;

    // Fallback: a simple shield drawn by hand.
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    HBITMAP bmp = CreateCompatibleBitmap(screen, size, size);
    HBITMAP oldBmp = (HBITMAP)SelectObject(mem, bmp);
    RECT all = {0, 0, size, size};
    HBRUSH bg = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(mem, &all, bg);
    DeleteObject(bg);
    POINT pts[5] = {
        {size / 2, 1}, {size - 2, size / 4}, {size - 3, size * 3 / 5},
        {size / 2, size - 2}, {3, size * 3 / 5}
    };
    HBRUSH shield = CreateSolidBrush(RGB(250, 250, 250));
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(90, 120, 160));
    HGDIOBJ ob = SelectObject(mem, shield);
    HGDIOBJ op = SelectObject(mem, pen);
    Polygon(mem, pts, 5);
    SelectObject(mem, ob);
    SelectObject(mem, op);
    DeleteObject(shield);
    DeleteObject(pen);
    SelectObject(mem, oldBmp);
    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmColor = bmp;
    ii.hbmMask = CreateBitmap(size, size, 1, 1, nullptr);
    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(bmp);
    DeleteObject(ii.hbmMask);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    return icon;
}

// The shell's own icon for one file, at the size the shell publishes it: 48 px through
// SHIL_EXTRALARGE, not a 32 px icon stretched to fit. The size of what came back is reported so
// the drawing code can put it on screen 1:1.
static HICON ShellIconForPath(const std::wstring& path, int want, int* artSize) {
    SHFILEINFOW sfi = {};
    if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX)) {
        IImageList* list = nullptr;
        // 16 px for a tiny icon, 32 px for the one the dialog shows beside the program name,
        // 48 px when the screen asks for a little more, 256 px beyond that: the art is then drawn
        // 1:1 at that size, never stretched.
        int shil = (want <= 20) ? SHIL_SMALL
                                : (want <= 40 ? SHIL_LARGE
                                              : (want <= 72 ? SHIL_EXTRALARGE : SHIL_JUMBO));
        if (SUCCEEDED(SHGetImageList(shil, IID_IImageList, (void**)&list)) && list) {
            HICON icon = nullptr;
            HRESULT hr = (sfi.iIcon >= 0) ? list->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &icon)
                                          : E_FAIL;
            list->Release();
            if (SUCCEEDED(hr) && icon) {
                if (artSize)
                    *artSize = (shil == SHIL_SMALL) ? 16
                                 : (shil == SHIL_LARGE ? 32
                                                       : (shil == SHIL_EXTRALARGE ? 48 : 256));
                return icon;
            }
        }
    }
    // Fallback: the classic call, which gives the small or the large icon.
    SHFILEINFOW sfi2 = {};
    if (SHGetFileInfoW(path.c_str(), 0, &sfi2, sizeof(sfi2),
                       SHGFI_ICON | (want > 16 ? SHGFI_LARGEICON : SHGFI_SMALLICON))) {
        if (artSize) *artSize = (want > 16) ? 32 : 16;
        return sfi2.hIcon;
    }
    return nullptr;
}

static HICON LoadAppIcon(const std::vector<std::wstring>& paths, int size, int* artSize) {
    for (const auto& p : paths) {
        size_t dot = p.find_last_of(L'.');
        if (dot == std::wstring::npos) continue;
        std::wstring ext = ToLower(p.substr(dot));
        if (ext != L".exe" && ext != L".msc" && ext != L".cpl" && ext != L".dll") continue;
        HICON icon = ShellIconForPath(p, size, artSize);
        if (icon) return icon;
    }
    return nullptr;
}

static HICON LoadGenericAppIcon(int size) {
    SHSTOCKICONINFO sii = {};
    sii.cbSize = sizeof(sii);
    if (SUCCEEDED(SHGetStockIconInfo(SIID_APPLICATION,
            SHGSI_ICON | (size > 16 ? SHGSI_LARGEICON : SHGSI_SMALLICON), &sii)))
        return sii.hIcon;
    return nullptr;
}

// The quality the system itself is set to, read through the documented SPI_* values: ClearType
// where ClearType is on, plain antialiasing where that is the choice, and no smoothing at all
// where the user turned smoothing off. Asking for it explicitly - instead of leaving the font at
// whatever the default happens to be - is what gives the bold publisher and product names their
// clean, properly hinted edges.
static BYTE FontQualityFromSystem() {
    BOOL smooth = TRUE;
    if (!SystemParametersInfoW(SPI_GETFONTSMOOTHING, 0, &smooth, 0) || !smooth)
        return NONANTIALIASED_QUALITY;
    UINT type = FE_FONTSMOOTHINGSTANDARD;
    SystemParametersInfoW(SPI_GETFONTSMOOTHINGTYPE, 0, &type, 0);
    return (type == FE_FONTSMOOTHINGCLEARTYPE) ? CLEARTYPE_QUALITY : ANTIALIASED_QUALITY;
}

static void RebuildFonts() {
    NONCLIENTMETRICSW ncm = {};
    ncm.cbSize = sizeof(ncm);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

    const BYTE quality = FontQualityFromSystem();
    Dbg(L"fonts: %s smoothing, TrueType precision, 9/9/12 pt at %d dpi",
        quality == CLEARTYPE_QUALITY
            ? L"ClearType"
            : (quality == ANTIALIASED_QUALITY ? L"standard antialiased" : L"no"),
        g_ui.dpi);

    // Three faces, all of them from the system's own message font: the regular one (the font the
    // dialog hands to its own controls with WM_SETFONT, so the buttons are never bold), the bold
    // one that draws the values of the information rows, and the semibold one the reference uses
    // for the question of the banner - its stroke measures between regular and bold.
    struct FontWanted { HFONT* slot; int pt; int weight; };
    const FontWanted wanted[] = {
        {&g_ui.fontNormal, 9, FW_NORMAL},
        {&g_ui.fontBold, 9, FW_BOLD},
        {&g_ui.fontBanner, 12, FW_SEMIBOLD},
    };
    for (const FontWanted& f : wanted) {
        LOGFONTW lf = ncm.lfMessageFont;          // the face the system uses for dialog text
        lf.lfHeight = -MulDiv(f.pt, g_ui.dpi, 72);
        lf.lfWidth = 0;
        lf.lfWeight = f.weight;
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfOutPrecision = OUT_TT_PRECIS;        // a TrueType face, never a substituted bitmap
        lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        lf.lfQuality = quality;
        lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
        if (*f.slot) DeleteObject(*f.slot);
        *f.slot = CreateFontIndirectW(&lf);
    }
}

static void DrawWin7Button(HDC dc, RECT r, const WCHAR* label, BOOL focused, BOOL pressed,
                           BOOL disabled, BOOL hover, HFONT font) {
    // Only the owner-drawn way (style = 1) reaches this function, and that way is the Windows 7
    // Aero one: the shaded, rounded button of the original dialog.
    BOOL aero = (g_settings.style == 1);
    COLORREF top, bottom, border;
    if (disabled) {
        top = RGB(244, 244, 244); bottom = RGB(232, 232, 232); border = RGB(190, 190, 190);
    } else if (pressed) {
        top = RGB(187, 208, 237); bottom = RGB(157, 186, 226); border = RGB(90, 112, 145);
    } else if (hover) {
        top = RGB(250, 252, 255); bottom = RGB(216, 230, 247); border = RGB(112, 146, 190);
    } else {
        top = RGB(252, 252, 253); bottom = RGB(226, 233, 245); border = RGB(138, 158, 184);
    }

    if (aero) {
        FillGradientV(dc, r, top, bottom);
    } else {
        HBRUSH b = CreateSolidBrush(top);
        FillRect(dc, &r, b);
        DeleteObject(b);
    }

    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    RoundRect(dc, r.left, r.top, r.right, r.bottom, ScaleFor(6, g_ui.dpi), ScaleFor(6, g_ui.dpi));
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);

    if (focused && !disabled) {
        HPEN fpen = CreatePen(PS_SOLID, 1, RGB(70, 110, 165));
        oldPen = SelectObject(dc, fpen);
        oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
        RoundRect(dc, r.left + 1, r.top + 1, r.right - 1, r.bottom - 1, ScaleFor(6, g_ui.dpi),
                  ScaleFor(6, g_ui.dpi));
        SelectObject(dc, oldBrush);
        SelectObject(dc, oldPen);
        DeleteObject(fpen);
    }

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, disabled ? RGB(150, 150, 150) : RGB(20, 20, 20));
    HGDIOBJ oldFont = SelectObject(dc, font);
    RECT tr = r;
    DrawTextW(dc, label, -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(dc, oldFont);
}

// Where the right link starts. The row is split according to the width of the two strings as the
// dialog's own font measures them, not 50/50: with a long "change when..." string half of the
// window is not enough and the link used to be cut. Painting and hit testing share this, so a
// click always lands on the link it looks like it lands on.

#if W7UAC_BOTTOM_LINKS
static void DrawLink(HDC dc, const RECT& r, const WCHAR* text, BOOL hover, BOOL rightAlign) {
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(0, 102, 204));
    HGDIOBJ oldFont = SelectObject(dc, g_ui.fontNormal);
    RECT tr = r;
    DrawTextW(dc, text, -1, &tr,
              (rightAlign ? DT_RIGHT : DT_LEFT) | DT_VCENTER | DT_SINGLELINE |
              DT_END_ELLIPSIS | DT_NOPREFIX);
    if (hover) {
        SIZE sz = {};
        GetTextExtentPoint32W(dc, text, (int)wcslen(text), &sz);
        int x = rightAlign ? r.right - sz.cx : r.left;
        int y = r.bottom - ScaleFor(2, g_ui.dpi);
        MoveToEx(dc, x, y, nullptr);
        LineTo(dc, x + sz.cx, y);
    }
    SelectObject(dc, oldFont);
}

// Where the link sits: the right end of its row, exactly where the Windows 7 prompt puts it. The
// rect is as wide as the text, so a click on the empty part of the row does nothing.
static RECT ChangeLinkRect(HDC dc, const Layout& L) {
    int linkPad = ScaleFor(PAD, g_ui.dpi);
    int right = L.W - linkPad;
    int width = ScaleFor(180, g_ui.dpi);
    if (dc && g_loc) {
        SIZE sz = {};
        if (GetTextExtentPoint32W(dc, g_loc->changeNotifications, -1, &sz) && sz.cx > 0)
            width = sz.cx + ScaleFor(4, g_ui.dpi);
    }
    RECT r = {right - width, L.linkY, right, L.linkY + ScaleFor(20, g_ui.dpi)};
    if (r.left < linkPad) r.left = linkPad;
    return r;
}

#endif   // W7UAC_BOTTOM_LINKS

// ---------------------------------------------------------------------------------------------
// The expander of the "Show details" row, embedded as PNG in base64 exactly like the shield: the
// button of the classic prompt. Every number of it was fitted to the reference of the original
// dialog, not to an impression of it: a circle 19 px across with a 1 px ring of `#333333` around a
// white disc, and a V 7 px wide whose ink rows match the reference row by row (arms from 2.7 px
// out and 1.0 px above the centre to a point 2.9 px below it, a 2.0 px stroke with round ends and
// a round joint). The label that follows starts 6 px after the button, and that the reference
// underlines the first letter of each word is part of the drawing too (see DrawDetailsRow).
//
// The whole drawing of every size is at **98.7%**: the button is the reference's own, made 1.3%
// smaller as asked. The canvas of each size is not touched - 20, 30 and 40 px - so the layout, the
// label after it and the hover box stay exactly where they were: what shrank is the button inside
// the canvas. Those three are the sizes the row asks for at 100%, 150% and 200% display scale, and
// at those scales the button is drawn 1:1, never resampled.
//
// Six states per size: the chevron down (the pane is closed) and up (it is open), each in the
// normal state, hovered and pressed. The normal state is the reference's own. The two glow states
// are modelled - the reference does not show them - and they take their palette from the Windows 7
// network flyout recreation (windhawk mod `win7-network-flyout-recreation`), whose expander button
// lights up with a fill of RGB(230,240,255) and a border of RGB(180,210,245) on hover: the same
// light blue, laid on the disc of this button the way Aero lights a round one up.
// ---------------------------------------------------------------------------------------------

static const WCHAR kChevronDownNormal20B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAB1UlEQVR42rVVPY+bQBB9NphidU1aV1aQTqIeV/GVFCn4C1R0/AAsIcsNVWp+BOUW10T+6FKaGokGKlyGIiDtCmVTxLYuwfguke91uzP7NG/n7ewIN0BEDwDmAGanrRLAIU3TH0NnRgNETwCWjLHPlmXp0+kUAFBVFbIs69q2/QrgS5qm3/BKRToRxY7jKM65EkKovyGEUJxz5TiOIqKYiPRbZNsgCFTTNOo1NE2jgiBQRLS9SkpEcRAEqus69VZ0XXcmjXt35jjOmyq7VulJ/hMAjE+cS8/zwBjDv4IxBs/zAGAJACMiemCMfd/tdrphGPgfSClh23bXtu2HMYC5ZVk9siRJYNs28jy/7OV5Dtu2kSTJH7mGYcCyLB3AfAxgdvbZSwghUNc1wjCElBJSSoRhiLquIYTo5Z84ZvqQDNd1sdlskOc54vh3E8uyxOPjI1zXHZSvAyirquoFNE1DFEVwXfci0TAMRFEETdN6+SeOcgzgkGVZJ6XsJZmmCd/3L2vf92Ga5tWmZFnWATicffjMOR/02mq1UqvVajDOOVdE9Pwuxr7709NetH1bFMWnoig+LhYLTCaTm2Zu2xbr9Rr7/X4HwD0ejz/fZXzdfcCO7v0F/AJEaTndiQe63AAAAABJRU5ErkJggg==";
static const WCHAR kChevronDownNormal30B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAAB4AAAAeCAYAAAA7MK6iAAADGElEQVR42s2XMWjifhTHP9daKYKTIKVQcOjQRRCedCwBQW47BxGk"
    L"y8F/7JLhhloliHPXW5xcSuYuR92EbqU/EF1a6RAa+HsIjkqhCbklCd71qlZ73H235P3e75OX9/J+L7CiRGRbRLZX9f+wLAT4CHwC"
    L"ssABEPHNDnAH3AKXwJVS6mktsA/8ApwAO4lEgkwmQyqVIplMAjAajbAsi263y3g8BvgOfAXO5z3AhznQLGAC+5qmUS6XEZG5USil"
    L"ME2TTqcD8ACUlVK3b8nfZxF5LhQKXq/X896qXq/nFQoFT0SeReTzW6CeruveZDLxVtVkMvF0XfdExFsIF5GsiDzruu45juOtK8dx"
    L"Avizn7qXOfYLqb+3t7d/cXFBLBbjPTSdTjk+Psa27QcgHRTcxsyaL8B+o9F4NyhALBaj0WgA7PsMQrAf7YmmaaTTad5b6XQaTdMA"
    L"ToKmE0T8Edgpl8v8Kfl77/isEPwpkUgs/E7XkYiQSCTwu18IzmYymVedhsMhxWKRYrGIbdsv7LZth/bhcPjqPj4jOws+SKVSrzq0"
    L"220sy8KyLAzDwHXd0Oa6LoZhhPZ2u/3qPj7jAGDDT3Yk6L2/k6ZpRKNRAPr9Pq1WK7S1Wi36/T4A0Wg0KKLfymdERGR7Y5n8pFIp"
    L"dF0Pr5vNJoPBgMFgQLPZDO/rus68NzeriFLqSUSc0WgUmbewVCrR6XS4ubnBdV3Ozs7CVw1weHhIqVSaCxuNRgCOUuopiPjOsqyF"
    L"T1mv14nH4wBhTgHi8Tj1en2hv7/+bra4brvd7kLHZDJJpVJ5cb9SqTCvRgL5jNtZ8OV4PEYptdA5n8+Ty+XC61wuRz6fX+inlAoG"
    L"hUtmxpcr4LtpmjvLNJFarcbm5mYY7TIyTTOYTq5+7Sw1EVnp4F9mMPDP5dpPh4Svc+DBMAym0+m7tcrpdIphGMEodP4C7J+TZdu2"
    L"nWq1+lN3WlWu61KtVrFt2/Hnr3D42/ylJ/+/u7trPz4+Fu7v7zk6OmJra2vlSE9PT7m+vgb4Tyn17Z8Y9v7aePvvDfR/+hdmnYli"
    L"rZ+2H1/ytZDtUfRQAAAAAElFTkSuQmCC";
static const WCHAR kChevronDownNormal40B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAACgAAAAoCAYAAACM/rhtAAAEaElEQVR42tWYz0sjVxzAP7sRBTElEA9VGBgIaA8NZHketoUcJLmI"
    L"RAt6NHpzIYKYXCSiIGxg8dJUcwjNTYbeDFvdQ/8ADyvBhwF7qAEhkEOkKIRmERTT9DITZnSjEzex6/c27+dnvu99fz34yuXFl0wW"
    L"QowAr4FhQAX69K5PQBE4AQ6klIdPBiiE8AFvgGmg3+a0c2AH+FVKme8IoBDie+At8JPRpqoqQgg8Hg+KotDd3Q3A9fU1pVKJ09NT"
    L"pJQUi0XzUr8Da1LKP9sCKIToAt4BS0CX0+lkamqKyclJFEWx9XOlUond3V2y2SzVahXgBvgFiEspbx4NKIT4FvgAjDgcDsLhMHNz"
    L"czidzkfdp2q1yvb2NpqmUavVAA6BkJTyrGVAIcR3wB+AqigKGxsbDA0NtcUyC4UCy8vLlEoldGMak1L+ZRtQ19xHQPX5fCSTyUdr"
    L"7T5tRqNR8vm8AfnD5zT5osmd+wiM+Hw+tra26O3t7YiPu7y8ZHFx0YA81CEtd/LlZ+a9A0YURSGZTHYMDqC3t5dkMmkY24i+d3MN"
    L"6q7kyOFwdGma1rY7Z+dOhsNharXaDfDK7IJua/At0BUOh58MDmBoaIhwOAzQpTPc1aAeIY6cTid7e3ttNwo7RjMxMWH4yVdGxDFr"
    L"8A3A1NTUk8MBGAHAzHIbcBpgcnLyf8tcTHtPWwD1rKRfVVXb4asToigKqqoC9OtMDQ2+1kFtLZTJZAgGg0QiES4uLpqOu7i4IBKJ"
    L"EAwGyWQydpMSzEwG4DCAx+Ox5RIymQyVSoVcLkcikWg6NpFIkMvlqFQqZDIZCoXCg+ubGIbNgKqh4ofk6urK8r2/v082m70zLpvN"
    L"sr+/f+/cZsdsZjIA+4BGPnefeL1e/H6/pW1zc5Nyudz4LpfLbG5uWsb4/X68Xu+D65sY+pqFugdldXUVl8tliakrKyvUajVqtRor"
    L"KytcXl42+l0uF6urq48ynJemGoLr62tbk9xuN/F43NJ2fHyMpmlomsbx8bGlLx6P43a7ba1tYvhkBiwama9dCQQChEIhS1s6nSad"
    L"TlvaQqEQgUDA9romhqIZ8ATg9PS0JfXHYjEGBgYa38YRGzIwMEAsFmtpTRPDiRnwAEBK2XJ4Wl9fb9q/vr7ectg0MRw0APW69bxY"
    L"LLZ0zIZjnZmZudM+MzNj2/Gbj1evAM+NWtpsxTsAu7u7LVtaJBKxOHmPx0MkEml5HdPeO21Pt8rlciOczc/PW+7ml6Rbt4/rvRCi"
    L"nkql6k8tqVSqLoSoCyHe31eTrAE3mqbZipvtTPk1TTMK+jVzn+PWMf09ODj4Tb1e/zGXyzE+Pk5PT0/HM+mFhQUqlQrAz1LK3x6q"
    L"6uLAYalUIhqNWkJWJ8rOaDRqeI5DfW+aalDX4r+Dg4MfgOmzszPX0dERo6OjbddktVplaWnJXLgHpJT/NIvFt53lGTAGFPP5PLOz"
    L"s229k4VCgdnZWTPcWLP3mef7ePQsnt+exQPms3kCbgLb8Uf0r17+AzJDh5sQTnL2AAAAAElFTkSuQmCC";
static const WCHAR kChevronUpNormal20B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAB7UlEQVR42rVVoa6jQBQ9ZVtISAVrq8g69K3aPlzFCn4AsQqNpYpKkuomOBQfgCNN2rqVi25CQqiick2ZBB7srGlfXhb62t28d9zMvXPmzrlnZgZ4A0Q0BjAFoF6mjgB+JklyvrVmcIPoCcBCluVvmqYNJ5MJAKAoChwOh4YxtgGwSpLkB+5UNCSitWEYPIoiXlUV/xtVVfEoirhhGJyI1kQ0fIts6zgOL8uS30NZltxxHE5E215SIlo7jsObpuGPommaK+m6o5lhGA9V1lfp5fhPACBcOBeWZUGW5V45XNeF67q9MVmWYVkWACxerKHr+nNfAzjnPAxDTkSciHgYhr05VVVxXdefiWgsAJhqmjYURbGze5Zl8H3/Zez7PrIs6+SJoghN04YApgIA9eqz12jbFq7roq5rmKYJ0zRR1zVc10Xbtp38C4cq3PJkGIZI0xSqqsK2bdi2DVVVkaYpwjC86WUBwLEoik5AkiQoigLP8yCKIkRRhOd5UBQFkiR18i8cxwERjWVZ/rXb7Xp1fAR1XWM+nzeMsc9CkiRnxtgmjmP8L+I4BmNskyTJ+arhKggCMMb+mYwxhiAIAGD1IVfv06u2b/M8/5rn+ZfZbIbRaHS3suVyif1+vwPw/XQ6/f6Q5+vdH9jBe38BfwDPpwN6veeyDAAAAABJRU5ErkJggg==";
static const WCHAR kChevronUpNormal30B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAAB4AAAAeCAYAAAA7MK6iAAADDUlEQVR42s2XMWgiTRTHfxcTCLYLIQQCWwRJIwhjHaz0urOVNMKB"
    L"TZotrggKIotok8Im9VVBsEtzHAvCQjq5gaDNBVIsWfgSFqxEOczKfsXN7nncnRqTcPfvdnfe/ObNe2/mLawpIcS2EGJ7Xfs3q0KA"
    L"t8A7IA0cApvqsw98Bb4Al8BnKeW3Z4EV8ANwAuxqmkYqlULXdXZ2dgDwPA/Hcbi+vmY4HAI8AOfA2aIFvFkATQNt4CCTyVAoFBBC"
    L"LPRCSkm73ca2bYBboCCl/PKU+BWFEI/5fD7o9/vBU9Xv94N8Ph8IIR6FEMWnQAPDMILxeBysq/F4HBiGEQghgqVwIURaCPFoGEbg"
    L"+37wXPm+H8IfVeh+jbFKpMH+/v7BxcUF8Xicl9BkMuH4+BjXdW+BZJhwG3NjPgAHpmm+GBQgHo9jmibAgWIQgZW3J5lMhmQyuXSy"
    L"0WhEuVymXC4zGo2Wjk8mk2QyGYCT8NAJPX4L7BYKhZW8qNfrWJaFZVnU6/WVbNTcu4oVgd9pmra0TgEsy6Lb7UbP3W4Xy7JWqRY0"
    L"TUOdfhE4nUqllhp7nkez2fzlfbPZxPO8pfaKkZ4HH+q6vtSwVqtFMdV1ndBmNBpRq9WW2qvxhwAbKtib4dn7J3U6HXq9HgCxWIxG"
    L"o0Gj0SAWiwHQ6/XodDoL51CMTSHE9sYqieE4Dq1WK3oulUokEgkSiQSlUil632q1cBxnpWTbUAXtL4qRbdtMp9OoNIrFHydgsViM"
    L"SnA6nYYXxB9zBPCllN9Cj78uWmkul4tiappmtL3htpumGX3P5XILd07d3VGqf8xms8FrK5vNBkKIj/NZfTkcDpFS8lqSUoaNwuU8"
    L"+DPw0G63Xw2s5n5QrO9glWDntm0zGAxeHDoYDMKkO//d7XQG3FarVSaTyYtBJ5MJ1Wo1bIXOfrqd5rwuuK7rVyoVZrPZs6Gz2YxK"
    L"pYLrur7qv6LmLzY/8P7+/r+9vT337u4uf3Nzw9HREVtbW2t7enp6ytXVFcB7KeWnf6LZ+2vt7b/X0L/2L8zaeu5P2/+uG8IShiW5"
    L"IwAAAABJRU5ErkJggg==";
static const WCHAR kChevronUpNormal40B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAACgAAAAoCAYAAACM/rhtAAAEbElEQVR42tWYQUgjVxjHf+uIgpiSg4cqDAwI2kOFLGNlW8hBehKp"
    L"FpLr6C1LEExykoUIworiJSkehPUWkt4iqHPoyZOHLTYPA/ZQA4HAHCLFQGhKDsHUXmbCjGvWGVfd9bvNm8x7v3zve//v+x584fbi"
    L"Uz5WVXUSeAWMAwowaL76F6gA58DvQojCkwGqqhoAXgNhYMjlZ5dAHngnhCg+CqCqqt8Cb4GfrTFFUVBVldHRUWRZpq+vD4BWq4Vh"
    L"GJTLZYQQVCoV+1T7wKoQ4s8HAVRVtRfYBOJAr8/nIxQKMT8/jyzLrv6cYRgcHBywt7dHo9EAuAJ+Ad4IIa7uDaiq6teADkxKkoSm"
    L"aSwuLuLz+e4VT41Gg0wmQzabpd1uAxSAn4QQF54BVVX9BvgNUGRZZmtri7GxsQc5maVSiZWVFQzDwDxMM0KIv1wDmp57DyiBQIB0"
    L"On1vr33Mm4lEgmKxaEF+f5snX3SJuffAZCAQYHt7m4GBgUfRuGazyfLysgVZMCEdMdlzy3ebwKQsy6TT6UeDAxgYGCCdTluHbdJc"
    L"u7sHTSk5lSSpN5vNPljMuYlJTdNot9tXwEu7BN304FugV9O0J4MDGBsbQ9M0gF6T4UMPmhni1OfzcXh46PlQVKtVdnd3AYhEIgwP"
    L"D3s+NHNzc5ZOvrQyjt2DrwFCoZBnuFarRTweR9d1dF0nHo/TarU8zWElADvLTcAwwPz8vOct2tnZoVwud57L5TI7Ozue57GtHXYA"
    L"mlXJkKIortOXZUIIcrncB+O5XA4hhKe5ZFlGURSAIZOp48FXJqjnuFlbW+v6fm1tzYopL9USdiYLcBxgdHTU02SpVIpqtdp5liQJ"
    L"SZIcByeVSnma08YwbgdULBe7taOjI3Rdd4xFo1Gi0ahjTNd1jo6OPG2znckCHAQ69dxdVqvV2Nx0iv7ExASapqFpGhMTE87UtLlJ"
    L"rVZzNbeNYbBbqrvT1tfXqdfrjpS1sbHR2eKNjQ1HiqzX66yvr99LxHtsPYQr7To7O+P4+NgxFovFHMI8PDxMLBZz/Ob4+JizszNX"
    L"mmpnsgArVuV7l/X39zueg8GgXWA7FgqFCAaDH/22W/VtZ7IAzy2BdZM3I5EIfr+fqakpkslk198mk0mmpqbw+/1EIhFX+d3GcN7J"
    L"xaYo/qEoCvl8/rP2weFw2GqyvhNCFHrMbFAALiuViqttfiwzDMOCu7R6afspzgMcHBx8NkDb2vnbioV3gL01fFJrNBrs7e05WByA"
    L"Zv21b7WGT22ZTMZyzL799uGmUK8CV9lsllKp9GRwpVKJbDZrNfSr9nfSjar475GRka+ur69/ODk5YXZ21pV2ferWLi0tWZkpJYT4"
    L"9a6u7g1QMAyDRCJBs9l8NLhms0kikbCUo2CuTVcPml78b2RkRAfCFxcX/tPTU6anpx/ck41Gg3g8bm/cfxRC/NMtF9+ski+AGaBS"
    L"LBZZWFh40JgslUosLCzY4Wa63c8838ujZ3H99iwuMJ/NFXAX2Ee/RP/i7X8/ZhVnE2yQjAAAAABJRU5ErkJggg==";
static const WCHAR kChevronDownHover20B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAD4UlEQVR42q2VT0gcVxzHP+/N7IRdSc1uV9k1pkYWE/AQYhEUoTWl"
    L"RhIaJZhDYC0WAkESKJVcgpQ0eArmlIJNDpYcbE1IIB5ipGAluKSIUttIDqFk2ZpN/YcuuxpR6+zMvB5mVrS0tzz4wvu9+f6+833v"
    L"/eY3Am88ebFRBJQBUSAMBNdXM6GX04nK7PJ8CCBUejBbXds4u/9AOAvkgAywCCycOVa0ASA8sfeAw0A5EJ0ef1wzNfaoaS27XBWt"
    L"OCKD4QgAucwSi+lXTnGoNFnXdG6s9kTrc09wDnh95ljRW+E5iwGH8+Z2xeCtq/HcykJ9Y0sHxxuaMQwDIQQASilM02RmYpTE8ADB"
    L"krLJ9q7eez5jXxp4DaTEkxcbVUBV3tyO3bl+4atIeSzWdrEbvz+AJkEK4W4DUICjFLYDW1ubDPXfYGkulbrUc/dbn7EvBSSld2bR"
    L"wVtX45HyWOz85esUBQIYmsDnwdBd7MSaoCgQ4Pzl60TKY7HBW1fjBR0JhKfHH9fkVhbq2y52Y/h0fJpA9xL3COnuml6IfTptF7vJ"
    L"rSzUT48/rgHCEghOjT1qamzpwO8PoEuBJve6KTjc7VqTAl0K/P4AjS0dTI09agKCcn01E1rLLlcdb2h2z0yCXsBuUW+ua2LnuZSg"
    L"STje0MxadrlqfTUTki+nE5XRiiPSMAykEC6k4OnIQ7o6TrOQTmLoEkOXLKSTdHWc5unIQ6QUO3zDMIhWHJEvpxOVMrs8HwqGIwjv"
    L"NqVwi9PKm6y/XaXv5jWwTbBN+m5eY/3tKlbe3MMVQhAMR8guz4d0/mecOhvn119+5s1skh/vfgfA/F+v+aCyilNn41jqv/NkqPRg"
    L"NpdZQinl1Zlbb0JqdF7pwWcYDA/dZ3joPj7DoPNKD0Jqe7hKKXKZJUKlB7OyurZxdjH9yjFNE0cpF47CdhSRQ5W0tXfuvL2tvZPI"
    L"oUpsx+UU+KZpsph+5VTXNs7q+w+Es8Wh0uTMxOjRuk9a0ARYAtyvTXGyNU76zyQAJ1vjWLbCcsBywHHAdmBmYpTiUGly/4FwVgdy"
    L"dU3nxhLDA0eP1X+KDAQ8MVAKbAVffPkNAKbtOrMcsB2F5Si2tjZJDA/w0WefjwE5CWRqT7Q+D5aUTQ7138DMW+RthWUrTFthWoq8"
    L"7cK03DWrEOcthvpvECwpm/Q6T0Z67Wexvav33tJcKvXgdg8bm5uY9r+Edgvbio3NTR7c7mFpLpVq7+q9V9DR4pe+3gZ0TdPFhx+f"
    L"+eP3ZyPBxPAP5brh5/3oYRASx9u67Sj+3jb57dlPPLzTg+4zJi90933vta83wJt332Df9S/gH5HE9MShqnYDAAAAAElFTkSuQmCC";
static const WCHAR kChevronDownHover30B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAAB4AAAAeCAYAAAA7MK6iAAAGlElEQVR42sWXX0xUVxrAf+fccWRmGIYZHQ3Q0A0VloDialwEjc26"
    L"ENuwvKi0/iFrbLKNqzXajYmbaF3TNb7VbEx205o+1dTYEKRJQ1yXQLObrnXEpEYQQiC4gkXQgcFhGC6QmXv2Yc7cGbAPffMm3z3n"
    L"3vud8/vOd75zzncFWVd7T1wCPmAVkA/kAV7AA7iAlYATcMzPxZwAOW7vIpAAFoEFwATiQAyYAV4AU0C0scpjpVkiC+rQwNVAQIN9"
    L"gNeMx3zdXW2VQ713K6fDT4viMy9WK2VJACGk5cnLn/QHC8dKN2ztq67b0+fyeKMaHNXgCDAJTDVWeRI2WENXA2uywH4zHlvVfvVS"
    L"3WBPqGZxfi431xeguLSKYEExeYEgADORMOHxUUaHepiNRnDmuGfLqmpCjYdOdbk83ilgOgv8HJhsrPIkhHZvEFibDe4NdVbcvHb5"
    L"wNxsNFC+eQe1u5ooKd+UslRkXKX0TQGPBu5zp6OVgR++w53rizQ0n7y+oaa+fxn4GRAW7T1xP1CYDb711d9/E+po3e0PFsi9R85R"
    L"/EYFUoAUAiGWzpFKlwospbAUjA73c+PKBabD41bNrqav395//N/LwE+Ng0fPvqaBQQ3deedfLXvLNtaKw6f/RnBtIYYUGFIgJUhd"
    L"Tz2nRGiDUoYJfIEgm3b8joknw+LB7VsVC/Nxc9366gnA0pKUOojyAX9vqLMi1NG6u2zjNppPXMTlcmkIGIbAYQgcEhxSLBNwGALD"
    L"0LpS4HK5aD5xkbKN2wh1tO7uDXVWAP40T+ol4zPjsVU3r10+4A8WyHePnsfhMOyRLYGk4YYWDc3Wsds5DN49eh5/sEDevHb5gBmP"
    L"rdIrJU/qdeptv3qpbm42Gth75FzWSDMjSJcZ+NL6T+mmR773yDnmZqOB9quX6tI8CXjMeMw32BOqKd+8QweSQAowBBgiG8rLHrC9"
    L"kgUVAkNgB2TxGxWUb97BYE+oxozHfIBHAq7urrbKxfm53NpdTVoZO3DsgBIZuEPq+TQyc2xonew2Ugq7v9pdTSzOz+V2d7VVAi4J"
    L"rBzqvVuZ6wtQUr7JXjIyy+I0NNv9mdEu+6bbpNuno72kfBO5vgBDvXcrgZUScE6HnxYVl1YhSCkKIey6FBAJT/DR8f2c+WAfk8/G"
    L"sqI7FViTz8Y488E+Pjq+n0h4wgam+hB2vbi0iunw0yLAKQFHfObF6mBBsb0rLIffu93JxNgI4z+O8Nml86CsFNwQoCw+u3Se8R9H"
    L"mBgb4d7tzpeg6R0nWFBMfObFasAh5+diTqUsmRcIppSzRXeweeubrHA6ARgaeMg3LV/Yrv2m5QuGBh4CsMLpZPPWN5e0zZa8QBCl"
    L"LDk/F3NKfsZVUPQ6+947YT+3fPk5j4cHeTw8SMuXn9vv9713goKi139Olzhy3N5FIaQ1EwlLpfdeW1SmXt/QxP3Qf+h7cI9kMskn"
    L"F88CkEwmAajc+GvqG5pIWOqltmmZiYQRQlo5bu+iBBKevPzJ8PioveMrBUplOrD0AfD+h+dwe7wAjD15zNiTxwC4PV7e//CcfUjY"
    L"UKVQKnOahMdH8eTlTwIJCSz6g4Vjo0M9GUuXQZXu0BdYw+Fjp19y2+Fjp/EF1izRXQ5XwOhQD/5g4RiwKIGF0g1b+2ajER4N3MfS"
    L"ilbWSJMKkpYiaSm2bK9ny7adNnTLtp1s2V5vf09mHY9pAyyleDRwn9lohNINW/uABQdgVtft6fvvP6/P3ulozS0p34SlQFgKCxAI"
    L"BCq1oC2FJQSHjp1BSAOA3//xzySs1MiSCixLYVnpMmPAnY5WnDnu2eq6PX2A6QDiLo83WlZVE3rY/W396HA/v1hXgVCCpO1vgUKh"
    L"hEAKhdPl4Q9/+qudCCRtQAqaTBuh340O9zPww3esr/5tSOdjcamTsljjoVNd7lxf5MaVC5immXGdxZIyYSkSSV1m1X9KN2kpTNPk"
    L"xpULuHN9kcZDp7rSPKlT0KjL451qaD55fTo8brV8+jGJRNJunLCWgyCR1GKx1BDbCEUikaTl04+ZDo9bDc0nr+vkLwrMGAePnnUD"
    L"KwBj7Wslc/Pm7MKD27cqno4M8ctfbcdwOFL7jh2pYtky05KeW+1e0zT56h9/YfDB99S+9U7b9rf3d+v8egqYNA4ePasAB2AAct36"
    L"6ucL83Gz5/uO8ofd34rCkgq8/qBe4iloJlqX1tNzOzLcz9VPTjH2vwGr9q132nSyN6UTvkng+atLb19ZQv9Kf2FexU/b/wGTedPe"
    L"tpOQlwAAAABJRU5ErkJggg==";
static const WCHAR kChevronDownHover40B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAACgAAAAoCAYAAACM/rhtAAAJPUlEQVR42tWZe2xUVR7HP+dMH9Pp9DWt08fIUKRIEYtCEJQGDZAQ"
    L"sSQq2Y2EGtT1FRUiRFeMFTcbjYYsvlbNZrM0sgqGxg0EBUlMKitICIWAUpFq61JaWtoJLW2ZR9vp3Lt/9Nw7507bzca/9Can5965"
    L"5/zO53x/53V/FUxyHTgbyQRygRyVsgEPkAW4gUyV0oE0wAVIQCgTJmAACWAMiAMjKg0DMSAKRIBrKg2tnpc9ksoiUsCEgsgH8hSc"
    L"VwP06IDNJxqD7S1ngv2hLn946Gr+WHw0AyAtPWPUm1sw4PMHQuWV8zuqFq/oSAGMaoBhBTgIDACx1fOyzQmACi4bKFCAuRqg11Lw"
    L"h1Nflzc17l186cL5ufGRYQ//x5We6Y5eP2POuUUr1py4aeFd7ZqCYQ1wSAFeBSIWpEiB8ynAvFQX//jtsfKv9tav7Olsq7QaLioN"
    L"Ul45n+LADHz+AGnpGQCMxUfpD3XR23WB9pYzXLncYcOWTKtoWb7m0S9n31rdnupipeJVoN+CtAA9QKECzNcAc8fio3l73n+5pq25"
    L"6Q7TNKTb4+W2Zfey4M7VFBYHEFYvRYps5vhANIG+3i5OHznAycP7GY6GEUIaFVWLjq/d8NrBtPSMQQU3pLm5H+hbPS87KtSEKFSp"
    L"QAPM6730c+nut7c8PNgfKpPSRfWqtSy9p5asbC9CjDMJIZJs+hRRmWmaKodYJMzRL3Zz7NAeDCNBns/fXbt5287i62deVnCDmpv7"
    L"gD5x4GzkOqBIc28+kN/W3HTDv/7+50dikWv5Pn+AB555lbLpFUiRhBICG3SyywIzzSSsYUL3xTYaPthKf6iLrOycgd89+acPK6oW"
    L"/UfBDWhuvuJa91RdSYpy+b2Xfi775N0Xn4hFruUHZ1Xxhxf/SuF1JUgBUghcAqQEKQVSiGSekgRivDN2J8b/5uT5uGXJ3VxsPUtf"
    L"T6e79ezxObPm3d7izfWNquXJSgmZMlNzxuKjebvf3vKwBbf+ue14vF6kFLikwKXAXHIc1CVBCtS7ZJLqnUu9k6qude/xeln/3HaC"
    L"s6qIRa7l7357y8Nj8dG8VB6pZm+2td7tef/lmsH+UJnPH+DBTdtwZ2UpRXAqKDRQu3E9CRtGClQdpx13VhYPbtqGzx9gsD9Utuf9"
    L"l2t0FiBbaguw58dvj5W3NTfdIaWLB555FU+212F0vEGcalrqiZSkq2iBWvcapCfbywPPvIqULtqam+748dtj5TqTVDuHB8j6am/9"
    L"StM0ZPWqtZRNr0DYhpKGXZqCFshE9TQVRVJB+16DFALKpldQvWotpmnIr/bWr9SZpNq63D+c+rq8p7Ot0u3xsvSeWruHVi6sZ01B"
    L"qUGkjkFbxRQFHba0fOk9tbg9Xno62yp/OPV1ucUlrX21qXHvYoDblt2r1jlhLylyEoO6mvqYc4xJTbXJbOhtZGWPbwAAiiUTyLQB"
    L"L104PxdgwZ2rtUU46QY9tyaIvdQ4xpv+XiszhS29rQV3rgZAsdiA6c0nGoPxkWFPUWkwuX05FuSkWyZzjyt1okhtpk8yTCazLYDC"
    L"4gBFpUHiI8Oe5hONQSBdAmntLWeCAOWV8ydUsntq3ws+b9jBs+tXsf2VjVwb7J9ymbk22M/2Vzby7PpVfN6wIwnHRPUs4PLK+QAo"
    L"pjQJuPpDXX6A4sAMe/O3KtrbrDJ+qb2VzxrqCQ8NcO67k9S/93rKjE7O3Pr3XufcdycJDw3wWUM9l9pbbZikXRxtFgdmAKCYXBKQ"
    L"4aGr+QA+f2DCxupQE4iPOg+9Z5q+ofHQvuSMVuOu8dA+zjR94ygbHx1xdF4HtXZCnz8AgGKSEhDaSXiSc7bzmjn7Zm5ZWO347eMd"
    L"73Kl97I9Ea70XubjHe86ytyysJqZs2+e2rBqUztTZgBC8guuRza8RE5uvv08HIvx1ht1GIkERiLBW2/UMRyL2e9zcvN5ZMNLv6Qp"
    L"JGCmpWeMWidh/Tw31ZVXUMhDT7/g+O2n89+z79Nd7Pt0Fz+d/97x7qGnXyCvoPB/G1VtWgyKyUwDDG9uwUBofGCOF9RcbJompikw"
    L"hX6+M1m4ZDlLV9RwtPGgXfaTnX+b0O7SFTUsXLIcwzCT50LNTipkf6gLAG9uwQBgSCDh8wdCAL1dF5IGlBEnqPMQuu6xTRT5S+0y"
    L"iUSCRCJhPxf5S1n32KaUQ6sTzAGtGNSEDVnnwbHyyvkdau1JHtM1g4bjfvxUbJgmbk8Oj2/aOqXXHt+0Fbcnx1HHsmtMAm0qBrUm"
    L"dwBjEohXLV7RkZ7pjl653EFfb9eEShawoeVWgzfOXcDd99VOgLv7vlpunLtgApwNmWLb+ri6crmD9Ex3VH1Lx6X1QX39jDnnAE4f"
    L"OTBRvZTcMMEwTAxjPL+/9gkCwRtsuEDwBu6vfcJRxpjClt7W6SMHAFAsI8CIDbhoxZoTACcP7ycWCdsfOKamgDOHhGmSMEG6MthY"
    L"t50ly2pYsqyGjXXbka4MEqqM3akUG3obsUiYk4f3A6BYRoCRNBWKGL5p4V3tJdMqWno62yqPfrGblb9/EjARprBzAxMMAcLEVPuy"
    L"KUyEgIKiEh7aUJecMLYrdfdOPkwME45+sZvhaJiSaRUtKvowDAxLLQwRW77m0S+FkMaxQ3vovtjmcGlCuclSzb43TBIGUyTTVlBX"
    L"M5Hi8u6LbRw7tAchpLF8zaNf6kxSC+REZ99a3V5Rtei4YSRo+GAr0UjY6VJtTFlg1u/j4FqyOmRY9cbr6nCGaRKNhGn4YCuGkaCi"
    L"atFxFRKxmaSKj1hRpsjaDa8dzPP5u/tDXex6ZwvDsZgD0tAUTIKak6tngSkFU+0Mx2LsemcL/aEu8nz+7rUbXjuoswARqQI3dpQp"
    L"LT1jsHbztp1Z2TkDHa3NfPTm80TDYadq1r1SyFZRS7Z6ZlJBXc1oOMxHbz5PR2szWdk5A7Wbt+1UcRoHj2vdU3VpqUFIb65vrGTa"
    L"rIutZ4/P6evpdJ879W+CN96KN8+ndiQxybY1MRloy1JK6OOff9lMT2ebHfoIzqrqVLGZIT1O41r3VF1CgTmipL7iQGzWvNtbfvru"
    L"+MyBKz05p48cID46Qun0SlzpGUkwC3YSYHuWKtBoJMzh/R+yb8cbRMOD5Pn83ev/+NY/poC7Cgz8+sNvv4kA5q8+BPybCKL/Wv8N"
    L"8V9v5vnVqN3y1gAAAABJRU5ErkJggg==";
static const WCHAR kChevronUpHover20B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAADz0lEQVR42q2VT0hcRxzHPzPv7ZNdMXG3G3HVoGHRQw4hFsHgoYYi"
    L"Xqo5mNIQDR4EKfkD9RSRUsTkIF6CJSVBDIVYXEhBCZoSSSxkCYiCbSSHUCLLRmN0WZfdpOJufbvvTQ/7dlsPuWXge5iZ3+/Dd2Z+"
    L"/EbgjEcv90uBKiAA+AHv3vuE79Vq+EQy/s4H4KuoTp5sao2WlfuTQApIADvAdsep0n0A4cCOAHVADRBYfTbXuLI40/YhGa8P1DZI"
    L"r78SgFQixs7Ga/uor2K9ue38YtPZcy8c4BbwpuNU6d/CcRYE6rLmQe30+GB3anf7TGtnL6db2jEMAyEEAEopTNNkbekJ4fkpvMeq"
    L"lnsGxkIuo2QDeANExKOX+/VAfdY8CN4d7vuusiYY7Oofwu32oEmQQuSPASjAVgrLhkwmzezkKLGtSOTyyM8/uoySCLAunTsLTI8P"
    L"dlfWBIMXrgxT6vFgaAKXI0PPqzjXBKUeDxeuDFNZEwxOjw92FzgS8K8+m2tM7W6f6eofwnDpuDSB7iQeAun5Nb0wd+l09Q+R2t0+"
    L"s/psrhHwS8C7sjjT1trZi9vtQZcCTR52c//2De7fvnHItSYFuhS43R5aO3tZWZxpA7xy733C9yEZrz/d0p6/Mwl6QZrg6VyI5fAC"
    L"y+EFns6F0DVR3JcSNAmnW9r5kIzX771P+OSr1fCJQG2DNAwDKURejsvY2yiz0xOFUmV2eoLY2yiazMcU4g3DIFDbIF+thk/IZPyd"
    L"z+uvRDivKUW+OJVtMXFrmKxp0tl1kc6ui2RNk4lbwyjbOhQrhMDrryQZf+eTfGQsPAyxGV2n+ngdl/qucqnvKtXH69iMrrPwMPSx"
    L"NKSvojqZSsRQSjl1lq833WVQdqSca9dvgmaAZnDt+k3KjpSju4xDsUopUokYvorqpH6yqTX6++w92zRNqWsl2ApsG7786hvaz11A"
    L"lwIzZwNQVVvP+NRjcpYiaylslZdpmuxsvLa//vaHqCwr9yeP+irW15aeYNl5WK4gJ9F0lLUUOUsV920bLBvWlp5w1FexXlbuT0og"
    L"1dx2fjE8P0UmkyZnKyw7n1yE5RxZ/61btiJnKzKZNOH5KZrbzi8CKQkkms6ee+E9VrU8OzmKmc0VnRRgRbgDLTrP5pidHMV7rGrZ"
    L"6TwJ6bSfnZ6BsVBsKxJ5cGeE/XT6kJuCw/+73k+neXBnhNhWJNIzMBYqcLTuy98fALqm6eLzLzr++vP5b97w/C81uuHms0AdCImt"
    L"wFJg2Yp/Dkz+eP6YX++OoLuM5b6hn+457WsT2Pz0DfZTfwH/AojfFY4dJcW1AAAAAElFTkSuQmCC";
static const WCHAR kChevronUpHover30B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAAB4AAAAeCAYAAAA7MK6iAAAGgElEQVR42sWXX2hUVxrAf+fc2ZjJZDLOmKkmsVG0yYZE4yo0JhVh"
    L"l4gtbij4p6FqsftSbGCpDyn7ErRIVh/KirW0FmmhJSD+IbovIjYk26VWHf+gmGwkGHQxS0zXSSZOJjM3md65Zx/mzJ07WR/65oVv"
    L"znfnft/9nT/f+c53Ba7r0mBSAgFgCbAYKAP8gA/wAouAIsAzl0oUARSX+NOABaSBecAEkkACmAGeA1NAvK3RZ+dYwgX1aGA5ENLg"
    L"AOA3k4nArYGLDaNDNxumo0+rkjPPy5WyJYAQ0vaVLZ4MhivHa9ZuHG5q3THs9fnjGhzX4BgwCUy1NfosB6yh5cArLnDQTCaWXOo5"
    L"1vpwMNKcnkuVlgZCVNc0Eq6opiwUBmAmFiU6McbY6CCz8RhFxSWztY3NkbZ9nQNen38KmHaBnwGTbY0+S+jpDQNL3eChSH/95dMn"
    L"dqdm46G6DZtp2bqLVXXrsz0V+alS+kcBj0fucaOvl5G7VykpDcS27T1wZm3zlgcLwP8FouLSYDIIVLrBV85+8ftIX+/2YLhC7tx/"
    L"kOrV9UgBUgiEKFwjlWsV2EphKxh79IALp7qZjk7YzVt3/f2td//8zwXgp8aejq7lGhjW0D/c+P78ztp1LeJPfzlOeGklhhQYUiAl"
    L"SK1n77MidIeyHRMEQmHWb/4jP//nkbh/7Ur9/FzSfG1N08+ArSUjdRAtBoJDkf76SF/v9tp1b7D3oyN4vV4NAcMQeAyBR4JHigUC"
    L"HkNgGNpWCrxeL3s/OkLtujeI9PVuH4r01wPBHE/qLRMwk4kll0+f2B0MV8j2jk/weAxnZAWQHNzQoqFuG8fPY9De8QnBcIW8fPrE"
    L"bjOZWKJ3SpnU+9R/qedYa2o2Htq5/6BrpPkR5No8vFB/kW1u5Dv3HyQ1Gw9d6jnWmuNJwGcmE4GHg5Hmug2bdSAJpABDgCHc0Gyb"
    L"NpN8c/wQ3xw/RNpMFjwzpPYROAFZvbqeug2beTgYaTaTiQDgk4D31sDFhvRcqrRl6y5tjBM4TkCJPLzn5FFu/9TP7Z/66Tl5NA91"
    L"/Fz++n0tW3eRnkuV3hq42AB4JbBodOhmQ2kgxKq69c6Wka4e56CGFNy51s+d6z84afbO9R+4c60/vzTaJ+efi/ZVdespDYQYHbrZ"
    L"ACySQNF09GlVdU0jgqyhEMLRpb6XAuKxZ3x38lMWXt+d/JR47FmBrdBJJrfVBFBd08h09GkVUCQBT3LmeXm4otrJCi+CSyH4+rNu"
    L"UskEAFWvrqTq1ZUApJIJvv6s2xnpQmgu44QrqknOPC8HPHIulShSypZloXDW2C0ir/df7mX4/m0ADMPg464jfNx1BMMwABi+f5v+"
    L"y70v9M1JWSiMUracSyWKJL/imhh/wrlvP3fu29/7gJWra1m5upb29z5w/j/37edMjD/5Na9EFpf400JIeyYWRenc64hO/ndv/sgv"
    L"6TQANXVreLv9fTK2ImMr3m5/n5q6NQD8kk5z9+aPBb5umYlFEULaxSX+tAQsX9niyejEmJPxlQKllPOC1zdtYVnVCiqWr+DDzsMg"
    L"JFZGYWUUCMmHnYepWL6CZVUreH3TljxUKZTKnybRiTF8ZYsnAcsDpIPhyvGx0cFXVAFU6BMHQuFl/PWLs9mDQYCVUQgdNUopypdW"
    L"cfTLc84sWDb/B1fA2OggwXDlOJCWwHzN2o3Ds/EYj0fuYWtD2xFFRuG8NCtg2QpL6wXPXMejrfLH5eORe8zGY9Ss3TgMzEvAbGrd"
    L"MVxUXDJ7o683D7SVlqyeg1samslkxdL/OVCXj23nO3Cjr5ei4pLZptYdw4ApgaTX54/XNjZHRu5eZezRA6fHGQUZlR+V5bSF4n6W"
    L"sbWPa8bGHj1g5O5VahubI7oeS0pdlCXa9nUOlJQGYhdOdWOaZsG0ulvLzgaVtUB/kW3GVpimyYVT3ZSUBmJt+zoHcjypS9C41+ef"
    L"2rb3wJnp6IR9/qvDWFbGFSwLQWBltNgUdsS13paV4fxXh5mOTtjb9h44o4u/ODBj7OnoKgF+AxhLl69KzZmz8/evXal/+mSU3/5u"
    L"E4bHk807Khepwtlmtltya6un1zRNzn55iIf3r9Py5jsXN7317i1dX08Bk8aeji4FeAADkK+taXo2P5c0B6/31f3r1j9E5ap6/MGw"
    L"3uJZaD5aC/Xc2j559ICev3Uy/u8Ru+XNdy7qYm9KF3yTwLOXV96+tIL+pX7CvIyPtv8BgvQMmv9lwxIAAAAASUVORK5CYII=";
static const WCHAR kChevronUpHover40B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAACgAAAAoCAYAAACM/rhtAAAJM0lEQVR42tWZe2wVVR7HP3Omj/vqgwve0l4oZW21iEVrpOiibpTE"
    L"LMpqIGtgAQ27JbshahRlg89sdhE3uLqyazVZH0GMNZhsUDZUjUlBIYWADSgVLVKktLSUm757X33cmf3jnpl7ZtpuNv6lk5yemXvO"
    L"/M7nfH/nNb9qTHHtPxXLBfKBPJn8gA/wAh4gV6ZsIAvQAQFo0oQJGEAKmADGgVGZkkACiAMxYESm4RWL/KNuFs0FpkmIQqBAwgUU"
    L"QJ8K2HKssbS99WRpf6QrFB0eKJwYH8sByMrOGQvkzxgMhsKRssrqjqolyzpcgHEFMCoBh4BBILFikd+cBCjh/MAMCZivAAYsBb9p"
    L"/rzseOPeJRfPf7twfDTp4/+4snM98TnzF5yuWbbq2DU3/qJdUTCqAA5LwAEgZkFqLrigBCxwu/jMl01lB/a+dWdPZ1ul1fCs4lLK"
    L"KqspCs8nGAqTlZ0DwMT4GP2RLi53nae99SS9lzps2Nlzy1vvWFX76dXXL213u1iqOAD0W5AWoA+YKQELFcD8ifGxgj11z9zd1nL8"
    L"ZtM0hMcXYPHt93LDbSuYWRRGs3qpuWQz0wPRBPoud3Hi0H6+OLiPZDyKpgmjvKrm6JqHnmvIys4ZknDDipv7gb4Vi/xxTU6ImTLN"
    L"UAALLl88V1z/8tYNQ/2REiF0li5fw613rcPrD6BpaSZN0zJs6hSRmWmaModELMrhj+pp+ngPhpGiIBjqXrd5x9tFc668JOGGFDf3"
    L"AX3a/lOxK4BZinsLgcK2luM/+/e//vzbRGykMBgKs/rBbZTMK0doGShNwwad6rLATDMDa5jQfaGN9199lv5IF15/3uCv//CnXeVV"
    L"Nd9LuEHFzb362k1Pz3YpV3j54rmS9/7xxO8TsZHC0ooqfvfEP5l5xWyEBkLT0DUQAoTQEJqWyV1JQ0t3xu5E+m9eQZDrfv5LLpw9"
    L"RV9Pp+fsqaMLKhbd1BrID47J5clKKeGaqXkT42MF9S9v3WDBPfD4i/gCAYTQ0IWGLsF0kQbVBQgNWZZJQpbpskzId617XyDAA4+/"
    L"SGlFFYnYSGH9y1s3TIyPFbh5hJy9fmu921P3zN1D/ZGSYCjM+kd34PF6pSI4FdQUULtxNWk2jNCQ7zjteLxe1j+6g2AozFB/pGRP"
    L"3TN3qyyAXygLsO/Ml01lbS3HbxZCZ/WD2/D5Aw6j6QZxqmmpp7mSqqIFat0rkD5/gNUPbkMInbaW4zef+bKpTGUScufwAd4De9+6"
    L"0zQNsXT5GkrmlaPZhjKGdUVBC2SyeoqKWkZB+16B1DQomVfO0uVrME1DHNj71p0qk5Bbl+eb5s/LejrbKj2+ALfetc7uoZVr1rOi"
    L"oFAgBnp72F23nd112xno7cmo6FLQYUvJb71rHR5fgJ7Otspvmj8vs7iEta8eb9y7BGDx7ffKdU6zlxQxhUFVTSM1xivbt3DkYANH"
    L"DjbwyvYtGKkxh2pT2VDb8PrTGwCAZMkFcm3Ai+e/XQhww20rlEU44wY1tyaIpeYH9a/T1fG9vf51dXzPB/WvO+qIaWypbd1w2woA"
    L"JIsNmN1yrLF0fDTpm1Vcmtm+HAtyxi1u93x3+gSffFg/aZH+5MN6vjt9YsphMpVtDZhZFGZWcSnjo0lfy7HGUiBbAFntrSdLAcoq"
    L"qye9ZPfUvs80mIyP8MbObdOeYt7YuY1kfGQSpMZk9SzgsspqACRTlgD0/khXCKAoPN/e/K0X7W3WBa1p8N6bO+mNXLLr6LqOruv2"
    L"c2/kEu+9udOxJVowGbs42iwKzwdAMukCENHhgUKAYCg8aWN1gCkNNB85wOHGBkfdtRs2sXbDJsdvhxsbaD5yYFo77uNzMBQGQDIJ"
    L"AWjKSXiKc/bka2igj92vveD47aoF17LyvvWsvG89Vy241lG2+7UXGBro+99GZZvKmTIH0AQ/4NpV9zwjw4P2s8fr5bEntyN0HaHr"
    L"PPbkdjxer10+MjzIrrrnf0hTCMDMys4Zs07C6nluquvcma/5qrnJ8dv9Gx9hVlExhgmGCbOKirl/4yOOOl81N3HuzNfTG5ZtWgyS"
    L"yRSAEcifMSgH5iQ40zQz5zkgOyfXUV5dcwvLlq/EMCBlmKQME8OAZctXUl1zi/PbJCfXtpM+J5qTIPsjXQBIJkMAqWAoHAG43HU+"
    L"Y0AacYPOKavgntW1BPILWXjdYmoffgrDhJRp2oApeTCtffgpFl63mEB+IfesrmVOWcUkMLXzpmSQEzYCpLKAibLK6o7mz/5De+vJ"
    L"zDHd1DC1tAHDTM+69L3Jr1Zv5N7fbLTXt5RhOo7+lo28giBb/vIKhplWNaV4w3CctDOfBu2tJ5FrcgcwIYDxqiXLOrJzPfHeSx30"
    L"Xe6a9JKlnqHkhpKnTGzlbCVlx9S6qg23bevjqvdSB9m5nrj8lh4X1gf1nPkLTgOcOLTf0TOrp2pumGDIsZbOJaiRTplypc40ttS2"
    L"ThzaD4BkGQVGbcCaZauOAXxxcB+JWNT+wDFdamVyOe5sMNOVpLJKfbcNtY1ELMoXB/cBIFlswCSQvObGX7TPnlvemoynPw2nc4+l"
    L"iDVbbde6AU1nHVvNaYbJ4Y/qScajzJ5b3iqjD0kgKZQwROKOVbWfapowmj7eQ/eFNodLU5Ybzcz4yoAxTcrMaFXNlMvl3RfaaPp4"
    L"D5omjDtW1X6qMgklkBO/+vql7eVVNUcNI8X7rz5LPBZ1utRwKmiNN8cEUSaKobjfUNVUXB6PRXn/1WcxjBTlVTVHZUjEZhIyPmJF"
    L"mWJrHnquoSAY6u6PdPHuzq0kEwkHpKEoaLjHnFs9dQK5xq9hmiQTCd7duZX+SBcFwVD3moeea1BZgJiQgRs7ypSVnTO0bvOOt73+"
    L"vMGOsy2889IW4tGoUzUjs5Q4VFSSrZ6ZUVBVMx6N8s5LW+g424LXnze4bvOOt2WcxsGjr930dJY7CBnID07Mnltx4eypowv6ejo9"
    L"p5s/o/Sq6wkUBOVCrDl3AHPqZKAsS67Qx+6/baans80OfZRWVHXK2MywGqfR1256OiXBHFHSYFE4UbHoptbvvjp65WBvT96JQ/sZ"
    L"HxuleF4lenZOBsyCnQLYnqUSNB6LcnDfLj5486/Eo0MUBEPdD/zx729MAzcADP74w28/iQDmjz4E/JMIov9Y/w3xX2GkVMihl249"
    L"AAAAAElFTkSuQmCC";
static const WCHAR kChevronDownPressed20B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAD7klEQVR42q2VT2hURxzHPzNv3nNZ7aZxk7DZLJXEJi3FLU0CtRCE"
    L"xRpNKS0iYRWk0JykXgJFBU89eJGaiyCW3nox+OegbZGKmLAguWhNoCvVbnTTyu5mTbJKVl3Wt+/N9PDWYKC9OTDMj+H7+/zmN39+"
    L"I2i2s9cLLcBWoBtIAB31F9W24vxcV7VSjgBEorFqV29/MbQxsgIsAQVgAXh4eHdiFUA0YVHgA+A9oHshO5PM3rwyuLpc6IzGe8Sm"
    L"1g4Anj9dolLKm5b2xGJyx9473cmhbBP4F/Dn4d2Jimiu7ENgm+813s+cnxhZKeX7+nem6RtIoWwHIYIsjAGv4ZKbzTA3fZG2eE8u"
    L"deDINUvZ94G7wB/i7PXCADDge43krz8cS0c2x2Kp/eOEQmEsKZASRJAIBoPW4GtDvV4jc+E01Sfl8hfffH/RUnYWmJXNPevOnJ8Y"
    L"iWyOxXYdPEo4HMZREseWOMoKxtdtJQmHw+w6eJTI5lgsc35i5BVHAomF7ExypZTvS+0fx3EUtiWxlVyDbmj2VzBbSWxL4jiK1P5x"
    L"Vkr5voXsTBJISKAje/PKYP/ONKFQGCUlypLYSqyDroMpgbIkSkpCoTD9O9Nkb14ZBDpk/UW1bXW50Nk3kMKSAssSKEusQR1bskFZ"
    L"bGimuwazAq0lBX0DKVaXC531F9U2WZyf64rGe4SyHaQEKQRSCm5NXebk+CjLxfzaCpeLeU6Oj3Jr6jJSiqYWlO0QjfeI4vxcl6xW"
    L"ypFNrR0IEZymECAFaM+l9myVyTMnEMZDGI/JMyeoPVtFey5SsM5nU2sH1Uo5ovifNjSS5u7tDIuPHvLLuR8BeFx6ROc7WxkaSePp"
    L"//aTkWis+vzpEsYE98wY0AYQkvSh4yjbYfrqJaavXkLZDulDx0FItGGdz/OnS0Sisars6u0vVkp54zVctAZtDFobfG2Idm5heN/Y"
    L"WvThfWNEO7fg60ATaIPXUynlTVdvf1GFNkZWWtoTi7nZTHzbJ3vwfYMnDEJoQLJ9eJTiPw8A2D48SsMzeL7G8w2+HwTOzWZoaU8s"
    L"hjZGVhSwlNyx986t336Kv/vRDmQ4jPABJMZofCn4cuwYAK6n0drg+U2o1tTrNeamL/LxZ1/fAZYkUOhODmXb4j25zIXTuK5Hw9c0"
    L"PI3radyG5mWzu41gruFpGr7GdT0yF07TFu/JNStPQTbLz0LqwJFr1Sfl8o1zp6jVamsw1/OD8XXb09RqNW6cO0X1SbmcOnDk2iuO"
    L"9flX39YAJaUlewc/ffz33ZnW369PRi07REt7AoPEN8Feeb7BdV9y7/YUU5MThN96O7dn7Lufm+XrHnD/zRfYN/0F/AuwAgFzUx/r"
    L"UgAAAABJRU5ErkJggg==";
static const WCHAR kChevronDownPressed30B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAAB4AAAAeCAYAAAA7MK6iAAAG7klEQVR42sWXX2xT1xnAf+dc38RObBInJn/ASUoKWUIYGg0hSAUE"
    L"o4uGstFOdEKjoiDEVAkJraCuD0iVqr1UkyqhCqnqw6ry0NEixAtIgMIoCNJNSRbYUCCBpRY2zv8/TrCJY+x7zx7unziUh75xpc/3"
    L"+Pp83++c73zfdz8L8q7PO+MaEAKqgQqgHAgCJYAfKAK8gJ7NpHUAvdCXBbLAAjAPpIA5IAFMAxPAKDB1pD1sOCyRB9VtYBioygOX"
    L"ZTPp4NCd6/Wxgd7a2cl4+dPZqYBSpgAQQqri0lCydHl4urapNbZ6w46IXuhLADN54DEgDoweaQ9nXbANDQM1wEobXJnNpCt6Lp9u"
    L"idztaniWTnl9gSBVdU2UVIQpXlYOwNMn08xNxBmLDpBOJijw+Rfq1295uGnXwT690DcBjNvgYeAxED/SHs4K270rgVeAWmAFUB1/"
    L"0Ndw6/ypbfPJhL9ubRvrtuxmRf06hBAIwZJLKVBKMRLpp7/rAtH73RQFgqmte47eDP+s5aHt6hEgBjwChsXnnfFK4FWgzt5x+D/X"
    L"z7XevnqmLVBWKbbvPUZVXSNS4EKFAGGfkkLZYAtuKhiLDnLj7EmSM+PqtV/t6/7Fjt/32q5+DESBH7SO/cfXOEAb2tbX+fXmmsaN"
    L"ouOPf6E0VI2mCTQp0aTAIyVSCjQpkLZo7oKsu78kRMPGN5gefSQGu6+EpeZRVaua5wDDlpy0g6gCqIw/6Gu4ffVMW01jK+3vnsDr"
    L"9eGRwhJN4NEkHk2gu2Npjxd/c+Z7vT7a3z1BTWMrt6+eaYs/6GsAKh2etCO3PJtJV9w6f2pboKxS7Nz3AbpHs3bqGFwCkOgege5Z"
    L"/J4/R9MkmibQPRo7931AoKxS3Dp/als2k3YypVzaeVrWc/l0y3wy4d++9xiFXh+aXHSvJiUe+eIFLHkm5RIdTQoKvT627z3GfDLh"
    L"77l8ugUoA4ISKMlm0sHI3a6GurVtViA5ZyexjGiWLLp6Ebg4lu48zdZ17FTVNVK3to3I3a6GbCYdBEok4B+6c73+WTrlXbdlN1KA"
    L"FHmBY8viWS/C86HO79pzgSeFQApYt2U3z9Ip79Cd6/WAXwJFsYHeWl8g6OapCxcC6exaOrsWz7lY2ueaD83Tt6N9Rf06fIEgsYHe"
    L"WqBIAt7ZyXh5VV2Tmw7yOfiTmXFOfXSIz04cYnZy5EdnPDs5wmcnDnHqo0M8mRlfApV5qVZV18TsZLwc8EpAfzo7FSipCLvFAadI"
    L"2HKv9wZTo4+ZHI3x7RefIDDR7bQSmHz7xSdMjsaYGn3Mvd4bS3TzbZVUhHk6OxUAdJnNpHWlTOHUXuHWpMWVNm14HY9eAEB06D7f"
    L"XfzGDaTvLn5DdOg+AB69gKYNr+eVVcuWY7F4WTlKmSKbSeuSn3CFqmvYtfc99/ulc18RfzRE/NEQl8595T7ftfc9QtU1P8UkHr3Q"
    L"lxVCqqdPpoVbexHWSAmUUqAEm3e+ycCd7xm6dxvTMPjy5McAmIb1il3d/Bqbd75JzlAoZdVvy5rzab3JhJBKL/RlJZAtLg0l5ybi"
    L"brHHLfqWmHbxf/vwh3iL/ACMD8cYH44B4C3y8/bhDzHtufm6+bbmJuIUl4aSQFYCC6XLw9Nj0QF3pQ7IuttjU+EvDfHWgfd/5La3"
    L"DryPvzSEab5Az12IYiw6QOny8DSwIIH52qbWWDqZYCTS777aXGUTDFNZYiiaW7fTvHGbC23euI3m1u0YhnLnmeZSuPOuTicT1Da1"
    L"xoB5D5BavWFH5N+df2/r77rgXfnqzxE20BRgoNwGSQFSKX574BhCWnH5m/1/ImeYmCpvgaaydm8uwvu7LlDg8y+s3rAjAqQ8wJxe"
    L"6EvUr9/ycLD7yvqx6CDVrzRiAoYQNs45K4UUAk9BEb87fMLuPhQ54wXeMXHhY9FBove7aWz79UO7H5uTdjc4s2nXwb6iQDB14+xJ"
    L"MgtpW9l07znTJGeoPDFtyXtmmkt0DFORWUhz4+xJigLB1KZdB/vsJjAh7U5wWi/0TWzdc/RmcmZcXTvzKdmcYZ2b8WJgNqfI5l68"
    L"AMMwMQxFNmdw7cynJGfG1dY9R2/azd80MK117D8eAAoAz7LQipyQmhjsvhKeGo5Q27QJqXmsXLRdbUW9018pDEWei8FQyt3pP77+"
    L"K48He2lpf6e7YeMb/7UbvhEgrnXsP64cMCCrVjWnpOZRD3o6V0budolQeA3Fy0KQl2r5CzBVXiTb97HoIJe//Jip+P9US/s7TrPn"
    L"QIeBmNax//iCnRkaIAFVtap5bnl4TeJR/z+r+7+/WDA1EsHrD1Jcuvy5ouLkuAUd/qGff138Gz2XTqN59NQv//Dna8/v1G5xx19e"
    L"Q/9S/8K8jD9t/wem7Aznx0CZogAAAABJRU5ErkJggg==";
static const WCHAR kChevronDownPressed40B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAACgAAAAoCAYAAACM/rhtAAAJmklEQVR42tWZX2wTVxaHv3vHjhM7sUni4NgEMH+WKA00paWFVg1i"
    L"xTZAs1qBSl+oshQJ7QMPqEIrqpW2FQKt1ErtQ3nYPuwu0NKqfQCpUHUXwlJFpFqKYEEpDSkREBZSO04cEpvY+eeZ2QffcWacUK0q"
    L"rdSOdDR2PHPvN79z7rlnTgRzHH9u7y8DqoEqoBIIABXKfECZMg9QArgADRBqCBPQgRwwBUwC48oywENlKWAEeAAM72mpGy9mEUVg"
    L"QkHUAEEFZwH6gXIF6AXK+nuvhgb6umtSyVggmx725aYnXQAutyfn9VdnAsFIqnZJ41DdiicTCi6rAMeAtA1wBEgCQ8DDPS115ixA"
    L"BTcPmK8ALQXnKfMD5bFbXZEbF7+oj92+vnB6MlvC/3C4Pd6pyLJV9x97tvVmZHlTzAY4quwBMKwAB4FRC1IUwdUqwGCRi+cN9HUv"
    L"uNJ+fHXibk/EmjhQU0d46UqqQovwV4fR3HlefXqK9HCcB4l7xO98S2qovwAbijbE1rS0Xatd0vi9ghuxASYV4IAF6VL3VSiw+UWA"
    L"1Yaeq7pw4vDaO12dK0zTECWlPhrWbqb+6RcIBMMgQChHCOUP07QC0QQTUsk4Ny+fo+fSGRJ3eyJ//8sfw0ubmnvXb997SWouewxL"
    L"xWOoGE4LtSAiQBgI2VwcTA/Hw2ePHdyYTsYqpdRY1byVpg0v4fH6kCKPJYQTzjoKkKaJCRimyWQ2Q1fHSa53foZh6PiDkZFNr755"
    L"3l8djtticBBIAHEgprW27au1ubbGssF730XPHDmwKTM65PdXh9my+yC/eHIDJR4PUko0KZBSIIXtbDMhrXP+IYQQuNwlRJY3sajh"
    L"GWK3ukgPx8v6vvkqGoo2jPgCwSm1+k2loAHktNa2fVGba2uAmvRwfMGZIwc2jY+NekPRBn79uz8RqA6hSVEAKwBKgSZsoNIJKcQM"
    L"oBD5oPdWVLJ89S8Z+M8NRgf73fe/u7JwceO6AY+3ImeHA3LSlkrmAZWGnqs6e+zgRgtuy64DlHrLC3BOkzOfNYFLSlxSommPuMZm"
    L"pd5ytuw6QCjawPjYqPfssYMbDT1XZWcBKqXKcQErnVw4cXhtOhmr9FeH2bTzDUpKy5RazChWbJrAZfvuUn8rvq6gsARNCkpKy9i0"
    L"8w381WHSyVjlhROH19rSWgAISLWC/YB/oK97wZ2uzhVSamx85fWCclKCFPbJmANSKaepz7MeZOaefDhQUHLjK68jpcadrs4VA33d"
    L"CyweoMICLAfKr7QfX22ahljVvJVgZAlSoEzMXhSWSppTOVeRsppWFJt2SDV+MLKEVc1bMU1DXGk/vtrisQB9gC92qyuSuNsTKSn1"
    L"0bThJZs7nBPMpZ7LUswG7HqEio7FZLOmDS9RUuojcbcnErvVFbG4pNr0vTcuflEP0LB2Mx6vz7bybE/7A3Ho0ub+PFf8WV6xz+Hx"
    L"5jcAAMXiBcoswLLY7esLAeqffsEhv93NlmnCGew/ZNZDaaI4V84ev/7pFwBQLGUWoKe/92poejJbEqipIxAMqx2CohxW7BbQxGwV"
    L"Z6knnBlASqdyhXmAQDBMoKaO6clsSX/v1RDgkUDJQF93DUB46cr83ioEgqIBbNZx+kPe2rudo+/sJ5MemdPlUgoy6RGOvrOft/Zu"
    L"p+P0h3OOZZ8LoRgAxVQiAVcqGQsAVIUWFTZ+1FM51AQS92/Tcfo42bEUt7qvcvLIu/mFo1lJ2UrUcPLIu9zqvkp2LEXH6eMk7t+e"
    L"NZ6wFX0CQVVoEQCKySUBLZse9gH4q8OFjV/YKkarKBBCoOemHUVBz7WLfP3l5/nYtNKKEHz95ef0XLvouFbPTc+41gE2U2xYDIpJ"
    L"k4CwKmHNXeKsSgqllCgMtHBZA/VN6xwTn/rofUaTiYJ7R5MJTn30vuOa+qZ1LFzWYCvh86611/RCUKgpFZOQ/Ihj2659eCsChe+T"
    L"E+Mcfe8Ahq5j6DpH3zvA5MTM64W3IsC2Xft+zFRIwHS5PTmrEjZNe1Gnik7MwpuQaUJ5oIqtO19zDNTXe4P2U5/QfuoT+npvOH7b"
    L"uvM1ygNVmKY1klXOOv6AaeYZrPcawHQButdfnQFID8cLF5oOSFV4qrcE04SVa9bzVPMm/t15tjDB55/+dZYCTzVvYuWa9eiGOVPA"
    L"qiIWE8eD2xkUky6BXCAYSQE8SNxTitnALGD13TDNgrXu2ENlsLYAY7nYOiqDtbTu2OO4p3g8J6jJg8S9/PtOniknganaJY1DAPE7"
    L"3+bBzLz8lmpzmWGaeEp9bN+9/5Hxs333fjylvjzYI8axz4WpGADFNCWByboVTybcHu9UaqifVDLufErLJaaJYdgNdNNkcf3jPL/5"
    L"5Vlwz29+mcX1j6Ob+Wvt99rHtKuZSsZJDfXj9nin1Lv0pLTe+CPLVt0HuHn5nHIHRTbjJt20QRomG7ftYv6CaAFu/oIoG7ftQjdm"
    L"4HTb/Y8a/+blcwAolnFg3ALMPvZs602AnktnmMxmnMpZA9qU0G0mNDc79h7iiedaeOK5FnbsPYTQ3I5r7MpbQPY5JrMZei6dAUCx"
    L"ZIFxl2pFZCLLm2KhaEMscbcn0tVxkme2/NaWotVZAobIx4zIu0UIMDCpqAzxm1d/X1iVOd0ohIphqIc0lCeM2dbVcZKpiQyhaENM"
    L"dR8yQEaqJs4YMLampe2aENK83vkZyVifQ/6CCrYJdF2ZUimnrKCc+t0oulc3nG5Oxvq43vkZQkhzTUvbNYsHeGgBpoF07ZLG75c2"
    L"Nfcahs75j99mIjs2E0em3a043JeHMWaAdWP277Z7LDV1w2QiO8b5j9/GMHSWNjX3qpZIWtlDqTpMKauRs3773kv+YGQkPRzn7AeH"
    L"mJoYdwS7MWviPJRduZzhVHauONQNk6mJcc5+cIj0cBx/MDKyfvveS7aGUgpIaa1t+wKAW5kmpNQW1j813PfNV9HRwX53/G430cZn"
    L"ybdQZjK/bbOayY2oZGxYuZKiFWu5GiayY5w5dpDE3R7KyudlX9x9qL3UF0ioFojV6RrSWtv2FeCsBo7HW2GGog0j97+7snB0sN/d"
    L"9+2/CEUbKSufVwRogc2VyME05gZMxvr4x9/e5EG8j7Lyedlftf3hfFV4Sb+Cs9ug1tq2b9oOp0z4AsGpxY3rBvp7r4bTw/Gy3svn"
    L"yE1NUr1gOdLlduzZjrMFaNhXcB5uIpvh6j8/5cKJw0xk0/iDkZEXdx9qnwPOaiINWv1Bv+pu1f6/229TExmEkKat/Wb1Bov7g/E9"
    L"LXXpn3wD8+fRAv5ZNNF/qv+G+C/HUC3Dr/Y+NwAAAABJRU5ErkJggg==";
static const WCHAR kChevronUpPressed20B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAD4UlEQVR42q2VT2jURxTHPzO/+W2XVXcbN4nr7iJkS9JSuoUk0IsI"
    L"SxQVpGILRjH00JNUBKFo6UV6FDSXXGw95aToIqL2EoyRBUlRWhNwpdoVkyqb7DbJKll02fz+zPSwv0Rz8ObAY4bh+z7Me2/mjSAY"
    L"52+VY8AnQBeQBjqbb+rtc0+nU/VaNQoQjSfqqe7eufCG6BKwAJSBWeDZsd3pZQARwOLA58CnQNdscTJbvHu9f3mxvDWezIiNbZ0A"
    L"vH61QG1+xsQ60pXsjgMPurLbiwHwH+DvY7vTNRGc7EvgC99zPytcHt67ND/T0zswSE9fDmWHEKIVhTHguQ6lqQLTd/K0JzOl3OGT"
    L"Y5aynwCPgIfi/K1yH9Dne272919/GoxuTiRyh04QDkewpEBKEK1AMBi0Bl8bms0GhSsj1F9Wq1//cDZvKbsITMkgZ12Fy8N7o5sT"
    L"iV1Dp4hEIoSUJGRLQspqze+ulSQSibBr6BTRzYlE4fLw3lWOBNKzxcns0vxMT+7QCUIhhW1JbCXXoB8FtgqzlcS2JKGQInfoBEvz"
    L"Mz2zxckskJZAZ/Hu9f7egUHC4QhKSpQlsZVYg94cPcvN0bNvYUqgLImSknA4Qu/AIMW71/uBTtl8U29fXixv7enLYUmBZQmUJdag"
    L"98ev8vDeBA/vTXB//OpbmNXSWlLQ05djebG8tfmm3i7nnk6n4smMUHYIKUEKgZQtYa3ynPFro6tXlfFro9Qqz4NiiUALyg4RT2bE"
    L"3NPplKzXqtGNbZ0I0aqmECAFYDT5C2fwXIeBfQcZ2HcQz3XIXzgDRiMF63w2tnVSr1WjkveMybE8lRfP2JLcxv6ho+wfOsqW5DYq"
    L"L54xOZZ/nxsyGk/UX79awJjWPTMGtAGpQkQ2xThy/DRGKIxQHDl+msimGFKF0IZ1Pq9fLRCNJ+oq1d0798eN34znOsK2wmhj0Nrw"
    L"1c5v2L7nW5QlcVwNQEcqw88jV3E9g+vpQNt6PbX5GZPq7p2T4Q3RpVhHulKaKuBrg+8bPN/g+RrXMziuZsXzWfF8HLe15/kaz29p"
    L"fW0oTRWIdaQr4Q3RJQksZHcceDB9J0+z2cDTeg3mehrH0zhuYJ7G9d6Bak2z2WD6Tp7sjgMPgAUJlLuy24vtyUypcGUEx/Fwfb0O"
    L"thLYOqivcRyPwpUR2pOZUtB5yjJoP7O5wyfH6i+r1dsXz9FoNN6eLAh13drTNBoNbl88R/1ltZo7fHJslWPt++7HBqCktGR3/87/"
    L"/n002fbXrUtxyw4T60hjkPimlSvPNzjOCo//nGDi0jCRTR+X9nz/y42gfT0Gnnz4Bvuhv4D/AT/eIrjXCpMLAAAAAElFTkSuQmCC";
static const WCHAR kChevronUpPressed30B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAAB4AAAAeCAYAAAA7MK6iAAAG4ElEQVR42sWXX2hUZxbAf993Z8xMMpNkksk/ncxoNmaNiqwbY/Yh"
    L"ito2YKW+iFgtFktZKIIsSvehfZJdELYULAiFglsUiiBbaauwKXbFoHG3SRrdlcRE1w5OHDXGGSdxJpmZ3Ln324e592aS9aFvXjjc"
    L"v+f8zvnuOeeeKyjZPr8c14Ag0ATUA7VAAKgCfEA54AHcej7rBnCXeXVAB3LAHJABZoAUkASmgCdA4nBPyLBZogTqtoAhoLEEXKPn"
    L"s4H7t662TIwNhaefxWtnpxN+pUwBIIRUFdXBdHVdKBlu75xo3bg96i7zpoDnJeBJIA48OdwT0h2wBQ0BzcAKC9yg57P1g71nOqK3"
    L"+9vmsxmP1x+gMdJOVX2IispaAGZfJJmZijMZGyObTrHM68u1bOi+t3nnoWF3mXcKeGqBHwEPgfjhnpAurOVdAawEwsByoCl+d7jt"
    L"+oVTW+fSKV9kbRfru3ezvGU9QgiEYNGmFCileBwdYaT/IrE7A5T7A5kte45cC/2645611I+BCeAB8Eh8fjneAPwKiFgRh/599W+d"
    L"N3841+WvaRDb9h2lMbIGKXCgQoCw3pJCWeAi3FQwGRun7/xJ0s+fqt++cWDgN9v3DllL/RCIAT9ruw4eW20DLWjX8OWvfte8ZpPY"
    L"9fs/UR1sQtMEmpRoUuCSEikFmhRISzTHoeLeVxWkbdPrJJ88EOMD34ek5lKNq9bNAIYlBWklUT3QEL873Hbzh3NdzWs66Xn3Yzwe"
    L"Ly4piqIJXJrEpQnczrG0jhfu2c97PF563v2Y5jWd3PzhXFf87nAb0GDzpJW5tXo+W3/9wqmt/poG8dqBD3G7tGKktsFFAInbJXC7"
    L"Fs5Ln9E0iaYJ3C6N1w58iL+mQVy/cGqrns/alVIrrTqtGew90zGXTvm27TtKmceLJheWV5MSl3y5A4uuSblIR5OCMo+XbfuOMpdO"
    L"+QZ7z3QANUBAAlV6PhuI3u5vi6ztKiaS/e4kRSNaUWxQYX6Ob06f4JvTJyjMzzmO2M9plq5tpzGyhsjaLqK3+9v0fDYAVEnAd//W"
    L"1Zb5bMazvns3UoAUJYljycK7llw6e5KRwT5GBvu4dPZk0SHrvrYk8aQQSAHru3czn8147t+62gL4JFA+MTYU9voDTp06cCGQdtRW"
    L"5KNDfYz+dM2p4dGfrjE61FcSaVHH0beyfXnLerz+ABNjQ2GgXAKe6Wfx2sZIu1MOcilcFJctM53g27OfsXT79uxnZKYTVoRL9EpK"
    L"rTHSzvSzeC3gkYB7djrhr6oPOc0Bu0lYYhv5+vQn5OYyADSsCNOwIgxAbi7D16c/WQL6f1tV9SFmpxN+wC31fNatlCns3iucnrTg"
    L"KQJ+vPId90dvAiA1jfePHuf9o8eRmgbA/dGb/HjlOwtkt9WiLdtiRWUtSplCz2fdkl+wJZ48pPf8F875m3vfI7SyldDKVt7c+55z"
    L"vff8FySePPwlJpHuMq8uhFSzL5ILvdc+svrv2K0bFPR5ACKta9nx1n4MQ2EYih1v7SfSuhaAgj7P2K0bKFXUxbJlW5x9kUQIqdxl"
    L"Xl0CekV1MD0zFXeaPU7TL8q6zm0Em5qpawrz9gcfoZDohkI3FArJ2x98RF1TmGBTM+s6ty3SLbU1MxWnojqYBnQXkKuuCyUnY2OV"
    L"RU8FplKYzh4qaxo48ucvnZIpGAohlPNJrK5bzh9OfIlhKAqmomCYjq6pFlZuMjZGbdOqJJCTwFy4vXMim07xODrifNqKigrTBMNU"
    L"RTEUBaNouLDk2DCU85xpluiXfKuz6RTh9s4JYE4CmdaN26PLvL7cSP/FBahZFNtYwVyIpmAodMNEdxwwnfsLcEss+Ej/RZZ5fbnW"
    L"jdujQEYCM+4yb6plQ/e92J0BJmPjJVCcSA1jMdSOuBRuLIocx85kbJzYnQFaNnTfs+axGWlNg8837zw0XO4PZPrOnySfy1rKprMv"
    L"mKXLazux5JppLtIxTEU+l6Xv/EnK/YHM5p2Hhq0hMCWtSTDpLvNObdlz5Fr6+VN15dyn6AXDiuDlQL2g0Asvd8CwotcLBlfOfUr6"
    L"+VO1Zc+Ra9bwlwSS2q6Dx/zAMsBVGVxeEFIT4wPfhxKPooTbNyM1V7EWrSRRCidhTKUwFCUJCIZSTqT/+OovPBwfoqPnnYG2Ta//"
    L"xxr4HgNxbdfBY8oGA7Jx1bqM1Fzq7uDlFdHb/SIYWk1FZRAcqFrkgKlKMtnaT8bG6f3rcRLx/6qOnnfsYc+GPgImtF0Hj+WsLqYB"
    L"ElCNq9bN1IVWpx6M/LNp5MalZYnHUTy+ABXVdU4zKC0XG/ro5xH+dek0g38/g+ZyZ3bs/+OVpZFaI+7TVzfQv9JfmFfx0/Y/b8BU"
    L"n5L50OEAAAAASUVORK5CYII=";
static const WCHAR kChevronUpPressed40B64[] =
    L"iVBORw0KGgoAAAANSUhEUgAAACgAAAAoCAYAAACM/rhtAAAJlklEQVR42tWZXWxUZRrHf+85005nWmb6MXU6Q8EWWEippIHVBS4w"
    L"rKxF7SIkeKHR6mLIJnqhCW5iNlkMgeyFxhjDhTfrCsRINhEDMWG3LUtiKFEJiJYPaxugFcpMP6YfM+1M2+mc8+7FvOf0nGm72Zhs"
    L"oid5MuecnvO8v/f/vB/PeSpY5PigY8AHVAGVQAUQBJYpKwV8yrxAMeABdEAoFxIwgByQBWaBaWVpYFJZEhgHxoDRV5trpwtZRAGY"
    L"UBDVQEjBWYABoEwB+gHfQO/V8GDfzepkIhbMpEZLc3OzHgBPkTfnD1Slg6Fosqa+caR27aYhBZdRgFNAygE4DiSAEWDy1eZauQBQ"
    L"wZUDDyhAS8FyZQGgLHarK/r9V2fXxW5fXzE3mynmfziKvP5sdPWGe+u3tvRE1zTFHIATysaAUQU4DExYkKIArkYBhgpCXD7Yd3P5"
    L"lY6PNw71d0ethoPVtURWPURleCWBqgh6UZ7XmMuSGo0zNnSX+J0bJEcGbNhwXUPs4ebWb2vqG+8ruHEHYEIBDlqQHvXeMgX2QAFg"
    L"lWnkKi+cOrr5TlfnWilNUVxSSsPmJ1j3yOMEQxEQIFQghIqHlNZAlCAhmYjTc/kc3ZfaGOrvjv7zb3+JrGra1vvoM69d0nSPcwxr"
    L"isdUYzgl1ISIAhEg7AhxKDUaj7QfP7wjlYhVaJrOhm17aNq+F6+/FE3ksYRww1mHDSklEjClZDaTpuuLz7jeeQbTNAiEouM7//DW"
    L"+UBVJO4Yg8PAEBAHYnpL64EaR2irLRu++0Nd20eHdqYnRgKBqghP7j/MrzZtp9jrRdM0dE2gaQJNOH4dJjTrN98JIQSeomKia5pY"
    L"2fAbYre6SI3GfX3XLtaF6xrGS4OhrJr9UiloAjm9pfVAnSO01UB1ajS+vO2jQzunpyb84boGfv/HvxKsCqNrwgazATWBLhygmhtS"
    L"iHlAIfKD3r+sgjUbf8vgj98zMTxQdO+HKysebNwy6PUvyznhgJzmWErKgQrTyFW2Hz+8w4J7ct8hSvxlNpzbtPlzXeDRNDyahq4v"
    L"8YzDSvxlPLnvEOG6BqanJvztxw/vMI1cpZMFqNDUGhe0lpMLp45uTiViFYGqCDtfOkhxiU+pxbxihaYLPI5rj7pX+JytsAa6Jigu"
    L"8bHzpYMEqiKkErGKC6eObnYsa0EgqKkZHAACg303l9/p6lyraTo7nn/TVk7TQBPOxlgEUimnq/MFHZl/Jz8csJXc8fybaJrOna7O"
    L"tYN9N5dbPMAyC7AMKLvS8fFGKU2xYdseQtF6NIEysXBSWCrpbuU8BcrqesHYdEIq/6FoPRu27UFKU1zp+HijxWMBlgKlsVtd0aH+"
    L"7mhxSSlN2/c6wuFuYDH1PLrG5PgQnx9/l8+Pv8vk+BCeJVR0TSaHNW3fS3FJKUP93dHYra6oxaWpTd///Vdn1wE0bH4Cr7/UMfMc"
    L"vV1iHEpjjpNHD/Ldlx1892UHJ48eRBpzS44/KyrONrz+/AYAoFj8gM8C9MVuX18BsO6Rx13yO8NsmS7cg/386WMM3++3F+nh+/2c"
    L"P33MNbl0UbhWLvS/7pHHAVAsPgvQO9B7NTw3mykOVtcSDEXUDkHBGlYYFtCF4Meea1xs+3RBgnCx7VN+7LmmOoPrXeFSD3t9DIYi"
    L"BKtrmZvNFA/0Xg0DXg0oHuy7WQ0QWfVQfm8VAkGBgwLThGB2Js2pD99ZMos59eE7zM6kVThZwubbQigGQDEVa4AnmYgFASrDK+2N"
    L"H9Url5q4Q3325AeMJwZtIE3X0XTdvh5PDHL25AfuLbDAn3AkfQJBZXglAIrJowF6JjVaChCoitgbv3BkjFZS4EwMbly5wDed7S7F"
    L"dj27n13P7nfd+6aznRtXLtjJhB1aF9h8smExKCZdA4SVCetFxe6sxE6lhMvRVHKMMyfed4HUr11P8+7naN79HPVr17v+dubE+0wl"
    L"x9wdzwfWldMLgZ1TKiah8ROO08feIzOZtK+9JT72vX7IDvG+1w/hLfHZf89MJjl97L2f0hQaID1F3pyVCUvpTOpU0om0v4Tu3e6m"
    L"p+trl5PdL7xCeSiMYUoMU1IeCrP7hVdcz/R0fc29291Ih3MroXXmkMZcFuu7BpAaYPgDVWmA1GjcflC6IFXiKSW6p8jVcMPGrWx5"
    L"bBemlBhG3kwp2fLYLho2bnU9q3uKbD8Su9/Kf/7cYlBMhgbkgqFoEmBs6K5SzAFmAavr8IrVbH+6FX9ZkDWNm9j78hsYJnk408yb"
    L"ITFM2PvyG6xp3IS/LMj2p1sJr1i9wJ8bVDI2dDf/vZNnynmAbE1940jXF6eI37mRB1M9zDsS8w6VbX/6RR7b86K9o5imRAr3N4mU"
    L"UBqoYN+f3sGQEtMEw5QLfOXbyquKJM8A1NQ3jgBZDZitXbtpqMjrzyZHBkgm4u5eWiGREtN0GhhS2uPOMCU5I2/Oexac812nT6ea"
    L"yUSc5MgARV5/Vn1Lz2rWF3909YZ7AD2Xz2FKiSkpMGmbIR2QpvyvZsEZjveX8t9z+RwAimUamLYAM+u3tvQAdF9qYzaTditnOXQo"
    L"UQjiVG6BigXKW0DONmYzabovtQGgWDIWYBpIR9c0xcJ1DbHsTP7T0B1OZTI/+BfCmflzNYtd91ymIrGI764vPiM7kyZc1xBT1Yc0"
    L"kNZUEWcKmHq4ufVbITR5vfMMiVifS37DAWmr6AAyTElOmQ1kLTsF7xqmO8yJWB/XO88ghCYfbm791uIBJi3AFJCqqW+8v6ppW69p"
    L"Gpz/5G1mMlPz40i6lVigjmHOAy+hnuGEU9czmSnOf/I2pmmwqmlbryqJpJRNaqrClLQKOY8+89qlQCg6nhqN037iCNmZaddgX2z8"
    L"GYZbuZzpVnaxcWiYkuzMNO0njpAajRMIRccffea1S46CUhJI6i2tB4JAkTJdaJq+Yt2vR/uuXaybGB4oivffpK5xK/kSyvzK79is"
    L"7KXCJK+ONNW1Pfsd52b+eiYzRdvxwwz1d+MrK888tf9IR0lpcEiVQKxK14je0nrAhrMKOF7/Mhmuaxi/98OVFRPDA0V9N74kXNeI"
    L"r6y8ANACY8ECbEqQ5uKAiVgf//r7W4zF+/CVlWd+1/rn85WR+gEF57RhvaX1wJwTTpkoDYayDzZuGRzovRpJjcZ9vZfPkcvOUrV8"
    L"DZraj+3tShZsiWpJyv9KG24mk+bqv//BhVNHmcmkCISi40/tP9KxCJxVRBq26oMBVd2q+X+X37IzaYTQpKP8ZtUGC+uD8Veba1M/"
    L"+wLmL6ME/Isoov9c/w3xHwBpmzD4xBhWAAAAAElFTkSuQmCC";

struct ChevronArtTier {
    const WCHAR* art[CHEVRON_STATE_COUNT];   // down, up, hover down, hover up, pressed down, pressed up
};

static const ChevronArtTier kChevronArtTiers[CHEVRON_TIER_COUNT] = {
    { { kChevronDownNormal20B64, kChevronUpNormal20B64, kChevronDownHover20B64, kChevronUpHover20B64, kChevronDownPressed20B64, kChevronUpPressed20B64 } },
    { { kChevronDownNormal30B64, kChevronUpNormal30B64, kChevronDownHover30B64, kChevronUpHover30B64, kChevronDownPressed30B64, kChevronUpPressed30B64 } },
    { { kChevronDownNormal40B64, kChevronUpNormal40B64, kChevronDownHover40B64, kChevronUpHover40B64, kChevronDownPressed40B64, kChevronUpPressed40B64 } }
};

// The pixel size of each embedded tier, in the order of the table above. Declared here, before the
// first function that reads it: an identifier has to be declared before it is used.
static const int kChevronTierPixels[CHEVRON_TIER_COUNT] = {20, 30, 40};

// The base64 of one state turned into the icon it carries. The decode is the same one the shield
// uses: the PNG bytes go straight to CreateIconFromResourceEx.
static HICON LoadChevronArt(const WCHAR* b64) {
    std::vector<BYTE> png;
    if (!Base64Decode(b64, png)) return nullptr;
    return IconFromPngBytes(png);
}

// The six states of one size are decoded together, the first time that size is asked for, and kept
// for the dialog: a repaint then costs one DrawIconEx. A tier that cannot be decoded is remembered
// as tried, so the log says so once and the row uses the fallbacks instead of retrying for ever.
static void EnsureChevronTier(int tier) {
    if (tier < 0 || tier >= CHEVRON_TIER_COUNT || g_ui.chevTried[tier]) return;
    g_ui.chevTried[tier] = true;
    int done = 0;
    for (int i = 0; i < CHEVRON_STATE_COUNT; i++) {
        g_ui.chevArt[tier][i] = LoadChevronArt(kChevronArtTiers[tier].art[i]);
        if (g_ui.chevArt[tier][i]) done++;
    }
    if (done == CHEVRON_STATE_COUNT)
        Dbg(L"expander: the Windows 7 art is ready (%d px tier)", kChevronTierPixels[tier]);
    else
        Log(L"expander: %d of the %d embedded Windows 7 states could not be decoded - the expander "
            L"uses what the theme manager gives", done, CHEVRON_STATE_COUNT);
}

// The art of the state the row is in, or nullptr if nothing of that tier could be decoded. The
// closest tier is used, so the button is drawn 1:1 at 100%, 150% and 200% and only lightly scaled
// in between - always downwards from the tier above, which is the way a bitmap keeps an edge.
static HICON ChevronArtIcon(int slot, BOOL expanded, BOOL hover, BOOL pressed) {
    int tier = (slot <= 24) ? 0 : (slot <= 34 ? 1 : 2);
    EnsureChevronTier(tier);
    int idx = pressed ? (expanded ? 5 : 4) : (hover ? (expanded ? 3 : 2) : (expanded ? 1 : 0));
    HICON h = g_ui.chevArt[tier][idx];
    if (!h) h = g_ui.chevArt[tier][expanded ? 1 : 0];   // a missing state falls back to normal
    return h;
}

// The same six states of one tier through GDI+, decoded the first time the tier is asked for. It
// is the art itself, with its alpha channel, and it is what the row is drawn from whenever the size
// the row asks for is not the size of the art: GDI+ resamples it with a real filter, where the
// Win32 path would leave the edges of the disc and the V stepped.
static void EnsureChevronTierGdiplus(int tier) {
    if (tier < 0 || tier >= CHEVRON_TIER_COUNT || g_ui.chevBmpTried[tier]) return;
    g_ui.chevBmpTried[tier] = true;
    if (!GdiplusStart()) return;
    int done = 0;
    for (int i = 0; i < CHEVRON_STATE_COUNT; i++) {
        std::vector<BYTE> png;
        if (!Base64Decode(kChevronArtTiers[tier].art[i], png)) continue;
        g_ui.chevBmp[tier][i] = GdiplusBitmapFromPng(png, &g_ui.chevStream[tier][i]);
        if (g_ui.chevBmp[tier][i]) done++;
    }
    if (done == CHEVRON_STATE_COUNT)
        Dbg(L"expander: the Windows 7 art is decoded through GDI+ too (%d px tier)",
            kChevronTierPixels[tier]);
    else
        Dbg(L"expander: GDI+ decoded %d of the %d states of the %d px tier", done,
            CHEVRON_STATE_COUNT, kChevronTierPixels[tier]);
}

// The art of the state the row is in as a GDI+ bitmap, or nullptr. Same choice of tier and state as
// the icon path above, so the two can never disagree about which picture belongs to which state.
static GpBitmap* ChevronArtBitmap(int slot, BOOL expanded, BOOL hover, BOOL pressed) {
    int tier = (slot <= 24) ? 0 : (slot <= 34 ? 1 : 2);
    EnsureChevronTierGdiplus(tier);
    int idx = pressed ? (expanded ? 5 : 4) : (hover ? (expanded ? 3 : 2) : (expanded ? 1 : 0));
    GpBitmap* b = g_ui.chevBmp[tier][idx];
    if (!b) b = g_ui.chevBmp[tier][expanded ? 1 : 0];
    return b;
}

// The fallback glyph: a small circle with a chevron, drawn from GDI primitives. It is used only
// where neither the embedded Windows 7 art nor the theme manager's own part can be used (visual
// styles off, classic theme, where the rest of the dialog is classic too).
static void DrawChevronFallback(HDC dc, int x, int y, int size, BOOL expanded) {
    HBRUSH b = CreateSolidBrush(RGB(238, 242, 247));
    HBRUSH ob = (HBRUSH)SelectObject(dc, b);
    HPEN p = CreatePen(PS_SOLID, 1, RGB(140, 160, 185));
    HPEN op = (HPEN)SelectObject(dc, p);
    Ellipse(dc, x, y, x + size, y + size);
    SelectObject(dc, ob);
    SelectObject(dc, op);
    DeleteObject(b);
    DeleteObject(p);

    POINT tri[3];
    int cx = x + size / 2, cy = y + size / 2;
    int s = size / 4;
    if (expanded) {
        tri[0] = {cx - s, cy - s / 2};
        tri[1] = {cx + s, cy - s / 2};
        tri[2] = {cx, cy + s};
    } else {
        tri[0] = {cx - s / 2, cy - s};
        tri[1] = {cx - s / 2, cy + s};
        tri[2] = {cx + s, cy};
    }
    HBRUSH tb = CreateSolidBrush(RGB(70, 90, 115));
    HGDIOBJ otb = SelectObject(dc, tb);
    HPEN tp = CreatePen(PS_SOLID, 1, RGB(70, 90, 115));
    HGDIOBJ otp = SelectObject(dc, tp);
    Polygon(dc, tri, 3);
    SelectObject(dc, otb);
    SelectObject(dc, otp);
    DeleteObject(tb);
    DeleteObject(tp);
}

// The fallback expander of the "Show details" row: the task dialog expando of the theme manager
// (class TASKDIALOG, part TDLG_EXPANDOBUTTON), for a system whose theme carries no art the
// embedded Windows 7 button can be built from. Its handle is opened once for the dialog and
// closed with it, like every other resource of the session.
static HTHEME ExpandoTheme() {
    if (!g_ui.hExpando && !g_ui.expandoTried && g_ui.hwnd) {
        g_ui.hExpando = OpenThemeData(g_ui.hwnd, L"TASKDIALOG");
        g_ui.expandoTried = true;
        if (!g_ui.hExpando)
            Dbg(L"expander: the theme manager has no TASKDIALOG part - the small chevron is used");
    }
    return g_ui.hExpando;
}

// Draws the expander in the state the row is in, centered in a slot x slot box at (x, y) and never
// larger than the slot, so the glyph can never push the label away from it. The Windows 7 art of
// the reference comes first, drawn 1:1 with DrawIconEx. The theme manager's own task dialog part
// is the fallback of a system whose theme has no expander art: it is asked for the size it draws
// it at (TS_DRAW), the documented way, and drawn with DrawThemeBackground. The small chevron
// above stands in when even that fails.
static void DrawExpanderGlyph(HDC dc, int x, int y, int slot, BOOL expanded, BOOL hover,
                              BOOL pressed) {
    // The art at the size it has costs nothing and stays pixel for pixel as it was drawn; at any
    // other size GDI+ resamples it, which is the difference between a round button and a stepped
    // one on a 125% display, where the row asks for 25 px of a 30 px picture.
    int artPixels = kChevronTierPixels[(slot <= 24) ? 0 : (slot <= 34 ? 1 : 2)];
    if (slot != artPixels) {
        GpBitmap* bmp = ChevronArtBitmap(slot, expanded, hover, pressed);
        if (bmp && GdiplusDrawBitmap(dc, bmp, x, y, slot, slot)) return;
    }
    HICON art = ChevronArtIcon(slot, expanded, hover, pressed);
    if (art && DrawIconEx(dc, x, y, art, slot, slot, 0, nullptr, DI_NORMAL))
        return;

    int state = expanded ? (pressed ? TDLGEBS_EXPANDEDPRESSED : TDLGEBS_EXPANDEDNORMAL)
                         : (pressed ? TDLGEBS_PRESSED : TDLGEBS_NORMAL);
    HTHEME th = ExpandoTheme();
    if (th) {
        SIZE s = {slot, slot};
        SIZE natural = {0, 0};
        if (SUCCEEDED(GetThemePartSize(th, dc, TDLG_EXPANDOBUTTON, state, nullptr, TS_DRAW,
                                       &natural)) &&
            natural.cx > 0 && natural.cy > 0) {
            s = natural;
        }
        if (s.cx > slot) s.cx = slot;
        if (s.cy > slot) s.cy = slot;
        int gx = x + (slot - s.cx) / 2, gy = y + (slot - s.cy) / 2;
        RECT part = {gx, gy, gx + s.cx, gy + s.cy};
        if (SUCCEEDED(DrawThemeBackground(th, dc, TDLG_EXPANDOBUTTON, state, &part, nullptr)))
            return;
    }
    DrawChevronFallback(dc, x, y, slot, expanded);
}

// The native title bar is the system's: the only documented way to colour it is the DWM
// attribute available from Windows 11 22H2 on. Where it does not exist the call fails and the
// system colour stays - nothing is faked by painting the non-client area by hand.
// The frame around the dialog. By default it is the one this Windows draws, which on Windows 10
// and 11 is the DWM frame. The "basic" choice asks DWM to stop rendering the non-client area for
// this one window, and the system falls back to the flat Windows 7 basic frame it still carries.
// Both halves are public and documented: DwmSetWindowAttribute with DWMWA_NCRENDERING_POLICY and
// the DWMNCRENDERINGPOLICY values. Nothing here touches DWM for any other window, and the call is
// skipped silently when the attribute is not accepted, so an older build simply keeps its frame.
static void ApplyWindowTheme(HWND hwnd) {
    // Only the Windows 7 (Basic Theme) skin requests the documented per-window Basic frame.
    if (g_settings.windowTheme != WindowThemeWin7Basic) return;
    HMODULE dwm = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dwm) return;
    typedef HRESULT (WINAPI *DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
    auto set = (DwmSetWindowAttribute_t)GetProcAddress(dwm, "DwmSetWindowAttribute");
    if (!set) { FreeLibrary(dwm); return; }
    const DWORD attrNcPolicy = 2;   // DWMWA_NCRENDERING_POLICY
    DWORD policy = 1;               // DWMNCRP_DISABLED -> the flat Windows 7 basic frame
    HRESULT hr = set(hwnd, attrNcPolicy, &policy, sizeof(policy));
    if (SUCCEEDED(hr)) {
        Log(L"window frame: the Windows 7 basic frame was applied to this dialog");
    } else {
        Dbg(L"window frame: this build did not accept the basic frame (hr=0x%08X) - the native "
            L"frame is used", (unsigned)hr);
    }
    FreeLibrary(dwm);
}

static void ApplyCaptionColour(HWND hwnd) {
    // Basic and the additional custom Aero theme keep the Windows 7 light-blue title-bar tint.
    if (g_settings.windowTheme != WindowThemeWin7Basic &&
        g_settings.windowTheme != WindowThemeWin7Aero)
        return;
    HMODULE dwm = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dwm) return;
    typedef HRESULT (WINAPI *DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
    auto set = (DwmSetWindowAttribute_t)GetProcAddress(dwm, "DwmSetWindowAttribute");
    if (!set) { FreeLibrary(dwm); return; }
    const DWORD attrCaption = 35;      // DWMWA_CAPTION_COLOR
    const DWORD attrText = 36;         // DWMWA_TEXT_COLOR
    COLORREF cap = RGB(209, 231, 246); // fixed Windows 7 light blue
    COLORREF ink = RGB(20, 30, 45);
    HRESULT hr = E_NOTIMPL;
    if (set) hr = set(hwnd, attrCaption, &cap, sizeof(cap));
    if (set && SUCCEEDED(hr)) {
        set(hwnd, attrText, &ink, sizeof(ink));
        Log(L"caption colour: fixed light blue applied to the native title bar");
    } else {
        // Windows 10 and earlier answer E_INVALIDARG here: the attribute exists from Windows 11
        // 22H2 on. There is no documented way to colour a caption on the older builds, and the
        // mod does not paint the non-client area by hand, so the system colour simply stays.
        Dbg(L"caption colour: this build does not accept a caption colour (hr=0x%08X) - the "
            L"system title bar colour is used", (unsigned)hr);
    }
    FreeLibrary(dwm);
}

// Registered from the thread that owns the Windows 7 dialog, so its message loop receives
// WM_HOTKEY; a chord that cannot be registered is logged and simply does not exist.
static void RegisterPanicHotkey() {
    if (!g_settings.panicVk) {
        if (g_settings.panicHotkey[0])
            Log(L"panic hotkey '%s': not understood, it stays disabled",
                g_settings.panicHotkey);
        return;
    }
    if (RegisterHotKey(nullptr, IDC_W7UAC_PANIC, g_settings.panicMods | MOD_NOREPEAT,
                       g_settings.panicVk)) {
        g_ui.hotkeyId = IDC_W7UAC_PANIC;
        Log(L"panic hotkey '%s' registered", g_settings.panicHotkey);
    } else {
        Log(L"panic hotkey '%s' could not be registered (error %u) - it stays disabled",
            g_settings.panicHotkey, GetLastError());
    }
}

// The contents of the "Show details" pane: what the original screen shows once its own details
// are expanded, then the path of the elevated file, and never an empty pane. Nothing here is
// invented - every row comes from the prompt's own text (read with UI Automation) or from the
// file the prompt names.
static void AddDetailRow(const std::wstring& row) {
    if (row.size() < 2 || row.size() > 300) return;
    if (IsJunkText(row)) return;
    if (std::find(g_ui.details.begin(), g_ui.details.end(), row) != g_ui.details.end()) return;
    if ((int)g_ui.details.size() >= DETAILS_MAX_ROWS) return;
    g_ui.details.push_back(row);
}

// Declared here because the pane needs it to drop the label of the details control itself (the
// definition, with the word list it matches against, is further down with the harvest code).
static bool NameIsDetails(const std::wstring& name);

static void BuildDetailsPane(const PromptInfo& info) {
    g_ui.details.clear();
    if (!g_loc) return;

    // The three rows the recreated screen already shows, repeated here the way the original screen
    // repeats them when it unfolds. They carry the prompt's own values, so the pane is never empty
    // and never a hole - even when the prompt publishes nothing else at all.
    if (!info.appText.empty())
        AddDetailRow(std::wstring(g_loc->programName) + L" " + info.appText);
    if (!info.publisherText.empty())
        AddDetailRow(std::wstring(PublisherLabelFor(info.publisherVerifiedByWindows)) + L" " + info.publisherText);
    if (!info.originText.empty())
        AddDetailRow(std::wstring(g_loc->origin) + L" " + info.originText);

    // Everything the original screen showed when it was unfolded, then the paths the prompt
    // published, then the file that is being elevated.
    for (const auto& t : info.detailTexts) {
        std::wstring row = Trim(t);
        if (row == info.appText || row == info.publisherText || row == info.questionText) continue;
        if (info.publisherFromSignature) {
            std::wstring detailLabel, detailValue;
            SplitLabelValue(row, detailLabel, detailValue);
            if (IsUnknownPublisherValue(detailValue) &&
                (LooksLikePublisher(detailLabel) || LooksLikePublisher(row) ||
                 IsUnknownPublisherValue(row)))
                continue;   // the verified signer has replaced the native unknown-publisher row
        }
        // The window title and the labels of the prompt's own controls are its chrome, not
        // information about the request: the pane carries what the screen carries, not its
        // buttons.
        if (!info.windowTitle.empty() && row == info.windowTitle) continue;
        bool chrome = false;
        for (const auto& e : info.elements) {
            if (e.name.empty() || e.name != row) continue;
            if (e.type == UIA_ButtonControlTypeId || e.type == UIA_HyperlinkControlTypeId) {
                chrome = true;
                break;
            }
        }
        if (chrome) continue;
        // The labels of the two link rows are commands of the prompt's chrome, not facts about
        // the request, and so is the label of the details control itself: the pane dropped them
        // even when the prompt only reveals them once it is unfolded ("Nascondi dettagli" is the
        // same control with its other wording).
        if (TextHasAnyWord(row, kHelpLinkWords, ARRAYSIZE(kHelpLinkWords))) continue;
        if (TextHasAnyWord(row, kChangeLinkWords, ARRAYSIZE(kChangeLinkWords))) continue;
        if (NameIsDetails(row)) continue;
        AddDetailRow(row);
    }
    for (const auto& p : info.paths) {
        if (p.empty()) continue;
        AddDetailRow(p);
    }
    if (!info.appPath.empty())
        AddDetailRow(std::wstring(g_loc->programLocation) + L" " + info.appPath);

    if (g_ui.details.empty()) AddDetailRow(g_loc->noExtraInfo);
    if (g_ui.details.empty()) g_ui.details.push_back(L"-");
}

// The information the original screen keeps behind its own "Show details": it is taken by
// expanding that control (a click on the prompt's own UI, which can only reveal information,
// never approve anything) and reading the texts that appear. The row this fills is the same one
// the original dialog has, so the two screens end up showing the same things.
// Matched by part of the text, not by equality: the prompt may add an access key, a colon or a
// different verb, and the control may carry the action while the text sits in a child.
static const WCHAR* kDetailsWords[] = {
    L"mostra dettagli", L"nascondi dettagli", L"show details", L"hide details",
    L"details anzeigen", L"details ausblenden", L"afficher les d\u00e9tails",
    L"masquer les d\u00e9tails", L"mostrar detalles", L"ocultar detalles",
    L"dettagli", L"details", L"d\u00e9tails", L"detalles",
    L"mostrar detalhes", L"ocultar detalhes", L"detalhes",
    L"details weergeven", L"details verbergen", L"poka\u017c szczeg\u00f3\u0142y",
    L"ukryj szczeg\u00f3\u0142y", L"szczeg\u00f3\u0142y",
    L"\u043f\u043e\u0434\u0440\u043e\u0431\u043d\u043e\u0441\u0442\u0438",
};

static bool NameIsDetails(const std::wstring& name) {
    std::wstring t = ToLower(Trim(name));
    if (t.empty() || t.size() > 120) return false;
    for (const WCHAR* w : kDetailsWords)
        if (t.find(w) != std::wstring::npos) return true;
    return false;
}

#if HAVE_UIA
// Invoke first, then the legacy accessibility action: some templates expose the action of a
// button-like element only through it. Both are documented UI Automation interfaces.
static bool InvokeElementPattern(IUIAutomationElement* el) {
    if (!el) return false;
    IUIAutomationInvokePattern* ip = nullptr;
    if (SUCCEEDED(el->GetCurrentPatternAs(UIA_InvokePatternId, IID_PPV_ARGS(&ip))) && ip) {
        HRESULT hr = ip->Invoke();
        ip->Release();
        if (SUCCEEDED(hr)) return true;
    }
    IUIAutomationLegacyIAccessiblePattern* leg = nullptr;
    if (SUCCEEDED(el->GetCurrentPatternAs(UIA_LegacyIAccessiblePatternId, IID_PPV_ARGS(&leg))) &&
        leg) {
        HRESULT hr = leg->DoDefaultAction();
        leg->Release();
        if (SUCCEEDED(hr)) return true;
    }
    return false;
}

// The element that carries the text is not always the element that carries the action: the
// action is looked for on the element and on up to three ancestors of it. No further than that:
// this must never end up clicking something unrelated.
static bool ActivateElementOrAncestor(IUIAutomationElement* el) {
    if (!el || !g_uia) return false;
    if (InvokeElementPattern(el)) return true;
    IUIAutomationTreeWalker* w = nullptr;
    if (FAILED(g_uia->get_ControlViewWalker(&w)) || !w) return false;
    IUIAutomationElement* cur = el;      // borrowed: the elements are kept alive by g_uiaRefs
    IUIAutomationElement* owned = nullptr;
    bool done = false;
    for (int i = 0; i < 3 && cur && !done; i++) {
        IUIAutomationElement* parent = nullptr;
        if (FAILED(w->GetParentElement(cur, &parent)) || !parent) break;
        if (owned) owned->Release();
        owned = parent;
        cur = owned;
        done = InvokeElementPattern(owned);
    }
    if (owned) owned->Release();
    w->Release();
    return done;
}
#endif

// The elements of the prompt as the log sees them: used when no details control can be found, so
// the next run of the log says exactly what this build of Windows exposes instead of leaving it
// to a guess.
static void LogPromptElements(const PromptInfo& info) {
    Dbg(L"native prompt exposes %d element(s):", (int)info.elements.size());
    for (const auto& e : info.elements) {
        if (e.name.empty() && !e.type) continue;
        Dbg(L"  type=%-5d enabled=%d password=%d name='%s'", (int)e.type, e.enabled,
            e.isPassword, e.name.c_str());
    }
}

#if HAVE_UIA
// Every name in the prompt's tree, texts only: no element list is touched, so the pointers the
// buttons live on (yes/no, resolved on the frozen set) stay valid.
static void CollectNativeTexts(HWND hNative, std::vector<std::wstring>& out) {
    out.clear();
    if (!UiaInit()) return;
    IUIAutomationElement* root = nullptr;
    if (FAILED(g_uia->ElementFromHandle(hNative, &root)) || !root) return;
    IUIAutomationTreeWalker* walker = nullptr;
    if (SUCCEEDED(g_uia->get_RawViewWalker(&walker)) && walker) {
        std::vector<UiElement> extra;
        int budget = 300;
        WalkTree(walker, root, extra, 0, &budget);
        walker->Release();
        for (const auto& e : extra)
            if (!e.name.empty()) out.push_back(e.name);
    }
    root->Release();
}
#else
static void CollectNativeTexts(HWND, std::vector<std::wstring>& out) { out.clear(); }
#endif

static void HarvestNativeDetails(PromptInfo& info) {
    info.detailTexts.clear();
    if (!info.hNative || !IsWindow(info.hNative)) return;

    std::vector<std::wstring> before;
    CollectNativeTexts(info.hNative, before);

    UiElement* target = nullptr;
    for (auto& e : info.elements) {
        if (e.name.empty() || !NameIsDetails(e.name)) continue;
        target = &e;
        break;
    }
    if (!target) {
        // Nothing that looks like a details control: the element list goes into the log, so this
        // build's own wording can be read instead of guessed.
        Dbg(L"native details: no 'Show details' control found by its text");
        LogPromptElements(info);
        return;
    }
    Dbg(L"native details: control '%s' (type=%d)", target->name.c_str(), (int)target->type);
    bool invoked = false;
#if HAVE_UIA
    invoked = ActivateElementOrAncestor(target->element);
#endif
    if (!invoked) invoked = RelayViaWin32(info.hNative, target->name);
    if (!invoked) {
        Log(L"native details: the 'Show details' control could not be activated");
        return;
    }
    // The expanded view is built asynchronously: it is read as soon as it has grown, and the wait
    // is bounded (about 300 ms) so a prompt with nothing to reveal never delays the dialog.
    size_t prev = before.size();
    for (int i = 0; i < 5; i++) {
        if (StopWait(60)) break;
        CollectNativeTexts(info.hNative, info.detailTexts);
        if (info.detailTexts.size() > before.size()) break;      // the view grew: take it
        if (i >= 2 && info.detailTexts.size() == prev) break;     // nothing is coming
        prev = info.detailTexts.size();
    }
    Dbg(L"native details: %d text(s) in the prompt after expanding (%d before)",
        (int)info.detailTexts.size(), (int)before.size());
}

// Reading the prompt's own details is a bonus: it can fail in any way it likes - a tree that
// changes under the walker, an allocation that fails - and the dialog must still come up. Whatever
// happens, this returns with the texts it managed to collect, possibly none.
static void HarvestDetailsSafely(PromptInfo& info) {
#if W7UAC_HAS_EXCEPTIONS
    try {
        HarvestNativeDetails(info);
    } catch (...) {
        Log(L"reading the prompt's own details failed -> the pane keeps what it already has");
        info.detailTexts.clear();
    }
#else
    HarvestNativeDetails(info);
#endif
}

// A mod-side notice, so it follows the language of the dialog itself. The prompt's own strings
// are never reused for it: they say what the prompt says, not what the mod just did.

// The flat-icon skin uses the shell's own documented SIID_SHIELD artwork. Request the list tier
// that best fits the layout (16, 48 or 256 source pixels); the dialog draws it at its 16/48/128
// layout tier, so large-DPI prompts are not stuck with a 32-pixel stock icon.
static HICON LoadSystemShieldIcon(int desiredArtSize, int* artSize) {
    SHSTOCKICONINFO sii = {};
    sii.cbSize = sizeof(sii);
    if (SUCCEEDED(SHGetStockIconInfo(SIID_SHIELD, SHGSI_SYSICONINDEX, &sii))) {
        IImageList* list = nullptr;
        int tier = desiredArtSize <= 16 ? SHIL_SMALL
                   : desiredArtSize <= 48 ? SHIL_EXTRALARGE
                                          : SHIL_JUMBO;
        if (SUCCEEDED(SHGetImageList(tier, IID_IImageList, (void**)&list)) && list) {
            HICON icon = nullptr;
            HRESULT hr = (sii.iSysImageIndex >= 0)
                             ? list->GetIcon(sii.iSysImageIndex, ILD_TRANSPARENT, &icon)
                             : E_FAIL;
            list->Release();
            if (SUCCEEDED(hr) && icon) {
                if (artSize) *artSize = desiredArtSize;
                return icon;
            }
        }
    }
    // Fallback: the shell's native small/large icon. Its display size is still bounded by the
    // current skin's layout tier; if this lookup also fails, the caller uses the embedded Win7 art.
    sii = {};
    sii.cbSize = sizeof(sii);
    if (SUCCEEDED(SHGetStockIconInfo(
            SIID_SHIELD, SHGSI_ICON | (desiredArtSize <= 16 ? SHGSI_SMALLICON : SHGSI_LARGEICON),
            &sii)) && sii.hIcon) {
        if (artSize) *artSize = desiredArtSize;
        return sii.hIcon;
    }
    return nullptr;
}

static HICON LoadSkinShieldIcon(int desiredArtSize, int variant, int* artSize) {
    if (variant < 0 || variant > 2) variant = 0;
    if (artSize) *artSize = desiredArtSize;

    // Flat Icons always uses the Windows blue system shield, independently of the banner state
    // or the bannerShield override. The warning colour and warning text remain visible separately.
    if (g_settings.windowTheme == WindowThemeWin10Flat) {
        HICON icon = LoadSystemShieldIcon(desiredArtSize, artSize);
        if (icon) return icon;
        Dbg(L"the Windows flat system shield is unavailable - no non-flat shield is substituted");
        return nullptr;
    }

    // The supplied orange exclamation shield matches orange banners in the Basic and 8.1 skins.
    // The custom Aero preset keeps the complete built-in Windows 7 set, including its own warning icon.
    if (variant == 2 && g_settings.windowTheme != WindowThemeWin7Aero) {
        const WCHAR* art = desiredArtSize <= 16 ? kShieldOrange_16B64
                           : desiredArtSize <= 48 ? kShieldOrange_48B64
                                                  : kShieldOrange_128B64;
        HICON icon = LoadB64Icon(art);
        if (icon) return icon;
        Dbg(L"the supplied orange shield could not be loaded - the built-in exclamation shield is used");
    }

    if (variant == 0 && g_settings.windowTheme == WindowThemeWin81Native) {
        const WCHAR* art = desiredArtSize <= 16 ? kShield81_16B64
                           : desiredArtSize <= 48 ? kShield81_48B64
                                                  : kShield81_128B64;
        HICON icon = LoadB64Icon(art);
        if (icon) return icon;
        Dbg(L"Windows 8.1 shield image could not be loaded - the Windows 7 shield is used");
    }

    // Both Windows 7 themes use embedded verified and question-mark shields; Aero also keeps the
    // built-in Windows 7 exclamation shield. A trusted third-party publisher uses the blue-banner question mark.
    return LoadShieldIcon(desiredArtSize, variant);
}

// The "Show details" row: the expander button and the label, drawn by the dialog itself - the
// button with the Windows 7 art embedded in this file (see DrawExpanderGlyph). The same function
// also answers the owner-draw message of the row's hit area, and that is not a detail: Windows
// erases an owner-drawn control with the system face colour *before* asking the parent to draw
// it, so with only the WM_PAINT copy the erase stayed on screen as a grey, empty rectangle -
// exactly where the expander and the label should have been. No custom class, no subclassing: the
// documented owner-draw message is simply answered with the real drawing.
static void DrawDetailsRow(HDC dc, int left, int top, int right, BOOL focused, BOOL hover,
                           BOOL pressed) {
    if (!dc || !g_loc || right <= left) return;
    int dpi = g_ui.dpi;
    int rowH = ScaleFor(22, dpi);
    // The button of the reference is 19-20 px across and its label starts 6 px after it, so the
    // disc sits in the middle of the 22 px row and the words stay where they were.
    int chevSize = ScaleFor(20, dpi);

    // The DC of an owner-drawn control arrives with an opaque background and with the system
    // colour as its background colour: without this the words carry a block of that colour exactly
    // as wide as the line of text, on top of the row.
    SetBkMode(dc, TRANSPARENT);
    SetBkColor(dc, RGB(240, 240, 240));

    // The light grey strip of the original, not white: the expander is a white disc, and it is the
    // grey around it that makes it read as the button of the reference (see PaintWin7Ui).
    RECT bg = {left, top, right, top + rowH};
    HBRUSH w = CreateSolidBrush(RGB(240, 240, 240));
    FillRect(dc, &bg, w);
    DeleteObject(w);

    DrawExpanderGlyph(dc, left, top + (rowH - chevSize) / 2, chevSize, g_ui.detailsExpanded, hover,
                      pressed);
    // The same ink the labels of the dialog use: the row belongs to the paragraph above it.
    SetTextColor(dc, RGB(10, 10, 10));
    SelectObject(dc, g_ui.fontNormal);
    const WCHAR* label = g_ui.detailsExpanded ? g_loc->hideDetails : g_loc->showDetails;
    RECT dr = {left + chevSize + ScaleFor(6, dpi), top + ScaleFor(1, dpi), right,
               top + ScaleFor(20, dpi)};
    DrawTextW(dc, label, -1, &dr, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

    // The reference shows the first letter of each word underlined - it is the prompt's keyboard
    // shortcut, drawn even while the keyboard is nowhere near the dialog - and a shortcut that is
    // never drawn is a shortcut nobody can see. The label is laid out word by word, with a one
    // pixel rule under its first letter: a flat line in the same ink, drawn at the depth the
    // reference shows - under the glyphs, not at a guessed offset, which is why the font's own
    // metrics say where their last row is.
    int barY = top + ScaleFor(16, dpi);
    TEXTMETRICW tm = {};
    if (GetTextMetricsW(dc, &tm) && tm.tmHeight > 0 && tm.tmDescent < tm.tmHeight)
        barY = dr.top + tm.tmHeight - tm.tmDescent + ScaleFor(2, dpi);

    int lx = dr.left;
    const WCHAR* cur = label;          // not `w`: that name already holds the strip's brush here
    while (*cur) {
        while (*cur == L' ') cur++;
        if (!*cur) break;
        const WCHAR* wordStart = cur;
        while (*cur && *cur != L' ') cur++;
        int wordLen = (int)(cur - wordStart);
        std::wstring word(wordStart, wordLen);
        SIZE wsz = {};
        GetTextExtentPoint32W(dc, word.c_str(), wordLen, &wsz);
        if (wsz.cx > 0) {
            SIZE one = {};
            GetTextExtentPoint32W(dc, word.c_str(), 1, &one);
            RECT ur = {lx + ScaleFor(1, dpi), barY, lx + one.cx + ScaleFor(1, dpi),
                       barY + ScaleFor(1, dpi)};
            HBRUSH ub = CreateSolidBrush(RGB(10, 10, 10));
            FillRect(dc, &ur, ub);
            DeleteObject(ub);
            lx += wsz.cx;
        }
        SIZE sp = {};
        if (GetTextExtentPoint32W(dc, L" ", 1, &sp)) lx += sp.cx;
    }
    if (focused) {
        RECT fr = {left - ScaleFor(3, dpi), top - ScaleFor(2, dpi), left + ScaleFor(220, dpi),
                   top + ScaleFor(20, dpi)};
        DrawFocusRect(dc, &fr);
    }
}

static void PaintWin7Ui(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);
    Layout L = ComputeLayout(rc.right, rc.bottom, g_ui.dpi, g_ui.detailsExpanded);

    // The content area of the original is white; everything below it is the light grey strip that
    // carries the "Show details" row, the buttons and the line of links, and the content ends with
    // the one pixel separator the original draws under the last information row. The strip is
    // painted first, so every element of that half of the dialog sits on it.
    int bandY = L.detailsY - ScaleFor(12, g_ui.dpi);
    if (bandY < 0) bandY = 0;
    if (bandY < rc.bottom) {
        RECT band = {0, bandY, rc.right, rc.bottom};
        HBRUSH strip = CreateSolidBrush(RGB(240, 240, 240));
        FillRect(dc, &band, strip);
        DeleteObject(strip);
        HPEN bandSep = CreatePen(PS_SOLID, 1, RGB(223, 223, 223));
        HPEN oldBandSep = (HPEN)SelectObject(dc, bandSep);
        MoveToEx(dc, 0, bandY, nullptr);
        LineTo(dc, rc.right, bandY);
        SelectObject(dc, oldBandSep);
        DeleteObject(bandSep);
    }

    HGDIOBJ originalFont = SelectObject(dc, g_ui.fontNormal);

    // Nothing is drawn for the title bar: it is the native one (see the window styles in the
    // session). The client area starts directly under it.

    // Banner
    RECT banner = {0, L.capH, L.W, L.capH + L.bannerH};
    int bannerState = g_ui.info ? g_ui.info->bannerState : 1;
    COLORREF bannerColour = BannerColour(bannerState);
    FillGradientV(dc, banner, bannerColour, bannerColour);   // flat: one colour, no ramp

    // The shield follows the selected theme and state. The Flat Icons skin keeps the blue Windows
    // shield; non-flat skins use warning artwork, including the supplied exclamation on amber.
    if (g_ui.shieldBig)
        DrawIconEx(dc, L.iconX, L.iconY, g_ui.shieldBig, L.iconSize, L.iconSize, 0, nullptr,
                   DI_NORMAL);

    SetBkMode(dc, TRANSPARENT);
    // The amber "unsigned publisher" banner is the only one with black text (authui.dll carries
    // an explicit rgb(0,0,0) for that state).
    SetTextColor(dc, bannerState == 2 ? RGB(0, 0, 0) : RGB(255, 255, 255));
    SelectObject(dc, g_ui.fontBanner);
    std::wstring question = g_loc->question;
    std::wstring appName, publisher;
    if (g_ui.info) {
        if (!g_ui.info->publisherFromSignature && !g_ui.info->questionText.empty())
            question = g_ui.info->questionText;
        appName = g_ui.info->appText;
        publisher = g_ui.info->publisherText;
    }
    RECT qr = {L.textX, L.textY, L.textX + L.textW, L.capH + L.bannerH - ScaleFor(4, g_ui.dpi)};
    DrawTextW(dc, question.c_str(), -1, &qr,
              LocTextFlags(DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX));

    // The rows were mapped in FinishHarvest (label + position inside the prompt's own tree).

    int rowY = L.rowY;
    int rowStep = ScaleFor(ROW_H, g_ui.dpi);
    int rowH = ScaleFor(18, g_ui.dpi);
    // The size the layout is drawn for at this display scale - not the size of the art the shell
    // happened to publish: at 250% the shell hands over a 256 px icon and the dialog still shows it
    // at 80 px, through the filter (see the GDI+ section).
    int iconSize = ScaleFor(32, g_ui.dpi);

    // One label/value pair per line: program, publisher, and - when the prompt says which file it
    // is elevating - the origin of the file. The third line is not decoration: it is exactly what
    // the modern prompt hides behind "show details".
    const WCHAR* labels[3] = {g_loc->programName,
                              PublisherLabelFor(g_ui.info && g_ui.info->publisherVerifiedByWindows),
                              g_loc->origin};
    std::wstring values[3];
    values[0] = appName;
    values[1] = publisher;
    if (g_ui.info) values[2] = g_ui.info->originText;
    int rows = values[2].empty() ? 2 : 3;
    int rowsHeight = rowStep * (rows - 1) + rowH;
    // The program icon is centred on the block of label rows, not stuck to the first one.
    int iconTop = rowY + (rowsHeight - iconSize) / 2;
    if (g_ui.appIcon) {
        bool drawn = false;
        if (g_ui.appIconBmp && iconSize != g_ui.appIconSize)
            drawn = GdiplusDrawBitmap(dc, g_ui.appIconBmp, ScaleFor(PAD, g_ui.dpi), iconTop,
                                      iconSize, iconSize) != FALSE;
        if (!drawn)
            DrawIconEx(dc, ScaleFor(PAD, g_ui.dpi), iconTop, g_ui.appIcon, iconSize, iconSize, 0,
                       nullptr, DI_NORMAL);
    }

    // The values sit in one column, so the column starts after the longest label that is actually
    // used: the labels are the prompt's own strings and they are never clipped against the value.
    int labelX = ScaleFor(PAD, g_ui.dpi) + ScaleFor(44, g_ui.dpi);
    int labelW = 0;
    SelectObject(dc, g_ui.fontNormal);
    for (int i = 0; i < rows; i++) {
        SIZE sz = {};
        if (GetTextExtentPoint32W(dc, labels[i], (int)wcslen(labels[i]), &sz) && sz.cx > labelW)
            labelW = sz.cx;
    }
    int valueX = labelX + labelW + ScaleFor(12, g_ui.dpi);

    // The label and the bold value of a row sit on one baseline, the way the reference shows them:
    // the two faces have slightly different ascents, so the value is started by the difference
    // between them instead of by the same top. The ink is the reference's own, which measures pure
    // black for the labels and for the values alike.
    TEXTMETRICW tmLabel = {}, tmValue = {};
    SelectObject(dc, g_ui.fontNormal);
    GetTextMetricsW(dc, &tmLabel);
    SelectObject(dc, g_ui.fontBold);
    GetTextMetricsW(dc, &tmValue);
    const int valueDrop = (int)tmLabel.tmAscent - (int)tmValue.tmAscent;   // 0, or one pixel

    SetTextColor(dc, RGB(0, 0, 0));
    SelectObject(dc, g_ui.fontNormal);
    for (int i = 0; i < rows; i++) {
        RECT lr = {labelX, rowY + i * rowStep, valueX - ScaleFor(4, g_ui.dpi),
                   rowY + i * rowStep + rowH};
        DrawTextW(dc, labels[i], -1, &lr,
                  LocTextFlags(DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX));
    }

    // Only the publisher is bold - the name of the author, or the prompt's own word for an unknown
    // one. The file name and the path of its origin are plain text, like the labels beside them.
    SelectObject(dc, g_ui.fontNormal);
    for (int i = 0; i < rows; i++) {
        if (i == 1) continue;
        RECT vr = {valueX, rowY + i * rowStep, L.W - ScaleFor(PAD, g_ui.dpi),
                   rowY + i * rowStep + rowH};
        // DT_NOPREFIX, on every value: a file path and a company name are data, not a menu caption.
        // Without it DrawText reads an ampersand as a mnemonic prefix - "C:\R&D\tool.exe" came out
        // as "C:\RD\tool.exe" with the D underlined, which is the artefact this replaces. The
        // "File origin" row uses DT_PATH_ELLIPSIS, the flag Microsoft documents for paths: a path
        // too long for the row keeps its file name after the last backslash instead of losing it.
        UINT ellipsis = (i == 2) ? DT_PATH_ELLIPSIS : DT_END_ELLIPSIS;
        DrawTextW(dc, values[i].empty() ? L"-" : values[i].c_str(), -1, &vr,
                  LocTextFlags(DT_LEFT | DT_TOP | DT_SINGLELINE | ellipsis | DT_NOPREFIX));
    }

    SelectObject(dc, g_ui.fontBold);
    {
        RECT vr = {valueX, rowY + rowStep + valueDrop, L.W - ScaleFor(PAD, g_ui.dpi),
                   rowY + rowStep + rowH + ScaleFor(2, g_ui.dpi)};
        DrawTextW(dc, values[1].empty() ? L"-" : values[1].c_str(), -1, &vr,
                  LocTextFlags(DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX));
    }

    // "Show details": drawn only when there is something to show, and never as an empty box. The
    // separator under it is always drawn; the link at the right of the bottom row is drawn when the
    // `bottomLinks` setting asks for it.
    if (HasDetailsRow()) {
    DrawDetailsRow(dc, ScaleFor(PAD, g_ui.dpi), L.detailsY, L.W - ScaleFor(PAD, g_ui.dpi),
                   (g_ui.hDetails && GetFocus() == g_ui.hDetails), g_ui.detailsHot, FALSE);

    // Details box: anchored to the row above it and exactly as tall as the rows it holds, so it
    // can never come out empty, and its text can never be pushed out of the client area by
    // whatever space happens to be left above the separator. Every row is one line: the pane is
    // a list of the request's own strings, not a paragraph. If the prompt published nothing,
    // BuildDetailsPane has already put its own line in `details`, so this box is never blank.
    if (g_ui.detailsExpanded) {
        int dpi = g_ui.dpi;
        int paneH = DetailsPaneHeight(dpi);
        int top = L.detailsY + ScaleFor(22, dpi);   // one row below it, never inside it
        RECT box = {ScaleFor(PAD, dpi), top, L.W - ScaleFor(PAD, dpi), top + paneH};
        HBRUSH b = CreateSolidBrush(RGB(252, 252, 252));
        FillRect(dc, &box, b);
        DeleteObject(b);
        HPEN p = CreatePen(PS_SOLID, 1, RGB(190, 200, 212));
        HPEN op = (HPEN)SelectObject(dc, p);
        HGDIOBJ ob = SelectObject(dc, GetStockObject(NULL_BRUSH));
        Rectangle(dc, box.left, box.top, box.right, box.bottom);
        SelectObject(dc, ob);
        SelectObject(dc, op);
        DeleteObject(p);

        // dRowH, not rowH: the row pitch of the details box is its own measure, and a local with
        // the same name as the one above would shadow it inside this block.
        int dRowH = DetailsRowHeight(dpi);
        int dy = box.top + ScaleFor(5, dpi);
        SetTextColor(dc, RGB(45, 45, 45));
        SelectObject(dc, g_ui.fontNormal);
        for (size_t i = 0; i < g_ui.details.size(); i++) {
            RECT r = {box.left + ScaleFor(8, dpi), dy, box.right - ScaleFor(8, dpi), dy + dRowH};
            DrawTextW(dc, g_ui.details[i].c_str(), -1, &r,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            dy += dRowH;
        }
    }
    }   // HasDetailsRow()

    // Separator and links (the same one pixel line the original draws in both places, measured off
    // the reference: #DFDFDF)
    HPEN sep = CreatePen(PS_SOLID, 1, RGB(223, 223, 223));
    HPEN oldSep = (HPEN)SelectObject(dc, sep);
    MoveToEx(dc, 0, L.sepY, nullptr);
    LineTo(dc, L.W, L.sepY);
    SelectObject(dc, oldSep);
    DeleteObject(sep);

#if W7UAC_BOTTOM_LINKS
    // One link, at the right: "Change when these notifications appear".
    {
        RECT changeRc = ChangeLinkRect(dc, L);
        DrawLink(dc, changeRc, g_loc->changeNotifications, g_ui.hoverId == 2, TRUE);
    }
#endif

    // The notice sits in the button row, in the space the right-aligned buttons leave free.
    if (!g_ui.notice.empty() && (LONG)(GetTickCount() - g_ui.noticeUntil) < 0) {
        RECT nr = {ScaleFor(PAD, g_ui.dpi), L.btnY,
                   L.W - ScaleFor(PAD, g_ui.dpi) - ScaleFor(16, g_ui.dpi) -
                       ((g_ui.hYes || g_ui.hNo) ? 2 * ScaleFor(BTN_W, g_ui.dpi) : 0),
                   L.btnY + ScaleFor(BTN_H, g_ui.dpi)};
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(120, 120, 120));
        SelectObject(dc, g_ui.fontNormal);
        DrawTextW(dc, g_ui.notice.c_str(), -1, &nr,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    }

    SelectObject(dc, originalFont);
    EndPaint(hWnd, &ps);
}

static void LayoutControls(HWND hWnd) {
    RECT rc;
    GetClientRect(hWnd, &rc);
    Layout L = ComputeLayout(rc.right, rc.bottom, g_ui.dpi, g_ui.detailsExpanded);
    int dpi = g_ui.dpi;
    int bw = ScaleFor(BTN_W, dpi), bh = ScaleFor(BTN_H, dpi);

    if (g_ui.hNo)
        MoveWindow(g_ui.hNo, rc.right - ScaleFor(PAD, dpi) - bw, L.btnY, bw, bh, TRUE);
    if (g_ui.hYes)
        MoveWindow(g_ui.hYes, rc.right - ScaleFor(PAD, dpi) - 2 * bw - ScaleFor(8, dpi), L.btnY,
                   bw, bh, TRUE);
    // "Show details" is painted in WM_PAINT (chevron + text); the control is an invisible
    // hit area on top of it, so Tab and the mouse work while the look stays Windows 7.
    if (g_ui.hDetails) {
        if (HasDetailsRow())
            MoveWindow(g_ui.hDetails, ScaleFor(PAD, dpi), L.detailsY, ScaleFor(220, dpi),
                       ScaleFor(22, dpi), TRUE);
        else
            ShowWindow(g_ui.hDetails, SW_HIDE);
    }
}

// The program icon as a GDI+ bitmap, built from the icon the shell published. Called wherever that
// icon is replaced, so the two can never drift apart; the previous bitmap is disposed here.
static void BuildAppIconBitmap() {
    GdiplusDisposeBitmap(g_ui.appIconBmp);
    g_ui.appIconBmp = nullptr;
    if (!g_ui.appIcon) return;
    int w = 0, h = 0;
    g_ui.appIconBmp = GdiplusBitmapFromHIcon(g_ui.appIcon, &w, &h);
    if (g_ui.appIconBmp)
        Dbg(L"program icon: %d px art ready for GDI+ (drawn at %d px, the size this display scale "
            L"lays out)", w, ScaleFor(32, g_ui.dpi));
}

static void FreeUiResources() {
    if (g_ui.fontNormal) { DeleteObject(g_ui.fontNormal); g_ui.fontNormal = nullptr; }
    if (g_ui.fontBold) { DeleteObject(g_ui.fontBold); g_ui.fontBold = nullptr; }
    if (g_ui.fontBanner) { DeleteObject(g_ui.fontBanner); g_ui.fontBanner = nullptr; }
    if (g_ui.shieldSmall) { DestroyIcon(g_ui.shieldSmall); g_ui.shieldSmall = nullptr; }
    if (g_ui.shieldBig) { DestroyIcon(g_ui.shieldBig); g_ui.shieldBig = nullptr; }
    if (g_ui.appIcon) { DestroyIcon(g_ui.appIcon); g_ui.appIcon = nullptr; }
    GdiplusDisposeBitmap(g_ui.appIconBmp);
    for (int tier = 0; tier < CHEVRON_TIER_COUNT; tier++) {
        for (int i = 0; i < CHEVRON_STATE_COUNT; i++) {
            GdiplusDisposeBitmap(g_ui.chevBmp[tier][i]);
            if (g_ui.chevStream[tier][i]) {
                g_ui.chevStream[tier][i]->Release();
                g_ui.chevStream[tier][i] = nullptr;
            }
        }
    }
    if (g_ui.hExpando) { CloseThemeData(g_ui.hExpando); g_ui.hExpando = nullptr; }
    for (int tier = 0; tier < CHEVRON_TIER_COUNT; tier++) {
        for (int i = 0; i < CHEVRON_STATE_COUNT; i++)
            if (g_ui.chevArt[tier][i]) {
                DestroyIcon(g_ui.chevArt[tier][i]);
                g_ui.chevArt[tier][i] = nullptr;
            }
        g_ui.chevTried[tier] = false;
    }
    g_ui.detailsHot = false;
}

// The pane is filled while the dialog is being built. If the prompt's own details could not be
// read then - its tree can still be filling in that early - the click on the row is the last
// chance: one more attempt, on the spot, because the user asked for exactly those details. This
// is the only moment the mod touches the native prompt while its own dialog is up, and it can
// only reveal information: it can never decide anything.
// Sizes the window for a *client* area of baseHeight plus, when the pane is open, exactly the
// height the pane needs. The documented AdjustWindowRectEx turns that client size into the size
// the system wants, so the frame is never counted twice: SetWindowPos takes a *window* size, and
// passing a client height straight to it is how the dialog used to grow less than the pane
// needed (and to shrink by the title bar on every collapse).
static void ResizeUiToContent(HWND hWnd) {
    if (!hWnd) return;
    int clientH = g_ui.baseHeight + (g_ui.detailsExpanded ? DetailsPaneHeight(g_ui.dpi) : 0);
    // A client height is never allowed to be nonsense: a dialog taller than the work area is worse
    // than a small one, and a zero here would only produce a window nobody can see.
    if (clientH < ScaleFor(120, g_ui.dpi)) clientH = ScaleFor(120, g_ui.dpi);
    RECT r = {0, 0, g_ui.baseWidth, clientH};
    DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
    DWORD exStyle = WS_EX_TOPMOST | WS_EX_DLGMODALFRAME;
    if (!AdjustWindowRectEx(&r, style, FALSE, exStyle)) {
        r.left = 0; r.top = 0; r.right = g_ui.baseWidth; r.bottom = clientH;
    }
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, r.right - r.left, r.bottom - r.top,
                 SWP_NOMOVE | SWP_NOACTIVATE);
}

static void RefreshDetailsOnDemand() {
    PromptInfo* info = g_ui.info;
    if (!info) return;
    g_ui.detailsAttempts++;

    size_t paneBefore = g_ui.details.size();
    HarvestDetailsSafely(*info);

    const WCHAR* src = L"none";
    std::wstring again = FindTargetPath(*info, &src);
    if (!again.empty() && again != info->appPath) {
        info->appPath = again;
        info->originText = again;
        Dbg(L"details on demand: the file is '%s' (source: %s)", again.c_str(), src);
    }
    BuildDetailsPane(*info);
    g_ui.detailsFromNative = !info->detailTexts.empty();

    // Whatever was learned now has to reach the screen: the icon of the program (if it was not
    // known when the dialog was built) and the third info row, which did not exist before.
    if (!g_ui.appIcon && !info->appPath.empty()) {
        std::vector<std::wstring> one;
        one.push_back(info->appPath);
        g_ui.appIcon = LoadAppIcon(one, ScaleFor(32, g_ui.dpi), &g_ui.appIconSize);
        if (g_ui.appIcon)
            Dbg(L"details on demand: program icon taken from '%s' (%d px art)", info->appPath.c_str(),
                g_ui.appIconSize);
        BuildAppIconBitmap();
    }
    if (g_ui.hwnd) {
        g_ui.baseHeight = BaseClientHeight(g_ui.dpi);
        ResizeUiToContent(g_ui.hwnd);
        LayoutControls(g_ui.hwnd);
        Dbg(L"details on demand: %d row(s) in the pane (%d before)",
            (int)g_ui.details.size(), (int)paneBefore);
    }
}

static LRESULT CALLBACK UiWndProcBody(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        g_ui.dpi = (int)GetSystemDpiValue();
        RebuildFonts();
        // The small caption icon is the selected theme's verified shield, never the verdict for
        // this request. The banner uses state-specific artwork on non-flat themes; Flat Icons keeps
        // the Windows blue shield, and an orange banner uses the supplied matching exclamation art.
        int smallArt = ShieldArtSize(ScaleFor(16, g_ui.dpi));
        g_ui.shieldSmall = LoadSkinShieldIcon(smallArt, 0, nullptr);
        int art = ShieldArtSize(ScaleFor(44, g_ui.dpi));
        int variant = g_ui.info ? g_ui.info->shieldVariant : 0;
        if (g_settings.bannerShield >= 1 && g_settings.bannerShield <= 3 &&
            g_settings.bannerShield - 1 > variant)
            variant = g_settings.bannerShield - 1;
        // An orange banner always carries its matching exclamation shield, even if a manual
        // shield override was selected. Flat Icons is the exception: it remains consistently blue.
        if (g_settings.windowTheme != WindowThemeWin10Flat && g_ui.info &&
            g_ui.info->bannerState == 2)
            variant = 2;
        g_ui.shieldBig = LoadSkinShieldIcon(art, variant, &g_ui.shieldSize);
        if (!g_ui.shieldBig && g_settings.windowTheme != WindowThemeWin10Flat) {
            g_ui.shieldBig = LoadShieldIcon(art, variant);
            g_ui.shieldSize = art;
        }
        Dbg(L"banner shield: %d px art, drawn at %d px (%d%% of the art), skin=%d state=%d",
            g_ui.shieldSize, BannerShieldPixels(g_ui.shieldSize), BANNER_SHIELD_PERCENT,
            g_settings.windowTheme, variant);
        // The real icon of the elevated file when the prompt tells us which file it is; the
        // stock "application" icon - the one Windows 7 uses for an unknown program - otherwise.
        if (g_ui.info && !g_ui.info->appPath.empty()) {
            std::vector<std::wstring> one;
            one.push_back(g_ui.info->appPath);
            g_ui.appIcon = LoadAppIcon(one, ScaleFor(32, g_ui.dpi), &g_ui.appIconSize);
            if (g_ui.appIcon)
                Dbg(L"program icon taken from '%s' (%d px art, drawn 1:1)",
                    g_ui.info->appPath.c_str(), g_ui.appIconSize);
            BuildAppIconBitmap();
        }
        if (!g_ui.appIcon && g_ui.info)
            g_ui.appIcon = LoadAppIcon(g_ui.info->paths, ScaleFor(32, g_ui.dpi), &g_ui.appIconSize);
        if (!g_ui.appIcon) {
            g_ui.appIcon = LoadGenericAppIcon(ScaleFor(32, g_ui.dpi));
            g_ui.appIconSize = ScaleFor(32, g_ui.dpi);
            Dbg(L"program icon: stock application icon (no usable path in the prompt)");
        }
        BuildAppIconBitmap();

        const WCHAR* yesLabel = L"Yes";
        const WCHAR* noLabel = L"No";
        if (g_ui.info && g_ui.info->yes) yesLabel = g_ui.info->yes->name.c_str();
        if (g_ui.info && g_ui.info->no) noLabel = g_ui.info->no->name.c_str();

        // The two buttons are real Win32 push buttons, drawn by the system theme - the shape, the
        // colours and the states a button of this dialog has on this Windows, nothing imitated.
        // The `buttonStyle` setting keeps the other way available: 1 = owner-drawn and painted
        // by the mod, the Windows 7 Aero way.
        //
        // "No" is the default button of the Windows 7 prompt: Enter and Esc both mean No.
        DWORD yesStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                         (g_settings.style == 1 ? BS_OWNERDRAW : BS_PUSHBUTTON);
        DWORD noStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON;
        if (g_settings.style == 1) noStyle |= BS_OWNERDRAW;
        g_ui.hYes = CreateWindowExW(0, L"BUTTON", yesLabel, yesStyle, 0, 0, 10, 10, hWnd,
            (HMENU)IDC_W7UAC_YES, nullptr, nullptr);
        g_ui.hNo = CreateWindowExW(0, L"BUTTON", noLabel, noStyle, 0, 0, 10, 10, hWnd,
            (HMENU)IDC_W7UAC_NO, nullptr, nullptr);
        g_ui.hDetails = CreateWindowExW(0, L"BUTTON", g_loc->showDetails,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, 0, 0, 10, 10, hWnd,
            (HMENU)IDC_W7UAC_DETAILS, nullptr, nullptr);

        // A prompt with no button to answer with is worse than the Windows one: failing the creation
        // destroys this window, and the session's own fail-open path (null hwnd) hands the native
        // prompt back - the same road every other failure here takes.
        if (!g_ui.hYes || !g_ui.hNo || !g_ui.hDetails) {
            Log(L"a control of the replacement could not be created -> fail open");
            return -1;
        }

        // The documented way to give a control its font. A control that is never handed one uses
        // the default GUI font, which is how a push button ends up with text heavier than the
        // labels around it: this hands the same regular font the rest of the dialog uses.
        if (g_ui.hYes) SendMessageW(g_ui.hYes, WM_SETFONT, (WPARAM)g_ui.fontNormal, TRUE);
        if (g_ui.hNo) SendMessageW(g_ui.hNo, WM_SETFONT, (WPARAM)g_ui.fontNormal, TRUE);
        if (g_ui.hDetails) SendMessageW(g_ui.hDetails, WM_SETFONT, (WPARAM)g_ui.fontNormal, TRUE);

        if (g_ui.info && !g_ui.info->yesEnabled && g_ui.hYes) EnableWindow(g_ui.hYes, FALSE);
        if (!HasDetailsRow() && g_ui.hDetails) ShowWindow(g_ui.hDetails, SW_HIDE);
        LayoutControls(hWnd);
        SetFocus(g_ui.hNo ? g_ui.hNo : hWnd);
        return 0;
    }
    case WM_SIZE:
        LayoutControls(hWnd);
        return 0;
    case WM_CTLCOLORBTN: {
        // The three controls of the lower strip - the row of "Show details" and the two buttons -
        // are painted on the grey of that strip. Windows erases an owner-drawn control with the
        // brush returned here before asking the parent to draw it, and returning the strip's own
        // colour is the documented way to colour a control's background: anything else leaves a
        // differently coloured rectangle under the row or behind the rounded corners.
        if ((HWND)lParam == g_ui.hDetails || (HWND)lParam == g_ui.hYes ||
            (HWND)lParam == g_ui.hNo) {
            static HBRUSH strip = nullptr;
            if (!strip) strip = CreateSolidBrush(RGB(240, 240, 240));
            SetBkColor((HDC)wParam, RGB(240, 240, 240));
            return (LRESULT)strip;
        }
        break;
    }
    case WM_ERASEBKGND: {
        // The content area is white and nothing in WM_PAINT clears it: the banner, the strip and
        // the details pane each paint their own rectangle, but the information rows and the icon
        // sit on bare client area. Without this erase every repaint draws the text on top of the
        // previous one (InvalidateRect is called with bErase = FALSE), and after a few repaints
        // the glyphs pile up and read as bold. Clearing the whole client area here, once, is what
        // keeps a repaint a repaint - which is exactly what WM_ERASEBKGND is for, since the class
        // registers no background brush.
        HDC dcErase = (HDC)wParam;
        if (dcErase) {
            RECT rcErase;
            GetClientRect(hWnd, &rcErase);
            HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(dcErase, &rcErase, white);
            DeleteObject(white);
        }
        return 1;
    }
    case WM_PAINT:
        PaintWin7Ui(hWnd);
        return 0;
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        if ((int)dis->CtlID == IDC_W7UAC_DETAILS) {
            // The control has already erased itself with the colour returned for it (the grey of
            // the strip): the row is drawn here, with the same code the WM_PAINT path uses, and
            // this is the copy that carries the hover and pressed states of the button.
            DrawDetailsRow(dis->hDC, dis->rcItem.left, dis->rcItem.top, dis->rcItem.right,
                           (dis->itemState & ODS_FOCUS) != 0,
                           g_ui.detailsHot || (dis->itemState & ODS_HOTLIGHT) != 0,
                           (dis->itemState & ODS_SELECTED) != 0);
            return TRUE;
        }
        BOOL pressed = (dis->itemState & ODS_SELECTED) != 0;
        BOOL focused = (dis->itemState & ODS_FOCUS) != 0;
        BOOL disabled = (dis->itemState & ODS_DISABLED) != 0;
        BOOL hover = ((int)dis->CtlID == g_ui.hoverId);
        WCHAR label[128] = L"";
        GetWindowTextW(dis->hwndItem, label, ARRAYSIZE(label));
        DrawWin7Button(dis->hDC, dis->rcItem, label, focused, pressed, disabled, hover,
                       g_ui.fontNormal);
        return TRUE;
    }
#if W7UAC_BOTTOM_LINKS
    case WM_MOUSEMOVE: {
        // Only the link needs a hover test of its own: the "Show details" row learns about the
        // cursor from WM_SETCURSOR, and the buttons are children with their own tracking.
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        RECT rc;
        GetClientRect(hWnd, &rc);
        Layout L = ComputeLayout(rc.right, rc.bottom, g_ui.dpi, g_ui.detailsExpanded);
        int hover = 0;
        HDC dc = GetDC(hWnd);
        RECT lr = ChangeLinkRect(dc, L);
        if (dc) ReleaseDC(hWnd, dc);
        if (pt.x >= lr.left && pt.x <= lr.right && pt.y >= lr.top && pt.y <= lr.bottom) hover = 2;
        if (hover != g_ui.hoverId) {
            g_ui.hoverId = hover;
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hWnd, 0};
        TrackMouseEvent(&tme);
        return 0;
    }
#endif
    case WM_MOUSELEAVE:
        if (g_ui.hoverId || g_ui.detailsHot) {
            g_ui.hoverId = 0;
            g_ui.detailsHot = false;
            if (g_ui.hDetails) InvalidateRect(g_ui.hDetails, nullptr, TRUE);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        return 0;
    case WM_LBUTTONDOWN: {
        // The click is consumed here, so the default button has to be given the focus back by
        // hand: Enter and Esc keep working after a click on the painted area.
        if (g_ui.hNo && GetFocus() != g_ui.hNo && GetFocus() != g_ui.hYes) SetFocus(g_ui.hNo);
        // With no link there is nothing else under the cursor: the click only gives the default
        // button the focus back, which the lines above already did.
        return 0;
    }
    case WM_SETCURSOR: {
        if (LOWORD(lParam) == HTCLIENT) {
            // The window that holds the cursor is the one Windows names here, and for a child
            // control it relays the message to its parent: that is how the hover state of the
            // "Show details" row is known, with no subclassing and no polling, and the row is
            // repainted only when the state really changed.
            BOOL hot = (g_ui.hDetails && (HWND)wParam == g_ui.hDetails);
            if (hot != g_ui.detailsHot) {
                g_ui.detailsHot = hot;
                // The row is a child control painted on demand: what shows its hover state is its
                // own owner-draw message, so the control - not only the dialog - has to be asked
                // to paint again.
                if (g_ui.hDetails) InvalidateRect(g_ui.hDetails, nullptr, TRUE);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            if (g_ui.hoverId == 1 || g_ui.hoverId == 2) {
                SetCursor(LoadCursorW(nullptr, IDC_HAND));
                return TRUE;
            }
        }
        break;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == IDC_W7UAC_YES) {
            g_ui.result = IDC_W7UAC_YES;
            g_ui.done = true;
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        } else if (id == IDC_W7UAC_NO || id == IDCANCEL || id == IDOK) {
            g_ui.result = IDC_W7UAC_NO;
            g_ui.done = true;
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        } else if (id == IDC_W7UAC_DETAILS) {
            g_ui.detailsExpanded = !g_ui.detailsExpanded;
            // Nothing from the original screen yet and the cap is not reached: try again now.
            if (g_ui.detailsExpanded && !g_ui.detailsFromNative && g_ui.detailsAttempts < 3)
                RefreshDetailsOnDemand();
            ResizeUiToContent(hWnd);
            SetWindowTextW(g_ui.hDetails,
                           g_ui.detailsExpanded ? g_loc->hideDetails : g_loc->showDetails);
            LayoutControls(hWnd);
            InvalidateRect(hWnd, nullptr, TRUE);
            UpdateWindow(hWnd);
            // The pane and its geometry go into the log: if it ever comes out empty again, the
            // next log line says why instead of leaving it to a guess.
            RECT cr = {};
            GetClientRect(hWnd, &cr);
            Dbg(L"details pane: %s, %d row(s), pane height %d, client %dx%d",
                g_ui.detailsExpanded ? L"expanded" : L"collapsed", (int)g_ui.details.size(),
                g_ui.detailsExpanded ? DetailsPaneHeight(g_ui.dpi) : 0,
                (int)cr.right, (int)cr.bottom);
        }
        return 0;
    }
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            g_ui.result = IDC_W7UAC_NO;
            g_ui.done = true;
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        }
        return 0;
    case WM_SYSCOMMAND:
        // Only the Alt+<letter> shortcuts are ours. Everything else has to reach DefWindowProc:
        // SC_CLOSE is what the X of the native title bar (and Alt+F4) sends, and swallowing the
        // whole message is exactly why the close button used to do nothing at all. SC_MOVE,
        // SC_SIZE and the system menu work the same way - the system draws them, the system
        // handles them.
        if ((wParam & 0xFFF0) == SC_KEYMENU && lParam != 0) {
            WCHAR c = towupper((WCHAR)lParam);
            if (c == L'Y') {
                g_ui.result = IDC_W7UAC_YES;
                g_ui.done = true;
                PostMessageW(hWnd, WM_CLOSE, 0, 0);
            } else if (c == L'N') {
                g_ui.result = IDC_W7UAC_NO;
                g_ui.done = true;
                PostMessageW(hWnd, WM_CLOSE, 0, 0);
            }
            return 0;
        }
        break;
    case WM_CLOSE:
        // The X of the native title bar (and Alt+F4) is a decision, exactly like the X of the
        // native prompt: it means "No". The paths that must not decide anything - the panic
        // hotkey, the watchdog, the consent flow ending by itself - set g_ui.done first.
        if (!g_ui.done) {
            g_ui.done = true;
            if (g_ui.result == RESULT_NO_DECISION && !NativeCallFinished() && !StopRequested()) {
                Log(L"the dialog was closed from the title bar -> No");
                g_ui.result = IDC_W7UAC_NO;
            }
        }
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        g_ui.hwnd = nullptr;
        g_ui.info = nullptr;   // the session's PromptInfo is on the thread's stack, never ours to keep
        PostQuitMessage(0);
        return 0;
    case WM_NCDESTROY:
        FreeUiResources();
        break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// The window procedure Windows calls, with the same last net the rest of this file has. A C++
// exception (an allocation the dialog's own containers cannot get, in the middle of a paint) must
// never travel back into the system's dispatch. It decides nothing here: the message is dropped,
// the dialog stays as it is and the watchdog hands the Windows prompt back the way it always does.
static LRESULT CALLBACK UiWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
#if W7UAC_HAS_EXCEPTIONS
    try {
        return UiWndProcBody(hWnd, msg, wParam, lParam);
    } catch (...) {
        Log(L"exception in the window procedure (message 0x%04X): the message is dropped, no "
            L"decision is taken and the watchdog brings the Windows prompt back", msg);
        return 0;
    }
#else
    return UiWndProcBody(hWnd, msg, wParam, lParam);
#endif
}

static HMODULE g_uiModule = nullptr;
static bool g_uiClassRegistered = false;
static bool RegisterUiClass() {
    if (g_uiClassRegistered) return true;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&UiWndProc), &g_uiModule)) return false;
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = UiWndProc;
    wc.hInstance = g_uiModule;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = WIN7UAC_CLASS;
    if (!RegisterClassExW(&wc)) return false;
    g_uiClassRegistered = true;
    return true;
}

//------------------------------------------------------------------------------
// Session
//------------------------------------------------------------------------------

static HWND g_hNativePrompt = nullptr;
static HANDLE g_uiThread = nullptr;      // the replacement-UI thread (one prompt at a time)

static RECT g_nativeOriginalRect = {};
static bool g_nativeMoved = false;
static bool g_nativeRectValid = false;   // a real on-screen rect of the prompt was captured
// When the consent hook was entered: the clock the instant ejection is reported against.
static DWORD g_requestTick = 0;

static void RestoreNativePrompt() {
    NativeLock lock;
    InterlockedExchange(&g_promptGivenBack, 1);
    if (g_hNativePrompt && g_nativeMoved && IsWindow(g_hNativePrompt)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(g_hNativePrompt, &pid);
        if (pid != GetCurrentProcessId()) {
            g_nativeMoved = false;
            g_hNativePrompt = nullptr;
            return;
        }
        Log(L"restoring the native prompt window");
        // The bypass was set under g_nativeCs before this move. All watcher state uses
        // that same recursive lock, so a late location event cannot start a new takeover.
        BOOL restored;
        if (g_nativeRectValid) {
            restored = SetWindowPos(g_hNativePrompt, HWND_TOPMOST, g_nativeOriginalRect.left,
                         g_nativeOriginalRect.top,
                         g_nativeOriginalRect.right - g_nativeOriginalRect.left,
                         g_nativeOriginalRect.bottom - g_nativeOriginalRect.top,
                         SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
        } else {
            RECT cur = {}, wa = {};
            GetWindowRect(g_hNativePrompt, &cur);
            SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
            restored = SetWindowPos(g_hNativePrompt, HWND_TOPMOST,
                         wa.left + ((wa.right - wa.left) - (cur.right - cur.left)) / 2,
                         wa.top + ((wa.bottom - wa.top) - (cur.bottom - cur.top)) / 2, 0, 0,
                         SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
        }
        SetForegroundWindow(g_hNativePrompt);
        if (restored) g_nativeMoved = false;
        else Log(L"native restore failed: %u", GetLastError());

    }
}
static void TakeOverNativePrompt(HWND h) {
    NativeLock lock;
    if (StopRequested() || InterlockedCompareExchange(&g_promptGivenBack, 0, 0)) return;
    if (!h || !IsWindow(h)) return;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) {
        Log(L"refusing to move a window that is not ours (%p)", h);
        return;
    }
    g_hNativePrompt = h;
    // The position to restore is the one the window had before it was first taken over: the
    // function can be called again while the window is already off screen.
    {
        // Remember the last place the prompt really had on screen (it can be re-placed by the system).
        RECT cur = {};
        if (GetWindowRect(h, &cur) && cur.left > -30000 && cur.right - cur.left >= 160 &&
            cur.bottom - cur.top >= 90) {
            g_nativeOriginalRect = cur;
            g_nativeRectValid = true;
        }
    }
    if (g_settings.nativeWindowMode == 2) { g_nativeMoved = false; return; }
    if (g_settings.nativeWindowMode == 1) {
        g_nativeMoved = ShowWindowAsync(h, SW_HIDE) != FALSE;
    } else {
        g_nativeMoved = SetWindowPos(h, nullptr, -32000, -32000, 0, 0,
                                    SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                                    SWP_ASYNCWINDOWPOS) != FALSE;
    }
}

// The documented way to follow the state of another window: an event hook, not a poll. The system
// calls this the moment the native prompt is shown, hidden, moved or destroyed, and the only thing
// done here is to post one message - the dialog's own thread then looks at the window. Until now a
// 50 ms timer woke that thread twenty times a second whether anything had happened or not, and on a
// slow machine that is twenty chances a second for the dialog to fall behind its own painting.
//
// WINEVENT_OUTOFCONTEXT keeps the callback in this process (no code is injected anywhere), and
// WINEVENT_SKIPOWNTHREAD drops the events this very thread makes while it paints and moves its own
// dialog, so what arrives is the native prompt and nothing else.
static void CALLBACK PromptStateProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                     LONG, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || !hwnd) return;
    { NativeLock lock; if (hwnd != g_hNativePrompt) return; }      // only the prompt this session is replacing
    HWND dlg = g_ui.hwnd;
    if (!dlg) return;
    PostMessageW(dlg, WM_W7UAC_STATECHANGED, event, 0);
}

// A window that was moved off screen by this mod is still "visible" for IsWindowVisible, so the
// check asks where it is: only a prompt that is really on screen has to be taken away again.
static bool NativePromptOnScreen() {
    NativeLock lock;
    if (!g_hNativePrompt || !IsWindow(g_hNativePrompt)) return false;
    if (!IsWindowVisible(g_hNativePrompt)) return false;
    RECT r = {};
    if (!GetWindowRect(g_hNativePrompt, &r)) return false;
    return r.left > -30000;
}

// The instant ejection. The CreateWindowExW hook calls it before the native host can be shown,
// and the watcher calls it again whenever the system moves that window somewhere else. The window
// is therefore parked before UI Automation and the rest of the reading work begin, with no timer
// keeping either desktop busy while the prompt is pending.
//
// Nothing here is irreversible and nothing touches the window's messages: it is only moved - or
// hidden, when the setting asks for that - with documented calls, the place it had is remembered,
// and a session that then decides *not* to replace the UI hands it back untouched
// (GiveNativePromptBack). A window that is off screen is still alive, still enabled and still
// reachable through UI Automation, which is what the rest of the session works on.

static void EjectTheNativePromptAtOnce(HWND hwnd) {
    NativeLock lock;
    if (StopRequested() || NativeCallFinished() ||
        !InterlockedCompareExchange(&g_inHook, 0, 0) ||
        InterlockedCompareExchange(&g_promptGivenBack, 0, 0)) return;
    if (!hwnd || !IsWindow(hwnd)) return;
    if (g_settings.relayMode != 1 || g_settings.dumpUiTree) return;   // observe: never touched
    if (g_settings.nativeWindowMode == 2) return;                     // leave it in place
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return;

    if (g_nativeMoved && g_hNativePrompt == hwnd) {
        // It is already away: the only reason to act again is that the system has put it back on
        // screen, and then it goes away again in the same event - the log only says it once.
        if (!NativePromptOnScreen()) return;
        Dbg(L"the native prompt came back on screen -> off it goes again");
        TakeOverNativePrompt(hwnd);
        return;
    }

    const unsigned long ms = g_requestTick ? (unsigned long)(GetTickCount() - g_requestTick) : 0;
    g_hNativePrompt = hwnd;
    TakeOverNativePrompt(hwnd);
    if (g_nativeMoved)
        Log(L"the native prompt left the screen %lu ms after the request (%s)", ms,
            g_settings.nativeWindowMode == 1 ? L"hidden" : L"moved off screen");
}

// The native XAML host is created by consent.exe itself. Parking it at the return of
// CreateWindowExW is earlier than any SHOW event and avoids making the desktop watcher responsible
// for catching a transient on-screen frame. The WinEvent watcher remains as a no-polling safety net
// for later LOCATIONCHANGE events.
typedef HWND(WINAPI* CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
                                        HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t g_createWindowExW_orig = nullptr;

static HWND WINAPI CreateWindowExWHook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
                                       DWORD style, int x, int y, int width, int height,
                                       HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    HWND hwnd = g_createWindowExW_orig(exStyle, className, windowName, style, x, y, width, height,
                                       parent, menu, instance, param);
    if (!hwnd) return hwnd;

    WCHAR cls[128] = L"";
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (g_settings.detection != 2 && IsConsentHostClass(cls))
        EjectTheNativePromptAtOnce(hwnd);
    return hwnd;
}

static void InstallNativeCreateHook() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    FARPROC target = user32 ? GetProcAddress(user32, "CreateWindowExW") : nullptr;
    if (!target || !Wh_SetFunctionHook((void*)target, (void*)CreateWindowExWHook,
                                       (void**)&g_createWindowExW_orig)) {
        Log(L"CreateWindowExW hook unavailable -> WinEvent discovery remains active (no polling)");
        return;
    }
    Log(L"hooked user32!CreateWindowExW for synchronous consent-host parking");
}

static void GiveNativePromptBack() {
    // Restore sets the permanent per-request bypass before raising location/show events.
    RestoreNativePrompt();
}

static BOOL CALLBACK EnumerateDesktopProc(HWND h, LPARAM lParam) {
    std::vector<PromptCandidate>* out = (std::vector<PromptCandidate>*)lParam;
    if (!IsWindowVisible(h)) return TRUE;
    WCHAR cls[128] = L"";
    GetClassNameW(h, cls, ARRAYSIZE(cls));
    if (cls[0] == L'\0' || wcscmp(cls, WIN7UAC_CLASS) == 0) return TRUE;
    if (IsInternalHelperClass(cls)) return TRUE;
    RECT rc = {};
    if (!GetWindowRect(h, &rc)) return TRUE;
    if (rc.right - rc.left < 160 || rc.bottom - rc.top < 90) return TRUE;

    PromptCandidate c = {};
    c.hwnd = h;
    GetWindowThreadProcessId(h, &c.pid);
    c.ours = (c.pid == GetCurrentProcessId()) ? TRUE : FALSE;
    wcsncpy_s(c.cls, cls, _TRUNCATE);
    GetWindowTextW(h, c.title, ARRAYSIZE(c.title));
    c.desktop = GetThreadDesktop(GetCurrentThreadId());
    GetDesktopName(c.desktop, c.desktopName, ARRAYSIZE(c.desktopName));
    out->push_back(c);
    return TRUE;
}

static void EnumerateCurrentDesktop(std::vector<PromptCandidate>& out) {
    out.clear();
    EnumDesktopWindows(GetThreadDesktop(GetCurrentThreadId()), EnumerateDesktopProc,
                       (LPARAM)&out);
}

static bool WaitForAnyCandidate(int timeoutMs) {
    DWORD start = GetTickCount();
    for (;;) {
        EnterCriticalSection(&g_cs);
        bool have = !g_candidates.empty();
        LeaveCriticalSection(&g_cs);
        if (StopRequested() || NativeCallFinished()) return false;
        if (have) return true;
        if (GetTickCount() - start >= (DWORD)timeoutMs) return false;
        if (StopWait(20)) return false;
    }
}

// The dump goes to the storage folder of this mod, which Windhawk removes together with the mod:
// a fixed name in a folder any user can write to (C:\Windows\Temp) would let another user create
// the file first and read what consent.exe, which runs as SYSTEM, writes into it. Wh_GetModStoragePath
// fills a caller-provided buffer and returns a non-zero value on success; it does not allocate.
static void WriteDump(const std::wstring& content) {
    WCHAR dir[MAX_PATH] = L"";
    if (Wh_GetModStoragePath(dir, ARRAYSIZE(dir)) == 0 || !dir[0]) {
        Log(L"cannot write the dump: the storage folder of the mod is not available");
        return;
    }
    CreateDirectoryW(dir, nullptr);   // exists already in the normal case

    WCHAR path[MAX_PATH + 32];
    _snwprintf_s(path, ARRAYSIZE(path), _TRUNCATE, L"%s\\win7uac-dump.txt", dir);

    HANDLE h = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        Log(L"cannot write the dump to %s (error %u)", path, (unsigned)GetLastError());
        return;
    }
    DWORD written = 0;
    WriteFile(h, content.c_str(), (DWORD)(content.size() * sizeof(WCHAR)), &written, nullptr);
    CloseHandle(h);
    Log(L"dump written to %s", path);
}

static void AppendLine(std::wstring& s, const WCHAR* fmt, ...) {
    WCHAR buf[2048];
    va_list a;
    va_start(a, fmt);
    _vsnwprintf_s(buf, ARRAYSIZE(buf), _TRUNCATE, fmt, a);
    va_end(a);
    s += buf;
    s += L"\r\n";
}

static std::wstring BuildDump(const PromptInfo& info, bool harvested, const Identity& id) {
    std::wstring d;
    WCHAR deskName[64] = L"";
    GetDesktopName(GetThreadDesktop(GetCurrentThreadId()), deskName, ARRAYSIZE(deskName));

    AppendLine(d, L"=== win7-uac-recreation dump ===");
    AppendLine(d, L"consent.exe pid=%lu, UI thread desktop='%s'", GetCurrentProcessId(), deskName);
    AppendLine(d, L"last step: %s", g_step);
    AppendLine(d, L"command line: %s", GetCommandLineW() ? GetCommandLineW() : L"");
    AppendLine(d, L"user desktop app: '%s' (class '%s')", id.userDesktopApp.c_str(),
               id.userDesktopAppClass.c_str());
    for (const auto& t : id.cmdLineTokens) AppendLine(d, L"cmd line token: %s", t.c_str());

    AppendLine(d, L"\r\n=== candidate windows ===");
    EnterCriticalSection(&g_cs);
    std::vector<PromptCandidate> list = g_candidates;
    LeaveCriticalSection(&g_cs);
    for (const auto& c : list) {
        if (!IsWindow(c.hwnd)) {
            AppendLine(d, L"hwnd=%p pid=%lu (vanished before the dump)", c.hwnd, c.pid);
            continue;
        }
        RECT rc = {};
        GetWindowRect(c.hwnd, &rc);
        AppendLine(d,
                   L"hwnd=%p pid=%lu ours=%d desktop=%s class='%s' title='%s' rect=(%ld,%ld,%ld,%ld) visible=%d",
                   c.hwnd, c.pid, c.ours, c.desktopName, c.cls, c.title, rc.left, rc.top, rc.right,
                   rc.bottom, IsWindowVisible(c.hwnd));
    }

    AppendLine(d, L"\r\n=== other processes' windows seen while the prompt was pending ===");
    AppendLine(d, L"(recorded only: they are never enumerated, never harvested, never clicked)");
    EnterCriticalSection(&g_cs);
    std::vector<std::wstring> foreign = g_rejectLog;
    LeaveCriticalSection(&g_cs);
    for (const auto& f : foreign) AppendLine(d, L"%s", f.c_str());

    if (!harvested) {
        AppendLine(d, L"\r\nharvest failed");
        return d;
    }

    AppendLine(d, L"\r\n=== controls (%s) hwnd=%p desktop='%s' ===", info.harvestSource, info.hNative,
               deskName);
    for (const auto& e : info.elements) {
        AppendLine(d, L"type=%d enabled=%d pass=%d class='%s' id='%s' name='%s' rect=(%ld,%ld,%ld,%ld)",
                   (int)e.type, e.enabled, e.isPassword, e.className.c_str(),
                   e.automationId.c_str(), e.name.c_str(), e.rect.left, e.rect.top, e.rect.right,
                   e.rect.bottom);
    }
    for (const auto& t : info.texts) AppendLine(d, L"text: %s", t.c_str());
    AppendLine(d, L"mapped: question='%s' app='%s' publisher='%s'", info.questionText.c_str(),
               info.appText.c_str(), info.publisherText.c_str());
    AppendLine(d, L"buttonsConfident=%d yesEnabled=%d banner=%d credential=%d",
               info.buttonsConfident, info.yesEnabled, info.bannerState, info.credentialVariant);

    HMODULE mods[256];
    DWORD needed = 0;
    if (EnumProcessModules(GetCurrentProcess(), mods, sizeof(mods), &needed)) {
        AppendLine(d, L"\r\n=== interesting loaded modules ===");
        int n = (int)(needed / sizeof(HMODULE));
        for (int i = 0; i < n && i < (int)ARRAYSIZE(mods); i++) {
            WCHAR path[MAX_PATH] = L"";
            GetModuleFileNameW(mods[i], path, ARRAYSIZE(path));
            std::wstring lower = ToLower(path);
            if (lower.find(L"authui") != std::wstring::npos ||
                lower.find(L"credui") != std::wstring::npos ||
                lower.find(L"xaml") != std::wstring::npos ||
                lower.find(L"directui") != std::wstring::npos ||
                lower.find(L"uiautomation") != std::wstring::npos ||
                lower.find(L"consent") != std::wstring::npos ||
                lower.find(L"themeui") != std::wstring::npos ||
                lower.find(L"comctl") != std::wstring::npos ||
                lower.find(L"win32u") != std::wstring::npos ||
                lower.find(L"uxtheme") != std::wstring::npos)
                AppendLine(d, L"module: %s", path);
        }
    }
    return d;
}

static void DescribePrompt(HWND h, const PromptInfo& info, WCHAR* out, size_t cch) {
    RECT rc = {};
    GetWindowRect(h, &rc);
    swprintf_s(out, cch,
               L"hwnd=%p title='%s' rect=(%ld,%ld,%ld,%ld) harvest=%s elements=%d buttons=%d",
               h, info.windowTitle.c_str(), rc.left, rc.top, rc.right, rc.bottom,
               info.harvestSource, (int)info.elements.size(), info.buttonsConfident);
}

// The yes/no pointers point into `elements`, which is filled before the dialog is built and never
// touched again. This checks that they still point inside that vector before they are used to
// press a button: if anything ever did reallocate it, the relay fails open instead of reading
// freed memory.
static bool ElementUsable(const PromptInfo& info, const UiElement* e) {
    if (!e || info.elements.empty()) return false;
    const UiElement* first = info.elements.data();
    return e >= first && e < (first + info.elements.size());
}
static bool NativeButtonsUsable(const PromptInfo& info) {
    if (!IsWindow(info.hNative) || !IsWindowEnabled(info.hNative)) return false;
    if (!ElementUsable(info, info.yes) || !ElementUsable(info, info.no)) return false;
    for (const UiElement* button : {info.yes, info.no}) {
#if HAVE_UIA
        if (button->element) {
            BOOL enabled = FALSE;
            if (FAILED(button->element->get_CurrentIsEnabled(&enabled)) || !enabled) return false;
            continue;
        }
#endif
        RelayCtx ctx = {&button->name, nullptr};
        EnumChildWindows(info.hNative, FindButtonByTextProc, (LPARAM)&ctx);
        if (!ctx.found || !IsWindowEnabled(ctx.found)) return false;
    }
    return true;
}

static DWORD WINAPI UiSessionRunInner() {
    g_step = L"session: start";
    EnterCriticalSection(&g_cs);
    g_rejectLog.clear();
    LeaveCriticalSection(&g_cs);
    g_ui.details.clear();
    g_ui.detailsFromNative = false;
    g_ui.detailsAttempts = 0;
    g_ui.notice.clear();
    g_ui.noticeUntil = 0;
    g_ui.result = RESULT_NO_DECISION;
    g_ui.done = false;
    InterlockedExchange(&g_mappingLogged, 0);

    bool useEnumeration = (g_settings.detection == 1);
    g_step = L"session: waiting for the prompt window";
    if (useEnumeration) {
        Log(L"detection = polling: no watchers, the desktops are enumerated from here");
    } else if (!WaitForAnyCandidate(g_settings.detectTimeoutMs)) {
        if (NativeCallFinished()) {
            Log(L"the native call already returned: there is no prompt to replace -> fail open");
            LogSeenWindows(L"windows parked");
            return 0;
        }
        Log(L"no candidate window from the watchers within %d ms -> enumerating the desktops",
            g_settings.detectTimeoutMs);
        useEnumeration = true;
    }
    if (StopRequested() || NativeCallFinished()) {
        GiveNativePromptBack();
        Log(L"the native call already returned: there is no prompt to replace -> fail open");
        LogSeenWindows(L"windows parked");
        return 0;
    }

    std::vector<HDESK> desktops;
    std::vector<std::wstring> desktopNames;
    if (useEnumeration) {
        for (const auto& w : g_watchers) {
            if (!w.desktop) continue;
            desktops.push_back(w.desktop);
            desktopNames.push_back(w.name);
        }
        if (desktops.empty()) {
            for (const WCHAR* name : {kDesktopWinlogon, kDesktopDefault}) {
                HDESK d = OpenDesktopByName(name);
                if (!d) continue;
                desktops.push_back(d);
                desktopNames.push_back(name);
            }
        }
    } else {
        EnterCriticalSection(&g_cs);
        for (const auto& c : g_candidates) {
            if (!c.desktop) continue;
            if (std::find(desktops.begin(), desktops.end(), c.desktop) == desktops.end()) {
                desktops.push_back(c.desktop);
                desktopNames.push_back(c.desktopName);
            }
        }
        LeaveCriticalSection(&g_cs);
    }

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    PromptInfo info;
    bool haveInfo = false;
    bool infoOurs = false;
    bool confident = false;
    DWORD overallDeadline = GetTickCount() + (DWORD)g_settings.detectTimeoutMs;
    int nDesktops = desktops.empty() ? 1 : (int)desktops.size();
    DWORD sliceMs = (DWORD)g_settings.detectTimeoutMs / (DWORD)nDesktops;
    if (sliceMs < 800) sliceMs = 800;

    std::vector<HWND> skipLogged;
    auto logSkip = [&](HWND h, const WCHAR* cls, const RECT* rc, const WCHAR* why) {
        if (std::find(skipLogged.begin(), skipLogged.end(), h) != skipLogged.end()) return;
        skipLogged.push_back(h);
        if (rc)
            Log(L"window not usable: hwnd=%p class='%s' %dx%d (%s)", h, cls,
                (int)(rc->right - rc->left), (int)(rc->bottom - rc->top), why);
        else
            Log(L"window not usable: hwnd=%p class='%s' (%s)", h, cls, why);
    };

    for (size_t di = 0; di < desktops.size() && !confident && !StopRequested(); di++) {
        DWORD sliceDeadline = GetTickCount() + sliceMs;
        if ((LONG)(sliceDeadline - overallDeadline) > 0) sliceDeadline = overallDeadline;
        if (!SetThreadDesktop(desktops[di])) {
            Log(L"SetThreadDesktop('%s') failed: %u", desktopNames[di].c_str(), GetLastError());
            continue;
        }
        WCHAR now[64] = L"";
        GetDesktopName(GetThreadDesktop(GetCurrentThreadId()), now, ARRAYSIZE(now));
        Log(L"UI thread now on desktop '%s' (prompt desktop '%s')", now,
            desktopNames[di].c_str());
        g_step = L"session: harvesting the prompt";

        for (;;) {
            if (StopRequested() || (NativeCallFinished() && !haveInfo)) break;
            std::vector<PromptCandidate> list;
            if (useEnumeration) {
                EnumerateCurrentDesktop(list);
            } else {
                EnterCriticalSection(&g_cs);
                list = g_candidates;
                LeaveCriticalSection(&g_cs);
            }

            for (auto it = list.rbegin(); it != list.rend(); ++it) {
                if (it->desktop != desktops[di]) continue;
                bool ours = (it->pid == GetCurrentProcessId());
                if (!ours && !(g_settings.relayMode == 0 || g_settings.dumpUiTree)) continue;
                if (!IsWindow(it->hwnd) || !IsWindowVisible(it->hwnd)) {
                    logSkip(it->hwnd, it->cls, nullptr, it->ours ? L"not visible"
                                                                 : L"not visible, other process");
                    continue;
                }
                RECT rc = {};
                if (!GetWindowRect(it->hwnd, &rc)) continue;
                if (rc.right - rc.left < 160 || rc.bottom - rc.top < 90) {
                    logSkip(it->hwnd, it->cls, &rc, L"too small");
                    continue;
                }

                PromptInfo attempt;
                bool haveWin32 = HarvestWin32(it->hwnd, attempt);
                attempt.hNative = it->hwnd;
                {
                    WCHAR t[512] = L"";
                    GetWindowTextW(it->hwnd, t, ARRAYSIZE(t));
                    attempt.windowTitle = t;
                }
                if (IsKnownPromptClass(it->cls)) {
                    Log(L"known prompt window: class='%s' hwnd=%p win32 children=%s", it->cls,
                        it->hwnd, haveWin32 ? L"yes" : L"no");
                }
                if (ours) {
                    PromptInfo u;
                    if (HarvestUia(it->hwnd, u)) {
                        u.hNative = it->hwnd;
                        u.windowTitle = attempt.windowTitle;
                        if (!haveWin32) {
                            attempt = std::move(u);
                        } else {
                            for (const auto& t : u.texts)
                                if (std::find(attempt.texts.begin(), attempt.texts.end(), t) ==
                                    attempt.texts.end())
                                    attempt.texts.push_back(t);
                            for (const auto& pp : u.paths)
                                if (std::find(attempt.paths.begin(), attempt.paths.end(), pp) ==
                                    attempt.paths.end())
                                    attempt.paths.push_back(pp);
                            for (const auto& e : u.elements) {
                                bool dup = false;
                                for (const auto& e2 : attempt.elements) {
                                    if (e.name.empty() || e2.name != e.name) continue;
                                    if (e2.rect.left == e.rect.left &&
                                        e2.rect.top == e.rect.top) { dup = true; break; }
                                }
                                if (!dup) attempt.elements.push_back(e);
                            }
                            if (u.credentialVariant) attempt.credentialVariant = true;
                            attempt.harvestSource = L"win32+uia";
                        }
                    }
                }
                if (attempt.elements.empty()) {
                    Dbg(L"nothing harvested from hwnd=%p class='%s' (win32=%s)", it->hwnd,
                        it->cls, haveWin32 ? L"partial" : L"none");
                    continue;
                }
                FinishHarvest(attempt);

                int btnCount = 0;
                for (const auto& e : attempt.elements)
                    if (e.type == UIA_ButtonControlTypeId) btnCount++;
                bool usable = attempt.buttonsConfident ||
                              (btnCount >= 2 && attempt.texts.size() >= 2);
                if (!usable) continue;

                bool better;
                if (!haveInfo) better = true;
                else if (ours != infoOurs) better = ours;
                else better = (!info.buttonsConfident && attempt.buttonsConfident);
                if (better) {
                    WCHAR d[256] = L"";
                    DescribePrompt(it->hwnd, attempt, d, ARRAYSIZE(d));
                    Log(L"prompt window (%s): %s",
                        ours ? L"in-process" : L"foreign, diagnosis only", d);
                    info = std::move(attempt);
                    haveInfo = true;
                    infoOurs = ours;
                    confident = attempt.buttonsConfident && ours;
                }
                if (confident) break;
            }
            if (confident) break;
            if ((LONG)(GetTickCount() - overallDeadline) > 0) break;
            if (!haveInfo && (LONG)(GetTickCount() - sliceDeadline) > 0) {
                Dbg(L"nothing usable on desktop '%s' in %lu ms -> next desktop",
                    desktopNames[di].c_str(), (unsigned long)sliceMs);
                break;
            }
            if (StopWait(25)) break;
        }
    }

    if (!haveInfo) {
        EnterCriticalSection(&g_cs);
        unsigned cand = (unsigned)g_candidates.size(), oursN = 0, visN = 0;
        for (const auto& c : g_candidates) {
            if (c.ours) oursN++;
            if (IsWindow(c.hwnd) && IsWindowVisible(c.hwnd)) visN++;
        }
        LeaveCriticalSection(&g_cs);
        Log(L"no usable prompt window found (native call %s) -> fail open; candidates=%u ours=%u "
            L"visible=%u",
            NativeCallFinished() ? L"already returned" : L"still running", cand, oursN, visN);
        LogSeenWindows(L"windows parked");
        GiveNativePromptBack();
        CoUninitialize();
        return 0;
    }

    if (infoOurs) {
        g_step = L"session: harvest (uia)";
        PromptInfo u;
        if (HarvestUia(info.hNative, u)) {
            u.hNative = info.hNative;
            u.windowTitle = info.windowTitle;
            FinishHarvest(u);
            if (!confident) {
                info = std::move(u);
                confident = info.buttonsConfident;
            } else {
                for (const auto& t : u.texts)
                    if (std::find(info.texts.begin(), info.texts.end(), t) == info.texts.end())
                        info.texts.push_back(t);
                for (const auto& pp : u.paths)
                    if (std::find(info.paths.begin(), info.paths.end(), pp) == info.paths.end())
                        info.paths.push_back(pp);
                info.harvestSource = L"win32+uia";
            }
        }
    }

    g_step = L"session: identity";
    Identity id;
    CollectIdentity(id);

    if (g_settings.dumpUiTree || g_settings.relayMode == 0) {
        WriteDump(BuildDump(info, true, id));
        Log(L"observe/dump mode (%s): the native prompt was not touched",
            g_settings.dumpUiTree ? L"Write a diagnostics dump is on"
                                  : L"Replace the UAC prompt is off");
        GiveNativePromptBack();
        UiaRelease();
        CoUninitialize();
        return 0;
    }

    if (StopRequested() || !infoOurs || info.credentialVariant) {
        Log(L"the prompt asks for credentials (public signals: edit control / IsPassword / "
            L"wording) -> the recreated dialog is not shown and the native one is given back");
        GiveNativePromptBack();
        UiaRelease();
        CoUninitialize();
        return 0;
    }
    if (!info.buttonsConfident) {
        Log(L"buttons not positively identified -> fail open (window in-process: %s)",
            infoOurs ? L"yes" : L"no");
        GiveNativePromptBack();
        UiaRelease();
        CoUninitialize();
        return 0;
    }

    g_step = L"session: whitelist";
    std::wstring matchedBy;
    int wl = MatchWhitelist(id, info, &matchedBy);
    if (g_settings.whitelistMode == 0 && wl != 1) {
        Log(L"app not in the whitelist (%s) -> native UI left in place",
            wl < 0 ? L"identity undecidable" : L"no match");
        GiveNativePromptBack();
        UiaRelease();
        CoUninitialize();
        return 0;
    }
    if (g_settings.whitelistMode == 2 && wl == 1) {
        Log(L"app matched the exception list (%s) -> native UI left in place", matchedBy.c_str());
        GiveNativePromptBack();
        UiaRelease();
        CoUninitialize();
        return 0;
    }
    Log(L"replacing the UI (whitelist match: %s)",
        matchedBy.empty() ? L"mode 1 (all apps)" : matchedBy.c_str());

    g_step = L"session: taking the native prompt off screen";
    TakeOverNativePrompt(info.hNative);

    g_step = L"session: reading the native details";
    HarvestDetailsSafely(info);
    if (info.detailTexts.empty()) {
        StopWait(250);
        HarvestDetailsSafely(info);
    }
    g_ui.detailsFromNative = !info.detailTexts.empty();
    {
        const WCHAR* pathSource = L"none";
        std::wstring again = FindTargetPath(info, &pathSource);
        if (!again.empty() && again != info.appPath) {
            info.appPath = again;
            info.originText = again;
            Dbg(L"elevated file (from the native details): source=%s path='%s'", pathSource,
                again.c_str());
        }
    }

    // Recheck the native evidence after expanding the details. The compatibility name match is
    // allowed to run only when no unknown/unverified publisher row was revealed by that expansion.
    info.sawUnknownWording = PromptHasUnknownWording(info);
    if (ApplyRegeditOverrideIfSafe(info))
        Log(L"Regedit compatibility presentation enabled only after the native path/evidence "
            L"checks; signature verification is still required");

    // *** FIX 1: build the details pane HERE, before the signature check, so the
    //     three mapped rows are always present. The signature check may add the
    //     verified signer and rebuild the pane again. ***
    g_step = L"session: building the details pane";
    BuildDetailsPane(info);
    Log(L"details pane built before the dialog: %d row(s) (%d native text(s))",
        (int)g_ui.details.size(), (int)info.detailTexts.size());

    g_step = L"session: reading the digital signature";
    // systemFileConfirmed means only that the prompt named the system path. It never bypasses
    // WinVerifyTrust: even this tightly gated compatibility exception must earn its verified
    // presentation from the file's actual signature.
    // The catalog of the system is asked only when the two verdicts can disagree: when the
    // native prompt says that the publisher is verified (or when the Registry Editor
    // compatibility path is active) and the file carries no embedded signature of its own.
    // That is exactly the case of a component of Windows, whose signature lives in the catalog.
    if (!info.appPath.empty())
        info.signatureState = VerifySignatureState(info.appPath, nullptr,
                                                   info.publisherVerifiedByWindows ||
                                                       info.regeditOverrideApplied);

    if (info.regeditOverrideApplied) {
        bool trusted = info.signatureState == SigTrusted;
        info.publisherVerifiedByWindows = trusted;
        if (!(info.yes && !info.yesEnabled))
            info.bannerState = trusted ? 1 : 2;
        if (g_settings.bannerOverride >= 1 && g_settings.bannerOverride <= 3 &&
            g_settings.bannerOverride > info.bannerState)
            info.bannerState = g_settings.bannerOverride;
        Log(L"Regedit compatibility presentation: system path=%d signature=%s -> %s",
            info.systemFileConfirmed ? 1 : 0,
            trusted ? L"trusted" : L"not trusted/unknown",
            trusted ? L"verified publisher" : L"plain publisher");
        // The publisher label in the details pane follows the final, signature-backed state too.
        BuildDetailsPane(info);
    }

    {
        // The mod's own signature check can only make the prompt *more* cautious than the
        // operating system, never less: a component of Windows carries its signature in the
        // security catalog of the system, which WinVerifyTrust does not read, so the check
        // alone would answer "no signature" for notepad.exe and would take the full shield
        // away from every component of Windows. When the native row says "Verified publisher:
        // Microsoft Windows", the verdict of the operating system is the one that counts.
        bool signatureRefuted = (info.signatureState == SigUnsigned) ||
                                (info.signatureState == SigInvalid);
        bool nativeVerdictKept = signatureRefuted && info.nativeWindowsComponent;
        if (nativeVerdictKept)
            Log(L"the native row says a verified 'Microsoft Windows' publisher: the mod's own "
                L"signature check does not read the security catalog of the system, and the "
                L"verdict of the operating system is the one that counts");

        bool unconfirmed = (info.bannerState == 2) ||
                           (!info.publisherVerifiedByWindows) ||
                           (signatureRefuted && !info.nativeWindowsComponent) ||
                           (info.regeditOverrideApplied && info.signatureState != SigTrusted);
        int variant = 0;
        if (unconfirmed)
            variant = info.sawUnknownAuthor ? 2 : 1;
        else if (info.bannerState == 1 && !IsWindowsPublisherText(info.publisherText))
            variant = 1;
        if (info.bannerState == 3) variant = 2;

        // The manual choice. A choice that is more cautious than the automatic one is always
        // applied. The "verified" one is applied only when the operating system itself has
        // verified the publisher - the banner is blue and nothing contradicts the native row -
        // so that the prompt can never show more trust than Windows itself grants. Before this,
        // the setting could not do that at all: the automatic shield always won, and asking
        // for the verified shield gave the question mark back.
        if (g_settings.bannerShield >= 1 && g_settings.bannerShield <= 3) {
            int chosen = g_settings.bannerShield - 1;
            if (chosen > variant) {
                variant = chosen;                       // more cautious: always allowed
            } else if (chosen == 0 && variant != 0 && !unconfirmed && info.bannerState == 1) {
                variant = 0;                            // the full Windows shield
                Log(L"the manual choice asks for the verified shield: the native prompt has "
                    L"verified the publisher, so the full Windows shield is shown");
            }
        }
        info.shieldVariant = variant;
        Log(L"banner shield: %s (publisher %s, signature %s, author %s)",
            variant == 0 ? L"the verified shield"
                         : (variant == 1 ? L"the question-mark shield"
                                         : L"the exclamation shield"),
            unconfirmed ? L"not confirmed" : L"verified",
            info.signatureState == SigTrusted ? L"trusted"
                                              : (info.signatureState == SigUnsigned
                                                     ? L"absent"
                                                     : (info.signatureState == SigInvalid
                                                            ? L"present but not valid"
                                                            : L"unknown")),
            info.sawUnknownAuthor ? L"unknown" : L"known");
    }

    if (StopRequested()) {
        GiveNativePromptBack();
        UiaRelease();
        CoUninitialize();
        return 0;
    }
    g_ui.info = &info;
    { NativeLock lock; g_hNativePrompt = info.hNative; }

    if (!RegisterUiClass()) {
        Log(L"RegisterClassEx failed -> fail open");
        RestoreNativePrompt();
        UiaRelease();
        CoUninitialize();
        return 0;
    }

    g_step = L"session: creating the dialog";
    int dpi = (int)GetSystemDpiValue();
    g_ui.baseWidth = ScaleFor(500, dpi);
    g_ui.baseHeight = BaseClientHeight(dpi);
    DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
    DWORD exStyle = WS_EX_TOPMOST | WS_EX_DLGMODALFRAME;

    RECT wanted = {0, 0, g_ui.baseWidth, g_ui.baseHeight};
    if (!AdjustWindowRectEx(&wanted, style, FALSE, exStyle)) {
        wanted.left = 0; wanted.top = 0;
        wanted.right = g_ui.baseWidth; wanted.bottom = g_ui.baseHeight;
    }
    int w = wanted.right - wanted.left;
    int h = wanted.bottom - wanted.top;

    g_ui.hwnd = CreateWindowExW(exStyle, WIN7UAC_CLASS,
        info.windowTitle.empty() ? g_loc->title : info.windowTitle.c_str(), style, 0, 0, w, h,
        nullptr, nullptr, g_uiModule, nullptr);
    if (!g_ui.hwnd) {
        Log(L"CreateWindowEx failed -> fail open");
        RestoreNativePrompt();
        UiaRelease();
        CoUninitialize();
        return 0;
    }

    if (g_ui.shieldSmall) {
        SendMessageW(g_ui.hwnd, WM_SETICON, ICON_SMALL, (LPARAM)g_ui.shieldSmall);
        SendMessageW(g_ui.hwnd, WM_SETICON, ICON_BIG,
                     (LPARAM)(g_ui.shieldBig ? g_ui.shieldBig : g_ui.shieldSmall));
    }
    ApplyCaptionColour(g_ui.hwnd);
    ApplyWindowTheme(g_ui.hwnd);
    DWORD promptPid = 0;
    DWORD promptThread =
        info.hNative ? GetWindowThreadProcessId(info.hNative, &promptPid) : 0;
    g_ui.hStateHook = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                                      PromptStateProc, promptPid, promptThread,
                                      WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD);
    if (!g_ui.hStateHook)
        Log(L"SetWinEventHook failed (%u): the one second check is the only watch left",
            (unsigned)GetLastError());
    SetTimer(g_ui.hwnd, 1, 1000, nullptr);

    RECT wa = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
    RECT wr = {};
    GetWindowRect(g_ui.hwnd, &wr);
    SetWindowPos(g_ui.hwnd, HWND_TOPMOST,
                 wa.left + ((wa.right - wa.left) - (wr.right - wr.left)) / 2,
                 wa.top + ((wa.bottom - wa.top) - (wr.bottom - wr.top)) / 2, 0, 0,
                 SWP_NOSIZE | SWP_NOACTIVATE);
    g_step = L"session: taking over the native window";
    TakeOverNativePrompt(info.hNative);
    // Native operations are queued to its owner thread: never hold our state lock across a
    // synchronous cross-thread window move. Confirm that the move/hide actually took effect
    // before showing the replacement, with a short cancellable bound.
    DWORD hideStart = GetTickCount();
    while (!StopRequested() && NativePromptOnScreen() && GetTickCount() - hideStart < 500)
        if (StopWait(10)) break;
    if (StopRequested() || NativeCallFinished() || !IsWindow(info.hNative) || NativePromptOnScreen()) {
        Log(L"native prompt unavailable or still on screen -> no replacement");
        g_ui.result = RESULT_NO_DECISION;
        g_ui.done = true;
        DestroyWindow(g_ui.hwnd);
        if (g_ui.hStateHook) { UnhookWinEvent(g_ui.hStateHook); g_ui.hStateHook = nullptr; }
        GiveNativePromptBack();
        UiaRelease();
        CoUninitialize();
        return 0;
    }
    g_step = L"session: showing the Win7 dialog";
    ShowWindow(g_ui.hwnd, SW_SHOW);
    SetForegroundWindow(g_ui.hwnd);
    g_ui.shownAt = GetTickCount();
    {
        RECT cc = {};
        if (GetClientRect(g_ui.hwnd, &cc))
            Dbg(L"dialog: client %dx%d (layout base %dx%d), %d info row(s), %d details row(s)",
                (int)cc.right, (int)cc.bottom, g_ui.baseWidth, g_ui.baseHeight, RowCount(),
                (int)g_ui.details.size());
    }
    RegisterPanicHotkey();

    g_step = L"session: message loop";
    DWORD nextProbe = GetTickCount() + 1000;
    DWORD unusableSince = 0;
    MSG msg;
    for (;;) {
        DWORD wait = MsgWaitForMultipleObjectsEx(1, &g_stopEvent, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) {
            g_ui.result = RESULT_NO_DECISION;
            g_ui.done = true;
            if (g_ui.hwnd) DestroyWindow(g_ui.hwnd);
            break;
        }
        if (!PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) continue;
        if (msg.message == WM_QUIT) break;
        DWORD now = GetTickCount();
        if (msg.message == WM_W7UAC_STATECHANGED) nextProbe = 0;
        if (now >= nextProbe) {
            nextProbe = now + 1000;
            if (NativeCallFinished() || !IsWindow(info.hNative)) {
                Log(L"the consent flow ended while the Win7 dialog was up -> closing it");
                g_ui.result = RESULT_NO_DECISION;
                g_ui.done = true;
                PostMessageW(g_ui.hwnd, WM_CLOSE, 0, 0);
                continue;
            }
            bool buttonsUsable = NativeButtonsUsable(info);
            if (buttonsUsable) unusableSince = 0;
            else if (!unusableSince) unusableSince = now;
            if (!g_ui.done && g_settings.watchdogSeconds > 0 && unusableSince &&
                now - unusableSince >= (DWORD)g_settings.watchdogSeconds * 1000) {
                Log(L"watchdog: the native buttons are still not usable after %d s -> fail open",
                    g_settings.watchdogSeconds);
                g_ui.result = RESULT_NO_DECISION;
                g_ui.done = true;
                PostMessageW(g_ui.hwnd, WM_CLOSE, 0, 0);
                continue;
            }
            if (!g_ui.notice.empty() && (LONG)(now - g_ui.noticeUntil) >= 0) {
                g_ui.notice.clear();
                InvalidateRect(g_ui.hwnd, nullptr, TRUE);
            }
            if (g_settings.nativeWindowMode != 2 && NativePromptOnScreen()) {
                Dbg(L"the native prompt became visible again -> moving it away");
                TakeOverNativePrompt(info.hNative);
                if (g_ui.hwnd) SetForegroundWindow(g_ui.hwnd);
            }
        }
        if (msg.message == WM_HOTKEY && msg.wParam == IDC_W7UAC_PANIC) {
            Log(L"panic hotkey: closing the Windows 7 dialog and giving the native prompt back");
            g_ui.result = RESULT_NO_DECISION;
            g_ui.done = true;
            PostMessageW(g_ui.hwnd, WM_CLOSE, 0, 0);
            continue;
        }
        if (!IsDialogMessageW(g_ui.hwnd, &msg)) {
            if (msg.message == WM_KEYDOWN && (msg.wParam == VK_RETURN || msg.wParam == VK_ESCAPE)) {
                g_ui.result = IDC_W7UAC_NO;
                g_ui.done = true;
                PostMessageW(g_ui.hwnd, WM_CLOSE, 0, 0);
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (g_ui.hStateHook) {
        UnhookWinEvent(g_ui.hStateHook);
        g_ui.hStateHook = nullptr;
    }
    if (g_ui.hotkeyId) {
        UnregisterHotKey(nullptr, g_ui.hotkeyId);
        g_ui.hotkeyId = 0;
    }

    g_step = L"session: relay";
    const WCHAR* failReason = nullptr;
    if (StopRequested()) {
        failReason = L"mod stopping; no decision relayed";
    } else if (!infoOurs) {
        failReason = L"the prompt window is not ours";
    } else if (!ElementUsable(info, info.yes) || !ElementUsable(info, info.no)) {
        failReason = L"the native buttons were not identified, or the element list moved under "
                     L"them";
    } else if (g_ui.result == IDC_W7UAC_YES) {
        if (!info.yesEnabled) {
            failReason = L"the native Yes button is disabled";
        } else {
            bool relayed = RelayViaWin32(info.hNative, info.yes->name) || RelayViaUia(info.yes);
            if (!relayed && g_settings.yesHotkey[0]) {
                PostMessageW(info.hNative, WM_SYSKEYDOWN, (WPARAM)towupper(g_settings.yesHotkey[0]), 0);
                PostMessageW(info.hNative, WM_SYSKEYUP, (WPARAM)towupper(g_settings.yesHotkey[0]), 0);
                relayed = true;
            }
            if (!relayed) failReason = L"no relay mechanism available for Yes";
        }
    } else if (g_ui.result == IDC_W7UAC_NO) {
        bool relayed = RelayViaWin32(info.hNative, info.no->name) || RelayViaUia(info.no);
        if (!relayed && g_settings.noHotkey[0]) {
            PostMessageW(info.hNative, WM_SYSKEYDOWN, (WPARAM)towupper(g_settings.noHotkey[0]), 0);
            PostMessageW(info.hNative, WM_SYSKEYUP, (WPARAM)towupper(g_settings.noHotkey[0]), 0);
            relayed = true;
        }
        if (!relayed) failReason = L"no relay mechanism available for No";
    } else {
        failReason = L"the Windows 7 dialog was closed without a decision";
    }

    if (failReason) {
        Log(L"fail open: %s", failReason);
        RestoreNativePrompt();
    } else {
        DWORD t0 = GetTickCount();
        while (GetTickCount() - t0 < 8000 &&
               InterlockedCompareExchange(&g_originalReturned, 0, 0) == 0)
            if (StopWait(20)) break;
        if (InterlockedCompareExchange(&g_originalReturned, 0, 0) == 0) {
            Log(L"the native prompt did not finish after the relay -> restoring it (fail open)");
            RestoreNativePrompt();
        }
    }

    g_step = L"session: done";
    g_ui.done = true;
    g_ui.info = nullptr;
    UiaRelease();
    CoUninitialize();
    return 0;
}

// The last net of the session, and the only place a C++ exception can be seen: an allocation that
// fails, a library that throws on the way out. The ending is the one every other doubt produces -
// the native prompt is given back, nothing is decided, nothing stays hidden.
static DWORD WINAPI UiSessionRun() {
#if W7UAC_HAS_EXCEPTIONS
    try {
        return UiSessionRunInner();
    } catch (const std::exception& e) {
        Log(L"the replacement session threw a C++ exception (%S) -> the native prompt is given back",
            e.what() ? e.what() : "no message");
    } catch (...) {
        Log(L"the replacement session threw an unknown C++ exception -> the native prompt is given "
            L"back");
    }
    if (g_ui.hStateHook) {
        UnhookWinEvent(g_ui.hStateHook);
        g_ui.hStateHook = nullptr;
    }
    if (g_ui.hotkeyId) {
        UnregisterHotKey(nullptr, g_ui.hotkeyId);
        g_ui.hotkeyId = 0;
    }
    if (g_ui.hwnd && IsWindow(g_ui.hwnd) && GetCurrentThreadId() == GetWindowThreadProcessId(g_ui.hwnd, nullptr))
        DestroyWindow(g_ui.hwnd);
    g_ui.hwnd = nullptr;
    g_ui.done = true;
    RestoreNativePrompt();
    return 0;
#else
    return UiSessionRunInner();
#endif
}

// One replacement session at a time: a second prompt while ours is up is left native.
static volatile LONG g_uiRunning = 0;

static DWORD WINAPI UiThreadProcBody(LPVOID) {
    if (InterlockedCompareExchange(&g_uiRunning, 1, 0) != 0) {
        Log(L"a replacement session is already running -> leaving this prompt native");
        return 0;
    }
    DWORD r = UiSessionRun();
    InterlockedExchange(&g_uiRunning, 0);
    return r;
}

static DWORD WINAPI UiThreadProc(LPVOID param) {
#if W7UAC_HAS_EXCEPTIONS
    try {
        DWORD r = UiThreadProcBody(param);
        InterlockedDecrement(&g_threadsActive);
        return r;
    } catch (...) {
        // The session has a net of its own; this one is for what happens around it, and the prompt
        // is handed back here rather than left in the hands of a thread that is going away.
        Log(L"exception in the interface thread: giving the Windows prompt back");
        RestoreNativePrompt();
        InterlockedDecrement(&g_threadsActive);
        return 0;
    }
#else
    DWORD r = UiThreadProcBody(param);
    InterlockedDecrement(&g_threadsActive);
    return r;
#endif
}

//------------------------------------------------------------------------------
// Hook
//------------------------------------------------------------------------------

// Calls currently inside CredPromptWorkerHook. Unloading waits until none is left, because the
// original worker returns into the hook's own stack frame, which must still be mapped.
static volatile LONG g_hookCalls = 0;
struct HookCallGuard {
    HookCallGuard() { InterlockedIncrement(&g_hookCalls); }
    ~HookCallGuard() { InterlockedDecrement(&g_hookCalls); }
    HookCallGuard(const HookCallGuard&) = delete;
    HookCallGuard& operator=(const HookCallGuard&) = delete;
};
static void WaitForHookCalls() {
    bool logged = false;
    while (InterlockedCompareExchange(&g_hookCalls, 0, 0) > 0) {
        if (!logged) {
            logged = true;
            Log(L"unload: waiting for a call still inside the hook to return");
        }
        Sleep(10);
    }
}

// Undocumented worker ABI: retain the existing sixteen-argument forwarding hook on x64.
// This is not proof of the worker's real prototype. Do not replace it with another guessed
// argument count; a narrower hook requires per-build ABI evidence and Windows testing.
typedef ULONG (WINAPI *CredPromptWorker_t)(PVOID, PVOID, PVOID, PVOID, PVOID, PVOID, PVOID, PVOID,
                                           PVOID, PVOID, PVOID, PVOID, PVOID, PVOID, PVOID, PVOID);
static CredPromptWorker_t g_credPromptWorker_orig = nullptr;

static ULONG WINAPI CredPromptWorkerHook(PVOID a1, PVOID a2, PVOID a3, PVOID a4, PVOID a5,
                                         PVOID a6, PVOID a7, PVOID a8, PVOID a9, PVOID a10,
                                         PVOID a11, PVOID a12, PVOID a13, PVOID a14, PVOID a15,
                                         PVOID a16) {
    HookCallGuard hookGuard;
    if (StopRequested() || InterlockedCompareExchange(&g_inHook, 1, 0) != 0)
        return g_credPromptWorker_orig(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16);

    g_step = L"hook: prompt requested";
    InterlockedExchange(&g_originalReturned, 0);
    InterlockedExchange(&g_promptGivenBack, 0); // reset only at the start of a request
    g_nativeRectValid = false;

    if (g_settings.detection == 2) {
        // Hook-only mode: this exists purely to answer one question - does the native prompt
        // still come up with the mod loaded? Nothing is opened, nothing is enumerated, no
        // window is created; only the return value of the native call is logged.
        g_step = L"hook-only pass-through";
        DWORD tOnly = GetTickCount();
        ULONG rOnly = g_credPromptWorker_orig(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16);
        Log(L"hook-only mode: the native call returned (0x%08X) after %lu ms%s", rOnly,
            (unsigned long)(GetTickCount() - tOnly),
            rOnly == ERROR_INVALID_PARAMETER ? L" (ERROR_INVALID_PARAMETER)" : L"");
        if (rOnly != ERROR_SUCCESS && rOnly != ERROR_CANCELLED)
            Log(L"hook-only mode: the failure is the native code's own - none of the mod's "
                L"replacement logic ran: args %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p, "
                L"last error %u",
                a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16,
                (unsigned)GetLastError());
        InterlockedExchange(&g_inHook, 0);
        return rOnly;
    }

    g_requestTick = GetTickCount();
    Log(L"consent prompt requested (replaceUAC=%d)", g_settings.relayMode);
    EnterCriticalSection(&g_cs);
    g_candidates.clear();
    g_userDesktopApp.clear();
    g_userDesktopAppClass.clear();
    ResetCandidateLog();          // the log of the candidates starts over with the new prompt
    LeaveCriticalSection(&g_cs);
    InterlockedExchange(&g_userDesktopAppReady, 0);
    HANDLE hThread = nullptr;
    EnterCriticalSection(&g_cs);
    if (!StopRequested()) {
        if (g_watchers[0].tid) {
            PostThreadMessageW(g_watchers[0].tid, WM_PROMPT_STARTED, 0, 0);
        } else {
            // Polling mode: capture the identity on a thread of its own, so that the consent
            // thread (which still has to switch desktop) never makes a GUI call itself.
            InterlockedIncrement(&g_threadsActive);
            HANDLE hId = CreateThread(nullptr, 0, IdentityThreadProc, nullptr, 0, nullptr);
            if (hId) {
                // The handle is kept (see TrackModThread) instead of being closed here: this thread is
                // waited for before the DLL is unloaded. Closing the handle the moment it was created
                // was what made the unload path unable to see it at all.
                TrackModThread(hId, L"identity");
            } else {
                InterlockedDecrement(&g_threadsActive);
                Log(L"polling mode: the identity thread could not be started: %u", GetLastError());
            }
        }

        InterlockedIncrement(&g_threadsActive);
        hThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
        if (hThread) {
            InterlockedExchangePointer((PVOID volatile*)&g_uiThread, hThread);
            // Also in the registry, so the unload path waits for it even if the handle above has been
            // taken over by a later session.
            TrackModThread(hThread, L"interface");
        } else {
            InterlockedDecrement(&g_threadsActive);
            Log(L"the replacement UI thread could not be started");
        }

    }
    LeaveCriticalSection(&g_cs);

    g_step = L"hook: calling the original worker";
    DWORD tWorker = GetTickCount();
    ULONG result = g_credPromptWorker_orig(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16);

    g_step = L"hook: the original returned";
    InterlockedExchange(&g_originalReturned, 1);
    {
        const WCHAR* rname = ErrorCodeName(result);
        Log(L"the original call returned (0x%08X %s) after %lu ms", result,
            *rname ? rname : L"(no name for this code)",
            (unsigned long)(GetTickCount() - tWorker));
    }
    if (result == ERROR_CANCELLED) {
        // The ordinary "No", and not a failure of anything: either the user answered the native
        // prompt directly, or the answer this mod relayed was No.
        Dbg(L"the original call ended with ERROR_CANCELLED (0x000004C7): the prompt was answered "
            L"with No, or closed");
    } else if (result != ERROR_SUCCESS) {
        // The prompt's own code refused the call. Not one of the ten arguments was touched on
        // the way in, so they are written down here as they arrived - as addresses, never the
        // buffers behind them - with this thread's last error and the number of windows the
        // watcher has seen. That is what tells a prompt that never existed apart from one that
        // was created and then withdrawn.
        int seen = 0;
        EnterCriticalSection(&g_cs);
        seen = (int)g_candidates.size();
        LeaveCriticalSection(&g_cs);
        Log(L"the original call failed inside its own code: args %p %p %p %p %p %p %p %p %p %p, "
            L"last error %u, windows seen %d",
            a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, (unsigned)GetLastError(), seen);
        // Said plainly, because this is the one failure that has nothing to do with this mod:
        // Windows did not manage to create a prompt of its own, so there was nothing to replace
        // and nothing was answered. The line above is the code the error dialog shows.
        Log(L"Windows did not create a prompt of its own: the failure is inside Windows' own call, "
            L"before any of this mod's code ran. Nothing was replaced and nothing was answered. "
            L"If the same elevation fails with this mod disabled in Windhawk too, it is not this "
            L"mod.");
    }

    if (g_ui.hwnd && IsWindow(g_ui.hwnd)) {
        Log(L"the original call returned while our dialog is up -> closing it");
        PostMessageW(g_ui.hwnd, WM_CLOSE, 0, 0);
    }
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        // Deliberately not closed here. The handle belongs to the thread registry, which waits for
        // it and closes it once, at unload (WaitForModThreads). Closing it here as well closed the
        // same value twice: the second close lands on whatever handle the process reused in the
        // meantime. Clearing g_uiThread is enough - it only says "no session is running".
        InterlockedCompareExchangePointer((PVOID volatile*)&g_uiThread, nullptr, hThread);
    }
    g_step = L"hook: teardown";
    RestoreNativePrompt();
    { NativeLock lock; g_hNativePrompt = nullptr; }
    InterlockedExchange(&g_inHook, 0);
    return result;
}

BOOL Wh_ModInit() {
    g_step = L"mod init";
        // Initialize shared state before any failure branch, so the unload path can always enter the
    // lock even when a later setup step fails.
    InitializeCriticalSection(&g_cs);
    InitializeCriticalSection(&g_nativeCs);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return FALSE;
    auto failInit = []() -> BOOL {
        RemoveExceptionLogger();
        if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
        return FALSE;
    };
    LoadSettings();
    InitLocale();
    InstallNativeCreateHook();

    WCHAR deskName[64] = L"";
    GetDesktopName(GetThreadDesktop(GetCurrentThreadId()), deskName, ARRAYSIZE(deskName));
    Log(L"init: replaceUAC=%d appsScope=%d promptDetection=%d dumpDiagnostics=%d "
        L"process desktop='%s'",
        g_settings.relayMode, g_settings.whitelistMode, g_settings.detection,
        g_settings.dumpUiTree, deskName);

    // Said out loud, because it is the one setting that makes the mod look broken: in observe or
    // dump mode the native prompt is everything that will appear, by design. The log names the
    // setting that did it rather than leaving the next question open.
    if (g_settings.dumpUiTree || g_settings.relayMode == 0)
        Log(L"observe mode: %s -> the Windows 7 dialog will NOT be shown and the native prompt is "
            L"left alone; turn Replace the UAC prompt on and Write a diagnostics dump off to "
        L"replace the UI",
            g_settings.dumpUiTree ? L"Write a diagnostics dump is on"
                                  : L"Replace the UAC prompt is off");

    if (g_settings.debugLog) InstallExceptionLogger();

    HMODULE credui = LoadLibraryExW(L"credui.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!credui) {
        Log(L"credui.dll not loadable -> mod inactive");
        return failInit();
    }

    auto worker =
        (CredPromptWorker_t)GetProcAddress(credui, "CredUIPromptForWindowsCredentialsWorker");
    if (!worker) {
        Log(L"CredUIPromptForWindowsCredentialsWorker not exported -> mod inactive");
        FreeLibrary(credui);
        return failInit();
    }

    if (!Wh_SetFunctionHook((void*)worker, (void*)CredPromptWorkerHook,
                            (void**)&g_credPromptWorker_orig)) {
        Log(L"Wh_SetFunctionHook failed -> mod inactive");
        FreeLibrary(credui);
        return failInit();
    }
    Log(L"hooked credui!CredUIPromptForWindowsCredentialsWorker at %p", (void*)worker);


    // The threads are the last thing that starts, and only now that the hook is in place: every
    // branch above that gives up (credui.dll, the export, the hook itself) returns FALSE to a
    // process that is exactly as it was, with no thread of this mod running in it. A thread left
    // behind by a mod the loader is about to unmap is the one failure that cannot be survived.
    if (g_settings.detection == 0) {
        StartWatchers();
    } else {
        Log(L"detection = polling: nothing is opened at init, the desktops are looked at only "
            L"while a prompt is requested");
    }

    return TRUE;
}

// Windhawk's own thread, at any moment: the new snapshot is built there and published in one
// store, so no thread of the mod can be caught reading a half-written struct, and every limit is
// re-applied because the same FillSettings runs for a reload as for the first load. A change that
// cannot be read at all leaves the previous snapshot in place.
void Wh_ModSettingsChanged() {
    LoadSettings();
    Dbg(L"settings reloaded from a new snapshot (replaceUAC=%d)", g_settings.relayMode);
}

// Everything that ends the recreation and hands the prompt back, done here rather than at unload:
// this runs while the hooks are still installed, which is what makes it safe for the interface
// thread to be inside the relay of a click at this very moment. Windhawk calls it before
// Wh_ModUninit, and the hooks are removed only after both have returned.
void Wh_ModBeforeUninit() {
    Dbg(L"before uninit (last step: %s)", g_step);
    EnterCriticalSection(&g_cs);
    if (g_stopEvent) SetEvent(g_stopEvent);
    LeaveCriticalSection(&g_cs);

    // The stop event wakes the UI loop, which destroys the window without a decision.

    // Waited for, but not closed here: this handle is one of the tracked ones and is closed by
    // WaitForModThreads, so no handle is ever closed twice.
    HANDLE t = (HANDLE)InterlockedCompareExchangePointer((PVOID volatile*)&g_uiThread, nullptr,
                                                        nullptr);
    if (t && WaitForSingleObject(t, INFINITE) != WAIT_OBJECT_0)
        Log(L"the replacement UI thread did not stop in time");

    RestoreNativePrompt();
    WaitForHookCalls();   // the native prompt is back, so a pending call can finish
}

// The containers of the globals that carry [[clang::no_destroy]] are emptied here, once every
// thread of the mod has been waited for and the prompt has been given back: what the process exit
// would otherwise do under the loader lock is done here, on a thread that owns nothing else.
static void ReleaseGlobalState() {
    EnterCriticalSection(&g_cs);
    std::vector<PromptCandidate>().swap(g_candidates);
    std::vector<std::wstring>().swap(g_rejectLog);
    std::wstring().swap(g_userDesktopApp);
    std::wstring().swap(g_userDesktopAppClass);
    LeaveCriticalSection(&g_cs);
#if HAVE_UIA
    // Only the vector: its elements are interface pointers of the harvesting thread, which releases
    // them on its own thread (UiaRelease). Releasing them from here would be worse than letting the
    // process take them, and the process this runs in is on its way out either way.
    std::vector<IUIAutomationElement*>().swap(g_uiaRefs);
#endif
    std::wstring().swap(g_ui.notice);
    std::vector<std::wstring>().swap(g_ui.details);
    g_ui.info = nullptr;
}

void Wh_ModUninit() {
    Dbg(L"uninit (last step: %s)", g_step);
    WaitForHookCalls();

    // No interface work is done here: the window, the interface thread and the native prompt were
    // dealt with in Wh_ModBeforeUninit, while the hook was still in place.
    StopWatchers();
    WaitForModThreads();
    if (g_uiClassRegistered) {
        if (!UnregisterClassW(WIN7UAC_CLASS, g_uiModule))
            Log(L"UnregisterClassW failed: %u", GetLastError());
        g_uiClassRegistered = false;
    }
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
    ReleaseGlobalState();
    RemoveExceptionLogger();
    // Last: every GDI+ object of the dialog was disposed with it (FreeUiResources), so the library
    // can be stopped and unloaded here.
    GdiplusStop();
}
