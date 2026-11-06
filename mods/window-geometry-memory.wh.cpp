// ==WindhawkMod==
// @id              window-geometry-memory
// @name            Where I Left It
// @description     Remembers each app's window position and size, restores it when the app opens again, and keeps open windows reachable when your display setup changes
// @version         1.2
// @author          metanoid18
// @github          https://github.com/Metanoid18
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -luser32 -ldwmapi -ladvapi32
// ==/WindhawkMod==

// Source code is published under the MIT license.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Where I Left It

Remembers where you left each app's window -- its position, its size, and which
monitor it was on -- and puts it back when the app opens again. It also keeps
windows that are already open reachable when your display setup changes: a
monitor going away, a different resolution, docking or undocking, or a
rotation.


## Getting started

The mod does nothing until you tell it which apps to manage. Open the mod's
settings, add an executable name (for example `notepad.exe`) or a full path to
**Apps to manage**, and save. Only windows belonging to the executables in that
list are remembered or moved.

## How it behaves

* Geometry is captured when you move or resize a window, and when a window
  closes. Move a window where you want it, close it, and it opens there next
  time -- in the same session or after a reboot.
* Maximized and minimized windows are handled as you would expect: a window that
  was maximized comes back maximized, and one that was minimized comes back where
  it was, at its normal size, or maximized if it was maximized before it was
  minimized.
* Saving is per monitor arrangement. A window is restored to the same monitor it
  was on, and after docking or undocking it is mapped to the closest equivalent
  monitor instead of being left off-screen.
* **Windows that are already open are moved too.** When the display arrangement
  changes -- a different resolution, docking, a monitor waking up or going away
  -- open tracked windows are mapped from the monitor they were on to the closest
  equivalent monitor that still exists, keeping the same fraction of that
  monitor. Nothing is left stranded on a monitor that is no longer there.
* Moving or resizing a window always wins over anything the mod was doing. If an
  application insists on its own placement, the mod tries once more and then
  stops fighting it.
* Each app's saved geometry can remember its position and its size separately,
  per app, in the settings. With **Restore size** off, a window is placed but not
  resized, so an app that sizes itself to its content keeps that size; with
  **Restore position** off, it is sized where it opened. Both are still recorded
  while off, so switching a box back on brings back the last position or size
  that window had rather than a value frozen at the moment it was switched off.

## Out of scope in v1

* Moving windows between virtual desktops
* Z-order (stacking order) restore
* Re-scaling what is *inside* a window when the scale percentage changes. The
  frame is moved and sized; how an app redraws its contents is up to the app.
* A periodic check for monitor changes. The mod reacts to Windows' display-change
  notification and is silent the rest of the time.
* UWP / Store apps that restore their own window placement

## Known limitations

* **Elevated windows are not moved.** Windows cannot be repositioned across
  integrity levels: the move is refused with access denied, the mod detects the
  refusal, leaves that window alone for the rest of the session, and records why.
  The window is still tracked, so it is not ignored -- the mod tries. While a
  refusal stands, that window's position is not saved either: your chosen
  position for it is not overwritten by wherever it happened to be sitting.
* **Titles are part of how a window is matched.** A window whose title changed
  between sessions falls back to matching by executable, class and ordinal, and
  is only restored when that match is unambiguous. Windows with identical titles
  may be restored to each other's positions.
* **A save at close can lose its turn.** The rectangle is read from the window as
  the window is closing, and Windows sometimes has the window out of the way first.
  When that happens nothing is recorded for that close, the diagnostics journal says
  so with a reason, and the window keeps whatever position it was last remembered
  at. Moving the window once by hand records it there and then, and that save is not
  a last-moment read, so this is about windows that are moved by something other than
  you and closed immediately after.
* **A window that was minimized when it closed comes back where it was**, at its
  normal size, or maximized if it was maximized before it was minimized. It does
  not come back minimized.
* **Stored resolution is not rescaled when DPI changes.** Nothing multiplies a
  stored rectangle by a DPI ratio. After a display change a window is mapped by
  the rules above, which already express it as a fraction of its monitor -- so a
  change of scale percentage on its own leaves windows where they are, because no
  monitor has moved.
* **A window is measured against the screen, not against the taskbar.** The
  taskbar is taller in pixels at a larger scale, so a window restored after a
  change of scale percentage can sit partly behind it. Move the window once and
  the new position is what is remembered.
* **A maximized window is left where Windows puts it.** While its monitor still
  exists, its maximized state and placement stay Windows' own business -- the mod
  does not move it, and it is not resized by a resolution change. If the monitor
  it was maximized on disappears, it is rescued onto a remaining monitor like any
  other window.
* **UWP and Store apps** often restore their own placement and may ignore the
  mod.
* If an application moves its window right after showing, the mod re-applies the
  stored geometry once. Applications that keep overriding it are left alone.

## Diagnostics

Turn on **Write a diagnostics journal** in the settings and the mod appends a
line to `window-geometry-journal.txt` for every save, restore and decision, with
the exact rectangle and monitor involved. It is the fastest way to see why a
particular window was or was not restored, and it is worth attaching to a bug
report. It is off by default.

Both files live in a folder named after the signed-in user's SID, inside the mod's
storage folder. That storage folder is shared by the whole machine and a tool mod
runs one process per signed-in session, so the folder is created with a protected
access control list that gives full access to that user, SYSTEM and
administrators, and to nobody else: two users signed in at once neither overwrite
each other's saved geometry nor can read each other's window titles. If the mod
cannot read the user's SID it saves nothing at all -- sharing the data would be
worse than forgetting it. The journal is a diagnostics aid rather than a history,
so it is capped: once it passes a megabyte it starts over from empty.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- allowlist:
  - - Exe: ""
      $name: Executable
      $description: >-
        Executable file name (for example notepad.exe) or a full path to the
        executable. Entries are read in order until the first one with nothing in
        it, so keep them at the top of the list. Apps that are not listed here
        are never tracked or moved.
    - RestorePosition: true
      $name: Restore position
      $description: >-
        Remember and restore this app's window position and monitor. Turn it off
        and the window is still sized, but stays wherever it opens; its position
        is still recorded, so turning it back on restores the last one it had.
    - RestoreSize: true
      $name: Restore size
      $description: >-
        Remember and restore this app's window size. Turn it off and the window
        is still placed, but keeps the size it opens with; its size is still
        recorded, so turning it back on restores the last one it had.
  $name: Apps to manage
  $description: >-
    Opt-in list. Only windows belonging to the executables listed here are
    remembered and restored. The default template entry is intentionally empty.
- diagnosticsLog: false
  $name: Write a diagnostics journal
  $description: >-
    Appends a line to window-geometry-journal.txt in this user's own folder
    inside the mod's storage folder, for every save, restore and decision, with
    the exact rectangle and monitor involved. Intended for validating behaviour
    and for bug reports: the stored geometry file is written lazily and holds
    every tracked app for this user. Off by default.
- clearStoredData: none
  $name: Clear stored data
  $description: >-
    Choose "Clear stored data now" and save the settings to wipe the whole
    stored geometry database. The action runs once when you select it, and
    re-saving the settings afterwards does not wipe again. To run it a second
    time, set this to "Do nothing", save, then select it again.
  $options:
  - none: Do nothing
  - clearNow: Clear stored data now
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <windhawk_api.h>

// swprintf_s and wcscmp below are CRT functions; windows.h does not declare
// them, so the include is required rather than incidental.
#include <cwchar>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <ctime>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// What it does, and where the edges are:
//
// * Saves a window's rectangle and monitor when it is closed or dragged, and
//   restores them the next time a window of the same app appears, matched on
//   process path, window class and a normalized title, with an ordinal for
//   windows whose title changed.
// * Reacts to a display change by relaying out the windows that are already
//   open, through the same normalized mapping and the same apply path as the
//   restore.
// * The only windows it ever moves are the ones it is restoring, at most twice
//   per show (see the settle state machine below). User input wins
//   unconditionally: a MOVESIZEEND drops any restore still in flight before the
//   position is recorded. A display change moves only windows the user has not
//   already moved by hand, and never a minimized one; a maximized window is
//   Windows' own business until its monitor disappears.
// * Stored geometry lives in one JSON file inside this user's folder under the
//   mod storage path, written atomically and debounced. The identity cache, the
//   ordinal sets and the session override set are in-memory only.
// * A dedicated event thread with its own message loop is required: with
//   WINEVENT_OUTOFCONTEXT, the system delivers events to the installing
//   thread's message queue, so a thread that never pumps never receives
//   anything.
// * Elevation/UIPI refusals are detected and remembered rather than retried: a
//   window whose move was refused is also not saved, so a position the mod could
//   not apply never becomes the position the next session opens at.

// --- Settings ---------------------------------------------------------------

// The geometry store inside this user's folder. Defined once here so the clear
// action and the store code cannot drift apart.
static constexpr PCWSTR kGeometryStoreFileName = L"window-geometry.json";
static constexpr PCWSTR kDiagnosticsJournalFileName =
    L"window-geometry-journal.txt";


// Defined further down, with the other file I/O. Declared here because the
// window-filtering and skip-logging code above it also journals its decisions.
static void AppendDiagnosticsJournal(const std::wstring& line);

// Defined with the clear action at the bottom of the file, called from both the
// message handler and the startup path.
static void ClearStoredFile();

// Mod storage key latching the last handled "Clear stored data" value.
// See ApplyClearStoredDataAction for why this is needed.
static constexpr PCWSTR kClearLatchValueName = L"clearStoredDataLatch";
static constexpr int kClearLatchNone = 0;
static constexpr int kClearLatchHandled = 1;

// --- Event plumbing --------------------------------------------------------

// Hidden top-level window used to receive the WM_DISPLAYCHANGE broadcast.
static constexpr PCWSTR kMessageWindowClassName =
    L"WindhawkWhereILeftItMessageWnd";

// Posted from Windhawk's threads, handled on the event thread so that all mod
// state is owned by one thread and needs no locking.
static constexpr UINT kMsgReloadSettings = WM_APP + 1;
static constexpr UINT kMsgShutdown = WM_APP + 2;
static constexpr UINT kMsgClearStore = WM_APP + 3;


// Processes whose windows are never tracked, checked before the allowlist.
static constexpr PCWSTR kExcludedProcesses[] = {
    L"windhawk.exe",
    L"explorer.exe",
    L"dwm.exe",
    L"ShellExperienceHost.exe",
};

// --- Window identity --------------------------------------------------------

// >>> WINDOW_IDENTITY_CORE >>>
// Pure string handling: the title normalization cases are unit-tested off Windows.

static bool IsDecimalDigit(wchar_t c) {
    return c >= L'0' && c <= L'9';
}

// Normalizes a window title into the volatile-bits-removed form used for the
// identity key and for logging. Deliberately three narrow rules instead of a
// general parser, so that every removal is explainable:
//
//   Rule 1: drop parenthesised digit groups — "Inbox (3)" -> "Inbox". Counts and
//           badges are volatile. Groups without digits are kept, because
//           "(Final)" carries meaning.
//   Rule 2: drop '*' modified markers at the start/end of the title or directly
//           after a path separator — "*document.txt" -> "document.txt". A '*'
//           in the middle of text is left alone, since it may be literal.
//   Rule 3: collapse whitespace runs and trim. Leftovers from rules 1-2 should
//           not create phantom key differences.
static std::wstring NormalizeWindowTitle(const std::wstring& title) {
    std::wstring out;
    out.reserve(title.size());

    for (size_t i = 0; i < title.size();) {
        wchar_t c = title[i];

        if (c == L'(') {
            size_t j = i + 1;
            bool groupIsDigitsOnly = true;
            bool sawDigit = false;
            while (j < title.size() && title[j] != L')') {
                if (IsDecimalDigit(title[j])) {
                    sawDigit = true;
                } else if (title[j] != L' ') {
                    groupIsDigitsOnly = false;
                }
                j++;
            }
            if (j < title.size() && title[j] == L')' && groupIsDigitsOnly &&
                sawDigit) {
                i = j + 1;
                continue;
            }
        }

        if (c == L'*') {
            bool isMarker = i == 0 || i + 1 == title.size();
            if (!isMarker) {
                wchar_t previous = title[i - 1];
                isMarker = previous == L'\\' || previous == L'/' ||
                           previous == L':';
            }
            if (isMarker) {
                i++;
                continue;
            }
        }

        bool isSpace = c == L' ' || c == L'\t' || c == L'\r' || c == L'\n';
        if (isSpace) {
            if (!out.empty() && out.back() != L' ') {
                out.push_back(L' ');
            }
        } else {
            out.push_back(c);
        }
        i++;
    }

    if (!out.empty() && out.back() == L' ') {
        out.pop_back();
    }

    return out;
}
// <<< WINDOW_IDENTITY_CORE <<<

struct WindowIdentity {
    std::wstring exePath;
    std::wstring exeName;
    std::wstring className;
    // Key component captured when the identity is first resolved. Note that the
    // log line prints the title as of the event instead: at EVENT_OBJECT_CREATE
    // the title is often not set yet, and titles change during a window's life.
    std::wstring normalizedTitle;
    int ordinal = 0;
    // Whether this app's remembered position and size may be used. Carried with
    // the identity because every restore and relayout decision needs it, and it is
    // resolved once, from the allowlist entry that matched, when the window is
    // first seen. Defaults are "yes" so that an identity built without an
    // allowlist match behaves exactly as it did before the flags were read.
    bool restorePosition = true;
    bool restoreSize = true;
};

// Worker-thread-only state. Every function below that touches these runs on the
// event thread, which is why none of it is guarded by a lock.
static std::unordered_map<HWND, WindowIdentity> g_identityCache;

// Ordinals currently held by live windows, per identity key. A closed window
// releases its number so the next window of the same key can reuse it; see
// AllocateOrdinal in the core block for why that matters.
static std::map<std::tuple<std::wstring, std::wstring, std::wstring>,
                std::set<int>>
    g_liveOrdinals;
// One allowlist entry: the executable to match, and which parts of its remembered
// geometry the mod may use.
struct AllowlistEntry {
    std::wstring exe;  // lowercased
    bool restorePosition = true;
    bool restoreSize = true;
};

static std::vector<AllowlistEntry> g_allowlist;
static bool g_allowlistIsEmpty = true;

// Assigned in creation order within the session and persisted with the geometry
// entry, so a window can be matched to last session's geometry even after its
// title changed (see the ordinal fallback in FindStoredWindowFor).
//
// Defined in the GEOMETRY_CORE block below, kept there so the tests extract it;
// the allocation rule itself is documented with the definition.
static int AllocateOrdinal(const std::set<int>& liveOrdinals);

static std::tuple<std::wstring, std::wstring, std::wstring> OrdinalKeyOf(
    const WindowIdentity& identity) {
    return std::make_tuple(identity.exePath, identity.className,
                           identity.normalizedTitle);
}

// Claims the lowest free ordinal for this window; released in ReleaseOrdinal.
static int NextOrdinal(const WindowIdentity& identity) {
    std::set<int>& live = g_liveOrdinals[OrdinalKeyOf(identity)];
    int ordinal = AllocateOrdinal(live);
    live.insert(ordinal);
    return ordinal;
}

// Called when a window is destroyed, after its geometry has been stored.
static void ReleaseOrdinal(const WindowIdentity& identity) {
    auto it = g_liveOrdinals.find(OrdinalKeyOf(identity));
    if (it == g_liveOrdinals.end()) {
        return;
    }
    it->second.erase(identity.ordinal);
    if (it->second.empty()) {
        g_liveOrdinals.erase(it);
    }
}

// >>> GEOMETRY_CORE >>>
// Everything in this block is pure logic over plain data with no Win32 calls, so
// the rules that are most likely to be subtly wrong -- matching, clamping,
// eviction and persistence -- are unit-tested off Windows. The Win32 layer below
// converts to and from these types at the boundary.

struct GeoRect {
    long left = 0;
    long top = 0;
    long right = 0;
    long bottom = 0;
};

struct NormRect {
    double left = 0;
    double top = 0;
    double right = 0;
    double bottom = 0;
};

// A monitor as it currently exists. key/fallbackId identity is described at
// MonitorIdentityOf below.
struct MonitorView {
    std::wstring key;
    std::wstring fallbackId;
    std::wstring friendlyName;
    std::wstring gdiName;  // \\.\DISPLAYn, for mapping and diagnostics only
    GeoRect pixelRect;
    GeoRect workRect;
    bool primary = false;
};

// A monitor as stored. Adds the arrangement relative to the primary monitor,
// which is what makes "the monitor that was on the right" recoverable when that
// exact panel is absent (undocked).
struct StoredMonitor {
    std::wstring key;
    std::wstring fallbackId;
    std::wstring friendlyName;
    std::wstring slotAtSave;  // diagnostic only, never used for matching
    GeoRect pixelRect;
    GeoRect workRect;  // stored, but deliberately not compared by ConfigsMatch
    bool primary = false;
    double relLeft = 0;
    double relTop = 0;
    double relWidth = 1;
    double relHeight = 1;
};

struct StoredWindow {
    std::wstring exePath;
    std::wstring className;
    std::wstring titleKey;
    int ordinal = 0;
    long long lastSeen = 0;
    int configId = 0;
    std::wstring monitorKey;
    std::wstring state;  // "normal" or "maximized"
    GeoRect rectPixels;
    NormRect rectNormalized;
    unsigned dpiAtSave = 0;
};

struct StoredConfig {
    int id = 0;
    long long lastSeen = 0;
    std::vector<StoredMonitor> monitors;
};

struct GeometryStore {
    int schemaVersion = 1;
    long long nextConfigId = 1;
    std::vector<StoredConfig> configs;
    std::vector<StoredWindow> windows;
};

// Limits and thresholds.
static constexpr size_t kMaxStoredWindows = 500;
static constexpr size_t kMaxStoredConfigs = 32;
static constexpr long long kStoreMaxAgeSeconds = 90LL * 24 * 60 * 60;
static constexpr long kMinVisiblePixels = 80;
static constexpr long kTitleBarHeight = 24;
static constexpr long kApplyTolerancePixels = 2;
static constexpr double kRelativeMatchTolerance = 0.25;

static bool GeoRectsEqual(const GeoRect& a, const GeoRect& b) {
    return a.left == b.left && a.top == b.top && a.right == b.right &&
           a.bottom == b.bottom;
}

static long GeoRectWidth(const GeoRect& r) {
    return r.right - r.left;
}

static long GeoRectHeight(const GeoRect& r) {
    return r.bottom - r.top;
}

static bool GeoRectsApproxEqual(const GeoRect& a, const GeoRect& b,
                                long tolerance) {
    return std::abs(a.left - b.left) <= tolerance &&
           std::abs(a.top - b.top) <= tolerance &&
           std::abs(a.right - b.right) <= tolerance &&
           std::abs(a.bottom - b.bottom) <= tolerance;
}

static NormRect ToNormalized(const GeoRect& rect, const GeoRect& monitor) {
    NormRect normalized;
    long width = GeoRectWidth(monitor);
    long height = GeoRectHeight(monitor);
    if (width <= 0 || height <= 0) {
        return normalized;
    }

    // Deliberately not clamped to [0,1]: a window hanging off the edge is
    // legitimate and must survive the round trip.
    normalized.left = (double)(rect.left - monitor.left) / (double)width;
    normalized.top = (double)(rect.top - monitor.top) / (double)height;
    normalized.right = (double)(rect.right - monitor.left) / (double)width;
    normalized.bottom = (double)(rect.bottom - monitor.top) / (double)height;
    return normalized;
}

static GeoRect FromNormalized(const NormRect& normalized,
                              const GeoRect& monitor) {
    GeoRect rect;
    long width = GeoRectWidth(monitor);
    long height = GeoRectHeight(monitor);
    rect.left = monitor.left + (long)std::lround(normalized.left * width);
    rect.top = monitor.top + (long)std::lround(normalized.top * height);
    rect.right = monitor.left + (long)std::lround(normalized.right * width);
    rect.bottom = monitor.top + (long)std::lround(normalized.bottom * height);
    return rect;
}

