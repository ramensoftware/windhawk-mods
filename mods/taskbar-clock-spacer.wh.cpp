// ==WindhawkMod==
// @id              taskbar-clock-spacer
// @name            Taskbar Clock Spacer
// @description     Companion for Taskbar Clock Customization: adds a %s% elastic spacer token that distributes leftover clock width between items. Windows 11 only.
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

**The spacer works but the clock is the wrong width.** Adjust the same Max width
value. Use **Line width override** only if the automatic width is being read
incorrectly.

## Settings

- **Line width override** — explicit width for the spacer grid. Usually `0`
  (automatic) is correct; the width is inherited from the clock's Max width.
- **Max clock width** — fixed width for the generated spacer rows. Equivalent
  to setting Max width in Taskbar Clock Customization; that mod's own Max width
  is respected automatically when this is `0`.
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
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- lineWidth: 0
  $name: Line width override (px, 0 = auto)
  $description: >-
    Explicit width for the spacer grid. Usually 0 is correct — the clock area
    inherits its width from the Max width set in Taskbar Clock Customization.
    Set this only if the spacer doesn't expand as expected.

- maxWidth: 0
  $name: Max clock width (px, 0 = off)
  $description: >-
    Fixed width for the generated spacer rows. Equivalent to setting Max width
    in Taskbar Clock Customization — use whichever you prefer; that mod's own
    Max width is respected automatically when this is 0.

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

#include <atomic>
#include <cmath>
#include <functional>
#include <string>
#include <vector>

#include <windhawk_utils.h>
#include <winver.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

// ============================================================
// Settings
// ============================================================

struct ModSettings {
    int lineWidth = 0;
    int maxWidth = 0;
    int minSpacerWidth = 0;
};
static ModSettings g_settings;

static void LoadSettings() {
    g_settings.lineWidth = Wh_GetIntSetting(L"lineWidth");
    g_settings.maxWidth = Wh_GetIntSetting(L"maxWidth");
    g_settings.minSpacerWidth = Wh_GetIntSetting(L"minSpacerWidth");
    if (g_settings.lineWidth < 0) g_settings.lineWidth = 0;
    if (g_settings.maxWidth < 0) g_settings.maxWidth = 0;
    if (g_settings.minSpacerWidth < 0) g_settings.minSpacerWidth = 0;
}

// Generated subtrees are reused across clock ticks. They must be rebuilt when a
// setting that changes their *shape* changes, so the layout settings are folded
// into a key that is stored alongside each generated panel.
static uint64_t CurrentLayoutKey() {
    return (static_cast<uint64_t>(static_cast<uint32_t>(g_settings.maxWidth))) |
           (static_cast<uint64_t>(static_cast<uint32_t>(g_settings.lineWidth)) << 20) |
           (static_cast<uint64_t>(static_cast<uint32_t>(g_settings.minSpacerWidth)) << 40);
}

// ============================================================
// GetTaskbarXamlRoot
// ============================================================

using CTaskBand_GetTaskbarHost_t = void* (WINAPI*)(void* pThis, void* result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using TaskbarHost_FrameHeight_t = int (WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void (WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

static void* CTaskBand_ITaskListWndSite_vftable = nullptr;

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    // Every one of these is dereferenced below. If symbol resolution failed,
    // proceeding would call through a null pointer.
    if (!CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original ||
        !CTaskBand_ITaskListWndSite_vftable)
        return nullptr;

    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) return nullptr;

    void* taskBandForSite = taskBand;
    for (int i = 0; *(void**)taskBandForSite != CTaskBand_ITaskListWndSite_vftable; i++) {
        if (i == 20) return nullptr;
        taskBandForSite = (void**)taskBandForSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);
    // Either slot being null means we have no usable host; release the control
    // block if we got one so a partial result doesn't leak a reference.
    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) {
        if (taskbarHostSharedPtr[1])
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    size_t offset = 0x10;
#if defined(_M_X64)
    {
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F)
            offset = b[7];
        else
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }
#elif defined(_M_ARM64)
    {
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00)
            offset = (p[3] >> 12) & 0xFF;
        else
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }
#else
#error "Unsupported architecture"
#endif

    auto* iunk = *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] + offset);
    if (!iunk) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    FrameworkElement taskbarElem = nullptr;
    iunk->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElem));
    auto result = taskbarElem ? taskbarElem.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}

