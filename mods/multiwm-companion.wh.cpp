// ==WindhawkMod==
// @id              multiwm-companion
// @name            MultiWM Companion
// @description     Opens new windows directly in the slot MultiWM will give them, so they don't appear elsewhere first and jump.
// @version         0.3.2
// @author          meteoni
// @github          https://github.com/Meteoni
// @license         MIT
// @include         *
// @exclude         windhawk.exe
// @exclude         windhawk-mod.exe
// @exclude         windhawk-mod-uiaccess.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         LockApp.exe
// @exclude         SystemSettings.exe
// @exclude         TextInputHost.exe
// @exclude         Widgets.exe
// @exclude         ctfmon.exe
// @exclude         GameBarPresenceWriter.exe
// @exclude         backgroundTaskHost.exe
// @exclude         SecurityHealthSystray.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MultiWM Companion

An optional add-on for [MultiWM](https://github.com/Meteoni/MultiWM). Without it,
a new window first appears where the app puts it, then jumps into its tile. With
it, the window opens in its tile directly.

## What it does
- New windows open in the tile MultiWM will give them, with your gap, insets and
  window rules applied.
- Windows restored from the taskbar or the tray reappear in their own tile.
- Windows that will float (by a rule, in the Floating layout, or because they
  don't fit) open where they'll float.
- If a guess turns out wrong (say the app resizes itself right after opening),
  MultiWM moves the window as usual.

Dialogs, tool windows, excluded windows, and windows that open maximized or
fullscreen are left alone.

## Requirements
- MultiWM installed and running, in Automatic mode. In Manual mode this mod does
  nothing.
- A matching MultiWM release (see the version chart below). Try to update MultiWM
  and the companion together: mismatched versions may give no hints.

## Compatibility
Don't combine with other mods that move windows before they appear, such as
Window Centralizer or Center New Windows: both would move the same window.

## Troubleshooting
Enable logging for this mod in Windhawk to see each placement decision.

## Version chart 

| Companion | MultiWM |
| --------- | ------- |
| 0.3.2     | 1.16.1+ |
*/
// ==/WindhawkModReadme==

// Like src/multiwm/multiwm.cpp, the headers below are chapters of this one file, pasted
// in order by WHAM into output/companion/multiwm-companion.wh.cpp. This mod runs inside
// every application, so it stays small: one decision per window as it is shown.

#include <windows.h>
#include <dwmapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// Shared with MultiWM (src/multiwm/multiwm.cpp): rule matching and the snapshot ABI.
// WHAM@5 begin 1: src/shared/window_rules.h
// Window-rule vocabulary and matching, shared by MultiWM and MultiWM Companion.
//
// Both mods compile this file, so it may use only Windows and the C++ standard
// library, never a MultiWM chapter. The include guard (rather than #pragma once)
// keeps it safe when WHAM pastes it into a single generated file.

#ifndef MULTIWM_SHARED_WINDOW_RULES_H
#define MULTIWM_SHARED_WINDOW_RULES_H

#include <windows.h>

#include <algorithm>
#include <cwctype>
#include <string>
#include <vector>

// What a matching window rule does. The numeric values are part of the companion
// snapshot ABI (shared/companion_snapshot.h): append new values, never reorder.
enum class WindowRuleTreatment {
  Exclude,
  DefaultToFloating,
  AlwaysFloating,
  TraceToOwner,
  FloatingPlacementOverride,
  InitialPlacement,
};

namespace SharedRules {

// Shell surfaces MultiWM never manages, whatever the rules say.
inline constexpr const wchar_t* kIgnoredWindowClasses[] = {
    L"Progman",
    L"WorkerW",
    L"Shell_TrayWnd",
    L"Shell_SecondaryTrayWnd",
    L"Windows.UI.Core.CoreWindow",
    L"OperationStatusWindow",
    // DWM's stand-in for a hung app's window. MultiWM follows it through the
    // hung window instead (Platform::Windows::GhostWindowOf).
    L"Ghost",
};

inline bool IsIgnoredWindowClass(const wchar_t* className) {
  if (!className) return false;
  for (const wchar_t* ignored : kIgnoredWindowClasses) {
    if (_wcsicmp(className, ignored) == 0) return true;
  }
  return false;
}

// Why `hwnd` can't be a managed top-level window, or nullptr: it isn't its own
// root, is owned, or is a child. The first shape check in both mods.
inline const char* RejectNonTopLevel(HWND hwnd) {
  if (GetAncestor(hwnd, GA_ROOT) != hwnd || GetWindow(hwnd, GW_OWNER)) return "child or owned window";
  if (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) return "child window";
  return nullptr;
}

// Why `hwnd` is a tool window or shell surface MultiWM never manages, or nullptr.
inline const char* RejectToolOrShellWindow(HWND hwnd) {
  if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return "tool window";
  wchar_t className[64];
  if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) && IsIgnoredWindowClass(className)) {
    return "shell window";
  }
  return nullptr;
}

// The file name of `process`'s executable ("notepad.exe"), or empty when it
// can't be read. Both mods read it this way, so a process rule matches the same
// windows in each, even for paths longer than MAX_PATH.
inline std::wstring ProcessImageFileName(HANDLE process) {
  std::wstring path(32768, L'\0');
  DWORD length = static_cast<DWORD>(path.size());
  if (!QueryFullProcessImageNameW(process, 0, path.data(), &length)) return {};
  path.resize(length);
  const size_t slash = path.find_last_of(L"\\/");
  return slash == std::wstring::npos ? path : path.substr(slash + 1);
}

inline bool EqualsInsensitive(const std::wstring& a, const std::wstring& b) {
  return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

inline bool ContainsInsensitive(const std::wstring& text, const std::wstring& needle) {
  if (needle.empty()) return false;
  return std::search(
             text.begin(), text.end(), needle.begin(), needle.end(),
             [](wchar_t a, wchar_t b) { return std::towlower(a) == std::towlower(b); }) != text.end();
}

// The window properties a rule can match, each read on first use and then
// cached. Reading the process name can be costly (MultiWM opens the process), so
// rules that don't name a process never pay for it.
//
// The title is read with InternalGetWindowText, which never sends WM_GETTEXT.
// For another process's window that is what GetWindowText returns anyway; inside
// the window's own process it keeps application code from running mid-hook.
class LazyWindowFacts {
 public:
  using ProcessNameReader = std::wstring (*)(HWND);

  LazyWindowFacts(HWND hwnd, ProcessNameReader readProcessName)
      : hwnd_(hwnd), readProcessName_(readProcessName) {}

  HWND Window() const { return hwnd_; }

  const std::wstring& ClassName() {
    if (!haveClass_) {
      wchar_t buffer[256]{};
      if (GetClassNameW(hwnd_, buffer, ARRAYSIZE(buffer))) className_ = buffer;
      haveClass_ = true;
    }
    return className_;
  }

  const std::wstring& Title() {
    if (!haveTitle_) {
      wchar_t buffer[512]{};
      if (InternalGetWindowText(hwnd_, buffer, ARRAYSIZE(buffer))) title_ = buffer;
      haveTitle_ = true;
    }
    return title_;
  }

  const std::wstring& ProcessName() {
    if (!haveProcess_) {
      if (readProcessName_) processName_ = readProcessName_(hwnd_);
      haveProcess_ = true;
    }
    return processName_;
  }

 private:
  HWND hwnd_;
  ProcessNameReader readProcessName_;
  std::wstring className_;
  std::wstring title_;
  std::wstring processName_;
  bool haveClass_ = false;
  bool haveTitle_ = false;
  bool haveProcess_ = false;
};

// True when every non-empty match field of `rule` matches the window: an exact
// class name, a title fragment, an exact process file name, all ignoring case.
// Within a field the values are alternatives. `Rule` is any type with
// `classNames`, `titleFragments` and `processNames` string vectors.
template <class Rule>
bool RuleMatchesWindow(const Rule& rule, LazyWindowFacts& facts) {
  auto anyEqual = [](const std::vector<std::wstring>& candidates, const std::wstring& value) {
    return std::any_of(candidates.begin(), candidates.end(),
                       [&](const std::wstring& candidate) { return EqualsInsensitive(value, candidate); });
  };
  // Cheapest observation first; the process name is last because it can be costly.
  if (!rule.classNames.empty() && !anyEqual(rule.classNames, facts.ClassName())) return false;
  if (!rule.titleFragments.empty() &&
      std::none_of(rule.titleFragments.begin(), rule.titleFragments.end(),
                   [&](const std::wstring& fragment) { return ContainsInsensitive(facts.Title(), fragment); })) {
    return false;
  }
  if (!rule.processNames.empty() && !anyEqual(rule.processNames, facts.ProcessName())) return false;
  return true;
}

// The first rule with `treatment` that matches the window, or nullptr. `Rule`
// also needs a `treatment` member.
template <class Rule>
const Rule* FindFirstMatchingRule(
    const std::vector<Rule>& rules, WindowRuleTreatment treatment, LazyWindowFacts& facts) {
  for (const Rule& rule : rules) {
    if (rule.treatment == treatment && RuleMatchesWindow(rule, facts)) return &rule;
  }
  return nullptr;
}

}  // namespace SharedRules

#endif  // MULTIWM_SHARED_WINDOW_RULES_H
// WHAM@5 end 1
// WHAM@5 begin 2: src/shared/split_geometry.h
// Divider arithmetic shared by MultiWM's layout planner and MultiWM Companion,
// so both mods size a split, and fit minimum sizes to it, with the same code.
// Physical pixels throughout. Windows and the standard library only.
//
// The companion uses it to place a new BSP window exactly where MultiWM will:
// MultiWM publishes the splits above the window's tile (BspPathSplit), and
// SolveBspPath fits the window's own minimum size to them.
#ifndef MULTIWM_SHARED_SPLIT_GEOMETRY_H
#define MULTIWM_SHARED_SPLIT_GEOMETRY_H

#include <windows.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>

namespace SharedSplits {

// Divider ratios (master size, Dwindle and BSP splits) are kept within 10%..90%
// so neither side of a divider can collapse to nothing.
inline double ClampSplitRatio(double ratio) {
  return std::clamp(ratio, 0.1, 0.9);
}

// Pixels left for the two sides of a split after removing the gap between them.
// The gap shrinks if needed so each side keeps at least one pixel.
// Requires span >= 2.
inline LONG UsableSplitSpan(LONG span, LONG gap) {
  return span - std::clamp<LONG>(gap, 0, span - 2);
}

// Size of the first side of a split: `ratio` of `usable`, rounded to whole pixels,
// always leaving at least one pixel for each side. Requires usable >= 2.
inline LONG SplitLeadingSpan(LONG usable, double ratio) {
  return std::clamp<LONG>(static_cast<LONG>(std::llround(usable * ratio)), 1, usable - 1);
}

// Pixel bounds for automatic movement from the saved split ratio, within
// the same ratio limits used for manual resizing.
inline std::pair<LONG, LONG> AdaptiveSplitSpanBounds(LONG usable, double savedRatio, double deviation) {
  const double ratio = ClampSplitRatio(savedRatio);
  return {SplitLeadingSpan(usable, ClampSplitRatio(ratio - deviation)),
          SplitLeadingSpan(usable, ClampSplitRatio(ratio + deviation))};
}

// How one split may fit minimum sizes: its saved ratio, and whether (and how
// far) it may move from it.
struct SplitFit {
  double ratio = 0.5;
  bool adaptive = false;
  double deviation = 0.0;  // Of the usable span, when adaptive.
};

// The first side's span in `usable` pixels: the saved ratio's, unless that
// leaves a side below its minimum; then, for an adaptive split, the nearest
// span inside the adaptive range that fits both. No value when nothing does.
inline std::optional<LONG> FitSplitFirstSpan(const SplitFit& fit, LONG usable, LONG firstMinimum, LONG secondMinimum) {
  const LONG preferred = SplitLeadingSpan(usable, ClampSplitRatio(fit.ratio));
  if (preferred >= firstMinimum && usable - preferred >= secondMinimum) return preferred;
  if (!fit.adaptive) return std::nullopt;
  const auto [minimumAdaptive, maximumAdaptive] = AdaptiveSplitSpanBounds(usable, fit.ratio, fit.deviation);
  const LONG lower = std::max(firstMinimum, minimumAdaptive);
  const LONG upper = std::min(usable - secondMinimum, maximumAdaptive);
  if (lower > upper) return std::nullopt;
  return std::clamp(preferred, lower, upper);
}

// The smallest span along which a split can leave its sides `first` and
// `second` pixels (FitSplitFirstSpan succeeds); `maximumSpan` + 1 when no span
// up to `maximumSpan` can.
inline LONG RequiredSplitSpan(const SplitFit& fit, LONG gap, LONG first, LONG second, LONG maximumSpan) {
  auto fits = [&](LONG span) {
    if (span < 2) return false;
    const LONG usable = UsableSplitSpan(span, gap);
    return usable >= 2 && FitSplitFirstSpan(fit, usable, first, second).has_value();
  };
  if (!fits(maximumSpan)) return maximumSpan + 1;
  LONG low = 2;
  LONG high = maximumSpan;
  while (low < high) {
    const LONG middle = low + (high - low) / 2;
    if (fits(middle)) {
      high = middle;
    } else {
      low = middle + 1;
    }
  }
  return low;
}

// One split above a new BSP window's tile, as MultiWM publishes it to the
// companion: only splits with windows on both sides, from the root down. Fixed
// layout (CompanionAbi).
struct BspPathSplit {
  double ratio;               // The saved first-side share.
  int32_t otherMinimumWidth;  // The other side's smallest size, its own
  int32_t otherMinimumHeight; // adaptive splits free to move.
  uint8_t alongX;             // 1: side by side.
  uint8_t windowFirst;        // 1: the new window is on the first side.
  uint8_t adaptive;           // 1: the split may move to fit minimums.
  uint8_t reserved0;
  uint32_t reserved1;
};
static_assert(sizeof(BspPathSplit) == 24, "BspPathSplit layout");

namespace Detail {

inline constexpr uint32_t kMaximumPathDepth = 64;

// The smallest size of the new window's side below each split of `path`, as
// MultiWM's planner measures a subtree: along a split the smallest span that
// leaves both sides their minimums, across it the larger side. below[count] is
// the window's own minimum.
inline void MeasureBelowPath(LONG gap, double deviation, const BspPathSplit* path, uint32_t count, SIZE window,
                             LONG maximumSpan, SIZE* below) {
  below[count] = window;
  for (uint32_t k = count; k-- > 0;) {
    const BspPathSplit& split = path[k];
    const SIZE other{std::max<LONG>(1, split.otherMinimumWidth), std::max<LONG>(1, split.otherMinimumHeight)};
    const SIZE& first = split.windowFirst ? below[k + 1] : other;
    const SIZE& second = split.windowFirst ? other : below[k + 1];
    SIZE combined{std::max(first.cx, second.cx), std::max(first.cy, second.cy)};
    LONG& along = split.alongX ? combined.cx : combined.cy;
    along = RequiredSplitSpan({split.ratio, split.adaptive != 0, deviation}, gap, split.alongX ? first.cx : first.cy,
                              split.alongX ? second.cx : second.cy, maximumSpan);
    below[k] = combined;
  }
}

// Cuts `region` at `split` as the planner does and keeps the new window's side,
// whose minimum is `window`. False when the split can't fit both sides.
inline bool CutTowardWindow(RECT& region, const BspPathSplit& split, LONG gap, double deviation, SIZE window) {
  LONG& start = split.alongX ? region.left : region.top;
  LONG& end = split.alongX ? region.right : region.bottom;
  const LONG span = end - start;
  if (span < 2) return false;
  const LONG usable = UsableSplitSpan(span, gap);
  const LONG windowMinimum = split.alongX ? window.cx : window.cy;
  const LONG otherMinimum = std::max<LONG>(1, split.alongX ? split.otherMinimumWidth : split.otherMinimumHeight);
  const std::optional<LONG> firstSpan =
      FitSplitFirstSpan({split.ratio, split.adaptive != 0, deviation}, usable,
                        split.windowFirst ? windowMinimum : otherMinimum, split.windowFirst ? otherMinimum : windowMinimum);
  if (!firstSpan) return false;
  const LONG divider = start + *firstSpan;
  if (split.windowFirst) {
    end = divider;
  } else {
    start = divider + (span - usable);
  }
  return true;
}

}  // namespace Detail

// The tile a new BSP window with minimum `minWidth` x `minHeight` gets in
// `area`, below `path` (count splits, root first): every split along the path
// fits as MultiWM's planner fits it (FitSplitFirstSpan), with the window's own
// minimum and the other sides' published ones. The width depends on the side-
// by-side splits only and the height on the stacked ones, so the result is the
// planner's exactly. No value when the window can't fit there. Pure.
inline std::optional<RECT> SolveBspPath(const RECT& area, LONG gap, double deviation, const BspPathSplit* path,
                                        uint32_t count, LONG minWidth, LONG minHeight) {
  if (count > Detail::kMaximumPathDepth) return std::nullopt;
  const SIZE window{std::max<LONG>(1, minWidth), std::max<LONG>(1, minHeight)};
  SIZE below[Detail::kMaximumPathDepth + 1];
  Detail::MeasureBelowPath(gap, deviation, path, count, window,
                           std::max(area.right - area.left, area.bottom - area.top), below);
  RECT region = area;
  for (uint32_t k = 0; k < count; ++k) {
    if (!Detail::CutTowardWindow(region, path[k], gap, deviation, below[k + 1])) return std::nullopt;
  }
  if (region.right - region.left < window.cx || region.bottom - region.top < window.cy) return std::nullopt;
  return region;
}

}  // namespace SharedSplits

#endif  // MULTIWM_SHARED_SPLIT_GEOMETRY_H
// WHAM@5 end 2
// WHAM@5 begin 3: src/shared/companion_snapshot.h
// The read-only snapshot MultiWM publishes for MultiWM Companion.
//
// MultiWM (64-bit, one writer) fills a named shared-memory section; the companion
// (loaded into every process, 32- or 64-bit, many readers) maps it read-only.
// Nothing ever flows back to MultiWM.
//
// Both mods compile this file: only Windows and the standard library here.
// Include shared/window_rules.h and shared/split_geometry.h first.
//
// ABI rules: every field has a fixed width and offset, checked below for both
// bitnesses. A change that moves or reinterprets a field bumps kAbiMajor and the
// object names; appending into reserved space bumps kAbiMinor.

#ifndef MULTIWM_SHARED_COMPANION_SNAPSHOT_H
#define MULTIWM_SHARED_COMPANION_SNAPSHOT_H

#include <windows.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace CompanionAbi {

// The section exists while MultiWM or any reader holds a handle, so it outlives a
// MultiWM restart; a restarted MultiWM reopens it and rewrites the header.
inline constexpr wchar_t kSectionName[] = L"Local\\MultiWM.Companion.Snapshot.v6";
// Held open by MultiWM only. Readers probe it to tell a live MultiWM from a
// section that only their own handles keep alive.
inline constexpr wchar_t kAliveEventName[] = L"Local\\MultiWM.Companion.Alive.v6";

inline constexpr uint32_t kMagic = 0x5343574D;  // "MWCS"
inline constexpr uint16_t kAbiMajor = 6;
inline constexpr uint16_t kAbiMinor = 0;

inline constexpr uint32_t kMaxWorkspaces = 16;
inline constexpr uint32_t kMaxTrackedWindows = 2048;
inline constexpr uint32_t kRulesCapacity = 32768;
inline constexpr uint32_t kMaxRestoreWindows = 128;
inline constexpr uint32_t kMaxBspArrivalTiles = 4;
inline constexpr uint32_t kMaxBspPathSplits = 16;

enum SnapshotFlags : uint32_t {
  // MultiWM's WM thread is running and the rest of the body is meaningful.
  kFlagRunning = 1u << 0,
  // Automatic management: unknown windows are admitted, so hints apply.
  kFlagAutomaticMode = 1u << 1,
  // Every rule the companion needs fit into `rules`. Without it a missing
  // Exclude rule could be ignored, so readers give no hints.
  kFlagRulesComplete = 1u << 2,
  // Every window MultiWM knows is listed in `trackedWindows`.
  kFlagTrackedComplete = 1u << 3,
  // A window Automatic overflow floats is centred at its workspace's
  // floatingFrame (FailureCenter). Clear when overflow routes such windows to
  // another monitor or desktop instead; they then float where they are. With
  // one display and desktop creation off, monitor-only routing centres directly.
  kFlagOverflowCentersFloat = 1u << 4,
};

// The current desktop's workspace on one monitor. All rectangles are physical
// screen pixels; frames are visible DWM frames, not GetWindowRect rectangles.
struct WorkspaceHint {
  RECT monitorRect;    // Readers find the workspace by monitor rectangle.
  RECT workArea;       // What MultiWM plans in: monitor work area minus insets.
  RECT tiledFrame;     // Valid when hasTiledFrame: the next tiled slot.
  // Valid when hasFloatingFrame: the default floating size centred in the work
  // area, before size rules. Where a new window goes in the Floating layout, and
  // where overflow centres a window it floats (see kFlagOverflowCentersFloat).
  RECT floatingFrame;
  uint32_t dpi;        // Effective DPI of the monitor.
  uint8_t floatingLayout;    // The workspace uses the Floating layout.
  uint8_t hasTiledFrame;
  uint8_t hasFloatingFrame;
  // Tiled layouts: the next new window will float regardless of its size,
  // because the limit is reached or even a minimal window cannot fit.
  uint8_t newWindowFloats;
};

enum class WindowTreatment : uint32_t {
  Unknown,  // Missing/ambiguous ownership: never infer new admission from it.
  Tiled,    // Includes suspended tiled members keeping a saved slot.
  Floating,
  Ignored,
};

// Sorted by HWND. Ownership is model state, not a claim that a native desktop
// move has completed. Destination hints describe this snapshot's desktop only.
struct TrackedWindowHint {
  uint32_t hwnd;
  uint32_t processId;
  GUID desktopId;
  RECT monitorRect;  // Owning workspace's monitor; zero if no longer attached.
  WindowTreatment treatment;
  // Restored resize borders, retained across hiding and desktop changes. Zero
  // DPI means unavailable; readers must also match the destination's DPI.
  RECT frameMargins;
  uint32_t marginsDpi;
};

// A suspended member of a published workspace, and its planned restore tile.
struct RestoreWindowHint {
  uint32_t hwnd;
  uint32_t processId;
  uint32_t workspaceIndex;
  uint32_t hasFrame;  // 1 when `frame` is valid.
  // The window's tile once restored: MultiWM's own Restore on a copy of the
  // workspace, planned with the window's cached size limits. Absent when that
  // plan is unsatisfiable (MultiWM would then move or float the window).
  RECT frame;
};

// One tile a BSP arrival may get, for a window whose size limits MultiWM
// doesn't know yet. Include shared/split_geometry.h first.
struct BspArrivalTile {
  RECT tile;  // At the saved ratios: the tile a window with no minimum gets.
  // The splits above the tile (SharedSplits::SolveBspPath), root first. Fitting
  // the window's own minimum to them gives MultiWM's tile exactly. A length past
  // kMaxBspPathSplits means the path was too deep to publish.
  uint32_t pathLength;
  uint32_t reserved0;
  SharedSplits::BspPathSplit path[kMaxBspPathSplits];
};

// The tiles MultiWM tries for the next arrival on a BSP workspace, in its order:
// the split tile (focused or largest), that split turned, the largest other
// tile, and that split turned. A window takes the first one that fits it and
// floats when none does.
struct BspArrivalTiles {
  uint32_t count;  // Up to kMaxBspArrivalTiles.
  // 0 while another arrival hasn't settled: arrangement may then turn that one's
  // split instead, so only the first tile, which needs no turn, is certain.
  uint32_t othersSettled;
  int32_t gap;           // The gap MultiWM plans with, physical pixels.
  uint32_t reserved0;
  double adaptiveDeviation;  // How far adaptive splits may move (SharedSplits::SplitFit).
  BspArrivalTile tiles[kMaxBspArrivalTiles];
};

// Where a BSP arrival whose minimum is `minWidth` x `minHeight` goes.
struct BspArrivalChoice {
  enum class Outcome { Tile, Floats, Unknown } outcome = Outcome::Unknown;
  RECT tile{};  // Outcome::Tile: the whole tile, before a maximum size centres the window in it.
};

// Decides as MultiWM's arrangement does: tries `tiles` in its order, fitting
// the minimum to each one's published splits with the planner's own arithmetic
// (SharedSplits::SolveBspPath). The first that fits is MultiWM's tile exactly;
// none means MultiWM floats the window. Unknown when the snapshot can't say: no
// tiles, a path too deep to publish, or a later tile while another arrival
// hasn't settled (arrangement may turn that one's split instead). Pure.
inline BspArrivalChoice ChooseBspArrivalTile(const BspArrivalTiles& tiles, const RECT& workArea, LONG minWidth,
                                             LONG minHeight) {
  const uint32_t count = tiles.count < kMaxBspArrivalTiles ? tiles.count : kMaxBspArrivalTiles;
  for (uint32_t i = 0; i < count; ++i) {
    const BspArrivalTile& tile = tiles.tiles[i];
    if ((i > 0 && !tiles.othersSettled) || tile.pathLength > kMaxBspPathSplits) return {};
    if (const auto frame = SharedSplits::SolveBspPath(workArea, tiles.gap, tiles.adaptiveDeviation, tile.path,
                                                      tile.pathLength, minWidth, minHeight)) {
      return {BspArrivalChoice::Outcome::Tile, *frame};
    }
  }
  if (count == 0 || !tiles.othersSettled) return {};
  return {BspArrivalChoice::Outcome::Floats, {}};
}

struct SnapshotBody {
  uint64_t generation;  // Increases with every publish.
  GUID desktopId;       // The desktop these workspaces belong to.
  uint32_t flags;       // SnapshotFlags.
  uint32_t workspaceCount;
  uint32_t trackedCount;
  uint32_t rulesBytes;
  uint32_t restoreWindowCount;
  // One bit per workspace. Zero from older writers.
  uint16_t bspWorkspaces;  // BSP tile hints may require the full tree to refit.
  uint16_t bspLocalFloats; // Definite BSP admission floats that stay on this monitor.
  WorkspaceHint workspaces[kMaxWorkspaces];
  // Every known window, including owners on other desktops. HWND values are
  // 32-bit significant on Windows. Duplicate owners publish Unknown treatment.
  TrackedWindowHint trackedWindows[kMaxTrackedWindows];
  uint8_t rules[kRulesCapacity];  // See SerializeRules.
  RestoreWindowHint restoreWindows[kMaxRestoreWindows];
  // Per workspace; count 0 unless its bspWorkspaces bit is set.
  BspArrivalTiles bspArrivals[kMaxWorkspaces];
};

struct SnapshotSection {
  uint32_t magic;
  uint16_t abiMajor;
  uint16_t abiMinor;
  uint32_t sectionSize;
  uint32_t writerPid;
  // Seqlock: odd while the writer is changing `body`. Readers copy the body and
  // keep the copy only if the value was even and unchanged around the copy.
  volatile LONG sequence;
  uint32_t reserved0;
  SnapshotBody body;
};

static_assert(kMaxWorkspaces <= 16, "Workspace masks use 16 bits");
static_assert(sizeof(WorkspaceHint) == 72, "WorkspaceHint layout");
static_assert(sizeof(TrackedWindowHint) == 64, "TrackedWindowHint layout");
static_assert(offsetof(TrackedWindowHint, frameMargins) == 44, "TrackedWindowHint margins");
static_assert(sizeof(RestoreWindowHint) == 32, "RestoreWindowHint layout");
static_assert(sizeof(BspArrivalTile) == 24 + sizeof(SharedSplits::BspPathSplit) * kMaxBspPathSplits, "BspArrivalTile layout");
static_assert(offsetof(BspArrivalTiles, tiles) == 24, "BspArrivalTiles layout");
static_assert(sizeof(BspArrivalTiles) == 24 + sizeof(BspArrivalTile) * kMaxBspArrivalTiles, "BspArrivalTiles layout");
static_assert(offsetof(SnapshotBody, bspArrivals) % 8 == 0, "SnapshotBody layout");
static_assert(offsetof(SnapshotBody, desktopId) == 8, "SnapshotBody layout");
static_assert(offsetof(SnapshotBody, workspaces) == 48, "SnapshotBody layout");
static_assert(offsetof(SnapshotBody, trackedWindows) == 48 + sizeof(WorkspaceHint) * kMaxWorkspaces, "SnapshotBody layout");
static_assert(sizeof(SnapshotBody) == 48 + sizeof(WorkspaceHint) * kMaxWorkspaces + sizeof(TrackedWindowHint) * kMaxTrackedWindows +
                                         kRulesCapacity + sizeof(RestoreWindowHint) * kMaxRestoreWindows +
                                         sizeof(BspArrivalTiles) * kMaxWorkspaces,
              "SnapshotBody layout");
static_assert(offsetof(SnapshotSection, sequence) == 16, "SnapshotSection layout");
static_assert(offsetof(SnapshotSection, body) == 24, "SnapshotSection layout");

// Security for both named objects: full access for the creating user and
// SYSTEM; read/map (section) or synchronize/query (event) for everyone,
// including AppContainer apps. No mandatory label: an unlabeled object counts
// as Medium with no-write-up, which already lets Low and AppContainer readers
// in. A Low label would only let a Low process of the same user write here,
// since it matches the owner entry.
inline constexpr wchar_t kSectionSddl[] =
    L"D:(A;;GA;;;OW)(A;;GA;;;SY)(A;;0x0005;;;WD)(A;;0x0005;;;AC)";
inline constexpr wchar_t kAliveEventSddl[] =
    L"D:(A;;GA;;;OW)(A;;GA;;;SY)(A;;0x00100001;;;WD)(A;;0x00100001;;;AC)";

// --- Seqlock -------------------------------------------------------------------

inline void BeginWrite(SnapshotSection* section) {
  // One writer only. Advance even after a predecessor died mid-write: never
  // reset the counter or briefly expose its unfinished body as readable.
  const LONG previous = section->sequence;
  InterlockedExchange(&section->sequence,
                      static_cast<LONG>((static_cast<uint32_t>(previous) + 1u) | 1u));
}

inline void EndWrite(SnapshotSection* section) {
  InterlockedIncrement(&section->sequence);  // Full barrier; the value is even again.
}

// Copies a consistent body into `*out`. Returns false when the writer was busy
// on every attempt; `*out` then holds partial data and must be discarded.
inline bool ReadConsistentBody(const SnapshotSection* section, SnapshotBody* out) {
  for (int attempt = 0; attempt < 4; ++attempt) {
    const LONG before = section->sequence;
    std::atomic_thread_fence(std::memory_order_acquire);
    if (before & 1) {
      YieldProcessor();
      continue;
    }
    std::memcpy(out, &section->body, sizeof(*out));
    std::atomic_thread_fence(std::memory_order_acquire);
    if (section->sequence == before) return true;
  }
  return false;
}

// Reads the body's generation and the writer's process ID as one consistent
// pair, without copying the body. Every publish increases the generation, so a
// reader holding a copy with the same pair can keep using it. The pair, not the
// generation alone: a restarted MultiWM counts again from zero. Returns false
// while a write is in progress; the caller then copies the body as usual.
inline bool ReadConsistentGeneration(const SnapshotSection* section, uint64_t* generation, uint32_t* writerPid) {
  const LONG before = section->sequence;
  std::atomic_thread_fence(std::memory_order_acquire);
  if (before & 1) return false;
  // Plain copies: a 64-bit read isn't atomic on x86, which the recheck covers.
  const uint64_t readGeneration = section->body.generation;
  const uint32_t readPid = section->writerPid;
  std::atomic_thread_fence(std::memory_order_acquire);
  if (section->sequence != before) return false;
  *generation = readGeneration;
  *writerPid = readPid;
  return true;
}

// --- Rules -----------------------------------------------------------------------
//
// Encoding, little-endian, no padding, rules back to back:
//   u8  treatment (WindowRuleTreatment)
//   u8  flags: bit 0 preserveFloatingSize, bit 1 useWorkspaceSize
//   i32 floatingSizeDip.width, i32 floatingSizeDip.height
//   then classNames, titleFragments, processNames, each as
//     u16 count, and per string: u16 length, then that many UTF-16 code units.

// The rule fields the companion needs, with the same member names as MultiWM's
// WindowRule so SharedRules matching works on both.
struct SnapshotRule {
  WindowRuleTreatment treatment = WindowRuleTreatment::Exclude;
  std::vector<std::wstring> classNames;
  std::vector<std::wstring> titleFragments;
  std::vector<std::wstring> processNames;
  bool preserveFloatingSize = true;
  bool useWorkspaceSize = false;
  struct {
    LONG width = 0;
    LONG height = 0;
  } floatingSizeDip;
};

// Trace-to-owner only affects drags of already-managed windows.
inline bool IsRuleUsefulToCompanion(WindowRuleTreatment treatment) {
  return treatment != WindowRuleTreatment::TraceToOwner;
}

namespace Detail {

class ByteWriter {
 public:
  ByteWriter(uint8_t* out, uint32_t capacity) : out_(out), capacity_(capacity) {}
  uint32_t Size() const { return size_; }
  bool Ok() const { return ok_; }

  void Bytes(const void* data, uint32_t size) {
    if (!ok_ || capacity_ - size_ < size) {
      ok_ = false;
      return;
    }
    std::memcpy(out_ + size_, data, size);
    size_ += size;
  }
  void U8(uint8_t value) { Bytes(&value, sizeof(value)); }
  void U16(uint16_t value) { Bytes(&value, sizeof(value)); }
  void I32(int32_t value) { Bytes(&value, sizeof(value)); }

  void Strings(const std::vector<std::wstring>& values) {
    if (values.size() > 0xFFFF) {
      ok_ = false;
      return;
    }
    U16(static_cast<uint16_t>(values.size()));
    for (const std::wstring& value : values) {
      if (value.size() > 0xFFFF) {
        ok_ = false;
        return;
      }
      U16(static_cast<uint16_t>(value.size()));
      Bytes(value.data(), static_cast<uint32_t>(value.size() * sizeof(wchar_t)));
    }
  }

 private:
  uint8_t* out_;
  uint32_t capacity_;
  uint32_t size_ = 0;
  bool ok_ = true;
};

class ByteReader {
 public:
  ByteReader(const uint8_t* data, uint32_t size) : data_(data), size_(size) {}
  bool AtEnd() const { return offset_ == size_; }
  bool Ok() const { return ok_; }

  void Bytes(void* out, uint32_t size) {
    if (!ok_ || size_ - offset_ < size) {
      ok_ = false;
      return;
    }
    std::memcpy(out, data_ + offset_, size);
    offset_ += size;
  }
  uint8_t U8() {
    uint8_t value = 0;
    Bytes(&value, sizeof(value));
    return value;
  }
  uint16_t U16() {
    uint16_t value = 0;
    Bytes(&value, sizeof(value));
    return value;
  }
  int32_t I32() {
    int32_t value = 0;
    Bytes(&value, sizeof(value));
    return value;
  }

  std::vector<std::wstring> Strings() {
    std::vector<std::wstring> values(U16());
    for (std::wstring& value : values) {
      value.resize(U16());
      Bytes(value.data(), static_cast<uint32_t>(value.size() * sizeof(wchar_t)));
      if (!ok_) return {};
    }
    return values;
  }

 private:
  const uint8_t* data_;
  uint32_t size_;
  uint32_t offset_ = 0;
  bool ok_ = true;
};

}  // namespace Detail

// Writes the rules the companion needs into `out`, in order. Returns the number
// of bytes written. `*complete` is false when a rule did not fit; the bytes
// written are then a valid prefix (whole rules only).
template <class Rule>
uint32_t SerializeRules(const std::vector<Rule>& rules, uint8_t* out, uint32_t capacity, bool* complete) {
  Detail::ByteWriter writer(out, capacity);
  uint32_t lastWholeRule = 0;
  bool allFit = true;
  for (const Rule& rule : rules) {
    if (!IsRuleUsefulToCompanion(rule.treatment)) continue;
    writer.U8(static_cast<uint8_t>(rule.treatment));
    writer.U8(static_cast<uint8_t>((rule.preserveFloatingSize ? 1 : 0) | (rule.useWorkspaceSize ? 2 : 0)));
    writer.I32(static_cast<int32_t>(rule.floatingSizeDip.width));
    writer.I32(static_cast<int32_t>(rule.floatingSizeDip.height));
    writer.Strings(rule.classNames);
    writer.Strings(rule.titleFragments);
    writer.Strings(rule.processNames);
    if (!writer.Ok()) {
      allFit = false;
      break;
    }
    lastWholeRule = writer.Size();
  }
  if (complete) *complete = allFit;
  return lastWholeRule;
}

// Decodes what SerializeRules wrote. Returns false, leaving `*out` empty, for
// malformed data.
inline bool DeserializeRules(const uint8_t* data, uint32_t size, std::vector<SnapshotRule>* out) {
  out->clear();
  Detail::ByteReader reader(data, size);
  while (reader.Ok() && !reader.AtEnd()) {
    SnapshotRule rule;
    const uint8_t treatment = reader.U8();
    if (treatment > static_cast<uint8_t>(WindowRuleTreatment::InitialPlacement)) {
      out->clear();
      return false;
    }
    rule.treatment = static_cast<WindowRuleTreatment>(treatment);
    const uint8_t flags = reader.U8();
    rule.preserveFloatingSize = (flags & 1) != 0;
    rule.useWorkspaceSize = (flags & 2) != 0;
    rule.floatingSizeDip.width = reader.I32();
    rule.floatingSizeDip.height = reader.I32();
    rule.classNames = reader.Strings();
    rule.titleFragments = reader.Strings();
    rule.processNames = reader.Strings();
    if (reader.Ok()) out->push_back(std::move(rule));
  }
  if (!reader.Ok()) out->clear();
  return reader.Ok();
}

}  // namespace CompanionAbi

