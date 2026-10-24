// ==WindhawkMod==
// @id              window-rules
// @name            Window Rules
// @description     Apply a rule to an app's windows: force position and size, move to a monitor, set opacity, keep on top. Matches executable, class and title.
// @version         1.0
// @author          metanoid18
// @github          https://github.com/Metanoid18
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -luser32 -ldwmapi
// ==/WindhawkMod==

// Source code is published under the MIT license.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Window Rules

Tell Windows what to do with an app's windows, once, and stop fixing them by
hand. Each rule matches a window (by executable, window class and title) and
asks for one or more of:

* a position and size, in pixels
* a monitor to open on
* an opacity level
* always on top

Rules are tried in order. The first enabled rule that matches a window is the
only one applied to it -- later rules are not consulted for that window, and
actions are never merged across rules. That is the same model KWin rules and
WindowManager use, and it is the one that stays predictable when two of your
rules would both apply.

## What it does

Open the app and the rule happens: the window is moved, sized and made
translucent where the rule says so. Move the window yourself and the mod lets go
-- your position wins for the rest of the session, and the rule is not re-applied
to that window until the app is started again. Saving the settings re-applies
every rule to the windows that are open at that moment, so editing a rule shows
you its effect immediately instead of at the next restart.

Nothing else is touched: no focus, no hiding, and no continuous watch over
windows that are already in place.

## Always on top

A rule can keep an app's windows above other windows. The window goes up when it
opens and stays up: activating another window does not take the top away, because
Windows keeps a window's topmost bit until something actually removes it.

What it deliberately does not do is fight for the top:

* **Another always-on-top window can sit above yours.** The taskbar and other
  pinned apps are the user's own choices, and the mod does not climb back over
  them.
* **A fullscreen app on the same monitor makes it step aside.** A window pinned by
  this mod is lowered while a fullscreen game or video covers its monitor, and
  raised again when that app is gone. A maximized window is not fullscreen and
  never triggers this.
* **If the app clears the bit itself, the mod leaves it cleared**, and the journal
  says `app-cleared`, rather than pinning it again on the next activation.
* **Dragging the window stops the mod touching it**, the z-order included, for the
  rest of the session.
* **Turning the rule off takes the window back down.** The mod only ever lowers a
  bit it set itself, so an app that pins its own windows is never affected.
* **The same goes for opacity.** The mod records the windows it made translucent,
  so changing the level takes effect on windows that are already open, and
  clearing the level, deleting the rule or turning the mod off puts them back to
  fully opaque. Only windows the mod faded itself are touched: an app that draws
  its own transparency is never made opaque by this.

One consequence worth knowing before you pin something: a topmost window stays
above whatever you switch to with Alt+Tab, so if the pinned window covers most of
the screen, switching away looks like nothing happened -- the window you wanted is
behind it. That is how the topmost bit works, not something this mod adds. Pin
small windows, or leave the rule on "No change" while you are switching around.

## Where windows go, and in what units

**Positions and sizes are in physical pixels, not points.** A rule that says
`800x600` gets a window 800 physical pixels wide whatever the display scale is,
so on a 150% display it will look smaller than the same rule on a 100% one. That
is not adjustable in this version, and it is the same truth the whole mod works
in: the mod reads and writes physical pixels, and reports them in the journal so
that what it did can be checked by hand.

**A position is measured from the top-left of the target monitor, not from the
desktop.** `position: 100,200` means 100 pixels in and 200 down from the corner
of the monitor the rule names -- or of the monitor the window is already on, when
the rule names none. So a rule survives docking, undocking and adding a monitor
above the primary, which the same numbers in desktop coordinates would not. X and
Y may be negative to place a window partly off the edge.

## How matching works

A rule has three optional match fields. Every field you fill in must match; a
field you leave blank matches anything. The fields are:

* **Executable** -- a file name (`notepad` or `notepad.exe`) or a full path.
* **Window class** -- the window's class name, for example `Notepad` or
  `Chrome_WidgetWin_1`.
* **Window title** -- searched *inside* the title, so a fragment is enough.

All matching is case-insensitive. `*` and `?` are wildcards, so `chrome*` matches
`chrome.exe` and `Untitled*` matches any title starting that way. To use a full
regular expression, wrap it in slashes: `/^Document - \w+\.txt$/`. Nothing else
is treated as a pattern, so a path like `C:\Program Files (x86)\Steam\steam.exe`
or a title like `Inbox | Outlook` matches literally.

## What it does not do, and where the limits are

* Hide a window from the taskbar or Alt+Tab. Toggling the extended window style is
  one-shot and applications reset it themselves; a version that cannot re-assert
  would be a broken promise, so it is not here yet.
* Move windows between virtual desktops: that needs an undocumented Windows
  interface, and this mod does not use one.
* **Override an app that insists.** A rule is applied when the window appears and
  re-applied once if the app immediately moves it somewhere else. An app that
  moves its window again after that wins -- the mod stops rather than fighting it.
* **A rule waits for a minimized window to be restored.** A minimized window has
  no meaningful rectangle to move, so nothing is applied while it is minimized;
  the rule is applied the moment you restore it, including for an app that starts
  in the tray.
* **A maximized window stays maximized.** The mod does not restore it to normal
  first, and what Windows does with a maximized window's restore rectangle is
  Windows' business rather than the mod's, so a rule that sets a position may not
  show until you restore the window yourself.
* **Opacity on a window that is already translucent by itself.** Those windows
  draw their own transparency; a flat opacity level would replace it with
  something that renders wrong, so the mod reports `already-layered` in the journal
  and leaves them alone.
* **Change a window's size when the app is in charge of it.** Some apps re-create
  their window when resized; they are left as they are after the single retry.
* **The invisible border.** Since Windows 10 a window's rectangle includes about
  seven pixels of invisible resize border on the left, right and bottom, so
  `position: 0,0` leaves a small visible gap at the left, and an `800x600` window
  looks about 14 pixels narrower and 7 shorter. The mod does exactly what the
  numbers say in the coordinates Windows reports, and the journal prints them.
* **File Explorer windows are not matched.** `explorer.exe` is on the built-in
  exclude list, because the same process owns the taskbar, the desktop and the
  shell's own windows. Explorer windows cannot be targeted by a rule in this
  version.
* **A window on another virtual desktop is left alone until you save the
  settings while it is in front of you.** Those windows are cloaked, and the mod
  ignores cloaked windows on purpose. So if you delete a rule while one of its
  windows sits on another desktop, that window keeps its opacity and its always on
  top until the next settings save with it visible, or until you turn the mod off.
  Nothing is left behind after that.
* **Always on top and fullscreen is detected geometrically.** A window counts as
  fullscreen when it covers a monitor and is not maximized. A window that nearly
  covers a monitor is not detected, so a pinned window can stay above it.
* **Stepping aside for a fullscreen app is tested, not observed, across
  monitors.** It was built and tried on a single-monitor machine, so "a game on
  the other monitor leaves this window pinned" has not been watched happening.

## Not the same as other mods

* **Always On Top Windows** pins and unpins the active window with a hotkey, by
  hand. This mod keeps an app's windows on top automatically, every time they
  open.
* **Prevent Focus Stealing** and **Snap Commander** already cover their own
  actions (focus, and snapping to screen regions) and this mod deliberately does
  not re-implement them.
* **Where I Left It** remembers where you left a window and puts it back. This
  mod puts a window where *you* say, every time. Do not list the same app in
  both mods' settings at once: both would place the window, in an order neither
  can control. The same goes for **Snap Commander**'s auto-snap rules.

## Diagnostics

**Write a diagnostics journal** appends to `window-rules-journal.txt` in a folder
of your own: `<Windhawk mod storage>\<your SID>`, created with an access control
list that gives full access to you, to SYSTEM and to administrators, and to nobody
else. Journal lines carry executable names and window titles, and the shared mod
storage folder is readable by every account on the machine, so the file is kept
where only you can read it. If that folder cannot be created the journal is simply
not written; the mod itself is unaffected.

Neither the file nor the folder exists until you turn the journal on. It is capped
at 1 MB (past that it starts again from empty, with a line saying so), and it is
**off by default**; turn it on when a rule does something you did not expect, or
when reporting a bug, and the file will say what the mod decided and why. Every
line is prefixed with what it is doing:

* `RULES-LOADED` and `RULE-LOADED` -- the rules as the mod understood them, with
  any problem it found in a rule, so a typo is visible instead of silent.
* `RULE-HIT` -- which rule matched which window, and which field decided it.
* `RULE-NONE` -- a window that no rule matched (a few per app, not every one).
* `RULE-FILTER-SKIP` -- a window that was never matched at all, and why.
* `APPLY` -- what was done to a matched window, with the rectangle it was at and
  the rectangle the rule asked for, and why nothing happened when nothing did
  (`already-there`, `overridden`, `minimized`, `refused`).
* `SETTLE` -- whether the window stayed where the rule put it, was put back once,
  or was left alone because the app kept moving it.
* `OVERRIDE` -- you moved a window yourself; the mod has let go of it.
* `OPACITY` -- what happened to the level, and why: `applied`, `already-there`,
  `already-layered` (the app draws its own transparency, left alone), `failed`
  (the window refused the change), `app-cleared` (the app removed the style
  itself), `withdrawn` (the rule stopped asking, or the mod was turned off).
* `MONITOR-SKIP` -- a rule names a monitor that is not there.
* `TOPMOST` -- what happened to the z-order, and why: `applied`, `already-there`,
  `overridden`, `minimized`, `refused`, `yielded` (a fullscreen app took the
  monitor), `reclaimed` (it gave the monitor back), `app-cleared` (the app
  removed the bit itself), `withdrawn` (the rule stopped asking), or
  `not-claimed` (the window is on top, but this mod did not put it there).
* `REAPPLY-PASS` -- the sweep after a settings save, and how many windows it moved.

## Settings

Rules are read from the top down, and **an entry with every field blank ends the
list** -- the settings template ships one blank entry, so a fresh install has no
rules at all until you fill it in. Leave the blank entry last.

A rule with no match fields is not loaded, because it would apply to every window
on the desktop; the journal says `RULE-PROBLEM` and names the rule. To mean every
window on purpose, put a `*` in one of the match fields.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- diagnosticsLog: false
  $name: Write a diagnostics journal
  $description: >-
    Appends a line to window-rules-journal.txt for every decision: which rule
    matched which window, which field decided it, which windows were rejected
    before matching, and what happened to the z-order. The file lives in a folder
    only your Windows account can read, because its lines contain window titles;
    neither the file nor the folder is created until this is turned on. Off by
    default; turn it on when a rule does something you did not expect, or when
    reporting a bug. The file is capped at 1 MB.
- rules:
  - - name: ""
      $name: Rule name
      $description: >-
        A label for this rule. It is only used in the journal, so that you can
        tell your rules apart.
    - enabled: true
      $name: Rule enabled
      $description: >-
        Turn this rule off without deleting it.
    - executable: ""
      $name: Executable
      $description: >-
        A file name (notepad or notepad.exe) or a full path to the executable.
        Case-insensitive. Blank matches any executable. * and ? are wildcards,
        so chrome* matches chrome.exe. A value containing a backslash, a slash
        or a colon is matched against the full path instead, and wildcards work
        there too. Wrap the value in slashes to use a regular expression:
        /^(chrome|msedge)/ . Anything not in slashes is matched literally, so a
        path with brackets in it is fine. A regular expression in slashes is
        matched against the file name, not the full path.
    - windowClass: ""
      $name: Window class
      $description: >-
        The window's class name, for example Notepad or Chrome_WidgetWin_1.
        Case-insensitive, matched against the whole name, with the same wildcard
        and slash-delimited regular expression rules as Executable. Blank matches
        any class.
    - title: ""
      $name: Window title
      $description: >-
        Searched inside the window title, so a fragment is enough.
        Case-insensitive, with the same wildcard and slash-delimited regular
        expression rules as Executable; inside slashes, ^ and $ anchor the match.
        Blank matches any title.
    - position: ""
      $name: Position
      $description: >-
        Where to put the window, as X,Y in PHYSICAL PIXELS -- not points -- for
        example 100,200. Measured from the top-left corner of the target monitor,
        not from the desktop, so the same numbers keep working when monitors are
        added, removed or rearranged. X and Y may be negative to place the window
        partly off the edge. Leave blank to leave the position alone.
    - size: ""
      $name: Size
      $description: >-
        How big to make the window, as WxH in PHYSICAL PIXELS -- not points --
        for example 800x600. On a display scaled above 100% this looks smaller
        than the same numbers would in a settings dialog that uses points. Leave
        blank to leave the size alone.
    - monitor: ""
      $name: Monitor
      $description: >-
        Which monitor to put the window on: primary, a monitor number counted from
        the left (1 is the leftmost, 2 the next to its right), a display name like
        DISPLAY2, or part of a monitor's device path. A monitor number that does
        not exist, or a name that matches nothing, is reported in the journal and
        the rule moves nothing rather than guessing. Leave blank to use the monitor
        the window is already on.
    - opacity: ""
      $name: Opacity
      $description: >-
        How opaque the window should be, from 1 to 255, where 255 is fully opaque
        (230 is a noticeable but usable translucency). Windows that already draw
        their own transparency are left alone, and the journal says so. Leave
        blank to leave the opacity alone.
    - topmost: leave
      $name: Always on top
      $description: >-
        Keep this app's windows above other windows. A pinned window is lowered
        while a fullscreen app covers the same monitor, and raised again after it
        closes; another always-on-top window may still sit above it. "No change"
        takes back a pin this mod set, so a window it pinned earlier comes back
        down; it never touches the z-order of a window the mod did not pin.
      $options:
      - leave: No change
      - "on": Always on top
  $name: Rules
  $description: >-
    One entry per rule. The first enabled rule that matches a window is applied
    to it; later rules are not consulted for that window. The default template
    entry is intentionally empty.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <sddl.h>
#include <windhawk_api.h>

// swprintf_s and wcscmp below are CRT functions; windows.h does not declare
// them, so the include is required rather than incidental.
#include <cwchar>

#include <algorithm>
#include <atomic>
#include <ctime>
#include <map>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Design overview, for the reviewer.
//
// What this is: a tool mod. It runs in windhawk.exe, hooks window events, and
// changes the z-order and geometry of other applications' windows when a rule
// matches them. It injects nothing, reads no memory of another process, and
// writes nothing to disk except its own journal.
//
// * One event thread with its own message loop, and four out-of-context
//   SetWinEventHook registrations on it:
//     - EVENT_OBJECT_CREATE..EVENT_OBJECT_SHOW, for a window appearing;
//     - EVENT_SYSTEM_MOVESIZEEND, for the user placing a window by hand;
//     - EVENT_SYSTEM_MINIMIZEEND, for a window whose rule was deferred while it
//       was minimized and which is now back;
//     - EVENT_SYSTEM_FOREGROUND, alone, for a fullscreen app taking a monitor.
//   EVENT_OBJECT_LOCATIONCHANGE is deliberately not registered: it is the
//   highest-volume event in the shell and a rules engine learns nothing from a
//   window being dragged.
// * One SetWindowPos call site. It takes the rectangle (or none) and the
//   z-order argument (or none), which is how a move, a pin, a yield and a
//   withdrawal are all issued. Always SWP_ASYNCWINDOWPOS and always
//   SWP_NOACTIVATE: a rule must never steal focus or deadlock against a window
//   owned by another thread.
// * Two SetLayeredWindowAttributes and two SetWindowLongPtrW, both for opacity:
//   one pair sets the level on a window the app has not layered itself, the other
//   takes it back. A window the app layered itself is never touched.
// * No timer except the settle poll, which is armed only while a move is
//   settling and killed when the last one is. The mod is idle otherwise.
// * No polling, no sleeping, no PeekMessage loop, no persistence. Rules are
//   user-authored settings and live in the settings block; the mod never writes
//   settings back.
// * A rules engine that re-applied on every event would fight the user and the
//   application. So: apply once on appearance, one retry if the app moves the
//   window, then stop. A hand move releases the window for the session. The
//   journal records which of those happened, because "nothing happened" is the
//   answer that needs evidence most.
// * The journal is the validation surface: one line appended at the instant of
//   each decision, nothing debounced or overwritten, so a script can assert
//   against it without racing the mod.
// * `tools/check_rules_boundaries.sh` in the mod's repository asserts the
//   structure above mechanically, including that a claim on the z-order is
//   released before any early return in the apply path. That last one was a
//   real defect found in the field, and it is silent when it recurs.

// --- Constants --------------------------------------------------------------

static constexpr PCWSTR kJournalFileName = L"window-rules-journal.txt";

// A top-level window, not message-only, because WM_DISPLAYCHANGE is a broadcast
// and message-only windows do not receive broadcasts.
static constexpr PCWSTR kMessageWindowClassName =
    L"WindhawkWindowRulesMessageWnd";

// Posted from Windhawk's threads, handled on the event thread so that all mod
// state is owned by one thread and needs no locking.
static constexpr UINT kMsgReloadSettings = WM_APP + 1;
static constexpr UINT kMsgShutdown = WM_APP + 2;

