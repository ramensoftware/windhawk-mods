// ==WindhawkMod==
// @id              game-mode-toggle
// @name            Game Mode Toggle
// @description     One hotkey to get ready to play: closes the apps you don't need and switches the power plan. Press it again to bring everything back exactly as it was.
// @version         1.0.0
// @author          theshindesahil
// @github          https://github.com/theshindesahil
// @include         windhawk.exe
// @compilerOptions -lpowrprof -lshell32 -lole32 -ldwmapi -ladvapi32 -luser32 -lgdi32 -lversion -lgdiplus
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Game Mode Toggle

Press **Ctrl+Alt+G** before you play: the mod shows what it is about to close,
you confirm, and your PC is ready. Press the hotkey again and everything comes
back: the apps are reopened, the power plan is restored, game priorities are put
back.

Nothing runs in the background while Game Mode is off. The mod only waits for
its hotkey, in a small separate process started by Windhawk.

## What it looks like

One window carries the whole run: it opens as a checklist, turns into a live
progress view while apps close, and ends as a summary.

![The checklist](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/flow-1-review.png)

![Apps closing](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/flow-2-running.png)

![The summary](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/flow-3-done.png)

### Themes

Pick one in the settings, together with the font and the window size.

| OLED purple (default) | OLED white |
|---|---|
| ![OLED purple (default)](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/theme-oled-purple.png) | ![OLED white](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/theme-oled-white.png) |

| OLED lime | OLED blue |
|---|---|
| ![OLED lime](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/theme-oled-lime.png) | ![OLED blue](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/theme-oled-blue.png) |

| Midnight | Graphite lime |
|---|---|
| ![Midnight](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/theme-midnight.png) | ![Graphite lime](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/theme-graphite-lime.png) |

| Violet night | Ember |
|---|---|
| ![Violet night](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/theme-violet-night.png) | ![Ember](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/theme-ember.png) |

| Mint | Paper light |
|---|---|
| ![Mint](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/theme-mint.png) | ![Paper light](https://raw.githubusercontent.com/theshindesahil/clear-the-deck/feat/windhawk-game-mode-mod/docs/game-mode-toggle/theme-paper-light.png) |

## What happens when you turn it on

1. **A fresh snapshot is taken**: the active power plan, every app it is about
   to close, and the priority of any running game.
2. **A window shows what will be closed**: every app is a card with its own icon
   and a switch. Untick anything to leave it alone this time, use *Select all* or
   *Select none* for long lists, then Enter confirms and Esc cancels. Long lists
   scroll. Turn the checklist off in the settings if you prefer.
3. **Apps are closed politely**, all at once: each app's visible windows get a
   normal close request, exactly like clicking the X. Hidden helper windows of
   tray apps are only touched when the app is allowed to be ended afterwards,
   so an app is never left running but broken (no tray icon, no hotkey). An app
   that asks "save changes?" stays open and is left alone. Apps that only hide to the tray when
   asked to close, and background apps with no window at all, are ended, but
   only the ones you left ticked in the checklist, and they are put back when
   you turn Game Mode off. Apps you list under *Apps to force close* are ended
   too.
4. **The power plan is switched**, only if you pick one in the settings
   (Ultimate Performance or High performance, if your PC has it). It is off by
   default.
5. **Games that are already running** (from your game folders) get High
   priority. Realtime is never used.
6. **The same window turns into a live progress view**: each app shows a spinner
   until it has closed, then a check mark (or an amber warning if it stayed
   open), with a progress bar underneath. When everything is done it becomes a
   summary with a Done button, and it closes itself after a while. The progress
   view can be switched off in the settings.

The window can be restyled in the settings: **Theme** (OLED black with purple,
white, lime or blue accents, Midnight, Graphite lime, Violet night, Ember, Mint,
Paper light, or follow Windows dark/light), **Font** (Geist by default, with
a fallback to a Windows font when it is not installed, or any installed font
you type in) and **Window size** (automatic, or 100% to 250%).

## What happens when you turn it off

Exactly the snapshot is restored, then deleted: apps that were closed are
reopened, the power plan goes back to what it was, priorities are reset. There
is no stored "original": changes you make while Game Mode is off simply stay.
Apps you reopened yourself in the meantime are not started twice.
By default the apps are reopened **in the background**: they start minimized and
do not take the focus, so nothing flashes over what you are doing. An app that
ignores the request to start minimized is minimized the moment its window
appears, for a few seconds after it was started. Turn *Reopen apps in the
background* off in the settings if you want them to open normally.

## Which apps are closed

- **Apps to close**: process names you list (for example `Rainmeter.exe`). Every
  process with that name is asked to close, tray apps included.
- **Close every other app too** (on by default): every other program of yours
  that is not needed for gaming, whether it has a taskbar window or only runs in
  the background (tray apps, launchers, sync clients). Helper processes are
  grouped with the app that started them. Never touched: Windows itself,
  Explorer, Windhawk and its editor, console tools (git, node, shells), drivers
  and GPU software in the *Folders to keep* list, anything under *Apps to keep*,
  anything running from your *Game folders*, and administrator apps, which a
  normal program cannot close anyway. Tools made by your PC's manufacturer or
  by hardware vendors (Acer, Lenovo, HP, Dell, Intel, NVIDIA, Realtek and
  others, see *Manufacturers to keep*) are kept by default.
- Store apps (Phone Link, WhatsApp, Files, ...) are reopened through Windows
  with their app identity, other apps with their original command line. A web
  link a browser was started with is not replayed.

## Safety

- Every app is asked to close politely first. An app is ended (not just asked)
  only when it is in *Apps to force close*, or when it has no window, or hid its
  window to the tray after being asked, and you left it ticked in the checklist.
  The checklist opens with the full list of names, so you see each one before
  you confirm. With the checklist off nothing is ended, only asked to close.
- The power plan is a system-wide setting that Windows keeps across restarts.
  It is only changed if you pick a plan in the settings. It is restored when
  you turn Game Mode off, when the mod is disabled or removed, and about 90
  seconds after the mod starts following a restart or crash. If Windhawk is
  killed outright while Game Mode is on, the plan stays changed until then.
- An unreadable or half-written snapshot is discarded with a notification
  instead of blocking the hotkey.
- If the PC restarted or crashed while Game Mode was on, the leftover snapshot
  is restored about 90 seconds after the mod starts, so Windows startup apps
  come up first and nothing opens twice.
- Elevated (administrator) apps cannot be closed by a normal-privilege tool;
  they are left running.

## Limits

- Windows services (a local database, for instance) need administrator rights
  and are not touched.
- The GPU mode (hybrid or discrete-only) is a firmware/driver setting and is not
  changed.
- Do not run it from inside an app that is on the close list: that app is
  closed, with whatever it was doing.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: oled-purple
  $name: Theme
  $description: >-
    The colours of the Game Mode window. "Follow Windows" uses OLED purple when
    Windows is in dark mode and Paper light when it is in light mode.
  $options:
    - auto: Follow Windows (dark or light)
    - oled-purple: OLED black, purple accent
    - oled-white: OLED black, white accent
    - oled-lime: OLED black, lime accent
    - oled-blue: OLED black, blue accent
    - midnight: Midnight (charcoal, blue)
    - graphite-lime: Graphite lime
    - violet-night: Violet night
    - ember: Ember (warm brown, orange)
    - mint: Mint
    - paper-light: Paper light
- font: Geist
  $name: Font
  $description: >-
    The font of the Game Mode window. If it is not installed on this PC, Segoe UI
    Variable Text is used instead. Geist is a free font you can install.
  $options:
    - Geist: Geist
    - Geist Mono: Geist Mono
    - Segoe UI Variable Text: Segoe UI Variable (Windows 11 default)
    - Segoe UI: Segoe UI
    - Cascadia Code: Cascadia Code
    - Cascadia Mono: Cascadia Mono
    - JetBrains Mono: JetBrains Mono
    - Consolas: Consolas
    - Inter: Inter
- customFont: ""
  $name: Custom font name
  $description: >-
    Optional. Type the name of any installed font to use it instead of the one
    chosen above. Leave empty to use the list.
- uiScale: auto
  $name: Window size
  $description: >-
    "Automatic" follows the display scale of the monitor the window is on. Pick a
    percentage to make the window larger or smaller on every screen.
  $options:
    - auto: Automatic
    - 100: 100%
    - 125: 125%
    - 150: 150%
    - 175: 175%
    - 200: 200%
    - 250: 250%
- confirm: true
  $name: Show the checklist before turning on and off
  $description: >-
    Shows the apps as a checklist first. Untick an app to leave it alone this
    time. Enter confirms, Esc cancels. With this off nothing is ended, only
    closed politely.
- showProgress: true
  $name: Show a progress window
  $description: >-
    Shows a small window with a progress bar and the apps being closed or
    reopened, then a summary when it is done. With it off, only the
    notification below is shown.
- notify: true
  $name: Show a notification when done
- hotkey: Ctrl+Alt+G
  $name: Hotkey
  $description: >-
    Press it once to turn Game Mode on, again to turn it off. Use Ctrl, Alt,
    Shift or Win plus a letter, digit or F1-F24, for example Ctrl+Alt+G.
- gamePowerPlan: none
  $name: Power plan while playing
  $options:
    - none: Leave the power plan alone
    - ultimate: Ultimate Performance
    - high: High performance
  $description: >-
    Off by default. Changes the active Windows power plan, a system-wide
    setting that survives restarts. If the plan does not exist on this PC, the
    power plan is left alone. The plan that was active is restored when Game
    Mode is turned off, when the mod is unloaded, or after a restart.
- reopenInBackground: true
  $name: Reopen apps in the background
  $description: >-
    When Game Mode is turned off, apps are started minimized and without taking
    the focus, so nothing flashes over what you are doing. Some apps ignore the
    request to start minimized; they are minimized the moment their window
    appears. Turn it off to let apps open normally.
- boostGames: true
  $name: High priority for running games
  $description: >-
    Only applies to games that are already running when you turn Game Mode on.
- closeOthers: true
  $name: Close every other app too
  $description: >-
    Also close every other app that is not needed for gaming, except Windows
    itself, Windhawk and the lists below. The checklist shows the full list.
- closeBackground: true
  $name: Include background and tray apps
  $description: >-
    Also list apps that only run in the background without a window. They are
    ended (not just asked to close) when you leave them ticked.
- closeApps: [""]
  $name: Apps to close
  $description: >-
    Process names, one per row, for example Rainmeter.exe. All processes with
    that name are asked to close, including tray apps without a window.
- keepApps: [steam.exe, EpicGamesLauncher.exe, Battle.net.exe, GalaxyClient.exe, Discord.exe, obs64.exe, WindowsTerminal.exe]
  $name: Apps to keep
  $description: >-
    Process names that are never closed, even with "Close every other app too".
    Add your game launchers, voice chat and recording tools here.
- keepVendorApps: true
  $name: Keep PC manufacturer and hardware utilities
  $description: >-
    Never close programs made by the companies listed below, such as Acer,
    Lenovo, HP and Dell system tools, RGB and fan control, GPU and audio
    software. Matched on the company name stored in the program's file.
- keepVendors: [Acer, ULIC, Predator, Nitro, Lenovo, Legion, Hewlett-Packard, HP Inc, OMEN, Dell, Alienware, ASUS, ROG, Armoury, Micro-Star, MSI, Gigabyte, Razer, Logitech, Corsair, SteelSeries, Intel, NVIDIA, AMD, Advanced Micro Devices, Realtek, Qualcomm, Rivet Networks, Killer]
  $name: Manufacturers to keep
  $description: >-
    Company, brand or product names, any capitalization. A program is kept when
    one of them starts a word in its company name, its product name or the
    folder it is installed in under Program Files. Names of 3 letters or fewer
    must match a whole word. Add or remove names to suit your PC.
- keepFolders: [\NVIDIA Corporation\, \NVIDIA\, \AMD\, \Intel\, .IntelGraphicsExperience, \Realtek, \Windhawk\, \WindowsApps\MicrosoftWindows., \WindowsApps\Microsoft.Widgets, \WindowsApps\Microsoft.DesktopAppInstaller, \EdgeWebView\]
  $name: Folders to keep
  $description: >-
    Programs whose path contains any of these pieces of text are never closed.
    Use it for drivers, GPU software and tools you always want running.
- forceCloseApps: [""]
  $name: Apps to force close
  $description: >-
    Listed apps that ignore the normal close request can be ended after the
    wait below. Unsaved data in these apps is lost. Leave empty to never force.
- closeTimeoutSec: 8
  $name: Seconds to wait for apps to close
  $description: >-
    How long apps get to close on their own before the remaining ones are
    reported as still running (or ended, if they are in the force list).
- gameFolders: [""]
  $name: Game folders
  $description: >-
    Programs running from these folders are never closed and get High
    priority, for example D:\SteamLibrary\steamapps\common. Subfolders count.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <powrprof.h>
#include <sddl.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <tlhelp32.h>

#include <algorithm>
#include <cstdlib>
#include <cwctype>
#include <cmath>
#include <cwctype>
#include <map>
#include <set>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Pure logic. No Windows calls: this part is unit-tested outside Windhawk.

struct Settings {
    UINT hotMods = MOD_CONTROL | MOD_ALT;
    UINT hotVk = 'G';
    bool hotkeyValid = true;
    bool confirm = true;
    bool closeOthers = true;
    bool closeBackground = true;
    bool boostGames = true;
    bool notify = true;
    bool showProgress = false;  // the settings load turns it on by default
    std::wstring fontName;      // empty = a modern Windows font
    std::wstring theme = L"oled-purple";
    float uiScale = 0;          // 0 = follow the display scale
    bool reopenBackground = false;   // the settings load turns it on by default
    std::wstring plan = L"none";
    int timeoutSec = 8;
    std::vector<std::wstring> closeApps;
    std::vector<std::wstring> keepApps;
    std::vector<std::wstring> keepFolders;  // pieces of text a path may contain
    bool keepVendorApps = true;
    std::vector<std::wstring> keepVendors;  // company names (start of a word)
    std::vector<std::wstring> forceApps;
    std::vector<std::wstring> gameFolders;
};

static std::wstring Lower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n\"");
    if (a == std::wstring::npos) {
        return L"";
    }
    size_t b = s.find_last_not_of(L" \t\r\n\"");
    return s.substr(a, b - a + 1);
}

// Whitespace only: unlike Trim, it keeps quotes, which are part of arguments.
static std::wstring TrimSpace(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) {
        return L"";
    }
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

static std::wstring BaseName(const std::wstring& path) {
    size_t i = path.find_last_of(L"\\/");
    return i == std::wstring::npos ? path : path.substr(i + 1);
}

// "Rainmeter" and "Rainmeter.exe" both name the same process.
static std::wstring ExeKey(const std::wstring& name) {
    std::wstring n = Lower(Trim(name));
    if (n.size() > 4 && n.compare(n.size() - 4, 4, L".exe") == 0) {
        n.resize(n.size() - 4);
    }
    return n;
}

static bool InNameList(const std::vector<std::wstring>& list,
                       const std::wstring& exeName) {
    std::wstring key = ExeKey(exeName);
    for (const auto& item : list) {
        if (ExeKey(item) == key) {
            return true;
        }
    }
    return false;
}

static bool InFolder(const std::wstring& path, const std::wstring& folder) {
    std::wstring f = Lower(Trim(folder));
    while (!f.empty() && (f.back() == L'\\' || f.back() == L'/')) {
        f.pop_back();
    }
    if (f.empty()) {
        return false;
    }
    std::wstring p = Lower(path);
    return p.size() > f.size() && p.compare(0, f.size(), f) == 0 &&
           (p[f.size()] == L'\\' || p[f.size()] == L'/');
}

static bool InAnyFolder(const std::wstring& path,
                        const std::vector<std::wstring>& folders) {
    for (const auto& f : folders) {
        if (InFolder(path, f)) {
            return true;
        }
    }
    return false;
}

// "Ctrl+Alt+G" -> modifiers + virtual key. At least one modifier and exactly
// one key are required, so a bare letter can never hijack typing.
static bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    UINT m = 0;
    UINT k = 0;
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t plus = text.find(L'+', pos);
        std::wstring tok = Lower(Trim(text.substr(
            pos, plus == std::wstring::npos ? std::wstring::npos : plus - pos)));
        pos = plus == std::wstring::npos ? text.size() + 1 : plus + 1;
        if (tok.empty()) {
            return false;
        }
        if (tok == L"ctrl" || tok == L"control") {
            m |= MOD_CONTROL;
        } else if (tok == L"alt") {
            m |= MOD_ALT;
        } else if (tok == L"shift") {
            m |= MOD_SHIFT;
        } else if (tok == L"win" || tok == L"windows") {
            m |= MOD_WIN;
        } else if (k != 0) {
            return false;
        } else if (tok.size() == 1 && ((tok[0] >= L'a' && tok[0] <= L'z') ||
                                       (tok[0] >= L'0' && tok[0] <= L'9'))) {
            k = (UINT)towupper(tok[0]);
        } else if (tok.size() >= 2 && tok.size() <= 3 && tok[0] == L'f') {
            int n = _wtoi(tok.c_str() + 1);
            if (n < 1 || n > 24) {
                return false;
            }
            k = (UINT)(VK_F1 + n - 1);
        } else if (tok == L"space") {
            k = VK_SPACE;
        } else {
            return false;
        }
    }
    if (m == 0 || k == 0) {
        return false;
    }
    *mods = m;
    *vk = k;
    return true;
}

// What follows the executable in a full command line. A link handler start
// ("firefox.exe -url https://...") is not a way to start the app, so such
// arguments are dropped.
static std::wstring ArgsFromCommandLine(const std::wstring& cl) {
    const std::wstring& s = cl;
    size_t i = 0;
    while (i < s.size() && s[i] == L' ') {
        i++;
    }
    size_t end;
    if (i < s.size() && s[i] == L'"') {
        end = s.find(L'"', i + 1);
        if (end == std::wstring::npos) {
            return L"";
        }
        end++;
    } else {
        end = s.find(L' ', i);
        if (end == std::wstring::npos) {
            return L"";
        }
    }
    std::wstring args = TrimSpace(s.substr(end));
    if (args.find(L"://") != std::wstring::npos) {
        return L"";
    }
    return args;
}

// Processes that are part of the desktop itself.
static bool IsShellProcess(const std::wstring& exeName) {
    static const wchar_t* const kShell[] = {
        L"explorer",
        L"windhawk",
        L"applicationframehost",
        L"shellexperiencehost",
        L"startmenuexperiencehost",
        L"searchhost",
        L"textinputhost",
        L"lockapp",
        L"securityhealthsystray",
        L"msedgewebview2",
        L"widgets",
        L"ctfmon",
        L"dwm",
    };
    std::wstring key = ExeKey(exeName);
    for (const wchar_t* k : kShell) {
        if (key == k) {
            return true;
        }
    }
    return false;
}

// Why an app found through its window must not be closed, or nullptr if it may.
static const wchar_t* WhyKeep(const std::wstring& exeName,
                              const std::wstring& path,
                              const std::wstring& windowsDir,
                              const Settings& s) {
    if (IsShellProcess(exeName)) {
        return L"part of Windows";
    }
    if (!windowsDir.empty() && InFolder(path, windowsDir)) {
        return L"part of Windows";
    }
    if (InNameList(s.keepApps, exeName)) {
        return L"in Apps to keep";
    }
    if (InAnyFolder(path, s.gameFolders)) {
        return L"in a game folder";
    }
    std::wstring lowerPath = Lower(path);
    for (const auto& piece : s.keepFolders) {
        std::wstring p = Lower(Trim(piece));
        if (!p.empty() && lowerPath.find(p) != std::wstring::npos) {
            return L"in Folders to keep";
        }
    }
    return nullptr;
}