// Both types identify a monitor the same way: the CCD hardware path when
// available, otherwise the EnumDisplayDevices interface path. \\.\DISPLAYn is
// never an identity -- Windows renumbers slots across driver resets.
static std::wstring MonitorIdentityOf(const std::wstring& key,
                                      const std::wstring& fallbackId) {
    return !key.empty() ? key : fallbackId;
}

static std::wstring MonitorIdentityOf(const MonitorView& view) {
    return MonitorIdentityOf(view.key, view.fallbackId);
}

static std::wstring MonitorIdentityOf(const StoredMonitor& monitor) {
    return MonitorIdentityOf(monitor.key, monitor.fallbackId);
}

// Configuration match, used to decide exact restore versus normalized mapping.
//
// Compares the monitor sets unordered, on identity, primary flag and pixel rect
// only. workRect is NOT compared: taskbar auto-hide, a moved taskbar or a
// different DPI change the work area without any monitor change, and treating
// that as a new configuration would both grow the config table without bound
// and force a spurious clamp on every window.
static bool ConfigsMatch(const std::vector<MonitorView>& current,
                         const std::vector<StoredMonitor>& saved) {
    if (current.size() != saved.size()) {
        return false;
    }

    std::vector<bool> used(saved.size(), false);
    for (const MonitorView& view : current) {
        std::wstring identity = MonitorIdentityOf(view);
        bool found = false;
        for (size_t i = 0; i < saved.size(); i++) {
            if (used[i]) {
                continue;
            }
            if (MonitorIdentityOf(saved[i]) == identity &&
                saved[i].primary == view.primary &&
                GeoRectsEqual(saved[i].pixelRect, view.pixelRect)) {
                used[i] = true;
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}

// Arrangement of a monitor relative to the primary one, expressed in units of
// the primary monitor's size.
static void ComputeRelativeToPrimary(const std::vector<MonitorView>& monitors,
                                     size_t index, double& relLeft,
                                     double& relTop, double& relWidth,
                                     double& relHeight) {
    relLeft = 0;
    relTop = 0;
    relWidth = 1;
    relHeight = 1;

    const MonitorView* primary = nullptr;
    for (const MonitorView& monitor : monitors) {
        if (monitor.primary) {
            primary = &monitor;
            break;
        }
    }
    if (!primary || index >= monitors.size()) {
        return;
    }

    long primaryWidth = GeoRectWidth(primary->pixelRect);
    long primaryHeight = GeoRectHeight(primary->pixelRect);
    if (primaryWidth <= 0 || primaryHeight <= 0) {
        return;
    }

    const GeoRect& rect = monitors[index].pixelRect;
    relLeft = (double)(rect.left - primary->pixelRect.left) / primaryWidth;
    relTop = (double)(rect.top - primary->pixelRect.top) / primaryHeight;
    relWidth = (double)GeoRectWidth(rect) / primaryWidth;
    relHeight = (double)GeoRectHeight(rect) / primaryHeight;
}

enum class MonitorMatchRule {
    None,
    HardwarePath,
    FallbackId,
    FriendlyName,
    RelativePosition,
    Primary,
};

struct MonitorResolution {
    int index = -1;
    MonitorMatchRule rule = MonitorMatchRule::None;
};

// Five-step monitor resolution. First hit wins; a relative
// position match must be within kRelativeMatchTolerance on every component, and
// otherwise we fall through to the primary monitor rather than guessing.
static MonitorResolution ResolveTargetMonitor(
    const std::vector<MonitorView>& monitors, const StoredWindow& window,
    const StoredConfig* savedConfig) {
    MonitorResolution resolution;

    for (size_t i = 0; i < monitors.size(); i++) {
        if (!monitors[i].key.empty() && monitors[i].key == window.monitorKey) {
            resolution.index = (int)i;
            resolution.rule = MonitorMatchRule::HardwarePath;
            return resolution;
        }
    }

    if (savedConfig) {
        for (size_t i = 0; i < monitors.size(); i++) {
            if (!monitors[i].fallbackId.empty() &&
                monitors[i].fallbackId == window.monitorKey) {
                resolution.index = (int)i;
                resolution.rule = MonitorMatchRule::FallbackId;
                return resolution;
            }
        }
    }

    const StoredMonitor* savedMonitor = nullptr;
    if (savedConfig) {
        for (const StoredMonitor& monitor : savedConfig->monitors) {
            if (MonitorIdentityOf(monitor) == window.monitorKey) {
                savedMonitor = &monitor;
                break;
            }
        }
    }

    if (savedMonitor && !savedMonitor->friendlyName.empty()) {
        int matches = 0;
        int matchIndex = -1;
        for (size_t i = 0; i < monitors.size(); i++) {
            if (monitors[i].friendlyName == savedMonitor->friendlyName) {
                matches++;
                matchIndex = (int)i;
            }
        }
        if (matches == 1) {
            resolution.index = matchIndex;
            resolution.rule = MonitorMatchRule::FriendlyName;
            return resolution;
        }
    }

    if (savedMonitor) {
        double bestDelta = 0;
        int bestIndex = -1;
        for (size_t i = 0; i < monitors.size(); i++) {
            double relLeft, relTop, relWidth, relHeight;
            ComputeRelativeToPrimary(monitors, i, relLeft, relTop, relWidth,
                                     relHeight);
            double delta = std::abs(relLeft - savedMonitor->relLeft) +
                           std::abs(relTop - savedMonitor->relTop) +
                           std::abs(relWidth - savedMonitor->relWidth) +
                           std::abs(relHeight - savedMonitor->relHeight);
            bool acceptable =
                std::abs(relLeft - savedMonitor->relLeft) <=
                    kRelativeMatchTolerance &&
                std::abs(relTop - savedMonitor->relTop) <=
                    kRelativeMatchTolerance &&
                std::abs(relWidth - savedMonitor->relWidth) <=
                    kRelativeMatchTolerance &&
                std::abs(relHeight - savedMonitor->relHeight) <=
                    kRelativeMatchTolerance;
            if (acceptable && (bestIndex < 0 || delta < bestDelta)) {
                bestIndex = (int)i;
                bestDelta = delta;
            }
        }
        if (bestIndex >= 0) {
            resolution.index = bestIndex;
            resolution.rule = MonitorMatchRule::RelativePosition;
            return resolution;
        }
    }

    for (size_t i = 0; i < monitors.size(); i++) {
        if (monitors[i].primary) {
            resolution.index = (int)i;
            resolution.rule = MonitorMatchRule::Primary;
            return resolution;
        }
    }

    if (!monitors.empty()) {
        resolution.index = 0;
        resolution.rule = MonitorMatchRule::Primary;
    }

    return resolution;
}

// Whether enough of the window -- a strip that can act as its title bar -- is
// inside the work area to be reachable with the mouse.
static bool IsTitleBarReachable(const GeoRect& rect, const GeoRect& workRect) {
    long overlapX = std::min(rect.right, workRect.right) -
                    std::max(rect.left, workRect.left);
    long overlapY = std::min(rect.bottom, workRect.bottom) -
                    std::max(rect.top, workRect.top);
    return overlapX >= kMinVisiblePixels && overlapY >= kTitleBarHeight;
}

// Whether geometry may be moved back into view. Stored geometry is restored
// verbatim when the configuration matches and the window is still reachable --
// users deliberately place windows partly off-screen, and silently "fixing" that
// is a bug report, not a feature. Clamping happens only when the configuration
// changed (so the stored coordinates may refer to monitors or resolutions that
// no longer exist) or when the window would be unreachable.
static bool ShouldClampRect(const GeoRect& rect, const GeoRect& workRect,
                            bool configurationChanged) {
    return configurationChanged || !IsTitleBarReachable(rect, workRect);
}

// Moves (and if necessary shrinks) a rect so that it is fully inside the work
// area. Only called when the configuration changed or the title bar would be
// unreachable: when the configuration matches, saved geometry is restored
// verbatim, because users deliberately place windows partly off-screen.
static GeoRect ClampRectToWorkArea(const GeoRect& rect,
                                   const GeoRect& workRect) {
    long width = GeoRectWidth(rect);
    long height = GeoRectHeight(rect);
    long workWidth = GeoRectWidth(workRect);
    long workHeight = GeoRectHeight(workRect);

    if (workWidth <= 0 || workHeight <= 0) {
        return rect;
    }

    if (width > workWidth) {
        width = workWidth;
    }
    if (height > workHeight) {
        height = workHeight;
    }

    long left = std::max(rect.left, workRect.left);
    left = std::min(left, workRect.right - width);
    long top = std::max(rect.top, workRect.top);
    top = std::min(top, workRect.bottom - height);

    GeoRect clamped;
    clamped.left = left;
    clamped.top = top;
    clamped.right = left + width;
    clamped.bottom = top + height;
    return clamped;
}

// Which parts of a remembered rectangle an app is allowed to use. Both default to
// on: the settings entry ships both toggled on, and a window whose allowlist entry
// says nothing keeps the behaviour it had before the flags were read.
struct RestoreFlags {
    bool restorePosition = true;
    bool restoreSize = true;
};

static RestoreFlags MakeRestoreFlags(bool restorePosition, bool restoreSize) {
    return RestoreFlags{restorePosition, restoreSize};
}

// Composes the rectangle to apply from the remembered one and the window's own.
//
// "Restore size off" means the window keeps the size it has now and takes only the
// position; "restore position off" means it keeps where it is and takes only the
// size. Applying the flags here, rather than at capture, is deliberate: both
// components are still recorded, so turning a flag back on gives the window the
// size or position it last had, instead of a value frozen whenever the flag was
// switched off.
static GeoRect ApplyRestoreFlags(const GeoRect& target, const GeoRect& current,
                                 const RestoreFlags& flags) {
    GeoRect result = target;

    if (!flags.restoreSize) {
        result.right = result.left + GeoRectWidth(current);
        result.bottom = result.top + GeoRectHeight(current);
    }

    if (!flags.restorePosition) {
        long width = GeoRectWidth(result);
        long height = GeoRectHeight(result);
        result.left = current.left;
        result.top = current.top;
        result.right = current.left + width;
        result.bottom = current.top + height;
    }

    return result;
}

static std::wstring FormatRestoreFlags(const RestoreFlags& flags) {
    if (flags.restorePosition && flags.restoreSize) {
        return L"position+size";
    }
    if (flags.restorePosition) {
        return L"position";
    }
    if (flags.restoreSize) {
        return L"size";
    }
    return L"none";
}

// WINDOWPLACEMENT's rcNormalPosition is in *workspace* coordinates, which differ
// from the screen coordinates every other rectangle in this mod uses by the
// thickness of the taskbar when the taskbar sits on the top or left edge -- the
// default on Windows 10, and reachable on Windows 11 with a taskbar mod. Storing it
// unchanged meant a window closed while maximized came back shifted by that amount,
// and the next save read the already-shifted rectangle, so the window drifted
// further on every maximize-and-reopen. The conversion is a no-op with a bottom or
// right taskbar.
static GeoRect PlacementRectToScreen(const GeoRect& rect, const GeoRect& monitor,
                                     const GeoRect& work) {
    const long dx = work.left - monitor.left;
    const long dy = work.top - monitor.top;
    return {rect.left + dx, rect.top + dy, rect.right + dx, rect.bottom + dy};
}

// The store and the journal live in a folder named for the signed-in user. A tool
// mod runs one process per signed-in session, but Wh_GetModStoragePath is a single
// machine-wide folder, so before the folder existed the store was shared: with two
// users signed in (fast user switching) each session's flush replaced the other's
// data, and each user's window titles were readable from the other session.
// The text form of a security identifier, from its parts: "S-<revision>-<authority>"
// followed by one "-<n>" per sub-authority, which is what the file names below are
// built from.
//
// The parts come in separately rather than as a SID* because this is the half that
// can be wrong -- the authority is a big-endian six-byte value and the sub-authorities
// are 32-bit words -- and it is therefore the half worth testing. The struct
// unpacking in SidToString is the other half and is four lines long.
//
// Written by hand rather than with ConvertSidToStringSidW on purpose, and not
// because that function is out of reach: sddl.h is available to a mod (the
// desktop-sticky-notes mod includes it) and advapi32 is linked here anyway: the
// token calls and the access control lists come from it. The reason is that
// everything below is plain arithmetic over values the caller has already read, so
// the half that can be wrong is a pure function with unit tests over it, rather
// than a call that allocates a string the caller then has to free.
//
// Documented form note: an identifier authority larger than 32 bits is rendered in
// hexadecimal by the SID documentation. A process token's authority is 5 on every
// installation this mod runs on (16 for a mandatory label, 15 for an app package),
// so that rendering is not reachable here and this keeps one decimal path instead --
// a longer string than the canonical one only if the impossible happens, and a
// usable file name either way.
static std::wstring FormatSidString(unsigned revision, unsigned long long authority,
                                    const std::vector<unsigned long>& subAuthorities) {
    std::wstring text = L"S-" + std::to_wstring(revision) + L"-" +
                        std::to_wstring(authority);
    for (unsigned long sub : subAuthorities) {
        text += L"-" + std::to_wstring(sub);
    }
    return text;
}

// Ordinal for a new window: the lowest number no live window of the same key is
// using.
//
// Lowest free rather than monotonic, because matching requires the stored ordinal to
// be equal: a counter would hand a reopened window a fresh number, and it could never
// match the entry saved moments earlier, which is the headline use case. A closed
// window's number is therefore reused, while windows open at the same time still get
// distinct ones.
static int AllocateOrdinal(const std::set<int>& liveOrdinals) {
    int candidate = 0;
    for (int live : liveOrdinals) {  // std::set iterates in ascending order
        if (live == candidate) {
            candidate++;
        } else if (live > candidate) {
            break;
        }
    }
    return candidate;
}

static bool ConfigIsReferenced(const GeometryStore& store, int configId) {
    for (const StoredWindow& window : store.windows) {
        if (window.configId == configId) {
            return true;
        }
    }
    return false;
}

// Expiry first, then orphaned configurations, then the caps. Configurations that
// windows still reference are evicted last: dropping one costs those windows
// their exact-restore path (they fall back to normalized mapping), so it should
// only happen under real pressure.
static void PruneStore(GeometryStore& store, long long now) {
    store.windows.erase(
        std::remove_if(store.windows.begin(), store.windows.end(),
                       [now](const StoredWindow& window) {
                           return now - window.lastSeen > kStoreMaxAgeSeconds;
                       }),
        store.windows.end());

    store.configs.erase(
        std::remove_if(store.configs.begin(), store.configs.end(),
                       [&store, now](const StoredConfig& config) {
                           if (now - config.lastSeen > kStoreMaxAgeSeconds) {
                               return true;
                           }
                           return !ConfigIsReferenced(store, config.id);
                       }),
        store.configs.end());

    if (store.windows.size() > kMaxStoredWindows) {
        std::sort(store.windows.begin(), store.windows.end(),
                  [](const StoredWindow& a, const StoredWindow& b) {
                      return a.lastSeen > b.lastSeen;
                  });
        store.windows.resize(kMaxStoredWindows);
    }

    if (store.configs.size() > kMaxStoredConfigs) {
        // Most valuable first: configurations windows still reference, then
        // newest. resize() truncates the tail, so the least valuable entries
        // are the ones that go.
        std::sort(store.configs.begin(), store.configs.end(),
                  [&store](const StoredConfig& a, const StoredConfig& b) {
                      bool aReferenced = ConfigIsReferenced(store, a.id);
                      bool bReferenced = ConfigIsReferenced(store, b.id);
                      if (aReferenced != bReferenced) {
                          return aReferenced;
                      }
                      return a.lastSeen > b.lastSeen;
                  });
        store.configs.resize(kMaxStoredConfigs);
    }

    // A window whose configuration disappeared loses its exact-restore path but
    // keeps its normalized rect, so it is repaired rather than dropped.
    for (StoredWindow& window : store.windows) {
        bool configExists = false;
        for (const StoredConfig& config : store.configs) {
            if (config.id == window.configId) {
                configExists = true;
                break;
            }
        }
        if (!configExists) {
            window.configId = -1;
        }
    }
}

// Finds the stored entry for a window.
//
// Exact match first. If that misses -- most often because the title changed
// between sessions -- the ordinal fallback matches on executable, class and
// ordinal, and only when exactly one stored entry matches, so an ambiguous case
// yields no restore rather than a wrong one. The bias is deliberate: miss a few
// windows, never restore something wrong.
static const StoredWindow* FindStoredWindowFor(const GeometryStore& store,
                                              const std::wstring& exePath,
                                              const std::wstring& className,
                                              const std::wstring& titleKey,
                                              int ordinal) {
    for (const StoredWindow& window : store.windows) {
        if (window.exePath == exePath && window.className == className &&
            window.titleKey == titleKey && window.ordinal == ordinal) {
            return &window;
        }
    }

    const StoredWindow* candidate = nullptr;
    for (const StoredWindow& window : store.windows) {
        if (window.exePath == exePath && window.className == className &&
            window.ordinal == ordinal && window.titleKey != titleKey) {
            if (candidate) {
                return nullptr;  // ambiguous
            }
            candidate = &window;
        }
    }
    return candidate;
}

// --- JSON ---------------------------------------------------------------
//
// Hand-rolled, because the store is the only consumer. Kept in the core block so
// that round-trip and malformed-input behaviour is unit-tested.

struct JsonValue {
    enum class Type { Null, Bool, Number, String, Array, Object };

    Type type = Type::Null;
    bool boolean = false;
    double number = 0;
    bool numberIsInteger = false;
    long long integer = 0;
    std::wstring str;
    std::vector<JsonValue> array;
    std::vector<std::pair<std::wstring, JsonValue>> object;

    const JsonValue* Find(const wchar_t* name) const {
        for (const auto& entry : object) {
            if (entry.first == name) {
                return &entry.second;
            }
        }
        return nullptr;
    }
};

static void JsonAppendEscaped(std::wstring& out, const std::wstring& value) {
    out.push_back(L'"');
    for (wchar_t c : value) {
        switch (c) {
            case L'"':
                out += L"\\\"";
                break;
            case L'\\':
                out += L"\\\\";
                break;
            case L'\n':
                out += L"\\n";
                break;
            case L'\r':
                out += L"\\r";
                break;
            case L'\t':
                out += L"\\t";
                break;
            default:
                if (c > 0 && c < 0x20) {
                    static const wchar_t kHex[] = L"0123456789abcdef";
                    out += L"\\u00";
                    out.push_back(kHex[(c >> 4) & 0xF]);
                    out.push_back(kHex[c & 0xF]);
                } else {
                    // Non-ASCII is written through; the file layer converts the
                    // whole text to UTF-8.
                    out.push_back(c);
                }
                break;
        }
    }
    out.push_back(L'"');
}

static std::wstring FormatDouble(double value) {
    std::wostringstream stream;
    stream.precision(10);
    stream << value;
    return stream.str();
}

static void JsonAppendRect(std::wstring& out, const wchar_t* indent,
                           const GeoRect& rect) {
    out += indent;
    out += L"{\"l\": ";
    out += std::to_wstring(rect.left);
    out += L", \"t\": ";
    out += std::to_wstring(rect.top);
    out += L", \"r\": ";
    out += std::to_wstring(rect.right);
    out += L", \"b\": ";
    out += std::to_wstring(rect.bottom);
    out += L"}";
}

static std::wstring SerializeGeometryStore(const GeometryStore& store) {
    std::wstring out;
    out += L"{\n";
    out += L"  \"schemaVersion\": " + std::to_wstring(store.schemaVersion) +
           L",\n";
    out += L"  \"nextConfigId\": " + std::to_wstring(store.nextConfigId) +
           L",\n";
    out += L"  \"configs\": [\n";
    for (size_t c = 0; c < store.configs.size(); c++) {
        const StoredConfig& config = store.configs[c];
        out += L"    {\n";
        out += L"      \"id\": " + std::to_wstring(config.id) + L",\n";
        out += L"      \"lastSeen\": " + std::to_wstring(config.lastSeen) +
               L",\n";
        out += L"      \"monitors\": [\n";
        for (size_t m = 0; m < config.monitors.size(); m++) {
            const StoredMonitor& monitor = config.monitors[m];
            out += L"        {\n";
            out += L"          \"key\": ";
            JsonAppendEscaped(out, monitor.key);
            out += L",\n          \"fallbackId\": ";
            JsonAppendEscaped(out, monitor.fallbackId);
            out += L",\n          \"friendlyName\": ";
            JsonAppendEscaped(out, monitor.friendlyName);
            out += L",\n          \"slotAtSave\": ";
            JsonAppendEscaped(out, monitor.slotAtSave);
            out += L",\n          \"primary\": ";
            out += monitor.primary ? L"true" : L"false";
            out += L",\n";
            out += L"          \"relLeft\": " + FormatDouble(monitor.relLeft) +
                   L", \"relTop\": " + FormatDouble(monitor.relTop) +
                   L", \"relWidth\": " + FormatDouble(monitor.relWidth) +
                   L", \"relHeight\": " + FormatDouble(monitor.relHeight) +
                   L",\n";
            out += L"          \"pixelRect\": ";
            JsonAppendRect(out, L"", monitor.pixelRect);
            out += L",\n          \"workRect\": ";
            JsonAppendRect(out, L"", monitor.workRect);
            out += L"\n        }";
            out += (m + 1 < config.monitors.size()) ? L",\n" : L"\n";
        }
        out += L"      ]\n    }";
        out += (c + 1 < store.configs.size()) ? L",\n" : L"\n";
    }
    out += L"  ],\n  \"windows\": [\n";
    for (size_t w = 0; w < store.windows.size(); w++) {
        const StoredWindow& window = store.windows[w];
        out += L"    {\n";
        out += L"      \"exePath\": ";
        JsonAppendEscaped(out, window.exePath);
        out += L",\n      \"className\": ";
        JsonAppendEscaped(out, window.className);
        out += L",\n      \"titleKey\": ";
        JsonAppendEscaped(out, window.titleKey);
        out += L",\n      \"ordinal\": " + std::to_wstring(window.ordinal) +
               L",\n";
        out += L"      \"lastSeen\": " + std::to_wstring(window.lastSeen) +
               L",\n";
        out += L"      \"configId\": " + std::to_wstring(window.configId) +
               L",\n      \"monitorKey\": ";
        JsonAppendEscaped(out, window.monitorKey);
        out += L",\n      \"state\": ";
        JsonAppendEscaped(out, window.state);
        out += L",\n      \"dpiAtSave\": " + std::to_wstring(window.dpiAtSave) +
               L",\n";
        out += L"      \"rectPixels\": ";
        JsonAppendRect(out, L"", window.rectPixels);
        out += L",\n      \"rectNormalized\": {\"l\": " +
               FormatDouble(window.rectNormalized.left) + L", \"t\": " +
               FormatDouble(window.rectNormalized.top) + L", \"r\": " +
               FormatDouble(window.rectNormalized.right) + L", \"b\": " +
               FormatDouble(window.rectNormalized.bottom) + L"}\n";
        out += L"    }";
        out += (w + 1 < store.windows.size()) ? L",\n" : L"\n";
    }
    out += L"  ]\n}\n";
    return out;
}

class JsonParser {
   public:
    explicit JsonParser(const std::wstring& text) : text_(text) {}

    bool Parse(JsonValue& out) {
        SkipWhitespace();
        if (!ParseValue(out)) {
            return false;
        }
        SkipWhitespace();
        return pos_ == text_.size();
    }

   private:
    const std::wstring& text_;
    size_t pos_ = 0;

    void SkipWhitespace() {
        while (pos_ < text_.size()) {
            wchar_t c = text_[pos_];
            if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
                pos_++;
            } else {
                break;
            }
        }
    }

    bool ParseValue(JsonValue& out) {
        if (pos_ >= text_.size()) {
            return false;
        }
        wchar_t c = text_[pos_];
        switch (c) {
            case L'{':
                return ParseObject(out);
            case L'[':
                return ParseArray(out);
            case L'"':
                out.type = JsonValue::Type::String;
                return ParseString(out.str);
            case L't':
                return ParseLiteral(L"true", out, true);
            case L'f':
                return ParseLiteral(L"false", out, false);
            case L'n':
                if (text_.compare(pos_, 4, L"null") != 0) {
                    return false;
                }
                pos_ += 4;
                out.type = JsonValue::Type::Null;
                return true;
            default:
                return ParseNumber(out);
        }
    }

    bool ParseLiteral(const wchar_t* literal, JsonValue& out, bool value) {
        size_t length = std::wcslen(literal);
        if (text_.compare(pos_, length, literal) != 0) {
            return false;
        }
        pos_ += length;
        out.type = JsonValue::Type::Bool;
        out.boolean = value;
        return true;
    }

    bool ParseObject(JsonValue& out) {
        out.type = JsonValue::Type::Object;
        pos_++;  // '{'
        SkipWhitespace();
        if (pos_ < text_.size() && text_[pos_] == L'}') {
            pos_++;
            return true;
        }
        while (true) {
            SkipWhitespace();
            std::wstring name;
            if (!ParseString(name)) {
                return false;
            }
            SkipWhitespace();
            if (pos_ >= text_.size() || text_[pos_] != L':') {
                return false;
            }
            pos_++;
            SkipWhitespace();
            JsonValue value;
            if (!ParseValue(value)) {
                return false;
            }
            out.object.emplace_back(std::move(name), std::move(value));
            SkipWhitespace();
            if (pos_ < text_.size() && text_[pos_] == L',') {
                pos_++;
                continue;
            }
            if (pos_ < text_.size() && text_[pos_] == L'}') {
                pos_++;
                return true;
            }
            return false;
        }
    }

    bool ParseArray(JsonValue& out) {
        out.type = JsonValue::Type::Array;
        pos_++;  // '['
        SkipWhitespace();
        if (pos_ < text_.size() && text_[pos_] == L']') {
            pos_++;
            return true;
        }
        while (true) {
            SkipWhitespace();
            JsonValue value;
            if (!ParseValue(value)) {
                return false;
            }
            out.array.push_back(std::move(value));
            SkipWhitespace();
            if (pos_ < text_.size() && text_[pos_] == L',') {
                pos_++;
                continue;
            }
            if (pos_ < text_.size() && text_[pos_] == L']') {
                pos_++;
                return true;
            }
            return false;
        }
    }

    bool ParseString(std::wstring& out) {
        if (pos_ >= text_.size() || text_[pos_] != L'"') {
            return false;
        }
        pos_++;
        out.clear();
        while (pos_ < text_.size()) {
            wchar_t c = text_[pos_++];
            if (c == L'"') {
                return true;
            }
            if (c != L'\\') {
                out.push_back(c);
                continue;
            }
            if (pos_ >= text_.size()) {
                return false;
            }
            wchar_t escape = text_[pos_++];
            switch (escape) {
                case L'"':
                    out.push_back(L'"');
                    break;
                case L'\\':
                    out.push_back(L'\\');
                    break;
                case L'/':
                    out.push_back(L'/');
                    break;
                case L'b':
                    out.push_back(L'\b');
                    break;
                case L'f':
                    out.push_back(L'\f');
                    break;
                case L'n':
                    out.push_back(L'\n');
                    break;
                case L'r':
                    out.push_back(L'\r');
                    break;
                case L't':
                    out.push_back(L'\t');
                    break;
                case L'u': {
                    unsigned value = 0;
                    for (int i = 0; i < 4; i++) {
                        if (pos_ >= text_.size()) {
                            return false;
                        }
                        wchar_t digit = text_[pos_++];
                        unsigned nibble;
                        if (digit >= L'0' && digit <= L'9') {
                            nibble = (unsigned)(digit - L'0');
                        } else if (digit >= L'a' && digit <= L'f') {
                            nibble = (unsigned)(digit - L'a' + 10);
                        } else if (digit >= L'A' && digit <= L'F') {
                            nibble = (unsigned)(digit - L'A' + 10);
                        } else {
                            return false;
                        }
                        value = (value << 4) | nibble;
                    }
                    // Surrogate halves are appended as-is: on Windows wchar_t
                    // is UTF-16, so a pair reconstructs the character.
                    out.push_back((wchar_t)value);
                    break;
                }
                default:
                    return false;
            }
        }
        return false;
    }

    bool ParseNumber(JsonValue& out) {
        size_t start = pos_;
        if (pos_ < text_.size() && (text_[pos_] == L'-' || text_[pos_] == L'+')) {
            pos_++;
        }
        bool isInteger = true;
        while (pos_ < text_.size()) {
            wchar_t c = text_[pos_];
            if (c >= L'0' && c <= L'9') {
                pos_++;
            } else if (c == L'.' || c == L'e' || c == L'E' || c == L'+' ||
                       c == L'-') {
                isInteger = false;
                pos_++;
            } else {
                break;
            }
        }
        if (pos_ == start) {
            return false;
        }

        std::wstring token = text_.substr(start, pos_ - start);
        out.type = JsonValue::Type::Number;
        out.numberIsInteger = isInteger;
        if (isInteger) {
            out.integer = std::wcstoll(token.c_str(), nullptr, 10);
            out.number = (double)out.integer;
        } else {
            out.number = std::wcstod(token.c_str(), nullptr);
            out.integer = (long long)out.number;
        }
        return true;
    }
};

static long long JsonInt(const JsonValue* value, long long fallback) {
    if (!value || value->type != JsonValue::Type::Number) {
        return fallback;
    }
    return value->integer;
}

static double JsonDouble(const JsonValue* value, double fallback) {
    if (!value || value->type != JsonValue::Type::Number) {
        return fallback;
    }
    return value->number;
}

static bool JsonBool(const JsonValue* value, bool fallback) {
    if (!value || value->type != JsonValue::Type::Bool) {
        return fallback;
    }
    return value->boolean;
}

static std::wstring JsonString(const JsonValue* value, const wchar_t* fallback) {
    if (!value || value->type != JsonValue::Type::String) {
        return std::wstring(fallback);
    }
    return value->str;
}

static GeoRect JsonRect(const JsonValue* value) {
    GeoRect rect;
    if (!value || value->type != JsonValue::Type::Object) {
        return rect;
    }
    rect.left = (long)JsonInt(value->Find(L"l"), 0);
    rect.top = (long)JsonInt(value->Find(L"t"), 0);
    rect.right = (long)JsonInt(value->Find(L"r"), 0);
    rect.bottom = (long)JsonInt(value->Find(L"b"), 0);
    return rect;
}

static bool DeserializeGeometryStore(const std::wstring& text,
                                    GeometryStore& store) {
    JsonValue root;
    JsonParser parser(text);
    if (!parser.Parse(root) || root.type != JsonValue::Type::Object) {
        return false;
    }

    GeometryStore parsed;
    parsed.schemaVersion = (int)JsonInt(root.Find(L"schemaVersion"), 1);
    if (parsed.schemaVersion != 1) {
        return false;  // unknown format: start clean rather than guess
    }
    parsed.nextConfigId = JsonInt(root.Find(L"nextConfigId"), 1);

    const JsonValue* configs = root.Find(L"configs");
    if (configs && configs->type == JsonValue::Type::Array) {
        for (const JsonValue& entry : configs->array) {
            if (entry.type != JsonValue::Type::Object) {
                continue;
            }
            StoredConfig config;
            config.id = (int)JsonInt(entry.Find(L"id"), 0);
            config.lastSeen = JsonInt(entry.Find(L"lastSeen"), 0);
            const JsonValue* monitors = entry.Find(L"monitors");
            if (monitors && monitors->type == JsonValue::Type::Array) {
                for (const JsonValue& monitorValue : monitors->array) {
                    if (monitorValue.type != JsonValue::Type::Object) {
                        continue;
                    }
                    StoredMonitor monitor;
                    monitor.key = JsonString(monitorValue.Find(L"key"), L"");
                    monitor.fallbackId =
                        JsonString(monitorValue.Find(L"fallbackId"), L"");
                    monitor.friendlyName =
                        JsonString(monitorValue.Find(L"friendlyName"), L"");
                    monitor.slotAtSave =
                        JsonString(monitorValue.Find(L"slotAtSave"), L"");
                    monitor.primary =
                        JsonBool(monitorValue.Find(L"primary"), false);
                    monitor.relLeft =
                        JsonDouble(monitorValue.Find(L"relLeft"), 0);
                    monitor.relTop =
                        JsonDouble(monitorValue.Find(L"relTop"), 0);
                    monitor.relWidth =
                        JsonDouble(monitorValue.Find(L"relWidth"), 1);
                    monitor.relHeight =
                        JsonDouble(monitorValue.Find(L"relHeight"), 1);
                    monitor.pixelRect =
                        JsonRect(monitorValue.Find(L"pixelRect"));
                    monitor.workRect = JsonRect(monitorValue.Find(L"workRect"));
                    config.monitors.push_back(std::move(monitor));
                }
            }
            parsed.configs.push_back(std::move(config));
        }
    }

    const JsonValue* windows = root.Find(L"windows");
    if (windows && windows->type == JsonValue::Type::Array) {
        for (const JsonValue& entry : windows->array) {
            if (entry.type != JsonValue::Type::Object) {
                continue;
            }
            StoredWindow window;
            window.exePath = JsonString(entry.Find(L"exePath"), L"");
            window.className = JsonString(entry.Find(L"className"), L"");
            window.titleKey = JsonString(entry.Find(L"titleKey"), L"");
            window.ordinal = (int)JsonInt(entry.Find(L"ordinal"), 0);
            window.lastSeen = JsonInt(entry.Find(L"lastSeen"), 0);
            window.configId = (int)JsonInt(entry.Find(L"configId"), -1);
            window.monitorKey = JsonString(entry.Find(L"monitorKey"), L"");
            window.state = JsonString(entry.Find(L"state"), L"normal");
            window.dpiAtSave = (unsigned)JsonInt(entry.Find(L"dpiAtSave"), 0);
            window.rectPixels = JsonRect(entry.Find(L"rectPixels"));
            const JsonValue* normalized = entry.Find(L"rectNormalized");
            if (normalized && normalized->type == JsonValue::Type::Object) {
                window.rectNormalized.left =
                    JsonDouble(normalized->Find(L"l"), 0);
                window.rectNormalized.top =
                    JsonDouble(normalized->Find(L"t"), 0);
                window.rectNormalized.right =
                    JsonDouble(normalized->Find(L"r"), 1);
                window.rectNormalized.bottom =
                    JsonDouble(normalized->Find(L"b"), 1);
            }
            if (window.exePath.empty()) {
                continue;  // unusable entry
            }
            parsed.windows.push_back(std::move(window));
        }
    }

    if (parsed.nextConfigId <= 0) {
        parsed.nextConfigId = 1;
    }
    for (const StoredConfig& config : parsed.configs) {
        if (config.id >= parsed.nextConfigId) {
            parsed.nextConfigId = config.id + 1;
        }
    }

    store = std::move(parsed);
    return true;
}
// --- Restoring a maximized window -------------------------------------------
//
// A maximized window reports the *monitor's* rectangle, not the normal rectangle
// that was stored, so "is the rectangle where we want it?" is the wrong question
// for a maximized target. Acting on the wrong answer is what un-maximized
// windows in the field: the settle loop saw a mismatch, re-issued the move, and
// that move un-maximized the window it had just maximized.
//
// A maximized restore is therefore two phases with an observable checkpoint
// between them:
//
//   1. move the window to the stored rectangle (which is also what brings it to
//      the right monitor -- maximizing a window still on the old monitor just
//      maximizes it there),
//   2. once the move has landed, issue SW_MAXIMIZE and judge success by whether
//      the window is actually zoomed.
//
// Deciding this is pure logic, so it lives here and is covered by the offline
// tests rather than only being exercised on someone's desktop.
enum class SettleAction {
    Wait,        // nothing to do on this tick
    Move,        // (re-)issue the move to the stored rectangle
    Maximize,    // issue SW_MAXIMIZE
    Succeeded,   // the window is maximized on the intended monitor
    Overridden,  // gave up; leave the window alone for this session
};

struct MaximizeSettle {
    bool atStoredRect = false;    // observed rect matches the stored rectangle
    bool zoomed = false;          // IsZoomed
    bool onTargetMonitor = false; // window sits on the monitor it was stored for
    bool moveIssued = false;      // a move has been issued for this apply
    bool maximizeIssued = false;  // SW_MAXIMIZE has been issued for this apply
    bool maximizeRetried = false; // SW_MAXIMIZE has been re-issued once
    bool deadlinePassed = false;  // the current phase has run out of time
    int moveRetriesLeft = 0;      // retry budget for phase 1
};

static SettleAction DecideMaximizeSettle(const MaximizeSettle& state) {
    // Already maximized, and on the monitor it was saved for: done. A window
    // maximized on the *wrong* monitor still needs the move, which is why the
    // monitor is part of the question.
    if (state.zoomed && state.onTargetMonitor) {
        return SettleAction::Succeeded;
    }

    // Phase 1: get it onto the stored rectangle. A maximized window fails this
    // test by definition, so a window zoomed on the wrong monitor falls through
    // here and gets moved -- which un-maximizes it, as intended, after which
    // phase 2 re-maximizes it on the right monitor.
    if (!state.atStoredRect) {
        if (!state.moveIssued) {
            return SettleAction::Move;
        }
        if (!state.deadlinePassed) {
            return SettleAction::Wait;
        }
        return state.moveRetriesLeft > 0 ? SettleAction::Move
                                         : SettleAction::Overridden;
    }

    // Phase 2: it is where it belongs, so maximize it and confirm. The maximize
    // gets its own single retry: an application that resets its window right
    // after being shown is exactly the case a second attempt fixes, and unlike a
    // move, re-issuing it cannot un-maximize anything.
    if (!state.maximizeIssued) {
        return SettleAction::Maximize;
    }
    if (!state.deadlinePassed) {
        return SettleAction::Wait;
    }
    return state.maximizeRetried ? SettleAction::Overridden
                                 : SettleAction::Maximize;
}

// --- Display-change relayout -------------------------------------------------
//
// The second trigger for moving a window: the monitor layout changed while the
// window was already open. Restore-on-show returns a window to the geometry the
// user chose; relayout keeps an open window reachable and roughly where it was,
// taking the window's OWN current rectangle as the source of truth. It is a
// rescue, not a restore, which is why it reads nothing from the stored geometry
// except the DPI it was saved at.
//
// Everything here is decided before a single Win32 call, and is unit-tested the
// same way the restore rules are.

// Whether two monitor sets describe the same arrangement. Compared unordered, on
// identity and pixel rect -- the two things the relayout decision itself depends
// on, so "equivalent" means "no window's mapping can have changed" and the pass
// can be skipped whole. workRect is excluded for the same
// reason ConfigsMatch excludes it: a moved taskbar or a different work area is
// not a monitor change, and treating it as one would move every window on the
// desktop for nothing.
static bool MonitorSetsEquivalent(const std::vector<MonitorView>& a,
                                  const std::vector<MonitorView>& b) {
    if (a.size() != b.size()) {
        return false;
    }

    std::vector<bool> used(b.size(), false);
    for (const MonitorView& view : a) {
        std::wstring identity = MonitorIdentityOf(view);
        bool found = false;
        for (size_t i = 0; i < b.size(); i++) {
            if (used[i]) {
                continue;
            }
            if (MonitorIdentityOf(b[i]) == identity &&
                GeoRectsEqual(b[i].pixelRect, view.pixelRect)) {
                used[i] = true;
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }

    return true;
}

// The monitor a window was on before the change: the one it overlaps most. A
// partial overlap wins over "nearest", because that is the monitor the user saw
// the window on. Returns -1 when the window overlaps no monitor at all, in which
// case the pass leaves it alone -- a window we cannot place is not one we should
// move.
static int FindOriginMonitor(const std::vector<MonitorView>& monitors,
                             const GeoRect& rect) {
    long bestArea = 0;
    int bestIndex = -1;

    for (size_t i = 0; i < monitors.size(); i++) {
        const GeoRect& monitor = monitors[i].pixelRect;
        long overlapX = std::min(rect.right, monitor.right) -
                        std::max(rect.left, monitor.left);
        long overlapY = std::min(rect.bottom, monitor.bottom) -
                        std::max(rect.top, monitor.top);
        if (overlapX <= 0 || overlapY <= 0) {
            continue;
        }
        long area = overlapX * overlapY;
        if (area > bestArea) {
            bestArea = area;
            bestIndex = (int)i;
        }
    }

    return bestIndex;
}

// Which monitor a restore measures the work area and the maximize confirm
// against. Normally the monitor the geometry was stored for: that is where the
// window is going. With "Restore position" off the window stays where it opened,
// so the answer is the monitor the composed rectangle landed on -- clamping
// against the stored monitor instead would drag the window across the desktop,
// and the maximize confirm would wait for a monitor the window never reaches.
// A rectangle that overlaps no monitor keeps the stored one: a window that opened
// off-screen is the case the clamp exists for.
static int RestoreClampMonitor(const std::vector<MonitorView>& monitors,
                               int storedIndex, const GeoRect& composed,
                               bool restorePosition) {
    if (restorePosition || storedIndex < 0) {
        return storedIndex;
    }
    int landed = FindOriginMonitor(monitors, composed);
    return landed >= 0 ? landed : storedIndex;
}

// Whether a monitor with this identity is absent from the given set. "Gone" is
// the case that overrides everything else: it is the one situation where a
// window's placement is not a preference but a problem.
static bool MonitorIdentityIsGone(const std::vector<MonitorView>& current,
                                  const std::wstring& identity) {
    if (identity.empty()) {
        return false;
    }
    for (const MonitorView& view : current) {
        if (MonitorIdentityOf(view) == identity) {
            return false;
        }
    }
    return true;
}

// Where the origin monitor went, resolved with the exact five-step chain
// restore-on-show already uses: hardware path, fallback id, unique friendly
// name, arrangement relative to the primary, primary. The origin monitor's own
// identity, name and arrangement stand in for the stored config a restore would
// read, so there is one matching rule in the mod rather than two.
static MonitorResolution ResolveOriginMonitorTarget(
    const std::vector<MonitorView>& previous, int originIndex,
    const std::vector<MonitorView>& current) {
    MonitorResolution none;
    if (originIndex < 0 || (size_t)originIndex >= previous.size()) {
        return none;
    }

    const MonitorView& origin = previous[(size_t)originIndex];

    StoredMonitor stored;
    stored.key = origin.key;
    stored.fallbackId = origin.fallbackId;
    stored.friendlyName = origin.friendlyName;
    stored.pixelRect = origin.pixelRect;
    stored.workRect = origin.workRect;
    stored.primary = origin.primary;
    ComputeRelativeToPrimary(previous, (size_t)originIndex, stored.relLeft,
                             stored.relTop, stored.relWidth, stored.relHeight);

    StoredConfig config;
    config.monitors.push_back(std::move(stored));

    StoredWindow window;
    window.monitorKey = MonitorIdentityOf(origin);

    return ResolveTargetMonitor(current, window, &config);
}

// Why a window was not moved by the pass. Every one of these reaches the
// journal: "the mod did nothing" is the answer a reader most often needs, and
// it is invisible unless it is written down.
enum class RelayoutSkip {
    None,
    Overridden,
    Minimized,
    Unwritable,
    PendingApply,
    NoOrigin,
    NoTarget,
    MonitorUnchanged,
    Maximized,
    TargetEqualsCurrent,
};

static const wchar_t* RelayoutSkipReason(RelayoutSkip skip) {
    switch (skip) {
        case RelayoutSkip::Overridden:
            return L"overridden";
        case RelayoutSkip::Minimized:
            return L"minimized";
        case RelayoutSkip::Unwritable:
            return L"unwritable";
        case RelayoutSkip::PendingApply:
            return L"pending-apply";
        case RelayoutSkip::NoOrigin:
            return L"no-origin-monitor";
        case RelayoutSkip::NoTarget:
            return L"no-target-monitor";
        case RelayoutSkip::MonitorUnchanged:
            return L"monitor-unchanged";
        case RelayoutSkip::Maximized:
            return L"maximized";
        case RelayoutSkip::TargetEqualsCurrent:
            return L"no-move-needed";
        default:
            return L"none";
    }
}

struct RelayoutPlan {
    bool relayout = false;      // a move is needed
    bool originGone = false;    // the window's monitor is absent from the new set
    bool monitorUnchanged = false;
    int originIndex = -1;
    int targetIndex = -1;
    MonitorMatchRule rule = MonitorMatchRule::None;
    GeoRect originRect{};
    GeoRect target{};
};

// The per-window decision, from the pre-change snapshot and the live monitor
// set. The window's current rect is mapped with the same normalized mapping the
// restore path uses when the configuration does not match, for position and size.
static RelayoutPlan ComputeRelayoutPlan(const std::vector<MonitorView>& previous,
                                        const std::vector<MonitorView>& current,
                                        const GeoRect& currentRect) {
    RelayoutPlan plan;

    // Nothing to compare against (a pass before the first snapshot) or nothing
    // to map onto (a display change that left no monitors to enumerate).
    if (previous.empty() || current.empty()) {
        return plan;
    }

    plan.originIndex = FindOriginMonitor(previous, currentRect);
    if (plan.originIndex < 0) {
        return plan;
    }

    const MonitorView& origin = previous[(size_t)plan.originIndex];
    plan.originRect = origin.pixelRect;
    plan.originGone = MonitorIdentityIsGone(current, MonitorIdentityOf(origin));

    MonitorResolution resolution =
        ResolveOriginMonitorTarget(previous, plan.originIndex, current);
    if (resolution.index < 0) {
        return plan;
    }
    plan.targetIndex = resolution.index;
    plan.rule = resolution.rule;

    const MonitorView& target = current[(size_t)resolution.index];

    // The common case, and it must be silent rather than a no-op move: the same
    // monitor is still there, at the same place and the same scale. This is what
    // makes a duplicate WM_DISPLAYCHANGE cost nothing.
    plan.monitorUnchanged =
        !plan.originGone &&
        MonitorIdentityOf(target) == MonitorIdentityOf(origin) &&
        GeoRectsEqual(target.pixelRect, origin.pixelRect);
    if (plan.monitorUnchanged) {
        return plan;
    }

    plan.target = FromNormalized(ToNormalized(currentRect, origin.pixelRect),
                                 target.pixelRect);
    // Within the tolerance the apply path already treats as "landed": moving a
    // window to where it effectively already is would burn a settle cycle and
    // journal a RESTORE-shaped line for nothing.
    plan.relayout = !GeoRectsApproxEqual(plan.target, currentRect,
                                         kApplyTolerancePixels);
    return plan;
}

// What the Win32 layer knows about the window that the geometry cannot say.
struct RelayoutSubject {
    bool overridden = false;    // the user's placement already won this session
    bool pendingApply = false;  // a restore is still settling
    bool unwritable = false;    // a move was refused earlier this session
    bool minimized = false;
    bool maximized = false;
};

// First applicable reason wins, so the order is the journal's vocabulary:
// cheapest and most specific first.
static RelayoutSkip DecideRelayoutSkip(const RelayoutPlan& plan,
                                       const RelayoutSubject& subject) {
    if (subject.minimized) {
        // GetWindowRect on a minimized window returns the well-known bogus
        // off-screen rectangle, so there is no source rect to map.
        return RelayoutSkip::Minimized;
    }
    if (subject.unwritable) {
        // Nothing to do: the move would be refused again.
        return RelayoutSkip::Unwritable;
    }
    if (subject.pendingApply) {
        // Skipped rather than queued or cancelled: it will be reconsidered on
        // the next display change if it still matters.
        return RelayoutSkip::PendingApply;
    }
    if (subject.overridden && !plan.originGone) {
        // The user has already told the mod to leave this window alone, and its
        // monitor is still there -- that is a preference question the override
        // has answered. A monitor that is gone is a different question: "don't
        // fight the user" was never "let the window drift out of reach".
        return RelayoutSkip::Overridden;
    }
    if (plan.originIndex < 0) {
        return RelayoutSkip::NoOrigin;
    }
    if (plan.targetIndex < 0) {
        return RelayoutSkip::NoTarget;
    }
    if (plan.monitorUnchanged) {
        return RelayoutSkip::MonitorUnchanged;
    }
    if (!plan.relayout) {
        // Checked before the maximize rule on purpose: when the mapping lands the
        // window where it already is -- a resolution change on another monitor,
        // say -- "nothing needed doing" is the honest reason, and reporting
        // "maximized" instead would suggest it would have moved.
        return RelayoutSkip::TargetEqualsCurrent;
    }
    if (subject.maximized && !plan.originGone) {
        // Windows keeps a maximized window's monitor assignment sane while that
        // monitor still exists, and moving a maximized window un-maximizes it.
        // Deferred work, not an oversight: see the README's known limitations.
        return RelayoutSkip::Maximized;
    }
    return RelayoutSkip::None;
}

// What to do with the "this window's moves are refused" set.
// A window whose move was refused by UIPI still gets a DESTROY-time save, and
// that save replaces the geometry the user chose with wherever the window
// happened to be -- wrong for every later session. One refusal must silence that
// window's saves for the rest of the session, and nothing longer: a later
// successful apply, or the user placing the window by hand, ends it.
enum class UnwritableChange {
    None,
    Mark,
    Clear,
};

static UnwritableChange DecideUnwritableChange(bool applySucceeded,
                                               bool accessDenied,
                                               bool userMoved) {
    if (userMoved) {
        return UnwritableChange::Clear;
    }
    if (applySucceeded) {
        return UnwritableChange::Clear;
    }
    if (accessDenied) {
        return UnwritableChange::Mark;
    }
    return UnwritableChange::None;
}

// The DPI sanity field. Deliberately a log value and not a correction: the
// normalized mapping may already be DPI-correct, and adding an unverified
// multiply on top of a mechanism that might be right is how two wrongs cancel
// out in testing and reappear in the field. Logged as a ratio, with both
// "unknown" (nothing stored yet) and a true 1.00 kept apart.
static std::wstring FormatDpiRatio(unsigned dpiAtSave, unsigned currentDpi) {
    if (dpiAtSave == 0 || currentDpi == 0) {
        return L"unknown";
    }
    double ratio = (double)currentDpi / (double)dpiAtSave;
    int hundredths = (int)std::lround(ratio * 100.0);
    std::wstring text = std::to_wstring(hundredths / 100);
    text += L".";
    int fraction = hundredths % 100;
    if (fraction < 10) {
        text += L"0";
    }
    text += std::to_wstring(fraction);
    return text;
}

// <<< GEOMETRY_CORE <<<

// --- Settings helpers -------------------------------------------------------

static void ToLowerInPlace(std::wstring& value) {
    if (!value.empty()) {
        CharLowerBuffW(&value[0], (DWORD)value.size());
    }
}

static void LoadAllowlist() {
    g_allowlist.clear();

    for (int i = 0;; i++) {
        // An entry with nothing in it ends the list. Wh_GetStringSetting never
        // returns NULL: an entry that is not there comes back as an empty string,
        // which is also how the settings template's empty first entry means
        // "track nothing".
        PCWSTR exe = Wh_GetStringSetting(L"allowlist[%d].Exe", i);
        if (!*exe) {
            Wh_FreeStringSetting(exe);
            break;
        }

        AllowlistEntry entry;
        entry.exe = exe;
        Wh_FreeStringSetting(exe);
        ToLowerInPlace(entry.exe);

        // Read for every entry, so that a flag left at its default and a flag
        // explicitly set to the same value are indistinguishable -- which is what
        // the settings UI shows.
        entry.restorePosition =
            Wh_GetIntSetting(L"allowlist[%d].RestorePosition", i) != 0;
        entry.restoreSize =
            Wh_GetIntSetting(L"allowlist[%d].RestoreSize", i) != 0;

        g_allowlist.push_back(std::move(entry));
    }

    g_allowlistIsEmpty = g_allowlist.empty();
}

// The matched entry, or nullptr. Returning the entry rather than a bool is what
// carries the per-app flags into the identity: this is the one place that knows
// both which window this is and what the user asked for it.
static const AllowlistEntry* FindAllowlistEntry(const std::wstring& exePath,
                                                const std::wstring& exeName) {
    std::wstring lowerPath = exePath;
    std::wstring lowerName = exeName;
    ToLowerInPlace(lowerPath);
    ToLowerInPlace(lowerName);

    for (const AllowlistEntry& entry : g_allowlist) {
        if (entry.exe == lowerName || entry.exe == lowerPath) {
            return &entry;
        }
    }

    return nullptr;
}

// --- Window inspection ------------------------------------------------------

static std::wstring BaseNameOf(const std::wstring& path) {
    size_t separator = path.find_last_of(L"\\/");
    return separator == std::wstring::npos ? path : path.substr(separator + 1);
}

// GetWindowTextW does not send WM_GETTEXT to windows owned by other processes
// (it reads the cached caption), so this cannot block on a hung app.
static std::wstring GetWindowTitle(HWND hwnd) {
    int length = GetWindowTextLengthW(hwnd);
    if (length <= 0) {
        return std::wstring();
    }

    std::wstring title((size_t)length, L'\0');
    int copied = GetWindowTextW(hwnd, &title[0], length + 1);
    if (copied <= 0) {
        return std::wstring();
    }

    title.resize((size_t)copied);
    return title;
}

// *reason receives a short human-readable explanation of the first rule that
// rejected the window, so a log line answers "why was my window not tracked?"
// without anyone having to read this function.
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
    // Hidden windows are framework scaffolding, not app windows. WinUI 3 and
    // Windows App SDK apps create several such top-level windows per window
    // (input, composition and adapter windows); they are never shown, so they
    // are rejected here and the real window is picked up on its SHOW event.
    if (!IsWindowVisible(hwnd)) {
        *reason = L"not visible";
        return false;
    }
    // Cloaked windows are invisible to the user: suspended UWP apps, XAML
    // islands on another virtual desktop, and similar. Moving them achieves
    // nothing and can disturb the owning app.
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

// Cheap rejections (process identity and the allowlist) happen before the class
// and title work, so windows we are about to discard never pay for string
// normalization.
static bool ResolveIdentity(HWND hwnd, WindowIdentity& identity,
                            const wchar_t** reason) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (!processId || processId == GetCurrentProcessId()) {
        *reason = L"no process id";
        return false;
    }

    // PROCESS_QUERY_LIMITED_INFORMATION succeeds across integrity levels, so
    // elevated apps resolve here. A failure means a protected process, which is
    // out of scope for this mod.
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

    std::wstring exePath(path);
    std::wstring exeName = BaseNameOf(exePath);

    for (PCWSTR excluded : kExcludedProcesses) {
        if (_wcsicmp(exeName.c_str(), excluded) == 0) {
            *reason = L"process is on the built-in exclude list";
            return false;
        }
    }

    const AllowlistEntry* allowlistEntry = FindAllowlistEntry(exePath, exeName);
    if (!allowlistEntry) {
        *reason = L"not in the allowlist (add the app under 'Apps to manage')";
        return false;
    }

    WCHAR className[256]{};
    int classNameLength =
        GetClassNameW(hwnd, className, ARRAYSIZE(className));
    if (classNameLength <= 0) {
        *reason = L"no window class";
        return false;
    }

    // A window with no title is not something a user opens, moves and closes:
    // every scaffolding window seen while this was written (adapter, input site,
    // GDI+ hook) had an empty title, while every real window had one. A window whose
    // title arrives after it is shown is picked up later, on MOVESIZEEND.
    std::wstring title = NormalizeWindowTitle(GetWindowTitle(hwnd));
    if (title.empty()) {
        *reason = L"empty title";
        return false;
    }

    identity.exePath = exePath;
    identity.exeName = exeName;
    identity.className.assign(className, (size_t)classNameLength);
    identity.normalizedTitle = std::move(title);
    identity.ordinal = NextOrdinal(identity);
    // The per-app switches travel with the window from here on, so the restore and
    // the relayout both see the entry that matched rather than re-reading the
    // settings from the event thread.
    identity.restorePosition = allowlistEntry->restorePosition;
    identity.restoreSize = allowlistEntry->restoreSize;
    return true;
}

// --- Logging ----------------------------------------------------------------

static PCWSTR EventName(DWORD event) {
    switch (event) {
        case EVENT_OBJECT_CREATE:
            return L"CREATE";
        case EVENT_OBJECT_SHOW:
            return L"SHOW";
        case EVENT_OBJECT_DESTROY:
            return L"DESTROY";
        case EVENT_SYSTEM_MOVESIZEEND:
            return L"MOVESIZEEND";
        default:
            return L"UNKNOWN";
    }
}

// Bounded diagnostic: without it, a window that is not tracked leaves no trace
// at all and the only way to find out why is to guess. Bounded because WinUI
// apps create dozens of scaffolding windows each time they start.
//
// The budget is per REASON, not per session. A session-wide cap of 20 lines was
// exhausted within seconds by whatever appeared on the desktop -- the hook sees
// every window in the system, not just the app being investigated -- and from
// then on every skip was silent, including the one reason the user had turned
// the journal on to find. One chatty reason must not be able to silence the
// rest.
static constexpr int kMaxSkipLogLinesPerReason = 10;
static std::map<std::wstring, int> g_skipLogCounts;

// Three outcomes, so that both bounded logs can share one policy: log this
// line, write the one-off "that is all of them" notice for this reason, or stay
// silent. The budget belongs to the reason, never to the session.
enum class SkipLogBudget {
    Log,
    Notice,
    Silent,
};

static SkipLogBudget TakeSkipLogBudget(const std::wstring& reason) {
    int& count = g_skipLogCounts[reason];
    if (count < kMaxSkipLogLinesPerReason) {
        count++;
        return SkipLogBudget::Log;
    }
    if (count == kMaxSkipLogLinesPerReason) {
        count++;
        return SkipLogBudget::Notice;
    }
    return SkipLogBudget::Silent;
}

static void LogSkippedWindow(HWND hwnd, PCWSTR reason) {
    SkipLogBudget budget = TakeSkipLogBudget(reason);
    if (budget == SkipLogBudget::Notice) {
        Wh_Log(L"[GEOM] further skips for reason '%s' are not logged", reason);
        AppendDiagnosticsJournal(L"SKIP reason=" + std::wstring(reason) +
                                 L" (further skips for this reason are not "
                                 L"logged)");
        return;
    }
    if (budget == SkipLogBudget::Silent) {
        return;
    }

    WCHAR className[256]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    Wh_Log(L"[GEOM] skip class=%s reason=%s styles=0x%08lX exStyles=0x%08lX "
           L"visible=%d",
           className, reason, (unsigned long)GetWindowLongPtrW(hwnd, GWL_STYLE),
           (unsigned long)GetWindowLongPtrW(hwnd, GWL_EXSTYLE),
           (int)IsWindowVisible(hwnd));
    AppendDiagnosticsJournal(L"SKIP class=" + std::wstring(className) +
                             L" reason=" + reason);
}

// The relayout pass walks every tracked window, so it needs the same bound as
// the filter skip log above. The reason tokens are prefixed so that the two logs
// cannot silence each other by colliding on a key.
static void LogRelayoutSkip(const WindowIdentity& identity, RelayoutSkip skip) {
    std::wstring reason = RelayoutSkipReason(skip);
    SkipLogBudget budget = TakeSkipLogBudget(L"relayout:" + reason);
    if (budget == SkipLogBudget::Silent) {
        return;
    }

    Wh_Log(L"[GEOM] RELAYOUT-SKIP exe=%s ordinal=%d reason=%s",
           identity.exeName.c_str(), identity.ordinal, reason.c_str());

    if (budget == SkipLogBudget::Notice) {
        AppendDiagnosticsJournal(L"RELAYOUT-SKIP reason=" + reason +
                                 L" (further skips for this reason are not "
                                 L"logged)");
        return;
    }

    AppendDiagnosticsJournal(L"RELAYOUT-SKIP exe=" + identity.exeName +
                             L" ordinal=" + std::to_wstring(identity.ordinal) +
                             L" reason=" + reason);
}

static void LogWindowEvent(PCWSTR eventName, HWND hwnd,
                           const WindowIdentity& identity) {
    RECT rect{};
    GetWindowRect(hwnd, &rect);

    std::wstring title = NormalizeWindowTitle(GetWindowTitle(hwnd));

    Wh_Log(L"[GEOM] %s exe=%s class=%s title=\"%s\" ordinal=%d "
           L"rect=(%ld,%ld,%ld,%ld) hwnd=%p",
           eventName, identity.exeName.c_str(), identity.className.c_str(),
           title.c_str(), identity.ordinal, rect.left, rect.top, rect.right,
           rect.bottom, (void*)hwnd);
}

// --- Monitor enumeration ----------------------------------------------------

// Current monitor set, refreshed at startup and on WM_DISPLAYCHANGE.
static std::vector<MonitorView> g_monitors;

// The same set as it was before the current display change: what the relayout
// pass compares the live set against, so it can tell what a window's monitor
// used to be. Built at startup, and replaced only once a pass has finished -- a
// pass interrupted halfway must not leave the mod comparing against a mix of old
// and new state on the next event.
static std::vector<MonitorView> g_lastKnownMonitors;

static bool BuildMonitorViews(std::vector<MonitorView>& monitors) {
    monitors.clear();

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM data) CALLBACK -> BOOL {
            auto* list = reinterpret_cast<std::vector<MonitorView>*>(data);

            MONITORINFOEXW info{};
            info.cbSize = sizeof(info);
            if (!GetMonitorInfoW(monitor, &info)) {
                return TRUE;
            }

            MonitorView view;
            view.gdiName = info.szDevice;
            view.pixelRect = {info.rcMonitor.left, info.rcMonitor.top,
                              info.rcMonitor.right, info.rcMonitor.bottom};
            view.workRect = {info.rcWork.left, info.rcWork.top,
                             info.rcWork.right, info.rcWork.bottom};
            view.primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;

            // The stable identity is the device interface path
            // (\\?\DISPLAY#DELA1C5#4&...#{...}), which survives slot
            // renumbering; \\.\DISPLAYn is kept for mapping and diagnostics
            // only. See MonitorIdentityOf in the core block.
            DISPLAY_DEVICEW device{};
            device.cb = sizeof(device);
            if (EnumDisplayDevicesW(info.szDevice, 0, &device,
                                    EDD_GET_DEVICE_INTERFACE_NAME)) {
                view.key = device.DeviceID;
                view.friendlyName = device.DeviceString;
            }
            if (view.key.empty()) {
                DISPLAY_DEVICEW plain{};
                plain.cb = sizeof(plain);
                if (EnumDisplayDevicesW(info.szDevice, 0, &plain, 0)) {
                    view.fallbackId = plain.DeviceID;
                    if (view.friendlyName.empty()) {
                        view.friendlyName = plain.DeviceString;
                    }
                }
            }

            list->push_back(std::move(view));
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&monitors));

    return !monitors.empty();
}

static void RefreshMonitorViews() {
    std::vector<MonitorView> monitors;
    if (BuildMonitorViews(monitors)) {
        g_monitors = std::move(monitors);
    } else {
        Wh_Log(L"[GEOM] monitor enumeration returned nothing");
    }
}

static int FindMonitorIndexByHandle(HMONITOR monitor) {
    // The cache stores no HMONITOR (which would go stale), so match by GDI name.
    if (!monitor) {
        return -1;
    }

    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return -1;
    }

    for (size_t i = 0; i < g_monitors.size(); i++) {
        if (g_monitors[i].gdiName == info.szDevice) {
            return (int)i;
        }
    }
    return -1;
}