// Processes whose windows are never matched, checked before anything else.
//
// The shell and the DWM own their own windows, and placing or styling one fights
// the shell's own layout decisions.
// windhawk.exe is excluded so the mod never matches Windhawk's own windows.
static constexpr PCWSTR kExcludedProcesses[] = {
    L"windhawk.exe",
    L"explorer.exe",
    L"dwm.exe",
    L"ShellExperienceHost.exe",
};

// A window that matches nothing is worth reporting, but only a few times per
// app: the journal is read by people, and every window on the desktop would
// otherwise produce a line.
static constexpr int kMaxNoMatchLinesPerApp = 5;
static constexpr int kMaxFilterSkipLinesPerReason = 10;

// --- Pure rule core ---------------------------------------------------------
//
// Everything between the markers is pure: no Win32, no mod state, no I/O. The
// unit tests extract exactly this region from this file and compile it, so the
// tests exercise the code that ships rather than a copy of it.

// >>> RULES_CORE >>>

// A rule is three optional match fields plus the actions it asks for. All
// populated match fields must match; a blank field matches anything.
struct RuleSpec {
    bool enabled = true;
    std::wstring name;
    std::wstring executable;
    std::wstring windowClass;
    std::wstring title;
};

struct RuleActions {
    std::wstring position;  // "X,Y"
    std::wstring size;      // "WxH" or "W,H"
    std::wstring monitor;   // "primary", an index, a device name or a device path
    std::wstring opacity;   // 1-255
    std::wstring topmost;   // "leave" or "on"
};

// How a match field is interpreted. The order of the checks matters and is
// documented in the settings text, so that nobody has to read this to know what
// to type: a value wrapped in slashes is a regular expression, one containing *
// or ? is a wildcard pattern, and everything else is a plain literal.
enum class PatternKind {
    Blank,  // matches anything
    Plain,  // literal text
    Glob,   // literal text with * and ?
    Regex,  // a regular expression
};

static bool ContainsGlobMeta(const std::wstring& value) {
    return value.find(L'*') != std::wstring::npos ||
           value.find(L'?') != std::wstring::npos;
}

// A regular expression is opt-in and written between slashes: /^Untitled.*/
//
// Auto-detection was tried first, and it failed silently. Any of a handful of
// characters turned a field into a regex, so "C:\\Program Files (x86)\\Steam\\steam.exe"
// became a regex in which \\s is a whitespace class and the rule never matched
// anything, "Inbox | Outlook" matched the wrong windows, and "Untitled (1)"
// became a group. Requiring the slashes costs a little typing and makes every
// other value mean exactly what it looks like.
static bool IsSlashDelimitedRegex(const std::wstring& value) {
    return value.size() >= 2 && value.front() == L'/' && value.back() == L'/';
}

static std::wstring InnerRegexText(const std::wstring& value) {
    if (!IsSlashDelimitedRegex(value)) {
        return value;
    }
    return value.substr(1, value.size() - 2);
}

static PatternKind ClassifyPattern(const std::wstring& value) {
    if (value.empty()) {
        return PatternKind::Blank;
    }
    if (IsSlashDelimitedRegex(value)) {
        return PatternKind::Regex;
    }
    if (ContainsGlobMeta(value)) {
        return PatternKind::Glob;
    }
    return PatternKind::Plain;
}

static std::wstring ToLowerCopy(const std::wstring& value) {
    std::wstring lowered = value;
    for (wchar_t& c : lowered) {
        if (c >= L'A' && c <= L'Z') {
            c = (wchar_t)(c - L'A' + L'a');
        }
    }
    return lowered;
}

// "notepad.exe" and "notepad" are the same executable to a human, and the
// settings text promises both work, so a trailing .exe is dropped before
// comparing. Nothing else is touched: this is a stem comparison, not a fuzzy
// match, so "notepad" does not match "notepad2.exe".
static std::wstring StripExeSuffix(const std::wstring& value) {
    static const std::wstring kSuffix = L".exe";
    if (value.size() > kSuffix.size() &&
        value.compare(value.size() - kSuffix.size(), kSuffix.size(),
                      kSuffix) == 0) {
        return value.substr(0, value.size() - kSuffix.size());
    }
    return value;
}

// A match field that names a path rather than a file name is compared against
// the full path, so the user's entry works either way.
static bool LooksLikePath(const std::wstring& value) {
    return value.find(L'\\') != std::wstring::npos ||
           value.find(L'/') != std::wstring::npos ||
           value.find(L':') != std::wstring::npos;
}

// Wildcard match over a whole value: * matches any run of characters (including
// none), ? matches exactly one. Iterative with backtracking on the last *, so a
// pathological pattern cannot blow up the way a recursive matcher can.
static bool GlobMatchesWhole(const std::wstring& pattern,
                             const std::wstring& value) {
    size_t p = 0;
    size_t v = 0;
    size_t starPattern = std::wstring::npos;
    size_t starValue = 0;

    while (v < value.size()) {
        if (p < pattern.size() && (pattern[p] == L'?' || pattern[p] == value[v])) {
            p++;
            v++;
        } else if (p < pattern.size() && pattern[p] == L'*') {
            starPattern = p++;
            starValue = v;
        } else if (starPattern != std::wstring::npos) {
            p = starPattern + 1;
            v = ++starValue;
        } else {
            return false;
        }
    }

    while (p < pattern.size() && pattern[p] == L'*') {
        p++;
    }
    return p == pattern.size();
}

// A compiled match field. Compilation happens once per rule per settings load,
// never per window: matching runs on every window event, and compiling a
// regular expression there would be the mod's only real cost.
struct CompiledPattern {
    PatternKind kind = PatternKind::Blank;
    bool usable = true;        // false when a regular expression did not compile
    std::wstring problem;      // human-readable reason when !usable
    std::wstring literal;      // lowered literal, for Plain and Glob
    std::wregex regex;         // used when kind is Regex
};

static CompiledPattern CompilePattern(const std::wstring& value) {
    CompiledPattern compiled;
    compiled.kind = ClassifyPattern(value);

    if (compiled.kind == PatternKind::Blank) {
        return compiled;
    }

    compiled.literal = ToLowerCopy(value);

    if (compiled.kind == PatternKind::Regex) {
        const std::wstring inner = InnerRegexText(value);
        if (inner.empty()) {
            // "//" compiles to an empty pattern, which matches every string. That
            // is the same "applies to everything" failure the blank match fields
            // were rejected for, wearing a regular expression's clothes.
            compiled.usable = false;
            compiled.problem = L"a regular expression between slashes is empty";
            return compiled;
        }
        try {
            compiled.regex =
                std::wregex(inner, std::regex_constants::icase);
        } catch (const std::regex_error&) {
            // A pattern that does not compile is reported once at load time and
            // then never matches. Silently ignoring it would leave the user
            // staring at a rule that does nothing for no visible reason.
            compiled.usable = false;
            compiled.problem = L"not a valid regular expression";
        }
    }

    return compiled;
}

static bool RegexMatches(const CompiledPattern& pattern,
                         const std::wstring& value) {
    try {
        return std::regex_search(value, pattern.regex);
    } catch (const std::regex_error&) {
        return false;
    }
}

// Match one field against one piece of text.
//
// `contains` is the difference between the two kinds of field. Executable and
// window class are matched against the whole value (so "chrome" does not
// silently match "chrome_remote_desktop_host.exe"), while the title is searched
// (a fragment is enough, because demanding the whole title from a user would be
// absurd -- titles carry document names and version numbers).
static bool PatternMatchesValue(const CompiledPattern& pattern,
                                const std::wstring& value, bool contains) {
    if (!pattern.usable) {
        return false;
    }

    switch (pattern.kind) {
        case PatternKind::Blank:
            return true;

        case PatternKind::Plain: {
            std::wstring lowered = ToLowerCopy(value);
            if (!contains) {
                return lowered == pattern.literal;
            }
            return lowered.find(pattern.literal) != std::wstring::npos;
        }

        case PatternKind::Glob: {
            std::wstring lowered = ToLowerCopy(value);
            if (contains) {
                // Searching with wildcards: the pattern has to appear somewhere
                // in the title, with its own * and ? still meaningful. Wrapping
                // it in * on both sides is what turns whole-value glob matching
                // into a search, and it keeps ^/$-free patterns behaving
                // exactly as the settings text says.
                return GlobMatchesWhole(L"*" + pattern.literal + L"*", lowered);
            }
            return GlobMatchesWhole(pattern.literal, lowered);
        }

        case PatternKind::Regex:
            // Unanchored on purpose: the settings text promises that wrap-^-
            // and-$ is how you anchor, which only means something if the
            // default has no anchor of its own.
            return RegexMatches(pattern, value);
    }

    return false;
}

// The executable field accepts a file name, a file name with .exe, a wildcard
// pattern, or a full path -- and a user who types "note?ad" means the executable
// whether or not they also typed the suffix. So the name is tried both as it is
// and with a trailing .exe removed, and a value that looks like a path is tried
// against the full path as well. Trying both is what makes the common entries
// work; it is not a fuzzy match, since every candidate comparison is exact for
// its kind.
static bool PatternMatchesExecutable(const CompiledPattern& pattern,
                                     const std::wstring& exePath,
                                     const std::wstring& exeName) {
    if (!pattern.usable) {
        return false;
    }
    if (pattern.kind == PatternKind::Blank) {
        return true;
    }

    const std::wstring loweredPath = ToLowerCopy(exePath);
    const std::wstring loweredName = ToLowerCopy(exeName);
    const std::wstring stemOriginal = StripExeSuffix(exeName);
    const std::wstring stem = StripExeSuffix(loweredName);

    const auto matchesText = [&pattern](const std::wstring& loweredValue,
                                        const std::wstring& originalValue) {
        switch (pattern.kind) {
            case PatternKind::Plain:
                return loweredValue == pattern.literal;
            case PatternKind::Glob:
                return GlobMatchesWhole(pattern.literal, loweredValue);
            case PatternKind::Regex:
                return RegexMatches(pattern, originalValue);
            case PatternKind::Blank:
                break;
        }
        return false;
    };

    // A value that looks like a path is about the path and nothing else, so a
    // path entry cannot silently start matching same-named executables in other
    // folders. Not for a regex: its delimiters are slashes, so every regex would
    // look like a path and be compared against the full path instead of the name.
    if (pattern.kind != PatternKind::Regex && LooksLikePath(pattern.literal)) {
        return matchesText(loweredPath, exePath);
    }

    // The stem is passed along with its own text, not the ".exe" name: a
    // regex asking for the end of the name ("pad$") means the end of the name
    // the user wrote, which is the one without the suffix they left out.
    return matchesText(loweredName, exeName) ||
           (stem != loweredName && matchesText(stem, stemOriginal));
}

// A rule with nothing to match on would match every window on the desktop, and a
// reapply pass would then move or fade everything open. It is not a useful thing
// to configure by accident, so it does not load at all. "Every window" is still
// sayable on purpose: an asterisk in any one field.
static bool RuleMatchesAnything(const RuleSpec& rule) {
    return rule.executable.empty() && rule.windowClass.empty() &&
           rule.title.empty();
}

struct CompiledRule {
    bool usable = true;
    std::wstring problem;  // first problem found; empty when usable
    CompiledPattern executable;
    CompiledPattern windowClass;
    CompiledPattern title;
};

static CompiledRule CompileRule(const RuleSpec& rule) {
    CompiledRule compiled;
    compiled.executable = CompilePattern(rule.executable);
    compiled.windowClass = CompilePattern(rule.windowClass);
    compiled.title = CompilePattern(rule.title);

    if (RuleMatchesAnything(rule)) {
        // Rejected rather than applied to everything. The journal line names the
        // rule, and the settings text says how to mean it on purpose.
        compiled.usable = false;
        compiled.problem =
            L"no match fields: fill in Executable, Window class or Window title, "
            L"or type * to mean every window";
        return compiled;
    }

    if (!compiled.executable.usable) {
        compiled.usable = false;
        compiled.problem = L"Executable: " + compiled.executable.problem;
    } else if (!compiled.windowClass.usable) {
        compiled.usable = false;
        compiled.problem = L"Window class: " + compiled.windowClass.problem;
    } else if (!compiled.title.usable) {
        compiled.usable = false;
        compiled.problem = L"Window title: " + compiled.title.problem;
    }

    return compiled;
}

// An entry in the rules array exists when any of its keys carried text. This is
// what ends the loop that reads rules, so it is in the pure core and unit-tested
// rather than buried in the reader: getting it wrong is not a small mistake, it
// is an endless loop on the mod's own event thread.
static bool RuleEntryIsPresent(const RuleSpec& rule, const RuleActions& actions) {
    // "leave" is the default the settings template writes for the dropdown, and
    // it means "no action", so it must not make an otherwise empty entry look
    // present. The shipped template ends the list with exactly such an entry: if
    // Windhawk ever reports that default instead of an empty string for it, the
    // reader below would never find its terminator and would spin on the event
    // thread, where nothing would ever load again.
    const bool topmostSet = !actions.topmost.empty() &&
                            actions.topmost != L"leave";
    return !rule.name.empty() || !rule.executable.empty() ||
           !rule.windowClass.empty() || !rule.title.empty() ||
           !actions.position.empty() || !actions.size.empty() ||
           !actions.monitor.empty() || !actions.opacity.empty() || topmostSet;
}

struct MatchOutcome {
    bool matched = false;
    int ruleIndex = -1;
    std::wstring field;  // which field decided it
    std::wstring value;  // the text that matched, for the journal
};

// Rule matching is by the window's own facts and nothing else: no window handle
// cache, no learned state, no per-session memory. Rules are configuration, so
// matching one is a pure function of the window in front of it -- which is also
// why the whole decision is reproducible from a journal line.
static MatchOutcome FindFirstMatchingRule(
    const std::vector<RuleSpec>& rules,
    const std::vector<CompiledRule>& compiledRules,
    const std::wstring& exePath,
    const std::wstring& exeName,
    const std::wstring& windowClass,
    const std::wstring& title) {
    MatchOutcome outcome;

    for (size_t i = 0; i < rules.size() && i < compiledRules.size(); i++) {
        const RuleSpec& rule = rules[i];
        const CompiledRule& compiled = compiledRules[i];

        if (!rule.enabled || !compiled.usable) {
            continue;
        }

        if (!PatternMatchesExecutable(compiled.executable, exePath, exeName)) {
            continue;
        }
        if (!PatternMatchesValue(compiled.windowClass, windowClass,
                                 /*contains=*/false)) {
            continue;
        }
        if (!PatternMatchesValue(compiled.title, title, /*contains=*/true)) {
            continue;
        }

        outcome.matched = true;
        outcome.ruleIndex = (int)i;
        // Report the most specific field that was actually used, so a journal
        // line explains why this rule won and not one of the others.
        if (compiled.title.kind != PatternKind::Blank) {
            outcome.field = L"title";
            outcome.value = title;
        } else if (compiled.windowClass.kind != PatternKind::Blank) {
            outcome.field = L"windowClass";
            outcome.value = windowClass;
        } else if (compiled.executable.kind != PatternKind::Blank) {
            outcome.field = L"executable";
            outcome.value = exeName;
        } else {
            outcome.field = L"any";
            outcome.value = exeName;
        }
        return outcome;
    }

    return outcome;
}

// --- Action schema, validated but not applied -------------------------------
//
// Checks that what the user typed can mean something, so that a mistyped rule is
// a journal line at load time rather than a window that mysteriously does not
// move. Nothing here touches a window; the parsed values are not even kept.

static bool ParseLongPair(const std::wstring& text, wchar_t separator,
                          long& first, long& second) {
    size_t split = text.find(separator);
    if (split == std::wstring::npos) {
        return false;
    }

    std::wstring firstText = text.substr(0, split);
    std::wstring secondText = text.substr(split + 1);

    // Trim spaces, so "100, 200" and "800 x 600" are accepted, but nothing else
    // is: a value with letters in it is a mistake, not a smaller number.
    const auto trim = [](std::wstring& value) {
        size_t begin = value.find_first_not_of(L" \t");
        size_t end = value.find_last_not_of(L" \t");
        value = (begin == std::wstring::npos) ? std::wstring()
                                              : value.substr(begin, end - begin + 1);
    };
    trim(firstText);
    trim(secondText);

    const auto toLong = [](const std::wstring& value, long& out) {
        if (value.empty()) {
            return false;
        }
        size_t index = 0;
        if (value[0] == L'-' || value[0] == L'+') {
            index = 1;
        }
        if (index >= value.size()) {
            return false;
        }
        long parsed = 0;
        for (; index < value.size(); index++) {
            if (value[index] < L'0' || value[index] > L'9') {
                return false;
            }
            parsed = parsed * 10 + (value[index] - L'0');
            if (parsed > 1000000) {
                return false;
            }
        }
        out = (value[0] == L'-') ? -parsed : parsed;
        return true;
    };

    return toLong(firstText, first) && toLong(secondText, second);
}

static bool ParsePositionValue(const std::wstring& text, long& x, long& y) {
    return ParseLongPair(text, L',', x, y) && x >= -100000 && x <= 100000 &&
           y >= -100000 && y <= 100000;
}

static bool ParseSizeValue(const std::wstring& text, long& width, long& height) {
    if (!ParseLongPair(text, L'x', width, height) &&
        !ParseLongPair(text, L'X', width, height) &&
        !ParseLongPair(text, L',', width, height)) {
        return false;
    }
    return width >= 1 && width <= 100000 && height >= 1 && height <= 100000;
}