// True when any listed vendor name appears in `text` at the start of a word
// ("Acer Incorporated" matches Acer; "Racer Games" does not). Case is ignored.
static bool VendorMatches(const std::wstring& text,
                          const std::vector<std::wstring>& vendors) {
    std::wstring t = Lower(text);
    for (const auto& vendor : vendors) {
        std::wstring v = Lower(Trim(vendor));
        if (v.empty()) {
            continue;
        }
        // Very short names (MSI, AMD, ROG) must be a whole word: "rogue" is not ROG.
        const bool wholeWord = v.size() <= 3;
        for (size_t pos = t.find(v); pos != std::wstring::npos; pos = t.find(v, pos + 1)) {
            const bool startOk = pos == 0 || !iswalnum(t[pos - 1]);
            const size_t end = pos + v.size();
            const bool endOk = !wholeWord || end >= t.size() || !iswalnum(t[end]);
            if (startOk && endOk) {
                return true;
            }
        }
    }
    return false;
}

// The folder a program was installed into under Program Files or WindowsApps
// ("Acer" for C:\Program Files\Acer\Care Center\x.exe). Only this level is used
// to recognise a maker, never the whole path, so a user account named "Dell"
// cannot make everything it owns look like a Dell tool.
static std::wstring InstallFolderName(const std::wstring& path) {
    std::wstring p = Lower(path);
    // WindowsApps first: it lives inside Program Files.
    for (const wchar_t* root : {L"\\windowsapps\\", L"\\program files (x86)\\", L"\\program files\\"}) {
        size_t at = p.find(root);
        if (at == std::wstring::npos) {
            continue;
        }
        size_t start = at + wcslen(root);
        size_t end = p.find(L'\\', start);
        return p.substr(start, end == std::wstring::npos ? std::wstring::npos : end - start);
    }
    return L"";
}

// Helper programs of an installer or updater are never the app itself.
static bool LooksLikeHelper(const std::wstring& exeName) {
    std::wstring n = Lower(exeName);
    for (const wchar_t* part : {L"update", L"crash", L"setup", L"install", L"uninst"}) {
        if (n.find(part) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

// ---- snapshot text format --------------------------------------------------

struct SavedApp {
    std::wstring name, path, args, aumid;
};

struct SavedPriority {
    DWORD pid = 0;
    std::wstring exe;
    DWORD priorityClass = 0;
};

struct Snapshot {
    unsigned long long startedAt = 0;  // FILETIME, 100 ns ticks
    std::wstring plan;                 // GUID text, empty = not changed
    std::wstring logon;                // id of the sign-in that wrote it, may be empty
    std::vector<SavedApp> apps;
    std::vector<SavedPriority> prios;
};

static std::wstring Esc(const std::wstring& s) {
    std::wstring o;
    for (wchar_t c : s) {
        if (c == L'\\') {
            o += L"\\\\";
        } else if (c == L'\t') {
            o += L"\\t";
        } else if (c == L'\n') {
            o += L"\\n";
        } else if (c == L'\r') {
            o += L"\\r";
        } else {
            o += c;
        }
    }
    return o;
}

static std::wstring Unesc(const std::wstring& s) {
    std::wstring o;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == L'\\' && i + 1 < s.size()) {
            wchar_t n = s[++i];
            o += n == L't' ? L'\t' : n == L'n' ? L'\n' : n == L'r' ? L'\r' : n;
        } else {
            o += s[i];
        }
    }
    return o;
}

static std::wstring SerializeSnapshot(const Snapshot& sn) {
    std::wstring o = L"GMT1\n";
    o += L"started\t" + std::to_wstring(sn.startedAt) + L"\n";
    o += L"plan\t" + Esc(sn.plan) + L"\n";
    if (!sn.logon.empty()) {
        o += L"logon\t" + Esc(sn.logon) + L"\n";
    }
    for (const auto& a : sn.apps) {
        o += L"app\t" + Esc(a.name) + L"\t" + Esc(a.path) + L"\t" + Esc(a.args) +
             L"\t" + Esc(a.aumid) + L"\n";
    }
    for (const auto& p : sn.prios) {
        o += L"prio\t" + std::to_wstring(p.pid) + L"\t" + Esc(p.exe) + L"\t" +
             std::to_wstring(p.priorityClass) + L"\n";
    }
    o += L"end\n";
    return o;
}

static std::vector<std::wstring> SplitTabs(const std::wstring& line) {
    std::vector<std::wstring> f;
    size_t pos = 0;
    while (true) {
        size_t t = line.find(L'\t', pos);
        if (t == std::wstring::npos) {
            f.push_back(line.substr(pos));
            break;
        }
        f.push_back(line.substr(pos, t - pos));
        pos = t + 1;
    }
    return f;
}

// Fails on anything that is not a complete, current-format snapshot, so a
// half-written file can never be half-applied.
static bool ParseSnapshot(const std::wstring& text, Snapshot* out) {
    Snapshot sn;
    size_t pos = 0;
    bool first = true;
    bool sawStarted = false;
    bool sawEnd = false;
    while (pos < text.size()) {
        size_t nl = text.find(L'\n', pos);
        std::wstring line = text.substr(
            pos, nl == std::wstring::npos ? std::wstring::npos : nl - pos);
        pos = nl == std::wstring::npos ? text.size() : nl + 1;
        if (sawEnd) {
            return false;  // data after the end marker
        }
        if (first) {
            if (line != L"GMT1") {
                return false;
            }
            first = false;
            continue;
        }
        auto f = SplitTabs(line);
        if (f[0] == L"started" && f.size() == 2) {
            wchar_t* e = nullptr;
            sn.startedAt = _wcstoui64(f[1].c_str(), &e, 10);
            if (!e || *e || sn.startedAt == 0) {
                return false;
            }
            sawStarted = true;
        } else if (f[0] == L"plan" && f.size() == 2) {
            sn.plan = Unesc(f[1]);
        } else if (f[0] == L"logon" && f.size() == 2) {
            sn.logon = Unesc(f[1]);
        } else if (f[0] == L"app" && f.size() == 5) {
            sn.apps.push_back(
                {Unesc(f[1]), Unesc(f[2]), Unesc(f[3]), Unesc(f[4])});
        } else if (f[0] == L"prio" && f.size() == 4) {
            SavedPriority p;
            p.pid = (DWORD)wcstoul(f[1].c_str(), nullptr, 10);
            p.exe = Unesc(f[2]);
            p.priorityClass = (DWORD)wcstoul(f[3].c_str(), nullptr, 10);
            sn.prios.push_back(p);
        } else if (f[0] == L"end" && f.size() == 1) {
            sawEnd = true;
        } else {
            return false;
        }
    }
    if (first || !sawStarted || !sawEnd) {
        return false;
    }
    *out = sn;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Windows side.

namespace {

constexpr UINT WM_APP_RELOAD = WM_APP + 1;
constexpr UINT WM_APP_STOP = WM_APP + 2;
constexpr int kHotkeyId = 0x6D47;
UINT_PTR g_recoverTimer;
constexpr DWORD kRecoverDelayMs = 90 * 1000;
const wchar_t kGuidBalanced[] = L"381b4222-f694-41f0-9685-ff5bb260df2e";
const wchar_t kGuidUltimate[] = L"e9a42b02-d5df-448d-aa00-03f14749eb61";
const wchar_t kGuidHigh[] = L"8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c";

Settings g_settings;
std::wstring g_windowsDir;
HANDLE g_thread;
DWORD g_threadId;
HANDLE g_threadReady;
bool g_hotkeyRegistered;
bool g_busy;

// ---- settings --------------------------------------------------------------

std::vector<std::wstring> ReadList(PCWSTR name) {
    std::vector<std::wstring> list;
    for (int i = 0; i < 200; i++) {
        PCWSTR v = Wh_GetStringSetting(name, i);
        std::wstring s = v;
        Wh_FreeStringSetting(v);
        s = Trim(s);
        if (s.empty()) {
            break;
        }
        list.push_back(s);
    }
    return list;
}

void LoadSettings() {
    Settings s;
    PCWSTR hk = Wh_GetStringSetting(L"hotkey");
    std::wstring hotkeyText = hk;
    Wh_FreeStringSetting(hk);
    s.hotkeyValid = ParseHotkey(hotkeyText, &s.hotMods, &s.hotVk);
    if (!s.hotkeyValid) {
        Wh_Log(L"Invalid hotkey \"%s\"", hotkeyText.c_str());
    }
    s.confirm = Wh_GetIntSetting(L"confirm") != 0;
    s.closeOthers = Wh_GetIntSetting(L"closeOthers") != 0;
    s.closeBackground = Wh_GetIntSetting(L"closeBackground") != 0;
    s.boostGames = Wh_GetIntSetting(L"boostGames") != 0;
    s.notify = Wh_GetIntSetting(L"notify") != 0;
    s.showProgress = Wh_GetIntSetting(L"showProgress") != 0;
    s.reopenBackground = Wh_GetIntSetting(L"reopenInBackground") != 0;
    PCWSTR fontSetting = Wh_GetStringSetting(L"font");
    s.fontName = Trim(fontSetting);
    Wh_FreeStringSetting(fontSetting);
    PCWSTR customFont = Wh_GetStringSetting(L"customFont");
    if (!Trim(customFont).empty()) {
        s.fontName = Trim(customFont);   // a font typed in by hand wins over the list
    }
    Wh_FreeStringSetting(customFont);
    PCWSTR themeSetting = Wh_GetStringSetting(L"theme");
    s.theme = Lower(Trim(themeSetting));
    Wh_FreeStringSetting(themeSetting);
    PCWSTR scaleSetting = Wh_GetStringSetting(L"uiScale");
    int scalePercent = _wtoi(scaleSetting);   // "auto" reads as 0
    s.uiScale = scalePercent >= 100 ? scalePercent / 100.0f : 0.0f;
    Wh_FreeStringSetting(scaleSetting);
    PCWSTR plan = Wh_GetStringSetting(L"gamePowerPlan");
    s.plan = Lower(plan);
    Wh_FreeStringSetting(plan);
    s.timeoutSec = std::clamp(Wh_GetIntSetting(L"closeTimeoutSec"), 1, 60);
    s.closeApps = ReadList(L"closeApps[%d]");
    s.keepApps = ReadList(L"keepApps[%d]");
    s.keepFolders = ReadList(L"keepFolders[%d]");
    s.keepVendorApps = Wh_GetIntSetting(L"keepVendorApps") != 0;
    s.keepVendors = ReadList(L"keepVendors[%d]");
    s.forceApps = ReadList(L"forceCloseApps[%d]");
    s.gameFolders = ReadList(L"gameFolders[%d]");
    g_settings = s;
}

// ---- small helpers ---------------------------------------------------------

unsigned long long NowFileTime() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}

std::wstring GuidToString(const GUID& g) {
    WCHAR buf[64];
    StringFromGUID2(g, buf, ARRAYSIZE(buf));
    std::wstring s = Lower(buf);
    return s.substr(1, s.size() - 2);  // without braces
}

bool GuidFromString(const std::wstring& text, GUID* g) {
    std::wstring braced = L"{" + text + L"}";
    return SUCCEEDED(CLSIDFromString(braced.c_str(), g));
}

// The storage folder is shared by every Windows account on the PC, so each
// account gets its own snapshot file, named after its SID.
std::wstring UserSidText() {
    std::wstring out;
    HANDLE tok = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tok)) {
        return L"unknown";
    }
    DWORD need = 0;
    GetTokenInformation(tok, TokenUser, nullptr, 0, &need);
    std::vector<BYTE> buf(need ? need : 1);
    if (need && GetTokenInformation(tok, TokenUser, buf.data(), need, &need)) {
        LPWSTR text = nullptr;
        if (ConvertSidToStringSidW(((TOKEN_USER*)buf.data())->User.Sid, &text)) {
            out = text;
            LocalFree(text);
        }
    }
    CloseHandle(tok);
    return out.empty() ? L"unknown" : out;
}

// Changes at every sign-in (also after a restart with Fast Startup, which
// keeps the tick count running), so a snapshot from an earlier sign-in is
// recognisable as left over.
std::wstring CurrentLogonId() {
    std::wstring out;
    HANDLE tok = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tok)) {
        TOKEN_STATISTICS st;
        DWORD got = 0;
        if (GetTokenInformation(tok, TokenStatistics, &st, sizeof(st), &got)) {
            WCHAR b[40];
            wsprintfW(b, L"%08lx%08lx", (unsigned long)st.AuthenticationId.HighPart,
                      (unsigned long)st.AuthenticationId.LowPart);
            out = b;
        }
        CloseHandle(tok);
    }
    return out;
}

std::wstring SnapshotPath() {
    WCHAR dir[MAX_PATH];
    if (Wh_GetModStoragePath(dir, ARRAYSIZE(dir)) == 0) {
        return L"";
    }
    CreateDirectoryW(dir, nullptr);
    return std::wstring(dir) + L"\\snapshot-" + UserSidText() + L".txt";
}

bool ReadFileText(const std::wstring& path, std::wstring* text) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    LARGE_INTEGER size;
    std::string bytes;
    bool ok = GetFileSizeEx(h, &size) && size.QuadPart < (16 << 20);
    if (ok) {
        bytes.resize((size_t)size.QuadPart);
        DWORD got = 0;
        ok = ReadFile(h, bytes.data(), (DWORD)bytes.size(), &got, nullptr) &&
             got == bytes.size();
    }
    CloseHandle(h);
    if (!ok) {
        return false;
    }
    int n = MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                                nullptr, 0);
    text->assign(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                        text->data(), n);
    return true;
}

// Written to a temp file and renamed over the real one, so a crash in the
// middle leaves the old snapshot or the new one, never half of one.
bool WriteSnapshot(const Snapshot& sn) {
    std::wstring path = SnapshotPath();
    if (path.empty()) {
        return false;
    }
    std::wstring text = SerializeSnapshot(sn);
    int n = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(),
                                nullptr, 0, nullptr, nullptr);
    std::string bytes(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), bytes.data(),
                        n, nullptr, nullptr);
    std::wstring tmp = path + L".tmp";
    HANDLE h = CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD wrote = 0;
    bool ok =
        WriteFile(h, bytes.data(), (DWORD)bytes.size(), &wrote, nullptr) &&
        wrote == bytes.size() && FlushFileBuffers(h);
    CloseHandle(h);
    if (!ok) {
        DeleteFileW(tmp.c_str());
        return false;
    }
    return MoveFileExW(tmp.c_str(), path.c_str(),
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
}

enum class LoadResult { None, Ok, Corrupt };

LoadResult LoadSnapshot(Snapshot* sn) {
    std::wstring path = SnapshotPath();
    if (path.empty() ||
        GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return LoadResult::None;
    }
    std::wstring text;
    if (!ReadFileText(path, &text) || !ParseSnapshot(text, sn)) {
        return LoadResult::Corrupt;
    }
    return LoadResult::Ok;
}

void DeleteSnapshot() {
    std::wstring path = SnapshotPath();
    if (!path.empty()) {
        DeleteFileW(path.c_str());
    }
}

// ---- notification and confirmation -----------------------------------------

HWND g_notifyWnd;
constexpr UINT_PTR kNotifyTimer = 1;

LRESULT CALLBACK NotifyProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_TIMER && wp == kNotifyTimer) {
        KillTimer(hwnd, kNotifyTimer);
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd = hwnd;
        nid.uID = 1;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void Notify(const std::wstring& text) {
    if (!g_settings.notify) {
        return;
    }
    if (!g_notifyWnd) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = NotifyProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"GameModeToggleNotify";
        RegisterClassW(&wc);
        g_notifyWnd =
            CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0,
                            HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
        if (!g_notifyWnd) {
            return;
        }
    }
    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd = g_notifyWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO;
    nid.hIcon = LoadIconW(nullptr, IDI_INFORMATION);
    wcsncpy(nid.szTip, L"Game Mode", ARRAYSIZE(nid.szTip) - 1);
    wcsncpy(nid.szInfoTitle, L"Game Mode", ARRAYSIZE(nid.szInfoTitle) - 1);
    wcsncpy(nid.szInfo, text.c_str(), ARRAYSIZE(nid.szInfo) - 1);
    nid.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    Shell_NotifyIconW(NIM_ADD, &nid);
    SetTimer(g_notifyWnd, kNotifyTimer, 8000, nullptr);
}

ULONG_PTR g_gdiplusToken = 0;

void GdiplusOn() {
    if (!g_gdiplusToken) {
        Gdiplus::GdiplusStartupInput in;
        Gdiplus::GdiplusStartup(&g_gdiplusToken, &in, nullptr);
    }
}