#endif  // MULTIWM_SHARED_COMPANION_SNAPSHOT_H
// WHAM@5 end 3
// WHAM@5 begin 4: src/shared/geometry.h
// Geometry and tracking-limit conversion used by both mods. All frame sizes
// and rectangles are physical pixels unless a DPI is explicitly supplied.
// No window queries, placement, model state or layout solver lives here.
#ifndef MULTIWM_SHARED_GEOMETRY_H
#define MULTIWM_SHARED_GEOMETRY_H

#include <windows.h>
#include <algorithm>
#include <cstdlib>
#include <optional>

namespace SharedGeometry {

// Visible-frame limits. A maximum of zero means the app kept the unrestricted
// system default; a reported maximum is never allowed below the minimum.
struct FrameLimits {
  LONG minWidth = 1;
  LONG minHeight = 1;
  LONG maxWidth = 0;
  LONG maxHeight = 0;
};

inline LONG Width(const RECT& rect) { return rect.right - rect.left; }
inline LONG Height(const RECT& rect) { return rect.bottom - rect.top; }

inline POINT RectCenter(const RECT& rect) {
  return {rect.left + (rect.right - rect.left) / 2,
          rect.top + (rect.bottom - rect.top) / 2};
}

inline RECT CenteredRect(POINT center, LONG width, LONG height) {
  width = std::max<LONG>(1, width);
  height = std::max<LONG>(1, height);
  const LONG left = center.x - width / 2;
  const LONG top = center.y - height / 2;
  return {left, top, left + width, top + height};
}

inline RECT CenteredIn(const RECT& area, LONG width, LONG height) {
  return CenteredRect(RectCenter(area), width, height);
}

inline RECT ClampToArea(RECT rect, const RECT& area) {
  const LONG width = std::min(std::max<LONG>(1, rect.right - rect.left),
                              std::max<LONG>(1, area.right - area.left));
  const LONG height = std::min(std::max<LONG>(1, rect.bottom - rect.top),
                               std::max<LONG>(1, area.bottom - area.top));
  LONG left = std::max(rect.left, area.left);
  LONG top = std::max(rect.top, area.top);
  if (left + width > area.right) left = area.right - width;
  if (top + height > area.bottom) top = area.bottom - height;
  return {left, top, left + width, top + height};
}

inline LONG ScaleDip(LONG value, UINT dpi) {
  return MulDiv(value, static_cast<int>(dpi ? dpi : 96), 96);
}

// A `width` x `height` frame centred in `area` and kept inside it: where a new
// or overflowed floating window goes.
inline RECT CenteredClampedIn(const RECT& area, LONG width, LONG height) {
  return ClampToArea(CenteredIn(area, width, height), area);
}

// The size an override-size window rule gives a centred floating window: the
// whole work area for "Workspace", else the rule's DIP size at `dpi`. `Rule` is
// MultiWM's WindowRule or the snapshot's SnapshotRule; both have
// `useWorkspaceSize` and `floatingSizeDip.width/height`.
template <class Rule>
SIZE RuleFloatingSize(const Rule& rule, const RECT& workArea, UINT dpi) {
  if (rule.useWorkspaceSize) return {Width(workArea), Height(workArea)};
  return {ScaleDip(rule.floatingSizeDip.width, dpi), ScaleDip(rule.floatingSizeDip.height, dpi)};
}

// A window's invisible resize borders: how far each edge of its visible (DWM)
// frame lies inside its window rectangle. Physical pixels.
inline RECT FrameMargins(const RECT& windowRect, const RECT& frame) {
  return {frame.left - windowRect.left, frame.top - windowRect.top,
          windowRect.right - frame.right, windowRect.bottom - frame.bottom};
}

// The window rectangle around visible frame `frame` with resize borders `margins`.
inline RECT AddFrameMargins(const RECT& frame, const RECT& margins) {
  return {frame.left - margins.left, frame.top - margins.top,
          frame.right + margins.right, frame.bottom + margins.bottom};
}

// The visible frame inside window rectangle `windowRect` with resize borders `margins`.
inline RECT RemoveFrameMargins(const RECT& windowRect, const RECT& margins) {
  return {windowRect.left + margins.left, windowRect.top + margins.top,
          windowRect.right - margins.right, windowRect.bottom - margins.bottom};
}

// How close, in physical pixels, every edge of a window must be to its
// monitor's for the window to count as covering it (CoversMonitor), and its
// size to the monitor's to count as monitor-sized (IsMonitorSized).
inline constexpr LONG kCoversMonitorTolerance = 2;

// True when `windowRect` is `monitorRect`'s size, within the tolerance, wherever
// it is: an app asking for fullscreen without maximizing. Both mods read
// fullscreen this way, so a window mid-way through moving and sizing itself to
// its monitor already counts. Exact, not "at least": with an auto-hidden
// taskbar, a window filling the work area is larger than its monitor by its
// invisible resize borders, and isn't fullscreen. Compare with the monitor the
// window is on, never another, or a window the size of a smaller monitor would
// count on a larger one.
inline bool IsMonitorSized(const RECT& windowRect, const RECT& monitorRect) {
  return std::abs((windowRect.right - windowRect.left) - (monitorRect.right - monitorRect.left)) <=
             kCoversMonitorTolerance &&
         std::abs((windowRect.bottom - windowRect.top) - (monitorRect.bottom - monitorRect.top)) <=
             kCoversMonitorTolerance;
}

// True when `windowRect` covers `monitorRect`, every edge within the tolerance:
// how an app in fullscreen without maximizing looks.
inline bool CoversMonitor(const RECT& windowRect, const RECT& monitorRect) {
  return std::abs(windowRect.left - monitorRect.left) <= kCoversMonitorTolerance &&
         std::abs(windowRect.top - monitorRect.top) <= kCoversMonitorTolerance &&
         std::abs(windowRect.right - monitorRect.right) <= kCoversMonitorTolerance &&
         std::abs(windowRect.bottom - monitorRect.bottom) <= kCoversMonitorTolerance;
}

// The area to tile in, given `workArea` (the monitor's work area minus the
// insets) on `monitorRect`. A tile filling a work area that covers the whole
// monitor would look fullscreen (CoversMonitor), so then every edge closer than
// the tolerance is moved in to one pixel past it, evenly. That only happens
// with a hidden taskbar and insets of about zero; any other area is unchanged.
inline RECT KeepTilesOffWholeMonitor(RECT workArea, const RECT& monitorRect) {
  if (!CoversMonitor(workArea, monitorRect)) return workArea;
  constexpr LONG margin = kCoversMonitorTolerance + 1;
  workArea.left = std::max(workArea.left, monitorRect.left + margin);
  workArea.top = std::max(workArea.top, monitorRect.top + margin);
  workArea.right = std::min(workArea.right, monitorRect.right - margin);
  workArea.bottom = std::min(workArea.bottom, monitorRect.bottom - margin);
  return workArea;
}

// The track sizes Windows offers a window before it answers WM_GETMINMAXINFO,
// in the window's own DPI units: the system minimum and maximum, the maximum
// never below the minimum. Both mods' size-limit probes start from these.
struct TrackingDefaults {
  POINT minimum;
  POINT maximum;
};

inline TrackingDefaults DefaultTrackingLimits(UINT windowDpi) {
  const UINT dpi = windowDpi ? windowDpi : 96;
  const LONG minWidth = std::max<LONG>(1, GetSystemMetricsForDpi(SM_CXMINTRACK, dpi));
  const LONG minHeight = std::max<LONG>(1, GetSystemMetricsForDpi(SM_CYMINTRACK, dpi));
  return {{minWidth, minHeight},
          {std::max<LONG>(minWidth, GetSystemMetricsForDpi(SM_CXMAXTRACK, dpi)),
           std::max<LONG>(minHeight, GetSystemMetricsForDpi(SM_CYMAXTRACK, dpi))}};
}

// Convert an application's WM_GETMINMAXINFO reply and the supplied defaults
// from window-DPI units to physical frame sizes. Decoration spans are already
// physical. Callers obtain the metrics and send the message using their own
// timeout/reentrancy policy; both DPIs must be nonzero.
inline FrameLimits TrackingLimitsToFrame(
    const MINMAXINFO& reply, POINT systemMinimum, POINT systemMaximum,
    UINT windowDpi, UINT physicalDpi, LONG decorationWidth, LONG decorationHeight) {
  auto toPhysical = [&](LONG windowUnits) {
    return MulDiv(windowUnits, static_cast<int>(physicalDpi), static_cast<int>(windowDpi));
  };
  FrameLimits frame;
  frame.minWidth = std::max<LONG>(
      1, toPhysical(std::max(systemMinimum.x, reply.ptMinTrackSize.x)) - decorationWidth);
  frame.minHeight = std::max<LONG>(
      1, toPhysical(std::max(systemMinimum.y, reply.ptMinTrackSize.y)) - decorationHeight);
  // Identify an explicit maximum in the window's units, before scaling.
  if (reply.ptMaxTrackSize.x < systemMaximum.x) {
    frame.maxWidth = std::max<LONG>(frame.minWidth, toPhysical(reply.ptMaxTrackSize.x) - decorationWidth);
  }
  if (reply.ptMaxTrackSize.y < systemMaximum.y) {
    frame.maxHeight = std::max<LONG>(frame.minHeight, toPhysical(reply.ptMaxTrackSize.y) - decorationHeight);
  }
  return frame;
}

// The size within `limits` nearest to `size`.
inline SIZE ClampSizeToLimits(SIZE size, const FrameLimits& limits) {
  LONG width = std::max(size.cx, limits.minWidth);
  LONG height = std::max(size.cy, limits.minHeight);
  if (limits.maxWidth > 0) width = std::min(width, limits.maxWidth);
  if (limits.maxHeight > 0) height = std::min(height, limits.maxHeight);
  return {width, height};
}

// `frame` moved, not resized, to lie inside `area`. Along an axis where it is
// larger than `area`, it starts at the area's left or top edge, so a window's
// caption stays on screen.
inline RECT MoveInsideArea(const RECT& frame, const RECT& area) {
  auto start = [](LONG from, LONG span, LONG low, LONG high) {
    return span >= high - low ? low : std::clamp(from, low, high - span);
  };
  const LONG width = Width(frame);
  const LONG height = Height(frame);
  const LONG left = start(frame.left, width, area.left, area.right);
  const LONG top = start(frame.top, height, area.top, area.bottom);
  return {left, top, left + width, top + height};
}

// --- Floating placement -------------------------------------------------------
//
// Where a floating window goes. One rule for both mods: MultiWM places windows
// with it (RepairFloatingGeometry) and the companion predicts where MultiWM will
// put a new window with it (PredictWindowPlacement). Each mod reads the size
// limits its own way (docs/ARCHITECTURE.md).

// What sizes a floating placement.
enum class FloatingSizeSource {
  Remembered,  // The window's remembered floating size; without one, as Fallback.
  Fallback,    // An override-size rule's size, else the default floating size.
  Own,         // The window's current size, unless an override-size rule names
               // another and the window can resize. At its own size it is only
               // moved, and may extend past the work area.
};

// An override-size rule as it applies to one placement.
struct FloatingSizeRule {
  bool preserveSize = false;  // Keep the window's current size.
  SIZE size{};                // Otherwise this size (physical pixels).
};

// `rule` (MultiWM's WindowRule or the snapshot's SnapshotRule) at `dpi`.
template <class Rule>
FloatingSizeRule ResolveFloatingSizeRule(const Rule& rule, const RECT& workArea, UINT dpi) {
  if (rule.preserveFloatingSize) return {true, {}};
  return {false, RuleFloatingSize(rule, workArea, dpi)};
}

// Everything a floating placement depends on. Sizes and points are physical.
struct FloatingRequest {
  FloatingSizeSource source = FloatingSizeSource::Fallback;
  RECT workArea{};
  std::optional<SIZE> defaultSize;       // Floating Default Size at this DPI.
  std::optional<SIZE> remembered;        // The remembered size, at this DPI.
  std::optional<SIZE> current;           // The window's current size.
  bool canResize = true;
  std::optional<FloatingSizeRule> rule;  // A matching override-size rule.
  bool honorPreserveSize = true;         // False: a preserve-size rule is ignored.
  std::optional<POINT> center;           // Default: the work area's center.
};

struct FloatingPlacement {
  RECT frame{};
  bool moveOnly = false;       // Keeps its own size, unclamped; only moved.
  bool appliesLimits = false;  // Its size still has to fit the window's size
                               // limits (FitFloatingPlacementToLimits).
  POINT center{};              // Where it is centred, and the work area it is
  RECT workArea{};             // kept inside, for fitting it to those limits.
};

// The frame for `request`, before the window's size limits
// (FitFloatingPlacementToLimits). Centered on `center`; a frame that is resized
// is kept inside the work area. No value when the size it needs is unknown.
// Whether the limits apply is decided here: never to a window that is only
// moved or can't resize.
inline std::optional<FloatingPlacement> PlanFloatingPlacement(const FloatingRequest& request) {
  std::optional<FloatingSizeRule> rule = request.rule;
  if (rule && rule->preserveSize && !request.honorPreserveSize) rule.reset();
  const bool ruleNamesSize = rule && !rule->preserveSize;

  FloatingPlacement placement;
  std::optional<SIZE> size;
  if (request.source == FloatingSizeSource::Own) {
    if (!request.current) return std::nullopt;
    placement.moveOnly = !ruleNamesSize || !request.canResize;
    size = placement.moveOnly ? *request.current : rule->size;
  } else if (request.source == FloatingSizeSource::Remembered && request.remembered) {
    size = request.remembered;
  } else if (rule && rule->preserveSize && request.current) {
    size = request.current;
  } else {
    size = ruleNamesSize ? std::optional<SIZE>{rule->size} : request.defaultSize;
  }
  if (!request.canResize && request.current) size = request.current;
  if (!size) return std::nullopt;

  placement.appliesLimits = !placement.moveOnly && request.canResize;
  placement.center = request.center ? *request.center : RectCenter(request.workArea);
  placement.workArea = request.workArea;
  placement.frame = CenteredRect(placement.center, size->cx, size->cy);
  if (!placement.moveOnly) placement.frame = ClampToArea(placement.frame, request.workArea);
  return placement;
}

// `placement`'s frame with its size fitted to the window's size `limits`, still
// centred where it was planned and kept inside the work area; a minimum larger
// than the work area overhangs its right or bottom edge (MoveInsideArea).
// Unchanged when the limits don't apply or the planned size already fits them.
inline RECT FitFloatingPlacementToLimits(const FloatingPlacement& placement, const FrameLimits& limits) {
  if (!placement.appliesLimits) return placement.frame;
  const SIZE planned{Width(placement.frame), Height(placement.frame)};
  const SIZE size = ClampSizeToLimits(planned, limits);
  if (size.cx == planned.cx && size.cy == planned.cy) return placement.frame;
  return MoveInsideArea(CenteredRect(placement.center, size.cx, size.cy), placement.workArea);
}

// Centre one axis inside its cell when a maximum caps it. Returns whether the
// axis changed; integer rounding deliberately matches the layout planner.
inline bool CenterWithinMaximumSize(LONG& start, LONG& end, LONG maximum) {
  if (maximum <= 0 || end - start <= maximum) return false;
  start += (end - start - maximum) / 2;
  end = start + maximum;
  return true;
}

}  // namespace SharedGeometry

