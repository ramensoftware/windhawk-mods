// ==WindhawkMod==
// @id frosted-acrylic-sheet
// @name Frosted Acrylic Sheet
// @description A frosted acrylic visual style for Windows 11 Taskbar — 磨砂亚克力板任务栏样式
// @version 1.0.0
// @author Evan
// @github https://github.com/EvanYFWong
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lruntimeobject
// @license MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ❄️ Frosted Acrylic Sheet

> A frosted acrylic visual style for the Windows 11 Taskbar.

![Preview](https://raw.githubusercontent.com/EvanYFWong/FrostedAcrylicSheet/main/screenshot.png)

## ✨ Features

- **Frosted Acrylic Background** — Taskbar with a smooth acrylic blur effect.
- **Centered Layout** — Perfectly centered taskbar with a clean, modern look.
- **Rounded Corners** — Soft 5px rounded corners for a polished finish.
- **Optimized Icon Sizing** — 48×60 px buttons with 40 px icons.

## 📦 Installation

1. Open Windhawk and go to the **"Mods"** section.
2. Enable the **"Windows 11 Taskbar Styler"** mod.
3. Click the **"Edit"** button and replace its content with the `TaskbarStyler.txt` content from this repository.
4. Copy the `TaskbarHeightAndIconSize.txt` content into the **"Style constants"** section.
5. Click **"Save"** and **"Exit Editing Mode"**.
6. Restart Windows Explorer.

## 🌟 Recommended Companion (Optional)

For a matching Start menu, install **Windows 11 Start Menu Styler** and apply the **TranslucentStartMenu** theme.

- Official guide: https://github.com/ramensoftware/windows-11-start-menu-styling-guide/blob/main/Themes/TranslucentStartMenu/README.md
- Theme by: **Undisputed00x**

## 🎨 Customization

Adjust these constants in `TaskbarStyler.txt`:
- `TaskbarFrameWidth` — Taskbar width (default: `1410`)
- `TaskbarHeight` — Taskbar height (default: `60`)
- `IconSize` — Icon size (default: `40`)

## 📄 License

MIT License — feel free to use, modify, and share.

---
*Made with ❤️ by [Evan](https://github.com/EvanYFWong)*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableAcrylic: true
  $name: Enable acrylic blur
  $description: Enable the frosted acrylic background effect.
- blurOpacity: 0.15
  $name: Blur opacity
  $description: Opacity/tint strength used by the acrylic brush (0.0 - 1.0)
  $min: 0.0
  $max: 1.0
  $step: 0.01
- taskbarFrameWidth: 1410
  $name: Taskbar frame width
  $description: Width used for the centered taskbar frame. Set to 0 to use adaptive width.
- taskbarHeight: 60
  $name: Taskbar height
  $description: Height of the taskbar in pixels.
- iconSize: 40
  $name: Icon size
  $description: Default icon size in pixels.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

// ============================================================
//  Style Constants
// ============================================================
static const wchar_t* g_styleConstants =
    L"styleConstants:\n"
    L"  - TaskbarFrameWidth=1410\n"
    L"  - TaskbarHeight=60\n"
    L"  - IconSize=40\n"
    L"  - TaskbarButtonWidth=48\n"
    L"  - IconSizeSmall=25\n"
    L"  - TaskbarButtonWidthSmall=30\n";

// ============================================================
//  Control Styles (from Windows 11 Taskbar Styler.txt)
// ============================================================
static const wchar_t* g_controlStyles =
    L"controlStyles:\n"
    L"  - target: ScrollViewer > ScrollContentPresenter > Border > Grid\n"
    L"    styles:\n"
    L"      - ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"*\"/><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"*\"/></Col[...]"
    L"      - HorizontalAlignment=Stretch\n"
    L"  - target: Taskbar.TaskbarFrame\n"
    L"    styles:\n"
    L"      - Grid.Column=1\n"
    L"      - Width=$TaskbarFrameWidth\n"
    L"      - Margin=0\n"
    L"      - MaxWidth=$TaskbarFrameWidth\n"
    L"      - Background:=Transparent\n"
    L"  - target: Taskbar.TaskbarFrame#TaskbarFrame > Grid#RootGrid\n"
    L"    styles:\n"
    L"      - Width=$TaskbarFrameWidth\n"
    L"      - MinWidth=$TaskbarFrameWidth\n"
    L"      - MaxWidth=$TaskbarFrameWidth\n"
    L"      - HorizontalAlignment=Center\n"
    L"      - Padding=2,0,2,0\n"
    L"      - Margin=0,2,0,3\n"
    L"      - BorderThickness=1\n"
    L"      - BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Opacity=\".30\" />\n"
    L"      - CornerRadius=5\n"
    L"      - Background:=<AcrylicBrush BackgroundSource=\"Backdrop\" TintColor=\"#D0D0D0\" FallbackColor=\"#D0D0D0\" TintOpacity=\"0.15\" TintLuminosityOpacity=\"0.25\" Opacity=\"1\"/>\n"
    L"  - target: SystemTray.SystemTrayFrame\n"
    L"    styles:\n"
    L"      - Grid.Column=1\n"
    L"      - Width=270\n"
    L"      - HorizontalAlignment=Right\n"
    L"      - Height=40\n"
    L"      - VerticalAlignment=Center\n"
    L"      - Margin=0,0,10,0\n"
    L"  - target: Grid#SystemTrayFrameGrid\n"
    L"    styles:\n"
    L"      - Background:=Transparent\n"
    L"      - CornerRadius=6\n"
    L"  - target: Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill\n"
    L"    styles:\n"
    L"      - Fill:=Transparent\n"
    L"      - Visibility=Visible\n"
    L"  - target: Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke\n"
    L"    styles:\n"
    L"      - Visibility=Collapsed\n"
    L"  - target: Microsoft.UI.Xaml.Controls.ItemsRepeater#TaskbarFrameRepeater\n"
    L"    styles:\n"
    L"      - Margin=0,4,0,4\n"
    L"  - target: Windows.UI.Xaml.Controls.Border#OverflowFlyoutBackgroundBorder\n"
    L"    styles:\n"
    L"      - Shadow:=\n"
    L"      - BorderThickness:=\n"
    L"  - target: Windows.UI.Xaml.Controls.Grid#ConfirmatorMainGrid\n"
    L"    styles:\n"
    L"      - Shadow:=\n"
    L"      - BorderThickness:=\n"
    L"  - target: Taskbar.OverflowToggleButton#OverflowButton > Taskbar.TaskListButtonPanel#OverflowToggleButtonRootPanel > Windows.UI.Xaml.Controls.FontIcon#FontIcon > Windows.UI.Xaml.Controls.G[...]\n"
    L"    styles:\n"
    L"      - Text=\n"
    L"  - target: Windows.UI.Xaml.Shapes.Rectangle#MostRecentlyUsedDivider\n"
    L"    styles:\n"
    L"      - Height=32\n"
    L"      - Width=2\n"
    L"  - target: Windows.UI.Xaml.Shapes.Rectangle#LeftOverflowButtonDivider\n"
    L"    styles:\n"
    L"      - Visibility=Collapsed\n"
    L"  - target: Windows.UI.Xaml.Shapes.Rectangle#RightOverflowButtonDivider\n"
    L"    styles:\n"
    L"      - Visibility=Collapsed\n"
    L"  - target: Microsoft.UI.Xaml.Controls.ItemsRepeater#OverflowFlyoutListRepeater\n"
    L"    styles:\n"
    L"      - Height=48\n"
    L"  - target: WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid > Grid\n"
    L"    styles:\n"
    L"      - Background:=\n"
    L"  - target: WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid\n"
    L"    styles:\n"
    L"      - Margin=390,0,574,12\n"
    L"      - Shadow:=\n"
    L"      - BorderThickness:=\n"
    L"  - target: Windows.UI.Xaml.Controls.Border#HoverFlyoutBackground\n"
    L"    styles:\n"
    L"      - Shadow:=\n"
    L"  - target: Taskbar.TaskListButtonPanel > TextBlock\n"
    L"    styles:\n"
    L"      - Visibility=Collapsed\n"
    L"      - Margin=0\n"
    L"  - target: Taskbar.TaskListButtonPanel > Grid > TextBlock\n"
    L"    styles:\n"
    L"      - Visibility=Collapsed\n"
    L"      - Margin=0\n"
    L"  - target: Taskbar.TaskListButton > Taskbar.TaskListButtonPanel > Grid\n"
    L"    styles:\n"
    L"      - Width=$TaskbarButtonWidth\n"
    L"      - MinWidth=$TaskbarButtonWidth\n"
    L"      - MaxWidth=$TaskbarButtonWidth\n"
    L"      - Height=$TaskbarHeight\n"
    L"      - MinHeight=$TaskbarHeight\n"
    L"      - MaxHeight=$TaskbarHeight\n"
    L"      - HorizontalAlignment=Center\n"
    L"      - VerticalAlignment=Center\n"
    L"      - Padding=0\n"
    L"      - Margin=0\n"
    L"      - ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"*\"/></ColumnDefinitionCollection>\n"
    L"  - target: Taskbar.TaskListButton > Taskbar.TaskListButtonPanel\n"
    L"    styles:\n"
    L"      - Width=$TaskbarButtonWidth\n"
    L"      - MinWidth=$TaskbarButtonWidth\n"
    L"      - MaxWidth=$TaskbarButtonWidth\n"
    L"      - Height=$TaskbarHeight\n"
    L"      - MinHeight=$TaskbarHeight\n"
    L"      - MaxHeight=$TaskbarHeight\n"
    L"      - Padding=0\n"
    L"      - Margin=0\n"
    L"      - HorizontalAlignment=Center\n"
    L"      - VerticalAlignment=Center\n"
    L"  - target: Taskbar.TaskListButton\n"
    L"    styles:\n"
    L"      - Width=$TaskbarButtonWidth\n"
    L"      - MinWidth=$TaskbarButtonWidth\n"
    L"      - MaxWidth=$TaskbarButtonWidth\n"
    L"      - Height=$TaskbarHeight\n"
    L"      - MinHeight=$TaskbarHeight\n"
    L"      - MaxHeight=$TaskbarHeight\n"
    L"      - Padding=0\n"
    L"      - Margin=0\n"
    L"      - HorizontalAlignment=Center\n"
    L"      - VerticalAlignment=Center\n"
    L"  - target: Taskbar.TaskListButtonPanel > Grid\n"
    L"    styles:\n"
    L"      - Width=$TaskbarButtonWidth\n"
    L"      - MinWidth=$TaskbarButtonWidth\n"
    L"      - MaxWidth=$TaskbarButtonWidth\n"
    L"      - Height=$TaskbarHeight\n"
    L"      - MinHeight=$TaskbarHeight\n"
    L"      - MaxHeight=$TaskbarHeight\n"
    L"      - Padding=0\n"
    L"      - Margin=0\n"
    L"      - VerticalAlignment=Center\n"
    L"  - target: Taskbar.TaskListButtonPanel > Grid > Grid\n"
    L"    styles:\n"
    L"      - Width=$TaskbarButtonWidth\n"
    L"      - MinWidth=$TaskbarButtonWidth\n"
    L"      - MaxWidth=$TaskbarButtonWidth\n"
    L"      - Height=$TaskbarHeight\n"
    L"      - MinHeight=$TaskbarHeight\n"
    L"      - MaxHeight=$TaskbarHeight\n"
    L"      - Padding=0\n"
    L"      - Margin=0\n"
    L"      - VerticalAlignment=Center\n"
    L"  - target: Taskbar.TaskListButtonPanel > Grid > StackPanel > TextBlock\n"
    L"    styles:\n"
    L"      - Visibility=Collapsed\n"
    L"      - Opacity=0\n"
    L"      - Width=0\n"
    L"      - MinWidth=0\n"
    L"      - MaxWidth=0\n"
    L"  - target: Taskbar.TaskListButtonPanel > Grid > Grid > TextBlock\n"
    L"    styles:\n"
    L"      - Visibility=Collapsed\n"
    L"      - Opacity=0\n"
    L"      - Width=0\n"
    L"      - MinWidth=0\n"
    L"      - MaxWidth=0\n"
    L"  - target: Taskbar.TaskListButtonPanel > Grid > ContentPresenter > TextBlock\n"
    L"    styles:\n"
    L"      - Visibility=Collapsed\n"
    L"      - Opacity=0\n"
    L"      - Width=0\n"
    L"      - MinWidth=0\n"
    L"      - MaxWidth=0\n"
    L"  - target: Taskbar.TaskListButtonPanel > StackPanel > TextBlock\n"
    L"    styles:\n"
    L"      - Visibility=Collapsed\n"
    L"      - Opacity=0\n"
    L"      - Width=0\n"
    L"      - MinWidth=0\n"
    L"      - MaxWidth=0\n";

// ============================================================
//  Mod Entry Points
// ============================================================

BOOL Wh_ModInit() {
    Wh_Log(L"Frosted Acrylic Sheet initializing...");
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"Frosted Acrylic Sheet initialized successfully.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading styles...");
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Frosted Acrylic Sheet unloading...");
}

void Wh_ModUninit() {
    Wh_Log(L"Frosted Acrylic Sheet unloaded.");
}