void GdiplusOff() {
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

// ---- the Game Mode window --------------------------------------------------
//
// One window carries the whole flow: it opens as a checklist, turns into a live
// progress view while apps are closed (or reopened), and ends as a summary. It is
// drawn as a single layered surface with per-pixel alpha, so its size, position
// and pixels change together and the animation never tears. Colours come from a
// theme, the font and the scale from the settings.

struct ChecklistItem {
    std::wstring text;
    std::wstring note;       // shown as a small tag after the name, e.g. "background"
    bool keepNote = false;   // a warning (ends process): shown in amber
    std::wstring path;       // the program, for its icon
    std::wstring aumid;      // the Store app id, when it has one: gives the real tile icon
};

struct ChecklistSpec {
    std::wstring title;              // "Turn on Game Mode?"
    std::wstring verb;               // "Closes" or "Reopens" (kept for the tests)
    std::wstring subtitle;           // "{n} app{s} will close. ..." ({n} = ticked apps)
    std::vector<std::wstring> info;  // muted facts, shown on one line (power plan, games)
    std::wstring hint;
    std::wstring okText;             // "Turn on"
    std::wstring runTitle;           // "Closing apps"
    std::wstring runHint;            // "Asking apps to close politely"
    std::wstring doneVerb;           // "closed" / "reopened"
    std::vector<ChecklistItem> items;
    bool startExpanded = false;      // unused: the list is always shown
};

struct ChecklistResult {
    bool ok = false;
    std::vector<bool> checked;
};

bool IsDarkTheme() {
    DWORD v = 1;
    DWORD size = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return v == 0;
}

namespace flow {

using namespace Gdiplus;

float Clamp01(float v) { return v < 0 ? 0 : v > 1 ? 1 : v; }
float Lerp(float a, float b, float t) { return a + (b - a) * t; }
float EaseOut(float t) { t = Clamp01(t); return 1 - std::pow(1 - t, 3.0f); }
float EaseInOut(float t) {
    t = Clamp01(t);
    return t < 0.5f ? 4 * t * t * t : 1 - std::pow(-2 * t + 2, 3.0f) / 2;
}
float Overshoot(float t) {
    t = Clamp01(t);
    const float c1 = 1.70158f, c3 = c1 + 1;
    return 1 + c3 * std::pow(t - 1, 3.0f) + c1 * std::pow(t - 1, 2.0f);
}
Color Alpha(Color c, float a) {
    return Color((BYTE)std::clamp((int)(a * c.GetA()), 0, 255), c.GetR(), c.GetG(), c.GetB());
}
Color Mix(Color a, Color b, float t) {
    return Color(255, (BYTE)Lerp(a.GetR(), b.GetR(), t), (BYTE)Lerp(a.GetG(), b.GetG(), t),
                 (BYTE)Lerp(a.GetB(), b.GetB(), t));
}

// ---- themes ---------------------------------------------------------------

struct Theme {
    bool light = false;
    Color bg, card, cardLine, cardHot, cardHotLine, text, muted, accent, accentHot, accentDeep, onAccent, amber,
        track, knobOff, glowA, glowB, edge;
    float glowLo = 0.2f, glowHi = 0.38f;
};

Theme MakeTheme(const std::wstring& id, bool systemDark) {
    std::wstring t = id;
    if (t == L"auto" || t.empty()) {
        t = systemDark ? L"oled-purple" : L"paper-light";
    }
    Color bg(255, 0, 0, 0), card(255, 12, 11, 16), line(255, 32, 27, 42), text(255, 245, 243, 255),
        muted(255, 142, 136, 160), accent(255, 167, 139, 250), on(255, 18, 7, 43), glow(255, 124, 58, 237);
    bool light = false;
    if (t == L"oled-white") {
        accent = Color(255, 245, 245, 247); on = Color(255, 0, 0, 0); glow = Color(255, 90, 90, 100);
        card = Color(255, 14, 14, 16); line = Color(255, 38, 38, 42); muted = Color(255, 150, 150, 158);
    } else if (t == L"oled-lime") {
        accent = Color(255, 163, 230, 53); on = Color(255, 8, 12, 0); glow = Color(255, 70, 110, 10);
        card = Color(255, 14, 14, 16); line = Color(255, 38, 38, 42); muted = Color(255, 150, 150, 158);
    } else if (t == L"oled-blue") {
        accent = Color(255, 76, 154, 255); on = Color(255, 4, 10, 24); glow = Color(255, 30, 80, 200);
        card = Color(255, 14, 14, 16); line = Color(255, 38, 38, 42); muted = Color(255, 150, 150, 158);
    } else if (t == L"midnight") {
        bg = Color(255, 30, 30, 34); card = Color(255, 42, 42, 48); line = Color(255, 60, 60, 68);
        text = Color(255, 244, 244, 245); muted = Color(255, 161, 161, 170); accent = Color(255, 76, 154, 255);
        on = Color(255, 8, 16, 32); glow = Color(255, 40, 90, 210);
    } else if (t == L"graphite-lime") {
        bg = Color(255, 18, 18, 18); card = Color(255, 28, 28, 30); line = Color(255, 44, 44, 46);
        text = Color(255, 245, 245, 245); muted = Color(255, 160, 160, 165); accent = Color(255, 163, 230, 53);
        on = Color(255, 8, 12, 0); glow = Color(255, 70, 110, 10);
    } else if (t == L"violet-night") {
        bg = Color(255, 23, 20, 31); card = Color(255, 33, 28, 46); line = Color(255, 52, 44, 74);
        text = Color(255, 243, 240, 255); muted = Color(255, 168, 160, 196); accent = Color(255, 167, 139, 250);
        on = Color(255, 20, 10, 50); glow = Color(255, 110, 60, 220);
    } else if (t == L"ember") {
        bg = Color(255, 26, 21, 18); card = Color(255, 37, 29, 24); line = Color(255, 58, 46, 38);
        text = Color(255, 253, 246, 239); muted = Color(255, 190, 170, 154); accent = Color(255, 251, 146, 60);
        on = Color(255, 40, 16, 0); glow = Color(255, 190, 80, 20);
    } else if (t == L"mint") {
        bg = Color(255, 15, 23, 21); card = Color(255, 21, 34, 30); line = Color(255, 31, 48, 41);
        text = Color(255, 236, 253, 245); muted = Color(255, 150, 182, 170); accent = Color(255, 52, 211, 153);
        on = Color(255, 0, 30, 18); glow = Color(255, 20, 140, 100);
    } else if (t == L"paper-light") {
        light = true;
        bg = Color(255, 250, 250, 249); card = Color(255, 255, 255, 255); line = Color(255, 228, 228, 231);
        text = Color(255, 24, 24, 27); muted = Color(255, 107, 114, 128); accent = Color(255, 37, 99, 235);
        on = Color(255, 255, 255, 255); glow = Color(255, 110, 150, 255);
    }
    Theme th;
    th.light = light;
    th.bg = bg; th.card = card; th.cardLine = line; th.text = text; th.muted = muted;
    th.accent = accent; th.onAccent = on;
    th.accentHot = Mix(accent, Color(255, 255, 255, 255), 0.25f);
    th.accentDeep = Mix(accent, Color(255, 0, 0, 0), 0.35f);
    th.cardHot = light ? Color(255, 244, 244, 245) : Mix(card, Color(255, 255, 255, 255), 0.05f);
    th.cardHotLine = Mix(line, accent, 0.35f);
    th.amber = light ? Color(255, 180, 110, 0) : Color(255, 251, 191, 36);
    th.track = light ? Color(255, 228, 228, 231) : Mix(card, Color(255, 255, 255, 255), 0.06f);
    th.knobOff = light ? Color(255, 160, 160, 170) : Color(255, 120, 114, 138);
    th.glowA = glow;
    th.glowB = accent;
    th.edge = Mix(line, accent, 0.15f);
    th.glowLo = light ? 0.16f : 0.20f;
    th.glowHi = light ? 0.30f : 0.38f;
    return th;
}

Theme g_th = MakeTheme(L"oled-purple", true);
std::wstring g_themeId;

// A near-white accent (the white OLED theme) needs dark marks on top of it.
bool BrightAccent(const Theme& T) {
    return 0.299f * T.accent.GetR() + 0.587f * T.accent.GetG() + 0.114f * T.accent.GetB() > 215.0f;
}


// ---- model ----------------------------------------------------------------

constexpr float W = 520;      // window width (logical px)
constexpr float M = 26;       // transparent margin around it, for the shadow
constexpr float TB = 44;      // the drawn title strip
constexpr float kP1 = 420, kP2 = 760;  // review -> running, running -> done (ms)
constexpr float rowH = 52, gap = 8;
constexpr int kIdCancel = 100000, kIdOk = 100001, kIdDone = 100002, kIdMin = 100003, kIdClose = 100004,
              kIdAll = 100005;

enum class Phase { Closed, Review, Running, Done };

struct Item {
    std::wstring name, note, path, aumid;
    bool keepNote = false;
    bool ticked = true;
    float knob = 1.0f;       // the switch's position, 0 = off, 1 = on; glides to the ticked state
    int state = 0;           // 0 waiting, 1 done, 2 left running
    float finishedAt = -1;
    Bitmap* icon = nullptr;
    Color tile;
};

struct Hit { RectF r; int id; };

HWND g_wnd = nullptr;
Phase g_phase = Phase::Closed;
// The window has its own thread (it only draws and handles input), so its animation never waits for the work
// the worker thread does: switching the power plan, closing apps, reopening apps. The two share the model
// below; the lock covers a frame being drawn and the few places where the worker changes the model.
CRITICAL_SECTION g_cs;
bool g_csInit = false;
HANDLE g_uiThread = nullptr, g_readyEvt = nullptr, g_answerEvt = nullptr;
volatile LONG g_quit = 0, g_finishPending = 0, g_frames = 0, g_renderUs = 0;   // drawn frames and time spent drawing them, for the tests
bool g_startRunning = false;
struct Guard {
    Guard() { EnterCriticalSection(&g_cs); }
    ~Guard() { LeaveCriticalSection(&g_cs); }
};
ChecklistSpec g_spec;
std::vector<Item> g_items;
std::vector<int> g_run;          // indices of the items that are being worked on
std::wstring g_infoLine;
float g_clickAt = -1, g_doneAt = -1;
std::wstring g_doneTitle, g_doneSummary;
float g_doneBody = 360;
bool g_answered = false, g_answerOk = false;
bool g_stopRequested = false, g_reloadRequested = false;
int g_hover = -1, g_pressed = -1, g_focus = kIdOk;
int g_titleHover = -1;
std::vector<Hit> g_hits;
float g_body = 480;
float g_listCap = 300;
float g_scroll = 0, g_scrollTarget = 0;
float g_scale = 1.0f;
LARGE_INTEGER g_freq, g_start;
HDC g_memDC = nullptr;
HBITMAP g_dib = nullptr;
Bitmap* g_surface = nullptr;
int g_maxW = 0, g_maxH = 0;
double g_cx = 0, g_cy = 0;
int g_lastL = -99999, g_lastT = -99999;
HICON g_iconS = nullptr, g_iconB = nullptr;
float g_lastDoneInteraction = 0;
bool g_userWentIdle = false;

float NowMs() {
    LARGE_INTEGER q;
    QueryPerformanceCounter(&q);
    return (float)((q.QuadPart - g_start.QuadPart) * 1000.0 / g_freq.QuadPart);
}

// ---- drawing helpers ------------------------------------------------------

void RR(GraphicsPath& p, float x, float y, float w, float h, float r) {
    r = std::min(r, std::min(w, h) / 2);
    float d = r * 2;
    if (d <= 0.1f) {
        p.AddRectangle(RectF(x, y, w, h));
        return;
    }
    p.AddArc(x, y, d, d, 180.0f, 90.0f);
    p.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
    p.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
    p.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
    p.CloseFigure();
}
void FillRR(Graphics& g, Color c, float x, float y, float w, float h, float r) {
    GraphicsPath p;
    RR(p, x, y, w, h, r);
    SolidBrush b(c);
    g.FillPath(&b, &p);
}
void StrokeRR(Graphics& g, Color c, float width, float x, float y, float w, float h, float r) {
    GraphicsPath p;
    RR(p, x, y, w, h, r);
    Pen pen(c, width);
    g.DrawPath(&pen, &p);
}

// ---- fonts ----------------------------------------------------------------

struct Fonts {
    FontFamily* ff;
    Font *title, *body, *row, *tag, *btn, *big, *small;
};
Fonts* g_fonts = nullptr;
std::wstring g_fontsFor;
std::map<std::pair<std::wstring, Font*>, float> g_textW;
std::map<Font*, float> g_lineH;   // line height per font

std::wstring WantedFont() {
    return g_settings.fontName.empty() ? L"Segoe UI Variable Text" : g_settings.fontName;
}

void FreeFonts() {
    if (g_fonts) {
        delete g_fonts->title; delete g_fonts->body; delete g_fonts->row; delete g_fonts->tag;
        delete g_fonts->btn; delete g_fonts->big; delete g_fonts->small; delete g_fonts->ff;
        delete g_fonts;
        g_fonts = nullptr;
    }
    g_textW.clear();
    g_lineH.clear();
    g_fontsFor.clear();
}

// The font from the settings; if it is not installed, a modern Windows font.
Fonts* GetFonts() {
    std::wstring want = WantedFont();
    if (g_fonts && g_fontsFor == want) {
        return g_fonts;
    }
    FreeFonts();
    FontFamily* ff = new FontFamily(want.c_str());
    if (!ff->IsAvailable()) {
        delete ff;
        ff = new FontFamily(L"Segoe UI Variable Text");
    }
    if (!ff->IsAvailable()) {
        delete ff;
        ff = new FontFamily(L"Segoe UI");
    }
    const int bold = ff->IsStyleAvailable(FontStyleBold) ? FontStyleBold : FontStyleRegular;
    g_fonts = new Fonts{ff,
                        new Font(ff, 23, bold, UnitPixel),
                        new Font(ff, 14, FontStyleRegular, UnitPixel),
                        new Font(ff, 15, FontStyleRegular, UnitPixel),
                        new Font(ff, 11, FontStyleRegular, UnitPixel),
                        new Font(ff, 14, bold, UnitPixel),
                        new Font(ff, 26, bold, UnitPixel),
                        new Font(ff, 13, FontStyleRegular, UnitPixel)};
    g_fontsFor = want;
    return g_fonts;
}

// GDI+ adds a hidden margin to ordinary strings, which makes text sit a few pixels away from where it is
// asked to go. The typographic format has none, so text lines up exactly with the cards and buttons.
// 0 near, 1 centre, 2 far (one line), 3 centred both ways, 4 centred and wrapping.
StringFormat* Fmt(int k) {
    static StringFormat* f[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    if (!f[k]) {
        f[k] = new StringFormat(StringFormat::GenericTypographic());
        f[k]->SetFormatFlags(f[k]->GetFormatFlags() | StringFormatFlagsMeasureTrailingSpaces | (k == 4 ? 0 : StringFormatFlagsNoWrap));
        f[k]->SetTrimming(StringTrimmingNone);
        f[k]->SetAlignment(k == 2 ? StringAlignmentFar : (k == 0 ? StringAlignmentNear : StringAlignmentCenter));
        f[k]->SetLineAlignment(k == 3 ? StringAlignmentCenter : StringAlignmentNear);
    }
    return f[k];
}

float TextW(Graphics& g, const std::wstring& s, Font& f) {
    auto key = std::make_pair(s, &f);
    auto it = g_textW.find(key);
    if (it != g_textW.end()) {
        return it->second;
    }
    RectF r;
    g.MeasureString(s.c_str(), -1, &f, PointF(0, 0), Fmt(0), &r);
    g_textW[key] = r.Width;
    return r.Width;
}

float LineH(Graphics& g, Font& f) {
    auto it = g_lineH.find(&f);
    if (it != g_lineH.end()) {
        return it->second;
    }
    float h = f.GetHeight(&g);
    g_lineH[&f] = h;
    return h;
}

std::wstring FitStr(Graphics& g, const std::wstring& s, Font& f, float maxW) {
    if (TextW(g, s, f) <= maxW) {
        return s;
    }
    std::wstring t = s;
    while (t.size() > 1 && TextW(g, t + L"…", f) > maxW) {
        t.pop_back();
    }
    return t + L"…";
}

void Str(Graphics& g, const std::wstring& s, Font& f, float x, float y, Color c, float alpha,
         StringAlignment al = StringAlignmentNear, float boxW = 0) {
    if (alpha <= 0.01f) {
        return;
    }
    SolidBrush b(Alpha(c, alpha));
    int k = al == StringAlignmentNear ? 0 : al == StringAlignmentCenter ? 1 : 2;
    if (al == StringAlignmentNear) {
        g.DrawString(s.c_str(), -1, &f, PointF(x, y), Fmt(k), &b);
    } else {
        g.DrawString(s.c_str(), -1, &f, RectF(x, y, boxW, 40), Fmt(k), &b);
    }
}

void CheckMark(Graphics& g, float cx, float cy, float s, float u, Color c, float width) {
    PointF p0(cx - 0.34f * s, cy + 0.02f * s), p1(cx - 0.09f * s, cy + 0.27f * s),
        p2(cx + 0.38f * s, cy - 0.26f * s);
    float l1 = std::hypot(p1.X - p0.X, p1.Y - p0.Y), l2 = std::hypot(p2.X - p1.X, p2.Y - p1.Y);
    float total = (l1 + l2) * Clamp01(u);
    Pen pen(c, width);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    pen.SetLineJoin(LineJoinRound);
    if (total <= l1) {
        float k = total / l1;
        g.DrawLine(&pen, p0, PointF(Lerp(p0.X, p1.X, k), Lerp(p0.Y, p1.Y, k)));
    } else {
        float k = (total - l1) / l2;
        g.DrawLine(&pen, p0, p1);
        g.DrawLine(&pen, p1, PointF(Lerp(p1.X, p2.X, k), Lerp(p1.Y, p2.Y, k)));
    }
}

// A colour matrix that only scales the opacity.
ColorMatrix AlphaMatrix(float a) {
    ColorMatrix cm = {};
    cm.m[0][0] = cm.m[1][1] = cm.m[2][2] = cm.m[4][4] = 1.0f;
    cm.m[3][3] = std::clamp(a, 0.0f, 1.0f);
    return cm;
}

// A glow is baked once into a bitmap and then drawn with a changing opacity.
Bitmap* g_glowBmp[3] = {nullptr, nullptr, nullptr};
void Glow(Graphics& g, int slot, float cx, float cy, float rx, float ry, Color c, float alpha) {
    if (alpha <= 0.01f) {
        return;
    }
    if (!g_glowBmp[slot]) {
        int w = (int)(rx * 2 * g_scale) + 2, h = (int)(ry * 2 * g_scale) + 2;
        g_glowBmp[slot] = new Bitmap(w, h, PixelFormat32bppPARGB);
        Graphics gg(g_glowBmp[slot]);
        gg.SetSmoothingMode(SmoothingModeAntiAlias);
        gg.Clear(Color(0, 0, 0, 0));
        GraphicsPath p;
        p.AddEllipse(0.0f, 0.0f, (float)w, (float)h);
        PathGradientBrush pg(&p);
        pg.SetCenterColor(Color(255, c.GetR(), c.GetG(), c.GetB()));
        Color edge(0, c.GetR(), c.GetG(), c.GetB());
        int n = 1;
        pg.SetSurroundColors(&edge, &n);
        gg.FillPath(&pg, &p);
    }
    ColorMatrix cm = AlphaMatrix(alpha);
    ImageAttributes ia;
    ia.SetColorMatrix(&cm);
    g.DrawImage(g_glowBmp[slot], RectF(cx - rx, cy - ry, rx * 2, ry * 2), 0.0f, 0.0f,
                (float)g_glowBmp[slot]->GetWidth(), (float)g_glowBmp[slot]->GetHeight(), UnitPixel, &ia);
}

// The window background (base colour plus glows) is baked into one bitmap and
// re-baked only when the glow opacity really changes.
constexpr float kBgH = 840;
Bitmap* g_bg = nullptr;
int g_bgKey = -1;
void DrawBackground(Graphics& g, float p2) {
    int key = (int)std::lround(p2 * 48);
    int wPx = (int)std::ceil(W * g_scale), hPx = (int)std::ceil((kBgH + TB) * g_scale);
    if (!g_bg || key != g_bgKey) {
        delete g_bg;
        g_bg = new Bitmap(wPx, hPx, PixelFormat32bppPARGB);
        Graphics gb(g_bg);
        gb.SetSmoothingMode(SmoothingModeAntiAlias);
        gb.ScaleTransform(g_scale, g_scale);
        SolidBrush bg(g_th.bg);
        gb.FillRectangle(&bg, 0.0f, 0.0f, W + 2, kBgH + TB + 2);
        float q = key / 48.0f;
        Glow(gb, 0, W * 0.3f, TB + 60, 300, 130, g_th.glowA, Lerp(g_th.glowLo, g_th.glowHi, q));
        Glow(gb, 1, W * 0.5f, TB + 130, 260, 150, g_th.glowB, 0.10f * q);
        g_bgKey = key;
    }
    g.DrawImage(g_bg, RectF(0, -TB, wPx / g_scale, hPx / g_scale), 0.0f, 0.0f, (float)wPx, (float)hPx, UnitPixel);
}

void FreeCaches() {
    for (auto& b : g_glowBmp) {
        delete b;
        b = nullptr;
    }
    delete g_bg;
    g_bg = nullptr;
    g_bgKey = -1;
}

// ---- layout ---------------------------------------------------------------

struct Lay {
    float p1, p2, listH, viewH, body, footerH, y0;
    std::vector<float> fac;
};

Lay Layout(float t) {
    Lay L;
    L.y0 = 104 + (g_infoLine.empty() ? 0 : 22);
    L.p1 = g_clickAt >= 0 ? EaseInOut((t - g_clickAt) / kP1) : 0;
    L.p2 = g_doneAt >= 0 ? EaseInOut((t - g_doneAt) / kP2) : 0;
    L.listH = 0;
    L.fac.resize(g_items.size());
    for (size_t i = 0; i < g_items.size(); i++) {
        float f = 1;
        if (!g_items[i].ticked) {
            f *= 1 - L.p1;
        }
        if (g_doneAt >= 0) {
            f *= 1 - EaseInOut((t - g_doneAt - 24.0f * std::min<size_t>(i, 12)) / 340);
        }
        L.fac[i] = f;
        L.listH += (rowH + gap) * f;
    }
    L.viewH = std::min(L.listH, g_listCap);
    L.footerH = Lerp(86, 70, L.p1);
    float bodyRun = L.y0 + L.viewH + L.footerH;
    L.body = Lerp(bodyRun, g_doneBody, L.p2);
    return L;
}

void AddHit(float x, float y, float w, float h, int id) { g_hits.push_back({RectF(x, y, w, h), id}); }

int TitleHit(float x, float y) {
    if (y < 0 || y >= TB) {
        return -1;
    }
    if (x >= W - 46) {
        return kIdClose;   // the hit areas meet in the middle of the 4 px gap between the two buttons
    }
    if (x >= W - 90) {
        return kIdMin;
    }
    return -1;
}

std::wstring Fill(std::wstring s, int n) {
    auto rep = [&](const wchar_t* key, const std::wstring& val) {
        size_t p;
        while ((p = s.find(key)) != std::wstring::npos) {
            s.replace(p, wcslen(key), val);
        }
    };
    rep(L"{n}", std::to_wstring(n));
    rep(L"{s}", n == 1 ? L"" : L"s");
    return s;
}

// ---- one frame (content area, origin below the title strip) ---------------

void Frame(Graphics& g, float t) {
    Lay L = Layout(t);
    float p1 = L.p1, p2 = L.p2;
    g_body = L.body;
    g_hits.clear();
    Fonts* F = GetFonts();
    Font& fTitle = *F->title; Font& fBody = *F->body; Font& fRow = *F->row; Font& fTag = *F->tag;
    Font& fBtn = *F->btn; Font& fBig = *F->big; Font& fSmall = *F->small;
    const Theme& T = g_th;

    int nTicked = 0;
    for (auto& a : g_items) {
        nTicked += a.ticked;
    }
    int nDone = 0;
    for (auto& a : g_items) {
        nDone += a.ticked && a.state == 1;
    }
    float prog = 0;
    if (g_clickAt >= 0 && nTicked > 0) {
        float c = 0;
        for (auto& a : g_items) {
            if (a.ticked && a.state != 0) {
                c += EaseOut((t - a.finishedAt) / 220);
            }
        }
        prog = c / nTicked;
        if (g_doneAt >= 0) {
            prog = Lerp(prog, 1.0f, EaseOut((t - g_doneAt) / 260));
        }
    }

    DrawBackground(g, p2);

    float cx0 = 28, cw = W - 56, top = 0;
    wchar_t buf[160];

    // ---- header
    float tA = 1 - EaseOut(p1 * 2.2f);
    float tB = EaseOut((p1 - 0.35f) / 0.65f) * (1 - Clamp01(p2 * 2.2f));
    float enterHdr = EaseOut(t / 280);
    bool allOn = nTicked == (int)g_items.size();
    bool canAll = g_items.size() > 1 && g_clickAt < 0;
    Str(g, FitStr(g, g_spec.title, fTitle, canAll ? W - 56 - 108 : W - 56), fTitle, cx0, top + 26 + (1 - enterHdr) * 8 + p1 * -6, T.text,
        tA * enterHdr);
    std::wstring sub = g_items.empty() ? L"There are no apps to deal with right now."
                       : nTicked == 0   ? L"Nothing is ticked, so no app will change."
                                        : Fill(g_spec.subtitle, nTicked);
    Str(g, FitStr(g, sub, fBody, W - 56), fBody, cx0, top + 62 + (1 - enterHdr) * 8, T.muted,
        tA * enterHdr);
    if (!g_infoLine.empty()) {
        Str(g, FitStr(g, g_infoLine, fSmall, W - 56), fSmall, cx0, top + 86 + (1 - enterHdr) * 8, T.muted,
            tA * enterHdr * 0.8f);
    }
    Str(g, FitStr(g, g_spec.runTitle, fTitle, W - 56), fTitle, cx0, top + 26 + (1 - tB) * 8, T.text, tB);
    swprintf(buf, 160, L"%d of %d %ls", nDone, nTicked, g_spec.doneVerb.c_str());
    Str(g, FitStr(g, buf, fBody, W - 56), fBody, cx0, top + 62 + (1 - tB) * 8, T.muted, tB);

    // select all / none
    if (canAll && tA * enterHdr > 0.05f) {
        float bw = 96, bh = 28, bx = cx0 + cw - bw, by = top + 26 + LineH(g, fTitle) / 2 - bh / 2 - p1 * 6;
        bool hot = g_hover == kIdAll;
        FillRR(g, Alpha(hot ? T.cardHot : T.card, tA * enterHdr), bx, by, bw, bh, 14);
        StrokeRR(g, Alpha(hot ? T.cardHotLine : T.cardLine, tA * enterHdr), 1, bx + 0.5f, by + 0.5f, bw - 1, bh - 1, 14);
        StringFormat& sf = *Fmt(3);
        SolidBrush tb(Alpha(T.text, tA * enterHdr));
        g.DrawString(allOn ? L"Select none" : L"Select all", -1, &fTag, RectF(bx, by, bw, bh), &sf, &tb);
        if (tA * enterHdr > 0.6f) {
            AddHit(bx, by, bw, bh, kIdAll);
        }
    }

    // ---- scrolling: smooth, clamped; follows the focus in review and the first unfinished app while working
    static float prevT = 0;
    float dt = t - prevT;
    if (dt < 0 || dt > 100) {
        dt = 16;
    }
    prevT = t;
    float maxScroll = std::max(0.0f, L.listH - L.viewH);
    {
        static int lastFocus = -2;
        if (g_focus != lastFocus) {
            lastFocus = g_focus;
            if (g_clickAt < 0 && g_focus >= 0 && g_focus < (int)g_items.size()) {
                float yRow = g_focus * (rowH + gap);
                if (yRow < g_scrollTarget) {
                    g_scrollTarget = yRow;
                } else if (yRow + rowH > g_scrollTarget + L.viewH) {
                    g_scrollTarget = yRow + rowH - L.viewH;
                }
            }
        }
        if (g_clickAt >= 0 && g_doneAt < 0 && maxScroll > 0) {
            float pos = 0;
            for (size_t i = 0; i < g_items.size(); i++) {
                if (g_items[i].ticked && g_items[i].state == 0) {
                    g_scrollTarget = pos - L.viewH * 0.35f;
                    break;
                }
                pos += (rowH + gap) * L.fac[i];
            }
        }
        g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, maxScroll);
        g_scroll += (g_scrollTarget - g_scroll) * (1 - std::exp(-dt / 45.0f));
        g_scroll = std::clamp(g_scroll, 0.0f, maxScroll);
    }

    // the switches glide to their state: quick and slightly overshooting nothing, just a short slide
    for (auto& a : g_items) {
        float target = a.ticked ? 1.0f : 0.0f;
        a.knob += (target - a.knob) * (1 - std::exp(-dt / 30.0f));
        if (std::fabs(target - a.knob) < 0.012f) {
            a.knob = target;
        }
    }

    // ---- rows
    GraphicsState listState = g.Save();
    float vpTop = top + L.y0, vpH = L.viewH;
    g.SetClip(RectF(cx0 - 4, vpTop - 4, cw + 8, vpH + 8), CombineModeIntersect);
    float ry = vpTop - g_scroll;
    for (size_t i = 0; i < g_items.size(); i++) {
        Item& a = g_items[i];
        float f = L.fac[i];
        if (f <= 0.002f) {
            continue;
        }
        float rowTop = ry;
        ry += (rowH + gap) * f;
        if (rowTop > vpTop + vpH + 4 || rowTop + rowH < vpTop - 4) {
            continue;   // scrolled out of view: do not draw
        }
        float enter = EaseOut((t - 40 - 32.0f * std::min<size_t>(i, 12)) / 280);
        float alpha = enter * std::min(1.0f, f * 1.6f);
        float slide = (1 - enter) * 14;
        float yy = rowTop + slide;
        float hh = rowH * (f < 1 ? std::max(0.35f, f) : 1.0f);
        bool interactive = g_clickAt < 0 && enter > 0.9f;
        bool hot = interactive && g_hover == (int)i;
        FillRR(g, Alpha(hot ? T.cardHot : T.card, alpha), cx0, yy, cw, hh, 14);
        StrokeRR(g, Alpha(hot ? T.cardHotLine : T.cardLine, alpha), 1, cx0 + 0.5f, yy + 0.5f, cw - 1, hh - 1, 14);
        if (g_focus == (int)i && g_clickAt < 0) {
            StrokeRR(g, Alpha(T.accent, alpha * 0.9f), 1.6f, cx0 - 1.5f, yy - 1.5f, cw + 3, hh + 3, 16);
        }
        if (interactive) {
            float hy0 = std::max(yy, vpTop), hy1 = std::min(yy + rowH, vpTop + vpH);
            if (hy1 > hy0) {
                AddHit(cx0, hy0, cw, hy1 - hy0, (int)i);
            }
        }
        GraphicsState rs = g.Save();
        GraphicsPath clipP;
        RR(clipP, cx0, yy, cw, hh, 14);
        g.SetClip(&clipP, CombineModeIntersect);
        float content = alpha * (f < 0.55f ? f / 0.55f : 1.0f);
        float dim = g_clickAt >= 0 ? 1.0f : Lerp(0.55f, 1.0f, a.knob);
        if (a.icon) {
            float ia = std::min(1.0f, content * dim);
            if (ia > 0.995f) {
                g.DrawImage(a.icon, RectF(cx0 + 13, yy + 12, 28, 28), 0.0f, 0.0f, (float)a.icon->GetWidth(),
                            (float)a.icon->GetHeight(), UnitPixel);
            } else {
                ColorMatrix cm = AlphaMatrix(ia);
                ImageAttributes attr;
                attr.SetColorMatrix(&cm);
                g.DrawImage(a.icon, RectF(cx0 + 13, yy + 12, 28, 28), 0.0f, 0.0f, (float)a.icon->GetWidth(),
                            (float)a.icon->GetHeight(), UnitPixel, &attr);
            }
        } else {
            GraphicsPath tp;
            RR(tp, cx0 + 13, yy + 12, 28, 28, 8);
            LinearGradientBrush tg(PointF(0, yy + 12), PointF(0, yy + 40), Alpha(Mix(a.tile, Color(255, 255, 255, 255), 0.25f), content * dim),
                                   Alpha(Mix(a.tile, Color(255, 0, 0, 0), 0.25f), content * dim));
            g.FillPath(&tg, &tp);
            wchar_t letter[2] = {a.name.empty() ? L'?' : (wchar_t)towupper(a.name[0]), 0};
            StringFormat& sf = *Fmt(3);
            SolidBrush lb(Alpha(Color(255, 255, 255, 255), content * dim));
            g.DrawString(letter, -1, &fBtn, RectF(cx0 + 13, yy + 12, 28, 28), &sf, &lb);
        }
        float tagW = a.note.empty() ? 0 : TextW(g, a.note, fTag) + 14 + 10;
        std::wstring nm = FitStr(g, a.name, fRow, cw - 52 - 70 - tagW);
        Str(g, nm, fRow, cx0 + 52, yy + (rowH - LineH(g, fRow)) / 2, T.text, content * (g_clickAt >= 0 ? 1.0f : Lerp(0.6f, 1.0f, a.knob)));
        if (!a.note.empty()) {
            float nw = TextW(g, nm, fRow);
            float tx = cx0 + 52 + nw + 10, tw = TextW(g, a.note, fTag) + 14;
            StrokeRR(g, Alpha(a.keepNote ? T.amber : T.cardLine, content * (a.keepNote ? 0.7f : 1.0f)), 1, tx, yy + 16, tw, 20, 10);
            Str(g, a.note, fTag, tx + 7, yy + 26 - LineH(g, fTag) / 2, a.keepNote ? T.amber : T.muted, content);
        }
        float cxr = cx0 + cw - 14 - 21, cyr = yy + rowH / 2;
        float toggleA = (1 - EaseOut(p1 * 2.5f)) * content;
        if (toggleA > 0.01f) {
            FillRR(g, Alpha(Mix(T.track, T.accent, a.knob), toggleA), cxr - 21, cyr - 12, 42, 24, 12);
            float kx = Lerp(cxr - 9, cxr + 9, a.knob);
            float stretch = 4.0f * std::sin(3.14159265f * a.knob);   // the knob stretches a little while it moves
            SolidBrush kb(Alpha(Mix(T.knobOff, T.onAccent, a.knob), toggleA));
            g.FillEllipse(&kb, kx - 8.5f - stretch / 2, cyr - 8.5f, 17.0f + stretch, 17.0f);
        }
        if (a.ticked && g_clickAt >= 0) {
            float runA = EaseOut((p1 - 0.3f) / 0.7f) * content;
            float doneU = a.state == 1 ? EaseOut((t - a.finishedAt) / 240) : 0;
            float gaveU = a.state == 2 ? EaseOut((t - a.finishedAt) / 200) : 0;
            float spinA = runA * (1 - std::max(doneU, gaveU));
            if (spinA > 0.01f) {
                Pen track(Alpha(T.track, spinA), 2.5f);
                g.DrawEllipse(&track, cxr - 9, cyr - 9, 18.0f, 18.0f);
                Pen sp(Alpha(T.accent, spinA), 2.5f);
                sp.SetStartCap(LineCapRound);
                sp.SetEndCap(LineCapRound);
                float ang = std::fmod(t * 0.42f + i * 70, 360.0f);
                g.DrawArc(&sp, cxr - 9, cyr - 9, 18.0f, 18.0f, ang, 250.0f);
            }
            if (a.state == 1) {
                float s = Overshoot(doneU);
                SolidBrush cb(Alpha(T.accent, runA * doneU));
                float rad = 11 * s;
                g.FillEllipse(&cb, cxr - rad, cyr - rad, rad * 2, rad * 2);
                CheckMark(g, cxr, cyr, 16, EaseOut((t - a.finishedAt - 30) / 210), Alpha(T.onAccent, runA), 2.2f);
            }
            if (a.state == 2) {
                SolidBrush cb(Alpha(Mix(T.bg, T.amber, 0.18f), runA * gaveU));
                g.FillEllipse(&cb, cxr - 11, cyr - 11, 22.0f, 22.0f);
                Pen ap(Alpha(T.amber, runA * gaveU), 2.0f);
                ap.SetStartCap(LineCapRound);
                ap.SetEndCap(LineCapRound);
                g.DrawEllipse(&ap, cxr - 10.5f, cyr - 10.5f, 21.0f, 21.0f);
                g.DrawLine(&ap, cxr, cyr - 5, cxr, cyr + 1);
                SolidBrush ad(Alpha(T.amber, runA * gaveU));
                g.FillEllipse(&ad, cxr - 1.3f, cyr + 4.2f, 2.6f, 2.6f);
            }
        }
        g.Restore(rs);
    }
    g.Restore(listState);
    if (maxScroll > 1) {   // soft fade at the cut edges and a thin scroll indicator
        float fa = 22;
        Color b0 = Color(255, T.bg.GetR(), T.bg.GetG(), T.bg.GetB()), b1 = Color(0, T.bg.GetR(), T.bg.GetG(), T.bg.GetB());
        if (g_scroll > 1) {
            LinearGradientBrush lgT(PointF(0, vpTop), PointF(0, vpTop + fa), b0, b1);
            g.FillRectangle(&lgT, cx0 - 4, vpTop, cw + 8, fa);
        }
        if (g_scroll < maxScroll - 1) {
            LinearGradientBrush lgB(PointF(0, vpTop + vpH - fa), PointF(0, vpTop + vpH), b1, b0);
            g.FillRectangle(&lgB, cx0 - 4, vpTop + vpH - fa, cw + 8, fa);
        }
        float thumbH = std::max(28.0f, vpH * vpH / L.listH);
        float thumbY = vpTop + (vpH - thumbH) * (g_scroll / maxScroll);
        FillRR(g, Alpha(T.muted, 0.45f), W - 12, thumbY, 3, thumbH, 1.5f);
    }

    // ---- footer: review buttons
    float footTop = top + L.y0 + L.viewH;
    float btnA = (1 - EaseOut(p1 * 2.4f)) * EaseOut((t - 200) / 260);
    float btnY = footTop + 22;
    if (btnA > 0.01f) {
        bool onHot = g_hover == kIdOk, onDown = g_pressed == kIdOk && onHot, cancelHot = g_hover == kIdCancel,
             cancelDown = g_pressed == kIdCancel && cancelHot;
        bool disabled = false;   // confirming with nothing ticked is fine: the power plan and priorities still apply
        Color fill = disabled ? T.track : onDown ? T.accentDeep : onHot ? T.accentHot : T.accent;
        float okW = std::max(132.0f, TextW(g, g_spec.okText, fBtn) + 44);
        FillRR(g, Alpha(fill, btnA), cx0 + cw - okW, btnY, okW, 42, 12);
        StringFormat& sf = *Fmt(3);
        SolidBrush ob(Alpha(disabled ? T.muted : T.onAccent, btnA));
        g.DrawString(g_spec.okText.c_str(), -1, &fBtn, RectF(cx0 + cw - okW, btnY, okW, 42), &sf, &ob);
        float cxB = cx0 + cw - okW - 12 - 108;
        FillRR(g, Alpha(cancelDown ? T.track : cancelHot ? T.cardHot : T.card, btnA), cxB, btnY, 108, 42, 12);
        StrokeRR(g, Alpha(cancelHot ? T.cardHotLine : T.cardLine, btnA), 1, cxB + 0.5f, btnY + 0.5f, 107, 41, 12);
        SolidBrush tb2(Alpha(T.text, btnA));
        g.DrawString(L"Cancel", -1, &fBtn, RectF(cxB, btnY, 108, 42), &sf, &tb2);
        if (g_focus == kIdOk) {
            StrokeRR(g, Alpha(T.accentHot, btnA), 1.6f, cx0 + cw - okW - 2.5f, btnY - 2.5f, okW + 5, 47, 14);
        }
        if (g_focus == kIdCancel) {
            StrokeRR(g, Alpha(T.accent, btnA), 1.6f, cxB - 2.5f, btnY - 2.5f, 113, 47, 14);
        }
        float hintMax = cxB - cx0 - 14;
        if (hintMax > 60) {
            std::wstring hintText = L"Enter to confirm   Esc to cancel";
            if (TextW(g, hintText, fTag) > hintMax) {
                hintText = L"Enter   Esc";   // a wide font leaves less room
            }
            Str(g, FitStr(g, hintText, fTag, hintMax), fTag, cx0, btnY + 21 - LineH(g, fTag) / 2, T.muted,
                btnA * 0.8f);
        }
        if (btnA > 0.6f && g_clickAt < 0) {
            AddHit(cx0 + cw - okW, btnY, okW, 42, kIdOk);
            AddHit(cxB, btnY, 108, 42, kIdCancel);
        }
    }

    // ---- progress bar -> Done button
    float barA = EaseOut((p1 - 0.35f) / 0.65f);
    float barY = footTop + 38, barX = cx0, barW = cw, barH = 8;
    float btnW = 150, btnH = 44;
    float doneBtnX = (W - btnW) / 2, doneBtnY = top + g_doneBody - btnH - 40;
    float m = EaseInOut((p2 - 0.45f) / 0.55f);
    float bx = Lerp(barX, doneBtnX, m), bw = Lerp(barW, btnW, m), by = Lerp(barY, doneBtnY, m),
          bh = Lerp(barH, btnH, m), br = Lerp(4, 13, m);
    if (barA > 0.01f) {
        float cA = barA * (1 - Clamp01(p2 * 3));
        swprintf(buf, 160, L"%d%%", (int)(prog * 100));
        Str(g, FitStr(g, g_spec.runHint, fSmall, cw - 60), fSmall, cx0, barY - 10 - LineH(g, fSmall), T.muted, cA);
        Str(g, buf, fSmall, cx0, barY - 10 - LineH(g, fSmall), T.text, cA, StringAlignmentFar, cw);
        FillRR(g, Alpha(T.track, barA * (1 - m)), barX, barY, barW, barH, 4);
        float fillW = std::max(barH, barW * prog);
        float glowA = 0.35f * (1 - m);
        if (glowA > 0.01f) {
            FillRR(g, Alpha(T.accent, barA * glowA * 0.5f), barX - 2, barY - 3, fillW + 4, barH + 6, 7);
        }
        bool dHot = m > 0.99f && g_hover == kIdDone, dDown = dHot && g_pressed == kIdDone;
        Color fc = m > 0.5f ? (dDown ? T.accentDeep : dHot ? T.accentHot : T.accent) : T.accent;
        if (m < 0.001f) {
            FillRR(g, Alpha(T.accent, barA), barX, barY, fillW, barH, 4);
        } else {
            FillRR(g, Alpha(fc, barA), bx, by, bw, bh, br);
        }
        if (m > 0.55f) {
            float ta = Clamp01((m - 0.55f) / 0.45f);
            StringFormat& sf = *Fmt(3);
            SolidBrush ob(Alpha(T.onAccent, ta));
            g.DrawString(L"Done", -1, &fBtn, RectF(bx, by, bw, bh), &sf, &ob);
        }
        if (m > 0.99f) {
            AddHit(bx, by, bw, bh, kIdDone);
            if (g_focus == kIdDone) {
                StrokeRR(g, Alpha(T.accentHot, 1.0f), 1.6f, bx - 2.5f, by - 2.5f, bw + 5, bh + 5, 15);
            }
        }
    }

    // ---- done: badge, rings, headline, summary
    if (p2 > 0.2f) {
        float u = (p2 - 0.2f) / 0.8f;
        float bcx = W / 2, bcy = top + 106;
        for (int k = 0; k < 2; k++) {
            float ru = Clamp01((u - 0.12f * k - 0.04f) / 0.7f);
            if (ru > 0 && ru < 1) {
                Pen rp(Alpha(T.accent, (1 - ru) * 0.45f), 2.0f);
                float rr = 38 + ru * 30;   // the rings stay inside the badge's own area
                g.DrawEllipse(&rp, bcx - rr, bcy - rr, rr * 2, rr * 2);
            }
        }
        float s = Overshoot((u - 0.0f) / 0.46f);
        Glow(g, 2, bcx, bcy, 70, 70, T.glowB, 0.55f * EaseOut(u) * std::min(1.0f, s));
        GraphicsPath bp;
        float rad = 32 * s;
        bp.AddEllipse(bcx - rad, bcy - rad, rad * 2, rad * 2);
        LinearGradientBrush lg(PointF(bcx - rad, bcy - rad), PointF(bcx + rad, bcy + rad), Mix(T.accent, Color(255, 255, 255, 255), 0.35f), T.accentDeep);
        g.FillPath(&lg, &bp);
        CheckMark(g, bcx, bcy, 44, EaseOut((u - 0.22f) / 0.34f), (T.light || BrightAccent(T)) ? T.onAccent : Color(255, 255, 255, 255), 4.2f);
        float ha = EaseOut((u - 0.30f) / 0.40f);
        Str(g, FitStr(g, g_doneTitle, fBig, W - 56), fBig, 0, top + 180 + (1 - ha) * 10, T.text, ha, StringAlignmentCenter, W);
        if (ha > 0.01f) {
            SolidBrush sb(Alpha(T.muted, ha));
            g.DrawString(g_doneSummary.c_str(), -1, &fBody, RectF(28, top + 228 + (1 - ha) * 10, W - 56, 80), Fmt(4), &sb);
        }
    }
}

// ---- title strip ------------------------------------------------------------

// The app icon: a game controller on a rounded tile in the theme's accent colour. Drawn in a unit square
// at (x, y) with side s, so the same drawing serves the title strip and the window/taskbar icons.
void DrawGlyph(Graphics& g, float x, float y, float s, const Theme& T) {
    GraphicsPath tile;
    RR(tile, x + 0.5f, y + 0.5f, s - 1, s - 1, s * 0.26f);
    LinearGradientBrush grad(PointF(x, y), PointF(x + s, y + s), Mix(T.accent, Color(255, 255, 255, 255), 0.35f), T.accentDeep);
    g.FillPath(&grad, &tile);
    const bool dark = BrightAccent(T);
    const Color ink = dark ? T.onAccent : Color(255, 255, 255, 255);
    SolidBrush white(ink);
    // body: a wide rounded bar with a grip hanging from each end
    FillRR(g, ink, x + 0.17f * s, y + 0.31f * s, 0.66f * s, 0.30f * s, 0.15f * s);
    g.FillEllipse(&white, x + 0.17f * s, y + 0.44f * s, 0.22f * s, 0.30f * s);
    g.FillEllipse(&white, x + 0.61f * s, y + 0.44f * s, 0.22f * s, 0.30f * s);
    // d-pad and two buttons, cut out in the tile's colour
    SolidBrush cut(dark ? Mix(T.accent, T.accentDeep, 0.35f) : Mix(T.accent, T.accentDeep, 0.55f));
    g.FillRectangle(&cut, x + 0.27f * s, y + 0.425f * s, 0.15f * s, 0.045f * s);
    g.FillRectangle(&cut, x + 0.325f * s, y + 0.37f * s, 0.045f * s, 0.155f * s);
    g.FillEllipse(&cut, x + 0.575f * s, y + 0.38f * s, 0.075f * s, 0.075f * s);
    g.FillEllipse(&cut, x + 0.665f * s, y + 0.45f * s, 0.075f * s, 0.075f * s);
}

HICON MakeIcon(int size) {
    HICON icon = nullptr;
    Bitmap bmp(size, size, PixelFormat32bppARGB);
    Graphics g(&bmp);
    g.SetSmoothingMode(SmoothingModeHighQuality);
    g.Clear(Color(0, 0, 0, 0));
    DrawGlyph(g, 0, 0, (float)size, g_th);
    bmp.GetHICON(&icon);
    return icon;
}

void DrawTitle(Graphics& g, bool active) {
    Font& f = *GetFonts()->small;
    const Theme& T = g_th;
    const float cy = TB / 2;                       // the strip's centre line: icon, title and buttons share it
    DrawGlyph(g, 18, cy - 8.5f, 17, T);
    Str(g, L"Game Mode", f, 46, cy - LineH(g, f) / 2, T.muted, active ? 1.0f : 0.6f);

    // two buttons, 36 x 28, 4 px apart, 8 px from the top and right edge; the glyphs sit in their centres
    const float bh = 28, by = cy - bh / 2;
    const float closeX = W - 8 - 36, minX = closeX - 4 - 36;
    bool canClose = g_phase != Phase::Running;
    if (g_titleHover == kIdClose && canClose) {
        FillRR(g, Color(255, 229, 72, 77), closeX, by, 36, bh, 8);
    } else if (g_titleHover == kIdMin) {
        FillRR(g, T.cardHot, minX, by, 36, bh, 8);
    }
    Pen x(g_titleHover == kIdClose && canClose ? Color(255, 255, 255, 255) : Alpha(T.muted, canClose ? 1.0f : 0.35f), 1.4f);
    x.SetStartCap(LineCapRound);
    x.SetEndCap(LineCapRound);
    const float cxc = closeX + 18;
    g.DrawLine(&x, cxc - 5, cy - 5, cxc + 5, cy + 5);
    g.DrawLine(&x, cxc + 5, cy - 5, cxc - 5, cy + 5);
    Pen mn(Alpha(T.muted, 1), 1.4f);
    mn.SetStartCap(LineCapRound);
    mn.SetEndCap(LineCapRound);
    const float cxm = minX + 18;
    g.DrawLine(&mn, cxm - 5, cy, cxm + 5, cy);
}

// ---- window and surface -----------------------------------------------------

float ScaleFor(HWND h) {
    if (g_settings.uiScale >= 1.0f) {
        return std::clamp(g_settings.uiScale, 1.0f, 4.0f);
    }
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static auto fn = (GetDpiForWindow_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
    if (fn) {
        UINT d = fn(h);
        if (d) {
            return d / 96.0f;
        }
    }
    HDC dc = GetDC(nullptr);
    int d = GetDeviceCaps(dc, LOGPIXELSX);
    ReleaseDC(nullptr, dc);
    return d > 0 ? d / 96.0f : 1.0f;
}

// How many rows fit: the window may use at most ~92% of the monitor's work area; longer lists scroll.
void ComputeListCap(HWND h) {
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST), &mi);
    float availLogical = (mi.rcWork.bottom - mi.rcWork.top) / g_scale;
    float cap = availLogical * 0.92f - 2 * M - TB - 126 - 92;
    g_listCap = std::clamp(cap, 3 * (rowH + gap), 9 * (rowH + gap));
}

void AllocSurface() {
    delete g_surface;
    g_surface = nullptr;
    if (g_dib) {
        DeleteObject(g_dib);
        g_dib = nullptr;
    }
    if (g_memDC) {
        DeleteDC(g_memDC);
        g_memDC = nullptr;
    }
    g_maxW = (int)std::ceil((W + 2 * M) * g_scale) + 2;
    g_maxH = (int)std::ceil((TB + 126 + std::max(g_listCap, 300.0f) + 100 + 2 * M) * g_scale) + 2;
    HDC screen = GetDC(nullptr);
    g_memDC = CreateCompatibleDC(screen);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = g_maxW;
    bi.bmiHeader.biHeight = -g_maxH;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    void* bits = nullptr;
    g_dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    SelectObject(g_memDC, g_dib);
    ReleaseDC(nullptr, screen);
    g_surface = new Bitmap(g_maxW, g_maxH, g_maxW * 4, PixelFormat32bppPARGB, (BYTE*)bits);
}

void FreeIcons() {
    if (g_iconS) {
        DestroyIcon(g_iconS);
        g_iconS = nullptr;
    }
    if (g_iconB) {
        DestroyIcon(g_iconB);
        g_iconB = nullptr;
    }
}

// Everything that depends on the scale: baked glows, icons, the drawing surface.
void ApplyScale(HWND h, float scale) {
    g_scale = scale;
    FreeCaches();
    ComputeListCap(h);
    AllocSurface();
    FreeIcons();
    g_iconS = MakeIcon(std::max(16, (int)(16 * g_scale)));
    g_iconB = MakeIcon(std::max(32, (int)(32 * g_scale)));
    SendMessageW(h, WM_SETICON, ICON_SMALL, (LPARAM)g_iconS);
    SendMessageW(h, WM_SETICON, ICON_BIG, (LPARAM)g_iconB);
}

void Render() {
    if (!g_wnd || IsIconic(g_wnd) || !g_surface) {
        return;
    }
    float now = NowMs();
    Lay L = Layout(now);
    g_body = L.body;
    float totW = W + 2 * M, totH = TB + g_body + 2 * M;
    int cw = (int)std::lround(totW * g_scale), ch = (int)std::lround(totH * g_scale);
    cw = std::min(cw, g_maxW);
    ch = std::min(ch, g_maxH);
    {
        Graphics g(g_surface);
        g.SetCompositingMode(CompositingModeSourceCopy);
        SolidBrush clear(Color(0, 0, 0, 0));
        g.FillRectangle(&clear, 0, 0, cw, ch);
        g.SetCompositingMode(CompositingModeSourceOver);
        // Everything that is copied (the baked background, the icons, the glows) is already at its final size, so
        // copying needs no filtering; gamma-correct blending would cost a lot per frame for no visible gain.
        g.SetCompositingQuality(CompositingQualityHighSpeed);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(PixelOffsetModeHalf);
        g.SetInterpolationMode(InterpolationModeNearestNeighbor);
        // Whole-number scales (100%, 200%) get hinted text, which is crisper; in-between scales get smooth,
        // unhinted text, which keeps the letter spacing even.
        bool wholeScale = std::fabs(g_scale - std::round(g_scale)) < 0.01f;
        g.SetTextRenderingHint(wholeScale ? TextRenderingHintAntiAliasGridFit : TextRenderingHintAntiAlias);
        g.ScaleTransform(g_scale, g_scale);
        float H = TB + g_body;
        GraphicsPath win;
        RR(win, M, M, W, H, 14);
        {   // soft shadow, only in the ring outside the window
            Region ring(RectF(0, 0, totW, totH));
            ring.Exclude(&win);
            g.SetClip(&ring);
            for (int i = 0; i < 7; i++) {
                float e = (float)i * 3.2f;
                FillRR(g, Color((BYTE)((g_th.light ? 6 : 10) + (g_th.light ? 1.0f : 1.6f) * (7 - i)), 0, 0, 0), M - e, M + 5 - e,
                       W + 2 * e, H + 2 * e, 14 + e);
            }
            g.ResetClip();
        }
        g.SetClip(&win);
        g.TranslateTransform(M, M + TB);
        Frame(g, now);
        g.ResetTransform();
        g.ScaleTransform(g_scale, g_scale);
        g.TranslateTransform(M, M);
        DrawTitle(g, GetForegroundWindow() == g_wnd);
        g.ResetClip();
        g.ResetTransform();
        g.ScaleTransform(g_scale, g_scale);
        StrokeRR(g, g_th.edge, 1.0f, M + 0.5f, M + 0.5f, W - 1, H - 1, 14);
    }
    GdiFlush();

    // keep the window centred where it is, even while its height changes; follow it if the user drags it
    RECT cur;
    GetWindowRect(g_wnd, &cur);
    if (g_lastL != -99999 && (cur.left != g_lastL || cur.top != g_lastT)) {
        g_cx = cur.left + (cur.right - cur.left) / 2.0;
        g_cy = cur.top + (cur.bottom - cur.top) / 2.0;
    }
    int left = (int)std::lround(g_cx - cw / 2.0), topPx = (int)std::lround(g_cy - ch / 2.0);
    g_lastL = left;
    g_lastT = topPx;
    POINT dst = {left, topPx}, src = {0, 0};
    SIZE sz = {cw, ch};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_wnd, nullptr, &dst, &sz, g_memDC, &src, 0, &bf, ULW_ALPHA);
}

