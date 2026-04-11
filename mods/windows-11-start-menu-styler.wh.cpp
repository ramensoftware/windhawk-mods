// ==WindhawkMod==
// @id              windows-11-start-menu-styler
// @name            Windows 11 Start Menu Styler
// @description     Customize the start menu with themes contributed by others or create your own
// @version         1.4.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         SearchApp.exe
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
# Windows 11 Start Menu Styler

Customize the start menu with themes contributed by others or create your own.

Also check out the **Windows 11 Taskbar Styler**, **Windows 11 Notification
Center Styler** mods.

## Themes

Themes are collections of styles. The following themes are integrated into the
mod and can be selected in the settings:

[![TranslucentStartMenu](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/TranslucentStartMenu/screenshot-small.png)
\
TranslucentStartMenu](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/TranslucentStartMenu/README.md)

[![NoRecommendedSection](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/NoRecommendedSection/screenshot-small.png)
\
NoRecommendedSection](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/NoRecommendedSection/README.md)

[![SideBySide](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/SideBySide/screenshot-small.png)
\
SideBySide](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/SideBySide/README.md)

[![SideBySide2](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/SideBySide2/screenshot-small.png)
\
SideBySide2](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/SideBySide2/README.md)

[![SideBySideMinimal](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/SideBySideMinimal/screenshot-small.png)
\
SideBySideMinimal](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/SideBySideMinimal/README.md)

[![Down
Aero](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Down%20Aero/screenshot-small.png)
\
Down
Aero](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Down%20Aero/README.md)

[![Windows10](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Windows10/screenshot-small.png)
\
Windows10](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Windows10/README.md)

[![Windows11_Metro10](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Windows11_Metro10/screenshot-small.png)
\
Windows11_Metro10](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Windows11_Metro10/README.md)

[![Fluent2Inspired](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Fluent2Inspired/screenshot-small.png)
\
Fluent2Inspired](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Fluent2Inspired/README.md)

[![RosePine](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/RosePine/screenshot-small.png)
\
RosePine](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/RosePine/README.md)

[![Windows11_Metro10Minimal](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Windows11_Metro10Minimal/screenshot-small.png)
\
Windows11_Metro10Minimal](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Windows11_Metro10Minimal/README.md)

[![Everblush](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Everblush/screenshot-small.png)
\
Everblush](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Everblush/README.md)

[![SunValley](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/SunValley/screenshot-small.png)
\
SunValley](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/SunValley/README.md)

[![SunValley
(Legacy)](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/21996/screenshot-small.png)
\
SunValley
(Legacy)](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/21996/README.md)

[![UniMenu](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/UniMenu/screenshot-small.png)
\
UniMenu](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/UniMenu/README.md)

[![LegacyFluent](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/LegacyFluent/screenshot-small.png)
\
LegacyFluent](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/LegacyFluent/README.md)

[![OnlySearch](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/OnlySearch/screenshot-small.png)
\
OnlySearch](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/OnlySearch/README.md)

[![WindowGlass](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/WindowGlass/screenshot-small.png)
\
WindowGlass](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/WindowGlass/README.md)

[![Fluid](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Fluid/screenshot-small.png)
\
Fluid](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Fluid/README.md)

[![Oversimplified&Accentuated](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Oversimplified&Accentuated/screenshot-small.png)
\
Oversimplified&Accentuated](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Oversimplified&Accentuated/README.md)

[![LiquidGlass](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/LiquidGlass/screenshot-small.png)
\
LiquidGlass](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/LiquidGlass/README.md)

[![Windows10X](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Windows10X/screenshot-small.png)
\
Windows10X](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Windows10X/README.md)

[![TintedGlass](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/TintedGlass/screenshot-small.png)
\
TintedGlass](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/TintedGlass/README.md)

[![LayerMicaUI](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/LayerMicaUI/screenshot-small.png)
\
LayerMicaUI](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/LayerMicaUI/README.md)

More themes can be found in the **Themes** section of [The Windows 11 start menu
styling
guide](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/README.md#themes).
Contributions of new themes are welcome!

## Advanced styling

Aside from themes, the settings have two sections: control styles and resource
variables. Control styles allow to override styles, such as size and color, for
the target elements. Resource variables allow to override predefined variables.
For a more detailed explanation and examples, refer to the sections below.

The start menu's XAML resources can help find out which elements and resource
variables can be customized. To the best of my knowledge, there are no public
tools that are able to decode the resource files of recent Windows versions, but
here are XAML resources which were obtained via other means for your
convenience:
[StartResources.xbf](https://gist.github.com/m417z/a7e4e2c7b451ee79c62c51ca2dba7349).

The [UWPSpy](https://ramensoftware.com/uwpspy) tool can be used to inspect the
start menu control elements in real time, and experiment with various styles.

For a collection of commonly requested start menu styling customizations, check
out [The Windows 11 start menu styling
guide](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/README.md).

### Control styles

Each entry has a target control and a list of styles.

The target control is written as `Class` or `Class#Name`, i.e. the target
control class name (the tag name in XAML resource files), such as
`StartMenu.StartInnerFrame` or `Rectangle`, optionally followed by `#` and the
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
`BlurAmount`, `TintColor`, `TintOpacity`, `TintLuminosityOpacity`,
`TintSaturation`, `NoiseOpacity`, and `NoiseDensity` properties. For example:
`Fill:=<WindhawkBlur BlurAmount="10" TintColor="#80FF00FF"/>`. Theme resources
are also supported, for example: `Fill:=<WindhawkBlur BlurAmount="18"
TintColor="{ThemeResource SystemAccentColorDark1}" TintOpacity="0.5"/>`.

Targets and styles starting with two slashes (`//`) are ignored. This can be
useful for temporarily disabling a target or style.

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

Some themes use style constants to allow easy customization. Refer to the
theme page for details on which constants are available.

### Search WebView styles

While the start menu uses WinUI for its user interface, most of the search
content (all but the top search bar) is a WebView element. To style the search
WebView, CSS targets and styles can be used. For example, to set a red
background, the target `body` and the style `background: red !important` can be
used.

### Search WebView custom JavaScript code

Custom JavaScript code can be injected into the search content WebView. One use
case example is loading the [DOM
Inspector](https://github.com/janmyler/DOM-inspector) script to inspect the
search content elements:

![Screenshot](https://i.imgur.com/19PL0ss.png)

The following JavaScript code can be used to load a bundled version of DOM
Inspector:

```
const s=document.createElement('script');s.setAttribute('src','https://m417z.github.io/DOM-inspector/acid-dom/bundled.js');document.head.appendChild(s);
```

To reset all side-effects of the injected scripts, clear the custom code in the
mod settings and then terminate the search host process. It will be relaunched
automatically by Windows.

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
  - TranslucentStartMenu: TranslucentStartMenu
  - NoRecommendedSection: NoRecommendedSection
  - SideBySide: SideBySide
  - SideBySide2: SideBySide2
  - SideBySideMinimal: SideBySideMinimal
  - Down Aero: Down Aero
  - Windows10: Windows10
  - Windows10_variant_Minimal: Windows10 (Minimal)
  - Windows11_Metro10: Windows11_Metro10
  - Fluent2Inspired: Fluent2Inspired
  - RosePine: RosePine
  - Windows11_Metro10Minimal: Windows11_Metro10Minimal
  - Everblush: Everblush
  - SunValley: SunValley
  - 21996: SunValley (Legacy)
  - UniMenu: UniMenu
  - LegacyFluent: LegacyFluent
  - OnlySearch: OnlySearch
  - WindowGlass: WindowGlass (for the redesigned Start menu)
  - WindowGlass_variant_Minimal: WindowGlass (Minimal) (legacy, use WindowGlass)
  - Fluid: Fluid (for the redesigned Start menu)
  - Oversimplified&Accentuated: Oversimplified&Accentuated
  - LiquidGlass: LiquidGlass (for the redesigned Start menu)
  - Windows10X: Windows10X
  - TintedGlass: TintedGlass
  - LayerMicaUI: LayerMicaUI (for the redesigned Start menu)
- disableNewStartMenuLayout: ""
  $name: Disable the new start menu layout
  $description: >-
    Allows to disable the new start menu layout which is incompatible with some
    themes.
  $options:
  - "": Don't disable (use Windows default)
  - 1: Disable new layout and Phone Link
  - disableNewLayoutKeepPhoneLink: Disable new layout but keep Phone Link
  - forceNewLayout: Force new layout (if available)
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
- webContentStyles:
  - - target: ""
      $name: Target
    - styles: [""]
      $name: Styles
  $name: Search WebView styles
- webContentCustomJs: ""
  $name: Search WebView custom JavaScript code
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
    std::vector<ThemeTargetStyles> webViewTargetStyles;
};

// clang-format off

const Theme g_themeTranslucentStartMenu = {{
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#BorderElement", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>",
        L"BorderThickness=0",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#22000000\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Border#AccentAppBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>",
        L"BorderThickness=0",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton > ContentPresenter@CommonStates", {
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15C0C0C0\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > Border#BorderElement", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Grid@CommonStates > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid@CommonStates > Border", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.45\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1",
        L"Margin@Normal=4"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid@CommonStates > Border", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.45\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1",
        L"Margin@Normal=4"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#22000000\"/>"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"CornerRadius=16",
        L"Margin=-1"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"CornerRadius=15",
        L"Margin=-1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#Text", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid#RootGrid", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.6\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Button > Grid@CommonStates > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.6\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"DropDownButton", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.2\"/>"}},
    ThemeTargetStyles{L"Button#Header > Border#Border@CommonStates", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.6\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"StartMenu.FolderModal > Grid > Border", {
        L"Background:=$CommonBgBrush",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"ListViewItem > Grid#ContentBorder@CommonStates", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.9\"/>",
        L"BorderThickness=1",
        L"CornerRadius=5"}},
}, {
    L"CommonBgBrush=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#25323232\"/>",
}};

const Theme g_themeTranslucentStartMenu_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#BorderElement", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>",
        L"BorderThickness=0",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=504"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#00000000\"/>",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Border#AccentAppBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>",
        L"BorderThickness=0",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"0\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Margin=1",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton > ContentPresenter@CommonStates", {
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15C0C0C0\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid > Border#BorderElement", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Grid@CommonStates > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"0\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid@CommonStates > Border", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.45\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1",
        L"Margin@Normal=4"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid@CommonStates > Border", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.45\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1",
        L"Margin@Normal=4"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#00000000\"/>"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListViewItem > Grid@CommonStates > Border", {
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.55\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton > ContentPresenter@CommonStates", {
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15C0C0C0\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.AllAppsZoomListViewItem > Grid@CommonStates > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"0\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.6\"/>"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"CornerRadius=16",
        L"Margin=-1"}},
    ThemeTargetStyles{L"Border#DropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#RootGridDropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#RightCompanionDropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListViewItem > Grid#ContentBorder@CommonStates", {
        L"Background@PointerOver:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15C0C0C0\"/>",
        L"CornerRadius=4"}},
}, {
    L"CommonBgBrush=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#25323232\"/>",
}};

const Theme g_themeNoRecommendedSection = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText", {
        L"Height=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"RenderTransform:=<TranslateTransform Y=\"8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowMoreSuggestionsButton > Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=Recommended"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot > Grid[2]", {
        L"MinHeight=0"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"Grid.Row=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#RecommendedList", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"RenderTransform:=<TranslateTransform Y=\"8\"/>"}},
    ThemeTargetStyles{L"GridView", {
        L"Margin=0,-8,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"RenderTransform:=<TranslateTransform Y=\"-5\" />"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid[2] > Button", {
        L"RenderTransform:=<TranslateTransform X=\"-135\" />"}},
}};

const Theme g_themeNoRecommendedSection_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsContainer", {
        L"Height=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#RecommendedList", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=504"}},
}};

const Theme g_themeSideBySide = {{
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"MinHeight=420",
        L"MaxHeight=420"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"Margin=-40,0,40,0",
        L"Width=325"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid", {
        L"CornerRadius=8",
        L"Margin=-85,0,0,0",
        L"Width=350"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid > Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-15,0,0,0"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"Width=825"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"Height=825"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Margin=0,-70,0,0"}},
    ThemeTargetStyles{L"GridView#PinnedList", {
        L"MaxWidth=480",
        L"RenderTransform:=<TranslateTransform X=\"-145\" Y=\"790\"/>",
        L"MinHeight=420",
        L"MaxHeight=420"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid", {
        L"Width=280",
        L"Margin=55,12,-55,0"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter", {
        L"RenderTransform:=<TranslateTransform Y=\"-795\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"Margin=-60,170,60,-170",
        L"FontWeight=SemiBold",
        L"Height=32",
        L"Width=200",
        L"Style:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#ZoomedOutListView", {
        L"Margin=0,-35,0,35"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Visibility=Visible",
        L"RenderTransform:=<TranslateTransform X=\"-4\" Y=\"788.5\"/>",
        L"FontWeight=SemiBold"}},
    ThemeTargetStyles{L"StartMenu.StartHome", {
        L"RenderTransform:=<TranslateTransform Y=\"-1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Frame > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Margin=0,-15,0,0"}},
    ThemeTargetStyles{L"DropDownButton > Grid > ContentPresenter > TextBlock", {
        L"MaxLines=2",
        L"TextLineBounds=0",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"RenderTransform:=<TranslateTransform X=\"-160\" Y=\"800\"/>",
        L"Width=450",
        L"MinHeight=129",
        L"BorderThickness=0,1,0,0",
        L"BorderBrush=#22BBBBBB"}},
    ThemeTargetStyles{L"TextBlock#TopLevelSuggestionsListHeaderText", {
        L"RenderTransform:=<TranslateTransform X=\"-50\" />"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton", {
        L"RenderTransform:=<TranslateTransform X=\"50\" />"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid", {
        L"Margin=485,175,0,0"}},
    ThemeTargetStyles{L"GridView#RecommendedList > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem > Border", {
        L"MaxWidth=185",
        L"HorizontalAlignment=2"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Width=630",
        L"Margin=-50,0,0,0"}},
    ThemeTargetStyles{L"GridView#RecommendedList > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem", {
        L"Margin=-25,0,-25,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView", {
        L"Margin=15,0,-15,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Height=650",
        L"RenderTransform:=<TranslateTransform Y=\"-50\" />"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Grid", {
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#PinnedList > Border > Windows.UI.Xaml.Controls.ScrollViewer", {
        L"ScrollViewer.VerticalScrollMode=2"}},
}};

