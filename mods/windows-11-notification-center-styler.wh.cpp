// ==WindhawkMod==
// @id              windows-11-notification-center-styler
// @name            Windows 11 Notification Center Styler
// @description     Customize the Notification Center and Action Center with themes contributed by others or create your own
// @version         1.4
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
`BlurAmount`, `TintColor`, and `TintOpacity` properties. For example:
`Fill:=<WindhawkBlur BlurAmount="10" TintColor="#80FF00FF"/>`. Theme resources
are also supported, for example: `Fill:=<WindhawkBlur BlurAmount="18"
TintColor="{ThemeResource SystemAccentColorDark1}" TintOpacity="0.5"/>`.

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
- controlStyles:
  - - target: ""
      $name: Target
    - styles: [""]
      $name: Styles
  $name: Control styles
- styleConstants: [""]
  $name: Style constants
  $description: >-
    Constants which can be defined once and used in multiple styles.

    Each entry contains a style name and value, separated by '=', for example:

    mainColor=#fafad2

    The constant can then be used in style definitions by prepending '$', for
    example:

    Fill=$mainColor

    Background:=<AcrylicBrush TintColor="$mainColor" TintOpacity="0.3" />
- themeResourceVariables: [""]
  $name: Resource variables
  $description: >-
    Use "Key=Value" to override an existing resource with a new value.

    Use "Key@Dark=Value" or "Key@Light=Value" to define theme-aware resources
    that can be referenced with {ThemeResource Key} in styles.
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
        L"Background:=Trabsparent",
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
    L"Glass=<WindhawkBlur BlurAmount=\"3\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Frosted=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Acrylic=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.3\" FallbackColor=\"{ThemeResource SystemChromeAltHighColor}\" />",
    L"Background=$Translucent",
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
        L"Background:=Trabsparent",
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
    L"Glass=<WindhawkBlur BlurAmount=\"3\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Frosted=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Acrylic=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.3\" FallbackColor=\"{ThemeResource SystemChromeAltHighColor}\" />",
    L"Background=$Translucent",
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
    L"BorderBrush=<LinearGradientBrush x:Key=\"ShellTaskbarItemGradientStrokeColorSecondaryBrush\" MappingMode=\"Absolute\" StartPoint=\"0,0\" EndPoint=\"0,3\"><LinearGradientBrush.GradientStops><GradientStop Offset=\"0.33\" Color=\"#1AFFFFFF\" /><GradientStop Offset=\"1\" Color=\"#0FFFFFFF\" /></LinearGradientBrush.GradientStops></LinearGradientBrush>",
    L"BorderThickness=2",
    L"CornerRadius=4",
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

    if (!g_initializedForThread) {
        Wh_Log(L"NOTE: Not initialized for thread %u", GetCurrentThreadId());
    }

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

#include <list>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

using namespace std::string_view_literals;

#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

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
    std::wstring type;
    std::wstring name;
    std::optional<std::wstring> visualStateGroupName;
    int oneBasedIndex = 0;
    PropertyValuesMaybeUnresolved propertyValues;
};

struct StyleRule {
    std::wstring name;
    std::wstring visualState;
    std::wstring value;
    bool isXamlValue = false;
};

using PropertyOverridesUnresolved = std::vector<StyleRule>;

struct XamlBlurBrushParams {
    float blurAmount;
    winrt::Windows::UI::Color tint;
    std::optional<uint8_t> tintOpacity;
    std::wstring tintThemeResourceKey;  // Empty if not from ThemeResource
};

using PropertyOverrideValue =
    std::variant<winrt::Windows::Foundation::IInspectable, XamlBlurBrushParams>;

// Property -> visual state -> value.
using PropertyOverrides =
    std::unordered_map<DependencyProperty,
                       std::unordered_map<std::wstring, PropertyOverrideValue>>;

using PropertyOverridesMaybeUnresolved =
    std::variant<PropertyOverridesUnresolved, PropertyOverrides>;

