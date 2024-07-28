// ==WindhawkMod==
// @id              windows-11-start-menu-styler
// @name            Windows 11 Start Menu Styler
// @description     Customize the start menu with themes contributed by others or create your own
// @version         1.1.5
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lversion -Wl,--export-all-symbols
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

**Note**: This mod requires Windhawk v1.4 or later.

Also check out the **Windows 11 Taskbar Styler**, **Windows 11 Notification
Center Styler** mods.

## Themes

Themes are collections of styles. The following themes are integrated into the
mod and can be selected in the settings:

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

[![TranslucentStartMenu](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/TranslucentStartMenu/screenshot-small.png)
\
TranslucentStartMenu](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/TranslucentStartMenu/README.md)

[![Fluent2Inspired](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Fluent2Inspired/screenshot-small.png)
\
Fluent2Inspired](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Fluent2Inspired/README.md)

[![Windows10](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Windows10/screenshot-small.png)
\
Windows10](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Windows10/README.md)

[![Windows11_Metro10](https://raw.githubusercontent.com/ramensoftware/windows-11-start-menu-styling-guide/main/Themes/Windows11_Metro10/screenshot-small.png)
\
Windows11_Metro10](https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/Windows11_Metro10/README.md)

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

### Resource variables

Some variables, such as size and padding for various controls, are defined as
resource variables.

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
  - NoRecommendedSection: NoRecommendedSection
  - SideBySide: SideBySide
  - SideBySide2: SideBySide2
  - SideBySideMinimal: SideBySideMinimal
  - TranslucentStartMenu: TranslucentStartMenu
  - Fluent2Inspired: Fluent2Inspired
  - Windows10: Windows10
  - Windows11_Metro10: Windows11_Metro10
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

const Theme g_themeNoRecommendedSection = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=504"}},
}};

