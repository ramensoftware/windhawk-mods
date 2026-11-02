// ==WindhawkMod==
// @id              windows-11-settings-styler
// @name            Windows 11 Settings Styler
// @description     Customize the Windows Settings app with themes contributed by others or create your own
// @version         1.2.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         SystemSettings.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshlwapi
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

[![Densy](https://raw.githubusercontent.com/ramensoftware/windows-11-settings-styling-guide/refs/heads/main/Themes/Densy/screenshot-small.png)
\
Densy](https://github.com/ramensoftware/windows-11-settings-styling-guide/blob/main/Themes/Densy/README.md)

[![ClassicSearchBar](https://raw.githubusercontent.com/ramensoftware/windows-11-settings-styling-guide/refs/heads/main/Themes/ClassicSearchBar/screenshot-small.png)
\
ClassicSearchBar](https://github.com/ramensoftware/windows-11-settings-styling-guide/blob/main/Themes/ClassicSearchBar/README.md)

[![StoreFrame11](https://raw.githubusercontent.com/ramensoftware/windows-11-settings-styling-guide/refs/heads/main/Themes/StoreFrame11/screenshot-small.png)
\
StoreFrame11](https://github.com/ramensoftware/windows-11-settings-styling-guide/blob/main/Themes/StoreFrame11/README.md)

[![Blue](https://raw.githubusercontent.com/ramensoftware/windows-11-settings-styling-guide/refs/heads/main/Themes/Blue/screenshot-small.png)
\
Blue](https://github.com/ramensoftware/windows-11-settings-styling-guide/blob/main/Themes/Blue/README.md)

[![Translucent
Settings11](https://raw.githubusercontent.com/ramensoftware/windows-11-settings-styling-guide/refs/heads/main/Themes/Translucent%20Settings11/screenshot-small.png)
\
Translucent
Settings11](https://github.com/ramensoftware/windows-11-settings-styling-guide/blob/main/Themes/Translucent%20Settings11/README.md)

[![WindowGlass](https://raw.githubusercontent.com/ramensoftware/windows-11-settings-styling-guide/refs/heads/main/Themes/WindowGlass/screenshot-small.png)
\
WindowGlass](https://github.com/ramensoftware/windows-11-settings-styling-guide/blob/main/Themes/WindowGlass/README.md)

[![OLED](https://raw.githubusercontent.com/ramensoftware/windows-11-settings-styling-guide/refs/heads/main/Themes/OLED/screenshot-small.png)
\
OLED](https://github.com/ramensoftware/windows-11-settings-styling-guide/blob/main/Themes/OLED/README.md)

[![Translucent
Seamless](https://raw.githubusercontent.com/ramensoftware/windows-11-settings-styling-guide/refs/heads/main/Themes/Translucent%20Seamless/screenshot-small.png)
\
Translucent
Seamless](https://github.com/ramensoftware/windows-11-settings-styling-guide/blob/main/Themes/Translucent%20Seamless/README.md)

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
  - ClassicSearchBar: ClassicSearchBar
  - StoreFrame11: StoreFrame11
  - Blue: Blue
  - Translucent_Settings11: Translucent Settings11
  - WindowGlass: WindowGlass
  - OLED_variant_ModrinthGreen: OLED (Modrinth Green)
  - OLED_variant_SystemAscent: OLED (System Ascent)
  - Translucent_Seamless: Translucent Seamless
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

const Theme g_themeClassicSearchBar = {{
    ThemeTargetStyles{L"Grid@DisplayModeStates > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid", {
        L"Margin=0,52,0,0",
        L"Margin@OpenOverlayLeft=-11,-15,0,0",
        L"Margin@Closed=-11,-15,0,0"}},
    ThemeTargetStyles{L"Grid#RootCommandSearchGrid", {
        L"Margin=15,133,0,0",
        L"HorizontalAlignment=0",
        L"Width=282",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsAutoSuggestCommandSearchBox#CommandSearchBox", {
        L"Width=282",
        L"CornerRadius=3"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox", {
        L"CornerRadius=3"}},
    ThemeTargetStyles{L"StackPanel#SettingsCommandSearchBoxBackground", {
        L"CornerRadius=3"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox > Grid > ScrollViewer", {
        L"Margin=-325,0,0,0",
        L"Width=232"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox > Grid > TextBlock#PlaceholderTextContentPresenter", {
        L"Margin=-325,0,0,0",
        L"Width=232"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox > Grid > Button#DeleteButton > Grid", {
        L"Margin=-100,5,5,5",
        L"Width=25",
        L"CornerRadius=3"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox > Grid > Button#QueryButton > ContentPresenter", {
        L"Margin=253,0,0,0"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox > Grid > Button#QueryButton > ContentPresenter > FontIcon > Grid > TextBlock", {
        L"FontSize=12",
        L"Opacity=0.8"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsAutoSuggestCommandSearchBox", {
        L"Width=282"}},
    ThemeTargetStyles{L"Button#CommandSearchBoxFlyoutButton", {
        L"Margin=140,-125,0,0"}},
}};

const Theme g_themeStoreFrame11 = {{
    ThemeTargetStyles{L"Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter", {
        L"Margin=2"}},
    ThemeTargetStyles{L"Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent", {
        L"Margin=10,-38,0,5"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter", {
        L"Margin=-12,8,0,0"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox > Grid > Button#DeleteButton > Grid#ButtonLayoutGrid", {
        L"CornerRadius=$InRadius",
        L"MinHeight=32"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox", {
        L"CornerRadius=$InRadius",
        L"MinHeight=32"}},
    ThemeTargetStyles{L"StackPanel#SettingsCommandSearchBoxBackground", {
        L"CornerRadius=$InRadius",
        L"MinHeight=32"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Grid.Column=0",
        L"Visibility=Hidden"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem", {
        L"MinHeight=48",
        L"MinWidth=65",
        L"ToolTipService.Placement=5",
        L"MaxWidth=65"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid@DisplayModeStates > Grid#PaneRoot", {
        L"MaxWidth@OpenInlineLeft=65",
        L"Grid.ColumnSpan@OpenInlineLeft=1",
        L"Grid.ColumnSpan=>Span"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid", {
        L"Background:=$BgOverlay",
        L"CornerRadius={{Span > 1 ? 0 : $OutRadius}},0,0,0",
        L"Margin={{Span > 1 ? 0 : 65}},48,0,0",
        L"BorderBrush:=$BgBorder",
        L"BorderThickness={{Span > 1 ? 0 : 1}},1,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid@DisplayModeStates > Grid#ContentRoot", {
        L"Grid.Column@OpenInlineLeft=0",
        L"Grid.ColumnSpan@OpenInlineLeft=3"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > Grid#ShadowCaster", {
        L"Grid.ColumnSpan=1",
        L"MaxWidth=65"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Padding=3,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"FontFamily=Segoe Fluent Icons",
        L"Foreground@Normal:=<SolidColorBrush Color=\"{ThemeResource TextFillColorSecondary}\" />",
        L"Foreground@PointerOver:=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" />",
        L"Foreground@Pressed:=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" />",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource Accent}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource Accent}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource Accent}\" />",
        L"FontSize=20",
        L"Margin=15,0,-2,0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[1] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE80F",
        L"Content@PointerOver:=\uE80F",
        L"Content@Pressed:=\uE80F",
        L"Content@Selected:=\uEA8A",
        L"Content@PointerOverSelected:=\uEA8A",
        L"Content@PressedSelected:=\uEA8A"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[2] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE75B",
        L"Content@PointerOver:=\uE75B",
        L"Content@Pressed:=\uE75B",
        L"Content@Selected:=$System",
        L"Content@PointerOverSelected:=$System",
        L"Content@PressedSelected:=$System"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[3] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uF0C5",
        L"Content@PointerOver:=\uF0C5",
        L"Content@Pressed:=\uF0C5",
        L"Content@Selected:=\uF0C5",
        L"Content@PointerOverSelected:=\uF0C5",
        L"Content@PressedSelected:=\uF0C5"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[4] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uEA18",
        L"Content@PointerOver:=\uEA18",
        L"Content@Pressed:=\uEA18",
        L"Content@Selected:=$Shield",
        L"Content@PointerOverSelected:=$Shield",
        L"Content@PressedSelected:=$Shield"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[5] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE776",
        L"Content@PointerOver:=\uE776",
        L"Content@Pressed:=\uE776",
        L"Content@Selected:=$EOA",
        L"Content@PointerOverSelected:=$EOA",
        L"Content@PressedSelected:=$EOA"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[6] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE7FC",
        L"Content@PointerOver:=\uE7FC",
        L"Content@Pressed:=\uE7FC",
        L"Content@Selected:=$Games",
        L"Content@PointerOverSelected:=$Games",
        L"Content@PressedSelected:=$Games"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[7] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE775",
        L"Content@PointerOver:=\uE775",
        L"Content@Pressed:=\uE775",
        L"Content@Selected:=\uE775",
        L"Content@PointerOverSelected:=\uE775",
        L"Content@PressedSelected:=\uE775"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[8] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE77B",
        L"Content@PointerOver:=\uE77B",
        L"Content@Pressed:=\uE77B",
        L"Content@Selected:=\uEA8C",
        L"Content@PointerOverSelected:=\uEA8C",
        L"Content@PressedSelected:=\uEA8C"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[9] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE74C",
        L"Content@PointerOver:=\uE74C",
        L"Content@Pressed:=\uE74C",
        L"Content@Selected:=$Apps",
        L"Content@PointerOverSelected:=$Apps",
        L"Content@PressedSelected:=$Apps"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[10] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE771",
        L"Content@PointerOver:=\uE771",
        L"Content@Pressed:=\uE771",
        L"Content@Selected:=$Personalize",
        L"Content@PointerOverSelected:=$Personalize",
        L"Content@PressedSelected:=$Personalize"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[11] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE701",
        L"Content@PointerOver:=\uE701",
        L"Content@Pressed:=\uE701",
        L"Content@Selected:=\uE701",
        L"Content@PointerOverSelected:=\uE701",
        L"Content@PressedSelected:=\uE701"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[12] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE702",
        L"Content@PointerOver:=\uE702",
        L"Content@Pressed:=\uE702",
        L"Content@Selected:=\uE702",
        L"Content@PointerOverSelected:=\uE702",
        L"Content@PressedSelected:=\uE702"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[1]", {
        L"Content=>t1",
        L"ToolTipService.ToolTip={{t1}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[2]", {
        L"Content=>t2",
        L"ToolTipService.ToolTip={{t2}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[3]", {
        L"Content=>t3",
        L"ToolTipService.ToolTip={{t3}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[4]", {
        L"Content=>t4",
        L"ToolTipService.ToolTip={{t4}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[5]", {
        L"Content=>t5",
        L"ToolTipService.ToolTip={{t5}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[6]", {
        L"Content=>t6",
        L"ToolTipService.ToolTip={{t6}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[7]", {
        L"Content=>t7",
        L"ToolTipService.ToolTip={{t7}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[8]", {
        L"Content=>t8",
        L"ToolTipService.ToolTip={{t8}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[9]", {
        L"Content=>t9",
        L"ToolTipService.ToolTip={{t9}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[10]", {
        L"Content=>t10",
        L"ToolTipService.ToolTip={{t10}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[11]", {
        L"Content=>t11",
        L"ToolTipService.ToolTip={{t11}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[12]", {
        L"Content=>t12",
        L"ToolTipService.ToolTip={{t12}}"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid[2]", {
        L"Visibility=1",
        L"Grid.Column=0"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid[2] > TextBlock#UserName", {
        L"Text=>UserName"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton", {
        L"ToolTipService.ToolTip={{UserName}}",
        L"ToolTipService.Placement=10",
        L"Visibility=1"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid#UserImageGrid > Image", {
        L"Width=30",
        L"Height=30"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel", {
        L"MaxHeight=48",
        L"MaxWidth=65",
        L"MinHeight=48",
        L"MinWidth=65",
        L"Visibility=1"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton", {
        L"MinHeight=48",
        L"MaxHeight=48",
        L"Margin=3,3,3,-3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ProgressBarIndicator", {
        L"RadiusX=3",
        L"RadiusY=3",
        L"Height=6",
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource Accent}\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#DeterminateRoot", {
        L"CornerRadius=3",
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ProgressBar", {
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#DeterminateRoot > Windows.UI.Xaml.Shapes.Rectangle#ProgressBarIndicator", {
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#DeterminateRoot", {
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar", {
        L"Height=16"}},
    ThemeTargetStyles{L"Rectangle#SelectionIndicator", {
        L"Height=22"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > Grid > Windows.UI.Xaml.Controls.Primitives.ScrollBar#VerticalScrollBar > Grid#Root > Grid#VerticalRoot > Windows.UI.Xaml.Controls.Primitives.Thumb#VerticalThumb", {
        L"Visibility=1",
        L"AllowFocusOnInteraction=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > Grid > Windows.UI.Xaml.Controls.Primitives.ScrollBar#VerticalScrollBar > Grid#Root > Grid#VerticalRoot > Rectangle#VerticalTrackRect", {
        L"Visibility=1",
        L"AllowFocusOnInteraction=0"}},
}, {
    L"OutRadius=8",
    L"InRadius=4",
    L"BgBorder=<SolidColorBrush Color=\"{ThemeResource Border}\" />",
    L"BgOverlay=<SolidColorBrush Color=\"{ThemeResource Overlay}\" />",
    L"Apps=<Viewbox Width=\"23\" Height=\"24\" Stretch=\"Uniform\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\"><PathIcon Data=\"M 701.82 117.346 C 697.748 117.467 693.689 118.045 689.707 119.363 C 686.011 120.586 686.199 121.121 685.371 121.738 C 684.544 122.355 683.762 122.997 682.863 123.758 C 681.066 125.279 678.863 127.249 676.199 129.695 C 670.872 134.588 663.758 141.342 655.4 149.4 C 638.685 165.517 617.042 186.82 595.473 208.322 C 573.903 229.824 552.419 251.512 536.031 268.395 C 527.837 276.836 520.925 284.066 515.857 289.535 C 513.323 292.27 511.257 294.554 509.654 296.416 C 508.051 298.278 507.44 298.054 505.416 302.299 C 500.14 313.365 499.903 325.866 504.924 337.061 C 506.011 339.484 506.472 339.735 507.086 340.531 C 507.699 341.327 508.358 342.117 509.135 343.021 C 510.688 344.83 512.695 347.053 515.166 349.725 C 520.107 355.068 526.872 362.166 534.91 370.473 C 550.986 387.086 572.123 408.501 593.408 429.807 C 614.693 451.112 636.114 472.295 652.762 488.438 C 661.085 496.509 668.207 503.314 673.582 508.297 C 676.27 510.788 678.511 512.816 680.334 514.389 C 682.157 515.961 681.819 516.515 686.131 518.574 C 697.413 523.963 710.413 523.966 721.701 518.584 C 725.946 516.56 725.722 515.949 727.584 514.346 C 729.446 512.743 731.73 510.677 734.465 508.143 C 739.934 503.075 747.164 496.163 755.605 487.969 C 772.488 471.581 794.176 450.097 815.678 428.527 C 837.18 406.958 858.483 385.315 874.6 368.6 C 882.658 360.242 889.412 353.128 894.305 347.801 C 896.751 345.137 898.721 342.934 900.242 341.137 C 901.003 340.238 901.645 339.456 902.262 338.629 C 902.879 337.801 903.414 337.989 904.637 334.293 C 908.15 323.676 907.715 312.437 902.945 302.287 C 900.915 297.968 900.328 298.261 898.756 296.428 C 897.184 294.594 895.16 292.348 892.676 289.658 C 887.707 284.278 880.926 277.162 872.881 268.848 C 856.791 252.219 835.675 230.836 814.42 209.58 C 793.164 188.325 771.781 167.209 755.152 151.119 C 746.838 143.074 739.722 136.293 734.342 131.324 C 731.652 128.84 729.406 126.816 727.572 125.244 C 725.739 123.672 726.032 123.085 721.713 121.055 C 715.369 118.073 708.609 117.143 701.82 117.346 z M 153.568 181.342 A 11.5012 11.5012 0 0 0 148.701 182.41 L 142.982 185.064 C 133.395 189.515 125.608 197.159 120.93 206.652 L 118.4 211.783 A 11.5012 11.5012 0 0 0 117.217 216.844 L 116.918 364.41 L 116.619 511.977 A 11.5012 11.5012 0 0 0 128.119 523.5 L 287.701 523.5 L 447.283 523.5 A 11.5012 11.5012 0 0 0 458.783 512 L 458.783 365.469 C 458.783 284.201 458.926 246.394 458.025 226.781 C 457.575 216.975 457.064 211.545 455.137 206.373 C 453.209 201.201 449.922 198.208 448.611 196.678 C 445.121 192.6 441.184 189.301 436.559 186.582 L 431.352 183.521 A 11.5012 11.5012 0 0 0 425.549 181.936 L 289.559 181.639 L 153.568 181.342 z M 128.16 565.217 A 11.5012 11.5012 0 0 0 116.66 576.717 L 116.66 720.604 C 116.66 770.286 116.754 806.709 116.953 831.252 C 117.052 843.523 117.177 852.818 117.332 859.371 C 117.41 862.648 117.493 865.231 117.592 867.234 C 117.691 869.237 117.552 869.871 118.127 872.553 C 121.534 888.438 133.659 901.128 149.357 905.348 C 153.656 906.503 155.533 906.25 161.027 906.449 C 166.521 906.648 174.437 906.797 185.803 906.912 C 208.533 907.143 245.012 907.236 302.5 907.266 L 447.277 907.34 A 11.5012 11.5012 0 0 0 458.783 895.84 L 458.783 736.279 L 458.783 576.717 A 11.5012 11.5012 0 0 0 447.283 565.217 L 287.721 565.217 L 128.16 565.217 z M 512 565.217 A 11.5012 11.5012 0 0 0 500.5 576.717 L 500.5 736.299 L 500.5 895.881 A 11.5012 11.5012 0 0 0 512.023 907.381 L 659.59 907.082 L 807.156 906.783 A 11.5012 11.5012 0 0 0 812.217 905.6 L 817.348 903.07 C 826.841 898.392 834.485 890.605 838.936 881.018 L 841.59 875.299 A 11.5012 11.5012 0 0 0 842.658 870.432 L 842.361 734.441 L 842.064 598.451 A 11.5012 11.5012 0 0 0 840.479 592.648 L 837.418 587.441 C 834.699 582.816 831.4 578.879 827.322 575.389 C 825.792 574.078 822.799 570.791 817.627 568.863 C 812.455 566.936 807.025 566.425 797.219 565.975 C 777.606 565.074 739.799 565.217 658.531 565.217 L 512 565.217 z\"/></Viewbox>",
    L"Games=<Viewbox Width=\"20\" Height=\"19\" Stretch=\"Uniform\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\"><PathIcon Data=\"M 110.623 -120.279 C 108.722 -120.279 107.182 -118.738 107.182 -116.838 C 107.182 -114.937 108.722 -113.397 110.623 -113.396 C 112.524 -113.397 114.064 -114.937 114.064 -116.838 C 114.064 -118.738 112.524 -120.279 110.623 -120.279 z M 305.123 -0.257812 C 167.608 13.434 82.6439 103.935 70.6387 121.48 C -18.1232 251.204 5.34591 377.952 9.46094 405.965 C 14.2145 438.325 23.5579 468.04 35.5195 497.795 C 36.3747 500.138 37.2149 502.476 38.2715 504.729 C 41.1966 511.663 43.1933 518.587 46.5469 525.582 C 102.179 641.626 223.481 703.656 295.424 703.641 C 361.645 703.627 333.315 703.922 385.943 703.754 L 483.369 703.443 C 487.759 703.429 533.389 703.621 559.621 703.33 C 610.38 702.768 704.928 703.73 744.312 703.658 C 792.883 703.57 821.111 687.608 852.68 669.836 C 875.445 657.02 911.431 621.342 929.621 602.498 C 997.965 531.697 1009.32 480.423 1019.89 408.557 C 1033.99 312.717 1011.11 212.921 956.035 130.078 C 907.975 57.784 808.161 0.292007 721.553 0.898438 C 718.898 0.917027 672.689 1.21954 620.865 1.07617 C 567.201 0.927722 474.093 0.0141719 441.496 -0.173828 C 356.088 -0.666408 317.409 -1.48108 305.123 -0.257812 z M 771.555 191.719 C 774.683 191.893 777.829 192.292 780.969 192.926 C 790.48 194.845 803.848 201.515 810.764 207.795 C 820.501 216.636 827.485 228.242 830.574 240.713 C 832.6 248.891 832.473 263.957 830.305 272.434 C 824.274 296.014 804.459 314.608 780.471 319.197 C 756.47 323.789 731.256 314.645 717.42 296.33 C 708.082 283.969 704 271.704 704 256 C 704 240.687 707.211 230.454 715.83 218.301 C 728.569 200.339 749.654 190.502 771.555 191.719 z M 288 192 C 295.184 192 296.474 192.27 301.758 194.871 C 309.059 198.465 315.646 205.414 318.186 212.201 C 319.945 216.905 320 218.616 320 268.279 L 320 319.508 L 371.965 319.754 L 423.93 320 L 429.76 322.871 C 441.609 328.706 448 338.912 448 352 C 448 365.088 441.609 375.294 429.76 381.129 L 423.93 384 L 371.965 384.246 L 320 384.492 L 320 435.721 C 320 485.384 319.945 487.095 318.186 491.799 C 315.669 498.526 309.053 505.538 301.902 509.059 C 297.308 511.32 294.922 511.912 289.479 512.141 C 278.806 512.589 270.725 509.071 263.508 500.828 C 255.758 491.977 256 494.071 256 435.721 L 256 384.492 L 204.035 384.246 L 152.07 384 L 146.209 381.115 C 139.049 377.59 132.285 370.4 129.75 363.623 C 127.338 357.177 127.366 346.746 129.814 340.201 C 132.353 333.415 138.941 326.465 146.24 322.871 L 152.07 320 L 204.035 319.754 L 256 319.508 L 256 268.279 C 256 218.616 256.055 216.905 257.814 212.201 C 260.354 205.414 266.941 198.465 274.242 194.871 C 279.526 192.27 280.816 192 288 192 z M 643.555 383.719 C 646.683 383.893 649.829 384.292 652.969 384.926 C 662.48 386.845 675.848 393.515 682.764 399.795 C 692.501 408.636 699.485 420.242 702.574 432.713 C 704.6 440.891 704.473 455.957 702.305 464.434 C 696.274 488.014 676.459 506.608 652.471 511.197 C 628.47 515.789 603.256 506.645 589.42 488.33 C 580.082 475.969 576 463.704 576 448 C 576 432.687 579.211 422.454 587.83 410.301 C 600.569 392.339 621.654 382.502 643.555 383.719 z\"/></Viewbox>",
    L"System=<Viewbox Width=\"20\" Height=\"20\" Stretch=\"Uniform\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\"><PathIcon Data=\"M20,20z M0,0z M19.98,15C19.98,16.38,18.87,17.49,17.49,17.49L2.52,17.49C1.13,17.49,0.02,16.38,0.02,15L0.02,13.74 19.98,13.74 19.98,15z M17.49,2.52C18.87,2.52,19.98,3.63,19.98,5.02L19.98,12.5 0.02,12.5 0.02,5.02C0.02,3.63,1.13,2.52,2.52,2.52L17.49,2.52z\" /> </Viewbox>",
    L"EOA=<Viewbox Width=\"22\" Height=\"22\" Stretch=\"Uniform\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\"> <PathIcon Data=\"M 508.264 -2.78711 C 482.48 -2.38051 456.899 5.27177 434.363 19.25 C 411.663 33.3305 393.168 53.9702 381.559 78.0625 C 373.199 95.4108 368.797 115.928 368.797 135.701 C 368.797 156.54 374.009 178.356 383.455 196.812 C 403.651 236.272 441.825 263.614 485.664 271.922 C 506.828 275.933 524.641 275.015 545.039 269.947 C 585.225 259.963 619.384 233.474 638.006 196.609 C 657.267 158.479 657.094 112.168 637.654 74.0801 C 631.073 61.1858 621.639 48.6144 610.73 38.2207 C 602.112 30.0087 596.422 24.1766 583.039 16.6523 C 559.843 3.61147 534.048 -3.19371 508.264 -2.78711 z M 862.893 184.939 C 856.391 184.757 849.697 185.292 842.996 186.67 C 836.445 188.017 837.18 188.193 835.078 188.873 C 832.976 189.553 830.525 190.377 827.58 191.383 C 821.69 193.395 813.904 196.119 804.617 199.408 C 786.044 205.986 761.534 214.807 735.414 224.34 C 625.643 264.403 574.992 282.988 550.395 291.314 C 525.797 299.641 534.443 296.921 519.488 298.523 C 509.6 299.583 492.797 298.214 481.875 295.168 C 477.612 293.979 441.212 281.228 271.676 219.029 L 187.371 188.102 A 43.33 43.33 0 0 0 172.641 185.451 L 160.693 185.398 A 43.33 43.33 0 0 0 160.688 185.398 C 149.063 185.348 132.412 188.432 123.289 192.508 C 103.381 201.402 88.6472 214.214 78.1758 234.518 C 65.679 258.749 64.4826 280.321 73.8359 305.748 C 80.7765 324.616 92.2513 339.48 109.543 350.604 C 116.295 354.947 116.591 354.434 118.928 355.453 C 121.264 356.472 123.808 357.52 126.854 358.742 C 132.945 361.187 140.94 364.276 150.877 368.035 C 170.75 375.554 198.282 385.709 231.742 397.85 C 261.222 408.546 288.576 418.492 308.881 425.893 C 319.033 429.593 327.426 432.657 333.418 434.854 C 333.937 435.044 333.694 434.955 334.213 435.146 L 334.225 517.873 L 334.238 608.219 L 279.688 752 C 252.253 824.311 237.945 862.097 230.086 884.016 C 222.227 905.935 219.452 918.552 219.057 926.65 C 218.071 946.846 225.23 969.974 237.912 985.961 C 252.209 1003.98 265.351 1012.2 287.264 1017.75 C 294.209 1019.51 296.788 1020.77 308.605 1021 C 327.634 1021.38 335.306 1019.42 351.803 1011.41 C 370.673 1002.26 378.857 995.295 390.09 976.664 C 396.22 966.497 393.56 969.797 394.537 967.459 C 395.514 965.121 396.679 962.25 398.094 958.715 C 400.922 951.645 404.709 942.01 409.24 930.361 C 418.303 907.064 430.319 875.777 443.156 841.992 C 455.93 808.375 467.939 777.063 476.961 753.785 C 481.472 742.146 485.242 732.504 487.949 725.68 C 489.058 722.886 489.85 720.928 490.555 719.195 C 491.355 718.309 493.535 715.082 493.738 714.887 C 498.697 710.132 499.791 709.221 510.963 709.221 C 523.067 709.221 526.643 711.912 529.861 717.336 C 529.99 717.606 530.093 717.774 530.195 718.018 C 530.948 719.809 532.084 722.584 533.482 726.059 C 536.279 733.008 540.156 742.827 544.773 754.65 C 554.008 778.298 566.229 810.028 579.148 843.986 C 604.897 911.664 617.102 944.243 626.404 964.721 C 631.055 974.959 635.175 983.168 642.365 991.516 C 649.555 999.863 658.268 1005.02 661.129 1006.78 C 679.667 1018.19 697.574 1022.6 719.625 1020.39 C 775.186 1014.82 816.131 961.053 799.422 905.939 C 796.732 897.069 796.412 897.333 793.262 888.871 C 790.111 880.409 785.742 868.765 780.451 854.725 C 769.87 826.644 755.612 789.005 740.207 748.527 L 687.076 608.922 L 687.076 517.594 C 687.076 471.723 687.134 448.049 687.408 435.289 C 693.033 433.196 700.141 430.568 709.266 427.215 C 729.245 419.873 756.405 409.956 785.855 399.26 C 815.411 388.525 842.871 378.475 863.377 370.904 C 873.63 367.119 882.135 363.96 888.369 361.613 C 891.486 360.44 894.022 359.475 896.037 358.693 C 898.052 357.912 896.87 358.736 902.672 355.898 C 936.632 339.286 956.576 304.048 952.521 266.188 C 947.406 218.421 908.487 186.216 862.893 184.939 z\"/></Viewbox>",
    L"Personalize=<Viewbox Width=\"20\" Height=\"20\" Stretch=\"Uniform\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\"><PathIcon Data=\"M 958.535 3.11133 C 947.18 3.40174 936.456 6.75756 926.732 11.9199 C 919.819 15.5902 919.638 16.4173 915.17 19.6562 C 910.702 22.8952 904.961 27.1533 898.178 32.2422 C 884.611 42.42 866.933 55.8853 847.975 70.459 C 810.057 99.6063 767.179 133.055 742.133 153.318 C 567.359 294.715 420.316 441.078 266.635 626.514 L 259.141 635.559 A 29.2899 29.2899 0 0 0 272.189 681.949 L 281.182 685.033 L 281.182 685.031 C 288.912 687.682 309.26 697.471 313.463 700.16 C 335.515 714.267 358.805 740.807 368.4 761.658 C 369.9 764.917 371.347 767.77 373.078 770.67 C 373.944 772.12 374.729 773.475 376.58 775.785 C 377.506 776.94 378.488 778.328 381.561 780.789 C 383.097 782.02 385.139 783.646 389.311 785.287 C 393.482 786.928 401.081 788.714 409.732 785.721 C 419.007 782.512 416.67 782.253 417.371 781.748 C 418.073 781.243 418.392 780.985 418.688 780.75 C 419.279 780.28 419.652 779.966 420.074 779.609 C 420.918 778.897 421.842 778.095 422.984 777.096 C 425.269 775.097 428.353 772.364 432.105 769.016 C 439.611 762.318 449.745 753.198 460.771 743.205 C 610.252 607.74 756.081 446.936 876.355 284.943 C 899.082 254.334 931.73 209.162 959.32 170.453 C 973.116 151.099 985.628 133.384 995.021 119.887 C 999.718 113.138 1003.63 107.455 1006.59 103.043 C 1009.56 98.6306 1010.38 98.308 1013.65 91.6895 C 1021.34 76.1408 1023.96 57.418 1017.27 40.5703 C 1010.58 23.7227 995.468 11.7423 979.006 6.16992 C 972.352 3.91765 965.35 2.93705 958.535 3.11133 z M 227.619 688.389 C 220.3 688.583 213.027 689.412 205.715 690.725 C 159.436 699.035 119.738 722.907 100.512 760.834 C 92.4502 776.736 88.8984 794.262 85.8086 823.357 C 81.2234 866.531 67.1948 903.296 51.2051 923.285 C 51.4618 922.964 39.4669 935.514 29.8105 944.547 C 24.9824 949.063 20.2705 953.329 16.9844 956.162 C 15.6312 957.329 14.7842 958.009 14.2051 958.471 C 5.21643 963.512 -1.52148 972.908 -1.52148 984.879 C -1.52148 1000.89 7.7536 1006.69 11.125 1009.04 C 14.4964 1011.39 16.1829 1011.92 17.5312 1012.44 C 20.2279 1013.49 21.4213 1013.71 22.6289 1013.99 C 25.0441 1014.56 26.9036 1014.85 29.084 1015.17 C 33.4448 1015.82 38.6249 1016.4 44.375 1016.89 C 49.5293 1017.33 54.5536 1017.79 58.3984 1018.17 C 62.2433 1018.54 67.4712 1019.32 63.9336 1018.71 C 72.9013 1020.25 74.2123 1019.47 79.9688 1019.4 C 85.7252 1019.34 92.6154 1019.13 99.8008 1018.84 C 114.172 1018.24 128.83 1017.41 138.795 1016.16 C 186.115 1010.2 230.832 997.902 268.658 979.898 C 315.499 957.604 351.523 922.052 364.738 878.094 C 372.42 852.541 371.484 824.144 365.244 798.35 C 354.927 755.7 330.055 720.599 293.408 703.814 C 270.846 693.481 249.57 687.807 227.619 688.389 z M 384.748 732.719 C 384.67 732.787 384.499 732.94 384.424 733.006 C 383.875 733.486 383.638 733.686 383.256 734.016 C 383.318 733.791 383.273 733.501 384.748 732.719 z M 29.0664 954.396 C 25.3372 954.396 21.6268 955.156 18.1309 956.521 C 19.694 955.658 21.8408 954.396 29.0664 954.396 z\"/></Viewbox>",
    L"Shield=<Viewbox Width=\"20\" Height=\"20\" Stretch=\"Uniform\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\"><PathIcon Data=\"M 512 0 C 508.333 0 505.167 0.5 502.5 1.5 C 499.833 2.5 497 3.83337 494 5.5 C 471 19.8334 448.333 33.3334 426 46 C 403.667 58.6667 381.083 70.3334 358.25 81 C 335.417 91.6667 312 101.5 288 110.5 C 264 119.5 239 127.667 213 135 C 200.333 138.667 187.75 141.917 175.25 144.75 C 162.75 147.583 150.167 150.167 137.5 152.5 C 130.167 154.167 122.5 155.417 114.5 156.25 C 106.5 157.083 98.8333 158.5 91.5 160.5 C 83.5 162.833 76.9167 166.333 71.75 171 C 66.5833 175.667 64 182.667 64 192 L 64 480 C 64 550 75.1667 613.167 97.5 669.5 C 119.833 725.833 150.583 776.25 189.75 820.75 C 228.917 865.25 275.083 904.083 328.25 937.25 C 381.417 970.417 438.833 998.667 500.5 1022 C 502.167 1022.67 504.083 1023.17 506.25 1023.5 C 508.417 1023.83 510.333 1024 512 1024 C 516.667 1024 520.5 1023.33 523.5 1022 C 585.167 998.333 642.583 970 695.75 937 C 748.917 904 795.083 865.417 834.25 821.25 C 873.417 777.083 904.167 726.75 926.5 670.25 C 948.833 613.75 960 550.333 960 480 L 960 192 C 960 182.667 957.417 175.667 952.25 171 C 947.083 166.333 940.5 162.833 932.5 160.5 C 925.167 158.5 917.5 157.083 909.5 156.25 C 901.5 155.417 893.833 154.167 886.5 152.5 C 873.833 150.167 861.25 147.583 848.75 144.75 C 836.25 141.917 823.667 138.667 811 135 C 785 127.667 760 119.5 736 110.5 C 712 101.5 688.583 91.6667 665.75 81 C 642.917 70.3334 620.333 58.6667 598 46 C 575.667 33.3334 553 19.8334 530 5.5 C 527 3.83337 524.167 2.5 521.5 1.5 C 518.833 0.5 515.667 0 512 0 z\"/></Viewbox>",
}, {
    L"Overlay@Light=#55FFFFFF",
    L"Overlay@Dark=#09FFFFFF",
    L"Border@Light=#0F000000",
    L"Border@Dark=#19000000",
    L"Accent@Dark={ThemeResource SystemAccentColorLight2}",
    L"Accent@Light={ThemeResource SystemAccentColorDark1}",
}};

const Theme g_themeBlue = {{
    ThemeTargetStyles{L"Frame#PermanentNavRootFrame", {
        L"Background=#03A5FC"}},
    ThemeTargetStyles{L"SystemSettings.View.EntityItem", {
        L"Background=#0373FC",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView", {
        L"Background=#03ADFC",
        L"Foreground=White"}},
    ThemeTargetStyles{L"ContentPresenter", {
        L"Foreground=Black"}},
    ThemeTargetStyles{L"ContentPresenter#SubtitleContent", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"StackPanel#BackgroundStackPanel", {
        L"Background=Blue"}},
    ThemeTargetStyles{L"TextBlock#TitleContent", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"ContentPresenter#TitleContent", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"ContentPresenter#IconContentPresenter", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Button#ContainerButton", {
        L"Background=Blue"}},
    ThemeTargetStyles{L"SystemSettings.View.ReactNativeExperienceViewControl", {
        L"Background=Blue"}},
    ThemeTargetStyles{L"SystemSettings.View.TwoSegmentsHeroUserControl#OneSegmentHeroEntityItemUserControl", {
        L"Background=DeepSkyBlue"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter > TextBlock", {
        L"Foreground=White"}},
}};

const Theme g_themeTranslucent_Settings11 = {{
    ThemeTargetStyles{L"ContentPresenter#IconContentPresenter", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter", {
        L"CornerRadius:=12,12,12,12"}},
    ThemeTargetStyles{L"SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid", {
        L"CornerRadius:=12"}},
    ThemeTargetStyles{L"SystemSettings.View.EntityItem#BluetoothRadioToggleEntityItem > Grid", {
        L"CornerRadius:=12"}},
    ThemeTargetStyles{L"SystemSettings.View.TwoSegmentsHeroUserControl#DefaultOneSegmentHeroUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > Button > ContentPresenter#ContentPresenter", {
        L"CornerRadius:=12",
        L"Width=250"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent", {
        L"CornerRadius:=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid", {
        L"CornerRadius:=$InRadius"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > Border", {
        L"CornerRadius:=$InRadius"}},
    ThemeTargetStyles{L"SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter", {
        L"CornerRadius:=$InRadius"}},
    ThemeTargetStyles{L"Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter", {
        L"Margin=2"}},
    ThemeTargetStyles{L"Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent", {
        L"Margin=10,-38,0,5"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter", {
        L"Margin=-12,8,0,0"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox > Grid > Button#DeleteButton > Grid#ButtonLayoutGrid", {
        L"CornerRadius=$InRadius",
        L"MinHeight=32"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox", {
        L"CornerRadius=$InRadius",
        L"MinHeight=32"}},
    ThemeTargetStyles{L"StackPanel#SettingsCommandSearchBoxBackground", {
        L"CornerRadius=$InRadius",
        L"MinHeight=32"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Grid.Column=0",
        L"Visibility=Hidden"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem", {
        L"MinHeight=48",
        L"MinWidth=65",
        L"ToolTipService.Placement=5",
        L"MaxWidth=65"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid@DisplayModeStates > Grid#PaneRoot", {
        L"MaxWidth@OpenInlineLeft=65",
        L"Grid.ColumnSpan@OpenInlineLeft=1",
        L"Grid.ColumnSpan=>Span"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#00000000\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>",
        L"CornerRadius={{Span > 1 ? 0 : $OutRadius}},0,0,0",
        L"Margin={{Span > 1 ? 0 : 65}},48,0,0",
        L"BorderBrush:=$BgBorder",
        L"BorderThickness={{Span > 1 ? 0 : 1}},1,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid@DisplayModeStates > Grid#ContentRoot", {
        L"Grid.Column@OpenInlineLeft=0",
        L"Grid.ColumnSpan@OpenInlineLeft=3"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > Grid#ShadowCaster", {
        L"Grid.ColumnSpan=1",
        L"MaxWidth=65"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Padding=3,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"FontFamily=Segoe Fluent Icons",
        L"Foreground@Normal:=<SolidColorBrush Color=\"{ThemeResource TextFillColorSecondary}\" />",
        L"Foreground@PointerOver:=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" />",
        L"Foreground@Pressed:=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" />",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource Accent}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource Accent}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource Accent}\" />",
        L"FontSize=20",
        L"Margin=15,0,-2,0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[Content=Home] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE80F",
        L"Content@PointerOver:=\uE80F",
        L"Content@Pressed:=\uE80F",
        L"Content@Selected:=\uEA8A",
        L"Content@PointerOverSelected:=\uEA8A",
        L"Content@PressedSelected:=\uEA8A",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[Content=System] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE75B",
        L"Content@PointerOver:=\uE75B",
        L"Content@Pressed:=\uE75B",
        L"Content@Selected:=\uE75B",
        L"Content@PointerOverSelected:=\uE75B",
        L"Content@PressedSelected:=\uE75B",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[Content=Bluetooth & devices] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE702",
        L"Content@PointerOver:=\uE702",
        L"Content@Pressed:=\uE702",
        L"Content@Selected:=\uE702",
        L"Content@PointerOverSelected:=\uE702",
        L"Content@PressedSelected:=\uE702",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[3] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uF0C5",
        L"Content@PointerOver:=\uF0C5",
        L"Content@Pressed:=\uF0C5",
        L"Content@Selected:=\uF0C5",
        L"Content@PointerOverSelected:=\uF0C5",
        L"Content@PressedSelected:=\uF0C5",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[4] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uEA18",
        L"Content@PointerOver:=\uEA18",
        L"Content@Pressed:=\uEA18",
        L"Content@Selected:=\uE83D",
        L"Content@PointerOverSelected:=\uE83D",
        L"Content@PressedSelected:=\uE83D",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[5] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE776",
        L"Content@PointerOver:=\uE776",
        L"Content@Pressed:=\uE776",
        L"Content@Selected:=\uE776",
        L"Content@PointerOverSelected:=\uE776",
        L"Content@PressedSelected:=\uE776",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[6] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE7FC",
        L"Content@PointerOver:=\uE7FC",
        L"Content@Pressed:=\uE7FC",
        L"Content@Selected:=\uE7FC",
        L"Content@PointerOverSelected:=\uE7FC",
        L"Content@PressedSelected:=\uE7FC",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[7] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE775",
        L"Content@PointerOver:=\uE775",
        L"Content@Pressed:=\uE775",
        L"Content@Selected:=\uE775",
        L"Content@PointerOverSelected:=\uE775",
        L"Content@PressedSelected:=\uE775",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[8] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE77B",
        L"Content@PointerOver:=\uE77B",
        L"Content@Pressed:=\uE77B",
        L"Content@Selected:=\uEA8C",
        L"Content@PointerOverSelected:=\uEA8C",
        L"Content@PressedSelected:=\uEA8C",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[9] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE74C",
        L"Content@PointerOver:=\uE74C",
        L"Content@Pressed:=\uE74C",
        L"Content@Selected:=\uE74C",
        L"Content@PointerOverSelected:=\uE74C",
        L"Content@PressedSelected:=\uE74C",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[10] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE771",
        L"Content@PointerOver:=\uE771",
        L"Content@Pressed:=\uE771",
        L"Content@Selected:=\uE771",
        L"Content@PointerOverSelected:=\uE771",
        L"Content@PressedSelected:=\uE771",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[11] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE701",
        L"Content@PointerOver:=\uE701",
        L"Content@Pressed:=\uE701",
        L"Content@Selected:=\uE701",
        L"Content@PointerOverSelected:=\uE701",
        L"Content@PressedSelected:=\uE701",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[12] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE702",
        L"Content@PointerOver:=\uE702",
        L"Content@Pressed:=\uE702",
        L"Content@Selected:=\uE702",
        L"Content@PointerOverSelected:=\uE702",
        L"Content@PressedSelected:=\uE702",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[1]", {
        L"Content=>t1",
        L"ToolTipService.ToolTip={{t1}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[2]", {
        L"Content=>t2",
        L"ToolTipService.ToolTip={{t2}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[3]", {
        L"Content=>t3",
        L"ToolTipService.ToolTip={{t3}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[4]", {
        L"Content=>t4",
        L"ToolTipService.ToolTip={{t4}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[5]", {
        L"Content=>t5",
        L"ToolTipService.ToolTip={{t5}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[6]", {
        L"Content=>t6",
        L"ToolTipService.ToolTip={{t6}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[7]", {
        L"Content=>t7",
        L"ToolTipService.ToolTip={{t7}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[8]", {
        L"Content=>t8",
        L"ToolTipService.ToolTip={{t8}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[9]", {
        L"Content=>t9",
        L"ToolTipService.ToolTip={{t9}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[10]", {
        L"Content=>t10",
        L"ToolTipService.ToolTip={{t10}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[11]", {
        L"Content=>t11",
        L"ToolTipService.ToolTip={{t11}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[12]", {
        L"Content=>t12",
        L"ToolTipService.ToolTip={{t12}}"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid[2]", {
        L"Visibility=1",
        L"Grid.Column=0"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid[2] > TextBlock#UserName", {
        L"Text=>UserName"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton", {
        L"ToolTipService.ToolTip={{UserName}}",
        L"ToolTipService.Placement=10",
        L"Visibility=1"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid#UserImageGrid > Image", {
        L"Width=30",
        L"Height=30"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel", {
        L"MaxHeight=48",
        L"MaxWidth=65",
        L"MinHeight=48",
        L"MinWidth=65",
        L"Visibility=1"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton", {
        L"MinHeight=48",
        L"MaxHeight=48",
        L"Margin=3,3,3,-3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ProgressBarIndicator", {
        L"RadiusX=3",
        L"RadiusY=3",
        L"Height=6",
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource Accent}\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#DeterminateRoot", {
        L"CornerRadius=3",
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ProgressBar", {
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#DeterminateRoot > Windows.UI.Xaml.Shapes.Rectangle#ProgressBarIndicator", {
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#DeterminateRoot", {
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar", {
        L"Height=16"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid@DisplayModeStates > Grid#PaneRoot", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Frame#PermanentNavRootFrame", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Grid#ContentRoot", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#ContentRoot", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Grid#RootGrid", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Grid#AppTitleBar", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Border#AppTitleBarBackground", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Grid#TitleBar", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Border#TitleBarBackground", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox", {
        L"CornerRadius=$InRadius",
        L"MinHeight=32",
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"#761E1E1E\" FallbackColor=\"#00000000\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" Opacity=\"0.4\"/>",
        L"BorderBrush:=<SolidColorBrush Color=\"#22FFFFFF\"/>",
        L"BorderThickness=1"}},
}, {
    L"OutRadius=8",
    L"InRadius=12",
}, {
    L"Overlay@Light=#55FFFFFF",
    L"Overlay@Dark=#09FFFFFF",
    L"Border@Light=#0F000000",
    L"Border@Dark=#19000000",
    L"Accent@Dark={ThemeResource SystemAccentColorLight2}",
    L"Accent@Light={ThemeResource SystemAccentColorDark1}",
    L"WindowCaptionBackground@Dark=#00000000",
    L"WindowCaptionBackground@Light=#00000000",
    L"WindowCaptionBackgroundDisabled@Dark=#00000000",
    L"WindowCaptionBackgroundDisabled@Light=#00000000",
    L"SolidBackgroundFillColorBase@Dark=#00000000",
    L"SolidBackgroundFillColorBase@Light=#00000000",
    L"SolidBackgroundFillColorSecondary@Dark=#00000000",
    L"SolidBackgroundFillColorSecondary@Light=#00000000",
    L"LayerFillColorDefault@Dark=#00000000",
    L"LayerFillColorDefault@Light=#00000000",
    L"ApplicationPageBackgroundThemeBrush@Dark=#00000000",
    L"ApplicationPageBackgroundThemeBrush@Light=#00000000",
}};

const Theme g_themeWindowGlass = {{
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ContentRoot > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid#ContentGrid", {
        L"Background:=$ElementBG",
        L"BorderBrush:=$ElementBorderBrush",
        L"CornerRadius=13",
        L"BorderThickness=$ElementBorderThickness",
        L"Margin=8,50,8,8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.RelativePanel#PaneContentGrid > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Background:=$ElementBG"}},
    ThemeTargetStyles{L"WinStore.UX.Controls.SearchAutoSuggestBox#SearchBox > Windows.UI.Xaml.Controls.AutoSuggestBox#SearchTextBox > Windows.UI.Xaml.Controls.Grid#LayoutRoot > Windows.UI.Xaml.Controls.TextBox#TextBox > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BorderElement", {
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ControlPanelGrid", {
        L"CornerRadius=$CornerRadius",
        L"BorderBrush:=$BorderBrush",
        L"Background:=$Glass",
        L"BorderThickness=$BorderThickness",
        L"Width=Auto",
        L"Margin=100,0,100,-150",
        L"Height=Auto",
        L"MaxWidth:=700",
        L"MinWidth:=15",
        L"MinHeight:=15",
        L"MaxHeight:=300",
        L"HorizontalAlignment=1",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"-170\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#mtcMediaInformationButton", {
        L"CornerRadius=$CornerRadius",
        L"Padding=10",
        L"Margin=20,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#MediaTransportControls_Timeline_Border", {
        L"RenderTransform:=<TranslateTransform Y=\"25\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ToggleButton#mtcMediaInformationButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"RenderTransform:=<TranslateTransform Y=\"25\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#MediaControlsCommandBar_Center_Container", {
        L"RenderTransform:=<TranslateTransform Y=\"25\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#MediaControlsCommandBar_Right", {
        L"RenderTransform:=<TranslateTransform Y=\"25\"/>",
        L"Margin=0,0,20,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ContentRoot > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid#ContentGrid", {
        L"Background:=$MainContentBG",
        L"CornerRadius=0",
        L"Margin=0",
        L"BorderBrush:=$ElementBorderBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Windows.UI.Xaml.Controls.Border", {
        L"Background:=$$Frosted",
        L"BorderBrush=$BorderBrush",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Shapes.Ellipse#SliderInnerThumb", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SliderContainer > Windows.UI.Xaml.Controls.Grid#HorizontalTemplate > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb", {
        L"Height=20",
        L"Width=30"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#VerticalThumb > Windows.UI.Xaml.Controls.Border", {
        L"Background:=$Frosted",
        L"BorderBrush=$BorderBrush",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.Thumb#VerticalThumb > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Shapes.Ellipse#SliderInnerThumb", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SliderContainer > Windows.UI.Xaml.Controls.Grid#VerticalTemplate > Windows.UI.Xaml.Controls.Primitives.Thumb#VerticalThumb", {
        L"Height=30",
        L"Width=20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Background:=Transparent",
        L"BorderBrush:=Transparent",
        L"BorderThickness:=0",
        L"CornerRadius:=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundElement > Windows.UI.Xaml.Controls.Grid#DialogSpace", {
        L"Background:=$Frosted",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness:=$BorderThickness",
        L"CornerRadius:=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundElement > Windows.UI.Xaml.Controls.Grid#DialogSpace > Windows.UI.Xaml.Controls.Grid#CommandSpace", {
        L"Background:=Transparent",
        L"BorderBrush:=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToggleSwitch > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Shapes.Rectangle#OuterBorder", {
        L"Height=22",
        L"Width=50",
        L"RadiusX=10",
        L"RadiusY=10",
        L"Stroke:=$ElementSysColor2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToggleSwitch > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobBounds", {
        L"Height=22",
        L"Width=50",
        L"RadiusX=10",
        L"RadiusY=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SwitchKnob", {
        L"Height=20",
        L"Width=32"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SwitchKnob > Windows.UI.Xaml.Controls.Border#SwitchKnobOn", {
        L"Height=19",
        L"Width=26",
        L"CornerRadius=8",
        L"Background:=$ElementSysColor"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#SwitchKnob > Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobOff", {
        L"Height=17",
        L"Width=25",
        L"RadiusX=8",
        L"RadiusY=8",
        L"Fill:=$ElementSysColor"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter", {
        L"Background:=$Frosted",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter > Windows.UI.Xaml.Controls.Border", {
        L"Background:=$Frosted",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter#LayoutRoot", {
        L"Background:=$Frosted",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Canvas > Windows.UI.Xaml.Controls.Border#PopupBorder", {
        L"Background:=$Frosted",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
}, {
    L"Glass=<WindhawkBlur BlurAmount=\"3\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Frosted=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Acrylic=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.3\" FallbackColor=\"{ThemeResource SystemChromeAltHighColor}\" />",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"BorderBrush2=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"{ThemeResource SystemChromeHighColor}\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SystemChromeLowColor}\" Offset=\"0.15\" /><GradientStop Color=\"{ThemeResource SystemChromeHighColor}\" Offset=\"0.95\" /></LinearGradientBrush>",
    L"overlay=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.1\" />",
    L"overlay2=<WindhawkBlur BlurAmount=\"20\" TintColor=\"#60353535\"/>",
    L"CornerRadius=30",
    L"CR2=14",
    L"CR3=12",
    L"BorderThickness=0.3,1,0.3,0.3",
    L"ElementBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"1\" />",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"ElementCornerRadius=30",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"ElementSysColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"1\" />",
    L"ElementSysColor2=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1\" />",
    L"ElementSysColor3=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\" Opacity=\"1\" />",
    L"ElementSysColor4=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark1}\" Opacity=\"1\" />",
    L"Backdrop=<AcrylicBrush BackgroundSource=\"HostBackdrop\" TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.3\" FallbackColor=\"{ThemeResource SystemChromeAltHighColor}\" />",
}};

const Theme g_themeOLED_variant_ModrinthGreen = {{
    ThemeTargetStyles{L"ContentControl#GridViewItemContentControl > ContentPresenter > Grid", {
        L"Background=#101013"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > Border", {
        L"CornerRadius=15",
        L"Background=transparent"}},
    ThemeTargetStyles{L"Microsoft.ReactNative.ReactRootView > Microsoft.ReactNative.ViewPanel > Microsoft.ReactNative.ViewPanel > Microsoft.ReactNative.ViewPanel > Microsoft.ReactNative.ViewPanel > Border", {
        L"Background=#101013"}},
    ThemeTargetStyles{L"SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter", {
        L"Background=#101013"}},
    ThemeTargetStyles{L"ContentPresenter#ContentPresenter > Grid > Grid > TextBlock", {
        L"Foreground=white"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent > TextBlock", {
        L"Foreground=#ADADAE"}},
    ThemeTargetStyles{L"Microsoft.ReactNative.ViewPanel > TextBlock", {
        L"Foreground=#ADADAE"}},
    ThemeTargetStyles{L"ContentPresenter#SubtitleContent > TextBlock", {
        L"Foreground=#ADADAE"}},
    ThemeTargetStyles{L"ContentPresenter#SubtitleContent > StackPanel > StackPanel > TextBlock", {
        L"Foreground=#ADADAE"}},
    ThemeTargetStyles{L"ContentPresenter#SubtitleContent > Grid > TextBlock", {
        L"Foreground=#ADADAE"}},
    ThemeTargetStyles{L"ContentPresenter#InlineContentPresenter > StackPanel > ContentControl > ContentPresenter > StackPanel > TextBlock", {
        L"Foreground=white"}},
    ThemeTargetStyles{L"Button#DeviceOptionsButton > Grid > Grid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=white"}},
    ThemeTargetStyles{L"ContentPresenter#TitleContent > TextBlock", {
        L"Foreground=white"}},
    ThemeTargetStyles{L"SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter", {
        L"Margin=2"}},
    ThemeTargetStyles{L"Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent", {
        L"Margin=10,-38,0,5"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter", {
        L"Margin=-12,8,0,0"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox > Grid > Button#DeleteButton > Grid#ButtonLayoutGrid", {
        L"CornerRadius=$InRadius",
        L"MinHeight=32"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Grid.Column=0",
        L"Visibility=Hidden"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem", {
        L"MinHeight=48",
        L"MinWidth=65",
        L"ToolTipService.Placement=5",
        L"MaxWidth=65"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid@DisplayModeStates > Grid#PaneRoot", {
        L"MaxWidth@OpenInlineLeft=65",
        L"Grid.ColumnSpan@OpenInlineLeft=1",
        L"Grid.ColumnSpan=>Span"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid", {
        L"CornerRadius={{Span > 1 ? 0 : $OutRadius}},0,0,0",
        L"Margin={{Span > 1 ? 0 : 65}},48,0,0",
        L"BorderThickness={{Span > 1 ? 0 : 1}},1,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid@DisplayModeStates > Grid#ContentRoot", {
        L"Grid.Column@OpenInlineLeft=0",
        L"Grid.ColumnSpan@OpenInlineLeft=3"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > Grid#ShadowCaster", {
        L"Grid.ColumnSpan=1",
        L"MaxWidth=65"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[1]", {
        L"Content=>t1",
        L"ToolTipService.ToolTip={{t1}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[2]", {
        L"Content=>t2",
        L"ToolTipService.ToolTip={{t2}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[3]", {
        L"Content=>t3",
        L"ToolTipService.ToolTip={{t3}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[4]", {
        L"Content=>t4",
        L"ToolTipService.ToolTip={{t4}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[5]", {
        L"Content=>t5",
        L"ToolTipService.ToolTip={{t5}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[6]", {
        L"Content=>t6",
        L"ToolTipService.ToolTip={{t6}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[7]", {
        L"Content=>t7",
        L"ToolTipService.ToolTip={{t7}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[8]", {
        L"Content=>t8",
        L"ToolTipService.ToolTip={{t8}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[9]", {
        L"Content=>t9",
        L"ToolTipService.ToolTip={{t9}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[10]", {
        L"Content=>t10",
        L"ToolTipService.ToolTip={{t10}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[11]", {
        L"Content=>t11",
        L"ToolTipService.ToolTip={{t11}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[12]", {
        L"Content=>t12",
        L"ToolTipService.ToolTip={{t12}}"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid[2]", {
        L"Visibility=1",
        L"Grid.Column=0"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid[2] > TextBlock#UserName", {
        L"Text=>UserName"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton", {
        L"ToolTipService.ToolTip={{UserName}}",
        L"ToolTipService.Placement=10",
        L"Visibility=1"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid#UserImageGrid > Image", {
        L"Width=30",
        L"Height=30"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel", {
        L"MaxHeight=48",
        L"MaxWidth=65",
        L"MinHeight=48",
        L"MinWidth=65",
        L"Visibility=1"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton", {
        L"MinHeight=48",
        L"MaxHeight=48",
        L"Margin=3,3,3,-3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ProgressBarIndicator", {
        L"RadiusX=3",
        L"RadiusY=3",
        L"Height=6",
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource Accent}\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#DeterminateRoot", {
        L"CornerRadius=3",
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ProgressBar", {
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#DeterminateRoot > Windows.UI.Xaml.Shapes.Rectangle#ProgressBarIndicator", {
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#DeterminateRoot", {
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar", {
        L"Height=16"}},
    ThemeTargetStyles{L"CheckBox > Grid#RootGrid@CombinedStates > Grid > Rectangle#NormalRectangle", {
        L"StrokeThickness=1",
        L"Stroke=#27292E",
        L"Fill@UncheckedNormal=black",
        L"Fill@UncheckedPointerOver=black",
        L"Fill@UncheckedPointerOverSelected=black",
        L"Fill@CheckedNormal=#1BD96A",
        L"Fill@CheckedPointerOver=#1BD96A",
        L"Fill@CheckedPointerOverSelected=#1BD96A",
        L"RadiusX=6",
        L"RadiusY=6"}},
    ThemeTargetStyles{L"SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.WatermarkTextBox > Grid > Border#BorderElement", {
        L"Background=#101013",
        L"CornerRadius=12",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.HighContrastThemesCombobox", {
        L"BorderBrush=#1BD96A",
        L"BorderThickness=3",
        L"Background=Black",
        L"Foreground=#1BD96A",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.HighContrastThemesCombobox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"HyperlinkButton > ContentPresenter#ContentPresenter", {
        L"Foreground=#1BD96A"}},
    ThemeTargetStyles{L"ProgressBar > Grid > Border#DeterminateRoot > Rectangle#ProgressBarIndicator", {
        L"Fill=#1BD96A"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"BorderBrush=#1BD96A",
        L"BorderThickness=3",
        L"Background=Black",
        L"Foreground=#1BD96A",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"BorderBrush=#1BD96A",
        L"BorderThickness=3",
        L"Background=Black",
        L"Foreground=#1BD96A",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > StackPanel > ContentPresenter#TitleContent > StackPanel > RadioButton > Grid#RootGrid > Grid > Windows.UI.Xaml.Shapes.Ellipse#CheckOuterEllipse", {
        L"Fill=#1BD96A",
        L"StrokeThickness=0"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Grid > Slider > Grid > Grid#SliderContainer > Grid#HorizontalTemplate > Rectangle#HorizontalTrackRect", {
        L"Fill=black",
        L"Margin=0,0,5,0",
        L"StrokeThickness=0"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Grid > Slider > Grid > Grid#SliderContainer > Grid#HorizontalTemplate > Rectangle#HorizontalDecreaseRect", {
        L"Fill=#1BD96A",
        L"StrokeThickness=0"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Grid > Slider > Grid > Grid#SliderContainer > Grid#HorizontalTemplate > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Border > Windows.UI.Xaml.Shapes.Ellipse#SliderInnerThumb", {
        L"Fill=#1BD96A",
        L"Height=0",
        L"Width=0"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Grid > Slider > Grid > Grid#SliderContainer > Grid#HorizontalTemplate > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Border", {
        L"Width=17",
        L"Height=17",
        L"Margin=-5,0,0,0",
        L"Background=#1BD96A"}},
    ThemeTargetStyles{L"CheckBox > Grid#RootGrid@CombinedStates > Grid > Microsoft.UI.Xaml.Controls.AnimatedIcon#CheckGlyph", {
        L"Width=25",
        L"Height=25",
        L"Margin=-2,-1,0,0"}},
    ThemeTargetStyles{L"Button#focusStartButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background=#1BD96A",
        L"Foreground=Black",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusEnableControl#FocusEnableControl > StackPanel > Button#focusStartButton > ContentPresenter#ContentPresenter > Grid > TextBlock#StartButtonText", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusEnableControl#FocusEnableControl > StackPanel > Button#focusStopButton > ContentPresenter#ContentPresenter", {
        L"BorderBrush=#1BD96A",
        L"BorderThickness=3",
        L"Background=Black",
        L"Foreground=#1BD96A",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusSessionControl > StackPanel > Grid > Button#focusSessionDecreaseButton > Grid > ContentPresenter#ContentPresenter", {
        L"BorderBrush=#1BD96A",
        L"BorderThickness=4",
        L"Background=Black",
        L"Foreground=#1BD96A",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusSessionControl > StackPanel > Grid > Button#focusSessionIncreaseButton > Grid > ContentPresenter#ContentPresenter > FontIcon > Grid > TextBlock", {
        L"Padding=2,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusSessionControl > StackPanel > Grid > Button#focusSessionDecreaseButton > Grid > ContentPresenter#ContentPresenter > FontIcon > Grid > TextBlock", {
        L"Padding=2,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusSessionControl > StackPanel > Grid > Grid > TextBlock#focusSessionDurationTextBlock", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI",
        L"Background=#1BD96A"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusSessionControl > StackPanel > Grid > Button#focusSessionIncreaseButton > Grid > ContentPresenter#ContentPresenter", {
        L"BorderBrush=#1BD96A",
        L"BorderThickness=4",
        L"Background=Black",
        L"Foreground=#1BD96A",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusEnableControl#FocusEnableControl > StackPanel > Button#focusStopButton > ContentPresenter#ContentPresenter > Grid > TextBlock#StopButtonText", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl#heroContent > ContentPresenter > SystemSettings.View.AlignableContentControl > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#DefaultOneSegmentHeroUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=#1BD96A",
        L"FontWeight=Bold",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > UserControl > StackPanel > Button#WindowsProtectedPrintButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=#1BD96A",
        L"FontSize=14",
        L"FontWeight=Bold",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > UserControl > StackPanel > Button#WindowsProtectedPrintButton > ContentPresenter#ContentPresenter", {
        L"Background=Black",
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"Foreground=black",
        L"Background=#1BD96A",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=black",
        L"FontSize=14",
        L"FontWeight=Bold",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"SystemSettings.View.StableComboBox > Grid#LayoutRoot > Border#Background", {
        L"Background=Black",
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=#1BD96A",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootPageGrid", {
        L"Background=#101013"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"Background=Black",
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=#1BD96A",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Microsoft.UI.Xaml.Controls.DropDownButton > Grid#RootGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=#1BD96A",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"Background=Black",
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > Border#Background", {
        L"Background=Black",
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=#1BD96A",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > Border#Background", {
        L"Background=Black",
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=#1BD96A",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=#1BD96A",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=#1BD96A",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button#ClassicAppButton > ContentPresenter#ContentPresenter", {
        L"Foreground=#1BD96A",
        L"Background=Black",
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button#ClassicAppButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontWeight=bold"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsListView#DevicesHeroControlList > Border > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsPresenter > StackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > UserControl > Button#DevicesHeroControlButton > ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal:=#040b07",
        L"Background@PointerOver:=#06150d",
        L"Background@Pressed:=#04150c",
        L"Background@Disabled:=#010b04",
        L"BorderThickness=2",
        L"BorderBrush@Normal:=#0c190f",
        L"BorderBrush@PointerOver:=#0c190f",
        L"BorderBrush@Pressed:=#0c190f",
        L"BorderBrush@Disabled:=#000000",
        L"CornerRadius=14",
        L"Margin=3"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl#heroContent > ContentPresenter > SystemSettings.View.AlignableContentControl > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#DefaultOneSegmentHeroUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > SystemSettings.View.DevicesHeroControl > Grid > Button > ContentPresenter#ContentPresenter", {
        L"Background:=#040b07",
        L"Margin=3",
        L"CornerRadius=14",
        L"BorderBrush:=#0c190f",
        L"BorderThickness=2"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button#ModernAppButton > ContentPresenter#ContentPresenter", {
        L"Foreground=#1BD96A",
        L"Background=Black",
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button#ModernAppButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontWeight=bold"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsListItemsRepeater > ScrollViewer#SettingsListScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#ItemsRepeater > SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent > ContentControl > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.EntityItem#DeviceRemoveButtonContent > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"Foreground=#1BD96A",
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot", {
        L"BorderBrush=#1BD96A",
        L"BorderThickness=1",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > TextBlock", {
        L"FontWeight=bold"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot", {
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > TextBlock", {
        L"FontWeight=bold"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot", {
        L"BorderBrush=#1BD96A",
        L"BorderThickness=1",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#OuterBorder", {
        L"Fill=#1B1B20",
        L"Height=33",
        L"Width=55",
        L"RadiusX=15",
        L"RadiusY=20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobOff", {
        L"Height=18",
        L"Width=18",
        L"RadiusX=25",
        L"RadiusY=25",
        L"Margin=5,0,-5,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SwitchKnobOn", {
        L"Height=20",
        L"Width=20",
        L"CornerRadius=25",
        L"Margin=10,0,-10,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobBounds", {
        L"Fill=#1BD96A",
        L"Height=35",
        L"Width=55",
        L"RadiusX=15",
        L"RadiusY=20"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid", {
        L"Background=#000000",
        L"CornerRadius=14,0,0,0",
        L"BorderBrush=#25262B",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView", {
        L"Background=#000000",
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemSettings.View.EntityItem", {
        L"Background=#101013",
        L"Foreground=White",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsListItemsRepeater > ScrollViewer#SettingsListScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#ItemsRepeater > SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter > Grid > ContentPresenter > SystemSettings.View.EntityItem#EntityExpandableListItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button > ContentPresenter#ContentPresenter", {
        L"BorderBrush=#1BD96A",
        L"Width=150",
        L"Foreground=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl#heroContent > ContentPresenter > SystemSettings.View.AlignableContentControl > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#DefaultOneSegmentHeroUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > Button > ContentPresenter#ContentPresenter", {
        L"Background=#141417",
        L"Width=150",
        L"BorderBrush=#1BD96A",
        L"BorderThickness=2",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter", {
        L"Background=#101013",
        L"Background@PointerOver=#101013",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter", {
        L"Background=#101013",
        L"Background@PointerOver=#101013",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter", {
        L"Background=#101013",
        L"Background@PointerOver=#101013",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"StackPanel#BackgroundStackPanel", {
        L"Background=#101013",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Rectangle#SelectionIndicator", {
        L"Height=0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[Content=Home] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE80F",
        L"Content@PointerOver:=\uE80F",
        L"Content@Pressed:=\uE80F",
        L"Content@Selected:=\uEA8A",
        L"Content@PointerOverSelected:=\uEA8A",
        L"Content@PressedSelected:=\uEA8A",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[Content=System] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE75B",
        L"Content@PointerOver:=\uE75B",
        L"Content@Pressed:=\uE75B",
        L"Content@Selected:=\uE75B",
        L"Content@PointerOverSelected:=\uE75B",
        L"Content@PressedSelected:=\uE75B",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[Content=Bluetooth & devices] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE702",
        L"Content@PointerOver:=\uE702",
        L"Content@Pressed:=\uE702",
        L"Content@Selected:=\uE702",
        L"Content@PointerOverSelected:=\uE702",
        L"Content@PressedSelected:=\uE702",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[3] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uF0C5",
        L"Content@PointerOver:=\uF0C5",
        L"Content@Pressed:=\uF0C5",
        L"Content@Selected:=\uF0C5",
        L"Content@PointerOverSelected:=\uF0C5",
        L"Content@PressedSelected:=\uF0C5",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[4] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uEA18",
        L"Content@PointerOver:=\uEA18",
        L"Content@Pressed:=\uEA18",
        L"Content@Selected:=\uE83D",
        L"Content@PointerOverSelected:=\uE83D",
        L"Content@PressedSelected:=\uE83D",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[5] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE776",
        L"Content@PointerOver:=\uE776",
        L"Content@Pressed:=\uE776",
        L"Content@Selected:=\uE776",
        L"Content@PointerOverSelected:=\uE776",
        L"Content@PressedSelected:=\uE776",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[6] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE7FC",
        L"Content@PointerOver:=\uE7FC",
        L"Content@Pressed:=\uE7FC",
        L"Content@Selected:=\uE7FC",
        L"Content@PointerOverSelected:=\uE7FC",
        L"Content@PressedSelected:=\uE7FC",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[7] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE775",
        L"Content@PointerOver:=\uE775",
        L"Content@Pressed:=\uE775",
        L"Content@Selected:=\uE775",
        L"Content@PointerOverSelected:=\uE775",
        L"Content@PressedSelected:=\uE775",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[8] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE77B",
        L"Content@PointerOver:=\uE77B",
        L"Content@Pressed:=\uE77B",
        L"Content@Selected:=\uEA8C",
        L"Content@PointerOverSelected:=\uEA8C",
        L"Content@PressedSelected:=\uEA8C",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[9] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE74C",
        L"Content@PointerOver:=\uE74C",
        L"Content@Pressed:=\uE74C",
        L"Content@Selected:=\uE74C",
        L"Content@PointerOverSelected:=\uE74C",
        L"Content@PressedSelected:=\uE74C",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[10] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE771",
        L"Content@PointerOver:=\uE771",
        L"Content@Pressed:=\uE771",
        L"Content@Selected:=\uE771",
        L"Content@PointerOverSelected:=\uE771",
        L"Content@PressedSelected:=\uE771",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[11] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE701",
        L"Content@PointerOver:=\uE701",
        L"Content@Pressed:=\uE701",
        L"Content@Selected:=\uE701",
        L"Content@PointerOverSelected:=\uE701",
        L"Content@PressedSelected:=\uE701",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[12] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE702",
        L"Content@PointerOver:=\uE702",
        L"Content@Pressed:=\uE702",
        L"Content@Selected:=\uE702",
        L"Content@PointerOverSelected:=\uE702",
        L"Content@PressedSelected:=\uE702",
        L"Foreground@Selected=#1BD96A",
        L"Foreground@PointerOverSelected=#1BD96A",
        L"Foreground@PressedSelected=#1BD96A",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Padding=10,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"FontFamily=Segoe Fluent Icons",
        L"FontSize=20",
        L"Margin=15,0,-2,0"}},
    ThemeTargetStyles{L"ContentPresenter#IconContentPresenter", {
        L"Foreground:=#1BD96A"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates", {
        L"Background@Normal=Transparent",
        L"Height=48",
        L"Margin=8,0,8,0",
        L"Padding= -4",
        L"Background@PointerOver=#101013",
        L"Background@Pressed:=#134229",
        L"Background@Selected:=#134229",
        L"Background@PointerOverSelected:=#134229",
        L"Background@PressedSelected:=#134229",
        L"CornerRadius@Normal=10",
        L"CornerRadius=30"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground@Normal=White",
        L"Foreground@PointerOver=White",
        L"Foreground@Pressed=White",
        L"Foreground@Selected:=#1BD96A",
        L"Foreground@PointerOverSelected:=#1BD96A",
        L"Foreground@PressedSelected:=#1BD96A",
        L"FontWeight=Bold",
        L"FontSize@Selected=16",
        L"FontSize@PointerOverSelected=16"}},
    ThemeTargetStyles{L"SystemSettings.View.UserProfileControl#UserProfileControl", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#UserProfileButton", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ThumbVisual", {
        L"Fill=#1E1E1E"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#NavigationViewBackButton > Windows.UI.Xaml.Controls.Grid#RootGrid", {
        L"Background=#1A1A1F",
        L"CornerRadius=25",
        L"Height=30",
        L"Width=30"}},
    ThemeTargetStyles{L"SystemSettings.View.AlignableContentControl#PermanentNavViewHeaderAlignControl > ContentPresenter", {
        L"Background=#101013",
        L"CornerRadius=25",
        L"Padding=25,5"}},
    ThemeTargetStyles{L"SystemSettings.View.AlignableContentControl#PermanentNavViewHeaderAlignControl > ContentPresenter > Microsoft.UI.Xaml.Controls.BreadcrumbBar#PermanentNavigationViewBreadcrumbBar > Microsoft.UI.Xaml.Controls.ItemsRepeater#PART_ItemsRepeater > Microsoft.UI.Xaml.Controls.BreadcrumbBarItem > Grid#PART_LayoutRoot > ContentPresenter#PART_LastItemContentPresenter", {
        L"Padding=20,0,20,0",
        L"Height=45",
        L"Background:=#134229",
        L"Foreground:=#1BD96A",
        L"CornerRadius=25"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter", {
        L"Background=#101013",
        L"CornerRadius=12,12,12,12",
        L"Margin=0,2,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#SettingsCommandSearchBoxBackground", {
        L"Height=30",
        L"Background=#18181B",
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView", {
        L"Background=#101013"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"Foreground=Black",
        L"Background=#1BD96A",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"Foreground=Black",
        L"Background=#1BD96A",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#OneSegmentHeroEntityItemUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > StackPanel > Button > ContentPresenter#ContentPresenter", {
        L"Foreground=Black",
        L"Background=#1BD96A",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#OneSegmentHeroEntityItemUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > StackPanel > Button", {
        L"Foreground=black",
        L"Background=#1BD96A",
        L"CornerRadius=12",
        L"FontSize=14",
        L"FontWeight=Bold"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Foreground=Black",
        L"Background=#1BD96A",
        L"CornerRadius=12",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBox#CommandSearchTextBox", {
        L"Background=#1A1A1F",
        L"CornerRadius=12",
        L"Foreground=White",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot", {
        L"Background=Transparent"}},
}};

const Theme g_themeOLED_variant_SystemAscent = {{
    ThemeTargetStyles{L"ContentControl#GridViewItemContentControl > ContentPresenter > Grid", {
        L"Background=#101013"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > Border", {
        L"CornerRadius=15",
        L"Background=transparent"}},
    ThemeTargetStyles{L"Microsoft.ReactNative.ReactRootView > Microsoft.ReactNative.ViewPanel > Microsoft.ReactNative.ViewPanel > Microsoft.ReactNative.ViewPanel > Microsoft.ReactNative.ViewPanel > Border", {
        L"Background=#101013"}},
    ThemeTargetStyles{L"SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter", {
        L"Background=#101013"}},
    ThemeTargetStyles{L"ContentPresenter#ContentPresenter > Grid > Grid > TextBlock", {
        L"Foreground=white"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > StackPanel > ContentPresenter#SubtitleContent > TextBlock", {
        L"Foreground=#ADADAE"}},
    ThemeTargetStyles{L"Microsoft.ReactNative.ViewPanel > TextBlock", {
        L"Foreground=#ADADAE"}},
    ThemeTargetStyles{L"ContentPresenter#SubtitleContent > TextBlock", {
        L"Foreground=#ADADAE"}},
    ThemeTargetStyles{L"ContentPresenter#SubtitleContent > StackPanel > StackPanel > TextBlock", {
        L"Foreground=#ADADAE"}},
    ThemeTargetStyles{L"ContentPresenter#SubtitleContent > Grid > TextBlock", {
        L"Foreground=#ADADAE"}},
    ThemeTargetStyles{L"ContentPresenter#InlineContentPresenter > StackPanel > ContentControl > ContentPresenter > StackPanel > TextBlock", {
        L"Foreground=white"}},
    ThemeTargetStyles{L"Button#DeviceOptionsButton > Grid > Grid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=white"}},
    ThemeTargetStyles{L"ContentPresenter#TitleContent > TextBlock", {
        L"Foreground=white"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter", {
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter", {
        L"Margin=2"}},
    ThemeTargetStyles{L"Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent", {
        L"Margin=10,-38,0,5"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter", {
        L"Margin=-12,8,0,0"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox > Grid > Button#DeleteButton > Grid#ButtonLayoutGrid", {
        L"CornerRadius=$InRadius",
        L"MinHeight=32"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Grid.Column=0",
        L"Visibility=Hidden"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > Grid#ItemsContainerGrid > Microsoft.UI.Xaml.Controls.ItemsRepeaterScrollHost > ScrollViewer#MenuItemsScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#MenuItemsHost > SystemSettings.View.SettingsNavigationViewItem", {
        L"MinHeight=48",
        L"MinWidth=65",
        L"ToolTipService.Placement=5",
        L"MaxWidth=65"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid@DisplayModeStates > Grid#PaneRoot", {
        L"MaxWidth@OpenInlineLeft=65",
        L"Grid.ColumnSpan@OpenInlineLeft=1",
        L"Grid.ColumnSpan=>Span"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid", {
        L"CornerRadius={{Span > 1 ? 0 : $OutRadius}},0,0,0",
        L"Margin={{Span > 1 ? 0 : 65}},48,0,0",
        L"BorderThickness={{Span > 1 ? 0 : 1}},1,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid@DisplayModeStates > Grid#ContentRoot", {
        L"Grid.Column@OpenInlineLeft=0",
        L"Grid.ColumnSpan@OpenInlineLeft=3"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > Grid#ShadowCaster", {
        L"Grid.ColumnSpan=1",
        L"MaxWidth=65"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[1]", {
        L"Content=>t1",
        L"ToolTipService.ToolTip={{t1}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[2]", {
        L"Content=>t2",
        L"ToolTipService.ToolTip={{t2}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[3]", {
        L"Content=>t3",
        L"ToolTipService.ToolTip={{t3}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[4]", {
        L"Content=>t4",
        L"ToolTipService.ToolTip={{t4}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[5]", {
        L"Content=>t5",
        L"ToolTipService.ToolTip={{t5}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[6]", {
        L"Content=>t6",
        L"ToolTipService.ToolTip={{t6}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[7]", {
        L"Content=>t7",
        L"ToolTipService.ToolTip={{t7}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[8]", {
        L"Content=>t8",
        L"ToolTipService.ToolTip={{t8}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[9]", {
        L"Content=>t9",
        L"ToolTipService.ToolTip={{t9}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[10]", {
        L"Content=>t10",
        L"ToolTipService.ToolTip={{t10}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[11]", {
        L"Content=>t11",
        L"ToolTipService.ToolTip={{t11}}"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[12]", {
        L"Content=>t12",
        L"ToolTipService.ToolTip={{t12}}"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid[2]", {
        L"Visibility=1",
        L"Grid.Column=0"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid[2] > TextBlock#UserName", {
        L"Text=>UserName"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton", {
        L"ToolTipService.ToolTip={{UserName}}",
        L"ToolTipService.Placement=10",
        L"Visibility=1"}},
    ThemeTargetStyles{L"ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton > ContentPresenter#ContentPresenter > Grid#UserProfileLayout > Grid#UserImageGrid > Image", {
        L"Width=30",
        L"Height=30"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel", {
        L"MaxHeight=48",
        L"MaxWidth=65",
        L"MinHeight=48",
        L"MinWidth=65",
        L"Visibility=1"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#PaneRoot > Border > Grid#PaneContentGrid > ContentControl#PaneCustomContentBorder > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.UserProfileControl#UserProfileControl > Button#UserProfileButton", {
        L"MinHeight=48",
        L"MaxHeight=48",
        L"Margin=3,3,3,-3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ProgressBarIndicator", {
        L"RadiusX=3",
        L"RadiusY=3",
        L"Height=6",
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource Accent}\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#DeterminateRoot", {
        L"CornerRadius=3",
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ProgressBar", {
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#DeterminateRoot > Windows.UI.Xaml.Shapes.Rectangle#ProgressBarIndicator", {
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#DeterminateRoot", {
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar", {
        L"Height=16"}},
    ThemeTargetStyles{L"CheckBox > Grid#RootGrid@CombinedStates > Grid > Rectangle#NormalRectangle", {
        L"StrokeThickness=1",
        L"Stroke=#27292E",
        L"Fill@UncheckedNormal=black",
        L"Fill@UncheckedPointerOver=black",
        L"Fill@UncheckedPointerOverSelected=black",
        L"Fill@CheckedNormal=<SolidColorBrush Color=\"{ThemeResource Accent}\"/>",
        L"Fill@CheckedPointerOver=<SolidColorBrush Color=\"{ThemeResource Accent}\"/>",
        L"Fill@CheckedPointerOverSelected=<SolidColorBrush Color=\"{ThemeResource Accent}\"/>",
        L"RadiusX=6",
        L"RadiusY=6"}},
    ThemeTargetStyles{L"SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.WatermarkTextBox > Grid > Border#BorderElement", {
        L"Background=#101013",
        L"CornerRadius=12",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.HighContrastThemesCombobox", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=3",
        L"Background=Black",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.HighContrastThemesCombobox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"HyperlinkButton > ContentPresenter#ContentPresenter", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />"}},
    ThemeTargetStyles{L"ProgressBar > Grid > Border#DeterminateRoot > Rectangle#ProgressBarIndicator", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=3",
        L"Background=Black",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=3",
        L"Background=Black",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > StackPanel > ContentPresenter#TitleContent > StackPanel > RadioButton > Grid#RootGrid > Grid > Windows.UI.Xaml.Shapes.Ellipse#CheckOuterEllipse", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"StrokeThickness=0"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Grid > Slider > Grid > Grid#SliderContainer > Grid#HorizontalTemplate > Rectangle#HorizontalTrackRect", {
        L"Fill=black",
        L"Margin=0,0,5,0",
        L"StrokeThickness=0"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Grid > Slider > Grid > Grid#SliderContainer > Grid#HorizontalTemplate > Rectangle#HorizontalDecreaseRect", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"StrokeThickness=0"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Grid > Slider > Grid > Grid#SliderContainer > Grid#HorizontalTemplate > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Border > Windows.UI.Xaml.Shapes.Ellipse#SliderInnerThumb", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Height=0",
        L"Width=0"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Grid > Slider > Grid > Grid#SliderContainer > Grid#HorizontalTemplate > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Border", {
        L"Width=17",
        L"Height=17",
        L"Margin=-5,0,0,0",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />"}},
    ThemeTargetStyles{L"CheckBox > Grid#RootGrid@CombinedStates > Grid > Microsoft.UI.Xaml.Controls.AnimatedIcon#CheckGlyph", {
        L"Width=25",
        L"Height=25",
        L"Margin=-2,-1,0,0"}},
    ThemeTargetStyles{L"Button#focusStartButton > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=Black",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusEnableControl#FocusEnableControl > StackPanel > Button#focusStartButton > ContentPresenter#ContentPresenter > Grid > TextBlock#StartButtonText", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusEnableControl#FocusEnableControl > StackPanel > Button#focusStopButton > ContentPresenter#ContentPresenter", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=3",
        L"Background=Black",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusSessionControl > StackPanel > Grid > Button#focusSessionDecreaseButton > Grid > ContentPresenter#ContentPresenter", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=4",
        L"Background=Black",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusSessionControl > StackPanel > Grid > Button#focusSessionIncreaseButton > Grid > ContentPresenter#ContentPresenter > FontIcon > Grid > TextBlock", {
        L"Padding=2,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusSessionControl > StackPanel > Grid > Button#focusSessionDecreaseButton > Grid > ContentPresenter#ContentPresenter > FontIcon > Grid > TextBlock", {
        L"Padding=2,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusSessionControl > StackPanel > Grid > Grid > TextBlock#focusSessionDurationTextBlock", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusSessionControl > StackPanel > Grid > Button#focusSessionIncreaseButton > Grid > ContentPresenter#ContentPresenter", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=4",
        L"Background=Black",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"SystemSettings.View.FocusEnableControl#FocusEnableControl > StackPanel > Button#focusStopButton > ContentPresenter#ContentPresenter > Grid > TextBlock#StopButtonText", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl#heroContent > ContentPresenter > SystemSettings.View.AlignableContentControl > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#DefaultOneSegmentHeroUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"FontWeight=Bold",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > UserControl > StackPanel > Button#WindowsProtectedPrintButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"FontSize=14",
        L"FontWeight=Bold",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > UserControl > StackPanel > Button#WindowsProtectedPrintButton > ContentPresenter#ContentPresenter", {
        L"Background=Black",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"Foreground=black",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground=black",
        L"FontSize=14",
        L"FontWeight=Bold",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"SystemSettings.View.StableComboBox > Grid#LayoutRoot > Border#Background", {
        L"Background=Black",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootPageGrid", {
        L"Background=#101013"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Microsoft.UI.Xaml.Controls.DropDownButton", {
        L"Background=Black",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Microsoft.UI.Xaml.Controls.DropDownButton > Grid#RootGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"Background=Black",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > Border#Background", {
        L"Background=Black",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > Border#Background", {
        L"Background=Black",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button#ClassicAppButton > ContentPresenter#ContentPresenter", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Background=Black",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button#ClassicAppButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontWeight=bold"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsListView#DevicesHeroControlList > Border > ScrollViewer#ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > ItemsPresenter > StackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter#Root > ContentControl > ContentPresenter > UserControl > Button#DevicesHeroControlButton > ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@Normal:=#040b07",
        L"Background@PointerOver:=#06150d",
        L"Background@Pressed:=#04150c",
        L"Background@Disabled:=#010b04",
        L"BorderThickness=2",
        L"BorderBrush@Normal:=#0c190f",
        L"BorderBrush@PointerOver:=#0c190f",
        L"BorderBrush@Pressed:=#0c190f",
        L"BorderBrush@Disabled:=#000000",
        L"CornerRadius=14",
        L"Margin=3"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl#heroContent > ContentPresenter > SystemSettings.View.AlignableContentControl > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#DefaultOneSegmentHeroUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > SystemSettings.View.DevicesHeroControl > Grid > Button > ContentPresenter#ContentPresenter", {
        L"Background:=#040b07",
        L"Margin=3",
        L"CornerRadius=14",
        L"BorderBrush:=#0c190f",
        L"BorderThickness=2"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button#ModernAppButton > ContentPresenter#ContentPresenter", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Background=Black",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.FullScreenPage#FullScreenPage > Grid#MainGrid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button#ModernAppButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontWeight=bold"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsListItemsRepeater > ScrollViewer#SettingsListScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#ItemsRepeater > SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent > ContentControl > ContentPresenter > SystemSettings.View.SpacingStackPanel > SystemSettings.View.EntityItem#DeviceRemoveButtonContent > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=1",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter > Grid > SystemSettings.View.ReservedWidthReflowingPanel#ReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > TextBlock", {
        L"FontWeight=bold"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot > TextBlock", {
        L"FontWeight=bold"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > SystemSettings.View.StableComboBox > Grid#LayoutRoot", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=1",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#OuterBorder", {
        L"Fill=#1B1B20",
        L"Height=33",
        L"Width=55",
        L"RadiusX=15",
        L"RadiusY=20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobOff", {
        L"Height=18",
        L"Width=18",
        L"RadiusX=25",
        L"RadiusY=25",
        L"Margin=5,0,-5,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SwitchKnobOn", {
        L"Height=20",
        L"Width=20",
        L"CornerRadius=25",
        L"Margin=10,0,-10,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#SwitchKnobBounds", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Height=35",
        L"Width=55",
        L"RadiusX=15",
        L"RadiusY=20"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid", {
        L"Background=#000000",
        L"CornerRadius=14,0,0,0",
        L"BorderBrush=#25262B",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView", {
        L"Background=#000000",
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemSettings.View.EntityItem", {
        L"Background=#101013",
        L"Foreground=White",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.SettingsListItemsRepeater > ScrollViewer#SettingsListScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Microsoft.UI.Xaml.Controls.ItemsRepeater#ItemsRepeater > SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter > Grid > ContentPresenter > SystemSettings.View.EntityItem#EntityExpandableListItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > Button > ContentPresenter#ContentPresenter", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Width=150",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl#heroContent > ContentPresenter > SystemSettings.View.AlignableContentControl > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#DefaultOneSegmentHeroUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > Button > ContentPresenter#ContentPresenter", {
        L"Background=#141417",
        L"Width=150",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=2",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter", {
        L"Background=#101013",
        L"Background@PointerOver=#101013",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter", {
        L"Background=#101013",
        L"Background@PointerOver=#101013",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > StackPanel > ContentPresenter > SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter", {
        L"Background=#101013",
        L"Background@PointerOver=#101013",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"StackPanel#BackgroundStackPanel", {
        L"Background=#101013",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"Rectangle#SelectionIndicator", {
        L"Height=0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[Content=Home] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE80F",
        L"Content@PointerOver:=\uE80F",
        L"Content@Pressed:=\uE80F",
        L"Content@Selected:=\uEA8A",
        L"Content@PointerOverSelected:=\uEA8A",
        L"Content@PressedSelected:=\uEA8A",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[Content=System] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE75B",
        L"Content@PointerOver:=\uE75B",
        L"Content@Pressed:=\uE75B",
        L"Content@Selected:=\uE75B",
        L"Content@PointerOverSelected:=\uE75B",
        L"Content@PressedSelected:=\uE75B",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[Content=Bluetooth & devices] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE702",
        L"Content@PointerOver:=\uE702",
        L"Content@Pressed:=\uE702",
        L"Content@Selected:=\uE702",
        L"Content@PointerOverSelected:=\uE702",
        L"Content@PressedSelected:=\uE702",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[3] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uF0C5",
        L"Content@PointerOver:=\uF0C5",
        L"Content@Pressed:=\uF0C5",
        L"Content@Selected:=\uF0C5",
        L"Content@PointerOverSelected:=\uF0C5",
        L"Content@PressedSelected:=\uF0C5",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[4] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uEA18",
        L"Content@PointerOver:=\uEA18",
        L"Content@Pressed:=\uEA18",
        L"Content@Selected:=\uE83D",
        L"Content@PointerOverSelected:=\uE83D",
        L"Content@PressedSelected:=\uE83D",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[5] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE776",
        L"Content@PointerOver:=\uE776",
        L"Content@Pressed:=\uE776",
        L"Content@Selected:=\uE776",
        L"Content@PointerOverSelected:=\uE776",
        L"Content@PressedSelected:=\uE776",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[6] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE7FC",
        L"Content@PointerOver:=\uE7FC",
        L"Content@Pressed:=\uE7FC",
        L"Content@Selected:=\uE7FC",
        L"Content@PointerOverSelected:=\uE7FC",
        L"Content@PressedSelected:=\uE7FC",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[7] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE775",
        L"Content@PointerOver:=\uE775",
        L"Content@Pressed:=\uE775",
        L"Content@Selected:=\uE775",
        L"Content@PointerOverSelected:=\uE775",
        L"Content@PressedSelected:=\uE775",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[8] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE77B",
        L"Content@PointerOver:=\uE77B",
        L"Content@Pressed:=\uE77B",
        L"Content@Selected:=\uEA8C",
        L"Content@PointerOverSelected:=\uEA8C",
        L"Content@PressedSelected:=\uEA8C",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[9] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE74C",
        L"Content@PointerOver:=\uE74C",
        L"Content@Pressed:=\uE74C",
        L"Content@Selected:=\uE74C",
        L"Content@PointerOverSelected:=\uE74C",
        L"Content@PressedSelected:=\uE74C",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[10] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE771",
        L"Content@PointerOver:=\uE771",
        L"Content@Pressed:=\uE771",
        L"Content@Selected:=\uE771",
        L"Content@PointerOverSelected:=\uE771",
        L"Content@PressedSelected:=\uE771",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[11] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE701",
        L"Content@PointerOver:=\uE701",
        L"Content@Pressed:=\uE701",
        L"Content@Selected:=\uE701",
        L"Content@PointerOverSelected:=\uE701",
        L"Content@PressedSelected:=\uE701",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem[12] > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"Content@Normal:=\uE702",
        L"Content@PointerOver:=\uE702",
        L"Content@Pressed:=\uE702",
        L"Content@Selected:=\uE702",
        L"Content@PointerOverSelected:=\uE702",
        L"Content@PressedSelected:=\uE702",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground=#FFFFFF"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Padding=10,0,0,0"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > Border#IconColumn > Viewbox#IconBox > Border > ContentPresenter#Icon", {
        L"FontFamily=Segoe Fluent Icons",
        L"FontSize=20",
        L"Margin=15,0,-2,0"}},
    ThemeTargetStyles{L"ContentPresenter#IconContentPresenter", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates", {
        L"Background@Normal=Transparent",
        L"Height=48",
        L"Margin=8,0,8,0",
        L"Padding= -4",
        L"Background@PointerOver=#101013",
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark3}\" />",
        L"Background@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark3}\" />",
        L"Background@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark3}\" />",
        L"Background@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark3}\" />",
        L"CornerRadius@Normal=10",
        L"CornerRadius=30"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsNavigationViewItem > Grid#NVIRootGrid > Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Grid#LayoutRoot@PointerStates > Grid#PresenterContentRootGrid > Grid#ContentGrid > ContentPresenter#ContentPresenter > TextBlock", {
        L"Foreground@Normal=White",
        L"Foreground@PointerOver=White",
        L"Foreground@Pressed=White",
        L"Foreground@Selected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PointerOverSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"Foreground@PressedSelected:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"FontWeight=Bold",
        L"FontSize@Selected=16",
        L"FontSize@PointerOverSelected=16"}},
    ThemeTargetStyles{L"SystemSettings.View.UserProfileControl#UserProfileControl", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#UserProfileButton", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ThumbVisual", {
        L"Fill=#1E1E1E"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#NavigationViewBackButton > Windows.UI.Xaml.Controls.Grid#RootGrid", {
        L"Background=#1A1A1F",
        L"CornerRadius=25",
        L"Height=30",
        L"Width=30"}},
    ThemeTargetStyles{L"SystemSettings.View.AlignableContentControl#PermanentNavViewHeaderAlignControl > ContentPresenter", {
        L"Background=#101013",
        L"CornerRadius=25",
        L"Padding=25,5"}},
    ThemeTargetStyles{L"SystemSettings.View.AlignableContentControl#PermanentNavViewHeaderAlignControl > ContentPresenter > Microsoft.UI.Xaml.Controls.BreadcrumbBar#PermanentNavigationViewBreadcrumbBar > Microsoft.UI.Xaml.Controls.ItemsRepeater#PART_ItemsRepeater > Microsoft.UI.Xaml.Controls.BreadcrumbBarItem > Grid#PART_LayoutRoot > ContentPresenter#PART_LastItemContentPresenter", {
        L"Padding=20,0,20,0",
        L"Height=45",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark3}\" />",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=25"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter", {
        L"Background=#101013",
        L"CornerRadius=12,12,12,12",
        L"Margin=0,2,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#SettingsCommandSearchBoxBackground", {
        L"Height=30",
        L"Background=#18181B",
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView", {
        L"Background=#101013"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"Foreground=Black",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > SystemSettings.View.SpacingStackPanel > SystemSettings.View.ExpandItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter", {
        L"Foreground=Black",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.CategoryPage > Grid > ScrollViewer > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.SettingsListView#settingPagesList > ItemsPresenter > ItemsStackPanel > SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > Button > ContentPresenter#ContentPresenter > TextBlock", {
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#OneSegmentHeroEntityItemUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > StackPanel > Button > ContentPresenter#ContentPresenter", {
        L"Foreground=Black",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid > ContentPresenter#ContentPresenter > Frame#PermanentNavRootFrame > ContentPresenter > SystemSettings.View.L2Page#L2Page > Grid > Grid > SystemSettings.View.AlignableContentControl > ContentPresenter > Grid > SystemSettings.View.SettingsPageHost#pageContent > ScrollViewer#SettingsPageHostPanel > Border#Root > Grid > ScrollContentPresenter#ScrollContentPresenter > Grid#RootScrollableGrid > Grid > Grid > ContentControl > ContentPresenter > ItemsControl > ItemsPresenter > SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.AlignableContentControl > ContentPresenter > SystemSettings.View.TwoSegmentsHeroUserControl#OneSegmentHeroEntityItemUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > SystemSettings.View.EntityItem > Grid > SystemSettings.View.ReservedWidthReflowingPanel > ContentPresenter#InlineContentPresenter > StackPanel > StackPanel > Button", {
        L"Foreground=black",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=12",
        L"FontSize=14",
        L"FontWeight=Bold"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Foreground=Black",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=12",
        L"FontWeight=Bold",
        L"FontSize=14",
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBox#CommandSearchTextBox", {
        L"Background=#1A1A1F",
        L"CornerRadius=12",
        L"Foreground=White",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Frame > ContentPresenter > SystemSettings.View.RootPage > Grid#RootPageGrid > Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid > SplitView#RootSplitView > Grid > Grid#PaneRoot", {
        L"Background=Transparent"}},
}};

const Theme g_themeTranslucent_Seamless = {{
    ThemeTargetStyles{L"SystemSettings.View.SettingsExpander > Grid > SystemSettings.View.ExpanderToggleButton#ContainerButton > ContentPresenter#ContentPresenter", {
        L"CornerRadius:=12,12,12,12"}},
    ThemeTargetStyles{L"SystemSettings.View.SpacingStackPanel > ContentPresenter > SystemSettings.View.EntityItem > Grid", {
        L"CornerRadius:=12"}},
    ThemeTargetStyles{L"SystemSettings.View.EntityItem#BluetoothRadioToggleEntityItem > Grid", {
        L"CornerRadius:=12"}},
    ThemeTargetStyles{L"SystemSettings.View.TwoSegmentsHeroUserControl#DefaultOneSegmentHeroUserControl > Grid#LayoutRoot > Grid#LeftLayout > ContentPresenter > ItemsControl > ItemsPresenter > StackPanel > ContentPresenter > StackPanel > Button > ContentPresenter#ContentPresenter", {
        L"CornerRadius:=12",
        L"Width=250"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsExpander > Grid > ContentPresenter#RevealedContent", {
        L"CornerRadius:=12"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > SystemSettings.View.EntityItem > Grid", {
        L"CornerRadius:=$InRadius"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsListViewItem > Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > Border", {
        L"CornerRadius:=$InRadius"}},
    ThemeTargetStyles{L"SystemSettings.View.ButtonEntityItem > Button#ContainerButton > ContentPresenter#ContentPresenter", {
        L"CornerRadius:=$InRadius"}},
    ThemeTargetStyles{L"Grid#PaneRoot", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" FallbackColor=\"#00000000\" TintColor=\"#101010\" TintLuminosityOpacity=\"0.45\" TintOpacity=\"0.4\"/>",
        L"BorderThickness=0",
        L"BorderBrush:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"SplitView#RootSplitView", {
        L"PaneBorderThickness=0",
        L"BorderThickness=0",
        L"PaneBorderBrush:=<SolidColorBrush Color=\"#00000000\"/>",
        L"BorderBrush:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Rectangle", {
        L"Visibility=Collapsed",
        L"Width=0"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Border#PaneBorder", {
        L"BorderThickness=0",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#ContentRoot", {
        L"Margin=-2,0,0,0",
        L"Background:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#ContentRoot > Border", {
        L"BorderThickness=0",
        L"BorderBrush:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"SplitView#RootSplitView > Grid > Grid#ContentRoot > Border > Grid#ContentGrid", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" FallbackColor=\"#00000000\" TintColor=\"#101010\" TintLuminosityOpacity=\"0.45\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=0",
        L"BorderThickness=0",
        L"BorderBrush:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"Grid#ContentRoot", {
        L"Background:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"Grid#ContentRoot > Border > Grid#ContentGrid > ContentControl#HeaderContent", {
        L"Background:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"Frame#PermanentNavRootFrame", {
        L"Background:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"SystemSettings.View.RootPage > Grid#RootPageGrid", {
        L"Background:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid", {
        L"Background:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.NavigationView#PermanentNavigationView > Grid#RootGrid > Grid", {
        L"Background:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"Grid#TitleBar", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" FallbackColor=\"#00000000\" TintColor=\"#101010\" TintLuminosityOpacity=\"0.45\" TintOpacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Border#TitleBarBackground", {
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" FallbackColor=\"#00000000\" TintColor=\"#101010\" TintLuminosityOpacity=\"0.45\" TintOpacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"StackPanel#SettingsCommandSearchBoxBackground", {
        L"CornerRadius=$InRadius",
        L"MinHeight=32"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox", {
        L"CornerRadius=$InRadius",
        L"VerticalContentAlignment=Center",
        L"Background:=<AcrylicBrush BackgroundSource=\"HostBackdrop\" FallbackColor=\"#00000000\" TintColor=\"#252525\" TintLuminosityOpacity=\"0.6\" TintOpacity=\"0.5\"/>",
        L"BorderBrush:=<SolidColorBrush Color=\"#22FFFFFF\"/>",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"TextBox#CommandSearchTextBox > Grid > ScrollViewer", {
        L"VerticalAlignment=Center"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ProgressBarIndicator", {
        L"RadiusX=4",
        L"RadiusY=4",
        L"Height=6",
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource Accent}\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#DeterminateRoot", {
        L"CornerRadius=3",
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ProgressBar", {
        L"Height=6"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#DeterminateRoot > Windows.UI.Xaml.Shapes.Rectangle#ProgressBarIndicator", {
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#DeterminateRoot", {
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel#TopBreakdownBar > Windows.UI.Xaml.Controls.ProgressBar", {
        L"Height=16"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsExpander > Grid", {
        L"BorderThickness=0",
        L"BorderBrush:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"SystemSettings.View.SettingsExpander", {
        L"BorderThickness=0",
        L"BorderBrush:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter", {
        L"BorderThickness=0",
        L"BorderBrush:=<SolidColorBrush Color=\"#00000000\"/>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Primitives.ListViewItemPresenter > Border", {
        L"BorderThickness=0",
        L"BorderBrush:=<SolidColorBrush Color=\"#00000000\"/>"}},
}, {
    L"OutRadius=8",
    L"InRadius=10",
    L"BgBorder=<SolidColorBrush Color=\"#00000000\"/>",
}, {
    L"Overlay@Light=#55FFFFFF",
    L"Overlay@Dark=#09FFFFFF",
    L"Border@Light=#00000000",
    L"Border@Dark=#00000000",
    L"Accent@Dark={ThemeResource SystemAccentColorLight2}",
    L"Accent@Light={ThemeResource SystemAccentColorDark1}",
    L"WindowCaptionBackground@Dark=#00000000",
    L"WindowCaptionBackground@Light=#00000000",
    L"WindowCaptionBackgroundDisabled@Dark=#00000000",
    L"WindowCaptionBackgroundDisabled@Light=#00000000",
    L"SolidBackgroundFillColorBase@Dark=#00000000",
    L"SolidBackgroundFillColorBase@Light=#00000000",
    L"SolidBackgroundFillColorSecondary@Dark=#00000000",
    L"SolidBackgroundFillColorSecondary@Light=#00000000",
    L"LayerFillColorDefault@Dark=#00000000",
    L"LayerFillColorDefault@Light=#00000000",
    L"ApplicationPageBackgroundThemeBrush@Dark=#00000000",
    L"ApplicationPageBackgroundThemeBrush@Light=#00000000",
    L"CardStrokeColorDefault@Dark=#00000000",
    L"CardStrokeColorDefault@Light=#00000000",
    L"ControlStrokeColorDefault@Dark=#00000000",
    L"ControlStrokeColorDefault@Light=#00000000",
    L"SurfaceStrokeColorDefault@Dark=#00000000",
    L"SurfaceStrokeColorDefault@Light=#00000000",
    L"NavigationViewItemSeparatorForeground@Dark=#00000000",
    L"NavigationViewItemSeparatorForeground@Light=#00000000",
    L"SystemControlForegroundBaseLowBrush@Dark=#00000000",
    L"SystemControlForegroundBaseLowBrush@Light=#00000000",
    L"SplitViewPaneBorderBrush@Dark=#00000000",
    L"SplitViewPaneBorderBrush@Light=#00000000",
    L"NavigationViewBorderBrush@Dark=#00000000",
    L"NavigationViewBorderBrush@Light=#00000000",
}};

// clang-format on

std::atomic<DWORD> g_targetThreadId = 0;

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
                         winrt::Windows::UI::Xaml::FrameworkElement element,
                         PCWSTR fallbackClassName);
void CleanupCustomizations(ElementId elementId);
void QueueDiagnosticsRelease(InstanceHandle handle);
void FlushDiagnosticsReleasesIfQuiet();
void TrackSplitView(winrt::Windows::UI::Xaml::FrameworkElement element);
void TrackSplitViewChildHost(
    winrt::Windows::UI::Xaml::FrameworkElement element);

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

// The XAML composition diagnostics rebuild a process-wide visual tree walker
// without any locking whenever a DirectComposition visual is added, so any UI
// thread which adds one corrupts the heap while another thread is in the same
// code. Only element mutations are needed here, and those are reported by an
// unrelated code path, so the composition diagnostics are kept from being
// created at all: XamlDiagnostics::CreateCompVisualDiag skips them when the
// HKLM\Software\Microsoft\XAML\Debug\DisableCompositionDiag value is 1.
// Windows.UI.Xaml.dll reads and caches the value once, from within
// AdviseVisualTreeChange, so answering that single read is enough.
thread_local bool g_reportCompositionDiagAsDisabled;

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
        namespace UI::Xaml {}
    }
}

// alias some long namespaces for convenience
namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;

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

#include <winrt/Windows.UI.Xaml.h>

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
            auto service = watcher->m_XamlDiagnostics.as<IVisualTreeService3>();
            g_reportCompositionDiagAsDisabled = true;
            HRESULT hr = service->AdviseVisualTreeChange(watcher);
            g_reportCompositionDiagAsDisabled = false;
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

    // Caught here rather than by the handler below, so that the bookkeeping
    // which hands the element's reference back still runs when the styling work
    // throws. Otherwise a single failed element would be held for good.
    try
    {
        if (mutationType == Add)
        {
            const auto inspectable = FromHandle(element.Handle);
            auto elementId = GetOrCreateElementId(element.Handle, inspectable);
            auto frameworkElement = inspectable.try_as<wux::FrameworkElement>();
            if (frameworkElement)
            {
                Wh_Log(L"FrameworkElement name: %s", frameworkElement.Name().c_str());
                TrackSplitView(frameworkElement);
                TrackSplitViewChildHost(frameworkElement);
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

// Read by the UI threads while the thread which injects or uninitializes the TAP
// replaces it.
[[clang::no_destroy]] winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;
SRWLOCK g_visualTreeWatcherLock = SRWLOCK_INIT;

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

winrt::com_ptr<VisualTreeWatcher> GetVisualTreeWatcher()
{
    AcquireSRWLockShared(&g_visualTreeWatcherLock);
    auto watcher = g_visualTreeWatcher;
    ReleaseSRWLockShared(&g_visualTreeWatcherLock);
    return watcher;
}

// Hands the previous watcher back to be unadvised and released outside the
// lock: both can wait on the UI threads, which take it.
winrt::com_ptr<VisualTreeWatcher> ExchangeVisualTreeWatcher(winrt::com_ptr<VisualTreeWatcher> watcher)
{
    AcquireSRWLockExclusive(&g_visualTreeWatcherLock);
    std::swap(g_visualTreeWatcher, watcher);
    ReleaseSRWLockExclusive(&g_visualTreeWatcherLock);
    return watcher;
}

HRESULT WindhawkTAP::SetSite(IUnknown *pUnkSite) try
{
    // Only ever 1 VTW at once.
    if (auto previous = ExchangeVisualTreeWatcher(nullptr))
    {
        previous->UnadviseVisualTreeChange();
    }

    site.copy_from(pUnkSite);

    if (site)
    {
        // Decrease refcount increased by InitializeXamlDiagnosticsEx.
        FreeLibrary(GetCurrentModuleHandle());

        ExchangeVisualTreeWatcher(winrt::make_self<VisualTreeWatcher>(site));
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
#include <roapi.h>
#include <shlwapi.h>
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

#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

using namespace winrt::Windows::UI::Xaml;

namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
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
std::unordered_map<void*, std::weak_ptr<ElementTreeNode>> g_elementTreeNodes;

// Expired pool entries are reaped once the map grows past this, which is then
// set to twice the surviving size, making the sweep amortized O(1).
size_t g_elementTreeNodesReapThreshold = 64;

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

std::unordered_map<ElementId, ElementCustomizationState>
    g_elementsCustomizationState;

// Empties a thread local container and frees its storage, which clear() keeps.
// A thread local isn't destroyed when the module is unloaded while its thread
// lives on, so what it still holds then is leaked.
template <typename T>
void ResetThreadLocal(T& container) {
    container = T{};
}

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
    // Elements whose captures are out of `variables` while they sit in a
    // recycled repeater item. See RefreshRecycledItemCaptures.
    std::unordered_set<ElementId> withdrawnCaptureOwners;
};

StyleVariableState g_styleVariableState;

// Non-zero while PropagateStyleVariableChange is running, so nested calls queue
// instead of recursing.
int g_styleVariablePropagationDepth;

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
std::vector<PendingStyleVariablePropagation> g_pendingStyleVariablePropagations;

StyleVariableState* GetStyleVariableState() {
    return &g_styleVariableState;
}

bool g_elementPropertyModifying;

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

    // The BitmapImage the last retry assigned to the target, which is the mod's
    // value just like the one the style declared.
    winrt::weak_ref<Media::Imaging::BitmapImage> retryImage;

    ULONGLONG lastRetryTick = 0;
    int retryCount = 0;
};

struct TrackedImagesForThread {
    // Entries are held by shared_ptr so that event handlers can reference them
    // via a weak_ptr and do nothing once an entry is gone.
    std::list<std::shared_ptr<TrackedImage>> images;
    winrt::Windows::System::DispatcherQueue dispatcher{nullptr};
    winrt::Windows::System::DispatcherQueueTimer retryTimer{nullptr};
    winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
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
// Set once the mod is being unloaded, so that nothing which runs during the
// uninitialization tracks an image again.
bool g_imageRetryStopping;
// The dispatcher of each UI thread which has tracked images, used to run a
// retry on the thread that owns the image.
std::vector<winrt::weak_ref<winrt::Windows::System::DispatcherQueue>>
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

// The last stream generated, cached to avoid regenerating when density hasn't
// changed. The cached stream is never read directly; callers get independent
// clones via CloneStream() so they don't share a seek cursor.
thread_local float g_noiseStreamDensity =
    std::numeric_limits<float>::quiet_NaN();
thread_local winrt::Windows::Storage::Streams::InMemoryRandomAccessStream
    g_noiseStream{nullptr};

winrt::Windows::Storage::Streams::IRandomAccessStream CreateNoiseStream(
    float density) {
    if (density == g_noiseStreamDensity && g_noiseStream) {
        return g_noiseStream.CloneStream();
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

    g_noiseStream = std::move(stream);
    g_noiseStreamDensity = density;

    return g_noiseStream.CloneStream();
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
        tracked->retryImage = winrt::make_weak(retryImage);

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
    if (GetCurrentThreadId() != g_targetThreadId) {
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
    if (GetCurrentThreadId() != g_targetThreadId) {
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
                [](winrt::Windows::System::DispatcherQueueTimer const&,
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
        std::lock_guard<std::mutex> lock(g_imageRetryMutex);

        g_imageRetryPendingCallbacks--;

        // Under the lock, since the module can be freed as soon as the waiter
        // sees the count drop.
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

    std::vector<winrt::Windows::System::DispatcherQueue> dispatchers;
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

    // Runs on a Windows Runtime thread pool thread, where the connectivity
    // query is allowed and doesn't hold up a UI thread.
    ScheduleImageLoadRetryOnAllUiThreads();
}

// Must not be called with g_imageRetryMutex held.
winrt::event_token RegisterNetworkStatusChangedHandler() {
    // Removing the handler doesn't wait for an invocation which is already in
    // flight, and the event source releases the handler, which is mod code,
    // only after that invocation. The handler holds the reference so that it
    // covers both.
    auto callbackRef = TrackImageRetryCallback();
    if (!callbackRef) {
        return {};
    }

    try {
        auto token = winrt::Windows::Networking::Connectivity::
            NetworkInformation::NetworkStatusChanged(
                [callbackRef](
                    winrt::Windows::Foundation::IInspectable const& sender) {
                    OnNetworkStatusChanged(sender);
                });
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
        g_imageRetryStopping = true;

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
    // A callback still returns through mod code after dropping its reference,
    // which the rest of the uninitialization is left to outlast.
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

Media::Imaging::BitmapImage SetupImageTracking(
    DependencyObject const& target,
    DependencyProperty const& sourceProperty,
    Media::Imaging::BitmapImage const& bitmapImage,
    winrt::Windows::Foundation::Uri const& uri) {
    {
        std::lock_guard<std::mutex> lock(g_imageRetryMutex);

        if (g_imageRetryStopping) {
            return nullptr;
        }
    }

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
        // Keep the retry state which was collected so far unless the source
        // changed.
        if ((*it)->uri.Equals(uri)) {
            RestoreRejectedImageSource(*it, bitmapImage);

            // A declared image which shows nothing has typically failed, and
            // isn't loaded again by being reapplied, so it would blank a retry
            // image which has loaded since, or cut short one which is still
            // loading. The target keeps its retry image instead.
            Media::Imaging::BitmapImage retryImage{nullptr};
            if (bitmapImage.PixelWidth() == 0) {
                retryImage = (*it)->retryImage.get();
            }
            auto const& applied = retryImage ? retryImage : bitmapImage;

            // The image being applied is what a load failure is judged by, and
            // what decides whether the target has an image.
            (*it)->usingCache = !applied.UriSource().Equals(uri);

            bool wasLoaded = (*it)->loaded;
            (*it)->loaded = applied.PixelWidth() != 0;

            // Nothing else starts a round for a target which loses its image
            // this way: the value may have failed already, which raises no
            // further event.
            if (wasLoaded && !(*it)->loaded) {
                ScheduleImageLoadRetryOnCurrentThread(kImageRetryBaseDelayMs,
                                                      /*reschedule=*/false);
            }
            return retryImage;
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

        // Checked again, since StopImageLoadRetries runs on another thread.
        if (g_imageRetryStopping) {
            return nullptr;
        }

        g_imageRetryActive = true;

        if (!g_trackedImagesForThread.dispatcher) {
            try {
                auto dispatcher = winrt::Windows::System::DispatcherQueue::
                    GetForCurrentThread();
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
        return nullptr;
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

    return nullptr;
}

// Tracks the target if the image source is a remote URL, which can fail to
// load and is worth caching. Returns the image to apply in place of
// `imageSource`, null if it's `imageSource` itself.
Media::Imaging::BitmapImage TrackIfRemoteImageSource(
    DependencyObject const& target,
    DependencyProperty const& sourceProperty,
    winrt::Windows::Foundation::IInspectable const& imageSource) {
    auto bitmapImage = imageSource.try_as<Media::Imaging::BitmapImage>();
    if (!bitmapImage) {
        return nullptr;
    }

    auto uri = bitmapImage.UriSource();
    if (!uri) {
        return nullptr;
    }

    auto scheme = uri.SchemeName();
    if (scheme == L"file") {
        // A cached file which was substituted for its remote address, by which
        // the entry is identified. A style value is shared by many targets, so
        // the substitution a previous target's entry made is what the rest of
        // them are given.
        auto it = g_imageCacheUriRemotes.find(std::wstring(uri.RawUri()));
        if (it == g_imageCacheUriRemotes.end()) {
            return nullptr;
        }

        uri = it->second;
    } else if (scheme != L"http" && scheme != L"https") {
        return nullptr;
    }

    return SetupImageTracking(target, sourceProperty, bitmapImage, uri);
}

// Stops tracking the image which the target shows through `sourceProperty`.
void UntrackImageTarget(DependencyObject const& target,
                        DependencyProperty const& sourceProperty) {
    std::erase_if(g_trackedImagesForThread.images, [&](const auto& tracked) {
        return tracked->sourceProperty == sourceProperty &&
               tracked->target.get() == target;
    });
}

// Whether `value` is the image a retry assigned to the target's
// `sourceProperty`.
bool IsImageRetryValue(DependencyObject const& target,
                       DependencyProperty const& sourceProperty,
                       winrt::Windows::Foundation::IInspectable const& value) {
    if (!value) {
        return false;
    }

    const auto& images = g_trackedImagesForThread.images;
    return std::any_of(images.begin(), images.end(), [&](const auto& tracked) {
        return tracked->sourceProperty == sourceProperty &&
               tracked->retryImage.get() == value &&
               tracked->target.get() == target;
    });
}

// Returns the value written, UnsetValue for a clear, so that callers can
// record what they applied. Reading it back is not an option: while a visual
// state has a setter on the property, ReadLocalValue keeps reporting the value
// from before that state rather than the one written.
winrt::Windows::Foundation::IInspectable SetOrClearValue(
    DependencyObject elementDo,
    DependencyProperty property,
    const PropertyOverrideValue& overrideValue,
    bool initialApply = false,
    bool trackImage = true) {
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
    // Image element. A value being restored isn't tracked: it's the app's own,
    // and during uninitialization tracking it would register handlers which
    // outlive the mod.
    if (trackImage) {
        if (auto imageBrush = value.try_as<Media::ImageBrush>()) {
            TrackIfRemoteImageSource(imageBrush,
                                     Media::ImageBrush::ImageSourceProperty(),
                                     imageBrush.ImageSource());
        } else if (auto imageBrush = elementDo.try_as<Media::ImageBrush>()) {
            if (property == Media::ImageBrush::ImageSourceProperty()) {
                if (auto retryImage =
                        TrackIfRemoteImageSource(imageBrush, property, value)) {
                    value = retryImage;
                }
            }
        } else if (auto image = elementDo.try_as<Controls::Image>()) {
            if (property == Controls::Image::SourceProperty()) {
                if (auto retryImage =
                        TrackIfRemoteImageSource(image, property, value)) {
                    value = retryImage;
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

// Puts back the value the property had before the style. The target's image
// entry goes with the style's value: a retry would put that image back, and the
// restored image's load events would be taken for it.
void RestoreOriginalValue(
    DependencyObject elementDo,
    DependencyProperty property,
    winrt::Windows::Foundation::IInspectable const& originalValue) {
    UntrackImageTarget(elementDo, property);
    SetOrClearValue(elementDo, property, originalValue, /*initialApply=*/false,
                    /*trackImage=*/false);
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
            override.elementMatcher.type,
            fallbackClassName ? fallbackClassName
                              : winrt::name_of<FrameworkElement>(),
            &override.propertyOverrides);

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
// the DLL is unloaded. An image which a retry assigned is the mod's too.
void AdoptExternalValueAsOriginal(
    FrameworkElement element,
    DependencyProperty property,
    ElementPropertyCustomizationState* propertyCustomizationState) {
    if (!propertyCustomizationState->customValue) {
        return;
    }
    auto localValue = ReadLocalValueWithWorkaround(element, property);
    if (!SameLocalValue(localValue,
                        propertyCustomizationState->lastAppliedValue) &&
        !IsImageRetryValue(element, property, localValue)) {
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
        RestoreOriginalValue(element, property,
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

// Take an element's captures out of the registry, returning the names they were
// under. Other elements may still capture the same names, so a name only
// becomes undefined once its last capture is gone.
std::vector<std::wstring> WithdrawCapturesForElement(
    StyleVariableState* state,
    ElementId elementId,
    const ElementCustomizationState& elementState) {
    std::vector<std::wstring> varNames;

    for (const auto& [property, captureState] :
         elementState.captureCustomizationStates) {
        if (captureState.varName.empty()) {
            continue;
        }

        auto varIt = state->variables.find(captureState.varName);
        if (varIt == state->variables.end()) {
            continue;
        }

        size_t removed = std::erase_if(
            varIt->second, [elementId](const StyleVariableCapture& capture) {
                return capture.elementId == elementId;
            });
        if (!removed) {
            continue;
        }

        ReleaseStyleVariableElementRefs(state, elementId, removed);

        varNames.push_back(captureState.varName);
        if (varIt->second.empty()) {
            state->variables.erase(varIt);
        }
    }

    return varNames;
}

// Put back the captures WithdrawCapturesForElement took out, returning the
// names they're under. Values are read afresh: the capture callbacks drop
// changes for a capture that isn't in the registry.
std::vector<std::wstring> RepublishCapturesForElement(
    StyleVariableState* state,
    ElementId elementId,
    FrameworkElement element,
    const ElementCustomizationState& elementState) {
    std::vector<std::wstring> varNames;

    for (const auto& [property, captureState] :
         elementState.captureCustomizationStates) {
        if (captureState.varName.empty()) {
            continue;
        }

        auto& captures = state->variables[captureState.varName];
        if (std::any_of(captures.begin(), captures.end(),
                        [elementId](const StyleVariableCapture& capture) {
                            return capture.elementId == elementId;
                        })) {
            continue;
        }

        captures.push_back(
            {elementId, ReadCapturedStyleVariableValue(element, property)});
        AddStyleVariableElementRef(state, elementId);

        varNames.push_back(captureState.varName);
    }

    return varNames;
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
    // Before the properties are restored, so that a visual state change which
    // restoring one causes can't apply the style again.
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
                RestoreOriginalValue(element, property,
                                     *propState.originalValue);
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
//
// A boxed value is recorded by value, since a source can box it anew on every
// read. Anything else is recorded by a weak reference, so that a destroyed item
// can't be mistaken for a successor at the same address, which would leave an
// element wearing the styles matched for its predecessor.
using RepeaterItem =
    std::variant<winrt::weak_ref<wf::IInspectable>, UnboxedPropertyValue>;

thread_local std::unordered_map<ElementId, RepeaterItem> g_elementMatchedItems;

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

// The item a repeater realized an element for, nullopt when there is no such
// item or it is neither a boxed value nor supports a weak reference.
std::optional<RepeaterItem> RepeaterItemAt(muxc::ItemsRepeater const& repeater,
                                           int index) {
    try {
        auto itemsSourceView = repeater.ItemsSourceView();
        if (!itemsSourceView || index < 0 || index >= itemsSourceView.Count()) {
            return std::nullopt;
        }

        auto item = itemsSourceView.GetAt(index);
        if (auto value = TryUnboxPropertyValue(item)) {
            return RepeaterItem{std::move(*value)};
        }

        if (auto weakItem = TryMakeWeak(item)) {
            return RepeaterItem{std::move(weakItem)};
        }

        return std::nullopt;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        return std::nullopt;
    }
}

// Never true for a destroyed item.
bool SameRepeaterItem(RepeaterItem const& a, RepeaterItem const& b) {
    using WeakItem = winrt::weak_ref<wf::IInspectable>;
    if (auto* weakA = std::get_if<WeakItem>(&a)) {
        auto* weakB = std::get_if<WeakItem>(&b);
        auto strongA = weakA->get();
        return weakB && strongA && strongA == weakB->get();
    }

    auto* valueB = std::get_if<UnboxedPropertyValue>(&b);
    return valueB && std::get<UnboxedPropertyValue>(a) == *valueB;
}

// Whether the element is in an item element a repeater holds without having it
// realized, i.e. one sitting in its recycle pool.
bool IsInRecycledRepeaterItem(FrameworkElement element) {
    try {
        DependencyObject child = element;
        while (child) {
            auto parent = Media::VisualTreeHelper::GetParent(child);
            if (auto repeater = parent.try_as<muxc::ItemsRepeater>()) {
                if (repeater.GetElementIndex(child.as<UIElement>()) < 0) {
                    return true;
                }
            }

            child = std::move(parent);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    return false;
}

thread_local bool g_recycledItemCapturesRefreshQueued;
thread_local winrt::Windows::System::DispatcherQueueTimer
    g_recycledItemCapturesRefreshTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    g_recycledItemCapturesRefreshTimerTickRevoker;

// The captures of an element in a recycled repeater item are withdrawn, as if
// the item was removed: the recycle pool keeps the item in the tree, out of
// sight, and its captures would keep publishing the values they last saw. This
// runs once layout is done rather than on each clear and prepare, since a
// layout can clear an element and hand it straight back within a pass, and
// flipping the captures there would restyle their consumers on every pass.
void RefreshRecycledItemCaptures() {
    g_recycledItemCapturesRefreshQueued = false;

    auto* state = GetStyleVariableState();

    std::vector<ElementId> owners(state->withdrawnCaptureOwners.begin(),
                                  state->withdrawnCaptureOwners.end());
    for (const auto& [varName, captures] : state->variables) {
        for (const auto& capture : captures) {
            owners.push_back(capture.elementId);
        }
    }

    std::sort(owners.begin(), owners.end());
    owners.erase(std::unique(owners.begin(), owners.end()), owners.end());

    std::vector<std::wstring> changedVarNames;
    for (ElementId owner : owners) {
        auto it = g_elementsCustomizationState.find(owner);
        if (it == g_elementsCustomizationState.end()) {
            continue;
        }

        auto& elementState = it->second;
        auto element = elementState.element.get();
        if (!element) {
            continue;
        }

        bool withdrawn = state->withdrawnCaptureOwners.contains(owner);
        if (IsInRecycledRepeaterItem(element) == withdrawn) {
            continue;
        }

        std::vector<std::wstring> varNames;
        if (withdrawn) {
            Wh_Log(L"Republishing captures of %llu",
                   static_cast<uint64_t>(owner));
            varNames = RepublishCapturesForElement(state, owner, element,
                                                   elementState);
            state->withdrawnCaptureOwners.erase(owner);
        } else {
            Wh_Log(L"Withdrawing captures of %llu",
                   static_cast<uint64_t>(owner));
            varNames = WithdrawCapturesForElement(state, owner, elementState);
            state->withdrawnCaptureOwners.insert(owner);
        }

        changedVarNames.insert(changedVarNames.end(), varNames.begin(),
                               varNames.end());
    }

    std::sort(changedVarNames.begin(), changedVarNames.end());
    changedVarNames.erase(
        std::unique(changedVarNames.begin(), changedVarNames.end()),
        changedVarNames.end());

    for (const auto& varName : changedVarNames) {
        PropagateStyleVariableChange(state, varName, std::nullopt);
    }
}

// Armed from inside a layout pass, and the timer carries the refresh out of it.
void QueueRecycledItemCapturesRefresh() {
    if (g_recycledItemCapturesRefreshQueued) {
        return;
    }

    auto* state = GetStyleVariableState();
    if (state->variables.empty() && state->withdrawnCaptureOwners.empty()) {
        return;
    }

    try {
        if (!g_recycledItemCapturesRefreshTimer) {
            auto dispatcherQueue =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (!dispatcherQueue) {
                Wh_Log(L"No dispatcher queue, recycled item captures stay");
                return;
            }

            g_recycledItemCapturesRefreshTimer = dispatcherQueue.CreateTimer();
            g_recycledItemCapturesRefreshTimer.IsRepeating(false);
            g_recycledItemCapturesRefreshTimer.Interval(
                std::chrono::milliseconds{1});
            g_recycledItemCapturesRefreshTimerTickRevoker =
                g_recycledItemCapturesRefreshTimer.Tick(
                    winrt::auto_revoke,
                    [](winrt::Windows::System::DispatcherQueueTimer const&,
                       winrt::Windows::Foundation::IInspectable const&) {
                        RefreshRecycledItemCaptures();
                    });
        }

        g_recycledItemCapturesRefreshTimer.Start();
        g_recycledItemCapturesRefreshQueued = true;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

void StopRecycledItemCapturesRefresh() {
    if (g_recycledItemCapturesRefreshTimer) {
        try {
            g_recycledItemCapturesRefreshTimer.Stop();
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    g_recycledItemCapturesRefreshTimerTickRevoker.revoke();
    g_recycledItemCapturesRefreshTimer = nullptr;
    g_recycledItemCapturesRefreshQueued = false;
}

// A virtualizing container recycles its item elements instead of destroying
// them: the recycle pool keeps the element parented, out of sight, so XAML
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
            // Ahead of the early returns: the item's descendants can hold
            // captures even when the item itself has no id.
            QueueRecycledItemCapturesRefresh();

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
            QueueRecycledItemCapturesRefresh();

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
            // reference can get back, such as one a source wraps anew on every
            // read, is never recorded: an entry which could match nothing would
            // keep the element held for good.
            wf::IInspectable strongItem;
            if (item) {
                if (auto* weakItem =
                        std::get_if<winrt::weak_ref<wf::IInspectable>>(
                            &*item)) {
                    strongItem = weakItem->get();
                    if (!strongItem) {
                        item.reset();
                    }
                }
            }

            if (!g_recycledElements.erase(elementId)) {
                // Freshly created, so the styles its Add mutation applied are
                // the ones for this item, and only the item is recorded.
                if (item) {
                    g_elementMatchedItems[elementId] = std::move(*item);
                }
                return;
            }

            if (item) {
                auto it = g_elementMatchedItems.find(elementId);
                if (it != g_elementMatchedItems.end() &&
                    SameRepeaterItem(it->second, *item)) {
                    Wh_Log(L"Element reused for the same item: %llu",
                           static_cast<uint64_t>(elementId));
                    return;
                }
            }

            Wh_Log(L"Element reused: %llu", static_cast<uint64_t>(elementId));

            ReapplyCustomizationsForSubtree(element);

            // After the walk, which erases the entry as part of the teardown.
            if (item) {
                g_elementMatchedItems[elementId] = std::move(*item);
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

    // The element may be in an item already sitting in a recycle pool, which
    // no clear will be reported for.
    if (!resolved.captures.empty()) {
        QueueRecycledItemCapturesRefresh();
    }

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

// With XAML diagnostics enabled, which the mod relies on to observe the visual
// tree, CFrameworkElement::UpdateImplicitStyle drops an element's implicit
// style when all implicit styles are invalidated, instead of looking it up
// again. Changing the app's resources does that, e.g. opening Personalization >
// Background or Bluetooth & devices > Printers & scanners, and leaves controls
// with their default styles, such as the navigation pane without its backdrop.
// The diagnostics check is patched to take the path it takes without
// diagnostics. With no symbols to go by, it's found by the code around it.
//
// The patched code behaves as the original does with diagnostics disabled and
// refers to nothing in the mod, so it's left in place when the mod unloads.
bool g_implicitStyleDiagnosticsCheckPatchAttempted;

struct CodeRange {
    const BYTE* begin;
    const BYTE* end;
};

// The executable sections of a module, or for an ARM64X module, its ARM64 code
// without the ARM64EC code it also holds.
std::vector<CodeRange> GetNativeCodeRanges(HMODULE module) {
    auto* base = reinterpret_cast<const BYTE*>(module);
    auto* ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        base + reinterpret_cast<const IMAGE_DOS_HEADER*>(base)->e_lfanew);

    std::vector<CodeRange> ranges;

#ifdef _M_ARM64
    // IMAGE_ARM64EC_METADATA and IMAGE_CHPE_RANGE_ENTRY, which the headers
    // don't define. The low bits of a range's start hold its code type, ARM64
    // being 0.
    struct Arm64EcMetadata {
        ULONG Version;
        ULONG CodeMap;
        ULONG CodeMapCount;
    };
    struct ChpeRangeEntry {
        ULONG StartOffset;
        ULONG Length;
    };

    const auto& loadConfigDirectory =
        ntHeaders->OptionalHeader
            .DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];
    auto* loadConfig = reinterpret_cast<const IMAGE_LOAD_CONFIG_DIRECTORY*>(
        base + loadConfigDirectory.VirtualAddress);
    if (loadConfigDirectory.VirtualAddress &&
        loadConfig->Size >=
            offsetof(IMAGE_LOAD_CONFIG_DIRECTORY, CHPEMetadataPointer) +
                sizeof(loadConfig->CHPEMetadataPointer) &&
        loadConfig->CHPEMetadataPointer) {
        auto* metadata = reinterpret_cast<const Arm64EcMetadata*>(
            base + (loadConfig->CHPEMetadataPointer -
                    ntHeaders->OptionalHeader.ImageBase));
        auto* codeMap =
            reinterpret_cast<const ChpeRangeEntry*>(base + metadata->CodeMap);
        for (ULONG i = 0; i < metadata->CodeMapCount; i++) {
            if ((codeMap[i].StartOffset & 3) == 0) {
                const BYTE* begin = base + codeMap[i].StartOffset;
                ranges.push_back({begin, begin + codeMap[i].Length});
            }
        }

        return ranges;
    }
#endif

    auto* section = IMAGE_FIRST_SECTION(ntHeaders);
    for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections;
         i++, section++) {
        if (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
            const BYTE* begin = base + section->VirtualAddress;
            ranges.push_back({begin, begin + section->Misc.VirtualSize});
        }
    }

    return ranges;
}

#if defined(_M_X64)
// The opcode of `test al, al`, which `xor al, al` replaces.
using CheckCode = BYTE;
constexpr CheckCode kPatchedCheck = 0x30;

// xor r8d, r8d
// lea edx, [r8+5]  ; RuntimeEnabledFeature::XamlDiagnostics
// call IsFeatureEnabled
// test al, al
// je <lookup>
//
// Directly after the comparison of the old and new styles, which branches to
// the same place:
// jne <lookup>
// mov rcx, [rbp+disp8] or mov rcx, [rsp+disp8]
void FindImplicitStyleDiagnosticsChecks(CodeRange range,
                                        std::vector<CheckCode*>& found) {
    constexpr BYTE kFeatureCheck[] = {0x45, 0x33, 0xC0, 0x41,
                                      0x8D, 0x50, 0x05, 0xE8};
    for (const BYTE* p = range.begin + 11; p + 20 <= range.end; p++) {
        if (memcmp(p, kFeatureCheck, sizeof(kFeatureCheck)) != 0 ||
            (p[12] != 0x84 && p[12] != kPatchedCheck) || p[13] != 0xC0 ||
            p[14] != 0x0F || p[15] != 0x84) {
            continue;
        }

        const BYTE* lookup = p + 20 + *reinterpret_cast<const INT32*>(p + 16);

        const BYTE* mov;
        if (p[-4] == 0x48 && p[-3] == 0x8B && p[-2] == 0x4D) {
            mov = p - 4;
        } else if (p[-5] == 0x48 && p[-4] == 0x8B && p[-3] == 0x4C &&
                   p[-2] == 0x24) {
            mov = p - 5;
        } else {
            continue;
        }

        if ((mov[-2] == 0x75 && mov + static_cast<INT8>(mov[-1]) == lookup) ||
            (mov[-6] == 0x0F && mov[-5] == 0x85 &&
             mov + *reinterpret_cast<const INT32*>(mov - 4) == lookup)) {
            found.push_back(const_cast<BYTE*>(p + 12));
        }
    }
}
#elif defined(_M_ARM64)
// The call of IsFeatureEnabled, which `mov w0, #0` replaces.
using CheckCode = DWORD;
constexpr CheckCode kPatchedCheck = 0x52800000;

// mov w2, #0
// mov w1, #5  ; RuntimeEnabledFeature::XamlDiagnostics
// bl IsFeatureEnabled
// uxtb w8, w0 + cbz w8, <lookup> or tbz x0, #0, <lookup>
// mov wN, #1  ; possibly after a branch
// ...
// <lookup>:
// mov wN, #0
//
// x0 is set before the call too, and the order of the moves varies.
void FindImplicitStyleDiagnosticsChecks(CodeRange range,
                                        std::vector<CheckCode*>& found) {
    auto* begin = reinterpret_cast<const DWORD*>(range.begin);
    auto* end = reinterpret_cast<const DWORD*>(range.end);

    auto branchTarget = [](const DWORD* p, int shift, int bits) {
        INT32 offset =
            static_cast<INT32>((*p >> shift) << (32 - bits)) >> (32 - bits);
        return p + offset;
    };

    for (const DWORD* p = begin + 3; p + 3 <= end; p++) {
        if ((*p >> 26 != 0x25 && *p != kPatchedCheck) ||
            std::find(p - 3, p, 0x52800002) == p ||
            std::find(p - 3, p, 0x528000A1) == p) {
            continue;
        }

        const DWORD* branch = p + 1;
        const DWORD* lookup;
        if (branch[0] == 0x53001C08 && (branch[1] & 0xFF00001F) == 0x34000008) {
            branch++;
            lookup = branchTarget(branch, 5, 19);
        } else if ((branch[0] & 0xFFF8001F) == 0x36000000) {
            lookup = branchTarget(branch, 5, 14);
        } else {
            continue;
        }

        const DWORD* set = branch + 1;
        if (set < end && *set >> 26 == 0x05) {
            set = branchTarget(set, 0, 26);
        }

        if (lookup >= begin && lookup < end && set >= begin && set < end &&
            (*lookup & 0xFFFFFFE0) == 0x52800000 && *set == (*lookup | 0x20)) {
            found.push_back(const_cast<DWORD*>(p));
        }
    }
}
#endif

void PatchImplicitStyleDiagnosticsCheck() {
    if (g_implicitStyleDiagnosticsCheckPatchAttempted) {
        return;
    }

    g_implicitStyleDiagnosticsCheckPatchAttempted = true;

    HMODULE module = GetModuleHandle(L"Windows.UI.Xaml.dll");
    if (!module) {
        Wh_Log(L"Windows.UI.Xaml.dll isn't loaded");
        return;
    }

    std::vector<CheckCode*> found;
    for (const auto& range : GetNativeCodeRanges(module)) {
        FindImplicitStyleDiagnosticsChecks(range, found);
    }

    if (found.size() != 1) {
        Wh_Log(L"Found %zu implicit style diagnostics checks", found.size());
        return;
    }

    CheckCode* check = found[0];
    if (*check == kPatchedCheck) {
        Wh_Log(L"Implicit style diagnostics check is already patched");
        return;
    }

    DWORD oldProtect;
    if (!VirtualProtect(check, sizeof(*check), PAGE_EXECUTE_READWRITE,
                        &oldProtect)) {
        Wh_Log(L"VirtualProtect failed: %u", GetLastError());
        return;
    }

    // A single store, since other UI threads may be running the code.
    *reinterpret_cast<volatile CheckCode*>(check) = kPatchedCheck;

    VirtualProtect(check, sizeof(*check), oldProtect, &oldProtect);
    FlushInstructionCache(GetCurrentProcess(), check, sizeof(*check));

    Wh_Log(L"Patched the implicit style diagnostics check at %p", check);
}

// If the diagnostics check above can't be patched, the dropped implicit style
// also replaces the template of a control styled that way. The Settings app
// hits this with the SplitView inside its NavigationView, e.g. when opening
// Personalization > Background. The SplitView keeps its old `PaneRoot` and
// `ContentRoot` until the new template is applied, and their Borders still own
// `SplitView.Pane` and `SplitView.Content`, which the new template binds
// `Border.Child` to. Taking them over fails the association check with
// E_INVALIDARG, and XAML turns that into an unhandled error which terminates
// the app. Hand the children back as soon as the template changes, before the
// replacement is built.
//
// By then the old content has left the tree, where neither
// VisualTreeHelper::GetParent nor FrameworkElement.Parent reports an owner for
// the children anymore, so the Borders holding them are recorded as they're
// added.
struct TrackedSplitView {
    winrt::weak_ref<Controls::SplitView> splitView;
    int64_t templateChangedToken = 0;
    std::vector<winrt::weak_ref<Controls::Border>> childHosts;
};

std::vector<TrackedSplitView> g_trackedSplitViews;

bool IsSplitViewChild(Controls::SplitView const& splitView,
                      UIElement const& element) {
    return element &&
           (element == splitView.Pane() || element == splitView.Content());
}

void AddSplitViewChildHost(TrackedSplitView& tracked,
                           Controls::Border const& border) {
    std::erase_if(tracked.childHosts,
                  [](const auto& host) { return !host.get(); });

    if (std::any_of(
            tracked.childHosts.begin(), tracked.childHosts.end(),
            [&border](const auto& host) { return host.get() == border; })) {
        return;
    }

    tracked.childHosts.push_back(winrt::make_weak(border));
}

// For a SplitView whose template was applied before it was reported. The
// children themselves aren't descended into, since they can be whole pages.
void FindSplitViewChildHosts(TrackedSplitView& tracked,
                             Controls::SplitView const& splitView,
                             DependencyObject const& element) {
    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i);
        if (auto border = child.try_as<Controls::Border>();
            border && IsSplitViewChild(splitView, border.Child())) {
            AddSplitViewChildHost(tracked, border);
        } else if (!IsSplitViewChild(splitView, child.try_as<UIElement>())) {
            FindSplitViewChildHosts(tracked, splitView, child);
        }
    }
}

void TrackSplitViewChildHost(FrameworkElement element) {
    auto border = element.try_as<Controls::Border>();
    if (!border) {
        return;
    }

    auto child = border.Child();
    for (auto& tracked : g_trackedSplitViews) {
        auto splitView = tracked.splitView.get();
        if (splitView && IsSplitViewChild(splitView, child)) {
            AddSplitViewChildHost(tracked, border);
            return;
        }
    }
}

void ReleaseSplitViewChildren(Controls::SplitView const& splitView) {
    auto it =
        std::find_if(g_trackedSplitViews.begin(), g_trackedSplitViews.end(),
                     [&splitView](const TrackedSplitView& tracked) {
                         return tracked.splitView.get() == splitView;
                     });
    if (it == g_trackedSplitViews.end()) {
        return;
    }

    // A copy: detaching reports removals, which re-enter the mod.
    auto childHosts = it->childHosts;
    for (const auto& host : childHosts) {
        auto border = host.get();
        if (!border) {
            continue;
        }

        auto child = border.Child();
        if (!IsSplitViewChild(splitView, child)) {
            continue;
        }

        Wh_Log(L"Detaching %s from the replaced SplitView template",
               winrt::get_class_name(child).c_str());
        border.Child(nullptr);
    }
}

void TrackSplitView(FrameworkElement element) {
    auto splitView = element.try_as<Controls::SplitView>();
    if (!splitView) {
        return;
    }

    std::erase_if(g_trackedSplitViews, [](const TrackedSplitView& tracked) {
        return !tracked.splitView.get();
    });

    // An element is reported again every time it re-enters the tree.
    if (std::any_of(g_trackedSplitViews.begin(), g_trackedSplitViews.end(),
                    [&splitView](const TrackedSplitView& tracked) {
                        return tracked.splitView.get() == splitView;
                    })) {
        return;
    }

    auto token = splitView.RegisterPropertyChangedCallback(
        Controls::Control::TemplateProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            // Runs from within XAML's own property change, which an exception
            // escaping into would turn into a crash.
            try {
                if (auto splitView = sender.try_as<Controls::SplitView>()) {
                    ReleaseSplitViewChildren(splitView);
                }
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult());
            }
        });

    g_trackedSplitViews.push_back({winrt::make_weak(splitView), token});

    FindSplitViewChildHosts(g_trackedSplitViews.back(), splitView, splitView);
}

void UntrackSplitViews() {
    for (const auto& tracked : g_trackedSplitViews) {
        auto splitView = tracked.splitView.get();
        if (!splitView) {
            continue;
        }

        try {
            splitView.UnregisterPropertyChangedCallback(
                Controls::Control::TemplateProperty(),
                tracked.templateChangedToken);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    g_trackedSplitViews.clear();
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
thread_local winrt::Windows::System::DispatcherQueueTimer
    g_diagnosticsReleaseDrainTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    g_diagnosticsReleaseDrainTimerTickRevoker;

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
// match the Settings app match almost nothing of such a tree, so what this
// retains is small, but a rule written against a bare type would retain much
// more.
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

    auto visualTreeWatcher = GetVisualTreeWatcher();
    if (!visualTreeWatcher) {
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

        if (visualTreeWatcher->ReleaseDiagnosticsReference(handle)) {
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
            auto dispatcherQueue =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
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
                    [](winrt::Windows::System::DispatcherQueueTimer const&,
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
    ResetThreadLocal(g_pendingDiagnosticsRelease);

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

    // After RestoreCapturesForElement so the just-unregistered capture
    // callbacks can't re-seed a variable mid-teardown.
    std::vector<std::wstring> removedVarNames;
    if (state) {
        removedVarNames = WithdrawCapturesForElement(state, elementId,
                                                     elementCustomizationState);
        state->withdrawnCaptureOwners.erase(elementId);
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
            return L"Windows.UI.Xaml.Shapes.Rectangle";
        }

        return L"Windows.UI.Xaml.Controls." + std::wstring{type};
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
    } else if (wcscmp(themeName, L"ClassicSearchBar") == 0) {
        theme = &g_themeClassicSearchBar;
    } else if (wcscmp(themeName, L"StoreFrame11") == 0) {
        theme = &g_themeStoreFrame11;
    } else if (wcscmp(themeName, L"Blue") == 0) {
        theme = &g_themeBlue;
    } else if (wcscmp(themeName, L"Translucent_Settings11") == 0) {
        theme = &g_themeTranslucent_Settings11;
    } else if (wcscmp(themeName, L"WindowGlass") == 0) {
        theme = &g_themeWindowGlass;
    } else if (wcscmp(themeName, L"OLED_variant_ModrinthGreen") == 0) {
        theme = &g_themeOLED_variant_ModrinthGreen;
    } else if (wcscmp(themeName, L"OLED_variant_SystemAscent") == 0) {
        theme = &g_themeOLED_variant_SystemAscent;
    } else if (wcscmp(themeName, L"Translucent_Seamless") == 0) {
        theme = &g_themeTranslucent_Seamless;
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
    ResetThreadLocal(g_imageCacheUriRemotes);
    StopImageLoadRetriesForCurrentThread();

    // Before the teardown below, so that no recycling callback can fire into
    // half-cleared state, and no pending release can consult state which is
    // being cleared.
    StopDiagnosticsReleases();
    StopRecycledItemCapturesRefresh();
    ResetThreadLocal(g_virtualizingRepeaters);
    ResetThreadLocal(g_recycledElements);
    ResetThreadLocal(g_elementMatchedItems);

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
    ResetThreadLocal(g_elementIds);
    g_elementIdsReapThreshold = 64;

    g_elementsCustomizationRules.clear();
    UntrackSplitViews();

    UninitializeResourceVariables();

    g_noiseStream = nullptr;

    g_targetThreadId = 0;
}

void InitializeSettingsAndTap() {
    DWORD kNoThreadId = 0;
    if (!g_targetThreadId.compare_exchange_strong(kNoThreadId,
                                                  GetCurrentThreadId())) {
        return;
    }

    ProcessAllStylesFromSettings();

    PatchImplicitStyleDiagnosticsCheck();

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

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExW_Original;
LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey,
                                  LPCWSTR lpSubKey,
                                  DWORD ulOptions,
                                  REGSAM samDesired,
                                  PHKEY phkResult) {
    LSTATUS result = RegOpenKeyExW_Original(hKey, lpSubKey, ulOptions,
                                            samDesired, phkResult);
    if (result == ERROR_SUCCESS || !g_reportCompositionDiagAsDisabled ||
        hKey != HKEY_LOCAL_MACHINE || !lpSubKey ||
        _wcsicmp(lpSubKey, L"Software\\Microsoft\\XAML\\Debug") != 0) {
        return result;
    }

    // The key usually doesn't exist, and the value isn't queried unless the key
    // could be opened, so hand out a key which does exist.
    Wh_Log(L"Substituting the XAML debug key");
    return RegOpenKeyExW_Original(HKEY_LOCAL_MACHINE, L"Software\\Microsoft",
                                  ulOptions, samDesired, phkResult);
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey,
                                     LPCWSTR lpValueName,
                                     LPDWORD lpReserved,
                                     LPDWORD lpType,
                                     LPBYTE lpData,
                                     LPDWORD lpcbData) {
    if (!g_reportCompositionDiagAsDisabled || !lpValueName ||
        _wcsicmp(lpValueName, L"DisableCompositionDiag") != 0) {
        return RegQueryValueExW_Original(hKey, lpValueName, lpReserved, lpType,
                                         lpData, lpcbData);
    }

    Wh_Log(L"Reporting DisableCompositionDiag as set");

    if (lpType) {
        *lpType = REG_DWORD;
    }

    if (lpData && (!lpcbData || *lpcbData < sizeof(DWORD))) {
        if (lpcbData) {
            *lpcbData = sizeof(DWORD);
        }
        return ERROR_MORE_DATA;
    }

    if (lpData) {
        *reinterpret_cast<DWORD*>(lpData) = 1;
    }

    if (lpcbData) {
        *lpcbData = sizeof(DWORD);
    }

    return ERROR_SUCCESS;
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

            auto isCurrentProcessWnd = [](HWND hWnd) {
                DWORD dwProcessId = 0;
                return GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                       dwProcessId == GetCurrentProcessId();
            };

            // The CoreWindow is top-level until the frame window adopts it,
            // and while the app is minimized.
            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                if (isCurrentProcessWnd(hWnd)) {
                    *param.hWnd = hWnd;
                    return FALSE;
                }

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
                if (isCurrentProcessWnd(hCoreWnd)) {
                    *param.hWnd = hCoreWnd;
                    return FALSE;
                }
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

PTP_TIMER g_statsTimer;

bool StartStatsTimer() {
    static constexpr WCHAR kStatsBaseUrl[] =
        L"https://github.com/ramensoftware/"
        L"windows-11-settings-styling-guide/"
        L"releases/download/stats-v2/";

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
    constexpr ULONGLONG k10Seconds = 10 * 10000000LL;

    ULONGLONG minDueTime = currentTime + k10Seconds;
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
    constexpr DWORD k1SecondInMs = 1000;

    FILETIME dueTimeFt;
    dueTimeFt.dwLowDateTime = (DWORD)(dueTime & 0xFFFFFFFF);
    dueTimeFt.dwHighDateTime = (DWORD)(dueTime >> 32);
    SetThreadpoolTimer(g_statsTimer, &dueTimeFt, k24HoursInMs, k1SecondInMs);
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

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    void* pKernelBaseRegOpenKeyExW =
        (void*)GetProcAddress(kernelBaseModule, "RegOpenKeyExW");
    Wh_SetFunctionHook(pKernelBaseRegOpenKeyExW, (void*)RegOpenKeyExW_Hook,
                       (void**)&RegOpenKeyExW_Original);

    void* pKernelBaseRegQueryValueExW =
        (void*)GetProcAddress(kernelBaseModule, "RegQueryValueExW");
    Wh_SetFunctionHook(pKernelBaseRegQueryValueExW,
                       (void*)RegQueryValueExW_Hook,
                       (void**)&RegQueryValueExW_Original);

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

    StopImageDownloads();

    // Before the UI threads are uninitialized, so that a retry can't be
    // scheduled on a thread which is being uninitialized.
    StopImageLoadRetries();

    if (auto watcher = ExchangeVisualTreeWatcher(nullptr)) {
        watcher->UnadviseVisualTreeChange();
    }

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Uninitializing - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { UninitializeSettingsAndTap(); }, nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (auto watcher = ExchangeVisualTreeWatcher(nullptr)) {
        watcher->UnadviseVisualTreeChange();
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