// ---- input --------------------------------------------------------------------

void ToLogical(LPARAM l, float* x, float* y) {
    *x = (short)LOWORD(l) / g_scale - M;
    *y = (short)HIWORD(l) / g_scale - M;
}

int HitAt(float x, float y) {
    for (int i = (int)g_hits.size() - 1; i >= 0; i--) {
        if (g_hits[i].r.Contains(x, y)) {
            return g_hits[i].id;
        }
    }
    return -1;
}

int HitAtWindow(float x, float y) {
    int t = TitleHit(x, y);
    if (t >= 0) {
        return t;
    }
    return y < TB ? -1 : HitAt(x, y - TB);
}

// Asks the window's thread to finish; the worker joins it with Close().
void RequestClose() { InterlockedExchange(&g_quit, 1); }

void BeginRun() {
    g_run.clear();
    for (size_t i = 0; i < g_items.size(); i++) {
        if (g_items[i].ticked) {
            g_run.push_back((int)i);
        }
    }
    g_clickAt = NowMs();
    g_phase = Phase::Running;
    g_focus = -1;
}

void Answer(bool ok) {
    if (g_phase != Phase::Review || g_answered) {
        return;
    }
    g_answerOk = ok;
    if (ok) {
        BeginRun();   // before the answer is announced, so the worker sees the running phase
    }
    g_answered = true;
    SetEvent(g_answerEvt);
}

