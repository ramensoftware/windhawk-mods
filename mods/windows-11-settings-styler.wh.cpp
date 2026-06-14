// ==WindhawkMod==
// @id              windows-11-settings-styler
// @name            Windows 11 Settings Styler
// @description     Customize the Windows Settings app with themes contributed by others or create your own
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         SystemSettings.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
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
# Windows 11 Settings Styler

Customize the Windows Settings app (Win+I) with themes contributed by others or
create your own.

Also check out the **Windows 11 Start Menu Styler**, **Windows 11 Taskbar
Styler**, **Windows 11 Notification Center Styler** mods.

## Themes

Themes are collections of styles. The following themes are integrated into the
mod and can be selected in the settings:

[![Densy](https://raw.githubusercontent.com/ramensoftware/windows-11-settings-styling-guide/refs/heads/main/Themes/Densy/screenshot.png)
\
Densy](https://github.com/ramensoftware/windows-11-settings-styling-guide/blob/main/Themes/Densy/README.md)

More themes can be found in the **Themes** section of [The Windows 11 Settings
styling
guide](https://github.com/ramensoftware/windows-11-settings-styling-guide/blob/main/README.md#themes)

## Advanced styling

See the same section of the **Windows 11 Start Menu Styler**
[mod](https://windhawk.net/mods/windows-11-start-menu-styler)

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
  - Densy: Densy
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

const Theme g_themeDensy = {{
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Grid#RootCommandSearchGrid > SystemSettings.View.SettingsAutoSuggestCommandSearchBox#CommandSearchBox", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot", {
        L"Margin=0,0,0,0",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton", {
        L"Margin=0,0,0,0",
        L"Padding=1,0,0,0",
        L"Height=60"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot", {
        L"Margin=0,0,0,0",
        L"MinHeight=18"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid > Rectangle#SelectionIndicator", {
        L"Height=18"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid#ContentGrid", {
        L"Margin=0,0,6,0",
        L"MinHeight=32"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView", {
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot", {
        L"Width=180"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > Grid#ShadowCaster", {
        L"Width=180"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Grid#AppTitleBar", {
        L"MinHeight:=$ListItem_Hmin"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid#PaneToggleButtonGrid > Grid#ButtonHolderGrid", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid#PaneToggleButtonGrid > Grid#ButtonHolderGrid > Button#NavigationViewBackButton", {
        L"Width=24"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent > ContentPresenter > SystemSettings.View.AlignableContentControl#PermanentNavViewHeaderAlignControl > ContentPresenter > Microsoft.UI.Xaml.Controls.BreadcrumbBar#PermanentNavigationViewBreadcrumbBar > Microsoft.UI.Xaml.Controls.ItemsRepeater#PART_ItemsRepeater > Microsoft.UI.Xaml.Controls.BreadcrumbBarItem", {
        L"FontSize=24"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot", {
        L"Margin=0,-15,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl", {
        L"Margin=1,0,15,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter", {
        L"ContentMargin=1,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > Border", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem", {
        L"Padding=1,0,0,0",
        L"MinHeight:=$ListItem_Hmin"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsStackPanel > SystemSettings.View.SettingsListViewItem", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent", {
        L"Foreground=#DD000000"}},
    ThemeTargetStyles{L"ItemsStackPanel > SystemSettings.View.SettingsListViewItem", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > Border", {
        L"Margin=0,0,0,0",
        L"Height=24"}},
    ThemeTargetStyles{L"StackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.HeroUserControl#HeroUserControl > Grid#LayoutRoot > Grid#ProfileLayout > ContentControl#VisualGrid", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"StackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.HeroUserControl#HeroUserControl > Grid#LayoutRoot > Grid#ProfileLayout > Border", {
        L"Margin=4,0,0,0"}},
    ThemeTargetStyles{L"ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.HeroUserControl#HeroUserControl", {
        L"Margin=0,0,0,2"}},
    ThemeTargetStyles{L"ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel", {
        L"Orientation=1"}},
    ThemeTargetStyles{L"ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem#EntityListItemControl", {
        L"Orientation=0"}},
    ThemeTargetStyles{L"ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent", {
        L"Margin:=$SubTitleMrg",
        L"FontSize:=$SubTitleFontSz"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent > TextBlock", {
        L"Foreground:=$SubTitleCol"}},
    ThemeTargetStyles{L"ItemsStackPanel > SystemSettings.View.SettingsListViewItem", {
        L"Height=26",
        L"MinHeight:=$ListItem_Hmin"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid", {
        L"ColumnDefinitions:=$ColDef"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent > ContentPresenter > SystemSettings.View.AlignableContentControl#PermanentNavViewHeaderAlignControl", {
        L"Height=32"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent > ContentPresenter > SystemSettings.View.AlignableContentControl#PermanentNavViewHeaderAlignControl > ContentPresenter > Microsoft.UI.Xaml.Controls.BreadcrumbBar#PermanentNavigationViewBreadcrumbBar > Microsoft.UI.Xaml.Controls.ItemsRepeater#PART_ItemsRepeater > Microsoft.UI.Xaml.Controls.BreadcrumbBarItem", {
        L"Height=32"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent > ContentPresenter > SystemSettings.View.AlignableContentControl#PermanentNavViewHeaderAlignControl > ContentPresenter > Microsoft.UI.Xaml.Controls.BreadcrumbBar#PermanentNavigationViewBreadcrumbBar > Microsoft.UI.Xaml.Controls.ItemsRepeater#PART_ItemsRepeater > Microsoft.UI.Xaml.Controls.BreadcrumbBarItem", {
        L"FontSize=18"}},
    ThemeTargetStyles{L"Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > RichTextBlock", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button", {
        L"Padding=3,1,3,2"}},
    ThemeTargetStyles{L"SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem", {
        L"MinHeight:=$ListItem_Hmin",
        L"Height:=$ListItem_H",
        L"Padding=0,1,0,1"}},
    ThemeTargetStyles{L"SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > ToggleSwitch > Grid > Grid", {
        L"RowDefinitions:=<RowDefinitionCollection><RowDefinition Height=\"24\"/></RowDefinitionCollection>"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem", {
        L"Padding=0,0,0,0",
        L"MinHeight:=$ListItem_Hmin"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > StackPanel > ContentPresenter#SubtitleContent", {
        L"Foreground:=$SubTitleCol",
        L"Margin:=$SubTitleMrg",
        L"FontSize:=$SubTitleFontSz"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > StackPanel", {
        L"Orientation=1"}},
    ThemeTargetStyles{L"ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentControl#ListTemplate > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem", {
        L"MinHeight=20",
        L"MaxHeight=42"}},
    ThemeTargetStyles{L"ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentControl#ListTemplate > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > Border", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentControl#ListTemplate > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem#EntityListItemControl", {
        L"MinHeight=20"}},
    ThemeTargetStyles{L"ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentControl#ListTemplate > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem#EntityListItemControl > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent > Grid > SystemSettings.View.SettingsWrapPanel > TextBlock", {
        L"Foreground:=$SubTitleCol",
        L"FontSize:=$SubTitleFontSz"}},
    ThemeTargetStyles{L"ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentControl#ListTemplate > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem#EntityListItemControl > Grid > ContentPresenter#IconContentPresenter", {
        L"Margin=1,0,6,0"}},
    ThemeTargetStyles{L"ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentControl#ListTemplate > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem#EntityListItemControl > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > TextBlock", {
        L"Foreground=#DD000000"}},
    ThemeTargetStyles{L"ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentControl#ListTemplate > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentControl#ListTemplate > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem#EntityListItemControl", {
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem#EntityListItemControl > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent", {
        L"Margin=1,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentControl#ListTemplate > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem", {
        L"MinHeight=36",
        L"Height=36",
        L"MaxHeight=36"}},
    ThemeTargetStyles{L"SystemSettings.View.SpacingStackPanel", {
        L"Spacing=0"}},
    ThemeTargetStyles{L"SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > SystemSettings.View.SpacingStackPanel > ContentPresenter > UserControl > Grid > Grid > StackPanel#VerticalAlignedPanel > ContentControl > ContentPresenter > StackPanel > ItemsControl > SystemSettings.View.SettingsWrapPanel > ContentPresenter > ContentControl > ContentPresenter > StackPanel > SystemSettings.View.StableComboBox > Grid#LayoutRoot > Border#HighlightBackground", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsWrapPanel > ContentPresenter > ContentControl > ContentPresenter > StackPanel > SystemSettings.View.StableComboBox > Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.AnimatedIcon#DropDownGlyph", {
        L"Margin=0,0,4,0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsWrapPanel > ContentPresenter > ContentControl > ContentPresenter > StackPanel > SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"VerticalAlignment=1"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > UserControl > Grid > Grid > StackPanel#VerticalAlignedPanel > ContentControl > ContentPresenter > StackPanel > ItemsControl > ItemsPresenter > SystemSettings.View.SettingsWrapPanel", {
        L"Margin=-14,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsWrapPanel > ContentPresenter > ContentControl > ContentPresenter > StackPanel > SystemSettings.View.StableComboBox", {
        L"Height=28"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > UserControl > Grid > Grid > StackPanel#VerticalAlignedPanel > ContentControl > ContentPresenter > StackPanel > ItemsControl > ItemsPresenter > SystemSettings.View.SettingsWrapPanel > ContentPresenter > ContentControl > ContentPresenter > StackPanel > FontIcon", {
        L"Margin=12,4,0,3"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > UserControl > Grid > Grid > StackPanel#VerticalAlignedPanel > ContentControl > ContentPresenter > StackPanel > ItemsControl > ItemsPresenter > SystemSettings.View.SettingsWrapPanel > ContentPresenter > ContentControl > ContentPresenter > StackPanel > TextBlock#Label", {
        L"Margin=2,3,3,2"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > UserControl > Grid > Grid > StackPanel#VerticalAlignedPanel > ContentControl > ContentPresenter > StackPanel > ItemsControl > ItemsPresenter > SystemSettings.View.SettingsWrapPanel > ContentPresenter > ContentControl > ContentPresenter > StackPanel > SystemSettings.View.StableComboBox > Grid#LayoutRoot > Border#HighlightBackground", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > UserControl > Grid > Grid > StackPanel#VerticalAlignedPanel > ContentControl > ContentPresenter > Grid > StackPanel", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem", {
        L"Height=52"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem", {
        L"MinHeight=90"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent > TextBlock", {
        L"Foreground:=$SubTitleCol"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent > StackPanel > TextBlock", {
        L"Foreground:=$SubTitleCol"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent > SystemSettings.View.AlignableContentControl > SystemSettings.View.ExpandItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel", {
        L"Orientation=1"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent > SystemSettings.View.AlignableContentControl > SystemSettings.View.ExpandItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent", {
        L"Margin:=$SubTitleMrg",
        L"Foreground:=$SubTitleCol",
        L"FontSize:=$SubTitleFontSz"}},
    ThemeTargetStyles{L"ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent > StackPanel > TextBlock", {
        L"FontSize:=$SubTitleFontSz"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent > SystemSettings.View.AlignableContentControl > SystemSettings.View.ExpandItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem > Grid", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem > Grid", {
        L"Padding=1,0,0,0"}},
    ThemeTargetStyles{L"StackPanel > ContentPresenter > SystemSettings.View.EntityItem", {
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent > SystemSettings.View.AlignableContentControl > SystemSettings.View.ExpandItemsControl > ItemsPresenter > StackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > Rectangle#IconPlaceholder", {
        L"Margin:=$Icon_Margin"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > ContentPresenter#IconContentPresenter", {
        L"Margin:=$Icon_Margin"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent > SystemSettings.View.AlignableContentControl > SystemSettings.View.ExpandItemsControl > StackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > Rectangle#IconPlaceholder", {
        L"Margin:=$Icon_Margin"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent > SystemSettings.View.AlignableContentControl > SystemSettings.View.ExpandItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > ContentPresenter#IconContentPresenter", {
        L"Margin:=$Icon_Margin"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter > Grid > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > ContentPresenter#IconContentPresenter", {
        L"Margin:=$Icon_Margin"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent > SystemSettings.View.AlignableContentControl > SystemSettings.View.ExpandItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem > Grid", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid", {
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter > Grid > ContentPresenter > SystemSettings.View.EntityItem#EntityListItemControl > Grid", {
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter > Grid > ContentPresenter > SystemSettings.View.EntityItem#EntityListItemControl > Grid", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter > Grid > ContentPresenter > SystemSettings.View.EntityItem#EntityListItemControl", {
        L"MinHeight=34"}},
    ThemeTargetStyles{L"ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter", {
        L"BorderThickness=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter > Grid > ContentPresenter > SystemSettings.View.EntityItem#EntityListItemControl > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel", {
        L"Orientation=1"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter > Grid > ContentPresenter > SystemSettings.View.EntityItem#EntityListItemControl > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent > TextBlock", {
        L"FontSize:=$SubTitleFontSz",
        L"Foreground:=$SubTitleCol",
        L"Margin:=$SubTitleMrg"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem", {
        L"MinHeight=35"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem", {
        L"MinHeight=35"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel", {
        L"GroupPadding=0,0,0,4"}},
    ThemeTargetStyles{L"ItemsStackPanel > ListViewHeaderItem", {
        L"Margin=0,0,0,2"}},
    ThemeTargetStyles{L"SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button#ClassicAppButton > ContentPresenter#ContentPresenter", {
        L"Padding=3,1,1,3"}},
    ThemeTargetStyles{L"SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button#ModernAppButton > ContentPresenter#ContentPresenter", {
        L"Padding=3,1,1,3"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid", {
        L"Padding=0,-3,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel", {
        L"VerticalAlignment=0",
        L"Spacing=-3"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent > SystemSettings.View.SpacingStackPanel > TextBlock", {
        L"Foreground:=$SubTitleCol"}},
    ThemeTargetStyles{L"SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > RichTextBlock", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.BorderedItemsControl > ItemsPresenter > StackPanel > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > Border", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.BorderedItemsControl > ItemsPresenter > StackPanel > ContentPresenter > SystemSettings.View.SettingsListView > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#ListViewItemPresenter > SystemSettings.View.EntityItem > Grid", {
        L"VerticalAlignment:=0",
        L"Padding=1,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#OneSegmentHeroEntityItemUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > SystemSettings.View.EntityItem > Grid > ContentPresenter#IconContentPresenter", {
        L"MinWidth=48",
        L"MinHeight=48",
        L"Width=48",
        L"Height=48",
        L"Padding=0,0,0,0",
        L"Margin=0,0,10,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#OneSegmentHeroEntityItemUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > SystemSettings.View.EntityItem > Grid > ContentPresenter#IconContentPresenter > Grid > ContentControl > Grid > TextBlock", {
        L"FontSize=48"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#OneSegmentHeroEntityItemUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > SystemSettings.View.EntityItem > Grid > ContentPresenter#IconContentPresenter > Grid > ContentControl > Grid > TextBlock#BadgeBackground", {
        L"FontSize=48"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#OneSegmentHeroEntityItemUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > SystemSettings.View.EntityItem > Grid > ContentPresenter#IconContentPresenter > Grid > ContentControl > Grid > TextBlock#BadgeContent", {
        L"FontSize=48"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel", {
        L"Spacing=16"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent > ContentPresenter > SystemSettings.View.AlignableContentControl#PermanentNavViewHeaderAlignControl > ContentPresenter > Microsoft.UI.Xaml.Controls.BreadcrumbBar#PermanentNavigationViewBreadcrumbBar > Microsoft.UI.Xaml.Controls.ItemsRepeater#PART_ItemsRepeater > Microsoft.UI.Xaml.Controls.BreadcrumbBarItem > Grid#PART_LayoutRoot > ContentPresenter#PART_LastItemContentPresenter", {
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > StackPanel > ContentPresenter#SubtitleContent", {
        L"Foreground:=$SubTitleCol",
        L"Margin:=$SubTitleMrg",
        L"FontSize:=$SubTitleFontSz"}},
}, {
    L"ColDef=<ColumnDefinitionCollection><ColumnDefinition Width=\"180\"/><ColumnDefinition Width=\"*\"/></ColumnDefinitionCollection>",
    L"SubTitleCol=#DD000000",
    L"SubTitleFontSz=14",
    L"SubTitleMrg=15,0,0,0",
    L"ListItem_Hmin=24",
    L"ListItem_H=34",
    L"Icon_Margin=0,0,8,0",
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
        Wh_Log(L"Not initialized for thread %u", GetCurrentThreadId());
        return S_OK;
    }

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
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuxh = wux::Hosting;
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
};

using PropertyOverridesMaybeUnresolved =
    std::variant<UnresolvedRules, ResolvedRules>;

struct ElementCustomizationRules {
    ElementMatcher elementMatcher;
    std::vector<ElementMatcher> parentElementMatchers;
    PropertyOverridesMaybeUnresolved propertyOverrides;
};

std::vector<ElementCustomizationRules> g_elementsCustomizationRules;

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

std::unordered_map<InstanceHandle, ElementCustomizationState>
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

// Mod-global style variable registry. The struct mirrors the per-XamlRoot state
// used by the taskbar styler so the variable-resolution call paths stay aligned
// across the styler mods, but here all elements share one registry.
struct StyleVariableState {
    std::unordered_map<std::wstring, StyleVariableValue> variables;
    std::unordered_map<std::wstring, std::vector<StyleVariableConsumer>>
        consumers;
};

StyleVariableState g_styleVariableState;

StyleVariableState* GetStyleVariableState() {
    return &g_styleVariableState;
}

bool g_elementPropertyModifying;

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
    auto value = elementDo.ReadLocalValue(property);

    // Workaround for RowDefinitions/ColumnDefinitions.
    if (value == DependencyProperty::UnsetValue()) {
        if (auto grid = elementDo.try_as<Controls::Grid>()) {
            if (auto value2 = elementDo.GetValue(property)) {
                auto className = winrt::get_class_name(value2);
                if (className ==
                        L"Windows.UI.Xaml.Controls."
                        L"ColumnDefinitionCollection" ||
                    className ==
                        L"Windows.UI.Xaml.Controls.RowDefinitionCollection") {
                    Wh_Log(L"Using GetValue workaround for %s",
                           className.c_str());
                    value = std::move(value2);
                }
            }
        }
    }

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
                                     std::vector<std::wstring>* outDeps,
                                     StyleVariableState* state)
        : m_text(text), m_outDeps(outDeps), m_state(state) {}

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

    double ParseExpression() { return ParseTernary(); }

    // Conditional operator `cond ? thenVal : elseVal`, right-associative.
    // Short-circuit: only the taken branch is evaluated. The untaken branch is
    // still parsed (to advance the position and enforce syntax) with m_live
    // cleared, which suppresses value-level errors (division by zero, a
    // non-numeric / undefined variable, an unknown function) and dependency
    // capture for that branch.
    double ParseTernary() {
        double cond = ParseEquality();
        if (!ConsumeChar(L'?')) {
            return cond;
        }
        bool condTrue = cond != 0.0;
        bool prevLive = m_live;

        m_live = prevLive && condTrue;
        double thenVal = ParseExpression();
        m_live = prevLive;

        if (!ConsumeChar(L':')) {
            throw std::runtime_error(
                "Missing ':' for '?' in style variable expression");
        }

        m_live = prevLive && !condTrue;
        double elseVal = ParseTernary();
        m_live = prevLive;

        return condTrue ? thenVal : elseVal;
    }

    double ParseEquality() {
        double v = ParseRelational();
        while (true) {
            if (ConsumeOperator(L"==")) {
                v = (v == ParseRelational()) ? 1.0 : 0.0;
            } else if (ConsumeOperator(L"!=")) {
                v = (v != ParseRelational()) ? 1.0 : 0.0;
            } else {
                break;
            }
        }
        return v;
    }

    double ParseRelational() {
        double v = ParseAdditive();
        while (true) {
            // Match the two-char operators before their single-char prefixes.
            if (ConsumeOperator(L"<=")) {
                v = (v <= ParseAdditive()) ? 1.0 : 0.0;
            } else if (ConsumeOperator(L">=")) {
                v = (v >= ParseAdditive()) ? 1.0 : 0.0;
            } else if (ConsumeOperator(L"<")) {
                v = (v < ParseAdditive()) ? 1.0 : 0.0;
            } else if (ConsumeOperator(L">")) {
                v = (v > ParseAdditive()) ? 1.0 : 0.0;
            } else {
                break;
            }
        }
        return v;
    }

    double ParseAdditive() {
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
                    if (m_live) {
                        throw std::runtime_error(
                            "Division by zero in style variable expression");
                    }
                    // Dead ternary branch: the result is discarded, so skip the
                    // divide instead of throwing or producing inf/nan.
                } else {
                    v /= rhs;
                }
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
            if (m_live) {
                throw std::runtime_error(
                    "Unknown function in style variable expression");
            }
            // Dead ternary branch: value discarded, don't fail on the name.
            return 0.0;
        }
        return LookupVariableNumeric(std::wstring(ident));
    }

    double LookupVariableNumeric(const std::wstring& name) {
        // In a dead ternary branch (m_live == false) the value is discarded, so
        // suppress dependency capture and the value-level errors below; the
        // branch must not abort the whole expression.
        if (m_live && m_outDeps) {
            m_outDeps->push_back(name);
        }
        auto it = m_state->variables.find(name);
        if (it == m_state->variables.end()) {
            if (m_live) {
                Wh_Log(L"Style variable '%s' not yet defined; treating as 0",
                       name.c_str());
            }
            return 0.0;
        }
        if (!it->second.numeric) {
            if (m_live) {
                throw std::runtime_error(
                    "Style variable used in arithmetic is not numeric");
            }
            return 0.0;
        }
        return *it->second.numeric;
    }

    std::wstring_view m_text;
    std::vector<std::wstring>* m_outDeps;
    StyleVariableState* m_state;
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
// rather than substituting a value that won't parse.
std::optional<std::wstring> EvaluateStyleVariableExpression(
    std::wstring_view exprText,
    std::vector<std::wstring>* outDeps,
    StyleVariableState* state) {
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
        auto it = state->variables.find(name);
        if (it == state->variables.end()) {
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
        StyleVariableExpressionEvaluator eval(trimmed, outDeps, state);
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
    std::vector<std::wstring>* outDeps,
    StyleVariableState* state) {
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
        auto expanded =
            EvaluateStyleVariableExpression(exprText, outDeps, state);
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
void UpdateStyleVariableConsumers(StyleVariableState* state,
                                  InstanceHandle handle,
                                  DependencyProperty property,
                                  PCWSTR fallbackClassName,
                                  const std::vector<std::wstring>& oldDeps,
                                  const std::vector<std::wstring>& newDeps) {
    if (!state) {
        // The element's XamlRoot has already been destroyed (or was never
        // available); the StyleVariableState entry has been or will be reaped,
        // and there is nothing to clean up. New registrations (newDeps) are
        // also dropped on the floor: without a state we cannot route
        // propagations anyway.
        return;
    }

    for (const auto& dep : oldDeps) {
        auto it = state->consumers.find(dep);
        if (it == state->consumers.end()) {
            continue;
        }
        auto& consumers = it->second;
        std::erase_if(consumers, [&](const StyleVariableConsumer& c) {
            return c.elementHandle == handle && c.property == property;
        });
        if (consumers.empty()) {
            state->consumers.erase(it);
        }
    }

    std::wstring fallbackClassNameStr =
        fallbackClassName ? fallbackClassName : L"";
    for (const auto& dep : newDeps) {
        auto& consumers = state->consumers[dep];
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
// Updates the (handle, property) -> state->consumers registry to match the
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
// Returns std::nullopt if the state has no template, expansion failed, or XAML
// resolution failed.
std::optional<PropertyOverrideValue> ResolveDynamicStyleValue(
    StyleVariableState* state,
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
    auto expanded = ExpandStyleVariables(tmpl.rawValue, &newDeps, state);

    UpdateStyleVariableConsumers(
        state, handle, property, fallbackClassName,
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
void PropagateStyleVariableChange(StyleVariableState* state,
                                  const std::wstring& varName) {
    auto consumersIt = state->consumers.find(varName);
    if (consumersIt == state->consumers.end()) {
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
                state, consumer.elementHandle, element, consumer.property,
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

// Compare a captured value to whatever's currently in state->variables for the
// same name; if different, store and notify dependents. Each consumer's own
// fallbackClassName lives on the consumer entry, so this function does not need
// to be told the capturer's context. Used by every path that wants to publish a
// captured value -- the per-property capture callback, the SizeChanged
// catch-all, and the initial seeding loop -- so the no-op fast path applies
// uniformly.
void SetStyleVariableIfChangedAndPropagate(StyleVariableState* state,
                                           const std::wstring& varName,
                                           StyleVariableValue value) {
    auto it = state->variables.find(varName);
    if (it != state->variables.end() &&
        it->second.stringForm == value.stringForm &&
        it->second.numeric == value.numeric &&
        it->second.substitutable == value.substitutable) {
        Wh_Log(L"Style variable '%s' unchanged at '%s'", varName.c_str(),
               value.stringForm.c_str());
        return;
    }

    Wh_Log(L"Style variable '%s' changed: '%s' -> '%s'", varName.c_str(),
           it != state->variables.end() ? it->second.stringForm.c_str()
                                        : L"(unset)",
           value.stringForm.c_str());
    state->variables[varName] = std::move(value);
    PropagateStyleVariableChange(state, varName);
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
// multiple variables from this element) and then propagates only the variables
// whose values actually changed -- the no-op fast path matches the one used by
// the change-driven callbacks below. The function does not need the capturer's
// fallbackClassName: each StyleVariableConsumer entry already carries its own
// consumer-side fallback, so propagation routes through the right context per
// consumer.
void SetUpCapturesForElement(StyleVariableState* state,
                             InstanceHandle handle,
                             FrameworkElement element,
                             const std::vector<CaptureSpec>& captures,
                             ElementCustomizationState* elementState) {
    if (captures.empty()) {
        return;
    }

    auto elementDo = element.as<DependencyObject>();
    winrt::weak_ref<FrameworkElement> elementWeakRef = element;

    // Names of variables whose seeded value differs from whatever's already in
    // state->variables. Only these need a propagation pass at the end.
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

        auto existingIt = state->variables.find(capture.varName);
        const bool changed =
            existingIt == state->variables.end() ||
            existingIt->second.stringForm != value.stringForm ||
            existingIt->second.numeric != value.numeric ||
            existingIt->second.substitutable != value.substitutable;

        if (changed) {
            Wh_Log(
                L"Seeding capture variable '%s' from %s with value '%s' "
                L"(was: '%s')",
                capture.varName.c_str(), winrt::get_class_name(element).c_str(),
                value.stringForm.c_str(),
                existingIt != state->variables.end()
                    ? existingIt->second.stringForm.c_str()
                    : L"(unset)");
            state->variables[capture.varName] = std::move(value);
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
                [state, varName, elementWeakRef](DependencyObject sender,
                                                 DependencyProperty property) {
                    auto element = elementWeakRef.get();
                    if (!element) {
                        return;
                    }
                    auto value =
                        ReadCapturedStyleVariableValue(element, property);
                    SetStyleVariableIfChangedAndPropagate(state, varName,
                                                          std::move(value));
                });
    }

    if (!sizeChangedCaptures.empty()) {
        elementState->captureSizeChangedToken = element.SizeChanged(
            [state, elementWeakRef,
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
                    SetStyleVariableIfChangedAndPropagate(state, varName,
                                                          std::move(value));
                }
            });
    }

    // Propagate the freshly seeded values to any consumers that were already
    // registered before this element was matched. Variables whose value did not
    // actually change are skipped, matching the per-callback fast path.
    for (const auto& varName : changedVarNames) {
        PropagateStyleVariableChange(state, varName);
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
                    state, handle, element, property, fallbackClassName,
                    &propertyCustomizationState);
            } else {
                resolved = it->second;
            }

            if (resolved) {
                propertyCustomizationState.originalValue =
                    ReadLocalValueWithWorkaround(element, property);
                propertyCustomizationState.customValue = *resolved;
                SetOrClearValue(element, property, *resolved,
                                /*initialApply=*/true);
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
                [state, elementWeakRef, propertyOverrides, handle,
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
                                    state, handle, element, property,
                                    fallbackClassNamePtr,
                                    &propertyCustomizationState);
                            } else {
                                // Transitioning from dynamic to static for this
                                // visual state: clear template metadata and
                                // unregister consumer entries.
                                if (propertyCustomizationState
                                        .dynamicTemplate) {
                                    UpdateStyleVariableConsumers(
                                        state, handle, property,
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
                                    state, handle, property,
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
    StyleVariableState* state,
    InstanceHandle handle,
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
                UpdateStyleVariableConsumers(state, handle, property,
                                             /*fallbackClassName=*/nullptr,
                                             propState.variableDependencies,
                                             {});
            }

            if (propState.originalValue) {
                SetOrClearValue(element, property, *propState.originalValue);
            }
        }
    } else {
        // Element is gone; still clear consumer entries so a stale (handle,
        // property) pair isn't visited during PropagateStyleVariableChange.
        for (const auto& [property, propState] :
             elementCustomizationStateForVisualStateGroup
                 .propertyCustomizationStates) {
            if (!propState.variableDependencies.empty()) {
                UpdateStyleVariableConsumers(state, handle, property,
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

    auto& elementCustomizationState = g_elementsCustomizationState[handle];

    for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
        RestoreCustomizationsForVisualStateGroup(
            state, handle, element, visualStateGroupOptionalWeakPtrIter,
            stateIter);
    }

    elementCustomizationState.element = element;
    elementCustomizationState.perVisualStateGroup.clear();

    // Wire up captures first so any variables they define are visible to
    // dynamic value-rules applied below. Note: SetUpCapturesForElement does not
    // need this element's fallbackClassName -- propagation routes through each
    // consumer's own stored fallback.
    SetUpCapturesForElement(state, handle, element, resolved.captures,
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
            state, handle, element, visualStateGroup, fallbackClassName,
            std::move(overridesForVisualStateGroup),
            elementCustomizationStateForVisualStateGroup);
    }
}

void CleanupCustomizations(InstanceHandle handle) {
    if (auto it = g_elementsCustomizationState.find(handle);
        it != g_elementsCustomizationState.end()) {
        auto& elementCustomizationState = it->second;

        auto element = elementCustomizationState.element.get();
        auto* state = GetStyleVariableState();

        RestoreCapturesForElement(element, elementCustomizationState);

        for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
             elementCustomizationState.perVisualStateGroup) {
            RestoreCustomizationsForVisualStateGroup(
                state, handle, element, visualStateGroupOptionalWeakPtrIter,
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
    if (wcscmp(themeName, L"Densy") == 0) {
        theme = &g_themeDensy;
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

void UninitializeSettingsAndTap() {
    // Clear failed image brushes list for this thread (revokers will
    // automatically unregister).
    g_failedImageBrushesForThread.failedImageBrushes.clear();
    g_failedImageBrushesForThread.dispatcher = nullptr;

    for (const auto& [handle, elementCustomizationState] :
         g_elementsCustomizationState) {
        auto element = elementCustomizationState.element.get();
        auto* state = GetStyleVariableState();

        RestoreCapturesForElement(element, elementCustomizationState);

        for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
             elementCustomizationState.perVisualStateGroup) {
            RestoreCustomizationsForVisualStateGroup(
                state, handle, element, visualStateGroupOptionalWeakPtrIter,
                stateIter);
        }
    }

    g_elementsCustomizationState.clear();
    g_styleVariableState = {};

    g_elementsCustomizationRules.clear();

    UninitializeResourceVariables();

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
    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        Wh_Log(L"Initializing - Created core window: %08X via %S",
               (DWORD)(ULONG_PTR)hWnd, funcName);
        InitializeSettingsAndTap();
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

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"ApplicationFrameWindow") != 0) {
                return TRUE;
            }

            // Look for a direct child CoreWindow belonging to this process.
            HWND hCoreWnd = nullptr;
            while ((hCoreWnd = FindWindowEx(hWnd, hCoreWnd,
                                            L"Windows.UI.Core.CoreWindow",
                                            nullptr)) != nullptr) {
                DWORD dwProcessId = 0;
                if (GetWindowThreadProcessId(hCoreWnd, &dwProcessId) &&
                    dwProcessId == GetCurrentProcessId()) {
                    *param.hWnd = hCoreWnd;
                    return FALSE;
                }
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

bool StartStatsTimer() {
    // No stats for this mod for now.
    return true;
}

void StopStatsTimer() {
    // No stats for this mod for now.
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

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

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Initializing - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { InitializeSettingsAndTap(); }, nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    StopStatsTimer();

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