static bool ParseOpacityValue(const std::wstring& text, int& alpha) {
    if (text.empty()) {
        return false;
    }
    int parsed = 0;
    for (wchar_t c : text) {
        if (c < L'0' || c > L'9') {
            return false;
        }
        parsed = parsed * 10 + (c - L'0');
        if (parsed > 1000) {
            return false;
        }
    }
    // Zero is rejected rather than clamped: a fully transparent window is
    // invisible but still there, which reads as a bug rather than a setting.
    if (parsed < 1 || parsed > 255) {
        return false;
    }
    alpha = parsed;
    return true;
}

static bool IsAllDigits(const std::wstring& text) {
    if (text.empty()) {
        return false;
    }
    for (wchar_t c : text) {
        if (c < L'0' || c > L'9') {
            return false;
        }
    }
    return true;
}

static bool ValidateMonitorValue(const std::wstring& text,
                                 std::wstring& problem) {
    if (text.empty()) {
        return true;  // not set
    }

    if (ToLowerCopy(text) == L"primary") {
        return true;
    }

    if (IsAllDigits(text)) {
        if (text.size() > 2) {
            problem = L"a display number must be between 1 and 32";
            return false;
        }
        long index = 0;
        for (wchar_t c : text) {
            index = index * 10 + (c - L'0');
        }
        if (index >= 1 && index <= 32) {
            return true;
        }
        problem = L"a display number must be between 1 and 32";
        return false;
    }

    // A device name (DISPLAY2), a device path, or a fragment of one. Anything
    // this short cannot identify a monitor, and a bare word that is not
    // "primary" is far more likely to be a typo than a monitor name.
    if (text.size() < 4) {
        problem = L"expected primary, a display number, or a device name";
        return false;
    }

    return true;
}

static bool ValidateTopmostValue(const std::wstring& text,
                                 std::wstring& problem) {
    // Blank is "not set": an entry saved before this setting existed, or one
    // whose key is absent, must not be reported as a mistake.
    if (text.empty() || text == L"leave" || text == L"on") {
        return true;
    }
    problem = L"expected \"No change\" or \"Always on top\"";
    return false;
}

static std::vector<std::wstring> DescribeActionProblems(
    const RuleActions& actions) {
    std::vector<std::wstring> problems;

    long x = 0;
    long y = 0;
    if (!actions.position.empty() && !ParsePositionValue(actions.position, x, y)) {
        problems.push_back(
            L"Position: expected X,Y in pixels, for example 100,200");
    }

    long width = 0;
    long height = 0;
    if (!actions.size.empty() && !ParseSizeValue(actions.size, width, height)) {
        problems.push_back(L"Size: expected WxH in pixels, for example 800x600");
    }

    std::wstring problem;
    if (!ValidateMonitorValue(actions.monitor, problem)) {
        problems.push_back(L"Monitor: " + problem);
    }

    int alpha = 0;
    if (!actions.opacity.empty() && !ParseOpacityValue(actions.opacity, alpha)) {
        problems.push_back(L"Opacity: expected a whole number from 1 to 255");
    }

    if (!ValidateTopmostValue(actions.topmost, problem)) {
        problems.push_back(L"Always on top: " + problem);
    }

    return problems;
}

// A compact description of what a rule asks for, for the journal. Only the
// actions the user actually set are listed, so "no actions set" is visible as
// an empty value rather than as five defaults.
static std::wstring DescribeRequestedActions(const RuleActions& actions) {
    std::wstring text;
    const auto add = [&text](const wchar_t* field, const std::wstring& value) {
        if (value.empty()) {
            return;
        }
        if (!text.empty()) {
            text += L";";
        }
        text += field;
        text += L":";
        text += value;
    };

    add(L"position", actions.position);
    add(L"size", actions.size);
    add(L"monitor", actions.monitor);
    add(L"opacity", actions.opacity);
    if (actions.topmost == L"on") {
        add(L"topmost", L"on");
    }

    return text.empty() ? std::wstring(L"none") : text;
}

// --- What a rule does to a window -------------------------------------------
//
// Still pure: rectangles are the mod's own type, monitors are plain structs, and
// nothing below calls Win32 or reads mod state. The apply path in the impure half
// is a thin translation of these decisions into calls, which is what makes the
// interesting half of this mod testable without Windows.

struct RuleRect {
    long left = 0;
    long top = 0;
    long right = 0;
    long bottom = 0;
};

static long RuleRectWidth(const RuleRect& rect) {
    return rect.right - rect.left;
}

static long RuleRectHeight(const RuleRect& rect) {
    return rect.bottom - rect.top;
}

static long AbsLong(long value) {
    return value < 0 ? -value : value;
}

// Two pixels is "where it already is", so the mod does not issue a move that
// would only flicker and restart an animation. Without this, re-applying on a
// settings save would nudge every matched window on every save.
static constexpr long kApplyTolerancePixels = 2;

static bool RectsWithinTolerance(const RuleRect& a, const RuleRect& b,
                                 long tolerance) {
    return AbsLong(a.left - b.left) <= tolerance &&
           AbsLong(a.top - b.top) <= tolerance &&
           AbsLong(a.right - b.right) <= tolerance &&
           AbsLong(a.bottom - b.bottom) <= tolerance;
}

// A rule's position and size are separate promises: a rule may set either, both,
// or neither.
struct TargetPlan {
    bool setPosition = false;
    bool setSize = false;
    bool anything = false;
    RuleRect target;
};

// The position is relative to the top-left of the target monitor, not to the
// desktop. Absolute coordinates would break the moment the user docks, undocks,
// or adds a monitor above the primary, because the same numbers would then point
// somewhere else on the desk.
//
// A negative value is allowed: a window the user placed off an edge is allowed to
// stay there.
static TargetPlan ComputeTargetPlan(const RuleRect& current,
                                    const RuleRect& monitor,
                                    const std::wstring& position,
                                    const std::wstring& size) {
    TargetPlan plan;
    plan.target = current;

    long x = 0;
    long y = 0;
    long width = 0;
    long height = 0;

    if (!position.empty() && ParsePositionValue(position, x, y)) {
        plan.setPosition = true;
        const long currentWidth = RuleRectWidth(plan.target);
        const long currentHeight = RuleRectHeight(plan.target);
        plan.target.left = monitor.left + x;
        plan.target.top = monitor.top + y;
        // Moving a window must not resize it. With no size in the rule, the far
        // edges travel with the near ones -- the first version left them where
        // they were, which silently resized every window a position-only rule
        // touched. The unit tests caught it before it reached a desktop.
        if (!plan.setSize) {
            plan.target.right = plan.target.left + currentWidth;
            plan.target.bottom = plan.target.top + currentHeight;
        }
    }

    if (!size.empty() && ParseSizeValue(size, width, height)) {
        plan.setSize = true;
        plan.target.right = plan.target.left + width;
        plan.target.bottom = plan.target.top + height;
    }

    plan.anything = plan.setPosition || plan.setSize;
    return plan;
}

// A monitor as the rules layer sees it. Deliberately not a remembered monitor:
// this one has no relative-to-primary coordinates and no config table, because a
// rule asks "which monitor is this?" and never "which monitor was this?".
struct RuleMonitor {
    std::wstring key;       // stable device interface path, survives a replug
    std::wstring gdiName;   // \\.\DISPLAY1, slot-dependent, for readability
    RuleRect pixelRect;
    bool primary = false;
};

enum class MonitorSpecKind {
    Unset,       // no monitor asked for: stay where the window already is
    Primary,
    Index,       // 1-based, in the order the mod lists monitors (left to right)
    DeviceName,  // a device name, a device path, or a fragment of one
};

struct MonitorSpec {
    MonitorSpecKind kind = MonitorSpecKind::Unset;
    int index = 0;
    std::wstring text;  // lowered, for DeviceName
};

static MonitorSpec ParseMonitorSpec(const std::wstring& text) {
    MonitorSpec spec;
    if (text.empty()) {
        return spec;
    }

    std::wstring lowered = ToLowerCopy(text);
    if (lowered == L"primary") {
        spec.kind = MonitorSpecKind::Primary;
        return spec;
    }

    // Two digits is the most a display number can be, and the length is checked
    // before the arithmetic: a settings block is editable by hand, and
    // "99999999999" would otherwise overflow and become a confident nonsense
    // monitor number.
    if (IsAllDigits(text) && text.size() <= 2) {
        spec.kind = MonitorSpecKind::Index;
        int value = 0;
        for (wchar_t c : text) {
            value = value * 10 + (c - L'0');
        }
        spec.index = value;
        return spec;
    }

    spec.kind = MonitorSpecKind::DeviceName;
    spec.text = lowered;
    return spec;
}

struct MonitorChoice {
    bool usable = false;
    int index = -1;
    std::wstring reason;  // why not usable, for the journal
};

// Windows reports a monitor's name as "\\.\DISPLAY2"; nobody types the prefix.
// It is stripped before comparing, so "DISPLAY2" -- the name the user sees in
// Windows' own settings -- resolves. The device path is matched as a substring,
// so an identity fragment works too, and neither depends on enumeration order.
static std::wstring ShortDeviceName(const std::wstring& gdiName) {
    static const std::wstring kPrefix = L"\\\\.\\";
    std::wstring lowered = ToLowerCopy(gdiName);
    if (lowered.compare(0, kPrefix.size(), kPrefix) == 0) {
        lowered = lowered.substr(kPrefix.size());
    }
    return lowered;
}

// Resolving by identity, not by slot number.
static MonitorChoice ResolveRuleMonitor(const std::vector<RuleMonitor>& monitors,
                                        const MonitorSpec& spec,
                                        int fallbackIndex) {
    MonitorChoice choice;

    const auto usable = [&monitors](int index) {
        return index >= 0 && index < (int)monitors.size();
    };

    switch (spec.kind) {
        case MonitorSpecKind::Unset:
            // The window keeps whatever monitor it is on -- which is the monitor
            // it was just opened on, so this is the common case for a rule that
            // only sets a position.
            if (usable(fallbackIndex)) {
                choice.usable = true;
                choice.index = fallbackIndex;
            } else {
                choice.reason = L"no monitor to work from";
            }
            return choice;

        case MonitorSpecKind::Primary:
            for (size_t i = 0; i < monitors.size(); i++) {
                if (monitors[i].primary) {
                    choice.usable = true;
                    choice.index = (int)i;
                    return choice;
                }
            }
            choice.reason = L"no primary monitor reported";
            return choice;

        case MonitorSpecKind::Index: {
            int index = spec.index - 1;  // the setting is 1-based, like a display number
            if (usable(index)) {
                choice.usable = true;
                choice.index = index;
            } else {
                choice.reason = L"no monitor number " +
                                std::to_wstring(spec.index) + L" (there " +
                                (monitors.size() == 1 ? L"is 1 monitor"
                                                      : L"are " +
                                                            std::to_wstring(
                                                                monitors.size()) +
                                                            L" monitors") +
                                L")";
            }
            return choice;
        }

        case MonitorSpecKind::DeviceName:
            for (size_t i = 0; i < monitors.size(); i++) {
                const std::wstring shortName = ShortDeviceName(monitors[i].gdiName);
                if (shortName == spec.text ||
                    ToLowerCopy(monitors[i].gdiName) == spec.text) {
                    choice.usable = true;
                    choice.index = (int)i;
                    return choice;
                }
            }
            for (size_t i = 0; i < monitors.size(); i++) {
                if (ToLowerCopy(monitors[i].key).find(spec.text) !=
                    std::wstring::npos) {
                    choice.usable = true;
                    choice.index = (int)i;
                    return choice;
                }
            }
            choice.reason = L"no monitor matches \"" + spec.text + L"\"";
            return choice;
    }

    choice.reason = L"unrecognized monitor setting";
    return choice;
}

// Why an apply did not happen. One reason per outcome, and the reason is
// journalled, so "the window did not move" always has an answer in the log.
enum class ApplyDecision {
    Proceed,
    AlreadyThere,
    Overridden,   // the user moved it; their intent outranks the rule
    Minimized,    // a minimized window has no rectangle worth applying to
    Refused,      // a previous move was refused (elevated window); do not retry
};

// Order matters and is deliberate. A refusal is permanent for the session, so it
// is checked first: retrying a refused window on every event would be a journal
// full of identical failures. Then the user's override, then the minimization,
// then "already there" -- which is checked last so that a window that is both
// already there and overridden is reported as overridden, which is the more
// useful fact.
static ApplyDecision DecideApply(bool overridden, bool refused, bool minimized,
                                 bool alreadyThere) {
    if (refused) {
        return ApplyDecision::Refused;
    }
    if (overridden) {
        return ApplyDecision::Overridden;
    }
    if (minimized) {
        return ApplyDecision::Minimized;
    }
    if (alreadyThere) {
        return ApplyDecision::AlreadyThere;
    }
    return ApplyDecision::Proceed;
}

// --- Always on top ----------------------------------------------------------
//
// The topmost bit is sticky: once set, activating an ordinary window does not
// take it away. What it does answer to is another topmost window (the taskbar,
// another pinned app) and the application itself. That is why this mod
// re-asserts nothing on a foreground change and re-pins nothing an app cleared:
// the only thing it does with the foreground is step aside for a fullscreen app
// on the same monitor, and step back when that app is gone.
enum class TopmostDecision {
    NoChange,      // the rule does not ask for it
    Claim,         // set the bit, and record that this mod set it
    AlreadyThere,  // the app pinned it; not ours, so never withdrawn
    Overridden,    // the user moved the window; the rule is not applied to it
    Refused,       // an earlier change to this window was refused
    Minimized,     // the rule is not applied to a minimized window
    Yielded,       // a fullscreen app owns the monitor: record the claim, pin later
};

// A claim means the mod flipped the bit, which is what makes "withdrawn" an
// honest word: a window the app pinned by itself can never enter the claim set,
// so releasing the rules can never demote it. Hence isTopmost never yields
// Claim, and the caller stores a claim only on a not-topmost to topmost move.
static TopmostDecision DecideTopmost(bool ruleWantsOn, bool isTopmost,
                                     bool overridden, bool refused,
                                     bool minimized, bool yieldNow) {
    if (!ruleWantsOn) {
        return TopmostDecision::NoChange;
    }
    if (refused) {
        return TopmostDecision::Refused;
    }
    if (overridden) {
        return TopmostDecision::Overridden;
    }
    if (minimized) {
        return TopmostDecision::Minimized;
    }
    if (isTopmost) {
        return TopmostDecision::AlreadyThere;
    }
    if (yieldNow) {
        return TopmostDecision::Yielded;
    }
    return TopmostDecision::Claim;
}

// What to do about a rule's opacity for one window. Three answers, and the split
// matters for the same reason it does for topmost: the mod may only undo what it
// itself set, so it has to know whether the layered style on this window is its
// own or the application's.
enum class OpacityDecision {
    Apply,          // set the style and the value, and record the value
    AlreadyThere,   // the mod set this exact value; nothing to do
    SkipAppLayered, // the application layered this window itself: not ours
};

// `ours` is the window being in the mod's own record and `currentAlpha` is the
// value that record holds. `styleSet` is whether the window carries the layered
// style right now. A recorded window whose style was cleared never reaches this
// question: the caller forgets it instead, the way the topmost claim is dropped
// when an app removes that bit.
static OpacityDecision DecideOpacity(bool styleSet, bool ours, int currentAlpha,
                                     int wantedAlpha) {
    if (!styleSet) {
        // Not layered and not ours: nothing to preserve, so layer it.
        return OpacityDecision::Apply;
    }
    if (!ours) {
        return OpacityDecision::SkipAppLayered;
    }
    if (currentAlpha == wantedAlpha) {
        return OpacityDecision::AlreadyThere;
    }
    return OpacityDecision::Apply;
}

// Does this window rectangle cover this monitor? "Cover" is containment, not
// equality, deliberately: a maximized window with an auto-hide taskbar is the
// monitor plus about seven pixels of invisible resize border on the left, right
// and bottom, so an equality test would read a maximize as a fullscreen app. The
// state test in IsFullscreenCandidate is what separates the two, and this
// function stays purely geometric so that it can be tested with numbers.
static bool RectsCoverMonitor(const RuleRect& window, const RuleRect& monitor,
                              long tolerance) {
    return window.left <= monitor.left + tolerance &&
           window.top <= monitor.top + tolerance &&
           window.right >= monitor.right - tolerance &&
           window.bottom >= monitor.bottom - tolerance;
}

// A maximized window is never a fullscreen app: it is the work area when the
// taskbar is showing, and the monitor plus a few pixels when the taskbar is set
// to hide, and in both cases it is zoomed -- which a borderless fullscreen app
// never is. Geometry alone cannot tell those two apart; the window state can.
static bool IsFullscreenCandidate(const RuleRect& window,
                                  const RuleRect& monitor, bool zoomed,
                                  long tolerance) {
    if (zoomed) {
        return false;
    }
    return RectsCoverMonitor(window, monitor, tolerance);
}

// Which monitor a rectangle covers, or -1. Monitors do not overlap, so the first
// match is the only one.
static int FindCoveredMonitor(const RuleRect& window,
                              const std::vector<RuleMonitor>& monitors,
                              long tolerance) {
    for (size_t i = 0; i < monitors.size(); i++) {
        if (RectsCoverMonitor(window, monitors[i].pixelRect, tolerance)) {
            return (int)i;
        }
    }
    return -1;
}