// ============================================================
// Globals
// ============================================================

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_scanDone{false};
static std::atomic<bool> g_systemTrayModuleHooked{false};
static std::atomic<bool> g_warnedNoElasticRoom{false};
static HANDLE g_scanThread = nullptr;
static HANDLE g_scanStopEvent = nullptr;

static void HandleLoadedModuleIfSystemTray(HMODULE hModule, LPCWSTR lpLibFileName);
static void StartInitialScan();

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
};

// Wh_ModUninit is not called when Explorer terminates. Without this the vector's
// destructor runs on the shutdown thread and releases XAML weak references off
// their UI thread.
[[clang::no_destroy]] static std::vector<SpacerState> g_states;

// ============================================================
// XAML helpers
// ============================================================

static FrameworkElement FindChildRecursive(FrameworkElement const& element,
    std::function<bool(FrameworkElement)> const& cb, int maxDepth = 20) {
    int n = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < n && maxDepth > 0; i++) {
        auto child = VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (cb(child)) return child;
        auto found = FindChildRecursive(child, cb, maxDepth - 1);
        if (found) return found;
    }
    return nullptr;
}

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
    dst.Foreground(src.Foreground());
    dst.TextAlignment(src.TextAlignment());
    dst.TextWrapping(TextWrapping::NoWrap);
}