// --- Geometry store I/O -----------------------------------------------------

// --- Per-user storage -------------------------------------------------------
//
// Wh_GetModStoragePath is a single folder for the whole machine, and a tool mod
// runs one process per signed-in session. The files carry executable paths and
// window titles, so two users must neither share them nor be able to read each
// other's -- which are two different requirements:
//
//  * Two sessions signed in at once must not overwrite each other's store. A
//    shared file name cannot keep them apart; the layout has to.
//  * A file is readable by whoever can open the folder it is in, and the mod's
//    storage folder is readable by every user on the machine. A SID in a file
//    name does not change that; it only names the file after its reader.
//
// So each user gets a folder of their own: <storage>\<SID>, created with a
// protected DACL that gives full access to that user, SYSTEM and administrators,
// and inherits nothing from the shared folder. What is inside is private by
// construction, whatever the files are called. This is the same shape as
// desktop-sticky-notes and the same three entries, built through the ACL
// functions rather than parsed from an SDDL string: building the entries
// directly is a shorter path to the same descriptor, and it keeps a
// string-to-descriptor conversion out of a path that runs on every load.
// sddl.h is available to a mod; this is a choice, not a limitation.
//
// The SID comes from this process's own token, and its text form is written out
// by hand rather than with a call into ConvertSidToStringSidW -- see
// FormatSidString for why.

