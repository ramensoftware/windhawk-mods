// ==WindhawkMod==
// @id              windows-11-taskbar-styler
// @name            Windows 11 Taskbar Styler
// @description     Customize the taskbar with themes contributed by others or create your own
// @version         1.5
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
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
# Windows 11 Taskbar Styler

Customize the taskbar with themes contributed by others or create your own.

Also check out the **Windows 11 Start Menu Styler**, **Windows 11 Notification
Center Styler** mods.

## Themes

Themes are collections of styles. The following themes are integrated into the
mod and can be selected in the settings:

[![WinXP](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/WinXP/screenshot-small.png)
\
WinXP](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/WinXP/README.md)

[![Bubbles](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/Bubbles/screenshot.png)
\
Bubbles](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/Bubbles/README.md)

[![TranslucentTaskbar](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/TranslucentTaskbar/screenshot.png)
\
TranslucentTaskbar](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/TranslucentTaskbar/README.md)

[![Squircle](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/Squircle/screenshot.png)
\
Squircle](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/Squircle/README.md)

[![RosePine](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/RosePine/screenshot.png)
\
RosePine](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/RosePine/README.md)

[![DockLike](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/DockLike/screenshot.png)
\
DockLike](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/DockLike/README.md)

[![WinVista](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/WinVista/screenshot.png)
\
WinVista](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/WinVista/README.md)

[![CleanSlate](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/CleanSlate/screenshot.png)
\
CleanSlate](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/CleanSlate/README.md)

[![Lucent](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/Lucent/screenshot.png)
\
Lucent](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/Lucent/README.md)

[![21996Taskbar](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/21996Taskbar/screenshot.png)
\
21996Taskbar](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/21996Taskbar/README.md)

[![BottomDensy](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/BottomDensy/screenshot.png)
\
BottomDensy](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/BottomDensy/README.md)

[![TaskbarXII](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/TaskbarXII/screenshot.png)
\
TaskbarXII](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/TaskbarXII/README.md)

[![xdark](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/xdark/screenshot.png)
\
xdark](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/xdark/README.md)

[![Matter](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/Matter/screenshot.png)
\
Matter](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/Matter/README.md)

[![SimplyTransparent](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/SimplyTransparent/screenshot.png)
\
SimplyTransparent](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/SimplyTransparent/README.md)

[![Windows7](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/Windows7/screenshot.png)
\
Windows7](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/Windows7/README.md)

[![Aeris](https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/main/Themes/Aeris/screenshot.png)
\
Aeris](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/Themes/Aeris/README.md)