void Activate(int id) {
    if (id >= 0 && id < (int)g_items.size()) {
        if (g_clickAt < 0) {
            g_items[id].ticked = !g_items[id].ticked;
        }
    } else if (id == kIdAll) {
        bool all = true;
        for (auto& a : g_items) {
            all = all && a.ticked;
        }
        for (auto& a : g_items) {
            a.ticked = !all;
        }
    } else if (id == kIdOk) {
        Answer(true);
    } else if (id == kIdCancel) {
        Answer(false);
    } else if (id == kIdDone) {
        RequestClose();
    } else if (id == kIdMin) {
        ShowWindow(g_wnd, SW_MINIMIZE);
    } else if (id == kIdClose) {
        if (g_phase == Phase::Review) {
            Answer(false);
        } else if (g_phase == Phase::Done) {
            RequestClose();
        }
    }
}

LRESULT CALLBACK Proc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(h, &ps);
            EndPaint(h, &ps);
            return 0;
        }
        case WM_NCHITTEST: {
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            RECT r;
            GetWindowRect(h, &r);
            float x = (pt.x - r.left) / g_scale - M, y = (pt.y - r.top) / g_scale - M;
            float H = TB + g_body;
            if (x < 0 || y < 0 || x >= W || y >= H) {
                return HTTRANSPARENT;   // the shadow margin
            }
            if (y < TB && TitleHit(x, y) < 0) {
                return HTCAPTION;       // drag by the strip
            }
            return HTCLIENT;
        }
        case WM_NCACTIVATE:
            return TRUE;
        case WM_CLOSE:   // Alt+F4 or the taskbar: same as the close button
            if (g_phase == Phase::Review) {
                Answer(false);
            } else if (g_phase == Phase::Done) {
                RequestClose();
            }
            return 0;
        case WM_MOUSEWHEEL:
            g_scrollTarget -= (short)HIWORD(w) / 120.0f * 60.0f;
            return 0;
        case WM_DPICHANGED:
            if (g_settings.uiScale < 1.0f) {
                ApplyScale(h, HIWORD(w) / 96.0f);
            }
            return 0;
        case WM_MOUSEMOVE: {
            float mx, my;
            ToLogical(l, &mx, &my);
            g_titleHover = TitleHit(mx, my);
            g_hover = g_titleHover >= 0 ? -1 : (my < TB ? -1 : HitAt(mx, my - TB));
            TRACKMOUSEEVENT te = {sizeof(te), TME_LEAVE, h, 0};
            TrackMouseEvent(&te);
            return 0;
        }
        case WM_MOUSELEAVE:
            g_hover = -1;
            g_titleHover = -1;
            return 0;
        case WM_SETCURSOR:
            if (LOWORD(l) == HTCLIENT) {
                SetCursor(LoadCursorW(nullptr, (g_hover >= 0 || g_titleHover >= 0) ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            break;
        case WM_LBUTTONDOWN: {
            float mx, my;
            ToLogical(l, &mx, &my);
            g_pressed = HitAtWindow(mx, my);
            if (g_pressed >= 0 && g_pressed < (int)g_items.size()) {
                g_focus = g_pressed;
            }
            SetCapture(h);
            return 0;
        }
        case WM_LBUTTONUP: {
            ReleaseCapture();
            float mx, my;
            ToLogical(l, &mx, &my);
            int id = HitAtWindow(mx, my);
            int pressed = g_pressed;
            g_pressed = -1;
            if (id >= 0 && id == pressed) {
                Activate(id);
            }
            return 0;
        }
        case WM_KEYDOWN: {
            bool review = g_phase == Phase::Review, done = g_phase == Phase::Done && g_doneAt >= 0 && NowMs() > g_doneAt + kP2;
            if (w == VK_ESCAPE) {
                if (review) {
                    Answer(false);
                } else if (done) {
                    RequestClose();
                }
                return 0;
            }
            if (review) {
                std::vector<int> order;
                for (size_t i = 0; i < g_items.size(); i++) {
                    order.push_back((int)i);
                }
                order.push_back(kIdCancel);
                order.push_back(kIdOk);
                int idx = (int)order.size() - 1;
                for (size_t i = 0; i < order.size(); i++) {
                    if (order[i] == g_focus) {
                        idx = (int)i;
                    }
                }
                int n = (int)order.size();
                bool shift = GetKeyState(VK_SHIFT) < 0;
                if (w == VK_TAB || w == VK_DOWN || w == VK_UP || w == VK_RIGHT || w == VK_LEFT) {
                    bool back = (w == VK_TAB && shift) || w == VK_UP || w == VK_LEFT;
                    idx = (idx + (back ? n - 1 : 1)) % n;
                    g_focus = order[idx];
                } else if (w == VK_SPACE) {
                    if (g_focus >= 0) {
                        Activate(g_focus);
                    }
                } else if (w == VK_RETURN) {
                    if (g_focus == kIdCancel) {
                        Answer(false);
                    } else {
                        Answer(true);
                    }
                }
            } else if (done && (w == VK_RETURN || w == VK_SPACE)) {
                RequestClose();
            }
            return 0;
        }
        case WM_DESTROY:
            g_wnd = nullptr;
            SetEvent(g_answerEvt);   // a worker waiting for an answer must not wait for a window that is gone
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

// ---- icons of the listed programs ---------------------------------------------

Color TileColor(const std::wstring& name) {
    static const Color pal[] = {Color(255, 88, 101, 242), Color(255, 29, 185, 84), Color(255, 224, 30, 90),
                                Color(255, 0, 97, 255), Color(255, 42, 171, 238), Color(255, 255, 140, 0),
                                Color(255, 168, 85, 247), Color(255, 20, 170, 160)};
    unsigned h = 0;
    for (wchar_t c : name) {
        h = h * 31 + c;
    }
    return pal[h % 8];
}

Bitmap* IconOf(const std::wstring& path);

// The shell's own picture of a program or Store app at a requested size, with transparency: the same
// icon Explorer and the Start menu show, including the proper tile for Store apps.
Bitmap* ShellImage(const std::wstring& parsingName, int px) {
    IShellItemImageFactory* factory = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(parsingName.c_str(), nullptr, IID_PPV_ARGS(&factory));
    if (FAILED(hr) || !factory) {
        return nullptr;
    }
    HBITMAP hbm = nullptr;
    SIZE sz = {px, px};
    hr = factory->GetImage(sz, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbm);
    factory->Release();
    if (FAILED(hr) || !hbm) {
        return nullptr;
    }
    BITMAP bm = {};
    GetObjectW(hbm, sizeof(bm), &bm);
    Bitmap* out = nullptr;
    if (bm.bmBitsPixel == 32 && bm.bmWidth > 0 && bm.bmHeight > 0) {
        int w = bm.bmWidth, h = bm.bmHeight;
        std::vector<BYTE> bits((size_t)w * h * 4);
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;   // top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        HDC dc = GetDC(nullptr);
        GetDIBits(dc, hbm, 0, h, bits.data(), &bi, DIB_RGB_COLORS);
        ReleaseDC(nullptr, dc);
        // Some icons come back with real transparency, others fully opaque: if no pixel has any
        // transparency, the alpha channel is not used and the picture is taken as it is.
        bool anyAlpha = false;
        for (size_t i = 3; i < bits.size(); i += 4) {
            if (bits[i] != 0) {
                anyAlpha = true;
                break;
            }
        }
        if (!anyAlpha) {
            for (size_t i = 3; i < bits.size(); i += 4) {
                bits[i] = 255;
            }
        }
        Bitmap tmp(w, h, w * 4, PixelFormat32bppPARGB, bits.data());
        out = new Bitmap(w, h, PixelFormat32bppPARGB);
        Graphics g(out);
        g.SetCompositingMode(CompositingModeSourceCopy);
        g.DrawImage(&tmp, 0, 0, w, h);   // copied, so the pixels outlive the vector
    }
    DeleteObject(hbm);
    return out;
}

// Best icon first: the Store app's tile, the program's own high-resolution icon, the shell's 32 px icon.
// If none works the caller draws a tile with the app's initial.
Bitmap* LoadAppIcon(const std::wstring& aumid, const std::wstring& path, int px) {
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    Bitmap* b = nullptr;
    if (!aumid.empty()) {
        b = ShellImage(L"shell:AppsFolder\\" + aumid, px);
    }
    if (!b && !path.empty()) {
        b = ShellImage(path, px);
    }
    if (SUCCEEDED(co)) {
        CoUninitialize();
    }
    return b ? b : IconOf(path);
}

Bitmap* IconOf(const std::wstring& path) {
    if (path.empty()) {
        return nullptr;
    }
    SHFILEINFOW sfi = {};
    if (!SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON) || !sfi.hIcon) {
        return nullptr;
    }
    Bitmap* b = Bitmap::FromHICON(sfi.hIcon);
    DestroyIcon(sfi.hIcon);
    if (b && b->GetLastStatus() != Ok) {
        delete b;
        return nullptr;
    }
    return b;
}

void ClearItems() {
    for (auto& a : g_items) {
        delete a.icon;
        a.icon = nullptr;
    }
    g_items.clear();
}

// ---- public API -----------------------------------------------------------------

bool Alive() { return g_wnd != nullptr; }

// One frame, under the lock.
void RenderLocked() {
    Guard lock;
    Render();
}

// The picture of a program, scaled once to the size it is drawn at, so a frame only has to copy it.
Bitmap* ScaledIcon(Bitmap* big, int dst) {
    if (!big) {
        return nullptr;
    }
    Bitmap* out = new Bitmap(dst, dst, PixelFormat32bppPARGB);
    Graphics g(out);
    g.SetCompositingMode(CompositingModeSourceCopy);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    g.DrawImage(big, Rect(0, 0, dst, dst));
    delete big;
    return out;
}

// The worker said the work is over: measure the summary here, where the fonts live, and show it.
void ApplyFinish() {
    Guard lock;
    Bitmap tmp(4, 4, PixelFormat32bppARGB);
    Graphics gg(&tmp);
    Font& f = *GetFonts()->body;
    RectF box;
    gg.MeasureString(g_doneSummary.c_str(), -1, &f, RectF(0, 0, W - 56, 1000), Fmt(4), &box);
    float lineH = f.GetHeight(&gg);
    int lines = std::clamp((int)std::lround(box.Height / std::max(1.0f, lineH)), 1, 4);
    g_doneBody = 360 + (lines - 1) * lineH;
    g_phase = Phase::Done;
    g_doneAt = NowMs();
    g_focus = kIdDone;
}

// The window's thread: creates the window, then draws and handles input until it is asked to stop.
DWORD WINAPI UiThread(LPVOID) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    GdiplusOn();
    static bool registered = false;
    HINSTANCE inst = GetModuleHandleW(nullptr);
    if (!registered) {
        WNDCLASSEXW wc = {sizeof(wc)};
        wc.lpfnWndProc = Proc;
        wc.hInstance = inst;
        wc.lpszClassName = L"GameModeToggleFlow";
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        registered = RegisterClassExW(&wc) != 0;
    }
    POINT pt;
    GetCursorPos(&pt);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
    g_cx = (mi.rcWork.left + mi.rcWork.right) / 2.0;
    g_cy = (mi.rcWork.top + mi.rcWork.bottom) / 2.0;
    g_lastL = g_lastT = -99999;
    HWND wnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_APPWINDOW | WS_EX_TOPMOST, L"GameModeToggleFlow", L"Game Mode",
                               WS_POPUP | WS_MINIMIZEBOX | WS_SYSMENU, (int)g_cx - 100, (int)g_cy - 100, 200, 200,
                               nullptr, nullptr, inst, nullptr);
    if (!wnd) {
        g_phase = Phase::Closed;
        SetEvent(g_readyEvt);
        return 0;
    }
    g_wnd = wnd;
    ApplyScale(wnd, ScaleFor(wnd));
    {
        int px = std::clamp((int)std::lround(28 * g_scale * 2), 64, 256);
        int dst = std::max(16, (int)std::lround(28 * g_scale));
        for (auto& a : g_items) {
            a.icon = ScaledIcon(LoadAppIcon(a.aumid, a.path, px), dst);
        }
    }
    QueryPerformanceCounter(&g_start);
    // warm up (font, glows) before the clock starts and the window is shown, so the opening animation plays in full
    RenderLocked();
    RenderLocked();
    QueryPerformanceCounter(&g_start);
    if (g_startRunning) {
        Guard lock;
        for (auto& a : g_items) {
            a.ticked = true;
        }
        BeginRun();
        g_clickAt = -(kP1 + 10);   // already in the running look
    }
    RenderLocked();
    ShowWindow(wnd, SW_SHOWNA);
    SetForegroundWindow(wnd);
    SetEvent(g_readyEvt);

    while (!g_quit && g_wnd) {
        MSG m;
        while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
        if (InterlockedExchange(&g_finishPending, 0)) {
            ApplyFinish();
        }
        if (g_phase == Phase::Done && g_doneAt >= 0 && NowMs() - g_doneAt > 16000 && g_hover < 0 && g_titleHover < 0) {
            break;   // closes itself a while after the work is done
        }
        float r0 = NowMs();
        RenderLocked();
        InterlockedExchangeAdd(&g_renderUs, (LONG)((NowMs() - r0) * 1000));
        InterlockedIncrement(&g_frames);
        if (g_phase == Phase::Done && NowMs() - g_doneAt > 2200) {
            MsgWaitForMultipleObjects(0, nullptr, FALSE, 45, QS_ALLINPUT);   // nothing moves any more: draw less often
        } else if (FAILED(DwmFlush())) {
            Sleep(8);
        }
    }
    if (g_wnd) {
        DestroyWindow(g_wnd);
    }
    g_wnd = nullptr;
    g_phase = Phase::Closed;
    SetEvent(g_answerEvt);
    return 0;
}