const Theme g_themeSideBySide = {{
    ThemeTargetStyles{L"Grid#UndockedRoot", {
        L"MaxWidth=700",
        L"Margin=0,0,300,0"}},
    ThemeTargetStyles{L"Grid#AllAppsRoot", {
        L"Visibility=Visible",
        L"Width=390",
        L"Margin=-590,0,590,0",
        L"Padding=0,0,40,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MinWidth=860",
        L"MaxWidth=860"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ShowAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Margin=-22,0,0,0"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Margin=45,25,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar[1]", {
        L"Margin=0,0,6,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.PipsPager#PinnedListPipsPager", {
        L"Margin=-8,0,8,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsWrapGrid > Windows.UI.Xaml.Controls.GridViewItem", {
        L"MaxWidth=220",
        L"MinWidth=85",
        L"MaxHeight=120"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"Margin=-15,0,5,0"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Margin=0,20,0,-20"}},
    ThemeTargetStyles{L"Grid#MoreSuggestionsRoot", {
        L"Margin=-1,-26,-4,0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox > Grid > Border#BorderElement", {
        L"Background=White"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter[1]", {
        L"Margin=-10,-10,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter > Grid", {
        L"Margin=-400,0,400,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#MoreSuggestionsListHeaderText", {
        L"Margin=-40,0,0,0"}},
}};

const Theme g_themeSideBySide2 = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Visibility=Visible",
        L"Width=510",
        L"Margin=264,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Visibility=Visible",
        L"Width=320",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-1060\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MinWidth=776",
        L"MaxWidth=776"}},
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
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SuggestionsParentContainer", {
        L"Height=302"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView#NavigationPane", {
        L"FlowDirection=1",
        L"Margin=30,0,30,0"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView#PowerButton", {
        L"FlowDirection=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsStackPanel > Windows.UI.Xaml.Controls.ListViewItem", {
        L"FlowDirection=0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton#StartMenuSearchBox", {
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
    ThemeTargetStyles{L" Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
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
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#HideMoreSuggestionsButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.FontIcon ", {
        L"FontSize=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter[1]", {
        L"Margin=-240,-20,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter > Grid", {
        L"Margin=-450,0,450,0"}},
    ThemeTargetStyles{L"Panel", {
        L"FlowDirection=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter", {
        L"Margin=-10,-8,0,0"}},
}};

const Theme g_themeSideBySideMinimal = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Visibility=Visible",
        L"Width=348",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"178\" />",
        L"Margin=-80,-20,0,0",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Visibility=Visible",
        L"Width=320",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-800\" />",
        L"Margin=-30,-20,0,0"}},
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
}};

const Theme g_themeTranslucentStartMenu = {{
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"CornerRadius=15",
        L"Background:=<AcrylicBrush TintColor=\"Transparent\" TintLuminosityOpacity=\"0\" TintOpacity=\"0\" Opacity=\"1\" FallbackColor=\"#70262626\"/>",
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#BorderElement", {
        L"CornerRadius=10",
        L"BorderThickness=0,0,0,0",
        L"Background:=<AcrylicBrush TintLuminosityOpacity=\"0.03\" TintOpacity=\"0\" Opacity=\"1\" FallbackColor=\"#70262626\"/>"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=504"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background:=<AcrylicBrush TintColor=\"Transparent\" TintLuminosityOpacity=\"0\" TintOpacity=\"0\" Opacity=\"1\" FallbackColor=\"#70262626\"/>",
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=<AcrylicBrush TintColor=\"Transparent\" TintLuminosityOpacity=\"0\" TintOpacity=\"0\" Opacity=\"1\" FallbackColor=\"#70262626\"/>",
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"Border#AccentAppBorder", {
        L"Background:=<AcrylicBrush TintColor=\"Transparent\" TintLuminosityOpacity=\"0\" TintOpacity=\"0\" Opacity=\"1\" FallbackColor=\"#70262626\"/>",
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"Border#LayerBorder", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Background:=<AcrylicBrush TintColor=\"Transparent\" TintLuminosityOpacity=\"0.03\" TintOpacity=\"0\" Opacity=\"1\" FallbackColor=\"#70262626\"/>",
        L"CornerRadius=10",
        L"BorderThickness=0,0,0,0"}},
}};

const Theme g_themeFluent2Inspired = {{
    ThemeTargetStyles{L"Button#CloseAllAppsButton", {
        L"CornerRadius=14",
        L"Margin=0,0,-32,0",
        L"Width=74",
        L"Height=26"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Button#ShowAllAppsButton", {
        L"CornerRadius=14",
        L"Margin=0,0,32,0",
        L"Width=148",
        L"Height=26"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Margin=30,0,31,28"}},
    ThemeTargetStyles{L"PipsPager#PinnedListPipsPager", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".85\" Opacity=\"1\"/>",
        L"CornerRadius=12",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".25\" Opacity=\"1\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Grid#MainContent", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"StartMenu.PinnedList", {
        L"Height=690"}},
    ThemeTargetStyles{L"TextBlock#DisplayName", {
        L"Margin=0,16,0,-16",
        L"FontSize=13",
        L"FontFamily=Aptos",
        L"Opacity=.65"}},
    ThemeTargetStyles{L"TextBlock#PinnedListHeaderText", {
        L"Margin=-32,0,0,0",
        L"FontFamily=Aptos",
        L"Opacity=.85",
        L"FontSize=16"}},
    ThemeTargetStyles{L"Border#TaskbarSearchBackground", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#AppBorder", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".85\" Opacity=\"1\"/>",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".25\" Opacity=\"1\"/>",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Border#dropshadow", {
        L"CornerRadius=12"}},
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
    ThemeTargetStyles{L"GridViewItem > Border#ContentBorder > Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"FocusVisualPrimaryThickness=0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > ContentPresenter > TextBlock#PlaceholderText", {
        L"Text=Where to next?",
        L"FontWeight=700",
        L"FontFamily=Aptos",
        L"FontSize=24",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\" Opacity=\".85\"/>",
        L"Margin=-50,0,0,0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > Border", {
        L"Background=transparent",
        L"BorderBrush=transparent"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton > Grid > FontIcon", {
        L"Transform3D:=<CompositeTransform3D  TranslateX=\"165\" TranslateY=\"-1\"/>",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource FocusStrokeColorOuter}\" Opacity=\".85\"/>",
        L"FontSize=24"}},
    ThemeTargetStyles{L"Grid#TopLevelRoot", {
        L"Margin=0,-8,0,0"}},
    ThemeTargetStyles{L"GridViewItem", {
        L"Height=96",
        L"Margin=0,0,0,2"}},
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
        L"Opacity=.85"}},
    ThemeTargetStyles{L"Button#CloseAllAppsButton > ContentPresenter > StackPanel > TextBlock", {
        L"FontFamily=Aptos",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#AllAppsPaneHeader", {
        L"Margin=64,-8,64,0"}},
    ThemeTargetStyles{L"Grid#InnerContent", {
        L"Margin=0,31,0,-64"}},
    ThemeTargetStyles{L"TextBlock#AppDisplayName", {
        L"FontFamily=Aptos",
        L"Opacity=.85",
        L"Margin=4,0,0,0"}},
    ThemeTargetStyles{L"Button#Header > Border > TextBlock", {
        L"FontFamily=Aptos",
        L"FontWeight=600",
        L"Opacity=.85"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView > StartDocked.NavigationPaneButton > Grid > Border", {
        L"CornerRadius=99",
        L"Margin=1"}},
    ThemeTargetStyles{L"TileGrid", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource ControlFillColorInputActive}\" Opacity=\".7\"/>",
        L"CornerRadiusProtected=8",
        L"BorderThicknessProtected=1",
        L"BorderBrushProtected:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\".35\"/>"}},
    ThemeTargetStyles{L"ListViewItem", {
        L"Margin=1,0,-6,0",
        L"CornerRadius=4"}},
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
        L"Margin=25,32,25,13"}},
    ThemeTargetStyles{L"Grid#WebViewGrid > WebView", {
        L"Margin=-3,0,0,0"}},
    ThemeTargetStyles{L"TextBlock#StatusMessage", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#LogoBackgroundPlate", {
        L"Margin=12,0,0,0"}},
    ThemeTargetStyles{L"Grid#WebViewGrid", {
        L"CornerRadius=0,0,12,0"}},
    ThemeTargetStyles{L"Border#DropShadow", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Border#DropShadowDismissTarget", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter[1]", {
        L"Transform3D:=<CompositeTransform3D   TranslateX=\"-250\" TranslateY=\"50\"/>"}},
    ThemeTargetStyles{L"Rectangle[4]", {
        L"Margin=0,0,0,20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter", {
        L"Margin=-250,0,0,0"}},
}};

