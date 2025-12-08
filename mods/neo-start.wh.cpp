// ==WindhawkMod==
// @id              neo-start
// @name            NeoStart
// @description     NeoStart is a modern and minimalistic start menu for Windows
// @version         1.0.3
// @author          GDKAYKY
// @github          https://github.com/GDKAYKY
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         SearchApp.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The MIT License
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/GDKAYKY/NeoStart/issues
//
// For pull requests, development takes place here:
// https://github.com/GDKAYKY/NeoStart

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*...*/
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
  std::vector<ThemeTargetStyles> webViewTargetStyles;
  std::vector<PCWSTR> styleConstants;
};

// clang-format off

const Theme g_themeNeoStart = {{
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
    ThemeTargetStyles{L"Grid#RootGrid@SearchBoxLocationStates ", {
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

const Theme g_themeNeoStart_variant_ClassicStartMenu = {{
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
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

enum class Target {
  StartMenu,
  SearchHost,
};

Target g_target;

bool g_isRedesignedStartMenu;

// https://stackoverflow.com/a/51274008
template <auto fn> struct deleter_from_fn {
  template <typename T> constexpr void operator()(T *arg) const { fn(arg); }
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
  std::wstring tintThemeResourceKey; // Empty if not from ThemeResource
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

bool g_disableNewStartMenuLayout;

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

winrt::Windows::Foundation::IInspectable
ReadLocalValueWithWorkaround(DependencyObject elementDo,
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

class XamlBlurBrush : public wux::Media::XamlCompositionBrushBaseT<XamlBlurBrush>
{
public:
	XamlBlurBrush(wuc::Compositor compositor,
	              float blurAmount,
	              winrt::Windows::UI::Color tint,
	              std::optional<uint8_t> tintOpacity,
	              winrt::hstring tintThemeResourceKey);

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

  auto &failedImageBrushes = g_failedImageBrushesForThread.failedImageBrushes;

  // Retry loading all failed images by re-setting the ImageSource property.
  for (auto &info : failedImageBrushes) {
    if (auto brush = info.brush.get()) {
      try {
        Wh_Log(L"Retrying image load for: %s", info.imageSource.c_str());
        // Clear the ImageSource first to force a reload.
        brush.ImageSource(nullptr);
        // Then create a new BitmapImage and set it.
        Media::Imaging::BitmapImage bitmapImage;
        bitmapImage.UriSource(
            winrt::Windows::Foundation::Uri(info.imageSource));
        brush.ImageSource(bitmapImage);
      } catch (winrt::hresult_error const &ex) {
        Wh_Log(L"Error retrying image load %08X: %s", ex.code(),
               ex.message().c_str());
      }
    }
  }

  // Clean up any weak refs that are no longer valid.
  std::erase_if(failedImageBrushes,
                [](const auto &info) { return !info.brush.get(); });
}

void OnNetworkStatusChanged(
    winrt::Windows::Foundation::IInspectable const &sender) {
  Wh_Log(L"Network status changed, dispatching retry to all UI threads");

  // Get snapshot of dispatchers under lock.
  std::vector<winrt::Windows::System::DispatcherQueue> dispatchers;
  {
    std::lock_guard<std::mutex> lock(g_failedImageBrushesRegistryMutex);

    for (auto &weakDispatcher : g_failedImageBrushesRegistry) {
      if (auto dispatcher = weakDispatcher.get()) {
        dispatchers.push_back(dispatcher);
      }
    }

    // Clean up dead weak refs.
    std::erase_if(g_failedImageBrushesRegistry, [](const auto &weakDispatcher) {
      return !weakDispatcher.get();
    });
  }

  // Dispatch retry to each UI thread.
  for (auto &dispatcher : dispatchers) {
    try {
      dispatcher.TryEnqueue([]() { RetryFailedImageLoadsOnCurrentThread(); });
    } catch (winrt::hresult_error const &ex) {
      Wh_Log(L"Error dispatching retry to UI thread %08X: %s", ex.code(),
             ex.message().c_str());
    }
  }
}

void RemoveFromFailedImageBrushes(Media::ImageBrush const &brush) {
  auto &failedImageBrushes = g_failedImageBrushesForThread.failedImageBrushes;

  std::erase_if(failedImageBrushes, [&brush](const auto &info) {
    if (auto existingBrush = info.brush.get()) {
      return existingBrush == brush;
    }
    return false;
  });
}

void SetupImageBrushTracking(Media::ImageBrush const &brush,
                             winrt::hstring const &imageSourceUrl) {
  // First remove any existing entry for this brush to avoid duplicates.
  RemoveFromFailedImageBrushes(brush);

  // Add new entry with event handlers.
  ImageBrushFailedLoadInfo info;
  info.brush = winrt::make_weak(brush);
  info.imageSource = imageSourceUrl;

  // Set up ImageFailed event handler - add to list only when load fails.
  info.imageFailedRevoker = brush.ImageFailed(
      winrt::auto_revoke,
      [brushWeak = winrt::make_weak(brush),
       imageSourceUrl](winrt::Windows::Foundation::IInspectable const &sender,
                       ExceptionRoutedEventArgs const &e) {
        Wh_Log(L"ImageBrush load failed for: %s, error: %s",
               imageSourceUrl.c_str(), e.ErrorMessage().c_str());
        // The brush should already be in the list, no action needed here as
        // we add it preemptively in SetupImageBrushTracking.
      });

  // Set up ImageOpened event handler - remove from list when load succeeds.
  info.imageOpenedRevoker = brush.ImageOpened(
      winrt::auto_revoke,
      [brushWeak = winrt::make_weak(brush)](
          winrt::Windows::Foundation::IInspectable const &sender,
          RoutedEventArgs const &e) {
        Wh_Log(L"ImageBrush loaded successfully, removing from retry list");

        if (auto brush = brushWeak.get()) {
          RemoveFromFailedImageBrushes(brush);
        }
      });

  // Add to the list preemptively - will be removed if load succeeds.
  auto &failedImageBrushes = g_failedImageBrushesForThread.failedImageBrushes;
  failedImageBrushes.push_back(std::move(info));

  // Ensure we have a dispatcher for this thread.
  if (!g_failedImageBrushesForThread.dispatcher) {
    try {
      g_failedImageBrushesForThread.dispatcher =
          winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
      if (g_failedImageBrushesForThread.dispatcher) {
        // Register this thread's dispatcher globally.
        std::lock_guard<std::mutex> lock(g_failedImageBrushesRegistryMutex);
        g_failedImageBrushesRegistry.push_back(
            winrt::make_weak(g_failedImageBrushesForThread.dispatcher));
        Wh_Log(L"Registered UI thread dispatcher for network retry");
      }
    } catch (winrt::hresult_error const &ex) {
      Wh_Log(L"Error getting dispatcher for current thread %08X: %s", ex.code(),
             ex.message().c_str());
    }
  }

  // Register global network status changed handler if not already registered.
  // This is a one-time global registration.
  [[maybe_unused]] static bool networkHandlerRegistered = []() {
    try {
      g_networkStatusChangedToken = winrt::Windows::Networking::Connectivity::
          NetworkInformation::NetworkStatusChanged(OnNetworkStatusChanged);
      Wh_Log(L"Registered global network status change handler");
    } catch (winrt::hresult_error const &ex) {
      Wh_Log(L"Error registering network status handler %08X: %s", ex.code(),
             ex.message().c_str());
    }
    return true;
  }();
}

void SetOrClearValue(DependencyObject elementDo, DependencyProperty property,
                     const PropertyOverrideValue &overrideValue,
                     bool initialApply = false) {
  winrt::Windows::Foundation::IInspectable value;
  if (auto *inspectable = std::get_if<winrt::Windows::Foundation::IInspectable>(
          &overrideValue)) {
    value = *inspectable;
  } else if (auto *blurBrushParams =
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
      g_delayedAllAppsRootVisibilitySet = elementDo.Dispatcher().TryRunAsync(
          winrt::Windows::UI::Core::CoreDispatcherPriority::High,
          [elementDo = std::move(elementDo), property = std::move(property),
           value = std::move(value)]() {
            Wh_Log(L"Running delayed SetValue for AllAppsRoot");
            try {
              elementDo.SetValue(property, value);
            } catch (winrt::hresult_error const &ex) {
              Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }
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
    elementDo.ClearValue(property);
    return;
  }

  // Track ImageBrush with remote ImageSource for retry on network
  // reconnection. This handles cases where an ImageBrush is set as a property
  // value (e.g., Background).
  if (auto imageBrush = value.try_as<Media::ImageBrush>()) {
    auto imageSource = imageBrush.ImageSource();
    if (auto bitmapImage = imageSource.try_as<Media::Imaging::BitmapImage>()) {
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
      if (auto bitmapImage = value.try_as<Media::Imaging::BitmapImage>()) {
        auto uriSource = bitmapImage.UriSource();
        if (uriSource) {
          winrt::hstring uriString = uriSource.ToString();
          if (uriString.starts_with(L"https://") ||
              uriString.starts_with(L"http://")) {
            Wh_Log(L"Tracking ImageBrush ImageSource property with "
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
  } catch (winrt::hresult_error const &ex) {
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

  while ((pos_end = s.find(delimiter, pos_start)) != std::wstring_view::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

std::optional<PropertyOverrideValue>
ParseNonXamlPropertyOverrideValue(std::wstring_view stringValue) {
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

    Wh_Log(L"  %.*s", static_cast<int>(propSubstr.length()), propSubstr.data());

    if (pendingTintColorThemeResource) {
      if (!propSubstr.ends_with(kTintColorThemeResourceSuffix)) {
        throw std::runtime_error(
            "WindhawkBlur: Invalid TintColor theme resource syntax");
      }

      pendingTintColorThemeResource = false;

      tintThemeResourceKey = propSubstr.substr(
          0, propSubstr.size() - std::size(kTintColorThemeResourceSuffix));

      continue;
    }

    if (propSubstr == kTintColorThemeResourcePrefix) {
      pendingTintColorThemeResource = true;
      continue;
    }

    if (propSubstr.starts_with(kTintColorPrefix) &&
        propSubstr.back() == L'\"') {
      auto valStr = propSubstr.substr(std::size(kTintColorPrefix),
                                      propSubstr.size() -
                                          std::size(kTintColorPrefix) - 1);

      bool hasAlpha;
      switch (valStr.size()) {
      case 6:
        hasAlpha = false;
        break;
      case 8:
        hasAlpha = true;
        break;
      default:
        throw std::runtime_error("WindhawkBlur: Unsupported TintColor value");
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
      auto valStr = propSubstr.substr(std::size(kTintOpacityPrefix),
                                      propSubstr.size() -
                                          std::size(kTintOpacityPrefix) - 1);
      tintOpacity = std::stof(std::wstring(valStr));
      continue;
    }

    if (propSubstr.starts_with(kBlurAmountPrefix) &&
        propSubstr.back() == L'\"') {
      auto valStr = propSubstr.substr(std::size(kBlurAmountPrefix),
                                      propSubstr.size() -
                                          std::size(kBlurAmountPrefix) - 1);
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
  std::wstring xaml = LR"(<ResourceDictionary
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
    xaml += L"\">\n"
            L"    <Style TargetType=\"windhawkstyler:";
    xaml += EscapeXmlAttribute(typeName);
    xaml += L"\">\n";
  } else {
    xaml += L">\n"
            L"    <Style TargetType=\"";
    xaml += EscapeXmlAttribute(type);
    xaml += L"\">\n";
  }

  xaml += xamlStyleSetters;

  xaml += L"    </Style>\n"
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
    const std::wstring_view type, const std::wstring_view fallbackType,
    const std::wstring_view xamlStyleSetters) {
  try {
    return GetStyleFromXamlSetters(type, xamlStyleSetters);
  } catch (winrt::hresult_error const &ex) {
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
    Wh_Log(L"Retrying with fallback type type due to error %08X: %s", ex.code(),
           ex.message().c_str());
    return GetStyleFromXamlSetters(fallbackType, xamlStyleSetters);
  }
}

const PropertyOverrides &GetResolvedPropertyOverrides(
    const std::wstring_view type, const std::wstring_view fallbackType,
    PropertyOverridesMaybeUnresolved *propertyOverridesMaybeUnresolved) {
  if (const auto *resolved =
          std::get_if<PropertyOverrides>(propertyOverridesMaybeUnresolved)) {
    return *resolved;
  }

  PropertyOverrides propertyOverrides;

  try {
    const auto &styleRules = std::get<PropertyOverridesUnresolved>(
        *propertyOverridesMaybeUnresolved);
    if (!styleRules.empty()) {
      std::wstring xaml;

      std::vector<std::optional<PropertyOverrideValue>> propertyOverrideValues;
      propertyOverrideValues.reserve(styleRules.size());

      for (const auto &rule : styleRules) {
        propertyOverrideValues.push_back(
            rule.isXamlValue ? ParseNonXamlPropertyOverrideValue(rule.value)
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
          xaml += L">\n"
                  L"            <Setter.Value>\n";
          xaml += rule.value;
          xaml += L"\n"
                  L"            </Setter.Value>\n"
                  L"        </Setter>\n";
        }
      }

      auto style =
          GetStyleFromXamlSettersWithFallbackType(type, fallbackType, xaml);

      uint32_t i = 0;
      for (const auto &rule : styleRules) {
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
  } catch (winrt::hresult_error const &ex) {
    Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
  } catch (std::exception const &ex) {
    Wh_Log(L"Error: %S", ex.what());
  }

  *propertyOverridesMaybeUnresolved = std::move(propertyOverrides);
  return std::get<PropertyOverrides>(*propertyOverridesMaybeUnresolved);
}

const PropertyValues &GetResolvedPropertyValues(
    const std::wstring_view type, const std::wstring_view fallbackType,
    PropertyValuesMaybeUnresolved *propertyValuesMaybeUnresolved) {
  if (const auto *resolved =
          std::get_if<PropertyValues>(propertyValuesMaybeUnresolved)) {
    return *resolved;
  }

  PropertyValues propertyValues;

  try {
    const auto &propertyValuesStr =
        std::get<PropertyValuesUnresolved>(*propertyValuesMaybeUnresolved);
    if (!propertyValuesStr.empty()) {
      std::wstring xaml;

      for (const auto &[property, value] : propertyValuesStr) {
        xaml += L"        <Setter Property=\"";
        xaml += EscapeXmlAttribute(property);
        xaml += L"\" Value=\"";
        xaml += EscapeXmlAttribute(value);
        xaml += L"\" />\n";
      }

      auto style =
          GetStyleFromXamlSettersWithFallbackType(type, fallbackType, xaml);

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
  } catch (winrt::hresult_error const &ex) {
    Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
  } catch (std::exception const &ex) {
    Wh_Log(L"Error: %S", ex.what());
  }

  *propertyValuesMaybeUnresolved = std::move(propertyValues);
  return std::get<PropertyValues>(*propertyValuesMaybeUnresolved);
}

// https://stackoverflow.com/a/12835139
VisualStateGroup GetVisualStateGroup(FrameworkElement element,
                                     std::wstring_view visualStateGroupName) {
  auto list = VisualStateManager::GetVisualStateGroups(element);

  for (const auto &v : list) {
    if (v.Name() == visualStateGroupName) {
      return v;
    }
  }

  return nullptr;
}

bool TestElementMatcher(FrameworkElement element, ElementMatcher &matcher,
                        VisualStateGroup *visualStateGroup,
                        PCWSTR fallbackClassName) {
  if (!matcher.type.empty() && matcher.type != winrt::get_class_name(element) &&
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

  for (const auto &propertyValue : GetResolvedPropertyValues(
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
    const auto expectedClassName = winrt::get_class_name(propertyValue.second);
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
    auto &override = *it;

    VisualStateGroup visualStateGroup = nullptr;

    if (!TestElementMatcher(element, override.elementMatcher, &visualStateGroup,
                            fallbackClassName)) {
      continue;
    }

    auto parentElementIter = element;
    bool parentElementMatchFailed = false;

    for (auto &matcher : override.parentElementMatchers) {
      // Using parentElementIter.Parent() was sometimes returning null.
      parentElementIter = Media::VisualTreeHelper::GetParent(parentElementIter)
                              .try_as<FrameworkElement>();
      if (!parentElementIter) {
        parentElementMatchFailed = true;
        break;
      }

      if (!TestElementMatcher(parentElementIter, matcher, &visualStateGroup,
                              nullptr)) {
        parentElementMatchFailed = true;
        break;
      }
    }

    if (parentElementMatchFailed) {
      continue;
    }

    auto &overridesForVisualStateGroup = overrides[visualStateGroup];
    for (const auto &[property, valuesPerVisualState] :
         GetResolvedPropertyOverrides(override.elementMatcher.type,
                                      fallbackClassName
                                          ? fallbackClassName
                                          : winrt::name_of<FrameworkElement>(),
                                      &override.propertyOverrides)) {
      bool propertyInserted = propertiesAdded.insert(property).second;
      if (!propertyInserted) {
        continue;
      }

      auto &propertyOverrides = overridesForVisualStateGroup[property];
      for (const auto &[visualState, value] : valuesPerVisualState) {
        propertyOverrides.insert({visualState, value});
      }
    }
  }

  std::erase_if(overrides, [](const auto &item) {
    auto const &[key, value] = item;
    return value.empty();
  });

  return overrides;
}

void ApplyCustomizationsForVisualStateGroup(
    FrameworkElement element, VisualStateGroup visualStateGroup,
    PropertyOverrides propertyOverrides,
    ElementCustomizationStateForVisualStateGroup
        *elementCustomizationStateForVisualStateGroup) {
  auto elementDo = element.as<DependencyObject>();

  VisualState currentVisualState(
      visualStateGroup ? visualStateGroup.CurrentState() : nullptr);

  std::wstring currentVisualStateName(
      currentVisualState ? currentVisualState.Name() : L"");

  for (const auto &[property, valuesPerVisualState] : propertyOverrides) {
    const auto [propertyCustomizationStatesIt, inserted] =
        elementCustomizationStateForVisualStateGroup
            ->propertyCustomizationStates.insert({property, {}});
    if (!inserted) {
      continue;
    }

    auto &propertyCustomizationState = propertyCustomizationStatesIt->second;

    auto it = valuesPerVisualState.find(currentVisualStateName);
    if (it == valuesPerVisualState.end() && !currentVisualStateName.empty()) {
      it = valuesPerVisualState.find(L"");
    }

    if (it != valuesPerVisualState.end()) {
      propertyCustomizationState.originalValue =
          ReadLocalValueWithWorkaround(element, property);
      propertyCustomizationState.customValue = it->second;
      SetOrClearValue(element, property, it->second,
                      /*initialApply=*/true);
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

              auto localValue = ReadLocalValueWithWorkaround(element, property);

              if (auto *customValue =
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
                winrt::Windows::Foundation::IInspectable const &sender,
                VisualStateChangedEventArgs const &e) {
              auto element = elementWeakRef.get();
              if (!element) {
                return;
              }

              Wh_Log(L"Re-applying all styles for %s",
                     winrt::get_class_name(element).c_str());

              g_elementPropertyModifying = true;

              auto &propertyCustomizationStates =
                  elementCustomizationStateForVisualStateGroup
                      ->propertyCustomizationStates;

              for (const auto &[property, valuesPerVisualState] :
                   propertyOverrides) {
                auto &propertyCustomizationState =
                    propertyCustomizationStates.at(property);

                auto newState = e.NewState();
                auto newStateName =
                    std::wstring{newState ? newState.Name() : L""};
                auto it = valuesPerVisualState.find(newStateName);
                if (it == valuesPerVisualState.end()) {
                  it = valuesPerVisualState.find(L"");
                  if (it != valuesPerVisualState.end()) {
                    auto oldState = e.OldState();
                    auto oldStateName =
                        std::wstring{oldState ? oldState.Name() : L""};
                    if (!valuesPerVisualState.contains(oldStateName)) {
                      continue;
                    }
                  }
                }

                if (it != valuesPerVisualState.end()) {
                  if (!propertyCustomizationState.originalValue) {
                    propertyCustomizationState.originalValue =
                        ReadLocalValueWithWorkaround(element, property);
                  }

                  propertyCustomizationState.customValue = it->second;
                  SetOrClearValue(element, property, it->second);
                } else {
                  if (propertyCustomizationState.originalValue) {
                    SetOrClearValue(element, property,
                                    *propertyCustomizationState.originalValue);
                    propertyCustomizationState.originalValue.reset();
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
    const ElementCustomizationStateForVisualStateGroup
        &elementCustomizationStateForVisualStateGroup) {
  if (element) {
    for (const auto &[property, state] :
         elementCustomizationStateForVisualStateGroup
             .propertyCustomizationStates) {
      element.UnregisterPropertyChangedCallback(property,
                                                state.propertyChangedToken);

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
  std::wstring jsCode = LR"(
        (() => {
        const styleElementId = "windhawk-windows-11-start-menu-styler-style";
        const styleContent = `
    )";

  jsCode += EscapeJsTemplateString(g_webContentCss);

  jsCode += LR"(
        `;
        if (!document.getElementById(styleElementId)) {
            const style = document.createElement("style");
            style.id = styleElementId;
            style.textContent = styleContent;
            document.head.appendChild(style);
        }
    )";

  jsCode += g_webContentJs;

  jsCode += LR"(
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
    WebView2Standalone_IWebView2 *webViewElement) {
  void *sourcePtr;
  winrt::check_hresult(webViewElement->get_Source(&sourcePtr));
  auto source = winrt::Windows::Foundation::Uri{sourcePtr,
                                                winrt::take_ownership_from_abi};
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

  void *operationPtr;
  auto jsCodeHstring = winrt::hstring(jsCode.c_str(), jsCode.size());
  winrt::check_hresult(webViewElement->ExecuteScriptAsync(
      *(void **)(&jsCodeHstring), &operationPtr));
  auto operation = winrt::Windows::Foundation::IAsyncOperation<winrt::hstring>{
      operationPtr, winrt::take_ownership_from_abi};

  return true;
}

void ApplyCustomizationsIfWebView(InstanceHandle handle,
                                  FrameworkElement element) {
  auto className = winrt::get_class_name(element);
  if (className == L"Windows.UI.Xaml.Controls.WebView") {
    auto &webViewCustomizationState = g_webViewsCustomizationState[handle];
    if (!webViewCustomizationState.element.get()) {
      webViewCustomizationState.element = element;

      auto webViewElement = element.as<Controls::WebView>();

      ApplyWebViewStyleCustomizations(webViewElement);

      webViewCustomizationState.navigationCompletedEventToken =
          webViewElement.NavigationCompleted(
              [](const Controls::WebView &sender,
                 const Controls::WebViewNavigationCompletedEventArgs &args) {
                if (args.IsSuccess()) {
                  ApplyWebViewStyleCustomizations(sender);
                }
              });
    }
  } else if (className == L"WebView2Standalone.Controls.WebView2") {
    auto &webViewCustomizationState = g_webViewsCustomizationState[handle];
    if (!webViewCustomizationState.element.get()) {
      webViewCustomizationState.element = element;
      webViewCustomizationState.isWebView2 = true;

      winrt::com_ptr<WebView2Standalone_IWebView2> webViewElement;
      winrt::check_hresult(
          ((IUnknown *)winrt::get_abi(element))
              ->QueryInterface(IID_WebView2Standalone_IWebView2,
                               webViewElement.put_void()));

      ApplyWebView2StyleCustomizations(webViewElement.get());

      winrt::Windows::Foundation::TypedEventHandler<
          winrt::Windows::Foundation::IInspectable,
          winrt::Windows::Foundation::IInspectable>
          eventHandler =
              [](const winrt::Windows::Foundation::IInspectable &sender,
                 const winrt::Windows::Foundation::IInspectable &args) {
                winrt::com_ptr<ICoreWebView2NavigationCompletedEventArgs>
                    webViewArgs;
                winrt::check_hresult(
                    ((IUnknown *)winrt::get_abi(args))
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
                    ((IUnknown *)winrt::get_abi(sender))
                        ->QueryInterface(IID_WebView2Standalone_IWebView2,
                                         webViewElement.put_void()));

                ApplyWebView2StyleCustomizations(webViewElement.get());
              };

      winrt::check_hresult(webViewElement->add_NavigationCompleted(
          *(void **)(&eventHandler),
          put_abi(webViewCustomizationState.navigationCompletedEventToken)));
    }
  }
}

PCWSTR CreateWebViewJsCodeForClear() {
  PCWSTR jsCode = LR"(
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
      L"eval",
      winrt::single_threaded_vector<winrt::hstring>({winrt::hstring(jsCode)}));
}

void ClearWebView2StyleCustomizations(
    WebView2Standalone_IWebView2 *webViewElement) {
  PCWSTR jsCode = CreateWebViewJsCodeForClear();

  void *operationPtr;
  auto jsCodeHstring = winrt::hstring(jsCode);
  winrt::check_hresult(webViewElement->ExecuteScriptAsync(
      *(void **)(&jsCodeHstring), &operationPtr));
  auto operation = winrt::Windows::Foundation::IAsyncOperation<winrt::hstring>{
      operationPtr, winrt::take_ownership_from_abi};
}

void ClearWebViewCustomizations(
    const WebViewCustomizationState &webViewCustomizationState) {
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
    winrt::check_hresult(((IUnknown *)winrt::get_abi(element))
                             ->QueryInterface(IID_WebView2Standalone_IWebView2,
                                              webViewElement.put_void()));

    ClearWebView2StyleCustomizations(webViewElement.get());

    winrt::check_hresult(webViewElement->remove_NavigationCompleted(
        webViewCustomizationState.navigationCompletedEventToken));
  }
}

void ApplyCustomizations(InstanceHandle handle, FrameworkElement element,
                         PCWSTR fallbackClassName) {
  if (!g_webContentCss.empty() || !g_webContentJs.empty()) {
    try {
      ApplyCustomizationsIfWebView(handle, element);
    } catch (winrt::hresult_error const &ex) {
      Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
  }

  auto overrides = FindElementPropertyOverrides(element, fallbackClassName);
  if (overrides.empty()) {
    return;
  }

  Wh_Log(L"Applying styles");

  auto &elementCustomizationState = g_elementsCustomizationState[handle];

  for (const auto &[visualStateGroupOptionalWeakPtrIter, stateIter] :
       elementCustomizationState.perVisualStateGroup) {
    RestoreCustomizationsForVisualStateGroup(
        element, visualStateGroupOptionalWeakPtrIter, stateIter);
  }

  elementCustomizationState.element = element;
  elementCustomizationState.perVisualStateGroup.clear();

  for (auto &[visualStateGroup, overridesForVisualStateGroup] : overrides) {
    std::optional<winrt::weak_ref<VisualStateGroup>>
        visualStateGroupOptionalWeakPtr;
    if (visualStateGroup) {
      visualStateGroupOptionalWeakPtr = visualStateGroup;
    }

    elementCustomizationState.perVisualStateGroup.push_back(
        {visualStateGroupOptionalWeakPtr, {}});
    auto *elementCustomizationStateForVisualStateGroup =
        &elementCustomizationState.perVisualStateGroup.back().second;

    ApplyCustomizationsForVisualStateGroup(
        element, visualStateGroup, std::move(overridesForVisualStateGroup),
        elementCustomizationStateForVisualStateGroup);
  }
}

void CleanupCustomizations(InstanceHandle handle) {
  if (auto it = g_elementsCustomizationState.find(handle);
      it != g_elementsCustomizationState.end()) {
    auto &elementCustomizationState = it->second;

    auto element = elementCustomizationState.element.get();

    for (const auto &[visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
      RestoreCustomizationsForVisualStateGroup(
          element, visualStateGroupOptionalWeakPtrIter, stateIter);
    }

    g_elementsCustomizationState.erase(it);
  }

  if (auto it = g_webViewsCustomizationState.find(handle);
      it != g_webViewsCustomizationState.end()) {
    const auto &webViewCustomizationState = it->second;
    try {
      ClearWebViewCustomizations(webViewCustomizationState);
    } catch (winrt::hresult_error const &ex) {
      Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    g_webViewsCustomizationState.erase(it);
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

StyleConstants
LoadStyleConstants(const std::vector<PCWSTR> &themeStyleConstants) {
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
                   [](const StyleConstant &a, const StyleConstant &b) {
                     return a.first.size() > b.first.size();
                   });

  return result;
}

std::wstring ApplyStyleConstants(std::wstring_view style,
                                 const StyleConstants &styleConstants) {
  std::wstring result;

  size_t lastPos = 0;
  size_t findPos;

  while ((findPos = style.find('$', lastPos)) != style.npos) {
    result.append(style, lastPos, findPos - lastPos);

    const StyleConstant *constant = nullptr;
    for (const auto &s : styleConstants) {
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
        throw std::runtime_error("Bad target syntax, more than one name");
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
        throw std::runtime_error("Bad target syntax, empty property");
      }

      if (rule.find_first_not_of(L"0123456789") == rule.npos) {
        result.oneBasedIndex = std::stoi(std::wstring(rule));
        break;
      }

      auto ruleEqPos = rule.find(L'=');
      if (ruleEqPos == rule.npos) {
        throw std::runtime_error("Bad target syntax, missing '=' in property");
      }

      auto ruleKey = TrimStringView(rule.substr(0, ruleEqPos));
      auto ruleVal = TrimStringView(rule.substr(ruleEqPos + 1));

      if (ruleKey.length() == 0) {
        throw std::runtime_error("Bad target syntax, empty property name");
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

  for (const auto &adjustment : adjustments) {
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
    const auto &targetPart = *i;

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
      for (const auto &style : styles) {
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

  g_elementsCustomizationRules.push_back(std::move(elementCustomizationRules));
}

bool ProcessSingleTargetStylesFromSettings(
    int index, const StyleConstants &styleConstants) {
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

    styles.push_back(ApplyStyleConstants(styleSetting.get(), styleConstants));
  }

  if (styles.size() > 0) {
    AddElementCustomizationRules(targetStringSetting.get(), std::move(styles));
  }

  return true;
}

void ProcessWebStylesFromSettings(
    const StyleConstants &styleConstants,
    const std::vector<ThemeTargetStyles> &themeStyles) {
  std::wstring webContentCss;

  for (const auto &themeStyle : themeStyles) {
    Wh_Log(L"Processing theme WebView styles for %s", themeStyle.target);

    webContentCss += themeStyle.target;
    webContentCss += L" {\n";

    for (const auto &style : themeStyle.styles) {
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

      webContentCss += ApplyStyleConstants(styleSetting.get(), styleConstants);
      webContentCss += L";\n";
    }

    webContentCss += L"}\n";
  }

  g_webContentCss = std::move(webContentCss);
  g_webContentJs =
      string_setting_unique_ptr(Wh_GetStringSetting(L"webContentCustomJs"))
          .get();
}

void ProcessAllStylesFromSettings() {
  PCWSTR themeName = Wh_GetStringSetting(L"theme");
  const Theme *theme = nullptr;
  if (wcscmp(themeName, L"NeoStart") == 0) {
    theme = g_isRedesignedStartMenu ? &g_themeNeoStart
                                    : &g_themeNeoStart_variant_ClassicStartMenu;
  }
  Wh_FreeStringSetting(themeName);

  StyleConstants styleConstants =
      LoadStyleConstants(theme ? theme->styleConstants : std::vector<PCWSTR>{});

  if (theme) {
    for (const auto &themeTargetStyle : theme->targetStyles) {
      try {
        std::vector<std::wstring> styles;
        styles.reserve(themeTargetStyle.styles.size());
        for (const auto &s : themeTargetStyle.styles) {
          styles.push_back(ApplyStyleConstants(s, styleConstants));
        }

        AddElementCustomizationRules(themeTargetStyle.target,
                                     std::move(styles));
      } catch (winrt::hresult_error const &ex) {
        Wh_Log(L"Error %08X", ex.code());
      } catch (std::exception const &ex) {
        Wh_Log(L"Error: %S", ex.what());
      }
    }
  }

  for (int i = 0;; i++) {
    try {
      if (!ProcessSingleTargetStylesFromSettings(i, styleConstants)) {
        break;
      }
    } catch (winrt::hresult_error const &ex) {
      Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const &ex) {
      Wh_Log(L"Error: %S", ex.what());
    }
  }

  if (g_target == Target::SearchHost) {
    ProcessWebStylesFromSettings(styleConstants,
                                 theme ? theme->webViewTargetStyles
                                       : std::vector<ThemeTargetStyles>{});
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
    } catch (winrt::hresult_error const &ex) {
      Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const &ex) {
      Wh_Log(L"Error: %S", ex.what());
    }
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

  for (const auto &[handle, elementCustomizationState] :
       g_elementsCustomizationState) {
    auto element = elementCustomizationState.element.get();

    for (const auto &[visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
      RestoreCustomizationsForVisualStateGroup(
          element, visualStateGroupOptionalWeakPtrIter, stateIter);
    }
  }

  g_elementsCustomizationState.clear();

  g_elementsCustomizationRules.clear();

  for (const auto &[handle, webViewCustomizationState] :
       g_webViewsCustomizationState) {
    try {
      ClearWebViewCustomizations(webViewCustomizationState);
    } catch (winrt::hresult_error const &ex) {
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
  ProcessResourceVariablesFromSettings();

  HRESULT hr = InjectWindhawkTAP();
  if (FAILED(hr)) {
    Wh_Log(L"Error %08X", hr);
  }

  // Unregister global network status change handler.
  if (g_networkStatusChangedToken) {
    try {
      winrt::Windows::Networking::Connectivity::NetworkInformation::
          NetworkStatusChanged(g_networkStatusChangedToken);
      Wh_Log(L"Unregistered global network status change handler");
    } catch (winrt::hresult_error const &ex) {
      Wh_Log(L"Error unregistering network status handler %08X: %s", ex.code(),
             ex.message().c_str());
    }
    g_networkStatusChangedToken = {};
  }

  // Clear the dispatcher registry.
  {
    std::lock_guard<std::mutex> lock(g_failedImageBrushesRegistryMutex);
    g_failedImageBrushesRegistry.clear();
  }
}

using CreateWindowInBand_t = HWND(WINAPI *)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, PVOID lpParam, DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName, DWORD dwStyle, int X,
                                    int Y, int nWidth, int nHeight,
                                    HWND hWndParent, HMENU hMenu,
                                    HINSTANCE hInstance, PVOID lpParam,
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
    Wh_Log(L"Initializing - Created core window: %08X", (DWORD)(ULONG_PTR)hWnd);
    InitializeSettingsAndTap();
  }

  return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI *)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
    HINSTANCE hInstance, PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                      LPCWSTR lpWindowName, DWORD dwStyle,
                                      int X, int Y, int nWidth, int nHeight,
                                      HWND hWndParent, HMENU hMenu,
                                      HINSTANCE hInstance, PVOID lpParam,
                                      DWORD dwBand, DWORD dwTypeFlags) {
  HWND hWnd = CreateWindowInBandEx_Original(
      dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
      hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
  if (!hWnd) {
    return hWnd;
  }

  BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

  if (bTextualClassName &&
      _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
    Wh_Log(L"Initializing - Created core window: %08X", (DWORD)(ULONG_PTR)hWnd);
    InitializeSettingsAndTap();
  }

  return hWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI *)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc,
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
          const CWPSTRUCT *cwp = (const CWPSTRUCT *)lParam;
          if (cwp->message == runFromWindowThreadRegisteredMsg) {
            RUN_FROM_WINDOW_THREAD_PARAM *param =
                (RUN_FROM_WINDOW_THREAD_PARAM *)cwp->lParam;
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
    HWND *hWnd;
  };

  HWND hWnd = nullptr;
  ENUM_WINDOWS_PARAM param = {&hWnd};
  EnumWindows(
      [](HWND hWnd, LPARAM lParam) -> BOOL {
        ENUM_WINDOWS_PARAM &param = *(ENUM_WINDOWS_PARAM *)lParam;

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

using RtlQueryFeatureConfiguration_t =
    int(NTAPI *)(UINT32, int, INT64 *, RTL_FEATURE_CONFIGURATION *);
RtlQueryFeatureConfiguration_t RtlQueryFeatureConfiguration_Original;
int NTAPI RtlQueryFeatureConfiguration_Hook(UINT32 featureId, int group,
                                            INT64 *variant,
                                            RTL_FEATURE_CONFIGURATION *config) {
  int ret =
      RtlQueryFeatureConfiguration_Original(featureId, group, variant, config);

  switch (featureId) {
  // Disable the Start Menu Phone Link layout feature.
  // https://winaero.com/enable-phone-link-flyout-start-menu/
  case 48697323:
  // Disable the revamped Start menu experience.
  // https://x.com/phantomofearth/status/1907877141540118888
  case 47205210:
  // case 49221331:
  case 49402389:
    config->enabledState = FEATURE_ENABLED_STATE_DISABLED;
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
    Wh_Log(L"RtlQueryFeatureConfiguration error for %u: %08X", featureId, hr);
  }

  return std::nullopt;
}

PTP_TIMER g_statsTimer;

bool StartStatsTimer() {
  static constexpr WCHAR kStatsBaseUrl[] =
      L"https://github.com/ramensoftware/"
      L"windows-11-start-menu-styling-guide/"
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

        HANDLE mutex = CreateMutex(nullptr, FALSE, L"WindhawkStats_" WH_MOD_ID);
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

        const WH_URL_CONTENT *content = nullptr;
        if (currentTime - lastStatsTime >= k10Minutes) {
          Wh_SetBinaryValue(L"statsTimerLastTime", &currentTime,
                            sizeof(currentTime));

          std::wstring themeNameEscaped = themeName.get();
          std::replace(themeNameEscaped.begin(), themeNameEscaped.end(), L' ',
                       L'_');
          std::replace(themeNameEscaped.begin(), themeNameEscaped.end(), L'&',
                       L'_');

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

  g_disableNewStartMenuLayout = Wh_GetIntSetting(L"disableNewStartMenuLayout");

  g_isRedesignedStartMenu = !g_disableNewStartMenuLayout &&
                            IsOsFeatureEnabled(47205210).value_or(false) &&
                            IsOsFeatureEnabled(49221331).value_or(false) &&
                            IsOsFeatureEnabled(49402389).value_or(false);

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

  HMODULE user32Module =
      LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
  if (user32Module) {
    void *pCreateWindowInBand =
        (void *)GetProcAddress(user32Module, "CreateWindowInBand");
    if (pCreateWindowInBand) {
      Wh_SetFunctionHook(pCreateWindowInBand, (void *)CreateWindowInBand_Hook,
                         (void **)&CreateWindowInBand_Original);
    }

    void *pCreateWindowInBandEx =
        (void *)GetProcAddress(user32Module, "CreateWindowInBandEx");
    if (pCreateWindowInBandEx) {
      Wh_SetFunctionHook(pCreateWindowInBandEx,
                         (void *)CreateWindowInBandEx_Hook,
                         (void **)&CreateWindowInBandEx_Original);
    }
  }

  if (g_target == Target::StartMenu && g_disableNewStartMenuLayout) {
    HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
    RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration =
        (RtlQueryFeatureConfiguration_t)GetProcAddress(
            hNtDll, "RtlQueryFeatureConfiguration");
    if (pRtlQueryFeatureConfiguration) {
      Wh_SetFunctionHook((void *)pRtlQueryFeatureConfiguration,
                         (void *)RtlQueryFeatureConfiguration_Hook,
                         (void **)&RtlQueryFeatureConfiguration_Original);
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

  if (g_target == Target::StartMenu) {
    if (g_disableNewStartMenuLayout) {
      // Exit to have the new setting take effect. The process will be
      // relaunched automatically.
      ExitProcess(0);
    }

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
}

void Wh_ModSettingsChanged() {
  Wh_Log(L">");

  if (g_target == Target::StartMenu &&
      Wh_GetIntSetting(L"disableNewStartMenuLayout") !=
          g_disableNewStartMenuLayout) {
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