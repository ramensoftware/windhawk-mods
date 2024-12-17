// ==WindhawkMod==
// @id              windows-11-taskbar-styler
// @name            Windows 11 Taskbar Styler
// @description     Customize the taskbar with themes contributed by others or create your own
// @version         1.3.8
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

* `ContextMenuMargin`: The margin between the taskbar and the notification area
  context menu.

* `MediumTaskbarButtonExtent`: The width of the taskbar buttons.

## Implementation notes

The VisualTreeWatcher implementation is based on the
[ExplorerTAP](https://github.com/TranslucentTB/TranslucentTB/tree/develop/ExplorerTAP)
code from the **TranslucentTB** project.
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
  - Bubbles: Bubbles
  - TranslucentTaskbar: TranslucentTaskbar
  - Squircle: Squircle
  - RosePine: RosePine
  - DockLike: DockLike
  - WinVista: WinVista
  - CleanSlate: CleanSlate
  - Lucent: Lucent (Accented Bar)
  - Lucent_variant_Light: Lucent (Light Bar)
- controlStyles:
  - - target: ""
      $name: Target
    - styles: [""]
      $name: Styles
  $name: Control styles
- resourceVariables:
  - - variableKey: ""
      $name: Variable key
    - value: ""
      $name: Value
  $name: Resource variables
*/
// ==/WindhawkModSettings==

#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
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
};

// clang-format off

const Theme g_themeWinXP = {{
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#3168d5\" Offset=\"0.0\" /> <GradientStop Color=\"#4993E6\" Offset=\"0.1\" /> <GradientStop Color=\"#2157D7\" Offset=\"0.35\" /> <GradientStop Color=\"#2663E0\" Offset=\"0.8\" /> <GradientStop Color=\"#1941A5\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"VerticalAlignment=Stretch",
        L"Height=Auto"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"CornerRadius=0",
        L"Margin=-4,0,4,0",
        L"MaxWidth=48"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"Padding=0",
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#388238\" Offset=\"0.0\" /> <GradientStop Color=\"#71B571\" Offset=\"0.1\" /> <GradientStop Color=\"#71B571\" Offset=\"0.35\" /> <GradientStop Color=\"#47AA47\" Offset=\"0.8\" /> <GradientStop Color=\"#307443\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Border#BackgroundElement", {
        L"Background:=<ImageBrush Stretch=\"None\" ImageSource=\"https://i.imgur.com/BvXJlkj.png\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"Foreground=White",
        L"Margin=-4,0,4,0"}},
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
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#3358B5\" Offset=\"0.0\" /> <GradientStop Color=\"#8AC4FD\" Offset=\"0.1\" /> <GradientStop Color=\"#3B8FEF\" Offset=\"0.2\" /> <GradientStop Color=\"#56A3FF\" Offset=\"0.85\" /> <GradientStop Color=\"#378DF6\" Offset=\"0.9\" /> <GradientStop Color=\"#163E95\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderThickness@ActiveRunningIndicator=1.5",
        L"BorderBrush@NoRunningIndicator=Transparent",
        L"BorderBrush@ActiveRunningIndicator=#1B67D7",
        L"BorderBrush=#3358B5"}},
    ThemeTargetStyles{L"Grid#RootGrid > Grid", {
        L"Margin=12,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"Margin=-1.5"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0.42\" EndPoint=\"0.5,0.75\"> <GradientStop Color=\"#3168d5\" Offset=\"0.0\" /> <GradientStop Color=\"#0C40B1\" Offset=\"0.5\" /> <GradientStop Color=\"#1941A5\" Offset=\"0.35\" /> <GradientStop Color=\"#1941A5\" Offset=\"0.8\" /> <GradientStop Color=\"#1941A5\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderThickness=1,0,0,0",
        L"BorderBrush=#2244BB",
        L"Padding=4,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel@RunningIndicatorStates > Windows.UI.Xaml.Controls.Image#Icon", {
        L"Height@NoRunningIndicator=16"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#3168d5\" Offset=\"0.0\" /> <GradientStop Color=\"#4993E6\" Offset=\"0.1\" /> <GradientStop Color=\"#2157D7\" Offset=\"0.35\" /> <GradientStop Color=\"#2663E0\" Offset=\"0.8\" /> <GradientStop Color=\"#1941A5\" Offset=\"1.0\" /></LinearGradientBrush>"}},
}};

const Theme g_themeBubbles = {{
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill=#EE080810"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background=#303030",
        L"CornerRadius=20",
        L"Background@NoRunningIndicator=#40303030"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background=#303030",
        L"CornerRadius=20",
        L"Background@ActivePointerOver=#202020",
        L"Background@InactivePointerOver=#202020",
        L"Background@ActivePressed=#101010",
        L"Background@InactivePressed=#101010"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background=#303030",
        L"CornerRadius=20",
        L"Margin=0,5,4,5",
        L"Padding=10,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle#RunningIndicator", {
        L"Width=40",
        L"Height=40",
        L"Stroke@InactivePointerOver=#75A8E6",
        L"Stroke@InactivePressed=#7CB1F2",
        L"Stroke@ActiveNormal=#5F87B9",
        L"Stroke@ActivePointerOver=#75A8E6",
        L"Stroke@ActivePressed=#7CB1F2",
        L"Fill=Transparent",
        L"RadiusX=20",
        L"RadiusY=20",
        L"StrokeThickness=3",
        L"Margin=0",
        L"Stroke@MultiWindowPointerOver=#CCCCDD",
        L"Stroke@MultiWindowPressed=White",
        L"Stroke@MultiWindowActive=#BBBBCC",
        L"Fill@MultiWindowNormal=#88AAAABB",
        L"Fill@MultiWindowPointerOver=#88AAAABB",
        L"Fill@MultiWindowActive=#88AAAABB",
        L"Fill@MultiWindowPressed=#88AAAABB"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=4,0,0,0",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Height=48",
        L"Margin=0,-2,0,0"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Height=0"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background=#EE080810",
        L"BorderBrush=#303030",
        L"BorderThickness=2.5"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Margin=1,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Padding=5,0,5,0",
        L"Margin=2,0,10,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ShowDesktopPipe", {
        L"MinWidth=4",
        L"RadiusX=2",
        L"RadiusY=2"}},
    ThemeTargetStyles{L"SystemTray.Stack#NotifyIconStack > Windows.UI.Xaml.Controls.Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=16,5,5,16",
        L"Margin=-3,4,0,4"}},
}};