// The first and last segments must hug the fixed clock edges, otherwise each
// Auto column centers its text and the gaps look uneven.
static void ApplySegmentAlignment(TextBlock textBlock, int index, int count) {
    if (count <= 1) {
        textBlock.HorizontalAlignment(HorizontalAlignment::Stretch);
        return;
    }
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

static double EffectiveLineWidth(TextBlock original, StackPanel parent) {
    if (g_settings.lineWidth > 0)
        return (double)g_settings.lineWidth;
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
    Wh_Log(L"[Spacer] No spare width to distribute, so %%s%% produces no visible "
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

static FrameworkElement BuildLineElement(winrt::hstring const& baseName,
                                         std::wstring const& line,
                                         TextBlock styleSource,
                                         StackPanel parent,
                                         int lineIndex) {
    auto segments = SplitOnSpacer(line);
    double width = EffectiveLineWidth(styleSource, parent);

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

static bool UpdateLineElementText(FrameworkElement lineElement,
                                  std::wstring const& line,
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
        }
        return true;
    }

    auto textBlock = lineElement.try_as<TextBlock>();
    if (!textBlock) return false;
    textBlock.Text(line);
    return true;
}

static bool UpdateGeneratedPanelText(StackPanel generatedPanel,
                                     std::vector<std::wstring> const& lines,
                                     double width) {
    if (!generatedPanel ||
        generatedPanel.Children().Size() != (uint32_t)lines.size())
        return false;

    ApplyPanelWidthConstraint(generatedPanel, width);

    for (uint32_t i = 0; i < (uint32_t)lines.size(); i++) {
        auto lineElement = generatedPanel.Children().GetAt(i).try_as<FrameworkElement>();
        if (!lineElement || !UpdateLineElementText(lineElement, lines[i], width))
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
static void CollapseSourceTextBlock(TextBlock original) {
    if (!original) return;
    original.Height(0.0);
    original.MinHeight(0.0);
    original.MaxHeight(0.0);
    original.Width(0.0);
    original.MinWidth(0.0);
    original.Visibility(Visibility::Collapsed);
}

static void RestoreSourceTextBlock(TextBlock original) {
    if (!original) return;
    original.ClearValue(FrameworkElement::HeightProperty());
    original.ClearValue(FrameworkElement::MinHeightProperty());
    original.ClearValue(FrameworkElement::MaxHeightProperty());
    original.ClearValue(FrameworkElement::WidthProperty());
    original.ClearValue(FrameworkElement::MinWidthProperty());
    original.Visibility(Visibility::Visible);
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
        RestoreSourceTextBlock(original);
        return;
    }

    double width = EffectiveLineWidth(original, parent);
    WarnIfNoElasticRoom(width > 1.0);
    auto lines = SplitLines(fullText);
    uint64_t layoutKey = CurrentLayoutKey();

    // Fast path: same shape, same settings — rewrite text only.
    if (auto generated = state.generatedRef.get();
        generated && state.generatedLayoutKey == layoutKey &&
        UpdateGeneratedPanelText(generated, lines, width)) {
        CollapseSourceTextBlock(original);
        return;
    }

    RemoveGeneratedPanel(state);

    StackPanel generated;
    generated.Name(original.Name() + L"_SpacerPanel");
    generated.Orientation(Orientation::Vertical);
    generated.HorizontalAlignment(HorizontalAlignment::Stretch);
    generated.VerticalAlignment(VerticalAlignment::Center);
    ApplyPanelWidthConstraint(generated, width);

    for (int i = 0; i < (int)lines.size(); i++)
        generated.Children().Append(
            BuildLineElement(original.Name(), lines[i], original, parent, i));

    uint32_t originalIndex = 0;
    if (parent.Children().IndexOf(original, originalIndex))
        parent.Children().InsertAt(originalIndex, generated);
    else
        parent.Children().Append(generated);

    state.generatedRef = winrt::make_weak(generated);
    state.generatedLayoutKey = layoutKey;
    CollapseSourceTextBlock(original);
    generated.Visibility(Visibility::Visible);
}

// ============================================================
// Registration
// ============================================================

static void SetupSpacerForTextBlock(StackPanel parent, TextBlock textBlock) {
    if (!parent || !textBlock) return;

    for (auto& state : g_states)
        if (state.originalRef.get() == textBlock) return;

    SpacerState state;
    state.originalRef = winrt::make_weak(textBlock);
    state.parentRef   = winrt::make_weak(parent);
    UpdateSpacerLine(state);

    g_states.push_back(std::move(state));

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

    Wh_Log(L"[Spacer] Registered '%s'", textBlock.Name().c_str());
}

static void ApplySpacerToDateTimeContent(FrameworkElement element) {
    PCWSTR blockNames[] = {kTimeBlock, kDateBlock};
    int found = 0;
    for (PCWSTR blockName : blockNames) {
        auto textBlockElement = FindChildRecursive(element, [blockName](FrameworkElement fe) {
            return fe.Name() == blockName;
        });
        if (!textBlockElement) { Wh_Log(L"[Spacer] '%s' not found", blockName); continue; }
        auto textBlock = textBlockElement.try_as<TextBlock>();
        if (!textBlock) continue;
        auto parentDep = VisualTreeHelper::GetParent(textBlock);
        if (!parentDep) continue;
        auto parent = parentDep.try_as<StackPanel>();
        if (!parent) { Wh_Log(L"[Spacer] parent of '%s' not a StackPanel", blockName); continue; }
        SetupSpacerForTextBlock(parent, textBlock);
        found++;
    }
    if (!found) Wh_Log(L"[Spacer] No text blocks found in DateTimeIconContent");
}

// ============================================================
// Initial scan (for elements rendered before mod load)
// ============================================================

static void ScanForSpacerTargets(FrameworkElement root) {
    if (!root) return;
    // ContainerGrid appears throughout the system tray, so the class name is the
    // only reliable way to identify DateTimeIconContent specifically.
    try {
        if (winrt::get_class_name(root) == L"SystemTray.DateTimeIconContent") {
            ApplySpacerToDateTimeContent(root);
            return;
        }
    } catch (...) {}
    int n = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (child) ScanForSpacerTargets(child);
    }
}

using RunFromWindowThreadProc_t = void (*)(void*);

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT kMsg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param { RunFromWindowThreadProc_t proc; void* procParam; };
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!dwThreadId) return false;
    if (dwThreadId == GetCurrentThreadId()) { proc(procParam); return true; }
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
        if (nCode == HC_ACTION) {
            const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
            if (cwp->message == RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID)) {
                auto* p = (Param*)cwp->lParam;
                p->proc(p->procParam);
            }
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }, nullptr, dwThreadId);
    if (!hook) return false;
    Param param{ proc, procParam };
    SendMessage(hWnd, kMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD pid = 0;
        WCHAR cls[32];
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
            _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(lParam) = hWnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
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
        Wh_Log(L"[Spacer] Exception in OnApplyTemplate hook");
    }
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