// What to do about one claimed window, given what is covering which monitor.
enum class YieldDecision {
    Nothing,
    Demote,      // claimed and still topmost, and its monitor is covered
    AppCleared,  // claimed, but the bit is gone: the app cleared it, so is the claim
    Reclaim,     // yielded, and its monitor is not covered any more
};

// The order matters here for the same reason it does in DecideApply: a window
// this mod demoted must never be demoted again, and a window whose bit the app
// cleared must never be re-pinned on the next activation -- that would be the
// fight this mod exists to avoid.
static YieldDecision DecideYield(bool claimed, bool yielded, bool topmostBitSet,
                                 bool overridden, int windowMonitor,
                                 int coveredMonitor) {
    // A window the user has placed by hand is not touched again this session,
    // and the z-order is part of that promise. Without this check, dragging a
    // window while it happened to be yielded would re-pin it the moment the
    // fullscreen app closed -- the mod acting on a window it said it had let go.
    if (overridden) {
        return YieldDecision::Nothing;
    }
    if (yielded) {
        if (coveredMonitor >= 0 && windowMonitor == coveredMonitor) {
            return YieldDecision::Nothing;
        }
        // Reclaim when the cover is gone, and also when the window's monitor
        // cannot be resolved: leaving a window demoted forever is the worse of
        // the two failures.
        return YieldDecision::Reclaim;
    }
    if (!claimed) {
        return YieldDecision::Nothing;
    }
    if (coveredMonitor < 0 || windowMonitor != coveredMonitor) {
        return YieldDecision::Nothing;
    }
    return topmostBitSet ? YieldDecision::Demote : YieldDecision::AppCleared;
}

enum class SettleDecision {
    Done,   // the window is where the rule asked
    Retry,  // the app moved it and one retry is left
    Wait,   // the app moved it, no retries left, and time remains
    GiveUp  // out of time; leave it alone
};

// One retry, then stop: the same "never fight the user or the app" rule the
// rest of the mod follows, and the reason an application that repositions itself
// on every show is left alone rather than fought forever.
static constexpr int kRetriesPerApply = 1;
static constexpr unsigned long long kSettleWindowMs = 800;

// A move is asynchronous, so for a moment after issuing one the window is
// legitimately still where it was. Without a grace period, the first poll reads
// that as "the app moved it back" and fires the single retry on every apply --
// two moves where one was asked for, and a journal full of retries that describe
// nothing real. This is the trap a settle state machine exists for.
static constexpr unsigned long long kSettleGraceMs = 250;

static SettleDecision DecideSettle(bool atTarget, int retriesLeft,
                                   bool withinGrace, bool timedOut) {
    if (atTarget) {
        return SettleDecision::Done;
    }
    if (withinGrace) {
        return SettleDecision::Wait;
    }
    if (timedOut) {
        return SettleDecision::GiveUp;
    }
    if (retriesLeft > 0) {
        return SettleDecision::Retry;
    }
    return SettleDecision::Wait;
}

// The SID text the per-user folder is named after, rendered by hand.
//
// ConvertSidToStringSidW does this job and is what the mod calls first. This stays
// underneath it for two reasons: the conversion decides a directory name, so a
// failed call must not silently turn diagnostics off, and the arithmetic is a pure
// function that the unit tests pin against strings the field scripts also compare
// file names against.
//
// An identifier authority larger than 32 bits is rendered in hexadecimal by the
// SID documentation. A process token's authority is 5 on every installation this
// mod runs on, so that rendering is unreachable here and this keeps one decimal
// path.
static std::wstring FormatSidString(unsigned revision, unsigned long long authority,
                                    const std::vector<unsigned long>& subAuthorities) {
    std::wstring text = L"S-" + std::to_wstring(revision) + L"-" +
                        std::to_wstring(authority);
    for (unsigned long sub : subAuthorities) {
        text += L"-" + std::to_wstring(sub);
    }
    return text;
}

// <<< RULES_CORE <<<

// Outside the pure-core region above: nothing in the matching layer calls it, and
// the standalone test build would warn about that.
static std::wstring BaseNameOf(const std::wstring& path) {
    size_t separator = path.find_last_of(L"\\/");
    return separator == std::wstring::npos ? path : path.substr(separator + 1);
}

// --- Settings ---------------------------------------------------------------

static std::vector<RuleSpec> g_rules;
static std::vector<CompiledRule> g_compiledRules;
static std::vector<RuleActions> g_ruleActions;

// Reads one setting, leaving the value untouched when the key is absent. Every
// key is read this way: a rule entry whose "name" key is missing -- which is what
// a hand-edited settings block can look like -- must not end the loop and silently
// drop every rule after it.
static bool ReadStringSetting(PCWSTR key, std::wstring& out) {
    PCWSTR value = Wh_GetStringSetting(key);
    // Windhawk hands back an EMPTY STRING for a settings key that is not there,
    // and treating "empty" as "present" once made the loop below run forever on
    // the mod's own event thread: it spun inside LoadRules and never reached the
    // code that installs the hook, so the mod started, wrote its build line and
    // then did nothing at all. Emptiness is the only signal there is, and it is
    // also what a blank field means, so the two agree.
    //
    // The null test stays. It costs nothing, and this is the one reader where
    // getting the contract wrong has already cost a debugging session; the
    // geometry-memory mod guards here the same way.
    if (!value || !*value) {
        Wh_FreeStringSetting(value);
        return false;
    }
    out = value;
    Wh_FreeStringSetting(value);
    return true;
}

// Journals as it loads, so that "the rule is there and the mod agrees with it"
// is a fact in the log rather than an assumption.
static void LoadRules() {
    g_rules.clear();
    g_compiledRules.clear();
    g_ruleActions.clear();

    for (int index = 0;; index++) {
        RuleSpec rule;
        RuleActions actions;
        WCHAR key[64];

        // Every key is read by the same helper, which reports "not there" for a
        // missing key and for a blank one alike. A rule written by hand and a
        // rule saved by the settings UI therefore load identically.
        const std::wstring indexText = std::to_wstring(index);
        const auto keyFor = [&key](const std::wstring& entry,
                                   const wchar_t* field) {
            swprintf_s(key, L"rules[%s].%s", entry.c_str(), field);
        };

        keyFor(indexText, L"name");
        ReadStringSetting(key, rule.name);
        keyFor(indexText, L"executable");
        ReadStringSetting(key, rule.executable);
        keyFor(indexText, L"windowClass");
        ReadStringSetting(key, rule.windowClass);
        keyFor(indexText, L"title");
        ReadStringSetting(key, rule.title);
        keyFor(indexText, L"position");
        ReadStringSetting(key, actions.position);
        keyFor(indexText, L"size");
        ReadStringSetting(key, actions.size);
        keyFor(indexText, L"monitor");
        ReadStringSetting(key, actions.monitor);
        keyFor(indexText, L"opacity");
        ReadStringSetting(key, actions.opacity);
        keyFor(indexText, L"topmost");
        ReadStringSetting(key, actions.topmost);

        // The loop's only exit. It is deliberately the first check, so nothing
        // else in this function can be reached with an empty entry.
        if (!RuleEntryIsPresent(rule, actions)) {
            break;
        }

        keyFor(indexText, L"enabled");
        rule.enabled = Wh_GetIntSetting(key) != 0;

        g_rules.push_back(rule);
        g_compiledRules.push_back(CompileRule(rule));
        g_ruleActions.push_back(actions);
    }

}

// --- Diagnostics journal ----------------------------------------------------
//
// One line appended at the instant of each decision, nothing debounced, nothing
// overwritten, so a test script can assert against the journal without racing the
// mod.
static bool g_diagnosticsLog = false;

// How many times the private folder has been tried this session. Owned here rather
// than inside UserDataFolder so that turning the journal back on can reset it.
static int g_folderAttempts = 0;

// Journalled at startup so a stale build is visible immediately: a journal that
// looks complete but was written by the previous copy of the mod is the easiest
// way to waste an hour.
//
// Hand-maintained rather than WH_MOD_VERSION, because @version has to stay
// numeric for the submission validator ("1.0"), and "1.0" cannot tell one build
// of 1.0 from the next. This tag is what the test scripts compare.
static constexpr PCWSTR kJournalBuild = L"r15-2026-10-21";

static void RefreshDiagnosticsSetting() {
    const bool was = g_diagnosticsLog;
    g_diagnosticsLog = Wh_GetIntSetting(L"diagnosticsLog") != 0;
    // Turning the journal on is a fresh instruction, and it gets fresh attempts.
    // Without this the three tries were spent by the first three journal lines,
    // milliseconds apart, so a folder that was briefly refused was gone for the
    // session no matter what the user did afterwards.
    if (!was && g_diagnosticsLog) {
        g_folderAttempts = 0;
    }
}

static std::string WideToUtf8(const std::wstring& text) {
    if (text.empty()) {
        return std::string();
    }
    int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(),
                                   nullptr, 0, nullptr, nullptr);
    if (size <= 0) {
        return std::string();
    }
    std::string out((size_t)size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), &out[0],
                        size, nullptr, nullptr);
    return out;
}

// --- Per-user storage -------------------------------------------------------
//
// Wh_GetModStoragePath is one folder for the whole machine, and the journal holds
// executable names and window titles: "RULE-NONE exe=... title=\"<a document name>\"".
// A file is readable by whoever can open the folder it is in, and the shared
// storage folder is writable and readable by every account on the machine. A SID
// in a file name would not change that; it would only name the file after its
// reader.
//
// So the journal lives in <storage>\<SID>, created with a protected access control
// list that gives full access to this user, to SYSTEM and to administrators, and
// that inherits nothing from the folder it sits in. What is inside is private by
// construction, whatever the files are called.
//
// The list is built from access-control entries rather than parsed from an SDDL
// string: same three entries as the SDDL form, without a string-to-descriptor
// conversion on a path that runs at startup.

struct UserTokenSid {
    std::vector<BYTE> bytes;
    std::wstring text;
};

static std::wstring SidToString(PSID sid) {
    if (!sid) {
        return std::wstring();
    }

    // The operating system's own conversion, which is the one worth trusting for
    // a string that becomes a directory name.
    LPWSTR converted = nullptr;
    if (ConvertSidToStringSidW(sid, &converted) && converted) {
        std::wstring text(converted);
        LocalFree(converted);
        if (!text.empty()) {
            return text;
        }
    }

    // Reached only when the conversion failed, which it does not do for a token's
    // own SID. The bytes are still unpacked here rather than the journal being
    // lost: a wrong folder name would be a nuisance, no journal at all is worse.
    const SID* parsed = reinterpret_cast<const SID*>(sid);
    // A pointer into a token buffer: a count this large can only mean the buffer
    // is not what it claims to be.
    if (parsed->SubAuthorityCount > 15) {
        return std::wstring();
    }

    unsigned long long authority = 0;
    for (int i = 0; i < 6; i++) {
        authority = (authority << 8) | parsed->IdentifierAuthority.Value[i];
    }

    std::vector<unsigned long> subAuthorities;
    subAuthorities.reserve(parsed->SubAuthorityCount);
    for (unsigned i = 0; i < parsed->SubAuthorityCount; i++) {
        subAuthorities.push_back(parsed->SubAuthority[i]);
    }

    return FormatSidString(parsed->Revision, authority, subAuthorities);
}

// This user's SID in both forms the mod needs: the bytes, for the folder's access
// control list, and the text, for its name. Read once and cached.
static const UserTokenSid& CurrentUserTokenSid() {
    static bool attempted = false;
    static UserTokenSid cached;

    if (attempted) {
        return cached;
    }
    attempted = true;

    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        Wh_Log(L"OpenProcessToken failed (error %lu)", GetLastError());
        return cached;
    }

    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    if (size > 0) {
        std::vector<BYTE> buffer(size);
        if (GetTokenInformation(token, TokenUser, buffer.data(), size, &size)) {
            auto* user = reinterpret_cast<TOKEN_USER*>(buffer.data());
            if (user->User.Sid) {
                const DWORD sidLength = GetLengthSid(user->User.Sid);
                if (sidLength > 0 && sidLength <= size) {
                    const BYTE* first =
                        reinterpret_cast<const BYTE*>(user->User.Sid);
                    cached.bytes.assign(first, first + sidLength);
                    cached.text = SidToString(user->User.Sid);
                }
            }
        }
    }

    CloseHandle(token);
    return cached;
}

// Full access for this user, SYSTEM and administrators, and for nobody else. The
// last two because Windhawk has to be able to remove the folder along with the
// mod.
static bool BuildPrivateAcl(const std::vector<BYTE>& userSid,
                            bool inheritToChildren,
                            std::vector<BYTE>& aclBuffer) {
    if (userSid.empty()) {
        return false;
    }

    const DWORD allowEntrySize =
        sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + SECURITY_MAX_SID_SIZE;
    aclBuffer.assign(sizeof(ACL) + 3 * allowEntrySize + 32, 0);

    PACL acl = reinterpret_cast<PACL>(aclBuffer.data());
    if (!InitializeAcl(acl, (DWORD)aclBuffer.size(), ACL_REVISION)) {
        return false;
    }

    const DWORD aceFlags = inheritToChildren
                               ? (OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE)
                               : 0;

    if (!AddAccessAllowedAceEx(acl, ACL_REVISION, aceFlags, FILE_ALL_ACCESS,
                               reinterpret_cast<PSID>(
                                   const_cast<BYTE*>(userSid.data())))) {
        return false;
    }

    const WELL_KNOWN_SID_TYPE wellKnownSids[] = {WinLocalSystemSid,
                                                 WinBuiltinAdministratorsSid};
    for (WELL_KNOWN_SID_TYPE type : wellKnownSids) {
        BYTE sidBuffer[SECURITY_MAX_SID_SIZE];
        DWORD sidSize = sizeof(sidBuffer);
        if (!CreateWellKnownSid(type, nullptr, sidBuffer, &sidSize)) {
            return false;
        }
        if (!AddAccessAllowedAceEx(acl, ACL_REVISION, aceFlags, FILE_ALL_ACCESS,
                                   reinterpret_cast<PSID>(sidBuffer))) {
            return false;
        }
    }

    return true;
}

// Whether an existing folder is safe to write into.
//
// This user owning it is the case to establish: the storage folder it lives in is
// writable by every account, so another user could create this folder first with
// a list of their own, and everything written inside would inherit it. SYSTEM and
// administrators are accepted too, because an elevated tool process creates
// objects owned by the Administrators group rather than by the account.
static bool FolderIsSafe(const std::wstring& path,
                         const std::vector<BYTE>& userSid) {
    DWORD needed = 0;
    GetFileSecurityW(path.c_str(), OWNER_SECURITY_INFORMATION, nullptr, 0,
                     &needed);
    if (needed == 0) {
        return false;
    }

    std::vector<BYTE> descriptor(needed);
    if (!GetFileSecurityW(path.c_str(), OWNER_SECURITY_INFORMATION,
                          reinterpret_cast<PSECURITY_DESCRIPTOR>(
                              descriptor.data()),
                          needed, &needed)) {
        return false;
    }

    PSID owner = nullptr;
    BOOL defaulted = FALSE;
    if (!GetSecurityDescriptorOwner(
            reinterpret_cast<PSECURITY_DESCRIPTOR>(descriptor.data()), &owner,
            &defaulted) ||
        !owner) {
        return false;
    }

    PSID user = reinterpret_cast<PSID>(const_cast<BYTE*>(userSid.data()));
    return EqualSid(owner, user) != FALSE ||
           IsWellKnownSid(owner, WinBuiltinAdministratorsSid) != FALSE ||
           IsWellKnownSid(owner, WinLocalSystemSid) != FALSE;
}

static bool EnsurePrivateFolder(const std::wstring& path,
                                const std::vector<BYTE>& userSid) {
    std::vector<BYTE> aclBuffer;
    if (!BuildPrivateAcl(userSid, /*inheritToChildren=*/true, aclBuffer)) {
        Wh_Log(L"could not build the private folder's access control list");
        return false;
    }

    SECURITY_DESCRIPTOR descriptor{};
    if (!InitializeSecurityDescriptor(&descriptor,
                                      SECURITY_DESCRIPTOR_REVISION) ||
        !SetSecurityDescriptorDacl(&descriptor, TRUE,
                                   reinterpret_cast<PACL>(aclBuffer.data()),
                                   FALSE) ||
        // The list above and nothing else: without the protected bit the entries
        // inherited from the shared storage folder would be merged into it, and
        // every user on the machine would be back in.
        !SetSecurityDescriptorControl(&descriptor, SE_DACL_PROTECTED,
                                      SE_DACL_PROTECTED) ||
        // Named explicitly rather than left to the token's default owner, which
        // is the Administrators group whenever the tool process runs elevated.
        !SetSecurityDescriptorOwner(
            &descriptor,
            reinterpret_cast<PSID>(const_cast<BYTE*>(userSid.data())),
            FALSE)) {
        Wh_Log(L"could not build the private folder's security descriptor");
        return false;
    }

    SECURITY_ATTRIBUTES attributes{sizeof(attributes), &descriptor, FALSE};
    if (CreateDirectoryW(path.c_str(), &attributes)) {
        return true;
    }

    const DWORD error = GetLastError();
    if (error == ERROR_ALREADY_EXISTS) {
        // Not an error in itself: the folder may be this user's from an earlier
        // start. It is an error if somebody else got there first.
        if (FolderIsSafe(path, userSid)) {
            return true;
        }
        Wh_Log(L"%s already exists and is not this user's; the journal will not "
               L"be written", path.c_str());
        return false;
    }

    Wh_Log(L"could not create %s (error %lu); the journal will not be written",
           path.c_str(), error);
    return false;
}