// Reads the bytes of a SID into the parts FormatSidString renders. Kept out of the
// tested core because it is the only part that has to know what a SID looks like;
// SID and SID_IDENTIFIER_AUTHORITY are declared in winnt.h, which windows.h pulls
// in, so unlike ConvertSidToStringSidW this needs no header of its own.
//
// The count is checked against SID_MAX_SUB_AUTHORITIES (15) before anything is read
// through it: this is a pointer into a token buffer, and a count that large can only
// mean the buffer is not what it claims to be.
static std::wstring SidToString(PSID sid) {
    if (!sid) {
        return std::wstring();
    }

    const SID* parsed = reinterpret_cast<const SID*>(sid);
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

// The signed-in user's SID in both forms the mod needs: the bytes, for the
// folder's access control list, and the text, for the folder name.
//
// Read once and cached: the SID cannot change while the process lives, and the
// readers run on two threads (init and the event thread).
struct UserTokenSid {
    std::vector<BYTE> bytes;
    std::wstring text;
};

static const UserTokenSid& CurrentUserTokenSid() {
    static bool attempted = false;
    static UserTokenSid cached;

    if (attempted) {
        return cached;
    }
    attempted = true;

    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        Wh_Log(L"[GEOM] OpenProcessToken failed (error %lu)", GetLastError());
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

    if (cached.text.empty()) {
        // Without a SID there is no folder that can be made private and no name
        // that keeps two sessions apart. The mod saves nothing at all rather than
        // putting the data back in a file every user can read: it remembers
        // nothing across a restart, and that is the best available answer.
        Wh_Log(L"[GEOM] the user SID could not be read; this session will not "
               L"save any geometry");
    }

    return cached;
}

static std::wstring CurrentUserSid() {
    return CurrentUserTokenSid().text;
}

// Built once: the latch key is used on every settings change. Empty means no SID,
// which also means no folder and nothing saved, so the action has nothing to do.
static const std::wstring& PerUserLatchValueName() {
    static bool resolved = false;
    static std::wstring name;
    if (!resolved) {
        resolved = true;
        const std::wstring sid = CurrentUserSid();
        if (!sid.empty()) {
            name = std::wstring(kClearLatchValueName) + L"-" + sid;
        }
    }
    return name;
}

// The access control list of a user's private folder, or of a file inside it: full
// access for that user, for SYSTEM and for administrators -- the last two because
// Windhawk has to be able to remove the folder along with the mod -- and for nobody
// else.
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

    // Inheritance is for the folder: it carries the same three entries to the files
    // created inside it. A file has no children, and the bits would show up in an
    // ACL query, so the file form is written without them.
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
// writable by every account, so another user could create this folder first with a
// list of their own, and every file written inside would inherit it. SYSTEM and
// administrators are accepted as well, because a tool process that runs elevated
// creates objects owned by the Administrators group rather than by the account
// (the token's default owner), and both of those identities already have full
// control through the folder's own list and can take ownership of anything on the
// volume anyway. Neither is a way in for another standard user.
static bool FolderOwnerIsCurrentUser(const std::wstring& path,
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

// Creates this user's folder, with a protected access control list of its own.
// Files created inside inherit that list, so nothing has to be re-permissioned
// later; a file that arrives any other way is not written here at all.
static bool EnsurePrivateFolder(const std::wstring& path,
                                const std::vector<BYTE>& userSid) {
    std::vector<BYTE> aclBuffer;
    if (!BuildPrivateAcl(userSid, /*inheritToChildren=*/true, aclBuffer)) {
        Wh_Log(L"[GEOM] could not build the private folder's access control list");
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
        // Named explicitly rather than left to the token's default owner, which is
        // the Administrators group whenever the tool process runs elevated. The
        // folder would then be owned by Administrators for the rest of the install,
        // and the owner check below would refuse it on every later start.
        !SetSecurityDescriptorOwner(
            &descriptor,
            reinterpret_cast<PSID>(const_cast<BYTE*>(userSid.data())),
            FALSE)) {
        Wh_Log(L"[GEOM] could not build the private folder's security descriptor");
        return false;
    }

    SECURITY_ATTRIBUTES attributes{sizeof(attributes), &descriptor, FALSE};
    if (CreateDirectoryW(path.c_str(), &attributes)) {
        return true;
    }

    const DWORD error = GetLastError();
    if (error == ERROR_ALREADY_EXISTS) {
        if (FolderOwnerIsCurrentUser(path, userSid)) {
            return true;
        }
        Wh_Log(L"[GEOM] %s already exists and is not this user's; this session "
               L"will not save any geometry", path.c_str());
        return false;
    }

    Wh_Log(L"[GEOM] could not create %s (error %lu); this session will not save "
           L"any geometry", path.c_str(), error);
    return false;
}


// This user's data folder, created on first use. Empty means "not available",
// and every caller reads that as "do not save".
//
// Resolved once, from WhTool_ModInit before the event thread exists, so the
// folder and the SID behind it are initialized by a single thread; everything
// else only reads the result.
static const std::wstring& UserDataFolder() {
    static bool attempted = false;
    static std::wstring cached;

    if (attempted) {
        return cached;
    }
    attempted = true;

    const UserTokenSid& sid = CurrentUserTokenSid();
    if (sid.text.empty()) {
        return cached;
    }

    WCHAR storage[MAX_PATH];
    const size_t len = Wh_GetModStoragePath(storage, ARRAYSIZE(storage));
    if (len == 0 || len + 1 >= ARRAYSIZE(storage)) {
        Wh_Log(L"Failed to get the mod storage path");
        return cached;
    }

    std::wstring folder = storage;
    folder += L"\\";
    folder += sid.text;

    if (EnsurePrivateFolder(folder, sid.bytes)) {
        cached = std::move(folder);
    }

    return cached;
}

// A file inside this user's folder. False means the folder is not available (no
// SID, or it could not be created), and then nothing is written or read.
static bool BuildUserFilePath(PCWSTR fileName, PWSTR buffer, size_t bufferSize) {
    const std::wstring& folder = UserDataFolder();
    if (folder.empty()) {
        return false;
    }
    return swprintf_s(buffer, bufferSize, L"%s\\%s", folder.c_str(), fileName) >=
           0;
}

static bool BuildGeometryStorePath(PWSTR buffer, size_t bufferSize) {
    return BuildUserFilePath(kGeometryStoreFileName, buffer, bufferSize);
}




static constexpr DWORD kStoreFlushDelayMs = 2000;
static constexpr UINT kSettleTickMs = 100;
static constexpr UINT_PTR kSettleTimerId = 1;
static constexpr ULONGLONG kSettleWindowMs = 800;

// The relayout debounce. One topology change arrives as several
// WM_DISPLAYCHANGE messages, and running the whole pass on each of them would
// spend the settle/retry budget several times over and can flicker a window
// through two intermediate rectangles for one real event. The pass waits for
// quiet instead: a single-shot timer, reset rather than stacked, on the mod's
// own message window. The monitor cache still refreshes immediately.
static constexpr UINT kRelayoutDebounceMs = 500;
static constexpr UINT_PTR kRelayoutTimerId = 2;

// A restore in flight. "Our own move" is decided geometrically -- within
// kApplyTolerancePixels of the requested rect while the record exists -- which is
// what makes "our move landed and the app then adjusted itself" (retry once)
// distinguishable from "the user grabbed the window" (never retry, never undo).
// A boolean or a window property could not answer the retry question, and a
// property would additionally be inherited by a recycled HWND after a crash.
struct PendingApply {
    GeoRect target;
    ULONGLONG deadline = 0;
    int retriesLeft = 0;
    bool maximize = false;
    bool observedTarget = false;
    GeoRect lastObserved;
    // Maximize-specific bookkeeping. See DecideMaximizeSettle: a maximized
    // restore is two phases, and each phase is issued at most once plus one
    // retry, so the mod never re-issues a move for a window it just maximized.
    bool moveIssued = false;
    bool maximizeIssued = false;
    bool maximizeRetried = false;
    GeoRect targetMonitorRect;
    std::wstring identityKey;  // matches the g_overrideWindows key space
    std::wstring exeName;      // for the verdict line
};

static GeometryStore g_store;
static bool g_storeDirty = false;
static ULONGLONG g_storeLastChange = 0;
static UINT_PTR g_settleTimer = 0;
// The mod's own (message-only) window, on the event thread. Timers must be
// created against this window rather than against a tracked one: SetTimer
// requires a window owned by the calling thread, and tracked windows belong to
// other processes.
static std::atomic<HWND> g_messageWindow{nullptr};
static std::unordered_map<HWND, PendingApply> g_pendingApplies;
static std::unordered_set<std::wstring> g_overrideWindows;
static std::atomic<bool> g_clearStoreRequested{false};

// Identities whose moves were refused this session. A refusal
// is not only a failed restore: the DESTROY-time save that follows it would
// record the position the window happened to be at as if the user had chosen it,
// and the next session would open there. Session state, exactly like
// g_overrideWindows, and erased as soon as the refusal no longer holds -- a later
// successful apply, or the user placing the window by hand.
static std::unordered_set<std::wstring> g_unwritableWindows;

// Single-shot debounce timer for the relayout pass (see kRelayoutDebounceMs).
static UINT_PTR g_relayoutTimer = 0;

static std::wstring Utf8ToWide(const std::string& text) {
    if (text.empty()) {
        return std::wstring();
    }
    int length = MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(),
                                     nullptr, 0);
    if (length <= 0) {
        return std::wstring();
    }
    std::wstring wide((size_t)length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), &wide[0],
                        length);
    return wide;
}