const Theme g_themeSideBySide_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Grid#UndockedRoot", {
        L"MaxWidth=700",
        L"Margin=0,0,300,0"}},
    ThemeTargetStyles{L"Grid#AllAppsRoot", {
        L"Visibility=Visible",
        L"MinWidth=390",
        L"Padding=-40,0,110,0",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\"1\" Opacity=\"1\"/>",
        L"Margin=-300,0,745,1"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Grid#AllAppsRoot", {
        L"Margin=-516,0,745,1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MaxWidth=860",
        L"Width=860"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid", {
        L"Width=644"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Margin=-22,-5,0,0"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Margin=45,-15,0,0"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListView > Windows.UI.Xaml.Controls.ScrollViewer > Border > Grid > Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Margin=-8,0,8,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-8,0,8,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem", {
        L"MaxWidth=185",
        L"MinWidth=85"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"Margin=-15,0,5,0"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Margin=0,20,0,-20"}},
    ThemeTargetStyles{L"Grid#MoreSuggestionsRoot", {
        L"Margin=-1,0,-4,-30"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#MoreSuggestionsListHeaderText", {
        L"Margin=-40,0,0,0"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton", {
        L"Margin=0,-58,25,0"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Margin=30,-10,30,-60"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridViewItem", {
        L"Margin=0"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0.1\" TintLuminosityOpacity=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.SemanticZoom#ZoomControl", {
        L"IsZoomOutButtonEnabled=true"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE73F"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton", {
        L"Width=24",
        L"Height=24",
        L"FontSize=14",
        L"CornerRadius=4",
        L"VerticalAlignment=0",
        L"Margin=-8,-35,8,0"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0.1\" TintLuminosityOpacity=\"1\" Opacity=\"1\"/>"}},
}};

const Theme g_themeSideBySide2 = {{
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#UserControl > Grid#RootPanel", {
        L"FlowDirection=1"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"MinHeight=420",
        L"MaxHeight=420"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"Margin=-40,0,40,0",
        L"Width=325"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid", {
        L"CornerRadius=8",
        L"Margin=-85,0,0,0",
        L"Width=350"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid > Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-15,0,0,0"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"Width=825"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"Height=825"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Margin=0,-70,0,0"}},
    ThemeTargetStyles{L"GridView#PinnedList", {
        L"MaxWidth=480",
        L"RenderTransform:=<TranslateTransform X=\"345\" Y=\"752\"/>",
        L"MinHeight=420",
        L"MaxHeight=420"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid", {
        L"Width=280",
        L"Margin=-55,12,55,0"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter", {
        L"RenderTransform:=<TranslateTransform X=\"-200\" Y=\"-760\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"Margin=-390,132,390,-132",
        L"FontWeight=SemiBold",
        L"Height=32",
        L"Width=200"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#ZoomedOutListView", {
        L"Margin=0,-35,0,35"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Visibility=Visible",
        L"RenderTransform:=<TranslateTransform X=\"485\" Y=\"753\"/>",
        L"FontWeight=SemiBold"}},
    ThemeTargetStyles{L"StartMenu.StartHome", {
        L"RenderTransform:=<TranslateTransform Y=\"-1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Frame > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Margin=0,-15,0,0"}},
    ThemeTargetStyles{L"DropDownButton > Grid > ContentPresenter > TextBlock", {
        L"MaxLines=2",
        L"TextLineBounds=0",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"RenderTransform:=<TranslateTransform X=\"360\" Y=\"784\"/>",
        L"Width=450",
        L"MinHeight=129",
        L"BorderThickness=0,1,0,0",
        L"BorderBrush=#22BBBBBB"}},
    ThemeTargetStyles{L"TextBlock#TopLevelSuggestionsListHeaderText", {
        L"RenderTransform:=<TranslateTransform X=\"-50\" />"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton", {
        L"RenderTransform:=<TranslateTransform X=\"50\" />"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid", {
        L"RenderTransform:=<TranslateTransform X=\"-40\" />",
        L"Margin=0,134,0,0"}},
    ThemeTargetStyles{L"ScrollViewer", {
        L"ScrollViewer.VerticalScrollMode=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"MaximumRowsOrColumns=5"}},
    ThemeTargetStyles{L"GridView#RecommendedList > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem > Border", {
        L"MaxWidth=185",
        L"HorizontalAlignment=2"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Width=630",
        L"Margin=-50,0,0,0"}},
    ThemeTargetStyles{L"GridView#RecommendedList > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem", {
        L"Margin=-25,0,-25,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView", {
        L"Margin=15,0,-15,0"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Grid", {
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Height=650",
        L"RenderTransform:=<TranslateTransform Y=\"-50\" />"}},
}};

const Theme g_themeSideBySide2_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Visibility=Visible",
        L"Width=510",
        L"MinHeight=585",
        L"Margin=264,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Grid#UndockedRoot", {
        L"Width=320"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Visibility=Visible",
        L"Width=320",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-1060\" />"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Grid#AllAppsRoot", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-833\" />",
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MinWidth=776",
        L"MaxWidth=776"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid", {
        L"Width=560"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid > Grid#RootContent", {
        L"MinWidth=560"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowAllAppsButton", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#RecommendedList > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem", {
        L"MaxWidth=220",
        L"MinWidth=220"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListView#AppsList", {
        L"Padding=48,3,-36,16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsPaneHeader", {
        L"Margin=97,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsContainer", {
        L"Height=302"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#NavigationPane", {
        L"FlowDirection=1",
        L"Margin=30,0,30,0"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView#PowerButton", {
        L"FlowDirection=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsStackPanel > Windows.UI.Xaml.Controls.ListViewItem", {
        L"FlowDirection=0"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > StartDocked.SearchBoxToggleButton#StartMenuSearchBox", {
        L"Margin=23,1,23,14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#NoSuggestionsWithoutSettingsLink", {
        L"Margin=11,0,48,0"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#MainContent > Windows.UI.Xaml.Controls.Grid#InnerContent > Windows.UI.Xaml.Shapes.Rectangle", {
        L"Margin=67,7,0,21"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.SemanticZoom#ZoomControl", {
        L"IsZoomOutButtonEnabled=true"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE73F"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton", {
        L"Width=24",
        L"Height=24",
        L"Margin=0,0,0,0",
        L"FontSize=14",
        L"CornerRadius=4",
        L"VerticalAlignment=0",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-1\" TranslateY=\"-34\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#ZoomAppsList", {
        L"Padding=86,0,25,0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList > Windows.UI.Xaml.Controls.Grid#Root", {
        L"Padding=0,0,4,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Margin=-32,0,32,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Margin=31,-3,12,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText", {
        L"Margin=31,0,63,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsContainer", {
        L"Margin=20,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#RecommendedList", {
        L"Width=490"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowMoreSuggestionsButton", {
        L"Margin=0,0,36,2",
        L"Height=24"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#HideMoreSuggestionsButton", {
        L"Margin=0,0,36,2",
        L"Height=24"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MoreSuggestionsRoot > Windows.UI.Xaml.Controls.Grid", {
        L"Margin=31,-3,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MoreSuggestionsContainer", {
        L"Margin=20,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsStackPanel > Windows.UI.Xaml.Controls.ListViewItem[MaxHeight=5000]", {
        L"MaxWidth=460",
        L"MinWidth=460",
        L"Margin=0,0,16,0",
        L"Padding=10,0,14,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowMoreSuggestionsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"Margin=0,0,2,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowMoreSuggestionsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.FontIcon", {
        L"FontSize=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#HideMoreSuggestionsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"Margin=2,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#HideMoreSuggestionsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.FontIcon", {
        L"FontSize=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter[1]", {
        L"Margin=-268,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter[1] > Grid", {
        L"Margin=-543,0,543,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter", {
        L"Margin=-245,-12,0,0"}},
}};

const Theme g_themeSideBySideMinimal = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton", {
        L"Height=0",
        L"Width=0"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Margin=-528,2,0,0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"MinHeight=504",
        L"MaxHeight=504"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"Margin=-40,0,40,0",
        L"Width=325"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid", {
        L"CornerRadius=8",
        L"Margin=-85,0,0,0",
        L"Width=350"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid > Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-15,0,0,0"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"Width=600"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"Height=710"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Margin=0,-70,0,0"}},
    ThemeTargetStyles{L"GridView#PinnedList", {
        L"Margin=16,0,-16,0",
        L"Width=300",
        L"MinHeight=504",
        L"RenderTransform:=<TranslateTransform X=\"270\" Y=\"585\"/>"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid", {
        L"Width=250"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter", {
        L"RenderTransform:=<TranslateTransform X=\"-150\" Y=\"-600\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"Margin=-174,4,174,0",
        L"FontWeight=SemiBold",
        L"Height=32",
        L"Width=250",
        L"Style:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#ZoomedOutListView", {
        L"Margin=0,-50,0,50"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Visibility=Visible",
        L"RenderTransform:=<TranslateTransform X=\"400\" Y=\"580.5\"/>",
        L"FontWeight=SemiBold"}},
    ThemeTargetStyles{L"StartMenu.StartHome", {
        L"RenderTransform:=<TranslateTransform Y=\"-1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Frame > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Margin=0,-35,0,0"}},
    ThemeTargetStyles{L"DropDownButton > Grid > ContentPresenter > TextBlock", {
        L"MaxLines=2",
        L"TextLineBounds=0",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl", {
        L"Margin=20,20,-20,-20"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Grid", {
        L"Canvas.ZIndex=1"}},
}};

const Theme g_themeSideBySideMinimal_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Visibility=Visible",
        L"Width=348",
        L"Margin=132,-42,-132,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Visibility=Visible",
        L"Width=320",
        L"Margin=-830,-42,830,0"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Grid#AllAppsRoot", {
        L"Margin=-1046,-42,1046,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Height=0",
        L"Width=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelRoot > Windows.UI.Xaml.Controls.Border", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Margin=-575,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=504"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"Margin=-40,0,40,0",
        L"Width=325"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid", {
        L"CornerRadius=8",
        L"Margin=-85,0,0,0",
        L"Width=350"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid > Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-15,0,0,0"}},
    ThemeTargetStyles{L"Rectangle[4]", {
        L"Margin=0,-20,0,0"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.AppListView", {
        L"Margin=38,0,-38,0"}},
}};

const Theme g_themeDown_Aero = {{
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"MaxHeight=520"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton", {
        L"Margin=0,-77,147,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText", {
        L"Height=0"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton > Grid > ContentPresenter > StackPanel > TextBlock", {
        L"Text=Recommended",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"CornerRadius=30"}},
    ThemeTargetStyles{L"Rectangle", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"CornerRadius=30",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"CornerRadius=30",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\"1\" Opacity=\"1\"/>",
        L"Height=430",
        L"Margin=0,-65,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Margin=0,-90,0,90"}},
    ThemeTargetStyles{L"Grid#MainContent", {
        L"Grid.Row=0",
        L"VerticalAlignment=0",
        L"MinHeight=Auto"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView > Windows.UI.Xaml.Controls.Border", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"CornerRadius=18",
        L"Margin=0,0,15,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\"1\" Opacity=\"1\"/>",
        L"BorderBrush@Normal:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".1\" Opacity=\"1\"/>",
        L"CornerRadius=30",
        L"BorderThickness=5",
        L"Margin=-7",
        L"BorderBrush@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColor}\" FallbackColor=\"{ThemeResource SystemAccentColor}\" TintOpacity=\".8\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Grid > Border#BackgroundBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"CornerRadius=18"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button[AutomationProperties.Name=Show all] > Grid@CommonStates > Border", {
        L"Background@Normal:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\".8\"/>",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\"1\" />",
        L"Padding=10,7",
        L"Margin=0,0,-5,0",
        L"CornerRadius=0,15,15,0",
        L"BorderThickness=0",
        L"Width=85"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton > Grid@CommonStates > Border", {
        L"Background@Normal:=<SolidColorBrush Color=\"{ThemeResource SystemAltMediumLowColor}\" Opacity=\"0\" />",
        L"BorderBrush@Normal:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\".8\"/>",
        L"Padding=10,5",
        L"Margin=0,0,-2,0",
        L"CornerRadius=15,0,0,15",
        L"BorderThickness=2,2,0,2",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\".7\" />",
        L"BorderBrush@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#HideMoreSuggestionsButton", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeMediumLowColor}\" Opacity=\"1\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView > Windows.UI.Xaml.Controls.Grid#RootPanel", {
        L"Grid.Row=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Frame", {
        L"Margin=0,-65,0,0"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"MaxWidth=650"}},
    ThemeTargetStyles{L"TextBlock#AllListHeadingText", {
        L"Margin=63,-184,12,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#RecommendedList", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#AllAppsGrid > Border > Windows.UI.Xaml.Controls.ScrollViewer > Border > Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"Margin=45,-180,45,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid@CommonStates", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\".8\"/>",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAltMediumLowColor}\" Opacity=\"1\" />",
        L"BorderThickness=0,2,2,2",
        L"CornerRadius=0,15,15,0",
        L"Height=32",
        L"BorderBrush@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\".7\" />"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Margin=0,20,-40,180"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"Grid.Row=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"Margin=-57,-422.5,57,422",
        L"MaxWidth=100"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid > ContentPresenter > TextBlock", {
        L"Margin=8,0,8,0",
        L"Text=View"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl", {
        L"Margin=15,0,-15,0"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button", {
        L"Margin=-430,0,430,0",
        L"Height=32",
        L"CornerRadius=15",
        L"BorderThickness=0,2,2,2"}},
    ThemeTargetStyles{L"GridView#PinnedList > Border > Windows.UI.Xaml.Controls.ScrollViewer", {
        L"ScrollViewer.VerticalScrollMode=2",
        L"Height=280"}},
    ThemeTargetStyles{L"Border#RightCompanionDropShadow", {
        L"CornerRadius=30"}},
    ThemeTargetStyles{L"Grid#CompanionRoot > Grid#MainContent > Border#AcrylicOverlay", {
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button > Grid@CommonStates > Border", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAltMediumLowColor}\" Opacity=\"1\" />",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\".8\"/>",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\".7\" />",
        L"BorderBrush@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\"1\"/>",
        L"BorderThickness=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton", {
        L"Margin=12,7,-12,-7"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Grid", {
        L"Canvas.ZIndex=1"}},
}};

const Theme g_themeDown_Aero_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MaxHeight=520"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=340"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"RenderTransform:=<TranslateTransform Y=\"-408\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText", {
        L"Height=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=Recommended"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#DropShadow", {
        L"CornerRadius=30"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#StartDropShadow", {
        L"CornerRadius=30"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#RootGridDropShadow", {
        L"CornerRadius=30"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#RightCompanionDropShadow", {
        L"CornerRadius=30"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Rectangle", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent", {
        L"Margin=0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Height=0"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > StartDocked.SearchBoxToggleButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"CornerRadius=30",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"CornerRadius=30",
        L"Margin=0,0,0,20",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Margin=0,0,0,40"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Margin=0,0,0,40"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#NavigationPane > Windows.UI.Xaml.Controls.Grid#RootPanel", {
        L"Margin=0,-10,0,10"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView > Windows.UI.Xaml.Controls.Border", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"CornerRadius=18",
        L"Margin=0,0,15,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\"1\" Opacity=\"1\"/>",
        L"BorderBrush@Normal:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".1\" Opacity=\"1\"/>",
        L"CornerRadius=30",
        L"BorderThickness=5",
        L"Margin=-7",
        L"BorderBrush@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColor}\" FallbackColor=\"{ThemeResource SystemAccentColor}\" TintOpacity=\".8\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Grid > Border#BackgroundBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"CornerRadius=18"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowAllAppsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\".8\"/>",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\"1\" />",
        L"Padding=10,7",
        L"Margin=0,0,-35,0",
        L"CornerRadius=0,15,15,0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowMoreSuggestionsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal:=<SolidColorBrush Color=\"{ThemeResource SystemAltMediumLowColor}\" Opacity=\"0\" />",
        L"BorderBrush@Normal:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\".8\"/>",
        L"Padding=10,5",
        L"Margin=0,0,19,0",
        L"CornerRadius=15,0,0,15",
        L"BorderThickness=2,2,0,2",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\".7\" />",
        L"BorderBrush@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#HideMoreSuggestionsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeMediumLowColor}\" Opacity=\"1\"/>",
        L"Padding=10,6",
        L"Margin=0,0,-35,0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeMediumLowColor}\" Opacity=\"1\"/>",
        L"Padding=10,6",
        L"Margin=0,0,-35,0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"StartDocked.StartMenuCompanion#RightCompanion > Grid#CompanionRoot > Grid#MainContent > Grid#AdaptiveCardContent", {
        L"MaxHeight=350"}},
}};

const Theme g_themeWindows10 = {{
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"Height=750",
        L"Margin=-16,0,0,-14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"BorderThickness=0",
        L"CornerRadius=0,8,0,0"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid > Border#BackgroundBorder", {
        L"CornerRadius=0",
        L"BorderThickness=0,1,1,0",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#Border@CommonStates", {
        L"CornerRadius=0",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"270\" />",
        L"Width=740",
        L"VerticalAlignment=0",
        L"Margin=-11,-549,0,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"HorizontalAlignment=2",
        L"Margin=0,0,200,0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"HorizontalAlignment=2",
        L"Margin=0,-2,35,0",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"50\" />",
        L"Height=42"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#UserTileNameText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton > Grid@CommonStates > Windows.UI.Xaml.Controls.ContentPresenter > Grid > Grid#UserTileIcon", {
        L"Margin=-3,0,-3,-62",
        L"Transform3D:=<CompositeTransform3D RotationZ=\"90\" />",
        L"Width=30",
        L"Height=30"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid > ContentPresenter", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"90\" />",
        L"Margin=0,40,0,-40"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-600\" TranslateX=\"465\" RotationZ=\"90\" />",
        L"Margin=-669,640,670,-640"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#PinnedList > Border > Windows.UI.Xaml.Controls.ScrollViewer > Border > Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > Border", {
        L"CornerRadius=4",
        L"Background:=<SolidColorBrush Color=\"#24B4B4B4\" />",
        L"Margin=2",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemBaseMediumColor}\" Opacity=\"0.2\"/>"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"MaxWidth=700",
        L"RenderTransform:=<TranslateTransform X=\"345\" Y=\"880\" />",
        L"Height=674"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Margin=0,-15,17,15",
        L"Height=700"}},
    ThemeTargetStyles{L"MenuFlyoutSeparator", {
        L"Margin=0,-2,0,-2",
        L"Padding=4"}},
    ThemeTargetStyles{L"MenuFlyoutItem", {
        L"Margin=2,0,0,2"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border > ScrollViewer", {
        L"CornerRadius=8",
        L"Padding=-3,0,-1,0"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.FolderModal", {
        L"RenderTransform:=<TranslateTransform X=\"150\" />"}},
    ThemeTargetStyles{L"StartMenu.FolderModal > Grid > Border", {
        L"Width=350",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Border#UninstallFlyoutPresenterBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentDialog", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Button#Header > Border#Border@CommonStates", {
        L"BorderThickness=1",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>"}},
    ThemeTargetStyles{L"TextBlock#Text", {
        L"FontSize=16",
        L"HorizontalAlignment=3",
        L"VerticalAlignment=2",
        L"Height=64",
        L"Padding=5,40,0,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Grid@CommonStates > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5,0,0,0",
        L"Margin=1,1,1.5,1.5",
        L"BorderThickness=1,2,1,0",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid@CommonStates", {
        L"BorderThickness=0,0,1,1",
        L"Margin=0.5,2.5,0.5,0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"#22FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=0",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid > Border#HighContrastBorder", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.64\"/>",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"#22FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid > Grid", {
        L"Width=880",
        L"Height=886",
        L"Margin=-60,-10,0,-28"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Width=750",
        L"BorderThickness=2"}},
    ThemeTargetStyles{L"Grid#QueryFormulationRoot", {
        L"Padding=-14,8,-14,0",
        L"Width=720"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"BorderBrush=#88FFFFFF",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"FlyoutPresenter", {
        L"Margin=10,20,140,0"}},
    ThemeTargetStyles{L"FlyoutPresenter > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderThickness=1",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentDialog > Border > Grid > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#MainContent", {
        L"Margin=0,-63,1,-63"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid[2]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#AllAppsGrid", {
        L"Margin=10,0,-10,0"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"Width=720"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"Margin=0,0,2,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"MaxWidth=333"}},
    ThemeTargetStyles{L"StartMenu.StartHome", {
        L"Margin=-280,1,0,0"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton", {
        L"Visibility=Visible",
        L"Margin=-653,92,653-92"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid > Windows.UI.Xaml.Controls.ContentPresenter > TextBlock", {
        L"Text=\uE179",
        L"FontFamily=Segoe Fluent Icons",
        L"FontSize=16",
        L"Margin=-8"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Grid", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView > Border > ScrollViewer", {
        L"ScrollViewer.VerticalScrollMode=2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"Style:=<StaticResource ResourceKey=\"ButtonRevealStyle\"/>",
        L"Margin=-695,172,695,-172",
        L"Width=24",
        L"Padding=0,4,0,4",
        L"Height=24"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader", {
        L"Margin=0,-900,0,0"}},
    ThemeTargetStyles{L"Grid#RootGrid > Cortana.UI.Views.RichSearchBoxControl", {
        L"MaxWidth=710"}},
    ThemeTargetStyles{L"Grid#RootGrid@SearchBoxLocationStates", {
        L"Margin@SearchBoxOnBottomWithoutQFMargin=0"}},
    ThemeTargetStyles{L"Button", {
        L"Style:=<ResourceKey=\"ButtonRevealStyle\" />"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"CornerRadius=0,6,0,0",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=2",
        L"Margin=-1,0,0,-1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#ZoomedOutListView", {
        L"Margin=142,0,-142,0"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl > Grid > Border", {
        L"Width=132",
        L"Height=132",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Button#LogoContainer > Grid@CommonStates > Border", {
        L"Width=58",
        L"Height=58",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>"}},
    ThemeTargetStyles{L"Button#FolderPlate > Grid@CommonStates > Border", {
        L"Width=58",
        L"Height=58",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl", {
        L"Margin=-12,-8,-12,-16",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Button#SeeAllButton", {
        L"MaxWidth=132",
        L"Margin=0,-4,0,4"}},
    ThemeTargetStyles{L"Button#SeeAllButton > Grid@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5",
        L"BorderThickness=1",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent TargetTheme=\"1\" Opacity=\"0\"/>"}},
    ThemeTargetStyles{L"StartMenu.StartMenuCompanion#RightCompanion > Grid > Grid", {
        L"Margin=0",
        L"CornerRadius=0,8,0,0"}},
    ThemeTargetStyles{L"Grid#CompanionRoot > Grid#MainContent > Border#AcrylicOverlay", {
        L"Margin=1,1,1,-62",
        L"BorderThickness=12,2,2,1"}},
    ThemeTargetStyles{L"StartMenu.StartMenuCompanion", {
        L"Canvas.ZIndex=0",
        L"Margin=-10,0,0,0"}},
    ThemeTargetStyles{L"Grid#RightCompanionContainerGrid", {
        L"Margin=-8,0,0,0",
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Grid > Border", {
        L"Margin=25,0,0,0"}},
    ThemeTargetStyles{L"Border#Root > Grid > ScrollContentPresenter > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Border > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Grid > Border > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > TextBlock", {
        L"Margin=36,0,0,0",
        L"Text=Recent",
        L"TextAlignment=0",
        L"Text=Recent Phone Activity"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=View your recent calls, messages, photos, and more.]", {
        L"TextAlignment=0"}},
    ThemeTargetStyles{L"Border > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Border > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Border", {
        L"Margin=40,0,0,0"}},
    ThemeTargetStyles{L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Grid > Windows.UI.Xaml.Controls.Image", {
        L"MaxWidth=52",
        L"MaxHeight=92"}},
    ThemeTargetStyles{L"Button#Header > Border > TextBlock", {
        L"Margin=-4,0,4,0"}},
    ThemeTargetStyles{L"ItemsWrapGrid > ListViewItem > Grid@CommonStates", {
        L"BorderThickness=1",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"#34FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>"}},
    ThemeTargetStyles{L"ListViewItem > Grid#ContentBorder@CommonStates", {
        L"BorderThickness=1",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"#34FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border#Root > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem > Border#ContentBorder@CommonStates > Grid > Border", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border#Root > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem > Border#ContentBorder@CommonStates > Grid > Border#HighContrastBorder", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Grid", {
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager", {
        L"RenderTransform:=<TranslateTransform X=\"-45\" />"}},
}};

const Theme g_themeWindows10_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#RootContent", {
        L"Height=800"}},
    ThemeTargetStyles{L"Rectangle[4]", {
        L"Margin=0,-20,0,0"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"Margin=-15,24,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Margin=305,-30,-305,-30"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Width=360",
        L"Visibility=Visible",
        L"Margin=-785,-18,785,0"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=2.5,1,1.5,1"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"CornerRadius=0",
        L"BorderThickness=0,1,1,0",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates > Border#BorderBackground", {
        L"CornerRadius=0",
        L"BorderThickness=1",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.42\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.42\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#Border@CommonStates", {
        L"CornerRadius=0",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates", {
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"270\" />",
        L"Width=740.5",
        L"VerticalAlignment=0",
        L"Margin=40,-558,0,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"HorizontalAlignment=2",
        L"Margin=0,0,202,0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"HorizontalAlignment=2",
        L"Margin=0,-2,37,0",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"50\" />",
        L"Height=42"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#UserTileNameText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton > Grid@CommonStates > Windows.UI.Xaml.Controls.ContentPresenter > Grid > Grid#UserTileIcon", {
        L"Margin=-3,0,-3,-62",
        L"Transform3D:=<CompositeTransform3D RotationZ=\"90\" />",
        L"Width=30",
        L"Height=30"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid > ContentPresenter", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"90\" />",
        L"Margin=0,40,0,-40"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-600\" TranslateX=\"465\" RotationZ=\"90\" />",
        L"Margin=-669,640,670,-640"}},
    ThemeTargetStyles{L"Grid#AllAppsPaneHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > Border", {
        L"FocusVisualPrimaryBrush:=<SolidColorBrush Color=\"#BBFEFEFF\" Opacity=\"1\"/>",
        L"CornerRadius=4",
        L"Background=#99646464",
        L"Height=80",
        L"Width=92",
        L"BorderBrush=#22FFFFFF"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"MaxWidth=375",
        L"Margin=-270,-28,0,0",
        L"Height=674"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Margin=0,-15,32,15",
        L"Height=692"}},
    ThemeTargetStyles{L"MenuFlyoutSeparator", {
        L"Margin=0,-2,0,-2",
        L"Padding=4"}},
    ThemeTargetStyles{L"MenuFlyoutItem", {
        L"Margin=2,0,0,2"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border > ScrollViewer", {
        L"CornerRadius=8",
        L"Padding=-3,0,-1,0"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"Margin=-145,0,145,0",
        L"Width=312"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid", {
        L"CornerRadius=8",
        L"Width=350",
        L"Margin=-295,0,0,0"}},
    ThemeTargetStyles{L"Border#UninstallFlyoutPresenterBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentDialog", {
        L"Margin=-960,214,0,0",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton", {
        L"Width=38",
        L"Height=38",
        L"Margin=0,0,317,679",
        L"Visibility=Visible",
        L"FontSize=14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.SemanticZoom#ZoomControl", {
        L"IsZoomOutButtonEnabled=True"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE75F",
        L"FontSize=27",
        L"Padding=0,7,0,0",
        L"Margin=0,1,0,8",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#MoreSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-32,0,32,0"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Button#Header > Border#Border@CommonStates", {
        L"BorderThickness=1",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"TextBlock#Text", {
        L"FontSize=16",
        L"HorizontalAlignment=3",
        L"VerticalAlignment=2",
        L"Height=64",
        L"Padding=5,40,0,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Grid@CommonStates > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5,0,0,0",
        L"Margin=1,1,2,1.5",
        L"BorderThickness=1,2,1,0",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton > Windows.UI.Xaml.Controls.ContentPresenter@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid > Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-18,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#InnerContent > Rectangle", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllAppsHeading", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid@CommonStates > Border", {
        L"BorderThickness=0,0,1,1",
        L"Margin=0.5,2,0,0"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid > Border#HighContrastBorder", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"#22FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.64\"/>",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"#22FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.AllAppsZoomListViewItem > Grid@CommonStates > Border", {
        L"BorderThickness=1",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage", {
        L"Margin=-100,17,0,-25",
        L"Width=740",
        L"Padding=0",
        L"Height=750"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#QueryFormulationRoot", {
        L"Padding=-14,0,-14,0"}},
    ThemeTargetStyles{L"Grid#BorderGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background=#88FFFFFF"}},
    ThemeTargetStyles{L"FlyoutPresenter", {
        L"Margin=10,20,140,0"}},
    ThemeTargetStyles{L"FlyoutPresenter > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderThickness=1",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentDialog > Border > Grid > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
}};

const Theme g_themeWindows10_variant_Minimal = {{
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"Height=754",
        L"Margin=0,0,0,-4",
        L"Padding=0",
        L"MaxWidth=389"}},
    ThemeTargetStyles{L"Grid#MainMenu > Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"Margin=0",
        L"BorderThickness=42,2,0,0",
        L"CornerRadius=0,12,0,0",
        L"BorderBrush:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#88242424\"/>"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid > Border#BackgroundBorder", {
        L"CornerRadius=0",
        L"BorderThickness=0,1,1,0",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates > Border#BorderBackground", {
        L"CornerRadius=0",
        L"BorderThickness=2",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.42\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.42\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#Border@CommonStates", {
        L"CornerRadius=0",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=2"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates", {
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"270\" />",
        L"Width=740",
        L"VerticalAlignment=0",
        L"Margin=-11,-551,0,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"HorizontalAlignment=2",
        L"Margin=0,0,202,0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"HorizontalAlignment=2",
        L"Margin=0,-2,36,0",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"50\" />",
        L"Height=42"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#UserTileNameText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton > Grid@CommonStates > Windows.UI.Xaml.Controls.ContentPresenter > Grid > Grid#UserTileIcon", {
        L"Margin=-3,0,-3,-62",
        L"Transform3D:=<CompositeTransform3D RotationZ=\"90\" />",
        L"Width=30",
        L"Height=30"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid > ContentPresenter", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"90\" />",
        L"Margin=0,40,0,-40"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-600\" TranslateX=\"465\" RotationZ=\"90\" />",
        L"Margin=-669,640,670,-640"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > Border", {
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Margin=0,0,34,0",
        L"Height=740"}},
    ThemeTargetStyles{L"MenuFlyoutSeparator", {
        L"Margin=0,-2,0,-2",
        L"Padding=4"}},
    ThemeTargetStyles{L"MenuFlyoutItem", {
        L"Margin=2,0,0,2"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border > ScrollViewer", {
        L"CornerRadius=8",
        L"Padding=-3,0,-1,0"}},
    ThemeTargetStyles{L"StartMenu.FolderModal", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"Margin=-102,0,102,0",
        L"MinWidth=300"}},
    ThemeTargetStyles{L"StartMenu.FolderModal > Grid > Border", {
        L"CornerRadius=8",
        L"Width=330"}},
    ThemeTargetStyles{L"Border#UninstallFlyoutPresenterBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentDialog", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Button#Header > Border#Border@CommonStates", {
        L"BorderThickness=1",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"TextBlock#Text", {
        L"FontSize=16",
        L"HorizontalAlignment=3",
        L"VerticalAlignment=2",
        L"Height=64",
        L"Padding=5,40,0,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Grid@CommonStates > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5,0,0,0",
        L"Margin=1,1,2,1.5",
        L"BorderThickness=1,2,1,0",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid@CommonStates > Border", {
        L"BorderThickness=0,0,1,1",
        L"Margin=0.5,2,0.5,0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"#22FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid > Border#HighContrastBorder", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"#22FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.64\"/>",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"#22FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.AllAppsZoomListViewItem > Grid@CommonStates > Border", {
        L"BorderThickness=1",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid > Grid", {
        L"Width=690",
        L"Height=886",
        L"Margin=-20,-10,0,-24"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#QueryFormulationRoot", {
        L"Padding=-14,0,-14,0"}},
    ThemeTargetStyles{L"Grid#BorderGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background=#88FFFFFF",
        L"MaxWidth=600"}},
    ThemeTargetStyles{L"FlyoutPresenter", {
        L"Margin=10,20,140,0"}},
    ThemeTargetStyles{L"FlyoutPresenter > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderThickness=1",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Grid#MainContent", {
        L"Margin=0,-63,1,-63"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid[2]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#AllAppsGrid", {
        L"Width=420",
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"Margin=-382,50,381,-50",
        L"Width=32",
        L"Padding=0,4,0,4",
        L"Style:=<StaticResource ResourceKey=\"ButtonRevealStyle\"/>"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton", {
        L"Margin=-567,92,567-92",
        L"Style:=<StaticResource ResourceKey=\"ButtonRevealStyle\"/>",
        L"Width=32"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid > Windows.UI.Xaml.Controls.ContentPresenter > TextBlock", {
        L"Text=\uE179",
        L"FontFamily=Segoe Fluent Icons",
        L"FontSize=16",
        L"Margin=4,0,4,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#ChevronIcon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader", {
        L"Margin=0,-85,0,0"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"CornerRadius=0",
        L"Margin=0,0,-240,0",
        L"Width=630",
        L"Padding=0,0,-1,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"MaxWidth=315",
        L"Margin=14,0,0,0",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"Border#TaskbarMargin", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#RootGrid@SearchBoxLocationStates", {
        L"HorizontalAlignment=Left",
        L"Margin@SearchBoxOnBottomWithoutQFMargin=0"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock[Text=All]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.StartMenuCompanion#RightCompanion > Grid > Grid", {
        L"Margin=0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"StartMenu.StartMenuCompanion", {
        L"Margin=-10,-1,0,0"}},
    ThemeTargetStyles{L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Grid > Border", {
        L"RenderTransform:=<TranslateTransform X=\"25\"/>"}},
    ThemeTargetStyles{L"Border#Root > Grid > ScrollContentPresenter > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Border > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Grid > Border > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > TextBlock", {
        L"Margin=36,0,0,0",
        L"Text=Recent",
        L"TextAlignment=0",
        L"Text=Recent Phone Activity"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=View your recent calls, messages, photos, and more.]", {
        L"TextAlignment=0"}},
    ThemeTargetStyles{L"Button > Grid#RootGrid > Windows.UI.Xaml.Controls.ContentPresenter > Border > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Border > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Border", {
        L"Margin=40,0,0,0"}},
    ThemeTargetStyles{L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Grid > Windows.UI.Xaml.Controls.Image", {
        L"MaxWidth=52",
        L"MaxHeight=92"}},
    ThemeTargetStyles{L"Button#Header > Border > TextBlock", {
        L"Margin=-4,0,4,0"}},
    ThemeTargetStyles{L"ItemsWrapGrid > ListViewItem > Grid@CommonStates", {
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"#34FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"ListViewItem > Grid#ContentBorder@CommonStates", {
        L"BorderThickness=1",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"#34FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Button", {
        L"Style:=<ResourceKey=\"ButtonRevealStyle\" />"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=2,2,3,2",
        L"Margin=0,0,240,0"}},
    ThemeTargetStyles{L"Grid#CompanionRoot > Grid#MainContent > Border#AcrylicOverlay", {
        L"Margin=-1,2,1,-63",
        L"BorderThickness=12,2,2,0",
        L"CornerRadius=0,8,0,0"}},
    ThemeTargetStyles{L"Grid#CompanionRoot > Border#AcrylicBorder", {
        L"CornerRadius=0,12,0,0"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl > Grid > Border", {
        L"Height=132",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Button#LogoContainer > Grid@CommonStates > Border", {
        L"Width=55",
        L"Height=55",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Button#FolderPlate > Grid@CommonStates > Border", {
        L"Width=55",
        L"Height=55",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl", {
        L"Margin=-20,-8,-20,-16",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"RenderTransform:=<TranslateTransform X=\"16\"/>"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Grid", {
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"Button#SeeAllButton > Grid@CommonStates", {
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"Visibility=Collapsed"}},
}};

const Theme g_themeWindows10_variant_Minimal_ClassicStartMenu = {{
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#RootContent", {
        L"Height=800"}},
    ThemeTargetStyles{L"Rectangle[4]", {
        L"Margin=0,-20,0,0"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"Margin=-15,24,450,0",
        L"MinWidth=400",
        L"Width=400"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Width=425",
        L"Visibility=Visible",
        L"Margin=-750,-18,750,0"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=2.5,1,1.5,1",
        L"MaxWidth=400",
        L"Margin=-121,0,121,0"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"CornerRadius=0",
        L"BorderThickness=0,1,1,0",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates > Border#BorderBackground", {
        L"CornerRadius=0",
        L"BorderThickness=1",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.42\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.42\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#Border@CommonStates", {
        L"CornerRadius=0",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates", {
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"270\" />",
        L"Width=740",
        L"VerticalAlignment=0",
        L"Margin=40,-557,0,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"HorizontalAlignment=2",
        L"Margin=0,0,202,0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"HorizontalAlignment=2",
        L"Margin=0,-2,37,0",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"50\" />",
        L"Height=42"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#UserTileNameText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton > Grid@CommonStates > Windows.UI.Xaml.Controls.ContentPresenter > Grid > Grid#UserTileIcon", {
        L"Margin=-3,0,-3,-62",
        L"Transform3D:=<CompositeTransform3D RotationZ=\"90\" />",
        L"Width=30",
        L"Height=30"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid > ContentPresenter", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"90\" />",
        L"Margin=0,40,0,-40"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-600\" TranslateX=\"465\" RotationZ=\"90\" />",
        L"Margin=-669,640,670,-640"}},
    ThemeTargetStyles{L"Grid#AllAppsPaneHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView", {
        L"ItemsSource:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Margin=0,-15,32,15",
        L"Height=692"}},
    ThemeTargetStyles{L"MenuFlyoutSeparator", {
        L"Margin=0,-2,0,-2",
        L"Padding=4"}},
    ThemeTargetStyles{L"MenuFlyoutItem", {
        L"Margin=2,0,0,2"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border > ScrollViewer", {
        L"CornerRadius=8",
        L"Padding=-3,0,-1,0"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"Margin=-145,0,145,0",
        L"Width=312"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid", {
        L"CornerRadius=8",
        L"Width=350",
        L"Margin=-295,0,0,0"}},
    ThemeTargetStyles{L"Border#UninstallFlyoutPresenterBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentDialog", {
        L"Margin=-960,214,0,0",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton", {
        L"Width=38",
        L"Height=38",
        L"Margin=0,0,383,679",
        L"Visibility=Visible",
        L"FontSize=14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.SemanticZoom#ZoomControl", {
        L"IsZoomOutButtonEnabled=True"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE75F",
        L"FontSize=27",
        L"Padding=0,7,0,0",
        L"Margin=0,1,0,8",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#MoreSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Button#Header > Border#Border@CommonStates", {
        L"BorderThickness=1",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"TextBlock#Text", {
        L"FontSize=16",
        L"HorizontalAlignment=3",
        L"VerticalAlignment=2",
        L"Height=64",
        L"Padding=5,40,0,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Grid@CommonStates > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5,0,0,0",
        L"Margin=1,1,2,1.5",
        L"BorderThickness=1,2,1,0",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton > Windows.UI.Xaml.Controls.ContentPresenter@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid > Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-18,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#InnerContent > Rectangle", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllAppsHeading", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid@CommonStates > Border", {
        L"BorderThickness=0,0,1,1",
        L"Margin=0.5,2,0,0"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid > Border#HighContrastBorder", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"#22FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.64\"/>",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"#22FFFFFF\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.AllAppsZoomListViewItem > Grid@CommonStates > Border", {
        L"BorderThickness=1",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage", {
        L"Margin=-100,17,0,-25",
        L"Width=740",
        L"Padding=0",
        L"Height=750"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#QueryFormulationRoot", {
        L"Padding=-14,0,-14,0"}},
    ThemeTargetStyles{L"Grid#BorderGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background=#88FFFFFF"}},
    ThemeTargetStyles{L"FlyoutPresenter", {
        L"Margin=10,20,140,0"}},
    ThemeTargetStyles{L"FlyoutPresenter > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderThickness=1",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentDialog > Border > Grid > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFramePanel > Border#DropShadow", {
        L"MaxWidth=150"}},
}};

const Theme g_themeWindows11_Metro10 = {{
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid", {
        L"Visibility=Visible",
        L"Width=300",
        L"Margin=-150,-600,150,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"MaxWidth=650"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid[2]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#RecommendedList > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem", {
        L"MaxWidth=145",
        L"MinWidth=145",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsPaneHeader", {
        L"Margin=97,-10,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SuggestionsParentContainer", {
        L"Height=168"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#NavigationPane", {
        L"FlowDirection=0",
        L"Margin=30,0,30,0"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView#PowerButton", {
        L"FlowDirection=0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"FlowDirection=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewItem", {
        L"FlowDirection=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Frame", {
        L"Margin=0,-65,0,0"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#StartMenuSearchBox", {
        L"Margin=23,-101,23,14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#NoSuggestionsWithoutSettingsLink", {
        L"Margin=11,0,48,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#ZoomAppsList", {
        L"Padding=86,0,27,0"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton", {
        L"Height=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Margin=-30,-2,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Margin=35,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Border#ContentBorder@CommonStates > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border", {
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"White\" TargetTheme=\"1\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"Margin=1",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.1\"/>"}},
    ThemeTargetStyles{L"GridView#PinnedList > Border > ScrollViewer > Border > Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Border#ContentBorder@CommonStates > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border", {
        L"Background:=<RevealBorderBrush Color=\"#646464\" TargetTheme=\"1\" Opacity=\".1\"/>",
        L"Margin=2",
        L"CornerRadius=5",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Visible",
        L"Margin=40,-10,0,12"}},
    ThemeTargetStyles{L"GridView#RecommendedList", {
        L"Margin=290,-58,-290,58",
        L"Width=290"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid", {
        L"Margin=0,0,80,0"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"Width=350",
        L"Margin=0,0,92,0"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid > Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-20,0,20,0"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Margin=0,-70,0,0"}},
    ThemeTargetStyles{L"GridView#PinnedList", {
        L"Margin=165,0,-165,0",
        L"Width=300"}},
    ThemeTargetStyles{L"Grid#AllListHeading", {
        L"RenderTransform:=<TranslateTransform X=\"-334\" Y=\"-604\"/>"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"RenderTransform:=<TranslateTransform X=\"252\" Y=\"-58\" />"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > TextBlock", {
        L"RenderTransform:=<TranslateTransform X=\"305\" Y=\"6\" />"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"MaxHeight=670"}},
    ThemeTargetStyles{L"TextBlock#AllListHeadingText", {
        L"Margin=65,1,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"MaximumRowsOrColumns=3",
        L"Grid.Row=1"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"MaxHeight=420",
        L"MinHeight=420",
        L"Height=420",
        L"Margin=0,-32,0,32"}},
    ThemeTargetStyles{L"GridView#PinnedList > Border > Windows.UI.Xaml.Controls.ScrollViewer", {
        L"ScrollViewer.VerticalScrollMode=2",
        L"Height=336"}},
    ThemeTargetStyles{L"GridView#RecommendedList > Border > Windows.UI.Xaml.Controls.ScrollViewer", {
        L"ScrollViewer.VerticalScrollMode=2",
        L"Height=120"}},
    ThemeTargetStyles{L"Button#Header > Border#Border@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewItem > Grid#ContentBorder@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border#HighContrastBorder", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl > Grid > Border", {
        L"Height=132",
        L"Width=132"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl", {
        L"Margin=-22,-16,-22,0",
        L"Width=165"}},
    ThemeTargetStyles{L"Button#SeeAllButton", {
        L"Width=132",
        L"Margin=0,-4,0,4"}},
    ThemeTargetStyles{L"Button#SeeAllButton > Grid@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"CornerRadius=5",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Button#LogoContainer > Grid@CommonStates > Border", {
        L"Width=57",
        L"Height=57",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Button#LogoContainer", {
        L"Margin=5,-1,-5,0"}},
    ThemeTargetStyles{L"Button#FolderPlate > Grid@CommonStates > Border", {
        L"Width=57",
        L"Height=57",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Button#FolderPlate", {
        L"Margin=4,-1,-4,0"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Grid", {
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"MinHeight=129"}},
}};

const Theme g_themeWindows11_Metro10_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Visibility=Visible",
        L"MaxWidth=600",
        L"Margin=290,-10,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Visibility=Visible",
        L"Width=360",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-1059\" />",
        L"Margin=180,0,-220,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MinWidth=650",
        L"MaxWidth=650",
        L"MaxHeight=670"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#RecommendedList > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer#ScrollViewer > Windows.UI.Xaml.Controls.Border#Root > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem", {
        L"MaxWidth=145",
        L"MinWidth=145",
        L"Margin=0"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListView#AppsList", {
        L"Padding=90,3,6,16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsPaneHeader", {
        L"Margin=97,-10,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SuggestionsParentContainer", {
        L"Height=168"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#NavigationPane", {
        L"FlowDirection=0",
        L"Margin=30,0,30,0"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView#PowerButton", {
        L"FlowDirection=0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"FlowDirection=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewItem", {
        L"FlowDirection=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsStackPanel > Windows.UI.Xaml.Controls.ListViewItem", {
        L"FlowDirection=0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox", {
        L"Margin=23,-101,23,14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#NoSuggestionsWithoutSettingsLink", {
        L"Margin=11,0,48,0"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Grid#MainContent > Windows.UI.Xaml.Controls.Grid#InnerContent > Windows.UI.Xaml.Shapes.Rectangle", {
        L"Margin=67,7,0,21"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.SemanticZoom#ZoomControl", {
        L"IsZoomOutButtonEnabled=true",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE73F"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton", {
        L"Width=28",
        L"Height=28",
        L"Margin=-1,-36,0,0",
        L"FontSize=14",
        L"CornerRadius=4",
        L"VerticalAlignment=0",
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#ZoomAppsList", {
        L"Padding=86,0,27,0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Height=0",
        L"Margin=0,-100,0,24"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"MaxHeight=400"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Margin=-30,-2,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SuggestionsParentContainer", {
        L"Margin=-20,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Margin=35,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border@CommonStates", {
        L"BorderBrush@Active:=<RevealBorderBrush Color=\"White\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"Margin=1",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"Background:=<RevealBorderBrush Color=\"#646464\" TargetTheme=\"1\" Opacity=\".1\"/>",
        L"Margin=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Rectangle[4]", {
        L"Margin=0,-20,0,0"}},
    ThemeTargetStyles{L"GridView#RecommendedList", {
        L"Margin=-20,0,20,0"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid", {
        L"Margin=0,0,80,0"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"Width=350",
        L"Margin=0,0,92,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-10,0,0,0"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid > Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-20,0,20,0"}},
}};

const Theme g_themeFluent2Inspired = {{
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton", {
        L"Margin=5,2,-5,-2"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0.45\" TintLuminosityOpacity=\".96\" Opacity=\"1\"/>",
        L"CornerRadius=12",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".25\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#MainContent", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"MaxWidth=650",
        L"Margin=-14,14,4,0"}},
    ThemeTargetStyles{L"TextBlock#DisplayName", {
        L"Margin=0,8,0,-8",
        L"FontSize=13",
        L"FontFamily=Aptos",
        L"Opacity=.75",
        L"FontWeight=500",
        L"Padding=14,0,14,0"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"FontFamily=Aptos",
        L"Opacity=.85",
        L"FontSize=16",
        L"Margin=40,0,-40,0"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0.25\" TintLuminosityOpacity=\".96\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".25\" Opacity=\"1\"/>",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"CornerRadius=12",
        L"Margin=-1"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl", {
        L"Margin=33,33,33,10"}},
    ThemeTargetStyles{L"TextBlock#UserTileNameText", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TextBlock#AllListHeadingText", {
        L"FontFamily=Aptos",
        L"Margin=50,5,0,0",
        L"FontSize=16",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"Border#ContentBorder", {
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"Text=Where to next?",
        L"FontWeight=700",
        L"FontFamily=Aptos",
        L"FontSize=24",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\" Opacity=\".85\"/>",
        L"Margin=2,0,0,0"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > Border", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > FontIcon", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"165\" TranslateY=\"-1\"/>",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\" FallbackColor=\"{ThemeResource FocusStrokeColorOuter}\" Opacity=\".85\"/>",
        L"FontSize=24"}},
    ThemeTargetStyles{L"Grid#TopLevelRoot", {
        L"Margin=0,-8,0,0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"RenderTransform:=<TranslateTransform X=\"512\" Y=\"-685\" />"}},
    ThemeTargetStyles{L"StartDocked.UserTileView > StartDocked.NavigationPaneButton > Grid > Border", {
        L"CornerRadius=99",
        L"Margin=7,0,8,1"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"RenderTransform:=<TranslateTransform X=\"-14\" Y=\"-685\" />",
        L"CornerRadius=99",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TextBlock#AppDisplayName", {
        L"FontFamily=Aptos",
        L"Opacity=.85",
        L"Margin=4,0,0,4",
        L"FontWeight=500"}},
    ThemeTargetStyles{L"Button#Header > Border > TextBlock", {
        L"FontFamily=Aptos",
        L"FontWeight=600",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView > StartDocked.NavigationPaneButton > Grid > Border", {
        L"CornerRadius=99",
        L"Margin=1"}},
    ThemeTargetStyles{L"ListViewItem", {
        L"Margin=1,5,-5,-5",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Button#Header", {
        L"Margin=4,0,-3,-5"}},
    ThemeTargetStyles{L"TextBlock#PlaceholderTextContentPresenter", {
        L"FontFamily=Aptos",
        L"FontSize=24",
        L"FontWeight=700",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\" Opacity=\".7\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#SearchIconPlayer", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Button#SearchGlyphContainer", {
        L"FontSize=32",
        L"Visibility=1"}},
    ThemeTargetStyles{L"Cortana.UI.Views.CortanaRichSearchBox#SearchTextBox", {
        L"FontSize=24",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" Opacity=\".85\"/>",
        L"FontFamily=Aptos",
        L"Opacity=.85",
        L"FontWeight=ExtraBold"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon#SearchBoxOnTaskbarSearchGlyph", {
        L"Visibility=0",
        L"Margin=0",
        L"FontSize=32",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl", {
        L"Margin=31,31,17,17"}},
    ThemeTargetStyles{L"Grid#WebViewGrid", {
        L"Margin=-13,0,-10,15"}},
    ThemeTargetStyles{L"TextBlock#StatusMessage", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"CornerRadius=12",
        L"Margin=-1"}},
    ThemeTargetStyles{L"Border#StartDropShadowDismissTarget", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter[1]", {
        L"Margin=-250,50,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter", {
        L"Margin=-250,0,0,0"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > FontIcon#SearchGlyph", {
        L"Margin=0,-3,0,0",
        L"FontSize=25",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\" Opacity=\".85\"/>"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid#Root > Border", {
        L"Height=420"}},
    ThemeTargetStyles{L"TextBox#ExpandedFolderNameTextBox", {
        L"Margin=-15,-15,15,20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#FolderList > Border", {
        L"Margin=0,0,0,-60"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#NavigationPane > Grid > StartDocked.AppListView", {
        L"Margin=0,0,-36,0"}},
    ThemeTargetStyles{L"Image#SearchIconOn", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Image#SearchIconOff", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#ContentBorder > Border#BackgroundBorder", {
        L"CornerRadius=99",
        L"Height=38",
        L"Width=38"}},
    ThemeTargetStyles{L"Grid#ContentBorder > ContentPresenter > FontIcon", {
        L"Opacity=.85"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"Padding=2,0,6,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView > Border > ScrollViewer > Border#Root > Grid > ScrollContentPresenter > ItemsPresenter > ItemsStackPanel > ListViewItem", {
        L"Margin=-2,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"Margin=0,0,-46,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-10\" TranslateY=\"3\"/>",
        L"Height=615"}},
    ThemeTargetStyles{L"Grid#ContentBorder > Border#BorderBackground", {
        L"Margin=1,0,-1,0"}},
    ThemeTargetStyles{L"StackPanel#RootPanel > Button#Header > Border#Border", {
        L"Margin=0,0,-1,0"}},
    ThemeTargetStyles{L"Rectangle#TextCaret", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#RootGrid@SearchBoxLocationStates", {
        L"Margin@SearchBoxOnBottomWithoutQFMargin=-32,10,32,-10"}},
    ThemeTargetStyles{L"Grid#RootGrid@SearchBoxLocationStates > Cortana.UI.Views.CortanaRichSearchBox > Grid > TextBlock#PlaceholderTextContentPresenter", {
        L"FontSize=16"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"Width=630"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"MaxHeight=775"}},
    ThemeTargetStyles{L"ListView#ZoomedOutListView", {
        L"Margin=0,-150,0,0"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border#Root > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid", {
        L"RenderTransform:=<TranslateTransform X=\"-8\" />",
        L"Margin=0",
        L"Width=540"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl", {
        L"Margin=0,0,-8,-8",
        L"RenderTransform:=<TranslateTransform X=\"14\" />"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid > ContentPresenter > TextBlock", {
        L"FontFamily=Aptos",
        L"FontSize=16"}},
    ThemeTargetStyles{L"TextBlock#ShowMorePinnedButtonText", {
        L"FontFamily=Aptos",
        L"FontSize=16"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton", {
        L"Margin=-80,0,80,0"}},
}};

const Theme g_themeFluent2Inspired_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Button#CloseAllAppsButton", {
        L"CornerRadius=14",
        L"Margin=0,0,-32,0",
        L"Padding=10,4,12,5"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton", {
        L"CornerRadius=14",
        L"Margin=0,0,32,0",
        L"Padding=12,4,10,5"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Margin=30,0,120,26"}},
    ThemeTargetStyles{L"PipsPager#PinnedListPipsPager", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0.45\" TintLuminosityOpacity=\".96\" Opacity=\"1\"/>",
        L"CornerRadius=12",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".25\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#MainContent", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"MaxWidth=650",
        L"Height=504",
        L"Margin=-8,14,8,-14"}},
    ThemeTargetStyles{L"TextBlock#DisplayName", {
        L"Margin=0,8,0,-8",
        L"FontSize=13",
        L"FontFamily=Aptos",
        L"Opacity=.75",
        L"FontWeight=500",
        L"Padding=14,0,14,0"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Margin=-14,0,0,0",
        L"FontFamily=Aptos",
        L"Opacity=.85",
        L"FontSize=16",
        L"Margin=-32,0,0,0"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0.25\" TintLuminosityOpacity=\".96\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".25\" Opacity=\"1\"/>",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"CornerRadius=12",
        L"Margin=-1"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl", {
        L"Margin=33,33,33,10"}},
    ThemeTargetStyles{L"TextBlock#UserTileNameText", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TextBlock#AllAppsHeading", {
        L"FontFamily=Aptos",
        L"Margin=-32,0,0,0",
        L"FontSize=16",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"Border#ContentBorder", {
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"Text=Where to next?",
        L"FontWeight=700",
        L"FontFamily=Aptos",
        L"FontSize=24",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\" Opacity=\".85\"/>",
        L"Margin=2,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > Border", {
        L"Background=transparent",
        L"BorderBrush=transparent"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > FontIcon", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"165\" TranslateY=\"-1\"/>",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\" FallbackColor=\"{ThemeResource FocusStrokeColorOuter}\" Opacity=\".85\"/>",
        L"FontSize=24"}},
    ThemeTargetStyles{L"Grid#TopLevelRoot", {
        L"Margin=0,-8,0,0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Margin=512,-1290,-2000,0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView > StartDocked.NavigationPaneButton > Grid > Border", {
        L"CornerRadius=99",
        L"Margin=8,0,8,0"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Margin=-64,-1290,-2000,0",
        L"CornerRadius=99",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"TextBlock#ShowAllAppsButtonText", {
        L"FontFamily=Aptos",
        L"Opacity=.85",
        L"FontWeight=500"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton > ContentPresenter > StackPanel > TextBlock", {
        L"FontFamily=Aptos",
        L"Opacity=.85",
        L"FontWeight=500"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#AllAppsPaneHeader", {
        L"Margin=64,-8,64,5"}},
    ThemeTargetStyles{L"Grid#InnerContent", {
        L"Margin=0,31,0,-64"}},
    ThemeTargetStyles{L"TextBlock#AppDisplayName", {
        L"FontFamily=Aptos",
        L"Opacity=.85",
        L"Margin=4,0,0,4",
        L"FontWeight=500"}},
    ThemeTargetStyles{L"Button#Header > Border > TextBlock", {
        L"FontFamily=Aptos",
        L"FontWeight=600",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView > StartDocked.NavigationPaneButton > Grid > Border", {
        L"CornerRadius=99",
        L"Margin=1"}},
    ThemeTargetStyles{L"TileGrid", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource TextFillColorInverse}\" Opacity=\".2\"/>",
        L"CornerRadiusProtected=8",
        L"BorderThicknessProtected=1",
        L"BorderBrushProtected:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\".35\"/>"}},
    ThemeTargetStyles{L"ListViewItem", {
        L"Margin=1,0,-6,0",
        L"CornerRadius=4",
        L"Padding=0,0,6,0"}},
    ThemeTargetStyles{L"Button#Header", {
        L"Margin=4,0,-3,0"}},
    ThemeTargetStyles{L"StartDocked.AllAppsPane#AllAppsPanel", {
        L"Margin=-20,0,-6,0"}},
    ThemeTargetStyles{L"TextBlock#PlaceholderTextContentPresenter", {
        L"FontFamily=Aptos",
        L"FontSize=24",
        L"FontWeight=700",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\" Opacity=\".7\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#SearchIconPlayer", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Button#SearchGlyphContainer", {
        L"FontSize=32",
        L"Visibility=1"}},
    ThemeTargetStyles{L"Cortana.UI.Views.CortanaRichSearchBox#SearchTextBox", {
        L"FontSize=24",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" Opacity=\".85\"/>",
        L"FontFamily=Aptos",
        L"Opacity=.85",
        L"FontWeight=ExtraBold"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon#SearchBoxOnTaskbarSearchGlyph", {
        L"Visibility=0",
        L"Margin=0",
        L"FontSize=32",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl", {
        L"Margin=31,31,17,17"}},
    ThemeTargetStyles{L"Grid#WebViewGrid", {
        L"Margin=-13,0,-10,15"}},
    ThemeTargetStyles{L"TextBlock#StatusMessage", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#LogoBackgroundPlate", {
        L"Margin=12,0,0,0"}},
    ThemeTargetStyles{L"Border#DropShadow", {
        L"CornerRadius=12",
        L"Margin=-1"}},
    ThemeTargetStyles{L"Border#DropShadowDismissTarget", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter[1]", {
        L"Margin=-250,50,0,0"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Rectangle", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter", {
        L"Margin=-250,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > FontIcon#SearchGlyph", {
        L"Margin=0,-3,0,0",
        L"FontSize=25",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\" Opacity=\".85\"/>"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid#Root > Border", {
        L"Height=420"}},
    ThemeTargetStyles{L"TextBox#ExpandedFolderNameTextBox", {
        L"Margin=-15,-15,15,20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#FolderList > Border", {
        L"Margin=0,0,0,-60"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#NavigationPane > Grid > StartDocked.AppListView", {
        L"Margin=0,0,-36,0"}},
    ThemeTargetStyles{L"Image#SearchIconOn", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Image#SearchIconOff", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#ContentBorder > Border#BackgroundBorder", {
        L"CornerRadius=99",
        L"Height=38",
        L"Width=38"}},
    ThemeTargetStyles{L"Grid#ContentBorder > ContentPresenter > FontIcon", {
        L"Margin=6,0,0,0",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"Padding=2,0,6,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView > Border > ScrollViewer > Border#Root > Grid > ScrollContentPresenter > ItemsPresenter > ItemsStackPanel > ListViewItem", {
        L"Margin=-2,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"Margin=0,0,-46,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridViewItem", {
        L"Height=84"}},
}};

const Theme g_themeRosePine = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton", {
        L"Background=#1f1d2e",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=340",
        L"Width=342"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#Margin", {
        L"Margin=210,0,210,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"BorderThickness=1.5",
        L"CornerRadius=25",
        L"BorderBrush=#ebbcba",
        L"Background=#191724"}},
    ThemeTargetStyles{L"StartMenu.StartBlendedFlexFrame", {
        L"CornerRadius=25"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Foreground=#eb6f92"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AppDisplayName", {
        L"Foreground=#e0def4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#DisplayName", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UserTileIcon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicOverlay", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"StartMenu.PinnedListTile > Windows.UI.Xaml.Controls.Grid#Root", {
        L"Padding=0,25,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"BorderBrush=#191724",
        L"BorderThickness=5",
        L"Background=#1f1d2e",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Margin=-260,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background=#1f1d2e",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=\uE7E8]", {
        L"Text=\uE72E  \uE708  \uE7E8  \uE777"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton", {
        L"Width=120"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PlaceholderText", {
        L"Text=Search",
        L"Foreground=#524f67",
        L"FontFamily=JetBrainsMono NF"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=\uF78B]", {
        L"Foreground=#c4a7e7"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ContentBorder > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background=#1f1d2e",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=\uE713]", {
        L"Foreground=#c4a7e7"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"Margin=0,0,-38,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AppBorder", {
        L"Background=#191724",
        L"BorderThickness=1.5",
        L"BorderBrush=#ebbcba",
        L"CornerRadius=25"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#OuterBorderGrid", {
        L"CornerRadius=25"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#TaskbarSearchBackground", {
        L"BorderThickness=1.5",
        L"BorderBrush=#ebbcba"}},
    ThemeTargetStyles{L"StartMenu:ExpandedFolderList", {
        L"Margin=-50,0,-50,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#AllAppsGrid > Border > Windows.UI.Xaml.Controls.ScrollViewer > Border > Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"MaxHeight=550"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"Width=515"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#AllListHeading", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"ScrollViewer", {
        L"ScrollViewer.VerticalScrollMode=2"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid[2]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#RightCompanionDropShadow", {
        L"CornerRadius=25"}},
}};

const Theme g_themeRosePine_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Width=350",
        L"Margin=0,-40,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Width=320",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-800\" />",
        L"Margin=-30,-60,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Margin=114,53,114,0",
        L"Background=#1f1d2e",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelRoot > Windows.UI.Xaml.Controls.Border", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=340"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#Margin", {
        L"Margin=210,0,210,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"BorderThickness=1.5",
        L"CornerRadius=25",
        L"BorderBrush=#ebbcba",
        L"Background=#191724"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFramePanel", {
        L"CornerRadius=25"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Foreground=#eb6f92"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AppDisplayName", {
        L"Foreground=#e0def4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#DisplayName", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllAppsHeading", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MaxHeight=580"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UserTileIcon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicOverlay", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"StartMenu.PinnedListTile > Windows.UI.Xaml.Controls.Grid#Root", {
        L"Padding=0,25,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"BorderBrush=#191724",
        L"BorderThickness=5",
        L"Background=#1f1d2e",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Margin=-260,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background=#1f1d2e",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=\uE7E8]", {
        L"Text=\uE72E  \uE708  \uE7E8  \uE777"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton", {
        L"Width=120"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#InnerContent > Windows.UI.Xaml.Shapes.Rectangle", {
        L"Margin=150,53,134,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PlaceholderText", {
        L"Text=Search",
        L"Foreground=#524f67",
        L"FontFamily=JetBrainsMono NF"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=\uF78B]", {
        L"Foreground=#c4a7e7"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ContentBorder > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background=#1f1d2e",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=\uE713]", {
        L"Foreground=#c4a7e7"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"Margin=0,0,-38,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AppBorder", {
        L"Background=#191724",
        L"BorderThickness=1.5",
        L"BorderBrush=#ebbcba",
        L"CornerRadius=25"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#OuterBorderGrid", {
        L"CornerRadius=25"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#TaskbarSearchBackground", {
        L"BorderThickness=1.5",
        L"BorderBrush=#ebbcba"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootGrid > Grid#RootContent", {
        L"MaxWidth=500",
        L"Width=500",
        L"MinWidth=500"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFramePanel", {
        L"MaxWidth=500",
        L"Width=500",
        L"MinWidth=500"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootGrid > Grid#RootContent", {
        L"MaxWidth=500",
        L"Width=500",
        L"MinWidth=500"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MinWidth=500",
        L"Width=500",
        L"MaxWidth=500"}},
    ThemeTargetStyles{L"StartMenu:ExpandedFolderList", {
        L"Margin=-50,0,-50,0"}},
}};

const Theme g_themeWindows11_Metro10Minimal = {{
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"Visibility=Visible",
        L"Width=420",
        L"Background=Transparent",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"MaxHeight=670"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid", {
        L"Visibility=Visible",
        L"Margin=0,-32,0,1"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView", {
        L"Margin=-30,0,-30,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"FlowDirection=1"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid > Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-20,0,20,0"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".25\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".85\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".25\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Frame", {
        L"Margin=0,-65,0,0"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Margin=0,-70,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"Margin=12,0,12,0"}},
    ThemeTargetStyles{L"StartMenu.FolderModal > Grid > Border", {
        L"Width=350",
        L"Margin=-20,0,20,0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#LevelOneGridView", {
        L"Margin=16,0,-16,0"}},
    ThemeTargetStyles{L"GridView#RecommendedList", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot > Grid[2]", {
        L"MinHeight=0"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid[2] > Button", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock[Text=All]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"Grid.Column=0",
        L"RenderTransform:=<TranslateTransform X=\"12\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"MaxHeight=570",
        L"Margin=0,15,0,-15"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl", {
        L"Margin=-15,0-15,0",
        L"RenderTransform:=<TranslateTransform X=\"24\" />"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Grid", {
        L"Canvas.ZIndex=1"}},
}};

const Theme g_themeWindows11_Metro10Minimal_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"MaxWidth=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Visibility=Visible",
        L"Width=540",
        L"Margin=-1000,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MinWidth=460",
        L"MaxWidth=460",
        L"MaxHeight=670"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListView#AppsList", {
        L"Padding=90,3,6,16"}},
    ThemeTargetStyles{L"Grid#AllAppsPaneHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#NavigationPane", {
        L"Margin=30,0,30,0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"FlowDirection=1"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox", {
        L"Margin=23,-101,23,14"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Height=0"}},
    ThemeTargetStyles{L"Rectangle[4]", {
        L"Margin=0,-20,0,0"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid > Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-20,0,20,0"}},
    ThemeTargetStyles{L"StartMenu.StartInnerFrame", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#RootContent", {
        L"MinWidth=460"}},
    ThemeTargetStyles{L"Grid#InnerContent", {
        L"Margin=0,12,0,0"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".85\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".25\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".85\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".25\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=1"}},
}};

const Theme g_themeEverblush = {{
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background=#141b1e",
        L"BorderBrush=#268ccf7e"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Background=#141b1e"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > Border", {
        L"Background=#232a2d",
        L"BorderBrush=transparent"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"Background=#232a2d"}},
    ThemeTargetStyles{L"TextBlock#PlaceholderText", {
        L"Foreground=#80b3b9b8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button > Grid@CommonStates", {
        L"Background=#d28ccf7e",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"StackPanel > Windows.UI.Xaml.Controls.Button", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ItemsRepeater > Windows.UI.Xaml.Controls.Button", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"TextBlock#DisplayName", {
        L"Foreground=#b3b9b8"}},
    ThemeTargetStyles{L"TextBlock#Title", {
        L"Foreground=#b3b9b8"}},
    ThemeTargetStyles{L"TextBlock#Subtitle", {
        L"Foreground=#6cbfbf"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Foreground=#8ccf7e"}},
    ThemeTargetStyles{L"TextBlock#TopLevelSuggestionsListHeaderText", {
        L"Foreground=#8ccf7e"}},
    ThemeTargetStyles{L"TextBlock#AllAppsHeading", {
        L"Foreground=#8ccf7e"}},
    ThemeTargetStyles{L"TextBlock#MoreSuggestionsListHeaderText", {
        L"Foreground=#8ccf7e"}},
    ThemeTargetStyles{L"TextBlock#AppDisplayName", {
        L"Foreground=#b3b9b8"}},
    ThemeTargetStyles{L"TextBlock#Text", {
        L"Foreground=#e5c76b"}},
    ThemeTargetStyles{L"TextBlock#FolderGlyph", {
        L"Foreground=#e5c76b"}},
    ThemeTargetStyles{L"TextBlock#StatusMessage", {
        L"Foreground=#8ccf7e"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border@CommonStates", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0,0.5\"> <GradientStop Color=\"#268ccf7e\" Offset=\"0\" /><GradientStop Color=\"#26e5c76b\" Offset=\"1\" /></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0,0.5\"> <GradientStop Color=\"#828ccf7e\" Offset=\"0\" /><GradientStop Color=\"#82e5c76b\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background=#141b1e"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background=#232a2d",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"TextBlock#UserTileNameText", {
        L"Foreground=#67b0e8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Foreground=#6cbfbf"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Foreground=#e5c76b"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background=#232a2d"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon#SearchGlyph > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Foreground=#232a2d"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid", {
        L"Background=#d28ccf7e"}},
}};

const Theme g_themeEverblush_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background=#141b1e",
        L"BorderBrush=#268ccf7e"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Background=#141b1e"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > Border", {
        L"Background=#232a2d",
        L"BorderBrush=transparent"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"Background=#232a2d"}},
    ThemeTargetStyles{L"TextBlock#PlaceholderText", {
        L"Foreground=#80b3b9b8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button", {
        L"Background=#d28ccf7e"}},
    ThemeTargetStyles{L"StackPanel > Windows.UI.Xaml.Controls.Button", {
        L"Background=transparent",
        L"BorderBrush=transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ItemsRepeater > Windows.UI.Xaml.Controls.Button", {
        L"Background=transparent",
        L"BorderBrush=transparent"}},
    ThemeTargetStyles{L"TextBlock#DisplayName", {
        L"Foreground=#b3b9b8"}},
    ThemeTargetStyles{L"TextBlock#Title", {
        L"Foreground=#b3b9b8"}},
    ThemeTargetStyles{L"TextBlock#Subtitle", {
        L"Foreground=#6cbfbf"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Foreground=#8ccf7e"}},
    ThemeTargetStyles{L"TextBlock#TopLevelSuggestionsListHeaderText", {
        L"Foreground=#8ccf7e"}},
    ThemeTargetStyles{L"TextBlock#AllAppsHeading", {
        L"Foreground=#8ccf7e"}},
    ThemeTargetStyles{L"TextBlock#MoreSuggestionsListHeaderText", {
        L"Foreground=#8ccf7e"}},
    ThemeTargetStyles{L"TextBlock#AppDisplayName", {
        L"Foreground=#b3b9b8"}},
    ThemeTargetStyles{L"TextBlock#Text", {
        L"Foreground=#e5c76b"}},
    ThemeTargetStyles{L"TextBlock#FolderGlyph", {
        L"Foreground=#e5c76b"}},
    ThemeTargetStyles{L"TextBlock#StatusMessage", {
        L"Foreground=#8ccf7e"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border@CommonStates", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0,0.5\"> <GradientStop Color=\"#268ccf7e\" Offset=\"0\" /><GradientStop Color=\"#26e5c76b\" Offset=\"1\" /></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0,0.5\"> <GradientStop Color=\"#828ccf7e\" Offset=\"0\" /><GradientStop Color=\"#82e5c76b\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"Background=transparent"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background=#141b1e"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background=#232a2d",
        L"BorderBrush=transparent"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"TextBlock#UserTileNameText", {
        L"Foreground=#67b0e8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Foreground=#6cbfbf"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Foreground=#e5c76b"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background=#232a2d"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon#SearchGlyph > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Foreground=#232a2d"}},
}};

const Theme g_themeSunValley = {{
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage", {
        L"Margin=-2,0,0,0"}},
    ThemeTargetStyles{L"Border#TaskbarMargin", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Grid.Row=3",
        L"Margin=0,0,0,2",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"Height=44",
        L"VerticalAlignment=2",
        L"CornerRadius=0,0,7,7",
        L"BorderThickness=0,0,1,1"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl", {
        L"Margin=0,0,0,2",
        L"Grid.Row=3",
        L"Height=44",
        L"VerticalAlignment=2",
        L"HorizontalAlignment=0",
        L"Width=340"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl > Grid@SearchBoxStates", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"1\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderThickness=1",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.2\" />",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#SearchIconPlayer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Button#SearchGlyphContainer", {
        L"Visibility=Visible",
        L"Width=35",
        L"Margin=2,0,-11,0"}},
    ThemeTargetStyles{L"Button#SearchGlyphContainer > Grid > ContentPresenter > FontIcon", {
        L"FontFamily=Segoe Fluent Icons",
        L"FontSize=17",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.8\" />"}},
    ThemeTargetStyles{L"Cortana.UI.Views.CortanaRichSearchBox#SearchTextBox > Grid > TextBlock#PlaceholderTextContentPresenter", {
        L"Text=Type here to search",
        L"FontFamily=Segoe UI",
        L"FontSize=15",
        L"Padding=39,0,0,0",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.8\" />"}},
    ThemeTargetStyles{L"Cortana.UI.Views.CortanaRichSearchBox#SearchTextBox > Grid > ScrollViewer > Border > Grid > ScrollContentPresenter", {
        L"Margin=38,0,0,0",
        L"FontSize=15"}},
    ThemeTargetStyles{L"Grid#SearchBoxOnTaskbarGleamContainer > Grid", {
        L"Margin=0,0,-3,0",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid#RootGrid > Grid#OuterBorderGrid > Grid#BorderGrid > Border", {
        L"CornerRadius=7,7,0,0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid#RootGrid > Grid#OuterBorderGrid > Grid#BorderGrid > Border#AppBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"Visibility=Visible",
        L"BorderThickness=1,1,1,0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid#RootGrid > Grid#OuterBorderGrid > Grid#BorderGrid > Border#dropshadow", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl > Grid > Grid#UnderlineContainer", {
        L"Visibility=0",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=0,0,0,2",
        L"CornerRadius=5",
        L"Margin=-3,0,-3,0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"Margin=28,0,0,0",
        L"Text=Type here to search"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid > Border#BorderElement", {
        L"BorderThickness=0,0,0,2",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid > FontIcon#SearchGlyph", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.5\" />",
        L"Margin=13,0,-13,1",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.1\" />",
        L"CornerRadius=4",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"CornerRadius=4",
        L"Height=40"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowAllAppsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#ShowAllAppsButtonText", {
        L"Text=All apps"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllAppsHeading", {
        L"Text=All apps"}},
    ThemeTargetStyles{L"Image#SearchIconOn", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Image#SearchIconOff", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton > Grid > Border", {
        L"CornerRadius=0",
        L"BorderThickness=0,0,0,2",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\"/>",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllListHeadingText", {
        L"Text=All apps"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid@CommonStates", {
        L"Background@Normal:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" />",
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\" />",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" FallbackColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" />",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"Text=Type here to search",
        L"Margin=24,0,0,0",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.5\" />"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton", {
        L"Height=40"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > Rectangle#TextCaret", {
        L"Margin=24,0,0,0"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid", {
        L"BorderThickness=1",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" FallbackColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" />",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button > Grid@CommonStates", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" FallbackColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" />",
        L"BorderThickness=1",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion > Border", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" FallbackColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" />",
        L"BorderThickness=1",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion > Border > ContentPresenter", {
        L"Height=40",
        L"Width=40"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion", {
        L"Height=40",
        L"Width=40"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion > Border > ContentPresenter > FontIcon > Grid > TextBlock", {
        L"FontSize=20"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@Normal:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" />",
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\" />"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > FontIcon#SearchGlyph", {
        L"Visibility=Visible",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.5\" />",
        L"Margin=13,0,-13,1"}},
    ThemeTargetStyles{L"Grid#AnimationRoot > Grid#MainMenu > Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MainContent", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"CornerRadius=7"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton > Grid@CommonStates", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" FallbackColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" />",
        L"BorderThickness=1",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@Normal:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" />",
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\" />"}},
    ThemeTargetStyles{L"Button#HideMoreSuggestionsButton > Grid@CommonStates", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" FallbackColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" />",
        L"BorderThickness=1",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Button#HideMoreSuggestionsButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@Normal:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" />",
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\" />"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichComposerBoxControl > Grid > Cortana.UI.Views.CortanaRichSearchBox", {
        L"CornerRadius=4",
        L"Grid.Row=3",
        L"Margin=-1,0,-1,0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichComposerBoxControl > Grid", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"60\" />",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderThickness=1",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.2\" />",
        L"Height=44",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Grid#SearchBoxOnTaskbarGleamContainer > Grid > Image", {
        L"Height=40"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichComposerBoxControl", {
        L"Margin=-4,22,-4,0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichComposerBoxControl > Grid > Grid#ComposerBoxOnTaskbarGleamContainer", {
        L"Padding=12,6,6,6"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichComposerBoxControl > Grid > Cortana.UI.Views.CortanaRichSearchBox > Grid", {
        L"BorderThickness=0,0,0,2",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichComposerBoxControl > Grid > Cortana.UI.Views.CortanaRichSearchBox > Grid > ScrollViewer", {
        L"Margin=8,0,8,0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichComposerBoxControl > Grid > Cortana.UI.Views.CortanaRichSearchBox > Grid > TextBlock", {
        L"Margin=8,0,0,1",
        L"Text=Ask me anything"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichComposerBoxControl > Grid > StackPanel", {
        L"Margin=0,0,5,0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichComposerBoxControl > Grid > StackPanel > Grid > Button > ContentPresenter", {
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootGrid > Grid#RootContent > Border#AcrylicBorder", {
        L"Opacity=0.5"}},
}};

const Theme g_theme21996 = {{
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"CornerRadius=4",
        L"BorderThickness=0,0,0,0",
        L"Height=33",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource ControlStrokeColorDefault}\"/>"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"Margin=28,0,0,2",
        L"Text=Type here to search"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid > Border#BorderElement", {
        L"BorderThickness=2",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>"}},
    ThemeTargetStyles{L"FontIcon#SearchGlyph", {
        L"Foreground:=<SolidColorBrush Color=\"gray\" />",
        L"Margin=13,0,-13,1",
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"16\" />",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#SearchIconPlayer", {
        L"Visibility=1",
        L"FlowDirection=1",
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"16\" />"}},
    ThemeTargetStyles{L"FontIcon#SearchBoxOnTaskbarSearchGlyph", {
        L"Visibility=0",
        L"Foreground:=<SolidColorBrush Color=\"gray\" />",
        L"FlowDirection=1",
        L"FontFamily=Segoe Fluent Icons",
        L"RequestedTheme=1",
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"23\" TranslateY=\"0.5\" />",
        L"FontSize=17"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource ControlStrokeColorDefault}\"/>",
        L"BorderThickness=1,1,1,0",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl > Grid#RootGrid", {
        L"CornerRadius=4",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />",
        L"BorderThickness=2,2,2,2",
        L"Margin=-2,-0,0,-2"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"CornerRadius=4",
        L"Height=40"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SearchBoxOnTaskbarGleamContainer", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SearchBoxOnTaskbarGleamImageContainer", {
        L"CornerRadius=4",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"1.8\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#SearchIconOff", {
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"16\" TranslateY=\"-1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#SearchIconOn", {
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"16\" TranslateY=\"-1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowAllAppsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#ShowAllAppsButtonText", {
        L"Text=All apps"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllAppsHeading", {
        L"Text=All apps"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Height=0",
        L"Margin=0,0,0,32"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame", {
        L"Height=670"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#InnerContent", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.HostedWebViewControl#QueryFormulationHostedWebView", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"1\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#QueryFormulationRoot", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MainContent", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.5\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
        L"CornerRadius=7"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AppBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Border#FullBleedBackground", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.76\" TintLuminosityOpacity=\"0.77\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Image#SearchIconOn", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Image#SearchIconOff", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton > Grid > Border", {
        L"CornerRadius=3",
        L"BorderThickness=2",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\"/>",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllListHeadingText", {
        L"Text=All apps"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid@CommonStates", {
        L"Background@Normal:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" FallbackColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"1\" />",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" FallbackColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" />",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"Text=Type here to search",
        L"Margin=24,0,0,0"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton", {
        L"Height=40"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > Rectangle#TextCaret", {
        L"Margin=24,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PlaceholderTextContentPresenter", {
        L"Text=Type here to search",
        L"Width=750",
        L"TextAlignment=Left"}},
}};

const Theme g_theme21996_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"CornerRadius=4",
        L"BorderThickness=0,0,0,0",
        L"Height=33",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource ControlStrokeColorDefault}\"/>"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"Margin=0,0,0,2"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid > Border#BorderElement", {
        L"BorderThickness=0,0,0,2",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > FontIcon > Grid > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"gray\" />",
        L"Margin=0,0,0,1",
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"16\" />"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#SearchIconPlayer", {
        L"Visibility=1",
        L"FlowDirection=1",
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"16\" />"}},
    ThemeTargetStyles{L"FontIcon#SearchBoxOnTaskbarSearchGlyph", {
        L"Visibility=0",
        L"Foreground:=<SolidColorBrush Color=\"gray\" />",
        L"FlowDirection=1",
        L"FontFamily=Segoe Fluent Icons",
        L"RequestedTheme=1",
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"23\" TranslateY=\"0.5\" />",
        L"FontSize=17"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource ControlStrokeColorDefault}\"/>",
        L"BorderThickness=1,1,1,0",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl > Grid#RootGrid", {
        L"CornerRadius=4",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />",
        L"BorderThickness=2,2,2,2",
        L"Margin=-2,-0,0,-2"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"CornerRadius=4",
        L"Height=40"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SearchBoxOnTaskbarGleamContainer", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SearchBoxOnTaskbarGleamImageContainer", {
        L"CornerRadius=4",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"1.8\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#SearchIconOff", {
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"16\" TranslateY=\"-1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#SearchIconOn", {
        L"Transform3D:=<CompositeTransform3D RotationY=\"180\" TranslateX=\"16\" TranslateY=\"-1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowAllAppsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#ShowAllAppsButtonText", {
        L"Text=All apps"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllAppsHeading", {
        L"Text=All apps"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Height=0",
        L"Margin=0,0,0,32"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame", {
        L"Height=670"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#InnerContent", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.HostedWebViewControl#QueryFormulationHostedWebView", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"1\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#QueryFormulationRoot", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MainContent", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.5\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
        L"CornerRadius=7"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AppBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />"}},
}};

const Theme g_themeUniMenu = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#MoreSuggestionsRoot > Grid", {
        L"RenderTransform:=<TranslateTransform Y=\"24\" />"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.AppListView", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"MaxHeight=520",
        L"Margin=-8,10,0,0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Margin=42,25,42,0",
        L"MaxHeight=380"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton", {
        L"Width=430",
        L"Height=40",
        L"Margin=-50,0,0,0",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"RenderTransform:=<TranslateTransform Y=\"11\" />"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > Border#BorderElement", {
        L"CornerRadius=5",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Margin=-135,-950,0,0",
        L"Canvas.ZIndex=99"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#PinnedList > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem", {
        L"Margin=5,0,0,0",
        L"Padding=-1,0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList > Grid#Root", {
        L"Padding=0"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=\uE76C]", {
        L"Text=\uEA37",
        L"FontSize=16"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button > Grid@CommonStates > Border", {
        L"Width=40",
        L"Height=40",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\".2\" TintLuminosityOpacity=\".3\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".2\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"BorderThickness=1",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\".2\" TintLuminosityOpacity=\".3\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".2\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=\uE76B]", {
        L"Text=\uE846",
        L"FontSize=16"}},
    ThemeTargetStyles{L"Grid#AllAppsRoot", {
        L"Margin=0,-20,0,-40"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Margin=0,-64,0,-58",
        L"BorderThickness=0",
        L"Background=Transparent",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Margin=6,6,6,12",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage", {
        L"Margin=5,0,0,8"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid#RootGrid", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".65\" Opacity=\"1\"/>",
        L"BorderThickness=2",
        L"Padding=3,3,3,-8",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"MaxWidth=650"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid[2] > Button", {
        L"Width=40",
        L"Height=40",
        L"Margin=-73,-6,73,6"}},
    ThemeTargetStyles{L"TextBlock#ShowMorePinnedButtonText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon[Glyph=\uE70D]", {
        L"Glyph=\uEA37",
        L"FontSize=16",
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"Margin=0,-58,0,0"}},
    ThemeTargetStyles{L"StartMenu.StartHome", {
        L"Margin=0,-17,0,-52"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton", {
        L"Height=40",
        L"Width=40"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton > Border@CommonStates > ContentPresenter", {
        L"Width=40",
        L"Height=40",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".2\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\".2\" TintLuminosityOpacity=\".3\" Opacity=\"1\"/>",
        L"BorderThickness=1",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"CornerRadius=5",
        L"RenderTransform:=<TranslateTransform Y=\"11\" X=\"-4\" />",
        L"Background@CheckedPointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".2\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Grid", {
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"Grid#AnimationRoot", {
        L"CornerRadius=8",
        L"BorderThickness=2,2,2,3",
        L"Background=Transparent",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".65\" Opacity=\"1\"/>",
        L"Padding=-1.5,-1.5,-1.5,-6"}},
    ThemeTargetStyles{L"StartMenu.StartMenuCompanion#RightCompanion > Grid", {
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid#CompanionRoot > Grid#MainContent > Border#AcrylicOverlay", {
        L"Margin=0,-64,0,-58",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Border#Root > Grid > ScrollContentPresenter > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Border > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Grid > Border > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > TextBlock", {
        L"Margin=6.5,5,0,-4"}},
    ThemeTargetStyles{L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Windows.UI.Xaml.Controls.TextBlock[Text=View your recent calls, messages, photos, and more.]", {
        L"Margin=5.5,0,0,0"}},
    ThemeTargetStyles{L"Border#RightCompanionDropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#CompanionRoot@CloseCompanionButtonStates", {
        L"Margin=-6,0,6,0",
        L"Padding=-2,0"}},
    ThemeTargetStyles{L"Grid#FrameRoot@CompanionStates > Grid > Grid#RightCompanionContainerGrid", {
        L"OpacityTransition:=<ScalarTransition Duration=\"0:0:0.014\" />",
        L"Opacity@CloseCompanion=0",
        L"Opacity@OpeningCompanion_Large=0.14",
        L"Opacity@OpenCompanion=1",
        L"Opacity@OpeningOverlapCompanion=0.64",
        L"Opacity@ClosingOverlapCompanion=0.64",
        L"Opacity@ClosingCompanion_Large=0.24"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton", {
        L"Margin=-67,-22,67,0"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton > Grid > ContentPresenter > StackPanel > TextBlock", {
        L"Text=",
        L"Margin=0"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton > Grid@CommonStates > ContentPresenter", {
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".2\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\".2\" TintLuminosityOpacity=\".3\" Opacity=\"1\"/>",
        L"Width=40",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"CornerRadius=5",
        L"Height=40"}},
    ThemeTargetStyles{L"Grid#MainContent", {
        L"Margin=0,-50,0,0"}},
    ThemeTargetStyles{L"Grid#MainContent > Grid", {
        L"Margin=-19,2,18,-80"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter#PrimaryCardContainer", {
        L"Margin=0,40,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#ZoomedOutListView", {
        L"Margin=0,40,0,0"}},
    ThemeTargetStyles{L"Button#HideMoreSuggestionsButton", {
        L"Margin=-67,-14,67,0",
        L"Height=40"}},
    ThemeTargetStyles{L"Button#HideMoreSuggestionsButton > Grid@CommonStates > ContentPresenter", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"Width=40",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\".2\" TintLuminosityOpacity=\".3\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".2\" Opacity=\"1\"/>",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Button#HideMoreSuggestionsButton > Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > TextBlock", {
        L"Text=\uE0A6",
        L"FontFamily=Segoe Fluent Icons",
        L"Margin=0",
        L"FontSize=16"}},
    ThemeTargetStyles{L"Button#HideMoreSuggestionsButton > Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.FontIcon > Grid > TextBlock", {
        L"Visibility=Collapsed"}},
}};

const Theme g_themeUniMenu_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.AppListView", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Rectangle", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MaxHeight=520",
        L"Margin=-8,10,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Margin=0,-70,0,-90"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=375",
        L"Margin=13,30,-13,0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Width=480",
        L"Height=40",
        L"Margin=-100,0,0,30",
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=5",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Margin=-68,-870,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#PinnedList > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem", {
        L"Margin=5,10,0,0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList > Grid#Root", {
        L"Padding=0"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#ShowAllAppsButtonText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton", {
        L"Margin=0,-2,30,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=\uE76C]", {
        L"Text=\uEA37",
        L"FontSize=16"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Width=40",
        L"Height=40",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\".2\" TintLuminosityOpacity=\".3\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".2\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"BorderThickness=1",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\".2\" TintLuminosityOpacity=\".3\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".2\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=Back]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock[Text=\uE76B]", {
        L"Text=\uE846",
        L"FontSize=16"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter@CommonStates", {
        L"Width=40",
        L"Height=40",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\".3\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\".2\" TintLuminosityOpacity=\".3\" Opacity=\"1\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".2\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton", {
        L"Margin=0,-102,-33,0"}},
    ThemeTargetStyles{L"Grid#AllAppsRoot", {
        L"Margin=0,-20,0,-40"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Margin=0,0,0,-58",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".65\" Opacity=\"1\"/>",
        L"BorderThickness=2",
        L"Background=Transparent",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Margin=6,6,6,12",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Border#DropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#RootGridDropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#RightCompanionDropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage", {
        L"Margin=5,0,0,8"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid#RootGrid", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".65\" Opacity=\"1\"/>",
        L"BorderThickness=2",
        L"Padding=3,3,3,-8",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=Collapsed"}},
}};

const Theme g_themeLegacyFluent = {{
    ThemeTargetStyles{L"GridView#PinnedList > Border > ScrollViewer > Border > Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Border#ContentBorder@CommonStates > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper", {
        L"BorderBrush:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"BorderThickness=1.5",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemListLowColor}\" Opacity=\"1\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#PinnedList > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem", {
        L"Width=100",
        L"Height=100",
        L"Margin=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#PinnedList > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid", {
        L"HorizontalAlignment=Center"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText", {
        L"Height=0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=518"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=Recommended"}},
    ThemeTargetStyles{L"StartMenu.PinnedListTile > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#DisplayNameAndDownloadIconContainer > Windows.UI.Xaml.Controls.TextBlock#DisplayName", {
        L"Margin=4,0,0,2",
        L"TextAlignment=1"}},
    ThemeTargetStyles{L"StartMenu.PinnedListTile > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#DisplayNameAndDownloadIconContainer", {
        L"HorizontalAlignment=1",
        L"Width=95",
        L"Margin=0",
        L"VerticalAlignment=2"}},
    ThemeTargetStyles{L"StartMenu.PinnedListTile > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#LogoContainer", {
        L"Margin=0,17,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid", {
        L"Height=95",
        L"Width=100"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Text=PINNED",
        L"FontWeight=Bold",
        L"Margin=78,-4,0,4"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsStackPanel > StartDocked.AppListViewItem > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"CornerRadius=0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicOverlay", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"Margin=-13,0,0,-13"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=0",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" Opacity=\"0.5\"/>",
        L"BorderThickness=2",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemListMediumColor}\"/>",
        L"BorderBrush@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\"/>",
        L"BorderBrush@CheckedPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#SearchIconOn", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#SearchIconOff", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon#SearchGlyph", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PlaceholderText", {
        L"Text=Type here to search"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.ContentPresenter@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background=Transparent",
        L"Height=30"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListViewItem > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.Button#Header > Windows.UI.Xaml.Controls.Border@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background=Transparent",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListViewItem > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BorderBackground", {
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background=Transparent",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllAppsHeading", {
        L"Text=ALL",
        L"FontWeight=Bold"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#StatusMessage[Text=System]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#Root > Windows.UI.Xaml.Controls.Grid#VerticalRoot > Windows.UI.Xaml.Controls.Primitives.Thumb > Windows.UI.Xaml.Shapes.Rectangle#ThumbVisual", {
        L"RadiusX=0",
        L"RadiusY=0",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#VerticalTrackRect", {
        L"RadiusX=0",
        L"RadiusY=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#VerticalSmallIncrease > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.FontIcon#Arrow > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE011"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#VerticalSmallDecrease > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.FontIcon#Arrow > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE010"}},
    ThemeTargetStyles{L"StartDocked.AllAppsZoomListViewItem > Windows.UI.Xaml.Controls.Grid#ContentBorder@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background=Transparent",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"BorderThickness@PointerOver=2",
        L"BorderThickness@Pressed=2"}},
    ThemeTargetStyles{L"StartDocked.AllAppsZoomListViewItem > Windows.UI.Xaml.Controls.Grid#ContentBorder@DisabledStates > Windows.UI.Xaml.Controls.Border", {
        L"RenderTransform@Disabled:=<ScaleTransform ScaleX=\"0\" ScaleY=\"0\" CenterX=\"0.5\" CenterY=\"0.5\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#LayerBorder", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage", {
        L"RenderTransform:=<TranslateTransform X=\"-13\" Y=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#TaskbarMargin", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AppBorder", {
        L"CornerRadius=0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#StartDropShadow", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootGrid@SearchBoxInputStates > Windows.UI.Xaml.Controls.Border#TaskbarSearchBackground", {
        L"CornerRadius=0",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" Opacity=\"0.5\"/>",
        L"BorderThickness=2",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemListMediumColor}\"/>",
        L"BorderBrush@SearchBoxHover:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\"/>",
        L"BorderBrush@FindInStartSearchBoxHover:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\"/>",
        L"Margin=25,37,21,15"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SearchGlyphContainer", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#SearchIconPlayer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#MoreSuggestionsListHeaderText", {
        L"Text=RECOMMENDED",
        L"FontWeight=Bold"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#RecommendedList > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsStackPanel > Windows.UI.Xaml.Controls.ListViewItem > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background=Transparent",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter > Windows.UI.Xaml.Controls.Border", {
        L"Background:=<AcrylicBrush TintColor=\"#22848484\" TintOpacity=\"0.2\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"#22848484\" TintOpacity=\"0.2\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe UI, Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe MDL2 Assets, Segoe UI"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutItem", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewItem", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#HideMoreSuggestionsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontSize=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"RenderTransform:=<TranslateTransform X=\"-18.5\" Y=\"-586\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowMoreSuggestionsButton > Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=Recommended"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"Width=650"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"Height=750"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridViewItem", {
        L"Margin=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#RecommendedList", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"Margin=0,0,0,-190"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowMoreSuggestionsButton", {
        L"Style:=<Style x:Key=\"RevealButtonStyle\" TargetType=\"Button\" />"}},
    ThemeTargetStyles{L"Button#CloseStartAccessibleButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid#RootGrid", {
        L"CornerRadius=0",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.6\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Border#ContentBorder@CommonStates > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Margin=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>"}},
    ThemeTargetStyles{L"Button#Header", {
        L"Style:=",
        L"Height=40"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#ZoomedOutListView", {
        L"Margin=0,-150,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewItem > Grid#ContentBorder@CommonStates > Border", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"Background@Pressed=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>"}},
}, {}, {}, {
    ThemeTargetStyles{L"#chatButtonRight", {
        L"display: none !important"}},
    ThemeTargetStyles{L".groupTitle", {
        L"text-transform: uppercase !important",
        L"font-weight: bold !important"}},
    ThemeTargetStyles{L"div, span, h1, h2, h3, h4, h5, p", {
        L"font-family: Segoe UI !important"}},
    ThemeTargetStyles{L".cortanaFontIcon, .iconContent", {
        L"font-family: Segoe MDL2 Assets !important"}},
}};

const Theme g_themeLegacyFluent_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridViewItem > Windows.UI.Xaml.Controls.Border#ContentBorder@CommonStates > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"BorderThickness=2",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemListLowColor}\" Opacity=\"1\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridViewItem", {
        L"Width=100",
        L"Height=100",
        L"Margin=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"HorizontalAlignment=Center"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText", {
        L"Height=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"RenderTransform:=<TranslateTransform Y=\"-586\" X=\"-55\" />"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=518"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions > Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=Recommended"}},
    ThemeTargetStyles{L"StartMenu.PinnedListTile > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#DisplayNameAndDownloadIconContainer > Windows.UI.Xaml.Controls.TextBlock#DisplayName", {
        L"Margin=4,0,0,2",
        L"TextAlignment=1"}},
    ThemeTargetStyles{L"StartMenu.PinnedListTile > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#DisplayNameAndDownloadIconContainer", {
        L"HorizontalAlignment=1",
        L"Width=95",
        L"Margin=0",
        L"VerticalAlignment=2"}},
    ThemeTargetStyles{L"StartMenu.PinnedListTile > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid#LogoContainer", {
        L"Margin=0,17,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid", {
        L"Height=95",
        L"Width=100"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Text=PINNED",
        L"FontWeight=Bold"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsStackPanel > StartDocked.AppListViewItem > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"CornerRadius=0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicOverlay", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"Margin=-13,13,0,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=0",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" Opacity=\"0.5\"/>",
        L"BorderThickness=2",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemListMediumColor}\"/>",
        L"BorderBrush@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\"/>",
        L"BorderBrush@CheckedPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#SearchIconOn", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#SearchIconOff", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon#SearchGlyph", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PlaceholderText", {
        L"Text=Type here to search"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button > Windows.UI.Xaml.Controls.ContentPresenter@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background=Transparent",
        L"Height=30"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListViewItem > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.Button#Header > Windows.UI.Xaml.Controls.Border@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background=Transparent",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListViewItem > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BorderBackground", {
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background=Transparent",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllAppsHeading", {
        L"Text=ALL",
        L"FontWeight=Bold"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#StatusMessage[Text=System]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#Root > Windows.UI.Xaml.Controls.Grid#VerticalRoot > Windows.UI.Xaml.Controls.Primitives.Thumb > Windows.UI.Xaml.Shapes.Rectangle#ThumbVisual", {
        L"RadiusX=0",
        L"RadiusY=0",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#VerticalTrackRect", {
        L"RadiusX=0",
        L"RadiusY=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#VerticalSmallIncrease > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.FontIcon#Arrow > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE011"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.RepeatButton#VerticalSmallDecrease > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.FontIcon#Arrow > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE010"}},
    ThemeTargetStyles{L"StartDocked.AllAppsZoomListViewItem > Windows.UI.Xaml.Controls.Grid#ContentBorder@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background=Transparent",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"BorderThickness@PointerOver=2",
        L"BorderThickness@Pressed=2"}},
    ThemeTargetStyles{L"StartDocked.AllAppsZoomListViewItem > Windows.UI.Xaml.Controls.Grid#ContentBorder@DisabledStates > Windows.UI.Xaml.Controls.Border", {
        L"RenderTransform@Disabled:=<ScaleTransform ScaleX=\"0\" ScaleY=\"0\" CenterX=\"0.5\" CenterY=\"0.5\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#LayerBorder", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage", {
        L"RenderTransform:=<TranslateTransform X=\"-13\" Y=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#TaskbarMargin", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AppBorder", {
        L"CornerRadius=0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#dropshadow", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootGrid@SearchBoxInputStates > Windows.UI.Xaml.Controls.Border#TaskbarSearchBackground", {
        L"CornerRadius=0",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" Opacity=\"0.5\"/>",
        L"BorderThickness=2",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemListMediumColor}\"/>",
        L"BorderBrush@SearchBoxHover:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\"/>",
        L"BorderBrush@FindInStartSearchBoxHover:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\"/>",
        L"Margin=25,37,21,15"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SearchGlyphContainer", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#SearchIconPlayer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#MoreSuggestionsListHeaderText", {
        L"Text=RECOMMENDED",
        L"FontWeight=Bold"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListView#RecommendedList > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ScrollViewer > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsStackPanel > Windows.UI.Xaml.Controls.ListViewItem > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border", {
        L"BorderThickness=1",
        L"CornerRadius=0",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"{ThemeResource SystemListLowColor}\" TargetTheme=\"1\" Opacity=\"0.5\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"{ThemeResource SystemChromeHighColor}\" TargetTheme=\"1\" Opacity=\"0.6\" FallbackColor=\"{ThemeResource SystemListLowColor}\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Background=Transparent",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe UI, Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe MDL2 Assets, Segoe UI"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutItem", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ListViewItem", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#HideMoreSuggestionsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontSize=10"}},
}, {}, {}, {
    ThemeTargetStyles{L"#chatButtonRight", {
        L"display: none !important"}},
    ThemeTargetStyles{L".groupTitle", {
        L"text-transform: uppercase !important",
        L"font-weight: bold !important"}},
    ThemeTargetStyles{L"div, span, h1, h2, h3, h4, h5, p", {
        L"font-family: Segoe UI !important"}},
    ThemeTargetStyles{L".cortanaFontIcon, .iconContent", {
        L"font-family: Segoe MDL2 Assets !important"}},
}};

const Theme g_themeOnlySearch = {{
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"MaxHeight=160",
        L"MinHeight=100"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Height=3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#RightCompanionContainerGrid", {
        L"Visibility=Collapsed"}},
}};

const Theme g_themeOnlySearch_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MaxHeight=160"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MinHeight=100"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Visibility=Collapsed"}},
}};

const Theme g_themeWindowGlass = {{
    ThemeTargetStyles{L"StackPanel#TimeAndDatePanel", {
        L"VerticalAlignment=Top",
        L"HorizontalAlignment=Center",
        L"RenderTransform:=<TranslateTransform X=\"0\" />"}},
    ThemeTargetStyles{L"StackPanel#TimePanel > TextBlock#Time", {
        L"HorizontalAlignment:=Center",
        L"RenderTransform:=<TransformGroup><TranslateTransform X=\"-30\" Y=\"-10\" /><ScaleTransform ScaleX=\"3.3\" ScaleY=\"6\" /></TransformGroup>",
        L"FontFamily=Morganite SemiBold",
        L"Foreground:=$ClockBG"}},
    ThemeTargetStyles{L"StackPanel#TimeAndDatePanel > TextBlock#Date", {
        L"HorizontalAlignment=Center",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"-110\" />",
        L"FontFamily=vivo Sans EN VF",
        L"Foreground:=$ClockBG"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#WidgetFrameGrid", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#WidgetCanvasPanel", {
        L"HorizontalAlignment=Center",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"50\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaTransportControls", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MediaControlsContainer", {
        L"Visibility=Visible",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"-250\" />",
        L"Margin=0,0,0,0",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootPanel > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#RootContent", {
        L"Margin=-20,-20,-20,0"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"Width=860"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#RootGridDropShadow", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#RightCompanionDropShadow", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#StartDropShadow", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#DropShadowDismissTarget", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius",
        L"Margin=2",
        L"Padding=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootContent > Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"Background:=$ElementBG",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius",
        L"Margin=0,60,0,10",
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AcrylicOverlay", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox", {
        L"Width=650",
        L"Height=50",
        L"Margin=0,-15,0,0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BorderElement", {
        L"Background:=$ElementBG",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock#PlaceholderText", {
        L"Text=Search This Precision"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelRoot > Windows.UI.Xaml.Controls.Grid", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#NavigationPane", {
        L"Width=550",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"10\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowAllAppsButton", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"Margin=0",
        L"Height=280"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList > Windows.UI.Xaml.Controls.Grid#Root", {
        L"Background:=$ElementBG",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Visibility=0",
        L"Width=650",
        L"Margin=0,-130,0,230",
        L"Canvas.ZIndex=1",
        L"MaxHeight:=340"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Visibility=0",
        L"Margin=-1600,190,115,-100",
        L"MaxHeight=330",
        L"Background:=$ElementBG",
        L"CornerRadius=$ElementCornerRadius",
        L"Width=650",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllAppsHeading", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartDocked.AllAppsPane#AllAppsPanel", {
        L"Margin=-20,-20,20,20"}},
    ThemeTargetStyles{L"StartDocked.StartMenuCompanion#RightCompanion > Windows.UI.Xaml.Controls.Grid#CompanionRoot > Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"Background:=$ElementBG",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius:=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#CompanionRoot > Windows.UI.Xaml.Controls.Grid#MainContent > Windows.UI.Xaml.Controls.Grid#ActionsBar > Windows.UI.Xaml.Controls.Button#PrimaryActionBarButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background:=$ElementBG",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$ElementCornerRadius",
        L"Height=40"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ActionsBar > Windows.UI.Xaml.Controls.Button#ActionBarOverflowButton", {
        L"Background:=$ElementBG",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$ElementCornerRadius",
        L"Height=40"}},
    ThemeTargetStyles{L"StartDocked.StartMenuCompanion#RightCompanion > Windows.UI.Xaml.Controls.Grid#CompanionRoot", {
        L"Height=730",
        L"Margin=0,-10,0,-10",
        L"Padding=10,0,-2,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter > Windows.UI.Xaml.Controls.Border", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#HoverFlyoutGrid > Windows.UI.Xaml.Controls.Border#HoverFlyoutBackground", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#OuterBorderGrid", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#LayerBorder", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AccentLayerBorder", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#dropshadow", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#AppBorder", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter#LayoutRoot", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"StartMenu.FolderModal#StartFolderModal > Windows.UI.Xaml.Controls.Grid#Root", {
        L"MaxHeight:=420",
        L"MaxWidth:=420",
        L"Height=Auto",
        L"Width=Auto"}},
    ThemeTargetStyles{L"StartMenu.FolderModal#StartFolderModal > Windows.UI.Xaml.Controls.Grid#Root > Windows.UI.Xaml.Controls.ContentControl#ContentControl > Windows.UI.Xaml.Controls.ContentPresenter > StartMenu.UniversalTileContainer#UniversalTileContainer > Windows.UI.Xaml.Controls.Grid#GridViewContainer", {
        L"Width=400",
        L"Height=400"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#Root > Windows.UI.Xaml.Controls.Border", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList", {
        L"Margin=0,30,0,-120"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MainMenu > Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton", {
        L"Height=50",
        L"Margin=-20,20,-20,-20",
        L"Width=400"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BorderElement", {
        L"Background:=$ElementBG",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius:=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion", {
        L"Margin=-50,40,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllListHeading", {
        L"Margin=0,-10,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllListHeading > Windows.UI.Xaml.Controls.TextBlock#AllListHeadingText", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Border", {
        L"Background:=$ElementBG",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#MainMenu", {
        L"Width=460"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"Width=400",
        L"Height=450",
        L"Margin=0,0,0,30"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#PinnedList > Border > Windows.UI.Xaml.Controls.ScrollViewer", {
        L"ScrollViewer.VerticalScrollMode=2",
        L"MaxHeight:=336",
        L"MinHeight:=100",
        L"Width=300",
        L"Margin=0,0,60,0"}},
    ThemeTargetStyles{L"StartMenu.StartMenuCompanion#RightCompanion", {
        L"Height=810",
        L"Margin=15,0,30,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#CompanionRoot > Windows.UI.Xaml.Controls.Border#AcrylicBorder", {
        L"Background:=$ElementBG",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#AllAppsGrid > Windows.UI.Xaml.Controls.ItemsWrapGrid", {
        L"Visibility=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#AllAppsGrid", {
        L"Margin=0,15,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelHeader > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness:=$BorderThickness",
        L"CornerRadius:=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter", {
        L"CornerRadius:=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllListHeading > Microsoft.UI.Xaml.Controls.DropDownButton#ViewSelectionButton > Grid#RootGrid", {
        L"CornerRadius=$HoverCornerRadius",
        L"Margin=-12,0,12,0"}},
    ThemeTargetStyles{L"MenuFlyoutItem", {
        L"CornerRadius:=$HoverCornerRadius",
        L"Margin=4,0,4,0"}},
    ThemeTargetStyles{L"ToggleMenuFlyoutItem", {
        L"CornerRadius:=$HoverCornerRadius",
        L"Margin=4,0,4,0"}},
}, {
    L"Translucent=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#10808080\"/>",
    L"Glass=<WindhawkBlur BlurAmount=\"5\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Frosted=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Acrylic=<WindhawkBlur BlurAmount=\"30\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.8\" />",
    L"Background=$Glass",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#60808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#40808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"BorderBrush2=<WindhawkBlur BlurAmount=\"10\" TintColor=\"#909090\" TintOpacity=\"0.3\"/>",
    L"ClockBG=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" Opacity=\"1\"/>",
    L"BorderThickness=0.3,1,0.3,1",
    L"CornerRadius=35",
    L"SearchBoxRadius=20",
    L"ElementBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.3\" />",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"ElementCornerRadius=25",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"ElementBorderBrush2=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#909090\" TintOpacity=\"0.3\"/>",
    L"HoverCornerRadius=15",
}};

const Theme g_themeFluid = {{
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid > Border#BackgroundBorder", {
        L"BorderThickness=2",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor"}},
    ThemeTargetStyles{L"Button > Grid@CommonStates > Border#BackgroundBorder", {
        L"BorderThickness=2",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor",
        L"BackgroundSizing=InnerBorderEdge"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl > Grid > Border", {
        L"BorderThickness=2",
        L"BorderBrush:=$borderColor",
        L"BackgroundSizing=InnerBorderEdge",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />",
        L"Opacity=0.8"}},
    ThemeTargetStyles{L"Grid#LayoutRoot", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"}},
    ThemeTargetStyles{L"Button#Header > Border@CommonStates", {
        L"BorderThickness=2",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor",
        L"BackgroundSizing=InnerBorderEdge"}},
    ThemeTargetStyles{L"ListViewItem > Grid@CommonStates > Border#BorderBackground", {
        L"BorderThickness=2",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor",
        L"BackgroundSizing=InnerBorderEdge"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid@CommonStates > Border#BorderElement", {
        L"CornerRadius=4",
        L"BorderThickness=1",
        L"BorderBrush:=$borderColor",
        L"Background@Checked:=$backgroundNormal",
        L"Background@CheckedPointerOver:=$backgroundHover",
        L"Background@CheckedPressed:=$backgroundPressed",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"}},
    ThemeTargetStyles{L"Button#HideMoreSuggestionsButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@Normal:=$backgroundNormal",
        L"BorderBrush@Normal:=$borderColor",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor",
        L"Background@PointerOver:=$backgroundHover",
        L"Background@Pressed:=$backgroundPressed",
        L"BorderThickness=1",
        L"Margin=2"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@Normal:=$backgroundNormal",
        L"BorderBrush@Normal:=$borderColor",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor",
        L"Background@PointerOver:=$backgroundHover",
        L"Background@Pressed:=$backgroundPressed",
        L"BorderThickness=1",
        L"Margin=2"}},
    ThemeTargetStyles{L"StartMenu.FolderModal > Grid#Root > Border", {
        L"BorderThickness=1",
        L"BorderBrush:=$borderColor"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"BorderThickness=1",
        L"BorderBrush:=$borderColor"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter#ContentPresenter > ContentControl > Grid#RootGrid > Border#LogoBackgroundPlate > Image#AllAppsItemLogo", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"BorderThickness=1",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor",
        L"BackgroundSizing=InnerBorderEdge"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid@CommonStates > Border#BackgroundBorder", {
        L"BorderThickness=1",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor",
        L"BackgroundSizing=InnerBorderEdge"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid@CommonStates", {
        L"Background@PointerOver:=$backgroundHover",
        L"Background@Pressed:=$backgroundPressed",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor",
        L"BackgroundSizing=InnerBorderEdge",
        L"Background@Normal:=$backgroundNormal",
        L"BorderBrush@Normal:=$borderColor",
        L"Padding=9,3,7,4"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter#ContentPresenter > ContentControl > Grid#RootGrid > Grid#LogoContainer > Image#AllAppsTileLogo", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter > Grid > Grid#LogoContainer > Grid", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter > Grid > Grid#LogoContainer > Grid", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5"}},
    ThemeTargetStyles{L"ScrollViewer#MenuFlyoutPresenterScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > StackPanel", {
        L"ChildrenTransitions:=<TransitionCollection><EntranceThemeTransition IsStaggeringEnabled=\"False\" FromHorizontalOffset=\"-25\" FromVerticalOffset=\"0\" /></TransitionCollection>"}},
    ThemeTargetStyles{L"FlyoutPresenter > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ContentPresenter > Border", {
        L"BorderBrush:=$borderColor",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Button > ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@PointerOver:=$backgroundHover",
        L"Background@Pressed:=$backgroundPressed",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor",
        L"BorderThickness=1",
        L"Background@Normal=Transparent"}},
    ThemeTargetStyles{L"Grid@SearchBoxInputStates > Border#TaskbarSearchBackground", {
        L"CornerRadius=4",
        L"Background@ActiveInput:=$backgroundNormal",
        L"BorderBrush:=$borderColor",
        L"BorderThickness=1",
        L"Background@SearchBoxHover:=$backgroundHover",
        L"Background@NoFocus:=$backgroundNormal",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"}},
    ThemeTargetStyles{L"Border@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter > Grid > Grid#LogoContainer > Image", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates > ContentPresenter > Grid > Grid#LogoContainer > Grid", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5"}},
}, {
    L"borderColor=<LinearGradientBrush x:Key=\"ShellTaskbarItemGradientStrokeColorSecondaryBrush\" MappingMode=\"Absolute\" StartPoint=\"0,0\" EndPoint=\"0,3\"><LinearGradientBrush.GradientStops><GradientStop Offset=\"0.33\" Color=\"#1AFFFFFF\" /><GradientStop Offset=\"1\" Color=\"#0FFFFFFF\" /></LinearGradientBrush.GradientStops></LinearGradientBrush>",
    L"backgroundNormal=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />",
    L"backgroundHover=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" />",
    L"backgroundPressed=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\" />",
}, {}, {
    ThemeTargetStyles{L"*", {
        L"transition: background-color 0.083s ease-in-out !important"}},
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
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=$Alt",
        L"BorderBrush=Transparent",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#DropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#RootGridDropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#RightCompanionDropShadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#BorderElement", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid", {
        L"BorderBrush:=$Reveal",
        L"BorderThickness=2",
        L"CornerRadius=20",
        L"Margin=0,0,0,-8"}},
    ThemeTargetStyles{L"Grid > Image#SearchIconOn", {
        L"Width=20"}},
    ThemeTargetStyles{L"TextBlock#PlaceholderText", {
        L"Text=Search"}},
    ThemeTargetStyles{L"Grid#InnerContent > Rectangle", {
        L"Fill:=$SolidAccent",
        L"MinHeight=22",
        L"MinWidth=2",
        L"Margin=80,8,0,31",
        L"Opacity=1"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"Height=421"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"Background@Normal:=<SolidColorBrush Color=\"Transparent\" Opacity=\"0.8\"/>",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" Opacity=\"0.8\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\" />",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.2\" />",
        L"BorderBrush:=$Reveal",
        L"BorderThickness=2",
        L"CornerRadius=12",
        L"Height=70",
        L"Width=70"}},
    ThemeTargetStyles{L"Grid#LogoContainer", {
        L"Height=60",
        L"Width=60"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter#ContentPresenter > Grid > StartMenu.PinnedListTile > Grid#Root > Grid#LogoContainer > Image#Logo", {
        L"Height=40",
        L"Width=40",
        L"Height@Pressed=36",
        L"Width@Pressed=36"}},
    ThemeTargetStyles{L"Border#FolderPlate", {
        L"Background=Transparent",
        L"BorderBrush=Transparent",
        L"Height=56",
        L"Width=56"}},
    ThemeTargetStyles{L"Grid#LogosContainer", {
        L"Height=68",
        L"Width=68"}},
    ThemeTargetStyles{L"ItemsControl#LogosItemsControl", {
        L"Height=50",
        L"Width=50"}},
    ThemeTargetStyles{L"ItemsControl#LogosItemsControl > ItemsPresenter > ItemsWrapGrid > ContentPresenter > Windows.UI.Xaml.Controls.Grid", {
        L"Height=22",
        L"Width=22"}},
    ThemeTargetStyles{L"Grid#Root > Border", {
        L"Background:=$DarkAccent",
        L"BorderBrush:=Transparent",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"TextBlock#TruncationTextBlock", {
        L"FontSize=30"}},
    ThemeTargetStyles{L"TextBlock#DisplayName", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid", {
        L"BorderBrush:=$Reveal",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid > ContentPresenter > Grid > FontIcon", {
        L"Foreground:=$SolidAccent"}},
    ThemeTargetStyles{L"FontIcon#WindowsUpdatePendingReminder", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"FontIcon#IconOverlay", {
        L"Foreground:=$SolidAccent"}},
    ThemeTargetStyles{L"Grid#AccountBadgePlaceholder > StartDocked.IconBadgeView > Grid#IconBadgeRoot > Grid > Windows.UI.Xaml.Shapes.Ellipse", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton > ContentPresenter", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\" />",
        L"BorderBrush=Transparent",
        L"Height=25"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton > ContentPresenter", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\" />",
        L"BorderBrush=Transparent",
        L"Height=25"}},
    ThemeTargetStyles{L"Button > ContentPresenter > StackPanel > TextBlock", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StackPanel > FontIcon > Grid > TextBlock", {
        L"FontSize=14 //(Default=10)"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#AllAppsHeading", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.AllAppsZoomListViewItem > Grid#ContentBorder@DisabledStates > Border#BorderBackground", {
        L"BorderBrush@Enabled:=$Reveal"}},
    ThemeTargetStyles{L"Border#Border", {
        L"BorderBrush:=$Reveal",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Border#BorderBackground", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=$Alt",
        L"BorderBrush=Transparent",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl > Grid#RootGrid", {
        L"BorderBrush:=$Reveal",
        L"BorderThickness=2",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#SearchIconPlayer", {
        L"Width=20"}},
}, {
    L"Alt=<AcrylicBrush TintColor=\"{ThemeResource SystemAltHighColor}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAltHighColor}\" />",
    L"Accent=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColor}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColor}\" />",
    L"DarkAccent=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.3\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
    L"SolidAccent=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" Opacity=\"1\"/>",
    L"Reveal=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
}, {}, {
    ThemeTargetStyles{L"li.rightHeaderButtons.itemTooltip.MouseHoverTooltip", {
        L"display: none"}},
    ThemeTargetStyles{L".scope-with-background__backButton", {
        L"display: none !important"}},
    ThemeTargetStyles{L".scope-with-background__rightCaret,.scope-with-background__leftCaret", {
        L"display: none !important"}},
    ThemeTargetStyles{L".previewContainer", {
        L"border-radius: 20px !important"}},
    ThemeTargetStyles{L".scopes-list", {
        L"justify-content: center"}},
    ThemeTargetStyles{L".scope-with-background.darkTheme .scope-tile--selected .scope-tile__title,.scope-with-background .scope-tile--selected .scope-tile__title", {
        L"Background: var(--accent0) !important",
        L"color: white !important"}},
    ThemeTargetStyles{L".scope-tile > div", {
        L"background-color: Transparent !important"}},
    ThemeTargetStyles{L".darkTheme #menuContainer", {
        L"background: black",
        L"border: 1px solid #404040",
        L"border-radius: 20px",
        L"box-shadow: none"}},
    ThemeTargetStyles{L"#root.darkTheme:not(.fileExplorer) .contextMenu", {
        L"background: black !important",
        L"border-radius: 20px !important"}},
    ThemeTargetStyles{L".lightTheme #menuContainer", {
        L"background: white !important",
        L"border: 1px solid #404040",
        L"border-radius: 20px !important",
        L"box-shadow: none"}},
    ThemeTargetStyles{L"#root:not(.fileExplorer) .contextMenu", {
        L"background: white !important",
        L"border-radius: 20px !important"}},
    ThemeTargetStyles{L"ul.contextMenu::before", {
        L"display: none !important"}},
}};

const Theme g_themeLiquidGlass = {{
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=$background",
        L"BorderBrush:=$borderColor",
        L"BorderThickness=$borderThickness",
        L"CornerRadius=$cornerRadius"}},
    ThemeTargetStyles{L"Border#AccentAppBorder", {
        L"Background:=$background",
        L"BorderBrush:=$borderColor",
        L"BorderThickness=$borderThickness",
        L"CornerRadius=$cornerRadius"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid > Border#BackgroundBorder", {
        L"BorderThickness=$borderThickness2",
        L"BorderBrush@PointerOver:=$borderColor2",
        L"BorderBrush@Pressed:=$borderColor2",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"Button > Grid@CommonStates > Border#BackgroundBorder", {
        L"BorderThickness=$borderThickness2",
        L"BorderBrush@PointerOver:=$borderColor2",
        L"BorderBrush@Pressed:=$borderColor2",
        L"BackgroundSizing=InnerBorderEdge",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"StartMenu.CategoryControl > Grid > Border", {
        L"BorderThickness=$borderThickness2",
        L"BorderBrush:=$borderColor2",
        L"BackgroundSizing=InnerBorderEdge",
        L"Background:=$background",
        L"Opacity=0.8",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"Grid#LayoutRoot", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"}},
    ThemeTargetStyles{L"Button#Header > Border@CommonStates", {
        L"BorderThickness=$borderThickness2",
        L"BorderBrush@PointerOver:=$borderColor2",
        L"BorderBrush@Pressed:=$borderColor2",
        L"BackgroundSizing=InnerBorderEdge",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"ListViewItem > Grid@CommonStates > Border#BorderBackground", {
        L"BorderThickness=$borderThickness2",
        L"BorderBrush@PointerOver:=$borderColor2",
        L"BorderBrush@Pressed:=$borderColor2",
        L"BackgroundSizing=InnerBorderEdge",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid@CommonStates > Border#BorderElement", {
        L"CornerRadius=$cornerRadius2",
        L"BorderThickness=$borderThickness2",
        L"BorderBrush:=$borderColor2",
        L"Background@Checked:=$background",
        L"Background@CheckedPointerOver:=$background",
        L"Background@CheckedPressed:=$background",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"}},
    ThemeTargetStyles{L"Button#HideMoreSuggestionsButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@Normal:=$background",
        L"BorderBrush@Normal:=$borderColor2",
        L"BorderBrush@PointerOver:=$borderColor2",
        L"BorderBrush@Pressed:=$borderColor2",
        L"Background@PointerOver:=$background",
        L"Background@Pressed:=$background",
        L"BorderThickness=1",
        L"Margin=2",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"Button#ShowMoreSuggestionsButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@Normal:=$background",
        L"BorderBrush@Normal:=$borderColor2",
        L"BorderBrush@PointerOver:=$borderColor2",
        L"BorderBrush@Pressed:=$borderColor2",
        L"Background@PointerOver:=$background",
        L"Background@Pressed:=$background",
        L"BorderThickness=$borderThickness2",
        L"Margin=2",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"StartMenu.FolderModal > Grid#Root > Border", {
        L"BorderThickness=$borderThickness",
        L"BorderBrush:=$borderColor",
        L"CornerRadius=$cornerRadius"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"BorderThickness=$borderThickness",
        L"BorderBrush:=$borderColor",
        L"CornerRadius=$cornerRadius"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter#ContentPresenter > ContentControl > Grid#RootGrid > Border#LogoBackgroundPlate > Image#AllAppsItemLogo", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"BorderThickness=$borderThickness2",
        L"BorderBrush@PointerOver:=$borderColor2",
        L"BorderBrush@Pressed:=$borderColor2",
        L"BackgroundSizing=InnerBorderEdge",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid@CommonStates > Border#BackgroundBorder", {
        L"BorderThickness=$borderThickness",
        L"BorderBrush@PointerOver:=$borderColor",
        L"BorderBrush@Pressed:=$borderColor",
        L"BackgroundSizing=InnerBorderEdge",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid@CommonStates", {
        L"Background@PointerOver:=$background",
        L"Background@Pressed:=$background",
        L"BorderBrush@PointerOver:=$borderColor2",
        L"BorderBrush@Pressed:=$borderColor2",
        L"BackgroundSizing=InnerBorderEdge",
        L"Background@Normal:=$background",
        L"BorderBrush@Normal:=$borderColor2",
        L"Padding=9,3,7,4",
        L"CornerRadius=$cornerRadius2",
        L"BorderThickness@Normal=$borderThickness2",
        L"BorderThickness@PointerOver=$borderThickness2",
        L"BorderThickness@Pressed=$borderThickness2"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter#ContentPresenter > ContentControl > Grid#RootGrid > Grid#LogoContainer > Image#AllAppsTileLogo", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter > Grid > Grid#LogoContainer > Grid", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter > Grid > Grid#LogoContainer > Grid", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"ScrollViewer#MenuFlyoutPresenterScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > StackPanel", {
        L"ChildrenTransitions:=<TransitionCollection><EntranceThemeTransition IsStaggeringEnabled=\"False\" FromHorizontalOffset=\"-25\" FromVerticalOffset=\"0\" /></TransitionCollection>"}},
    ThemeTargetStyles{L"FlyoutPresenter > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ContentPresenter > Border", {
        L"BorderBrush:=$borderColor",
        L"BorderThickness=$borderThickness",
        L"CornerRadius=$cornerRadius"}},
    ThemeTargetStyles{L"Button > ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@PointerOver:=$background",
        L"Background@Pressed:=$background",
        L"BorderBrush@PointerOver:=$borderColor2",
        L"BorderBrush@Pressed:=$borderColor2",
        L"BorderThickness=$borderThickness2",
        L"Background@Normal=$background",
        L"CornerRadius=$cornerRadius2"}},
    ThemeTargetStyles{L"Grid@SearchBoxInputStates > Border#TaskbarSearchBackground", {
        L"CornerRadius=$cornerRadius2",
        L"Background@ActiveInput:=$background",
        L"BorderBrush:=$borderColor2",
        L"BorderThickness=$borderThickness2",
        L"Background@SearchBoxHover:=$background",
        L"Background@NoFocus:=$background",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"}},
    ThemeTargetStyles{L"Border@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter > Grid > Grid#LogoContainer > Image", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates > ContentPresenter > Grid > Grid#LogoContainer > Grid", {
        L"RenderTransform@Pressed:=<ScaleTransform ScaleX=\"0.8\" ScaleY=\"0.8\" />",
        L"RenderTransformOrigin=0.5,0.5"}},
}, {
    L"borderColor=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>\"",
    L"borderColor2=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"background=<WindhawkBlur BlurAmount=\"10\" TintColor=\"#25323232\" TintOpacity=\"0.2\" />",
    L"borderThickness=0.3,1,0.3,0.3",
    L"borderThickness2=0.3,0.3,0.3,1",
    L"cornerRadius=6",
    L"cornerRadius2=4",
}, {}, {
    ThemeTargetStyles{L"*", {
        L"transition: background-color 0.083s ease-in-out !important"}},
}};

const Theme g_themeWindows10X = {{
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Margin=0,10,-180,0",
        L"BorderThickness=0,1,0,0",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\".5\"/>"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background:=$button",
        L"Background@PointerOver:=$buttonHover",
        L"Background@Pressed:=$buttonPress",
        L"CornerRadius=12",
        L"Height=23"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=$acrylic",
        L"CornerRadius=4",
        L"BorderThickness=0,1,0,0",
        L"BorderBrush:=$button",
        L"BackgroundSizing=1"}},
    ThemeTargetStyles{L"Grid#MainContent", {
        L"CornerRadius=3",
        L"Margin=0"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Margin=78,0,0,0",
        L"Text=My apps and websites",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"CornerRadius=4,4,5,5",
        L"Margin=79,35,79,1",
        L"BorderThickness=0,0,0,4",
        L"Height=48",
        L"BorderBrush:=$fakeShadow"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=$acrylic",
        L"CornerRadius=4",
        L"BorderThickness=0,1,0,0",
        L"BorderBrush:=$button",
        L"BackgroundSizing=1"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"Canvas.ZIndex=-1",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl", {
        L"Margin=79,39,79,10"}},
    ThemeTargetStyles{L"Border#ContentBorder", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"ItemsWrapGrid > GridViewItem > Border > Grid > ContentPresenter > ContentControl > Grid > TextBlock", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#LogoBackgroundPlate", {
        L"CornerRadius=2"}},
    ThemeTargetStyles{L"TextBlock#AppDisplayName", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Grid#WebViewGrid", {
        L"Background=Transparent",
        L"Margin=0"}},
    ThemeTargetStyles{L"Button#Header > Border#Border > TextBlock#Text", {
        L"FontWeight=600",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Grid#QueryFormulationRoot", {
        L"CornerRadius=0",
        L"BorderThickness=0",
        L"Margin=0,31,0,0",
        L"Background:=$buttonPress"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Rectangle", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Margin=0,51,0,-64",
        L"Padding=0,1,0,1",
        L"BorderThickness=0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl > Grid#RootGrid", {
        L"CornerRadius=3",
        L"Margin=0,1,0,0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"FontIcon#SearchBoxOnTaskbarSearchGlyph", {
        L"Visibility=0",
        L"FontFamily=Segoe MDL2 Assets",
        L"Glyph=\uE721",
        L"Margin=16,0,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#SearchIconPlayer", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage", {
        L"Margin=-28,42,-28,0",
        L"MaxWidth=790",
        L"Width=790",
        L"Height=708",
        L"VerticalAlignment=2"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid#RootGrid@SearchBoxInputStates > Border#TaskbarSearchBackground", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorInputActive}\"/>"}},
    ThemeTargetStyles{L"Image#SearchIconOn", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"FontIcon#SearchGlyph", {
        L"Visibility=0",
        L"Glyph=\uE721"}},
    ThemeTargetStyles{L"Image#SearchIconOff", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"FontIcon", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"CornerRadius=4",
        L"BorderThickness=0,1,0,0",
        L"BorderBrush:=$button",
        L"Background:=$acrylicMenu",
        L"BackgroundSizing=1"}},
    ThemeTargetStyles{L"TextBlock#DisplayName", {
        L"Margin=0,6,0,-16",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"TextBlock#AllAppsHeading", {
        L"Margin=17,0,0,0",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"TextBlock#TopLevelSuggestionsListHeaderText", {
        L"Margin=80,25,0,0",
        L"Text=Recent",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"GridView#RecommendedList > Border > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem > Border#ContentBorder > Grid#DroppedFlickerWorkaroundWrapper", {
        L"Margin=29,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView#PowerButton", {
        L"Margin=-121,-1230,0,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid > ContentPresenter > Grid > FontIcon", {
        L"FontFamily=Segoe MDL2 Assets",
        L"Glyph=\uE720"}},
    ThemeTargetStyles{L"MenuFlyoutItem", {
        L"CornerRadius=0",
        L"Margin=-4,-2,-4,-2"}},
    ThemeTargetStyles{L"TextBlock#Title", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"TextBlock#Subtitle", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"GridViewItem > Border#ContentBorder > Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"FocusVisualPrimaryThickness=0",
        L"FocusVisualSecondaryThickness=0"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListView > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsStackPanel > ListViewItem", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"MenuFlyoutSubItem", {
        L"CornerRadius=0",
        L"Margin=-4,0,-4,0",
        L"Padding=11,4,11,5"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Rectangle", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartMenu.StartBlendedFlexFrame > Grid#FrameRoot", {
        L"Height=708"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Grid", {
        L"Margin=0,0,0,-40"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton", {
        L"Margin=79,14,79,0",
        L"CornerRadius=4",
        L"Height=48"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton > Grid@CommonStates > Border#BorderElement", {
        L"Background:=$button",
        L"Background@PointerOver:=$buttonHover",
        L"Background@Pressed:=$buttonPress",
        L"Background@Checked:=$button",
        L"Background@CheckedPointerOver:=$buttonHover",
        L"Background@CheckedPressed:=$buttonPress",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton > Grid", {
        L"BorderThickness=0,0,0,4",
        L"BorderBrush:=$fakeShadow",
        L"CornerRadius=0,0,4,4"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"Text=Search the web and your devices",
        L"Foreground:=$textSecondary",
        L"FontFamily=Segoe UI",
        L"Opacity=1"}},
    ThemeTargetStyles{L"Rectangle#TextCaret", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"MaxWidth=766",
        L"Width=766"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer", {
        L"Margin=0,51,0,0"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button > Grid > ContentPresenter > StackPanel > FontIcon", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TextBlock#ShowMorePinnedButtonText", {
        L"Text=Show all",
        L"FontFamily=Segoe UI",
        L"Margin=0",
        L"FontSize=12",
        L"Padding=5,0,5,0"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button", {
        L"Margin=-79,-2,79,0"}},
    ThemeTargetStyles{L"Frame#StartFrame", {
        L"Margin=0,0,0,-65"}},
    ThemeTargetStyles{L"GridView#AllAppsGrid > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid", {
        L"Margin=78,8,78,0"}},
    ThemeTargetStyles{L"Grid#AllListHeading", {
        L"BorderThickness=0,1,0,0",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\".5\"/>",
        L"Margin=0,25,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton#ViewSelectionButton > Grid@CommonStates", {
        L"Background:=$button",
        L"Background@PointerOver:=$buttonHover",
        L"Background@Pressed:=$buttonPress",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton#ViewSelectionButton", {
        L"Height=23",
        L"Margin=0,25,81,0",
        L"Padding=10,0,10,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton#ViewSelectionButton > Grid > ContentPresenter > TextBlock", {
        L"FontSize=12",
        L"Margin=0,1,0,0",
        L"Padding=5,0,5,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton#ViewSelectionButton > Grid > Microsoft.UI.Xaml.Controls.AnimatedIcon#ChevronIcon", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton#ViewSelectionButton > Grid > ContentPresenter", {
        L"Height=24",
        L"Margin=0,-2,0,0",
        L"Padding=0,0,0,2"}},
    ThemeTargetStyles{L"TextBlock#AllListHeadingText", {
        L"Margin=81,25,0,0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList > Grid#Root", {
        L"Padding=0,6,0,-6",
        L"MaxWidth=760"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList > Grid#Root > GridView#PinnedList > Border > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem", {
        L"Margin=0,0,31,0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"Margin=79,0,0,0"}},
    ThemeTargetStyles{L"GridViewHeaderItem", {
        L"Padding=0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList > Grid#Root > GridView#PinnedList > Border > ScrollViewer#ScrollViewer", {
        L"VerticalScrollBarVisibility=Auto",
        L"ScrollViewer.VerticalScrollMode=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion", {
        L"Margin=-60,10,0,0",
        L"Height=40",
        L"Width=40"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion > Border > ContentPresenter", {
        L"Height=40",
        L"Width=40"}},
}, {
    L"lightAccent=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark1}\"/>",
    L"lightAccentHover=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark1}\" Opacity=\".9\"/>",
    L"lightAccentPress=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark1}\" Opacity=\".8\"/>",
    L"darkAccent=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
    L"darkAccentHover=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\".9\"/>",
    L"darkAccentPress=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\".8\"/>",
    L"subtleButtonHover=<SolidColorBrush Color=\"{ThemeResource SubtleFillColorSecondary}\"/>",
    L"subtleButtonPress=<SolidColorBrush Color=\"{ThemeResource SubtleFillColorTertiary}\"/>",
    L"button=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\"/>",
    L"buttonHover=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\"/>",
    L"buttonPress=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\"/>",
    L"textPrimary=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\"/>",
    L"textSecondary=<SolidColorBrush Color=\"{ThemeResource TextFillColorSecondary}\"/>",
    L"textDisabled=<SolidColorBrush Color=\"{ThemeResource TextFillColorDisabled}\"/>",
    L"textInverse=<SolidColorBrush Color=\"{ThemeResource TextFillColorInverse}\"/>",
    L"acrylic=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\".0\" TintLuminosityOpacity=\".86\"/>",
    L"fakeShadow=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#10000000\" Offset=\"0.84\" /><GradientStop Color=\"#26000000\" Offset=\"0.85\" /><GradientStop Color=\"#00000000\" Offset=\"1.0\" /></LinearGradientBrush>",
    L"acrylicMenu=<AcrylicBrush TintColor=\"{ThemeResource LayerOnMicaBaseAltFillColorTertiary}\" FallbackColor=\"{ThemeResource SystemChromeHighColor}\" TintOpacity=\".0\" TintLuminosityOpacity=\".75\"/>",
}, {}, {
    ThemeTargetStyles{L"#qfPreviewPane, #qfPreviewPane *, .leftPill::before, #temporaryMessages, .scope-with-background__backButton, #gr11, #pp_Share, #pp_Review, #chatButtonRight, .curatedSettingsGroup, .scope-with-background__rightCaret, #topHitHeader, .userProfileMenuIcon, .scope-tile__button, .additionalInfoText.annotation, #root:not(.zeroInput19H1):not(.fileExplorer) .topResults .openPreviewPaneBtn .iconContent, .openPreviewIcon .iconContent.cortanaFontIcon, #scopesHeader, #scopesHeader *, #gr36, div[data-region=\"TopApps\"], #gr43, .openPreviewPaneBtn, .suggContainer.largerSearchIcon14 .secondaryText", {
        L"display: none !important",
        L"visibility: hidden !important"}},
    ThemeTargetStyles{L"#qfContainer", {
        L"max-width: 100% !important",
        L"margin-inline: 75px !important",
        L"margin-top: 7px !important"}},
    ThemeTargetStyles{L".cortanaFontIcon, .iconContent", {
        L"font-family: 'Segoe MDL2 Assets' !important"}},
    ThemeTargetStyles{L".leftPill", {
        L"border-left: 3px solid var(--accent11) !important",
        L"border-radius: 2px !important"}},
    ThemeTargetStyles{L".darkTheme .leftPill", {
        L"border-left: 3px solid var(--accent12) !important"}},
    ThemeTargetStyles{L"*", {
        L"scrollbar-width: none !important",
        L"border-color: transparent !important",
        L"cursor: default !important"}},
    ThemeTargetStyles{L".groupContainer.topItemsGroup", {
        L"order: -1 !important"}},
    ThemeTargetStyles{L".leftPaneZIsuggestions", {
        L"margin-left: -19px !important"}},
    ThemeTargetStyles{L"#root.win11.zeroInput19H1:not(.fileExplorer) .groupContainer:not(.curatedSettingsGroup) .suggestion.selectable:not(.focusable), .suggContainer", {
        L"border-radius: 2px !important",
        L"transition: all 83ms ease-out"}},
    ThemeTargetStyles{L"div[data-region=\"MRUHistory\"] > .suggsList, div[data-region=\"MRUHistory\"] .suggContainer", {
        L"width: 600px !important"}},
    ThemeTargetStyles{L".suggestion:not(.groupHeader)", {
        L"border-radius: 4px !important",
        L"clip-path: inset(1px 0px 1px 3px round 4px 2px 2px 4px) !important"}},
    ThemeTargetStyles{L".suggestion[aria-selected=\"true\"] .iconContainer, .suggestion[aria-selected=\"true\"] .details", {
        L"margin-left: -3px !important"}},
    ThemeTargetStyles{L".groupHeader", {
        L"margin-inline: 3px 4px !important"}},
    ThemeTargetStyles{L".topResults .suggDetailsContainer", {
        L"min-height: 0px !important"}},
    ThemeTargetStyles{L".suggDetailsContainer.limitScaleRange", {
        L"background: transparent !important"}},
    ThemeTargetStyles{L".topResults .suggDetailsContainer .primaryText", {
        L"margin-bottom: -2px !important"}},
}};

const Theme g_themeWindows10X_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Button#CloseAllAppsButton", {
        L"Margin=0,0,16,0",
        L"Padding=16,3,16,3.5",
        L"CornerRadius=12",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Margin=0,-23,-180,0",
        L"BorderThickness=0,1,0,0",
        L"BorderBrush:=$separator"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton", {
        L"Margin=0,0,80,0",
        L"CornerRadius=12",
        L"BorderThickness=0",
        L"Padding=16,3,16,3.5"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Margin=80,5,80,46",
        L"Height=48"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=$acrylic",
        L"CornerRadius=4",
        L"BorderThickness=0,1,0,0",
        L"BorderBrush:=$button",
        L"BackgroundSizing=1"}},
    ThemeTargetStyles{L"Grid#MainContent", {
        L"CornerRadius=3",
        L"Margin=0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=252",
        L"Margin=24,0,0,0",
        L"Width=610"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Margin=16,0,0,0",
        L"Text=My apps and websites",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"CornerRadius=4,4,6,6",
        L"Margin=80,36,80,1",
        L"BorderThickness=0,0,0,4",
        L"Height=48",
        L"BorderBrush:=$fakeShadow"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=$acrylic",
        L"CornerRadius=4",
        L"BorderThickness=0,1,0,0",
        L"BorderBrush:=$button",
        L"BackgroundSizing=1"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"Canvas.ZIndex=-1",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl", {
        L"Margin=79,39,79,10"}},
    ThemeTargetStyles{L"Border#ContentBorder", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"TextBlock#StatusMessage", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#LogoBackgroundPlate", {
        L"CornerRadius=2"}},
    ThemeTargetStyles{L"TextBlock#AppDisplayName", {
        L"Margin=-4,0,0,0",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Grid#WebViewGrid", {
        L"Background=Transparent",
        L"Margin=0"}},
    ThemeTargetStyles{L"Border#DropShadow", {
        L"Canvas.ZIndex=-1",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Button#Header > Border#Border > TextBlock#Text", {
        L"FontWeight=600",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Grid#QueryFormulationRoot", {
        L"CornerRadius=0",
        L"BorderThickness=0",
        L"Margin=0,31,0,0",
        L"Background:=$buttonPress"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"Text=Search the web or your devices",
        L"Margin=0",
        L"Foreground:=$textSecondary",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"TextBlock#ShowAllAppsButtonText", {
        L"Margin=0",
        L"Text=Show all",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton > ContentPresenter > StackPanel > TextBlock", {
        L"Margin=8,-1,0,0",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton > ContentPresenter > StackPanel > FontIcon > Grid > TextBlock", {
        L"Margin=-2,0,0,0",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Rectangle", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Margin=0,115,0,-64",
        L"Padding=0,1,0,1",
        L"BorderThickness=0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.RichSearchBoxControl#SearchBoxControl > Grid#RootGrid", {
        L"CornerRadius=3",
        L"Margin=0,1,0,0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"FontIcon#SearchBoxOnTaskbarSearchGlyph", {
        L"Visibility=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#SearchIconPlayer", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid@CommonStates", {
        L"CornerRadius=0,0,5,5",
        L"BorderThickness=0,0,0,4",
        L"BorderBrush:=$fakeShadow",
        L"Background:=transparent"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid@CommonStates > Border#BorderElement", {
        L"CornerRadius=3",
        L"Margin=-1,0,-1,0",
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid@CommonStates > FontIcon > Grid > TextBlock", {
        L"Margin=-1,0,0,1",
        L"FontFamily=Segoe MDL2 Assets",
        L"Text=\uE721",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\"/>"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage", {
        L"Margin=-28,42,-28,0",
        L"MaxWidth=790",
        L"Width=790",
        L"Height=708",
        L"VerticalAlignment=2"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid#RootGrid@SearchBoxInputStates > Border#TaskbarSearchBackground", {
        L"Background:=$inputActive"}},
    ThemeTargetStyles{L"Image#SearchIconOn", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"FontIcon#SearchGlyph", {
        L"Visibility=0"}},
    ThemeTargetStyles{L"Image#SearchIconOff", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"FontIcon#SearchBoxOnTaskbarSearchGlyph", {
        L"FontFamily=Segoe MDL2 Assets",
        L"Glyph=\uE721"}},
    ThemeTargetStyles{L"GridView#PinnedList > Border > ScrollViewer > Border#Root > Grid > ScrollContentPresenter > ItemsPresenter > GridViewItem > Border#ContentBorder > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter#ContentPresenter > Grid", {
        L"Height=84",
        L"Width=100"}},
    ThemeTargetStyles{L"GridView#PinnedList", {
        L"ScrollViewer.VerticalScrollMode=1"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton > ContentPresenter > StackPanel > FontIcon", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton > ContentPresenter > StackPanel > FontIcon", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton > ContentPresenter > StackPanel > TextBlock", {
        L"Margin=0",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"StartDocked.AllAppsPane#AllAppsPanel", {
        L"Margin=28,0,28,-65"}},
    ThemeTargetStyles{L"FontIcon", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"CornerRadius=4",
        L"BorderThickness=0,1,0,0",
        L"BorderBrush:=$button",
        L"BackgroundSizing=1"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MaxHeight=684",
        L"Height=684",
        L"MaxWidth=766",
        L"MinWidth=766"}},
    ThemeTargetStyles{L"TextBlock#DisplayName", {
        L"Margin=0,6,0,-16",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"TextBlock#AllAppsHeading", {
        L"Margin=17,0,0,0",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"TextBlock#TopLevelSuggestionsListHeaderText", {
        L"Margin=80,25,0,0",
        L"Text=Recent",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"GridView#RecommendedList > Border > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem > Border#ContentBorder > Grid#DroppedFlickerWorkaroundWrapper", {
        L"Margin=0"}},
    ThemeTargetStyles{L"GridView#RecommendedList", {
        L"Margin=77,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar#VerticalScrollBar", {
        L"Margin=0,0,42,0"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView#PowerButton", {
        L"Margin=-70,-1188,0,0"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid > ContentPresenter > Grid > FontIcon", {
        L"FontFamily=Segoe MDL2 Assets",
        L"Glyph=\uE720"}},
    ThemeTargetStyles{L"MenuFlyoutItem", {
        L"CornerRadius=0",
        L"Margin=-4,-2,-4,-2"}},
    ThemeTargetStyles{L"GridView#PinnedList > ItemsWrapGrid > GridViewItem > Border#ContentBorder > Grid#DroppedFlickerWorkaroundWrapper", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"TextBlock#Title", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"TextBlock#Subtitle", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"GridViewItem > Border#ContentBorder > Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"FocusVisualPrimaryThickness=0",
        L"FocusVisualSecondaryThickness=0"}},
    ThemeTargetStyles{L"JumpViewUI.JumpListListView > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsStackPanel > ListViewItem", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"MenuFlyoutSubItem", {
        L"CornerRadius=0",
        L"Margin=-4,0,-4,0",
        L"Padding=11,4,11,5"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid > Grid#RootContent > Grid#MainContent > Grid#InnerContent > Rectangle", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartDocked.LauncherFrame > Grid#RootPanel > Grid#RootGrid", {
        L"MinWidth=766"}},
    ThemeTargetStyles{L"Grid#UndockedRoot", {
        L"Height=553",
        L"Margin=0,0,0,-64"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid@CommonStates > Border#BorderElement", {
        L"Background:=$button",
        L"Background@PointerOver:=$buttonHover",
        L"Background@Pressed:=$buttonPress",
        L"Background@Checked:=$button",
        L"Background@CheckedPointerOver:=$buttonHover",
        L"Background@CheckedPressed:=$buttonPress",
        L"CornerRadius=4",
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid#NoTopLevelSuggestionsText", {
        L"Margin=152,0,0,0"}},
    ThemeTargetStyles{L"TextBlock#NoSuggestionsWithoutSettingsLink", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TextBlock#NoSuggestionsWithSettingsLink", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"GridView#PinnedList > Border > ScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem", {
        L"Margin=0,0,24,0"}},
    ThemeTargetStyles{L"StartMenu.PinnedList > Grid#Root", {
        L"Padding=5,0,0,0"}},
    ThemeTargetStyles{L"GridView#RecommendedList > Border > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem", {
        L"Margin=0"}},
    ThemeTargetStyles{L"GridView#PinnedList > Border > ScrollViewer > Border#Root > Grid > ScrollContentPresenter > ItemsPresenter", {
        L"MinHeight=340"}},
}, {
    L"accent=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\"/>",
    L"accentHover=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" Opacity=\".9\"/>",
    L"accentPress=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" Opacity=\".8\"/>",
    L"subtleButtonHover=<SolidColorBrush Color=\"{ThemeResource SubtleFillColorSecondary}\"/>",
    L"subtleButtonPress=<SolidColorBrush Color=\"{ThemeResource SubtleFillColorTertiary}\"/>",
    L"button=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\"/>",
    L"buttonHover=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\"/>",
    L"buttonPress=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\"/>",
    L"textPrimary=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\"/>",
    L"textSecondary=<SolidColorBrush Color=\"{ThemeResource TextFillColorSecondary}\"/>",
    L"textDisabled=<SolidColorBrush Color=\"{ThemeResource TextFillColorDisabled}\"/>",
    L"textInverse=<SolidColorBrush Color=\"{ThemeResource TextFillColorInverse}\"/>",
    L"acrylic=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"1\"/>",
    L"fakeShadow=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#10000000\" Offset=\"0.84\" /><GradientStop Color=\"#26000000\" Offset=\"0.85\" /><GradientStop Color=\"#00000000\" Offset=\"1.0\" /></LinearGradientBrush>",
    L"inputActive=<SolidColorBrush Color=\"{ThemeResource ControlFillColorInputActive}\"/>",
    L"separator=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\".5\"/>",
}, {}, {
    ThemeTargetStyles{L"#qfPreviewPane, #qfPreviewPane *, .leftPill::before, #temporaryMessages, .scope-with-background__backButton, #gr11, #pp_Share, #pp_Review, #chatButtonRight, .curatedSettingsGroup, .scope-with-background__rightCaret, #topHitHeader, .userProfileMenuIcon, .scope-tile__button, .additionalInfoText.annotation, #root:not(.zeroInput19H1):not(.fileExplorer) .topResults .openPreviewPaneBtn .iconContent, .openPreviewIcon .iconContent.cortanaFontIcon, #scopesHeader, #scopesHeader *, #gr36, div[data-region=\"TopApps\"], #gr43, .openPreviewPaneBtn, .suggContainer.largerSearchIcon14 .secondaryText", {
        L"display: none !important",
        L"visibility: hidden !important"}},
    ThemeTargetStyles{L"#qfContainer", {
        L"max-width: 100% !important",
        L"margin-inline: 75px !important",
        L"margin-top: 7px !important"}},
    ThemeTargetStyles{L".cortanaFontIcon, .iconContent", {
        L"font-family: 'Segoe MDL2 Assets' !important"}},
    ThemeTargetStyles{L".leftPill", {
        L"border-left: 3px solid var(--accent11) !important",
        L"border-radius: 2px !important"}},
    ThemeTargetStyles{L".darkTheme .leftPill", {
        L"border-left: 3px solid var(--accent12) !important"}},
    ThemeTargetStyles{L"*", {
        L"scrollbar-width: none !important",
        L"border-color: transparent !important",
        L"cursor: default !important"}},
    ThemeTargetStyles{L".groupContainer.topItemsGroup", {
        L"order: -1 !important"}},
    ThemeTargetStyles{L".leftPaneZIsuggestions", {
        L"margin-left: -19px !important"}},
    ThemeTargetStyles{L"#root.win11.zeroInput19H1:not(.fileExplorer) .groupContainer:not(.curatedSettingsGroup) .suggestion.selectable:not(.focusable), .suggContainer", {
        L"border-radius: 2px !important",
        L"transition: all 83ms ease-out"}},
    ThemeTargetStyles{L"div[data-region=\"MRUHistory\"] > .suggsList, div[data-region=\"MRUHistory\"] .suggContainer", {
        L"width: 578px !important"}},
    ThemeTargetStyles{L".suggestion:not(.groupHeader)", {
        L"border-radius: 4px !important",
        L"clip-path: inset(1px 0px 1px 3px round 4px 2px 2px 4px) !important"}},
    ThemeTargetStyles{L".suggestion[aria-selected=\"true\"] .iconContainer, .suggestion[aria-selected=\"true\"] .details", {
        L"margin-left: -3px !important"}},
    ThemeTargetStyles{L".groupHeader", {
        L"margin-inline: 3px 4px !important"}},
    ThemeTargetStyles{L".topResults .suggDetailsContainer", {
        L"min-height: 0px !important"}},
    ThemeTargetStyles{L".suggDetailsContainer.limitScaleRange", {
        L"background: transparent !important"}},
    ThemeTargetStyles{L".topResults .suggDetailsContainer .primaryText", {
        L"margin-bottom: -2px !important"}},
}};

const Theme g_themeTintedGlass = {{
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#BorderElement", {
        L"Background:=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#1AFFFFFF\"/>",
        L"BorderThickness=0",
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#22000000\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#AccentAppBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>",
        L"BorderThickness=0",
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton > ContentPresenter@CommonStates", {
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15C0C0C0\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > Border#BorderElement", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Grid@CommonStates > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid@CommonStates > Border", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.45\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1",
        L"Margin@Normal=4"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid@CommonStates > Border", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.45\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1",
        L"Margin@Normal=4"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#22000000\"/>"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"CornerRadius=14",
        L"Margin=-1"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"CornerRadius=14",
        L"Margin=-1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#Text", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid#RootGrid", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.6\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Button > Grid@CommonStates > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.6\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"DropDownButton", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.2\"/>"}},
    ThemeTargetStyles{L"Button#Header > Border#Border@CommonStates", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.6\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"StartMenu.FolderModal > Grid > Border", {
        L"Background:=$CommonBgBrush",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"ListViewItem > Grid#ContentBorder@CommonStates", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.9\"/>",
        L"BorderThickness=1",
        L"CornerRadius=5"}},
}, {
    L"CommonBgBrush=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#80000000\"/>",
}};

const Theme g_themeTintedGlass_variant_ClassicStartMenu = {{
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#BorderElement", {
        L"Background:=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#1AFFFFFF\"/>",
        L"BorderThickness=0",
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=504"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#00000000\"/>",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#AccentAppBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0",
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>",
        L"BorderThickness=0",
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#ContentBorder@CommonStates > Grid#DroppedFlickerWorkaroundWrapper > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"0\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"Margin=1",
        L"Background@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@Pressed:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton > ContentPresenter@CommonStates", {
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid > Border#BorderElement", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#UserTileButton > Grid@CommonStates > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"0\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid@CommonStates > Border", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.45\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1",
        L"Margin@Normal=4"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneButton#PowerButton > Grid@CommonStates > Border", {
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.45\"/>",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.7\"/>",
        L"BorderThickness=1",
        L"Margin@Normal=4"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListViewItem > Grid@CommonStates > Border", {
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.55\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton > ContentPresenter@CommonStates", {
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"StartDocked.AllAppsZoomListViewItem > Grid@CommonStates > Border", {
        L"Background@Normal:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"0\" Opacity=\"0.2\"/>",
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.6\"/>"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"CornerRadius=14",
        L"Margin=-1"}},
    ThemeTargetStyles{L"Border#DropShadow", {
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#RootGridDropShadow", {
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"Border#RightCompanionDropShadow", {
        L"CornerRadius=14"}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListViewItem > Grid#ContentBorder@CommonStates", {
        L"Background@PointerOver:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15C0C0C0\"/>",
        L"CornerRadius=14"}},
}, {
    L"CommonBgBrush=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#80000000\"/>",
}};

const Theme g_themeLayerMicaUI = {{
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"CornerRadius=$OuterRadius",
        L"BorderThickness=1",
        L"Width=445"}},
    ThemeTargetStyles{L"Grid#MainMenu > Grid#MainContent > Border#AcrylicOverlay", {
        L"Margin=9,-3,9,-55",
        L"CornerRadius=8,8,10,10",
        L"BorderThickness=1",
        L"Background:=$ThemeOverlay"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView > StartDocked.NavigationPaneButton > Grid@CommonStates > Border", {
        L"CornerRadius=0,8,8,0",
        L"Margin=-2,-1,-2,-1",
        L"BorderThickness=1",
        L"Background@Normal:=$ThemeOverlay",
        L"Background@Pressed:=$ThemeBtn",
        L"Background@PointerOver:=$ThemeBtn",
        L"Background@Disabled:=$ThemeOverlay",
        L"BorderBrush:=$ThemeControlBorder"}},
    ThemeTargetStyles{L"StartDocked.UserTileView > StartDocked.NavigationPaneButton > Grid@CommonStates > Border", {
        L"Margin=0,-1,0,-1",
        L"CornerRadius=0",
        L"Background@Pressed:=$ThemeBtn",
        L"BorderThickness=0,1,0,1",
        L"Width=48",
        L"Background@PointerOver:=$ThemeBtn",
        L"Background@Normal:=$ThemeOverlay",
        L"Background@Disabled:=$ThemeOverlay",
        L"Background:=$ThemeOverlay",
        L"BorderBrush:=$ThemeControlBorder"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton > Grid@CommonStates > Border", {
        L"Background@Normal:=$ThemeOverlay",
        L"CornerRadius=8,0,0,8",
        L"BorderBrush:=$ThemeControlBorder",
        L"BorderThickness=1",
        L"Background@PointerOver:=$ThemeBtn",
        L"Background@Pressed:=$ThemeBtn",
        L"Background:=$ThemeOverlay"}},
    ThemeTargetStyles{L"Border#ContentBorder > Grid#DroppedFlickerWorkaroundWrapper > Border@CommonStates", {
        L"Margin=1"}},
    ThemeTargetStyles{L"StartMenu.FolderModal#StartFolderModal > Grid#Root > Border", {
        L"BorderThickness=1",
        L"Width=340",
        L"Height=350",
        L"Margin=5,0,0,0",
        L"BorderBrush:=$ThemeOutBorder",
        L"Background:=$ThemeFlyout"}},
    ThemeTargetStyles{L"Border#StartDropShadow", {
        L"Width=445",
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartMenu.StartMenuCompanion#RightCompanion > Grid#CompanionRoot > Border#AcrylicBorder", {
        L"Width=225",
        L"CornerRadius=0,$OuterRadius,$OuterRadius,0",
        L"BorderBrush:=$ThemeOutBorder",
        L"BorderThickness=0,1,1,1"}},
    ThemeTargetStyles{L"StartMenu.StartMenuCompanion#RightCompanion > Grid#CompanionRoot > Grid#MainContent > Border#AcrylicOverlay", {
        L"Margin=20,170,20,-2",
        L"BorderThickness=0,1,0,1",
        L"CornerRadius=0",
        L"Background:=Transparent",
        L"BorderBrush:=$ThemeControlBorder"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion > Border", {
        L"Background:=$ThemeOverlay",
        L"BorderBrush:=$ThemeControlBorder",
        L"BorderThickness=1",
        L"Background:=$ThemeOverlay"}},
    ThemeTargetStyles{L"Border > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > GridViewItem > Border#ContentBorder > Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList > Grid#Root > GridView#PinnedList > Border", {
        L"Padding=0,5,0,10",
        L"BorderThickness=0,0,0,1",
        L"BorderBrush:=$ThemeControlBorder",
        L"Margin=0,0,0,-25"}},
    ThemeTargetStyles{L"ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsPresenter > ItemsStackPanel > ListViewItem > Grid#ContentBorder > Border#BorderBackground", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"GridViewHeaderItem > Border > ContentPresenter#ContentPresenter > Button#Header > Border#Border", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"ScrollContentPresenter > Border", {
        L"MaxHeight=665",
        L"VerticalAlignment=Bottom",
        L"MinWidth=700"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid#RootGrid > Grid#OuterBorderGrid > Grid#BorderGrid > Border#LayerBorder", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid#RootGrid > Grid#OuterBorderGrid > Grid#BorderGrid > Border#AppBorder", {
        L"BorderBrush:=$ThemeOutBorder",
        L"CornerRadius=$OuterRadius",
        L"Margin=1"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=$ThemeFlyout",
        L"BorderBrush:=$ThemeOutBorder"}},
    ThemeTargetStyles{L"Button#ZoomInButton > Grid > Border#BackgroundBorder", {
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"ListView#ZoomedOutListView > Border > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > ListViewItem > Grid#ContentBorder > Border#BorderBackground", {
        L"CornerRadius=$InnerRadius",
        L"BorderBrush:=$ThemeControlBorder"}},
    ThemeTargetStyles{L"Border#RightCompanionDropShadowDismissTarget", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#RightCompanionDropShadow", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#LeftCompanionDropShadowDismissTarget", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#DropShadowDismissTarget", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid", {
        L"RenderTransform:=<TranslateTransform X=\"3\" Y=\"-8\" />",
        L"Canvas.ZIndex=99"}},
    ThemeTargetStyles{L"StartMenu.StartMenuCompanion#RightCompanion > Grid#CompanionRoot > Grid#MainContent > ContentPresenter#PrimaryCardContainer", {
        L"Margin=0,0,0,-8"}},
    ThemeTargetStyles{L"Grid#CommandSpace > Button#PrimaryButton > ContentPresenter#ContentPresenter", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Grid#CommandSpace > Button#SecondaryButton > Button#CloseButton > ContentPresenter#ContentPresenter", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion > Border > ContentPresenter#ContentPresenter", {
        L"Height=42",
        L"Width=45",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.CortanaRichSearchBox#SearchTextBox", {
        L"FontFamily=$ThFnt",
        L"FontSize=14",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Frame#StartFrame", {
        L"Margin=0,-10,0,0"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#GridViewContainer", {
        L"Width=360",
        L"Margin=5,0,-5,0"}},
    ThemeTargetStyles{L"Grid#FrameRoot", {
        L"Height=665"}},
    ThemeTargetStyles{L"Grid#MainMenu", {
        L"Width=445",
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > StartMenu.StartBlendedFlexFrame > Grid#FrameRoot", {
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid#RightCompanionContainerGrid", {
        L"Width=221",
        L"Padding=-10,0,0,0",
        L"RenderTransform:=<TranslateTransform X=\"-9\" />"}},
    ThemeTargetStyles{L"Grid#CompanionRoot", {
        L"Width=225"}},
    ThemeTargetStyles{L"StartMenu.StartMenuCompanion#RightCompanion > Grid#CompanionRoot > Grid#MainContent > Grid#ActionsBar", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-3\"/>"}},
    ThemeTargetStyles{L"Grid#AllListHeading > Microsoft.UI.Xaml.Controls.DropDownButton#ViewSelectionButton > Grid#RootGrid", {
        L"CornerRadius=$InnerRadius",
        L"Margin=2,0,0,0"}},
    ThemeTargetStyles{L"Grid#NavPanePlaceholder", {
        L"Margin=52,-577,52,576"}},
    ThemeTargetStyles{L"Grid#AllListHeading", {
        L"Margin=0,80,-5,0"}},
    ThemeTargetStyles{L"Grid#NoTopLevelSuggestionsText", {
        L"Height=0",
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Height=0",
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot > Grid[2]", {
        L"MinHeight=0",
        L"Visibility=1"}},
    ThemeTargetStyles{L"GridView#PinnedList", {
        L"Margin=9,0,-9,-60"}},
    ThemeTargetStyles{L"GridView#RecommendedList", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"Margin=0,-30,0,0",
        L"Padding=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager#PipsPager", {
        L"Margin=-30,-10,0,10"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Margin=0,0,-44,0"}},
    ThemeTargetStyles{L"Cortana.UI.Views.TaskbarSearchPage > Grid#RootGrid > Grid#QueryFormulationRoot", {
        L"Margin=0,-4,0,10"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton#SearchBoxToggleButton", {
        L"Height=42",
        L"Margin=-23,-2,72,0"}},
    ThemeTargetStyles{L"StartMenu.StartHome", {
        L"Width=450",
        L"Margin=-15,8,5,-53"}},
    ThemeTargetStyles{L"TextBlock#ZoomedOutHeading", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TextBlock#DisplayName", {
        L"Margin=8,3,8,0",
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt",
        L"Margin=62,6,0,-6"}},
    ThemeTargetStyles{L"TextBlock#AllListHeadingText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt"}},
    ThemeTargetStyles{L"TextBlock#UserTileNameText", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartMenu.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"FontFamily=$ThFnt",
        L"FontSize=14",
        L"FontWeight=$ThFntWt",
        L"RenderTransform:=<TranslateTransform Y=\"1.5\" />"}},
    ThemeTargetStyles{L"TextBlock#AppDisplayName", {
        L"FontFamily=$ThFnt",
        L"FontSize=12.5",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Button#Header > Border > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontSize=14",
        L"FontWeight=$ThHdnWt"}},
    ThemeTargetStyles{L"TextBlock#PlaceholderTextContentPresenter", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton > Grid > ContentPresenter > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontSize=14",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#ShowMorePinnedButtonText", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"ItemsWrapGrid > GridViewItem > Border#ContentBorder > Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter#ContentPresenter > ContentControl > Grid > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontSize=12.5",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock[FontSize=12]", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=12.5"}},
    ThemeTargetStyles{L"TextBlock[FontSize=14]", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock[FontSize=18]", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=13"}},
    ThemeTargetStyles{L"ContentPresenter#PrimaryCardContainer > Grid > Grid > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt",
        L"FontSize=15"}},
    ThemeTargetStyles{L"ContentPresenter > TextBlock#FolderNameTextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt"}},
    ThemeTargetStyles{L"MenuFlyoutItem > Grid#LayoutRoot > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=14.5",
        L"RenderTransform:=<TranslateTransform X=\"1\" Y=\"0.5\" />"}},
    ThemeTargetStyles{L"Button#ZoomInButton > Grid > ContentPresenter#ContentPresenter > StackPanel > TextBlock", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"ListView#ZoomedOutListView > Border > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsPresenter > ItemsWrapGrid > ListViewItem > Grid#ContentBorder > ContentPresenter#ContentPresenter > Viewbox > Border > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"Grid#DroppedFlickerWorkaroundWrapper > ContentPresenter#ContentPresenter > Grid > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#SeeAllButtonLabelTextblock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#FolderNameTextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt"}},
    ThemeTargetStyles{L"TextBox#MutableFolderNameTextBox", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt"}},
    ThemeTargetStyles{L"TextBox#MutableFolderNameTextBox > Grid > ScrollViewer#ContentElement > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Windows.UI.Xaml.Internal.TextBoxView", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#ShowHideCompanion", {
        L"Margin=-73,-1,73,1",
        L"Height=42",
        L"Width=45"}},
    ThemeTargetStyles{L"ToolTip", {
        L"CornerRadius=$OuterRadius",
        L"BorderBrush:=$ThemeOutBorder",
        L"Background:=$ThemeFlyout"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Margin=286,0,-23,0"}},
    ThemeTargetStyles{L"TextBlock#SubFolderNameTextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThHdnWt"}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid > Button", {
        L"RenderTransform:=<TranslateTransform X=\"5\" />"}},
}, {
    L"ThemeBorder=<SolidColorBrush Color=\"{ThemeResource Border}\" />",
    L"OuterRadius=10",
    L"InnerRadius=8",
    L"ThFnt=Nunito",
    L"ThemeOverlay=<SolidColorBrush Color=\"{ThemeResource Overlay}\" />",
    L"ThFntWt=Normal",
    L"ThHdnWt=Semibold",
    L"ThemeBtn=<SolidColorBrush Color=\"{ThemeResource Btn}\"/>",
    L"ThemeControlBorder=<SolidColorBrush Color=\"{ThemeResource ControlBorder}\" />",
    L"ThemeOutBorder=<SolidColorBrush Color=\"#66757575\"/>",
    L"ThemeFlyout=<AcrylicBrush BackgroundSource=\"Backdrop\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.1\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
}, {
    L"Overlay@Light=#40FFFFFF",
    L"Overlay@Dark=#09FFFFFF",
    L"Border@Light=#0F000000",
    L"Border@Dark=#19000000",
    L"ControlBorder@Light=#0F000000",
    L"ControlBorder@Dark=#15FFFFFF",
    L"Btn@Light=#90FFFFFF",
    L"Btn@Dark=#20FFFFFF",
}, {
    ThemeTargetStyles{L".curatedSettingsGroup, #scopesHeader", {
        L"display: none !important"}},
    ThemeTargetStyles{L"#qfPreviewPane", {
        L"margin-right: -10px !important",
        L"border-radius: 8px !important"}},
    ThemeTargetStyles{L".suggsList, .suggContainer", {
        L"margin-right: 5px !important",
        L"margin-left: 0px !important"}},
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
#include <cmath>
#include <list>
#include <mutex>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
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
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuxh = wux::Hosting;
namespace awge = ABI::Windows::Graphics::Effects;

enum class Target {
    StartMenu,
    SearchHost,
};

Target g_target;

bool g_isRedesignedStartMenu;

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
    std::optional<float> tintLuminosityOpacity;
    std::optional<float> tintSaturation;
    std::optional<float> noiseOpacity;
    std::optional<float> noiseDensity;
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

std::vector<ElementCustomizationRules> g_elementsCustomizationRules;

struct ElementPropertyCustomizationState {
    std::optional<winrt::Windows::Foundation::IInspectable> originalValue;
    std::optional<PropertyOverrideValue> customValue;
    winrt::Windows::Foundation::IInspectable lastAppliedValue{nullptr};
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

std::wstring g_webContentCss;
std::wstring g_webContentJs;

struct WebViewCustomizationState {
    winrt::weak_ref<FrameworkElement> element;
    bool isWebView2 = false;
    winrt::event_token navigationCompletedEventToken;
};

std::unordered_map<InstanceHandle, WebViewCustomizationState>
    g_webViewsCustomizationState;

bool g_elementPropertyModifying;

winrt::Windows::Foundation::IAsyncOperation<bool>
    g_delayedAllAppsRootVisibilitySet;

enum class DisableNewStartMenuLayout {
    windowsDefault,
    disableNewLayoutAndPhoneLink,
    disableNewLayoutKeepPhoneLink,
    forceNewLayout,
};

DisableNewStartMenuLayout g_disableNewStartMenuLayout;

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

std::vector<ResourceVariableEntry> g_resourceVariables;

// Track original resource values for restoration (per-thread since
// Application::Current().Resources() is per-thread).
std::unordered_map<std::wstring, winrt::Windows::Foundation::IInspectable>
    g_originalResourceValues;

// Track our merged theme dictionary for cleanup (per-thread).
ResourceDictionary g_resourceVariablesThemeDict{nullptr};

// For listening to theme color changes (per-thread).
winrt::Windows::UI::ViewManagement::UISettings g_uiSettings{nullptr};
thread_local winrt::event_token g_colorValuesChangedToken;

winrt::Windows::Foundation::IInspectable ReadLocalValueWithWorkaround(
    DependencyObject elementDo,
    DependencyProperty property) {
    // Workaround for AcrylicBrushes returning an incorrect background brush.
    // When restored, it doesn't look correct.
    bool getValueWorkaround = false;
    if (property == Controls::Border::BackgroundProperty()) {
        auto border = elementDo.try_as<Controls::Border>();
        if (border && border.Name() == L"AcrylicBorder") {
            Wh_Log(L"Using GetValue workaround for AcrylicBorder background");
            getValueWorkaround = true;
        }
    } else {
        // The properties below return null from ReadLocalValue for some reason.
        struct {
            DependencyProperty property;
            std::wstring_view elementType;
            std::wstring_view elementName;
        } propertiesForWorkaround[] = {
            {FrameworkElement::MaxHeightProperty(),
             L"Windows.UI.Xaml.Controls.Grid", L"FrameRoot"},
            {FrameworkElement::WidthProperty(),
             L"Windows.UI.Xaml.Controls.Grid", L"MainMenu"},
            {FrameworkElement::WidthProperty(),
             L"Windows.UI.Xaml.Controls.GridView", L"PinnedList"},
        };

        auto element = elementDo.try_as<FrameworkElement>();
        if (element) {
            auto elementName = element.Name();
            auto elementClassName = winrt::get_class_name(element);
            for (const auto& [prop, type, name] : propertiesForWorkaround) {
                if (property == prop && elementName == name &&
                    elementClassName == type) {
                    Wh_Log(L"Using GetValue workaround for %.*s",
                           static_cast<int>(name.length()), name.data());
                    getValueWorkaround = true;
                    break;
                }
            }
        }
    }

    auto value = getValueWorkaround ? elementDo.GetValue(property)
                                    : elementDo.ReadLocalValue(property);
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
class XamlBlurBrush : public wux::Media::XamlCompositionBrushBaseT<XamlBlurBrush>
{
public:
    XamlBlurBrush(wuc::Compositor compositor,
                  float blurAmount,
                  winrt::Windows::UI::Color tint,
                  std::optional<uint8_t> tintOpacity,
                  winrt::hstring tintThemeResourceKey,
                  std::optional<float> tintLuminosityOpacity,
                  std::optional<float> tintSaturation,
                  std::optional<float> noiseOpacity,
                  std::optional<float> noiseDensity);

    void OnConnected();
    void OnDisconnected();

private:
    void RefreshThemeTint();
    void OnThemeRefreshed();

    wuc::Compositor m_compositor;
    float m_blurAmount;
    winrt::Windows::UI::Color m_tint;
    std::optional<uint8_t> m_tintOpacity;
    winrt::hstring m_tintThemeResourceKey;
    std::optional<float> m_tintLuminosityOpacity;
    std::optional<float> m_tintSaturation;
    std::optional<float> m_noiseOpacity;
    std::optional<float> m_noiseDensity;
    winrt::Windows::UI::ViewManagement::UISettings m_uiSettings;
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
XamlBlurBrush::XamlBlurBrush(wuc::Compositor compositor,
                             float blurAmount,
                             winrt::Windows::UI::Color tint,
                             std::optional<uint8_t> tintOpacity,
                             winrt::hstring tintThemeResourceKey,
                             std::optional<float> tintLuminosityOpacity,
                             std::optional<float> tintSaturation,
                             std::optional<float> noiseOpacity,
                             std::optional<float> noiseDensity) :
    m_compositor(std::move(compositor)),
    m_blurAmount(blurAmount),
    m_tint(tint),
    m_tintOpacity(tintOpacity),
    m_tintThemeResourceKey(std::move(tintThemeResourceKey)),
    m_tintLuminosityOpacity(tintLuminosityOpacity),
    m_tintSaturation(tintSaturation),
    m_noiseOpacity(noiseOpacity),
    m_noiseDensity(noiseDensity)
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
                wux::Media::LoadedImageSurface::StartLoadFromStream(stream);
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

        auto factory = m_compositor.CreateEffectFactory(
            *compositeEffect,
            // List of animatable properties.
            {L"FloodEffect.Color"}
        );
        auto brush = factory.CreateBrush();

        brush.SetSourceParameter(L"backdrop", backdropBrush);

        // Bind the noise brush if we created one.
        if (noiseBrush)
        {
            brush.SetSourceParameter(L"NoiseSource", noiseBrush);
        }

        CompositionBrush(brush);
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
            auto compositor =
                wuxh::ElementCompositionPreview::GetElementVisual(uiElement)
                    .Compositor();

            value = winrt::make<XamlBlurBrush>(
                std::move(compositor), blurBrushParams->blurAmount,
                blurBrushParams->tint, blurBrushParams->tintOpacity,
                winrt::hstring(blurBrushParams->tintThemeResourceKey),
                blurBrushParams->tintLuminosityOpacity,
                blurBrushParams->tintSaturation, blurBrushParams->noiseOpacity,
                blurBrushParams->noiseDensity);
        } else {
            Wh_Log(L"Can't get UIElement for blur brush");
            return;
        }
    } else {
        Wh_Log(L"Unsupported override value");
        return;
    }

    // Below is a workaround to the following bug: If the AllAppsRoot Grid is
    // made visible too early, it becomes truncated such that only the part
    // that's visible without scrolling is accessible. The rest of the content
    // is blank. Delaying the property setting seems to fix it. See:
    // https://github.com/ramensoftware/windhawk-mods/issues/1335
    if (winrt::get_class_name(elementDo) == L"Windows.UI.Xaml.Controls.Grid" &&
        elementDo.as<FrameworkElement>().Name() == L"AllAppsRoot" &&
        property == UIElement::VisibilityProperty()) {
        if (value != DependencyProperty::UnsetValue() && initialApply &&
            !g_delayedAllAppsRootVisibilitySet) {
            Wh_Log(L"Delaying SetValue for AllAppsRoot");
            g_delayedAllAppsRootVisibilitySet =
                elementDo.Dispatcher().TryRunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                    [elementDo = std::move(elementDo),
                     property = std::move(property),
                     value = std::move(value)]() {
                        Wh_Log(L"Running delayed SetValue for AllAppsRoot");
                        g_elementPropertyModifying = true;
                        try {
                            elementDo.SetValue(property, value);
                        } catch (winrt::hresult_error const& ex) {
                            Wh_Log(L"Error %08X: %s", ex.code(),
                                   ex.message().c_str());
                        }
                        g_elementPropertyModifying = false;
                        g_delayedAllAppsRootVisibilitySet = nullptr;
                    });
            return;
        } else if (g_delayedAllAppsRootVisibilitySet) {
            Wh_Log(L"Canceling delayed SetValue for AllAppsRoot");
            g_delayedAllAppsRootVisibilitySet.Cancel();
            g_delayedAllAppsRootVisibilitySet = nullptr;
        }
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
    std::wstring tintThemeResourceKey;
    winrt::Windows::UI::Color tint{};
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
            SetOrClearValue(element, property, it->second,
                            /*initialApply=*/true);
            propertyCustomizationState.lastAppliedValue =
                ReadLocalValueWithWorkaround(element, property);
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
                            propertyCustomizationState.lastAppliedValue =
                                ReadLocalValueWithWorkaround(element, property);
                        } else {
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
        try {
            visualStateGroupIter.CurrentStateChanged(
                elementCustomizationStateForVisualStateGroup
                    .visualStateGroupCurrentStateChangedToken);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }
}

// winrt::WebView2Standalone::Controls::IWebView2
// 59C47E46-CC96-525F-A17C-2C213F988447
constexpr winrt::guid IID_WebView2Standalone_IWebView2{
    0x59C47E46,
    0xCC96,
    0x525F,
    {0xA1, 0x7C, 0x2C, 0x21, 0x3F, 0x98, 0x84, 0x47}};

// 4865E238-036A-5664-95A3-447EC44CF498
constexpr winrt::guid IID_ICoreWebView2NavigationCompletedEventArgs{
    0x4865E238,
    0x036A,
    0x5664,
    {0x95, 0xA3, 0x44, 0x7E, 0xC4, 0x4C, 0xF4, 0x98}};

// clang-format off
struct WebView2Standalone_IWebView2 : ::IInspectable {
    virtual int32_t __stdcall get_CoreWebView2(void**) noexcept = 0;
    virtual int32_t __stdcall get_CoreWebView2Controller(void**) noexcept = 0;
    virtual int32_t __stdcall EnsureCoreWebView2Async(void**) noexcept = 0;
    virtual int32_t __stdcall ExecuteScriptAsync(void*, void**) noexcept = 0;
    virtual int32_t __stdcall get_Source(void**) noexcept = 0;
    virtual int32_t __stdcall put_Source(void*) noexcept = 0;
    virtual int32_t __stdcall get_CanGoForward(bool*) noexcept = 0;
    virtual int32_t __stdcall put_CanGoForward(bool) noexcept = 0;
    virtual int32_t __stdcall get_CanGoBack(bool*) noexcept = 0;
    virtual int32_t __stdcall put_CanGoBack(bool) noexcept = 0;
    virtual int32_t __stdcall Reload() noexcept = 0;
    virtual int32_t __stdcall GoForward() noexcept = 0;
    virtual int32_t __stdcall GoBack() noexcept = 0;
    virtual int32_t __stdcall NavigateToString(void*) noexcept = 0;
    virtual int32_t __stdcall Close() noexcept = 0;
    virtual int32_t __stdcall add_NavigationCompleted(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_NavigationCompleted(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_WebMessageReceived(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_WebMessageReceived(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_NavigationStarting(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_NavigationStarting(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_CoreProcessFailed(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_CoreProcessFailed(winrt::event_token) noexcept = 0;
    virtual int32_t __stdcall add_CoreWebView2Initialized(void*, winrt::event_token*) noexcept = 0;
    virtual int32_t __stdcall remove_CoreWebView2Initialized(winrt::event_token) noexcept = 0;
};

struct ICoreWebView2NavigationCompletedEventArgs : ::IInspectable {
    virtual int32_t __stdcall get_IsSuccess(bool*) noexcept = 0;
    virtual int32_t __stdcall get_WebErrorStatus(int32_t*) noexcept = 0;
    virtual int32_t __stdcall get_NavigationId(uint64_t*) noexcept = 0;
};
// clang-format on

std::wstring EscapeJsTemplateString(std::wstring_view str) {
    std::wstring buffer;
    buffer.reserve(str.size());
    for (const auto c : str) {
        switch (c) {
            case '\\':
                buffer.append(L"\\\\");
                break;
            case '`':
                buffer.append(L"\\`");
                break;
            default:
                buffer.push_back(c);
                break;
        }
    }

    return buffer;
}

std::wstring CreateWebViewJsCodeForApply() {
    std::wstring jsCode =
        LR"(
        (() => {
        const styleElementId = "windhawk-windows-11-start-menu-styler-style";
        const styleContent = `
    )";

    jsCode += EscapeJsTemplateString(g_webContentCss);

    jsCode +=
        LR"(
        `;
        if (!document.getElementById(styleElementId)) {
            const style = document.createElement("style");
            style.id = styleElementId;
            style.textContent = styleContent;
            document.head.appendChild(style);
        }
    )";

    jsCode += g_webContentJs;

    jsCode +=
        LR"(
        })();
    )";

    Wh_Log(L"======================================== JS:");
    std::wstringstream ss(jsCode);
    std::wstring line;
    while (std::getline(ss, line, L'\n')) {
        Wh_Log(L"%s", line.c_str());
    }
    Wh_Log(L"========================================");

    return jsCode;
}

bool ApplyWebViewStyleCustomizations(Controls::WebView webViewElement) {
    auto source = webViewElement.Source();
    if (!source) {
        return false;
    }

    auto canonicalUri = source.AbsoluteCanonicalUri();
    Wh_Log(L"WebView source: %s", canonicalUri.c_str());

    if (canonicalUri != L"https://www.bing.com/WS/Init" &&
        // Offline content (DisableSearchBoxSuggestions registry option).
        canonicalUri !=
            L"ms-appx-web://microsoftwindows.client.cbs/Cortana.UI/cache/"
            L"svlocal/desktop/2.html" &&
        // Windows 10 (SearchApp.exe).
        canonicalUri !=
            L"https://www.bing.com/AS/API/WindowsCortanaPane/V2/Init" &&
        canonicalUri !=
            L"ms-appx-web://microsoft.windows.search/cache/local/desktop/"
            L"2.html") {
        return false;
    }

    std::wstring jsCode = CreateWebViewJsCodeForApply();

    webViewElement.InvokeScriptAsync(
        L"eval", winrt::single_threaded_vector<winrt::hstring>(
                     {winrt::hstring(jsCode.c_str(), jsCode.size())}));

    return true;
}

bool ApplyWebView2StyleCustomizations(
    WebView2Standalone_IWebView2* webViewElement) {
    void* sourcePtr;
    winrt::check_hresult(webViewElement->get_Source(&sourcePtr));
    auto source = winrt::Windows::Foundation::Uri{
        sourcePtr, winrt::take_ownership_from_abi};
    if (!source) {
        return false;
    }

    auto canonicalUri = source.AbsoluteCanonicalUri();
    Wh_Log(L"WebView source: %s", canonicalUri.c_str());

    if (canonicalUri != L"https://www.bing.com/WS/Init" &&
        // Offline content (DisableSearchBoxSuggestions registry option).
        canonicalUri !=
            L"https://searchapp.bundleassets.example/desktop/2.html") {
        return false;
    }

    std::wstring jsCode = CreateWebViewJsCodeForApply();

    void* operationPtr;
    auto jsCodeHstring = winrt::hstring(jsCode.c_str(), jsCode.size());
    winrt::check_hresult(webViewElement->ExecuteScriptAsync(
        *(void**)(&jsCodeHstring), &operationPtr));
    auto operation =
        winrt::Windows::Foundation::IAsyncOperation<winrt::hstring>{
            operationPtr, winrt::take_ownership_from_abi};

    return true;
}

void ApplyCustomizationsIfWebView(InstanceHandle handle,
                                  FrameworkElement element) {
    auto className = winrt::get_class_name(element);
    if (className == L"Windows.UI.Xaml.Controls.WebView") {
        auto& webViewCustomizationState = g_webViewsCustomizationState[handle];
        if (!webViewCustomizationState.element.get()) {
            webViewCustomizationState.element = element;

            auto webViewElement = element.as<Controls::WebView>();

            ApplyWebViewStyleCustomizations(webViewElement);

            webViewCustomizationState.navigationCompletedEventToken =
                webViewElement.NavigationCompleted(
                    [](const Controls::WebView& sender,
                       const Controls::WebViewNavigationCompletedEventArgs&
                           args) {
                        if (args.IsSuccess()) {
                            ApplyWebViewStyleCustomizations(sender);
                        }
                    });
        }
    } else if (className == L"WebView2Standalone.Controls.WebView2") {
        auto& webViewCustomizationState = g_webViewsCustomizationState[handle];
        if (!webViewCustomizationState.element.get()) {
            webViewCustomizationState.element = element;
            webViewCustomizationState.isWebView2 = true;

            winrt::com_ptr<WebView2Standalone_IWebView2> webViewElement;
            winrt::check_hresult(
                ((IUnknown*)winrt::get_abi(element))
                    ->QueryInterface(IID_WebView2Standalone_IWebView2,
                                     webViewElement.put_void()));

            ApplyWebView2StyleCustomizations(webViewElement.get());

            winrt::Windows::Foundation::TypedEventHandler<
                winrt::Windows::Foundation::IInspectable,
                winrt::Windows::Foundation::IInspectable>
                eventHandler = [](const winrt::Windows::Foundation::
                                      IInspectable& sender,
                                  const winrt::Windows::Foundation::
                                      IInspectable& args) {
                    winrt::com_ptr<ICoreWebView2NavigationCompletedEventArgs>
                        webViewArgs;
                    winrt::check_hresult(
                        ((IUnknown*)winrt::get_abi(args))
                            ->QueryInterface(
                                IID_ICoreWebView2NavigationCompletedEventArgs,
                                webViewArgs.put_void()));

                    bool success;
                    winrt::check_hresult(webViewArgs->get_IsSuccess(&success));
                    if (!success) {
                        return;
                    }

                    winrt::com_ptr<WebView2Standalone_IWebView2> webViewElement;
                    winrt::check_hresult(
                        ((IUnknown*)winrt::get_abi(sender))
                            ->QueryInterface(IID_WebView2Standalone_IWebView2,
                                             webViewElement.put_void()));

                    ApplyWebView2StyleCustomizations(webViewElement.get());
                };

            winrt::check_hresult(webViewElement->add_NavigationCompleted(
                *(void**)(&eventHandler),
                put_abi(
                    webViewCustomizationState.navigationCompletedEventToken)));
        }
    }
}

PCWSTR CreateWebViewJsCodeForClear() {
    PCWSTR jsCode =
        LR"(
        (() => {
        const styleElementId = "windhawk-windows-11-start-menu-styler-style";
        const style = document.getElementById(styleElementId);
        if (style) {
            style.parentNode.removeChild(style);
        }
        })();
    )";

    Wh_Log(L"======================================== JS:");
    std::wstringstream ss(jsCode);
    std::wstring line;
    while (std::getline(ss, line, L'\n')) {
        Wh_Log(L"%s", line.c_str());
    }
    Wh_Log(L"========================================");

    return jsCode;
}

void ClearWebViewStyleCustomizations(Controls::WebView webViewElement) {
    PCWSTR jsCode = CreateWebViewJsCodeForClear();

    webViewElement.InvokeScriptAsync(
        L"eval", winrt::single_threaded_vector<winrt::hstring>(
                     {winrt::hstring(jsCode)}));
}

void ClearWebView2StyleCustomizations(
    WebView2Standalone_IWebView2* webViewElement) {
    PCWSTR jsCode = CreateWebViewJsCodeForClear();

    void* operationPtr;
    auto jsCodeHstring = winrt::hstring(jsCode);
    winrt::check_hresult(webViewElement->ExecuteScriptAsync(
        *(void**)(&jsCodeHstring), &operationPtr));
    auto operation =
        winrt::Windows::Foundation::IAsyncOperation<winrt::hstring>{
            operationPtr, winrt::take_ownership_from_abi};
}

void ClearWebViewCustomizations(
    const WebViewCustomizationState& webViewCustomizationState) {
    auto element = webViewCustomizationState.element.get();
    if (!element) {
        return;
    }

    if (!webViewCustomizationState.isWebView2) {
        auto webViewElement = element.as<Controls::WebView>();

        ClearWebViewStyleCustomizations(webViewElement);

        webViewElement.NavigationCompleted(
            webViewCustomizationState.navigationCompletedEventToken);
    } else {
        winrt::com_ptr<WebView2Standalone_IWebView2> webViewElement;
        winrt::check_hresult(
            ((IUnknown*)winrt::get_abi(element))
                ->QueryInterface(IID_WebView2Standalone_IWebView2,
                                 webViewElement.put_void()));

        ClearWebView2StyleCustomizations(webViewElement.get());

        winrt::check_hresult(webViewElement->remove_NavigationCompleted(
            webViewCustomizationState.navigationCompletedEventToken));
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

    if (!g_webContentCss.empty() || !g_webContentJs.empty()) {
        try {
            ApplyCustomizationsIfWebView(handle, element);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

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

    if (auto it = g_webViewsCustomizationState.find(handle);
        it != g_webViewsCustomizationState.end()) {
        const auto& webViewCustomizationState = it->second;
        try {
            ClearWebViewCustomizations(webViewCustomizationState);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }

        g_webViewsCustomizationState.erase(it);
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
            {L"StartMenu:", L"StartMenu."},
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

void ProcessWebStylesFromSettings(
    const StyleConstants& styleConstants,
    const std::vector<ThemeTargetStyles>& themeStyles) {
    std::wstring webContentCss;

    for (const auto& themeStyle : themeStyles) {
        Wh_Log(L"Processing theme WebView styles for %s", themeStyle.target);

        webContentCss += themeStyle.target;
        webContentCss += L" {\n";

        for (const auto& style : themeStyle.styles) {
            webContentCss += ApplyStyleConstants(style, styleConstants);
            webContentCss += L";\n";
        }

        webContentCss += L"}\n";
    }

    for (int i = 0;; i++) {
        string_setting_unique_ptr targetStringSetting(
            Wh_GetStringSetting(L"webContentStyles[%d].target", i));
        if (!*targetStringSetting.get()) {
            break;
        }

        Wh_Log(L"Processing WebView styles for %s", targetStringSetting.get());

        webContentCss += targetStringSetting.get();
        webContentCss += L" {\n";

        for (int styleIndex = 0;; styleIndex++) {
            string_setting_unique_ptr styleSetting(Wh_GetStringSetting(
                L"webContentStyles[%d].styles[%d]", i, styleIndex));
            if (!*styleSetting.get()) {
                break;
            }

            // Skip if commented.
            if (styleSetting[0] == L'/' && styleSetting[1] == L'/') {
                continue;
            }

            webContentCss +=
                ApplyStyleConstants(styleSetting.get(), styleConstants);
            webContentCss += L";\n";
        }

        webContentCss += L"}\n";
    }

    g_webContentCss = std::move(webContentCss);
    g_webContentJs =
        string_setting_unique_ptr(Wh_GetStringSetting(L"webContentCustomJs"))
            .get();
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
    if (wcscmp(themeName, L"TranslucentStartMenu") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeTranslucentStartMenu
                    : &g_themeTranslucentStartMenu_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"NoRecommendedSection") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeNoRecommendedSection
                    : &g_themeNoRecommendedSection_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"SideBySide") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeSideBySide
                    : &g_themeSideBySide_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"SideBySide2") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeSideBySide2
                    : &g_themeSideBySide2_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"SideBySideMinimal") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeSideBySideMinimal
                    : &g_themeSideBySideMinimal_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"Down Aero") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeDown_Aero
                    : &g_themeDown_Aero_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"Windows10") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeWindows10
                    : &g_themeWindows10_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"Windows10_variant_Minimal") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeWindows10_variant_Minimal
                    : &g_themeWindows10_variant_Minimal_ClassicStartMenu;
    } else if (wcscmp(themeName, L"Windows11_Metro10") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeWindows11_Metro10
                    : &g_themeWindows11_Metro10_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"Fluent2Inspired") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeFluent2Inspired
                    : &g_themeFluent2Inspired_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"RosePine") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeRosePine
                    : &g_themeRosePine_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"Windows11_Metro10Minimal") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeWindows11_Metro10Minimal
                    : &g_themeWindows11_Metro10Minimal_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"Everblush") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeEverblush
                    : &g_themeEverblush_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"SunValley") == 0) {
        theme = &g_themeSunValley;
    } else if (wcscmp(themeName, L"21996") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_theme21996
                    : &g_theme21996_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"UniMenu") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeUniMenu
                    : &g_themeUniMenu_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"LegacyFluent") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeLegacyFluent
                    : &g_themeLegacyFluent_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"OnlySearch") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeOnlySearch
                    : &g_themeOnlySearch_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"WindowGlass") == 0) {
        theme = &g_themeWindowGlass;
    } else if (wcscmp(themeName, L"WindowGlass_variant_Minimal") == 0) {
        // Same as WindowGlass, kept for backward compatibility.
        theme = &g_themeWindowGlass;
    } else if (wcscmp(themeName, L"Fluid") == 0) {
        theme = &g_themeFluid;
    } else if (wcscmp(themeName, L"Oversimplified&Accentuated") == 0) {
        theme = &g_themeOversimplified_Accentuated;
    } else if (wcscmp(themeName, L"LiquidGlass") == 0) {
        theme = &g_themeLiquidGlass;
    } else if (wcscmp(themeName, L"Windows10X") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeWindows10X
                    : &g_themeWindows10X_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"TintedGlass") == 0) {
        theme = g_isRedesignedStartMenu
                    ? &g_themeTintedGlass
                    : &g_themeTintedGlass_variant_ClassicStartMenu;
    } else if (wcscmp(themeName, L"LayerMicaUI") == 0) {
        theme = &g_themeLayerMicaUI;
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

    if (g_target == Target::SearchHost) {
        ProcessWebStylesFromSettings(styleConstants,
                                     theme ? theme->webViewTargetStyles
                                           : std::vector<ThemeTargetStyles>{});
    }
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

void UninitializeSettingsAndTap() {
    // Clear failed image brushes list for this thread (revokers will
    // automatically unregister).
    g_failedImageBrushesForThread.failedImageBrushes.clear();
    g_failedImageBrushesForThread.dispatcher = nullptr;

    if (g_delayedAllAppsRootVisibilitySet) {
        g_delayedAllAppsRootVisibilitySet.Cancel();
        g_delayedAllAppsRootVisibilitySet = nullptr;
    }

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

    for (const auto& [handle, webViewCustomizationState] :
         g_webViewsCustomizationState) {
        try {
            ClearWebViewCustomizations(webViewCustomizationState);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    g_webViewsCustomizationState.clear();

    g_targetThreadId = 0;
}

void InitializeSettingsAndTap() {
    DWORD kNoThreadId = 0;
    if (!g_targetThreadId.compare_exchange_strong(kNoThreadId,
                                                  GetCurrentThreadId())) {
        return;
    }

    ProcessAllStylesFromSettings();

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
        case Target::StartMenu:
            if (bTextualClassName &&
                _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                Wh_Log(L"Initializing - Created core window: %08X via %S",
                       (DWORD)(ULONG_PTR)hWnd, funcName);
                InitializeSettingsAndTap();
            }
            break;

        case Target::SearchHost:
            if (bTextualClassName &&
                _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                Wh_Log(L"Initializing - Created core window: %08X via %S",
                       (DWORD)(ULONG_PTR)hWnd, funcName);
                // Initializing at this point is too early and doesn't work.
                RunFromWindowThreadViaPostMessage(
                    hWnd, [](PVOID) { InitializeSettingsAndTap(); }, nullptr);
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

HWND GetCoreWnd() {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd};
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

            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                *param.hWnd = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

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

using RtlQueryFeatureConfiguration_t = int(NTAPI*)(UINT32,
                                                   int,
                                                   INT64*,
                                                   RTL_FEATURE_CONFIGURATION*);
RtlQueryFeatureConfiguration_t RtlQueryFeatureConfiguration_Original;
int NTAPI RtlQueryFeatureConfiguration_Hook(UINT32 featureId,
                                            int group,
                                            INT64* variant,
                                            RTL_FEATURE_CONFIGURATION* config) {
    int ret = RtlQueryFeatureConfiguration_Original(featureId, group, variant,
                                                    config);

    switch (featureId) {
        // Disable the Start Menu Phone Link layout feature.
        // https://winaero.com/enable-phone-link-flyout-start-menu/
        case 48697323:
            Wh_Log(L"%u", featureId);
            if (g_disableNewStartMenuLayout ==
                DisableNewStartMenuLayout::forceNewLayout) {
                config->enabledState = FEATURE_ENABLED_STATE_ENABLED;
            } else if (g_disableNewStartMenuLayout ==
                       DisableNewStartMenuLayout::
                           disableNewLayoutAndPhoneLink) {
                config->enabledState = FEATURE_ENABLED_STATE_DISABLED;
            }
            break;

        // Disable the revamped Start menu experience.
        // https://x.com/phantomofearth/status/1907877141540118888
        case 47205210:
        // case 49221331:
        case 49402389:
            Wh_Log(L"%u", featureId);
            if (g_disableNewStartMenuLayout ==
                DisableNewStartMenuLayout::forceNewLayout) {
                config->enabledState = FEATURE_ENABLED_STATE_ENABLED;
            } else {
                config->enabledState = FEATURE_ENABLED_STATE_DISABLED;
            }
            break;
    }

    return ret;
}

std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
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

PTP_TIMER g_statsTimer;

bool StartStatsTimer() {
    static constexpr WCHAR kStatsBaseUrl[] =
        L"https://github.com/ramensoftware/"
        L"windows-11-start-menu-styling-guide/"
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

DisableNewStartMenuLayout GetDisableNewStartMenuLayout() {
    PCWSTR disableNewStartMenuLayoutStr =
        Wh_GetStringSetting(L"disableNewStartMenuLayout");
    DisableNewStartMenuLayout disableNewStartMenuLayout =
        DisableNewStartMenuLayout::windowsDefault;
    if (wcscmp(disableNewStartMenuLayoutStr, L"1") == 0) {
        disableNewStartMenuLayout =
            DisableNewStartMenuLayout::disableNewLayoutAndPhoneLink;
    } else if (wcscmp(disableNewStartMenuLayoutStr,
                      L"disableNewLayoutKeepPhoneLink") == 0) {
        disableNewStartMenuLayout =
            DisableNewStartMenuLayout::disableNewLayoutKeepPhoneLink;
    } else if (wcscmp(disableNewStartMenuLayoutStr, L"forceNewLayout") == 0) {
        disableNewStartMenuLayout = DisableNewStartMenuLayout::forceNewLayout;
    }
    Wh_FreeStringSetting(disableNewStartMenuLayoutStr);
    return disableNewStartMenuLayout;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_target = Target::StartMenu;

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
                if (_wcsicmp(moduleFileName, L"SearchHost.exe") == 0 ||
                    _wcsicmp(moduleFileName, L"SearchApp.exe") == 0) {
                    g_target = Target::SearchHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
            }
            break;
    }

    g_disableNewStartMenuLayout = GetDisableNewStartMenuLayout();

    if (g_disableNewStartMenuLayout !=
        DisableNewStartMenuLayout::windowsDefault) {
#ifdef _WIN64
        const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
#else
        const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
#endif
        bool isInitialThread =
            *(USHORT*)((BYTE*)NtCurrentTeb() + OFFSET_SAME_TEB_FLAGS) & 0x0400;
        if (!isInitialThread) {
            // Throttle to avoid exiting in a loop if something goes wrong.
            WCHAR lastExitTickCountKey[64];
            swprintf_s(lastExitTickCountKey, L"lastExitTickCount_%d",
                       static_cast<int>(g_target));
            DWORD lastTickCount =
                (DWORD)Wh_GetIntValue(lastExitTickCountKey, 0);
            DWORD currentTickCount = GetTickCount();
            if (currentTickCount - lastTickCount > 10 * 1000) {
                Wh_SetIntValue(lastExitTickCountKey, currentTickCount);
                // Exit to have the new setting take effect. The process will be
                // relaunched automatically.
                ExitProcess(0);
            }
        }
    }

    g_isRedesignedStartMenu = g_disableNewStartMenuLayout ==
                                  DisableNewStartMenuLayout::forceNewLayout ||
                              (g_disableNewStartMenuLayout ==
                                   DisableNewStartMenuLayout::windowsDefault &&
                               IsOsFeatureEnabled(47205210).value_or(false) &&
                               IsOsFeatureEnabled(49221331).value_or(false) &&
                               IsOsFeatureEnabled(49402389).value_or(false));

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

    if (g_disableNewStartMenuLayout !=
        DisableNewStartMenuLayout::windowsDefault) {
        HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
        RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration =
            (RtlQueryFeatureConfiguration_t)GetProcAddress(
                hNtDll, "RtlQueryFeatureConfiguration");
        if (pRtlQueryFeatureConfiguration) {
            Wh_SetFunctionHook((void*)pRtlQueryFeatureConfiguration,
                               (void*)RtlQueryFeatureConfiguration_Hook,
                               (void**)&RtlQueryFeatureConfiguration_Original);
        } else {
            Wh_Log(L"Failed to hook RtlQueryFeatureConfiguration");
        }
    }

    if (g_target == Target::StartMenu) {
        StartStatsTimer();
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Initializing - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { InitializeSettingsAndTap(); }, nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_disableNewStartMenuLayout !=
        DisableNewStartMenuLayout::windowsDefault) {
        // Exit to have the new setting take effect. The process will be
        // relaunched automatically.
        ExitProcess(0);
    }

    if (g_target == Target::StartMenu) {
        StopStatsTimer();
    }

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Uninitializing - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { UninitializeSettingsAndTap(); }, nullptr);
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

    if (GetDisableNewStartMenuLayout() != g_disableNewStartMenuLayout) {
        // Exit to have the new setting take effect. The process will be
        // relaunched automatically.
        ExitProcess(0);
    }

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Reinitializing - Found core window");
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                UninitializeSettingsAndTap();
                InitializeSettingsAndTap();
            },
            nullptr);
    }
}