// This user's folder, created on first use. Empty means "not available", and the
// journal is then simply not written.
//
// Called only from the journal's own write path, which returns early while the
// journal is off: a user who never turns diagnostics on never gets a folder, a
// SID lookup or an access control list. That is why this is not resolved at init.
//
// Cached once the folder exists. A failure is retried a few times and then given
// up on for this session, so a transient refusal does not cost the journal for
// good. The journal path is the event thread's, so there is no race to guard.
//
// How many times the private folder is created before it is given up on.
static constexpr int kMaxFolderAttempts = 3;

static const std::wstring& UserDataFolder() {
    static std::wstring cached;

    if (!cached.empty() || g_folderAttempts >= kMaxFolderAttempts) {
        return cached;
    }
    g_folderAttempts++;

    const UserTokenSid& sid = CurrentUserTokenSid();
    if (sid.text.empty()) {
        // Without a SID there is no folder that can be made private. The mod
        // still works; it just has no journal, rather than putting window titles
        // somewhere every account on the machine can read.
        Wh_Log(L"the user SID could not be read; there will be no diagnostics "
               L"journal");
        return cached;
    }

    WCHAR storage[MAX_PATH];
    const size_t len = Wh_GetModStoragePath(storage, ARRAYSIZE(storage));
    if (len == 0 || len + 1 >= ARRAYSIZE(storage)) {
        Wh_Log(L"failed to get the mod storage path");
        return cached;
    }

    std::wstring folder = storage;
    folder += L"\\";
    folder += sid.text;

    // A failure is not latched for the session: turning the journal off and on
    // again, or fixing whatever was in the way, gets another try. The attempt
    // count is what stops a single stuck failure turning every journal line into
    // a directory creation call.
    if (EnsurePrivateFolder(folder, sid.bytes)) {
        cached = std::move(folder);
    }
    return cached;
}

static bool BuildJournalPath(PWSTR buffer, size_t bufferSize) {
    const std::wstring& folder = UserDataFolder();
    if (folder.empty()) {
        return false;
    }
    return swprintf_s(buffer, bufferSize, L"%s\\%s", folder.c_str(),
                      kJournalFileName) >= 0;
}

// Never fails loudly: a diagnostics write that cannot happen must not affect
// the decisions it exists to observe.
// The journal is a debugging aid that a user may leave on for months, so it is
// capped the same way the sibling mod's is: past this size the file is emptied
// and a line saying so is written first, rather than growing without limit in
// someone's AppData.
static constexpr long long kMaxJournalBytes = 1024 * 1024;


static void AppendDiagnosticsJournal(const std::wstring& line) {
    if (!g_diagnosticsLog) {
        return;
    }

    WCHAR path[MAX_PATH];
    if (!BuildJournalPath(path, ARRAYSIZE(path))) {
        return;
    }

    // Write access, not FILE_APPEND_DATA. An append-only handle cannot truncate:
    // SetEndOfFile needs FILE_WRITE_DATA, and an append handle sends every write
    // to the end whatever the file pointer says. The cap this file advertises was
    // silently doing nothing for exactly that reason, which is the sort of thing a
    // log is supposed to be immune from. The file is seeked by hand instead.
    HANDLE file = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
                              nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    bool truncated = false;
    LARGE_INTEGER size{};
    if (GetFileSizeEx(file, &size) && size.QuadPart > kMaxJournalBytes) {
        SetFilePointer(file, 0, nullptr, FILE_BEGIN);
        truncated = SetEndOfFile(file) != FALSE;
    }
    // Seek to the end in both cases: after a truncation that is position 0.
    SetFilePointer(file, 0, nullptr, FILE_END);

    std::wstring stamped;
    if (truncated) {
        stamped += L"[0] JOURNAL-TRUNCATED the journal passed " +
                   std::to_wstring(size.QuadPart) + L" bytes\r\n";
    }
    stamped += L"[" + std::to_wstring((long long)time(nullptr)) + L"] " + line +
               L"\r\n";
    std::string utf8 = WideToUtf8(stamped);
    DWORD written = 0;
    WriteFile(file, utf8.data(), (DWORD)utf8.size(), &written, nullptr);
    CloseHandle(file);
}

// --- Logging of rules -------------------------------------------------------

static std::map<std::wstring, int> g_noMatchCounts;
static std::map<std::wstring, int> g_filterSkipCounts;

// Budgeted: an unbudgeted "no rule matched" line would write one journal line
// per window on the desktop, which is how a diagnostics feature becomes the
// reason a mod is uninstalled.
static bool TakeBudget(std::map<std::wstring, int>& counts,
                       const std::wstring& key, int limit) {
    int& used = counts[key];
    if (used >= limit) {
        return false;
    }
    used++;
    return true;
}

static std::wstring RuleNameText(int index) {
    if (index < 0 || index >= (int)g_rules.size()) {
        return L"(none)";
    }
    return g_rules[(size_t)index].name.empty() ? L"(unnamed)"
                                               : g_rules[(size_t)index].name;
}

static void JournalRulesLoaded() {
    AppendDiagnosticsJournal(L"RULES-LOADED count=" +
                             std::to_wstring(g_rules.size()));

    for (size_t i = 0; i < g_rules.size(); i++) {
        const RuleSpec& rule = g_rules[i];
        const CompiledRule& compiled = g_compiledRules[i];
        const std::wstring actions =
            DescribeRequestedActions(g_ruleActions[i]);

        AppendDiagnosticsJournal(
            L"RULE-LOADED i=" + std::to_wstring(i) +
            L" enabled=" + (rule.enabled ? L"yes" : L"no") +
            L" name=\"" + (rule.name.empty() ? L"(unnamed)" : rule.name) +
            L"\" exe=\"" + rule.executable + L"\" class=\"" + rule.windowClass +
            L"\" title=\"" + rule.title + L"\" actions=" + actions);

        if (compiled.usable) {
            AppendDiagnosticsJournal(L"RULE-COMPILED i=" + std::to_wstring(i) +
                                     L" exe=" +
                                     std::to_wstring((int)compiled.executable.kind) +
                                     L" class=" +
                                     std::to_wstring((int)compiled.windowClass.kind) +
                                     L" title=" +
                                     std::to_wstring((int)compiled.title.kind));
        } else {
            AppendDiagnosticsJournal(L"RULE-PROBLEM i=" + std::to_wstring(i) +
                                     L" name=\"" + RuleNameText((int)i) +
                                     L"\" problem=\"" + compiled.problem + L"\"");
        }

        for (const std::wstring& problem : DescribeActionProblems(g_ruleActions[i])) {
            AppendDiagnosticsJournal(L"RULE-PROBLEM i=" + std::to_wstring(i) +
                                     L" name=\"" + RuleNameText((int)i) +
                                     L"\" problem=\"" + problem + L"\"");
        }
    }
}

static void JournalRuleHit(int ruleIndex, const MatchOutcome& outcome,
                           const std::wstring& exeName) {
    if (!g_diagnosticsLog) {
        return;
    }
    // The index comes from the matcher and the three rule vectors are filled in
    // lockstep, so this cannot be out of range. It is still bounded here: the
    // journal is the one thing that runs on every match, and an out-of-range read
    // in it would be a crash in the code that exists to explain crashes.
    const std::wstring actions =
        ruleIndex >= 0 && ruleIndex < (int)g_ruleActions.size()
            ? DescribeRequestedActions(g_ruleActions[(size_t)ruleIndex])
            : std::wstring(L"none");
    AppendDiagnosticsJournal(
        L"RULE-HIT exe=" + exeName + L" rule=" + std::to_wstring(ruleIndex) +
        L" name=\"" + RuleNameText(ruleIndex) + L"\" matched=" + outcome.field +
        L":\"" + outcome.value + L"\" actions=" + actions);
}

static void JournalNoMatch(const std::wstring& exeName,
                           const std::wstring& title) {
    // Before the budget, not after: with the journal off nothing is built, and
    // nothing is counted, so turning it on starts from a full budget.
    if (!g_diagnosticsLog) {
        return;
    }
    if (!TakeBudget(g_noMatchCounts, exeName, kMaxNoMatchLinesPerApp)) {
        return;
    }
    AppendDiagnosticsJournal(L"RULE-NONE exe=" + exeName + L" title=\"" + title +
                             L"\"" + (g_noMatchCounts[exeName] == 1
                                          ? L" (further windows of this app are "
                                            L"not journalled)"
                                          : L""));
}

// The executable name costs an OpenProcess, so it is resolved only when a line
// is actually about to be written. The budget above means that happens at most
// ten times per rejection reason, so the cost is bounded by the setting that
// benefits from it.
//
// It is resolved at all because "(unknown)" in a journal is not an answer: the
// first run on a real desktop printed twenty rejection lines that named no process, which
// made it impossible to tell the app being tested from the shell's own windows.
static std::wstring ExeNameForLog(HWND hwnd) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (!processId) {
        return L"(no process id)";
    }

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return L"(process not readable)";
    }

    WCHAR path[MAX_PATH]{};
    DWORD pathLength = ARRAYSIZE(path);
    BOOL queried = QueryFullProcessImageNameW(process, 0, path, &pathLength);
    CloseHandle(process);
    if (!queried) {
        return L"(path unavailable)";
    }

    return BaseNameOf(path);
}

static void JournalFilterSkip(HWND hwnd, PCWSTR reason) {
    std::wstring key = reason;
    if (!TakeBudget(g_filterSkipCounts, key, kMaxFilterSkipLinesPerReason)) {
        return;
    }
    AppendDiagnosticsJournal(L"RULE-FILTER-SKIP exe=" + ExeNameForLog(hwnd) +
                             L" reason=\"" + key + L"\"");
}

// --- Window inspection ------------------------------------------------------

// GetWindowTextW does not send WM_GETTEXT to windows owned by other processes
// (it reads the cached caption), so this cannot block on a hung app.
static std::wstring GetWindowTitle(HWND hwnd) {
    int length = GetWindowTextLengthW(hwnd);
    if (length <= 0) {
        return std::wstring();
    }

    // length + 1 characters, because the count passed to GetWindowTextW includes
    // the terminator. Sized to exactly that rather than to `length` plus the
    // string's own null slot, which works but reads like an off-by-one.
    std::wstring title((size_t)length + 1, L'\0');
    int copied = GetWindowTextW(hwnd, &title[0], length + 1);
    if (copied <= 0) {
        return std::wstring();
    }

    title.resize((size_t)copied);
    return title;
}

// *reason receives a short human-readable explanation of the first filter that
// rejected the window, so the journal answers "why was my rule not applied to
// this window?" without anyone having to read this function.
static bool PassesWindowFilters(HWND hwnd, const wchar_t** reason) {
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
        *reason = L"child window";
        return false;
    }
    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        *reason = L"owned window (dialog)";
        return false;
    }
    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
        *reason = L"tool window";
        return false;
    }
    // Hidden windows are framework scaffolding, not app windows: WinUI 3 and
    // Windows App SDK apps create several such top-level windows per window
    // (input, composition and adapter windows). They are never shown, so they
    // are rejected here and the real window is picked up on its SHOW event.
    if (!IsWindowVisible(hwnd)) {
        *reason = L"not visible";
        return false;
    }
    // Cloaked windows are invisible to the user: suspended UWP apps, XAML
    // islands on another virtual desktop, and similar. A rule applied to one
    // would act on something nobody can see.
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked != 0) {
        *reason = L"cloaked (not shown to the user)";
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(hwnd, &rect)) {
        *reason = L"window rectangle unavailable";
        return false;
    }
    if (rect.right <= rect.left || rect.bottom <= rect.top) {
        *reason = L"zero-sized (not laid out yet)";
        return false;
    }

    return true;
}

// The facts a rule is matched against. Everything here is read-only: matching
// only ever reads a window, because deciding what to do with one is a separate
// step.
struct WindowFacts {
    std::wstring exePath;
    std::wstring exeName;
    std::wstring windowClass;
    std::wstring title;
};

static bool ResolveWindowFacts(HWND hwnd, WindowFacts& facts,
                               const wchar_t** reason) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (!processId) {
        *reason = L"no process id";
        return false;
    }
    // This mod runs inside windhawk.exe, so any window of this process is one of
    // Windhawk's own. Named separately because reporting "no process id" for a
    // window that has a perfectly good process id sent a field run looking in the
    // wrong place.
    if (processId == GetCurrentProcessId()) {
        *reason = L"window belongs to Windhawk itself";
        return false;
    }

    // PROCESS_QUERY_LIMITED_INFORMATION succeeds across integrity levels, so
    // elevated apps resolve here.
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        *reason = L"process not readable (protected or elevated)";
        return false;
    }

    WCHAR path[MAX_PATH]{};
    DWORD pathLength = ARRAYSIZE(path);
    BOOL queried = QueryFullProcessImageNameW(process, 0, path, &pathLength);
    CloseHandle(process);
    if (!queried) {
        *reason = L"process path unavailable";
        return false;
    }

    facts.exePath = path;
    facts.exeName = BaseNameOf(facts.exePath);

    for (PCWSTR excluded : kExcludedProcesses) {
        if (_wcsicmp(facts.exeName.c_str(), excluded) == 0) {
            *reason = L"process is on the built-in exclude list";
            return false;
        }
    }

    WCHAR className[256]{};
    int classNameLength = GetClassNameW(hwnd, className, ARRAYSIZE(className));
    if (classNameLength <= 0) {
        *reason = L"no window class";
        return false;
    }
    facts.windowClass.assign(className, (size_t)classNameLength);

    // A window with no title is not something a user opens: in practice every
    // scaffolding window observed had an empty title, while every real window
    // had one. It is still matched here (a rule with a blank title field would
    // match it), which is exactly the kind of thing the journal should show.
    facts.title = GetWindowTitle(hwnd);
    return true;
}

// --- Acting on a window -----------------------------------------------------
//
// Everything from here down touches the operating system. It is deliberately
// thin: the decisions it makes were all taken in the pure core above, and each
// line below is a translation of one of them into a call.

static void EvaluateWindow(HWND hwnd, bool forced, PCWSTR source);

static int g_appliedSincePass = 0;

// The mod's hidden message window: the settle timer is armed on it (a timer on a
// window owned by another process fails and returns 0), and messages from
// Windhawk's threads arrive through it.
static std::atomic<HWND> g_messageWindow{nullptr};

// --- Monitor views ----------------------------------------------------------

static std::vector<RuleMonitor> g_monitors;

static BOOL CALLBACK MonitorEnumProc(HMONITOR monitor, HDC /*hdc*/,
                                     LPRECT /*rect*/, LPARAM data) {
    auto* monitors = reinterpret_cast<std::vector<RuleMonitor>*>(data);

    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return TRUE;
    }

    RuleMonitor view;
    view.gdiName = info.szDevice;
    view.pixelRect = {info.rcMonitor.left, info.rcMonitor.top,
                      info.rcMonitor.right, info.rcMonitor.bottom};
    view.primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;

    // The stable identity: the device interface path, which does not change when
    // a monitor is unplugged and replugged into a different slot. The device name
    // (\\.\DISPLAY2) does change, which is why a rule may use either.
    DISPLAY_DEVICEW device{};
    device.cb = sizeof(device);
    if (EnumDisplayDevicesW(info.szDevice, 0, &device,
                            EDD_GET_DEVICE_INTERFACE_NAME)) {
        view.key = device.DeviceID;
    }

    monitors->push_back(view);
    return TRUE;
}

// Left to right, then top to bottom, so that "monitor 1" means the same thing on
// every machine and in every session: the leftmost, and ties broken upwards. The
// alternative -- Windows' enumeration order -- is not stable across reboots, and
// a rule that silently pointed at a different monitor after a reboot would be
// worse than one that refuses.
static void SortMonitorsPositionally(std::vector<RuleMonitor>& monitors) {
    std::sort(monitors.begin(), monitors.end(),
              [](const RuleMonitor& a, const RuleMonitor& b) {
                  if (a.pixelRect.left != b.pixelRect.left) {
                      return a.pixelRect.left < b.pixelRect.left;
                  }
                  if (a.pixelRect.top != b.pixelRect.top) {
                      return a.pixelRect.top < b.pixelRect.top;
                  }
                  return a.gdiName < b.gdiName;
              });
}

static void RefreshMonitorViews() {
    std::vector<RuleMonitor> monitors;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                        reinterpret_cast<LPARAM>(&monitors));
    SortMonitorsPositionally(monitors);
    g_monitors = std::move(monitors);
}

// Which monitor a window is on, as an index into g_monitors.
static int MonitorIndexForWindow(HWND hwnd) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return -1;
    }

    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return -1;
    }

    for (size_t i = 0; i < g_monitors.size(); i++) {
        if (_wcsicmp(g_monitors[i].gdiName.c_str(), info.szDevice) == 0) {
            return (int)i;
        }
    }
    return -1;
}