More themes can be found in the **Themes** section of [The Windows 11 taskbar
styling
guide](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/README.md#themes).
Contributions of new themes are welcome!

## Advanced styling

Aside from themes, the settings have two sections: control styles and resource
variables. Control styles allow to override styles, such as size and color, for
the target elements. Resource variables allow to override predefined variables.
For a more detailed explanation and examples, refer to the sections below.

The taskbar's XAML resources can help find out which elements and resource
variables can be customized. To the best of my knowledge, there are no public
tools that are able to decode the resource files of recent Windows versions, but
here are XAML resources which were obtained via other means for your
convenience: [TaskbarResources.xbf and
SystemTrayResources.xbf](https://gist.github.com/m417z/ad0ab39351aca905f1d186b1f1c3d8c7).

The [UWPSpy](https://ramensoftware.com/uwpspy) tool can be used to inspect the
taskbar's control elements in real time, and experiment with various styles.

For a collection of commonly requested taskbar styling customizations, check out
[The Windows 11 taskbar styling
guide](https://github.com/ramensoftware/windows-11-taskbar-styling-guide/blob/main/README.md).

### Control styles

Each entry has a target control and a list of styles.

The target control is written as `Class` or `Class#Name`, i.e. the target
control class name (the tag name in XAML resource files), such as
`Taskbar.TaskListButton` or `Rectangle`, optionally followed by `#` and the
target control's name (`x:Name` attribute in XAML resource files). The target
control can also include:
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
`BlurAmount` and `TintColor` properties. For example: `Fill:=<WindhawkBlur
BlurAmount="10" TintColor="#80FF00FF"/>`.

Targets and styles starting with two slashes (`//`) are ignored. This can be
useful for temporarily disabling a target or style.

A couple of practical examples:

#### Task list button corner radius

![Screenshot](https://i.imgur.com/zDATi9K.png)

* Target: `Taskbar.TaskListButton`
* Style: `CornerRadius=0`

#### Running indicator size and color

![Screenshot](https://i.imgur.com/mR5c3F5.png)

* Target: `Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates >
  Rectangle#RunningIndicator`
* Styles:
    * `Fill=#FFED7014`
    * `Height=2`
    * `Width=12`
    * `Fill@ActiveRunningIndicator=Red`
    * `Width@ActiveRunningIndicator=20`

#### Task list button background gradient

![Screenshot](https://i.imgur.com/LNPcw0G.png)

* Targets:
    * `Taskbar.TaskListButtonPanel > Border#BackgroundElement`
    * `Taskbar.TaskListLabeledButtonPanel > Border#BackgroundElement`
* Style: `Background:=<LinearGradientBrush StartPoint="0.5,0"
  EndPoint="0.5,1"><GradientStop Offset="0" Color="DodgerBlue"/><GradientStop
  Offset="1" Color="Yellow"/></LinearGradientBrush>`

#### Hide the start button

* Target:
  `Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]`
* Style: `Visibility=Collapsed`

#### Hide the network notification icon

* Target: `SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter >
  ItemsPresenter > StackPanel > ContentPresenter[1] > SystemTray.IconView >
  Grid > Grid`
* Style: `Visibility=Collapsed`

**Note**: To hide the volume notification icon instead, use `[2]` instead of
`[1]`.

### Resource variables

Some variables, such as size and padding for various controls, are defined as
resource variables. Here are several examples:

* `TaskbarContextMenuMargin`: The margin between the taskbar and the start
  button context menu.

* `ContextMenuMargin`: The margin between the taskbar and the tray area context
  menu.

* `MediumTaskbarButtonExtent`: The width of the taskbar buttons.

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
  - WinXP: WinXP
  - WinXP_variant_Zune: WinXP (Zune)
  - Bubbles: Bubbles
  - TranslucentTaskbar: TranslucentTaskbar
  - Squircle: Squircle
  - RosePine: RosePine
  - DockLike: DockLike
  - WinVista: WinVista
  - CleanSlate: CleanSlate
  - Lucent: Lucent (Accented Bar)
  - Lucent_variant_Light: Lucent (Light Bar)
  - 21996Taskbar: 21996Taskbar
  - BottomDensy: BottomDensy
  - TaskbarXII: TaskbarXII
  - xdark: xdark
  - Matter: Matter
  - SimplyTransparent: SimplyTransparent
  - Windows7: Windows7
  - Aeris: Aeris
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
- resourceVariables:
  - - variableKey: ""
      $name: Variable key
    - value: ""
      $name: Value
  $name: Resource variables
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

const Theme g_themeWinXP = {{
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#3168d5\" Offset=\"0.0\" /> <GradientStop Color=\"#4993E6\" Offset=\"0.1\" /> <GradientStop Color=\"#2157D7\" Offset=\"0.35\" /> <GradientStop Color=\"#2663E0\" Offset=\"0.8\" /> <GradientStop Color=\"#1941A5\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"VerticalAlignment=Stretch",
        L"Height=Auto"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#3168d5\" Offset=\"0.0\" /> <GradientStop Color=\"#4993E6\" Offset=\"0.1\" /> <GradientStop Color=\"#2157D7\" Offset=\"0.35\" /> <GradientStop Color=\"#2663E0\" Offset=\"0.8\" /> <GradientStop Color=\"#1941A5\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"CornerRadius=0",
        L"Margin=0,0,4,0",
        L"MaxWidth=48"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"Padding=0",
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#388238\" Offset=\"0.0\" /> <GradientStop Color=\"#71B571\" Offset=\"0.1\" /> <GradientStop Color=\"#71B571\" Offset=\"0.35\" /> <GradientStop Color=\"#47AA47\" Offset=\"0.8\" /> <GradientStop Color=\"#307443\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"MaxWidth=48"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Border#BackgroundElement", {
        L"Background:=<ImageBrush Stretch=\"None\" ImageSource=\"https://i.imgur.com/BvXJlkj.png\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background@NoRunningIndicator=Transparent",
        L"Background@ActiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#1B67D7\" Offset=\"0.0\" /> <GradientStop Color=\"#1542A8\" Offset=\"0.1\" /> <GradientStop Color=\"#1951BA\" Offset=\"0.15\" /> <GradientStop Color=\"#1951BA\" Offset=\"0.95\" /> <GradientStop Color=\"#1542A8\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#3358B5\" Offset=\"0.0\" /> <GradientStop Color=\"#8AC4FD\" Offset=\"0.1\" /> <GradientStop Color=\"#56A3FF\" Offset=\"0.2\" /> <GradientStop Color=\"#56A3FF\" Offset=\"0.85\" /> <GradientStop Color=\"#378DF6\" Offset=\"0.9\" /> <GradientStop Color=\"#163E95\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderThickness@ActiveRunningIndicator=1",
        L"BorderBrush@NoRunningIndicator=Transparent",
        L"BorderBrush@ActiveRunningIndicator=#1B67D7",
        L"BorderBrush=#3358B5"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"Margin=-1.5"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#16ADF0\" Offset=\"0.0\" /> <GradientStop Color=\"#19B9F3\" Offset=\"0.1\" /> <GradientStop Color=\"#118FE9\" Offset=\"0.35\" /> <GradientStop Color=\"#0E9EF0\" Offset=\"0.8\" /> <GradientStop Color=\"#1580D9\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderThickness=1,1,0,1",
        L"BorderBrush=#095BC9",
        L"Padding=4,0,0,0"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#3168d5\" Offset=\"0.0\" /> <GradientStop Color=\"#4993E6\" Offset=\"0.1\" /> <GradientStop Color=\"#2157D7\" Offset=\"0.35\" /> <GradientStop Color=\"#2663E0\" Offset=\"0.8\" /> <GradientStop Color=\"#1941A5\" Offset=\"1.0\" /></LinearGradientBrush>"}},
}};

const Theme g_themeWinXP_variant_Zune = {{
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0.5,0.5\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#656565\" Offset=\"0.0\" /> <GradientStop Color=\"#363636\" Offset=\"0.1\" /> <GradientStop Color=\"#363636\" Offset=\"0.35\" /> <GradientStop Color=\"#363636\" Offset=\"0.8\" /> <GradientStop Color=\"#363636\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"VerticalAlignment=Stretch",
        L"Height=Auto"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"CornerRadius=0",
        L"Margin=-4,0,4,0",
        L"MaxWidth=48"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"Padding=0",
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0.5\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#D76A27\" Offset=\"0.05\" /> <GradientStop Color=\"#B44704\" Offset=\"0.1\" /> <GradientStop Color=\"#772E01\" Offset=\"0.5\" /> <GradientStop Color=\"#772E01\" Offset=\"1\" /> <GradientStop Color=\"#AA4201\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Border#BackgroundElement", {
        L"Background:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"https://i.imgur.com/BvXJlkj.png\" />",
        L"Height=32"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border", {
        L"BorderThickness=1",
        L"CornerRadius=2",
        L"BorderBrush@NoRunningIndicator=Transparent",
        L"Margin=-2,-1,-2,-1"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"BorderBrush=#BB4B4B4B",
        L"Margin=1",
        L"BorderThickness=1",
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0.42\" EndPoint=\"0.5,0.75\"> <GradientStop Color=\"#6B6B6B\" Offset=\"0.0\" /> <GradientStop Color=\"#363636\" Offset=\"0.5\" /> <GradientStop Color=\"#363636\" Offset=\"0.35\" /> <GradientStop Color=\"#363636\" Offset=\"0.8\" /> <GradientStop Color=\"#363636\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderBrush@NoRunningIndicator=Transparent",
        L"Background@NoRunningIndicator=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=#858585"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0.42\" EndPoint=\"0.5,0.75\"> <GradientStop Color=\"#454545\" Offset=\"0.0\" /> <GradientStop Color=\"#313131\" Offset=\"0.5\" /> <GradientStop Color=\"#363636\" Offset=\"0.35\" /> <GradientStop Color=\"#1D1D1D\" Offset=\"0.8\" /> <GradientStop Color=\"#1D1D1D\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderThickness=1,0,0,0",
        L"BorderBrush=#222222",
        L"Padding=4,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel@RunningIndicatorStates > Windows.UI.Xaml.Controls.Image#Icon", {
        L"Height@NoRunningIndicator=16"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel@RunningIndicatorStates", {
        L"Margin@NoRunningIndicator=-7,0,-7,0",
        L"Padding@NoRunningIndicator=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"Margin=-1.5"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0.5\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#656565\" Offset=\"0.0\" /> <GradientStop Color=\"#363636\" Offset=\"0.1\" /> <GradientStop Color=\"#363636\" Offset=\"0.35\" /> <GradientStop Color=\"#363636\" Offset=\"0.8\" /> <GradientStop Color=\"#363636\" Offset=\"1.0\" /></LinearGradientBrush>"}},
}};

const Theme g_themeBubbles = {{
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill:=<SolidColorBrush x:Name=\"SystemChromeLow\" Color=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"CornerRadius=20",
        L"Background@NoRunningIndicator:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.18\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Background:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"BorderThickness=1.5",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.25\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"BorderThickness@NoRunningIndicator=1",
        L"BorderBrush@NoRunningIndicator:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"Margin=1"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.3\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.6\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Background@ActivePointerOver:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.8\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Background@InactivePointerOver:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.8\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Background@ActivePressed:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"1\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Background@InactivePressed:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"1\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderBrush@InactivePressed:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.8\" Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=20",
        L"BorderThickness@InactivePressed=3",
        L"BorderThickness=2"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.6\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"CornerRadius=20",
        L"Margin=-5,5,8,5",
        L"Padding=10,0,-10,0",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.9\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderThickness=1.5"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle#RunningIndicator", {
        L"Stroke@InactivePointerOver=#75A8E6",
        L"Stroke@InactivePressed=#7CB1F2",
        L"Stroke@ActiveNormal=#5F87B9",
        L"Stroke@ActivePointerOver=#75A8E6",
        L"Stroke@ActivePressed=#7CB1F2",
        L"Fill=Transparent",
        L"RadiusX=20",
        L"RadiusY=20",
        L"StrokeThickness=3",
        L"Stroke@MultiWindowPointerOver=#CCCCDD",
        L"Stroke@MultiWindowPressed=White",
        L"Stroke@MultiWindowActive=#BBBBCC",
        L"Fill@MultiWindowNormal=#88AAAABB",
        L"Fill@MultiWindowPointerOver=#88AAAABB",
        L"Fill@MultiWindowActive=#88AAAABB",
        L"Fill@MultiWindowPressed=#88AAAABB",
        L"Stroke@RequestingAttention=Crimson",
        L"Stroke@RequestingAttentionPointerOver=Red",
        L"Fill@RequestingAttention:=<SolidColorBrush Opacity=\"0.4\" Color=\"DarkOrange\" />",
        L"Fill@RequestingAttentionPointerOver:=<SolidColorBrush Opacity=\"0.4\" Color=\"Orange\" />",
        L"StrokeThickness@RequestingAttention=2.5",
        L"StrokeThickness@RequestingAttentionPointerOver=2.5",
        L"Height=39",
        L"Width=39",
        L"MinWidth=Auto"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=4,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Height=48",
        L"Margin=0,-2,0,0"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Height=0"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.8\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.1\" Color=\"{ThemeResource SearchPillButtonForeground}\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Margin=1,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Padding=5,0,5,0",
        L"Margin=2,0,10,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ShowDesktopPipe", {
        L"MinWidth=4",
        L"RadiusX=2",
        L"RadiusY=2",
        L"Margin=-5,0,5,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NotifyIconStack > Windows.UI.Xaml.Controls.Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=16,5,5,16",
        L"Margin=-3,4,0,4"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock", {
        L"Foreground:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Color=\"{ThemeResource SearchPillButtonForeground}\" />"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"Foreground:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Color=\"{ThemeResource SearchPillButtonForeground}\" />"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"Margin=0,2.5,0,-2.5"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"Margin=0,-1,0,2"}},
    ThemeTargetStyles{L"Grid#ContainerGrid@ > Border#BackgroundBorder ", {
        L"Background@PointerOver:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.2\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"CornerRadius=20",
        L"Margin=-1",
        L"Height=28",
        L"Width=28",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"SystemTray.IconView > Grid#ContainerGrid@ > Border#BackgroundBorder", {
        L"CornerRadius=20",
        L"Background@PointerOver:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"Height=34",
        L"Width=34"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"CornerRadius=20",
        L"Width=75",
        L"Margin=-2,1,-2,1",
        L"Background@PointerOver:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"CornerRadius=20",
        L"Background@PointerOver:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"Margin=1"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton > Taskbar.TaskListButtonPanel@CommonStates ", {
        L"Background:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.6\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.9\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderThickness=1.5",
        L"Margin=5",
        L"Background@InactivePointerOver:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"1\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Padding=-1.5,-1,-1.5,-1",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid ", {
        L"Background:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.5\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Padding=0",
        L"Margin=0,0,0,12",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Rectangle#LeftDropInsertionMarker", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />"}},
    ThemeTargetStyles{L"Rectangle#RightDropInsertionMarker", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton > Taskbar.TaskListButtonPanel > Border", {
        L"CornerRadius=20",
        L"Background:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.25\" Color=\"{ThemeResource SearchPillButtonForeground}\" />"}},
}};

const Theme g_themeTranslucentTaskbar = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=$CommonBgBrush"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill=$CommonBgBrush"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=14",
        L"Padding=3,4,3,4"}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"Background=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=15",
        L"Margin=-2,-2,-2,-2"}},
}, {
    L"CommonBgBrush=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#25323232\"/>",
}};

const Theme g_themeSquircle = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill=#CC222222"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=5",
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\" />",
        L"Background@InactivePointerOver:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />",
        L"Background@ActivePointerOver:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.9\" FallbackColor=\"#CC222222\" />",
        L"Background@ActiveNormal:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />",
        L"Background@InactiveNormal:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.7\" FallbackColor=\"#BB222222\" />",
        L"Background@InactivePressed:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />",
        L"Background@ActivePressed:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\"/>",
        L"CornerRadius=5",
        L"Margin=0,5,14,5",
        L"Padding=10,0,0,0"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Fill=Transparent",
        L"RadiusX=5",
        L"RadiusY=5",
        L"Height=38",
        L"Width=40"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=4,0,0,0",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Foreground=White",
        L"Margin=-11,0,0,0"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"FontSize=12",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton[AutomationProperties.Name=Copilot] > Taskbar.TaskListLabeledButtonPanel#IconPanel > Border#BackgroundElement", {
        L"Background:=<AcrylicBrush TintColor=\"Red\" TintOpacity=\"0.8\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"Margin=0,3,0,3",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement@CommonStates", {
        L"Background@InactivePointerOver:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0\" />",
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\" />"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Border#BackgroundElement", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton", {
        L"Margin=-11,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.Name=Task View]", {
        L"Margin=-12,0,0,0"}},
    ThemeTargetStyles{L"taskbar:TaskListLabeledButtonPanel@RunningIndicatorStates > Border", {
        L"Background@ActiveRunningIndicator:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" />",
        L"Background@InactiveRunningIndicator:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" />",
        L"Background@InactiveRunningIndicatorPointerOver:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background@InactivePointerOver:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#DD222222\"/>",
        L"Background@ActivePointerOver:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#EE222222\"/>",
        L"Background@InactiveNormal:=<AcrylicBrush TintOpacity=\"0.2\" TintColor=\"Black\" FallbackColor=\"#BB222222\"/>",
        L"Background@ActiveNormal:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#CC222222\"/>",
        L"Background@ActivePressed:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"#333333\" FallbackColor=\"#BB333333\" />",
        L"Background@InactivePressed:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"#333333\" FallbackColor=\"#BB333333\" />",
        L"CornerRadius=5",
        L"Margin=1"}},
}};

const Theme g_themeSquircle_variant_WeatherOnTheRight = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill=#CC222222"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=5",
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\" />",
        L"Background@InactivePointerOver:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />",
        L"Background@ActivePointerOver:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />",
        L"Background@ActiveNormal:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />",
        L"Background@InactivePressed:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />",
        L"Background@ActivePressed:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\" />",
        L"CornerRadius=5",
        L"Margin=0,5,12,5",
        L"Padding=10,0,0,0"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Fill=Transparent",
        L"RadiusX=5",
        L"RadiusY=5",
        L"Height=38",
        L"Width=40"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=4,0,0,0",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Foreground=White",
        L"Margin=0,0,-10,0"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"FontSize=12",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton[AutomationProperties.Name=Copilot]", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" />",
        L"Margin=12,0,12,0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"Margin=0,3,0,3",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement@CommonStates", {
        L"Background@InactivePointerOver:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0\" />",
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\"/>",
        L"Width=125"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\" />"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Border#BackgroundElement", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton", {
        L"Margin=20,1,-20,1"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton", {
        L"Margin=0,0,-11,0",
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"Margin=12,0,-10,0"}},
    ThemeTargetStyles{L"Grid#AugmentedEntryPointContentGrid", {
        L"Margin=10,0,-5,0"}},
    ThemeTargetStyles{L"taskbar:TaskListLabeledButtonPanel@RunningIndicatorStates > Border", {
        L"Background@ActiveRunningIndicator:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" />",
        L"Background@InactiveRunningIndicator:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" />",
        L"Background@InactiveRunningIndicatorPointerOver:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" />"}},
    ThemeTargetStyles{L"taskbar:TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background@InactivePointerOver:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#DD222222\" />",
        L"Background@ActivePointerOver:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#EE222222\" />",
        L"Background@InactiveNormal:=<AcrylicBrush TintOpacity=\"0.2\" TintColor=\"Black\" FallbackColor=\"#BB222222\" />",
        L"Background@ActiveNormal:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#CC222222\" />",
        L"Background@ActivePressed:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"#333333\" FallbackColor=\"#BB333333\" />",
        L"Background@InactivePressed:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"#333333\" FallbackColor=\"#BB333333\" />",
        L"CornerRadius=5",
        L"Margin=1"}},
}};

const Theme g_themeRosePine = {{
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"CornerRadius=3"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"MinWidth=25"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[1] > SystemTray.IconView > Grid > Grid", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"Padding=2"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"MinWidth=27"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#BackgroundElement", {
        L"Background:=#302d47",
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=#302d47",
        L"CornerRadius=6",
        L"Margin=0,5,4,4",
        L"Padding=3,0,-8,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle#RunningIndicator", {
        L"Height=27",
        L"RadiusX=5",
        L"RadiusY=5",
        L"StrokeThickness=2",
        L"Stroke@InactivePointerOver=#ebbcba",
        L"Stroke@InactivePressed=#ebbcba",
        L"Stroke@ActiveNormal=#ebbcba",
        L"Stroke@ActivePointerOver=#ebbcba",
        L"Stroke@ActivePressed=#ebbcba",
        L"Fill=Transparent",
        L"Width=37",
        L"VerticalAlignment=1",
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=13"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"FontSize=14"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"FontFamily=JetBrainsMono NF",
        L"Foreground=#e0def4",
        L"Padding=2,0,8,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uE712"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill=#302d47"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement", {
        L"Background=#302d47"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Margin=0,0,0,-2"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background=#302d47"}},
}};

const Theme g_themeDockLike = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Width=Auto",
        L"HorizontalAlignment=Center",
        L"Margin=250,0,250,0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
        L"Padding=6,0,6,0",
        L"CornerRadius=8,8,0,0",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
        L"Margin=-4,-8,-4,-8",
        L"CornerRadius=10",
        L"BorderThickness=12,12,12,12",
        L"BackgroundSizing=InnerBorderEdge"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.CopilotIcon", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > systemtray:IconView#SystemTrayIcon > Grid", {
        L"Padding=4,0,4,0"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.StackListView#IconStack > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Margin=0,-4,-12,-4"}},
    ThemeTargetStyles{L"Taskbar.Gripper#GripperControl", {
        L"Width=Auto",
        L"MinWidth=24"}},
}};