struct ElementCustomizationRules {
    ElementMatcher elementMatcher;
    std::vector<ElementMatcher> parentElementMatchers;
    PropertyOverridesMaybeUnresolved propertyOverrides;
};

thread_local std::vector<ElementCustomizationRules>
    g_elementsCustomizationRules;

struct ElementPropertyCustomizationState {
    std::optional<winrt::Windows::Foundation::IInspectable> originalValue;
    std::optional<PropertyOverrideValue> customValue;
    int64_t propertyChangedToken = 0;
};

struct ElementCustomizationStateForVisualStateGroup {
    std::unordered_map<DependencyProperty, ElementPropertyCustomizationState>
        propertyCustomizationStates;
    winrt::event_token visualStateGroupCurrentStateChangedToken;
};

struct ElementCustomizationState {
    winrt::weak_ref<FrameworkElement> element;

    // Use list to avoid reallocations on insertion, as pointers to items are
    // captured in callbacks and stored.
    std::list<std::pair<std::optional<winrt::weak_ref<VisualStateGroup>>,
                        ElementCustomizationStateForVisualStateGroup>>
        perVisualStateGroup;
};

thread_local std::unordered_map<InstanceHandle, ElementCustomizationState>
    g_elementsCustomizationState;

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

struct ResourceVariableEntry {
    std::wstring key;
    std::wstring value;
    ResourceVariableTheme theme;
};

// Track original resource values for restoration (per-thread since
// Application::Current().Resources() is per-thread).
thread_local std::unordered_map<std::wstring,
                                winrt::Windows::Foundation::IInspectable>
    g_originalResourceValues;

// Track our merged theme dictionary for cleanup (per-thread).
thread_local ResourceDictionary g_resourceVariablesThemeDict{nullptr};

// Track theme resource entries that reference {ThemeResource ...} for refresh
// (per-thread).
thread_local std::vector<ResourceVariableEntry> g_themeResourceEntries;

// For listening to theme color changes (per-thread).
thread_local winrt::Windows::UI::ViewManagement::UISettings g_uiSettings{
    nullptr};
thread_local winrt::event_token g_colorValuesChangedToken;