static std::string WideToUtf8(const std::wstring& text) {
    if (text.empty()) {
        return std::string();
    }
    int length = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(),
                                     nullptr, 0, nullptr, nullptr);
    if (length <= 0) {
        return std::string();
    }
    std::string narrow((size_t)length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), &narrow[0],
                        length, nullptr, nullptr);
    return narrow;
}

// The Win32 file API rather than <fstream>: the standard streams take a narrow
// path, which would break any profile directory with non-ASCII characters in it.
static bool ReadUtf8File(const WCHAR* path, std::string& out) {
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    out.clear();
    char buffer[8192];
    DWORD read = 0;
    while (ReadFile(file, buffer, sizeof(buffer), &read, nullptr) && read > 0) {
        out.append(buffer, (size_t)read);
        if (read < sizeof(buffer)) {
            break;
        }
    }

    CloseHandle(file);
    return true;
}

static bool WriteUtf8File(const WCHAR* path, const std::string& data) {
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    bool ok = true;
    if (!data.empty()) {
        DWORD written = 0;
        ok = WriteFile(file, data.data(), (DWORD)data.size(), &written,
                       nullptr) &&
             written == data.size();
    }

    CloseHandle(file);
    return ok;
}

// --- Diagnostics journal ----------------------------------------------------
//
// The stored geometry file is a poor thing to assert against from outside: it is
// written on a two-second debounce, overwritten in place, and shared by every
// tracked application, so anything reading it can easily read the wrong moment
// or the wrong entry. Validation kept tripping over exactly that.
//
// The journal is the opposite: one line appended at the instant of each decision,
// nothing debounced, nothing overwritten. Off by default because it is for
// diagnosis, not for normal use.
static bool g_diagnosticsLog = false;

// Journalled at startup, so that a journal line names one exact build: a stale copy
// once looked complete while the lines being looked for were missing from it, which
// cost a round of diagnosis. It identifies a verified behaviour, not every text
// revision, so a run can be tied to the field result it belongs to.
static constexpr PCWSTR kJournalBuild = L"r25-2026-10-16";

// The journal flag has to be known before the store is loaded, because the load
// itself journals a line. Reading it only in ApplySettingsOnEventThread -- which
// runs after LoadGeometryStore -- meant the load's line was silently dropped,
// and anything waiting for it (the validation script) waited forever.
static void RefreshDiagnosticsSetting() {
    g_diagnosticsLog = Wh_GetIntSetting(L"diagnosticsLog") != 0;
}

static bool BuildJournalPath(PWSTR buffer, size_t bufferSize) {
    return BuildUserFilePath(kDiagnosticsJournalFileName, buffer, bufferSize);
}

// A diagnostics log that is never trimmed is a file that grows for the life of
// the install. Past this size it starts over: the journal is for the last thing
// that happened, not for history.
static constexpr long long kMaxJournalBytes = 1024 * 1024;

