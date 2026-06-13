// ==WindhawkMod==
// @id              windows-11-notification-center-styler
// @name            Windows 11 Notification Center Styler
// @description     Customize the Notification Center and Action Center with themes contributed by others or create your own
// @version         1.5
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -Wl,--export-all-symbols
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
# Windows 11 Notification Center Styler

Customize the Notification Center and Action Center with themes contributed by
others or create your own.

Also check out the **Windows 11 Taskbar Styler**, **Windows 11 Start Menu
Styler** mods.

## Themes

Themes are collections of styles. The following themes are integrated into the
mod and can be selected in the settings:

[![TranslucentShell](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/TranslucentShell/screenshot-small.png)
\
TranslucentShell](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/TranslucentShell/README.md)

[![Matter](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/Matter/screenshot-small.png)
\
Matter](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/Matter/README.md)

[![Unified](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/Unified/screenshot-small.png)
\
Unified](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/Unified/README.md)

[![10JumpLists](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/10JumpLists/screenshot-small.png)
\
10JumpLists](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/10JumpLists/README.md)

[![WindowGlass](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/WindowGlass/screenshot-small.png)
\
WindowGlass](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/WindowGlass/README.md)

[![Oversimplified&Accentuated](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/Oversimplified&Accentuated/screenshot-small.png)
\
Oversimplified&Accentuated](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/Oversimplified&Accentuated/README.md)

[![TintedGlass](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/TintedGlass/screenshot-small.png)
\
TintedGlass](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/TintedGlass/README.md)

[![Fluid](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/Fluid/screenshot-small.png)
\
Fluid](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/Fluid/README.md)

[![LiquidGlass](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/LiquidGlass/screenshot-small.png)
\
LiquidGlass](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/LiquidGlass/README.md)

[![BetterControl11](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/BetterControl11/screenshot-small.png)
\
BetterControl11](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/BetterControl11/README.md)

[![LayerMicaUI](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/LayerMicaUI/screenshot-small.png)
\
LayerMicaUI](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/LayerMicaUI/README.md)

[![Borderless](https://raw.githubusercontent.com/ramensoftware/windows-11-notification-center-styling-guide/main/Themes/Borderless/screenshot-small.png)
\
Borderless](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/Themes/Borderless/README.md)

More themes can be found in the **Themes** section of [The Windows 11
notification center styling
guide](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/README.md#themes).
Contributions of new themes are welcome!

## Advanced styling

Aside from themes, the settings have two sections: control styles and resource
variables. Control styles allow to override styles, such as size and color, for
the target elements. Resource variables allow to override predefined variables.
For a more detailed explanation and examples, refer to the sections below.

The [UWPSpy](https://ramensoftware.com/uwpspy) tool can be used to inspect the
notification center control elements in real time, and experiment with various
styles.

For a collection of commonly requested notification center styling
customizations, check out [The Windows 11 notification center styling
guide](https://github.com/ramensoftware/windows-11-notification-center-styling-guide/blob/main/README.md).

### Control styles

Each entry has a target control and a list of styles.

The target control is written as `Class` or `Class#Name`, i.e. the target
control class name (the tag name in XAML resource files), such as
`ActionCenter.FocusSessionControl` or `Rectangle`, optionally followed by `#`
and the target control's name (`x:Name` attribute in XAML resource files). The
target control can also include:
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

A couple of practical examples:

#### Hide the focus assist section
**Target**: `ActionCenter.FocusSessionControl` \
**Style**: `Height=0`

#### Hide the notification center
**Target**: `Grid#NotificationCenterGrid` \
**Style**: `Visibility=Collapsed`

#### Shrink the notification center height
Makes panel non full-height when there are fewer notifications (fit to size). \
**Target**: `Grid#NotificationCenterGrid` \
**Style**: `VerticalAlignment=2`

#### Square the corners of the notification center
**Target**: `Grid#NotificationCenterGrid` \
**Style**: `CornerRadius=0`

#### Square the corners of the calendar
**Target**: `Grid#CalendarCenterGrid` \
**Style**: `CornerRadius=0`

#### Square the corners of the quick action center
**Target**: `Grid#ControlCenterRegion` \
**Style**: `CornerRadius=0`

#### Calendar and notification titlebars: titles on the right, buttons on the left
**Target**: `Grid#RootContent` \
**Style**: `FlowDirection=1`

#### Add accelerator key (ALT+X) to clear all notifications
**Target**: `Windows.UI.Xaml.Controls.Button#ClearAll` \
**Style**: `AccessKey=x`

#### Add accelerator key (ALT+E) to expand/collapse the calendar
**Target**: `Windows.UI.Xaml.Controls.Button#ExpandCollapseButton` \
**Style**: `AccessKey=e`

#### Style variables

In addition to literal values, XAML values, and style constants, styles can
reference live property values via global *style variables*. A capture rule of
the form `Property=>VarName` observes a control's property and publishes its
value to a variable. Other styles can then substitute that variable with
`{{VarName}}`. When the source property changes, every style that uses the
variable is recomputed and reapplied.

For example, the following two styles on the same target make it square - the
height tracks the width:

```
ActualWidth=>width1
Height={{width1}}
```

Substitution can appear anywhere in a style's value, including alongside literal
text:

```
Margin=0,{{x1}},0,{{x2 + 10}}
```

Inside `{{ ... }}`, the supported expression syntax is:

* Numbers (e.g. `42`, `3.14`).
* Variable references (a previously captured `VarName`).
* Binary operators `+`, `-`, `*`, `/`, with standard precedence.
* Unary `+` and `-`.
* Parentheses for grouping.
* The two-argument functions `min(a, b)` and `max(a, b)`.

Expressions can be nested (`{{min(a, b + 1) * 2}}`), and `{{ ... }}` markers can
appear inside larger expressions. Brace pairs match innermost-first, so
`{{{x}}}` is parsed as a literal `{`, the variable substitution `{{x}}`, and a
literal `}` - producing `{<value-of-x>}`.

A bare-identifier substitution (`{{VarName}}` with no operators) inserts the
variable's captured string form verbatim. This is meaningful only for primitive
captured types: numeric, boolean, and string. Other captured types (brushes,
thicknesses, etc.) are currently unsupported - substitution of such a variable
is treated as a failure and the style is skipped. Substitutions that involve
arithmetic require numeric source values; using a non-numeric variable in an
expression also skips the style and logs a warning. Referencing a variable that
has never been captured likewise skips the style.

Variables are global - a capture from any matched element overwrites the same
name. Capture rules cannot be combined with `:=` or with the per-rule
`@VisualState` qualifier.

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
  - TranslucentShell: TranslucentShell
  - Matter: Matter
  - Unified: Unified
  - 10JumpLists: 10JumpLists
  - WindowGlass: WindowGlass
  - WindowGlass_variant_alternative: WindowGlass (Alternative)
  - Oversimplified&Accentuated: Oversimplified&Accentuated
  - TintedGlass: TintedGlass
  - Fluid: Fluid
  - LiquidGlass: LiquidGlass
  - BetterControl11: BetterControl11
  - LayerMicaUI: LayerMicaUI
  - Borderless: Borderless
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
*/
// ==/WindhawkModSettings==

#include <xamlom.h>

#include <atomic>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.h>

struct ThemeTargetStyles {
    PCWSTR target;
    std::vector<PCWSTR> styles;
};

struct Theme {
    std::vector<ThemeTargetStyles> targetStyles;
    std::vector<PCWSTR> styleConstants;
    std::vector<PCWSTR> themeResourceVariables;
};

// clang-format off

const Theme g_themeTranslucentShell = {{
    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Border#CalendarHeaderMinimizedOverlay", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"ActionCenter.FocusSessionControl#FocusSessionControl > Grid#FocusGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#00000000\"/>",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=15",
        L"Padding=2,4,2,4"}},
    ThemeTargetStyles{L"Border#JumpListRestyledAcrylic", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=15",
        L"Margin=-2,-2,-2,-2"}},
    ThemeTargetStyles{L"Grid#ControlCenterRegion", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#L1Grid > Border", {
        L"Background:=<SolidColorBrush Color=\"Transparent\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot", {
        L"Background:=<SolidColorBrush Color=\"Transparent\"/>"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent", {
        L"Background:=<SolidColorBrush Color=\"Transparent\"/>"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"Background:=<SolidColorBrush Color=\"Transparent\"/>"}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot", {
        L"Background:=<SolidColorBrush Color=\"Transparent\"/>"}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot > ContentPresenter#PageHeader", {
        L"Background:=<SolidColorBrush Color=\"Transparent\"/>"}},
    ThemeTargetStyles{L"ScrollViewer#ListContent", {
        L"Background:=<SolidColorBrush Color=\"Transparent\"/>"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleToastView#FlexibleNormalToastView", {
        L"Background:=<SolidColorBrush Color=\"Transparent\"/>"}},
    ThemeTargetStyles{L"Border#ToastBackgroundBorder2", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"FocusVisualPrimaryThickness=0,0,0,0",
        L"FocusVisualSecondaryThickness=0,0,0,0"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"FocusVisualPrimaryThickness=0,0,0,0"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView", {
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"ControlCenter.MediaTransportControls#MediaTransportControls > Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion", {
        L"Height=Auto"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ThumbnailImage", {
        L"Width=$thumbnailImageSize",
        L"Height=$thumbnailImageSize",
        L"HorizontalAlignment=Center",
        L"VerticalAlignment=Top",
        L"Grid.Column=1",
        L"Margin=0,2,0,45"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ThumbnailImage > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer", {
        L"VerticalAlignment=Bottom",
        L"Grid.Column=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer > Windows.UI.Xaml.Controls.TextBlock#TitleText", {
        L"TextAlignment=Center"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer > Windows.UI.Xaml.Controls.TextBlock#SubtitleText", {
        L"TextAlignment=Center"}},
}, {
    L"CommonBgBrush=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#25323232\"/>",
    L"thumbnailImageSize=300",
}};