const Theme g_themeWindows10 = {{
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#RootContent", {
        L"Height=800"}},
    ThemeTargetStyles{L"Rectangle[4]", {
        L"Margin=0,-20,0,0"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MaxWidth=650",
        L"Margin=-14,25,12,-12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Margin=292,-30,-292,0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Visibility=Collapsed",
        L"Height=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Width=364",
        L"Height=Auto",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-795\" TranslateY=\"-14\" />",
        L"Visibility=Visible",
        L"Margin=0,-45,0,0"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"BorderThickness=1.5",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=0",
        L"BorderThickness=1",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Border#BorderBackground", {
        L"CornerRadius=0",
        L"BorderThickness=1",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.42\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#Border", {
        L"CornerRadius=0",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"BorderBrush:=<RevealBorderBrush Color=\"White\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Height=720",
        L"Margin=0,-6,0,6"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.32\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"270\" TranslateY=\"-175\" TranslateX=\"-12\"/>",
        L"Width=745",
        L"CornerRadius=8",
        L"Grid.Row=0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"141\" CenterX=\"250\" />",
        L"Grid.Column=0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"85\" />",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#UserTileNameText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#UserTileIcon", {
        L"Transform3D:=<CompositeTransform3D  RotationZ=\"90\" TranslateY=\"32\"/>"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid > ContentPresenter", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"90\" TranslateY=\"40\"/>"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-300\" TranslateX=\"475\" RotationZ=\"90\" />",
        L"Margin=-680,340,680,-340"}},
    ThemeTargetStyles{L"Grid#AllAppsPaneHeader", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border@CommonStates", {
        L"BorderBrush@Active:=<RevealBorderBrush Color=\"White\" TargetTheme=\"1\" Opacity=\"0.32\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Margin=1",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"CornerRadius=4",
        L"Background=#99646464",
        L"Margin=1",
        L"Height=98",
        L"Width=98"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"Width=375",
        L"Height=750",
        L"Margin=-270,-30,0,30"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Margin=0,-18,37,0",
        L"Height=690"}},
    ThemeTargetStyles{L"MenuFlyoutSeparator", {
        L"Margin=0,-2,0,-2",
        L"Padding=4"}},
    ThemeTargetStyles{L"MenuFlyoutItem", {
        L"Margin=2,0,0,2"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border > ScrollViewer", {
        L"BorderThickness=1",
        L"CornerRadius=8",
        L"Padding=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"StartMenu:ExpandedFolderList", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-135\"/>",
        L"Width=320",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#FolderList", {
        L"Margin=20,-15,0,0",
        L"Height=450",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid", {
        L"CornerRadius=8",
        L"Width=420",
        L"Margin=-120,0,0,0"}},
    ThemeTargetStyles{L"Border#UninstallFlyoutPresenterBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentDialog", {
        L"Margin=-650,250,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Rectangle#StartDocked.SearchBoxToggleButton > StartMenuSearchBox", {
        L"Visibility=Collapsed",
        L"Height=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton", {
        L"Width=40",
        L"Height=40",
        L"Margin=0,0,320,663",
        L"Visibility=Visible",
        L"FontSize=14",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.SemanticZoom#ZoomControl", {
        L"IsZoomOutButtonEnabled=True"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE75F",
        L"FontSize=28",
        L"Padding=0,8,0,0",
        L"Margin=0,0,0,10",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#MoreSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter[1]", {
        L"Transform3D:=<CompositeTransform3D  TranslateX=\"-700\"/>"}},
    ThemeTargetStyles{L"Border#UninstallFlyoutPresenterBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
}};

const Theme g_themeWindows10_variant_24H2 = {{
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#RootContent", {
        L"Height=800"}},
    ThemeTargetStyles{L"Rectangle[4]", {
        L"Margin=0,-20,0,0"}},
    ThemeTargetStyles{L"StartDocked.StartSizingFrame", {
        L"MaxWidth=650",
        L"Margin=-14,25,12,-12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#UndockedRoot", {
        L"Margin=292,-30,-292,0"}},
    ThemeTargetStyles{L"StartDocked.SearchBoxToggleButton", {
        L"Visibility=Collapsed",
        L"Height=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AllAppsRoot", {
        L"Width=365",
        L"Height=Auto",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-785\" TranslateY=\"-14\" />",
        L"Visibility=Visible",
        L"Margin=-1,-45,0,0"}},
    ThemeTargetStyles{L"Border#AcrylicBorder", {
        L"BorderThickness=1.5",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=0",
        L"BorderThickness=1",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Border#BorderBackground", {
        L"CornerRadius=0",
        L"BorderThickness=1",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.42\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#Border", {
        L"CornerRadius=0",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Border#AcrylicOverlay", {
        L"BorderBrush:=<RevealBorderBrush Color=\"White\" TargetTheme=\"1\" Opacity=\"1\" />",
        L"Height=720",
        L"Margin=0,-6,0,6"}},
    ThemeTargetStyles{L"Grid#ContentBorder@CommonStates", {
        L"Background@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.32\"/>",
        L"BorderBrush@PointerOver:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"StartDocked.NavigationPaneView", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"270\" TranslateY=\"-175\" TranslateX=\"-12\"/>",
        L"Width=Auto",
        L"Grid.Row=0"}},
    ThemeTargetStyles{L"StartDocked.AppListView#NavigationPanePlacesListView", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"141\" CenterX=\"250\" />",
        L"Grid.Column=0"}},
    ThemeTargetStyles{L"StartDocked.UserTileView", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"85\" />",
        L"Margin=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#UserTileNameText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#UserTileIcon", {
        L"Transform3D:=<CompositeTransform3D  RotationZ=\"90\" TranslateY=\"32\"/>"}},
    ThemeTargetStyles{L"StartDocked.AppListViewItem > Grid > ContentPresenter", {
        L"Transform3D:=<CompositeTransform3D RotationZ=\"90\" TranslateY=\"40\"/>"}},
    ThemeTargetStyles{L"StartDocked.PowerOptionsView", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-300\" TranslateX=\"685\" RotationZ=\"90\" />",
        L"Margin=-680,340,680,-340"}},
    ThemeTargetStyles{L"Grid#AllAppsPaneHeader", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border@CommonStates", {
        L"BorderBrush@Active:=<RevealBorderBrush Color=\"White\" TargetTheme=\"1\" Opacity=\"0.32\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.5\"/>",
        L"Margin=1",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"CornerRadius=4",
        L"Background=#99646464",
        L"Margin=1",
        L"Height=98",
        L"Width=98"}},
    ThemeTargetStyles{L"StartMenu.PinnedList#StartMenuPinnedList", {
        L"Width=375",
        L"Height=750",
        L"Margin=-270,-30,0,30"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ScrollBar", {
        L"Margin=0,-18,37,0",
        L"Height=690"}},
    ThemeTargetStyles{L"MenuFlyoutSeparator", {
        L"Margin=0,-2,0,-2",
        L"Padding=4"}},
    ThemeTargetStyles{L"MenuFlyoutItem", {
        L"Margin=0"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border > ScrollViewer", {
        L"BorderThickness=1",
        L"CornerRadius=8",
        L"Padding=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"StartMenu:ExpandedFolderList", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-135\"/>",
        L"Width=320",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.GridView#FolderList", {
        L"Margin=20,-15,0,0",
        L"Height=450",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Border", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"StartMenu.ExpandedFolderList > Grid > Grid", {
        L"CornerRadius=8",
        L"Width=420",
        L"Margin=-120,0,0,0"}},
    ThemeTargetStyles{L"Border#UninstallFlyoutPresenterBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentDialog", {
        L"Margin=-650,250,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#CloseAllAppsButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Rectangle#StartDocked.SearchBoxToggleButton > StartMenuSearchBox", {
        L"Visibility=Collapsed",
        L"Height=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton", {
        L"Width=40",
        L"Height=40",
        L"Margin=0,0,320,663",
        L"Visibility=Visible",
        L"FontSize=14",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.SemanticZoom#ZoomControl", {
        L"IsZoomOutButtonEnabled=True"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE75F",
        L"FontSize=28",
        L"Padding=0,8,0,0",
        L"Margin=0,0,0,10",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#SuggestionsParentContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#MoreSuggestionsRoot", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter[1]", {
        L"Transform3D:=<CompositeTransform3D  TranslateX=\"-700\"/>"}},
    ThemeTargetStyles{L"Border#UninstallFlyoutPresenterBorder", {
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
}};

const Theme g_themeWindows11_Metro10 = {{
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
        L"Margin=0,0,0,0"}},
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
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ItemsStackPanel", {
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
        L"IsZoomOutButtonEnabled=true"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE73F"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#ZoomOutButton", {
        L"Width=28",
        L"Height=28",
        L"Margin=0,0,0,0",
        L"FontSize=14",
        L"CornerRadius=4",
        L"VerticalAlignment=0",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-1\" TranslateY=\"-36\"/>",
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
        L"Margin=-30,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SuggestionsParentContainer", {
        L"Margin=-20,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader", {
        L"Margin=35,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border@CommonStates", {
        L"BorderBrush@Active:=<RevealBorderBrush Color=\"White\" TargetTheme=\"1\" Opacity=\"0.3\"/>",
        L"Background:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.3\"/>\u0009",
        L"Margin=1",
        L"BorderBrush:=<RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"0.8\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#ContentBorder > Windows.UI.Xaml.Controls.Grid#DroppedFlickerWorkaroundWrapper > Border#BackgroundBorder", {
        L"Background:=<RevealBorderBrush Color=\"#646464\" TargetTheme=\"1\" Opacity=\".1\"/>\u0009",
        L"Margin=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#PinnedListHeaderText", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Rectangle[4]", {
        L"Margin=0,-20,0,0"}},
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
    winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(this));
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

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

bool IsVersionAtLeast(WORD major, WORD minor, WORD build, WORD qfe) {
    static VS_FIXEDFILEINFO* fixedFileInfo =
        GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return false;
    }

    WORD moduleMajor = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD moduleMinor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD moduleBuild = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD moduleQfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    if (moduleMajor != major) {
        return moduleMajor > major;
    }

    if (moduleMinor != minor) {
        return moduleMinor > minor;
    }

    if (moduleBuild != build) {
        return moduleBuild > build;
    }

    return moduleQfe >= qfe;
}

void ProcessAllStylesFromSettings() {
    PCWSTR themeName = Wh_GetStringSetting(L"theme");
    const Theme* theme = nullptr;
    if (wcscmp(themeName, L"NoRecommendedSection") == 0) {
        theme = &g_themeNoRecommendedSection;
    } else if (wcscmp(themeName, L"SideBySide") == 0) {
        theme = &g_themeSideBySide;
    } else if (wcscmp(themeName, L"SideBySide2") == 0) {
        theme = &g_themeSideBySide2;
    } else if (wcscmp(themeName, L"SideBySideMinimal") == 0) {
        theme = &g_themeSideBySideMinimal;
    } else if (wcscmp(themeName, L"TranslucentStartMenu") == 0) {
        theme = &g_themeTranslucentStartMenu;
    } else if (wcscmp(themeName, L"Fluent2Inspired") == 0) {
        theme = &g_themeFluent2Inspired;
    } else if (wcscmp(themeName, L"Windows10") == 0) {
        if (IsVersionAtLeast(10, 0, 26100, 0)) {
            theme = &g_themeWindows10_variant_24H2;
        } else {
            theme = &g_themeWindows10;
        }
    } else if (wcscmp(themeName, L"Windows11_Metro10") == 0) {
        theme = &g_themeWindows11_Metro10;
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

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        Wh_Log(L"Initializing - Created core window: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        InitializeSettingsAndTap();
    }

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

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        Wh_Log(L"Initializing - Created core window: %08X",
               (DWORD)(ULONG_PTR)hWnd);
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

HWND GetCoreWnd() {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
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

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE user32Module = LoadLibrary(L"user32.dll");
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

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Initializing - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) WINAPI { InitializeSettingsAndTap(); },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Uninitializing - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) WINAPI { UninitializeSettingsAndTap(); },
            nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Reinitializing - Found core window");
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) WINAPI {
                UninitializeSettingsAndTap();
                InitializeSettingsAndTap();
            },
            nullptr);
    }
}