const Theme g_themeWinVista = {{
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton", {
        L"CornerRadius=2"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"CornerRadius=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"CornerRadius=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Rectangle#RunningIndicator", {
        L"Height=2",
        L"Width@ActiveRunningIndicator=30",
        L"Width@InactiveRunningIndicator=8",
        L"Fill@ActiveRunningIndicator=#00BEE0",
        L"Fill@InactiveRunningIndicator=#DDDDDD"}},
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\" Opacity=\"0.7\"><GradientStop Color=\"#B5B9BC\" Offset=\"0.0\" /><GradientStop Color=\"#B5B9BC\" Offset=\"0.03125\" /><GradientStop Color=\"#909296\" Offset=\"0.03125\" /><GradientStop Color=\"#464B51\" Offset=\"0.5\" /><GradientStop Color=\"#060F15\" Offset=\"0.5\" /><GradientStop Color=\"#040C11\" Offset=\"0.96875\" /><GradientStop Color=\"#000000\" Offset=\"0.96875\" /><GradientStop Color=\"#000000\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border", {
        L"Background@ActiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\" Opacity=\"0.2\"><GradientStop Color=\"#111111\" Offset=\"0.0\" /><GradientStop Color=\"#111111\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"CornerRadius=2",
        L"Background@RequestingAttentionRunningIndicator:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\" Opacity=\"0.2\"><GradientStop Color=\"#D53300\" Offset=\"0.0\" /><GradientStop Color=\"#111111\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderBrush=#33101010",
        L"BorderThickness=1",
        L"BorderBrush@NoRunningIndicator=Transparent",
        L"Background@NoRunningIndicator=Transparent",
        L"Background@ActiveRunningIndicator=#55BBBBBB",
        L"BorderBrush@ActiveRunningIndicator=#55212121"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Margin=0,0,0,2",
        L"BorderThickness=1",
        L"Background@ActivePointerOver=#88DDDDDD",
        L"Background@ActiveNormal=#33BBBBBB",
        L"Background@InactivePointerOver=#33BBBBBB",
        L"BorderBrush@ActiveNormal=#44AAAAAA",
        L"BorderBrush@ActivePointerOver=#FF888888",
        L"BorderBrush@InactiveNormal=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#LanguageInnerTextBlock > TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Grid", {
        L"Background:=<AcrylicBrush TintColor=\"Transparent\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Background=#BB212121",
        L"BorderThickness=0",
        L"Margin=0,2,1,4"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\" Opacity=\"0.7\"><GradientStop Color=\"#B5B9BC\" Offset=\"0.0\" /><GradientStop Color=\"#B5B9BC\" Offset=\"0.03125\" /><GradientStop Color=\"#909296\" Offset=\"0.03125\" /><GradientStop Color=\"#464B51\" Offset=\"0.5\" /><GradientStop Color=\"#060F15\" Offset=\"0.5\" /><GradientStop Color=\"#040C11\" Offset=\"0.96875\" /><GradientStop Color=\"#000000\" Offset=\"0.96875\" /><GradientStop Color=\"#000000\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid", {
        L"Background:=<AcrylicBrush TintColor=\"Transparent\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.1\" Opacity=\"1\" />",
        L"Padding=-1",
        L"Margin=0,6,0,6",
        L"CornerRadius=8"}},
}};

const Theme g_themeCleanSlate = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=100",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@InactivePointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\"/>",
        L"Background@ActivePointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@ActiveNormal:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\"/>",
        L"Background@InactivePressed:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@ActivePressed:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" />"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.5\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"CornerRadius=5",
        L"Margin=0,5,5,5",
        L"Padding=1,0,-10,0"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=8,0,0,0",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton ", {
        L"Padding=8",
        L"Margin=4,0,4,0"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"FontSize=12"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton[AutomationProperties.Name=Copilot] > Taskbar.TaskListLabeledButtonPanel#IconPanel > Border#BackgroundElement", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" />"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView > Grid > Border#BackgroundBorder", {
        L"Margin=0,3,0,3"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement@CommonStates", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"CornerRadius=20",
        L"Margin=0,1,0,1"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Border#BackgroundElement", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background@InactiveRunningIndicator:=<SolidColorBrush Color=\"Black\" Opacity=\"0.4\" />",
        L"Background@InactiveRunningIndicator:=<SolidColorBrush Color=\"Black\" Opacity=\"0.4\" />",
        L"Background@ActiveRunningIndicator:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark2}\" Opacity=\"0.4\" />",
        L"Background@RequestingAttentionRunningIndicator:=<SolidColorBrush Color=\"#ffdf5e\" Opacity=\"0.4\" />"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe", {
        L"Width=12",
        L"Height=38",
        L"Margin=-6,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Width=12"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel", {
        L"Margin=-3,0,0,0"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Border", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark3}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=1",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Border#ProgressBarRoot > Border > Grid > Rectangle", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />"}},
    ThemeTargetStyles{L"Border#ProgressBarRoot > Border > Grid > Rectangle#ProgressBarTrack", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark3} />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"BorderBrush@InactivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"  />",
        L"BorderBrush@ActiveNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" />",
        L"BorderBrush@ActivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"  />",
        L"BorderBrush@MultiWindowPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"  />",
        L"BorderThickness@ActiveNormal=1",
        L"BorderThickness@ActivePointerOver=1",
        L"BorderThickness@MultiWindowActive=2",
        L"BorderThickness@MultiWindowNormal=2",
        L"BorderThickness@MultiWindowPointerOver=2.5",
        L"BorderBrush@MultiWindowActive:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"  />",
        L"BorderBrush@MultiWindowNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"  />",
        L"Margin@MultiWindowNormal=0",
        L"Margin@MultiWindowPointerOver=0",
        L"Margin@MultiWindowPressed=0"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark2}\"  />",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#MultiWindowElement", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=5",
        L"CornerRadius@InactiveNormal_SearchIcon=100",
        L"CornerRadius@InactivePointerOver_SearchIcon=100",
        L"CornerRadius@InactivePressed_SearchIcon=100",
        L"CornerRadius@ActiveNormal_SearchIcon=100",
        L"CornerRadius@ActivePointerOver_SearchIcon=100",
        L"CornerRadius@ActivePressed_SearchIcon=100",
        L"Background@InactiveNormal_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@InactivePointerOver_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"Background@InactivePressed_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"Background@ActiveNormal_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@ActivePointerOver_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"Background@ActivePressed_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />"}},
    ThemeTargetStyles{L"Taskbar.OverflowToggleButton", {
        L"Margin=8,0,8,0",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark2}\"  />"}},
    ThemeTargetStyles{L"Rectangle#RightOverflowButtonDivider", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"  />",
        L"Margin=8,4,-8,4"}},
    ThemeTargetStyles{L"Taskbar.OverflowToggleButton > Taskbar.TaskListButtonPanel@CommonStates > Border", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@InactivePointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel", {
        L"Margin=0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchIconButton > SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=100",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid", {
        L"Margin=0,0,3,0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid > Border#BackgroundElement", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />"}},
    ThemeTargetStyles{L"Border#SearchPillBackgroundElement", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"BorderThickness=16",
        L"CornerRadius=8"}},
}};

const Theme g_themeLucent = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#00000000\" Offset=\"0.3\" /><GradientStop Color=\"#AA000000\" Offset=\"0.9\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#ee000000\" Offset=\"0.1\" /><GradientStop Color=\"{ThemeResource SystemAccentColorDark2}\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Rectangle#RunningIndicator", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background@InactiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#3300290c\" Offset=\"0.1\" /><GradientStop Color=\"{ThemeResource SystemAccentColorDark2}\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Margin=0,-1,0,-1"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > TextBlock#LabelControl", {
        L"Foreground@ActiveNormal=Black",
        L"Foreground@ActivePointerOver=Black",
        L"Margin=0,0,3,0",
        L"Foreground@ActivePressed=#BFBFBF"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Grid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50000000\" Offset=\"0.3\" /><GradientStop Color=\"#EE000000\" Offset=\"0.9\" /></LinearGradientBrush>",
        L"Margin=0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=2"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"CornerRadius=0",
        L"BorderThickness=0",
        L"MaxWidth=48",
        L"Margin=0",
        L"Padding=0",
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#80000000\" Offset=\"0.0\" /><GradientStop Color=\"#FF000000\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50000000\" Offset=\"0.3\" /><GradientStop Color=\"#EE000000\" Offset=\"0.9\" /></LinearGradientBrush>",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton", {
        L"Margin=10,0,-10,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates", {
        L"Background@ActiveNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@ActivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@InactivePointerOver=#EBEBEB",
        L"Background@InactivePressed=#BBBBBB",
        L"Background@ActivePressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@RequestingAttention=#FFE9AFAA",
        L"Background@RequestingAttentionPointerOver=#FFF8E7E5",
        L"Background@RequestingAttentionPressed=#FFFEEEF0",
        L"Background@MultiWindowPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@MultiWindowActive:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@MultiWindowPressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border", {
        L"BorderThickness=0",
        L"Margin=-2,-4,-2,-4",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border", {
        L"CornerRadius=0",
        L"Background@InactivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@InactivePressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@ActiveNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@ActivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@ActivePressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Margin=-3,-10,-3,-10",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Grid#ContainerGrid@", {
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Margin=0",
        L"Padding=0",
        L"CornerRadius=0",
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Grid@CommonStates", {
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@Checked:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@CheckedPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@CheckedPressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Grid@", {
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@CheckedPressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@CheckedNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@CheckedPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack > Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Grid@ ", {
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@CheckedPressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@CheckedNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@CheckedPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.WrapGrid > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.NotifyIconView > Grid@", {
        L"CornerRadius=0",
        L"Margin=2,-5,2,-5"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Border", {
        L"Width=48"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#MultiWindowElement", {
        L"Height=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle", {
        L"StrokeThickness=3",
        L"Stroke@MultiWindowNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Stroke@MultiWindowPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Stroke@MultiWindowActive:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Stroke@MultiWindowPressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Width=Auto",
        L"RadiusX=0",
        L"Margin=-2,0,-2,-3"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Grid > Border", {
        L"BorderThickness=0",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Grid > Border", {
        L"BorderThickness=0",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel@CommonStates", {
        L"Background@InactivePointerOver_SearchIcon=#BEBEBE",
        L"Background@InactivePressed_SearchIcon=#DDDDDD",
        L"Background@ActiveNormal_SearchIcon=#BEBEBE",
        L"Background@ActivePointerOver_SearchIcon=#DDDDDD",
        L"Background@ActivePressed_SearchIcon=#EEEEEE"}},
}};