// Never fails loudly: a diagnostics log that cannot be written must not affect
// the behaviour it is meant to observe.
static void AppendDiagnosticsJournal(const std::wstring& line) {
    if (!g_diagnosticsLog) {
        return;
    }

    WCHAR path[MAX_PATH];
    if (!BuildJournalPath(path, ARRAYSIZE(path))) {
        return;
    }

    HANDLE file = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    bool truncated = false;
    LARGE_INTEGER size{};
    if (GetFileSizeEx(file, &size) && size.QuadPart > kMaxJournalBytes) {
        SetFilePointer(file, 0, nullptr, FILE_BEGIN);
        truncated = SetEndOfFile(file) != FALSE;
    } else {
        SetFilePointer(file, 0, nullptr, FILE_END);
    }

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

static void LoadGeometryStore() {
    g_store = GeometryStore();


    WCHAR path[MAX_PATH];
    if (!BuildGeometryStorePath(path, ARRAYSIZE(path))) {
        return;
    }

    std::string json;
    if (!ReadUtf8File(path, json)) {
        Wh_Log(L"[GEOM] no stored geometry yet");
        // Journalled for the same reason as the successful load below: a reader
        // waiting to learn that the store was read must not have to distinguish
        // "no file" from "the line never came".
        AppendDiagnosticsJournal(L"STORE-LOADED entries=0 (no store file)");
        return;
    }

    if (!DeserializeGeometryStore(Utf8ToWide(json), g_store)) {
        // Do not silently overwrite: keep the unreadable file for inspection and
        // start from an empty store. The next save writes the fresh one.
        std::wstring quarantine = std::wstring(path) + L".corrupt";
        if (MoveFileExW(path, quarantine.c_str(), MOVEFILE_REPLACE_EXISTING)) {
            Wh_Log(L"[GEOM] stored geometry is unreadable; kept as %s",
                   quarantine.c_str());
        } else {
            Wh_Log(L"[GEOM] stored geometry is unreadable and could not be "
                   L"quarantined");
        }
        g_store = GeometryStore();
        return;
    }

    Wh_Log(L"[GEOM] loaded %d window entries, %d configurations",
           (int)g_store.windows.size(), (int)g_store.configs.size());
    AppendDiagnosticsJournal(L"STORE-LOADED entries=" +
                             std::to_wstring(g_store.windows.size()));
}

static bool WriteGeometryStore() {
    WCHAR path[MAX_PATH];
    if (!BuildGeometryStorePath(path, ARRAYSIZE(path))) {
        return false;
    }

    std::wstring serialized = SerializeGeometryStore(g_store);
    std::string narrow = WideToUtf8(serialized);

    // Write to a temporary file and replace: a crash mid-write must never leave
    // a truncated store behind.
    std::wstring temporary = std::wstring(path) + L".tmp";
    if (!WriteUtf8File(temporary.c_str(), narrow)) {
        Wh_Log(L"[GEOM] failed to write the store");
        return false;
    }

    if (!MoveFileExW(temporary.c_str(), path,
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"[GEOM] failed to replace the store (error %lu)", GetLastError());
        DeleteFileW(temporary.c_str());
        return false;
    }

    return true;
}

static void MarkStoreDirty() {
    // Nothing is written without the private folder, so do not arm the flush timer
    // for a save that cannot happen: it would keep waking every 100 ms and fail to
    // write every 2 s for the rest of the session.
    if (UserDataFolder().empty()) {
        return;
    }

    g_storeDirty = true;
    g_storeLastChange = GetTickCount64();
}

static const StoredConfig* FindConfigById(const GeometryStore& store, int id) {
    for (const StoredConfig& config : store.configs) {
        if (config.id == id) {
            return &config;
        }
    }
    return nullptr;
}

static void SnapshotMonitors(const std::vector<MonitorView>& monitors,
                             std::vector<StoredMonitor>& stored) {
    stored.clear();
    for (size_t i = 0; i < monitors.size(); i++) {
        StoredMonitor monitor;
        monitor.key = monitors[i].key;
        monitor.fallbackId = monitors[i].fallbackId;
        monitor.friendlyName = monitors[i].friendlyName;
        monitor.slotAtSave = monitors[i].gdiName;
        monitor.pixelRect = monitors[i].pixelRect;
        monitor.workRect = monitors[i].workRect;
        monitor.primary = monitors[i].primary;
        ComputeRelativeToPrimary(monitors, i, monitor.relLeft, monitor.relTop,
                                 monitor.relWidth, monitor.relHeight);
        stored.push_back(std::move(monitor));
    }
}

// Returns the id of the configuration matching the current monitors, creating
// one if this arrangement has not been seen before.
static int FindOrCreateConfigId() {
    long long now = (long long)time(nullptr);

    for (StoredConfig& config : g_store.configs) {
        if (ConfigsMatch(g_monitors, config.monitors)) {
            config.lastSeen = now;
            return config.id;
        }
    }

    StoredConfig config;
    config.id = (int)g_store.nextConfigId++;
    config.lastSeen = now;
    SnapshotMonitors(g_monitors, config.monitors);
    int id = config.id;
    g_store.configs.push_back(std::move(config));
    return id;
}

// The monitor a window is currently on. Used when reading the window's own
// rectangle, which the positions and sizes have to be measured against.
static const MonitorView* CurrentMonitorView(HWND hwnd) {
    HMONITOR handle = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    int index = FindMonitorIndexByHandle(handle);
    if (index < 0) {
        // Stale cache, e.g. a display change that has not been processed yet.
        RefreshMonitorViews();
        index = FindMonitorIndexByHandle(handle);
    }
    return index >= 0 ? &g_monitors[(size_t)index] : nullptr;
}

// The rectangle that represents where a window is, in the coordinates this mod
// stores and applies: a normal window's screen rectangle, or the normal rectangle
// of a minimized or maximized one -- converted out of workspace coordinates, which
// is the difference between a window that stays put and one that drifts down the
// screen on a taskbar-at-the-top machine.
//
// *showCommand receives the placement's show state when the rectangle came from
// GetWindowPlacement, and is left alone otherwise; the save path uses it to record
// whether the window was maximized.
static bool WindowOwnRect(HWND hwnd, GeoRect& rect, UINT* showCommand,
                          UINT* placementFlags = nullptr) {
    if (IsIconic(hwnd) || IsZoomed(hwnd)) {
        WINDOWPLACEMENT placement{};
        placement.length = sizeof(placement);
        if (!GetWindowPlacement(hwnd, &placement)) {
            return false;
        }

        // The monitor rectangles come from Windows, not from the monitor cache.
        // The cache is refreshed when a window sits on a monitor it has not seen,
        // and a refresh replaces the vector: a caller holding a reference into the
        // old one would be reading freed memory. Asking for the two rectangles
        // directly keeps this helper -- and everyone who calls it -- out of that.
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        if (!GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                             &info)) {
            return false;
        }

        const GeoRect pixelRect{info.rcMonitor.left, info.rcMonitor.top,
                                info.rcMonitor.right, info.rcMonitor.bottom};
        const GeoRect workRect{info.rcWork.left, info.rcWork.top,
                               info.rcWork.right, info.rcWork.bottom};

        const GeoRect normal{placement.rcNormalPosition.left,
                             placement.rcNormalPosition.top,
                             placement.rcNormalPosition.right,
                             placement.rcNormalPosition.bottom};
        rect = PlacementRectToScreen(normal, pixelRect, workRect);

        if (showCommand) {
            *showCommand = placement.showCmd;
        }
        if (placementFlags) {
            *placementFlags = placement.flags;
        }
        return true;
    }

    RECT raw{};
    if (!GetWindowRect(hwnd, &raw)) {
        return false;
    }
    rect = {raw.left, raw.top, raw.right, raw.bottom};
    return true;
}

// Whether the window is maximized on this monitor. Used to recognise a restore
// that has nothing to do: an application that opens maximized by itself is
// already where the user left it, and moving it to the remembered normal
// rectangle first -- which is what the settle sequence does -- un-maximizes it
// and maximizes it again, a visible bounce on every launch.
static bool WindowIsZoomedOn(HWND hwnd, const GeoRect& monitorRect) {
    if (!IsZoomed(hwnd)) {
        return false;
    }

    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
                         &info)) {
        return false;
    }

    return info.rcMonitor.left == monitorRect.left &&
           info.rcMonitor.top == monitorRect.top &&
           info.rcMonitor.right == monitorRect.right &&
           info.rcMonitor.bottom == monitorRect.bottom;
}

// Applies an app's position/size switches to a target rectangle, using the
// window's own rectangle as the other operand. Shared by the restore and the
// relayout, so the two cannot disagree about what "keep the current size" means.
//
// positionRequired is the one thing that can override "Restore position" being
// off: a window whose monitor has just disappeared has to be moved onto a monitor
// that exists, or it becomes unreachable. That move is a rescue, not a remembered
// position, so it is not the setting's business -- and it is the only case where
// the setting loses.
static GeoRect ApplyOwnRestoreFlags(HWND hwnd, const WindowIdentity& identity,
                                    const GeoRect& target,
                                    const GeoRect& currentRect,
                                    bool positionRequired) {
    const RestoreFlags flags = MakeRestoreFlags(
        identity.restorePosition || positionRequired, identity.restoreSize);
    if (flags.restorePosition && flags.restoreSize) {
        return target;
    }

    // The window's own rectangle, read straight from Windows. This is the call
    // that used to go through CurrentMonitorView and could therefore refresh the
    // monitor cache -- and both callers hold a reference into that cache across
    // this call, so the refresh left them reading a freed vector. The rectangle
    // does not need the cache, and now never touches it.
    //
    // Best effort: if the rectangle cannot be read, the caller's rectangle (a
    // GetWindowRect) is the fallback, which is correct for every normal window.
    GeoRect own = currentRect;
    WindowOwnRect(hwnd, own, nullptr);

    return ApplyRestoreFlags(target, own, flags);
}

// --- Restore and settle -----------------------------------------------------

static std::wstring WindowKeyOf(const WindowIdentity& identity) {
    return identity.exePath + L"|" + identity.className + L"|" +
           identity.normalizedTitle + L"|" +
           std::to_wstring(identity.ordinal);
}

static const wchar_t* MonitorRuleName(MonitorMatchRule rule) {
    switch (rule) {
        case MonitorMatchRule::HardwarePath:
            return L"hardware-path";
        case MonitorMatchRule::FallbackId:
            return L"fallback-id";
        case MonitorMatchRule::FriendlyName:
            return L"friendly-name";
        case MonitorMatchRule::RelativePosition:
            return L"relative-position";
        case MonitorMatchRule::Primary:
            return L"primary";
        default:
            return L"none";
    }
}

// Must be armed on the mod's own message window. Passing a tracked window made
// SetTimer fail and return 0 without setting an error, which silently disabled
// the settle poll, the retry, the verdict lines and the debounced store flush.
// The offline tests could not catch it: it is an API contract, not logic.
static void EnsureSettleTimer() {
    HWND target = g_messageWindow.load();
    if (!g_settleTimer && target) {
        g_settleTimer = SetTimer(target, kSettleTimerId, kSettleTickMs, nullptr);
    }
}

static void StopSettleTimerIfIdle(HWND messageWindow) {
    if (g_settleTimer && messageWindow && g_pendingApplies.empty() &&
        !g_storeDirty) {
        KillTimer(messageWindow, kSettleTimerId);
        g_settleTimer = 0;
    }
}

// Arms the debounced relayout pass. Called from the WM_DISPLAYCHANGE handler,
// never from a tick: this is a debounce, not a poll.
// Same rule as the settle timer above -- the timer belongs to the mod's own
// message window, because SetTimer requires a window owned by the calling thread.
static void ArmRelayoutTimer() {
    HWND target = g_messageWindow.load();
    if (!target) {
        return;
    }

    // Reset, never stack. There is a single timer id, killed and re-armed on
    // every message of the burst, so the pass runs once, ~500 ms after the last
    // one instead of once per message.
    if (g_relayoutTimer) {
        KillTimer(target, kRelayoutTimerId);
        g_relayoutTimer = 0;
    }

    g_relayoutTimer =
        SetTimer(target, kRelayoutTimerId, kRelayoutDebounceMs, nullptr);
}