// Ends the window's thread (and with it the window) and waits for it. For the worker thread only.
void Close() {
    if (g_uiThread) {
        InterlockedExchange(&g_quit, 1);
        WaitForSingleObject(g_uiThread, 4000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_csInit) {
        Guard lock;
        ClearItems();
    }
    FreeIcons();
    g_phase = Phase::Closed;
}

// Plain wait: the window animates by itself, whatever the worker is doing.
void Sleep_(DWORD ms) { Sleep(ms); }

void EnsureWindow(const ChecklistSpec& spec, bool startRunning) {
    Close();
    if (!g_csInit) {
        InitializeCriticalSection(&g_cs);
        g_csInit = true;
    }
    QueryPerformanceFrequency(&g_freq);
    GdiplusOn();
    g_th = MakeTheme(g_settings.theme, IsDarkTheme());
    if (g_themeId != g_settings.theme) {
        FreeCaches();
        g_themeId = g_settings.theme;
    }
    {
        Guard lock;
        g_spec = spec;
        ClearItems();
        for (const auto& it : spec.items) {
            Item a;
            a.name = it.text;
            a.note = it.note;
            a.keepNote = it.keepNote;
            a.path = it.path;
            a.aumid = it.aumid;
            a.tile = TileColor(it.text);
            g_items.push_back(std::move(a));
        }
        g_infoLine.clear();
        for (const auto& s : spec.info) {
            if (!s.empty()) {
                g_infoLine += (g_infoLine.empty() ? L"" : L"  \u00B7  ") + s;
            }
        }
        g_run.clear();
        g_clickAt = g_doneAt = -1;
        g_answered = false;
        g_answerOk = false;
        g_scroll = g_scrollTarget = 0;
        g_hover = g_pressed = g_titleHover = -1;
        g_focus = kIdOk;
        g_doneBody = 360;
        g_phase = Phase::Review;
    }
    g_startRunning = startRunning;
    InterlockedExchange(&g_quit, 0);
    InterlockedExchange(&g_finishPending, 0);
    if (!g_readyEvt) {
        g_readyEvt = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g_answerEvt = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    }
    ResetEvent(g_readyEvt);
    ResetEvent(g_answerEvt);
    g_uiThread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    if (!g_uiThread) {
        g_phase = Phase::Closed;
        return;
    }
    WaitForSingleObject(g_readyEvt, 10000);
}

// Shows the checklist and waits for an answer. On OK the window stays open and
// switches to its progress view; call SetState() as the apps finish.
ChecklistResult Review(const ChecklistSpec& spec) {
    ChecklistResult res;
    res.checked.assign(spec.items.size(), true);
    if (g_wnd) {
        return res;
    }
    EnsureWindow(spec, false);
    if (!g_wnd) {
        Close();
        return res;   // could not make the window: treated as cancelled, so nothing is closed unseen
    }
    while (g_wnd && !g_answered && !g_stopRequested) {
        MsgWaitForMultipleObjects(1, &g_answerEvt, FALSE, 250, QS_ALLINPUT);
        MSG m;
        while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE)) {
            if (!m.hwnd) {   // messages meant for the hotkey loop that is waiting for us
                if (m.message == WM_APP_STOP) {
                    g_stopRequested = true;
                } else if (m.message == WM_APP_RELOAD) {
                    g_reloadRequested = true;
                }
                continue;
            }
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }
    {
        Guard lock;
        for (size_t i = 0; i < g_items.size() && i < res.checked.size(); i++) {
            res.checked[i] = g_items[i].ticked;
        }
    }
    res.ok = g_answered && g_answerOk && !g_stopRequested;
    if (!res.ok) {
        Close();
    }
    if (g_stopRequested) {
        g_stopRequested = false;
        PostThreadMessageW(GetCurrentThreadId(), WM_APP_STOP, 0, 0);
    }
    if (g_reloadRequested) {
        g_reloadRequested = false;
        PostThreadMessageW(GetCurrentThreadId(), WM_APP_RELOAD, 0, 0);
    }
    return res;
}

// Opens straight in the progress view (no checklist), for a run that needs no confirmation.
void Direct(const ChecklistSpec& spec) {
    EnsureWindow(spec, true);
}

// The k-th item that was worked on has finished: state 1 = done, 2 = left running.
void SetState(size_t k, int state) {
    if (!g_wnd) {
        return;
    }
    Guard lock;
    if (k >= g_run.size()) {
        return;
    }
    Item& a = g_items[g_run[k]];
    if (a.state != state) {
        a.state = state;
        a.finishedAt = NowMs();
    }
}

void Finish(const std::wstring& title, const std::wstring& summary) {
    if (!g_wnd) {
        return;
    }
    {
        Guard lock;
        g_doneTitle = title;
        g_doneSummary = summary;
    }
    InterlockedExchange(&g_finishPending, 1);   // the window's thread shows it on its next frame
}

void Shutdown() {
    Close();
    FreeCaches();
    FreeFonts();
    if (g_surface) {
        delete g_surface;
        g_surface = nullptr;
    }
    if (g_dib) {
        DeleteObject(g_dib);
        g_dib = nullptr;
    }
    if (g_memDC) {
        DeleteDC(g_memDC);
        g_memDC = nullptr;
    }
}

}  // namespace flow

// ---- processes -------------------------------------------------------------

struct ProcInfo {
    DWORD pid = 0;
    DWORD parent = 0;
    std::wstring exe;   // file name as reported by the snapshot
    std::wstring path;  // full image path
    unsigned long long created = 0;
};

std::wstring QueryPath(HANDLE h) {
    WCHAR buf[32768];
    DWORD len = ARRAYSIZE(buf);
    if (QueryFullProcessImageNameW(h, 0, buf, &len)) {
        return std::wstring(buf, len);
    }
    return L"";
}

struct UnicodeString {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
};

std::wstring QueryCommandLine(HANDLE h) {
    using NtQueryInformationProcess_t =
        LONG(NTAPI*)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    static auto fn = (NtQueryInformationProcess_t)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationProcess");
    if (!fn) {
        return L"";
    }
    constexpr ULONG kProcessCommandLineInformation = 60;
    ULONG len = 0;
    fn(h, kProcessCommandLineInformation, nullptr, 0, &len);
    if (len == 0 || len > (1 << 20)) {
        return L"";
    }
    std::vector<BYTE> buf(len + 16);
    if (fn(h, kProcessCommandLineInformation, buf.data(), (ULONG)buf.size(),
           &len) < 0) {
        return L"";
    }
    // The string sits inside the buffer; trust no pointer or length from a
    // process we only read through WOW64.
    auto* us = (UnicodeString*)buf.data();
    const BYTE* first = (const BYTE*)us->Buffer;
    if (buf.size() < sizeof(UnicodeString) || first < buf.data() + sizeof(UnicodeString) ||
        first > buf.data() + buf.size() || us->Length % sizeof(WCHAR) != 0 ||
        us->Length > (size_t)(buf.data() + buf.size() - first)) {
        return L"";
    }
    return std::wstring(us->Buffer, us->Length / sizeof(WCHAR));
}

std::wstring QueryAumid(HANDLE h) {
    using GetAumid_t = LONG(WINAPI*)(HANDLE, UINT32*, PWSTR);
    static auto fn = (GetAumid_t)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "GetApplicationUserModelId");
    if (!fn) {
        return L"";
    }
    WCHAR buf[256];
    UINT32 len = ARRAYSIZE(buf);
    if (fn(h, &len, buf) != ERROR_SUCCESS || len == 0) {
        return L"";
    }
    return std::wstring(buf, len - 1);
}

unsigned long long QueryCreated(HANDLE h) {
    FILETIME c, e, k, u;
    if (!GetProcessTimes(h, &c, &e, &k, &u)) {
        return 0;
    }
    return ((unsigned long long)c.dwHighDateTime << 32) | c.dwLowDateTime;
}

// Every process of this logon session whose image path can be read.
std::vector<ProcInfo> ListProcesses() {
    std::vector<ProcInfo> list;
    DWORD mySession = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &mySession);
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return list;
    }
    PROCESSENTRY32W pe = {sizeof(pe)};
    for (BOOL ok = Process32FirstW(snap, &pe); ok;
         ok = Process32NextW(snap, &pe)) {
        DWORD session = 0;
        if (!ProcessIdToSessionId(pe.th32ProcessID, &session) ||
            session != mySession) {
            continue;
        }
        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                               pe.th32ProcessID);
        if (!h) {
            continue;
        }
        ProcInfo p;
        p.pid = pe.th32ProcessID;
        p.parent = pe.th32ParentProcessID;
        p.exe = pe.szExeFile;
        p.path = QueryPath(h);
        p.created = QueryCreated(h);
        CloseHandle(h);
        if (!p.path.empty()) {
            list.push_back(p);
        }
    }
    CloseHandle(snap);
    return list;
}

// `created` 0 skips the check that the PID was not reused by another process.
bool PidRunning(DWORD pid, unsigned long long created) {
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                           FALSE, pid);
    if (!h) {
        return false;
    }
    bool running = WaitForSingleObject(h, 0) == WAIT_TIMEOUT;
    if (running && created != 0 && QueryCreated(h) != created) {
        running = false;
    }
    CloseHandle(h);
    return running;
}

std::set<std::wstring> RunningPaths() {
    std::set<std::wstring> paths;
    for (const auto& p : ListProcesses()) {
        paths.insert(Lower(p.path));
    }
    return paths;
}

// ---- windows ---------------------------------------------------------------

struct WindowInfo {
    HWND hwnd;
    DWORD pid;
    bool taskbar;  // an app window as shown in Alt+Tab
};

bool IsTaskbarWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER)) {
        return false;
    }
    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
        return false;
    }
    if (GetWindowTextLengthW(hwnd) == 0) {
        return false;
    }
    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return false;
    }
    return true;
}

// Window callbacks are named CALLBACK functions, not lambdas: on 32-bit builds
// WNDENUMPROC is stdcall, which a lambda does not convert to.
BOOL CALLBACK CollectWindowProc(HWND hwnd, LPARAM lp) {
    auto* out = (std::vector<WindowInfo>*)lp;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    out->push_back({hwnd, pid, IsTaskbarWindow(hwnd)});
    return TRUE;
}

std::vector<WindowInfo> ListTopLevelWindows() {
    std::vector<WindowInfo> list;
    EnumWindows(CollectWindowProc, (LPARAM)&list);
    return list;
}

struct FrameSearch {
    DWORD host;
    DWORD found;
};

BOOL CALLBACK FindAppChildProc(HWND child, LPARAM lp) {
    auto* c = (FrameSearch*)lp;
    DWORD pid = 0;
    GetWindowThreadProcessId(child, &pid);
    if (pid && pid != c->host) {
        c->found = pid;
        return FALSE;
    }
    return TRUE;
}

// A Store app's window is a frame owned by ApplicationFrameHost; the app's
// own process owns a CoreWindow child. Returns that process id, or 0.
DWORD StoreAppPidOfFrame(HWND frame, DWORD hostPid) {
    FrameSearch ctx = {hostPid, 0};
    EnumChildWindows(frame, FindAppChildProc, (LPARAM)&ctx);
    return ctx.found;
}

// ---- app groups ------------------------------------------------------------

// All processes of one program (same image path), closed and reopened as one.
struct AppGroup {
    std::wstring name;  // shown in the list
    std::wstring path;
    std::wstring exe;
    std::wstring args;
    std::wstring aumid;
    std::map<DWORD, unsigned long long> pids;  // pid -> creation time
    std::vector<HWND> windows;         // every top-level window, hidden ones included
    std::vector<HWND> visibleWindows;  // the ones a user could click the X of
    bool forced = false;        // listed under "Apps to force close"
    bool background = false;    // no taskbar window: a tray or background app
    bool windowless = false;    // no window at all: it cannot be asked to close
};

// A text field from the program's version resource (FileDescription,
// CompanyName, ...), or empty.
std::wstring FileVersionString(const std::wstring& path, const wchar_t* key) {
    DWORD ignored = 0;
    DWORD size = GetFileVersionInfoSizeW(path.c_str(), &ignored);
    if (!size) {
        return L"";
    }
    std::vector<BYTE> buf(size);
    struct Translation {
        WORD lang;
        WORD codepage;
    }* tr = nullptr;
    UINT len = 0;
    if (!GetFileVersionInfoW(path.c_str(), 0, size, buf.data()) ||
        !VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation", (LPVOID*)&tr, &len) ||
        len < sizeof(Translation)) {
        return L"";
    }
    WCHAR sub[96];
    swprintf_s(sub, L"\\StringFileInfo\\%04x%04x\\%s", tr[0].lang, tr[0].codepage, key);
    PWSTR value = nullptr;
    UINT vlen = 0;
    if (VerQueryValueW(buf.data(), sub, (LPVOID*)&value, &vlen) && value && vlen > 1) {
        return Trim(value);
    }
    return L"";
}