const Theme g_themeTranslucentTaskbar = {{
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.15\" Opacity=\"1\" FallbackColor=\"#70262626\"/>"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.12\" Opacity=\"1\" FallbackColor=\"#A0262626\"/>",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=14",
        L"Padding=3,4,3,4"}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.12\" Opacity=\"1\" FallbackColor=\"#A0262626\"/>",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=15",
        L"Margin=-2,-2,-2,-2"}},
}};

const Theme g_themeSquircle = {{
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
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
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=5",
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\"/>",
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
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"FontSize=16"}},
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
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
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
    ThemeTargetStyles{L"Taskbar.TaskbarFrame#TaskbarFrame", {
        L"Width=Auto",
        L"HorizontalAlignment=Center",
        L"Margin=250,0,250,0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame#TaskbarFrame > Grid#RootGrid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
        L"Padding=6,0,6,0",
        L"CornerRadius=8,8,0,0",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" />"}},
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
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
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
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
        L"Fill=Transparent",
        L"RadiusX=5",
        L"RadiusY=5",
        L"Height=40",
        L"Margin=0,-4,0,0",
        L"Canvas.ZIndex=1",
        L"MinWidth=40"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=8,0,0,0",
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
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"Margin=0,3,0,3",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement@CommonStates", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" />"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Border#BackgroundElement", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle#RunningIndicator", {
        L"StrokeThickness=1",
        L"Stroke@InactivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Stroke@InactivePressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Stroke@ActiveNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" />",
        L"Stroke@ActivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />",
        L"Stroke@ActivePressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background@InactiveRunningIndicator:=<SolidColorBrush Color=\"Black\" Opacity=\"0.4\" />",
        L"Background@InactiveRunningIndicator:=<SolidColorBrush Color=\"Black\" Opacity=\"0.4\" />",
        L"Background@ActiveRunningIndicator:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark2}\" Opacity=\"0.4\" />",
        L"Background@NoRunningIndicator:=Transparent",
        L"Background@RequestingAttentionRunningIndicator:=<SolidColorBrush Color=\"#ffdf5e\" Opacity=\"0.4\" />",
        L"Margin=1"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe", {
        L"Width=12",
        L"Height=38",
        L"Margin=-6,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Width=12"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel", {
        L"Margin=-3,0,0,0"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />"}},
}};