winrt::Windows::Foundation::IInspectable ReadLocalValueWithWorkaround(
    DependencyObject elementDo,
    DependencyProperty property) {
    auto value = elementDo.ReadLocalValue(property);
    if (value) {
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
        // A workaround for Fill of HorizontalTrackRect which can't be read by
        // ReadLocalValue for some reason (null is returned instead).
        auto rect = elementDo.try_as<Shapes::Rectangle>();
        if (rect && rect.Name() == L"HorizontalTrackRect") {
            auto value2 = elementDo.GetValue(property);
            if (value2 && winrt::get_class_name(value2) ==
                              L"Windows.UI.Xaml.Media.SolidColorBrush") {
                Wh_Log(L"Using GetValue workaround for HorizontalTrackRect");
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

// Blur background implementation, copied from TranslucentTB.
////////////////////////////////////////////////////////////////////////////////
// clang-format off

#include <initguid.h>

#include <winrt/Windows.UI.Xaml.Hosting.h>

namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuxh = wux::Hosting;

template <> inline constexpr winrt::guid winrt::impl::guid_v<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>{
    winrt::impl::guid_v<winrt::Windows::Foundation::IPropertyValue>
};

#ifndef E_BOUNDS
#define E_BOUNDS (HRESULT)(0x8000000BL)
#endif

typedef enum MY_D2D1_GAUSSIANBLUR_OPTIMIZATION
{
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED = 0,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED = 1,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY = 2,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_FORCE_DWORD = 0xffffffff

} MY_D2D1_GAUSSIANBLUR_OPTIMIZATION;

////////////////////////////////////////////////////////////////////////////////
// XamlBlurBrush.h
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.ViewManagement.h>

class XamlBlurBrush : public wux::Media::XamlCompositionBrushBaseT<XamlBlurBrush, wux::Media::ISolidColorBrush>
{
public:
	XamlBlurBrush(wuc::Compositor compositor,
	              float blurAmount,
	              winrt::Windows::UI::Color tint,
	              std::optional<uint8_t> tintOpacity,
	              winrt::hstring tintThemeResourceKey);

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
	void OnThemeRefreshed();

	wuc::Compositor m_compositor;
	float m_blurAmount;
	winrt::Windows::UI::Color m_tint;
	std::optional<uint8_t> m_tintOpacity;
	winrt::hstring m_tintThemeResourceKey;
	winrt::Windows::UI::ViewManagement::UISettings m_uiSettings;
};

////////////////////////////////////////////////////////////////////////////////
// CompositeEffect.h
#include <d2d1effects.h>
#include <d2d1_1.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
// #include <windows.graphics.effects.interop.h>

#include <windows.graphics.effects.h>
#include <sdkddkver.h>

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



namespace awge = ABI::Windows::Graphics::Effects;

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
#include <d2d1effects.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.Effects.h>
// #include <windows.graphics.effects.interop.h>

namespace awge = ABI::Windows::Graphics::Effects;

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
// GaussianBlurEffect.h
#include <d2d1effects.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
// #include <windows.graphics.effects.interop.h>

namespace wge = winrt::Windows::Graphics::Effects;
namespace awge = ABI::Windows::Graphics::Effects;

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
// XamlBlurBrush.cpp
#include <winrt/Windows.System.h>

XamlBlurBrush::XamlBlurBrush(wuc::Compositor compositor,
                             float blurAmount,
                             winrt::Windows::UI::Color tint,
                             std::optional<uint8_t> tintOpacity,
                             winrt::hstring tintThemeResourceKey) :
	m_compositor(std::move(compositor)),
	m_blurAmount(blurAmount),
	m_tint(tint),
	m_tintOpacity(tintOpacity),
	m_tintThemeResourceKey(std::move(tintThemeResourceKey))
{
	if (!m_tintThemeResourceKey.empty())
	{
		RefreshThemeTint();

		auto dq = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();

		m_uiSettings.ColorValuesChanged([weakThis = get_weak(), dq] (auto const&, auto const&)
		{
			dq.TryEnqueue([weakThis]
			{
				if (auto self = weakThis.get())
				{
					self->OnThemeRefreshed();
				}
			});
		});
	}
}

void XamlBlurBrush::OnConnected()
{
	if (!CompositionBrush())
	{
		auto backdropBrush = m_compositor.CreateBackdropBrush();

		auto blurEffect = winrt::make_self<GaussianBlurEffect>();
		blurEffect->Source = wuc::CompositionEffectSourceParameter(L"backdrop");
		blurEffect->BlurAmount = m_blurAmount;

		auto floodEffect = winrt::make_self<FloodEffect>();
		floodEffect->Color = m_tint;

		auto compositeEffect = winrt::make_self<CompositeEffect>();
		compositeEffect->Sources.push_back(*blurEffect);
		compositeEffect->Sources.push_back(*floodEffect);
		compositeEffect->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;

		auto factory = m_compositor.CreateEffectFactory(
			*compositeEffect,
			// List of animatable properties.
			{L"FloodEffect.Color"}
		);
		auto blurBrush = factory.CreateBrush();
		blurBrush.SetSourceParameter(L"backdrop", backdropBrush);

		CompositionBrush(blurBrush);
	}
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
	if (m_tintThemeResourceKey.empty())
	{
		return;
	}

	auto resources = Application::Current().Resources();
	auto resource = resources.TryLookup(winrt::box_value(m_tintThemeResourceKey));
	if (!resource)
	{
		Wh_Log(L"Failed to find resource");
		return;
	}

	if (auto colorBrush = resource.try_as<wux::Media::SolidColorBrush>())
	{
		m_tint = colorBrush.Color();
	}
	else if (auto color = resource.try_as<winrt::Windows::UI::Color>())
	{
		m_tint = *color;
	}
	else
	{
		Wh_Log(L"Resource type is unsupported: %s",
			winrt::get_class_name(resource).c_str());
		return;
	}

	if (m_tintOpacity)
	{
		m_tint.A = *m_tintOpacity;
	}
}

void XamlBlurBrush::OnThemeRefreshed()
{
	Wh_Log(L"Theme refreshed");

	auto prevTint = m_tint;

	RefreshThemeTint();

	if (prevTint != m_tint)
	{
		if (auto effectBrush = CompositionBrush().try_as<wuc::CompositionEffectBrush>())
		{
			effectBrush.Properties().InsertColor(L"FloodEffect.Color", m_tint);
		}
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
            auto compositor =
                wuxh::ElementCompositionPreview::GetElementVisual(uiElement)
                    .Compositor();

            value = winrt::make<XamlBlurBrush>(
                std::move(compositor), blurBrushParams->blurAmount,
                blurBrushParams->tint, blurBrushParams->tintOpacity,
                winrt::hstring(blurBrushParams->tintThemeResourceKey));
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
        elementDo.ClearValue(property);
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
    std::wstring tintThemeResourceKey;
    winrt::Windows::UI::Color tint{};
    float tintOpacity = std::numeric_limits<float>::quiet_NaN();
    float blurAmount = 0;

    constexpr auto kTintColorThemeResourcePrefix =
        L"TintColor=\"{ThemeResource"sv;
    constexpr auto kTintColorThemeResourceSuffix = L"}\""sv;
    constexpr auto kTintColorPrefix = L"TintColor=\"#"sv;
    constexpr auto kTintOpacityPrefix = L"TintOpacity=\""sv;
    constexpr auto kBlurAmountPrefix = L"BlurAmount=\""sv;
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

        if (propSubstr == kTintColorThemeResourcePrefix) {
            pendingTintColorThemeResource = true;
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

        if (propSubstr.starts_with(kTintOpacityPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kTintOpacityPrefix),
                propSubstr.size() - std::size(kTintOpacityPrefix) - 1);
            tintOpacity = std::stof(std::wstring(valStr));
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

const PropertyOverrides& GetResolvedPropertyOverrides(
    const std::wstring_view type,
    const std::wstring_view fallbackType,
    PropertyOverridesMaybeUnresolved* propertyOverridesMaybeUnresolved) {
    if (const auto* resolved =
            std::get_if<PropertyOverrides>(propertyOverridesMaybeUnresolved)) {
        return *resolved;
    }

    PropertyOverrides propertyOverrides;

    try {
        const auto& styleRules = std::get<PropertyOverridesUnresolved>(
            *propertyOverridesMaybeUnresolved);
        if (!styleRules.empty()) {
            std::wstring xaml;

            std::vector<std::optional<PropertyOverrideValue>>
                propertyOverrideValues;
            propertyOverrideValues.reserve(styleRules.size());

            for (const auto& rule : styleRules) {
                propertyOverrideValues.push_back(
                    rule.isXamlValue
                        ? ParseNonXamlPropertyOverrideValue(rule.value)
                        : std::nullopt);

                xaml += L"        <Setter Property=\"";
                xaml += EscapeXmlAttribute(rule.name);
                xaml += L"\"";
                if (propertyOverrideValues.back() ||
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

            auto style = GetStyleFromXamlSettersWithFallbackType(
                type, fallbackType, xaml);

            uint32_t i = 0;
            for (const auto& rule : styleRules) {
                const auto setter = style.Setters().GetAt(i).as<Setter>();
                propertyOverrides[setter.Property()][rule.visualState] =
                    propertyOverrideValues[i].value_or(
                        rule.isXamlValue && rule.value.empty()
                            ? DependencyProperty::UnsetValue()
                            : setter.Value());
                i++;
            }
        }

        Wh_Log(L"%.*s: %zu override styles", static_cast<int>(type.length()),
               type.data(), propertyOverrides.size());
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const& ex) {
        Wh_Log(L"Error: %S", ex.what());
    }

    *propertyOverridesMaybeUnresolved = std::move(propertyOverrides);
    return std::get<PropertyOverrides>(*propertyOverridesMaybeUnresolved);
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
        }

        const auto className = winrt::get_class_name(value);
        const auto expectedClassName =
            winrt::get_class_name(propertyValue.second);
        if (className != expectedClassName) {
            Wh_Log(L"Different property class: %s vs. %s", className.c_str(),
                   expectedClassName.c_str());
            return false;
        }

        if (className == L"Windows.Foundation.IReference`1<String>") {
            if (winrt::unbox_value<winrt::hstring>(propertyValue.second) ==
                winrt::unbox_value<winrt::hstring>(value)) {
                continue;
            }

            return false;
        }

        if (className == L"Windows.Foundation.IReference`1<Double>") {
            if (winrt::unbox_value<double>(propertyValue.second) ==
                winrt::unbox_value<double>(value)) {
                continue;
            }

            return false;
        }

        if (className == L"Windows.Foundation.IReference`1<Boolean>") {
            if (winrt::unbox_value<bool>(propertyValue.second) ==
                winrt::unbox_value<bool>(value)) {
                continue;
            }

            return false;
        }

        Wh_Log(L"Unsupported property class: %s", className.c_str());
        return false;
    }

    if (matcher.visualStateGroupName && visualStateGroup) {
        *visualStateGroup =
            GetVisualStateGroup(element, *matcher.visualStateGroupName);
    }

    return true;
}

std::unordered_map<VisualStateGroup, PropertyOverrides>
FindElementPropertyOverrides(FrameworkElement element,
                             PCWSTR fallbackClassName) {
    std::unordered_map<VisualStateGroup, PropertyOverrides> overrides;
    std::unordered_set<DependencyProperty> propertiesAdded;

    for (auto it = g_elementsCustomizationRules.rbegin();
         it != g_elementsCustomizationRules.rend(); ++it) {
        auto& override = *it;

        VisualStateGroup visualStateGroup = nullptr;

        if (!TestElementMatcher(element, override.elementMatcher,
                                &visualStateGroup, fallbackClassName)) {
            continue;
        }

        auto parentElementIter = element;
        bool parentElementMatchFailed = false;

        for (auto& matcher : override.parentElementMatchers) {
            // Using parentElementIter.Parent() was sometimes returning null.
            parentElementIter =
                Media::VisualTreeHelper::GetParent(parentElementIter)
                    .try_as<FrameworkElement>();
            if (!parentElementIter) {
                parentElementMatchFailed = true;
                break;
            }

            if (!TestElementMatcher(parentElementIter, matcher,
                                    &visualStateGroup, nullptr)) {
                parentElementMatchFailed = true;
                break;
            }
        }

        if (parentElementMatchFailed) {
            continue;
        }

        auto& overridesForVisualStateGroup = overrides[visualStateGroup];
        for (const auto& [property, valuesPerVisualState] :
             GetResolvedPropertyOverrides(
                 override.elementMatcher.type,
                 fallbackClassName ? fallbackClassName
                                   : winrt::name_of<FrameworkElement>(),
                 &override.propertyOverrides)) {
            bool propertyInserted = propertiesAdded.insert(property).second;
            if (!propertyInserted) {
                continue;
            }

            auto& propertyOverrides = overridesForVisualStateGroup[property];
            for (const auto& [visualState, value] : valuesPerVisualState) {
                propertyOverrides.insert({visualState, value});
            }
        }
    }

    std::erase_if(overrides, [](const auto& item) {
        auto const& [key, value] = item;
        return value.empty();
    });

    return overrides;
}

void ApplyCustomizationsForVisualStateGroup(
    FrameworkElement element,
    VisualStateGroup visualStateGroup,
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
            propertyCustomizationState.originalValue =
                ReadLocalValueWithWorkaround(element, property);
            propertyCustomizationState.customValue = it->second;
            SetOrClearValue(element, property, it->second);
        }

        propertyCustomizationState
            .propertyChangedToken = elementDo.RegisterPropertyChangedCallback(
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

                Wh_Log(L"Re-applying style for %s",
                       winrt::get_class_name(element).c_str());

                auto localValue =
                    ReadLocalValueWithWorkaround(element, property);

                if (auto* customValue =
                        std::get_if<winrt::Windows::Foundation::IInspectable>(
                            &*propertyCustomizationState.customValue)) {
                    if (*customValue != localValue) {
                        propertyCustomizationState.originalValue = localValue;
                    }
                }

                g_elementPropertyModifying = true;
                SetOrClearValue(element, property,
                                *propertyCustomizationState.customValue);
                g_elementPropertyModifying = false;
            });
    }

    if (visualStateGroup) {
        winrt::weak_ref<FrameworkElement> elementWeakRef = element;
        elementCustomizationStateForVisualStateGroup
            ->visualStateGroupCurrentStateChangedToken =
            visualStateGroup.CurrentStateChanged(
                [elementWeakRef, propertyOverrides,
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
                            if (!propertyCustomizationState.originalValue) {
                                propertyCustomizationState.originalValue =
                                    ReadLocalValueWithWorkaround(element,
                                                                 property);
                            }

                            propertyCustomizationState.customValue = it->second;
                            SetOrClearValue(element, property, it->second);
                        } else {
                            if (propertyCustomizationState.originalValue) {
                                SetOrClearValue(
                                    element, property,
                                    *propertyCustomizationState.originalValue);
                                propertyCustomizationState.originalValue
                                    .reset();
                            }

                            propertyCustomizationState.customValue.reset();
                        }
                    }

                    g_elementPropertyModifying = false;
                });
    }
}

void RestoreCustomizationsForVisualStateGroup(
    FrameworkElement element,
    std::optional<winrt::weak_ref<VisualStateGroup>>
        visualStateGroupOptionalWeakPtr,
    const ElementCustomizationStateForVisualStateGroup&
        elementCustomizationStateForVisualStateGroup) {
    if (element) {
        for (const auto& [property, state] :
             elementCustomizationStateForVisualStateGroup
                 .propertyCustomizationStates) {
            element.UnregisterPropertyChangedCallback(
                property, state.propertyChangedToken);

            if (state.originalValue) {
                SetOrClearValue(element, property, *state.originalValue);
            }
        }
    }

    auto visualStateGroupIter = visualStateGroupOptionalWeakPtr
                                    ? visualStateGroupOptionalWeakPtr->get()
                                    : nullptr;
    if (visualStateGroupIter && elementCustomizationStateForVisualStateGroup
                                    .visualStateGroupCurrentStateChangedToken) {
        visualStateGroupIter.CurrentStateChanged(
            elementCustomizationStateForVisualStateGroup
                .visualStateGroupCurrentStateChangedToken);
    }
}

void ApplyCustomizations(InstanceHandle handle,
                         FrameworkElement element,
                         PCWSTR fallbackClassName) {
    auto overrides = FindElementPropertyOverrides(element, fallbackClassName);
    if (overrides.empty()) {
        return;
    }

    Wh_Log(L"Applying styles");

    auto& elementCustomizationState = g_elementsCustomizationState[handle];

    for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
        RestoreCustomizationsForVisualStateGroup(
            element, visualStateGroupOptionalWeakPtrIter, stateIter);
    }

    elementCustomizationState.element = element;
    elementCustomizationState.perVisualStateGroup.clear();

    for (auto& [visualStateGroup, overridesForVisualStateGroup] : overrides) {
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
            element, visualStateGroup, std::move(overridesForVisualStateGroup),
            elementCustomizationStateForVisualStateGroup);
    }
}