// Applies a target rect. Returns 0 on success, otherwise the Win32 error code.
// A non-zero result is a different situation from the app overriding the move:
// an elevated (UIPI) target lands here. The code is returned rather than merely
// logged because ERROR_ACCESS_DENIED is also the signal to stop saving that
// window's geometry: a refused move must not be recorded as a placement.
// Deliberately does not maximize: issuing both in one go is what made a
// maximized restore unreliable, because the move lands after the maximize
// request and un-maximizes the window again. Maximizing is a separate step,
// taken once this move has been observed to land.
static DWORD ApplyMoveGeometry(HWND hwnd, const GeoRect& target) {
    BOOL moved = SetWindowPos(
        hwnd, nullptr, (int)target.left, (int)target.top,
        (int)GeoRectWidth(target), (int)GeoRectHeight(target),
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    if (!moved) {
        DWORD error = GetLastError();
        if (error == ERROR_ACCESS_DENIED) {
            Wh_Log(L"[GEOM] apply failed: access denied (error %lu); the window "
                   L"likely belongs to an elevated process and is left alone",
                   error);
        } else {
            Wh_Log(L"[GEOM] apply failed: SetWindowPos error %lu", error);
        }
        return error;
    }

    return 0;
}

// The one place where the session's "refused" set changes, so an insert can
// never appear without its matching erase elsewhere.
static void ApplyUnwritableChange(const std::wstring& identityKey,
                                  UnwritableChange change) {
    if (change == UnwritableChange::Mark) {
        g_unwritableWindows.insert(identityKey);
    } else if (change == UnwritableChange::Clear) {
        g_unwritableWindows.erase(identityKey);
    }
}

// Called after every apply. The decision itself is a tested core function; this
// only translates its verdict.
static void NoteApplyResult(const std::wstring& identityKey, DWORD error) {
    ApplyUnwritableChange(identityKey,
                          DecideUnwritableChange(error == 0,
                                                 error == ERROR_ACCESS_DENIED,
                                                 /*userMoved=*/false));
}

static bool IssueMaximize(HWND hwnd) {
    if (!ShowWindowAsync(hwnd, SW_MAXIMIZE)) {
        Wh_Log(L"[GEOM] apply failed: ShowWindowAsync error %lu",
               GetLastError());
        return false;
    }
    return true;
}

static void GiveUpOnWindow(HWND hwnd, const WindowIdentity& identity,
                           const wchar_t* reason) {
    Wh_Log(L"[GEOM] %s; not retrying this window until the next session",
           reason);
    // Currently only reached when the apply call itself failed (an elevated or
    // otherwise unwritable target). It has to reach the journal and not only the
    // debug log: a refusal is exactly what a reader of the journal is looking
    // for, and a refusal that is visible only in a different log reads as
    // silence.
    AppendDiagnosticsJournal(L"VERDICT restore=api-failed exe=" +
                             identity.exeName);
    g_overrideWindows.insert(WindowKeyOf(identity));
    g_pendingApplies.erase(hwnd);
}

// The one place a move is issued from. Restore-on-show and the display-change
// relayout both feed this, so the settle/retry/override behaviour cannot drift
// between the two triggers. The record is inserted before the call: a fast tick
// must see the move as ours, and "our move landed" is decided geometrically
// while the record exists (see PendingApply).
static void IssueApply(HWND hwnd, const WindowIdentity& identity,
                       const GeoRect& target, const GeoRect& currentRect,
                       bool maximize, const GeoRect& monitorRect) {
    PendingApply pending;
    pending.target = target;
    pending.deadline = GetTickCount64() + kSettleWindowMs;
    pending.retriesLeft = 1;
    pending.maximize = maximize;
    pending.observedTarget = false;
    pending.lastObserved = currentRect;
    pending.identityKey = WindowKeyOf(identity);
    pending.exeName = identity.exeName;
    pending.targetMonitorRect = monitorRect;  // GeoRect, like the target
    g_pendingApplies[hwnd] = pending;

    // A maximized target starts with the move only; the maximize follows once
    // that move has been observed to land. See DecideMaximizeSettle.
    DWORD error = ApplyMoveGeometry(hwnd, target);
    NoteApplyResult(pending.identityKey, error);
    if (error == 0) {
        g_pendingApplies[hwnd].moveIssued = true;
        EnsureSettleTimer();
        return;
    }

    // The call itself failed; a retry would fail the same way. An elevated
    // target lands here, and is also marked unwritable above so that its
    // DESTROY-time save cannot record a position the mod never applied.
    GiveUpOnWindow(hwnd, identity,
                   L"VERDICT restore=api-failed (see the apply error above; "
                   L"skipping this window)");
}

static void TryRestoreWindow(HWND hwnd, const WindowIdentity& identity) {
    if (g_overrideWindows.count(WindowKeyOf(identity))) {
        // Silent until now, and it is the last link in a chain: an earlier window
        // of this app moved away from its restored position (the app doing it, or
        // the user dragging it), so the app is in charge from then on. Without this
        // line, every later window of that app is left alone with no explanation
        // anywhere, which reads exactly like a broken feature.
        AppendDiagnosticsJournal(L"VERDICT restore=skipped exe=" +
                                 identity.exeName +
                                 L" reason=overridden-earlier");
        return;
    }

    const StoredWindow* entry = FindStoredWindowFor(
        g_store, identity.exePath, identity.className, identity.normalizedTitle,
        identity.ordinal);
    if (!entry) {
        // The ordinary answer for a window seen for the first time, and the one
        // a user asking "why was my window not restored?" needs to see: nothing
        // was stored for it yet.
        AppendDiagnosticsJournal(L"VERDICT restore=none exe=" + identity.exeName +
                                 L" ordinal=" +
                                 std::to_wstring(identity.ordinal) +
                                 L" (no stored geometry for this window yet)");
        return;
    }

    const StoredConfig* config = FindConfigById(g_store, entry->configId);
    bool exactConfiguration = config && ConfigsMatch(g_monitors, config->monitors);

    MonitorResolution resolution =
        ResolveTargetMonitor(g_monitors, *entry, config);
    if (resolution.index < 0) {
        return;
    }
    const MonitorView& monitor = g_monitors[(size_t)resolution.index];

    // Exact pixels when the configuration matches (no rounding drift), the
    // normalized rect otherwise, so a resolution change does not push the window
    // off-screen.
    GeoRect target = exactConfiguration
                         ? entry->rectPixels
                         : FromNormalized(entry->rectNormalized,
                                          monitor.pixelRect);

    RECT current{};
    if (!GetWindowRect(hwnd, &current)) {
        return;
    }
    GeoRect currentRect{current.left, current.top, current.right, current.bottom};

    // The per-app switches, applied to the rectangle about to be used rather than
    // to what was stored. This is what makes "Restore size" off mean something:
    // before, both the restore and the relayout set the size regardless of it.
    // There is no rescue to force here: this is a restore of a remembered
    // position, which is exactly what the setting governs.
    target = ApplyOwnRestoreFlags(hwnd, identity, target, currentRect,
                                  /*positionRequired=*/false);

    // The clamp comes last, on the rectangle that will actually be applied. With
    // "Restore size" off the window keeps its own size, which can be larger than
    // the remembered one, so clamping the remembered rectangle first could leave
    // the rectangle handed to the window hanging past the edge of the work area.
    //
    // It is also measured against the monitor the window is actually going to be
    // on, which is not the stored one while the position is left alone.
    const MonitorView& placed = g_monitors[(size_t)RestoreClampMonitor(
        g_monitors, resolution.index, target, identity.restorePosition)];
    bool clamped = ShouldClampRect(target, placed.workRect, !exactConfiguration);
    if (clamped) {
        target = ClampRectToWorkArea(target, placed.workRect);
    }

    bool maximize = entry->state == L"maximized";
    if (maximize && WindowIsZoomedOn(hwnd, placed.pixelRect)) {
        // Already maximized on the monitor this restore would use. Nothing to do,
        // and doing anything means un-maximizing it first: the settle sequence
        // moves the window to the remembered normal rectangle and then maximizes
        // it again. Every app that opens maximized on its own would bounce.
        return;
    }
    if (!maximize &&
        GeoRectsApproxEqual(currentRect, target, kApplyTolerancePixels)) {
        return;  // already where it belongs
    }

    Wh_Log(L"[GEOM] RESTORE exe=%s monitor=%s rule=%s coords=%s clamp=%s "
           L"state=%s flags=%s target=(%ld,%ld,%ld,%ld)",
           identity.exeName.c_str(), monitor.gdiName.c_str(),
           MonitorRuleName(resolution.rule),
           exactConfiguration ? L"pixels" : L"normalized",
           clamped ? L"yes" : L"no", maximize ? L"maximized" : L"normal",
           FormatRestoreFlags(MakeRestoreFlags(identity.restorePosition,
                                                    identity.restoreSize))
               .c_str(),
           target.left, target.top, target.right, target.bottom);

    AppendDiagnosticsJournal(
        L"RESTORE exe=" + identity.exeName + L" ordinal=" +
        std::to_wstring(identity.ordinal) + L" rule=" +
        MonitorRuleName(resolution.rule) + L" target=(" +
        std::to_wstring(target.left) + L"," + std::to_wstring(target.top) +
        L"," + std::to_wstring(target.right) + L"," +
        std::to_wstring(target.bottom) + L") state=" +
        (maximize ? L"maximized" : L"normal") + L" clamp=" +
        (clamped ? L"yes" : L"no") +
        // Which halves of the remembered geometry this restore was allowed to
        // apply. The journal is the only place this is written down, and "the
        // setting did nothing" and "the setting did something" look identical in
        // the rectangle alone when the window happened to be in the right place.
        L" flags=" +
        FormatRestoreFlags(MakeRestoreFlags(identity.restorePosition,
                                           identity.restoreSize)) +
        // The same DPI sanity field the relayout path logs: the ratio of the DPI
        // this window reports now to the DPI stored with its geometry, so that one
        // field answers the question for every decision the mod makes.
        L" dpiRatio=" +
        FormatDpiRatio(entry->dpiAtSave, (unsigned)GetDpiForWindow(hwnd)));

    // The maximize confirm compares against the same monitor, for the same reason:
    // it has to be the monitor the window is on, not the one it was saved on.
    IssueApply(hwnd, identity, target, currentRect, maximize, placed.pixelRect);
}

// --- Display-change relayout pass -------------------------------------------

// Walks every window the mod is currently tracking and moves the ones whose
// monitor changed underneath them. Run only from the debounce timer, never from
// the WM_DISPLAYCHANGE handler directly.
static void RunRelayoutPass() {
    if (g_monitors.empty() || g_lastKnownMonitors.empty()) {
        // No previous arrangement to compare against (monitor enumeration failed,
        // or a pass before the first snapshot). Take the current set as the new
        // baseline: there is no evidence that any window's monitor changed.
        g_lastKnownMonitors = g_monitors;
        return;
    }

    if (MonitorSetsEquivalent(g_lastKnownMonitors, g_monitors)) {
        // A duplicate WM_DISPLAYCHANGE, or a change that left every monitor
        // exactly where it was. This is the case that has to be silent rather
        // than a no-op move of every tracked window. Journalled once per pass,
        // not once per window.
        AppendDiagnosticsJournal(
            L"RELAYOUT-SKIP exe=- ordinal=- reason=same-configuration");
        return;
    }

    // Iterate a copy of the handles: the pass must not walk a container it could
    // invalidate, and a window can be destroyed by its app at any moment.
    std::vector<HWND> windows;
    windows.reserve(g_identityCache.size());
    for (const auto& entry : g_identityCache) {
        windows.push_back(entry.first);
    }

    int moved = 0;
    int skipped = 0;

    for (HWND hwnd : windows) {
        auto cached = g_identityCache.find(hwnd);
        if (cached == g_identityCache.end() || !IsWindow(hwnd)) {
            continue;
        }
        const WindowIdentity& identity = cached->second;

        RECT current{};
        if (!GetWindowRect(hwnd, &current)) {
            continue;
        }
        GeoRect currentRect{current.left, current.top, current.right,
                            current.bottom};

        // The window's own current rectangle is the source of truth here, not the
        // stored preference: this keeps an open window reachable and roughly
        // where it was, it does not return it to a remembered position.
        RelayoutPlan plan =
            ComputeRelayoutPlan(g_lastKnownMonitors, g_monitors, currentRect);

        RelayoutSubject subject;
        std::wstring identityKey = WindowKeyOf(identity);
        subject.overridden = g_overrideWindows.count(identityKey) != 0;
        subject.pendingApply = g_pendingApplies.count(hwnd) != 0;
        subject.unwritable = g_unwritableWindows.count(identityKey) != 0;
        subject.minimized = IsIconic(hwnd) != FALSE;
        subject.maximized = IsZoomed(hwnd) != FALSE;

        RelayoutSkip skip = DecideRelayoutSkip(plan, subject);
        if (skip != RelayoutSkip::None) {
            skipped++;
            LogRelayoutSkip(identity, skip);
            continue;
        }

        const MonitorView& targetMonitor = g_monitors[(size_t)plan.targetIndex];

        // The same per-app switches as the restore path: an app the user told the
        // mod not to resize must not be resized by a display change either.
        //
        // With "Restore position" off the window keeps its current position, with
        // one exception: if its monitor is the one that vanished, the move is the
        // rescue and it happens anyway. Otherwise the setting would leave the
        // window on a monitor that no longer exists, which the readme promises
        // never happens -- and the user cannot get it back by dragging it.
        GeoRect target = plan.target;
        target = ApplyOwnRestoreFlags(hwnd, identity, target, currentRect,
                                      /*positionRequired=*/plan.originGone);

        // The clamp comes last, on the rectangle that will actually be applied:
        // keeping a window reachable is not the same as restoring its size, and a
        // larger size may need the window pulled back in.
        //
        // configurationChanged is false on purpose, as in the restore path: the
        // mapped rectangle already refers to a monitor that exists, so the only
        // question left is whether the title bar is reachable -- and a window the
        // user deliberately hung off an edge has to survive a rescue.
        bool clamped = ShouldClampRect(target, targetMonitor.workRect,
                                       /*configurationChanged=*/false);
        if (clamped) {
            target = ClampRectToWorkArea(target, targetMonitor.workRect);
        }

        // The DPI sanity field: logged, never applied. The stored DPI is read for
        // this line only.
        unsigned currentDpi = (unsigned)GetDpiForWindow(hwnd);
        const StoredWindow* entry = FindStoredWindowFor(
            g_store, identity.exePath, identity.className,
            identity.normalizedTitle, identity.ordinal);
        std::wstring dpiRatio =
            FormatDpiRatio(entry ? entry->dpiAtSave : 0, currentDpi);

        Wh_Log(L"[GEOM] RELAYOUT exe=%s ordinal=%d rule=%s origin=(%ld,%ld,%ld,"
               L"%ld) target=(%ld,%ld,%ld,%ld) dpiRatio=%s clamp=%s flags=%s",
               identity.exeName.c_str(), identity.ordinal,
               MonitorRuleName(plan.rule), plan.originRect.left,
               plan.originRect.top, plan.originRect.right, plan.originRect.bottom,
               target.left, target.top, target.right, target.bottom,
               dpiRatio.c_str(), clamped ? L"yes" : L"no",
               FormatRestoreFlags(MakeRestoreFlags(identity.restorePosition,
                                                   identity.restoreSize))
                   .c_str());

        AppendDiagnosticsJournal(
            L"RELAYOUT exe=" + identity.exeName + L" ordinal=" +
            std::to_wstring(identity.ordinal) + L" rule=" +
            MonitorRuleName(plan.rule) + L" origin=(" +
            std::to_wstring(plan.originRect.left) + L"," +
            std::to_wstring(plan.originRect.top) + L"," +
            std::to_wstring(plan.originRect.right) + L"," +
            std::to_wstring(plan.originRect.bottom) + L") target=(" +
            std::to_wstring(target.left) + L"," +
            std::to_wstring(target.top) + L"," +
            std::to_wstring(target.right) + L"," +
            std::to_wstring(target.bottom) + L") dpiRatio=" + dpiRatio +
            L" clamp=" + (clamped ? L"yes" : L"no") + L" flags=" +
            FormatRestoreFlags(
                MakeRestoreFlags(identity.restorePosition,
                                 identity.restoreSize)));

        moved++;

        // The existing apply path, fed from a new trigger: same asynchronous
        // move, same settle tick, same single retry, same journalled verdict. A
        // maximized window can only reach here after losing its monitor, and is
        // put back the way the restore path restores one: move first, maximize once
        // the move has been seen to land.
        IssueApply(hwnd, identity, target, currentRect, subject.maximized,
                   targetMonitor.pixelRect);
    }

    // Replaced only now, with every window considered: an interrupted pass leaves
    // the previous snapshot in place rather than a half-updated one.
    g_lastKnownMonitors = g_monitors;

    // One line per pass, so that "the pass ran and decided nothing needed doing"
    // is distinguishable from "the pass never ran" -- the difference between a
    // broken trigger and a working one with nothing to do.
    Wh_Log(L"[GEOM] relayout pass: %d tracked, %d moved, %d skipped",
           (int)windows.size(), moved, skipped);
    AppendDiagnosticsJournal(L"RELAYOUT-PASS windows=" +
                             std::to_wstring(windows.size()) + L" moved=" +
                             std::to_wstring(moved) + L" skipped=" +
                             std::to_wstring(skipped));
}


// A save that could not be made, journalled rather than silent.
//
// The mod's promise is that a window comes back where it was left. When that does
// not happen, the journal has to answer which of two things happened: the mod never
// saw the window close, or it saw the close and could not read a rectangle from a
// window that was already on its way out. Without a line for the second, both look
// like silence, and silence reads as the mod doing nothing at all.
static void JournalSaveSkip(const WindowIdentity& identity, PCWSTR reason) {
    AppendDiagnosticsJournal(L"SAVE-SKIP exe=" + identity.exeName + L" class=" +
                             identity.className + L" ordinal=" +
                             std::to_wstring(identity.ordinal) + L" reason=" +
                             reason);
}

static void SaveWindowGeometry(HWND hwnd, const WindowIdentity& identity) {
    // Never record geometry while our own apply is in flight: that would store a
    // half-applied position as if the user had chosen it.
    if (g_pendingApplies.count(hwnd)) {
        return;
    }

    // A window whose move was refused (an elevated target
    // under UIPI) must not have the position it happens to be sitting at
    // recorded as the user's choice: that is what makes the next launch open
    // there instead of where it was asked to be. Journaled rather than silently
    // skipped, because "why is this app's position not remembered?" needs an
    // answer in the journal, and a refusal visible only in the debug log reads
    // as silence.
    if (g_unwritableWindows.count(WindowKeyOf(identity))) {
        Wh_Log(L"[GEOM] save skipped for %s: a move for this window was refused "
               L"earlier this session",
               identity.exeName.c_str());
        JournalSaveSkip(identity, L"unwritable");
        return;
    }

    // The window's own rectangle, in one place: a normal window's screen rect, or
    // the converted normal rect of a minimized or maximized one (minimized windows
    // report roughly -32000, and a maximized window's GetWindowRect covers the whole
    // monitor, so neither is usable as "where the user left it"). A window that is
    // minimized at close therefore comes back where it was, at its normal size, or
    // maximized if it was maximized before it was minimized, rather than minimized.
    //
    // The conversion needs the monitor's rectangles, and WindowOwnRect asks
    // Windows for them rather than the cache -- see its comment. The cache is still
    // resolved here for the identity and the normalized rectangle recorded below,
    // and it is resolved before the reference is taken, never after.
    const MonitorView* monitorView = CurrentMonitorView(hwnd);
    if (!monitorView) {
        // No monitor resolves for the window: it is gone, and so is its rectangle.
        JournalSaveSkip(identity, L"no-monitor");
        return;
    }
    const MonitorView& monitor = *monitorView;

    GeoRect geometry{};
    UINT showCommand = 0;
    UINT placementFlags = 0;
    if (!WindowOwnRect(hwnd, geometry, &showCommand, &placementFlags)) {
        // The usual case at a close: the window is being destroyed, and Windows
        // answers nothing for a handle that is already gone. Nothing can be done
        // about a rectangle that cannot be read; it is recorded so that the miss
        // has a reason instead of leaving a gap.
        JournalSaveSkip(identity, L"no-placement");
        return;
    }
    // A window minimized from the maximized state reports SW_SHOWMINIMIZED and
    // sets WPF_RESTORETOMAXIMIZED: the state the user left it in is maximized, and
    // storing it as normal would bring it back at its normal size.
    const bool maximizeState =
        showCommand == SW_SHOWMAXIMIZED ||
        (showCommand == SW_SHOWMINIMIZED &&
         (placementFlags & WPF_RESTORETOMAXIMIZED));
    const std::wstring state = maximizeState ? L"maximized" : L"normal";
    if (GeoRectWidth(geometry) <= 0 || GeoRectHeight(geometry) <= 0) {
        JournalSaveSkip(identity, L"empty-rect");
        return;
    }

    long long now = (long long)time(nullptr);
    int configId = FindOrCreateConfigId();
    int dpi = (int)GetDpiForWindow(hwnd);

    StoredWindow* entry = nullptr;
    for (StoredWindow& window : g_store.windows) {
        if (window.exePath == identity.exePath &&
            window.className == identity.className &&
            window.titleKey == identity.normalizedTitle &&
            window.ordinal == identity.ordinal) {
            entry = &window;
            break;
        }
    }

    if (!entry) {
        StoredWindow window;
        window.exePath = identity.exePath;
        window.className = identity.className;
        window.titleKey = identity.normalizedTitle;
        window.ordinal = identity.ordinal;
        g_store.windows.push_back(std::move(window));
        entry = &g_store.windows.back();
    }

    entry->lastSeen = now;
    entry->configId = configId;
    entry->monitorKey = MonitorIdentityOf(monitor);
    entry->state = state;
    entry->rectPixels = geometry;
    entry->rectNormalized = ToNormalized(geometry, monitor.pixelRect);
    entry->dpiAtSave = (unsigned)(dpi > 0 ? dpi : 0);

    PruneStore(g_store, now);
    MarkStoreDirty();
    EnsureSettleTimer();

    Wh_Log(L"[GEOM] SAVED exe=%s class=%s ordinal=%d rect=(%ld,%ld,%ld,%ld) "
           L"state=%s monitor=%s flags=%s",
           identity.exeName.c_str(), identity.className.c_str(),
           identity.ordinal, geometry.left, geometry.top, geometry.right,
           geometry.bottom, state.c_str(), monitor.gdiName.c_str(),
           FormatRestoreFlags(MakeRestoreFlags(identity.restorePosition,
                                               identity.restoreSize))
               .c_str());

    AppendDiagnosticsJournal(
        L"SAVED exe=" + identity.exeName + L" class=" + identity.className +
        L" titleKey=" + identity.normalizedTitle +
        L" ordinal=" + std::to_wstring(identity.ordinal) +
        L" rect=(" + std::to_wstring(geometry.left) + L"," +
        std::to_wstring(geometry.top) + L"," + std::to_wstring(geometry.right) +
        L"," + std::to_wstring(geometry.bottom) + L") state=" + state +
        L" monitor=" + monitor.key + L" configId=" + std::to_wstring(configId) +
        L" flags=" +
        FormatRestoreFlags(MakeRestoreFlags(identity.restorePosition,
                                           identity.restoreSize)));
}

// The settle sequence for a maximized target. Kept separate from the
// normal-window path on purpose: that path's rectangle comparison is correct for
// a normal window and was field-validated, and a maximized window cannot use it
// at all (its rectangle is the monitor's).
static auto ProcessMaximizeTick(HWND window, PendingApply& pending,
                                std::unordered_map<HWND, PendingApply>::iterator it,
                                ULONGLONG now)
    -> std::unordered_map<HWND, PendingApply>::iterator {
    RECT current{};
    if (!GetWindowRect(window, &current)) {
        return g_pendingApplies.erase(it);
    }
    GeoRect currentRect{current.left, current.top, current.right, current.bottom};

    HMONITOR windowMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEXW monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    bool onTargetMonitor = false;
    if (GetMonitorInfoW(windowMonitor, &monitorInfo)) {
        onTargetMonitor =
            monitorInfo.rcMonitor.left == pending.targetMonitorRect.left &&
            monitorInfo.rcMonitor.top == pending.targetMonitorRect.top &&
            monitorInfo.rcMonitor.right == pending.targetMonitorRect.right &&
            monitorInfo.rcMonitor.bottom == pending.targetMonitorRect.bottom;
    }

    MaximizeSettle state;
    state.atStoredRect = GeoRectsApproxEqual(currentRect, pending.target,
                                             kApplyTolerancePixels);
    state.zoomed = IsZoomed(window) != FALSE;
    state.onTargetMonitor = onTargetMonitor;
    state.moveIssued = pending.moveIssued;
    state.maximizeIssued = pending.maximizeIssued;
    state.maximizeRetried = pending.maximizeRetried;
    state.deadlinePassed = now >= pending.deadline;
    state.moveRetriesLeft = pending.retriesLeft;

    switch (DecideMaximizeSettle(state)) {
        case SettleAction::Wait:
            return ++it;

        case SettleAction::Move: {
            bool reissue = pending.moveIssued;
            DWORD moveError = ApplyMoveGeometry(window, pending.target);
            NoteApplyResult(pending.identityKey, moveError);
            if (moveError != 0) {
                Wh_Log(L"[GEOM] VERDICT restore=api-failed exe=%s (the maximize "
                       L"move was rejected; see the error above)",
                       pending.exeName.c_str());
                g_overrideWindows.insert(pending.identityKey);
                return g_pendingApplies.erase(it);
            }
            if (reissue) {
                pending.retriesLeft--;
                Wh_Log(L"[GEOM] maximize: move did not land, re-issuing it once");
            }

            pending.moveIssued = true;
            pending.deadline = now + kSettleWindowMs;
            return ++it;
        }

        case SettleAction::Maximize: {
            bool reissue = pending.maximizeIssued;
            if (!IssueMaximize(window)) {
                Wh_Log(L"[GEOM] VERDICT restore=api-failed exe=%s (SW_MAXIMIZE "
                       L"was rejected; see the error above)",
                       pending.exeName.c_str());
                g_overrideWindows.insert(pending.identityKey);
                return g_pendingApplies.erase(it);
            }
            if (reissue) {
                pending.maximizeRetried = true;
                Wh_Log(L"[GEOM] maximize: still not zoomed, issuing SW_MAXIMIZE "
                       L"once more");
            } else {
                Wh_Log(L"[GEOM] maximize: move landed, issuing SW_MAXIMIZE");
            }
            pending.maximizeIssued = true;
            pending.deadline = now + kSettleWindowMs;
            return ++it;
        }

        case SettleAction::Succeeded:
            Wh_Log(L"[GEOM] VERDICT restore=ok exe=%s state=maximized "
                   L"monitor=(%ld,%ld,%ld,%ld)",
                   pending.exeName.c_str(), currentRect.left, currentRect.top,
                   currentRect.right, currentRect.bottom);
            AppendDiagnosticsJournal(
                L"VERDICT restore=ok exe=" + pending.exeName +
                L" state=maximized rect=(" + std::to_wstring(currentRect.left) +
                L"," + std::to_wstring(currentRect.top) + L"," +
                std::to_wstring(currentRect.right) + L"," +
                std::to_wstring(currentRect.bottom) + L")");
            return g_pendingApplies.erase(it);

        case SettleAction::Overridden:
        default:
            Wh_Log(L"[GEOM] VERDICT restore=overridden exe=%s (it will not "
                   L"stay maximized on the stored monitor; leaving it alone)",
                   pending.exeName.c_str());
            AppendDiagnosticsJournal(L"VERDICT restore=overridden exe=" +
                                     pending.exeName +
                                     L" reason=will-not-stay-maximized");
            g_overrideWindows.insert(pending.identityKey);
            return g_pendingApplies.erase(it);
    }
}

// One 100 ms tick, running only while there is work: settling applies and/or a
// dirty store. Cheaper than subscribing to EVENT_OBJECT_LOCATIONCHANGE, the
// highest-volume event in the shell, for what is normally zero to two windows.
static void ProcessSettleTick(HWND hwnd) {
    ULONGLONG now = GetTickCount64();

    for (auto it = g_pendingApplies.begin(); it != g_pendingApplies.end();) {
        HWND window = it->first;
        PendingApply& pending = it->second;

        if (!IsWindow(window)) {
            it = g_pendingApplies.erase(it);
            continue;
        }

        if (pending.maximize) {
            it = ProcessMaximizeTick(window, pending, it, now);
            continue;
        }

        if (IsIconic(window)) {
            ++it;  // minimized mid-settle; wait
            continue;
        }

        RECT current{};
        if (!GetWindowRect(window, &current)) {
            it = g_pendingApplies.erase(it);
            continue;
        }
        GeoRect currentRect{current.left, current.top, current.right,
                            current.bottom};

        if (GeoRectsApproxEqual(currentRect, pending.target,
                                kApplyTolerancePixels)) {
            // Our move landed. If the rect has stopped changing, the window has
            // settled.
            if (pending.observedTarget &&
                GeoRectsApproxEqual(currentRect, pending.lastObserved,
                                    kApplyTolerancePixels)) {
                // One searchable line for the outcome.
                Wh_Log(L"[GEOM] VERDICT restore=ok exe=%s rect=(%ld,%ld,%ld,%ld)",
                       pending.exeName.c_str(), currentRect.left,
                       currentRect.top, currentRect.right, currentRect.bottom);
                AppendDiagnosticsJournal(
                    L"VERDICT restore=ok exe=" + pending.exeName + L" rect=(" +
                    std::to_wstring(currentRect.left) + L"," +
                    std::to_wstring(currentRect.top) + L"," +
                    std::to_wstring(currentRect.right) + L"," +
                    std::to_wstring(currentRect.bottom) + L")");
                it = g_pendingApplies.erase(it);
                continue;
            }
            pending.observedTarget = true;
            pending.lastObserved = currentRect;
            ++it;
            continue;
        }

        if (pending.observedTarget) {
            // It landed and something moved it afterwards: the app (or the user)
            // has the final say.
            Wh_Log(L"[GEOM] VERDICT restore=overridden exe=%s (moved after "
                   L"restore; leaving it alone)",
                   pending.exeName.c_str());
            AppendDiagnosticsJournal(L"VERDICT restore=overridden exe=" +
                                     pending.exeName +
                                     L" reason=moved-after-restore");
            g_overrideWindows.insert(pending.identityKey);
            it = g_pendingApplies.erase(it);
            continue;
        }

        if (now < pending.deadline) {
            ++it;
            continue;
        }

        if (pending.retriesLeft > 0) {
            pending.retriesLeft--;
            pending.deadline = now + kSettleWindowMs;
            pending.lastObserved = currentRect;
            Wh_Log(L"[GEOM] re-applying geometry once (the app moved the window "
                   L"after it was shown)");
            DWORD retryError = ApplyMoveGeometry(window, pending.target);
            NoteApplyResult(pending.identityKey, retryError);
            if (retryError != 0) {
                g_overrideWindows.insert(pending.identityKey);
                it = g_pendingApplies.erase(it);
                continue;
            }
            ++it;
            continue;
        }

        // Out of retries and never landed: the app is fighting us. Stop, and
        // remember for this session only.
        Wh_Log(L"[GEOM] VERDICT restore=overridden exe=%s (the app keeps "
               L"moving its window; giving up)",
               pending.exeName.c_str());
        AppendDiagnosticsJournal(L"VERDICT restore=overridden exe=" +
                                 pending.exeName + L" reason=app-keeps-moving");
        g_overrideWindows.insert(pending.identityKey);
        it = g_pendingApplies.erase(it);
    }

    if (g_storeDirty && now - g_storeLastChange >= kStoreFlushDelayMs) {
        if (WriteGeometryStore()) {
            g_storeDirty = false;
            Wh_Log(L"[GEOM] store saved (%d window entries, %d configurations)",
                   (int)g_store.windows.size(), (int)g_store.configs.size());
            AppendDiagnosticsJournal(
                L"STORE-WRITTEN entries=" +
                std::to_wstring(g_store.windows.size()) + L" configs=" +
                std::to_wstring(g_store.configs.size()));
        } else {
            // Retry on the next tick, but do not spin: push the deadline out.
            g_storeLastChange = now;
        }
    }

    StopSettleTimerIfIdle(hwnd);
}

// --- Event handling ---------------------------------------------------------

static void CALLBACK WinEventProc(HWINEVENTHOOK /*hook*/, DWORD event,
                                  HWND hwnd, LONG idObject, LONG idChild,
                                  DWORD /*eventThread*/,
                                  DWORD /*eventTime*/) {
    // Window-level events only. Non-window objects (carets, menus, list items)
    // also produce CREATE/SHOW/DESTROY events.
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    // No re-entrancy guard is needed around this callback itself: the mod only
    // moves windows with SetWindowPos, which does not raise MOVESIZEEND, and
    // EVENT_OBJECT_LOCATIONCHANGE is deliberately not registered. Our own moves
    // are recognised geometrically by the pending record instead.

    if (event == EVENT_OBJECT_DESTROY) {
        g_pendingApplies.erase(hwnd);

        auto cached = g_identityCache.find(hwnd);
        if (cached == g_identityCache.end()) {
            return;  // a window we never tracked
        }

        // Save before dropping the identity: this is the last chance to record
        // where the window was left.
        SaveWindowGeometry(hwnd, cached->second);
        LogWindowEvent(L"DESTROY", hwnd, cached->second);
        // Release the ordinal after saving, so the next window of this app can
        // reuse it and pick up the geometry just stored.
        ReleaseOrdinal(cached->second);
        g_identityCache.erase(cached);
        return;
    }

    auto cached = g_identityCache.find(hwnd);

    if (cached != g_identityCache.end()) {
        if (event == EVENT_SYSTEM_MOVESIZEEND) {
            // The user moved the window. Their intent supersedes any restore
            // still settling, so drop the pending record before recording.
            g_pendingApplies.erase(hwnd);
            // ... and it also ends any earlier refusal: the user has just placed
            // this window by hand, so its position is theirs again and its saves
            // resume from here. Cleared before the save below, so that this very
            // move is the first thing recorded.
            ApplyUnwritableChange(WindowKeyOf(cached->second),
                                  DecideUnwritableChange(
                                      /*applySucceeded=*/false,
                                      /*accessDenied=*/false,
                                      /*userMoved=*/true));
            SaveWindowGeometry(hwnd, cached->second);
        }
        LogWindowEvent(EventName(event), hwnd, cached->second);
        return;
    }

    // First time we see this window pass the filters. Usually CREATE, but a
    // window created at zero size is filtered there and first passes at SHOW.
    const wchar_t* reason = L"unknown";
    if (!PassesWindowFilters(hwnd, &reason)) {
        LogSkippedWindow(hwnd, reason);
        return;
    }

    WindowIdentity identity;
    if (!ResolveIdentity(hwnd, identity, &reason)) {
        LogSkippedWindow(hwnd, reason);
        return;
    }

    g_identityCache.emplace(hwnd, identity);
    // Journalled because it answers a question that was otherwise unanswerable
    // from outside: whether the mod ever saw a window at all. Without it, "the
    // window was not restored" cannot be told apart from "the window was never a
    // candidate", and the two call for completely different investigations.
    AppendDiagnosticsJournal(L"TRACK exe=" + identity.exeName + L" class=" +
                             identity.className + L" titleKey=" +
                             identity.normalizedTitle + L" ordinal=" +
                             std::to_wstring(identity.ordinal));
    LogWindowEvent(EventName(event), hwnd, identity);
    if (event == EVENT_SYSTEM_MOVESIZEEND) {
        // Tracked late -- typically an app that titles its window only after
        // showing it. The user has just placed it, so record that immediately and
        // stop: restoring here would move the window they are looking at, which
        // is the one thing the readme promises never happens.
        SaveWindowGeometry(hwnd, identity);
        return;
    }
    TryRestoreWindow(hwnd, identity);
}

// --- Display changes --------------------------------------------------------


struct MonitorRecord {
    std::wstring device;
    RECT rect{};
    bool primary = false;
};

static BOOL CALLBACK MonitorEnumProc(HMONITOR monitor, HDC /*hdc*/,
                                     LPRECT /*rect*/, LPARAM data) {
    auto* monitors = reinterpret_cast<std::vector<MonitorRecord>*>(data);

    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (GetMonitorInfoW(monitor, &info)) {
        MonitorRecord record;
        record.device = info.szDevice;
        record.rect = info.rcMonitor;
        record.primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
        monitors->push_back(record);
    }

    return TRUE;
}

// What the mod reads back for the primary monitor, for the display-change line
// below: compared against the size Windows reports in the message itself.
static std::wstring PrimaryMonitorSizeText() {
    for (const MonitorView& monitor : g_monitors) {
        if (monitor.primary) {
            return std::to_wstring(GeoRectWidth(monitor.pixelRect)) + L"x" +
                   std::to_wstring(GeoRectHeight(monitor.pixelRect));
        }
    }
    return L"none";
}

static void LogDisplayChange(WPARAM wParam, LPARAM lParam) {
    std::vector<MonitorRecord> monitors;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                        reinterpret_cast<LPARAM>(&monitors));

    // wParam carries the new color depth, lParam the new screen size.
    Wh_Log(L"[GEOM] WM_DISPLAYCHANGE bpp=%u screen=%ux%u monitors=%d",
           (unsigned)wParam, (unsigned)LOWORD(lParam), (unsigned)HIWORD(lParam),
           (int)monitors.size());

    for (size_t i = 0; i < monitors.size(); i++) {
        const MonitorRecord& monitor = monitors[i];
        // szDevice is the slot-dependent name, e.g. \\.\DISPLAY1: kept for
        // readability only, since Windows renumbers slots. The stable identity
        // used for matching is the device interface path; see MonitorIdentityOf.
        Wh_Log(L"[GEOM]   monitor[%d] device=%s primary=%s "
               L"rect=(%ld,%ld,%ld,%ld)",
               (int)i, monitor.device.c_str(), monitor.primary ? L"yes" : L"no",
               monitor.rect.left, monitor.rect.top, monitor.rect.right,
               monitor.rect.bottom);
    }
}

