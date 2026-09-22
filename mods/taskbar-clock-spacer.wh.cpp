// ==WindhawkMod==
// @id              taskbar-clock-spacer
// @name            Taskbar Clock Spacer
// @description     Companion for Taskbar Clock Customization: a %s% token for explicit, weightable gaps between clock items, where its built-in Justified alignment stretches every space. Windows 11 only.
// @version         1.1
// @author          sb4ssman
// @github          https://github.com/sb4ssman
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Clock Spacer

Adds a `%s%` elastic spacer token to the Windows 11 taskbar clock, so clock items
can be pushed apart to fill a fixed width instead of bunching together.

![Clock Spacer distributing a custom clock across multiple rows](https://raw.githubusercontent.com/sb4ssman/Windhawk-Mod-Lab/main/taskbar-clock-spacer/assets/clock-spacer-working.png)
*User-confirmed working configuration, September 9, 2026, with system stats,
time, date, and weather arranged across a fixed-width clock.*

## Two requirements — please read before installing

**1. This mod does nothing on its own.** It is a companion for
[Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization).
That mod produces the clock text; this mod only rearranges it. Install and
configure that mod first.

**2. The clock needs a fixed width.** An elastic spacer distributes *leftover*
width. If the clock sizes itself to its own text there is no leftover width,
every gap computes to zero, and the result looks exactly as if the mod were not
installed. Set a fixed width using either:

- **Max width** in Taskbar Clock Customization's settings, or
- **Max clock width** in this mod's settings.

Either one works. 120 px is a reasonable starting point.

Windows 11 only. This mod does not work on Windows 10.

## Try the built-in option first

Taskbar Clock Customization 1.8 and later can spread a line across a fixed
width on its own: set its **Text alignment** to **Justified**, use ordinary
spaces where you want the gaps, and use a non-breaking space inside items you
want kept together. If that looks right for your clock, you don't need this mod.

What `%s%` does that **Justified** does not:

- **Gaps only where you put them.** Justified stretches every space in the
  line, and can widen the spacing between characters — its author noted large
  gaps opening inside `MB/s`. With `%s%`, ordinary spaces and the characters
  inside each item are left exactly as they are.
- **Weighted gaps.** `%s%%s%` takes twice the share of a single `%s%`.
- **A minimum gap.** **Minimum spacer width** keeps every gap visible even when
  the text nearly fills the clock.
- **Gaps inside the weather.** `{spacer}` works inside the Weather format, where
  the weather text arrives as one pre-formatted string.

## What it does

Put `%s%` between items in the clock's Top Line or Bottom Line format. Each `%s%`
becomes a gap, and all leftover width is shared out evenly between the gaps.

| Format | Result |
| --- | --- |
| `%time%%s%%date%` | time hugs the left edge, date hugs the right, gap fills the middle |
| `%time%%s%%date%%s%%weekday%` | three items, two equal gaps |
| `%time%%s%%date%%s%%s%%weekday%` | Double-spacer: more space is weighted between date and weekday |

The first item always hugs the left edge and the last always hugs the right edge,
so the line stays anchored as the text changes width.

### Spacers inside the weather

The weather service substitutes `%s` as its sunset token, so `%s%` cannot be
written inside Taskbar Clock Customization's **Weather format**. Write
`{spacer}` there instead, for example:

```
%c{spacer}🌡️%t{spacer}🌬️%w
```

`{spacer}` passes through the weather service verbatim, arrives in the clock
line, and becomes the same elastic gap as `%s%` — so weather items justify
with the rest of the clock.

## Setup

1. Install **Taskbar Clock Customization** and set up your clock format.
2. Set a **Max width** in its settings, for example `120`.
3. Install this mod.
4. Edit the clock mod's **Top line** or **Bottom line** to put `%s%` between
   items, for example `%time%%s%%date%`.

The `%s%` token passes through Taskbar Clock Customization untouched and is
interpreted here at display time.

## Troubleshooting

**`%s%` disappears and nothing moves.** This is the fixed-width problem in
requirement 2 above. Set a **Max width** in Taskbar Clock Customization, or a
**Max clock width** here. The mod also writes a one-line explanation to the
Windhawk log the first time it detects this.

**Nothing happens at all.** Confirm Taskbar Clock Customization is installed and
enabled, and that `%s%` is in its **Top line** or **Bottom line** setting — not in
the tooltip, the middle line, or the weather format.

**The spacer works but the clock is the wrong width.** Adjust either the clock
mod's **Max width** or this mod's **Max clock width**. The latter applies only
to generated spacer rows, so leave it at `0` when you want the clock mod to
own the whole clock width.

## Settings

- **Max clock width** — fixed width for generated spacer rows. When it is `0`,
  the mod uses a finite **Max width** already set on the shared clock panel by
  Taskbar Clock Customization. It does not constrain an unspaced native line.
- **Minimum spacer width** — a floor, in pixels, for every gap. `0` (the default)
  leaves gaps fully elastic. A small value such as `8` guarantees a visible gap
  even before a fixed clock width is configured.

## Limitations

- `%s%` is interpreted after Taskbar Clock Customization expands its format
  tokens, so it works in the top and bottom line formats. Inside the composite
  weather segment use `{spacer}` instead — the weather service would consume
  `%s%` as its sunset token.
- Lines without `%s%` are left completely alone — the mod is a no-op for them.
- Font, size, and color of the spaced segments follow the original clock text's
  current style, so the clock mod's style settings continue to apply.

## How it works

The mod hooks two system-tray symbols and watches the clock's time and date text
blocks. `DateTimeIconContent::OnApplyTemplate` catches every clock that is
templated from then on, including after Explorer rebuilds the taskbar;
`BadgeIconContent::get_ViewModel` catches the clocks that were already on screen
when the mod was enabled, on every monitor's taskbar. Between the two there is no
clock left to search for, so the mod needs no visual-tree scan.

When a line contains `%s%`, the source text block is collapsed and a generated
panel is inserted in its place: each line becomes a Grid whose text segments sit
in `Auto` columns separated by `Star` columns, and the star columns absorb the
leftover width. When only the text changes — which happens every second — the
existing segments are rewritten in place rather than rebuilt, so the visual tree
stays stable.

## Relationship to Taskbar Clock Customization

The spacer was first offered as a patch to Taskbar Clock Customization itself
([m417z/my-windhawk-mods#68](https://github.com/m417z/my-windhawk-mods/pull/68)).
Its maintainer preferred an approach without generated layout elements and
added the **Justified** text alignment described above, then judged creating
extra text elements to be out of scope for that mod. This companion carries the
explicit-gap approach separately and leaves Taskbar Clock Customization
untouched.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxWidth: 0
  $name: Max clock width (px, 0 = off)
  $description: >-
    Fixed width for the generated spacer rows. When this is 0, the mod uses a
    finite Max width already set on the shared clock panel by Taskbar Clock
    Customization. This setting does not constrain an unspaced native line.

- minSpacerWidth: 0
  $name: Minimum spacer width (px, 0 = off)
  $description: >-
    A floor for every gap. 0 keeps gaps fully elastic. A small value such as 8
    guarantees a visible gap even before a fixed clock width is configured.
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include <windhawk_utils.h>
#include <winver.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

// ============================================================
// Visual tree walk
// ============================================================

// First descendant (root excluded) matching the predicate, depth-first in
// document order, bounded by maxDepth. This mod runs exactly one kind of
// query, over the clock's own subtree.
static FrameworkElement FindChildRecursive(FrameworkElement const& element,
    std::function<bool(FrameworkElement)> const& cb, int maxDepth = 20) {
    if (!element || maxDepth <= 0) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; ++i) {
        auto child =
            VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (cb(child)) return child;
        if (auto found = FindChildRecursive(child, cb, maxDepth - 1))
            return found;
    }
    return nullptr;
}

// ============================================================
// Taskbar window and UI-thread dispatch
//
// This mod reaches the taskbar's UI thread for exactly two reasons: to refresh
// the spaced lines when settings change, and to revoke its XAML registrations
// on unload. It needs no XamlRoot walk, no taskbar.dll symbols and no taskbar
// metrics — every clock element arrives through the system-tray hooks, which
// deliver the element itself.
// ============================================================

static HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND window, LPARAM parameter) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(window, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(window, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(parameter) = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

// XAML may only be touched from the thread that owns it. This runs work on
// the taskbar's thread with a CALLWNDPROC hook and a private registered
// message, and reports whether the callback actually ran — a caller that
// assumes it did will corrupt its own state when the dispatch failed.
using ThreadProc = void (*)(void*);

static bool InvokeUiCallback(ThreadProc proc, void* parameter) {
    try {
        proc(parameter);
        return true;
    } catch (...) {
        Wh_Log(L"UI callback failed with an exception");
    }
    return false;
}

struct Dispatch {
    ThreadProc proc;
    void* parameter;
    bool succeeded = false;
    // Each concurrent caller installs its own hook with the same proc, and
    // every hook instance sees every message equal to g_dispatchMessage, so
    // with two dispatches in flight each would otherwise run twice. The hooks
    // run one after another on the UI thread, so a plain flag suffices.
    bool ran = false;
};

// A CALLWNDPROC HOOK SEES EVERY MESSAGE SENT TO EVERY WINDOW ON THE TASKBAR'S
// UI THREAD. `lParam` for all of those is arbitrary, so the message MUST be
// checked first, against a value that does not come from lParam, and only then
// may lParam be treated as a Dispatch*. Atomic because the hook proc runs on
// the taskbar's UI thread while the caller may be another; it settles on one
// value for the session.
static std::atomic<UINT> g_dispatchMessage{0};

static bool RunFromWindowThread(HWND window, ThreadProc proc, void* parameter) {
    UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    if (!message) return false;

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) return false;
    if (threadId == GetCurrentThreadId()) return InvokeUiCallback(proc, parameter);

    g_dispatchMessage.store(message, std::memory_order_release);

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto const* call = reinterpret_cast<CWPSTRUCT const*>(lParam);
                UINT expected =
                    g_dispatchMessage.load(std::memory_order_acquire);
                if (expected && call->message == expected) {
                    if (auto* dispatch =
                            reinterpret_cast<Dispatch*>(call->lParam);
                        dispatch && !dispatch->ran) {
                        dispatch->ran = true;
                        dispatch->succeeded =
                            InvokeUiCallback(dispatch->proc, dispatch->parameter);
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) return false;

    Dispatch dispatch{proc, parameter};
    SendMessageW(window, message, 0, reinterpret_cast<LPARAM>(&dispatch));
    UnhookWindowsHookEx(hook);
    return dispatch.succeeded;
}

// ============================================================
// Settings
// ============================================================

struct ModSettings {
    int maxWidth = 0;
    int minSpacerWidth = 0;
};
static ModSettings g_settings;

// Both values end up on MinWidth/MaxWidth of a panel inside the shared clock
// StackPanel, so a fat-fingered entry is worth capping: 4000 DIPs is wider than
// any real taskbar and still leaves the setting feeling unlimited.
static constexpr int kMaxWidthDip = 4000;

static void LoadSettings() {
    g_settings.maxWidth =
        std::clamp(Wh_GetIntSetting(L"maxWidth"), 0, kMaxWidthDip);
    g_settings.minSpacerWidth =
        std::clamp(Wh_GetIntSetting(L"minSpacerWidth"), 0, kMaxWidthDip);
}

// Generated subtrees are reused across clock ticks. They must be rebuilt when a
// setting that changes their *shape* changes, so the layout settings are folded
// into a key that is stored alongside each generated panel.
static uint64_t CurrentLayoutKey() {
    return (static_cast<uint64_t>(static_cast<uint32_t>(g_settings.maxWidth))) |
           (static_cast<uint64_t>(static_cast<uint32_t>(g_settings.minSpacerWidth)) << 32);
}

// ============================================================
// Globals
// ============================================================

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_systemTrayModuleHooked{false};
static std::atomic<bool> g_warnedNoElasticRoom{false};

static void HandleLoadedModuleIfSystemTray(HMODULE hModule, LPCWSTR lpLibFileName);

static constexpr PCWSTR kSpacerToken    = L"%s%";
static constexpr size_t kSpacerTokenLen = 3;
// Weather-format spacer: wttr.in substitutes %s (sunset), so %s% cannot be
// written inside Taskbar Clock Customization's Weather format. A literal
// {spacer} instead rides through the wttr.in request untouched and arrives in
// the clock line text, where it splits exactly like %s%.
static constexpr PCWSTR kWeatherSpacerToken    = L"{spacer}";
static constexpr size_t kWeatherSpacerTokenLen = 8;
static constexpr PCWSTR kDateBlock      = L"DateInnerTextBlock";
static constexpr PCWSTR kTimeBlock      = L"TimeInnerTextBlock";

struct SpacerState {
    winrt::weak_ref<TextBlock>  originalRef;
    winrt::weak_ref<StackPanel> parentRef;
    winrt::weak_ref<StackPanel> generatedRef;
    uint64_t                    generatedLayoutKey = 0;
    int64_t                     textToken = 0;
    bool                        sourceCollapsed = false;
};

// Wh_ModUninit is not called when Explorer terminates, so this vector's
// destructor can run on the shutdown thread. That is fine here, and the reason
// is worth stating: SpacerState holds only winrt::weak_ref and integers, and
// releasing a weak_ref is an in-process refcount decrement that is safe from
// any thread. The normal destructor is therefore correct and leak-free, while a
// [[clang::no_destroy]] would only leak the vector's buffer on every unload.
static std::vector<SpacerState> g_states;  // exit-time-safe: heap-only

// ============================================================
// Spacer geometry
// ============================================================

static size_t FindNextSpacer(std::wstring_view text, size_t pos,
                             size_t* tokenLen) {
    size_t plain = text.find(kSpacerToken, pos);
    size_t weather = text.find(kWeatherSpacerToken, pos);
    if (weather < plain) {
        *tokenLen = kWeatherSpacerTokenLen;
        return weather;
    }
    *tokenLen = kSpacerTokenLen;
    return plain;
}

static bool HasSpacerToken(std::wstring_view text) {
    size_t tokenLen;
    return FindNextSpacer(text, 0, &tokenLen) != std::wstring_view::npos;
}

static std::vector<std::wstring> SplitOnSpacer(std::wstring_view text) {
    std::vector<std::wstring> segments;
    size_t pos = 0;
    while (true) {
        size_t tokenLen;
        size_t found = FindNextSpacer(text, pos, &tokenLen);
        if (found == std::wstring_view::npos) {
            segments.emplace_back(text.substr(pos));
            break;
        }
        segments.emplace_back(text.substr(pos, found - pos));
        pos = found + tokenLen;
    }
    return segments;
}

static std::vector<std::wstring> SplitLines(std::wstring_view text) {
    std::vector<std::wstring> lines;
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t found = text.find(L'\n', pos);
        if (found == std::wstring_view::npos) {
            lines.emplace_back(text.substr(pos));
            break;
        }
        size_t end = found;
        if (end > pos && text[end - 1] == L'\r') end--;
        lines.emplace_back(text.substr(pos, end - pos));
        pos = found + 1;
    }
    return lines;
}

static void CopyTextStyle(TextBlock src, TextBlock dst) {
    dst.FontSize(src.FontSize());
    dst.FontFamily(src.FontFamily());
    dst.FontWeight(src.FontWeight());
    dst.FontStyle(src.FontStyle());
    dst.FontStretch(src.FontStretch());
    dst.CharacterSpacing(src.CharacterSpacing());
    // Taskbar Clock Customization's "Line height" lands on the source block as
    // these two; without them spaced lines stack at the font's natural height
    // while the unspaced block beside them honours the user's setting.
    dst.LineHeight(src.LineHeight());
    dst.LineStackingStrategy(src.LineStackingStrategy());
    dst.Foreground(src.Foreground());
    dst.TextAlignment(src.TextAlignment());
    dst.TextWrapping(TextWrapping::NoWrap);
}

// The first and last segments must hug the fixed clock edges, otherwise each
// Auto column centers its text and the gaps look uneven.
//
// Precondition: count > 1. The only caller is BuildSpacerGrid, which is only
// reached when the line actually split on a spacer token; a single segment is
// built as a plain TextBlock and never comes through here.
static void ApplySegmentAlignment(TextBlock textBlock, int index, int count) {
    if (index == 0) {
        textBlock.HorizontalAlignment(HorizontalAlignment::Left);
        textBlock.TextAlignment(TextAlignment::Left);
    } else if (index == count - 1) {
        textBlock.HorizontalAlignment(HorizontalAlignment::Right);
        textBlock.TextAlignment(TextAlignment::Right);
    } else {
        textBlock.HorizontalAlignment(HorizontalAlignment::Center);
        textBlock.TextAlignment(TextAlignment::Center);
    }
}

static double EffectiveLineWidth(StackPanel parent) {
    if (g_settings.maxWidth > 0)
        return (double)g_settings.maxWidth;
    // Taskbar Clock Customization applies its "Max width" setting as MaxWidth
    // on this same StackPanel, so a finite value there is the fixed clock
    // width to fill. It must be read as a *setting*, never measured:
    // deriving the width from ActualWidth and then setting Width feeds the
    // next measurement, and the clock ratchets permanently wider every time
    // any line's text gets longer (live-observed as multiplying gaps).
    if (parent) {
        double parentMax = parent.MaxWidth();
        if (std::isfinite(parentMax) && parentMax > 1.0)
            return parentMax;
    }
    return 0.0;
}

// The generated panel is pinned to exactly the effective width (min AND max).
// The values are constants from settings, never measurements, so there is no
// feedback ratchet. Both bounds matter: MinWidth expands short content to the
// fixed clock width; MaxWidth stops a naturally wider line from dragging the
// panel past it — a StackPanel arranges a child at max(slot, desired), so an
// uncapped panel would exceed Taskbar Clock Customization's Max width and
// stretch every spaced row with it.
static void ApplyPanelWidthConstraint(FrameworkElement element, double width) {
    if (!element) return;
    if (width > 1.0) {
        element.MinWidth(width);
        element.MaxWidth(width);
    } else {
        element.ClearValue(FrameworkElement::MinWidthProperty());
        element.ClearValue(FrameworkElement::MaxWidthProperty());
    }
}

// Rows only get the cap. They stretch to the pinned panel width, and an
// unspaced over-long line (for example the weather line) clips at the fixed
// width exactly like the native text block does under TCC's Max width.
static void ApplyRowWidthCap(FrameworkElement element, double width) {
    if (!element) return;
    if (width > 1.0)
        element.MaxWidth(width);
    else
        element.ClearValue(FrameworkElement::MaxWidthProperty());
}

static void WarnIfNoElasticRoom(bool hasElasticRoom) {
    if (hasElasticRoom || g_warnedNoElasticRoom.exchange(true))
        return;
    Wh_Log(L"No spare width to distribute, so %%s%% produces no visible "
           L"gap. Set 'Max width' in Taskbar Clock Customization, or 'Max clock "
           L"width' in this mod, to give the spacer room to expand.");
}

// ============================================================
// Spacer grid construction
// ============================================================

// Layout: [Auto text] [* gap] [Auto text] [* gap] ... [Auto text]
static Grid BuildSpacerGrid(winrt::hstring const& name,
                            const std::vector<std::wstring>& segments,
                            TextBlock styleSource,
                            double width) {
    Grid grid;
    grid.Name(name + L"_Spacer");
    grid.HorizontalAlignment(HorizontalAlignment::Stretch);
    grid.VerticalAlignment(VerticalAlignment::Center);
    ApplyRowWidthCap(grid, width);

    double minSpacer = (double)g_settings.minSpacerWidth;

    int segmentCount = (int)segments.size();
    for (int i = 0; i < segmentCount; i++) {
        ColumnDefinition textColumn;
        textColumn.Width({1.0, GridUnitType::Auto});
        grid.ColumnDefinitions().Append(textColumn);

        if (i + 1 < segmentCount) {
            ColumnDefinition spacerColumn;
            spacerColumn.Width({1.0, GridUnitType::Star});
            if (minSpacer > 0.0)
                spacerColumn.MinWidth(minSpacer);
            grid.ColumnDefinitions().Append(spacerColumn);
        }
    }

    int gridColumn = 0;
    for (int i = 0; i < segmentCount; i++) {
        TextBlock textBlock;
        textBlock.Text(segments[i]);
        textBlock.VerticalAlignment(VerticalAlignment::Center);
        CopyTextStyle(styleSource, textBlock);
        ApplySegmentAlignment(textBlock, i, segmentCount);
        Grid::SetColumn(textBlock, gridColumn);
        grid.Children().Append(textBlock);
        gridColumn += 2;
    }

    return grid;
}

// `width` comes from the caller, which has already computed it for the panel —
// recomputing it per line would just read the same settings again.
static FrameworkElement BuildLineElement(winrt::hstring const& baseName,
                                         std::wstring const& line,
                                         TextBlock styleSource,
                                         double width,
                                         int lineIndex) {
    auto segments = SplitOnSpacer(line);

    if (segments.size() > 1)
        return BuildSpacerGrid(baseName + L"_Line" + winrt::to_hstring(lineIndex),
                               segments, styleSource, width);

    TextBlock textBlock;
    textBlock.Name(baseName + L"_Line" + winrt::to_hstring(lineIndex));
    textBlock.Text(line);
    textBlock.VerticalAlignment(VerticalAlignment::Center);
    CopyTextStyle(styleSource, textBlock);
    ApplyRowWidthCap(textBlock, width);
    return textBlock;
}

// ============================================================
// In-place update
//
// The clock text changes every second. Rebuilding the generated subtree each
// tick thrashes layout and makes the inspected visual tree unstable, so when the
// shape is unchanged only the text is rewritten.
// ============================================================

// The style is re-copied here, not only when the subtree is rebuilt. Taskbar
// Clock Customization's font size, family and colour live on the source text
// block and can change without this mod's settings changing — and the layout key
// folds in only this mod's settings, so the fast path is exactly the case where
// a style change would otherwise be missed. Without this, the spaced rows keep
// the old style while the unspaced ones update, leaving a visibly mismatched
// clock until the segment count or one of this mod's settings happens to change.
static bool UpdateLineElementText(FrameworkElement lineElement,
                                  std::wstring const& line,
                                  TextBlock styleSource,
                                  double width) {
    if (!lineElement) return false;
    auto segments = SplitOnSpacer(line);
    // Reapplied on the fast path: a TCC Max width change alters the effective
    // width without changing this mod's settings (the layout key).
    ApplyRowWidthCap(lineElement, width);

    if (segments.size() > 1) {
        auto grid = lineElement.try_as<Grid>();
        if (!grid) return false;
        if (grid.Children().Size() != (uint32_t)segments.size()) return false;
        for (uint32_t i = 0; i < (uint32_t)segments.size(); i++) {
            auto textBlock = grid.Children().GetAt(i).try_as<TextBlock>();
            if (!textBlock) return false;
            textBlock.Text(segments[i]);
            if (styleSource) {
                CopyTextStyle(styleSource, textBlock);
                // CopyTextStyle does not own alignment, and the per-segment
                // edge-hugging must survive it.
                ApplySegmentAlignment(textBlock, (int)i, (int)segments.size());
            }
        }
        return true;
    }

    auto textBlock = lineElement.try_as<TextBlock>();
    if (!textBlock) return false;
    textBlock.Text(line);
    if (styleSource) CopyTextStyle(styleSource, textBlock);
    return true;
}

static bool UpdateGeneratedPanelText(StackPanel generatedPanel,
                                     std::vector<std::wstring> const& lines,
                                     TextBlock styleSource,
                                     double width) {
    if (!generatedPanel ||
        generatedPanel.Children().Size() != (uint32_t)lines.size())
        return false;

    ApplyPanelWidthConstraint(generatedPanel, width);

    for (uint32_t i = 0; i < (uint32_t)lines.size(); i++) {
        auto lineElement = generatedPanel.Children().GetAt(i).try_as<FrameworkElement>();
        if (!lineElement ||
            !UpdateLineElementText(lineElement, lines[i], styleSource, width))
            return false;
    }
    return true;
}

// ============================================================
// Source text block visibility
// ============================================================

// Zero both axes: Taskbar Clock Customization re-sets Visibility on its own
// schedule, and a nonzero-width collapsed block would still widen the shared
// StackPanel past the generated rows.
static void CollapseSourceTextBlock(SpacerState& state, TextBlock original) {
    if (!original) return;
    original.Height(0.0);
    original.MinHeight(0.0);
    original.MaxHeight(0.0);
    original.Width(0.0);
    original.MinWidth(0.0);
    original.Visibility(Visibility::Collapsed);
    state.sourceCollapsed = true;
}

static void RestoreSourceTextBlock(SpacerState& state, TextBlock original) {
    // Do not overwrite a Visibility value owned by the clock template or Taskbar
    // Clock Customization. This mod restores only a block it collapsed itself.
    if (!original || !state.sourceCollapsed) return;
    original.ClearValue(FrameworkElement::HeightProperty());
    original.ClearValue(FrameworkElement::MinHeightProperty());
    original.ClearValue(FrameworkElement::MaxHeightProperty());
    original.ClearValue(FrameworkElement::WidthProperty());
    original.ClearValue(FrameworkElement::MinWidthProperty());
    original.ClearValue(UIElement::VisibilityProperty());
    state.sourceCollapsed = false;
}

static void RemoveGeneratedPanel(SpacerState& state) {
    auto parent = state.parentRef.get();
    auto generated = state.generatedRef.get();
    if (parent && generated) {
        uint32_t index;
        if (parent.Children().IndexOf(generated, index))
            parent.Children().RemoveAt(index);
    }
    state.generatedRef = {};
    state.generatedLayoutKey = 0;
}

// ============================================================
// Per-line update
// ============================================================

static void UpdateSpacerLine(SpacerState& state) {
    auto original = state.originalRef.get();
    auto parent   = state.parentRef.get();
    if (!original || !parent) return;

    // The parent StackPanel is deliberately never resized here. Taskbar Clock
    // Customization owns its MaxWidth (clearing it erased the user's fixed
    // clock width), and the panel is auto-width, so it follows the generated
    // rows on its own once the source block collapses to zero size.

    winrt::hstring textHString = original.Text();
    std::wstring fullText{textHString.c_str(), textHString.size()};

    if (!HasSpacerToken(fullText)) {
        RemoveGeneratedPanel(state);
        RestoreSourceTextBlock(state, original);
        return;
    }

    double width = EffectiveLineWidth(parent);
    WarnIfNoElasticRoom(width > 1.0);
    auto lines = SplitLines(fullText);
    uint64_t layoutKey = CurrentLayoutKey();

    // Fast path: same shape, same settings — rewrite text only.
    if (auto generated = state.generatedRef.get();
        generated && state.generatedLayoutKey == layoutKey &&
        UpdateGeneratedPanelText(generated, lines, original, width)) {
        CollapseSourceTextBlock(state, original);
        return;
    }

    RemoveGeneratedPanel(state);

    StackPanel generated;
    generated.Name(original.Name() + L"_SpacerPanel");
    generated.Orientation(Orientation::Vertical);
    // No Spacing between rows, deliberately. An unspaced multi-line block is
    // ONE TextBlock whose lines are separated only by its line height, which
    // CopyTextStyle carries onto each row. Taskbar Clock Customization's
    // "Text spacing" is the gap between the time and date blocks in the shared
    // panel, not between lines, so copying it here would add a gap the
    // unspaced block does not have.
    generated.HorizontalAlignment(HorizontalAlignment::Stretch);
    generated.VerticalAlignment(VerticalAlignment::Center);
    ApplyPanelWidthConstraint(generated, width);

    for (int i = 0; i < (int)lines.size(); i++)
        generated.Children().Append(
            BuildLineElement(original.Name(), lines[i], original, width, i));

    uint32_t originalIndex = 0;
    if (parent.Children().IndexOf(original, originalIndex))
        parent.Children().InsertAt(originalIndex, generated);
    else
        parent.Children().Append(generated);

    state.generatedRef = winrt::make_weak(generated);
    state.generatedLayoutKey = layoutKey;
    CollapseSourceTextBlock(state, original);
    generated.Visibility(Visibility::Visible);
}

// ============================================================
// Registration
// ============================================================

static void SetupSpacerForTextBlock(StackPanel parent, TextBlock textBlock) {
    if (!parent || !textBlock) return;

    for (auto& state : g_states)
        if (state.originalRef.get() == textBlock) return;

    // After a taskbar rebuild the old text blocks die and their weak_refs
    // expire, but the entries would otherwise stay forever — and every
    // registration and every text change walks this vector. Registration is
    // rare, so this is the right place to keep it bounded.
    std::erase_if(g_states, [](SpacerState const& state) {
        return !state.originalRef.get();
    });

    // RECORD BEFORE BUILDING. UpdateSpacerLine inserts a generated panel into
    // the live tree; if it threw after that on a state not yet in g_states,
    // unload would have nothing to remove the panel with.
    SpacerState state;
    state.originalRef = winrt::make_weak(textBlock);
    state.parentRef   = winrt::make_weak(parent);
    g_states.push_back(std::move(state));
    UpdateSpacerLine(g_states.back());

    g_states.back().textToken = textBlock.RegisterPropertyChangedCallback(
        TextBlock::TextProperty(),
        [](DependencyObject sender, DependencyProperty) {
            if (g_unloading) return;
            auto changed = sender.try_as<TextBlock>();
            if (!changed) return;
            for (auto& state : g_states) {
                if (state.originalRef.get() == changed) {
                    UpdateSpacerLine(state);
                    return;
                }
            }
        });

    Wh_Log(L"Registered '%s'", textBlock.Name().c_str());
}

static void ApplySpacerToDateTimeContent(FrameworkElement element) {
    PCWSTR blockNames[] = {kTimeBlock, kDateBlock};
    int found = 0;
    for (PCWSTR blockName : blockNames) {
        auto textBlockElement = FindChildRecursive(element, [blockName](FrameworkElement fe) {
            return fe.Name() == blockName;
        });
        if (!textBlockElement) { Wh_Log(L"'%s' not found", blockName); continue; }
        auto textBlock = textBlockElement.try_as<TextBlock>();
        if (!textBlock) continue;
        auto parentDep = VisualTreeHelper::GetParent(textBlock);
        if (!parentDep) continue;
        auto parent = parentDep.try_as<StackPanel>();
        if (!parent) { Wh_Log(L"parent of '%s' not a StackPanel", blockName); continue; }
        SetupSpacerForTextBlock(parent, textBlock);
        found++;
    }
    if (!found) Wh_Log(L"No text blocks found in DateTimeIconContent");
}

// ============================================================
// Hooks
// ============================================================

using DateTimeIconContent_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
DateTimeIconContent_OnApplyTemplate_t DateTimeIconContent_OnApplyTemplate_Original;

void WINAPI DateTimeIconContent_OnApplyTemplate_Hook(void* pThis) {
    DateTimeIconContent_OnApplyTemplate_Original(pThis);
    if (g_unloading) return;

    auto* iunk = *((IUnknown**)pThis + 1);
    if (!iunk) return;
    FrameworkElement element = nullptr;
    iunk->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    if (!element) return;

    try {
        ApplySpacerToDateTimeContent(element);
    } catch (...) {
        Wh_Log(L"Exception in OnApplyTemplate hook");
    }
}

// The other half of the coverage. OnApplyTemplate fires for a clock that is
// templated after this mod loads, which includes every clock on a taskbar
// Explorer rebuilds — but NOT a clock that was already rendered when the user
// enabled the mod. get_ViewModel is called on every live instance shortly
// after load, so between the two there is no clock left to go looking for:
// no XamlRoot walk, no taskbar.dll symbols, and no scan thread. It also
// reaches secondary-monitor taskbars, which a Shell_TrayWnd-rooted scan never
// did, because it is handed the element rather than searching for it.
using BadgeIconContent_get_ViewModel_t =
    HRESULT(WINAPI*)(LPVOID pThis, LPVOID pArgs);
BadgeIconContent_get_ViewModel_t BadgeIconContent_get_ViewModel_Original;

HRESULT WINAPI BadgeIconContent_get_ViewModel_Hook(LPVOID pThis, LPVOID pArgs) {
    HRESULT result = BadgeIconContent_get_ViewModel_Original(pThis, pArgs);
    if (g_unloading) return result;

    try {
        winrt::Windows::Foundation::IInspectable object = nullptr;
        winrt::check_hresult(
            static_cast<IUnknown*>(pThis)->QueryInterface(
                winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
                winrt::put_abi(object)));

        // ContainerGrid appears throughout the system tray, so the runtime
        // class name is the only reliable way to single out the clock.
        if (winrt::get_class_name(object) == L"SystemTray.DateTimeIconContent") {
            auto content = object.as<FrameworkElement>();
            // An unloaded element has no template applied yet, so its text
            // blocks do not exist; OnApplyTemplate covers that instance.
            if (content.IsLoaded())
                ApplySpacerToDateTimeContent(content);
        }
    } catch (...) {
        Wh_Log(L"Exception in get_ViewModel hook");
    }

    return result;
}

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;
    HRSRC hResource = FindResourceW(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen) || !uPtrLen)
                    pFixedFileInfo = nullptr;
            }
        }
    }
    if (puPtrLen) *puPtrLen = uPtrLen;
    return static_cast<VS_FIXEDFILEINFO*>(pFixedFileInfo);
}