void CleanupCustomizations(InstanceHandle handle) {
    if (auto it = g_elementsCustomizationState.find(handle);
        it != g_elementsCustomizationState.end()) {
        auto& elementCustomizationState = it->second;

        auto element = elementCustomizationState.element.get();

        for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
             elementCustomizationState.perVisualStateGroup) {
            RestoreCustomizationsForVisualStateGroup(
                element, visualStateGroupOptionalWeakPtrIter, stateIter);
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

StyleRule StyleRuleFromString(std::wstring_view str) {
    StyleRule result;

    auto eqPos = str.find(L'=');
    if (eqPos == str.npos) {
        throw std::runtime_error("Bad style syntax, '=' is missing");
    }

    auto name = str.substr(0, eqPos);
    auto value = str.substr(eqPos + 1);

    result.value = TrimStringView(value);

    if (name.size() > 0 && name.back() == L':') {
        result.isXamlValue = true;
        name = name.substr(0, name.size() - 1);
    }

    auto atPos = name.find(L'@');
    if (atPos != name.npos) {
        result.visualState = TrimStringView(name.substr(atPos + 1));
        name = name.substr(0, atPos);
    }

    result.name = TrimStringView(name);
    if (result.name.empty()) {
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

        auto matcher = ElementMatcherFromString(targetPart);
        matcher.type = AdjustTypeName(matcher.type);

        if (matcher.visualStateGroupName) {
            if (hasVisualStateGroup) {
                throw std::runtime_error(
                    "Element type can't have more than one visual state group");
            }

            hasVisualStateGroup = true;
        }

        if (first) {
            std::vector<StyleRule> styleRules;
            for (const auto& style : styles) {
                styleRules.push_back(StyleRuleFromString(style));
            }

            elementCustomizationRules.elementMatcher = std::move(matcher);
            elementCustomizationRules.propertyOverrides = std::move(styleRules);
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

    return ResourceVariableEntry{std::move(key), std::move(value), theme};
}

constexpr std::wstring_view kThemeResourcePrefix = L"{ThemeResource ";

bool IsThemeResourceReference(std::wstring_view value) {
    return value.starts_with(kThemeResourcePrefix) && value.ends_with(L"}");
}

winrt::Windows::Foundation::IInspectable ResolveResourceVariableValue(
    ResourceDictionary resources,
    std::wstring_view value) {
    // Check for {ThemeResource X} syntax - look up the resource directly
    // to preserve dynamic theme-aware behavior.
    if (IsThemeResourceReference(value)) {
        auto resourceKey =
            value.substr(kThemeResourcePrefix.size(),
                         value.size() - kThemeResourcePrefix.size() - 1);
        return resources.Lookup(
            winrt::box_value(winrt::hstring(TrimStringView(resourceKey))));
    }

    // For other values, use boxed string (works for colors, etc.).
    return winrt::box_value(winrt::hstring(value));
}

// Returns true if a theme resource was added.
bool ProcessResourceVariableFromSetting(ResourceDictionary resources,
                                        ResourceDictionary darkDict,
                                        ResourceDictionary lightDict,
                                        const ResourceVariableEntry& entry) {
    auto boxedKey = winrt::box_value(entry.key);

    if (entry.theme != ResourceVariableTheme::None) {
        // Key@Dark= or Key@Light= - add to theme dict.
        auto value = ResolveResourceVariableValue(resources, entry.value);
        if (entry.theme == ResourceVariableTheme::Dark) {
            darkDict.Insert(boxedKey, value);
        } else {
            lightDict.Insert(boxedKey, value);
        }
        return true;
    }

    // key= - convert using existing resource type.
    auto existingResource = resources.TryLookup(boxedKey);
    if (!existingResource) {
        Wh_Log(L"Resource variable key '%s' not found, skipping",
               entry.key.c_str());
        return false;
    }

    if (!g_originalResourceValues.contains(entry.key)) {
        g_originalResourceValues[entry.key] = existingResource;
    }

    auto resourceClassName = winrt::get_class_name(existingResource);

    // Unwrap IReference<T> to get inner type name.
    if (resourceClassName.starts_with(L"Windows.Foundation.IReference`1<") &&
        resourceClassName.ends_with(L'>')) {
        size_t prefixSize = sizeof("Windows.Foundation.IReference`1<") - 1;
        resourceClassName =
            winrt::hstring(resourceClassName.data() + prefixSize,
                           resourceClassName.size() - prefixSize - 1);
    }

    resources.Insert(boxedKey, Markup::XamlBindingHelper::ConvertValue(
                                   Interop::TypeName{resourceClassName},
                                   winrt::box_value(entry.value)));
    return false;
}

void RefreshThemeResourceEntries() {
    if (g_themeResourceEntries.empty()) {
        return;
    }

    Wh_Log(L"Refreshing %zu theme resource entries",
           g_themeResourceEntries.size());

    auto resources = Application::Current().Resources();

    auto darkDict = g_resourceVariablesThemeDict.ThemeDictionaries()
                        .TryLookup(winrt::box_value(L"Dark"))
                        .try_as<ResourceDictionary>();
    auto lightDict = g_resourceVariablesThemeDict.ThemeDictionaries()
                         .TryLookup(winrt::box_value(L"Light"))
                         .try_as<ResourceDictionary>();

    for (const auto& entry : g_themeResourceEntries) {
        try {
            auto boxedKey = winrt::box_value(entry.key);
            auto value = ResolveResourceVariableValue(resources, entry.value);

            if (entry.theme == ResourceVariableTheme::Dark && darkDict) {
                darkDict.Insert(boxedKey, value);
            } else if (entry.theme == ResourceVariableTheme::Light &&
                       lightDict) {
                lightDict.Insert(boxedKey, value);
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error refreshing '%s': %08X", entry.key.c_str(),
                   ex.code());
        }
    }
}

void ProcessResourceVariablesFromSettings() {
    StyleConstants styleConstants = LoadStyleConstants(std::vector<PCWSTR>{});

    auto resources = Application::Current().Resources();

    // Create theme dictionaries for @Dark/@Light resources.
    g_resourceVariablesThemeDict = ResourceDictionary();
    ResourceDictionary darkDict;
    ResourceDictionary lightDict;
    bool hasThemeResources = false;

    for (int i = 0;; i++) {
        string_setting_unique_ptr setting(
            Wh_GetStringSetting(L"themeResourceVariables[%d]", i));
        if (!*setting.get()) {
            break;
        }

        Wh_Log(L"Processing theme resource variable %s", setting.get());

        auto parsed = ParseResourceVariable(setting.get(), styleConstants);
        if (!parsed) {
            continue;
        }

        try {
            if (ProcessResourceVariableFromSetting(resources, darkDict,
                                                   lightDict, *parsed)) {
                hasThemeResources = true;

                // Track entries with {ThemeResource ...} for refresh on color
                // change.
                if (IsThemeResourceReference(parsed->value)) {
                    g_themeResourceEntries.push_back(*parsed);
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
    if (!g_themeResourceEntries.empty()) {
        g_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
        auto dispatcherQueue =
            winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        g_colorValuesChangedToken =
            g_uiSettings.ColorValuesChanged([dispatcherQueue](auto&&, auto&&) {
                dispatcherQueue.TryEnqueue(
                    []() { RefreshThemeResourceEntries(); });
            });
    }
}

void UninitializeResourceVariables() {
    // Unregister color change handler.
    if (g_colorValuesChangedToken) {
        g_uiSettings.ColorValuesChanged(g_colorValuesChangedToken);
        g_colorValuesChangedToken = {};
    }
    g_uiSettings = nullptr;
    g_themeResourceEntries.clear();

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

        for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
             elementCustomizationState.perVisualStateGroup) {
            RestoreCustomizationsForVisualStateGroup(
                element, visualStateGroupOptionalWeakPtrIter, stateIter);
        }
    }

    g_elementsCustomizationState.clear();

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
    ProcessResourceVariablesFromSettings();

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
        L"releases/download/stats-v3/";

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
            break;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
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