static bool WindowRectOf(HWND hwnd, RuleRect& rect) {
    RECT raw{};
    if (!GetWindowRect(hwnd, &raw)) {
        return false;
    }
    rect = {raw.left, raw.top, raw.right, raw.bottom};
    return true;
}

// --- Applying a rule --------------------------------------------------------

struct PendingApply {
    RuleRect target;
    int ruleIndex = -1;
    int retriesLeft = kRetriesPerApply;
    unsigned long long startedTick = 0;   // when the apply began, for the deadline
    unsigned long long lastMoveTick = 0;  // when a move was last issued, for the grace
};

static constexpr UINT kSettleTickMs = 100;
static constexpr UINT_PTR kSettleTimerId = 1;

static std::unordered_map<HWND, PendingApply> g_pendingApplies;

// The user moved a window: their intent outranks the rule for the rest of the
// session. Nothing re-applies to a window in this set and no rule reasserts --
// the first principle is that the computer must not fight the person using it.
// An "always put it back" mode would be a different feature and would have to be
// asked for explicitly.
static std::unordered_set<HWND> g_overriddenWindows;

// A move that Windows refused: the window is elevated and this process may not
// move it. Remembered for the session so the mod does not retry on every event,
// and so the journal says it once instead of a hundred times.
static std::unordered_set<HWND> g_unwritableWindows;

// Windows a rule matched while they were minimized. A minimized window's
// rectangle is a placeholder and moving it is meaningless, so the rule is not
// applied -- but the window must not be written off as decided either, or an app
// that starts in the tray would never get its rule when the user finally opens
// it. These are re-examined on the next event, which on a restore is the SHOW
// that ShowWindow raises.
static std::unordered_set<HWND> g_deferredWindows;

// Windows whose topmost bit this mod set, with the executable name for the
// journal. A window the application pinned by itself is never in here: the mod
// claims only what it changed, which is what lets it lower a claim again without
// ever demoting a window the user pinned on purpose.
static std::unordered_map<HWND, std::wstring> g_topmostWindows;

// Windows in the claim set whose bit is currently withdrawn because a fullscreen
// app covers their monitor. Separate from the claim set because the claim
// survives the yield: the mod still owns the bit and puts it back.
static std::unordered_set<HWND> g_yieldedWindows;

// Windows whose layered style this mod added, with the alpha it set. The value is
// kept so a reapply pass can say "already-there" instead of reporting a fresh
// change on every sweep, and so that releasing the opacity knows what it owns.
// A window the application layered by itself is never in here -- the mod does not
// touch those at all, because a flat alpha on a per-pixel-alpha window renders
// wrong.
struct OpacityRecord {
    int alpha = 255;
    std::wstring exeName;
};

static std::unordered_map<HWND, OpacityRecord> g_opacityWindows;

// Windows a rule has matched at some point this session. The override exists to
// stop the mod fighting a window it governs; dragging a window no rule touches
// must not record anything, or a rule added for that app later would be silently
// blocked for the rest of the session.
static std::unordered_set<HWND> g_governedWindows;

static UINT_PTR g_settleTimer = 0;

static void EnsureSettleTimer() {
    if (g_settleTimer) {
        return;
    }
    HWND target = g_messageWindow.load();
    if (!target) {
        return;
    }
    // Armed on the mod's own message window: a timer armed on a window owned by
    // another process fails and returns 0, which would silently disable the
    // settle poll for the rest of the session.
    g_settleTimer = SetTimer(target, kSettleTimerId, kSettleTickMs, nullptr);
}

static void StopSettleTimerIfIdle() {
    if (!g_pendingApplies.empty() || !g_settleTimer) {
        return;
    }
    HWND target = g_messageWindow.load();
    if (target) {
        KillTimer(target, kSettleTimerId);
    }
    g_settleTimer = 0;
}

// The mod's single SetWindowPos call site, asserted as such by the boundary
// checks. Asynchronous, because a synchronous move can deadlock against a window
// owned by another thread, and never activating, because a rule must not steal
// focus.
//
// The z-order half shares that call site rather than adding a second one, so a rule that sets
// a position and "always on top" together moves and pins in one call. Two calls
// would restack the window twice, which is visible as a flicker. A null rectangle
// means "z-order only", which is how a pin, a yield and a withdrawal are issued.
static DWORD ApplyWindowPos(HWND hwnd, const RuleRect* rect, HWND insertAfter) {
    UINT flags = SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;
    int x = 0;
    int y = 0;
    int cx = 0;
    int cy = 0;
    if (rect) {
        x = (int)rect->left;
        y = (int)rect->top;
        cx = (int)RuleRectWidth(*rect);
        cy = (int)RuleRectHeight(*rect);
    } else {
        flags |= SWP_NOMOVE | SWP_NOSIZE;
    }
    if (!insertAfter) {
        flags |= SWP_NOZORDER;
    }
    SetLastError(ERROR_SUCCESS);
    BOOL moved = SetWindowPos(hwnd, insertAfter, x, y, cx, cy, flags);
    return moved ? ERROR_SUCCESS : GetLastError();
}

static void JournalApply(const WindowFacts& facts, int ruleIndex, PCWSTR source,
                         PCWSTR result, const RuleRect& target,
                         const RuleRect& current, DWORD error) {
    if (!g_diagnosticsLog) {
        return;
    }
    AppendDiagnosticsJournal(
        std::wstring(L"APPLY exe=") + facts.exeName + L" rule=" +
        std::to_wstring(ruleIndex) + L" source=" + source +
        L" result=" + result +
        L" current=(" + std::to_wstring(current.left) + L"," +
        std::to_wstring(current.top) + L"," + std::to_wstring(current.right) +
        L"," + std::to_wstring(current.bottom) + L")" +
        L" target=(" + std::to_wstring(target.left) + L"," +
        std::to_wstring(target.top) + L"," + std::to_wstring(target.right) +
        L"," + std::to_wstring(target.bottom) + L")" +
        (error != ERROR_SUCCESS
             ? L" error=" + std::to_wstring((unsigned long)error)
             : std::wstring()));
}

// The z-order state as the journal should report it: what the window actually
// has, not what the mod intended. A blocked or refused path never changed it, and
// a line claiming state=off for a window that is still on top is worse than no
// line at all.
static PCWSTR TopmostStateOf(HWND hwnd) {
    return (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) ? L"on" : L"off";
}

// One line for anything the mod does, or declines to do, with the z-order. Kept
// separate from APPLY so that APPLY keeps meaning "this is the rectangle it aimed
// at", and a combined move and pin writes one line of each for a single call.
static void JournalTopmost(const std::wstring& exeName, int ruleIndex,
                           PCWSTR source, PCWSTR result, PCWSTR state) {
    if (!g_diagnosticsLog) {
        return;
    }
    AppendDiagnosticsJournal(
        std::wstring(L"TOPMOST exe=") + exeName + L" rule=" +
        std::to_wstring(ruleIndex) + L" source=" +
        (source ? source : L"none") + L" result=" + result + L" state=" + state);
}

static void JournalOpacity(const std::wstring& exeName, int ruleIndex,
                           PCWSTR source, PCWSTR result, int alpha) {
    if (!g_diagnosticsLog) {
        return;
    }
    AppendDiagnosticsJournal(
        std::wstring(L"OPACITY exe=") + exeName + L" rule=" +
        std::to_wstring(ruleIndex) + L" source=" +
        (source ? source : L"none") + L" result=" + result + L" alpha=" +
        std::to_wstring(alpha));
}

// Records a claim and says so. Only ever called after the mod itself flipped the
// bit from clear to set.
static void ClaimTopmost(HWND hwnd, const std::wstring& exeName, int ruleIndex,
                         PCWSTR source) {
    g_topmostWindows[hwnd] = exeName;
    g_yieldedWindows.erase(hwnd);
    JournalTopmost(exeName, ruleIndex, source, L"applied", L"on");
}

// The claim is the mod's, so releasing it is the mod's job too. A window the app
// pinned by itself is not in the claim set and is never touched here.
static void WithdrawTopmost(HWND hwnd, const std::wstring& exeName,
                            int ruleIndex, PCWSTR source) {
    auto claim = g_topmostWindows.find(hwnd);
    if (claim == g_topmostWindows.end()) {
        // Not ours. The one case worth a line is a window that IS on top and that
        // this mod did not pin: the journal should answer "why did it stay up"
        // without anyone reading the code. Silent otherwise, because every
        // reapply pass evaluates every matched window.
        // Silent for a window this process may not change at all: it is reported
        // as unwritable where that is decided, and repeating it on every sweep
        // would bury the lines that matter.
        if ((GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) &&
            !g_unwritableWindows.count(hwnd)) {
            JournalTopmost(exeName, ruleIndex, source, L"not-claimed", L"on");
        }
        return;
    }
    g_yieldedWindows.erase(hwnd);
    if (!(GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        // The app cleared it first. Nothing to lower, and nothing left to own.
        g_topmostWindows.erase(claim);
        JournalTopmost(exeName, ruleIndex, source, L"app-cleared", L"off");
        return;
    }
    const DWORD error = ApplyWindowPos(hwnd, nullptr, HWND_NOTOPMOST);
    if (error == ERROR_SUCCESS) {
        g_topmostWindows.erase(claim);
    }
    // Read back rather than assumed: a withdrawal the window refused leaves the
    // bit set, and the journal is the only place that shows.
    JournalTopmost(exeName, ruleIndex, source,
                   error == ERROR_SUCCESS ? L"withdrawn" : L"failed",
                   TopmostStateOf(hwnd));
}

// Everything the mod owns goes back down: the mod was turned off, or it is
// unloading. A pinned window that outlives the mod is the one piece of state a
// user would have to fix by hand.
static void ReleaseAllTopmost(PCWSTR source) {
    std::vector<std::pair<HWND, std::wstring>> claims(g_topmostWindows.begin(),
                                                      g_topmostWindows.end());
    g_topmostWindows.clear();
    g_yieldedWindows.clear();

    for (const auto& claim : claims) {
        if (!IsWindow(claim.first)) {
            continue;
        }
        if (!(GetWindowLongPtrW(claim.first, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
            JournalTopmost(claim.second, -1, source, L"app-cleared", L"off");
            continue;
        }
        const DWORD error = ApplyWindowPos(claim.first, nullptr, HWND_NOTOPMOST);
        JournalTopmost(claim.second, -1, source,
                       error == ERROR_SUCCESS ? L"withdrawn" : L"failed",
                       TopmostStateOf(claim.first));
    }
}

// Opacity through the layered-window style.
//
// A window that was ALREADY layered when the mod first saw it is left alone. It
// is almost certainly using per-pixel alpha (every modern custom-chrome app
// does), and setting a flat alpha on it replaces that with a uniform
// translucency -- which renders wrong, in a way that looks like the app's bug.
// Journaled as a skip rather than silently doing the wrong thing.
//
// A window the mod layered itself is a different case: the style is the mod's,
// so the value is the mod's to change and to take back. Without the record, an
// edited opacity would be read as "already layered, not mine" and silently
// ignored -- which is exactly what an earlier build did.
static void ApplyOpacity(HWND hwnd, int alpha, const WindowFacts& facts,
                         int ruleIndex, PCWSTR source) {
    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    auto mine = g_opacityWindows.find(hwnd);
    const bool ours = mine != g_opacityWindows.end();
    // Copied before anything can erase the record: the branch below does exactly
    // that, and reading the iterator afterwards would be reading a dead node.
    const int recordedAlpha = ours ? mine->second.alpha : 0;
    const bool styleSet = (exStyle & WS_EX_LAYERED) != 0;

    // The app cleared the style itself. The record goes with it, exactly as the
    // topmost claim goes when an app clears that bit: the mod does not spend the
    // session putting back a style that something else keeps removing. A settings
    // save is a fresh instruction and applies it again.
    bool oursNow = ours;
    if (ours && !styleSet) {
        g_opacityWindows.erase(mine);
        JournalOpacity(facts.exeName, ruleIndex, source, L"app-cleared", alpha);
        // And then carry on into the apply path in the same pass. A settings save
        // is a fresh instruction, so the level goes back on now rather than on the
        // next save; topmost already behaves that way, and a save that visibly
        // does nothing half the time is worse than no save at all.
        oursNow = false;
    }

    const OpacityDecision decision =
        DecideOpacity(styleSet, oursNow, recordedAlpha, alpha);

    if (decision == OpacityDecision::SkipAppLayered) {
        JournalOpacity(facts.exeName, ruleIndex, source, L"already-layered",
                       alpha);
        return;
    }

    if (!styleSet) {
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    }
    SetLayeredWindowAttributes(hwnd, 0, (BYTE)alpha, LWA_ALPHA);

    // An elevated window refuses both of those through UIPI. Recording it as the
    // mod's would mean claiming a change that was never made, and later
    // "withdrawing" it, which would be a lie in the journal.
    if (!(GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED)) {
        JournalOpacity(facts.exeName, ruleIndex, source, L"failed", alpha);
        return;
    }
    g_opacityWindows[hwnd] = OpacityRecord{alpha, facts.exeName};

    // No re-assert on every event, deliberately: if the application clears the
    // style, the record goes with it and nothing is put back until something asks
    // again. A settings save is something asking again, and the branch above
    // applies the level in that same pass.
    JournalOpacity(facts.exeName, ruleIndex, source,
                   decision == OpacityDecision::AlreadyThere ? L"already-there"
                                                             : L"applied",
                   alpha);
}

// The layering is the mod's, so taking it back is the mod's job too. Same
// principle as WithdrawTopmost: only a window in the record is touched, so an
// application that fades its own windows is never made opaque by this.
//
// Restoring means alpha 255 and the style gone, which is the state the window was
// in before the mod layered it: a window only enters the record on the branch
// where the style was clear.
static void WithdrawOpacity(HWND hwnd, const std::wstring& exeName,
                            int ruleIndex, PCWSTR source) {
    auto mine = g_opacityWindows.find(hwnd);
    if (mine == g_opacityWindows.end()) {
        return;
    }
    if (!IsWindow(hwnd)) {
        g_opacityWindows.erase(mine);
        return;
    }

    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_LAYERED)) {
        // The application cleared the style first. Nothing to undo, and nothing
        // left to own.
        g_opacityWindows.erase(mine);
        JournalOpacity(exeName, ruleIndex, source, L"app-cleared", 255);
        return;
    }

    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE,
                      GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);

    // Same rule as WithdrawTopmost: a change that was refused is not a change.
    // The record stays, so the next pass tries again, and the journal says
    // "failed" rather than claiming a window is opaque when it is not.
    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED) {
        JournalOpacity(exeName, ruleIndex, source, L"failed", 255);
        return;
    }

    g_opacityWindows.erase(mine);
    JournalOpacity(exeName, ruleIndex, source, L"withdrawn", 255);
}

// The opacity counterpart of ReleaseAllTopmost, called from the same places. A
// window left translucent after the mod is turned off is worse than a window
// left pinned: there is no obvious way for a user to fix it by hand.
static void ReleaseAllOpacity(PCWSTR source) {
    std::vector<std::pair<HWND, std::wstring>> windows;
    windows.reserve(g_opacityWindows.size());
    for (const auto& entry : g_opacityWindows) {
        windows.emplace_back(entry.first, entry.second.exeName);
    }
    for (const auto& entry : windows) {
        WithdrawOpacity(entry.first, entry.second, -1, source);
    }
    // WithdrawOpacity erases as it goes; this covers a window that was not a
    // window any more, which is erased above without a line.
    g_opacityWindows.clear();
}

// Which monitor a fullscreen app is covering right now, or -1. The foreground
// window is the only candidate: a fullscreen app is by definition in front.
//
// The monitor list is the cached one, which is rect-only data. That is deliberate
// for the foreground hook, which runs on every focus change: there is no device
// enumeration here, no identity resolution, and no walk of device interface paths.
// `exclude` is the window currently being applied to, and it must not be counted
// as the fullscreen app that gets in its own way. Without this, an application
// whose window opens fullscreen and is ruled always-on-top would see itself as
// the covering window, record a yield, and never pin itself until the user
// switched to something else and back.
static int CoveredMonitorOfForeground(HWND exclude) {
    if (g_monitors.empty()) {
        return -1;
    }
    HWND foreground = GetForegroundWindow();
    if (!foreground || foreground == g_messageWindow.load() ||
        foreground == exclude) {
        return -1;
    }
    if (IsIconic(foreground)) {
        return -1;
    }
    const wchar_t* reason = L"unknown";
    if (!PassesWindowFilters(foreground, &reason)) {
        return -1;
    }
    // ResolveWindowFacts is reused for its exclude list as much as its facts:
    // Explorer's Alt+Tab and Task View hosts are full-monitor surfaces, and
    // without this every Alt+Tab would demote and re-promote every pinned window.
    WindowFacts facts;
    if (!ResolveWindowFacts(foreground, facts, &reason)) {
        return -1;
    }
    RuleRect rect;
    if (!WindowRectOf(foreground, rect)) {
        return -1;
    }
    for (size_t i = 0; i < g_monitors.size(); i++) {
        if (IsFullscreenCandidate(rect, g_monitors[i].pixelRect,
                                  IsZoomed(foreground) != FALSE,
                                  kApplyTolerancePixels)) {
            return (int)i;
        }
    }
    return -1;
}