static bool HookSystemTraySymbols(HMODULE h) {
    // SystemTray.dll, Taskbar.View.dll (pre-2604), ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {{
        {LR"(public: void __cdecl winrt::SystemTray::implementation::DateTimeIconContent::OnApplyTemplate(void))"},
        &DateTimeIconContent_OnApplyTemplate_Original,
        DateTimeIconContent_OnApplyTemplate_Hook,
        true,
    }};
    return WindhawkUtils::HookSymbols(h, systemTrayDllHooks,
                                      ARRAYSIZE(systemTrayDllHooks));
}

static void TryHookSystemTrayModule(PCWSTR reason) {
    if (g_systemTrayModuleHooked) return;
    HMODULE h = GetSystemTrayModuleHandle();
    if (!h) return;
    if (g_systemTrayModuleHooked.exchange(true)) return;
    Wh_Log(L"[Hooks] System tray module found (%s) — hooking symbols", reason);
    if (HookSystemTraySymbols(h))
        Wh_ApplyHookOperations();
    else
        Wh_Log(L"[Hooks] System tray symbol hooks failed");
}

// Preferred wait-for-module path: TrayUI::StartTaskbar runs once the taskbar is
// actually starting, by which point the system tray module is loaded. This
// replaces watching every DLL load in the process.
using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (g_unloading) return;
    TryHookSystemTrayModule(L"TrayUI::StartTaskbar");
    StartInitialScan();
}

// Fallback only, used when the TrayUI::StartTaskbar symbol cannot be resolved.
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
        Wh_Log(L"[LoadLib] %s", lpLibFileName);
        TryHookSystemTrayModule(L"LoadLibraryExW");
    }
}

static bool g_trayUiStartTaskbarHooked = false;