// Order matters: SystemTray.dll is the new home (Win11 Insider 26200+);
// older builds have the symbols in Taskbar.View.dll.
static HMODULE GetSystemTrayModuleHandle() {
    if (HMODULE h = GetModuleHandleW(L"SystemTray.dll")) return h;
    if (HMODULE h = GetModuleHandleW(L"Taskbar.View.dll")) {
        // Starting with Taskbar.View.dll 2604.x, the SystemTray types moved out
        // into SystemTray.dll — don't hook this version.
        VS_FIXEDFILEINFO* fi = GetModuleVersionInfo(h, nullptr);
        WORD moduleMajor = fi ? HIWORD(fi->dwFileVersionMS) : 0;
        if (!moduleMajor || moduleMajor >= 2604) return nullptr;
        return h;
    }
    if (HMODULE h = GetModuleHandleW(L"ExplorerExtensions.dll")) return h;
    return nullptr;
}

// OnApplyTemplate is REQUIRED: it is the whole feature. Marked optional,
// HookSymbols would report success when the symbol no longer resolves, and the
// mod would sit resident doing nothing but logging that it loaded.
// get_ViewModel is optional because it only covers already-rendered clocks;
// losing it costs one reload of Explorer, not the feature.
static bool HookSystemTraySymbols(HMODULE h) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayModuleHooks[] = {
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::DateTimeIconContent::OnApplyTemplate(void))"},
            &DateTimeIconContent_OnApplyTemplate_Original,
            DateTimeIconContent_OnApplyTemplate_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::BadgeIconContent,struct winrt::SystemTray::IBadgeIconContent>::get_ViewModel(void * *))"},
            &BadgeIconContent_get_ViewModel_Original,
            BadgeIconContent_get_ViewModel_Hook,
            true,
        },
    };
    return WindhawkUtils::HookSymbols(h, systemTrayModuleHooks,
                                      ARRAYSIZE(systemTrayModuleHooks));
}