// --- Event thread -----------------------------------------------------------

static HWINEVENTHOOK g_hookWindows = nullptr;
static HWINEVENTHOOK g_hookMoveSizeEnd = nullptr;
static std::atomic<DWORD> g_eventThreadId{0};
static HANDLE g_eventThread = nullptr;

static void InstallHooks() {
    if (g_hookWindows && g_hookMoveSizeEnd) {
        return;
    }

    // Each hook is retried independently, so a failure of one does not stop the
    // other from being installed on a later reload.
    // EVENT_OBJECT_CREATE (0x8000), EVENT_OBJECT_DESTROY (0x8001) and
    // EVENT_OBJECT_SHOW (0x8002). EVENT_OBJECT_LOCATIONCHANGE is deliberately
    // not registered: it is a storm of events, and the job it would do here is a
    // save trigger rather than something to log.
    if (!g_hookWindows) {
        g_hookWindows = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW,
                                        nullptr, WinEventProc, 0, 0,
                                        WINEVENT_OUTOFCONTEXT);
    }
    if (!g_hookMoveSizeEnd) {
        g_hookMoveSizeEnd =
            SetWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND,
                            nullptr, WinEventProc, 0, 0,
                            WINEVENT_OUTOFCONTEXT);
    }

    if (!g_hookWindows || !g_hookMoveSizeEnd) {
        Wh_Log(L"[GEOM] SetWinEventHook failed (last error %lu)",
               GetLastError());
    } else {
        Wh_Log(L"[GEOM] window event hooks installed");
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
}

// Event-thread only. Re-reads everything the mod needs from the settings.
static void ApplySettingsOnEventThread() {
    LoadAllowlist();

    // A clear that ran before the event thread existed (Wh_ModInit runs first)
    // is applied here rather than being lost.
    if (g_clearStoreRequested.load()) {
        g_clearStoreRequested.store(false);
        ClearStoredFile();
        g_store = GeometryStore();
        g_storeDirty = false;
        Wh_Log(L"[GEOM] cleared stored data requested before startup");
    }

    // A settings change may drop an app from the allowlist, so any restore still
    // settling is abandoned rather than carried out.
    if (!g_pendingApplies.empty()) {
        Wh_Log(L"[GEOM] settings changed; abandoning %d pending restore(s)",
               (int)g_pendingApplies.size());
        g_pendingApplies.clear();
    }

    RefreshDiagnosticsSetting();

    // Windows that are already tracked were resolved against the old settings.
    // Their switches and their place in the allowlist are refreshed here, before
    // anything below can return: otherwise turning a switch off would not reach an
    // open window until it closed, and an app removed from the list would keep
    // being saved and moved.
    for (auto it = g_identityCache.begin(); it != g_identityCache.end();) {
        const AllowlistEntry* entry =
            FindAllowlistEntry(it->second.exePath, it->second.exeName);
        if (!entry) {
            Wh_Log(L"[GEOM] no longer allowlisted; forgetting %s",
                   it->second.exeName.c_str());
            ReleaseOrdinal(it->second);
            g_pendingApplies.erase(it->first);
            it = g_identityCache.erase(it);
            continue;
        }
        it->second.restorePosition = entry->restorePosition;
        it->second.restoreSize = entry->restoreSize;
        ++it;
    }

    // The hooks are idempotent, so this is safe on every save. The relayout
    // baseline deliberately is not reset here: a save within half a second of a
    // display change would make the debounced pass compare the new arrangement
    // with itself and skip the rescue. It is set where the thread starts.
    InstallHooks();

    if (g_allowlistIsEmpty) {
        Wh_Log(L"[GEOM] the allowlist is empty; no windows will be tracked. Add "
               L"app executable names in the mod settings.");
        return;
    }

    // Logging the effective allowlist makes a misconfigured entry (a typo, or a
    // name that does not match the real executable) obvious from the log alone.
    std::wstring joined;
    for (const AllowlistEntry& entry : g_allowlist) {
        if (!joined.empty()) {
            joined += L", ";
        }
        joined += entry.exe;
        if (!entry.restorePosition || !entry.restoreSize) {
            // Worth naming in the log: "why does this app keep its size?" is
            // answered by the entry it matched, not by the mod's behaviour.
            joined += L" (";
            joined += FormatRestoreFlags(
                MakeRestoreFlags(entry.restorePosition, entry.restoreSize));
            joined += L")";
        }
    }
    Wh_Log(L"[GEOM] allowlist loaded: %s", joined.c_str());
}

static LRESULT CALLBACK MessageWndProc(HWND hwnd, UINT message, WPARAM wParam,
                                       LPARAM lParam) {
    switch (message) {
        case WM_DISPLAYCHANGE:
            LogDisplayChange(wParam, lParam);
            // Keep the monitor cache current so the next save or restore sees
            // the new arrangement. The refresh is immediate and idempotent; the
            // relayout pass that reacts to it is debounced, because a single
            // topology change arrives as a burst of these messages.
            RefreshMonitorViews();
            // Journalled as well as logged: the mod log needs DebugView, while the
            // journal can be read from a script. This is also the line to look for
            // after changing resolution or docking, to confirm the mod saw it.
            AppendDiagnosticsJournal(
                L"DISPLAYCHANGE bpp=" + std::to_wstring((unsigned long)wParam) +
                L" size=" + std::to_wstring((unsigned long)(lParam & 0xFFFF)) +
                L"x" + std::to_wstring((unsigned long)((lParam >> 16) & 0xFFFF)) +
                L" monitors=" + std::to_wstring(g_monitors.size()) +
                // size= is what Windows reports and primary= is what the mod
                // reads back for the same monitor; the tool process is
                // per-monitor aware, so the two describe the same pixels.
                L" primary=" + PrimaryMonitorSizeText() +
                L" dpi=" + std::to_wstring((unsigned long)GetDpiForWindow(hwnd)));
            ArmRelayoutTimer();
            return 0;

        case WM_TIMER:
            if (wParam == kSettleTimerId) {
                ProcessSettleTick(hwnd);
                return 0;
            }
            if (wParam == kRelayoutTimerId) {
                // Single-shot, and stopped before the pass runs: the pass can
                // therefore never be re-entered from the timer, and the mod is
                // silent between bursts of display messages.
                KillTimer(hwnd, kRelayoutTimerId);
                g_relayoutTimer = 0;
                RunRelayoutPass();
                return 0;
            }
            break;

        case kMsgReloadSettings:
            ApplySettingsOnEventThread();
            return 0;

        case kMsgClearStore:
            // The file first, then the in-memory copy, both on this thread: doing
            // the delete on the settings thread left a window in which the pending
            // flush wrote the cleared data straight back to disk.
            ClearStoredFile();
            g_store = GeometryStore();
            g_storeDirty = false;
            g_pendingApplies.clear();
            g_overrideWindows.clear();
            g_unwritableWindows.clear();
            g_clearStoreRequested.store(false);
            Wh_Log(L"[GEOM] cleared stored data (memory and file)");
            return 0;

        case kMsgShutdown:
            UninstallHooks();
            if (g_storeDirty) {
                WriteGeometryStore();
                g_storeDirty = false;
            }
            if (g_settleTimer) {
                KillTimer(hwnd, kSettleTimerId);
                g_settleTimer = 0;
            }
            if (g_relayoutTimer) {
                KillTimer(hwnd, kRelayoutTimerId);
                g_relayoutTimer = 0;
            }
            // Everything else is in-memory only.
            g_identityCache.clear();
            g_liveOrdinals.clear();
            g_allowlist.clear();
            g_pendingApplies.clear();
            g_overrideWindows.clear();
            g_unwritableWindows.clear();
            g_monitors.clear();
            g_lastKnownMonitors.clear();
            g_store = GeometryStore();
            g_messageWindow.store(nullptr);
            DestroyWindow(hwnd);
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
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
        Wh_Log(L"[GEOM] RegisterClassExW failed (last error %lu)",
               GetLastError());
        return 1;
    }

    // A top-level window, not a message-only one: message-only windows do not
    // receive broadcast messages, and WM_DISPLAYCHANGE is a broadcast. It stays
    // hidden because it is never shown. Same approach as the
    // per-monitor-scale-switcher mod.
    HWND hwnd = CreateWindowExW(0, kMessageWindowClassName, L"", WS_POPUP, 0, 0,
                                0, 0, nullptr, nullptr, instance, nullptr);
    if (!hwnd) {
        Wh_Log(L"[GEOM] failed to create the message window (last error %lu)",
               GetLastError());
        UnregisterClassW(kMessageWindowClassName, instance);
        return 1;
    }

    g_messageWindow.store(hwnd);

    RefreshMonitorViews();
    // The arrangement the relayout pass compares against. Taken once, here, so
    // that a display change arriving before (or during) the first settings apply
    // is still a change: a later compare against the same snapshots is what the
    // 500 ms debounce is for.
    g_lastKnownMonitors = g_monitors;
    RefreshDiagnosticsSetting();   // must precede the load: the load journals
    AppendDiagnosticsJournal(std::wstring(L"MOD build=") + kJournalBuild);
    LoadGeometryStore();
    ApplySettingsOnEventThread();

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    UnregisterClassW(kMessageWindowClassName, instance);
    g_eventThreadId.store(0);
    return 0;
}

// --- Clear stored data action -----------------------------------------------


static void DeleteStoredFile(PWSTR path) {
    // "Nothing to clear" is a normal outcome, not an error.
    if (DeleteFileW(path)) {
        Wh_Log(L"Cleared stored data: %s", path);
        return;
    }
    DWORD error = GetLastError();
    if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND) {
        Wh_Log(L"Failed to clear stored data (error %u): %s", error, path);
    }
}

// Event-thread only, so that a flush already pending cannot write the data back
// after the file is gone.
static void ClearStoredFile() {
    WCHAR path[MAX_PATH];
    if (BuildGeometryStorePath(path, ARRAYSIZE(path))) {
        DeleteStoredFile(path);
    }
}

// One-click handling of the "Clear stored data" action.
//
// Windhawk doesn't let a mod write its own settings back, so the dropdown can't
// reset itself after firing. Instead the last handled value is latched in mod
// storage: the wipe runs when the setting becomes "clearNow", and does not run
// again until the user has set it back to "none" and saved. Without the latch,
// re-saving any unrelated setting would silently wipe data the mod had
// collected since the previous clear.
//
// Deliberately independent of the allowlist and of any tracked window: clearing
// is a maintenance action, and it runs whenever it is chosen.
static void ApplyClearStoredDataAction() {
    const std::wstring& latchName = PerUserLatchValueName();
    if (latchName.empty()) {
        // No private folder, so this session never saved anything to clear.
        return;
    }

    PCWSTR requestedAction = Wh_GetStringSetting(L"clearStoredData");
    const bool requestedClear = wcscmp(requestedAction, L"clearNow") == 0;
    Wh_FreeStringSetting(requestedAction);

    const int latch = (int)Wh_GetIntValue(latchName.c_str(), kClearLatchNone);

    if (!requestedClear) {
        if (latch != kClearLatchNone) {
            Wh_SetIntValue(latchName.c_str(), kClearLatchNone);
        }
        return;
    }

    if (latch == kClearLatchHandled) {
        return;
    }

    if (!Wh_SetIntValue(latchName.c_str(), kClearLatchHandled)) {
        // Worth surfacing: without the latch the next settings save would clear
        // again.
        Wh_Log(L"Failed to latch the clear action; it may run again on the next "
               L"settings save");
    }

    // The file is deleted by the event thread, which is also the only thread that
    // touches the in-memory copy: deleting it here left a window in which a flush
    // that was already pending wrote the data straight back. The flag covers the
    // case where the thread does not exist yet -- the startup path acts on it.
    g_clearStoreRequested.store(true);
    HWND messageWindow = g_messageWindow.load();
    if (messageWindow) {
        PostMessageW(messageWindow, kMsgClearStore, 0, 0);
    }
}

// --- Windhawk entry points --------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"[GEOM] init");

    // Resolved here, before the event thread exists, so that the folder is created
    // and the SID read by one thread: a settings save can call into the same state
    // from Windhawk's thread.
    UserDataFolder();

    ApplyClearStoredDataAction();

    g_eventThread = CreateThread(nullptr, 0, EventThreadProc, nullptr, 0,
                                 nullptr);
    if (!g_eventThread) {
        Wh_Log(L"[GEOM] failed to create the event thread (last error %lu)",
               GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    ApplyClearStoredDataAction();

    // Settings are re-read on the event thread so that all mod state stays
    // owned by one thread. If the window does not exist yet, the thread reads
    // the current settings when it starts.
    HWND messageWindow = g_messageWindow.load();
    if (messageWindow) {
        PostMessageW(messageWindow, kMsgReloadSettings, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"[GEOM] uninit");

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
            Wh_Log(L"[GEOM] the event thread did not exit in time");
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