const Theme g_themeLucent = {{
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#00000000\" Offset=\"0.3\" /><GradientStop Color=\"#AA000000\" Offset=\"0.9\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Rectangle#RunningIndicator", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"CornerRadius=15",
        L"Background@ActiveRunningIndicator:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@InactiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#3300290c\" Offset=\"0.1\" /><GradientStop Color=\"{ThemeResource SystemAccentColorDark2}\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Margin@ActiveRunningIndicator=-4",
        L"Margin=0,-1,0,-1",
        L"CornerRadius@ActiveRunningIndicator=2",
        L"CornerRadius@InactiveRunningIndicator=0",
        L"Margin@InactiveRunningIndicator=-4",
        L"Margin@RequestingAttentionRunningIndicator=0,-4,0,-4",
        L"CornerRadius@RequestingAttentionRunningIndicator=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > TextBlock#LabelControl", {
        L"Foreground@ActiveNormal=Black",
        L"Foreground@ActivePointerOver=Black",
        L"MaxWidth=450",
        L"Margin=0,0,3,0"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Grid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50000000\" Offset=\"0.3\" /><GradientStop Color=\"#EE000000\" Offset=\"0.9\" /></LinearGradientBrush>",
        L"Margin=0,0,0,2",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"Padding=20"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=2"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#80000000\" Offset=\"0.0\" /><GradientStop Color=\"#FF000000\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Padding=0",
        L"CornerRadius=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#3300290c\" Offset=\"0.1\" /><GradientStop Color=\"{ThemeResource SystemAccentColorDark2}\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>"}},
}};

const Theme g_themeLucent_variant_Light = {{
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#00000000\" Offset=\"0.3\" /><GradientStop Color=\"#AA000000\" Offset=\"0.9\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Rectangle#RunningIndicator", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"CornerRadius=15",
        L"Background@ActiveRunningIndicator:=#FCFCFC",
        L"Background@InactiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#3300290c\" Offset=\"0.1\" /><GradientStop Color=\"{ThemeResource SystemAccentColorDark2}\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Margin@ActiveRunningIndicator=-4",
        L"Margin=0,-1,0,-1",
        L"CornerRadius@ActiveRunningIndicator=2",
        L"CornerRadius@InactiveRunningIndicator=0",
        L"Margin@InactiveRunningIndicator=-4",
        L"Margin@RequestingAttentionRunningIndicator=0,-4,0,-4",
        L"CornerRadius@RequestingAttentionRunningIndicator=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > TextBlock#LabelControl", {
        L"Foreground@ActiveNormal=Black",
        L"Foreground@ActivePointerOver=Black",
        L"MaxWidth=450",
        L"Margin=0,0,3,0"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Grid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50000000\" Offset=\"0.3\" /><GradientStop Color=\"#EE000000\" Offset=\"0.9\" /></LinearGradientBrush>",
        L"Margin=0,0,0,2",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"Padding=20"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=2"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#80000000\" Offset=\"0.0\" /><GradientStop Color=\"#FF000000\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Padding=0",
        L"CornerRadius=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#3300290c\" Offset=\"0.1\" /><GradientStop Color=\"{ThemeResource SystemAccentColorDark2}\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>"}},
}};