#endif  // MULTIWM_SHARED_GEOMETRY_H
// WHAM@5 end 4

// WHAM@5 begin 5: src/companion/snapshot_reader.h
// Reads MultiWM's companion snapshot (shared/companion_snapshot.h).
// Part of the MultiWM Companion mod; see the include order in src/companion/multiwm-companion.cpp.
//
// Runs on application threads, any number at once, so every access goes through
// one lock. The lock is never held across a call that could run application code.

namespace Companion {

enum class PlacementIntent { Admission, Restore, Arrival, KeepFloating, Unavailable };

// What one placement decision needs from the snapshot, copied out so the lock
// is released before the window is touched.
struct HintInputs {
  uint32_t flags = 0;
  bool tracked = false;  // MultiWM already has a record for the window.
  bool bspLayout = false;
  bool bspFloatsLocally = false;
  // BSP: the tiles an arrival tries, in MultiWM's order; count 0 when unknown.
  CompanionAbi::BspArrivalTiles bspArrivals{};
  PlacementIntent intent = PlacementIntent::Admission;
  std::optional<CompanionAbi::WorkspaceHint> workspace;  // The workspace on the window's monitor.
  // Where MultiWM will tile the window once restored (visible frame). Absent
  // when MultiWM can't tell, or the window isn't a suspended member.
  std::optional<RECT> restoreFrame;
  // The window's resize borders as MultiWM measured them while it was restored,
  // with the DPI they belong to. Absent when MultiWM never measured them.
  std::optional<RECT> restoreFrameMargins;
  uint32_t restoreMarginsDpi = 0;
  std::shared_ptr<const std::vector<CompanionAbi::SnapshotRule>> rules;
};

struct SnapshotReader {
  SRWLOCK lock = SRWLOCK_INIT;
  bool unloading = false;
  // Kept open while the mod is loaded: it keeps the section's name alive, so a
  // restarted MultiWM reopens this same section instead of creating a new one.
  HANDLE section = nullptr;
  const CompanionAbi::SnapshotSection* view = nullptr;
  // Opening is retried no sooner than this (GetTickCount64), so a process
  // showing many windows while MultiWM is off doesn't retry each time.
  ULONGLONG nextOpenAttempt = 0;
  // A consistent copy of the body; reused to keep the snapshot off app stacks.
  std::unique_ptr<CompanionAbi::SnapshotBody> body;
  // Which publish `body` holds. While MultiWM's generation and process ID still
  // match, a check reuses the copy instead of copying the whole body again.
  bool bodyCurrent = false;
  uint64_t bodyGeneration = 0;
  uint32_t bodyWriterPid = 0;
  // Decoded rules and the bytes they came from; decoded again only on change.
  std::vector<uint8_t> rulesBytes;
  std::shared_ptr<const std::vector<CompanionAbi::SnapshotRule>> rules;
};

static SnapshotReader g_reader;

constexpr ULONGLONG kOpenRetryMs = 2000;

// MultiWM holds this event while it runs. The section alone can't tell, because
// this process's own handle keeps it alive after MultiWM exits.
static bool IsMultiWmRunning() {
  const HANDLE alive = OpenEventW(SYNCHRONIZE, FALSE, CompanionAbi::kAliveEventName);
  if (!alive) return false;
  CloseHandle(alive);
  return true;
}

// Maps the section if it isn't mapped yet. Caller holds the lock exclusively.
static bool EnsureMappedLocked() {
  if (g_reader.view) return true;
  const ULONGLONG now = GetTickCount64();
  if (now < g_reader.nextOpenAttempt) return false;
  g_reader.nextOpenAttempt = now + kOpenRetryMs;

  const HANDLE section = OpenFileMappingW(FILE_MAP_READ, FALSE, CompanionAbi::kSectionName);
  if (!section) return false;
  auto* view = static_cast<const CompanionAbi::SnapshotSection*>(MapViewOfFile(section, FILE_MAP_READ, 0, 0, 0));
  MEMORY_BASIC_INFORMATION region{};
  if (!view || !VirtualQuery(view, &region, sizeof(region)) ||
      region.RegionSize < sizeof(CompanionAbi::SnapshotSection)) {
    if (view) UnmapViewOfFile(view);
    CloseHandle(section);
    return false;
  }
  g_reader.section = section;
  g_reader.view = view;
  if (!g_reader.body) g_reader.body = std::make_unique<CompanionAbi::SnapshotBody>();
  return true;
}

// Decodes the body's rules unless they are the bytes decoded last time.
// Caller holds the lock exclusively. Leaves `rules` null for malformed data.
static void RefreshRulesLocked(const CompanionAbi::SnapshotBody& body) {
  const uint32_t size = std::min(body.rulesBytes, CompanionAbi::kRulesCapacity);
  if (g_reader.rules && g_reader.rulesBytes.size() == size &&
      std::memcmp(g_reader.rulesBytes.data(), body.rules, size) == 0) {
    return;
  }
  g_reader.rulesBytes.assign(body.rules, body.rules + size);
  auto decoded = std::make_shared<std::vector<CompanionAbi::SnapshotRule>>();
  g_reader.rules = CompanionAbi::DeserializeRules(body.rules, size, decoded.get()) ? decoded : nullptr;
}

// Brings the cached body up to date with the published one. Most checks find it
// current and skip both the body copy and the rules comparison; only a new
// publish costs a full copy. Returns false, with no usable body, for an unknown
// ABI or a writer that stayed busy. Caller holds the lock exclusively.
static bool RefreshBodyLocked() {
  const CompanionAbi::SnapshotSection* section = g_reader.view;
  if (section->magic != CompanionAbi::kMagic || section->abiMajor != CompanionAbi::kAbiMajor) {
    g_reader.bodyCurrent = false;
    return false;
  }
  uint64_t generation = 0;
  uint32_t writerPid = 0;
  if (g_reader.bodyCurrent && CompanionAbi::ReadConsistentGeneration(section, &generation, &writerPid) &&
      generation == g_reader.bodyGeneration && writerPid == g_reader.bodyWriterPid) {
    return true;
  }
  if (!CompanionAbi::ReadConsistentBody(section, g_reader.body.get())) {
    g_reader.bodyCurrent = false;
    return false;
  }
  RefreshRulesLocked(*g_reader.body);
  // Remember which publish this copy is. If another write landed in between,
  // use the copy for this check but don't trust it for the next one.
  const uint64_t copied = g_reader.body->generation;
  g_reader.bodyCurrent = CompanionAbi::ReadConsistentGeneration(section, &generation, &writerPid) &&
                         generation == copied;
  g_reader.bodyGeneration = copied;
  g_reader.bodyWriterPid = writerPid;
  return true;
}

// Runs `read(body)` under the reader lock when a current snapshot is available.
// False, without calling `read`, when there is none: MultiWM not running, an
// unknown ABI, or a writer that stayed busy.
template <typename Read>
static bool ReadCurrentBody(Read&& read) {
  AcquireSRWLockExclusive(&g_reader.lock);
  const bool ok = !g_reader.unloading && EnsureMappedLocked() && IsMultiWmRunning() && RefreshBodyLocked();
  if (ok) read(*g_reader.body);
  ReleaseSRWLockExclusive(&g_reader.lock);
  return ok;
}

// The tracked entry for `hwnd`, or nullptr. Entries are sorted by HWND value.
static const CompanionAbi::TrackedWindowHint* FindTrackedWindow(const CompanionAbi::SnapshotBody& body, HWND hwnd) {
  const uint32_t windowValue = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(hwnd));
  const uint32_t trackedCount = std::min(body.trackedCount, CompanionAbi::kMaxTrackedWindows);
  const auto* trackedEnd = body.trackedWindows + trackedCount;
  const auto* window = std::lower_bound(body.trackedWindows, trackedEnd, windowValue,
      [](const auto& entry, uint32_t value) { return entry.hwnd < value; });
  return window != trackedEnd && window->hwnd == windowValue ? window : nullptr;
}