static bool HookTaskbarDllSymbols() {
    HMODULE h = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!h) return false;

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        { {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
          &CTaskBand_ITaskListWndSite_vftable },
        { {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
          &CTaskBand_GetTaskbarHost_Original },
        { {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
          &TaskbarHost_FrameHeight_Original },
        { {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
          &std__Ref_count_base__Decref_Original },
    };
    if (!WindhawkUtils::HookSymbols(h, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks)))
        return false;

    // Optional: resolution failure falls back to the LoadLibraryExW watcher.
    // taskbar.dll
    WindhawkUtils::SYMBOL_HOOK startTaskbarHooks[] = {{
        {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
        &TrayUI_StartTaskbar_Original,
        TrayUI_StartTaskbar_Hook,
        true,
    }};
    g_trayUiStartTaskbarHooked =
        WindhawkUtils::HookSymbols(h, startTaskbarHooks,
                                   ARRAYSIZE(startTaskbarHooks)) &&
        TrayUI_StartTaskbar_Original != nullptr;
    return true;
}

// ============================================================
// Initial scan
// ============================================================

static void WaitForThreadWithSentMessagePump(HANDLE thread) {
    DWORD result;
    do {
        result = MsgWaitForMultipleObjects(1, &thread, FALSE, INFINITE, QS_SENDMESSAGE);
        if (result == WAIT_OBJECT_0 + 1) {
            MSG msg;
            PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
        }
    } while (result == WAIT_OBJECT_0 + 1);
}

static void StartInitialScan() {
    if (g_unloading || g_scanThread || g_scanDone) return;

    g_scanStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_scanStopEvent) {
        Wh_Log(L"[Spacer] Failed to create scan stop event");
        return;
    }

    g_scanThread = CreateThread(nullptr, 0, [](void* param) -> DWORD {
        HANDLE stopEvent = static_cast<HANDLE>(param);
        for (int i = 0; i < 5 && !g_unloading && !g_scanDone; i++) {
            if (i > 0 && WaitForSingleObject(stopEvent, 2000) != WAIT_TIMEOUT)
                break;
            HWND hWnd = FindCurrentProcessTaskbarWnd();
            if (!hWnd) continue;
            RunFromWindowThread(hWnd, [](void* param) {
                HWND h = (HWND)param;
                auto xamlRoot = GetTaskbarXamlRoot(h);
                if (!xamlRoot) return;
                auto root = xamlRoot.Content().try_as<FrameworkElement>();
                if (!root) return;
                g_scanDone = true;
                ScanForSpacerTargets(root);
                Wh_Log(L"[Spacer] Scan done, states=%d", (int)g_states.size());
            }, hWnd);
            if (g_scanDone) break;
        }
        return 0;
    }, g_scanStopEvent, 0, nullptr);

    if (!g_scanThread) {
        CloseHandle(g_scanStopEvent);
        g_scanStopEvent = nullptr;
        Wh_Log(L"[Spacer] Failed to create scan thread");
    }
}

// ============================================================
// Uninit
// ============================================================

static void ClearSpacerStates() {
    for (auto& state : g_states) {
        if (auto textBlock = state.originalRef.get()) {
            if (state.textToken)
                textBlock.UnregisterPropertyChangedCallback(
                    TextBlock::TextProperty(), state.textToken);
            textBlock.ClearValue(FrameworkElement::MaxWidthProperty());
            RestoreSourceTextBlock(textBlock);
        }
        // The parent StackPanel is intentionally untouched: this mod no longer
        // sets anything on it, and clearing MaxWidth here would erase Taskbar
        // Clock Customization's fixed clock width.
        RemoveGeneratedPanel(state);
    }
    g_states.clear();
}

// ============================================================
// Windhawk lifecycle
// ============================================================

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Clock Spacer v1.1");
    LoadSettings();

    // GetTaskbarXamlRoot depends on every one of these symbols, and the initial
    // scan depends on GetTaskbarXamlRoot. Continuing without them leaves the mod
    // unable to do its job, so fail loudly here instead of later.
    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"[Init] taskbar.dll symbol hooks failed");
        return FALSE;
    }

    if (HMODULE hSystemTray = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(hSystemTray)) {
            Wh_Log(L"[Init] System tray symbol hooks failed");
            return FALSE;
        }
    } else if (!g_trayUiStartTaskbarHooked) {
        Wh_Log(L"[Init] System tray module not loaded and TrayUI::StartTaskbar "
               L"unavailable — falling back to LoadLibraryExW");
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        auto pLoadLibraryExW = kernelbase
            ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelbase, "LoadLibraryExW"))
            : nullptr;
        if (!pLoadLibraryExW ||
            !WindhawkUtils::SetFunctionHook(pLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original)) {
            Wh_Log(L"[Init] LoadLibraryExW hook unavailable");
            return FALSE;
        }
    } else {
        Wh_Log(L"[Init] System tray module not loaded — waiting for "
               L"TrayUI::StartTaskbar");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    TryHookSystemTrayModule(L"Wh_ModAfterInit");
    Wh_Log(L"[AfterInit] hooked=%d startTaskbarHook=%d",
           (int)g_systemTrayModuleHooked.load(), (int)g_trayUiStartTaskbarHooked);

    // If the taskbar already exists, scan now. Otherwise TrayUI::StartTaskbar
    // (or the LoadLibraryExW fallback plus this call on a later reload) covers it.
    if (g_systemTrayModuleHooked)
        StartInitialScan();
}

void Wh_ModUninit() {
    g_unloading = true;
    Wh_Log(L"[Uninit]");

    if (g_scanStopEvent)
        SetEvent(g_scanStopEvent);
    if (g_scanThread) {
        WaitForThreadWithSentMessagePump(g_scanThread);
        CloseHandle(g_scanThread);
        g_scanThread = nullptr;
    }
    if (g_scanStopEvent) {
        CloseHandle(g_scanStopEvent);
        g_scanStopEvent = nullptr;
    }

    // ClearSpacerStates touches WinRT objects — must run on the UI thread.
    if (HWND hWnd = FindCurrentProcessTaskbarWnd())
        RunFromWindowThread(hWnd, [](void*) { ClearSpacerStates(); }, nullptr);
    else
        ClearSpacerStates();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    g_warnedNoElasticRoom.store(false);
    Wh_Log(L"[Settings] lineWidth=%d maxWidth=%d minSpacerWidth=%d",
           g_settings.lineWidth, g_settings.maxWidth, g_settings.minSpacerWidth);

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"[Settings] No taskbar window found");
        return;
    }

    RunFromWindowThread(hWnd, [](void*) {
        for (auto& state : g_states)
            UpdateSpacerLine(state);
    }, nullptr);
}