const Theme g_themeLucent_variant_Light = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#00000000\" Offset=\"0.3\" /><GradientStop Color=\"#AA000000\" Offset=\"0.9\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#ee000000\" Offset=\"0.1\" /><GradientStop Color=\"#EBEBEB\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Rectangle#RunningIndicator", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background@InactiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#33000000\" Offset=\"0.1\" /><GradientStop Color=\"#33EBEBEB\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Margin=0,-1,0,-1"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > TextBlock#LabelControl", {
        L"Foreground@ActiveNormal=Black",
        L"Foreground@ActivePointerOver=Black",
        L"Margin=0,0,3,0",
        L"Foreground@ActivePressed=#424242"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Grid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50000000\" Offset=\"0.3\" /><GradientStop Color=\"#EE000000\" Offset=\"0.9\" /></LinearGradientBrush>",
        L"Margin=0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=2"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"CornerRadius=0",
        L"BorderThickness=0",
        L"MaxWidth=48",
        L"Margin=0",
        L"Padding=0",
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#80000000\" Offset=\"0.0\" /><GradientStop Color=\"#FF000000\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50000000\" Offset=\"0.3\" /><GradientStop Color=\"#EE000000\" Offset=\"0.9\" /></LinearGradientBrush>",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton", {
        L"Margin=10,0,-10,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates", {
        L"Background@ActiveNormal=#FCFCFC",
        L"Background@ActivePointerOver=#BBBBBB",
        L"Background@InactivePointerOver=#BBBBBB",
        L"Background@InactivePressed=#EBEBEB",
        L"Background@ActivePressed=#EBEBEB",
        L"Background@RequestingAttention=#FFE9AFAA",
        L"Background@RequestingAttentionPointerOver=#FFF8E7E5",
        L"Background@RequestingAttentionPressed=#FFFEEEF0",
        L"Background@MultiWindowPointerOver=#BBBBBB",
        L"Background@MultiWindowActive=#BBBBBB",
        L"Background@MultiWindowPressed=#EBEBEB"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border", {
        L"BorderThickness=0",
        L"Margin=-2,-4,-2,-4",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border", {
        L"CornerRadius=0",
        L"Background@InactivePointerOver=#BBBBBB",
        L"Background@InactivePressed=#EBEBEB",
        L"Background@ActiveNormal=#BBBBBB",
        L"Background@ActivePointerOver=#BBBBBB",
        L"Background@ActivePressed=#EBEBEB",
        L"Margin=-3,-10,-3,-10",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Grid#ContainerGrid@", {
        L"Background@PointerOver=#BBBBBB",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@PointerOver=#BBBBBB",
        L"Margin=0",
        L"Padding=0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Grid@CommonStates", {
        L"Background@PointerOver=#BBBBBB",
        L"Background@Pressed=#EBEBEB",
        L"Background@Checked=#BBBBBB",
        L"Background@CheckedPointerOver=#BBBBBB",
        L"Background@CheckedPressed=#EBEBEB"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Grid@", {
        L"Background@Pressed=#EBEBEB",
        L"Background@CheckedPressed=#EBEBEB",
        L"Background@PointerOver=#BBBBBB",
        L"Background@CheckedNormal=#BBBBBB",
        L"Background@CheckedPointerOver=#EBEBEB"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack > Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Grid@ ", {
        L"Background@Pressed=#EBEBEB",
        L"Background@CheckedPressed=#EBEBEB",
        L"Background@PointerOver=#BBBBBB",
        L"Background@CheckedNormal=#BBBBBB",
        L" Background@CheckedPointerOver=#EBEBEB"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.WrapGrid > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.NotifyIconView > Grid@", {
        L"CornerRadius=0",
        L"Margin=2,-5,2,-5"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Border", {
        L"Width=48"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#MultiWindowElement", {
        L"Height=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle", {
        L"StrokeThickness=3",
        L"Stroke@MultiWindowNormal=#BBBBBB",
        L"Stroke@MultiWindowPointerOver=#EBEBEB",
        L"Stroke@MultiWindowPressed=#EBEBEB",
        L"Stroke@MultiWindowActive=#EBEBEB",
        L"Width=Auto",
        L"RadiusX=0",
        L"Margin=-2,0,-2,-3"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Grid > Border", {
        L"BorderThickness=0",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Grid > Border", {
        L"BorderThickness=0",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel@CommonStates", {
        L"Background@InactivePointerOver_SearchIcon=#BEBEBE",
        L"Background@ActiveNormal_SearchIcon=#BEBEBE",
        L"Background@ActivePointerOver_SearchIcon=#EBEBEB"}},
}};