// Fills `*out` for a decision about `hwnd` on the monitor whose rectangle is
// `monitorRect`. Returns false when there is no usable snapshot: MultiWM not
// running, an unknown ABI, or a writer that stayed busy.
static bool ReadHintInputs(HWND hwnd, const RECT& monitorRect, HintInputs* out) {
  return ReadCurrentBody([&](const CompanionAbi::SnapshotBody& body) {
    out->flags = body.flags;
    out->tracked = false;
    out->intent = PlacementIntent::Admission;
    out->rules = g_reader.rules;
    out->workspace.reset();
    out->bspLayout = false;
    out->bspFloatsLocally = false;
    out->bspArrivals = {};
    out->restoreFrame.reset();
    out->restoreFrameMargins.reset();
    out->restoreMarginsDpi = 0;
    const uint32_t count = std::min(body.workspaceCount, CompanionAbi::kMaxWorkspaces);
    uint32_t workspaceIndex = count;
    for (uint32_t i = 0; i < count; ++i) {
      if (EqualRect(&body.workspaces[i].monitorRect, &monitorRect)) {
        out->workspace = body.workspaces[i];
        out->bspLayout = (body.bspWorkspaces & (1u << i)) != 0;
        out->bspFloatsLocally = (body.bspLocalFloats & (1u << i)) != 0;
        if (out->bspLayout) out->bspArrivals = body.bspArrivals[i];
        workspaceIndex = i;
        break;
      }
    }
    const uint32_t windowValue = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(hwnd));
    if (const auto* window = FindTrackedWindow(body, hwnd)) {
      out->tracked = true;
      out->intent = PlacementIntent::Unavailable;
      // A stale or ambiguous record must never fall back to new admission.
      if (window->processId == GetCurrentProcessId()) {
        if (window->marginsDpi) {
          out->restoreFrameMargins = window->frameMargins;
          out->restoreMarginsDpi = window->marginsDpi;
        }
        if (window->treatment == CompanionAbi::WindowTreatment::Floating) {
          out->intent = PlacementIntent::KeepFloating;
        } else if (window->treatment == CompanionAbi::WindowTreatment::Tiled && out->workspace &&
                   !IsEqualGUID(window->desktopId, GUID{}) && !IsEqualGUID(body.desktopId, GUID{}) &&
                   window->monitorRect.right > window->monitorRect.left &&
                   window->monitorRect.bottom > window->monitorRect.top) {
          const bool sameWorkspace = IsEqualGUID(window->desktopId, body.desktopId) &&
                                     EqualRect(&window->monitorRect, &out->workspace->monitorRect);
          out->intent = sameWorkspace ? PlacementIntent::Restore : PlacementIntent::Arrival;
        }
      }
    }
    if (out->intent == PlacementIntent::Restore) {
      const uint32_t restoreCount = std::min(body.restoreWindowCount, CompanionAbi::kMaxRestoreWindows);
      for (uint32_t i = 0; i < restoreCount; ++i) {
        const auto& restore = body.restoreWindows[i];
        if (restore.hwnd == windowValue && restore.processId == GetCurrentProcessId() &&
            restore.workspaceIndex == workspaceIndex) {
          if (restore.hasFrame) out->restoreFrame = restore.frame;
          break;
        }
      }
    }
  });
}

// What MultiWM's snapshot says about one window: its tracked entry (zero when
// untracked) and whether it has a restore entry. When MultiWM processes a
// window's show, this changes: an unknown window becomes tracked, a hidden
// member loses its restore entry, an arriving one gets its new desktop or
// monitor, an overflowed one becomes floating. Unrelated republishes don't.
struct WindowFacts {
  CompanionAbi::TrackedWindowHint entry{};
  bool restoreEntry = false;

  bool operator==(const WindowFacts& other) const {
    return restoreEntry == other.restoreEntry && std::memcmp(&entry, &other.entry, sizeof(entry)) == 0;
  }
};

// Fills `*out` for `hwnd`. Returns false when there is no usable snapshot.
static bool ReadWindowFacts(HWND hwnd, WindowFacts* out) {
  return ReadCurrentBody([&](const CompanionAbi::SnapshotBody& body) {
    *out = {};
    if (const auto* window = FindTrackedWindow(body, hwnd)) out->entry = *window;
    const uint32_t windowValue = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(hwnd));
    const uint32_t restoreCount = std::min(body.restoreWindowCount, CompanionAbi::kMaxRestoreWindows);
    for (uint32_t i = 0; i < restoreCount && !out->restoreEntry; ++i) {
      out->restoreEntry = body.restoreWindows[i].hwnd == windowValue;
    }
  });
}