// The two bookkeeping steps every successful geometry apply performs. Factored
// out because a second success path was added (move and pin in one call) and the
// settle machinery must be armed identically from both.
static void AccountForApply(PCWSTR source) {
    if (source && wcscmp(source, L"reapply") == 0) {
        g_appliedSincePass++;
    }
}

static void RememberPendingApply(HWND hwnd, const RuleRect& target,
                                 int ruleIndex) {
    PendingApply pending;
    pending.target = target;
    pending.ruleIndex = ruleIndex;
    pending.startedTick = GetTickCount64();
    pending.lastMoveTick = pending.startedTick;
    g_pendingApplies[hwnd] = pending;
    EnsureSettleTimer();
}

// Applies one matched rule to one window. Called from the event path and from the
// settings-changed re-apply pass, so both routes behave identically -- which is
// what makes "save the settings and watch it happen" the same code as "open the
// app and watch it happen".
static void ApplyRuleToWindow(HWND hwnd, const WindowFacts& facts,
                              int ruleIndex, PCWSTR source) {
    if (ruleIndex < 0 || ruleIndex >= (int)g_ruleActions.size()) {
        return;
    }
    const RuleActions& actions = g_ruleActions[(size_t)ruleIndex];

    // Releasing a claim comes first, and before every early return below.
    //
    // This is not a detail of ordering. A claim is the mod's own property, and a
    // rule that stops asking for the top is a rule that no longer justifies it,
    // regardless of what else happens to this window. The first version did this
    // at the bottom of the function, which meant a window the user had dragged,
    // or one whose move was refused, or one with a monitor that resolved to
    // nothing, returned early and kept the bit for the rest of the session --
    // pinned, on top of everything, with the rule switched off. The journal had
    // no line for it, because the code never got there.
    const bool wantTopmost = actions.topmost == L"on";
    if (!wantTopmost) {
        WithdrawTopmost(hwnd, facts.exeName, ruleIndex, source);
    }

    // Same reasoning, same place: a rule that stops asking for a level gives back
    // the level it set, before any early return below can skip it. Deleting a
    // rule, or blanking its opacity, has to put the window back to opaque.
    if (actions.opacity.empty()) {
        WithdrawOpacity(hwnd, facts.exeName, ruleIndex, source);
    }

    RuleRect current;
    if (!WindowRectOf(hwnd, current)) {
        return;
    }

    // The monitor is what a position and a size are measured from, and nothing
    // else uses it, so it is resolved only when the rule asks for geometry. Doing
    // it unconditionally meant an opacity-only or always-on-top-only rule did
    // nothing at all when the window's monitor could not be resolved -- right
    // after a monitor was unplugged, for instance, while the cached list is a
    // moment out of date.
    //
    // A rule that names a monitor that is not there still moves nothing rather
    // than guessing, and the journal says MONITOR-SKIP.
    const bool wantsGeometry = !actions.position.empty() || !actions.size.empty();
    TargetPlan plan;
    plan.target = current;
    if (wantsGeometry) {
        const MonitorSpec spec = ParseMonitorSpec(actions.monitor);
        const MonitorChoice choice =
            ResolveRuleMonitor(g_monitors, spec, MonitorIndexForWindow(hwnd));
        if (!choice.usable) {
            AppendDiagnosticsJournal(L"MONITOR-SKIP exe=" + facts.exeName +
                                     L" rule=" + std::to_wstring(ruleIndex) +
                                     L" spec=\"" + actions.monitor +
                                     L"\" reason=\"" + choice.reason + L"\"");
        } else {
            plan = ComputeTargetPlan(current,
                                     g_monitors[(size_t)choice.index].pixelRect,
                                     actions.position, actions.size);
        }
    }

    const bool minimized = IsIconic(hwnd) != FALSE;
    const bool overridden = g_overriddenWindows.count(hwnd) != 0;
    const bool refused = g_unwritableWindows.count(hwnd) != 0;
    const bool alreadyThere =
        !plan.anything || RectsWithinTolerance(current, plan.target,
                                               kApplyTolerancePixels);

    const ApplyDecision decision =
        DecideApply(overridden, refused, minimized, alreadyThere);

    // The z-order half of the rule. Decided apart from the rectangle, because
    // "already where it belongs" and "already pinned" are different facts and
    // either one alone must stop that half from issuing a call -- otherwise every
    // settings save would restack every matched window.
    TopmostDecision topmost = TopmostDecision::NoChange;
    if (wantTopmost) {
        const bool isTopmost =
            (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
        const int covered = CoveredMonitorOfForeground(hwnd);
        topmost = DecideTopmost(true, isTopmost, overridden, refused, minimized,
                                covered >= 0 &&
                                    covered == MonitorIndexForWindow(hwnd));
    }

    // A refusal, an override or a minimized window stops the whole rule, not one
    // action of it, and both halves say so in the journal.
    const bool blocked = decision == ApplyDecision::Refused ||
                         decision == ApplyDecision::Overridden ||
                         decision == ApplyDecision::Minimized;
    if (blocked) {
        const PCWSTR result = decision == ApplyDecision::Refused
                                  ? L"refused"
                                  : decision == ApplyDecision::Overridden
                                        ? L"overridden"
                                        : L"minimized";
        if (wantTopmost) {
            JournalTopmost(facts.exeName, ruleIndex, source, result,
                           TopmostStateOf(hwnd));
        }
        if (plan.anything) {
            JournalApply(facts, ruleIndex, source, result, plan.target, current,
                         ERROR_SUCCESS);
        }
        // Blocked means blocked. Overridden is the user's own placement, refused
        // is a window that cannot be changed and minimized is a window with no
        // rectangle worth touching; fading any of them would be the mod doing
        // something to a window it just said it would leave alone. This used to
        // return only when the rule had geometry, so a rule with opacity but no
        // position still faded a window the user had dragged.
        return;
    }

    const bool move =
        !blocked && plan.anything && decision == ApplyDecision::Proceed;
    const bool claim = !blocked && topmost == TopmostDecision::Claim;

    if (move && claim) {
        // One call for both halves: two would restack the window twice.
        const DWORD error = ApplyWindowPos(hwnd, &plan.target, HWND_TOPMOST);
        if (error == ERROR_ACCESS_DENIED) {
            g_unwritableWindows.insert(hwnd);
            JournalApply(facts, ruleIndex, source, L"refused", plan.target,
                         current, error);
        } else {
            JournalApply(facts, ruleIndex, source,
                         error == ERROR_SUCCESS ? L"applied" : L"failed",
                         plan.target, current, error);
            if (error == ERROR_SUCCESS) {
                ClaimTopmost(hwnd, facts.exeName, ruleIndex, source);
                AccountForApply(source);
                RememberPendingApply(hwnd, plan.target, ruleIndex);
            } else {
                JournalTopmost(facts.exeName, ruleIndex, source, L"failed",
                               TopmostStateOf(hwnd));
            }
        }
    } else if (move) {
        const DWORD error = ApplyWindowPos(hwnd, &plan.target, nullptr);
        if (error == ERROR_ACCESS_DENIED) {
            // Elevated window: the move is refused, not retried, and this
            // window is remembered so the next event does not try again.
            g_unwritableWindows.insert(hwnd);
            JournalApply(facts, ruleIndex, source, L"refused", plan.target,
                         current, error);
        } else {
            JournalApply(facts, ruleIndex, source,
                         error == ERROR_SUCCESS ? L"applied" : L"failed",
                         plan.target, current, error);
            if (error == ERROR_SUCCESS) {
                AccountForApply(source);
                RememberPendingApply(hwnd, plan.target, ruleIndex);
            }
        }
    } else if (claim) {
        const DWORD error = ApplyWindowPos(hwnd, nullptr, HWND_TOPMOST);
        if (error == ERROR_ACCESS_DENIED) {
            g_unwritableWindows.insert(hwnd);
            JournalTopmost(facts.exeName, ruleIndex, source, L"refused",
                           TopmostStateOf(hwnd));
        } else {
            if (error == ERROR_SUCCESS) {
                ClaimTopmost(hwnd, facts.exeName, ruleIndex, source);
            } else {
                // The state is read back rather than assumed: a pin that failed is
                // not a pinned window, and the journal is the only place that
                // difference is visible.
                JournalTopmost(facts.exeName, ruleIndex, source, L"failed",
                               TopmostStateOf(hwnd));
            }
        }
    }

    if (!blocked) {
        if (plan.anything && decision == ApplyDecision::AlreadyThere) {
            JournalApply(facts, ruleIndex, source, L"already-there", plan.target,
                         current, ERROR_SUCCESS);
        }
        if (topmost == TopmostDecision::AlreadyThere) {
            JournalTopmost(facts.exeName, ruleIndex, source, L"already-there",
                           L"on");
        } else if (topmost == TopmostDecision::Yielded) {
            // Claimed but not pinned: a fullscreen app owns this monitor.
            // Recorded so the foreground hook raises it when that app is gone,
            // rather than leaving the rule to be reapplied by hand.
            g_topmostWindows[hwnd] = facts.exeName;
            g_yieldedWindows.insert(hwnd);
            JournalTopmost(facts.exeName, ruleIndex, source, L"yielded", L"off");
        }
    }

    if (!actions.opacity.empty()) {
        int alpha = 0;
        if (ParseOpacityValue(actions.opacity, alpha)) {
            ApplyOpacity(hwnd, alpha, facts, ruleIndex, source);
        } else {
            // The load journals an unreadable level but does not stop the rule,
            // so this is reachable. Giving the level back is the honest answer:
            // leaving the window at the previous level would be the mod applying
            // a rule it cannot read.
            WithdrawOpacity(hwnd, facts.exeName, ruleIndex, source);
        }
    }
}

// The settle poll: an application that repositions itself right after being moved
// would otherwise silently win, so the mod looks once more and re-applies once.
// After that the window is left alone -- the failure bias is "a window the app
// insists on placing stays where the app put it", never a fight.
static void ProcessSettleTick() {
    if (g_pendingApplies.empty()) {
        StopSettleTimerIfIdle();
        return;
    }

    std::vector<HWND> windows;
    windows.reserve(g_pendingApplies.size());
    for (const auto& entry : g_pendingApplies) {
        windows.push_back(entry.first);
    }

    for (HWND hwnd : windows) {
        auto pending = g_pendingApplies.find(hwnd);
        if (pending == g_pendingApplies.end()) {
            continue;
        }

        if (!IsWindow(hwnd)) {
            g_pendingApplies.erase(pending);
            continue;
        }

        // A user move or a refusal overtakes the settle poll; both are recorded
        // where they happen, so the poll simply drops the record.
        if (g_overriddenWindows.count(hwnd) || g_unwritableWindows.count(hwnd)) {
            g_pendingApplies.erase(pending);
            continue;
        }

        RuleRect now;
        if (!WindowRectOf(hwnd, now)) {
            g_pendingApplies.erase(pending);
            continue;
        }

        const unsigned long long nowTick = GetTickCount64();
        const bool atTarget =
            RectsWithinTolerance(now, pending->second.target,
                                 kApplyTolerancePixels);
        const bool withinGrace =
            nowTick - pending->second.lastMoveTick <= kSettleGraceMs;
        const bool timedOut =
            nowTick - pending->second.startedTick > kSettleWindowMs;

        switch (DecideSettle(atTarget, pending->second.retriesLeft, withinGrace,
                             timedOut)) {
            case SettleDecision::Done:
                AppendDiagnosticsJournal(
                    L"SETTLE state=ok rule=" +
                    std::to_wstring(pending->second.ruleIndex) +
                    L" rect=(" + std::to_wstring(now.left) + L"," +
                    std::to_wstring(now.top) + L"," +
                    std::to_wstring(now.right) + L"," +
                    std::to_wstring(now.bottom) + L")");
                g_pendingApplies.erase(pending);
                break;

            case SettleDecision::Retry: {
                // Copied before the move: the erase below frees the node, and the
                // journal line after it used to read ruleIndex straight out of
                // the freed entry. The string is built whether or not the journal
                // is on, so this was a use-after-free on every refused retry.
                const int ruleIndex = pending->second.ruleIndex;
                const DWORD error =
                    ApplyWindowPos(hwnd, &pending->second.target, nullptr);
                pending->second.retriesLeft--;
                // The retry is a fresh move, so it gets its own grace period: the
                // poll must not read the retry's own latency as another failure.
                pending->second.lastMoveTick = GetTickCount64();
                if (error == ERROR_ACCESS_DENIED) {
                    g_unwritableWindows.insert(hwnd);
                    g_pendingApplies.erase(pending);
                }
                AppendDiagnosticsJournal(L"SETTLE state=retry rule=" +
                                         std::to_wstring(ruleIndex) + L" error=" +
                                         std::to_wstring((unsigned long)error));
                break;
            }

            case SettleDecision::Wait:
                break;

            case SettleDecision::GiveUp:
                AppendDiagnosticsJournal(
                    L"SETTLE state=given-up rule=" +
                    std::to_wstring(pending->second.ruleIndex) +
                    L" rect=(" + std::to_wstring(now.left) + L"," +
                    std::to_wstring(now.top) + L"," +
                    std::to_wstring(now.right) + L"," +
                    std::to_wstring(now.bottom) + L")");
                g_pendingApplies.erase(pending);
                break;
        }
    }

    StopSettleTimerIfIdle();
}

// --- Re-applying after a settings change ------------------------------------

// This mod has no allowlist, so "the windows the mod knows about" is not a set
// it maintains -- it evaluates whatever appears in front of it. A settings change
// therefore has to sweep the desktop, or a rule edited while its app is already
// open would appear to do nothing until the app was restarted.
struct ReapplyStats {
    int considered = 0;
    int applied = 0;
};

static BOOL CALLBACK ReapplyEnumProc(HWND hwnd, LPARAM data) {
    auto* stats = reinterpret_cast<ReapplyStats*>(data);
    stats->considered++;
    // The same evaluation path as an event uses, forced past the "already
    // decided" bookkeeping: the rules have changed, so every window is due a
    // fresh decision. Windows in the override set are skipped inside the apply
    // path itself, which is where that promise belongs.
    EvaluateWindow(hwnd, /*forced=*/true, L"reapply");
    return TRUE;
}

static void RunReapplyPass() {
    ReapplyStats stats;
    g_appliedSincePass = 0;
    EnumWindows(ReapplyEnumProc, reinterpret_cast<LPARAM>(&stats));
    AppendDiagnosticsJournal(L"REAPPLY-PASS windows=" +
                             std::to_wstring(stats.considered) + L" applied=" +
                             std::to_wstring(g_appliedSincePass));
}

// --- Rule evaluation --------------------------------------------------------

// Windows whose fate has already been decided, so that the CREATE and the SHOW
// event for the same window do not each produce a journal line. This is
// bookkeeping for the log, not state the matching depends on: matching is a
// pure function of the window, as FindFirstMatchingRule is careful to stay.
//
// It is filled in only once a window has actually been decided -- matched, or
// evaluated with its title present -- and never when a filter rejected it. An
// early "already evaluated" mark for an unshown window is not a cosmetic bug:
// it silently discards the SHOW event the mod was written for.
static std::unordered_set<HWND> g_evaluatedWindows;

static void EvaluateWindow(HWND hwnd, bool forced, PCWSTR source) {
    // The re-apply pass forces every window past this bookkeeping, because the
    // rules themselves have changed and so has every window's answer.
    // A window that was matched while minimized stays un-decided, so the event
    // raised when it is restored gets it its rule.
    if (!forced && g_evaluatedWindows.count(hwnd) &&
        !g_deferredWindows.count(hwnd)) {
        return;
    }

    const wchar_t* reason = L"unknown";
    WindowFacts facts;

    // A rejection here does NOT decide the window's fate, and must not mark it
    // as decided. Almost every window is created hidden, at zero size, or before
    // it is laid out, and passes these same filters a moment later when it is
    // shown.
    if (!PassesWindowFilters(hwnd, &reason)) {
        if (g_diagnosticsLog) {
            JournalFilterSkip(hwnd, reason);
        }
        return;
    }

    if (!ResolveWindowFacts(hwnd, facts, &reason)) {
        if (g_diagnosticsLog) {
            JournalFilterSkip(hwnd, reason);
        }
        return;
    }

    if (g_rules.empty()) {
        return;
    }

    MatchOutcome outcome = FindFirstMatchingRule(
        g_rules, g_compiledRules, facts.exePath, facts.exeName,
        facts.windowClass, facts.title);

    if (outcome.matched) {
        JournalRuleHit(outcome.ruleIndex, outcome, facts.exeName);
        g_governedWindows.insert(hwnd);
        if (IsIconic(hwnd)) {
            g_deferredWindows.insert(hwnd);
        } else {
            g_deferredWindows.erase(hwnd);
            g_evaluatedWindows.insert(hwnd);
        }
        ApplyRuleToWindow(hwnd, facts, outcome.ruleIndex, source);
        return;
    }

    JournalNoMatch(facts.exeName, facts.title);

    // A rule that no longer matches this window is a rule that no longer asks
    // for it to be on top, or for it to be translucent, so anything the mod set
    // goes back here too.
    WithdrawTopmost(hwnd, facts.exeName, -1, source);
    WithdrawOpacity(hwnd, facts.exeName, -1, source);
    g_governedWindows.erase(hwnd);
    // A window that was waiting to be restored and now matches no rule has
    // nothing left to wait for. Left in the set it would be re-evaluated on every
    // event for the rest of the session, which is work and journal lines about a
    // decision that has already been made.
    g_deferredWindows.erase(hwnd);

    // A window with no title yet is not decided either: the title is the last of
    // its facts to settle, so a rule that matches on the title would have missed
    // its only chance.
    if (!facts.title.empty()) {
        g_evaluatedWindows.insert(hwnd);
    }
}

// --- Foreground changes -----------------------------------------------------

// The only thing the foreground is used for: a fullscreen app taking a monitor is
// a reason to lower the windows this mod pinned there, and giving it back is a
// reason to raise them again. Nothing here re-asserts the top on an ordinary
// focus change, because the topmost bit does not need it -- see the note on
// DecideTopmost.
static void OnForegroundChanged() {
    // Nothing claimed, nothing to do. A user with no always-on-top rule pays one
    // lookup per focus change and no process, window or monitor work at all.
    if (g_topmostWindows.empty() && g_yieldedWindows.empty()) {
        return;
    }

    const int covered = CoveredMonitorOfForeground(nullptr);

    std::vector<HWND> candidates;
    candidates.reserve(g_topmostWindows.size() + g_yieldedWindows.size());
    for (const auto& entry : g_topmostWindows) {
        candidates.push_back(entry.first);
    }
    for (HWND hwnd : g_yieldedWindows) {
        candidates.push_back(hwnd);
    }

    for (HWND hwnd : candidates) {
        auto claim = g_topmostWindows.find(hwnd);
        const bool claimed = claim != g_topmostWindows.end();
        const bool yielded = g_yieldedWindows.count(hwnd) != 0;
        const std::wstring exeName =
            claimed ? claim->second : std::wstring(L"(unknown)");

        if (!IsWindow(hwnd)) {
            g_topmostWindows.erase(hwnd);
            g_yieldedWindows.erase(hwnd);
            continue;
        }

        const bool bitSet =
            (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
        switch (DecideYield(claimed, yielded, bitSet,
                            g_overriddenWindows.count(hwnd) != 0,
                            MonitorIndexForWindow(hwnd), covered)) {
            case YieldDecision::Demote: {
                const DWORD error = ApplyWindowPos(hwnd, nullptr, HWND_NOTOPMOST);
                if (error == ERROR_ACCESS_DENIED) {
                    // An elevated window: this process may not change it at all.
                    // Remembered so the mod stops trying on every focus change,
                    // and the journal says why it stayed up.
                    g_unwritableWindows.insert(hwnd);
                } else if (error == ERROR_SUCCESS) {
                    g_yieldedWindows.insert(hwnd);
                }
                JournalTopmost(exeName, -1, L"foreground",
                               error == ERROR_SUCCESS ? L"yielded" : L"failed",
                               TopmostStateOf(hwnd));
                break;
            }
            case YieldDecision::AppCleared:
                // The app cleared the bit itself. The claim goes with it: this is
                // deliberately not a reason to pin the window again.
                g_topmostWindows.erase(hwnd);
                g_yieldedWindows.erase(hwnd);
                JournalTopmost(exeName, -1, L"foreground", L"app-cleared", L"off");
                break;
            case YieldDecision::Reclaim: {
                const DWORD error = ApplyWindowPos(hwnd, nullptr, HWND_TOPMOST);
                if (error == ERROR_SUCCESS) {
                    g_yieldedWindows.erase(hwnd);
                } else if (error == ERROR_ACCESS_DENIED) {
                    g_unwritableWindows.insert(hwnd);
                    g_yieldedWindows.erase(hwnd);
                }
                JournalTopmost(exeName, -1, L"foreground",
                               error == ERROR_SUCCESS ? L"reclaimed" : L"failed",
                               TopmostStateOf(hwnd));
                break;
            }
            case YieldDecision::Nothing:
                break;
        }
    }
}

// --- Event handling ---------------------------------------------------------

static void CALLBACK WinEventProc(HWINEVENTHOOK /*hook*/, DWORD event,
                                  HWND hwnd, LONG idObject, LONG idChild,
                                  DWORD /*eventThread*/,
                                  DWORD /*eventTime*/) {
    // Foreground changes first, and before the window-object filter below: the
    // event arrives for the new foreground window, but the window this path
    // cares about is read from GetForegroundWindow, so nothing here depends on
    // the callback's handle.
    if (event == EVENT_SYSTEM_FOREGROUND) {
        OnForegroundChanged();
        return;
    }

    // Window-level events only. Non-window objects (carets, menus, list items)
    // also produce CREATE/SHOW/DESTROY events.
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    if (event == EVENT_OBJECT_DESTROY) {
        g_evaluatedWindows.erase(hwnd);
        g_pendingApplies.erase(hwnd);
        g_overriddenWindows.erase(hwnd);
        g_unwritableWindows.erase(hwnd);
        g_topmostWindows.erase(hwnd);
        g_yieldedWindows.erase(hwnd);
        g_opacityWindows.erase(hwnd);
        g_governedWindows.erase(hwnd);
        g_deferredWindows.erase(hwnd);
        return;
    }

    if (event == EVENT_SYSTEM_MINIMIZEEND) {
        // The window is back on the screen. A rule that matched it while it was
        // minimized was deferred rather than dropped, and this is the event that
        // gives it its chance: restoring a minimized window raises MINIMIZEEND,
        // not OBJECT_SHOW, so relying on SHOW alone left an app that starts in the
        // tray with no rule at all.
        if (g_deferredWindows.count(hwnd)) {
            EvaluateWindow(hwnd, /*forced=*/true, L"restored");
        }
        return;
    }

    if (event == EVENT_SYSTEM_MOVESIZEEND) {
        // The user has just placed this window by hand. Their intent outranks the
        // rule for the rest of the session -- the project's first principle, and
        // the same override the geometry-memory mod applies.
        //
        // Only a window a rule actually governs is recorded. g_evaluatedWindows
        // also holds windows that matched nothing, so using it here meant
        // dragging any window at all marked it, and a rule added for that app
        // later in the session was silently blocked.
        if (g_governedWindows.count(hwnd) || g_pendingApplies.count(hwnd)) {
            g_pendingApplies.erase(hwnd);
            if (g_overriddenWindows.insert(hwnd).second) {
                AppendDiagnosticsJournal(
                    L"OVERRIDE reason=user-moved (this window is left alone for "
                    L"the rest of the session)");
            }
        }
        return;
    }

    EvaluateWindow(hwnd, /*forced=*/false, L"window-event");
}

// --- Event thread -----------------------------------------------------------

static HWINEVENTHOOK g_hookWindows = nullptr;
static HWINEVENTHOOK g_hookMoveSizeEnd = nullptr;
static HWINEVENTHOOK g_hookMinimizeEnd = nullptr;
static HWINEVENTHOOK g_hookForeground = nullptr;
static std::atomic<DWORD> g_eventThreadId{0};
static HANDLE g_eventThread = nullptr;

static void InstallHooks() {
    if (g_hookWindows && g_hookMoveSizeEnd && g_hookMinimizeEnd &&
        g_hookForeground) {
        return;
    }

    // Each hook is retried independently, so a failure of one does not stop the
    // other from being installed on a later reload.
    if (!g_hookWindows) {
        // EVENT_OBJECT_CREATE (0x8000), EVENT_OBJECT_DESTROY (0x8001) and
        // EVENT_OBJECT_SHOW (0x8002) in one hook. EVENT_OBJECT_LOCATIONCHANGE is
        // deliberately not registered: it is a storm of events, and a rules
        // engine has nothing to learn from a window being dragged.
        g_hookWindows = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW,
                                        nullptr, WinEventProc, 0, 0,
                                        WINEVENT_OUTOFCONTEXT);
    }
    if (!g_hookMoveSizeEnd) {
        // The only way to know the user moved a window themselves: a
        // SetWindowPos does not raise this event, so everything this hook
        // reports is real user input, which is exactly what the override set
        // needs to be trustworthy.
        g_hookMoveSizeEnd =
            SetWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND,
                            nullptr, WinEventProc, 0, 0,
                            WINEVENT_OUTOFCONTEXT);
    }
    if (!g_hookMinimizeEnd) {
        // Restoring a minimized window raises MINIMIZEEND, not OBJECT_SHOW, so a
        // window whose rule was deferred while it was minimized needs this to get
        // its chance. Without it, an app that starts in the tray never gets its
        // rule at all, which is the case the deferral exists for.
        g_hookMinimizeEnd =
            SetWinEventHook(EVENT_SYSTEM_MINIMIZEEND, EVENT_SYSTEM_MINIMIZEEND,
                            nullptr, WinEventProc, 0, 0,
                            WINEVENT_OUTOFCONTEXT);
    }
    if (!g_hookForeground) {
        // One event, not a range, and only for the yield path: a fullscreen app
        // taking a monitor, and giving it back. The callback returns immediately
        // when no window is claimed, so a user with no always-on-top rule pays a
        // set lookup per focus change and nothing else.
        g_hookForeground =
            SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                            nullptr, WinEventProc, 0, 0,
                            WINEVENT_OUTOFCONTEXT);
    }

    if (!g_hookWindows || !g_hookMoveSizeEnd || !g_hookMinimizeEnd ||
        !g_hookForeground) {
        Wh_Log(L"SetWinEventHook failed (last error %lu)", GetLastError());
    } else {
        Wh_Log(L"window event hooks installed");
    }
}