const Theme g_themeMatter = {{
    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {
        L"Background:=$base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$r1",
        L"Shadow :="}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"Background:=$base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$r1",
        L"Shadow :=",
        L"Margin = 0,6,0,6",
        L"MinHeight = 40"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer", {
        L"Background:=$overlay",
        L"CornerRadius=$r2",
        L"Margin=-10,11,-10,-14",
        L"Shadow :="}},
    ThemeTargetStyles{L"Border#CalendarHeaderMinimizedOverlay", {
        L"Background:=$overlay",
        L"CornerRadius=$r2",
        L"Shadow :=",
        L"Margin =-10,-6,-10,-8",
        L"Height = 45"}},
    ThemeTargetStyles{L"ActionCenter.FocusSessionControl#FocusSessionControl > Grid#FocusGrid", {
        L"Background:=$overlay",
        L"CornerRadius=$r2",
        L"Margin=6,7,6,6",
        L"Shadow :="}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background:=$base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$r3",
        L"Padding=1,2,1,2",
        L"Shadow :="}},
    ThemeTargetStyles{L"Border#JumpListRestyledAcrylic", {
        L"Background:=$base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$r3",
        L"Margin=-2,-2,-2,-2",
        L"Shadow :="}},
    ThemeTargetStyles{L"Grid#ControlCenterRegion", {
        L"Background:=$base",
        L"CornerRadius=$r1",
        L"BorderThickness=0,0,0,0",
        L"Shadow :=",
        L"Margin = 0,0,0,-6"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent", {
        L"Background:= $transparent",
        L"Shadow :="}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"Background:=$overlay",
        L"CornerRadius=$r2",
        L"Margin=8,0,8,2",
        L"Shadow :="}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot", {
        L"Background:= $transparent",
        L"Shadow :="}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot > ContentPresenter#PageHeader", {
        L"Background:=$overlay",
        L"CornerRadius=$r2",
        L"Margin=7,7,7,7",
        L"Shadow :="}},
    ThemeTargetStyles{L"ScrollViewer#ListContent", {
        L"Background:=$overlay",
        L"CornerRadius=$r2",
        L"Margin=8,0,8,0",
        L"Shadow :="}},
    ThemeTargetStyles{L"ActionCenter.FlexibleToastView#FlexibleNormalToastView", {
        L"Background:= $transparent",
        L"Shadow :="}},
    ThemeTargetStyles{L"Border#ToastBackgroundBorder2", {
        L"Background:=$base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=16",
        L"Shadow :="}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"Background:=$overlay",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView", {
        L"CornerRadius=16",
        L"Shadow :="}},
    ThemeTargetStyles{L"QuickActions.AccessibleToggleButton#ToggleButton", {
        L"CornerRadius=13",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"QuickActions.AccessibleToggleButton#SplitL2Button", {
        L"CornerRadius =13",
        L"Margin=4,0,-4,0",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Grid#NotificationCenterTopBanner", {
        L"Background:=$overlay",
        L"CornerRadius=$r2",
        L"Margin=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#L1Grid > Border", {
        L"Background:= $transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#FooterButton[AutomationProperties.Name = Edit quick settings]", {
        L"Margin = 0,0,8,0",
        L"CornerRadius=$r3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button[AutomationProperties.AutomationId = Microsoft.QuickAction.Battery]", {
        L"Margin = 2,0,0,0",
        L"CornerRadius=$r3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#FooterButton[AutomationProperties.Name = All settings]", {
        L"Margin = 0,0,-1,0",
        L"CornerRadius = 13"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button[AutomationProperties.AutomationId = Microsoft.QuickAction.Volume]", {
        L"CornerRadius = 10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#VolumeL2Button[AutomationProperties.Name = Select a sound output]", {
        L"CornerRadius = 10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalTrackRect", {
        L"Height = 10",
        L"Fill := $overlay",
        L"RadiusY = 3",
        L"RadiusX = 3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalDecreaseRect", {
        L"Height =10",
        L"RadiusY = 3",
        L"RadiusX = 3",
        L"Margin = 0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb", {
        L"Visibility = 1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion", {
        L"Height=Auto",
        L"CornerRadius=$r1",
        L"BorderThickness = 0",
        L"Background:=$base",
        L"Shadow :=",
        L"Padding = 0,0,0,12",
        L"Margin = 0,0,0,12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ThumbnailImage", {
        L"Width=$thumbnailImageSize",
        L"Height=$thumbnailImageSize",
        L"HorizontalAlignment=Center",
        L"VerticalAlignment=Top",
        L"Grid.Column=1",
        L"Margin=0,2,0,45",
        L"CornerRadius=$r2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer", {
        L"VerticalAlignment=Bottom",
        L"Margin = 0,5,0,-5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer > Windows.UI.Xaml.Controls.TextBlock#Title", {
        L"TextAlignment=Center",
        L"FontFamily = Tektur",
        L"FontSize = 18"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer > Windows.UI.Xaml.Controls.TextBlock#Subtitle", {
        L"TextAlignment=Center",
        L"FontFamily = Montserrat",
        L"Margin = 0,3,0,0",
        L"FontWeight= 600"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#MediaButtonsListView", {
        L"VerticalAlignment=Top",
        L"Height=48",
        L"Margin = 0,12,0,-12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#PreviousButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal:=$overlay",
        L"Background@PointerOver:=$accentColor",
        L"Background@Pressed:=$overlay2",
        L"Width=40",
        L"Height= 30",
        L"CornerRadius = 6",
        L"Margin = 15,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PlayPauseButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal:=$overlay",
        L"Background@PointerOver:=$accentColor",
        L"Background@Pressed:=$overlay2",
        L"Width=40",
        L"Height = 40",
        L"CornerRadius = 8",
        L"Margin = -10,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#NextButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal:=$overlay",
        L"Background@PointerOver:=$accentColor",
        L"Background@Pressed:=$overlay2",
        L"Width=40",
        L"Height = 30",
        L"CornerRadius = 6",
        L"Margin = -20,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AppNameText", {
        L"FontFamily = Tektur",
        L"FontSize = 16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#IconImage", {
        L"Height = 20",
        L"Width = 20"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot", {
        L"Background:= $transparent"}},
    ThemeTargetStyles{L"Grid#ToastPeekRegion", {
        L"Background =",
        L"RenderTransform:=<TranslateTransform Y=\"-495\" X=\"395\" />",
        L"Grid.Column = 0",
        L"Grid.Row = 2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.CalendarViewDayItem > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius = 8",
        L"Margin = 1,2,1,2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.CalendarViewDayItem", {
        L"CornerRadius = 8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Control > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius = 8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarViewItem", {
        L"CornerRadius = 8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewHeaderItem", {
        L"Margin = 50,6,50,2",
        L"CornerRadius = 8",
        L"Height = 35"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SettingsButton", {
        L"CornerRadius = 4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#DismissButton", {
        L"CornerRadius = 4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#CalendarHeader", {
        L"Margin = 6,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter", {
        L"Margin = 1,2,1,2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#WeekDayNames", {
        L"Background := <SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity = \"0.8\" />",
        L"CornerRadius = 8",
        L"Margin = 4,0,4,0",
        L"Padding = 0,-5,0,-3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewItem", {
        L"CornerRadius=$r3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background := <SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity = \"0.5\" />",
        L"BorderThickness = 0",
        L"CornerRadius = 8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#ItemOpaquePlating", {
        L"Background := $overlay2",
        L"BorderThickness = 0",
        L"CornerRadius=$r3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#StandardHeroContainer", {
        L"Margin = 12,0,12,0",
        L"CornerRadius = 0",
        L"Height = 150"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar#VerticalScrollBar", {
        L"Visibility = 1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SliderContainer", {
        L"Margin = 0-2,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#BackButton", {
        L"CornerRadius=$r3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#OuterBorder", {
        L"RadiusX = 6",
        L"RadiusY = 6",
        L"Height = 18"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobOff", {
        L"RadiusY = 3",
        L"RadiusX = 3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SwitchKnobOn", {
        L"CornerRadius = 3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobBounds", {
        L"RadiusX = 6",
        L"RadiusY = 6",
        L"Height = 18"}},
    ThemeTargetStyles{L"ActionCenter.NotificationListViewItem", {
        L"Margin = 5,2,5,3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid[AutomationProperties.LocalizedLandmarkType = Footer]", {
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"NetworkUX.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root", {
        L"CornerRadius = 12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Border", {
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Button#ClearAll", {
        L"AccessKey=x"}},
    ThemeTargetStyles{L"Button#ExpandCollapseButton", {
        L"AccessKey=e"}},
}, {
    L"transparent = <SolidColorBrush Color=\"Transparent\"/>",
    L"base = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"0.6\" Opacity = \"1\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
    L"overlay = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"{ThemeResource LayerFillColorDefault}\" />",
    L"accentColor =<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity = \"1\" />",
    L"overlay2 = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"0.4\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" />",
    L"r1 = 18",
    L"r2 = 14",
    L"r3 = 12",
    L"thumbnailImageSize = 300",
}};

const Theme g_themeUnified = {{
    ThemeTargetStyles{L"ActionCenter.FocusSessionControl", {
        L"Height=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid", {
        L"CornerRadius=0,0,6,6",
        L"Margin=0,0,0,12",
        L"BorderThickness=1,0,1,1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NotificationCenterGrid", {
        L"CornerRadius=6,6,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion", {
        L"CornerRadius=6,6,0,0",
        L"BorderThickness=1,1,1,0",
        L"Margin=0,0,0,-6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#CalendarHeaderMinimizedOverlay", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ScrollViewer#CalendarControlScrollViewer", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.Border", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ComboBox > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.CalendarViewDayItem > Windows.UI.Xaml.Controls.Border", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Control > Windows.UI.Xaml.Controls.Border", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\"/>"}},
}};

const Theme g_theme10JumpLists = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#JumpListGrid", {
        L"Margin=0,0,0,0",
        L"CornerRadius=0",
        L"Width=256"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#JumpListRestyledAcrylic", {
        L"CornerRadius=0",
        L"Background=Transparent",
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemListView#SystemItemList", {
        L"Width=256"}},
    ThemeTargetStyles{L"JumpViewUI.TaskbarJumpListFrame", {
        L"Width=256"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListView#ItemList", {
        L"Width=256",
        L"Padding=0,5,0,5"}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemControl > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PinButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PinButton", {
        L"Width=30",
        L"Height=30"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListViewItem", {
        L"Margin=0,0,0,0",
        L"Height=30"}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemListViewItem", {
        L"Margin=0,0,0,0",
        L"Height=30"}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemListViewItem > Windows.UI.Xaml.Controls.Grid#LayoutRoot@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.9\" FallbackColor=\"{ThemeResource SystemListLowColor}\" />",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"BorderThickness=1,1,1,1"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListViewItem > Windows.UI.Xaml.Controls.Grid#LayoutRoot@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.9\" FallbackColor=\"{ThemeResource SystemListLowColor}\" />",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"BorderThickness=1,1,1,1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PinButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Shapes.Rectangle", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#DisplayNameTextBlock", {
        L"FontSize=12",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListCategoryHeaderControl > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock#HeadingTextBlock", {
        L"Margin=12,10,12,6",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SystemItemsContainer > Windows.UI.Xaml.Shapes.Rectangle", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SystemItemsContainer", {
        L"Padding=0,5,0,5"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListViewItem > Windows.UI.Xaml.Controls.Grid#LayoutRoot > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid#LayoutRoot > Windows.UI.Xaml.Shapes.Rectangle", {
        L"Margin=12,4,12,4"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListControl#JumpList", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PinButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SystemListLowColor}\" TintOpacity=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<AcrylicBrush TintColor=\"{ThemeResource SystemListLowColor}\" TintOpacity=\"1\" Opacity=\"0.9\" FallbackColor=\"{ThemeResource SystemListMediumColor}\"/>",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#JumpListAcrylic", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SystemItemsContainer > Windows.UI.Xaml.Controls.Border#SystemItemsAcrylic", {
        L"Visibility=Visible",
        L"Margin=0,-5,0,-5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=0",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAltHighColor}\" TintOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutItem", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter#IconContent > Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe MDL2 Assets, Segoe UI"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutItem > Grid > TextBlock", {
        L"FontSize=12",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutItem > Grid", {
        L"BorderThickness=1,1,1,1",
        L"Margin=0"}},
}};

const Theme g_themeWindowGlass = {{
    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius",
        L"Margin=0,6,0,6",
        L"MinHeight=40",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer", {
        L"Background:=$ElementBG",
        L"CornerRadius=$CR2",
        L"Margin=-10,11,-10,-14",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Border#CalendarHeaderMinimizedOverlay", {
        L"Background:=$ElementBG",
        L"CornerRadius=$CR2",
        L"Margin=-10,-6,-10,-8",
        L"Height=45",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"ActionCenter.FocusSessionControl#FocusSessionControl > Grid#FocusGrid", {
        L"Background:=$Background",
        L"CornerRadius=$CR2",
        L"Margin=6,7,6,6",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CR3",
        L"Padding=1,2,1,2",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"MenuFlyoutItem > Grid#LayoutRoot", {
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"Border#JumpListRestyledAcrylic", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CR3",
        L"Margin=-2,-2,-2,-2",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ControlCenterRegion", {
        L"Background:=$Background",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"Background:=$overlay",
        L"CornerRadius=$CR2",
        L"Margin=8,0,8,2"}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot > ContentPresenter#PageHeader", {
        L"Background:=$overlay",
        L"CornerRadius=$CR2",
        L"Margin=7,7,7,7"}},
    ThemeTargetStyles{L"ScrollViewer#ListContent", {
        L"Background:=$overlay",
        L"CornerRadius=$CR2",
        L"Margin=8,0,8,0"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleToastView#FlexibleNormalToastView", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Border#ToastBackgroundBorder2", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=16",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView", {
        L"CornerRadius=16"}},
    ThemeTargetStyles{L"Grid#NotificationCenterTopBanner", {
        L"Background=Transparent",
        L"CornerRadius=$CR2",
        L"Margin=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#L1Grid > Border", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#FooterButton[AutomationProperties.Name=Edit quick settings]", {
        L"Margin=0,0,8,0",
        L"CornerRadius=$CR3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button[AutomationProperties.AutomationId=Microsoft.QuickAction.Battery]", {
        L"Margin=2,0,0,0",
        L"CornerRadius=$CR3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#FooterButton[AutomationProperties.Name=All settings]", {
        L"Margin=0,0,-1,0",
        L"CornerRadius=13",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button[AutomationProperties.AutomationId=Microsoft.QuickAction.Volume]", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#VolumeL2Button[AutomationProperties.Name=Select a sound output]", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalTrackRect", {
        L"Height=10",
        L"Fill:=$overlay",
        L"RadiusY=5",
        L"RadiusX=5",
        L"Margin=0,-10,10,-10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalDecreaseRect", {
        L"Height=10",
        L"RadiusY=5",
        L"RadiusX=5",
        L"Margin=0,-10,-10,-10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb", {
        L"Visibility=Visible",
        L"Height=25",
        L"Width=40",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion", {
        L"Height=100",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"Background:=$Background",
        L"Margin=0,10,0,0",
        L"BorderBrush:=$BorderBrush",
        L"Grid.Row=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AlbumTextAndArtContainer", {
        L"Height=55",
        L"MaxWidth=150",
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ThumbnailImage", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer", {
        L"VerticalAlignment=Center",
        L"HorizontalAlignment=Left",
        L"Margin=0,0,10,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer > Windows.UI.Xaml.Controls.TextBlock#Title", {
        L"TextAlignment=Center",
        L"FontSize=18"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer > Windows.UI.Xaml.Controls.TextBlock#Subtitle", {
        L"TextAlignment=Center",
        L"FontFamily=vivo Sans EN VF",
        L"Margin=0,3,0,0",
        L"FontWeight=600"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#MediaButtonsListView", {
        L"VerticalAlignment=Center",
        L"Height=20",
        L"Margin=130,-60,0,0",
        L"Width=Auto",
        L"HorizontalAlignment=Right",
        L"Visibility=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#PreviousButton", {
        L"Width=40",
        L"Height=40",
        L"Margin=10,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PlayPauseButton", {
        L"Width=40",
        L"Height=40",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#NextButton", {
        L"Width=40",
        L"Height=30",
        L"Margin=0,0,10,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AppNameText", {
        L"FontFamily=vivo Sans EN VF",
        L"FontSize=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#IconImage", {
        L"Height=20",
        L"Width=20"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#ToastPeekRegion", {
        L"Background=",
        L"RenderTransform:=<TranslateTransform Y=\"-495\" X=\"395\" />",
        L"Grid.Column=0",
        L"Grid.Row=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.CalendarViewDayItem > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=8",
        L"Margin=1,2,1,2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.CalendarViewDayItem", {
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Control > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarViewItem", {
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewHeaderItem", {
        L"Margin=50,6,50,2",
        L"CornerRadius=8",
        L"Height=35"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SettingsButton", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#DismissButton", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#CalendarHeader", {
        L"Margin=6,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter", {
        L"Margin=1,2,1,2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#WeekDayNames", {
        L"Background:=$ElementSysColor",
        L"CornerRadius=8",
        L"Margin=4,0,4,0",
        L"Padding=0,-5,0,-3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewItem", {
        L"CornerRadius=$CR3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"0.5\"/>",
        L"BorderThickness=0",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#ItemOpaquePlating", {
        L"Background:=$overlay2",
        L"BorderThickness=0",
        L"CornerRadius=$CR3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#StandardHeroContainer", {
        L"Margin=12,0,12,0",
        L"CornerRadius=0",
        L"Height=150"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar#VerticalScrollBar", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SliderContainer", {
        L"Margin=0-2,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#BackButton", {
        L"CornerRadius=$CR3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#OuterBorder", {
        L"RadiusX=8",
        L"RadiusY=8",
        L"Height=18"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobOff", {
        L"RadiusY=8",
        L"RadiusX=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SwitchKnobOn", {
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobBounds", {
        L"RadiusX=8",
        L"RadiusY=8",
        L"Height=18"}},
    ThemeTargetStyles{L"ActionCenter.NotificationListViewItem", {
        L"Margin=5,2,5,3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid[AutomationProperties.LocalizedLandmarkType=Footer]", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"NetworkUX.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Border", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Button#ClearAll", {
        L"AccessKey=x"}},
    ThemeTargetStyles{L"Button#ExpandCollapseButton", {
        L"AccessKey=e"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#ToggleButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"CornerRadius=$CR2",
        L"BorderThickness=$ElementBorderThickness",
        L"BorderBrush:=$ElementBorderBrush"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#SplitL2Button > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"CornerRadius=30",
        L"BorderThickness=$ElementBorderThickness",
        L"BorderBrush:=$ElementBorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=12",
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Shapes.Ellipse#SliderInnerThumb", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter#LayoutRoot", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#PreviousButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Foreground@Normal:=$ElementSysColor",
        L"Foreground@PointerOver:=$ElementSysColor2",
        L"Foreground@Pressed:=$ElementSysColor3",
        L"Foreground@Disabled:=$ElementSysColor4",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PlayPauseButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Foreground@Normal:=$ElementSysColor",
        L"Foreground@PointerOver:=$ElementSysColor2",
        L"Foreground@Pressed:=$ElementSysColor3",
        L"Foreground@Disabled:=$ElementSysColor4",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#NextButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Foreground@Normal:=$ElementSysColor",
        L"Foreground@PointerOver:=$ElementSysColor2",
        L"Foreground@Pressed:=$ElementSysColor3",
        L"Foreground@Disabled:=$ElementSysColor4",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#ControlCenterRegion", {
        L"Grid.Row=0"}},
    ThemeTargetStyles{L"ControlCenter.MediaTransportControls", {
        L"VerticalAlignment=2",
        L"Grid.Row=1",
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"Grid#RootGrid", {
        L"VerticalAlignment=3",
        L"MinHeight=0"}},
}, {
    L"Translucent=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#10808080\"/>",
    L"Glass=<WindhawkBlur BlurAmount=\"5\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Frosted=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Acrylic=<WindhawkBlur BlurAmount=\"30\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.8\" />",
    L"Background=$Glass",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"BorderBrush2=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"{ThemeResource SystemChromeHighColor}\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SystemChromeLowColor}\" Offset=\"0.15\" /><GradientStop Color=\"{ThemeResource SystemChromeHighColor}\" Offset=\"0.95\" /></LinearGradientBrush>",
    L"overlay=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.1\" />",
    L"overlay2=<WindhawkBlur BlurAmount=\"20\" TintColor=\"#60353535\"/>",
    L"CornerRadius=20",
    L"CR2=14",
    L"CR3=12",
    L"BorderThickness=0.3,1,0.3,0.3",
    L"ElementBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.3\" />",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"ElementCornerRadius=20",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"ElementSysColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"1\" />",
    L"ElementSysColor2=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1\" />",
    L"ElementSysColor3=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\" Opacity=\"1\" />",
    L"ElementSysColor4=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark1}\" Opacity=\"1\" />",
}};

const Theme g_themeWindowGlass_variant_alternative = {{
    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius",
        L"Margin=0,6,0,6",
        L"MinHeight=40",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer", {
        L"Background:=$ElementBG",
        L"CornerRadius=$CR2",
        L"Margin=-10,11,-10,-14",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Border#CalendarHeaderMinimizedOverlay", {
        L"Background:=$ElementBG",
        L"CornerRadius=$CR2",
        L"Margin=-10,-6,-10,-8",
        L"Height=45",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"ActionCenter.FocusSessionControl#FocusSessionControl > Grid#FocusGrid", {
        L"Background:=$Background",
        L"CornerRadius=$CR2",
        L"Margin=6,7,6,6",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CR3",
        L"Padding=1,2,1,2",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"MenuFlyoutItem > Grid#LayoutRoot", {
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"Border#JumpListRestyledAcrylic", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CR3",
        L"Margin=-2,-2,-2,-2",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ControlCenterRegion", {
        L"Background:=$Background",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"Margin=0,5,0,0",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"Background:=$overlay",
        L"CornerRadius=$CR2",
        L"Margin=8,0,8,2"}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot > ContentPresenter#PageHeader", {
        L"Background:=$overlay",
        L"CornerRadius=$CR2",
        L"Margin=7,7,7,7"}},
    ThemeTargetStyles{L"ScrollViewer#ListContent", {
        L"Background:=$overlay",
        L"CornerRadius=$CR2",
        L"Margin=8,0,8,0"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleToastView#FlexibleNormalToastView", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Border#ToastBackgroundBorder2", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=16",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView", {
        L"CornerRadius=16"}},
    ThemeTargetStyles{L"Grid#NotificationCenterTopBanner", {
        L"Background=Transparent",
        L"CornerRadius=$CR2",
        L"Margin=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#L1Grid > Border", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#FooterButton[AutomationProperties.Name=Edit quick settings]", {
        L"Margin=0,0,8,0",
        L"CornerRadius=$CR3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button[AutomationProperties.AutomationId=Microsoft.QuickAction.Battery]", {
        L"Margin=2,0,0,0",
        L"CornerRadius=$CR3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#FooterButton[AutomationProperties.Name=All settings]", {
        L"Margin=0,0,-1,0",
        L"CornerRadius=13",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button[AutomationProperties.AutomationId=Microsoft.QuickAction.Volume]", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#VolumeL2Button[AutomationProperties.Name=Select a sound output]", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalTrackRect", {
        L"Height=10",
        L"Fill:=$overlay",
        L"RadiusY=5",
        L"RadiusX=5",
        L"Margin=0,-10,10,-10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalDecreaseRect", {
        L"Height=10",
        L"RadiusY=5",
        L"RadiusX=5",
        L"Margin=0,-10,-10,-10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb", {
        L"Visibility=Visible",
        L"Height=25",
        L"Width=40",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion", {
        L"Height=100",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"Background:=$Background",
        L"Margin=0,20,0,5",
        L"BorderBrush:=$BorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AlbumTextAndArtContainer", {
        L"Height=55",
        L"MaxWidth=150",
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ThumbnailImage", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer", {
        L"VerticalAlignment=Center",
        L"HorizontalAlignment=Left",
        L"Margin=0,0,10,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer > Windows.UI.Xaml.Controls.TextBlock#Title", {
        L"TextAlignment=Center",
        L"FontSize=18"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer > Windows.UI.Xaml.Controls.TextBlock#Subtitle", {
        L"TextAlignment=Center",
        L"FontFamily=vivo Sans EN VF",
        L"Margin=0,3,0,0",
        L"FontWeight=600"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#MediaButtonsListView", {
        L"VerticalAlignment=Center",
        L"Height=20",
        L"Margin=130,-60,0,0",
        L"Width=Auto",
        L"HorizontalAlignment=Right",
        L"Visibility=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#PreviousButton", {
        L"Width=40",
        L"Height=40",
        L"Margin=10,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PlayPauseButton", {
        L"Width=40",
        L"Height=40",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#NextButton", {
        L"Width=40",
        L"Height=30",
        L"Margin=0,0,10,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AppNameText", {
        L"FontFamily=vivo Sans EN VF",
        L"FontSize=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#IconImage", {
        L"Height=20",
        L"Width=20"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#ToastPeekRegion", {
        L"Background=",
        L"RenderTransform:=<TranslateTransform Y=\"-495\" X=\"395\" />",
        L"Grid.Column=0",
        L"Grid.Row=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.CalendarViewDayItem > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=8",
        L"Margin=1,2,1,2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.CalendarViewDayItem", {
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Control > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarViewItem", {
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewHeaderItem", {
        L"Margin=50,6,50,2",
        L"CornerRadius=8",
        L"Height=35"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SettingsButton", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#DismissButton", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#CalendarHeader", {
        L"Margin=6,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter", {
        L"Margin=1,2,1,2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#WeekDayNames", {
        L"Background:=$ElementSysColor",
        L"CornerRadius=8",
        L"Margin=4,0,4,0",
        L"Padding=0,-5,0,-3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewItem", {
        L"CornerRadius=$CR3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"0.5\"/>",
        L"BorderThickness=0",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#ItemOpaquePlating", {
        L"Background:=$overlay2",
        L"BorderThickness=0",
        L"CornerRadius=$CR3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#StandardHeroContainer", {
        L"Margin=12,0,12,0",
        L"CornerRadius=0",
        L"Height=150"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar#VerticalScrollBar", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SliderContainer", {
        L"Margin=0-2,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#BackButton", {
        L"CornerRadius=$CR3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#OuterBorder", {
        L"RadiusX=8",
        L"RadiusY=8",
        L"Height=18"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobOff", {
        L"RadiusY=8",
        L"RadiusX=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SwitchKnobOn", {
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobBounds", {
        L"RadiusX=8",
        L"RadiusY=8",
        L"Height=18"}},
    ThemeTargetStyles{L"ActionCenter.NotificationListViewItem", {
        L"Margin=5,2,5,3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid[AutomationProperties.LocalizedLandmarkType=Footer]", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"NetworkUX.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Border", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Button#ClearAll", {
        L"AccessKey=x"}},
    ThemeTargetStyles{L"Button#ExpandCollapseButton", {
        L"AccessKey=e"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#ToggleButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"CornerRadius=$CR2",
        L"BorderThickness=$ElementBorderThickness",
        L"BorderBrush:=$ElementBorderBrush"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#SplitL2Button > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"CornerRadius=30",
        L"BorderThickness=$ElementBorderThickness",
        L"BorderBrush:=$ElementBorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=12",
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Shapes.Ellipse#SliderInnerThumb", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter#LayoutRoot", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#PreviousButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Foreground@Normal:=$ElementSysColor",
        L"Foreground@PointerOver:=$ElementSysColor2",
        L"Foreground@Pressed:=$ElementSysColor3",
        L"Foreground@Disabled:=$ElementSysColor4",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PlayPauseButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Foreground@Normal:=$ElementSysColor",
        L"Foreground@PointerOver:=$ElementSysColor2",
        L"Foreground@Pressed:=$ElementSysColor3",
        L"Foreground@Disabled:=$ElementSysColor4",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#NextButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Foreground@Normal:=$ElementSysColor",
        L"Foreground@PointerOver:=$ElementSysColor2",
        L"Foreground@Pressed:=$ElementSysColor3",
        L"Foreground@Disabled:=$ElementSysColor4",
        L"Background=Transparent"}},
}, {
    L"Translucent=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#10808080\"/>",
    L"Glass=<WindhawkBlur BlurAmount=\"5\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Frosted=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Acrylic=<WindhawkBlur BlurAmount=\"30\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.8\" />",
    L"Background=$Glass",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"BorderBrush2=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"{ThemeResource SystemChromeHighColor}\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SystemChromeLowColor}\" Offset=\"0.15\" /><GradientStop Color=\"{ThemeResource SystemChromeHighColor}\" Offset=\"0.95\" /></LinearGradientBrush>",
    L"overlay=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.1\" />",
    L"overlay2=<WindhawkBlur BlurAmount=\"20\" TintColor=\"#60353535\"/>",
    L"CornerRadius=20",
    L"CR2=14",
    L"CR3=12",
    L"BorderThickness=0.3,1,0.3,0.3",
    L"ElementBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.3\" />",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"ElementCornerRadius=20",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"ElementSysColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"1\" />",
    L"ElementSysColor2=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1\" />",
    L"ElementSysColor3=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\" Opacity=\"1\" />",
    L"ElementSysColor4=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark1}\" Opacity=\"1\" />",
}};

const Theme g_themeOversimplified_Accentuated = {{
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background:=$DarkAccent",
        L"BorderBrush=Transparent",
        L"Shadow:="}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot", {
        L"Background:=$DarkAccent",
        L"BorderBrush:=$Reveal",
        L"Shadow:="}},
    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {
        L"Background:=$Alt",
        L"BorderBrush=Transparent",
        L"Shadow:="}},
    ThemeTargetStyles{L"TextBlock#NotificationsTextBlock", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Button#ClearAll", {
        L"AccessKey=C"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#DoNotDisturbButton", {
        L"AccessKey=D"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#DoNotDisturbButtonIcon", {
        L"Height=16",
        L"Width=16"}},
    ThemeTargetStyles{L"Grid#DoNotDisturbSubtext", {
        L"Background:=$Accent",
        L"BorderBrush:=$Reveal",
        L"BorderThickness=2",
        L"CornerRadius=5",
        L"Margin=0,0,0,10"}},
    ThemeTargetStyles{L"Grid#DoNotDisturbSubtext > TextBlock[1]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#DoNotDisturbSubtext > TextBlock[2]", {
        L"HorizontalAlignment=Center",
        L"FontSize=18"}},
    ThemeTargetStyles{L"Grid#DoNotDisturbSubtext > TextBlock[3]", {
        L"TextAlignment=Center",
        L"FontSize=11"}},
    ThemeTargetStyles{L"Grid#DoNotDisturbSubtext > Button", {
        L"HorizontalAlignment=Center",
        L"Margin= 0,0,0,0"}},
    ThemeTargetStyles{L"TextBlock#NotificationSettingsButtonText[Text=Notification settings]", {
        L"Text=Settings"}},
    ThemeTargetStyles{L"Border#ItemOpaquePlating", {
        L"BorderBrush:=$Reveal"}},
    ThemeTargetStyles{L"Border#StandardImageBorder", {
        L"Height=30",
        L"Width=30"}},
    ThemeTargetStyles{L"Grid#GroupTitleGrid > TextBlock#Title", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid > Button#VerbButton", {
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Border#PopupBorder", {
        L"Background:=$DarkAccent",
        L"Shadow:="}},
    ThemeTargetStyles{L"ProgressBar#progressBar > Grid > Border#DeterminateRoot", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Border#ToastBackgroundBorder2", {
        L"Background:=$Alt",
        L"BorderBrush=Transparent",
        L"CornerRadius=15",
        L"Shadow:="}},
    ThemeTargetStyles{L"Border#AppLogoBorder2", {
        L"Height=30",
        L"Width=30"}},
    ThemeTargetStyles{L"Border#AppLogoBorder", {
        L"Height=30",
        L"Width=30"}},
    ThemeTargetStyles{L"Image#AppLogo2", {
        L"Height=30",
        L"Width=30"}},
    ThemeTargetStyles{L"Grid#ToastTitleBar > TextBlock#SenderName", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"Background:=$Alt",
        L"BorderBrush=Transparent",
        L"CornerRadius=20",
        L"Shadow:="}},
    ThemeTargetStyles{L"Border#CalendarHeaderMinimizedOverlay", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Button#ExpandCollapseButton", {
        L"Background=Transparent",
        L"BorderBrush=Transparent",
        L"AccessKey=E"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"CalendarViewDayItem", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"CalendarViewDayItem > Border", {
        L"BorderBrush:= <RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarPanel#YearViewPanel > Control", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarPanel#YearViewPanel > Control > Border", {
        L"BorderBrush:=$Reveal",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarPanel#DecadeViewPanel > Control", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarPanel#DecadeViewPanel > Control > Border", {
        L"BorderBrush:=$Reveal",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Grid#FocusGrid", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Button#IncreaseTimeButton", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Button#DecreaseTimeButton", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Button#StartButton", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Grid#ControlCenterRegion", {
        L"Background=Transparent",
        L"BorderBrush=Transparent",
        L"CornerRadius=20",
        L"Shadow:="}},
    ThemeTargetStyles{L"Grid#L1Grid > Border", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#L1Grid", {
        L"Background:=$Alt",
        L"BorderBrush=Transparent",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedGridView > Grid > GridView#RootGridView", {
        L"Height=auto"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager#QuickActionsPager", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"ContentPresenter#ContentPresenter", {
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton", {
        L"Height=60"}},
    ThemeTargetStyles{L"ContentControl > ContentPresenter > Grid > Grid", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton > ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal:= <AcrylicBrush TintColor=\"{ThemeResource SystemAltHighColor}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" />",
        L"Background@Checked:=$Accent",
        L"Background@CheckedPointerOver:= <AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorLight1}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColorLight1}\" />",
        L"Background@CheckedPressed:= <AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"Background@CheckedDisabled:= <AcrylicBrush TintColor=\"red\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"red\" />"}},
    ThemeTargetStyles{L"Grid > Microsoft.UI.Xaml.Controls.AnimatedIcon", {
        L"Height=30",
        L"Width=30"}},
    ThemeTargetStyles{L"ContentPresenter#Content > StackPanel > TextBlock", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#ToggleButton[AutomationProperties.Name=Accessibility]", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#ToggleButton[AutomationProperties.Name=Cast]", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#ToggleButton[AutomationProperties.Name=Project]", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedGridView > Grid", {
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Rectangle#HorizontalTrackRect", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"Rectangle#HorizontalDecreaseRect", {
        L"Fill:=$Accent",
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Border", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb", {
        L"Width=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#BrightnessPlayer", {
        L"Height=25",
        L"Width=25"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#FooterButtonIcon", {
        L"Height=25",
        L"Width=25"}},
    ThemeTargetStyles{L"Button#VolumeL2Button > ContentPresenter > StackPanel > FontIcon[1]", {
        L"FontSize=20"}},
    ThemeTargetStyles{L"StackPanel > ContentPresenter > ContentControl > ContentPresenter > Button > ContentPresenter > StackPanel > TextBlock#Icon", {
        L"FontSize=25"}},
    ThemeTargetStyles{L"StackPanel > ContentPresenter > ContentControl > ContentPresenter > Button > ContentPresenter > StackPanel > TextBlock[2]", {
        L"FontSize=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton > ContentPresenter > Microsoft.UI.Xaml.Controls.AnimatedIcon", {
        L"Height=25",
        L"Width=25"}},
    ThemeTargetStyles{L"Grid#L1Grid > Grid", {
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"ContentPresenter#PageHeader", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#FullScreenPageRoot > ContentPresenter#PageHeader > Border > Grid > Button#BackButton", {
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"ContentPresenter > Grid#FullScreenPageRoot", {
        L"Background:=$DarkAccent"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid > ScrollViewer#ListContent", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Border#SwitchKnobOn", {
        L"Background="}},
    ThemeTargetStyles{L"StackPanel > ContentPresenter > Border", {
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Border#WADFeatureFooter", {
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRegion", {
        L"Background:=$DarkAccent",
        L"BorderBrush=Transparent",
        L"CornerRadius=20",
        L"Height=Auto",
        L"Shadow:="}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot > Grid[2]", {
        L"Margin=-8,0,0,12"}},
    ThemeTargetStyles{L"StackPanel#PrimaryAndSecondaryTextContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#AlbumTextAndArtContainer", {
        L"HorizontalAlignment=Center"}},
    ThemeTargetStyles{L"Grid#ThumbnailImage", {
        L"CornerRadius=15",
        L"Height=300",
        L"Width=300"}},
    ThemeTargetStyles{L"Border#JumpListRestyledAcrylic", {
        L"Background:=$DarkAccent",
        L"CornerRadius=15",
        L"Shadow:="}},
    ThemeTargetStyles{L"GridViewItem[1] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=1"}},
    ThemeTargetStyles{L"GridViewItem[2] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=2"}},
    ThemeTargetStyles{L"GridViewItem[3] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=3"}},
    ThemeTargetStyles{L"GridViewItem[4] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=4"}},
    ThemeTargetStyles{L"GridViewItem[5] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=5"}},
    ThemeTargetStyles{L"GridViewItem[6] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=6"}},
    ThemeTargetStyles{L"GridViewItem[7] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=7"}},
    ThemeTargetStyles{L"GridViewItem[8] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=8"}},
    ThemeTargetStyles{L"GridViewItem[9] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=9"}},
    ThemeTargetStyles{L"GridViewItem[10] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=0"}},
    ThemeTargetStyles{L"GridViewItem[11] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=-"}},
    ThemeTargetStyles{L"GridViewItem[1] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=1"}},
    ThemeTargetStyles{L"GridViewItem[2] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=2"}},
    ThemeTargetStyles{L"GridViewItem[3] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=3"}},
    ThemeTargetStyles{L"GridViewItem[4] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=4"}},
    ThemeTargetStyles{L"GridViewItem[5] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=5"}},
    ThemeTargetStyles{L"GridViewItem[6] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=6"}},
    ThemeTargetStyles{L"GridViewItem[7] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=7"}},
    ThemeTargetStyles{L"GridViewItem[8] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=8"}},
    ThemeTargetStyles{L"GridViewItem[9] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=9"}},
    ThemeTargetStyles{L"GridViewItem[10] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=0"}},
    ThemeTargetStyles{L"GridViewItem[11] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"AccessKey=-"}},
    ThemeTargetStyles{L"Grid#RootGrid > QuickActions.ControlCenter.FrameWithContentChanged#L2Frame", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow", {
        L"Background=Transparent"}},
}, {
    L"Alt = <AcrylicBrush TintColor=\"{ThemeResource SystemAltHighColor}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAltHighColor}\" />",
    L"Accent = <AcrylicBrush TintColor=\"{ThemeResource SystemAccentColor}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColor}\" />",
    L"DarkAccent = <AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.3\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
    L"SolidAccent = <SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" Opacity=\"1\"/>",
    L"Reveal = <RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
}};

const Theme g_themeTintedGlass = {{
    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {
        L"Background:=$Base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$Radius"}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"Background:=$Base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$Radius"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer", {
        L"BorderThickness=0,0,0,0",
        L"Background:=$Transparent"}},
    ThemeTargetStyles{L"Border#CalendarHeaderMinimizedOverlay", {
        L"Background:=$Transparent",
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"ActionCenter.FocusSessionControl#FocusSessionControl > Grid#FocusGrid", {
        L"Background:=$Transparent",
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=$Overlay",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$Radius",
        L"Padding=2,4,2,4"}},
    ThemeTargetStyles{L"Border#JumpListRestyledAcrylic", {
        L"Background:=$Base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$Radius",
        L"Margin=-2,-2,-2,-2"}},
    ThemeTargetStyles{L"Grid#ControlCenterRegion", {
        L"Background:=$Base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$Radius"}},
    ThemeTargetStyles{L"Grid#L1Grid > Border", {
        L"Background:=$Transparent",
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRegion", {
        L"Background:=$Base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$Radius"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot", {
        L"Background:=$Transparent"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent", {
        L"Background:=$Transparent"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"Background:=$Transparent"}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot", {
        L"Background:=$Transparent"}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot > ContentPresenter#PageHeader", {
        L"Background:=$Transparent"}},
    ThemeTargetStyles{L"ScrollViewer#ListContent", {
        L"Background:=$Transparent"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleToastView#FlexibleNormalToastView", {
        L"Background:=$Transparent"}},
    ThemeTargetStyles{L"Border#ToastBackgroundBorder2", {
        L"Background:=$Base",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=$Radius"}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"Background:=$Transparent",
        L"CornerRadius=$Radius"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"CornerRadius=$Radius"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView", {
        L"CornerRadius=$Radius"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRegion", {
        L"Height=Auto"}},
    ThemeTargetStyles{L"Grid#ThumbnailImage", {
        L"Width=$thumbnailImageSize",
        L"Height=$thumbnailImageSize",
        L"HorizontalAlignment=Center",
        L"VerticalAlignment=Top",
        L"Grid.Column=1",
        L"Margin=0,2,0,45"}},
    ThemeTargetStyles{L"Grid#ThumbnailImage > Border", {
        L"CornerRadius=$Radius"}},
    ThemeTargetStyles{L"StackPanel#PrimaryAndSecondaryTextContainer", {
        L"VerticalAlignment=Bottom",
        L"Grid.Column=0"}},
    ThemeTargetStyles{L"StackPanel#PrimaryAndSecondaryTextContainer > TextBlock#TitleText", {
        L"TextAlignment=Center"}},
    ThemeTargetStyles{L"StackPanel#PrimaryAndSecondaryTextContainer > TextBlock#SubtitleText", {
        L"TextAlignment=Center"}},
    ThemeTargetStyles{L"ContentControl#TogglesGroup > ContentPresenter > ControlCenter.PaginatedGridView > Grid", {
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"Grid#FooterGrid", {
        L"BorderThickness=0,0,0,0"}},
}, {
    L"Base=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#80000000\"/>",
    L"Radius=14",
    L"Transparent=<SolidColorBrush Color=\"Transparent\"/>",
    L"Accent=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity = \"1\" />",
    L"Overlay=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#1AFFFFFF\"/>",
    L"thumbnailImageSize=300",
}};

const Theme g_themeFluid = {{
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius",
        L"VerticalAlignment=2"}},
    ThemeTargetStyles{L"Grid#ControlCenterRegion", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Button#ClearAll", {
        L"AccessKey=x"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#DoNotDisturbButton", {
        L"AccessKey=d"}},
    ThemeTargetStyles{L"Button#ExpandCollapseButton", {
        L"AccessKey=e"}},
    ThemeTargetStyles{L"Border#ItemOpaquePlating", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Border#PopupBorder", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Border#ToastBackgroundBorder2", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Border#CalendarHeaderMinimizedOverlay", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"CalendarViewDayItem", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"CalendarViewDayItem > Border", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarPanel#YearViewPanel > Control", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarPanel#YearViewPanel > Control > Border", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarPanel#DecadeViewPanel > Control", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarPanel#DecadeViewPanel > Control > Border", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Grid > Microsoft.UI.Xaml.Controls.AnimatedIcon", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"ContentPresenter > Grid#FullScreenPageRoot", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Grid > ScrollViewer#ListContent", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRegion", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Border#JumpListRestyledAcrylic", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=1",
        L"CornerRadius=$CornerRadius"}},
}, {
    L"BorderBrush=<LinearGradientBrush x:Key=\"ShellTaskbarItemGradientStrokeColorSecondaryBrush\" MappingMode=\"Absolute\" StartPoint=\"0,0\" EndPoint=\"0,3\"><LinearGradientBrush.GradientStops><GradientStop Offset=\"0.33\" Color=\"{ThemeResource ControlFillColorSecondary}\" /><GradientStop Offset=\"1\" Color=\"{ThemeResource ControlFillColorTertiary}\" /></LinearGradientBrush.GradientStops></LinearGradientBrush>",
    L"BorderThickness=1",
    L"CornerRadius=4",
}};

const Theme g_themeLiquidGlass = {{
    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {
        L"Background:=$Background",
        L"CornerRadius = $CornerRadius",
        L"Shadow :=",
        L"BorderThickness = $BorderThickness",
        L"BorderBrush := $BorderBrush"}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"Background:=$Background",
        L"CornerRadius=$CornerRadius",
        L"Shadow :=",
        L"Margin = 0,6,0,6",
        L"MinHeight = 40",
        L"BorderThickness = $BorderThickness",
        L"BorderBrush := $BorderBrush"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer", {
        L"Background := $ElementBackground",
        L"CornerRadius = $ElementCornerRadius",
        L"Margin = -10,11,-10,-14",
        L"Shadow :=",
        L"BorderThickness = $ElementBorderThickness",
        L"BorderBrush := $ElementBorderBrush"}},
    ThemeTargetStyles{L"Border#CalendarHeaderMinimizedOverlay", {
        L"Background := $ElementBackground",
        L"CornerRadius = $CornerRadius",
        L"Shadow :=",
        L"Margin =-10,-6,-10,-8",
        L"Height = 45",
        L"BorderThickness = $ElementBorderThickness",
        L"BorderBrush := $ElementBorderBrush"}},
    ThemeTargetStyles{L"ActionCenter.FocusSessionControl#FocusSessionControl > Grid#FocusGrid", {
        L"Background := $ElementBackground",
        L"CornerRadius=$CornerRadius",
        L"Margin = 6,7,6,6",
        L"Shadow :=",
        L"BorderThickness = $ElementBorderThickness",
        L"BorderBrush := $ElementBorderBrush"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background := $Background",
        L"BorderThickness = 0,0,0,0",
        L"CornerRadius = $CornerRadius",
        L"Padding = 1,2,1,2",
        L"Shadow :=",
        L"BorderThickness = $BorderThickness",
        L"BorderBrush := $BorderBrush"}},
    ThemeTargetStyles{L"Border#JumpListRestyledAcrylic", {
        L"Background := $Background",
        L"BorderThickness = 0,0,0,0",
        L"CornerRadius = $CornerRadius",
        L"Margin = -2,-2,-2,-2",
        L"Shadow :=",
        L"BorderThickness = $BorderThickness",
        L"BorderBrush := $BorderBrush"}},
    ThemeTargetStyles{L"Grid#ControlCenterRegion", {
        L"Background := $Background",
        L"CornerRadius = $CornerRadius",
        L"BorderThickness = 0,0,0,0",
        L"Shadow :=",
        L"Margin = 0,0,0,-6"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent", {
        L"Background := Transparent",
        L"Shadow :="}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"Background := $ElementBackground",
        L"CornerRadius=$ElementCornerRadius",
        L"Margin = 8,0,8,2",
        L"Shadow :="}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot", {
        L"Background := Transparent",
        L"Shadow :="}},
    ThemeTargetStyles{L"QuickActions.ControlCenter.AccessibleWindow#PageWindow > ContentPresenter > Grid#FullScreenPageRoot > ContentPresenter#PageHeader", {
        L"Background := $ElementBackground",
        L"CornerRadius = $ElementCornerRadius",
        L"Margin = 7,7,7,7",
        L"Shadow :="}},
    ThemeTargetStyles{L"ScrollViewer#ListContent", {
        L"Background := $ElementBackground",
        L"CornerRadius = $ElementCornerRadius",
        L"Margin = 8,0,8,0",
        L"Shadow :="}},
    ThemeTargetStyles{L"ActionCenter.FlexibleToastView#FlexibleNormalToastView", {
        L"Background := Transparent",
        L"Shadow :="}},
    ThemeTargetStyles{L"Border#ToastBackgroundBorder2", {
        L"Background :=$Background",
        L"BorderThickness = 0,0,0,0",
        L"CornerRadius = $CornerRadius",
        L"Shadow :="}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"Background := $ElementBackground",
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListViewItem > Grid#LayoutRoot > Border#BackgroundBorder", {
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView", {
        L"CornerRadius = $CornerRadius",
        L"Shadow :="}},
    ThemeTargetStyles{L"QuickActions.AccessibleToggleButton#ToggleButton", {
        L"CornerRadius = $ElementCornerRadius",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"QuickActions.AccessibleToggleButton#SplitL2Button", {
        L"CornerRadius = $ElementCornerRadius",
        L"Margin = 4,0,-4,0",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Grid#NotificationCenterTopBanner", {
        L"Background := $ElementBackground",
        L"CornerRadius = $ElementCornerRadius",
        L"Margin = 6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#L1Grid > Border", {
        L"Background := Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter", {
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#FooterButton[AutomationProperties.Name = Edit quick settings]", {
        L"Margin = 0,0,8,0",
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button[AutomationProperties.AutomationId = Microsoft.QuickAction.Battery]", {
        L"Margin = 2,0,0,0",
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#FooterButton[AutomationProperties.Name = All settings]", {
        L"Margin = 0,0,-1,0",
        L"CornerRadius = $ElementCornerRadius",
        L"Background := $Background"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button[AutomationProperties.AutomationId = Microsoft.QuickAction.Volume]", {
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#VolumeL2Button[AutomationProperties.Name = Select a sound output]", {
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalTrackRect", {
        L"Height = 10",
        L"Fill := $ElementBackground",
        L"RadiusY = 3",
        L"RadiusX = 3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalDecreaseRect", {
        L"Height =10",
        L"RadiusY = 3",
        L"RadiusX = 3",
        L"Margin = 0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb", {
        L"Visibility = 1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion", {
        L"Height = Auto",
        L"CornerRadius = $CornerRadius",
        L"BorderThickness = 0",
        L"Background := $Background",
        L"Shadow :=",
        L"Padding = 0,0,0,12",
        L"Margin = 0,0,0,12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ThumbnailImage", {
        L"Grid.Column = 2",
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer", {
        L"Grid.Column = 1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer > Windows.UI.Xaml.Controls.TextBlock#Title", {
        L"TextAlignment = Center",
        L"FontFamily = Tektur",
        L"FontSize = 18"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer > Windows.UI.Xaml.Controls.TextBlock#Subtitle", {
        L"TextAlignment = Center",
        L"FontFamily = Montserrat",
        L"Margin = 0,3,0,0",
        L"FontWeight= 600"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#MediaButtonsListView", {
        L"VerticalAlignment = Top",
        L"Height = 48",
        L"Margin = 0,12,0,-12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#PreviousButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal := $ElementBackground",
        L"Background@PointerOver := $AccentBackground",
        L"Background@Pressed := $ElementBackground2",
        L"Width = 40",
        L"Height = 30",
        L"CornerRadius = $ElementCornerRadius",
        L"Margin = 15,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PlayPauseButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal := $ElementBackground",
        L"Background@PointerOver := $AccentBackground",
        L"Background@Pressed := $ElementBackground2",
        L"Width = 40",
        L"Height = 40",
        L"CornerRadius = $ElementCornerRadius",
        L"Margin = -10,0,0,0",
        L"BorderBrush := $ElementBorderBrush",
        L"BorderThickness = $ElementBorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#NextButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal := $ElementBackground",
        L"Background@PointerOver := $AccentBackground",
        L"Background@Pressed := $ElementBackground2",
        L"Width = 40",
        L"Height = 30",
        L"CornerRadius = $ElementCornerRadius",
        L"Margin = -20,0,0,0",
        L"BorderBrush := $ElementBorderBrush",
        L"BorderThickness = $ElementBorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AppNameText", {
        L"FontFamily = Tektur",
        L"FontSize = 16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#IconImage", {
        L"Height = 20",
        L"Width = 20"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot", {
        L"Background := Transparent"}},
    ThemeTargetStyles{L"Grid#ToastPeekRegion", {
        L"Background =",
        L"RenderTransform := <TranslateTransform Y=\"-495\" X=\"395\" />",
        L"Grid.Column = 0",
        L"Grid.Row = 2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.CalendarViewDayItem > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius = $ElementCornerRadius",
        L"Margin = 1,2,1,2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.CalendarViewDayItem", {
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Control > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarViewItem", {
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewHeaderItem", {
        L"Margin = 50,6,50,2",
        L"CornerRadius = $ElementCornerRadius",
        L"Height = 35"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SettingsButton", {
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#DismissButton", {
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#CalendarHeader", {
        L"Margin = 6,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter", {
        L"Margin = 1,2,1,2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#WeekDayNames", {
        L"Background := $AccentBackground",
        L"CornerRadius = $ElementCornerRadius",
        L"Margin = 4,0,4,0",
        L"Padding = 0,-5,0,-3",
        L"BorderThickness = $ElementBorderThickness",
        L"BorderBrush := $ElementBorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewItem", {
        L"CornerRadius = $ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background := $AccentBackground",
        L"CornerRadius = $ElementCornerRadius",
        L"BorderThickness = $ElementBorderThickness",
        L"BorderBrush := $ElementBorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#ItemOpaquePlating", {
        L"Background := $ElementBackground2",
        L"CornerRadius = $ElementCornerRadius",
        L"BorderThickness = $ElementBorderThickness",
        L"BorderBrush := $ElementBorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#StandardHeroContainer", {
        L"Margin = 12,0,12,0",
        L"CornerRadius = 0",
        L"Height = 150"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SliderContainer", {
        L"Margin = 0-2,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#BackButton", {
        L"CornerRadius = $ElementCornerRadius",
        L"BorderBrush := $ElementBorderBrush",
        L"BorderThickness = $ElementBorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#OuterBorder", {
        L"RadiusX = 6",
        L"RadiusY = 6",
        L"Height = 18"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobOff", {
        L"RadiusY = 3",
        L"RadiusX = 3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SwitchKnobOn", {
        L"CornerRadius = 3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobBounds", {
        L"RadiusX = 6",
        L"RadiusY = 6",
        L"Height = 18"}},
    ThemeTargetStyles{L"ActionCenter.NotificationListViewItem", {
        L"Margin = 5,2,5,3",
        L"BorderThickness = $ElementBorderThickness",
        L"BorderBrush := $ElementBorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid[AutomationProperties.LocalizedLandmarkType = Footer]", {
        L"BorderThickness = $ElementBorderThickness",
        L"BorderBrush := $ElementBorderBrush"}},
    ThemeTargetStyles{L"NetworkUX.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root", {
        L"CornerRadius = $CornerRadius",
        L"BorderThickness = $ElementBorderThickness",
        L"BorderBrush := $ElementBorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Border", {
        L"BorderThickness = $ElementBorderThickness",
        L"BorderBrush := $ElementBorderBrush"}},
    ThemeTargetStyles{L"Button#ClearAll", {
        L"AccessKey=x"}},
    ThemeTargetStyles{L"Button#ExpandCollapseButton", {
        L"AccessKey=e"}},
}, {
    L"transparent = Transparent",
    L"Background = <WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.4\"  />",
    L"ElementBackground = <WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.4\"  />",
    L"AccentBackground =<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemAccentColorLight1}\" TintOpacity=\"0.2\"  />",
    L"ElementBackground2 = <WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\"  />",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"BorderThickness=0.3,1,0.3,0.3",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"CornerRadius = 12",
    L"ElementCornerRadius = 8",
}};

const Theme g_themeBetterControl11 = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"MaximumRowsOrColumns=2"}},
    ThemeTargetStyles{L"ContentControl#TogglesGroup > ContentPresenter > ControlCenter.PaginatedGridView > Grid > GridView#RootGridView", {
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"ContentControl#TogglesGroup > ContentPresenter > ControlCenter.PaginatedGridView > Grid > GridView#RootGridView", {
        L"Height=375"}},
    ThemeTargetStyles{L"ContentControl#SlidersGroup", {
        L"Grid.Row=0",
        L"RenderTransform:=<TransformGroup><RotateTransform Angle=\"-90\" /><TranslateTransform X=\"123\" Y=\"-94\" /></TransformGroup>",
        L"RenderTransformOrigin=0.5,0.5",
        L"Margin=0,0,30,0",
        L"VerticalAlignment=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentControl#QuickActionContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Primitives.ToggleButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Microsoft.UI.Xaml.Controls.AnimatedIcon", {
        L"RenderTransform:=<RotateTransform Angle=\"90\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#BrightnessPlayer", {
        L"RenderTransform:=<RotateTransform Angle=\"90\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentControl#QuickActionContentControl", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />",
        L"Margin=-15,0,0,0"}},
    ThemeTargetStyles{L"ControlCenter.AsyncSlider", {
        L"Margin=35,0,50,0"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.Grid#FooterGrid > Windows.UI.Xaml.Controls.ItemsControl#LeftFooter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"RenderTransform:=<TranslateTransform X=\"216\" Y=\"-343\" />",
        L"Margin=0",
        L"Width=121"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager#QuickActionsPager", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.Grid#FooterGrid > Windows.UI.Xaml.Controls.ItemsControl#RightFooter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Button#FooterButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"-383\" />",
        L"Width=121"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#FooterGrid", {
        L"Margin=0,0,0,-48"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Grid", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#L1Grid > Border", {
        L"Visibility=0",
        L"Height=272",
        L"CornerRadius=8",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1",
        L"Margin=228,0,10,-4",
        L"RenderTransform:=<TransformGroup><RotateTransform Angle=\"0\" /><TranslateTransform X=\"0\" Y=\"48\" /></TransformGroup>",
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRoot", {
        L"Background:=transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#MediaButtonsListView", {
        L"Background:=#09FFFFFF",
        L"Margin=0,-6,0,6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AlbumTextAndArtContainer", {
        L"Padding=12,10,10,10",
        L"Background:=#09FFFFFF",
        L"CornerRadius=6",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1",
        L"Margin=-12,-32,-12,32"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PrimaryAndSecondaryTextContainer", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"10\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRoot > Grid > Windows.UI.Xaml.Controls.Image#IconImage", {
        L"RenderTransform:=<TranslateTransform X=\"-2\" Y=\"5\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRoot > Grid > Windows.UI.Xaml.Controls.TextBlock#AppNameText", {
        L"RenderTransform:=<TranslateTransform X=\"-2\" Y=\"5\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#MediaButtonsListView > Windows.UI.Xaml.Controls.ItemsPresenter", {
        L"Margin=0,-14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter#PageContent > Grid > Border", {
        L"Background:=transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter#PageHeader", {
        L"Background:=transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PlayPauseButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Margin=-22,0",
        L"Background:=#09FFFFFF",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#NextButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Margin=4,0",
        L"Background:=#09FFFFFF",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#PreviousButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Margin=4,0",
        L"Background:=#09FFFFFF",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ThumbnailImage", {
        L"CornerRadius=4",
        L"Height=64",
        L"Width=64",
        L"Margin=-4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentControl#QuickActionContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Primitives.ToggleButton", {
        L"RenderTransform:=<TransformGroup><RotateTransform Angle=\"0\" /><TranslateTransform X=\"25\" Y=\"0\" /></TransformGroup>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentControl#QuickActionContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button", {
        L"RenderTransform:=<TransformGroup><RotateTransform Angle=\"0\" /><TranslateTransform X=\"25\" Y=\"0\" /></TransformGroup>"}},
    ThemeTargetStyles{L"Button#VolumeL2Button", {
        L"RenderTransform:=<TransformGroup><RotateTransform Angle=\"90\" /><TranslateTransform X=\"0\" Y=\"0\" /></TransformGroup>"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"Margin=0,0,0,-36",
        L"Height=80"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"Margin=0,0,0,-36",
        L"Height=80"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#SplitL2Button", {
        L"Margin=0,0,0,-36",
        L"Height=80"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#SplitL2Button > ContentPresenter", {
        L"BorderThickness=0,1,1,1"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > ControlCenter.PaginatedToggleButton#ToggleButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid#SplitToggleContent", {
        L"Margin=0,0,0,20"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#SplitL2Button > ContentPresenter > FontIcon", {
        L"Margin=0,0,0,20"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > ControlCenter.PaginatedToggleButton#ToggleButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid#ToggleButtonContent > Microsoft.UI.Xaml.Controls.AnimatedIcon", {
        L"Margin=0,0,0,20"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > ControlCenter.PaginatedToggleButton#ToggleButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid > Microsoft.UI.Xaml.Controls.AnimatedIcon", {
        L"Margin=-2,0,2,20"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > ControlCenter.PaginatedToggleButton#ToggleButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.FontIcon", {
        L"Margin=2,0,-2,20"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem[1] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > ControlCenter.PaginatedToggleButton#ToggleButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"BorderThickness=1,1,0,1"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem[2] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > ControlCenter.PaginatedToggleButton#ToggleButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"BorderThickness=1,1,0,1"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"Margin=-6,0,6,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Internal.RootScrollViewer > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.Border > ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.Grid#FooterGrid > Windows.UI.Xaml.Controls.ItemsControl#LeftFooter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Margin=0,-16,0,-2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#FooterGrid > Windows.UI.Xaml.Controls.ItemsControl#LeftFooter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background:=$Overlay1",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1,0,1,1",
        L"CornerRadius=0,0,4,4",
        L"Margin=-1,0,-80,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#FooterGrid > Windows.UI.Xaml.Controls.ItemsControl#RightFooter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Button#FooterButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background:=$Overlay1",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1,1,1,0",
        L"CornerRadius=4,4,0,0",
        L"Margin=0"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid", {
        L"Margin=0,-6,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#FooterGrid > Windows.UI.Xaml.Controls.ItemsControl#RightFooter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Button#FooterButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Microsoft.UI.Xaml.Controls.AnimatedIcon#FooterButtonIcon", {
        L"Margin=0,3,0,-3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#FooterGrid > Windows.UI.Xaml.Controls.ItemsControl#LeftFooter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel", {
        L"Margin=0,-3,0,3"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter#Content > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#TitleText", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"0\" TranslateY=\"0\" TranslateZ=\"-99\" />",
        L"Visibility=0"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter#Content > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#StatusText", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"0\" TranslateY=\"0\" TranslateZ=\"-99\" />",
        L"Visibility=1"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#ControlCenterRegion > ControlCenter.ControlCenterView#ControlCenterView > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.ContentControl#TogglesGroup > Windows.UI.Xaml.Controls.ContentPresenter > ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter#Content > Windows.UI.Xaml.Controls.StackPanel", {
        L"Canvas.ZIndex=-1",
        L"Height=0",
        L"Padding=-48,-8",
        L"IsHitTestVisible=False",
        L"Width=0",
        L"Margin=0,16,0,-16"}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"Margin=0,0,0,14",
        L"CornerRadius=0,0,8,8",
        L"BorderThickness=1,0,1,1"}},
    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {
        L"VerticalAlignment=2",
        L"Margin=0",
        L"CornerRadius=8,8,0,0",
        L"BorderThickness=1,1,1,0",
        L"Padding=0,0,0,2"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRegion", {
        L"Height=164",
        L"Margin=0,0,0,-12",
        L"CornerRadius=8,8,0,0",
        L"BorderThickness=1,1,1,0",
        L"Padding=0,0,0,24",
        L"Shadow:="}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid", {
        L"Padding=0,6,0,0"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > ControlCenter.MediaTransportControls#MediaTransportControls > Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion > Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRoot > Windows.UI.Xaml.Controls.ListView#MediaButtonsListView > Windows.UI.Xaml.Controls.ItemsPresenter", {
        L"Margin=-74,6,74,-6",
        L"Padding=-40"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > ControlCenter.MediaTransportControls#MediaTransportControls > Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion > Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRoot > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button#SessionSwitchButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1",
        L"Height=40",
        L"Width=40"}},
    ThemeTargetStyles{L"ControlCenter.ControlCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > ControlCenter.MediaTransportControls#MediaTransportControls > Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRegion > Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRoot > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button#SessionSwitchButton", {
        L"RenderTransform:=<TranslateTransform X=\"4\" Y=\"98\" />",
        L"Visibility=0"}},
    ThemeTargetStyles{L"Grid#ControlCenterRegion", {
        L"Canvas.ZIndex=-99"}},
    ThemeTargetStyles{L"ActionCenter.FocusSessionControl", {
        L"Visibility=0"}},
    ThemeTargetStyles{L"ActionCenter.NotificationCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid > ActionCenter.ClockCalendarView#ClockCalendarView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#CalendarSection > Windows.UI.Xaml.Controls.ScrollViewer#CalendarControlScrollViewer", {
        L"Visibility=0",
        L"Margin=-10,-73,-10,-6",
        L"Canvas.ZIndex=-99",
        L"Padding=0,16,0,2",
        L"CornerRadius=6",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"ActionCenter.NotificationCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid > ActionCenter.ClockCalendarView#ClockCalendarView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#CalendarSection > Windows.UI.Xaml.Controls.StackPanel#CalendarHeader", {
        L"Margin=0,-4,0,4"}},
    ThemeTargetStyles{L"ActionCenter.NotificationCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid > ActionCenter.ClockCalendarView#ClockCalendarView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#CalendarSection > Windows.UI.Xaml.Controls.ScrollViewer#CalendarControlScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.CalendarView#CalendarControl > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button#HeaderButton", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"ActionCenter.NotificationCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid > ActionCenter.ClockCalendarView#ClockCalendarView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#CalendarSection > Windows.UI.Xaml.Controls.ScrollViewer#CalendarControlScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.CalendarView#CalendarControl > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button#PreviousButton", {
        L"Margin=-40,8,46,5",
        L"Opacity=0",
        L"Width=0"}},
    ThemeTargetStyles{L"ActionCenter.NotificationCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid > ActionCenter.ClockCalendarView#ClockCalendarView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#CalendarSection > Windows.UI.Xaml.Controls.ScrollViewer#CalendarControlScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.CalendarView#CalendarControl > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button#NextButton", {
        L"Margin=-40,8,50,5",
        L"Opacity=0",
        L"Width=0"}},
    ThemeTargetStyles{L"ActionCenter.NotificationCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid > ActionCenter.ClockCalendarView#ClockCalendarView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#CalendarSection > Windows.UI.Xaml.Controls.ScrollViewer#CalendarControlScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.CalendarView#CalendarControl > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid", {
        L"Margin=0"}},
    ThemeTargetStyles{L"ActionCenter.NotificationCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid > ActionCenter.ClockCalendarView#ClockCalendarView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#CalendarSection > Windows.UI.Xaml.Controls.TextBlock#ClockWithMeridianOld", {
        L"Margin=0,0,100,0"}},
    ThemeTargetStyles{L"ActionCenter.NotificationCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid > ActionCenter.ClockCalendarView#ClockCalendarView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#CalendarSection > Windows.UI.Xaml.Controls.Border#CalendarHeaderMinimizedOverlay", {
        L"CornerRadius=6",
        L"Margin=-10,-10,-10,-6",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"ActionCenter.NotificationCenterPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid > ActionCenter.ClockCalendarView#ClockCalendarView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#CalendarSection > ActionCenter.FocusSessionControl#FocusSessionControl > Windows.UI.Xaml.Controls.Grid#FocusGrid", {
        L"CornerRadius=6",
        L"Margin=6,2,6,6",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1",
        L"Padding=8"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Grid#ItemGrid > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#ItemOpaquePlating", {
        L"CornerRadius=6",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\"0.8\" />",
        L"BorderThickness=1",
        L"Margin=6,4"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Grid#ItemGrid > Windows.UI.Xaml.Controls.Grid#StandardHeroContainer > Windows.UI.Xaml.Controls.Image", {
        L"Margin=7,0,7,5"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Grid#ItemGrid > Windows.UI.Xaml.Controls.Grid#StandardHeroContainer", {
        L"CornerRadius=0,0,27,27",
        L"Margin=0",
        L"Padding=0"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"Height=80"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter#Content > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#TitleText", {
        L"Margin=0,-36,0,36",
        L"Visibility=0"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter#Content > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#StatusText", {
        L"Margin=0,-36,0,36",
        L"Visibility=1"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter#Content > Windows.UI.Xaml.Controls.StackPanel", {
        L"IsHitTestVisible=False"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedGridView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > ControlCenter.PaginatedToggleButton#ToggleButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid#ToggleButtonContent > Microsoft.UI.Xaml.Controls.AnimatedIcon", {
        L"Margin=0,-10,0,10"}},
    ThemeTargetStyles{L"ContentControl#SlidersGroup > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid", {
        L"Background:=$Overlay1",
        L"BorderBrush:=$OverlayBorder1",
        L"BorderThickness=1",
        L"CornerRadius=4",
        L"Height=120",
        L"Margin=7,0,20,10",
        L"RenderTransform:=<TranslateTransform X=\"-9\" Y=\"-2\" />"}},
    ThemeTargetStyles{L"ContentControl#SlidersGroup > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.GridView#RootGridView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter", {
        L"Padding=0,9,-20,0"}},
}, {
    L"Overlay1=<SolidColorBrush Color=\"{ThemeResource NativeOverlay}\" />",
    L"OverlayBorder1=<SolidColorBrush Color=\"{ThemeResource NativeOverlayBorder}\" />",
}, {
    L"NativeOverlay@Dark=#09FFFFFF",
    L"NativeOverlay@Light=#89FFFFFF",
    L"NativeOverlayBorder@Dark={ThemeResource SurfaceStrokeColorDefault}",
    L"NativeOverlayBorder@Light=#14000000",
}};

const Theme g_themeLayerMicaUI = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#BrightnessPlayer", {
        L"Height=22",
        L"Width=22"}},
    ThemeTargetStyles{L"Grid > Microsoft.UI.Xaml.Controls.AnimatedIcon", {
        L"Width=18",
        L"Height=18"}},
    ThemeTargetStyles{L"CalendarViewDayItem > Border", {
        L"CornerRadius=$InnerRadius",
        L"BorderThickness=2"}},
    ThemeTargetStyles{L"Control > Border", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Grid#JumpListGrid > Border", {
        L"Background:=$ThemeLayer",
        L"BorderBrush:=$ThemeOutBorder",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Border#ToastBackgroundBorder2", {
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeOutBorder"}},
    ThemeTargetStyles{L"Border#CalendarHeaderMinimizedOverlay", {
        L"Margin=-11,-7,-11,-10",
        L"CornerRadius=$InnerRadius",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeBorder"}},
    ThemeTargetStyles{L"Grid#L1Grid > Border", {
        L"Margin=6,0,6,0",
        L"CornerRadius=$InnerRadius",
        L"BorderBrush:=$ThemeBorder",
        L"BorderThickness=1",
        L"Height=$ControlCenterLayer",
        L"VerticalAlignment=Bottom",
        L"Background:=$ThemeOverlay"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"BorderBrush=Transparent",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"StackPanel > ContentPresenter > Border", {
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Border#WADFeatureFooter", {
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Border#JumpListRestyledAcrylic", {
        L"Background:=$ThemeLayer",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeOutBorder"}},
    ThemeTargetStyles{L"JumpViewUI.TaskbarJumpListFrame > Grid#JumpListGrid > Grid#SystemItemsContainer > Border", {
        L"Background:=$ThemeOverlay",
        L"CornerRadius=$InnerRadius",
        L"Margin=2",
        L"BorderBrush:=$ThemeBorder",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemListViewItem > Grid#LayoutRoot@CommonStates > Border#BackgroundBorder", {
        L"Width=220",
        L"Margin=-30,0,27,0",
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"ListView#MediaButtonsListView > ItemsPresenter > StackPanel > ListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Border", {
        L"Background:=$ThemeBtn",
        L"CornerRadius=$InnerRadius",
        L"BorderBrush:=$ThemeControlBorder",
        L"BorderThickness=1",
        L"Margin=0"}},
    ThemeTargetStyles{L"ListView#MediaButtonsListView > ItemsPresenter > StackPanel > ListViewItem[2] > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Border", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource Accent1}\" />",
        L"CornerRadius=$InnerRadius",
        L"BorderBrush:=$ThemeControlBorder",
        L"BorderThickness=1",
        L"Margin=25,0,25,0"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer > Border#Root", {
        L"BorderBrush:=$ThemeBorder",
        L"BorderThickness=1",
        L"CornerRadius=$InnerRadius",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView > Grid#MainGrid > Grid#ItemGrid > Grid > Border#ItemOpaquePlating", {
        L"Margin=6,-1,6,6",
        L"CornerRadius=3,3,$InnerRadius,$InnerRadius",
        L"BorderThickness=1,0,1,1",
        L"BorderBrush:=$ThemeBorder",
        L"Background:=$ThemeOverlay"}},
    ThemeTargetStyles{L"CalendarView#CalendarControl > Border > Grid > Border", {
        L"BorderBrush:=$ThemeBorder",
        L"Height=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Border[1]", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Button#SettingsButton", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Button#DismissButton", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Button#BackButton", {
        L"CornerRadius=$InnerRadius",
        L"Margin=1,0,10,0"}},
    ThemeTargetStyles{L"Grid#DoNotDisturbSubtext > Button", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Button#FooterButton[AutomationProperties.Name = Edit quick settings]", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Button#FooterButton[AutomationProperties.Name = All settings]", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Button[AutomationProperties.AutomationId = Microsoft.QuickAction.Battery]", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Button[CornerRadius=6]", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"CalendarView#CalendarControl > Border > Grid > Grid > Button", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Button#VerbButton", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"CalendarViewDayItem", {
        L"CornerRadius=$InnerRadius",
        L"Margin=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarViewItem", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"ContentControl#PageHeaderContentControl", {
        L"Width=64"}},
    ThemeTargetStyles{L"ContentPresenter#PageHeader", {
        L"Margin=5",
        L"CornerRadius=$InnerRadius",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeBorder",
        L"Background:=$ThemeOverlay",
        L"Padding=0,-2,0,2",
        L"Height=45"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#ToggleButton > ContentPresenter#ContentPresenter", {
        L"CornerRadius=$InnerRadius",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeControlBorder"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#SplitL2Button > ContentPresenter#ContentPresenter", {
        L"CornerRadius=$InnerRadius",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeControlBorder",
        L"Margin=5,0,0,0"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot > ControlCenter.MediaTransportControlsButton#MediaTransportControlsButton > ContentPresenter", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Button#DateTextButtonWithClock > Grid > Border#Border > ContentPresenter#ContentPresenter", {
        L"FontFamily=$ThFnt",
        L"FontSize=14"}},
    ThemeTargetStyles{L"Button#BackButton > ContentPresenter#ContentPresenter", {
        L"Content:=<FontIcon FontSize=\"20\" FontFamily=\"Segoe Fluent Icons\" Glyph=\"&#xE973;\" />",
        L"Padding=-2,0,0,0"}},
    ThemeTargetStyles{L"ListView#MediaButtonsListView > ItemsPresenter > StackPanel > ListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Button > ContentPresenter#ContentPresenter", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"ListView#MediaButtonsListView > ItemsPresenter > StackPanel > ListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Windows.UI.Xaml.Controls.Primitives.RepeatButton > ContentPresenter#ContentPresenter", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Control", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarPanel#YearViewPanel > Control", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Grid#NotificationCenterGrid", {
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRegion", {
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"Grid#ControlCenterRegion", {
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"Grid#JumpListGrid", {
        L"CornerRadius=$OuterRadius",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeOutBorder",
        L"Width=250"}},
    ThemeTargetStyles{L"Grid#WeekDayNames", {
        L"Margin=2,-5,2,-8"}},
    ThemeTargetStyles{L"Grid#FocusGrid", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Grid#DoNotDisturbSubtext", {
        L"Margin=0,-53,0,0",
        L"Canvas.ZIndex=-1",
        L"BorderThickness=0,0,0,1",
        L"BorderBrush:=$ThemeControlBorder",
        L"Background:=$ThemeOverlay",
        L"Padding=0,63,0,6",
        L"CornerRadius=$OuterRadius,$OuterRadius,12,12"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedGridView > Grid", {
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Grid#L1Grid > Grid", {
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot", {
        L"CornerRadius=$OuterRadius",
        L"BorderThickness=0",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#ThumbnailImage", {
        L"Height=105",
        L"Width=105",
        L"Margin=0,0,-8,0",
        L"CornerRadius=$InnerRadius",
        L"BorderThickness=0",
        L"Grid.Column=2"}},
    ThemeTargetStyles{L"Grid#NotificationCenterTopBanner", {
        L"Margin=5,5,5,0",
        L"CornerRadius=$InnerRadius",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeBorder",
        L"Background:=$ThemeOverlay",
        L"Padding=10,-2,8,2",
        L"Height=38"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer > Border#Root > Grid", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"ActionCenter.FlexibleItemView > Grid#MainGrid > Grid#ItemGrid > Grid#StandardHeroContainer", {
        L"Margin=5,-3,5,0",
        L"Padding=8,0,8,0",
        L"MaxHeight=160"}},
    ThemeTargetStyles{L"ActionCenter.GroupView > Grid#GroupGrid", {
        L"Margin=6,5,6,-1",
        L"Background:=$ThemeOverlay",
        L"Padding=0,2,0,0",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeBorder",
        L"CornerRadius=$InnerRadius,$InnerRadius,3,3"}},
    ThemeTargetStyles{L"CalendarView#CalendarControl > Border > Grid > Grid[1]", {
        L"Height=46",
        L"Margin=-4,-5,-4,0",
        L"Padding=0,-2,0,-2"}},
    ThemeTargetStyles{L"Grid#MediaTransportControlsRoot > Grid > Image#IconImage", {
        L"Opacity=0.8",
        L"Margin=-4,0,0,0"}},
    ThemeTargetStyles{L"ListView#MediaButtonsListView", {
        L"Margin=-62,-60,62,-20"}},
    ThemeTargetStyles{L"ListViewItem", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"NetworkUX.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root", {
        L"PointerOverBackground:=$ThemeHover",
        L"PressedBackground:=$ThemeBtn",
        L"SelectedBackground:=$ThemeHover",
        L"SelectedDisabledBackground:=$ThemeHover",
        L"SelectedPointerOverBackground:=$ThemeHover",
        L"SelectedPressedBackground:=$ThemeBtn",
        L"PlaceholderBackground:=$ThemeHover"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background:=$ThemeLayer",
        L"CornerRadius=$OuterRadius",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeOutBorder"}},
    ThemeTargetStyles{L"ActionCenter.NotificationContentView#NotificationContentView", {
        L"Margin=5,3,5,1"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#ToggleButton", {
        L"BorderBrush:=$ThemeControlBorder",
        L"BorderThickness=1",
        L"CornerRadius=$InnerRadius",
        L"FocusVisualPrimaryThickness=0",
        L"FocusVisualSecondaryThickness=0"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#SplitL2Button", {
        L"BorderBrush:=$ThemeControlBorder",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Rectangle#HorizontalTrackRect", {
        L"Height=6",
        L"RadiusX=3",
        L"RadiusY=3",
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Rectangle#HorizontalDecreaseRect", {
        L"Height=6",
        L"RadiusX=3",
        L"RadiusY=3"}},
    ThemeTargetStyles{L"ScrollViewer#CalendarControlScrollViewer", {
        L"Margin=-11,10,-11,-12",
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"ScrollViewer#ListContent", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"Grid > ScrollViewer#ListContent", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"StackPanel#CalendarHeader", {
        L"Margin=6,-5,0,0"}},
    ThemeTargetStyles{L"StackPanel#PrimaryAndSecondaryTextContainer", {
        L"VerticalAlignment=Top",
        L"Grid.Column=0",
        L"Margin=5,0,0,0"}},
    ThemeTargetStyles{L"Grid#DoNotDisturbSubtext > TextBlock[3]", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#DoNotDisturbSubtext > TextBlock[2]", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource TextFillColorSecondary}\" />",
        L"FontSize=15",
        L"HorizontalAlignment=Center",
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Grid#DoNotDisturbSubtext > TextBlock[1]", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"MenuFlyoutItem > Grid > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"StackPanel#PrimaryAndSecondaryTextContainer > TextBlock#Title", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"StackPanel#PrimaryAndSecondaryTextContainer > TextBlock#Subtitle", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"TextBlock#StatusText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=12.5"}},
    ThemeTargetStyles{L"TextBlock#TitleText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=12.5"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot > TextBlock", {
        L"FontWeight=$ThFntWt",
        L"FontFamily=$ThFnt",
        L"FontSize=13"}},
    ThemeTargetStyles{L"Grid#WeekDayNames > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt",
        L"FontSize=12"}},
    ThemeTargetStyles{L"CalendarViewDayItem > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Button#HeaderButton > ContentPresenter#Text > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt",
        L"FontSize=15"}},
    ThemeTargetStyles{L"TextBlock#ClockWithMeridian", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt"}},
    ThemeTargetStyles{L"TextBlock#DurationTextBlock", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"TextBlock#NotificationsTextBlock", {
        L"FontWeight=$ThHdnWt",
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"TextBlock#StartButtonText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#NoNotificationsTextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=13"}},
    ThemeTargetStyles{L"TextBlock[AutomationProperties.AutomationId=TextTopologyTileDescription]", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#PageTitleText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt",
        L"FontSize=15"}},
    ThemeTargetStyles{L"TextBlock#PageTitle", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt",
        L"FontSize=15"}},
    ThemeTargetStyles{L"Button[AutomationProperties.AutomationId=Microsoft.QuickAction.Battery] > ContentPresenter#ContentPresenter > StackPanel > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=12.5"}},
    ThemeTargetStyles{L"Grid#GroupTitleGrid > TextBlock#Title", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=13.5"}},
    ThemeTargetStyles{L"StackPanel#TextContentPanel > TextBlock", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"TextBlock#TimeStamp", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock[AutomationProperties.AutomationId=TextBluetoothDisabled]", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"TextBlock[AutomationProperties.AutomationId=HeaderBluetoothDisabled]", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"TextBlock[AutomationProperties.AutomationId=YourDevices]", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"NetworkUX.View.EntityListItemControl#EntityListItemControl > Grid > TextBlock", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"ActionCenter.ClearAllButton#ClearAllButtonControl > Button#ClearAll > ContentPresenter#ContentPresenter@CommonStates > TextBlock", {
        L"Foreground@PointerOver:=<SolidColorBrush Color=\"Red\" Opacity=\"0.7\"/>",
        L"Foreground@Pressed:=<SolidColorBrush Color=\"Red\" Opacity=\"0.7\"/>",
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"StackPanel#OffUxPanel > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"ContentControl#QuickActionContentControl > ContentPresenter > Grid > StackPanel > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"NetworkUX.View.EntityListItemControl#EntityListItemControl > Grid > StackPanel > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontFamily=Segoe Fluent Icons",
        L"FontSize=15"}},
    ThemeTargetStyles{L"Button#PlayPauseButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontFamily=Segoe Fluent Icons",
        L"FontSize=16",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource TextFillColorInverse}\"/>"}},
    ThemeTargetStyles{L"StackPanel#PrimaryAndSecondaryTextContainer > TextBlock#TitleText", {
        L"FontFamily=$ThFnt",
        L"FontSize=16.5",
        L"FontWeight=$ThHdnWt",
        L"Margin=0,0,8,0",
        L"Padding=0,0,0,10"}},
    ThemeTargetStyles{L"StackPanel#PrimaryAndSecondaryTextContainer > TextBlock#SubtitleText", {
        L"FontFamily=$ThFnt",
        L"FontSize=13",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource TextFillColorSecondary}\" />",
        L"Margin=0,0,0,-5"}},
    ThemeTargetStyles{L"TextBlock#AppNameText", {
        L"FontSize=12",
        L"FontWeight=$ThFntWt",
        L"FontFamily=$ThFnt",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource TextFillColorSecondary}\" />"}},
    ThemeTargetStyles{L"JumpViewUI.SystemItemControl > Grid > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListItemControl > Grid > Grid > TextBlock", {
        L"FontWeight=$ThFntWt",
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"TextBlock[AutomationProperties.AutomationId=TextDeviceListEmpty]", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock[AutomationProperties.AutomationId=AvailableDisplaysTextBlock]", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Grid#RootGrid > ContentPresenter#Content > TextBlock", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"TextBlock#MixerGroupTitle", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=15"}},
    ThemeTargetStyles{L"TextBlock#SpatialGroupTitle", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=15"}},
    ThemeTargetStyles{L"TextBlock#OutputGroupTitle", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=15"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > Grid > TextBlock#TitleText", {
        L"FontSize=13.5",
        L"Margin=0,1,0,-1"}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > ScrollViewer#ListContent > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=15"}},
    ThemeTargetStyles{L"Button#DisconnectButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"StackPanel > StackPanel > CheckBox > Grid#RootGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"StackPanel > StackPanel > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#ProjectInterfaceDeviceTileTextDeviceStatus", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"TextBlock#ProjectInterfaceDeviceTileTextDeviceName", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#SplitL2Button > ContentPresenter#ContentPresenter > FontIcon > Grid > TextBlock", {
        L"FontWeight=Normal",
        L"FontSize=17"}},
    ThemeTargetStyles{L"ControlCenter.PaginatedToggleButton#ToggleButton > ContentPresenter#ContentPresenter > Grid > FontIcon > Grid > TextBlock", {
        L"FontSize=16",
        L"Padding=6,0,0,0",
        L"Margin=0,0,-6,0"}},
    ThemeTargetStyles{L"Button#DateTextButtonWithClockOld > Grid > Border#Border > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontFamily=$ThFnt",
        L"Margin=-3,0,0,0"}},
    ThemeTargetStyles{L"TextBlock#ClockWithMeridianOld", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"Button#VolumeL2Button > ContentPresenter#ContentPresenter > StackPanel > FontIcon[2] > Grid > TextBlock", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Button#VolumeL2Button > ContentPresenter#ContentPresenter > StackPanel > FontIcon[1] > Grid > TextBlock", {
        L"Text=\uE9E9"}},
    ThemeTargetStyles{L"TextBlock#StopButtonText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"StackPanel#MainContent > TextBlock#Title2", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt"}},
    ThemeTargetStyles{L"StackPanel#MainContent > TextBlock#MessageText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#Attribution", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#SubgroupTitleText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#SenderName", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#VerbText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"StackPanel#MainContent > TextBlock#MessageText2", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"StackPanel#TextContentPanel > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Button#DismissButton > Grid@CommonStates > Border#Border > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground@PointerOver:=<SolidColorBrush Color=\"Red\" Opacity=\"0.7\"/>",
        L"FontWeight=$ThHdnWt",
        L"Foreground@Pressed:=<SolidColorBrush Color=\"Red\" Opacity=\"0.7\"/>"}},
    ThemeTargetStyles{L"Button#SettingsButton > Grid@CommonStates > Border#Border > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground@PointerOver:=<SolidColorBrush Color=\"{ThemeResource Accent1}\" />",
        L"FontWeight=$ThHdnWt",
        L"Foreground@Pressed:=<SolidColorBrush Color=\"{ThemeResource Accent1}\" />"}},
    ThemeTargetStyles{L"Grid#FocusControlGrid > Button#StopButton > ContentPresenter#ContentPresenter@CommonStates > StackPanel > FontIcon > Grid > TextBlock", {
        L"Foreground@PointerOver:=<SolidColorBrush Color=\"Red\" Opacity=\"0.7\"/>",
        L"Foreground@Pressed:=<SolidColorBrush Color=\"Red\" Opacity=\"0.7\"/>"}},
    ThemeTargetStyles{L"Grid#FocusControlGrid > Button#StartButton > ContentPresenter#ContentPresenter@CommonStates > StackPanel > FontIcon > Grid > TextBlock", {
        L"Foreground@PointerOver:=<SolidColorBrush Color=\"{ThemeResource Accent1}\" />",
        L"Foreground@Pressed:=<SolidColorBrush Color=\"{ThemeResource Accent1}\" />"}},
    ThemeTargetStyles{L"Grid#FocusStateGrid > Grid#FocusTimeSelector > Button#IncreaseTimeButton > ContentPresenter#ContentPresenter@CommonStates > FontIcon > Grid > TextBlock", {
        L"Foreground@PointerOver:=<SolidColorBrush Color=\"Green\" Opacity=\"0.7\"/>",
        L"Foreground@Pressed:=<SolidColorBrush Color=\"Green\" Opacity=\"0.7\"/>"}},
    ThemeTargetStyles{L"Grid#FocusStateGrid > Grid#FocusTimeSelector > Button#DecreaseTimeButton > ContentPresenter#ContentPresenter@CommonStates > FontIcon > Grid > TextBlock", {
        L"Foreground@PointerOver:=<SolidColorBrush Color=\"Red\" Opacity=\"0.7\"/>",
        L"Foreground@Pressed:=<SolidColorBrush Color=\"Red\" Opacity=\"0.7\"/>"}},
    ThemeTargetStyles{L"TextBlock#FocusingText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.CalendarPanel#YearViewPanel > Windows.UI.Xaml.Controls.Primitives.CalendarViewItem > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"NetworkUX.View.CFEWiFiPassKey > StackPanel > TextBlock#WiFiPassKeyLabel", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"NetworkUX.View.CFEConnectionCompletion > StackPanel > TextBlock#ErrorMessageLabel", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"NetworkUX.View.CFEConnectionCompletion > StackPanel > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"ToolTip", {
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"NetworkUX.View.CFEWiFiPassKey > StackPanel > PasswordBox#PassKeyPasswordBox", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"StackPanel > Button", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"TextBlock[AutomationProperties.AutomationId=NewDevices]", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
}, {
    L"ThemeLayer=<AcrylicBrush BackgroundSource=\"Backdrop\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.1\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
    L"OuterRadius=10",
    L"InnerRadius=8",
    L"ThemeBorder=<SolidColorBrush Color=\"{ThemeResource Border}\" />",
    L"ThemeControlBorder=<SolidColorBrush Color=\"{ThemeResource ControlBorder}\" />",
    L"ThemeOverlay=<SolidColorBrush Color=\"{ThemeResource Overlay}\" />",
    L"ThFnt=Nunito",
    L"ThFntWt=Normal",
    L"ThHdnWt=Semibold",
    L"ThemeOutBorder=<SolidColorBrush Color=\"#66757575\"/>",
    L"ThemeBtn=<SolidColorBrush Color=\"{ThemeResource Btn}\" />",
    L"ControlCenterLayer=120",
    L"ThemeHover=<SolidColorBrush Color=\"{ThemeResource Hover}\" />",
    L"OneSlider=67",
    L"TwoSlider=120",
    L"ThreeSlider=177",
}, {
    L"Overlay@Light=#55FFFFFF",
    L"Overlay@Dark=#09FFFFFF",
    L"Border@Light=#0F000000",
    L"Border@Dark=#19000000",
    L"ControlBorder@Light=#0F000000",
    L"ControlBorder@Dark=#15FFFFFF",
    L"Btn@Light=#90FFFFFF",
    L"Btn@Dark=#20FFFFFF",
    L"Accent1@Dark={ThemeResource SystemAccentColorLight2}",
    L"Accent1@Light={ThemeResource SystemAccentColorDark1}",
    L"Hover@Light=#65FFFFFF",
    L"Hover@Dark=#09FFFFFF",
}};

const Theme g_themeBorderless = {{
    ThemeTargetStyles{L"ActionCenter.FocusSessionControl", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid", {
        L"Shadow:=",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border", {
        L"Shadow:=",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NotificationCenterGrid", {
        L"Height=Auto",
        L"VerticalAlignment=Stretch",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#NoNotificationsTextBlock", {
        L"Text=\u00AF\\_(\u30C4)_/\u00AF",
        L"FontSize=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ExpandCollapseButton", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#DoNotDisturbSubtext > Windows.UI.Xaml.Controls.Button", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ClearAll > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE653",
        L"FontFamily=Segoe Fluent Icons",
        L"FontSize=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock", {
        L"FontWeight=Normal"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#CalendarSection", {
        L"Height=300"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#PreviousButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#NextButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#HeaderButton", {
        L"Width=Auto",
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#WeekDayNames", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ExpandCollapseButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#CalendarCenterGrid", {
        L"Margin=0,10,0,160"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ClearAll", {
        L"Width=24",
        L"Height=24"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#DoNotDisturbButton", {
        L"Height=24",
        L"Width=24"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#DoNotDisturbButtonIcon", {
        L"Height=12",
        L"Width=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#CalendarHeaderMinimizedOverlay", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"ActionCenter.NotificationCenterView#NotificationCenterView", {
        L"Margin=16,0,16,20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#L1Grid > Windows.UI.Xaml.Controls.Border", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControlsRoot", {
        L"Background:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ScrollViewer#CalendarControlScrollViewer", {
        L"Background:="}},
    ThemeTargetStyles{L"ContentPresenter#PageContent > Grid > Border", {
        L"Background:="}},
    ThemeTargetStyles{L"ContentPresenter#PageHeader", {
        L"Background:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ToastCenterMainGrid", {
        L"MaxWidth=348"}},
    ThemeTargetStyles{L"ScrollViewer#ListContent", {
        L"Background:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#SharePickerHeader > Windows.UI.Xaml.Shapes.Rectangle", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#PreviewComponentPanel", {
        L"Background:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ItemOpaquePlating", {
        L"Opacity=0.7"}},
    ThemeTargetStyles{L"ActionCenter.ToastCenterView", {
        L"MaxWidth=348"}},
}};

// clang-format on

std::atomic<bool> g_initialized;
thread_local bool g_initializedForThread;

void ApplyCustomizations(InstanceHandle handle,
                         winrt::Windows::UI::Xaml::FrameworkElement element,
                         PCWSTR fallbackClassName);
void CleanupCustomizations(InstanceHandle handle);

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
#include <winrt/base.h>

// forward declare namespaces we alias
namespace winrt {
    namespace Windows {
        namespace Foundation {}
        namespace UI::Xaml {}
    }
}

// alias some long namespaces for convenience
namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;

#pragma endregion  // winrt_hpp

#pragma region visualtreewatcher_hpp

#include <winrt/Windows.UI.Xaml.h>

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
};

#pragma endregion  // visualtreewatcher_hpp

#pragma region visualtreewatcher_cpp

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site) :
    m_XamlDiagnostics(site.as<IXamlDiagnostics>())
{
    Wh_Log(L"Constructing VisualTreeWatcher");
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

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation, VisualElement element, VisualMutationType mutationType) try
{
    Wh_Log(L"========================================");

    switch (mutationType)
    {
    case Add:
        Wh_Log(L"Mutation type: Add");
        break;

    case Remove:
        Wh_Log(L"Mutation type: Remove");
        break;

    default:
        Wh_Log(L"Mutation type: %d", static_cast<int>(mutationType));
        break;
    }

    Wh_Log(L"Element type: %s", element.Type);

    if (!g_initializedForThread) {
        Wh_Log(L"Not initialized for thread %u", GetCurrentThreadId());
        return S_OK;
    }

    if (mutationType == Add)
    {
        const auto inspectable = FromHandle(element.Handle);
        auto frameworkElement = inspectable.try_as<wux::FrameworkElement>();
        if (frameworkElement)
        {
            Wh_Log(L"FrameworkElement name: %s", frameworkElement.Name().c_str());
            ApplyCustomizations(element.Handle, frameworkElement, element.Type);
        }
        else
        {
            Wh_Log(L"Skipping non-FrameworkElement");
        }
    }
    else if (mutationType == Remove)
    {
        CleanupCustomizations(element.Handle);
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
_Use_decl_annotations_ STDAPI DllCanUnloadNow(void)
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

    const HMODULE wux(LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
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
    HRESULT hr;
    for (int i = 0; i < 10000; i++)
    {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"VisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), L"", location, CLSID_WindhawkTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            break;
        }
    }

    return hr;
}

#pragma endregion  // api_cpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <list>
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
#include <roapi.h>
#include <windows.graphics.effects.h>
#include <winstring.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuxh = wux::Hosting;
namespace awge = ABI::Windows::Graphics::Effects;

enum class Target {
    ShellExperienceHost,
    ShellHost,  // Win11 24H2.
};

Target g_target;

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
};

using PropertyOverridesMaybeUnresolved =
    std::variant<UnresolvedRules, ResolvedRules>;

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
    winrt::Windows::Foundation::IInspectable lastAppliedValue{nullptr};
    int64_t propertyChangedToken = 0;
    // Source template for dynamic styles whose value contains `{{...}}`
    // substitutions; re-evaluated whenever a referenced variable changes, with
    // the resolved result written back into `customValue`. Empty for static
    // styles.
    std::optional<DynamicStyleTemplate> dynamicTemplate;
    // Names of style variables this property's value depends on. Populated
    // alongside `dynamicTemplate`; empty for static styles.
    std::vector<std::wstring> variableDependencies;
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

thread_local std::unordered_map<InstanceHandle, ElementCustomizationState>
    g_elementsCustomizationState;

// Mod-global style variable registry. Populated by `Property=>VarName` capture
// rules and consumed by `{{VarName}}` substitutions in other styles. Last
// writer wins -- a new capture from any element overwrites the value.
struct StyleVariableValue {
    std::wstring stringForm;        // invariant-formatted text representation
    std::optional<double> numeric;  // only present when source was numeric
    // True for primitive captures whose `stringForm` is meaningful to insert
    // verbatim into a XAML attribute (numeric, boolean, string). False for
    // opaque types -- their stringForm is the captured class name, kept only
    // for diagnostics; bare-identifier substitution skips such variables.
    bool substitutable = false;
};

struct StyleVariableConsumer {
    InstanceHandle elementHandle;
    DependencyProperty property{nullptr};
    // Each consumer remembers its own fallbackClassName so that propagation can
    // re-resolve dynamic styles using the consumer's match-site context, not
    // the (potentially different) capturer's.
    std::wstring fallbackClassName;
};

thread_local std::unordered_map<std::wstring, StyleVariableValue>
    g_styleVariables;
thread_local std::unordered_map<std::wstring,
                                std::vector<StyleVariableConsumer>>
    g_styleVariableConsumers;

thread_local bool g_elementPropertyModifying;

// Global list to track ImageBrushes with failed loads for retry on network
// reconnection.
struct ImageBrushFailedLoadInfo {
    winrt::weak_ref<Media::ImageBrush> brush;
    winrt::hstring imageSource;
    Media::ImageBrush::ImageFailed_revoker imageFailedRevoker;
    Media::ImageBrush::ImageOpened_revoker imageOpenedRevoker;
};

struct FailedImageBrushesForThread {
    std::list<ImageBrushFailedLoadInfo> failedImageBrushes;
    winrt::Windows::System::DispatcherQueue dispatcher{nullptr};
};

thread_local FailedImageBrushesForThread g_failedImageBrushesForThread;

// Global registry of all threads that have failed image brushes.
std::mutex g_failedImageBrushesRegistryMutex;
std::vector<winrt::weak_ref<winrt::Windows::System::DispatcherQueue>>
    g_failedImageBrushesRegistry;
winrt::event_token g_networkStatusChangedToken;

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
        // A workaround for RowDefinitionCollection of RootContent which can't
        // be read by ReadLocalValue for some reason, even though it seems to be
        // a local property.
        if (value == DependencyProperty::UnsetValue()) {
            auto grid = elementDo.try_as<Controls::Grid>();
            if (grid && grid.Name() == L"RootContent") {
                auto value2 = elementDo.GetValue(property);
                if (value2 &&
                    winrt::get_class_name(value2) ==
                        L"Windows.UI.Xaml.Controls.RowDefinitionCollection") {
                    Wh_Log(
                        L"Using GetValue workaround for "
                        L"RowDefinitionCollection");
                    value = std::move(value2);
                }
            }
        }

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
    } else {
        // A workaround for Fill of HorizontalTrackRect or
        // HorizontalDecreaseRect which can't be read by ReadLocalValue for some
        // reason (null is returned instead).
        auto rect = elementDo.try_as<Shapes::Rectangle>();
        if (rect && (rect.Name() == L"HorizontalTrackRect" ||
                     rect.Name() == L"HorizontalDecreaseRect")) {
            auto value2 = elementDo.GetValue(property);
            if (value2 && winrt::get_class_name(value2) ==
                              L"Windows.UI.Xaml.Media.SolidColorBrush") {
                Wh_Log(L"Using GetValue workaround for %s",
                       rect.Name().c_str());
                value = std::move(value2);
            }
        }
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
class XamlBlurBrush : public Media::XamlCompositionBrushBaseT<XamlBlurBrush, Media::ISolidColorBrush>
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

    // The ISolidColorBrush implementation is required for
    // ActionCenter::FlexibleToastView::OnToastBackgroundBorderBackgroundChanged
    // in Windows.UI.ActionCenter.dll. If missing, the app crashes while trying
    // to show the first notification, which results in a crash loop.
    winrt::Windows::UI::Color Color() const {
        return m_tint;
    }
    void Color(winrt::Windows::UI::Color const& value) {
        // Do nothing.
    }

private:
    void RefreshThemeTint();
    void RefreshFallbackColor();
    bool ShouldUseFallback() const;
    void RefreshBrush();
    wuc::CompositionBrush CreateEffectBrush();
    wuc::CompositionBrush CreateFallbackBrush();

    wuc::Compositor m_compositor;
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
    winrt::Windows::System::DispatcherQueue m_dispatcher{nullptr};
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
    m_compositor(wuxh::ElementCompositionPreview::GetElementVisual(element)
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
            winrt::Windows::System::DispatcherQueue::GetForCurrentThread();

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

wuc::CompositionBrush XamlBlurBrush::CreateFallbackBrush()
{
    return m_compositor.CreateColorBrush(m_fallbackColor.value_or(m_tint));
}

wuc::CompositionBrush XamlBlurBrush::CreateEffectBrush()
{
    auto backdropBrush = m_compositor.CreateBackdropBrush();

    // Rec. 709 luma coefficients, used for saturation and luminosity.
    constexpr float kLumaR = 0.2126f;
    constexpr float kLumaG = 0.7152f;
    constexpr float kLumaB = 0.0722f;

    // 1. Blur
    auto blurEffect = winrt::make_self<GaussianBlurEffect>();
    blurEffect->Source = wuc::CompositionEffectSourceParameter(L"backdrop");
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
    wuc::CompositionSurfaceBrush noiseBrush{nullptr};
    if (m_noiseOpacity && *m_noiseOpacity > 0.0f)
    {
        float density = m_noiseDensity.value_or(1.0f);

        auto stream = CreateNoiseStream(density);
        auto surface =
            Media::LoadedImageSurface::StartLoadFromStream(stream);
        noiseBrush = m_compositor.CreateSurfaceBrush(surface);
        noiseBrush.Stretch(wuc::CompositionStretch::None);

        // Tile via border effect (wrap mode).
        auto borderEffect = winrt::make_self<BorderEffect>();
        borderEffect->Source =
            wuc::CompositionEffectSourceParameter(L"NoiseSource");

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

// Helper functions for tracking and retrying failed ImageBrush loads.
void RetryFailedImageLoadsOnCurrentThread() {
    Wh_Log(L"Retrying failed image loads on current thread");

    auto& failedImageBrushes = g_failedImageBrushesForThread.failedImageBrushes;

    // Retry loading all failed images by re-setting the ImageSource property.
    for (auto& info : failedImageBrushes) {
        if (auto brush = info.brush.get()) {
            try {
                Wh_Log(L"Retrying image load for: %s",
                       info.imageSource.c_str());
                // Clear the ImageSource first to force a reload.
                brush.ImageSource(nullptr);
                // Then create a new BitmapImage and set it.
                Media::Imaging::BitmapImage bitmapImage;
                bitmapImage.UriSource(
                    winrt::Windows::Foundation::Uri(info.imageSource));
                brush.ImageSource(bitmapImage);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error retrying image load %08X: %s", ex.code(),
                       ex.message().c_str());
            }
        }
    }

    // Clean up any weak refs that are no longer valid.
    std::erase_if(failedImageBrushes,
                  [](const auto& info) { return !info.brush.get(); });
}

void OnNetworkStatusChanged(
    winrt::Windows::Foundation::IInspectable const& sender) {
    Wh_Log(L"Network status changed, dispatching retry to all UI threads");

    // Get snapshot of dispatchers under lock.
    std::vector<winrt::Windows::System::DispatcherQueue> dispatchers;
    {
        std::lock_guard<std::mutex> lock(g_failedImageBrushesRegistryMutex);

        for (auto& weakDispatcher : g_failedImageBrushesRegistry) {
            if (auto dispatcher = weakDispatcher.get()) {
                dispatchers.push_back(dispatcher);
            }
        }

        // Clean up dead weak refs.
        std::erase_if(
            g_failedImageBrushesRegistry,
            [](const auto& weakDispatcher) { return !weakDispatcher.get(); });
    }

    // Dispatch retry to each UI thread.
    for (auto& dispatcher : dispatchers) {
        try {
            dispatcher.TryEnqueue(
                []() { RetryFailedImageLoadsOnCurrentThread(); });
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error dispatching retry to UI thread %08X: %s", ex.code(),
                   ex.message().c_str());
        }
    }
}

void RemoveFromFailedImageBrushes(Media::ImageBrush const& brush) {
    auto& failedImageBrushes = g_failedImageBrushesForThread.failedImageBrushes;

    std::erase_if(failedImageBrushes, [&brush](const auto& info) {
        if (auto existingBrush = info.brush.get()) {
            return existingBrush == brush;
        }
        return false;
    });
}

void SetupImageBrushTracking(Media::ImageBrush const& brush,
                             winrt::hstring const& imageSourceUrl) {
    // First remove any existing entry for this brush to avoid duplicates.
    RemoveFromFailedImageBrushes(brush);

    // Add new entry with event handlers.
    ImageBrushFailedLoadInfo info;
    info.brush = winrt::make_weak(brush);
    info.imageSource = imageSourceUrl;

    // Set up ImageFailed event handler - add to list only when load fails.
    info.imageFailedRevoker = brush.ImageFailed(
        winrt::auto_revoke,
        [brushWeak = winrt::make_weak(brush), imageSourceUrl](
            winrt::Windows::Foundation::IInspectable const& sender,
            ExceptionRoutedEventArgs const& e) {
            Wh_Log(L"ImageBrush load failed for: %s, error: %s",
                   imageSourceUrl.c_str(), e.ErrorMessage().c_str());
            // The brush should already be in the list, no action needed here as
            // we add it preemptively in SetupImageBrushTracking.
        });

    // Set up ImageOpened event handler - remove from list when load succeeds.
    info.imageOpenedRevoker = brush.ImageOpened(
        winrt::auto_revoke,
        [brushWeak = winrt::make_weak(brush)](
            winrt::Windows::Foundation::IInspectable const& sender,
            RoutedEventArgs const& e) {
            Wh_Log(L"ImageBrush loaded successfully, removing from retry list");

            if (auto brush = brushWeak.get()) {
                RemoveFromFailedImageBrushes(brush);
            }
        });

    // Add to the list preemptively - will be removed if load succeeds.
    auto& failedImageBrushes = g_failedImageBrushesForThread.failedImageBrushes;
    failedImageBrushes.push_back(std::move(info));

    // Ensure we have a dispatcher for this thread.
    if (!g_failedImageBrushesForThread.dispatcher) {
        try {
            g_failedImageBrushesForThread.dispatcher =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (g_failedImageBrushesForThread.dispatcher) {
                // Register this thread's dispatcher globally.
                std::lock_guard<std::mutex> lock(
                    g_failedImageBrushesRegistryMutex);
                g_failedImageBrushesRegistry.push_back(
                    winrt::make_weak(g_failedImageBrushesForThread.dispatcher));
                Wh_Log(L"Registered UI thread dispatcher for network retry");
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error getting dispatcher for current thread %08X: %s",
                   ex.code(), ex.message().c_str());
        }
    }

    // Register global network status changed handler if not already registered.
    // This is a one-time global registration.
    [[maybe_unused]] static bool networkHandlerRegistered = []() {
        try {
            g_networkStatusChangedToken =
                winrt::Windows::Networking::Connectivity::NetworkInformation::
                    NetworkStatusChanged(OnNetworkStatusChanged);
            Wh_Log(L"Registered global network status change handler");
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error registering network status handler %08X: %s",
                   ex.code(), ex.message().c_str());
        }
        return true;
    }();
}

void SetOrClearValue(DependencyObject elementDo,
                     DependencyProperty property,
                     const PropertyOverrideValue& overrideValue) {
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
            return;
        }
    } else {
        Wh_Log(L"Unsupported override value");
        return;
    }

    if (value == DependencyProperty::UnsetValue()) {
        Wh_Log(L"Clearing property value");
        try {
            elementDo.ClearValue(property);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
        return;
    }

    Wh_Log(L"Setting property value %s",
           value ? winrt::get_class_name(value).c_str() : L"(null)");

    // Track ImageBrush with remote ImageSource for retry on network
    // reconnection. This handles cases where an ImageBrush is set as a property
    // value (e.g., Background).
    if (auto imageBrush = value.try_as<Media::ImageBrush>()) {
        auto imageSource = imageBrush.ImageSource();
        if (auto bitmapImage =
                imageSource.try_as<Media::Imaging::BitmapImage>()) {
            auto uriSource = bitmapImage.UriSource();
            if (uriSource) {
                winrt::hstring uriString = uriSource.ToString();
                if (uriString.starts_with(L"https://") ||
                    uriString.starts_with(L"http://")) {
                    Wh_Log(L"Tracking ImageBrush with remote source: %s",
                           uriString.c_str());
                    SetupImageBrushTracking(imageBrush, uriString);
                }
            }
        }
    }
    // Also handle direct ImageSource property being set on an ImageBrush.
    else if (auto imageBrush = elementDo.try_as<Media::ImageBrush>()) {
        if (property == Media::ImageBrush::ImageSourceProperty()) {
            // Check if the value is a BitmapImage with an http(s):// URI.
            if (auto bitmapImage =
                    value.try_as<Media::Imaging::BitmapImage>()) {
                auto uriSource = bitmapImage.UriSource();
                if (uriSource) {
                    winrt::hstring uriString = uriSource.ToString();
                    if (uriString.starts_with(L"https://") ||
                        uriString.starts_with(L"http://")) {
                        Wh_Log(
                            L"Tracking ImageBrush ImageSource property with "
                            L"remote source: %s",
                            uriString.c_str());
                        SetupImageBrushTracking(imageBrush, uriString);
                    }
                }
            }
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
            property == Controls::Control::FontWeightProperty()) {
            auto valueInt = value.try_as<int>();
            if (valueInt && *valueInt >= std::numeric_limits<uint16_t>::min() &&
                *valueInt <= std::numeric_limits<uint16_t>::max()) {
                value = winrt::box_value(winrt::Windows::UI::Text::FontWeight{
                    static_cast<uint16_t>(*valueInt)});
            }
        }

        elementDo.SetValue(property, value);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
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

Style GetStyleFromXamlSettersWithFallbackType(
    const std::wstring_view type,
    const std::wstring_view fallbackType,
    const std::wstring_view xamlStyleSetters) {
    try {
        return GetStyleFromXamlSetters(type, xamlStyleSetters);
    } catch (winrt::hresult_error const& ex) {
        constexpr HRESULT kStowedException = 0x802B000A;
        if (ex.code() != kStowedException || fallbackType.empty() ||
            fallbackType == type) {
            throw;
        }

        // For some types such as JumpViewUI.JumpListListViewItem, the following
        // error is returned:
        //
        // Error 802B000A: Failed to create a 'System.Type' from the text
        // 'windhawkstyler:JumpListListViewItem'. [Line: 8 Position: 12]
        //
        // Retry with a fallback type, which will allow to at least use the
        // basic properties.
        Wh_Log(L"Retrying with fallback type type due to error %08X: %s",
               ex.code(), ex.message().c_str());
        return GetStyleFromXamlSetters(fallbackType, xamlStyleSetters);
    }
}

const ResolvedRules& GetResolvedPropertyOverrides(
    const std::wstring_view type,
    const std::wstring_view fallbackType,
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

            auto style = GetStyleFromXamlSettersWithFallbackType(
                type, fallbackType, xaml);

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
    std::wstring_view fallbackType,
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
        auto style =
            GetStyleFromXamlSettersWithFallbackType(type, fallbackType, xaml);
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
    const std::wstring_view fallbackType,
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

            auto style = GetStyleFromXamlSettersWithFallbackType(
                type, fallbackType, xaml);

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
// representation so XAML always sees `.` as the decimal separator.
std::wstring FormatDoubleInvariant(double d) {
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

    for (const auto& propertyValue : GetResolvedPropertyValues(
             matcher.type,
             fallbackClassName ? fallbackClassName
                               : winrt::name_of<FrameworkElement>(),
             &matcher.propertyValues)) {
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
            override.elementMatcher.type,
            fallbackClassName ? fallbackClassName
                              : winrt::name_of<FrameworkElement>(),
            &override.propertyOverrides);

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

// Recursive-descent evaluator for `{{ ... }}` expressions. Supports number
// literal, identifier (style variable reference), parenthesized subexpression,
// the binary ops + - * /, unary - / +, and the two-arg functions min(a, b) and
// max(a, b). Standard math precedence.
//
// Variable references pushed into outDeps so the dependent style can be
// re-evaluated when those variables change.
class StyleVariableExpressionEvaluator {
   public:
    StyleVariableExpressionEvaluator(std::wstring_view text,
                                     std::vector<std::wstring>* outDeps)
        : m_text(text), m_outDeps(outDeps) {}

    // Returns the numeric result of the expression. Throws std::runtime_error
    // on parse / evaluation failure (including when an identifier resolves to a
    // non-numeric variable, or when the expression produces a non-finite result
    // -- NaN/Inf can't be formatted into XAML attributes meaningfully and would
    // also break the consumer-equality check in
    // SetStyleVariableIfChangedAndPropagate, since NaN != NaN).
    double Evaluate() {
        m_pos = 0;
        SkipWhitespace();
        double v = ParseExpression();
        SkipWhitespace();
        if (m_pos != m_text.size()) {
            throw std::runtime_error(
                "Unexpected trailing characters in style variable expression");
        }
        if (!std::isfinite(v)) {
            throw std::runtime_error(
                "Style variable expression produced a non-finite result");
        }
        return v;
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

    double ParseExpression() {
        double v = ParseTerm();
        while (true) {
            SkipWhitespace();
            if (ConsumeChar(L'+')) {
                v += ParseTerm();
            } else if (ConsumeChar(L'-')) {
                v -= ParseTerm();
            } else {
                break;
            }
        }
        return v;
    }

    double ParseTerm() {
        double v = ParseFactor();
        while (true) {
            SkipWhitespace();
            if (ConsumeChar(L'*')) {
                v *= ParseFactor();
            } else if (ConsumeChar(L'/')) {
                double rhs = ParseFactor();
                if (rhs == 0.0) {
                    throw std::runtime_error(
                        "Division by zero in style variable expression");
                }
                v /= rhs;
            } else {
                break;
            }
        }
        return v;
    }

    double ParseFactor() {
        SkipWhitespace();
        if (ConsumeChar(L'+')) {
            return ParseFactor();
        }
        if (ConsumeChar(L'-')) {
            return -ParseFactor();
        }
        return ParsePrimary();
    }

    double ParsePrimary() {
        SkipWhitespace();
        if (m_pos >= m_text.size()) {
            throw std::runtime_error(
                "Unexpected end of style variable expression");
        }

        wchar_t c = m_text[m_pos];
        if (c == L'(') {
            m_pos++;
            double v = ParseExpression();
            SkipWhitespace();
            if (!ConsumeChar(L')')) {
                throw std::runtime_error(
                    "Missing ')' in style variable expression");
            }
            return v;
        }

        if ((c >= L'0' && c <= L'9') || c == L'.') {
            return ParseNumberLiteral();
        }

        if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') || c == L'_') {
            return ParseIdentifierOrCall();
        }

        throw std::runtime_error(
            "Unexpected character in style variable expression");
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

    double ParseIdentifierOrCall() {
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
            double a = ParseExpression();
            if (!ConsumeChar(L',')) {
                throw std::runtime_error(
                    "Expected ',' in min/max style variable call");
            }
            double b = ParseExpression();
            if (!ConsumeChar(L')')) {
                throw std::runtime_error(
                    "Missing ')' after min/max style variable call");
            }
            if (ident == L"min") {
                return (a < b) ? a : b;
            }
            if (ident == L"max") {
                return (a > b) ? a : b;
            }
            throw std::runtime_error(
                "Unknown function in style variable expression");
        }
        return LookupVariableNumeric(std::wstring(ident));
    }

    double LookupVariableNumeric(const std::wstring& name) {
        if (m_outDeps) {
            m_outDeps->push_back(name);
        }
        auto it = g_styleVariables.find(name);
        if (it == g_styleVariables.end()) {
            Wh_Log(L"Style variable '%s' not yet defined; treating as 0",
                   name.c_str());
            return 0.0;
        }
        if (!it->second.numeric) {
            throw std::runtime_error(
                "Style variable used in arithmetic is not numeric");
        }
        return *it->second.numeric;
    }

    std::wstring_view m_text;
    std::vector<std::wstring>* m_outDeps;
    size_t m_pos = 0;
};

// Evaluate a single expression body (the text between `{{` and `}}`). If the
// body is a bare identifier, returns the variable's `stringForm` directly --
// but only when the captured value is a primitive type flagged `substitutable`
// (numeric, boolean, or string). Missing variables and opaque-type captures
// both cause this function to return std::nullopt, at which point
// ExpandStyleVariables aborts the whole expansion and the consuming style is
// skipped. This matches the arithmetic path's behaviour of failing closed
// rather than substituting a value that won't parse.
std::optional<std::wstring> EvaluateStyleVariableExpression(
    std::wstring_view exprText,
    std::vector<std::wstring>* outDeps) {
    auto trimmed = TrimStringView(exprText);
    if (trimmed.empty()) {
        Wh_Log(L"Empty style variable expression");
        return std::nullopt;
    }

    if (IsValidStyleVariableIdentifier(trimmed)) {
        std::wstring name(trimmed);
        if (outDeps) {
            outDeps->push_back(name);
        }
        auto it = g_styleVariables.find(name);
        if (it == g_styleVariables.end()) {
            Wh_Log(L"Style variable '%s' not yet defined; skipping style",
                   name.c_str());
            return std::nullopt;
        }
        if (!it->second.substitutable) {
            Wh_Log(
                L"Style variable '%s' is not substitutable (captured type "
                L"'%s'); skipping style",
                name.c_str(), it->second.stringForm.c_str());
            return std::nullopt;
        }
        return it->second.stringForm;
    }

    try {
        StyleVariableExpressionEvaluator eval(trimmed, outDeps);
        double v = eval.Evaluate();
        return FormatDoubleInvariant(v);
    } catch (std::exception const& ex) {
        Wh_Log(L"Style variable expression failed: %S (in '%.*s')", ex.what(),
               static_cast<int>(trimmed.size()), trimmed.data());
        return std::nullopt;
    }
}

// Walks the input text, repeatedly expanding the innermost `{{ ... }}`
// substitution. Returns std::nullopt on parse failure (and logs a warning).
//
// Inner-matching rule: the first `}}` is paired with the *rightmost* `{{` that
// precedes it. So `{{{x}}}` -> `{` + value-of-x + `}` (literal outer braces).
//
// Substituted text is treated as literal (no further `{{...}}` expansion of the
// substituted output) to keep behavior predictable.
std::optional<std::wstring> ExpandStyleVariables(
    std::wstring_view input,
    std::vector<std::wstring>* outDeps) {
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
        auto expanded = EvaluateStyleVariableExpression(exprText, outDeps);
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

// Remove this (handle, property) entry from the consumer lists of every
// variable named in oldDeps, then add it for every variable named in newDeps.
// `fallbackClassName` is stored on each newly-added consumer entry so the
// per-consumer context is preserved across propagations; it is irrelevant when
// newDeps is empty (pure-removal calls from the cleanup paths).
void UpdateStyleVariableConsumers(InstanceHandle handle,
                                  DependencyProperty property,
                                  PCWSTR fallbackClassName,
                                  const std::vector<std::wstring>& oldDeps,
                                  const std::vector<std::wstring>& newDeps) {
    for (const auto& dep : oldDeps) {
        auto it = g_styleVariableConsumers.find(dep);
        if (it == g_styleVariableConsumers.end()) {
            continue;
        }
        auto& consumers = it->second;
        std::erase_if(consumers, [&](const StyleVariableConsumer& c) {
            return c.elementHandle == handle && c.property == property;
        });
        if (consumers.empty()) {
            g_styleVariableConsumers.erase(it);
        }
    }

    std::wstring fallbackClassNameStr =
        fallbackClassName ? fallbackClassName : L"";
    for (const auto& dep : newDeps) {
        auto& consumers = g_styleVariableConsumers[dep];
        bool already = std::any_of(consumers.begin(), consumers.end(),
                                   [&](const StyleVariableConsumer& c) {
                                       return c.elementHandle == handle &&
                                              c.property == property;
                                   });
        if (!already) {
            consumers.push_back({handle, property, fallbackClassNameStr});
        }
    }
}

// Re-evaluate the dynamic template stored on `propertyCustomizationState` and
// return the resolved IInspectable / XamlBlurBrushParams ready to be applied.
// Updates the (handle, property) -> g_styleVariableConsumers registry to match
// the freshly computed dependency set so future variable changes route to this
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
// Returns std::nullopt if the state has no template, expansion failed, or XAML
// resolution failed.
std::optional<PropertyOverrideValue> ResolveDynamicStyleValue(
    InstanceHandle handle,
    FrameworkElement element,
    DependencyProperty property,
    PCWSTR fallbackClassName,
    ElementPropertyCustomizationState* propertyCustomizationState) {
    if (!propertyCustomizationState->dynamicTemplate) {
        return std::nullopt;
    }

    const auto& tmpl = *propertyCustomizationState->dynamicTemplate;

    std::vector<std::wstring> newDeps;
    auto expanded = ExpandStyleVariables(tmpl.rawValue, &newDeps);

    UpdateStyleVariableConsumers(
        handle, property, fallbackClassName,
        propertyCustomizationState->variableDependencies, newDeps);
    propertyCustomizationState->variableDependencies = std::move(newDeps);

    if (!expanded) {
        return std::nullopt;
    }

    auto typeName = winrt::get_class_name(element);
    auto resolved = ResolveExpandedSinglePropertyValue(
        std::wstring_view(typeName),
        fallbackClassName ? std::wstring_view(fallbackClassName)
                          : winrt::name_of<FrameworkElement>(),
        tmpl.propertyName, *expanded, tmpl.isXamlValue);
    if (!resolved) {
        Wh_Log(
            L"Dynamic style resolution failed for '%s' on %s; keeping "
            L"previously applied value",
            tmpl.propertyName.c_str(), typeName.c_str());
    }
    return resolved;
}

// Re-evaluate every dependent style for the named variable. Driven by capture
// callbacks when the source property changes, and by the initial capture when a
// target is first matched. Each consumer carries its own fallbackClassName
// (recorded when the consumer was registered), so propagation correctly uses
// the consumer's own match-site context to re-parse the rule body, even when
// the capturer was matched against a different type/fallback class.
void PropagateStyleVariableChange(const std::wstring& varName) {
    auto consumersIt = g_styleVariableConsumers.find(varName);
    if (consumersIt == g_styleVariableConsumers.end()) {
        return;
    }

    auto consumersCopy = consumersIt->second;
    for (const auto& consumer : consumersCopy) {
        auto stateIt =
            g_elementsCustomizationState.find(consumer.elementHandle);
        if (stateIt == g_elementsCustomizationState.end()) {
            continue;
        }
        auto element = stateIt->second.element.get();
        if (!element) {
            continue;
        }

        PCWSTR consumerFallbackClassName =
            consumer.fallbackClassName.empty()
                ? nullptr
                : consumer.fallbackClassName.c_str();

        for (auto& [vsgWeak, vsgState] : stateIt->second.perVisualStateGroup) {
            auto propIt =
                vsgState.propertyCustomizationStates.find(consumer.property);
            if (propIt == vsgState.propertyCustomizationStates.end()) {
                continue;
            }
            auto& propState = propIt->second;
            if (!propState.dynamicTemplate) {
                continue;
            }

            auto resolved = ResolveDynamicStyleValue(
                consumer.elementHandle, element, consumer.property,
                consumerFallbackClassName, &propState);
            if (!resolved) {
                continue;
            }
            if (!propState.originalValue) {
                propState.originalValue =
                    ReadLocalValueWithWorkaround(element, consumer.property);
            }
            propState.customValue = *resolved;

            bool wasModifying = g_elementPropertyModifying;
            g_elementPropertyModifying = true;
            SetOrClearValue(element, consumer.property, *resolved);
            propState.lastAppliedValue =
                ReadLocalValueWithWorkaround(element, consumer.property);
            g_elementPropertyModifying = wasModifying;
        }
    }
}

// Compare a captured value to whatever's currently in g_styleVariables for the
// same name; if different, store and notify dependents. Each consumer's own
// fallbackClassName lives on the consumer entry, so this function does not need
// to be told the capturer's context. Used by every path that wants to publish a
// captured value -- the per-property capture callback, the SizeChanged
// catch-all, and the initial seeding loop -- so the no-op fast path applies
// uniformly.
void SetStyleVariableIfChangedAndPropagate(const std::wstring& varName,
                                           StyleVariableValue value) {
    auto it = g_styleVariables.find(varName);
    if (it != g_styleVariables.end() &&
        it->second.stringForm == value.stringForm &&
        it->second.numeric == value.numeric &&
        it->second.substitutable == value.substitutable) {
        Wh_Log(L"Style variable '%s' unchanged at '%s'", varName.c_str(),
               value.stringForm.c_str());
        return;
    }

    Wh_Log(L"Style variable '%s' changed: '%s' -> '%s'", varName.c_str(),
           it != g_styleVariables.end() ? it->second.stringForm.c_str()
                                        : L"(unset)",
           value.stringForm.c_str());
    g_styleVariables[varName] = std::move(value);
    PropagateStyleVariableChange(varName);
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
// Seeding writes the captured values into g_styleVariables in a single batch
// (to avoid intermediate inconsistent states for consumers that depend on
// multiple variables from this element) and then propagates only the variables
// whose values actually changed -- the no-op fast path matches the one used by
// the change-driven callbacks below. The function does not need the capturer's
// fallbackClassName: each StyleVariableConsumer entry already carries its own
// consumer-side fallback, so propagation routes through the right context per
// consumer.
void SetUpCapturesForElement(InstanceHandle handle,
                             FrameworkElement element,
                             const std::vector<CaptureSpec>& captures,
                             ElementCustomizationState* elementState) {
    if (captures.empty()) {
        return;
    }

    auto elementDo = element.as<DependencyObject>();
    winrt::weak_ref<FrameworkElement> elementWeakRef = element;

    // Names of variables whose seeded value differs from whatever's already in
    // g_styleVariables. Only these need a propagation pass at the end.
    std::vector<std::wstring> changedVarNames;
    changedVarNames.reserve(captures.size());

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

        auto existingIt = g_styleVariables.find(capture.varName);
        const bool changed =
            existingIt == g_styleVariables.end() ||
            existingIt->second.stringForm != value.stringForm ||
            existingIt->second.numeric != value.numeric ||
            existingIt->second.substitutable != value.substitutable;

        if (changed) {
            Wh_Log(
                L"Seeding capture variable '%s' from %s with value '%s' "
                L"(was: '%s')",
                capture.varName.c_str(), winrt::get_class_name(element).c_str(),
                value.stringForm.c_str(),
                existingIt != g_styleVariables.end()
                    ? existingIt->second.stringForm.c_str()
                    : L"(unset)");
            g_styleVariables[capture.varName] = std::move(value);
            changedVarNames.push_back(capture.varName);
        } else {
            Wh_Log(L"Capture variable '%s' from %s already at '%s'",
                   capture.varName.c_str(),
                   winrt::get_class_name(element).c_str(),
                   value.stringForm.c_str());
        }

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
                [varName, elementWeakRef](DependencyObject sender,
                                          DependencyProperty property) {
                    auto element = elementWeakRef.get();
                    if (!element) {
                        return;
                    }
                    auto value =
                        ReadCapturedStyleVariableValue(element, property);
                    SetStyleVariableIfChangedAndPropagate(varName,
                                                          std::move(value));
                });
    }

    if (!sizeChangedCaptures.empty()) {
        elementState->captureSizeChangedToken = element.SizeChanged(
            [elementWeakRef,
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
                    SetStyleVariableIfChangedAndPropagate(varName,
                                                          std::move(value));
                }
            });
    }

    // Propagate the freshly seeded values to any consumers that were already
    // registered before this element was matched. Variables whose value did not
    // actually change are skipped, matching the per-callback fast path.
    for (const auto& varName : changedVarNames) {
        PropagateStyleVariableChange(varName);
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
    InstanceHandle handle,
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
                    handle, element, property, fallbackClassName,
                    &propertyCustomizationState);
            } else {
                resolved = it->second;
            }

            if (resolved) {
                propertyCustomizationState.originalValue =
                    ReadLocalValueWithWorkaround(element, property);
                propertyCustomizationState.customValue = *resolved;
                SetOrClearValue(element, property, *resolved);
                propertyCustomizationState.lastAppliedValue =
                    ReadLocalValueWithWorkaround(element, property);
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

                    auto localValue =
                        ReadLocalValueWithWorkaround(element, property);

                    // Only update originalValue if the local value was changed
                    // externally (e.g. by a Setter). When an animation changes
                    // only the effective value, the local value still matches
                    // what we set, so updating originalValue would corrupt it
                    // with our own brush - causing the brush to survive cleanup
                    // and crash when the mod's DLL is unloaded.
                    if (localValue !=
                        propertyCustomizationState.lastAppliedValue) {
                        propertyCustomizationState.originalValue = localValue;
                    }

                    Wh_Log(L"Re-applying style for %s",
                           winrt::get_class_name(element).c_str());

                    g_elementPropertyModifying = true;
                    SetOrClearValue(element, property,
                                    *propertyCustomizationState.customValue);
                    propertyCustomizationState.lastAppliedValue =
                        ReadLocalValueWithWorkaround(element, property);
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
                [elementWeakRef, propertyOverrides, handle,
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
                                    handle, element, property,
                                    fallbackClassNamePtr,
                                    &propertyCustomizationState);
                            } else {
                                // Transitioning from dynamic to static for this
                                // visual state: clear template metadata and
                                // unregister consumer entries.
                                if (propertyCustomizationState
                                        .dynamicTemplate) {
                                    UpdateStyleVariableConsumers(
                                        handle, property,
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
                                if (!propertyCustomizationState.originalValue) {
                                    propertyCustomizationState.originalValue =
                                        ReadLocalValueWithWorkaround(element,
                                                                     property);
                                }

                                propertyCustomizationState.customValue =
                                    *resolved;
                                SetOrClearValue(element, property, *resolved);
                                propertyCustomizationState.lastAppliedValue =
                                    ReadLocalValueWithWorkaround(element,
                                                                 property);
                            }
                        } else {
                            if (propertyCustomizationState.dynamicTemplate) {
                                UpdateStyleVariableConsumers(
                                    handle, property,
                                    /*fallbackClassName=*/nullptr,
                                    propertyCustomizationState
                                        .variableDependencies,
                                    {});
                                propertyCustomizationState.variableDependencies
                                    .clear();
                                propertyCustomizationState.dynamicTemplate
                                    .reset();
                            }
                            if (propertyCustomizationState.originalValue) {
                                SetOrClearValue(
                                    element, property,
                                    *propertyCustomizationState.originalValue);
                                propertyCustomizationState.originalValue
                                    .reset();
                            }
                            propertyCustomizationState.lastAppliedValue =
                                nullptr;

                            propertyCustomizationState.customValue.reset();
                        }
                    }

                    g_elementPropertyModifying = false;
                });
    }
}

void RestoreCustomizationsForVisualStateGroup(
    InstanceHandle handle,
    FrameworkElement element,
    std::optional<winrt::weak_ref<VisualStateGroup>>
        visualStateGroupOptionalWeakPtr,
    const ElementCustomizationStateForVisualStateGroup&
        elementCustomizationStateForVisualStateGroup) {
    if (element) {
        for (const auto& [property, state] :
             elementCustomizationStateForVisualStateGroup
                 .propertyCustomizationStates) {
            try {
                element.UnregisterPropertyChangedCallback(
                    property, state.propertyChangedToken);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }

            if (!state.variableDependencies.empty()) {
                UpdateStyleVariableConsumers(handle, property,
                                             /*fallbackClassName=*/nullptr,
                                             state.variableDependencies, {});
            }

            if (state.originalValue) {
                SetOrClearValue(element, property, *state.originalValue);
            }
        }
    } else {
        // Element is gone; still clear consumer entries so a stale (handle,
        // property) pair isn't visited during PropagateStyleVariableChange.
        for (const auto& [property, state] :
             elementCustomizationStateForVisualStateGroup
                 .propertyCustomizationStates) {
            if (!state.variableDependencies.empty()) {
                UpdateStyleVariableConsumers(handle, property,
                                             /*fallbackClassName=*/nullptr,
                                             state.variableDependencies, {});
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

void MergeResourceVariables();

void ApplyCustomizations(InstanceHandle handle,
                         FrameworkElement element,
                         PCWSTR fallbackClassName) {
    // Merge resource dictionary on first element add. Merging it earlier on
    // window creation doesn't work, perhaps merged dictionaries are reset
    // during initialization.
    if (!g_resourceVariablesThemeDict) {
        MergeResourceVariables();
    }

    auto resolved = FindElementPropertyOverrides(element, fallbackClassName);
    if (resolved.overridesPerVSG.empty() && resolved.captures.empty()) {
        return;
    }

    Wh_Log(L"Applying styles to %s", winrt::get_class_name(element).c_str());

    auto& elementCustomizationState = g_elementsCustomizationState[handle];

    for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
        RestoreCustomizationsForVisualStateGroup(
            handle, element, visualStateGroupOptionalWeakPtrIter, stateIter);
    }

    elementCustomizationState.element = element;
    elementCustomizationState.perVisualStateGroup.clear();

    // Wire up captures first so any variables they define are visible to
    // dynamic value-rules applied below. Note: SetUpCapturesForElement does not
    // need this element's fallbackClassName -- propagation routes through each
    // consumer's own stored fallback.
    SetUpCapturesForElement(handle, element, resolved.captures,
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
            handle, element, visualStateGroup, fallbackClassName,
            std::move(overridesForVisualStateGroup),
            elementCustomizationStateForVisualStateGroup);
    }
}

void CleanupCustomizations(InstanceHandle handle) {
    if (auto it = g_elementsCustomizationState.find(handle);
        it != g_elementsCustomizationState.end()) {
        auto& elementCustomizationState = it->second;

        auto element = elementCustomizationState.element.get();

        RestoreCapturesForElement(element, elementCustomizationState);

        for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
             elementCustomizationState.perVisualStateGroup) {
            RestoreCustomizationsForVisualStateGroup(
                handle, element, visualStateGroupOptionalWeakPtrIter,
                stateIter);
        }

        g_elementsCustomizationState.erase(it);
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
            return L"Windows.UI.Xaml.Shapes.Rectangle";
        }

        return L"Windows.UI.Xaml.Controls." + std::wstring{type};
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

void AddElementCustomizationRules(std::wstring_view target,
                                  std::vector<std::wstring> styles) {
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
        AddElementCustomizationRules(targetStringSetting.get(),
                                     std::move(styles));
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
                Interop::TypeName{resourceClassName},
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
        auto dispatcherQueue =
            winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        g_colorValuesChangedToken =
            g_uiSettings.ColorValuesChanged([dispatcherQueue](auto&&, auto&&) {
                dispatcherQueue.TryEnqueue(RefreshThemeResourceEntries);
            });
    }
}

void ProcessAllStylesFromSettings() {
    PCWSTR themeName = Wh_GetStringSetting(L"theme");
    const Theme* theme = nullptr;
    if (wcscmp(themeName, L"TranslucentShell") == 0) {
        theme = &g_themeTranslucentShell;
    } else if (wcscmp(themeName, L"Matter") == 0) {
        theme = &g_themeMatter;
    } else if (wcscmp(themeName, L"Unified") == 0) {
        theme = &g_themeUnified;
    } else if (wcscmp(themeName, L"10JumpLists") == 0) {
        theme = &g_theme10JumpLists;
    } else if (wcscmp(themeName, L"WindowGlass") == 0) {
        theme = &g_themeWindowGlass;
    } else if (wcscmp(themeName, L"WindowGlass_variant_alternative") == 0) {
        theme = &g_themeWindowGlass_variant_alternative;
    } else if (wcscmp(themeName, L"Oversimplified&Accentuated") == 0) {
        theme = &g_themeOversimplified_Accentuated;
    } else if (wcscmp(themeName, L"TintedGlass") == 0) {
        theme = &g_themeTintedGlass;
    } else if (wcscmp(themeName, L"Fluid") == 0) {
        theme = &g_themeFluid;
    } else if (wcscmp(themeName, L"LiquidGlass") == 0) {
        theme = &g_themeLiquidGlass;
    } else if (wcscmp(themeName, L"BetterControl11") == 0) {
        theme = &g_themeBetterControl11;
    } else if (wcscmp(themeName, L"LayerMicaUI") == 0) {
        theme = &g_themeLayerMicaUI;
    } else if (wcscmp(themeName, L"Borderless") == 0) {
        theme = &g_themeBorderless;
    }
    Wh_FreeStringSetting(themeName);

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

                AddElementCustomizationRules(themeTargetStyle.target,
                                             std::move(styles));
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
    // Clear failed image brushes list for this thread (revokers will
    // automatically unregister).
    g_failedImageBrushesForThread.failedImageBrushes.clear();
    g_failedImageBrushesForThread.dispatcher = nullptr;

    for (const auto& [handle, elementCustomizationState] :
         g_elementsCustomizationState) {
        auto element = elementCustomizationState.element.get();

        RestoreCapturesForElement(element, elementCustomizationState);

        for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
             elementCustomizationState.perVisualStateGroup) {
            RestoreCustomizationsForVisualStateGroup(
                handle, element, visualStateGroupOptionalWeakPtrIter,
                stateIter);
        }
    }

    g_elementsCustomizationState.clear();
    g_styleVariables.clear();
    g_styleVariableConsumers.clear();

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

bool RunFromWindowThreadViaPostMessage(HWND hWnd,
                                       RunFromWindowThreadProc_t proc,
                                       PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsgViaPostMessage =
        RegisterWindowMessage(
            L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
        HHOOK hook;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_GETMESSAGE,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION && wParam == PM_REMOVE) {
                MSG* msg = (MSG*)lParam;
                if (msg->message ==
                    runFromWindowThreadRegisteredMsgViaPostMessage) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)msg->lParam;
                    if (param) {
                        param->proc(param->procParam);
                        UnhookWindowsHookEx(param->hook);
                        delete param;
                        msg->lParam = 0;
                    }
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    auto* param = new RUN_FROM_WINDOW_THREAD_PARAM{
        .proc = proc,
        .procParam = procParam,
        .hook = hook,
    };
    if (!PostMessage(hWnd, runFromWindowThreadRegisteredMsgViaPostMessage, 0,
                     (LPARAM)param)) {
        UnhookWindowsHookEx(hook);
        delete param;
        return false;
    }

    return true;
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName, PCSTR funcName) {
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    switch (g_target) {
        case Target::ShellExperienceHost:
            if (bTextualClassName &&
                _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                Wh_Log(L"Initializing - Created core window: %08X via %S",
                       (DWORD)(ULONG_PTR)hWnd, funcName);
                InitializeForCurrentThread();
                InitializeSettingsAndTap();
            }
            break;

        case Target::ShellHost:
            if (bTextualClassName &&
                _wcsicmp(lpClassName, L"ControlCenterWindow") == 0) {
                Wh_Log(
                    L"Initializing - Created ControlCenterWindow: %08X via %S",
                    (DWORD)(ULONG_PTR)hWnd, funcName);
                // Initializing at this point is too early and doesn't work.
                RunFromWindowThreadViaPostMessage(
                    hWnd,
                    [](PVOID) {
                        InitializeForCurrentThread();
                        InitializeSettingsAndTap();
                    },
                    nullptr);
            }
            break;
    }
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

    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);

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

    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);

    return hWnd;
}

std::vector<HWND> GetCoreWnds() {
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

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            switch (g_target) {
                case Target::ShellExperienceHost:
                    if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") ==
                        0) {
                        param.hWnds->push_back(hWnd);
                    }
                    break;

                case Target::ShellHost:
                    if (_wcsicmp(szClassName, L"ControlCenterWindow") == 0) {
                        param.hWnds->push_back(hWnd);
                    }
                    break;
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

PTP_TIMER g_statsTimer;

bool StartStatsTimer() {
    static constexpr WCHAR kStatsBaseUrl[] =
        L"https://github.com/ramensoftware/"
        L"windows-11-notification-center-styling-guide/"
        L"releases/download/stats-v4/";

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

    ULONGLONG dueTime = k24Hours - (currentTime - lastStatsTime);
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

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_target = Target::ShellExperienceHost;

    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        void* pCreateWindowInBand =
            (void*)GetProcAddress(user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            Wh_SetFunctionHook(pCreateWindowInBand,
                               (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }

        void* pCreateWindowInBandEx =
            (void*)GetProcAddress(user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx) {
            Wh_SetFunctionHook(pCreateWindowInBandEx,
                               (void*)CreateWindowInBandEx_Hook,
                               (void**)&CreateWindowInBandEx_Original);
        }
    }

    if (g_target == Target::ShellExperienceHost) {
        StartStatsTimer();
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    bool initialize = false;

    for (auto hCoreWnd : GetCoreWnds()) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { InitializeForCurrentThread(); }, nullptr);
        initialize = true;
    }

    if (initialize) {
        InitializeSettingsAndTap();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_target == Target::ShellExperienceHost) {
        StopStatsTimer();
    }

    UninitializeSettingsAndTap();

    for (auto hCoreWnd : GetCoreWnds()) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { UninitializeForCurrentThread(); }, nullptr);
    }

    // Unregister global network status change handler.
    if (g_networkStatusChangedToken) {
        try {
            winrt::Windows::Networking::Connectivity::NetworkInformation::
                NetworkStatusChanged(g_networkStatusChangedToken);
            Wh_Log(L"Unregistered global network status change handler");
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error unregistering network status handler %08X: %s",
                   ex.code(), ex.message().c_str());
        }
        g_networkStatusChangedToken = {};
    }

    // Clear the dispatcher registry.
    {
        std::lock_guard<std::mutex> lock(g_failedImageBrushesRegistryMutex);
        g_failedImageBrushesRegistry.clear();
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    UninitializeSettingsAndTap();

    bool initialize = false;

    for (auto hCoreWnd : GetCoreWnds()) {
        Wh_Log(L"Reinitializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                UninitializeForCurrentThread();
                InitializeForCurrentThread();
            },
            nullptr);
        initialize = true;
    }

    if (initialize) {
        InitializeSettingsAndTap();
    }
}