// Unmaps the section. After this, ReadHintInputs always fails. Mod unload.
static void CloseSnapshotReader() {
  AcquireSRWLockExclusive(&g_reader.lock);
  g_reader.unloading = true;
  if (g_reader.view) UnmapViewOfFile(g_reader.view);
  g_reader.view = nullptr;
  if (g_reader.section) CloseHandle(g_reader.section);
  g_reader.section = nullptr;
  g_reader.body.reset();
  g_reader.bodyCurrent = false;
  g_reader.rules.reset();
  ReleaseSRWLockExclusive(&g_reader.lock);
}

}  // namespace Companion
// WHAM@5 end 5
// WHAM@5 begin 6: src/companion/placement.h
// Places a hidden window at the frame MultiWM will give it: the admission frame
// for an unknown window, or its published restore frame for a suspended member.
// Part of the MultiWM Companion mod; see the include order in src/companion/multiwm-companion.cpp.
//
// One-shot: one decision when a window goes from hidden to shown, nothing after.
// No per-window state is kept. A window MultiWM already knows (say, hidden to the
// tray) uses its own restore frame or a destination arrival frame. restoration.h
// separately prepares the normal rectangle for minimized/maximized restores.
// Unknown windows will be admitted as new, so the next slot is right on any show.
// MultiWM stays the authority: a wrong guess costs one move by MultiWM afterwards,
// the same as without the companion.

namespace Companion {

// Set while the companion itself moves a window, so its own calls never re-enter.
static thread_local bool g_placing = false;

// Native placement and minimum-size queries can run application code. Nested
// show calls must pass through until the outer decision and native call finish.
class PlacementScope {
 public:
  PlacementScope() { g_placing = true; }
  ~PlacementScope() { g_placing = false; }
  PlacementScope(const PlacementScope&) = delete;
  PlacementScope& operator=(const PlacementScope&) = delete;
};

using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
// The original NtUserSetWindowPos, filled in when the hook (companion/show_hooks.h)
// is applied; null until then or when win32u doesn't export it.
static SetWindowPos_t g_unhookedSetWindowPos = nullptr;
// The companion's own moves go through the public user32 SetWindowPos, so other
// mods' SetWindowPos hooks see them. Mobile Open Animation, for one, retargets its
// panel when an animating window moves; an unhooked call would leave the panel at
// the old rectangle. The companion's own NtUserSetWindowPos hook passes the call
// through because the caller holds PlacementScope. Tests redirect this pointer.
static SetWindowPos_t g_setWindowPosForOwnMoves = SetWindowPos;

// The process name of the current process: the companion only handles windows
// of its own process, so it never needs OpenProcess.
static std::wstring CurrentProcessName(HWND) {
  // Read as MultiWM reads other processes' names, so process rules match alike.
  static const std::wstring name = SharedRules::ProcessImageFileName(GetCurrentProcess());
  return name;
}

// Switches the calling thread to per-monitor DPI awareness for its lifetime.
// The snapshot is in physical pixels, and so are MultiWM's placements; without
// this, a DPI-unaware app's thread would see and set virtualized coordinates.
class PhysicalCoordinatesScope {
 public:
  PhysicalCoordinatesScope()
      : previous_(SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {}
  ~PhysicalCoordinatesScope() {
    if (previous_) SetThreadDpiAwarenessContext(previous_);
  }
  PhysicalCoordinatesScope(const PhysicalCoordinatesScope&) = delete;
  PhysicalCoordinatesScope& operator=(const PhysicalCoordinatesScope&) = delete;

 private:
  DPI_AWARENESS_CONTEXT previous_;
};

// --- Geometry ------------------------------------------------------------------------

using SharedGeometry::Height;
using SharedGeometry::Width;

// Use DWM's visible frame when available; hidden windows may have no frame yet.
static RECT FrameOf(HWND hwnd, const RECT& windowRect) {
  RECT frame{};
  if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(frame))) &&
      frame.right > frame.left && frame.bottom > frame.top) {
    return frame;
  }
  return windowRect;
}

// --- Decision -----------------------------------------------------------------------

// Cheap checks that need no snapshot: the structural part of MultiWM's
// WindowCanBeManaged, plus the companion's own limits. Returns nullptr when the
// window is a candidate, else the reason it isn't.
static const char* RejectStructurally(HWND hwnd, bool allowSuspended = false) {
  DWORD processId = 0;
  const DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
  // Moving another thread's window would wait on that thread mid-show.
  if (processId != GetCurrentProcessId() || threadId != GetCurrentThreadId()) return "not shown by its own thread";
  if (const char* reason = SharedRules::RejectNonTopLevel(hwnd)) return reason;
  if (!allowSuspended && (GetWindowLongPtrW(hwnd, GWL_STYLE) & (WS_MINIMIZE | WS_MAXIMIZE))) {
    return "minimized or maximized";
  }
  if (const char* reason = SharedRules::RejectToolOrShellWindow(hwnd)) return reason;
  if (!IsWindowEnabled(hwnd)) return "disabled";
  return nullptr;
}

// Asks the window for its tracking limits and converts them to frame geometry
// using `margins` (the invisible resize borders). Both admission and restoration
// use this. Caller holds PlacementScope and uses physical coordinates. Returns
// nothing when the callback changed the window's geometry or physical state
// while answering, since the decision would then be about a different window.
static std::optional<SharedGeometry::FrameLimits> QueryFrameLimits(HWND hwnd, UINT dpi, const RECT& margins) {
  RECT before{};
  if (!GetWindowRect(hwnd, &before)) return std::nullopt;
  const bool wasVisible = IsWindowVisible(hwnd) != FALSE;
  const LONG_PTR showState = GetWindowLongPtrW(hwnd, GWL_STYLE) & (WS_MINIMIZE | WS_MAXIMIZE);
  // The window answers in its own DPI units even to this per-monitor-aware
  // scope (measured: a DPI-unaware window on a 200% monitor reports half its
  // physical limits). Offer the defaults in those units and scale the reply to
  // `dpi`, the monitor's physical DPI, exactly as MultiWM's probe does.
  const UINT reportedWindowDpi = GetDpiForWindow(hwnd);
  const UINT windowDpi = reportedWindowDpi ? reportedWindowDpi : dpi;
  const SharedGeometry::TrackingDefaults defaults = SharedGeometry::DefaultTrackingLimits(windowDpi);
  MINMAXINFO limits{};
  limits.ptMinTrackSize = defaults.minimum;
  limits.ptMaxTrackSize = defaults.maximum;
  SendMessageW(hwnd, WM_GETMINMAXINFO, 0, reinterpret_cast<LPARAM>(&limits));
  RECT after{};
  if (RejectStructurally(hwnd, true) || !GetWindowRect(hwnd, &after) || !EqualRect(&before, &after) ||
      (IsWindowVisible(hwnd) != FALSE) != wasVisible ||
      (GetWindowLongPtrW(hwnd, GWL_STYLE) & (WS_MINIMIZE | WS_MAXIMIZE)) != showState) {
    return std::nullopt;
  }
  return SharedGeometry::TrackingLimitsToFrame(limits, defaults.minimum, defaults.maximum,
      windowDpi, dpi, margins.left + margins.right, margins.top + margins.bottom);
}

// One axis of FitSlotToLimits. `start`/`end` are the slot's edges, `areaStart`/
// `areaEnd` the work area's. Returns false when the minimum is larger than the
// whole work area on this axis.
static bool FitSlotAxis(LONG& start, LONG& end, LONG areaStart, LONG areaEnd, LONG minimum, LONG maximum) {
  const LONG size = end - start;
  // Capped below the slot: MultiWM centres it in its cell (CenterWithinMaximumSizes).
  if (SharedGeometry::CenterWithinMaximumSize(start, end, maximum)) return true;
  if (size >= minimum) return true;
  if (minimum > areaEnd - areaStart) return false;

  // Too small: MultiWM's constrained step takes the space from neighbouring
  // windows, so an edge on the work-area boundary stays and the edges facing
  // other windows move. With neighbours on both sides, MultiWM splits the growth
  // in proportion to their weights; growing from the centre approximates that.
  constexpr LONG kBoundaryTolerancePx = 1;
  const bool atStart = std::abs(start - areaStart) <= kBoundaryTolerancePx;
  const bool atEnd = std::abs(end - areaEnd) <= kBoundaryTolerancePx;
  if (atStart && !atEnd) {
    end = start + minimum;
  } else if (atEnd && !atStart) {
    start = end - minimum;
  } else {
    start -= (minimum - size) / 2;
    end = start + minimum;
  }
  if (start < areaStart) {
    end += areaStart - start;
    start = areaStart;
  }
  if (end > areaEnd) {
    start -= end - areaEnd;
    end = areaEnd;
  }
  return true;
}

static bool FitsMinimum(const RECT& region, const SharedGeometry::FrameLimits& limits) {
  return Width(region) >= limits.minWidth && Height(region) >= limits.minHeight;
}

// Where MultiWM will put a window whose size limits don't match the tiled
// `slot` it was predicted (frames, physical pixels):
//  - larger than its maximum: centred in the slot at the maximum, as MultiWM's
//    planner does, so the hint is exact;
//  - smaller than its minimum: grown at the edges that face other windows, an
//    approximation of MultiWM's constrained step, which MultiWM then refines.
// Returns nothing when the minimum doesn't fit the work area. BSP also
// defers growth to MultiWM, which can search for a split that fits.
static std::optional<RECT> FitSlotToLimits(
    const RECT& slot, const RECT& workArea, const SharedGeometry::FrameLimits& limits, bool allowGrowth) {
  if (!allowGrowth && !FitsMinimum(slot, limits)) return std::nullopt;
  RECT fitted = slot;
  if (!FitSlotAxis(fitted.left, fitted.right, workArea.left, workArea.right, limits.minWidth, limits.maxWidth) ||
      !FitSlotAxis(fitted.top, fitted.bottom, workArea.top, workArea.bottom, limits.minHeight, limits.maxHeight)) {
    return std::nullopt;
  }
  return fitted;
}


// How MultiWM treats the frame PredictFrame returns once it reads the window's
// size limits.
enum class TargetKind {
  Tile,      // A tiled slot, fitted to the limits as FitSlotToLimits does.
  Float,     // A floating frame, fitted by SharedGeometry::FitFloatingPlacementToLimits.
  KeepSize,  // Only moved; the app keeps its size.
};

// Where MultiWM will centre a new floating window, by the rule both mods share
// (SharedGeometry::PlanFloatingPlacement, as RepairFloatingGeometry uses it):
// sized by `source` from the window's matching size rule (`sizeRule`, may be
// null), its own frame `frame`, and the workspace's default floating frame. A
// preserve-size rule is ignored when `ignorePreserveSize` (FailureCenter, for a
// window overflow floats). Nothing when the size it needs is unknown.
static std::optional<SharedGeometry::FloatingPlacement> PredictFloatingPlacement(
    const CompanionAbi::WorkspaceHint& workspace, const CompanionAbi::SnapshotRule* sizeRule, const RECT& frame,
    bool resizable, SharedGeometry::FloatingSizeSource source, bool ignorePreserveSize) {
  SharedGeometry::FloatingRequest request;
  request.source = source;
  request.workArea = workspace.workArea;
  // The published frame is the default size centred and clamped in the work area.
  if (workspace.hasFloatingFrame) {
    request.defaultSize = SIZE{Width(workspace.floatingFrame), Height(workspace.floatingFrame)};
  }
  request.current = SIZE{Width(frame), Height(frame)};
  request.canResize = resizable;
  if (sizeRule) request.rule = SharedGeometry::ResolveFloatingSizeRule(*sizeRule, workspace.workArea, workspace.dpi);
  request.honorPreserveSize = !ignorePreserveSize;
  return SharedGeometry::PlanFloatingPlacement(request);
}

// Why there is no hint, which decides what happens to a window the companion
// is holding (settling) when a later show finds none:
//  - Unmanaged: MultiWM won't tile the window as asked (fullscreen, fixed-size,
//    a rule, Manual mode, a float), so the hold ends at the app's own frame;
//  - Undecided: a running MultiWM's snapshot can't say yet (another BSP
//    arrival is unsettled, the snapshot or a measurement is incomplete), and
//    MultiWM will still place the window, so the hold stays.
// A reason not known to be undecided counts as unmanaged, as every one did before.
enum class NoHint { Unmanaged, Undecided };

// The frame MultiWM will give `hwnd` (currently at `frame`), using its restore
// hint when known or Automatic discovery otherwise, and how MultiWM applies the
// window's size limits to it (`*kind`). `*floating` is the floating placement
// behind a floating frame, which the limits are fitted to. For a tiled slot it
// is the fallback if its minimum turns out larger than the work area and
// MultiWM floats it locally. Returns nothing, with `*reason` set, when MultiWM
// won't manage it or the snapshot can't say where it goes; `*noHint` (optional)
// says which.
static std::optional<RECT> PredictFrame(HWND hwnd, const RECT& frame, const HintInputs& inputs, TargetKind* kind,
                                        std::optional<SharedGeometry::FloatingPlacement>* floating,
                                        const char** reason, NoHint* noHint = nullptr) {
  using namespace CompanionAbi;
  *kind = TargetKind::Tile;
  floating->reset();
  if (noHint) *noHint = NoHint::Unmanaged;
  auto reject = [&](const char* why) -> std::optional<RECT> {
    *reason = why;
    return std::nullopt;
  };
  auto undecided = [&](const char* why) -> std::optional<RECT> {
    if (noHint) *noHint = NoHint::Undecided;
    return reject(why);
  };
  constexpr uint32_t kComplete = kFlagRunning | kFlagRulesComplete | kFlagTrackedComplete;
  if ((inputs.flags & kComplete) != kComplete || !inputs.rules) return undecided("no complete snapshot");
  if (!(inputs.flags & kFlagAutomaticMode)) return reject("Manual mode");
  if (!inputs.workspace) return undecided("no workspace on this monitor");

  const std::vector<SnapshotRule>& rules = *inputs.rules;
  SharedRules::LazyWindowFacts facts(hwnd, CurrentProcessName);
  auto matches = [&](WindowRuleTreatment treatment) {
    return SharedRules::FindFirstMatchingRule(rules, treatment, facts);
  };
  if (matches(WindowRuleTreatment::Exclude)) return reject("excluded by a rule");
  const WorkspaceHint& workspace = *inputs.workspace;
  if (inputs.tracked) {
    if (inputs.intent == PlacementIntent::KeepFloating) return reject("retaining this window's floating treatment");
    if (inputs.intent != PlacementIntent::Restore && inputs.intent != PlacementIntent::Arrival) {
      return undecided("known window has no usable ownership or treatment");
    }
    if (workspace.floatingLayout) return reject("known window's destination uses the Floating layout");
    if (!inputs.restoreFrameMargins || inputs.restoreMarginsDpi != workspace.dpi) {
      return undecided("no current restore border measurement");
    }
    if (inputs.intent == PlacementIntent::Arrival) {
      // Migration retains treatment and uses the same destination insertion as
      // admission. Never rerun opening rules or guess how overflow will route it.
      if (!workspace.hasTiledFrame || workspace.newWindowFloats ||
          Width(workspace.tiledFrame) <= 0 || Height(workspace.tiledFrame) <= 0) {
        return reject("no tiled arrival hint for the destination workspace");
      }
      *reason = "using MultiWM's destination arrival tile";
      return workspace.tiledFrame;
    }
    if (!inputs.restoreFrame || Width(*inputs.restoreFrame) <= 0 || Height(*inputs.restoreFrame) <= 0) {
      return reject("no restore tile for this workspace member");
    }
    *reason = "using MultiWM's restore tile";
    return inputs.restoreFrame;
  }
  if (matches(WindowRuleTreatment::InitialPlacement)) return reject("routed by an Open-on-monitor rule");
  const bool floatingRule =
      matches(WindowRuleTreatment::AlwaysFloating) || matches(WindowRuleTreatment::DefaultToFloating);
  const bool resizable = (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_SIZEBOX) != 0;
  if (!resizable && !floatingRule) return reject("fixed-size window");

  // Rule-floated windows are centered (RuleFloatedCenter), in every layout: at
  // the size a matching override-size rule names, else at the app's own size
  // without clamping. A window that can't resize keeps its size either way.
  using SharedGeometry::FloatingSizeSource;
  const SnapshotRule* sizeRule = matches(WindowRuleTreatment::FloatingPlacementOverride);
  if (floatingRule) {
    *floating = PredictFloatingPlacement(workspace, sizeRule, frame, resizable, FloatingSizeSource::Own, false);
    if (!*floating) return reject("no frame to centre");
    *kind = (*floating)->moveOnly ? TargetKind::KeepSize : TargetKind::Float;
    if (!(*floating)->moveOnly) *reason = "rule-floated at its size rule's size";
    return (*floating)->frame;
  }

  if (workspace.floatingLayout) {
    // NewWindowCenter: the default size unless a size rule says otherwise.
    *kind = TargetKind::Float;
    *floating = PredictFloatingPlacement(workspace, sizeRule, frame, resizable, FloatingSizeSource::Fallback, false);
    if (!*floating) return reject("no floating default");
    return (*floating)->frame;
  }

  // FailureCenter uses the default floating size for a new window. BSP floats
  // a failed split locally, while a release at the window limit uses overflow.
  if ((inputs.flags & kFlagOverflowCentersFloat) || inputs.bspFloatsLocally ||
      (inputs.bspLayout && workspace.hasTiledFrame)) {
    *floating = PredictFloatingPlacement(workspace, sizeRule, frame, resizable, FloatingSizeSource::Fallback, true);
  }
  if (!workspace.hasTiledFrame) {
    if (!workspace.newWindowFloats) return reject("no free tiled slot");
    if (!*floating) return reject("overflow routes the window");
    *kind = TargetKind::Float;
    *reason = inputs.bspFloatsLocally ? "BSP floats it locally" : "overflow floats it";
    return (*floating)->frame;
  }
  return workspace.tiledFrame;
}