const Theme g_theme21996Taskbar = {{
    ThemeTargetStyles{L"Taskbar.SearchBoxButton#SearchBoxButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Height=48"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid > SystemTray.AdaptiveTextBlock > Windows.UI.Xaml.Controls.TextBlock", {
        L"Visibility=Visible",
        L"Text=\u200E \u200E\u200E\u200E\uE91C",
        L"FontSize=16.4",
        L"FontFamily=Segoe MDL2 Assets",
        L"Width=30",
        L"FontWeight=ExtraLight",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon#SearchBoxFontIcon", {
        L"FontFamily=Segoe Fluent Icons",
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"16\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#SearchBoxTextBlock", {
        L"Text=Search",
        L"FontSize=14"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"CornerRadius=0",
        L"Height=61",
        L"Margin=0,-5,0,0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"CornerRadius=0",
        L"Height=61",
        L"Margin=-7,-6,0,0",
        L"Width=24",
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton#SearchBoxButton[AutomationProperties.AutomationId=SearchButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"FlowDirection=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button#GleamEntryPointButton > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#DynamicSearchBoxGleamContainer", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton", {
        L"CornerRadius=0",
        L"Padding=0,0,0,0",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack", {
        L"Height=61",
        L"CornerRadius=0",
        L"Margin=0,-7.5,-6,0"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe@CommonStates", {
        L"Width=9",
        L"Margin=0,0,-10,0",
        L"Height=500",
        L"Fill@Active:=<AcrylicBrush TintColor=\"{ThemeResource SystemBaseLowColor}\" TintOpacity=\"0.5\" Opacity=\"0\"/>",
        L"Stroke:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#LanguageInnerTextBlock > TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe UI",
        L"Margin=-8,0,0,0",
        L"FontSize=12"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Windows.UI.Xaml.Controls.Grid#SystemTrayFrameGrid > SystemTray.Stack#NotifyIconStack > Windows.UI.Xaml.Controls.Grid#Content > SystemTray.StackListView#IconStack > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid#ContentGrid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe MDL2 Assets",
        L"FontSize=12.4",
        L"Width=22"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Windows.UI.Xaml.Controls.Grid#SystemTrayFrameGrid > SystemTray.Stack#NotifyIconStack > Windows.UI.Xaml.Controls.Grid#Content > SystemTray.StackListView#IconStack > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Width=30"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#AccentOverlay > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#Underlay > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[1] > SystemTray.IconView > Grid > Grid", {
        L"Margin=-6,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#MainStack > Windows.UI.Xaml.Controls.Grid#Content", {
        L"CornerRadius=0",
        L"Height=61",
        L"Margin=-4,-7,-4,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#TimeInnerTextBlock", {
        L"FontFamily=Segoe UI",
        L"TextAlignment=0",
        L"FontSize=12",
        L"Margin=0,-1,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#DateInnerTextBlock", {
        L"FontFamily=Segoe UI",
        L"TextAlignment=0",
        L"FontSize=12",
        L"Margin=0,2,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Width=23",
        L"Margin=0,-2,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.NotifyIconView#NotifyItemIcon > Windows.UI.Xaml.Controls.Grid#ContainerGrid", {
        L"Margin=-9,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NotifyIconStack", {
        L"Width=24"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid > SystemTray.AdaptiveTextBlock > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontSize=16",
        L"Margin=0,-1,-0,0",
        L"FontWeight=0"}},
    ThemeTargetStyles{L"SystemTray.CopilotIcon#CopilotIcon", {
        L"Visibility=Visible",
        L"Margin=0,-7,0,0",
        L"Height=61"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaOverflow > Windows.UI.Xaml.Controls.Grid#OverflowRootGrid > Windows.UI.Xaml.Controls.Border#OverflowFlyoutBackgroundBorder", {
        L"CornerRadius=0",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.76\" TintLuminosityOpacity=\"0.77\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaOverflow > Windows.UI.Xaml.Controls.Grid#OverflowRootGrid > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.WrapGrid", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ScrollViewer > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.Border > SystemTray.NotificationAreaOverflow", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"15\" />"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootGrid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill", {
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[3] > SystemTray.IconView > Grid > Grid", {
        L"Margin=0,0,-6,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter#HoverFlyoutContent", {
        L"CornerRadius=0",
        L"Margin=0,0,0,-15",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0.76\" TintLuminosityOpacity=\"0.67\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > TextBlock", {
        L"FontFamily=Segoe UI",
        L"FontSize=12",
        L"Margin=3,0,8,8"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Windows.UI.Xaml.Controls.Grid > Microsoft.UI.Xaml.Controls.ItemsRepeater > Windows.UI.Xaml.Controls.Image", {
        L"Margin=0,-7,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Button > ContentPresenter > TextBlock", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Button", {
        L"CornerRadius=0",
        L"Height=32",
        L"Margin=0,0,0,9",
        L"Width=32"}},
    ThemeTargetStyles{L"Grid#DetailedViewGrid", {
        L"Margin=0,-7,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Border", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.5\" />",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid > SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\u200E\uE91C",
        L"FontWeight=Light",
        L"FontSize=16.4",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />",
        L"Margin=-1,0,1,0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView", {
        L"CornerRadius=0",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.DateTimeIconContent > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe UI",
        L"TextAlignment=Center"}},
}};

const Theme g_themeBottomDensy = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Rectangle#RunningIndicator", {
        L"Fill=#8f8f8f",
        L"Fill@ActiveRunningIndicator=#fef9f0",
        L"Width=2",
        L"Height=2",
        L"Margin=0,-2,0,0",
        L"Width@ActiveRunningIndicator=32"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > muxc:ProgressBar#ProgressIndicator", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Border#ProgressBarRoot", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Rectangle#IndeterminateProgressBarIndicator", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Rectangle#IndeterminateProgressBarIndicator2", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel", {
        L"Padding=2,0,2,0",
        L"VerticalAlignment=2"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Width=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ShowDesktopPipe", {
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.NotifyIconView#NotifyItemIcon > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=20",
        L"Height=20"}},
    ThemeTargetStyles{L"WrapGrid > ContentPresenter > SystemTray.NotifyIconView > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=20",
        L"Height=20"}},
}};

const Theme g_themeTaskbarXII = {{
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Grid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemListLowColor}\" TintOpacity=\"0.1\" FallbackColor=\"{ThemeResource SystemChromeHighColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame#TaskbarFrame", {
        L"HorizontalAlignment=Right",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-820\"/>",
        L"Width=Auto",
        L"Height=56"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame#TaskbarFrame > Grid", {
        L"Height=48",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl", {
        L"Height=48",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"156.5\"/>",
        L"Opacity=0.7"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground > Grid", {
        L"CornerRadius=4",
        L"Opacity=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ItemsRepeater#TaskbarFrameRepeater", {
        L"Margin=0,0,3,0"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel", {
        L"Margin=2,0,6,0"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"Text=\u2726 Meow"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton > Taskbar.TaskListButtonPanel", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.6\" />",
        L"CornerRadius=4",
        L"Padding=0",
        L"Margin=0,0,7,0"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton > Taskbar.TaskListButtonPanel > Grid", {
        L"Margin=8,0,0,0"}},
    ThemeTargetStyles{L"Border#LargeTicker1", {
        L"Margin=0,2,4,0"}},
    ThemeTargetStyles{L"Border#LargeTicker1 > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Image", {
        L"MaxHeight=27",
        L"MaxWidth=27"}},
    ThemeTargetStyles{L"Border#LargeTicker1 > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer", {
        L"MaxHeight=27",
        L"MaxWidth=27"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", {
        L"HorizontalAlignment=Left",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"1104.5\"/>"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.6\" />",
        L"CornerRadius=4",
        L"Padding=8,3,0,3"}},
    ThemeTargetStyles{L"SystemTray.Stack#SecondaryClockStack", {
        L"Grid.Column=8"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Grid.Column=4"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton", {
        L"Grid.Column=5"}},
    ThemeTargetStyles{L"SystemTray.Stack#MainStack", {
        L"Grid.Column=6"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Grid.Column=7"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uED14"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"10\"/>",
        L"FontSize=15",
        L"FontWeight=Bold",
        L"Margin=94,0,0,0"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-10\"/>",
        L"FontSize=15",
        L"FontWeight=SemiBold",
        L"HorizontalAlignment=Left"}},
}};

const Theme g_themexdark = {{
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"CornerRadius=13",
        L"Padding=6,0,6,0",
        L"HorizontalContentAlignment=Left"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"MinWidth=25"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[1] > SystemTray.IconView > Grid > Grid", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"Padding=2"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"MinWidth=27"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#BackgroundElement", {
        L"Background=#000000"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background=#000000",
        L"CornerRadius=13",
        L"Margin=0,5,4,5",
        L"Padding=2,0,-18,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton > Grid > Rectangle#RunningIndicator", {
        L"Height=3",
        L"RadiusX=1.5",
        L"RadiusY=1.5",
        L"Fill@ActiveNormal=#facc15",
        L"VerticalAlignment=Bottom",
        L"Margin=16,0,16,4",
        L"StrokeThickness=0"}},
    ThemeTargetStyles{L"SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=13"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"FontFamily=Segoe UI Medium",
        L"Foreground=#facc15",
        L"Margin=1,0,0,0",
        L"VerticalAlignment=Center",
        L"TextWrapping=NoWrap"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uE712",
        L"Foreground=#facc15"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"Foreground=#facc15"}},
}};

const Theme g_themeMatter = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill := $transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill := $transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl", {
        L"Fill:=$base",
        L"CornerRadius = $mainRadius"}},
    ThemeTargetStyles{L" Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton", {
        L"Margin=-1,1,1,1"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius = $mainRadius",
        L"Background :=$base",
        L"Background@InactivePointerOver :=$overlay2",
        L"Background@ActivePointerOver:=$overlay",
        L"Background@ActiveNormal :=$active"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.Name=Task View]", {
        L"Margin=0,0,2,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton[AutomationProperties.Name=Copilot] > Taskbar.TaskListLabeledButtonPanel#IconPanel > Border#BackgroundElement", {
        L"Visibility = 1"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Margin=0,0,2,0"}},
    ThemeTargetStyles{L"Border#BackgroundElement", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background@InactiveNormal :=$base",
        L"Background@ActiveNormal :=$active",
        L"Background@InactivePointerOver :=$overlay2",
        L"Background@ActivePointerOver:=$overlay",
        L"CornerRadius = $mainRadius",
        L"Margin = 1,0,1,0",
        L"Background@MultiWindowNormal:=$base",
        L"Background@MultiWindowPointerOver:=$overlay2",
        L"Background@MultiWindowActive:=$active",
        L"Background@MultiWindowPressed:=$overlay"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"CornerRadius = $mainRadius",
        L"Padding = 7,0,8,0",
        L"Background :=$accentColor"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=0,0,2,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Rectangle#RunningIndicator", {
        L"Fill := $inverseBW",
        L"RadiusX=1.5",
        L"RadiusY=1.5",
        L"Height=4",
        L"Width=12",
        L"Fill@ActiveRunningIndicator :=$accentColor",
        L"Width@ActiveRunningIndicator=21"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=$base",
        L"CornerRadius = $mainRadius",
        L"Margin=0,5,12,5",
        L"Padding=5,0,0,0"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"Margin=2,5,2,5",
        L"CornerRadius=8",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=$base",
        L"Shadow :=",
        L"CornerRadius = 14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalTrackRect", {
        L"Height = 8",
        L"Margin = 0",
        L"Fill := $overlay"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalDecreaseRect", {
        L"Height = 8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#volumeLevelText", {
        L"FontFamily = Tektur",
        L"Margin = 0,-2,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#VolumeConfirmator", {
        L"Padding = 8,0,3,0",
        L"CornerRadius = 20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ConfirmatorMainGrid", {
        L"Background :=$base",
        L"CornerRadius = 14",
        L"BorderThickness = 0",
        L"Margin = 0,0,0,10",
        L"Shadow :="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#BrightnessConfirmator", {
        L"Padding = 15,0,17,0",
        L"CornerRadius = 20"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#BrightnessIcon", {
        L"Margin = 0,-1,12,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ProgressBar#ProgressIndicator", {
        L"Margin = 0,0,0,1"}},
    ThemeTargetStyles{L" Windows.UI.Xaml.Shapes.Rectangle#ProgressBarTrack", {
        L"Fill := $inverseBW",
        L"RadiusX = 1.5",
        L"RadiusY = 1.5"}},
    ThemeTargetStyles{L" Windows.UI.Xaml.Shapes.Rectangle#DeterminateProgressBarIndicator", {
        L"Fill :=$accentColor"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel > Microsoft.UI.Xaml.Controls.ProgressBar#ProgressIndicator", {
        L"MinHeight = 4",
        L"Width = 26"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.Name=Start]", {
        L"Margin = 0,0,2,0"}},
    ThemeTargetStyles{L"Taskbar.Badge#BadgeControl", {
        L"Height = 14",
        L"MinWidth = 14",
        L"Margin = 0,0,0,0",
        L"CornerRadius = 20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundRect", {
        L"RadiusX = 4",
        L"RadiusY = 4"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background := $base",
        L"Shadow :=",
        L"CornerRadius = 8"}},
}, {
    L"mainRadius = 8",
    L"transparent = <SolidColorBrush Color=\"Transparent\"/>",
    L"base = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
    L"overlay = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" />",
    L"overlay2 = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"0.5\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" />",
    L"accentColor = <SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" Opacity = \"1\" />",
    L"inverseBW = <SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity = \"1\" />",
    L"active = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"1\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" />",
}};

const Theme g_themeSimplyTransparent = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
}};

const Theme g_themeWindows7 = {{
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Background@InactiveNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandInactiveNormal\" />",
        L"Background@InactivePointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandInactivePointerOver\" />",
        L"Background@ActiveNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActiveNormal\" />",
        L"Background@ActivePressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActivePressed\" />",
        L"Background@ActivePointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActivePointerOver\" />",
        L"Background@InactivePressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandInactivePressed\" />",
        L"BorderThickness=0",
        L"Background@MultiWindowNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandInactiveNormal\" />",
        L"Background@MultiWindowActive:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActiveNormal\" />",
        L"Background@MultiWindowPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActivePressed\" />",
        L"Background@MultiWindowPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActivePointerOver\" />",
        L"Background@RequestingAttention:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionMulti:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionMultiPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionMultiPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel", {
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#RunningIndicator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Image", {
        L"RenderTransform:=<TranslateTransform X=\"2\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Opacity@NoRunningIndicator=0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background@InactiveNormal:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbNormal\" />",
        L"Background@InactivePointerOver:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbPointerOver\" />",
        L"Background@InactivePressed:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbPressed\" />",
        L"Background@ActiveNormal:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbPressed\" />",
        L"Background@ActivePointerOver:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbPointerOver\" />",
        L"Background@ActivePressed:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbPressed\" />",
        L"BorderThickness=0",
        L"Width=54"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Padding=0,0,0,0",
        L"MinWidth=55",
        L"Margin=0,0,5,0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid", {
        L"Background:=<WindhawkBlur BlurAmount=\"3\" TintColor=\"#$aeroOpacity$aeroColor\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Fill:=<ImageBrush Stretch=\"UniformtoFill\" ImageSource=\"$reflection\" />",
        L"Height=39"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbarBackground\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"Margin=1,0,1,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Windows.UI.Xaml.Controls.Border#MultiWindowElement", {
        L"Background@MultiWindowNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandInactiveNormal\" />",
        L"Background@MultiWindowActive:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActiveNormal\" />",
        L"Background@MultiWindowPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActiveNormal\" />",
        L"BorderThickness=0",
        L"RenderTransform:=<TranslateTransform X=\"2\" />",
        L"Background@MultiWindowPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActiveNormal\" />",
        L"Background@RequestingAttentionMulti:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionMultiPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionMultiPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid@ > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@Normal:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowNormal\" />",
        L"Background@PointerOver:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowPointerOver\" />",
        L"Background@Pressed:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowPressed\" />",
        L"Background@CheckedNormal:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowPressed\" />",
        L"Background@CheckedPointerOver:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowPressed\" />",
        L"Background@CheckedPressed:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowPressed\" />",
        L"BorderThickness=0",
        L"Width=21",
        L"Height=20"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@Normal=Transparent",
        L"Background@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@Pressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />",
        L"BorderThickness=0",
        L"Margin=0",
        L"MinWidth=68",
        L"Background@Checked:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@CheckedPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@CheckedPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />"}},
    ThemeTargetStyles{L"SystemTray.DateTimeIconContent > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"TextAlignment=0",
        L"Foreground=White",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@Normal=Transparent",
        L"Background@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@Pressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />",
        L"BorderThickness=0",
        L"Margin=0",
        L"Background@Checked:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@CheckedPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@CheckedPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock > Windows.UI.Xaml.Controls.TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon > Windows.UI.Xaml.Controls.Grid#ContainerGrid@ > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@Normal=Transparent",
        L"Background@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPointerOver\" />",
        L"BorderThickness=0",
        L"Margin=0",
        L"Width=24"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Width=24",
        L"Padding=-2,0,-2,0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Grid#ContentGrid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid#ContainerGrid > SystemTray.AdaptiveTextBlock[2] > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe MDL2 Assets",
        L"Text=\uE81B"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel", {
        L"FlowDirection=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ContainerGrid@ > Windows.UI.Xaml.Shapes.Rectangle#ShowDesktopPipe", {
        L"Fill@Normal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$desktopNormal\" />",
        L"Height=39",
        L"Width=$desktopWidth",
        L"RadiusX=0",
        L"RadiusY=0",
        L"Fill@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$desktopPointerOver\" />",
        L"Fill@Pressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$desktopPressed\" />"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Width=$desktopWidth"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack > Windows.UI.Xaml.Controls.Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Width=$desktopWidth"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack > Windows.UI.Xaml.Controls.Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView", {
        L"Width=$desktopWidth"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView#SystemTrayIcon > Windows.UI.Xaml.Controls.Grid#ContainerGrid", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Margin=3,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#MainStack > Windows.UI.Xaml.Controls.Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid@ > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@Normal=Transparent",
        L"Background@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPointerOver\" />",
        L"Background@Pressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPressed\" />",
        L"BorderThickness=0",
        L"Margin=0",
        L"Background@Checked:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPointerOver\" />",
        L"Background@CheckedPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPointerOver\" />",
        L"Background@CheckedPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPressed\" />",
        L"Width=24"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel@RunningIndicatorStates > Windows.UI.Xaml.Shapes.Rectangle#DefaultIcon", {
        L"Visibility=Collapsed",
        L"Visibility@NoRunningIndicator=Visible"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel@CommonStates > Windows.UI.Xaml.Shapes.Rectangle#DefaultIcon", {
        L"Fill=Transparent",
        L"Width=54",
        L"Height=54",
        L"Fill@InactivePointerOver:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPointerOver\" />",
        L"Fill@InactivePressed:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPressed\" />",
        L"Transform3D:=<CompositeTransform3D ScaleY=\"1.1\" ScaleX=\"1.04\" TranslateY=\"1\" CenterY=\"27\" />"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#LanguageInnerTextBlock > TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Border#SearchPillBackgroundElement", {
        L"BorderBrush=#4F222222",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=TaskViewButton] > Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"BorderBrush@InactivePointerOver:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPointerOver\" />",
        L"BorderThickness=2",
        L"Background:=<ImageBrush Stretch=\"None\" ImageSource=\"$taskviewIcon\" />",
        L"BorderBrush@InactivePressed:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPressed\" />",
        L"BorderBrush@ActivePressed:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPressed\" />",
        L"BorderBrush@ActivePointerOver:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPointerOver\" />",
        L"BorderBrush@ActiveNormal=Transparent"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=TaskViewButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton#SearchBoxButton[AutomationProperties.AutomationId=SearchButton] > Taskbar.TaskListButtonPanel@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"BorderBrush@InactivePointerOver_SearchIcon:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPointerOver\" />",
        L"BorderBrush@InactivePressed_SearchIcon:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPressed\" />",
        L"BorderBrush@ActivePressed_SearchIcon:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPressed\" />",
        L"BorderBrush@ActivePointerOver_SearchIcon:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPointerOver\" />",
        L"BorderBrush@ActiveNormal_SearchIcon=Transparent",
        L"BorderThickness@InactivePointerOver_SearchIcon=2",
        L"BorderThickness@InactivePressed_SearchIcon=2",
        L"BorderThickness@ActivePressed_SearchIcon=2",
        L"BorderThickness@ActivePointerOver_SearchIcon=2",
        L"Background@ActiveNormal_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Background@InactivePointerOver_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Background@InactivePressed_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Background@ActivePressed_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Background@ActivePointerOver_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Background@InactiveNormal_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Height=30",
        L"Height@ActiveNormal_SearchIcon=Auto",
        L"Height@InactivePointerOver_SearchIcon=Auto",
        L"Height@InactivePressed_SearchIcon=Auto",
        L"Height@ActivePressed_SearchIcon=Auto",
        L"Height@ActivePointerOver_SearchIcon=Auto",
        L"Height@InactiveNormal_SearchIcon=Auto"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton#SearchBoxButton[AutomationProperties.AutomationId=SearchButton] > Taskbar.TaskListButtonPanel@CommonStates > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer", {
        L"Visibility@ActiveNormal_SearchIcon=Collapsed",
        L"Visibility@InactivePointerOver_SearchIcon=Collapsed",
        L"Visibility@InactivePressed_SearchIcon=Collapsed",
        L"Visibility@ActivePressed_SearchIcon=Collapsed",
        L"Visibility@ActivePointerOver_SearchIcon=Collapsed",
        L"Visibility@InactiveNormal_SearchIcon=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Windows.UI.Xaml.Controls.Grid@ > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@CheckedPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />",
        L"Background@CheckedPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@CheckedNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"BorderThickness=0",
        L"Background@Pressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />",
        L"Background@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@Normal=Transparent",
        L"Margin=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack", {
        L"Margin=4,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton[AutomationProperties.AutomationId=WidgetsButton] > Taskbar.TaskListButtonPanel@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Background@InactivePointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$widgetsPointerOver\" />",
        L"Background@InactivePressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$widgetsPressed\" />",
        L"Background@ActivePointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$widgetsPointerOver\" />",
        L"Background@ActivePressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$widgetsPressed\" />",
        L"Background@ActiveNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$widgetsPointerOver\" />",
        L"BorderThickness=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Width=54"}},
}, {
    L"orbNormal=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/orbNormal.png",
    L"orbPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/orbHover.png",
    L"orbPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/orbPressed.png",
    L"aeroColor=74b8fc",
    L"aeroOpacity=4D",
    L"reflection=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/AeroPeek.png",
    L"taskbandInactiveNormal=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/InactiveNormal.png",
    L"taskbandInactivePointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/InactivePointerOver.png",
    L"taskbandInactivePressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/ActiveNormal.png",
    L"taskbandActiveNormal=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/ActiveNormal.png",
    L"taskbandActivePointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/ActiveNormal.png",
    L"taskbandActivePressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/ActiveNormal.png",
    L"overflowNormal=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/overflowNormal.png",
    L"overflowPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/overflowPointerOver.png",
    L"overflowPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/overflowPressed.png",
    L"clockPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/clockPointerOver.png",
    L"clockPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/clockPressed.png",
    L"trayPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/trayPointerOver.png",
    L"trayPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/trayPressed.png",
    L"desktopNormal=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/desktopNormal.png",
    L"desktopPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/desktopPointerOver.png",
    L"desktopPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/desktopPressed.png",
    L"desktopWidth=15",
    L"taskbandRequestingAttention=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/requestingAttention.png",
    L"taskbandNotRunningPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/notRunningPointerOver.png",
    L"taskbandNotRunningPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/notRunningPressed.png",
    L"taskbarBackground=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/taskbarBackground.png",
    L"taskviewIcon=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/taskview.png",
    L"searchIcon=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/search.png",
    L"widgetsPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/widgetsPointerOver.png",
    L"widgetsPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/widgetsPressed.png",
}};

const Theme g_themeAeris = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Background:=$taskbarBackground"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid", {
        L"Background:=<WindhawkBlur BlurAmount=\"$taskbarBlurIncreace\" TintColor=\"#00000000\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<SolidColorBrush Color=\"$themeColor\" Opacity=\"$themeColorOpacity\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed",
        L"Fill:=<SolidColorBrush Color=\"$primaryColor\" Opacity=\"0.05\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates", {
        L"Padding=0",
        L"Margin=$taskListMargin,0,$taskListMargin,0",
        L"Background@ActiveNormal:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePointerOver:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePressed:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background:=$transparent",
        L"Background@InactivePointerOver:=$pointerOver",
        L"Background@InactivePressed:=$pressed",
        L"Background@ActivePointerOver:=$pointerOver",
        L"Background@ActivePressed:=$pressed",
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates", {
        L"Padding=0",
        L"Margin=$taskListMargin,0,$taskListMargin,0",
        L"Background@NoRunningIndicator:=$transparent",
        L"Background@InactiveRunningIndicator:=<SolidColorBrush Color=\"$primaryColor\" Opacity=\"0.1\"/>",
        L"Background@ActiveRunningIndicator:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background:=<SolidColorBrush Color=\"$requestAttentionColor\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background:=$transparent",
        L"Background@InactivePointerOver:=$pointerOver",
        L"Background@InactivePressed:=$pressed",
        L"Background@ActivePointerOver:=$pointerOver",
        L"Background@ActivePressed:=$pressed",
        L"Background@MultiWindowPointerOver:=$pointerOver",
        L"Background@MultiWindowPressed:=$pressed",
        L"Background@RequestingAttentionPointerOver:=$pointerOver",
        L"Background@RequestingAttentionPressed:=$pressed",
        L"Background@RequestingAttentionMultiPointerOver:=$pointerOver",
        L"Background@RequestingAttentionMultiPressed:=$pressed",
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#MultiWindowElement", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle#RunningIndicator", {
        L"Visibility=Collapsed",
        L"Fill:=<SolidColorBrush Color=\"$primaryColor\" Opacity=\"0.2\"/>",
        L"VerticalAlignment=0",
        L"HorizontalAlignment=Stretch",
        L"Margin=0,0,-4,0",
        L"Width=Auto",
        L"Height=3",
        L"RadiusX=0",
        L"RadiusY=0",
        L"Visibility@MultiWindowNormal=Visible",
        L"Visibility@MultiWindowActive=Visible",
        L"Visibility@MultiWindowPointerOver=Visible",
        L"Visibility@MultiWindowPressed=Visible",
        L"Visibility@RequestingAttentionMulti=Visible",
        L"Visibility@RequestingAttentionMultiPointerOver=Visible",
        L"Visibility@RequestingAttentionMultiPressed=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ProgressBar#ProgressIndicator", {
        L"VerticalAlignment=Stretch",
        L"HorizontalAlignment=Stretch",
        L"Margin=0,0,-4,0",
        L"Width=Auto",
        L"Height=Auto",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Border#ProgressBarRoot > Border > Grid", {
        L"Height=Auto"}},
    ThemeTargetStyles{L"Grid#LayoutRoot@CommonStates > Border#ProgressBarRoot > Border > Grid > Rectangle#ProgressBarTrack", {
        L"Margin=0",
        L"RadiusX=0",
        L"RadiusY=0",
        L"Fill:=<SolidColorBrush Color=\"$progressColor\" Opacity=\"0.15\"/>",
        L"Fill@Paused:=<SolidColorBrush Color=\"$progressPausedColor\" Opacity=\"0.15\"/>"}},
    ThemeTargetStyles{L"Grid#LayoutRoot@CommonStates > Border#ProgressBarRoot > Border > Grid > Rectangle#DeterminateProgressBarIndicator", {
        L"RadiusX=0",
        L"RadiusY=0",
        L"Fill:=<SolidColorBrush Color=\"$progressColor\" Opacity=\"0.4\"/>",
        L"Fill@Paused:=<SolidColorBrush Color=\"$progressPausedColor\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Image", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"2\" TranslateY=\"1\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Rectangle#DefaultIcon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel > AnimatedVisualPlayer", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"0\" TranslateY=\"1\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"MinWidth=60",
        L"Margin=0,0,$taskListMargin,0"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel@CommonStates", {
        L"Padding=0",
        L"Margin=$taskListMargin,0,$taskListMargin,0",
        L"Background@ActiveNormal_SearchIcon:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePointerOver_SearchIcon:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePressed_SearchIcon:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxLaunchListButton > Taskbar.TaskListButtonPanel", {
        L"Padding=0",
        L"Margin=$taskListMargin,0,$taskListMargin,0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid@CommonStates", {
        L"Padding=0",
        L"Margin=$taskListMargin,0,$taskListMargin,0",
        L"Background@ActiveNormal:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePointerOver:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePressed:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0",
        L"Background@InactiveNormal_SearchIcon:=$transparent",
        L"Background@InactivePointerOver_SearchIcon:=$pointerOver",
        L"Background@InactivePressed_SearchIcon:=$pressed",
        L"Background@ActiveNormal_SearchIcon:=$transparent",
        L"Background@ActivePointerOver_SearchIcon:=$pointerOver",
        L"Background@ActivePressed_SearchIcon:=$pressed"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxLaunchListButton > Taskbar.TaskListButtonPanel > Border#BackgroundElement", {
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border#BackgroundElement", {
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0",
        L"Background@InactiveNormal:=$transparent",
        L"Background@InactivePointerOver:=$pointerOver",
        L"Background@InactivePressed:=$pressed",
        L"Background@ActiveNormal:=$transparent",
        L"Background@ActivePointerOver:=$pointerOver",
        L"Background@ActivePressed:=$pressed"}},
    ThemeTargetStyles{L"Border#SearchPillBackgroundElement", {
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid#DynamicSearchBoxGleamContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid@ > Border#BackgroundBorder", {
        L"Padding=0",
        L"CornerRadius=0",
        L"Margin=0",
        L"BorderThickness=0",
        L"Background@PointerOver:=$pointerOver",
        L"Background@Pressed:=$pressed"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@PointerOver:=$pointerOver",
        L"Background@Pressed:=$pressed"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"MinWidth=$showDesktopWidth",
        L"MaxWidth=$showDesktopWidth"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack > Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView", {
        L"MinWidth=$showDesktopWidth",
        L"MaxWidth=$showDesktopWidth"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid@ > Rectangle#ShowDesktopPipe", {
        L"VerticalAlignment=Stretch",
        L"HorizontalAlignment=Stretch",
        L"Height=Auto",
        L"Width=Auto",
        L"Fill@PointerOver:=$pointerOver",
        L"Fill@Pressed:=$pressed"}},
}, {
    L"themeColor={ThemeResource SystemAccentColorDark1}",
    L"themeColorOpacity=0",
    L"primaryColor={ThemeResource TextFillColorPrimary}",
    L"activeColor=#33AAFF",
    L"requestAttentionColor=#FF7788",
    L"progressColor=#44CC66",
    L"progressPausedColor=#EECC44",
    L"taskbarBackground=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.8\" Opacity=\"1\"/>",
    L"taskbarBlurIncreace=0",
    L"taskListMargin=2",
    L"showDesktopWidth=10",
    L"transparent=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" Opacity=\"0\"/>",
    L"pointerOver=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" Opacity=\"0.075\"/>",
    L"pressed=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" Opacity=\"0.05\"/>",
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
    winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this));
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
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

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
    wf::Numerics::float4 tint;
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

thread_local std::list<
    std::pair<winrt::weak_ref<DependencyObject>,
              winrt::Windows::Foundation::IAsyncOperation<bool>>>
    g_delayedBackgroundFillSet;

winrt::Windows::Foundation::IInspectable ReadLocalValueWithWorkaround(
    DependencyObject elementDo,
    DependencyProperty property) {
    const auto value = elementDo.ReadLocalValue(property);
    if (value && winrt::get_class_name(value) ==
                     L"Windows.UI.Xaml.Data.BindingExpressionBase") {
        // BindingExpressionBase was observed to be returned for XAML properties
        // that were declared as following:
        //
        // <Border ... CornerRadius="{TemplateBinding CornerRadius}" />
        //
        // Calling SetValue with it fails with an error, so we won't be able to
        // use it to restore the value. As a workaround, we use
        // GetAnimationBaseValue to get the value.
        return elementDo.GetAnimationBaseValue(property);
    }

    return value;
}

// Blur background implementation, copied from TranslucentTB.
////////////////////////////////////////////////////////////////////////////////
// clang-format off

#include <initguid.h>

#include <winrt/Windows.UI.Xaml.Hosting.h>

namespace wfn = wf::Numerics;
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

class XamlBlurBrush : public wux::Media::XamlCompositionBrushBaseT<XamlBlurBrush>
{
public:
	XamlBlurBrush(wuc::Compositor compositor, float blurAmount, wfn::float4 tint);

	void OnConnected();
	void OnDisconnected();

private:
	wuc::Compositor m_compositor;
	float m_blurAmount;
	wfn::float4 m_tint;
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

	wfn::float4 Color = { 0.0f, 0.0f, 0.0f, 1.0f };
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
			*value = wf::PropertyValue::CreateSingleArray({ Color.x, Color.y, Color.z, Color.w }).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
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
XamlBlurBrush::XamlBlurBrush(wuc::Compositor compositor, float blurAmount, wfn::float4 tint) :
	m_compositor(std::move(compositor)),
	m_blurAmount(blurAmount),
	m_tint(tint)
{ }

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

		auto factory = m_compositor.CreateEffectFactory(*compositeEffect);
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

// clang-format on
////////////////////////////////////////////////////////////////////////////////

void SetOrClearValue(DependencyObject elementDo,
                     DependencyProperty property,
                     const PropertyOverrideValue& overrideValue,
                     bool initialApply = false) {
    winrt::Windows::Foundation::IInspectable value;
    if (auto* inspectable =
            std::get_if<winrt::Windows::Foundation::IInspectable>(
                &overrideValue)) {
        value = *inspectable;
    } else if (auto* blurBrashParams =
                   std::get_if<XamlBlurBrushParams>(&overrideValue)) {
        if (auto uiElement = elementDo.try_as<UIElement>()) {
            auto compositor =
                wuxh::ElementCompositionPreview::GetElementVisual(uiElement)
                    .Compositor();

            value = winrt::make<XamlBlurBrush>(std::move(compositor),
                                               blurBrashParams->blurAmount,
                                               blurBrashParams->tint);
        } else {
            Wh_Log(L"Can't get UIElement for blur brush");
            return;
        }
    } else {
        Wh_Log(L"Unsupported override value");
        return;
    }

    // If customized before
    // `winrt::Taskbar::implementation::TaskbarBackground::OnApplyTemplate` is
    // executed, it can lead to a crash, or the customization may be overridden.
    // See:
    // https://github.com/ramensoftware/windows-11-taskbar-styling-guide/issues/4
    if (winrt::get_class_name(elementDo) ==
            L"Windows.UI.Xaml.Shapes.Rectangle" &&
        elementDo.as<FrameworkElement>().Name() == L"BackgroundFill" &&
        property == Shapes::Shape::FillProperty()) {
        auto it = std::find_if(g_delayedBackgroundFillSet.begin(),
                               g_delayedBackgroundFillSet.end(),
                               [&elementDo](const auto& it) {
                                   if (auto elementDoIter = it.first.get()) {
                                       return elementDoIter == elementDo;
                                   }
                                   return false;
                               });

        if (value != DependencyProperty::UnsetValue() && initialApply &&
            it == g_delayedBackgroundFillSet.end()) {
            Wh_Log(L"Delaying SetValue for BackgroundFill");
            auto asyncOp = elementDo.Dispatcher().TryRunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                [elementDo = std::move(elementDo),
                 property = std::move(property), value = std::move(value)]() {
                    Wh_Log(L"Running delayed SetValue for BackgroundFill");
                    try {
                        elementDo.SetValue(property, value);
                    } catch (winrt::hresult_error const& ex) {
                        Wh_Log(L"Error %08X: %s", ex.code(),
                               ex.message().c_str());
                    }
                    std::erase_if(g_delayedBackgroundFillSet,
                                  [&elementDo](const auto& it) {
                                      if (auto elementDoIter = it.first.get()) {
                                          return elementDoIter == elementDo;
                                      }
                                      return false;
                                  });
                });
            g_delayedBackgroundFillSet.emplace_back(elementDo,
                                                    std::move(asyncOp));
            return;
        } else if (it != g_delayedBackgroundFillSet.end()) {
            Wh_Log(L"Canceling delayed SetValue for BackgroundFill");
            it->second.Cancel();
            g_delayedBackgroundFillSet.erase(it);
        }
    }

    if (value == DependencyProperty::UnsetValue()) {
        elementDo.ClearValue(property);
        return;
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

    auto value = XamlBlurBrushParams{
        .blurAmount = 0,
        .tint = {},
    };

    constexpr auto kBlurAmountPrefix = L"BlurAmount=\""sv;
    constexpr auto kTintColorPrefix = L"TintColor=\"#"sv;
    for (const auto prop : SplitStringView(substr, L" ")) {
        const auto propSubstr = TrimStringView(prop);
        if (propSubstr.empty()) {
            continue;
        }

        Wh_Log(L"  %.*s", static_cast<int>(propSubstr.length()),
               propSubstr.data());

        if (propSubstr.starts_with(kBlurAmountPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kBlurAmountPrefix),
                propSubstr.size() - std::size(kBlurAmountPrefix) - 1);
            value.blurAmount = std::stoi(std::wstring(valStr));
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
            value.tint = {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
            continue;
        }

        throw std::runtime_error("WindhawkBlur: Bad property");
    }

    return value;
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
                    ParseNonXamlPropertyOverrideValue(rule.value));

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
    // The TaskListButtonPanel child element of the search box (with "Icon and
    // label" configuration) returns a list of size 1, but accessing the first
    // item leads to a null dereference crash. Skip this element.
    if (winrt::get_class_name(element) == L"Taskbar.TaskListButtonPanel") {
        auto parent = Media::VisualTreeHelper::GetParent(element)
                          .try_as<FrameworkElement>();
        if (parent && winrt::get_class_name(parent) ==
                          L"Taskbar.SearchBoxLaunchListButton") {
            return nullptr;
        }
    }

    // Same as above for an updated element layout (around Jun 2025).
    if (winrt::get_class_name(element) ==
        L"SearchUx.SearchUI.SearchButtonRootGrid") {
        auto parent = Media::VisualTreeHelper::GetParent(element)
                          .try_as<FrameworkElement>();
        if (parent && winrt::get_class_name(parent) ==
                          L"SearchUx.SearchUI.SearchPillButton") {
            return nullptr;
        }
    }

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
            SetOrClearValue(element, property, it->second,
                            /*initialApply=*/true);
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

std::optional<StyleConstant> ParseStyleConstant(std::wstring_view constant) {
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
    auto val = TrimStringView(constant.substr(eqPos + 1));

    return StyleConstant{std::wstring(key), std::wstring(val)};
}

StyleConstants LoadStyleConstants(
    const std::vector<PCWSTR>& themeStyleConstants) {
    StyleConstants result;

    for (const auto themeStyleConstant : themeStyleConstants) {
        if (auto parsed = ParseStyleConstant(themeStyleConstant)) {
            result.push_back(std::move(*parsed));
        }
    }

    for (int i = 0;; i++) {
        string_setting_unique_ptr constantSetting(
            Wh_GetStringSetting(L"styleConstants[%d]", i));
        if (!*constantSetting.get()) {
            break;
        }

        if (auto parsed = ParseStyleConstant(constantSetting.get())) {
            result.push_back(std::move(*parsed));
        }
    }

    // Reverse the order to allow overriding definitions with the same name.
    std::reverse(result.begin(), result.end());

    // Sort by name length to replace long names first.
    std::stable_sort(result.begin(), result.end(),
                     [](const StyleConstant& a, const StyleConstant& b) {
                         return a.first.size() > b.first.size();
                     });

    return result;
}

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
            {L"taskbar:", L"Taskbar."},
            {L"systemtray:", L"SystemTray."},
            {L"udk:", L"WindowsUdk.UI.Shell."},
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

std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
    enum FEATURE_ENABLED_STATE {
        FEATURE_ENABLED_STATE_DEFAULT = 0,
        FEATURE_ENABLED_STATE_DISABLED = 1,
        FEATURE_ENABLED_STATE_ENABLED = 2,
    };

#pragma pack(push, 1)
    struct RTL_FEATURE_CONFIGURATION {
        unsigned int featureId;
        unsigned __int32 group : 4;
        FEATURE_ENABLED_STATE enabledState : 2;
        unsigned __int32 enabledStateOptions : 1;
        unsigned __int32 unused1 : 1;
        unsigned __int32 variant : 6;
        unsigned __int32 variantPayloadKind : 2;
        unsigned __int32 unused2 : 16;
        unsigned int payload;
    };
#pragma pack(pop)

    using RtlQueryFeatureConfiguration_t =
        int(NTAPI*)(UINT32, int, INT64*, RTL_FEATURE_CONFIGURATION*);
    static RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration = []() {
        HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
        return hNtDll ? (RtlQueryFeatureConfiguration_t)GetProcAddress(
                            hNtDll, "RtlQueryFeatureConfiguration")
                      : nullptr;
    }();

    if (!pRtlQueryFeatureConfiguration) {
        Wh_Log(L"RtlQueryFeatureConfiguration not found");
        return std::nullopt;
    }

    RTL_FEATURE_CONFIGURATION feature = {0};
    INT64 changeStamp = 0;
    HRESULT hr =
        pRtlQueryFeatureConfiguration(featureId, 1, &changeStamp, &feature);
    if (SUCCEEDED(hr)) {
        Wh_Log(L"RtlQueryFeatureConfiguration result for %u: %d", featureId,
               feature.enabledState);

        switch (feature.enabledState) {
            case FEATURE_ENABLED_STATE_DISABLED:
                return false;
            case FEATURE_ENABLED_STATE_ENABLED:
                return true;
            case FEATURE_ENABLED_STATE_DEFAULT:
                return std::nullopt;
        }
    } else {
        Wh_Log(L"RtlQueryFeatureConfiguration error for %u: %08X", featureId,
               hr);
    }

    return std::nullopt;
}

void ProcessAllStylesFromSettings() {
    PCWSTR themeName = Wh_GetStringSetting(L"theme");
    const Theme* theme = nullptr;
    if (wcscmp(themeName, L"WinXP") == 0) {
        theme = &g_themeWinXP;
    } else if (wcscmp(themeName, L"WinXP_variant_Zune") == 0) {
        theme = &g_themeWinXP_variant_Zune;
    } else if (wcscmp(themeName, L"Bubbles") == 0) {
        theme = &g_themeBubbles;
    } else if (wcscmp(themeName, L"TranslucentTaskbar") == 0) {
        theme = &g_themeTranslucentTaskbar;
    } else if (wcscmp(themeName, L"Squircle") == 0) {
        // Weather widget on the right.
        // https://www.reddit.com/r/Windows11/comments/1dnew8x/my_weather_widget_is_on_the_right_side/
        constexpr UINT32 kExtendedModeAEPForTaskbar = 48660958;
        theme = IsOsFeatureEnabled(kExtendedModeAEPForTaskbar).value_or(false)
                    ? &g_themeSquircle_variant_WeatherOnTheRight
                    : &g_themeSquircle;
    } else if (wcscmp(themeName, L"RosePine") == 0) {
        theme = &g_themeRosePine;
    } else if (wcscmp(themeName, L"DockLike") == 0) {
        theme = &g_themeDockLike;
    } else if (wcscmp(themeName, L"WinVista") == 0) {
        theme = &g_themeWinVista;
    } else if (wcscmp(themeName, L"CleanSlate") == 0) {
        theme = &g_themeCleanSlate;
    } else if (wcscmp(themeName, L"Lucent") == 0) {
        theme = &g_themeLucent;
    } else if (wcscmp(themeName, L"Lucent_variant_Light") == 0) {
        theme = &g_themeLucent_variant_Light;
    } else if (wcscmp(themeName, L"21996Taskbar") == 0) {
        theme = &g_theme21996Taskbar;
    } else if (wcscmp(themeName, L"BottomDensy") == 0) {
        theme = &g_themeBottomDensy;
    } else if (wcscmp(themeName, L"TaskbarXII") == 0) {
        theme = &g_themeTaskbarXII;
    } else if (wcscmp(themeName, L"xdark") == 0) {
        theme = &g_themexdark;
    } else if (wcscmp(themeName, L"Matter") == 0) {
        theme = &g_themeMatter;
    } else if (wcscmp(themeName, L"SimplyTransparent") == 0) {
        theme = &g_themeSimplyTransparent;
    } else if (wcscmp(themeName, L"Windows7") == 0) {
        theme = &g_themeWindows7;
    } else if (wcscmp(themeName, L"Aeris") == 0) {
        theme = &g_themeAeris;
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

bool ProcessSingleResourceVariableFromSettings(int index) {
    string_setting_unique_ptr variableKeyStringSetting(
        Wh_GetStringSetting(L"resourceVariables[%d].variableKey", index));
    if (!*variableKeyStringSetting.get()) {
        return false;
    }

    Wh_Log(L"Processing resource variable %s", variableKeyStringSetting.get());

    std::wstring_view variableKey = variableKeyStringSetting.get();

    auto resources = Application::Current().Resources();

    auto resource = resources.Lookup(winrt::box_value(variableKey));

    // Example: Windows.Foundation.IReference`1<Windows.UI.Xaml.Thickness>
    auto resourceClassName = winrt::get_class_name(resource);
    if (resourceClassName.starts_with(L"Windows.Foundation.IReference`1<") &&
        resourceClassName.ends_with(L'>')) {
        size_t prefixSize = sizeof("Windows.Foundation.IReference`1<") - 1;
        resourceClassName =
            winrt::hstring(resourceClassName.data() + prefixSize,
                           resourceClassName.size() - prefixSize - 1);
    }

    auto resourceTypeName = Interop::TypeName{resourceClassName};

    string_setting_unique_ptr valueStringSetting(
        Wh_GetStringSetting(L"resourceVariables[%d].value", index));

    std::wstring_view value = valueStringSetting.get();

    resources.Insert(winrt::box_value(variableKey),
                     Markup::XamlBindingHelper::ConvertValue(
                         resourceTypeName, winrt::box_value(value)));

    return true;
}

void ProcessResourceVariablesFromSettings() {
    for (int i = 0;; i++) {
        try {
            if (!ProcessSingleResourceVariableFromSettings(i)) {
                break;
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        } catch (std::exception const& ex) {
            Wh_Log(L"Error: %S", ex.what());
        }
    }
}

void UninitializeForCurrentThread() {
    for (const auto& [elementDo, asyncOp] : g_delayedBackgroundFillSet) {
        asyncOp.Cancel();
    }

    g_delayedBackgroundFillSet.clear();

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

void OnWindowCreated(HWND hWnd,
                     HWND hWndParent,
                     LPCWSTR lpClassName,
                     PCSTR funcName) {
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    WCHAR className[64];
    if (hWndParent && GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 &&
        GetClassName(hWndParent, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Initializing - Created DesktopWindowContentBridge window");
        InitializeForCurrentThread();
        InitializeSettingsAndTap();
        return;
    }

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"XamlExplorerHostIslandWindow") == 0) {
        Wh_Log(L"Initializing - Created XAML host window: %08X via %S",
               (DWORD)(ULONG_PTR)hWnd, funcName);
        InitializeForCurrentThread();
        InitializeSettingsAndTap();
        return;
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

    OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);

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

    OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);

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

    OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);

    return hWnd;
}

std::vector<HWND> GetXamlHostWnds() {
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

            if (_wcsicmp(szClassName, L"XamlExplorerHostIslandWindow") == 0) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

HWND GetTaskbarUiWnd() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return nullptr;
    }

    return FindWindowEx(hTaskbarWnd, nullptr,
                        L"Windows.UI.Composition.DesktopWindowContentBridge",
                        nullptr);
}

PTP_TIMER g_statsTimer;

bool StartStatsTimer() {
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

            FILETIME currentTimeFt;
            GetSystemTimeAsFileTime(&currentTimeFt);
            ULONGLONG currentTime =
                ((ULONGLONG)currentTimeFt.dwHighDateTime << 32) |
                currentTimeFt.dwLowDateTime;

            Wh_SetBinaryValue(L"statsTimerLastTime", &currentTime,
                              sizeof(currentTime));

            string_setting_unique_ptr themeName(Wh_GetStringSetting(L"theme"));
            if (!*themeName.get()) {
                return;
            }

            std::wstring themeNameEscaped = themeName.get();
            std::replace(themeNameEscaped.begin(), themeNameEscaped.end(), L' ',
                         L'_');

            std::wstring statsUrl =
                L"https://github.com/ramensoftware/"
                L"windows-11-taskbar-styling-guide/releases/download/stats-v1/";
            statsUrl += themeNameEscaped;
            statsUrl += L".txt";

            Wh_Log(L"Submitting stats to %s", statsUrl.c_str());

            const WH_URL_CONTENT* content =
                Wh_GetUrlContent(statsUrl.c_str(), nullptr);
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

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

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

    StartStatsTimer();

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    bool initialize = false;

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Initializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID) { InitializeForCurrentThread(); },
            nullptr);
        initialize = true;
    }

    for (auto hXamlHostWnd : GetXamlHostWnds()) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd, [](PVOID) { InitializeForCurrentThread(); }, nullptr);
        initialize = true;
    }

    if (initialize) {
        InitializeSettingsAndTap();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    StopStatsTimer();

    UninitializeSettingsAndTap();

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Uninitializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID) { UninitializeForCurrentThread(); },
            nullptr);
    }

    for (auto hXamlHostWnd : GetXamlHostWnds()) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd, [](PVOID) { UninitializeForCurrentThread(); },
            nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    UninitializeSettingsAndTap();

    bool initialize = false;

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Reinitializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd,
            [](PVOID) {
                UninitializeForCurrentThread();
                InitializeForCurrentThread();
            },
            nullptr);
        initialize = true;
    }

    for (auto hXamlHostWnd : GetXamlHostWnds()) {
        Wh_Log(L"Reinitializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd,
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