// clang-format on

std::atomic<DWORD> g_targetThreadId = 0;

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

#include <guiddef.h>
#include <Unknwn.h>
#include <winrt/base.h>

// forward declare namespaces we alias
namespace winrt {
    namespace Windows {
        namespace Foundation::Collections {}
        namespace UI::Xaml {
            namespace Controls {}
            namespace Hosting {}
        }
    }
}

// alias some long namespaces for convenience
namespace wf = winrt::Windows::Foundation;
// namespace wfc = wf::Collections;
namespace wux = winrt::Windows::UI::Xaml;
// namespace wuxc = wux::Controls;
namespace wuxh = wux::Hosting;

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

    template<typename T>
    T FromHandle(InstanceHandle handle)
    {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));

        return obj.as<T>();
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
};

#pragma endregion  // visualtreewatcher_hpp

#pragma region visualtreewatcher_cpp

#include <winrt/Windows.UI.Xaml.Hosting.h>

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site) :
    m_XamlDiagnostics(site.as<IXamlDiagnostics>())
{
    Wh_Log(L"Constructing VisualTreeWatcher");
    // winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(this));

    // Calling AdviseVisualTreeChange from the current thread causes the app to
    // hang on Windows 10 in Advising::RunOnUIThread. Creating a new thread and
    // calling it from there fixes it.
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            auto watcher = reinterpret_cast<VisualTreeWatcher*>(lpParam);
            HRESULT hr = watcher->m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(watcher);
            if (FAILED(hr)) {
                Wh_Log(L"Error %08X", hr);
            }
            return 0;
        },
        this, 0, nullptr);
    if (thread) {
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
    if (GetCurrentThreadId() != g_targetThreadId)
    {
        return S_OK;
    }

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
        const auto inspectable = FromHandle<wf::IInspectable>(element.Handle);

        auto frameworkElement = inspectable.try_as<wux::FrameworkElement>();
        if (!frameworkElement)
        {
            const auto desktopXamlSource = FromHandle<wuxh::DesktopWindowXamlSource>(element.Handle);
            frameworkElement = desktopXamlSource.Content().try_as<wux::FrameworkElement>();
        }

        if (frameworkElement)
        {
            Wh_Log(L"FrameworkElement name: %s", frameworkElement.Name().c_str());
            ApplyCustomizations(element.Handle, frameworkElement, element.Type);
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

// TODO: weak_ref might be better here.
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
    return winrt::to_hresult();
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
        return winrt::to_hresult();
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
    return winrt::to_hresult();
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

    const HRESULT hr2 = ixde(L"VisualDiagConnection1", GetCurrentProcessId(), nullptr, location, CLSID_WindhawkTAP, nullptr);
    if (FAILED(hr2)) [[unlikely]]
    {
        return hr2;
    }

    return S_OK;
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

#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
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

// Property -> visual state -> value.
using PropertyOverrides = std::unordered_map<
    DependencyProperty,
    std::unordered_map<std::wstring, winrt::Windows::Foundation::IInspectable>>;

using PropertyOverridesMaybeUnresolved =
    std::variant<PropertyOverridesUnresolved, PropertyOverrides>;

struct ElementCustomizationRules {
    ElementMatcher elementMatcher;
    std::vector<ElementMatcher> parentElementMatchers;
    PropertyOverridesMaybeUnresolved propertyOverrides;
};

std::vector<ElementCustomizationRules> g_elementsCustomizationRules;

struct ElementPropertyCustomizationState {
    std::optional<winrt::Windows::Foundation::IInspectable> originalValue;
    std::optional<winrt::Windows::Foundation::IInspectable> customValue;
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

std::unordered_map<InstanceHandle, ElementCustomizationState>
    g_elementsCustomizationState;

bool g_elementPropertyModifying;

struct BackgroundFillDelayedApplyData {
    UINT_PTR timer = 0;
    winrt::weak_ref<wux::FrameworkElement> element;
    std::wstring fallbackClassName;
};

std::unordered_map<InstanceHandle, BackgroundFillDelayedApplyData>
    g_backgroundFillDelayedApplyData;

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

void SetOrClearValue(DependencyObject elementDo,
                     DependencyProperty property,
                     winrt::Windows::Foundation::IInspectable value) {
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
        if (property == Controls::TextBlock::FontWeightProperty()) {
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
    for (size_t pos = 0; pos != data.size(); ++pos) {
        switch (data[pos]) {
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
                buffer.append(&data[pos], 1);
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

const PropertyOverrides& GetResolvedPropertyOverrides(
    const std::wstring_view type,
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

            for (const auto& rule : styleRules) {
                xaml += L"        <Setter Property=\"";
                xaml += EscapeXmlAttribute(rule.name);
                xaml += L"\"";
                if (rule.isXamlValue && rule.value.empty()) {
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

            auto style = GetStyleFromXamlSetters(type, xaml);

            uint32_t i = 0;
            for (const auto& rule : styleRules) {
                const auto setter = style.Setters().GetAt(i++).as<Setter>();
                propertyOverrides[setter.Property()][rule.visualState] =
                    rule.isXamlValue && rule.value.empty()
                        ? DependencyProperty::UnsetValue()
                        : setter.Value();
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
    // The TaskListButtonPanel child element of the search box (with "Icon and
    // label" configuration) returns a list of size 1, but accessing the first
    // item leads to a null dereference crash. Skip this element.
    if (winrt::get_class_name(element) == L"Taskbar.TaskListButtonPanel" &&
        element.Name() == L"ExperienceToggleButtonRootPanel") {
        auto parent = Media::VisualTreeHelper::GetParent(element)
                          .try_as<FrameworkElement>();
        if (parent &&
            winrt::get_class_name(parent) ==
                L"Taskbar.SearchBoxLaunchListButton" &&
            parent.Name() == L"SearchBoxLaunchListButton") {
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

    for (const auto& propertyValue :
         GetResolvedPropertyValues(matcher.type, &matcher.propertyValues)) {
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
             GetResolvedPropertyOverrides(override.elementMatcher.type,
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

                    Wh_Log(L"Re-applying style for %s",
                           winrt::get_class_name(element).c_str());

                    auto localValue =
                        ReadLocalValueWithWorkaround(element, property);

                    if (*propertyCustomizationState.customValue != localValue) {
                        propertyCustomizationState.originalValue = localValue;
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

void ApplyCustomizationsWithNoDelay(InstanceHandle handle,
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

void ApplyCustomizations(InstanceHandle handle,
                         FrameworkElement element,
                         PCWSTR fallbackClassName) {
    if (winrt::get_class_name(element) == L"Windows.UI.Xaml.Shapes.Rectangle" &&
        element.Name() == L"BackgroundFill") {
        // If customized before
        // `winrt::Taskbar::implementation::TaskbarBackground::OnApplyTemplate`
        // is executed, it can lead to a crash, or the customization may be
        // overridden. See:
        // https://github.com/ramensoftware/windows-11-taskbar-styling-guide/issues/4
        Wh_Log(L"Delaying customization of BackgroundFill");

        auto& delayedApplyData = g_backgroundFillDelayedApplyData[handle];

        auto previousTimer = delayedApplyData.timer;

        delayedApplyData.element = element;
        delayedApplyData.fallbackClassName = fallbackClassName;

        delayedApplyData.timer = SetTimer(
            nullptr, previousTimer, 0,
            [](HWND hwnd,         // handle of window for timer messages
               UINT uMsg,         // WM_TIMER message
               UINT_PTR idEvent,  // timer identifier
               DWORD dwTime       // current system time
               ) WINAPI {
                Wh_Log(L"Running delayed customization of BackgroundFill");

                for (auto it = g_backgroundFillDelayedApplyData.begin();
                     it != g_backgroundFillDelayedApplyData.end(); ++it) {
                    auto& delayedApplyData = it->second;
                    if (delayedApplyData.timer != idEvent) {
                        continue;
                    }

                    InstanceHandle handle = it->first;

                    if (auto element = delayedApplyData.element.get()) {
                        ApplyCustomizationsWithNoDelay(
                            handle, std::move(element),
                            delayedApplyData.fallbackClassName.c_str());
                    } else {
                        Wh_Log(L"Element no longer exists");
                    }

                    KillTimer(nullptr, delayedApplyData.timer);
                    g_backgroundFillDelayedApplyData.erase(it);
                    break;
                }
            });

        return;
    }

    ApplyCustomizationsWithNoDelay(handle, std::move(element),
                                   fallbackClassName);
}

void CleanupCustomizations(InstanceHandle handle) {
    if (auto it = g_backgroundFillDelayedApplyData.find(handle);
        it != g_backgroundFillDelayedApplyData.end()) {
        KillTimer(nullptr, it->second.timer);
        g_backgroundFillDelayedApplyData.erase(it);
    }

    auto it = g_elementsCustomizationState.find(handle);
    if (it == g_elementsCustomizationState.end()) {
        return;
    }

    auto& elementCustomizationState = it->second;

    auto element = elementCustomizationState.element.get();

    for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
        RestoreCustomizationsForVisualStateGroup(
            element, visualStateGroupOptionalWeakPtrIter, stateIter);
    }

    g_elementsCustomizationState.erase(it);
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
        if (result.visualState.empty()) {
            throw std::runtime_error("Bad style syntax, empty visual state");
        }

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

bool ProcessSingleTargetStylesFromSettings(int index) {
    string_setting_unique_ptr targetStringSetting(
        Wh_GetStringSetting(L"controlStyles[%d].target", index));
    if (!*targetStringSetting.get()) {
        return false;
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

        styles.push_back(styleSetting.get());
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
    }
    Wh_FreeStringSetting(themeName);

    if (theme) {
        for (const auto& themeTargetStyle : theme->targetStyles) {
            try {
                std::vector<std::wstring> styles{
                    themeTargetStyle.styles.begin(),
                    themeTargetStyle.styles.end()};
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
            if (!ProcessSingleTargetStylesFromSettings(i)) {
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

void UninitializeSettingsAndTap() {
    for (auto& [handle, data] : g_backgroundFillDelayedApplyData) {
        KillTimer(nullptr, data.timer);
    }

    g_backgroundFillDelayedApplyData.clear();

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

    g_targetThreadId = 0;
}

void InitializeSettingsAndTap() {
    DWORD kNoThreadId = 0;
    if (!g_targetThreadId.compare_exchange_strong(kNoThreadId,
                                                  GetCurrentThreadId())) {
        return;
    }

    ProcessAllStylesFromSettings();
    ProcessResourceVariablesFromSettings();

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"Error %08X", hr);
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

    WCHAR className[64];
    if (!g_targetThreadId && hWndParent &&
        GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 &&
        GetClassName(hWndParent, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Initializing - Created DesktopWindowContentBridge window");
        InitializeSettingsAndTap();
    }

    return hWnd;
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
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
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

HWND GetTaskbarUiWnd() {
    DWORD dwProcessId;
    DWORD dwCurrentProcessId = GetCurrentProcessId();

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTaskbarWnd || !GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId) ||
        dwProcessId != dwCurrentProcessId) {
        return nullptr;
    }

    return FindWindowEx(hTaskbarWnd, nullptr,
                        L"Windows.UI.Composition.DesktopWindowContentBridge",
                        nullptr);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Initializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID) WINAPI { InitializeSettingsAndTap(); },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Uninitializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID) WINAPI { UninitializeSettingsAndTap(); },
            nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Reinitializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd,
            [](PVOID) WINAPI {
                UninitializeSettingsAndTap();
                InitializeSettingsAndTap();
            },
            nullptr);
    }
}