// The name the program calls itself ("Google Chrome"), else the file name.
std::wstring DisplayName(const std::wstring& path) {
    std::wstring name = FileVersionString(path, L"FileDescription");
    if (!name.empty()) {
        return name;
    }
    std::wstring n = BaseName(path);
    if (n.size() > 4 && Lower(n.substr(n.size() - 4)) == L".exe") {
        n.resize(n.size() - 4);
    }
    return n;
}

// 2 = Windows GUI program, 3 = console program. Console tools (git, node,
// shells) are never closed; unreadable files are treated as not-GUI.
bool IsGuiExe(const std::wstring& path) {
    static std::map<std::wstring, int> cache;
    std::wstring key = Lower(path);
    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second == 2;
    }
    int subsystem = 0;
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           nullptr, OPEN_EXISTING, 0, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
        BYTE dos[64];
        DWORD got = 0;
        if (ReadFile(h, dos, sizeof(dos), &got, nullptr) && got == sizeof(dos) &&
            dos[0] == 'M' && dos[1] == 'Z') {
            LONG pe = *(LONG*)(dos + 0x3C);
            BYTE hdr[96];
            if (pe > 0 && pe < (1 << 20) &&
                SetFilePointer(h, pe, nullptr, FILE_BEGIN) != INVALID_SET_FILE_POINTER &&
                ReadFile(h, hdr, sizeof(hdr), &got, nullptr) && got == sizeof(hdr) &&
                hdr[0] == 'P' && hdr[1] == 'E') {
                subsystem = *(WORD*)(hdr + 4 + 20 + 68);
            }
        }
        CloseHandle(h);
    }
    cache[key] = subsystem;
    return subsystem == 2;
}

bool CanControl(DWORD pid) {
    HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!h) {
        return false;  // elevated or protected: it cannot be closed from here
    }
    CloseHandle(h);
    return true;
}

std::vector<AppGroup> CollectGroups(const Settings& s) {
    std::vector<ProcInfo> procs = ListProcesses();
    std::vector<WindowInfo> windows = ListTopLevelWindows();
    DWORD self = GetCurrentProcessId();
    std::map<std::wstring, AppGroup> groups;  // key: lower-case image path
    std::map<DWORD, const ProcInfo*> byPid;
    for (const auto& p : procs) {
        byPid[p.pid] = &p;
    }

    auto addPid = [&](const ProcInfo& p) {
        AppGroup& g = groups[Lower(p.path)];
        if (g.path.empty()) {
            g.path = p.path;
            g.exe = BaseName(p.path);
            g.name = DisplayName(p.path);
        }
        g.pids[p.pid] = p.created;
    };

    // Processes that own a taskbar window (a Store app's window is owned by
    // ApplicationFrameHost, so it is mapped back to the app's own process).
    std::set<DWORD> taskbarPids;
    for (const auto& w : windows) {
        if (!w.taskbar) {
            continue;
        }
        DWORD pid = w.pid;
        auto it = byPid.find(pid);
        if (it != byPid.end() && ExeKey(it->second->exe) == L"applicationframehost") {
            pid = StoreAppPidOfFrame(w.hwnd, w.pid);
        }
        if (pid) {
            taskbarPids.insert(pid);
        }
    }

    // A program started by an app you keep (Steam's web helper, a launcher's
    // overlay) stays with it. Windows itself does not count as a keeper here,
    // or everything started by Explorer would be kept.
    auto startedByKeptApp = [&](const ProcInfo& p) {
        DWORD cur = p.pid;
        for (int hop = 0; hop < 12; hop++) {
            auto child = byPid.find(cur);
            if (child == byPid.end()) {
                return false;
            }
            auto parent = byPid.find(child->second->parent);
            if (parent == byPid.end() || parent->second->created >= child->second->created) {
                return false;
            }
            const wchar_t* why = WhyKeep(parent->second->exe, parent->second->path, g_windowsDir, s);
            if (why && (wcscmp(why, L"in Apps to keep") == 0 || wcscmp(why, L"in Folders to keep") == 0 ||
                        wcscmp(why, L"in a game folder") == 0)) {
                return true;
            }
            cur = parent->first;
        }
        return false;
    };

    // 1) Listed apps, then 2) every other program that is not needed for gaming.
    for (const auto& p : procs) {
        if (p.pid == self || !CanControl(p.pid)) {
            continue;
        }
        const bool listed = InNameList(s.closeApps, p.exe) && !IsShellProcess(p.exe);
        if (!listed) {
            if (!s.closeOthers || IsShellProcess(p.exe) || LooksLikeHelper(p.exe)) {
                continue;
            }
            if (!taskbarPids.count(p.pid) && !s.closeBackground) {
                continue;
            }
            const wchar_t* why = WhyKeep(p.exe, p.path, g_windowsDir, s);
            if (why) {
                continue;
            }
            if (!IsGuiExe(p.path)) {
                continue;
            }
            if (s.keepVendorApps) {
                static std::map<std::wstring, std::wstring> makerOf;  // path -> who made it
                std::wstring key = Lower(p.path);
                if (!makerOf.count(key)) {
                    makerOf[key] = FileVersionString(p.path, L"CompanyName") + L" | " +
                                   FileVersionString(p.path, L"ProductName") + L" | " +
                                   InstallFolderName(p.path);
                }
                if (VendorMatches(makerOf[key], s.keepVendors)) {
                    continue;
                }
            }
            if (startedByKeptApp(p)) {
                continue;
            }
        }
        addPid(p);
    }

    // A helper that was started by another program on the list belongs to it
    // (PowerToys' modules, a browser's crash handler): it is listed once, under
    // the program that started it. The parent must be older than the child, so
    // a reused process id never links two unrelated programs.
    for (bool merged = true; merged;) {
        merged = false;
        std::map<DWORD, std::wstring> keyOfPid;
        for (const auto& kv : groups) {
            for (const auto& pc : kv.second.pids) {
                keyOfPid[pc.first] = kv.first;
            }
        }
        for (auto it = groups.begin(); it != groups.end(); ++it) {
            std::wstring parentKey;
            for (const auto& pc : it->second.pids) {
                DWORD cur = pc.first;
                for (int hop = 0; hop < 12 && parentKey.empty(); hop++) {
                    auto child = byPid.find(cur);
                    if (child == byPid.end()) {
                        break;
                    }
                    auto parent = byPid.find(child->second->parent);
                    if (parent == byPid.end() ||
                        parent->second->created >= child->second->created) {
                        break;
                    }
                    auto owner = keyOfPid.find(parent->first);
                    if (owner != keyOfPid.end() && owner->second != it->first) {
                        parentKey = owner->second;
                    }
                    cur = parent->first;
                }
                if (!parentKey.empty()) {
                    break;
                }
            }
            if (!parentKey.empty()) {
                for (const auto& pc : it->second.pids) {
                    groups[parentKey].pids[pc.first] = pc.second;
                }
                groups.erase(it);
                merged = true;
                break;  // the iterator is gone: rebuild the pid map and start over
            }
        }
    }

    std::vector<AppGroup> result;
    for (auto& kv : groups) {
        AppGroup& g = kv.second;
        g.background = true;
        for (const auto& pc : g.pids) {
            if (taskbarPids.count(pc.first)) {
                g.background = false;
            }
        }
        // Copy the command line from the oldest process of the program.
        const ProcInfo* root = nullptr;
        for (const auto& pc : g.pids) {
            const ProcInfo* p = byPid[pc.first];
            if (p && (!root || p->created < root->created)) {
                root = p;
            }
        }
        if (!root) {
            continue;
        }
        for (const auto& w : windows) {
            if (g.pids.count(w.pid)) {
                g.windows.push_back(w.hwnd);
                continue;
            }
            auto it = byPid.find(w.pid);
            if (it != byPid.end() &&
                ExeKey(it->second->exe) == L"applicationframehost" &&
                g.pids.count(StoreAppPidOfFrame(w.hwnd, w.pid))) {
                g.windows.push_back(w.hwnd);  // the Store app's frame window
            }
        }
        g.forced = InNameList(s.forceApps, g.exe);
        g.windowless = g.windows.empty();
        for (HWND w : g.windows) {
            if (IsWindowVisible(w)) {
                g.visibleWindows.push_back(w);
            }
        }
        HANDLE h =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, root->pid);
        if (h) {
            g.aumid = QueryAumid(h);
            if (g.aumid.empty()) {
                g.args = ArgsFromCommandLine(QueryCommandLine(h));
            }
            CloseHandle(h);
        }
        result.push_back(g);
    }
    std::sort(result.begin(), result.end(),
              [](const AppGroup& a, const AppGroup& b) {
                  return Lower(a.name) < Lower(b.name);
              });
    return result;
}

bool GroupRunning(const AppGroup& g) {
    for (const auto& pc : g.pids) {
        if (PidRunning(pc.first, pc.second)) {
            return true;
        }
    }
    return false;
}

// ---- power plan ------------------------------------------------------------

std::vector<std::pair<std::wstring, std::wstring>> ListPowerPlans() {
    std::vector<std::pair<std::wstring, std::wstring>> plans;
    for (ULONG i = 0; i < 64; i++) {
        GUID g;
        DWORD size = sizeof(g);
        if (PowerEnumerate(nullptr, nullptr, nullptr, ACCESS_SCHEME, i,
                           (UCHAR*)&g, &size) != ERROR_SUCCESS) {
            break;
        }
        WCHAR name[256] = {};
        DWORD nameSize = sizeof(name);
        PowerReadFriendlyName(nullptr, &g, nullptr, nullptr, (PUCHAR)name,
                              &nameSize);
        plans.push_back({GuidToString(g), name});
    }
    return plans;
}

std::wstring ActivePlan() {
    GUID* g = nullptr;
    if (PowerGetActiveScheme(nullptr, &g) != ERROR_SUCCESS || !g) {
        return L"";
    }
    std::wstring s = GuidToString(*g);
    LocalFree(g);
    return s;
}

bool SetPlan(const std::wstring& guidText) {
    GUID g;
    return GuidFromString(guidText, &g) &&
           PowerSetActiveScheme(nullptr, &g) == ERROR_SUCCESS;
}

// The plan to switch to for the setting, or empty when it does not exist.
std::wstring ResolveGamePlan(const std::wstring& setting, std::wstring* label) {
    auto plans = ListPowerPlans();
    if (setting == L"high") {
        for (const auto& p : plans) {
            if (p.first == kGuidHigh) {
                *label = L"High performance";
                return p.first;
            }
        }
    } else if (setting == L"ultimate") {
        for (const auto& p : plans) {  // the built-in GUID first: the name is localized
            if (p.first == kGuidUltimate) {
                *label = p.second;
                return p.first;
            }
        }
        for (const auto& p : plans) {
            if (Lower(p.second).find(L"ultimate") != std::wstring::npos) {
                *label = p.second;
                return p.first;
            }
        }
    }
    return L"";
}

// ---- turning on ------------------------------------------------------------

ChecklistSpec OnChecklist(const std::vector<ChecklistItem>& items,
                          bool changePlan,
                          bool planNotSet,
                          const std::wstring& planLabel,
                          size_t gameCount) {
    ChecklistSpec spec;
    spec.title = L"Turn on Game Mode?";
    spec.verb = L"Closes";
    spec.okText = L"Turn on";
    spec.subtitle = L"{n} app{s} will close. The hotkey brings them back.";
    spec.runTitle = L"Closing apps";
    spec.runHint = L"Asking apps to close politely";
    spec.doneVerb = L"closed";
    spec.items = items;
    spec.startExpanded = true;  // the user must see every name before it is closed
    spec.info.push_back(changePlan ? L"Power plan: " + planLabel
                        : planNotSet
                            ? L"Power plan: not changed"
                            : L"Power plan: " + planLabel + L" (already on)");
    spec.info.push_back(gameCount ? L"Boosts " + std::to_wstring(gameCount) + L" running game" +
                                        (gameCount == 1 ? L"" : L"s")
                                  : L"No game running yet");
    spec.hint = L"Press the hotkey again to restore everything exactly as it is now.";
    return spec;
}

ChecklistSpec OffChecklist(const std::vector<ChecklistItem>& items,
                           bool planRestored) {
    ChecklistSpec spec;
    spec.title = L"Turn off Game Mode?";
    spec.verb = L"Reopens";
    spec.okText = L"Turn off";
    spec.subtitle = L"{n} app{s} will be reopened.";
    spec.runTitle = L"Reopening apps";
    spec.runHint = L"Starting your apps again";
    spec.doneVerb = L"reopened";
    spec.items = items;
    spec.info.push_back(planRestored ? L"Restores the power plan from before"
                                     : L"Power plan: not changed by Game Mode");
    spec.hint = L"Everything ticked above is put back as it was before.";
    return spec;
}

// What the checklist shows next to an app name.
std::wstring NoteFor(const AppGroup& g) {
    if (g.forced) {
        return L"force close";
    }
    if (g.windowless) {
        return L"ends process";
    }
    return g.background ? L"background" : L"";
}

// "Closing: A, B, C and 4 more" for the apps that are still open.
std::wstring NamesOfOpen(const std::vector<AppGroup>& groups, const std::vector<bool>& done) {
    std::wstring names;
    size_t shown = 0, open = 0;
    for (size_t i = 0; i < groups.size(); i++) {
        if (done[i]) {
            continue;
        }
        open++;
        if (shown < 3) {
            names += (shown ? L", " : L"") + groups[i].name;
            shown++;
        }
    }
    if (open > shown) {
        names += L" and " + std::to_wstring(open - shown) + L" more";
    }
    return names;
}

// The summary goes into the window when there is one, otherwise into a notification.
void ShowResult(const std::wstring& title, const std::wstring& summary) {
    if (flow::Alive()) {
        flow::Finish(title, summary);
    } else {
        Notify(summary);
    }
}

void TurnOn() {
    const Settings s = g_settings;
    std::vector<AppGroup> groups = CollectGroups(s);

    std::wstring planLabel;
    std::wstring current = ActivePlan();
    std::wstring target =
        s.plan == L"none" ? L"" : ResolveGamePlan(s.plan, &planLabel);
    bool changePlan = !target.empty() && target != current;

    // Games that are running now.
    std::vector<ProcInfo> games;
    if (s.boostGames && !s.gameFolders.empty()) {
        for (const auto& p : ListProcesses()) {
            if (InAnyFolder(p.path, s.gameFolders)) {
                games.push_back(p);
            }
        }
    }

    if (s.confirm) {
        std::vector<ChecklistItem> items;
        for (const auto& g : groups) {
            items.push_back({g.name, NoteFor(g), g.forced || g.windowless, g.path, g.aumid});
        }
        ChecklistResult choice = flow::Review(
            OnChecklist(items, changePlan, target.empty(), planLabel, games.size()));
        if (!choice.ok) {
            Wh_Log(L"ON cancelled");
            return;
        }
        std::vector<AppGroup> picked;  // only what was left ticked
        for (size_t i = 0; i < groups.size(); i++) {
            if (choice.checked[i]) {
                picked.push_back(groups[i]);
            } else {
                Wh_Log(L"Left alone this time: %s", groups[i].name.c_str());
            }
        }
        groups = picked;
        if (!s.showProgress) {
            flow::Close();   // the progress view is switched off: only the notification at the end
        }
    } else if (s.showProgress && !groups.empty()) {
        // No checklist: the window opens straight in its progress view.
        std::vector<ChecklistItem> items;
        for (const auto& g : groups) {
            items.push_back({g.name, NoteFor(g), g.forced || g.windowless, g.path, g.aumid});
        }
        flow::Direct(OnChecklist(items, changePlan, target.empty(), planLabel, games.size()));
    }

    // Snapshot first: if anything below is interrupted, a restore knows what
    // to do. Apps that fail to close are dropped from it afterwards.
    Snapshot sn;
    sn.startedAt = NowFileTime();
    sn.logon = CurrentLogonId();
    sn.plan = changePlan ? current : L"";
    for (const auto& g : groups) {
        sn.apps.push_back({g.name, g.path, g.args, g.aumid});
    }
    for (const auto& p : games) {
        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, p.pid);
        if (!h) {
            continue;
        }
        DWORD cls = GetPriorityClass(h);
        CloseHandle(h);
        if (cls) {
            sn.prios.push_back({p.pid, p.exe, cls});
        }
    }
    if (!WriteSnapshot(sn)) {
        Wh_Log(L"Could not write the snapshot; nothing was changed");
        flow::Close();
        Notify(L"Could not save the snapshot, so nothing was changed.");
        return;
    }

    if (changePlan) {
        Wh_Log(L"Power plan %s -> %s", current.c_str(), target.c_str());
        if (!SetPlan(target)) {
            Wh_Log(L"SetPlan failed");
            sn.plan = L"";
        }
    }

    // An app may be ended (not just asked to close) when it is on the force
    // list, or when it has no window to ask and the user saw it in the checklist
    // and left it ticked.
    auto mayEnd = [&](const AppGroup& g) {
        return g.forced || (g.windowless && s.confirm);
    };
    auto endProcesses = [](const AppGroup& g) {
        for (const auto& pc : g.pids) {
            HANDLE h = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pc.first);
            if (h) {
                TerminateProcess(h, 1);
                for (ULONGLONG until = GetTickCount64() + 2000;
                     GetTickCount64() < until && WaitForSingleObject(h, 0) == WAIT_TIMEOUT;) {
                    flow::Sleep_(16);   // keeps the window animating while the process goes away
                }
                CloseHandle(h);
            }
        }
    };

    // Close everything at once: one round of WM_CLOSE, one shared wait.
    //
    // The close request goes to the windows a user could click the X of. Hidden
    // windows (the hotkey and tray helpers of a tray app) get it only for an app
    // with nothing visible to close, and only when this app may be ended if it
    // then refuses to quit: an app that survives after losing its hidden
    // windows is alive but broken (no tray icon, no hotkey).
    for (const auto& g : groups) {
        const bool mayEndIfStuck = g.forced || s.confirm;
        const std::vector<HWND> none;
        const std::vector<HWND>& targets =
            !g.visibleWindows.empty() ? g.visibleWindows
            : mayEndIfStuck           ? g.windows
                                      : none;
        Wh_Log(L"Closing %s (%zu process(es), %zu of %zu window(s))", g.name.c_str(),
               g.pids.size(), targets.size(), g.windows.size());
        if (targets.empty() && !g.windows.empty()) {
            Wh_Log(L"%s only has hidden windows and may not be ended: left untouched", g.name.c_str());
        }
        for (HWND w : targets) {
            PostMessageW(w, WM_CLOSE, 0, 0);
        }
        if (g.windowless && mayEnd(g)) {
            Wh_Log(L"Ending %s (no window to close)", g.name.c_str());
            endProcesses(g);
        }
    }
    ULONGLONG deadline = GetTickCount64() + (ULONGLONG)s.timeoutSec * 1000;
    std::vector<bool> done(groups.size(), false);
    std::vector<char> reported(groups.size(), 0);
    // Tells the window which apps have finished (1) or stayed running (2, at the end).
    auto syncStates = [&](bool last = false) {
        for (size_t i = 0; i < groups.size(); i++) {
            if (reported[i]) {
                continue;
            }
            if (done[i]) {
                flow::SetState(i, 1);
                reported[i] = 1;
            } else if (last) {
                flow::SetState(i, 2);
                reported[i] = 1;
            }
        }
    };
    while (true) {
        bool pending = false;
        for (size_t i = 0; i < groups.size(); i++) {
            if (!done[i] && !GroupRunning(groups[i])) {
                done[i] = true;
            }
            pending = pending || !done[i];
        }
        if (!pending || GetTickCount64() >= deadline) {
            break;
        }
        syncStates();
        flow::Sleep_(250);
    }

    // Some apps (WhatsApp, Phone Link, tray tools) answer a close request by
    // hiding their window and carrying on. Their windows are gone but the
    // process lives on, so without this they would stay in the background and
    // never be put back. After a short extra wait (a browser may still be
    // saving its session) they are ended, if the user left them ticked.
    auto hasVisibleWindow = [](const AppGroup& g) {
        for (HWND w : g.windows) {
            if (IsWindow(w) && IsWindowVisible(w)) {
                return true;  // still showing something, e.g. "save changes?"
            }
        }
        return false;
    };
    auto hidToTray = [&](size_t i) {
        return !done[i] && !groups[i].windowless && !hasVisibleWindow(groups[i]);
    };
    bool anyHidden = false;
    for (size_t i = 0; i < groups.size(); i++) {
        anyHidden = anyHidden || hidToTray(i);
    }
    if (anyHidden) {
        ULONGLONG grace = GetTickCount64() + (ULONGLONG)std::min(20, s.timeoutSec * 2) * 1000;
        while (GetTickCount64() < grace) {
            bool waiting = false;
            for (size_t i = 0; i < groups.size(); i++) {
                if (hidToTray(i)) {
                    if (!GroupRunning(groups[i])) {
                        done[i] = true;
                    } else {
                        waiting = true;
                    }
                }
            }
            if (!waiting) {
                break;
            }
            syncStates();
            flow::Sleep_(250);
        }
        for (size_t i = 0; i < groups.size(); i++) {
            if (hidToTray(i) && (groups[i].forced || s.confirm)) {
                Wh_Log(L"Ending %s: it hid to the tray instead of closing", groups[i].name.c_str());
                endProcesses(groups[i]);
                done[i] = !GroupRunning(groups[i]);
            }
        }
    }
    int closed = 0;
    int left = 0;
    Snapshot after = sn;
    after.apps.clear();
    for (size_t i = 0; i < groups.size(); i++) {
        const AppGroup& g = groups[i];
        if (!done[i] && g.forced) {
            Wh_Log(L"Force closing %s", g.name.c_str());
            endProcesses(g);
        }
        if (!done[i]) {
            done[i] = !GroupRunning(g);
        }
        if (done[i]) {
            closed++;
            after.apps.push_back({g.name, g.path, g.args, g.aumid});
        } else {
            left++;
            Wh_Log(L"%s did not close (%s); left running", g.name.c_str(),
                   hasVisibleWindow(g) ? L"it is still showing a window, maybe asking to save"
                                       : L"it only hid its window");
        }
    }

    syncStates(true);

    // Priority last, on the games that were running.
    int boosted = 0;
    for (const auto& pr : after.prios) {
        HANDLE h = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pr.pid);
        if (h) {
            boosted += SetPriorityClass(h, HIGH_PRIORITY_CLASS) ? 1 : 0;
            CloseHandle(h);
        }
    }
    WriteSnapshot(after);

    std::wstring msg = L"On: closed " + std::to_wstring(closed) + L" app(s)";
    if (left) {
        msg += L", " + std::to_wstring(left) + L" left running";
    }
    msg += changePlan ? L", power plan set." : L".";
    if (boosted) {
        msg += L" " + std::to_wstring(boosted) + L" game process(es) boosted.";
    }
    Wh_Log(L"%s", msg.c_str());
    std::wstring summary = L"Closed " + std::to_wstring(closed) + L" of " + std::to_wstring(groups.size()) + L" app(s).";
    if (left) {
        summary += L" Still running: " + NamesOfOpen(groups, done) + L".";
    }
    if (changePlan) {
        summary += L" Power plan set.";
    }
    if (boosted) {
        summary += L" " + std::to_wstring(boosted) + L" game process(es) boosted.";
    }
    ShowResult(left ? L"Game Mode is on (with exceptions)" : L"Game Mode is on", summary);
}

