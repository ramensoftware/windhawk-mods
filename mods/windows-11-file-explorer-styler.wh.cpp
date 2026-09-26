// ==WindhawkMod==
// @id              windows-11-file-explorer-styler
// @name            Windows 11 File Explorer Styler
// @description     Customize the File Explorer with themes contributed by others or create your own
// @version         1.7
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ld2d1 -ldwmapi -lgdi32 -lmsimg32 -lole32 -loleaut32 -lruntimeobject -lshlwapi -luxtheme
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Windows 11 File Explorer Styler

Customize the File Explorer with themes contributed by others or create your
own.

Also check out the **Windows 11 Taskbar Styler**, **Windows 11 Start Menu
Styler** and **Windows 11 Notification Center Styler** mods.

## Themes

Themes are collections of styles. The following themes are integrated into the
mod and can be selected in the settings:

[![Translucent
Explorer11](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/Translucent%20Explorer11/screenshot-small.png)
\
Translucent
Explorer11](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/Translucent%20Explorer11/README.md)

[![MicaBar](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/MicaBar/screenshot-small.png)
\
MicaBar](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/MicaBar/README.md)

[![NoCommandBar](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/NoCommandBar/screenshot-small.png)
\
NoCommandBar](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/NoCommandBar/README.md)

[![Minimal
Explorer11](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/Minimal%20Explorer11/screenshot-small.png)
\
Minimal
Explorer11](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/Minimal%20Explorer11/README.md)

[![Tabless](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/Tabless/screenshot-small.png)
\
Tabless](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/Tabless/README.md)

[![Matter](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/Matter/screenshot-small.png)
\
Matter](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/Matter/README.md)

[![WindowGlass](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/WindowGlass/screenshot-small.png)
\
WindowGlass](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/WindowGlass/README.md)

[![AddressSearchOnly](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/AddressSearchOnly/screenshot-small.png)
\
AddressSearchOnly](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/AddressSearchOnly/README.md)

[![TintedGlass](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/TintedGlass/screenshot-small.png)
\
TintedGlass](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/TintedGlass/README.md)

[![LiquidGlass](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/LiquidGlass/screenshot-small.png)
\
LiquidGlass](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/LiquidGlass/README.md)

[![MicaTabless](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/MicaTabless/screenshot-small.png)
\
MicaTabless](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/MicaTabless/README.md)

[![OS26 Liquid
Glass](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/OS26%20Liquid%20Glass/screenshot-small.png)
\
OS26 Liquid
Glass](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/OS26%20Liquid%20Glass/README.md)

[![ZEUSosX_044](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/ZEUSosX_044/screenshot-small.png)
\
ZEUSosX_044](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/ZEUSosX_044/README.md)

[![Compact
Explorer11](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/Compact%20Explorer11/screenshot-small.png)
\
Compact
Explorer11](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/Compact%20Explorer11/README.md)

[![Float](https://raw.githubusercontent.com/ramensoftware/windows-11-file-explorer-styling-guide/main/Themes/Float/screenshot-small.png)
\
Float](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/Themes/Float/README.md)

More themes can be found in the **Themes** section of [The Windows 11 file
explorer styling
guide](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/README.md#themes).
Contributions of new themes are welcome!

## Advanced styling

Aside from themes, the settings have two sections: control styles and resource
variables. Control styles allow to override styles, such as size and color, for
the target elements. Resource variables allow to override predefined variables.
For a more detailed explanation and examples, refer to the sections below.

The [UWPSpy](https://ramensoftware.com/uwpspy) tool can be used to inspect the
file explorer control elements in real time, and experiment with various styles.
To target file explorer with UWPSpy, select the `explorer.exe` process and the
WinUI 3 target framework.

For a collection of commonly requested file explorer styling customizations,
check out [The Windows 11 file explorer styling
guide](https://github.com/ramensoftware/windows-11-file-explorer-styling-guide/blob/main/README.md).

### Control styles

Each entry has a target control and a list of styles.

The target control is written as `Class` or `Class#Name`, i.e. the target
control class name (the tag name in XAML resource files), such as
`FileExplorerExtensions.NavigationBarControl` or `Rectangle`, optionally
followed by `#` and the target control's name (`x:Name` attribute in XAML
resource files). The target control can also include:
* Child control index, for example: `Class#Name[2]` will only match the relevant
  control that's also the second child among all of its parent's child controls.
* Control properties, for example:
  `Class#Name[Property1=Value1][Property2=Value2]`.
* Parent controls, separated by `>`, for example: `ParentClass#ParentName >
  Class#Name`.
* `*` between two `>` separators to match any number of intermediate parent
  controls, for example: `ParentClass > * > Class#Name` matches `Class#Name`
  when `ParentClass` is any of its ancestors (with zero or more controls in
  between). `*` must be in the middle of the target (not the leftmost or
  rightmost part), and consecutive `*` are not allowed.
* `:root` as the leftmost target part to require that the next part has no
  parent, i.e., it's a root element. For example: `:root > Class#Name` matches
  `Class#Name` only when it has no parent control. `:root` must be followed by a
  non-`*` target part.
* Visual state group name, for example: `Class#Name@VisualStateGroupName`. It
  can be specified for the target control or for a parent control, but can be
  specified only once per target. The visual state group can be used in styles
  as specified below.

Several target controls can be specified for the same styles by separating them
with commas, for example: `ParentClass > Class#Name1, ParentClass >
Class#Name2`. Commas inside `[...]` are part of the property value and don't
separate targets, for example: `Class[Margin=0,0,0,1]` is a single target.

**Note**: The target is evaluated only once. If, for example, the index or the
properties of a control change, the target conditions aren't evaluated again.

Each style is written as `Style=Value`, for example: `Height=5`. The `:=` syntax
can be used to use XAML syntax, for example: `Fill:=<SolidColorBrush
Color="Red"/>`. Specifying an empty value with the XAML syntax will clear the
property value, for example: `Fill:=`. In addition, a visual state can be
specified as following: `Style@VisualState=Value`, in which case the style will
only apply when the visual state group specified in the target matches the
specified visual state.

For the XAML syntax, in addition to the built-in taskbar objects, the mod
provides a built-in blur brush via the `WindhawkBlur` object, which supports the
`BlurAmount`, `TintColor`, `TintOpacity`, `TintLuminosityOpacity`,
`TintSaturation`, `NoiseOpacity`, `NoiseDensity`, and `FallbackColor`
properties. For example: `Fill:=<WindhawkBlur BlurAmount="10"
TintColor="#80FF00FF"/>`. Theme resources are also supported for `TintColor` and
`FallbackColor`, for example: `Fill:=<WindhawkBlur BlurAmount="18"
TintColor="{ThemeResource SystemAccentColorDark1}" TintOpacity="0.5"/>`. The
`FallbackColor` is used in place of the blur effect when battery saver is on or
when transparency effects are disabled in the system settings.

Targets and styles starting with two slashes (`//`) are ignored. This can be
useful for temporarily disabling a target or style.

#### Style variables

Beyond literal values, XAML values, and style constants, styles can reference
live property values via *style variables*. A capture rule of the form
`Property=>VarName` observes a control's property and publishes its value as
`VarName`; other styles then substitute it with `{{VarName}}`. Whenever the
captured value changes or the variable becomes undefined, every dependent style
is recomputed and reapplied.

Capture rules cannot be combined with `:=` or with the per-rule `@VisualState`
qualifier.

For example, these two styles on the same target keep it square, with the height
tracking the width:

```
ActualWidth=>width1
Height={{width1}}
```

A capture rule may match several controls at once. A style reading `{{VarName}}`
gets the value from whichever capturing control is closest to it in the control
tree, i.e. the one it shares the deepest common parent with; between equally
close controls, the one that appeared last wins. The variable only becomes
undefined once the last capturing control is gone.

A substitution can appear anywhere in a value, including alongside literal text:

```
Margin=0,{{x1}},0,{{x2 + 10}}
```

Inside `{{ ... }}`, the supported expression syntax is:

* Numbers (e.g. `42`, `3.14`).
* Backtick-delimited string literals (e.g. `` `Auto` ``, `` `*` ``), where a
  doubled backtick encodes one literal backtick. Backtick is used rather than a
  quote so literals don't clash with YAML or XAML quoting.
* Variable references (a previously captured `VarName`).
* Arithmetic `+`, `-`, `*`, `/` with standard precedence, and unary `+`, `-`.
* Comparisons `<`, `<=`, `==`, `>=`, `>`, `!=`, evaluating to `1` or `0`. The
  relational operators require numbers; `==` and `!=` compare two numbers or two
  strings by value and treat a number-versus-string mismatch as unequal.
* The conditional `cond ? a : b`: `a` when `cond` is non-zero, otherwise `b`.
  The condition must be numeric, but each branch may be a number or a string,
  e.g. `` {{width > 0 ? `*` : `Auto`}} `` selects a `GridLength` keyword.
* `min(a, b)` and `max(a, b)`.
* `skip()`: leaves the style unapplied, so the property keeps (or returns to)
  its original value, e.g. `{{width > 0 ? width : skip()}}` applies only once
  `width` is positive.
* Parentheses for grouping, and nesting such as `{{min(a, b + 1) * 2}}`.

Arithmetic, the unary sign, the relational comparisons, and `min` / `max`
require numeric operands. A string can only be produced by a literal or a
string-typed variable, compared with `==` / `!=`, and selected by the
conditional.

Brace pairs match innermost-first, so `{{{x}}}` is parsed as a literal `{`, the
substitution `{{x}}`, and a literal `}`, producing `{<value-of-x>}`.

A bare substitution `{{VarName}}` (with no operators) inserts the variable's
captured value verbatim. This works only for primitive captured types (numeric,
boolean, string); other types (brushes, thicknesses, etc.) are unsupported, and
substituting one skips the style, as does a bare reference to an undefined
variable.

Inside an expression, an undefined variable instead evaluates to the empty
string, letting a style supply its own default via the conditional, e.g. ```
{{width == `` ? 80 : width}} ``` yields `80` until `width` is captured. The
numeric operators above then fail on such a variable, skipping the style rather
than treating it as `0`.

### Resource variables

Some variables, such as size and padding for various controls, colors, and
brushes, are defined as resource variables. You can override existing resources
or define new theme-aware resources.

#### Overriding existing resources

Use `key=value` to override an existing resource.

#### Defining theme-aware resources

Use `Key@Dark=value` and `Key@Light=value` to define new resources with
different values for dark and light themes. These can then be referenced in
styles using `{ThemeResource key}`.

For example, to define a custom accent color that automatically adjusts based on
the system theme:

```
AutoAccent@Dark={ThemeResource SystemAccentColorDark1}
AutoAccent@Light={ThemeResource SystemAccentColorLight2}
```

Then use it in a style:

```
Background:=<SolidColorBrush Color="{ThemeResource AutoAccent}" />
```

The value will automatically update when the system accent color changes.

#### Using XAML syntax

The `:=` syntax can be used to set a XAML value as a resource, for example:
`MyBrush:=<SolidColorBrush Color="Red"/>`. This can be combined with theme
variants: `MyBrush@Dark:=<SolidColorBrush Color="#FF202020"/>`. Specifying an
empty value with the XAML syntax will clear the resource value, for example:
`MyBrush:=`.

### Style constants

Style constants allow defining a value once and referencing it in multiple
styles. Each entry contains a name and value, separated by `=`, for example:

```
mainColor=#fafad2
```

The constant can then be used in style definitions by prepending `$`, for
example:

```
Fill=$mainColor
Background:=<AcrylicBrush TintColor="$mainColor" TintOpacity="0.3" />
```

Some themes use style constants to allow easy customization. Refer to the theme
page for details on which constants are available.

## XAML diagnostics consumer handling

This mod uses XAML diagnostics to inspect and customize the File Explorer.
However, there can only be one XAML diagnostics consumer at a time. If another
program (such as ExplorerBlurMica) tries to use XAML diagnostics while this mod
is running, there will be a conflict.

The **XAML diagnostics consumer handling** setting controls how this mod handles
such conflicts:

* **Alert**: When another program tries to use XAML diagnostics, a message box
  will appear asking whether to block it. This allows you to decide on a
  case-by-case basis.
* **Block**: Automatically block other programs from using XAML diagnostics.
  This ensures this mod works correctly, but might break the other program.
* **Allow**: Allow other programs to use XAML diagnostics. This might break this
  mod's styling, but allows the other program to work.

## Implementation notes

The VisualTreeWatcher implementation is based on the
[ExplorerTAP](https://github.com/TranslucentTB/TranslucentTB/tree/develop/ExplorerTAP)
code from the **TranslucentTB** project.

The `WindhawkBlur` brush object implementation is based on
[XamlBlurBrush](https://github.com/TranslucentTB/TranslucentTB/blob/release/ExplorerTAP/XamlBlurBrush.cpp)
from the **TranslucentTB** project.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: ""
  $name: Theme
  $description: >-
    Themes are collections of styles. For details about the themes below, or for
    information about submitting your own theme, refer to the relevant section
    in the mod details.
  $options:
  - "": None
  - Translucent Explorer11: Translucent Explorer11
  - MicaBar: MicaBar
  - NoCommandBar: NoCommandBar
  - Minimal Explorer11: Minimal Explorer11
  - Tabless: Tabless
  - Matter: Matter
  - WindowGlass: WindowGlass
  - AddressSearchOnly: AddressSearchOnly
  - TintedGlass: TintedGlass
  - LiquidGlass: LiquidGlass
  - MicaTabless: MicaTabless
  - OS26 Liquid Glass: OS26 Liquid Glass
  - OS26 Liquid Glass_variant_Compact: OS26 Liquid Glass (Compact)
  - ZEUSosX_044: ZEUSosX_044
  - Compact Explorer11: Compact Explorer11
  - Float: Float
- backgroundTranslucentEffect: ""
  $name: Translucent background effect
  $description: >-
    The translucent effect to use for the File Explorer background. For
    additional translucent effects, check out the Translucent Windows mod.
  $options:
  - "": Default for the selected theme
  - default: Windows default
  - acrylicblur: Blur (AccentBlurBehind)
  - acrylic: Acrylic
  - mica: Mica
  - micaAlt: Mica Alt
  - none: None
- backgroundTranslucentEffectRegion: ""
  $name: Translucent background effect region
  $description: >-
    The region where the translucent background effect is applied.
  $options:
  - "": Entire window
  - explorerFrame: File Explorer frame only
- styleConstants: [""]
  $name: Style constants
  $description: >-
    Some themes support style constants for customization, such as colors. Refer
    to the theme page for available constants. For technical details, refer to
    the mod description.
- controlStyles:
  - - target: ""
      $name: Target
    - styles: [""]
      $name: Styles
  $name: Control styles
- themeResourceVariables: [""]
  $name: Resource variables
  $description: >-
    Use "Key=Value" to override an existing resource with a new value.

    Use "Key@Dark=Value" or "Key@Light=Value" to define theme-aware resources
    that can be referenced with {ThemeResource Key} in styles.

    The ":=" syntax can be used to set a XAML value. For details, refer to the
    mod description.
- explorerFrameContainerHeight: 0
  $name: Explorer frame container height
  $description: >-
    The height of the explorer frame container which includes the tabs, the
    address bar, and the command bar, set to zero to use the default height.
- xamlDiagnosticsHandling: alert
  $name: XAML diagnostics consumer handling
  $description: >-
    How to handle other programs (e.g. ExplorerBlurMica) that try to use XAML
    diagnostics. There can only be one consumer at a time. Block will prevent
    other programs from using it, which might break them. Allow will let them
    use it, which might break this mod.
  $options:
  - alert: Alert (prompt before blocking)
  - block: Block other consumers
  - allow: Allow other consumers
*/
// ==/WindhawkModSettings==

#include <xamlom.h>

#include <atomic>
#include <optional>
#include <vector>

#undef GetCurrentTime

#include <winrt/Microsoft.UI.Xaml.h>

struct ThemeTargetStyles {
    PCWSTR target;
    std::vector<PCWSTR> styles;
};

enum class BackgroundTranslucentEffect {
    kDefault,
    kBlur,
    kAcrylic,
    kMica,
    kMicaAlt,
    kNone,
};

struct Theme {
    std::vector<ThemeTargetStyles> targetStyles;
    std::vector<PCWSTR> styleConstants;
    std::vector<PCWSTR> themeResourceVariables;
    int explorerFrameContainerHeight = 0;
    BackgroundTranslucentEffect backgroundTranslucentEffect =
        BackgroundTranslucentEffect::kDefault;
};

// clang-format off

const Theme g_themeTranslucent_Explorer11 = {{
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background=Transparent",
        L"BorderThickness=0,0,0,1",
        L"BorderBrush=#40A0A0A0"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas > Microsoft.UI.Xaml.Shapes.Path#SelectedBackgroundPath", {
        L"Fill=#40404040"}},
    ThemeTargetStyles{L"Grid#HomeViewRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Background:=<AcrylicBrush TintColor=\"#121212\" Opacity=\"0.3\"/>"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background=Transparent"}},
}, {}, {}, /*explorerFrameContainerHeight=*/0, BackgroundTranslucentEffect::kAcrylic};

const Theme g_themeMicaBar = {{
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource LayerOnMicaBaseAltFillColorDefault}\"/>",
        L"BorderThickness=0,0,0,1"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent"}},
}};

const Theme g_themeNoCommandBar = {{
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1, FileExplorerExtensions.CommandBarControl", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.RowSpan=2",
        L"Margin=0,0,0,1"}},
}, {}, {}, /*explorerFrameContainerHeight=*/87};

const Theme g_themeMinimal_Explorer11 = {{
    ThemeTargetStyles{L"AppBarButton#backButton > Grid#Root@CommonStates > Border#AppBarButtonInnerBorder", {
        L"Background@Normal:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.07\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.12\"/>",
        L"Background@Pressed:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.12\"/>",
        L"Background@Disabled:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.05\"/>"}},
    ThemeTargetStyles{L"AppBarButton#forwardButton > Grid#Root@CommonStates > Border#AppBarButtonInnerBorder", {
        L"Background@Normal:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.05\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.12\"/>",
        L"Background@Pressed:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.12\"/>",
        L"Background@Disabled:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.05\"/>"}},
    ThemeTargetStyles{L"AppBarButton#refreshButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#upButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#BottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1, FileExplorerExtensions.CommandBarControl", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"FileExplorerExtensions.AddressBarControl > Grid#PART_LayoutRoot > Grid#NormalModeGrid", {
        L"BorderThickness=0,0,0,1",
        L"BorderBrush=#A0A0A0"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Margin=0,0,3,0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"CornerRadius=4",
        L"Margin=0,-3,0,3",
        L"Height=28"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.35\"/>",
        L"Background@PointerOverSelected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.35\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.13\"/>",
        L"Background@Normal:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.05\"/>",
        L"Background@PressedSelected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.35\"/>"}},
    ThemeTargetStyles{L"Grid#FileExplorerAddressBarGrid", {
        L"Grid.ColumnSpan=2",
        L"Margin=0,0,10,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#backButton > Grid#Root", {
        L"Padding=2"}},
    ThemeTargetStyles{L"AppBarButton#forwardButton > Grid#Root", {
        L"Padding=2"}},
    ThemeTargetStyles{L"AppBarButton#forwardButton > Grid#Root > Grid#ContentRoot > Viewbox#ContentViewbox", {
        L"Margin=9"}},
    ThemeTargetStyles{L"AppBarButton#backButton > Grid#Root > Grid#ContentRoot > Viewbox#ContentViewbox", {
        L"Margin=9"}},
    ThemeTargetStyles{L"Grid#PART_LayoutRoot", {
        L"MinHeight=28",
        L"Height=28"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Margin=0,0,20,4"}},
    ThemeTargetStyles{L"Border#ScrollIncreaseButtonContainer", {
        L"Margin=0,0,0,4"}},
    ThemeTargetStyles{L"Border#ScrollDecreaseButtonContainer", {
        L"Margin=0,0,0,4"}},
    ThemeTargetStyles{L"Grid#FileExplorerAddressBarGrid", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl#NavigationBarControl", {
        L"Grid.Row=0",
        L"Grid.RowSpan=2"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FileExplorerTabControl", {
        L"Margin=100,0,0,-15",
        L"Grid.RowSpan=2"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl > Grid#NavigationBarControlGrid", {
        L"Margin=0,0,0,-18",
        L"Background=Transparent",
        L"Width=100",
        L"HorizontalAlignment=0"}},
}, {}, {}, /*explorerFrameContainerHeight=*/42};

const Theme g_themeTabless = {{
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#ContentRoot", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.Row=$NavigationBarGrid"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1, FileExplorerExtensions.CommandBarControl", {
        L"Grid.Row=$CommandBarGrid"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainerGrid > Border", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer > Microsoft.UI.Xaml.Controls.Button#CloseButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TabViewItem > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.Canvas", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ContentPresenter > Microsoft.UI.Xaml.Controls.StackPanel > Microsoft.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe UI, Segoe Fluent Icons",
        L"FontWeight=Normal"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"BorderThickness=0,0,0,1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FileExplorerTabControl", {
        L"Height=36"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"Padding=1,0,0,1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox#IconBox", {
        L"Margin=0,0,4,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TabViewItem", {
        L"Margin=0,-8,0,0"}},
}, {
    L"NavigationBarGrid=2",
    L"CommandBarGrid=1",
}};

const Theme g_themeMatter = {{
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent",
        L"HorizontalAlignment  = 1"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerSecondaryCommandBar", {
        L"Background=Transparent",
        L"Visibility = 1"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Margin=0,0,4,0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"CornerRadius=5",
        L"Margin=2,4,0,4",
        L"Height=29"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background = Transparent",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:= $accentColor2",
        L"Background@PointerOverSelected:= $accentColor",
        L"Background@PointerOver:= $accentColor2",
        L"Background@Normal=$accentColor",
        L"Background@PressedSelected:=$accentColor2",
        L"Background@Pressed := $accentColor2"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility  = 0",
        L"Margin = 0,0,0,3"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background=Transparent",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#PART_LayoutRoot", {
        L"Background :=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"0.4\" />",
        L"CornerRadius = 6",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1, FileExplorerExtensions.CommandBarControl", {
        L"Margin = 0,-5,0,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox > Grid#LayoutRoot > TextBox > Grid@CommonStates > Border#BorderElement", {
        L"Background :=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"0.4\" />",
        L"CornerRadius = 6",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Cut]", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Copy]", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Paste]", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Rename]", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Share]", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarSeparator", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#ScrollDecreaseButtonContainer", {
        L"Margin = 0,0,0,3"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#ScrollIncreaseButtonContainer", {
        L"Margin = 0,0,0,3"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton", {
        L"Visibility  =1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#upButton", {
        L"Visibility  =1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton", {
        L"Visibility  =1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton", {
        L"Visibility  =1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Create a new item in the current location.]", {
        L"Visibility  = 1"}},
}, {
    L"accentColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />",
    L"accentColor2=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"0.5\" />",
}};

const Theme g_themeWindowGlass = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#PART_LayoutRoot", {
        L"Background=Transparent",
        L"RenderTransform:=<TranslateTransform X=\"0\"/>"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FirstCrumbStackPanelControl#FirstCrumbStackPanel", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootCommandSearchGrid > Windows.UI.Xaml.Controls.Border#BorderElement", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot", {
        L"BorderThickness=$BorderThickness",
        L"Background:=$ButtonBackground",
        L"BorderBrush:=$ButtonBorder"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerCommandBar", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />",
        L"HorizontalAlignment=Center",
        L"Margin=-4",
        L"Padding=10"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerSecondaryCommandBar", {
        L"RenderTransform:=<TranslateTransform X=\"Auto\" />",
        L"HorizontalAlignment=Center",
        L"Margin=-4",
        L"Padding=10",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerCommandBar > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.Grid#ContentRoot", {
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush=Transparent",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerSecondaryCommandBar > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.Grid#ContentRoot", {
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background=#10808080",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#NavigationBarControlGrid", {
        L"Background=Transparent",
        L"BorderBrush=Transparent",
        L"ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"*\"/><ColumnDefinition Width=\"430\"/></ColumnDefinitionCollection>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#HomeViewRootGrid", {
        L"BorderBrush:=$MainContentBG",
        L"CornerRadius=8",
        L"BorderThickness=0",
        L"Margin=0,0,8,8",
        L"Background:=$MainContentBG"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"BorderBrush:=$MainContentBG",
        L"CornerRadius=8",
        L"BorderThickness=0",
        L"Margin=0,0,8,8",
        L"Background:=$MainContentBG"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid > Grid#GalleryRootGrid", {
        L"Background:=$MainContentBG"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Background:=$Background"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"CornerRadius=8",
        L"Margin=5",
        L"Height=35"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.10\"/>",
        L"Background@PointerOverSelected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.10\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.13\"/>",
        L"Background@Normal:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.05\"/>",
        L"Background@PressedSelected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.10\"/>"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#BottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Shapes.Path#LeftRadiusRenderArc", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Shapes.Path#RightRadiusRenderArc", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox#IconBox", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.CommandBarFlyoutCommandBar > Grid#LayoutRoot > Grid#OuterContentRoot > Grid#ContentRoot > Grid#PrimaryItemsRoot", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Margin=0,0,0,-5",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Grid#OuterOverflowContentRootV2 > Grid#OverflowContentRoot > CommandBarOverflowPresenter#SecondaryItemsControl > Grid#LayoutRoot", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"CommandBarOverflowPresenter#SecondaryItemsControl > Grid#LayoutRoot", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AutoSuggestBox#FileExplorerSearchBox > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.TextBox#TextBox", {
        L"CornerRadius=$CornerRadius",
        L"Margin=0,0,180,0",
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"MaxWidth=750",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AutoSuggestBox#PART_AutoSuggestBox > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.TextBox#TextBox", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#NavigationCommands", {
        L"Margin=180,0,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#RootContainer", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border > Microsoft.UI.Xaml.Controls.Button#AddButton", {
        L"RenderTransform:=<TranslateTransform Y=\"-6\" />"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TextBlock#TextLabel", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#SubItemChevronPanel > Microsoft.UI.Xaml.Controls.FontIcon#SubItemChevron", {
        L"RenderTransform:=<TranslateTransform X=\"-5\" Y=\"12\" />"}},
}, {
    L"Background=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#15323232\"/>",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"{ThemeResource SystemChromeHighColor}\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SystemChromeLowColor}\" Offset=\"0.15\" /><GradientStop Color=\"{ThemeResource SystemChromeHighColor}\" Offset=\"0.95\" /></LinearGradientBrush>",
    L"BorderThickness=0.3,1,0.3,0.3",
    L"ButtonBackground=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" Opacity=\"1\" />",
    L"ButtonBorder=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\" Opacity=\"1\" />",
    L"CornerRadius=8",
    L"Background2=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0\" />",
    L"MainContentBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"1\" />",
}, {}, /*explorerFrameContainerHeight=*/0, BackgroundTranslucentEffect::kAcrylic};

const Theme g_themeAddressSearchOnly = {{
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.Row=0",
        L"Background=Transparent",
        L"MinHeight=48",
        L"Margin=0,26,0,1"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FileExplorerTabControl", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#refreshButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#upButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#backButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#forwardButton", {
        L"Visibility=Collapsed"}},
}, {}, {}, /*explorerFrameContainerHeight=*/80};

const Theme g_themeTintedGlass = {{
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"BorderBrush=$CommonBgBrush"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas > Microsoft.UI.Xaml.Shapes.Path#SelectedBackgroundPath", {
        L"Fill:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Grid#HomeViewRootGrid", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background:=$CommonBgBrush"}},
}, {
    L"CommonBgBrush=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#80000000\"/>",
}, {}, /*explorerFrameContainerHeight=*/0, BackgroundTranslucentEffect::kAcrylic};

const Theme g_themeLiquidGlass = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#PART_LayoutRoot", {
        L"Background=Transparent",
        L"HorizontalAlignment=Stretch"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FirstCrumbStackPanelControl#FirstCrumbStackPanel", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootCommandSearchGrid > Windows.UI.Xaml.Controls.Border#BorderElement", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot", {
        L"BorderThickness=$ElementBorderThickness",
        L"Background:=$ElementBackground",
        L"BorderBrush:=$ElementBorder",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#NavigationBarControlGrid", {
        L"Background:=Transparent",
        L"BorderBrush:=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#HomeViewRootGrid", {
        L"BorderBrush:=$ElementBorderBrush",
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"Margin=4,0"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"BorderBrush:=$ElementBorderBrush",
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"Margin=4,0"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid > Grid#GalleryRootGrid", {
        L"Background:=Transparent"}},
    ThemeTargetStyles{L"ToolTip", {
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"Margin=5",
        L"Height=35",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius",
        L"BorderBrush:=$ElementBorderBrush"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=$ElementBackground",
        L"Background@PointerOverSelected:=$AccentBackground",
        L"Background@PointerOver:=$AccentBackground",
        L"Background@Normal:=$ElementBackground",
        L"Background@PressedSelected:=$ButtonBackground2"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#BottomBorderLine", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Shapes.Path#LeftRadiusRenderArc", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Shapes.Path#RightRadiusRenderArc", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"Visibility=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox#IconBox", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"CommandBarOverflowPresenter#SecondaryItemsControl > Grid#LayoutRoot", {
        L"Background:=$ElementBackground",
        L"BorderThickness=$ElementBorderThickness",
        L"BorderBrush:=$ElementBorderBrush",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AutoSuggestBox#FileExplorerSearchBox > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.TextBox#TextBox", {
        L"CornerRadius=$ElementCornerRadius",
        L"Background:=$ElementBackground",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"CornerRadius=$ElementCornerRadius",
        L"Background:=$ElementBackground",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AutoSuggestBox#PART_AutoSuggestBox > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.TextBox#TextBox", {
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#RootContainer", {
        L"Background:=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border > Microsoft.UI.Xaml.Controls.Button#AddButton", {
        L"RenderTransform:=<TranslateTransform Y=\"-8\" />"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TextBlock#TextLabel", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#SubItemChevronPanel > Microsoft.UI.Xaml.Controls.FontIcon#SubItemChevron", {
        L"RenderTransform:=<TranslateTransform X=\"-5\" Y=\"12\" />"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"Height = 28"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1, FileExplorerExtensions.CommandBarControl", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.RowSpan=2",
        L"Margin=0,0,0,1"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Background:=Transparent"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail", {
        L"Background:=Transparent"}},
}, {
    L"ContentBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"1\" />",
    L"Background=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\" />",
    L"ElementBackground=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.4\" />",
    L"ElementBackground2=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\" />",
    L"AccentBackground=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAccentColorLight1}\" TintOpacity=\"0.2\" />",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"BorderThickness=0.3,1,0.3,0.3",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"CornerRadius=12",
    L"ElementCornerRadius=8",
}, {}, /*explorerFrameContainerHeight=*/87};

const Theme g_themeMicaTabless = {{
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#ContentRoot", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.Row=$NavigationBarGrid"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1, FileExplorerExtensions.CommandBarControl", {
        L"Grid.Row=$CommandBarGrid"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainerGrid > Border", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer > Microsoft.UI.Xaml.Controls.Button#CloseButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TabViewItem > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.Canvas", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ContentPresenter > Microsoft.UI.Xaml.Controls.StackPanel > Microsoft.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe UI, Segoe Fluent Icons",
        L"FontWeight=Normal"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"BorderThickness=0,0,0,1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FileExplorerTabControl", {
        L"Height=36"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"Padding=1,0,0,1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox#IconBox", {
        L"Margin=0,0,4,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TabViewItem", {
        L"Margin=0,-8,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#DetailsViewControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarSeparator", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Background:="}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Microsoft.UI.Xaml.Controls.Grid", {
        L"Background:="}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#HomeViewRootGrid", {
        L"Background:="}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#DetailsViewControlRootGrid", {
        L"Background:="}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.StackPanel#DetailsViewThumbnail > Microsoft.UI.Xaml.Controls.Grid", {
        L"Background:="}},
}, {
    L"NavigationBarGrid=1",
    L"CommandBarGrid=2",
}};

const Theme g_themeOS26_Liquid_Glass = {{
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Margin=20,20,20,1",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#HomeViewRootGrid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Margin=10",
        L"Background:=transparent",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=12",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=2,6,2,6",
        L"Padding@Disabled=0,-7",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#upButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerSecondaryCommandBar", {
        L"Background=Transparent",
        L"MinHeight=0"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Margin=0,0,8,0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"CornerRadius=12",
        L"Margin=2,4,0,4",
        L"Height=27",
        L"BorderThickness=1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\" />",
        L"Background@PointerOverSelected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#35ffffff\" />",
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#15ffffff\" />"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility=Visible",
        L"Margin=0,0,0,2",
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"BorderBrush:=<LinearGradientBrush EndPoint=\"1,1\" StartPoint=\"0,0\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\"/><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\"/><GradientStop Color=\"#80ffffff\" Offset=\"1\"/></LinearGradientBrush>",
        L"Width=24",
        L"Height=24"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"Margin=-6,0,0,0"}},
    ThemeTargetStyles{L"Grid#PART_LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\" />",
        L"CornerRadius=14",
        L"BorderThickness=1",
        L"Margin=2",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1, FileExplorerExtensions.CommandBarControl", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox > Grid#LayoutRoot > TextBox > Grid@CommonStates > Border#BorderElement", {
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\" />",
        L"CornerRadius=14",
        L"BorderThickness=1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\" />",
        L"CornerRadius=12",
        L"BorderThickness=1",
        L"Margin=3,0,3,1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarToggleButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\" />",
        L"CornerRadius=12",
        L"BorderThickness=1",
        L"Margin=3,0,3,1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#OuterOverflowContentRootV2", {
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Button#MoreButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarSeparator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton", {
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton", {
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#upButton", {
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton", {
        L"Margin=0,9,9,0"}},
}};

const Theme g_themeOS26_Liquid_Glass_variant_Compact = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.SuggestionsPopup", {
        L"Margin=0,0,0,900"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton > Grid@CommonStates", {
        L"Background@Disabled:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius@Disabled=12",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=2,6,2,6",
        L"Padding@Disabled=0,-7"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > Button#MoreButton", {
        L"Background:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=12",
        L"BorderThickness=1",
        L"Margin=3,2,3,2",
        L"Width=45",
        L"Height=32"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton > Grid@CommonStates", {
        L"Background@Disabled:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton > Grid@CommonStates", {
        L"Background@Disabled:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton > Grid@CommonStates", {
        L"Background@Disabled:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarToggleButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#2D101010\"/>"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Margin=20,20,20,1",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid > OuterOverflowContentRootV2", {
        L"CornerRadius=250"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBarOverflowPresenter > Microsoft.UI.Xaml.Controls.CommandBarOverflowPresenter", {
        L"Background=transparent"}},
    ThemeTargetStyles{L"AppBarButton[7]", {
        L"Visibility=Collapsed",
        L"Width=0",
        L"MinWidth=0",
        L"Margin=0,0,0,0",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox > ContentViewB", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#HomeViewRootGrid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Margin=10",
        L"Background:=transparent",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Grid.Row=0",
        L"Grid.RowSpan=1",
        L"CornerRadius:=15",
        L"Width=400",
        L"HorizontalAlignment=Left",
        L"Background:=transparent",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > Grid#OverflowSeparator", {
        L"Visibility=Collapsed",
        L"Width=0",
        L"MinWidth=0"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot", {
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > ItemsControl#PrimaryItemsControl", {
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerSecondaryCommandBar", {
        L"Visibility=Visible",
        L"Margin=0,40,0,-20"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid", {
        L"Margin=370,1,0,1"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Width=150",
        L"Height=40",
        L"Margin=0,0,8,0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=2,2,0,2",
        L"Height=35"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#20ffffff\"/>",
        L"Background@PointerOverSelected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\"/>",
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#15ffffff\"/>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility=Visible",
        L"Margin=0,0,0,4",
        L"Background:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=8",
        L"BorderThickness=1",
        L"BorderBrush:=<LinearGradientBrush EndPoint=\"1,1\" StartPoint=\"0,0\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\"/><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\"/><GradientStop Color=\"#80ffffff\" Offset=\"1\"/></LinearGradientBrush>",
        L"Width=24",
        L"Height=24"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background:=",
        L"BorderBrush:="}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#PART_LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#15ffffff\"/>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=2"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1, FileExplorerExtensions.CommandBarControl", {
        L"Grid.Row=0",
        L"Grid.RowSpan=2",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox > Grid#LayoutRoot > TextBox > Grid@CommonStates", {
        L"BorderThickness=1",
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#15ffffff\"/>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=10",
        L"Margin=-90,0,90,0",
        L"Height=32"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"Margin=-8,0,90,0"}},
    ThemeTargetStyles{L"CommandBarOverflowPresenter Microsoft.UI.Xaml.Controls.AppBarButton", {
        L"Background=Transparent",
        L"CornerRadius=8",
        L"Margin=2,1,2,1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton", {
        L"Background:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=12",
        L"BorderThickness=1",
        L"Margin=3,2,3,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#OuterOverflowContentRootV2", {
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarToggleButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#2D101010\"/>",
        L"CornerRadius=8",
        L"BorderThickness=1",
        L"Margin=3,0,3,1",
        L"BorderBrush:=<LinearGradientBrush EndPoint=\"1,1\" StartPoint=\"0,0\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\"/><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\"/><GradientStop Color=\"#80ffffff\" Offset=\"1\"/></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarSeparator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#upButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton", {
        L"Visibility=Visible",
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#stopButton", {
        L"Visibility=Collapsed",
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.RowSpan=2"}},
}, {}, {}, /*explorerFrameContainerHeight=*/87};

const Theme g_themeZEUSosX_044 = {{
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background=Transparent",
        L"BorderThickness=0",
        L"Grid.Row=0",
        L"Grid.RowSpan=2",
        L"HorizontalAlignment=Left",
        L"VerticalAlignment=Top",
        L"Width=155",
        L"Margin=197,-30,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent",
        L"HorizontalAlignment=Left",
        L"VerticalAlignment=Top"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#BottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#NavigationBarControlGrid", {
        L"Background=Transparent",
        L"BorderBrush=Transparent",
        L"ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"*\"/><ColumnDefinition Width=\"380\"/></ColumnDefinitionCollection>",
        L"Margin=0,-16,0,-21"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid", {
        L"Grid.Row=0",
        L"HorizontalAlignment=Left",
        L"Margin=100,0,0,0",
        L"Width=1",
        L"MaxWidth=1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FileExplorerTabControl", {
        L"HorizontalAlignment=Left",
        L"Margin=100,0,0,0",
        L"Width=1",
        L"MaxWidth=1"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Width=0",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox > Grid#LayoutRoot > TextBox > Grid@CommonStates > Border#BorderElement", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid > Grid#LayoutRoot > TextBox > Grid@CommonStates > Border#BorderElement", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AutoSuggestBox#FileExplorerSearchBox > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.TextBox#TextBox", {
        L"Margin=0,0,140,0",
        L"Background=Transparent",
        L"BorderBrush=Transparent",
        L"TextAlignment=Center",
        L"HorizontalContentAlignment=Center"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"HorizontalAlignment=Stretch",
        L"Height=28",
        L"Margin=155,0,0,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox", {
        L"HorizontalAlignment=Stretch",
        L"Height=28",
        L"Margin=-7,-1,7,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerCommandBar Button", {
        L"FontSize=14"}},
}, {}, {}, /*explorerFrameContainerHeight=*/44, BackgroundTranslucentEffect::kMica};

const Theme g_themeCompact_Explorer11 = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.SuggestionsPopup", {
        L"Margin=0,0,0,900"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=10",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=2,6,2,6",
        L"Padding@Disabled=0,-7"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > Button#MoreButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=3,2,3,2",
        L"Width=45",
        L"Height=32"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarToggleButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#2D101010\"/>"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Margin=20,20,20,1",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid > OuterOverflowContentRootV2", {
        L"CornerRadius=250"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBarOverflowPresenter > Microsoft.UI.Xaml.Controls.CommandBarOverflowPresenter", {
        L"Background=transparent"}},
    ThemeTargetStyles{L"AppBarButton[7]", {
        L"Visibility=Collapsed",
        L"Width=0",
        L"MinWidth=0",
        L"Margin=0,0,0,0",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox > ContentViewB", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#HomeViewRootGrid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Margin=10",
        L"Background:=transparent",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Grid.Row=0",
        L"Grid.RowSpan=1",
        L"CornerRadius:=15",
        L"Width=400",
        L"HorizontalAlignment=Left",
        L"Background:=transparent",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > Grid#OverflowSeparator", {
        L"Visibility=Collapsed",
        L"Width=0",
        L"MinWidth=0"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot", {
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > ItemsControl#PrimaryItemsControl", {
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerSecondaryCommandBar", {
        L"Visibility=Visible",
        L"Margin=0,40,0,-20"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid", {
        L"Margin=370,1,0,1"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Width=150",
        L"Height=40",
        L"Margin=0,0,8,0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=2,2,0,2",
        L"Height=35"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#30ffffff\"/>",
        L"Background@PointerOverSelected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#40ffffff\"/>",
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#20ffffff\"/>"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility=Visible",
        L"Margin=0,0,0,4",
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=0",
        L"BorderBrush:=<LinearGradientBrush EndPoint=\"1,1\" StartPoint=\"0,0\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\"/><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\"/><GradientStop Color=\"#80ffffff\" Offset=\"1\"/></LinearGradientBrush>",
        L"Width=24",
        L"Height=24"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Background:=",
        L"BorderBrush:="}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#PART_LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1, FileExplorerExtensions.CommandBarControl", {
        L"Grid.Row=0",
        L"Grid.RowSpan=2",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox > Grid#LayoutRoot > TextBox > Grid@CommonStates", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"Margin=-90,0,90,0",
        L"Height=30"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"Margin=-8,0,90,0"}},
    ThemeTargetStyles{L"CommandBarOverflowPresenter#SecondaryItemsControl > Microsoft.UI.Xaml.Controls.AppBarButton", {
        L"Background=Transparent",
        L"CornerRadius=4",
        L"BorderThickness=0",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=3,2,3,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarToggleButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#2D101010\"/>",
        L"CornerRadius=8",
        L"BorderThickness=1",
        L"Margin=3,0,3,1",
        L"BorderBrush:=<LinearGradientBrush EndPoint=\"1,1\" StartPoint=\"0,0\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\"/><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\"/><GradientStop Color=\"#80ffffff\" Offset=\"1\"/></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarSeparator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#upButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton", {
        L"Visibility=Visible",
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#stopButton", {
        L"Visibility=Collapsed",
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.RowSpan=2"}},
}, {}, {}, /*explorerFrameContainerHeight=*/87};

const Theme g_themeFloat = {{
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<AcrylicBrush TintColor=\"{ThemeResource Tab}\" TintOpacity=\"0.9\" Opacity=\"0.6\"/>",
        L"Background@PointerOverSelected:=<AcrylicBrush TintColor=\"{ThemeResource Tab}\" TintOpacity=\"0.9\" Opacity=\"0.7\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource Tab}\" TintOpacity=\"0.9\" Opacity=\"0.3\"/>",
        L"Background@Normal:=<AcrylicBrush TintColor=\"{ThemeResource Tab}\" TintOpacity=\"0.9\" Opacity=\"0\"/>",
        L"Background@PressedSelected:=<AcrylicBrush TintColor=\"{ThemeResource Tab}\" TintOpacity=\"0.9\" Opacity=\"0.9\"/>",
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"BorderThickness=1",
        L"Margin=2,0,0,0",
        L"Height=35"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#BottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility=Visible",
        L"Margin=0,0,0,3",
        L"CornerRadius=10",
        L"BorderThickness=0",
        L"Width=24",
        L"Height=24"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainerGrid", {
        L"Height=44"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#NavigationBarControlGrid", {
        L"CornerRadius=6",
        L"Margin=8,4,8,0",
        L"Height=54"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl_Wave1 > Grid, Grid#CommandBarControlRootGrid", {
        L"Margin=0,8,0,0",
        L"BorderThickness=0,1,0,1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.TabViewListView#TabListView", {
        L"Margin=-3,0,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Shapes.Path#RightRadiusRenderArc", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Shapes.Path#LeftRadiusRenderArc", {
        L"Visibility=Collapsed"}},
}, {}, {
    L"Tab@Light=#ffffffff",
    L"Tab@Dark=#000000",
}, /*explorerFrameContainerHeight=*/160};

// clang-format on

enum class BackgroundTranslucentEffectRegion {
    kEntireWindow,
    kExplorerFrame,
};

enum class XamlDiagnosticsHandling {
    kAlert,
    kBlock,
    kAllow,
};

struct {
    std::optional<BackgroundTranslucentEffect> backgroundTranslucentEffect;
    BackgroundTranslucentEffectRegion backgroundTranslucentEffectRegion;
    int explorerFrameContainerHeight;
    XamlDiagnosticsHandling xamlDiagnosticsHandling;
} g_settings;

BackgroundTranslucentEffect g_themeBackgroundTranslucentEffect;
int g_themeExplorerFrameContainerHeight;

std::atomic<bool> g_initialized;
thread_local bool g_initializedForThread;

// An InstanceHandle is the address of an interface on the element, so it names
// an element only for as long as that element lives: an element allocated over
// a destroyed one is reported under the same handle. Everything the mod records
// is therefore keyed by an id minted per reported element, which is never
// reused, rather than by the handle itself.
enum class ElementId : uint64_t { None = 0 };

ElementId GetOrCreateElementId(
    InstanceHandle handle,
    winrt::Windows::Foundation::IInspectable const& element);
ElementId FindElementId(InstanceHandle handle);
void ForgetElementId(InstanceHandle handle);

void ApplyCustomizations(ElementId elementId,
                         winrt::Microsoft::UI::Xaml::FrameworkElement element,
                         PCWSTR fallbackClassName);
void CleanupCustomizations(ElementId elementId);
void QueueDiagnosticsRelease(InstanceHandle handle);
void FlushDiagnosticsReleasesIfQuiet();

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

////////////////////////////////////////////////////////////////////////////////
// clang-format off

#pragma region winrt_hpp

#include <Unknwn.h>
#include <weakreference.h>
#include <winrt/base.h>

// forward declare namespaces we alias
namespace winrt {
    namespace Windows {
        namespace Foundation {}
    }
    namespace Microsoft {
        namespace UI::Xaml {}
    }
}

// alias some long namespaces for convenience
namespace wf = winrt::Windows::Foundation;
namespace mux = winrt::Microsoft::UI::Xaml;

// A weak reference for the object, or an empty one when the object is null or
// doesn't support weak references: cppwinrt's make_weak dereferences a null
// pointer for an object without that support instead of reporting it. Throws,
// as make_weak does, when the object supports weak references but one can't be
// made.
winrt::weak_ref<wf::IInspectable> TryMakeWeak(wf::IInspectable const& object)
{
    if (!object.try_as<::IWeakReferenceSource>())
    {
        return nullptr;
    }

    return winrt::make_weak(object);
}

#pragma endregion  // winrt_hpp

#pragma region visualtreewatcher_hpp

#include <winrt/Microsoft.UI.Xaml.h>

// XamlDiagnostics implements this interface too, and xamlom.h does not declare
// it. UnregisterInstance closes the runtime object cached for a handle, the
// only reference the diagnostics keep to an element once it was reported.
static constexpr GUID IID_IXamlDiagnosticsTestHooks =
    {0x735941a2, 0x3ee3, 0x495a, {0x8d, 0xa9, 0x97, 0x26, 0x27, 0x00, 0x30, 0x75}};

struct IXamlDiagnosticsTestHooks : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE UnregisterInstance(InstanceHandle handle) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetDispatcherQueueForObject(InstanceHandle handle, void** dispatcherQueue) = 0;
};

// The handle a mutation callback would report for an element, for elements
// which were reached some other way, e.g. by walking the visual tree. Derived
// the way the diagnostics derive it, by querying IInspectable and taking the
// pointer, and not through GetHandleFromIInspectable: that one creates the
// runtime object when none is cached, so asking it about an element whose
// reference was released would take a new reference and pin it again.
InstanceHandle HandleFromInspectable(wf::IInspectable const& instance)
{
    winrt::com_ptr<::IInspectable> inspectable;
    winrt::check_hresult(reinterpret_cast<::IUnknown*>(winrt::get_abi(instance))->QueryInterface(winrt::guid_of<wf::IInspectable>(), inspectable.put_void()));
    return reinterpret_cast<InstanceHandle>(inspectable.get());
}

class VisualTreeWatcher : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile>
{
public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site);

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;

    VisualTreeWatcher(VisualTreeWatcher&&) = delete;
    VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;

    ~VisualTreeWatcher();

    void UnadviseVisualTreeChange();

    bool ReleaseDiagnosticsReference(InstanceHandle handle);

private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation, VisualElement element, VisualMutationType mutationType) override;
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle element, VisualElementState elementState, LPCWSTR context) noexcept override;

    wf::IInspectable FromHandle(InstanceHandle handle)
    {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));
        return obj;
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
    winrt::com_ptr<IXamlDiagnosticsTestHooks> m_XamlDiagnosticsTestHooks = nullptr;
};

#pragma endregion  // visualtreewatcher_hpp

#pragma region visualtreewatcher_cpp

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site) :
    m_XamlDiagnostics(site.as<IXamlDiagnostics>())
{
    Wh_Log(L"Constructing VisualTreeWatcher");

    HRESULT hr = m_XamlDiagnostics->QueryInterface(IID_IXamlDiagnosticsTestHooks, m_XamlDiagnosticsTestHooks.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"IXamlDiagnosticsTestHooks is unavailable, elements will be leaked: %08X", hr);
    }

    // winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(this));

    // Calling AdviseVisualTreeChange from the current thread causes the app to
    // hang in Advising::RunOnUIThread sometimes. Creating a new thread and
    // calling it from there fixes it.
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            auto watcher = reinterpret_cast<VisualTreeWatcher*>(lpParam);
            HRESULT hr = watcher->m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(watcher);
            watcher->Release();
            if (FAILED(hr)) {
                Wh_Log(L"Error %08X", hr);
            }
            return 0;
        },
        this, 0, nullptr);
    if (thread) {
        AddRef();
        CloseHandle(thread);
    }
}

VisualTreeWatcher::~VisualTreeWatcher()
{
    Wh_Log(L"Destructing VisualTreeWatcher");
}

void VisualTreeWatcher::UnadviseVisualTreeChange()
{
    Wh_Log(L"UnadviseVisualTreeChange VisualTreeWatcher");
    HRESULT hr = m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
    if (FAILED(hr)) {
        Wh_Log(L"UnadviseVisualTreeChange failed with error %08X", hr);
    }
}

// Reports whether dropping the reference destroyed the element, which is what
// tells the caller that the handle is free to name a different element from now
// on and that the id recorded for this one has to go.
bool VisualTreeWatcher::ReleaseDiagnosticsReference(InstanceHandle handle)
{
    if (!m_XamlDiagnosticsTestHooks) {
        return false;
    }

    winrt::weak_ref<wf::IInspectable> weakElement;
    {
        // Not through FromHandle: a handle whose runtime object is already gone
        // fails to resolve routinely, and throwing for it would pay for an
        // originate with a stack capture every time. The strong reference has
        // to be gone again before the release below, hence the scope.
        wf::IInspectable element;
        HRESULT hr = m_XamlDiagnostics->GetIInspectableFromHandle(handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(element)));
        if (SUCCEEDED(hr) && element) {
            try {
                weakElement = TryMakeWeak(element);
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult());
            }
        }
    }

    HRESULT hr = m_XamlDiagnosticsTestHooks->UnregisterInstance(handle);
    if (FAILED(hr)) {
        Wh_Log(L"UnregisterInstance failed with error %08X", hr);
        return false;
    }

    // Not every reported object supports weak references, and then the release
    // just proceeds unobserved.
    return weakElement && !weakElement.get();
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation relation, VisualElement element, VisualMutationType mutationType) try
{
    Wh_Log(L"========================================");

    switch (mutationType)
    {
    case Add:
        Wh_Log(L"Mutation type: Add %llu", element.Handle);
        break;

    case Remove:
        Wh_Log(L"Mutation type: Remove %llu", element.Handle);
        break;

    default:
        Wh_Log(L"Mutation type: %d %llu", static_cast<int>(mutationType), element.Handle);
        break;
    }

    Wh_Log(L"Element type: %s", element.Type);

    if (!g_initializedForThread)
    {
        Wh_Log(L"Not initialized for thread %u", GetCurrentThreadId());
        return S_OK;
    }

    // Caught here rather than by the handler below, so that the bookkeeping
    // which hands the element's reference back still runs when the styling work
    // throws. Otherwise a single failed element would be held for good.
    try
    {
        if (mutationType == Add)
        {
            const auto inspectable = FromHandle(element.Handle);
            auto elementId = GetOrCreateElementId(element.Handle, inspectable);
            auto frameworkElement = inspectable.try_as<mux::FrameworkElement>();
            if (frameworkElement)
            {
                Wh_Log(L"FrameworkElement name: %s", frameworkElement.Name().c_str());
                if (elementId == ElementId::None)
                {
                    Wh_Log(L"Skipping element which can't be given an id");
                }
                else
                {
                    ApplyCustomizations(elementId, frameworkElement, element.Type);
                }
            }
            else
            {
                Wh_Log(L"Skipping non-FrameworkElement");
            }
        }
        else if (mutationType == Remove)
        {
            CleanupCustomizations(FindElementId(element.Handle));
        }
    }
    catch (...)
    {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }

    // A tree discarded whole is never dismantled, so it reports no removals to
    // be released by.
    FlushDiagnosticsReleasesIfQuiet();

    if (mutationType == Add)
    {
        QueueDiagnosticsRelease(element.Handle);
        QueueDiagnosticsRelease(relation.Parent);
    }
    else if (mutationType == Remove)
    {
        // Queued rather than released outright: this report arrives from inside
        // the Leave walk which is still visiting the subtree being removed.
        QueueDiagnosticsRelease(element.Handle);
        ForgetElementId(element.Handle);
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);

    // Returning an error prevents (some?) further messages, always return
    // success.
    // return hr;
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept
{
    return S_OK;
}

#pragma endregion  // visualtreewatcher_cpp

#pragma region tap_hpp

#include <ocidl.h>

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// {C85D8CC7-5463-40E8-A432-F5916B6427E5}
static constexpr CLSID CLSID_WindhawkTAP = { 0xc85d8cc7, 0x5463, 0x40e8, { 0xa4, 0x32, 0xf5, 0x91, 0x6b, 0x64, 0x27, 0xe5 } };

class WindhawkTAP : public winrt::implements<WindhawkTAP, IObjectWithSite, winrt::non_agile>
{
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown *pUnkSite) override;
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void **ppvSite) noexcept override;

private:
    winrt::com_ptr<IUnknown> site;
};

#pragma endregion  // tap_hpp

#pragma region tap_cpp

HRESULT WindhawkTAP::SetSite(IUnknown *pUnkSite) try
{
    // Only ever 1 VTW at once.
    if (g_visualTreeWatcher)
    {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    site.copy_from(pUnkSite);

    if (site)
    {
        // Decrease refcount increased by InitializeXamlDiagnosticsEx.
        FreeLibrary(GetCurrentModuleHandle());

        g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

HRESULT WindhawkTAP::GetSite(REFIID riid, void **ppvSite) noexcept
{
    return site.as(riid, ppvSite);
}

#pragma endregion  // tap_cpp

#pragma region simplefactory_hpp

#include <Unknwn.h>

template<class T>
struct SimpleFactory : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile>
{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override try
    {
        if (!pUnkOuter)
        {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        }
        else
        {
            return CLASS_E_NOAGGREGATION;
        }
    }
    catch (...)
    {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override
    {
        return S_OK;
    }
};

#pragma endregion  // simplefactory_hpp

#pragma region module_cpp

#include <combaseapi.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try
{
    if (rclsid == CLSID_WindhawkTAP)
    {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    }
    else
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllCanUnloadNow()
{
    if (winrt::get_module_lock())
    {
        return S_FALSE;
    }
    else
    {
        return S_OK;
    }
}

#pragma clang diagnostic pop

#pragma endregion  // module_cpp

#pragma region api_cpp

bool g_inInjectWindhawkTAP = false;

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX = decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkTAP() noexcept
{
    HMODULE module = GetCurrentModuleHandle();
    if (!module)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location)))
    {
    case 0:
    case ARRAYSIZE(location):
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux(GetModuleHandle(L"Microsoft.Internal.FrameworkUdk.dll"));
    if (!wux) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // I didn't find a better way than trying many connections until one works.
    // Reference:
    // https://github.com/microsoft/microsoft-ui-xaml/blob/d74a0332cf0d5e58f12eddce1070fa7a79b4c2db/src/dxaml/xcp/dxaml/lib/DXamlCore.cpp#L2782
    g_inInjectWindhawkTAP = true;

    HRESULT hr;
    for (int i = 0; i < 10000; i++)
    {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"WinUIVisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), L"", location, CLSID_WindhawkTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            break;
        }
    }

    g_inInjectWindhawkTAP = false;

    return hr;
}

#pragma endregion  // api_cpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

#include <windhawk_utils.h>

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <filesystem>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

using namespace std::string_view_literals;

#include <initguid.h>

#include <commctrl.h>
#include <d2d1_1.h>
#include <dwmapi.h>
#include <roapi.h>
#include <shlwapi.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <windows.graphics.effects.h>
#include <winstring.h>

#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.UI.ViewManagement.h>

using namespace winrt::Microsoft::UI::Xaml;

namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace wge = winrt::Windows::Graphics::Effects;
namespace muc = winrt::Microsoft::UI::Composition;
namespace muxh = mux::Hosting;
namespace awge = ABI::Windows::Graphics::Effects;

// https://stackoverflow.com/a/51274008
template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};
using string_setting_unique_ptr =
    std::unique_ptr<const WCHAR[], deleter_from_fn<Wh_FreeStringSetting>>;

using PropertyKeyValue =
    std::pair<DependencyProperty, winrt::Windows::Foundation::IInspectable>;

using PropertyValuesUnresolved =
    std::vector<std::pair<std::wstring, std::wstring>>;
using PropertyValues = std::vector<PropertyKeyValue>;
using PropertyValuesMaybeUnresolved =
    std::variant<PropertyValuesUnresolved, PropertyValues>;

struct ElementMatcher {
    enum class Kind {
        Element,   // Normal element matcher.
        Wildcard,  // '*': matches zero or more intermediate ancestors.
        Root,      // ':root': asserts the next element has no parent.
    };
    Kind kind = Kind::Element;
    std::wstring type;
    std::wstring name;
    std::optional<std::wstring> visualStateGroupName;
    int oneBasedIndex = 0;
    PropertyValuesMaybeUnresolved propertyValues;
};

// A `Property[@VisualState][:]=value` rule that sets a control property.
// `value` may contain `{{...}}` placeholders, in which case `isDynamic()`
// returns true and the rule is re-resolved on every apply.
struct ValueRule {
    std::wstring propertyName;
    std::wstring visualState;
    std::wstring value;
    bool isXamlValue = false;

    bool isDynamic() const { return value.find(L"{{") != std::wstring::npos; }
};

// A `Property=>VarName` rule that observes a control property and writes its
// current value into the named mod-global style variable.
struct CaptureRule {
    std::wstring propertyName;
    std::wstring varName;
};

// Parsed-but-not-yet-resolved rules for one target. Captures and value-rules
// are intentionally split: they live in different fields of `ResolvedRules`
// post-resolution, and the parser already validates that captures cannot carry
// `:=` or `@VisualState`.
struct UnresolvedRules {
    std::vector<ValueRule> valueRules;
    std::vector<CaptureRule> captureRules;
};

struct XamlBlurBrushParams {
    float blurAmount;
    winrt::Windows::UI::Color tint;
    std::optional<uint8_t> tintOpacity;
    std::wstring tintThemeResourceKey;  // Empty if not from ThemeResource
    std::optional<float> tintLuminosityOpacity;
    std::optional<float> tintSaturation;
    std::optional<float> noiseOpacity;
    std::optional<float> noiseDensity;
    std::optional<winrt::Windows::UI::Color> fallbackColor;
    std::wstring fallbackThemeResourceKey;  // Empty if not from ThemeResource
};

// Holds the raw rule body for a style whose value depends on `{{...}}`
// substitutions. Re-resolved on every apply and on every variable change.
// `propertyName` is kept alongside the value because Windows.UI.Xaml's
// DependencyProperty does not expose its name, and the re-resolution path needs
// to feed the name back to the XAML parser.
struct DynamicStyleTemplate {
    std::wstring propertyName;
    std::wstring rawValue;
    bool isXamlValue = false;
};

// Tagged value for one (property, visualState) cell of PropertyOverrides.
// Possible states:
// - IInspectable        : fully resolved WinRT value (literal or static XAML).
//                         Apply directly via SetValue.
// - XamlBlurBrushParams : parsed `<WindhawkBlur .../>` parameters. The brush
//                         instance is constructed at apply time (needs the live
//                         UIElement).
// - DynamicStyleTemplate: rule body contains `{{...}}` substitutions.
//                         Re-resolved on every apply and on every variable
//                         change. This arm appears only inside
//                         PropertyOverrides cells; it is never stored in
//                         ElementPropertyCustomizationState::customValue (see
//                         notes there).
using PropertyOverrideValue =
    std::variant<winrt::Windows::Foundation::IInspectable,
                 XamlBlurBrushParams,
                 DynamicStyleTemplate>;

// Property -> visual state -> value.
using PropertyOverrides =
    std::unordered_map<DependencyProperty,
                       std::unordered_map<std::wstring, PropertyOverrideValue>>;

// Resolved counterpart to CaptureRule: the property name string has been turned
// into an actual DependencyProperty by the XAML parser, so the apply path can
// call RegisterPropertyChangedCallback / GetValue directly without re-resolving
// on every use.
struct CaptureSpec {
    DependencyProperty property{nullptr};
    std::wstring varName;
};

struct ResolvedRules {
    PropertyOverrides propertyOverrides;
    std::vector<CaptureSpec> captures;
    // Whether this target consumes style variables. Lets ApplyCustomizations
    // skip the visual-tree bookkeeping that only variable users need.
    bool hasDynamicValues = false;
};

using PropertyOverridesMaybeUnresolved =
    std::variant<UnresolvedRules, ResolvedRules>;

// A `{{Var}}` reference resolved for one consuming property. The owner lets a
// value change on some other capture of the same name be skipped.
struct StyleVariableDependency {
    std::wstring name;
    ElementId owner = ElementId::None;  // None when the variable was undefined
};

// Interned node of an element's visual-tree spine. Nodes are shared by every
// tracked element under the same ancestor, so the pool holds one node per
// distinct ancestor rather than a full path per element. Once a node exists its
// `parent` and `depth` are final; an element that is later reparented keeps the
// spine it was first seen with, and only the nodes of a spine interned before
// its root object was attached (see GetOrCreateElementTreeNode) are ever
// replaced.
struct ElementTreeNode {
    // A node can outlive the object it describes -- descendant nodes and
    // not-yet-cleaned-up ElementCustomizationState entries keep it alive -- so
    // this is what proves a pool hit isn't a recycled address.
    winrt::weak_ref<DependencyObject> ref;
    std::shared_ptr<ElementTreeNode> parent;
    uint32_t depth = 0;
    // The depth-0 node this spine hangs from, `this` for a root itself. The
    // parent chain keeps it alive, so a raw pointer is enough.
    ElementTreeNode* root = nullptr;
};

// Keyed by the object's IUnknown pointer: COM only guarantees a stable pointer
// for that interface, and the same element is reached both as a
// FrameworkElement and as a VisualTreeHelper::GetParent result.
thread_local std::unordered_map<void*, std::weak_ptr<ElementTreeNode>>
    g_elementTreeNodes;

// Expired pool entries are reaped once the map grows past this, which is then
// set to twice the surviving size, making the sweep amortized O(1).
thread_local size_t g_elementTreeNodesReapThreshold = 64;

void* ElementIdentityKey(DependencyObject const& object) {
    return winrt::get_abi(object.as<winrt::Windows::Foundation::IUnknown>());
}

// A depth-0 node is a placeholder root until proven otherwise: if its object
// has since gained a parent, the spine was interned before that object was
// attached and stops short of the real root. Asked of any node on the spine,
// not just of the root itself, so that a descendant interned through a
// placeholder root is repaired too.
bool IsStaleSpine(ElementTreeNode const& node) {
    auto object = node.root->ref.get();
    return object && Media::VisualTreeHelper::GetParent(object);
}

// Fetch (or build) the spine node for `object`. Uses
// VisualTreeHelper::GetParent rather than Parent(), same reason as in
// FindElementPropertyOverrides. Returns nullptr if a node can't be built,
// leaving callers with no proximity information rather than a wrong answer.
std::shared_ptr<ElementTreeNode> GetOrCreateElementTreeNode(
    DependencyObject object) {
    if (!object) {
        return nullptr;
    }

    std::shared_ptr<ElementTreeNode> node;

    // Ancestors still lacking a node, innermost first. The walk stops at the
    // first ancestor that is already interned, so a new sibling of an
    // already-seen element costs one GetParent call.
    std::vector<DependencyObject> missing;

    try {
        for (auto iter = object; iter;
             iter = Media::VisualTreeHelper::GetParent(iter)) {
            auto key = ElementIdentityKey(iter);

            if (auto it = g_elementTreeNodes.find(key);
                it != g_elementTreeNodes.end()) {
                auto existing = it->second.lock();
                // A weak_ref never resolves to an object other than its own, so
                // a live ref proves this address hasn't been recycled since.
                if (!existing || !existing->ref.get()) {
                    Wh_Log(L"Replacing stale tree node for a reused address");
                    g_elementTreeNodes.erase(it);
                } else if (!IsStaleSpine(*existing)) {
                    node = std::move(existing);
                    break;
                } else {
                    // Drop the node and keep walking: the ancestors above it
                    // are stale for the same reason, up to the placeholder
                    // root, above which the real spine gets built. A stale
                    // shared_ptr already cached elsewhere (see
                    // EnsureElementTreeNode) is refreshed the same way on its
                    // own next use, so no element is stuck unrankable.
                    Wh_Log(L"Rebuilding tree node interned before attachment");
                    g_elementTreeNodes.erase(it);
                }
            }

            missing.push_back(iter);
        }

        for (auto it = missing.rbegin(); it != missing.rend(); ++it) {
            auto fresh = std::make_shared<ElementTreeNode>();
            fresh->ref = *it;
            fresh->depth = node ? node->depth + 1 : 0;
            fresh->root = node ? node->root : fresh.get();
            fresh->parent = std::move(node);
            g_elementTreeNodes[ElementIdentityKey(*it)] = fresh;
            node = std::move(fresh);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        return nullptr;
    }

    return node;
}

void ReapElementTreeNodesIfNeeded() {
    if (g_elementTreeNodes.size() < g_elementTreeNodesReapThreshold) {
        return;
    }

    std::erase_if(g_elementTreeNodes,
                  [](const auto& item) { return item.second.expired(); });
    g_elementTreeNodesReapThreshold =
        std::max<size_t>(64, g_elementTreeNodes.size() * 2);
}

// Depth of the lowest common ancestor of two spine nodes, or -1 when they have
// none (separate visual trees, or a node that couldn't be built). A node counts
// as its own ancestor, so an element on the other's parent chain scores its own
// depth -- the deepest score that element can reach.
int ElementTreeLcaDepth(ElementTreeNode const* a, ElementTreeNode const* b) {
    if (!a || !b) {
        return -1;
    }

    while (a->depth > b->depth) {
        a = a->parent.get();
    }
    while (b->depth > a->depth) {
        b = b->parent.get();
    }

    while (a != b) {
        a = a->parent.get();
        b = b->parent.get();
        if (!a || !b) {
            return -1;
        }
    }

    return static_cast<int>(a->depth);
}

struct ElementCustomizationRules {
    ElementMatcher elementMatcher;
    std::vector<ElementMatcher> parentElementMatchers;
    PropertyOverridesMaybeUnresolved propertyOverrides;
};

thread_local std::vector<ElementCustomizationRules>
    g_elementsCustomizationRules;

struct ElementPropertyCustomizationState {
    std::optional<winrt::Windows::Foundation::IInspectable> originalValue;
    // The most recently applied value, re-pushed by the per-DP property-
    // changed callback when something external (animation, system Setter)
    // overrides it. Although PropertyOverrideValue's variant declares a
    // DynamicStyleTemplate arm, customValue here is always either IInspectable
    // or XamlBlurBrushParams in practice -- dynamic styles get resolved into
    // one of those before being stored, and the source template lives
    // separately in `dynamicTemplate` below.
    std::optional<PropertyOverrideValue> customValue;
    // The value SetOrClearValue wrote for customValue, which is what a write
    // by something else is told apart from.
    winrt::Windows::Foundation::IInspectable lastAppliedValue{nullptr};
    int64_t propertyChangedToken = 0;
    // Source template for dynamic styles whose value contains `{{...}}`
    // substitutions; re-evaluated whenever a referenced variable changes, with
    // the resolved result written back into `customValue`. Empty for static
    // styles.
    std::optional<DynamicStyleTemplate> dynamicTemplate;
    // Style variables this property's value depends on, each with the capture
    // that supplied it. Populated alongside `dynamicTemplate`; empty for static
    // styles.
    std::vector<StyleVariableDependency> variableDependencies;
    // Makes this property re-resolve on any change to any of its variables:
    // expansion aborts at the first failure, so the names past that point have
    // no recorded owner and a targeted propagation would never reach them.
    bool lastResolveFailed = false;
};

struct CapturePropertyCustomizationState {
    std::wstring varName;
    int64_t propertyChangedToken = 0;
};

struct ElementCustomizationStateForVisualStateGroup {
    std::unordered_map<DependencyProperty, ElementPropertyCustomizationState>
        propertyCustomizationStates;
    winrt::event_token visualStateGroupCurrentStateChangedToken;
};

struct ElementCustomizationState {
    winrt::weak_ref<FrameworkElement> element;

    // Scores how close each capture of a style variable is to this element.
    // Only built for elements that capture or consume a variable.
    std::shared_ptr<ElementTreeNode> treeNode;

    // Capture state lives at the element level: capture rules (`Prop=>Var`) are
    // intentionally not visual-state-aware (the parser rejects `@VisualState`
    // on them), and a single element observed by multiple targets with
    // different VSGs should still only register one
    // RegisterPropertyChangedCallback per DP and one SizeChanged subscription.
    std::unordered_map<DependencyProperty, CapturePropertyCustomizationState>
        captureCustomizationStates;

    // ActualWidth/ActualHeight (and other layout-driven DPs) do not fire
    // RegisterPropertyChangedCallback on UWP, so any element with capture rules
    // also subscribes to `FrameworkElement.SizeChanged` to pick up size
    // changes.
    winrt::event_token captureSizeChangedToken;

    // Use list to avoid reallocations on insertion, as pointers to items are
    // captured in callbacks and stored.
    std::list<std::pair<std::optional<winrt::weak_ref<VisualStateGroup>>,
                        ElementCustomizationStateForVisualStateGroup>>
        perVisualStateGroup;
};

thread_local std::unordered_map<ElementId, ElementCustomizationState>
    g_elementsCustomizationState;

// The weak reference is what keeps an id honest. A handle is an address, so a
// destroyed element can be replaced by one reporting the same handle, and an
// entry whose element is gone, or is no longer the element being asked about,
// belongs to that destroyed predecessor and must not name the new one.
struct ElementIdEntry {
    ElementId id = ElementId::None;
    winrt::weak_ref<wf::IInspectable> element;
};

thread_local std::unordered_map<InstanceHandle, ElementIdEntry> g_elementIds;
thread_local uint64_t g_lastElementId;

ElementId GetOrCreateElementId(InstanceHandle handle,
                               wf::IInspectable const& element) {
    if (!handle || !element) {
        return ElementId::None;
    }

    auto& entry = g_elementIds[handle];
    if (entry.id != ElementId::None && entry.element.get() == element) {
        return entry.id;
    }

    entry.id = static_cast<ElementId>(++g_lastElementId);

    winrt::weak_ref<wf::IInspectable> weakElement;
    try {
        weakElement = TryMakeWeak(element);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    if (!weakElement) {
        // Without a weak reference the entry cannot be told apart from one for
        // a successor at the same address, so neither it nor the id it names is
        // kept: an id no lookup can reach again would key state that nothing
        // could ever tear down, on an element nothing would then hold back from
        // being released.
        g_elementIds.erase(handle);
        return ElementId::None;
    }

    entry.element = std::move(weakElement);
    return entry.id;
}

// By handle alone, for the element which is being reported as removed: it is
// the element the entry was made for, and a stale entry names something already
// destroyed, whose state is due for teardown either way.
ElementId FindElementId(InstanceHandle handle) {
    auto it = g_elementIds.find(handle);
    return it != g_elementIds.end() ? it->second.id : ElementId::None;
}

void ForgetElementId(InstanceHandle handle) {
    g_elementIds.erase(handle);
}

// Dead entries are reaped once the map grows past this, which is then set to
// twice the surviving size, making the sweep amortized O(1).
thread_local size_t g_elementIdsReapThreshold = 64;

// An element whose diagnostics reference was handed back is destroyed without a
// removal being reported for it, so what the mod keys by that element has to be
// found rather than told. An entry whose weak reference no longer resolves
// names such an element, and is torn down the way its removal would have.
void ReapDeadElementIdsIfNeeded() {
    if (g_elementIds.size() < g_elementIdsReapThreshold) {
        return;
    }

    // Collected before anything is torn down: CleanupCustomizations runs XAML
    // work which can re-enter ApplyCustomizations and rehash the map.
    std::vector<std::pair<InstanceHandle, ElementId>> dead;
    for (const auto& [handle, entry] : g_elementIds) {
        if (!entry.element.get()) {
            dead.push_back({handle, entry.id});
        }
    }

    if (!dead.empty()) {
        Wh_Log(L"Reaping %zu of %zu element ids", dead.size(),
               g_elementIds.size());
    }

    for (const auto& [handle, elementId] : dead) {
        CleanupCustomizations(elementId);
        g_elementIds.erase(handle);
    }

    g_elementIdsReapThreshold = std::max<size_t>(64, g_elementIds.size() * 2);
}

// The element's spine node. An element can be matched before its subtree is
// attached, in which case the eager build in ApplyCustomizations interns a
// spine that stops at a placeholder root; re-checked on every use so it's
// rebuilt once the subtree is actually in the tree.
ElementTreeNode* EnsureElementTreeNode(
    ElementCustomizationState& elementCustomizationState) {
    if (!elementCustomizationState.treeNode ||
        IsStaleSpine(*elementCustomizationState.treeNode)) {
        if (auto element = elementCustomizationState.element.get()) {
            elementCustomizationState.treeNode =
                GetOrCreateElementTreeNode(element);
        }
    }

    return elementCustomizationState.treeNode.get();
}

// Mod-global style variable registry. Populated by `Property=>VarName` capture
// rules and consumed by `{{VarName}}` substitutions in other styles. Every
// capturing element gets its own entry, so a name stays defined until its last
// capture goes away, and a consumer reading the name resolves to whichever
// capture is closest to it in the visual tree.
struct StyleVariableValue {
    std::wstring stringForm;        // invariant-formatted text representation
    std::optional<double> numeric;  // only present when source was numeric
    // True for primitive captures whose `stringForm` is meaningful to insert
    // verbatim into a XAML attribute (numeric, boolean, string). False for
    // opaque types -- their stringForm is the captured class name, kept only
    // for diagnostics; bare-identifier substitution skips such variables.
    bool substitutable = false;
};

// One element's capture of a variable. FindElementPropertyOverrides dedupes
// captures by name, so (name, elementId) identifies an entry.
struct StyleVariableCapture {
    ElementId elementId;
    StyleVariableValue value;
};

struct StyleVariableConsumer {
    ElementId elementId;
    DependencyProperty property{nullptr};
    // Each consumer remembers its own fallbackClassName so that propagation can
    // re-resolve dynamic styles using the consumer's match-site context, not
    // the (potentially different) capturer's.
    std::wstring fallbackClassName;
};

// Mod-global style variable registry. The struct mirrors the per-XamlRoot state
// used by the taskbar styler so the variable-resolution call paths stay aligned
// across the styler mods, but here all elements share one registry.
struct StyleVariableState {
    std::unordered_map<std::wstring, std::vector<StyleVariableCapture>>
        variables;
    std::unordered_map<std::wstring, std::vector<StyleVariableConsumer>>
        consumers;
    // How many entries the two maps above hold for each element. They're keyed
    // by variable name, so without this, asking whether an element appears in
    // either of them means walking every name.
    std::unordered_map<ElementId, size_t> elementRefs;
};

thread_local StyleVariableState g_styleVariableState;

// Non-zero while PropagateStyleVariableChange is running, so nested calls queue
// instead of recursing.
thread_local int g_styleVariablePropagationDepth;

struct PendingStyleVariablePropagation {
    StyleVariableState* state;
    std::wstring varName;
    std::optional<ElementId> changedOwner;

    bool operator==(const PendingStyleVariablePropagation&) const = default;
};

void AddStyleVariableElementRef(StyleVariableState* state,
                                ElementId elementId) {
    state->elementRefs[elementId]++;
}

void ReleaseStyleVariableElementRefs(StyleVariableState* state,
                                     ElementId elementId,
                                     size_t count) {
    if (!count) {
        return;
    }

    auto it = state->elementRefs.find(elementId);
    if (it == state->elementRefs.end()) {
        return;
    }

    if (it->second > count) {
        it->second -= count;
    } else {
        state->elementRefs.erase(it);
    }
}

// Propagations queued while another one is running, drained by the outermost
// PropagateStyleVariableChange frame.
thread_local std::vector<PendingStyleVariablePropagation>
    g_pendingStyleVariablePropagations;

StyleVariableState* GetStyleVariableState() {
    return &g_styleVariableState;
}

thread_local bool g_elementPropertyModifying;

// An image with a remote source fails to load when the process starts before
// the network is up. Such images are tracked so that the load can be retried
// once there's internet access, and are cached in a file in the mod storage
// folder, which is what's loaded when it's there, so that the image shows up at
// once and offline. Only a target which has no image is retried, and only a
// source which isn't showing anything is replaced, so an image that's currently
// displayed can't be blanked out.
struct TrackedImage {
    // An ImageBrush or an Image element. Both hold an image source which can
    // fail to load and both report the outcome, but through unrelated types, so
    // the source is addressed by DependencyProperty and each type gets its own
    // revoker pair.
    winrt::weak_ref<DependencyObject> target;
    DependencyProperty sourceProperty{nullptr};
    // The remote address: the entry's identity and what's downloaded, even
    // while the cached file is what's loaded.
    winrt::Windows::Foundation::Uri uri{nullptr};
    std::wstring url;
    // The cached copy of the image, empty when there's no cache folder.
    std::filesystem::path cachePath;

    // Decode properties of the BitmapImage the style declared, reapplied to the
    // BitmapImage a retry creates.
    int32_t decodePixelWidth = 0;
    int32_t decodePixelHeight = 0;
    Media::Imaging::DecodePixelType decodePixelType =
        Media::Imaging::DecodePixelType::Physical;
    Media::Imaging::BitmapCreateOptions createOptions =
        Media::Imaging::BitmapCreateOptions::None;
    bool autoPlay = true;

    Media::ImageBrush::ImageFailed_revoker brushImageFailedRevoker;
    Media::ImageBrush::ImageOpened_revoker brushImageOpenedRevoker;
    Controls::Image::ImageFailed_revoker elementImageFailedRevoker;
    Controls::Image::ImageOpened_revoker elementImageOpenedRevoker;

    // Whether the target has an image. Retries target the ones which don't.
    bool loaded = false;

    // Whether the target is loading from the cached file rather than from the
    // remote address, which is what a load failure is judged by.
    bool usingCache = false;

    ULONGLONG lastRetryTick = 0;
    int retryCount = 0;
};

struct TrackedImagesForThread {
    // Entries are held by shared_ptr so that event handlers can reference them
    // via a weak_ptr and do nothing once an entry is gone.
    std::list<std::shared_ptr<TrackedImage>> images;
    winrt::Microsoft::UI::Dispatching::DispatcherQueue dispatcher{nullptr};
    winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer retryTimer{nullptr};
    winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer::Tick_revoker
        retryTimerTickRevoker;
    // Tick the scheduled retry round is due at, zero if none is scheduled.
    ULONGLONG retryDueTick = 0;
};

thread_local TrackedImagesForThread g_trackedImagesForThread;

// The remote address of each cached file which has been substituted for one, so
// that a target given an already substituted source is tracked as well.
// Outlives the entries, since the style value it describes is shared by targets
// which come and go. Thread local like that value.
thread_local std::unordered_map<std::wstring, winrt::Windows::Foundation::Uri>
    g_imageCacheUriRemotes;

// A single connectivity transition raises several network status events, and
// the state right after the first one isn't final yet.
constexpr DWORD kNetworkChangeDebounceMs = 2000;

// Minimum delay between the retries of an image, doubling with each attempt up
// to about five minutes. Also keeps a retry from being started while the
// previous one is still loading.
constexpr ULONGLONG kImageRetryBaseDelayMs = 5000;
constexpr int kImageRetryMaxBackoffShift = 6;
constexpr ULONGLONG kImageRetryMaxDelayMs = kImageRetryBaseDelayMs
                                            << kImageRetryMaxBackoffShift;

// Caps the attempts of an image, bounding the series of retries which a failure
// starts. The count starts over once the image has been idle for the maximum
// delay, so connectivity which returns much later can still recover it.
constexpr int kImageRetryMaxCount = 20;

// Guards the globals below it. The network status handler acquires it, so it
// must never be held while adding or removing that handler: the event source
// can wait for an invocation which is already in flight, and registering from a
// UI thread pumps messages, which can re-enter this code on the same thread.
std::mutex g_imageRetryMutex;
bool g_imageRetryActive;
// The dispatcher of each UI thread which has tracked images, used to run a
// retry on the thread that owns the image.
std::vector<winrt::weak_ref<winrt::Microsoft::UI::Dispatching::DispatcherQueue>>
    g_imageRetryDispatchers;
winrt::event_token g_networkStatusChangedToken;
// Set while a thread is registering the handler outside the mutex, so that a
// concurrent or re-entrant call doesn't register a second one.
bool g_networkStatusChangedRegistering;
// Callbacks which are on their way into mod code, counted so that the module
// isn't freed out from under them.
size_t g_imageRetryPendingCallbacks;
std::condition_variable g_imageRetryPendingCallbacksCv;

// A cached file is fetched again once it's this old, and its write time is
// stamped whether or not the fetch gets through, so that the write time doubles
// as when the file was last known to be in use.
constexpr ULONGLONG kImageCacheRefreshIntervalMs = 7ULL * 24 * 60 * 60 * 1000;
// A file which nothing stamps ages until it's swept. Long enough for a theme
// which is switched away from and back to keep its images.
constexpr ULONGLONG kImageCacheMaxUnusedMs = 30ULL * 24 * 60 * 60 * 1000;

// Guards the globals below it.
std::mutex g_imageDownloadMutex;
// The URL of each image to fetch, or an empty string for a cache sweep. The
// path a URL is cached at follows from the URL, so it isn't carried along.
std::list<std::wstring> g_imageDownloadQueue;
// The URL of every queued and in flight job, so that one image isn't fetched
// twice at once. A job which failed is dropped: the retries of the image it's
// for are what ask again, and they're already paced and capped.
std::unordered_set<std::wstring> g_imageDownloadUrls;
// The URL of every cached file which failed to load, taking the images it's
// for back to the remote address for the rest of the process. Not per entry,
// since the file is what was rejected and the entries which share the URL
// would otherwise hand it out again. Global for the same reason: the file is
// process wide, not thread wide.
std::unordered_set<std::wstring> g_imageCacheRejectedUrls;
PTP_WORK g_imageDownloadWork;
// Whether a callback is draining the queue; a job added meanwhile joins it.
bool g_imageDownloadRunning;
bool g_imageDownloadStopping;

enum class ResourceVariableTheme {
    None,
    Dark,
    Light,
};

enum class ResourceVariableType {
    String,
    Xaml,
    ThemeResourceReference,
};

struct ResourceVariableEntry {
    std::wstring key;
    std::wstring value;
    ResourceVariableTheme theme;
    ResourceVariableType type;
};

thread_local std::vector<ResourceVariableEntry> g_resourceVariables;

// Track original resource values for restoration (per-thread since
// Application::Current().Resources() is per-thread).
thread_local std::unordered_map<std::wstring,
                                winrt::Windows::Foundation::IInspectable>
    g_originalResourceValues;

// Track our merged theme dictionary for cleanup (per-thread).
thread_local ResourceDictionary g_resourceVariablesThemeDict{nullptr};

// For listening to theme color changes (per-thread).
thread_local winrt::Windows::UI::ViewManagement::UISettings g_uiSettings{
    nullptr};
thread_local winrt::event_token g_colorValuesChangedToken;

winrt::Windows::Foundation::IInspectable ReadLocalValueWithWorkaround(
    DependencyObject elementDo,
    DependencyProperty property) {
    auto value = elementDo.ReadLocalValue(property);
    if (value) {
        // A workaround for ColumnDefinitionCollection of
        // NavigationBarControlGrid which can't be read by ReadLocalValue for
        // some reason, even though it seems to be a local property.
        if (value == DependencyProperty::UnsetValue()) {
            auto grid = elementDo.try_as<Controls::Grid>();
            if (grid && grid.Name() == L"NavigationBarControlGrid") {
                auto value2 = elementDo.GetValue(property);
                if (value2 && winrt::get_class_name(value2) ==
                                  L"Microsoft.UI.Xaml.Controls."
                                  L"ColumnDefinitionCollection") {
                    Wh_Log(
                        L"Using GetValue workaround for "
                        L"ColumnDefinitionCollection");
                    value = std::move(value2);
                }
            }
        }

        // TODO: Is this still needed?
#if 0
        auto className = winrt::get_class_name(value);
        if (className == L"Windows.UI.Xaml.Data.BindingExpressionBase" ||
            className == L"Windows.UI.Xaml.Data.BindingExpression") {
            // BindingExpressionBase was observed to be returned for XAML
            // properties that were declared as following:
            //
            // <Border ... CornerRadius="{TemplateBinding CornerRadius}" />
            //
            // Calling SetValue with it fails with an error, so we won't be able
            // to use it to restore the value. As a workaround, we use
            // GetAnimationBaseValue to get the value.
            Wh_Log(L"ReadLocalValue returned %s, using GetAnimationBaseValue",
                   className.c_str());
            value = elementDo.GetAnimationBaseValue(property);
        }
#endif
    }

    Wh_Log(L"Read property value %s",
           value ? (value == DependencyProperty::UnsetValue()
                        ? L"(unset)"
                        : winrt::get_class_name(value).c_str())
                 : L"(null)");

    return value;
}

////////////////////////////////////////////////////////////////////////////////
// Noise generation
//
// Generates a tileable noise BMP in memory. Density controls the brightness
// distribution curve via a power function (lower density = sparser bright
// pixels). Opacity is handled downstream by the composition effect graph.
winrt::Windows::Storage::Streams::IRandomAccessStream CreateNoiseStream(
    float density) {
    // Cache the last stream to avoid regenerating when density hasn't changed.
    // The cached stream is never read directly; callers get independent clones
    // via CloneStream() so they don't share a seek cursor.
    thread_local float cachedDensity = std::numeric_limits<float>::quiet_NaN();
    thread_local winrt::Windows::Storage::Streams::InMemoryRandomAccessStream
        cachedStream{nullptr};

    if (density == cachedDensity && cachedStream) {
        return cachedStream.CloneStream();
    }

    // Use 256x256 to minimize visible tiling seams.
    constexpr int kSize = 256;
    constexpr DWORD kBpp = 32;
    constexpr DWORD rowSize = kSize * (kBpp / 8);
    constexpr DWORD dataSize = rowSize * kSize;

    BITMAPFILEHEADER fileHeader{
        .bfType = 0x4D42,  // "BM"
        .bfSize =
            sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dataSize,
        .bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
    };

    BITMAPINFOHEADER infoHeader{
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = kSize,
        .biHeight = kSize,
        .biPlanes = 1,
        .biBitCount = kBpp,
        .biSizeImage = dataSize,
    };

    std::vector<uint8_t> pixels(dataSize);

    // Precompute the density power curve as a lookup table so that
    // std::pow is called 256 times instead of once per pixel (65536).
    float safeDensity = std::clamp(density, 0.001f, 1.0f);
    float exponent = 1.0f / safeDensity;

    uint8_t lut[256];
    for (int i = 0; i < 256; i++) {
        lut[i] = static_cast<uint8_t>(std::pow(i / 255.0f, exponent) * 255.0f);
    }

    std::mt19937 rng(0);
    std::uniform_int_distribution<int> dist(0, 255);

    for (size_t i = 0; i < pixels.size(); i += 4) {
        uint8_t gray = lut[dist(rng)];

        // Fully opaque; opacity is applied downstream by ColorMatrixEffect.
        pixels[i] = gray;
        pixels[i + 1] = gray;
        pixels[i + 2] = gray;
        pixels[i + 3] = 255;
    }

    winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;
    winrt::Windows::Storage::Streams::DataWriter writer(stream);
    writer.WriteBytes(winrt::array_view<const uint8_t>(
        reinterpret_cast<const uint8_t*>(&fileHeader), sizeof(fileHeader)));
    writer.WriteBytes(winrt::array_view<const uint8_t>(
        reinterpret_cast<const uint8_t*>(&infoHeader), sizeof(infoHeader)));
    writer.WriteBytes(pixels);
    writer.StoreAsync().get();
    writer.DetachStream();

    cachedStream = std::move(stream);
    cachedDensity = density;

    return cachedStream.CloneStream();
}

// Blur background implementation, copied from TranslucentTB.
////////////////////////////////////////////////////////////////////////////////
// clang-format off
template <> inline constexpr winrt::guid winrt::impl::guid_v<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>{
    winrt::impl::guid_v<winrt::Windows::Foundation::IPropertyValue>
};

typedef enum MY_D2D1_GAUSSIANBLUR_OPTIMIZATION
{
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED = 0,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED = 1,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY = 2,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_FORCE_DWORD = 0xffffffff

} MY_D2D1_GAUSSIANBLUR_OPTIMIZATION;

////////////////////////////////////////////////////////////////////////////////
// XamlBlurBrush.h
class XamlBlurBrush : public Media::XamlCompositionBrushBaseT<XamlBlurBrush>
{
public:
    XamlBlurBrush(UIElement element,
                  float blurAmount,
                  winrt::Windows::UI::Color tint,
                  std::optional<uint8_t> tintOpacity,
                  winrt::hstring tintThemeResourceKey,
                  std::optional<float> tintLuminosityOpacity,
                  std::optional<float> tintSaturation,
                  std::optional<float> noiseOpacity,
                  std::optional<float> noiseDensity,
                  std::optional<winrt::Windows::UI::Color> fallbackColor,
                  winrt::hstring fallbackThemeResourceKey);
    ~XamlBlurBrush();

    void OnConnected();
    void OnDisconnected();

private:
    void RefreshThemeTint();
    void RefreshFallbackColor();
    bool ShouldUseFallback() const;
    void RefreshBrush();
    muc::CompositionBrush CreateEffectBrush();
    muc::CompositionBrush CreateFallbackBrush();

    muc::Compositor m_compositor;
    float m_blurAmount;
    winrt::Windows::UI::Color m_tint;
    std::optional<uint8_t> m_tintOpacity;
    winrt::hstring m_tintThemeResourceKey;
    std::optional<float> m_tintLuminosityOpacity;
    std::optional<float> m_tintSaturation;
    std::optional<float> m_noiseOpacity;
    std::optional<float> m_noiseDensity;
    std::optional<winrt::Windows::UI::Color> m_fallbackColor;
    winrt::hstring m_fallbackThemeResourceKey;
    Media::SolidColorBrush m_proxyBrush{nullptr};
    Media::SolidColorBrush m_fallbackProxyBrush{nullptr};
    winrt::weak_ref<FrameworkElement> m_weakProxyElement;
    winrt::hstring m_proxyKey;
    winrt::hstring m_fallbackProxyKey;
    winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{nullptr};
    winrt::event_token m_advancedEffectsEnabledChangedToken{};
    winrt::event_token m_energySaverStatusChangedToken{};
    winrt::Microsoft::UI::Dispatching::DispatcherQueue m_dispatcher{nullptr};
    HKEY m_powerKey{nullptr};
    HANDLE m_regNotifyEvent{nullptr};
    HANDLE m_regWaitHandle{nullptr};

    static void CALLBACK OnEnergySaverRegistryChanged(PVOID context,
                                                      BOOLEAN timerOrWaitFired);
};

////////////////////////////////////////////////////////////////////////////////
// windows.graphics.effects.interop.h
#ifndef BUILD_WINDOWS
namespace ABI {
#endif
namespace Windows {
namespace Graphics {
namespace Effects {

typedef interface IGraphicsEffectSource                         IGraphicsEffectSource;
typedef interface IGraphicsEffectD2D1Interop                    IGraphicsEffectD2D1Interop;


typedef enum GRAPHICS_EFFECT_PROPERTY_MAPPING
{
    GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORX,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORY,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORZ,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORW,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4
} GRAPHICS_EFFECT_PROPERTY_MAPPING;

//+-----------------------------------------------------------------------------
//
//  Interface:
//      IGraphicsEffectD2D1Interop
//
//  Synopsis:
//      An interface providing a Interop counterpart to IGraphicsEffect
//      and allowing for metadata queries.
//
//------------------------------------------------------------------------------

#undef INTERFACE
#define INTERFACE IGraphicsEffectD2D1Interop
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop, IUnknown, "2FC57384-A068-44D7-A331-30982FCF7177")
{
    STDMETHOD(GetEffectId)(
        _Out_ GUID * id
        ) PURE;

    STDMETHOD(GetNamedPropertyMapping)(
        LPCWSTR name,
        _Out_ UINT * index,
        _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING * mapping
        ) PURE;

    STDMETHOD(GetPropertyCount)(
        _Out_ UINT * count
        ) PURE;

    STDMETHOD(GetProperty)(
        UINT index,
        _Outptr_ winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue> ** value
        ) PURE;

    STDMETHOD(GetSource)(
        UINT index,
        _Outptr_ IGraphicsEffectSource ** source
        ) PURE;

    STDMETHOD(GetSourceCount)(
        _Out_ UINT * count
        ) PURE;
};


} // namespace Effects
} // namespace Graphics
} // namespace Windows
#ifndef BUILD_WINDOWS
} // namespace ABI
#endif

template <> inline constexpr winrt::guid winrt::impl::guid_v<ABI::Windows::Graphics::Effects::IGraphicsEffectD2D1Interop>{
    0x2FC57384, 0xA068, 0x44D7, { 0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77 }
};


////////////////////////////////////////////////////////////////////////////////
// CompositeEffect.h
struct CompositeEffect : winrt::implements<CompositeEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop>
{
public:
    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override;
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override;
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override;

    // IGraphicsEffect
    winrt::hstring Name();
    void Name(winrt::hstring name);

    std::vector<wge::IGraphicsEffectSource> Sources;
    D2D1_COMPOSITE_MODE Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
private:
    winrt::hstring m_name = L"CompositeEffect";
};

////////////////////////////////////////////////////////////////////////////////
// CompositeEffect.cpp
HRESULT CompositeEffect::GetEffectId(GUID* id) noexcept
{
    if (id == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *id = CLSID_D2D1Composite;
    return S_OK;
}

HRESULT CompositeEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept
{
    if (index == nullptr || mapping == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    const std::wstring_view nameView(name);
    if (nameView == L"Mode")
    {
        *index = D2D1_COMPOSITE_PROP_MODE;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT CompositeEffect::GetPropertyCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = 1;
    return S_OK;
}

HRESULT CompositeEffect::GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept try
{
    if (value == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    switch (index)
    {
        case D2D1_COMPOSITE_PROP_MODE:
            *value = wf::PropertyValue::CreateUInt32((UINT32)Mode).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        default:
            return E_BOUNDS;
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT CompositeEffect::GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept try
{
    if (source == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    winrt::copy_to_abi(Sources.at(index), *reinterpret_cast<void**>(source));
    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT CompositeEffect::GetSourceCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = static_cast<UINT>(Sources.size());
    return S_OK;
}

winrt::hstring CompositeEffect::Name()
{
    return m_name;
}

void CompositeEffect::Name(winrt::hstring name)
{
    m_name = name;
}

////////////////////////////////////////////////////////////////////////////////
// FloodEffect.h
struct FloodEffect : winrt::implements<FloodEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop>
{
public:
    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override;
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override;
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override;

    // IGraphicsEffect
    winrt::hstring Name();
    void Name(winrt::hstring name);

    winrt::Windows::UI::Color Color{};
private:
    winrt::hstring m_name = L"FloodEffect";
};

////////////////////////////////////////////////////////////////////////////////
// FloodEffect.cpp
HRESULT FloodEffect::GetEffectId(GUID* id) noexcept
{
    if (id == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *id = CLSID_D2D1Flood;
    return S_OK;
}

HRESULT FloodEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept
{
    if (index == nullptr || mapping == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    const std::wstring_view nameView(name);
    if (nameView == L"Color")
    {
        *index = D2D1_FLOOD_PROP_COLOR;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT FloodEffect::GetPropertyCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = 1;
    return S_OK;
}

HRESULT FloodEffect::GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept try
{
    if (value == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    switch (index)
    {
        case D2D1_FLOOD_PROP_COLOR:
            *value = wf::PropertyValue::CreateSingleArray({
                Color.R / 255.0f,
                Color.G / 255.0f,
                Color.B / 255.0f,
                Color.A / 255.0f,
            }).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        default:
            return E_BOUNDS;
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT FloodEffect::GetSource(UINT, awge::IGraphicsEffectSource** source) noexcept
{
    if (source == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    return E_BOUNDS;
}

HRESULT FloodEffect::GetSourceCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = 0;
    return S_OK;
}

winrt::hstring FloodEffect::Name()
{
    return m_name;
}

void FloodEffect::Name(winrt::hstring name)
{
    m_name = name;
}

////////////////////////////////////////////////////////////////////////////////
// BorderEffect.h
struct BorderEffect : winrt::implements<BorderEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop>
{
public:
    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override;
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override;
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override;

    // IGraphicsEffect
    winrt::hstring Name();
    void Name(winrt::hstring name);

    wge::IGraphicsEffectSource Source{nullptr};
    D2D1_BORDER_EDGE_MODE ExtendX = D2D1_BORDER_EDGE_MODE_WRAP;
    D2D1_BORDER_EDGE_MODE ExtendY = D2D1_BORDER_EDGE_MODE_WRAP;
private:
    winrt::hstring m_name = L"BorderEffect";
};

////////////////////////////////////////////////////////////////////////////////
// BorderEffect.cpp
HRESULT BorderEffect::GetEffectId(GUID* id) noexcept
{
    if (!id)
    {
        return E_INVALIDARG;
    }

    *id = CLSID_D2D1Border;
    return S_OK;
}

HRESULT BorderEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept
{
    if (!index || !mapping)
    {
        return E_INVALIDARG;
    }

    const std::wstring_view nameView(name);
    if (nameView == L"ExtendX")
    {
        *index = D2D1_BORDER_PROP_EDGE_MODE_X;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    if (nameView == L"ExtendY")
    {
        *index = D2D1_BORDER_PROP_EDGE_MODE_Y;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT BorderEffect::GetPropertyCount(UINT* count) noexcept
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = 2;
    return S_OK;
}

HRESULT BorderEffect::GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept try
{
    if (!value)
    {
        return E_INVALIDARG;
    }

    switch (index)
    {
        case D2D1_BORDER_PROP_EDGE_MODE_X:
            *value = wf::PropertyValue::CreateUInt32((UINT32)ExtendX).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        case D2D1_BORDER_PROP_EDGE_MODE_Y:
            *value = wf::PropertyValue::CreateUInt32((UINT32)ExtendY).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        default:
            return E_BOUNDS;
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT BorderEffect::GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept
{
    if (!source)
    {
        return E_INVALIDARG;
    }

    if (index == 0 && Source)
    {
        winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
        return S_OK;
    }

    return E_BOUNDS;
}

HRESULT BorderEffect::GetSourceCount(UINT* count) noexcept
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = 1;
    return S_OK;
}

winrt::hstring BorderEffect::Name()
{
    return m_name;
}

void BorderEffect::Name(winrt::hstring name)
{
    m_name = name;
}

////////////////////////////////////////////////////////////////////////////////
// GaussianBlurEffect.h
struct GaussianBlurEffect : winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop>
{
public:
    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override;
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override;
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override;

    // IGraphicsEffect
    winrt::hstring Name();
    void Name(winrt::hstring name);

    wge::IGraphicsEffectSource Source;

    float BlurAmount = 3.0f;
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION Optimization = MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED;
    D2D1_BORDER_MODE BorderMode = D2D1_BORDER_MODE_SOFT;
private:
    winrt::hstring m_name = L"GaussianBlurEffect";
};

////////////////////////////////////////////////////////////////////////////////
// GaussianBlurEffect.cpp
HRESULT GaussianBlurEffect::GetEffectId(GUID* id) noexcept
{
    if (id == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *id = CLSID_D2D1GaussianBlur;
    return S_OK;
}

HRESULT GaussianBlurEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept
{
    if (index == nullptr || mapping == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    const std::wstring_view nameView(name);
    if (nameView == L"BlurAmount")
    {
        *index = D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }
    else if (nameView == L"Optimization")
    {
        *index = D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }
    else if (nameView == L"BorderMode")
    {
        *index = D2D1_GAUSSIANBLUR_PROP_BORDER_MODE;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT GaussianBlurEffect::GetPropertyCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = 3;
    return S_OK;
}

HRESULT GaussianBlurEffect::GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept try
{
    if (value == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    switch (index)
    {
        case D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION:
            *value = wf::PropertyValue::CreateSingle(BlurAmount).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        case D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION:
            *value = wf::PropertyValue::CreateUInt32((UINT32)Optimization).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        case D2D1_GAUSSIANBLUR_PROP_BORDER_MODE:
            *value = wf::PropertyValue::CreateUInt32((UINT32)BorderMode).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        default:
            return E_BOUNDS;
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT GaussianBlurEffect::GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept
{
    if (source == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    if (index == 0)
    {
        winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
        return S_OK;
    }
    else
    {
        return E_BOUNDS;
    }
}

HRESULT GaussianBlurEffect::GetSourceCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = 1;
    return S_OK;
}

winrt::hstring GaussianBlurEffect::Name()
{
    return m_name;
}

void GaussianBlurEffect::Name(winrt::hstring name)
{
    m_name = name;
}

////////////////////////////////////////////////////////////////////////////////
// ColorMatrixEffect.h
struct ColorMatrixEffect : winrt::implements<ColorMatrixEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop>
{
public:
    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override;
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override;
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override;

    // IGraphicsEffect
    winrt::hstring Name();
    void Name(winrt::hstring name);

    wge::IGraphicsEffectSource Source{nullptr};

    // D2D1_MATRIX_5X4_F: 5 rows x 4 columns (20 floats), initialized to identity.
    float Matrix[20] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
        0, 0, 0, 0,
    };

    uint32_t AlphaMode = D2D1_COLORMATRIX_ALPHA_MODE_PREMULTIPLIED;
    bool ClampOutput = false;
private:
    winrt::hstring m_name = L"ColorMatrixEffect";
};

////////////////////////////////////////////////////////////////////////////////
// ColorMatrixEffect.cpp
HRESULT ColorMatrixEffect::GetEffectId(GUID* id) noexcept
{
    if (!id)
    {
        return E_INVALIDARG;
    }

    *id = CLSID_D2D1ColorMatrix;
    return S_OK;
}

HRESULT ColorMatrixEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept
{
    if (!index || !mapping)
    {
        return E_INVALIDARG;
    }

    const std::wstring_view nameView(name);
    if (nameView == L"ColorMatrix")
    {
        *index = D2D1_COLORMATRIX_PROP_COLOR_MATRIX;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    if (nameView == L"AlphaMode")
    {
        *index = D2D1_COLORMATRIX_PROP_ALPHA_MODE;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    if (nameView == L"ClampOutput")
    {
        *index = D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT ColorMatrixEffect::GetPropertyCount(UINT* count) noexcept
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = 3;
    return S_OK;
}

HRESULT ColorMatrixEffect::GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept try
{
    if (!value)
    {
        return E_INVALIDARG;
    }

    switch (index)
    {
        case D2D1_COLORMATRIX_PROP_COLOR_MATRIX:
            *value = wf::PropertyValue::CreateSingleArray(
                winrt::array_view<const float>(Matrix, Matrix + 20)
            ).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        case D2D1_COLORMATRIX_PROP_ALPHA_MODE:
            *value = wf::PropertyValue::CreateUInt32(AlphaMode).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        case D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT:
            *value = wf::PropertyValue::CreateBoolean(ClampOutput).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        default:
            return E_BOUNDS;
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT ColorMatrixEffect::GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept
{
    if (!source)
    {
        return E_INVALIDARG;
    }

    if (index == 0 && Source)
    {
        winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
        return S_OK;
    }

    return E_BOUNDS;
}

HRESULT ColorMatrixEffect::GetSourceCount(UINT* count) noexcept
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = 1;
    return S_OK;
}

winrt::hstring ColorMatrixEffect::Name()
{
    return m_name;
}

void ColorMatrixEffect::Name(winrt::hstring name)
{
    m_name = name;
}

////////////////////////////////////////////////////////////////////////////////
// XamlBlurBrush.cpp
XamlBlurBrush::XamlBlurBrush(UIElement element,
                             float blurAmount,
                             winrt::Windows::UI::Color tint,
                             std::optional<uint8_t> tintOpacity,
                             winrt::hstring tintThemeResourceKey,
                             std::optional<float> tintLuminosityOpacity,
                             std::optional<float> tintSaturation,
                             std::optional<float> noiseOpacity,
                             std::optional<float> noiseDensity,
                             std::optional<winrt::Windows::UI::Color> fallbackColor,
                             winrt::hstring fallbackThemeResourceKey) :
    m_compositor(muxh::ElementCompositionPreview::GetElementVisual(element)
                     .Compositor()),
    m_blurAmount(blurAmount),
    m_tint(tint),
    m_tintOpacity(tintOpacity),
    m_tintThemeResourceKey(std::move(tintThemeResourceKey)),
    m_tintLuminosityOpacity(tintLuminosityOpacity),
    m_tintSaturation(tintSaturation),
    m_noiseOpacity(noiseOpacity),
    m_noiseDensity(noiseDensity),
    m_fallbackColor(fallbackColor),
    m_fallbackThemeResourceKey(std::move(fallbackThemeResourceKey))
{
    auto fe = element.try_as<FrameworkElement>();

    auto createProxy = [&](winrt::hstring const& themeResourceKey)
        -> Media::SolidColorBrush
    {
        if (!fe)
        {
            return nullptr;
        }
        std::wstring xaml =
            L"<SolidColorBrush"
            L" xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation\""
            L" Color=\"{ThemeResource " +
            std::wstring(themeResourceKey) + L"}\"/>";
        try
        {
            return Markup::XamlReader::Load(winrt::hstring(xaml))
                .try_as<Media::SolidColorBrush>();
        }
        catch (winrt::hresult_error const& ex)
        {
            Wh_Log(L"Failed to create proxy brush: %08X", ex.code());
            return nullptr;
        }
    };

    static std::atomic<uint64_t> s_proxyCounter{0};

    if (!m_tintThemeResourceKey.empty())
    {
        if (auto proxyBrush = createProxy(m_tintThemeResourceKey))
        {
            auto proxyKey = winrt::hstring(
                L"__WhBlurProxy_" +
                std::to_wstring(++s_proxyCounter));
            fe.Resources().Insert(
                winrt::box_value(proxyKey), proxyBrush);
            m_proxyBrush = proxyBrush;
            m_weakProxyElement = winrt::make_weak(fe);
            m_proxyKey = proxyKey;
            Wh_Log(L"Tint proxy brush for %s inserted with key %s",
                   m_tintThemeResourceKey.c_str(),
                   proxyKey.c_str());
        }

        if (m_proxyBrush)
        {
            m_proxyBrush.RegisterPropertyChangedCallback(
                Media::SolidColorBrush::ColorProperty(),
                [weakThis = get_weak()](auto&&, auto&&)
                {
                    if (auto self = weakThis.get())
                    {
                        Wh_Log(L"Tint theme color changed");
                        self->RefreshBrush();
                    }
                });
        }
    }

    if (!m_fallbackThemeResourceKey.empty())
    {
        if (auto proxyBrush = createProxy(m_fallbackThemeResourceKey))
        {
            auto proxyKey = winrt::hstring(
                L"__WhBlurFallbackProxy_" +
                std::to_wstring(++s_proxyCounter));
            fe.Resources().Insert(
                winrt::box_value(proxyKey), proxyBrush);
            m_fallbackProxyBrush = proxyBrush;
            if (!m_weakProxyElement.get())
            {
                m_weakProxyElement = winrt::make_weak(fe);
            }
            m_fallbackProxyKey = proxyKey;
            Wh_Log(L"Fallback proxy brush for %s inserted with key %s",
                   m_fallbackThemeResourceKey.c_str(),
                   proxyKey.c_str());
        }

        if (m_fallbackProxyBrush)
        {
            m_fallbackProxyBrush.RegisterPropertyChangedCallback(
                Media::SolidColorBrush::ColorProperty(),
                [weakThis = get_weak()](auto&&, auto&&)
                {
                    if (auto self = weakThis.get())
                    {
                        Wh_Log(L"Fallback theme color changed");
                        self->RefreshBrush();
                    }
                });
        }
    }

    if (m_fallbackColor || !m_fallbackThemeResourceKey.empty())
    {
        m_dispatcher =
            winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();

        try
        {
            m_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
            auto dispatcher = m_dispatcher;
            m_advancedEffectsEnabledChangedToken =
                m_uiSettings.AdvancedEffectsEnabledChanged(
                    [weakThis = get_weak(), dispatcher](auto&&, auto&&)
                    {
                        dispatcher.TryEnqueue([weakThis]
                        {
                            if (auto self = weakThis.get())
                            {
                                Wh_Log(L"AdvancedEffectsEnabled changed");
                                self->RefreshBrush();
                            }
                        });
                    });
            m_energySaverStatusChangedToken =
                winrt::Windows::System::Power::PowerManager::
                    EnergySaverStatusChanged(
                        [weakThis = get_weak(), dispatcher](auto&&, auto&&)
                        {
                            dispatcher.TryEnqueue([weakThis]
                            {
                                if (auto self = weakThis.get())
                                {
                                    Wh_Log(L"EnergySaverStatus changed");
                                    self->RefreshBrush();
                                }
                            });
                        });
        }
        catch (winrt::hresult_error const& ex)
        {
            Wh_Log(L"Failed to register fallback state listeners: %08X",
                   ex.code());
        }

        // Watch HKLM\SYSTEM\CurrentControlSet\Control\Power for changes to
        // EnergySaverState. On Windows 11 24H2+ neither the WinRT
        // PowerManager.EnergySaverStatus property nor the Win32
        // GetSystemPowerStatus.SystemStatusFlag flag reliably reflects the
        // "Always use energy saver" setting; the registry value is the only
        // signal that updates in that case. The wait callback re-arms the
        // notification and posts a brush refresh on the UI thread.
        LONG regStatus = RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SYSTEM\\CurrentControlSet\\Control\\Power", 0, KEY_NOTIFY,
            &m_powerKey);
        if (regStatus == ERROR_SUCCESS)
        {
            m_regNotifyEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            if (m_regNotifyEvent)
            {
                regStatus = RegNotifyChangeKeyValue(m_powerKey, FALSE,
                                                   REG_NOTIFY_CHANGE_LAST_SET,
                                                   m_regNotifyEvent, TRUE);
                if (regStatus == ERROR_SUCCESS)
                {
                    if (!RegisterWaitForSingleObject(
                            &m_regWaitHandle, m_regNotifyEvent,
                            OnEnergySaverRegistryChanged, this, INFINITE,
                            WT_EXECUTEINWAITTHREAD))
                    {
                        Wh_Log(L"RegisterWaitForSingleObject failed: %lu",
                               GetLastError());
                        m_regWaitHandle = nullptr;
                    }
                }
                else
                {
                    Wh_Log(L"RegNotifyChangeKeyValue failed: %ld", regStatus);
                    CloseHandle(m_regNotifyEvent);
                    m_regNotifyEvent = nullptr;
                    RegCloseKey(m_powerKey);
                    m_powerKey = nullptr;
                }
            }
            else
            {
                Wh_Log(L"CreateEvent failed: %lu", GetLastError());
                RegCloseKey(m_powerKey);
                m_powerKey = nullptr;
            }
        }
        else
        {
            Wh_Log(L"RegOpenKeyEx for Power key failed: %ld", regStatus);
        }
    }
}

void CALLBACK XamlBlurBrush::OnEnergySaverRegistryChanged(PVOID context,
                                                          BOOLEAN)
{
    auto* self = static_cast<XamlBlurBrush*>(context);

    // Re-arm before dispatching so a rapid second change isn't dropped.
    if (self->m_powerKey && self->m_regNotifyEvent)
    {
        RegNotifyChangeKeyValue(self->m_powerKey, FALSE,
                                REG_NOTIFY_CHANGE_LAST_SET,
                                self->m_regNotifyEvent, TRUE);
    }

    if (self->m_dispatcher)
    {
        auto weakThis = self->get_weak();
        self->m_dispatcher.TryEnqueue([weakThis]
        {
            if (auto strongThis = weakThis.get())
            {
                Wh_Log(L"Power registry key changed, refreshing brush");
                strongThis->RefreshBrush();
            }
        });
    }
}

XamlBlurBrush::~XamlBlurBrush()
{
    // Tear down the registry watch first so no more callbacks can fire while
    // we close the underlying handles.
    if (m_regWaitHandle)
    {
        UnregisterWaitEx(m_regWaitHandle, INVALID_HANDLE_VALUE);
        m_regWaitHandle = nullptr;
    }
    if (m_regNotifyEvent)
    {
        CloseHandle(m_regNotifyEvent);
        m_regNotifyEvent = nullptr;
    }
    if (m_powerKey)
    {
        RegCloseKey(m_powerKey);
        m_powerKey = nullptr;
    }

    if (m_uiSettings && m_advancedEffectsEnabledChangedToken.value)
    {
        try
        {
            m_uiSettings.AdvancedEffectsEnabledChanged(
                m_advancedEffectsEnabledChangedToken);
        }
        catch (...)
        {
            Wh_Log(L"Error %08X", winrt::to_hresult());
        }
    }

    if (m_energySaverStatusChangedToken.value)
    {
        try
        {
            winrt::Windows::System::Power::PowerManager::
                EnergySaverStatusChanged(m_energySaverStatusChangedToken);
        }
        catch (...)
        {
            Wh_Log(L"Error %08X", winrt::to_hresult());
        }
    }

    if (auto element = m_weakProxyElement.get())
    {
        try
        {
            if (!m_proxyKey.empty())
            {
                element.Resources().Remove(winrt::box_value(m_proxyKey));
            }
            if (!m_fallbackProxyKey.empty())
            {
                element.Resources().Remove(
                    winrt::box_value(m_fallbackProxyKey));
            }
        }
        catch (...)
        {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }
}

void XamlBlurBrush::OnConnected()
{
    if (!CompositionBrush())
    {
        RefreshThemeTint();
        RefreshFallbackColor();

        CompositionBrush(ShouldUseFallback() ? CreateFallbackBrush()
                                             : CreateEffectBrush());
    }
}

muc::CompositionBrush XamlBlurBrush::CreateFallbackBrush()
{
    return m_compositor.CreateColorBrush(m_fallbackColor.value_or(m_tint));
}

muc::CompositionBrush XamlBlurBrush::CreateEffectBrush()
{
    auto backdropBrush = m_compositor.CreateBackdropBrush();

    // Rec. 709 luma coefficients, used for saturation and luminosity.
    constexpr float kLumaR = 0.2126f;
    constexpr float kLumaG = 0.7152f;
    constexpr float kLumaB = 0.0722f;

    // 1. Blur
    auto blurEffect = winrt::make_self<GaussianBlurEffect>();
    blurEffect->Source = muc::CompositionEffectSourceParameter(L"backdrop");
    blurEffect->BlurAmount = m_blurAmount;
    blurEffect->Name(L"BlurEffect");

    wge::IGraphicsEffectSource topOfStack = *blurEffect;

    // 2. Saturation (optional)
    if (m_tintSaturation && *m_tintSaturation != 1.0f)
    {
        float s = std::max(*m_tintSaturation, 0.0f);
        float invS = 1.0f - s;

        auto satMatrix = winrt::make_self<ColorMatrixEffect>();
        satMatrix->Source = topOfStack;

        // Standard saturation matrix: lerp between luminance and identity.
        auto& m = satMatrix->Matrix;
        m[0]  = invS * kLumaR + s; m[1]  = invS * kLumaR;     m[2]  = invS * kLumaR;     m[3]  = 0.0f;
        m[4]  = invS * kLumaG;     m[5]  = invS * kLumaG + s; m[6]  = invS * kLumaG;     m[7]  = 0.0f;
        m[8]  = invS * kLumaB;     m[9]  = invS * kLumaB;     m[10] = invS * kLumaB + s; m[11] = 0.0f;
        m[12] = 0.0f;              m[13] = 0.0f;              m[14] = 0.0f;              m[15] = 1.0f;

        satMatrix->Name(L"SaturationEffect");
        topOfStack = *satMatrix;
    }

    // 3. Luminosity (optional) - shifts pixel luminance towards the tint's
    // luminance, blended by the opacity factor.
    if (m_tintLuminosityOpacity && *m_tintLuminosityOpacity > 0.0f)
    {
        float op = std::clamp(*m_tintLuminosityOpacity, 0.0f, 1.0f);

        float tintLum = (m_tint.R / 255.0f) * kLumaR +
                        (m_tint.G / 255.0f) * kLumaG +
                        (m_tint.B / 255.0f) * kLumaB;

        auto lumMatrix = winrt::make_self<ColorMatrixEffect>();
        lumMatrix->Source = topOfStack;

        auto& m = lumMatrix->Matrix;
        m[0]  = 1.0f - (kLumaR * op); m[1]  = -(kLumaR * op);       m[2]  = -(kLumaR * op);       m[3]  = 0.0f;
        m[4]  = -(kLumaG * op);       m[5]  = 1.0f - (kLumaG * op); m[6]  = -(kLumaG * op);       m[7]  = 0.0f;
        m[8]  = -(kLumaB * op);       m[9]  = -(kLumaB * op);       m[10] = 1.0f - (kLumaB * op); m[11] = 0.0f;
        m[12] = 0.0f;                 m[13] = 0.0f;                 m[14] = 0.0f;                 m[15] = 1.0f;
        m[16] = tintLum * op;         m[17] = tintLum * op;         m[18] = tintLum * op;         m[19] = 0.0f;

        lumMatrix->Name(L"LuminosityBlend");
        topOfStack = *lumMatrix;
    }

    // 4. Noise overlay (optional) - procedural tiled noise with adjustable
    // density and opacity.
    muc::CompositionSurfaceBrush noiseBrush{nullptr};
    if (m_noiseOpacity && *m_noiseOpacity > 0.0f)
    {
        float density = m_noiseDensity.value_or(1.0f);

        auto stream = CreateNoiseStream(density);
        auto surface =
            Media::LoadedImageSurface::StartLoadFromStream(stream);
        noiseBrush = m_compositor.CreateSurfaceBrush(surface);
        noiseBrush.Stretch(muc::CompositionStretch::None);

        // Tile via border effect (wrap mode).
        auto borderEffect = winrt::make_self<BorderEffect>();
        borderEffect->Source =
            muc::CompositionEffectSourceParameter(L"NoiseSource");

        // Scale all channels by opacity for premultiplied blending.
        float nOp = std::clamp(*m_noiseOpacity, 0.0f, 1.0f);

        auto opacityEffect = winrt::make_self<ColorMatrixEffect>();
        opacityEffect->Source = *borderEffect;
        // Matrix: Scale all channels by opacity (for premultiplied blending).
        opacityEffect->Matrix[0] = nOp;
        opacityEffect->Matrix[5] = nOp;
        opacityEffect->Matrix[10] = nOp;
        opacityEffect->Matrix[15] = nOp;
        opacityEffect->Name(L"NoiseOpacityEffect");

        // Composite noise over the current stack.
        auto noiseComposite = winrt::make_self<CompositeEffect>();
        noiseComposite->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
        noiseComposite->Sources.push_back(topOfStack);
        noiseComposite->Sources.push_back(*opacityEffect);
        noiseComposite->Name(L"NoiseComposite");
        topOfStack = *noiseComposite;
    }

    // 5. Tint (flood color composited over the stack).
    auto floodEffect = winrt::make_self<FloodEffect>();
    floodEffect->Color = m_tint;
    floodEffect->Name(L"FloodEffect");

    auto compositeEffect = winrt::make_self<CompositeEffect>();
    compositeEffect->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
    compositeEffect->Sources.push_back(topOfStack);
    compositeEffect->Sources.push_back(*floodEffect);

    auto factory = m_compositor.CreateEffectFactory(*compositeEffect);
    auto brush = factory.CreateBrush();

    brush.SetSourceParameter(L"backdrop", backdropBrush);

    // Bind the noise brush if we created one.
    if (noiseBrush)
    {
        brush.SetSourceParameter(L"NoiseSource", noiseBrush);
    }

    return brush;
}

void XamlBlurBrush::OnDisconnected()
{
    if (const auto brush = CompositionBrush())
    {
        brush.Close();
        CompositionBrush(nullptr);
    }
}

void XamlBlurBrush::RefreshThemeTint()
{
    if (!m_proxyBrush)
    {
        return;
    }

    m_tint = m_proxyBrush.Color();
    if (m_tintOpacity)
    {
        m_tint.A = *m_tintOpacity;
    }
}

void XamlBlurBrush::RefreshFallbackColor()
{
    if (!m_fallbackProxyBrush)
    {
        return;
    }

    m_fallbackColor = m_fallbackProxyBrush.Color();
}

bool XamlBlurBrush::ShouldUseFallback() const
{
    if (!m_fallbackColor && m_fallbackThemeResourceKey.empty())
    {
        return false;
    }

    // The HKLM\SYSTEM\CurrentControlSet\Control\Power\EnergySaverState value
    // is the only signal that consistently reflects "Always use energy saver"
    // on Windows 11 24H2+; the WinRT and Win32 power-status APIs can stay
    // stuck in the off state on those builds. 1 = enabled, 2 = disabled.
    bool energySaverActive = false;
    HKEY key{};
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SYSTEM\\CurrentControlSet\\Control\\Power", 0,
                      KEY_QUERY_VALUE, &key) == ERROR_SUCCESS)
    {
        DWORD value = 0;
        DWORD type = 0;
        DWORD size = sizeof(value);
        if (RegQueryValueExW(key, L"EnergySaverState", nullptr, &type,
                             reinterpret_cast<LPBYTE>(&value),
                             &size) == ERROR_SUCCESS &&
            type == REG_DWORD)
        {
            energySaverActive = (value == 1);
        }
        RegCloseKey(key);
    }

    // Backup for older Windows where the registry value above isn't populated.
    if (!energySaverActive)
    {
        SYSTEM_POWER_STATUS powerStatus{};
        if (GetSystemPowerStatus(&powerStatus) &&
            powerStatus.SystemStatusFlag != 0)
        {
            energySaverActive = true;
        }
    }

    bool advancedEffectsOff = false;
    if (m_uiSettings)
    {
        try
        {
            advancedEffectsOff = !m_uiSettings.AdvancedEffectsEnabled();
        }
        catch (...)
        {
            Wh_Log(L"AdvancedEffectsEnabled query failed: %08X",
                   winrt::to_hresult());
        }
    }

    return energySaverActive || advancedEffectsOff;
}

void XamlBlurBrush::RefreshBrush()
{
    if (const auto brush = CompositionBrush())
    {
        brush.Close();
        CompositionBrush(nullptr);
        OnConnected();
    }
}

// clang-format on
////////////////////////////////////////////////////////////////////////////////

// Helper functions for tracking, caching and retrying remote image loads.

// Reports true if the query itself fails, as a retry which turns out to be
// pointless is harmless, while skipping a necessary one leaves images missing.
bool HasInternetAccess() {
    try {
        auto profile = winrt::Windows::Networking::Connectivity::
            NetworkInformation::GetInternetConnectionProfile();
        return profile && profile.GetNetworkConnectivityLevel() ==
                              winrt::Windows::Networking::Connectivity::
                                  NetworkConnectivityLevel::InternetAccess;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        return true;
    }
}

// The folder the remote images are cached in, empty if it's not available, in
// which case images are only ever loaded from their remote source.
const std::filesystem::path& GetImageCacheDir() {
    static const std::filesystem::path dir = []() -> std::filesystem::path {
        WCHAR storagePathBuffer[MAX_PATH];
        if (!Wh_GetModStoragePath(storagePathBuffer,
                                  ARRAYSIZE(storagePathBuffer))) {
            Wh_Log(L"Wh_GetModStoragePath failed");
            return std::filesystem::path();
        }

        auto path = std::filesystem::path{storagePathBuffer} / L"images";

        std::error_code ec;
        std::filesystem::create_directories(path, ec);
        if (!std::filesystem::is_directory(path, ec)) {
            Wh_Log(L"Failed to create %s", path.c_str());
            return std::filesystem::path();
        }

        return path;
    }();

    return dir;
}

// The cached copy of a remote image, named uniquely after its URL. Empty when
// there's no cache folder. The extension of the URL is kept so that the folder
// can be looked through.
std::filesystem::path ImageCachePath(std::wstring_view url) {
    const auto& cacheDir = GetImageCacheDir();
    if (cacheDir.empty()) {
        return std::filesystem::path();
    }

    // FNV-1a; one mod's folder, so an unlikely collision is good enough.
    uint64_t hash = 14695981039346656037ULL;
    for (wchar_t c : url) {
        hash ^= (uint16_t)c;
        hash *= 1099511628211ULL;
    }

    WCHAR hashString[17];
    swprintf_s(hashString, L"%016llx", hash);

    std::wstring name = hashString;

    auto urlPath = url.substr(0, url.find_first_of(L"?#"));
    auto extension = std::filesystem::path(urlPath).extension().native();
    if (extension.length() >= 2 && extension.length() <= 5 &&
        std::all_of(extension.begin() + 1, extension.end(), [](wchar_t c) {
            return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') ||
                   (c >= L'0' && c <= L'9');
        })) {
        name += extension;
    }

    return cacheDir / name;
}

// XAML loads a local image through a file URI.
winrt::Windows::Foundation::Uri ImageCacheFileUri(
    const std::filesystem::path& path) {
    // Room for every character to be escaped, plus the scheme.
    std::wstring uri(path.native().size() * 3 + 16, L'\0');

    DWORD uriLength = (DWORD)uri.size();
    HRESULT hr = UrlCreateFromPath(path.c_str(), uri.data(), &uriLength, 0);
    if (FAILED(hr)) {
        Wh_Log(L"UrlCreateFromPath returned 0x%08X", hr);
        return nullptr;
    }

    uri.resize(uriLength);

    try {
        return winrt::Windows::Foundation::Uri(uri);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        return nullptr;
    }
}

// The time since a file was written.
ULONGLONG FileAgeMs(std::filesystem::file_time_type writeTime) {
    auto age = std::filesystem::file_time_type::clock::now() - writeTime;

    // A file stamped in the future, e.g. after a clock change, is brand new.
    if (age.count() <= 0) {
        return 0;
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(age).count();
}

// The time since a file was written, nullopt if there's no such file.
std::optional<ULONGLONG> FileAgeMs(const std::filesystem::path& path) {
    std::error_code ec;
    auto writeTime = std::filesystem::last_write_time(path, ec);
    if (ec) {
        return std::nullopt;
    }

    return FileAgeMs(writeTime);
}

void TouchFile(const std::filesystem::path& path) {
    std::error_code ec;
    std::filesystem::last_write_time(
        path, std::filesystem::file_time_type::clock::now(), ec);
}

// Removes the files which haven't been stamped for a long time, which is what
// becomes of a theme's images once it's out of use, and of what an interrupted
// download leaves behind.
void SweepImageCache() {
    const auto& cacheDir = GetImageCacheDir();
    if (cacheDir.empty()) {
        return;
    }

    Wh_Log(L"Sweeping the image cache");

    try {
        std::error_code ec;
        for (const auto& entry :
             std::filesystem::directory_iterator(cacheDir, ec)) {
            if (!entry.is_regular_file(ec)) {
                continue;
            }

            auto writeTime = entry.last_write_time(ec);
            if (ec || FileAgeMs(writeTime) < kImageCacheMaxUnusedMs) {
                continue;
            }

            Wh_Log(L"Removing unused cached image: %s",
                   entry.path().filename().c_str());
            std::filesystem::remove(entry.path(), ec);
        }
    } catch (const std::exception& ex) {
        Wh_Log(L"Error sweeping the image cache: %S", ex.what());
    }
}

// Via a temporary file, so that a partial or failed response, which the engine
// writes before the status code is known, never becomes the cached image.
void DownloadImage(const std::wstring& url) {
    auto cachePath = ImageCachePath(url);
    if (cachePath.empty()) {
        return;
    }

    auto tempPath = cachePath;
    tempPath += L".tmp" + std::to_wstring(GetCurrentProcessId());

    bool succeeded = false;

    WH_GET_URL_CONTENT_OPTIONS options{
        .optionsSize = sizeof(options),
        .targetFilePath = tempPath.c_str(),
    };

    if (const WH_URL_CONTENT* urlContent =
            Wh_GetUrlContent(url.c_str(), &options)) {
        if (urlContent->statusCode == 200) {
            succeeded = true;
        } else {
            Wh_Log(L"Wh_GetUrlContent returned %d", urlContent->statusCode);
        }

        Wh_FreeUrlContent(urlContent);
    } else {
        Wh_Log(L"Wh_GetUrlContent failed");
    }

    std::error_code ec;

    if (succeeded) {
        auto size = std::filesystem::file_size(tempPath, ec);
        if (ec || size == 0) {
            Wh_Log(L"No downloaded file to move into place");
            succeeded = false;
        }
    }

    if (succeeded) {
        std::filesystem::rename(tempPath, cachePath, ec);
        if (ec) {
            // Another process can be reading it.
            Wh_Log(L"Failed to move %s into place", tempPath.c_str());
        }
    }

    std::filesystem::remove(tempPath, ec);
}

void ProcessImageDownloads() {
    for (;;) {
        std::wstring url;

        {
            std::lock_guard<std::mutex> lock(g_imageDownloadMutex);

            if (g_imageDownloadStopping || g_imageDownloadQueue.empty()) {
                g_imageDownloadRunning = false;
                return;
            }

            url = std::move(g_imageDownloadQueue.front());
            g_imageDownloadQueue.pop_front();
        }

        if (url.empty()) {
            SweepImageCache();
            continue;
        }

        Wh_Log(L"Downloading image: %s", url.c_str());
        DownloadImage(url);

        std::lock_guard<std::mutex> lock(g_imageDownloadMutex);
        g_imageDownloadUrls.erase(url);
    }
}

// Must be called with g_imageDownloadMutex held, with the job it's for already
// queued.
void SubmitImageDownloadWork() {
    if (g_imageDownloadRunning) {
        return;
    }

    if (!g_imageDownloadWork) {
        g_imageDownloadWork =
            CreateThreadpoolWork([](PTP_CALLBACK_INSTANCE, PVOID,
                                    PTP_WORK) { ProcessImageDownloads(); },
                                 nullptr, nullptr);
        if (!g_imageDownloadWork) {
            Wh_Log(L"Failed to create the image download work item");
            g_imageDownloadQueue.clear();
            g_imageDownloadUrls.clear();
            return;
        }
    }

    g_imageDownloadRunning = true;
    SubmitThreadpoolWork(g_imageDownloadWork);
}

void QueueImageDownload(const std::wstring& url) {
    std::lock_guard<std::mutex> lock(g_imageDownloadMutex);

    if (g_imageDownloadStopping) {
        return;
    }

    if (!g_imageDownloadUrls.insert(url).second) {
        return;
    }

    g_imageDownloadQueue.push_back(url);

    SubmitImageDownloadWork();
}

// Asks the download thread for a sweep, once per process. Not tied to there
// being anything to download, so that a cache which is fully up to date, and
// whose unused files nothing else removes, is swept too.
void QueueImageCacheSweep() {
    static std::once_flag once;
    std::call_once(once, []() {
        std::lock_guard<std::mutex> lock(g_imageDownloadMutex);

        if (g_imageDownloadStopping) {
            return;
        }

        g_imageDownloadQueue.emplace_back();

        SubmitImageDownloadWork();
    });
}

// Whether the cached file of a URL failed to load, which takes the images it's
// for back to the remote address.
bool IsImageCacheRejected(const std::wstring& url) {
    std::lock_guard<std::mutex> lock(g_imageDownloadMutex);

    return g_imageCacheRejectedUrls.contains(url);
}

void RejectImageCache(const std::wstring& url) {
    std::lock_guard<std::mutex> lock(g_imageDownloadMutex);

    g_imageCacheRejectedUrls.insert(url);
}

void StopImageDownloads() {
    PTP_WORK work;

    {
        std::lock_guard<std::mutex> lock(g_imageDownloadMutex);

        g_imageDownloadStopping = true;
        g_imageDownloadQueue.clear();
        g_imageDownloadUrls.clear();
        g_imageCacheRejectedUrls.clear();

        work = g_imageDownloadWork;
        g_imageDownloadWork = nullptr;
    }

    // A request which is in flight can't be cancelled, so a server which is
    // slow to answer holds up the unload for as long as it takes. Accepted as
    // it is: the alternative is letting the callback run on into a module which
    // is going away.
    if (work) {
        WaitForThreadpoolWorkCallbacks(work, TRUE);
        CloseThreadpoolWork(work);
    }
}

// The address an entry should load from: the cached file when there is one, the
// remote address otherwise. Asks for the download the answer implies.
winrt::Windows::Foundation::Uri ImageSourceUri(
    const std::shared_ptr<TrackedImage>& tracked) {
    if (!tracked->cachePath.empty() && !IsImageCacheRejected(tracked->url)) {
        if (auto age = FileAgeMs(tracked->cachePath)) {
            if (*age >= kImageCacheRefreshIntervalMs) {
                // Stamped whether or not the download gets through, so that an
                // offline machine doesn't lose the images it's using.
                TouchFile(tracked->cachePath);
                QueueImageDownload(tracked->url);
            }

            if (auto uri = ImageCacheFileUri(tracked->cachePath)) {
                return uri;
            }
        } else {
            QueueImageDownload(tracked->url);
        }
    }

    return tracked->uri;
}

// Takes the BitmapImage the style declared off a cached file which has been
// rejected, so that reapplying the value doesn't put the file which failed back
// on the target. Nothing is showing from that file, so this is the one case
// where a source is replaced without regard for what the target holds.
void RestoreRejectedImageSource(
    const std::shared_ptr<TrackedImage>& tracked,
    Media::Imaging::BitmapImage const& bitmapImage) {
    try {
        if (bitmapImage.UriSource().Equals(tracked->uri) ||
            !IsImageCacheRejected(tracked->url)) {
            return;
        }

        Wh_Log(L"Loading remote image for: %s", tracked->url.c_str());

        bitmapImage.UriSource(tracked->uri);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

void StartImageRetry(const std::shared_ptr<TrackedImage>& tracked) {
    auto target = tracked->target.get();
    if (!target) {
        return;
    }

    Wh_Log(L"Retrying image load for: %s", tracked->url.c_str());

    tracked->lastRetryTick = GetTickCount64();
    tracked->retryCount++;

    // An Image element's source is a property the mod customizes, and writing
    // to it would otherwise be seen as an external change and reverted to the
    // failed source the style declared.
    bool wasModifying = g_elementPropertyModifying;
    g_elementPropertyModifying = true;

    try {
        // The cached file when a download has landed since the last attempt.
        auto uri = ImageSourceUri(tracked);
        tracked->usingCache = !uri.Equals(tracked->uri);

        Media::Imaging::BitmapImage retryImage;
        // Bypass the XAML image cache: a retry is only needed when what the
        // cache holds for the URI is a failed or missing image.
        retryImage.CreateOptions(
            tracked->createOptions |
            Media::Imaging::BitmapCreateOptions::IgnoreImageCache);
        retryImage.DecodePixelType(tracked->decodePixelType);
        retryImage.DecodePixelWidth(tracked->decodePixelWidth);
        retryImage.DecodePixelHeight(tracked->decodePixelHeight);
        retryImage.AutoPlay(tracked->autoPlay);

        // A BitmapImage is loaded by the framework as part of the tree it's
        // used in, so it has to be assigned to the target for anything to
        // happen. A new object rather than the failed one, since reassigning
        // the same URI to a BitmapImage doesn't reload it. The target's own
        // ImageOpened and ImageFailed report how this attempt went.
        target.SetValue(tracked->sourceProperty, retryImage);

        // The URI goes last: XAML decodes an image to the size it's displayed
        // at only when the BitmapImage is already connected to the live tree by
        // the time its source is set. Setting the URI first decodes at the
        // image's natural size, which is then scaled at render time and looks
        // poor.
        retryImage.UriSource(uri);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    g_elementPropertyModifying = wasModifying;
}

// The wait before the next attempt of an image which has been retried
// `retryCount` times.
ULONGLONG ImageRetryDelayMs(int retryCount) {
    return kImageRetryBaseDelayMs
           << std::clamp(retryCount - 1, 0, kImageRetryMaxBackoffShift);
}

void ScheduleImageLoadRetryOnCurrentThread(ULONGLONG delayMs, bool reschedule);

// Retries every image which is due, and arms the next round for the earliest
// image which isn't, so that a failed image recovers on its own instead of
// waiting for something external to start a round.
void RetryFailedImageLoadsOnCurrentThread() {
    if (!g_initializedForThread) {
        return;
    }

    Wh_Log(L"Retrying failed image loads on current thread");

    auto& images = g_trackedImagesForThread.images;

    std::erase_if(images,
                  [](const auto& tracked) { return !tracked->target.get(); });

    // Copy the entries before iterating: a retry can raise image events, and
    // their handlers modify the entries.
    std::vector<std::shared_ptr<TrackedImage>> snapshot(images.begin(),
                                                        images.end());

    ULONGLONG tick = GetTickCount64();

    ULONGLONG nextRoundDelay = 0;
    auto armNextRoundIn = [&nextRoundDelay](ULONGLONG delay) {
        if (!nextRoundDelay || delay < nextRoundDelay) {
            nextRoundDelay = delay;
        }
    };

    for (const auto& tracked : snapshot) {
        if (tracked->loaded) {
            continue;
        }

        ULONGLONG remaining = 0;

        if (tracked->lastRetryTick) {
            ULONGLONG sinceLastRetry = tick - tracked->lastRetryTick;
            if (sinceLastRetry >= kImageRetryMaxDelayMs) {
                tracked->retryCount = 0;
            } else {
                ULONGLONG delay = ImageRetryDelayMs(tracked->retryCount);
                if (sinceLastRetry < delay) {
                    remaining = delay - sinceLastRetry;
                }
            }
        }

        // An image which ran out of attempts is left to a network status
        // change, which is what the count starting over is for.
        if (tracked->retryCount >= kImageRetryMaxCount) {
            continue;
        }

        if (remaining) {
            armNextRoundIn(remaining);
            continue;
        }

        StartImageRetry(tracked);

        if (tracked->retryCount < kImageRetryMaxCount) {
            armNextRoundIn(ImageRetryDelayMs(tracked->retryCount));
        }
    }

    if (nextRoundDelay) {
        ScheduleImageLoadRetryOnCurrentThread(nextRoundDelay,
                                              /*reschedule=*/false);
    }
}

// Runs a retry round in `delayMs`. A round which is already scheduled is kept
// if it's due sooner, unless `reschedule` moves it to the new time.
void ScheduleImageLoadRetryOnCurrentThread(ULONGLONG delayMs, bool reschedule) {
    if (!g_initializedForThread) {
        return;
    }

    auto& state = g_trackedImagesForThread;

    ULONGLONG dueTick = GetTickCount64() + delayMs;

    if (!reschedule && state.retryDueTick && state.retryDueTick <= dueTick) {
        return;
    }

    try {
        if (!state.retryTimer) {
            if (!state.dispatcher) {
                return;
            }

            state.retryTimer = state.dispatcher.CreateTimer();
            state.retryTimer.IsRepeating(false);
            state.retryTimerTickRevoker = state.retryTimer.Tick(
                winrt::auto_revoke,
                [](winrt::Microsoft::UI::Dispatching::
                       DispatcherQueueTimer const&,
                   winrt::Windows::Foundation::IInspectable const&) {
                    g_trackedImagesForThread.retryDueTick = 0;
                    RetryFailedImageLoadsOnCurrentThread();
                });
        }

        state.retryTimer.Stop();
        state.retryTimer.Interval(std::chrono::milliseconds{delayMs});
        state.retryTimer.Start();
        state.retryDueTick = dueTick;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

// Counts a callback which is about to be handed to code outside the mod for as
// long as the returned reference is alive, so that StopImageLoadRetries waits
// for it whether it ends up running or being dropped. Null once the retries
// have been stopped, which is the caller's cue not to hand it over at all.
std::shared_ptr<void> TrackImageRetryCallback() {
    {
        std::lock_guard<std::mutex> lock(g_imageRetryMutex);

        if (!g_imageRetryActive) {
            return nullptr;
        }

        g_imageRetryPendingCallbacks++;
    }

    // The pointer is only a non-null tag; the deleter is what the reference is
    // for, and it runs whether the shared_ptr is destroyed or its construction
    // throws.
    return std::shared_ptr<void>(&g_imageRetryPendingCallbacks, [](void*) {
        {
            std::lock_guard<std::mutex> lock(g_imageRetryMutex);

            g_imageRetryPendingCallbacks--;
        }

        g_imageRetryPendingCallbacksCv.notify_all();
    });
}

void ScheduleImageLoadRetryOnAllUiThreads() {
    // Losing connectivity raises a network status event just like gaining it
    // does, and there's nothing to retry with no internet access.
    if (!HasInternetAccess()) {
        Wh_Log(L"No internet access, not retrying image loads");
        return;
    }

    std::vector<winrt::Microsoft::UI::Dispatching::DispatcherQueue> dispatchers;
    {
        std::lock_guard<std::mutex> lock(g_imageRetryMutex);

        if (!g_imageRetryActive) {
            return;
        }

        for (auto& weakDispatcher : g_imageRetryDispatchers) {
            if (auto dispatcher = weakDispatcher.get()) {
                dispatchers.push_back(dispatcher);
            }
        }

        std::erase_if(g_imageRetryDispatchers, [](const auto& weakDispatcher) {
            return !weakDispatcher.get();
        });
    }

    for (auto& dispatcher : dispatchers) {
        auto callbackRef = TrackImageRetryCallback();
        if (!callbackRef) {
            return;
        }

        try {
            dispatcher.TryEnqueue([callbackRef]() {
                ScheduleImageLoadRetryOnCurrentThread(kNetworkChangeDebounceMs,
                                                      /*reschedule=*/true);
            });
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error dispatching retry to UI thread %08X: %s", ex.code(),
                   ex.message().c_str());
        }
    }
}

void OnNetworkStatusChanged(
    winrt::Windows::Foundation::IInspectable const& sender) {
    Wh_Log(L">");

    // Removing the handler doesn't wait for an invocation which is already in
    // flight, so this one counts itself instead.
    auto callbackRef = TrackImageRetryCallback();
    if (!callbackRef) {
        return;
    }

    // Runs on a Windows Runtime thread pool thread, where the connectivity
    // query is allowed and doesn't hold up a UI thread.
    ScheduleImageLoadRetryOnAllUiThreads();
}

// Must not be called with g_imageRetryMutex held.
winrt::event_token RegisterNetworkStatusChangedHandler() {
    try {
        auto token = winrt::Windows::Networking::Connectivity::
            NetworkInformation::NetworkStatusChanged(OnNetworkStatusChanged);
        Wh_Log(L"Registered global network status change handler");
        return token;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error registering network status handler %08X: %s", ex.code(),
               ex.message().c_str());
        return {};
    }
}

// Must not be called with g_imageRetryMutex held.
void UnregisterNetworkStatusChangedHandler(winrt::event_token token) {
    try {
        winrt::Windows::Networking::Connectivity::NetworkInformation::
            NetworkStatusChanged(token);
        Wh_Log(L"Unregistered global network status change handler");
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error unregistering network status handler %08X: %s",
               ex.code(), ex.message().c_str());
    }
}

void StopImageLoadRetries() {
    winrt::event_token token;

    {
        std::lock_guard<std::mutex> lock(g_imageRetryMutex);

        // Makes any handler which acquires the mutex from here on return
        // early, which is what stops the retries. Removing the handler only
        // stops further invocations.
        g_imageRetryActive = false;

        token = g_networkStatusChangedToken;
        g_networkStatusChangedToken = {};

        g_imageRetryDispatchers.clear();
    }

    if (token) {
        UnregisterNetworkStatusChangedHandler(token);
    }

    // The module is freed once the mod is uninitialized, so the callbacks which
    // are already on their way into it are let through first. What they wait on
    // is a connectivity query and a dispatcher pass of a UI thread, and the
    // uninitialization which follows depends on those threads running anyway.
    std::unique_lock<std::mutex> lock(g_imageRetryMutex);
    g_imageRetryPendingCallbacksCv.wait(
        lock, [] { return g_imageRetryPendingCallbacks == 0; });
}

// Drops the calling thread from the dispatcher registry, and stops the retries
// altogether once the last thread is out of it.
void StopImageLoadRetriesForCurrentThread() {
    auto dispatcher = g_trackedImagesForThread.dispatcher;
    if (!dispatcher) {
        return;
    }

    g_trackedImagesForThread.dispatcher = nullptr;

    winrt::event_token token;

    {
        std::lock_guard<std::mutex> lock(g_imageRetryMutex);

        std::erase_if(g_imageRetryDispatchers, [&dispatcher](
                                                   const auto& weakDispatcher) {
            auto registeredDispatcher = weakDispatcher.get();
            return !registeredDispatcher || registeredDispatcher == dispatcher;
        });

        if (!g_imageRetryDispatchers.empty()) {
            return;
        }

        // What StopImageLoadRetries does, kept under the lock which found the
        // registry empty so that a thread which registers in between isn't
        // stopped as well.
        g_imageRetryActive = false;

        token = g_networkStatusChangedToken;
        g_networkStatusChangedToken = {};
    }

    if (token) {
        UnregisterNetworkStatusChangedHandler(token);
    }
}

void SetupImageTracking(DependencyObject const& target,
                        DependencyProperty const& sourceProperty,
                        Media::Imaging::BitmapImage const& bitmapImage,
                        winrt::Windows::Foundation::Uri const& uri) {
    auto& images = g_trackedImagesForThread.images;

    std::erase_if(images,
                  [](const auto& tracked) { return !tracked->target.get(); });

    auto it = std::find_if(images.begin(), images.end(),
                           [&target](const auto& tracked) {
                               if (auto trackedTarget = tracked->target.get()) {
                                   return trackedTarget == target;
                               }
                               return false;
                           });

    if (it != images.end()) {
        // Resolved style values are cached, so the same source object is
        // applied to many targets and reapplied on every visual state change.
        // Keep the load state which was collected so far unless the source
        // changed.
        if ((*it)->uri.Equals(uri)) {
            RestoreRejectedImageSource(*it, bitmapImage);

            // The value being applied takes over from whatever a retry has put
            // there, so it's what a load failure is judged by.
            (*it)->usingCache = !bitmapImage.UriSource().Equals(uri);
            return;
        }

        images.erase(it);
    }

    Wh_Log(L"Tracking %s with remote image source: %s",
           winrt::get_class_name(target).c_str(), uri.RawUri().c_str());

    auto tracked = std::make_shared<TrackedImage>();
    tracked->target = winrt::make_weak(target);
    tracked->sourceProperty = sourceProperty;
    tracked->uri = uri;
    tracked->url = std::wstring(uri.RawUri());
    tracked->cachePath = ImageCachePath(tracked->url);

    if (!tracked->cachePath.empty()) {
        QueueImageCacheSweep();
    }

    try {
        tracked->decodePixelWidth = bitmapImage.DecodePixelWidth();
        tracked->decodePixelHeight = bitmapImage.DecodePixelHeight();
        tracked->decodePixelType = bitmapImage.DecodePixelType();
        tracked->createOptions = bitmapImage.CreateOptions();
        tracked->autoPlay = bitmapImage.AutoPlay();
        // A load which completed before tracking started raises no further
        // event, so the decoded size is what tells an image that's there from
        // one that isn't. An image which is still loading counts as missing,
        // which at worst costs a redundant download.
        tracked->loaded = bitmapImage.PixelWidth() != 0;

        // The cached file when there is one, and a download asked for when
        // there isn't.
        auto sourceUri = ImageSourceUri(tracked);

        // Assigned to the BitmapImage the style declared rather than to a new
        // object, so that the value the element customization state knows stays
        // the one which is applied and nothing looks like an external change.
        // Only while the image isn't showing anything: swapping a source which
        // is would blank its targets for the length of a load, and the file is
        // there for the next process either way.
        if (!tracked->loaded && !sourceUri.Equals(bitmapImage.UriSource())) {
            bool fromCache = !sourceUri.Equals(uri);
            Wh_Log(L"Loading %s image for: %s",
                   fromCache ? L"cached" : L"remote", tracked->url.c_str());

            // Recorded before the substitution, so that a source which was
            // pointed at a cached file is never one no entry can be recovered
            // from.
            if (fromCache) {
                g_imageCacheUriRemotes.insert_or_assign(
                    std::wstring(sourceUri.RawUri()), uri);
            }

            bitmapImage.UriSource(sourceUri);
        }

        tracked->usingCache = !bitmapImage.UriSource().Equals(uri);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    std::weak_ptr<TrackedImage> trackedWeak = tracked;

    auto onImageFailed = [trackedWeak](
                             winrt::Windows::Foundation::IInspectable const&,
                             ExceptionRoutedEventArgs const& e) {
        auto tracked = trackedWeak.lock();
        if (!tracked) {
            return;
        }

        Wh_Log(L"Image load failed for: %s, error: %s", tracked->url.c_str(),
               e.ErrorMessage().c_str());

        tracked->loaded = false;

        // A cached file which doesn't decode is dropped, taking every image the
        // URL is for back to the remote address and fetching the file once
        // more, in case it was left incomplete, for the next process. The
        // declared source is pointed back at the remote address by the next
        // apply, rather than from here, so that a source isn't swapped from
        // within the event which reports how loading it went.
        if (tracked->usingCache) {
            Wh_Log(L"Dropping the cached image which failed to load");

            tracked->usingCache = false;
            RejectImageCache(tracked->url);

            std::error_code ec;
            std::filesystem::remove(tracked->cachePath, ec);

            QueueImageDownload(tracked->url);
        }

        // Waiting for the base delay coalesces the failures of a batch of
        // images into a single round, and keeps the retries off the burst of
        // requests the failures came from.
        ScheduleImageLoadRetryOnCurrentThread(kImageRetryBaseDelayMs,
                                              /*reschedule=*/false);
    };

    auto onImageOpened = [trackedWeak](
                             winrt::Windows::Foundation::IInspectable const&,
                             RoutedEventArgs const&) {
        auto tracked = trackedWeak.lock();
        if (!tracked) {
            return;
        }

        Wh_Log(L"Image loaded for: %s", tracked->url.c_str());

        tracked->loaded = true;
        tracked->retryCount = 0;
        tracked->lastRetryTick = 0;
    };

    if (auto brush = target.try_as<Media::ImageBrush>()) {
        tracked->brushImageFailedRevoker =
            brush.ImageFailed(winrt::auto_revoke, onImageFailed);
        tracked->brushImageOpenedRevoker =
            brush.ImageOpened(winrt::auto_revoke, onImageOpened);
    } else if (auto image = target.try_as<Controls::Image>()) {
        tracked->elementImageFailedRevoker =
            image.ImageFailed(winrt::auto_revoke, onImageFailed);
        tracked->elementImageOpenedRevoker =
            image.ImageOpened(winrt::auto_revoke, onImageOpened);
    }

    images.push_back(std::move(tracked));

    bool registerHandler = false;

    {
        std::lock_guard<std::mutex> lock(g_imageRetryMutex);

        g_imageRetryActive = true;

        if (!g_trackedImagesForThread.dispatcher) {
            try {
                auto dispatcher = winrt::Microsoft::UI::Dispatching::
                    DispatcherQueue::GetForCurrentThread();
                if (dispatcher) {
                    g_trackedImagesForThread.dispatcher = dispatcher;
                    g_imageRetryDispatchers.push_back(
                        winrt::make_weak(dispatcher));
                    Wh_Log(
                        L"Registered UI thread dispatcher for network retry");
                }
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error getting dispatcher for current thread %08X: %s",
                       ex.code(), ex.message().c_str());
            }
        }

        if (!g_networkStatusChangedToken &&
            !g_networkStatusChangedRegistering) {
            g_networkStatusChangedRegistering = true;
            registerHandler = true;
        }
    }

    if (!registerHandler) {
        return;
    }

    winrt::event_token token = RegisterNetworkStatusChangedHandler();

    bool stopped;

    {
        std::lock_guard<std::mutex> lock(g_imageRetryMutex);

        g_networkStatusChangedRegistering = false;

        stopped = !g_imageRetryActive;
        if (!stopped) {
            g_networkStatusChangedToken = token;
        }
    }

    // StopImageLoadRetries ran while the handler was being registered, so it
    // found no token to remove.
    if (stopped && token) {
        UnregisterNetworkStatusChangedHandler(token);
    }
}

// Tracks the target if the image source is a remote URL, which can fail to
// load and is worth caching.
void TrackIfRemoteImageSource(
    DependencyObject const& target,
    DependencyProperty const& sourceProperty,
    winrt::Windows::Foundation::IInspectable const& imageSource) {
    auto bitmapImage = imageSource.try_as<Media::Imaging::BitmapImage>();
    if (!bitmapImage) {
        return;
    }

    auto uri = bitmapImage.UriSource();
    if (!uri) {
        return;
    }

    auto scheme = uri.SchemeName();
    if (scheme == L"file") {
        // A cached file which was substituted for its remote address, by which
        // the entry is identified. A style value is shared by many targets, so
        // the substitution a previous target's entry made is what the rest of
        // them are given.
        auto it = g_imageCacheUriRemotes.find(std::wstring(uri.RawUri()));
        if (it == g_imageCacheUriRemotes.end()) {
            return;
        }

        uri = it->second;
    } else if (scheme != L"http" && scheme != L"https") {
        return;
    }

    SetupImageTracking(target, sourceProperty, bitmapImage, uri);
}

// Returns the value written, UnsetValue for a clear, so that callers can
// record what they applied. Reading it back is not an option: while a visual
// state has a setter on the property, ReadLocalValue keeps reporting the value
// from before that state rather than the one written.
winrt::Windows::Foundation::IInspectable SetOrClearValue(
    DependencyObject elementDo,
    DependencyProperty property,
    const PropertyOverrideValue& overrideValue,
    bool initialApply = false) {
    winrt::Windows::Foundation::IInspectable value;
    if (auto* inspectable =
            std::get_if<winrt::Windows::Foundation::IInspectable>(
                &overrideValue)) {
        value = *inspectable;
    } else if (auto* blurBrushParams =
                   std::get_if<XamlBlurBrushParams>(&overrideValue)) {
        if (auto uiElement = elementDo.try_as<UIElement>()) {
            value = winrt::make<XamlBlurBrush>(
                uiElement, blurBrushParams->blurAmount, blurBrushParams->tint,
                blurBrushParams->tintOpacity,
                winrt::hstring(blurBrushParams->tintThemeResourceKey),
                blurBrushParams->tintLuminosityOpacity,
                blurBrushParams->tintSaturation, blurBrushParams->noiseOpacity,
                blurBrushParams->noiseDensity, blurBrushParams->fallbackColor,
                winrt::hstring(blurBrushParams->fallbackThemeResourceKey));
        } else {
            Wh_Log(L"Can't get UIElement for blur brush");
            return nullptr;
        }
    } else {
        Wh_Log(L"Unsupported override value");
        return nullptr;
    }

    if (value == DependencyProperty::UnsetValue()) {
        Wh_Log(L"Clearing property value");
        try {
            elementDo.ClearValue(property);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
        return value;
    }

    Wh_Log(L"Setting property value %s",
           value ? winrt::get_class_name(value).c_str() : L"(null)");

    // Track a remote image source for retry on network reconnection. A style
    // can declare one as the ImageBrush a property is set to (e.g. Background),
    // as the ImageSource of an ImageBrush it targets, or as the Source of an
    // Image element.
    if (auto imageBrush = value.try_as<Media::ImageBrush>()) {
        TrackIfRemoteImageSource(imageBrush,
                                 Media::ImageBrush::ImageSourceProperty(),
                                 imageBrush.ImageSource());
    } else if (auto imageBrush = elementDo.try_as<Media::ImageBrush>()) {
        if (property == Media::ImageBrush::ImageSourceProperty()) {
            TrackIfRemoteImageSource(imageBrush, property, value);
        }
    } else if (auto image = elementDo.try_as<Controls::Image>()) {
        if (property == Controls::Image::SourceProperty()) {
            TrackIfRemoteImageSource(image, property, value);
        }
    }

    // This might fail. See `ReadLocalValueWithWorkaround` for an example (which
    // we now handle but there might be other cases).
    try {
        // `setter.Value()` returns font weight as an int. Using it with
        // `SetValue` results in the following error: 0x80004002 (No such
        // interface supported). Box it as `Windows.UI.Text.FontWeight` as a
        // workaround.
        if (property == Controls::TextBlock::FontWeightProperty() ||
            property == Controls::Control::FontWeightProperty() ||
            property == Controls::RichTextBlock::FontWeightProperty() ||
            property == Controls::FontIcon::FontWeightProperty() ||
            property == Controls::FontIconSource::FontWeightProperty() ||
            property == Controls::ContentPresenter::FontWeightProperty()) {
            auto valueInt = value.try_as<int>();
            if (valueInt && *valueInt >= std::numeric_limits<uint16_t>::min() &&
                *valueInt <= std::numeric_limits<uint16_t>::max()) {
                value = winrt::box_value(winrt::Windows::UI::Text::FontWeight{
                    static_cast<uint16_t>(*valueInt)});
            }
        }

        // Grid ColumnDefinitions/RowDefinitions hold DependencyObjects
        // (ColumnDefinition/RowDefinition) that the layout engine writes
        // ActualWidth/ActualHeight back into. The resolved value is parsed once
        // and cached, so applying it to more than one grid - e.g. a taskbar per
        // monitor, all sharing one UI thread - would set the same collection on
        // each, and one monitor's column sizes would then leak onto another's.
        // Give each element a private copy. The scratch Grid owns the fresh
        // collection until SetValue reassigns ownership to the target, so it's
        // kept alive through the SetValue call below.
        Controls::Grid definitionsCloneOwner{nullptr};
        if (auto sourceColumns =
                value.try_as<Controls::ColumnDefinitionCollection>()) {
            definitionsCloneOwner = Controls::Grid{};
            auto clonedColumns = definitionsCloneOwner.ColumnDefinitions();
            for (auto const& column : sourceColumns) {
                Controls::ColumnDefinition clonedColumn;
                clonedColumn.Width(column.Width());
                clonedColumn.MinWidth(column.MinWidth());
                clonedColumn.MaxWidth(column.MaxWidth());
                clonedColumns.Append(clonedColumn);
            }
            value = clonedColumns;
        } else if (auto sourceRows =
                       value.try_as<Controls::RowDefinitionCollection>()) {
            definitionsCloneOwner = Controls::Grid{};
            auto clonedRows = definitionsCloneOwner.RowDefinitions();
            for (auto const& row : sourceRows) {
                Controls::RowDefinition clonedRow;
                clonedRow.Height(row.Height());
                clonedRow.MinHeight(row.MinHeight());
                clonedRow.MaxHeight(row.MaxHeight());
                clonedRows.Append(clonedRow);
            }
            value = clonedRows;
        }

        elementDo.SetValue(property, value);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    return value;
}

// https://stackoverflow.com/a/5665377
std::wstring EscapeXmlAttribute(std::wstring_view data) {
    std::wstring buffer;
    buffer.reserve(data.size());
    for (const auto c : data) {
        switch (c) {
            case '&':
                buffer.append(L"&amp;");
                break;
            case '\"':
                buffer.append(L"&quot;");
                break;
            // case '\'':
            //     buffer.append(L"&apos;");
            //     break;
            case '<':
                buffer.append(L"&lt;");
                break;
            case '>':
                buffer.append(L"&gt;");
                break;
            default:
                buffer.push_back(c);
                break;
        }
    }

    return buffer;
}

// https://stackoverflow.com/a/54364173
std::wstring_view TrimStringView(std::wstring_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
    s.remove_suffix(
        std::min(s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
    return s;
}

// https://stackoverflow.com/a/46931770
std::vector<std::wstring_view> SplitStringView(std::wstring_view s,
                                               std::wstring_view delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::wstring_view token;
    std::vector<std::wstring_view> res;

    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

std::optional<PropertyOverrideValue> ParseNonXamlPropertyOverrideValue(
    std::wstring_view stringValue) {
    // Example:
    // <WindhawkBlur BlurAmount="10" TintColor="#FFFF0000"/>

    auto substr = TrimStringView(stringValue);

    constexpr auto kWindhawkBlurPrefix = L"<WindhawkBlur "sv;
    if (!substr.starts_with(kWindhawkBlurPrefix)) {
        return std::nullopt;
    }
    Wh_Log(L"%.*s", static_cast<int>(substr.length()), substr.data());
    substr = substr.substr(std::size(kWindhawkBlurPrefix));

    constexpr auto kWindhawkBlurSuffix = L"/>"sv;
    if (!substr.ends_with(kWindhawkBlurSuffix)) {
        throw std::runtime_error("WindhawkBlur: Bad suffix");
    }
    substr = substr.substr(0, substr.size() - std::size(kWindhawkBlurSuffix));

    bool pendingTintColorThemeResource = false;
    bool pendingFallbackColorThemeResource = false;
    std::wstring tintThemeResourceKey;
    std::wstring fallbackThemeResourceKey;
    winrt::Windows::UI::Color tint{};
    std::optional<winrt::Windows::UI::Color> fallbackColor;
    float tintOpacity = std::numeric_limits<float>::quiet_NaN();
    float tintLuminosityOpacity = std::numeric_limits<float>::quiet_NaN();
    float tintSaturation = std::numeric_limits<float>::quiet_NaN();
    float noiseOpacity = std::numeric_limits<float>::quiet_NaN();
    float noiseDensity = std::numeric_limits<float>::quiet_NaN();
    float blurAmount = 0;

    constexpr auto kTintColorThemeResourcePrefix =
        L"TintColor=\"{ThemeResource"sv;
    constexpr auto kTintColorThemeResourceSuffix = L"}\""sv;
    constexpr auto kTintColorPrefix = L"TintColor=\"#"sv;
    constexpr auto kTintOpacityPrefix = L"TintOpacity=\""sv;
    constexpr auto kTintLuminosityOpacityPrefix = L"TintLuminosityOpacity=\""sv;
    constexpr auto kTintSaturationPrefix = L"TintSaturation=\""sv;
    constexpr auto kNoiseOpacityPrefix = L"NoiseOpacity=\""sv;
    constexpr auto kNoiseDensityPrefix = L"NoiseDensity=\""sv;
    constexpr auto kBlurAmountPrefix = L"BlurAmount=\""sv;
    constexpr auto kFallbackColorThemeResourcePrefix =
        L"FallbackColor=\"{ThemeResource"sv;
    constexpr auto kFallbackColorThemeResourceSuffix = L"}\""sv;
    constexpr auto kFallbackColorPrefix = L"FallbackColor=\"#"sv;
    for (const auto prop : SplitStringView(substr, L" ")) {
        const auto propSubstr = TrimStringView(prop);
        if (propSubstr.empty()) {
            continue;
        }

        Wh_Log(L"  %.*s", static_cast<int>(propSubstr.length()),
               propSubstr.data());

        if (pendingTintColorThemeResource) {
            if (!propSubstr.ends_with(kTintColorThemeResourceSuffix)) {
                throw std::runtime_error(
                    "WindhawkBlur: Invalid TintColor theme resource syntax");
            }

            pendingTintColorThemeResource = false;

            tintThemeResourceKey = propSubstr.substr(
                0,
                propSubstr.size() - std::size(kTintColorThemeResourceSuffix));

            continue;
        }

        if (pendingFallbackColorThemeResource) {
            if (!propSubstr.ends_with(kFallbackColorThemeResourceSuffix)) {
                throw std::runtime_error(
                    "WindhawkBlur: Invalid FallbackColor theme resource "
                    "syntax");
            }

            pendingFallbackColorThemeResource = false;

            fallbackThemeResourceKey = propSubstr.substr(
                0, propSubstr.size() -
                       std::size(kFallbackColorThemeResourceSuffix));

            continue;
        }

        if (propSubstr == kTintColorThemeResourcePrefix) {
            pendingTintColorThemeResource = true;
            continue;
        }

        if (propSubstr == kFallbackColorThemeResourcePrefix) {
            pendingFallbackColorThemeResource = true;
            continue;
        }

        if (propSubstr.starts_with(kTintColorPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kTintColorPrefix),
                propSubstr.size() - std::size(kTintColorPrefix) - 1);

            bool hasAlpha;
            switch (valStr.size()) {
                case 6:
                    hasAlpha = false;
                    break;
                case 8:
                    hasAlpha = true;
                    break;
                default:
                    throw std::runtime_error(
                        "WindhawkBlur: Unsupported TintColor value");
            }

            auto valNum = std::stoul(std::wstring(valStr), nullptr, 16);
            uint8_t a = hasAlpha ? HIBYTE(HIWORD(valNum)) : 255;
            uint8_t r = LOBYTE(HIWORD(valNum));
            uint8_t g = HIBYTE(LOWORD(valNum));
            uint8_t b = LOBYTE(LOWORD(valNum));
            tint = {a, r, g, b};
            continue;
        }

        if (propSubstr.starts_with(kFallbackColorPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kFallbackColorPrefix),
                propSubstr.size() - std::size(kFallbackColorPrefix) - 1);

            bool hasAlpha;
            switch (valStr.size()) {
                case 6:
                    hasAlpha = false;
                    break;
                case 8:
                    hasAlpha = true;
                    break;
                default:
                    throw std::runtime_error(
                        "WindhawkBlur: Unsupported FallbackColor value");
            }

            auto valNum = std::stoul(std::wstring(valStr), nullptr, 16);
            uint8_t a = hasAlpha ? HIBYTE(HIWORD(valNum)) : 255;
            uint8_t r = LOBYTE(HIWORD(valNum));
            uint8_t g = HIBYTE(LOWORD(valNum));
            uint8_t b = LOBYTE(LOWORD(valNum));
            fallbackColor = winrt::Windows::UI::Color{a, r, g, b};
            continue;
        }

        if (propSubstr.starts_with(kTintOpacityPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kTintOpacityPrefix),
                propSubstr.size() - std::size(kTintOpacityPrefix) - 1);
            tintOpacity = std::stof(std::wstring(valStr));
            continue;
        }

        if (propSubstr.starts_with(kTintLuminosityOpacityPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kTintLuminosityOpacityPrefix),
                propSubstr.size() - std::size(kTintLuminosityOpacityPrefix) -
                    1);
            tintLuminosityOpacity = std::stof(std::wstring(valStr));
            continue;
        }

        if (propSubstr.starts_with(kTintSaturationPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kTintSaturationPrefix),
                propSubstr.size() - std::size(kTintSaturationPrefix) - 1);
            tintSaturation = std::stof(std::wstring(valStr));
            continue;
        }

        if (propSubstr.starts_with(kNoiseOpacityPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kNoiseOpacityPrefix),
                propSubstr.size() - std::size(kNoiseOpacityPrefix) - 1);
            noiseOpacity = std::stof(std::wstring(valStr));
            continue;
        }

        if (propSubstr.starts_with(kNoiseDensityPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kNoiseDensityPrefix),
                propSubstr.size() - std::size(kNoiseDensityPrefix) - 1);
            noiseDensity = std::stof(std::wstring(valStr));
            continue;
        }

        if (propSubstr.starts_with(kBlurAmountPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kBlurAmountPrefix),
                propSubstr.size() - std::size(kBlurAmountPrefix) - 1);
            blurAmount = std::stof(std::wstring(valStr));
            continue;
        }

        throw std::runtime_error("WindhawkBlur: Bad property");
    }

    if (pendingTintColorThemeResource) {
        throw std::runtime_error(
            "WindhawkBlur: Unterminated TintColor theme resource");
    }

    if (pendingFallbackColorThemeResource) {
        throw std::runtime_error(
            "WindhawkBlur: Unterminated FallbackColor theme resource");
    }

    if (!std::isnan(tintOpacity)) {
        if (tintOpacity < 0.0f) {
            tintOpacity = 0.0f;
        } else if (tintOpacity > 1.0f) {
            tintOpacity = 1.0f;
        }

        tint.A = static_cast<uint8_t>(tintOpacity * 255.0f);
    }

    return XamlBlurBrushParams{
        .blurAmount = blurAmount,
        .tint = tint,
        .tintOpacity =
            !std::isnan(tintOpacity) ? std::optional(tint.A) : std::nullopt,
        .tintThemeResourceKey = std::move(tintThemeResourceKey),
        .tintLuminosityOpacity = !std::isnan(tintLuminosityOpacity)
                                     ? std::optional(tintLuminosityOpacity)
                                     : std::nullopt,
        .tintSaturation = !std::isnan(tintSaturation)
                              ? std::optional(tintSaturation)
                              : std::nullopt,
        .noiseOpacity = !std::isnan(noiseOpacity) ? std::optional(noiseOpacity)
                                                  : std::nullopt,
        .noiseDensity = !std::isnan(noiseDensity) ? std::optional(noiseDensity)
                                                  : std::nullopt,
        .fallbackColor = fallbackColor,
        .fallbackThemeResourceKey = std::move(fallbackThemeResourceKey),
    };
}

Style GetStyleFromXamlSetters(const std::wstring_view type,
                              const std::wstring_view xamlStyleSetters) {
    std::wstring xaml =
        LR"(<ResourceDictionary
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    xmlns:muxc="using:Microsoft.UI.Xaml.Controls")";

    if (auto pos = type.rfind('.'); pos != type.npos) {
        auto typeNamespace = std::wstring_view(type).substr(0, pos);
        auto typeName = std::wstring_view(type).substr(pos + 1);

        xaml += L"\n    xmlns:windhawkstyler=\"using:";
        xaml += EscapeXmlAttribute(typeNamespace);
        xaml +=
            L"\">\n"
            L"    <Style TargetType=\"windhawkstyler:";
        xaml += EscapeXmlAttribute(typeName);
        xaml += L"\">\n";
    } else {
        xaml +=
            L">\n"
            L"    <Style TargetType=\"";
        xaml += EscapeXmlAttribute(type);
        xaml += L"\">\n";
    }

    xaml += xamlStyleSetters;

    xaml +=
        L"    </Style>\n"
        L"</ResourceDictionary>";

    Wh_Log(L"======================================== XAML:");
    std::wstringstream ss(xaml);
    std::wstring line;
    while (std::getline(ss, line, L'\n')) {
        Wh_Log(L"%s", line.c_str());
    }
    Wh_Log(L"========================================");

    auto resourceDictionary =
        Markup::XamlReader::Load(xaml).as<ResourceDictionary>();

    auto [styleKey, styleInspectable] = resourceDictionary.First().Current();
    return styleInspectable.as<Style>();
}

const ResolvedRules& GetResolvedPropertyOverrides(
    const std::wstring_view type,
    PropertyOverridesMaybeUnresolved* propertyOverridesMaybeUnresolved) {
    if (const auto* resolved =
            std::get_if<ResolvedRules>(propertyOverridesMaybeUnresolved)) {
        return *resolved;
    }

    ResolvedRules resolved;

    try {
        const auto& unresolved =
            std::get<UnresolvedRules>(*propertyOverridesMaybeUnresolved);
        const auto& valueRules = unresolved.valueRules;
        const auto& captureRules = unresolved.captureRules;

        if (!valueRules.empty() || !captureRules.empty()) {
            // Build a single XAML <Style> with one <Setter> per rule. Setters
            // for value rules come first, followed by one per capture rule.
            // Dynamic / capture rules emit a placeholder `{x:Null}` value -- we
            // only need the resolved DependencyProperty from those setters; the
            // value is computed elsewhere (per apply for dynamic, never for
            // captures).
            std::wstring xaml;

            std::vector<std::optional<PropertyOverrideValue>>
                propertyOverrideValues;
            propertyOverrideValues.reserve(valueRules.size());

            for (const auto& rule : valueRules) {
                const bool isDynamic = rule.isDynamic();

                propertyOverrideValues.push_back(
                    !isDynamic && rule.isXamlValue
                        ? ParseNonXamlPropertyOverrideValue(rule.value)
                        : std::nullopt);

                xaml += L"        <Setter Property=\"";
                xaml += EscapeXmlAttribute(rule.propertyName);
                xaml += L"\"";
                if (isDynamic || propertyOverrideValues.back() ||
                    (rule.isXamlValue && rule.value.empty())) {
                    xaml += L" Value=\"{x:Null}\" />\n";
                } else if (!rule.isXamlValue) {
                    xaml += L" Value=\"";
                    xaml += EscapeXmlAttribute(rule.value);
                    xaml += L"\" />\n";
                } else {
                    xaml +=
                        L">\n"
                        L"            <Setter.Value>\n";
                    xaml += rule.value;
                    xaml +=
                        L"\n"
                        L"            </Setter.Value>\n"
                        L"        </Setter>\n";
                }
            }

            for (const auto& rule : captureRules) {
                xaml += L"        <Setter Property=\"";
                xaml += EscapeXmlAttribute(rule.propertyName);
                xaml += L"\" Value=\"{x:Null}\" />\n";
            }

            auto style = GetStyleFromXamlSetters(type, xaml);

            uint32_t setterIndex = 0;
            for (size_t i = 0; i < valueRules.size(); i++, setterIndex++) {
                const auto& rule = valueRules[i];
                const auto setter =
                    style.Setters().GetAt(setterIndex).as<Setter>();
                auto property = setter.Property();
                if (rule.isDynamic()) {
                    resolved.propertyOverrides[property][rule.visualState] =
                        DynamicStyleTemplate{rule.propertyName, rule.value,
                                             rule.isXamlValue};
                    resolved.hasDynamicValues = true;
                } else {
                    resolved.propertyOverrides[property][rule.visualState] =
                        propertyOverrideValues[i].value_or(
                            rule.isXamlValue && rule.value.empty()
                                ? DependencyProperty::UnsetValue()
                                : setter.Value());
                }
            }

            for (const auto& rule : captureRules) {
                const auto setter =
                    style.Setters().GetAt(setterIndex++).as<Setter>();
                resolved.captures.push_back({setter.Property(), rule.varName});
            }
        }

        Wh_Log(L"%.*s: %zu override styles, %zu captures",
               static_cast<int>(type.length()), type.data(),
               resolved.propertyOverrides.size(), resolved.captures.size());
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const& ex) {
        Wh_Log(L"Error: %S", ex.what());
    }

    *propertyOverridesMaybeUnresolved = std::move(resolved);
    return std::get<ResolvedRules>(*propertyOverridesMaybeUnresolved);
}

// Resolve a single style rule's expanded textual value into a usable
// PropertyOverrideValue. Built for re-resolving dynamic `{{...}}` styles on
// every variable change; falls back to the same XAML-Setter parse trick used by
// the bulk resolver above. propertyName is the property whose XAML name should
// appear on the synthetic Setter (already known at apply time).
std::optional<PropertyOverrideValue> ResolveExpandedSinglePropertyValue(
    std::wstring_view type,
    std::wstring_view propertyName,
    std::wstring_view expandedValue,
    bool isXamlValue) {
    if (isXamlValue) {
        if (auto blur = ParseNonXamlPropertyOverrideValue(expandedValue)) {
            return *blur;
        }

        if (TrimStringView(expandedValue).empty()) {
            return PropertyOverrideValue{DependencyProperty::UnsetValue()};
        }
    }

    std::wstring xaml = L"        <Setter Property=\"";
    xaml += EscapeXmlAttribute(propertyName);
    xaml += L"\"";
    if (!isXamlValue) {
        xaml += L" Value=\"";
        xaml += EscapeXmlAttribute(expandedValue);
        xaml += L"\" />\n";
    } else {
        xaml +=
            L">\n"
            L"            <Setter.Value>\n";
        xaml += expandedValue;
        xaml +=
            L"\n"
            L"            </Setter.Value>\n"
            L"        </Setter>\n";
    }

    try {
        auto style = GetStyleFromXamlSetters(type, xaml);
        const auto setter = style.Setters().GetAt(0).as<Setter>();
        return PropertyOverrideValue{setter.Value()};
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const& ex) {
        Wh_Log(L"Error: %S", ex.what());
    }

    return std::nullopt;
}

const PropertyValues& GetResolvedPropertyValues(
    const std::wstring_view type,
    PropertyValuesMaybeUnresolved* propertyValuesMaybeUnresolved) {
    if (const auto* resolved =
            std::get_if<PropertyValues>(propertyValuesMaybeUnresolved)) {
        return *resolved;
    }

    PropertyValues propertyValues;

    try {
        const auto& propertyValuesStr =
            std::get<PropertyValuesUnresolved>(*propertyValuesMaybeUnresolved);
        if (!propertyValuesStr.empty()) {
            std::wstring xaml;

            for (const auto& [property, value] : propertyValuesStr) {
                xaml += L"        <Setter Property=\"";
                xaml += EscapeXmlAttribute(property);
                xaml += L"\" Value=\"";
                xaml += EscapeXmlAttribute(value);
                xaml += L"\" />\n";
            }

            auto style = GetStyleFromXamlSetters(type, xaml);

            for (size_t i = 0; i < propertyValuesStr.size(); i++) {
                const auto setter = style.Setters().GetAt(i).as<Setter>();
                propertyValues.push_back({
                    setter.Property(),
                    setter.Value(),
                });
            }
        }

        Wh_Log(L"%.*s: %zu matcher styles", static_cast<int>(type.length()),
               type.data(), propertyValues.size());
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const& ex) {
        Wh_Log(L"Error: %S", ex.what());
    }

    *propertyValuesMaybeUnresolved = std::move(propertyValues);
    return std::get<PropertyValues>(*propertyValuesMaybeUnresolved);
}

// https://stackoverflow.com/a/12835139
VisualStateGroup GetVisualStateGroup(FrameworkElement element,
                                     std::wstring_view visualStateGroupName) {
    auto list = VisualStateManager::GetVisualStateGroups(element);

    for (const auto& v : list) {
        if (v.Name() == visualStateGroupName) {
            return v;
        }
    }

    return nullptr;
}

// Locale-independent double formatter. Uses `std::to_chars` shortest round-trip
// representation so XAML always sees `.` as the decimal separator. Non-finite
// values use the spellings the XAML double converter accepts (case-sensitive;
// it rejects "-NaN").
std::wstring FormatDoubleInvariant(double d) {
    if (std::isnan(d)) {
        return L"NaN";
    }
    if (std::isinf(d)) {
        return d < 0 ? L"-Infinity" : L"Infinity";
    }
    char buf[64];
    auto [end, ec] = std::to_chars(buf, buf + std::size(buf), d);
    if (ec != std::errc{}) {
        return L"0";
    }
    return std::wstring(buf, end);
}

// Locale-independent double parser. Accepts an optional leading sign followed
// by a decimal fraction or exponent. Returns std::nullopt on partial / bad
// input.
std::optional<double> ParseDoubleInvariant(std::wstring_view sv) {
    std::string narrow;
    narrow.reserve(sv.size());
    for (auto c : sv) {
        if (c > 127) {
            return std::nullopt;
        }
        narrow.push_back(static_cast<char>(c));
    }
    double result = 0;
    auto* first = narrow.data();
    auto* last = first + narrow.size();
    auto [ptr, ec] = std::from_chars(first, last, result);
    if (ec != std::errc{} || ptr != last) {
        return std::nullopt;
    }
    return result;
}

using UnboxedPropertyValue = std::variant<std::wstring,
                                          bool,
                                          char16_t,
                                          uint8_t,
                                          int16_t,
                                          uint16_t,
                                          int32_t,
                                          uint32_t,
                                          int64_t,
                                          uint64_t,
                                          float,
                                          double>;

// Unwraps a boxed primitive into a typed primitive variant. Dispatches on
// IPropertyValue::Type(). Returns std::nullopt for non-primitive (opaque)
// values such as brushes or thicknesses.
std::optional<UnboxedPropertyValue> TryUnboxPropertyValue(
    winrt::Windows::Foundation::IInspectable const& value) {
    using winrt::Windows::Foundation::IPropertyValue;
    using winrt::Windows::Foundation::PropertyType;

    auto pv = value.try_as<IPropertyValue>();
    if (!pv) {
        return std::nullopt;
    }

    switch (pv.Type()) {
        case PropertyType::String:
            return UnboxedPropertyValue{std::wstring(pv.GetString())};
        case PropertyType::Boolean:
            return UnboxedPropertyValue{pv.GetBoolean()};
        case PropertyType::Char16:
            return UnboxedPropertyValue{pv.GetChar16()};
        case PropertyType::Double:
            return UnboxedPropertyValue{pv.GetDouble()};
        case PropertyType::Single:
            return UnboxedPropertyValue{pv.GetSingle()};
        case PropertyType::UInt8:
            return UnboxedPropertyValue{pv.GetUInt8()};
        case PropertyType::Int16:
            return UnboxedPropertyValue{pv.GetInt16()};
        case PropertyType::UInt16:
            return UnboxedPropertyValue{pv.GetUInt16()};
        case PropertyType::Int32:
            return UnboxedPropertyValue{pv.GetInt32()};
        case PropertyType::UInt32:
            return UnboxedPropertyValue{pv.GetUInt32()};
        case PropertyType::Int64:
            return UnboxedPropertyValue{pv.GetInt64()};
        case PropertyType::UInt64:
            return UnboxedPropertyValue{pv.GetUInt64()};
        case PropertyType::OtherType: {
            // Common for enums.
            if (auto intVal = value.try_as<int32_t>()) {
                return UnboxedPropertyValue{*intVal};
            }
            return std::nullopt;
        }
        default: {
            return std::nullopt;
        }
    }
}

// Invariant-formatted text form, suitable for XAML attribute use or diagnostic
// logs.
std::wstring FormatUnboxedPropertyValue(UnboxedPropertyValue const& v) {
    return std::visit(
        [](auto const& x) -> std::wstring {
            using T = std::decay_t<decltype(x)>;
            if constexpr (std::is_same_v<T, std::wstring>) {
                return x;
            } else if constexpr (std::is_same_v<T, bool>) {
                return x ? L"True" : L"False";
            } else if constexpr (std::is_same_v<T, char16_t>) {
                // Single-character text form so substitution emits the
                // character itself.
                return std::wstring(1, static_cast<wchar_t>(x));
            } else if constexpr (std::is_floating_point_v<T>) {
                return FormatDoubleInvariant(static_cast<double>(x));
            } else {
                return std::to_wstring(x);
            }
        },
        v);
}

// Numeric-as-double form, or std::nullopt if the value isn't numeric (i.e.
// holds a string).
std::optional<double> UnboxedPropertyValueAsNumeric(
    UnboxedPropertyValue const& v) {
    return std::visit(
        [](auto const& x) -> std::optional<double> {
            using T = std::decay_t<decltype(x)>;
            if constexpr (std::is_same_v<T, std::wstring>) {
                return std::nullopt;
            } else {
                return static_cast<double>(x);
            }
        },
        v);
}

bool TestElementMatcher(FrameworkElement element,
                        ElementMatcher& matcher,
                        VisualStateGroup* visualStateGroup,
                        PCWSTR fallbackClassName) {
    if (!matcher.type.empty() &&
        matcher.type != winrt::get_class_name(element) &&
        (!fallbackClassName || matcher.type != fallbackClassName)) {
        return false;
    }

    if (!matcher.name.empty() && matcher.name != element.Name()) {
        return false;
    }

    if (matcher.oneBasedIndex) {
        auto parent = Media::VisualTreeHelper::GetParent(element);
        if (!parent) {
            return false;
        }

        int index = matcher.oneBasedIndex - 1;
        if (index < 0 ||
            index >= Media::VisualTreeHelper::GetChildrenCount(parent) ||
            Media::VisualTreeHelper::GetChild(parent, index) != element) {
            return false;
        }
    }

    auto elementDo = element.as<DependencyObject>();

    for (const auto& propertyValue :
         GetResolvedPropertyValues(matcher.type, &matcher.propertyValues)) {
        const auto value =
            ReadLocalValueWithWorkaround(elementDo, propertyValue.first);
        if (!value) {
            Wh_Log(L"Null property value");
            return false;
        } else if (value == DependencyProperty::UnsetValue()) {
            return false;
        }

        auto expectedUnboxed = TryUnboxPropertyValue(propertyValue.second);
        auto valueUnboxed = TryUnboxPropertyValue(value);
        if (!expectedUnboxed || !valueUnboxed) {
            Wh_Log(L"Unsupported property class: %s",
                   winrt::get_class_name(value).c_str());
            return false;
        }

        if (*expectedUnboxed != *valueUnboxed) {
            return false;
        }
    }

    if (matcher.visualStateGroupName && visualStateGroup) {
        *visualStateGroup =
            GetVisualStateGroup(element, *matcher.visualStateGroupName);
    }

    return true;
}

// Aggregated resolved rules for an element. Value-rules are still bucketed by
// visual-state-group (each target's rules live under that target's @VSGName);
// captures are intentionally NOT per-VSG -- they are wired up once at element
// level (see SetUpCapturesForElement).
struct ElementResolvedRules {
    std::unordered_map<VisualStateGroup, PropertyOverrides> overridesPerVSG;
    std::vector<CaptureSpec> captures;
    bool hasDynamicValues = false;
};

ElementResolvedRules FindElementPropertyOverrides(FrameworkElement element,
                                                  PCWSTR fallbackClassName) {
    ElementResolvedRules result;
    std::unordered_set<DependencyProperty> propertiesAdded;
    std::unordered_set<std::wstring> capturesAdded;

    for (auto it = g_elementsCustomizationRules.rbegin();
         it != g_elementsCustomizationRules.rend(); ++it) {
        auto& override = *it;

        VisualStateGroup visualStateGroup = nullptr;

        if (!TestElementMatcher(element, override.elementMatcher,
                                &visualStateGroup, fallbackClassName)) {
            continue;
        }

        // Using iter.Parent() was sometimes returning null, so use
        // VisualTreeHelper::GetParent below instead.
        //
        // Recursive lambda so that '*' can backtrack: when a candidate match
        // for the wildcard's next matcher leads to a failure further up the
        // chain, retry with a farther ancestor.
        auto& parentMatchers = override.parentElementMatchers;
        auto matchParents = [&](auto& self, FrameworkElement iter,
                                size_t mi) -> bool {
            if (mi >= parentMatchers.size()) {
                return true;
            }

            auto& matcher = parentMatchers[mi];

            if (matcher.kind == ElementMatcher::Kind::Root) {
                if (Media::VisualTreeHelper::GetParent(iter)) {
                    return false;
                }

                return self(self, iter, mi + 1);
            }

            if (matcher.kind == ElementMatcher::Kind::Wildcard) {
                // '*' is always followed by an Element matcher (validated at
                // parse time). Walk up parents and try recursing for each
                // ancestor that matches the next matcher.
                auto& nextMatcher = parentMatchers[mi + 1];
                auto cur = iter;
                while (true) {
                    auto parent = Media::VisualTreeHelper::GetParent(cur)
                                      .try_as<FrameworkElement>();
                    if (!parent) {
                        return false;
                    }

                    cur = parent;
                    if (TestElementMatcher(cur, nextMatcher, &visualStateGroup,
                                           nullptr) &&
                        self(self, cur, mi + 2)) {
                        return true;
                    }
                }
            }

            auto parent = Media::VisualTreeHelper::GetParent(iter)
                              .try_as<FrameworkElement>();
            if (!parent) {
                return false;
            }

            if (!TestElementMatcher(parent, matcher, &visualStateGroup,
                                    nullptr)) {
                return false;
            }

            return self(self, parent, mi + 1);
        };

        if (!matchParents(matchParents, element, 0)) {
            continue;
        }

        const auto& resolvedRules = GetResolvedPropertyOverrides(
            override.elementMatcher.type, &override.propertyOverrides);

        result.hasDynamicValues |= resolvedRules.hasDynamicValues;

        auto& propertyOverridesForVSG =
            result.overridesPerVSG[visualStateGroup];
        for (const auto& [property, valuesPerVisualState] :
             resolvedRules.propertyOverrides) {
            bool propertyInserted = propertiesAdded.insert(property).second;
            if (!propertyInserted) {
                continue;
            }

            auto& propertyOverrides = propertyOverridesForVSG[property];
            for (const auto& [visualState, value] : valuesPerVisualState) {
                propertyOverrides.insert({visualState, value});
            }
        }

        for (const auto& capture : resolvedRules.captures) {
            if (!capturesAdded.insert(capture.varName).second) {
                continue;
            }

            result.captures.push_back(capture);
        }
    }

    std::erase_if(result.overridesPerVSG,
                  [](const auto& item) { return item.second.empty(); });

    return result;
}

struct StyleVariableResolution {
    // Points into state->variables; only valid until that map is next touched,
    // so read it out before doing anything that could apply a style.
    const StyleVariableValue* value = nullptr;
    ElementId owner = ElementId::None;
};

// How well a capture serves a consumer, as a sort key -- smaller is better.
// Captures are ranked by, in order:
//
//  1. Deepest common ancestor with the consumer.
//  2. Shallowest capture element. On a tie the capture that lies on the
//     consumer's own parent chain *is* the common ancestor, so this is what
//     makes a capture on an ancestor beat one on a cousin below it.
//  3. Registration order, applied by the callers below keeping the last of
//     equal keys. Only a last resort, but latest-wins is the useful direction:
//     a host that rebuilds a subtree often leaves the previous copy in the tree
//     beside the new one, and a consumer above both ties on the keys above.
//     The newer capture is the live one.
//
// The closest capture wins even when its value is opaque, in which case the
// consuming style is skipped rather than falling through to a farther capture
// that happens to be usable.
std::pair<int, int> StyleVariableCaptureRank(
    ElementTreeNode const* consumerNode,
    ElementTreeNode const* captureNode) {
    int lcaDepth = ElementTreeLcaDepth(consumerNode, captureNode);
    int captureDepth = captureNode ? static_cast<int>(captureNode->depth)
                                   : std::numeric_limits<int>::max();
    return {-lcaDepth, captureDepth};
}

// Pick the capture of `varName` that `consumerNode` should read.
StyleVariableResolution FindWinningCapture(
    StyleVariableState* state,
    const std::wstring& varName,
    ElementTreeNode const* consumerNode) {
    StyleVariableResolution result;

    auto it = state->variables.find(varName);
    if (it == state->variables.end() || it->second.empty()) {
        return result;
    }

    const auto& captures = it->second;
    if (captures.size() == 1) {
        // The common case by far: nothing to rank, and the owner's spine node
        // never has to be resolved.
        return {&captures.front().value, captures.front().elementId};
    }

    std::pair<int, int> bestRank;
    for (const auto& capture : captures) {
        ElementTreeNode const* captureNode = nullptr;
        if (auto elementIt =
                g_elementsCustomizationState.find(capture.elementId);
            elementIt != g_elementsCustomizationState.end()) {
            captureNode = EnsureElementTreeNode(elementIt->second);
        }

        auto rank = StyleVariableCaptureRank(consumerNode, captureNode);
        if (!result.value || rank <= bestRank) {
            bestRank = rank;
            result = {&capture.value, capture.elementId};
        }
    }

    return result;
}

// A capture reduced to what ranking needs. The node is held by strong ref so a
// snapshot stays usable even after re-entrant work tears the owning element
// down.
struct StyleVariableCandidate {
    ElementId owner = ElementId::None;
    std::shared_ptr<ElementTreeNode> node;
};

// Resolve every capture's spine node once. A pass that ranks one variable
// against many consumers would otherwise repeat the same lookups per consumer,
// and only the ranking actually varies between them.
std::vector<StyleVariableCandidate> SnapshotStyleVariableCaptures(
    const std::vector<StyleVariableCapture>& captures) {
    std::vector<StyleVariableCandidate> candidates;
    candidates.reserve(captures.size());

    for (const auto& capture : captures) {
        StyleVariableCandidate candidate;
        candidate.owner = capture.elementId;
        if (auto elementIt =
                g_elementsCustomizationState.find(capture.elementId);
            elementIt != g_elementsCustomizationState.end()) {
            auto& elementCustomizationState = elementIt->second;
            EnsureElementTreeNode(elementCustomizationState);
            candidate.node = elementCustomizationState.treeNode;
        }

        candidates.push_back(std::move(candidate));
    }

    return candidates;
}

// The owner FindWinningCapture would pick, ranked from a snapshot. A snapshot
// taken before a re-entrant capture change can go stale, which at worst skips a
// consumer that needed redoing -- the change that invalidated it queues its own
// propagation, and that pass re-snapshots and picks the consumer up.
ElementId PickWinningCaptureOwner(
    const std::vector<StyleVariableCandidate>& candidates,
    ElementTreeNode const* consumerNode) {
    ElementId owner = ElementId::None;
    bool haveBest = false;
    std::pair<int, int> bestRank;

    for (const auto& candidate : candidates) {
        auto rank =
            StyleVariableCaptureRank(consumerNode, candidate.node.get());
        if (!haveBest || rank <= bestRank) {
            haveBest = true;
            bestRank = rank;
            owner = candidate.owner;
        }
    }

    return owner;
}

// What a `{{...}}` expansion needs. `consumerNode` is the consuming element's
// position in the tree, used to pick the closest capture of each name.
struct StyleVariableLookupContext {
    StyleVariableState* state;
    ElementTreeNode const* consumerNode;
    std::vector<StyleVariableDependency>* outDeps;
};

bool IsValidStyleVariableIdentifier(std::wstring_view sv) {
    if (sv.empty()) {
        return false;
    }
    auto isStart = [](wchar_t c) {
        return (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') ||
               c == L'_';
    };
    auto isCont = [&](wchar_t c) {
        return isStart(c) || (c >= L'0' && c <= L'9');
    };
    if (!isStart(sv[0])) {
        return false;
    }
    for (size_t i = 1; i < sv.size(); i++) {
        if (!isCont(sv[i])) {
            return false;
        }
    }
    return true;
}

// Value produced while evaluating a `{{ ... }}` expression: either a number or
// a string. Number literals and numeric variables produce numbers; backtick-
// delimited string literals and string-typed variables produce strings.
struct StyleExpressionValue {
    // Engaged => numeric value; otherwise `text` holds the string value.
    std::optional<double> number;
    std::wstring text;

    static StyleExpressionValue Number(double d) { return {d, std::wstring()}; }
    static StyleExpressionValue String(std::wstring s) {
        return {std::nullopt, std::move(s)};
    }

    bool IsNumber() const { return number.has_value(); }
};

// Thrown by a live skip() call to unwind the evaluator. Not a std::exception,
// so a generic failure handler on the way up does not mistake it for an error.
struct StyleVariableSkipRequested {};

// Recursive-descent evaluator for `{{ ... }}` expressions. Operands: number
// literals, backtick-delimited string literals, style variable references, and
// parenthesized subexpressions. Operators: binary + - * /, unary - / +, the
// comparisons < <= == >= > !=, the conditional operator cond ? a : b, the
// two-arg functions min(a, b) and max(a, b), and skip(), which throws
// StyleVariableSkipRequested so the consuming style is left unapplied.
// Standard math precedence.
// Arithmetic, relational, unary-sign, and min/max operators require numeric
// operands; == and != compare two numbers or two strings; the conditional
// selects one of its (possibly string) branches. Evaluate() formats the result
// to text.
//
// Variable references pushed into outDeps so the dependent style can be
// re-evaluated when those variables change.
class StyleVariableExpressionEvaluator {
   public:
    StyleVariableExpressionEvaluator(std::wstring_view text,
                                     const StyleVariableLookupContext* context)
        : m_text(text), m_context(context) {}

    // Returns the text form of the result: numeric results are formatted with
    // FormatDoubleInvariant, string results are returned verbatim. Throws
    // std::runtime_error on parse / evaluation failure (including when a value
    // is used where the grammar requires a number).
    std::wstring Evaluate() {
        m_pos = 0;
        SkipWhitespace();
        StyleExpressionValue v = ParseExpression();
        SkipWhitespace();
        if (m_pos != m_text.size()) {
            throw std::runtime_error(
                "Unexpected trailing characters in style variable expression");
        }
        if (v.IsNumber()) {
            return FormatDoubleInvariant(*v.number);
        }
        return v.text;
    }

   private:
    void SkipWhitespace() {
        while (m_pos < m_text.size() &&
               (m_text[m_pos] == L' ' || m_text[m_pos] == L'\t' ||
                m_text[m_pos] == L'\r' || m_text[m_pos] == L'\n')) {
            m_pos++;
        }
    }

    bool ConsumeChar(wchar_t c) {
        SkipWhitespace();
        if (m_pos < m_text.size() && m_text[m_pos] == c) {
            m_pos++;
            return true;
        }
        return false;
    }

    // Tries to consume the multi-char operator `op` at the current position
    // (after skipping leading whitespace). The operator must match exactly with
    // no embedded whitespace; advances past it and returns true on success.
    bool ConsumeOperator(std::wstring_view op) {
        SkipWhitespace();
        if (m_text.size() - m_pos >= op.size() &&
            m_text.compare(m_pos, op.size(), op) == 0) {
            m_pos += op.size();
            return true;
        }
        return false;
    }

    // Unwraps a numeric operand. In a dead ternary branch (m_live == false) the
    // value is discarded, so a string operand is tolerated (reported as 0)
    // rather than aborting the whole expression.
    double RequireNumber(const StyleExpressionValue& v) {
        if (v.IsNumber()) {
            return *v.number;
        }
        if (m_live) {
            throw std::runtime_error(
                "Non-numeric value used where a number is required in style "
                "variable expression");
        }
        return 0.0;
    }

    // Equality test for == / !=. Two numbers compare numerically, two strings
    // compare by content. A number/string mismatch is always unequal rather
    // than an error, so `{{var == `` ? default : var}}` can supply a fallback
    // for an undefined variable (which reads as the empty string) without
    // failing when the variable is instead a captured number.
    bool ValuesEqual(const StyleExpressionValue& a,
                     const StyleExpressionValue& b) {
        if (a.IsNumber() && b.IsNumber()) {
            return *a.number == *b.number;
        }
        if (!a.IsNumber() && !b.IsNumber()) {
            return a.text == b.text;
        }
        return false;
    }

    StyleExpressionValue ParseExpression() { return ParseTernary(); }

    // Conditional operator `cond ? thenVal : elseVal`, right-associative.
    // Short-circuit: only the taken branch is evaluated. The untaken branch is
    // still parsed (to advance the position and enforce syntax) with m_live
    // cleared, which suppresses value-level errors (division by zero, a
    // non-numeric / undefined variable, an unknown function), skip(), and
    // dependency capture for that branch.
    StyleExpressionValue ParseTernary() {
        StyleExpressionValue cond = ParseEquality();
        if (!ConsumeChar(L'?')) {
            return cond;
        }
        bool condTrue = RequireNumber(cond) != 0.0;
        bool prevLive = m_live;

        m_live = prevLive && condTrue;
        StyleExpressionValue thenVal = ParseExpression();
        m_live = prevLive;

        if (!ConsumeChar(L':')) {
            throw std::runtime_error(
                "Missing ':' for '?' in style variable expression");
        }

        m_live = prevLive && !condTrue;
        StyleExpressionValue elseVal = ParseTernary();
        m_live = prevLive;

        return condTrue ? thenVal : elseVal;
    }

    StyleExpressionValue ParseEquality() {
        StyleExpressionValue v = ParseRelational();
        while (true) {
            if (ConsumeOperator(L"==")) {
                v = StyleExpressionValue::Number(
                    ValuesEqual(v, ParseRelational()) ? 1.0 : 0.0);
            } else if (ConsumeOperator(L"!=")) {
                v = StyleExpressionValue::Number(
                    ValuesEqual(v, ParseRelational()) ? 0.0 : 1.0);
            } else {
                break;
            }
        }
        return v;
    }

    StyleExpressionValue ParseRelational() {
        StyleExpressionValue v = ParseAdditive();
        while (true) {
            // Match the two-char operators before their single-char prefixes.
            if (ConsumeOperator(L"<=")) {
                double lhs = RequireNumber(v);
                v = StyleExpressionValue::Number(
                    lhs <= RequireNumber(ParseAdditive()) ? 1.0 : 0.0);
            } else if (ConsumeOperator(L">=")) {
                double lhs = RequireNumber(v);
                v = StyleExpressionValue::Number(
                    lhs >= RequireNumber(ParseAdditive()) ? 1.0 : 0.0);
            } else if (ConsumeOperator(L"<")) {
                double lhs = RequireNumber(v);
                v = StyleExpressionValue::Number(
                    lhs < RequireNumber(ParseAdditive()) ? 1.0 : 0.0);
            } else if (ConsumeOperator(L">")) {
                double lhs = RequireNumber(v);
                v = StyleExpressionValue::Number(
                    lhs > RequireNumber(ParseAdditive()) ? 1.0 : 0.0);
            } else {
                break;
            }
        }
        return v;
    }

    StyleExpressionValue ParseAdditive() {
        StyleExpressionValue v = ParseTerm();
        while (true) {
            SkipWhitespace();
            if (ConsumeChar(L'+')) {
                double lhs = RequireNumber(v);
                v = StyleExpressionValue::Number(lhs +
                                                 RequireNumber(ParseTerm()));
            } else if (ConsumeChar(L'-')) {
                double lhs = RequireNumber(v);
                v = StyleExpressionValue::Number(lhs -
                                                 RequireNumber(ParseTerm()));
            } else {
                break;
            }
        }
        return v;
    }

    StyleExpressionValue ParseTerm() {
        StyleExpressionValue v = ParseFactor();
        while (true) {
            SkipWhitespace();
            if (ConsumeChar(L'*')) {
                double lhs = RequireNumber(v);
                v = StyleExpressionValue::Number(lhs *
                                                 RequireNumber(ParseFactor()));
            } else if (ConsumeChar(L'/')) {
                double lhs = RequireNumber(v);
                double rhs = RequireNumber(ParseFactor());
                if (rhs == 0.0) {
                    if (m_live) {
                        throw std::runtime_error(
                            "Division by zero in style variable expression");
                    }
                    // Dead ternary branch: the result is discarded, so skip the
                    // divide instead of throwing or producing inf/nan.
                    v = StyleExpressionValue::Number(lhs);
                } else {
                    v = StyleExpressionValue::Number(lhs / rhs);
                }
            } else {
                break;
            }
        }
        return v;
    }

    StyleExpressionValue ParseFactor() {
        SkipWhitespace();
        if (ConsumeChar(L'+')) {
            return StyleExpressionValue::Number(RequireNumber(ParseFactor()));
        }
        if (ConsumeChar(L'-')) {
            return StyleExpressionValue::Number(-RequireNumber(ParseFactor()));
        }
        return ParsePrimary();
    }

    StyleExpressionValue ParsePrimary() {
        SkipWhitespace();
        if (m_pos >= m_text.size()) {
            throw std::runtime_error(
                "Unexpected end of style variable expression");
        }

        wchar_t c = m_text[m_pos];
        if (c == L'(') {
            m_pos++;
            StyleExpressionValue v = ParseExpression();
            SkipWhitespace();
            if (!ConsumeChar(L')')) {
                throw std::runtime_error(
                    "Missing ')' in style variable expression");
            }
            return v;
        }

        if (c == L'`') {
            return ParseStringLiteral();
        }

        if ((c >= L'0' && c <= L'9') || c == L'.') {
            return StyleExpressionValue::Number(ParseNumberLiteral());
        }

        if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') || c == L'_') {
            return ParseIdentifierOrCall();
        }

        throw std::runtime_error(
            "Unexpected character in style variable expression");
    }

    // Backtick-delimited string literal. A doubled backtick encodes one literal
    // backtick character; every other character is taken verbatim. Backtick is
    // used (rather than a quote) so that literals don't clash with the string
    // quoting of YAML settings or with the double quotes of XAML attributes,
    // inside which these expressions often appear. The literal must be closed
    // before the end of the expression.
    StyleExpressionValue ParseStringLiteral() {
        m_pos++;  // Skip the opening backtick.
        std::wstring out;
        while (m_pos < m_text.size()) {
            wchar_t c = m_text[m_pos];
            if (c == L'`') {
                if (m_pos + 1 < m_text.size() && m_text[m_pos + 1] == L'`') {
                    out.push_back(L'`');
                    m_pos += 2;
                    continue;
                }
                m_pos++;
                return StyleExpressionValue::String(std::move(out));
            }
            out.push_back(c);
            m_pos++;
        }
        throw std::runtime_error(
            "Unterminated string literal in style variable expression");
    }

    double ParseNumberLiteral() {
        size_t start = m_pos;
        bool sawDigit = false;
        bool sawDot = false;
        while (m_pos < m_text.size()) {
            wchar_t c = m_text[m_pos];
            if (c >= L'0' && c <= L'9') {
                sawDigit = true;
                m_pos++;
            } else if (c == L'.' && !sawDot) {
                sawDot = true;
                m_pos++;
            } else {
                break;
            }
        }
        if (m_pos < m_text.size() &&
            (m_text[m_pos] == L'e' || m_text[m_pos] == L'E')) {
            m_pos++;
            if (m_pos < m_text.size() &&
                (m_text[m_pos] == L'+' || m_text[m_pos] == L'-')) {
                m_pos++;
            }
            while (m_pos < m_text.size() && m_text[m_pos] >= L'0' &&
                   m_text[m_pos] <= L'9') {
                m_pos++;
            }
        }
        if (!sawDigit) {
            throw std::runtime_error(
                "Bad number literal in style variable expression");
        }
        auto parsed = ParseDoubleInvariant(m_text.substr(start, m_pos - start));
        if (!parsed) {
            throw std::runtime_error(
                "Bad number literal in style variable expression");
        }
        return *parsed;
    }

    StyleExpressionValue ParseIdentifierOrCall() {
        size_t start = m_pos;
        while (m_pos < m_text.size()) {
            wchar_t c = m_text[m_pos];
            if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') ||
                (c >= L'0' && c <= L'9') || c == L'_') {
                m_pos++;
            } else {
                break;
            }
        }
        std::wstring_view ident = m_text.substr(start, m_pos - start);
        SkipWhitespace();
        if (m_pos < m_text.size() && m_text[m_pos] == L'(') {
            m_pos++;
            if (ident == L"skip") {
                if (!ConsumeChar(L')')) {
                    throw std::runtime_error(
                        "skip() takes no arguments in style variable "
                        "expression");
                }
                if (m_live) {
                    throw StyleVariableSkipRequested{};
                }
                // Dead ternary branch: value discarded.
                return StyleExpressionValue::Number(0.0);
            }
            double a = RequireNumber(ParseExpression());
            if (!ConsumeChar(L',')) {
                throw std::runtime_error(
                    "Expected ',' in min/max style variable call");
            }
            double b = RequireNumber(ParseExpression());
            if (!ConsumeChar(L')')) {
                throw std::runtime_error(
                    "Missing ')' after min/max style variable call");
            }
            if (ident == L"min") {
                return StyleExpressionValue::Number((a < b) ? a : b);
            }
            if (ident == L"max") {
                return StyleExpressionValue::Number((a > b) ? a : b);
            }
            if (m_live) {
                throw std::runtime_error(
                    "Unknown function in style variable expression");
            }
            // Dead ternary branch: value discarded, don't fail on the name.
            return StyleExpressionValue::Number(0.0);
        }
        return LookupVariable(std::wstring(ident));
    }

    StyleExpressionValue LookupVariable(const std::wstring& name) {
        // In a dead ternary branch (m_live == false) the value is discarded, so
        // skip the lookup along with dependency capture and the value-level
        // errors below; the branch must not abort the whole expression, and
        // every operator tolerates a string operand while not live.
        if (!m_live) {
            return StyleExpressionValue::String(L"");
        }

        auto resolution =
            FindWinningCapture(m_context->state, name, m_context->consumerNode);

        if (m_context->outDeps) {
            m_context->outDeps->push_back({name, resolution.owner});
        }
        if (!resolution.value) {
            Wh_Log(L"Style variable '%s' not defined; treating as empty string",
                   name.c_str());
            // Undefined reads as the empty string sentinel, so `{{var == `` ?
            // default : var}}` can detect the undefined state and substitute a
            // fallback. Arithmetic on an undefined variable then fails
            // RequireNumber and skips the style, rather than silently using 0.
            return StyleExpressionValue::String(L"");
        }
        if (resolution.value->numeric) {
            return StyleExpressionValue::Number(*resolution.value->numeric);
        }
        // Non-numeric primitive (e.g. a captured string property): usable as a
        // string operand.
        if (resolution.value->substitutable) {
            return StyleExpressionValue::String(resolution.value->stringForm);
        }
        // Opaque capture (brush, thickness, etc.): no value form usable in an
        // expression.
        throw std::runtime_error(
            "Style variable used in expression is not a primitive value");
    }

    std::wstring_view m_text;
    const StyleVariableLookupContext* m_context;
    size_t m_pos = 0;
    // When false, we're parsing (but discarding) the untaken branch of a
    // ternary; value-level errors and dependency capture are suppressed.
    bool m_live = true;
};

// Evaluate a single expression body (the text between `{{` and `}}`). If the
// body is a bare identifier, returns the variable's `stringForm` directly --
// but only when the captured value is a primitive type flagged `substitutable`
// (numeric, boolean, or string). Missing variables and opaque-type captures
// both cause this function to return std::nullopt, at which point
// ExpandStyleVariables aborts the whole expansion and the consuming style is
// skipped. This matches the arithmetic path's behaviour of failing closed
// rather than substituting a value that won't parse. A live skip() call unwinds
// through here as StyleVariableSkipRequested.
std::optional<std::wstring> EvaluateStyleVariableExpression(
    std::wstring_view exprText,
    const StyleVariableLookupContext* context) {
    auto trimmed = TrimStringView(exprText);
    if (trimmed.empty()) {
        Wh_Log(L"Empty style variable expression");
        return std::nullopt;
    }

    if (IsValidStyleVariableIdentifier(trimmed)) {
        std::wstring name(trimmed);
        auto resolution =
            FindWinningCapture(context->state, name, context->consumerNode);
        if (context->outDeps) {
            context->outDeps->push_back({name, resolution.owner});
        }
        if (!resolution.value) {
            Wh_Log(L"Style variable '%s' not yet defined; skipping style",
                   name.c_str());
            return std::nullopt;
        }
        if (!resolution.value->substitutable) {
            Wh_Log(
                L"Style variable '%s' is not substitutable (captured type "
                L"'%s'); skipping style",
                name.c_str(), resolution.value->stringForm.c_str());
            return std::nullopt;
        }
        return resolution.value->stringForm;
    }

    try {
        StyleVariableExpressionEvaluator eval(trimmed, context);
        return eval.Evaluate();
    } catch (StyleVariableSkipRequested const&) {
        Wh_Log(L"skip() reached in '%.*s'; leaving style unapplied",
               static_cast<int>(trimmed.size()), trimmed.data());
        throw;
    } catch (std::exception const& ex) {
        Wh_Log(L"Style variable expression failed: %S (in '%.*s')", ex.what(),
               static_cast<int>(trimmed.size()), trimmed.data());
        return std::nullopt;
    }
}

// Walks the input text, repeatedly expanding the innermost `{{ ... }}`
// substitution. Returns std::nullopt on parse failure (and logs a warning); a
// StyleVariableSkipRequested from an expression propagates out.
//
// Inner-matching rule: the first `}}` is paired with the *rightmost* `{{` that
// precedes it. So `{{{x}}}` -> `{` + value-of-x + `}` (literal outer braces).
//
// Substituted text is treated as literal (no further `{{...}}` expansion of the
// substituted output) to keep behavior predictable.
std::optional<std::wstring> ExpandStyleVariables(
    std::wstring_view input,
    const StyleVariableLookupContext* context) {
    std::wstring result(input);
    size_t scanFrom = 0;

    while (true) {
        size_t closePos = std::wstring::npos;
        for (size_t i = scanFrom; i + 1 < result.size(); i++) {
            if (result[i] == L'}' && result[i + 1] == L'}') {
                closePos = i;
                break;
            }
        }
        if (closePos == std::wstring::npos) {
            break;
        }

        // Find rightmost `{{` strictly before closePos. Search from closePos -
        // 1 downward; the pair occupies indices (j-1, j).
        size_t openPos = std::wstring::npos;
        if (closePos >= 2) {
            for (size_t j = closePos - 1; j >= 1; j--) {
                if (result[j - 1] == L'{' && result[j] == L'{') {
                    openPos = j - 1;
                    break;
                }
                if (j == 1) {
                    break;
                }
            }
        }

        if (openPos == std::wstring::npos) {
            Wh_Log(L"Unmatched '}}' in style value at offset %zu", closePos);
            return std::nullopt;
        }

        std::wstring_view exprText(result.data() + openPos + 2,
                                   closePos - openPos - 2);
        auto expanded = EvaluateStyleVariableExpression(exprText, context);
        if (!expanded) {
            return std::nullopt;
        }

        size_t spanLen = closePos + 2 - openPos;
        result.replace(openPos, spanLen, *expanded);
        scanFrom = openPos + expanded->size();
    }

    return result;
}

// Read a property's current effective value and convert it to a
// StyleVariableValue suitable for `{{Var}}` substitution. Numeric primitives
// produce both string + numeric forms and are flagged substitutable; boolean
// and string primitives are flagged substitutable but have no numeric form.
// Opaque types (brushes, thicknesses, etc.) record only the captured class name
// as a diagnostic and are NOT flagged substitutable -- the bare- identifier
// substitution path skips them rather than emitting a class name into the XAML
// output.
StyleVariableValue ReadCapturedStyleVariableValue(FrameworkElement element,
                                                  DependencyProperty property) {
    StyleVariableValue out;

    auto elementDo = element.as<DependencyObject>();
    winrt::Windows::Foundation::IInspectable value{nullptr};
    // Get effective value so layout-driven properties like ActualWidth (which
    // never have a local value) still capture.
    try {
        value = elementDo.GetValue(property);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
    if (!value || value == DependencyProperty::UnsetValue()) {
        out.stringForm = L"";
        return out;
    }

    try {
        if (auto unboxed = TryUnboxPropertyValue(value)) {
            out.stringForm = FormatUnboxedPropertyValue(*unboxed);
            out.numeric = UnboxedPropertyValueAsNumeric(*unboxed);
            out.substitutable = true;
            return out;
        }

        // Opaque value (brush, thickness, etc.). Stored as a diagnostic only;
        // not flagged substitutable, so bare `{{Var}}` skips the consuming
        // style with a clear log message rather than emitting `className` into
        // the XAML.
        out.stringForm = std::wstring(winrt::get_class_name(value));
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        out.stringForm = L"";
    }
    return out;
}

// Remove this (elementId, property) entry from the consumer lists of every
// variable named in oldDeps, then add it for every variable named in newDeps.
// `fallbackClassName` is stored on each newly-added consumer entry so the
// per-consumer context is preserved across propagations; it is irrelevant when
// newDeps is empty (pure-removal calls from the cleanup paths).
void UpdateStyleVariableConsumers(
    StyleVariableState* state,
    ElementId elementId,
    DependencyProperty property,
    PCWSTR fallbackClassName,
    const std::vector<StyleVariableDependency>& oldDeps,
    const std::vector<StyleVariableDependency>& newDeps) {
    if (!state) {
        // The element's XamlRoot has already been destroyed (or was never
        // available); the StyleVariableState entry has been or will be reaped,
        // and there is nothing to clean up. New registrations (newDeps) are
        // also dropped on the floor: without a state we cannot route
        // propagations anyway.
        return;
    }

    for (const auto& dep : oldDeps) {
        auto it = state->consumers.find(dep.name);
        if (it == state->consumers.end()) {
            continue;
        }
        auto& consumers = it->second;
        ReleaseStyleVariableElementRefs(
            state, elementId,
            std::erase_if(consumers, [&](const StyleVariableConsumer& c) {
                return c.elementId == elementId && c.property == property;
            }));
        if (consumers.empty()) {
            state->consumers.erase(it);
        }
    }

    std::wstring fallbackClassNameStr =
        fallbackClassName ? fallbackClassName : L"";
    for (const auto& dep : newDeps) {
        auto& consumers = state->consumers[dep.name];
        bool already = std::any_of(consumers.begin(), consumers.end(),
                                   [&](const StyleVariableConsumer& c) {
                                       return c.elementId == elementId &&
                                              c.property == property;
                                   });
        if (!already) {
            consumers.push_back({elementId, property, fallbackClassNameStr});
            AddStyleVariableElementRef(state, elementId);
        }
    }
}

// Value comparison of `a` and `b` as boxes of the first struct type in
// T, Ts... that `a` is a box of; nullopt when it is none of them.
template <typename T, typename... Ts>
std::optional<bool> SameBoxedStruct(
    winrt::Windows::Foundation::IInspectable const& a,
    winrt::Windows::Foundation::IInspectable const& b) {
    if (auto ra = a.try_as<winrt::Windows::Foundation::IReference<T>>()) {
        auto rb = b.try_as<winrt::Windows::Foundation::IReference<T>>();
        return rb && ra.Value() == rb.Value();
    }
    if constexpr (sizeof...(Ts) > 0) {
        return SameBoxedStruct<Ts...>(a, b);
    } else {
        return std::nullopt;
    }
}

// Whether two values read from a property are the same local value. XAML boxes
// value types anew on every read, so those compare by value: primitives and
// enums through TryUnboxPropertyValue, then the struct types styles commonly
// set. Any other boxed value type is taken as unchanged: adopting a value that
// may be the mod's own would leave it in place on cleanup, the worse mistake.
// Reference types, UnsetValue included, compare by identity.
bool SameLocalValue(winrt::Windows::Foundation::IInspectable const& a,
                    winrt::Windows::Foundation::IInspectable const& b) {
    if (a == b) {
        return true;
    }
    if (!a || !b) {
        return false;
    }

    auto ua = TryUnboxPropertyValue(a);
    auto ub = TryUnboxPropertyValue(b);
    if (ua || ub) {
        return ua && ub &&
               std::visit(
                   [](auto const& x, auto const& y) -> bool {
                       using X = std::decay_t<decltype(x)>;
                       if constexpr (!std::is_same_v<
                                         X, std::decay_t<decltype(y)>>) {
                           return false;
                       } else if constexpr (std::is_floating_point_v<X>) {
                           return x == y || (std::isnan(x) && std::isnan(y));
                       } else {
                           return x == y;
                       }
                   },
                   *ua, *ub);
    }

    if (auto same = SameBoxedStruct<
            Thickness, CornerRadius, GridLength,
            winrt::Windows::Foundation::Point, winrt::Windows::Foundation::Size,
            winrt::Windows::Foundation::Rect, winrt::Windows::UI::Color,
            winrt::Windows::UI::Text::FontWeight>(a, b)) {
        return *same;
    }

    auto isBoxedValue = [](winrt::Windows::Foundation::IInspectable const& v) {
        return v.try_as<winrt::Windows::Foundation::IPropertyValue>() !=
                   nullptr ||
               std::wstring_view(winrt::get_class_name(v))
                   .starts_with(L"Windows.Foundation.IReference`1<");
    };
    return isBoxedValue(a) && isBoxedValue(b);
}

// Record a write by something other than the mod as the property's pre-style
// value. A local value still equal to the one the mod applied means nothing
// external happened (an animation changes only the effective value), and
// adopting the mod's own brush would make it survive cleanup and crash when
// the DLL is unloaded.
void AdoptExternalValueAsOriginal(
    FrameworkElement element,
    DependencyProperty property,
    ElementPropertyCustomizationState* propertyCustomizationState) {
    if (!propertyCustomizationState->customValue) {
        return;
    }
    auto localValue = ReadLocalValueWithWorkaround(element, property);
    if (!SameLocalValue(localValue,
                        propertyCustomizationState->lastAppliedValue)) {
        propertyCustomizationState->originalValue = localValue;
    }
}

// Put the property back to its pre-style value and forget what was applied.
// Leaves the dynamic template alone, so a later variable change can apply the
// style again.
void UnapplyStyleValue(
    FrameworkElement element,
    DependencyProperty property,
    ElementPropertyCustomizationState* propertyCustomizationState) {
    AdoptExternalValueAsOriginal(element, property, propertyCustomizationState);
    if (propertyCustomizationState->originalValue) {
        bool wasModifying = g_elementPropertyModifying;
        g_elementPropertyModifying = true;
        SetOrClearValue(element, property,
                        *propertyCustomizationState->originalValue);
        g_elementPropertyModifying = wasModifying;
        propertyCustomizationState->originalValue.reset();
    }
    propertyCustomizationState->lastAppliedValue = nullptr;
    propertyCustomizationState->customValue.reset();
}

// Re-evaluate the dynamic template stored on `propertyCustomizationState` and
// return the resolved IInspectable / XamlBlurBrushParams ready to be applied.
// Updates the (elementId, property) -> state->consumers registry to match the
// freshly computed dependency set so future variable changes route to this
// property. The dependency registry is committed *before* the final XAML
// resolution attempt: ExpandStyleVariables records every variable name it scans
// into newDeps even on partial parse failure, which lets a future change to any
// of those variables re-enter this function and retry. The trade-off is that on
// resolution failure the caller's last-good `customValue` is preserved (we
// return std::nullopt and the caller leaves the property as-is); this
// self-heals on the next variable change.
//
// `fallbackClassName` is the consumer-element's own fallback class name (the
// one that was used when matching the consumer's target rule), which is
// generally NOT the same as the capturer's. It is what
// ResolveExpandedSinglePropertyValue feeds to the synthetic <Style> used to
// re-parse the rule body, and it is also stored on each new
// StyleVariableConsumer entry so subsequent propagations route through this
// same context.
//
// `elementCustomizationState` is the consumer's own entry when the caller
// already has it, saving the lookup needed to rank captures by proximity; pass
// nullptr to have it looked up from `elementId`.
//
// Returns std::nullopt if the state has no template, expansion failed, or XAML
// resolution failed. A skip() in the rule body also yields std::nullopt, after
// putting the property back to its original value.
std::optional<PropertyOverrideValue> ResolveDynamicStyleValue(
    StyleVariableState* state,
    ElementId elementId,
    FrameworkElement element,
    DependencyProperty property,
    PCWSTR fallbackClassName,
    ElementPropertyCustomizationState* propertyCustomizationState,
    ElementCustomizationState* elementCustomizationState) {
    if (!propertyCustomizationState->dynamicTemplate) {
        return std::nullopt;
    }

    const auto& tmpl = *propertyCustomizationState->dynamicTemplate;

    if (!elementCustomizationState) {
        if (auto it = g_elementsCustomizationState.find(elementId);
            it != g_elementsCustomizationState.end()) {
            elementCustomizationState = &it->second;
        }
    }

    ElementTreeNode const* consumerNode =
        elementCustomizationState
            ? EnsureElementTreeNode(*elementCustomizationState)
            : nullptr;

    std::vector<StyleVariableDependency> newDeps;
    StyleVariableLookupContext context{state, consumerNode, &newDeps};
    std::optional<std::wstring> expanded;
    bool skipped = false;
    try {
        expanded = ExpandStyleVariables(tmpl.rawValue, &context);
    } catch (StyleVariableSkipRequested const&) {
        skipped = true;
    }

    UpdateStyleVariableConsumers(
        state, elementId, property, fallbackClassName,
        propertyCustomizationState->variableDependencies, newDeps);
    propertyCustomizationState->variableDependencies = std::move(newDeps);

    if (skipped) {
        // Every variable that decides whether skip() is reached was read
        // before it, so targeted propagation still reaches this property.
        propertyCustomizationState->lastResolveFailed = false;
        UnapplyStyleValue(element, property, propertyCustomizationState);
        return std::nullopt;
    }

    if (!expanded) {
        propertyCustomizationState->lastResolveFailed = true;
        return std::nullopt;
    }

    auto typeName = winrt::get_class_name(element);
    auto resolved = ResolveExpandedSinglePropertyValue(
        std::wstring_view(typeName), tmpl.propertyName, *expanded,
        tmpl.isXamlValue);
    if (!resolved) {
        Wh_Log(
            L"Dynamic style resolution failed for '%s' on %s; keeping "
            L"previously applied value",
            tmpl.propertyName.c_str(), typeName.c_str());
    }
    propertyCustomizationState->lastResolveFailed = !resolved;
    return resolved;
}

// Whether a change to `varName` can alter this property's resolved value.
// `changedOwner` is set when one capture's value changed: only consumers that
// read from that capture are affected. It is empty when the set of captures
// changed instead, in which case `winningOwner` is the capture the consumer
// would read now, and only a consumer whose recorded owner differs needs
// redoing.
bool StyleVariableChangeAffectsConsumer(
    const ElementPropertyCustomizationState& propertyCustomizationState,
    const std::wstring& varName,
    std::optional<ElementId> changedOwner,
    ElementId winningOwner) {
    if (propertyCustomizationState.lastResolveFailed) {
        return true;
    }

    for (const auto& dep : propertyCustomizationState.variableDependencies) {
        if (dep.name != varName) {
            continue;
        }

        return changedOwner ? dep.owner == *changedOwner
                            : dep.owner != winningOwner;
    }

    return false;
}

// Re-evaluate the dependent styles a change to `varName` can actually reach.
// Each consumer carries its own fallbackClassName (recorded when the consumer
// was registered), so propagation uses the consumer's own match-site context to
// re-parse the rule body, even when the capturer was matched against a
// different type/fallback class.
void PropagateStyleVariableChangeCore(StyleVariableState* state,
                                      const std::wstring& varName,
                                      std::optional<ElementId> changedOwner) {
    auto consumersIt = state->consumers.find(varName);
    if (consumersIt == state->consumers.end()) {
        return;
    }

    // Only the ranking varies per consumer, so the captures' spine nodes are
    // resolved once for the whole pass. Needed only when the set of captures
    // changed; a value change routes by the recorded owner instead.
    std::vector<StyleVariableCandidate> candidates;
    if (!changedOwner) {
        if (auto varIt = state->variables.find(varName);
            varIt != state->variables.end()) {
            candidates = SnapshotStyleVariableCaptures(varIt->second);
        }
    }

    auto consumersCopy = consumersIt->second;
    for (const auto& consumer : consumersCopy) {
        auto stateIt = g_elementsCustomizationState.find(consumer.elementId);
        if (stateIt == g_elementsCustomizationState.end()) {
            continue;
        }
        // A reference rather than the iterator: applying a style below can
        // realize children, which re-enters ApplyCustomizations and may rehash
        // g_elementsCustomizationState. Rehashing invalidates iterators but not
        // references to the mapped values. A re-entrant cleanup or re-apply of
        // this same elementId would invalidate both the reference and the loop
        // below, but the re-entrancy is for the newly realized children.
        auto& elementState = stateIt->second;

        auto element = elementState.element.get();
        if (!element) {
            continue;
        }

        // A handful of pointer comparisons against the snapshot above, far
        // cheaper than the re-parse it avoids.
        ElementId winningOwner =
            changedOwner ? ElementId::None
                         : PickWinningCaptureOwner(
                               candidates, EnsureElementTreeNode(elementState));

        PCWSTR consumerFallbackClassName =
            consumer.fallbackClassName.empty()
                ? nullptr
                : consumer.fallbackClassName.c_str();

        for (auto& [vsgWeak, vsgState] : elementState.perVisualStateGroup) {
            auto propIt =
                vsgState.propertyCustomizationStates.find(consumer.property);
            if (propIt == vsgState.propertyCustomizationStates.end()) {
                continue;
            }
            auto& propState = propIt->second;
            if (!propState.dynamicTemplate) {
                continue;
            }

            if (!StyleVariableChangeAffectsConsumer(
                    propState, varName, changedOwner, winningOwner)) {
                continue;
            }

            auto resolved = ResolveDynamicStyleValue(
                state, consumer.elementId, element, consumer.property,
                consumerFallbackClassName, &propState, &elementState);
            if (!resolved) {
                continue;
            }
            AdoptExternalValueAsOriginal(element, consumer.property,
                                         &propState);
            if (!propState.originalValue) {
                propState.originalValue =
                    ReadLocalValueWithWorkaround(element, consumer.property);
            }
            propState.customValue = *resolved;

            bool wasModifying = g_elementPropertyModifying;
            g_elementPropertyModifying = true;
            propState.lastAppliedValue =
                SetOrClearValue(element, consumer.property, *resolved);
            g_elementPropertyModifying = wasModifying;
        }
    }
}

// Notify the styles that depend on `varName`. `changedOwner` names the capture
// whose value changed, or is empty when captures were added or removed.
//
// Applying a style can realize children (running ApplyCustomizations, which
// adds captures) or write a captured property (running a capture callback,
// which g_elementPropertyModifying deliberately does not suppress), so this
// re-enters. Nested calls queue instead of running, and the outermost frame
// drains the queue, which also coalesces a burst into one pass.
void PropagateStyleVariableChange(StyleVariableState* state,
                                  const std::wstring& varName,
                                  std::optional<ElementId> changedOwner) {
    PendingStyleVariablePropagation propagation{state, varName, changedOwner};

    if (g_styleVariablePropagationDepth > 0) {
        auto& pending = g_pendingStyleVariablePropagations;
        if (std::find(pending.begin(), pending.end(), propagation) ==
            pending.end()) {
            pending.push_back(std::move(propagation));
        }
        return;
    }

    struct DepthScope {
        DepthScope() { g_styleVariablePropagationDepth++; }
        ~DepthScope() { g_styleVariablePropagationDepth--; }
    } depthScope;

    PropagateStyleVariableChangeCore(state, varName, changedOwner);

    // A style that writes a property some rule captures keeps refilling the
    // queue. The unchanged-value fast path settles most such loops within a
    // round or two; a value that oscillates never settles, so give up loudly
    // instead of hanging the UI thread.
    constexpr int kMaxDrainRounds = 32;

    for (int round = 0; !g_pendingStyleVariablePropagations.empty(); round++) {
        if (round >= kMaxDrainRounds) {
            Wh_Log(
                L"Style variables did not settle after %d rounds; dropping %zu "
                L"queued update(s)",
                kMaxDrainRounds, g_pendingStyleVariablePropagations.size());
            g_pendingStyleVariablePropagations.clear();
            break;
        }

        auto pending = std::move(g_pendingStyleVariablePropagations);
        g_pendingStyleVariablePropagations.clear();
        for (const auto& pendingPropagation : pending) {
            PropagateStyleVariableChangeCore(pendingPropagation.state,
                                             pendingPropagation.varName,
                                             pendingPropagation.changedOwner);
        }
    }
}

// std::optional<double>'s operator== follows IEEE (NaN != NaN), which would
// report a NaN capture such as Height=Auto as changed on every re-read.
bool SameNumericValue(const std::optional<double>& a,
                      const std::optional<double>& b) {
    if (a.has_value() != b.has_value()) {
        return false;
    }
    return !a || *a == *b || (std::isnan(*a) && std::isnan(*b));
}

// Store a capture's freshly read value and notify dependents if it changed.
// The comparison is against this capture's own previous value: comparing
// against whichever capture currently wins would silently drop a second
// capturer's change whenever it happened to match. Used by every path that
// publishes a captured value -- the per-property capture callback and the
// SizeChanged catch-all -- so the no-op fast path applies uniformly.
void SetStyleVariableIfChangedAndPropagate(StyleVariableState* state,
                                           const std::wstring& varName,
                                           ElementId owner,
                                           StyleVariableValue value) {
    auto varIt = state->variables.find(varName);
    if (varIt == state->variables.end()) {
        return;
    }

    auto& captures = varIt->second;
    auto it = std::find_if(captures.begin(), captures.end(),
                           [owner](const StyleVariableCapture& capture) {
                               return capture.elementId == owner;
                           });
    if (it == captures.end()) {
        // The capture was torn down between the notification and here.
        return;
    }

    if (it->value.stringForm == value.stringForm &&
        SameNumericValue(it->value.numeric, value.numeric) &&
        it->value.substitutable == value.substitutable) {
        Wh_Log(L"Style variable '%s' unchanged at '%s'", varName.c_str(),
               value.stringForm.c_str());
        return;
    }

    Wh_Log(L"Style variable '%s' changed: '%s' -> '%s'", varName.c_str(),
           it->value.stringForm.c_str(), value.stringForm.c_str());
    it->value = std::move(value);
    PropagateStyleVariableChange(state, varName, owner);
}

// True for layout-driven DPs whose updates do not fire
// RegisterPropertyChangedCallback on UWP, so capture rules on those DPs need
// `FrameworkElement.SizeChanged` as their notification source instead.
bool IsLayoutDrivenSizeProperty(DependencyProperty property) {
    return property == FrameworkElement::ActualWidthProperty() ||
           property == FrameworkElement::ActualHeightProperty();
}

// Wire up `Property=>VarName` capture rules for an element. Called once per
// matched element (captures are not visual-state-aware). Seeds the variables
// from the current property values, registers per-DP property-changed
// callbacks, and -- because UWP's ActualWidth/ActualHeight don't fire those
// callbacks on layout -- subscribes to FrameworkElement.SizeChanged as a
// catch-all that re-reads every active capture on resize.
//
// Seeding writes the captured values into state->variables in a single batch
// (to avoid intermediate inconsistent states for consumers that depend on
// multiple variables from this element) and only then propagates. Every seeded
// name propagates, even one whose value matches an existing capture's: adding a
// capture changes which captures a consumer chooses between, so the consumers
// have to be re-scored regardless of the value. The function does not need the
// capturer's fallbackClassName: each StyleVariableConsumer entry already
// carries its own consumer-side fallback, so propagation routes through the
// right context per consumer.
void SetUpCapturesForElement(StyleVariableState* state,
                             ElementId elementId,
                             FrameworkElement element,
                             const std::vector<CaptureSpec>& captures,
                             ElementCustomizationState* elementState) {
    if (captures.empty()) {
        return;
    }

    auto elementDo = element.as<DependencyObject>();
    winrt::weak_ref<FrameworkElement> elementWeakRef = element;

    // Names seeded below, propagated once the whole batch is in place.
    std::vector<std::wstring> seededVarNames;
    seededVarNames.reserve(captures.size());

    // Captures whose source DP is layout-driven (ActualWidth/ActualHeight) need
    // a SizeChanged subscription as their notification source. Collect them so
    // we only subscribe once and only when needed.
    std::vector<std::pair<DependencyProperty, std::wstring>>
        sizeChangedCaptures;

    for (const auto& capture : captures) {
        const auto [it, inserted] =
            elementState->captureCustomizationStates.insert(
                {capture.property, {}});
        if (!inserted) {
            // Same DP captured twice on this element (different rules with the
            // same property); keep the first and warn so the dropped second is
            // not a silent footgun for users who later try to reference the
            // dropped variable in a `{{...}}` substitution.
            Wh_Log(
                L"Capture for property already registered on %s; "
                L"dropping duplicate variable '%s' (kept: '%s')",
                winrt::get_class_name(element).c_str(), capture.varName.c_str(),
                it->second.varName.c_str());
            continue;
        }
        auto& captureState = it->second;
        captureState.varName = capture.varName;

        auto value = ReadCapturedStyleVariableValue(element, capture.property);

        // No entry for this element can exist yet: the insert above rejects a
        // second capture of the same DP, and FindElementPropertyOverrides
        // rejects a second capture of the same name.
        auto& capturesForVar = state->variables[capture.varName];
        Wh_Log(
            L"Seeding capture variable '%s' from %s with value '%s' "
            L"(%zu other capture(s))",
            capture.varName.c_str(), winrt::get_class_name(element).c_str(),
            value.stringForm.c_str(), capturesForVar.size());
        capturesForVar.push_back({elementId, std::move(value)});
        AddStyleVariableElementRef(state, elementId);

        seededVarNames.push_back(capture.varName);

        if (IsLayoutDrivenSizeProperty(capture.property)) {
            sizeChangedCaptures.push_back({capture.property, capture.varName});
            // No property-changed callback: the DP doesn't fire one for layout
            // updates anyway, and SizeChanged below covers it.
            continue;
        }

        std::wstring varName = capture.varName;
        captureState.propertyChangedToken =
            elementDo.RegisterPropertyChangedCallback(
                capture.property,
                [state, varName, elementId, elementWeakRef](
                    DependencyObject sender, DependencyProperty property) {
                    auto element = elementWeakRef.get();
                    if (!element) {
                        return;
                    }
                    auto value =
                        ReadCapturedStyleVariableValue(element, property);
                    SetStyleVariableIfChangedAndPropagate(
                        state, varName, elementId, std::move(value));
                });
    }

    if (!sizeChangedCaptures.empty()) {
        elementState->captureSizeChangedToken = element.SizeChanged(
            [state, elementId, elementWeakRef,
             sizeChangedCaptures = std::move(sizeChangedCaptures)](
                winrt::Windows::Foundation::IInspectable const& sender,
                SizeChangedEventArgs const& e) {
                auto element = elementWeakRef.get();
                if (!element) {
                    return;
                }
                Wh_Log(L"SizeChanged on %s: %.3fx%.3f",
                       winrt::get_class_name(element).c_str(),
                       e.NewSize().Width, e.NewSize().Height);
                for (const auto& [property, varName] : sizeChangedCaptures) {
                    auto value =
                        ReadCapturedStyleVariableValue(element, property);
                    SetStyleVariableIfChangedAndPropagate(
                        state, varName, elementId, std::move(value));
                }
            });
    }

    // The new captures may be closer to consumers registered before this
    // element was matched than whatever they were reading.
    for (const auto& varName : seededVarNames) {
        PropagateStyleVariableChange(state, varName, std::nullopt);
    }
}

// Tear down capture subscriptions for an element. Called from
// CleanupCustomizations and UninitializeSettingsAndTap before the
// ElementCustomizationState entry is erased.
void RestoreCapturesForElement(FrameworkElement element,
                               const ElementCustomizationState& elementState) {
    if (!element) {
        return;
    }

    for (const auto& [property, captureState] :
         elementState.captureCustomizationStates) {
        if (!captureState.propertyChangedToken) {
            continue;
        }
        try {
            element.UnregisterPropertyChangedCallback(
                property, captureState.propertyChangedToken);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    if (elementState.captureSizeChangedToken) {
        try {
            element.SizeChanged(elementState.captureSizeChangedToken);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }
}

void ApplyCustomizationsForVisualStateGroup(
    StyleVariableState* state,
    ElementId elementId,
    FrameworkElement element,
    VisualStateGroup visualStateGroup,
    PCWSTR fallbackClassName,
    PropertyOverrides propertyOverrides,
    ElementCustomizationStateForVisualStateGroup*
        elementCustomizationStateForVisualStateGroup) {
    auto elementDo = element.as<DependencyObject>();

    VisualState currentVisualState(
        visualStateGroup ? visualStateGroup.CurrentState() : nullptr);

    std::wstring currentVisualStateName(
        currentVisualState ? currentVisualState.Name() : L"");

    for (const auto& [property, valuesPerVisualState] : propertyOverrides) {
        const auto [propertyCustomizationStatesIt, inserted] =
            elementCustomizationStateForVisualStateGroup
                ->propertyCustomizationStates.insert({property, {}});
        if (!inserted) {
            continue;
        }

        auto& propertyCustomizationState =
            propertyCustomizationStatesIt->second;

        auto it = valuesPerVisualState.find(currentVisualStateName);
        if (it == valuesPerVisualState.end() &&
            !currentVisualStateName.empty()) {
            it = valuesPerVisualState.find(L"");
        }

        if (it != valuesPerVisualState.end()) {
            std::optional<PropertyOverrideValue> resolved;
            if (auto* tmpl = std::get_if<DynamicStyleTemplate>(&it->second)) {
                propertyCustomizationState.dynamicTemplate = *tmpl;
                resolved = ResolveDynamicStyleValue(
                    state, elementId, element, property, fallbackClassName,
                    &propertyCustomizationState,
                    /*elementCustomizationState=*/nullptr);
            } else {
                resolved = it->second;
            }

            if (resolved) {
                propertyCustomizationState.originalValue =
                    ReadLocalValueWithWorkaround(element, property);
                propertyCustomizationState.customValue = *resolved;
                propertyCustomizationState.lastAppliedValue = SetOrClearValue(
                    element, property, *resolved, /*initialApply=*/true);
            }
        }

        propertyCustomizationState.propertyChangedToken =
            elementDo.RegisterPropertyChangedCallback(
                property,
                [&propertyCustomizationState](DependencyObject sender,
                                              DependencyProperty property) {
                    if (g_elementPropertyModifying) {
                        return;
                    }

                    auto element = sender.try_as<FrameworkElement>();
                    if (!element) {
                        return;
                    }

                    if (!propertyCustomizationState.customValue) {
                        return;
                    }

                    AdoptExternalValueAsOriginal(element, property,
                                                 &propertyCustomizationState);

                    Wh_Log(L"Re-applying style for %s",
                           winrt::get_class_name(element).c_str());

                    g_elementPropertyModifying = true;
                    propertyCustomizationState.lastAppliedValue =
                        SetOrClearValue(
                            element, property,
                            *propertyCustomizationState.customValue);
                    g_elementPropertyModifying = false;
                });
    }

    if (visualStateGroup) {
        winrt::weak_ref<FrameworkElement> elementWeakRef = element;
        std::wstring fallbackClassNameStr =
            fallbackClassName ? fallbackClassName : L"";
        elementCustomizationStateForVisualStateGroup
            ->visualStateGroupCurrentStateChangedToken =
            visualStateGroup.CurrentStateChanged(
                [state, elementWeakRef, propertyOverrides, elementId,
                 fallbackClassNameStr,
                 elementCustomizationStateForVisualStateGroup](
                    winrt::Windows::Foundation::IInspectable const& sender,
                    VisualStateChangedEventArgs const& e) {
                    auto element = elementWeakRef.get();
                    if (!element) {
                        return;
                    }

                    Wh_Log(L"Re-applying all styles for %s",
                           winrt::get_class_name(element).c_str());

                    g_elementPropertyModifying = true;

                    auto& propertyCustomizationStates =
                        elementCustomizationStateForVisualStateGroup
                            ->propertyCustomizationStates;

                    PCWSTR fallbackClassNamePtr =
                        fallbackClassNameStr.empty()
                            ? nullptr
                            : fallbackClassNameStr.c_str();

                    for (const auto& [property, valuesPerVisualState] :
                         propertyOverrides) {
                        auto& propertyCustomizationState =
                            propertyCustomizationStates.at(property);

                        auto newState = e.NewState();
                        auto newStateName =
                            std::wstring{newState ? newState.Name() : L""};
                        auto it = valuesPerVisualState.find(newStateName);
                        if (it == valuesPerVisualState.end()) {
                            it = valuesPerVisualState.find(L"");
                            if (it != valuesPerVisualState.end()) {
                                auto oldState = e.OldState();
                                auto oldStateName = std::wstring{
                                    oldState ? oldState.Name() : L""};
                                if (!valuesPerVisualState.contains(
                                        oldStateName)) {
                                    continue;
                                }
                            }
                        }

                        if (it != valuesPerVisualState.end()) {
                            std::optional<PropertyOverrideValue> resolved;
                            if (auto* tmpl = std::get_if<DynamicStyleTemplate>(
                                    &it->second)) {
                                propertyCustomizationState.dynamicTemplate =
                                    *tmpl;
                                resolved = ResolveDynamicStyleValue(
                                    state, elementId, element, property,
                                    fallbackClassNamePtr,
                                    &propertyCustomizationState,
                                    /*elementCustomizationState=*/nullptr);
                            } else {
                                // Transitioning from dynamic to static for this
                                // visual state: clear template metadata and
                                // unregister consumer entries.
                                if (propertyCustomizationState
                                        .dynamicTemplate) {
                                    UpdateStyleVariableConsumers(
                                        state, elementId, property,
                                        /*fallbackClassName=*/nullptr,
                                        propertyCustomizationState
                                            .variableDependencies,
                                        {});
                                    propertyCustomizationState
                                        .variableDependencies.clear();
                                    propertyCustomizationState.dynamicTemplate
                                        .reset();
                                }

                                resolved = it->second;
                            }

                            if (resolved) {
                                AdoptExternalValueAsOriginal(
                                    element, property,
                                    &propertyCustomizationState);
                                if (!propertyCustomizationState.originalValue) {
                                    propertyCustomizationState.originalValue =
                                        ReadLocalValueWithWorkaround(element,
                                                                     property);
                                }

                                propertyCustomizationState.customValue =
                                    *resolved;
                                propertyCustomizationState.lastAppliedValue =
                                    SetOrClearValue(element, property,
                                                    *resolved);
                            }
                        } else {
                            if (propertyCustomizationState.dynamicTemplate) {
                                UpdateStyleVariableConsumers(
                                    state, elementId, property,
                                    /*fallbackClassName=*/nullptr,
                                    propertyCustomizationState
                                        .variableDependencies,
                                    {});
                                propertyCustomizationState.variableDependencies
                                    .clear();
                                propertyCustomizationState.dynamicTemplate
                                    .reset();
                            }
                            UnapplyStyleValue(element, property,
                                              &propertyCustomizationState);
                        }
                    }

                    g_elementPropertyModifying = false;
                });
    }
}

void RestoreCustomizationsForVisualStateGroup(
    StyleVariableState* state,
    ElementId elementId,
    FrameworkElement element,
    std::optional<winrt::weak_ref<VisualStateGroup>>
        visualStateGroupOptionalWeakPtr,
    const ElementCustomizationStateForVisualStateGroup&
        elementCustomizationStateForVisualStateGroup) {
    if (element) {
        for (const auto& [property, propState] :
             elementCustomizationStateForVisualStateGroup
                 .propertyCustomizationStates) {
            try {
                element.UnregisterPropertyChangedCallback(
                    property, propState.propertyChangedToken);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }

            if (!propState.variableDependencies.empty()) {
                UpdateStyleVariableConsumers(state, elementId, property,
                                             /*fallbackClassName=*/nullptr,
                                             propState.variableDependencies,
                                             {});
            }

            if (propState.originalValue) {
                SetOrClearValue(element, property, *propState.originalValue);
            }
        }
    } else {
        // Element is gone; still clear consumer entries so a stale (elementId,
        // property) pair isn't visited during PropagateStyleVariableChange.
        for (const auto& [property, propState] :
             elementCustomizationStateForVisualStateGroup
                 .propertyCustomizationStates) {
            if (!propState.variableDependencies.empty()) {
                UpdateStyleVariableConsumers(state, elementId, property,
                                             /*fallbackClassName=*/nullptr,
                                             propState.variableDependencies,
                                             {});
            }
        }
    }

    auto visualStateGroupIter = visualStateGroupOptionalWeakPtr
                                    ? visualStateGroupOptionalWeakPtr->get()
                                    : nullptr;
    if (visualStateGroupIter && elementCustomizationStateForVisualStateGroup
                                    .visualStateGroupCurrentStateChangedToken) {
        try {
            visualStateGroupIter.CurrentStateChanged(
                elementCustomizationStateForVisualStateGroup
                    .visualStateGroupCurrentStateChangedToken);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }
}

// Item elements the current virtualization pass recycled, consumed by the
// matching ElementPrepared. A freshly created element is prepared without ever
// having been cleared, and its Add mutation already applied the styles, so
// re-matching it would restore and re-push every value for nothing.
thread_local std::unordered_set<ElementId> g_recycledElements;

// The item each element was last matched against, which lets a reuse for that
// same item be left alone. A layout can realize elements during measure only to
// ask for their size and recycle them again during arrange, so an element is
// cleared and handed straight back for the same item on every layout pass.
// Re-matching there sets dependency properties from inside the pass, which
// dirties layout and schedules another one, and layout never settles: XAML
// gives up after enough passes and fails the process with a layout cycle.
thread_local std::unordered_map<ElementId, winrt::weak_ref<wf::IInspectable>>
    g_elementMatchedItems;

struct VirtualizingRepeaterState {
    muxc::ItemsRepeater::ElementClearing_revoker elementClearingRevoker;
    muxc::ItemsRepeater::ElementPrepared_revoker elementPreparedRevoker;
};

thread_local std::unordered_map<ElementId, VirtualizingRepeaterState>
    g_virtualizingRepeaters;

// The id of an element which was reached some other way, e.g. by walking the
// visual tree. None for an element the mutation callbacks never reported, which
// leaves callers to skip it rather than key it by something made up.
ElementId ElementIdFromElement(FrameworkElement const& element) {
    if (!element) {
        return ElementId::None;
    }

    try {
        auto it = g_elementIds.find(HandleFromInspectable(element));
        if (it == g_elementIds.end() || it->second.element.get() != element) {
            return ElementId::None;
        }

        return it->second.id;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        return ElementId::None;
    }
}

// Tear down and re-match every element of a subtree. The whole subtree is
// revisited rather than only its root, since a rule can match a descendant
// through a condition on an ancestor, and descendants of a reused element get
// no mutation of their own.
void ReapplyCustomizationsForSubtree(FrameworkElement element) {
    // Caught per element, both because these run from a layout pass the caller
    // can't fail, and so that one element's failure doesn't skip the rest of
    // the subtree.
    try {
        if (auto elementId = ElementIdFromElement(element);
            elementId != ElementId::None) {
            CleanupCustomizations(elementId);
            auto className = winrt::get_class_name(element);
            ApplyCustomizations(elementId, element, className.c_str());
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    // Snapshotted because applying a style runs arbitrary XAML work which can
    // change the children collection mid-walk.
    std::vector<FrameworkElement> children;
    try {
        int count = Media::VisualTreeHelper::GetChildrenCount(element);
        for (int i = 0; i < count; i++) {
            if (auto child = Media::VisualTreeHelper::GetChild(element, i)
                                 .try_as<FrameworkElement>()) {
                children.push_back(std::move(child));
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        return;
    }

    for (const auto& child : children) {
        ReapplyCustomizationsForSubtree(child);
    }
}

// A weak reference to the item a repeater realized an element for, empty when
// there is no such item or it supports no weak reference. Weak so that a
// destroyed item can't be mistaken for a successor at the same address, which
// would leave an element wearing the styles matched for its predecessor.
winrt::weak_ref<wf::IInspectable> RepeaterItemAt(
    muxc::ItemsRepeater const& repeater,
    int index) {
    try {
        auto itemsSourceView = repeater.ItemsSourceView();
        if (!itemsSourceView || index < 0 || index >= itemsSourceView.Count()) {
            return nullptr;
        }

        return TryMakeWeak(itemsSourceView.GetAt(index));
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        return nullptr;
    }
}

// A virtualizing container recycles its item elements instead of destroying
// them: the recycle pool collapses the element and leaves it parented, so XAML
// diagnostics reports no Remove/Add mutation and the styles matched for the
// previous item would stay on the element once it's reused for another one.
// Treat a cleared element as removed and a prepared one as newly added.
void HandleVirtualizingRepeater(ElementId elementId, FrameworkElement element) {
    auto repeater = element.try_as<muxc::ItemsRepeater>();
    if (!repeater || g_virtualizingRepeaters.contains(elementId)) {
        return;
    }

    Wh_Log(L"Tracking recycling of %s", winrt::get_class_name(element).c_str());

    auto& state = g_virtualizingRepeaters[elementId];

    state.elementClearingRevoker = repeater.ElementClearing(
        winrt::auto_revoke,
        [](muxc::ItemsRepeater const&,
           muxc::ItemsRepeaterElementClearingEventArgs const& args) {
            auto element = args.Element().try_as<FrameworkElement>();
            if (!element) {
                return;
            }

            auto elementId = ElementIdFromElement(element);
            if (elementId == ElementId::None) {
                return;
            }

            Wh_Log(L"Element cleared: %llu", static_cast<uint64_t>(elementId));

            // Nothing is restored here. Whether the styles still apply depends
            // on the item the element is handed back for, which only the
            // matching ElementPrepared knows, and until then they sit on an
            // element the recycle pool keeps out of sight.
            g_recycledElements.insert(elementId);
        });

    state.elementPreparedRevoker = repeater.ElementPrepared(
        winrt::auto_revoke,
        [](muxc::ItemsRepeater const& sender,
           muxc::ItemsRepeaterElementPreparedEventArgs const& args) {
            auto element = args.Element().try_as<FrameworkElement>();
            if (!element) {
                return;
            }

            auto elementId = ElementIdFromElement(element);
            if (elementId == ElementId::None) {
                return;
            }

            auto item = RepeaterItemAt(sender, args.Index());

            // Held across the walk below, so that an item which no weak
            // reference can get back, such as one a source boxes anew on every
            // read, is never recorded: an entry which could match nothing would
            // keep the element held for good.
            auto strongItem = item.get();

            if (!g_recycledElements.erase(elementId)) {
                // Freshly created, so the styles its Add mutation applied are
                // the ones for this item, and only the item is recorded.
                if (strongItem) {
                    g_elementMatchedItems[elementId] = std::move(item);
                }
                return;
            }

            if (strongItem) {
                auto it = g_elementMatchedItems.find(elementId);
                if (it != g_elementMatchedItems.end() &&
                    it->second.get() == strongItem) {
                    Wh_Log(L"Element reused for the same item: %llu",
                           static_cast<uint64_t>(elementId));
                    return;
                }
            }

            Wh_Log(L"Element reused: %llu", static_cast<uint64_t>(elementId));

            ReapplyCustomizationsForSubtree(element);

            // After the walk, which erases the entry as part of the teardown.
            if (strongItem) {
                g_elementMatchedItems[elementId] = std::move(item);
            }
        });
}

void MergeResourceVariables();

void ApplyCustomizations(ElementId elementId,
                         FrameworkElement element,
                         PCWSTR fallbackClassName) {
    // Merge resource dictionary on first element add. Merging it earlier on
    // window creation doesn't work, perhaps merged dictionaries are reset
    // during initialization.
    if (!g_resourceVariablesThemeDict) {
        MergeResourceVariables();
    }

    // Before the early return below: a repeater rarely has styles of its own,
    // but its item elements do.
    HandleVirtualizingRepeater(elementId, element);

    auto* state = GetStyleVariableState();
    if (!state) {
        Wh_Log(L"No XamlRoot for %s, skipping",
               winrt::get_class_name(element).c_str());
        return;
    }

    auto resolved = FindElementPropertyOverrides(element, fallbackClassName);
    if (resolved.overridesPerVSG.empty() && resolved.captures.empty()) {
        return;
    }

    Wh_Log(L"Applying styles to %s", winrt::get_class_name(element).c_str());

    auto& elementCustomizationState = g_elementsCustomizationState[elementId];

    for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
        RestoreCustomizationsForVisualStateGroup(
            state, elementId, element, visualStateGroupOptionalWeakPtrIter,
            stateIter);
    }

    elementCustomizationState.element = element;
    elementCustomizationState.perVisualStateGroup.clear();

    // Elements that neither capture nor consume a variable pay nothing. The
    // rest get their spine now that the element has been matched; if it isn't
    // attached yet the spine stops at a placeholder root, which
    // EnsureElementTreeNode rebuilds on first use once the element is actually
    // in the tree. Cleared unconditionally so a re-apply that drops all
    // variable use cannot leave a stale node behind.
    elementCustomizationState.treeNode = nullptr;
    if (!resolved.captures.empty() || resolved.hasDynamicValues) {
        elementCustomizationState.treeNode =
            GetOrCreateElementTreeNode(element);
    }

    // Wire up captures first so any variables they define are visible to
    // dynamic value-rules applied below. Note: SetUpCapturesForElement does not
    // need this element's fallbackClassName -- propagation routes through each
    // consumer's own stored fallback.
    SetUpCapturesForElement(state, elementId, element, resolved.captures,
                            &elementCustomizationState);

    for (auto& [visualStateGroup, overridesForVisualStateGroup] :
         resolved.overridesPerVSG) {
        std::optional<winrt::weak_ref<VisualStateGroup>>
            visualStateGroupOptionalWeakPtr;
        if (visualStateGroup) {
            visualStateGroupOptionalWeakPtr = visualStateGroup;
        }

        elementCustomizationState.perVisualStateGroup.push_back(
            {visualStateGroupOptionalWeakPtr, {}});
        auto* elementCustomizationStateForVisualStateGroup =
            &elementCustomizationState.perVisualStateGroup.back().second;

        ApplyCustomizationsForVisualStateGroup(
            state, elementId, element, visualStateGroup, fallbackClassName,
            std::move(overridesForVisualStateGroup),
            elementCustomizationStateForVisualStateGroup);
    }
}

// The diagnostics create a runtime object which holds every element they
// report, and drop it only once the element is reported as removed. Removals
// are reported for elements taken out of their parent, which is how a recycled
// list item is released, but a tree which is discarded whole is never taken
// apart that way: dropping its root would destroy it. Holding every element is
// what stops that destruction, so nothing is ever removed and nothing is ever
// reported. Elements nothing is keyed by are handed back from here so the
// teardown can happen.
//
// Reporting an element recreates the runtime object of its parent, which is why
// each report queues the parent as well, and why the queue is drained only once
// the burst of reports has stopped: releasing mid-burst would just be undone by
// the next child of whatever was released.
thread_local std::vector<InstanceHandle> g_pendingDiagnosticsRelease;
thread_local ULONGLONG g_lastDiagnosticsReleaseQueueTick;
thread_local bool g_diagnosticsReleaseDrainQueued;
thread_local winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer
    g_diagnosticsReleaseDrainTimer{nullptr};
thread_local winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer::
    Tick_revoker g_diagnosticsReleaseDrainTimerTickRevoker;

// Long enough to sit out a tree being built.
constexpr ULONGLONG kDiagnosticsReleaseDelay = 200;

// The drain waits on a one-shot timer, which the thread teardown can stop,
// rather than on a dispatcher item, which it cannot: the module is freed once
// the mod is uninitialized, and an item still on the dispatcher would call into
// it. The interval only has to carry the drain out of the report which arms it.
constexpr ULONGLONG kDiagnosticsReleaseDrainDelay = 1;

// Releasing an element the mod still records something for would strand that
// recording, since no removal is reported for a handle whose runtime object is
// gone. Such an element stays held, and its own removal releases it.
//
// Which leaves a styled element of a tree discarded whole held for good, and
// with it everything below it, since a parent holds its children. Rules that
// match File Explorer match almost nothing of such a tree, so what this retains
// is small, but a rule written against a bare type would retain much more.
bool ElementHasState(ElementId elementId) {
    if (elementId == ElementId::None) {
        return false;
    }

    if (g_elementsCustomizationState.contains(elementId) ||
        g_virtualizingRepeaters.contains(elementId) ||
        g_recycledElements.contains(elementId) ||
        g_elementMatchedItems.contains(elementId)) {
        return true;
    }

    if (g_styleVariableState.elementRefs.contains(elementId)) {
        return true;
    }

    for (const auto& propagation : g_pendingStyleVariablePropagations) {
        if (propagation.changedOwner == elementId) {
            return true;
        }
    }

    return false;
}

void FlushDiagnosticsReleases() {
    auto pending = std::move(g_pendingDiagnosticsRelease);
    g_pendingDiagnosticsRelease.clear();

    if (!g_visualTreeWatcher) {
        return;
    }

    // A handle is queued once per report naming it, so a parent appears once
    // per child, and each repeat would pay for another ElementHasState scan.
    std::sort(pending.begin(), pending.end());
    pending.erase(std::unique(pending.begin(), pending.end()), pending.end());

    for (InstanceHandle handle : pending) {
        if (ElementHasState(FindElementId(handle))) {
            continue;
        }

        if (g_visualTreeWatcher->ReleaseDiagnosticsReference(handle)) {
            ForgetElementId(handle);
        }
    }

    // Here rather than anywhere else on the report path: the releases above are
    // what let elements be destroyed unreported, and this runs from the
    // dispatcher, so the teardown of what they were keyed by is outside the
    // walk the reports came from.
    ReapDeadElementIdsIfNeeded();
}

void QueueDiagnosticsRelease(InstanceHandle handle) {
    if (!handle) {
        return;
    }

    g_pendingDiagnosticsRelease.push_back(handle);
    g_lastDiagnosticsReleaseQueueTick = GetTickCount64();
}

// Whether the burst has stopped is decided when this is scheduled: the report
// which schedules it queues its own handles right afterwards, so the time since
// the last queue is short again by the time this runs.
void DrainDiagnosticsReleases() {
    g_diagnosticsReleaseDrainQueued = false;
    FlushDiagnosticsReleases();
}

// Reports arrive from inside XAML's own Enter and Leave walks, and a release
// there re-enters the diagnostics while the tree is being mutated: dropping the
// last reference to an element the walk is still visiting destroys it mid-walk.
// The drain is therefore armed on the thread's dispatcher, which runs it once
// the walk has finished.
//
// Whether to arm it is decided by the next report rather than by a recurring
// timer, so that nothing of the mod is left waiting on a thread it does not
// tear down. A thread which goes quiet therefore holds its last burst until it
// is used again.
void FlushDiagnosticsReleasesIfQuiet() {
    if (g_pendingDiagnosticsRelease.empty() ||
        g_diagnosticsReleaseDrainQueued ||
        GetTickCount64() - g_lastDiagnosticsReleaseQueueTick <
            kDiagnosticsReleaseDelay) {
        return;
    }

    try {
        if (!g_diagnosticsReleaseDrainTimer) {
            auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::
                DispatcherQueue::GetForCurrentThread();
            if (!dispatcherQueue) {
                // Releasing from here is the one thing that isn't safe, so the
                // elements stay held instead.
                Wh_Log(L"No dispatcher queue, elements will be held");
                return;
            }

            g_diagnosticsReleaseDrainTimer = dispatcherQueue.CreateTimer();
            g_diagnosticsReleaseDrainTimer.IsRepeating(false);
            g_diagnosticsReleaseDrainTimer.Interval(
                std::chrono::milliseconds{kDiagnosticsReleaseDrainDelay});
            g_diagnosticsReleaseDrainTimerTickRevoker =
                g_diagnosticsReleaseDrainTimer.Tick(
                    winrt::auto_revoke,
                    [](winrt::Microsoft::UI::Dispatching::
                           DispatcherQueueTimer const&,
                       winrt::Windows::Foundation::IInspectable const&) {
                        DrainDiagnosticsReleases();
                    });
        }

        g_diagnosticsReleaseDrainTimer.Start();
        g_diagnosticsReleaseDrainQueued = true;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

void StopDiagnosticsReleases() {
    g_pendingDiagnosticsRelease.clear();

    if (g_diagnosticsReleaseDrainTimer) {
        try {
            g_diagnosticsReleaseDrainTimer.Stop();
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    g_diagnosticsReleaseDrainTimerTickRevoker.revoke();
    g_diagnosticsReleaseDrainTimer = nullptr;
    g_diagnosticsReleaseDrainQueued = false;
}

void CleanupCustomizations(ElementId elementId) {
    // Unconditional: a repeater, or an item element which matched no rule, has
    // no customization state but can still have virtualization bookkeeping.
    g_virtualizingRepeaters.erase(elementId);
    g_recycledElements.erase(elementId);
    g_elementMatchedItems.erase(elementId);

    auto it = g_elementsCustomizationState.find(elementId);
    if (it == g_elementsCustomizationState.end()) {
        return;
    }

    // A reference rather than the iterator: restoring a style below runs
    // arbitrary XAML work that can re-enter ApplyCustomizations and rehash
    // g_elementsCustomizationState, which invalidates iterators but not
    // references to the mapped values. A re-entrant cleanup or re-apply of this
    // same elementId would invalidate both the reference and the loop below,
    // but the re-entrancy is for other elements, not the one being torn down
    // here.
    auto& elementCustomizationState = it->second;

    auto element = elementCustomizationState.element.get();
    auto* state = GetStyleVariableState();

    RestoreCapturesForElement(element, elementCustomizationState);

    // Drop this element's captures from the registry. Other elements may still
    // capture the same names, so a name only becomes undefined once its last
    // capture is gone. Runs after RestoreCapturesForElement so the
    // just-unregistered capture callbacks can't re-seed a variable
    // mid-teardown.
    std::vector<std::wstring> removedVarNames;
    if (state) {
        for (const auto& [property, captureState] :
             elementCustomizationState.captureCustomizationStates) {
            if (captureState.varName.empty()) {
                continue;
            }

            auto varIt = state->variables.find(captureState.varName);
            if (varIt == state->variables.end()) {
                continue;
            }

            size_t removed =
                std::erase_if(varIt->second,
                              [elementId](const StyleVariableCapture& capture) {
                                  return capture.elementId == elementId;
                              });
            if (!removed) {
                continue;
            }

            ReleaseStyleVariableElementRefs(state, elementId, removed);

            removedVarNames.push_back(captureState.varName);
            if (varIt->second.empty()) {
                state->variables.erase(varIt);
            }
        }
    }

    for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
        RestoreCustomizationsForVisualStateGroup(
            state, elementId, element, visualStateGroupOptionalWeakPtrIter,
            stateIter);
    }

    // By elementId, not by `it`: a re-entrant apply above may have rehashed the
    // map since the lookup.
    g_elementsCustomizationState.erase(elementId);

    ReapElementTreeNodesIfNeeded();

    // Deferred until this element is out of g_elementsCustomizationState, both
    // so it can't be scored as a winning capture while being torn down and so
    // the loops above don't walk state that re-entrant style applies could
    // invalidate. Every removal propagates, not just the one that left a name
    // undefined: dropping one of several captures still changes which one wins
    // for the consumers that were closest to it.
    for (const auto& varName : removedVarNames) {
        PropagateStyleVariableChange(state, varName, std::nullopt);
    }
}

using StyleConstant = std::pair<std::wstring, std::wstring>;
using StyleConstants = std::vector<StyleConstant>;

std::wstring ApplyStyleConstants(std::wstring_view style,
                                 const StyleConstants& styleConstants) {
    std::wstring result;

    size_t lastPos = 0;
    size_t findPos;

    while ((findPos = style.find('$', lastPos)) != style.npos) {
        result.append(style, lastPos, findPos - lastPos);

        const StyleConstant* constant = nullptr;
        for (const auto& s : styleConstants) {
            if (s.first == style.substr(findPos + 1, s.first.size())) {
                constant = &s;
                break;
            }
        }

        if (constant) {
            result += constant->second;
            lastPos = findPos + 1 + constant->first.size();
        } else {
            result += '$';
            lastPos = findPos + 1;
        }
    }

    // Care for the rest after last occurrence.
    result += style.substr(lastPos);

    return result;
}

std::optional<StyleConstant> ParseStyleConstant(
    std::wstring_view constant,
    const StyleConstants& styleConstants) {
    // Skip if commented.
    if (constant.starts_with(L"//")) {
        return std::nullopt;
    }

    auto eqPos = constant.find(L'=');
    if (eqPos == constant.npos) {
        Wh_Log(L"Skipping entry with no '=': %.*s",
               static_cast<int>(constant.length()), constant.data());
        return std::nullopt;
    }

    auto key = TrimStringView(constant.substr(0, eqPos));
    auto valueRaw = TrimStringView(constant.substr(eqPos + 1));
    auto value = ApplyStyleConstants(valueRaw, styleConstants);

    return StyleConstant{std::wstring(key), std::move(value)};
}

StyleConstants LoadStyleConstants(
    const std::vector<PCWSTR>& themeStyleConstants) {
    StyleConstants result;

    auto addToResult = [&result](StyleConstant sc) {
        // Keep sorted by name length to replace long names first. Reverse the
        // order to allow overriding definitions with the same name.
        auto insertIndex = std::lower_bound(
            result.begin(), result.end(), sc,
            [](const StyleConstant& a, const StyleConstant& b) {
                return a.first.size() > b.first.size();
            });

        result.insert(insertIndex, std::move(sc));
    };

    for (const auto themeStyleConstant : themeStyleConstants) {
        if (auto parsed = ParseStyleConstant(themeStyleConstant, result)) {
            addToResult(std::move(*parsed));
        }
    }

    for (int i = 0;; i++) {
        string_setting_unique_ptr constantSetting(
            Wh_GetStringSetting(L"styleConstants[%d]", i));
        if (!*constantSetting.get()) {
            break;
        }

        if (auto parsed = ParseStyleConstant(constantSetting.get(), result)) {
            addToResult(std::move(*parsed));
        }
    }

    return result;
}

ElementMatcher ElementMatcherFromString(std::wstring_view str) {
    ElementMatcher result;
    PropertyValuesUnresolved propertyValuesUnresolved;

    auto trimmed = TrimStringView(str);
    if (trimmed == L"*") {
        result.kind = ElementMatcher::Kind::Wildcard;
        return result;
    }
    if (trimmed == L":root") {
        result.kind = ElementMatcher::Kind::Root;
        return result;
    }

    auto i = str.find_first_of(L"#@[");
    result.type = TrimStringView(str.substr(0, i));
    if (result.type.empty()) {
        throw std::runtime_error("Bad target syntax, empty type");
    }

    while (i != str.npos) {
        auto iNext = str.find_first_of(L"#@[", i + 1);
        auto nextPart =
            str.substr(i + 1, iNext == str.npos ? str.npos : iNext - (i + 1));

        switch (str[i]) {
            case L'#':
                if (!result.name.empty()) {
                    throw std::runtime_error(
                        "Bad target syntax, more than one name");
                }

                result.name = TrimStringView(nextPart);
                if (result.name.empty()) {
                    throw std::runtime_error("Bad target syntax, empty name");
                }
                break;

            case L'@':
                if (result.visualStateGroupName) {
                    throw std::runtime_error(
                        "Bad target syntax, more than one visual state group");
                }

                result.visualStateGroupName = TrimStringView(nextPart);
                break;

            case L'[': {
                auto rule = TrimStringView(nextPart);
                if (rule.length() == 0 || rule.back() != L']') {
                    throw std::runtime_error("Bad target syntax, missing ']'");
                }

                rule = TrimStringView(rule.substr(0, rule.length() - 1));
                if (rule.length() == 0) {
                    throw std::runtime_error(
                        "Bad target syntax, empty property");
                }

                if (rule.find_first_not_of(L"0123456789") == rule.npos) {
                    result.oneBasedIndex = std::stoi(std::wstring(rule));
                    break;
                }

                auto ruleEqPos = rule.find(L'=');
                if (ruleEqPos == rule.npos) {
                    throw std::runtime_error(
                        "Bad target syntax, missing '=' in property");
                }

                auto ruleKey = TrimStringView(rule.substr(0, ruleEqPos));
                auto ruleVal = TrimStringView(rule.substr(ruleEqPos + 1));

                if (ruleKey.length() == 0) {
                    throw std::runtime_error(
                        "Bad target syntax, empty property name");
                }

                propertyValuesUnresolved.push_back(
                    {std::wstring(ruleKey), std::wstring(ruleVal)});
                break;
            }

            default:
                throw std::runtime_error("Bad target syntax");
        }

        i = iNext;
    }

    result.propertyValues = std::move(propertyValuesUnresolved);

    return result;
}

// Parses a single `controlStyles[*].styles[*]` entry into either a ValueRule
// (`Property[@VisualState][:]=value`) or a CaptureRule (`Property=>VarName`).
// Throws std::runtime_error on malformed input or disallowed combinations such
// as `:=>` or `@VisualState=>`.
std::variant<ValueRule, CaptureRule> ParseRule(std::wstring_view str) {
    auto eqPos = str.find(L'=');
    if (eqPos == str.npos) {
        throw std::runtime_error("Bad style syntax, '=' is missing");
    }

    auto name = str.substr(0, eqPos);
    auto value = str.substr(eqPos + 1);

    if (!value.empty() && value.front() == L'>') {
        // Capture rule: `Property=>VarName`. The right-hand side (after the
        // leading `>` marker) is the name of a mod-global style variable into
        // which the property's current value is captured.
        value = value.substr(1);

        if (!name.empty() && name.back() == L':') {
            throw std::runtime_error(
                "Bad style syntax, ':=>' is not valid (':=' XAML value "
                "cannot be combined with '=>' capture)");
        }

        if (name.find(L'@') != name.npos) {
            throw std::runtime_error(
                "Bad style syntax, '@VisualState' not allowed on a capture "
                "rule");
        }

        auto trimmedPropertyName = TrimStringView(name);
        if (trimmedPropertyName.empty()) {
            throw std::runtime_error("Bad style syntax, empty name");
        }

        auto trimmedVarName = TrimStringView(value);
        if (trimmedVarName.empty()) {
            throw std::runtime_error(
                "Bad style syntax, empty capture variable name");
        }
        if (!IsValidStyleVariableIdentifier(trimmedVarName)) {
            throw std::runtime_error(
                "Bad style syntax, invalid capture variable name");
        }

        return CaptureRule{std::wstring(trimmedPropertyName),
                           std::wstring(trimmedVarName)};
    }

    ValueRule result;
    result.value = TrimStringView(value);

    if (!name.empty() && name.back() == L':') {
        result.isXamlValue = true;
        name = name.substr(0, name.size() - 1);
    }

    auto atPos = name.find(L'@');
    if (atPos != name.npos) {
        result.visualState = TrimStringView(name.substr(atPos + 1));
        name = name.substr(0, atPos);
    }

    result.propertyName = TrimStringView(name);
    if (result.propertyName.empty()) {
        throw std::runtime_error("Bad style syntax, empty name");
    }

    return result;
}

std::wstring AdjustTypeName(std::wstring_view type) {
    if (type.find_first_of(L".:") == type.npos) {
        if (type == L"Rectangle") {
            return L"Microsoft.UI.Xaml.Shapes.Rectangle";
        }

        return L"Microsoft.UI.Xaml.Controls." + std::wstring{type};
    }

    static const std::vector<std::pair<std::wstring_view, std::wstring_view>>
        adjustments = {
            {L"muxc:", L"Microsoft.UI.Xaml.Controls."},
    };

    for (const auto& adjustment : adjustments) {
        if (type.starts_with(adjustment.first)) {
            auto result = std::wstring{adjustment.second};
            result += type.substr(adjustment.first.size());
            return result;
        }
    }

    return std::wstring{type};
}

// Splits a target string on the commas which separate targets, ignoring commas
// which are part of a `[Property=Value]` clause.
std::vector<std::wstring_view> SplitTargetString(std::wstring_view target) {
    std::vector<std::wstring_view> result;

    size_t partBegin = 0;
    bool inProperty = false;
    for (size_t i = 0; i < target.size(); i++) {
        switch (target[i]) {
            case L'[':
                inProperty = true;
                break;

            case L']':
                inProperty = false;
                break;

            case L',':
                if (!inProperty) {
                    result.push_back(target.substr(partBegin, i - partBegin));
                    partBegin = i + 1;
                }
                break;
        }
    }

    result.push_back(target.substr(partBegin));

    return result;
}

void AddElementCustomizationRulesForSingleTarget(
    std::wstring_view target,
    const std::vector<std::wstring>& styles) {
    ElementCustomizationRules elementCustomizationRules;

    auto targetParts = SplitStringView(target, L" > ");

    bool first = true;
    bool hasVisualStateGroup = false;
    for (auto i = targetParts.rbegin(); i != targetParts.rend(); ++i) {
        const auto& targetPart = *i;
        const bool isLeftmost = (i + 1 == targetParts.rend());

        auto matcher = ElementMatcherFromString(targetPart);

        const auto& prevParents =
            elementCustomizationRules.parentElementMatchers;
        const bool prevIsWildcard =
            !prevParents.empty() &&
            prevParents.back().kind == ElementMatcher::Kind::Wildcard;

        switch (matcher.kind) {
            case ElementMatcher::Kind::Element:
                matcher.type = AdjustTypeName(matcher.type);
                break;

            case ElementMatcher::Kind::Wildcard:
                if (first) {
                    throw std::runtime_error(
                        "Bad target syntax, '*' can't be the matched element");
                }
                if (isLeftmost) {
                    throw std::runtime_error(
                        "Bad target syntax, '*' can't be the leftmost target "
                        "part");
                }
                if (prevIsWildcard) {
                    throw std::runtime_error(
                        "Bad target syntax, '*' can't be adjacent to another "
                        "'*'");
                }
                break;

            case ElementMatcher::Kind::Root:
                if (first) {
                    throw std::runtime_error(
                        "Bad target syntax, ':root' can't be the matched "
                        "element");
                }
                if (!isLeftmost) {
                    throw std::runtime_error(
                        "Bad target syntax, ':root' must be the leftmost "
                        "target part");
                }
                if (prevIsWildcard) {
                    throw std::runtime_error(
                        "Bad target syntax, ':root' must be followed by a "
                        "non-wildcard target part");
                }
                break;
        }

        if (matcher.visualStateGroupName) {
            if (hasVisualStateGroup) {
                throw std::runtime_error(
                    "Element type can't have more than one visual state group");
            }

            hasVisualStateGroup = true;
        }

        if (first) {
            UnresolvedRules unresolvedRules;
            for (const auto& style : styles) {
                auto parsed = ParseRule(style);
                if (auto* valueRule = std::get_if<ValueRule>(&parsed)) {
                    unresolvedRules.valueRules.push_back(std::move(*valueRule));
                } else {
                    unresolvedRules.captureRules.push_back(
                        std::move(std::get<CaptureRule>(parsed)));
                }
            }

            elementCustomizationRules.elementMatcher = std::move(matcher);
            elementCustomizationRules.propertyOverrides =
                std::move(unresolvedRules);
        } else {
            elementCustomizationRules.parentElementMatchers.push_back(
                std::move(matcher));
        }

        first = false;
    }

    g_elementsCustomizationRules.push_back(
        std::move(elementCustomizationRules));
}

void AddElementCustomizationRules(std::wstring_view target,
                                  const std::vector<std::wstring>& styles) {
    auto targets = SplitTargetString(target);

    for (const auto& singleTarget : targets) {
        try {
            AddElementCustomizationRulesForSingleTarget(singleTarget, styles);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X for target %.*s", ex.code(),
                   static_cast<int>(singleTarget.length()),
                   singleTarget.data());
        } catch (std::exception const& ex) {
            Wh_Log(L"Error for target %.*s: %S",
                   static_cast<int>(singleTarget.length()), singleTarget.data(),
                   ex.what());
        }
    }
}

bool ProcessSingleTargetStylesFromSettings(
    int index,
    const StyleConstants& styleConstants) {
    string_setting_unique_ptr targetStringSetting(
        Wh_GetStringSetting(L"controlStyles[%d].target", index));
    if (!*targetStringSetting.get()) {
        return false;
    }

    // Skip if commented.
    if (targetStringSetting[0] == L'/' && targetStringSetting[1] == L'/') {
        return true;
    }

    Wh_Log(L"Processing styles for %s", targetStringSetting.get());

    std::vector<std::wstring> styles;

    for (int styleIndex = 0;; styleIndex++) {
        string_setting_unique_ptr styleSetting(Wh_GetStringSetting(
            L"controlStyles[%d].styles[%d]", index, styleIndex));
        if (!*styleSetting.get()) {
            break;
        }

        // Skip if commented.
        if (styleSetting[0] == L'/' && styleSetting[1] == L'/') {
            continue;
        }

        styles.push_back(
            ApplyStyleConstants(styleSetting.get(), styleConstants));
    }

    if (styles.size() > 0) {
        AddElementCustomizationRules(targetStringSetting.get(), styles);
    }

    return true;
}

std::optional<ResourceVariableEntry> ParseResourceVariable(
    std::wstring_view entry,
    const StyleConstants& styleConstants) {
    // Skip if commented.
    if (entry.starts_with(L"//")) {
        return std::nullopt;
    }

    // Find the first '=' to split key and value.
    auto eqPos = entry.find(L'=');
    if (eqPos == entry.npos) {
        Wh_Log(L"Skipping entry with no '=': %.*s",
               static_cast<int>(entry.length()), entry.data());
        return std::nullopt;
    }

    auto keyPart = TrimStringView(entry.substr(0, eqPos));
    auto valueRaw = TrimStringView(entry.substr(eqPos + 1));
    auto value = ApplyStyleConstants(valueRaw, styleConstants);

    constexpr std::wstring_view kThemeResourcePrefix = L"{ThemeResource ";

    ResourceVariableType type = ResourceVariableType::String;
    if (keyPart.size() > 0 && keyPart.back() == L':') {
        type = ResourceVariableType::Xaml;
        keyPart = keyPart.substr(0, keyPart.size() - 1);
        keyPart = TrimStringView(keyPart);
    } else if (value.starts_with(kThemeResourcePrefix) &&
               value.ends_with(L"}")) {
        type = ResourceVariableType::ThemeResourceReference;
        value = TrimStringView(
            value.substr(kThemeResourcePrefix.size(),
                         value.size() - kThemeResourcePrefix.size() - 1));
    }

    ResourceVariableTheme theme = ResourceVariableTheme::None;
    std::wstring key;

    // Check for @theme suffix in key part.
    auto atPos = keyPart.find(L'@');
    if (atPos != keyPart.npos) {
        key = TrimStringView(keyPart.substr(0, atPos));
        auto themePart = TrimStringView(keyPart.substr(atPos + 1));
        if (themePart == L"Dark") {
            theme = ResourceVariableTheme::Dark;
        } else if (themePart == L"Light") {
            theme = ResourceVariableTheme::Light;
        } else {
            Wh_Log(L"Unknown theme '%.*s', expected 'Dark' or 'Light'",
                   static_cast<int>(themePart.size()), themePart.data());
            return std::nullopt;
        }
    } else {
        key = std::wstring(keyPart);
    }

    return ResourceVariableEntry{std::move(key), std::move(value), theme, type};
}

winrt::Windows::Foundation::IInspectable ParseXamlValue(
    std::wstring_view xamlValue) {
    std::wstring xaml;
    xaml += L"        <Setter Property=\"Tag\">\n";
    xaml += L"            <Setter.Value>\n";
    xaml += xamlValue;
    xaml += L"\n";
    xaml += L"            </Setter.Value>\n";
    xaml += L"        </Setter>\n";

    auto style = GetStyleFromXamlSetters(L"FrameworkElement", xaml);
    return style.Setters().GetAt(0).as<Setter>().Value();
}

bool ProcessResourceVariable(ResourceDictionary resources,
                             ResourceDictionary darkDict,
                             ResourceDictionary lightDict,
                             const ResourceVariableEntry& entry) {
    auto boxedKey = winrt::box_value(entry.key);

    if (entry.theme != ResourceVariableTheme::None) {
        ResourceDictionary& targetDict =
            entry.theme == ResourceVariableTheme::Dark ? darkDict : lightDict;

        if (targetDict.HasKey(boxedKey)) {
            Wh_Log(
                L"Resource variable key '%s' already exists in theme '%s', "
                L"skipping",
                entry.key.c_str(),
                entry.theme == ResourceVariableTheme::Dark ? L"Dark"
                                                           : L"Light");
            return false;
        }

        winrt::Windows::Foundation::IInspectable value;
        switch (entry.type) {
            case ResourceVariableType::String:
                value = winrt::box_value(entry.value);
                break;
            case ResourceVariableType::Xaml:
                value =
                    entry.value.empty() ? nullptr : ParseXamlValue(entry.value);
                break;
            case ResourceVariableType::ThemeResourceReference:
                value = resources.Lookup(winrt::box_value(entry.value));
                break;
        }

        targetDict.Insert(boxedKey, value);

        return true;
    }

    // key= - convert using existing resource type.
    auto existingResource = resources.TryLookup(boxedKey);
    if (!existingResource) {
        Wh_Log(L"Resource variable key '%s' not found, skipping",
               entry.key.c_str());
        return false;
    }

    auto [it, inserted] =
        g_originalResourceValues.try_emplace(entry.key, existingResource);
    if (!inserted) {
        Wh_Log(L"Resource variable key '%s' already modified, skipping",
               entry.key.c_str());
        return false;
    }

    winrt::Windows::Foundation::IInspectable value;
    switch (entry.type) {
        case ResourceVariableType::String: {
            auto resourceClassName = winrt::get_class_name(existingResource);

            // Unwrap IReference<T> to get inner type name.
            if (resourceClassName.starts_with(
                    L"Windows.Foundation.IReference`1<") &&
                resourceClassName.ends_with(L'>')) {
                size_t prefixSize =
                    sizeof("Windows.Foundation.IReference`1<") - 1;
                resourceClassName =
                    winrt::hstring(resourceClassName.data() + prefixSize,
                                   resourceClassName.size() - prefixSize - 1);
            }

            value = Markup::XamlBindingHelper::ConvertValue(
                winrt::Windows::UI::Xaml::Interop::TypeName{resourceClassName},
                winrt::box_value(entry.value));
            break;
        }

        case ResourceVariableType::Xaml:
            value = entry.value.empty() ? nullptr : ParseXamlValue(entry.value);
            break;

        case ResourceVariableType::ThemeResourceReference:
            value = resources.Lookup(winrt::box_value(entry.value));
            break;
    }

    resources.Insert(boxedKey, value);

    return true;
}

void RefreshThemeResourceEntries() {
    if (g_resourceVariables.empty()) {
        return;
    }

    Wh_Log(L"Refreshing theme resource entries");

    auto resources = Application::Current().Resources();

    auto darkDict = g_resourceVariablesThemeDict.ThemeDictionaries()
                        .TryLookup(winrt::box_value(L"Dark"))
                        .try_as<ResourceDictionary>();
    auto lightDict = g_resourceVariablesThemeDict.ThemeDictionaries()
                         .TryLookup(winrt::box_value(L"Light"))
                         .try_as<ResourceDictionary>();

    for (const auto& entry : g_resourceVariables) {
        if (entry.type != ResourceVariableType::ThemeResourceReference) {
            continue;
        }

        try {
            auto boxedKey = winrt::box_value(entry.key);
            auto value = resources.Lookup(winrt::box_value(entry.value));

            if (entry.theme == ResourceVariableTheme::Dark && darkDict) {
                darkDict.Insert(boxedKey, value);
            } else if (entry.theme == ResourceVariableTheme::Light &&
                       lightDict) {
                lightDict.Insert(boxedKey, value);
            } else {
                resources.Insert(boxedKey, value);
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error refreshing '%s': %08X", entry.key.c_str(),
                   ex.code());
        }
    }
}

std::vector<ResourceVariableEntry> ProcessResourceVariablesFromSettings(
    const StyleConstants& styleConstants,
    const std::vector<PCWSTR>& themeResourceVariables) {
    std::vector<ResourceVariableEntry> resourceVariables;

    for (const auto& themeResourceVariable : themeResourceVariables) {
        Wh_Log(L"Processing theme resource variable %s", themeResourceVariable);

        auto parsed =
            ParseResourceVariable(themeResourceVariable, styleConstants);
        if (parsed) {
            resourceVariables.push_back(std::move(*parsed));
        }
    }

    for (int i = 0;; i++) {
        string_setting_unique_ptr setting(
            Wh_GetStringSetting(L"themeResourceVariables[%d]", i));
        if (!*setting.get()) {
            break;
        }

        Wh_Log(L"Processing resource variable %s", setting.get());

        auto parsed = ParseResourceVariable(setting.get(), styleConstants);
        if (parsed) {
            resourceVariables.push_back(std::move(*parsed));
        }
    }

    return resourceVariables;
}

void MergeResourceVariables() {
    auto resources = Application::Current().Resources();

    // Create theme dictionaries for @Dark/@Light resources.
    g_resourceVariablesThemeDict = ResourceDictionary();
    ResourceDictionary darkDict;
    ResourceDictionary lightDict;
    bool hasThemeResources = false;
    bool hasThemeResourceReferences = false;

    for (auto it = g_resourceVariables.rbegin();
         it != g_resourceVariables.rend(); ++it) {
        Wh_Log(L"Processing resource variable %s", it->key.c_str());

        try {
            if (ProcessResourceVariable(resources, darkDict, lightDict, *it)) {
                if (it->theme != ResourceVariableTheme::None) {
                    hasThemeResources = true;
                }

                if (it->type == ResourceVariableType::ThemeResourceReference) {
                    hasThemeResourceReferences = true;
                }
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        } catch (std::exception const& ex) {
            Wh_Log(L"Error: %S", ex.what());
        }
    }

    if (hasThemeResources) {
        g_resourceVariablesThemeDict.ThemeDictionaries().Insert(
            winrt::box_value(L"Dark"), darkDict);
        g_resourceVariablesThemeDict.ThemeDictionaries().Insert(
            winrt::box_value(L"Light"), lightDict);

        resources.MergedDictionaries().Append(g_resourceVariablesThemeDict);
    }

    // Register for color changes to refresh theme resource references.
    if (hasThemeResourceReferences) {
        g_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
        auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::
            DispatcherQueue::GetForCurrentThread();
        g_colorValuesChangedToken =
            g_uiSettings.ColorValuesChanged([dispatcherQueue](auto&&, auto&&) {
                dispatcherQueue.TryEnqueue(RefreshThemeResourceEntries);
            });
    }
}

const Theme* GetSelectedTheme() {
    PCWSTR themeName = Wh_GetStringSetting(L"theme");
    const Theme* theme = nullptr;
    if (wcscmp(themeName, L"Translucent Explorer11") == 0) {
        theme = &g_themeTranslucent_Explorer11;
    } else if (wcscmp(themeName, L"MicaBar") == 0) {
        theme = &g_themeMicaBar;
    } else if (wcscmp(themeName, L"NoCommandBar") == 0) {
        theme = &g_themeNoCommandBar;
    } else if (wcscmp(themeName, L"Minimal Explorer11") == 0) {
        theme = &g_themeMinimal_Explorer11;
    } else if (wcscmp(themeName, L"Tabless") == 0) {
        theme = &g_themeTabless;
    } else if (wcscmp(themeName, L"Matter") == 0) {
        theme = &g_themeMatter;
    } else if (wcscmp(themeName, L"WindowGlass") == 0) {
        theme = &g_themeWindowGlass;
    } else if (wcscmp(themeName, L"AddressSearchOnly") == 0) {
        theme = &g_themeAddressSearchOnly;
    } else if (wcscmp(themeName, L"TintedGlass") == 0) {
        theme = &g_themeTintedGlass;
    } else if (wcscmp(themeName, L"LiquidGlass") == 0) {
        theme = &g_themeLiquidGlass;
    } else if (wcscmp(themeName, L"MicaTabless") == 0) {
        theme = &g_themeMicaTabless;
    } else if (wcscmp(themeName, L"OS26 Liquid Glass") == 0) {
        theme = &g_themeOS26_Liquid_Glass;
    } else if (wcscmp(themeName, L"OS26 Liquid Glass_variant_Compact") == 0) {
        theme = &g_themeOS26_Liquid_Glass_variant_Compact;
    } else if (wcscmp(themeName, L"ZEUSosX_044") == 0) {
        theme = &g_themeZEUSosX_044;
    } else if (wcscmp(themeName, L"Compact Explorer11") == 0) {
        theme = &g_themeCompact_Explorer11;
    } else if (wcscmp(themeName, L"Float") == 0) {
        theme = &g_themeFloat;
    }
    Wh_FreeStringSetting(themeName);
    return theme;
}

void ProcessAllStylesFromSettings() {
    const Theme* theme = GetSelectedTheme();

    StyleConstants styleConstants = LoadStyleConstants(
        theme ? theme->styleConstants : std::vector<PCWSTR>{});

    if (theme) {
        for (const auto& themeTargetStyle : theme->targetStyles) {
            try {
                std::vector<std::wstring> styles;
                styles.reserve(themeTargetStyle.styles.size());
                for (const auto& s : themeTargetStyle.styles) {
                    styles.push_back(ApplyStyleConstants(s, styleConstants));
                }

                AddElementCustomizationRules(themeTargetStyle.target, styles);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error %08X", ex.code());
            } catch (std::exception const& ex) {
                Wh_Log(L"Error: %S", ex.what());
            }
        }
    }

    for (int i = 0;; i++) {
        try {
            if (!ProcessSingleTargetStylesFromSettings(i, styleConstants)) {
                break;
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        } catch (std::exception const& ex) {
            Wh_Log(L"Error: %S", ex.what());
        }
    }

    g_resourceVariables = ProcessResourceVariablesFromSettings(
        styleConstants,
        theme ? theme->themeResourceVariables : std::vector<PCWSTR>{});
}

void UninitializeResourceVariables() {
    // Unregister color change handler.
    if (g_colorValuesChangedToken) {
        g_uiSettings.ColorValuesChanged(g_colorValuesChangedToken);
        g_colorValuesChangedToken = {};
    }
    g_uiSettings = nullptr;
    g_resourceVariables.clear();

    if (g_originalResourceValues.empty() && !g_resourceVariablesThemeDict) {
        return;
    }

    // Restore original resource values.
    auto resources = Application::Current().Resources();
    for (const auto& [key, originalValue] : g_originalResourceValues) {
        try {
            resources.Insert(winrt::box_value(key), originalValue);
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }
    g_originalResourceValues.clear();

    // Remove our merged theme dictionary.
    if (g_resourceVariablesThemeDict) {
        auto merged = resources.MergedDictionaries();
        uint32_t index;
        if (merged.IndexOf(g_resourceVariablesThemeDict, index)) {
            merged.RemoveAt(index);
        }
        g_resourceVariablesThemeDict = nullptr;
    }
}

void UninitializeForCurrentThread() {
    // Clear tracked images for this thread (revokers will automatically
    // unregister).
    if (auto& timer = g_trackedImagesForThread.retryTimer) {
        try {
            timer.Stop();
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }
    g_trackedImagesForThread.retryTimerTickRevoker.revoke();
    g_trackedImagesForThread.retryTimer = nullptr;
    g_trackedImagesForThread.retryDueTick = 0;
    g_trackedImagesForThread.images.clear();
    g_imageCacheUriRemotes.clear();
    StopImageLoadRetriesForCurrentThread();

    // Before the teardown below, so that no recycling callback can fire into
    // half-cleared state, and no pending release can consult state which is
    // being cleared.
    StopDiagnosticsReleases();
    g_virtualizingRepeaters.clear();
    g_recycledElements.clear();
    g_elementMatchedItems.clear();

    // Detached from the global before being walked: restoring a value runs
    // arbitrary XAML work, and whatever it re-enters looks its elements up in
    // g_elementsCustomizationState. Walking a map nothing else can reach keeps
    // a re-entrant insert or erase from invalidating this loop, and leaving the
    // global empty makes those lookups miss, which is what teardown wants.
    auto elementsCustomizationState = std::move(g_elementsCustomizationState);
    g_elementsCustomizationState.clear();

    for (const auto& [elementId, elementCustomizationState] :
         elementsCustomizationState) {
        auto element = elementCustomizationState.element.get();
        auto* state = GetStyleVariableState();

        RestoreCapturesForElement(element, elementCustomizationState);

        for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
             elementCustomizationState.perVisualStateGroup) {
            RestoreCustomizationsForVisualStateGroup(
                state, elementId, element, visualStateGroupOptionalWeakPtrIter,
                stateIter);
        }
    }

    // Before g_elementTreeNodes, since the states hold the last strong refs to
    // the spine nodes.
    elementsCustomizationState.clear();
    g_elementTreeNodes.clear();
    g_elementTreeNodesReapThreshold = 64;
    g_pendingStyleVariablePropagations.clear();
    g_styleVariableState = {};

    // After everything keyed by an id. g_lastElementId keeps counting, since an
    // id must never name two elements.
    g_elementIds.clear();
    g_elementIdsReapThreshold = 64;

    g_elementsCustomizationRules.clear();

    UninitializeResourceVariables();

    g_initializedForThread = false;
}

void UninitializeSettingsAndTap() {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    g_initialized = false;
}

void InitializeForCurrentThread() {
    if (g_initializedForThread) {
        return;
    }

    ProcessAllStylesFromSettings();

    g_initializedForThread = true;
}

void InitializeSettingsAndTap() {
    if (g_initialized.exchange(true)) {
        return;
    }

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"Error %08X", hr);
    }
}

enum class TargetWindowType {
    None,
    FileExplorer,
    XamlExplorerHost,
};

TargetWindowType GetTargetWindowType(HWND hWnd) {
    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return TargetWindowType::None;
    }

    if (_wcsicmp(className, L"CabinetWClass") == 0) {
        return TargetWindowType::FileExplorer;
    }

    // Used by the desktop context menu.
    if (_wcsicmp(className, L"XamlExplorerHostIslandWindow_WASDK") == 0) {
        return TargetWindowType::XamlExplorerHost;
    }

    return TargetWindowType::None;
}

BackgroundTranslucentEffect GetEffectiveBackgroundTranslucentEffect() {
    if (FindAtom(L"WindhawkFileExplorerStylerNoBackgroundEffect")) {
        return BackgroundTranslucentEffect::kDefault;
    }

    return g_settings.backgroundTranslucentEffect.value_or(
        g_themeBackgroundTranslucentEffect);
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hWnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    auto original = [=]() {
        return DwmSetWindowAttribute_Original(hWnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };

    if (dwAttribute != DWMWA_SYSTEMBACKDROP_TYPE &&
        dwAttribute != DWMWA_USE_HOSTBACKDROPBRUSH) {
        return original();
    }

    if (GetTargetWindowType(hWnd) != TargetWindowType::FileExplorer) {
        return original();
    }

    auto backgroundTranslucentEffect =
        GetEffectiveBackgroundTranslucentEffect();

    int backdropType;
    switch (backgroundTranslucentEffect) {
        case BackgroundTranslucentEffect::kDefault:
            return original();
        case BackgroundTranslucentEffect::kBlur:
            backdropType = DWMSBT_AUTO;
            break;
        case BackgroundTranslucentEffect::kAcrylic:
            backdropType = DWMSBT_TRANSIENTWINDOW;
            break;
        case BackgroundTranslucentEffect::kMica:
            backdropType = DWMSBT_MAINWINDOW;
            break;
        case BackgroundTranslucentEffect::kMicaAlt:
            backdropType = DWMSBT_TABBEDWINDOW;
            break;
        case BackgroundTranslucentEffect::kNone:
            backdropType = DWMSBT_NONE;
            break;
    }

    Wh_Log(L">");

    return DwmSetWindowAttribute_Original(hWnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                          &backdropType, sizeof(backdropType));
}

using DwmExtendFrameIntoClientArea_t = decltype(&DwmExtendFrameIntoClientArea);
DwmExtendFrameIntoClientArea_t DwmExtendFrameIntoClientArea_Original;
HRESULT WINAPI DwmExtendFrameIntoClientArea_Hook(HWND hWnd,
                                                 const MARGINS* pMarInset) {
    auto original = [=]() {
        return DwmExtendFrameIntoClientArea_Original(hWnd, pMarInset);
    };

    if (GetTargetWindowType(hWnd) != TargetWindowType::FileExplorer) {
        return original();
    }

    auto backgroundTranslucentEffect =
        GetEffectiveBackgroundTranslucentEffect();
    if (backgroundTranslucentEffect == BackgroundTranslucentEffect::kDefault ||
        g_settings.backgroundTranslucentEffectRegion !=
            BackgroundTranslucentEffectRegion::kEntireWindow) {
        return original();
    }

    Wh_Log(L">");

    MARGINS margins = {-1, -1, -1, -1};
    return DwmExtendFrameIntoClientArea_Original(hWnd, &margins);
}

// Set on threads which host a File Explorer window with the effect extended to
// the entire window. DWM treats the client area as having an alpha channel
// which GDI doesn't write, so text and some theme parts are rendered here with
// explicit alpha. The rendering is based on the Translucent Windows mod.
thread_local bool g_entireWindowEffectForThread;

bool IsEntireWindowEffectDC(HDC hdc) {
    if (!g_entireWindowEffectForThread) {
        return false;
    }

    // Memory DCs, such as those of buffered painting and comctl32 double
    // buffering, have no window, and are attributed to the thread.
    HWND hWnd = WindowFromDC(hdc);
    return !hWnd || GetTargetWindowType(GetAncestor(hWnd, GA_ROOT)) ==
                        TargetWindowType::FileExplorer;
}

// The content background is painted with the window color, which is white in
// light mode. Fills of that color are replaced with transparent black.
bool IsWindowBackgroundBrush(HDC hdc, HBRUSH hbr) {
    if (hbr == (HBRUSH)(COLOR_WINDOW + 1)) {
        return true;
    }

    COLORREF color;
    if (hbr == GetStockObject(DC_BRUSH)) {
        color = GetDCBrushColor(hdc);
    } else {
        LOGBRUSH logBrush;
        if (GetObject(hbr, sizeof(logBrush), &logBrush) != sizeof(logBrush) ||
            logBrush.lbStyle != BS_SOLID) {
            return false;
        }

        color = logBrush.lbColor;
    }

    return color == GetSysColor(COLOR_WINDOW);
}

using FillRect_t = decltype(&FillRect);
FillRect_t FillRect_Original;
int WINAPI FillRect_Hook(HDC hDC, const RECT* lprc, HBRUSH hbr) {
    if (IsEntireWindowEffectDC(hDC) && IsWindowBackgroundBrush(hDC, hbr)) {
        hbr = (HBRUSH)GetStockObject(BLACK_BRUSH);
    }

    return FillRect_Original(hDC, lprc, hbr);
}

using PatBlt_t = decltype(&PatBlt);
PatBlt_t PatBlt_Original;
BOOL WINAPI PatBlt_Hook(HDC hdc, int x, int y, int w, int h, DWORD rop) {
    if (rop == PATCOPY && IsEntireWindowEffectDC(hdc) &&
        IsWindowBackgroundBrush(hdc,
                                (HBRUSH)GetCurrentObject(hdc, OBJ_BRUSH))) {
        HGDIOBJ prevBrush = SelectObject(hdc, GetStockObject(BLACK_BRUSH));
        BOOL result = PatBlt_Original(hdc, x, y, w, h, rop);
        SelectObject(hdc, prevBrush);
        return result;
    }

    return PatBlt_Original(hdc, x, y, w, h, rop);
}

bool FillRectWithAlpha(HDC hdc, const RECT* rect, COLORREF color, BYTE alpha) {
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    BP_PAINTPARAMS params = {sizeof(params)};
    params.pBlendFunction = &blend;
    HDC memDC;
    HPAINTBUFFER hpb =
        BeginBufferedPaint(hdc, rect, BPBF_TOPDOWNDIB, &params, &memDC);
    if (!hpb) {
        return false;
    }

    // The buffer is blended as premultiplied alpha.
    HBRUSH brush = CreateSolidBrush(RGB(GetRValue(color) * alpha / 255,
                                        GetGValue(color) * alpha / 255,
                                        GetBValue(color) * alpha / 255));
    FillRect_Original(memDC, rect, brush);
    DeleteObject(brush);
    BufferedPaintSetAlpha(hpb, rect, alpha);
    EndBufferedPaint(hpb, TRUE);
    return true;
}

HMODULE GetModuleFromAddress(void* address) {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)address, &module)) {
        return nullptr;
    }

    return module;
}

std::wstring GetModulePath(HMODULE module) {
    if (!module) {
        return L"<unknown>";
    }

    std::wstring path(MAX_PATH, L'\0');
    while (true) {
        DWORD len = GetModuleFileName(module, path.data(), path.size());
        if (len == 0) {
            return L"<unknown>";
        }

        // A result equal to the buffer size means the path was truncated.
        if (len == path.size()) {
            path.resize(len * 2);
            continue;
        }

        path.resize(len);
        return path;
    }
}

// Whether the path is under the Windows directory.
bool IsSystemModulePath(PCWSTR path) {
    WCHAR windowsDir[MAX_PATH];
    UINT len = GetSystemWindowsDirectory(windowsDir, ARRAYSIZE(windowsDir));
    if (len == 0 || len >= ARRAYSIZE(windowsDir)) {
        return false;
    }

    return _wcsnicmp(path, windowsDir, len) == 0 && path[len] == L'\\';
}

// Another hook on top of ours makes its hook function the direct caller, so a
// few frames further up the stack are checked as well. A hook isn't in a system
// module, so the search stops at the first frame in one.
[[clang::noinline]] bool IsHookCallerFromModule(void* retAddress,
                                                PCWSTR moduleName) {
    HMODULE expectedModule = GetModuleHandle(moduleName);
    if (!expectedModule) {
        return false;
    }

    HMODULE callerModule = GetModuleFromAddress(retAddress);
    if (callerModule == expectedModule) {
        return true;
    }

    std::wstring callerPath = GetModulePath(callerModule);
    if (IsSystemModulePath(callerPath.c_str())) {
        Wh_Log(L"Skipping caller %p in module %s, expected %s", retAddress,
               callerPath.c_str(), moduleName);
        return false;
    }

    Wh_Log(L"Tracing caller %p in module %s, expected %s", retAddress,
           callerPath.c_str(), moduleName);

    // The backtrace skips the frames of this function, the hook, and the
    // caller.
    void* frames[4];
    WORD count = CaptureStackBackTrace(3, ARRAYSIZE(frames), frames, nullptr);
    for (WORD i = 0; i < count; i++) {
        HMODULE module = GetModuleFromAddress(frames[i]);
        std::wstring modulePath = GetModulePath(module);
        Wh_Log(L"Frame %u: %p in module %s", i + 1, frames[i],
               modulePath.c_str());
        if (module == expectedModule) {
            return true;
        }

        if (IsSystemModulePath(modulePath.c_str())) {
            return false;
        }
    }

    return false;
}

// The navigation pane divider is a horizontal line drawn by ExplorerFrame.dll,
// which is drawn translucent.
using Polyline_t = decltype(&Polyline);
Polyline_t Polyline_Original;
BOOL WINAPI Polyline_Hook(HDC hdc, const POINT* apt, int cpt) {
    if (cpt != 2 || apt[0].y != apt[1].y || !IsEntireWindowEffectDC(hdc)) {
        return Polyline_Original(hdc, apt, cpt);
    }

    LOGPEN pen;
    if (GetObject(GetCurrentObject(hdc, OBJ_PEN), sizeof(pen), &pen) !=
            sizeof(pen) ||
        pen.lopnStyle != PS_SOLID) {
        return Polyline_Original(hdc, apt, cpt);
    }

    if (!IsHookCallerFromModule(__builtin_return_address(0),
                                L"ExplorerFrame.dll")) {
        return Polyline_Original(hdc, apt, cpt);
    }

    // A wide pen is centered on the line, and the last point isn't drawn.
    int width = std::max(pen.lopnWidth.x, 1L);
    int top = apt[0].y - width / 2;
    RECT rect = {std::min(apt[0].x, apt[1].x), top,
                 std::max(apt[0].x, apt[1].x), top + width};
    if (!FillRectWithAlpha(hdc, &rect, pen.lopnColor, 96)) {
        return Polyline_Original(hdc, apt, cpt);
    }

    return TRUE;
}

BYTE g_lightTextAlphaLut[256];
BYTE g_darkTextAlphaLut[256];

void InitTextAlphaLuts() {
    for (int i = 0; i < 256; i++) {
        // Inverse gamma from 1.2 for low to 1.5 for full coverage, brightens
        // antialiased edges similarly to DrawTextWithGlow.
        float a = i / 255.0f;
        float gamma = 1.2f + 0.3f * a;
        g_lightTextAlphaLut[i] = (BYTE)(powf(a, 1.0f / gamma) * 255.0f + 0.5f);

        // Dark text is heavy with boosted edges. The max opacity matches the
        // WinUI TextFillColorPrimary light theme color (#E4000000).
        g_darkTextAlphaLut[i] = (BYTE)(i * 0xE4 / 255);
    }
}

bool CalcExtTextOutRect(HDC hdc,
                        int x,
                        int y,
                        UINT options,
                        const RECT* lprect,
                        LPCWSTR lpString,
                        UINT c,
                        const INT* lpDx,
                        RECT* textRect) {
    SIZE textSize;
    BOOL res = (options & ETO_GLYPH_INDEX)
                   ? GetTextExtentPointI(hdc, (WORD*)lpString, c, &textSize)
                   : GetTextExtentPoint32(hdc, lpString, c, &textSize);
    if (!res) {
        return false;
    }

    if (lpDx) {
        int dx = 0;
        int dy = 0;
        UINT stride = (options & ETO_PDY) ? 2 : 1;
        for (UINT i = 0; i < c; i++) {
            dx += lpDx[i * stride];
            if (options & ETO_PDY) {
                dy += lpDx[i * stride + 1];
            }
        }

        textSize.cx = std::max(textSize.cx, (LONG)dx);
        textSize.cy += abs(dy);
    }

    // TA_BASELINE and TA_CENTER are supersets of TA_BOTTOM and TA_RIGHT, so the
    // fields are compared as a whole.
    UINT align = GetTextAlign(hdc);
    switch (align & (TA_BOTTOM | TA_BASELINE)) {
        case TA_BASELINE: {
            TEXTMETRIC tm;
            if (GetTextMetrics(hdc, &tm)) {
                y -= tm.tmAscent;
            }
            break;
        }
        case TA_BOTTOM:
            y -= textSize.cy;
            break;
    }

    switch (align & (TA_RIGHT | TA_CENTER)) {
        case TA_CENTER:
            x -= textSize.cx / 2;
            break;
        case TA_RIGHT:
            x -= textSize.cx;
            break;
    }

    SetRect(textRect, x, y, x + textSize.cx, y + textSize.cy);

    if (lprect) {
        if (options & ETO_CLIPPED) {
            IntersectRect(textRect, textRect, lprect);
        }
        if (options & ETO_OPAQUE) {
            UnionRect(textRect, textRect, lprect);
        }
    }

    return !IsRectEmpty(textRect);
}

bool PaintExtTextOutBackground(HDC hdc, const RECT* lprect) {
    COLORREF color = GetBkColor(hdc);

    // The selection highlight is made opaque, other backgrounds are left for
    // DWM to treat as with any other GDI fill.
    if (color != GetSysColor(COLOR_HIGHLIGHT)) {
        HBRUSH brush = CreateSolidBrush(color);
        FillRect(hdc, lprect, brush);
        DeleteObject(brush);
        return true;
    }

    BP_PAINTPARAMS params = {sizeof(params)};
    HDC memDC;
    HPAINTBUFFER hpb =
        BeginBufferedPaint(hdc, lprect, BPBF_TOPDOWNDIB, &params, &memDC);
    if (!hpb) {
        return false;
    }

    FillRect(memDC, lprect, GetSysColorBrush(COLOR_HIGHLIGHT));
    BufferedPaintMakeOpaque(hpb, lprect);
    EndBufferedPaint(hpb, TRUE);
    return true;
}

using ExtTextOutW_t = decltype(&ExtTextOutW);
ExtTextOutW_t ExtTextOutW_Original;

BOOL ExtTextOutWithAlpha(HDC hdc,
                         int x,
                         int y,
                         UINT options,
                         const RECT* lprect,
                         LPCWSTR lpString,
                         UINT c,
                         const INT* lpDx) {
    auto original = [=]() {
        return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c,
                                    lpDx);
    };

    if (!lpString || !c || !options || (GetTextAlign(hdc) & TA_UPDATECP)) {
        return original();
    }

    if ((options & (ETO_OPAQUE | ETO_CLIPPED)) &&
        (!lprect || IsRectEmpty(lprect))) {
        return original();
    }

    RECT textRect;
    if (!CalcExtTextOutRect(hdc, x, y, options, lprect, lpString, c, lpDx,
                            &textRect)) {
        return original();
    }

    if ((options & ETO_OPAQUE) && !PaintExtTextOutBackground(hdc, lprect)) {
        return original();
    }

    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    BP_PAINTPARAMS params = {sizeof(params)};
    params.dwFlags = BPPF_ERASE | BPPF_NOCLIP;
    params.pBlendFunction = &blend;
    HDC memDC;
    HPAINTBUFFER hpb =
        BeginBufferedPaint(hdc, &textRect, BPBF_TOPDOWNDIB, &params, &memDC);
    if (!hpb) {
        return original();
    }

    // Draw a white coverage mask, then turn it into the text color with alpha.
    HGDIOBJ prevFont = SelectObject(memDC, GetCurrentObject(hdc, OBJ_FONT));
    SetTextAlign(memDC, GetTextAlign(hdc));
    SetLayout(memDC, GetLayout(hdc));
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));

    BOOL result = ExtTextOutW_Original(memDC, x, y, options & ~ETO_OPAQUE,
                                       lprect, lpString, c, lpDx);

    SelectObject(memDC, prevFont);

    RGBQUAD* pixels;
    int rowWidth;
    if (FAILED(GetBufferedPaintBits(hpb, &pixels, &rowWidth))) {
        EndBufferedPaint(hpb, FALSE);
        return original();
    }

    COLORREF textColor = GetTextColor(hdc);
    BYTE textRed = GetRValue(textColor);
    BYTE textGreen = GetGValue(textColor);
    BYTE textBlue = GetBValue(textColor);

    bool darkText = textRed * 299 + textGreen * 587 + textBlue * 114 < 128000;
    const BYTE* alphaLut = darkText ? g_darkTextAlphaLut : g_lightTextAlphaLut;

    int width = textRect.right - textRect.left;
    int height = textRect.bottom - textRect.top;
    for (int row = 0; row < height; row++) {
        RGBQUAD* rowPixels = pixels + row * rowWidth;
        for (int col = 0; col < width; col++) {
            RGBQUAD& px = rowPixels[col];
            if (!(px.rgbBlue | px.rgbGreen | px.rgbRed)) {
                continue;
            }

            BYTE luma = (px.rgbBlue + (px.rgbGreen << 1) + px.rgbRed) >> 2;
            BYTE alpha = alphaLut[luma];
            px.rgbBlue = (textBlue * alpha) >> 8;
            px.rgbGreen = (textGreen * alpha) >> 8;
            px.rgbRed = (textRed * alpha) >> 8;
            px.rgbReserved = alpha;
        }
    }

    EndBufferedPaint(hpb, TRUE);
    return result;
}

BOOL WINAPI ExtTextOutW_Hook(HDC hdc,
                             int x,
                             int y,
                             UINT options,
                             const RECT* lprect,
                             LPCWSTR lpString,
                             UINT c,
                             const INT* lpDx) {
    if (!IsEntireWindowEffectDC(hdc)) {
        return ExtTextOutW_Original(hdc, x, y, options, lprect, lpString, c,
                                    lpDx);
    }

    bool windowBk =
        (options & ETO_OPAQUE) && GetBkColor(hdc) == GetSysColor(COLOR_WINDOW);
    COLORREF prevBkColor = windowBk ? SetBkColor(hdc, RGB(0, 0, 0)) : 0;
    BOOL result =
        ExtTextOutWithAlpha(hdc, x, y, options, lprect, lpString, c, lpDx);
    if (windowBk) {
        SetBkColor(hdc, prevBkColor);
    }

    return result;
}

using DrawTextWithGlow_t =
    HRESULT(WINAPI*)(HDC hdcMem,
                     LPWSTR pszText,
                     UINT cch,
                     RECT* pRect,
                     DWORD dwFlags,
                     COLORREF crText,
                     COLORREF crGlow,
                     UINT nGlowRadius,
                     UINT nGlowIntensity,
                     BOOL fPreMultiply,
                     DTT_CALLBACK_PROC pfnDrawTextCallback,
                     LPARAM lParam);
DrawTextWithGlow_t DrawTextWithGlow_Original;
HRESULT WINAPI DrawTextWithGlow_Hook(HDC hdcMem,
                                     LPWSTR pszText,
                                     UINT cch,
                                     RECT* pRect,
                                     DWORD dwFlags,
                                     COLORREF crText,
                                     COLORREF crGlow,
                                     UINT nGlowRadius,
                                     UINT nGlowIntensity,
                                     BOOL fPreMultiply,
                                     DTT_CALLBACK_PROC pfnDrawTextCallback,
                                     LPARAM lParam) {
    auto original = [=]() {
        return DrawTextWithGlow_Original(
            hdcMem, pszText, cch, pRect, dwFlags, crText, crGlow, nGlowRadius,
            nGlowIntensity, fPreMultiply, pfnDrawTextCallback, lParam);
    };

    if (nGlowRadius > 0 || !IsEntireWindowEffectDC(hdcMem)) {
        return original();
    }

    if ((pRect->right == pRect->left || pRect->bottom == pRect->top) &&
        !(dwFlags & DT_CALCRECT)) {
        return original();
    }

    // Draw plain text, the alpha is handled by the ExtTextOutW hook.
    SetTextColor(hdcMem, crText);
    SetBkColor(hdcMem, RGB(0, 0, 0));

    if (pfnDrawTextCallback) {
        return pfnDrawTextCallback(hdcMem, pszText, cch, pRect, dwFlags,
                                   lParam);
    }

    return DrawTextW(hdcMem, pszText, cch, pRect, dwFlags & ~DT_MODIFYSTRING)
               ? S_OK
               : E_FAIL;
}

// Theme part bitmaps are shared by the File Explorer threads.
SRWLOCK g_themePartCacheLock = SRWLOCK_INIT;
winrt::com_ptr<ID2D1Factory> g_d2dFactory;
HDC g_scrollBarThumbCache[4];
HDC g_headerItemCache[2];

void DeleteThemePartBitmap(HDC& hdc) {
    if (hdc) {
        HGDIOBJ bitmap = GetCurrentObject(hdc, OBJ_BITMAP);
        DeleteDC(hdc);
        DeleteObject(bitmap);
        hdc = nullptr;
    }
}

void ClearThemePartCache() {
    AcquireSRWLockExclusive(&g_themePartCacheLock);

    for (HDC& hdc : g_scrollBarThumbCache) {
        DeleteThemePartBitmap(hdc);
    }

    for (HDC& hdc : g_headerItemCache) {
        DeleteThemePartBitmap(hdc);
    }

    g_d2dFactory = nullptr;

    ReleaseSRWLockExclusive(&g_themePartCacheLock);
}

float GetThemePartScale() {
    return GetDpiForSystem() / 96.0f;
}

D2D1_COLOR_F ThemePartColor(BYTE a, BYTE r, BYTE g, BYTE b) {
    return D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

winrt::com_ptr<ID2D1DCRenderTarget> CreateBoundRenderTarget(HDC hdc,
                                                            const RECT* rect) {
    if (!g_d2dFactory &&
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
                                 __uuidof(ID2D1Factory), nullptr,
                                 g_d2dFactory.put_void()))) {
        return nullptr;
    }

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);

    winrt::com_ptr<ID2D1DCRenderTarget> renderTarget;
    if (FAILED(
            g_d2dFactory->CreateDCRenderTarget(&props, renderTarget.put())) ||
        FAILED(renderTarget->BindDC(hdc, rect))) {
        return nullptr;
    }

    return renderTarget;
}

// Returns a memory DC with a 32-bit bitmap selected, painted by drawFunc.
template <typename DrawFunc>
HDC CreateThemePartBitmap(int width, int height, DrawFunc drawFunc) {
    HDC hdc = CreateCompatibleDC(nullptr);
    if (!hdc) {
        return nullptr;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits;
    HBITMAP bitmap =
        CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(hdc);
        return nullptr;
    }

    SelectObject(hdc, bitmap);

    RECT rect = {0, 0, width, height};
    auto renderTarget = CreateBoundRenderTarget(hdc, &rect);
    if (!renderTarget) {
        DeleteThemePartBitmap(hdc);
        return nullptr;
    }

    renderTarget->BeginDraw();
    drawFunc(renderTarget.get());
    if (FAILED(renderTarget->EndDraw())) {
        DeleteThemePartBitmap(hdc);
        return nullptr;
    }

    return hdc;
}

// Stretches the bitmap over the rect, keeping the margins unscaled.
void AlphaBlendNineGrid(HDC hdc,
                        const RECT* rect,
                        HDC src,
                        int left,
                        int top,
                        int right,
                        int bottom) {
    BITMAP bmp;
    if (!GetObject(GetCurrentObject(src, OBJ_BITMAP), sizeof(bmp), &bmp)) {
        return;
    }

    int width = rect->right - rect->left;
    int height = rect->bottom - rect->top;
    left = std::min(left, width);
    right = std::min(right, width - left);
    top = std::min(top, height);
    bottom = std::min(bottom, height - top);

    const int srcX[] = {0, left, bmp.bmWidth - right, (int)bmp.bmWidth};
    const int srcY[] = {0, top, bmp.bmHeight - bottom, (int)bmp.bmHeight};
    const int dstX[] = {rect->left, rect->left + left, rect->right - right,
                        rect->right};
    const int dstY[] = {rect->top, rect->top + top, rect->bottom - bottom,
                        rect->bottom};

    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int dstWidth = dstX[i + 1] - dstX[i];
            int dstHeight = dstY[j + 1] - dstY[j];
            int srcWidth = srcX[i + 1] - srcX[i];
            int srcHeight = srcY[j + 1] - srcY[j];
            if (dstWidth > 0 && dstHeight > 0 && srcWidth > 0 &&
                srcHeight > 0) {
                AlphaBlend(hdc, dstX[i], dstY[j], dstWidth, dstHeight, src,
                           srcX[i], srcY[j], srcWidth, srcHeight, blend);
            }
        }
    }
}

bool PaintScrollBarThumb(HDC hdc, int iPartId, int iStateId, LPCRECT pRect) {
    bool horizontal = iPartId == SBP_THUMBBTNHORZ;
    bool normal = iStateId == SCRBS_NORMAL;
    float scale = GetThemePartScale();

    HDC& cached =
        g_scrollBarThumbCache[(horizontal ? 2 : 0) + (normal ? 0 : 1)];
    if (!cached) {
        int width = (horizontal ? 20 : 17) * scale;
        int height = (horizontal ? 17 : 11) * scale;
        cached = CreateThemePartBitmap(
            width, height, [&](ID2D1RenderTarget* renderTarget) {
                // The thumb gets wider when hovered.
                float inset = normal ? 0.35f : 0.25f;
                D2D1_RECT_F rect =
                    horizontal ? D2D1::RectF(0, height * inset, width,
                                             height - height * inset)
                               : D2D1::RectF(width * inset, 0,
                                             width - width * inset, height);
                float radius = 4.0f * scale;

                winrt::com_ptr<ID2D1SolidColorBrush> brush;
                renderTarget->CreateSolidColorBrush(
                    normal ? ThemePartColor(128, 160, 160, 160)
                           : ThemePartColor(160, 224, 224, 224),
                    brush.put());
                if (brush) {
                    renderTarget->FillRoundedRectangle(
                        D2D1::RoundedRect(rect, radius, radius), brush.get());
                }
            });
        if (!cached) {
            return false;
        }
    }

    FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    AlphaBlendNineGrid(hdc, pRect, cached, 8 * scale, 5 * scale, 8 * scale,
                       5 * scale);
    return true;
}

bool PaintScrollBarArrow(HDC hdc, int iStateId, LPCRECT pRect) {
    // States 1-16 are normal, hot, pressed and disabled for each of up, down,
    // left and right. States 17-20 are hover for each direction.
    int direction;
    int state;
    if (iStateId >= ABS_UPNORMAL && iStateId <= ABS_RIGHTDISABLED) {
        direction = (iStateId - ABS_UPNORMAL) / 4;
        state = (iStateId - ABS_UPNORMAL) % 4 + 1;
    } else if (iStateId >= ABS_UPHOVER && iStateId <= ABS_RIGHTHOVER) {
        direction = iStateId - ABS_UPHOVER;
        state = 0;
    } else {
        return false;
    }

    FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // Arrows are hidden until the scroll bar is hovered.
    if (state == 1) {
        return true;
    }

    float scale = GetThemePartScale();
    float baseWidth = 7.0f * scale;
    float arrowHeight = 4.5f * scale;
    D2D1_COLOR_F color = ThemePartColor(128, 160, 160, 160);
    if (state == 2) {
        baseWidth = 8.0f * scale;
        arrowHeight = 5.5f * scale;
        color = ThemePartColor(192, 224, 224, 224);
    } else if (state == 4) {
        color = ThemePartColor(192, 64, 64, 64);
    }

    // An up arrow with a thick base, rotated clockwise for the other
    // directions.
    D2D1_POINT_2F center = D2D1::Point2F((pRect->right - pRect->left) / 2.0f,
                                         (pRect->bottom - pRect->top) / 2.0f);
    const D2D1_POINT_2F points[] = {
        {center.x - 1 - baseWidth / 2, center.y + arrowHeight / 2},
        {center.x - 1 - baseWidth / 2, center.y + 2 + arrowHeight / 2},
        {center.x + baseWidth / 2, center.y + 2 + arrowHeight / 2},
        {center.x + baseWidth / 2, center.y + arrowHeight / 2},
        {center.x, center.y - arrowHeight / 2},
        {center.x - 1, center.y - arrowHeight / 2},
    };

    // Up, down, left, right.
    constexpr float kRotationAngles[] = {0, 180, 270, 90};

    auto renderTarget = CreateBoundRenderTarget(hdc, pRect);
    if (!renderTarget) {
        return false;
    }

    winrt::com_ptr<ID2D1PathGeometry> geometry;
    winrt::com_ptr<ID2D1GeometrySink> sink;
    winrt::com_ptr<ID2D1SolidColorBrush> brush;
    if (FAILED(g_d2dFactory->CreatePathGeometry(geometry.put())) ||
        FAILED(geometry->Open(sink.put())) ||
        FAILED(renderTarget->CreateSolidColorBrush(color, brush.put()))) {
        return false;
    }

    sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLines(points + 1, ARRAYSIZE(points) - 1);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();

    renderTarget->BeginDraw();
    renderTarget->SetTransform(
        D2D1::Matrix3x2F::Rotation(kRotationAngles[direction], center));
    renderTarget->FillGeometry(geometry.get(), brush.get());
    return SUCCEEDED(renderTarget->EndDraw());
}

bool PaintScrollBarPart(HDC hdc, int iPartId, int iStateId, LPCRECT pRect) {
    switch (iPartId) {
        case SBP_ARROWBTN:
            return PaintScrollBarArrow(hdc, iStateId, pRect);

        case SBP_THUMBBTNHORZ:
        case SBP_THUMBBTNVERT:
            return PaintScrollBarThumb(hdc, iPartId, iStateId, pRect);

        case SBP_LOWERTRACKHORZ:
        case SBP_UPPERTRACKHORZ:
        case SBP_LOWERTRACKVERT:
        case SBP_UPPERTRACKVERT:
            FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            return true;
    }

    return false;
}

bool PaintHeaderPart(HDC hdc, int iPartId, int iStateId, LPCRECT pRect) {
    if (iPartId != 0 && iPartId != HP_HEADERITEM) {
        return false;
    }

    FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // Item states cycle through normal, hot and pressed.
    if (iPartId == 0 || iStateId % 3 == 1) {
        return true;
    }

    bool hot = iStateId % 3 == 2;
    float scale = GetThemePartScale();

    HDC& cached = g_headerItemCache[hot ? 0 : 1];
    if (!cached) {
        int size = 24 * scale;
        cached = CreateThemePartBitmap(
            size, size, [&](ID2D1RenderTarget* renderTarget) {
                winrt::com_ptr<ID2D1PathGeometry> geometry;
                winrt::com_ptr<ID2D1GeometrySink> sink;
                winrt::com_ptr<ID2D1SolidColorBrush> brush;
                if (FAILED(g_d2dFactory->CreatePathGeometry(geometry.put())) ||
                    FAILED(geometry->Open(sink.put())) ||
                    FAILED(renderTarget->CreateSolidColorBrush(
                        hot ? ThemePartColor(96, 144, 144, 144)
                            : ThemePartColor(64, 144, 144, 144),
                        brush.put()))) {
                    return;
                }

                // Rounded bottom corners.
                float radius = 6.0f * scale;
                D2D1_SIZE_F arcSize = D2D1::SizeF(radius, radius);
                sink->BeginFigure(D2D1::Point2F(0, 0),
                                  D2D1_FIGURE_BEGIN_FILLED);
                sink->AddLine(D2D1::Point2F(size, 0));
                sink->AddLine(D2D1::Point2F(size, size - radius));
                sink->AddArc(D2D1::ArcSegment(
                    D2D1::Point2F(size - radius, size), arcSize, 0,
                    D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
                sink->AddLine(D2D1::Point2F(radius, size));
                sink->AddArc(D2D1::ArcSegment(
                    D2D1::Point2F(0, size - radius), arcSize, 0,
                    D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                sink->Close();

                renderTarget->FillGeometry(geometry.get(), brush.get());
            });
        if (!cached) {
            return false;
        }
    }

    AlphaBlendNineGrid(hdc, pRect, cached, 12 * scale, 0, 11 * scale,
                       12 * scale);
    return true;
}

// The separator between the navigation pane and the content.
bool PaintPaneSeparatorPart(HDC hdc, int iPartId, LPCRECT pRect) {
    if (iPartId != 3 && iPartId != 4) {
        return false;
    }

    FillRect(hdc, pRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    return true;
}

using GetThemeClass_t = HRESULT(WINAPI*)(HTHEME hTheme,
                                         LPWSTR pszClassName,
                                         int cchClassName);
GetThemeClass_t g_pGetThemeClass;

bool PaintThemeBackground(HTHEME hTheme,
                          HDC hdc,
                          int iPartId,
                          int iStateId,
                          LPCRECT pRect,
                          LPCRECT pClipRect) {
    if (!g_pGetThemeClass || !IsEntireWindowEffectDC(hdc)) {
        return false;
    }

    WCHAR themeClass[64];
    if (FAILED(g_pGetThemeClass(hTheme, themeClass, ARRAYSIZE(themeClass)))) {
        return false;
    }

    enum class Part { ScrollBar, Header, PaneSeparator };
    Part part;
    if (_wcsicmp(themeClass, L"ScrollBar") == 0) {
        part = Part::ScrollBar;
    } else if (_wcsicmp(themeClass, L"Header") == 0) {
        part = Part::Header;
    } else if (_wcsicmp(themeClass, L"PreviewPane") == 0) {
        part = Part::PaneSeparator;
    } else {
        return false;
    }

    int savedDC = 0;
    if (pClipRect) {
        savedDC = SaveDC(hdc);
        IntersectClipRect(hdc, pClipRect->left, pClipRect->top,
                          pClipRect->right, pClipRect->bottom);
    }

    AcquireSRWLockExclusive(&g_themePartCacheLock);

    bool painted = false;
    switch (part) {
        case Part::ScrollBar:
            painted = PaintScrollBarPart(hdc, iPartId, iStateId, pRect);
            break;
        case Part::Header:
            painted = PaintHeaderPart(hdc, iPartId, iStateId, pRect);
            break;
        case Part::PaneSeparator:
            painted = PaintPaneSeparatorPart(hdc, iPartId, pRect);
            break;
    }

    ReleaseSRWLockExclusive(&g_themePartCacheLock);

    if (savedDC) {
        RestoreDC(hdc, savedDC);
    }

    return painted;
}

using DrawThemeBackground_t = decltype(&DrawThemeBackground);
DrawThemeBackground_t DrawThemeBackground_Original;
HRESULT WINAPI DrawThemeBackground_Hook(HTHEME hTheme,
                                        HDC hdc,
                                        int iPartId,
                                        int iStateId,
                                        LPCRECT pRect,
                                        LPCRECT pClipRect) {
    if (PaintThemeBackground(hTheme, hdc, iPartId, iStateId, pRect,
                             pClipRect)) {
        return S_OK;
    }

    return DrawThemeBackground_Original(hTheme, hdc, iPartId, iStateId, pRect,
                                        pClipRect);
}

using DrawThemeBackgroundEx_t = decltype(&DrawThemeBackgroundEx);
DrawThemeBackgroundEx_t DrawThemeBackgroundEx_Original;
HRESULT WINAPI DrawThemeBackgroundEx_Hook(HTHEME hTheme,
                                          HDC hdc,
                                          int iPartId,
                                          int iStateId,
                                          LPCRECT pRect,
                                          const DTBGOPTS* pOptions) {
    LPCRECT pClipRect = pOptions && (pOptions->dwFlags & DTBG_CLIPRECT)
                            ? &pOptions->rcClip
                            : nullptr;
    if (PaintThemeBackground(hTheme, hdc, iPartId, iStateId, pRect,
                             pClipRect)) {
        return S_OK;
    }

    return DrawThemeBackgroundEx_Original(hTheme, hdc, iPartId, iStateId, pRect,
                                          pOptions);
}

// Based on the Translucent Windows mod.
void SetAccentBlurBehind(HWND hWnd, bool enable) {
    // Without blur behind, the extended frame is drawn over the accent blur.
    HRGN hRgn = enable ? CreateRectRgn(0, 0, -1, -1) : nullptr;
    DWM_BLURBEHIND blurBehind = {
        .dwFlags = DWM_BB_ENABLE | (enable ? DWM_BB_BLURREGION : 0u),
        .fEnable = enable,
        .hRgnBlur = hRgn,
    };
    DwmEnableBlurBehindWindow(hWnd, &blurBehind);
    if (hRgn) {
        DeleteObject(hRgn);
    }

    if (!enable) {
        // Restore the accent policy set by WinUI when the window is created
        // (ACCENT_ENABLE_HOSTBACKDROP).
        BOOL useHostBackdropBrush = TRUE;
        DwmSetWindowAttribute_Original(hWnd, DWMWA_USE_HOSTBACKDROPBRUSH,
                                       &useHostBackdropBrush,
                                       sizeof(useHostBackdropBrush));
        return;
    }

    constexpr int ACCENT_ENABLE_ACRYLICBLURBEHIND = 4;

    struct ACCENT_POLICY {
        int AccentState;
        int AccentFlags;
        int GradientColor;
        int AnimationId;
    };

    constexpr DWORD WCA_ACCENT_POLICY = 19;

    struct WINDOWCOMPOSITIONATTRIBDATA {
        DWORD Attrib;
        PVOID pvData;
        SIZE_T cbData;
    };

    using SetWindowCompositionAttribute_t =
        BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
    static auto pSetWindowCompositionAttribute =
        (SetWindowCompositionAttribute_t)GetProcAddress(
            GetModuleHandle(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!pSetWindowCompositionAttribute) {
        return;
    }

    ACCENT_POLICY accentPolicy = {
        .AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND,
        // AABBGGRR.
        .GradientColor = 0x3A232323,
    };

    WINDOWCOMPOSITIONATTRIBDATA data = {
        .Attrib = WCA_ACCENT_POLICY,
        .pvData = &accentPolicy,
        .cbData = sizeof(accentPolicy),
    };

    pSetWindowCompositionAttribute(hWnd, &data);
}

void ApplyBackgroundTranslucentEffect(
    HWND hWnd,
    std::optional<BackgroundTranslucentEffect> effectToApply = std::nullopt) {
    constexpr WCHAR kBackgroundTranslucentEffectAppliedKey[] =
        L"windhawk_background_effect-" WH_MOD_ID;

    auto effect =
        effectToApply.value_or(GetEffectiveBackgroundTranslucentEffect());

    g_entireWindowEffectForThread =
        effect != BackgroundTranslucentEffect::kDefault &&
        g_settings.backgroundTranslucentEffectRegion ==
            BackgroundTranslucentEffectRegion::kEntireWindow;

    if (effect == BackgroundTranslucentEffect::kDefault) {
        if (!RemoveProp(hWnd, kBackgroundTranslucentEffectAppliedKey)) {
            return;
        }
    } else {
        SetProp(hWnd, kBackgroundTranslucentEffectAppliedKey, (HANDLE)1);
    }

    Wh_Log(L"Applying background translucent effect %d for %08X",
           static_cast<int>(effect), (DWORD)(ULONG_PTR)hWnd);

    if (effect != BackgroundTranslucentEffect::kDefault &&
        g_settings.backgroundTranslucentEffectRegion ==
            BackgroundTranslucentEffectRegion::kEntireWindow) {
        MARGINS margins = {-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea_Original(hWnd, &margins);
    }

    int backdropType;
    switch (effect) {
        case BackgroundTranslucentEffect::kDefault:
            backdropType = DWMSBT_TABBEDWINDOW;
            break;
        case BackgroundTranslucentEffect::kBlur:
            backdropType = DWMSBT_AUTO;
            break;
        case BackgroundTranslucentEffect::kAcrylic:
            backdropType = DWMSBT_TRANSIENTWINDOW;
            break;
        case BackgroundTranslucentEffect::kMica:
            backdropType = DWMSBT_MAINWINDOW;
            break;
        case BackgroundTranslucentEffect::kMicaAlt:
            backdropType = DWMSBT_TABBEDWINDOW;
            break;
        case BackgroundTranslucentEffect::kNone:
            backdropType = DWMSBT_NONE;
            break;
    }

    DwmSetWindowAttribute_Original(hWnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                   &backdropType, sizeof(backdropType));

    SetAccentBlurBehind(hWnd, effect == BackgroundTranslucentEffect::kBlur);
}

void TriggerWindowCompositionUpdate(HWND hWnd) {
    WINDOWPOS windowPos = {
        .hwnd = hWnd,
        .flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE,
    };
    SendMessage(hWnd, WM_WINDOWPOSCHANGED, 0, (LPARAM)&windowPos);
    SendMessage(hWnd, WM_DWMCOMPOSITIONCHANGED, 0, 0);

    // Repaint the content, which is rendered differently with the effect
    // extended to the entire window.
    RedrawWindow(hWnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
}

void OnWindowCreated(HWND hWnd, PCSTR funcName) {
    TargetWindowType windowType = GetTargetWindowType(hWnd);
    if (windowType != TargetWindowType::None) {
        Wh_Log(L"Initializing - Created window %08X via %S",
               (DWORD)(ULONG_PTR)hWnd, funcName);

        if (windowType == TargetWindowType::FileExplorer) {
            ApplyBackgroundTranslucentEffect(hWnd);
        }

        InitializeForCurrentThread();
        InitializeSettingsAndTap();
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, __FUNCTION__);

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           PVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    PVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, __FUNCTION__);

    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle,
                                             LPCWSTR lpClassName,
                                             LPCWSTR lpWindowName,
                                             DWORD dwStyle,
                                             int X,
                                             int Y,
                                             int nWidth,
                                             int nHeight,
                                             HWND hWndParent,
                                             HMENU hMenu,
                                             HINSTANCE hInstance,
                                             PVOID lpParam,
                                             DWORD dwBand,
                                             DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
                                      LPCWSTR lpClassName,
                                      LPCWSTR lpWindowName,
                                      DWORD dwStyle,
                                      int X,
                                      int Y,
                                      int nWidth,
                                      int nHeight,
                                      HWND hWndParent,
                                      HMENU hMenu,
                                      HINSTANCE hInstance,
                                      PVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, __FUNCTION__);

    return hWnd;
}

PFN_INITIALIZE_XAML_DIAGNOSTICS_EX InitializeXamlDiagnosticsEx_Original;
HRESULT WINAPI
InitializeXamlDiagnosticsEx_Hook(_In_ PCWSTR endPointName,
                                 _In_ DWORD pid,
                                 _In_ PCWSTR wszDllXamlDiagnostics,
                                 _In_ PCWSTR wszTAPDllName,
                                 _In_ CLSID tapClsid,
                                 _In_opt_ PCWSTR wszInitializationData) {
    if (g_inInjectWindhawkTAP) {
        return InitializeXamlDiagnosticsEx_Original(
            endPointName, pid, wszDllXamlDiagnostics, wszTAPDllName, tapClsid,
            wszInitializationData);
    }

    bool blockCall = false;

    switch (g_settings.xamlDiagnosticsHandling) {
        case XamlDiagnosticsHandling::kAlert: {
            void* retAddress = __builtin_return_address(0);

            WCHAR modulePath[MAX_PATH];
            PCWSTR modulePathStr = L"<unknown>";
            HMODULE module;
            if (GetModuleHandleEx(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCWSTR>(retAddress), &module)) {
                switch (GetModuleFileName(module, modulePath,
                                          ARRAYSIZE(modulePath))) {
                    case 0:
                    case ARRAYSIZE(modulePath):
                        break;

                    default:
                        modulePathStr = modulePath;
                        break;
                }
            }

            WCHAR message[1024];
            _snwprintf_s(
                message, _TRUNCATE,
                L"The following module is trying to use XAML diagnostics:\n\n"
                L"%s\n\n"
                L"There can only be one consumer at a time. Blocking it might "
                L"break that module, but allowing it might break this mod.\n\n"
                L"Do you want to block it?\n\n"
                L"Note: You can change this behavior in the mod settings.",
                modulePathStr);
            int result = MessageBox(
                nullptr, message, L"Windows 11 File Explorer Styler - Windhawk",
                MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
            blockCall = (result == IDYES);
            break;
        }

        case XamlDiagnosticsHandling::kBlock:
            blockCall = true;
            break;

        case XamlDiagnosticsHandling::kAllow:
            blockCall = false;
            break;
    }

    if (blockCall) {
        Wh_Log(L"Blocking InitializeXamlDiagnosticsEx call");
        // Return success to avoid exception in the caller.
        return S_OK;
    }

    Wh_Log(L"Allowing InitializeXamlDiagnosticsEx call");
    return InitializeXamlDiagnosticsEx_Original(
        endPointName, pid, wszDllXamlDiagnostics, wszTAPDllName, tapClsid,
        wszInitializationData);
}

bool HookInitializeXamlDiagnosticsExIfNeeded() {
    if (InitializeXamlDiagnosticsEx_Original) {
        return false;  // Already hooked
    }

    const HMODULE wux = GetModuleHandle(L"Microsoft.Internal.FrameworkUdk.dll");
    if (!wux) {
        return false;  // DLL not loaded yet
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) {
        return false;
    }

    Wh_Log(L"Hooking InitializeXamlDiagnosticsEx to handle other consumers");
    return WindhawkUtils::SetFunctionHook(
        ixde, InitializeXamlDiagnosticsEx_Hook,
        &InitializeXamlDiagnosticsEx_Original);
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module && !InitializeXamlDiagnosticsEx_Original && lpLibFileName) {
        PCWSTR fileName = wcsrchr(lpLibFileName, L'\\');
        fileName = fileName ? fileName + 1 : lpLibFileName;
        // CoreMessagingXP.dll loads Microsoft.Internal.FrameworkUdk.dll via the
        // import table.
        if (_wcsicmp(fileName, L"CoreMessagingXP.dll") == 0 &&
            HookInitializeXamlDiagnosticsExIfNeeded()) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

std::vector<HWND> GetTargetWnds() {
    struct ENUM_WINDOWS_PARAM {
        std::vector<HWND>* hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            if (GetTargetWindowType(hWnd) != TargetWindowType::None) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

using XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_t =
    HRESULT(WINAPI*)(void* pThis, SIZE* size);
XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_t
    XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Original;
HRESULT WINAPI
XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Hook(void* pThis,
                                                           SIZE* size) {
    Wh_Log(L">");

    HRESULT ret =
        XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Original(pThis,
                                                                       size);

    int explorerFrameContainerHeight = g_settings.explorerFrameContainerHeight;
    if (!explorerFrameContainerHeight) {
        explorerFrameContainerHeight = g_themeExplorerFrameContainerHeight;
    }

    if (SUCCEEDED(ret) && explorerFrameContainerHeight) {
        int originalCy = size->cy;
        size->cy = MulDiv(size->cy, explorerFrameContainerHeight, 136);
        Wh_Log(L"%d -> %d", originalCy, size->cy);
    }

    return ret;
}

bool HookWindowsUIFileExplorerSymbols() {
    HMODULE module = LoadLibraryEx(L"Windows.UI.FileExplorer.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load Windows.UI.FileExplorer.dll");
        return false;
    }

    // Windows.UI.FileExplorer.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlIslandViewAdapter::get_DesiredSizeInPhysicalPixels(struct tagSIZE *))"},
            &XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Original,
            XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Hook,
        },
    };

    if (!HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

PTP_TIMER g_statsTimer;

bool StartStatsTimer() {
    static constexpr WCHAR kStatsBaseUrl[] =
        L"https://github.com/ramensoftware/"
        L"windows-11-file-explorer-styling-guide/"
        L"releases/download/stats-v6/";

    ULONGLONG lastStatsTime = 0;
    Wh_GetBinaryValue(L"statsTimerLastTime", &lastStatsTime,
                      sizeof(lastStatsTime));

    // -1 can be set for disabling the stats timer.
    if (lastStatsTime == 0xFFFFFFFF'FFFFFFFF) {
        return false;
    }

    FILETIME currentTimeFt;
    GetSystemTimeAsFileTime(&currentTimeFt);

    ULONGLONG currentTime = ((ULONGLONG)currentTimeFt.dwHighDateTime << 32) |
                            currentTimeFt.dwLowDateTime;

    constexpr ULONGLONG k10Minutes = 10 * 60 * 10000000LL;
    constexpr ULONGLONG k24Hours = 24 * 60 * 60 * 10000000LL;

    ULONGLONG minDueTime = currentTime + k10Minutes;
    ULONGLONG maxDueTime = currentTime + k24Hours;

    ULONGLONG dueTime = lastStatsTime + k24Hours;
    if (dueTime < minDueTime) {
        dueTime = minDueTime;
    } else if (dueTime > maxDueTime) {
        dueTime = maxDueTime;
    }

    g_statsTimer = CreateThreadpoolTimer(
        [](PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER) {
            Wh_Log(L">");

            string_setting_unique_ptr themeName(Wh_GetStringSetting(L"theme"));
            if (!*themeName.get()) {
                return;
            }

            HANDLE mutex =
                CreateMutex(nullptr, FALSE, L"WindhawkStats_" WH_MOD_ID);
            if (mutex) {
                WaitForSingleObject(mutex, INFINITE);
            }

            ULONGLONG lastStatsTime = 0;
            Wh_GetBinaryValue(L"statsTimerLastTime", &lastStatsTime,
                              sizeof(lastStatsTime));

            FILETIME currentTimeFt;
            GetSystemTimeAsFileTime(&currentTimeFt);
            ULONGLONG currentTime =
                ((ULONGLONG)currentTimeFt.dwHighDateTime << 32) |
                currentTimeFt.dwLowDateTime;

            const WH_URL_CONTENT* content = nullptr;
            if (currentTime - lastStatsTime >= k10Minutes) {
                Wh_SetBinaryValue(L"statsTimerLastTime", &currentTime,
                                  sizeof(currentTime));

                std::wstring themeNameEscaped = themeName.get();
                std::replace(themeNameEscaped.begin(), themeNameEscaped.end(),
                             L' ', L'_');
                std::replace(themeNameEscaped.begin(), themeNameEscaped.end(),
                             L'&', L'_');
                std::replace(themeNameEscaped.begin(), themeNameEscaped.end(),
                             L'.', L'_');

                std::wstring statsUrl = kStatsBaseUrl;
                statsUrl += themeNameEscaped;
                statsUrl += L".txt";

                Wh_Log(L"Submitting stats to %s", statsUrl.c_str());

                content = Wh_GetUrlContent(statsUrl.c_str(), nullptr);
            } else {
                Wh_Log(L"Skipping, last submission %llu seconds ago",
                       (currentTime - lastStatsTime) / 10000000LL);
            }

            if (mutex) {
                ReleaseMutex(mutex);
                CloseHandle(mutex);
            }

            if (!content) {
                Wh_Log(L"Failed to get stats content");
                return;
            }

            if (content->statusCode != 200) {
                Wh_Log(L"Stats content status code: %d", content->statusCode);
            }

            Wh_FreeUrlContent(content);
            Wh_Log(L"Stats content submitted");
        },
        nullptr, nullptr);
    if (!g_statsTimer) {
        Wh_Log(L"Failed to create stats timer");
        return false;
    }

    constexpr DWORD k24HoursInMs = 24 * 60 * 60 * 1000;
    constexpr ULONGLONG k10MinutesInMs = 10 * 60 * 1000;

    FILETIME dueTimeFt;
    dueTimeFt.dwLowDateTime = (DWORD)(dueTime & 0xFFFFFFFF);
    dueTimeFt.dwHighDateTime = (DWORD)(dueTime >> 32);
    SetThreadpoolTimer(g_statsTimer, &dueTimeFt, k24HoursInMs, k10MinutesInMs);
    return true;
}

void StopStatsTimer() {
    if (g_statsTimer) {
        SetThreadpoolTimer(g_statsTimer, nullptr, 0, 0);
        WaitForThreadpoolTimerCallbacks(g_statsTimer, TRUE);
        CloseThreadpoolTimer(g_statsTimer);
        g_statsTimer = nullptr;
    }
}

void LoadSettings() {
    PCWSTR backgroundTranslucentEffectRegion =
        Wh_GetStringSetting(L"backgroundTranslucentEffectRegion");
    g_settings.backgroundTranslucentEffectRegion =
        BackgroundTranslucentEffectRegion::kEntireWindow;
    if (wcscmp(backgroundTranslucentEffectRegion, L"explorerFrame") == 0) {
        g_settings.backgroundTranslucentEffectRegion =
            BackgroundTranslucentEffectRegion::kExplorerFrame;
    }
    Wh_FreeStringSetting(backgroundTranslucentEffectRegion);

    PCWSTR backgroundTranslucentEffect =
        Wh_GetStringSetting(L"backgroundTranslucentEffect");
    g_settings.backgroundTranslucentEffect.reset();
    if (wcscmp(backgroundTranslucentEffect, L"default") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kDefault;
    } else if (wcscmp(backgroundTranslucentEffect, L"acrylicblur") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kBlur;
    } else if (wcscmp(backgroundTranslucentEffect, L"acrylic") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kAcrylic;
    } else if (wcscmp(backgroundTranslucentEffect, L"mica") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kMica;
    } else if (wcscmp(backgroundTranslucentEffect, L"micaAlt") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kMicaAlt;
    } else if (wcscmp(backgroundTranslucentEffect, L"none") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kNone;
    }
    Wh_FreeStringSetting(backgroundTranslucentEffect);

    g_settings.explorerFrameContainerHeight =
        Wh_GetIntSetting(L"explorerFrameContainerHeight");

    PCWSTR xamlDiagnosticsHandling =
        Wh_GetStringSetting(L"xamlDiagnosticsHandling");
    g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kAlert;
    if (wcscmp(xamlDiagnosticsHandling, L"block") == 0) {
        g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kBlock;
    } else if (wcscmp(xamlDiagnosticsHandling, L"allow") == 0) {
        g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kAllow;
    }
    Wh_FreeStringSetting(xamlDiagnosticsHandling);
}

void LoadThemeSettings() {
    const Theme* theme = GetSelectedTheme();
    g_themeBackgroundTranslucentEffect =
        theme ? theme->backgroundTranslucentEffect
              : BackgroundTranslucentEffect::kDefault;
    g_themeExplorerFrameContainerHeight =
        theme ? theme->explorerFrameContainerHeight : 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();
    LoadThemeSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute,
                                   DwmSetWindowAttribute_Hook,
                                   &DwmSetWindowAttribute_Original);

    WindhawkUtils::SetFunctionHook(DwmExtendFrameIntoClientArea,
                                   DwmExtendFrameIntoClientArea_Hook,
                                   &DwmExtendFrameIntoClientArea_Original);

    InitTextAlphaLuts();

    WindhawkUtils::SetFunctionHook(ExtTextOutW, ExtTextOutW_Hook,
                                   &ExtTextOutW_Original);

    WindhawkUtils::SetFunctionHook(FillRect, FillRect_Hook, &FillRect_Original);

    WindhawkUtils::SetFunctionHook(PatBlt, PatBlt_Hook, &PatBlt_Original);

    WindhawkUtils::SetFunctionHook(Polyline, Polyline_Hook, &Polyline_Original);

    WindhawkUtils::SetFunctionHook(DrawThemeBackground,
                                   DrawThemeBackground_Hook,
                                   &DrawThemeBackground_Original);

    WindhawkUtils::SetFunctionHook(DrawThemeBackgroundEx,
                                   DrawThemeBackgroundEx_Hook,
                                   &DrawThemeBackgroundEx_Original);

    HMODULE uxthemeModule = GetModuleHandle(L"uxtheme.dll");
    if (uxthemeModule) {
        g_pGetThemeClass = (GetThemeClass_t)GetProcAddress(
            uxthemeModule, MAKEINTRESOURCEA(74));

        auto pDrawTextWithGlow = (DrawTextWithGlow_t)GetProcAddress(
            uxthemeModule, MAKEINTRESOURCEA(126));
        if (pDrawTextWithGlow) {
            WindhawkUtils::SetFunctionHook(pDrawTextWithGlow,
                                           DrawTextWithGlow_Hook,
                                           &DrawTextWithGlow_Original);
        }
    }

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }

        auto pCreateWindowInBandEx = (CreateWindowInBandEx_t)GetProcAddress(
            user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBandEx,
                                           CreateWindowInBandEx_Hook,
                                           &CreateWindowInBandEx_Original);
        }
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    // Hook immediately if DLL is already loaded.
    HookInitializeXamlDiagnosticsExIfNeeded();

    HookWindowsUIFileExplorerSymbols();

    StartStatsTimer();

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    auto hTargetWnds = GetTargetWnds();
    for (auto hTargetWnd : hTargetWnds) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hTargetWnd);
        RunFromWindowThread(
            hTargetWnd,
            [](PVOID param) {
                HWND hTargetWnd = (HWND)param;

                InitializeForCurrentThread();

                if (GetTargetWindowType(hTargetWnd) ==
                    TargetWindowType::FileExplorer) {
                    ApplyBackgroundTranslucentEffect(hTargetWnd);
                    TriggerWindowCompositionUpdate(hTargetWnd);
                }
            },
            (PVOID)hTargetWnd);
    }

    if (hTargetWnds.size() > 0) {
        Wh_Log(L"Initializing - Found target windows");
        InitializeSettingsAndTap();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    StopStatsTimer();

    StopImageDownloads();

    // Before the UI threads are uninitialized, so that a retry can't be
    // scheduled on a thread which is being uninitialized.
    StopImageLoadRetries();

    UninitializeSettingsAndTap();

    auto hTargetWnds = GetTargetWnds();
    for (auto hTargetWnd : hTargetWnds) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hTargetWnd);
        RunFromWindowThread(
            hTargetWnd,
            [](PVOID param) {
                HWND hTargetWnd = (HWND)param;

                UninitializeForCurrentThread();

                if (GetTargetWindowType(hTargetWnd) ==
                    TargetWindowType::FileExplorer) {
                    ApplyBackgroundTranslucentEffect(
                        hTargetWnd, BackgroundTranslucentEffect::kDefault);
                    TriggerWindowCompositionUpdate(hTargetWnd);
                }
            },
            (PVOID)hTargetWnd);
    }

    ClearThemePartCache();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    UninitializeSettingsAndTap();

    LoadSettings();
    LoadThemeSettings();

    auto hTargetWnds = GetTargetWnds();
    for (auto hTargetWnd : hTargetWnds) {
        Wh_Log(L"Reinitializing for %08X", (DWORD)(ULONG_PTR)hTargetWnd);
        RunFromWindowThread(
            hTargetWnd,
            [](PVOID param) {
                HWND hTargetWnd = (HWND)param;

                UninitializeForCurrentThread();
                InitializeForCurrentThread();

                if (GetTargetWindowType(hTargetWnd) ==
                    TargetWindowType::FileExplorer) {
                    ApplyBackgroundTranslucentEffect(hTargetWnd);
                    TriggerWindowCompositionUpdate(hTargetWnd);
                }
            },
            (PVOID)hTargetWnd);
    }

    if (hTargetWnds.size() > 0) {
        Wh_Log(L"Reinitializing - Found target windows");
        InitializeSettingsAndTap();
    }
}