static std::wstring ClassNameOf(HWND hwnd) {
  wchar_t className[128] = L"";
  GetClassNameW(hwnd, className, ARRAYSIZE(className));
  return className;
}

// Windhawk's per-mod logging switch gates this: with logging off, Wh_Log skips
// its arguments, so not even the class name is read.
static void LogDecision(HWND hwnd, const char* path, const char* outcome) {
  Wh_Log(L"%S hwnd=%p class=%s: %S", path, hwnd, ClassNameOf(hwnd).c_str(), outcome);
}

struct PredictedPlacement {
  RECT windowRect;
  bool keepSize;
};

// Where `target`, a frame of `kind` from PredictFrame, ends up once MultiWM
// applies the window's `limits`: tiles by FitSlotToLimits, floating frames by
// the rule both mods share, and a BSP arrival (new, or moved here) in the first
// tile MultiWM tries that fits it. Restores already carry their exact tile. No
// value, with `*reason` and `*why` set, when the companion should give no hint;
// a float MultiWM will fall back to sets `*reason` too.
static std::optional<RECT> FitTargetToLimits(
    const RECT& target, TargetKind kind, const std::optional<SharedGeometry::FloatingPlacement>& floating,
    const HintInputs& inputs, const SharedGeometry::FrameLimits& limits, const char** reason, NoHint* why) {
  const RECT& workArea = inputs.workspace->workArea;
  auto floatLocally = [&](const char* reasonText) -> std::optional<RECT> {
    *reason = reasonText;
    return SharedGeometry::FitFloatingPlacementToLimits(*floating, limits);
  };
  auto noHint = [&](const char* reasonText) -> std::optional<RECT> {
    *reason = reasonText;
    *why = NoHint::Unmanaged;
    return std::nullopt;
  };
  // MultiWM will still tile it once arrangement plans with its limits.
  auto undecided = [&](const char* reasonText) -> std::optional<RECT> {
    *reason = reasonText;
    *why = NoHint::Undecided;
    return std::nullopt;
  };
  if (kind == TargetKind::Float) return SharedGeometry::FitFloatingPlacementToLimits(*floating, limits);
  const bool bspArrival = inputs.bspArrivals.count > 0 && (!inputs.tracked || inputs.intent == PlacementIntent::Arrival);
  if (bspArrival) {
    using CompanionAbi::BspArrivalChoice;
    BspArrivalChoice choice =
        CompanionAbi::ChooseBspArrivalTile(inputs.bspArrivals, workArea, limits.minWidth, limits.minHeight);
    if (choice.outcome == BspArrivalChoice::Outcome::Tile) {
      // A capped window is centred in its tile, as MultiWM's planner centres it.
      SharedGeometry::CenterWithinMaximumSize(choice.tile.left, choice.tile.right, limits.maxWidth);
      SharedGeometry::CenterWithinMaximumSize(choice.tile.top, choice.tile.bottom, limits.maxHeight);
      return choice.tile;
    }
    if (choice.outcome == BspArrivalChoice::Outcome::Unknown) return undecided("BSP tile depends on another arrival");
    return floating && !inputs.tracked ? floatLocally("no BSP tile fits: floats locally") : noHint("no BSP tile fits");
  }
  if (const auto fitted = FitSlotToLimits(target, workArea, limits, !inputs.bspLayout)) return fitted;
  if (inputs.bspLayout && FitsMinimum(workArea, limits)) return undecided("BSP tile needs a new layout plan");
  if (floating) return floatLocally("minimum larger than the work area: floats locally");
  return noHint("minimum larger than the work area; no floating hint");
}

// Calculates without placing. `proposed` is the app's intended window rectangle
// in physical screen pixels, including resize borders. DWM margins translate
// new windows; known members use the published measured borders and restore tile.
// Caller holds PlacementScope on the window's
// own thread, so application callbacks cannot recursively predict another show.
// With no placement, `*noHint` (optional) says whether MultiWM won't tile the
// window as asked or the snapshot can't say yet (NoHint).
static std::optional<PredictedPlacement> PredictWindowPlacement(
    HWND hwnd, const RECT& proposed, const char* path, NoHint* noHint = nullptr) {
  NoHint unused = NoHint::Unmanaged;
  NoHint& why = noHint ? *noHint : unused;
  why = NoHint::Unmanaged;
  PhysicalCoordinatesScope physical;
  RECT windowRect{};
  if (!GetWindowRect(hwnd, &windowRect)) return std::nullopt;
  const RECT currentFrame = FrameOf(hwnd, windowRect);
  RECT margins = SharedGeometry::FrameMargins(windowRect, currentFrame);
  const RECT frame = SharedGeometry::RemoveFrameMargins(proposed, margins);

  // The monitor MultiWM will assign: the one holding the frame (GetWindowPhysicalMonitor).
  HMONITOR monitor = MonitorFromRect(&frame, MONITOR_DEFAULTTONULL);
  if (!monitor) monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
  MONITORINFO monitorInfo{sizeof(monitorInfo)};
  if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo)) {
    LogDecision(hwnd, path, "off every monitor");
    return std::nullopt;
  }
  if (SharedGeometry::IsMonitorSized(proposed, monitorInfo.rcMonitor)) {
    LogDecision(hwnd, path, "fullscreen");
    return std::nullopt;
  }

  HintInputs inputs;
  if (!ReadHintInputs(hwnd, monitorInfo.rcMonitor, &inputs)) {
    LogDecision(hwnd, path, "no snapshot (MultiWM not running?)");  // Nothing will tile it: unmanaged.
    return std::nullopt;
  }
  // MultiWM already has this window; it places it, not the app's frame.
  if (inputs.tracked && IsWindowVisible(hwnd)) {
    why = NoHint::Undecided;
    return std::nullopt;
  }
  // Known hidden members use the same measured borders as minimized restores;
  // a hidden window's current DWM bounds can be missing or stale.
  if (inputs.tracked && inputs.restoreFrameMargins) margins = *inputs.restoreFrameMargins;
  TargetKind kind = TargetKind::Tile;
  std::optional<SharedGeometry::FloatingPlacement> floating;
  const char* reason = nullptr;
  const std::optional<RECT> target = PredictFrame(hwnd, frame, inputs, &kind, &floating, &reason, &why);
  if (!target) {
    LogDecision(hwnd, path, reason);
    return std::nullopt;
  }

  // MultiWM predicted the slot without knowing this window's size limits. Apply
  // them the way MultiWM will once it reads them.
  RECT frameTarget = *target;
  if (kind != TargetKind::KeepSize) {
    const UINT dpi = inputs.workspace->dpi ? inputs.workspace->dpi : 96;
    const auto limits = QueryFrameLimits(hwnd, dpi, margins);
    // The window changed while answering, possibly into one MultiWM won't tile.
    if (!limits) return std::nullopt;
    const auto fitted = FitTargetToLimits(*target, kind, floating, inputs, *limits, &reason, &why);
    if (!fitted) {
      LogDecision(hwnd, path, reason);
      return std::nullopt;
    }
    frameTarget = *fitted;
  }
  if (reason) LogDecision(hwnd, path, reason);
  // The snapshot holds visible frames; SetWindowPos takes the window rectangle,
  // which includes the invisible resize borders.
  const RECT placement = SharedGeometry::AddFrameMargins(frameTarget, margins);
  return PredictedPlacement{placement, kind == TargetKind::KeepSize};
}

// --- Settling ----------------------------------------------------------------------
//
// After the companion places a window at its show, the app may still move it
// before MultiWM gets to it: WeChat restores its pre-hide position 64 ms after
// showing, ApplicationFrameHost the app's last position just before uncloaking
// (traced). MultiWM would move it back, so the move is a visible jump. Until
// MultiWM takes the window over, the app's own moves are kept at the frame the
// companion chose (show_hooks.h, SteerSettlingWindow).
//
// "Taken over" is read from MultiWM itself, never guessed: its snapshot entry
// for the window changes once it has processed the show (WindowFacts), or its
// own placement arrives. One window per thread: the last one placed.
struct SettlingWindow {
  HWND hwnd = nullptr;
  RECT windowRect{};  // Physical screen pixels, including resize borders.
  WindowFacts facts;  // MultiWM's view of the window when it was placed.
  // The frame the app last asked for (physical): its creation or show frame, or
  // the move the companion rewrote, then each move the steering held. Where the
  // window goes when the hold ends because MultiWM won't tile it after all, so
  // no request the app made while held is lost. Empty when unknown.
  RECT appRect{};
};
static thread_local SettlingWindow g_settling;

static bool IsUwpFrame(HWND hwnd) {
  wchar_t className[32];
  return GetClassNameW(hwnd, className, ARRAYSIZE(className)) && wcscmp(className, L"ApplicationFrameWindow") == 0;
}

// Reads the title without sending the window a message.
static bool HasTitle(HWND hwnd) {
  wchar_t first[2];
  return InternalGetWindowText(hwnd, first, ARRAYSIZE(first)) > 0;
}

static void EndSettling() { g_settling = {}; }

// Starts settling `hwnd` at `windowRect`, replacing any earlier window on this
// thread. A spare UWP frame (no title yet) is not settled: no app uses it yet,
// and the launch that takes it places it again (show_hooks.h).
// `appRect` (physical) is the frame the app asked for (SettlingWindow::appRect).
static void BeginSettling(HWND hwnd, const RECT& windowRect, const RECT& appRect) {
  WindowFacts facts;
  if ((IsUwpFrame(hwnd) && !HasTitle(hwnd)) || !ReadWindowFacts(hwnd, &facts)) {
    EndSettling();
    return;
  }
  g_settling = {hwnd, windowRect, facts, appRect};
}

// The app's frame for a window about to be placed at `current`: while it is
// already settling, `current` is the companion's frame, so the app's last
// request is kept instead.
static RECT AppFrameBeforePlacing(HWND hwnd, const RECT& current) {
  return g_settling.hwnd == hwnd ? g_settling.appRect : current;
}

// Why MultiWM's discovery will now refuse an untracked settling window that
// passed at its show, or nullptr: the parts of WindowCanBeManaged and its
// enabled check that an app can change after showing. A framework window
// created resizable can drop its resize border by its show (PowerToys' WinUI
// overlays, traced). Tracked windows tolerate these, as in MultiWM.
static const char* SettlingIneligibility(HWND hwnd) {
  if (g_settling.facts.entry.hwnd) return nullptr;
  if (!(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_SIZEBOX)) return "fixed-size window";
  if (const char* reason = SharedRules::RejectNonTopLevel(hwnd)) return reason;
  if (const char* reason = SharedRules::RejectToolOrShellWindow(hwnd)) return reason;
  if (!IsWindowEnabled(hwnd)) return "disabled";
  return nullptr;
}

// Ends settling `hwnd` because it turned out ineligible after all (a later show
// found it so, or SettlingIneligibility). MultiWM won't tile it, so holding its
// moves would only fight the app. It goes to the frame the app last asked for,
// as if the companion had never acted. Callers hold PlacementScope and
// PhysicalCoordinatesScope.
static void ReleaseIneligibleSettling(HWND hwnd, const char* path) {
  if (!hwnd || g_settling.hwnd != hwnd) return;
  const RECT appRect = g_settling.appRect;
  EndSettling();
  if (IsRectEmpty(&appRect)) return;
  g_setWindowPosForOwnMoves(hwnd, nullptr, appRect.left, appRect.top, Width(appRect), Height(appRect),
                            SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
  LogDecision(hwnd, path, "released the predicted frame: moved to the app's own frame");
}

// Used by show and creation hooks that have no geometry arguments to rewrite.
static bool PlaceAtPredictedFrame(HWND hwnd, const char* path) {
  if (g_placing || !hwnd) return false;
  PlacementScope placing;
  PhysicalCoordinatesScope physical;
  RECT current{};
  if (RejectStructurally(hwnd) || !GetWindowRect(hwnd, &current)) {
    ReleaseIneligibleSettling(hwnd, path);
    return false;
  }
  NoHint noHint = NoHint::Unmanaged;
  const auto prediction = PredictWindowPlacement(hwnd, current, path, &noHint);
  if (!prediction) {
    // Undecided: MultiWM will still tile it, so it stays where the hold has it.
    if (noHint == NoHint::Unmanaged) ReleaseIneligibleSettling(hwnd, path);
    return false;
  }

  const RECT& placement = prediction->windowRect;
  const bool samePosition = placement.left == current.left && placement.top == current.top;
  const bool keepCurrentSize = prediction->keepSize ||
                               (Width(placement) == Width(current) && Height(placement) == Height(current));
  bool placed = true;
  if (samePosition && keepCurrentSize) {
    LogDecision(hwnd, path, "already at MultiWM's frame");
  } else {
    UINT flags = SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE;
    if (keepCurrentSize) flags |= SWP_NOSIZE;
    placed = g_setWindowPosForOwnMoves(hwnd, nullptr, placement.left, placement.top,
                                     Width(placement), Height(placement), flags) != FALSE;
    LogDecision(hwnd, path, placed ? "placed at MultiWM's frame" : "SetWindowPos failed");
  }
  // A window MultiWM only centres (keepSize) has no exact frame to hold.
  if (placed && !prediction->keepSize) BeginSettling(hwnd, placement, AppFrameBeforePlacing(hwnd, current));
  return placed;
}

}  // namespace Companion
// WHAM@5 end 6
// WHAM@5 begin 7: src/companion/restoration.h
// Restoring a minimized or maximized member straight into the tile MultiWM will
// give it, using the exact restore tile MultiWM publishes for each suspended member.
// Part of the companion; included after placement.h and before show_hooks.h.
// This chapter also owns show-command and native coordinate conversion.

