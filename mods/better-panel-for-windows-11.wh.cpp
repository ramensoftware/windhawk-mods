// ==WindhawkMod==
// @id              better-panel-for-windows-11
// @name            Better Panel for Windows 11
// @description     Upgrades the Windows 11 Explorer details pane with previews, media playback, archive tools, file actions, and cross-tab transfers
// @version         2.2.0
// @author          Nicole S
// @github          https://github.com/NikkiD97
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -lbcrypt
// @license         GPL-3.0-or-later
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- hoverAnimation:
  - style: windows
    $name: Hover style
    $description: "Default: Windows default. Adds motion when the pointer moves over any Better Panel button, including Home cards and Details rows. None removes added motion while keeping native Windows color feedback."
    $options:
    - windows: Windows default
    - none: None
    - scale: Gentle scale
    - lift: Lift
    - tilt: Tilt
    - spring: Spring
  $name: Hover animation
  $description: Choose how Better Panel controls respond when the pointer moves over them.
- clickAnimation:
  - style: windows
    $name: Click style
    $description: "Default: Windows default. Adds motion after mouse, touch, pen, or keyboard activation. None removes added motion while keeping the button's native Windows response."
    $options:
    - windows: Windows default
    - none: None
    - press: Press and release
    - pulse: Pulse
    - bounce: Bounce
    - flash: Flash
    - shake: Shake
    - spring: Spring
    - spin: Spin
    - squash: Squash
    - wobble: Wobble
  $name: Click animation
  $description: Choose how every Better Panel button moves when activated.
- buttonLayout:
  - mode: native
    $name: Layout mode
    $description: "Default: Compact. Comfort uses the displayed default values below. Custom uses your entered values."
    $options:
    - native: Compact
    - comfort: Comfort
    - custom: Custom
  - horizontalMargin: 0
    $name: Horizontal margin
    $description: "Default: 0 px. Range: -20 to 40 px. Space added to the left and right of each button. Custom mode only."
  - verticalMargin: 0
    $name: Vertical margin
    $description: "Default: 0 px. Range: -20 to 40 px. Space added above and below each button. Custom mode only."
  - horizontalPadding: 10
    $name: Horizontal padding
    $description: "Default: 10 px. Range: 0 to 40 px. Space between a labeled button's content and its left and right edges. Custom mode only."
  - verticalPadding: 4
    $name: Vertical padding
    $description: "Default: 4 px. Range: 0 to 30 px. Space between a labeled button's content and its top and bottom edges. Custom mode only."
  - minimumWidth: 0
    $name: Minimum button width
    $description: "Default: 0 px. Range: 0 to 240 px. Minimum width for labeled buttons; zero keeps their natural width. Custom mode only."
  - minimumHeight: 32
    $name: Minimum button height
    $description: "Default: 32 px. Range: 24 to 80 px. Minimum height for labeled buttons. Custom mode only."
  - iconButtonSize: 32
    $name: Icon-only button size
    $description: "Default: 32 px. Range: 15 to 64 px. Width and height for compact icon-only buttons; the audio speed button keeps enough width for its text. Custom mode only."
  $name: Button layout
  $description: Choose a compact or comfortable preset, or customize Better Panel button spacing and dimensions.
*/
// ==/WindhawkModSettings==

// Copyright (C) 2026 Nicole S
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// This standalone local package is maintained independently. Upstream
// open-source attribution is retained in the source where required.

// ==WindhawkModReadme==
/*
# Better Panel for Windows 11

**Better Panel for Windows 11** is a standalone Windhawk mod package that adds
an interactive action area, rich previews, media playback, archive tools, and
cross-tab file operations to the modern Windows 11 File Explorer details pane.

This package has its own mod ID, source, DLL, settings, changelog, and release
version.

## WARNING: Windows compatibility

**Windows 11 25H2 x64 is the only version currently confirmed working.** It has
been tested on Windows 11 25H2 build 26200.8875.

Windows 11 24H2 x64 is expected to work because 24H2 and 25H2 share the same
core operating system and system files, but Better Panel has not yet been tested
on an actual 24H2 installation. Compatibility with Windows 11 24H2 must not be
described as confirmed until that direct testing is completed.

Earlier Windows 11 releases are untested, Windows 11 26H1 is untested, and the
current build does not support ARM64 or Windows 10. Because Better Panel uses
Explorer's private WinUI Details-pane structure, Windows updates can require
mod changes even on an otherwise compatible Windows release.

Better Panel discovers the Details pane through Explorer lifecycle hooks
instead of occupying Explorer's single XAML Diagnostics connection. It can run
beside Windows 11 File Explorer Styler on the tested Windows 11 25H2 system.

## Better Detail Panel features

* Middle-click folder navigation through Explorer's native **Open in new tab**
  command.
* A custom Home panel with native Shell icons, devices, drives, capacity bars,
  available space, and persistent Recent/Favorites views.
* Direct drive navigation, detailed storage information, Disk Cleanup, Optimize
  Drives, and Windows Storage controls.
* Folder and multiple-selection analysis with sizes, counts, types, and modified
  dates.
* Compact native-style **Share**, **Open**, and **Open with** actions, plus
  aligned icon-only Favorites, Print, Delete, and Copy controls where supported.
* File-type icons, a single clean title row, and click-to-rename titles.
* Responsive image previews with inline expand and restore controls.
* Animated GIF previews that use the original file instead of a static Explorer
  thumbnail.
* An inline PDF viewer with multi-page navigation, sharp re-rendered zoom, and
  scrolling for enlarged pages.
* A compact **Print** action for PDFs, text, images, and any other selected file
  type with a print command registered in Windows.
* Inline preview and editing for TXT, Markdown, JSON, XML, YAML, INI, LOG, CSV,
  scripts, configuration files, and common source-code formats, with explicit
  Save, Cancel, and Reload controls.
* Native syntax highlighting with automatic language detection and a manual
  language selector for common code, markup, configuration, and script files.
* An audio player with artwork, play/pause, previous/next file, playback speed,
  repeat, shuffle, timeline, volume, mute, and elapsed/total time for MP3, OGG,
  Opus, FLAC, WAV, AAC/M4A, WMA, and other Windows-supported audio formats.
* A responsive video player with compact auto-hiding controls for MOV, MP4, M4V,
  MKV, AVI, WMV, WebM, MPEG, TS, and other Windows-supported video formats.
* Automatic media cleanup when the selection changes, the Details pane closes,
  the Explorer window closes, or the mod unloads.
* Archive extraction actions for ZIP, RAR, 7Z, TAR, GZ, CAB, ISO, and other
  common formats, using WinRAR, 7-Zip, WinZip, or Windows archive support.
* Exact archive choices for **Extract here**, **Extract to...**, and **Extract to
  "archive-name\\"**.
* Inline archive contents preview for ZIP, RAR, and 7Z before extraction.
* Cross-tab **Extract here** and **Move to** actions that target any open
  filesystem folder, selected destination subfolder, or drive root.
* Cross-tab Move labels show the source and destination, with an expandable
  list of every item waiting to be moved.
* Multi-selection support for moving files and folders through Windows' native
  progress, conflict, elevation, and undo handling.
* A dedicated multi-selection row with **Move to...**, recycle-aware **Delete**,
  and **Archive** using an installed WinRAR, 7-Zip, WinZip, or Windows tool.
* Destination transfer controls appear only in the destination folder tab, not
  on normal audio, video, image, archive, or other source-file panels.
* Text safety protections include encoding preservation, external-change
  detection, read-only handling, binary-content detection, and conservative
  preview/editing size limits.
* A replacement **Details** section with click-to-copy name, path, size,
  modified date, and SHA-256 values.
* Image EXIF details including dimensions, camera maker/model, date taken,
  rating, and GPS coordinates when the file contains those fields.
* Rich multi-selection summaries with file/folder counts, combined file size,
  file-type totals, and the selected items' modified-date range.
* A compact overlapping preview of representative Shell icons for multiple
  selections without repeating the same item type.
* A compact show/hide control for file previews.

Better Panel is maintained as its own package with its own identity, features,
settings, documentation, changelog, source, and compiled library.

## Recent changelog

### 2.2.0

* Added syntax highlighting with automatic language detection and a manual
  language selector.
* Added representative Shell-icon previews for multiple selections.
* Added clearer cross-tab Move labels and an expandable source-item list.
* Added drive roots as cross-tab Move destinations.
* Moved multiple-selection actions higher in the panel.
* Made Compact sizing consistent across icon buttons.
* Prevented stale previews and Open-button icons on unsupported file types.
* Added safer Details-pane recovery after sleep or hibernation.

### 2.1.2

* Added a saved text-preview size menu.
* Put temporary messages beside the controls they belong to.
* Added Compact, Comfort, and Custom button layouts.
* Made custom padding and animations consistent across panel sizes.

### 2.1.2-beta.3

* Added a circular loading indicator for slower systems.
* Added settings for button margins, padding, height, width, and icon size.
* Added optional hover and click animations.
* Settings update live and can be reset to default.
* Buttons now use Explorer's style.
* Home and the Details pane now load faster.

### 2.1.2-beta.2

* Added a native **Add to Favorites** and **Remove from Favorites** action. The
  star now reflects the real Windows Favorites state and turns yellow when the
  selected item is favorited.
* Added persistent **Recent** and **Favorites** views to the custom Home panel.
  The selected view is restored the next time the panel opens.
* Replaced generic Home file, folder, archive, and drive glyphs with icons
  supplied by the Windows Shell and the user's current file associations.
* Restyled **Devices and drives** with larger native drive artwork, compact
  spacing, clearer capacity bars, and a flatter classic Explorer layout.
* Removed the unavailable Share action from folders, added a recycle-aware
  icon-only Delete action for eligible files and folders, and protected main
  system and known folders from that action.
* Moved **Move to** and archive extraction actions directly below the primary
  buttons so they remain visible at the panel's normal height.
* Matched the Open, Open with, rename, expand, restore, Print, Delete, and Copy
  controls more closely to Explorer's native button styling and alignment.
* Replaced the separate rename pencil with click-to-edit title text and refined
  the editor sizing to prevent clipping.

### 2.0.0-beta.1

* Replaced Better Panel's active XAML Diagnostics connection with Explorer
  lifecycle and selection hooks from `FileExplorerExtensions.dll`.
* Added diagnostics-free Details-pane discovery through the live WinUI visual
  tree.
* Added direct association between each Better Panel instance and its owning
  Explorer window and active tab.
* Better Panel should now run alongside Windows 11 File Explorer Styler without
  a XAML Diagnostics conflict warning.
* Removed the requirement to disable other File Explorer mods solely because
  they use XAML Diagnostics.
* Removed the obsolete XAML Diagnostics compatibility setting.

### 1.15.10

* Added middle-click folder navigation, a custom Home panel, drive navigation,
  drive information and tools, and expanded folder/multiple-selection details.
* Replaced frequent polling with event-driven updates and added bounded,
  cache-backed folder analysis to improve responsiveness and resource use.
* Fixed folder totals, selection summaries, Home navigation and refresh issues,
  large-scan crashes, and unwanted native elements in drive views.
* Known limitations: some shell locations may not honor native new-tab actions;
  the native empty-selection banner can take about one second to disappear; and
  protected or very large folder totals can be incomplete.

### 1.13.2

* Fixed Backspace navigating Explorer while editing text. Explorer consumed the
  key as Back navigation before the injected editor, so Better Panel now routes
  it to the active editor first.

### 1.13.1

* Fixed video resizing during playback.

### 1.13.0

* Added a collapsible Details section with click-to-copy fields and Copy All.
* Added SHA-256 hashes and image EXIF details.
* Added folder and multiple-selection summaries.
* Added preview show/hide and compact audio playback.
* Added volume, mute, speed, repeat, shuffle, previous, and next controls.
* Added Fluent media icons.
* Added ZIP, RAR, and 7Z contents preview.
* Removed Explorer's duplicate Details section.
* Removed Copy path and 10-second back/forward controls.

### 1.12.0

* Added an inline PDF viewer with previous/next page navigation, page counts,
  scrolling, and 50–250% sharp re-rendered zoom.
* Added a Print action beside the existing file actions whenever Windows has a
  print handler registered for the selected file type.

### 1.11.0

* Added inline preview and editing for common text, configuration, markup,
  script, and source-code files.
* Added explicit Edit, Save, Cancel, and Reload controls with encoding
  and line-ending preservation plus external-change detection.
* Added safe 2 MB preview and 1 MB editing limits, binary-content detection,
  read-only handling, and paused selection refresh while editing.


### 1.10.1

* Reworked action and media controls to use native WinUI button proportions
  instead of circular custom shapes.
* Added consistent edge, title, text, media-card, and transfer-row padding.
* Standardized icon size, label spacing, and control heights for better
  alignment with Explorer's built-in Share and Properties buttons.

### 1.10.0

* Added dedicated multi-selection Move to, Delete, and Archive actions.
* Deferred selection-changing file operations outside XAML click callbacks and
  paused panel refreshes during operations to prevent Explorer crashes.
* Hid single-file title, media, extraction, and share controls while multiple
  items are selected.

### 1.9.1

* Limited cross-tab transfer controls to destination folder views and cleared
  stale transfer messages from normal file panels.

### 1.9.0

* Added cross-tab archive extraction and native multi-file/folder Move to.
* Added dynamic destination-folder and selected-subfolder detection.

### 1.8.2

* Replaced unreliable duplicated archive-provider menus with three consistent
  extraction choices and automatic extractor fallback.

### 1.8.0

* Added inline archive detection and extraction support for Windows, WinRAR,
  7-Zip, and WinZip-compatible formats.

### 1.7.5

* Added reliable animated GIF previews with responsive inline expansion.

### 1.7.4

* Stopped audio and video when changing files, closing Explorer, closing the
  Details pane, or unloading the mod.

### 1.7.2

* Made the video player responsive to Details-pane resizing and added compact,
  faster auto-hiding transport controls.

### 1.7.0

* Added Windows-codec video playback, file-type icons, a unified title layout,
  and inline file renaming.

## Compatibility

Better Panel 2.0 no longer occupies Explorer's XAML Diagnostics connection. It
uses Explorer lifecycle and selection hooks to locate and update the modern
Details pane, allowing Windows 11 File Explorer Styler to remain the process's
XAML Diagnostics consumer. Compatibility has been confirmed on the tested
Windows 11 25H2 system and remains beta on other Windows builds.

The first start after installing or updating this beta may take longer while
Windhawk resolves and caches Explorer's private symbols. If the panel loads
incompletely, continues showing old content, or does not react to selections,
allow Windhawk to finish processing and restart File Explorer. Restart Windows
if the problem remains. Manual cache deletion is not normally required and
should be used only when directed during troubleshooting.

## Attribution

Better Panel is a separate mod created by Nicole S. It is not a theme or styling
mod and does not include Windows 11 File Explorer Styler.
*/
// ==/WindhawkModReadme==

#include <xamlom.h>

#include <atomic>
#include <optional>
#include <vector>

#undef GetCurrentTime

#include <winrt/Microsoft.UI.Xaml.h>

struct ThemeTargetStyles {
    PCWSTR target;
    std::vector<PCWSTR> styles;
};

enum class BackgroundTranslucentEffect {
    kDefault,
    kAcrylic,
    kMica,
    kMicaAlt,
    kNone,
};

struct Theme {
    std::vector<ThemeTargetStyles> targetStyles;
    std::vector<PCWSTR> styleConstants;
    std::vector<PCWSTR> themeResourceVariables;
    int explorerFrameContainerHeight = 0;
    BackgroundTranslucentEffect backgroundTranslucentEffect =
        BackgroundTranslucentEffect::kDefault;
};

// clang-format off

const Theme g_themeTranslucent_Explorer11 = {{
    ThemeTargetStyles{L"Grid#CommandBarControlRootGrid", {
        L"Background=Transparent",
        L"BorderThickness=0,0,0,1",
        L"BorderBrush=#40A0A0A0"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas > Microsoft.UI.Xaml.Shapes.Path#SelectedBackgroundPath", {
        L"Fill=#40404040"}},
    ThemeTargetStyles{L"Grid#HomeViewRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Background:=<AcrylicBrush TintColor=\"#121212\" Opacity=\"0.3\"/>"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background=Transparent"}},
}, {}, {}, /*explorerFrameContainerHeight=*/0, BackgroundTranslucentEffect::kAcrylic};

const Theme g_themeMicaBar = {{
    ThemeTargetStyles{L"Grid#CommandBarControlRootGrid", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource LayerOnMicaBaseAltFillColorDefault}\"/>",
        L"BorderThickness=0,0,0,1"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent"}},
}};

const Theme g_themeNoCommandBar = {{
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.RowSpan=2",
        L"Margin=0,0,0,1"}},
}, {}, {}, /*explorerFrameContainerHeight=*/87};

const Theme g_themeMinimal_Explorer11 = {{
    ThemeTargetStyles{L"AppBarButton#backButton > Grid#Root@CommonStates > Border#AppBarButtonInnerBorder", {
        L"Background@Normal:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.07\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.12\"/>",
        L"Background@Pressed:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.12\"/>",
        L"Background@Disabled:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.05\"/>"}},
    ThemeTargetStyles{L"AppBarButton#forwardButton > Grid#Root@CommonStates > Border#AppBarButtonInnerBorder", {
        L"Background@Normal:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.05\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.12\"/>",
        L"Background@Pressed:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.12\"/>",
        L"Background@Disabled:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.05\"/>"}},
    ThemeTargetStyles{L"AppBarButton#refreshButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#upButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#BottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"FileExplorerExtensions.AddressBarControl > Grid#PART_LayoutRoot > Grid#NormalModeGrid", {
        L"BorderThickness=0,0,0,1",
        L"BorderBrush=#A0A0A0"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Margin=0,0,3,0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"CornerRadius=4",
        L"Margin=0,-3,0,3",
        L"Height=28"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.35\"/>",
        L"Background@PointerOverSelected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.35\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.13\"/>",
        L"Background@Normal:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.05\"/>",
        L"Background@PressedSelected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.35\"/>"}},
    ThemeTargetStyles{L"Grid#FileExplorerAddressBarGrid", {
        L"Grid.ColumnSpan=2",
        L"Margin=0,0,10,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#backButton > Grid#Root", {
        L"Padding=2"}},
    ThemeTargetStyles{L"AppBarButton#forwardButton > Grid#Root", {
        L"Padding=2"}},
    ThemeTargetStyles{L"AppBarButton#forwardButton > Grid#Root > Grid#ContentRoot > Viewbox#ContentViewbox", {
        L"Margin=9"}},
    ThemeTargetStyles{L"AppBarButton#backButton > Grid#Root > Grid#ContentRoot > Viewbox#ContentViewbox", {
        L"Margin=9"}},
    ThemeTargetStyles{L"Grid#PART_LayoutRoot", {
        L"MinHeight=28",
        L"Height=28"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Margin=0,0,20,4"}},
    ThemeTargetStyles{L"Border#ScrollIncreaseButtonContainer", {
        L"Margin=0,0,0,4"}},
    ThemeTargetStyles{L"Border#ScrollDecreaseButtonContainer", {
        L"Margin=0,0,0,4"}},
    ThemeTargetStyles{L"Grid#FileExplorerAddressBarGrid", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl#NavigationBarControl", {
        L"Grid.Row=0",
        L"Grid.RowSpan=2"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FileExplorerTabControl", {
        L"Margin=100,0,0,-15",
        L"Grid.RowSpan=2"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl > Grid#NavigationBarControlGrid", {
        L"Margin=0,0,0,-18",
        L"Background=Transparent",
        L"Width=100",
        L"HorizontalAlignment=0"}},
}, {}, {}, /*explorerFrameContainerHeight=*/42};

const Theme g_themeTabless = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#CommandBarControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#ContentRoot", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.Row=$NavigationBarGrid"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl", {
        L"Grid.Row=$CommandBarGrid"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainerGrid > Border", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer > Microsoft.UI.Xaml.Controls.Button#CloseButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TabViewItem > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.Canvas", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ContentPresenter > Microsoft.UI.Xaml.Controls.StackPanel > Microsoft.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe UI, Segoe Fluent Icons",
        L"FontWeight=Normal"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#CommandBarControlRootGrid", {
        L"BorderThickness=0,0,0,1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FileExplorerTabControl", {
        L"Height=36"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"Padding=1,0,0,1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox#IconBox", {
        L"Margin=0,0,4,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TabViewItem", {
        L"Margin=0,-8,0,0"}},
}, {
    L"NavigationBarGrid=2",
    L"CommandBarGrid=1",
}};

const Theme g_themeMatter = {{
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent",
        L"HorizontalAlignment  = 1"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerSecondaryCommandBar", {
        L"Background=Transparent",
        L"Visibility = 1"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Margin=0,0,4,0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"CornerRadius=5",
        L"Margin=2,4,0,4",
        L"Height=29"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background = Transparent",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:= $accentColor2",
        L"Background@PointerOverSelected:= $accentColor",
        L"Background@PointerOver:= $accentColor2",
        L"Background@Normal=$accentColor",
        L"Background@PressedSelected:=$accentColor2",
        L"Background@Pressed := $accentColor2"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility  = 0",
        L"Margin = 0,0,0,3"}},
    ThemeTargetStyles{L"Grid#CommandBarControlRootGrid", {
        L"Background=Transparent",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#PART_LayoutRoot", {
        L"Background :=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"0.4\" />",
        L"CornerRadius = 6",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl", {
        L"Margin = 0,-5,0,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox > Grid#LayoutRoot > TextBox > Grid@CommonStates > Border#BorderElement", {
        L"Background :=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"0.4\" />",
        L"CornerRadius = 6",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Cut]", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Copy]", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Paste]", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Rename]", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Share]", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarSeparator", {
        L"Visibility  = 1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#ScrollDecreaseButtonContainer", {
        L"Margin = 0,0,0,3"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#ScrollIncreaseButtonContainer", {
        L"Margin = 0,0,0,3"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton", {
        L"Visibility  =1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#upButton", {
        L"Visibility  =1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton", {
        L"Visibility  =1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton", {
        L"Visibility  =1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton[ToolTipService.ToolTip = Create a new item in the current location.]", {
        L"Visibility  = 1"}},
}, {
    L"accentColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />",
    L"accentColor2=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"0.5\" />",
}};

const Theme g_themeWindowGlass = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#PART_LayoutRoot", {
        L"Background=Transparent",
        L"RenderTransform:=<TranslateTransform X=\"0\"/>"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FirstCrumbStackPanelControl#FirstCrumbStackPanel", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootCommandSearchGrid > Windows.UI.Xaml.Controls.Border#BorderElement", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot", {
        L"BorderThickness=$BorderThickness",
        L"Background:=$ButtonBackground",
        L"BorderBrush:=$ButtonBorder"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#CommandBarControlRootGrid", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerCommandBar", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />",
        L"HorizontalAlignment=Center",
        L"Margin=-4",
        L"Padding=10"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerSecondaryCommandBar", {
        L"RenderTransform:=<TranslateTransform X=\"Auto\" />",
        L"HorizontalAlignment=Center",
        L"Margin=-4",
        L"Padding=10",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerCommandBar > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.Grid#ContentRoot", {
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush=Transparent",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerSecondaryCommandBar > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.Grid#ContentRoot", {
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background=#10808080",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#NavigationBarControlGrid", {
        L"Background=Transparent",
        L"BorderBrush=Transparent",
        L"ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"*\"/><ColumnDefinition Width=\"430\"/></ColumnDefinitionCollection>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#HomeViewRootGrid", {
        L"BorderBrush:=$MainContentBG",
        L"CornerRadius=8",
        L"BorderThickness=0",
        L"Margin=0,0,8,8",
        L"Background:=$MainContentBG"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"BorderBrush:=$MainContentBG",
        L"CornerRadius=8",
        L"BorderThickness=0",
        L"Margin=0,0,8,8",
        L"Background:=$MainContentBG"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid > Grid#GalleryRootGrid", {
        L"Background:=$MainContentBG"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Background:=$Background"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"CornerRadius=8",
        L"Margin=5",
        L"Height=35"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.10\"/>",
        L"Background@PointerOverSelected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.10\"/>",
        L"Background@PointerOver:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.13\"/>",
        L"Background@Normal:=<AcrylicBrush TintColor=\"Transparent\" Opacity=\"0.05\"/>",
        L"Background@PressedSelected:=<SolidColorBrush Color=\"#808080\" Opacity=\"0.10\"/>"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#BottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Shapes.Path#LeftRadiusRenderArc", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Shapes.Path#RightRadiusRenderArc", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox#IconBox", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.CommandBarFlyoutCommandBar > Grid#LayoutRoot > Grid#OuterContentRoot > Grid#ContentRoot > Grid#PrimaryItemsRoot", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Margin=0,0,0,-5",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Grid#OuterOverflowContentRootV2 > Grid#OverflowContentRoot > CommandBarOverflowPresenter#SecondaryItemsControl > Grid#LayoutRoot", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"CommandBarOverflowPresenter#SecondaryItemsControl > Grid#LayoutRoot", {
        L"Background:=$Background",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AutoSuggestBox#FileExplorerSearchBox > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.TextBox#TextBox", {
        L"CornerRadius=$CornerRadius",
        L"Margin=0,0,180,0",
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"MaxWidth=750",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AutoSuggestBox#PART_AutoSuggestBox > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.TextBox#TextBox", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#NavigationCommands", {
        L"Margin=180,0,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#RootContainer", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border > Microsoft.UI.Xaml.Controls.Button#AddButton", {
        L"RenderTransform:=<TranslateTransform Y=\"-6\" />"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TextBlock#TextLabel", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#SubItemChevronPanel > Microsoft.UI.Xaml.Controls.FontIcon#SubItemChevron", {
        L"RenderTransform:=<TranslateTransform X=\"-5\" Y=\"12\" />"}},
}, {
    L"Background=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#15323232\"/>",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"{ThemeResource SystemChromeHighColor}\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SystemChromeLowColor}\" Offset=\"0.15\" /><GradientStop Color=\"{ThemeResource SystemChromeHighColor}\" Offset=\"0.95\" /></LinearGradientBrush>",
    L"BorderThickness=0.3,1,0.3,0.3",
    L"ButtonBackground=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" Opacity=\"1\" />",
    L"ButtonBorder=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\" Opacity=\"1\" />",
    L"CornerRadius=8",
    L"Background2=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0\" />",
    L"MainContentBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"1\" />",
}, {}, /*explorerFrameContainerHeight=*/0, BackgroundTranslucentEffect::kAcrylic};

const Theme g_themeAddressSearchOnly = {{
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.Row=0",
        L"Background=Transparent",
        L"MinHeight=48",
        L"Margin=0,26,0,1"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FileExplorerTabControl", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#refreshButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#upButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#backButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AppBarButton#forwardButton", {
        L"Visibility=Collapsed"}},
}, {}, {}, /*explorerFrameContainerHeight=*/80};

const Theme g_themeTintedGlass = {{
    ThemeTargetStyles{L"Grid#CommandBarControlRootGrid", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"BorderBrush=$CommonBgBrush"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas > Microsoft.UI.Xaml.Shapes.Path#SelectedBackgroundPath", {
        L"Fill:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Grid#HomeViewRootGrid", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background:=$CommonBgBrush"}},
}, {
    L"CommonBgBrush=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#80000000\"/>",
}, {}, /*explorerFrameContainerHeight=*/0, BackgroundTranslucentEffect::kAcrylic};

const Theme g_themeLiquidGlass = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#PART_LayoutRoot", {
        L"Background=Transparent",
        L"HorizontalAlignment=Stretch"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FirstCrumbStackPanelControl#FirstCrumbStackPanel", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#RootCommandSearchGrid > Windows.UI.Xaml.Controls.Border#BorderElement", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.NavigationViewItemPresenter#NavigationViewItemPresenter > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot", {
        L"BorderThickness=$ElementBorderThickness",
        L"Background:=$ElementBackground",
        L"BorderBrush:=$ElementBorder",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#NavigationBarControlGrid", {
        L"Background:=Transparent",
        L"BorderBrush:=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#HomeViewRootGrid", {
        L"BorderBrush:=$ElementBorderBrush",
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"Margin=4,0"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"BorderBrush:=$ElementBorderBrush",
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"Margin=4,0"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid > Grid#GalleryRootGrid", {
        L"Background:=Transparent"}},
    ThemeTargetStyles{L"ToolTip", {
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"Margin=5",
        L"Height=35",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius",
        L"BorderBrush:=$ElementBorderBrush"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=$ElementBackground",
        L"Background@PointerOverSelected:=$AccentBackground",
        L"Background@PointerOver:=$AccentBackground",
        L"Background@Normal:=$ElementBackground",
        L"Background@PressedSelected:=$ButtonBackground2"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#BottomBorderLine", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Shapes.Path#LeftRadiusRenderArc", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Shapes.Path#RightRadiusRenderArc", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"Visibility=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox#IconBox", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"CommandBarOverflowPresenter#SecondaryItemsControl > Grid#LayoutRoot", {
        L"Background:=$ElementBackground",
        L"BorderThickness=$ElementBorderThickness",
        L"BorderBrush:=$ElementBorderBrush",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AutoSuggestBox#FileExplorerSearchBox > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.TextBox#TextBox", {
        L"CornerRadius=$ElementCornerRadius",
        L"Background:=$ElementBackground",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"CornerRadius=$ElementCornerRadius",
        L"Background:=$ElementBackground",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AutoSuggestBox#PART_AutoSuggestBox > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.TextBox#TextBox", {
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#RootContainer", {
        L"Background:=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border > Microsoft.UI.Xaml.Controls.Button#AddButton", {
        L"RenderTransform:=<TranslateTransform Y=\"-8\" />"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TextBlock#TextLabel", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#SubItemChevronPanel > Microsoft.UI.Xaml.Controls.FontIcon#SubItemChevron", {
        L"RenderTransform:=<TranslateTransform X=\"-5\" Y=\"12\" />"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"Height = 28"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.RowSpan=2",
        L"Margin=0,0,0,1"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Background:=Transparent"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail", {
        L"Background:=Transparent"}},
}, {
    L"ContentBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"1\" />",
    L"Background=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\" />",
    L"ElementBackground=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.4\" />",
    L"ElementBackground2=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\" />",
    L"AccentBackground=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAccentColorLight1}\" TintOpacity=\"0.2\" />",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"BorderThickness=0.3,1,0.3,0.3",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"CornerRadius=12",
    L"ElementCornerRadius=8",
}, {}, /*explorerFrameContainerHeight=*/87};

const Theme g_themeMicaTabless = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#CommandBarControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#ContentRoot", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.Row=$NavigationBarGrid"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl", {
        L"Grid.Row=$CommandBarGrid"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainerGrid > Border", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer > Microsoft.UI.Xaml.Controls.Button#CloseButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TabViewItem > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.Canvas", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ContentPresenter > Microsoft.UI.Xaml.Controls.StackPanel > Microsoft.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe UI, Segoe Fluent Icons",
        L"FontWeight=Normal"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#CommandBarControlRootGrid", {
        L"BorderThickness=0,0,0,1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FileExplorerTabControl", {
        L"Height=36"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#TabContainer", {
        L"Padding=1,0,0,1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox#IconBox", {
        L"Margin=0,0,4,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.TabViewItem", {
        L"Margin=0,-8,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#DetailsViewControlRootGrid", {
        L"Background=Transparent"}},
}, {
    L"NavigationBarGrid=1",
    L"CommandBarGrid=2",
}};

const Theme g_themeOS26_Liquid_Glass = {{
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Margin=20,20,20,1",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#HomeViewRootGrid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Margin=10",
        L"Background:=transparent",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=12",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=2,6,2,6",
        L"Padding@Disabled=0,-7",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#upButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerSecondaryCommandBar", {
        L"Background=Transparent",
        L"MinHeight=0"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Margin=0,0,8,0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"CornerRadius=12",
        L"Margin=2,4,0,4",
        L"Height=27",
        L"BorderThickness=1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\" />",
        L"Background@PointerOverSelected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#35ffffff\" />",
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#15ffffff\" />"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility=Visible",
        L"Margin=0,0,0,2",
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"BorderBrush:=<LinearGradientBrush EndPoint=\"1,1\" StartPoint=\"0,0\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\"/><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\"/><GradientStop Color=\"#80ffffff\" Offset=\"1\"/></LinearGradientBrush>",
        L"Width=24",
        L"Height=24"}},
    ThemeTargetStyles{L"Grid#CommandBarControlRootGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"Margin=-6,0,0,0"}},
    ThemeTargetStyles{L"Grid#PART_LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\" />",
        L"CornerRadius=14",
        L"BorderThickness=1",
        L"Margin=2",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox > Grid#LayoutRoot > TextBox > Grid@CommonStates > Border#BorderElement", {
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\" />",
        L"CornerRadius=14",
        L"BorderThickness=1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\" />",
        L"CornerRadius=12",
        L"BorderThickness=1",
        L"Margin=3,0,3,1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarToggleButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\" />",
        L"CornerRadius=12",
        L"BorderThickness=1",
        L"Margin=3,0,3,1",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#OuterOverflowContentRootV2", {
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Button#MoreButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarSeparator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton", {
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton", {
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#upButton", {
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton", {
        L"Margin=0,9,9,0"}},
}};

const Theme g_themeOS26_Liquid_Glass_variant_Compact = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.SuggestionsPopup", {
        L"Margin=0,0,0,900"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton > Grid@CommonStates", {
        L"Background@Disabled:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius@Disabled=12",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=2,6,2,6",
        L"Padding@Disabled=0,-7"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > Button#MoreButton", {
        L"Background:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=12",
        L"BorderThickness=1",
        L"Margin=3,2,3,2",
        L"Width=45",
        L"Height=32"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton > Grid@CommonStates", {
        L"Background@Disabled:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton > Grid@CommonStates", {
        L"Background@Disabled:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton > Grid@CommonStates", {
        L"Background@Disabled:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarToggleButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#2D101010\"/>"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Margin=20,20,20,1",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid > OuterOverflowContentRootV2", {
        L"CornerRadius=250"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBarOverflowPresenter > Microsoft.UI.Xaml.Controls.CommandBarOverflowPresenter", {
        L"Background=transparent"}},
    ThemeTargetStyles{L"AppBarButton[7]", {
        L"Visibility=Collapsed",
        L"Width=0",
        L"MinWidth=0",
        L"Margin=0,0,0,0",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox > ContentViewB", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#HomeViewRootGrid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Margin=10",
        L"Background:=transparent",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Grid.Row=0",
        L"Grid.RowSpan=1",
        L"CornerRadius:=15",
        L"Width=400",
        L"HorizontalAlignment=Left",
        L"Background:=transparent",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > Grid#OverflowSeparator", {
        L"Visibility=Collapsed",
        L"Width=0",
        L"MinWidth=0"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot", {
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > ItemsControl#PrimaryItemsControl", {
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerSecondaryCommandBar", {
        L"Visibility=Visible",
        L"Margin=0,40,0,-20"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid", {
        L"Margin=370,1,0,1"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Width=150",
        L"Height=40",
        L"Margin=0,0,8,0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=2,2,0,2",
        L"Height=35"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#20ffffff\"/>",
        L"Background@PointerOverSelected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\"/>",
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#15ffffff\"/>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility=Visible",
        L"Margin=0,0,0,4",
        L"Background:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=8",
        L"BorderThickness=1",
        L"BorderBrush:=<LinearGradientBrush EndPoint=\"1,1\" StartPoint=\"0,0\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\"/><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\"/><GradientStop Color=\"#80ffffff\" Offset=\"1\"/></LinearGradientBrush>",
        L"Width=24",
        L"Height=24"}},
    ThemeTargetStyles{L"Grid#CommandBarControlRootGrid", {
        L"Background:=",
        L"BorderBrush:="}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#PART_LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#15ffffff\"/>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=2"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl", {
        L"Grid.Row=0",
        L"Grid.RowSpan=2",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox > Grid#LayoutRoot > TextBox > Grid@CommonStates", {
        L"BorderThickness=1",
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#15ffffff\"/>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=10",
        L"Margin=-90,0,90,0",
        L"Height=32"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"Margin=-8,0,90,0"}},
    ThemeTargetStyles{L"CommandBarOverflowPresenter Microsoft.UI.Xaml.Controls.AppBarButton", {
        L"Background=Transparent",
        L"CornerRadius=8",
        L"Margin=2,1,2,1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton", {
        L"Background:=<LinearGradientBrush StartPoint=\"-0.3,-0.3\" EndPoint=\"1.3,1.3\"><GradientStop Color=\"#55f0f07d\" Offset=\"0.0\"/><GradientStop Color=\"#2AF0F0F0\" Offset=\"0.3\"/><GradientStop Color=\"#00F0F0F0\" Offset=\"0.6\"/></LinearGradientBrush>",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"1,1\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\" /><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\" /><GradientStop Color=\"#80ffffff\" Offset=\"1\" /></LinearGradientBrush>",
        L"CornerRadius=12",
        L"BorderThickness=1",
        L"Margin=3,2,3,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#OuterOverflowContentRootV2", {
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarToggleButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#2D101010\"/>",
        L"CornerRadius=8",
        L"BorderThickness=1",
        L"Margin=3,0,3,1",
        L"BorderBrush:=<LinearGradientBrush EndPoint=\"1,1\" StartPoint=\"0,0\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\"/><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\"/><GradientStop Color=\"#80ffffff\" Offset=\"1\"/></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarSeparator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#upButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton", {
        L"Visibility=Visible",
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#stopButton", {
        L"Visibility=Collapsed",
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.RowSpan=2"}},
}, {}, {}, /*explorerFrameContainerHeight=*/87};

const Theme g_themeZEUSosX_044 = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#CommandBarControlRootGrid", {
        L"Background=Transparent",
        L"BorderThickness=0",
        L"Grid.Row=0",
        L"Grid.RowSpan=2",
        L"HorizontalAlignment=Left",
        L"VerticalAlignment=Top",
        L"Width=155",
        L"Margin=197,-30,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerCommandBar", {
        L"Background=Transparent",
        L"HorizontalAlignment=Left",
        L"VerticalAlignment=Top"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Border#BottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#NavigationBarControlGrid", {
        L"Background=Transparent",
        L"BorderBrush=Transparent",
        L"ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"*\"/><ColumnDefinition Width=\"380\"/></ColumnDefinitionCollection>",
        L"Margin=0,-16,0,-21"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid", {
        L"Grid.Row=0",
        L"HorizontalAlignment=Left",
        L"Margin=100,0,0,0",
        L"Width=1",
        L"MaxWidth=1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.FileExplorerTabControl", {
        L"HorizontalAlignment=Left",
        L"Margin=100,0,0,0",
        L"Width=1",
        L"MaxWidth=1"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Width=0",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox > Grid#LayoutRoot > TextBox > Grid@CommonStates > Border#BorderElement", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid > Grid#LayoutRoot > TextBox > Grid@CommonStates > Border#BorderElement", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AutoSuggestBox#FileExplorerSearchBox > Microsoft.UI.Xaml.Controls.Grid#LayoutRoot > Microsoft.UI.Xaml.Controls.TextBox#TextBox", {
        L"Margin=0,0,140,0",
        L"Background=Transparent",
        L"BorderBrush=Transparent",
        L"TextAlignment=Center",
        L"HorizontalContentAlignment=Center"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"HorizontalAlignment=Stretch",
        L"Height=28",
        L"Margin=155,0,0,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox", {
        L"HorizontalAlignment=Stretch",
        L"Height=28",
        L"Margin=-7,-1,7,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBar#FileExplorerCommandBar Button", {
        L"FontSize=14"}},
}, {}, {}, /*explorerFrameContainerHeight=*/44, BackgroundTranslucentEffect::kMica};

const Theme g_themeCompact_Explorer11 = {{
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Primitives.SuggestionsPopup", {
        L"Margin=0,0,0,900"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=10",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=2,6,2,6",
        L"Padding@Disabled=0,-7"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > Button#MoreButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=3,2,3,2",
        L"Width=45",
        L"Height=32"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius@Disabled=11",
        L"BorderThickness@Disabled=1",
        L"Margin@Disabled=0,0,0,0",
        L"Height@Disabled=32",
        L"Width@Disabled=20",
        L"Padding@Disabled=0,-2,0,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarToggleButton > Grid@CommonStates", {
        L"Background@Disabled:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#2D101010\"/>"}},
    ThemeTargetStyles{L"Grid#DetailsViewControlRootGrid", {
        L"Margin=20,20,20,1",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"StackPanel#DetailsViewThumbnail > Grid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid > OuterOverflowContentRootV2", {
        L"CornerRadius=250"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.CommandBarOverflowPresenter > Microsoft.UI.Xaml.Controls.CommandBarOverflowPresenter", {
        L"Background=transparent"}},
    ThemeTargetStyles{L"AppBarButton[7]", {
        L"Visibility=Collapsed",
        L"Width=0",
        L"MinWidth=0",
        L"Margin=0,0,0,0",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Viewbox > ContentViewB", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#HomeViewRootGrid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"FileExplorerExtensions.GalleryViewControl#GalleryViewControl > Grid", {
        L"Margin=20,20,20,0",
        L"Background:=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#2D101010\" TintOpacity=\"0.4\"/>",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#GalleryRootGrid", {
        L"Margin=10",
        L"Background:=transparent",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar", {
        L"Grid.Row=0",
        L"Grid.RowSpan=1",
        L"CornerRadius:=15",
        L"Width=400",
        L"HorizontalAlignment=Left",
        L"Background:=transparent",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > Grid#OverflowSeparator", {
        L"Visibility=Collapsed",
        L"Width=0",
        L"MinWidth=0"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot", {
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerCommandBar > Grid#LayoutRoot > Grid#ContentRoot > ItemsControl#PrimaryItemsControl", {
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"CommandBar#FileExplorerSecondaryCommandBar", {
        L"Visibility=Visible",
        L"Margin=0,40,0,-20"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid", {
        L"Margin=370,1,0,1"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#LeftBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border#RightBottomBorderLine", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem", {
        L"Width=150",
        L"Height=40",
        L"Margin=0,0,8,0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=2,2,0,2",
        L"Height=35"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Canvas", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot > Grid#TabContainer", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"TabViewItem > Grid#LayoutRoot@CommonStates", {
        L"Background@Selected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#30ffffff\"/>",
        L"Background@PointerOverSelected:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#40ffffff\"/>",
        L"Background@Normal:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#20ffffff\"/>"}},
    ThemeTargetStyles{L"Grid#TabContainerGrid > Border > Button#AddButton", {
        L"Visibility=Visible",
        L"Margin=0,0,0,4",
        L"Background:=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=0",
        L"BorderBrush:=<LinearGradientBrush EndPoint=\"1,1\" StartPoint=\"0,0\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\"/><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\"/><GradientStop Color=\"#80ffffff\" Offset=\"1\"/></LinearGradientBrush>",
        L"Width=24",
        L"Height=24"}},
    ThemeTargetStyles{L"Grid#CommandBarControlRootGrid", {
        L"Background:=",
        L"BorderBrush:="}},
    ThemeTargetStyles{L"Grid#NavigationBarControlGrid", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Grid#PART_LayoutRoot", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=1"}},
    ThemeTargetStyles{L"FileExplorerExtensions.CommandBarControl", {
        L"Grid.Row=0",
        L"Grid.RowSpan=2",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"AutoSuggestBox#FileExplorerSearchBox > Grid#LayoutRoot > TextBox > Grid@CommonStates", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"Margin=-90,0,90,0",
        L"Height=30"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.Grid#FileExplorerAddressBarGrid", {
        L"Margin=-8,0,90,0"}},
    ThemeTargetStyles{L"CommandBarOverflowPresenter#SecondaryItemsControl > Microsoft.UI.Xaml.Controls.AppBarButton", {
        L"Background=Transparent",
        L"CornerRadius=4",
        L"BorderThickness=0",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#25ffffff\"/>",
        L"CornerRadius=10",
        L"BorderThickness=1",
        L"Margin=3,2,3,2"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarToggleButton", {
        L"Background:=<WindhawkBlur BlurAmount=\"8\" TintColor=\"#2D101010\"/>",
        L"CornerRadius=8",
        L"BorderThickness=1",
        L"Margin=3,0,3,1",
        L"BorderBrush:=<LinearGradientBrush EndPoint=\"1,1\" StartPoint=\"0,0\"><GradientStop Color=\"#80ffffff\" Offset=\"0.0\"/><GradientStop Color=\"{ThemeResource SurfaceStrokeColorDefault}\" Offset=\"0.55\"/><GradientStop Color=\"#80ffffff\" Offset=\"1\"/></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarSeparator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#backButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#forwardButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#upButton", {
        L"Margin=0,9,9,0",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#refreshButton", {
        L"Visibility=Visible",
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AppBarButton#stopButton", {
        L"Visibility=Collapsed",
        L"Margin=0,9,9,0"}},
    ThemeTargetStyles{L"FileExplorerExtensions.NavigationBarControl", {
        L"Grid.RowSpan=2"}},
}, {}, {}, /*explorerFrameContainerHeight=*/87};

// clang-format on

enum class BackgroundTranslucentEffectRegion {
    kExplorerFrame,
    kEntireWindow,
};

enum class XamlDiagnosticsHandling {
    kAlert,
    kBlock,
    kAllow,
};

enum class BetterPanelHoverAnimation {
    WindowsDefault,
    None,
    Scale,
    Lift,
    Tilt,
    Spring,
};

enum class BetterPanelClickAnimation {
    WindowsDefault,
    None,
    Press,
    Pulse,
    Bounce,
    Flash,
    Shake,
    Spring,
    Spin,
    Squash,
    Wobble,
};

enum class BetterPanelButtonLayoutMode {
    Compact,
    Comfort,
    Custom,
};

struct {
    std::optional<BackgroundTranslucentEffect> backgroundTranslucentEffect;
    BackgroundTranslucentEffectRegion backgroundTranslucentEffectRegion;
    int explorerFrameContainerHeight;
    XamlDiagnosticsHandling xamlDiagnosticsHandling;
    BetterPanelButtonLayoutMode buttonLayoutMode =
        BetterPanelButtonLayoutMode::Compact;
    int buttonHorizontalMargin = 0;
    int buttonVerticalMargin = 0;
    int buttonHorizontalPadding = 10;
    int buttonVerticalPadding = 4;
    int buttonMinimumWidth = 0;
    int buttonMinimumHeight = 32;
    int iconButtonSize = 32;
    BetterPanelHoverAnimation hoverAnimation =
        BetterPanelHoverAnimation::WindowsDefault;
    BetterPanelClickAnimation clickAnimation =
        BetterPanelClickAnimation::WindowsDefault;
} g_settings;

BackgroundTranslucentEffect g_themeBackgroundTranslucentEffect;
int g_themeExplorerFrameContainerHeight;

std::atomic<bool> g_initialized;
thread_local bool g_initializedForThread;

void ApplyCustomizations(InstanceHandle handle,
                         winrt::Microsoft::UI::Xaml::FrameworkElement element,
                         PCWSTR fallbackClassName);
void CleanupCustomizations(InstanceHandle handle);
void BetterPanelSuppressNativeElementOnAdd(
    winrt::Microsoft::UI::Xaml::FrameworkElement const& element);
void BetterPanelScheduleCurrentThreadDiscovery();

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
    }
    namespace Microsoft {
        namespace UI::Xaml {}
    }
}

// alias some long namespaces for convenience
namespace wf = winrt::Windows::Foundation;
namespace mux = winrt::Microsoft::UI::Xaml;

#pragma endregion  // winrt_hpp

#pragma region visualtreewatcher_hpp

#include <winrt/Microsoft.UI.Xaml.h>

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

    if (!g_initializedForThread)
    {
        Wh_Log(L"Not initialized for thread %u", GetCurrentThreadId());
        return S_OK;
    }

    if (mutationType == Add)
    {
        const auto inspectable = FromHandle(element.Handle);
        auto frameworkElement = inspectable.try_as<mux::FrameworkElement>();
        if (frameworkElement)
        {
            Wh_Log(L"FrameworkElement name: %s", frameworkElement.Name().c_str());
            BetterPanelSuppressNativeElementOnAdd(frameworkElement);
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

bool g_inInjectWindhawkTAP = false;

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

    const HMODULE wux(GetModuleHandle(L"Microsoft.Internal.FrameworkUdk.dll"));
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
    g_inInjectWindhawkTAP = true;

    HRESULT hr;
    for (int i = 0; i < 10000; i++)
    {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"WinUIVisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), L"", location, CLSID_WindhawkTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            break;
        }
    }

    g_inInjectWindhawkTAP = false;

    return hr;
}

#pragma endregion  // api_cpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

#include <windhawk_utils.h>

#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

using namespace std::string_view_literals;

#include <initguid.h>

#include <commctrl.h>
#include <bcrypt.h>
#include <d2d1_1.h>
#include <dwmapi.h>
#include <roapi.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <windows.graphics.effects.h>
#include <winstring.h>

#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Documents.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.Playback.h>
#include <winrt/Windows.Data.Pdf.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.ViewManagement.h>

using namespace winrt::Microsoft::UI::Xaml;

namespace wge = winrt::Windows::Graphics::Effects;
namespace muc = winrt::Microsoft::UI::Composition;
namespace muxh = mux::Hosting;
namespace awge = ABI::Windows::Graphics::Effects;

////////////////////////////////////////////////////////////////////////////////
// Better Detail Panel additions.

#if 0  // Version 0.1 prototype retained for reference.

namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace mud = winrt::Microsoft::UI::Dispatching;

#define CWM_GETISHELLBROWSER (WM_USER + 7)

struct BetterPanelEntry {
    mud::DispatcherQueue dispatcher{nullptr};
    winrt::weak_ref<muxc::StackPanel> host;
    winrt::weak_ref<FrameworkElement> panel;
};

std::mutex g_betterPanelMutex;
std::vector<BetterPanelEntry> g_betterPanels;
std::mutex g_betterMediaMutex;
std::wstring g_betterMediaPath;

std::wstring BetterPanelExtractPath(IShellBrowser* shellBrowser) {
    if (!shellBrowser) {
        return {};
    }

    winrt::com_ptr<IShellView> shellView;
    if (FAILED(shellBrowser->QueryActiveShellView(shellView.put())) ||
        !shellView) {
        return {};
    }

    winrt::com_ptr<IFolderView2> folderView;
    if (FAILED(shellView->QueryInterface(IID_PPV_ARGS(folderView.put()))) ||
        !folderView) {
        return {};
    }

    winrt::com_ptr<IShellItemArray> selection;
    if (FAILED(folderView->GetSelection(FALSE, selection.put())) ||
        !selection) {
        return {};
    }

    DWORD count = 0;
    if (FAILED(selection->GetCount(&count)) || count != 1) {
        return {};
    }

    winrt::com_ptr<IShellItem> item;
    if (FAILED(selection->GetItemAt(0, item.put())) || !item) {
        return {};
    }

    PWSTR rawPath = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &rawPath)) ||
        !rawPath) {
        return {};
    }

    std::wstring path(rawPath);
    CoTaskMemFree(rawPath);
    return path;
}

IShellBrowser* BetterPanelGetShellBrowser(HWND window) {
    if (!window) {
        return nullptr;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId != GetCurrentProcessId()) {
        return nullptr;
    }

    WCHAR className[64]{};
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        (_wcsicmp(className, L"CabinetWClass") != 0 &&
         _wcsicmp(className, L"ShellTabWindowClass") != 0)) {
        return nullptr;
    }

    return reinterpret_cast<IShellBrowser*>(
        SendMessageW(window, CWM_GETISHELLBROWSER, 0, 0));
}

std::wstring BetterPanelGetSelectedPath() {
    HWND focus = nullptr;
    GUITHREADINFO threadInfo{sizeof(threadInfo)};
    if (GetGUIThreadInfo(GetCurrentThreadId(), &threadInfo)) {
        focus = threadInfo.hwndFocus ? threadInfo.hwndFocus
                                     : threadInfo.hwndActive;
    }
    if (!focus) {
        focus = GetForegroundWindow();
    }
    if (!focus) {
        return {};
    }

    for (HWND window = focus; window; window = GetParent(window)) {
        if (auto browser = BetterPanelGetShellBrowser(window)) {
            auto path = BetterPanelExtractPath(browser);
            if (!path.empty()) {
                return path;
            }
        }
    }

    HWND root = GetAncestor(focus, GA_ROOT);
    for (HWND tab = nullptr; root &&
         (tab = FindWindowExW(root, tab, L"ShellTabWindowClass", nullptr));) {
        if (auto browser = BetterPanelGetShellBrowser(tab)) {
            auto path = BetterPanelExtractPath(browser);
            if (!path.empty()) {
                return path;
            }
        }
    }

    return {};
}

void BetterPanelSetStatus(winrt::weak_ref<muxc::TextBlock> weakStatus,
                          std::wstring_view text) {
    if (auto status = weakStatus.get()) {
        status.Text(text);
    }
}

std::wstring BetterPanelGetMciError(MCIERROR error) {
    WCHAR message[256]{};
    if (error && mciGetErrorStringW(error, message, ARRAYSIZE(message))) {
        return message;
    }
    return error ? L"Media playback failed" : L"";
}

void BetterPanelCloseMedia() {
    std::lock_guard lock(g_betterMediaMutex);
    mciSendStringW(L"close BetterDetailPanelMedia", nullptr, 0, nullptr);
    g_betterMediaPath.clear();
}

std::wstring BetterPanelPlayOrPause(const std::wstring& path) {
    std::lock_guard lock(g_betterMediaMutex);

    auto extension = path.substr(path.find_last_of(L'.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   towlower);
    if (extension != L"mp3") {
        return L"Select one MP3 file first";
    }

    if (path == g_betterMediaPath) {
        WCHAR mode[32]{};
        mciSendStringW(L"status BetterDetailPanelMedia mode", mode,
                       ARRAYSIZE(mode), nullptr);
        if (_wcsicmp(mode, L"playing") == 0) {
            MCIERROR error = mciSendStringW(
                L"pause BetterDetailPanelMedia", nullptr, 0, nullptr);
            return error ? BetterPanelGetMciError(error) : L"Paused";
        }
        if (_wcsicmp(mode, L"paused") == 0 ||
            _wcsicmp(mode, L"stopped") == 0) {
            MCIERROR error = mciSendStringW(
                L"play BetterDetailPanelMedia", nullptr, 0, nullptr);
            return error ? BetterPanelGetMciError(error) : L"Playing";
        }
    }

    mciSendStringW(L"close BetterDetailPanelMedia", nullptr, 0, nullptr);
    std::wstring command = L"open \"" + path +
                           L"\" type mpegvideo alias BetterDetailPanelMedia";
    MCIERROR error = mciSendStringW(command.c_str(), nullptr, 0, nullptr);
    if (error) {
        g_betterMediaPath.clear();
        return BetterPanelGetMciError(error);
    }

    g_betterMediaPath = path;
    error = mciSendStringW(L"play BetterDetailPanelMedia", nullptr, 0,
                           nullptr);
    if (error) {
        mciSendStringW(L"close BetterDetailPanelMedia", nullptr, 0, nullptr);
        g_betterMediaPath.clear();
        return BetterPanelGetMciError(error);
    }

    return L"Playing";
}

muxc::Button BetterPanelMakeButton(PCWSTR label) {
    muxc::Button button;
    button.Content(winrt::box_value(label));
    button.MinWidth(0);
    button.Padding(Thickness{10, 4, 10, 4});
    button.MinHeight(32);
    button.HorizontalAlignment(HorizontalAlignment::Stretch);
    return button;
}

muxc::StackPanel BetterPanelMakeRow() {
    muxc::StackPanel row;
    row.Orientation(muxc::Orientation::Horizontal);
    row.Spacing(6);
    return row;
}

void TryInstallBetterDetailPanel(FrameworkElement element) {
    if (element.Name() != L"DetailsViewThumbnail") {
        return;
    }

    auto host = element.try_as<muxc::StackPanel>();
    if (!host) {
        return;
    }

    for (auto const& child : host.Children()) {
        if (auto frameworkElement = child.try_as<FrameworkElement>();
            frameworkElement &&
            frameworkElement.Name() == L"BetterDetailPanelRoot") {
            return;
        }
    }

    muxc::Border panel;
    panel.Name(L"BetterDetailPanelRoot");
    panel.Margin(Thickness{0, 10, 0, 0});
    panel.Padding(Thickness{8, 8, 8, 8});
    panel.CornerRadius(CornerRadius{8});

    muxc::StackPanel content;
    content.Spacing(7);

    muxc::TextBlock heading;
    heading.Text(L"Better Detail Panel");
    heading.FontWeight(winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    content.Children().Append(heading);

    muxc::TextBlock status;
    status.Text(L"Choose an action for the selected item");
    status.TextWrapping(TextWrapping::Wrap);
    status.Opacity(0.75);
    auto weakStatus = winrt::make_weak(status);

    auto row1 = BetterPanelMakeRow();
    auto openButton = BetterPanelMakeButton(L"Open");
    openButton.Click([weakStatus](winrt::Windows::Foundation::IInspectable const&,
                                  RoutedEventArgs const&) {
        auto path = BetterPanelGetSelectedPath();
        if (path.empty()) {
            BetterPanelSetStatus(weakStatus, L"Select one item first");
            return;
        }
        auto result = reinterpret_cast<INT_PTR>(ShellExecuteW(
            GetForegroundWindow(), L"open", path.c_str(), nullptr, nullptr,
            SW_SHOWNORMAL));
        BetterPanelSetStatus(weakStatus,
                             result > 32 ? L"Opened" : L"Open failed");
    });
    row1.Children().Append(openButton);

    auto openWithButton = BetterPanelMakeButton(L"Open with");
    openWithButton.Click(
        [weakStatus](winrt::Windows::Foundation::IInspectable const&,
                     RoutedEventArgs const&) {
            auto path = BetterPanelGetSelectedPath();
            if (path.empty()) {
                BetterPanelSetStatus(weakStatus, L"Select one file first");
                return;
            }
            OPENASINFO info{path.c_str(), nullptr, OAIF_EXEC};
            HRESULT hr = SHOpenWithDialog(GetForegroundWindow(), &info);
            BetterPanelSetStatus(weakStatus,
                                 SUCCEEDED(hr) ? L"Open With displayed"
                                               : L"Open With failed");
        });
    row1.Children().Append(openWithButton);
    content.Children().Append(row1);

    auto row2 = BetterPanelMakeRow();
    auto copyButton = BetterPanelMakeButton(L"Copy path");
    copyButton.Click([weakStatus](winrt::Windows::Foundation::IInspectable const&,
                                  RoutedEventArgs const&) {
        auto path = BetterPanelGetSelectedPath();
        if (path.empty()) {
            BetterPanelSetStatus(weakStatus, L"Select one item first");
            return;
        }

        bool copied = false;
        if (OpenClipboard(GetForegroundWindow())) {
            if (EmptyClipboard()) {
                SIZE_T bytes = (path.size() + 1) * sizeof(WCHAR);
                HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
                if (memory) {
                    if (void* target = GlobalLock(memory)) {
                        memcpy(target, path.c_str(), bytes);
                        GlobalUnlock(memory);
                        copied = SetClipboardData(CF_UNICODETEXT, memory) !=
                                 nullptr;
                    }
                    if (!copied) {
                        GlobalFree(memory);
                    }
                }
            }
            CloseClipboard();
        }
        BetterPanelSetStatus(weakStatus,
                             copied ? L"Path copied" : L"Copy failed");
    });
    row2.Children().Append(copyButton);

    auto propertiesButton = BetterPanelMakeButton(L"Properties");
    propertiesButton.Click(
        [weakStatus](winrt::Windows::Foundation::IInspectable const&,
                     RoutedEventArgs const&) {
            auto path = BetterPanelGetSelectedPath();
            if (path.empty()) {
                BetterPanelSetStatus(weakStatus, L"Select one item first");
                return;
            }
            SHELLEXECUTEINFOW executeInfo{sizeof(executeInfo)};
            executeInfo.fMask = SEE_MASK_INVOKEIDLIST;
            executeInfo.hwnd = GetForegroundWindow();
            executeInfo.lpVerb = L"properties";
            executeInfo.lpFile = path.c_str();
            executeInfo.nShow = SW_SHOWNORMAL;
            BetterPanelSetStatus(
                weakStatus, ShellExecuteExW(&executeInfo)
                                ? L"Properties displayed"
                                : L"Properties failed");
        });
    row2.Children().Append(propertiesButton);
    content.Children().Append(row2);

    auto previewState = std::make_shared<std::pair<bool, double>>(
        false, std::numeric_limits<double>::quiet_NaN());
    auto weakHost = winrt::make_weak(host);

    auto row3 = BetterPanelMakeRow();
    auto previewButton = BetterPanelMakeButton(L"Large preview");
    previewButton.Click(
        [weakHost, weakStatus, previewState](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&) {
            auto host = weakHost.get();
            if (!host) {
                return;
            }
            for (auto const& child : host.Children()) {
                auto thumbnail = child.try_as<muxc::Grid>();
                if (!thumbnail) {
                    continue;
                }
                if (!previewState->first) {
                    previewState->second = thumbnail.Height();
                    thumbnail.Height(280);
                    previewState->first = true;
                    BetterPanelSetStatus(weakStatus, L"Large preview enabled");
                } else {
                    thumbnail.Height(previewState->second);
                    previewState->first = false;
                    BetterPanelSetStatus(weakStatus, L"Preview restored");
                }
                return;
            }
            BetterPanelSetStatus(weakStatus, L"Preview is unavailable");
        });
    row3.Children().Append(previewButton);

    auto playButton = BetterPanelMakeButton(L"Play / pause MP3");
    playButton.Click([weakStatus](winrt::Windows::Foundation::IInspectable const&,
                                  RoutedEventArgs const&) {
        auto path = BetterPanelGetSelectedPath();
        BetterPanelSetStatus(
            weakStatus,
            path.empty() ? L"Select one MP3 file first"
                         : BetterPanelPlayOrPause(path));
    });
    row3.Children().Append(playButton);
    content.Children().Append(row3);

    auto stopButton = BetterPanelMakeButton(L"Stop audio");
    stopButton.Click([weakStatus](winrt::Windows::Foundation::IInspectable const&,
                                  RoutedEventArgs const&) {
        BetterPanelCloseMedia();
        BetterPanelSetStatus(weakStatus, L"Audio stopped");
    });
    content.Children().Append(stopButton);
    content.Children().Append(status);

    panel.Child(content);
    host.Children().Append(panel);

    std::lock_guard lock(g_betterPanelMutex);
    g_betterPanels.push_back(
        {host.DispatcherQueue(), winrt::make_weak(host),
         winrt::make_weak(panel.as<FrameworkElement>())});
    Wh_Log(L"Better Detail Panel added");
}

void RemoveBetterDetailPanelsForCurrentThread() {
    std::lock_guard lock(g_betterPanelMutex);
    for (auto it = g_betterPanels.begin(); it != g_betterPanels.end();) {
        if (!it->dispatcher || !it->dispatcher.HasThreadAccess()) {
            ++it;
            continue;
        }

        auto host = it->host.get();
        auto panel = it->panel.get();
        if (host && panel) {
            uint32_t index = 0;
            if (host.Children().IndexOf(panel.as<UIElement>(), index)) {
                host.Children().RemoveAt(index);
            }
        }
        it = g_betterPanels.erase(it);
    }
}

#endif  // Version 0.1 prototype.

////////////////////////////////////////////////////////////////////////////////
// Better Detail Panel 0.2: compact actions and an audio-only media card.

namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxp = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace muxa = winrt::Microsoft::UI::Xaml::Automation;
namespace muxap = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace muxapr = winrt::Microsoft::UI::Xaml::Automation::Provider;
namespace muxd = winrt::Microsoft::UI::Xaml::Documents;
namespace mud = winrt::Microsoft::UI::Dispatching;
namespace wmc = winrt::Windows::Media::Core;
namespace wmp = winrt::Windows::Media::Playback;
namespace wdp = winrt::Windows::Data::Pdf;
namespace wss = winrt::Windows::Storage::Streams;
namespace wsf = winrt::Windows::Storage::FileProperties;
namespace ws = winrt::Windows::Storage;

#define CWM_GETISHELLBROWSER (WM_USER + 7)

enum class BetterPanelTextEncoding {
    Utf8,
    Utf8Bom,
    Utf16Le,
    Utf16Be,
    Ansi,
};

enum class BetterPanelSyntaxLanguage {
    Auto,
    PlainText,
    Markdown,
    Cpp,
    CSharp,
    JavaScript,
    TypeScript,
    Json,
    Python,
    PowerShell,
    HtmlXml,
    Css,
    Yaml,
    Ini,
    Batch,
    Sql,
};

struct BetterPanelAudioMetadata {
    std::wstring title;
    std::wstring artist;
    std::wstring album;
    std::wstring genre;
    std::wstring year;
    std::wstring rating;
    std::wstring tags;
    std::wstring comments;
};

struct BetterPanelButtonLayoutSnapshot {
    winrt::weak_ref<muxp::ButtonBase> button;
    Thickness margin{};
    Thickness padding{};
    double width = std::numeric_limits<double>::quiet_NaN();
    double height = std::numeric_limits<double>::quiet_NaN();
    double minWidth = 0;
    double minHeight = 0;
};

struct BetterPanelState {
    mud::DispatcherQueue dispatcher{nullptr};
    HWND explorerWindow = nullptr;
    DispatcherTimer timer{nullptr};
    DispatcherTimer mediaTimer{nullptr};
    DispatcherTimer videoControlsTimer{nullptr};
    winrt::weak_ref<muxc::StackPanel> host;
    winrt::weak_ref<FrameworkElement> panel;
    winrt::weak_ref<FrameworkElement> audioCard;
    winrt::weak_ref<FrameworkElement> videoCard;
    winrt::weak_ref<FrameworkElement> gifCard;
    winrt::weak_ref<muxc::Image> gifImage;
    winrt::weak_ref<muxc::MediaPlayerElement> videoPlayer;
    winrt::weak_ref<muxc::Viewbox> videoViewport;
    winrt::weak_ref<muxc::Grid> nativePreview;
    winrt::weak_ref<muxc::Image> nativePreviewImage;
    winrt::weak_ref<FrameworkElement> multiPreviewSurface;
    winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage animatedGif{nullptr};
    winrt::weak_ref<muxc::Image> artwork;
    winrt::weak_ref<muxc::TextBlock> artworkPlaceholder;
    winrt::weak_ref<muxc::TextBlock> audioTitle;
    winrt::weak_ref<FrameworkElement> fileTitleRow;
    winrt::weak_ref<muxc::TextBlock> fileTitle;
    winrt::weak_ref<muxc::Image> fileIcon;
    winrt::weak_ref<muxc::FontIcon> fileFallbackIcon;
    winrt::weak_ref<muxc::TextBox> fileRenameBox;
    winrt::weak_ref<muxc::Button> fileRenameButton;
    winrt::weak_ref<muxc::TextBox> audioRenameBox;
    winrt::weak_ref<muxc::Button> audioRenameButton;
    winrt::weak_ref<FrameworkElement> nativeTitleContainer;
    Visibility nativeTitleVisibility = Visibility::Visible;
    winrt::weak_ref<muxc::Button> playButton;
    winrt::weak_ref<muxc::Slider> timeline;
    winrt::weak_ref<muxc::TextBlock> timeText;
    winrt::weak_ref<muxc::Slider> volumeSlider;
    winrt::weak_ref<muxc::Button> muteButton;
    winrt::weak_ref<muxc::TextBlock> volumeText;
    winrt::weak_ref<muxc::Button> playbackSpeedButton;
    winrt::weak_ref<muxc::Button> repeatButton;
    winrt::weak_ref<muxc::Button> shuffleButton;
    winrt::weak_ref<muxc::TextBlock> status;
    winrt::weak_ref<muxc::Button> nativeShareButton;
    winrt::weak_ref<muxc::Button> shareButton;
    winrt::weak_ref<muxc::Panel> nativeShareParent;
    winrt::weak_ref<muxc::Grid> shareActionRow;
    winrt::weak_ref<muxc::Button> openButton;
    winrt::weak_ref<muxc::Image> openActionIcon;
    winrt::weak_ref<muxc::FontIcon> openActionFallbackIcon;
    winrt::weak_ref<muxc::Button> openWithButton;
    winrt::weak_ref<muxc::Button> singleDeleteButton;
    winrt::weak_ref<muxc::Button> extractButton;
    winrt::weak_ref<muxc::Button> previewExpandButton;
    winrt::weak_ref<muxc::Button> gifExpandButton;
    winrt::weak_ref<FrameworkElement> transferRow;
    winrt::weak_ref<muxc::Button> transferExtractButton;
    winrt::weak_ref<muxc::Button> transferMoveButton;
    winrt::weak_ref<muxc::Button> transferItemsToggle;
    winrt::weak_ref<FrameworkElement> transferItemsCard;
    winrt::weak_ref<muxc::TextBlock> transferItemsText;
    winrt::weak_ref<FrameworkElement> multiActionRow;
    winrt::weak_ref<muxc::TextBlock> multiSelectionText;
    winrt::weak_ref<muxc::Grid> multiPreviewHost;
    winrt::weak_ref<muxc::Button> multiPreviewExpandButton;
    winrt::weak_ref<FrameworkElement> textCard;
    winrt::weak_ref<muxc::TextBox> textEditor;
    winrt::weak_ref<FrameworkElement> syntaxPreviewSurface;
    winrt::weak_ref<muxc::TextBlock> syntaxPreview;
    winrt::weak_ref<muxc::ComboBox> syntaxLanguagePicker;
    winrt::weak_ref<muxc::Button> textEditButton;
    winrt::weak_ref<muxc::Button> textSaveButton;
    winrt::weak_ref<muxc::Button> textCancelButton;
    winrt::weak_ref<muxc::Button> textReloadButton;
    winrt::weak_ref<muxc::TextBlock> textInfo;
    winrt::weak_ref<muxc::TextBlock> textActionStatus;
    winrt::weak_ref<FrameworkElement> pdfCard;
    winrt::weak_ref<muxc::Image> pdfImage;
    winrt::weak_ref<muxc::TextBlock> pdfPageText;
    winrt::weak_ref<muxc::TextBlock> pdfZoomText;
    winrt::weak_ref<muxc::TextBlock> pdfInfo;
    winrt::weak_ref<muxc::Button> pdfPreviousButton;
    winrt::weak_ref<muxc::Button> pdfNextButton;
    winrt::weak_ref<muxc::Button> favoriteButton;
    winrt::weak_ref<muxc::Button> printButton;
    Style nativeButtonStyle{nullptr};
    winrt::weak_ref<muxc::StackPanel> rootPanel;
    winrt::weak_ref<muxc::StackPanel> actionsHost;
    winrt::weak_ref<FrameworkElement> panelUtilities;
    winrt::weak_ref<muxc::Button> previewToggleButton;
    winrt::weak_ref<FrameworkElement> quickAudioControls;
    winrt::weak_ref<muxc::TextBlock> quickAudioTitle;
    winrt::weak_ref<muxc::Button> quickAudioPlayButton;
    winrt::weak_ref<muxc::TextBlock> detailsCopyHint;
    winrt::weak_ref<FrameworkElement> nativeDetailsSection;
    Visibility nativeDetailsVisibility = Visibility::Visible;
    winrt::weak_ref<FrameworkElement> nativeDetailsVisibilityHook;
    int64_t nativeDetailsVisibilityToken = 0;
    winrt::weak_ref<FrameworkElement> nativeInfoBanner;
    Visibility nativeInfoBannerVisibility = Visibility::Visible;
    winrt::weak_ref<FrameworkElement> insightsCard;
    winrt::weak_ref<FrameworkElement> homeCard;
    winrt::weak_ref<muxc::StackPanel> homeContent;
    winrt::weak_ref<FrameworkElement> driveCard;
    winrt::weak_ref<muxc::StackPanel> driveContent;
    winrt::weak_ref<muxc::StackPanel> insightsContent;
    winrt::weak_ref<muxc::Button> insightsToggleButton;
    winrt::weak_ref<FrameworkElement> metadataCard;
    winrt::weak_ref<muxc::StackPanel> metadataContent;
    winrt::weak_ref<muxc::Button> metadataToggleButton;
    winrt::weak_ref<muxc::TextBox> metadataTitle;
    winrt::weak_ref<muxc::TextBox> metadataArtist;
    winrt::weak_ref<muxc::TextBox> metadataAlbum;
    winrt::weak_ref<muxc::TextBox> metadataGenre;
    winrt::weak_ref<muxc::TextBox> metadataYear;
    winrt::weak_ref<muxc::TextBox> metadataRating;
    winrt::weak_ref<muxc::TextBox> metadataTags;
    winrt::weak_ref<muxc::TextBox> metadataComments;
    winrt::weak_ref<muxc::TextBox> metadataFocusedEditor;
    winrt::weak_ref<muxc::Button> metadataEditButton;
    winrt::weak_ref<muxc::Button> metadataSaveButton;
    winrt::weak_ref<muxc::Button> metadataCancelButton;
    winrt::weak_ref<muxc::Button> metadataReloadButton;
    winrt::weak_ref<muxc::TextBlock> metadataInfo;
    winrt::weak_ref<muxc::TextBlock> hashText;
    winrt::weak_ref<muxc::Button> hashCopyButton;
    winrt::weak_ref<FrameworkElement> pdfPageSurface;
    winrt::weak_ref<FrameworkElement> audioArtworkFrame;
    winrt::weak_ref<FrameworkElement> archivePreviewCard;
    winrt::weak_ref<muxc::TextBlock> archivePreviewText;
    uint32_t nativeShareIndex = 0;
    Thickness nativeShareMargin{};
    Visibility nativeShareVisibility = Visibility::Visible;
    bool previewExpanded = false;
    double previewOriginalHeight =
        std::numeric_limits<double>::quiet_NaN();
    double previewOriginalMinHeight = 0;
    std::wstring selectedPath;
    ULONGLONG lastNonEmptySelectionTick = 0;
    bool updatingTimeline = false;
    bool updatingVolume = false;
    bool unloaded = false;
    bool gifExpanded = false;
    double gifNormalHeight = 260;
    std::vector<std::wstring> transferSources;
    std::wstring transferDestination;
    std::wstring transferItemsKey;
    bool transferItemsExpanded = false;
    std::wstring multiPreviewKey;
    bool multiPreviewExpanded = false;
    double multiPreviewNormalHeight = 200;
    std::wstring textLoadedPath;
    std::wstring textOriginal;
    std::wstring textNewline = L"\r\n";
    BetterPanelTextEncoding textEncoding = BetterPanelTextEncoding::Utf8;
    FILETIME textLastWriteTime{};
    uint64_t textFileSize = 0;
    bool textLoading = false;
    bool textEditable = false;
    bool textEditing = false;
    bool textDirty = false;
    bool suppressTextChanged = false;
    BetterPanelSyntaxLanguage syntaxLanguage = BetterPanelSyntaxLanguage::Auto;
    BetterPanelSyntaxLanguage detectedSyntaxLanguage =
        BetterPanelSyntaxLanguage::PlainText;
    wdp::PdfDocument pdfDocument{nullptr};
    std::wstring pdfLoadedPath;
    uint32_t pdfPageIndex = 0;
    double pdfZoom = 1.0;
    double pdfBaseWidth = 420;
    bool pdfLoading = false;
    std::wstring insightsLoadedPath;
    std::wstring hashValue;
    std::wstring detailsCopyText;
    std::wstring multiSelectionKey;
    bool insightsLoading = false;
    bool multiSummaryLoading = false;
    bool previewsCollapsed = false;
    bool insightsCollapsed = false;
    BetterPanelAudioMetadata metadataOriginal;
    std::wstring metadataLoadedPath;
    std::wstring metadataCopyText;
    bool metadataLoading = false;
    bool metadataEditing = false;
    bool metadataDirty = false;
    bool suppressMetadataChanged = false;
    bool metadataCollapsed = false;
    struct AnimationHandlerRegistration {
        winrt::weak_ref<muxp::ButtonBase> button;
        winrt::event_token pointerEntered{};
        winrt::event_token pointerExited{};
        winrt::event_token click{};
    };
    std::unordered_map<void*, AnimationHandlerRegistration>
        animationHandlersInstalled;
    std::unordered_map<void*, muc::SpriteVisual> animationFlashOverlays;
    std::unordered_map<void*, BetterPanelButtonLayoutSnapshot>
        buttonLayoutSnapshots;
    std::atomic_uint64_t insightsGeneration{0};
    ULONGLONG transferLastScanTick = 0;
    HWND transferCachedActiveTab = nullptr;
    std::wstring transferCachedSourcePath;
    std::wstring printHandlerPath;
    bool printHandlerAvailable = false;
    std::wstring favoriteStatePath;
    bool favoriteStateKnown = false;
    bool favoritePinned = false;
    bool favoriteCheckPending = false;
    uint64_t favoriteCheckGeneration = 0;
    ULONGLONG nativeTitleLastSearchTick = 0;
    std::wstring nativeTitleSearchPath;
    ULONGLONG shareLastSearchTick = 0;
    ULONGLONG nativeDetailsLastSearchTick = 0;
    int64_t displayedPositionSecond = -1;
    int64_t displayedDurationSecond = -1;
    bool displayedPlaying = false;
    bool displayedPlaybackInitialized = false;
    bool mediaTimerRunning = false;
    bool interactiveRefreshQueued = false;
    winrt::event_token hostUnloadedToken{};
    bool homeContentLoaded = false;
    bool homeWasVisible = false;
    std::wstring archivePreviewPath;
    bool archivePreviewLoading = false;
};

std::mutex g_betterPanelMutex;
std::vector<std::shared_ptr<BetterPanelState>> g_betterPanels;
std::mutex g_betterMediaMutex;
wmp::MediaPlayer g_betterMediaPlayer{nullptr};
std::wstring g_betterMediaPath;
double g_betterMediaVolume = 0.75;
bool g_betterMediaMuted = false;
double g_betterPlaybackRate = 1.0;
bool g_betterRepeatEnabled = false;
bool g_betterShuffleEnabled = false;

bool BetterPanelPlayBrightFlash(
    std::shared_ptr<BetterPanelState> const& state,
    UIElement const& element,
    std::chrono::milliseconds duration) {
    if (!state || !element) return false;

    try {
        void* identity = winrt::get_abi(element);
        muc::SpriteVisual overlay{nullptr};
        if (auto existing = state->animationFlashOverlays.find(identity);
            existing != state->animationFlashOverlays.end()) {
            overlay = existing->second;
        } else {
            auto currentChild =
                muxh::ElementCompositionPreview::GetElementChildVisual(
                    element);
            if (currentChild) {
                // Never replace or reparent a visual owned by Explorer.
                // The caller will provide a safe transform fallback.
                return false;
            }
            auto visual = muxh::ElementCompositionPreview::GetElementVisual(
                element);
            auto compositor = visual.Compositor();
            overlay = compositor.CreateSpriteVisual();
            overlay.Brush(compositor.CreateColorBrush(
                winrt::Windows::UI::ColorHelper::FromArgb(
                    255, 255, 255, 255)));
            overlay.Opacity(0.0f);
            muxh::ElementCompositionPreview::SetElementChildVisual(
                element, overlay);
            state->animationFlashOverlays.emplace(identity, overlay);
        }

        overlay.Size(element.ActualSize());
        overlay.Opacity(0.0f);
        auto animation = overlay.Compositor().CreateScalarKeyFrameAnimation();
        animation.Duration(duration);
        animation.InsertKeyFrame(0.0f, 0.0f);
        animation.InsertKeyFrame(0.22f, 0.62f);
        animation.InsertKeyFrame(0.48f, 0.34f);
        animation.InsertKeyFrame(1.0f, 0.0f);
        overlay.StartAnimation(L"Opacity", animation);
        return true;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Flash animation error %08X: %s", ex.code(),
               ex.message().c_str());
        return false;
    }
}

winrt::Windows::Foundation::Numerics::float3 BetterPanelClampedScale(
    UIElement const& element, float relativeChange,
    float maximumEdgeMovement = 4.0f) {
    auto size = element.ActualSize();
    auto clampedAxis = [relativeChange, maximumEdgeMovement](float length) {
        if (length <= 0.0f) return 1.0f + relativeChange;
        float limit = maximumEdgeMovement * 2.0f / length;
        float magnitude = std::min(std::abs(relativeChange), limit);
        return 1.0f + std::copysign(magnitude, relativeChange);
    };
    return {clampedAxis(size.x), clampedAxis(size.y), 1.0f};
}

float BetterPanelClampedRotation(UIElement const& element,
                                 float requestedDegrees,
                                 float maximumEdgeMovement = 4.0f) {
    auto size = element.ActualSize();
    float radius = std::max(size.x, size.y) / 2.0f;
    if (radius <= 0.0f) return requestedDegrees;
    constexpr float radiansToDegrees = 57.2957795131f;
    float limit = std::atan(maximumEdgeMovement / radius) * radiansToDegrees;
    return std::copysign(std::min(std::abs(requestedDegrees), limit),
                         requestedDegrees);
}

UIElement BetterPanelFindCompactAnimationTarget(DependencyObject const& root,
                                                int depth = 0) {
    if (!root || depth > 10) return nullptr;
    if (depth > 0 &&
        (root.try_as<muxc::FontIcon>() || root.try_as<muxc::Image>())) {
        return root.try_as<UIElement>();
    }
    int count = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
        GetChildrenCount(root);
    for (int index = 0; index < count; ++index) {
        auto target = BetterPanelFindCompactAnimationTarget(
            winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::GetChild(
                root, index),
            depth + 1);
        if (target) return target;
    }
    return nullptr;
}

winrt::Windows::Foundation::Numerics::float3 BetterPanelHoverRestScale(
    UIElement const& element, bool pointerOver) {
    if (!pointerOver) return {1.0f, 1.0f, 1.0f};
    switch (g_settings.hoverAnimation) {
        case BetterPanelHoverAnimation::Scale:
            return BetterPanelClampedScale(element, 0.07f);
        case BetterPanelHoverAnimation::Lift:
            return BetterPanelClampedScale(element, 0.025f, 2.0f);
        case BetterPanelHoverAnimation::Spring:
            return BetterPanelClampedScale(element, 0.08f);
        default:
            return {1.0f, 1.0f, 1.0f};
    }
}

void BetterPanelSetAnimationRestState(
                                      std::shared_ptr<BetterPanelState> const& state,
                                      UIElement const& element,
                                      bool pointerOver) {
    if (!element) return;

    try {
        auto visual = muxh::ElementCompositionPreview::GetElementVisual(
            element);
        auto compositor = visual.Compositor();
        visual.CenterPoint({element.ActualSize().x / 2,
                            element.ActualSize().y / 2, 0.0f});

        auto animateVector = [&](wchar_t const* property,
                                 winrt::Windows::Foundation::Numerics::float3
                                     from,
                                 winrt::Windows::Foundation::Numerics::float3
                                     to) {
            auto animation = compositor.CreateVector3KeyFrameAnimation();
            animation.Duration(std::chrono::milliseconds(140));
            auto easing = compositor.CreateCubicBezierEasingFunction(
                {0.1f, 0.9f}, {0.2f, 1.0f});
            animation.InsertKeyFrame(0.0f, from);
            animation.InsertKeyFrame(1.0f, to, easing);
            visual.StartAnimation(property, animation);
        };
        auto animateScalar = [&](wchar_t const* property, float from,
                                 float to) {
            auto animation = compositor.CreateScalarKeyFrameAnimation();
            animation.Duration(std::chrono::milliseconds(140));
            auto easing = compositor.CreateCubicBezierEasingFunction(
                {0.1f, 0.9f}, {0.2f, 1.0f});
            animation.InsertKeyFrame(0.0f, from);
            animation.InsertKeyFrame(1.0f, to, easing);
            visual.StartAnimation(property, animation);
        };

        auto targetScale = BetterPanelHoverRestScale(element, pointerOver);
        float targetOpacity = 1.0f;
        float targetRotation = 0.0f;
        auto targetTranslation =
            winrt::Windows::Foundation::Numerics::float3{0.0f, 0.0f, 0.0f};

        if (pointerOver) {
            switch (g_settings.hoverAnimation) {
                case BetterPanelHoverAnimation::Scale:
                    break;
                case BetterPanelHoverAnimation::Lift:
                    // A small scale plus a visible rise keeps Lift distinct
                    // from Explorer's built-in color-only hover response.
                    targetTranslation = {0.0f, -4.0f, 0.0f};
                    break;
                case BetterPanelHoverAnimation::Tilt:
                    targetRotation =
                        BetterPanelClampedRotation(element, -2.0f);
                    break;
                case BetterPanelHoverAnimation::Spring:
                    break;
                default:
                    break;
            }
        }

        auto currentScale = visual.Scale();
        float currentOpacity = visual.Opacity();
        float currentRotation = visual.RotationAngleInDegrees();
        visual.Scale(targetScale);
        visual.Opacity(targetOpacity);
        visual.RotationAngleInDegrees(targetRotation);

        Vector3Transition translationTransition;
        translationTransition.Duration(std::chrono::milliseconds(140));
        element.TranslationTransition(translationTransition);
        element.Translation(targetTranslation);

        animateVector(L"Scale", currentScale, targetScale);
        animateScalar(L"Opacity", currentOpacity, targetOpacity);
        animateScalar(L"RotationAngleInDegrees", currentRotation,
                      targetRotation);

        if (pointerOver &&
                   g_settings.hoverAnimation ==
                       BetterPanelHoverAnimation::Spring) {
            auto spring = compositor.CreateVector3KeyFrameAnimation();
            spring.Duration(std::chrono::milliseconds(380));
            spring.InsertKeyFrame(0.0f, {1.0f, 1.0f, 1.0f});
            spring.InsertKeyFrame(
                0.32f, BetterPanelClampedScale(element, 0.11f));
            spring.InsertKeyFrame(
                0.58f, BetterPanelClampedScale(element, 0.04f));
            spring.InsertKeyFrame(
                0.78f, BetterPanelClampedScale(element, 0.09f));
            spring.InsertKeyFrame(1.0f, targetScale);
            visual.StartAnimation(L"Scale", spring);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Hover animation error %08X: %s", ex.code(),
               ex.message().c_str());
    }
}

void BetterPanelPlayClickAnimation(
                                   std::shared_ptr<BetterPanelState> const& state,
                                   UIElement const& element,
                                   bool pointerOver) {
    if (!element ||
        g_settings.clickAnimation ==
            BetterPanelClickAnimation::WindowsDefault ||
        g_settings.clickAnimation == BetterPanelClickAnimation::None) {
        return;
    }

    try {
        auto visual = muxh::ElementCompositionPreview::GetElementVisual(
            element);
        auto compositor = visual.Compositor();
        visual.CenterPoint({element.ActualSize().x / 2,
                            element.ActualSize().y / 2, 0.0f});

        auto restScale = BetterPanelHoverRestScale(element, pointerOver);
        float restRotation =
            pointerOver && g_settings.hoverAnimation ==
                               BetterPanelHoverAnimation::Tilt
                ? BetterPanelClampedRotation(element, -2.0f)
                : 0.0f;

        if (g_settings.clickAnimation == BetterPanelClickAnimation::Press ||
            g_settings.clickAnimation == BetterPanelClickAnimation::Pulse) {
            auto animation = compositor.CreateVector3KeyFrameAnimation();
            animation.Duration(std::chrono::milliseconds(220));
            animation.InsertKeyFrame(
                0.0f, restScale);
            animation.InsertKeyFrame(
                0.35f,
                g_settings.clickAnimation == BetterPanelClickAnimation::Press
                    ? BetterPanelClampedScale(element, -0.14f)
                    : BetterPanelClampedScale(element, 0.14f));
            animation.InsertKeyFrame(
                1.0f, restScale);
            visual.StartAnimation(L"Scale", animation);
        } else if (g_settings.clickAnimation ==
                   BetterPanelClickAnimation::Bounce) {
            auto base = visual.Offset();
            auto animation = compositor.CreateVector3KeyFrameAnimation();
            animation.Duration(std::chrono::milliseconds(330));
            animation.InsertKeyFrame(0.0f, base);
            animation.InsertKeyFrame(
                0.28f, {base.x, base.y + 7.0f, base.z});
            animation.InsertKeyFrame(
                0.62f, {base.x, base.y - 5.0f, base.z});
            animation.InsertKeyFrame(1.0f, base);
            visual.StartAnimation(L"Offset", animation);
        } else if (g_settings.clickAnimation ==
                   BetterPanelClickAnimation::Flash) {
            if (!BetterPanelPlayBrightFlash(
                    state, element, std::chrono::milliseconds(320))) {
                auto fallback = compositor.CreateVector3KeyFrameAnimation();
                fallback.Duration(std::chrono::milliseconds(280));
                fallback.InsertKeyFrame(
                    0.0f, restScale);
                fallback.InsertKeyFrame(
                    0.30f, BetterPanelClampedScale(element, 0.16f));
                fallback.InsertKeyFrame(
                    1.0f, restScale);
                visual.StartAnimation(L"Scale", fallback);
            }
        } else if (g_settings.clickAnimation ==
                   BetterPanelClickAnimation::Shake) {
            auto animation = compositor.CreateScalarKeyFrameAnimation();
            animation.Duration(std::chrono::milliseconds(300));
            animation.InsertKeyFrame(0.0f, restRotation);
            animation.InsertKeyFrame(
                0.22f, BetterPanelClampedRotation(element, -6.0f));
            animation.InsertKeyFrame(
                0.48f, BetterPanelClampedRotation(element, 6.0f));
            animation.InsertKeyFrame(
                0.74f, BetterPanelClampedRotation(element, -3.0f));
            animation.InsertKeyFrame(1.0f, restRotation);
            visual.StartAnimation(L"RotationAngleInDegrees", animation);
        } else if (g_settings.clickAnimation ==
                   BetterPanelClickAnimation::Spring) {
            auto animation = compositor.CreateVector3KeyFrameAnimation();
            animation.Duration(std::chrono::milliseconds(430));
            animation.InsertKeyFrame(
                0.0f, BetterPanelClampedScale(element, -0.16f));
            animation.InsertKeyFrame(
                0.30f, BetterPanelClampedScale(element, 0.15f));
            animation.InsertKeyFrame(
                0.55f, BetterPanelClampedScale(element, -0.04f));
            animation.InsertKeyFrame(
                0.76f, BetterPanelClampedScale(element, 0.07f));
            animation.InsertKeyFrame(1.0f, restScale);
            visual.StartAnimation(L"Scale", animation);
        } else if (g_settings.clickAnimation ==
                   BetterPanelClickAnimation::Spin) {
            auto spinTarget = BetterPanelFindCompactAnimationTarget(element);
            auto spinVisual = spinTarget
                ? muxh::ElementCompositionPreview::GetElementVisual(spinTarget)
                : visual;
            auto spinSize = spinTarget ? spinTarget.ActualSize()
                                       : element.ActualSize();
            spinVisual.CenterPoint(
                {spinSize.x / 2, spinSize.y / 2, 0.0f});
            auto animation =
                spinVisual.Compositor().CreateScalarKeyFrameAnimation();
            animation.Duration(std::chrono::milliseconds(360));
            animation.InsertKeyFrame(0.0f, spinTarget ? 0.0f : restRotation);
            animation.InsertKeyFrame(
                1.0f, (spinTarget ? 0.0f : restRotation) + 360.0f);
            spinVisual.StartAnimation(L"RotationAngleInDegrees", animation);
        } else if (g_settings.clickAnimation ==
                   BetterPanelClickAnimation::Squash) {
            auto animation = compositor.CreateVector3KeyFrameAnimation();
            animation.Duration(std::chrono::milliseconds(260));
            animation.InsertKeyFrame(
                0.0f, restScale);
            auto squashWide = BetterPanelClampedScale(element, 0.16f);
            auto squashShort = BetterPanelClampedScale(element, -0.22f);
            animation.InsertKeyFrame(
                0.36f, {squashWide.x, squashShort.y, 1.0f});
            animation.InsertKeyFrame(
                1.0f, restScale);
            visual.StartAnimation(L"Scale", animation);
        } else if (g_settings.clickAnimation ==
                   BetterPanelClickAnimation::Wobble) {
            auto animation = compositor.CreateScalarKeyFrameAnimation();
            animation.Duration(std::chrono::milliseconds(380));
            animation.InsertKeyFrame(0.0f, restRotation);
            animation.InsertKeyFrame(
                0.20f, BetterPanelClampedRotation(element, -7.0f));
            animation.InsertKeyFrame(
                0.43f, BetterPanelClampedRotation(element, 6.0f));
            animation.InsertKeyFrame(
                0.66f, BetterPanelClampedRotation(element, -4.0f));
            animation.InsertKeyFrame(
                0.84f, BetterPanelClampedRotation(element, 2.0f));
            animation.InsertKeyFrame(1.0f, restRotation);
            visual.StartAnimation(L"RotationAngleInDegrees", animation);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Button animation error %08X: %s", ex.code(),
               ex.message().c_str());
    }
}

void BetterPanelRemoveAnimationHandlers(
    std::shared_ptr<BetterPanelState> const& state) {
    if (!state) return;

    for (auto const& [identity, registration] :
         state->animationHandlersInstalled) {
        try {
            if (auto button = registration.button.get()) {
                if (registration.pointerEntered.value) {
                    button.PointerEntered(registration.pointerEntered);
                }
                if (registration.pointerExited.value) {
                    button.PointerExited(registration.pointerExited);
                }
                if (registration.click.value) {
                    button.Click(registration.click);
                }
                if (auto overlay = state->animationFlashOverlays.find(identity);
                    overlay != state->animationFlashOverlays.end()) {
                    auto child = muxh::ElementCompositionPreview::
                        GetElementChildVisual(button);
                    if (child == overlay->second) {
                        muxh::ElementCompositionPreview::SetElementChildVisual(
                            button, nullptr);
                    }
                }
            }
        } catch (...) {
            Wh_Log(L"Animation handler cleanup error %08X",
                   winrt::to_hresult().value);
        }
    }
    state->animationHandlersInstalled.clear();
    state->animationFlashOverlays.clear();
}

void BetterPanelDetachHostUnloadedHandler(
    std::shared_ptr<BetterPanelState> const& state) {
    if (!state || !state->hostUnloadedToken.value) return;
    try {
        if (auto host = state->host.get()) {
            host.Unloaded(state->hostUnloadedToken);
        }
    } catch (...) {
        Wh_Log(L"Host unload cleanup error %08X", winrt::to_hresult().value);
    }
    state->hostUnloadedToken = {};
}

void BetterPanelInstallAnimationHandlers(
    std::shared_ptr<BetterPanelState> const& state,
    muxp::ButtonBase const& button) {
    if (!state || !button) return;

    void* identity = winrt::get_abi(button);
    if (auto existing = state->animationHandlersInstalled.find(identity);
        existing != state->animationHandlersInstalled.end()) {
        if (auto installedButton = existing->second.button.get();
            installedButton == button) {
            BetterPanelSetAnimationRestState(state, button,
                                              button.IsPointerOver());
            return;
        }
        state->animationHandlersInstalled.erase(existing);
    }

    auto weakButton = winrt::make_weak(button);
    std::weak_ptr<BetterPanelState> weakState = state;
    BetterPanelState::AnimationHandlerRegistration registration;
    registration.button = weakButton;
    registration.pointerEntered = button.PointerEntered(
        [weakButton, weakState](
                     winrt::Windows::Foundation::IInspectable const&,
                     winrt::Microsoft::UI::Xaml::Input::
                         PointerRoutedEventArgs const&) {
            if (auto element = weakButton.get(); element) {
                if (auto state = weakState.lock()) {
                    BetterPanelSetAnimationRestState(state, element, true);
                }
            }
        });
    registration.pointerExited = button.PointerExited(
        [weakButton, weakState](
                     winrt::Windows::Foundation::IInspectable const&,
                     winrt::Microsoft::UI::Xaml::Input::
                         PointerRoutedEventArgs const&) {
            if (auto element = weakButton.get(); element) {
                if (auto state = weakState.lock()) {
                    BetterPanelSetAnimationRestState(state, element, false);
                }
            }
        });

    registration.click = button.Click([weakButton, weakState](
                     winrt::Windows::Foundation::IInspectable const&,
                     RoutedEventArgs const&) {
        if (auto element = weakButton.get(); element) {
            if (auto state = weakState.lock()) {
                BetterPanelPlayClickAnimation(
                    state, element, element.IsPointerOver());
            }
        }
    });
    state->animationHandlersInstalled.emplace(identity,
                                               std::move(registration));
    BetterPanelSetAnimationRestState(state, button, button.IsPointerOver());
}

void BetterPanelApplyNativeButtonStyle(
    std::shared_ptr<BetterPanelState> const& state,
    DependencyObject const& root) {
    if (!state || !root) return;

    if (auto buttonBase = root.try_as<muxp::ButtonBase>()) {
        BetterPanelInstallAnimationHandlers(state, buttonBase);
    }

    if (auto button = root.try_as<muxc::Button>()) {
        if (state->nativeButtonStyle &&
            button.Style() != state->nativeButtonStyle) {
            button.Style(state->nativeButtonStyle);
        }
        auto buttonBase = button.as<muxp::ButtonBase>();
        void* identity = winrt::get_abi(buttonBase);
        auto snapshotIt = state->buttonLayoutSnapshots.find(identity);
        if (snapshotIt == state->buttonLayoutSnapshots.end() ||
            snapshotIt->second.button.get() != buttonBase) {
            snapshotIt = state->buttonLayoutSnapshots.insert_or_assign(
                identity,
                BetterPanelButtonLayoutSnapshot{
                    winrt::make_weak(buttonBase), button.Margin(), button.Padding(),
                    button.Width(), button.Height(), button.MinWidth(),
                    button.MinHeight()}).first;
        }
        auto const& snapshot = snapshotIt->second;
        button.Margin(snapshot.margin);
        button.Padding(snapshot.padding);
        button.Width(snapshot.width);
        button.Height(snapshot.height);
        button.MinWidth(snapshot.minWidth);
        button.MinHeight(snapshot.minHeight);

        bool compact = g_settings.buttonLayoutMode ==
                       BetterPanelButtonLayoutMode::Compact;
        bool comfort = g_settings.buttonLayoutMode ==
                       BetterPanelButtonLayoutMode::Comfort;
        auto content = button.Content();
        bool iconOnly = content &&
            (content.try_as<muxc::FontIcon>() || content.try_as<muxc::Image>());
        bool playbackSpeed = state->playbackSpeedButton.get() == button;
        if (!compact) {
            int horizontalMargin =
                comfort ? 0 : g_settings.buttonHorizontalMargin;
            int verticalMargin = comfort ? 0 : g_settings.buttonVerticalMargin;
            button.Margin(Thickness{
                snapshot.margin.Left + horizontalMargin,
                snapshot.margin.Top + verticalMargin,
                snapshot.margin.Right + horizontalMargin,
                snapshot.margin.Bottom + verticalMargin});
        }
        if (iconOnly || playbackSpeed) {
            // Compact is a real shared preset, not each control's unrelated
            // hard-coded starting size. This keeps Copy all, preview, print,
            // delete, expand, and media icon buttons in step with one another.
            double size = compact ? 28.0
                                  : (comfort ? 32.0
                                             : g_settings.iconButtonSize);
            double width = playbackSpeed ? std::max(34.0, size) : size;
            button.Width(width);
            button.Height(size);
            button.MinWidth(width);
            button.MinHeight(size);
            button.Padding(playbackSpeed ? Thickness{2, 0, 2, 0}
                                         : Thickness{0});
        } else if (!compact) {
                int horizontalPadding =
                    comfort ? 10 : g_settings.buttonHorizontalPadding;
                int verticalPadding =
                    comfort ? 4 : g_settings.buttonVerticalPadding;
                int minimumWidth = comfort ? 0 : g_settings.buttonMinimumWidth;
                int minimumHeight =
                    comfort ? 32 : g_settings.buttonMinimumHeight;
                button.MinWidth(minimumWidth);
                button.MinHeight(minimumHeight);
                button.Padding(Thickness{
                    static_cast<double>(horizontalPadding),
                    static_cast<double>(verticalPadding),
                    static_cast<double>(horizontalPadding),
                    static_cast<double>(verticalPadding)});
                // Fixed-size action buttons must grow with their padding too;
                // otherwise the content area shrinks and the setting appears
                // to do nothing (or clips the label).
                if (std::isfinite(snapshot.width)) {
                    double paddingDelta = horizontalPadding * 2.0 -
                        snapshot.padding.Left - snapshot.padding.Right;
                    button.Width(std::max(0.0,
                                          snapshot.width + paddingDelta));
                }
                if (std::isfinite(snapshot.height)) {
                    double paddingDelta = verticalPadding * 2.0 -
                        snapshot.padding.Top - snapshot.padding.Bottom;
                    button.Height(std::max(0.0,
                                           snapshot.height + paddingDelta));
                }
        }
    } else if (auto button = root.try_as<muxp::ToggleButton>();
               button) {
        auto buttonBase = button.as<muxp::ButtonBase>();
        void* identity = winrt::get_abi(buttonBase);
        auto snapshotIt = state->buttonLayoutSnapshots.find(identity);
        if (snapshotIt == state->buttonLayoutSnapshots.end() ||
            snapshotIt->second.button.get() != buttonBase) {
            snapshotIt = state->buttonLayoutSnapshots.insert_or_assign(
                identity,
                BetterPanelButtonLayoutSnapshot{
                    winrt::make_weak(buttonBase), button.Margin(), button.Padding(),
                    button.Width(), button.Height(), button.MinWidth(),
                    button.MinHeight()}).first;
        }
        auto const& snapshot = snapshotIt->second;
        button.Margin(snapshot.margin);
        button.Padding(snapshot.padding);
        button.Width(snapshot.width);
        button.Height(snapshot.height);
        button.MinWidth(snapshot.minWidth);
        button.MinHeight(snapshot.minHeight);
        if (g_settings.buttonLayoutMode !=
            BetterPanelButtonLayoutMode::Compact) {
            bool comfort = g_settings.buttonLayoutMode ==
                           BetterPanelButtonLayoutMode::Comfort;
            int horizontalMargin =
                comfort ? 0 : g_settings.buttonHorizontalMargin;
            int verticalMargin = comfort ? 0 : g_settings.buttonVerticalMargin;
            int horizontalPadding =
                comfort ? 10 : g_settings.buttonHorizontalPadding;
            int verticalPadding = comfort ? 4 : g_settings.buttonVerticalPadding;
            button.Margin(Thickness{
                snapshot.margin.Left + horizontalMargin,
                snapshot.margin.Top + verticalMargin,
                snapshot.margin.Right + horizontalMargin,
                snapshot.margin.Bottom + verticalMargin});
            button.MinWidth(comfort ? 0 : g_settings.buttonMinimumWidth);
            button.MinHeight(comfort ? 32 : g_settings.buttonMinimumHeight);
            button.Padding(Thickness{
                static_cast<double>(horizontalPadding),
                static_cast<double>(verticalPadding),
                static_cast<double>(horizontalPadding),
                static_cast<double>(verticalPadding)});
            if (std::isfinite(snapshot.width)) {
                double paddingDelta = horizontalPadding * 2.0 -
                    snapshot.padding.Left - snapshot.padding.Right;
                button.Width(std::max(0.0,
                                      snapshot.width + paddingDelta));
            }
            if (std::isfinite(snapshot.height)) {
                double paddingDelta = verticalPadding * 2.0 -
                    snapshot.padding.Top - snapshot.padding.Bottom;
                button.Height(std::max(0.0,
                                       snapshot.height + paddingDelta));
            }
        }
    }

    int childCount = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
        GetChildrenCount(root);
    for (int index = 0; index < childCount; index++) {
        BetterPanelApplyNativeButtonStyle(
            state,
            winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::GetChild(
                root, index));
    }
}

void BetterPanelPruneButtonLayoutSnapshots(
    std::shared_ptr<BetterPanelState> const& state) {
    if (!state) return;
    std::erase_if(state->buttonLayoutSnapshots, [](auto const& entry) {
        return !entry.second.button.get();
    });
}

std::optional<bool> BetterPanelReadFavoriteState(std::wstring const& path);
void BetterPanelSetStatus(winrt::weak_ref<muxc::TextBlock> weakStatus,
                          std::wstring_view text);
winrt::fire_and_forget BetterPanelShowTemporaryButtonMessage(
    winrt::weak_ref<muxc::Button> weakButton, std::wstring_view text);
IShellBrowser* BetterPanelGetShellBrowser(HWND window);

void BetterPanelRefreshFavoritesViews(
    std::shared_ptr<BetterPanelState> const& state,
    std::wstring const& path) {
    SHChangeNotify(SHCNE_UPDATEITEM,
                   SHCNF_PATHW | SHCNF_FLUSH,
                   path.c_str(), nullptr);

    PIDLIST_ABSOLUTE homePidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(L"shell:Home", nullptr, &homePidl,
                                     0, nullptr)) && homePidl) {
        SHChangeNotify(SHCNE_UPDATEDIR,
                       SHCNF_IDLIST | SHCNF_FLUSH,
                       homePidl, nullptr);
        CoTaskMemFree(homePidl);
    }

    HWND root = state && state->explorerWindow
                    ? state->explorerWindow
                    : GetForegroundWindow();
    root = root ? GetAncestor(root, GA_ROOT) : nullptr;
    for (HWND tab = nullptr; root &&
         (tab = FindWindowExW(root, tab, L"ShellTabWindowClass", nullptr));) {
        if (auto browser = BetterPanelGetShellBrowser(tab)) {
            winrt::com_ptr<IShellView> view;
            if (SUCCEEDED(browser->QueryActiveShellView(view.put())) && view) {
                view->Refresh();
            }
        }
    }
}

void BetterPanelApplyFavoriteVisual(
    std::shared_ptr<BetterPanelState> const& state) {
    if (!state) return;
    auto button = state->favoriteButton.get();
    if (!button) return;
    auto icon = button.Content().try_as<muxc::FontIcon>();
    if (icon) {
        icon.Glyph(state->favoritePinned ? L"\uE735" : L"\uE734");
        if (state->favoritePinned) {
            icon.Foreground(
                winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(
                    winrt::Windows::UI::ColorHelper::FromArgb(
                        255, 255, 193, 7)));
        } else {
            icon.ClearValue(muxc::IconElement::ForegroundProperty());
        }
    }
    std::wstring label = state->favoritePinned
                             ? L"Remove from Favorites"
                             : L"Add to Favorites";
    muxa::AutomationProperties::SetName(button, label);
    muxc::ToolTipService::SetToolTip(button, winrt::box_value(label));
}

void BetterPanelQueueFavoriteCheck(
    std::shared_ptr<BetterPanelState> const& state,
    std::wstring const& path, bool verifyAction = false,
    bool expectedPinned = true) {
    if (!state || path.empty()) return;
    state->favoriteCheckPending = true;
    uint64_t generation = ++state->favoriteCheckGeneration;
    auto dispatcher = state->dispatcher;
    std::weak_ptr<BetterPanelState> weakState = state;
    std::thread([weakState, dispatcher, path, generation, verifyAction,
                 expectedPinned]() {
        HRESULT initialized = CoInitializeEx(nullptr,
                                             COINIT_APARTMENTTHREADED);
        std::optional<bool> pinned;
        int attempts = verifyAction ? 12 : 1;
        for (int attempt = 0; attempt < attempts; attempt++) {
            if (attempt) Sleep(150);
            pinned = BetterPanelReadFavoriteState(path);
            if (pinned &&
                (!verifyAction || *pinned == expectedPinned)) break;
        }
        if (SUCCEEDED(initialized)) CoUninitialize();
        dispatcher.TryEnqueue(
            [weakState, path, generation, pinned, verifyAction,
             expectedPinned]() {
                auto state = weakState.lock();
                if (!state || state->unloaded ||
                    state->favoriteCheckGeneration != generation ||
                    state->selectedPath != path) {
                    return;
                }
                state->favoriteCheckPending = false;
                state->favoriteStatePath = path;
                state->favoriteStateKnown = pinned.has_value();
                state->favoritePinned = pinned.value_or(false);
                BetterPanelApplyFavoriteVisual(state);
                if (verifyAction) {
                    bool changed = state->favoriteStateKnown &&
                                   state->favoritePinned == expectedPinned;
                    if (changed) {
                        BetterPanelRefreshFavoritesViews(state, path);
                    }
                    BetterPanelShowTemporaryButtonMessage(
                        state->favoriteButton,
                        changed
                            ? (expectedPinned ? L"Added to Favorites"
                                              : L"Removed from Favorites")
                            : (expectedPinned
                                   ? L"Windows did not add this item to Favorites"
                                   : L"Windows did not remove this item from Favorites"));
                }
            });
    }).detach();
}

void BetterPanelRefresh(std::shared_ptr<BetterPanelState> const& state);
void BetterPanelInvalidateExplorerQueryCaches();
HWND BetterPanelGetFocusedTabWindow();
void BetterPanelHandlePowerMessage(MSG const* message);

void BetterPanelPrepareMiddleClick(MSG const* message);
void BetterPanelHandleMiddleClick(MSG const* message);

bool BetterPanelMessageCanChangeExplorerState(MSG const* message) {
    if (!message) return false;
    switch (message->message) {
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        case WM_COMMAND:
        case WM_APPCOMMAND:
            return true;
        default:
            return false;
    }
}

void BetterPanelQueueCurrentThreadRefresh(bool skipEditingStates = false) {
    BetterPanelInvalidateExplorerQueryCaches();
    std::lock_guard lock(g_betterPanelMutex);
    for (auto const& state : g_betterPanels) {
        if (!state || state->unloaded || state->interactiveRefreshQueued ||
            !state->dispatcher || !state->dispatcher.HasThreadAccess()) {
            continue;
        }
        if (skipEditingStates &&
            (state->textEditing || state->metadataEditing)) {
            continue;
        }

        state->transferLastScanTick = 0;
        state->interactiveRefreshQueued = true;
        std::weak_ptr<BetterPanelState> weakState = state;
        if (!state->dispatcher.TryEnqueue([weakState]() {
                auto state = weakState.lock();
                if (!state) return;
                state->interactiveRefreshQueued = false;
                BetterPanelRefresh(state);
            })) {
            state->interactiveRefreshQueued = false;
        }
    }
}

void BetterPanelQueueInteractiveRefresh(MSG const* message) {
    if (message && message->message == WM_MBUTTONDOWN) {
        BetterPanelPrepareMiddleClick(message);
        return;
    }
    if (!BetterPanelMessageCanChangeExplorerState(message)) return;

    if (message->message == WM_MBUTTONUP) {
        BetterPanelHandleMiddleClick(message);
    }

    // Capture the native tab while Explorer's input message still has focus.
    // Deferred XAML refreshes often run after focus has moved into the details
    // pane, where enumerating visible ShellTabWindowClass windows can select a
    // stale background tab (commonly Home/This PC).
    BetterPanelGetFocusedTabWindow();
    BetterPanelScheduleCurrentThreadDiscovery();
    BetterPanelQueueCurrentThreadRefresh(
        message->message == WM_KEYUP || message->message == WM_SYSKEYUP);
}

void BetterPanelApplyEditorBackspace(muxc::TextBox const& editor) {
    int32_t selectionStart = editor.SelectionStart();
    int32_t selectionLength = editor.SelectionLength();
    if (selectionLength == 0) {
        if (selectionStart <= 0) return;

        auto text = editor.Text();
        int32_t deleteLength = 1;
        if (selectionStart >= 2) {
            wchar_t previous = text.c_str()[selectionStart - 1];
            wchar_t beforePrevious = text.c_str()[selectionStart - 2];
            bool surrogatePair = previous >= 0xDC00 && previous <= 0xDFFF &&
                                 beforePrevious >= 0xD800 &&
                                 beforePrevious <= 0xDBFF;
            bool windowsNewline = previous == L'\n' && beforePrevious == L'\r';
            if (surrogatePair || windowsNewline) deleteLength = 2;
        }
        selectionStart -= deleteLength;
        editor.Select(selectionStart, deleteLength);
    }

    editor.SelectedText(L"");
    editor.Select(selectionStart, 0);
}

bool BetterPanelConsumeBackspaceMessage(MSG* message) {
    if (!message || message->message != WM_KEYDOWN ||
        message->wParam != VK_BACK) {
        return false;
    }

    try {
        std::lock_guard lock(g_betterPanelMutex);
        for (auto const& state : g_betterPanels) {
            if (!state || !state->dispatcher ||
                !state->dispatcher.HasThreadAccess()) {
                continue;
            }
            muxc::TextBox editor{nullptr};
            if (state->textEditing) {
                editor = state->textEditor.get();
            } else if (state->metadataEditing) {
                editor = state->metadataFocusedEditor.get();
            }
            if (editor) {
                BetterPanelApplyEditorBackspace(editor);
                message->message = WM_NULL;
                message->wParam = 0;
                message->lParam = 0;
                return true;
            }
        }
    } catch (...) {
    }
    return false;
}

using GetMessageW_t = decltype(&GetMessageW);
GetMessageW_t GetMessageW_Original;
BOOL WINAPI GetMessageW_Hook(LPMSG message,
                             HWND window,
                             UINT minimumMessage,
                             UINT maximumMessage) {
    BOOL result =
        GetMessageW_Original(message, window, minimumMessage, maximumMessage);
    if (result > 0) {
        BetterPanelHandlePowerMessage(message);
        if (!BetterPanelConsumeBackspaceMessage(message)) {
            BetterPanelQueueInteractiveRefresh(message);
        }
    }
    return result;
}

using PeekMessageW_t = decltype(&PeekMessageW);
PeekMessageW_t PeekMessageW_Original;
BOOL WINAPI PeekMessageW_Hook(LPMSG message,
                              HWND window,
                              UINT minimumMessage,
                              UINT maximumMessage,
                              UINT removeMessage) {
    BOOL result = PeekMessageW_Original(message, window, minimumMessage,
                                         maximumMessage, removeMessage);
    if (result && (removeMessage & PM_REMOVE)) {
        BetterPanelHandlePowerMessage(message);
        if (!BetterPanelConsumeBackspaceMessage(message)) {
            BetterPanelQueueInteractiveRefresh(message);
        }
    }
    return result;
}

std::wstring BetterPanelExtractPath(IShellBrowser* shellBrowser) {
    if (!shellBrowser) {
        return {};
    }

    winrt::com_ptr<IShellView> shellView;
    if (FAILED(shellBrowser->QueryActiveShellView(shellView.put())) ||
        !shellView) {
        return {};
    }

    winrt::com_ptr<IFolderView2> folderView;
    if (FAILED(shellView->QueryInterface(IID_PPV_ARGS(folderView.put()))) ||
        !folderView) {
        return {};
    }

    winrt::com_ptr<IShellItemArray> selection;
    if (FAILED(folderView->GetSelection(FALSE, selection.put())) ||
        !selection) {
        return {};
    }

    DWORD count = 0;
    if (FAILED(selection->GetCount(&count)) || count != 1) {
        return {};
    }

    winrt::com_ptr<IShellItem> item;
    if (FAILED(selection->GetItemAt(0, item.put())) || !item) {
        return {};
    }

    PWSTR rawPath = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &rawPath)) ||
        !rawPath) {
        return {};
    }

    std::wstring path(rawPath);
    CoTaskMemFree(rawPath);
    return path;
}

IShellBrowser* BetterPanelGetShellBrowser(HWND window) {
    if (!window) {
        return nullptr;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId != GetCurrentProcessId()) {
        return nullptr;
    }

    WCHAR className[64]{};
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        (_wcsicmp(className, L"CabinetWClass") != 0 &&
         _wcsicmp(className, L"ShellTabWindowClass") != 0)) {
        return nullptr;
    }

    return reinterpret_cast<IShellBrowser*>(
        SendMessageW(window, CWM_GETISHELLBROWSER, 0, 0));
}

std::wstring BetterPanelGetSelectedPath() {
    HWND focus = nullptr;
    GUITHREADINFO threadInfo{sizeof(threadInfo)};
    if (GetGUIThreadInfo(GetCurrentThreadId(), &threadInfo)) {
        focus = threadInfo.hwndFocus ? threadInfo.hwndFocus
                                     : threadInfo.hwndActive;
    }
    if (!focus) {
        focus = GetForegroundWindow();
    }
    if (!focus) {
        return {};
    }

    for (HWND window = focus; window; window = GetParent(window)) {
        if (auto browser = BetterPanelGetShellBrowser(window)) {
            auto path = BetterPanelExtractPath(browser);
            if (!path.empty()) {
                return path;
            }
        }
    }

    HWND root = GetAncestor(focus, GA_ROOT);
    for (HWND tab = nullptr; root &&
         (tab = FindWindowExW(root, tab, L"ShellTabWindowClass", nullptr));) {
        if (auto browser = BetterPanelGetShellBrowser(tab)) {
            auto path = BetterPanelExtractPath(browser);
            if (!path.empty()) {
                return path;
            }
        }
    }

    return {};
}

std::vector<std::wstring> BetterPanelExtractPaths(
    IShellBrowser* shellBrowser) {
    std::vector<std::wstring> paths;
    if (!shellBrowser) {
        return paths;
    }
    winrt::com_ptr<IShellView> shellView;
    if (FAILED(shellBrowser->QueryActiveShellView(shellView.put())) ||
        !shellView) {
        return paths;
    }
    winrt::com_ptr<IFolderView2> folderView;
    if (FAILED(shellView->QueryInterface(IID_PPV_ARGS(folderView.put()))) ||
        !folderView) {
        return paths;
    }
    winrt::com_ptr<IShellItemArray> selection;
    if (FAILED(folderView->GetSelection(FALSE, selection.put())) ||
        !selection) {
        return paths;
    }
    DWORD count = 0;
    if (FAILED(selection->GetCount(&count))) {
        return paths;
    }
    for (DWORD index = 0; index < count; index++) {
        winrt::com_ptr<IShellItem> item;
        if (FAILED(selection->GetItemAt(index, item.put())) || !item) {
            continue;
        }
        PWSTR rawPath = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &rawPath)) &&
            rawPath) {
            paths.emplace_back(rawPath);
        }
        CoTaskMemFree(rawPath);
    }
    return paths;
}

std::wstring BetterPanelExtractFolderPath(IShellBrowser* shellBrowser) {
    if (!shellBrowser) {
        return {};
    }
    winrt::com_ptr<IShellView> shellView;
    if (FAILED(shellBrowser->QueryActiveShellView(shellView.put())) ||
        !shellView) {
        return {};
    }
    winrt::com_ptr<IFolderView2> folderView;
    if (FAILED(shellView->QueryInterface(IID_PPV_ARGS(folderView.put()))) ||
        !folderView) {
        return {};
    }
    winrt::com_ptr<IShellItem> folderItem;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(folderItem.put()))) ||
        !folderItem) {
        return {};
    }
    PWSTR rawPath = nullptr;
    if (FAILED(folderItem->GetDisplayName(SIGDN_FILESYSPATH, &rawPath)) ||
        !rawPath) {
        return {};
    }
    std::wstring path(rawPath);
    CoTaskMemFree(rawPath);
    return path;
}

thread_local HWND g_betterLastFocusedTabWindow = nullptr;
thread_local HWND g_betterQueryExplorerWindow = nullptr;

HWND BetterPanelGetExplorerWindowForElement(FrameworkElement const& element) {
    // ContentIslandEnvironment::AppWindowId is an identifier, not an HWND.
    // Treating its numeric Value as a window handle can associate the panel
    // with an unrelated/invalid root and makes selection queries read Home or
    // a background tab. DetailsPaneControl runs on its owning Explorer
    // window's UI thread, so resolve the real CabinetWClass on that thread.
    auto isExplorerRoot = [](HWND window) {
        if (!window) return false;
        WCHAR className[64]{};
        DWORD processId = 0;
        return GetWindowThreadProcessId(window, &processId) != 0 &&
               processId == GetCurrentProcessId() &&
               GetClassNameW(window, className, ARRAYSIZE(className)) != 0 &&
               _wcsicmp(className, L"CabinetWClass") == 0;
    };

    for (HWND candidate : {GetActiveWindow(), GetForegroundWindow()}) {
        HWND root = candidate ? GetAncestor(candidate, GA_ROOT) : nullptr;
        if (isExplorerRoot(root) &&
            GetWindowThreadProcessId(root, nullptr) == GetCurrentThreadId()) {
            return root;
        }
    }

    struct FindContext {
        HWND window = nullptr;
    } context;
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM parameter) -> BOOL {
            auto context = reinterpret_cast<FindContext*>(parameter);
            WCHAR className[64]{};
            if (IsWindowVisible(window) &&
                GetClassNameW(window, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"CabinetWClass") == 0) {
                context->window = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
    return context.window;
}

struct BetterPanelExplorerWindowScope {
    HWND previous = nullptr;

    explicit BetterPanelExplorerWindowScope(HWND window)
        : previous(g_betterQueryExplorerWindow) {
        g_betterQueryExplorerWindow = window;
    }

    ~BetterPanelExplorerWindowScope() {
        g_betterQueryExplorerWindow = previous;
    }
};

HWND BetterPanelGetFocusedTabWindow() {
    HWND focus = nullptr;
    GUITHREADINFO threadInfo{sizeof(threadInfo)};
    if (GetGUIThreadInfo(GetCurrentThreadId(), &threadInfo)) {
        focus = threadInfo.hwndFocus ? threadInfo.hwndFocus
                                     : threadInfo.hwndActive;
    }
    if (!focus) {
        focus = GetForegroundWindow();
    }
    for (HWND window = focus; window; window = GetParent(window)) {
        WCHAR className[64]{};
        if (GetClassNameW(window, className, ARRAYSIZE(className)) &&
            _wcsicmp(className, L"ShellTabWindowClass") == 0) {
            g_betterLastFocusedTabWindow = window;
            return window;
        }
    }

    // Focus moves into the injected details pane when its controls are used.
    // Resolve the currently visible tab before consulting the cache. Returning
    // a cached tab here first could permanently pin queries to a background
    // Home tab after the user switched tabs or navigated elsewhere.
    HWND root = g_betterQueryExplorerWindow &&
                        IsWindow(g_betterQueryExplorerWindow)
                    ? g_betterQueryExplorerWindow
                    : GetAncestor(focus ? focus : GetForegroundWindow(),
                                  GA_ROOT);
    for (HWND tab = nullptr; root &&
         (tab = FindWindowExW(root, tab, L"ShellTabWindowClass", nullptr));) {
        if (IsWindowVisible(tab) && BetterPanelGetShellBrowser(tab)) {
            g_betterLastFocusedTabWindow = tab;
            return tab;
        }
    }
    if (root && g_betterLastFocusedTabWindow &&
        GetAncestor(g_betterLastFocusedTabWindow, GA_ROOT) == root &&
        IsWindow(g_betterLastFocusedTabWindow) &&
        BetterPanelGetShellBrowser(g_betterLastFocusedTabWindow)) {
        return g_betterLastFocusedTabWindow;
    }
    return nullptr;
}

bool BetterPanelNavigateCurrentTab(std::wstring const& path) {
    if (path.empty()) return false;
    HWND tab = BetterPanelGetFocusedTabWindow();
    auto browser = BetterPanelGetShellBrowser(tab);
    if (!browser) return false;
    PIDLIST_ABSOLUTE itemId = nullptr;
    HRESULT result = SHParseDisplayName(path.c_str(), nullptr, &itemId, 0,
                                        nullptr);
    if (FAILED(result) || !itemId) return false;
    result = browser->BrowseObject(itemId, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
    CoTaskMemFree(itemId);
    return SUCCEEDED(result);
}

struct BetterPanelTransferContext {
    std::vector<std::wstring> sources;
    std::wstring destination;
};

BetterPanelTransferContext BetterPanelResolveTransferContext(
    std::wstring_view preferredSource,
    std::vector<std::wstring> const& fallbackSources) {
    struct TabInfo {
        HWND window = nullptr;
        std::wstring folder;
        std::vector<std::wstring> selection;
    };
    BetterPanelTransferContext context;
    HWND activeTab = BetterPanelGetFocusedTabWindow();
    HWND root = activeTab ? GetAncestor(activeTab, GA_ROOT)
                          : GetForegroundWindow();
    std::vector<TabInfo> tabs;
    for (HWND tab = nullptr; root &&
         (tab = FindWindowExW(root, tab, L"ShellTabWindowClass", nullptr));) {
        if (auto browser = BetterPanelGetShellBrowser(tab)) {
            auto folder = BetterPanelExtractFolderPath(browser);
            if (!folder.empty()) {
                tabs.push_back(
                    TabInfo{tab, std::move(folder),
                            BetterPanelExtractPaths(browser)});
            }
        }
    }
    if (tabs.empty()) {
        return context;
    }

    size_t activeIndex = 0;
    for (size_t index = 0; index < tabs.size(); index++) {
        if (tabs[index].window == activeTab) {
            activeIndex = index;
            break;
        }
    }
    size_t sourceIndex = tabs.size();
    if (!preferredSource.empty()) {
        for (size_t index = 0; index < tabs.size(); index++) {
            if (std::find(tabs[index].selection.begin(),
                          tabs[index].selection.end(), preferredSource) !=
                tabs[index].selection.end()) {
                sourceIndex = index;
                break;
            }
        }
    }
    if (sourceIndex == tabs.size()) {
        for (size_t index = 0; index < tabs.size(); index++) {
            if (!tabs[index].selection.empty()) {
                sourceIndex = index;
                break;
            }
        }
    }

    if (sourceIndex < tabs.size()) {
        context.sources = tabs[sourceIndex].selection;
    } else {
        context.sources = fallbackSources;
    }
    if (context.sources.empty()) {
        return context;
    }

    // These actions belong to the destination folder view only. When the
    // source tab is active, its regular file-specific controls stay clean.
    if (sourceIndex != activeIndex) {
        context.destination = tabs[activeIndex].folder;
        if (tabs[activeIndex].selection.size() == 1) {
            DWORD attributes = GetFileAttributesW(
                tabs[activeIndex].selection[0].c_str());
            if (attributes != INVALID_FILE_ATTRIBUTES &&
                (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
                context.destination = tabs[activeIndex].selection[0];
            }
        }
    }
    return context;
}

struct BetterPanelSelectionCache {
    HWND tab = nullptr;
    ULONGLONG tick = 0;
    std::vector<std::wstring> paths;
};

struct BetterPanelFolderCache {
    HWND tab = nullptr;
    ULONGLONG tick = 0;
    std::wstring path;
};

thread_local BetterPanelSelectionCache g_betterSelectionCache;
thread_local BetterPanelFolderCache g_betterFolderCache;

void BetterPanelInvalidateExplorerQueryCaches() {
    g_betterSelectionCache.tick = 0;
    g_betterFolderCache.tick = 0;
}

std::vector<std::wstring> BetterPanelGetActiveSelectionPaths(
    bool allowShortCache = true) {
    auto& cache = g_betterSelectionCache;
    HWND activeTab = BetterPanelGetFocusedTabWindow();
    if (!activeTab) {
        return {};
    }
    ULONGLONG now = GetTickCount64();
    if (allowShortCache && cache.tab == activeTab &&
        now - cache.tick <= 100) {
        return cache.paths;
    }
    auto paths = BetterPanelExtractPaths(BetterPanelGetShellBrowser(activeTab));
    cache.tab = activeTab;
    cache.tick = now;
    cache.paths = paths;
    return paths;
}

std::wstring BetterPanelFileName(std::wstring_view path) {
    size_t slash = path.find_last_of(L"\\/");
    return std::wstring(path.substr(slash == std::wstring_view::npos
                                        ? 0
                                        : slash + 1));
}

bool BetterPanelIsAudioFile(std::wstring_view path) {
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring_view::npos) {
        return false;
    }

    std::wstring extension(path.substr(dot));
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   towlower);
    return extension == L".mp3" || extension == L".ogg" ||
           extension == L".oga" || extension == L".opus" ||
           extension == L".flac" || extension == L".wav" ||
           extension == L".m4a" || extension == L".aac" ||
           extension == L".wma";
}

bool BetterPanelIsVideoFile(std::wstring_view path) {
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring_view::npos) {
        return false;
    }
    std::wstring extension(path.substr(dot));
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   towlower);
    return extension == L".mov" || extension == L".mp4" ||
           extension == L".m4v" || extension == L".mkv" ||
           extension == L".avi" || extension == L".wmv" ||
           extension == L".webm" || extension == L".mpeg" ||
           extension == L".mpg" || extension == L".ts" ||
           extension == L".mts" || extension == L".m2ts";
}

bool BetterPanelIsGifFile(std::wstring_view path) {
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring_view::npos) {
        return false;
    }
    std::wstring extension(path.substr(dot));
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   towlower);
    return extension == L".gif";
}

bool BetterPanelIsArchiveFile(std::wstring_view path) {
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring_view::npos) {
        return false;
    }
    std::wstring extension(path.substr(dot));
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   towlower);
    static constexpr std::wstring_view archiveExtensions[] = {
        L".zip", L".rar", L".7z",  L".tar", L".gz",  L".gzip",
        L".bz2", L".xz",  L".tgz", L".tbz", L".tbz2", L".txz",
        L".cab", L".iso", L".wim", L".arj", L".lzh", L".lha",
        L".zst", L".zipx"};
    return std::find(std::begin(archiveExtensions),
                     std::end(archiveExtensions), extension) !=
           std::end(archiveExtensions);
}

bool BetterPanelIsTextFile(std::wstring_view path) {
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring_view::npos) {
        return false;
    }
    std::wstring extension(path.substr(dot));
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   towlower);
    static constexpr std::wstring_view textExtensions[] = {
        L".txt",        L".md",       L".markdown", L".log",
        L".csv",        L".tsv",      L".json",     L".jsonc",
        L".xml",        L".xaml",     L".yaml",     L".yml",
        L".ini",        L".cfg",      L".conf",     L".config",
        L".properties", L".reg",      L".url",      L".bat",
        L".cmd",        L".ps1",      L".psm1",     L".psd1",
        L".sh",         L".py",       L".js",       L".jsx",
        L".ts",         L".tsx",      L".css",      L".scss",
        L".less",       L".html",     L".htm",      L".c",
        L".h",          L".cpp",      L".hpp",      L".cc",
        L".cs",         L".java",     L".kt",       L".kts",
        L".rs",         L".go",       L".php",      L".rb",
        L".swift",      L".sql",      L".toml",     L".gradle",
        L".sln",        L".vcxproj",  L".csproj",   L".props",
        L".targets",    L".gitignore", L".gitattributes"};
    return std::find(std::begin(textExtensions), std::end(textExtensions),
                     extension) != std::end(textExtensions);
}

bool BetterPanelIsPdfFile(std::wstring_view path) {
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring_view::npos) {
        return false;
    }
    std::wstring extension(path.substr(dot));
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   towlower);
    return extension == L".pdf";
}

bool BetterPanelHasPrintHandler(std::wstring_view path) {
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring_view::npos) {
        return false;
    }
    std::wstring extension(path.substr(dot));
    WCHAR command[2048]{};
    DWORD commandLength = static_cast<DWORD>(std::size(command));
    return SUCCEEDED(AssocQueryStringW(
               ASSOCF_INIT_IGNOREUNKNOWN, ASSOCSTR_COMMAND,
               extension.c_str(), L"print", command, &commandLength)) &&
           command[0] != L'\0';
}

bool BetterPanelPrintFile(std::wstring const& path) {
    if (path.empty() || !BetterPanelHasPrintHandler(path)) {
        return false;
    }
    SHELLEXECUTEINFOW executeInfo{sizeof(executeInfo)};
    executeInfo.fMask = SEE_MASK_ASYNCOK;
    executeInfo.hwnd = GetForegroundWindow();
    executeInfo.lpVerb = L"print";
    executeInfo.lpFile = path.c_str();
    executeInfo.nShow = SW_SHOWNORMAL;
    return ShellExecuteExW(&executeInfo) != FALSE;
}

struct BetterPanelTextFileData {
    bool success = false;
    bool editable = false;
    std::wstring text;
    std::wstring newline = L"\r\n";
    std::wstring message;
    BetterPanelTextEncoding encoding = BetterPanelTextEncoding::Utf8;
    FILETIME lastWriteTime{};
    uint64_t size = 0;
};

constexpr uint64_t kBetterPanelTextPreviewLimit = 2 * 1024 * 1024;
constexpr uint64_t kBetterPanelTextEditLimit = 1024 * 1024;

std::wstring BetterPanelTextEncodingName(BetterPanelTextEncoding encoding) {
    switch (encoding) {
        case BetterPanelTextEncoding::Utf8:
            return L"UTF-8";
        case BetterPanelTextEncoding::Utf8Bom:
            return L"UTF-8 BOM";
        case BetterPanelTextEncoding::Utf16Le:
            return L"UTF-16 LE";
        case BetterPanelTextEncoding::Utf16Be:
            return L"UTF-16 BE";
        case BetterPanelTextEncoding::Ansi:
            return L"ANSI";
    }
    return L"Text";
}

std::wstring BetterPanelFormatByteSize(uint64_t size) {
    WCHAR buffer[48]{};
    constexpr uint64_t tebibyte = 1024ULL * 1024 * 1024 * 1024;
    constexpr uint64_t gibibyte = 1024ULL * 1024 * 1024;
    if (size >= tebibyte) {
        swprintf_s(buffer, L"%.2f TB",
                   static_cast<double>(size) / static_cast<double>(tebibyte));
    } else if (size >= gibibyte) {
        swprintf_s(buffer, L"%.2f GB",
                   static_cast<double>(size) / static_cast<double>(gibibyte));
    } else if (size >= 1024 * 1024) {
        swprintf_s(buffer, L"%.2f MB",
                   static_cast<double>(size) / (1024.0 * 1024.0));
    } else if (size >= 1024) {
        swprintf_s(buffer, L"%.2f KB", static_cast<double>(size) / 1024.0);
    } else {
        swprintf_s(buffer, L"%llu bytes",
                   static_cast<unsigned long long>(size));
    }
    return buffer;
}

std::wstring BetterPanelGetActiveFolderPath() {
    auto& cache = g_betterFolderCache;
    HWND activeTab = BetterPanelGetFocusedTabWindow();
    if (!activeTab) return {};
    ULONGLONG now = GetTickCount64();
    if (cache.tab == activeTab && now - cache.tick <= 100) {
        return cache.path;
    }
    auto path = BetterPanelExtractFolderPath(BetterPanelGetShellBrowser(activeTab));
    cache.tab = activeTab;
    cache.tick = now;
    cache.path = path;
    return path;
}

bool BetterPanelCopyText(std::wstring const& text) {
    if (text.empty() || !OpenClipboard(GetForegroundWindow())) return false;
    bool copied = false;
    if (EmptyClipboard()) {
        SIZE_T bytes = (text.size() + 1) * sizeof(WCHAR);
        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (memory) {
            if (void* target = GlobalLock(memory)) {
                memcpy(target, text.c_str(), bytes);
                GlobalUnlock(memory);
                copied = SetClipboardData(CF_UNICODETEXT, memory) != nullptr;
            }
            if (!copied) GlobalFree(memory);
        }
    }
    CloseClipboard();
    return copied;
}

std::wstring BetterPanelFormatFileTime(FILETIME fileTime) {
    FILETIME localTime{};
    SYSTEMTIME systemTime{};
    if (!FileTimeToLocalFileTime(&fileTime, &localTime) ||
        !FileTimeToSystemTime(&localTime, &systemTime)) {
        return L"Unknown";
    }
    WCHAR date[64]{};
    WCHAR time[64]{};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &systemTime,
                    nullptr, date, ARRAYSIZE(date), nullptr);
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &systemTime,
                    nullptr, time, ARRAYSIZE(time));
    return std::wstring(date) + L" " + time;
}

bool BetterPanelIsImageFile(std::wstring_view path) {
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring_view::npos) return false;
    std::wstring extension(path.substr(dot));
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   towlower);
    static constexpr std::wstring_view extensions[] = {
        L".jpg", L".jpeg", L".jpe", L".png", L".bmp", L".tif",
        L".tiff", L".gif", L".webp", L".heic", L".heif", L".avif"};
    return std::find(std::begin(extensions), std::end(extensions), extension) !=
           std::end(extensions);
}

std::wstring BetterPanelSha256(
    std::wstring const& path, std::weak_ptr<BetterPanelState> weakState,
    uint64_t generation) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE |
                                  FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) return {};

    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    DWORD objectLength = 0;
    DWORD bytesWritten = 0;
    std::vector<UCHAR> hashObject;
    std::vector<UCHAR> digest(32);
    std::vector<UCHAR> buffer(1024 * 1024);
    bool success = false;
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM,
                                    nullptr, 0) >= 0 &&
        BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH,
                          reinterpret_cast<PUCHAR>(&objectLength),
                          sizeof(objectLength), &bytesWritten, 0) >= 0) {
        hashObject.resize(objectLength);
        if (BCryptCreateHash(algorithm, &hash, hashObject.data(), objectLength,
                             nullptr, 0, 0) >= 0) {
            DWORD read = 0;
            success = true;
            while (true) {
                auto state = weakState.lock();
                if (!state ||
                    state->insightsGeneration.load(std::memory_order_relaxed) !=
                        generation) {
                    success = false;
                    break;
                }
                if (!ReadFile(file, buffer.data(),
                              static_cast<DWORD>(buffer.size()), &read,
                              nullptr)) {
                    success = false;
                    break;
                }
                if (!read) break;
                if (BCryptHashData(hash, buffer.data(), read, 0) < 0) {
                    success = false;
                    break;
                }
            }
            if (success && BCryptFinishHash(hash, digest.data(),
                                            static_cast<ULONG>(digest.size()),
                                            0) < 0) {
                success = false;
            }
        }
    }
    if (hash) BCryptDestroyHash(hash);
    if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
    CloseHandle(file);
    if (!success) return {};

    static constexpr WCHAR digits[] = L"0123456789abcdef";
    std::wstring result;
    result.reserve(64);
    for (UCHAR byte : digest) {
        result.push_back(digits[byte >> 4]);
        result.push_back(digits[byte & 0x0F]);
    }
    return result;
}

struct BetterPanelInsightData {
    bool success = false;
    bool directory = false;
    uint64_t size = 0;
    FILETIME modified{};
    std::wstring path;
    std::wstring name;
};

BetterPanelInsightData BetterPanelReadInsightData(std::wstring const& path) {
    BetterPanelInsightData result;
    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &data)) {
        return result;
    }
    result.success = true;
    result.path = path;
    result.name = BetterPanelFileName(path);
    result.directory = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    result.size = (static_cast<uint64_t>(data.nFileSizeHigh) << 32) |
                  data.nFileSizeLow;
    result.modified = data.ftLastWriteTime;
    return result;
}

struct BetterPanelMultiSummary {
    size_t selectedItems = 0;
    size_t files = 0;
    size_t folders = 0;
    size_t containedFiles = 0;
    size_t subfolders = 0;
    uint64_t totalSize = 0;
    std::map<std::wstring, size_t> types;
    FILETIME oldest{};
    FILETIME newest{};
    bool hasDate = false;
    bool incomplete = false;
    size_t scannedEntries = 0;
};

struct BetterPanelFolderSummaryCacheEntry {
    BetterPanelMultiSummary summary;
    ULONGLONG tick = 0;
};

std::mutex g_betterFolderSummaryCacheMutex;
std::unordered_map<std::wstring, BetterPanelFolderSummaryCacheEntry>
    g_betterFolderSummaryCache;

bool BetterPanelGetCachedFolderSummary(
    std::wstring const& path, BetterPanelMultiSummary& summary) {
    std::lock_guard lock(g_betterFolderSummaryCacheMutex);
    auto found = g_betterFolderSummaryCache.find(path);
    if (found == g_betterFolderSummaryCache.end() ||
        GetTickCount64() - found->second.tick > 60000) {
        return false;
    }
    summary = found->second.summary;
    return true;
}

void BetterPanelCacheFolderSummary(
    std::wstring const& path, BetterPanelMultiSummary const& summary) {
    if (summary.incomplete) return;
    std::lock_guard lock(g_betterFolderSummaryCacheMutex);
    if (g_betterFolderSummaryCache.size() >= 64) {
        auto oldest = std::min_element(
            g_betterFolderSummaryCache.begin(),
            g_betterFolderSummaryCache.end(), [](auto const& left,
                                                  auto const& right) {
                return left.second.tick < right.second.tick;
            });
        if (oldest != g_betterFolderSummaryCache.end()) {
            g_betterFolderSummaryCache.erase(oldest);
        }
    }
    g_betterFolderSummaryCache[path] = {summary, GetTickCount64()};
}

BetterPanelMultiSummary BetterPanelBuildFolderSummary(
    std::wstring const& folder, std::weak_ptr<BetterPanelState> weakState,
    uint64_t generation, size_t maxEntries = 75000,
    ULONGLONG deadline = 0) {
    BetterPanelMultiSummary summary;
    if (!deadline) deadline = GetTickCount64() + 3000;
    std::vector<std::wstring> pending{folder};
    size_t visited = 0;
    while (!pending.empty() && visited < maxEntries &&
           GetTickCount64() < deadline) {
        auto state = weakState.lock();
        if (!state ||
            state->insightsGeneration.load(std::memory_order_relaxed) !=
                generation) {
            return {};
        }
        std::wstring current = std::move(pending.back());
        pending.pop_back();
        std::wstring pattern = current;
        if (!pattern.ends_with(L'\\')) pattern += L'\\';
        pattern += L"*";
        WIN32_FIND_DATAW findData{};
        HANDLE find = FindFirstFileExW(pattern.c_str(), FindExInfoBasic,
                                       &findData, FindExSearchNameMatch,
                                       nullptr, FIND_FIRST_EX_LARGE_FETCH);
        if (find == INVALID_HANDLE_VALUE) {
            summary.incomplete = true;
            continue;
        }
        do {
            if (wcscmp(findData.cFileName, L".") == 0 ||
                wcscmp(findData.cFileName, L"..") == 0) continue;
            ++visited;
            if ((visited & 0x3F) == 0) {
                auto state = weakState.lock();
                if (!state ||
                    state->insightsGeneration.load(
                        std::memory_order_relaxed) != generation) {
                    FindClose(find);
                    return {};
                }
                if (GetTickCount64() >= deadline) break;
            }
            std::wstring child = current;
            if (!child.ends_with(L'\\')) child += L'\\';
            child += findData.cFileName;
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ++summary.folders;
                if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                    pending.push_back(std::move(child));
                }
            } else {
                ++summary.files;
                summary.totalSize +=
                    (static_cast<uint64_t>(findData.nFileSizeHigh) << 32) |
                    findData.nFileSizeLow;
                std::wstring name = findData.cFileName;
                size_t dot = name.find_last_of(L'.');
                std::wstring type = dot == std::wstring::npos
                                        ? L"No extension"
                                        : name.substr(dot + 1);
                std::transform(type.begin(), type.end(), type.begin(), towupper);
                summary.types[type]++;
            }
        } while (FindNextFileW(find, &findData) && visited < maxEntries &&
                 GetTickCount64() < deadline);
        FindClose(find);
    }
    summary.scannedEntries = visited;
    if (!pending.empty() || visited >= maxEntries ||
        GetTickCount64() >= deadline) {
        summary.incomplete = true;
    }
    return summary;
}

BetterPanelMultiSummary BetterPanelBuildMultiSummary(
    std::vector<std::wstring> const& paths,
    std::weak_ptr<BetterPanelState> weakState,
    uint64_t generation) {
    BetterPanelMultiSummary summary;
    summary.selectedItems = paths.size();
    size_t remainingEntries = 50000;
    ULONGLONG deadline = GetTickCount64() + 2000;
    for (auto const& path : paths) {
        auto state = weakState.lock();
        if (!state || state->insightsGeneration.load(
                          std::memory_order_relaxed) != generation) {
            return {};
        }
        WIN32_FILE_ATTRIBUTE_DATA data{};
        if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &data))
            continue;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            ++summary.folders;
            summary.types[L"Folders"]++;
            if (!remainingEntries || GetTickCount64() >= deadline) {
                summary.incomplete = true;
                continue;
            }
            BetterPanelMultiSummary folderSummary;
            bool cached =
                BetterPanelGetCachedFolderSummary(path, folderSummary);
            if (!cached) {
                folderSummary = BetterPanelBuildFolderSummary(
                    path, weakState, generation, remainingEntries, deadline);
                BetterPanelCacheFolderSummary(path, folderSummary);
            }
            remainingEntries -=
                cached ? 0 : std::min(remainingEntries,
                                      folderSummary.scannedEntries);
            summary.containedFiles += folderSummary.files;
            summary.subfolders += folderSummary.folders;
            summary.totalSize += folderSummary.totalSize;
            summary.incomplete = summary.incomplete || folderSummary.incomplete;
            for (auto const& [type, count] : folderSummary.types) {
                summary.types[type] += count;
            }
        } else {
            ++summary.files;
            summary.totalSize +=
                (static_cast<uint64_t>(data.nFileSizeHigh) << 32) |
                data.nFileSizeLow;
            size_t dot = path.find_last_of(L'.');
            std::wstring type = dot == std::wstring::npos
                                    ? L"No extension"
                                    : path.substr(dot + 1);
            std::transform(type.begin(), type.end(), type.begin(), towupper);
            summary.types[type]++;
        }
        if (!summary.hasDate ||
            CompareFileTime(&data.ftLastWriteTime, &summary.oldest) < 0) {
            summary.oldest = data.ftLastWriteTime;
        }
        if (!summary.hasDate ||
            CompareFileTime(&data.ftLastWriteTime, &summary.newest) > 0) {
            summary.newest = data.ftLastWriteTime;
        }
        summary.hasDate = true;
    }
    return summary;
}

std::wstring BetterPanelFormatMultiSummary(
    BetterPanelMultiSummary const& summary) {
    std::wstring text = std::to_wstring(summary.selectedItems) +
                        L" selected  •  " + std::to_wstring(summary.files) +
                        L" files  •  " + std::to_wstring(summary.folders) +
                        L" folders\nCombined file size: " +
                        BetterPanelFormatByteSize(summary.totalSize);
    if (summary.folders) {
        text += L"\nInside folders: " +
                std::to_wstring(summary.containedFiles) + L" files, " +
                std::to_wstring(summary.subfolders) + L" subfolders";
    }
    if (!summary.types.empty()) {
        text += L"\nTypes: ";
        size_t shown = 0;
        for (auto const& [type, count] : summary.types) {
            if (shown++) text += L", ";
            text += type + L" × " + std::to_wstring(count);
            if (shown == 6 && summary.types.size() > shown) {
                text += L", +" + std::to_wstring(summary.types.size() - shown) +
                        L" more";
                break;
            }
        }
    }
    if (summary.hasDate) {
        text += L"\nModified: " + BetterPanelFormatFileTime(summary.oldest);
        if (CompareFileTime(&summary.oldest, &summary.newest) != 0) {
            text += L" – " + BetterPanelFormatFileTime(summary.newest);
        }
    }
    return text;
}

std::wstring BetterPanelFormatTypes(
    std::map<std::wstring, size_t> const& types) {
    std::wstring text;
    size_t shown = 0;
    for (auto const& [type, count] : types) {
        if (shown++) text += L", ";
        text += type + L" × " + std::to_wstring(count);
        if (shown == 8 && types.size() > shown) {
            text += L", +" + std::to_wstring(types.size() - shown) + L" more";
            break;
        }
    }
    return text.empty() ? L"None" : text;
}

bool BetterPanelDecodeMultiByte(UINT codePage, DWORD flags,
                                const char* bytes, int length,
                                std::wstring& text) {
    if (!length) {
        text.clear();
        return true;
    }
    int required = MultiByteToWideChar(codePage, flags, bytes, length, nullptr,
                                       0);
    if (required <= 0) {
        return false;
    }
    text.resize(required);
    return MultiByteToWideChar(codePage, flags, bytes, length, text.data(),
                               required) == required;
}

BetterPanelTextFileData BetterPanelReadTextFile(std::wstring const& path) {
    BetterPanelTextFileData result;
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE |
                                  FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        result.message = L"Preview unavailable (error " +
                         std::to_wstring(GetLastError()) + L")";
        return result;
    }

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file, &size) || size.QuadPart < 0) {
        result.message = L"Could not read the file size";
        CloseHandle(file);
        return result;
    }
    result.size = static_cast<uint64_t>(size.QuadPart);
    GetFileTime(file, nullptr, nullptr, &result.lastWriteTime);
    if (result.size > kBetterPanelTextPreviewLimit) {
        result.message = L"Preview limited to text files up to 2 MB";
        CloseHandle(file);
        return result;
    }

    std::vector<uint8_t> bytes(static_cast<size_t>(result.size));
    DWORD total = 0;
    while (total < bytes.size()) {
        DWORD chunk = 0;
        DWORD requested = static_cast<DWORD>(
            std::min<size_t>(bytes.size() - total, 1024 * 1024));
        if (!ReadFile(file, bytes.data() + total, requested, &chunk, nullptr)) {
            result.message = L"Could not read the file";
            CloseHandle(file);
            return result;
        }
        if (!chunk) break;
        total += chunk;
    }
    CloseHandle(file);
    bytes.resize(total);
    result.size = total;

    size_t offset = 0;
    if (bytes.size() >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB &&
        bytes[2] == 0xBF) {
        result.encoding = BetterPanelTextEncoding::Utf8Bom;
        offset = 3;
    } else if (bytes.size() >= 2 && bytes[0] == 0xFF &&
               bytes[1] == 0xFE) {
        result.encoding = BetterPanelTextEncoding::Utf16Le;
        offset = 2;
    } else if (bytes.size() >= 2 && bytes[0] == 0xFE &&
               bytes[1] == 0xFF) {
        result.encoding = BetterPanelTextEncoding::Utf16Be;
        offset = 2;
    }

    if (result.encoding == BetterPanelTextEncoding::Utf16Le ||
        result.encoding == BetterPanelTextEncoding::Utf16Be) {
        size_t remaining = bytes.size() - offset;
        if (remaining % 2) {
            result.message = L"Invalid UTF-16 text file";
            return result;
        }
        result.text.resize(remaining / 2);
        for (size_t index = 0; index < result.text.size(); index++) {
            uint8_t first = bytes[offset + index * 2];
            uint8_t second = bytes[offset + index * 2 + 1];
            result.text[index] = static_cast<wchar_t>(
                result.encoding == BetterPanelTextEncoding::Utf16Le
                    ? first | (second << 8)
                    : (first << 8) | second);
        }
    } else {
        auto begin = reinterpret_cast<const char*>(bytes.data() + offset);
        int length = static_cast<int>(bytes.size() - offset);
        size_t nulCount = std::count(bytes.begin() + offset, bytes.end(), 0);
        if (nulCount > std::max<size_t>(2, (bytes.size() - offset) / 100)) {
            result.message = L"This file appears to contain binary data";
            return result;
        }
        if (!BetterPanelDecodeMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, begin,
                                        length, result.text)) {
            result.encoding = BetterPanelTextEncoding::Ansi;
            if (!BetterPanelDecodeMultiByte(CP_ACP, 0, begin, length,
                                            result.text)) {
                result.message = L"Unsupported text encoding";
                return result;
            }
        }
    }

    result.success = true;
    if (result.text.find(L"\r\n") != std::wstring::npos) {
        result.newline = L"\r\n";
    } else if (result.text.find(L'\n') != std::wstring::npos) {
        result.newline = L"\n";
    } else if (result.text.find(L'\r') != std::wstring::npos) {
        result.newline = L"\r";
    }
    result.editable = result.size <= kBetterPanelTextEditLimit;
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes != INVALID_FILE_ATTRIBUTES &&
        (attributes & FILE_ATTRIBUTE_READONLY)) {
        result.editable = false;
        result.message = L"Read-only file";
    } else if (!result.editable) {
        result.message = L"Read-only preview: editing is limited to 1 MB";
    }
    return result;
}

bool BetterPanelGetFileStamp(std::wstring const& path, uint64_t& size,
                             FILETIME& lastWriteTime) {
    HANDLE file = CreateFileW(path.c_str(), FILE_READ_ATTRIBUTES,
                              FILE_SHARE_READ | FILE_SHARE_WRITE |
                                  FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER fileSize{};
    bool success = GetFileSizeEx(file, &fileSize) &&
                   GetFileTime(file, nullptr, nullptr, &lastWriteTime);
    CloseHandle(file);
    if (success) size = static_cast<uint64_t>(fileSize.QuadPart);
    return success;
}

std::wstring BetterPanelNormalizeLineEndings(
    std::wstring_view text, std::wstring_view newline) {
    std::wstring normalized;
    normalized.reserve(text.size() + 16);
    for (size_t index = 0; index < text.size(); index++) {
        if (text[index] == L'\r') {
            if (index + 1 < text.size() && text[index + 1] == L'\n') {
                index++;
            }
            normalized.append(newline);
        } else if (text[index] == L'\n') {
            normalized.append(newline);
        } else {
            normalized.push_back(text[index]);
        }
    }
    return normalized;
}

bool BetterPanelEncodeText(std::wstring const& text,
                           BetterPanelTextEncoding& encoding,
                           std::vector<uint8_t>& bytes) {
    bytes.clear();
    if (encoding == BetterPanelTextEncoding::Utf16Le ||
        encoding == BetterPanelTextEncoding::Utf16Be) {
        bytes.reserve(2 + text.size() * 2);
        bytes.push_back(encoding == BetterPanelTextEncoding::Utf16Le ? 0xFF
                                                                     : 0xFE);
        bytes.push_back(encoding == BetterPanelTextEncoding::Utf16Le ? 0xFE
                                                                     : 0xFF);
        for (wchar_t character : text) {
            uint8_t low = static_cast<uint8_t>(character & 0xFF);
            uint8_t high = static_cast<uint8_t>((character >> 8) & 0xFF);
            if (encoding == BetterPanelTextEncoding::Utf16Le) {
                bytes.push_back(low);
                bytes.push_back(high);
            } else {
                bytes.push_back(high);
                bytes.push_back(low);
            }
        }
        return true;
    }

    UINT codePage = encoding == BetterPanelTextEncoding::Ansi ? CP_ACP
                                                               : CP_UTF8;
    BOOL usedDefault = FALSE;
    int required = WideCharToMultiByte(
        codePage, encoding == BetterPanelTextEncoding::Ansi
                      ? WC_NO_BEST_FIT_CHARS
                      : WC_ERR_INVALID_CHARS,
        text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr,
        encoding == BetterPanelTextEncoding::Ansi ? &usedDefault : nullptr);
    if (required <= 0 || usedDefault) {
        if (encoding != BetterPanelTextEncoding::Ansi) return false;
        encoding = BetterPanelTextEncoding::Utf8Bom;
        return BetterPanelEncodeText(text, encoding, bytes);
    }
    size_t prefix = encoding == BetterPanelTextEncoding::Utf8Bom ? 3 : 0;
    bytes.resize(prefix + required);
    if (prefix) {
        bytes[0] = 0xEF;
        bytes[1] = 0xBB;
        bytes[2] = 0xBF;
    }
    usedDefault = FALSE;
    return WideCharToMultiByte(
               codePage, encoding == BetterPanelTextEncoding::Ansi
                             ? WC_NO_BEST_FIT_CHARS
                             : WC_ERR_INVALID_CHARS,
               text.data(), static_cast<int>(text.size()),
               reinterpret_cast<char*>(bytes.data() + prefix), required,
               nullptr,
               encoding == BetterPanelTextEncoding::Ansi ? &usedDefault
                                                          : nullptr) ==
               required &&
           !usedDefault;
}

bool BetterPanelWriteTextFile(std::wstring const& path,
                              std::wstring const& text,
                              BetterPanelTextEncoding& encoding,
                              DWORD& error) {
    std::vector<uint8_t> bytes;
    if (!BetterPanelEncodeText(text, encoding, bytes)) {
        error = ERROR_NO_UNICODE_TRANSLATION;
        return false;
    }
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                              nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        error = GetLastError();
        return false;
    }
    DWORD total = 0;
    bool success = true;
    while (total < bytes.size()) {
        DWORD written = 0;
        DWORD requested = static_cast<DWORD>(
            std::min<size_t>(bytes.size() - total, 1024 * 1024));
        if (!WriteFile(file, bytes.data() + total, requested, &written,
                       nullptr) ||
            !written) {
            success = false;
            error = GetLastError();
            break;
        }
        total += written;
    }
    if (success && !FlushFileBuffers(file)) {
        success = false;
        error = GetLastError();
    }
    CloseHandle(file);
    return success;
}

bool BetterPanelAppendExtractMenuItems(HMENU source, HMENU destination,
                                       IContextMenu3* contextMenu3,
                                       IContextMenu2* contextMenu2) {
    bool added = false;
    int count = GetMenuItemCount(source);
    for (int index = 0; index < count; index++) {
        WCHAR label[512]{};
        MENUITEMINFOW item{sizeof(item)};
        item.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU |
                     MIIM_STRING;
        item.dwTypeData = label;
        item.cch = ARRAYSIZE(label) - 1;
        if (!GetMenuItemInfoW(source, index, TRUE, &item) ||
            (item.fType & MFT_SEPARATOR)) {
            continue;
        }

        if (item.hSubMenu) {
            if (contextMenu3) {
                LRESULT menuResult = 0;
                contextMenu3->HandleMenuMsg2(
                    WM_INITMENUPOPUP,
                    reinterpret_cast<WPARAM>(item.hSubMenu),
                    MAKELPARAM(index, FALSE), &menuResult);
            } else if (contextMenu2) {
                contextMenu2->HandleMenuMsg(
                    WM_INITMENUPOPUP,
                    reinterpret_cast<WPARAM>(item.hSubMenu),
                    MAKELPARAM(index, FALSE));
            }
            HMENU filteredSubmenu = CreatePopupMenu();
            if (!filteredSubmenu) {
                continue;
            }
            if (BetterPanelAppendExtractMenuItems(
                    item.hSubMenu, filteredSubmenu, contextMenu3,
                    contextMenu2)) {
                MENUITEMINFOW output{sizeof(output)};
                output.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_STATE;
                output.dwTypeData = label;
                output.hSubMenu = filteredSubmenu;
                output.fState = item.fState;
                InsertMenuItemW(destination, GetMenuItemCount(destination),
                                TRUE, &output);
                added = true;
            } else {
                DestroyMenu(filteredSubmenu);
            }
            continue;
        }

        std::wstring lowered(label);
        std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                       towlower);
        if (lowered.find(L"extract") == std::wstring::npos ||
            item.wID == static_cast<UINT>(-1)) {
            continue;
        }

        MENUITEMINFOW output{sizeof(output)};
        output.fMask = MIIM_STRING | MIIM_ID | MIIM_STATE;
        output.dwTypeData = label;
        output.wID = item.wID;
        output.fState = item.fState;
        InsertMenuItemW(destination, GetMenuItemCount(destination), TRUE,
                        &output);
        added = true;
    }
    return added;
}

HRESULT BetterPanelShowExtractMenu(std::wstring const& path) {
    PIDLIST_ABSOLUTE absolutePidl = nullptr;
    HRESULT result = SHParseDisplayName(path.c_str(), nullptr, &absolutePidl,
                                        0, nullptr);
    if (FAILED(result)) {
        return result;
    }

    HWND owner = GetForegroundWindow();
    winrt::com_ptr<IShellFolder> parentFolder;
    PCUITEMID_CHILD childPidl = nullptr;
    result = SHBindToParent(absolutePidl, IID_PPV_ARGS(parentFolder.put()),
                            &childPidl);
    if (FAILED(result)) {
        CoTaskMemFree(absolutePidl);
        return result;
    }

    winrt::com_ptr<IContextMenu> contextMenu;
    PCUITEMID_CHILD children[] = {childPidl};
    result = parentFolder->GetUIObjectOf(
        owner, 1, children, IID_IContextMenu, nullptr,
        reinterpret_cast<void**>(contextMenu.put()));
    if (FAILED(result)) {
        CoTaskMemFree(absolutePidl);
        return result;
    }

    winrt::com_ptr<IContextMenu3> contextMenu3;
    contextMenu->QueryInterface(IID_PPV_ARGS(contextMenu3.put()));
    winrt::com_ptr<IContextMenu2> contextMenu2;
    if (!contextMenu3) {
        contextMenu->QueryInterface(IID_PPV_ARGS(contextMenu2.put()));
    }

    HMENU completeMenu = CreatePopupMenu();
    HMENU extractMenu = CreatePopupMenu();
    if (!completeMenu || !extractMenu) {
        if (completeMenu) DestroyMenu(completeMenu);
        if (extractMenu) DestroyMenu(extractMenu);
        CoTaskMemFree(absolutePidl);
        return E_OUTOFMEMORY;
    }

    result = contextMenu->QueryContextMenu(
        completeMenu, 0, 1, 0x7FFF,
        CMF_NORMAL | CMF_EXPLORE | CMF_EXTENDEDVERBS |
            CMF_SYNCCASCADEMENU);
    if (FAILED(result) ||
        !BetterPanelAppendExtractMenuItems(
            completeMenu, extractMenu, contextMenu3.get(),
            contextMenu2.get())) {
        DestroyMenu(extractMenu);
        DestroyMenu(completeMenu);
        CoTaskMemFree(absolutePidl);
        return FAILED(result) ? result : HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    POINT point{};
    GetCursorPos(&point);
    SetForegroundWindow(owner);
    UINT command = TrackPopupMenuEx(
        extractMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
        point.x, point.y, owner, nullptr);
    if (command) {
        CMINVOKECOMMANDINFOEX invoke{sizeof(invoke)};
        invoke.fMask = CMIC_MASK_UNICODE | CMIC_MASK_ASYNCOK;
        invoke.hwnd = owner;
        invoke.lpVerb = MAKEINTRESOURCEA(command - 1);
        invoke.lpVerbW = MAKEINTRESOURCEW(command - 1);
        invoke.nShow = SW_SHOWNORMAL;
        result = contextMenu->InvokeCommand(
            reinterpret_cast<LPCMINVOKECOMMANDINFO>(&invoke));
    } else {
        result = S_FALSE;
    }
    PostMessageW(owner, WM_NULL, 0, 0);
    DestroyMenu(extractMenu);
    DestroyMenu(completeMenu);
    CoTaskMemFree(absolutePidl);
    return result;
}

bool BetterPanelFindFavoriteMenuCommand(HMENU menu,
                                        IContextMenu* contextMenu,
                                        IContextMenu3* contextMenu3,
                                        IContextMenu2* contextMenu2,
                                        UINT& command,
                                        bool& removing) {
    int count = GetMenuItemCount(menu);
    for (int index = 0; index < count; index++) {
        WCHAR label[256]{};
        MENUITEMINFOW item{sizeof(item)};
        item.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU |
                     MIIM_STRING;
        item.dwTypeData = label;
        item.cch = ARRAYSIZE(label) - 1;
        if (!GetMenuItemInfoW(menu, index, TRUE, &item) ||
            (item.fType & MFT_SEPARATOR)) {
            continue;
        }
        if (item.hSubMenu) {
            if (contextMenu3) {
                LRESULT menuResult = 0;
                contextMenu3->HandleMenuMsg2(
                    WM_INITMENUPOPUP,
                    reinterpret_cast<WPARAM>(item.hSubMenu),
                    MAKELPARAM(index, FALSE), &menuResult);
            } else if (contextMenu2) {
                contextMenu2->HandleMenuMsg(
                    WM_INITMENUPOPUP,
                    reinterpret_cast<WPARAM>(item.hSubMenu),
                    MAKELPARAM(index, FALSE));
            }
            if (BetterPanelFindFavoriteMenuCommand(
                    item.hSubMenu, contextMenu, contextMenu3, contextMenu2,
                    command, removing)) {
                return true;
            }
        }
        if (item.wID == static_cast<UINT>(-1) ||
            (item.fState & (MFS_DISABLED | MFS_GRAYED))) {
            continue;
        }

        std::wstring loweredLabel(label);
        loweredLabel.erase(
            std::remove(loweredLabel.begin(), loweredLabel.end(), L'&'),
            loweredLabel.end());
        std::transform(loweredLabel.begin(), loweredLabel.end(),
                       loweredLabel.begin(), towlower);

        WCHAR verb[128]{};
        std::wstring loweredVerb;
        if (SUCCEEDED(contextMenu->GetCommandString(
                item.wID - 1, GCS_VERBW, nullptr,
                reinterpret_cast<LPSTR>(verb), ARRAYSIZE(verb)))) {
            loweredVerb.assign(verb);
            std::transform(loweredVerb.begin(), loweredVerb.end(),
                           loweredVerb.begin(), towlower);
        }

        bool add = loweredLabel.find(L"add to favorites") !=
                       std::wstring::npos ||
                   loweredVerb.find(L"pintohome") != std::wstring::npos;
        bool remove = loweredLabel.find(L"remove from favorites") !=
                          std::wstring::npos ||
                      loweredVerb.find(L"unpinfromhome") !=
                          std::wstring::npos;
        if (add || remove) {
            command = item.wID;
            removing = remove;
            return true;
        }
    }
    return false;
}

std::wstring BetterPanelFavoritesListPath() {
    WCHAR roamingAppData[MAX_PATH]{};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr,
                                SHGFP_TYPE_CURRENT, roamingAppData))) {
        return {};
    }
    return std::wstring(roamingAppData) +
           L"\\Microsoft\\Windows\\Recent\\AutomaticDestinations\\"
           L"5f7b5f1e01b83767.automaticDestinations-ms";
}

uint16_t BetterPanelReadUInt16(std::vector<uint8_t> const& bytes,
                               size_t offset) {
    if (offset + 2 > bytes.size()) return 0;
    return static_cast<uint16_t>(bytes[offset]) |
           static_cast<uint16_t>(bytes[offset + 1] << 8);
}

uint32_t BetterPanelReadUInt32(std::vector<uint8_t> const& bytes,
                               size_t offset) {
    if (offset + 4 > bytes.size()) return 0;
    return static_cast<uint32_t>(bytes[offset]) |
           (static_cast<uint32_t>(bytes[offset + 1]) << 8) |
           (static_cast<uint32_t>(bytes[offset + 2]) << 16) |
           (static_cast<uint32_t>(bytes[offset + 3]) << 24);
}

std::optional<bool> BetterPanelReadFavoriteState(std::wstring const& path) {
    std::wstring listPath = BetterPanelFavoritesListPath();
    if (listPath.empty()) return std::nullopt;

    winrt::com_ptr<IStorage> storage;
    HRESULT result = StgOpenStorage(
        listPath.c_str(), nullptr, STGM_READ | STGM_SHARE_DENY_WRITE,
        nullptr, 0, storage.put());
    if (FAILED(result) || !storage) return std::nullopt;

    winrt::com_ptr<IStream> stream;
    result = storage->OpenStream(L"DestList", nullptr,
                                 STGM_READ | STGM_SHARE_EXCLUSIVE, 0,
                                 stream.put());
    if (FAILED(result) || !stream) return std::nullopt;

    STATSTG stat{};
    result = stream->Stat(&stat, STATFLAG_NONAME);
    if (FAILED(result) || stat.cbSize.QuadPart < 32 ||
        stat.cbSize.QuadPart > 16 * 1024 * 1024) {
        return std::nullopt;
    }
    std::vector<uint8_t> bytes(static_cast<size_t>(stat.cbSize.QuadPart));
    ULONG bytesRead = 0;
    result = stream->Read(bytes.data(), static_cast<ULONG>(bytes.size()),
                          &bytesRead);
    if (FAILED(result) || bytesRead < 32) return std::nullopt;
    bytes.resize(bytesRead);

    uint32_t version = BetterPanelReadUInt32(bytes, 0);
    uint32_t entryCount = BetterPanelReadUInt32(bytes, 4);
    if (version < 3 || entryCount > 10000) return std::nullopt;

    size_t offset = 32;
    for (uint32_t index = 0;
         index < entryCount && offset + 130 <= bytes.size(); index++) {
        uint16_t pathLength = BetterPanelReadUInt16(bytes, offset + 128);
        size_t pathBytes = static_cast<size_t>(pathLength) * 2;
        if (offset + 130 + pathBytes > bytes.size()) return std::nullopt;

        std::wstring entryPath;
        entryPath.reserve(pathLength);
        for (uint16_t character = 0; character < pathLength; character++) {
            entryPath.push_back(static_cast<wchar_t>(BetterPanelReadUInt16(
                bytes, offset + 130 + static_cast<size_t>(character) * 2)));
        }
        if (_wcsicmp(entryPath.c_str(), path.c_str()) == 0) {
            return BetterPanelReadUInt32(bytes, offset + 108) != UINT32_MAX;
        }
        offset += 130 + pathBytes + 4;
    }
    return false;
}

HRESULT BetterPanelInvokeFavoriteCommand(std::wstring const& path,
                                         IShellBrowser* shellBrowser) {
    CLSID commandClass{};
    HRESULT result = CLSIDFromString(
        L"{b455f46e-e4af-4035-b0a4-cf18d2f6f28e}", &commandClass);
    if (FAILED(result)) return result;

    winrt::com_ptr<IExecuteCommand> executeCommand;
    result = CoCreateInstance(commandClass, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(executeCommand.put()));
    if (FAILED(result) || !executeCommand) return result;

    winrt::com_ptr<IShellItem> item;
    result = SHCreateItemFromParsingName(path.c_str(), nullptr,
                                         IID_PPV_ARGS(item.put()));
    if (FAILED(result) || !item) return result;

    winrt::com_ptr<IShellItemArray> selection;
    result = SHCreateShellItemArrayFromShellItem(
        item.get(), IID_PPV_ARGS(selection.put()));
    if (FAILED(result) || !selection) return result;

    winrt::com_ptr<IObjectWithSelection> objectWithSelection;
    result = executeCommand->QueryInterface(
        IID_PPV_ARGS(objectWithSelection.put()));
    if (FAILED(result) || !objectWithSelection) return result;
    result = objectWithSelection->SetSelection(selection.get());
    if (FAILED(result)) return result;

    winrt::com_ptr<IObjectWithSite> objectWithSite;
    if (shellBrowser &&
        SUCCEEDED(executeCommand->QueryInterface(
            IID_PPV_ARGS(objectWithSite.put()))) && objectWithSite) {
        objectWithSite->SetSite(shellBrowser);
    }

    executeCommand->SetKeyState(0);
    executeCommand->SetNoShowUI(FALSE);
    std::wstring directory = path;
    size_t separator = directory.find_last_of(L"\\/");
    if (separator != std::wstring::npos) {
        directory.resize(separator);
        executeCommand->SetDirectory(directory.c_str());
    }
    POINT point{};
    GetCursorPos(&point);
    executeCommand->SetParameters(L"");
    executeCommand->SetPosition(point);
    executeCommand->SetShowWindow(SW_SHOWNORMAL);
    return executeCommand->Execute();
}

HRESULT BetterPanelRemoveFavorite(std::wstring const& path) {
    winrt::com_ptr<IShellItem> item;
    HRESULT result = SHCreateItemFromParsingName(
        path.c_str(), nullptr, IID_PPV_ARGS(item.put()));
    if (FAILED(result) || !item) return result;

    winrt::com_ptr<IApplicationDestinations> destinations;
    result = CoCreateInstance(CLSID_ApplicationDestinations, nullptr,
                              CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(destinations.put()));
    if (FAILED(result) || !destinations) return result;
    result = destinations->SetAppID(
        L"Microsoft.Windows.Explorer_RecentFiles");
    if (FAILED(result)) return result;
    result = destinations->RemoveDestination(item.get());
    if (SUCCEEDED(result)) {
        // Removing a destination also removes it from Recent. Put it back as
        // an ordinary unpinned recent item, matching Explorer's separation of
        // Recent and Favorites.
        SHAddToRecentDocs(SHARD_SHELLITEM, item.get());
    }
    return result;
}

void BetterPanelRunFavoriteAction(
    std::shared_ptr<BetterPanelState> const& state,
    std::wstring const& path, bool removing) {
    if (!state || path.empty()) return;
    auto dispatcher = state->dispatcher;
    std::weak_ptr<BetterPanelState> weakState = state;
    std::thread([weakState, dispatcher, path, removing]() {
        HRESULT initialized = CoInitializeEx(nullptr,
                                             COINIT_APARTMENTTHREADED);
        HRESULT result = E_FAIL;
        if (removing) {
            result = BetterPanelRemoveFavorite(path);
        } else {
            // Explorer's native command first creates a current Recent Files
            // destination, then pins that fresh entry.
            SHAddToRecentDocs(SHARD_PATHW, path.c_str());
            Sleep(150);
            result = BetterPanelInvokeFavoriteCommand(path, nullptr);
        }
        if (SUCCEEDED(initialized)) CoUninitialize();
        dispatcher.TryEnqueue([weakState, path, removing, result]() {
            auto state = weakState.lock();
            if (!state || state->unloaded || state->selectedPath != path) {
                return;
            }
            if (SUCCEEDED(result)) {
                BetterPanelQueueFavoriteCheck(state, path, true, !removing);
            } else {
                BetterPanelShowTemporaryButtonMessage(
                    state->favoriteButton, L"Favorites action failed");
            }
        });
    }).detach();
}

HRESULT BetterPanelInvokeLegacyFavoriteCommand(std::wstring const& path,
                                               bool& removing,
                                               HWND owner = nullptr) {
    removing = false;
    PIDLIST_ABSOLUTE absolutePidl = nullptr;
    HRESULT result = SHParseDisplayName(path.c_str(), nullptr, &absolutePidl,
                                        0, nullptr);
    if (FAILED(result)) return result;

    if (!owner) owner = GetForegroundWindow();
    winrt::com_ptr<IShellFolder> parentFolder;
    PCUITEMID_CHILD childPidl = nullptr;
    result = SHBindToParent(absolutePidl, IID_PPV_ARGS(parentFolder.put()),
                            &childPidl);
    if (FAILED(result)) {
        CoTaskMemFree(absolutePidl);
        return result;
    }

    winrt::com_ptr<IContextMenu> contextMenu;
    PCUITEMID_CHILD children[] = {childPidl};
    result = parentFolder->GetUIObjectOf(
        owner, 1, children, IID_IContextMenu, nullptr,
        reinterpret_cast<void**>(contextMenu.put()));
    if (FAILED(result)) {
        CoTaskMemFree(absolutePidl);
        return result;
    }
    winrt::com_ptr<IContextMenu3> contextMenu3;
    contextMenu->QueryInterface(IID_PPV_ARGS(contextMenu3.put()));
    winrt::com_ptr<IContextMenu2> contextMenu2;
    if (!contextMenu3) {
        contextMenu->QueryInterface(IID_PPV_ARGS(contextMenu2.put()));
    }

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        CoTaskMemFree(absolutePidl);
        return E_OUTOFMEMORY;
    }
    result = contextMenu->QueryContextMenu(
        menu, 0, 1, 0x7FFF,
        CMF_NORMAL | CMF_EXPLORE | CMF_EXTENDEDVERBS |
            CMF_SYNCCASCADEMENU);
    UINT command = 0;
    if (SUCCEEDED(result) && BetterPanelFindFavoriteMenuCommand(
                                 menu, contextMenu.get(), contextMenu3.get(),
                                 contextMenu2.get(), command, removing)) {
        CMINVOKECOMMANDINFOEX invoke{sizeof(invoke)};
        // Keep the context-menu object, PIDL, and menu alive until Explorer's
        // native Favorites command has completed. ASYNCOK allowed those
        // objects to be released while the operation was still pending.
        invoke.fMask = CMIC_MASK_UNICODE;
        invoke.hwnd = owner;
        invoke.lpVerb = MAKEINTRESOURCEA(command - 1);
        invoke.lpVerbW = MAKEINTRESOURCEW(command - 1);
        invoke.nShow = SW_SHOWNORMAL;
        result = contextMenu->InvokeCommand(
            reinterpret_cast<LPCMINVOKECOMMANDINFO>(&invoke));
    } else if (SUCCEEDED(result)) {
        result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }
    DestroyMenu(menu);
    CoTaskMemFree(absolutePidl);
    return result;
}

HRESULT BetterPanelInvokeSelectedFavoriteCommand(
    IShellBrowser* shellBrowser, bool& removing) {
    removing = false;
    if (!shellBrowser) return E_INVALIDARG;

    winrt::com_ptr<IShellView> shellView;
    HRESULT result = shellBrowser->QueryActiveShellView(shellView.put());
    if (FAILED(result) || !shellView) return result;

    HWND owner = nullptr;
    shellView->GetWindow(&owner);

    winrt::com_ptr<IContextMenu> contextMenu;
    result = shellView->GetItemObject(
        SVGIO_SELECTION, IID_IContextMenu,
        reinterpret_cast<void**>(contextMenu.put()));
    if (FAILED(result) || !contextMenu) return result;

    winrt::com_ptr<IContextMenu3> contextMenu3;
    contextMenu->QueryInterface(IID_PPV_ARGS(contextMenu3.put()));
    winrt::com_ptr<IContextMenu2> contextMenu2;
    if (!contextMenu3) {
        contextMenu->QueryInterface(IID_PPV_ARGS(contextMenu2.put()));
    }

    HMENU menu = CreatePopupMenu();
    if (!menu) return E_OUTOFMEMORY;
    result = contextMenu->QueryContextMenu(
        menu, 0, 1, 0x7FFF,
        CMF_NORMAL | CMF_EXPLORE | CMF_EXTENDEDVERBS |
            CMF_SYNCCASCADEMENU);
    UINT command = 0;
    if (SUCCEEDED(result) && BetterPanelFindFavoriteMenuCommand(
                                 menu, contextMenu.get(), contextMenu3.get(),
                                 contextMenu2.get(), command, removing)) {
        CMINVOKECOMMANDINFOEX invoke{sizeof(invoke)};
        invoke.fMask = CMIC_MASK_UNICODE;
        invoke.hwnd = owner;
        invoke.lpVerb = MAKEINTRESOURCEA(command - 1);
        invoke.lpVerbW = MAKEINTRESOURCEW(command - 1);
        invoke.nShow = SW_SHOWNORMAL;
        result = contextMenu->InvokeCommand(
            reinterpret_cast<LPCMINVOKECOMMANDINFO>(&invoke));
    } else if (SUCCEEDED(result)) {
        result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }
    DestroyMenu(menu);
    return result;
}

std::wstring BetterPanelQuoteCommandArgument(std::wstring_view value) {
    std::wstring quoted = L"\"";
    for (wchar_t character : value) {
        if (character == L'\"') {
            quoted += L'\\';
        }
        quoted += character;
    }
    quoted += L'\"';
    return quoted;
}

bool BetterPanelFileExists(std::wstring const& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring BetterPanelFindExtractor() {
    WCHAR programFiles[MAX_PATH]{};
    WCHAR programFilesX86[MAX_PATH]{};
    ExpandEnvironmentStringsW(L"%ProgramFiles%", programFiles,
                              ARRAYSIZE(programFiles));
    ExpandEnvironmentStringsW(L"%ProgramFiles(x86)%", programFilesX86,
                              ARRAYSIZE(programFilesX86));
    std::wstring candidates[] = {
        std::wstring(programFiles) + L"\\WinRAR\\WinRAR.exe",
        std::wstring(programFilesX86) + L"\\WinRAR\\WinRAR.exe",
        std::wstring(programFiles) + L"\\7-Zip\\7z.exe",
        std::wstring(programFilesX86) + L"\\7-Zip\\7z.exe",
        std::wstring(programFiles) + L"\\WinZip\\wzunzip.exe",
        std::wstring(programFilesX86) + L"\\WinZip\\wzunzip.exe",
        L"C:\\Windows\\System32\\tar.exe"};
    for (auto const& candidate : candidates) {
        if (BetterPanelFileExists(candidate)) {
            return candidate;
        }
    }
    return {};
}

std::wstring BetterPanelReadArchiveListing(std::wstring const& archive) {
    std::wstring tool = BetterPanelFindExtractor();
    std::wstring lowered = tool;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), towlower);
    size_t dot = archive.find_last_of(L'.');
    std::wstring extension = dot == std::wstring::npos
                                 ? L""
                                 : archive.substr(dot);
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   towlower);
    std::wstring command;
    if (lowered.ends_with(L"7z.exe")) {
        command = BetterPanelQuoteCommandArgument(tool) + L" l -ba " +
                  BetterPanelQuoteCommandArgument(archive);
    } else if (lowered.ends_with(L"winrar.exe") && extension == L".rar") {
        // WinRAR.exe is the graphical program and shows an error dialog for
        // console-only listing commands. Use its companion console executable.
        size_t slash = tool.find_last_of(L"\\/");
        std::wstring folder = slash == std::wstring::npos
                                  ? L""
                                  : tool.substr(0, slash);
        std::wstring console = folder + L"\\UnRAR.exe";
        if (!BetterPanelFileExists(console)) console = folder + L"\\Rar.exe";
        if (!BetterPanelFileExists(console)) {
            return L"RAR preview requires UnRAR.exe or Rar.exe.";
        }
        command = BetterPanelQuoteCommandArgument(console) + L" lb -c- " +
                  BetterPanelQuoteCommandArgument(archive);
    } else {
        WCHAR systemDirectory[MAX_PATH]{};
        GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
        std::wstring tar = std::wstring(systemDirectory) + L"\\tar.exe";
        if (!BetterPanelFileExists(tar)) {
            return L"Archive preview requires Windows tar or 7-Zip for this format.";
        }
        command = BetterPanelQuoteCommandArgument(tar) + L" -tf " +
                  BetterPanelQuoteCommandArgument(archive);
    }

    WCHAR temporaryFolder[MAX_PATH]{};
    WCHAR temporaryFile[MAX_PATH]{};
    if (!GetTempPathW(ARRAYSIZE(temporaryFolder), temporaryFolder) ||
        !GetTempFileNameW(temporaryFolder, L"BPA", 0, temporaryFile)) {
        return L"Archive contents could not be read.";
    }
    SECURITY_ATTRIBUTES security{sizeof(security), nullptr, TRUE};
    HANDLE outputFile = CreateFileW(
        temporaryFile, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, &security, CREATE_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY, nullptr);
    HANDLE nullInput = CreateFileW(L"NUL", GENERIC_READ,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   &security, OPEN_EXISTING, 0, nullptr);
    if (outputFile == INVALID_HANDLE_VALUE || nullInput == INVALID_HANDLE_VALUE) {
        if (outputFile != INVALID_HANDLE_VALUE) CloseHandle(outputFile);
        if (nullInput != INVALID_HANDLE_VALUE) CloseHandle(nullInput);
        DeleteFileW(temporaryFile);
        return L"Archive contents could not be read.";
    }
    STARTUPINFOW startup{sizeof(startup)};
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdOutput = outputFile;
    startup.hStdError = outputFile;
    startup.hStdInput = nullInput;
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, TRUE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                        &process)) {
        CloseHandle(outputFile);
        CloseHandle(nullInput);
        DeleteFileW(temporaryFile);
        return L"Archive preview could not be started.";
    }
    CloseHandle(nullInput);
    CloseHandle(process.hThread);
    DWORD waitResult = WaitForSingleObject(process.hProcess, 10000);
    if (waitResult != WAIT_OBJECT_0) {
        TerminateProcess(process.hProcess, 1);
        WaitForSingleObject(process.hProcess, 1000);
    }
    DWORD exitCode = 1;
    GetExitCodeProcess(process.hProcess, &exitCode);
    CloseHandle(process.hProcess);

    LARGE_INTEGER start{};
    SetFilePointerEx(outputFile, start, nullptr, FILE_BEGIN);
    std::vector<char> bytes(512 * 1024);
    DWORD bytesRead = 0;
    if (!ReadFile(outputFile, bytes.data(), static_cast<DWORD>(bytes.size()),
                  &bytesRead, nullptr)) {
        bytesRead = 0;
    }
    bytes.resize(bytesRead);
    CloseHandle(outputFile);
    DeleteFileW(temporaryFile);

    std::wstring output;
    if (!bytes.empty() &&
        !BetterPanelDecodeMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS,
                                    bytes.data(), static_cast<int>(bytes.size()),
                                    output)) {
        BetterPanelDecodeMultiByte(CP_ACP, 0, bytes.data(),
                                   static_cast<int>(bytes.size()), output);
    }
    std::wistringstream lines(output);
    std::wstring line;
    std::wstring preview;
    size_t shown = 0;
    while (std::getline(lines, line) && shown < 200) {
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (line.empty()) continue;
        preview += line + L"\n";
        ++shown;
    }
    if (preview.empty()) {
        return exitCode == 0 ? L"No files were found in this archive."
                             : L"The archive contents could not be listed.";
    }
    if (!lines.eof()) preview += L"\u2026more entries are not shown";
    return preview;
}

void BetterPanelLoadArchivePreview(
    std::shared_ptr<BetterPanelState> const& state,
    std::wstring const& path) {
    if (!state || path.empty() || state->archivePreviewLoading ||
        state->archivePreviewPath == path) return;
    state->archivePreviewLoading = true;
    state->archivePreviewPath = path;
    if (auto text = state->archivePreviewText.get())
        text.Text(L"Reading archive contents…");
    auto weakState = std::weak_ptr<BetterPanelState>(state);
    auto dispatcher = state->dispatcher;
    std::thread([weakState, dispatcher, path]() {
        auto listing = BetterPanelReadArchiveListing(path);
        dispatcher.TryEnqueue([weakState, path,
                               listing = std::move(listing)]() {
            auto state = weakState.lock();
            if (!state || state->unloaded ||
                state->archivePreviewPath != path) return;
            state->archivePreviewLoading = false;
            if (auto text = state->archivePreviewText.get())
                text.Text(listing);
        });
    }).detach();
}

std::wstring BetterPanelArchiveParent(std::wstring_view path) {
    size_t slash = path.find_last_of(L"\\/");
    return slash == std::wstring_view::npos
               ? std::wstring()
               : std::wstring(path.substr(0, slash));
}

std::wstring BetterPanelArchiveBaseName(std::wstring_view path) {
    std::wstring name = BetterPanelFileName(path);
    std::wstring lowered = name;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), towlower);
    static constexpr std::wstring_view compoundExtensions[] = {
        L".tar.gz", L".tar.bz2", L".tar.xz", L".tar.zst"};
    for (auto extension : compoundExtensions) {
        if (lowered.ends_with(extension)) {
            name.resize(name.size() - extension.size());
            return name;
        }
    }
    size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        name.resize(dot);
    }
    return name;
}

HRESULT BetterPanelChooseExtractionFolder(HWND owner,
                                          std::wstring& destination) {
    winrt::com_ptr<IFileOpenDialog> dialog;
    HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                      CLSCTX_INPROC_SERVER,
                                      IID_PPV_ARGS(dialog.put()));
    if (FAILED(result)) {
        return result;
    }
    FILEOPENDIALOGOPTIONS options{};
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM |
                       FOS_PATHMUSTEXIST);
    dialog->SetTitle(L"Choose destination folder");
    result = dialog->Show(owner);
    if (FAILED(result)) {
        return result;
    }
    winrt::com_ptr<IShellItem> item;
    result = dialog->GetResult(item.put());
    if (FAILED(result)) {
        return result;
    }
    PWSTR rawPath = nullptr;
    result = item->GetDisplayName(SIGDN_FILESYSPATH, &rawPath);
    if (SUCCEEDED(result) && rawPath) {
        destination = rawPath;
    }
    CoTaskMemFree(rawPath);
    return result;
}

HRESULT BetterPanelStartExtraction(std::wstring const& archive,
                                   std::wstring destination) {
    std::wstring extractor = BetterPanelFindExtractor();
    if (extractor.empty()) {
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }
    int directoryResult =
        SHCreateDirectoryExW(nullptr, destination.c_str(), nullptr);
    if (directoryResult != ERROR_SUCCESS &&
        directoryResult != ERROR_ALREADY_EXISTS) {
        return HRESULT_FROM_WIN32(directoryResult);
    }

    std::wstring lowered = extractor;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), towlower);
    std::wstring command = BetterPanelQuoteCommandArgument(extractor);
    if (lowered.ends_with(L"winrar.exe")) {
        if (!destination.ends_with(L'\\')) destination += L'\\';
        command += L" x -o+ -ibck " + BetterPanelQuoteCommandArgument(archive) +
                   L" " + BetterPanelQuoteCommandArgument(destination);
    } else if (lowered.ends_with(L"7z.exe")) {
        command += L" x -y -o" + BetterPanelQuoteCommandArgument(destination) +
                   L" " + BetterPanelQuoteCommandArgument(archive);
    } else if (lowered.ends_with(L"wzunzip.exe")) {
        command += L" -d " + BetterPanelQuoteCommandArgument(archive) +
                   L" " + BetterPanelQuoteCommandArgument(destination);
    } else {
        command += L" -xf " + BetterPanelQuoteCommandArgument(archive) +
                   L" -C " + BetterPanelQuoteCommandArgument(destination);
    }

    STARTUPINFOW startup{sizeof(startup)};
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                        &process)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return S_OK;
}

HRESULT BetterPanelShowSimpleExtractMenu(std::wstring const& archive) {
    std::wstring parent = BetterPanelArchiveParent(archive);
    std::wstring baseName = BetterPanelArchiveBaseName(archive);
    if (parent.empty() || baseName.empty()) {
        return E_INVALIDARG;
    }

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return E_OUTOFMEMORY;
    }
    AppendMenuW(menu, MF_STRING, 1, L"Extract here");
    AppendMenuW(menu, MF_STRING, 2, L"Extract to...");
    std::wstring folderLabel = L"Extract to \"" + baseName + L"\\\"";
    AppendMenuW(menu, MF_STRING, 3, folderLabel.c_str());

    POINT point{};
    GetCursorPos(&point);
    HWND owner = GetForegroundWindow();
    UINT command = TrackPopupMenuEx(
        menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
        point.x, point.y, owner, nullptr);
    DestroyMenu(menu);
    if (!command) {
        return S_FALSE;
    }

    std::wstring destination;
    if (command == 1) {
        destination = parent;
    } else if (command == 2) {
        HRESULT result = BetterPanelChooseExtractionFolder(owner, destination);
        if (FAILED(result)) {
            return result;
        }
    } else {
        destination = parent + L"\\" + baseName;
    }
    return BetterPanelStartExtraction(archive, destination);
}

HRESULT BetterPanelMoveItemsToFolder(
    std::vector<std::wstring> const& sources,
    std::wstring const& destination) {
    if (sources.empty() || destination.empty()) {
        return E_INVALIDARG;
    }
    winrt::com_ptr<IFileOperation> operation;
    HRESULT result = CoCreateInstance(CLSID_FileOperation, nullptr,
                                      CLSCTX_INPROC_SERVER,
                                      IID_PPV_ARGS(operation.put()));
    if (FAILED(result)) {
        return result;
    }
    operation->SetOwnerWindow(GetForegroundWindow());
    operation->SetOperationFlags(FOF_ALLOWUNDO | FOF_NOCONFIRMMKDIR |
                                 FOFX_SHOWELEVATIONPROMPT |
                                 FOFX_ADDUNDORECORD);

    winrt::com_ptr<IShellItem> destinationItem;
    result = SHCreateItemFromParsingName(destination.c_str(), nullptr,
                                         IID_PPV_ARGS(destinationItem.put()));
    if (FAILED(result)) {
        return result;
    }
    for (auto const& source : sources) {
        winrt::com_ptr<IShellItem> sourceItem;
        result = SHCreateItemFromParsingName(source.c_str(), nullptr,
                                             IID_PPV_ARGS(sourceItem.put()));
        if (FAILED(result)) {
            return result;
        }
        result = operation->MoveItem(sourceItem.get(), destinationItem.get(),
                                     nullptr, nullptr);
        if (FAILED(result)) {
            return result;
        }
    }
    result = operation->PerformOperations();
    if (FAILED(result)) {
        return result;
    }
    BOOL aborted = FALSE;
    operation->GetAnyOperationsAborted(&aborted);
    return aborted ? HRESULT_FROM_WIN32(ERROR_CANCELLED) : S_OK;
}

HRESULT BetterPanelDeleteItems(
    std::vector<std::wstring> const& sources) {
    if (sources.empty()) {
        return E_INVALIDARG;
    }
    winrt::com_ptr<IFileOperation> operation;
    HRESULT result = CoCreateInstance(CLSID_FileOperation, nullptr,
                                      CLSCTX_INPROC_SERVER,
                                      IID_PPV_ARGS(operation.put()));
    if (FAILED(result)) return result;
    operation->SetOwnerWindow(GetForegroundWindow());
    operation->SetOperationFlags(FOF_ALLOWUNDO | FOF_WANTNUKEWARNING |
                                 FOFX_RECYCLEONDELETE |
                                 FOFX_SHOWELEVATIONPROMPT |
                                 FOFX_ADDUNDORECORD);
    for (auto const& source : sources) {
        winrt::com_ptr<IShellItem> sourceItem;
        result = SHCreateItemFromParsingName(source.c_str(), nullptr,
                                             IID_PPV_ARGS(sourceItem.put()));
        if (FAILED(result)) return result;
        result = operation->DeleteItem(sourceItem.get(), nullptr);
        if (FAILED(result)) return result;
    }
    result = operation->PerformOperations();
    if (FAILED(result)) return result;
    BOOL aborted = FALSE;
    operation->GetAnyOperationsAborted(&aborted);
    return aborted ? HRESULT_FROM_WIN32(ERROR_CANCELLED) : S_OK;
}

std::wstring BetterPanelFindArchiveCreator() {
    WCHAR programFiles[MAX_PATH]{};
    WCHAR programFilesX86[MAX_PATH]{};
    ExpandEnvironmentStringsW(L"%ProgramFiles%", programFiles,
                              ARRAYSIZE(programFiles));
    ExpandEnvironmentStringsW(L"%ProgramFiles(x86)%", programFilesX86,
                              ARRAYSIZE(programFilesX86));
    std::wstring candidates[] = {
        std::wstring(programFiles) + L"\\WinRAR\\WinRAR.exe",
        std::wstring(programFilesX86) + L"\\WinRAR\\WinRAR.exe",
        std::wstring(programFiles) + L"\\7-Zip\\7z.exe",
        std::wstring(programFilesX86) + L"\\7-Zip\\7z.exe",
        std::wstring(programFiles) + L"\\WinZip\\wzzip.exe",
        std::wstring(programFilesX86) + L"\\WinZip\\wzzip.exe",
        L"C:\\Windows\\System32\\tar.exe"};
    for (auto const& candidate : candidates) {
        if (BetterPanelFileExists(candidate)) return candidate;
    }
    return {};
}

HRESULT BetterPanelChooseArchivePath(HWND owner,
                                     std::wstring const& creator,
                                     std::wstring& archivePath) {
    winrt::com_ptr<IFileSaveDialog> dialog;
    HRESULT result = CoCreateInstance(CLSID_FileSaveDialog, nullptr,
                                      CLSCTX_INPROC_SERVER,
                                      IID_PPV_ARGS(dialog.put()));
    if (FAILED(result)) return result;
    std::wstring lowered = creator;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), towlower);
    COMDLG_FILTERSPEC filters[2]{};
    UINT filterCount = 0;
    if (lowered.ends_with(L"winrar.exe")) {
        filters[0] = {L"RAR archive (*.rar)", L"*.rar"};
        filters[1] = {L"ZIP archive (*.zip)", L"*.zip"};
        filterCount = 2;
        dialog->SetDefaultExtension(L"rar");
        dialog->SetFileName(L"Archive.rar");
    } else if (lowered.ends_with(L"7z.exe")) {
        filters[0] = {L"7-Zip archive (*.7z)", L"*.7z"};
        filters[1] = {L"ZIP archive (*.zip)", L"*.zip"};
        filterCount = 2;
        dialog->SetDefaultExtension(L"7z");
        dialog->SetFileName(L"Archive.7z");
    } else {
        filters[0] = {L"ZIP archive (*.zip)", L"*.zip"};
        filters[1] = {L"TAR archive (*.tar)", L"*.tar"};
        filterCount = 2;
        dialog->SetDefaultExtension(L"zip");
        dialog->SetFileName(L"Archive.zip");
    }
    dialog->SetFileTypes(filterCount, filters);
    dialog->SetTitle(L"Create archive from selected items");
    result = dialog->Show(owner);
    if (FAILED(result)) return result;
    winrt::com_ptr<IShellItem> item;
    result = dialog->GetResult(item.put());
    if (FAILED(result)) return result;
    PWSTR rawPath = nullptr;
    result = item->GetDisplayName(SIGDN_FILESYSPATH, &rawPath);
    if (SUCCEEDED(result) && rawPath) archivePath = rawPath;
    CoTaskMemFree(rawPath);
    return result;
}

HRESULT BetterPanelStartArchive(
    std::vector<std::wstring> const& sources,
    std::wstring const& creator,
    std::wstring const& archivePath) {
    if (sources.empty() || creator.empty() || archivePath.empty()) {
        return E_INVALIDARG;
    }
    std::wstring lowered = creator;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), towlower);
    std::wstring command = BetterPanelQuoteCommandArgument(creator);
    if (lowered.ends_with(L"winrar.exe")) {
        command += L" a -ep1 -r -ibck " +
                   BetterPanelQuoteCommandArgument(archivePath);
    } else if (lowered.ends_with(L"7z.exe")) {
        command += L" a -y " + BetterPanelQuoteCommandArgument(archivePath);
    } else if (lowered.ends_with(L"wzzip.exe")) {
        command += L" -a " + BetterPanelQuoteCommandArgument(archivePath);
    } else {
        command += L" -a -cf " +
                   BetterPanelQuoteCommandArgument(archivePath);
    }
    for (auto const& source : sources) {
        command += L" " + BetterPanelQuoteCommandArgument(source);
    }
    STARTUPINFOW startup{sizeof(startup)};
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                        &process)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return S_OK;
}

std::wstring BetterPanelFormatTime(int64_t ticks) {
    int64_t totalSeconds = std::max<int64_t>(0, ticks / 10000000);
    int64_t hours = totalSeconds / 3600;
    int64_t minutes = (totalSeconds % 3600) / 60;
    int64_t seconds = totalSeconds % 60;
    WCHAR buffer[32]{};
    if (hours) {
        swprintf_s(buffer, L"%lld:%02lld:%02lld", hours, minutes, seconds);
    } else {
        swprintf_s(buffer, L"%lld:%02lld", minutes, seconds);
    }
    return buffer;
}

winrt::fire_and_forget BetterPanelClearTemporaryMessageAfterDelay(
    winrt::weak_ref<muxc::TextBlock> weakMessage,
    mud::DispatcherQueue dispatcher,
    winrt::hstring temporaryText) {
    co_await winrt::resume_after(std::chrono::seconds(2));
    dispatcher.TryEnqueue([weakMessage, temporaryText]() {
        if (auto message = weakMessage.get();
            message && message.Text() == temporaryText) {
            if (message.Name() == L"BetterPanelDetailsCopyHint") {
                message.Text(L"Select a box to copy");
            } else {
                message.Text(L"");
                if (message.Name() != L"BetterPanelTemporaryStatus") {
                    message.Visibility(Visibility::Collapsed);
                }
            }
        }
    });
}

winrt::fire_and_forget BetterPanelShowTemporaryButtonMessage(
    winrt::weak_ref<muxc::Button> weakButton, std::wstring_view text) {
    auto button = weakButton.get();
    if (!button || text.empty()) co_return;

    muxc::ToolTip tip;
    tip.Content(winrt::box_value(text));
    muxc::ToolTipService::SetToolTip(button, tip);
    tip.IsOpen(true);
    auto weakTip = winrt::make_weak(tip);
    auto dispatcher = button.DispatcherQueue();
    auto restoreText = muxa::AutomationProperties::GetName(button);

    co_await winrt::resume_after(std::chrono::seconds(2));
    dispatcher.TryEnqueue([weakButton, weakTip, restoreText]() {
        if (auto tip = weakTip.get()) tip.IsOpen(false);
        if (auto button = weakButton.get()) {
            muxc::ToolTipService::SetToolTip(button,
                                             winrt::box_value(restoreText));
        }
    });
}

void BetterPanelSetStatus(winrt::weak_ref<muxc::TextBlock> weakStatus,
                          std::wstring_view text) {
    if (auto status = weakStatus.get()) {
        status.Text(text);
        status.Visibility(status.Name() == L"BetterPanelTemporaryStatus"
                              ? Visibility::Visible
                              : (text.empty() ? Visibility::Collapsed
                                              : Visibility::Visible));
        bool temporaryCopyHint =
            status.Name() == L"BetterPanelDetailsCopyHint" &&
            text != L"Select a box to copy";
        bool temporaryPanelMessage =
            (status.Name() == L"BetterPanelTemporaryStatus" ||
             status.Name() == L"BetterPanelTextActionStatus") &&
            !text.empty();
        if (temporaryCopyHint || temporaryPanelMessage) {
            BetterPanelClearTemporaryMessageAfterDelay(
                weakStatus, status.DispatcherQueue(), winrt::hstring(text));
        }
    }
}

struct BetterPanelSyntaxOption {
    BetterPanelSyntaxLanguage language;
    PCWSTR label;
};

constexpr std::array<BetterPanelSyntaxOption, 16> kBetterPanelSyntaxOptions{{
    {BetterPanelSyntaxLanguage::Auto, L"Auto"},
    {BetterPanelSyntaxLanguage::PlainText, L"Plain text"},
    {BetterPanelSyntaxLanguage::Markdown, L"Markdown"},
    {BetterPanelSyntaxLanguage::Cpp, L"C / C++"},
    {BetterPanelSyntaxLanguage::CSharp, L"C#"},
    {BetterPanelSyntaxLanguage::JavaScript, L"JavaScript"},
    {BetterPanelSyntaxLanguage::TypeScript, L"TypeScript"},
    {BetterPanelSyntaxLanguage::Json, L"JSON"},
    {BetterPanelSyntaxLanguage::Python, L"Python"},
    {BetterPanelSyntaxLanguage::PowerShell, L"PowerShell"},
    {BetterPanelSyntaxLanguage::HtmlXml, L"HTML / XML"},
    {BetterPanelSyntaxLanguage::Css, L"CSS"},
    {BetterPanelSyntaxLanguage::Yaml, L"YAML"},
    {BetterPanelSyntaxLanguage::Ini, L"INI / config"},
    {BetterPanelSyntaxLanguage::Batch, L"Batch"},
    {BetterPanelSyntaxLanguage::Sql, L"SQL"},
}};

PCWSTR BetterPanelSyntaxLanguageName(BetterPanelSyntaxLanguage language) {
    for (auto const& option : kBetterPanelSyntaxOptions) {
        if (option.language == language) return option.label;
    }
    return L"Plain text";
}

BetterPanelSyntaxLanguage BetterPanelDetectSyntaxLanguage(
    std::wstring const& path) {
    std::wstring extension = PathFindExtensionW(path.c_str());
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   towlower);
    if (extension == L".md" || extension == L".markdown")
        return BetterPanelSyntaxLanguage::Markdown;
    if (extension == L".c" || extension == L".cc" ||
        extension == L".cpp" || extension == L".cxx" ||
        extension == L".h" || extension == L".hh" ||
        extension == L".hpp") return BetterPanelSyntaxLanguage::Cpp;
    if (extension == L".cs") return BetterPanelSyntaxLanguage::CSharp;
    if (extension == L".js" || extension == L".jsx" ||
        extension == L".mjs" || extension == L".cjs")
        return BetterPanelSyntaxLanguage::JavaScript;
    if (extension == L".ts" || extension == L".tsx")
        return BetterPanelSyntaxLanguage::TypeScript;
    if (extension == L".json" || extension == L".jsonc")
        return BetterPanelSyntaxLanguage::Json;
    if (extension == L".py" || extension == L".pyw")
        return BetterPanelSyntaxLanguage::Python;
    if (extension == L".ps1" || extension == L".psm1" ||
        extension == L".psd1") return BetterPanelSyntaxLanguage::PowerShell;
    if (extension == L".html" || extension == L".htm" ||
        extension == L".xml" || extension == L".xaml" ||
        extension == L".svg") return BetterPanelSyntaxLanguage::HtmlXml;
    if (extension == L".css" || extension == L".scss" ||
        extension == L".less") return BetterPanelSyntaxLanguage::Css;
    if (extension == L".yaml" || extension == L".yml")
        return BetterPanelSyntaxLanguage::Yaml;
    if (extension == L".ini" || extension == L".cfg" ||
        extension == L".conf") return BetterPanelSyntaxLanguage::Ini;
    if (extension == L".bat" || extension == L".cmd")
        return BetterPanelSyntaxLanguage::Batch;
    if (extension == L".sql") return BetterPanelSyntaxLanguage::Sql;
    return BetterPanelSyntaxLanguage::PlainText;
}

enum class BetterPanelSyntaxColor { Keyword, String, Comment, Number, Type };
struct BetterPanelSyntaxSpan {
    size_t start;
    size_t length;
    BetterPanelSyntaxColor color;
};

bool BetterPanelIsSyntaxKeyword(BetterPanelSyntaxLanguage language,
                                std::wstring const& token) {
    static const std::unordered_set<std::wstring> common{
        L"abstract", L"as", L"async", L"await", L"break", L"case",
        L"catch", L"class", L"const", L"continue", L"default", L"do",
        L"else", L"enum", L"export", L"extends", L"false", L"finally",
        L"for", L"foreach", L"from", L"function", L"if", L"import",
        L"in", L"interface", L"internal", L"let", L"namespace", L"new",
        L"null", L"nullptr", L"operator", L"out", L"override", L"private",
        L"protected", L"public", L"readonly", L"return", L"static",
        L"struct", L"switch", L"this", L"throw", L"true", L"try",
        L"typeof", L"using", L"var", L"virtual", L"void", L"while",
        L"yield", L"string", L"int", L"double", L"float", L"bool",
    };
    static const std::unordered_set<std::wstring> python{
        L"and", L"as", L"assert", L"async", L"await", L"break", L"class",
        L"continue", L"def", L"del", L"elif", L"else", L"except", L"False",
        L"finally", L"for", L"from", L"global", L"if", L"import", L"in",
        L"is", L"lambda", L"None", L"nonlocal", L"not", L"or", L"pass",
        L"raise", L"return", L"True", L"try", L"while", L"with", L"yield",
    };
    static const std::unordered_set<std::wstring> sql{
        L"alter", L"and", L"as", L"asc", L"begin", L"by", L"create",
        L"delete", L"desc", L"distinct", L"drop", L"else", L"end", L"from",
        L"group", L"having", L"in", L"insert", L"into", L"is", L"join",
        L"like", L"limit", L"not", L"null", L"on", L"or", L"order",
        L"select", L"set", L"table", L"then", L"union", L"update",
        L"values", L"when", L"where",
    };
    if (language == BetterPanelSyntaxLanguage::Python)
        return python.contains(token);
    if (language == BetterPanelSyntaxLanguage::Sql) {
        std::wstring lowered = token;
        std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                       towlower);
        return sql.contains(lowered);
    }
    return common.contains(token);
}

std::vector<BetterPanelSyntaxSpan> BetterPanelBuildSyntaxSpans(
    std::wstring const& text, BetterPanelSyntaxLanguage language) {
    std::vector<BetterPanelSyntaxSpan> spans;
    if (language == BetterPanelSyntaxLanguage::PlainText ||
        text.size() > 200000) return spans;
    constexpr size_t maxSpans = 4000;
    auto add = [&](size_t start, size_t end, BetterPanelSyntaxColor color) {
        if (end > start && spans.size() < maxSpans)
            spans.push_back({start, end - start, color});
    };

    if (language == BetterPanelSyntaxLanguage::HtmlXml) {
        for (size_t i = 0; i < text.size() && spans.size() < maxSpans;) {
            if (text.compare(i, 4, L"<!--") == 0) {
                size_t end = text.find(L"-->", i + 4);
                end = end == std::wstring::npos ? text.size() : end + 3;
                add(i, end, BetterPanelSyntaxColor::Comment);
                i = end;
            } else if (text[i] == L'<') {
                size_t end = text.find(L'>', i + 1);
                end = end == std::wstring::npos ? text.size() : end + 1;
                add(i, end, BetterPanelSyntaxColor::Type);
                i = end;
            } else {
                ++i;
            }
        }
        return spans;
    }

    if (language == BetterPanelSyntaxLanguage::Markdown) {
        for (size_t i = 0; i < text.size() && spans.size() < maxSpans;) {
            bool lineStart = i == 0 || text[i - 1] == L'\n';
            if (lineStart && text.compare(i, 3, L"```") == 0) {
                size_t end = text.find(L'\n', i);
                end = end == std::wstring::npos ? text.size() : end;
                add(i, end, BetterPanelSyntaxColor::Keyword);
                i = end;
            } else if (lineStart && text[i] == L'#') {
                size_t end = text.find(L'\n', i);
                end = end == std::wstring::npos ? text.size() : end;
                add(i, end, BetterPanelSyntaxColor::Keyword);
                i = end;
            } else if (text[i] == L'`') {
                size_t end = text.find(L'`', i + 1);
                end = end == std::wstring::npos ? text.size() : end + 1;
                add(i, end, BetterPanelSyntaxColor::String);
                i = end;
            } else {
                ++i;
            }
        }
        return spans;
    }

    bool hashComments = language == BetterPanelSyntaxLanguage::Python ||
                        language == BetterPanelSyntaxLanguage::PowerShell ||
                        language == BetterPanelSyntaxLanguage::Yaml;
    bool semicolonComments = language == BetterPanelSyntaxLanguage::Ini;
    for (size_t i = 0; i < text.size() && spans.size() < maxSpans;) {
        if (i + 1 < text.size() && text[i] == L'/' && text[i + 1] == L'/') {
            size_t end = text.find(L'\n', i + 2);
            end = end == std::wstring::npos ? text.size() : end;
            add(i, end, BetterPanelSyntaxColor::Comment);
            i = end;
        } else if (i + 1 < text.size() && text[i] == L'/' &&
                   text[i + 1] == L'*') {
            size_t end = text.find(L"*/", i + 2);
            end = end == std::wstring::npos ? text.size() : end + 2;
            add(i, end, BetterPanelSyntaxColor::Comment);
            i = end;
        } else if ((hashComments && text[i] == L'#') ||
                   (semicolonComments && text[i] == L';')) {
            size_t end = text.find(L'\n', i + 1);
            end = end == std::wstring::npos ? text.size() : end;
            add(i, end, BetterPanelSyntaxColor::Comment);
            i = end;
        } else if (text[i] == L'"' || text[i] == L'\'' ||
                   (language == BetterPanelSyntaxLanguage::PowerShell &&
                    text[i] == L'`')) {
            wchar_t quote = text[i];
            size_t end = i + 1;
            while (end < text.size()) {
                if (text[end] == L'\\') {
                    end += std::min<size_t>(2, text.size() - end);
                } else if (text[end++] == quote) {
                    break;
                } else if (quote == L'`' && text[end - 1] == L'\n') {
                    break;
                }
            }
            add(i, end, BetterPanelSyntaxColor::String);
            i = end;
        } else if (iswdigit(text[i])) {
            size_t end = i + 1;
            while (end < text.size() &&
                   (iswalnum(text[end]) || text[end] == L'.' ||
                    text[end] == L'_')) ++end;
            add(i, end, BetterPanelSyntaxColor::Number);
            i = end;
        } else if (iswalpha(text[i]) || text[i] == L'_') {
            size_t end = i + 1;
            while (end < text.size() &&
                   (iswalnum(text[end]) || text[end] == L'_')) ++end;
            std::wstring token = text.substr(i, end - i);
            if (BetterPanelIsSyntaxKeyword(language, token)) {
                add(i, end, BetterPanelSyntaxColor::Keyword);
            } else if (!token.empty() && iswupper(token.front())) {
                add(i, end, BetterPanelSyntaxColor::Type);
            }
            i = end;
        } else {
            ++i;
        }
    }
    return spans;
}

void BetterPanelRenderSyntaxPreview(
    std::shared_ptr<BetterPanelState> const& state,
    std::wstring const& text) {
    auto preview = state->syntaxPreview.get();
    if (!preview) return;
    auto language = state->syntaxLanguage == BetterPanelSyntaxLanguage::Auto
                        ? BetterPanelDetectSyntaxLanguage(state->selectedPath)
                        : state->syntaxLanguage;
    state->detectedSyntaxLanguage = language;
    if (auto picker = state->syntaxLanguagePicker.get();
        picker && picker.Items().Size() > 0) {
        if (auto autoItem =
                picker.Items().GetAt(0).try_as<muxc::ComboBoxItem>()) {
            std::wstring autoLabel =
                std::wstring(L"Auto - ") +
                BetterPanelSyntaxLanguageName(
                    BetterPanelDetectSyntaxLanguage(state->selectedPath));
            std::wstring currentLabel;
            if (auto content = autoItem.Content()) {
                currentLabel = winrt::unbox_value_or<winrt::hstring>(
                                   content, L"")
                                   .c_str();
            }
            if (currentLabel != autoLabel) {
                // ComboBox caches the closed selection display. Briefly clear
                // and restore Auto so its visible label follows the newly
                // detected file type instead of keeping the first one shown.
                bool autoSelected = picker.SelectedIndex() == 0;
                if (autoSelected) picker.SelectedIndex(-1);
                autoItem.Content(winrt::box_value(autoLabel));
                if (autoSelected) picker.SelectedIndex(0);
            }
        }
    }
    auto spans = BetterPanelBuildSyntaxSpans(text, language);
    preview.Inlines().Clear();
    if (spans.empty()) {
        preview.Text(text);
        return;
    }

    using Brush = winrt::Microsoft::UI::Xaml::Media::SolidColorBrush;
    auto makeBrush = [](BYTE r, BYTE g, BYTE b) {
        return Brush(winrt::Windows::UI::ColorHelper::FromArgb(255, r, g, b));
    };
    Brush keyword = makeBrush(86, 156, 214);
    Brush string = makeBrush(206, 145, 120);
    Brush comment = makeBrush(106, 153, 85);
    Brush number = makeBrush(181, 206, 168);
    Brush type = makeBrush(78, 201, 176);
    size_t position = 0;
    for (auto const& span : spans) {
        if (span.start > position) {
            muxd::Run run;
            run.Text(text.substr(position, span.start - position));
            preview.Inlines().Append(run);
        }
        muxd::Run run;
        run.Text(text.substr(span.start, span.length));
        switch (span.color) {
            case BetterPanelSyntaxColor::Keyword: run.Foreground(keyword); break;
            case BetterPanelSyntaxColor::String: run.Foreground(string); break;
            case BetterPanelSyntaxColor::Comment: run.Foreground(comment); break;
            case BetterPanelSyntaxColor::Number: run.Foreground(number); break;
            case BetterPanelSyntaxColor::Type: run.Foreground(type); break;
        }
        preview.Inlines().Append(run);
        position = span.start + span.length;
    }
    if (position < text.size()) {
        muxd::Run run;
        run.Text(text.substr(position));
        preview.Inlines().Append(run);
    }
}

void BetterPanelUpdateTextControls(
    std::shared_ptr<BetterPanelState> const& state) {
    auto editor = state->textEditor.get();
    if (editor) {
        editor.IsReadOnly(!state->textEditing);
        editor.Visibility(state->textEditing ? Visibility::Visible
                                             : Visibility::Collapsed);
    }
    if (auto preview = state->syntaxPreviewSurface.get()) {
        preview.Visibility(state->textEditing ? Visibility::Collapsed
                                              : Visibility::Visible);
    }
    if (auto button = state->textEditButton.get()) {
        button.Visibility(state->textEditable && !state->textEditing
                              ? Visibility::Visible
                              : Visibility::Collapsed);
    }
    if (auto button = state->textSaveButton.get()) {
        button.Visibility(state->textEditing ? Visibility::Visible
                                             : Visibility::Collapsed);
        button.IsEnabled(state->textDirty);
    }
    if (auto button = state->textCancelButton.get()) {
        button.Visibility(state->textEditing ? Visibility::Visible
                                             : Visibility::Collapsed);
    }
    if (auto button = state->textReloadButton.get()) {
        button.Visibility(!state->textEditing && !state->textLoading
                              ? Visibility::Visible
                              : Visibility::Collapsed);
    }
    if (auto info = state->textInfo.get()) {
        if (state->textLoading) {
            info.Text(L"Loading preview…");
        } else if (!state->textLoadedPath.empty()) {
            std::wstring label = BetterPanelTextEncodingName(
                                     state->textEncoding) +
                                 L" • " +
                                 BetterPanelFormatByteSize(state->textFileSize);
            if (state->textEditing) {
                label += state->textDirty ? L" • Unsaved changes"
                                          : L" • Editing";
            } else if (!state->textEditable) {
                label += L" • Read-only preview";
            }
            if (state->detectedSyntaxLanguage !=
                BetterPanelSyntaxLanguage::PlainText) {
                label += L" • ";
                label += BetterPanelSyntaxLanguageName(
                    state->detectedSyntaxLanguage);
            }
            info.Text(label);
        } else {
            info.Text(L"");
        }
    }
}

void BetterPanelLoadTextPreview(
    std::shared_ptr<BetterPanelState> const& state,
    std::wstring const& path) {
    if (path.empty() || state->textEditing) return;
    auto editor = state->textEditor.get();
    if (!editor) return;

    state->textLoading = true;
    state->textLoadedPath.clear();
    state->textEditable = false;
    state->textDirty = false;
    state->suppressTextChanged = true;
    editor.IsReadOnly(true);
    editor.Text(L"Loading preview…");
    BetterPanelRenderSyntaxPreview(state, L"Loading preview…");
    state->suppressTextChanged = false;
    BetterPanelUpdateTextControls(state);

    auto weakState = std::weak_ptr<BetterPanelState>(state);
    auto dispatcher = state->dispatcher;
    std::thread([weakState, dispatcher, path]() {
        auto data = BetterPanelReadTextFile(path);
        dispatcher.TryEnqueue(
            [weakState, path, data = std::move(data)]() mutable {
                auto state = weakState.lock();
                if (!state || state->unloaded || state->selectedPath != path ||
                    state->textEditing) {
                    return;
                }
                auto editor = state->textEditor.get();
                if (!editor) return;
                state->textLoading = false;
                state->textLoadedPath = path;
                state->textEncoding = data.encoding;
                state->textNewline = data.newline;
                state->textLastWriteTime = data.lastWriteTime;
                state->textFileSize = data.size;
                state->textEditable = data.success && data.editable;
                state->textDirty = false;
                state->suppressTextChanged = true;
                editor.Text(data.success ? data.text : data.message);
                state->suppressTextChanged = false;
                state->textOriginal = data.success
                                          ? std::wstring(editor.Text().c_str())
                                          : L"";
                BetterPanelRenderSyntaxPreview(
                    state, data.success ? data.text : data.message);
                BetterPanelUpdateTextControls(state);
                if (!data.message.empty()) {
                    if (auto info = state->textInfo.get()) {
                        std::wstring existing = info.Text().c_str();
                        info.Text(existing.empty() ? data.message
                                                   : existing + L" • " +
                                                         data.message);
                    }
                }
            });
    }).detach();
}

void BetterPanelBeginTextEdit(
    std::shared_ptr<BetterPanelState> const& state) {
    if (state->textEditing || !state->textEditable ||
        state->textLoadedPath != state->selectedPath) {
        return;
    }
    state->textEditing = true;
    state->textDirty = false;
    if (state->timer) state->timer.Stop();
    BetterPanelUpdateTextControls(state);
    if (auto editor = state->textEditor.get()) {
        editor.Focus(FocusState::Programmatic);
    }
    BetterPanelSetStatus(
        state->textActionStatus,
        L"Editing " + BetterPanelFileName(state->textLoadedPath) +
            L" — Save or Cancel before changing files");
}

void BetterPanelCancelTextEdit(
    std::shared_ptr<BetterPanelState> const& state) {
    if (!state->textEditing) return;
    if (auto editor = state->textEditor.get()) {
        state->suppressTextChanged = true;
        editor.Text(state->textOriginal);
        state->suppressTextChanged = false;
        BetterPanelRenderSyntaxPreview(state, state->textOriginal);
    }
    state->textEditing = false;
    state->textDirty = false;
    BetterPanelUpdateTextControls(state);
    BetterPanelSetStatus(state->textActionStatus, L"Changes discarded");
    if (state->timer) state->timer.Start();
}

void BetterPanelSaveTextEdit(
    std::shared_ptr<BetterPanelState> const& state) {
    auto editor = state->textEditor.get();
    if (!state->textEditing || !state->textDirty || !editor ||
        state->textLoadedPath.empty()) {
        return;
    }

    uint64_t currentSize = 0;
    FILETIME currentWriteTime{};
    if (!BetterPanelGetFileStamp(state->textLoadedPath, currentSize,
                                 currentWriteTime) ||
        currentSize != state->textFileSize ||
        CompareFileTime(&currentWriteTime, &state->textLastWriteTime) != 0) {
        BetterPanelSetStatus(
            state->textActionStatus,
            L"The file changed outside Better Panel. Cancel and reload it before saving.");
        return;
    }

    std::wstring editorText = editor.Text().c_str();
    std::wstring text = BetterPanelNormalizeLineEndings(
        editorText, state->textNewline);
    auto encoding = state->textEncoding;
    DWORD error = ERROR_SUCCESS;
    if (!BetterPanelWriteTextFile(state->textLoadedPath, text, encoding,
                                  error)) {
        BetterPanelSetStatus(state->textActionStatus,
                             L"Save failed (error " +
                                 std::to_wstring(error) + L")");
        return;
    }

    state->textEncoding = encoding;
    state->textOriginal = editorText;
    BetterPanelGetFileStamp(state->textLoadedPath, state->textFileSize,
                            state->textLastWriteTime);
    state->textEditing = false;
    state->textDirty = false;
    BetterPanelRenderSyntaxPreview(state, editorText);
    BetterPanelUpdateTextControls(state);
    BetterPanelSetStatus(
        state->textActionStatus,
        L"Saved " + BetterPanelFileName(state->textLoadedPath));
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW,
                   state->textLoadedPath.c_str(), nullptr);
    if (state->timer) state->timer.Start();
}

winrt::fire_and_forget BetterPanelLoadArtwork(
    std::weak_ptr<BetterPanelState> weakState,
    std::wstring path) {
    try {
        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        auto thumbnail = co_await file.GetThumbnailAsync(
            wsf::ThumbnailMode::MusicView, 256,
            wsf::ThumbnailOptions::UseCurrentScale);
        if (!thumbnail || thumbnail.Type() != wsf::ThumbnailType::Image) {
            co_return;
        }

        winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
        co_await bitmap.SetSourceAsync(thumbnail);

        auto state = weakState.lock();
        if (!state || state->selectedPath != path) {
            co_return;
        }
        if (auto artwork = state->artwork.get()) {
            artwork.Source(bitmap);
        }
        if (auto placeholder = state->artworkPlaceholder.get()) {
            placeholder.Visibility(Visibility::Collapsed);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Artwork error %08X: %s", ex.code(), ex.message().c_str());
    }
}

winrt::fire_and_forget BetterPanelLoadFileIcon(
    std::weak_ptr<BetterPanelState> weakState,
    std::wstring path) {
    try {
        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        auto thumbnail = co_await file.GetThumbnailAsync(
            wsf::ThumbnailMode::SingleItem, 48,
            wsf::ThumbnailOptions::ResizeThumbnail);
        if (!thumbnail) {
            co_return;
        }
        winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
        co_await bitmap.SetSourceAsync(thumbnail);
        auto state = weakState.lock();
        if (!state || state->selectedPath != path) {
            co_return;
        }
        if (auto icon = state->fileIcon.get()) {
            icon.Source(bitmap);
            if (auto fallback = state->fileFallbackIcon.get()) {
                fallback.Visibility(Visibility::Collapsed);
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"File icon error %08X: %s", ex.code(), ex.message().c_str());
    }
}

winrt::fire_and_forget BetterPanelLoadOpenActionIcon(
    std::weak_ptr<BetterPanelState> weakState,
    std::wstring path) {
    try {
        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        // ListView requests the registered Shell/file-type artwork rather than
        // a large content preview, matching the modern context-menu command.
        auto thumbnail = co_await file.GetThumbnailAsync(
            wsf::ThumbnailMode::ListView, 32,
            wsf::ThumbnailOptions::UseCurrentScale);
        if (!thumbnail) {
            thumbnail = co_await file.GetThumbnailAsync(
                wsf::ThumbnailMode::SingleItem, 32,
                wsf::ThumbnailOptions::ResizeThumbnail);
        }
        if (!thumbnail) {
            co_return;
        }
        winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
        co_await bitmap.SetSourceAsync(thumbnail);
        auto state = weakState.lock();
        if (!state || state->selectedPath != path) {
            co_return;
        }
        if (auto icon = state->openActionIcon.get()) {
            icon.Source(bitmap);
            if (auto fallback = state->openActionFallbackIcon.get()) {
                fallback.Visibility(Visibility::Collapsed);
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Open action icon error %08X: %s", ex.code(),
               ex.message().c_str());
    }
}

winrt::fire_and_forget BetterPanelLoadHomeLocationIcon(
    winrt::weak_ref<muxc::Image> weakImage,
    winrt::weak_ref<muxc::FontIcon> weakFallback,
    std::wstring path) {
    try {
        DWORD attributes = GetFileAttributesW(path.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            co_return;
        }

        wsf::StorageItemThumbnail thumbnail{nullptr};
        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
            auto folder = co_await ws::StorageFolder::GetFolderFromPathAsync(path);
            thumbnail = co_await folder.GetThumbnailAsync(
                wsf::ThumbnailMode::ListView, 48,
                wsf::ThumbnailOptions::UseCurrentScale);
        } else {
            auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
            thumbnail = co_await file.GetThumbnailAsync(
                wsf::ThumbnailMode::ListView, 48,
                wsf::ThumbnailOptions::UseCurrentScale);
        }
        if (!thumbnail) {
            co_return;
        }

        winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
        co_await bitmap.SetSourceAsync(thumbnail);
        if (auto image = weakImage.get()) {
            image.Source(bitmap);
            image.Visibility(Visibility::Visible);
            if (auto fallback = weakFallback.get()) {
                fallback.Visibility(Visibility::Collapsed);
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Home Shell icon error %08X: %s", ex.code(),
               ex.message().c_str());
    }
}

winrt::fire_and_forget BetterPanelLoadMultiPreviewIcon(
    std::weak_ptr<BetterPanelState> weakState,
    winrt::weak_ref<muxc::Image> weakImage,
    winrt::weak_ref<muxc::FontIcon> weakFallback,
    std::wstring path,
    std::wstring selectionKey) {
    try {
        DWORD attributes = GetFileAttributesW(path.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            co_return;
        }

        wsf::StorageItemThumbnail thumbnail{nullptr};
        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
            auto folder = co_await ws::StorageFolder::GetFolderFromPathAsync(path);
            thumbnail = co_await folder.GetThumbnailAsync(
                wsf::ThumbnailMode::ListView, 64,
                wsf::ThumbnailOptions::UseCurrentScale);
        } else {
            auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
            thumbnail = co_await file.GetThumbnailAsync(
                wsf::ThumbnailMode::ListView, 64,
                wsf::ThumbnailOptions::UseCurrentScale);
        }
        if (!thumbnail) {
            co_return;
        }

        winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
        co_await bitmap.SetSourceAsync(thumbnail);
        auto state = weakState.lock();
        if (!state || state->unloaded ||
            state->multiPreviewKey != selectionKey) {
            co_return;
        }
        if (auto image = weakImage.get()) {
            image.Source(bitmap);
            image.Visibility(Visibility::Visible);
            if (auto fallback = weakFallback.get()) {
                fallback.Visibility(Visibility::Collapsed);
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Multi-selection icon error %08X: %s", ex.code(),
               ex.message().c_str());
    }
}

void BetterPanelPopulateMultiPreview(
    std::shared_ptr<BetterPanelState> const& state,
    std::vector<std::wstring> const& paths) {
    auto host = state->multiPreviewHost.get();
    if (!host) return;

    std::wstring key;
    for (auto const& path : paths) {
        key += path;
        key.push_back(L'\n');
    }
    if (key == state->multiPreviewKey) return;
    state->multiPreviewKey = key;
    host.Children().Clear();

    std::vector<std::wstring> representativePaths;
    std::unordered_set<std::wstring> representedTypes;
    for (auto const& path : paths) {
        DWORD attributes = GetFileAttributesW(path.c_str());
        bool directory = attributes != INVALID_FILE_ATTRIBUTES &&
                         (attributes & FILE_ATTRIBUTE_DIRECTORY);
        std::wstring typeKey;
        if (directory) {
            typeKey = L"<folder>";
        } else {
            PCWSTR extension = PathFindExtensionW(path.c_str());
            typeKey = extension && *extension ? extension : L"<file>";
            std::transform(typeKey.begin(), typeKey.end(), typeKey.begin(),
                           [](wchar_t ch) { return std::towlower(ch); });
        }
        if (representedTypes.insert(typeKey).second) {
            representativePaths.push_back(path);
            if (representativePaths.size() == 4) break;
        }
    }

    size_t visibleCount = representativePaths.size();
    if (visibleCount == 0) {
        host.Width(0);
        host.Height(0);
        return;
    }
    constexpr double iconSize = 56.0;
    constexpr double horizontalOffset = 24.0;
    constexpr double verticalOffset = 4.0;
    host.Width(iconSize +
               horizontalOffset * static_cast<double>(visibleCount - 1));
    host.Height(iconSize +
                verticalOffset * static_cast<double>(visibleCount - 1));
    for (size_t index = 0; index < visibleCount; ++index) {
        auto const& path = representativePaths[index];
        DWORD attributes = GetFileAttributesW(path.c_str());
        bool directory = attributes != INVALID_FILE_ATTRIBUTES &&
                         (attributes & FILE_ATTRIBUTE_DIRECTORY);

        muxc::Grid iconHost;
        iconHost.Width(iconSize);
        iconHost.Height(iconSize);
        iconHost.HorizontalAlignment(HorizontalAlignment::Left);
        iconHost.VerticalAlignment(VerticalAlignment::Top);
        iconHost.Margin(Thickness{
            horizontalOffset * static_cast<double>(index),
            verticalOffset * static_cast<double>(index), 0, 0});
        muxc::Canvas::SetZIndex(iconHost, static_cast<int32_t>(index));
        muxc::ToolTipService::SetToolTip(
            iconHost, winrt::box_value(BetterPanelFileName(path)));

        muxc::FontIcon fallback;
        fallback.Glyph(directory ? L"\uE8B7" : L"\uE7C3");
        fallback.FontSize(34);
        fallback.Opacity(0.80);
        iconHost.Children().Append(fallback);

        muxc::Image image;
        image.Width(48);
        image.Height(48);
        image.Stretch(
            winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
        image.Visibility(Visibility::Collapsed);
        iconHost.Children().Append(image);
        host.Children().Append(iconHost);

        BetterPanelLoadMultiPreviewIcon(
            std::weak_ptr<BetterPanelState>(state), winrt::make_weak(image),
            winrt::make_weak(fallback), path, key);
    }
}

std::wstring BetterPanelTransferItemsText(
    std::vector<std::wstring> const& sources) {
    std::wstring text;
    for (auto const& source : sources) {
        std::wstring name = BetterPanelFileName(source);
        if (name.empty()) name = source;
        if (!text.empty()) text += L"\r\n";
        text += L"\u2022 " + name;
    }
    return text;
}

winrt::fire_and_forget BetterPanelLoadVideo(
    std::weak_ptr<BetterPanelState> weakState,
    std::wstring path) {
    try {
        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        auto source = wmc::MediaSource::CreateFromStorageFile(file);
        auto state = weakState.lock();
        if (!state || state->selectedPath != path) {
            co_return;
        }
        if (auto player = state->videoPlayer.get()) {
            player.Source(source);
        }
    } catch (winrt::hresult_error const& ex) {
        auto state = weakState.lock();
        if (state) {
            BetterPanelSetStatus(state->status, L"Video preview unavailable");
        }
        Wh_Log(L"Video playback error %08X: %s", ex.code(),
               ex.message().c_str());
    }
}

winrt::fire_and_forget BetterPanelLoadAnimatedGif(
    std::weak_ptr<BetterPanelState> weakState,
    std::wstring path) {
    try {
        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        auto stream = co_await file.OpenAsync(ws::FileAccessMode::Read);
        winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
        bitmap.AutoPlay(true);
        co_await bitmap.SetSourceAsync(stream);

        auto state = weakState.lock();
        if (!state || state->selectedPath != path) {
            co_return;
        }
        state->animatedGif = bitmap;
        if (auto image = state->gifImage.get()) {
            image.Source(bitmap);
        }
        if (bitmap.IsAnimatedBitmap() && !bitmap.IsPlaying()) {
            bitmap.Play();
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Animated GIF error %08X: %s", ex.code(),
               ex.message().c_str());
    }
}

void BetterPanelUpdatePdfControls(
    std::shared_ptr<BetterPanelState> const& state) {
    uint32_t pageCount = state->pdfDocument ? state->pdfDocument.PageCount() : 0;
    if (auto pageText = state->pdfPageText.get()) {
        pageText.Text(pageCount
                          ? L"Page " + std::to_wstring(state->pdfPageIndex + 1) +
                                L" of " + std::to_wstring(pageCount)
                          : L"Page -- of --");
    }
    if (auto zoomText = state->pdfZoomText.get()) {
        zoomText.Text(std::to_wstring(
                          static_cast<int>(std::lround(state->pdfZoom * 100))) +
                      L"%");
    }
    if (auto previous = state->pdfPreviousButton.get()) {
        previous.IsEnabled(!state->pdfLoading && pageCount > 0 &&
                           state->pdfPageIndex > 0);
    }
    if (auto next = state->pdfNextButton.get()) {
        next.IsEnabled(!state->pdfLoading && pageCount > 0 &&
                       state->pdfPageIndex + 1 < pageCount);
    }
}

winrt::fire_and_forget BetterPanelRenderPdfPage(
    std::weak_ptr<BetterPanelState> weakState,
    std::wstring path,
    uint32_t pageIndex,
    double zoom) {
    auto state = weakState.lock();
    if (!state || !state->pdfDocument || state->selectedPath != path ||
        pageIndex >= state->pdfDocument.PageCount()) {
        co_return;
    }
    auto document = state->pdfDocument;
    state->pdfLoading = true;
    if (auto info = state->pdfInfo.get()) {
        info.Text(L"Rendering page…");
        info.Visibility(Visibility::Visible);
    }
    BetterPanelUpdatePdfControls(state);

    try {
        auto page = document.GetPage(pageIndex);
        auto pageBox = page.Dimensions().MediaBox();
        uint32_t destinationWidth = static_cast<uint32_t>(std::clamp(
            state->pdfBaseWidth * zoom * 1.75, 420.0, 2400.0));
        uint32_t destinationHeight = static_cast<uint32_t>(std::max(
            1.0, static_cast<double>(destinationWidth) * pageBox.Height /
                     pageBox.Width));
        wdp::PdfPageRenderOptions options;
        options.DestinationWidth(destinationWidth);
        options.DestinationHeight(destinationHeight);
        wss::InMemoryRandomAccessStream stream;
        co_await page.RenderToStreamAsync(stream, options);
        stream.Seek(0);
        winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
        co_await bitmap.SetSourceAsync(stream);
        page.Close();

        state = weakState.lock();
        if (!state || state->selectedPath != path ||
            state->pdfLoadedPath != path || state->pdfPageIndex != pageIndex ||
            std::abs(state->pdfZoom - zoom) > 0.001) {
            co_return;
        }
        if (auto image = state->pdfImage.get()) {
            image.Width(state->pdfBaseWidth * zoom);
            image.Source(bitmap);
        }
        if (auto info = state->pdfInfo.get()) {
            info.Text(L"");
            info.Visibility(Visibility::Collapsed);
        }
    } catch (winrt::hresult_error const& ex) {
        state = weakState.lock();
        if (state && state->selectedPath == path) {
            if (auto info = state->pdfInfo.get()) {
                info.Text(L"This PDF page could not be displayed");
                info.Visibility(Visibility::Visible);
            }
        }
        Wh_Log(L"PDF render error %08X: %s", ex.code(),
               ex.message().c_str());
    }
    state = weakState.lock();
    if (state && state->selectedPath == path &&
        state->pdfPageIndex == pageIndex) {
        state->pdfLoading = false;
        BetterPanelUpdatePdfControls(state);
    }
}

winrt::fire_and_forget BetterPanelLoadPdf(
    std::weak_ptr<BetterPanelState> weakState,
    std::wstring path) {
    try {
        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        auto document = co_await wdp::PdfDocument::LoadFromFileAsync(file);
        auto state = weakState.lock();
        if (!state || state->selectedPath != path) {
            co_return;
        }
        state->pdfDocument = document;
        state->pdfLoadedPath = path;
        state->pdfPageIndex = 0;
        state->pdfZoom = 1.0;
        if (auto host = state->host.get()) {
            state->pdfBaseWidth = std::clamp(host.ActualWidth() - 52.0,
                                             260.0, 680.0);
        }
        BetterPanelUpdatePdfControls(state);
        BetterPanelRenderPdfPage(weakState, path, 0, state->pdfZoom);
    } catch (winrt::hresult_error const& ex) {
        auto state = weakState.lock();
        if (state && state->selectedPath == path) {
            state->pdfLoading = false;
            state->pdfDocument = nullptr;
            if (auto info = state->pdfInfo.get()) {
                info.Text(L"PDF preview unavailable (the file may be protected or damaged)");
                info.Visibility(Visibility::Visible);
            }
            BetterPanelUpdatePdfControls(state);
        }
        Wh_Log(L"PDF load error %08X: %s", ex.code(), ex.message().c_str());
    }
}

winrt::fire_and_forget BetterPanelLoadAndPlay(
    std::wstring path,
    winrt::weak_ref<muxc::TextBlock> weakStatus) {
    try {
        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        auto source = wmc::MediaSource::CreateFromStorageFile(file);

        {
            std::lock_guard lock(g_betterMediaMutex);
            if (!g_betterMediaPlayer) {
                g_betterMediaPlayer = wmp::MediaPlayer();
                g_betterMediaPlayer.AutoPlay(false);
            }
            g_betterMediaPlayer.Volume(g_betterMediaVolume);
            g_betterMediaPlayer.IsMuted(g_betterMediaMuted);
            g_betterMediaPlayer.IsLoopingEnabled(g_betterRepeatEnabled);
            g_betterMediaPath = path;
            g_betterMediaPlayer.Source(source);
            g_betterMediaPlayer.PlaybackSession().PlaybackRate(
                g_betterPlaybackRate);
            g_betterMediaPlayer.Play();
        }
        BetterPanelSetStatus(weakStatus, L"Playing");
    } catch (winrt::hresult_error const& ex) {
        BetterPanelSetStatus(weakStatus, ex.message());
        Wh_Log(L"Playback error %08X: %s", ex.code(), ex.message().c_str());
    }
}

void BetterPanelTogglePlayback(const std::wstring& path,
                               winrt::weak_ref<muxc::TextBlock> weakStatus) {
    if (!BetterPanelIsAudioFile(path)) {
        BetterPanelSetStatus(weakStatus, L"Select one supported audio file");
        return;
    }

    {
        std::lock_guard lock(g_betterMediaMutex);
        if (g_betterMediaPlayer && g_betterMediaPath == path) {
            auto state = g_betterMediaPlayer.PlaybackSession().PlaybackState();
            if (state == wmp::MediaPlaybackState::Playing) {
                g_betterMediaPlayer.Pause();
                BetterPanelSetStatus(weakStatus, L"Paused");
            } else {
                g_betterMediaPlayer.Play();
                BetterPanelSetStatus(weakStatus, L"Playing");
            }
            return;
        }
    }

    BetterPanelSetStatus(weakStatus, L"Loading audio…");
    BetterPanelLoadAndPlay(path, weakStatus);
}

void BetterPanelSeek(int seconds,
                     winrt::weak_ref<muxc::TextBlock> weakStatus) {
    try {
        std::lock_guard lock(g_betterMediaMutex);
        if (!g_betterMediaPlayer) {
            BetterPanelSetStatus(weakStatus, L"No audio is loaded");
            return;
        }

        auto session = g_betterMediaPlayer.PlaybackSession();
        if (!session.CanSeek()) {
            BetterPanelSetStatus(weakStatus, L"This audio cannot be seeked");
            return;
        }

        int64_t duration = session.NaturalDuration().count();
        int64_t position = session.Position().count() +
                           static_cast<int64_t>(seconds) * 10000000;
        position = std::clamp<int64_t>(position, 0, duration);
        session.Position(winrt::Windows::Foundation::TimeSpan{position});
        BetterPanelSetStatus(weakStatus,
                             seconds < 0 ? L"Back 10 seconds"
                                         : L"Forward 10 seconds");
    } catch (winrt::hresult_error const& ex) {
        BetterPanelSetStatus(weakStatus, ex.message());
    }
}

void BetterPanelCloseMedia() {
    std::lock_guard lock(g_betterMediaMutex);
    if (g_betterMediaPlayer) {
        try {
            g_betterMediaPlayer.Pause();
            g_betterMediaPlayer.Source(nullptr);
            g_betterMediaPlayer.Close();
        } catch (...) {
        }
        g_betterMediaPlayer = nullptr;
    }
    g_betterMediaPath.clear();
}

void BetterPanelStopStateMedia(
    std::shared_ptr<BetterPanelState> const& state,
    std::wstring_view audioPath = {}) {
    if (!state) {
        return;
    }
    try {
        if (auto player = state->videoPlayer.get()) {
            player.Source(nullptr);
        }
    } catch (...) {
    }

    std::wstring_view path = audioPath.empty()
                                 ? std::wstring_view(state->selectedPath)
                                 : audioPath;
    bool closeAudio = false;
    {
        std::lock_guard lock(g_betterMediaMutex);
        closeAudio = !path.empty() && g_betterMediaPath == path;
    }
    if (closeAudio) {
        BetterPanelCloseMedia();
    }
}

muxc::Button BetterPanelMakeButton(PCWSTR label) {
    muxc::Button button;
    button.Content(winrt::box_value(label));
    button.MinWidth(0);
    button.Padding(Thickness{10, 5, 10, 5});
    button.HorizontalAlignment(HorizontalAlignment::Stretch);
    return button;
}

muxc::Button BetterPanelMakeIconButton(PCWSTR label, PCWSTR glyph) {
    muxc::StackPanel content;
    content.Orientation(muxc::Orientation::Horizontal);
    content.Spacing(6);

    muxc::FontIcon icon;
    icon.Glyph(glyph);
    icon.FontSize(13);
    content.Children().Append(icon);

    muxc::TextBlock text;
    text.Text(label);
    text.FontSize(12);
    text.VerticalAlignment(VerticalAlignment::Center);
    content.Children().Append(text);

    auto button = BetterPanelMakeButton(L"");
    button.Content(content);
    button.Padding(Thickness{10, 4, 10, 4});
    button.Height(32);
    return button;
}

void BetterPanelSetIconButtonLabel(muxc::Button const& button,
                                   std::wstring const& label) {
    if (!button) return;
    if (auto content = button.Content().try_as<muxc::StackPanel>();
        content && content.Children().Size() > 1) {
        if (auto text = content.Children().GetAt(1).try_as<muxc::TextBlock>()) {
            if (text.Text() != label) text.Text(label);
        }
    }
    muxa::AutomationProperties::SetName(button, label);
    muxc::ToolTipService::SetToolTip(button, winrt::box_value(label));
}

std::wstring BetterPanelDestinationLabel(std::wstring const& path) {
    if (path.empty()) return {};
    if (PathIsRootW(path.c_str())) {
        WCHAR volumeName[MAX_PATH]{};
        GetVolumeInformationW(path.c_str(), volumeName, ARRAYSIZE(volumeName),
                              nullptr, nullptr, nullptr, nullptr, 0);
        std::wstring label = volumeName[0] ? volumeName : L"Local Disk";
        if (path.size() >= 2) {
            label += L" (" + path.substr(0, 2) + L")";
        }
        return label;
    }
    auto label = BetterPanelFileName(path);
    return label.empty() ? path : label;
}

std::wstring BetterPanelTransferMoveLabel(
    std::vector<std::wstring> const& sources,
    std::wstring const& destination) {
    if (sources.empty() || destination.empty()) return L"Move to";
    std::wstring sourceLabel;
    if (sources.size() == 1) {
        sourceLabel = BetterPanelFileName(sources.front());
        if (sourceLabel.empty()) sourceLabel = sources.front();
    } else {
        sourceLabel = std::to_wstring(sources.size()) + L" items";
    }
    return L"Move " + sourceLabel + L" to " +
           BetterPanelDestinationLabel(destination);
}

struct BetterPanelHomeLocation {
    std::wstring name;
    std::wstring path;
    std::wstring glyph;
    std::wstring description;
};

std::wstring BetterPanelKnownFolderPath(REFKNOWNFOLDERID id) {
    PWSTR rawPath = nullptr;
    if (FAILED(SHGetKnownFolderPath(id, KF_FLAG_DEFAULT, nullptr, &rawPath)) ||
        !rawPath) {
        return {};
    }
    std::wstring path(rawPath);
    CoTaskMemFree(rawPath);
    return path;
}

bool BetterPanelPathsEqual(std::wstring const& left,
                           std::wstring const& right) {
    if (left.empty() || right.empty()) return false;
    std::wstring leftFull(MAX_PATH, L'\0');
    std::wstring rightFull(MAX_PATH, L'\0');
    DWORD leftLength = GetFullPathNameW(left.c_str(),
                                        static_cast<DWORD>(leftFull.size()),
                                        leftFull.data(), nullptr);
    DWORD rightLength = GetFullPathNameW(right.c_str(),
                                         static_cast<DWORD>(rightFull.size()),
                                         rightFull.data(), nullptr);
    if (!leftLength || leftLength >= leftFull.size() || !rightLength ||
        rightLength >= rightFull.size()) {
        return _wcsicmp(left.c_str(), right.c_str()) == 0;
    }
    leftFull.resize(leftLength);
    rightFull.resize(rightLength);
    while (leftFull.size() > 3 &&
           (leftFull.back() == L'\\' || leftFull.back() == L'/')) {
        leftFull.pop_back();
    }
    while (rightFull.size() > 3 &&
           (rightFull.back() == L'\\' || rightFull.back() == L'/')) {
        rightFull.pop_back();
    }
    return _wcsicmp(leftFull.c_str(), rightFull.c_str()) == 0;
}

bool BetterPanelIsProtectedDeleteLocation(std::wstring const& path) {
    if (path.empty() || PathIsRootW(path.c_str())) return true;

    static std::vector<std::wstring> const protectedFolders = [] {
        std::vector<std::wstring> folders;
        auto addKnownFolder = [&folders](REFKNOWNFOLDERID id) {
            auto folder = BetterPanelKnownFolderPath(id);
            if (!folder.empty()) folders.push_back(std::move(folder));
        };
        addKnownFolder(FOLDERID_Profile);
        addKnownFolder(FOLDERID_Desktop);
        addKnownFolder(FOLDERID_Documents);
        addKnownFolder(FOLDERID_Downloads);
        addKnownFolder(FOLDERID_Music);
        addKnownFolder(FOLDERID_Pictures);
        addKnownFolder(FOLDERID_Videos);
        addKnownFolder(FOLDERID_Public);
        addKnownFolder(FOLDERID_ProgramData);
        addKnownFolder(FOLDERID_ProgramFiles);
        addKnownFolder(FOLDERID_ProgramFilesX86);
        addKnownFolder(FOLDERID_Windows);
        addKnownFolder(FOLDERID_System);
        addKnownFolder(FOLDERID_SystemX86);
        return folders;
    }();

    return std::any_of(protectedFolders.begin(), protectedFolders.end(),
                       [&path](auto const& protectedPath) {
                           return BetterPanelPathsEqual(path, protectedPath);
                       });
}

bool BetterPanelCanDeleteSelectedPath(std::wstring const& path) {
    if (BetterPanelIsProtectedDeleteLocation(path)) return false;
    winrt::com_ptr<IShellItem> item;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                           IID_PPV_ARGS(item.put())))) {
        return false;
    }
    SFGAOF attributes = SFGAO_CANDELETE | SFGAO_FILESYSTEM;
    if (FAILED(item->GetAttributes(attributes, &attributes))) return false;
    return (attributes & SFGAO_CANDELETE) != 0 &&
           (attributes & SFGAO_FILESYSTEM) != 0;
}

std::vector<BetterPanelHomeLocation> BetterPanelRecentFolders() {
    struct LinkEntry {
        std::wstring path;
        FILETIME modified{};
    };
    std::vector<LinkEntry> links;
    auto recentPath = BetterPanelKnownFolderPath(FOLDERID_Recent);
    if (recentPath.empty()) return {};

    WIN32_FIND_DATAW data{};
    HANDLE find = FindFirstFileExW((recentPath + L"\\*.lnk").c_str(),
                                   FindExInfoBasic, &data,
                                   FindExSearchNameMatch, nullptr,
                                   FIND_FIRST_EX_LARGE_FETCH);
    if (find == INVALID_HANDLE_VALUE) return {};
    do {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            links.push_back({recentPath + L"\\" + data.cFileName,
                             data.ftLastWriteTime});
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    std::sort(links.begin(), links.end(), [](auto const& left, auto const& right) {
        return CompareFileTime(&left.modified, &right.modified) > 0;
    });

    std::vector<BetterPanelHomeLocation> folders;
    std::unordered_set<std::wstring> seen;
    size_t checked = 0;
    for (auto const& linkEntry : links) {
        if (folders.size() >= 8 || checked++ >= 100) break;
        winrt::com_ptr<IShellLinkW> link;
        if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr,
                                    CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(link.put()))) ||
            !link) {
            continue;
        }
        auto persist = link.try_as<IPersistFile>();
        if (!persist || FAILED(persist->Load(linkEntry.path.c_str(), STGM_READ))) {
            continue;
        }
        WCHAR target[MAX_PATH]{};
        WIN32_FIND_DATAW targetData{};
        if (FAILED(link->GetPath(target, ARRAYSIZE(target), &targetData,
                                 SLGP_RAWPATH)) ||
            !target[0]) {
            continue;
        }
        DWORD attributes = GetFileAttributesW(target);
        if (attributes == INVALID_FILE_ATTRIBUTES ||
            !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            continue;
        }
        std::wstring key(target);
        std::transform(key.begin(), key.end(), key.begin(), towlower);
        if (!seen.insert(key).second) continue;
        auto name = BetterPanelFileName(target);
        if (name.empty()) name = target;
        folders.push_back({std::move(name), target, L"\uE8B7"});
    }
    return folders;
}

std::vector<BetterPanelHomeLocation> BetterPanelFavoriteFiles() {
    struct DestinationFile {
        std::wstring path;
        FILETIME modified{};
    };
    std::vector<DestinationFile> destinationFiles;
    std::wstring explorerList = BetterPanelFavoritesListPath();
    size_t separator = explorerList.find_last_of(L"\\/");
    if (separator == std::wstring::npos) return {};
    std::wstring directory = explorerList.substr(0, separator);

    WIN32_FIND_DATAW findData{};
    HANDLE find = FindFirstFileExW(
        (directory + L"\\*.automaticDestinations-ms").c_str(),
        FindExInfoBasic, &findData, FindExSearchNameMatch, nullptr,
        FIND_FIRST_EX_LARGE_FETCH);
    if (find == INVALID_HANDLE_VALUE) return {};
    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            destinationFiles.push_back(
                {directory + L"\\" + findData.cFileName,
                 findData.ftLastWriteTime});
        }
    } while (FindNextFileW(find, &findData));
    FindClose(find);
    std::sort(destinationFiles.begin(), destinationFiles.end(),
              [](auto const& left, auto const& right) {
                  return CompareFileTime(&left.modified, &right.modified) > 0;
              });

    std::vector<BetterPanelHomeLocation> favorites;
    std::unordered_set<std::wstring> seen;
    for (auto const& destination : destinationFiles) {
        if (favorites.size() >= 50) break;
        winrt::com_ptr<IStorage> storage;
        if (FAILED(StgOpenStorage(
                destination.path.c_str(), nullptr,
                STGM_READ | STGM_SHARE_DENY_WRITE, nullptr, 0,
                storage.put())) || !storage) {
            continue;
        }
        winrt::com_ptr<IStream> stream;
        if (FAILED(storage->OpenStream(
                L"DestList", nullptr, STGM_READ | STGM_SHARE_EXCLUSIVE,
                0, stream.put())) || !stream) {
            continue;
        }
        STATSTG stat{};
        if (FAILED(stream->Stat(&stat, STATFLAG_NONAME)) ||
            stat.cbSize.QuadPart < 32 ||
            stat.cbSize.QuadPart > 16 * 1024 * 1024) {
            continue;
        }
        std::vector<uint8_t> bytes(
            static_cast<size_t>(stat.cbSize.QuadPart));
        ULONG bytesRead = 0;
        if (FAILED(stream->Read(bytes.data(),
                                static_cast<ULONG>(bytes.size()),
                                &bytesRead)) || bytesRead < 32) {
            continue;
        }
        bytes.resize(bytesRead);
        uint32_t version = BetterPanelReadUInt32(bytes, 0);
        uint32_t entryCount = BetterPanelReadUInt32(bytes, 4);
        if (version < 3 || entryCount > 10000) continue;

        size_t offset = 32;
        for (uint32_t index = 0;
             index < entryCount && offset + 130 <= bytes.size(); index++) {
            uint16_t pathLength = BetterPanelReadUInt16(bytes, offset + 128);
            size_t pathBytes = static_cast<size_t>(pathLength) * 2;
            if (offset + 130 + pathBytes > bytes.size()) break;
            uint32_t pinStatus = BetterPanelReadUInt32(bytes, offset + 108);
            if (pinStatus != UINT32_MAX && pathLength) {
                std::wstring path;
                path.reserve(pathLength);
                for (uint16_t character = 0; character < pathLength;
                     character++) {
                    path.push_back(static_cast<wchar_t>(
                        BetterPanelReadUInt16(
                            bytes, offset + 130 +
                                       static_cast<size_t>(character) * 2)));
                }
                DWORD attributes = GetFileAttributesW(path.c_str());
                if (attributes != INVALID_FILE_ATTRIBUTES) {
                    std::wstring key = path;
                    std::transform(key.begin(), key.end(), key.begin(),
                                   towlower);
                    if (seen.insert(key).second) {
                        std::wstring name = BetterPanelFileName(path);
                        if (name.empty()) name = path;
                        bool folder =
                            (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                        favorites.push_back(
                            {std::move(name), path,
                             folder ? L"\uE8B7" : L"\uE8A5"});
                        if (favorites.size() >= 50) break;
                    }
                }
            }
            offset += 130 + pathBytes + 4;
        }
    }
    return favorites;
}

void BetterPanelNavigateFromHome(
    std::weak_ptr<BetterPanelState> weakState,
    std::wstring target) {
    auto state = weakState.lock();
    if (!state || target.empty()) return;
    if (auto homeCard = state->homeCard.get()) {
        homeCard.Visibility(Visibility::Collapsed);
    }
    state->homeWasVisible = false;
    BetterPanelInvalidateExplorerQueryCaches();
    if (!BetterPanelNavigateCurrentTab(target)) {
        ShellExecuteW(GetForegroundWindow(), L"open", target.c_str(), nullptr,
                      nullptr, SW_SHOWNORMAL);
    }

    auto dispatcher = state->dispatcher;
    std::thread([weakState, dispatcher]() {
        Sleep(120);
        dispatcher.TryEnqueue([weakState]() {
            if (auto state = weakState.lock(); state && !state->unloaded) {
                BetterPanelInvalidateExplorerQueryCaches();
                BetterPanelRefresh(state);
            }
        });
    }).detach();
}

muxc::Button BetterPanelMakeHomeLocationButton(
    BetterPanelHomeLocation const& location,
    std::weak_ptr<BetterPanelState> weakState) {
    muxc::Grid content;
    muxc::ColumnDefinition iconColumn;
    iconColumn.Width(GridLength{34, GridUnitType::Pixel});
    muxc::ColumnDefinition textColumn;
    textColumn.Width(GridLength{1, GridUnitType::Star});
    content.ColumnDefinitions().Append(iconColumn);
    content.ColumnDefinitions().Append(textColumn);

    muxc::Grid iconHost;
    iconHost.Width(24);
    iconHost.Height(24);
    iconHost.HorizontalAlignment(HorizontalAlignment::Center);
    iconHost.VerticalAlignment(VerticalAlignment::Center);

    muxc::FontIcon fallbackIcon;
    fallbackIcon.Glyph(location.glyph);
    fallbackIcon.FontSize(20);
    fallbackIcon.HorizontalAlignment(HorizontalAlignment::Center);
    fallbackIcon.VerticalAlignment(VerticalAlignment::Center);
    iconHost.Children().Append(fallbackIcon);

    muxc::Image shellIcon;
    shellIcon.Width(24);
    shellIcon.Height(24);
    shellIcon.Stretch(
        winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
    shellIcon.HorizontalAlignment(HorizontalAlignment::Center);
    shellIcon.VerticalAlignment(VerticalAlignment::Center);
    shellIcon.Visibility(Visibility::Collapsed);
    iconHost.Children().Append(shellIcon);
    content.Children().Append(iconHost);

    muxc::StackPanel labels;
    labels.Spacing(1);
    muxc::Grid::SetColumn(labels, 1);
    muxc::TextBlock name;
    name.Text(location.name);
    name.FontWeight(winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    muxc::TextBlock path;
    path.Text(location.description.empty() ? location.path
                                           : location.description);
    path.FontSize(10);
    path.Opacity(0.65);
    path.TextTrimming(TextTrimming::CharacterEllipsis);
    labels.Children().Append(name);
    labels.Children().Append(path);
    content.Children().Append(labels);

    auto button = BetterPanelMakeButton(L"");
    button.Content(content);
    button.HorizontalContentAlignment(HorizontalAlignment::Stretch);
    button.Padding(Thickness{10, 7, 10, 7});
    button.Click([weakState, target = location.path](auto const&,
                                                     RoutedEventArgs const&) {
        BetterPanelNavigateFromHome(weakState, target);
    });
    BetterPanelLoadHomeLocationIcon(winrt::make_weak(shellIcon),
                                    winrt::make_weak(fallbackIcon),
                                    location.path);
    return button;
}

muxc::Button BetterPanelMakeDriveButton(std::wstring const& name,
                                         std::wstring const& path,
                                         uint64_t freeBytes,
                                         uint64_t totalBytes,
                                         std::weak_ptr<BetterPanelState> weakState) {
    muxc::Grid content;
    muxc::ColumnDefinition iconColumn;
    iconColumn.Width(GridLength{52, GridUnitType::Pixel});
    muxc::ColumnDefinition detailsColumn;
    detailsColumn.Width(GridLength{1, GridUnitType::Star});
    content.ColumnDefinitions().Append(iconColumn);
    content.ColumnDefinitions().Append(detailsColumn);

    muxc::Grid iconHost;
    iconHost.Width(44);
    iconHost.Height(44);
    iconHost.HorizontalAlignment(HorizontalAlignment::Center);
    iconHost.VerticalAlignment(VerticalAlignment::Center);

    muxc::FontIcon fallbackIcon;
    fallbackIcon.Glyph(L"\uEDA2");
    fallbackIcon.FontSize(30);
    fallbackIcon.HorizontalAlignment(HorizontalAlignment::Center);
    fallbackIcon.VerticalAlignment(VerticalAlignment::Center);
    iconHost.Children().Append(fallbackIcon);

    muxc::Image shellIcon;
    shellIcon.Width(40);
    shellIcon.Height(40);
    shellIcon.Stretch(
        winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
    shellIcon.HorizontalAlignment(HorizontalAlignment::Center);
    shellIcon.VerticalAlignment(VerticalAlignment::Center);
    shellIcon.Visibility(Visibility::Collapsed);
    iconHost.Children().Append(shellIcon);
    content.Children().Append(iconHost);

    muxc::StackPanel details;
    details.Spacing(1);
    muxc::Grid::SetColumn(details, 1);
    muxc::TextBlock title;
    title.Text(name);
    title.FontSize(13);
    title.FontWeight(winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    title.TextTrimming(TextTrimming::CharacterEllipsis);
    details.Children().Append(title);

    muxc::ProgressBar capacity;
    capacity.Minimum(0);
    capacity.Maximum(100);
    double usedPercent = totalBytes
                             ? 100.0 * static_cast<double>(totalBytes - freeBytes) /
                                   static_cast<double>(totalBytes)
                             : 0.0;
    capacity.Value(std::clamp(usedPercent, 0.0, 100.0));
    capacity.Height(12);
    capacity.HorizontalAlignment(HorizontalAlignment::Stretch);
    details.Children().Append(capacity);

    muxc::TextBlock capacityText;
    capacityText.Text(totalBytes
                          ? BetterPanelFormatByteSize(freeBytes) + L" free of " +
                                BetterPanelFormatByteSize(totalBytes)
                          : path);
    capacityText.FontSize(10.5);
    capacityText.Opacity(0.70);
    capacityText.TextTrimming(TextTrimming::CharacterEllipsis);
    details.Children().Append(capacityText);
    content.Children().Append(details);

    auto button = BetterPanelMakeButton(L"");
    button.Content(content);
    button.Background(nullptr);
    button.BorderThickness(Thickness{0});
    button.CornerRadius(CornerRadius{4});
    button.HorizontalContentAlignment(HorizontalAlignment::Stretch);
    button.Padding(Thickness{6, 6, 8, 6});
    button.MinHeight(64);
    button.Click([weakState, target = path](auto const&,
                                           RoutedEventArgs const&) {
        BetterPanelNavigateFromHome(weakState, target);
    });
    BetterPanelLoadHomeLocationIcon(winrt::make_weak(shellIcon),
                                    winrt::make_weak(fallbackIcon), path);
    return button;
}

void BetterPanelPopulateHome(std::shared_ptr<BetterPanelState> const& state) {
    if (!state || state->homeContentLoaded) return;
    auto content = state->homeContent.get();
    if (!content) return;
    content.Children().Clear();

    bool showFavorites =
        Wh_GetIntValue(L"betterPanelHomeList", 0) == 1;

    muxc::TextBlock homeTitle;
    homeTitle.Text(L"This PC");
    homeTitle.FontSize(18);
    homeTitle.FontWeight(
        winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    homeTitle.Margin(Thickness{2, 0, 0, 0});
    content.Children().Append(homeTitle);

    muxc::TextBlock homeSubtitle;
    homeSubtitle.Text(showFavorites ? L"Drives and favorite files"
                                    : L"Drives and recent locations");
    homeSubtitle.FontSize(11);
    homeSubtitle.Opacity(0.66);
    homeSubtitle.Margin(Thickness{2, 0, 0, 8});
    content.Children().Append(homeSubtitle);

    WCHAR driveBuffer[512]{};
    DWORD driveLength = GetLogicalDriveStringsW(ARRAYSIZE(driveBuffer),
                                                 driveBuffer);
    if (driveLength && driveLength < ARRAYSIZE(driveBuffer)) {
        muxc::TextBlock drivesTitle;
        drivesTitle.Text(L"Devices and drives");
        drivesTitle.FontSize(14);
        drivesTitle.FontWeight(
            winrt::Microsoft::UI::Text::FontWeights::SemiBold());
        drivesTitle.Margin(Thickness{2, 0, 0, 2});
        content.Children().Append(drivesTitle);
        muxc::Grid drivesGrid;
        drivesGrid.ColumnSpacing(12);
        drivesGrid.RowSpacing(4);
        muxc::ColumnDefinition leftDriveColumn;
        leftDriveColumn.Width(GridLength{1, GridUnitType::Star});
        muxc::ColumnDefinition rightDriveColumn;
        rightDriveColumn.Width(GridLength{1, GridUnitType::Star});
        drivesGrid.ColumnDefinitions().Append(leftDriveColumn);
        drivesGrid.ColumnDefinitions().Append(rightDriveColumn);
        uint32_t driveIndex = 0;
        for (PCWSTR drive = driveBuffer; *drive; drive += wcslen(drive) + 1) {
            UINT type = GetDriveTypeW(drive);
            if (type == DRIVE_NO_ROOT_DIR || type == DRIVE_UNKNOWN) continue;
            WCHAR volumeName[MAX_PATH]{};
            GetVolumeInformationW(drive, volumeName, ARRAYSIZE(volumeName),
                                  nullptr, nullptr, nullptr, nullptr, 0);
            std::wstring name = volumeName[0] ? volumeName : L"Local Disk";
            if (wcslen(drive) >= 2) {
                name += L" (";
                name.append(drive, 2);
                name += L")";
            }
            ULARGE_INTEGER available{}, total{}, free{};
            GetDiskFreeSpaceExW(drive, &available, &total, &free);
            if ((driveIndex & 1) == 0) {
                muxc::RowDefinition row;
                row.Height(GridLength{1, GridUnitType::Auto});
                drivesGrid.RowDefinitions().Append(row);
            }
            auto driveButton = BetterPanelMakeDriveButton(
                name, drive, free.QuadPart, total.QuadPart,
                std::weak_ptr<BetterPanelState>(state));
            muxc::Grid::SetRow(driveButton, driveIndex / 2);
            muxc::Grid::SetColumn(driveButton, driveIndex % 2);
            drivesGrid.Children().Append(driveButton);
            ++driveIndex;
        }
        content.Children().Append(drivesGrid);
    }

    muxc::StackPanel listSwitch;
    listSwitch.Orientation(muxc::Orientation::Horizontal);
    listSwitch.Spacing(6);
    listSwitch.Margin(Thickness{2, 10, 0, 3});
    auto makeSwitchButton =
        [state, showFavorites](PCWSTR label, PCWSTR glyph,
                               bool favoritesView) {
            muxp::ToggleButton button;
            muxc::StackPanel buttonContent;
            buttonContent.Orientation(muxc::Orientation::Horizontal);
            buttonContent.Spacing(6);
            muxc::FontIcon icon;
            icon.Glyph(glyph);
            icon.FontSize(13);
            buttonContent.Children().Append(icon);
            muxc::TextBlock text;
            text.Text(label);
            text.FontSize(12);
            buttonContent.Children().Append(text);
            button.Content(buttonContent);
            button.MinWidth(92);
            button.Height(32);
            button.Padding(Thickness{10, 4, 10, 4});
            bool selected = showFavorites == favoritesView;
            button.IsChecked(selected);
            button.IsHitTestVisible(!selected);
            std::weak_ptr<BetterPanelState> weakState = state;
            button.Click([weakState, favoritesView](auto const&,
                                                    RoutedEventArgs const&) {
                Wh_SetIntValue(L"betterPanelHomeList",
                               favoritesView ? 1 : 0);
                if (auto state = weakState.lock();
                    state && !state->unloaded) {
                    state->homeContentLoaded = false;
                    auto dispatcher = state->dispatcher;
                    dispatcher.TryEnqueue([weakState]() {
                        if (auto state = weakState.lock();
                            state && !state->unloaded) {
                            BetterPanelPopulateHome(state);
                        }
                    });
                }
            });
            return button;
        };
    listSwitch.Children().Append(
        makeSwitchButton(L"Recent", L"\uE823", false));
    listSwitch.Children().Append(
        makeSwitchButton(L"Favorites", L"\uE734", true));
    content.Children().Append(listSwitch);

    auto locations = showFavorites ? BetterPanelFavoriteFiles()
                                   : BetterPanelRecentFolders();
    if (!locations.empty()) {
        for (auto const& location : locations) {
            content.Children().Append(
                BetterPanelMakeHomeLocationButton(
                    location, std::weak_ptr<BetterPanelState>(state)));
        }
    } else {
        muxc::TextBlock emptyText;
        emptyText.Text(showFavorites ? L"No favorite files yet"
                                     : L"No recent folders yet");
        emptyText.FontSize(12);
        emptyText.Opacity(0.65);
        emptyText.Margin(Thickness{2, 6, 0, 0});
        content.Children().Append(emptyText);
    }
    state->homeContentLoaded = true;
}

std::vector<std::wstring> BetterPanelSiblingAudioFiles(
    std::wstring const& path) {
    std::vector<std::wstring> files;
    std::wstring folder = BetterPanelArchiveParent(path);
    if (folder.empty()) return files;
    std::wstring pattern = folder + L"\\*";
    WIN32_FIND_DATAW data{};
    HANDLE find = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &data,
                                   FindExSearchNameMatch, nullptr,
                                   FIND_FIRST_EX_LARGE_FETCH);
    if (find == INVALID_HANDLE_VALUE) return files;
    do {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            std::wstring candidate = folder + L"\\" + data.cFileName;
            if (BetterPanelIsAudioFile(candidate)) files.push_back(candidate);
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    std::sort(files.begin(), files.end(), [](auto const& left, auto const& right) {
        return _wcsicmp(left.c_str(), right.c_str()) < 0;
    });
    return files;
}

bool BetterPanelSelectPathInActiveView(std::wstring const& path) {
    HWND tab = BetterPanelGetFocusedTabWindow();
    auto browser = BetterPanelGetShellBrowser(tab);
    if (!browser) return false;
    winrt::com_ptr<IShellView> shellView;
    if (FAILED(browser->QueryActiveShellView(shellView.put())) || !shellView)
        return false;
    winrt::com_ptr<IFolderView2> folderView;
    if (FAILED(shellView->QueryInterface(IID_PPV_ARGS(folderView.put()))) ||
        !folderView) return false;
    winrt::com_ptr<IShellFolder> folder;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(folder.put()))) || !folder)
        return false;
    std::wstring name = BetterPanelFileName(path);
    ULONG eaten = 0;
    DWORD attributes = 0;
    PIDLIST_RELATIVE child = nullptr;
    if (FAILED(folder->ParseDisplayName(nullptr, nullptr, name.data(), &eaten,
                                        &child, &attributes)) || !child)
        return false;
    HRESULT result = shellView->SelectItem(
        child, SVSI_DESELECTOTHERS | SVSI_SELECT | SVSI_FOCUSED |
                   SVSI_ENSUREVISIBLE);
    CoTaskMemFree(child);
    return SUCCEEDED(result);
}

void BetterPanelPlayAdjacent(
    std::shared_ptr<BetterPanelState> const& state, int direction) {
    if (!state || state->selectedPath.empty()) return;
    auto files = BetterPanelSiblingAudioFiles(state->selectedPath);
    if (files.empty()) {
        BetterPanelSetStatus(state->status, L"No other audio files found");
        return;
    }
    auto current = std::find_if(files.begin(), files.end(), [&](auto const& item) {
        return _wcsicmp(item.c_str(), state->selectedPath.c_str()) == 0;
    });
    size_t index = current == files.end()
                       ? 0
                       : static_cast<size_t>(current - files.begin());
    if (g_betterShuffleEnabled && files.size() > 1) {
        std::random_device random;
        std::mt19937 generator(random());
        std::uniform_int_distribution<size_t> pick(0, files.size() - 2);
        size_t selected = pick(generator);
        if (selected >= index) ++selected;
        index = selected;
    } else if (direction < 0) {
        index = index == 0 ? files.size() - 1 : index - 1;
    } else {
        index = (index + 1) % files.size();
    }
    std::wstring next = files[index];
    BetterPanelLoadAndPlay(next, state->status);
    BetterPanelSelectPathInActiveView(next);
}

void BetterPanelAddInsightRow(
    muxc::StackPanel const& panel,
    std::wstring const& label,
    std::wstring const& value,
    winrt::weak_ref<muxc::TextBlock> weakStatus) {
    if (value.empty()) return;
    muxc::Grid row;
    row.ColumnSpacing(8);
    muxc::ColumnDefinition labelColumn;
    labelColumn.Width(GridLength{112, GridUnitType::Pixel});
    muxc::ColumnDefinition valueColumn;
    valueColumn.Width(GridLength{1, GridUnitType::Star});
    row.ColumnDefinitions().Append(labelColumn);
    row.ColumnDefinitions().Append(valueColumn);

    muxc::TextBlock labelText;
    labelText.Text(label);
    labelText.FontSize(11);
    labelText.Opacity(0.72);
    labelText.VerticalAlignment(VerticalAlignment::Center);
    row.Children().Append(labelText);

    auto valueButton = BetterPanelMakeButton(L"");
    muxc::TextBlock valueText;
    valueText.Text(value);
    valueText.TextWrapping(TextWrapping::Wrap);
    valueText.TextAlignment(TextAlignment::Right);
    valueButton.Content(valueText);
    valueButton.FontSize(11);
    valueButton.Padding(Thickness{6, 3, 6, 3});
    valueButton.HorizontalContentAlignment(HorizontalAlignment::Right);
    valueButton.HorizontalAlignment(HorizontalAlignment::Stretch);
    muxc::Grid::SetColumn(valueButton, 1);
    valueButton.Click(
        [value, weakStatus](winrt::Windows::Foundation::IInspectable const&,
                            RoutedEventArgs const&) {
            BetterPanelSetStatus(weakStatus,
                                 BetterPanelCopyText(value) ? L"Detail copied"
                                                            : L"Copy failed");
        });
    row.Children().Append(valueButton);
    panel.Children().Append(row);
}

std::wstring BetterPanelTrimMetadataValue(std::wstring value) {
    auto first = value.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return L"";
    auto last = value.find_last_not_of(L" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::vector<winrt::hstring> BetterPanelSplitMetadataValues(
    std::wstring const& text) {
    std::vector<winrt::hstring> result;
    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find_first_of(L",;", start);
        auto value = BetterPanelTrimMetadataValue(
            text.substr(start, end == std::wstring::npos
                                   ? std::wstring::npos
                                   : end - start));
        if (!value.empty()) result.emplace_back(value);
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    return result;
}

std::wstring BetterPanelJoinMetadataValues(
    winrt::Windows::Foundation::Collections::IIterable<winrt::hstring> const&
        values) {
    std::wstring result;
    for (auto const& value : values) {
        if (value.empty()) continue;
        if (!result.empty()) result += L", ";
        result += value.c_str();
    }
    return result;
}

std::wstring BetterPanelPropertyString(
    winrt::Windows::Foundation::Collections::IMap<
        winrt::hstring, winrt::Windows::Foundation::IInspectable> const& values,
    wchar_t const* name) {
    try {
        if (!values.HasKey(name)) return L"";
        auto property = values.Lookup(name)
                            .try_as<winrt::Windows::Foundation::IPropertyValue>();
        if (!property) return L"";
        if (property.Type() ==
            winrt::Windows::Foundation::PropertyType::String) {
            return property.GetString().c_str();
        }
    } catch (...) {
    }
    return L"";
}

std::wstring BetterPanelPropertyStringArray(
    winrt::Windows::Foundation::Collections::IMap<
        winrt::hstring, winrt::Windows::Foundation::IInspectable> const& values,
    wchar_t const* name) {
    try {
        if (!values.HasKey(name)) return L"";
        auto property = values.Lookup(name)
                            .try_as<winrt::Windows::Foundation::IPropertyValue>();
        if (!property) return L"";
        if (property.Type() ==
            winrt::Windows::Foundation::PropertyType::StringArray) {
            winrt::com_array<winrt::hstring> items;
            property.GetStringArray(items);
            std::wstring result;
            for (auto const& item : items) {
                if (item.empty()) continue;
                if (!result.empty()) result += L", ";
                result += item.c_str();
            }
            return result;
        }
    } catch (...) {
    }
    return L"";
}

std::wstring BetterPanelMetadataRatingText(uint32_t rating) {
    if (rating == 0) return L"";
    if (rating < 25) return L"1";
    if (rating < 50) return L"2";
    if (rating < 75) return L"3";
    if (rating < 99) return L"4";
    return L"5";
}

uint32_t BetterPanelMetadataRatingValue(uint32_t rating) {
    static constexpr uint32_t values[] = {0, 1, 25, 50, 75, 99};
    return rating <= 5 ? values[rating] : 0;
}

std::vector<muxc::TextBox> BetterPanelMetadataEditors(
    std::shared_ptr<BetterPanelState> const& state) {
    std::vector<muxc::TextBox> editors;
    for (auto const& weakEditor :
         {state->metadataTitle, state->metadataArtist, state->metadataAlbum,
          state->metadataGenre, state->metadataYear, state->metadataRating,
          state->metadataTags, state->metadataComments}) {
        if (auto editor = weakEditor.get()) editors.push_back(editor);
    }
    return editors;
}

BetterPanelAudioMetadata BetterPanelCurrentMetadata(
    std::shared_ptr<BetterPanelState> const& state) {
    BetterPanelAudioMetadata data;
    if (auto field = state->metadataTitle.get()) data.title = field.Text();
    if (auto field = state->metadataArtist.get()) data.artist = field.Text();
    if (auto field = state->metadataAlbum.get()) data.album = field.Text();
    if (auto field = state->metadataGenre.get()) data.genre = field.Text();
    if (auto field = state->metadataYear.get()) data.year = field.Text();
    if (auto field = state->metadataRating.get()) data.rating = field.Text();
    if (auto field = state->metadataTags.get()) data.tags = field.Text();
    if (auto field = state->metadataComments.get()) data.comments = field.Text();
    return data;
}

std::wstring BetterPanelFormatMetadata(BetterPanelAudioMetadata const& data) {
    return L"Title: " + data.title + L"\r\nArtist: " + data.artist +
           L"\r\nAlbum: " + data.album + L"\r\nGenre: " + data.genre +
           L"\r\nYear: " + data.year + L"\r\nRating: " + data.rating +
           L"\r\nTags: " + data.tags + L"\r\nComments: " + data.comments;
}

void BetterPanelSetMetadataFields(
    std::shared_ptr<BetterPanelState> const& state,
    BetterPanelAudioMetadata const& data) {
    state->suppressMetadataChanged = true;
    if (auto field = state->metadataTitle.get()) field.Text(data.title);
    if (auto field = state->metadataArtist.get()) field.Text(data.artist);
    if (auto field = state->metadataAlbum.get()) field.Text(data.album);
    if (auto field = state->metadataGenre.get()) field.Text(data.genre);
    if (auto field = state->metadataYear.get()) field.Text(data.year);
    if (auto field = state->metadataRating.get()) field.Text(data.rating);
    if (auto field = state->metadataTags.get()) field.Text(data.tags);
    if (auto field = state->metadataComments.get()) field.Text(data.comments);
    state->suppressMetadataChanged = false;
}

void BetterPanelUpdateMetadataControls(
    std::shared_ptr<BetterPanelState> const& state) {
    for (auto const& editor : BetterPanelMetadataEditors(state)) {
        editor.IsReadOnly(!state->metadataEditing);
    }
    if (auto button = state->metadataEditButton.get()) {
        button.Visibility(!state->metadataEditing && !state->metadataLoading
                              ? Visibility::Visible
                              : Visibility::Collapsed);
    }
    if (auto button = state->metadataSaveButton.get()) {
        button.Visibility(state->metadataEditing ? Visibility::Visible
                                                 : Visibility::Collapsed);
        button.IsEnabled(state->metadataDirty && !state->metadataLoading);
    }
    if (auto button = state->metadataCancelButton.get()) {
        button.Visibility(state->metadataEditing ? Visibility::Visible
                                                 : Visibility::Collapsed);
    }
    if (auto button = state->metadataReloadButton.get()) {
        button.Visibility(!state->metadataEditing && !state->metadataLoading
                              ? Visibility::Visible
                              : Visibility::Collapsed);
    }
}

winrt::fire_and_forget BetterPanelLoadAudioMetadata(
    std::weak_ptr<BetterPanelState> weakState, std::wstring path) {
    auto state = weakState.lock();
    if (!state || path.empty() || state->metadataEditing) co_return;
    state->metadataLoading = true;
    BetterPanelUpdateMetadataControls(state);
    if (auto info = state->metadataInfo.get()) info.Text(L"Reading metadata…");
    try {
        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        auto music = co_await file.Properties().GetMusicPropertiesAsync();
        auto keys = winrt::single_threaded_vector<winrt::hstring>();
        keys.Append(L"System.Keywords");
        keys.Append(L"System.Comment");
        auto extra = co_await file.Properties().RetrievePropertiesAsync(keys);

        BetterPanelAudioMetadata data;
        data.title = music.Title().c_str();
        data.artist = music.Artist().c_str();
        data.album = music.Album().c_str();
        data.genre = BetterPanelJoinMetadataValues(music.Genre());
        if (music.Year()) data.year = std::to_wstring(music.Year());
        data.rating = BetterPanelMetadataRatingText(music.Rating());
        data.tags = BetterPanelPropertyStringArray(extra, L"System.Keywords");
        data.comments = BetterPanelPropertyString(extra, L"System.Comment");

        state = weakState.lock();
        if (!state || state->unloaded || state->selectedPath != path) co_return;
        state->metadataLoading = false;
        state->metadataLoadedPath = path;
        state->metadataOriginal = data;
        state->metadataCopyText = BetterPanelFormatMetadata(data);
        state->metadataDirty = false;
        BetterPanelSetMetadataFields(state, data);
        BetterPanelUpdateMetadataControls(state);
        if (auto info = state->metadataInfo.get()) {
            info.Text(L"Edit support depends on the audio format and its Windows property handler.");
        }
    } catch (winrt::hresult_error const& ex) {
        state = weakState.lock();
        if (!state || state->selectedPath != path) co_return;
        state->metadataLoading = false;
        BetterPanelUpdateMetadataControls(state);
        if (auto info = state->metadataInfo.get()) {
            info.Text(L"Metadata is unavailable for this audio format.");
        }
        Wh_Log(L"Metadata load error %08X: %s", ex.code(), ex.message().c_str());
    }
}

void BetterPanelBeginMetadataEdit(
    std::shared_ptr<BetterPanelState> const& state) {
    if (!state || state->metadataLoading || state->metadataLoadedPath !=
                                               state->selectedPath) {
        return;
    }
    state->metadataEditing = true;
    state->metadataDirty = false;
    BetterPanelUpdateMetadataControls(state);
    if (auto field = state->metadataTitle.get()) field.Focus(FocusState::Programmatic);
    if (auto info = state->metadataInfo.get()) {
        info.Text(L"Rating accepts 0–5. Separate genres and tags with commas.");
    }
}

void BetterPanelCancelMetadataEdit(
    std::shared_ptr<BetterPanelState> const& state) {
    if (!state || !state->metadataEditing) return;
    state->metadataEditing = false;
    state->metadataDirty = false;
    state->metadataFocusedEditor = {};
    BetterPanelSetMetadataFields(state, state->metadataOriginal);
    BetterPanelUpdateMetadataControls(state);
    if (auto info = state->metadataInfo.get()) {
        info.Text(L"Changes discarded.");
    }
}

bool BetterPanelParseMetadataNumber(std::wstring text, uint32_t maximum,
                                    uint32_t* value) {
    text = BetterPanelTrimMetadataValue(std::move(text));
    if (text.empty()) {
        *value = 0;
        return true;
    }
    wchar_t* end = nullptr;
    unsigned long parsed = wcstoul(text.c_str(), &end, 10);
    if (!end || *end != L'\0' || parsed > maximum) return false;
    *value = static_cast<uint32_t>(parsed);
    return true;
}

winrt::fire_and_forget BetterPanelSaveAudioMetadata(
    std::weak_ptr<BetterPanelState> weakState) {
    auto state = weakState.lock();
    if (!state || !state->metadataEditing || !state->metadataDirty) co_return;
    std::wstring path = state->selectedPath;
    auto data = BetterPanelCurrentMetadata(state);
    uint32_t year = 0;
    uint32_t rating = 0;
    if (!BetterPanelParseMetadataNumber(data.year, 9999, &year)) {
        BetterPanelSetStatus(state->status, L"Year must be a number from 0 to 9999");
        co_return;
    }
    if (!BetterPanelParseMetadataNumber(data.rating, 5, &rating)) {
        BetterPanelSetStatus(state->status, L"Rating must be a number from 0 to 5");
        co_return;
    }

    state->metadataLoading = true;
    BetterPanelUpdateMetadataControls(state);
    if (auto info = state->metadataInfo.get()) info.Text(L"Saving metadata…");
    BetterPanelStopStateMedia(state, path);
    try {
        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        auto properties = winrt::single_threaded_map<
            winrt::hstring, winrt::Windows::Foundation::IInspectable>();
        properties.Insert(L"System.Title", winrt::box_value(data.title));
        properties.Insert(L"System.Music.Artist", winrt::box_value(data.artist));
        properties.Insert(L"System.Music.AlbumTitle", winrt::box_value(data.album));
        auto genres = BetterPanelSplitMetadataValues(data.genre);
        properties.Insert(
            L"System.Music.Genre",
            winrt::Windows::Foundation::PropertyValue::CreateStringArray(genres));
        properties.Insert(L"System.Media.Year", winrt::box_value(year));
        properties.Insert(L"System.Rating",
                          winrt::box_value(BetterPanelMetadataRatingValue(rating)));
        auto tags = BetterPanelSplitMetadataValues(data.tags);
        properties.Insert(
            L"System.Keywords",
            winrt::Windows::Foundation::PropertyValue::CreateStringArray(tags));
        properties.Insert(L"System.Comment", winrt::box_value(data.comments));
        co_await file.Properties().SavePropertiesAsync(properties);

        state = weakState.lock();
        if (!state || state->unloaded || state->selectedPath != path) co_return;
        state->metadataLoading = false;
        state->metadataEditing = false;
        state->metadataDirty = false;
        state->metadataFocusedEditor = {};
        BetterPanelUpdateMetadataControls(state);
        if (auto info = state->metadataInfo.get()) info.Text(L"Metadata saved.");
        BetterPanelSetStatus(state->status, L"Audio metadata saved");
        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW, path.c_str(), nullptr);
        BetterPanelLoadAudioMetadata(weakState, path);
    } catch (winrt::hresult_error const& ex) {
        state = weakState.lock();
        if (!state || state->selectedPath != path) co_return;
        state->metadataLoading = false;
        BetterPanelUpdateMetadataControls(state);
        if (auto info = state->metadataInfo.get()) {
            info.Text(L"Windows could not write metadata to this file format.");
        }
        BetterPanelSetStatus(state->status, L"Metadata save failed");
        Wh_Log(L"Metadata save error %08X: %s", ex.code(), ex.message().c_str());
    }
}

muxc::TextBox BetterPanelAddMetadataField(
    muxc::StackPanel const& panel, wchar_t const* label,
    wchar_t const* placeholder, bool multiline,
    std::weak_ptr<BetterPanelState> weakState) {
    muxc::Grid row;
    row.ColumnSpacing(8);
    muxc::ColumnDefinition labelColumn;
    labelColumn.Width(GridLength{112, GridUnitType::Pixel});
    muxc::ColumnDefinition editorColumn;
    editorColumn.Width(GridLength{1, GridUnitType::Star});
    row.ColumnDefinitions().Append(labelColumn);
    row.ColumnDefinitions().Append(editorColumn);

    muxc::TextBlock labelText;
    labelText.Text(label);
    labelText.FontSize(11);
    labelText.Opacity(0.72);
    labelText.VerticalAlignment(VerticalAlignment::Center);
    row.Children().Append(labelText);

    muxc::TextBox editor;
    editor.PlaceholderText(placeholder);
    editor.FontSize(12);
    editor.Padding(Thickness{8, 4, 8, 4});
    editor.HorizontalAlignment(HorizontalAlignment::Stretch);
    editor.IsReadOnly(true);
    editor.IsSpellCheckEnabled(false);
    editor.IsTextPredictionEnabled(false);
    if (multiline) {
        editor.AcceptsReturn(true);
        editor.TextWrapping(TextWrapping::Wrap);
        editor.MinHeight(70);
    }
    muxc::Grid::SetColumn(editor, 1);
    editor.TextChanged(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    muxc::TextChangedEventArgs const&) {
            auto state = weakState.lock();
            if (!state || !state->metadataEditing ||
                state->suppressMetadataChanged) {
                return;
            }
            state->metadataDirty = true;
            BetterPanelUpdateMetadataControls(state);
        });
    editor.GotFocus(
        [weakState](winrt::Windows::Foundation::IInspectable const& sender,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            auto field = sender.try_as<muxc::TextBox>();
            if (state && state->metadataEditing && field) {
                state->metadataFocusedEditor = winrt::make_weak(field);
            }
        });
    editor.LostFocus(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            if (auto state = weakState.lock()) {
                state->metadataFocusedEditor = {};
            }
        });
    row.Children().Append(editor);
    panel.Children().Append(row);
    return editor;
}

winrt::fire_and_forget BetterPanelLoadExif(
    std::weak_ptr<BetterPanelState> weakState,
    std::wstring path) {
    try {
        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        auto properties = co_await file.Properties().GetImagePropertiesAsync();
        auto state = weakState.lock();
        if (!state || state->selectedPath != path ||
            state->insightsLoadedPath != path) {
            co_return;
        }
        auto content = state->insightsContent.get();
        if (!content) co_return;
        muxc::TextBlock heading;
        heading.Text(L"Image EXIF");
        heading.FontWeight(
            winrt::Microsoft::UI::Text::FontWeights::SemiBold());
        heading.Margin(Thickness{0, 6, 0, 0});
        content.Children().Append(heading);
        auto weakStatus = state->detailsCopyHint;
        BetterPanelAddInsightRow(content, L"Dimensions",
            std::to_wstring(properties.Width()) + L" × " +
                std::to_wstring(properties.Height()), weakStatus);
        BetterPanelAddInsightRow(content, L"Camera maker",
                                 properties.CameraManufacturer().c_str(),
                                 weakStatus);
        BetterPanelAddInsightRow(content, L"Camera model",
                                 properties.CameraModel().c_str(), weakStatus);
        if (properties.DateTaken().time_since_epoch().count() != 0) {
            ULARGE_INTEGER value{};
            value.QuadPart = static_cast<ULONGLONG>(
                properties.DateTaken().time_since_epoch().count());
            FILETIME fileTime{value.LowPart, value.HighPart};
            BetterPanelAddInsightRow(content, L"Date taken",
                                     BetterPanelFormatFileTime(fileTime),
                                     weakStatus);
        }
        if (properties.Rating()) {
            BetterPanelAddInsightRow(content, L"Rating",
                                     std::to_wstring(properties.Rating()) +
                                         L" / 100",
                                     weakStatus);
        }
        if (auto latitude = properties.Latitude()) {
            BetterPanelAddInsightRow(content, L"Latitude",
                                     std::to_wstring(latitude.Value()),
                                     weakStatus);
        }
        if (auto longitude = properties.Longitude()) {
            BetterPanelAddInsightRow(content, L"Longitude",
                                     std::to_wstring(longitude.Value()),
                                     weakStatus);
        }
        BetterPanelPruneButtonLayoutSnapshots(state);
        BetterPanelApplyNativeButtonStyle(state, content);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"EXIF load error %08X: %s", ex.code(), ex.message().c_str());
    }
}

void BetterPanelLoadInsights(
    std::shared_ptr<BetterPanelState> const& state,
    std::wstring const& path) {
    auto content = state->insightsContent.get();
    if (!content || path.empty()) return;
    state->insightsLoading = true;
    state->insightsLoadedPath.clear();
    state->hashValue.clear();
    state->detailsCopyText.clear();
    uint64_t generation =
        state->insightsGeneration.fetch_add(1, std::memory_order_relaxed) + 1;
    content.Children().Clear();
    muxc::TextBlock loading;
    loading.Text(L"Reading file details…");
    loading.FontSize(11);
    loading.Opacity(0.70);
    content.Children().Append(loading);

    auto weakState = std::weak_ptr<BetterPanelState>(state);
    auto dispatcher = state->dispatcher;
    std::thread([weakState, dispatcher, path, generation]() {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
        auto data = BetterPanelReadInsightData(path);
        BetterPanelMultiSummary folderSummary;
        bool driveRoot = data.success && data.directory &&
                         PathIsRootW(path.c_str());
        uint64_t driveFree = 0;
        uint64_t driveTotal = 0;
        UINT driveType = DRIVE_UNKNOWN;
        std::wstring driveLabel;
        std::wstring driveFileSystem;
        DWORD driveSerial = 0;
        if (data.success && data.directory) {
            if (driveRoot) {
                driveType = GetDriveTypeW(path.c_str());
                ULARGE_INTEGER available{}, total{}, free{};
                if (GetDiskFreeSpaceExW(path.c_str(), &available, &total,
                                        &free)) {
                    driveFree = free.QuadPart;
                    driveTotal = total.QuadPart;
                    folderSummary.totalSize = driveTotal - driveFree;
                }
                WCHAR label[MAX_PATH]{};
                WCHAR fileSystem[MAX_PATH]{};
                GetVolumeInformationW(path.c_str(), label, ARRAYSIZE(label),
                                      &driveSerial, nullptr, nullptr,
                                      fileSystem, ARRAYSIZE(fileSystem));
                driveLabel = label;
                driveFileSystem = fileSystem;
            } else if (!BetterPanelGetCachedFolderSummary(path,
                                                           folderSummary)) {
                folderSummary = BetterPanelBuildFolderSummary(
                    path, weakState, generation, 500000,
                    GetTickCount64() + 15000);
                BetterPanelCacheFolderSummary(path, folderSummary);
            }
        }
        std::wstring hash;
        if (data.success && !data.directory) {
            hash = BetterPanelSha256(path, weakState, generation);
        }
        dispatcher.TryEnqueue([weakState, path, data = std::move(data),
                               folderSummary = std::move(folderSummary),
                               hash = std::move(hash), generation, driveRoot,
                               driveFree, driveTotal, driveType,
                               driveLabel = std::move(driveLabel),
                               driveFileSystem = std::move(driveFileSystem),
                               driveSerial]() {
            auto state = weakState.lock();
            if (!state || state->unloaded || state->selectedPath != path ||
                state->insightsGeneration.load(std::memory_order_relaxed) !=
                    generation) {
                return;
            }
            auto content = state->insightsContent.get();
            if (!content) return;
            content.Children().Clear();
            state->insightsLoading = false;
            state->insightsLoadedPath = path;
            state->hashValue = hash;
            if (!data.success) {
                muxc::TextBlock error;
                error.Text(L"File details unavailable");
                content.Children().Append(error);
                return;
            }
            auto weakStatus = state->detailsCopyHint;
            std::wstring sizeText = data.directory
                                        ? BetterPanelFormatByteSize(
                                              folderSummary.totalSize)
                                        : BetterPanelFormatByteSize(data.size);
            if (data.directory && folderSummary.incomplete) {
                sizeText += L" (partial)";
            }
            std::wstring modifiedText =
                BetterPanelFormatFileTime(data.modified);
            state->detailsCopyText =
                L"Name: " + data.name + L"\r\nFull path: " + data.path +
                L"\r\nType: " +
                (driveRoot ? L"Drive" : (data.directory ? L"Folder" : L"File")) +
                L"\r\n" + (driveRoot ? L"Used space: " : L"File Size: ") +
                sizeText +
                L"\r\nModified: " + modifiedText;
            if (driveRoot) {
                std::wstring driveTypeText = L"Drive";
                switch (driveType) {
                    case DRIVE_FIXED: driveTypeText = L"Local drive"; break;
                    case DRIVE_REMOVABLE: driveTypeText = L"Removable drive"; break;
                    case DRIVE_REMOTE: driveTypeText = L"Network drive"; break;
                    case DRIVE_CDROM: driveTypeText = L"Optical drive"; break;
                    case DRIVE_RAMDISK: driveTypeText = L"RAM drive"; break;
                }
                WCHAR serialText[16]{};
                swprintf_s(serialText, L"%04X-%04X",
                           HIWORD(driveSerial), LOWORD(driveSerial));
                state->detailsCopyText +=
                    L"\r\nFree space: " + BetterPanelFormatByteSize(driveFree) +
                    L"\r\nCapacity: " + BetterPanelFormatByteSize(driveTotal) +
                    (driveLabel.empty() ? L"" : L"\r\nVolume label: " + driveLabel) +
                    (driveFileSystem.empty()
                         ? L""
                         : L"\r\nFile system: " + driveFileSystem) +
                    L"\r\nDrive type: " + driveTypeText +
                    (driveSerial ? L"\r\nSerial number: " +
                                       std::wstring(serialText)
                                 : L"");
            } else if (data.directory) {
                std::wstring types = BetterPanelFormatTypes(folderSummary.types);
                state->detailsCopyText +=
                    L"\r\nFiles: " + std::to_wstring(folderSummary.files) +
                    L"\r\nSubfolders: " +
                    std::to_wstring(folderSummary.folders) +
                    L"\r\nFile types: " + types;
            }
            if (!hash.empty()) {
                state->detailsCopyText += L"\r\nSHA-256: " + hash;
            }
            BetterPanelAddInsightRow(content, L"Name", data.name, weakStatus);
            BetterPanelAddInsightRow(content, L"Full path", data.path,
                                     weakStatus);
            BetterPanelAddInsightRow(content, L"Type",
                                     driveRoot ? L"Drive"
                                               : (data.directory ? L"Folder"
                                                                 : L"File"),
                                     weakStatus);
            BetterPanelAddInsightRow(
                content, driveRoot ? L"Used space" : L"Size",
                sizeText,
                weakStatus);
            if (driveRoot) {
                BetterPanelAddInsightRow(
                    content, L"Free space",
                    BetterPanelFormatByteSize(driveFree), weakStatus);
                BetterPanelAddInsightRow(
                    content, L"Capacity",
                    BetterPanelFormatByteSize(driveTotal), weakStatus);
                if (!driveLabel.empty()) {
                    BetterPanelAddInsightRow(content, L"Volume label",
                                             driveLabel, weakStatus);
                }
                if (!driveFileSystem.empty()) {
                    BetterPanelAddInsightRow(content, L"File system",
                                             driveFileSystem, weakStatus);
                }
                std::wstring driveTypeText = L"Drive";
                switch (driveType) {
                    case DRIVE_FIXED: driveTypeText = L"Local drive"; break;
                    case DRIVE_REMOVABLE: driveTypeText = L"Removable drive"; break;
                    case DRIVE_REMOTE: driveTypeText = L"Network drive"; break;
                    case DRIVE_CDROM: driveTypeText = L"Optical drive"; break;
                    case DRIVE_RAMDISK: driveTypeText = L"RAM drive"; break;
                }
                BetterPanelAddInsightRow(content, L"Drive type",
                                         driveTypeText, weakStatus);
                if (driveSerial) {
                    WCHAR serialText[16]{};
                    swprintf_s(serialText, L"%04X-%04X",
                               HIWORD(driveSerial), LOWORD(driveSerial));
                    BetterPanelAddInsightRow(content, L"Serial number",
                                             serialText, weakStatus);
                }
            }
            BetterPanelAddInsightRow(content, L"Modified",
                                     modifiedText,
                                     weakStatus);
            if (driveRoot) {
                auto driveContent = state->driveContent.get();
                if (!driveContent) return;
                driveContent.Children().Clear();

                WCHAR drives[512]{};
                DWORD driveChars = GetLogicalDriveStringsW(
                    ARRAYSIZE(drives), drives);
                std::vector<std::wstring> otherDrives;
                if (driveChars && driveChars < ARRAYSIZE(drives)) {
                    for (PCWSTR drive = drives; *drive;
                         drive += wcslen(drive) + 1) {
                        if (_wcsicmp(drive, path.c_str()) != 0) {
                            otherDrives.emplace_back(drive);
                        }
                    }
                }
                if (!otherDrives.empty()) {
                    muxc::Grid driveGrid;
                    driveGrid.ColumnSpacing(6);
                    driveGrid.RowSpacing(6);
                    for (int column = 0; column < 2; ++column) {
                        muxc::ColumnDefinition definition;
                        definition.Width(GridLength{1, GridUnitType::Star});
                        driveGrid.ColumnDefinitions().Append(definition);
                    }
                    uint32_t index = 0;
                    for (auto const& otherPath : otherDrives) {
                        if ((index & 1) == 0) {
                            muxc::RowDefinition row;
                            row.Height(GridLength{1, GridUnitType::Auto});
                            driveGrid.RowDefinitions().Append(row);
                        }
                        WCHAR label[MAX_PATH]{};
                        GetVolumeInformationW(otherPath.c_str(), label,
                                              ARRAYSIZE(label), nullptr,
                                              nullptr, nullptr, nullptr, 0);
                        std::wstring name = label[0] ? label : L"Local Disk";
                        name += L" (" + otherPath.substr(0, 2) + L")";
                        auto button = BetterPanelMakeIconButton(
                            name.c_str(), L"\uEDA2");
                        button.HorizontalAlignment(HorizontalAlignment::Stretch);
                        button.HorizontalContentAlignment(HorizontalAlignment::Left);
                        button.Click([weakState, target = otherPath](
                                         auto const&, RoutedEventArgs const&) {
                            BetterPanelNavigateFromHome(weakState, target);
                        });
                        muxc::Grid::SetRow(button, index / 2);
                        muxc::Grid::SetColumn(button, index % 2);
                        driveGrid.Children().Append(button);
                        ++index;
                    }
                    driveContent.Children().Append(driveGrid);
                }

                muxc::TextBlock actionsTitle;
                actionsTitle.Text(L"Drive tools");
                actionsTitle.FontWeight(
                    winrt::Microsoft::UI::Text::FontWeights::SemiBold());
                actionsTitle.Margin(Thickness{0, 4, 0, 0});
                driveContent.Children().Append(actionsTitle);

                muxc::StackPanel actions;
                actions.Orientation(muxc::Orientation::Horizontal);
                actions.Spacing(6);
                if (driveType == DRIVE_FIXED) {
                    WCHAR systemDirectory[MAX_PATH]{};
                    GetSystemDirectoryW(systemDirectory,
                                        ARRAYSIZE(systemDirectory));
                    std::wstring cleanupPath =
                        std::wstring(systemDirectory) + L"\\cleanmgr.exe";
                    if (GetFileAttributesW(cleanupPath.c_str()) !=
                        INVALID_FILE_ATTRIBUTES) {
                        auto cleanup = BetterPanelMakeIconButton(
                            L"Disk Cleanup", L"\uE74D");
                        cleanup.Click([drivePath = path](auto const&,
                                                         RoutedEventArgs const&) {
                            std::wstring drive = drivePath.substr(0, 2);
                            std::wstring arguments = L"/d " + drive;
                            ShellExecuteW(GetForegroundWindow(), L"open",
                                          L"cleanmgr.exe", arguments.c_str(),
                                          nullptr, SW_SHOWNORMAL);
                        });
                        actions.Children().Append(cleanup);
                    }

                    auto optimize = BetterPanelMakeIconButton(
                        L"Optimize", L"\uE9D9");
                    optimize.Click([](auto const&, RoutedEventArgs const&) {
                        ShellExecuteW(GetForegroundWindow(), L"open",
                                      L"dfrgui.exe", nullptr, nullptr,
                                      SW_SHOWNORMAL);
                    });
                    actions.Children().Append(optimize);
                }

                auto storage = BetterPanelMakeIconButton(
                    L"Storage", L"\uEDA2");
                storage.Click([](auto const&, RoutedEventArgs const&) {
                    ShellExecuteW(GetForegroundWindow(), L"open",
                                  L"ms-settings:storagesense", nullptr,
                                  nullptr, SW_SHOWNORMAL);
                });
                actions.Children().Append(storage);
                driveContent.Children().Append(actions);
            }
            if (data.directory && !driveRoot) {
                BetterPanelAddInsightRow(content, L"Files",
                                         std::to_wstring(folderSummary.files),
                                         weakStatus);
                BetterPanelAddInsightRow(
                    content, L"Subfolders",
                    std::to_wstring(folderSummary.folders), weakStatus);
                BetterPanelAddInsightRow(
                    content, L"File types",
                    BetterPanelFormatTypes(folderSummary.types), weakStatus);
            }
            if (!hash.empty()) {
                BetterPanelAddInsightRow(content, L"SHA-256", hash, weakStatus);
            }
            if (BetterPanelIsImageFile(path)) {
                BetterPanelLoadExif(weakState, path);
            }
            BetterPanelPruneButtonLayoutSnapshots(state);
            if (auto panel = state->panel.get()) {
                BetterPanelApplyNativeButtonStyle(state, panel);
            }
        });
    }).detach();
}

void BetterPanelLoadMultiSummary(
    std::shared_ptr<BetterPanelState> const& state,
    std::vector<std::wstring> const& paths) {
    std::wstring key;
    for (auto const& path : paths) {
        key += path;
        key.push_back(L'\n');
    }
    if (key == state->multiSelectionKey) return;
    state->multiSelectionKey = key;
    uint64_t generation =
        state->insightsGeneration.fetch_add(1, std::memory_order_relaxed) + 1;
    state->multiSummaryLoading = true;
    state->detailsCopyText.clear();
    if (auto content = state->insightsContent.get()) {
        content.Children().Clear();
        BetterPanelAddInsightRow(content, L"Selected items",
                                 std::to_wstring(paths.size()),
                                 state->detailsCopyHint);
        BetterPanelAddInsightRow(content, L"Folder contents",
                                 L"Calculating...", state->detailsCopyHint);
    }
    if (auto text = state->multiSelectionText.get()) {
        text.Text(L"Analyzing " + std::to_wstring(paths.size()) +
                  L" selected items…");
    }
    auto weakState = std::weak_ptr<BetterPanelState>(state);
    auto dispatcher = state->dispatcher;
    std::thread([weakState, dispatcher, paths, key, generation]() {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
        auto summary =
            BetterPanelBuildMultiSummary(paths, weakState, generation);
        auto formatted = BetterPanelFormatMultiSummary(summary);
        dispatcher.TryEnqueue([weakState, key, summary = std::move(summary),
                               formatted = std::move(formatted)]() {
            auto state = weakState.lock();
            if (!state || state->unloaded || state->multiSelectionKey != key)
                return;
            state->multiSummaryLoading = false;
            if (auto text = state->multiSelectionText.get()) {
                text.Text(L"");
            }
            if (auto content = state->insightsContent.get()) {
                content.Children().Clear();
                auto weakStatus = state->detailsCopyHint;
                std::wstring selected =
                    std::to_wstring(summary.selectedItems);
                std::wstring files = std::to_wstring(summary.files);
                std::wstring folders = std::to_wstring(summary.folders);
                std::wstring size = BetterPanelFormatByteSize(summary.totalSize);
                if (summary.incomplete) size += L" (partial)";
                std::wstring types = BetterPanelFormatTypes(summary.types);
                std::wstring modified = summary.hasDate
                    ? BetterPanelFormatFileTime(summary.oldest) +
                          (CompareFileTime(&summary.oldest, &summary.newest) != 0
                               ? L" – " + BetterPanelFormatFileTime(summary.newest)
                               : L"")
                    : L"Unavailable";
                BetterPanelAddInsightRow(content, L"Selected items", selected,
                                         weakStatus);
                BetterPanelAddInsightRow(content, L"Files", files, weakStatus);
                BetterPanelAddInsightRow(content, L"Folders", folders, weakStatus);
                if (summary.folders) {
                    BetterPanelAddInsightRow(
                        content, L"Files inside",
                        std::to_wstring(summary.containedFiles), weakStatus);
                    BetterPanelAddInsightRow(
                        content, L"Subfolders",
                        std::to_wstring(summary.subfolders), weakStatus);
                }
                BetterPanelAddInsightRow(content, L"Combined size", size,
                                         weakStatus);
                BetterPanelAddInsightRow(content, L"File types", types,
                                         weakStatus);
                BetterPanelAddInsightRow(content, L"Modified", modified,
                                         weakStatus);
                state->detailsCopyText =
                    L"Selected items: " + selected + L"\r\nFiles: " + files +
                    L"\r\nFolders: " + folders +
                    (summary.folders
                         ? L"\r\nFiles inside: " +
                               std::to_wstring(summary.containedFiles) +
                               L"\r\nSubfolders: " +
                               std::to_wstring(summary.subfolders)
                         : L"") +
                    L"\r\nCombined size: " +
                    size + L"\r\nFile types: " + types +
                    L"\r\nModified: " + modified;
                BetterPanelPruneButtonLayoutSnapshots(state);
                BetterPanelApplyNativeButtonStyle(state, content);
            }
        });
    }).detach();
}

winrt::fire_and_forget BetterPanelShowLargePreview(
    std::wstring path,
    winrt::weak_ref<FrameworkElement> weakAnchor,
    winrt::weak_ref<muxc::TextBlock> weakStatus) {
    try {
        if (path.empty()) {
            BetterPanelSetStatus(weakStatus, L"Select one image first");
            co_return;
        }

        auto file = co_await ws::StorageFile::GetFileFromPathAsync(path);
        auto stream = co_await file.OpenAsync(ws::FileAccessMode::Read);
        winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
        bitmap.AutoPlay(true);
        co_await bitmap.SetSourceAsync(stream);

        auto anchor = weakAnchor.get();
        if (!anchor || !anchor.XamlRoot()) {
            co_return;
        }

        auto rootSize = anchor.XamlRoot().Size();
        muxc::Image image;
        image.Source(bitmap);
        image.Stretch(winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
        image.Width(std::max(320.0f, std::min(1100.0f, rootSize.Width - 100)));
        image.Height(std::max(260.0f, std::min(760.0f, rootSize.Height - 180)));

        muxc::ContentDialog dialog;
        dialog.XamlRoot(anchor.XamlRoot());
        dialog.Title(winrt::box_value(BetterPanelFileName(path)));
        dialog.Content(image);
        dialog.CloseButtonText(L"Close");
        dialog.FullSizeDesired(true);
        if (bitmap.IsAnimatedBitmap() && !bitmap.IsPlaying()) {
            bitmap.Play();
        }
        BetterPanelSetStatus(weakStatus, L"");
        co_await dialog.ShowAsync();
    } catch (winrt::hresult_error const& ex) {
        BetterPanelSetStatus(weakStatus, L"Preview unavailable");
        Wh_Log(L"Large preview error %08X: %s", ex.code(),
               ex.message().c_str());
    }
}

muxc::Image BetterPanelFindPreviewImage(DependencyObject const& root) {
    if (!root) {
        return nullptr;
    }
    if (auto image = root.try_as<muxc::Image>()) {
        return image;
    }
    int count = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
        GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
            GetChild(root, i);
        if (auto image = BetterPanelFindPreviewImage(child)) {
            return image;
        }
    }
    return nullptr;
}

void BetterPanelSetInlinePreviewExpanded(
    std::shared_ptr<BetterPanelState> const& state,
    bool expanded) {
    auto preview = state->nativePreview.get();
    auto image = state->nativePreviewImage.get();
    if (!preview || !image || state->previewExpanded == expanded) {
        return;
    }

    if (expanded) {
        state->previewOriginalHeight = preview.Height();
        state->previewOriginalMinHeight = preview.MinHeight();
        double expandedHeight = std::clamp(preview.ActualHeight() * 1.65,
                                           340.0, 520.0);
        preview.Height(expandedHeight);
        preview.MinHeight(expandedHeight);
        image.Stretch(
            winrt::Microsoft::UI::Xaml::Media::Stretch::UniformToFill);
    } else {
        preview.Height(state->previewOriginalHeight);
        preview.MinHeight(state->previewOriginalMinHeight);
        image.Stretch(winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
    }
    state->previewExpanded = expanded;
}

muxc::StackPanel BetterPanelMakeRow() {
    muxc::StackPanel row;
    row.Orientation(muxc::Orientation::Horizontal);
    row.Spacing(6);
    return row;
}

bool BetterPanelContainsTitleText(DependencyObject const& root,
                                  std::wstring_view filename) {
    if (!root) {
        return false;
    }
    if (auto text = root.try_as<muxc::TextBlock>();
        text && text.Text() == filename) {
        return true;
    }
    int count = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
        GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        if (BetterPanelContainsTitleText(
                winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
                    GetChild(root, i),
                filename)) {
            return true;
        }
    }
    return false;
}

FrameworkElement BetterPanelFindNativeTitleContainer(
    muxc::StackPanel const& host,
    std::wstring_view filename,
    muxc::Grid const& preview) {
    if (!host) {
        return nullptr;
    }
    for (auto const& child : host.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (!element || element.Name() == L"BetterDetailPanelRoot" ||
            (preview && child == preview)) {
            continue;
        }
        if (BetterPanelContainsTitleText(child, filename)) {
            return element;
        }
    }
    return nullptr;
}

muxc::Button BetterPanelFindShareButton(DependencyObject const& root) {
    if (!root) {
        return nullptr;
    }
    if (auto button = root.try_as<muxc::Button>();
        button && button.Name() == L"shareButton") {
        return button;
    }
    int count = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
        GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
            GetChild(root, i);
        if (auto result = BetterPanelFindShareButton(child)) {
            return result;
        }
    }
    return nullptr;
}

FrameworkElement BetterPanelFindNativeDetailsSection(
    DependencyObject const& root) {
    if (!root) return nullptr;
    if (auto element = root.try_as<FrameworkElement>()) {
        if (element.Name() == L"BetterDetailPanelRoot") return nullptr;
        try {
            if (muxa::AutomationProperties::GetName(element) == L"Details") {
                return element;
            }
        } catch (...) {
        }
    }
    int count = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
        GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
            GetChild(root, i);
        if (auto result = BetterPanelFindNativeDetailsSection(child)) {
            return result;
        }
    }
    return nullptr;
}

bool BetterPanelIsInsideCustomRoot(FrameworkElement const& element) {
    DependencyObject current = element;
    for (int level = 0; level < 64 && current; ++level) {
        if (auto currentElement = current.try_as<FrameworkElement>();
            currentElement &&
            currentElement.Name() == L"BetterDetailPanelRoot") {
            return true;
        }
        current = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
            GetParent(current);
    }
    return false;
}

void BetterPanelSuppressNativeElementOnAdd(
    FrameworkElement const& element) {
    if (!element || BetterPanelIsInsideCustomRoot(element)) return;

    try {
        if (muxa::AutomationProperties::GetName(element) == L"Details") {
            // OnVisualTreeChange(Add) runs as Explorer inserts the element,
            // before it can survive to a later BetterPanelRefresh frame.
            element.Visibility(Visibility::Collapsed);
            return;
        }
    } catch (...) {
    }

    auto text = element.try_as<muxc::TextBlock>();
    if (!text || std::wstring(text.Text()).find(
                     L"Select a single file to get more information") ==
                     std::wstring::npos) {
        return;
    }

    auto weakMarker = winrt::make_weak(element);
    element.Loaded([weakMarker](auto const&, RoutedEventArgs const&) {
        auto marker = weakMarker.get();
        if (!marker || BetterPanelIsInsideCustomRoot(marker)) return;

        FrameworkElement banner = nullptr;
        DependencyObject current = marker;
        for (int level = 0; level < 8 && current; ++level) {
            if (auto candidate = current.try_as<FrameworkElement>();
                candidate && candidate.ActualWidth() > 180 &&
                candidate.ActualHeight() >= 30 &&
                candidate.ActualHeight() <= 100) {
                banner = candidate;
            }
            auto parent = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
                GetParent(current);
            if (auto parentElement = parent.try_as<FrameworkElement>();
                parentElement &&
                (parentElement.Name() == L"DetailsViewThumbnail" ||
                 parentElement.Name() == L"BetterDetailPanelRoot")) {
                break;
            }
            current = parent;
        }
        if (banner) banner.Visibility(Visibility::Collapsed);
    });
}

void BetterPanelFindNativeInfoBannerCandidate(
    DependencyObject const& root,
    std::vector<DependencyObject> const& hostAncestors,
    FrameworkElement& bestCandidate,
    int& bestDistance) {
    if (!root) return;
    bool marker = false;
    if (auto text = root.try_as<muxc::TextBlock>()) {
        marker = std::wstring(text.Text()).find(
                     L"Select a single file to get more information") !=
                 std::wstring::npos;
    }
    if (auto icon = root.try_as<muxc::FontIcon>()) {
        marker = marker || icon.Glyph() == L"\uE946";
    }
    if (auto element = root.try_as<FrameworkElement>()) {
        if (element.Name() == L"BetterDetailPanelRoot") return;
        try {
            auto automationName =
                std::wstring(muxa::AutomationProperties::GetName(element));
            std::transform(automationName.begin(), automationName.end(),
                           automationName.begin(), towlower);
            marker = marker || automationName == L"information" ||
                     automationName == L"info";
        } catch (...) {
            // Some private Explorer elements reject automation-property
            // queries. They must not abort the recursive XAML-tree search.
        }
    }
    if (marker) {
        DependencyObject current = root;
        // Never fall back to the marker itself. During initial layout the
        // parent banner can still report a zero size; caching the TextBlock or
        // FontIcon at that point hides only the message and leaves the native
        // outline behind permanently. Return no candidate until the complete
        // sized container is available so a later refresh can find it.
        FrameworkElement candidate = nullptr;
        for (int level = 0; level < 8 && current; ++level) {
            // Explorer's empty-selection notice isn't always a Border. On
            // current builds the text and information icon are hosted by a
            // Grid-like FrameworkElement, so requiring Border leaves the
            // outlined container behind after its text is collapsed.
            if (auto element = current.try_as<FrameworkElement>();
                element && element.Visibility() == Visibility::Visible &&
                element.Name() != L"BetterDetailPanelRoot" &&
                element.ActualWidth() > 180 &&
                element.ActualHeight() >= 30 &&
                element.ActualHeight() <= 100) {
                candidate = element;
            }
            auto parent = winrt::Microsoft::UI::Xaml::Media::
                VisualTreeHelper::GetParent(current);
            if (auto parentElement = parent.try_as<FrameworkElement>()) {
                auto name = std::wstring(parentElement.Name());
                if (name == L"DetailsViewThumbnail" ||
                    name == L"BetterDetailPanelRoot") {
                    break;
                }
            }
            current = parent;
        }
        if (candidate) {
            DependencyObject currentCandidate = candidate;
            for (int candidateDepth = 0;
                 candidateDepth < 64 && currentCandidate;
                 ++candidateDepth) {
                for (size_t hostDepth = 0;
                     hostDepth < hostAncestors.size(); ++hostDepth) {
                    if (currentCandidate == hostAncestors[hostDepth]) {
                        int distance = candidateDepth +
                                       static_cast<int>(hostDepth);
                        if (distance < bestDistance) {
                            bestDistance = distance;
                            bestCandidate = candidate;
                        }
                        currentCandidate = nullptr;
                        break;
                    }
                }
                if (currentCandidate) {
                    currentCandidate =
                        winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
                            GetParent(currentCandidate);
                }
            }
        }
    }
    int count = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
        GetChildrenCount(root);
    for (int index = 0; index < count; ++index) {
        BetterPanelFindNativeInfoBannerCandidate(
            winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::GetChild(
                root, index),
            hostAncestors, bestCandidate, bestDistance);
    }
}

FrameworkElement BetterPanelFindNativeInfoBanner(
    DependencyObject const& root,
    FrameworkElement const& host) {
    if (!root || !host) return nullptr;
    std::vector<DependencyObject> hostAncestors;
    DependencyObject current = host;
    for (int level = 0; level < 64 && current; ++level) {
        hostAncestors.push_back(current);
        current = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
            GetParent(current);
    }
    FrameworkElement bestCandidate = nullptr;
    int bestDistance = INT_MAX;
    BetterPanelFindNativeInfoBannerCandidate(
        root, hostAncestors, bestCandidate, bestDistance);
    return bestCandidate;
}

std::wstring BetterPanelGetActiveFolderParsingName() {
    HWND tab = BetterPanelGetFocusedTabWindow();
    auto browser = BetterPanelGetShellBrowser(tab);
    if (!browser) return {};

    winrt::com_ptr<IShellView> shellView;
    if (FAILED(browser->QueryActiveShellView(shellView.put())) || !shellView) {
        return {};
    }
    winrt::com_ptr<IFolderView2> folderView;
    if (FAILED(shellView->QueryInterface(IID_PPV_ARGS(folderView.put()))) ||
        !folderView) {
        return {};
    }
    winrt::com_ptr<IShellItem> folderItem;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(folderItem.put()))) ||
        !folderItem) {
        return {};
    }
    PWSTR rawName = nullptr;
    if (FAILED(folderItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING,
                                          &rawName)) ||
        !rawName) {
        return {};
    }
    std::wstring name(rawName);
    CoTaskMemFree(rawName);
    return name;
}

std::wstring BetterPanelGetActiveFolderDisplayName() {
    HWND tab = BetterPanelGetFocusedTabWindow();
    auto browser = BetterPanelGetShellBrowser(tab);
    if (!browser) return {};
    winrt::com_ptr<IShellView> shellView;
    if (FAILED(browser->QueryActiveShellView(shellView.put())) || !shellView) {
        return {};
    }
    winrt::com_ptr<IFolderView2> folderView;
    if (FAILED(shellView->QueryInterface(IID_PPV_ARGS(folderView.put()))) ||
        !folderView) {
        return {};
    }
    winrt::com_ptr<IShellItem> folderItem;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(folderItem.put()))) ||
        !folderItem) {
        return {};
    }
    PWSTR rawName = nullptr;
    if (FAILED(folderItem->GetDisplayName(SIGDN_NORMALDISPLAY, &rawName)) ||
        !rawName) {
        return {};
    }
    std::wstring name(rawName);
    CoTaskMemFree(rawName);
    return name;
}

bool BetterPanelIsActiveHome() {
    auto parsingName = BetterPanelGetActiveFolderParsingName();
    std::transform(parsingName.begin(), parsingName.end(), parsingName.begin(),
                   towlower);
    // Explorer Home (formerly Quick access) is a virtual shell namespace.
    if (parsingName.find(
            L"{679f85cb-0220-4080-b29b-5540cc05aab6}") !=
        std::wstring::npos) {
        return true;
    }
    auto displayName = BetterPanelGetActiveFolderDisplayName();
    return _wcsicmp(displayName.c_str(), L"Home") == 0;
}

bool BetterPanelPointIsInActiveShellView(POINT point, HWND* target = nullptr) {
    HWND tab = BetterPanelGetFocusedTabWindow();
    auto browser = BetterPanelGetShellBrowser(tab);
    if (!browser) return false;
    winrt::com_ptr<IShellView> shellView;
    if (FAILED(browser->QueryActiveShellView(shellView.put())) || !shellView) {
        return false;
    }
    HWND viewWindow = nullptr;
    if (FAILED(shellView->GetWindow(&viewWindow)) || !viewWindow) return false;
    HWND pointWindow = WindowFromPoint(point);
    if (!pointWindow ||
        (pointWindow != viewWindow && !IsChild(viewWindow, pointWindow))) {
        return false;
    }
    if (target) *target = pointWindow;
    return true;
}

void BetterPanelPrepareMiddleClick(MSG const* message) {
    if (!message || message->message != WM_MBUTTONDOWN) return;
    HWND target = nullptr;
    if (!BetterPanelPointIsInActiveShellView(message->pt, &target)) return;

    // Modern Explorer doesn't always select an unselected item with the middle
    // button. Select the item under the pointer first so the native tab verb
    // receives the intended folder rather than an old or empty selection.
    POINT clientPoint = message->pt;
    if (!ScreenToClient(target, &clientPoint)) return;
    LPARAM coordinates = MAKELPARAM(clientPoint.x, clientPoint.y);
    SendMessageW(target, WM_LBUTTONDOWN, MK_LBUTTON, coordinates);
    SendMessageW(target, WM_LBUTTONUP, 0, coordinates);
    BetterPanelInvalidateExplorerQueryCaches();
}

void BetterPanelHandleMiddleClick(MSG const* message) {
    if (!message || message->message != WM_MBUTTONUP) return;

    HWND tab = BetterPanelGetFocusedTabWindow();
    auto browser = BetterPanelGetShellBrowser(tab);
    if (!browser) return;
    if (!BetterPanelPointIsInActiveShellView(message->pt)) return;

    auto paths = BetterPanelExtractPaths(browser);
    if (paths.size() != 1) return;
    DWORD attributes = GetFileAttributesW(paths.front().c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return;
    }

    SHELLEXECUTEINFOW executeInfo{sizeof(executeInfo)};
    executeInfo.fMask = SEE_MASK_ASYNCOK | SEE_MASK_FLAG_NO_UI;
    executeInfo.hwnd = GetAncestor(tab, GA_ROOT);
    executeInfo.lpVerb = L"opennewtab";
    executeInfo.lpFile = paths.front().c_str();
    executeInfo.nShow = SW_SHOWNORMAL;
    ShellExecuteExW(&executeInfo);
}

void BetterPanelUnwatchNativeDetailsVisibility(
    std::shared_ptr<BetterPanelState> const& state) {
    if (!state || !state->nativeDetailsVisibilityToken) return;
    if (auto hooked = state->nativeDetailsVisibilityHook.get()) {
        try {
            hooked.UnregisterPropertyChangedCallback(
                UIElement::VisibilityProperty(),
                state->nativeDetailsVisibilityToken);
        } catch (...) {
        }
    }
    state->nativeDetailsVisibilityHook = {};
    state->nativeDetailsVisibilityToken = 0;
}

void BetterPanelWatchNativeDetailsVisibility(
    std::shared_ptr<BetterPanelState> const& state,
    FrameworkElement const& section) {
    if (!state || !section) return;
    if (state->nativeDetailsVisibilityHook.get() == section &&
        state->nativeDetailsVisibilityToken) {
        return;
    }

    BetterPanelUnwatchNativeDetailsVisibility(state);
    std::weak_ptr<BetterPanelState> weakState = state;
    auto weakSection = winrt::make_weak(section);
    state->nativeDetailsVisibilityToken =
        section.RegisterPropertyChangedCallback(
            UIElement::VisibilityProperty(),
            [weakState, weakSection](DependencyObject const&,
                                     DependencyProperty const&) {
                auto currentState = weakState.lock();
                auto currentSection = weakSection.get();
                if (!currentState || currentState->unloaded ||
                    !currentSection) {
                    return;
                }
                if (currentSection.Visibility() != Visibility::Collapsed) {
                    currentSection.Visibility(Visibility::Collapsed);
                }
            });
    state->nativeDetailsVisibilityHook = weakSection;
}

void BetterPanelHideNativeDetails(
    std::shared_ptr<BetterPanelState> const& state) {
    auto section = state->nativeDetailsSection.get();
    auto host = state->host.get();
    if (!host || !host.XamlRoot()) return;

    // Explorer can replace the native Details section when the selection or
    // folder template changes. A still-valid weak reference can therefore
    // point at the old collapsed section while the replacement flashes on
    // screen. Resolve the current section on every real panel refresh.
    auto currentSection =
        BetterPanelFindNativeDetailsSection(host.XamlRoot().Content());
    if (currentSection && currentSection != section) {
        section = currentSection;
        state->nativeDetailsSection = winrt::make_weak(section);
        state->nativeDetailsVisibility = section.Visibility();
    }
    if (section) {
        BetterPanelWatchNativeDetailsVisibility(state, section);
        section.Visibility(Visibility::Collapsed);
    }
}

void BetterPanelEnsureShareActions(
    std::shared_ptr<BetterPanelState> const& state) {
    if (state->shareActionRow.get()) {
        return;
    }
    ULONGLONG now = GetTickCount64();
    if (state->shareLastSearchTick != 0 &&
        now - state->shareLastSearchTick < 1500) {
        return;
    }
    state->shareLastSearchTick = now;
    auto host = state->host.get();
    if (!host || !host.XamlRoot()) {
        return;
    }
    auto share = BetterPanelFindShareButton(host.XamlRoot().Content());
    if (!share) {
        return;
    }
    auto parent = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
                      GetParent(share)
                      .try_as<muxc::Panel>();
    if (!parent) {
        return;
    }
    uint32_t shareIndex = 0;
    if (!parent.Children().IndexOf(share, shareIndex)) {
        return;
    }

    auto weakStatus = state->status;
    muxc::Grid row;
    row.Name(L"BetterDetailPanelShareActions");
    row.HorizontalAlignment(HorizontalAlignment::Stretch);
    row.Margin(share.Margin());
    muxc::ColumnDefinition primaryActionsColumn;
    primaryActionsColumn.Width(GridLength{1, GridUnitType::Star});
    muxc::ColumnDefinition deleteActionColumn;
    deleteActionColumn.Width(GridLength{1, GridUnitType::Auto});
    row.ColumnDefinitions().Append(primaryActionsColumn);
    row.ColumnDefinitions().Append(deleteActionColumn);
    muxc::Grid::SetRow(row, muxc::Grid::GetRow(share));
    muxc::Grid::SetColumn(row, muxc::Grid::GetColumn(share));
    muxc::Grid::SetRowSpan(row, muxc::Grid::GetRowSpan(share));
    muxc::Grid::SetColumnSpan(row, muxc::Grid::GetColumnSpan(share));

    state->nativeShareButton = winrt::make_weak(share);
    state->nativeShareParent = winrt::make_weak(parent);
    state->nativeShareIndex = shareIndex;
    state->nativeShareMargin = share.Margin();
    state->nativeShareVisibility = share.Visibility();

    // Use Explorer's own Share control as the visual and sizing reference for
    // the primary file actions and the compact overlay controls. Leave the
    // native button in Explorer's own visual tree; its sharing view model owns
    // popup state that can be refreshed after sleep or hibernation.
    share.UpdateLayout();
    double primaryActionWidth = share.ActualWidth();
    if (primaryActionWidth < 1) primaryActionWidth = 80;
    auto nativeActionStyle = share.Style();
    state->nativeButtonStyle = nativeActionStyle;
    auto applyNativeButtonVisual = [&](muxc::Button const& button) {
        button.Style(nativeActionStyle);
        button.Background(share.Background());
        button.BorderBrush(share.BorderBrush());
        button.BorderThickness(share.BorderThickness());
        auto cornerRadius = share.CornerRadius();
        if (cornerRadius.TopLeft <= 0 && cornerRadius.TopRight <= 0 &&
            cornerRadius.BottomRight <= 0 && cornerRadius.BottomLeft <= 0) {
            cornerRadius = CornerRadius{4};
        }
        button.CornerRadius(cornerRadius);
    };
    if (auto expand = state->previewExpandButton.get()) {
        applyNativeButtonVisual(expand);
        BetterPanelApplyNativeButtonStyle(state, expand);
    }
    if (auto expand = state->multiPreviewExpandButton.get()) {
        applyNativeButtonVisual(expand);
        BetterPanelApplyNativeButtonStyle(state, expand);
    }
    if (auto expand = state->gifExpandButton.get()) {
        applyNativeButtonVisual(expand);
        BetterPanelApplyNativeButtonStyle(state, expand);
    }

    auto primaryActions = BetterPanelMakeRow();
    primaryActions.HorizontalAlignment(HorizontalAlignment::Left);
    row.Children().Append(primaryActions);

    auto actionIconBrush =
        winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(
            winrt::Windows::UI::ColorHelper::FromArgb(255, 64, 200, 255));

    double shareHeight = share.ActualHeight();
    if (shareHeight < 1) shareHeight = 30;

    auto shareButton = BetterPanelMakeIconButton(L"Share", L"\uE72D");
    applyNativeButtonVisual(shareButton);
    shareButton.Width(primaryActionWidth);
    shareButton.Height(shareHeight);
    shareButton.Padding(Thickness{6, 4, 6, 4});
    state->shareButton = winrt::make_weak(shareButton);
    auto weakNativeShare = winrt::make_weak(share);
    shareButton.Click(
        [weakNativeShare, weakStatus](auto const&, RoutedEventArgs const&) {
            try {
                auto nativeShare = weakNativeShare.get();
                if (!nativeShare) {
                    BetterPanelSetStatus(weakStatus, L"Share is unavailable");
                    return;
                }
                auto peer = muxap::FrameworkElementAutomationPeer::
                    CreatePeerForElement(nativeShare);
                if (!peer) peer = muxap::ButtonAutomationPeer(nativeShare);
                auto provider = peer.GetPattern(muxap::PatternInterface::Invoke)
                                    .try_as<muxapr::IInvokeProvider>();
                if (!provider) {
                    BetterPanelSetStatus(weakStatus, L"Share is unavailable");
                    return;
                }
                provider.Invoke();
            } catch (...) {
                BetterPanelSetStatus(weakStatus, L"Share is unavailable");
            }
        });
    primaryActions.Children().Append(shareButton);
    share.Visibility(Visibility::Collapsed);

    auto openButton = BetterPanelMakeIconButton(L"Open", L"\uE8E5");
    applyNativeButtonVisual(openButton);
    openButton.Width(primaryActionWidth);
    openButton.Padding(Thickness{6, 4, 6, 4});
    if (auto content = openButton.Content().try_as<muxc::StackPanel>()) {
        content.Spacing(4);
        if (content.Children().Size() > 0) {
            content.Children().RemoveAt(0);
        }
        muxc::Grid openActionIconHost;
        openActionIconHost.Width(16);
        openActionIconHost.Height(16);

        muxc::FontIcon openActionFallbackIcon;
        openActionFallbackIcon.Glyph(L"\uE7C3");
        openActionFallbackIcon.FontSize(13);
        state->openActionFallbackIcon =
            winrt::make_weak(openActionFallbackIcon);
        openActionIconHost.Children().Append(openActionFallbackIcon);

        muxc::Image openActionIcon;
        openActionIcon.Width(16);
        openActionIcon.Height(16);
        openActionIcon.Stretch(
            winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
        state->openActionIcon = winrt::make_weak(openActionIcon);
        openActionIconHost.Children().Append(openActionIcon);
        content.Children().InsertAt(0, openActionIconHost);
    }
    if (!state->selectedPath.empty()) {
        BetterPanelLoadOpenActionIcon(
            std::weak_ptr<BetterPanelState>(state), state->selectedPath);
    }
    openButton.Height(shareHeight);
    state->openButton = winrt::make_weak(openButton);
    openButton.Click(
        [weakStatus](winrt::Windows::Foundation::IInspectable const&,
                     RoutedEventArgs const&) {
            auto path = BetterPanelGetSelectedPath();
            auto result = path.empty() ? 0 : reinterpret_cast<INT_PTR>(
                ShellExecuteW(GetForegroundWindow(), L"open", path.c_str(),
                              nullptr, nullptr, SW_SHOWNORMAL));
            BetterPanelSetStatus(weakStatus,
                                 result > 32 ? L"" : L"Open failed");
        });
    primaryActions.Children().Append(openButton);

    auto openWithButton = BetterPanelMakeIconButton(L"Open with", L"\uE7AC");
    applyNativeButtonVisual(openWithButton);
    openWithButton.Width(primaryActionWidth + 12);
    openWithButton.Padding(Thickness{6, 4, 6, 4});
    if (auto content = openWithButton.Content().try_as<muxc::StackPanel>()) {
        content.Spacing(4);
        if (content.Children().Size() > 0) {
            content.Children().RemoveAt(0);
        }
        muxc::Grid openWithIcon;
        openWithIcon.Width(16);
        openWithIcon.Height(16);
        muxc::FontIcon openWithBase;
        openWithBase.Glyph(L"\uE7AC");
        openWithBase.FontSize(13);
        openWithBase.Foreground(share.Foreground());
        openWithIcon.Children().Append(openWithBase);
        muxc::Border openWithAccent;
        openWithAccent.Width(5);
        openWithAccent.Height(5);
        openWithAccent.CornerRadius(CornerRadius{2.5});
        openWithAccent.Background(actionIconBrush);
        openWithAccent.HorizontalAlignment(HorizontalAlignment::Left);
        openWithAccent.VerticalAlignment(VerticalAlignment::Bottom);
        openWithAccent.Margin(Thickness{0, 0, 0, 1});
        openWithIcon.Children().Append(openWithAccent);
        content.Children().InsertAt(0, openWithIcon);
        if (content.Children().Size() > 1) {
            if (auto text = content.Children().GetAt(1).try_as<muxc::TextBlock>()) {
                text.FontSize(11);
            }
        }
    }
    openWithButton.Height(shareHeight);
    state->openWithButton = winrt::make_weak(openWithButton);
    openWithButton.Click(
        [weakStatus](winrt::Windows::Foundation::IInspectable const&,
                     RoutedEventArgs const&) {
            auto path = BetterPanelGetSelectedPath();
            if (path.empty()) {
                BetterPanelSetStatus(weakStatus, L"Select one file first");
                return;
            }
            OPENASINFO info{path.c_str(), nullptr, OAIF_EXEC};
            HRESULT hr = SHOpenWithDialog(GetForegroundWindow(), &info);
            BetterPanelSetStatus(weakStatus,
                                 SUCCEEDED(hr) ? L"" : L"Open With failed");
        });
    primaryActions.Children().Append(openWithButton);

    auto singleDeleteButton = BetterPanelMakeButton(L"");
    muxc::FontIcon singleDeleteIcon;
    singleDeleteIcon.Glyph(L"\uE74D");
    singleDeleteIcon.FontSize(13);
    singleDeleteButton.Content(singleDeleteIcon);
    singleDeleteButton.Width(32);
    singleDeleteButton.Height(shareHeight);
    singleDeleteButton.MinHeight(shareHeight);
    singleDeleteButton.Padding(Thickness{0, 0, 0, 0});
    singleDeleteButton.Margin(Thickness{18, 0, 0, 0});
    singleDeleteButton.HorizontalAlignment(HorizontalAlignment::Right);
    singleDeleteButton.Visibility(Visibility::Collapsed);
    muxc::Grid::SetColumn(singleDeleteButton, 1);
    muxa::AutomationProperties::SetName(singleDeleteButton,
                                        L"Move selected item to Recycle Bin");
    muxc::ToolTipService::SetToolTip(
        singleDeleteButton, winrt::box_value(L"Move to Recycle Bin"));
    state->singleDeleteButton = winrt::make_weak(singleDeleteButton);
    std::weak_ptr<BetterPanelState> weakDeleteState = state;
    singleDeleteButton.Click(
        [weakDeleteState, weakStatus](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&) {
            auto state = weakDeleteState.lock();
            if (!state || state->selectedPath.empty() ||
                BetterPanelIsProtectedDeleteLocation(state->selectedPath)) {
                return;
            }
            std::vector<std::wstring> sources{state->selectedPath};
            if (state->timer) state->timer.Stop();
            bool queued = state->dispatcher.TryEnqueue(
                [weakDeleteState, weakStatus,
                 sources = std::move(sources)]() {
                    HRESULT result = BetterPanelDeleteItems(sources);
                    if (auto state = weakDeleteState.lock();
                        state && !state->unloaded) {
                        BetterPanelSetStatus(
                            weakStatus,
                            SUCCEEDED(result) ? L"Moved to Recycle Bin"
                                              : L"Delete cancelled");
                        state->transferLastScanTick = 0;
                        if (state->timer) state->timer.Start();
                        BetterPanelRefresh(state);
                    }
                });
            if (!queued && state->timer) state->timer.Start();
        });
    row.Children().Append(singleDeleteButton);

    auto extractButton = BetterPanelMakeIconButton(L"Extract", L"\uE8B7");
    extractButton.Height(shareHeight);
    extractButton.Visibility(Visibility::Collapsed);
    state->extractButton = winrt::make_weak(extractButton);
    extractButton.Click(
        [weakStatus](winrt::Windows::Foundation::IInspectable const&,
                     RoutedEventArgs const&) {
            auto path = BetterPanelGetSelectedPath();
            if (path.empty() || !BetterPanelIsArchiveFile(path)) {
                BetterPanelSetStatus(weakStatus,
                                     L"Select one supported archive first");
                return;
            }
            HRESULT result = BetterPanelShowSimpleExtractMenu(path);
            if (result == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
                BetterPanelSetStatus(
                    weakStatus,
                    L"No extraction commands are registered for this file");
            } else if (FAILED(result)) {
                BetterPanelSetStatus(weakStatus,
                                     L"Extraction menu unavailable");
            } else {
                BetterPanelSetStatus(weakStatus, L"");
            }
        });
    primaryActions.Children().Append(extractButton);

    if (auto actionsHost = state->actionsHost.get()) {
        actionsHost.Children().InsertAt(0, row);
    } else {
        parent.Children().InsertAt(shareIndex, row);
    }
    state->shareActionRow = winrt::make_weak(row);
    if (auto panel = state->panel.get()) {
        BetterPanelPruneButtonLayoutSnapshots(state);
        BetterPanelApplyNativeButtonStyle(state, panel);
    }
}

void BetterPanelEndRename(std::shared_ptr<BetterPanelState> const& state,
                          bool audio,
                          bool commit) {
    auto edit = audio ? state->audioRenameBox.get()
                      : state->fileRenameBox.get();
    auto title = audio ? state->audioTitle.get() : state->fileTitle.get();
    auto pencil = audio ? state->audioRenameButton.get()
                        : state->fileRenameButton.get();
    if (!edit || !title || !pencil) {
        return;
    }

    if (commit) {
        std::wstring newName = edit.Text().c_str();
        if (newName.empty() || newName == L"." || newName == L".." ||
            newName.find_first_of(L"\\/:*?\"<>|") != std::wstring::npos) {
            BetterPanelSetStatus(state->status, L"That filename isn't valid");
            return;
        }

        std::wstring oldPath = state->selectedPath;
        size_t slash = oldPath.find_last_of(L"\\/");
        if (slash == std::wstring::npos) {
            BetterPanelSetStatus(state->status, L"Rename is unavailable");
            return;
        }
        std::wstring newPath = oldPath.substr(0, slash + 1) + newName;
        if (_wcsicmp(oldPath.c_str(), newPath.c_str()) != 0) {
            bool reloadVideo = BetterPanelIsVideoFile(oldPath);
            if (reloadVideo) {
                if (auto player = state->videoPlayer.get()) {
                    player.Source(nullptr);
                }
            }
            bool closeMedia = false;
            {
                std::lock_guard lock(g_betterMediaMutex);
                closeMedia = g_betterMediaPath == oldPath;
            }
            if (closeMedia) {
                BetterPanelCloseMedia();
            }
            if (!MoveFileExW(oldPath.c_str(), newPath.c_str(),
                             MOVEFILE_WRITE_THROUGH)) {
                WCHAR message[96]{};
                swprintf_s(message, L"Rename failed (error %lu)",
                           GetLastError());
                BetterPanelSetStatus(state->status, message);
                return;
            }
            SHChangeNotify(SHCNE_RENAMEITEM, SHCNF_PATHW, oldPath.c_str(),
                           newPath.c_str());
            state->selectedPath = newPath;
            if (reloadVideo) {
                BetterPanelLoadVideo(state, newPath);
            }
        }
        title.Text(newName);
        if (auto other = audio ? state->fileTitle.get()
                               : state->audioTitle.get()) {
            other.Text(newName);
        }
        BetterPanelSetStatus(state->status, L"");
    }

    edit.Visibility(Visibility::Collapsed);
    title.Visibility(Visibility::Visible);
    pencil.Visibility(Visibility::Visible);
}

void BetterPanelBeginRename(std::shared_ptr<BetterPanelState> const& state,
                            bool audio) {
    auto edit = audio ? state->audioRenameBox.get()
                      : state->fileRenameBox.get();
    auto title = audio ? state->audioTitle.get() : state->fileTitle.get();
    auto pencil = audio ? state->audioRenameButton.get()
                        : state->fileRenameButton.get();
    if (!edit || !title || !pencil || state->selectedPath.empty()) {
        return;
    }

    std::wstring filename = BetterPanelFileName(state->selectedPath);
    if (!audio) {
        double availableWidth = 320;
        if (auto panel = state->panel.get(); panel && panel.ActualWidth() > 80) {
            availableWidth = panel.ActualWidth() - 24;
        }
        double editWidth = std::clamp(title.ActualWidth() + 38, 160.0,
                                      std::max(160.0, availableWidth));
        edit.MaxWidth(std::max(160.0, availableWidth));
        edit.Width(editWidth);
    }
    edit.Text(filename);
    title.Visibility(Visibility::Collapsed);
    pencil.Visibility(Visibility::Collapsed);
    edit.Visibility(Visibility::Visible);
    edit.Focus(FocusState::Programmatic);
    size_t dot = filename.find_last_of(L'.');
    edit.Select(0, static_cast<int32_t>(
                       dot == std::wstring::npos ? filename.size() : dot));
}

void BetterPanelSetVisibilityIfChanged(FrameworkElement const& element,
                                       Visibility visibility) {
    if (element && element.Visibility() != visibility) {
        element.Visibility(visibility);
    }
}

void BetterPanelSetTextIfChanged(muxc::TextBlock const& textBlock,
                                 std::wstring const& text) {
    if (textBlock && textBlock.Text() != text) {
        textBlock.Text(text);
    }
}

void BetterPanelRefreshPlaybackState(
    std::shared_ptr<BetterPanelState> const& state) {
    if (!state || state->unloaded ||
        !BetterPanelIsAudioFile(state->selectedPath)) {
        return;
    }
    if (auto panel = state->panel.get(); panel && !panel.IsLoaded()) return;

    auto timeline = state->timeline.get();
    auto timeText = state->timeText.get();
    auto playButton = state->playButton.get();
    if (!timeline || !timeText || !playButton) return;

    int64_t position = 0;
    int64_t duration = 0;
    bool playing = false;
    try {
        std::lock_guard lock(g_betterMediaMutex);
        if (g_betterMediaPlayer &&
            g_betterMediaPath == state->selectedPath) {
            auto session = g_betterMediaPlayer.PlaybackSession();
            position = session.Position().count();
            duration = session.NaturalDuration().count();
            playing = session.PlaybackState() ==
                      wmp::MediaPlaybackState::Playing;
        }
    } catch (...) {
    }

    int64_t positionSecond = position / 10000000;
    int64_t durationSecond = duration / 10000000;
    if (positionSecond != state->displayedPositionSecond ||
        durationSecond != state->displayedDurationSecond) {
        state->updatingTimeline = true;
        timeline.Maximum(std::max(1.0, duration / 10000000.0));
        timeline.Value(std::clamp(position / 10000000.0, 0.0,
                                  timeline.Maximum()));
        state->updatingTimeline = false;
        timeText.Text(BetterPanelFormatTime(position) + L" / " +
                      (duration ? BetterPanelFormatTime(duration) : L"--:--"));
        state->displayedPositionSecond = positionSecond;
        state->displayedDurationSecond = durationSecond;
    }
    if (!state->displayedPlaybackInitialized ||
        playing != state->displayedPlaying) {
        if (auto icon = playButton.Content().try_as<muxc::FontIcon>()) {
            icon.Glyph(playing ? L"\uE769" : L"\uE768");
        }
        muxa::AutomationProperties::SetName(playButton,
                                            playing ? L"Pause" : L"Play");
        if (auto quickPlay = state->quickAudioPlayButton.get()) {
            if (auto icon = quickPlay.Content().try_as<muxc::FontIcon>()) {
                icon.Glyph(playing ? L"\uE769" : L"\uE768");
            }
            muxa::AutomationProperties::SetName(quickPlay,
                                                playing ? L"Pause" : L"Play");
        }
        state->displayedPlaying = playing;
        state->displayedPlaybackInitialized = true;
    }
}

void BetterPanelRefresh(std::shared_ptr<BetterPanelState> const& state) {
    if (!state || state->unloaded) return;
    if (auto panel = state->panel.get(); panel && !panel.IsLoaded()) return;
    BetterPanelExplorerWindowScope explorerWindowScope(state->explorerWindow);
    BetterPanelEnsureShareActions(state);
    BetterPanelHideNativeDetails(state);
    auto activeSelection = BetterPanelGetActiveSelectionPaths();
    bool isHome = activeSelection.empty() && BetterPanelIsActiveHome();
    Wh_Log(L"Refresh explorer=%p tab=%p selectionCount=%zu first=%s home=%d",
           state->explorerWindow, BetterPanelGetFocusedTabWindow(),
           activeSelection.size(),
           activeSelection.empty() ? L"" : activeSelection.front().c_str(),
           isHome ? 1 : 0);
    if (!isHome && state->homeWasVisible && activeSelection.empty() &&
        BetterPanelGetActiveFolderPath().empty() &&
        BetterPanelGetActiveFolderDisplayName().empty()) {
        // Keep the Home dashboard through brief shell-view refresh gaps.
        isHome = true;
    }
    ULONGLONG selectionTick = GetTickCount64();
    if (!activeSelection.empty()) {
        state->lastNonEmptySelectionTick = selectionTick;
    }
    DWORD previousPathAttributes =
        state->selectedPath.empty()
            ? INVALID_FILE_ATTRIBUTES
            : GetFileAttributesW(state->selectedPath.c_str());
    bool previousPathIsFile =
        previousPathAttributes != INVALID_FILE_ATTRIBUTES &&
        !(previousPathAttributes & FILE_ATTRIBUTE_DIRECTORY);
    bool preserveSelectionDuringResize =
        !isHome && activeSelection.empty() && previousPathIsFile &&
        (((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0) ||
         (state->lastNonEmptySelectionTick != 0 &&
          selectionTick - state->lastNonEmptySelectionTick < 1500));
    bool isMultiSelection = activeSelection.size() > 1;
    auto path = activeSelection.size() == 1
                    ? activeSelection.front()
                    : (activeSelection.empty()
                           ? (preserveSelectionDuringResize
                                  ? state->selectedPath
                                  : BetterPanelGetActiveFolderPath())
                           : state->selectedPath);
    if (path.empty() && !isHome) path = state->selectedPath;
    DWORD pathAttributes =
        path.empty()
            ? INVALID_FILE_ATTRIBUTES
            : (path == state->selectedPath
                   ? previousPathAttributes
                   : GetFileAttributesW(path.c_str()));
    bool isDirectory = pathAttributes != INVALID_FILE_ATTRIBUTES &&
                       (pathAttributes & FILE_ATTRIBUTE_DIRECTORY);
    bool isDriveRoot = isDirectory && PathIsRootW(path.c_str());
    bool isAudio = BetterPanelIsAudioFile(path);
    bool isVideo = BetterPanelIsVideoFile(path);
    bool isGif = BetterPanelIsGifFile(path);
    bool isArchive = BetterPanelIsArchiveFile(path);
    bool isText = BetterPanelIsTextFile(path);
    bool isPdf = BetterPanelIsPdfFile(path);
    if (isMultiSelection) {
        path = state->selectedPath;
        isAudio = BetterPanelIsAudioFile(path);
        isVideo = BetterPanelIsVideoFile(path);
        isGif = BetterPanelIsGifFile(path);
        isArchive = BetterPanelIsArchiveFile(path);
        isText = BetterPanelIsTextFile(path);
        isPdf = BetterPanelIsPdfFile(path);
    }

    if (auto multiActionRow = state->multiActionRow.get()) {
        BetterPanelSetVisibilityIfChanged(
            multiActionRow, isMultiSelection ? Visibility::Visible
                                             : Visibility::Collapsed);
    }
    if (isMultiSelection) {
        BetterPanelLoadMultiSummary(state, activeSelection);
        BetterPanelPopulateMultiPreview(state, activeSelection);
    } else {
        state->multiSelectionKey.clear();
        state->multiPreviewKey.clear();
        state->multiSummaryLoading = false;
        if (auto previewHost = state->multiPreviewHost.get()) {
            previewHost.Children().Clear();
        }
        if (auto multiSelectionText = state->multiSelectionText.get()) {
            BetterPanelSetTextIfChanged(multiSelectionText, L"");
        }
    }
    if (auto shareActionRow = state->shareActionRow.get()) {
        bool canDeleteSingleSelection =
            activeSelection.size() == 1 && !isMultiSelection && !isHome &&
            !isDriveRoot && BetterPanelCanDeleteSelectedPath(path);
        BetterPanelSetVisibilityIfChanged(
            shareActionRow,
            (isMultiSelection || isHome || isDriveRoot || path.empty() ||
             (isDirectory && !canDeleteSingleSelection))
                                ? Visibility::Collapsed
                                : Visibility::Visible);
        if (auto shareButton = state->shareButton.get()) {
            BetterPanelSetVisibilityIfChanged(
                shareButton, isDirectory ? Visibility::Collapsed
                                         : Visibility::Visible);
        }
        if (auto nativeShare = state->nativeShareButton.get()) {
            BetterPanelSetVisibilityIfChanged(nativeShare,
                                               Visibility::Collapsed);
        }
        if (auto deleteButton = state->singleDeleteButton.get()) {
            BetterPanelSetVisibilityIfChanged(
                deleteButton, canDeleteSingleSelection ? Visibility::Visible
                                                       : Visibility::Collapsed);
        }
    }
    if (auto actionsHost = state->actionsHost.get()) {
        BetterPanelSetVisibilityIfChanged(
            actionsHost,
            (isMultiSelection || isHome || isDriveRoot || path.empty())
                ? Visibility::Collapsed
                : Visibility::Visible);
    }

    auto nativeInfoBanner = state->nativeInfoBanner.get();
    if (auto host = state->host.get(); host && host.XamlRoot()) {
        // Explorer can also replace or re-show this notice between selections.
        auto currentBanner = BetterPanelFindNativeInfoBanner(
            host.XamlRoot().Content(), host);
        if (currentBanner && currentBanner != nativeInfoBanner) {
            nativeInfoBanner = currentBanner;
            state->nativeInfoBanner = winrt::make_weak(nativeInfoBanner);
            state->nativeInfoBannerVisibility = nativeInfoBanner.Visibility();
        }
    }
    if (nativeInfoBanner) {
        BetterPanelSetVisibilityIfChanged(nativeInfoBanner,
                                          Visibility::Collapsed);
    }

    bool needsMediaTimer = isAudio && !isMultiSelection;
    if (state->mediaTimer && needsMediaTimer != state->mediaTimerRunning) {
        if (needsMediaTimer) {
            state->mediaTimer.Start();
        } else {
            state->mediaTimer.Stop();
        }
        state->mediaTimerRunning = needsMediaTimer;
    }
    if (auto openButton = state->openButton.get()) {
        BetterPanelSetVisibilityIfChanged(
            openButton, (path.empty() || isDirectory) ? Visibility::Collapsed
                                                      : Visibility::Visible);
    }
    if (auto openWithButton = state->openWithButton.get()) {
        BetterPanelSetVisibilityIfChanged(
            openWithButton,
            (path.empty() || isDirectory) ? Visibility::Collapsed
                                          : Visibility::Visible);
    }
    if (auto renameButton = state->fileRenameButton.get()) {
        BetterPanelSetVisibilityIfChanged(
            renameButton, path.empty() || isDirectory || isMultiSelection
                              ? Visibility::Collapsed
                              : Visibility::Visible);
    }

    HWND activeTab = BetterPanelGetFocusedTabWindow();
    bool refreshTransferContext =
        state->transferLastScanTick == 0 ||
        state->transferCachedActiveTab != activeTab ||
        state->transferCachedSourcePath != state->selectedPath ||
        selectionTick - state->transferLastScanTick >= 1500;
    if (refreshTransferContext) {
        auto transferContext = BetterPanelResolveTransferContext(
            state->selectedPath, state->transferSources);
        if (!transferContext.sources.empty()) {
            state->transferSources = std::move(transferContext.sources);
        }
        state->transferDestination = std::move(transferContext.destination);
        state->transferLastScanTick = selectionTick;
        state->transferCachedActiveTab = activeTab;
        state->transferCachedSourcePath = state->selectedPath;
    }
    bool hasTransferTarget = !isMultiSelection &&
                              !state->transferSources.empty() &&
                              !state->transferDestination.empty();
    bool hasArchiveSource = std::any_of(
        state->transferSources.begin(), state->transferSources.end(),
        [](auto const& source) { return BetterPanelIsArchiveFile(source); });
    if (auto transferRow = state->transferRow.get()) {
        BetterPanelSetVisibilityIfChanged(
            transferRow, hasTransferTarget ? Visibility::Visible
                                           : Visibility::Collapsed);
    }
    if (!hasTransferTarget) {
        if (auto status = state->status.get();
            status && (status.Text() == L"Move completed" ||
                       status.Text() == L"Extracting to the open folder")) {
            BetterPanelSetStatus(state->status, L"");
        }
    }
    if (auto transferExtractButton = state->transferExtractButton.get()) {
        BetterPanelSetVisibilityIfChanged(
            transferExtractButton,
            hasTransferTarget && hasArchiveSource ? Visibility::Visible
                                                  : Visibility::Collapsed);
    }
    if (auto transferMoveButton = state->transferMoveButton.get()) {
        BetterPanelSetVisibilityIfChanged(
            transferMoveButton, hasTransferTarget ? Visibility::Visible
                                                  : Visibility::Collapsed);
        BetterPanelSetIconButtonLabel(
            transferMoveButton,
            hasTransferTarget
                ? BetterPanelTransferMoveLabel(state->transferSources,
                                               state->transferDestination)
                : L"Move to");
    }
    std::wstring transferItemsKey;
    if (hasTransferTarget) {
        for (auto const& source : state->transferSources) {
            transferItemsKey += source;
            transferItemsKey.push_back(L'\n');
        }
    }
    if (transferItemsKey != state->transferItemsKey) {
        state->transferItemsKey = transferItemsKey;
        state->transferItemsExpanded = false;
        if (auto card = state->transferItemsCard.get()) {
            card.Visibility(Visibility::Collapsed);
        }
    }
    if (auto toggle = state->transferItemsToggle.get()) {
        BetterPanelSetVisibilityIfChanged(
            toggle, hasTransferTarget ? Visibility::Visible
                                      : Visibility::Collapsed);
        if (hasTransferTarget) {
            std::wstring label = state->transferSources.size() == 1
                ? L"Item being moved"
                : L"Items being moved (" +
                      std::to_wstring(state->transferSources.size()) + L")";
            label += state->transferItemsExpanded ? L"  \u25be" : L"  \u25b8";
            toggle.Content(winrt::box_value(label));
        }
    }
    if (auto text = state->transferItemsText.get()) {
        BetterPanelSetTextIfChanged(
            text, hasTransferTarget
                      ? BetterPanelTransferItemsText(state->transferSources)
                      : L"");
    }
    if (auto extractButton = state->extractButton.get()) {
        BetterPanelSetVisibilityIfChanged(
            extractButton, isArchive && !isMultiSelection
                               ? Visibility::Visible
                               : Visibility::Collapsed);
    }
    if (auto favoriteButton = state->favoriteButton.get()) {
        BetterPanelSetVisibilityIfChanged(
            favoriteButton,
            !isMultiSelection && !path.empty() && !isDirectory && !isDriveRoot
                ? Visibility::Visible
                : Visibility::Collapsed);
    }
    if (auto printButton = state->printButton.get()) {
        if (state->printHandlerPath != path) {
            state->printHandlerPath = path;
            state->printHandlerAvailable =
                !path.empty() && BetterPanelHasPrintHandler(path);
        }
        BetterPanelSetVisibilityIfChanged(
            printButton,
            !isMultiSelection && state->printHandlerAvailable
                ? Visibility::Visible
                : Visibility::Collapsed);
    }

    if (!isMultiSelection && !path.empty()) {
        auto nativeTitle = state->nativeTitleContainer.get();
        bool shouldSearchNativeTitle =
            state->nativeTitleSearchPath != path ||
            state->nativeTitleLastSearchTick == 0 ||
            selectionTick - state->nativeTitleLastSearchTick >= 1500;
        if (!nativeTitle && shouldSearchNativeTitle) {
            state->nativeTitleSearchPath = path;
            state->nativeTitleLastSearchTick = selectionTick;
            if (auto host = state->host.get()) {
                nativeTitle = BetterPanelFindNativeTitleContainer(
                    host, BetterPanelFileName(path),
                    state->nativePreview.get());
                if (nativeTitle) {
                    state->nativeTitleContainer = winrt::make_weak(nativeTitle);
                    state->nativeTitleVisibility = nativeTitle.Visibility();
                }
            }
        }
        if (nativeTitle) {
            nativeTitle.Visibility(Visibility::Collapsed);
        }
    } else if (isMultiSelection) {
        if (auto nativeTitle = state->nativeTitleContainer.get()) {
            nativeTitle.Visibility(state->nativeTitleVisibility);
        }
    }

    if ((isAudio || isVideo) && state->previewExpanded) {
        BetterPanelSetInlinePreviewExpanded(state, false);
    }

    if (auto audioCard = state->audioCard.get()) {
        BetterPanelSetVisibilityIfChanged(
            audioCard, isAudio && !isMultiSelection &&
                               !state->previewsCollapsed
                           ? Visibility::Visible
                           : Visibility::Collapsed);
    }
    if (auto videoCard = state->videoCard.get()) {
        BetterPanelSetVisibilityIfChanged(
            videoCard, isVideo && !isMultiSelection &&
                               !state->previewsCollapsed
                           ? Visibility::Visible
                           : Visibility::Collapsed);
    }
    if (auto gifCard = state->gifCard.get()) {
        BetterPanelSetVisibilityIfChanged(
            gifCard, isGif && !isMultiSelection && !state->previewsCollapsed
                         ? Visibility::Visible
                         : Visibility::Collapsed);
    }
    if (auto textCard = state->textCard.get()) {
        BetterPanelSetVisibilityIfChanged(
            textCard, isText && !isMultiSelection && !state->previewsCollapsed
                          ? Visibility::Visible
                          : Visibility::Collapsed);
    }
    if (auto pdfCard = state->pdfCard.get()) {
        BetterPanelSetVisibilityIfChanged(
            pdfCard, isPdf && !isMultiSelection && !state->previewsCollapsed
                         ? Visibility::Visible
                         : Visibility::Collapsed);
    }
    if (auto archiveCard = state->archivePreviewCard.get()) {
        BetterPanelSetVisibilityIfChanged(
            archiveCard, isArchive && !isMultiSelection &&
                                  !state->previewsCollapsed
                              ? Visibility::Visible
                              : Visibility::Collapsed);
    }
    if (auto fileTitleRow = state->fileTitleRow.get()) {
        BetterPanelSetVisibilityIfChanged(
            fileTitleRow, (isMultiSelection || isHome || path.empty())
                              ? Visibility::Collapsed
                              : Visibility::Visible);
    }
    if (auto utilities = state->panelUtilities.get()) {
        BetterPanelSetVisibilityIfChanged(
            utilities, (isHome || path.empty()) ? Visibility::Collapsed
                                                : Visibility::Visible);
    }
    if (auto quickAudio = state->quickAudioControls.get()) {
        BetterPanelSetVisibilityIfChanged(
            quickAudio, isAudio && !isMultiSelection && state->previewsCollapsed
                            ? Visibility::Visible
                            : Visibility::Collapsed);
    }
    if (auto quickTitle = state->quickAudioTitle.get()) {
        BetterPanelSetTextIfChanged(
            quickTitle, isAudio ? BetterPanelFileName(path) : L"");
    }
    if (auto insightsCard = state->insightsCard.get()) {
        if (auto border = insightsCard.try_as<muxc::Border>()) {
            border.Padding(isDriveRoot ? Thickness{0, 4, 0, 4}
                                       : Thickness{10, 8, 10, 10});
        }
        BetterPanelSetVisibilityIfChanged(
            insightsCard, !isHome && (isMultiSelection || !path.empty())
                              ? Visibility::Visible
                              : Visibility::Collapsed);
    }
    if (isHome && !state->homeWasVisible) {
        state->homeContentLoaded = false;
    }
    if (auto homeCard = state->homeCard.get()) {
        BetterPanelSetVisibilityIfChanged(
            homeCard, isHome ? Visibility::Visible : Visibility::Collapsed);
    }
    if (auto driveCard = state->driveCard.get()) {
        BetterPanelSetVisibilityIfChanged(
            driveCard, isDriveRoot && !isMultiSelection
                           ? Visibility::Visible
                           : Visibility::Collapsed);
    }
    if (isHome) {
        if (auto content = state->homeContent.get();
            content && content.Children().Size() == 0) {
            state->homeContentLoaded = false;
        }
    }
    if (isHome) BetterPanelPopulateHome(state);
    state->homeWasVisible = isHome;
    if (auto metadataCard = state->metadataCard.get()) {
        BetterPanelSetVisibilityIfChanged(
            metadataCard, isAudio && !isMultiSelection ? Visibility::Visible
                                                       : Visibility::Collapsed);
    }
    if (auto copyUtility = state->detailsCopyHint.get()) {
        BetterPanelSetVisibilityIfChanged(
            copyUtility,
            (isMultiSelection || !path.empty()) && !state->insightsCollapsed
                ? Visibility::Visible
                : Visibility::Collapsed);
    }
    if (auto nativePreview = state->nativePreview.get()) {
        BetterPanelSetVisibilityIfChanged(
            nativePreview,
            (state->previewsCollapsed || isHome || isMultiSelection)
                ? Visibility::Collapsed
                : ((isAudio || isVideo || isGif || isText || isPdf)
                       ? Visibility::Collapsed
                       : Visibility::Visible));
    }
    if (auto previewSurface = state->multiPreviewSurface.get()) {
        BetterPanelSetVisibilityIfChanged(
            previewSurface,
            isMultiSelection && !state->previewsCollapsed && !isHome
                ? Visibility::Visible
                : Visibility::Collapsed);
    }
    if (auto previewHost = state->multiPreviewHost.get()) {
        BetterPanelSetVisibilityIfChanged(
            previewHost, isMultiSelection && !state->previewsCollapsed
                             ? Visibility::Visible
                             : Visibility::Collapsed);
    }
    if (auto previewImage = state->nativePreviewImage.get()) {
        BetterPanelSetVisibilityIfChanged(
            previewImage, isMultiSelection ? Visibility::Collapsed
                                           : Visibility::Visible);
    }

    if (path != state->selectedPath) {
        BetterPanelSetStatus(state->status, L"");
        if (auto hint = state->detailsCopyHint.get()) {
            hint.Text(L"Select a box to copy");
        }
        std::wstring previousPath = state->selectedPath;
        if (!previousPath.empty()) {
            BetterPanelStopStateMedia(state, previousPath);
        }
        state->selectedPath = path;
        ++state->favoriteCheckGeneration;
        state->favoriteCheckPending = false;
        state->favoriteStatePath.clear();
        state->favoriteStateKnown = false;
        state->favoritePinned = false;
        BetterPanelApplyFavoriteVisual(state);
        if (!path.empty() && !isDirectory && !isMultiSelection &&
            !isDriveRoot) {
            BetterPanelQueueFavoriteCheck(state, path);
        }
        if (auto driveContent = state->driveContent.get()) {
            driveContent.Children().Clear();
        }
        state->displayedPositionSecond = -1;
        state->displayedDurationSecond = -1;
        state->displayedPlaybackInitialized = false;
        state->textLoading = false;
        state->textLoadedPath.clear();
        state->textOriginal.clear();
        state->textEditable = false;
        state->textDirty = false;
        state->detectedSyntaxLanguage = BetterPanelSyntaxLanguage::PlainText;
        state->insightsLoading = false;
        state->insightsLoadedPath.clear();
        state->hashValue.clear();
        state->metadataLoading = false;
        state->metadataEditing = false;
        state->metadataDirty = false;
        state->metadataLoadedPath.clear();
        state->metadataCopyText.clear();
        state->metadataFocusedEditor = {};
        state->metadataOriginal = {};
        BetterPanelSetMetadataFields(state, {});
        BetterPanelUpdateMetadataControls(state);
        state->archivePreviewLoading = false;
        state->archivePreviewPath.clear();
        state->pdfLoading = false;
        state->pdfDocument = nullptr;
        state->pdfLoadedPath.clear();
        state->pdfPageIndex = 0;
        state->pdfZoom = 1.0;
        if (auto pdfImage = state->pdfImage.get()) {
            pdfImage.Source(nullptr);
        }
        if (auto pdfInfo = state->pdfInfo.get()) {
            pdfInfo.Text(isPdf ? L"Loading PDF…" : L"");
            pdfInfo.Visibility(isPdf ? Visibility::Visible
                                     : Visibility::Collapsed);
        }
        BetterPanelUpdatePdfControls(state);
        state->animatedGif = nullptr;
        state->gifExpanded = false;
        if (auto gifCard = state->gifCard.get()) {
            gifCard.Height(state->gifNormalHeight);
        }
        if (auto gifImage = state->gifImage.get()) {
            gifImage.Source(nullptr);
        }
        if (auto icon = state->fileIcon.get()) {
            icon.Source(nullptr);
        }
        if (auto fallback = state->fileFallbackIcon.get()) {
            fallback.Glyph(isDirectory ? L"\uE8B7" : L"\uE7C3");
            fallback.Visibility(Visibility::Visible);
        }
        if (auto icon = state->openActionIcon.get()) {
            icon.Source(nullptr);
        }
        if (auto fallback = state->openActionFallbackIcon.get()) {
            fallback.Visibility(Visibility::Visible);
        }
        // Explorer can leave its previous thumbnail in place when the new file
        // type has no registered preview. Clear it as soon as the selection
        // changes; Explorer can still supply a fresh thumbnail afterward.
        if (auto previewImage = state->nativePreviewImage.get()) {
            previewImage.Source(nullptr);
        }
        if (auto title = state->audioTitle.get()) {
            title.Text(isAudio ? BetterPanelFileName(path) : L"");
        }
        if (auto title = state->fileTitle.get()) {
            title.Text(BetterPanelFileName(path));
        }
        if (auto player = state->videoPlayer.get()) {
            player.Source(nullptr);
        }
        if (auto artwork = state->artwork.get()) {
            artwork.Source(nullptr);
        }
        if (auto placeholder = state->artworkPlaceholder.get()) {
            placeholder.Visibility(Visibility::Visible);
        }
        if (isAudio) {
            BetterPanelLoadArtwork(state, path);
            BetterPanelLoadAudioMetadata(
                std::weak_ptr<BetterPanelState>(state), path);
        }
        if (!path.empty() && !isMultiSelection) {
            BetterPanelLoadFileIcon(state, path);
            BetterPanelLoadOpenActionIcon(state, path);
            BetterPanelLoadInsights(state, path);
        }
        if (isVideo) {
            BetterPanelLoadVideo(state, path);
        }
        if (isGif) {
            BetterPanelLoadAnimatedGif(state, path);
        }
        if (isText) {
            BetterPanelLoadTextPreview(state, path);
        }
        if (isPdf) {
            BetterPanelLoadPdf(state, path);
        }
        if (isArchive) {
            BetterPanelLoadArchivePreview(state, path);
        }
    } else if (isGif && state->animatedGif) {
        if (auto gifImage = state->gifImage.get();
            gifImage && gifImage.Source() != state->animatedGif) {
            gifImage.Source(state->animatedGif);
        }
        if (state->animatedGif.IsAnimatedBitmap() &&
            !state->animatedGif.IsPlaying()) {
            state->animatedGif.Play();
        }
    }

    if (auto panel = state->panel.get()) {
        BetterPanelPruneButtonLayoutSnapshots(state);
        BetterPanelApplyNativeButtonStyle(state, panel);
    }
    BetterPanelRefreshPlaybackState(state);
}

void BetterPanelInstallDetailPanel(FrameworkElement element) {
    if (element.Name() != L"DetailsViewThumbnail") {
        return;
    }

    auto host = element.try_as<muxc::StackPanel>();
    if (!host) {
        return;
    }

    for (auto const& child : host.Children()) {
        if (auto frameworkElement = child.try_as<FrameworkElement>();
            frameworkElement &&
            frameworkElement.Name() == L"BetterDetailPanelRoot") {
            return;
        }
    }

    auto state = std::make_shared<BetterPanelState>();
    std::weak_ptr<BetterPanelState> weakState = state;
    state->dispatcher = host.DispatcherQueue();
    state->explorerWindow = BetterPanelGetExplorerWindowForElement(host);
    Wh_Log(L"Installed panel host=%p explorer=%p thread=%u",
           winrt::get_abi(host), state->explorerWindow, GetCurrentThreadId());
    state->host = winrt::make_weak(host);

    muxc::StackPanel panel;
    panel.Name(L"BetterDetailPanelRoot");
    panel.Margin(Thickness{16, 12, 16, 0});
    panel.Spacing(10);
    state->panel = winrt::make_weak(panel.as<FrameworkElement>());

    muxc::StackPanel fileTitleRow;
    fileTitleRow.Name(L"BetterDetailPanelFileTitleRow");
    fileTitleRow.Orientation(muxc::Orientation::Horizontal);
    fileTitleRow.Spacing(6);
    fileTitleRow.Margin(Thickness{0, 4, 0, 2});
    fileTitleRow.HorizontalAlignment(HorizontalAlignment::Left);
    state->fileTitleRow =
        winrt::make_weak(fileTitleRow.as<FrameworkElement>());

    muxc::Grid fileIconHost;
    fileIconHost.Width(20);
    fileIconHost.Height(20);
    fileIconHost.VerticalAlignment(VerticalAlignment::Center);

    muxc::FontIcon fileFallbackIcon;
    fileFallbackIcon.Glyph(L"\uE7C3");
    fileFallbackIcon.FontSize(16);
    state->fileFallbackIcon = winrt::make_weak(fileFallbackIcon);
    fileIconHost.Children().Append(fileFallbackIcon);

    muxc::Image fileIcon;
    fileIcon.Width(20);
    fileIcon.Height(20);
    fileIcon.Stretch(winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
    fileIcon.VerticalAlignment(VerticalAlignment::Center);
    state->fileIcon = winrt::make_weak(fileIcon);
    fileIconHost.Children().Append(fileIcon);
    fileTitleRow.Children().Append(fileIconHost);

    muxc::TextBlock fileTitle;
    fileTitle.TextTrimming(TextTrimming::CharacterEllipsis);
    fileTitle.MaxWidth(host.ActualWidth() > 160 ? host.ActualWidth() - 90
                                                : 360);
    fileTitle.FontWeight(
        winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    fileTitle.VerticalAlignment(VerticalAlignment::Center);
    state->fileTitle = winrt::make_weak(fileTitle);

    auto fileTitleButton = BetterPanelMakeButton(L"");
    fileTitleButton.Content(fileTitle);
    fileTitleButton.MinHeight(28);
    fileTitleButton.Padding(Thickness{4, 2, 4, 2});
    fileTitleButton.Margin(Thickness{-4, 0, 0, 0});
    fileTitleButton.HorizontalAlignment(HorizontalAlignment::Left);
    fileTitleButton.Background(
        winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(
            winrt::Windows::UI::Colors::Transparent()));
    fileTitleButton.BorderThickness(Thickness{0});
    fileTitleButton.CornerRadius(CornerRadius{4});
    muxa::AutomationProperties::SetName(fileTitleButton,
                                        L"Rename selected file");
    muxc::ToolTipService::SetToolTip(fileTitleButton,
                                    winrt::box_value(L"Click to rename"));
    state->fileRenameButton = winrt::make_weak(fileTitleButton);
    fileTitleRow.Children().Append(fileTitleButton);

    muxc::TextBox fileRenameBox;
    fileRenameBox.Visibility(Visibility::Collapsed);
    fileRenameBox.VerticalAlignment(VerticalAlignment::Center);
    fileRenameBox.HorizontalAlignment(HorizontalAlignment::Left);
    fileRenameBox.Width(240);
    fileRenameBox.Height(34);
    fileRenameBox.MinHeight(34);
    fileRenameBox.MaxWidth(host.ActualWidth() > 160 ? host.ActualWidth() - 40
                                                    : 320);
    state->fileRenameBox = winrt::make_weak(fileRenameBox);
    fileTitleRow.Children().Append(fileRenameBox);
    std::weak_ptr<BetterPanelState> weakRenameState = state;
    fileTitleButton.Click(
        [weakRenameState](winrt::Windows::Foundation::IInspectable const&,
                          RoutedEventArgs const&) {
            if (auto state = weakRenameState.lock()) {
                BetterPanelBeginRename(state, false);
            }
        });
    fileRenameBox.KeyDown(
        [weakRenameState](
            winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args) {
            auto state = weakRenameState.lock();
            if (!state) return;
            if (args.Key() == winrt::Windows::System::VirtualKey::Enter) {
                args.Handled(true);
                BetterPanelEndRename(state, false, true);
            } else if (args.Key() ==
                       winrt::Windows::System::VirtualKey::Escape) {
                args.Handled(true);
                BetterPanelEndRename(state, false, false);
            }
        });

    muxc::TextBlock status;
    status.Name(L"BetterPanelTemporaryStatus");
    status.Text(L"");
    status.TextWrapping(TextWrapping::NoWrap);
    status.TextTrimming(TextTrimming::CharacterEllipsis);
    status.MaxLines(1);
    status.Height(18);
    status.FontSize(11);
    status.Opacity(0.70);
    status.Margin(Thickness{4, 0, 4, 0});
    status.HorizontalAlignment(HorizontalAlignment::Left);
    status.VerticalAlignment(VerticalAlignment::Center);
    status.Visibility(Visibility::Visible);
    state->status = winrt::make_weak(status);
    auto weakStatus = winrt::make_weak(status);

    muxc::StackPanel transferRow;
    transferRow.Name(L"BetterDetailPanelTransferActions");
    transferRow.Orientation(muxc::Orientation::Vertical);
    transferRow.Spacing(6);
    transferRow.Margin(Thickness{0, 6, 0, 4});
    transferRow.HorizontalAlignment(HorizontalAlignment::Stretch);
    transferRow.Visibility(Visibility::Collapsed);
    state->transferRow =
        winrt::make_weak(transferRow.as<FrameworkElement>());

    auto transferButtons = BetterPanelMakeRow();
    transferButtons.HorizontalAlignment(HorizontalAlignment::Left);

    auto transferExtractButton =
        BetterPanelMakeIconButton(L"Extract here", L"\uE8B7");
    state->transferExtractButton = winrt::make_weak(transferExtractButton);
    transferExtractButton.Click(
        [weakState, weakStatus](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            auto context = BetterPanelResolveTransferContext(
                state->selectedPath, state->transferSources);
            if (context.sources.empty() || context.destination.empty()) {
                BetterPanelSetStatus(weakStatus,
                                     L"Open a destination folder in another tab");
                return;
            }
            bool started = false;
            for (auto const& source : context.sources) {
                if (!BetterPanelIsArchiveFile(source)) continue;
                if (FAILED(BetterPanelStartExtraction(source,
                                                      context.destination))) {
                    BetterPanelSetStatus(weakStatus,
                                         L"Could not start extraction");
                    return;
                }
                started = true;
            }
            BetterPanelSetStatus(weakStatus,
                                 started ? L"Extracting to the open folder"
                                         : L"Select one or more archives");
        });
    transferButtons.Children().Append(transferExtractButton);

    auto transferMoveButton =
        BetterPanelMakeIconButton(L"Move to", L"\uE8DE");
    if (auto content = transferMoveButton.Content().try_as<muxc::StackPanel>();
        content && content.Children().Size() > 1) {
        if (auto text = content.Children().GetAt(1).try_as<muxc::TextBlock>()) {
            text.MaxWidth(280);
            text.TextWrapping(TextWrapping::NoWrap);
            text.TextTrimming(TextTrimming::CharacterEllipsis);
        }
    }
    state->transferMoveButton = winrt::make_weak(transferMoveButton);
    transferMoveButton.Click(
        [weakState, weakStatus](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            auto context = BetterPanelResolveTransferContext(
                state->selectedPath, state->transferSources);
            if (context.sources.empty() || context.destination.empty()) {
                BetterPanelSetStatus(weakStatus,
                                     L"Open a destination folder in another tab");
                return;
            }
            if (state->timer) state->timer.Stop();
            bool queued = state->dispatcher.TryEnqueue(
                [weakState, weakStatus, sources = std::move(context.sources),
                 destination = std::move(context.destination)]() {
                    HRESULT result = BetterPanelMoveItemsToFolder(
                        sources, destination);
                    if (auto state = weakState.lock();
                        state && !state->unloaded) {
                        BetterPanelSetStatus(
                            weakStatus, SUCCEEDED(result) ? L"Move completed"
                                                          : L"Move failed");
                        if (state->timer) state->timer.Start();
                    }
                });
            if (!queued && state->timer) state->timer.Start();
        });
    transferButtons.Children().Append(transferMoveButton);
    transferRow.Children().Append(transferButtons);

    auto transferItemsToggle = BetterPanelMakeButton(L"Items being moved  \u25b8");
    transferItemsToggle.HorizontalAlignment(HorizontalAlignment::Stretch);
    transferItemsToggle.HorizontalContentAlignment(HorizontalAlignment::Left);
    transferItemsToggle.Padding(Thickness{8, 4, 8, 4});
    transferItemsToggle.FontSize(11);
    state->transferItemsToggle = winrt::make_weak(transferItemsToggle);
    transferRow.Children().Append(transferItemsToggle);

    muxc::Border transferItemsCard;
    transferItemsCard.Padding(Thickness{10, 8, 10, 8});
    transferItemsCard.CornerRadius(CornerRadius{6});
    transferItemsCard.Background(
        winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(
            winrt::Windows::UI::ColorHelper::FromArgb(18, 128, 128, 128)));
    transferItemsCard.HorizontalAlignment(HorizontalAlignment::Stretch);
    transferItemsCard.Visibility(Visibility::Collapsed);
    state->transferItemsCard =
        winrt::make_weak(transferItemsCard.as<FrameworkElement>());

    muxc::ScrollViewer transferItemsScroll;
    transferItemsScroll.MaxHeight(160);
    transferItemsScroll.VerticalScrollBarVisibility(
        muxc::ScrollBarVisibility::Auto);
    transferItemsScroll.HorizontalScrollBarVisibility(
        muxc::ScrollBarVisibility::Disabled);
    muxc::TextBlock transferItemsText;
    transferItemsText.FontSize(11);
    transferItemsText.TextWrapping(TextWrapping::Wrap);
    transferItemsText.IsTextSelectionEnabled(true);
    state->transferItemsText = winrt::make_weak(transferItemsText);
    transferItemsScroll.Content(transferItemsText);
    transferItemsCard.Child(transferItemsScroll);
    transferRow.Children().Append(transferItemsCard);

    transferItemsToggle.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            state->transferItemsExpanded = !state->transferItemsExpanded;
            if (auto card = state->transferItemsCard.get()) {
                card.Visibility(state->transferItemsExpanded
                                    ? Visibility::Visible
                                    : Visibility::Collapsed);
            }
            if (auto toggle = state->transferItemsToggle.get()) {
                std::wstring label = state->transferSources.size() == 1
                    ? L"Item being moved"
                    : L"Items being moved (" +
                          std::to_wstring(state->transferSources.size()) +
                          L")";
                label += state->transferItemsExpanded ? L"  \u25be"
                                                      : L"  \u25b8";
                toggle.Content(winrt::box_value(label));
            }
        });

    muxc::StackPanel multiActionRow;
    multiActionRow.Name(L"BetterDetailPanelMultiActions");
    multiActionRow.Orientation(muxc::Orientation::Vertical);
    multiActionRow.Spacing(6);
    multiActionRow.Margin(Thickness{0, 6, 0, 4});
    multiActionRow.HorizontalAlignment(HorizontalAlignment::Stretch);
    multiActionRow.Visibility(Visibility::Collapsed);
    state->multiActionRow =
        winrt::make_weak(multiActionRow.as<FrameworkElement>());

    muxc::TextBlock multiSelectionText;
    multiSelectionText.FontSize(11);
    multiSelectionText.Opacity(0.70);
    multiSelectionText.TextWrapping(TextWrapping::Wrap);
    multiSelectionText.VerticalAlignment(VerticalAlignment::Center);
    state->multiSelectionText = winrt::make_weak(multiSelectionText);
    multiActionRow.Children().Append(multiSelectionText);

    muxc::Grid multiButtons;
    multiButtons.HorizontalAlignment(HorizontalAlignment::Stretch);
    muxc::ColumnDefinition multiPrimaryColumn;
    multiPrimaryColumn.Width(GridLength{1, GridUnitType::Star});
    muxc::ColumnDefinition multiDeleteColumn;
    multiDeleteColumn.Width(GridLength{1, GridUnitType::Auto});
    multiButtons.ColumnDefinitions().Append(multiPrimaryColumn);
    multiButtons.ColumnDefinitions().Append(multiDeleteColumn);

    auto multiPrimaryButtons = BetterPanelMakeRow();
    multiPrimaryButtons.HorizontalAlignment(HorizontalAlignment::Left);
    multiButtons.Children().Append(multiPrimaryButtons);

    auto multiMoveButton =
        BetterPanelMakeIconButton(L"Move to...", L"\uE8DE");
    multiMoveButton.Click(
        [weakState, weakStatus](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            auto sources = BetterPanelGetActiveSelectionPaths(false);
            if (sources.size() < 2) return;
            std::wstring destination;
            HRESULT result = BetterPanelChooseExtractionFolder(
                GetForegroundWindow(), destination);
            if (FAILED(result) || destination.empty()) return;
            if (state->timer) state->timer.Stop();
            bool queued = state->dispatcher.TryEnqueue(
                [weakState, weakStatus, sources = std::move(sources),
                 destination = std::move(destination)]() {
                    HRESULT result = BetterPanelMoveItemsToFolder(
                        sources, destination);
                    if (auto state = weakState.lock();
                        state && !state->unloaded) {
                        BetterPanelSetStatus(
                            weakStatus, SUCCEEDED(result) ? L"Move completed"
                                                          : L"Move failed");
                        if (state->timer) state->timer.Start();
                    }
                });
            if (!queued && state->timer) state->timer.Start();
        });
    multiPrimaryButtons.Children().Append(multiMoveButton);

    auto multiDeleteButton = BetterPanelMakeButton(L"");
    muxc::FontIcon multiDeleteIcon;
    multiDeleteIcon.Glyph(L"\uE74D");
    multiDeleteIcon.FontSize(13);
    multiDeleteButton.Content(multiDeleteIcon);
    multiDeleteButton.Width(32);
    multiDeleteButton.Height(32);
    multiDeleteButton.MinWidth(32);
    multiDeleteButton.MinHeight(32);
    multiDeleteButton.Padding(Thickness{0, 0, 0, 0});
    multiDeleteButton.HorizontalAlignment(HorizontalAlignment::Right);
    muxc::Grid::SetColumn(multiDeleteButton, 1);
    muxa::AutomationProperties::SetName(multiDeleteButton,
                                        L"Move selected items to Recycle Bin");
    muxc::ToolTipService::SetToolTip(
        multiDeleteButton, winrt::box_value(L"Move to Recycle Bin"));
    multiDeleteButton.Click(
        [weakState, weakStatus](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            auto sources = BetterPanelGetActiveSelectionPaths(false);
            if (sources.size() < 2) return;
            if (state->timer) state->timer.Stop();
            bool queued = state->dispatcher.TryEnqueue(
                [weakState, weakStatus, sources = std::move(sources)]() {
                    HRESULT result = BetterPanelDeleteItems(sources);
                    if (auto state = weakState.lock();
                        state && !state->unloaded) {
                        BetterPanelSetStatus(
                            weakStatus, SUCCEEDED(result) ? L"Delete completed"
                                                          : L"Delete cancelled");
                        if (state->timer) state->timer.Start();
                    }
                });
            if (!queued && state->timer) state->timer.Start();
        });
    multiButtons.Children().Append(multiDeleteButton);

    auto multiArchiveButton =
        BetterPanelMakeIconButton(L"Archive", L"\uE7B8");
    multiArchiveButton.Click(
        [weakState, weakStatus](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            auto sources = BetterPanelGetActiveSelectionPaths(false);
            if (sources.size() < 2) return;
            std::wstring creator = BetterPanelFindArchiveCreator();
            if (creator.empty()) {
                BetterPanelSetStatus(weakStatus,
                                     L"No archive creator is available");
                return;
            }
            std::wstring archivePath;
            HRESULT result = BetterPanelChooseArchivePath(
                GetForegroundWindow(), creator, archivePath);
            if (FAILED(result) || archivePath.empty()) return;
            result = BetterPanelStartArchive(sources, creator, archivePath);
            BetterPanelSetStatus(
                weakStatus, SUCCEEDED(result) ? L"Archive creation started"
                                              : L"Archive creation failed");
        });
    multiPrimaryButtons.Children().Append(multiArchiveButton);
    multiActionRow.Children().Append(multiButtons);

    muxc::StackPanel panelUtilities;
    panelUtilities.Spacing(5);
    state->panelUtilities =
        winrt::make_weak(panelUtilities.as<FrameworkElement>());

    muxc::Grid panelControls;
    muxc::ColumnDefinition utilityColumn;
    utilityColumn.Width(GridLength{1, GridUnitType::Auto});
    muxc::ColumnDefinition utilitySpacer;
    utilitySpacer.Width(GridLength{1, GridUnitType::Star});
    muxc::ColumnDefinition quickAudioColumn;
    quickAudioColumn.Width(GridLength{1, GridUnitType::Auto});
    panelControls.ColumnDefinitions().Append(utilityColumn);
    panelControls.ColumnDefinitions().Append(utilitySpacer);
    panelControls.ColumnDefinitions().Append(quickAudioColumn);

    auto utilityButtons = BetterPanelMakeRow();
    utilityButtons.HorizontalAlignment(HorizontalAlignment::Left);

    auto previewToggle = BetterPanelMakeButton(L"");
    muxc::FontIcon previewToggleIcon;
    previewToggleIcon.Glyph(L"\uE890");
    previewToggleIcon.FontSize(14);
    previewToggle.Content(previewToggleIcon);
    previewToggle.Width(32);
    previewToggle.Height(32);
    previewToggle.Padding(Thickness{0, 0, 0, 0});
    muxa::AutomationProperties::SetName(previewToggle, L"Hide preview");
    muxc::ToolTipService::SetToolTip(previewToggle,
                                    winrt::box_value(L"Show or hide preview"));
    state->previewToggleButton = winrt::make_weak(previewToggle);
    previewToggle.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            state->previewsCollapsed = !state->previewsCollapsed;
            if (auto button = state->previewToggleButton.get()) {
                if (auto icon = button.Content().try_as<muxc::FontIcon>()) {
                    icon.Glyph(state->previewsCollapsed ? L"\uED1A"
                                                        : L"\uE890");
                }
                muxa::AutomationProperties::SetName(
                    button, state->previewsCollapsed ? L"Show preview"
                                                     : L"Hide preview");
            }
            BetterPanelRefresh(state);
        });
    utilityButtons.Children().Append(previewToggle);

    panelControls.Children().Append(utilityButtons);

    auto rightUtilityActions = BetterPanelMakeRow();
    rightUtilityActions.HorizontalAlignment(HorizontalAlignment::Right);
    muxc::Grid::SetColumn(rightUtilityActions, 2);

    auto favoriteButton = BetterPanelMakeButton(L"");
    muxc::FontIcon favoriteIcon;
    favoriteIcon.Glyph(L"\uE734");
    favoriteIcon.FontSize(14);
    favoriteButton.Content(favoriteIcon);
    favoriteButton.Width(32);
    favoriteButton.Height(32);
    favoriteButton.MinWidth(32);
    favoriteButton.MinHeight(32);
    favoriteButton.Padding(Thickness{0, 0, 0, 0});
    favoriteButton.Visibility(Visibility::Collapsed);
    muxa::AutomationProperties::SetName(favoriteButton,
                                        L"Add to Favorites");
    muxc::ToolTipService::SetToolTip(
        favoriteButton, winrt::box_value(L"Add to Favorites"));
    state->favoriteButton = winrt::make_weak(favoriteButton);
    favoriteButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state || state->selectedPath.empty()) return;
            BetterPanelRunFavoriteAction(
                state, state->selectedPath,
                state->favoriteStateKnown && state->favoritePinned);
        });
    rightUtilityActions.Children().Append(favoriteButton);

    auto printButton = BetterPanelMakeButton(L"");
    muxc::FontIcon printIcon;
    printIcon.Glyph(L"\uE749");
    printIcon.FontSize(13);
    printButton.Content(printIcon);
    printButton.Width(32);
    printButton.Height(32);
    printButton.MinHeight(32);
    printButton.Padding(Thickness{0, 0, 0, 0});
    printButton.Visibility(Visibility::Collapsed);
    muxa::AutomationProperties::SetName(printButton, L"Print selected file");
    muxc::ToolTipService::SetToolTip(printButton,
                                    winrt::box_value(L"Print"));
    state->printButton = winrt::make_weak(printButton);
    auto weakPrintStatus = state->status;
    printButton.Click(
        [weakPrintStatus](winrt::Windows::Foundation::IInspectable const&,
                          RoutedEventArgs const&) {
            auto path = BetterPanelGetSelectedPath();
            if (path.empty()) {
                BetterPanelSetStatus(weakPrintStatus,
                                     L"Select one file first");
                return;
            }
            BetterPanelSetStatus(
                weakPrintStatus,
                BetterPanelPrintFile(path)
                    ? L"Print opened"
                    : L"Printing is unavailable for this file type");
        });
    rightUtilityActions.Children().Append(printButton);

    auto quickAudioControls = BetterPanelMakeRow();
    quickAudioControls.Visibility(Visibility::Collapsed);
    state->quickAudioControls =
        winrt::make_weak(quickAudioControls.as<FrameworkElement>());

    muxc::TextBlock quickAudioTitle;
    quickAudioTitle.MaxWidth(180);
    quickAudioTitle.TextTrimming(TextTrimming::CharacterEllipsis);
    quickAudioTitle.FontWeight(
        winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    quickAudioTitle.VerticalAlignment(VerticalAlignment::Center);
    state->quickAudioTitle = winrt::make_weak(quickAudioTitle);
    quickAudioControls.Children().Append(quickAudioTitle);

    auto quickPlay = BetterPanelMakeButton(L"");
    muxc::FontIcon quickPlayIcon;
    quickPlayIcon.Glyph(L"\uE768");
    quickPlayIcon.FontSize(13);
    quickPlay.Content(quickPlayIcon);
    muxa::AutomationProperties::SetName(quickPlay, L"Play");
    muxc::ToolTipService::SetToolTip(quickPlay, winrt::box_value(L"Play"));
    quickPlay.Width(38);
    quickPlay.Height(32);
    quickPlay.Padding(Thickness{4, 2, 4, 2});
    state->quickAudioPlayButton = winrt::make_weak(quickPlay);
    quickPlay.Click([weakState](auto const&, RoutedEventArgs const&) {
        if (auto state = weakState.lock()) {
            BetterPanelTogglePlayback(state->selectedPath, state->status);
        }
    });
    quickAudioControls.Children().Append(quickPlay);

    rightUtilityActions.Children().Append(quickAudioControls);
    panelControls.Children().Append(rightUtilityActions);

    panelUtilities.Children().Append(panelControls);

    for (auto const& child : host.Children()) {
        auto thumbnail = child.try_as<muxc::Grid>();
        if (!thumbnail) {
            continue;
        }
        state->nativePreview = winrt::make_weak(thumbnail);
        if (auto previewImage = BetterPanelFindPreviewImage(thumbnail)) {
            state->nativePreviewImage = winrt::make_weak(previewImage);
        }

        auto expandButton = BetterPanelMakeButton(L"");
        muxc::FontIcon expandIcon;
        expandIcon.Glyph(L"\uE740");
        expandIcon.FontSize(16);
        expandButton.Content(expandIcon);
        expandButton.Width(36);
        expandButton.Height(36);
        expandButton.MinWidth(36);
        expandButton.MinHeight(36);
        expandButton.Padding(Thickness{0, 0, 0, 0});
        expandButton.HorizontalAlignment(HorizontalAlignment::Right);
        expandButton.VerticalAlignment(VerticalAlignment::Bottom);
        expandButton.Margin(Thickness{0, 0, 10, 10});
        muxc::Canvas::SetZIndex(expandButton, 100);
        state->previewExpandButton = winrt::make_weak(expandButton);
        std::weak_ptr<BetterPanelState> weakPreviewState = state;
        auto weakExpandIcon = winrt::make_weak(expandIcon);
        expandButton.Click(
            [weakPreviewState, weakExpandIcon, weakStatus](
                winrt::Windows::Foundation::IInspectable const&,
                RoutedEventArgs const&) {
                if (auto state = weakPreviewState.lock()) {
                    bool expand = !state->previewExpanded;
                    BetterPanelSetInlinePreviewExpanded(state, expand);
                    if (auto icon = weakExpandIcon.get()) {
                        icon.Glyph(expand ? L"\uE73F" : L"\uE740");
                    }
                    BetterPanelSetStatus(weakStatus, L"");
                }
            });
        thumbnail.Children().Append(expandButton);

        muxc::Grid multiPreviewSurface;
        multiPreviewSurface.Name(L"BetterDetailPanelMultiPreviewSurface");
        double nativeHeight = thumbnail.ActualHeight();
        if (nativeHeight < 120) nativeHeight = 200;
        state->multiPreviewNormalHeight = nativeHeight;
        multiPreviewSurface.Height(nativeHeight);
        multiPreviewSurface.MinHeight(120);
        multiPreviewSurface.HorizontalAlignment(HorizontalAlignment::Stretch);
        multiPreviewSurface.Background(thumbnail.Background());
        multiPreviewSurface.Visibility(Visibility::Collapsed);
        state->multiPreviewSurface =
            winrt::make_weak(multiPreviewSurface.as<FrameworkElement>());

        muxc::Grid multiPreviewHost;
        multiPreviewHost.Name(L"BetterDetailPanelMultiPreview");
        multiPreviewHost.HorizontalAlignment(HorizontalAlignment::Center);
        multiPreviewHost.VerticalAlignment(VerticalAlignment::Center);
        multiPreviewHost.IsHitTestVisible(false);
        state->multiPreviewHost = winrt::make_weak(multiPreviewHost);
        multiPreviewSurface.Children().Append(multiPreviewHost);

        auto multiExpandButton = BetterPanelMakeButton(L"");
        muxc::FontIcon multiExpandIcon;
        multiExpandIcon.Glyph(L"\uE740");
        multiExpandIcon.FontSize(16);
        multiExpandButton.Content(multiExpandIcon);
        multiExpandButton.Width(36);
        multiExpandButton.Height(36);
        multiExpandButton.MinWidth(36);
        multiExpandButton.MinHeight(36);
        multiExpandButton.Padding(Thickness{0});
        multiExpandButton.HorizontalAlignment(HorizontalAlignment::Right);
        multiExpandButton.VerticalAlignment(VerticalAlignment::Bottom);
        multiExpandButton.Margin(Thickness{0, 0, 10, 10});
        muxc::Canvas::SetZIndex(multiExpandButton, 100);
        muxa::AutomationProperties::SetName(multiExpandButton,
                                            L"Expand preview");
        state->multiPreviewExpandButton =
            winrt::make_weak(multiExpandButton);
        auto weakMultiExpandIcon = winrt::make_weak(multiExpandIcon);
        multiExpandButton.Click(
            [weakState, weakMultiExpandIcon](auto const&,
                                             RoutedEventArgs const&) {
                auto state = weakState.lock();
                auto surface = state ? state->multiPreviewSurface.get()
                                     : nullptr;
                if (!state || !surface) return;
                state->multiPreviewExpanded = !state->multiPreviewExpanded;
                surface.Height(state->multiPreviewExpanded
                                   ? std::clamp(
                                         state->multiPreviewNormalHeight * 1.65,
                                         340.0, 520.0)
                                   : state->multiPreviewNormalHeight);
                if (auto icon = weakMultiExpandIcon.get()) {
                    icon.Glyph(state->multiPreviewExpanded ? L"\uE73F"
                                                           : L"\uE740");
                }
            });
        multiPreviewSurface.Children().Append(multiExpandButton);

        uint32_t thumbnailIndex = 0;
        if (host.Children().IndexOf(thumbnail.as<UIElement>(),
                                    thumbnailIndex)) {
            host.Children().InsertAt(thumbnailIndex + 1,
                                     multiPreviewSurface);
        }
        break;
    }

    muxc::Grid gifCard;
    gifCard.Name(L"BetterDetailPanelGifCard");
    gifCard.Margin(Thickness{0, 4, 0, 0});
    gifCard.Height(state->gifNormalHeight);
    gifCard.HorizontalAlignment(HorizontalAlignment::Stretch);
    gifCard.Visibility(Visibility::Collapsed);
    state->gifCard = winrt::make_weak(gifCard.as<FrameworkElement>());

    muxc::Image gifImage;
    gifImage.Stretch(winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
    gifImage.HorizontalAlignment(HorizontalAlignment::Stretch);
    gifImage.VerticalAlignment(VerticalAlignment::Stretch);
    state->gifImage = winrt::make_weak(gifImage);
    gifCard.Children().Append(gifImage);

    auto gifExpandButton = BetterPanelMakeButton(L"");
    muxc::FontIcon gifExpandIcon;
    gifExpandIcon.Glyph(L"\uE740");
    gifExpandIcon.FontSize(16);
    gifExpandButton.Content(gifExpandIcon);
    gifExpandButton.Width(36);
    gifExpandButton.Height(36);
    gifExpandButton.MinWidth(36);
    gifExpandButton.MinHeight(36);
    gifExpandButton.Padding(Thickness{0, 0, 0, 0});
    gifExpandButton.HorizontalAlignment(HorizontalAlignment::Right);
    gifExpandButton.VerticalAlignment(VerticalAlignment::Bottom);
    gifExpandButton.Margin(Thickness{0, 0, 10, 10});
    muxc::Canvas::SetZIndex(gifExpandButton, 100);
    state->gifExpandButton = winrt::make_weak(gifExpandButton);
    auto weakGifExpandIcon = winrt::make_weak(gifExpandIcon);
    gifExpandButton.Click(
        [weakState, weakGifExpandIcon](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&) {
            auto state = weakState.lock();
            auto card = state ? state->gifCard.get() : nullptr;
            if (!state || !card) {
                return;
            }
            state->gifExpanded = !state->gifExpanded;
            card.Height(state->gifExpanded
                            ? std::clamp(state->gifNormalHeight * 1.65,
                                         360.0, 560.0)
                            : state->gifNormalHeight);
            if (auto icon = weakGifExpandIcon.get()) {
                icon.Glyph(state->gifExpanded ? L"\uE73F" : L"\uE740");
            }
        });
    gifCard.Children().Append(gifExpandButton);
    panel.Children().Append(gifCard);

    muxc::Border videoCard;
    videoCard.Name(L"BetterDetailPanelVideoCard");
    videoCard.Margin(Thickness{0, 4, 0, 0});
    videoCard.CornerRadius(CornerRadius{8});
    videoCard.HorizontalAlignment(HorizontalAlignment::Center);
    videoCard.Visibility(Visibility::Collapsed);
    state->videoCard = winrt::make_weak(videoCard.as<FrameworkElement>());

    muxc::MediaPlayerElement videoPlayer;
    videoPlayer.AreTransportControlsEnabled(true);
    videoPlayer.AutoPlay(false);
    videoPlayer.Stretch(
        winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
    // Keep the decoder's rendering surface stable. The Viewbox below scales
    // this surface through XAML composition without forcing the active decoder
    // to recreate it for every pixel of a Details-pane resize.
    videoPlayer.Width(720);
    videoPlayer.Height(346);
    videoPlayer.HorizontalAlignment(HorizontalAlignment::Center);
    auto videoControls = videoPlayer.TransportControls();
    videoControls.IsCompact(true);
    videoControls.ShowAndHideAutomatically(true);
    state->videoPlayer = winrt::make_weak(videoPlayer);

    muxc::Viewbox videoViewport;
    videoViewport.Width(420);
    videoViewport.Height(202);
    videoViewport.HorizontalAlignment(HorizontalAlignment::Center);
    videoViewport.Stretch(
        winrt::Microsoft::UI::Xaml::Media::Stretch::Fill);
    videoViewport.Child(videoPlayer);
    state->videoViewport = winrt::make_weak(videoViewport);

    DispatcherTimer videoControlsTimer;
    videoControlsTimer.Interval(
        winrt::Windows::Foundation::TimeSpan{13000000});
    videoControlsTimer.Tick([weakState](
                                winrt::Windows::Foundation::IInspectable const&,
                                winrt::Windows::Foundation::IInspectable const&) {
        if (auto state = weakState.lock()) {
            if (auto player = state->videoPlayer.get()) {
                player.TransportControls().Hide();
            }
            if (state->videoControlsTimer) {
                state->videoControlsTimer.Stop();
            }
        }
    });
    state->videoControlsTimer = videoControlsTimer;

    videoPlayer.PointerMoved(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const&) {
            if (auto state = weakState.lock()) {
                if (auto player = state->videoPlayer.get()) {
                    player.TransportControls().Show();
                }
                if (state->videoControlsTimer) {
                    state->videoControlsTimer.Stop();
                    state->videoControlsTimer.Start();
                }
            }
        });
    videoPlayer.PointerExited(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const&) {
            if (auto state = weakState.lock(); state && state->videoControlsTimer) {
                state->videoControlsTimer.Stop();
                state->videoControlsTimer.Start();
            }
        });
    host.SizeChanged(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    SizeChangedEventArgs const& args) {
            if (auto state = weakState.lock()) {
                if (auto viewport = state->videoViewport.get()) {
                    double availableWidth =
                        std::max(120.0, static_cast<double>(args.NewSize().Width));
                    double viewportWidth = std::min(availableWidth, 960.0);
                    viewport.Width(viewportWidth);
                    viewport.Height(
                        std::clamp(viewportWidth * 0.48, 190.0, 460.0));
                }
                state->gifNormalHeight =
                    std::clamp(static_cast<double>(args.NewSize().Width) * 0.62,
                               220.0, 380.0);
                if (!state->gifExpanded) {
                    if (auto gifCard = state->gifCard.get()) {
                        gifCard.Height(state->gifNormalHeight);
                    }
                }
                state->pdfBaseWidth = std::clamp(
                    static_cast<double>(args.NewSize().Width) - 52.0,
                    260.0, 680.0);
                if (auto pdfImage = state->pdfImage.get()) {
                    pdfImage.Width(state->pdfBaseWidth * state->pdfZoom);
                }
            }
        });
    videoCard.Child(videoViewport);
    panel.Children().Append(videoCard);

    muxc::Border audioCard;
    audioCard.Name(L"BetterDetailPanelAudioCard");
    audioCard.Margin(Thickness{0, 4, 0, 0});
    audioCard.Padding(Thickness{14, 12, 14, 12});
    audioCard.CornerRadius(CornerRadius{8});
    audioCard.Visibility(Visibility::Collapsed);
    state->audioCard = winrt::make_weak(audioCard.as<FrameworkElement>());

    muxc::StackPanel audioContent;
    audioContent.Spacing(9);

    muxc::Grid artworkFrame;
    artworkFrame.Width(132);
    artworkFrame.Height(132);
    artworkFrame.HorizontalAlignment(HorizontalAlignment::Center);
    state->audioArtworkFrame =
        winrt::make_weak(artworkFrame.as<FrameworkElement>());

    muxc::TextBlock placeholder;
    placeholder.Text(L"♫");
    placeholder.FontSize(54);
    placeholder.Opacity(0.45);
    placeholder.HorizontalAlignment(HorizontalAlignment::Center);
    placeholder.VerticalAlignment(VerticalAlignment::Center);
    state->artworkPlaceholder = winrt::make_weak(placeholder);
    artworkFrame.Children().Append(placeholder);

    muxc::Image artwork;
    artwork.Width(132);
    artwork.Height(132);
    artwork.Stretch(winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
    state->artwork = winrt::make_weak(artwork);
    artworkFrame.Children().Append(artwork);
    audioContent.Children().Append(artworkFrame);

    muxc::TextBlock audioTitle;
    audioTitle.TextTrimming(TextTrimming::CharacterEllipsis);
    audioTitle.MaxWidth(host.ActualWidth() > 160 ? host.ActualWidth() - 90
                                                 : 300);
    audioTitle.FontWeight(
        winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    audioTitle.VerticalAlignment(VerticalAlignment::Center);
    audioTitle.HorizontalAlignment(HorizontalAlignment::Center);
    state->audioTitle = winrt::make_weak(audioTitle);
    audioContent.Children().Append(audioTitle);

    auto transport = BetterPanelMakeRow();
    transport.HorizontalAlignment(HorizontalAlignment::Center);

    auto previousButton = BetterPanelMakeButton(L"");
    muxc::FontIcon previousIcon;
    previousIcon.Glyph(L"\uE892");
    previousIcon.FontSize(13);
    previousButton.Content(previousIcon);
    previousButton.Width(38);
    previousButton.Height(32);
    previousButton.Padding(Thickness{4, 2, 4, 2});
    muxa::AutomationProperties::SetName(previousButton, L"Previous audio file");
    muxc::ToolTipService::SetToolTip(previousButton,
                                    winrt::box_value(L"Previous audio file"));
    previousButton.Click([weakState](auto const&, RoutedEventArgs const&) {
        if (auto state = weakState.lock()) BetterPanelPlayAdjacent(state, -1);
    });
    transport.Children().Append(previousButton);

    auto speedButton = BetterPanelMakeButton(L"1×");
    speedButton.MinWidth(46);
    speedButton.Height(32);
    speedButton.Padding(Thickness{6, 2, 6, 2});
    muxa::AutomationProperties::SetName(speedButton, L"Playback speed");
    muxc::ToolTipService::SetToolTip(speedButton,
                                    winrt::box_value(L"Playback speed"));
    state->playbackSpeedButton = winrt::make_weak(speedButton);
    speedButton.Click([weakState](auto const&, RoutedEventArgs const&) {
        static constexpr double rates[] = {1.0, 1.25, 1.5, 2.0, 0.75};
        size_t index = 0;
        for (size_t i = 0; i < std::size(rates); ++i) {
            if (std::abs(rates[i] - g_betterPlaybackRate) < 0.01) {
                index = (i + 1) % std::size(rates);
                break;
            }
        }
        g_betterPlaybackRate = rates[index];
        {
            std::lock_guard lock(g_betterMediaMutex);
            if (g_betterMediaPlayer) {
                try {
                    g_betterMediaPlayer.PlaybackSession().PlaybackRate(
                        g_betterPlaybackRate);
                } catch (...) {}
            }
        }
        if (auto state = weakState.lock()) {
            if (auto button = state->playbackSpeedButton.get()) {
                std::wostringstream label;
                label << g_betterPlaybackRate << L"×";
                button.Content(winrt::box_value(label.str()));
            }
            BetterPanelSetStatus(state->status, L"Playback speed changed");
        }
    });
    transport.Children().Append(speedButton);

    auto repeatButton = BetterPanelMakeButton(L"");
    muxc::FontIcon repeatIcon;
    repeatIcon.Glyph(L"\uE8EE");
    repeatIcon.FontSize(13);
    repeatButton.Content(repeatIcon);
    repeatButton.Width(38);
    repeatButton.Height(32);
    repeatButton.Padding(Thickness{4, 2, 4, 2});
    repeatButton.Opacity(g_betterRepeatEnabled ? 1.0 : 0.55);
    muxa::AutomationProperties::SetName(repeatButton, L"Repeat");
    muxc::ToolTipService::SetToolTip(repeatButton, winrt::box_value(L"Repeat"));
    state->repeatButton = winrt::make_weak(repeatButton);
    repeatButton.Click([weakState](auto const&, RoutedEventArgs const&) {
        g_betterRepeatEnabled = !g_betterRepeatEnabled;
        {
            std::lock_guard lock(g_betterMediaMutex);
            if (g_betterMediaPlayer) {
                try {
                    g_betterMediaPlayer.IsLoopingEnabled(g_betterRepeatEnabled);
                } catch (...) {}
            }
        }
        if (auto state = weakState.lock()) {
            if (auto button = state->repeatButton.get())
                button.Opacity(g_betterRepeatEnabled ? 1.0 : 0.55);
            BetterPanelSetStatus(state->status,
                                 g_betterRepeatEnabled ? L"Repeat on"
                                                       : L"Repeat off");
        }
    });
    transport.Children().Append(repeatButton);

    auto shuffleButton = BetterPanelMakeButton(L"");
    muxc::FontIcon shuffleIcon;
    shuffleIcon.Glyph(L"\uE8B1");
    shuffleIcon.FontSize(13);
    shuffleButton.Content(shuffleIcon);
    shuffleButton.Width(38);
    shuffleButton.Height(32);
    shuffleButton.Padding(Thickness{4, 2, 4, 2});
    shuffleButton.Opacity(g_betterShuffleEnabled ? 1.0 : 0.55);
    muxa::AutomationProperties::SetName(shuffleButton, L"Shuffle");
    muxc::ToolTipService::SetToolTip(shuffleButton,
                                    winrt::box_value(L"Shuffle"));
    state->shuffleButton = winrt::make_weak(shuffleButton);
    shuffleButton.Click([weakState](auto const&, RoutedEventArgs const&) {
        g_betterShuffleEnabled = !g_betterShuffleEnabled;
        if (auto state = weakState.lock()) {
            if (auto button = state->shuffleButton.get())
                button.Opacity(g_betterShuffleEnabled ? 1.0 : 0.55);
            BetterPanelSetStatus(state->status,
                                 g_betterShuffleEnabled ? L"Shuffle on"
                                                        : L"Shuffle off");
        }
    });
    auto playButton = BetterPanelMakeButton(L"");
    muxc::FontIcon playIcon;
    playIcon.Glyph(L"\uE768");
    playIcon.FontSize(13);
    playButton.Content(playIcon);
    muxa::AutomationProperties::SetName(playButton, L"Play");
    muxc::ToolTipService::SetToolTip(playButton, winrt::box_value(L"Play"));
    playButton.Width(38);
    playButton.Height(32);
    playButton.Padding(Thickness{4, 2, 4, 2});
    state->playButton = winrt::make_weak(playButton);
    playButton.Click(
        [weakState, weakStatus](
            winrt::Windows::Foundation::IInspectable const&,
            RoutedEventArgs const&) {
            if (auto state = weakState.lock()) {
                BetterPanelTogglePlayback(state->selectedPath, weakStatus);
            }
        });
    transport.Children().Append(playButton);

    transport.Children().Append(shuffleButton);

    auto nextButton = BetterPanelMakeButton(L"");
    muxc::FontIcon nextIcon;
    nextIcon.Glyph(L"\uE893");
    nextIcon.FontSize(13);
    nextButton.Content(nextIcon);
    nextButton.Width(38);
    nextButton.Height(32);
    nextButton.Padding(Thickness{4, 2, 4, 2});
    muxa::AutomationProperties::SetName(nextButton, L"Next audio file");
    muxc::ToolTipService::SetToolTip(nextButton,
                                    winrt::box_value(L"Next audio file"));
    nextButton.Click([weakState](auto const&, RoutedEventArgs const&) {
        if (auto state = weakState.lock()) BetterPanelPlayAdjacent(state, 1);
    });
    transport.Children().Append(nextButton);
    audioContent.Children().Append(transport);

    muxc::Grid playbackBar;
    playbackBar.HorizontalAlignment(HorizontalAlignment::Stretch);
    muxc::ColumnDefinition seekColumn;
    seekColumn.Width(GridLength{1, GridUnitType::Star});
    muxc::ColumnDefinition volumeIconColumn;
    volumeIconColumn.Width(GridLength{1, GridUnitType::Auto});
    muxc::ColumnDefinition volumeSliderColumn;
    volumeSliderColumn.Width(GridLength{112, GridUnitType::Pixel});
    muxc::ColumnDefinition volumeTextColumn;
    volumeTextColumn.Width(GridLength{1, GridUnitType::Auto});
    playbackBar.ColumnDefinitions().Append(seekColumn);
    playbackBar.ColumnDefinitions().Append(volumeIconColumn);
    playbackBar.ColumnDefinitions().Append(volumeSliderColumn);
    playbackBar.ColumnDefinitions().Append(volumeTextColumn);

    muxc::Slider timeline;
    timeline.Minimum(0);
    timeline.Maximum(1);
    timeline.StepFrequency(1);
    timeline.Margin(Thickness{0, 0, 10, 0});
    timeline.VerticalAlignment(VerticalAlignment::Center);
    state->timeline = winrt::make_weak(timeline);
    timeline.ValueChanged(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    muxp::RangeBaseValueChangedEventArgs const& args) {
            auto state = weakState.lock();
            if (!state || state->updatingTimeline) {
                return;
            }
            try {
                std::lock_guard lock(g_betterMediaMutex);
                if (!g_betterMediaPlayer ||
                    g_betterMediaPath != state->selectedPath) {
                    return;
                }
                auto session = g_betterMediaPlayer.PlaybackSession();
                if (session.CanSeek()) {
                    session.Position(winrt::Windows::Foundation::TimeSpan{
                        static_cast<int64_t>(args.NewValue() * 10000000.0)});
                }
            } catch (...) {
            }
        });
    playbackBar.Children().Append(timeline);

    muxc::TextBlock timeText;
    timeText.Text(L"0:00 / --:--");
    timeText.FontSize(11);
    timeText.Opacity(0.70);
    timeText.TextAlignment(TextAlignment::Center);
    state->timeText = winrt::make_weak(timeText);

    auto muteButton = BetterPanelMakeButton(L"");
    muxc::FontIcon muteIcon;
    muteIcon.Glyph(g_betterMediaMuted ? L"\uE74F" : L"\uE767");
    muteIcon.FontSize(14);
    muteButton.Content(muteIcon);
    muteButton.Width(32);
    muteButton.Height(32);
    muteButton.Padding(Thickness{0, 0, 0, 0});
    muteButton.VerticalAlignment(VerticalAlignment::Center);
    muxa::AutomationProperties::SetName(
        muteButton, g_betterMediaMuted ? L"Unmute" : L"Mute");
    muxc::ToolTipService::SetToolTip(
        muteButton, winrt::box_value(g_betterMediaMuted ? L"Unmute" : L"Mute"));
    state->muteButton = winrt::make_weak(muteButton);
    muxc::Grid::SetColumn(muteButton, 1);
    muteButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            {
                std::lock_guard lock(g_betterMediaMutex);
                g_betterMediaMuted = !g_betterMediaMuted;
                if (g_betterMediaPlayer) {
                    try { g_betterMediaPlayer.IsMuted(g_betterMediaMuted); }
                    catch (...) {}
                }
            }
            if (auto button = state->muteButton.get()) {
                if (auto icon = button.Content().try_as<muxc::FontIcon>()) {
                    icon.Glyph(g_betterMediaMuted ? L"\uE74F" : L"\uE767");
                }
                muxa::AutomationProperties::SetName(
                    button, g_betterMediaMuted ? L"Unmute" : L"Mute");
                muxc::ToolTipService::SetToolTip(
                    button, winrt::box_value(
                                g_betterMediaMuted ? L"Unmute" : L"Mute"));
            }
        });
    playbackBar.Children().Append(muteButton);

    muxc::Slider volumeSlider;
    volumeSlider.Minimum(0);
    volumeSlider.Maximum(100);
    volumeSlider.StepFrequency(1);
    volumeSlider.Value(g_betterMediaVolume * 100.0);
    volumeSlider.Margin(Thickness{6, 0, 6, 0});
    volumeSlider.VerticalAlignment(VerticalAlignment::Center);
    muxc::Grid::SetColumn(volumeSlider, 2);
    muxa::AutomationProperties::SetName(volumeSlider, L"Volume");
    state->volumeSlider = winrt::make_weak(volumeSlider);

    muxc::TextBlock volumeText;
    volumeText.Text(std::to_wstring(
                        static_cast<int>(g_betterMediaVolume * 100.0)) + L"%");
    volumeText.MinWidth(32);
    volumeText.FontSize(11);
    volumeText.Opacity(0.70);
    volumeText.TextAlignment(TextAlignment::Right);
    volumeText.VerticalAlignment(VerticalAlignment::Center);
    muxc::Grid::SetColumn(volumeText, 3);
    state->volumeText = winrt::make_weak(volumeText);

    volumeSlider.ValueChanged(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    muxp::RangeBaseValueChangedEventArgs const& args) {
            auto state = weakState.lock();
            if (!state || state->updatingVolume) return;
            double volume = std::clamp(args.NewValue() / 100.0, 0.0, 1.0);
            {
                std::lock_guard lock(g_betterMediaMutex);
                g_betterMediaVolume = volume;
                if (g_betterMediaPlayer) {
                    try { g_betterMediaPlayer.Volume(volume); }
                    catch (...) {}
                }
            }
            if (auto text = state->volumeText.get()) {
                text.Text(std::to_wstring(
                              static_cast<int>(std::lround(volume * 100.0))) +
                          L"%");
            }
        });
    playbackBar.Children().Append(volumeSlider);
    playbackBar.Children().Append(volumeText);
    audioContent.Children().Append(playbackBar);
    audioContent.Children().Append(timeText);

    audioCard.Child(audioContent);
    panel.Children().Append(audioCard);

    muxc::Border archivePreviewCard;
    archivePreviewCard.Name(L"BetterDetailPanelArchivePreviewCard");
    archivePreviewCard.Margin(Thickness{0, 4, 0, 0});
    archivePreviewCard.Padding(Thickness{12, 10, 12, 12});
    archivePreviewCard.CornerRadius(CornerRadius{8});
    archivePreviewCard.Visibility(Visibility::Collapsed);
    archivePreviewCard.HorizontalAlignment(HorizontalAlignment::Stretch);
    state->archivePreviewCard =
        winrt::make_weak(archivePreviewCard.as<FrameworkElement>());

    muxc::StackPanel archivePreviewContent;
    archivePreviewContent.Spacing(8);
    muxc::TextBlock archivePreviewHeading;
    archivePreviewHeading.Text(L"Archive contents");
    archivePreviewHeading.FontWeight(
        winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    archivePreviewContent.Children().Append(archivePreviewHeading);

    muxc::ScrollViewer archivePreviewScroll;
    archivePreviewScroll.MaxHeight(240);
    archivePreviewScroll.HorizontalScrollBarVisibility(
        muxc::ScrollBarVisibility::Auto);
    archivePreviewScroll.VerticalScrollBarVisibility(
        muxc::ScrollBarVisibility::Auto);
    muxc::TextBlock archivePreviewText;
    archivePreviewText.Text(L"Reading archive contents…");
    archivePreviewText.FontFamily(
        winrt::Microsoft::UI::Xaml::Media::FontFamily(L"Consolas"));
    archivePreviewText.FontSize(11);
    archivePreviewText.IsTextSelectionEnabled(true);
    archivePreviewText.TextWrapping(TextWrapping::NoWrap);
    state->archivePreviewText = winrt::make_weak(archivePreviewText);
    archivePreviewScroll.Content(archivePreviewText);
    archivePreviewContent.Children().Append(archivePreviewScroll);
    archivePreviewCard.Child(archivePreviewContent);
    panel.Children().Append(archivePreviewCard);

    muxc::Border textCard;
    textCard.Name(L"BetterDetailPanelTextCard");
    textCard.Margin(Thickness{0, 4, 0, 0});
    textCard.Padding(Thickness{12, 10, 12, 12});
    textCard.CornerRadius(CornerRadius{8});
    textCard.Visibility(Visibility::Collapsed);
    textCard.HorizontalAlignment(HorizontalAlignment::Stretch);
    state->textCard = winrt::make_weak(textCard.as<FrameworkElement>());

    muxc::StackPanel textContent;
    textContent.Spacing(8);

    muxc::Grid textHeadingRow;
    muxc::ColumnDefinition textHeadingColumn;
    textHeadingColumn.Width(GridLength{1, GridUnitType::Star});
    muxc::ColumnDefinition textLanguageColumn;
    textLanguageColumn.Width(GridLength{1, GridUnitType::Auto});
    textHeadingRow.ColumnDefinitions().Append(textHeadingColumn);
    textHeadingRow.ColumnDefinitions().Append(textLanguageColumn);

    muxc::TextBlock textHeading;
    textHeading.Text(L"Text preview");
    textHeading.FontWeight(
        winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    textHeading.VerticalAlignment(VerticalAlignment::Center);
    textHeadingRow.Children().Append(textHeading);

    muxc::ComboBox syntaxLanguagePicker;
    syntaxLanguagePicker.Width(150);
    syntaxLanguagePicker.Height(32);
    syntaxLanguagePicker.MinHeight(32);
    syntaxLanguagePicker.HorizontalAlignment(HorizontalAlignment::Right);
    syntaxLanguagePicker.VerticalAlignment(VerticalAlignment::Center);
    muxc::Grid::SetColumn(syntaxLanguagePicker, 1);
    muxa::AutomationProperties::SetName(syntaxLanguagePicker,
                                        L"Syntax language");
    for (auto const& option : kBetterPanelSyntaxOptions) {
        muxc::ComboBoxItem item;
        item.Content(winrt::box_value(option.label));
        syntaxLanguagePicker.Items().Append(item);
    }
    syntaxLanguagePicker.SelectedIndex(0);
    state->syntaxLanguagePicker = winrt::make_weak(syntaxLanguagePicker);
    syntaxLanguagePicker.SelectionChanged(
        [weakState](winrt::Windows::Foundation::IInspectable const& sender,
                    muxc::SelectionChangedEventArgs const&) {
            auto picker = sender.try_as<muxc::ComboBox>();
            auto state = weakState.lock();
            if (!picker || !state) return;
            int index = picker.SelectedIndex();
            if (index < 0 ||
                index >= static_cast<int>(kBetterPanelSyntaxOptions.size()))
                return;
            state->syntaxLanguage =
                kBetterPanelSyntaxOptions[static_cast<size_t>(index)].language;
            if (auto editor = state->textEditor.get()) {
                BetterPanelRenderSyntaxPreview(
                    state, std::wstring(editor.Text().c_str()));
                BetterPanelUpdateTextControls(state);
            }
        });
    textHeadingRow.Children().Append(syntaxLanguagePicker);
    textContent.Children().Append(textHeadingRow);

    muxc::TextBlock textInfo;
    textInfo.FontSize(11);
    textInfo.Opacity(0.70);
    textInfo.TextWrapping(TextWrapping::Wrap);
    state->textInfo = winrt::make_weak(textInfo);
    textContent.Children().Append(textInfo);

    muxc::TextBox textEditor;
    textEditor.AcceptsReturn(true);
    textEditor.IsReadOnly(true);
    textEditor.IsSpellCheckEnabled(false);
    textEditor.IsTextPredictionEnabled(false);
    textEditor.TextWrapping(TextWrapping::NoWrap);
    textEditor.MinHeight(220);
    textEditor.MaxHeight(430);
    textEditor.HorizontalAlignment(HorizontalAlignment::Stretch);
    textEditor.FontFamily(
        winrt::Microsoft::UI::Xaml::Media::FontFamily(L"Consolas"));
    constexpr std::array<int, 7> textPreviewSizes{10, 11, 12, 14, 16, 18, 20};
    int savedTextPreviewSize = Wh_GetIntValue(L"betterPanelTextPreviewSize", 12);
    if (std::find(textPreviewSizes.begin(), textPreviewSizes.end(),
                  savedTextPreviewSize) == textPreviewSizes.end()) {
        savedTextPreviewSize = 12;
    }
    textEditor.FontSize(savedTextPreviewSize);
    muxc::ScrollViewer::SetHorizontalScrollBarVisibility(
        textEditor, muxc::ScrollBarVisibility::Auto);
    muxc::ScrollViewer::SetVerticalScrollBarVisibility(
        textEditor, muxc::ScrollBarVisibility::Auto);
    state->textEditor = winrt::make_weak(textEditor);
    textEditor.TextChanged(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    muxc::TextChangedEventArgs const&) {
            auto state = weakState.lock();
            auto editor = state ? state->textEditor.get() : nullptr;
            if (!state || !editor || !state->textEditing ||
                state->suppressTextChanged) {
                return;
            }
            state->textDirty = editor.Text() != state->textOriginal;
            BetterPanelUpdateTextControls(state);
        });
    muxc::Border syntaxPreviewSurface;
    syntaxPreviewSurface.MinHeight(220);
    syntaxPreviewSurface.MaxHeight(430);
    syntaxPreviewSurface.Padding(Thickness{10, 7, 10, 7});
    syntaxPreviewSurface.CornerRadius(CornerRadius{4});
    syntaxPreviewSurface.Background(
        winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(
            winrt::Windows::UI::ColorHelper::FromArgb(24, 128, 128, 128)));
    syntaxPreviewSurface.BorderBrush(
        winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(
            winrt::Windows::UI::ColorHelper::FromArgb(40, 128, 128, 128)));
    syntaxPreviewSurface.BorderThickness(Thickness{1});
    state->syntaxPreviewSurface =
        winrt::make_weak(syntaxPreviewSurface.as<FrameworkElement>());

    muxc::ScrollViewer syntaxPreviewScroll;
    syntaxPreviewScroll.HorizontalScrollBarVisibility(
        muxc::ScrollBarVisibility::Auto);
    syntaxPreviewScroll.VerticalScrollBarVisibility(
        muxc::ScrollBarVisibility::Auto);
    muxc::TextBlock syntaxPreview;
    syntaxPreview.FontFamily(
        winrt::Microsoft::UI::Xaml::Media::FontFamily(L"Consolas"));
    syntaxPreview.FontSize(savedTextPreviewSize);
    syntaxPreview.TextWrapping(TextWrapping::NoWrap);
    syntaxPreview.IsTextSelectionEnabled(true);
    state->syntaxPreview = winrt::make_weak(syntaxPreview);
    syntaxPreviewScroll.Content(syntaxPreview);
    syntaxPreviewSurface.Child(syntaxPreviewScroll);
    textContent.Children().Append(syntaxPreviewSurface);

    textEditor.Visibility(Visibility::Collapsed);
    textContent.Children().Append(textEditor);

    muxc::Grid textActions;
    textActions.HorizontalAlignment(HorizontalAlignment::Stretch);
    muxc::ColumnDefinition textButtonsColumn;
    textButtonsColumn.Width(GridLength{1, GridUnitType::Star});
    muxc::ColumnDefinition textSizeColumn;
    textSizeColumn.Width(GridLength{1, GridUnitType::Auto});
    textActions.ColumnDefinitions().Append(textButtonsColumn);
    textActions.ColumnDefinitions().Append(textSizeColumn);

    auto textActionButtons = BetterPanelMakeRow();
    textActionButtons.HorizontalAlignment(HorizontalAlignment::Left);
    textActions.Children().Append(textActionButtons);

    auto textEditButton =
        BetterPanelMakeIconButton(L"Edit", L"\uE70F");
    textEditButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            if (auto state = weakState.lock()) {
                BetterPanelBeginTextEdit(state);
            }
        });
    state->textEditButton = winrt::make_weak(textEditButton);
    textActionButtons.Children().Append(textEditButton);

    auto textSaveButton =
        BetterPanelMakeIconButton(L"Save", L"\uE74E");
    textSaveButton.Visibility(Visibility::Collapsed);
    textSaveButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            if (auto state = weakState.lock()) {
                BetterPanelSaveTextEdit(state);
            }
        });
    state->textSaveButton = winrt::make_weak(textSaveButton);
    textActionButtons.Children().Append(textSaveButton);

    auto textCancelButton = BetterPanelMakeButton(L"Cancel");
    textCancelButton.Visibility(Visibility::Collapsed);
    textCancelButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            if (auto state = weakState.lock()) {
                BetterPanelCancelTextEdit(state);
            }
        });
    state->textCancelButton = winrt::make_weak(textCancelButton);
    textActionButtons.Children().Append(textCancelButton);

    muxc::TextBlock textActionStatus;
    textActionStatus.Name(L"BetterPanelTextActionStatus");
    textActionStatus.FontSize(11);
    textActionStatus.Opacity(0.70);
    textActionStatus.VerticalAlignment(VerticalAlignment::Center);
    textActionStatus.TextWrapping(TextWrapping::NoWrap);
    textActionStatus.TextTrimming(TextTrimming::CharacterEllipsis);
    textActionStatus.MaxWidth(360);
    textActionStatus.Margin(Thickness{4, 0, 0, 0});
    textActionStatus.Visibility(Visibility::Collapsed);
    state->textActionStatus = winrt::make_weak(textActionStatus);

    auto textReloadButton =
        BetterPanelMakeIconButton(L"Reload", L"\uE72C");
    textReloadButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (state && !state->textEditing &&
                BetterPanelIsTextFile(state->selectedPath)) {
                BetterPanelLoadTextPreview(state, state->selectedPath);
                BetterPanelSetStatus(state->textActionStatus,
                                     L"Reloading text preview");
            }
        });
    state->textReloadButton = winrt::make_weak(textReloadButton);
    textActionButtons.Children().Append(textReloadButton);
    textActionButtons.Children().Append(textActionStatus);

    muxc::ComboBox textSizePicker;
    textSizePicker.Width(82);
    textSizePicker.Height(32);
    textSizePicker.MinHeight(32);
    textSizePicker.HorizontalAlignment(HorizontalAlignment::Right);
    textSizePicker.VerticalAlignment(VerticalAlignment::Center);
    muxc::Grid::SetColumn(textSizePicker, 1);
    muxa::AutomationProperties::SetName(textSizePicker,
                                        L"Text preview size");
    int selectedTextSizeIndex = 2;
    for (size_t index = 0; index < textPreviewSizes.size(); ++index) {
        muxc::ComboBoxItem item;
        item.Content(winrt::box_value(
            std::to_wstring(textPreviewSizes[index]) + L" px"));
        textSizePicker.Items().Append(item);
        if (textPreviewSizes[index] == savedTextPreviewSize) {
            selectedTextSizeIndex = static_cast<int>(index);
        }
    }
    textSizePicker.SelectedIndex(selectedTextSizeIndex);
    textSizePicker.SelectionChanged(
        [weakState](winrt::Windows::Foundation::IInspectable const& sender,
                    muxc::SelectionChangedEventArgs const&) {
            static constexpr std::array<int, 7> sizes{10, 11, 12, 14,
                                                       16, 18, 20};
            auto picker = sender.try_as<muxc::ComboBox>();
            auto state = weakState.lock();
            if (!picker || !state) return;
            int index = picker.SelectedIndex();
            if (index < 0 || index >= static_cast<int>(sizes.size())) return;
            int size = sizes[index];
            if (auto editor = state->textEditor.get()) {
                editor.FontSize(size);
            }
            if (auto preview = state->syntaxPreview.get()) {
                preview.FontSize(size);
            }
            Wh_SetIntValue(L"betterPanelTextPreviewSize", size);
        });
    textActions.Children().Append(textSizePicker);

    textContent.Children().Append(textActions);
    textCard.Child(textContent);
    panel.Children().Append(textCard);

    muxc::Border pdfCard;
    pdfCard.Name(L"BetterDetailPanelPdfCard");
    pdfCard.Margin(Thickness{0, 4, 0, 0});
    pdfCard.Padding(Thickness{10, 10, 10, 12});
    pdfCard.CornerRadius(CornerRadius{8});
    pdfCard.Visibility(Visibility::Collapsed);
    pdfCard.HorizontalAlignment(HorizontalAlignment::Stretch);
    state->pdfCard = winrt::make_weak(pdfCard.as<FrameworkElement>());

    muxc::StackPanel pdfContent;
    pdfContent.Spacing(8);

    muxc::TextBlock pdfHeading;
    pdfHeading.Text(L"PDF preview");
    pdfHeading.FontWeight(
        winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    pdfContent.Children().Append(pdfHeading);

    muxc::TextBlock pdfInfo;
    pdfInfo.Text(L"Loading PDF…");
    pdfInfo.FontSize(11);
    pdfInfo.Opacity(0.72);
    pdfInfo.TextWrapping(TextWrapping::Wrap);
    state->pdfInfo = winrt::make_weak(pdfInfo);
    pdfContent.Children().Append(pdfInfo);

    muxc::Border pdfPageSurface;
    pdfPageSurface.Height(460);
    pdfPageSurface.HorizontalAlignment(HorizontalAlignment::Stretch);
    pdfPageSurface.CornerRadius(CornerRadius{4});
    state->pdfPageSurface =
        winrt::make_weak(pdfPageSurface.as<FrameworkElement>());

    muxc::ScrollViewer pdfScroller;
    pdfScroller.HorizontalScrollBarVisibility(muxc::ScrollBarVisibility::Auto);
    pdfScroller.VerticalScrollBarVisibility(muxc::ScrollBarVisibility::Auto);
    pdfScroller.HorizontalScrollMode(muxc::ScrollMode::Enabled);
    pdfScroller.VerticalScrollMode(muxc::ScrollMode::Enabled);

    muxc::Image pdfImage;
    pdfImage.Stretch(winrt::Microsoft::UI::Xaml::Media::Stretch::Uniform);
    pdfImage.HorizontalAlignment(HorizontalAlignment::Center);
    pdfImage.VerticalAlignment(VerticalAlignment::Top);
    state->pdfImage = winrt::make_weak(pdfImage);
    pdfScroller.Content(pdfImage);
    pdfPageSurface.Child(pdfScroller);
    pdfContent.Children().Append(pdfPageSurface);

    auto pdfNavigation = BetterPanelMakeRow();
    pdfNavigation.HorizontalAlignment(HorizontalAlignment::Center);

    auto pdfPrevious = BetterPanelMakeIconButton(L"Previous", L"\uE76B");
    pdfPrevious.IsEnabled(false);
    state->pdfPreviousButton = winrt::make_weak(pdfPrevious);
    pdfPrevious.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state || state->pdfLoading || !state->pdfDocument ||
                state->pdfPageIndex == 0) {
                return;
            }
            --state->pdfPageIndex;
            BetterPanelUpdatePdfControls(state);
            BetterPanelRenderPdfPage(weakState, state->selectedPath,
                                     state->pdfPageIndex, state->pdfZoom);
        });
    pdfNavigation.Children().Append(pdfPrevious);

    muxc::TextBlock pdfPageText;
    pdfPageText.Text(L"Page -- of --");
    pdfPageText.MinWidth(96);
    pdfPageText.TextAlignment(TextAlignment::Center);
    pdfPageText.VerticalAlignment(VerticalAlignment::Center);
    state->pdfPageText = winrt::make_weak(pdfPageText);
    pdfNavigation.Children().Append(pdfPageText);

    auto pdfNext = BetterPanelMakeIconButton(L"Next", L"\uE76C");
    pdfNext.IsEnabled(false);
    state->pdfNextButton = winrt::make_weak(pdfNext);
    pdfNext.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state || state->pdfLoading || !state->pdfDocument ||
                state->pdfPageIndex + 1 >= state->pdfDocument.PageCount()) {
                return;
            }
            ++state->pdfPageIndex;
            BetterPanelUpdatePdfControls(state);
            BetterPanelRenderPdfPage(weakState, state->selectedPath,
                                     state->pdfPageIndex, state->pdfZoom);
        });
    pdfNavigation.Children().Append(pdfNext);
    pdfContent.Children().Append(pdfNavigation);

    auto pdfZoomRow = BetterPanelMakeRow();
    pdfZoomRow.HorizontalAlignment(HorizontalAlignment::Center);

    auto pdfZoomOut = BetterPanelMakeIconButton(L"Zoom out", L"\uE71F");
    pdfZoomOut.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state || state->pdfLoading || !state->pdfDocument) return;
            state->pdfZoom = std::max(0.5, state->pdfZoom - 0.25);
            BetterPanelUpdatePdfControls(state);
            BetterPanelRenderPdfPage(weakState, state->selectedPath,
                                     state->pdfPageIndex, state->pdfZoom);
        });
    pdfZoomRow.Children().Append(pdfZoomOut);

    muxc::TextBlock pdfZoomText;
    pdfZoomText.Text(L"100%");
    pdfZoomText.MinWidth(52);
    pdfZoomText.TextAlignment(TextAlignment::Center);
    pdfZoomText.VerticalAlignment(VerticalAlignment::Center);
    state->pdfZoomText = winrt::make_weak(pdfZoomText);
    pdfZoomRow.Children().Append(pdfZoomText);

    auto pdfZoomIn = BetterPanelMakeIconButton(L"Zoom in", L"\uE8A3");
    pdfZoomIn.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state || state->pdfLoading || !state->pdfDocument) return;
            state->pdfZoom = std::min(2.5, state->pdfZoom + 0.25);
            BetterPanelUpdatePdfControls(state);
            BetterPanelRenderPdfPage(weakState, state->selectedPath,
                                     state->pdfPageIndex, state->pdfZoom);
        });
    pdfZoomRow.Children().Append(pdfZoomIn);
    pdfContent.Children().Append(pdfZoomRow);

    pdfCard.Child(pdfContent);
    panel.Children().Append(pdfCard);

    // Keep the preview toggle in one predictable location: directly below
    // whichever native or custom preview is currently visible.
    panel.Children().Append(panelUtilities);

    panel.Children().Append(fileTitleRow);

    muxc::StackPanel actionsHost;
    actionsHost.Name(L"BetterDetailPanelInlineActionsHost");
    actionsHost.HorizontalAlignment(HorizontalAlignment::Stretch);
    state->actionsHost = winrt::make_weak(actionsHost);
    panel.Children().Append(actionsHost);
    actionsHost.Children().Append(status);

    // Multiple-selection actions belong with the other top-level file actions,
    // not below the entire Details card.
    panel.Children().Append(multiActionRow);

    // Keep destination actions in the natural viewport immediately below the
    // normal action buttons instead of after Details and Metadata.
    panel.Children().Append(transferRow);

    muxc::Border homeCard;
    homeCard.Name(L"BetterDetailPanelHomeCard");
    homeCard.Padding(Thickness{10, 8, 10, 10});
    homeCard.CornerRadius(CornerRadius{8});
    homeCard.Visibility(Visibility::Collapsed);
    homeCard.HorizontalAlignment(HorizontalAlignment::Stretch);
    state->homeCard = winrt::make_weak(homeCard.as<FrameworkElement>());

    muxc::StackPanel homeContent;
    homeContent.Spacing(5);
    state->homeContent = winrt::make_weak(homeContent);
    homeCard.Child(homeContent);
    panel.Children().Append(homeCard);

    muxc::Border driveCard;
    driveCard.Name(L"BetterDetailPanelDriveCard");
    // Pull the drive shortcuts closer to the preview visibility button. The
    // native details-pane host reserves extra vertical space in this state.
    driveCard.Margin(Thickness{0, -32, 0, 0});
    driveCard.Padding(Thickness{0, 0, 0, 6});
    driveCard.Visibility(Visibility::Collapsed);
    driveCard.HorizontalAlignment(HorizontalAlignment::Stretch);
    state->driveCard = winrt::make_weak(driveCard.as<FrameworkElement>());
    muxc::StackPanel driveContent;
    driveContent.Spacing(6);
    state->driveContent = winrt::make_weak(driveContent);
    driveCard.Child(driveContent);
    panel.Children().Append(driveCard);

    muxc::Border insightsCard;
    insightsCard.Name(L"BetterDetailPanelInsightsCard");
    insightsCard.Padding(Thickness{10, 8, 10, 10});
    insightsCard.CornerRadius(CornerRadius{8});
    insightsCard.Visibility(Visibility::Collapsed);
    insightsCard.HorizontalAlignment(HorizontalAlignment::Stretch);
    state->insightsCard =
        winrt::make_weak(insightsCard.as<FrameworkElement>());

    muxc::StackPanel insightsShell;
    insightsShell.Spacing(6);

    muxc::Grid insightsHeader;
    muxc::ColumnDefinition insightsTitleColumn;
    insightsTitleColumn.Width(GridLength{1, GridUnitType::Star});
    muxc::ColumnDefinition insightsCopyColumn;
    insightsCopyColumn.Width(GridLength{1, GridUnitType::Auto});
    insightsHeader.ColumnDefinitions().Append(insightsTitleColumn);
    insightsHeader.ColumnDefinitions().Append(insightsCopyColumn);

    auto insightsToggle = BetterPanelMakeButton(L"Details  ▾");
    insightsToggle.FontWeight(
        winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    insightsToggle.HorizontalContentAlignment(HorizontalAlignment::Left);
    insightsToggle.HorizontalAlignment(HorizontalAlignment::Stretch);
    insightsToggle.Padding(Thickness{6, 4, 6, 4});
    state->insightsToggleButton = winrt::make_weak(insightsToggle);
    insightsHeader.Children().Append(insightsToggle);

    auto copyDetailsButton = BetterPanelMakeButton(L"");
    muxc::FontIcon copyDetailsIcon;
    copyDetailsIcon.Glyph(L"\uE8C8");
    copyDetailsIcon.FontSize(13);
    copyDetailsButton.Content(copyDetailsIcon);
    copyDetailsButton.Width(32);
    copyDetailsButton.Height(32);
    copyDetailsButton.MinWidth(32);
    copyDetailsButton.MinHeight(32);
    copyDetailsButton.Padding(Thickness{0, 0, 0, 0});
    copyDetailsButton.Margin(Thickness{6, 0, 0, 0});
    copyDetailsButton.HorizontalAlignment(HorizontalAlignment::Right);
    copyDetailsButton.VerticalAlignment(VerticalAlignment::Center);
    muxc::Grid::SetColumn(copyDetailsButton, 1);
    muxa::AutomationProperties::SetName(copyDetailsButton,
                                        L"Copy all details");
    muxc::ToolTipService::SetToolTip(copyDetailsButton,
                                    winrt::box_value(L"Copy all details"));
    copyDetailsButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            if (state->detailsCopyText.empty()) {
                BetterPanelSetStatus(state->detailsCopyHint,
                                     L"Details are still loading");
                return;
            }
            BetterPanelSetStatus(
                state->detailsCopyHint,
                BetterPanelCopyText(state->detailsCopyText)
                    ? L"All details copied"
                    : L"Copy failed");
        });
    insightsHeader.Children().Append(copyDetailsButton);
    insightsShell.Children().Append(insightsHeader);

    muxc::StackPanel insightsContent;
    insightsContent.Spacing(4);
    state->insightsContent = winrt::make_weak(insightsContent);
    insightsShell.Children().Append(insightsContent);

    muxc::TextBlock copyDetailsHint;
    copyDetailsHint.Name(L"BetterPanelDetailsCopyHint");
    copyDetailsHint.Text(L"Select a box to copy");
    copyDetailsHint.FontSize(11);
    copyDetailsHint.Opacity(0.72);
    copyDetailsHint.HorizontalAlignment(HorizontalAlignment::Right);
    copyDetailsHint.Margin(Thickness{0, 0, 2, 0});
    state->detailsCopyHint = winrt::make_weak(copyDetailsHint);
    insightsShell.Children().Append(copyDetailsHint);
    insightsToggle.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            state->insightsCollapsed = !state->insightsCollapsed;
            if (auto content = state->insightsContent.get()) {
                content.Visibility(state->insightsCollapsed
                                       ? Visibility::Collapsed
                                       : Visibility::Visible);
            }
            if (auto hint = state->detailsCopyHint.get()) {
                hint.Visibility(state->insightsCollapsed
                                    ? Visibility::Collapsed
                                    : Visibility::Visible);
            }
            if (auto button = state->insightsToggleButton.get()) {
                button.Content(winrt::box_value(state->insightsCollapsed
                                                    ? L"Details  ▸"
                                                    : L"Details  ▾"));
            }
        });

    auto propertiesButton = BetterPanelMakeIconButton(L"Properties", L"\uE90F");
    propertiesButton.Margin(Thickness{0, 8, 0, 0});
    propertiesButton.HorizontalAlignment(HorizontalAlignment::Left);
    propertiesButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state || state->selectedPath.empty()) return;
            SHELLEXECUTEINFOW info{sizeof(info)};
            info.fMask = SEE_MASK_INVOKEIDLIST;
            info.hwnd = GetForegroundWindow();
            info.lpVerb = L"properties";
            info.lpFile = state->selectedPath.c_str();
            info.nShow = SW_SHOWNORMAL;
            BetterPanelSetStatus(state->status,
                                 ShellExecuteExW(&info)
                                     ? L"Properties opened"
                                     : L"Properties unavailable");
        });
    insightsShell.Children().Append(propertiesButton);
    insightsCard.Child(insightsShell);
    panel.Children().Append(insightsCard);

    muxc::Border metadataCard;
    metadataCard.Name(L"BetterDetailPanelMetadataCard");
    metadataCard.Padding(Thickness{10, 8, 10, 10});
    metadataCard.CornerRadius(CornerRadius{8});
    metadataCard.Visibility(Visibility::Collapsed);
    metadataCard.HorizontalAlignment(HorizontalAlignment::Stretch);
    state->metadataCard =
        winrt::make_weak(metadataCard.as<FrameworkElement>());

    muxc::StackPanel metadataShell;
    metadataShell.Spacing(6);

    muxc::Grid metadataHeader;
    muxc::ColumnDefinition metadataTitleColumn;
    metadataTitleColumn.Width(GridLength{1, GridUnitType::Star});
    muxc::ColumnDefinition metadataCopyColumn;
    metadataCopyColumn.Width(GridLength{1, GridUnitType::Auto});
    metadataHeader.ColumnDefinitions().Append(metadataTitleColumn);
    metadataHeader.ColumnDefinitions().Append(metadataCopyColumn);

    auto metadataToggle = BetterPanelMakeButton(L"Metadata  ▾");
    metadataToggle.FontWeight(
        winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    metadataToggle.HorizontalContentAlignment(HorizontalAlignment::Left);
    metadataToggle.HorizontalAlignment(HorizontalAlignment::Stretch);
    metadataToggle.Padding(Thickness{6, 4, 6, 4});
    state->metadataToggleButton = winrt::make_weak(metadataToggle);
    metadataHeader.Children().Append(metadataToggle);

    auto copyMetadataButton = BetterPanelMakeButton(L"");
    muxc::FontIcon copyMetadataIcon;
    copyMetadataIcon.Glyph(L"\uE8C8");
    copyMetadataIcon.FontSize(13);
    copyMetadataButton.Content(copyMetadataIcon);
    copyMetadataButton.Width(32);
    copyMetadataButton.Height(32);
    copyMetadataButton.Padding(Thickness{0, 0, 0, 0});
    copyMetadataButton.Margin(Thickness{6, 0, 0, 0});
    muxc::Grid::SetColumn(copyMetadataButton, 1);
    muxa::AutomationProperties::SetName(copyMetadataButton,
                                        L"Copy all metadata");
    muxc::ToolTipService::SetToolTip(copyMetadataButton,
                                    winrt::box_value(L"Copy all metadata"));
    copyMetadataButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            std::wstring text = state->metadataEditing
                                    ? BetterPanelFormatMetadata(
                                          BetterPanelCurrentMetadata(state))
                                    : state->metadataCopyText;
            BetterPanelSetStatus(
                state->status,
                !text.empty() && BetterPanelCopyText(text)
                    ? L"All metadata copied"
                    : L"Metadata is still loading");
        });
    metadataHeader.Children().Append(copyMetadataButton);
    metadataShell.Children().Append(metadataHeader);

    muxc::StackPanel metadataContent;
    metadataContent.Spacing(5);
    state->metadataContent = winrt::make_weak(metadataContent);

    auto metadataTitle = BetterPanelAddMetadataField(
        metadataContent, L"Title", L"Song title", false, weakState);
    state->metadataTitle = winrt::make_weak(metadataTitle);
    auto metadataArtist = BetterPanelAddMetadataField(
        metadataContent, L"Artist", L"Artist", false, weakState);
    state->metadataArtist = winrt::make_weak(metadataArtist);
    auto metadataAlbum = BetterPanelAddMetadataField(
        metadataContent, L"Album", L"Album", false, weakState);
    state->metadataAlbum = winrt::make_weak(metadataAlbum);
    auto metadataGenre = BetterPanelAddMetadataField(
        metadataContent, L"Genre", L"Genre, separated by commas", false,
        weakState);
    state->metadataGenre = winrt::make_weak(metadataGenre);
    auto metadataYear = BetterPanelAddMetadataField(
        metadataContent, L"Year", L"Year", false, weakState);
    state->metadataYear = winrt::make_weak(metadataYear);
    auto metadataRating = BetterPanelAddMetadataField(
        metadataContent, L"Rating", L"0–5", false, weakState);
    state->metadataRating = winrt::make_weak(metadataRating);
    auto metadataTags = BetterPanelAddMetadataField(
        metadataContent, L"Tags", L"Tags, separated by commas", false,
        weakState);
    state->metadataTags = winrt::make_weak(metadataTags);
    auto metadataComments = BetterPanelAddMetadataField(
        metadataContent, L"Comments", L"Comments", true, weakState);
    state->metadataComments = winrt::make_weak(metadataComments);

    muxc::TextBlock metadataInfo;
    metadataInfo.Text(L"Reading metadata…");
    metadataInfo.FontSize(11);
    metadataInfo.Opacity(0.72);
    metadataInfo.TextWrapping(TextWrapping::Wrap);
    metadataInfo.Margin(Thickness{0, 2, 0, 2});
    state->metadataInfo = winrt::make_weak(metadataInfo);
    metadataContent.Children().Append(metadataInfo);

    auto metadataActions = BetterPanelMakeRow();
    metadataActions.HorizontalAlignment(HorizontalAlignment::Left);

    auto metadataEditButton =
        BetterPanelMakeIconButton(L"Edit", L"\uE70F");
    metadataEditButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            if (auto state = weakState.lock()) {
                BetterPanelBeginMetadataEdit(state);
            }
        });
    state->metadataEditButton = winrt::make_weak(metadataEditButton);
    metadataActions.Children().Append(metadataEditButton);

    auto metadataSaveButton =
        BetterPanelMakeIconButton(L"Save", L"\uE74E");
    metadataSaveButton.Visibility(Visibility::Collapsed);
    metadataSaveButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            BetterPanelSaveAudioMetadata(weakState);
        });
    state->metadataSaveButton = winrt::make_weak(metadataSaveButton);
    metadataActions.Children().Append(metadataSaveButton);

    auto metadataCancelButton = BetterPanelMakeButton(L"Cancel");
    metadataCancelButton.Visibility(Visibility::Collapsed);
    metadataCancelButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            if (auto state = weakState.lock()) {
                BetterPanelCancelMetadataEdit(state);
            }
        });
    state->metadataCancelButton = winrt::make_weak(metadataCancelButton);
    metadataActions.Children().Append(metadataCancelButton);

    auto metadataReloadButton =
        BetterPanelMakeIconButton(L"Reload", L"\uE72C");
    metadataReloadButton.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state || state->metadataEditing ||
                !BetterPanelIsAudioFile(state->selectedPath)) {
                return;
            }
            BetterPanelLoadAudioMetadata(weakState, state->selectedPath);
        });
    state->metadataReloadButton = winrt::make_weak(metadataReloadButton);
    metadataActions.Children().Append(metadataReloadButton);
    metadataContent.Children().Append(metadataActions);
    metadataShell.Children().Append(metadataContent);

    metadataToggle.Click(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state) return;
            state->metadataCollapsed = !state->metadataCollapsed;
            if (auto content = state->metadataContent.get()) {
                content.Visibility(state->metadataCollapsed
                                       ? Visibility::Collapsed
                                       : Visibility::Visible);
            }
            if (auto button = state->metadataToggleButton.get()) {
                button.Content(winrt::box_value(state->metadataCollapsed
                                                    ? L"Metadata  ▸"
                                                    : L"Metadata  ▾"));
            }
        });

    metadataCard.Child(metadataShell);
    panel.Children().Append(metadataCard);

    host.Children().Append(panel);

    DispatcherTimer timer;
    // Event-driven updates handle normal clicks, keyboard selection, tab
    // changes, and navigation. This slow timer is only a recovery path for
    // changes initiated without an Explorer input message.
    timer.Interval(winrt::Windows::Foundation::TimeSpan{40000000});
    timer.Tick([weakState](
                   winrt::Windows::Foundation::IInspectable const&,
                   winrt::Windows::Foundation::IInspectable const&) {
        if (auto state = weakState.lock()) {
            BetterPanelRefresh(state);
        }
    });
    state->timer = timer;

    DispatcherTimer mediaTimer;
    mediaTimer.Interval(winrt::Windows::Foundation::TimeSpan{5000000});
    mediaTimer.Tick([weakState](
                        winrt::Windows::Foundation::IInspectable const&,
                        winrt::Windows::Foundation::IInspectable const&) {
        if (auto state = weakState.lock()) {
            BetterPanelRefreshPlaybackState(state);
        }
    });
    state->mediaTimer = mediaTimer;
    BetterPanelRefresh(state);
    timer.Start();

    state->hostUnloadedToken = host.Unloaded(
        [weakState](winrt::Windows::Foundation::IInspectable const&,
                    RoutedEventArgs const&) {
            auto state = weakState.lock();
            if (!state || state->unloaded) {
                return;
            }
            state->unloaded = true;
            BetterPanelDetachHostUnloadedHandler(state);
            state->insightsGeneration.fetch_add(1, std::memory_order_relaxed);
            if (state->timer) {
                state->timer.Stop();
            }
            if (state->mediaTimer) {
                state->mediaTimer.Stop();
                state->mediaTimerRunning = false;
            }
            if (state->videoControlsTimer) {
                state->videoControlsTimer.Stop();
            }
            BetterPanelRemoveAnimationHandlers(state);
            BetterPanelStopStateMedia(state);
            BetterPanelUnwatchNativeDetailsVisibility(state);

            std::lock_guard lock(g_betterPanelMutex);
            std::erase_if(g_betterPanels,
                          [&](auto const& item) { return item == state; });
        });

    std::lock_guard lock(g_betterPanelMutex);
    g_betterPanels.push_back(std::move(state));
    Wh_Log(L"Better Detail Panel 0.5 added");
}

void TryInstallBetterDetailPanel(FrameworkElement element) {
    if (element.Name() != L"DetailsViewThumbnail") return;

    auto host = element.try_as<muxc::StackPanel>();
    if (!host) return;
    for (auto const& child : host.Children()) {
        if (auto frameworkElement = child.try_as<FrameworkElement>();
            frameworkElement &&
            (frameworkElement.Name() == L"BetterDetailPanelRoot" ||
             frameworkElement.Name() == L"BetterDetailPanelLoading")) {
            return;
        }
    }

    muxc::StackPanel loadingHost;
    loadingHost.Name(L"BetterDetailPanelLoading");
    loadingHost.HorizontalAlignment(HorizontalAlignment::Stretch);
    loadingHost.Margin(Thickness{16, 28, 16, 18});

    muxc::ProgressRing loadingRing;
    loadingRing.Width(28);
    loadingRing.Height(28);
    loadingRing.IsActive(true);
    loadingRing.HorizontalAlignment(HorizontalAlignment::Center);
    muxa::AutomationProperties::SetName(loadingRing,
                                        L"Loading Better Panel");
    loadingHost.Children().Append(loadingRing);
    host.Children().Append(loadingHost);

    auto weakHost = winrt::make_weak(host);
    auto weakLoadingHost = winrt::make_weak(loadingHost);
    auto install = [weakHost, weakLoadingHost]() {
        auto host = weakHost.get();
        if (!host) return;
        if (auto loadingHost = weakLoadingHost.get()) {
            uint32_t index = 0;
            if (host.Children().IndexOf(loadingHost, index)) {
                host.Children().RemoveAt(index);
            }
        }
        BetterPanelInstallDetailPanel(host);
    };
    if (!host.DispatcherQueue().TryEnqueue(
            mud::DispatcherQueuePriority::Low, install)) {
        install();
    }
}

// Diagnostics-free discovery. Explorer Command Bar demonstrated that a typed
// WinUI element supplied by FileExplorerExtensions.dll can safely anchor a
// public VisualTreeHelper walk. Keeping only a weak anchor avoids owning any of
// Explorer's XAML elements and allows Windows 11 File Explorer Styler to remain
// the process's single XAML Diagnostics consumer.
std::atomic<bool> g_betterPanelDiscoveryUnloading;
thread_local winrt::weak_ref<UIElement> g_betterPanelDiscoveryAnchor;
thread_local bool g_betterPanelDiscoveryQueued = false;
thread_local bool g_betterPanelResumeRecoveryQueued = false;

bool BetterPanelFindAndInstallDetailsHost(DependencyObject const& root,
                                          int depth = 0) {
    if (!root || depth > 64 || g_betterPanelDiscoveryUnloading) return false;

    bool found = false;
    if (auto element = root.try_as<FrameworkElement>(); element) {
        if (element.Name() == L"DetailsViewThumbnail") {
            TryInstallBetterDetailPanel(element);
            found = true;
        }
    }

    int count = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
        GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::
            GetChild(root, i);
        if (BetterPanelFindAndInstallDetailsHost(child, depth + 1)) {
            found = true;
        }
    }
    return found;
}

void BetterPanelScanFromElement(UIElement const& element) try {
    if (!element || g_betterPanelDiscoveryUnloading) {
        return;
    }
    auto xamlRoot = element.XamlRoot();
    auto content = xamlRoot ? xamlRoot.Content() : nullptr;
    if (content) BetterPanelFindAndInstallDetailsHost(content);
} catch (...) {
    Wh_Log(L"Details discovery error %08X", winrt::to_hresult().value);
}

void BetterPanelRememberDiscoveryAnchor(UIElement const& element) {
    if (element) g_betterPanelDiscoveryAnchor = winrt::make_weak(element);
}

void BetterPanelScanCurrentThread() try {
    if (g_betterPanelDiscoveryUnloading) {
        return;
    }
    if (auto anchor = g_betterPanelDiscoveryAnchor.get()) {
        BetterPanelScanFromElement(anchor);
    }
    auto focused = mux::Input::FocusManager::GetFocusedElement();
    if (auto element = focused ? focused.try_as<UIElement>() : nullptr) {
        BetterPanelRememberDiscoveryAnchor(element);
        BetterPanelScanFromElement(element);
    }
} catch (...) {
    Wh_Log(L"Current-thread discovery error %08X",
           winrt::to_hresult().value);
}

void BetterPanelScheduleCurrentThreadDiscovery() try {
    if (g_betterPanelDiscoveryUnloading || g_betterPanelDiscoveryQueued) {
        return;
    }
    auto dispatcher =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!dispatcher) {
        BetterPanelScanCurrentThread();
        return;
    }
    g_betterPanelDiscoveryQueued = true;
    if (!dispatcher.TryEnqueue([] {
            g_betterPanelDiscoveryQueued = false;
            BetterPanelScanCurrentThread();
        })) {
        g_betterPanelDiscoveryQueued = false;
    }
} catch (...) {
    g_betterPanelDiscoveryQueued = false;
    Wh_Log(L"Queue discovery error %08X", winrt::to_hresult().value);
}

void RemoveBetterDetailPanelsForCurrentThread() {
    std::lock_guard lock(g_betterPanelMutex);
    for (auto it = g_betterPanels.begin(); it != g_betterPanels.end();) {
        auto const& state = *it;
        if (!state->dispatcher || !state->dispatcher.HasThreadAccess()) {
            ++it;
            continue;
        }

        state->unloaded = true;
        BetterPanelDetachHostUnloadedHandler(state);

        if (state->timer) {
            state->timer.Stop();
        }
        if (state->mediaTimer) {
            state->mediaTimer.Stop();
            state->mediaTimerRunning = false;
        }
        if (state->videoControlsTimer) {
            state->videoControlsTimer.Stop();
        }
        BetterPanelRemoveAnimationHandlers(state);
        BetterPanelStopStateMedia(state);
        if (state->previewExpanded) {
            BetterPanelSetInlinePreviewExpanded(state, false);
        }
        if (auto nativeTitle = state->nativeTitleContainer.get()) {
            nativeTitle.Visibility(state->nativeTitleVisibility);
        }
        if (auto expand = state->previewExpandButton.get()) {
            auto expandParent = winrt::Microsoft::UI::Xaml::Media::
                VisualTreeHelper::GetParent(expand).try_as<muxc::Panel>();
            if (expandParent) {
                uint32_t expandIndex = 0;
                if (expandParent.Children().IndexOf(expand, expandIndex)) {
                    expandParent.Children().RemoveAt(expandIndex);
                }
            }
        }
        if (auto surface = state->multiPreviewSurface.get()) {
            auto surfaceParent = winrt::Microsoft::UI::Xaml::Media::
                VisualTreeHelper::GetParent(surface).try_as<muxc::Panel>();
            if (surfaceParent) {
                uint32_t surfaceIndex = 0;
                if (surfaceParent.Children().IndexOf(surface.as<UIElement>(),
                                                     surfaceIndex)) {
                    surfaceParent.Children().RemoveAt(surfaceIndex);
                }
            }
        }
        if (auto nativePreview = state->nativePreview.get()) {
            nativePreview.Visibility(Visibility::Visible);
        }
        auto host = state->host.get();
        auto panel = state->panel.get();
        if (host && panel) {
            uint32_t index = 0;
            if (host.Children().IndexOf(panel.as<UIElement>(), index)) {
                host.Children().RemoveAt(index);
            }
        }
        auto share = state->nativeShareButton.get();
        auto shareRow = state->shareActionRow.get();
        if (shareRow) {
            auto currentParent = winrt::Microsoft::UI::Xaml::Media::
                VisualTreeHelper::GetParent(shareRow).try_as<muxc::Panel>();
            if (currentParent) {
                uint32_t rowInCurrentParent = 0;
                if (currentParent.Children().IndexOf(shareRow,
                                                     rowInCurrentParent)) {
                    currentParent.Children().RemoveAt(rowInCurrentParent);
                }
            }
        }
        if (share) {
            share.Margin(state->nativeShareMargin);
            share.Visibility(state->nativeShareVisibility);
        }
        BetterPanelUnwatchNativeDetailsVisibility(state);
        if (auto nativeDetails = state->nativeDetailsSection.get()) {
            nativeDetails.Visibility(state->nativeDetailsVisibility);
        }
        if (auto nativeInfoBanner = state->nativeInfoBanner.get()) {
            nativeInfoBanner.Visibility(state->nativeInfoBannerVisibility);
        }
        it = g_betterPanels.erase(it);
    }
}

void BetterPanelHandlePowerMessage(MSG const* message) {
    if (!message || message->message != WM_POWERBROADCAST ||
        (message->wParam != PBT_APMRESUMEAUTOMATIC &&
         message->wParam != PBT_APMRESUMESUSPEND) ||
        g_betterPanelResumeRecoveryQueued) {
        return;
    }

    // Explorer rebuilds the native Details sharing section after resume. Tear
    // down our pane before Explorer processes that refresh, then reinstall it
    // after the native popup/view-model work has settled.
    g_betterPanelResumeRecoveryQueued = true;
    RemoveBetterDetailPanelsForCurrentThread();
    BetterPanelInvalidateExplorerQueryCaches();
    auto dispatcher =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!dispatcher) {
        g_betterPanelResumeRecoveryQueued = false;
        return;
    }
    auto weakDispatcher = winrt::make_weak(dispatcher);
    std::thread([weakDispatcher]() {
        Sleep(1200);
        if (auto dispatcher = weakDispatcher.get()) {
            dispatcher.TryEnqueue([] {
                g_betterPanelResumeRecoveryQueued = false;
                BetterPanelScheduleCurrentThreadDiscovery();
            });
        }
    }).detach();
}

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

thread_local std::vector<ElementCustomizationRules>
    g_elementsCustomizationRules;

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

thread_local std::unordered_map<InstanceHandle, ElementCustomizationState>
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
    // Keep the variable-resolution call paths aligned while all elements share
    // one registry.
struct StyleVariableState {
    std::unordered_map<std::wstring, StyleVariableValue> variables;
    std::unordered_map<std::wstring, std::vector<StyleVariableConsumer>>
        consumers;
};

thread_local StyleVariableState g_styleVariableState;

StyleVariableState* GetStyleVariableState() {
    return &g_styleVariableState;
}

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
    winrt::Microsoft::UI::Dispatching::DispatcherQueue dispatcher{nullptr};
};

thread_local FailedImageBrushesForThread g_failedImageBrushesForThread;

// Global registry of all threads that have failed image brushes.
std::mutex g_failedImageBrushesRegistryMutex;
std::vector<winrt::weak_ref<winrt::Microsoft::UI::Dispatching::DispatcherQueue>>
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

thread_local std::vector<ResourceVariableEntry> g_resourceVariables;

// Track original resource values for restoration (per-thread since
// Application::Current().Resources() is per-thread).
thread_local std::unordered_map<std::wstring,
                                winrt::Windows::Foundation::IInspectable>
    g_originalResourceValues;

// Track our merged theme dictionary for cleanup (per-thread).
thread_local ResourceDictionary g_resourceVariablesThemeDict{nullptr};

// For listening to theme color changes (per-thread).
thread_local winrt::Windows::UI::ViewManagement::UISettings g_uiSettings{
    nullptr};
thread_local winrt::event_token g_colorValuesChangedToken;

winrt::Windows::Foundation::IInspectable ReadLocalValueWithWorkaround(
    DependencyObject elementDo,
    DependencyProperty property) {
    auto value = elementDo.ReadLocalValue(property);
    if (value) {
        // A workaround for ColumnDefinitionCollection of
        // NavigationBarControlGrid which can't be read by ReadLocalValue for
        // some reason, even though it seems to be a local property.
        if (value == DependencyProperty::UnsetValue()) {
            auto grid = elementDo.try_as<Controls::Grid>();
            if (grid && grid.Name() == L"NavigationBarControlGrid") {
                auto value2 = elementDo.GetValue(property);
                if (value2 && winrt::get_class_name(value2) ==
                                  L"Microsoft.UI.Xaml.Controls."
                                  L"ColumnDefinitionCollection") {
                    Wh_Log(
                        L"Using GetValue workaround for "
                        L"ColumnDefinitionCollection");
                    value = std::move(value2);
                }
            }
        }

        // TODO: Is this still needed?
#if 0
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
#endif
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
    muc::CompositionBrush CreateEffectBrush();
    muc::CompositionBrush CreateFallbackBrush();

    muc::Compositor m_compositor;
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
    winrt::Microsoft::UI::Dispatching::DispatcherQueue m_dispatcher{nullptr};
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
    m_compositor(muxh::ElementCompositionPreview::GetElementVisual(element)
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
            winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();

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

muc::CompositionBrush XamlBlurBrush::CreateFallbackBrush()
{
    return m_compositor.CreateColorBrush(m_fallbackColor.value_or(m_tint));
}

muc::CompositionBrush XamlBlurBrush::CreateEffectBrush()
{
    auto backdropBrush = m_compositor.CreateBackdropBrush();

    // Rec. 709 luma coefficients, used for saturation and luminosity.
    constexpr float kLumaR = 0.2126f;
    constexpr float kLumaG = 0.7152f;
    constexpr float kLumaB = 0.0722f;

    // 1. Blur
    auto blurEffect = winrt::make_self<GaussianBlurEffect>();
    blurEffect->Source = muc::CompositionEffectSourceParameter(L"backdrop");
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
    muc::CompositionSurfaceBrush noiseBrush{nullptr};
    if (m_noiseOpacity && *m_noiseOpacity > 0.0f)
    {
        float density = m_noiseDensity.value_or(1.0f);

        auto stream = CreateNoiseStream(density);
        auto surface =
            Media::LoadedImageSurface::StartLoadFromStream(stream);
        noiseBrush = m_compositor.CreateSurfaceBrush(surface);
        noiseBrush.Stretch(muc::CompositionStretch::None);

        // Tile via border effect (wrap mode).
        auto borderEffect = winrt::make_self<BorderEffect>();
        borderEffect->Source =
            muc::CompositionEffectSourceParameter(L"NoiseSource");

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
    std::vector<winrt::Microsoft::UI::Dispatching::DispatcherQueue> dispatchers;
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
            g_failedImageBrushesForThread.dispatcher = winrt::Microsoft::UI::
                Dispatching::DispatcherQueue::GetForCurrentThread();
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

const ResolvedRules& GetResolvedPropertyOverrides(
    const std::wstring_view type,
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

            auto style = GetStyleFromXamlSetters(type, xaml);

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
        auto style = GetStyleFromXamlSetters(type, xaml);
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

    for (const auto& propertyValue :
         GetResolvedPropertyValues(matcher.type, &matcher.propertyValues)) {
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
            override.elementMatcher.type, &override.propertyOverrides);

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

// Recursive-descent evaluator for `{{ ... }}` expressions. Operands: number
// literals, backtick-delimited string literals, style variable references, and
// parenthesized subexpressions. Operators: binary + - * /, unary - / +, the
// comparisons < <= == >= > !=, the conditional operator cond ? a : b, and the
// two-arg functions min(a, b) and max(a, b). Standard math precedence.
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
                                     std::vector<std::wstring>* outDeps,
                                     StyleVariableState* state)
        : m_text(text), m_outDeps(outDeps), m_state(state) {}

    // Returns the text form of the result: numeric results are formatted with
    // FormatDoubleInvariant, string results are returned verbatim. Throws
    // std::runtime_error on parse / evaluation failure (including when a value
    // is used where the grammar requires a number, or when a numeric result is
    // non-finite -- NaN/Inf can't be formatted into XAML attributes
    // meaningfully and would also break the consumer-equality check in
    // SetStyleVariableIfChangedAndPropagate, since NaN != NaN).
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
            if (!std::isfinite(*v.number)) {
                throw std::runtime_error(
                    "Style variable expression produced a non-finite result");
            }
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
    // compare by content. A number/string mismatch is a type error in a live
    // branch; in a dead branch it's harmlessly reported as not-equal.
    bool ValuesEqual(const StyleExpressionValue& a,
                     const StyleExpressionValue& b) {
        if (a.IsNumber() && b.IsNumber()) {
            return *a.number == *b.number;
        }
        if (!a.IsNumber() && !b.IsNumber()) {
            return a.text == b.text;
        }
        if (m_live) {
            throw std::runtime_error(
                "Cannot compare a number with a string in style variable "
                "expression");
        }
        return false;
    }

    StyleExpressionValue ParseExpression() { return ParseTernary(); }

    // Conditional operator `cond ? thenVal : elseVal`, right-associative.
    // Short-circuit: only the taken branch is evaluated. The untaken branch is
    // still parsed (to advance the position and enforce syntax) with m_live
    // cleared, which suppresses value-level errors (division by zero, a
    // non-numeric / undefined variable, an unknown function) and dependency
    // capture for that branch.
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
            return StyleExpressionValue::Number(0.0);
        }
        if (it->second.numeric) {
            return StyleExpressionValue::Number(*it->second.numeric);
        }
        // Non-numeric primitive (e.g. a captured string property): usable as a
        // string operand.
        if (it->second.substitutable) {
            return StyleExpressionValue::String(it->second.stringForm);
        }
        // Opaque capture (brush, thickness, etc.): no value form usable in an
        // expression.
        if (m_live) {
            throw std::runtime_error(
                "Style variable used in expression is not a primitive value");
        }
        return StyleExpressionValue::Number(0.0);
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
        return eval.Evaluate();
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
        std::wstring_view(typeName), tmpl.propertyName, *expanded,
        tmpl.isXamlValue);
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
    // The DetailsViewThumbnail StackPanel is owned by the modern Windows 11
    // details pane. Better Panel only observes this element and adds its own
    // controls; it doesn't apply Explorer themes or custom style rules.
    TryInstallBetterDetailPanel(element);

    return;

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
            return L"Microsoft.UI.Xaml.Shapes.Rectangle";
        }

        return L"Microsoft.UI.Xaml.Controls." + std::wstring{type};
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
                winrt::Windows::UI::Xaml::Interop::TypeName{resourceClassName},
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
        auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::
            DispatcherQueue::GetForCurrentThread();
        g_colorValuesChangedToken =
            g_uiSettings.ColorValuesChanged([dispatcherQueue](auto&&, auto&&) {
                dispatcherQueue.TryEnqueue(RefreshThemeResourceEntries);
            });
    }
}

const Theme* GetSelectedTheme() {
    // Better Panel doesn't expose or activate themes.
    return nullptr;
#if 0
    PCWSTR themeName = Wh_GetStringSetting(L"theme");
    const Theme* theme = nullptr;
    if (wcscmp(themeName, L"Translucent Explorer11") == 0) {
        theme = &g_themeTranslucent_Explorer11;
    } else if (wcscmp(themeName, L"MicaBar") == 0) {
        theme = &g_themeMicaBar;
    } else if (wcscmp(themeName, L"NoCommandBar") == 0) {
        theme = &g_themeNoCommandBar;
    } else if (wcscmp(themeName, L"Minimal Explorer11") == 0) {
        theme = &g_themeMinimal_Explorer11;
    } else if (wcscmp(themeName, L"Tabless") == 0) {
        theme = &g_themeTabless;
    } else if (wcscmp(themeName, L"Matter") == 0) {
        theme = &g_themeMatter;
    } else if (wcscmp(themeName, L"WindowGlass") == 0) {
        theme = &g_themeWindowGlass;
    } else if (wcscmp(themeName, L"AddressSearchOnly") == 0) {
        theme = &g_themeAddressSearchOnly;
    } else if (wcscmp(themeName, L"TintedGlass") == 0) {
        theme = &g_themeTintedGlass;
    } else if (wcscmp(themeName, L"LiquidGlass") == 0) {
        theme = &g_themeLiquidGlass;
    } else if (wcscmp(themeName, L"MicaTabless") == 0) {
        theme = &g_themeMicaTabless;
    } else if (wcscmp(themeName, L"OS26 Liquid Glass") == 0) {
        theme = &g_themeOS26_Liquid_Glass;
    } else if (wcscmp(themeName, L"OS26 Liquid Glass_variant_Compact") == 0) {
        theme = &g_themeOS26_Liquid_Glass_variant_Compact;
    } else if (wcscmp(themeName, L"ZEUSosX_044") == 0) {
        theme = &g_themeZEUSosX_044;
    } else if (wcscmp(themeName, L"Compact Explorer11") == 0) {
        theme = &g_themeCompact_Explorer11;
    }
    Wh_FreeStringSetting(themeName);
    return theme;
#endif
}

void ProcessAllStylesFromSettings() {
    const Theme* theme = GetSelectedTheme();

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

    if (g_originalResourceValues.empty() && !g_resourceVariablesThemeDict) {
        return;
    }

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
    RemoveBetterDetailPanelsForCurrentThread();

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

    g_betterPanelDiscoveryAnchor = {};
    g_betterPanelDiscoveryQueued = false;
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

enum class TargetWindowType {
    None,
    FileExplorer,
    XamlExplorerHost,
};

TargetWindowType GetTargetWindowType(HWND hWnd) {
    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return TargetWindowType::None;
    }

    if (_wcsicmp(className, L"CabinetWClass") == 0) {
        return TargetWindowType::FileExplorer;
    }

    // Used by the desktop context menu.
    if (_wcsicmp(className, L"XamlExplorerHostIslandWindow_WASDK") == 0) {
        return TargetWindowType::XamlExplorerHost;
    }

    return TargetWindowType::None;
}

BackgroundTranslucentEffect GetEffectiveBackgroundTranslucentEffect() {
    if (FindAtom(L"WindhawkFileExplorerStylerNoBackgroundEffect")) {
        return BackgroundTranslucentEffect::kDefault;
    }

    return g_settings.backgroundTranslucentEffect.value_or(
        g_themeBackgroundTranslucentEffect);
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hWnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    auto original = [=]() {
        return DwmSetWindowAttribute_Original(hWnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };

    if (dwAttribute != DWMWA_SYSTEMBACKDROP_TYPE &&
        dwAttribute != DWMWA_USE_HOSTBACKDROPBRUSH) {
        return original();
    }

    if (GetTargetWindowType(hWnd) != TargetWindowType::FileExplorer) {
        return original();
    }

    auto backgroundTranslucentEffect =
        GetEffectiveBackgroundTranslucentEffect();

    int backdropType;
    switch (backgroundTranslucentEffect) {
        case BackgroundTranslucentEffect::kDefault:
            return original();
        case BackgroundTranslucentEffect::kAcrylic:
            backdropType = DWMSBT_TRANSIENTWINDOW;
            break;
        case BackgroundTranslucentEffect::kMica:
            backdropType = DWMSBT_MAINWINDOW;
            break;
        case BackgroundTranslucentEffect::kMicaAlt:
            backdropType = DWMSBT_TABBEDWINDOW;
            break;
        case BackgroundTranslucentEffect::kNone:
            backdropType = DWMSBT_NONE;
            break;
    }

    Wh_Log(L">");

    return DwmSetWindowAttribute_Original(hWnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                          &backdropType, sizeof(backdropType));
}

using DwmExtendFrameIntoClientArea_t = decltype(&DwmExtendFrameIntoClientArea);
DwmExtendFrameIntoClientArea_t DwmExtendFrameIntoClientArea_Original;
HRESULT WINAPI DwmExtendFrameIntoClientArea_Hook(HWND hWnd,
                                                 const MARGINS* pMarInset) {
    auto original = [=]() {
        return DwmExtendFrameIntoClientArea_Original(hWnd, pMarInset);
    };

    if (GetTargetWindowType(hWnd) != TargetWindowType::FileExplorer) {
        return original();
    }

    auto backgroundTranslucentEffect =
        GetEffectiveBackgroundTranslucentEffect();
    if (backgroundTranslucentEffect == BackgroundTranslucentEffect::kDefault ||
        g_settings.backgroundTranslucentEffectRegion !=
            BackgroundTranslucentEffectRegion::kEntireWindow) {
        return original();
    }

    Wh_Log(L">");

    MARGINS margins = {-1, -1, -1, -1};
    return DwmExtendFrameIntoClientArea_Original(hWnd, &margins);
}

void ApplyBackgroundTranslucentEffect(
    HWND hWnd,
    std::optional<BackgroundTranslucentEffect> effectToApply = std::nullopt) {
    constexpr WCHAR kBackgroundTranslucentEffectAppliedKey[] =
        L"windhawk_background_effect-" WH_MOD_ID;

    auto effect =
        effectToApply.value_or(GetEffectiveBackgroundTranslucentEffect());
    if (effect == BackgroundTranslucentEffect::kDefault) {
        if (!RemoveProp(hWnd, kBackgroundTranslucentEffectAppliedKey)) {
            return;
        }
    } else {
        SetProp(hWnd, kBackgroundTranslucentEffectAppliedKey, (HANDLE)1);
    }

    Wh_Log(L"Applying background translucent effect %d for %08X",
           static_cast<int>(effect), (DWORD)(ULONG_PTR)hWnd);

    if (effect != BackgroundTranslucentEffect::kDefault &&
        g_settings.backgroundTranslucentEffectRegion ==
            BackgroundTranslucentEffectRegion::kEntireWindow) {
        MARGINS margins = {-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea_Original(hWnd, &margins);
    }

    int backdropType;
    switch (effect) {
        case BackgroundTranslucentEffect::kDefault:
            backdropType = DWMSBT_TABBEDWINDOW;
            break;
        case BackgroundTranslucentEffect::kAcrylic:
            backdropType = DWMSBT_TRANSIENTWINDOW;
            break;
        case BackgroundTranslucentEffect::kMica:
            backdropType = DWMSBT_MAINWINDOW;
            break;
        case BackgroundTranslucentEffect::kMicaAlt:
            backdropType = DWMSBT_TABBEDWINDOW;
            break;
        case BackgroundTranslucentEffect::kNone:
            backdropType = DWMSBT_NONE;
            break;
    }

    DwmSetWindowAttribute_Original(hWnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                   &backdropType, sizeof(backdropType));
}

void TriggerWindowCompositionUpdate(HWND hWnd) {
    WINDOWPOS windowPos = {
        .hwnd = hWnd,
        .flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE,
    };
    SendMessage(hWnd, WM_WINDOWPOSCHANGED, 0, (LPARAM)&windowPos);
    SendMessage(hWnd, WM_DWMCOMPOSITIONCHANGED, 0, 0);
}

void OnWindowCreated(HWND hWnd, PCSTR funcName) {
    TargetWindowType windowType = GetTargetWindowType(hWnd);
    if (windowType != TargetWindowType::None) {
        Wh_Log(L"Initializing - Created window %08X via %S",
               (DWORD)(ULONG_PTR)hWnd, funcName);

        if (windowType == TargetWindowType::FileExplorer) {
            ApplyBackgroundTranslucentEffect(hWnd);
        }

        InitializeForCurrentThread();
        BetterPanelScheduleCurrentThreadDiscovery();
    }
}

void BetterPanelApplyButtonSettingsForCurrentThread() {
    std::vector<std::shared_ptr<BetterPanelState>> states;
    {
        std::lock_guard lock(g_betterPanelMutex);
        for (auto const& state : g_betterPanels) {
            if (state && state->dispatcher &&
                state->dispatcher.HasThreadAccess() && !state->unloaded) {
                states.push_back(state);
            }
        }
    }

    for (auto const& state : states) {
        try {
            if (auto panel = state->panel.get()) {
                BetterPanelPruneButtonLayoutSnapshots(state);
                BetterPanelApplyNativeButtonStyle(state, panel);
            }
            if (auto expand = state->previewExpandButton.get()) {
                BetterPanelApplyNativeButtonStyle(state, expand);
            }
            if (auto expand = state->multiPreviewExpandButton.get()) {
                BetterPanelApplyNativeButtonStyle(state, expand);
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Live button settings error %08X: %s", ex.code(),
                   ex.message().c_str());
        } catch (...) {
            Wh_Log(L"Live button settings error %08X",
                   winrt::to_hresult().value);
        }
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

    OnWindowCreated(hWnd, __FUNCTION__);

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

    OnWindowCreated(hWnd, __FUNCTION__);

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

    OnWindowCreated(hWnd, __FUNCTION__);

    return hWnd;
}

// Diagnostics-free details-pane discovery, adapted from the symbol-hook
// approach reviewed for Windhawk's Explorer Command Bar mod (PR #4895).
using BetterPanelDetailsPaneControl_OnLoaded_t =
    void(WINAPI*)(void* pThis, void* sender, void* args);
BetterPanelDetailsPaneControl_OnLoaded_t
    BetterPanelDetailsPaneControl_OnLoaded_Original;

void WINAPI BetterPanelDetailsPaneControl_OnLoaded_Hook(
    void* pThis, void* sender, void* args) {
    UIElement discoveryAnchor{nullptr};
    if (!g_betterPanelDiscoveryUnloading && sender) {
        // Explorer passes this lifecycle callback's sender as a raw ABI
        // IInspectable pointer on current builds, even though the public PDB
        // describes a projected const-reference. Copying from ABI both uses
        // the correct pointer level and keeps the element alive across the
        // original handler.
        wf::IInspectable inspectable{nullptr};
        winrt::copy_from_abi(inspectable, sender);
        if (inspectable) {
            discoveryAnchor = inspectable.try_as<UIElement>();
        }
    }

    BetterPanelDetailsPaneControl_OnLoaded_Original(pThis, sender, args);
    if (g_betterPanelDiscoveryUnloading) return;

    if (discoveryAnchor) {
        BetterPanelRememberDiscoveryAnchor(discoveryAnchor);
        // The sender is the exact DetailsPaneControl that loaded. Search only
        // its subtree so another tab's cached details host can't be selected.
        BetterPanelFindAndInstallDetailsHost(discoveryAnchor);
    }
    BetterPanelScheduleCurrentThreadDiscovery();
}

// Explorer raises this from the details-pane view model whenever its native
// selection source changes. Hooking the real event avoids depending on Win32
// mouse messages, which WinUI doesn't consistently route through GetMessage.
using BetterPanelDetailsPaneControlVM_SelectionUpdated_t =
    void(WINAPI*)(void* pThis, void* selectionSource, void* args);
BetterPanelDetailsPaneControlVM_SelectionUpdated_t
    BetterPanelDetailsPaneControlVM_SelectionUpdated_Original;

void BetterPanelPrimeMultiSelectionPreviewForCurrentThread() {
    BetterPanelInvalidateExplorerQueryCaches();
    std::vector<std::shared_ptr<BetterPanelState>> states;
    {
        std::lock_guard lock(g_betterPanelMutex);
        for (auto const& state : g_betterPanels) {
            if (state && !state->unloaded && state->dispatcher &&
                state->dispatcher.HasThreadAccess()) {
                states.push_back(state);
            }
        }
    }

    for (auto const& state : states) {
        try {
            BetterPanelExplorerWindowScope explorerWindowScope(
                state->explorerWindow);
            auto paths = BetterPanelGetActiveSelectionPaths();
            bool multiple = paths.size() > 1;
            if (auto nativePreview = state->nativePreview.get()) {
                if (multiple) {
                    double height = nativePreview.ActualHeight();
                    if (height >= 120) {
                        state->multiPreviewNormalHeight = height;
                        if (auto surface = state->multiPreviewSurface.get();
                            surface && !state->multiPreviewExpanded) {
                            surface.Height(height);
                        }
                    }
                }
                nativePreview.Visibility(
                    multiple || state->previewsCollapsed
                        ? Visibility::Collapsed
                        : Visibility::Visible);
            }
            if (auto nativeImage = state->nativePreviewImage.get()) {
                nativeImage.Visibility(multiple ? Visibility::Collapsed
                                                : Visibility::Visible);
            }
            if (auto surface = state->multiPreviewSurface.get()) {
                surface.Visibility(multiple && !state->previewsCollapsed
                                       ? Visibility::Visible
                                       : Visibility::Collapsed);
            }
            if (auto previewHost = state->multiPreviewHost.get()) {
                if (multiple) {
                    // Build the fallback stack during Explorer's selection
                    // callback, before XAML gets a chance to render its blue
                    // generic multiple-items artwork for a single frame.
                    BetterPanelPopulateMultiPreview(state, paths);
                    previewHost.Visibility(state->previewsCollapsed
                                               ? Visibility::Collapsed
                                               : Visibility::Visible);
                } else {
                    state->multiPreviewKey.clear();
                    previewHost.Children().Clear();
                    previewHost.Visibility(Visibility::Collapsed);
                }
            }
        } catch (...) {
            Wh_Log(L"Immediate multi-selection preview error %08X",
                   winrt::to_hresult().value);
        }
    }
}

void WINAPI BetterPanelDetailsPaneControlVM_SelectionUpdated_Hook(
    void* pThis, void* selectionSource, void* args) {
    BetterPanelDetailsPaneControlVM_SelectionUpdated_Original(
        pThis, selectionSource, args);
    if (g_betterPanelDiscoveryUnloading) return;

    BetterPanelPrimeMultiSelectionPreviewForCurrentThread();

    // Defer one dispatcher turn so IFolderView2 exposes the same selection the
    // native details pane has just accepted.
    BetterPanelQueueCurrentThreadRefresh();
}

using BetterPanelCommandBarManager_CommandBar_t =
    void(WINAPI*)(void* pThis, void* commandBar);
BetterPanelCommandBarManager_CommandBar_t
    BetterPanelCommandBarManager_CommandBar_Original;

void WINAPI BetterPanelCommandBarManager_CommandBar_Hook(void* pThis,
                                                          void* commandBar) {
    BetterPanelCommandBarManager_CommandBar_Original(pThis, commandBar);
    if (g_betterPanelDiscoveryUnloading || !commandBar) return;
    try {
        auto const& bar = *reinterpret_cast<muxc::CommandBar const*>(commandBar);
        if (!bar) return;
        BetterPanelRememberDiscoveryAnchor(bar);
        BetterPanelScheduleCurrentThreadDiscovery();
    } catch (...) {
        Wh_Log(L"Command-bar discovery error %08X",
               winrt::to_hresult().value);
    }
}

using BetterPanelCommandBarControl_OnApplyTemplate_t =
    void(WINAPI*)(void* pThis);
BetterPanelCommandBarControl_OnApplyTemplate_t
    BetterPanelCommandBarControl_OnApplyTemplate_Original;
BetterPanelCommandBarControl_OnApplyTemplate_t
    BetterPanelCommandBarControl_Wave1_OnApplyTemplate_Original;

void WINAPI BetterPanelCommandBarControl_OnApplyTemplate_Hook(void* pThis) {
    BetterPanelCommandBarControl_OnApplyTemplate_Original(pThis);
    BetterPanelScheduleCurrentThreadDiscovery();
}

void WINAPI BetterPanelCommandBarControl_Wave1_OnApplyTemplate_Hook(
    void* pThis) {
    BetterPanelCommandBarControl_Wave1_OnApplyTemplate_Original(pThis);
    BetterPanelScheduleCurrentThreadDiscovery();
}

using BetterPanelCommandBarControl_GotFocusHandler_t =
    void(WINAPI*)(void* pThis, void* sender, void* args);
BetterPanelCommandBarControl_GotFocusHandler_t
    BetterPanelCommandBarControl_GotFocusHandler_Original;
BetterPanelCommandBarControl_GotFocusHandler_t
    BetterPanelCommandBarControl_Wave1_GotFocusHandler_Original;

void BetterPanelHandleCommandBarFocus(void* sender) {
    if (g_betterPanelDiscoveryUnloading || !sender) {
        return;
    }
    try {
        auto const& inspectable =
            *reinterpret_cast<wf::IInspectable const*>(sender);
        if (auto element =
                inspectable ? inspectable.try_as<UIElement>() : nullptr) {
            BetterPanelRememberDiscoveryAnchor(element);
            BetterPanelScheduleCurrentThreadDiscovery();
        }
    } catch (...) {
        Wh_Log(L"Focus discovery error %08X", winrt::to_hresult().value);
    }
}

void WINAPI BetterPanelCommandBarControl_GotFocusHandler_Hook(
    void* pThis, void* sender, void* args) {
    BetterPanelCommandBarControl_GotFocusHandler_Original(pThis, sender, args);
    BetterPanelHandleCommandBarFocus(sender);
}

void WINAPI BetterPanelCommandBarControl_Wave1_GotFocusHandler_Hook(
    void* pThis, void* sender, void* args) {
    BetterPanelCommandBarControl_Wave1_GotFocusHandler_Original(pThis, sender,
                                                                args);
    BetterPanelHandleCommandBarFocus(sender);
}

std::atomic<bool> g_betterPanelExplorerSymbolsHooked;

bool BetterPanelHookExplorerSymbols(HMODULE module) {
    // FileExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::DetailsPaneControl::OnLoaded(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::DetailsPaneControl::OnLoaded(struct winrt::Windows::Foundation::IInspectable const & __ptr64,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const & __ptr64) __ptr64)",
            },
            &BetterPanelDetailsPaneControl_OnLoaded_Original,
            BetterPanelDetailsPaneControl_OnLoaded_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::DetailsPaneControlVM::SelectionUpdated(struct winrt::WindowsUdk::UI::Shell::FileExplorerSelectionSource,struct winrt::Windows::Foundation::IInspectable))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::DetailsPaneControlVM::SelectionUpdated(struct winrt::WindowsUdk::UI::Shell::FileExplorerSelectionSource,struct winrt::Windows::Foundation::IInspectable) __ptr64)",
            },
            &BetterPanelDetailsPaneControlVM_SelectionUpdated_Original,
            BetterPanelDetailsPaneControlVM_SelectionUpdated_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const & __ptr64) __ptr64)",
            },
            &BetterPanelCommandBarManager_CommandBar_Original,
            BetterPanelCommandBarManager_CommandBar_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void) __ptr64)",
            },
            &BetterPanelCommandBarControl_OnApplyTemplate_Original,
            BetterPanelCommandBarControl_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void) __ptr64)",
            },
            &BetterPanelCommandBarControl_Wave1_OnApplyTemplate_Original,
            BetterPanelCommandBarControl_Wave1_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const & __ptr64,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const & __ptr64) __ptr64)",
            },
            &BetterPanelCommandBarControl_GotFocusHandler_Original,
            BetterPanelCommandBarControl_GotFocusHandler_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const & __ptr64,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const & __ptr64) __ptr64)",
            },
            &BetterPanelCommandBarControl_Wave1_GotFocusHandler_Original,
            BetterPanelCommandBarControl_Wave1_GotFocusHandler_Hook,
            true,
        },
    };

    if (!HookSymbols(module, hooks, ARRAYSIZE(hooks))) return false;
    return BetterPanelDetailsPaneControl_OnLoaded_Original ||
           BetterPanelDetailsPaneControlVM_SelectionUpdated_Original ||
           BetterPanelCommandBarManager_CommandBar_Original ||
           BetterPanelCommandBarControl_OnApplyTemplate_Original ||
           BetterPanelCommandBarControl_Wave1_OnApplyTemplate_Original;
}

bool BetterPanelHookExplorerSymbolsIfLoaded(bool applyHooks) {
    if (g_betterPanelExplorerSymbolsHooked) return true;
    HMODULE module = GetModuleHandleW(L"FileExplorerExtensions.dll");
    if (!module) return true;
    if (g_betterPanelExplorerSymbolsHooked.exchange(true)) return true;
    if (!BetterPanelHookExplorerSymbols(module)) {
        g_betterPanelExplorerSymbolsHooked = false;
        Wh_Log(L"FileExplorerExtensions discovery symbols unavailable");
        return false;
    }
    if (applyHooks) Wh_ApplyHookOperations();
    return true;
}

PFN_INITIALIZE_XAML_DIAGNOSTICS_EX InitializeXamlDiagnosticsEx_Original;
HRESULT WINAPI
InitializeXamlDiagnosticsEx_Hook(_In_ PCWSTR endPointName,
                                 _In_ DWORD pid,
                                 _In_ PCWSTR wszDllXamlDiagnostics,
                                 _In_ PCWSTR wszTAPDllName,
                                 _In_ CLSID tapClsid,
                                 _In_opt_ PCWSTR wszInitializationData) {
    if (g_inInjectWindhawkTAP) {
        return InitializeXamlDiagnosticsEx_Original(
            endPointName, pid, wszDllXamlDiagnostics, wszTAPDllName, tapClsid,
            wszInitializationData);
    }

    bool blockCall = false;

    switch (g_settings.xamlDiagnosticsHandling) {
        case XamlDiagnosticsHandling::kAlert: {
            void* retAddress = __builtin_return_address(0);

            WCHAR modulePath[MAX_PATH];
            PCWSTR modulePathStr = L"<unknown>";
            HMODULE module;
            if (GetModuleHandleEx(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCWSTR>(retAddress), &module)) {
                switch (GetModuleFileName(module, modulePath,
                                          ARRAYSIZE(modulePath))) {
                    case 0:
                    case ARRAYSIZE(modulePath):
                        break;

                    default:
                        modulePathStr = modulePath;
                        break;
                }
            }

            WCHAR message[1024];
            _snwprintf_s(
                message, _TRUNCATE,
                L"The following module is trying to use XAML diagnostics:\n\n"
                L"%s\n\n"
                L"There can only be one consumer at a time. Blocking it might "
                L"break that module, but allowing it might break this mod.\n\n"
                L"Do you want to block it?\n\n"
                L"Note: You can change this behavior in the mod settings.",
                modulePathStr);
            int result = MessageBox(
                    nullptr, message, L"Better Panel for Windows 11 - Windhawk",
                MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
            blockCall = (result == IDYES);
            break;
        }

        case XamlDiagnosticsHandling::kBlock:
            blockCall = true;
            break;

        case XamlDiagnosticsHandling::kAllow:
            blockCall = false;
            break;
    }

    if (blockCall) {
        Wh_Log(L"Blocking InitializeXamlDiagnosticsEx call");
        // Return success to avoid exception in the caller.
        return S_OK;
    }

    Wh_Log(L"Allowing InitializeXamlDiagnosticsEx call");
    return InitializeXamlDiagnosticsEx_Original(
        endPointName, pid, wszDllXamlDiagnostics, wszTAPDllName, tapClsid,
        wszInitializationData);
}

bool HookInitializeXamlDiagnosticsExIfNeeded() {
    if (InitializeXamlDiagnosticsEx_Original) {
        return false;  // Already hooked
    }

    const HMODULE wux = GetModuleHandle(L"Microsoft.Internal.FrameworkUdk.dll");
    if (!wux) {
        return false;  // DLL not loaded yet
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) {
        return false;
    }

    Wh_Log(L"Hooking InitializeXamlDiagnosticsEx to handle other consumers");
    return WindhawkUtils::SetFunctionHook(
        ixde, InitializeXamlDiagnosticsEx_Hook,
        &InitializeXamlDiagnosticsEx_Original);
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module && !g_betterPanelDiscoveryUnloading && lpLibFileName) {
        PCWSTR fileName = lpLibFileName;
        for (PCWSTR p = lpLibFileName; *p; ++p) {
            if (*p == L'\\' || *p == L'/') fileName = p + 1;
        }
        if (_wcsicmp(fileName, L"FileExplorerExtensions.dll") == 0 ||
            _wcsicmp(fileName, L"FileExplorerExtensions") == 0) {
            BetterPanelHookExplorerSymbolsIfLoaded(/*applyHooks=*/true);
        }
    }

    return module;
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

std::vector<HWND> GetTargetWnds() {
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

            if (GetTargetWindowType(hWnd) != TargetWindowType::None) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

using XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_t =
    HRESULT(WINAPI*)(void* pThis, SIZE* size);
XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_t
    XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Original;
HRESULT WINAPI
XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Hook(void* pThis,
                                                           SIZE* size) {
    Wh_Log(L">");

    HRESULT ret =
        XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Original(pThis,
                                                                       size);

    int explorerFrameContainerHeight = g_settings.explorerFrameContainerHeight;
    if (!explorerFrameContainerHeight) {
        explorerFrameContainerHeight = g_themeExplorerFrameContainerHeight;
    }

    if (SUCCEEDED(ret) && explorerFrameContainerHeight) {
        int originalCy = size->cy;
        size->cy = MulDiv(size->cy, explorerFrameContainerHeight, 136);
        Wh_Log(L"%d -> %d", originalCy, size->cy);
    }

    return ret;
}

bool HookWindowsUIFileExplorerSymbols() {
    HMODULE module = LoadLibraryEx(L"Windows.UI.FileExplorer.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load Windows.UI.FileExplorer.dll");
        return false;
    }

    // Windows.UI.FileExplorer.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlIslandViewAdapter::get_DesiredSizeInPhysicalPixels(struct tagSIZE *))"},
            &XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Original,
            XamlIslandViewAdapter_get_DesiredSizeInPhysicalPixels_Hook,
        },
    };

    if (!HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

PTP_TIMER g_statsTimer;

bool StartStatsTimer() {
    static constexpr WCHAR kStatsBaseUrl[] =
        L"https://github.com/ramensoftware/"
        L"windows-11-file-explorer-styling-guide/"
        L"releases/download/stats-v5/";

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

void LoadSettings() {
    // Legacy appearance settings are intentionally fixed to Windows defaults.
    g_settings.backgroundTranslucentEffectRegion =
        BackgroundTranslucentEffectRegion::kExplorerFrame;
    g_settings.backgroundTranslucentEffect.reset();
    g_settings.explorerFrameContainerHeight = 0;

    // Kept only for dormant legacy diagnostics code. Better Panel 2.0 doesn't
    // install that hook or occupy Explorer's XAML Diagnostics connection.
    g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kAllow;

    PCWSTR buttonLayoutMode =
        Wh_GetStringSetting(L"buttonLayout.mode");
    g_settings.buttonLayoutMode = BetterPanelButtonLayoutMode::Compact;
    if (buttonLayoutMode && wcscmp(buttonLayoutMode, L"comfort") == 0) {
        g_settings.buttonLayoutMode = BetterPanelButtonLayoutMode::Comfort;
    } else if (buttonLayoutMode && wcscmp(buttonLayoutMode, L"custom") == 0) {
        g_settings.buttonLayoutMode = BetterPanelButtonLayoutMode::Custom;
    }
    Wh_FreeStringSetting(buttonLayoutMode);
    g_settings.buttonHorizontalMargin = std::clamp(
        Wh_GetIntSetting(L"buttonLayout.horizontalMargin"), -20, 40);
    g_settings.buttonVerticalMargin = std::clamp(
        Wh_GetIntSetting(L"buttonLayout.verticalMargin"), -20, 40);
    g_settings.buttonHorizontalPadding = std::clamp(
        Wh_GetIntSetting(L"buttonLayout.horizontalPadding"), 0, 40);
    g_settings.buttonVerticalPadding = std::clamp(
        Wh_GetIntSetting(L"buttonLayout.verticalPadding"), 0, 30);
    g_settings.buttonMinimumWidth = std::clamp(
        Wh_GetIntSetting(L"buttonLayout.minimumWidth"), 0, 240);
    g_settings.buttonMinimumHeight = std::clamp(
        Wh_GetIntSetting(L"buttonLayout.minimumHeight"), 24, 80);
    g_settings.iconButtonSize = std::clamp(
        Wh_GetIntSetting(L"buttonLayout.iconButtonSize"), 15, 64);

    PCWSTR hoverAnimation = Wh_GetStringSetting(L"hoverAnimation.style");
    g_settings.hoverAnimation = BetterPanelHoverAnimation::WindowsDefault;
    if (hoverAnimation && wcscmp(hoverAnimation, L"none") == 0) {
        g_settings.hoverAnimation = BetterPanelHoverAnimation::None;
    } else if (hoverAnimation && wcscmp(hoverAnimation, L"scale") == 0) {
        g_settings.hoverAnimation = BetterPanelHoverAnimation::Scale;
    } else if (hoverAnimation && wcscmp(hoverAnimation, L"lift") == 0) {
        g_settings.hoverAnimation = BetterPanelHoverAnimation::Lift;
    } else if (hoverAnimation && wcscmp(hoverAnimation, L"tilt") == 0) {
        g_settings.hoverAnimation = BetterPanelHoverAnimation::Tilt;
    } else if (hoverAnimation && wcscmp(hoverAnimation, L"spring") == 0) {
        g_settings.hoverAnimation = BetterPanelHoverAnimation::Spring;
    }
    Wh_FreeStringSetting(hoverAnimation);

    PCWSTR clickAnimation = Wh_GetStringSetting(L"clickAnimation.style");
    g_settings.clickAnimation = BetterPanelClickAnimation::WindowsDefault;
    if (clickAnimation && wcscmp(clickAnimation, L"none") == 0) {
        g_settings.clickAnimation = BetterPanelClickAnimation::None;
    } else if (clickAnimation && wcscmp(clickAnimation, L"press") == 0) {
        g_settings.clickAnimation = BetterPanelClickAnimation::Press;
    } else if (clickAnimation && wcscmp(clickAnimation, L"pulse") == 0) {
        g_settings.clickAnimation = BetterPanelClickAnimation::Pulse;
    } else if (clickAnimation && wcscmp(clickAnimation, L"bounce") == 0) {
        g_settings.clickAnimation = BetterPanelClickAnimation::Bounce;
    } else if (clickAnimation && wcscmp(clickAnimation, L"flash") == 0) {
        g_settings.clickAnimation = BetterPanelClickAnimation::Flash;
    } else if (clickAnimation && wcscmp(clickAnimation, L"shake") == 0) {
        g_settings.clickAnimation = BetterPanelClickAnimation::Shake;
    } else if (clickAnimation && wcscmp(clickAnimation, L"spring") == 0) {
        g_settings.clickAnimation = BetterPanelClickAnimation::Spring;
    } else if (clickAnimation && wcscmp(clickAnimation, L"spin") == 0) {
        g_settings.clickAnimation = BetterPanelClickAnimation::Spin;
    } else if (clickAnimation && wcscmp(clickAnimation, L"squash") == 0) {
        g_settings.clickAnimation = BetterPanelClickAnimation::Squash;
    } else if (clickAnimation && wcscmp(clickAnimation, L"wobble") == 0) {
        g_settings.clickAnimation = BetterPanelClickAnimation::Wobble;
    }
    Wh_FreeStringSetting(clickAnimation);
    return;

#if 0
    PCWSTR backgroundTranslucentEffectRegion =
        Wh_GetStringSetting(L"backgroundTranslucentEffectRegion");
    g_settings.backgroundTranslucentEffectRegion =
        BackgroundTranslucentEffectRegion::kExplorerFrame;
    if (wcscmp(backgroundTranslucentEffectRegion, L"entireWindow") == 0) {
        g_settings.backgroundTranslucentEffectRegion =
            BackgroundTranslucentEffectRegion::kEntireWindow;
    }
    Wh_FreeStringSetting(backgroundTranslucentEffectRegion);

    PCWSTR backgroundTranslucentEffect =
        Wh_GetStringSetting(L"backgroundTranslucentEffect");
    g_settings.backgroundTranslucentEffect.reset();
    if (wcscmp(backgroundTranslucentEffect, L"default") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kDefault;
    } else if (wcscmp(backgroundTranslucentEffect, L"acrylic") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kAcrylic;
    } else if (wcscmp(backgroundTranslucentEffect, L"mica") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kMica;
    } else if (wcscmp(backgroundTranslucentEffect, L"micaAlt") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kMicaAlt;
    } else if (wcscmp(backgroundTranslucentEffect, L"none") == 0) {
        g_settings.backgroundTranslucentEffect =
            BackgroundTranslucentEffect::kNone;
    }
    Wh_FreeStringSetting(backgroundTranslucentEffect);

    g_settings.explorerFrameContainerHeight =
        Wh_GetIntSetting(L"explorerFrameContainerHeight");

    PCWSTR xamlDiagnosticsHandling =
        Wh_GetStringSetting(L"xamlDiagnosticsHandling");
    g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kAlert;
    if (wcscmp(xamlDiagnosticsHandling, L"block") == 0) {
        g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kBlock;
    } else if (wcscmp(xamlDiagnosticsHandling, L"allow") == 0) {
        g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kAllow;
    }
    Wh_FreeStringSetting(xamlDiagnosticsHandling);
#endif
}

void LoadThemeSettings() {
    g_themeBackgroundTranslucentEffect =
        BackgroundTranslucentEffect::kDefault;
    g_themeExplorerFrameContainerHeight = 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_betterPanelDiscoveryUnloading = false;

    LoadSettings();
    LoadThemeSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(GetMessageW, GetMessageW_Hook,
                                   &GetMessageW_Original);
    WindhawkUtils::SetFunctionHook(PeekMessageW, PeekMessageW_Hook,
                                   &PeekMessageW_Original);

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    // Hook Explorer's own WinUI lifecycle instead of taking the process-wide
    // XAML Diagnostics slot used by Windows 11 File Explorer Styler.
    BetterPanelHookExplorerSymbolsIfLoaded(/*applyHooks=*/false);

    HookWindowsUIFileExplorerSymbols();

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    BetterPanelHookExplorerSymbolsIfLoaded(/*applyHooks=*/true);

    auto hTargetWnds = GetTargetWnds();
    for (auto hTargetWnd : hTargetWnds) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hTargetWnd);
        RunFromWindowThread(
            hTargetWnd,
            [](PVOID param) {
                InitializeForCurrentThread();
                BetterPanelScheduleCurrentThreadDiscovery();

            },
            (PVOID)hTargetWnd);
    }

}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_betterPanelDiscoveryUnloading = true;

    BetterPanelCloseMedia();

    auto hTargetWnds = GetTargetWnds();
    for (auto hTargetWnd : hTargetWnds) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hTargetWnd);
        RunFromWindowThread(
            hTargetWnd,
            [](PVOID param) {
                UninitializeForCurrentThread();

            },
            (PVOID)hTargetWnd);
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

    LoadSettings();
    LoadThemeSettings();

    auto hTargetWnds = GetTargetWnds();
    for (auto hTargetWnd : hTargetWnds) {
        Wh_Log(L"Applying settings for %08X", (DWORD)(ULONG_PTR)hTargetWnd);
        RunFromWindowThread(
            hTargetWnd,
            [](PVOID) {
                BetterPanelApplyButtonSettingsForCurrentThread();
            },
            nullptr);
    }

}