static void UninstallHooks() {
    if (g_hookWindows) {
        UnhookWinEvent(g_hookWindows);
        g_hookWindows = nullptr;
    }
    if (g_hookMoveSizeEnd) {
        UnhookWinEvent(g_hookMoveSizeEnd);
        g_hookMoveSizeEnd = nullptr;
    }
    if (g_hookMinimizeEnd) {
        UnhookWinEvent(g_hookMinimizeEnd);
        g_hookMinimizeEnd = nullptr;
    }
    if (g_hookForeground) {
        UnhookWinEvent(g_hookForeground);
        g_hookForeground = nullptr;
    }
}

// Event-thread only. Re-reads everything the mod needs from the settings.
static void ApplySettingsOnEventThread(bool initial) {
    if (!initial) {
        Wh_Log(L"settings changed; reloading rules");
    }

    RefreshDiagnosticsSetting();

    // No master switch, deliberately. Windhawk already has one, and a second one
    // inside the mod means two places to look when nothing happens. Turning the
    // mod off in Windhawk unloads it, which runs the shutdown path and releases
    // every claim this mod made.

    // Before the rules, because a rule that names a monitor cannot be resolved
    // without them.
    RefreshMonitorViews();

    // A pending retry belongs to the rule as it was before this save. It is
    // cleared here, before any branch below can return: a save that deletes the
    // last rule returns early, and a retry left over from that rule would still
    // put the window back at its old target up to 800 ms later.
    g_pendingApplies.clear();

    LoadRules();
    JournalRulesLoaded();
    InstallHooks();

    if (g_rules.empty()) {
        // No rules means no rule asks for the top any more. The sweep below is
        // skipped in this branch, so the release has to happen here or a deleted
        // rule would leave its windows pinned until they were closed.
        ReleaseAllTopmost(L"no-rules");
        ReleaseAllOpacity(L"no-rules");
        Wh_Log(L"no rules configured; nothing will be matched. A rule "
               L"entry with every field blank ends the list of rules, and the "
               L"settings template ships exactly that entry -- fill it in to "
               L"configure your first rule.");
        AppendDiagnosticsJournal(L"REAPPLY-PASS windows=0 applied=0");
        return;
    }

    Wh_Log(L"%d rule(s) loaded; %d monitor(s) visible",
           (int)g_rules.size(), (int)g_monitors.size());

    // A settings change re-applies to what is already open -- "save the rule,
    // watch it happen" is the expectation a rules engine sets, and requiring a
    // restart to see an edit would be a surprise. NOT at startup, though: turning
    // the mod on must not move windows the user did not ask about, which is the
    // same call the geometry-memory mod makes when it is enabled with windows
    // already open.
    if (initial) {
        AppendDiagnosticsJournal(L"REAPPLY-PASS windows=0 applied=0 skipped=initial");
        return;
    }

    RunReapplyPass();
}

static LRESULT CALLBACK MessageWndProc(HWND hwnd, UINT message, WPARAM wParam,
                                       LPARAM lParam) {
    switch (message) {
        case kMsgReloadSettings:
            ApplySettingsOnEventThread(/*initial=*/false);
            return 0;

        case WM_TIMER:
            if (wParam == kSettleTimerId) {
                ProcessSettleTick();
                return 0;
            }
            break;

        case WM_DISPLAYCHANGE:
            // A rearrangement changes what "monitor 1" and "primary" mean, so the
            // cache is refreshed. Nothing is moved here: a display change is not a
            // reason to re-apply a rule to windows the user did not touch, and
            // the geometry-memory mod owns the "a monitor went away" behaviour for
            // its own remembered geometry.
            RefreshMonitorViews();
            // A pending retry aims at coordinates that may now belong to a
            // different monitor or to none: dropping it is the only answer that
            // cannot put a window somewhere nobody asked for.
            g_pendingApplies.clear();
            AppendDiagnosticsJournal(
                L"DISPLAYCHANGE monitors=" + std::to_wstring(g_monitors.size()));
            return 0;

        case kMsgShutdown:
            ReleaseAllTopmost(L"shutdown");
            ReleaseAllOpacity(L"shutdown");
            UninstallHooks();
            if (g_settleTimer) {
                KillTimer(hwnd, kSettleTimerId);
                g_settleTimer = 0;
            }
            g_rules.clear();
            g_compiledRules.clear();
            g_ruleActions.clear();
            g_evaluatedWindows.clear();
            g_pendingApplies.clear();
            g_overriddenWindows.clear();
            g_unwritableWindows.clear();
            g_topmostWindows.clear();
            g_yieldedWindows.clear();
            g_opacityWindows.clear();
            g_governedWindows.clear();
            g_deferredWindows.clear();
            g_monitors.clear();
            g_noMatchCounts.clear();
            g_filterSkipCounts.clear();
            g_messageWindow.store(nullptr);
            DestroyWindow(hwnd);
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            // Ignored on purpose. The message window is internal, but it is a
            // top-level window, so a "close all windows" tool can send this. Letting
            // it through would end the event thread, and with it the hooks, the
            // reloads and the release of everything this mod did to other windows;
            // the mod would be off for the rest of the session with no way back.
            // Only kMsgShutdown tears this window down.
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static DWORD WINAPI EventThreadProc(LPVOID /*parameter*/) {
    g_eventThreadId.store(GetCurrentThreadId());

    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = MessageWndProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = kMessageWindowClassName;

    if (!RegisterClassExW(&windowClass)) {
        // Fatal, including ERROR_CLASS_ALREADY_EXISTS. This class lives in the
        // tool process this mod owns, is unregistered when the event thread ends
        // and the process exits with the mod, so its existence can only mean the
        // name belongs to somebody else -- and the procedure a foreign class
        // carries is not MessageWndProc. A window created over it would never
        // receive this mod's messages, so there is nothing to salvage by
        // continuing: the mod reports why and does not start.
        Wh_Log(L"RegisterClassExW failed (last error %lu)", GetLastError());
        return 1;
    }

    HWND hwnd = CreateWindowExW(0, kMessageWindowClassName, L"", WS_POPUP, 0, 0,
                                0, 0, nullptr, nullptr, instance, nullptr);
    if (!hwnd) {
        Wh_Log(L"failed to create the message window (last error %lu)",
               GetLastError());
        UnregisterClassW(kMessageWindowClassName, instance);
        return 1;
    }

    g_messageWindow.store(hwnd);

    RefreshDiagnosticsSetting();  // must precede the load: the load journals
    AppendDiagnosticsJournal(std::wstring(L"MOD build=") + kJournalBuild);
    ApplySettingsOnEventThread(/*initial=*/true);

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    UnregisterClassW(kMessageWindowClassName, instance);
    g_eventThreadId.store(0);
    return 0;
}

// --- Windhawk entry points --------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"init");

    g_eventThread = CreateThread(nullptr, 0, EventThreadProc, nullptr, 0,
                                 nullptr);
    if (!g_eventThread) {
        Wh_Log(L"failed to create the event thread (last error %lu)",
               GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    // Settings are re-read on the event thread so that all mod state stays
    // owned by one thread. If the window does not exist yet, the thread reads
    // the current settings when it starts.
    HWND messageWindow = g_messageWindow.load();
    if (messageWindow) {
        PostMessageW(messageWindow, kMsgReloadSettings, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"uninit");

    HWND messageWindow = g_messageWindow.load();
    DWORD eventThreadId = g_eventThreadId.load();

    if (messageWindow) {
        PostMessageW(messageWindow, kMsgShutdown, 0, 0);
    } else if (eventThreadId) {
        // The window was never created, e.g. RegisterClassExW failed.
        PostThreadMessageW(eventThreadId, WM_QUIT, 0, 0);
    }

    if (g_eventThread) {
        if (WaitForSingleObject(g_eventThread, 5000) != WAIT_OBJECT_0) {
            Wh_Log(L"the event thread did not exit in time");
        }
        CloseHandle(g_eventThread);
        g_eventThread = nullptr;
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
