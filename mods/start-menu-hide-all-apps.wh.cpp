// ==WindhawkMod==
// @id              start-menu-hide-all-apps
// @name            Windows 11 Start Menu Complete Styler
// @description     Mod visual completo para personalizar o menu Iniciar do Windows 11, ocultando seções indesejadas e ajustando aparência
// @version         1.0
// @author          Dznas
// @github          https://github.com/Dznas
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Start Menu Complete Styler

Mod visual completo para personalizar o menu Iniciar do Windows 11.

## Funcionalidades
- Oculta a seção "Todos" (All Apps)
- Remove o título "Todos"
- Remove categorias automáticas (Acessibilidade, Criatividade, etc)
- Remove barra de pesquisa
- Personaliza cantos arredondados (CornerRadius=30)
- Ajusta efeitos acrílicos e cores
- Modifica margens e posições dos elementos
- Mantém apenas os apps fixados (Pinned)

## Diferença do pinned-only-on-start-menu
Este mod oferece **personalização visual completa** do Start Menu, enquanto o `pinned-only-on-start-menu` usa políticas nativas do sistema para esconder seções.

## Como usar
1. Instale o Windhawk
2. Instale este mod
3. Reinicie o Explorer ou o computador

## Nota
Este mod funciona apenas no Windows 11 com o menu Iniciar padrão.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
{
  "theme": "",
  "disableNewStartMenuLayout": "",
  "styleConstants": [
    ""
  ],
  "controlStyles": [
    {
      "target": "Grid#FrameRoot",
      "styles": [
        "MaxHeight=520"
      ]
    },
    {
      "target": "TextBlock#ZoomedOutHeading",
      "styles": [
        "Visibility=Collapsed"
      ]
    },
    {
      "target": "Windows.UI.Xaml.Controls.Grid#TopLevelSuggestionsListHeader",
      "styles": [
        "Height=0",
        "Visibility=>showMoreSuggestionsVisible"
      ]
    },
    {
      "target": "Grid#ShowMoreSuggestions",
      "styles": [
        "Visibility={{showMoreSuggestionsVisible}}"
      ]
    },
    {
      "target": "Button#ShowMoreSuggestionsButton",
      "styles": [
        "Margin=0,-77,147,0"
      ]
    },
    {
      "target": "Windows.UI.Xaml.Controls.Grid#NoTopLevelSuggestionsText",
      "styles": [
        "Height=0"
      ]
    },
    {
      "target": "Button#ShowMoreSuggestionsButton > Grid > ContentPresenter > StackPanel > TextBlock",
      "styles": [
        "Text=Recommended",
        "Visibility=Visible"
      ]
    },
    {
      "target": "Border#StartDropShadow",
      "styles": [
        "CornerRadius=30"
      ]
    },
    {
      "target": "StartMenu.SearchBoxToggleButton",
      "styles": [
        "Visibility=Collapsed"
      ]
    },
    {
      "target": "Border#AcrylicBorder",
      "styles": [
        "CornerRadius=30",
        "Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>"
      ]
    },
    {
      "target": "Border#AcrylicOverlay",
      "styles": [
        "CornerRadius=30",
        "Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\"1\" Opacity=\"1\"/>",
        "Height=430",
        "Margin=0,-65,0,0"
      ]
    },
    {
      "target": "Windows.UI.Xaml.Controls.Grid#AllAppsRoot",
      "styles": [
        "Visibility=Collapsed"
      ]
    },
    {
      "target": "Grid#MainContent",
      "styles": [
        "Grid.Row=0",
        "VerticalAlignment=0",
        "MinHeight=Auto"
      ]
    },
    {
      "target": "StartDocked.AppListView#NavigationPanePlacesListView > Windows.UI.Xaml.Controls.Border",
      "styles": [
        "Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        "CornerRadius=18",
        "Margin=0,0,15,0"
      ]
    },
    {
      "target": "StartDocked.NavigationPaneButton#PowerButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder",
      "styles": [
        "Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\"1\" Opacity=\"1\"/>",
        "BorderBrush@Normal:=<AcrylicBrush TintColor=\"{ThemeResource SurfaceStrokeColorDefault}\" FallbackColor=\"{ThemeResource SurfaceStrokeColorDefault}\" TintOpacity=\"0\" TintLuminosityOpacity=\".1\" Opacity=\"1\"/>",
        "CornerRadius=30",
        "BorderThickness=5",
        "Margin=-7",
        "BorderBrush@PointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColor}\" FallbackColor=\"{ThemeResource SystemAccentColor}\" TintOpacity=\".8\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>"
      ]
    },
    {
      "target": "StartDocked.NavigationPaneButton#UserTileButton > Grid > Border#BackgroundBorder",
      "styles": [
        "Background:=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".5\" Opacity=\"1\"/>",
        "CornerRadius=18"
      ]
    },
    {
      "target": "Grid#TopLevelHeader > Grid > Button[AutomationProperties.Name=Show all] > Grid@CommonStates > Border",
      "styles": [
        "Background@Normal:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\".8\"/>",
        "Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\"1\" />",
        "Padding=10,7",
        "Margin=0,0,-5,0",
        "CornerRadius=0,15,15,0",
        "BorderThickness=0",
        "Width=85"
      ]
    },
    {
      "target": "Button#ShowMoreSuggestionsButton > Grid@CommonStates > Border",
      "styles": [
        "Background@Normal:=<SolidColorBrush Color=\"{ThemeResource SystemAltMediumLowColor}\" Opacity=\"0\" />",
        "BorderBrush@Normal:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\".8\"/>",
        "Padding=10,5",
        "Margin=0,0,-2,0",
        "CornerRadius=15,0,0,15",
        "BorderThickness=2,2,0,2",
        "Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\".7\" />",
        "BorderBrush@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\"1\"/>"
      ]
    },
    {
      "target": "Windows.UI.Xaml.Controls.Button#HideMoreSuggestionsButton",
      "styles": [
        "Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeMediumLowColor}\" Opacity=\"1\"/>",
        "CornerRadius=15"
      ]
    },
    {
      "target": "StartDocked.NavigationPaneView > Windows.UI.Xaml.Controls.Grid#RootPanel",
      "styles": [
        "Grid.Row=2"
      ]
    },
    {
      "target": "Windows.UI.Xaml.Controls.Frame",
      "styles": [
        "Margin=0,-65,0,0"
      ]
    },
    {
      "target": "Grid#MainMenu",
      "styles": [
        "MaxWidth=650"
      ]
    },
    {
      "target": "TextBlock#AllListHeadingText",
      "styles": [
        "Visibility=Collapsed"
      ]
    },
    {
      "target": "Windows.UI.Xaml.Controls.GridView#RecommendedList",
      "styles": [
        "Visibility=Collapsed"
      ]
    },
    {
      "target": "Windows.UI.Xaml.Controls.GridView#AllAppsGrid > Border > Windows.UI.Xaml.Controls.ScrollViewer > Border > Grid > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.ItemsWrapGrid",
      "styles": [
        "Visibility=Collapsed"
      ]
    },
    {
      "target": "Microsoft.UI.Xaml.Controls.DropDownButton > Grid@CommonStates",
      "styles": [
        "BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\".8\"/>",
        "Background:=<SolidColorBrush Color=\"{ThemeResource SystemAltMediumLowColor}\" Opacity=\"1\" />",
        "BorderThickness={{showMoreSuggestionsVisible*2}},2,2,2",
        "CornerRadius={{showMoreSuggestionsVisible*15}},15,15,{{showMoreSuggestionsVisible*15}}",
        "Height=32",
        "BorderBrush@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\"1\"/>",
        "Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\".7\" />"
      ]
    },
    {
      "target": "StartMenu.PinnedList",
      "styles": [
        "Margin=0,20,12,0",
        "ActualHeight=>pinnedListHeight"
      ]
    },
    {
      "target": "Grid#TopLevelSuggestionsRoot",
      "styles": [
        "Grid.Row=1"
      ]
    },
    {
      "target": "Microsoft.UI.Xaml.Controls.DropDownButton",
      "styles": [
        "RenderTransform:=<TranslateTransform X=\"-5\" Y=\"{{-254 - pinnedListHeight}}\" />",
        "MaxWidth=100"
      ]
    },
    {
      "target": "Microsoft.UI.Xaml.Controls.DropDownButton > Grid > ContentPresenter > TextBlock",
      "styles": [
        "Margin=8,0,8,0",
        "Text=View"
      ]
    },
    {
      "target": "StartMenu.CategoryControl",
      "styles": [
        "Visibility=Collapsed"
      ]
    },
    {
      "target": "Grid#TopLevelHeader > Grid > Button",
      "styles": [
        "Margin=-430,0,430,0",
        "Height=32",
        "CornerRadius=15",
        "BorderThickness=0,2,2,2"
      ]
    },
    {
      "target": "Border#RightCompanionDropShadow",
      "styles": [
        "CornerRadius=30"
      ]
    },
    {
      "target": "Grid#CompanionRoot > Grid#MainContent > Border#AcrylicOverlay",
      "styles": [
        "Margin=0"
      ]
    },
    {
      "target": "Windows.UI.Xaml.Controls.Primitives.ScrollBar",
      "styles": [
        "Visibility=Collapsed"
      ]
    },
    {
      "target": "Grid#TopLevelHeader > Grid > Button > Grid@CommonStates > Border",
      "styles": [
        "Background:=<SolidColorBrush Color=\"{ThemeResource SystemAltMediumLowColor}\" Opacity=\"1\" />",
        "BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\".8\"/>",
        "Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\".7\" />",
        "BorderBrush@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemBaseLowColor}\" Opacity=\"1\"/>",
        "BorderThickness=2"
      ]
    },
    {
      "target": "Windows.UI.Xaml.Controls.Primitives.ToggleButton",
      "styles": [
        "Margin=12,7,-12,-7"
      ]
    },
    {
      "target": "Grid#MainMenu > Grid#MainContent > Grid",
      "styles": [
        "Canvas.ZIndex=1"
      ]
    },
    {
      "target": "TextBlock#AppDisplayName",
      "styles": [
        "Visibility=Collapsed"
      ]
    },
    {
      "target": "TextBlock#Text",
      "styles": [
        "Visibility=Collapsed"
      ]
    }
  ],
  "themeResourceVariables": [
    ""
  ],
  "webContentStyles": [
    {
      "target": "",
      "styles": [
        ""
      ]
    }
  ],
  "webContentCustomJs": ""
}
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