namespace Companion {

static decltype(&SetWindowPlacement) g_unhookedSetWindowPlacement = nullptr;

// Show commands that bring up a restored window. Maximized and minimized shows
// are suspended by MultiWM rather than tiled, so they are left alone.
static int ResolveShowCommand(int command) {
  if (command == SW_SHOWDEFAULT) {
    STARTUPINFOW startup{sizeof(startup)};
    GetStartupInfoW(&startup);
    command = (startup.dwFlags & STARTF_USESHOWWINDOW) ? startup.wShowWindow : SW_SHOWNORMAL;
  }
  return command;
}

static bool IsRestoredShowCommand(int command) {
  command = ResolveShowCommand(command);
  return command == SW_SHOWNORMAL || command == SW_SHOW || command == SW_SHOWNA ||
         command == SW_SHOWNOACTIVATE || command == SW_RESTORE;
}

// Geometry arguments use the caller's DPI coordinates. The conversion APIs use
// the window's context, so skip unusual mixed-context calls rather than guess.
// Per-monitor callers already use physical pixels, even for an unaware window.
static std::optional<RECT> ConvertScreenRect(
    HWND hwnd, const RECT& rect, BOOL(WINAPI* convert)(HWND, LPPOINT)) {
  const auto context = GetThreadDpiAwarenessContext();
  if (GetAwarenessFromDpiAwarenessContext(context) == DPI_AWARENESS_PER_MONITOR_AWARE) return rect;
  if (!AreDpiAwarenessContextsEqual(context, GetWindowDpiAwarenessContext(hwnd))) return std::nullopt;
  POINT topLeft{rect.left, rect.top};
  POINT bottomRight{rect.right, rect.bottom};
  if (!convert(hwnd, &topLeft) || !convert(hwnd, &bottomRight)) return std::nullopt;
  return RECT{topLeft.x, topLeft.y, bottomRight.x, bottomRight.y};
}

// WINDOWPLACEMENT keeps rcNormalPosition in workspace coordinates (relative to
// the work area) for non-tool top-level windows. These convert to and from
// screen coordinates on the monitor `info` describes.
static RECT WorkspaceToScreen(RECT rect, const MONITORINFO& info) {
  OffsetRect(&rect, info.rcWork.left - info.rcMonitor.left, info.rcWork.top - info.rcMonitor.top);
  return rect;
}

static RECT ScreenToWorkspace(RECT rect, const MONITORINFO& info) {
  OffsetRect(&rect, info.rcMonitor.left - info.rcWork.left, info.rcMonitor.top - info.rcWork.top);
  return rect;
}

// Points the window's restore at the tile MultiWM will give it. MultiWM
// publishes that tile for each suspended member by running its own Restore on a
// copy of the workspace, so the hint is exact, not a guess. Caller holds
// PlacementScope. Only the copy's normal rectangle is changed.
static bool AdjustRestorePlacement(HWND hwnd, WINDOWPLACEMENT* placement, const char* path) {
  const int command = ResolveShowCommand(static_cast<int>(placement->showCmd));
  if (command != SW_RESTORE && command != SW_SHOWNORMAL && command != SW_SHOWNOACTIVATE) return false;
  if ((!IsIconic(hwnd) && !IsZoomed(hwnd)) || RejectStructurally(hwnd, true) ||
      (IsIconic(hwnd) && (placement->flags & WPF_RESTORETOMAXIMIZED))) return false;

  MONITORINFO monitorInfo{sizeof(monitorInfo)};
  RECT monitorRect{};
  {
    PhysicalCoordinatesScope physical;
    if (!GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitorInfo)) return false;
    monitorRect = monitorInfo.rcMonitor;
  }
  HintInputs inputs;
  if (!ReadHintInputs(hwnd, monitorRect, &inputs) || !inputs.tracked) return false;
  TargetKind kind;
  std::optional<SharedGeometry::FloatingPlacement> floating;
  const char* reason = nullptr;
  // Known transitions ignore the proposed rectangle. Both hidden-normal shows
  // and min/max restores select the same model-owned restore or arrival intent.
  const auto frame = PredictFrame(hwnd, placement->rcNormalPosition, inputs, &kind, &floating, &reason);
  if (!frame) {
    LogDecision(hwnd, path, reason);
    return false;
  }
  const auto& workspace = *inputs.workspace;
  RECT target = *frame;
  // The tile is a visible frame; the restored window rectangle adds the
  // invisible resize borders. A minimized or maximized window can't report them
  // (see RememberFrameMargins in MultiWM), and system metrics overestimate them by
  // a few pixels, within MultiWM's placement tolerance, so MultiWM would never
  // correct the result. Use MultiWM's measurement from the restored window, or
  // give no hint.
  if (!inputs.restoreFrameMargins || inputs.restoreMarginsDpi != workspace.dpi) return false;
  const RECT& margins = *inputs.restoreFrameMargins;
  {
    // MultiWM planned the tile with the window's cached limits. Apply the live
    // ones the same way (FitSlotToLimits), which changes nothing unless they
    // changed since, then add the borders to get the rectangle Windows restores to.
    PhysicalCoordinatesScope physical;
    const auto limits = QueryFrameLimits(hwnd, workspace.dpi ? workspace.dpi : 96, margins);
    if (!limits) return false;
    const auto fitted = FitSlotToLimits(target, workspace.workArea, *limits, !inputs.bspLayout);
    if (!fitted) return false;
    target = *fitted;
  }
  target = SharedGeometry::AddFrameMargins(target, margins);
  const auto logicalTarget = ConvertScreenRect(hwnd, target, PhysicalToLogicalPointForPerMonitorDPI);
  if (!logicalTarget || !GetMonitorInfoW(MonitorFromRect(&*logicalTarget, MONITOR_DEFAULTTONEAREST), &monitorInfo)) {
    return false;
  }
  placement->rcNormalPosition = ScreenToWorkspace(*logicalTarget, monitorInfo);
  LogDecision(hwnd, path, reason);
  return true;
}

// The SetWindowPlacement show command that updates only the normal rectangle:
// same visibility, same minimized/maximized state, no activation. Measured on a
// private desktop: SW_SHOWMAXIMIZED and SW_SHOWMINNOACTIVE both show a hidden
// window (SW_SHOWMAXIMIZED also activates it); SW_SHOWNA shows a hidden window;
// SW_HIDE keeps a hidden window hidden and keeps its state.
static UINT StatePreservingShowCommand(HWND hwnd) {
  if (!IsWindowVisible(hwnd)) return SW_HIDE;
  return IsIconic(hwnd) ? SW_SHOWMINNOACTIVE : SW_SHOWNA;
}

// Seed the rectangle Windows will restore, preserving the current min/max state.
// Never move the iconic/maximized rectangle itself, show the window early or
// activate it: the caller's own show call decides both.
static bool PrepareRestorePlacement(HWND hwnd, int command, const char* path) {
  if (g_placing || !hwnd || (!IsIconic(hwnd) && !IsZoomed(hwnd)) || RejectStructurally(hwnd, true)) return false;
  WINDOWPLACEMENT placement{sizeof(placement)};
  if (!GetWindowPlacement(hwnd, &placement)) return false;
  placement.showCmd = static_cast<UINT>(command);
  PlacementScope placing;
  if (!AdjustRestorePlacement(hwnd, &placement, path)) return false;
  placement.showCmd = StatePreservingShowCommand(hwnd);
  return g_unhookedSetWindowPlacement && g_unhookedSetWindowPlacement(hwnd, &placement);
}

}  // namespace Companion
// WHAM@5 end 7
// WHAM@5 begin 8: src/companion/show_hooks.h
// Hooks on the ways a top-level window becomes visible.
// Part of the MultiWM Companion mod; see the include order in src/companion/multiwm-companion.cpp.
//
// The same show paths Window Centralizer and Mobile Open Animation cover: the
// win32u entry points (user32 and the dialog manager call them directly, so
// hooking ShowWindow alone misses most dialogs), deferred positioning,
// SetWindowPlacement (shows inside the kernel) and visible creation.

namespace Companion {

using NtUserShowWindow_t = BOOL(WINAPI*)(HWND, int);
static NtUserShowWindow_t g_unhookedShowWindow = nullptr;
static NtUserShowWindow_t g_unhookedShowWindowAsync = nullptr;
static decltype(&DeferWindowPos) g_unhookedDeferWindowPos = nullptr;
static decltype(&CreateWindowExW) g_unhookedCreateWindowExW = nullptr;
static decltype(&CreateWindowExA) g_unhookedCreateWindowExA = nullptr;
static decltype(&DefWindowProcW) g_unhookedDefWindowProcW = nullptr;
static decltype(&DefWindowProcA) g_unhookedDefWindowProcA = nullptr;

// ApplicationFrameHost keeps a spare UWP frame: it creates and shows the next
// frame, still cloaked, whenever an app takes the current one, so that show is
// often minutes before any app uses it. A launch then reuses the spare: it sets
// the title, sometimes moves the frame to where the app last closed, and
// uncloaks it, with no show call. (Traced on Microsoft Store: the jump at launch
// was the spare's stale position or the app's saved one.) Until it uncloaks,
// such a frame counts as not shown yet. MultiWM ignores cloaked windows, so an
// untracked one is still a new window to it.
//
// The launch is recognised by the spare's title going from empty to set. Other
// cloaked frames (an app on another virtual desktop, say) already have a title,
// so their title changes are left alone. ApplicationFrameHost's follow-up move
// to the app's saved position is held by settling (placement.h).
static bool IsCloakedUwpFrame(HWND hwnd) {
  if (!IsWindowVisible(hwnd) || !IsUwpFrame(hwnd)) return false;
  DWORD cloaked = 0;
  return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked != 0;
}

// Calls that need a hint: a hidden window's show.
static bool NeedsPositionHint(HWND hwnd, UINT flags) {
  return (flags & SWP_SHOWWINDOW) && !IsWindowVisible(hwnd);
}

// True for a WM_SETTEXT that may be an app taking a spare UWP frame. Read
// before the title changes.
static bool MayAttachUwpFrame(HWND hwnd, UINT message) {
  return message == WM_SETTEXT && !g_placing && IsCloakedUwpFrame(hwnd) && !HasTitle(hwnd);
}

// After the title is set (so title rules match): place the frame at the current
// prediction, which also starts settling it.
static void PlaceAttachingUwpFrame(HWND hwnd) {
  if (HasTitle(hwnd) && IsCloakedUwpFrame(hwnd)) PlaceAtPredictedFrame(hwnd, "UWP frame launch");
}

// Windows sets this WINDOWPOS flag (undocumented, seen as 0x8124 in traces) for
// a minimize, maximize or restore.
constexpr UINT kSwpStateChanged = 0x8000;

// Resolve the app's requested geometry before any hint rewrites WINDOWPOS. A
// move or size alone keeps the other half from `base` (physical) when given,
// else from the window's current frame.
static std::optional<RECT> ProposedPhysicalWindowRect(const WINDOWPOS& position, const RECT* base = nullptr) {
  RECT current{};
  if (position.flags & (SWP_NOMOVE | SWP_NOSIZE)) {
    if (base) {
      const auto logical = ConvertScreenRect(position.hwnd, *base, PhysicalToLogicalPointForPerMonitorDPI);
      if (!logical) return std::nullopt;
      current = *logical;
    } else if (!GetWindowRect(position.hwnd, &current)) {
      return std::nullopt;
    }
  }
  const LONG left = (position.flags & SWP_NOMOVE) ? current.left : position.x;
  const LONG top = (position.flags & SWP_NOMOVE) ? current.top : position.y;
  const LONG width = (position.flags & SWP_NOSIZE) ? Width(current) : position.cx;
  const LONG height = (position.flags & SWP_NOSIZE) ? Height(current) : position.cy;
  return ConvertScreenRect(position.hwnd, {left, top, left + width, top + height},
                           LogicalToPhysicalPointForPerMonitorDPI);
}

// Makes `position` a move and size to `frame` (physical). Unchanged when the
// frame can't be converted.
static void SetWindowPosFrame(WINDOWPOS& position, const RECT& frame) {
  const auto logical = ConvertScreenRect(position.hwnd, frame, PhysicalToLogicalPointForPerMonitorDPI);
  if (!logical) return;
  position.x = logical->left;
  position.y = logical->top;
  position.cx = Width(*logical);
  position.cy = Height(*logical);
  position.flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
}

// Why the hold on a settling window ends at the app's `requested` frame
// (physical), or nullptr: the window turned ineligible, or the request is its
// monitor's size (fullscreen without maximizing, as MultiWM reads it).
static const char* SettlingReleaseForRequest(HWND hwnd, const std::optional<RECT>& requested) {
  if (const char* reason = SettlingIneligibility(hwnd)) return reason;
  if (!requested) return nullptr;
  PhysicalCoordinatesScope physical;
  MONITORINFO monitorInfo{sizeof(monitorInfo)};
  const HMONITOR monitor = MonitorFromRect(&*requested, MONITOR_DEFAULTTONEAREST);
  const bool fullscreen = monitor && GetMonitorInfoW(monitor, &monitorInfo) &&
                          SharedGeometry::IsMonitorSized(*requested, monitorInfo.rcMonitor);
  return fullscreen ? "fullscreen" : nullptr;
}

// A style change alone moves nothing, so a settling window that turned
// ineligible would sit in a tile MultiWM won't keep: release it now.
static void ReleaseSettlingAfterStyleChange(HWND hwnd) {
  const char* reason = SettlingIneligibility(hwnd);
  if (!reason) return;
  PlacementScope placing;
  PhysicalCoordinatesScope physical;
  LogDecision(hwnd, "WM_STYLECHANGED", reason);
  ReleaseIneligibleSettling(hwnd, "WM_STYLECHANGED");
}

// Holds a settling window (placement.h) at the companion's frame. Every
// positioning call (SetWindowPos, MoveWindow, SetWindowPlacement, the end of a
// DeferWindowPos batch) sends WM_WINDOWPOSCHANGING, and changing its WINDOWPOS
// changes the move (documented), so this one place covers them all. Runs before
// DefWindowProc, after the window's own procedure. Settling ends:
//  - when MultiWM has taken the window: its snapshot entry changed, or its own
//    placement arrived (WM_WINDOWPOSCHANGED from another thread; MultiWM uses
//    SWP_NOSENDCHANGING, so its moves never reach the steering);
//  - when anything but the app's own thread positions it, the user moves or
//    resizes it, or it is minimized, maximized, hidden or destroyed;
//  - when an untracked window turns ineligible (SettlingIneligibility), or the
//    app asks for fullscreen (its whole monitor, maximized or not). The window
//    then gets the frame the app last asked for, which the hold had replaced.
// From then on MultiWM handles the app's moves as before. Each request is read
// against the app's last one, not the held frame: an app that sizes a window to
// its monitor in two steps (move, then size) asks for the whole monitor.
static void SteerSettlingWindow(HWND hwnd, UINT message, LPARAM lparam) {
  if (!hwnd || hwnd != g_settling.hwnd || g_placing) return;
  if (message == WM_NCDESTROY) {
    EndSettling();
    return;
  }
  if (message == WM_STYLECHANGED) {
    ReleaseSettlingAfterStyleChange(hwnd);
    return;
  }
  if (message != WM_WINDOWPOSCHANGING && message != WM_WINDOWPOSCHANGED) return;
  auto* position = reinterpret_cast<WINDOWPOS*>(lparam);
  if (!position || position->hwnd != hwnd) return;
  const bool fromOtherThread = InSendMessageEx(nullptr) != ISMEX_NOSEND;
  if (message == WM_WINDOWPOSCHANGED) {
    if (fromOtherThread || (position->flags & SWP_NOSENDCHANGING)) EndSettling();
    return;
  }
  if ((position->flags & (SWP_NOMOVE | SWP_NOSIZE)) == (SWP_NOMOVE | SWP_NOSIZE) &&
      !(position->flags & SWP_HIDEWINDOW)) {
    return;  // Z-order or activation only.
  }
  GUITHREADINFO gui{sizeof(gui)};
  WindowFacts facts;
  if (fromOtherThread || (position->flags & (SWP_HIDEWINDOW | kSwpStateChanged)) || IsIconic(hwnd) ||
      IsZoomed(hwnd) || (GetGUIThreadInfo(GetCurrentThreadId(), &gui) && (gui.flags & GUI_INMOVESIZE)) ||
      !ReadWindowFacts(hwnd, &facts) || !(facts == g_settling.facts)) {
    EndSettling();
    return;
  }
  const RECT appRect = g_settling.appRect;
  const auto requested = ProposedPhysicalWindowRect(*position, IsRectEmpty(&appRect) ? nullptr : &appRect);
  if (const char* release = SettlingReleaseForRequest(hwnd, requested)) {
    EndSettling();
    // This move becomes the app's whole request, including what the hold kept.
    if (requested) SetWindowPosFrame(*position, *requested);
    LogDecision(hwnd, "WM_WINDOWPOSCHANGING", release);
    LogDecision(hwnd, "WM_WINDOWPOSCHANGING", "released the predicted frame: moved to the app's own frame");
    return;
  }
  const auto target = ConvertScreenRect(hwnd, g_settling.windowRect, PhysicalToLogicalPointForPerMonitorDPI);
  if (!target) {
    EndSettling();
    return;
  }
  if (requested) g_settling.appRect = *requested;
  position->x = target->left;
  position->y = target->top;
  position->cx = Width(*target);
  position->cy = Height(*target);
  position->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
  LogDecision(hwnd, "WM_WINDOWPOSCHANGING", "held at the companion's frame until MultiWM takes the window");
}

// ShowWindow and ShowWindowAsync: a restore of a minimized or maximized member
// is prepared first; a hidden window being shown restored is placed while still
// hidden. `show` is the original entry point, read at call time because Windhawk
// fills it in after the hooks are applied. `path` names the call in logs.
// Structural eligibility rejects cross-thread windows (an async show from another
// thread) before any native probe.
static BOOL ShowWithHint(NtUserShowWindow_t show, HWND hwnd, int command, const char* path,
                         const char* restorePath) {
  if (PrepareRestorePlacement(hwnd, command, restorePath)) {
    PlacementScope placing;
    return show(hwnd, command);
  }
  if (hwnd && IsRestoredShowCommand(command) && !IsWindowVisible(hwnd)) PlaceAtPredictedFrame(hwnd, path);
  return show(hwnd, command);
}

static BOOL WINAPI ShowWindowHook(HWND hwnd, int command) {
  return ShowWithHint(g_unhookedShowWindow, hwnd, command, "ShowWindow", "ShowWindow restore");
}

static BOOL WINAPI ShowWindowAsyncHook(HWND hwnd, int command) {
  return ShowWithHint(g_unhookedShowWindowAsync, hwnd, command, "ShowWindowAsync", "ShowWindowAsync restore");
}

// DefWindowProcW and DefWindowProcA: default SC_RESTORE handling restores a
// member into its tile; every message passes through the settling and UWP-frame
// checks. `defWindowProc` is the original, read at call time.
static LRESULT DefWindowProcWithHint(
    decltype(&DefWindowProcW) defWindowProc, HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  if (message == WM_SYSCOMMAND && (wparam & 0xFFF0) == SC_RESTORE &&
      PrepareRestorePlacement(hwnd, SW_RESTORE, "SC_RESTORE")) {
    PlacementScope placing;
    return defWindowProc(hwnd, message, wparam, lparam);
  }
  SteerSettlingWindow(hwnd, message, lparam);
  const bool attaching = MayAttachUwpFrame(hwnd, message);
  const LRESULT result = defWindowProc(hwnd, message, wparam, lparam);
  if (attaching) PlaceAttachingUwpFrame(hwnd);
  return result;
}

static LRESULT WINAPI DefWindowProcWHook(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  return DefWindowProcWithHint(g_unhookedDefWindowProcW, hwnd, message, wparam, lparam);
}

static LRESULT WINAPI DefWindowProcAHook(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  return DefWindowProcWithHint(g_unhookedDefWindowProcA, hwnd, message, wparam, lparam);
}

// Both immediate and deferred placement take the same geometry. Rewrite it
// without committing a move; a deferred batch must remain deferred. Callers
// hold PlacementScope through the original API call to prevent nested hints.
// Returns the frame to settle at (physical) when the hint set an exact frame,
// with the app's request it replaced in `*appRect`.
static std::optional<RECT> AdjustPositionForHint(WINDOWPOS& position, const char* path, RECT* appRect) {
  const HWND hwnd = position.hwnd;
  // A settling window's frame is the companion's; its app's is the last request.
  const bool settling = hwnd == g_settling.hwnd && !IsRectEmpty(&g_settling.appRect);
  const RECT settlingApp = g_settling.appRect;
  if (const auto proposed = ProposedPhysicalWindowRect(position, settling ? &settlingApp : nullptr)) {
    *appRect = *proposed;
    NoHint noHint = NoHint::Unmanaged;
    const auto prediction = PredictWindowPlacement(hwnd, *proposed, path, &noHint);
    const auto target = prediction ? ConvertScreenRect(hwnd, prediction->windowRect,
                                                       PhysicalToLogicalPointForPerMonitorDPI) : std::nullopt;
    if (target) {
      position.x = target->left;
      position.y = target->top;
      position.flags &= ~SWP_NOMOVE;
      if (!prediction->keepSize) {
        position.cx = Width(*target);
        position.cy = Height(*target);
        position.flags &= ~SWP_NOSIZE;
      }
      LogDecision(hwnd, path, "using MultiWM's predicted placement");
      if (!prediction->keepSize) return prediction->windowRect;
    }
    // A settling window this show finds ineligible gets the app's whole request,
    // including what the hold kept from it. One the snapshot can't place yet
    // stays held, with the request remembered for a later release.
    if (!prediction && hwnd == g_settling.hwnd) {
      if (noHint == NoHint::Undecided) {
        g_settling.appRect = *proposed;
        SetWindowPosFrame(position, g_settling.windowRect);
        LogDecision(hwnd, path, "no hint yet: kept at the held frame");
      } else {
        EndSettling();
        SetWindowPosFrame(position, *proposed);
      }
    }
  }
  return std::nullopt;
}

static BOOL WINAPI SetWindowPosHook(HWND hwnd, HWND after, int x, int y, int cx, int cy, UINT flags) {
  if (g_placing || !hwnd || !NeedsPositionHint(hwnd, flags) || RejectStructurally(hwnd)) {
    return g_unhookedSetWindowPos(hwnd, after, x, y, cx, cy, flags);
  }
  BOOL result = FALSE;
  std::optional<RECT> settleAt;
  RECT appRect{};
  {
    PlacementScope placing;
    WINDOWPOS position{hwnd, after, x, y, cx, cy, flags};
    settleAt = AdjustPositionForHint(position, "SetWindowPos", &appRect);
    // Preserve the app's single native call, flags and result, including failure.
    result = g_unhookedSetWindowPos(hwnd, after, position.x, position.y, position.cx, position.cy, position.flags);
  }
  if (result && settleAt) BeginSettling(hwnd, *settleAt, appRect);
  return result;
}

static HDWP WINAPI DeferWindowPosHook(
    HDWP batch, HWND hwnd, HWND after, int x, int y, int cx, int cy, UINT flags) {
  if (g_placing || !batch || !hwnd || !NeedsPositionHint(hwnd, flags) || RejectStructurally(hwnd)) {
    return g_unhookedDeferWindowPos(batch, hwnd, after, x, y, cx, cy, flags);
  }
  PlacementScope placing;
  WINDOWPOS position{hwnd, after, x, y, cx, cy, flags};
  RECT appRect{};
  AdjustPositionForHint(position, "DeferWindowPos", &appRect);
  return g_unhookedDeferWindowPos(batch, hwnd, after,
                                 position.x, position.y, position.cx, position.cy, position.flags);
}

// Programs that restore a saved position (7-Zip's file manager, for one) show
// their window this way, with no show call to hook.
static BOOL WINAPI SetWindowPlacementHook(HWND hwnd, const WINDOWPLACEMENT* placement) {
  if (!g_placing && hwnd && placement && placement->length == sizeof(*placement) &&
      (IsIconic(hwnd) || IsZoomed(hwnd))) {
    PlacementScope placing;
    WINDOWPLACEMENT adjusted = *placement;
    if (AdjustRestorePlacement(hwnd, &adjusted, "SetWindowPlacement restore")) {
      return g_unhookedSetWindowPlacement(hwnd, &adjusted);
    }
    return g_unhookedSetWindowPlacement(hwnd, placement);
  }
  if (g_placing || !hwnd || !placement || placement->length != sizeof(*placement) ||
      !IsRestoredShowCommand(static_cast<int>(placement->showCmd)) ||
      IsWindowVisible(hwnd) || RejectStructurally(hwnd)) {
    return g_unhookedSetWindowPlacement(hwnd, placement);
  }
  std::optional<RECT> settleAt;
  RECT appRect{};
  BOOL result = FALSE;
  {
    PlacementScope placing;
    // Non-tool top-level windows store rcNormalPosition in workspace coordinates.
    // Convert to screen coordinates before prediction and back at its destination.
    MONITORINFO monitorInfo{sizeof(monitorInfo)};
    std::optional<WINDOWPLACEMENT> adjusted;
    if (GetMonitorInfoW(MonitorFromRect(&placement->rcNormalPosition, MONITOR_DEFAULTTONEAREST), &monitorInfo)) {
      const RECT proposedScreen = WorkspaceToScreen(placement->rcNormalPosition, monitorInfo);
      const auto proposed = ConvertScreenRect(hwnd, proposedScreen, LogicalToPhysicalPointForPerMonitorDPI);
      const auto prediction = proposed ? PredictWindowPlacement(hwnd, *proposed, "SetWindowPlacement") : std::nullopt;
      const auto target = prediction ? ConvertScreenRect(hwnd, prediction->windowRect,
                                                         PhysicalToLogicalPointForPerMonitorDPI) : std::nullopt;
      if (target && GetMonitorInfoW(MonitorFromRect(&*target, MONITOR_DEFAULTTONEAREST), &monitorInfo)) {
        adjusted = *placement;
        adjusted->rcNormalPosition = ScreenToWorkspace(*target, monitorInfo);
        LogDecision(hwnd, "SetWindowPlacement", "using MultiWM's predicted placement");
        if (!prediction->keepSize) {
          settleAt = prediction->windowRect;
          appRect = *proposed;
        }
      }
    }
    result = g_unhookedSetWindowPlacement(hwnd, adjusted ? &*adjusted : placement);
  }
  if (result && settleAt) BeginSettling(hwnd, *settleAt, appRect);
  return result;
}

// Visible creation can show inside the kernel. Adjust as soon as creation
// returns; apps that paint during creation may already have exposed a frame.
static void PlaceIfCreatedVisible(HWND hwnd, DWORD style) {
  if (hwnd && (style & WS_VISIBLE) && !(style & WS_CHILD)) PlaceAtPredictedFrame(hwnd, "CreateWindowEx");
}

static HWND WINAPI CreateWindowExWHook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName, DWORD style, int x,
                                       int y, int width, int height, HWND parent, HMENU menu, HINSTANCE instance,
                                       LPVOID param) {
  const HWND hwnd = g_unhookedCreateWindowExW(exStyle, className, windowName, style, x, y, width, height, parent,
                                              menu, instance, param);
  PlaceIfCreatedVisible(hwnd, style);
  return hwnd;
}