static void TryHookSystemTrayModule(PCWSTR reason) {
    if (g_systemTrayModuleHooked) return;
    HMODULE h = GetSystemTrayModuleHandle();
    if (!h) return;
    if (g_systemTrayModuleHooked.exchange(true)) return;
    Wh_Log(L"System tray module found (%s) — hooking symbols", reason);
    if (HookSystemTraySymbols(h))
        Wh_ApplyHookOperations();
    else
        Wh_Log(L"System tray symbol hooks failed");
}

// The late-load path. When the system tray module is not yet loaded at init
// (the mod was enabled before Explorer finished starting), watching
// LoadLibraryExW is how the hooks get placed once it arrives.
using LoadLibraryExW_t = HMODULE (WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (hModule && lpLibFileName)
        HandleLoadedModuleIfSystemTray(hModule, lpLibFileName);
    return hModule;
}

static void HandleLoadedModuleIfSystemTray(HMODULE hModule, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == hModule) {
        Wh_Log(L"%s", lpLibFileName);
        TryHookSystemTrayModule(L"LoadLibraryExW");
    }
}

// ============================================================
// Uninit
// ============================================================

// ONE ENTRY'S FAILURE MUST NOT STRAND THE REST. Unload retries this whole
// function if the dispatch reports failure, so an exception escaping from
// entry k would re-throw on every retry and leave the callbacks of entries
// k+1..n registered on live XAML that outlives this image. Each entry is
// guarded on its own, and its token is zeroed as soon as it is unregistered,
// which makes a retry safe to run over entries already handled.
static void ClearSpacerStates() {
    for (auto& state : g_states) {
        try {
            if (auto textBlock = state.originalRef.get()) {
                if (state.textToken) {
                    textBlock.UnregisterPropertyChangedCallback(
                        TextBlock::TextProperty(), state.textToken);
                    state.textToken = 0;
                }
                RestoreSourceTextBlock(state, textBlock);
            }
            // The parent StackPanel is intentionally untouched: this mod never
            // sets anything on it, and clearing MaxWidth here would erase
            // Taskbar Clock Customization's fixed clock width.
            RemoveGeneratedPanel(state);
        } catch (...) {
            Wh_Log(L"Cleanup of one clock line failed; continuing");
        }
    }
    g_states.clear();
}