// ---- turning off -----------------------------------------------------------

// ---- reopening in the background ---------------------------------------------
//
// Reopened apps are started minimized and without taking the focus. Plenty of apps ignore that request and
// would flash over the game, so while apps are being started a hook watches for new windows of exactly those
// apps and minimizes each one the moment it appears. The hook runs on its own short-lived thread.

namespace reopen {

struct Target {
    std::wstring path;   // lower case
    std::wstring aumid;
    DWORD expires = 0;   // GetTickCount() value: an app is only watched for a short while after it was started,
                         // so a window you open yourself later is never touched
};

CRITICAL_SECTION g_cs;
bool g_csInit = false;
std::vector<Target> g_targets;
HANDLE g_thread = nullptr;
volatile LONG g_stop = 0;
volatile DWORD g_deadline = 0;   // GetTickCount() value after which the watching ends
std::set<HWND> g_minimized;      // only touched on the watcher's thread

bool Matches(HWND h) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid) {
        return false;
    }
    std::vector<Target> targets;
    EnterCriticalSection(&g_cs);
    targets = g_targets;
    LeaveCriticalSection(&g_cs);
    HANDLE ph = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!ph) {
        return false;
    }
    std::wstring path = Lower(QueryPath(ph));
    std::wstring aumid = QueryAumid(ph);
    CloseHandle(ph);
    if (ExeKey(BaseName(path)) == L"applicationframehost") {
        // a Store app's window belongs to ApplicationFrameHost: look at the app behind it
        DWORD real = StoreAppPidOfFrame(h, pid);
        HANDLE rp = real ? OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, real) : nullptr;
        if (!rp) {
            return false;
        }
        path = Lower(QueryPath(rp));
        aumid = QueryAumid(rp);
        CloseHandle(rp);
    }
    for (const auto& t : targets) {
        if ((LONG)(GetTickCount() - t.expires) >= 0) {
            continue;
        }
        if ((!t.aumid.empty() && aumid == t.aumid) || (!t.path.empty() && path == t.path)) {
            return true;
        }
    }
    return false;
}

void CALLBACK OnShow(HWINEVENTHOOK, DWORD, HWND h, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !h || GetAncestor(h, GA_ROOT) != h) {
        return;
    }
    LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    if ((ex & WS_EX_TOOLWINDOW) || GetWindow(h, GW_OWNER) || !IsWindowVisible(h) || IsIconic(h)) {
        return;   // tool windows, dialogs and popups of an app are left alone
    }
    if (g_minimized.count(h) || !Matches(h)) {
        return;
    }
    g_minimized.insert(h);
    ShowWindowAsync(h, SW_SHOWMINNOACTIVE);   // asynchronous: a slow app cannot hold the watcher up
}

DWORD WINAPI WatchThread(LPVOID) {
    MSG m;
    PeekMessageW(&m, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // make the queue
    HWINEVENTHOOK hook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, OnShow, 0, 0,
                                         WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    while (!g_stop && (LONG)(GetTickCount() - g_deadline) < 0) {
        MsgWaitForMultipleObjects(0, nullptr, FALSE, 100, QS_ALLINPUT);
        while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }
    if (hook) {
        UnhookWinEvent(hook);
    }
    return 0;
}

// Starts watching (call before the first app is started).
void Begin() {
    if (!g_csInit) {
        InitializeCriticalSection(&g_cs);
        g_csInit = true;
    }
    g_deadline = GetTickCount() + 13000;
    if (g_thread && WaitForSingleObject(g_thread, 0) == WAIT_TIMEOUT) {
        return;   // already watching: the deadline above is enough
    }
    if (g_thread) {
        CloseHandle(g_thread);
    }
    g_minimized.clear();
    g_stop = 0;
    g_thread = CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
}

// The app is about to be started: its windows are to be minimized. Call this before starting it, so a window
// that appears the instant the process exists is already recognised.
void Expect(const std::wstring& path, const std::wstring& aumid) {
    if (!g_csInit) {
        return;
    }
    EnterCriticalSection(&g_cs);
    std::wstring norm = Lower(path);
    std::replace(norm.begin(), norm.end(), L'/', L'\\');   // running programs report their path with backslashes
    g_targets.push_back({norm, aumid, GetTickCount() + 12000});
    LeaveCriticalSection(&g_cs);
    g_deadline = GetTickCount() + 13000;
}

void Stop() {
    if (g_thread) {
        g_stop = 1;
        WaitForSingleObject(g_thread, 2000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_csInit) {
        EnterCriticalSection(&g_cs);
        g_targets.clear();
        LeaveCriticalSection(&g_cs);
    }
}

}  // namespace reopen

bool Relaunch(const SavedApp& a) {
    if (!a.aumid.empty()) {
        std::wstring target = L"shell:AppsFolder\\" + a.aumid;
        return (INT_PTR)ShellExecuteW(nullptr, L"open", L"explorer.exe",
                                      target.c_str(), nullptr,
                                      g_settings.reopenBackground ? SW_SHOWMINNOACTIVE : SW_SHOWNORMAL) > 32;
    }
    if (GetFileAttributesW(a.path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    std::wstring cmd =
        L"\"" + a.path + L"\"" + (a.args.empty() ? L"" : L" " + a.args);
    std::wstring dir = a.path.substr(0, a.path.find_last_of(L'\\'));
    STARTUPINFOW si = {sizeof(si)};
    if (g_settings.reopenBackground) {
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOWMINNOACTIVE;   // minimized, and the focus stays where it is
    }
    PROCESS_INFORMATION pi = {};
    if (!CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr,
                        dir.c_str(), &si, &pi)) {
        return false;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

void RestorePriorities(const Snapshot& sn) {
    for (const auto& p : sn.prios) {
        if (!PidRunning(p.pid, 0)) {
            continue;
        }
        HANDLE h = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_INFORMATION, FALSE,
            p.pid);
        if (!h) {
            continue;
        }
        if (Lower(BaseName(QueryPath(h))) == Lower(p.exe)) {
            SetPriorityClass(h, p.priorityClass);
        }
        CloseHandle(h);
    }
}

// False when the saved plan could not be put back.
bool RestorePlan(const Snapshot& sn) {
    if (sn.plan.empty() || ActivePlan() == sn.plan) {
        return true;
    }
    std::wstring target = sn.plan;
    bool exists = false;
    for (const auto& p : ListPowerPlans()) {
        exists = exists || p.first == target;
    }
    if (!exists) {
        Wh_Log(L"Saved power plan %s no longer exists; using Balanced",
               target.c_str());
        target = kGuidBalanced;
    }
    if (!SetPlan(target)) {
        Wh_Log(L"Could not restore the power plan");
        return false;
    }
    return true;
}

// The mod is being unloaded (disabled, updated or removed) while Game Mode is
// on: put the machine-wide power plan and the priorities back now. The closed
// apps stay in the snapshot, so the next hotkey press can still reopen them.
void RestoreOnUnload() {
    Snapshot sn;
    if (LoadSnapshot(&sn) != LoadResult::Ok) {
        return;
    }
    RestorePriorities(sn);
    if (RestorePlan(sn)) {
        sn.plan.clear();
        sn.prios.clear();
        WriteSnapshot(sn);
    }
}

// `interactive` false = silent recovery after a crash or restart.
void TurnOff(const Snapshot& sn, bool interactive) {
    std::set<std::wstring> running = RunningPaths();
    std::set<std::wstring> skip;  // apps the user unticked: not reopened
    std::map<std::wstring, size_t> kOf;   // app path -> its position in the window's progress view
    if (interactive && g_settings.confirm) {
        std::vector<ChecklistItem> items;
        std::vector<std::wstring> itemPaths;
        for (const auto& a : sn.apps) {
            if (!running.count(Lower(a.path))) {
                items.push_back({a.name, L"", false, a.path, a.aumid});
                itemPaths.push_back(Lower(a.path));
            }
        }
        ChecklistResult choice = flow::Review(OffChecklist(items, !sn.plan.empty()));
        if (!choice.ok) {
            Wh_Log(L"OFF cancelled");
            return;
        }
        size_t nextK = 0;
        for (size_t i = 0; i < itemPaths.size(); i++) {
            if (!choice.checked[i]) {
                skip.insert(itemPaths[i]);
            } else {
                kOf[itemPaths[i]] = nextK++;
            }
        }
    }

    size_t toReopen = 0;
    {
        std::set<std::wstring> already = RunningPaths();
        for (const auto& a : sn.apps) {
            if (!already.count(Lower(a.path)) && !skip.count(Lower(a.path))) {
                toReopen++;
            }
        }
    }
    if (interactive && g_settings.confirm && !g_settings.showProgress) {
        flow::Close();
    } else if (interactive && !g_settings.confirm && g_settings.showProgress && toReopen) {
        // No checklist: the window opens straight in its progress view.
        std::vector<ChecklistItem> items;
        std::set<std::wstring> already = RunningPaths();
        size_t nextK = 0;
        for (const auto& a : sn.apps) {
            if (!already.count(Lower(a.path)) && !skip.count(Lower(a.path))) {
                items.push_back({a.name, L"", false, a.path, a.aumid});
                kOf[Lower(a.path)] = nextK++;
            }
        }
        flow::Direct(OffChecklist(items, !sn.plan.empty()));
    }
    RestorePriorities(sn);
    bool failed = !RestorePlan(sn);

    // Apps first started again by Windows or by the user are left alone.
    running = RunningPaths();
    int reopened = 0;
    std::vector<std::wstring> couldNot;
    Snapshot rest = sn;
    rest.apps.clear();
    rest.prios.clear();
    if (g_settings.reopenBackground) {
        reopen::Begin();   // the hook is in place before the first app is started
    }
    for (const auto& a : sn.apps) {
        if (running.count(Lower(a.path))) {
            Wh_Log(L"%s is already running", a.name.c_str());
            continue;
        }
        if (skip.count(Lower(a.path))) {
            Wh_Log(L"Not reopened (your choice): %s", a.name.c_str());
            continue;
        }
        if (g_settings.reopenBackground) {
            reopen::Expect(a.path, a.aumid);
        }
        const bool opened = Relaunch(a);
        if (opened) {
            Wh_Log(L"Reopened %s", a.name.c_str());
            reopened++;
        } else {
            Wh_Log(L"Could not reopen %s", a.name.c_str());
            rest.apps.push_back(a);
            couldNot.push_back(a.name);
            failed = true;
        }
        auto kit = kOf.find(Lower(a.path));
        if (kit != kOf.end()) {
            flow::SetState(kit->second, opened ? 1 : 2);
        }
        flow::Sleep_(150);  // keeps the window animating; apps start one by one
    }

    if (failed) {
        // Keep only what is left, so the next press retries just that.
        if (sn.plan.empty() || ActivePlan() == sn.plan) {
            rest.plan.clear();
        }
        WriteSnapshot(rest);
        std::wstring names;
        for (size_t i = 0; i < couldNot.size() && i < 3; i++) {
            names += (i ? L", " : L"") + couldNot[i];
        }
        ShowResult(L"Game Mode is off (with problems)",
                     L"Reopened " + std::to_wstring(reopened) + L" app(s)." +
                         (couldNot.empty() ? L" The power plan could not be restored."
                                           : L" Could not reopen: " + names + L".") +
                         L" Press the hotkey to retry.");
        return;
    }
    DeleteSnapshot();
    std::wstring msg = L"Off: reopened " + std::to_wstring(reopened) + L" app(s)";
    msg += sn.plan.empty() ? L"." : L", power plan restored.";
    Wh_Log(L"%s", msg.c_str());
    ShowResult(L"Game Mode is off",
                 L"Reopened " + std::to_wstring(reopened) + L" app(s)." +
                     (sn.plan.empty() ? L"" : L" Power plan restored."));
}

void Toggle() {
    if (g_busy) {
        return;
    }
    g_busy = true;
    flow::Close();  // a summary from the last press
    Snapshot sn;
    switch (LoadSnapshot(&sn)) {
        case LoadResult::None:
            TurnOn();
            break;
        case LoadResult::Ok:
            if (g_recoverTimer) {  // the user acted before recovery
                KillTimer(nullptr, g_recoverTimer);
                g_recoverTimer = 0;
            }
            TurnOff(sn, true);
            break;
        case LoadResult::Corrupt:
            Wh_Log(L"The snapshot is unreadable; discarding it");
            DeleteSnapshot();
            Notify(L"The saved snapshot was unreadable and was discarded. "
                   L"Nothing was restored.");
            break;
    }
    g_busy = false;
}

// After a restart or crash: restore quietly once Windows startup apps are up.
void Recover() {
    if (g_busy) {
        return;
    }
    g_busy = true;
    Snapshot sn;
    LoadResult r = LoadSnapshot(&sn);
    if (r == LoadResult::Ok) {
        Wh_Log(L"Recovering the snapshot left over from before the restart");
        TurnOff(sn, false);
    } else if (r == LoadResult::Corrupt) {
        DeleteSnapshot();
        Notify(L"A leftover Game Mode snapshot was unreadable and was "
               L"discarded.");
    }
    g_busy = false;
}

// True when a snapshot exists that was written before the last restart.
bool SnapshotIsLeftOver() {
    Snapshot sn;
    switch (LoadSnapshot(&sn)) {
        case LoadResult::Corrupt:
            return true;  // Recover() discards it
        case LoadResult::None:
            return false;
        case LoadResult::Ok:
            break;
    }
    if (!sn.logon.empty()) {
        // Written during another sign-in: the restart or sign-out closed the apps.
        return sn.logon != CurrentLogonId();
    }
    unsigned long long boot =
        NowFileTime() - (unsigned long long)GetTickCount64() * 10000ULL;
    return sn.startedAt + 50000000ULL < boot;  // began over 5 s before boot
}

// ---- hotkey thread ---------------------------------------------------------

void RegisterHotkey() {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, kHotkeyId);
        g_hotkeyRegistered = false;
    }
    if (!g_settings.hotkeyValid) {
        Notify(L"The hotkey in the settings is not valid, so Game Mode has no "
               L"hotkey.");
        return;
    }
    if (RegisterHotKey(nullptr, kHotkeyId, g_settings.hotMods | MOD_NOREPEAT,
                       g_settings.hotVk)) {
        g_hotkeyRegistered = true;
    } else {
        Wh_Log(L"RegisterHotKey failed (%lu): the hotkey is probably used by "
               L"another program",
               GetLastError());
        Notify(L"The hotkey is already used by another program. Pick another "
               L"one in the mod settings.");
    }
}

DWORD WINAPI WorkerThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // make the queue
    SetEvent(g_threadReady);

    LoadSettings();
    RegisterHotkey();
    if (SnapshotIsLeftOver()) {
        // A thread timer gets a system-chosen id; keep the one it returns.
        g_recoverTimer = SetTimer(nullptr, 0, kRecoverDelayMs, nullptr);
    }

    // Handles one message of the worker thread; false means "stop".
    auto handle = [&](MSG& m) -> bool {
        if (m.message == WM_QUIT || m.message == WM_APP_STOP) {
            return false;
        }
        if (m.message == WM_HOTKEY && m.wParam == (WPARAM)kHotkeyId) {
            Toggle();
            MSG drop;
            while (PeekMessageW(&drop, nullptr, WM_HOTKEY, WM_HOTKEY, PM_REMOVE)) {
            }  // presses that piled up while a dialog was open
        } else if (m.message == WM_TIMER && !m.hwnd && g_recoverTimer && m.wParam == g_recoverTimer) {
            KillTimer(nullptr, g_recoverTimer);
            g_recoverTimer = 0;
            Recover();
        } else if (m.message == WM_APP_RELOAD) {
            LoadSettings();
            RegisterHotkey();
        } else {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
        return true;
    };
    bool running = true;
    while (running) {
        if (GetMessageW(&msg, nullptr, 0, 0) <= 0) {
            break;
        }
        running = handle(msg);
    }
    if (g_recoverTimer) {
        KillTimer(nullptr, g_recoverTimer);
        g_recoverTimer = 0;
    }
    if (g_hotkeyRegistered) {
        UnregisterHotKey(nullptr, kHotkeyId);
    }
    flow::Shutdown();
    reopen::Stop();
    RestoreOnUnload();  // power plan and priorities, if Game Mode is still on
    if (g_notifyWnd) {
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd = g_notifyWnd;
        nid.uID = 1;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        DestroyWindow(g_notifyWnd);
    }
    GdiplusOff();
    return 0;
}

}  // namespace

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

BOOL WhTool_ModInit() {
    WCHAR windir[MAX_PATH];
    UINT n = GetWindowsDirectoryW(windir, ARRAYSIZE(windir));
    g_windowsDir = n && n < ARRAYSIZE(windir) ? windir : L"C:\\Windows";

    g_threadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }
    WaitForSingleObject(g_threadReady, 5000);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    PostThreadMessageW(g_threadId, WM_APP_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    PostThreadMessageW(g_threadId, WM_APP_STOP, 0, 0);
    WaitForSingleObject(g_thread, 3000);
    CloseHandle(g_thread);
    CloseHandle(g_threadReady);
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