static HWND WINAPI CreateWindowExAHook(DWORD exStyle, LPCSTR className, LPCSTR windowName, DWORD style, int x,
                                       int y, int width, int height, HWND parent, HMENU menu, HINSTANCE instance,
                                       LPVOID param) {
  const HWND hwnd = g_unhookedCreateWindowExA(exStyle, className, windowName, style, x, y, width, height, parent,
                                              menu, instance, param);
  PlaceIfCreatedVisible(hwnd, style);
  return hwnd;
}

// Registers every hook. Windhawk applies them after Wh_ModInit returns, and
// fills in the g_unhooked* pointers then. Returns false if a hook failed to
// register; the ones that registered still work.
static bool InstallShowHooks() {
  bool ok = true;
  if (HMODULE win32u = GetModuleHandleW(L"win32u.dll")) {
    if (auto showWindow = reinterpret_cast<NtUserShowWindow_t>(GetProcAddress(win32u, "NtUserShowWindow"))) {
      ok &= WindhawkUtils::SetFunctionHook(showWindow, ShowWindowHook, &g_unhookedShowWindow);
    }
    if (auto showWindowAsync = reinterpret_cast<NtUserShowWindow_t>(GetProcAddress(win32u, "NtUserShowWindowAsync"))) {
      ok &= WindhawkUtils::SetFunctionHook(showWindowAsync, ShowWindowAsyncHook, &g_unhookedShowWindowAsync);
    }
    if (auto setWindowPos = reinterpret_cast<SetWindowPos_t>(GetProcAddress(win32u, "NtUserSetWindowPos"))) {
      ok &= WindhawkUtils::SetFunctionHook(setWindowPos, SetWindowPosHook, &g_unhookedSetWindowPos);
    }
  }
  ok &= WindhawkUtils::SetFunctionHook(DeferWindowPos, DeferWindowPosHook, &g_unhookedDeferWindowPos);
  ok &= WindhawkUtils::SetFunctionHook(DefWindowProcW, DefWindowProcWHook, &g_unhookedDefWindowProcW);
  ok &= WindhawkUtils::SetFunctionHook(DefWindowProcA, DefWindowProcAHook, &g_unhookedDefWindowProcA);
  ok &= WindhawkUtils::SetFunctionHook(SetWindowPlacement, SetWindowPlacementHook, &g_unhookedSetWindowPlacement);
  ok &= WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExWHook, &g_unhookedCreateWindowExW);
  ok &= WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateWindowExAHook, &g_unhookedCreateWindowExA);
  return ok;
}

}  // namespace Companion
// WHAM@5 end 8

BOOL Wh_ModInit() {
  if (!Companion::InstallShowHooks()) Wh_Log(L"Some show hooks could not be installed");
  return TRUE;
}

void Wh_ModUninit() {
  Companion::CloseSnapshotReader();
}
// #region WHAM recovery
// clang-format off
// WHAM@5 recovery-begin
// {
//   "type": "manifest",
//   "version": 5,
//   "root": "src/companion/multiwm-companion.cpp",
//   "files": [
//     {
//       "path": "src/companion/multiwm-companion.cpp",
//       "hash": "1ca5005a072283ecef9e485ef9b5484c04fa138842bd7785070b5681e12fde56",
//       "bom": false
//     },
//     {
//       "path": "src/companion/placement.h",
//       "hash": "2aefbb181e7d08ed63e09ce6909edda43cf8e527d8785531e896ab0a23bde730",
//       "bom": false
//     },
//     {
//       "path": "src/companion/restoration.h",
//       "hash": "de29b58ad1b5eed6b0a5d4b4d5ca87be1f131c0e508449b01a5ef2b5b4f0504a",
//       "bom": false
//     },
//     {
//       "path": "src/companion/show_hooks.h",
//       "hash": "31007dcffbc3092b6c50e346e53a20c5f4a1e17c2d6818bd132792c92de73c12",
//       "bom": false
//     },
//     {
//       "path": "src/companion/snapshot_reader.h",
//       "hash": "976606d99f4c033ff056da6fbd41c3d9c9bffc888f48470c19891f3b59796589",
//       "bom": false
//     },
//     {
//       "path": "src/shared/companion_snapshot.h",
//       "hash": "40136d39bb7f7f9536e938548226f28d8333f7b132a8ffa3d56bb47cd7ae9e72",
//       "bom": false
//     },
//     {
//       "path": "src/shared/geometry.h",
//       "hash": "0045607a098b8d01ac3ceb56f85f8385ec581ea9c44cfd439c3488c35db93587",
//       "bom": false
//     },
//     {
//       "path": "src/shared/split_geometry.h",
//       "hash": "8aabf99308e2765cc9893dafa327a61626e23e71939784c9a2af6a6165c00145",
//       "bom": false
//     },
//     {
//       "path": "src/shared/window_rules.h",
//       "hash": "c0db59769a1815fa2c0fa5909ab09fe4f4f563461bcf2a437e62894535ae4d48",
//       "bom": false
//     }
//   ],
//   "config": {
//     "formatVersion": 1,
//     "sourceDirectory": "src",
//     "outputDirectory": "output",
//     "entries": [
//       "companion/multiwm-companion.cpp"
//     ],
//     "exclude": [],
//     "includeDirectories": [
//       "src"
//     ],
//     "externalIncludes": [],
//     "inlineAngleIncludes": false,
//     "generate": "onSave",
//     "debounceMs": 250
//   },
//   "tail": "",
//   "records": [
//     {
//       "type": "include",
//       "id": 1,
//       "path": "src/shared/window_rules.h",
//       "directive": "#include \"../shared/window_rules.h\"\r\n"
//     },
//     {
//       "type": "include",
//       "id": 2,
//       "path": "src/shared/split_geometry.h",
//       "directive": "#include \"../shared/split_geometry.h\"\r\n"
//     },
//     {
//       "type": "include",
//       "id": 3,
//       "path": "src/shared/companion_snapshot.h",
//       "directive": "#include \"../shared/companion_snapshot.h\"\r\n"
//     },
//     {
//       "type": "include",
//       "id": 4,
//       "path": "src/shared/geometry.h",
//       "directive": "#include \"../shared/geometry.h\"\r\n"
//     },
//     {
//       "type": "include",
//       "id": 5,
//       "path": "src/companion/snapshot_reader.h",
//       "directive": "#include \"snapshot_reader.h\"\r\n"
//     },
//     {
//       "type": "include",
//       "id": 6,
//       "path": "src/companion/placement.h",
//       "directive": "#include \"placement.h\"\r\n"
//     },
//     {
//       "type": "include",
//       "id": 7,
//       "path": "src/companion/restoration.h",
//       "directive": "#include \"restoration.h\"\r\n"
//     },
//     {
//       "type": "include",
//       "id": 8,
//       "path": "src/companion/show_hooks.h",
//       "directive": "#include \"show_hooks.h\"\r\n"
//     }
//   ]
// }
// WHAM@5 recovery-end
// clang-format on
// #endregion