// ============================================================
// Windhawk lifecycle
// ============================================================

BOOL Wh_ModInit() {
    Wh_Log(L"Clock Spacer v%s", WH_MOD_VERSION);
    LoadSettings();

    // The system-tray symbols are the whole mod. There are deliberately no
    // taskbar.dll hooks: a rebuilt taskbar re-templates its DateTimeIconContent,
    // so OnApplyTemplate already covers the rebuild that TrayUI::StartTaskbar
    // used to announce, and nothing here needs a XamlRoot any more.
    if (HMODULE hSystemTray = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(hSystemTray)) {
            Wh_Log(L"System tray symbol hooks failed");
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded — watching LoadLibraryExW");
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        auto pLoadLibraryExW = kernelbase
            ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelbase, "LoadLibraryExW"))
            : nullptr;
        if (!pLoadLibraryExW ||
            !WindhawkUtils::SetFunctionHook(pLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original)) {
            Wh_Log(L"LoadLibraryExW hook unavailable");
            return FALSE;
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    TryHookSystemTrayModule(L"Wh_ModAfterInit");
    Wh_Log(L"hooked=%d", (int)g_systemTrayModuleHooked.load());
    // No scan: get_ViewModel reaches the clocks that are already on screen,
    // including the ones on secondary-monitor taskbars.
}

void Wh_ModUninit() {
    g_unloading = true;
    Wh_Log(L"Uninit");

    // ClearSpacerStates owns XAML registrations and must never run from an
    // arbitrary Windhawk thread. Retry a taskbar-thread dispatch briefly; when
    // none exists the tree is already gone, so retain only weak state safely.
    bool cleared = false;
    for (int i = 0; i < 5 && !cleared; ++i) {
        if (HWND hWnd = FindCurrentProcessTaskbarWnd())
            cleared = RunFromWindowThread(
                hWnd, [](void*) { ClearSpacerStates(); }, nullptr);
        if (!cleared) Sleep(100);
    }
    if (!cleared)
        Wh_Log(L"Failed to dispatch XAML cleanup; taskbar tree is unavailable");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    g_warnedNoElasticRoom.store(false);
    Wh_Log(L"maxWidth=%d minSpacerWidth=%d",
           g_settings.maxWidth, g_settings.minSpacerWidth);

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"No taskbar window found");
        return;
    }

    RunFromWindowThread(hWnd, [](void*) {
        for (auto& state : g_states)
            UpdateSpacerLine(state);
    }, nullptr);
}
