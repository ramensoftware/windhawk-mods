// ==WindhawkMod==
// @id              simple-window-switcher
// @name            Simple Window Switcher
// @description     Customizable Alt+Tab replacement with live previews, application grouping, multiple layouts, animations, and precision touchpad controls
// @version         3.0
// @author          Lone
// @github          https://github.com/Louis047
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -ldwmapi -luxtheme -lgdi32 -lshlwapi -loleaut32 -lole32 -lcomctl32 -lgdiplus -lversion -lwinmm -ladvapi32 -lmsimg32 -lhid -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Simple Window Switcher

A customizable Alt+Tab replacement for Windows, based on the
[Simple Window Switcher](https://github.com/valinet/sws) project, with additional
improvements by [Asteski](https://github.com/Asteski) and
[bropines](https://github.com/bropines).

## Features

- Live DWM window previews, with optional titles, icons, close buttons, and
  overflow indicators.
- Entrance, exit, selection, hover, scrolling, and layout-resize animations that
  respect the Windows animation setting.
- Solid/transparent, Acrylic, and Mica themes; light/dark color schemes; custom
  or accent colors; configurable fonts, highlights, shadows, and corner radii.
- Optional desktop-snapshot or wallpaper backdrop blur behind the switcher.
- Application grouping with individual-window drill-in, window-count badges,
  and configurable group closing and restoration.
- Recent-window ordering, live window-list updates, and options to hide, sort,
  or mark minimized windows.
- Multi-monitor display and filtering, DPI-aware sizing, and current/all virtual
  desktop filtering. All virtual desktops are included by default.
- Window exclusions by title or executable name, custom application names/icons,
  and an option to use the native switcher in Xbox Mode.
- Keyboard, mouse, scroll-wheel, and supported precision-touchpad controls.

## Layouts

Choose a layout under **Appearance → Layout → Switcher Layout**.

| Layout | Description |
| --- | --- |
| Default | Classic window grid with horizontal or vertical orientation and configurable header and thumbnail placement. |
| Badge (macOS-style) | Thumbnail cards with an overlaid application icon and a title above or below the preview. Requires thumbnails to be enabled. |
| Dock | An application-icon strip with an optional central preview of the selected window and configurable close-button and group-indicator placement. |

## Controls

### Keyboard and mouse

| Input | Action |
| --- | --- |
| Alt+Tab | Open the switcher and cycle forward; release Alt to activate the selection. |
| Alt+Shift+Tab | Cycle backward by default. The **Backward Shortcut** setting also offers Alt+Shift. |
| Alt+Ctrl+Tab | Open sticky mode, which stays open after releasing Alt. |
| Alt+Backtick | Cycle backward or cycle the current application's windows, according to **Alt+Backtick Behavior**. The current application's group expands automatically in same-app mode. |
| Win+Alt+Tab | Temporarily bypass the per-monitor window filter. |
| Tab / Shift+Tab, arrow keys | Navigate entries while the switcher is open. Shift+Tab follows the configured backward shortcut. |
| Page Up / Page Down | Navigate overflow pages. |
| Enter / Space, left-click an entry | Activate the selected or clicked window. |
| Esc | Leave an expanded application group, or dismiss the switcher. |
| Ctrl tap | Expand or collapse an application group when grouping is enabled. |
| Q / Ctrl+W / Delete | Close the selected entry. |
| Close button / middle-click | Close the entry under the pointer. |
| Click outside | Dismiss the switcher. |

Scroll-wheel navigation is disabled by default. Enable it under
**Accessibility → Scroll Wheel Activation** to change selection or scroll pages
from anywhere. The secondary modifier/action settings provide an alternate wheel
action; two-finger scrolling follows these same settings. Dragging a pressed
entry or close button cancels the click.

Close commands request a normal application close; applications can still ask to
save changes. Closing a grouped entry follows **Grouping → Group Close Button
Behavior**, which defaults to closing its most recent window.

### Touchpad

Enable **Touchpad → Enable Three-Finger Gestures** for swipe and tap controls on
a supported precision touchpad. **Sticky Switcher from Upward Swipe** controls
the upward launcher independently.

| Gesture | Action |
| --- | --- |
| Three-finger horizontal swipe while closed | Open normal mode, drag to navigate, and lift all fingers to activate the selection. |
| Three-finger upward swipe while closed | Open sticky mode when the upward launcher is enabled. |
| Three-finger drag while open | Navigate entries and rows without wrapping past the spatial edges. In sticky mode, lifting after a drag leaves the switcher open. |
| Fresh stationary three-finger tap in sticky mode | Activate the selection after lifting from the previous swipe. |
| Fresh upward swipe in sticky mode | Expand the selected application group, when grouping is enabled and it has multiple windows. |
| Fresh downward swipe in sticky mode | Leave an expanded group, or dismiss the main switcher without selecting a window. |
| Short stationary two-finger tap while open | Close the selected entry. |

In sticky mode, start a horizontal drag before turning vertically to navigate
rows without executing the fresh up/down commands. Outside a session, downward
swipes and three-finger taps retain their Windows actions; disabling the upward
launcher also leaves outside upward swipes to Windows. **Reverse Scroll
Direction** reverses navigation, not the physical up/down commands.

When normal Alt+Tab and a three-finger drag are used together, selection waits
for both Alt release and finger lift. Enter/click and Esc/click-away remain
available to select or cancel.

## Compatibility and limitations

- Touchpad swipe/tap controls require usable Raw HID contact reports. Legacy
  mouse-only touchpads are not supported by the gesture reader; device and driver
  reporting can affect recognition. Tap detection expects a short, stationary
  contact: movement or staggered finger placement/lift can prevent recognition.
- Suppressing Windows' three-finger actions during a session requires a supported
  foreground gesture controller. Additional Explorer-side filtering depends on
  matching Windows shell symbols and is available only on 64-bit architectures.
  Native Windows gestures can interfere when these mechanisms are unavailable;
  keyboard and mouse controls remain available.
- On systems supporting the touchpad-parameter API (documented for
  [Windows 11 24H2 and later](https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-touchpad_parameters_v1)),
  Windows' two-finger right-click tap is temporarily disabled during the active
  switcher session and its previous value is restored afterward. This is a live
  change, without saving a Windows preference. If the API is unavailable or the
  change fails, the fallback relies on receiving the corresponding mouse events;
  a tap outside the switcher can otherwise right-click or dismiss it.
- Mica requires Windows 11. The automatic theme uses Mica on Windows 11 and
  Acrylic on Windows 10; backdrop blur is a separate, optional setting.

## Configuration

- **Theme / Appearance**: colors, materials, layout, positioning, fonts, preview
  styling, and animation options.
- **Dimensions**: tile sizing, screen limits, padding, and shrink-to-fit behavior.
- **Grouping**: application grouping, displayed titles, restoration, and closing.
- **Accessibility**: invocation delay, navigation, monitor/desktop filtering, and
  minimized-window options.
- **Touchpad / Excluded Windows / Custom Header**: gesture controls, exclusion
  patterns, Xbox Mode exclusion, and per-application names/icons.

## Screenshots

### Layouts — Default

| Horizontal squared | Horizontal squared without thumbnails |
| :---: | :---: |
| ![Horizontal default](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/4.png) | ![Horizontal without thumbnails](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/3.png) |

| Vertical small rounded | Vertical large rounded |
| :---: | :---: |
| ![Vertical small](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/58449dc268347949193f2c67b0b042d287c20bd5/img/simple-window-switcher/1.png) | ![Vertical large](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/2.png) |

| Horizontal rounded with centered task icons and titles | Horizontal squared with thumbnails on top |
| :---: | :---: |
| ![Horizontal centered](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/6.png) | ![Horizontal squared with thumbnails on top](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/7.png) |

| Horizontal rounded with thumbnails and no icons |
| :---: |
| ![Horizontal rounded with thumbnails and no icons](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/simple-window-switcher/8.png) |

### Layouts — Badge (macOS-style)

*Screenshot placeholder — images to be added.*

### Layouts — Dock

*Screenshot placeholder — images to be added.*

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Style:
    - theme: auto
      $name: Style
      $description: Visual theme style for the switcher background.
      $options:
      - auto: Auto (Acrylic on Windows 10, Mica on Windows 11)
      - none: None (Solid/Transparent)
      - backdrop: Acrylic (Windows 10+)
      - mica: Mica Blur (Windows 11 only)
    - colorScheme: system
      $name: Color Scheme
      $options:
      - system: Follow system setting
      - light: Light
      - dark: Dark
    - backdropBlurEffect: off
      $name: Backdrop Blur
      $description: Blur the desktop behind the switcher while it is open. Off by default. Acrylic blurs a snapshot of the live desktop; Acrylic + Wallpaper blurs the desktop wallpaper instead and follows the shell's wallpaper fit mode.
      $options:
      - off: Off
      - acrylic: Acrylic Blur
      - acrylicWallpaper: Acrylic Blur + Wallpaper
    - backdropBlurOpacity: 18
      $name: Backdrop Dim Opacity (%)
      $description: How much the blurred backdrop is darkened (0-100). Higher = darker. 15-25 keeps the switcher readable while leaving the blur visible. Only applies when Backdrop Blur is enabled.
    - highlightStyle: auto
      $name: Task Highlight Style
      $description: Style used for the selected task row/tile. Auto uses Background fill only on Windows 11 and Border only on Windows 10.
      $options:
      - auto: Auto (Fill on Windows 11, Border on Windows 10)
      - border: Border only
      - fillAndBorder: Background fill and border
      - fillOnly: Background fill only
    - opacity: 65
      $name: Background Opacity
      $description: Background opacity percentage (0-100), applies to None and Acrylic themes for both light and dark themes. Default is 65 for Acrylic.
    - showSwitcherBorder: true
      $name: Show Switcher Border
      $description: Show a 1px border around the entire switcher window. Applies to all themes and corner styles.
    - DarkMode:
        - borderColorMode: default
          $name: Border Color
          $description: Color source for the selected/hovered task border in dark mode.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - highlightFillColorMode: default
          $name: Task Highlight Background Fill Color
          $description: Color source for the selected task background fill in dark mode.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - bgColorMode: default
          $name: Switcher Background Color
          $description: Color source for the switcher window background in dark mode. Applies to None and Acrylic themes.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - customBorderColor: "#FFFFFF"
          $name: Custom Border Color
          $description: HEX color value, used when Border Color is set to Custom.
        - customHighlightFillColor: "#FFFFFF"
          $name: Custom Task Highlight Background Fill Color
          $description: HEX color value, used when Task Highlight Background Fill Color is set to Custom.
        - customBgColor: "#202020"
          $name: Custom Switcher Background Color
          $description: HEX color value, used when Switcher Background Color is set to Custom.
        - iconBgColorMode: default
          $name: Badge Icon Background Color
          $description: Color source for the badge icon background pill in dark mode.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - customIconBgColor: "#000000"
          $name: Custom Badge Icon Background Color
          $description: HEX color value, used when Badge Icon Background Color is set to Custom.
        - iconBgOpacity: 55
          $name: Badge Icon Background Opacity
          $description: Opacity percentage (0-100) for the badge icon background in dark mode.
        - indicatorBgColorMode: accent
          $name: Group Indicator Background Color
          $description: Color source for the group indicator background pill in dark mode.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - customIndicatorBgColor: "#333333"
          $name: Custom Group Indicator Background Color
          $description: HEX color value, used when Group Indicator Background Color is set to Custom.
        - indicatorBgOpacity: 85
          $name: Group Indicator Background Opacity
          $description: Opacity percentage (0-100) for the group indicator background in dark mode.
        - indicatorTextColorMode: default
          $name: Group Indicator Text Color
          $description: Color source for the group indicator text in dark mode.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - customIndicatorTextColor: "#FFFFFF"
          $name: Custom Group Indicator Text Color
          $description: HEX color value, used when Group Indicator Text Color is set to Custom.
      $name: Dark Mode
    - LightMode:
        - borderColorMode: default
          $name: Border Color
          $description: Color source for the selected/hovered task border in light mode.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - highlightFillColorMode: default
          $name: Task Highlight Background Fill Color
          $description: Color source for the selected task background fill in light mode.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - bgColorMode: default
          $name: Switcher Background Color
          $description: Color source for the switcher window background in light mode. Applies to None and Acrylic themes.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - customBorderColor: "#000000"
          $name: Custom Border Color
          $description: HEX color value, used when Border Color is set to Custom.
        - customHighlightFillColor: "#000000"
          $name: Custom Task Highlight Background Fill Color
          $description: HEX color value, used when Task Highlight Background Fill Color is set to Custom.
        - customBgColor: "#F3F3F3"
          $name: Custom Switcher Background Color
          $description: HEX color value, used when Switcher Background Color is set to Custom.
        - iconBgColorMode: default
          $name: Badge Icon Background Color
          $description: Color source for the badge icon background pill in light mode.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - customIconBgColor: "#FFFFFF"
          $name: Custom Badge Icon Background Color
          $description: HEX color value, used when Badge Icon Background Color is set to Custom.
        - iconBgOpacity: 55
          $name: Badge Icon Background Opacity
          $description: Opacity percentage (0-100) for the badge icon background in light mode.
        - indicatorBgColorMode: accent
          $name: Group Indicator Background Color
          $description: Color source for the group indicator background pill in light mode.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - customIndicatorBgColor: "#EAEAEA"
          $name: Custom Group Indicator Background Color
          $description: HEX color value, used when Group Indicator Background Color is set to Custom.
        - indicatorBgOpacity: 85
          $name: Group Indicator Background Opacity
          $description: Opacity percentage (0-100) for the group indicator background in light mode.
        - indicatorTextColorMode: default
          $name: Group Indicator Text Color
          $description: Color source for the group indicator text in light mode.
          $options:
          - default: Default
          - custom: Custom
          - accent: Accent
        - customIndicatorTextColor: "#000000"
          $name: Custom Group Indicator Text Color
          $description: HEX color value, used when Group Indicator Text Color is set to Custom.
      $name: Light Mode
  $name: Theme
- Appearance:
    - Layout:
        - switcherLayout: default
          $name: Switcher Layout
          $description: Choose the active layout style for the window switcher.
          $options:
          - default: Default (Classic Grid)
          - badge: Badge Layout (macOS-style)
          - dock: Dock / Strip Layout
      $name: Layout
    - Corners:
        - cornerPreference: default
          $name: Corner Preference
          $description: Corner radius for the switcher window and its elements.
          $options:
          - default: Default (Let Windows decide)
          - none: Squared
          - round: Rounded
          - roundSmall: Rounded small
          - custom: Custom
        - customCornerRadius: 8
          $name: Custom Corner Radius (px)
          $description: Corner radius in pixels, used when Corner Preference is set to Custom. Applies to task borders, close buttons, and thumbnails.
        - taskRoundedCorners: auto
          $name: Round Task Borders and Close Button
          $description: Apply rounded corners to the selected task border and close button.
          $options:
          - auto: Auto (Disabled on Windows 10, Enabled on Windows 11)
          - true: Enabled
          - false: Disabled
        - roundThumbnailCorners: auto
          $name: Round Thumbnail Corners
          $description: Round the corners of window thumbnails. Uses the radius from Corner Preference.
          $options:
          - auto: Auto (Disabled on Windows 10, Enabled on Windows 11)
          - true: Enabled
          - false: Disabled
        - roundGroupIndicator: auto
          $name: Round Group Indicator
          $description: Round the corners of the group indicator. Uses the radius from Corner Preference.
          $options:
          - auto: Auto (Disabled on Windows 10, Enabled on Windows 11)
          - true: Enabled
          - false: Disabled
        - roundBadgeIconBackground: auto
          $name: Round Icon Background (only for badge-layout)
          $description: Round the corners of the badge icon background pill. Uses the radius from Corner Preference.
          $options:
          - auto: Auto (Disabled on Windows 10, Enabled on Windows 11)
          - true: Enabled
          - false: Disabled
      $name: Corners
    - Thumbnails:
        - thumbnailPosition: bottom
          $name: Thumbnail Position
          $description: Change the thumbnail position.
          $options:
          - bottom: Bottom
          - top: Top
          - left: Left
          - right: Right
        - thumbnailAlignment: left
          $name: Thumbnail Alignment
          $description: Align thumbnail content inside the available thumbnail area.
          $options:
          - left: Left
          - centered: Center
          - right: Right
        - showThumbnails: true
          $name: Show Thumbnails
          $description: Show DWM live thumbnail previews of windows.
        - showCloseButton: true
          $name: Show Close Button
          $description: Show the 'X' button on hover to close windows.
        - showCloseButtonBackground: auto
          $name: Close Button Background
          $description: Show an idle background plate behind the close button.
          $options:
          - auto: Auto (Disabled on Windows 10, Enabled on Windows 11)
          - true: Enabled
          - false: Disabled
        - showHoverBorder: true
          $name: Show Hover Border
          $description: Show a colored border around the thumbnail when hovered.
        - thumbnailHoverEffect: auto
          $name: Thumbnail Hover Effect
          $description: Visual effect applied to thumbnails when hovered with mouse. Auto uses Zoom on Windows 11 and Border on Windows 10.
          $options:
          - auto: Auto (Zoom on Windows 11, Border on Windows 10)
          - border: Border Outline
          - zoom: Zoom (Subtle Scale)
        - showThumbnailShadow: auto
          $name: Show Thumbnail Shadow
          $description: Draw a soft drop shadow behind each window thumbnail.
          $options:
          - auto: Auto (Disabled on Windows 10, Enabled on Windows 11)
          - true: Enabled
          - false: Disabled
      $name: Thumbnails
    - HeaderContent:
        - iconSize: small
          $name: Icon Size
          $description: Size of the header icon.
          $options:
          - small: Small (16x16)
          - medium: Medium (32x32)
          - large: Large (48x48)
          - xlarge: Extra Large (64x64)
        - showTitle: true
          $name: Show Title Label
        - showIcon: true
          $name: Show Icon
        - centerTaskContent: false
          $name: Center Task Icon and Title
          $description: Center the icon and title together in each task row.
      $name: Header Content
    - Orientation:
        - taskListOrientation: horizontal
          $name: Task List Orientation
          $description: Arrange tasks left-to-right or top-to-bottom.
          $options:
          - horizontal: Horizontal
          - vertical: Vertical
        - headerContentOrientation: horizontal
          $name: Header Content Orientation
          $description: Orientation of the task header icon and title.
          $options:
          - horizontal: Horizontal
          - vertical: Vertical
      $name: Orientation
    - Position:
        - switcherPosition: center
          $name: Switcher Position
          $description: Where the switcher should appear on the screen.
          $options:
          - topLeft: Top Left
          - topCenter: Top Center
          - topRight: Top Right
          - centerLeft: Center Left
          - center: Center
          - centerRight: Center Right
          - bottomLeft: Bottom Left
          - bottomCenter: Bottom Center
          - bottomRight: Bottom Right
        - switcherPositionMargin: 0
          $name: Switcher Position Margin (px)
          $description: Offset from the screen edges when using non-centered positions.
      $name: Position
    - BadgeLayout:
        - badgeIconPosition: bottomCenter
          $name: Badge Icon Position
          $description: Where to place the icon overlay on the thumbnail.
          $options:
          - topLeft: Top Left
          - topCenter: Top Center
          - topRight: Top Right
          - centerLeft: Center Left
          - center: Center
          - centerRight: Center Right
          - bottomLeft: Bottom Left
          - bottomCenter: Bottom Center
          - bottomRight: Bottom Right
        - badgeTitlePosition: bottom
          $name: Badge Title Position
          $description: Where to place the title label relative to the thumbnail.
          $options:
          - top: Above Thumbnail
          - bottom: Below Thumbnail
        - badgeIconSize: medium
          $name: Badge Icon Size
          $description: Size of the icon overlay in Badge Layout.
          $options:
          - small: Small (16x16)
          - medium: Medium (32x32 - Default)
          - large: Large (40x40)
          - xlarge: Extra Large (48x48)
        - showBadgeIconBackground: auto
          $name: Show Badge Icon Background
          $description: Draw a backdrop shape behind the badge icon. Auto is disabled on Windows 11 (drawing soft drop shadows) and enabled on Windows 10.
          $options:
          - auto: Auto (Disabled on Windows 11, Enabled on Windows 10)
          - true: Enabled
          - false: Disabled
        - showBadgeIconBackgroundShadow: auto
          $name: Show Badge Icon Background Shadow
          $description: Draw a soft drop shadow under the badge icon background pill. Auto is disabled on Windows 10.
          $options:
          - auto: Auto (Disabled on Windows 10, Enabled on Windows 11)
          - true: Enabled
          - false: Disabled
        - badgeIconPadding: 4
          $name: Badge Icon Padding (px)
          $description: Extra space between the icon and the edge of its background.
        - badgeIconOffsetX: 0
          $name: Badge Icon Offset X (px)
          $description: Nudge the icon horizontally from its default position.
        - badgeIconOffsetY: 0
          $name: Badge Icon Offset Y (px)
          $description: Nudge the icon vertically from its default position.
        - badgeSwitcherPadding: 20
          $name: Badge Layout Padding (px)
          $description: Padding between the switcher window border and the window entries in pixels for Badge Layout (before DPI scaling). Default 20.
      $name: Badge Layout Settings
      $description: Configuration options applied when Switcher Layout is set to Badge Layout (macOS-style).
    - DockLayout:
        - dockIconPosition: top
          $name: Icon Strip Position
          $description: Placement of the app icon strip. The window title is automatically placed on the opposite side.
          $options:
          - top: Top (Icons on Top, Title on Bottom)
          - bottom: Bottom (Icons on Bottom, Title on Top)
        - dockShowPreview: true
          $name: Show Central Live Preview
          $description: Display a live window thumbnail preview of the selected window in the middle between icons and title.
        - dockPreviewHeight: 280
          $name: Central Preview Max Height (px)
          $description: Maximum height for the central preview window. Aspect ratio of the selected window is preserved.
        - dockIconSize: '48'
          $name: Dock Icon Size (px)
          $description: Size of application icons in the strip.
          $options:
          - '32': 32px (Medium)
          - '40': 40px (Large)
          - '48': 48px (Extra Large - Default)
          - '64': 64px (Jumbo)
        - dockIconSpacing: 8
          $name: Icon Spacing (px)
          $description: Horizontal spacing between icon tiles in the strip.
        - dockHighlightStyle: fillOnly
          $name: Selection Highlight Style
          $description: Highlight style used for the selected icon in the dock strip.
          $options:
          - fillOnly: Background Fill (Default for Dock)
          - border: Border Outline
          - fillAndBorder: Fill and Border
        - dockMaxVisibleIcons: '7'
          $name: Max Visible Icons in Strip
          $description: Maximum number of window icons displayed simultaneously in the dock strip. Excess windows overflow and can be navigated with scrolling and chevrons. Set to 0 to fit screen width.
          $options:
          - '5': 5 Icons
          - '7': 7 Icons (Default)
          - '9': 9 Icons
          - '11': 11 Icons
          - '0': Fit Screen Width
        - dockSwitcherPadding: 11
          $name: Dock Layout Padding (px)
          $description: Padding between the switcher window border and dock elements in pixels for Dock Layout (before DPI scaling). Default 11.
        - dockCloseButtonPosition: topRight
          $name: Close Button Position
          $description: Placement of the close button inside the icon area in Dock Layout. Positioned on any side around the icon.
          $options:
          - topRight: Top-Right (Default)
          - topLeft: Top-Left
          - bottomRight: Bottom-Right
          - bottomLeft: Bottom-Left
          - top: Top-Center
          - bottom: Bottom-Center
          - left: Left-Center
          - right: Right-Center
          - hidden: Hidden
        - dockGroupIndicatorPosition: bottomRight
          $name: Group Indicator Position
          $description: Placement of the grouped window count indicator inside the icon area in Dock Layout when "Group Windows by Application" is enabled. Positioned on any side around the icon.
          $options:
          - bottomRight: Bottom-Right (Default)
          - bottomLeft: Bottom-Left
          - topRight: Top-Right
          - topLeft: Top-Left
          - top: Top-Center
          - bottom: Bottom-Center
          - left: Left-Center
          - right: Right-Center
          - hidden: Hidden
      $name: Dock Layout Settings
      $description: Configuration options applied when Switcher Layout is set to Dock / Strip Layout.
    - Font:
        - fontFamily: ""
          $name: Font Family
          $description: Font used for window titles. Leave blank to use system default font (Segoe UI Variable Text on Windows 11, Segoe UI on Windows 10).
        - fontSize: 0
          $name: Font Size
          $description: Size of the font in points. Set to 0 to use system default (10 on Windows 11, 9 on Windows 10).
        - fontStyle: regular
          $name: Font Style
          $options:
          - light: Light
          - regular: Regular
          - semibold: Semi-Bold
          - bold: Bold
          - italic: Italic
          - boldItalic: Bold Italic
        - applyToGroupIndicator: false
          $name: Apply to Group Indicator
          $description: Use these custom font settings for the grouped window count indicator badge.
      $name: Font
    - showOverflowIndicator: true
      $name: Show Overflow Indicator
      $description: Show chevron indicators at the edges when there are more windows off-screen.
    - Animations:
        - enableAnimations: auto
          $name: Enable Animations
          $description: Enable smooth WinUI-style animations for the switcher. Automatically respects Windows Accessibility animation settings.
          $options:
          - auto: Auto (Disabled on Windows 10, Enabled on Windows 11)
          - true: Enabled
          - false: Disabled
        - enableEntranceAnimation: true
          $name: Window Entrance Animation
          $description: Smooth fade and entrance transition when the switcher opens.
        - enableSelectionAnimation: true
          $name: Selection Highlight Animation
          $description: Smooth gliding transition when moving the selection highlight.
        - enableScrollAnimation: true
          $name: Row and Page Slide Animation
          $description: Smooth sliding transitions when scrolling overflown rows or switching pages.
        - enableHoverAnimation: true
          $name: Hover Focus Animation
          $description: Smooth fade and motion when hovering over tasks with the mouse.
      $name: Animations
  $name: Appearance
- Dimensions:
    - rowHeight: 230
      $name: Row Height
      $description: Total height of each thumbnail row in pixels (before DPI scaling). Default 230 matches ExplorerPatcher.
    - rowWidth: 0
      $name: Row Width
      $description: Width of each thumbnail tile in pixels (before DPI scaling). Set to 0 for automatic width based on window aspect ratio.
    - maxWidthPercent: 80
      $name: Maximum Width (percentage of screen width)
    - maxHeightPercent: 80
      $name: Maximum Height (percentage of screen height)
    - stretchThumbnailsToTaskWidth: true
      $name: Stretch Thumbnails to Task Width
      $description: When enabled, custom row width also changes thumbnail width. Disable to keep thumbnail aspect sizing while row width controls only task tile width.
    - autoFitTasks: false
      $name: Shrink Tasks to Fit
      $description: Automatically shrink task tiles (thumbnails and icons) in discrete steps as the number of visible windows grows, so more tasks stay visible without being pushed off-screen. Your Row Height and Icon Size act as the maximum size.
    - switcherPadding: 20
      $name: Default Layout Padding (px)
      $description: Padding between the switcher window border and the window entries in pixels for Default Layout (before DPI scaling). Default 20.
    - entryPadding: 16
      $name: Entry Inner Padding (px)
      $description: Padding between the task entry background (card border/fill) and its inner elements (thumbnail, icon, and title) in pixels (before DPI scaling). Default 16.
  $name: Dimensions
- Grouping:
    - showApplications: false
      $name: Group Windows by Application
      $description: Show one entry per application instead of one per window, similar to macOS Cmd+Tab. Selecting an application switches to its most recently used window. Tap Ctrl while an application is selected to expand it and show all of its windows as thumbnails.
    - showTitles: windowTitle
      $name: Show Titles
      $description: Which title text to display for each entry. Only applies when "Group Windows by Application" is enabled.
      $options:
      - windowTitle: Window Title
      - appName: Application Name
      - appNameWindowTitle: Application Name - Window Title
    - restoreAllWindows: false
      $name: Restore All Windows
      $description: When switching to an application, restore all of its minimized windows to their previous state. Only applies when "Group Windows by Application" is enabled. Tip - to act on a single window instead, tap Ctrl while the application is selected to show all of its windows and pick one.
    - showGroupIndicator: true
      $name: Show Group Indicator
      $description: Show a count badge on grouped application entries indicating how many windows are in the group. Only visible when Group Windows by Application is enabled.
    - showGroupIndicatorShadow: auto
      $name: Show Group Indicator Shadow
      $description: Show a soft drop shadow behind the group indicator badge. Auto enables shadows on Windows 11 and disables them on Windows 10 and lower.
      $options:
      - auto: Auto (Enabled on Windows 11, Disabled on Windows 10)
      - true: Enabled
      - false: Disabled
    - groupCloseBehavior: closeRecent
      $name: Group Close Button Behavior
      $description: Action when closing a grouped application entry.
      $options:
      - closeRecent: Close Most Recent Window
      - closeAll: Close All Windows
  $name: Grouping
- Accessibility:
    - showDelay: 0
      $name: Show Delay (ms)
      $description: Delay in milliseconds before showing the switcher. Setting 0 (default) enables an automatic 75ms rapid-switch grace period that cleanly switches windows without flashing the UI during fast Alt+Tab taps.
    - scrollWheelBehavior: never
      $name: Scroll Wheel Activation
      $description: When the scroll wheel should be active.
      $options:
      - never: Never
      - always: Always
      - stickyOnly: Only in sticky mode
    - scrollWheelAction: selection
      $name: Scroll Wheel Action
      $options:
      - selection: Change Selection
      - page: Scroll Pages
    - scrollSecondaryAction: page
      $name: Secondary Scroll Wheel Action (With Modifier)
      $options:
      - none: None
      - selection: Change Selection
      - page: Scroll Pages
    - scrollSecondaryModifier: shift
      $name: Secondary Scroll Wheel Modifier Key
      $options:
      - none: None
      - shift: Shift
      - ctrl: Ctrl
      - alt: Alt
    - reverseScrollDirection: false
      $name: Reverse Scroll Direction
    - backwardShortcut: altShiftTab
      $name: Backward Shortcut
      $description: Shortcut used to move backward in the switcher. Alt+Backtick is not listed here because it has its own action setting below.
      $options:
      - altShiftTab: Alt+Shift+Tab (default)
      - altShift: Alt+Shift
    - altBacktickBehavior: backward
      $name: Alt+Backtick Behavior
      $description: Action to perform when pressing Alt+` (Backtick).
      $options:
      - none: Disabled / Do Nothing
      - backward: Cycle Backward
      - sameApp: Cycle Between Windows of Current Application
    - switcherDisplayBehavior: cursorMonitor
      $name: Switcher Display Behavior
      $options:
      - primaryOnly: Primary Monitor Only
      - allMonitors: All Monitors
      - cursorMonitor: Monitor Based on Cursor Location
    - perMonitorWindows: false
      $name: Display Windows Only from the Monitor Containing the Cursor
    - virtualDesktopBehavior: allDesktops
      $name: Virtual Desktop Behavior
      $description: Choose which virtual desktops to show windows from.
      $options:
      - currentOnly: Show windows from current virtual desktop only
      - allDesktops: Show windows from all virtual desktops
    - hideMinimizedWindows: false
      $name: Hide Minimized Windows
      $description: Hide minimized windows from the switcher. When "Group Windows by Application" is enabled, an application is only hidden if all of its windows are minimized.
    - sortMinimizedWindowsToEnd: true
      $name: Sort Minimized Windows to the End
      $description: Sort minimized windows after all active windows. Disable this to keep minimized windows in their Z-order.
    - showMinimizedIndicator: false
      $name: Show Minimized Window Indicator
      $description: Display a visual indicator for minimized windows (especially helpful in Dock Layout where minimized windows have no active screen presence).
    - minimizedIndicatorStyle: dimIcon
      $name: Minimized Window Indicator Style
      $description: Visual style used to indicate minimized windows.
      $options:
      - dimIcon: Dimmed Icon (Translucent)
      - dot: Subtle Pill/Dot Indicator
      - badge: Minimize Badge
      - dimAndDot: Dimmed Icon + Dot Indicator
      - dimAndBadge: Dimmed Icon + Minimize Badge
    - minimizedIconOpacity: 55
      $name: Minimized Icon Opacity (%)
      $description: Opacity percentage applied to minimized window icons when using dimmed icon indicator styles (20-90%).
  $name: Accessibility
- Touchpad:
    - enabled: true
      $name: Enable Three-Finger Gestures
      $description: Read precision-touchpad Raw HID reports and request three-finger manipulation and action control while the switcher owns the foreground. Windows Task View, Show desktop, native switching, and three-finger tap actions are blocked during supported active sessions. Horizontal swipes open the normal switcher; drag to navigate and lift all fingers to select. In sticky mode, dragging never commits; lift and make a fresh stationary three-finger tap to select. No legacy fallback is used.
    - stickyLaunch: true
      $name: Sticky Switcher from Upward Swipe
      $description: "When enabled, an upward three-finger swipe opens the switcher in sticky mode. Outside a switcher session, downward swipes and taps keep their configured Windows actions. When disabled, Windows also keeps its default outside upward gesture. Horizontal swipes open normal mode in either setting and commit on lift. Gesture takeover during an active session requires a supported foreground-only Windows controller."
  $name: Touchpad
  $description: Raw HID three-finger support with an optional sticky upward-swipe launcher. Outside an active switcher session, the sticky launcher setting controls whether upward launch and the associated gesture suppression are enabled.
- ExcludedWindows:
    - excludeByTitle: ""
      $name: Exclude by Window Title
      $description: "Window title patterns to exclude, separated by ';' (wildcards supported: * matches any characters, ? matches one). Example: *Notepad*;*Chrome*"
    - excludeByExe: ""
      $name: Exclude by Executable Name
      $description: "Executable name patterns to exclude, separated by ';' (wildcards supported: * matches any characters, ? matches one). Example: notepad.exe;chrome.exe"
    - excludeXboxMode: false
      $name: Exclude in Xbox Mode (Gaming Full Screen Experience)
      $description: When enabled, Simple Window Switcher yields Alt+Tab to the native Windows switcher while Windows is running in Gaming Full Screen Experience (Xbox Mode).
  $name: Excluded Windows
  $description: Exclude specific windows from appearing in the switcher.
- customHeader:
  - - process: ""
      $name: Process Name
      $description: "Executable name to match (wildcards supported: * matches any characters, ? matches one). Example: chrome.exe or *code.exe"
    - iconPath: ""
      $name: Icon Path
      $description: "Full path to an icon source (.ico, .exe or .dll); the first icon in the file is used. Leave empty to keep the default icon. Example: C:\\Icons\\myapp.ico"
    - appName: ""
      $name: Application Name
      $description: "Custom name to display for matching tasks, replacing the detected application name. Leave empty to keep the default. Shown in the 'App name' and 'App name + Window title' title modes (requires 'Group Windows by Application' to be enabled)."
  $name: Custom Header
  $description: Assign a custom icon and/or application name to tasks based on their executable name. The first matching rule wins.

*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windows.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propkey.h>
#include <shobjidl.h>
#include <knownfolders.h>
#include <windowsx.h>
#include <commctrl.h>
#include <inspectable.h>
#include <roapi.h>
#include <appmodel.h>
#include <hidusage.h>
#include <hidpi.h>
#include <vector>
#include <atomic>
#include <deque>
#include <memory>
#include <exception>
#include <map>
#include <string>
#include <algorithm>
#include <gdiplus.h>
#include <windhawk_utils.h>

#define SWS_CLASSNAME       L"WindhawkSWS_Switcher"
#define SWS_MAIN_WINDOW_TITLE L"WindhawkSWS_InputEndpoint"
#define SWS_ICON_SIZE       16
// Lower bound (pre-DPI px) for the auto-fit "Shrink tasks to fit" row height so
// thumbnails never collapse to an unusable size.
#define SWS_AUTOFIT_MIN_ROWHEIGHT 90
// EP-style nested padding layers (before DPI scaling)
#define SWS_ELEMENT_PAD_TOP     5   // Vertical margin between cell border and content
#define SWS_ELEMENT_PAD_BOTTOM  5
#define SWS_ELEMENT_PAD_LEFT    2   // Horizontal margin between cell border and content
#define SWS_ELEMENT_PAD_RIGHT   2
#define SWS_PAD_DIVIDER         7   // Vertical divider between title row and thumbnail
#define SWS_ROW_TITLE_HEIGHT    30  // Height of icon+title row
#define SWS_MAX_TILE_ASPECT     2.0 // Max thumbnail width = thumbH * this
#define SWS_CONTOUR_SIZE        2
#define SWS_HOTKEY_ALTTAB           1
#define SWS_HOTKEY_ALTSHIFTTAB      2
#define SWS_HOTKEY_ALTCTRLTAB       3
#define SWS_HOTKEY_ALTSHIFTCTRLTAB  4
#define SWS_HOTKEY_ALTBACKTICK      5
#define SWS_HOTKEY_WINALTTAB        6
#define SWS_HOTKEY_WINALTSHIFTTAB   7
#define SWS_HOTKEY_ALTBACKTICK_UK   8
#define SWS_HOTKEY_RETRY_TIMER_ID   100
#define SWS_HOTKEY_RETRY_INTERVAL   2000
#define SWS_BG_DARK          RGB(32, 32, 32)
#define SWS_BG_LIGHT         RGB(243, 243, 243)
#define SWS_CONTOUR_DARK     RGB(255, 255, 255)
#define SWS_CONTOUR_LIGHT    RGB(0, 0, 0)
#define SWS_TEXT_DARK         RGB(255, 255, 255)
#define SWS_TEXT_LIGHT        RGB(0, 0, 0)
#define SWS_SHOW_DELAY_TIMER_ID 101
#define SWS_ALT_POLL_TIMER_ID   102
#define SWS_CLOSE_VERIFY_TIMER_ID 103
#define SWS_TOUCHPAD_IDLE_TIMER_ID 105
#define SWS_DYNAMIC_RESIZE_TIMER_ID 106
// Explorer-side hook retry. Symbol lookup can fail while Windhawk is still loading symbols,
// so the Explorer IPC window keeps retrying until CTray::_RaiseDesktop is hooked.
#define SWS_EXPLORER_HOOK_RETRY_TIMER_ID 108
// Keep the invisible foreground shield alive through the release edge of a
// three-finger action before returning focus to the application underneath.
#define SWS_TOUCHPAD_SHIELD_RELEASE_TIMER_ID 109
// A foreground handoff can complete asynchronously, especially across an
// elevated foreground window. Retry it briefly while the raw stroke remains
// active instead of releasing ownership after one foreground check.
#define SWS_TOUCHPAD_SHIELD_FOCUS_RETRY_TIMER_ID 110
// Retry the selected-window activation briefly when foreground-lock rules reject
// the first handoff after a raw touchpad session.
#define SWS_TOUCHPAD_TARGET_FOCUS_RETRY_TIMER_ID 112
// Marker expiry never extends the invisible foreground shield's lifetime.
#define SWS_RAW_SWIPE_MARKER_EXPIRY_TIMER_ID 111
// Short post-commit polling closes the gap when Start reuses an existing shell
// window without emitting a usable OBJECT_SHOW notification.
// Captures show 1828/1937 ms gaps between active reports before Show Desktop
// runs, not after lift. Cover those gaps without claiming a lost reader forever.
#define SWS_RAW_SWIPE_OWNER_MS 3000
// The confirmed post-lift call arrived 184 ms after release. Keep a separate,
// shorter marker-only grace; its physical expiry is measured from the lift.
#define SWS_RAW_SWIPE_LIFT_GRACE_MS 1000
// The backstop must not end a raw session while the fingers are still down.
// Every frame refreshes this, so only a lost reader or a missed lift can reach it.
#define SWS_RAW_SESSION_LOST_TIMEOUT_MS 4000
// A stationary two-finger tap is owned only by a visible, interactive session.
// Keep its promoted mouse pair briefly after lift, without retaining foreground.
#define SWS_RAW_TAP_MAX_MS 300
#define SWS_RAW_TAP_SLOP (65535 / 50)
#define SWS_RAW_TWO_TAP_MOUSE_GRACE_MS 250u
#define SWS_RAW_TWO_TAP_MOUSE_PAIR_MS 500u
#define SWS_RAW_TWO_TAP_MOUSE_TIMER_ID 115
#define SWS_TWO_FINGER_TAP_RESTORE_TIMER_ID 116
// Posted by the low-level mouse hook so the heavy CycleLinear work runs in the
// wndproc instead of on the synchronous raw-input path. WPARAM is the direction.
#define WM_SWS_SCROLL           (WM_APP + 1)
#define WM_SWS_SETTINGS_CHANGED (WM_APP + 2)
#define WM_SWS_TOUCHPAD_READER_CHANGED (WM_APP + 3)
#define WM_SWS_CANCEL_INPUT (WM_APP + 4)
#define WM_SWS_ICON_READY (WM_APP + 5)
#define WM_SWS_TOUCHPAD_DIAGNOSTICS (WM_APP + 6)
#define WM_SWS_TOUCHPAD_READER_DIAGNOSTICS (WM_APP + 7)
#define WM_SWS_NATIVE_TOUCHPAD (WM_APP + 8)
// Pointer-free IPC: WPARAM carries a 29-bit UI epoch and a 3-bit boundary
// (Start left/up/right = 1/2/3, End = 5, Cancel = 6); LPARAM is a stroke token.
#define SWS_NATIVE_TOUCHPAD_END 5u
#define SWS_NATIVE_TOUCHPAD_CANCEL 6u
#define SWS_NATIVE_TOUCHPAD_START_MAX_AGE_MS 1000u
#define SWS_NATIVE_TOUCHPAD_COMPLETION_TIMER_ID 114
// Let the reader adopt reports after a foreground/context handoff. This is a
// single bounded wait, not a synthetic lift or a foreground retry loop.
#define SWS_NATIVE_TOUCHPAD_HANDOFF_MS 50u
#define SWS_NATIVE_TOUCHPAD_CANDIDATE_WAIT_MS 20u
#define SWS_RAW_SWIPE_PROP L"WindhawkSWSRawSwipe"
#define SWS_RAW_SWIPE_SESSION_PROP L"WindhawkSWSRawSwipeSession"
#define SWS_RAW_SWIPE_UP_PROP L"WindhawkSWSRawSwipeUp"
#define SWS_RAW_THREE_CANDIDATE_PROP L"WindhawkSWSRawThreeCandidate"
#define SWS_RAW_THREE_CANDIDATE_MS 1000u
// The tool publishes readiness independently of raw direction classification.
// Bits 0/1 declare enabled/usable reader, bit 2 enables outside sticky-up.
// Higher bits are an epoch: disable/reader loss invalidates existing decisions.
#define SWS_NATIVE_SWIPE_POLICY_PROP L"WindhawkSWSNativeSwipePolicy"
#define SWS_NATIVE_SWIPE_PROFILE_PROP L"WindhawkSWSNativeSwipeProfile"
#define SWS_NATIVE_SWIPE_ACTIVE_PROP L"WindhawkSWSNativeSwipeActive"
#define SWS_NATIVE_TOUCHPAD_EPOCH_PROP L"WindhawkSWSNativeTouchpadEpoch"
// Explorer publishes source-gate availability on its own taskbar window so
// both processes can retire the source-ambiguous window-show recovery path.
#define SWS_NATIVE_SWIPE_GATE_PROP L"WindhawkSWSNativeSwipeGate"
// The generic marker keeps the full 31-bit timestamp range. A separate session
// property distinguishes active switcher ownership from an outside sticky-launch
// candidate without reducing wraparound coverage.
#define SWS_RAW_SWIPE_LIFT_FLAG 0x80000000u
#define SWS_RAW_SWIPE_TIMESTAMP_MASK 0x7FFFFFFFu
#ifndef EVENT_SYSTEM_FOREGROUND
#define EVENT_SYSTEM_FOREGROUND 0x0003
#endif

typedef BOOL (WINAPI *IsShellWindow_t)(HWND);
typedef HWND (WINAPI *GhostWindowFromHungWindow_t)(HWND);
struct ACCENT_POLICY { DWORD AccentState; DWORD AccentFlags; DWORD GradientColor; DWORD AnimationId; };
struct WINDOWCOMPOSITIONATTRIBDATA { DWORD dwAttrib; PVOID pvData; SIZE_T cbData; };
typedef BOOL(WINAPI *SetWindowCompositionAttribute_t)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

#ifndef ZBID_SYSTEM_TOOLS
#define ZBID_SYSTEM_TOOLS 16
#endif

typedef HWND (WINAPI *CreateWindowInBand_t)(
    DWORD dwExStyle,
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
    LPVOID lpParam,
    DWORD dwBand
);

static HWND CreateSWSWindow(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                            int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                            HINSTANCE hInstance, LPVOID lpParam) {
    static auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "CreateWindowInBand");
    if (pCreateWindowInBand) {
        HWND hWnd = pCreateWindowInBand(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam,
                                        ZBID_SYSTEM_TOOLS);
        if (hWnd) return hWnd;
    }
    return CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle,
                           X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

struct MotionTrackClock {
    LARGE_INTEGER origin = {};
    float initial = 0.0f;
    float lastProgress = 0.0f;
    bool started = false;
};

struct OpacityMotionTrack {
    MotionTrackClock clock;
    float progress = 1.0f;
    float start = 0.0f;
    float target = 0.0f;
};

struct ItemTransitionMotion {
    OpacityMotionTrack opacity;
    MotionTrackClock scaleClock;
    float scaleProgress = 1.0f;
    float scaleFrom = 1.0f;
    float scaleTo = 1.0f;
};

struct OwnedWindowIcon {
    HICON handle;
    explicit OwnedWindowIcon(HICON icon) : handle(icon) {}
    ~OwnedWindowIcon() { if (handle) DestroyIcon(handle); }
};

// Shared by live entries and transition snapshots; only the UI changes a cell.
struct WindowIconCell {
    HWND hWnd = NULL;
    DWORD processId = 0, threadId = 0;
    int sizePx = 0;
    ULONGLONG settingsGeneration = 0, requestGeneration = 0;
    ULONGLONG retryAfter = 0;
    bool pending = false;
    bool lastFailed = false;
    std::atomic<ULONGLONG> discardedGeneration{0};
    std::atomic<bool> retired{false};
    std::shared_ptr<OwnedWindowIcon> icon;
};

struct WindowEntry {
    HWND hWnd; HICON hIcon; WCHAR title[256]; std::map<HWND, HTHUMBNAIL> hThumbs;
    std::shared_ptr<WindowIconCell> iconCell;
    RECT rcCell; RECT rcThumbActual; RECT rcThumbSlot;
    SIZE sourceSize;           // Raw DWM surface size
    RECT rcSourceCrop;         // Source crop rect for DWM_TNP_RECTSOURCE
    SIZE effectiveSourceSize;  // Source size after cropping invisible frame
    std::vector<HWND> groupWindows;  // All app windows when grouping by application
    int drawnIconX;            // X coordinate where the icon is drawn
    int drawnIconY;            // Y coordinate where the icon is drawn
    int drawnIconSz;           // Size of the drawn icon
    RECT rcCellStart = {};
    RECT rcCellTarget = {};
    RECT rcThumbStart = {};
    RECT rcThumbTarget = {};
    RECT rcThumbSlotStart = {};
    RECT rcThumbSlotTarget = {};
    RECT rcCellLayoutCurrent = {};
    RECT rcThumbLayoutCurrent = {};
    bool isNewEntry = false;
    float enterAlpha = 1.0f;
    float enterScale = 1.0f;
    ItemTransitionMotion entryMotion;
    float closeBtnAlpha = 0.0f;
    OpacityMotionTrack closeBtnMotion;
    float closeBtnScale = 0.85f;
    float closeBtnScaleStart = 0.85f;
    float closeBtnScaleTarget = 0.85f;
    float closeBtnScaleProgress = 1.0f;
    MotionTrackClock closeBtnScaleClock;
    float hoverScale = 1.0f;
    float hoverScaleStart = 1.0f;
    float hoverScaleTarget = 1.0f;
    float hoverScaleProgress = 1.0f;
    float hoverScaleDuration = 0.167f;
    MotionTrackClock hoverScaleClock;
};
struct Settings {
    WCHAR theme[32]; WCHAR colorScheme[32]; WCHAR cornerPreference[32]; WCHAR scrollWheelBehavior[32]; WCHAR scrollWheelAction[32]; WCHAR scrollSecondaryAction[32]; WCHAR scrollSecondaryModifier[32]; WCHAR taskListOrientation[32]; WCHAR headerContentOrientation[32]; WCHAR iconSize[32]; WCHAR backwardShortcut[32]; WCHAR altBacktickBehavior[32]; WCHAR thumbnailPosition[32]; WCHAR thumbnailAlignment[32]; WCHAR switcherDisplayBehavior[32];
    WCHAR virtualDesktopBehavior[32];
    WCHAR backdropBlurEffect[32];
    int backdropBlurOpacity;
    // Global theme settings (apply to both light and dark)
    WCHAR highlightStyle[32]; int opacity; bool showSwitcherBorder;
    // Dark Mode color settings
    WCHAR borderColorModeDark[16]; WCHAR highlightFillColorModeDark[16]; WCHAR bgColorModeDark[16]; WCHAR iconBgColorModeDark[16];
    WCHAR customBorderColorDark[16]; WCHAR customHighlightFillColorDark[16]; WCHAR customBgColorDark[16]; WCHAR customIconBgColorDark[16];
    int iconBgOpacityDark;
    WCHAR indicatorBgColorModeDark[16]; WCHAR customIndicatorBgColorDark[16]; int indicatorBgOpacityDark;
    WCHAR indicatorTextColorModeDark[16]; WCHAR customIndicatorTextColorDark[16];
    // Light Mode color settings
    WCHAR borderColorModeLight[16]; WCHAR highlightFillColorModeLight[16]; WCHAR bgColorModeLight[16]; WCHAR iconBgColorModeLight[16];
    WCHAR customBorderColorLight[16]; WCHAR customHighlightFillColorLight[16]; WCHAR customBgColorLight[16]; WCHAR customIconBgColorLight[16];
    int iconBgOpacityLight;
    WCHAR indicatorBgColorModeLight[16]; WCHAR customIndicatorBgColorLight[16]; int indicatorBgOpacityLight;
    WCHAR indicatorTextColorModeLight[16]; WCHAR customIndicatorTextColorLight[16];
    WCHAR fontFamily[64]; WCHAR fontStyle[32];
    int fontSize;
    bool applyToGroupIndicator;
    int rowHeight;
    int rowWidth;
    bool stretchThumbnailsToTaskWidth;
    bool showThumbnails;
    bool showCloseButton;
    bool showCloseButtonBackground;
    bool showOverflowIndicator;
    bool showTitle;
    bool showIcon;
    int maxWidthPercent;
    bool autoFitTasks;
    int maxHeightPercent; int showDelay;
    int switcherPadding;
    int defaultSwitcherPadding;
    int badgeSwitcherPadding;
    int dockSwitcherPadding;
    int entryPadding;
    bool perMonitorWindows; bool taskRoundedCorners; bool roundThumbnailCorners; bool roundGroupIndicator; bool roundBadgeIconBackground; bool reverseScrollDirection;
    bool centerTaskContent;
    bool showApplications;
    WCHAR showTitles[32];
    bool restoreAllWindows;
    bool hideMinimizedWindows;
    bool sortMinimizedWindowsToEnd;
    bool showMinimizedIndicator;
    WCHAR minimizedIndicatorStyle[32];
    int minimizedIconOpacity;
    int customCornerRadius;
    WCHAR switcherPosition[32];
    int switcherPositionMargin;
    bool showHoverBorder;
    WCHAR thumbnailHoverEffect[32];
    bool showThumbnailShadow;
    // Master layout
    WCHAR switcherLayout[32];
    // Badge layout (macOS-style)
    WCHAR badgeIconPosition[32];
    WCHAR badgeTitlePosition[16];
    WCHAR badgeIconSize[16];
    bool showBadgeIconBackground; bool showBadgeIconBackgroundShadow;
    int badgeIconPadding;
    int badgeIconOffsetX;
    int badgeIconOffsetY;
    // Dock layout
    WCHAR dockIconPosition[16];
    bool dockShowPreview;
    int dockPreviewHeight;
    int dockIconSize;
    int dockIconSpacing;
    WCHAR dockHighlightStyle[32];
    int dockMaxVisibleIcons;
    WCHAR dockCloseButtonPosition[32];
    WCHAR dockGroupIndicatorPosition[32];
    // Grouped indicator
    bool showGroupIndicator;
    bool showGroupIndicatorShadow;
    WCHAR groupCloseBehavior[16];
    // Animations
    bool enableAnimations;
    bool enableEntranceAnimation;
    bool enableSelectionAnimation;
    bool enableScrollAnimation;
    bool enableHoverAnimation;
    bool excludeXboxMode;
    bool handleTouchpadGestures;
    bool stickyTouchpadMode;
};

static std::vector<std::wstring> g_excludeTitlePatterns;
static std::vector<std::wstring> g_excludeExePatterns;
static std::vector<HWND> g_hMirrorSwitchers;

static HWND g_hSwitcher = NULL;
static HWND g_hBackdropWnd = NULL; // Backdrop Blur full-screen window (state block further below)
static HWND g_hCloseBtnWnd = NULL;

static bool IsSwitcherWindow(HWND hWnd) {
    if (!hWnd) return false;
    if (hWnd == g_hSwitcher || hWnd == g_hCloseBtnWnd || hWnd == g_hBackdropWnd) return true;
    for (HWND h : g_hMirrorSwitchers) {
        if (hWnd == h) return true;
    }
    return false;
}

using GetWindowBand_t = BOOL(WINAPI*)(HWND hWnd, PDWORD pdwBand);
static GetWindowBand_t pGetWindowBand = nullptr;

using GetThreadDescription_t = HRESULT(WINAPI*)(HANDLE hThread, PWSTR* ppszThreadDescription);
static GetThreadDescription_t pGetThreadDescription = nullptr;

#ifndef ZBID_SYSTEM_TOOLS
#define ZBID_SYSTEM_TOOLS 16
#endif

static bool IsNativeAltTabWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;

    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (!hRoot) hRoot = hWnd;

    WCHAR cls[64] = {0};
    if (!GetClassNameW(hRoot, cls, ARRAYSIZE(cls))) return false;

    bool isIsland = (wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0);
    bool isMultiView = (wcscmp(cls, L"MultitaskingViewFrame") == 0);
    bool isClassic = (wcscmp(cls, L"TaskSwitcherWnd") == 0);

    if (!isIsland && !isMultiView && !isClassic) {
        return false;
    }

    // Classic Windows 7 switcher class is dedicated
    if (isClassic) return true;

    // 1. Process check: ensure the window belongs to explorer.exe
    DWORD pid = 0;
    GetWindowThreadProcessId(hRoot, &pid);
    if (pid != GetCurrentProcessId()) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!hProcess) return false;
        WCHAR path[MAX_PATH] = {0};
        DWORD size = MAX_PATH;
        bool isExplorer = false;
        if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
            WCHAR* fileName = PathFindFileNameW(path);
            if (fileName && _wcsicmp(fileName, L"explorer.exe") == 0) {
                isExplorer = true;
            }
        }
        CloseHandle(hProcess);
        if (!isExplorer) return false;
    }

    // 2. Window Title check: Alt+Tab is always titled L"Task Switching"
    WCHAR title[64] = {0};
    GetWindowTextW(hRoot, title, ARRAYSIZE(title));
    if (wcscmp(title, L"Task Switching") != 0) {
        if (isIsland || isMultiView) {
            Wh_Log(L"SWS: Alt+Tab/TaskView candidate rejected (title): class=%s title='%s'", cls, title);
        }
        return false;
    }

    // 3. Z-Band check: Alt+Tab is strictly assigned to ZBID_SYSTEM_TOOLS (16)
    if (!pGetWindowBand) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            pGetWindowBand = (GetWindowBand_t)GetProcAddress(hUser32, "GetWindowBand");
        }
    }
    if (pGetWindowBand) {
        DWORD band = 0;
        if (pGetWindowBand(hRoot, &band) && band != ZBID_SYSTEM_TOOLS) {
            if (isIsland || isMultiView) {
                Wh_Log(L"SWS: Alt+Tab/TaskView candidate rejected (band=%u): class=%s title='%s'", band, cls, title);
            }
            return false;
        }
    }

    // 4. Thread Description check for Win11 XAML Island host
    if (isIsland) {
        if (!pGetThreadDescription) {
            HMODULE hKernel = GetModuleHandleW(L"kernelbase.dll");
            if (!hKernel) hKernel = GetModuleHandleW(L"kernel32.dll");
            if (hKernel) {
                pGetThreadDescription = (GetThreadDescription_t)GetProcAddress(hKernel, "GetThreadDescription");
            }
        }
        if (pGetThreadDescription) {
            DWORD tid = GetWindowThreadProcessId(hRoot, nullptr);
            if (tid) {
                HANDLE hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
                if (hThread) {
                    PWSTR desc = nullptr;
                    if (SUCCEEDED(pGetThreadDescription(hThread, &desc)) && desc) {
                        bool isMultitasking = (wcscmp(desc, L"MultitaskingView") == 0);
                        if (!isMultitasking) {
                            Wh_Log(L"SWS: Alt+Tab/TaskView candidate rejected (thread description='%s'): class=%s title='%s'", desc, cls, title);
                        }
                        LocalFree(desc);
                        if (!isMultitasking) {
                            CloseHandle(hThread);
                            return false;
                        }
                    }
                    CloseHandle(hThread);
                }
            }
        }
    }

    Wh_Log(L"SWS: Alt+Tab/TaskView match: class=%s title='%s'", cls, title);
    return true;
}

static bool IsNativeSwitcherWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (!hRoot) hRoot = hWnd;
    return IsNativeAltTabWindow(hRoot);
}
static IVirtualDesktopManager* g_pVirtualDesktopManager = NULL;
static bool g_showAllMonitors = false;
static HHOOK g_hMouseHook = NULL;
static HWINEVENTHOOK s_hWinEventHook = NULL;
static HWINEVENTHOOK s_hForegroundEventHook = NULL;
static void AddWindowEntry(HWND hWnd);
static void CloseSwitcherEntry(int idx);
static void CALLBACK WinEventShowHideProc(HWINEVENTHOOK hHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
static std::vector<WindowEntry> g_windows;
static int g_selectedIndex = 0, g_hoverIndex = -1;
static int g_hoverThumbIndex = -1;
static bool g_isPaginatedView = false;
static bool g_isDryRunLayout = false;
static HWND g_hoverWnd = NULL;
static int g_layoutStartIndex = 0; // EP-style: first window index visible in the layout
// App drill-in: when grouping by application, Ctrl drills into the selected app's
// windows. The grouped app list is stashed here so it can be restored on exit.
static bool g_drilledIn = false;
static std::vector<WindowEntry> g_savedAppList;
static int g_savedSelectedIndex = 0;
static int g_savedLayoutStartIndex = 0;
static bool g_consumeEscUp = false;
static bool g_ctrlTapPending = false;
// Deferred auto-drill for Alt+Backtick "sameApp" + grouping (issue #5532): set when
// the switcher is in its show-delay pending phase, consumed by RevealPendingSwitcher.
static bool g_drillInAfterReveal = false;
static bool g_isVisible = false, g_isSticky = false, g_isDarkMode = false;
static bool g_isHidingSwitcher = false;
static bool g_recoveringShellFocus = false;
static HFONT g_hFont = NULL;
static HTHEME g_hTheme = NULL;
static UINT g_shellHookMsg = 0;
static int g_dpiX = 96, g_dpiY = 96;
// Auto-fit ("Shrink tasks to fit") scale percentage applied to row/thumbnail
// height and icon size. 100 = no shrink. Recomputed in ComputeLayout from the
// visible task count when the setting is enabled.
static int g_autoFitScalePct = 100;
static int g_winW = 0, g_winH = 0;
static int g_activePadDivider = 0;
static bool g_hotkeysRegistered = false;
static bool g_isAltBacktickSameApp = false;
static WCHAR g_sameAppSessionKey[MAX_PATH] = {};
static HMONITOR g_hCurrentMonitor = NULL;
static Settings g_settings;

// ============================================================================
// Backdrop Blur (opt-in): a full-screen, click-through window shown *behind*
// the switcher while it is open.
//   - "acrylic"          : one-shot blurred snapshot of the live desktop
//   - "acrylicWallpaper" : blurred desktop wallpaper (shell's own placement)
// Both are dimmed by Style.backdropBlurOpacity and painted by us. DWM's own
// materials are not usable for this window: it is never activated, and on
// Windows 11 the acrylic accent/system backdrop then fall back to an opaque
// sheet whose tint ignores Style.backdropBlurOpacity (verified on 25H2).
// The blurred bitmap is built once per show, never per frame. Off by default;
// zero cost when disabled.
// ============================================================================
#define SWS_BACKDROP_CLASSNAME L"WindhawkSWS_Backdrop"
// Presentation timing does not change any theme's per-pixel alpha/material.
#define SWS_PRESENTATION_ANIMATION_MS 167
// A small shared travel distance keeps the reveal/exit visible on non-layered
// Mica/Acrylic windows, where a global HWND alpha is not available.
#define SWS_PRESENTATION_OFFSET_PX 6
// g_hBackdropWnd is declared above, near the other switcher window handles.
static bool g_backdropClassRegistered = false;
static Gdiplus::Bitmap* g_backdropBitmap = NULL; // blurred backdrop, dim veil baked in
static HDC g_backdropPaintDC = NULL;
static HBITMAP g_backdropPaintBitmap = NULL;
static HBITMAP g_backdropPaintOldBitmap = NULL;
static SIZE g_backdropPaintSize = {};

// The plate is fully applied before the switcher reveal. Its independent
// 83 ms exit fade starts only after every switcher surface has been hidden.
static float g_backdropFadeAlpha = 0.0f;
static OpacityMotionTrack g_backdropOpacity;
static bool g_backdropExitFadeActive = false;

static bool AreAnimationsGloballyEnabled(); // defined further down
static bool IsWin11OrGreater();             // defined further down
static void StartOpacityMotion(OpacityMotionTrack& track, float current, float target);
static bool StepOpacityMotion(OpacityMotionTrack& track, float& current, float dt);

static bool BackdropBlurEnabled() { return wcscmp(g_settings.backdropBlurEffect, L"off") != 0; }
static bool BackdropBlurUsesWallpaper() { return wcscmp(g_settings.backdropBlurEffect, L"acrylicWallpaper") == 0; }

static void BackdropFreeBitmap() {
    if (g_backdropPaintDC && g_backdropPaintOldBitmap) {
        SelectObject(g_backdropPaintDC, g_backdropPaintOldBitmap);
    }
    if (g_backdropPaintBitmap) DeleteObject(g_backdropPaintBitmap);
    if (g_backdropPaintDC) DeleteDC(g_backdropPaintDC);
    g_backdropPaintDC = NULL;
    g_backdropPaintBitmap = g_backdropPaintOldBitmap = NULL;
    g_backdropPaintSize = {};
    if (g_backdropBitmap) { delete g_backdropBitmap; g_backdropBitmap = NULL; }
}

// Resample once while the switcher is still hidden. A full-desktop bicubic
// DrawImage inside WM_PAINT can occupy several reveal frames, especially on
// high-resolution/multi-monitor desktops and on the repaint queued by show.
static bool PrepareBackdropSurface(int w, int h) {
    if (!g_backdropBitmap || w <= 0 || h <= 0) return false;
    g_backdropPaintDC = CreateCompatibleDC(NULL);
    if (!g_backdropPaintDC) {
        BackdropFreeBitmap();
        return false;
    }
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = NULL;
    g_backdropPaintBitmap = CreateDIBSection(g_backdropPaintDC, &bmi,
                                            DIB_RGB_COLORS, &bits, NULL, 0);
    if (!g_backdropPaintBitmap || !bits) {
        BackdropFreeBitmap();
        return false;
    }
    HGDIOBJ oldBitmap = SelectObject(g_backdropPaintDC, g_backdropPaintBitmap);
    if (!oldBitmap || oldBitmap == HGDI_ERROR) {
        BackdropFreeBitmap();
        return false;
    }
    g_backdropPaintOldBitmap = (HBITMAP)oldBitmap;
    PatBlt(g_backdropPaintDC, 0, 0, w, h, BLACKNESS);
    GdiFlush();
    Gdiplus::Status status;
    {
        Gdiplus::Graphics graphics(g_backdropPaintDC);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        status = graphics.DrawImage(g_backdropBitmap, 0, 0, w, h);
    }
    if (status != Gdiplus::Ok) {
        BackdropFreeBitmap();
        return false;
    }
    g_backdropPaintSize = { w, h };
    return true;
}

static void BackdropApplyAlpha(HWND hWnd, float alpha) {
    if (!hWnd || !IsWindow(hWnd)) return;
    g_backdropFadeAlpha = alpha;
    SetLayeredWindowAttributes(hWnd, 0, (BYTE)(alpha * 255.0f + 0.5f), LWA_ALPHA);
}

static void BackdropStopFade() {
    g_backdropExitFadeActive = false;
    g_backdropOpacity = {};
}

static void BackdropStartExitFade(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    StartOpacityMotion(g_backdropOpacity, g_backdropFadeAlpha, 0.0f);
    g_backdropExitFadeActive = true;
}

static bool BackdropFadeTick(float dt) {
    if (!g_backdropExitFadeActive || !g_hBackdropWnd ||
        !IsWindowVisible(g_hBackdropWnd)) return false;
    bool active = StepOpacityMotion(g_backdropOpacity, g_backdropFadeAlpha, dt);
    BackdropApplyAlpha(g_hBackdropWnd, g_backdropFadeAlpha);
    return active;
}

static LRESULT CALLBACK BackdropWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT; // fully click-through
        case WM_ERASEBKGND:
            return 1; // never let GDI flash a black background
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (g_backdropPaintDC) {
                const RECT& dirty = ps.rcPaint;
                BitBlt(hdc, dirty.left, dirty.top,
                       dirty.right - dirty.left, dirty.bottom - dirty.top,
                       g_backdropPaintDC, dirty.left, dirty.top, SRCCOPY);
            }
            // No bitmap (build failed): paint nothing, the window stays transparent
            // and the real desktop shows through.
            EndPaint(hWnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static void EnsureBackdropWindow() {
    if (g_hBackdropWnd) return;
    if (!g_backdropClassRegistered) {
        WNDCLASSEXW wc = { sizeof(wc) };
        wc.lpfnWndProc = BackdropWndProc;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.lpszClassName = SWS_BACKDROP_CLASSNAME;
        wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
        if (RegisterClassExW(&wc)) g_backdropClassRegistered = true;
    }
    if (!g_backdropClassRegistered) return;
    // WS_EX_LAYERED carries the fade-in/out (LWA_ALPHA); the content is painted opaque.
    g_hBackdropWnd = CreateSWSWindow(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        SWS_BACKDROP_CLASSNAME, L"",
        WS_POPUP, 0, 0, 0, 0, NULL, NULL, GetModuleHandleW(NULL), NULL);
    if (!g_hBackdropWnd) return;
    // Never let DWM round a full-screen overlay: rounded corners would leave the four
    // corner cut-outs showing the sharp desktop.
    if (IsWin11OrGreater()) {
        int corner = 1; // DWMWCP_DONOTROUND
        DwmSetWindowAttribute(g_hBackdropWnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */,
                              &corner, sizeof(corner));
    }
    BackdropApplyAlpha(g_hBackdropWnd, 0.0f);
}

// Backdrop sources are built at half resolution: cheap enough for a once-per-show
// blur, and detailed enough that the box passes below read as a smooth gaussian
// instead of as blocks.
#define SWS_BACKDROP_DOWNSCALE 2
#define SWS_BACKDROP_BLUR_RADIUS 24 // px at downscaled res (~48 px on screen)
#define SWS_BACKDROP_BLUR_PASSES 3  // three box passes approximate a gaussian

static Gdiplus::Bitmap* BackdropAllocCanvas(int w, int h, int& sw, int& sh) {
    sw = w / SWS_BACKDROP_DOWNSCALE > 0 ? w / SWS_BACKDROP_DOWNSCALE : 1;
    sh = h / SWS_BACKDROP_DOWNSCALE > 0 ? h / SWS_BACKDROP_DOWNSCALE : 1;
    return new Gdiplus::Bitmap(sw, sh, PixelFormat32bppARGB);
}

// One box-blur pass over `len` samples that are `step` bytes apart. Running sums keep
// it O(len) per line (no inner loop over the kernel), so multiple passes stay cheap.
static void BackdropBoxBlurLine(const BYTE* base, BYTE* out, int len, size_t step, int R) {
    int sum[4] = {0, 0, 0, 0};
    int count = 0;
    int seed = (R < len - 1) ? R : len - 1;
    for (int k = 0; k <= seed; k++) {
        const BYTE* p = base + (size_t)k * step;
        sum[0] += p[0]; sum[1] += p[1]; sum[2] += p[2]; sum[3] += p[3];
        count++;
    }
    for (int i = 0; i < len; i++) {
        BYTE* o = out + (size_t)i * step;
        o[0] = (BYTE)(sum[0] / count); o[1] = (BYTE)(sum[1] / count);
        o[2] = (BYTE)(sum[2] / count); o[3] = (BYTE)(sum[3] / count);
        int add = i + R + 1, rem = i - R;
        if (add < len) {
            const BYTE* p = base + (size_t)add * step;
            sum[0] += p[0]; sum[1] += p[1]; sum[2] += p[2]; sum[3] += p[3];
            count++;
        }
        if (rem >= 0) {
            const BYTE* p = base + (size_t)rem * step;
            sum[0] -= p[0]; sum[1] -= p[1]; sum[2] -= p[2]; sum[3] -= p[3];
            count--;
        }
    }
}

// Blur the 32bpp canvas in place: SWS_BACKDROP_BLUR_PASSES separable box passes, which
// approximate a gaussian without the blocky plateaus a single pass leaves behind (and
// MinGW has no GDI+ 1.1 effect classes). The dim veil is baked in afterwards so the
// backdrop is one opaque bitmap. Runs once per show, never per frame.
static void BackdropBlurAndVeil(Gdiplus::Bitmap* bmp, int sw, int sh) {
    const int R = SWS_BACKDROP_BLUR_RADIUS;
    Gdiplus::Rect full(0, 0, sw, sh);
    Gdiplus::BitmapData bd = {};
    if (bmp->LockBits(&full, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite,
                      PixelFormat32bppARGB, &bd) == Gdiplus::Ok && bd.Scan0) {
        const size_t stride = (size_t)bd.Stride;
        std::vector<BYTE> src(stride * sh), dst(stride * sh);
        memcpy(src.data(), bd.Scan0, stride * sh);
        for (int pass = 0; pass < SWS_BACKDROP_BLUR_PASSES; pass++) {
            for (int y = 0; y < sh; y++) {
                BackdropBoxBlurLine(src.data() + (size_t)y * stride, dst.data() + (size_t)y * stride,
                                    sw, 4, R);
            }
            for (int x = 0; x < sw; x++) {
                BackdropBoxBlurLine(dst.data() + (size_t)x * 4, src.data() + (size_t)x * 4,
                                    sh, stride, R);
            }
        }
        memcpy(bd.Scan0, src.data(), stride * sh);
        bmp->UnlockBits(&bd);
    }

    Gdiplus::Graphics g(bmp);
    int a = g_settings.backdropBlurOpacity * 255 / 100;
    Gdiplus::SolidBrush veil(Gdiplus::Color(a, 0, 0, 0));
    g.FillRectangle(&veil, full);
}

// Place the wallpaper inside one target rect (already in canvas units) the way the
// shell does. 0 center, 1 tile, 2 stretch, 6 fit, anything else = fill (cover).
static void BackdropDrawImageInRect(Gdiplus::Graphics& g, Gdiplus::Image& src,
                                    const Gdiplus::RectF& dst, DWORD style) {
    float iw = (float)src.GetWidth() / SWS_BACKDROP_DOWNSCALE;
    float ih = (float)src.GetHeight() / SWS_BACKDROP_DOWNSCALE;
    if (iw <= 0.0f || ih <= 0.0f || dst.Width <= 0.0f || dst.Height <= 0.0f) return;
    struct ScopedClip {
        Gdiplus::Graphics& graphics;
        Gdiplus::GraphicsState state;
        ~ScopedClip() { graphics.Restore(state); }
    } clip{g, g.Save()};
    g.SetClip(dst, Gdiplus::CombineModeIntersect);
    if (style == 2) { // Stretch
        g.DrawImage(&src, dst);
        return;
    }
    if (style == 0 || style == 1) { // Center / Tile keep the wallpaper's own size
        float x0 = dst.X + (dst.Width - iw) / 2.0f, y0 = dst.Y + (dst.Height - ih) / 2.0f;
        if (style == 0) {
            g.DrawImage(&src, Gdiplus::RectF(x0, y0, iw, ih));
            return;
        }
        for (float y = dst.Y; y < dst.Y + dst.Height; y += ih)
            for (float x = dst.X; x < dst.X + dst.Width; x += iw)
                g.DrawImage(&src, Gdiplus::RectF(x, y, iw, ih));
        return;
    }
    // Fit (6) keeps the whole image visible, Fill (default) covers the rect.
    float scale = (style == 6) ? (dst.Width / iw < dst.Height / ih ? dst.Width / iw : dst.Height / ih)
                               : (dst.Width / iw > dst.Height / ih ? dst.Width / iw : dst.Height / ih);
    float dw = iw * scale, dh = ih * scale;
    g.DrawImage(&src, Gdiplus::RectF(dst.X + (dst.Width - dw) / 2.0f,
                                     dst.Y + (dst.Height - dh) / 2.0f, dw, dh));
}

struct BackdropMonitorCtx {
    Gdiplus::Graphics* g;
    Gdiplus::Image* src;
    DWORD style;
    int originX, originY;
};
static BackdropMonitorCtx s_backdropMonitorCtx;

static BOOL CALLBACK BackdropMonitorEnumProc(HMONITOR hMon, HDC, LPRECT, LPARAM) {
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfoW(hMon, &mi)) {
        float d = (float)SWS_BACKDROP_DOWNSCALE;
        Gdiplus::RectF dst((mi.rcMonitor.left - s_backdropMonitorCtx.originX) / d,
                           (mi.rcMonitor.top - s_backdropMonitorCtx.originY) / d,
                           (mi.rcMonitor.right - mi.rcMonitor.left) / d,
                           (mi.rcMonitor.bottom - mi.rcMonitor.top) / d);
        BackdropDrawImageInRect(*s_backdropMonitorCtx.g, *s_backdropMonitorCtx.src, dst,
                                s_backdropMonitorCtx.style);
    }
    return TRUE;
}

// Build the blurred wallpaper bitmap once per show ("wallpaper" mode).
static void BuildBackdropWallpaper(int w, int h) {
    BackdropFreeBitmap();
    if (w <= 0 || h <= 0) return;
    WCHAR wp[MAX_PATH] = L"";
    if (!SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, wp, 0) || !wp[0]) return;
    Gdiplus::Bitmap src(wp);
    if (src.GetLastStatus() != Gdiplus::Ok || src.GetWidth() == 0 || src.GetHeight() == 0) return;

    // HKCU\Control Panel\Desktop: 0 center, 1 tile, 2 stretch, 6 fit, 10 fill, 22 span.
    DWORD style = 10, tile = 0;
    auto readWallpaperNumber = [](PCWSTR name, DWORD fallback) {
        WCHAR value[16] = {};
        DWORD size = sizeof(value);
        if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", name,
                         RRF_RT_REG_SZ, nullptr, value, &size) != ERROR_SUCCESS) return fallback;
        WCHAR* end = nullptr;
        unsigned long number = wcstoul(value, &end, 10);
        return end != value && *end == L'\0' ? (DWORD)number : fallback;
    };
    style = readWallpaperNumber(L"WallpaperStyle", style);
    tile = readWallpaperNumber(L"TileWallpaper", tile);
    if (tile == 1) style = 1; // "Tile" checkbox overrides the fit style

    int sw = 0, sh = 0;
    Gdiplus::Bitmap* small = BackdropAllocCanvas(w, h, sw, sh);
    {
        Gdiplus::Graphics g(small);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.Clear(Gdiplus::Color(255, 0, 0, 0));
        if (style == 22) { // Span: one image across the whole virtual screen
            BackdropDrawImageInRect(g, src, Gdiplus::RectF(0.0f, 0.0f, (float)sw, (float)sh), 10);
        } else {
            s_backdropMonitorCtx = { &g, &src, style,
                                     GetSystemMetrics(SM_XVIRTUALSCREEN),
                                     GetSystemMetrics(SM_YVIRTUALSCREEN) };
            EnumDisplayMonitors(NULL, NULL, BackdropMonitorEnumProc, 0);
        }
    }
    BackdropBlurAndVeil(small, sw, sh);
    g_backdropBitmap = small;
}

// Acrylic mode: blur a one-shot snapshot of the real desktop. The capture happens
// while the switcher's own windows are still hidden, so the switcher never ends up
// in its own backdrop.
static void BuildBackdropSnapshot(int w, int h) {
    BackdropFreeBitmap();
    if (w <= 0 || h <= 0) return;
    // Runs before any SWS window is shown (see PrepareBackdropBlur), so the capture is
    // the clean desktop. No DwmFlush here on purpose: it can block the switcher thread
    // indefinitely, and that thread owns the Alt+Tab hotkeys and the low-level mouse
    // hook, so a blocked thread freezes input system-wide.
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    HDC hScreen = GetDC(NULL);
    if (!hScreen) return;
    HDC hMem = CreateCompatibleDC(hScreen);
    HBITMAP hBmp = hMem ? CreateCompatibleBitmap(hScreen, w, h) : NULL;
    BOOL captured = FALSE;
    if (hBmp) {
        HGDIOBJ hOld = SelectObject(hMem, hBmp);
        captured = BitBlt(hMem, 0, 0, w, h, hScreen, vx, vy, SRCCOPY);
        SelectObject(hMem, hOld); // deselect before handing it to GDI+
    }
    int sw = 0, sh = 0;
    Gdiplus::Bitmap* small = NULL;
    if (captured) {
        small = BackdropAllocCanvas(w, h, sw, sh);
        Gdiplus::Bitmap shot(hBmp, NULL); // borrowed handle, hBmp stays alive below
        Gdiplus::Graphics g(small);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.DrawImage(&shot, 0, 0, sw, sh);
    }
    if (hBmp) DeleteObject(hBmp);
    if (hMem) DeleteDC(hMem);
    ReleaseDC(NULL, hScreen);
    if (!small) return;
    BackdropBlurAndVeil(small, sw, sh);
    g_backdropBitmap = small;
}

// Show the backdrop that PrepareBackdropBlur() already built, directly below the
// switcher. Presentation of the switcher windows does not depend on this function or
// on any timer: the caller presents them right after, so a backdrop that is up without
// a switcher on top of it can never be left behind on screen.
static void ShowBackdropBlur() {
    if (!BackdropBlurEnabled()) return;
    EnsureBackdropWindow();
    if (!g_hBackdropWnd || !g_backdropBitmap || !g_backdropPaintDC) return;

    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (g_backdropPaintSize.cx != w || g_backdropPaintSize.cy != h) return;

    // Native switcher materials are visible even at frame-zero. Finish the
    // backdrop first, so their reveal cannot get ahead of the desktop blur.
    BackdropStopFade();
    BackdropApplyAlpha(g_hBackdropWnd, 1.0f);
    InvalidateRect(g_hBackdropWnd, NULL, FALSE);

    // Directly below the switcher: the switcher sits in the shell's system-tools band,
    // so this is above every app window yet still behind the switcher.
    SetWindowPos(g_hBackdropWnd, g_hSwitcher ? g_hSwitcher : HWND_TOPMOST, vx, vy, w, h,
                 SWP_NOACTIVATE);
    UpdateWindow(g_hBackdropWnd);
    ShowWindow(g_hBackdropWnd, SW_SHOWNA);
    // Showing a layered HWND can invalidate it again. Drain that initial
    // cached paint before PresentSwitcherWindows starts the reveal clocks.
    UpdateWindow(g_hBackdropWnd);
}

// Called only after the switcher exit has retired its visible surfaces.
static bool FadeOutBackdropBlur() {
    if (!g_hBackdropWnd || !IsWindowVisible(g_hBackdropWnd) ||
        !AreAnimationsGloballyEnabled() || g_backdropFadeAlpha <= 0.0f) return false;
    BackdropStartExitFade(g_hBackdropWnd);
    return g_backdropExitFadeActive;
}

static void HideBackdropBlur() {
    if (g_hBackdropWnd && IsWindow(g_hBackdropWnd)) {
        BackdropStopFade();
        ShowWindow(g_hBackdropWnd, SW_HIDE);
    }
    BackdropFreeBitmap();
}

// Build the blurred backdrop once per show. Runs at the very start of a show, before any
// SWS window is shown or moved, so the capture is the clean desktop and nothing of ours
// can end up inside its own backdrop. Doing it here also keeps the capture/blur off the
// reveal path, which runs while the switcher thread owns the low-level mouse hook. If
// the show fails afterwards, the prepared bitmap is simply never shown and HideSwitcher
// frees it.
static void PrepareBackdropBlur() {
    if (!BackdropBlurEnabled()) return;
    if (g_hBackdropWnd && IsWindowVisible(g_hBackdropWnd)) {
        // Stale backdrop from an interrupted session: never let it into the capture.
        HideBackdropBlur();
    }
    if (g_hSwitcher && IsWindowVisible(g_hSwitcher)) {
        // Native touchpad foreground handoff briefly shows the switcher at
        // (-32000, -32000) before this capture. That staging window is outside
        // the virtual desktop and cannot contaminate the snapshot; only skip
        // when a visible SWS window actually intersects a display.
        RECT switcherRect = {}, virtualRect = {
            GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN),
            GetSystemMetrics(SM_XVIRTUALSCREEN) + GetSystemMetrics(SM_CXVIRTUALSCREEN),
            GetSystemMetrics(SM_YVIRTUALSCREEN) + GetSystemMetrics(SM_CYVIRTUALSCREEN)};
        RECT overlap = {};
        if (GetWindowRect(g_hSwitcher, &switcherRect) &&
            IntersectRect(&overlap, &switcherRect, &virtualRect)) {
            // Unexpected state: skip the blur rather than capture a partial
            // switcher into its own backdrop.
            return;
        }
    }
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (BackdropBlurUsesWallpaper()) {
        BuildBackdropWallpaper(w, h);
    } else {
        BuildBackdropSnapshot(w, h);
    }
    PrepareBackdropSurface(w, h);
}

static void DestroyBackdropWindow() {
    BackdropFreeBitmap();
    if (g_hBackdropWnd) {
        BackdropStopFade();
        if (IsWindow(g_hBackdropWnd)) DestroyWindow(g_hBackdropWnd);
        g_hBackdropWnd = NULL;
    }
}
static HANDLE g_hSwitcherThread = NULL;
static DWORD g_dwSwitcherThreadId = 0;
static bool g_isExplorer = false;
static HANDLE g_restartExplorerPromptThread = NULL;
static std::atomic<HWND> g_restartExplorerPromptWindow{nullptr};
static IsShellWindow_t g_IsShellManagedWindow = nullptr;
static IsShellWindow_t g_IsShellFrameWindow = nullptr;
static GhostWindowFromHungWindow_t g_GhostWindowFromHungWindow = nullptr;
static GhostWindowFromHungWindow_t g_HungWindowFromGhostWindow = nullptr;
static SetWindowCompositionAttribute_t g_SetWindowCompositionAttribute = nullptr;
static ULONG_PTR g_gdiplusToken = 0;
static bool g_isCloseHovered = false;
static float g_animCloseBtnAlpha = 0.0f;
static float g_animCloseBtnHoverAlpha = 0.0f;
static bool g_isClosePressed = false;
static HANDLE g_explorerIpcThread = NULL;
static DWORD g_explorerIpcThreadId = 0;
static bool g_isPendingShow = false;
static RECT g_pendingSwitcherRect = {0, 0, 0, 0};
static int g_switcherBaseX = 0;
static int g_switcherBaseY = 0;
static bool g_switcherBaseInitialized = false;
static bool g_isWin11OrGreater = false;

static bool IsWin11OrGreater() {
    static int s_cached = -1;
    if (s_cached != -1) return s_cached == 1;
    if (g_isWin11OrGreater) {
        s_cached = 1;
        return true;
    }
    DWORD sz = 0;
    if (RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                     L"CurrentBuildNumber", RRF_RT_REG_SZ, NULL, NULL, &sz) == ERROR_SUCCESS && sz > 0) {
        std::wstring buf(sz / sizeof(WCHAR), L'\0');
        if (RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                         L"CurrentBuildNumber", RRF_RT_REG_SZ, NULL, &buf[0], &sz) == ERROR_SUCCESS) {
            s_cached = (_wtoi(buf.c_str()) >= 22000) ? 1 : 0;
            g_isWin11OrGreater = (s_cached == 1);
            return s_cached == 1;
        }
    }
    s_cached = 0;
    return false;
}

static int g_systemDwmRadius = 8;
static int g_systemDwmSmallRadius = 4;
static bool g_customCornerRadiusModActive = false;
static HANDLE g_hDwmCornerWatchThread = NULL;
static HANDLE g_hDwmCornerWatchStopEvent = NULL;

static bool DetectSystemDwmCornerRadiusValues(int* outRadius, int* outSmallRadius) {
    const WCHAR* modKeys[] = {
        L"SOFTWARE\\Windhawk\\Engine\\Mods\\custom-corner-radius",
        L"SOFTWARE\\Windhawk\\Engine\\Mods\\local@custom-corner-radius"
    };

    for (const auto* modKeyPath : modKeys) {
        HKEY hKey = NULL;
        LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, modKeyPath, 0, KEY_READ | KEY_WOW64_64KEY, &hKey);
        if (lRes != ERROR_SUCCESS) {
            continue;
        }

        DWORD disabled = 0, sz = sizeof(disabled);
        bool modActive = true;
        if (RegQueryValueExW(hKey, L"Disabled", NULL, NULL, (LPBYTE)&disabled, &sz) == ERROR_SUCCESS) {
            modActive = (disabled == 0);
        }
        RegCloseKey(hKey);

        if (modActive) {
            WCHAR settingsPath[MAX_PATH];
            swprintf_s(settingsPath, L"%s\\Settings", modKeyPath);
            HKEY hSetKey = NULL;
            lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, settingsPath, 0, KEY_READ | KEY_WOW64_64KEY, &hSetKey);
            if (lRes == ERROR_SUCCESS) {
                DWORD rVal = (DWORD)-1, szVal = sizeof(rVal);
                LONG rRes = RegQueryValueExW(hSetKey, L"radius", NULL, NULL, (LPBYTE)&rVal, &szVal);

                DWORD sVal = (DWORD)-1;
                szVal = sizeof(sVal);
                LONG sRes = RegQueryValueExW(hSetKey, L"smallRadius", NULL, NULL, (LPBYTE)&sVal, &szVal);
                RegCloseKey(hSetKey);

                int finalRadius = 8;
                if (rRes == ERROR_SUCCESS) {
                    if (rVal == (DWORD)-1) {
                        finalRadius = IsWin11OrGreater() ? 8 : 0;
                    } else {
                        finalRadius = (int)rVal;
                    }
                } else {
                    finalRadius = 12; // custom-corner-radius mod default is 12
                }

                int finalSmallRadius = 4;
                if (sRes == ERROR_SUCCESS) {
                    if (sVal == (DWORD)-1) {
                        finalSmallRadius = IsWin11OrGreater() ? 4 : 0;
                    } else {
                        finalSmallRadius = (int)sVal;
                    }
                } else {
                    if (rRes == ERROR_SUCCESS && rVal != (DWORD)-1) {
                        finalSmallRadius = (int)roundf((float)rVal * 0.5f);
                    } else {
                        finalSmallRadius = 6; // custom-corner-radius mod default is 6
                    }
                }

                if (outRadius) *outRadius = finalRadius;
                if (outSmallRadius) *outSmallRadius = finalSmallRadius;
                return true;
            }
        }
    }

    // Native OS Fallback
    if (outRadius) *outRadius = IsWin11OrGreater() ? 8 : 0;
    if (outSmallRadius) *outSmallRadius = IsWin11OrGreater() ? 4 : 0;
    return false;
}

static void DetectSystemDwmCornerRadius() {
    int r = 8, s = 4;
    g_customCornerRadiusModActive = DetectSystemDwmCornerRadiusValues(&r, &s);
    g_systemDwmRadius = r;
    g_systemDwmSmallRadius = s;
}

static DWORD WINAPI DwmCornerWatchThread(LPVOID lpParam) {
    const HWND targetWindow = (HWND)lpParam;
    int lastRadius = 0, lastSmallRadius = 0;
    DetectSystemDwmCornerRadiusValues(&lastRadius, &lastSmallRadius);
    HKEY hKey = NULL;
    LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                              L"SOFTWARE\\Windhawk\\Engine\\Mods",
                              0, KEY_NOTIFY | KEY_READ | KEY_WOW64_64KEY, &hKey);
    if (lRes != ERROR_SUCCESS) {
        return 0;
    }

    HANDLE hChangeEvt = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!hChangeEvt) {
        RegCloseKey(hKey);
        return 0;
    }

    HANDLE waitHandles[2] = { g_hDwmCornerWatchStopEvent, hChangeEvt };

    while (true) {
        lRes = RegNotifyChangeKeyValue(hKey, TRUE,
                                       REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,
                                       hChangeEvt, TRUE);
        if (lRes != ERROR_SUCCESS) {
            break;
        }

        DWORD dwWait = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
        if (dwWait == WAIT_OBJECT_0) {
            // Stop event signaled
            break;
        } else if (dwWait == WAIT_OBJECT_0 + 1) {
            // Registry change detected: debounce 50ms for batch multi-value writes
            Sleep(50);

            int newR = 8, newS = 4;
            DetectSystemDwmCornerRadiusValues(&newR, &newS);
            if (newR != lastRadius || newS != lastSmallRadius) {
                lastRadius = newR;
                lastSmallRadius = newS;
                PostMessage(targetWindow, WM_SWS_SETTINGS_CHANGED, 0, 0);
            }
        } else {
            break;
        }
    }

    CloseHandle(hChangeEvt);
    RegCloseKey(hKey);
    return 0;
}

struct ThumbCacheState {
    RECT dst;
    BYTE alpha;
    BOOL visible;
    RECT source = {};
    bool hasSource = false;
};
static std::map<HTHUMBNAIL, ThumbCacheState> g_lastThumbState;
// DWM retains rcSource when a later update omits DWM_TNP_RECTSOURCE. This
// physical state outlives invalidation of the destination/opacity cache.
static std::map<HTHUMBNAIL, RECT> g_thumbSourceRects;

static void InvalidateThumbCache(HTHUMBNAIL thumbnail) {
    g_lastThumbState.erase(thumbnail);
}

static HRESULT UpdateDwmThumbnail(HTHUMBNAIL thumbnail, const DWM_THUMBNAIL_PROPERTIES* properties) {
    // Direct layout/visibility writes invalidate the animation cache too.
    InvalidateThumbCache(thumbnail);
    HRESULT result = DwmUpdateThumbnailProperties(thumbnail, properties);
    if (SUCCEEDED(result) && (properties->dwFlags & DWM_TNP_RECTSOURCE)) {
        g_thumbSourceRects[thumbnail] = properties->rcSource;
    }
    return result;
}

static inline HRESULT SafeDwmUnregisterThumbnail(HTHUMBNAIL h) {
    if (!h) return S_OK;
    InvalidateThumbCache(h);
    HRESULT result = DwmUnregisterThumbnail(h);
    if (SUCCEEDED(result)) g_thumbSourceRects.erase(h);
    return result;
}

// Forward declarations
static void LoadSettings();
static void SWS_RegisterHotkeys();
static void SWS_UnregisterHotkeys();
static void ApplySwitcherRegion();
static void ApplyThemeToWindow(HWND hWnd);
static void ApplyAcrylicWindowRegion(HWND hWnd);
static void CreateMirrorSwitchers();
static void ShowMirrorSwitchers();
static void HideSwitcher();
static void ShowBackdropBlur();
static void HideBackdropBlur();
static void DestroyBackdropWindow();
static void PaintSwitcher();
static void PaintSwitcherOverlay();
static INT GetCornerPref();
static int GetWindowCornerRadiusPx();
static void DrawSwitcherStaticContent(HDC hdc, bool fillBg, HWND hWnd);
static void FillSwitcherBackground(HDC hdc, const RECT& rect, bool fillBg);
static void DrawScrollTransitionFrame(HDC hdc, int w, int h, bool includeSelection);
static void PreRenderScrollCanvases();
static void UpdateThumbnailAnimations();
static void UpdateDockThumbnailDwm();
static void UpdateDockPreviewForSelection();
static void FinishAnimations();
static void StartExitAnimation(bool activateSelectedWindow);
static void UpdateHoverFromCursor(bool allowAnimation = true);
static void UpdateHoverAtPoint(HWND hWnd, int x, int y, bool allowAnimation = true);
static void GetOverflowState(bool& hasPrev, bool& hasNext);
static void UpdateChevronLayout(HWND hWnd);
static void RestoreWindowIfIconic(HWND hWnd);
static void UpdateChevronAnimationTargets(bool immediate = false);
static int HitTestChevron(HWND hWnd, int x, int y);
static void EnterAppGroup();
static void ActivateExitedWindow(HWND hTarget, const std::vector<HWND>& restoreWindows);
static void TapUnassignedKeyForForeground();
static bool ActivateRawTouchpadWindow();

static inline int DpiScale(int val, int dpi) { return MulDiv(val, dpi, 96); }

using SwsGetDpiForWindow_t = UINT(WINAPI*)(HWND);
using SwsGetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);

static UINT QueryWindowDpi(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return 0;
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto getDpi = user32 ? (SwsGetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow") : nullptr;
    return getDpi ? getDpi(hWnd) : 0;
}

// GetDpiForMonitor is retained only as the monitor-only fallback: Microsoft
// documents it as not DPI-aware and recommends GetDpiForWindow for PM-aware
// windows. Prefer the real window's DPI whenever that window is already on the
// requested monitor, which avoids mixing virtualized and physical coordinates.
static bool QueryMonitorDpi(HMONITOR hMon, UINT* dpiX, UINT* dpiY) {
    if (!dpiX || !dpiY) return false;
    *dpiX = *dpiY = 96;
    if (g_hSwitcher && IsWindow(g_hSwitcher) &&
        MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST) == hMon) {
        UINT dpi = QueryWindowDpi(g_hSwitcher);
        if (dpi) {
            *dpiX = *dpiY = dpi;
            return true;
        }
    }
    HMODULE shcore = GetModuleHandleW(L"shcore.dll");
    if (!shcore) shcore = LoadLibraryW(L"shcore.dll");
    auto getDpi = shcore ? (SwsGetDpiForMonitor_t)GetProcAddress(shcore, "GetDpiForMonitor") : nullptr;
    if (!getDpi || FAILED(getDpi(hMon, 0, dpiX, dpiY)) || !*dpiX || !*dpiY) {
        *dpiX = *dpiY = 96;
        return false;
    }
    return true;
}

// Helper: check if a window is truncated (not placed in current layout)
static bool IsWindowTruncated(int idx) {
    if (idx < 0 || idx >= (int)g_windows.size()) return true;
    const auto& w = g_windows[idx];
    return w.rcCell.left == 0 && w.rcCell.right == 0 &&
           w.rcCell.top == 0 && w.rcCell.bottom == 0;
}

// Helper: find index of a window entry by its HWND
static int FindWindowIndexByHwnd(HWND hWnd) {
    if (!hWnd) return -1;
    for (int i = 0; i < (int)g_windows.size(); i++) {
        if (g_windows[i].hWnd == hWnd) return i;
    }
    return -1;
}

// Helpers

struct CachedSettings {
    bool themeIsNone;
    bool themeIsMica;
    bool themeIsBackdrop;
    bool thumbnailHoverIsZoom;
    bool dockLayoutActive;
    bool badgeLayoutActive;
    bool highlightHasFill;
    bool highlightHasBorder;
    bool thumbnailIsBottom;
    bool thumbnailIsTop;
    bool thumbnailIsLeft;
    bool thumbnailIsRight;
    bool thumbnailIsSide;
};
static CachedSettings s_cachedSettings = {};

static void UpdateCachedSettings() {
    s_cachedSettings.themeIsNone = (wcscmp(g_settings.theme, L"none") == 0);
    s_cachedSettings.themeIsMica = (wcscmp(g_settings.theme, L"mica") == 0);
    s_cachedSettings.themeIsBackdrop = (wcscmp(g_settings.theme, L"backdrop") == 0);
    s_cachedSettings.thumbnailHoverIsZoom = (wcscmp(g_settings.thumbnailHoverEffect, L"zoom") == 0);
    s_cachedSettings.dockLayoutActive = (wcscmp(g_settings.switcherLayout, L"dock") == 0);
    s_cachedSettings.badgeLayoutActive = (wcscmp(g_settings.switcherLayout, L"badge") == 0 && g_settings.showThumbnails);

    if (s_cachedSettings.dockLayoutActive) {
        s_cachedSettings.highlightHasFill = (wcscmp(g_settings.dockHighlightStyle, L"border") != 0);
        s_cachedSettings.highlightHasBorder = (wcscmp(g_settings.dockHighlightStyle, L"border") == 0 ||
                                               wcscmp(g_settings.dockHighlightStyle, L"fillAndBorder") == 0);
    } else {
        bool fillAndBorder = (wcscmp(g_settings.highlightStyle, L"fillAndBorder") == 0);
        s_cachedSettings.highlightHasFill = fillAndBorder || (wcscmp(g_settings.highlightStyle, L"fillOnly") == 0);
        s_cachedSettings.highlightHasBorder = fillAndBorder || (wcscmp(g_settings.highlightStyle, L"border") == 0);
    }

    s_cachedSettings.thumbnailIsBottom = (wcscmp(g_settings.thumbnailPosition, L"bottom") == 0);
    s_cachedSettings.thumbnailIsTop = (wcscmp(g_settings.thumbnailPosition, L"top") == 0);
    s_cachedSettings.thumbnailIsLeft = (wcscmp(g_settings.thumbnailPosition, L"left") == 0);
    s_cachedSettings.thumbnailIsRight = (wcscmp(g_settings.thumbnailPosition, L"right") == 0);
    s_cachedSettings.thumbnailIsSide = (s_cachedSettings.thumbnailIsLeft || s_cachedSettings.thumbnailIsRight);
}

static bool ThemeIs(const WCHAR* v) {
    if (wcscmp(v, L"none") == 0) return s_cachedSettings.themeIsNone;
    if (wcscmp(v, L"mica") == 0) return s_cachedSettings.themeIsMica;
    if (wcscmp(v, L"backdrop") == 0) return s_cachedSettings.themeIsBackdrop;
    return wcscmp(g_settings.theme, v) == 0;
}
static bool ShouldFillBackground() {
    return ThemeIs(L"none");
}
static bool ScrollIs(const WCHAR* v) { return wcscmp(g_settings.scrollWheelBehavior, v) == 0; }
static bool LayoutIsVertical() { return wcscmp(g_settings.taskListOrientation, L"vertical") == 0; }
static bool HeaderOrientationIs(const WCHAR* v) { return wcscmp(g_settings.headerContentOrientation, v) == 0; }
static bool IconSizeIs(const WCHAR* v) { return wcscmp(g_settings.iconSize, v) == 0; }
static bool BackwardShortcutIs(const WCHAR* v) { return wcscmp(g_settings.backwardShortcut, v) == 0; }
static bool UseAltShiftTabBackward() { return BackwardShortcutIs(L"altShiftTab"); }
static bool UseAltShiftBackward() { return BackwardShortcutIs(L"altShift"); }
static bool ThumbnailIsBottom() { return s_cachedSettings.thumbnailIsBottom; }
static bool ThumbnailIsTop() { return s_cachedSettings.thumbnailIsTop; }
static bool ThumbnailIsLeft() { return s_cachedSettings.thumbnailIsLeft; }
static bool ThumbnailIsRight() { return s_cachedSettings.thumbnailIsRight; }
static bool ThumbnailIsSide() { return s_cachedSettings.thumbnailIsSide; }
static bool ThumbnailAlignmentIs(const WCHAR* v) { return wcscmp(g_settings.thumbnailAlignment, v) == 0; }
static bool ThumbnailAlignCentered() { return ThumbnailAlignmentIs(L"centered"); }
static bool ThumbnailAlignRight() { return ThumbnailAlignmentIs(L"right"); }
static bool ThumbnailHoverIsZoom() { return s_cachedSettings.thumbnailHoverIsZoom; }
static bool DockLayoutActive() {
    return s_cachedSettings.dockLayoutActive;
}
static bool BadgeLayoutActive() {
    return s_cachedSettings.badgeLayoutActive;
}
static inline int GetActiveSwitcherPadding() {
    if (DockLayoutActive()) return g_settings.dockSwitcherPadding;
    if (BadgeLayoutActive()) return g_settings.badgeSwitcherPadding;
    return g_settings.defaultSwitcherPadding;
}
static bool HighlightHasFill() {
    return s_cachedSettings.highlightHasFill;
}
static bool HighlightHasBorder() {
    return s_cachedSettings.highlightHasBorder;
}
static bool StretchThumbsToTaskWidth() {
    return g_settings.stretchThumbnailsToTaskWidth;
}
static bool HeaderIsVertical() {
    return HeaderOrientationIs(L"vertical");
}
static bool DockIconIsTop() {
    return wcscmp(g_settings.dockIconPosition, L"bottom") != 0;
}
static bool DockShowPreview() {
    return g_settings.dockShowPreview && g_settings.showThumbnails;
}
static bool DockCloseButtonIsHidden() { return wcscmp(g_settings.dockCloseButtonPosition, L"hidden") == 0; }
static bool DockGroupIndicatorIsHidden() { return wcscmp(g_settings.dockGroupIndicatorPosition, L"hidden") == 0; }
static bool DockPositionsOverlap() {
    if (DockCloseButtonIsHidden() || DockGroupIndicatorIsHidden()) return false;
    return wcscmp(g_settings.dockCloseButtonPosition, g_settings.dockGroupIndicatorPosition) == 0;
}

static RECT ComputeDockPerimeterRect(const RECT& rcCell, int itemW, int itemH, const WCHAR* posStr, int pad) {
    if (!posStr || wcscmp(posStr, L"hidden") == 0) return { 0, 0, 0, 0 };
    int bx = 0, by = 0;
    if (wcscmp(posStr, L"topLeft") == 0) {
        bx = rcCell.left + pad;
        by = rcCell.top + pad;
    } else if (wcscmp(posStr, L"bottomRight") == 0) {
        bx = rcCell.right - itemW - pad;
        by = rcCell.bottom - itemH - pad;
    } else if (wcscmp(posStr, L"bottomLeft") == 0) {
        bx = rcCell.left + pad;
        by = rcCell.bottom - itemH - pad;
    } else if (wcscmp(posStr, L"top") == 0 || wcscmp(posStr, L"topCenter") == 0) {
        bx = (rcCell.left + rcCell.right - itemW) / 2;
        by = rcCell.top + pad;
    } else if (wcscmp(posStr, L"bottom") == 0 || wcscmp(posStr, L"bottomCenter") == 0) {
        bx = (rcCell.left + rcCell.right - itemW) / 2;
        by = rcCell.bottom - itemH - pad;
    } else if (wcscmp(posStr, L"left") == 0 || wcscmp(posStr, L"leftCenter") == 0) {
        bx = rcCell.left + pad;
        by = (rcCell.top + rcCell.bottom - itemH) / 2;
    } else if (wcscmp(posStr, L"right") == 0 || wcscmp(posStr, L"rightCenter") == 0) {
        bx = rcCell.right - itemW - pad;
        by = (rcCell.top + rcCell.bottom - itemH) / 2;
    } else { // default: topRight
        bx = rcCell.right - itemW - pad;
        by = rcCell.top + pad;
    }
    return { bx, by, bx + itemW, by + itemH };
}

static bool BadgeIconPositionIs(const WCHAR* v) { return wcscmp(g_settings.badgeIconPosition, v) == 0; }
static bool BadgeTitleIsTop() { return wcscmp(g_settings.badgeTitlePosition, L"top") == 0; }
static inline bool IsEntryMinimized(const WindowEntry& w) {
    if (g_settings.showApplications && !w.groupWindows.empty()) {
        for (HWND hw : w.groupWindows) {
            if (IsWindow(hw) && !IsIconic(hw)) return false;
        }
        return true;
    }
    return IsIconic(w.hWnd) != FALSE;
}
static inline bool MinimizedStyleUsesDimming() {
    return wcscmp(g_settings.minimizedIndicatorStyle, L"dimIcon") == 0 ||
           wcscmp(g_settings.minimizedIndicatorStyle, L"dimAndDot") == 0 ||
           wcscmp(g_settings.minimizedIndicatorStyle, L"dimAndBadge") == 0;
}
static inline bool MinimizedStyleUsesDot() {
    return wcscmp(g_settings.minimizedIndicatorStyle, L"dot") == 0 ||
           wcscmp(g_settings.minimizedIndicatorStyle, L"dimAndDot") == 0;
}
static inline bool MinimizedStyleUsesBadge() {
    return wcscmp(g_settings.minimizedIndicatorStyle, L"badge") == 0 ||
           wcscmp(g_settings.minimizedIndicatorStyle, L"dimAndBadge") == 0;
}
static int GetHeaderIconSizeBase() {
    if (DockLayoutActive()) return g_settings.dockIconSize > 0 ? g_settings.dockIconSize : 48;
    if (BadgeLayoutActive()) {
        if (wcscmp(g_settings.badgeIconSize, L"small") == 0) return 16;
        if (wcscmp(g_settings.badgeIconSize, L"large") == 0) return 40;
        if (wcscmp(g_settings.badgeIconSize, L"xlarge") == 0) return 48;
        return 32; // Default to Medium (32px) for Badge Layout
    }
    if (IconSizeIs(L"xlarge")) return 64;
    if (IconSizeIs(L"large")) return 48;
    if (IconSizeIs(L"medium")) return 32;
    return SWS_ICON_SIZE;
}
static RECT g_rcCentralPreviewSlot = { 0, 0, 0, 0 };
static RECT g_rcCentralPreview = { 0, 0, 0, 0 };
static RECT g_rcDockTitleBar = { 0, 0, 0, 0 };
static RECT g_rcDockIconStrip = { 0, 0, 0, 0 };
// ── Animation Engine ─────────────────────────────────────────────────────────

struct CubicBezierEasing {
    float ax, bx, cx;
    float ay, by, cy;

    CubicBezierEasing() : ax(0), bx(0), cx(0), ay(0), by(0), cy(0) {}
    CubicBezierEasing(float x1, float y1, float x2, float y2) {
        Init(x1, y1, x2, y2);
    }

    void Init(float x1, float y1, float x2, float y2) {
        cx = 3.0f * x1;
        bx = 3.0f * (x2 - x1) - cx;
        ax = 1.0f - cx - bx;
        cy = 3.0f * y1;
        by = 3.0f * (y2 - y1) - cy;
        ay = 1.0f - cy - by;
    }

    float SampleX(float s) const { return ((ax * s + bx) * s + cx) * s; }
    float SampleY(float s) const { return ((ay * s + by) * s + cy) * s; }
    float SampleDerivativeX(float s) const { return (3.0f * ax * s + 2.0f * bx) * s + cx; }

    float Solve(float t) const {
        if (t <= 0.0f) return 0.0f;
        if (t >= 1.0f) return 1.0f;
        float s = t;
        for (int i = 0; i < 6; ++i) {
            float x2 = SampleX(s) - t;
            if (fabsf(x2) < 1e-5f) return SampleY(s);
            float d2 = SampleDerivativeX(s);
            if (fabsf(d2) < 1e-6f) break;
            s -= x2 / d2;
        }
        float s0 = 0.0f, s1 = 1.0f;
        s = t;
        while (s0 < s1) {
            float x2 = SampleX(s);
            if (fabsf(x2 - t) < 1e-5f) return SampleY(s);
            if (t > x2) s0 = s; else s1 = s;
            s = (s1 + s0) * 0.5f;
        }
        return SampleY(s);
    }
};

struct RectF {
    float left;
    float top;
    float right;
    float bottom;
};

static inline RectF ToRectF(const RECT& r) {
    return { (float)r.left, (float)r.top, (float)r.right, (float)r.bottom };
}

static inline RectF LerpRect(const RectF& a, const RectF& b, float t) {
    return {
        a.left   + (b.left   - a.left)   * t,
        a.top    + (b.top    - a.top)    * t,
        a.right  + (b.right  - a.right)  * t,
        a.bottom + (b.bottom - a.bottom) * t
    };
}

// Windows motion guidance: direct entrance/exit uses (0,0,0,1), while existing
// elements moving between states use (0.55,0.55,0,1).
static CubicBezierEasing g_easeEntrance(0.0f, 0.0f, 0.0f, 1.0f);
static CubicBezierEasing g_easeSlide(0.55f, 0.55f, 0.0f, 1.0f);
static CubicBezierEasing g_easeHover(0.55f, 0.55f, 0.0f, 1.0f);
static CubicBezierEasing g_easeHoverEnter(0.0f, 0.0f, 0.0f, 1.0f);
static CubicBezierEasing g_easeSelection(0.55f, 0.55f, 0.0f, 1.0f);
static CubicBezierEasing g_easeExit(0.0f, 0.0f, 0.0f, 1.0f);

static bool g_animActive = false;
static LARGE_INTEGER g_animPerfFreq = {};
static LARGE_INTEGER g_animLastTickTime = {};
static double g_animNextFrameDeadline = 0.0;
static bool g_animTickInProgress = false;
static bool g_animFrameSampleActive = false;
static bool g_animEntranceFrameZeroPending = false;
static bool g_animEntranceClockPending = false;
static void StartMotionTrack(float& progress, float initial = 0.0f);
static void StartMotionTrack(float& progress, MotionTrackClock& clock, float initial);
static bool StepMotionTrack(float& progress, float duration, float dt);
static void StartOpacityMotion(OpacityMotionTrack& track, float current, float target);
static bool StepOpacityMotion(OpacityMotionTrack& track, float& current, float dt);

// Exit animation state (the entrance presentation curve played in reverse)
static bool g_animExitActive = false;
static float g_animExitProgress = 1.0f;
static float g_animExitDuration = SWS_PRESENTATION_ANIMATION_MS / 1000.0f;
static float g_animExitCurrentAlpha = 1.0f;

// Monitor timing is cached across track starts. DWM timing is a session-wide
// fallback, not a per-monitor measurement; 60 Hz is the final explicit fallback.
static double s_animTargetIntervalMs = 1000.0 / 60.0;
static HMONITOR s_animTimingMonitor = NULL;
static bool s_animTimingValid = false;
static LARGE_INTEGER s_animTimingLastQuery = {};

static double GetMonitorRefreshRate(const WCHAR* deviceName) {
    // CCD preserves fractional rates (e.g. 60000/1001) that DEVMODE rounds to
    // integers. Query only active paths and match the switcher's monitor source.
    for (int attempt = 0; attempt < 2; ++attempt) {
        UINT32 pathCount = 0, modeCount = 0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS ||
            !pathCount || !modeCount) return 0.0;
        std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
        LONG result = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(),
                                         &modeCount, modes.data(), nullptr);
        if (result == ERROR_INSUFFICIENT_BUFFER) continue;
        if (result != ERROR_SUCCESS) return 0.0;
        for (UINT32 i = 0; i < pathCount; ++i) {
            const auto& path = paths[i];
            DISPLAYCONFIG_SOURCE_DEVICE_NAME source = {};
            source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
            source.header.size = sizeof(source);
            source.header.adapterId = path.sourceInfo.adapterId;
            source.header.id = path.sourceInfo.id;
            if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS ||
                _wcsicmp(source.viewGdiDeviceName, deviceName) != 0) continue;
            const auto& rate = path.targetInfo.refreshRate;
            if (!rate.Denominator) continue;
            double hz = (double)rate.Numerator / rate.Denominator;
            if (hz >= 24.0 && hz <= 1000.0) return hz;
        }
        return 0.0;
    }
    return 0.0;
}

static void UpdateRefreshRateTiming(bool force = false) {
    HWND targetWnd = g_hSwitcher ? g_hSwitcher : GetDesktopWindow();
    HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor :
                    MonitorFromWindow(targetWnd, MONITOR_DEFAULTTONEAREST);
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    // Invocation/display-change boundaries maintain this cache. Animation
    // frames and track restarts never query drivers or restart motion clocks.
    if (!force && s_animTimingValid && hMon == s_animTimingMonitor &&
        (g_animPerfFreq.QuadPart <= 0 ||
         now.QuadPart - s_animTimingLastQuery.QuadPart < g_animPerfFreq.QuadPart / 2)) return;
    s_animTimingLastQuery = now;
    s_animTimingMonitor = hMon;
    s_animTimingValid = true;
    double hz = 0.0;
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMon, &mi)) {
        hz = GetMonitorRefreshRate(mi.szDevice);
        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);
        if (hz <= 0.0 && EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) &&
            dm.dmDisplayFrequency >= 24 && dm.dmDisplayFrequency <= 1000) {
            hz = (double)dm.dmDisplayFrequency;
        }
    }
    if (hz <= 0.0) {
        DWM_TIMING_INFO ti = {};
        ti.cbSize = sizeof(ti);
        if (SUCCEEDED(DwmGetCompositionTimingInfo(NULL, &ti)) && ti.rateRefresh.uiDenominator > 0) {
            double fallbackHz = (double)ti.rateRefresh.uiNumerator / (double)ti.rateRefresh.uiDenominator;
            if (fallbackHz >= 24.0 && fallbackHz <= 1000.0) hz = fallbackHz;
        }
    }
    s_animTargetIntervalMs = 1000.0 / (hz > 0.0 ? hz : 60.0);
}

// Entrance/exit transform state. Reversing entrance gives a gentle exit with
// continuous interrupted motion; layered opacity has its own 83 ms clock.
static bool g_animEntranceActive = false;
static float g_animEntranceProgress = 1.0f;
static float g_animEntranceDuration = SWS_PRESENTATION_ANIMATION_MS / 1000.0f;
static float g_animEntranceCurrentAlpha = 1.0f;
static OpacityMotionTrack g_presentationOpacity;
static float g_presentationOpacityCurrent = 1.0f;

struct PresentationWindowState {
    RECT layoutRect;
    UINT dpiY;
};
static std::map<HWND, PresentationWindowState> g_presentationWindows;
static void SetSwitcherLayoutBounds(int x, int y, int w, int h,
                                    UINT flags = SWP_NOACTIVATE);

// Selection focus: documented 167 ms existing-element point-to-point motion.
static bool g_animSelectionActive = false;
static RectF g_animSelectionStart = {};
static RectF g_animSelectionTarget = {};
static RectF g_animSelectionCurrent = {};
static float g_animSelectionProgress = 1.0f;
static float g_animSelectionDuration = 0.167f;

static void InvalidateStaticCache();
static inline void SnapSelectionTo(const RectF& r) {
    InvalidateStaticCache();
    g_animSelectionCurrent = r;
    g_animSelectionTarget = r;
    g_animSelectionStart = r;
    g_animSelectionProgress = 1.0f;
    g_animSelectionActive = false;
}

static inline float CurrentPresentationAlpha() {
    // Native Mica/Acrylic clients and materials cannot share a whole-window
    // alpha without changing their rendering contract. Keep their thumbnails
    // and overlay opaque too; the entire native surface uses motion only.
    if (!ThemeIs(L"none")) return 1.0f;
    float alpha = g_presentationOpacityCurrent;
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    return alpha;
}

static inline BYTE CurrentPresentationAlphaByte() {
    return (BYTE)roundf(CurrentPresentationAlpha() * 255.0f);
}

static inline int GetAnimationOffsetY(UINT dpiY) {
    // Exit decreases the very same linear timeline used by entrance. This
    // retraces E(p), including when entrance is interrupted before p reaches 1.
    float p = g_animExitActive ? g_animExitProgress : g_animEntranceProgress;
    float e = g_easeEntrance.Solve(p);
    return (int)roundf((1.0f - e) * SWS_PRESENTATION_OFFSET_PX * dpiY / 96.0f);
}

static bool GetPresentationLayoutRect(HWND hWnd, RECT* rect) {
    if (hWnd == g_hSwitcher && g_isPendingShow &&
        g_pendingSwitcherRect.right > g_pendingSwitcherRect.left) {
        *rect = g_pendingSwitcherRect;
        return true;
    }
    auto it = g_presentationWindows.find(hWnd);
    if (it != g_presentationWindows.end()) {
        *rect = it->second.layoutRect;
        return true;
    }
    return GetWindowRect(hWnd, rect);
}

static UINT GetPresentationDpiY(HWND hWnd, const RECT& layoutRect) {
    // Mirrors use their own monitor's DPI, not the main layout's DPI. Resolve
    // from the untransformed rect so a tiny motion near an edge cannot change it.
    HMONITOR hMon = MonitorFromRect(&layoutRect, MONITOR_DEFAULTTONEAREST);
    if (hWnd && IsWindow(hWnd) && MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) == hMon) {
        UINT dpi = QueryWindowDpi(hWnd);
        if (dpi) return dpi;
    }
    UINT dpiX = 96, dpiY = 96;
    QueryMonitorDpi(hMon, &dpiX, &dpiY);
    return dpiY;
}

static bool GetPresentationWindowRect(HWND hWnd, RECT* rect) {
    auto it = g_presentationWindows.find(hWnd);
    if (it == g_presentationWindows.end()) return GetWindowRect(hWnd, rect);
    *rect = it->second.layoutRect;
    int offset = GetAnimationOffsetY(it->second.dpiY);
    rect->top += offset;
    rect->bottom += offset;
    return true;
}

static void PositionPresentationWindow(HWND hWnd,
                                       UINT flags = SWP_NOACTIVATE | SWP_NOZORDER) {
    if (!hWnd || !IsWindow(hWnd)) return;
    RECT target = {}, actual = {};
    if (!GetPresentationWindowRect(hWnd, &target) ||
        target.right <= target.left || target.bottom <= target.top) return;
    if (GetWindowRect(hWnd, &actual) && EqualRect(&actual, &target) &&
        !(flags & (SWP_FRAMECHANGED | SWP_SHOWWINDOW))) return;
    SetWindowPos(hWnd, HWND_TOPMOST, target.left, target.top,
                 target.right - target.left, target.bottom - target.top, flags);
    if (hWnd != g_hCloseBtnWnd &&
        (actual.right - actual.left != target.right - target.left ||
         actual.bottom - actual.top != target.bottom - target.top)) {
        ApplyAcrylicWindowRegion(hWnd);
    }
}

static void SetPresentationWindowLayout(HWND hWnd, int x, int y, int w, int h,
                                        UINT flags = SWP_NOACTIVATE) {
    if (!hWnd || !IsWindow(hWnd)) return;
    RECT rect = { x, y, x + w, y + h };
    g_presentationWindows[hWnd] = { rect, GetPresentationDpiY(hWnd, rect) };
    PositionPresentationWindow(hWnd, flags);
}

static void AnchorPresentationOverlay() {
    if (!g_hCloseBtnWnd || !IsWindow(g_hCloseBtnWnd)) return;
    HWND target = g_hoverWnd && IsWindow(g_hoverWnd) ? g_hoverWnd : g_hSwitcher;
    auto it = g_presentationWindows.find(target);
    if (it != g_presentationWindows.end()) {
        // Copy the target's layout AND DPI. The overlay follows that surface's
        // physical pixels even when hovering a mirror on a mixed-DPI monitor.
        g_presentationWindows[g_hCloseBtnWnd] = it->second;
    }
}

// Absolute layout coordinates are never inferred from an animated HWND. Every
// surface gets its offset exactly once, including after resize/reflow and in
// frame zero. Settled paints cause no redundant SetWindowPos/WM_SIZE traffic.
static void ApplyPresentationWindowOffset() {
    PositionPresentationWindow(g_hSwitcher);
    for (HWND hMirror : g_hMirrorSwitchers) {
        PositionPresentationWindow(hMirror);
    }
    AnchorPresentationOverlay();
    PositionPresentationWindow(g_hCloseBtnWnd);
}

static void HideSwitcherPresentationWindows() {
    if (g_hCloseBtnWnd && IsWindowVisible(g_hCloseBtnWnd)) {
        ShowWindow(g_hCloseBtnWnd, SW_HIDE);
    }
    for (HWND hMirror : g_hMirrorSwitchers) {
        if (IsWindow(hMirror)) ShowWindow(hMirror, SW_HIDE);
    }
    if (g_hSwitcher && IsWindowVisible(g_hSwitcher)) {
        ShowWindow(g_hSwitcher, SW_HIDE);
    }
}

static void ResetPresentationAnimation() {
    g_animEntranceFrameZeroPending = false;
    g_animEntranceClockPending = false;
    g_animEntranceActive = false;
    g_animEntranceProgress = 1.0f;
    g_animEntranceCurrentAlpha = 1.0f;
    g_animExitActive = false;
    g_animExitProgress = 1.0f;
    g_animExitCurrentAlpha = 1.0f;
    g_presentationOpacity = {};
    g_presentationOpacityCurrent = 1.0f;
    BackdropStopFade();
    if (g_hBackdropWnd && IsWindowVisible(g_hBackdropWnd)) {
        BackdropApplyAlpha(g_hBackdropWnd, 1.0f);
    }
    ApplyPresentationWindowOffset();
    if (!g_hSwitcher || !IsWindowVisible(g_hSwitcher)) {
        g_presentationWindows.clear();
    }
}

static void StartEntranceAnimation() {
    g_animExitActive = false;
    g_animExitProgress = 1.0f;
    g_animExitCurrentAlpha = 1.0f;
    g_animEntranceDuration = SWS_PRESENTATION_ANIMATION_MS / 1000.0f;
    g_animEntranceActive = AreAnimationsGloballyEnabled() && g_settings.enableEntranceAnimation;
    g_animEntranceProgress = g_animEntranceActive ? 0.0f : 1.0f;
    g_animEntranceCurrentAlpha = g_animEntranceActive ? 0.0f : 1.0f;
    g_animEntranceFrameZeroPending = g_animEntranceActive;
    g_animEntranceClockPending = g_animEntranceActive;
    g_presentationOpacityCurrent = g_animEntranceActive ? 0.0f : 1.0f;
    g_presentationOpacity = {};
    g_presentationOpacity.start = g_presentationOpacityCurrent;
    g_presentationOpacity.target = 1.0f;
    g_presentationOpacity.progress = g_animEntranceActive ? 0.0f : 1.0f;
}

// Start entrance clocks only after frame zero has been painted and the windows
// are visible. Backdrop capture, thumbnail registration, and icon work can be
// expensive; including that preparation in the animation timeline makes the
// reveal appear to skip or complete before the user sees it.
static void BeginEntranceAnimationClock() {
    if (!g_animEntranceClockPending || g_animEntranceFrameZeroPending) return;
    StartMotionTrack(g_animEntranceProgress, 0.0f);
    StartMotionTrack(g_presentationOpacity.progress, g_presentationOpacity.clock, 0.0f);
    g_animEntranceClockPending = false;
}

static void AdvancePresentationAnimation(float dt) {
    if (g_animEntranceActive) {
        StepMotionTrack(g_animEntranceProgress, g_animEntranceDuration, dt);
        g_animEntranceCurrentAlpha = g_easeEntrance.Solve(g_animEntranceProgress);
        if (g_animEntranceProgress == 1.0f) g_animEntranceActive = false;
    }
    if (g_animExitActive) {
        StepMotionTrack(g_animExitProgress, -g_animExitDuration, dt);
        g_animExitCurrentAlpha = g_easeEntrance.Solve(g_animExitProgress);
    }
    StepOpacityMotion(g_presentationOpacity, g_presentationOpacityCurrent, dt);
}

// Hover contour: 167 ms transform, with an independent 83 ms linear opacity.
static bool g_animHoverActive = false;
static RectF g_animHoverStart = {};
static RectF g_animHoverTarget = {};
static RectF g_animHoverCurrent = {};
static float g_animHoverAlphaStart = 0.0f;
static float g_animHoverAlphaTarget = 0.0f;
static float g_animHoverAlphaCurrent = 0.0f;
static float g_animHoverProgress = 1.0f;
static float g_animHoverDuration = 0.167f;
static OpacityMotionTrack g_hoverOpacity;

static inline void SnapHoverTo(const RectF& r) {
    g_animHoverCurrent = r;
    g_animHoverTarget = r;
    g_animHoverStart = r;
    g_animHoverProgress = 1.0f;
    g_animHoverActive = false;
}

// Hover thumbnail zoom animation state (per-entry concurrent WinUI 3 curves)
static constexpr float SWS_HOVER_ZOOM_DELTA = 0.025f; // Preserve the existing subtle zoom amplitude.
static bool g_animHoverScaleActive = false;

static inline RECT GetScaledThumbRect(const WindowEntry& e) {
    RECT r = e.rcThumbActual;
    if (ThumbnailHoverIsZoom() && e.hoverScale > 1.0001f) {
        int cx = (r.left + r.right) / 2;
        int cy = (r.top + r.bottom) / 2;
        int hw = (int)roundf(((float)(r.right - r.left) / 2.0f) * e.hoverScale);
        int hh = (int)roundf(((float)(r.bottom - r.top) / 2.0f) * e.hoverScale);
        r = { cx - hw, cy - hh, cx + hw, cy + hh };
    }
    return r;
}

// Modern Fluent Vector Chevron Indicators
static RECT g_rcChevronPrev = {0, 0, 0, 0};
static RECT g_rcChevronNext = {0, 0, 0, 0};
static int g_hoverChevron = 0;   // -1 = prev, +1 = next, 0 = none
static int g_pressedChevron = 0; // -1 = prev, +1 = next, 0 = none

// Mouse drag-to-cancel & click target integrity tracking
static POINT g_ptLButtonDown = {0, 0};
static int g_pressedIndex = -1; // -1 = none/bg, -2 = dock central preview, >= 0 = card index
static HWND g_pressedWindow = nullptr; // Stable identity across live list changes.
static bool g_isDragging = false;

static float g_animChevronAlphaPrev = 0.0f;
static float g_animChevronAlphaNext = 0.0f;
static float g_animChevronAlphaTargetPrev = 0.0f;
static float g_animChevronAlphaTargetNext = 0.0f;
static float g_animChevronProgressPrev = 1.0f;
static float g_animChevronStartAlphaPrev = 0.0f;
static float g_animChevronProgressNext = 1.0f;
static float g_animChevronStartAlphaNext = 0.0f;
static float g_animChevronHoverAlphaPrev = 0.0f;
static float g_animChevronHoverAlphaNext = 0.0f;
static float g_animChevronDuration = 0.167f;
static float g_animChevronTransformPrev = 0.0f;
static float g_animChevronTransformNext = 0.0f;
static float g_animChevronTransformStartPrev = 0.0f;
static float g_animChevronTransformStartNext = 0.0f;
static OpacityMotionTrack g_chevronRevealPrev;
static OpacityMotionTrack g_chevronRevealNext;
static OpacityMotionTrack g_chevronHoverPrev;
static OpacityMotionTrack g_chevronHoverNext;
static OpacityMotionTrack g_closeBtnHover;

// Dual-surface Row and Page Slide animation state
enum ScrollNavType {
    SCROLL_ROW,
    SCROLL_PAGE
};

struct OutgoingItemSnapshot {
    HWND hWnd = NULL;
    int windowIndex = -1;
    RECT rcCell;
    RECT rcThumbActual;
    RECT rcThumbSlot;
    WCHAR title[256];
    HICON hIcon;
    std::shared_ptr<WindowIconCell> iconCell;
    std::map<HWND, HTHUMBNAIL> hThumbs;
    int drawnIconX, drawnIconY, drawnIconSz;
    std::vector<HWND> groupWindows;
    SIZE sourceSize;
    RECT rcSourceCrop;
    SIZE effectiveSourceSize;
    float alpha = 1.0f;
    float scale = 1.0f;
    RECT rcCellLayout = {};
    RECT rcThumbLayout = {};
    ItemTransitionMotion entryMotion;
};

struct ScrollTransitionState {
    bool active = false;
    bool preservingThumbnails = false;
    float offsetStartX = 0.0f;
    float offsetStartY = 0.0f;
    float offsetCurrentX = 0.0f;
    float offsetCurrentY = 0.0f;
    float progress = 1.0f;
    float duration = 0.167f;
    int travelDistanceX = 0;
    int travelDistanceY = 0;
    float capturedOffsetX = 0.0f;
    float capturedOffsetY = 0.0f;
    std::vector<OutgoingItemSnapshot> outgoingItems;
};
static ScrollTransitionState g_scrollTransition;

struct DepartingEntrySnapshot {
    HWND hWnd;
    RECT rcCellStart;
    RECT rcCellCurrent;
    RECT rcThumbStart;
    RECT rcThumbCurrent;
    float alpha;
    float scale;
    ItemTransitionMotion motion;
    RECT rcCellTarget = {};
    RECT rcThumbTarget = {};
    HICON hIcon;
    std::shared_ptr<WindowIconCell> iconCell;
    WCHAR title[256];
    std::map<HWND, HTHUMBNAIL> hThumbs;
    std::vector<HWND> groupWindows;
    SIZE sourceSize = {};
    RECT rcSourceCrop = {};
};

struct LayoutTransitionState {
    bool active = false;
    float progress = 1.0f;
    float duration = 0.250f; // Windows existing-element point-to-point timing
    RectF rcWndStart = {};
    RectF rcWndTarget = {};
    RECT rcDockStripStart = {}, rcDockStripTarget = {};
    RECT rcDockPreviewStart = {}, rcDockPreviewTarget = {};
    RECT rcDockTitleStart = {}, rcDockTitleTarget = {};
    RECT rcDockSlotStart = {}, rcDockSlotTarget = {};
    bool scrollReflow = false;
    std::vector<DepartingEntrySnapshot> departingItems;
};
static LayoutTransitionState g_layoutTransition;
static CubicBezierEasing g_easeLayout(0.55f, 0.55f, 0.0f, 1.0f);
static CubicBezierEasing g_easeItemExit(1.0f, 0.0f, 1.0f, 1.0f);

struct DockPreviewSlideTransition {
    bool active = false;
    float progress = 1.0f;
    float duration = 0.250f; // Windows existing-element point-to-point timing
    float travelDistance = 0.0f;
    float currentOffset = 0.0f;
    float currentAlpha = 1.0f;
};
static DockPreviewSlideTransition g_dockPreviewSlide;

static bool g_calculatingLayoutTargets = false;
static void StartAnimationTicker();
static void TriggerSelectionAnimation(int prevSelected);
static void RegisterThumbnailsEarly();
static void ComputeLayout(HMONITOR hMon);
static void GetSwitcherPosition(const RECT& workArea, int* outX, int* outY,
                                int width, int height);
static void StartMotionTrack(float& progress, MotionTrackClock& clock, float initial);
static bool StepMotionTrack(float& progress, MotionTrackClock& clock, float duration, float dt);
static void StartOpacityMotion(OpacityMotionTrack& track, float current, float target);
static bool StepOpacityMotion(OpacityMotionTrack& track, float& current, float dt);

static RectF SelectionRectWithViewport() {
    if (g_selectedIndex < 0 || g_selectedIndex >= (int)g_windows.size()) return {};
    RectF rect = (g_animSelectionActive && AreAnimationsGloballyEnabled() &&
                  g_settings.enableSelectionAnimation)
                     ? g_animSelectionCurrent
                     : ToRectF(g_windows[g_selectedIndex].rcCell);
    rect.left += roundf(g_scrollTransition.offsetCurrentX);
    rect.right += roundf(g_scrollTransition.offsetCurrentX);
    rect.top += roundf(g_scrollTransition.offsetCurrentY);
    rect.bottom += roundf(g_scrollTransition.offsetCurrentY);
    return rect;
}

// Reflow changes the selected cell's local coordinates while the selection
// animation remains an independent 167 ms track. Carry all four edges by the
// target delta instead of restarting or letting layout ownership teleport it.
static void SyncSelectionAnimationToLayout() {
    if (g_selectedIndex < 0 || g_selectedIndex >= (int)g_windows.size()) return;
    RectF target = ToRectF(g_windows[g_selectedIndex].rcCell);
    if (!g_animSelectionActive) {
        if (g_animSelectionCurrent.left != target.left ||
            g_animSelectionCurrent.top != target.top ||
            g_animSelectionCurrent.right != target.right ||
            g_animSelectionCurrent.bottom != target.bottom) {
            SnapSelectionTo(target);
        }
        return;
    }
    RectF delta = {
        target.left - g_animSelectionTarget.left,
        target.top - g_animSelectionTarget.top,
        target.right - g_animSelectionTarget.right,
        target.bottom - g_animSelectionTarget.bottom,
    };
    auto shift = [&delta](RectF& rect) {
        rect.left += delta.left;
        rect.top += delta.top;
        rect.right += delta.right;
        rect.bottom += delta.bottom;
    };
    shift(g_animSelectionStart);
    shift(g_animSelectionCurrent);
    g_animSelectionTarget = target;
}

static void RetargetSelectionFromPresented(const RectF& presented) {
    if (g_selectedIndex < 0 || g_selectedIndex >= (int)g_windows.size()) return;
    RectF local = presented;
    local.left -= roundf(g_scrollTransition.offsetCurrentX);
    local.right -= roundf(g_scrollTransition.offsetCurrentX);
    local.top -= roundf(g_scrollTransition.offsetCurrentY);
    local.bottom -= roundf(g_scrollTransition.offsetCurrentY);
    if (!AreAnimationsGloballyEnabled() || !g_settings.enableSelectionAnimation) {
        SnapSelectionTo(ToRectF(g_windows[g_selectedIndex].rcCell));
        return;
    }
    g_animSelectionCurrent = local;
    g_animSelectionStart = local;
    TriggerSelectionAnimation(-1);
}

static bool IsItemTransitionActive(const ItemTransitionMotion& motion) {
    return motion.opacity.progress < 1.0f || motion.scaleProgress < 1.0f;
}

static void StartItemTransition(ItemTransitionMotion& motion, float alpha, float scale,
                                float targetAlpha, float targetScale) {
    StartOpacityMotion(motion.opacity, alpha, targetAlpha);
    if (scale == targetScale) {
        motion.scaleProgress = 1.0f;
        motion.scaleFrom = motion.scaleTo = targetScale;
    } else if (motion.scaleProgress >= 1.0f || motion.scaleTo != targetScale) {
        motion.scaleFrom = scale;
        motion.scaleTo = targetScale;
        StartMotionTrack(motion.scaleProgress, motion.scaleClock, 0.0f);
    }
}

static float ItemScaleEasing(const ItemTransitionMotion& motion) {
    return motion.scaleTo < motion.scaleFrom
        ? g_easeItemExit.Solve(motion.scaleProgress)
        : g_easeEntrance.Solve(motion.scaleProgress);
}

static bool StepItemTransition(ItemTransitionMotion& motion, float& alpha, float& scale, float dt) {
    bool opacityActive = StepOpacityMotion(motion.opacity, alpha, dt);
    bool scaleDone = StepMotionTrack(motion.scaleProgress, motion.scaleClock, 0.167f, dt);
    scale = scaleDone ? motion.scaleTo :
        motion.scaleFrom + (motion.scaleTo - motion.scaleFrom) * ItemScaleEasing(motion);
    return opacityActive || !scaleDone;
}

static bool HasLayoutRect(const RECT& rect) {
    return rect.right > rect.left && rect.bottom > rect.top;
}

static RECT InterpolateLayoutRect(const RECT& start, const RECT& target, float t) {
    RectF rect = LerpRect(ToRectF(start), ToRectF(target), t);
    return { (LONG)roundf(rect.left), (LONG)roundf(rect.top),
             (LONG)roundf(rect.right), (LONG)roundf(rect.bottom) };
}

static RECT ScaleLayoutRect(const RECT& rect, float scale) {
    int width = (int)roundf((rect.right - rect.left) * scale);
    int height = (int)roundf((rect.bottom - rect.top) * scale);
    int x = (rect.left + rect.right - width) / 2;
    int y = (rect.top + rect.bottom - height) / 2;
    return { x, y, x + width, y + height };
}

static bool UseDockScreenProjection() {
    // Scroll owns its viewport translation and its canvases. The fractional
    // projection is only for an ordinary Dock reflow where the window and the
    // inner layout are sampled by the same layout track.
    return DockLayoutActive() && g_layoutTransition.active &&
           !g_layoutTransition.scrollReflow && !g_scrollTransition.active;
}

static float DockTransitionSample() {
    return g_easeLayout.Solve(g_layoutTransition.progress);
}

struct DockPaintSpace {
    RectF screenFrame;
    int pixelOriginX;
    int pixelOriginY;
};

static DockPaintSpace GetDockPaintSpace() {
    RectF frame = LerpRect(g_layoutTransition.rcWndStart,
                           g_layoutTransition.rcWndTarget,
                           DockTransitionSample());
    return {frame, (int)roundf(frame.left), (int)roundf(frame.top)};
}

static RectF DockEntryCellForPaint(const WindowEntry& entry,
                                   const DockPaintSpace* paintSpace) {
    if (paintSpace && HasLayoutRect(entry.rcCellStart) &&
        HasLayoutRect(entry.rcCellTarget) && !entry.isNewEntry) {
        return LerpRect(ToRectF(entry.rcCellStart),
                        ToRectF(entry.rcCellTarget),
                        DockTransitionSample());
    }
    return ToRectF(entry.rcCell);
}

static RectF DockDepartingCellForPaint(
    const DepartingEntrySnapshot& entry, const DockPaintSpace* paintSpace) {
    if (paintSpace && HasLayoutRect(entry.rcCellStart) &&
        HasLayoutRect(entry.rcCellTarget)) {
        return LerpRect(ToRectF(entry.rcCellStart),
                        ToRectF(entry.rcCellTarget),
                        ItemScaleEasing(entry.motion));
    }
    return ToRectF(entry.rcCellCurrent);
}

static RECT DockScreenSnapRect(const RectF& localRect,
                               const DockPaintSpace& paintSpace) {
    return {
        (LONG)roundf(paintSpace.screenFrame.left + localRect.left) -
            paintSpace.pixelOriginX,
        (LONG)roundf(paintSpace.screenFrame.top + localRect.top) -
            paintSpace.pixelOriginY,
        (LONG)roundf(paintSpace.screenFrame.left + localRect.right) -
            paintSpace.pixelOriginX,
        (LONG)roundf(paintSpace.screenFrame.top + localRect.bottom) -
            paintSpace.pixelOriginY,
    };
}

static POINT DockScreenSnapIcon(const RectF& cell, int iconSize, int offsetX,
                                int offsetY, const DockPaintSpace& paintSpace) {
    float localX = cell.left + (cell.right - cell.left - iconSize) * 0.5f +
                   offsetX;
    float localY = cell.top + (cell.bottom - cell.top - iconSize) * 0.5f +
                   offsetY;
    return {
        (LONG)roundf(paintSpace.screenFrame.left + localX) -
            paintSpace.pixelOriginX,
        (LONG)roundf(paintSpace.screenFrame.top + localY) -
            paintSpace.pixelOriginY,
    };
}

static POINT DockLegacyIcon(const RECT& cell, int iconSize, int offsetX,
                            int offsetY) {
    return {
        (LONG)roundf(cell.left + (cell.right - cell.left - iconSize) * 0.5f) +
            offsetX,
        (LONG)roundf(cell.top + (cell.bottom - cell.top - iconSize) * 0.5f) +
            offsetY,
    };
}

static RECT DockTitleTextRectForPaint(const DockPaintSpace* paintSpace) {
    int margin = DpiScale(20, g_dpiX);
    if (!paintSpace) {
        RECT rcText = g_rcDockTitleBar;
        rcText.left += margin;
        rcText.right -= margin;
        if (g_dockPreviewSlide.active) {
            int offX = (int)roundf(g_dockPreviewSlide.currentOffset);
            rcText.left += offX;
            rcText.right += offX;
        }
        return rcText;
    }

    RectF title = LerpRect(ToRectF(g_layoutTransition.rcDockTitleStart),
                           ToRectF(g_layoutTransition.rcDockTitleTarget),
                           DockTransitionSample());
    title.left += margin;
    title.right -= margin;
    if (g_dockPreviewSlide.active) {
        int offX = (int)roundf(g_dockPreviewSlide.currentOffset);
        title.left += offX;
        title.right += offX;
    }
    return DockScreenSnapRect(title, *paintSpace);
}

static void ApplyEntryPresentationGeometry(WindowEntry& entry) {
    entry.rcCell = ScaleLayoutRect(entry.rcCellLayoutCurrent, entry.enterScale);
    entry.rcThumbActual = ScaleLayoutRect(entry.rcThumbLayoutCurrent, entry.enterScale);
}

static void ApplyEntryLayoutGeometry(WindowEntry& entry, float t) {
    if (!HasLayoutRect(entry.rcCellTarget)) {
        entry.rcCellLayoutCurrent = entry.rcThumbLayoutCurrent = {};
        entry.rcCell = entry.rcThumbActual = entry.rcThumbSlot = {};
        return;
    }
    entry.rcCellLayoutCurrent = g_layoutTransition.scrollReflow ? entry.rcCellTarget :
        InterpolateLayoutRect(entry.rcCellStart, entry.rcCellTarget, t);
    entry.rcThumbLayoutCurrent = InterpolateLayoutRect(entry.rcThumbStart, entry.rcThumbTarget, t);
    entry.rcThumbSlot = InterpolateLayoutRect(entry.rcThumbSlotStart, entry.rcThumbSlotTarget, t);
    if (DockLayoutActive()) {
        bool selected = g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() &&
                        &entry == &g_windows[g_selectedIndex] && DockShowPreview();
        entry.rcThumbLayoutCurrent = selected ? g_rcCentralPreview : RECT{};
        entry.rcThumbSlot = selected ? g_rcCentralPreviewSlot : RECT{};
    }
    ApplyEntryPresentationGeometry(entry);
}

static void SampleDepartingEntry(DepartingEntrySnapshot& entry, float dt) {
    StepItemTransition(entry.motion, entry.alpha, entry.scale, dt);
    float t = ItemScaleEasing(entry.motion);
    entry.rcCellCurrent = InterpolateLayoutRect(entry.rcCellStart, entry.rcCellTarget, t);
    entry.rcThumbCurrent = InterpolateLayoutRect(entry.rcThumbStart, entry.rcThumbTarget, t);
}

static void SampleOutgoingEntry(OutgoingItemSnapshot& entry, float dt) {
    StepItemTransition(entry.entryMotion, entry.alpha, entry.scale, dt);
    entry.rcCell = ScaleLayoutRect(entry.rcCellLayout, entry.scale);
    entry.rcThumbActual = ScaleLayoutRect(entry.rcThumbLayout, entry.scale);
}

static void RestoreCurrentLayoutGeometry() {
    float t = g_easeLayout.Solve(g_layoutTransition.progress);
    for (auto& entry : g_windows) {
        bool hadEntryMotion = entry.isNewEntry;
        if (entry.isNewEntry) {
            entry.isNewEntry = StepItemTransition(entry.entryMotion, entry.enterAlpha, entry.enterScale, 0.0f);
        }
        // Dry-run navigation writes rcCell but does not change track targets.
        // Reconstruct the last presented frame before capturing a new owner.
        if (g_layoutTransition.active) ApplyEntryLayoutGeometry(entry, t);
        else if (hadEntryMotion) ApplyEntryPresentationGeometry(entry);
    }
    for (auto& entry : g_layoutTransition.departingItems) {
        SampleDepartingEntry(entry, 0.0f);
    }
    for (auto& entry : g_scrollTransition.outgoingItems) {
        SampleOutgoingEntry(entry, 0.0f);
    }
}

static void CaptureLayoutTransitionStart() {
    RestoreCurrentLayoutGeometry();
    RECT bounds = {};
    GetPresentationLayoutRect(g_hSwitcher, &bounds);
    g_layoutTransition.rcWndStart = ToRectF(bounds);
    g_layoutTransition.rcDockStripStart = g_rcDockIconStrip;
    g_layoutTransition.rcDockPreviewStart = g_rcCentralPreview;
    g_layoutTransition.rcDockTitleStart = g_rcDockTitleBar;
    g_layoutTransition.rcDockSlotStart = g_rcCentralPreviewSlot;
    for (auto& entry : g_windows) {
        entry.rcCellStart = HasLayoutRect(entry.rcCellLayoutCurrent) ? entry.rcCellLayoutCurrent : entry.rcCell;
        entry.rcThumbStart = HasLayoutRect(entry.rcThumbLayoutCurrent) ? entry.rcThumbLayoutCurrent : entry.rcThumbActual;
        entry.rcThumbSlotStart = entry.rcThumbSlot;
    }
}

static DepartingEntrySnapshot CaptureDepartingEntry(const WindowEntry& entry) {
    DepartingEntrySnapshot snapshot = {};
    snapshot.hWnd = entry.hWnd;
    snapshot.rcCellStart = snapshot.rcCellCurrent = entry.rcCell;
    snapshot.rcThumbStart = snapshot.rcThumbCurrent = entry.rcThumbActual;
    snapshot.alpha = entry.isNewEntry ? entry.enterAlpha : 1.0f;
    snapshot.scale = entry.isNewEntry ? entry.enterScale : 1.0f;
    float targetRatio = 0.92f / snapshot.scale;
    snapshot.rcCellTarget = ScaleLayoutRect(entry.rcCell, targetRatio);
    snapshot.rcThumbTarget = ScaleLayoutRect(entry.rcThumbActual, targetRatio);
    StartItemTransition(snapshot.motion, snapshot.alpha, snapshot.scale, 0.0f, 0.92f);
    snapshot.hIcon = entry.hIcon;
    snapshot.iconCell = entry.iconCell;
    wcsncpy_s(snapshot.title, entry.title, _TRUNCATE);
    snapshot.hThumbs = entry.hThumbs;
    snapshot.groupWindows = entry.groupWindows;
    snapshot.sourceSize = entry.sourceSize;
    snapshot.rcSourceCrop = entry.rcSourceCrop;
    return snapshot;
}

static void CaptureGroupTransitionStart() {
    RestoreCurrentLayoutGeometry();
    if (g_scrollTransition.active) {
        int x = (int)roundf(g_scrollTransition.offsetCurrentX);
        int y = (int)roundf(g_scrollTransition.offsetCurrentY);
        // Group navigation takes over the viewport from the scroll. Preserve
        // displayed positions and transfer the outgoing handles to the fade.
        for (const auto& item : g_scrollTransition.outgoingItems) {
            int index = FindWindowIndexByHwnd(item.hWnd);
            if (index >= 0 && HasLayoutRect(g_windows[index].rcCell)) continue;
            WindowEntry frame = {};
            frame.hWnd = item.hWnd;
            frame.rcCell = item.rcCell;
            frame.rcThumbActual = item.rcThumbActual;
            OffsetRect(&frame.rcCell, x - g_scrollTransition.travelDistanceX,
                       y - g_scrollTransition.travelDistanceY);
            if (HasLayoutRect(frame.rcThumbActual)) {
                OffsetRect(&frame.rcThumbActual, x - g_scrollTransition.travelDistanceX,
                           y - g_scrollTransition.travelDistanceY);
            }
            frame.hIcon = item.hIcon;
            frame.iconCell = item.iconCell;
            wcsncpy_s(frame.title, item.title, _TRUNCATE);
            frame.hThumbs = item.hThumbs;
            frame.groupWindows = item.groupWindows;
            frame.sourceSize = item.sourceSize;
            frame.rcSourceCrop = item.rcSourceCrop;
            frame.enterAlpha = item.alpha;
            frame.enterScale = item.scale;
            frame.isNewEntry = item.alpha < 1.0f || item.scale != 1.0f;
            if (index >= 0) {
                for (const auto& thumbnail : item.hThumbs) {
                    auto owned = g_windows[index].hThumbs.find(thumbnail.first);
                    if (owned != g_windows[index].hThumbs.end() && owned->second == thumbnail.second) {
                        g_windows[index].hThumbs.erase(owned);
                    }
                }
            }
            g_layoutTransition.departingItems.push_back(CaptureDepartingEntry(frame));
        }
        for (auto& entry : g_windows) {
            if (!HasLayoutRect(entry.rcCell)) continue;
            OffsetRect(&entry.rcCell, x, y);
            if (HasLayoutRect(entry.rcCellLayoutCurrent)) OffsetRect(&entry.rcCellLayoutCurrent, x, y);
            if (!DockLayoutActive()) {
                if (HasLayoutRect(entry.rcThumbActual)) OffsetRect(&entry.rcThumbActual, x, y);
                if (HasLayoutRect(entry.rcThumbLayoutCurrent)) OffsetRect(&entry.rcThumbLayoutCurrent, x, y);
                if (HasLayoutRect(entry.rcThumbSlot)) OffsetRect(&entry.rcThumbSlot, x, y);
            }
        }
    }
    g_scrollTransition.active = false;
    g_scrollTransition.offsetCurrentX = g_scrollTransition.offsetCurrentY = 0.0f;
    g_scrollTransition.outgoingItems.clear();
    g_scrollTransition.preservingThumbnails = false;
    // The viewport has already been sampled above; avoid reconstructing its
    // pre-scroll cells a second time in CaptureLayoutTransitionStart.
    g_layoutTransition.active = false;
    CaptureLayoutTransitionStart();
}

static void ComputeTransitionLayout(HMONITOR monitor) {
    g_calculatingLayoutTargets = true;
    ComputeLayout(monitor);
    if (DockLayoutActive()) UpdateDockPreviewForSelection();
    g_calculatingLayoutTargets = false;
}

// Restore one complete frame zero before thumbnail submission or painting.
// Every reflow replaces the previous geometry targets, including navigation.
static void CommitLayoutTransition(HMONITOR monitor, bool scrollReflow = false) {
    MONITORINFO info = { sizeof(info) };
    GetMonitorInfoW(monitor, &info);
    int x, y;
    GetSwitcherPosition(info.rcWork, &x, &y, g_winW, g_winH);
    g_layoutTransition.rcWndTarget = { (float)x, (float)y,
        (float)(x + g_winW), (float)(y + g_winH) };
    g_layoutTransition.rcDockStripTarget = g_rcDockIconStrip;
    g_layoutTransition.rcDockPreviewTarget = g_rcCentralPreview;
    g_layoutTransition.rcDockTitleTarget = g_rcDockTitleBar;
    g_layoutTransition.rcDockSlotTarget = g_rcCentralPreviewSlot;
    g_layoutTransition.scrollReflow = scrollReflow;
    bool animate = g_isVisible && !g_isPendingShow && !g_animExitActive &&
                   AreAnimationsGloballyEnabled();
    for (auto& entry : g_windows) {
        entry.rcCellTarget = entry.rcCell;
        entry.rcThumbTarget = entry.rcThumbActual;
        entry.rcThumbSlotTarget = entry.rcThumbSlot;
        if (!HasLayoutRect(entry.rcCellTarget)) {
            entry.rcCellStart = entry.rcCell = {};
            entry.rcThumbStart = entry.rcThumbActual = {};
            entry.rcThumbSlotStart = entry.rcThumbSlot = {};
            entry.rcCellLayoutCurrent = entry.rcThumbLayoutCurrent = {};
            if (!animate) {
                entry.entryMotion = {};
                entry.isNewEntry = false;
                entry.enterAlpha = entry.enterScale = 1.0f;
            }
            continue;
        }
        if (!HasLayoutRect(entry.rcCellStart) && !scrollReflow) {
            if (!IsItemTransitionActive(entry.entryMotion)) {
                entry.isNewEntry = true;
                entry.enterAlpha = 0.0f;
                entry.enterScale = 0.94f;
                StartItemTransition(entry.entryMotion, entry.enterAlpha, entry.enterScale, 1.0f, 1.0f);
            }
            // New children fade/scale in place; only existing children reflow.
            // Pagination is a viewport slide, never an entry from empty bounds.
            entry.rcCellStart = entry.rcCellTarget;
            entry.rcThumbStart = entry.rcThumbTarget;
            entry.rcThumbSlotStart = entry.rcThumbSlotTarget;
        }
        if (!animate) {
            entry.isNewEntry = false;
            entry.entryMotion = {};
            entry.enterAlpha = entry.enterScale = 1.0f;
            entry.rcCellLayoutCurrent = entry.rcCellTarget;
            entry.rcThumbLayoutCurrent = entry.rcThumbTarget;
        } else {
            if (scrollReflow) {
                // Viewport translation belongs to the slide, not stale cells
                // from the layout that happened to precede this page change.
                entry.rcCellStart = entry.rcCellTarget;
                entry.rcThumbStart = entry.rcThumbTarget;
                entry.rcThumbSlotStart = entry.rcThumbSlotTarget;
            }
            entry.rcCellLayoutCurrent = entry.rcCellStart;
            entry.rcThumbLayoutCurrent = entry.rcThumbStart;
            entry.rcThumbSlot = entry.rcThumbSlotStart;
            ApplyEntryPresentationGeometry(entry);
        }
    }
    g_layoutTransition.active = animate;
    g_layoutTransition.duration = 0.250f;
    if (animate) {
        if (DockLayoutActive()) {
            g_rcDockIconStrip = g_layoutTransition.rcDockStripStart;
            g_rcCentralPreview = g_layoutTransition.rcDockPreviewStart;
            g_rcDockTitleBar = g_layoutTransition.rcDockTitleStart;
            g_rcCentralPreviewSlot = g_layoutTransition.rcDockSlotStart;
            if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
                g_windows[g_selectedIndex].rcThumbActual = g_rcCentralPreview;
                g_windows[g_selectedIndex].rcThumbSlot = g_rcCentralPreviewSlot;
                g_windows[g_selectedIndex].rcThumbLayoutCurrent = g_rcCentralPreview;
                ApplyEntryPresentationGeometry(g_windows[g_selectedIndex]);
            }
        }
        StartMotionTrack(g_layoutTransition.progress, 0.0f);
        StartAnimationTicker();
    } else {
        g_layoutTransition.progress = 1.0f;
        SetSwitcherLayoutBounds(x, y, g_winW, g_winH);
        for (auto& entry : g_layoutTransition.departingItems) {
            for (const auto& thumbnail : entry.hThumbs) {
                if (thumbnail.second) SafeDwmUnregisterThumbnail(thumbnail.second);
            }
        }
        g_layoutTransition.departingItems.clear();
    }
    if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        SyncSelectionAnimationToLayout();
    }
    InvalidateStaticCache();
    UpdateThumbnailAnimations();
}

static bool AdvanceItemTransitions(float dt) {
    bool anyActive = false;
    bool changed = false;
    for (auto& entry : g_windows) {
        if (!entry.isNewEntry) continue;
        changed = true;
        entry.isNewEntry = StepItemTransition(entry.entryMotion, entry.enterAlpha, entry.enterScale, dt);
        anyActive |= entry.isNewEntry;
        ApplyEntryPresentationGeometry(entry);
    }
    for (auto& entry : g_scrollTransition.outgoingItems) {
        if (!IsItemTransitionActive(entry.entryMotion)) continue;
        changed = true;
        SampleOutgoingEntry(entry, dt);
        anyActive |= IsItemTransitionActive(entry.entryMotion);
    }
    auto& departing = g_layoutTransition.departingItems;
    for (auto it = departing.begin(); it != departing.end();) {
        changed = true;
        SampleDepartingEntry(*it, dt);
        if (IsItemTransitionActive(it->motion)) {
            anyActive = true;
            ++it;
        } else {
            for (const auto& thumbnail : it->hThumbs) {
                if (thumbnail.second) SafeDwmUnregisterThumbnail(thumbnail.second);
            }
            it = departing.erase(it);
        }
    }
    if (changed) {
        if (g_layoutTransition.active && g_hoverThumbIndex >= 0 && g_hoverThumbIndex < (int)g_windows.size() &&
            HasLayoutRect(g_windows[g_hoverThumbIndex].rcThumbActual)) {
            g_animHoverCurrent = ToRectF(g_windows[g_hoverThumbIndex].rcThumbActual);
            g_animHoverTarget = g_animHoverStart = g_animHoverCurrent;
        }
        InvalidateStaticCache();
        // Include the terminal sample even when the layout track is inactive.
        if (!g_animFrameSampleActive) UpdateThumbnailAnimations();
    }
    return anyActive;
}

// Only static progress variables use this fixed registry. Per-entry clocks live
// with their WindowEntry, so vector relocation/erase/copy cannot leave stale keys.
static MotionTrackClock& GetMotionTrackClock(float& progress) {
    static struct {
        float* progress;
        MotionTrackClock clock;
    } tracks[] = {
        { &g_animEntranceProgress, {} }, { &g_animExitProgress, {} },
        { &g_animSelectionProgress, {} }, { &g_animHoverProgress, {} },
        { &g_animChevronProgressPrev, {} }, { &g_animChevronProgressNext, {} },
        { &g_scrollTransition.progress, {} }, { &g_layoutTransition.progress, {} },
        { &g_dockPreviewSlide.progress, {} },
    };
    for (auto& track : tracks) {
        if (track.progress == &progress) return track.clock;
    }
    // Per-window callers must use the overload taking their owned clock.
    std::terminate();
}

static LARGE_INTEGER GetMotionSampleTime() {
    if (g_animFrameSampleActive) return g_animLastTickTime;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return now;
}

static void StartMotionTrack(float& progress, MotionTrackClock& clock, float initial = 0.0f) {
    if (g_animPerfFreq.QuadPart == 0) QueryPerformanceFrequency(&g_animPerfFreq);
    clock.origin = GetMotionSampleTime();
    progress = initial;
    clock.initial = initial;
    clock.lastProgress = initial;
    clock.started = true;
}

static void StartMotionTrack(float& progress, float initial) {
    StartMotionTrack(progress, GetMotionTrackClock(progress), initial);
}

static bool StepMotionTrack(float& progress, MotionTrackClock& clock, float duration, float dt) {
    const bool reverse = duration < 0.0f;
    const float endpoint = reverse ? 0.0f : 1.0f;
    if ((!reverse && progress >= endpoint) || (reverse && progress <= endpoint)) return true;
    // Defensive legacy-reset detection. Actual starts must call StartMotionTrack
    // at the trigger, including a retarget at progress zero in the same frame.
    if (!clock.started || (!reverse && progress < clock.lastProgress) ||
        (reverse && progress > clock.lastProgress)) {
        StartMotionTrack(progress, clock, progress);
    }
    double elapsed;
    if (g_animPerfFreq.QuadPart > 0) {
        LARGE_INTEGER now = GetMotionSampleTime();
        elapsed = (std::max)(0.0, (double)(now.QuadPart - clock.origin.QuadPart) /
                                   (double)g_animPerfFreq.QuadPart);
    } else {
        elapsed = (std::max)(0.0f, dt);
        clock.initial = progress;
    }
    if (duration == 0.0f) {
        progress = endpoint;
    } else {
        float value = clock.initial + (float)(elapsed / duration);
        progress = reverse ? (std::max)(endpoint, value) : (std::min)(endpoint, value);
    }
    clock.lastProgress = progress;
    return progress == endpoint;
}

static bool StepMotionTrack(float& progress, float duration, float dt) {
    return StepMotionTrack(progress, GetMotionTrackClock(progress), duration, dt);
}

static void StartOpacityMotion(OpacityMotionTrack& track, float current, float target) {
    if (current == target) {
        track.progress = 1.0f;
        track.start = track.target = target;
        return;
    }
    if (track.target == target && track.progress < 1.0f) return;
    track.start = current;
    track.target = target;
    StartMotionTrack(track.progress, track.clock);
}

static bool StepOpacityMotion(OpacityMotionTrack& track, float& current, float dt) {
    if (track.progress >= 1.0f) return false;
    bool done = StepMotionTrack(track.progress, track.clock, 0.083f, dt);
    current = done ? track.target : track.start + (track.target - track.start) * track.progress;
    return !done;
}

static void SnapCloseButtonMotion(WindowEntry& entry, float alpha) {
    entry.closeBtnAlpha = alpha;
    entry.closeBtnMotion = {};
    entry.closeBtnMotion.start = entry.closeBtnMotion.target = alpha;
    float scale = alpha > 0.0f ? 1.0f : 0.85f;
    entry.closeBtnScale = entry.closeBtnScaleStart = entry.closeBtnScaleTarget = scale;
    entry.closeBtnScaleProgress = 1.0f;
    entry.closeBtnScaleClock = {};
}

static void UpdateCloseButtonMotionTarget(WindowEntry& entry, float targetAlpha) {
    if (!AreAnimationsGloballyEnabled() || !g_settings.enableHoverAnimation) {
        SnapCloseButtonMotion(entry, targetAlpha);
        return;
    }
    StartOpacityMotion(entry.closeBtnMotion, entry.closeBtnAlpha, targetAlpha);
    float targetScale = targetAlpha > 0.0f ? 1.0f : 0.85f;
    if (targetScale != entry.closeBtnScaleTarget) {
        // Capture the presented transform, independently of the fade. A rapid
        // hover reversal neither jumps scale nor restarts any concurrent clock.
        entry.closeBtnScaleStart = entry.closeBtnScale;
        entry.closeBtnScaleTarget = targetScale;
        StartMotionTrack(entry.closeBtnScaleProgress, entry.closeBtnScaleClock);
    }
}

static bool StepCloseButtonMotion(WindowEntry& entry, float dt) {
    bool opacityActive = StepOpacityMotion(entry.closeBtnMotion, entry.closeBtnAlpha, dt);
    bool scaleDone = StepMotionTrack(entry.closeBtnScaleProgress, entry.closeBtnScaleClock, 0.167f, dt);
    if (scaleDone) {
        entry.closeBtnScale = entry.closeBtnScaleTarget;
    } else {
        float eased = entry.closeBtnScaleTarget < entry.closeBtnScaleStart
            ? g_easeItemExit.Solve(entry.closeBtnScaleProgress)
            : g_easeEntrance.Solve(entry.closeBtnScaleProgress);
        entry.closeBtnScale = entry.closeBtnScaleStart +
            (entry.closeBtnScaleTarget - entry.closeBtnScaleStart) * eased;
    }
    // The fade can finish at 83 ms; keep the invisible collapse clock alive to
    // its own 167 ms endpoint so a later hover starts from a settled transform.
    return opacityActive || !scaleDone;
}

// Called at input/hover triggers as well as on ticks for geometry-driven changes.
// Starting one of these tracks never changes another track's origin or deadline.
static void UpdateMicrointeractionTargets() {
    StartOpacityMotion(g_chevronHoverPrev, g_animChevronHoverAlphaPrev, g_hoverChevron == -1 ? 1.0f : 0.0f);
    StartOpacityMotion(g_chevronHoverNext, g_animChevronHoverAlphaNext, g_hoverChevron == 1 ? 1.0f : 0.0f);
    for (int i = 0; i < (int)g_windows.size(); i++) {
        auto& w = g_windows[i];
        float targetAlpha = (i == g_hoverIndex && g_settings.showCloseButton && !IsWindowTruncated(i)) ? 1.0f : 0.0f;
        if (DockLayoutActive() && DockCloseButtonIsHidden()) targetAlpha = 0.0f;
        UpdateCloseButtonMotionTarget(w, targetAlpha);
        float targetScale = (ThumbnailHoverIsZoom() && i == g_hoverThumbIndex && !IsWindowTruncated(i)) ?
                                (1.0f + SWS_HOVER_ZOOM_DELTA) : 1.0f;
        if (fabsf(targetScale - w.hoverScaleTarget) > 0.0001f) {
            w.hoverScaleStart = w.hoverScale;
            w.hoverScaleTarget = targetScale;
            w.hoverScaleDuration = 0.167f;
            StartMotionTrack(w.hoverScaleProgress, w.hoverScaleClock);
        }
    }
    float targetHover = (g_isCloseHovered && g_hoverIndex >= 0 &&
                        g_hoverIndex < (int)g_windows.size()) ? 1.0f : 0.0f;
    StartOpacityMotion(g_closeBtnHover, g_animCloseBtnHoverAlpha, targetHover);
}

// Cached GDI buffers for 0-allocation rendering
static HDC s_cachedMemDC = NULL;
static HBITMAP s_cachedBitmap = NULL;
static HBITMAP s_cachedOldBitmap = NULL;
static void* s_cachedMemBits = NULL;
static int s_cachedW = 0, s_cachedH = 0;

static HDC s_cachedOverlayDC = NULL;
static HBITMAP s_cachedOverlayBitmap = NULL;
static HBITMAP s_cachedOverlayOldBitmap = NULL;
static void* s_cachedOverlayBits = NULL;
static int s_cachedOverlayW = 0, s_cachedOverlayH = 0;

// Static backing buffer cache for overlay (badge icon backgrounds, shadows, group badges) (<0.05ms BitBlt)
static HDC s_cachedOverlayStaticDC = NULL;
static HBITMAP s_cachedOverlayStaticBitmap = NULL;
static HBITMAP s_cachedOverlayStaticOldBitmap = NULL;
static void* s_cachedOverlayStaticBits = NULL;
static int s_cachedOverlayStaticW = 0, s_cachedOverlayStaticH = 0;
static bool g_overlayStaticDirty = true;

// Static backing buffer cache for instant micro-interaction rendering (<0.1ms BitBlt)
static HDC s_cachedStaticDC = NULL;
static HBITMAP s_cachedStaticBitmap = NULL;
static HBITMAP s_cachedStaticOldBitmap = NULL;
static void* s_cachedStaticBits = NULL;
static int s_cachedStaticW = 0, s_cachedStaticH = 0;
static bool g_staticContentDirty = true;

// Dual-canvas pre-rendered buffers for 144Hz buttery smooth overflow/page slide transitions (<0.1ms BitBlt)
static HDC s_cachedScrollFromDC = NULL;
static HBITMAP s_cachedScrollFromBitmap = NULL;
static HBITMAP s_cachedScrollFromOldBitmap = NULL;
static void* s_cachedScrollFromBits = NULL;
static int s_cachedScrollFromW = 0, s_cachedScrollFromH = 0;

static HDC s_cachedScrollToDC = NULL;
static HBITMAP s_cachedScrollToBitmap = NULL;
static HBITMAP s_cachedScrollToOldBitmap = NULL;
static void* s_cachedScrollToBits = NULL;
static int s_cachedScrollToW = 0, s_cachedScrollToH = 0;
static bool s_scrollIconCanvasDirty = false;
static bool s_iconRepaintPending = false;

static void EnsureScrollBuffers(int w, int h) {
    if (w <= 0 || h <= 0) return;
    HDC hdcScreen = GetDC(NULL);
    if (!s_cachedScrollFromDC || s_cachedScrollFromW != w || s_cachedScrollFromH != h) {
        if (s_cachedScrollFromDC) {
            if (s_cachedScrollFromOldBitmap) SelectObject(s_cachedScrollFromDC, s_cachedScrollFromOldBitmap);
            if (s_cachedScrollFromBitmap) DeleteObject(s_cachedScrollFromBitmap);
            DeleteDC(s_cachedScrollFromDC);
        }
        s_cachedScrollFromDC = CreateCompatibleDC(hdcScreen);
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
        s_cachedScrollFromBitmap = CreateDIBSection(s_cachedScrollFromDC, &bmi, DIB_RGB_COLORS, &s_cachedScrollFromBits, NULL, 0);
        s_cachedScrollFromOldBitmap = (HBITMAP)SelectObject(s_cachedScrollFromDC, s_cachedScrollFromBitmap);
        s_cachedScrollFromW = w; s_cachedScrollFromH = h;
    }
    if (!s_cachedScrollToDC || s_cachedScrollToW != w || s_cachedScrollToH != h) {
        if (s_cachedScrollToDC) {
            if (s_cachedScrollToOldBitmap) SelectObject(s_cachedScrollToDC, s_cachedScrollToOldBitmap);
            if (s_cachedScrollToBitmap) DeleteObject(s_cachedScrollToBitmap);
            DeleteDC(s_cachedScrollToDC);
        }
        s_cachedScrollToDC = CreateCompatibleDC(hdcScreen);
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
        s_cachedScrollToBitmap = CreateDIBSection(s_cachedScrollToDC, &bmi, DIB_RGB_COLORS, &s_cachedScrollToBits, NULL, 0);
        s_cachedScrollToOldBitmap = (HBITMAP)SelectObject(s_cachedScrollToDC, s_cachedScrollToBitmap);
        s_cachedScrollToW = w; s_cachedScrollToH = h;
    }
    ReleaseDC(NULL, hdcScreen);
}

static void InvalidateStaticCache() {
    g_staticContentDirty = true;
    g_overlayStaticDirty = true;
}

static HRGN s_cachedRoundRectRgn = NULL;
static int s_cachedRgnW = 0, s_cachedRgnH = 0, s_cachedRgnRadius = 0;

static HRGN GetCachedRoundRectRgn(int w, int h, int radius) {
    if (radius <= 0 || w <= 0 || h <= 0) return NULL;
    if (!s_cachedRoundRectRgn || s_cachedRgnW != w || s_cachedRgnH != h || s_cachedRgnRadius != radius) {
        if (s_cachedRoundRectRgn) {
            DeleteObject(s_cachedRoundRectRgn);
            s_cachedRoundRectRgn = NULL;
        }
        s_cachedRoundRectRgn = CreateRoundRectRgn(0, 0, w + 1, h + 1, radius * 2, radius * 2);
        s_cachedRgnW = w;
        s_cachedRgnH = h;
        s_cachedRgnRadius = radius;
    }
    return s_cachedRoundRectRgn;
}

// Reusable scratch DC + DIB for alpha-blended icon drawing, keyed by icon size.
// Avoids allocating a fresh DC + DIB section per icon per frame during fades and
// for dimmed minimized icons. Flushed below in FreeCachedBuffers(). The DIB stays
// selected into its DC for the cache lifetime, and its pixel pointer is stable.
struct IconAlphaScratch { HDC hdc = nullptr; HBITMAP dib = nullptr; void* bits = nullptr; };
static std::map<int, IconAlphaScratch> s_iconAlphaScratch;

// UI-owned shadow records retain the exact icon allocation, even after its
// window/cell retires. A draw also holds the record across reentrant cache
// removal. The bitmap is destroyed before the retained icon is released.
struct WindowIconShadow {
    std::shared_ptr<OwnedWindowIcon> owner;
    std::unique_ptr<Gdiplus::Bitmap> bitmap;
};
static std::map<std::pair<HICON, int>, std::shared_ptr<WindowIconShadow>> s_iconShadowCache;
static void SweepWindowIconShadows();

// Only actual presentation (or the scheduled scroll finish-copy) rebuilds a
// canvas dirtied by icon results. Clearing first preserves reentrant dirties.
static void RefreshScrollIconCanvas() {
    if (!s_scrollIconCanvasDirty || !s_cachedScrollToDC ||
        s_cachedScrollToW <= 0 || s_cachedScrollToH <= 0) return;
    s_scrollIconCanvasDirty = false;
    if (s_cachedScrollToBits)
        memset(s_cachedScrollToBits, 0, (size_t)s_cachedScrollToW * s_cachedScrollToH * sizeof(DWORD));
    SelectClipRgn(s_cachedScrollToDC,
                  ThemeIs(L"none") ? NULL :
                  GetCachedRoundRectRgn(s_cachedScrollToW, s_cachedScrollToH, GetWindowCornerRadiusPx()));
    DrawSwitcherStaticContent(s_cachedScrollToDC, ShouldFillBackground(), g_hSwitcher);
}

static void FreeCachedBuffers() {
    if (s_cachedRoundRectRgn) {
        DeleteObject(s_cachedRoundRectRgn);
        s_cachedRoundRectRgn = NULL;
        s_cachedRgnW = 0;
        s_cachedRgnH = 0;
        s_cachedRgnRadius = 0;
    }
    if (s_cachedMemDC) {
        if (s_cachedOldBitmap) SelectObject(s_cachedMemDC, s_cachedOldBitmap);
        if (s_cachedBitmap) DeleteObject(s_cachedBitmap);
        DeleteDC(s_cachedMemDC);
        s_cachedMemDC = NULL;
        s_cachedBitmap = NULL;
        s_cachedOldBitmap = NULL;
        s_cachedMemBits = NULL;
        s_cachedW = 0;
        s_cachedH = 0;
    }
    if (s_cachedOverlayDC) {
        if (s_cachedOverlayOldBitmap) SelectObject(s_cachedOverlayDC, s_cachedOverlayOldBitmap);
        if (s_cachedOverlayBitmap) DeleteObject(s_cachedOverlayBitmap);
        DeleteDC(s_cachedOverlayDC);
        s_cachedOverlayDC = NULL;
        s_cachedOverlayBitmap = NULL;
        s_cachedOverlayOldBitmap = NULL;
        s_cachedOverlayBits = NULL;
        s_cachedOverlayW = 0;
        s_cachedOverlayH = 0;
    }
    if (s_cachedOverlayStaticDC) {
        if (s_cachedOverlayStaticOldBitmap) SelectObject(s_cachedOverlayStaticDC, s_cachedOverlayStaticOldBitmap);
        if (s_cachedOverlayStaticBitmap) DeleteObject(s_cachedOverlayStaticBitmap);
        DeleteDC(s_cachedOverlayStaticDC);
        s_cachedOverlayStaticDC = NULL;
        s_cachedOverlayStaticBitmap = NULL;
        s_cachedOverlayStaticOldBitmap = NULL;
        s_cachedOverlayStaticBits = NULL;
        s_cachedOverlayStaticW = 0;
        s_cachedOverlayStaticH = 0;
    }
    if (s_cachedStaticDC) {
        if (s_cachedStaticOldBitmap) SelectObject(s_cachedStaticDC, s_cachedStaticOldBitmap);
        if (s_cachedStaticBitmap) DeleteObject(s_cachedStaticBitmap);
        DeleteDC(s_cachedStaticDC);
        s_cachedStaticDC = NULL;
        s_cachedStaticBitmap = NULL;
        s_cachedStaticOldBitmap = NULL;
        s_cachedStaticBits = NULL;
        s_cachedStaticW = 0;
        s_cachedStaticH = 0;
    }
    if (s_cachedScrollFromDC) {
        if (s_cachedScrollFromOldBitmap) SelectObject(s_cachedScrollFromDC, s_cachedScrollFromOldBitmap);
        if (s_cachedScrollFromBitmap) DeleteObject(s_cachedScrollFromBitmap);
        DeleteDC(s_cachedScrollFromDC);
        s_cachedScrollFromDC = NULL;
        s_cachedScrollFromBitmap = NULL;
        s_cachedScrollFromOldBitmap = NULL;
        s_cachedScrollFromBits = NULL;
        s_cachedScrollFromW = 0;
        s_cachedScrollFromH = 0;
    }
    if (s_cachedScrollToDC) {
        if (s_cachedScrollToOldBitmap) SelectObject(s_cachedScrollToDC, s_cachedScrollToOldBitmap);
        if (s_cachedScrollToBitmap) DeleteObject(s_cachedScrollToBitmap);
        DeleteDC(s_cachedScrollToDC);
        s_cachedScrollToDC = NULL;
        s_cachedScrollToBitmap = NULL;
        s_cachedScrollToOldBitmap = NULL;
        s_cachedScrollToBits = NULL;
        s_cachedScrollToW = 0;
        s_cachedScrollToH = 0;
    }
    for (auto& kv : s_iconAlphaScratch) {
        if (kv.second.dib) DeleteObject(kv.second.dib); // still selected into hdc; deleting detaches it
        if (kv.second.hdc) DeleteDC(kv.second.hdc);
    }
    s_iconAlphaScratch.clear();
    s_iconShadowCache.clear();
    s_scrollIconCanvasDirty = false;
    s_iconRepaintPending = false;
    g_staticContentDirty = true;
}

static bool s_clientAreaAnimCached = true;

static void RefreshClientAreaAnimCache() {
    BOOL clientAnim = TRUE;
    if (SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &clientAnim, 0)) {
        s_clientAreaAnimCached = (clientAnim != FALSE);
    } else {
        s_clientAreaAnimCached = true;
    }
}

static bool AreAnimationsGloballyEnabled() {
    if (!g_settings.enableAnimations) return false;
    return s_clientAreaAnimCached;
}

static void StartAnimationTicker() {
    if (!g_hSwitcher) return;
    // Entrance clocks belong to the presentation boundary. A pending-show
    // navigation may already have started the ticker, so this must run even
    // when another animation keeps g_animActive set.
    BeginEntranceAnimationClock();
    if (!g_animActive) {
        // Use the cached interval on the animation-critical path. Refresh-rate
        // maintenance is performed at invocation/display-change boundaries,
        // never as a synchronous restart cost.
        if (g_animPerfFreq.QuadPart == 0) {
            QueryPerformanceFrequency(&g_animPerfFreq);
        }
        QueryPerformanceCounter(&g_animLastTickTime);
        g_animNextFrameDeadline = (double)g_animLastTickTime.QuadPart;
        g_animActive = true;
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
        PostMessageW(g_hSwitcher, WM_NULL, 0, 0); // Wake wait loop immediately
    }
}

static void StopAnimationTicker() {
    g_animActive = false;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
}

static void FinishAnimations() {
    ResetPresentationAnimation();
    if (g_layoutTransition.active) {
        g_layoutTransition.active = false;
        g_layoutTransition.progress = 1.0f;
        for (auto& item : g_layoutTransition.departingItems) {
            for (const auto& kv : item.hThumbs) {
                if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
            }
        }
        g_layoutTransition.departingItems.clear();
        for (auto& w : g_windows) {
            w.rcCell = w.rcCellTarget;
            w.rcThumbActual = w.rcThumbTarget;
            w.isNewEntry = false;
        }
    }
    for (auto& w : g_windows) {
        w.entryMotion = {};
        w.enterAlpha = w.enterScale = 1.0f;
        w.isNewEntry = false;
        w.rcCellLayoutCurrent = w.rcCell;
        w.rcThumbLayoutCurrent = w.rcThumbActual;
    }
    g_animSelectionActive = false;
    if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        RectF r = ToRectF(g_windows[g_selectedIndex].rcCell);
        SnapSelectionTo(r);
    }
    g_scrollTransition.active = false;
    g_scrollTransition.offsetCurrentX = 0.0f;
    g_scrollTransition.offsetCurrentY = 0.0f;
    for (auto& item : g_scrollTransition.outgoingItems) {
        int curIdx = FindWindowIndexByHwnd(item.hWnd);
        if (curIdx == -1 || IsWindowTruncated(curIdx)) {
            for (const auto& kv : item.hThumbs) {
                if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
            }
            if (curIdx != -1) {
                g_windows[curIdx].hThumbs.clear();
            }
        }
    }
    g_scrollTransition.outgoingItems.clear();
    g_scrollTransition.preservingThumbnails = false;
    g_dockPreviewSlide.active = false;
    g_dockPreviewSlide.progress = 1.0f;
    g_dockPreviewSlide.currentOffset = 0.0f;
    g_dockPreviewSlide.currentAlpha = 1.0f;
    g_animHoverActive = false;
    g_animHoverAlphaCurrent = 0.0f;
    g_animChevronAlphaPrev = g_animChevronAlphaTargetPrev;
    g_animChevronAlphaNext = g_animChevronAlphaTargetNext;
    g_animChevronTransformPrev = g_animChevronAlphaTargetPrev;
    g_animChevronTransformNext = g_animChevronAlphaTargetNext;
    g_chevronRevealPrev = {};
    g_chevronRevealNext = {};
    g_animChevronProgressPrev = 1.0f;
    g_animChevronProgressNext = 1.0f;
    g_animHoverScaleActive = false;
    g_animCloseBtnAlpha = (g_hoverIndex >= 0 && g_settings.showCloseButton && !IsWindowTruncated(g_hoverIndex)) ? 1.0f : 0.0f;
    g_animCloseBtnHoverAlpha = g_isCloseHovered ? 1.0f : 0.0f;
    for (int i = 0; i < (int)g_windows.size(); i++) {
        SnapCloseButtonMotion(g_windows[i], (i == g_hoverIndex && g_settings.showCloseButton && !IsWindowTruncated(i)) ? 1.0f : 0.0f);
        g_windows[i].hoverScale = 1.0f;
        g_windows[i].hoverScaleStart = 1.0f;
        g_windows[i].hoverScaleTarget = 1.0f;
        g_windows[i].hoverScaleProgress = 1.0f;
        g_windows[i].hoverScaleDuration = 0.167f;
        g_windows[i].hoverScaleClock = {};
    }
    g_hoverOpacity = {};
    g_chevronHoverPrev = {};
    g_chevronHoverNext = {};
    g_closeBtnHover = {};
}

static void CompleteExitAnimation() {
    if (!g_animExitActive) return;
    // Selection was committed at release. Completion only retires the visuals,
    // including when accessibility settings interrupt the exit.
    g_animExitProgress = 0.0f;
    g_animExitCurrentAlpha = 0.0f;
    HideSwitcherPresentationWindows();
    BackdropApplyAlpha(g_hBackdropWnd, 0.0f);
    HideSwitcher();
}

static void CaptureOutgoingSnapshot() {
    RestoreCurrentLayoutGeometry();
    g_scrollTransition.capturedOffsetX = g_scrollTransition.offsetCurrentX;
    g_scrollTransition.capturedOffsetY = g_scrollTransition.offsetCurrentY;
    if (g_hSwitcher && (g_staticContentDirty || g_scrollTransition.active)) {
        RECT rc; GetClientRect(g_hSwitcher, &rc);
        int w = rc.right, h = rc.bottom;
        if (w > 0 && h > 0 && s_cachedStaticDC && s_cachedStaticW == w && s_cachedStaticH == h) {
            if (g_scrollTransition.active && s_cachedScrollToDC && s_cachedScrollToW == w && s_cachedScrollToH == h) {
                if (s_cachedStaticBits) memset(s_cachedStaticBits, 0, (size_t)w * h * sizeof(DWORD));
                DrawScrollTransitionFrame(s_cachedStaticDC, w, h, false);
                g_staticContentDirty = false;
            } else {
                int radius = GetWindowCornerRadiusPx();
                if (s_cachedStaticBits) memset(s_cachedStaticBits, 0, (size_t)w * h * sizeof(DWORD));
                HRGN hClip = ThemeIs(L"none") ? NULL : GetCachedRoundRectRgn(w, h, radius);
                SelectClipRgn(s_cachedStaticDC, hClip);
                DrawSwitcherStaticContent(s_cachedStaticDC, ShouldFillBackground(), g_hSwitcher);
                g_staticContentDirty = false;
            }
        }
    }
    auto previousOutgoing = std::move(g_scrollTransition.outgoingItems);
    g_scrollTransition.outgoingItems.clear();
    if (g_scrollTransition.active) {
        for (auto& item : previousOutgoing) {
            int curIdx = FindWindowIndexByHwnd(item.hWnd);
            if (curIdx == -1 || IsWindowTruncated(curIdx)) {
                int x = (int)roundf(g_scrollTransition.offsetCurrentX) - g_scrollTransition.travelDistanceX;
                int y = (int)roundf(g_scrollTransition.offsetCurrentY) - g_scrollTransition.travelDistanceY;
                OffsetRect(&item.rcCell, x, y);
                OffsetRect(&item.rcThumbActual, x, y);
                OffsetRect(&item.rcThumbSlot, x, y);
                OffsetRect(&item.rcCellLayout, x, y);
                OffsetRect(&item.rcThumbLayout, x, y);
                item.drawnIconX += x;
                item.drawnIconY += y;
                g_scrollTransition.outgoingItems.push_back(std::move(item));
            }
        }
    }
    for (int i = 0; i < (int)g_windows.size(); i++) {
        const auto& w = g_windows[i];
        if (w.rcCell.left == 0 && w.rcCell.right == 0 &&
            w.rcCell.top == 0 && w.rcCell.bottom == 0) continue;

        OutgoingItemSnapshot snap;
        snap.hWnd = w.hWnd;
        snap.windowIndex = i;
        snap.rcCell = w.rcCell;
        snap.rcThumbActual = w.rcThumbActual;
        snap.rcThumbSlot = w.rcThumbSlot;
        snap.rcCellLayout = HasLayoutRect(w.rcCellLayoutCurrent) ? w.rcCellLayoutCurrent : w.rcCell;
        snap.rcThumbLayout = HasLayoutRect(w.rcThumbLayoutCurrent) ? w.rcThumbLayoutCurrent : w.rcThumbActual;
        int x = (int)roundf(g_scrollTransition.offsetCurrentX);
        int y = (int)roundf(g_scrollTransition.offsetCurrentY);
        OffsetRect(&snap.rcCell, x, y);
        OffsetRect(&snap.rcThumbActual, x, y);
        OffsetRect(&snap.rcThumbSlot, x, y);
        OffsetRect(&snap.rcCellLayout, x, y);
        OffsetRect(&snap.rcThumbLayout, x, y);
        wcsncpy_s(snap.title, w.title, _countof(snap.title));
        snap.hIcon = w.hIcon;
        snap.iconCell = w.iconCell;
        snap.hThumbs = w.hThumbs;
        snap.drawnIconX = w.drawnIconX + x;
        snap.drawnIconY = w.drawnIconY + y;
        snap.drawnIconSz = w.drawnIconSz;
        snap.groupWindows = w.groupWindows;
        snap.sourceSize = w.sourceSize;
        snap.rcSourceCrop = w.rcSourceCrop;
        snap.effectiveSourceSize = w.effectiveSourceSize;
        snap.alpha = w.isNewEntry ? w.enterAlpha : 1.0f;
        snap.scale = w.isNewEntry ? w.enterScale : 1.0f;
        snap.entryMotion = w.entryMotion;
        g_scrollTransition.outgoingItems.push_back(snap);
    }
    g_scrollTransition.preservingThumbnails = true;
}

static void TriggerSelectionAnimation(int prevSelected) {
    InvalidateStaticCache();
    (void)prevSelected;
    if (g_selectedIndex < 0 || g_selectedIndex >= (int)g_windows.size()) return;
    RECT newCell = g_windows[g_selectedIndex].rcCell;
    if (newCell.left == 0 && newCell.right == 0 && newCell.top == 0 && newCell.bottom == 0) return;

    RectF targetRect = ToRectF(newCell);

    if (!AreAnimationsGloballyEnabled() || !g_settings.enableSelectionAnimation) {
        SnapSelectionTo(targetRect);
        return;
    }

    if (!g_animSelectionActive &&
        (g_animSelectionCurrent.left == 0 && g_animSelectionCurrent.right == 0 &&
         g_animSelectionCurrent.top == 0 && g_animSelectionCurrent.bottom == 0)) {
        SnapSelectionTo(targetRect);
        return;
    }

    g_animSelectionStart = g_animSelectionCurrent;
    g_animSelectionTarget = targetRect;
    StartMotionTrack(g_animSelectionProgress);
    g_animSelectionDuration = 0.167f;
    g_animSelectionActive = true;
    StartAnimationTicker();
}

static void PreRenderScrollCanvases() {
    if (!g_hSwitcher) return;
    RECT rc; GetClientRect(g_hSwitcher, &rc);
    int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0) return;
    EnsureScrollBuffers(w, h);

    int radius = GetWindowCornerRadiusPx();

    // 1. Pre-render Outgoing Canvas: copy from s_cachedStaticDC (holding previous view before reflow)
    if (s_cachedStaticDC && s_cachedStaticW == w && s_cachedStaticH == h) {
        BitBlt(s_cachedScrollFromDC, 0, 0, w, h, s_cachedStaticDC, 0, 0, SRCCOPY);
    } else {
        if (s_cachedScrollFromBits) memset(s_cachedScrollFromBits, 0, (size_t)w * h * sizeof(DWORD));
        HRGN hClip = ThemeIs(L"none") ? NULL : GetCachedRoundRectRgn(w, h, radius);
        SelectClipRgn(s_cachedScrollFromDC, hClip);
        DrawSwitcherStaticContent(s_cachedScrollFromDC, ShouldFillBackground(), g_hSwitcher);
    }

    // 2. Pre-render Incoming Canvas: render incoming layout at rest (offset 0,0)
    if (s_cachedScrollToBits) memset(s_cachedScrollToBits, 0, (size_t)w * h * sizeof(DWORD));
    // Layered canvases keep a rectangular background. Only the final presented
    // frame is rounded; moving an already-rounded canvas exposes alpha holes.
    HRGN hClip = ThemeIs(L"none") ? NULL : GetCachedRoundRectRgn(w, h, radius);
    SelectClipRgn(s_cachedScrollToDC, hClip);
    s_scrollIconCanvasDirty = false;
    DrawSwitcherStaticContent(s_cachedScrollToDC, ShouldFillBackground(), g_hSwitcher);
}

static void TriggerScrollAnimationEx(int dir, ScrollNavType type) {
    if (!AreAnimationsGloballyEnabled() || !g_settings.enableScrollAnimation) {
        g_scrollTransition.active = false;
        g_scrollTransition.offsetCurrentX = 0.0f;
        g_scrollTransition.offsetCurrentY = 0.0f;
        for (auto& item : g_scrollTransition.outgoingItems) {
            int curIdx = FindWindowIndexByHwnd(item.hWnd);
            if (curIdx == -1) {
                for (const auto& kv : item.hThumbs) {
                    if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
                }
            } else if (IsWindowTruncated(curIdx)) {
                for (const auto& kv : item.hThumbs) {
                    if (kv.second) {
                        DWM_THUMBNAIL_PROPERTIES p = {};
                        p.dwFlags = DWM_TNP_VISIBLE;
                        p.fVisible = FALSE;
                        UpdateDwmThumbnail(kv.second, &p);
                    }
                }
            }
        }
        g_scrollTransition.outgoingItems.clear();
        g_scrollTransition.preservingThumbnails = false;
        UpdateHoverFromCursor(false);
        return;
    }

    // Invalidate hover state immediately when scroll starts so old hover contour doesn't linger
    g_hoverIndex = -1;
    g_hoverThumbIndex = -1;
    g_hoverWnd = NULL;
    g_isCloseHovered = false;
    g_animHoverActive = false;
    g_animHoverAlphaCurrent = 0.0f;
    g_animHoverAlphaTarget = 0.0f;

    bool horizontalScroll = DockLayoutActive();
    bool vertical = LayoutIsVertical() && !horizontalScroll;
    float deltaX = 0.0f;
    float deltaY = 0.0f;
    bool computedFromShared = false;

    if (horizontalScroll) {
        for (const auto& snap : g_scrollTransition.outgoingItems) {
            int idx = FindWindowIndexByHwnd(snap.hWnd);
            if (idx != -1 && !IsWindowTruncated(idx)) {
                const auto& cur = g_windows[idx];
                int diffX = snap.rcCell.left - cur.rcCell.left;
                deltaX = (float)diffX;
                computedFromShared = true;
                break;
            }
        }
        if (!computedFromShared) {
            int iconSz = DpiScale(g_settings.dockIconSize > 0 ? g_settings.dockIconSize : 48, g_dpiX);
            int cellPad = DpiScale(8, g_dpiX);
            int cellW = iconSz + cellPad * 2;
            int spacing = DpiScale(g_settings.dockIconSpacing, g_dpiX);
            int step = (type == SCROLL_PAGE) ? (g_rcDockIconStrip.right - g_rcDockIconStrip.left) : (cellW + spacing);
            deltaX = (float)(dir * step);
        }
        deltaY = 0.0f;
    } else if (type == SCROLL_ROW) {
        // First try: calculate exact signed pixel displacement from any shared window.
        // If an item was at snap.rcCell and is now at cur.rcCell, it must start at
        // cur.rcCell + delta = snap.rcCell => delta = snap.rcCell - cur.rcCell.
        for (const auto& snap : g_scrollTransition.outgoingItems) {
            int idx = FindWindowIndexByHwnd(snap.hWnd);
            if (idx != -1 && !IsWindowTruncated(idx)) {
                const auto& cur = g_windows[idx];
                int diffX = snap.rcCell.left - cur.rcCell.left;
                int diffY = snap.rcCell.top - cur.rcCell.top;
                if (vertical && (diffX != 0 || diffY == 0)) {
                    deltaX = (float)diffX;
                    computedFromShared = true;
                    break;
                } else if (!vertical && (diffY != 0 || diffX == 0)) {
                    deltaY = (float)diffY;
                    computedFromShared = true;
                    break;
                }
            }
        }
    }

    if (!computedFromShared && !horizontalScroll) {
        int travel = 0;
        int masterPadX = DpiScale(g_settings.switcherPadding, g_dpiX);
        int masterPadY = DpiScale(g_settings.switcherPadding, g_dpiY);
        if (type == SCROLL_PAGE) {
            travel = vertical ? (g_winW - 2 * masterPadX) : (g_winH - 2 * masterPadY);
            if (travel <= 0) travel = vertical ? g_winW : g_winH;
            if (travel <= 0) travel = DpiScale(g_settings.rowHeight * 2, vertical ? g_dpiX : g_dpiY);
        } else {
            // SCROLL_ROW with disjoint windows (e.g. multi-row jump or wrap):
            // travel full viewport content dimension so all outgoing items exit cleanly
            travel = vertical ? (g_winW - 2 * masterPadX) : (g_winH - 2 * masterPadY);
            if (travel <= 0) travel = vertical ? g_winW : g_winH;
            if (travel <= 0) {
                int rowH = DpiScale(g_settings.rowHeight, vertical ? g_dpiX : g_dpiY);
                int rowSpacing = DpiScale(10, vertical ? g_dpiX : g_dpiY);
                travel = rowH + rowSpacing;
            }
        }

        deltaX = vertical ? (float)(dir * travel) : 0.0f;
        deltaY = vertical ? 0.0f : (float)(dir * travel);
    }

    g_scrollTransition.offsetStartX = deltaX + (computedFromShared ? 0.0f : g_scrollTransition.capturedOffsetX);
    g_scrollTransition.offsetStartY = deltaY + (computedFromShared ? 0.0f : g_scrollTransition.capturedOffsetY);
    g_scrollTransition.offsetCurrentX = g_scrollTransition.offsetStartX;
    g_scrollTransition.offsetCurrentY = g_scrollTransition.offsetStartY;
    StartMotionTrack(g_scrollTransition.progress, 0.0f);
    g_scrollTransition.duration = (type == SCROLL_PAGE) ? 0.250f : 0.167f;

    g_scrollTransition.travelDistanceX = (int)roundf(g_scrollTransition.offsetStartX);
    g_scrollTransition.travelDistanceY = (int)roundf(g_scrollTransition.offsetStartY);

    PreRenderScrollCanvases();
    g_scrollTransition.active = true;

    UpdateChevronAnimationTargets(false);
    StartAnimationTicker();
}

static void TriggerHoverAnimation(int thumbIdx) {
    if (!AreAnimationsGloballyEnabled() || !g_settings.enableHoverAnimation) {
        g_animHoverActive = false;
        g_animHoverScaleActive = false;
        if (thumbIdx >= 0 && thumbIdx < (int)g_windows.size() && !IsWindowTruncated(thumbIdx)) {
            SnapHoverTo(ToRectF(g_windows[thumbIdx].rcThumbActual));
            g_animHoverAlphaCurrent = 1.0f;
            g_animHoverAlphaTarget = 1.0f;
        } else {
            g_animHoverAlphaCurrent = 0.0f;
            g_animHoverAlphaTarget = 0.0f;
        }
        g_animCloseBtnAlpha = (g_hoverIndex >= 0 && g_settings.showCloseButton && !IsWindowTruncated(g_hoverIndex)) ? 1.0f : 0.0f;
        g_animCloseBtnHoverAlpha = (g_isCloseHovered && g_animCloseBtnAlpha > 0.05f) ? 1.0f : 0.0f;
        for (int i = 0; i < (int)g_windows.size(); i++) {
            SnapCloseButtonMotion(g_windows[i], (i == g_hoverIndex && g_settings.showCloseButton && !IsWindowTruncated(i)) ? 1.0f : 0.0f);
            float s = (ThumbnailHoverIsZoom() && i == thumbIdx && !IsWindowTruncated(i)) ? (1.0f + SWS_HOVER_ZOOM_DELTA) : 1.0f;
            g_windows[i].hoverScale = s;
            g_windows[i].hoverScaleStart = s;
            g_windows[i].hoverScaleTarget = s;
            g_windows[i].hoverScaleProgress = 1.0f;
            g_windows[i].hoverScaleDuration = 0.167f;
            g_windows[i].hoverScaleClock = {};
        }
        if (ThumbnailHoverIsZoom()) {
            UpdateThumbnailAnimations();
            PaintSwitcher();
        }
        return;
    }

    UpdateMicrointeractionTargets();
    if (ThumbnailHoverIsZoom()) {
        StartAnimationTicker();
    }

    if (thumbIdx >= 0 && thumbIdx < (int)g_windows.size() && !IsWindowTruncated(thumbIdx)) {
        RECT targetRc = g_windows[thumbIdx].rcThumbActual;
        if (targetRc.left == 0 && targetRc.right == 0 && targetRc.top == 0 && targetRc.bottom == 0) {
            if (g_animHoverAlphaCurrent > 0.01f) {
                g_animHoverStart = g_animHoverCurrent;
                g_animHoverTarget = g_animHoverCurrent;
                g_animHoverAlphaStart = g_animHoverAlphaCurrent;
                g_animHoverAlphaTarget = 0.0f;
                StartMotionTrack(g_animHoverProgress);
                StartOpacityMotion(g_hoverOpacity, g_animHoverAlphaCurrent, 0.0f);
                g_animHoverDuration = 0.083f;
                g_animHoverActive = true;
                StartAnimationTicker();
            }
            return;
        }

        RectF targetRectF = ToRectF(targetRc);

        if (g_animHoverAlphaCurrent < 0.05f) {
            // Fluent entrance blossom: start 2px inset and expand smoothly into targetRc
            float inset = (float)(std::max)(1, DpiScale(2, g_dpiY));
            if ((targetRectF.right - targetRectF.left) > inset * 4.0f &&
                (targetRectF.bottom - targetRectF.top) > inset * 4.0f) {
                g_animHoverStart = {
                    targetRectF.left + inset,
                    targetRectF.top + inset,
                    targetRectF.right - inset,
                    targetRectF.bottom - inset
                };
            } else {
                g_animHoverStart = targetRectF;
            }
            g_animHoverCurrent = g_animHoverStart;
            g_animHoverTarget = targetRectF;
            g_animHoverAlphaStart = 0.0f;
            g_animHoverAlphaTarget = 1.0f;
            g_animHoverDuration = 0.167f; // Direct entrance transform
        } else {
            // Glide between thumbnails
            g_animHoverStart = g_animHoverCurrent;
            g_animHoverTarget = targetRectF;
            g_animHoverAlphaStart = g_animHoverAlphaCurrent;
            g_animHoverAlphaTarget = 1.0f;
            g_animHoverDuration = 0.167f; // Existing-element point-to-point
        }
        StartMotionTrack(g_animHoverProgress);
        StartOpacityMotion(g_hoverOpacity, g_animHoverAlphaCurrent, 1.0f);
        g_animHoverActive = true;
        StartAnimationTicker();
    } else {
        if (g_animHoverAlphaCurrent > 0.01f) {
            g_animHoverStart = g_animHoverCurrent;
            g_animHoverTarget = g_animHoverCurrent;
            g_animHoverAlphaStart = g_animHoverAlphaCurrent;
            g_animHoverAlphaTarget = 0.0f;
            StartMotionTrack(g_animHoverProgress);
            StartOpacityMotion(g_hoverOpacity, g_animHoverAlphaCurrent, 0.0f);
            g_animHoverDuration = 0.083f; // Opacity-only fade
            g_animHoverActive = true;
            StartAnimationTicker();
        }
    }
}

static void SubmitAnimatedThumbnail(HTHUMBNAIL thumbnail, HWND sourceWindow,
                                     const RECT& destination, BYTE alpha,
                                     SIZE sourceSize, RECT sourceCrop,
                                     const RECT& contentClip) {
    if (!thumbnail) return;
    auto cached = g_lastThumbState.find(thumbnail);
    ThumbCacheState previous = cached != g_lastThumbState.end()
        ? cached->second : ThumbCacheState{};
    RECT clipped = {};
    if (!alpha || !IntersectRect(&clipped, &destination, &contentClip)) {
        if (cached != g_lastThumbState.end() && !previous.visible) return;
        DWM_THUMBNAIL_PROPERTIES properties = {};
        properties.dwFlags = DWM_TNP_VISIBLE;
        properties.fVisible = FALSE;
        if (SUCCEEDED(UpdateDwmThumbnail(thumbnail, &properties))) {
            previous.visible = FALSE;
            previous.alpha = 0;
            g_lastThumbState[thumbnail] = previous;
        }
        return;
    }
    RECT fullSource = { 0, 0, sourceSize.cx, sourceSize.cy };
    if (!HasLayoutRect(sourceCrop)) sourceCrop = fullSource;
    bool partial = !EqualRect(&clipped, &destination);
    // Use the caller's already-queried source size to reset a retained crop;
    // destination/opacity invalidation must not turn it back into default mode.
    bool useSource = partial || g_thumbSourceRects.count(thumbnail) || IsIconic(sourceWindow) ||
                      !EqualRect(&sourceCrop, &fullSource);
    RECT submittedSource = sourceCrop;
    if (partial && HasLayoutRect(sourceCrop)) {
        int width = destination.right - destination.left;
        int height = destination.bottom - destination.top;
        int sourceWidth = sourceCrop.right - sourceCrop.left;
        int sourceHeight = sourceCrop.bottom - sourceCrop.top;
        submittedSource = {
            sourceCrop.left + MulDiv(clipped.left - destination.left, sourceWidth, width),
            sourceCrop.top + MulDiv(clipped.top - destination.top, sourceHeight, height),
            sourceCrop.left + MulDiv(clipped.right - destination.left, sourceWidth, width),
            sourceCrop.top + MulDiv(clipped.bottom - destination.top, sourceHeight, height)
        };
    }
    if (cached != g_lastThumbState.end() && previous.visible &&
        EqualRect(&previous.dst, &clipped) && previous.alpha == alpha &&
        previous.hasSource == useSource &&
        (!useSource || EqualRect(&previous.source, &submittedSource))) return;
    DWM_THUMBNAIL_PROPERTIES properties = {};
    properties.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_OPACITY |
                         DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;
    properties.rcDestination = clipped;
    properties.opacity = alpha;
    properties.fVisible = TRUE;
    properties.fSourceClientAreaOnly = FALSE;
    if (useSource && HasLayoutRect(submittedSource)) {
        properties.dwFlags |= DWM_TNP_RECTSOURCE;
        properties.rcSource = submittedSource;
    } else {
        useSource = false;
    }
    if (SUCCEEDED(UpdateDwmThumbnail(thumbnail, &properties))) {
        g_lastThumbState[thumbnail] = { clipped, alpha, TRUE, submittedSource, useSource };
    }
}

static void UpdateThumbnailAnimations() {
    if (!g_settings.showThumbnails || !g_hSwitcher) return;
    if (DockLayoutActive()) {
        UpdateDockThumbnailDwm();
        return;
    }
    int offX = (int)roundf(g_scrollTransition.offsetCurrentX);
    int offY = (int)roundf(g_scrollTransition.offsetCurrentY);
    BYTE thumbAlpha = CurrentPresentationAlphaByte();

    RECT rcClient; GetClientRect(g_hSwitcher, &rcClient);
    int masterPadX = DpiScale(g_settings.switcherPadding, g_dpiX);
    int masterPadY = DpiScale(g_settings.switcherPadding, g_dpiY);
    RECT rcContentClip = { masterPadX, masterPadY, rcClient.right - masterPadX, rcClient.bottom - masterPadY };

    // Uses global g_lastThumbState tracked by SafeDwmUnregisterThumbnail

    auto updateThumb = [&](HTHUMBNAIL hThumb, HWND sourceWindow,
                           const RECT& dst, BYTE alpha, SIZE sourceSize,
                           const RECT& sourceCrop) {
        SubmitAnimatedThumbnail(hThumb, sourceWindow, dst, alpha,
                                sourceSize, sourceCrop, rcContentClip);
    };

    // 1. Incoming items
    for (auto& w : g_windows) {
        if (w.rcThumbActual.left == 0 && w.rcThumbActual.right == 0 &&
            w.rcThumbActual.top == 0 && w.rcThumbActual.bottom == 0) {
            for (const auto& kv : w.hThumbs) {
                if (kv.second) {
                    auto it = g_lastThumbState.find(kv.second);
                    if (it == g_lastThumbState.end() || it->second.visible) {
                        updateThumb(kv.second, w.hWnd, {}, 0, w.sourceSize, w.rcSourceCrop);
                    }
                }
            }
            continue;
        }

        RECT dst = w.rcThumbActual;
        if (ThumbnailHoverIsZoom() && w.hoverScale > 1.0001f) {
            int cx = (dst.left + dst.right) / 2;
            int cy = (dst.top + dst.bottom) / 2;
            int hw = (int)roundf(((float)(dst.right - dst.left) / 2.0f) * w.hoverScale);
            int hh = (int)roundf(((float)(dst.bottom - dst.top) / 2.0f) * w.hoverScale);
            dst = { cx - hw, cy - hh, cx + hw, cy + hh };
        }
        if (offX != 0 || offY != 0) {
            OffsetRect(&dst, offX, offY);
        }

        if (!w.hThumbs.count(g_hSwitcher)) {
            HTHUMBNAIL hT = NULL;
            if (SUCCEEDED(DwmRegisterThumbnail(g_hSwitcher, w.hWnd, &hT))) {
                w.hThumbs[g_hSwitcher] = hT;
                if (w.sourceSize.cx <= 0 || w.sourceSize.cy <= 0) {
                    SIZE src = {0}; DwmQueryThumbnailSourceSize(hT, &src);
                    w.sourceSize = src;
                }
            }
        }

        BYTE itemAlpha = thumbAlpha;
        if (g_layoutTransition.active && w.isNewEntry) {
            itemAlpha = (BYTE)roundf(thumbAlpha * w.enterAlpha);
        }
        for (const auto& kv : w.hThumbs) {
            updateThumb(kv.second, w.hWnd, dst, itemAlpha, w.sourceSize, w.rcSourceCrop);
        }
    }

    // 2. Outgoing items
    if (g_scrollTransition.active && !g_scrollTransition.outgoingItems.empty()) {
        int outOffX = offX - g_scrollTransition.travelDistanceX;
        int outOffY = offY - g_scrollTransition.travelDistanceY;

        for (const auto& snap : g_scrollTransition.outgoingItems) {
            int curIdx = FindWindowIndexByHwnd(snap.hWnd);
            if (curIdx != -1 && !IsWindowTruncated(curIdx)) continue; // Already updated above in incoming items

            RECT dst = snap.rcThumbActual;
            OffsetRect(&dst, outOffX, outOffY);

            // Collision guard: if an incoming visible thumbnail occupies this exact destination, hide outgoing
            bool collides = false;
            for (const auto& w : g_windows) {
                if (w.rcThumbActual.left == 0 && w.rcThumbActual.right == 0 &&
                    w.rcThumbActual.top == 0 && w.rcThumbActual.bottom == 0) continue;
                RECT inDst = w.rcThumbActual;
                if (offX != 0 || offY != 0) OffsetRect(&inDst, offX, offY);
                if (EqualRect(&dst, &inDst)) {
                    collides = true;
                    break;
                }
            }
            if (collides) {
                for (const auto& kv : snap.hThumbs) {
                    if (kv.second) {
                        DWM_THUMBNAIL_PROPERTIES p = {};
                        p.dwFlags = DWM_TNP_VISIBLE;
                        p.fVisible = FALSE;
                        UpdateDwmThumbnail(kv.second, &p);
                    }
                }
                continue;
            }

            for (const auto& kv : snap.hThumbs) {
                updateThumb(kv.second, snap.hWnd, dst, (BYTE)roundf(thumbAlpha * snap.alpha), snap.sourceSize, snap.rcSourceCrop);
            }
        }
    }

    // 3. Departing items in layout transition
    if (g_layoutTransition.active && !g_layoutTransition.departingItems.empty()) {
        for (const auto& item : g_layoutTransition.departingItems) {
            BYTE departAlpha = (BYTE)roundf(thumbAlpha * item.alpha);
            RECT dst = item.rcThumbCurrent;
            for (const auto& kv : item.hThumbs) {
                updateThumb(kv.second, item.hWnd, dst, departAlpha, item.sourceSize, item.rcSourceCrop);
            }
        }
    }
}

static void OnAnimationTick() {
    if (!g_animActive) return;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    float dt = (float)(s_animTargetIntervalMs / 1000.0);
    if (g_animPerfFreq.QuadPart > 0 && g_animLastTickTime.QuadPart > 0) {
        float measuredDt = (float)(now.QuadPart - g_animLastTickTime.QuadPart) / (float)g_animPerfFreq.QuadPart;
        if (measuredDt >= 0.0f) {
            dt = measuredDt;
        }
    }
    g_animLastTickTime = now;

    // Finite tracks use their own absolute trigger times. After a stall they
    // settle on the next available frame, without discarding elapsed time.
    if (dt < 0.0f) dt = 0.0f;
    struct TickScope {
        TickScope() { g_animTickInProgress = g_animFrameSampleActive = true; }
        ~TickScope() { g_animTickInProgress = g_animFrameSampleActive = false; }
    } tickScope;
    // The switcher is already hidden during the final backdrop-only phase.
    // Keep the exit guard and ticker alive without painting it back on screen.
    if (g_backdropExitFadeActive) {
        if (!BackdropFadeTick(dt)) CompleteExitAnimation();
        return;
    }
    UpdateMicrointeractionTargets();

    bool anyActive = false;
    bool hadThumbMotion = (g_animEntranceActive || g_scrollTransition.active || g_animExitActive || g_layoutTransition.active || g_dockPreviewSlide.active || g_animHoverScaleActive);
    bool hadItemMotion = !g_layoutTransition.departingItems.empty() ||
        std::any_of(g_windows.begin(), g_windows.end(),
                    [](const WindowEntry& entry) { return entry.isNewEntry; });

    // 1. Shared entrance/exit timeline
    AdvancePresentationAnimation(dt);
    if (g_animEntranceActive || g_presentationOpacity.progress < 1.0f) anyActive = true;

    // 2. Scroll / Page Slide animation
    if (g_scrollTransition.active) {
        if (StepMotionTrack(g_scrollTransition.progress, g_scrollTransition.duration, dt)) {
            g_scrollTransition.progress = 1.0f;
            RefreshScrollIconCanvas();
            g_scrollTransition.active = false;
            g_scrollTransition.offsetCurrentX = 0.0f;
            g_scrollTransition.offsetCurrentY = 0.0f;
            if (!s_scrollIconCanvasDirty && s_cachedStaticDC && s_cachedScrollToDC && s_cachedStaticW == s_cachedScrollToW && s_cachedStaticH == s_cachedScrollToH) {
                BitBlt(s_cachedStaticDC, 0, 0, s_cachedStaticW, s_cachedStaticH, s_cachedScrollToDC, 0, 0, SRCCOPY);
                g_staticContentDirty = false;
            } else {
                g_staticContentDirty = true;
            }
            for (auto& item : g_scrollTransition.outgoingItems) {
                int curIdx = FindWindowIndexByHwnd(item.hWnd);
                if (curIdx == -1) {
                    for (const auto& kv : item.hThumbs) {
                        if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
                    }
                } else if (IsWindowTruncated(curIdx)) {
                    for (const auto& kv : item.hThumbs) {
                        if (kv.second) {
                            DWM_THUMBNAIL_PROPERTIES p = {};
                            p.dwFlags = DWM_TNP_VISIBLE;
                            p.fVisible = FALSE;
                            UpdateDwmThumbnail(kv.second, &p);
                        }
                    }
                }
            }
            g_scrollTransition.outgoingItems.clear();
            g_scrollTransition.preservingThumbnails = false;
            if (g_hoverThumbIndex >= 0 && g_hoverThumbIndex < (int)g_windows.size() && !IsWindowTruncated(g_hoverThumbIndex)) {
                SnapHoverTo(ToRectF(g_windows[g_hoverThumbIndex].rcThumbActual));
            }
            UpdateHoverFromCursor(true);
            UpdateChevronAnimationTargets(false);
        } else {
            anyActive = true;
            float e = g_easeSlide.Solve(g_scrollTransition.progress);
            g_scrollTransition.offsetCurrentX = g_scrollTransition.offsetStartX * (1.0f - e);
            g_scrollTransition.offsetCurrentY = g_scrollTransition.offsetStartY * (1.0f - e);
        }
    }

    // 3. Hover animation
    if (g_animHoverActive) {
        bool opacityActive = StepOpacityMotion(g_hoverOpacity, g_animHoverAlphaCurrent, dt);
        if (StepMotionTrack(g_animHoverProgress, g_animHoverDuration, dt) && !opacityActive) {
            g_animHoverProgress = 1.0f;
            g_animHoverActive = false;
            g_animHoverCurrent = g_animHoverTarget;
            g_animHoverAlphaCurrent = g_animHoverAlphaTarget;
        } else {
            anyActive = true;
            float e = (g_animHoverAlphaStart < 0.05f)
                ? g_easeHoverEnter.Solve(g_animHoverProgress)
                : g_easeHover.Solve(g_animHoverProgress);
            g_animHoverCurrent = LerpRect(g_animHoverStart, g_animHoverTarget, e);
        }
    }

    // Hover zoom scale animation (per-entry concurrent WinUI 3 curves with C0 state continuity)
    bool hoverScaleAnimActive = false;
    static bool s_hadHoverScaleMotion = false;
    if (ThumbnailHoverIsZoom()) {
        for (int i = 0; i < (int)g_windows.size(); i++) {
            auto& w = g_windows[i];
            if (w.hoverScaleProgress < 1.0f) {
                // Even a first frame arriving after this track's deadline must
                // submit its terminal thumbnail transform once.
                s_hadHoverScaleMotion = true;
                if (StepMotionTrack(w.hoverScaleProgress, w.hoverScaleClock, w.hoverScaleDuration, dt)) {
                    w.hoverScaleProgress = 1.0f;
                    w.hoverScale = w.hoverScaleTarget;
                } else {
                    hoverScaleAnimActive = true;
                    float e = g_easeHover.Solve(w.hoverScaleProgress);
                    w.hoverScale = w.hoverScaleStart + (w.hoverScaleTarget - w.hoverScaleStart) * e;
                }
            } else {
                w.hoverScale = w.hoverScaleTarget;
            }
        }
    } else {
        for (auto& w : g_windows) {
            w.hoverScale = 1.0f;
            w.hoverScaleStart = 1.0f;
            w.hoverScaleTarget = 1.0f;
            w.hoverScaleProgress = 1.0f;
            w.hoverScaleDuration = 0.167f;
        }
    }
    if (hoverScaleAnimActive) {
        anyActive = true;
        s_hadHoverScaleMotion = true;
    }
    g_animHoverScaleActive = hoverScaleAnimActive || s_hadHoverScaleMotion;
    if (!hoverScaleAnimActive && s_hadHoverScaleMotion) {
        s_hadHoverScaleMotion = false; // final tick IPC flush complete
    }

    // 4. Exit retraces the entrance timeline at the same speed. Remaining
    // duration is 167ms * p. A full reverse is the documented gentle-exit curve
    // (1,0,1,1), permitted for native material motion-only presentation.
    if (g_animExitActive) {
        if (g_animExitProgress <= 0.0f &&
            (!ThemeIs(L"none") || g_presentationOpacity.progress >= 1.0f)) {
            HideSwitcherPresentationWindows();
            if (FadeOutBackdropBlur()) return;
            CompleteExitAnimation();
            return;
        } else {
            anyActive = true;
        }
    }

    // 5. Chevron reveal / fade animation with cubic-bezier easing & spatial glide
    if (g_animChevronProgressPrev < 1.0f) {
        if (StepMotionTrack(g_animChevronProgressPrev, g_animChevronDuration, dt)) {
            g_animChevronProgressPrev = 1.0f;
            g_animChevronTransformPrev = g_animChevronAlphaTargetPrev;
        } else {
            anyActive = true;
            float e = g_animChevronAlphaTargetPrev > g_animChevronTransformStartPrev
                ? g_easeEntrance.Solve(g_animChevronProgressPrev)
                : g_easeExit.Solve(g_animChevronProgressPrev);
            g_animChevronTransformPrev = g_animChevronTransformStartPrev + (g_animChevronAlphaTargetPrev - g_animChevronTransformStartPrev) * e;
        }
    } else {
        g_animChevronTransformPrev = g_animChevronAlphaTargetPrev;
    }

    if (g_animChevronProgressNext < 1.0f) {
        if (StepMotionTrack(g_animChevronProgressNext, g_animChevronDuration, dt)) {
            g_animChevronProgressNext = 1.0f;
            g_animChevronTransformNext = g_animChevronAlphaTargetNext;
        } else {
            anyActive = true;
            float e = g_animChevronAlphaTargetNext > g_animChevronTransformStartNext
                ? g_easeEntrance.Solve(g_animChevronProgressNext)
                : g_easeExit.Solve(g_animChevronProgressNext);
            g_animChevronTransformNext = g_animChevronTransformStartNext + (g_animChevronAlphaTargetNext - g_animChevronTransformStartNext) * e;
        }
    } else {
        g_animChevronTransformNext = g_animChevronAlphaTargetNext;
    }
    if (StepOpacityMotion(g_chevronRevealPrev, g_animChevronAlphaPrev, dt)) anyActive = true;
    if (StepOpacityMotion(g_chevronRevealNext, g_animChevronAlphaNext, dt)) anyActive = true;

    // Independent 83 ms linear hover-opacity tracks.
    if (StepOpacityMotion(g_chevronHoverPrev, g_animChevronHoverAlphaPrev, dt)) anyActive = true;
    if (StepOpacityMotion(g_chevronHoverNext, g_animChevronHoverAlphaNext, dt)) anyActive = true;

    // 6. Dynamic Layout Transition (Window resize + Card rearrange + Add/Remove animation)
    if (g_layoutTransition.active) {
        if (StepMotionTrack(g_layoutTransition.progress, g_layoutTransition.duration, dt)) {
            g_layoutTransition.progress = 1.0f;
            g_layoutTransition.active = false;

            for (auto& w : g_windows) {
                w.rcCellLayoutCurrent = w.rcCellTarget;
                w.rcThumbLayoutCurrent = w.rcThumbTarget;
                w.rcThumbSlot = w.rcThumbSlotTarget;
                ApplyEntryPresentationGeometry(w);
            }

            if (g_hoverThumbIndex >= 0 && g_hoverThumbIndex < (int)g_windows.size() && !IsWindowTruncated(g_hoverThumbIndex)) {
                g_animHoverCurrent = ToRectF(g_windows[g_hoverThumbIndex].rcThumbActual);
                g_animHoverTarget = g_animHoverCurrent;
                g_animHoverStart = g_animHoverCurrent;
                g_animHoverActive = false;
            }

            int finalCx = (int)roundf(g_layoutTransition.rcWndTarget.left);
            int finalCy = (int)roundf(g_layoutTransition.rcWndTarget.top);
            int finalW = (int)roundf(g_layoutTransition.rcWndTarget.right - g_layoutTransition.rcWndTarget.left);
            int finalH = (int)roundf(g_layoutTransition.rcWndTarget.bottom - g_layoutTransition.rcWndTarget.top);
            SetSwitcherLayoutBounds(finalCx, finalCy, finalW, finalH);
            g_switcherBaseX = finalCx;
            g_switcherBaseY = finalCy;
            g_switcherBaseInitialized = true;
            ApplySwitcherRegion();

            InvalidateStaticCache();
            if (DockLayoutActive()) {
                g_rcDockIconStrip = g_layoutTransition.rcDockStripTarget;
                g_rcCentralPreview = g_layoutTransition.rcDockPreviewTarget;
                g_rcDockTitleBar = g_layoutTransition.rcDockTitleTarget;
                g_rcCentralPreviewSlot = g_layoutTransition.rcDockSlotTarget;
                if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
                    g_windows[g_selectedIndex].rcThumbActual = g_rcCentralPreview;
                    g_windows[g_selectedIndex].rcThumbSlot = g_rcCentralPreviewSlot;
                    g_windows[g_selectedIndex].rcThumbLayoutCurrent = g_rcCentralPreview;
                    ApplyEntryPresentationGeometry(g_windows[g_selectedIndex]);
                }
            }
            UpdateHoverFromCursor(true);
            UpdateChevronAnimationTargets(false);
        } else {
            anyActive = true;
            float t = g_easeLayout.Solve(g_layoutTransition.progress);

            RectF curWnd = LerpRect(g_layoutTransition.rcWndStart, g_layoutTransition.rcWndTarget, t);
            int curCx = (int)roundf(curWnd.left);
            int curCy = (int)roundf(curWnd.top);
            int curW = (int)roundf(curWnd.right - curWnd.left);
            int curH = (int)roundf(curWnd.bottom - curWnd.top);
            SetSwitcherLayoutBounds(curCx, curCy, curW, curH);
            g_switcherBaseX = curCx;
            g_switcherBaseY = curCy;
            g_switcherBaseInitialized = true;

            if (DockLayoutActive()) {
                g_rcDockIconStrip = InterpolateLayoutRect(g_layoutTransition.rcDockStripStart, g_layoutTransition.rcDockStripTarget, t);
                g_rcCentralPreview = InterpolateLayoutRect(g_layoutTransition.rcDockPreviewStart, g_layoutTransition.rcDockPreviewTarget, t);
                g_rcDockTitleBar = InterpolateLayoutRect(g_layoutTransition.rcDockTitleStart, g_layoutTransition.rcDockTitleTarget, t);
                g_rcCentralPreviewSlot = InterpolateLayoutRect(g_layoutTransition.rcDockSlotStart, g_layoutTransition.rcDockSlotTarget, t);
            }

            for (auto& w : g_windows) {
                ApplyEntryLayoutGeometry(w, t);
            }

            if (g_hoverThumbIndex >= 0 && g_hoverThumbIndex < (int)g_windows.size() && !IsWindowTruncated(g_hoverThumbIndex)) {
                g_animHoverCurrent = ToRectF(g_windows[g_hoverThumbIndex].rcThumbActual);
                g_animHoverTarget = g_animHoverCurrent;
                g_animHoverStart = g_animHoverCurrent;
                g_animHoverActive = false;
            }

            g_staticContentDirty = true;
        }
    }

    // 6a. Owned child opacity (83 ms linear) and scale (167 ms), independent
    // of the 250 ms repositioning and of every other child's original clock.
    if (AdvanceItemTransitions(dt)) anyActive = true;

    // Selection owns its 167 ms track independently of the 250 ms layout and
    // 167/250 ms viewport tracks. Rebase it to the current selected cell after
    // those owners have sampled their geometry, then sample it once this frame.
    SyncSelectionAnimationToLayout();
    if (g_animSelectionActive) {
        if (StepMotionTrack(g_animSelectionProgress, g_animSelectionDuration, dt)) {
            g_animSelectionProgress = 1.0f;
            g_animSelectionActive = false;
            g_animSelectionCurrent = g_animSelectionTarget;
        } else {
            anyActive = true;
            float e = g_easeSelection.Solve(g_animSelectionProgress);
            g_animSelectionCurrent = LerpRect(g_animSelectionStart, g_animSelectionTarget, e);
        }
    }

    // 6b. Dock Layout Central Preview Directional Slide Transition (Window close)
    if (g_dockPreviewSlide.active) {
        if (StepMotionTrack(g_dockPreviewSlide.progress, g_dockPreviewSlide.duration, dt)) {
            g_dockPreviewSlide.progress = 1.0f;
            g_dockPreviewSlide.active = false;
            g_dockPreviewSlide.currentOffset = 0.0f;
            g_dockPreviewSlide.currentAlpha = 1.0f;
        } else {
            anyActive = true;
            float t = g_easeSlide.Solve(g_dockPreviewSlide.progress);
            g_dockPreviewSlide.currentOffset = (1.0f - t) * g_dockPreviewSlide.travelDistance;
            g_dockPreviewSlide.currentAlpha = (std::min)(1.0f, g_dockPreviewSlide.progress * g_dockPreviewSlide.duration / 0.083f);
        }
        if (DockLayoutActive()) {
            UpdateDockThumbnailDwm();
            g_staticContentDirty = true;
        }
    }

    // 7. Close buttons: independent 83 ms linear fades and 167 ms entrance /
    // gentle-exit scale transforms, including each invisible terminal sample.
    bool closeBtnAnimActive = false;
    for (int i = 0; i < (int)g_windows.size(); i++) {
        if (StepCloseButtonMotion(g_windows[i], dt)) closeBtnAnimActive = true;
    }
    if (closeBtnAnimActive) anyActive = true;

    g_animCloseBtnAlpha = (g_hoverIndex >= 0 && g_hoverIndex < (int)g_windows.size()) ? g_windows[g_hoverIndex].closeBtnAlpha : 0.0f;

    if (StepOpacityMotion(g_closeBtnHover, g_animCloseBtnHoverAlpha, dt)) anyActive = true;

    if (g_settings.showThumbnails && (hadThumbMotion || hadItemMotion || g_animHoverScaleActive || g_animEntranceActive || g_scrollTransition.active || g_animExitActive || g_layoutTransition.active)) {
        UpdateThumbnailAnimations();
    }

    PaintSwitcher();

    if (!anyActive && !(s_iconRepaintPending && g_isVisible)) {
        StopAnimationTicker();
    }
}

// Discrete shrink steps for the "Shrink tasks to fit" option, keyed off the
// number of visible tasks. Coarse but predictable; tune thresholds freely.
static int ComputeAutoFitScalePct(int taskCount) {
    if (!g_settings.autoFitTasks) return 100;
    if (taskCount <= 8)  return 100;
    if (taskCount <= 14) return 80;
    if (taskCount <= 22) return 65;
    if (taskCount <= 32) return 50;
    return 40;
}
// Apply the current auto-fit scale to a pixel value (no-op when not shrinking).
static int ScaleAutoFit(int px) {
    return g_autoFitScalePct == 100 ? px : px * g_autoFitScalePct / 100;
}
static int GetHeaderIconSizePx() {
    int px = MulDiv(GetHeaderIconSizeBase(), g_dpiX, 96);
    if (g_autoFitScalePct != 100) {
        px = ScaleAutoFit(px);
        int floorPx = MulDiv(SWS_ICON_SIZE, g_dpiX, 96);
        if (px < floorPx) px = floorPx;
    }
    return px;
}
static HFONT CreateScaledFont(int dpiY);

static int GetHeaderTitleHeightPx() {
    // Keep the original band when it fits. Measure the configured font at the
    // layout DPI: g_hFont can still belong to the previous monitor until paint.
    static struct {
        int dpi = 0;
        int size = 0;
        WCHAR family[64] = {};
        WCHAR style[32] = {};
        HFONT renderedFont = NULL;
        int lineHeight = 0;
    } metrics;
    if (metrics.dpi != g_dpiY || metrics.size != g_settings.fontSize ||
        wcscmp(metrics.family, g_settings.fontFamily) != 0 ||
        wcscmp(metrics.style, g_settings.fontStyle) != 0 ||
        metrics.renderedFont != g_hFont) {
        metrics.lineHeight = 0;
        HFONT font = CreateScaledFont(g_dpiY);
        HDC dc = GetDC(NULL);
        if (font && dc) {
            HGDIOBJ previous = SelectObject(dc, font);
            TEXTMETRICW tm = {};
            if (GetTextMetricsW(dc, &tm)) metrics.lineHeight = tm.tmHeight;
            SelectObject(dc, previous);
        }
        if (dc) ReleaseDC(NULL, dc);
        if (font) DeleteObject(font);
        metrics.dpi = g_dpiY;
        metrics.size = g_settings.fontSize;
        wcscpy_s(metrics.family, g_settings.fontFamily);
        wcscpy_s(metrics.style, g_settings.fontStyle);
        metrics.renderedFont = g_hFont;
    }
    return std::max(MulDiv(18, g_dpiY, 96), metrics.lineHeight);
}
static int GetHeaderRowHeightPx() {
    if (DockLayoutActive()) {
        int h = MulDiv(20, g_dpiY, 96);
        return g_settings.showTitle ? std::max(h, GetHeaderTitleHeightPx()) : h;
    }
    if (!g_settings.showTitle && !g_settings.showIcon) {
        return 0;
    }

    if (!HeaderIsVertical()) {
        int h = MulDiv(SWS_ROW_TITLE_HEIGHT, g_dpiY, 96);
        if (g_settings.showIcon && GetHeaderIconSizePx() > h) h = GetHeaderIconSizePx();
        if (g_settings.showTitle) h = std::max(h, GetHeaderTitleHeightPx());
        return h;
    }

    int gap = MulDiv(4, g_dpiY, 96);
    int h = 0;
    if (g_settings.showIcon) h += GetHeaderIconSizePx();
    if (g_settings.showTitle) h += (h > 0 ? gap : 0) + GetHeaderTitleHeightPx();
    return h;
}
static INT GetCornerPref() {
    if (wcscmp(g_settings.cornerPreference, L"none") == 0) return 1; // DWMWCP_DONOTROUND
    if (wcscmp(g_settings.cornerPreference, L"roundSmall") == 0) return 3; // DWMWCP_ROUNDSMALL
    if (wcscmp(g_settings.cornerPreference, L"round") == 0) return 2; // DWMWCP_ROUND
    if (wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        if (g_settings.customCornerRadius <= 0) return 1; // DONOTROUND (0px sharp rectangle)
        if (g_settings.customCornerRadius <= 5) return 3; // ROUNDSMALL (~4px)
        return 2; // ROUND (~8px)
    }
    // "default" or legacy "auto":
    // If DWM system radius is 0, explicitly tell DWM not to round
    if (g_systemDwmRadius <= 0) return 1; // DWMWCP_DONOTROUND
    return 0; // DWMWCP_DEFAULT (Let Windows / DWM mod decide)
}

static void GetResolvedCornerRadiiDIP(int* outStdDIP, int* outSmallDIP) {
    int stdDIP = 0;
    int smallDIP = 0;

    if (wcscmp(g_settings.cornerPreference, L"none") == 0) {
        stdDIP = 0;
        smallDIP = 0;
    } else if (wcscmp(g_settings.cornerPreference, L"roundSmall") == 0) {
        stdDIP = 4;
        smallDIP = 4;
    } else if (wcscmp(g_settings.cornerPreference, L"round") == 0) {
        stdDIP = 8;
        smallDIP = 4;
    } else if (wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        stdDIP = g_settings.customCornerRadius;
        smallDIP = (stdDIP <= 0) ? 0 : std::max(1, (stdDIP + 1) / 2);
    } else { // "default" or legacy "auto"
        stdDIP = g_systemDwmRadius;
        smallDIP = g_systemDwmSmallRadius;
    }

    // Invariant: if standard radius is 0 (square mode), all child elements are strictly 0
    if (stdDIP <= 0) {
        stdDIP = 0;
        smallDIP = 0;
    }

    if (outStdDIP) *outStdDIP = stdDIP;
    if (outSmallDIP) *outSmallDIP = smallDIP;
}

static int GetWindowCornerRadiusPx() {
    int stdDIP = 0, smallDIP = 0;
    GetResolvedCornerRadiiDIP(&stdDIP, &smallDIP);
    if (stdDIP <= 0) return 0;

    return MulDiv(stdDIP, g_dpiX, 96);
}

static bool UseTaskRoundedCorners() {
    return g_settings.taskRoundedCorners;
}

static int GetTaskUiCornerRadiusPx() {
    if (!UseTaskRoundedCorners()) {
        return 0;
    }
    int stdDIP = 0, smallDIP = 0;
    GetResolvedCornerRadiiDIP(&stdDIP, &smallDIP);
    if (stdDIP <= 0) return 0;
    return MulDiv(stdDIP, g_dpiX, 96);
}

// Thumbnail corner rounding is controlled independently from the task border /
// close button rounding, but shares the same radius from Corner Preference.
static int GetThumbnailCornerRadiusPx() {
    if (!g_settings.roundThumbnailCorners) {
        return 0;
    }
    int stdDIP = 0, smallDIP = 0;
    GetResolvedCornerRadiiDIP(&stdDIP, &smallDIP);
    if (stdDIP <= 0 || smallDIP <= 0) return 0;
    int effectiveDIP = std::min(smallDIP, stdDIP);
    return MulDiv(effectiveDIP, g_dpiX, 96);
}

static int GetGroupIndicatorCornerRadiusPx(int maxRadius) {
    if (!g_settings.roundGroupIndicator) {
        return 0;
    }
    int stdDIP = 0, smallDIP = 0;
    GetResolvedCornerRadiiDIP(&stdDIP, &smallDIP);
    if (stdDIP <= 0 || smallDIP <= 0) return 0;
    int effectiveDIP = std::min(smallDIP, stdDIP);
    int r = MulDiv(effectiveDIP, g_dpiX, 96);
    return (r > maxRadius) ? maxRadius : r;
}

static int GetBadgeIconBackgroundCornerRadiusPx(int maxRadius) {
    if (!g_settings.roundBadgeIconBackground) {
        return 0;
    }
    int stdDIP = 0, smallDIP = 0;
    GetResolvedCornerRadiiDIP(&stdDIP, &smallDIP);
    if (stdDIP <= 0 || smallDIP <= 0) return 0;
    int effectiveDIP = std::min(smallDIP, stdDIP);
    int r = MulDiv(effectiveDIP, g_dpiX, 96);
    return (r > maxRadius) ? maxRadius : r;
}

static void GetSwitcherPosition(const RECT& workArea, int* outX, int* outY,
                                 int width = g_winW, int height = g_winH) {
    int w = workArea.right - workArea.left;
    int h = workArea.bottom - workArea.top;
    // Position margins are DIP settings, so mirrored windows must use the
    // target monitor's DPI rather than the main switcher's cached DPI.
    UINT positionDpiX = g_dpiX;
    UINT positionDpiY = g_dpiY;
    HMONITOR hMon = MonitorFromRect(&workArea, MONITOR_DEFAULTTONEAREST);
    UINT monitorDpiX = 96, monitorDpiY = 96;
    if (QueryMonitorDpi(hMon, &monitorDpiX, &monitorDpiY)) {
        positionDpiX = monitorDpiX;
        positionDpiY = monitorDpiY;
    }
    int marginX = MulDiv(g_settings.switcherPositionMargin, positionDpiX, 96);
    int marginY = MulDiv(g_settings.switcherPositionMargin, positionDpiY, 96);
    if (wcscmp(g_settings.switcherPosition, L"topLeft") == 0) {
        *outX = workArea.left + marginX; *outY = workArea.top + marginY;
    } else if (wcscmp(g_settings.switcherPosition, L"topCenter") == 0) {
        *outX = workArea.left + (w - width) / 2; *outY = workArea.top + marginY;
    } else if (wcscmp(g_settings.switcherPosition, L"topRight") == 0) {
        *outX = workArea.right - width - marginX; *outY = workArea.top + marginY;
    } else if (wcscmp(g_settings.switcherPosition, L"centerLeft") == 0) {
        *outX = workArea.left + marginX; *outY = workArea.top + (h - height) / 2;
    } else if (wcscmp(g_settings.switcherPosition, L"centerRight") == 0) {
        *outX = workArea.right - width - marginX; *outY = workArea.top + (h - height) / 2;
    } else if (wcscmp(g_settings.switcherPosition, L"bottomLeft") == 0) {
        *outX = workArea.left + marginX; *outY = workArea.bottom - height - marginY;
    } else if (wcscmp(g_settings.switcherPosition, L"bottomCenter") == 0) {
        *outX = workArea.left + (w - width) / 2; *outY = workArea.bottom - height - marginY;
    } else if (wcscmp(g_settings.switcherPosition, L"bottomRight") == 0) {
        *outX = workArea.right - width - marginX; *outY = workArea.bottom - height - marginY;
    } else { // center
        *outX = workArea.left + (w - width) / 2; *outY = workArea.top + (h - height) / 2;
    }
}

static int GetCloseButtonCornerRadiusPx() {
    if (!UseTaskRoundedCorners()) return 0;
    int stdDIP = 0, smallDIP = 0;
    GetResolvedCornerRadiiDIP(&stdDIP, &smallDIP);
    if (stdDIP <= 0 || smallDIP <= 0) return 0;
    int effectiveDIP = std::min(smallDIP, stdDIP);
    return MulDiv(effectiveDIP, g_dpiX, 96);
}
static bool ShouldUseDarkMode() {
    if (wcscmp(g_settings.colorScheme, L"light") == 0) return false;
    if (wcscmp(g_settings.colorScheme, L"dark") == 0) return true;
    DWORD val = 0, sz = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, NULL, &val, &sz) == ERROR_SUCCESS) return val == 0;
    return true;
}
static COLORREF GetAccentColor() {
    DWORD col = 0, sz = sizeof(col);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
        L"AccentColor", RRF_RT_REG_DWORD, NULL, &col, &sz) == ERROR_SUCCESS) return col & 0x00FFFFFF;
    return RGB(0, 120, 215);
}

static bool ParseHexColor(const WCHAR* value, COLORREF* outColor) {
    if (!value) {
        return false;
    }

    const WCHAR* p = value;
    size_t len = wcslen(p);
    if (len == 7 && p[0] == L'#') {
        p++;
        len = 6;
    }

    if (len != 6) {
        return false;
    }

    unsigned int rgb = 0;
    for (size_t i = 0; i < len; ++i) {
        if (!((p[i] >= L'0' && p[i] <= L'9') || (p[i] >= L'a' && p[i] <= L'f') ||
              (p[i] >= L'A' && p[i] <= L'F'))) return false;
    }
    if (swscanf_s(p, L"%06x", &rgb) != 1) {
        return false;
    }

    if (outColor) {
        *outColor = RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
    }
    return true;
}

static bool ResolveAPIs() {
    HMODULE h = GetModuleHandleW(L"user32.dll");
    if (!h) return false;
    g_IsShellManagedWindow = (IsShellWindow_t)GetProcAddress(h, (LPCSTR)2574);
    g_IsShellFrameWindow = (IsShellWindow_t)GetProcAddress(h, (LPCSTR)2573);
    g_GhostWindowFromHungWindow = (GhostWindowFromHungWindow_t)GetProcAddress(h, "GhostWindowFromHungWindow");
    g_HungWindowFromGhostWindow = (GhostWindowFromHungWindow_t)GetProcAddress(h, "HungWindowFromGhostWindow");
    g_SetWindowCompositionAttribute = (SetWindowCompositionAttribute_t)GetProcAddress(h, "SetWindowCompositionAttribute");

    return true;
}

// Explorer restart prompt

constexpr WCHAR kRestartTitle[] = L"Simple Window Switcher - Windhawk";
constexpr WCHAR kRestartText[] = L"Explorer needs to be restarted for changes to take effect. Restart now?";

static HRESULT CALLBACK RestartPromptDialogCallback(HWND hwnd, UINT msg, WPARAM, LPARAM, LONG_PTR) {
    if (msg == TDN_CREATED) {
        g_restartExplorerPromptWindow = hwnd;
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    } else if (msg == TDN_DESTROYED) {
        g_restartExplorerPromptWindow = nullptr;
    }
    return S_OK;
}

static DWORD WINAPI RestartPromptThreadProc(LPVOID) {
    TASKDIALOGCONFIG tdc = {};
    tdc.cbSize = sizeof(tdc);
    tdc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
    tdc.dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
    tdc.pszWindowTitle = kRestartTitle;
    tdc.pszMainIcon = TD_INFORMATION_ICON;
    tdc.pszContent = kRestartText;
    tdc.pfCallback = RestartPromptDialogCallback;

    int button;
    if (SUCCEEDED(TaskDialogIndirect(&tdc, &button, nullptr, nullptr)) && button == IDYES) {
        WCHAR cmd[] = L"cmd.exe /c \"timeout /t 1 /nobreak >nul & taskkill /F /IM explorer.exe & start explorer.exe\"";
        STARTUPINFO si = { .cb = sizeof(si) };
        PROCESS_INFORMATION pi = {};
        if (CreateProcess(nullptr, cmd, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }
    return 0;
}

static void PromptForExplorerRestart() {
    if (g_restartExplorerPromptThread) {
        if (WaitForSingleObject(g_restartExplorerPromptThread, 0) != WAIT_OBJECT_0) return;
        CloseHandle(g_restartExplorerPromptThread);
    }
    g_restartExplorerPromptThread = CreateThread(
        nullptr, 0, RestartPromptThreadProc, nullptr, 0, nullptr);
}


// Window Filtering (ported from SWS)

static bool TestExStyle(HWND h, DWORD s) { return (s & (DWORD)GetWindowLongPtrW(h, GWL_EXSTYLE)) == s; }
static bool IsOwnerToolWindow(HWND hwnd) {
    HWND cur = hwnd, own = GetWindow(hwnd, GW_OWNER);
    while (!TestExStyle(cur, WS_EX_APPWINDOW) && own) {
        HWND prev = cur; cur = own; own = GetWindow(own, GW_OWNER);
        if (TestExStyle(cur, WS_EX_TOOLWINDOW))
            return !TestExStyle(prev, WS_EX_CONTROLPARENT) || own != NULL;
    }
    return false;
}
static bool IsReallyVisible(HWND h) { RECT r; GetWindowRect(h, &r); return IsWindowVisible(h) && !IsRectEmpty(&r); }
static bool IsGhosted(HWND h) { return g_GhostWindowFromHungWindow && g_GhostWindowFromHungWindow(h) != NULL; }
static bool ShouldListInAltTab(HWND hwnd) {
    if (!IsWindow(hwnd)) return false;
    if (!IsReallyVisible(hwnd)) return false;
    if (IsGhosted(hwnd)) return false;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (style & WS_CHILD) return false;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return false;

    DWORD ex = (DWORD)GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    // WS_EX_TOOLWINDOW always excludes from alt-tab, regardless of other flags
    if (ex & WS_EX_TOOLWINDOW) return false;

    // WS_EX_NOACTIVATE excludes unless WS_EX_APPWINDOW is also set
    if ((ex & WS_EX_NOACTIVATE) && !(ex & WS_EX_APPWINDOW)) return false;

    // Prefer the owner over the child popup/dialog, but only if the owner is actually listable —
    // otherwise both windows disappear from the switcher.
    HWND own = GetWindow(hwnd, GW_OWNER);
    if (!(ex & WS_EX_APPWINDOW) && IsWindow(own) && IsReallyVisible(own) &&
        !(GetWindowLongPtrW(own, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) &&
        !IsOwnerToolWindow(own)) {
        return false;
    }

    // Check if an ancestor in the owner chain is a tool window
    if (IsOwnerToolWindow(hwnd)) return false;

    return true;
}
static bool IsAltTabWindow(HWND h) {
    if (!IsWindow(h)) return false;
    if (g_IsShellFrameWindow && g_IsShellFrameWindow(h) && !(g_GhostWindowFromHungWindow && g_GhostWindowFromHungWindow(h))) return true;
    if (g_IsShellManagedWindow && g_IsShellManagedWindow(h) && !GetPropW(h, L"Microsoft.Windows.ShellManagedWindowAsNormalWindow")) return false;
    if (GetPropW(h, L"valinet.ExplorerPatcher.ShellManagedWindow")) return false;
    return ShouldListInAltTab(h);
}

static bool CanCloseWindow(HWND hWnd) {
    if (!IsWindow(hWnd)) return false;
    if (!IsWindowEnabled(hWnd)) return false; // IsWindowEnabled is exactly !(style & WS_DISABLED)
    HWND hPopup = GetLastActivePopup(hWnd);
    if (hPopup && hPopup != hWnd && IsWindow(hPopup) && IsWindowVisible(hPopup)) {
        return false;
    }
    return true;
}

static std::vector<HWND> s_pendingCloseWindows;
static int s_pendingCloseRetries = 0;
static std::map<HWND, ULONGLONG> s_pendingCloseDeadlines;
static constexpr ULONGLONG SWS_CLOSE_COOLDOWN_MS = 3000;

// Activation MRU (Most Recently Used) tracking
static std::vector<HWND> g_mruWindows;

static void UpdateMruWindow(HWND hWnd) {
    if (!hWnd || IsSwitcherWindow(hWnd)) return;
    HWND hTarget = hWnd;
    if (!IsAltTabWindow(hTarget)) {
        HWND own = GetWindow(hTarget, GW_OWNER);
        if (own && IsAltTabWindow(own)) {
            hTarget = own;
        } else {
            HWND root = GetAncestor(hTarget, GA_ROOTOWNER);
            if (root && IsAltTabWindow(root)) {
                hTarget = root;
            } else {
                return;
            }
        }
    }
    if (!hTarget || IsSwitcherWindow(hTarget)) return;

    auto it = std::find(g_mruWindows.begin(), g_mruWindows.end(), hTarget);
    if (it != g_mruWindows.end()) {
        g_mruWindows.erase(it);
    }
    g_mruWindows.insert(g_mruWindows.begin(), hTarget);
    if (g_mruWindows.size() > 128) {
        g_mruWindows.resize(128);
    }
}

static void RemoveMruWindow(HWND hWnd) {
    if (!hWnd) return;
    auto it = std::remove(g_mruWindows.begin(), g_mruWindows.end(), hWnd);
    g_mruWindows.erase(it, g_mruWindows.end());
}

// MRU rank used for ordering entries: position in g_mruWindows (lower = more
// recently activated). Untracked windows sort after all tracked ones, and
// untracked WS_EX_TOPMOST windows sort last so inactive always-on-top windows
// don't hijack the front of the list.
static int GetMruRank(HWND h) {
    for (size_t i = 0; i < g_mruWindows.size(); i++) {
        if (g_mruWindows[i] == h) return (int)i;
    }
    bool isTopmost = (GetWindowLongPtrW(h, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    return isTopmost ? 20000 : 10000;
}


// Window Enumeration

static HICON TryGetUwpIconFromExplorer(HWND hWnd, int desiredSizePx);

struct CustomHeaderRule {
    std::wstring pattern;   // executable name pattern; wildcards * and ? supported
    std::wstring iconPath;  // .ico / .exe / .dll to extract the icon from (optional)
    std::wstring appName;   // custom display name overriding the detected one (optional)
};
static std::vector<CustomHeaderRule> g_customHeaderRules;

// No shell I/O, application messages or resolution occur while holding either
// lock. Pending includes the active request AND results awaiting UI consumption.
static constexpr size_t SWS_ICON_MAX_PENDING = 128;
static constexpr size_t SWS_ICON_MAX_CELLS = 512;
static constexpr size_t SWS_ICON_MAX_SOURCE_CACHE = 128;
static constexpr DWORD SWS_ICON_NEGATIVE_RETRY_MS = 5000;
static constexpr DWORD SWS_ICON_POSITIVE_RETRY_MS = 30000;
static constexpr DWORD SWS_ICON_SHUTDOWN_WAIT_MS = 1500;

struct WindowIconRequest {
    std::shared_ptr<WindowIconCell> cell;
    ULONGLONG generation = 0;
    std::shared_ptr<const std::vector<CustomHeaderRule>> rules;
};
struct WindowIconResult {
    WindowIconRequest request;
    std::shared_ptr<OwnedWindowIcon> icon;
};
using IconSourceCache = std::map<std::wstring, std::shared_ptr<OwnedWindowIcon>>;
struct WindowIconWorkerState {
    SRWLOCK lock = SRWLOCK_INIT;
    std::atomic<bool> stop{false};
    HWND target = NULL;
    HANDLE wake = NULL;
    HMODULE module = NULL;
    DWORD threadId = 0;
    size_t pending = 0;
    std::deque<WindowIconRequest> queue;
    std::deque<WindowIconResult> results;
    IconSourceCache exeIcons, customIcons, uwpIcons;
    ~WindowIconWorkerState() { if (wake) CloseHandle(wake); }
};
static SRWLOCK s_iconLifecycleLock = SRWLOCK_INIT;
static std::shared_ptr<WindowIconWorkerState> s_iconWorkerState;
static HANDLE s_iconWorkerThread = NULL; // Switcher thread owns the handle.
static std::atomic<bool> s_iconShutdownRequested{false};
static std::map<HWND, std::shared_ptr<WindowIconCell>> s_windowIconCells;
static std::shared_ptr<const std::vector<CustomHeaderRule>> s_iconRules;
static ULONGLONG s_iconSettingsGeneration = 0, s_iconRequestGeneration = 0;
static HICON s_iconPlaceholder = NULL; // LR_SHARED system icon; never destroyed.
static thread_local WindowIconWorkerState* s_resolvingIcons = nullptr;
static thread_local const WindowIconRequest* s_resolvingIconRequest = nullptr;
static thread_local ULONGLONG s_iconMessageDeadline = 0;

static bool IconWindowIdentityMatches(const WindowIconCell& cell) {
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(cell.hWnd, &pid);
    return tid && tid == cell.threadId && pid == cell.processId;
}

static bool IconResolutionCancelled() {
    return s_resolvingIcons &&
           (s_resolvingIcons->stop.load() ||
            s_resolvingIconRequest->cell->retired.load() ||
            !IconWindowIdentityMatches(*s_resolvingIconRequest->cell));
}

// Worker-only. The three WM_GETICON waits plus Explorer IPC share a 1300ms
// deadline, with cancellation between stages. Shell/file/COM calls themselves
// have no Win32 hard timeout (see the bounded shutdown and module reference).
static bool GetWindowIconTimeout(HWND window, UINT message, WPARAM wParam,
                                 LPARAM lParam, DWORD timeout, DWORD_PTR* result) {
    *result = 0;
    if (!s_resolvingIcons || IconResolutionCancelled()) return false;
    ULONGLONG now = GetTickCount64();
    if (now >= s_iconMessageDeadline) return false;
    timeout = (std::min)(timeout, (DWORD)(s_iconMessageDeadline - now));
    return SendMessageTimeoutW(window, message, wParam, lParam,
                               SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT,
                               timeout, result) != 0;
}

// A worker retained across bounded teardown must not call Windhawk APIs.
// Wh_Log concatenates a literal prefix, so keep its format literal at the call
// site instead of forwarding a runtime string through a template.
#define IconResolverLog(format, ...)                  \
    do {                                             \
        if (!s_resolvingIcons) {                      \
            Wh_Log(format __VA_OPT__(,) __VA_ARGS__); \
        }                                            \
    } while (0)

static HICON CacheWorkerSourceIcon(IconSourceCache& cache,
                                   const std::wstring& key, HICON icon) {
    if (!icon) return NULL;
    auto owned = std::make_shared<OwnedWindowIcon>(icon);
    if (cache.size() >= SWS_ICON_MAX_SOURCE_CACHE) cache.erase(cache.begin());
    cache[key] = std::move(owned);
    return icon;
}

static void ForgetWindowIconShadows(HICON icon) {
    for (auto it = s_iconShadowCache.begin(); it != s_iconShadowCache.end();) {
        if (it->first.first == icon) {
            it = s_iconShadowCache.erase(it);
        } else {
            ++it;
        }
    }
}

// Switcher UI only. Retired snapshots still drawing their icons are live here;
// once they drop, the next presented frame releases their cache records. The
// record's strong owner prevents handle reuse before this liveness check, so
// neither worker releases nor reentrant UI invalidation can leave an ABA key.
static void SweepWindowIconShadows() {
    if (s_iconShadowCache.empty()) return;
    // One contiguous allocation per sweep, rather than one node per live icon.
    std::vector<const OwnedWindowIcon*> live;
    live.reserve(s_windowIconCells.size() + g_windows.size() + g_savedAppList.size() +
                 g_scrollTransition.outgoingItems.size() + g_layoutTransition.departingItems.size());
    for (const auto& pair : s_windowIconCells) {
        if (pair.second->icon) live.push_back(pair.second->icon.get());
    }
    auto collect = [&](const auto& entries) {
        for (const auto& entry : entries) {
            if (entry.iconCell && entry.iconCell->icon &&
                entry.hIcon == entry.iconCell->icon->handle)
                live.push_back(entry.iconCell->icon.get());
        }
    };
    collect(g_windows);
    collect(g_savedAppList);
    collect(g_scrollTransition.outgoingItems);
    collect(g_layoutTransition.departingItems);
    auto order = std::less<const OwnedWindowIcon*>{};
    std::sort(live.begin(), live.end(), order);
    for (auto it = s_iconShadowCache.begin(); it != s_iconShadowCache.end();) {
        const auto& owner = it->second->owner;
        if (owner ? !std::binary_search(live.begin(), live.end(), owner.get(), order)
                  : it->first.first != s_iconPlaceholder)
            it = s_iconShadowCache.erase(it);
        else
            ++it;
    }
}

// Called by LoadSettings on the UI, publishing an immutable rules snapshot.
static void ResetWindowIconSettings() {
    for (const auto& pair : s_windowIconCells) pair.second->retired.store(true);
    ++s_iconSettingsGeneration;
    s_iconRules = std::make_shared<const std::vector<CustomHeaderRule>>(g_customHeaderRules);
}

static void InvalidateWindowIcon(HWND window) {
    auto it = s_windowIconCells.find(window);
    if (it == s_windowIconCells.end()) return;
    it->second->retired.store(true);
    if (it->second->icon) ForgetWindowIconShadows(it->second->icon->handle);
    s_windowIconCells.erase(it);
}

// WM_GETICON returns a fixed (usually 32px) icon that looks blurry when scaled
// up to 48/64. PrivateExtractIconsW pulls the best-matching frame from the
// exe's icon resource at the exact requested size for a crisp result.
static HICON TryGetCrispExeIcon(HWND hWnd, int desiredSizePx) {
    if (!s_resolvingIcons || IconResolutionCancelled()) return NULL;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return NULL;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return NULL;
    WCHAR exePath[MAX_PATH] = {0};
    DWORD size = MAX_PATH;
    BOOL ok = !IconResolutionCancelled() && QueryFullProcessImageNameW(hProc, 0, exePath, &size);
    CloseHandle(hProc);
    if (!ok || !exePath[0]) return NULL;

    std::wstring key = std::wstring(exePath) + L"_" + std::to_wstring(desiredSizePx);
    auto& cache = s_resolvingIcons->exeIcons;
    auto it = cache.find(key);
    if (it != cache.end()) return it->second->handle;

    HICON hIcon = NULL;
    if (IconResolutionCancelled()) return NULL;
    if (PrivateExtractIconsW(exePath, 0, desiredSizePx, desiredSizePx,
                             &hIcon, NULL, 1, 0) == 1 && hIcon) {
        return CacheWorkerSourceIcon(cache, key, hIcon);
    }
    if (hIcon) DestroyIcon(hIcon);
    return NULL;
}

// === Custom per-process header (icon and/or application name) ===

static HICON LoadCustomIconFromPath(const std::wstring& path, int sizePx) {
    if (!s_resolvingIcons || IconResolutionCancelled() || path.empty() || sizePx <= 0) return NULL;
    std::wstring key = path + L"_" + std::to_wstring(sizePx);
    auto& cache = s_resolvingIcons->customIcons;
    auto it = cache.find(key);
    if (it != cache.end()) return it->second->handle;
    HICON hIcon = NULL;
    if (PrivateExtractIconsW(path.c_str(), 0, sizePx, sizePx,
                             &hIcon, NULL, 1, 0) == 1 && hIcon) {
        return CacheWorkerSourceIcon(cache, key, hIcon);
    }
    if (hIcon) DestroyIcon(hIcon);
    return NULL;
}

// If the window's executable name matches a user-defined rule, return its
// custom icon. The first matching rule wins; this overrides all other sources.
static HICON TryGetCustomIcon(HWND hWnd, int sizePx) {
    if (!s_resolvingIcons || IconResolutionCancelled()) return NULL;
    const auto& rules = *s_resolvingIconRequest->rules;
    if (rules.empty()) return NULL;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return NULL;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return NULL;
    WCHAR exePath[MAX_PATH] = {0};
    DWORD size = MAX_PATH;
    BOOL ok = !IconResolutionCancelled() && QueryFullProcessImageNameW(hProc, 0, exePath, &size);
    CloseHandle(hProc);
    if (!ok || !exePath[0]) return NULL;
    WCHAR* fileName = PathFindFileNameW(exePath);
    for (const auto& rule : rules) {
        if (PathMatchSpecW(fileName, rule.pattern.c_str())) {
            return LoadCustomIconFromPath(rule.iconPath, sizePx);
        }
    }
    return NULL;
}

// If the window's executable name matches a user-defined rule with a custom
// application name, copy it into `out` and return true. The first matching rule
// wins; an empty field keeps the detected default, not a later matching rule.
static bool TryGetCustomAppName(HWND hWnd, WCHAR* out, size_t cch) {
    if (g_customHeaderRules.empty()) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return false;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return false;
    WCHAR exePath[MAX_PATH] = {0};
    DWORD size = MAX_PATH;
    BOOL ok = QueryFullProcessImageNameW(hProc, 0, exePath, &size);
    CloseHandle(hProc);
    if (!ok || !exePath[0]) return false;
    WCHAR* fileName = PathFindFileNameW(exePath);
    for (const auto& rule : g_customHeaderRules) {
        if (PathMatchSpecW(fileName, rule.pattern.c_str())) {
            if (rule.appName.empty()) return false;
            wcsncpy_s(out, cch, rule.appName.c_str(), _TRUNCATE);
            return true;
        }
    }
    return false;
}

static HICON ResolveWindowIcon(HWND hWnd, int sizePx) {
    if (!s_resolvingIcons || IconResolutionCancelled()) return NULL;
    // User-assigned custom icon takes priority over everything else.
    HICON hIcon = TryGetCustomIcon(hWnd, sizePx);
    if (hIcon) return hIcon;

    // A crisp exe icon extracted at the exact target size beats the WM_GETICON
    // result, which is a fixed ~32px frame: blurry when upscaled to 48/64 and
    // pixelated when downscaled to 16 (GDI does no smoothing in DrawIconEx).
    // PrivateExtractIconsW picks the best-matching frame at the requested size.
    if (!hIcon) {
        hIcon = TryGetCrispExeIcon(hWnd, sizePx);
    }
    DWORD_PTR iconResult = 0;
    for (WPARAM kind : {ICON_BIG, ICON_SMALL2, ICON_SMALL}) {
        if (hIcon || IconResolutionCancelled()) break;
        if (GetWindowIconTimeout(hWnd, WM_GETICON, kind, 0, 100, &iconResult))
            hIcon = (HICON)iconResult;
    }
    if (IconResolutionCancelled()) return NULL;
    if (!hIcon) hIcon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICON);
    if (!hIcon) hIcon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
    // UWP / Modern Packaged apps: Explorer IPC extracts authentic AUMID/IShellItem icons
    if (!hIcon) {
        hIcon = TryGetUwpIconFromExplorer(hWnd, sizePx);
    }
    // NULL is a negative result, cached briefly by the UI. The UI already has
    // the shared application placeholder; never turn a failure into a warm hit.
    return hIcon;
}

static std::shared_ptr<WindowIconWorkerState> GetWindowIconWorkerState() {
    AcquireSRWLockShared(&s_iconLifecycleLock);
    auto state = s_iconWorkerState;
    ReleaseSRWLockShared(&s_iconLifecycleLock);
    return state;
}

static HICON LoadWindowIcon(HWND hWnd, std::shared_ptr<WindowIconCell>* outCell,
                            bool refresh = false) {
    // UI fast path: identity query, bounded cache lookup and queue insertion.
    // In particular, no process metadata, shell calls, CopyIcon or app messages.
    outCell->reset();
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hWnd, &pid);
    if (!tid || !pid || s_iconShutdownRequested.load()) return s_iconPlaceholder;
    auto it = s_windowIconCells.find(hWnd);
    int sizePx = GetHeaderIconSizePx();
    if (it != s_windowIconCells.end() &&
        (it->second->processId != pid || it->second->threadId != tid ||
         it->second->sizePx != sizePx ||
         it->second->settingsGeneration != s_iconSettingsGeneration)) {
        it->second->retired.store(true);
        if (it->second->icon) ForgetWindowIconShadows(it->second->icon->handle);
        s_windowIconCells.erase(it);
        it = s_windowIconCells.end();
    }
    if (it == s_windowIconCells.end()) {
        // Prune only unreferenced, non-pending cells. Entries/snapshots retain
        // ownership, including after HWND destruction or cache replacement.
        if (s_windowIconCells.size() >= SWS_ICON_MAX_CELLS) {
            auto victim = std::find_if(s_windowIconCells.begin(), s_windowIconCells.end(),
                [](const auto& pair) { return pair.second.use_count() == 1 && !pair.second->pending; });
            if (victim == s_windowIconCells.end()) return s_iconPlaceholder;
            if (victim->second->icon) ForgetWindowIconShadows(victim->second->icon->handle);
            s_windowIconCells.erase(victim);
        }
        auto cell = std::make_shared<WindowIconCell>();
        cell->hWnd = hWnd;
        cell->processId = pid;
        cell->threadId = tid;
        cell->sizePx = sizePx;
        cell->settingsGeneration = s_iconSettingsGeneration;
        it = s_windowIconCells.emplace(hWnd, std::move(cell)).first;
    }
    const auto& cell = it->second;
    *outCell = cell;
    if (cell->pending && cell->discardedGeneration.load() == cell->requestGeneration)
        cell->pending = false;
    ULONGLONG now = GetTickCount64();
    // Live redraws can refresh successful icons, but never bypass failure TTLs
    // or repeatedly queue the same request while it is active/awaiting commit.
    bool due = now >= cell->retryAfter || (refresh && cell->icon && !cell->lastFailed);
    auto state = GetWindowIconWorkerState();
    if (due && !cell->pending && state && !state->stop.load()) {
        AcquireSRWLockExclusive(&state->lock);
        if (!state->stop.load() && state->pending < SWS_ICON_MAX_PENDING) {
            cell->requestGeneration = ++s_iconRequestGeneration;
            cell->pending = true;
            ++state->pending;
            state->queue.push_back({cell, cell->requestGeneration, s_iconRules});
            SetEvent(state->wake);
        }
        ReleaseSRWLockExclusive(&state->lock);
    }
    return cell->icon ? cell->icon->handle : s_iconPlaceholder;
}

static DWORD WINAPI WindowIconWorkerThread(LPVOID parameter) {
    auto* holder = static_cast<std::shared_ptr<WindowIconWorkerState>*>(parameter);
    auto state = std::move(*holder);
    delete holder;
    HMODULE module = state->module;
    s_resolvingIcons = state.get();
    HRESULT apartment = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    HRESULT cancellation = CoEnableCallCancellation(NULL);
    while (!state->stop.load()) {
        WindowIconRequest request;
        AcquireSRWLockExclusive(&state->lock);
        if (!state->queue.empty()) {
            request = std::move(state->queue.front());
            state->queue.pop_front();
        } else {
            ResetEvent(state->wake);
        }
        ReleaseSRWLockExclusive(&state->lock);
        if (!request.cell) {
            // Stop may have signaled just before the empty-queue ResetEvent.
            // Check after resetting so that signal cannot become a lost wake.
            if (!state->stop.load()) WaitForSingleObject(state->wake, INFINITE);
            continue;
        }
        s_resolvingIconRequest = &request;
        s_iconMessageDeadline = GetTickCount64() + 1300;
        HICON borrowed = SUCCEEDED(apartment)
                             ? ResolveWindowIcon(request.cell->hWnd, request.cell->sizePx)
                             : NULL;
        // Never destroy the WM_GETICON/class/IPC handle. Every delivered icon
        // is our own CopyIcon, separate from worker caches and Explorer's cache.
        HICON copy = borrowed && !IconResolutionCancelled() ? CopyIcon(borrowed) : NULL;
        WindowIconResult result{request, copy ? std::make_shared<OwnedWindowIcon>(copy) : nullptr};
        s_resolvingIconRequest = nullptr;
        AcquireSRWLockExclusive(&state->lock);
        if (!state->stop.load() && state->target) {
            bool notify = state->results.empty();
            state->results.push_back(std::move(result));
            // Payload stays in the bounded inbox, not a raw pointer in the
            // HWND message queue. Failed posts/teardown cannot leak HICONs.
            if (notify && !PostMessageW(state->target, WM_SWS_ICON_READY, 0, 0)) {
                state->results.pop_back();
                --state->pending;
                request.cell->discardedGeneration.store(request.generation);
            }
        }
        ReleaseSRWLockExclusive(&state->lock);
    }
    state->exeIcons.clear();
    state->customIcons.clear();
    state->uwpIcons.clear();
    if (SUCCEEDED(cancellation)) CoDisableCallCancellation(NULL);
    if (SUCCEEDED(apartment)) CoUninitialize();
    s_resolvingIcons = nullptr;
    state.reset();
    // Release the code reference atomically with thread exit; a timed-out shell
    // call can safely finish after Windhawk has returned from mod teardown.
    FreeLibraryAndExitThread(module, 0);
}

static void StartWindowIconWorker() {
    s_iconPlaceholder = LoadIconW(NULL, IDI_APPLICATION);
    if (s_iconShutdownRequested.load()) return;
    auto state = std::make_shared<WindowIconWorkerState>();
    state->target = g_hSwitcher;
    state->wake = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!state->wake || !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<PCWSTR>(&WindowIconWorkerThread), &state->module)) return;
    auto* holder = new std::shared_ptr<WindowIconWorkerState>(state);
    s_iconWorkerThread = CreateThread(NULL, 0, WindowIconWorkerThread, holder, 0, &state->threadId);
    if (!s_iconWorkerThread) {
        delete holder;
        FreeLibrary(state->module);
        return;
    }
    AcquireSRWLockExclusive(&s_iconLifecycleLock);
    s_iconWorkerState = state;
    if (s_iconShutdownRequested.load()) {
        state->stop.store(true);
        SetEvent(state->wake);
    }
    ReleaseSRWLockExclusive(&s_iconLifecycleLock);
}

// Can be called from WhTool_ModUninit while the UI is still starting up.
static void RequestWindowIconWorkerStop() {
    s_iconShutdownRequested.store(true);
    auto state = GetWindowIconWorkerState();
    if (!state) return;
    state->stop.store(true);
    SetEvent(state->wake);
    CoCancelCall(state->threadId, 0); // Best effort, never wait for COM cancellation.
}

static void StopWindowIconWorker() {
    RequestWindowIconWorkerStop();
    auto state = GetWindowIconWorkerState();
    if (state) {
        std::deque<WindowIconRequest> queue;
        std::deque<WindowIconResult> results;
        AcquireSRWLockExclusive(&state->lock);
        state->target = NULL; // Serialized with posting, before HWND destruction.
        queue.swap(state->queue);
        results.swap(state->results);
        state->pending = 0;
        ReleaseSRWLockExclusive(&state->lock);
    }
    if (s_iconWorkerThread) {
        // Message waits are <=1300ms. In-process shell extensions/file I/O may
        // exceed this; retain their private state/code instead of killing a
        // thread, unloading executing code, or blocking mod unload indefinitely.
        WaitForSingleObject(s_iconWorkerThread, SWS_ICON_SHUTDOWN_WAIT_MS);
        CloseHandle(s_iconWorkerThread);
        s_iconWorkerThread = NULL;
    }
    AcquireSRWLockExclusive(&s_iconLifecycleLock);
    s_iconWorkerState.reset();
    ReleaseSRWLockExclusive(&s_iconLifecycleLock);
    for (const auto& pair : s_windowIconCells) {
        if (pair.second->icon) ForgetWindowIconShadows(pair.second->icon->handle);
    }
    s_windowIconCells.clear();
    s_iconRules.reset();
}

static void ConsumeWindowIconResults() {
    auto state = GetWindowIconWorkerState();
    if (!state || state->stop.load()) return;
    std::deque<WindowIconResult> results;
    AcquireSRWLockExclusive(&state->lock);
    results.swap(state->results);
    state->pending -= results.size();
    ReleaseSRWLockExclusive(&state->lock);
    if (results.empty()) return;
    bool changed = false;
    for (auto& result : results) {
        const auto& cell = result.request.cell;
        if (cell->requestGeneration != result.request.generation) continue;
        cell->pending = false;
        auto current = s_windowIconCells.find(cell->hWnd);
        if (s_iconShutdownRequested.load() || current == s_windowIconCells.end() ||
            current->second != cell || cell->settingsGeneration != s_iconSettingsGeneration ||
            !IconWindowIdentityMatches(*cell)) continue;
        cell->retryAfter = GetTickCount64() + (result.icon ? SWS_ICON_POSITIVE_RETRY_MS
                                                         : SWS_ICON_NEGATIVE_RETRY_MS);
        cell->lastFailed = !result.icon;
        if (!result.icon) continue; // Preserve a previously successful icon.
        if (cell->icon) ForgetWindowIconShadows(cell->icon->handle);
        cell->icon = std::move(result.icon);
        auto update = [&](auto& entries) {
            for (auto& entry : entries) {
                if (entry.hWnd == cell->hWnd && entry.iconCell == cell) {
                    entry.hIcon = cell->icon->handle;
                    changed = true;
                }
            }
        };
        update(g_windows);
        update(g_savedAppList);
        update(g_scrollTransition.outgoingItems);
        update(g_layoutTransition.departingItems);
    }
    // Overflow was intentionally deferred. Revisit live and saved UI cells
    // after freeing slots, without enumeration or any synchronous resolver.
    auto refill = [&](auto& entries) {
        for (auto& entry : entries) {
            if (entry.iconCell && (entry.iconCell->retired.load() ||
                                  !IconWindowIdentityMatches(*entry.iconCell))) continue;
            HICON previous = entry.hIcon;
            entry.hIcon = LoadWindowIcon(entry.hWnd, &entry.iconCell);
            changed |= entry.hIcon != previous;
        }
    };
    refill(g_windows);
    refill(g_savedAppList);
    if (changed) {
        InvalidateStaticCache();
        if (g_scrollTransition.active) s_scrollIconCanvasDirty = true;
        if (g_isVisible) {
            // Use a single ticker turn even when idle: many ready batches before
            // its deadline must not each perform a complete static repaint.
            s_iconRepaintPending = true;
            StartAnimationTicker();
        }
    }
}

static void GetWindowGroupKey(HWND hWnd, WCHAR* out, size_t cch);

typedef BOOL (WINAPI *pfnIsGamingFullScreenExperienceActive)();

static bool IsGamingExperienceActive() {
    static HMODULE s_hGamemode = LoadLibraryW(L"api-ms-win-gaming-experience-l1-1-0.dll");
    if (!s_hGamemode) {
        s_hGamemode = LoadLibraryW(L"gamemode.dll");
    }
    if (!s_hGamemode) return false;
    static pfnIsGamingFullScreenExperienceActive s_pfn = 
        (pfnIsGamingFullScreenExperienceActive)GetProcAddress(s_hGamemode, "IsGamingFullScreenExperienceActive");
    return s_pfn ? (s_pfn() != FALSE) : false;
}

static bool IsXboxModeOrForeground() {
    if (IsGamingExperienceActive()) return true;
    HWND hFg = GetForegroundWindow();
    if (hFg) {
        WCHAR cls[64] = {0};
        GetClassNameW(hFg, cls, 64);
        if (wcscmp(cls, L"ApplicationFrameWindow") == 0) {
            WCHAR title[64] = {0};
            GetWindowTextW(hFg, title, 64);
            if (_wcsicmp(title, L"Xbox") == 0 || _wcsicmp(title, L"XBOX") == 0) {
                return true;
            }
        }
    }
    return false;
}

static bool IsEligibleWindow(HWND hWnd, WindowEntry* outEntry = nullptr) {
    if (!hWnd || !IsWindow(hWnd) || IsSwitcherWindow(hWnd) || IsNativeSwitcherWindow(hWnd)) return false;
    if (!IsAltTabWindow(hWnd)) return false;

    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) {
        // Exclude windows cloaked by the app itself (e.g. dormant/suspended UWP background frames)
        if (cloaked & DWM_CLOAKED_APP) return false;

        // Shell cloaked (DWM_CLOAKED_SHELL):
        // In Gaming Full Screen Experience (Xbox Mode) or Tablet Mode, the Windows
        // Shell cloaks background desktop windows on the current desktop.
        // These are valid open applications and MUST be shown in Alt+Tab!
        if (g_pVirtualDesktopManager) {
            BOOL onCurrent = FALSE;
            if (SUCCEEDED(g_pVirtualDesktopManager->IsWindowOnCurrentVirtualDesktop(hWnd, &onCurrent))) {
                if (onCurrent) {
                    // Valid window on current desktop cloaked by the shell -> keep it!
                } else {
                    if (wcscmp(g_settings.virtualDesktopBehavior, L"allDesktops") != 0) {
                        return false;
                    }
                }
            } else {
                return false;
            }
        } else {
            if (wcscmp(g_settings.virtualDesktopBehavior, L"allDesktops") != 0) {
                return false;
            }
        }
    }

    bool isPrimaryOnly = (wcscmp(g_settings.switcherDisplayBehavior, L"primaryOnly") == 0);
    if (g_settings.perMonitorWindows && !g_showAllMonitors && g_hCurrentMonitor && !isPrimaryOnly) {
        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL) != g_hCurrentMonitor) return false;
    }

    if (g_isAltBacktickSameApp) {
        WCHAR targetKey[MAX_PATH] = {};
        GetWindowGroupKey(hWnd, targetKey, ARRAYSIZE(targetKey));
        if (!g_sameAppSessionKey[0] || wcscmp(targetKey, g_sameAppSessionKey) != 0) return false;
    }

    WCHAR title[256] = {0};
    GetWindowTextW(hWnd, title, 256);
    if (!title[0]) InternalGetWindowText(hWnd, title, 256);

    // Reject windows with empty titles, internal XAML island titles, or browser helper titles
    if (!title[0]) return false;
    if (_wcsicmp(title, L"DesktopWindowXamlSource") == 0) return false;
    if (_wcsicmp(title, L"Chrome Legacy Window") == 0) return false;

    // Check class name to filter out shell components and framework helper windows
    WCHAR cls[256] = {0};
    GetClassNameW(hWnd, cls, 256);
    if (wcscmp(cls, L"DesktopWindowXamlSource") == 0 ||
        wcscmp(cls, L"Chrome_RenderWidgetHostHWND") == 0 ||
        wcscmp(cls, L"Intermediate D3D Window") == 0 ||
        wcscmp(cls, L"MozillaDropShadowWindowClass") == 0 ||
        wcscmp(cls, L"Shell_TrayWnd") == 0 ||
        wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
        wcscmp(cls, L"Progman") == 0 ||
        wcscmp(cls, L"WorkerW") == 0 ||
        wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
        wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0 ||
        wcscmp(cls, L"XamlExplorerHostIslandWindow_WASDK") == 0 ||
        wcscmp(cls, L"DesktopWindowContentBridge") == 0 ||
        wcscmp(cls, L"InputNonClientPointerSource") == 0 ||
        wcsstr(cls, L"DesktopChildSiteBridge") != nullptr ||
        wcsstr(cls, L"AvaloniaSimpleWindow") != nullptr ||
        wcsstr(cls, L"AvaloniaMessageWindow") != nullptr ||
        wcsstr(cls, L"PopupHost") != nullptr) {
        return false;
    }

    // Geometry check: non-minimized windows must be at least 32x32 to exclude 1x1 or 0x0 anchor surfaces
    if (!IsIconic(hWnd)) {
        RECT r = {0};
        GetWindowRect(hWnd, &r);
        if ((r.right - r.left) < 32 || (r.bottom - r.top) < 32) return false;
    }

    for (const auto& pat : g_excludeTitlePatterns) {
        if (PathMatchSpecW(title, pat.c_str())) return false;
    }

    if (!g_excludeExePatterns.empty()) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid) {
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (hProc) {
                WCHAR exePath[MAX_PATH] = {0};
                DWORD size = MAX_PATH;
                if (QueryFullProcessImageNameW(hProc, 0, exePath, &size)) {
                    WCHAR* filename = PathFindFileNameW(exePath);
                    for (const auto& pat : g_excludeExePatterns) {
                        if (PathMatchSpecW(filename, pat.c_str())) {
                            CloseHandle(hProc);
                            return false;
                        }
                    }
                }
                CloseHandle(hProc);
            }
        }
    }

    if (outEntry) {
        outEntry->hWnd = hWnd;
        wcscpy_s(outEntry->title, title);
        outEntry->hIcon = LoadWindowIcon(hWnd, &outEntry->iconCell);
    }
    return true;
}

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<WindowEntry>*>(lParam);
    WindowEntry e = {};
    if (IsEligibleWindow(hWnd, &e)) {
        list->push_back(e);
    }
    return TRUE;
}

// === UWP Icon Extraction (Explorer IPC) ===

UINT g_WM_SWS_GET_UWP_ICON = 0;
UINT g_WM_SWS_TOUCHPAD_FRAME = 0;
static bool g_isTouchpadGestureActive = false;
static std::atomic<bool> g_touchpadReaderAvailable{false};
static std::atomic<bool> g_touchpadReaderStopping{false};
static std::atomic<bool> g_touchpadGesturesEnabled{true};
// Published to the raw-reader thread so an already-visible switcher can claim
// the next three-finger stroke before its UI message is dispatched.
static std::atomic<bool> g_touchpadSwitcherActive{false};
static std::atomic<bool> g_touchpadStickyLaunchEnabled{true};
static std::atomic<bool> g_touchpadGestureTakeoverAvailable{true};
// Built-in laptop traces use a single HID finger collection and historically
// reach the native target before raw tips are available. The Apple driver
// descriptor used by the reported external device exposes five collections;
// only that positively observed multi-collection profile requires raw proof
// before an outside native gesture is claimed.
static std::atomic<bool> g_touchpadNativeCandidateRequired{false};
// The reader and the UI thread share the physical three-finger stroke phase.
// The native Explorer hook cannot read this atomic directly; it uses the
// timestamp candidate window property published from the same reader thread.
static std::atomic<ULONGLONG> g_touchpadRawThreeFingerNextSerial{0};
static std::atomic<ULONGLONG> g_touchpadRawThreeFingerState{0};
static std::atomic<ULONGLONG> g_touchpadRawThreeFingerLastTick{0};
// The reader publishes tap evidence before posting to the UI. The mouse hook
// can then suppress the promoted right-click before it cancels/focuses an app
// under the cursor. High 32 bits identify the stroke; low 32 bits are expiry.
static std::atomic<bool> g_touchpadTwoFingerCloseActive{false};
static std::atomic<bool> g_touchpadTwoFingerTapSuppressed{false};
static std::atomic<DWORD> g_touchpadRawTwoFingerNextSerial{0};
static std::atomic<ULONGLONG> g_touchpadRawTwoFingerTapMouseState{0};
// Settings callbacks run outside the switcher thread. These atomics are the
// cross-thread source of truth until WM_SWS_SETTINGS_CHANGED reloads the full
// UI settings snapshot on the switcher thread.
static bool TouchpadHandlingEnabled() {
    return g_touchpadGesturesEnabled.load() && !g_touchpadReaderStopping.load();
}
static bool TouchpadStickyLaunchEnabled() {
    return g_touchpadStickyLaunchEnabled.load();
}
// Diagnostic totals are process-lifetime counters, not gesture ownership.
// Atomics let the UI sample the reader without delaying its input thread.
struct TouchpadInputDiagnostics {
    std::atomic<DWORD> rawMessages{0};
    std::atomic<DWORD> hidReports{0};
    std::atomic<DWORD> rejectedDevices{0};
    std::atomic<DWORD> readFailures{0};
    std::atomic<DWORD> frames{0};
    std::atomic<DWORD> contactFrames{0};
    std::atomic<DWORD> hybridReports{0};
    std::atomic<DWORD> emptyReports{0};
    std::atomic<DWORD> posted{0};
    std::atomic<DWORD> postFailures{0};
    std::atomic<DWORD> uiFrames{0};
    std::atomic<DWORD> lastReadError{0};
    std::atomic<DWORD> lastPostError{0};
    std::atomic<ULONG> lastTips{0};
    std::atomic<ULONGLONG> lastRawTick{0};
    std::atomic<ULONGLONG> lastFrameTick{0};
    std::atomic<ULONGLONG> lastUiTick{0};
};
static TouchpadInputDiagnostics s_touchpadInputDiagnostics;
static std::atomic<HWND> g_touchpadReaderWindow{nullptr};
static std::wstring FormatTouchpadInputDiagnostics();

struct TouchpadProcessSecurity {
    DWORD integrity = 0;
    int elevated = -1;
    int uiAccess = -1;
    DWORD error = ERROR_SUCCESS;
};

static TouchpadProcessSecurity ReadTouchpadProcessSecurity(DWORD pid) {
    TouchpadProcessSecurity result;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        result.error = GetLastError();
        return result;
    }
    HANDLE token = nullptr;
    BOOL opened = OpenProcessToken(process, TOKEN_QUERY, &token);
    DWORD error = opened ? ERROR_SUCCESS : GetLastError();
    CloseHandle(process);
    if (!opened) {
        result.error = error;
        return result;
    }

    DWORD returned = 0;
    TOKEN_ELEVATION elevation{};
    if (GetTokenInformation(token, TokenElevation, &elevation,
                            sizeof(elevation), &returned)) {
        result.elevated = elevation.TokenIsElevated != 0;
    } else {
        result.error = GetLastError();
    }
    DWORD uiAccess = 0;
    if (GetTokenInformation(token, TokenUIAccess, &uiAccess,
                            sizeof(uiAccess), &returned)) {
        result.uiAccess = uiAccess != 0;
    } else if (!result.error) {
        result.error = GetLastError();
    }
    alignas(TOKEN_MANDATORY_LABEL) BYTE labelBuffer[
        sizeof(TOKEN_MANDATORY_LABEL) + SECURITY_MAX_SID_SIZE]{};
    if (GetTokenInformation(token, TokenIntegrityLevel, labelBuffer,
                            sizeof(labelBuffer), &returned)) {
        auto* label = reinterpret_cast<TOKEN_MANDATORY_LABEL*>(labelBuffer);
        if (IsValidSid(label->Label.Sid) &&
            *GetSidSubAuthorityCount(label->Label.Sid)) {
            BYTE count = *GetSidSubAuthorityCount(label->Label.Sid);
            result.integrity = *GetSidSubAuthority(label->Label.Sid, count - 1);
        } else if (!result.error) {
            result.error = ERROR_INVALID_SID;
        }
    } else if (!result.error) {
        result.error = GetLastError();
    }
    CloseHandle(token);
    return result;
}

// Called inside Wh_Log arguments so token queries/formatting run only while
// debug logging is enabled. Unknown/denied token access is never labeled medium.
static std::wstring FormatTouchpadForegroundDiagnostic(HWND foreground, HWND target) {
    DWORD sourcePid = 0, targetPid = 0;
    GetWindowThreadProcessId(foreground, &sourcePid);
    if (target) GetWindowThreadProcessId(target, &targetPid);
    DWORD selfPid = GetCurrentProcessId();
    auto self = ReadTouchpadProcessSecurity(selfPid);
    auto source = ReadTouchpadProcessSecurity(sourcePid);
    auto destination = target ? ReadTouchpadProcessSecurity(targetPid) : TouchpadProcessSecurity{};
    WCHAR text[768];
    swprintf_s(text,
        L"callerPid=%u callerIL=0x%X callerElevated=%d callerUIAccess=%d callerTokenError=%u "
        L"foreground=%p foregroundPid=%u foregroundIL=0x%X foregroundElevated=%d foregroundUIAccess=%d foregroundTokenError=%u "
        L"target=%p targetPid=%u targetIL=0x%X targetElevated=%d targetUIAccess=%d targetTokenError=%u",
        selfPid, self.integrity, self.elevated, self.uiAccess, self.error,
        foreground, sourcePid, source.integrity, source.elevated, source.uiAccess, source.error,
        target, targetPid, destination.integrity, destination.elevated, destination.uiAccess, destination.error);
    return text;
}

static void RequestTouchpadInputDiagnostics(HWND endpoint, UINT event) {
    if (!endpoint) return;
    // Diagnostic-only, asynchronous sampling. No cross-process pointers or
    // waits; this message cannot launch, navigate, suppress, or activate SWS.
    if (!PostMessageW(endpoint, WM_SWS_TOUCHPAD_DIAGNOSTICS, event, 0)) {
        DWORD error = GetLastError();
        Wh_Log(L"SWS: native input snapshot post failed (event=0x%X endpoint=%p error=%u)",
               event, endpoint, error);
    }
}
// Raw-frame gesture state; switcher thread only.
// Three-finger strokes and visible-session two-finger taps are classified here;
// ordinary panning remains outside this state machine.
static ULONGLONG s_rawTouchpadLastFrameTick = 0;
static bool s_rawSessionOwned = false;
static int s_rawGestureTips = 0;
static bool s_rawGestureArmed = false;
static int s_rawGestureAnchorX = 0;
static int s_rawGestureAnchorY = 0;
static int s_rawGestureAxis = 0;  // 0 = none yet, 1 = across entries, 2 = rows
static int s_rawAppliedX = 0;     // entries applied since the current direction started
static ULONGLONG s_rawRowTick = 0; // pacing for the vertical axis
static ULONGLONG s_rawSwipePublishTick = 0; // last publish of the raw 3-finger swipe marker
static ULONGLONG s_rawGestureStartTick = 0;
static bool s_rawGestureSawThree = false;
static bool s_rawGestureMoved = false;
static bool s_rawTapEligible = false;
static bool s_rawIgnoreUntilLift = false;
static bool s_rawSwipePassedToWindows = false;
static bool s_rawTouchpadShieldActive = false;
static bool s_rawTouchpadShieldReleasePending = false;
static bool s_rawSwipeMarkerActive = false;
// Physical-stroke evidence survives UI cancellation, but never a complete lift.
static bool s_rawStrokeOwned = false;
static bool s_rawTwoFingerTapActive = false;
static bool s_rawTwoFingerTapMoved = false;
static ULONGLONG s_rawTwoFingerTapStartTick = 0;
static int s_rawTwoFingerTapOriginX = 0;
static int s_rawTwoFingerTapOriginY = 0;
static ULONGLONG s_rawTouchpadShieldFocusStartTick = 0;
static HWND s_rawShieldRestoreForeground = NULL;
static HWND s_touchpadActivationRetryTarget = NULL;
static ULONGLONG s_touchpadActivationRetryDeadline = 0;
static bool s_touchpadCommitActivation = false;
// A normal Alt session and a normal three-finger session are separate owners.
// The switcher commits only after both owners have released.
static bool s_altSessionOwner = false;
static bool s_altHeld = false;
static ULONGLONG s_altRawBaselineSerial = 0;
static ULONGLONG s_combinedRawSerial = 0;
static bool s_combinedRawSeen = false;
static bool s_combinedRawLiftHandled = false;
static bool s_commitStarted = false;
struct NativeTouchpadInvocation {
    DWORD token = 0;
    DWORD epoch = 0;
    bool rawAdopted = false;
    bool upward = false;
    UINT completion = 0;
    DWORD completionTick = 0;
    ULONGLONG completionDeadline = 0;
};
static NativeTouchpadInvocation s_nativeTouchpadInvocation;
static DWORD s_nativeTouchpadRawDiscardTick = 0;
static bool s_nativeTouchpadRawDiscardPending = false;
static bool SwitcherOwnsRawSwipe();
static bool SwitcherOwnsActiveRawSwipe();
static bool NativeSwipeSourceGateActive();
static bool CombinedRawReleasePending();
static void AdoptRawThreeFingerState(bool allowCurrentLive = false);

// Serialize the two in-process publishers with expiry/removal. Explorer only
// reads the property and never takes this lock or waits on the switcher queue.
static SRWLOCK s_rawSwipeMarkerLock = SRWLOCK_INIT;

// Protected by the publisher lock; Explorer reads properties, never tool globals.
static DWORD s_nativeSwipePolicyEpoch = 1;
static bool s_nativeSwipePolicyReady = false;
static std::atomic<DWORD> s_nativeTouchpadRelayEpoch{1};

static void PublishNativeSwipePolicy() {
    AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
    bool enabled = g_touchpadGesturesEnabled.load();
    bool reader = g_touchpadReaderAvailable.load() && !g_touchpadReaderStopping.load();
    bool ready = enabled && reader;
    if (s_nativeSwipePolicyReady && !ready) {
        s_nativeSwipePolicyEpoch = (s_nativeSwipePolicyEpoch + 1) & 0x1FFFFFFFu;
        if (!s_nativeSwipePolicyEpoch) s_nativeSwipePolicyEpoch = 1;
        s_nativeTouchpadRelayEpoch = (s_nativeTouchpadRelayEpoch + 1) & 0x1FFFFFFFu;
        if (!s_nativeTouchpadRelayEpoch) s_nativeTouchpadRelayEpoch = 1;
    }
    s_nativeSwipePolicyReady = ready;
    if (g_hSwitcher) {
        DWORD policy = (s_nativeSwipePolicyEpoch << 3) | (enabled ? 1u : 0u) |
                       (reader ? 2u : 0u) |
                       (g_touchpadStickyLaunchEnabled.load() ? 4u : 0u);
        bool profilePublished = true;
        if (g_touchpadNativeCandidateRequired.load()) {
            profilePublished = SetPropW(g_hSwitcher, SWS_NATIVE_SWIPE_PROFILE_PROP,
                                        (HANDLE)(ULONG_PTR)1);
        } else {
            RemovePropW(g_hSwitcher, SWS_NATIVE_SWIPE_PROFILE_PROP);
        }
        if (SetPropW(g_hSwitcher, SWS_NATIVE_SWIPE_POLICY_PROP,
                     (HANDLE)(ULONG_PTR)policy) &&
            SetPropW(g_hSwitcher, SWS_NATIVE_TOUCHPAD_EPOCH_PROP,
                     (HANDLE)(ULONG_PTR)s_nativeTouchpadRelayEpoch.load()) &&
            profilePublished) {
            if (ready && g_touchpadSwitcherActive.load()) {
                SetPropW(g_hSwitcher, SWS_NATIVE_SWIPE_ACTIVE_PROP,
                         (HANDLE)(ULONG_PTR)1);
            } else {
                RemovePropW(g_hSwitcher, SWS_NATIVE_SWIPE_ACTIVE_PROP);
            }
        } else {
            // Fail open if readiness cannot be published reliably.
            RemovePropW(g_hSwitcher, SWS_NATIVE_SWIPE_POLICY_PROP);
            RemovePropW(g_hSwitcher, SWS_NATIVE_SWIPE_ACTIVE_PROP);
            RemovePropW(g_hSwitcher, SWS_NATIVE_TOUCHPAD_EPOCH_PROP);
            RemovePropW(g_hSwitcher, SWS_NATIVE_SWIPE_PROFILE_PROP);
        }
    }
    ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
}

static void RemoveNativeSwipePolicy() {
    AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
    if (g_hSwitcher) {
        RemovePropW(g_hSwitcher, SWS_NATIVE_SWIPE_POLICY_PROP);
        RemovePropW(g_hSwitcher, SWS_NATIVE_SWIPE_ACTIVE_PROP);
        RemovePropW(g_hSwitcher, SWS_NATIVE_TOUCHPAD_EPOCH_PROP);
        RemovePropW(g_hSwitcher, SWS_NATIVE_SWIPE_PROFILE_PROP);
        RemovePropW(g_hSwitcher, SWS_RAW_THREE_CANDIDATE_PROP);
    }
    ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
}

static HANDLE EncodeRawSwipeMarker(ULONGLONG tick, bool liftGrace) {
    DWORD stamp = (DWORD)tick & SWS_RAW_SWIPE_TIMESTAMP_MASK;
    if (!stamp) {
        // Zero means no property. Use the previous tick at wrap, not a future
        // tick (which modular age arithmetic would immediately expire).
        stamp = SWS_RAW_SWIPE_TIMESTAMP_MASK;
    }
    if (liftGrace) stamp |= SWS_RAW_SWIPE_LIFT_FLAG;
    return (HANDLE)(ULONG_PTR)stamp;
}

static DWORD RawSwipeMarkerRemainingMs(HANDLE marker, ULONGLONG tick) {
    if (!marker) return 0;
    DWORD encoded = (DWORD)(ULONG_PTR)marker;
    DWORD age = ((DWORD)tick - (encoded & SWS_RAW_SWIPE_TIMESTAMP_MASK)) &
                SWS_RAW_SWIPE_TIMESTAMP_MASK;
    DWORD lifetime = (encoded & SWS_RAW_SWIPE_LIFT_FLAG)
                         ? SWS_RAW_SWIPE_LIFT_GRACE_MS
                         : SWS_RAW_SWIPE_OWNER_MS;
    return age < lifetime ? lifetime - age : 0;
}

static HANDLE EncodeRawThreeFingerCandidate(ULONGLONG tick) {
    DWORD stamp = (DWORD)tick & SWS_RAW_SWIPE_TIMESTAMP_MASK;
    if (!stamp) stamp = SWS_RAW_SWIPE_TIMESTAMP_MASK;
    return (HANDLE)(ULONG_PTR)stamp;
}

static DWORD RawThreeFingerCandidateRemainingMs(HANDLE candidate, ULONGLONG tick) {
    if (!candidate) return 0;
    DWORD encoded = (DWORD)(ULONG_PTR)candidate;
    DWORD age = ((DWORD)tick - (encoded & SWS_RAW_SWIPE_TIMESTAMP_MASK)) &
                SWS_RAW_SWIPE_TIMESTAMP_MASK;
    return age < SWS_RAW_THREE_CANDIDATE_MS
               ? SWS_RAW_THREE_CANDIDATE_MS - age
               : 0;
}

static void PublishRawThreeFingerCandidate(ULONGLONG tick) {
    if (!g_hSwitcher) return;
    AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
    if (TouchpadHandlingEnabled() && g_touchpadReaderAvailable.load()) {
        SetPropW(g_hSwitcher, SWS_RAW_THREE_CANDIDATE_PROP,
                 EncodeRawThreeFingerCandidate(tick));
    } else {
        RemovePropW(g_hSwitcher, SWS_RAW_THREE_CANDIDATE_PROP);
    }
    ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
}

static void ClearRawThreeFingerCandidateProperty() {
    if (!g_hSwitcher) return;
    AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
    RemovePropW(g_hSwitcher, SWS_RAW_THREE_CANDIDATE_PROP);
    ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
}

// Explorer reads this property without taking the tool-process lock. Like the
// existing raw-swipe marker, the value is only an expiring timestamp; a stale
// or missing property fails open to Windows' native gesture handling.
static bool FreshRawThreeFingerCandidate(HWND endpoint, ULONGLONG tick) {
    return endpoint && RawThreeFingerCandidateRemainingMs(
                             GetPropW(endpoint, SWS_RAW_THREE_CANDIDATE_PROP),
                             tick) != 0;
}

#define SWS_RAW_THREE_PHASE_NONE 0u
#define SWS_RAW_THREE_PHASE_LIVE 1u
#define SWS_RAW_THREE_PHASE_LIFTED 2u
#define SWS_RAW_THREE_PHASE_LOST 3u

static ULONGLONG RawThreeFingerStateSerial(ULONGLONG state) {
    return state >> 2;
}

static ULONG RawThreeFingerStatePhase(ULONGLONG state) {
    return (ULONG)(state & 3u);
}

static void PublishRawThreeFingerState(ULONGLONG serial, ULONG phase,
                                       ULONGLONG tick) {
    if (!serial) return;
    g_touchpadRawThreeFingerLastTick.store(tick, std::memory_order_release);
    g_touchpadRawThreeFingerState.store((serial << 2) | (phase & 3u),
                                        std::memory_order_release);
}

static void SetRawSwipeUpMarker(bool upward) {
    if (!g_hSwitcher) return;
    AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
    if (upward && TouchpadHandlingEnabled() &&
        g_touchpadReaderAvailable.load()) {
        SetPropW(g_hSwitcher, SWS_RAW_SWIPE_UP_PROP, (HANDLE)(ULONG_PTR)1);
    } else {
        RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_UP_PROP);
    }
    ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
}

static BOOL SetRawSwipeMarker(bool liftGrace, bool stickyLaunchOnly = false,
                              bool sessionOwned = true) {
    AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
    BOOL result = FALSE;
    if (g_hSwitcher && TouchpadHandlingEnabled() &&
        g_touchpadReaderAvailable.load() &&
        (!stickyLaunchOnly || TouchpadStickyLaunchEnabled())) {
        HANDLE encoded = EncodeRawSwipeMarker(GetTickCount64(), liftGrace);
        result = SetPropW(g_hSwitcher, SWS_RAW_SWIPE_PROP, encoded);
        if (result && sessionOwned) {
            SetPropW(g_hSwitcher, SWS_RAW_SWIPE_SESSION_PROP, encoded);
        } else {
            RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_SESSION_PROP);
        }
    }
    ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
    return result;
}

// Switcher-thread only. A queued old timer rechecks the latest timestamp under
// the publisher lock, so it cannot erase a new reader-thread publication.
static void UpdateRawSwipeMarkerTimer() {
    if (!g_hSwitcher) return;
    KillTimer(g_hSwitcher, SWS_RAW_SWIPE_MARKER_EXPIRY_TIMER_ID);
    AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
    HANDLE marker = GetPropW(g_hSwitcher, SWS_RAW_SWIPE_PROP);
    HANDLE sessionMarker = GetPropW(g_hSwitcher, SWS_RAW_SWIPE_SESSION_PROP);
    ULONGLONG now = GetTickCount64();
    DWORD remaining = RawSwipeMarkerRemainingMs(marker, now);
    DWORD sessionRemaining = RawSwipeMarkerRemainingMs(sessionMarker, now);
    if (marker && !remaining) {
        RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_PROP);
        RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_UP_PROP);
    }
    if (sessionMarker && !sessionRemaining) {
        RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_SESSION_PROP);
    }
    DWORD timerRemaining = (std::min)(remaining ? remaining : 0xFFFFFFFFu,
                                      sessionRemaining ? sessionRemaining : 0xFFFFFFFFu);
    if (timerRemaining == 0xFFFFFFFFu) timerRemaining = 0;
    ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
    if ((marker && !remaining) || (sessionMarker && !sessionRemaining)) {
        Wh_Log(L"SWS: raw swipe marker expiry update (marker=%p session=%p)",
               marker, sessionMarker);
    }
    if (timerRemaining) {
        SetTimer(g_hSwitcher, SWS_RAW_SWIPE_MARKER_EXPIRY_TIMER_ID,
                 timerRemaining, NULL);
    }
}

static void ClearRawSwipeMarker() {
    if (g_hSwitcher) {
        KillTimer(g_hSwitcher, SWS_RAW_SWIPE_MARKER_EXPIRY_TIMER_ID);
        AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
        RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_PROP);
        RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_SESSION_PROP);
        RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_UP_PROP);
        ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
    }
    s_rawSwipeMarkerActive = false;
    s_rawStrokeOwned = false;
    s_rawSwipePublishTick = 0;
}

// The reader thread cannot safely manipulate the switcher thread's timer, but
// it can retire the property under the same publisher lock. This prevents a
// new outside stroke from inheriting the previous session's lift grace before
// the posted frame reaches the switcher window procedure.
static void ClearRawSwipeMarkerPropertyFromReader() {
    if (!g_hSwitcher) return;
    AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
    RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_PROP);
    RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_SESSION_PROP);
    RemovePropW(g_hSwitcher, SWS_RAW_SWIPE_UP_PROP);
    ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
}

static bool ExplicitShellInputDown() {
    return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
           (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ||
           (GetAsyncKeyState(VK_MBUTTON) & 0x8000) ||
           (GetAsyncKeyState(VK_LWIN) & 0x8000) ||
           (GetAsyncKeyState(VK_RWIN) & 0x8000);
}

// A fresh vertical stroke is a command; horizontal-first movement is a drag.
// Once a command is consumed, the same contacts cannot execute another command.
static bool s_rawCommandEligible = false;
static int s_rawTapOriginX = 0, s_rawTapOriginY = 0;
static int s_rawDirectionAnchorX = 0, s_rawDirectionAnchorY = 0;
// Opening/drilling consumes that vertical direction until a turn or a new stroke.
static int s_rawVerticalActionDir = 0;
static int s_rawVerticalTravelDir = 0;
static int s_rawLastFrameX = 0;
static int s_rawLastFrameY = 0;

static void FinishRawTouchpadShield(bool keepMarkerGrace = false);
static void ScheduleRawTouchpadShieldRelease();

static bool RawTouchpadOwnsInput() {
    return TouchpadHandlingEnabled() && g_touchpadReaderAvailable.load() &&
           (s_rawSessionOwned || s_rawTouchpadShieldActive ||
            CombinedRawReleasePending()) &&
           (g_isVisible || g_isPendingShow || s_rawTouchpadShieldActive) &&
           !g_animExitActive && !g_isHidingSwitcher;
}

// The raw reader can identify a three-finger stroke before the foreground
// controller has won routing. Explorer-side hooks use this marker to suppress
// shell actions during that short race, but only after a switcher session has
// actually been opened. An outside stroke remains Windows' gesture, including
// Show Desktop and the configured outside tap action.
static bool RawTouchpadShellSuppressionActive() {
    return TouchpadHandlingEnabled() && g_touchpadReaderAvailable.load() &&
           !ExplicitShellInputDown() &&
           SwitcherOwnsRawSwipe();
}

static void CancelNativeTouchpadInvocation() {
    s_nativeTouchpadInvocation = {};
    if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_NATIVE_TOUCHPAD_COMPLETION_TIMER_ID);
    // The endpoint publishes this epoch. Invalidate queued starts/completions
    // without changing the source gate's ownership of the cancelled stroke.
    AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
    s_nativeTouchpadRelayEpoch = (s_nativeTouchpadRelayEpoch + 1) & 0x1FFFFFFFu;
    if (!s_nativeTouchpadRelayEpoch) s_nativeTouchpadRelayEpoch = 1;
    if (g_hSwitcher && GetPropW(g_hSwitcher, SWS_NATIVE_SWIPE_POLICY_PROP)) {
        if (!SetPropW(g_hSwitcher, SWS_NATIVE_TOUCHPAD_EPOCH_PROP,
                      (HANDLE)(ULONG_PTR)s_nativeTouchpadRelayEpoch.load())) {
            RemovePropW(g_hSwitcher, SWS_NATIVE_TOUCHPAD_EPOCH_PROP);
        }
    }
    ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
}

static void ResetRawTwoFingerTap() {
    s_rawTwoFingerTapActive = false;
    s_rawTwoFingerTapMoved = false;
    s_rawTwoFingerTapStartTick = 0;
    s_rawTwoFingerTapOriginX = 0;
    s_rawTwoFingerTapOriginY = 0;
}

static void CancelRawTouchpadStroke() {
    ResetRawTwoFingerTap();
    CancelNativeTouchpadInvocation();
    if (!TouchpadHandlingEnabled() ||
        !g_touchpadReaderAvailable.load()) {
        FinishRawTouchpadShield();
    } else if (s_rawTouchpadShieldActive) {
        // Keep the controller alive until the contacts actually lift, so
        // cancelling the UI cannot release a Windows action on the same
        // physical stroke.
        ScheduleRawTouchpadShieldRelease();
    } else if (s_rawSwipeMarkerActive) {
        // A foreground handoff may have failed, but the shell marker still has
        // to cover the release edge of this physical stroke.
        ScheduleRawTouchpadShieldRelease();
    }
    if (s_rawSessionOwned) {
        g_isTouchpadGestureActive = false;
        if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID);
    }
    s_rawIgnoreUntilLift = s_rawGestureTips != 0;
    s_rawGestureArmed = false;
    s_rawGestureSawThree = false;
    s_rawTapEligible = false;
    s_rawSessionOwned = false;
    s_rawCommandEligible = false;
    if (s_rawGestureTips == 0) {
        s_rawSwipePassedToWindows = false;
        s_rawVerticalActionDir = 0;
    }
}

static bool TouchpadGestureTakeoverWanted();
static bool UpdateTouchpadGestureTakeover(bool want);
static bool RefreshTouchpadGestureKinds();

// Hiding the shell window can synchronously move focus through another app.
// Ignore only callbacks nested inside this recovery, never later user input.
struct ShellFocusRecoveryScope {
    bool previous = g_recoveringShellFocus;
    ShellFocusRecoveryScope() { g_recoveringShellFocus = true; }
    ~ShellFocusRecoveryScope() { g_recoveringShellFocus = previous; }
};
std::map<std::wstring, HICON> g_uwpIconCache;

struct FindCoreWindowData { HWND coreHwnd; };

static BOOL CALLBACK FindCoreWindowProc(HWND hChild, LPARAM lParam) {
    auto* data = (FindCoreWindowData*)lParam;
    WCHAR cls[256];
    GetClassNameW(hChild, cls, 256);
    if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        data->coreHwnd = hChild;
        Wh_Log(L"Explorer IPC: Found CoreWindow %p for UWP app", hChild);
        return FALSE;
    }
    return TRUE;
}

static HICON ResolveIconFromAumid(const WCHAR* aumid, int desiredSizePx) {
    if (IconResolutionCancelled()) return NULL;
    HICON hIcon = NULL;
    IconResolverLog(L"ResolveIconFromAumid: aumid=%s, desiredSizePx=%d", aumid, desiredSizePx);
    
    IShellItem* psi = NULL;
    HRESULT hr = SHCreateItemInKnownFolder(
            FOLDERID_AppsFolder, KF_FLAG_DONT_VERIFY,
            aumid, IID_PPV_ARGS(&psi));
    if (SUCCEEDED(hr) && psi) {
        IconResolverLog(L"ResolveIconFromAumid: SHCreateItemInKnownFolder succeeded");
        IShellItemImageFactory* psiif = NULL;
        hr = IconResolutionCancelled() ? E_ABORT : psi->QueryInterface(IID_PPV_ARGS(&psiif));
        if (SUCCEEDED(hr) && psiif) {
            IconResolverLog(L"ResolveIconFromAumid: QueryInterface(IShellItemImageFactory) succeeded");
            SIZE sz = { desiredSizePx, desiredSizePx };
            HBITMAP hBitmap = NULL;
            hr = IconResolutionCancelled() ? E_ABORT : psiif->GetImage(sz, SIIGBF_RESIZETOFIT | SIIGBF_ICONONLY, &hBitmap);
            if (SUCCEEDED(hr) && hBitmap) {
                IconResolverLog(L"ResolveIconFromAumid: GetImage succeeded");
                HIMAGELIST hImageList = ImageList_Create(sz.cx, sz.cy, ILC_COLOR32, 1, 0);
                if (hImageList) {
                    if (ImageList_Add(hImageList, hBitmap, NULL) != -1) {
                        hIcon = ImageList_GetIcon(hImageList, 0, 0);
                        if (hIcon) IconResolverLog(L"ResolveIconFromAumid: Successfully converted to HICON");
                        else IconResolverLog(L"ResolveIconFromAumid: ImageList_GetIcon failed");
                    } else {
                        IconResolverLog(L"ResolveIconFromAumid: ImageList_Add failed");
                    }
                    ImageList_Destroy(hImageList);
                } else {
                    IconResolverLog(L"ResolveIconFromAumid: ImageList_Create failed");
                }
                DeleteObject(hBitmap);
            } else {
                IconResolverLog(L"ResolveIconFromAumid: GetImage failed, hr=0x%08X", hr);
            }
            psiif->Release();
        } else {
            IconResolverLog(L"ResolveIconFromAumid: QueryInterface failed, hr=0x%08X", hr);
        }
        psi->Release();
    } else {
        IconResolverLog(L"ResolveIconFromAumid: SHCreateItemInKnownFolder failed, hr=0x%08X", hr);
    }
    
    // Fallback: SHParseDisplayName + SHGetFileInfo
    if (!hIcon && !IconResolutionCancelled()) {
        IconResolverLog(L"ResolveIconFromAumid: Falling back to SHParseDisplayName");
        WCHAR appsFolderPath[768];
        if (swprintf_s(appsFolderPath, L"shell:AppsFolder\\%s", aumid) > 0) {
            PIDLIST_ABSOLUTE pidl = NULL;
            hr = SHParseDisplayName(appsFolderPath, NULL, &pidl, 0, NULL);
            if (SUCCEEDED(hr) && pidl) {
                IconResolverLog(L"ResolveIconFromAumid: SHParseDisplayName succeeded");
                SHFILEINFOW sfi = {};
                UINT flags = SHGFI_PIDL | SHGFI_ICON | (desiredSizePx > 24 ? SHGFI_LARGEICON : SHGFI_SMALLICON);
                if (!IconResolutionCancelled() && SHGetFileInfoW((LPCWSTR)pidl, 0, &sfi, sizeof(sfi), flags)) {
                    hIcon = sfi.hIcon;
                    if (hIcon) IconResolverLog(L"ResolveIconFromAumid: SHGetFileInfoW succeeded");
                    else IconResolverLog(L"ResolveIconFromAumid: SHGetFileInfoW returned no icon");
                } else {
                    IconResolverLog(L"ResolveIconFromAumid: SHGetFileInfoW failed");
                }
                CoTaskMemFree(pidl);
            } else {
                IconResolverLog(L"ResolveIconFromAumid: SHParseDisplayName failed, hr=0x%08X", hr);
            }
        }
    }

    return hIcon;
}

// Retries the Explorer-side symbol hook below; declared here because the IPC window (which
// drives the retry timer) is created earlier in the file.
static bool TryHookRaiseDesktop();
static bool TryHookTwinuiAltTab();
static bool TryHookExplorerSuppression();

LRESULT CALLBACK ExplorerIpcWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_TIMER && wParam == SWS_EXPLORER_HOOK_RETRY_TIMER_ID) {
        if (TryHookExplorerSuppression()) {
            KillTimer(hWnd, SWS_EXPLORER_HOOK_RETRY_TIMER_ID);
        }
        return 0;
    }
    if (g_WM_SWS_GET_UWP_ICON && uMsg == g_WM_SWS_GET_UWP_ICON) {
        HWND hWndTarget = (HWND)wParam;
        int desiredSizePx = (int)lParam;
        
        Wh_Log(L"Explorer IPC: Received icon request for HWND %p, size %d", hWndTarget, desiredSizePx);
        
        std::wstring aumid;
        
        {
            IPropertyStore* ps = NULL;
            if (SUCCEEDED(SHGetPropertyStoreForWindow(hWndTarget, IID_PPV_ARGS(&ps))) && ps) {
                PROPVARIANT pv;
                PropVariantInit(&pv);
                if (SUCCEEDED(ps->GetValue(PKEY_AppUserModel_ID, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal && pv.pwszVal[0]) {
                    aumid = pv.pwszVal;
                    Wh_Log(L"Explorer IPC: Got AUMID from PropertyStore = %s", aumid.c_str());
                }
                PropVariantClear(&pv);
                ps->Release();
            }
        }
        
        if (aumid.empty()) {
            Wh_Log(L"Explorer IPC: PropertyStore failed or empty, trying Process Handle fallback");
            FindCoreWindowData data = {0};
            EnumChildWindows(hWndTarget, FindCoreWindowProc, (LPARAM)&data);
            HWND hCore = data.coreHwnd ? data.coreHwnd : hWndTarget;
            
            DWORD pid = 0;
            GetWindowThreadProcessId(hCore, &pid);
            
            if (pid) {
                HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
                if (hProc) {
                    UINT32 aumidLen = 0;
                    LONG rc = GetApplicationUserModelId(hProc, &aumidLen, NULL);
                    if (rc == ERROR_INSUFFICIENT_BUFFER && aumidLen > 0) {
                        WCHAR* buf = new WCHAR[aumidLen];
                        if (GetApplicationUserModelId(hProc, &aumidLen, buf) == ERROR_SUCCESS) {
                            aumid = buf;
                            Wh_Log(L"Explorer IPC: Got AUMID from Process = %s", aumid.c_str());
                        }
                        delete[] buf;
                    } else {
                        Wh_Log(L"Explorer IPC: GetApplicationUserModelId failed, rc=%d", rc);
                    }
                    CloseHandle(hProc);
                } else {
                    Wh_Log(L"Explorer IPC: OpenProcess failed, err=%u", GetLastError());
                }
            }
            // If still no AUMID (common on Win10), try to get the process executable and return its icon
            if (aumid.empty() && pid) {
                // First try window/class icons via WM_GETICON / GetClassLongPtr — sometimes available on Win10
                HICON hWinIcon = NULL;
                DWORD_PTR iconRes = 0;
                if (!hWinIcon && SendMessageTimeoutW(hCore, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &iconRes) && iconRes)
                    hWinIcon = (HICON)iconRes;
                if (!hWinIcon && SendMessageTimeoutW(hCore, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &iconRes) && iconRes)
                    hWinIcon = (HICON)iconRes;
                if (!hWinIcon && SendMessageTimeoutW(hCore, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 500, &iconRes) && iconRes)
                    hWinIcon = (HICON)iconRes;
                if (!hWinIcon) {
                    HICON hClassIcon = (HICON)GetClassLongPtrW(hCore, GCLP_HICON);
                    if (!hClassIcon) hClassIcon = (HICON)GetClassLongPtrW(hCore, GCLP_HICONSM);
                    if (hClassIcon) hWinIcon = hClassIcon;
                }
                if (hWinIcon) {
                    Wh_Log(L"Explorer IPC: Returning WM_GETICON/GetClassLongPtr icon %p as Win10 fallback", hWinIcon);
                    return (LRESULT)hWinIcon;
                }

                WCHAR exePath[MAX_PATH] = {0};
                HANDLE hProc2 = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pid);
                if (hProc2) {
                    DWORD size = MAX_PATH;
                    if (QueryFullProcessImageNameW(hProc2, 0, exePath, &size)) {
                        Wh_Log(L"Explorer IPC: Fallback exe path = %s", exePath);
                        SHFILEINFOW sfi = {};
                        UINT flags = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | (desiredSizePx > 24 ? SHGFI_LARGEICON : SHGFI_SMALLICON);
                        if (SHGetFileInfoW(exePath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), flags)) {
                            if (sfi.hIcon) {
                                Wh_Log(L"Explorer IPC: Returning exe icon %p as Win10 fallback", sfi.hIcon);
                                g_uwpIconCache[std::wstring(exePath) + L"_" + std::to_wstring(desiredSizePx)] = sfi.hIcon;
                                CloseHandle(hProc2);
                                return (LRESULT)sfi.hIcon;
                            }
                        }
                    } else {
                        Wh_Log(L"Explorer IPC: QueryFullProcessImageNameW failed, err=%u", GetLastError());
                    }
                    CloseHandle(hProc2);
                }
            }
        }
        
        if (!aumid.empty()) {
            Wh_Log(L"Explorer IPC: Got AUMID = %s", aumid.c_str());
            
            std::wstring cacheKey = aumid + L"_" + std::to_wstring(desiredSizePx);
            if (g_uwpIconCache.find(cacheKey) != g_uwpIconCache.end()) {
                Wh_Log(L"Explorer IPC: Returning cached icon %p", g_uwpIconCache[cacheKey]);
                return (LRESULT)g_uwpIconCache[cacheKey];
            }
            
            HICON hIcon = ResolveIconFromAumid(aumid.c_str(), desiredSizePx);
            if (hIcon) {
                Wh_Log(L"Explorer IPC: Resolved new icon %p", hIcon);
                g_uwpIconCache[cacheKey] = hIcon;
                return (LRESULT)hIcon;
            } else {
                Wh_Log(L"Explorer IPC: ResolveIconFromAumid failed");
            }
        } else {
            Wh_Log(L"Explorer IPC: Failed to obtain AUMID for UWP app");
        }
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI ExplorerIpcThread(LPVOID) {
    HRESULT hrCo = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = ExplorerIpcWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkSWS_IpcWindow";
    RegisterClassW(&wc);
    
    HWND hIpcWnd = CreateWindowExW(0, L"WindhawkSWS_IpcWindow", L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);

    // Hooking CTray::_RaiseDesktop needs Windhawk's symbols for explorer.exe, which can
    // still be loading when the mod is injected: try once now and let the IPC window's
    // timer retry until it takes (see ExplorerIpcWndProc).
    if (hIpcWnd && !TryHookExplorerSuppression()) {
        SetTimer(hIpcWnd, SWS_EXPLORER_HOOK_RETRY_TIMER_ID, 2000, NULL);
    }

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    if (hIpcWnd) DestroyWindow(hIpcWnd);
    UnregisterClassW(L"WindhawkSWS_IpcWindow", wc.hInstance);

    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

static HICON TryGetUwpIconFromExplorer(HWND hWnd, int desiredSizePx) {
    if (!s_resolvingIcons || IconResolutionCancelled()) return NULL;
    if (!g_WM_SWS_GET_UWP_ICON) {
        g_WM_SWS_GET_UWP_ICON = RegisterWindowMessageW(L"Windhawk_SWS_GetUwpIcon");
    }
    HWND hIpc = FindWindowExW(HWND_MESSAGE, NULL, L"WindhawkSWS_IpcWindow", NULL);
    if (!hIpc) {
        hIpc = FindWindowW(L"WindhawkSWS_IpcWindow", NULL);
    }
    if (hIpc) {
        DWORD_PTR res = 0;
        GetWindowIconTimeout(hIpc, g_WM_SWS_GET_UWP_ICON, (WPARAM)hWnd, desiredSizePx, 1000, &res);
        if (IconResolutionCancelled()) return NULL;
        if (res) {
            // On Windows 11 explorer returns usable icon handles via IPC in our environment.
            if (g_isWin11OrGreater) {
                return (HICON)res;
            }
            std::wstring aumidLocal;
            IPropertyStore* ps = NULL;
            if (SUCCEEDED(SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&ps))) && ps) {
                PROPVARIANT pv; PropVariantInit(&pv);
                if (!IconResolutionCancelled() && SUCCEEDED(ps->GetValue(PKEY_AppUserModel_ID, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal && pv.pwszVal[0]) {
                    aumidLocal = pv.pwszVal;
                }
                PropVariantClear(&pv);
                ps->Release();
            }
            if (!aumidLocal.empty()) {
                std::wstring cacheKey = aumidLocal + L"_" + std::to_wstring(desiredSizePx);
                auto& cache = s_resolvingIcons->uwpIcons;
                auto it = cache.find(cacheKey);
                if (it != cache.end()) return it->second->handle;
                if (IconResolutionCancelled()) return NULL;
                HICON hLocal = ResolveIconFromAumid(aumidLocal.c_str(), desiredSizePx);
                if (hLocal) {
                    return CacheWorkerSourceIcon(cache, cacheKey, hLocal);
                }
            }
            return (HICON)res;
        }
    }

    // Local fallback if Explorer IPC is unavailable
    if (IconResolutionCancelled()) return NULL;
    std::wstring aumidLocal;
    IPropertyStore* ps = NULL;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&ps))) && ps) {
        PROPVARIANT pv; PropVariantInit(&pv);
        if (!IconResolutionCancelled() && SUCCEEDED(ps->GetValue(PKEY_AppUserModel_ID, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal && pv.pwszVal[0]) {
            aumidLocal = pv.pwszVal;
        }
        PropVariantClear(&pv);
        ps->Release();
    }
    if (aumidLocal.empty() && !IconResolutionCancelled()) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid) {
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (hProc) {
                UINT32 aumidLen = 0;
                if (GetApplicationUserModelId(hProc, &aumidLen, NULL) == ERROR_INSUFFICIENT_BUFFER && aumidLen > 0) {
                    std::vector<WCHAR> buf(aumidLen);
                    if (GetApplicationUserModelId(hProc, &aumidLen, buf.data()) == ERROR_SUCCESS) {
                        aumidLocal = buf.data();
                    }
                }
                CloseHandle(hProc);
            }
        }
    }
    if (!aumidLocal.empty()) {
        std::wstring cacheKey = aumidLocal + L"_" + std::to_wstring(desiredSizePx);
        auto& cache = s_resolvingIcons->uwpIcons;
        auto it = cache.find(cacheKey);
        if (it != cache.end()) return it->second->handle;
        if (IconResolutionCancelled()) return NULL;
        HICON hLocal = ResolveIconFromAumid(aumidLocal.c_str(), desiredSizePx);
        if (hLocal) {
            return CacheWorkerSourceIcon(cache, cacheKey, hLocal);
        }
    }
    return NULL;
}

// Identity key used to group windows by application. UWP/app-frame-host windows
// all share a single host process, so keying them by executable would merge
// unrelated apps; those are keyed per-window to avoid over-grouping.
static void GetWindowGroupKey(HWND hWnd, WCHAR* out, size_t cch) {
    out[0] = 0;
    if (g_IsShellFrameWindow && g_IsShellFrameWindow(hWnd)) {
        swprintf_s(out, cch, L"hwnd:%p", (void*)hWnd);
        return;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid) {
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProc) {
            WCHAR exePath[MAX_PATH] = {0};
            DWORD size = MAX_PATH;
            if (QueryFullProcessImageNameW(hProc, 0, exePath, &size)) {
                wcsncpy_s(out, cch, exePath, _TRUNCATE);
            }
            CloseHandle(hProc);
        }
    }
    if (!out[0]) swprintf_s(out, cch, L"hwnd:%p", (void*)hWnd);
}

// Human-readable application name for a window, taken from the executable's
// FileDescription version-info field (e.g. "Google Chrome"), falling back to
// the executable file name without extension.
static void GetAppName(HWND hWnd, WCHAR* out, size_t cch) {
    out[0] = 0;
    if (TryGetCustomAppName(hWnd, out, cch)) return;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return;
    WCHAR exePath[MAX_PATH] = {0};
    DWORD size = MAX_PATH;
    bool gotPath = QueryFullProcessImageNameW(hProc, 0, exePath, &size);
    CloseHandle(hProc);
    if (!gotPath) return;

    DWORD handle = 0;
    DWORD verSize = GetFileVersionInfoSizeW(exePath, &handle);
    if (verSize) {
        std::vector<BYTE> buf(verSize);
        if (GetFileVersionInfoW(exePath, handle, verSize, buf.data())) {
            struct LangAndCodePage { WORD wLanguage; WORD wCodePage; } *translate = nullptr;
            UINT cbTranslate = 0;
            if (VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation",
                               (LPVOID*)&translate, &cbTranslate) &&
                cbTranslate >= sizeof(LangAndCodePage)) {
                WCHAR subBlock[64];
                swprintf_s(subBlock, ARRAYSIZE(subBlock),
                           L"\\StringFileInfo\\%04x%04x\\FileDescription",
                           translate[0].wLanguage, translate[0].wCodePage);
                LPWSTR desc = nullptr;
                UINT descLen = 0;
                if (VerQueryValueW(buf.data(), subBlock, (LPVOID*)&desc, &descLen) &&
                    desc && desc[0]) {
                    wcsncpy_s(out, cch, desc, _TRUNCATE);
                }
            }
        }
    }
    if (!out[0]) {
        wcsncpy_s(out, cch, PathFindFileNameW(exePath), _TRUNCATE);
        PathRemoveExtensionW(out);
    }
}

static void BuildWindowList() {
    for (auto& w : g_windows) {
        for (const auto& kv : w.hThumbs) { if (kv.second) SafeDwmUnregisterThumbnail(kv.second); }
        w.hThumbs.clear();
    }
    g_windows.clear();
    EnumWindows(EnumWindowsProc, (LPARAM)&g_windows);

    // Prune destroyed windows from MRU history
    g_mruWindows.erase(
        std::remove_if(g_mruWindows.begin(), g_mruWindows.end(), [](HWND h) {
            return !IsWindow(h);
        }),
        g_mruWindows.end()
    );

    // Reorder g_windows based on activation MRU history:
    // 1. Windows present in g_mruWindows appear first in their MRU order (most recently activated first).
    // 2. Untracked normal windows follow in their EnumWindows Z-order.
    // 3. Untracked WS_EX_TOPMOST windows appear last, preventing inactive "always on top" windows from hijacking index 0.
    std::stable_sort(g_windows.begin(), g_windows.end(), [](const WindowEntry& a, const WindowEntry& b) {
        return GetMruRank(a.hWnd) < GetMruRank(b.hWnd);
    });

    // App grouping: keep one entry per application. EnumWindows yields windows in
    // Z-order (top to bottom), so the first window seen for each app is its most
    // recently used one, which becomes the representative entry.
    if (g_settings.showApplications && !g_isAltBacktickSameApp) {
        std::vector<WindowEntry> grouped;
        grouped.reserve(g_windows.size());
        std::vector<std::wstring> seenKeys;  // parallel to grouped
        for (auto& w : g_windows) {
            WCHAR key[MAX_PATH];
            GetWindowGroupKey(w.hWnd, key, ARRAYSIZE(key));
            int found = -1;
            for (size_t i = 0; i < seenKeys.size(); i++) {
                if (seenKeys[i] == key) { found = (int)i; break; }
            }
            if (found >= 0) {
                grouped[found].groupWindows.push_back(w.hWnd);
                continue;
            }
            seenKeys.emplace_back(key);
            w.groupWindows.assign(1, w.hWnd);
            grouped.push_back(std::move(w));
        }
        for (auto& e : grouped) {
            if (wcscmp(g_settings.showTitles, L"windowTitle") == 0) continue;
            WCHAR appName[256] = {0};
            GetAppName(e.hWnd, appName, ARRAYSIZE(appName));
            if (!appName[0]) continue;
            // UWP apps share the "Application Frame Host" executable, whose
            // FileDescription is useless as an app name; use the window title instead.
            if (_wcsicmp(appName, L"Application Frame Host") == 0 && e.title[0]) {
                wcscpy_s(appName, e.title);
            }
            if (wcscmp(g_settings.showTitles, L"appName") == 0) {
                wcscpy_s(e.title, appName);
            } else {  // appNameWindowTitle
                if (e.title[0]) {
                    WCHAR combined[256];
                    _snwprintf_s(combined, ARRAYSIZE(combined), _TRUNCATE,
                                 L"%s - %s", appName, e.title);
                    wcscpy_s(e.title, combined);
                } else {
                    wcscpy_s(e.title, appName);
                }
            }
        }
        g_windows = std::move(grouped);
    }
    // Optionally hide minimized windows. When grouping by application, an app
    // entry is only hidden if every one of its windows is minimized; apps with
    // at least one non-minimized window are kept.
    if (g_settings.hideMinimizedWindows) {
        g_windows.erase(std::remove_if(g_windows.begin(), g_windows.end(),
            [](const WindowEntry& w) {
                return IsEntryMinimized(w);
            }), g_windows.end());
    }
    if (g_settings.sortMinimizedWindowsToEnd) {
        std::stable_sort(g_windows.begin(), g_windows.end(), [](const WindowEntry& a, const WindowEntry& b) {
            return IsEntryMinimized(a) < IsEntryMinimized(b);
        });
    }
}

// Layout + Thumbnails

static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEXW* /*lpelfe*/, NEWTEXTMETRICEXW* /*lpntme*/, DWORD /*FontType*/, LPARAM lParam) {
    bool* pFound = (bool*)lParam;
    *pFound = true;
    return 0; // stop enumeration
}

static bool DoesFontExist(LPCWSTR fontName) {
    if (!fontName || !fontName[0]) return false;
    HDC hdc = GetDC(NULL);
    if (!hdc) return false;
    LOGFONTW lf = {};
    lf.lfCharSet = DEFAULT_CHARSET;
    wcsncpy_s(lf.lfFaceName, fontName, _TRUNCATE);
    bool found = false;
    EnumFontFamiliesExW(hdc, &lf, (FONTENUMPROCW)EnumFontFamExProc, (LPARAM)&found, 0);
    ReleaseDC(NULL, hdc);
    return found;
}

static HFONT CreateScaledFont(int dpiY) {
    NONCLIENTMETRICSW ncm = { sizeof(ncm) };
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    typedef BOOL(WINAPI* SPIFD)(UINT, UINT, PVOID, UINT, UINT);
    SPIFD sysParamInfoForDpi = hUser32 ? (SPIFD)GetProcAddress(hUser32, "SystemParametersInfoForDpi") : NULL;
    if (sysParamInfoForDpi) {
        sysParamInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpiY);
    } else {
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
    }
    LOGFONTW lf = ncm.lfMessageFont;
    if (g_settings.fontFamily[0] != L'\0') {
        wcsncpy_s(lf.lfFaceName, g_settings.fontFamily, _TRUNCATE);
    } else {
        // Windows 11: WinUI 3 uses Segoe UI Variable Text for cards and labels in the 9pt-14pt range
        if (IsWin11OrGreater() && DoesFontExist(L"Segoe UI Variable Text")) {
            wcsncpy_s(lf.lfFaceName, L"Segoe UI Variable Text", _TRUNCATE);
        }
        // Windows 10: defaults to ncm.lfMessageFont ("Segoe UI")
    }
    lf.lfHeight = -MulDiv(g_settings.fontSize, dpiY, 72);
    if (wcscmp(g_settings.fontStyle, L"light") == 0) {
        lf.lfWeight = FW_LIGHT;
    } else if (wcscmp(g_settings.fontStyle, L"semibold") == 0) {
        lf.lfWeight = FW_SEMIBOLD;
    } else if (wcscmp(g_settings.fontStyle, L"bold") == 0 || wcscmp(g_settings.fontStyle, L"boldItalic") == 0) {
        lf.lfWeight = FW_BOLD;
    } else {
        lf.lfWeight = FW_NORMAL;
    }
    lf.lfItalic = (wcscmp(g_settings.fontStyle, L"italic") == 0 || wcscmp(g_settings.fontStyle, L"boldItalic") == 0) ? TRUE : FALSE;
    lf.lfQuality = CLEARTYPE_QUALITY;
    return CreateFontIndirectW(&lf);
}

static void UpdateEntrySourceCrop(WindowEntry& w) {
    SIZE src = w.sourceSize;
    if (src.cx <= 0 || src.cy <= 0) {
        w.effectiveSourceSize = { 1, 1 };
        w.rcSourceCrop = { 0, 0, 1, 1 };
        return;
    }

    // Compute invisible frame crop using DWMWA_EXTENDED_FRAME_BOUNDS.
    // This fixes thumbnail displacement for maximized windows where
    // the window extends beyond screen edges to hide the frame.
    // Only apply for actively maximized windows (not minimized).
    // Minimized windows use DWM's low-res cached thumbnail where
    // frame borders are negligible; cropping them causes aspect ratio
    // distortion that makes the thumbnail overflow its destination.
    // Non-maximized windows are natively handled by DWM.
    if (IsZoomed(w.hWnd) && !IsIconic(w.hWnd)) {
        RECT wr = {0}, efb = {0};
        GetWindowRect(w.hWnd, &wr);
        if (SUCCEEDED(DwmGetWindowAttribute(w.hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &efb, sizeof(efb)))) {
            int wrW = wr.right - wr.left, wrH = wr.bottom - wr.top;
            int efbW = efb.right - efb.left, efbH = efb.bottom - efb.top;
            if (wrW > 0 && wrH > 0 && efbW > 0 && efbH > 0) {
                int ml = 0, mt = 0, mr = 0, mb = 0;
                double diffEfb = ((double)src.cx / efbW) - ((double)src.cy / efbH);
                if (diffEfb < 0) diffEfb = -diffEfb;
                double diffWr = ((double)src.cx / wrW) - ((double)src.cy / wrH);
                if (diffWr < 0) diffWr = -diffWr;
                
                if (diffEfb >= diffWr) {
                    double sx = (double)src.cx / wrW;
                    double sy = (double)src.cy / wrH;
                    ml = (int)((efb.left - wr.left) * sx);
                    mt = (int)((efb.top - wr.top) * sy);
                    mr = (int)((wr.right - efb.right) * sx);
                    mb = (int)((wr.bottom - efb.bottom) * sy);
                }
                if (ml < 0) ml = 0;
                if (mt < 0) mt = 0;
                if (mr < 0) mr = 0;
                if (mb < 0) mb = 0;
                w.rcSourceCrop = { ml, mt, src.cx - mr, src.cy - mb };
                w.effectiveSourceSize = { src.cx - ml - mr, src.cy - mt - mb };
                if (w.effectiveSourceSize.cx <= 0 || w.effectiveSourceSize.cy <= 0) {
                    w.effectiveSourceSize = src;
                    w.rcSourceCrop = { 0, 0, src.cx, src.cy };
                }
                return;
            }
        }
    }
    w.effectiveSourceSize = src;
    w.rcSourceCrop = { 0, 0, src.cx, src.cy };
}

// Restore-rect size for a minimized window. GetWindowRect on a minimized window
// returns the tiny "iconic" rect (measured 160x28), not the size the window has
// when restored, which made the derived thumbnail aspect ratio (and the
// max-tile-width clamp in ComputeLayout) shrink those entries. rcNormalPosition
// keeps the restore rect while the window is minimized.
static bool GetWindowRestoreSize(HWND hWnd, SIZE* out) {
    WINDOWPLACEMENT wp = { sizeof(wp) };
    if (GetWindowPlacement(hWnd, &wp)) {
        int w = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
        int h = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
        if (w > 0 && h > 0) {
            out->cx = w;
            out->cy = h;
            return true;
        }
    }
    return false;
}

static bool RefreshEntrySourceSize(WindowEntry& w) {
    if (!g_hSwitcher || !w.hWnd || !IsWindow(w.hWnd)) return false;
    HTHUMBNAIL hT = NULL;
    auto it = w.hThumbs.find(g_hSwitcher);
    if (it != w.hThumbs.end()) {
        hT = it->second;
    }
    SIZE src = {0};
    bool haveSrc = false;
    if (hT) {
        if (SUCCEEDED(DwmQueryThumbnailSourceSize(hT, &src)) && src.cx > 0 && src.cy > 0) {
            haveSrc = true;
        } else {
            SIZE restore = {0};
            if (IsIconic(w.hWnd) && GetWindowRestoreSize(w.hWnd, &restore)) {
                // DWM has no surface for it yet (it can report a placeholder or zero
                // right after registration). Prefer the restore rect over a stale or
                // iconic-sized value, but never shrink a larger DWM-reported size on a
                // transient query failure, which would make the layout oscillate.
                long curArea = (long)w.sourceSize.cx * w.sourceSize.cy;
                if (curArea <= 0 || (long)restore.cx * restore.cy >= curArea) {
                    src = restore;
                    haveSrc = true;
                }
            }
        }
    } else {
        SIZE restore = {0};
        if (IsIconic(w.hWnd) && GetWindowRestoreSize(w.hWnd, &restore)) {
            // No thumbnail yet, and GetWindowRect would give the tiny iconic rect here.
            src = restore;
            haveSrc = true;
        }
    }
    if (!haveSrc) {
        if (hT) return false;  // keep the last known good size
        RECT wr = {0};
        GetWindowRect(w.hWnd, &wr);
        src.cx = wr.right - wr.left;
        src.cy = wr.bottom - wr.top;
        if (src.cx <= 0 || src.cy <= 0) return false;
    }

    bool changed = (abs(w.sourceSize.cx - src.cx) > 1 || abs(w.sourceSize.cy - src.cy) > 1);
    if (changed || w.sourceSize.cx <= 0 || w.sourceSize.cy <= 0) {
        w.sourceSize = src;
        UpdateEntrySourceCrop(w);
        return true;
    }
    return false;
}

static void RegisterThumbnailsEarly() {
    if (!g_settings.showThumbnails || !g_hSwitcher) return;
    for (auto& w : g_windows) {
        if (!w.hThumbs.count(g_hSwitcher)) {
            HTHUMBNAIL hT = NULL;
            if (SUCCEEDED(DwmRegisterThumbnail(g_hSwitcher, w.hWnd, &hT))) {
                w.hThumbs[g_hSwitcher] = hT;
                SIZE src = {0}; DwmQueryThumbnailSourceSize(hT, &src);
                w.sourceSize = src;
            }
        } else {
            SIZE src = {0};
            if (SUCCEEDED(DwmQueryThumbnailSourceSize(w.hThumbs[g_hSwitcher], &src)) && src.cx > 0 && src.cy > 0) {
                w.sourceSize = src;
            }
        }
        for (HWND m : g_hMirrorSwitchers) {
            if (!w.hThumbs.count(m)) {
                HTHUMBNAIL hT = NULL;
                if (SUCCEEDED(DwmRegisterThumbnail(m, w.hWnd, &hT))) w.hThumbs[m] = hT;
            }
        }
        UpdateEntrySourceCrop(w);
    }
}

static void UpdateDockThumbnailDwm() {
    if (g_calculatingLayoutTargets || !DockLayoutActive() || !DockShowPreview() || !g_hSwitcher) return;
    int n = (int)g_windows.size();
    if (g_selectedIndex < 0 || g_selectedIndex >= n) return;

    // 1. Show and configure the selected window's thumbnail FIRST
    auto& selWnd = g_windows[g_selectedIndex];
    if (selWnd.rcThumbActual.right > selWnd.rcThumbActual.left &&
        selWnd.rcThumbActual.bottom > selWnd.rcThumbActual.top) {

        if (!selWnd.hThumbs.count(g_hSwitcher)) {
            HTHUMBNAIL hT = NULL;
            if (SUCCEEDED(DwmRegisterThumbnail(g_hSwitcher, selWnd.hWnd, &hT))) {
                selWnd.hThumbs[g_hSwitcher] = hT;
                if (selWnd.sourceSize.cx <= 0 || selWnd.sourceSize.cy <= 0) {
                    SIZE src = {0}; DwmQueryThumbnailSourceSize(hT, &src);
                    selWnd.sourceSize = src;
                }
            }
        }

        for (HWND m : g_hMirrorSwitchers) {
            if (!selWnd.hThumbs.count(m)) {
                HTHUMBNAIL hT = NULL;
                if (SUCCEEDED(DwmRegisterThumbnail(m, selWnd.hWnd, &hT))) {
                    selWnd.hThumbs[m] = hT;
                }
            }
        }

        BYTE alpha = CurrentPresentationAlphaByte();
        if (g_layoutTransition.active && selWnd.isNewEntry) {
            alpha = (BYTE)roundf(alpha * selWnd.enterAlpha);
        }

        RECT curDst = selWnd.rcThumbActual;
        if (g_dockPreviewSlide.active) {
            int offX = (int)roundf(g_dockPreviewSlide.currentOffset);
            curDst.left += offX;
            curDst.right += offX;
            alpha = (BYTE)roundf((float)alpha * g_dockPreviewSlide.currentAlpha);
        }

        for (const auto& kv : selWnd.hThumbs) {
            HTHUMBNAIL hThumb = kv.second;
            if (!hThumb) continue;
            RECT client = {};
            GetClientRect(g_hSwitcher, &client);
            SubmitAnimatedThumbnail(hThumb, selWnd.hWnd, curDst, alpha,
                                    selWnd.sourceSize, selWnd.rcSourceCrop, client);
        }
    }

    // 2. Hide all non-selected windows' thumbnails
    for (int i = 0; i < n; i++) {
        if (i == g_selectedIndex && HasLayoutRect(selWnd.rcThumbActual)) continue;
        for (const auto& kv : g_windows[i].hThumbs) {
            if (kv.second) {
                SubmitAnimatedThumbnail(kv.second, g_windows[i].hWnd, {}, 0,
                                        g_windows[i].sourceSize,
                                        g_windows[i].rcSourceCrop, {});
            }
        }
    }
}

static void UpdateDockPreviewForSelection() {
    if (!DockLayoutActive() || !DockShowPreview()) return;
    int n = (int)g_windows.size();
    if (g_selectedIndex < 0 || g_selectedIndex >= n) return;

    for (int i = 0; i < n; i++) {
        if (i == g_selectedIndex) {
            auto& selWnd = g_windows[i];
            int slotW = g_rcCentralPreviewSlot.right - g_rcCentralPreviewSlot.left;
            int slotH = g_rcCentralPreviewSlot.bottom - g_rcCentralPreviewSlot.top;
            int maxH = slotH - DpiScale(8, g_dpiY);
            if (maxH <= 0) maxH = DpiScale(g_settings.dockPreviewHeight > 0 ? g_settings.dockPreviewHeight : 280, g_dpiY);
            int prevW = maxH;
            int prevH = maxH;
            if (selWnd.effectiveSourceSize.cx > 0 && selWnd.effectiveSourceSize.cy > 0) {
                prevW = (int)((double)selWnd.effectiveSourceSize.cx * maxH / selWnd.effectiveSourceSize.cy);
                int maxPrevW = slotW - DpiScale(24, g_dpiX);
                if (maxPrevW > 0 && prevW > maxPrevW) {
                    prevH = (int)((double)maxPrevW * prevH / prevW);
                    prevW = maxPrevW;
                }
            }
            int px = g_rcCentralPreviewSlot.left + (slotW - prevW) / 2;
            int py = g_rcCentralPreviewSlot.top + (slotH - prevH) / 2;
            g_rcCentralPreview = { px, py, px + prevW, py + prevH };
            selWnd.rcThumbActual = g_rcCentralPreview;
            selWnd.rcThumbSlot = g_rcCentralPreviewSlot;
        } else {
            g_windows[i].rcThumbActual = { 0, 0, 0, 0 };
            g_windows[i].rcThumbSlot = { 0, 0, 0, 0 };
        }
    }
    UpdateDockThumbnailDwm();
}

static void ComputeDockLayout(HMONITOR hMon, const MONITORINFO& mi, UINT dpiX, UINT dpiY) {
    int monW = mi.rcWork.right - mi.rcWork.left;
    int monH = mi.rcWork.bottom - mi.rcWork.top;
    int maxW = monW * g_settings.maxWidthPercent / 100;
    int maxH = monH * g_settings.maxHeightPercent / 100;

    int masterPadX = DpiScale(g_settings.dockSwitcherPadding, dpiX);
    int masterPadY = DpiScale(g_settings.dockSwitcherPadding, dpiY);

    int n = (int)g_windows.size();
    if (n == 0) {
        g_winW = 0;
        g_winH = 0;
        return;
    }

    if (g_selectedIndex < 0 || g_selectedIndex >= n) {
        g_selectedIndex = 0;
    }

    int iconSz = DpiScale(g_settings.dockIconSize > 0 ? g_settings.dockIconSize : 48, dpiX);
    int cellPad = DpiScale(8, dpiX);
    int cellW = iconSz + cellPad * 2;
    int cellH = iconSz + cellPad * 2;
    if ((cellW - iconSz) % 2 != 0) { cellW++; }
    if ((cellH - iconSz) % 2 != 0) { cellH++; }
    int spacing = DpiScale(g_settings.dockIconSpacing, dpiX);

    // A percentage smaller than one usable icon must not clip that icon. Keep
    // configured padding and controls, reserving overflow edges before fitting.
    int minimumStripW = cellW + ((g_settings.showOverflowIndicator && n > 1) ? DpiScale(52, dpiX) : 0);
    if (g_settings.showTitle) minimumStripW = std::max(minimumStripW, DpiScale(80, dpiX));
    maxW = std::max(maxW, 2 * masterPadX + minimumStripW);

    int availStripW = maxW - 2 * masterPadX;
    int fitCount = 0;
    int testW = 0;
    for (int idx = 0; idx < n; idx++) {
        int needed = (idx == 0) ? cellW : (cellW + spacing);
        if (testW + needed <= availStripW) {
            testW += needed;
            fitCount++;
        } else {
            break;
        }
    }
    if (fitCount < 1) fitCount = 1;
    int maxLimit = g_settings.dockMaxVisibleIcons > 0 ? g_settings.dockMaxVisibleIcons : fitCount;
    int visibleCount = std::min(n, std::min(fitCount, maxLimit));
    int chevReserve = (g_settings.showOverflowIndicator && n > visibleCount) ? DpiScale(26, dpiX) : 0;
    if (chevReserve) {
        int available = std::max(0, availStripW - 2 * chevReserve);
        fitCount = std::max(1, (available + spacing) / std::max(1, cellW + spacing));
        visibleCount = std::min(n, std::min(fitCount, maxLimit));
    }
    int totalIconsW = visibleCount * cellW + (visibleCount - 1) * spacing;
    int totalStripW = totalIconsW + 2 * chevReserve;

    bool showPreview = DockShowPreview();
    int prevW = 0;
    int prevH = 0;
    int previewSlotH = 0;

    int titleH = g_settings.showTitle ? GetHeaderRowHeightPx() : 0;
    if (g_settings.showTitle && titleH < DpiScale(20, dpiY)) titleH = DpiScale(20, dpiY);

    if (showPreview) {
        int maxPrevH = DpiScale(g_settings.dockPreviewHeight > 0 ? g_settings.dockPreviewHeight : 280, dpiY);
        // Match the actual top/bottom strip, preview-slot and title spacings.
        // Only the preview is shrinkable; do not clip icons, title or padding.
        int reservedH = 2 * masterPadY + cellH + titleH +
                        DpiScale(12, dpiY) + 1 + DpiScale(8, dpiY) +
                        DpiScale(12, dpiY) +
                        (g_settings.showTitle ? DpiScale(12, dpiY) : 0);
        int minimumPreviewH = DpiScale(16, dpiY);
        maxH = std::max(maxH, reservedH + minimumPreviewH);
        maxPrevH = std::clamp(maxPrevH, minimumPreviewH, maxH - reservedH);

        prevW = maxPrevH;
        prevH = maxPrevH;
        if (g_selectedIndex >= 0 && g_selectedIndex < n) {
            const auto& selWnd = g_windows[g_selectedIndex];
            if (selWnd.effectiveSourceSize.cx > 0 && selWnd.effectiveSourceSize.cy > 0) {
                int maxPrevW = std::max(1, maxW - 2 * masterPadX - DpiScale(24, dpiX));
                double aspect = (double)selWnd.effectiveSourceSize.cx / selWnd.effectiveSourceSize.cy;
                prevW = std::max(1, (int)std::min((double)maxPrevW, maxPrevH * aspect));
                prevH = std::max(1, (int)std::min((double)maxPrevH, maxPrevW / aspect));
            }
        }
        previewSlotH = prevH + DpiScale(12, dpiY);
    }

    int contentW = std::max(totalStripW, prevW + DpiScale(32, dpiX));
    int minW = DpiScale(380, dpiX);
    if (contentW < minW) contentW = minW;
    if (contentW > maxW - 2 * masterPadX) contentW = maxW - 2 * masterPadX;
    g_winW = contentW + 2 * masterPadX;

    int curY = 0;
    if (DockIconIsTop()) {
        curY = masterPadY;
        // 1. Icon Strip (11px from window top)
        g_rcDockIconStrip = { masterPadX, curY, g_winW - masterPadX, curY + cellH };
        curY += cellH;

        if (showPreview) {
            // Divider between icons and preview: 12px from icons, 1px divider, 8px to preview
            curY += DpiScale(12, dpiY) + 1 + DpiScale(8, dpiY);
            // 2. Central Preview
            g_rcCentralPreviewSlot = { masterPadX, curY, g_winW - masterPadX, curY + previewSlotH };
            int px = masterPadX + (g_winW - 2 * masterPadX - prevW) / 2;
            int py = curY + (previewSlotH - prevH) / 2;
            g_rcCentralPreview = { px, py, px + prevW, py + prevH };
            curY += previewSlotH;

            // 3. Title Bar
            if (g_settings.showTitle) {
                curY += DpiScale(8, dpiY);
                g_rcDockTitleBar = { masterPadX, curY, g_winW - masterPadX, curY + titleH };
                curY += titleH + masterPadY + DpiScale(4, dpiY);
            } else {
                g_rcDockTitleBar = { 0, 0, 0, 0 };
                curY += masterPadY;
            }
        } else if (g_settings.showTitle) {
            g_rcCentralPreviewSlot = { 0, 0, 0, 0 };
            g_rcCentralPreview = { 0, 0, 0, 0 };
            // Divider between icons and title: 12px from icons, 1px divider, 15px to title text
            curY += DpiScale(12, dpiY) + 1 + DpiScale(15, dpiY);
            // 3. Title Bar (15px from divider, 15px to window bottom)
            g_rcDockTitleBar = { masterPadX, curY, g_winW - masterPadX, curY + titleH };
            curY += titleH + masterPadY + DpiScale(4, dpiY);
        } else {
            g_rcCentralPreviewSlot = { 0, 0, 0, 0 };
            g_rcCentralPreview = { 0, 0, 0, 0 };
            g_rcDockTitleBar = { 0, 0, 0, 0 };
            curY += masterPadY;
        }
        g_winH = curY;
    } else {
        if (g_settings.showTitle) {
            curY = masterPadY + DpiScale(4, dpiY);
            // 1. Title Bar (15px from window top)
            g_rcDockTitleBar = { masterPadX, curY, g_winW - masterPadX, curY + titleH };
            curY += titleH;
        } else {
            g_rcDockTitleBar = { 0, 0, 0, 0 };
            curY = masterPadY;
        }

        if (showPreview) {
            if (g_settings.showTitle) {
                curY += DpiScale(8, dpiY);
            }
            // 2. Central Preview
            g_rcCentralPreviewSlot = { masterPadX, curY, g_winW - masterPadX, curY + previewSlotH };
            int px = masterPadX + (g_winW - 2 * masterPadX - prevW) / 2;
            int py = curY + (previewSlotH - prevH) / 2;
            g_rcCentralPreview = { px, py, px + prevW, py + prevH };
            curY += previewSlotH;

            // Divider between preview and icons: 8px from preview, 1px divider, 12px to icons
            curY += DpiScale(8, dpiY) + 1 + DpiScale(12, dpiY);
        } else if (g_settings.showTitle) {
            g_rcCentralPreviewSlot = { 0, 0, 0, 0 };
            g_rcCentralPreview = { 0, 0, 0, 0 };
            // Divider between title and icons: 15px from title text, 1px divider, 12px to icons
            curY += DpiScale(15, dpiY) + 1 + DpiScale(12, dpiY);
        } else {
            g_rcCentralPreviewSlot = { 0, 0, 0, 0 };
            g_rcCentralPreview = { 0, 0, 0, 0 };
        }

        // 3. Icon Strip (11px to window bottom)
        g_rcDockIconStrip = { masterPadX, curY, g_winW - masterPadX, curY + cellH };
        curY += cellH + masterPadY;
        g_winH = curY;
    }

    if (g_layoutStartIndex > n - visibleCount) {
        g_layoutStartIndex = std::max(0, n - visibleCount);
    }
    if (g_layoutStartIndex < 0) g_layoutStartIndex = 0;

    int stripStartX = masterPadX + chevReserve + (g_winW - 2 * masterPadX - 2 * chevReserve - totalIconsW) / 2;
    for (int i = 0; i < n; i++) {
        auto& w = g_windows[i];
        if (i >= g_layoutStartIndex && i < g_layoutStartIndex + visibleCount) {
            int idx = i - g_layoutStartIndex;
            int cellX = stripStartX + idx * (cellW + spacing);
            int cellY = g_rcDockIconStrip.top;
            w.rcCell = { cellX, cellY, cellX + cellW, cellY + cellH };
            if (showPreview && i == g_selectedIndex) {
                w.rcThumbActual = g_rcCentralPreview;
                w.rcThumbSlot = g_rcCentralPreviewSlot;
            } else {
                w.rcThumbActual = { 0, 0, 0, 0 };
                w.rcThumbSlot = { 0, 0, 0, 0 };
            }
        } else {
            w.rcCell = { 0, 0, 0, 0 };
            w.rcThumbActual = { 0, 0, 0, 0 };
            w.rcThumbSlot = { 0, 0, 0, 0 };
        }
    }
}

static void ComputeLayout(HMONITOR hMon) {
    MONITORINFO mi = { sizeof(mi) }; GetMonitorInfoW(hMon, &mi);
    int monW = mi.rcWork.right - mi.rcWork.left, monH = mi.rcWork.bottom - mi.rcWork.top;
    UINT dpiX = 96, dpiY = 96;
    QueryMonitorDpi(hMon, &dpiX, &dpiY);
    g_dpiX = dpiX; g_dpiY = dpiY;

    g_settings.switcherPadding = GetActiveSwitcherPadding();
    int n = (int)g_windows.size();
    if (n == 0) { g_winW = 0; g_winH = 0; return; }

    if (DockLayoutActive()) {
        ComputeDockLayout(hMon, mi, dpiX, dpiY);
        return;
    }

    // "Shrink tasks to fit": pick a discrete scale from the task count. Must be
    // set before any GetHeaderIconSizePx()/GetHeaderRowHeightPx() call below so
    // icon and row sizes pick it up.
    g_autoFitScalePct = ComputeAutoFitScalePct(n);

    // DPI-scale all padding constants
    int masterPad    = DpiScale(g_settings.switcherPadding, dpiX);
    int elemPadTop   = DpiScale(SWS_ELEMENT_PAD_TOP, dpiY);
    int elemPadBot   = DpiScale(SWS_ELEMENT_PAD_BOTTOM, dpiY);
    int elemPadLeft  = DpiScale(SWS_ELEMENT_PAD_LEFT, dpiX);
    int elemPadRight = DpiScale(SWS_ELEMENT_PAD_RIGHT, dpiX);
    int padTop       = DpiScale(g_settings.entryPadding, dpiY);
    int padBot       = DpiScale(g_settings.entryPadding, dpiY);
    int padLeft      = DpiScale(g_settings.entryPadding, dpiX);
    int padRight     = DpiScale(g_settings.entryPadding, dpiX);
    int padDivider   = DpiScale(SWS_PAD_DIVIDER, dpiY);
    int rowTitleH    = GetHeaderRowHeightPx();

    // Badge layout override: the icon overlays the thumbnail, so the header
    // row only needs to account for the title text, not the icon.  Also force
    // the thumbnail position so the title band sits above or below.
    if (BadgeLayoutActive()) {
        rowTitleH = g_settings.showTitle ? GetHeaderTitleHeightPx() : 0;
    }

    // EP: cbThumbnailAvailableHeight = cbRowHeight - cbRowTitleHeight - cbTopPadding - 2 * cbBottomPadding
    // All values are DPI-scaled at this point (matching EP lines 826-844)
    // A row taller than the work area will be fitted below; avoid overflowing
    // DPI/auto-fit arithmetic for an arbitrarily large integer setting.
    int scaledRowH = DpiScale(std::min(g_settings.rowHeight, std::max(230, monH)), dpiY);
    if (g_autoFitScalePct != 100) {
        scaledRowH = ScaleAutoFit(scaledRowH);
        int floorH = DpiScale(SWS_AUTOFIT_MIN_ROWHEIGHT, dpiY);
        if (scaledRowH < floorH) scaledRowH = floorH;
    }
    bool sidePlacement = ThumbnailIsSide() && g_settings.showThumbnails;
    if (BadgeLayoutActive()) sidePlacement = false;  // badge mode never uses side placement
    int thumbH = 0;
    if (g_settings.showThumbnails) {
        thumbH = scaledRowH - (sidePlacement ? 0 : rowTitleH) - padTop - 2 * padBot;
        if (thumbH < 0) thumbH = 0;
    }
    // EP: cbMaxTileWidth = cbRowHeight * MAX_TILE_WIDTH (computed before DPI, then scaled)
    int maxTileW = (int)std::min((double)std::max(1, monW),
        g_settings.rowHeight * SWS_MAX_TILE_ASPECT * dpiX / 96.0 * g_autoFitScalePct / 100.0);

    // EP helper equivalents:
    // initialLeft  = elemPadLeft + padLeft
    // rightInc     = (padRight + elemPadRight) + initialLeft
    // initialTop   = elemPadTop + (padTop + rowTitleH + padDivider)
    // bottomInc    = (thumbH + padBot) + elemPadBot + initialTop
    int initialLeft = elemPadLeft + padLeft;
    int rightInc    = (padRight + elemPadRight) + initialLeft;
    bool showThumbs = g_settings.showThumbnails;
    bool thumbBottom = showThumbs ? ThumbnailIsBottom() : true;
    bool thumbTop = showThumbs ? ThumbnailIsTop() : false;
    bool thumbSide = showThumbs ? ThumbnailIsSide() : false;

    // Badge layout: override thumbnail position semantics.
    // "title on top" → title band above, thumbnail below → thumbBottom = true
    // "title on bottom" → title band below, thumbnail above → thumbTop = true
    int activePadDivider = (showThumbs && rowTitleH > 0) ? padDivider : 0;
    
    if (BadgeLayoutActive()) {
        thumbSide = false;
        thumbBottom = BadgeTitleIsTop();
        thumbTop = !BadgeTitleIsTop();

        // Calculate icon overlap due to offsets to dynamically resize borders & push title away
        bool isTop = BadgeIconPositionIs(L"topLeft") || BadgeIconPositionIs(L"topCenter") || BadgeIconPositionIs(L"topRight");
        bool isBottom = BadgeIconPositionIs(L"bottomLeft") || BadgeIconPositionIs(L"bottomCenter") || BadgeIconPositionIs(L"bottomRight");
        bool isLeft = BadgeIconPositionIs(L"topLeft") || BadgeIconPositionIs(L"centerLeft") || BadgeIconPositionIs(L"bottomLeft");
        bool isRight = BadgeIconPositionIs(L"topRight") || BadgeIconPositionIs(L"centerRight") || BadgeIconPositionIs(L"bottomRight");

        int shadowPad = g_settings.showBadgeIconBackgroundShadow ? DpiScale(6, dpiY) : 0;
        int shadowPadX = g_settings.showBadgeIconBackgroundShadow ? DpiScale(6, dpiX) : 0;

        int bIconOffY = DpiScale(g_settings.badgeIconOffsetY, dpiY) + shadowPad;
        int bIconOffX = DpiScale(g_settings.badgeIconOffsetX, dpiX) + shadowPadX;
        
        int bIconOffYNeg = DpiScale(g_settings.badgeIconOffsetY, dpiY) - shadowPad;
        int bIconOffXNeg = DpiScale(g_settings.badgeIconOffsetX, dpiX) - shadowPadX;
        
        int minGapY = DpiScale(8, dpiY);
        int minGapX = DpiScale(8, dpiX);
        
        if (bIconOffY > 0 && isBottom && thumbTop) {
            int extra = bIconOffY + minGapY - activePadDivider;
            if (extra > 0) activePadDivider += extra;
        } else if (bIconOffY > 0 && isBottom && !thumbTop) {
            int extra = bIconOffY + minGapY - padBot;
            if (extra > 0) padBot += extra;
        }
        
        if (bIconOffYNeg < 0 && isTop && thumbBottom) {
            int extra = abs(bIconOffYNeg) + minGapY - activePadDivider;
            if (extra > 0) activePadDivider += extra;
        } else if (bIconOffYNeg < 0 && isTop && !thumbBottom) {
            int extra = abs(bIconOffYNeg) + minGapY - padTop;
            if (extra > 0) padTop += extra;
        }

        if (bIconOffX > 0 && isRight) {
            int extra = bIconOffX + minGapX - padRight;
            if (extra > 0) padRight += extra;
        }
        if (bIconOffXNeg < 0 && isLeft) {
            int extra = abs(bIconOffXNeg) + minGapX - padLeft;
            if (extra > 0) padLeft += extra;
        }
    }
    
    g_activePadDivider = activePadDivider;
    
    int headerAndDividerH = rowTitleH + activePadDivider;
    int maxW = monW * g_settings.maxWidthPercent / 100;
    int maxH = monH * g_settings.maxHeightPercent / 100;
    int minimumThumbH = showThumbs ? DpiScale(16, dpiY) : 0;
    if (BadgeLayoutActive() && g_settings.showIcon) minimumThumbH = std::max(minimumThumbH, GetHeaderIconSizePx());
    if (showThumbs && rowTitleH == 0 && g_settings.showCloseButton) minimumThumbH = std::max(minimumThumbH, DpiScale(32, dpiY));
    int fixedHeight = 2 * masterPad + elemPadTop + elemPadBot + padTop + padBot +
                      (thumbSide ? 0 : headerAndDividerH);
    int minimumBodyH = thumbSide ? std::max(rowTitleH, minimumThumbH) : minimumThumbH;
    // Header, controls and chosen padding are not shrinkable. If the percentage
    // is below their minimum, use that minimum and fit the preview into it.
    maxH = std::max(maxH, fixedHeight + minimumBodyH);
    if (showThumbs) thumbH = std::clamp(thumbH, minimumThumbH, maxH - fixedHeight);
    int thumbTopOffset = padTop + (thumbBottom ? headerAndDividerH : 0);
    int initialTop  = elemPadTop + thumbTopOffset;
    int baseContentH = thumbSide ? std::max(thumbH, rowTitleH) : thumbH;
    int bottomInc   = (baseContentH + padBot) + elemPadBot + initialTop + (thumbTop ? headerAndDividerH : 0);
    int sideHeaderWidth = DpiScale(HeaderIsVertical() ? 96 : 150, dpiX);

    int minimumHeaderW = (!BadgeLayoutActive() && g_settings.showIcon) ? GetHeaderIconSizePx() : 0;
    if (g_settings.showTitle) {
        if (minimumHeaderW && !HeaderIsVertical()) minimumHeaderW += padLeft;
        if (HeaderIsVertical()) minimumHeaderW = std::max(minimumHeaderW, DpiScale(48, dpiX));
        else minimumHeaderW += DpiScale(48, dpiX);
        if (!BadgeLayoutActive() && !HeaderIsVertical() && g_settings.showCloseButton) minimumHeaderW += DpiScale(28, dpiX);
    } else if (!showThumbs && g_settings.showIcon) {
        minimumHeaderW += DpiScale(g_settings.centerTaskContent ? 16 : 20, dpiX);
    }
    sideHeaderWidth = std::max(sideHeaderWidth, minimumHeaderW);
    int minimumThumbW = showThumbs ? DpiScale(16, dpiX) : 0;
    if (BadgeLayoutActive() && g_settings.showIcon) minimumThumbW = std::max(minimumThumbW, GetHeaderIconSizePx());
    int minimumContentW = thumbSide
        ? minimumThumbW + (rowTitleH > 0 ? sideHeaderWidth + padDivider : 0)
        : std::max(minimumThumbW, minimumHeaderW);
    maxW = std::max(maxW, 2 * masterPad + rightInc + minimumContentW);
    maxTileW = std::min(std::max(1, maxTileW), maxW - 2 * masterPad - rightInc);

    auto fitTaskWidth = [&](int& width, int& thumbWidth, int& actualThumbH) {
        width = std::clamp(width, minimumContentW, maxW - 2 * masterPad - rightInc);
        int availableThumbW = width - (thumbSide && rowTitleH > 0 ? sideHeaderWidth + padDivider : 0);
        if (showThumbs && StretchThumbsToTaskWidth() && !thumbSide && g_settings.rowWidth > 0) {
            thumbWidth = width;
            actualThumbH = thumbH;
        } else if (showThumbs && thumbWidth > availableThumbW) {
            actualThumbH = std::max(1, (int)((double)availableThumbW * actualThumbH / thumbWidth));
            thumbWidth = availableThumbW;
        }
        if (showThumbs) {
            thumbWidth = std::clamp(thumbWidth, 1, availableThumbW);
            actualThumbH = std::max(1, actualThumbH);
        }
    };

    int sideThumbSlotW = 0;
    if (g_settings.showThumbnails && sidePlacement && thumbH > 0) {
        for (const auto& w : g_windows) {
            int slotW = thumbH;
            if (w.effectiveSourceSize.cx > 0 && w.effectiveSourceSize.cy > 0) {
                slotW = std::max(1, (int)std::min((double)INT_MAX,
                    (double)w.effectiveSourceSize.cx * thumbH / w.effectiveSourceSize.cy));
            }
            if (slotW > maxTileW) slotW = maxTileW;
            if (w.effectiveSourceSize.cx > 0 && slotW > w.effectiveSourceSize.cx) slotW = w.effectiveSourceSize.cx;
            if (slotW > sideThumbSlotW) sideThumbSlotW = slotW;
        }
        if (sideThumbSlotW <= 0) {
            sideThumbSlotW = DpiScale(16, dpiX);
        }
    }

    int curX = initialLeft + masterPad;
    int curY = initialTop + masterPad;
    int placedCount = n; // Track how many windows were actually placed

    auto truncateRemaining = [&](int startIdx) {
        for (int jj = startIdx; jj < n; jj++) {
            int ji = (g_layoutStartIndex + jj) % n;
            g_windows[ji].rcCell = {0, 0, 0, 0};
            g_windows[ji].rcThumbActual = {0, 0, 0, 0};
            g_windows[ji].rcThumbSlot = {0, 0, 0, 0};
            // During dry-run layout passes (e.g. CyclePage page-map discovery),
            // we do NOT destroy DWM thumbnail handles. Doing so on every scroll
            // event triggers rapid DwmUnregister/Register cycles that cause
            // visible flicker in the DWM compositor. The caller is responsible
            // for proper cleanup when g_isDryRunLayout is false.
            if (!g_isDryRunLayout) {
                bool keep = false;
                if (g_scrollTransition.preservingThumbnails) {
                    for (const auto& snap : g_scrollTransition.outgoingItems) {
                        if (snap.hWnd == g_windows[ji].hWnd) { keep = true; break; }
                    }
                }
                if (!keep) {
                    for (const auto& kv : g_windows[ji].hThumbs) {
                        if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
                    }
                    g_windows[ji].hThumbs.clear();
                }
            }
        }
        placedCount = startIdx;
    };

    if (!LayoutIsVertical()) {
        int maxRowW = 0;

        for (int idx = 0; idx < n; idx++) {
            int i = (g_layoutStartIndex + idx) % n;
            auto& w = g_windows[i];

            if (g_layoutStartIndex > 0 && idx > 0 && i < g_layoutStartIndex) {
                if (g_isPaginatedView) {
                    truncateRemaining(idx);
                    break;
                }
                if (((g_layoutStartIndex + idx - 1) % n) >= g_layoutStartIndex
                    && curX > initialLeft + masterPad) {
                    if (curX - initialLeft > maxRowW) maxRowW = curX - initialLeft;
                    curX = initialLeft + masterPad;
                    if (curY + 2 * bottomInc - initialTop > maxH - masterPad) {
                        truncateRemaining(idx);
                        break;
                    }
                    curY = curY + bottomInc;
                }
            }

            int width = 0;
            int thumbWidth = 0;
            int actualThumbH = thumbH;

            if (g_settings.showThumbnails && thumbH > 0) {
                if (w.effectiveSourceSize.cx > 0 && w.effectiveSourceSize.cy > 0) {
                    thumbWidth = std::max(1, (int)std::min((double)INT_MAX,
                        (double)w.effectiveSourceSize.cx * thumbH / w.effectiveSourceSize.cy));
                } else {
                    thumbWidth = thumbH;
                }

                int naturalThumbWidth = thumbWidth;
                if (thumbWidth > maxTileW) thumbWidth = maxTileW;
                if (w.effectiveSourceSize.cx > 0 && thumbWidth > w.effectiveSourceSize.cx) thumbWidth = w.effectiveSourceSize.cx;
                if (naturalThumbWidth > 0 && thumbWidth != naturalThumbWidth) {
                    actualThumbH = std::max(1, (int)((double)thumbWidth * thumbH / naturalThumbWidth));
                }

                width = thumbWidth;
                if (g_settings.rowWidth > 0) {
                    width = ScaleAutoFit(DpiScale(g_settings.rowWidth, dpiX));
                    if (StretchThumbsToTaskWidth() && !sidePlacement) {
                        thumbWidth = sidePlacement ? std::max(0, width - ((rowTitleH > 0) ? (sideHeaderWidth + padDivider) : 0)) : width;
                        if (thumbWidth <= 0) thumbWidth = DpiScale(16, dpiX);
                        actualThumbH = thumbH;
                    } else if (thumbWidth > width && width > 0) {
                        actualThumbH = (int)((double)width * actualThumbH / thumbWidth);
                        thumbWidth = width;
                    }
                }

                if (sidePlacement) {
                    int headerExtra = (rowTitleH > 0) ? (sideHeaderWidth + padDivider) : 0;
                    if (g_settings.rowWidth > 0) {
                        int maxThumbW = width - headerExtra;
                        if (maxThumbW > 0 && thumbWidth > maxThumbW) {
                            actualThumbH = (int)((double)maxThumbW * actualThumbH / thumbWidth);
                            thumbWidth = maxThumbW;
                        }
                    } else {
                        width = thumbWidth + headerExtra;
                    }
                }
            } else {
                if (!g_settings.showTitle && g_settings.showIcon) {
                    if (g_settings.centerTaskContent) {
                        width = GetHeaderIconSizePx() + DpiScale(16, dpiX); // Base padding
                    } else {
                        width = GetHeaderIconSizePx() + DpiScale(20, dpiX); // Icon + btnSz(16) + gap(4)
                    }
                } else {
                    width = DpiScale(160, dpiX);
                }
                thumbWidth = width;
                actualThumbH = 0;
            }

            if (!g_settings.showThumbnails && g_settings.rowWidth > 0) {
                width = ScaleAutoFit(DpiScale(g_settings.rowWidth, dpiX));
                thumbWidth = width;
            }

            fitTaskWidth(width, thumbWidth, actualThumbH);

            if (curX + width + rightInc - initialLeft > maxW - masterPad && curX > initialLeft + masterPad) {
                if (curX - initialLeft > maxRowW) maxRowW = curX - initialLeft;
                curX = initialLeft + masterPad;

                if (curY + 2 * bottomInc - initialTop > maxH - masterPad) {
                    truncateRemaining(idx);
                    break;
                }

                curY = curY + bottomInc;
            }

            w.rcCell.left   = curX - initialLeft + elemPadLeft;
            w.rcCell.top    = curY - initialTop + elemPadTop;
            w.rcCell.right  = curX + width + rightInc - initialLeft - elemPadRight;
            w.rcCell.bottom = curY + bottomInc - initialTop - elemPadBot;
            if (g_settings.showThumbnails) {
                if (sidePlacement) {
                    int contentH = std::max(actualThumbH, minimumBodyH);
                    int baseH = std::max(thumbH, rowTitleH);
                    if (contentH < baseH) {
                        w.rcCell.bottom -= (baseH - contentH);
                    }
                } else if (actualThumbH < thumbH) {
                    w.rcCell.bottom -= (thumbH - std::max(actualThumbH, minimumThumbH));
                }
            } else if (!g_settings.showTitle && g_settings.showIcon) {
                int iconSz = GetHeaderIconSizePx();
                int curCellW = w.rcCell.right - w.rcCell.left;
                if ((curCellW - iconSz) % 2 != 0) w.rcCell.right++;
                int curCellH = w.rcCell.bottom - w.rcCell.top;
                if ((curCellH - iconSz) % 2 != 0) w.rcCell.bottom++;
            }

            if (g_settings.showThumbnails) {
                int thumbX = curX;
                int thumbY = curY;
                int slotX = curX;
                int slotW = width;
                if (sidePlacement) {
                    int contentH = std::max(actualThumbH, rowTitleH);
                    int headerExtra = (rowTitleH > 0) ? (sideHeaderWidth + padDivider) : 0;
                    int thumbAreaW = std::max(0, width - headerExtra);
                    int thumbAreaStart = ThumbnailIsRight()
                        ? (curX + std::max(0, width - thumbAreaW))
                        : curX;
                    slotX = thumbAreaStart;
                    slotW = thumbAreaW;
                    thumbY = curY + (contentH - actualThumbH) / 2;
                    if (ThumbnailAlignRight()) {
                        thumbX = thumbAreaStart + std::max(0, thumbAreaW - thumbWidth);
                    } else if (ThumbnailAlignCentered()) {
                        thumbX = thumbAreaStart + std::max(0, (thumbAreaW - thumbWidth) / 2);
                    } else {
                        thumbX = thumbAreaStart;
                    }
                } else if (!StretchThumbsToTaskWidth() && width > thumbWidth) {
                    if (ThumbnailAlignRight()) thumbX += width - thumbWidth;
                    else if (ThumbnailAlignCentered()) thumbX += (width - thumbWidth) / 2;
                }
                w.rcThumbActual = { thumbX, thumbY, thumbX + thumbWidth, thumbY + actualThumbH };
                w.rcThumbSlot = { slotX, thumbY, slotX + slotW, thumbY + actualThumbH };
            }

            curX = curX + width + rightInc;
            placedCount = idx + 1;
        }

        if (curX - initialLeft > maxRowW) maxRowW = curX - initialLeft;
        g_winW = maxRowW + masterPad;
        g_winH = curY + bottomInc - initialTop + masterPad;
        if (g_winW > maxW) g_winW = maxW;

        for (int idx = 0; idx < placedCount; idx++) {
            int i = (g_layoutStartIndex + idx) % n;
            int rowTop = g_windows[i].rcCell.top;
            int rowMaxRight = 0;
            for (int jdx = idx; jdx < placedCount; jdx++) {
                int j = (g_layoutStartIndex + jdx) % n;
                if (g_windows[j].rcCell.top != rowTop) break;
                if (g_windows[j].rcCell.right > rowMaxRight) rowMaxRight = g_windows[j].rcCell.right;
            }
            int diff = (g_winW - masterPad > rowMaxRight) ? (g_winW - masterPad - rowMaxRight) / 2 : 0;
            if (diff > 0) {
                for (int jdx = idx; jdx < placedCount; jdx++) {
                    int j = (g_layoutStartIndex + jdx) % n;
                    if (g_windows[j].rcCell.top != rowTop) break;
                    g_windows[j].rcCell.left += diff;
                    g_windows[j].rcCell.right += diff;
                    g_windows[j].rcThumbActual.left += diff;
                    g_windows[j].rcThumbActual.right += diff;
                    g_windows[j].rcThumbSlot.left += diff;
                    g_windows[j].rcThumbSlot.right += diff;
                }
            }
            while (idx + 1 < placedCount && g_windows[(g_layoutStartIndex + idx + 1) % n].rcCell.top == rowTop) idx++;
        }
    } else {
        int curColMaxW = 0;
        int maxRight = 0;
        int maxBottom = 0;

        for (int idx = 0; idx < n; idx++) {
            int i = (g_layoutStartIndex + idx) % n;
            auto& w = g_windows[i];

            if (g_layoutStartIndex > 0 && idx > 0 && i < g_layoutStartIndex) {
                if (g_isPaginatedView) {
                    truncateRemaining(idx);
                    break;
                }
                if (((g_layoutStartIndex + idx - 1) % n) >= g_layoutStartIndex
                    && curY > initialTop + masterPad) {
                    curY = initialTop + masterPad;
                    curX = curX + curColMaxW + rightInc;
                    curColMaxW = 0;
                    if (curX + rightInc - initialLeft > maxW - masterPad) {
                        truncateRemaining(idx);
                        break;
                    }
                }
            }

            int width = 0;
            int thumbWidth = 0;
            int actualThumbH = thumbH;

            if (g_settings.showThumbnails && thumbH > 0) {
                if (w.effectiveSourceSize.cx > 0 && w.effectiveSourceSize.cy > 0) {
                    thumbWidth = std::max(1, (int)std::min((double)INT_MAX,
                        (double)w.effectiveSourceSize.cx * thumbH / w.effectiveSourceSize.cy));
                } else {
                    thumbWidth = thumbH;
                }

                int naturalThumbWidth = thumbWidth;
                if (thumbWidth > maxTileW) thumbWidth = maxTileW;
                if (w.effectiveSourceSize.cx > 0 && thumbWidth > w.effectiveSourceSize.cx) thumbWidth = w.effectiveSourceSize.cx;
                if (naturalThumbWidth > 0 && thumbWidth != naturalThumbWidth) {
                    actualThumbH = std::max(1, (int)((double)thumbWidth * thumbH / naturalThumbWidth));
                }

                width = thumbWidth;
                if (g_settings.rowWidth > 0) {
                    width = ScaleAutoFit(DpiScale(g_settings.rowWidth, dpiX));
                    if (StretchThumbsToTaskWidth() && !sidePlacement) {
                        thumbWidth = sidePlacement ? std::max(0, width - ((rowTitleH > 0) ? (sideHeaderWidth + padDivider) : 0)) : width;
                        if (thumbWidth <= 0) thumbWidth = DpiScale(16, dpiX);
                        actualThumbH = thumbH;
                    } else if (thumbWidth > width && width > 0) {
                        actualThumbH = (int)((double)width * actualThumbH / thumbWidth);
                        thumbWidth = width;
                    }
                }

                if (sidePlacement) {
                    int headerExtra = (rowTitleH > 0) ? (sideHeaderWidth + padDivider) : 0;
                    if (g_settings.rowWidth > 0) {
                        int minWidth = sideThumbSlotW + headerExtra;
                        if (width < minWidth) width = minWidth;
                    } else {
                        width = sideThumbSlotW + headerExtra;
                    }
                }
            } else {
                if (!g_settings.showTitle && g_settings.showIcon) {
                    if (g_settings.centerTaskContent) {
                        width = GetHeaderIconSizePx() + DpiScale(16, dpiX); // Base padding
                    } else {
                        width = GetHeaderIconSizePx() + DpiScale(20, dpiX); // Icon + btnSz(16) + gap(4)
                    }
                } else {
                    width = DpiScale(160, dpiX);
                }
                thumbWidth = width;
                actualThumbH = 0;
            }

            if (!g_settings.showThumbnails && g_settings.rowWidth > 0) {
                width = ScaleAutoFit(DpiScale(g_settings.rowWidth, dpiX));
                thumbWidth = width;
            }

            fitTaskWidth(width, thumbWidth, actualThumbH);

            if (curY + bottomInc - initialTop > maxH - masterPad && curY > initialTop + masterPad) {
                curY = initialTop + masterPad;
                curX = curX + curColMaxW + rightInc;
                curColMaxW = 0;
            }

            if (curX + width + rightInc - initialLeft > maxW - masterPad && idx > 0) {
                truncateRemaining(idx);
                break;
            }

            w.rcCell.left   = curX - initialLeft + elemPadLeft;
            w.rcCell.top    = curY - initialTop + elemPadTop;
            w.rcCell.right  = curX + width + rightInc - initialLeft - elemPadRight;
            w.rcCell.bottom = curY + bottomInc - initialTop - elemPadBot;
            if (g_settings.showThumbnails) {
                if (sidePlacement) {
                    int contentH = std::max(actualThumbH, minimumBodyH);
                    int baseH = std::max(thumbH, rowTitleH);
                    if (contentH < baseH) {
                        w.rcCell.bottom -= (baseH - contentH);
                    }
                } else if (actualThumbH < thumbH) {
                    w.rcCell.bottom -= (thumbH - std::max(actualThumbH, minimumThumbH));
                }
            } else if (!g_settings.showTitle && g_settings.showIcon) {
                int iconSz = GetHeaderIconSizePx();
                int curCellW = w.rcCell.right - w.rcCell.left;
                if ((curCellW - iconSz) % 2 != 0) w.rcCell.right++;
                int curCellH = w.rcCell.bottom - w.rcCell.top;
                if ((curCellH - iconSz) % 2 != 0) w.rcCell.bottom++;
            }

            if (g_settings.showThumbnails) {
                int thumbX = curX;
                int thumbY = curY;
                int slotX = curX;
                int slotW = width;
                if (sidePlacement) {
                    int contentH = std::max(actualThumbH, rowTitleH);
                    int headerExtra = (rowTitleH > 0) ? (sideHeaderWidth + padDivider) : 0;
                    int thumbAreaW = (g_settings.rowWidth > 0) ? sideThumbSlotW : std::max(0, width - headerExtra);
                    int thumbAreaStart = ThumbnailIsRight()
                        ? (curX + std::max(0, width - thumbAreaW))
                        : curX;
                    slotX = thumbAreaStart;
                    slotW = thumbAreaW;
                    thumbY = curY + (contentH - actualThumbH) / 2;
                    if (ThumbnailAlignRight()) {
                        thumbX = thumbAreaStart + std::max(0, thumbAreaW - thumbWidth);
                    } else if (ThumbnailAlignCentered()) {
                        thumbX = thumbAreaStart + std::max(0, (thumbAreaW - thumbWidth) / 2);
                    } else {
                        thumbX = thumbAreaStart;
                    }
                } else if (!StretchThumbsToTaskWidth() && width > thumbWidth) {
                    if (ThumbnailAlignRight()) thumbX += width - thumbWidth;
                    else if (ThumbnailAlignCentered()) thumbX += (width - thumbWidth) / 2;
                }
                w.rcThumbActual = { thumbX, thumbY, thumbX + thumbWidth, thumbY + actualThumbH };
                w.rcThumbSlot = { slotX, thumbY, slotX + slotW, thumbY + actualThumbH };
            }

            if (width > curColMaxW) curColMaxW = width;
            if (w.rcCell.right > maxRight) maxRight = w.rcCell.right;
            if (w.rcCell.bottom > maxBottom) maxBottom = w.rcCell.bottom;

            curY = curY + bottomInc;
            placedCount = idx + 1;
        }

        g_winW = maxRight + masterPad;
        g_winH = maxBottom + masterPad;
        if (g_winW > maxW) g_winW = maxW;
        if (g_winH > maxH) g_winH = maxH;

        for (int idx = 0; idx < placedCount; idx++) {
            int i = (g_layoutStartIndex + idx) % n;
            int colLeft = g_windows[i].rcCell.left;
            int colMaxBottom = 0;

            for (int jdx = idx; jdx < placedCount; jdx++) {
                int j = (g_layoutStartIndex + jdx) % n;
                if (g_windows[j].rcCell.left != colLeft) break;
                if (g_windows[j].rcCell.bottom > colMaxBottom) colMaxBottom = g_windows[j].rcCell.bottom;
            }

            int diff = (g_winH - masterPad > colMaxBottom) ? (g_winH - masterPad - colMaxBottom) / 2 : 0;
            if (diff > 0) {
                for (int jdx = idx; jdx < placedCount; jdx++) {
                    int j = (g_layoutStartIndex + jdx) % n;
                    if (g_windows[j].rcCell.left != colLeft) break;
                    g_windows[j].rcCell.top += diff;
                    g_windows[j].rcCell.bottom += diff;
                    g_windows[j].rcThumbActual.top += diff;
                    g_windows[j].rcThumbActual.bottom += diff;
                    g_windows[j].rcThumbSlot.top += diff;
                    g_windows[j].rcThumbSlot.bottom += diff;
                }
            }

            while (idx + 1 < placedCount && g_windows[(g_layoutStartIndex + idx + 1) % n].rcCell.left == colLeft) idx++;
        }
    }
}

static void RegisterThumbnails() {
    if (!g_settings.showThumbnails || !g_hSwitcher) return;
    if (DockLayoutActive()) {
        UpdateDockThumbnailDwm();
        return;
    }
    for (auto& w : g_windows) {
        if (!w.hThumbs.count(g_hSwitcher)) {
            HTHUMBNAIL hT = NULL;
            if (SUCCEEDED(DwmRegisterThumbnail(g_hSwitcher, w.hWnd, &hT))) {
                w.hThumbs[g_hSwitcher] = hT;
                SIZE src = {0}; DwmQueryThumbnailSourceSize(hT, &src);
                w.sourceSize = src;
            }
        } else {
            SIZE src = {0};
            if (SUCCEEDED(DwmQueryThumbnailSourceSize(w.hThumbs[g_hSwitcher], &src)) && src.cx > 0 && src.cy > 0) {
                w.sourceSize = src;
            }
        }
        UpdateEntrySourceCrop(w);
        for (HWND m : g_hMirrorSwitchers) {
            if (!w.hThumbs.count(m)) {
                HTHUMBNAIL hT = NULL;
                if (SUCCEEDED(DwmRegisterThumbnail(m, w.hWnd, &hT))) w.hThumbs[m] = hT;
            }
        }
        
        for (const auto& kv : w.hThumbs) {
            HTHUMBNAIL hThumb = kv.second;
            if (!hThumb) continue;
            // Truncated, off-page, or zero-size thumbnails must be explicitly hidden
            if (w.rcThumbActual.right <= w.rcThumbActual.left ||
                w.rcThumbActual.bottom <= w.rcThumbActual.top) {
                DWM_THUMBNAIL_PROPERTIES p = {};
                p.dwFlags = DWM_TNP_VISIBLE;
                p.fVisible = FALSE;
                UpdateDwmThumbnail(hThumb, &p);
                continue;
            }
            DWM_THUMBNAIL_PROPERTIES p = {};
            p.dwFlags = DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
            p.fSourceClientAreaOnly = FALSE;
            p.rcDestination = w.rcThumbActual;
            int offX = (int)roundf(g_scrollTransition.offsetCurrentX);
            int offY = (int)roundf(g_scrollTransition.offsetCurrentY);
            if (offX != 0 || offY != 0) {
                OffsetRect(&p.rcDestination, offX, offY);
            }
            float _itemAlpha = (g_layoutTransition.active && w.isNewEntry) ? w.enterAlpha : 1.0f;
            p.opacity = (BYTE)roundf(CurrentPresentationAlpha() * _itemAlpha * 255.0f);
            p.fVisible = TRUE;
            // Set DWM_TNP_RECTSOURCE for a non-trivial crop, minimized windows,
            // or to reset a rectangle retained by an earlier clipping update.
            // For restored non-maximized windows, omitting DWM_TNP_RECTSOURCE preserves
            // the window's visual style including rounded corners on Windows 11.
            // For iconic windows, DWM's default style introduces an extraneous drop shadow
            // halo around the cached thumbnail that spills past rcDestination; setting
            // RECTSOURCE clips it strictly to rcThumbActual.
            bool needsCrop = (IsIconic(w.hWnd) || g_thumbSourceRects.count(hThumb) ||
                             (w.rcSourceCrop.left != 0 || w.rcSourceCrop.top != 0 ||
                              w.rcSourceCrop.right != w.sourceSize.cx || w.rcSourceCrop.bottom != w.sourceSize.cy))
                             && (w.rcSourceCrop.right > w.rcSourceCrop.left && w.rcSourceCrop.bottom > w.rcSourceCrop.top);
            if (needsCrop) {
                p.dwFlags |= DWM_TNP_RECTSOURCE;
                p.rcSource = w.rcSourceCrop;
            }
            UpdateDwmThumbnail(hThumb, &p);
        }
    }
}
static void UnregisterThumbnails() {
    for (auto& w : g_windows) {
        for (const auto& kv : w.hThumbs) {
            if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
        }
        w.hThumbs.clear();
    }
    g_lastThumbState.clear();
}


// Drawing Helpers

static COLORREF ResolveColor(const WCHAR* mode, const WCHAR* customHex, COLORREF defaultColor) {
    if (wcscmp(mode, L"accent") == 0) return GetAccentColor();
    if (wcscmp(mode, L"custom") == 0) {
        COLORREF parsed;
        if (ParseHexColor(customHex, &parsed)) return parsed;
    }
    return defaultColor;
}

static COLORREF GetContourColor() {
    if (g_isDarkMode) {
        return ResolveColor(g_settings.borderColorModeDark,
                            g_settings.customBorderColorDark,
                            SWS_CONTOUR_DARK);
    }
    return ResolveColor(g_settings.borderColorModeLight,
                        g_settings.customBorderColorLight,
                        SWS_CONTOUR_LIGHT);
}

static COLORREF GetHighlightFillColor() {
    if (g_isDarkMode) {
        return ResolveColor(g_settings.highlightFillColorModeDark,
                            g_settings.customHighlightFillColorDark,
                            SWS_CONTOUR_DARK);
    }
    return ResolveColor(g_settings.highlightFillColorModeLight,
                        g_settings.customHighlightFillColorLight,
                        SWS_CONTOUR_LIGHT);
}

static COLORREF GetBgColor() {
    if (g_isDarkMode) {
        return ResolveColor(g_settings.bgColorModeDark,
                            g_settings.customBgColorDark,
                            SWS_BG_DARK);
    }
    return ResolveColor(g_settings.bgColorModeLight,
                        g_settings.customBgColorLight,
                        SWS_BG_LIGHT);
}

static COLORREF GetIconBackgroundColor() {
    if (g_isDarkMode) {
        return ResolveColor(g_settings.iconBgColorModeDark,
                            g_settings.customIconBgColorDark,
                            RGB(0, 0, 0));
    }
    return ResolveColor(g_settings.iconBgColorModeLight,
                        g_settings.customIconBgColorLight,
                        RGB(255, 255, 255));
}

static COLORREF GetIndicatorBackgroundColor() {
    if (g_isDarkMode) {
        return ResolveColor(g_settings.indicatorBgColorModeDark,
                            g_settings.customIndicatorBgColorDark,
                            RGB(51, 51, 51)); // #333333
    }
    return ResolveColor(g_settings.indicatorBgColorModeLight,
                        g_settings.customIndicatorBgColorLight,
                        RGB(234, 234, 234)); // #EAEAEA
}

static COLORREF GetIndicatorTextColor() {
    if (g_isDarkMode) {
        return ResolveColor(g_settings.indicatorTextColorModeDark,
                            g_settings.customIndicatorTextColorDark,
                            RGB(255, 255, 255)); // #FFFFFF
    }
    return ResolveColor(g_settings.indicatorTextColorModeLight,
                        g_settings.customIndicatorTextColorLight,
                        RGB(0, 0, 0)); // #000000
}

static Gdiplus::Bitmap* CreateIconShadowBitmap(HICON hIcon, int width, int height, float shadowAlphaMult) {
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    void* pBlackBits = nullptr;
    void* pWhiteBits = nullptr;
    HDC hdc = GetDC(NULL);
    HBITMAP hBmpBlack = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBlackBits, NULL, 0);
    HBITMAP hBmpWhite = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pWhiteBits, NULL, 0);
    
    if (!hBmpBlack || !hBmpWhite) {
        if (hBmpBlack) DeleteObject(hBmpBlack);
        if (hBmpWhite) DeleteObject(hBmpWhite);
        ReleaseDC(NULL, hdc);
        return nullptr;
    }
    
    HDC hdcMem = CreateCompatibleDC(hdc);
    
    // Draw on black
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmpBlack);
    memset(pBlackBits, 0, (size_t)width * height * 4);
    memset(pWhiteBits, 0, (size_t)width * height * 4);
    RECT rc = {0, 0, width, height};
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdcMem, &rc, blackBrush);
    DrawIconEx(hdcMem, 0, 0, hIcon, width, height, 0, NULL, DI_NORMAL);
    DeleteObject(blackBrush);
    
    // Draw on white
    SelectObject(hdcMem, hBmpWhite);
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdcMem, &rc, whiteBrush);
    DrawIconEx(hdcMem, 0, 0, hIcon, width, height, 0, NULL, DI_NORMAL);
    DeleteObject(whiteBrush);
    
    SelectObject(hdcMem, hOld);
    GdiFlush();
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdc);
    
    // Write into storage owned by the returned bitmap. Cloning a bitmap over
    // temporary scan0 memory can retain that backing after the buffer is freed.
    Gdiplus::Bitmap* shadowBmp = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
    Gdiplus::Rect shadowRect(0, 0, width, height);
    Gdiplus::BitmapData shadowData = {};
    if (shadowBmp->GetLastStatus() != Gdiplus::Ok ||
        shadowBmp->LockBits(&shadowRect, Gdiplus::ImageLockModeWrite,
                            PixelFormat32bppARGB, &shadowData) != Gdiplus::Ok) {
        delete shadowBmp;
        DeleteObject(hBmpBlack);
        DeleteObject(hBmpWhite);
        return nullptr;
    }
    BYTE* blackPtr = (BYTE*)pBlackBits;
    BYTE* whitePtr = (BYTE*)pWhiteBits;
    for (int y = 0; y < height; ++y) {
        BYTE* shadowRow = (BYTE*)shadowData.Scan0 + y * shadowData.Stride;
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 4;
            int whiteB = whitePtr[idx];
            int blackB = blackPtr[idx];
            int a = 255 - (whiteB - blackB);
            if (a < 0) a = 0;
            if (a > 255) a = 255;
            int finalAlpha = (int)(a * shadowAlphaMult);
            if (finalAlpha > 255) finalAlpha = 255;
            shadowRow[x * 4] = 0;     // B
            shadowRow[x * 4 + 1] = 0; // G
            shadowRow[x * 4 + 2] = 0; // R
            shadowRow[x * 4 + 3] = (BYTE)finalAlpha;
        }
    }
    Gdiplus::Status unlockStatus = shadowBmp->UnlockBits(&shadowData);
    DeleteObject(hBmpBlack);
    DeleteObject(hBmpWhite);
    if (unlockStatus != Gdiplus::Ok) {
        delete shadowBmp;
        return nullptr;
    }
    return shadowBmp;
}

// (s_iconAlphaScratch is defined above, near FreeCachedBuffers.)

static void DrawIconWithAlpha(HDC hdc, int x, int y, HICON hIcon, int size, float alpha) {
    if (!hIcon || size <= 0 || alpha <= 0.001f) return;
    if (alpha >= 0.999f) {
        DrawIconEx(hdc, x, y, hIcon, size, size, 0, NULL, DI_NORMAL);
        return;
    }

    auto it = s_iconAlphaScratch.find(size);
    if (it == s_iconAlphaScratch.end()) {
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = size;
        bmi.bmiHeader.biHeight = -size; // top-down
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        IconAlphaScratch s;
        s.hdc = CreateCompatibleDC(hdc);
        s.dib = CreateDIBSection(s.hdc, &bmi, DIB_RGB_COLORS, &s.bits, NULL, 0);
        if (!s.hdc || !s.dib || !s.bits) {
            if (s.dib) DeleteObject(s.dib);
            if (s.hdc) DeleteDC(s.hdc);
            return;
        }
        SelectObject(s.hdc, s.dib); // keep the DIB selected for the cache lifetime
        it = s_iconAlphaScratch.emplace(size, s).first;
    }

    const IconAlphaScratch& s = it->second;
    ZeroMemory(s.bits, (size_t)size * size * 4);
    DrawIconEx(s.hdc, 0, 0, hIcon, size, size, 0, NULL, DI_NORMAL);

    DWORD* pPixels = (DWORD*)s.bits;
    for (int i = 0; i < size * size; ++i) {
        if ((pPixels[i] & 0x00FFFFFF) != 0 && (pPixels[i] & 0xFF000000) == 0) {
            pPixels[i] |= 0xFF000000;
        }
    }

    BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)roundf(alpha * 255.0f), AC_SRC_ALPHA };
    AlphaBlend(hdc, x, y, size, size, s.hdc, 0, 0, size, size, bf);
    // Intentionally not deleting hdc/dib: cached for reuse, freed in
    // FreeCachedBuffers().
}

// (s_iconShadowCache is defined above, near FreeCachedBuffers.)

static std::shared_ptr<WindowIconShadow> GetWindowIconShadow(
    HICON icon, int size, std::shared_ptr<OwnedWindowIcon> owner) {
    // Unknown borrowed handles are drawn with an uncached record. The shared
    // application placeholder is permanent; every other cache key has an owner.
    if (owner && owner->handle != icon) return {};
    bool cacheable = owner || icon == s_iconPlaceholder;
    auto key = std::make_pair(icon, size);
    if (cacheable) {
        auto it = s_iconShadowCache.find(key);
        if (it != s_iconShadowCache.end() && it->second->owner == owner)
            return it->second;
    }
    auto record = std::make_shared<WindowIconShadow>();
    record->owner = std::move(owner); // Pin before entering the GDI factory.
    record->bitmap.reset(CreateIconShadowBitmap(icon, size, size, 0.08f));
    if (!record->bitmap) return {};
    if (cacheable) s_iconShadowCache[key] = record;
    return record;
}

static void MaskRectCorners(HDC hdc, const RECT& rc, int radiusPx, bool forceOpaque = false, COLORREF overrideBg = CLR_INVALID) {
    if (radiusPx <= 0) {
        return;
    }

    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) {
        return;
    }

    int r = radiusPx;
    if (r * 2 > w) r = w / 2;
    if (r * 2 > h) r = h / 2;
    if (r <= 0) {
        return;
    }

    COLORREF bg = (overrideBg != CLR_INVALID) ? overrideBg : GetBgColor();
    // In layered mode, punch fully transparent corners to force thumbnail clipping.
    BYTE alpha = (ThemeIs(L"none") && !forceOpaque) ? 0 : 255;

    Gdiplus::Graphics graphics(hdc);
    if (alpha == 0) {
        graphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
    }
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::SolidBrush brush(Gdiplus::Color(alpha, GetRValue(bg), GetGValue(bg), GetBValue(bg)));

    int d = r * 2;
    Gdiplus::GraphicsPath cutTl, cutTr, cutBr, cutBl;
    Gdiplus::REAL ext = (alpha != 0) ? 1.0f : 0.0f; // Extend outward to cover anti-aliased edge

    cutTl.StartFigure();
    cutTl.AddLine((Gdiplus::REAL)rc.left - ext, (Gdiplus::REAL)rc.top + r, (Gdiplus::REAL)rc.left - ext, (Gdiplus::REAL)rc.top - ext);
    cutTl.AddLine((Gdiplus::REAL)rc.left - ext, (Gdiplus::REAL)rc.top - ext, (Gdiplus::REAL)rc.left + r, (Gdiplus::REAL)rc.top - ext);
    cutTl.AddArc((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)d, (Gdiplus::REAL)d, 270, -90);
    cutTl.CloseFigure();

    cutTr.StartFigure();
    cutTr.AddLine((Gdiplus::REAL)rc.right - r, (Gdiplus::REAL)rc.top - ext, (Gdiplus::REAL)rc.right + ext, (Gdiplus::REAL)rc.top - ext);
    cutTr.AddLine((Gdiplus::REAL)rc.right + ext, (Gdiplus::REAL)rc.top - ext, (Gdiplus::REAL)rc.right + ext, (Gdiplus::REAL)rc.top + r);
    cutTr.AddArc((Gdiplus::REAL)rc.right - d, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)d, (Gdiplus::REAL)d, 0, -90);
    cutTr.CloseFigure();

    cutBr.StartFigure();
    cutBr.AddLine((Gdiplus::REAL)rc.right + ext, (Gdiplus::REAL)rc.bottom - r, (Gdiplus::REAL)rc.right + ext, (Gdiplus::REAL)rc.bottom + ext);
    cutBr.AddLine((Gdiplus::REAL)rc.right + ext, (Gdiplus::REAL)rc.bottom + ext, (Gdiplus::REAL)rc.right - r, (Gdiplus::REAL)rc.bottom + ext);
    cutBr.AddArc((Gdiplus::REAL)rc.right - d, (Gdiplus::REAL)rc.bottom - d, (Gdiplus::REAL)d, (Gdiplus::REAL)d, 90, -90);
    cutBr.CloseFigure();

    cutBl.StartFigure();
    cutBl.AddLine((Gdiplus::REAL)rc.left + r, (Gdiplus::REAL)rc.bottom + ext, (Gdiplus::REAL)rc.left - ext, (Gdiplus::REAL)rc.bottom + ext);
    cutBl.AddLine((Gdiplus::REAL)rc.left - ext, (Gdiplus::REAL)rc.bottom + ext, (Gdiplus::REAL)rc.left - ext, (Gdiplus::REAL)rc.bottom - r);
    cutBl.AddArc((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.bottom - d, (Gdiplus::REAL)d, (Gdiplus::REAL)d, 180, -90);
    cutBl.CloseFigure();

    graphics.FillPath(&brush, &cutTl);
    graphics.FillPath(&brush, &cutTr);
    graphics.FillPath(&brush, &cutBr);
    graphics.FillPath(&brush, &cutBl);
}

// Draw a rectangular contour around a floating-point rect.
// direction: 1 = inner (shrinks inward), -1 = outer (grows outward)
static void DrawContourF(HDC hdc, const RectF& rc, float contourSize, int direction, float overrideCornerRadius = -1.0f, BYTE alpha = 255) {
    if (alpha == 0) return;
    COLORREF c = GetContourColor();
    BYTE r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);

    float cornerRadius = (overrideCornerRadius >= 0.0f) ? overrideCornerRadius : (float)GetTaskUiCornerRadiusPx();
    float penWidth = contourSize * (float)g_dpiX / 96.0f;
    if (penWidth < 1.0f) penWidth = 1.0f;

    RectF drawRc = rc;
    if (direction < 0) {
        drawRc.left -= 2.0f;
        drawRc.top -= 2.0f;
        drawRc.right += 2.0f;
        drawRc.bottom += 2.0f;
        cornerRadius += 2.0f;
    }

    if (cornerRadius > 0.0f) {
        // --- ROUNDED BRANCH: Smooth WinUI 3 Fluent soft antialiasing ---
        float width = drawRc.right - drawRc.left - penWidth;
        float height = drawRc.bottom - drawRc.top - penWidth;
        if (width <= 0.0f || height <= 0.0f) return;

        if (cornerRadius * 2.0f > width) cornerRadius = width * 0.5f;
        if (cornerRadius * 2.0f > height) cornerRadius = height * 0.5f;

        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

        Gdiplus::REAL left = drawRc.left + penWidth * 0.5f;
        Gdiplus::REAL top = drawRc.top + penWidth * 0.5f;

        float pathRadius = cornerRadius - penWidth * 0.5f;
        if (pathRadius < 0.0f) pathRadius = 0.0f;
        Gdiplus::REAL d = pathRadius * 2.0f;
        Gdiplus::GraphicsPath path;
        path.AddArc(left, top, d, d, 180, 90);
        path.AddArc(left + width - d, top, d, d, 270, 90);
        path.AddArc(left + width - d, top + height - d, d, d, 0, 90);
        path.AddArc(left, top + height - d, d, d, 90, 90);
        path.CloseFigure();

        // Dual-pass WinUI 3 Fluent soft antialiasing
        BYTE haloAlpha = (BYTE)(alpha * 0.30f);
        BYTE coreAlpha = (BYTE)(alpha * 0.85f);
        if (haloAlpha > 0) {
            Gdiplus::Pen haloPen(Gdiplus::Color(haloAlpha, r, g, b), penWidth + 1.0f);
            graphics.DrawPath(&haloPen, &path);
        }
        Gdiplus::Pen corePen(Gdiplus::Color(coreAlpha, r, g, b), penWidth);
        graphics.DrawPath(&corePen, &path);
    } else {
        // --- SQUARED BRANCH: Razor-sharp, pixel-aligned rendering ---
        int snapLeft = (int)roundf(drawRc.left);
        int snapTop = (int)roundf(drawRc.top);
        int snapRight = (int)roundf(drawRc.right);
        int snapBottom = (int)roundf(drawRc.bottom);
        int snapWidth = snapRight - snapLeft;
        int snapHeight = snapBottom - snapTop;
        int strokePx = (int)roundf(penWidth);
        if (strokePx < 1) strokePx = 1;
        if (snapWidth <= strokePx || snapHeight <= strokePx) return;

        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);

        int inset = strokePx / 2;
        INT rectLeft = snapLeft + inset;
        INT rectTop = snapTop + inset;
        INT rectWidth = snapWidth - strokePx;
        INT rectHeight = snapHeight - strokePx;

        // Single stroke at 100% full alpha with sharp 90-degree mitered corners
        Gdiplus::Pen corePen(Gdiplus::Color(alpha, r, g, b), (Gdiplus::REAL)strokePx);
        corePen.SetLineJoin(Gdiplus::LineJoinMiter);
        graphics.DrawRectangle(&corePen, rectLeft, rectTop, rectWidth, rectHeight);
    }
}

static void DrawSelectionFillF(HDC hdc, const RectF& rc) {
    RectF fillRc = rc;
    fillRc.left += 1.0f;
    fillRc.top += 1.0f;
    fillRc.right -= 1.0f;
    fillRc.bottom -= 1.0f;
    float width = fillRc.right - fillRc.left;
    float height = fillRc.bottom - fillRc.top;
    if (width <= 0.0f || height <= 0.0f) return;

    COLORREF c = GetHighlightFillColor();
    BYTE r = GetRValue(c);
    BYTE g = GetGValue(c);
    BYTE b = GetBValue(c);

    float cornerRadius = (float)GetTaskUiCornerRadiusPx();
    if (cornerRadius > 0.0f) {
        float maxRadius = fminf(width, height) * 0.5f;
        if (cornerRadius > maxRadius) cornerRadius = maxRadius;

        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        BYTE fillAlpha = g_isDarkMode ? 28 : 18;
        BYTE strokeAlpha = g_isDarkMode ? 20 : 15;
        Gdiplus::SolidBrush brush(Gdiplus::Color(fillAlpha, r, g, b));

        Gdiplus::REAL left = fillRc.left;
        Gdiplus::REAL top = fillRc.top;
        Gdiplus::REAL w = width;
        Gdiplus::REAL h = height;
        Gdiplus::REAL d = cornerRadius * 2.0f;

        Gdiplus::GraphicsPath path;
        path.AddArc(left, top, d, d, 180, 90);
        path.AddArc(left + w - d, top, d, d, 270, 90);
        path.AddArc(left + w - d, top + h - d, d, d, 0, 90);
        path.AddArc(left, top + h - d, d, d, 90, 90);
        path.CloseFigure();
        graphics.FillPath(&brush, &path);
        Gdiplus::Pen strokePen(Gdiplus::Color(strokeAlpha, r, g, b), 1.0f);
        graphics.DrawPath(&strokePen, &path);
        return;
    }

    // Squared: razor-sharp integer pixel alignment
    int snapLeft = (int)roundf(fillRc.left);
    int snapTop = (int)roundf(fillRc.top);
    int snapRight = (int)roundf(fillRc.right);
    int snapBottom = (int)roundf(fillRc.bottom);
    int snapW = snapRight - snapLeft;
    int snapH = snapBottom - snapTop;
    if (snapW <= 0 || snapH <= 0) return;

    Gdiplus::Graphics graphics(hdc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);
    BYTE fillAlpha = g_isDarkMode ? 28 : 18;
    BYTE strokeAlpha = g_isDarkMode ? 20 : 15;
    Gdiplus::SolidBrush brush(Gdiplus::Color(fillAlpha, r, g, b));
    graphics.FillRectangle(&brush, snapLeft, snapTop, snapW, snapH);
    Gdiplus::Pen strokePen(Gdiplus::Color(strokeAlpha, r, g, b), 1.0f);
    graphics.DrawRectangle(&strokePen, snapLeft, snapTop, snapW - 1, snapH - 1);
}

static RECT GetHeaderContentRectForEntry(const RECT& rcCell, const RECT& rcThumbActual, const RECT& rcThumbSlot) {
    int padLeft = DpiScale(g_settings.entryPadding, g_dpiX);
    int padTop = DpiScale(g_settings.entryPadding, g_dpiY);
    int padBottom = DpiScale(g_settings.entryPadding, g_dpiY);
    RECT rc = {
        rcCell.left + padLeft,
        rcCell.top + padTop,
        rcCell.right - padLeft,
        rcCell.bottom - padBottom,
    };

    if (!g_settings.showThumbnails) {
        return rc;
    }

    const RECT& rcHeaderSplit = ((rcThumbSlot.left != 0 || rcThumbSlot.top != 0 ||
                                  rcThumbSlot.right != 0 || rcThumbSlot.bottom != 0))
                                    ? rcThumbSlot
                                    : rcThumbActual;

    // In badge mode the title position is controlled by badgeTitlePosition, not thumbnailPosition.
    // "title on top" → thumb on bottom → header rect is above the thumb (ThumbnailIsBottom semantics)
    // "title on bottom" → thumb on top → header rect is below the thumb (ThumbnailIsTop semantics)
    bool effectiveThumbTop = BadgeLayoutActive() ? !BadgeTitleIsTop() : ThumbnailIsTop();
    bool effectiveThumbBottom = BadgeLayoutActive() ? BadgeTitleIsTop() : ThumbnailIsBottom();
    bool effectiveThumbSide = BadgeLayoutActive() ? false : ThumbnailIsSide();

    if (effectiveThumbTop) {
        rc.top = rcHeaderSplit.bottom + g_activePadDivider;
    } else if (effectiveThumbBottom) {
        rc.bottom = rcHeaderSplit.top - g_activePadDivider;
    } else if (effectiveThumbSide) {
        int divider = DpiScale(SWS_PAD_DIVIDER, g_dpiX);
        if (ThumbnailIsLeft()) {
            rc.left = rcHeaderSplit.right + divider;
        } else {
            rc.right = rcHeaderSplit.left - divider;
        }
    }

    if (rc.right < rc.left) rc.right = rc.left;
    if (rc.bottom < rc.top) rc.bottom = rc.top;
    return rc;
}

static inline RECT GetHeaderContentRectForEntry(const WindowEntry& e) {
    return GetHeaderContentRectForEntry(e.rcCell, e.rcThumbActual, e.rcThumbSlot);
}

static int GetHeaderTopForEntry(const RECT& rcCell, const RECT& rcThumbActual, const RECT& rcThumbSlot) {
    RECT rcHeader = GetHeaderContentRectForEntry(rcCell, rcThumbActual, rcThumbSlot);
    int rowTitleH = GetHeaderRowHeightPx();
    if (BadgeLayoutActive()) {
        rowTitleH = g_settings.showTitle ? GetHeaderTitleHeightPx() : 0;
    }
    if (rowTitleH <= 0) {
        return rcHeader.top;
    }

    int available = rcHeader.bottom - rcHeader.top;
    if (available <= rowTitleH) {
        return rcHeader.top;
    }

    return rcHeader.top + (available - rowTitleH) / 2;
}

static inline int GetHeaderTopForEntry(const WindowEntry& e) {
    return GetHeaderTopForEntry(e.rcCell, e.rcThumbActual, e.rcThumbSlot);
}

static RECT GetCloseButtonRect(const RECT& rcCell, const RECT& rcThumbActual, const RECT& rcThumbSlot) {
    if (DockLayoutActive()) {
        if (DockCloseButtonIsHidden()) {
            return { 0, 0, 0, 0 };
        }
        int btnSz = DpiScale(16, g_dpiX);
        int pad = DpiScale(2, g_dpiX);
        return ComputeDockPerimeterRect(rcCell, btnSz, btnSz, g_settings.dockCloseButtonPosition, pad);
    }

    int rowTitleH = GetHeaderRowHeightPx();
    int btnSz = DpiScale(24, g_dpiX);
    int bx = 0, by = 0;
    if (rowTitleH == 0 || (g_settings.showThumbnails && ThumbnailIsSide()) || BadgeLayoutActive()) {
        int btnPadding = DpiScale(4, g_dpiX);
        bx = rcThumbActual.right - btnSz - btnPadding;
        by = rcThumbActual.top + btnPadding;
    } else if (!g_settings.showThumbnails && !g_settings.showTitle && g_settings.showIcon) {
        int padLeft = DpiScale(g_settings.entryPadding, g_dpiX);
        int padTop = DpiScale(g_settings.entryPadding, g_dpiY);
        int contentLeft = rcCell.left + padLeft;
        int contentRight = rcCell.right - padLeft;
        int iconSz = GetHeaderIconSizePx();
        int availableW = contentRight - contentLeft;
        if (availableW < 0) availableW = 0;

        int iconX = contentLeft;
        int iconY = rcCell.top + padTop + (rcCell.bottom - rcCell.top - 2 * padTop - iconSz) / 2;

        if (g_settings.centerTaskContent && iconSz < availableW) {
            iconX = contentLeft + (availableW - iconSz) / 2;
        }

        int btnPadding = DpiScale(2, g_dpiX);
        btnSz = DpiScale(16, g_dpiX);

        if (g_settings.centerTaskContent) {
            bx = iconX + iconSz - btnSz + btnPadding;
            by = iconY - btnPadding;
        } else {
            int gap = DpiScale(4, g_dpiX);
            bx = iconX + iconSz + gap;
            by = iconY;
        }
    } else {
        RECT rcHeaderContent = GetHeaderContentRectForEntry(rcCell, rcThumbActual, rcThumbSlot);
        int headerTop = GetHeaderTopForEntry(rcCell, rcThumbActual, rcThumbSlot);
        bx = rcHeaderContent.right - btnSz;
        by = HeaderIsVertical() ? headerTop : (headerTop + (rowTitleH - btnSz) / 2);
    }
    return { bx, by, bx + btnSz, by + btnSz };
}

static inline RECT GetCloseButtonRect(const WindowEntry& e) {
    return GetCloseButtonRect(e.rcCell, e.rcThumbActual, e.rcThumbSlot);
}

static bool HitTestCloseButton(const WindowEntry& e, POINT ptClient) {
    RECT rc = GetCloseButtonRect(e);
    if (rc.left == 0 && rc.right == 0 && rc.top == 0 && rc.bottom == 0) return false;
    // Inflate slightly (2px) for comfortable mouse target acquisition
    int pad = DpiScale(2, g_dpiX);
    RECT rcHit = { rc.left - pad, rc.top - pad, rc.right + pad, rc.bottom + pad };
    return PtInRect(&rcHit, ptClient) != FALSE;
}

static void DrawCloseButton(HDC hdc, const RECT& btnRc, float btnAlpha, float hoverAlpha, bool isPressed, float btnScale) {
    if (btnAlpha <= 0.01f) return;

    float origW = (float)(btnRc.right - btnRc.left);
    float origH = (float)(btnRc.bottom - btnRc.top);
    if (origW <= 0.0f || origH <= 0.0f) return;

    // Transform is sampled from its own 167 ms track, never from 83 ms opacity.
    float scale = btnScale;
    float btnW = origW * scale;
    float btnH = origH * scale;
    float bx = (float)btnRc.left + (origW - btnW) * 0.5f;
    float by = (float)btnRc.top + (origH - btnH) * 0.5f;

    Gdiplus::Graphics graphics(hdc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

    int btnRadius = GetCloseButtonCornerRadiusPx();
    if ((float)(btnRadius * 2) > btnW) btnRadius = (int)(btnW / 2.0f);

    // 1. Solid idle plate for contrast against bright or dark thumbnails (configured by setting / Win11 default)
    BYTE idlePlateAlpha = g_settings.showCloseButtonBackground ? (BYTE)roundf(255.0f * btnAlpha) : 0;
    if (idlePlateAlpha > 0) {
        COLORREF plateCol = g_isDarkMode ? RGB(36, 36, 36) : RGB(255, 255, 255);
        Gdiplus::SolidBrush idleBrush(Gdiplus::Color(idlePlateAlpha, GetRValue(plateCol), GetGValue(plateCol), GetBValue(plateCol)));
        COLORREF borderCol = g_isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
        BYTE borderAlpha = (BYTE)roundf((g_isDarkMode ? 32.0f : 22.0f) * btnAlpha);
        Gdiplus::Pen borderPen(Gdiplus::Color(borderAlpha, GetRValue(borderCol), GetGValue(borderCol), GetBValue(borderCol)), 1.0f);

        if (btnRadius > 0) {
            // Soft drop shadow for elevation contrast over complex thumbnail content
            Gdiplus::GraphicsPath shadowPath;
            Gdiplus::REAL d = (Gdiplus::REAL)(btnRadius * 2);
            shadowPath.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)(by + 1.0f), d, d, 180.0f, 90.0f);
            shadowPath.AddArc((Gdiplus::REAL)(bx + btnW) - d, (Gdiplus::REAL)(by + 1.0f), d, d, 270.0f, 90.0f);
            shadowPath.AddArc((Gdiplus::REAL)(bx + btnW) - d, (Gdiplus::REAL)(by + btnH + 1.0f) - d, d, d, 0.0f, 90.0f);
            shadowPath.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)(by + btnH + 1.0f) - d, d, d, 90.0f, 90.0f);
            shadowPath.CloseFigure();
            Gdiplus::SolidBrush shadowBrush(Gdiplus::Color((BYTE)roundf(40.0f * btnAlpha), 0, 0, 0));
            graphics.FillPath(&shadowBrush, &shadowPath);

            Gdiplus::GraphicsPath path;
            path.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)by, d, d, 180.0f, 90.0f);
            path.AddArc((Gdiplus::REAL)(bx + btnW) - d, (Gdiplus::REAL)by, d, d, 270.0f, 90.0f);
            path.AddArc((Gdiplus::REAL)(bx + btnW) - d, (Gdiplus::REAL)(by + btnH) - d, d, d, 0.0f, 90.0f);
            path.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)(by + btnH) - d, d, d, 90.0f, 90.0f);
            path.CloseFigure();
            graphics.FillPath(&idleBrush, &path);
            graphics.DrawPath(&borderPen, &path);
        } else {
            graphics.FillRectangle(&idleBrush, (Gdiplus::REAL)bx, (Gdiplus::REAL)by, (Gdiplus::REAL)btnW, (Gdiplus::REAL)btnH);
            graphics.DrawRectangle(&borderPen, (Gdiplus::REAL)bx, (Gdiplus::REAL)by, (Gdiplus::REAL)btnW, (Gdiplus::REAL)btnH);
        }
    }

    // 2. Interactive Crimson / Red Hover Plate
    BYTE redAlpha = (BYTE)roundf(255.0f * hoverAlpha * btnAlpha);
    if (redAlpha > 0) {
        COLORREF redPlateCol = IsWin11OrGreater() ? RGB(196, 43, 28) : RGB(232, 17, 35);
        Gdiplus::SolidBrush redBrush(Gdiplus::Color(redAlpha, GetRValue(redPlateCol), GetGValue(redPlateCol), GetBValue(redPlateCol)));
        if (btnRadius > 0) {
            Gdiplus::GraphicsPath path;
            Gdiplus::REAL d = (Gdiplus::REAL)(btnRadius * 2);
            path.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)by, d, d, 180.0f, 90.0f);
            path.AddArc((Gdiplus::REAL)(bx + btnW) - d, (Gdiplus::REAL)by, d, d, 270.0f, 90.0f);
            path.AddArc((Gdiplus::REAL)(bx + btnW) - d, (Gdiplus::REAL)(by + btnH) - d, d, d, 0.0f, 90.0f);
            path.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)(by + btnH) - d, d, d, 90.0f, 90.0f);
            path.CloseFigure();
            graphics.FillPath(&redBrush, &path);
        } else {
            graphics.FillRectangle(&redBrush, (Gdiplus::REAL)bx, (Gdiplus::REAL)by, (Gdiplus::REAL)btnW, (Gdiplus::REAL)btnH);
        }
    }

    // 3. Crisp Anti-Aliased Vector 'X' Glyph with Rounded / Flat Caps
    // Tactile displacement: 1px downward when pressed
    float pressOff = isPressed ? 1.0f : 0.0f;
    float cx = bx + btnW * 0.5f;
    float cy = by + btnH * 0.5f + pressOff;
    float glyphSpan = ((origW == (float)DpiScale(16, g_dpiX)) ? (3.5f * g_dpiX / 96.0f) : (4.5f * g_dpiX / 96.0f)) * scale;

    COLORREF idleCol = g_isDarkMode ? RGB(230, 230, 230) : RGB(40, 40, 40);
    COLORREF activeCol = RGB(255, 255, 255);
    int r = (int)roundf(GetRValue(idleCol) + (GetRValue(activeCol) - GetRValue(idleCol)) * hoverAlpha);
    int g = (int)roundf(GetGValue(idleCol) + (GetGValue(activeCol) - GetGValue(idleCol)) * hoverAlpha);
    int b = (int)roundf(GetBValue(idleCol) + (GetBValue(activeCol) - GetBValue(idleCol)) * hoverAlpha);
    BYTE xAlpha = (BYTE)roundf(255.0f * btnAlpha);

    float penWidth = ((1.4f * (float)g_dpiX) / 96.0f) * scale;
    if (penWidth < 1.0f) penWidth = 1.0f;

    Gdiplus::Pen xPen(Gdiplus::Color(xAlpha, r, g, b), penWidth);
    Gdiplus::LineCap cap = (btnRadius > 0) ? Gdiplus::LineCapRound : Gdiplus::LineCapFlat;
    xPen.SetStartCap(cap);
    xPen.SetEndCap(cap);

    graphics.DrawLine(&xPen, cx - glyphSpan, cy - glyphSpan, cx + glyphSpan, cy + glyphSpan);
    graphics.DrawLine(&xPen, cx + glyphSpan, cy - glyphSpan, cx - glyphSpan, cy + glyphSpan);
}

// Multi-pass Fluent elevation drop shadow behind a thumbnail. Drawn in the
// content layer below the DWM thumbnail, producing a subtle, soft ambient
// feathering with perfectly centered zero-offset distribution (cumulative alpha ~15% dark / ~10% light).
struct ThumbnailShadowPass {
    int baseSpread;     // Outward expansion in base pixels (at 96 DPI)
    int baseYOffset;    // Directional offset in base pixels (0 for centered)
    BYTE alphaDark;     // Pass alpha for dark mode (0-255)
    BYTE alphaLight;    // Pass alpha for light mode (0-255)
};

static const ThumbnailShadowPass kThumbnailShadowPasses[5] = {
    { 10, 0,  2, 1 },  // Pass 0: Wide subtle ambient feather
    {  7, 0,  3, 2 },  // Pass 1: Soft diffusion body
    {  5, 0,  4, 3 },  // Pass 2: Centered ambient body
    {  3, 0,  6, 4 },  // Pass 3: Centered core
    {  1, 0,  7, 5 }   // Pass 4: Subtle contact occlusion edge
};

static void DrawThumbnailShadow(HDC hdc, const RECT& rc, int cornerRadius, float alphaMult = 1.0f, float elevationScale = 1.0f) {
    if (alphaMult <= 0.01f) return;
    if (ThemeIs(L"none")) {
        alphaMult *= ((float)g_settings.opacity / 100.0f);
        if (alphaMult <= 0.01f) return;
    }
    int rcw = rc.right - rc.left;
    int rch = rc.bottom - rc.top;
    if (rcw <= 0 || rch <= 0) return;

    int shadowRadius = (std::min)(cornerRadius, DpiScale(12, g_dpiX));

    // Dynamic elevation lift factors when zoomed (WinUI 3 Fluent elevation standards)
    float spreadScale = 1.0f;
    if (elevationScale > 1.0001f) {
        float elevProg = (elevationScale - 1.0f) / SWS_HOVER_ZOOM_DELTA;
        if (elevProg < 0.0f) elevProg = 0.0f;
        if (elevProg > 1.0f) elevProg = 1.0f;
        spreadScale = 1.0f + elevProg * 0.15f;
    }

    // During active layout transition (250ms), render a single-pass ambient shadow
    // to maintain 144Hz frame pacing without CPU rasterization stutter.
    if (g_layoutTransition.active) {
        Gdiplus::Graphics gfx(hdc);
        gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        BYTE baseAlpha = g_isDarkMode ? 14 : 10;
        BYTE shadowAlpha = (BYTE)roundf(baseAlpha * alphaMult);
        if (shadowAlpha == 0) return;
        Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(shadowAlpha, 0, 0, 0));
        int sp = DpiScale((int)roundf(3.0f * spreadScale), g_dpiX);
        if (sp < 1) sp = 1;
        Gdiplus::REAL sx = (Gdiplus::REAL)(rc.left - sp);
        Gdiplus::REAL sy = (Gdiplus::REAL)(rc.top - sp);
        Gdiplus::REAL sw = (Gdiplus::REAL)(rcw + sp * 2);
        Gdiplus::REAL sh = (Gdiplus::REAL)(rch + sp * 2);
        if (shadowRadius > 0) {
            Gdiplus::REAL sd = (Gdiplus::REAL)(shadowRadius * 2 + sp * 2);
            if (sd > sw) sd = sw;
            if (sd > sh) sd = sh;
            Gdiplus::GraphicsPath sPath;
            sPath.AddArc(sx, sy, sd, sd, 180.0f, 90.0f);
            sPath.AddArc(sx + sw - sd, sy, sd, sd, 270.0f, 90.0f);
            sPath.AddArc(sx + sw - sd, sy + sh - sd, sd, sd, 0.0f, 90.0f);
            sPath.AddArc(sx, sy + sh - sd, sd, sd, 90.0f, 90.0f);
            sPath.CloseFigure();
            gfx.FillPath(&shadowBrush, &sPath);
        } else {
            gfx.FillRectangle(&shadowBrush, sx, sy, sw, sh);
        }
        return;
    }

    Gdiplus::Graphics gfx(hdc);
    gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    gfx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

    for (int i = 0; i < 5; ++i) {
        const auto& pass = kThumbnailShadowPasses[i];
        BYTE baseAlpha = g_isDarkMode ? pass.alphaDark : pass.alphaLight;
        BYTE shadowAlpha = (BYTE)roundf(baseAlpha * alphaMult);
        if (shadowAlpha == 0) continue;

        int sp = DpiScale((int)roundf(pass.baseSpread * spreadScale), g_dpiX);
        if (sp < 1) sp = 1;

        Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(shadowAlpha, 0, 0, 0));
        Gdiplus::REAL sx = (Gdiplus::REAL)(rc.left - sp);
        Gdiplus::REAL sy = (Gdiplus::REAL)(rc.top - sp);
        Gdiplus::REAL sw = (Gdiplus::REAL)(rcw + sp * 2);
        Gdiplus::REAL sh = (Gdiplus::REAL)(rch + sp * 2);
        if (shadowRadius > 0) {
            Gdiplus::REAL sd = (Gdiplus::REAL)(shadowRadius * 2 + sp * 2);
            if (sd > sw) sd = sw;
            if (sd > sh) sd = sh;
            Gdiplus::GraphicsPath sPath;
            sPath.AddArc(sx, sy, sd, sd, 180.0f, 90.0f);
            sPath.AddArc(sx + sw - sd, sy, sd, sd, 270.0f, 90.0f);
            sPath.AddArc(sx + sw - sd, sy + sh - sd, sd, sd, 0.0f, 90.0f);
            sPath.AddArc(sx, sy + sh - sd, sd, sd, 90.0f, 90.0f);
            sPath.CloseFigure();
            gfx.FillPath(&shadowBrush, &sPath);
        } else {
            gfx.FillRectangle(&shadowBrush, sx, sy, sw, sh);
        }
    }
}

// Shared drawing routine for both layered and buffered paint paths
static void DrawTaskEntry(HDC hdc, WindowEntry& e, HWND hWnd, int padLeft, int rowTitleH, int iconSz, int cornerRadius, int closeBtnReserve, bool isHovered, float alpha = 1.0f) {
    if (alpha <= 0.01f) return;

    // DrawText's rectangle clips negative side bearings and ClearType fringes.
    // Keep its logical alignment/ellipsis width, but clip ink to the card and
    // the caller's existing clip instead of cutting it at the advance bounds.
    auto drawTitle = [&](RECT rcText, UINT flags) {
        ABC firstGlyph = {}, lastGlyph = {};
        int leftOverhang = 1, rightOverhang = 1;
        if (e.title[0]) {
            if (GetCharABCWidthsW(hdc, e.title[0], e.title[0], &firstGlyph)) {
                leftOverhang += std::max(0, -firstGlyph.abcA);
            }
            WCHAR last = e.title[wcslen(e.title) - 1];
            if (GetCharABCWidthsW(hdc, last, last, &lastGlyph)) {
                rightOverhang += std::max(0, -lastGlyph.abcC);
            }
        }
        if (!(flags & (DT_CENTER | DT_RIGHT)) && e.title[0]) {
            // Only move a left-aligned origin when its measured ink would
            // escape the card (e.g. italic text with zero entry padding).
            rcText.left = std::max(rcText.left, e.rcCell.left + leftOverhang);
            if (rcText.right < rcText.left) rcText.right = rcText.left;
        }
        int savedDC = SaveDC(hdc);
        IntersectClipRect(hdc, e.rcCell.left, e.rcCell.top,
                          e.rcCell.right, e.rcCell.bottom);
        if (g_hTheme) {
            // The composited theme buffer can still cut off a tight measured
            // width despite DT_NOCLIP. Give its bitmap ink room, while the
            // callback retains the original alignment and ellipsis rectangle.
            struct InkInsets {
                int left, right;
                static int CALLBACK Draw(HDC dc, LPWSTR text, int count,
                                         LPRECT rect, UINT flags, LPARAM data) {
                    auto* insets = reinterpret_cast<const InkInsets*>(data);
                    RECT logical = *rect;
                    logical.left += insets->left;
                    logical.right -= insets->right;
                    return DrawTextW(dc, text, count, &logical, flags | DT_NOCLIP);
                }
            } insets = { leftOverhang, rightOverhang };
            DTTOPTS opts = { sizeof(DTTOPTS) };
            opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR | DTT_CALLBACK;
            opts.crText = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
            opts.pfnDrawTextCallback = InkInsets::Draw;
            opts.lParam = reinterpret_cast<LPARAM>(&insets);
            rcText.left -= leftOverhang;
            rcText.right += rightOverhang;
            DrawThemeTextEx(g_hTheme, hdc, 0, 0, e.title, -1,
                            flags | DT_NOCLIP, &rcText, &opts);
        } else {
            SetTextColor(hdc, g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT);
            DrawTextW(hdc, e.title, -1, &rcText, flags | DT_NOCLIP);
        }
        if (savedDC) RestoreDC(hdc, savedDC);
    };

    // Drop shadow behind the thumbnail (below the DWM thumbnail layer).
    // During active layout transition, shadows are rendered dynamically per-frame in PaintSwitcher to track resizing smoothly.
    if (g_settings.showThumbnails && g_settings.showThumbnailShadow && !g_layoutTransition.active &&
        !(e.rcThumbActual.left == 0 && e.rcThumbActual.right == 0 &&
          e.rcThumbActual.top == 0 && e.rcThumbActual.bottom == 0)) {
        DrawThumbnailShadow(hdc, e.rcThumbActual, cornerRadius, alpha, 1.0f);
    }

    if (g_settings.showThumbnails && cornerRadius > 0 && ThemeIs(L"none") && g_settings.opacity >= 99) {
        MaskRectCorners(hdc, e.rcThumbActual, cornerRadius);
    }

    // ---- Badge layout rendering path ----
    if (BadgeLayoutActive()) {
        // Badge title: draw centered in the header content rect
        if (g_settings.showTitle && e.title[0]) {
            RECT rcHeaderContent = GetHeaderContentRectForEntry(e);
            int titleH = GetHeaderTitleHeightPx();
            int headerTop = GetHeaderTopForEntry(e);
            RECT rcText = { rcHeaderContent.left, headerTop, rcHeaderContent.right, headerTop + titleH };
            if (alpha < 0.99f) {
                Gdiplus::Graphics gfx(hdc);
                gfx.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
                Gdiplus::Font font(hdc, g_hFont);
                COLORREF c = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
                BYTE textA = (BYTE)roundf(alpha * 255.0f);
                Gdiplus::SolidBrush brush(Gdiplus::Color(textA, GetRValue(c), GetGValue(c), GetBValue(c)));
                Gdiplus::StringFormat sf;
                sf.SetAlignment(Gdiplus::StringAlignmentCenter);
                sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
                sf.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
                sf.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
                Gdiplus::RectF r((Gdiplus::REAL)rcText.left, (Gdiplus::REAL)rcText.top,
                                (Gdiplus::REAL)(rcText.right - rcText.left), (Gdiplus::REAL)(rcText.bottom - rcText.top));
                gfx.DrawString(e.title, -1, &font, r, &sf, &brush);
            } else {
                drawTitle(rcText, DT_SINGLELINE | DT_CENTER | DT_VCENTER |
                                  DT_END_ELLIPSIS | DT_NOPREFIX);
            }
        }
        return;
    }

    // ---- Normal (non-badge) header content rendering ----
    int btnReserve = 0;
    bool isIconOnly = !g_settings.showThumbnails && !g_settings.showTitle && g_settings.showIcon;
    if (g_settings.showCloseButton && !HeaderIsVertical()) {
        if (!isIconOnly && !(g_settings.showThumbnails && ThumbnailIsSide())) {
            btnReserve = ((g_settings.centerTaskContent) || isHovered)
                     ? closeBtnReserve
                     : 0;
        }
    }
    RECT rcHeaderContent = GetHeaderContentRectForEntry(e);
    int contentLeft = rcHeaderContent.left;
    int contentRight = rcHeaderContent.right - btnReserve;
    if (contentRight < contentLeft) contentRight = contentLeft;

    int headerTop = GetHeaderTopForEntry(e);
    int iconX = contentLeft;
    int iconY = isIconOnly ? rcHeaderContent.top +
        (rcHeaderContent.bottom - rcHeaderContent.top - iconSz) / 2 :
        headerTop + (rowTitleH - iconSz) / 2;

    if (!HeaderIsVertical() && g_settings.showTitle && g_settings.showIcon) {
        int shift = (g_dpiY - 96) / 24;
        if (shift > 0) iconY += shift;
    }

    int textLeft = contentLeft;
    if (g_settings.showIcon) textLeft += iconSz + padLeft;
    int textRight = contentRight;
    int textTop = headerTop;
    int textBottom = textTop + rowTitleH;

    if (HeaderIsVertical() && !isIconOnly) {
        int availableW = contentRight - contentLeft;
        if (availableW < 0) availableW = 0;

        iconX = contentLeft + ((availableW > iconSz) ? (availableW - iconSz) / 2 : 0);
        iconY = headerTop;

        int headerGap = DpiScale(4, g_dpiY);
        int textH = GetHeaderTitleHeightPx();
        textTop = g_settings.showIcon ? (iconY + iconSz + headerGap) : iconY;
        textBottom = textTop + textH;
        textLeft = contentLeft;
        textRight = contentRight;
    } else if (g_settings.centerTaskContent) {
        int availableW = contentRight - contentLeft;
        if (availableW < 0) availableW = 0;

        int gap = padLeft;
        int textMaxW = availableW - (g_settings.showIcon ? iconSz + gap : 0);
        if (textMaxW < 0) textMaxW = 0;

        int textW = 0;
        if (g_settings.showTitle && textMaxW > 0 && e.title[0]) {
            RECT rcMeasure = { 0, 0, textMaxW, rowTitleH };
            DrawTextW(hdc, e.title, -1, &rcMeasure,
                      DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX | DT_CALCRECT);
            textW = rcMeasure.right - rcMeasure.left;
            if (textW < 0) textW = 0;
            if (textW > textMaxW) textW = textMaxW;
        }

        int blockW = (g_settings.showIcon ? iconSz : 0) + ((g_settings.showIcon && textW > 0) ? gap : 0) + textW;
        if (blockW < availableW) {
            iconX = contentLeft + (availableW - blockW) / 2;
        }

        textLeft = iconX + (g_settings.showIcon ? iconSz + ((textW > 0) ? gap : 0) : 0);
        textRight = textLeft + textW;
    }

    // Icon
    if (g_settings.showIcon && e.hIcon) {
        e.drawnIconX = iconX;
        e.drawnIconY = iconY;
        e.drawnIconSz = iconSz;

        float iconAlpha = alpha;
        bool isMin = g_settings.showMinimizedIndicator && IsEntryMinimized(e);
        if (isMin && MinimizedStyleUsesDimming()) {
            float minDim = (float)g_settings.minimizedIconOpacity / 100.0f;
            if (isHovered) { minDim = std::min(1.0f, minDim + 0.15f); }
            iconAlpha *= minDim;
        }

        if (iconAlpha < 0.99f) {
            DrawIconWithAlpha(hdc, iconX, iconY, e.hIcon, iconSz, iconAlpha);
        } else {
            DrawIconEx(hdc, iconX, iconY, e.hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
        }

        if (isMin && MinimizedStyleUsesBadge()) {
            int badgeSz = DpiScale(12, g_dpiX);
            int badgeX = iconX + iconSz - badgeSz + DpiScale(2, g_dpiX);
            int badgeY = iconY + iconSz - badgeSz + DpiScale(2, g_dpiY);
            Gdiplus::Graphics gfxBadge(hdc);
            gfxBadge.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            COLORREF bgC = g_isDarkMode ? RGB(40, 40, 40) : RGB(235, 235, 235);
            COLORREF fgC = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
            BYTE bAlpha = (BYTE)roundf(230.0f * (alpha < 1.0f ? alpha : 1.0f));
            Gdiplus::SolidBrush bgBrush(Gdiplus::Color(bAlpha, GetRValue(bgC), GetGValue(bgC), GetBValue(bgC)));
            gfxBadge.FillEllipse(&bgBrush, badgeX, badgeY, badgeSz, badgeSz);
            Gdiplus::Pen pen(Gdiplus::Color(bAlpha, GetRValue(fgC), GetGValue(fgC), GetBValue(fgC)), 1.5f);
            pen.SetStartCap(Gdiplus::LineCapRound);
            pen.SetEndCap(Gdiplus::LineCapRound);
            int lineW = DpiScale(5, g_dpiX);
            int lx1 = badgeX + (badgeSz - lineW) / 2;
            int lx2 = lx1 + lineW;
            int ly = badgeY + badgeSz / 2;
            gfxBadge.DrawLine(&pen, lx1, ly, lx2, ly);
        } else if (isMin && MinimizedStyleUsesDot()) {
            int dotSz = DpiScale(4, g_dpiX);
            int dotX = iconX + (iconSz - dotSz) / 2;
            int dotY = iconY + iconSz + DpiScale(2, g_dpiY);
            Gdiplus::Graphics gfxDot(hdc);
            gfxDot.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            COLORREF dotCol = g_isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
            BYTE dotAlpha = (BYTE)roundf((g_isDarkMode ? 180.0f : 140.0f) * (alpha < 1.0f ? alpha : 1.0f));
            Gdiplus::SolidBrush dotBrush(Gdiplus::Color(dotAlpha, GetRValue(dotCol), GetGValue(dotCol), GetBValue(dotCol)));
            gfxDot.FillEllipse(&dotBrush, dotX, dotY, dotSz, dotSz);
        }
    } else {
        e.drawnIconX = contentLeft;
        e.drawnIconY = headerTop;
        e.drawnIconSz = 0;
    }
    // Title text
    if (g_settings.showTitle) {
        RECT rcText = { textLeft, textTop, textRight, textBottom };
        if (rcText.right < rcText.left) rcText.right = rcText.left;
        if (alpha < 0.99f) {
            Gdiplus::Graphics gfx(hdc);
            gfx.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
            Gdiplus::Font font(hdc, g_hFont);
            COLORREF c = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
            BYTE textA = (BYTE)roundf(alpha * 255.0f);
            Gdiplus::SolidBrush brush(Gdiplus::Color(textA, GetRValue(c), GetGValue(c), GetBValue(c)));
            Gdiplus::StringFormat sf;
            sf.SetAlignment(HeaderIsVertical() ? Gdiplus::StringAlignmentCenter : Gdiplus::StringAlignmentNear);
            sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            sf.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
            sf.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
            Gdiplus::RectF r((Gdiplus::REAL)rcText.left, (Gdiplus::REAL)rcText.top,
                            (Gdiplus::REAL)(rcText.right - rcText.left), (Gdiplus::REAL)(rcText.bottom - rcText.top));
            gfx.DrawString(e.title, -1, &font, r, &sf, &brush);
        } else {
            drawTitle(rcText, DT_SINGLELINE |
                              (HeaderIsVertical() ? DT_CENTER : DT_VCENTER) |
                              DT_END_ELLIPSIS | DT_NOPREFIX);
        }
    }
}

static void FillSwitcherBackground(HDC hdc, const RECT& rect, bool fillBg) {
    if (rect.right <= rect.left || rect.bottom <= rect.top) return;
    RGBQUAD pixel = {};
    if (fillBg && ThemeIs(L"none")) {
        BYTE alpha = (BYTE)(std::max)(1, g_settings.opacity * 255 / 100);
        COLORREF color = GetBgColor();
        pixel = { (BYTE)(GetBValue(color) * alpha / 255),
                  (BYTE)(GetGValue(color) * alpha / 255),
                  (BYTE)(GetRValue(color) * alpha / 255), alpha };
    }
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bmi.bmiHeader.biHeight = 1;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    StretchDIBits(hdc, rect.left, rect.top, rect.right - rect.left,
                  rect.bottom - rect.top, 0, 0, 1, 1, &pixel, &bmi,
                  DIB_RGB_COLORS, SRCCOPY);
}

static void DrawDockEntryIcon(HDC hdc, const WindowEntry& e, int iconX,
                              int iconY, int iconSz, int index,
                              float itemAlpha) {
    bool emphasized = index >= 0 && (index == g_selectedIndex || index == g_hoverIndex);
    bool isMin = g_settings.showMinimizedIndicator && IsEntryMinimized(e);
    if (isMin && MinimizedStyleUsesDimming()) {
        float minDim = (float)g_settings.minimizedIconOpacity / 100.0f;
        if (emphasized) { minDim = std::min(1.0f, minDim + 0.15f); }
        itemAlpha *= minDim;
    }

    if (itemAlpha < 0.99f) {
        DrawIconWithAlpha(hdc, iconX, iconY, e.hIcon, iconSz, itemAlpha);
    } else {
        DrawIconEx(hdc, iconX, iconY, e.hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
    }

    if (isMin && MinimizedStyleUsesBadge()) {
        int badgeSz = DpiScale(14, g_dpiX);
        int badgeX = iconX + iconSz - badgeSz + DpiScale(2, g_dpiX);
        int badgeY = iconY + iconSz - badgeSz + DpiScale(2, g_dpiY);
        Gdiplus::Graphics gfxBadge(hdc);
        gfxBadge.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        COLORREF bgC = g_isDarkMode ? RGB(40, 40, 40) : RGB(235, 235, 235);
        COLORREF fgC = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
        BYTE bAlpha = (BYTE)roundf(230.0f * (itemAlpha < 1.0f ? itemAlpha : 1.0f));
        Gdiplus::SolidBrush bgBrush(Gdiplus::Color(bAlpha, GetRValue(bgC), GetGValue(bgC), GetBValue(bgC)));
        gfxBadge.FillEllipse(&bgBrush, badgeX, badgeY, badgeSz, badgeSz);
        Gdiplus::Pen pen(Gdiplus::Color(bAlpha, GetRValue(fgC), GetGValue(fgC), GetBValue(fgC)), 1.5f);
        pen.SetStartCap(Gdiplus::LineCapRound);
        pen.SetEndCap(Gdiplus::LineCapRound);
        int lineW = DpiScale(6, g_dpiX);
        int lx1 = badgeX + (badgeSz - lineW) / 2;
        int lx2 = lx1 + lineW;
        int ly = badgeY + badgeSz / 2;
        gfxBadge.DrawLine(&pen, lx1, ly, lx2, ly);
    } else if (isMin && MinimizedStyleUsesDot()) {
        int dotW = DpiScale(10, g_dpiX);
        int dotH = DpiScale(3, g_dpiY);
        int dotX = iconX + (iconSz - dotW) / 2;
        int dotY = DockIconIsTop() ? (iconY + iconSz + DpiScale(3, g_dpiY)) : (iconY - dotH - DpiScale(3, g_dpiY));
        Gdiplus::Graphics gfxDot(hdc);
        gfxDot.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        COLORREF dotCol = g_isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
        BYTE dotAlpha = (BYTE)roundf((g_isDarkMode ? 160.0f : 130.0f) * (itemAlpha < 1.0f ? itemAlpha : 1.0f));
        if (emphasized) dotAlpha = (BYTE)roundf(230.0f * (itemAlpha < 1.0f ? itemAlpha : 1.0f));
        Gdiplus::SolidBrush dotBrush(Gdiplus::Color(dotAlpha, GetRValue(dotCol), GetGValue(dotCol), GetBValue(dotCol)));
        Gdiplus::GraphicsPath dotPath;
        dotPath.AddArc((Gdiplus::REAL)dotX, (Gdiplus::REAL)dotY, (Gdiplus::REAL)dotH, (Gdiplus::REAL)dotH, 90, 180);
        dotPath.AddArc((Gdiplus::REAL)(dotX + dotW - dotH), (Gdiplus::REAL)dotY, (Gdiplus::REAL)dotH, (Gdiplus::REAL)dotH, 270, 180);
        dotPath.CloseFigure();
        gfxDot.FillPath(&dotBrush, &dotPath);
    }
}

// Rebuild just the strip above the cached background. The highlight is always
// below every icon; icons follow their own cells and only the viewport/reflow
// moves them. Cached icon pixels must be cleared before compositing the fill.
static void DrawDockIconStrip(HDC hdc) {
    int saved = SaveDC(hdc);
    IntersectClipRect(hdc, g_rcDockIconStrip.left, g_rcDockIconStrip.top,
                      g_rcDockIconStrip.right, g_rcDockIconStrip.bottom);
    FillSwitcherBackground(hdc, g_rcDockIconStrip, ShouldFillBackground());
    if (HighlightHasFill() && g_selectedIndex >= 0 &&
        g_selectedIndex < (int)g_windows.size()) {
        DrawSelectionFillF(hdc, SelectionRectWithViewport());
    }

    DockPaintSpace transitionPaint = {};
    const DockPaintSpace* paintSpace = nullptr;
    if (UseDockScreenProjection()) {
        transitionPaint = GetDockPaintSpace();
        paintSpace = &transitionPaint;
    }
    int offX = (int)roundf(g_scrollTransition.offsetCurrentX);
    int iconSz = DpiScale(g_settings.dockIconSize > 0 ? g_settings.dockIconSize : 48, g_dpiX);
    if (g_scrollTransition.active) {
        for (const auto& snap : g_scrollTransition.outgoingItems) {
            int index = FindWindowIndexByHwnd(snap.hWnd);
            if (index >= 0 && !IsWindowTruncated(index)) continue;
            WindowEntry entry = {};
            entry.hWnd = snap.hWnd;
            entry.hIcon = snap.hIcon;
            entry.groupWindows = snap.groupWindows;
            RECT cell = snap.rcCell;
            if (!HasLayoutRect(cell)) continue;
            int dx = offX - g_scrollTransition.travelDistanceX;
            POINT icon = paintSpace
                             ? DockScreenSnapIcon(ToRectF(cell), iconSz, dx, 0,
                                                  *paintSpace)
                             : DockLegacyIcon(cell, iconSz, dx, 0);
            DrawDockEntryIcon(hdc, entry, icon.x, icon.y, iconSz, index,
                              snap.alpha);
        }
    }
    for (int i = 0; i < (int)g_windows.size(); i++) {
        auto& entry = g_windows[i];
        if (IsWindowTruncated(i)) continue;
        RectF paintCell = DockEntryCellForPaint(entry, paintSpace);
        POINT unscrolledIcon = paintSpace
                                   ? DockScreenSnapIcon(paintCell, iconSz, 0,
                                                        0, *paintSpace)
                                   : DockLegacyIcon(entry.rcCell, iconSz, 0, 0);
        POINT icon = paintSpace
                         ? DockScreenSnapIcon(paintCell, iconSz, offX, 0,
                                              *paintSpace)
                         : DockLegacyIcon(entry.rcCell, iconSz, offX, 0);
        entry.drawnIconX = unscrolledIcon.x;
        entry.drawnIconY = unscrolledIcon.y;
        entry.drawnIconSz = iconSz;
        float alpha = (g_layoutTransition.active && entry.isNewEntry) ? entry.enterAlpha : 1.0f;
        DrawDockEntryIcon(hdc, entry, icon.x, icon.y, iconSz, i, alpha);
    }

    if (g_layoutTransition.active && !g_layoutTransition.departingItems.empty()) {
        for (const auto& dep : g_layoutTransition.departingItems) {
            if (dep.alpha <= 0.01f || !dep.hIcon) continue;
            int cellW = dep.rcCellCurrent.right - dep.rcCellCurrent.left;
            int cellH = dep.rcCellCurrent.bottom - dep.rcCellCurrent.top;
            if (cellW <= 0 || cellH <= 0) continue;
            RectF paintCell = DockDepartingCellForPaint(dep, paintSpace);
            POINT icon = paintSpace
                             ? DockScreenSnapIcon(paintCell, iconSz, offX, 0,
                                                  *paintSpace)
                             : DockLegacyIcon(dep.rcCellCurrent, iconSz, offX,
                                              0);
            DrawIconWithAlpha(hdc, icon.x, icon.y, dep.hIcon, iconSz,
                              dep.alpha);
        }
    }
    RestoreDC(hdc, saved);
}

static void DrawDockContentInner(HDC hdc, bool fillBg, HWND hWnd, bool includeSelectionFill) {
    RECT rcClient; GetClientRect(g_hSwitcher, &rcClient);
    int w = rcClient.right;

    if (fillBg && ThemeIs(L"none")) {
        FillSwitcherBackground(hdc, rcClient, true);
    }

    HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFont);
    SetBkMode(hdc, TRANSPARENT);

    int masterPadX = DpiScale(g_settings.switcherPadding, g_dpiX);
    int cornerRadius = GetThumbnailCornerRadiusPx();

    // Central Preview shadow / card backdrop
    bool showPreview = DockShowPreview();
    if (showPreview && g_rcCentralPreview.right > g_rcCentralPreview.left &&
        g_rcCentralPreview.bottom > g_rcCentralPreview.top) {
        RECT shadowRc = g_rcCentralPreview;
        float shadowAlphaMult = 1.0f;
        if (g_dockPreviewSlide.active) {
            int offX = (int)roundf(g_dockPreviewSlide.currentOffset);
            shadowRc.left += offX;
            shadowRc.right += offX;
            shadowAlphaMult = g_dockPreviewSlide.currentAlpha;
        }
        // Active preview reflow/slide shadows are submitted dynamically by PaintSwitcher.
        if (g_settings.showThumbnailShadow && shadowAlphaMult > 0.01f && !g_layoutTransition.active && !g_dockPreviewSlide.active) {
            DrawThumbnailShadow(hdc, shadowRc, cornerRadius, shadowAlphaMult);
        }
        if (cornerRadius > 0 && ThemeIs(L"none") && g_settings.opacity >= 99) {
            MaskRectCorners(hdc, shadowRc, cornerRadius);
        }
    }

    // 1px subtle divider between the icon strip and content area
    {
        Gdiplus::Graphics gfx(hdc);
        gfx.SetSmoothingMode(Gdiplus::SmoothingModeNone);
        COLORREF divCol = g_isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
        BYTE divAlpha = g_isDarkMode ? 24 : 18;
        Gdiplus::SolidBrush divBrush(Gdiplus::Color(divAlpha, GetRValue(divCol), GetGValue(divCol), GetBValue(divCol)));

        int divX = masterPadX + DpiScale(12, g_dpiX);
        int divW = w - 2 * (masterPadX + DpiScale(12, g_dpiX));
        if (divW > 0 && (showPreview || g_settings.showTitle)) {
            if (DockIconIsTop()) {
                int divY = g_rcDockIconStrip.bottom + DpiScale(12, g_dpiY);
                gfx.FillRectangle(&divBrush, divX, divY, divW, 1);
            } else {
                int divY = g_rcDockIconStrip.top - DpiScale(12, g_dpiY) - 1;
                gfx.FillRectangle(&divBrush, divX, divY, divW, 1);
            }
        }
    }

    // Static/scroll canvases hold only the backdrop, preview and title. Icons
    // are composited once at presentation, never copied from an old strip row
    // while a bottom-positioned Dock is resizing.
    if (includeSelectionFill) DrawDockIconStrip(hdc);

    // 5. Draw centered window title in g_rcDockTitleBar
    if (g_settings.showTitle && g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        const auto& selWnd = g_windows[g_selectedIndex];
        DockPaintSpace transitionPaint = {};
        const DockPaintSpace* paintSpace = nullptr;
        if (UseDockScreenProjection()) {
            transitionPaint = GetDockPaintSpace();
            paintSpace = &transitionPaint;
        }
        RECT rcText = DockTitleTextRectForPaint(paintSpace);
        float titleAlpha = 1.0f;
        if (g_dockPreviewSlide.active) {
            titleAlpha = g_dockPreviewSlide.currentAlpha;
        }

        if (titleAlpha < 0.99f) {
            Gdiplus::Graphics gfx(hdc);
            gfx.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
            COLORREF tc = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
            BYTE alphaVal = (BYTE)roundf(255.0f * titleAlpha);
            Gdiplus::SolidBrush tb(Gdiplus::Color(alphaVal, GetRValue(tc), GetGValue(tc), GetBValue(tc)));
            Gdiplus::Font gdiFont(hdc, g_hFont);
            Gdiplus::StringFormat sf;
            sf.SetAlignment(Gdiplus::StringAlignmentCenter);
            sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            sf.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
            sf.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
            Gdiplus::RectF lRc((float)rcText.left, (float)rcText.top, (float)(rcText.right - rcText.left), (float)(rcText.bottom - rcText.top));
            gfx.DrawString(selWnd.title, -1, &gdiFont, lRc, &sf, &tb);
        } else {
            if (g_hTheme) {
                DTTOPTS opts = { sizeof(DTTOPTS) };
                opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
                opts.crText = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
                DrawThemeTextEx(g_hTheme, hdc, 0, 0, selWnd.title, -1,
                    DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX, &rcText, &opts);
            } else {
                SetTextColor(hdc, g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT);
                DrawTextW(hdc, selWnd.title, -1, &rcText,
                          DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
            }
        }
    }

    SelectObject(hdc, hOldFont);
}

// Shared drawing routine for both layered and buffered paint paths
static void DrawSwitcherContentInner(HDC hdc, bool fillBg, HWND hWnd, bool includeSelectionFill) {
    if (DockLayoutActive()) {
        DrawDockContentInner(hdc, fillBg, hWnd, includeSelectionFill);
        return;
    }

    RECT rcClient; GetClientRect(g_hSwitcher, &rcClient);
    int w = rcClient.right, h = rcClient.bottom;

    if (fillBg && ThemeIs(L"none")) {
        FillSwitcherBackground(hdc, rcClient, true);
    }

    HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFont);
    SetBkMode(hdc, TRANSPARENT);

    // Draw animated selection background fill (underneath thumbnails)
    if (includeSelectionFill && g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        if (HighlightHasFill()) {
            DrawSelectionFillF(hdc, SelectionRectWithViewport());
        }
    }

    // DPI-scale layout constants for drawing
    int padLeft    = DpiScale(g_settings.entryPadding, g_dpiX);
    int rowTitleH  = GetHeaderRowHeightPx();
    int iconSz     = GetHeaderIconSizePx();
    int cornerRadius = GetThumbnailCornerRadiusPx();
    int closeBtnReserve = DpiScale(24, g_dpiX) + padLeft;

    int masterPadX = DpiScale(g_settings.switcherPadding, g_dpiX);
    int masterPadY = DpiScale(g_settings.switcherPadding, g_dpiY);

    // Restrict cards to content area (inside master padding)
    RECT rcClip = { masterPadX, masterPadY, w - masterPadX, h - masterPadY };
    HRGN hContentClip = CreateRectRgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
    HRGN hSavedClip = CreateRectRgn(0, 0, 0, 0);
    int clipState = GetClipRgn(hdc, hSavedClip);
    ExtSelectClipRgn(hdc, hContentClip, RGN_AND);
    DeleteObject(hContentClip);

    // 1. Draw departing items if layout transition is active
    if (g_layoutTransition.active && !g_layoutTransition.departingItems.empty()) {
        for (const auto& dep : g_layoutTransition.departingItems) {
            if (dep.alpha <= 0.01f) continue;
            WindowEntry e = {};
            e.rcCell = dep.rcCellCurrent;
            e.rcThumbActual = dep.rcThumbCurrent;
            e.rcThumbSlot = dep.rcThumbCurrent;
            e.hIcon = dep.hIcon;
            wcsncpy_s(e.title, dep.title, _TRUNCATE);
            DrawTaskEntry(hdc, e, hWnd, padLeft, rowTitleH, iconSz, cornerRadius, closeBtnReserve, false, dep.alpha);
        }
    }

    // 2. Draw current items
    for (int i = 0; i < (int)g_windows.size(); i++) {
        auto& e = g_windows[i];

        // Skip truncated (not placed) windows
        if (e.rcCell.left == 0 && e.rcCell.right == 0 &&
            e.rcCell.top == 0 && e.rcCell.bottom == 0) continue;

        bool isHovered = (i == g_hoverIndex && g_hoverWnd == hWnd);
        float itemAlpha = (g_layoutTransition.active && e.isNewEntry) ? e.enterAlpha : 1.0f;
        DrawTaskEntry(hdc, e, hWnd, padLeft, rowTitleH, iconSz, cornerRadius, closeBtnReserve, isHovered, itemAlpha);
    }

    // Restore clip
    if (clipState == 1) {
        SelectClipRgn(hdc, hSavedClip);
    } else {
        SelectClipRgn(hdc, NULL);
    }
    DeleteObject(hSavedClip);

    SelectObject(hdc, hOldFont);
}

static void DrawSwitcherContent(HDC hdc, bool fillBg, HWND hWnd) {
    DrawSwitcherContentInner(hdc, fillBg, hWnd, true);
}

static void DrawSwitcherStaticContent(HDC hdc, bool fillBg, HWND hWnd) {
    DrawSwitcherContentInner(hdc, fillBg, hWnd, false);
}


// Rendering

static void DrawBadgeIconOverlay(HDC hdc, const RECT& rcThumbActual, HICON hIcon, int* pOutIconX = NULL, int* pOutIconY = NULL, int* pOutIconSz = NULL, bool isMinimized = false, HWND hWnd = NULL, float motionAlpha = 1.0f,
                                 std::shared_ptr<OwnedWindowIcon> iconOwner = {}) {
    if (!BadgeLayoutActive() || !g_settings.showIcon || !hIcon) return;
    int iconSz = GetHeaderIconSizePx();
    int thumbW = rcThumbActual.right - rcThumbActual.left;
    int thumbHt = rcThumbActual.bottom - rcThumbActual.top;
    if (thumbW <= 0 || thumbHt <= 0) return;

    int badgePad = DpiScale(g_settings.badgeIconPadding, g_dpiX);
    int bIconX = 0, bIconY = 0;

    // Horizontal positioning
    if (BadgeIconPositionIs(L"topLeft") || BadgeIconPositionIs(L"centerLeft") || BadgeIconPositionIs(L"bottomLeft")) {
        bIconX = rcThumbActual.left + badgePad;
    } else if (BadgeIconPositionIs(L"topRight") || BadgeIconPositionIs(L"centerRight") || BadgeIconPositionIs(L"bottomRight")) {
        bIconX = rcThumbActual.right - iconSz - badgePad;
    } else {
        bIconX = rcThumbActual.left + (thumbW - iconSz) / 2;
    }
    // Vertical positioning
    if (BadgeIconPositionIs(L"topLeft") || BadgeIconPositionIs(L"topCenter") || BadgeIconPositionIs(L"topRight")) {
        bIconY = rcThumbActual.top + badgePad;
    } else if (BadgeIconPositionIs(L"bottomLeft") || BadgeIconPositionIs(L"bottomCenter") || BadgeIconPositionIs(L"bottomRight")) {
        bIconY = rcThumbActual.bottom - iconSz - badgePad;
    } else {
        bIconY = rcThumbActual.top + (thumbHt - iconSz) / 2;
    }

    bIconX += DpiScale(g_settings.badgeIconOffsetX, g_dpiX);
    bIconY += DpiScale(g_settings.badgeIconOffsetY, g_dpiY);

    if (pOutIconX) *pOutIconX = bIconX;
    if (pOutIconY) *pOutIconY = bIconY;
    if (pOutIconSz) *pOutIconSz = iconSz;
    motionAlpha = (std::clamp)(motionAlpha, 0.0f, 1.0f);
    if (motionAlpha <= 0.001f) return;

    bool isMin = g_settings.showMinimizedIndicator && isMinimized;
    float iconDim = 1.0f;
    if (isMin && MinimizedStyleUsesDimming()) {
        iconDim = (float)g_settings.minimizedIconOpacity / 100.0f;
    }

    Gdiplus::Graphics gfx(hdc);
    gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    gfx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

    if (g_settings.showBadgeIconBackground) {
        int bgSize = iconSz + badgePad * 2;
        int bgX = bIconX - badgePad;
        int bgY = bIconY - badgePad;
        
        COLORREF bgC = GetIconBackgroundColor();
        int op = g_isDarkMode ? g_settings.iconBgOpacityDark : g_settings.iconBgOpacityLight;
        float baseAlpha = (float)op * 2.55f;
        int alpha = (int)roundf(baseAlpha * (isMin && MinimizedStyleUsesDimming() ? iconDim : 1.0f) * motionAlpha);
        if (alpha > 255) alpha = 255;
        if (alpha < 0) alpha = 0;
        Gdiplus::SolidBrush bgBrush(Gdiplus::Color(alpha, GetRValue(bgC), GetGValue(bgC), GetBValue(bgC)));
        
        Gdiplus::REAL r = (Gdiplus::REAL)GetBadgeIconBackgroundCornerRadiusPx(bgSize / 2);
        
        if (g_settings.showBadgeIconBackgroundShadow) {
            for (int pass = 5; pass > 0; --pass) {
                int shadowAlpha = 15 - (pass * 2);
                if (shadowAlpha < 1) shadowAlpha = 1;
                shadowAlpha = (int)roundf(shadowAlpha * motionAlpha);
                Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(shadowAlpha, 0, 0, 0));
                int sp = pass;
                if (r > 0) {
                    Gdiplus::GraphicsPath sPath;
                    Gdiplus::REAL sw = (Gdiplus::REAL)(bgSize + sp * 2), sh = sw;
                    Gdiplus::REAL sx = (Gdiplus::REAL)(bgX - sp), sy = (Gdiplus::REAL)(bgY - sp + 1);
                    Gdiplus::REAL sd = r * 2 + sp * 2;
                    if (sd > sw) sd = sw;
                    if (sd > sh) sd = sh;
                    sPath.AddArc(sx, sy, sd, sd, 180, 90);
                    sPath.AddArc(sx + sw - sd, sy, sd, sd, 270, 90);
                    sPath.AddArc(sx + sw - sd, sy + sh - sd, sd, sd, 0, 90);
                    sPath.AddArc(sx, sy + sh - sd, sd, sd, 90, 90);
                    sPath.CloseFigure();
                    gfx.FillPath(&shadowBrush, &sPath);
                } else {
                    gfx.FillRectangle(&shadowBrush, bgX - sp, bgY - sp + 1, bgSize + sp * 2, bgSize + sp * 2);
                }
            }
        }

        if (r > 0) {
            Gdiplus::GraphicsPath path;
            Gdiplus::REAL w = (Gdiplus::REAL)bgSize, h = (Gdiplus::REAL)bgSize;
            Gdiplus::REAL x = (Gdiplus::REAL)bgX, y = (Gdiplus::REAL)bgY;
            Gdiplus::REAL d = r * 2;
            if (d > w) d = w;
            if (d > h) d = h;
            path.AddArc(x, y, d, d, 180, 90);
            path.AddArc(x + w - d, y, d, d, 270, 90);
            path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
            path.AddArc(x, y + h - d, d, d, 90, 90);
            path.CloseFigure();
            gfx.FillPath(&bgBrush, &path);
        } else {
            gfx.FillRectangle(&bgBrush, bgX, bgY, bgSize, bgSize);
        }
        if (iconDim * motionAlpha < 0.99f) {
            DrawIconWithAlpha(hdc, bIconX, bIconY, hIcon, iconSz, iconDim * motionAlpha);
        } else {
            DrawIconEx(hdc, bIconX, bIconY, hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
        }
    } else {
        bool drawShadow = IsWin11OrGreater() || g_settings.showThumbnailShadow;
        if (drawShadow) {
            auto shadow = GetWindowIconShadow(hIcon, iconSz, iconOwner);
            Gdiplus::Bitmap* pBmp = shadow ? shadow->bitmap.get() : nullptr;
            if (pBmp) {
                int dx[] = { 0, 1, 0, -1, 1 };
                int dy[] = { 1, 0, -1, 0, 1 };
                for (int p = 0; p < 5; ++p) {
                    int x = bIconX + DpiScale(dx[p], g_dpiX);
                    int y = bIconY + DpiScale(dy[p] + 2, g_dpiY);
                    if (motionAlpha < 1.0f) {
                        Gdiplus::ColorMatrix matrix = {{
                            {1, 0, 0, 0, 0},
                            {0, 1, 0, 0, 0},
                            {0, 0, 1, 0, 0},
                            {0, 0, 0, motionAlpha, 0},
                            {0, 0, 0, 0, 1}
                        }};
                        Gdiplus::ImageAttributes attributes;
                        attributes.SetColorMatrix(&matrix);
                        gfx.DrawImage(pBmp, Gdiplus::Rect(x, y, iconSz, iconSz),
                                      0, 0, iconSz, iconSz, Gdiplus::UnitPixel, &attributes);
                    } else {
                        gfx.DrawImage(pBmp, x, y, iconSz, iconSz);
                    }
                }
                // The local record pins the bitmap and icon through this draw,
                // even if a reentrant UI callback removes its cache entry.
            }
        }
        if (iconDim * motionAlpha < 0.99f) {
            DrawIconWithAlpha(hdc, bIconX, bIconY, hIcon, iconSz, iconDim * motionAlpha);
        } else {
            DrawIconEx(hdc, bIconX, bIconY, hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
        }
    }

    if (isMin && MinimizedStyleUsesBadge()) {
        int badgeSz = DpiScale(12, g_dpiX);
        int badgeX = bIconX + iconSz - badgeSz + DpiScale(2, g_dpiX);
        int badgeY = bIconY + iconSz - badgeSz + DpiScale(2, g_dpiY);
        Gdiplus::Graphics gfxBadge(hdc);
        gfxBadge.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        COLORREF bgC = g_isDarkMode ? RGB(40, 40, 40) : RGB(235, 235, 235);
        COLORREF fgC = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
        BYTE bAlpha = (BYTE)roundf(230 * motionAlpha);
        Gdiplus::SolidBrush bgBrush(Gdiplus::Color(bAlpha, GetRValue(bgC), GetGValue(bgC), GetBValue(bgC)));
        gfxBadge.FillEllipse(&bgBrush, badgeX, badgeY, badgeSz, badgeSz);
        Gdiplus::Pen pen(Gdiplus::Color(bAlpha, GetRValue(fgC), GetGValue(fgC), GetBValue(fgC)), 1.5f);
        pen.SetStartCap(Gdiplus::LineCapRound);
        pen.SetEndCap(Gdiplus::LineCapRound);
        int lineW = DpiScale(5, g_dpiX);
        int lx1 = badgeX + (badgeSz - lineW) / 2;
        int lx2 = lx1 + lineW;
        int ly = badgeY + badgeSz / 2;
        gfxBadge.DrawLine(&pen, lx1, ly, lx2, ly);
    } else if (isMin && MinimizedStyleUsesDot()) {
        int dotSz = DpiScale(4, g_dpiX);
        int dotX = bIconX + (iconSz - dotSz) / 2;
        int dotY = bIconY + iconSz + DpiScale(2, g_dpiY);
        Gdiplus::Graphics gfxDot(hdc);
        gfxDot.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        COLORREF dotCol = g_isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
        BYTE dotAlpha = (BYTE)roundf((g_isDarkMode ? 180 : 140) * motionAlpha);
        Gdiplus::SolidBrush dotBrush(Gdiplus::Color(dotAlpha, GetRValue(dotCol), GetGValue(dotCol), GetBValue(dotCol)));
        gfxDot.FillEllipse(&dotBrush, dotX, dotY, dotSz, dotSz);
    }
}

static void DrawSwitcherOuterBorder(HDC hdc, int w, int h, int radiusPx) {
    if (w <= 0 || h <= 0) return;

    Gdiplus::Graphics graphics(hdc);

    BYTE borderAlpha = g_isDarkMode ? 28 : 24;
    COLORREF borderCol = g_isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);

    const WCHAR* borderMode = g_isDarkMode ? g_settings.borderColorModeDark : g_settings.borderColorModeLight;
    if (wcscmp(borderMode, L"accent") == 0 || wcscmp(borderMode, L"custom") == 0) {
        borderCol = GetContourColor();
        borderAlpha = 90; // ~35% alpha for custom/accent outline
    }

    BYTE r = GetRValue(borderCol);
    BYTE g = GetGValue(borderCol);
    BYTE b = GetBValue(borderCol);

    if (radiusPx <= 0) {
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);
        Gdiplus::Pen pen(Gdiplus::Color(borderAlpha, r, g, b), 1.0f);
        pen.SetLineJoin(Gdiplus::LineJoinMiter);
        graphics.DrawRectangle(&pen, 0, 0, w - 1, h - 1);
    } else {
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        Gdiplus::Pen pen(Gdiplus::Color(borderAlpha, r, g, b), 1.0f);
        pen.SetLineJoin(Gdiplus::LineJoinRound);

        Gdiplus::REAL left = 0.5f;
        Gdiplus::REAL top = 0.5f;
        Gdiplus::REAL right = (Gdiplus::REAL)w - 0.5f;
        Gdiplus::REAL bottom = (Gdiplus::REAL)h - 0.5f;
        Gdiplus::REAL rad = (Gdiplus::REAL)radiusPx - 0.5f;
        if (rad * 2.0f > (right - left)) rad = (right - left) * 0.5f;
        if (rad * 2.0f > (bottom - top)) rad = (bottom - top) * 0.5f;

        if (rad > 0.0f) {
            Gdiplus::REAL d = rad * 2.0f;
            Gdiplus::GraphicsPath path;
            path.StartFigure();
            path.AddArc(left, top, d, d, 180.0f, 90.0f);
            path.AddArc(right - d, top, d, d, 270.0f, 90.0f);
            path.AddArc(right - d, bottom - d, d, d, 0.0f, 90.0f);
            path.AddArc(left, bottom - d, d, d, 90.0f, 90.0f);
            path.CloseFigure();
            graphics.DrawPath(&pen, &path);
        } else {
            graphics.DrawRectangle(&pen, left, top, right - left, bottom - top);
        }
    }
}

static void DrawSwitcherOverlay(HDC hdc, HWND hWnd) {
    if (g_windows.empty()) return;

    int offX = (int)roundf(g_scrollTransition.offsetCurrentX);
    int offY = (int)roundf(g_scrollTransition.offsetCurrentY);

    int cornerRadius = GetThumbnailCornerRadiusPx();

    RECT rcClient; GetClientRect(g_hSwitcher, &rcClient);
    int masterPadX = DpiScale(g_settings.switcherPadding, g_dpiX);
    int masterPadY = DpiScale(g_settings.switcherPadding, g_dpiY);
    RECT rcClip = { masterPadX, masterPadY, rcClient.right - masterPadX, rcClient.bottom - masterPadY };

    // 1. Draw corner masks and badge icons for outgoing items during scroll transition
    if (g_scrollTransition.active && !g_scrollTransition.outgoingItems.empty()) {
        int outOffX = offX - g_scrollTransition.travelDistanceX;
        int outOffY = offY - g_scrollTransition.travelDistanceY;
        for (const auto& snap : g_scrollTransition.outgoingItems) {
            int curIdx = FindWindowIndexByHwnd(snap.hWnd);
            if (curIdx != -1 && !IsWindowTruncated(curIdx)) continue; // Items still visible are handled in incoming loop below

            RECT snapCell = snap.rcCell;
            RECT snapThumb = snap.rcThumbActual;
            OffsetRect(&snapCell, outOffX, outOffY);
            OffsetRect(&snapThumb, outOffX, outOffY);

            RECT rcIntersect;
            if (!IntersectRect(&rcIntersect, &rcClip, &snapCell)) continue;

            bool collidesWithIncoming = false;
            for (int k = 0; k < (int)g_windows.size(); k++) {
                if (IsWindowTruncated(k)) continue;
                RECT inCell = g_windows[k].rcCell;
                if (offX != 0 || offY != 0) OffsetRect(&inCell, offX, offY);
                if (EqualRect(&snapCell, &inCell)) {
                    collidesWithIncoming = true;
                    break;
                }
            }
            if (collidesWithIncoming) continue;

            if (g_settings.showThumbnails && cornerRadius > 0 && ThemeIs(L"none") && g_settings.opacity >= 99) {
                COLORREF maskColor = GetBgColor();
                MaskRectCorners(hdc, snapThumb, cornerRadius, true, maskColor);
            }

            DrawBadgeIconOverlay(hdc, snapThumb, snap.hIcon, NULL, NULL, NULL, false, snap.hWnd, snap.alpha,
                                 snap.iconCell ? snap.iconCell->icon : nullptr);
        }
    }

    // 2. Draw departing items if layout transition is active
    if (g_layoutTransition.active && !g_layoutTransition.departingItems.empty()) {
        for (const auto& dep : g_layoutTransition.departingItems) {
            if (dep.alpha <= 0.01f) continue;
            RECT depThumb = dep.rcThumbCurrent;

            if (g_settings.showThumbnails && cornerRadius > 0 && ThemeIs(L"none") && g_settings.opacity >= 99) {
                COLORREF maskColor = GetBgColor();
                MaskRectCorners(hdc, depThumb, cornerRadius, true, maskColor);
            }
            DrawBadgeIconOverlay(hdc, depThumb, dep.hIcon, NULL, NULL, NULL, false, dep.hWnd, dep.alpha,
                                 dep.iconCell ? dep.iconCell->icon : nullptr);
        }
    }

    // 3. Incoming / current items
    for (int idx = 0; idx < (int)g_windows.size(); idx++) {
        int i = (g_layoutStartIndex + idx) % g_windows.size();
        const auto& e = g_windows[i];
        if (IsWindowTruncated(i)) continue;

        RECT rcThumbActual = (ThumbnailHoverIsZoom() && e.hoverScale > 1.0001f)
                             ? GetScaledThumbRect(e)
                             : e.rcThumbActual;
        RECT rcCell = e.rcCell;
        RECT rcThumbSlot = e.rcThumbSlot;

        if (offX != 0 || offY != 0) {
            OffsetRect(&rcCell, offX, offY);
            OffsetRect(&rcThumbActual, offX, offY);
            OffsetRect(&rcThumbSlot, offX, offY);
        }

        if (g_settings.showThumbnails && cornerRadius > 0 && ThemeIs(L"none") && g_settings.opacity >= 99) {
            COLORREF maskColor = GetBgColor();
            if (i == g_selectedIndex && HighlightHasFill() && !DockLayoutActive()) {
                maskColor = GetHighlightFillColor();
            }
            MaskRectCorners(hdc, rcThumbActual, cornerRadius, true, maskColor);
        }

        // Badge layout: draw icon overlay on thumbnail
        int drawnIconX = e.drawnIconX + offX;
        int drawnIconY = e.drawnIconY + offY;
        int drawnIconSz = e.drawnIconSz;
        if (!DockLayoutActive()) {
            DrawBadgeIconOverlay(hdc, rcThumbActual, e.hIcon, &drawnIconX, &drawnIconY, &drawnIconSz, IsEntryMinimized(e), e.hWnd,
                                 (g_layoutTransition.active && e.isNewEntry) ? e.enterAlpha : 1.0f,
                                 e.iconCell ? e.iconCell->icon : nullptr);
        }

        // Close button (rendered for any entry with closeBtnAlpha > 0.01f, enabling smooth cross-fades between entries)
        if (g_settings.showCloseButton && e.closeBtnAlpha > 0.01f) {
            if (!DockLayoutActive() || !DockCloseButtonIsHidden()) {
                RECT btnRc = GetCloseButtonRect(rcCell, rcThumbActual, rcThumbSlot);
                if (btnRc.right > btnRc.left && btnRc.bottom > btnRc.top) {
                    float hoverPlateAlpha = (i == g_hoverIndex && g_hoverWnd == hWnd && g_isCloseHovered) ? g_animCloseBtnHoverAlpha : 0.0f;
                    bool isBtnPressed = (i == g_hoverIndex && g_hoverWnd == hWnd && g_isClosePressed);
                    DrawCloseButton(hdc, btnRc, e.closeBtnAlpha, hoverPlateAlpha, isBtnPressed, e.closeBtnScale);
                }
            }
        }

        // Grouped window count badge
        bool showThisGroupBadge = g_settings.showGroupIndicator && g_settings.showApplications && (e.groupWindows.size() > 1);
        if (DockLayoutActive()) {
            if (DockGroupIndicatorIsHidden()) showThisGroupBadge = false;
        }

        float groupBadgeAlpha = 1.0f;
        if (DockLayoutActive() && DockPositionsOverlap()) {
            // Overlapping case: close button takes precedence in visibility when hovered over the button area
            if (e.closeBtnAlpha > 0.01f) {
                groupBadgeAlpha = (1.0f - e.closeBtnAlpha);
                if (groupBadgeAlpha < 0.01f) {
                    showThisGroupBadge = false;
                }
            }
        }

        if (showThisGroupBadge) {
            Gdiplus::Graphics gfx(hdc);
            gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            gfx.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

            // Badge text
            WCHAR countText[8];
            _snwprintf_s(countText, ARRAYSIZE(countText), _TRUNCATE, L"%d", (int)e.groupWindows.size());

            // Badge font
            int badgeFontSz = DpiScale(10, g_dpiX);
            int fontStyle = Gdiplus::FontStyleBold;
            LPCWSTR family = L"Segoe UI";
            if (g_settings.applyToGroupIndicator) {
                if (g_settings.fontFamily[0]) family = g_settings.fontFamily;
                badgeFontSz = MulDiv(g_settings.fontSize, g_dpiY, 72);
                if (wcscmp(g_settings.fontStyle, L"regular") == 0 || wcscmp(g_settings.fontStyle, L"light") == 0) fontStyle = Gdiplus::FontStyleRegular;
                else if (wcscmp(g_settings.fontStyle, L"semibold") == 0 || wcscmp(g_settings.fontStyle, L"bold") == 0) fontStyle = Gdiplus::FontStyleBold;
                else if (wcscmp(g_settings.fontStyle, L"italic") == 0) fontStyle = Gdiplus::FontStyleItalic;
                else if (wcscmp(g_settings.fontStyle, L"boldItalic") == 0) fontStyle = Gdiplus::FontStyleBoldItalic;
            } else if (IsWin11OrGreater() && DoesFontExist(L"Segoe UI Variable Text")) {
                family = L"Segoe UI Variable Text";
            }
            Gdiplus::Font badgeFont(family, (Gdiplus::REAL)badgeFontSz, fontStyle, Gdiplus::UnitPixel);

            // Measure text
            Gdiplus::StringFormat sf;
            sf.SetAlignment(Gdiplus::StringAlignmentCenter);
            sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            Gdiplus::RectF measureRect(0, 0, 100, 100);
            Gdiplus::RectF textBounds;
            gfx.MeasureString(countText, -1, &badgeFont, measureRect, &sf, &textBounds);

            int badgePadX = DpiScale(4, g_dpiX);
            int badgePadY = DpiScale(2, g_dpiY);
            int badgeW = (int)(textBounds.Width + badgePadX * 2);
            int badgeH = (int)(textBounds.Height + badgePadY * 2);
            int minW = badgeH;  // pill shape: at least as wide as tall
            if (badgeW < minW) badgeW = minW;

            // Position:
            int badgeX = 0, badgeY = 0;
            if (DockLayoutActive()) {
                int pad = DpiScale(2, g_dpiX);
                RECT bRc = ComputeDockPerimeterRect(rcCell, badgeW, badgeH, g_settings.dockGroupIndicatorPosition, pad);
                badgeX = bRc.left;
                badgeY = bRc.top;
            } else if (drawnIconSz > 0) {
                badgeX = drawnIconX + drawnIconSz - (badgeW / 2);
                badgeY = drawnIconY - (badgeH / 2);
            } else {
                int cellPad = DpiScale(4, g_dpiX);
                badgeX = rcCell.right - badgeW - cellPad;
                badgeY = rcCell.top + cellPad;
            }

            // Background pill
            COLORREF bgC = GetIndicatorBackgroundColor();
            int op = g_isDarkMode ? g_settings.indicatorBgOpacityDark : g_settings.indicatorBgOpacityLight;
            int alpha = (int)roundf(((float)(op * 255) / 100.0f) * groupBadgeAlpha);
            Gdiplus::SolidBrush pillBrush(Gdiplus::Color(alpha, GetRValue(bgC), GetGValue(bgC), GetBValue(bgC)));
            Gdiplus::REAL pillRadius = (Gdiplus::REAL)GetGroupIndicatorCornerRadiusPx(badgeH / 2);
            
            if (g_settings.showGroupIndicatorShadow && groupBadgeAlpha > 0.05f) {
                for (int pass = 5; pass > 0; --pass) {
                    int baseA = (pass == 1) ? 8 : (pass == 2) ? 6 : (pass == 3) ? 4 : (pass == 4) ? 3 : 2;
                    int shadowAlpha = (int)roundf(baseA * groupBadgeAlpha);
                    if (shadowAlpha < 1) shadowAlpha = 1;
                    Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(shadowAlpha, 0, 0, 0));
                    int sp = pass;
                    Gdiplus::REAL sx = (Gdiplus::REAL)(badgeX - sp);
                    Gdiplus::REAL sy = (Gdiplus::REAL)(badgeY - sp);
                    Gdiplus::REAL sw = (Gdiplus::REAL)(badgeW + sp * 2);
                    Gdiplus::REAL sh = (Gdiplus::REAL)(badgeH + sp * 2);
                    Gdiplus::REAL sd = pillRadius * 2.0f + sp * 2.0f;
                    if (sd > sw) sd = sw;
                    if (sd > sh) sd = sh;
                    
                    if (pillRadius > 0) {
                        Gdiplus::GraphicsPath sPath;
                        sPath.AddArc(sx, sy, sd, sd, 180, 90);
                        sPath.AddArc(sx + sw - sd, sy, sd, sd, 270, 90);
                        sPath.AddArc(sx + sw - sd, sy + sh - sd, sd, sd, 0, 90);
                        sPath.AddArc(sx, sy + sh - sd, sd, sd, 90, 90);
                        sPath.CloseFigure();
                        gfx.FillPath(&shadowBrush, &sPath);
                    } else {
                        gfx.FillRectangle(&shadowBrush, sx, sy, sw, sh);
                    }
                }
            }

            if (pillRadius > 0) {
                Gdiplus::GraphicsPath pillPath;
                Gdiplus::REAL d = pillRadius * 2.0f;
                Gdiplus::REAL px = (Gdiplus::REAL)badgeX, py = (Gdiplus::REAL)badgeY;
                Gdiplus::REAL pw = (Gdiplus::REAL)badgeW, ph = (Gdiplus::REAL)badgeH;
                pillPath.AddArc(px, py, d, d, 180, 90);
                pillPath.AddArc(px + pw - d, py, d, d, 270, 90);
                pillPath.AddArc(px + pw - d, py + ph - d, d, d, 0, 90);
                pillPath.AddArc(px, py + ph - d, d, d, 90, 90);
                pillPath.CloseFigure();
                gfx.FillPath(&pillBrush, &pillPath);
            } else {
                gfx.FillRectangle(&pillBrush, badgeX, badgeY, badgeW, badgeH);
            }

            // Badge text
            COLORREF txtC = GetIndicatorTextColor();
            BYTE txtAlpha = (BYTE)roundf(255.0f * groupBadgeAlpha);
            Gdiplus::SolidBrush textBrush(Gdiplus::Color(txtAlpha, GetRValue(txtC), GetGValue(txtC), GetBValue(txtC)));
            Gdiplus::RectF pillRect((Gdiplus::REAL)badgeX, (Gdiplus::REAL)badgeY,
                                    (Gdiplus::REAL)badgeW, (Gdiplus::REAL)badgeH);
            gfx.DrawString(countText, -1, &badgeFont, pillRect, &sf, &textBrush);
        }
    }

    // 3. Animated hover focus highlight
    if (g_animHoverAlphaCurrent > 0.01f && g_settings.showThumbnails && g_settings.showHoverBorder && !ThumbnailHoverIsZoom() && !DockLayoutActive()) {
        bool isValidHover = (g_hoverThumbIndex >= 0 && g_hoverThumbIndex < (int)g_windows.size() &&
                             !IsWindowTruncated(g_hoverThumbIndex));
        if (isValidHover || (g_animHoverActive && g_animHoverAlphaTarget == 0.0f)) {
            RectF hRc = (g_animHoverActive && AreAnimationsGloballyEnabled() && g_settings.enableHoverAnimation)
                        ? g_animHoverCurrent
                        : (isValidHover ? ToRectF(g_windows[g_hoverThumbIndex].rcThumbActual) : g_animHoverCurrent);
            if (offX != 0 || offY != 0) {
                hRc.left += (float)offX;
                hRc.right += (float)offX;
                hRc.top += (float)offY;
                hRc.bottom += (float)offY;
            }
            BYTE hoverAlpha = (BYTE)(g_animHoverAlphaCurrent * 255.0f);
            float hoverRadius = (float)GetThumbnailCornerRadiusPx();
            DrawContourF(hdc, hRc, 1.0f, 1, hoverRadius, hoverAlpha);
        } else {
            g_animHoverAlphaCurrent = 0.0f;
            g_animHoverActive = false;
        }
    }

    // 4. Active selection focus border (rendered on top of DWM thumbnails)
    if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() && !IsWindowTruncated(g_selectedIndex)) {
        if (HighlightHasBorder()) {
            RectF selRc = SelectionRectWithViewport();
            DrawContourF(hdc, selRc, (float)SWS_CONTOUR_SIZE, 1, (float)GetTaskUiCornerRadiusPx());
        }
    }

    // 5. Modern Fluent vector overflow chevrons (floating pill indicators)
    if (g_settings.showOverflowIndicator && !g_windows.empty()) {
        UpdateChevronLayout(hWnd);

        auto DrawChevronGlyph = [&](int dir, const RECT& baseRc, float alpha) {
            if (alpha <= 0.01f) return;

            Gdiplus::Graphics gfx(hdc);
            gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            gfx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

            bool verticalLayout = LayoutIsVertical() || DockLayoutActive();
            float motion = dir < 0 ? g_animChevronTransformPrev : g_animChevronTransformNext;
            float slideDist = (float)DpiScale(4, verticalLayout ? g_dpiX : g_dpiY) * (1.0f - motion);
            float offsetX = 0.0f;
            float offsetY = 0.0f;

            if (dir < 0) { // Prev (Up or Left)
                if (verticalLayout) offsetX = -slideDist;
                else offsetY = -slideDist;
            } else { // Next (Down or Right)
                if (verticalLayout) offsetX = slideDist;
                else offsetY = slideDist;
            }

            // Pressed feedback: 1.5px tactile displacement along arrow direction
            if (g_pressedChevron == dir) {
                float pressDist = 1.5f * (float)DpiScale(1, verticalLayout ? g_dpiX : g_dpiY);
                if (verticalLayout) {
                    offsetX += (dir < 0) ? -pressDist : pressDist;
                } else {
                    offsetY += (dir < 0) ? -pressDist : pressDist;
                }
            }

            Gdiplus::REAL px = (Gdiplus::REAL)baseRc.left + offsetX;
            Gdiplus::REAL py = (Gdiplus::REAL)baseRc.top + offsetY;
            Gdiplus::REAL pw = (Gdiplus::REAL)(baseRc.right - baseRc.left);
            Gdiplus::REAL ph = (Gdiplus::REAL)(baseRc.bottom - baseRc.top);

            // Center of the chevron glyph
            Gdiplus::REAL cx = px + pw * 0.5f;
            Gdiplus::REAL cy = py + ph * 0.5f;

            // Fluent proportions for thicker stroke
            Gdiplus::REAL wingAcross = 5.5f * (float)(verticalLayout ? g_dpiY : g_dpiX) / 96.0f;
            Gdiplus::REAL wingAlong = 3.0f * (float)(verticalLayout ? g_dpiX : g_dpiY) / 96.0f;

            // Hover bloom & tactile click state without background:
            // Idle: Subtle secondary text opacity (~70% alpha)
            // Hover: Linear 83 ms bloom to primary text opacity (100% alpha)
            // Pressed: Full primary opacity + 1.5px tactile displacement
            float hoverA = (dir < 0) ? g_animChevronHoverAlphaPrev : g_animChevronHoverAlphaNext;
            bool isPressed = (g_pressedChevron == dir);
            float idleA = 178.0f;
            float activeA = 255.0f;
            float effectiveA = isPressed ? activeA : (idleA + (activeA - idleA) * hoverA);
            BYTE glyphAlpha = (BYTE)roundf(effectiveA * alpha);
            BYTE gR, gG, gB;
            if (g_isDarkMode) {
                gR = 255; gG = 255; gB = 255;
            } else {
                BYTE baseC = (BYTE)roundf(32.0f * (1.0f - hoverA));
                gR = baseC; gG = baseC; gB = baseC;
            }

            // Thicker 2.25px stroke for modern Fluent readability without background
            Gdiplus::REAL penThickness = 2.25f * (float)g_dpiX / 96.0f;
            if (penThickness < 1.5f) penThickness = 1.5f;

            Gdiplus::Pen glyphPen(Gdiplus::Color(glyphAlpha, gR, gG, gB), penThickness);
            glyphPen.SetLineCap(Gdiplus::LineCapRound, Gdiplus::LineCapRound, Gdiplus::DashCapRound);
            glyphPen.SetLineJoin(Gdiplus::LineJoinRound);

            Gdiplus::GraphicsPath glyphPath;
            if (!verticalLayout) {
                if (dir < 0) { // Up
                    glyphPath.AddLine(cx - wingAcross, cy + wingAlong, cx, cy - wingAlong);
                    glyphPath.AddLine(cx, cy - wingAlong, cx + wingAcross, cy + wingAlong);
                } else { // Down
                    glyphPath.AddLine(cx - wingAcross, cy - wingAlong, cx, cy + wingAlong);
                    glyphPath.AddLine(cx, cy + wingAlong, cx + wingAcross, cy - wingAlong);
                }
            } else {
                if (dir < 0) { // Left
                    glyphPath.AddLine(cx + wingAlong, cy - wingAcross, cx - wingAlong, cy);
                    glyphPath.AddLine(cx - wingAlong, cy, cx + wingAlong, cy + wingAcross);
                } else { // Right
                    glyphPath.AddLine(cx - wingAlong, cy - wingAcross, cx + wingAlong, cy);
                    glyphPath.AddLine(cx + wingAlong, cy, cx - wingAlong, cy + wingAcross);
                }
            }
            gfx.DrawPath(&glyphPen, &glyphPath);
        };

        if (g_animChevronAlphaPrev > 0.01f) {
            DrawChevronGlyph(-1, g_rcChevronPrev, g_animChevronAlphaPrev);
        }
        if (g_animChevronAlphaNext > 0.01f) {
            DrawChevronGlyph(1, g_rcChevronNext, g_animChevronAlphaNext);
        }
    }

    if (g_settings.showSwitcherBorder) {
        SelectClipRgn(hdc, NULL);
        RECT wRc; GetClientRect(hWnd, &wRc);
        int winRadius = GetWindowCornerRadiusPx();
        DrawSwitcherOuterBorder(hdc, wRc.right, wRc.bottom, winRadius);
    }
}

static void DrawSwitcherOverlayStaticContent(HDC hdc, HWND hWnd) {
    if (g_windows.empty()) return;
    int cornerRadius = GetThumbnailCornerRadiusPx();

    for (int idx = 0; idx < (int)g_windows.size(); idx++) {
        int i = (g_layoutStartIndex + idx) % g_windows.size();
        const auto& e = g_windows[i];
        if (IsWindowTruncated(i)) continue;

        RECT rcThumbActual = e.rcThumbActual;
        RECT rcCell = e.rcCell;

        if (g_settings.showThumbnails && cornerRadius > 0 && ThemeIs(L"none") && g_settings.opacity >= 99) {
            COLORREF maskColor = GetBgColor();
            if (i == g_selectedIndex && HighlightHasFill() && !DockLayoutActive()) {
                maskColor = GetHighlightFillColor();
            }
            MaskRectCorners(hdc, rcThumbActual, cornerRadius, true, maskColor);
        }

        // Badge layout: draw icon overlay on thumbnail
        int drawnIconX = e.drawnIconX;
        int drawnIconY = e.drawnIconY;
        int drawnIconSz = e.drawnIconSz;
        if (!DockLayoutActive()) {
            DrawBadgeIconOverlay(hdc, rcThumbActual, e.hIcon, &drawnIconX, &drawnIconY, &drawnIconSz, IsEntryMinimized(e), e.hWnd,
                                 (g_layoutTransition.active && e.isNewEntry) ? e.enterAlpha : 1.0f,
                                 e.iconCell ? e.iconCell->icon : nullptr);
        }

        // Grouped window count badge
        bool showThisGroupBadge = g_settings.showGroupIndicator && g_settings.showApplications && (e.groupWindows.size() > 1);
        if (DockLayoutActive()) {
            if (DockGroupIndicatorIsHidden()) showThisGroupBadge = false;
        }

        float groupBadgeAlpha = 1.0f;
        if (DockLayoutActive() && DockPositionsOverlap()) {
            if (e.closeBtnAlpha > 0.01f) {
                groupBadgeAlpha = (1.0f - e.closeBtnAlpha);
                if (groupBadgeAlpha < 0.01f) {
                    showThisGroupBadge = false;
                }
            }
        }

        if (showThisGroupBadge) {
            Gdiplus::Graphics gfx(hdc);
            gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            gfx.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

            WCHAR countText[8];
            _snwprintf_s(countText, ARRAYSIZE(countText), _TRUNCATE, L"%d", (int)e.groupWindows.size());

            int badgeFontSz = DpiScale(10, g_dpiX);
            int fontStyle = Gdiplus::FontStyleBold;
            LPCWSTR family = L"Segoe UI";
            if (g_settings.applyToGroupIndicator) {
                if (g_settings.fontFamily[0]) family = g_settings.fontFamily;
                badgeFontSz = MulDiv(g_settings.fontSize, g_dpiY, 72);
                if (wcscmp(g_settings.fontStyle, L"regular") == 0 || wcscmp(g_settings.fontStyle, L"light") == 0) fontStyle = Gdiplus::FontStyleRegular;
                else if (wcscmp(g_settings.fontStyle, L"semibold") == 0 || wcscmp(g_settings.fontStyle, L"bold") == 0) fontStyle = Gdiplus::FontStyleBold;
                else if (wcscmp(g_settings.fontStyle, L"italic") == 0) fontStyle = Gdiplus::FontStyleItalic;
                else if (wcscmp(g_settings.fontStyle, L"boldItalic") == 0) fontStyle = Gdiplus::FontStyleBoldItalic;
            } else if (IsWin11OrGreater() && DoesFontExist(L"Segoe UI Variable Text")) {
                family = L"Segoe UI Variable Text";
            }
            Gdiplus::Font badgeFont(family, (Gdiplus::REAL)badgeFontSz, fontStyle, Gdiplus::UnitPixel);

            Gdiplus::StringFormat sf;
            sf.SetAlignment(Gdiplus::StringAlignmentCenter);
            sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            Gdiplus::RectF measureRect(0, 0, 100, 100);
            Gdiplus::RectF textBounds;
            gfx.MeasureString(countText, -1, &badgeFont, measureRect, &sf, &textBounds);

            int badgePadX = DpiScale(4, g_dpiX);
            int badgePadY = DpiScale(2, g_dpiY);
            int badgeW = (int)(textBounds.Width + badgePadX * 2);
            int badgeH = (int)(textBounds.Height + badgePadY * 2);
            int minW = badgeH;
            if (badgeW < minW) badgeW = minW;

            int badgeX = 0, badgeY = 0;
            if (DockLayoutActive()) {
                int pad = DpiScale(2, g_dpiX);
                RECT bRc = ComputeDockPerimeterRect(rcCell, badgeW, badgeH, g_settings.dockGroupIndicatorPosition, pad);
                badgeX = bRc.left;
                badgeY = bRc.top;
            } else if (drawnIconSz > 0) {
                badgeX = drawnIconX + drawnIconSz - (badgeW / 2);
                badgeY = drawnIconY - (badgeH / 2);
            } else {
                int cellPad = DpiScale(4, g_dpiX);
                badgeX = rcCell.right - badgeW - cellPad;
                badgeY = rcCell.top + cellPad;
            }

            COLORREF bgC = GetIndicatorBackgroundColor();
            int op = g_isDarkMode ? g_settings.indicatorBgOpacityDark : g_settings.indicatorBgOpacityLight;
            int alpha = (int)roundf(((float)(op * 255) / 100.0f) * groupBadgeAlpha);
            Gdiplus::SolidBrush pillBrush(Gdiplus::Color(alpha, GetRValue(bgC), GetGValue(bgC), GetBValue(bgC)));
            Gdiplus::REAL pillRadius = (Gdiplus::REAL)GetGroupIndicatorCornerRadiusPx(badgeH / 2);

            if (g_settings.showGroupIndicatorShadow && groupBadgeAlpha > 0.05f) {
                for (int pass = 5; pass > 0; --pass) {
                    int baseA = (pass == 1) ? 8 : (pass == 2) ? 6 : (pass == 3) ? 4 : (pass == 4) ? 3 : 2;
                    int shadowAlpha = (int)roundf(baseA * groupBadgeAlpha);
                    if (shadowAlpha < 1) shadowAlpha = 1;
                    Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(shadowAlpha, 0, 0, 0));
                    int sp = pass;
                    Gdiplus::REAL sx = (Gdiplus::REAL)(badgeX - sp);
                    Gdiplus::REAL sy = (Gdiplus::REAL)(badgeY - sp);
                    Gdiplus::REAL sw = (Gdiplus::REAL)(badgeW + sp * 2);
                    Gdiplus::REAL sh = (Gdiplus::REAL)(badgeH + sp * 2);
                    Gdiplus::REAL sd = pillRadius * 2.0f + sp * 2.0f;
                    if (sd > sw) sd = sw;
                    if (sd > sh) sd = sh;

                    if (pillRadius > 0) {
                        Gdiplus::GraphicsPath sPath;
                        sPath.AddArc(sx, sy, sd, sd, 180, 90);
                        sPath.AddArc(sx + sw - sd, sy, sd, sd, 270, 90);
                        sPath.AddArc(sx + sw - sd, sy + sh - sd, sd, sd, 0, 90);
                        sPath.AddArc(sx, sy + sh - sd, sd, sd, 90, 90);
                        sPath.CloseFigure();
                        gfx.FillPath(&shadowBrush, &sPath);
                    } else {
                        gfx.FillRectangle(&shadowBrush, sx, sy, sw, sh);
                    }
                }
            }

            if (pillRadius > 0) {
                Gdiplus::GraphicsPath pillPath;
                Gdiplus::REAL d = pillRadius * 2.0f;
                Gdiplus::REAL px = (Gdiplus::REAL)badgeX, py = (Gdiplus::REAL)badgeY;
                Gdiplus::REAL pw = (Gdiplus::REAL)badgeW, ph = (Gdiplus::REAL)badgeH;
                pillPath.AddArc(px, py, d, d, 180, 90);
                pillPath.AddArc(px + pw - d, py, d, d, 270, 90);
                pillPath.AddArc(px + pw - d, py + ph - d, d, d, 0, 90);
                pillPath.AddArc(px, py + ph - d, d, d, 90, 90);
                pillPath.CloseFigure();
                gfx.FillPath(&pillBrush, &pillPath);
            } else {
                gfx.FillRectangle(&pillBrush, badgeX, badgeY, badgeW, badgeH);
            }

            COLORREF txtC = GetIndicatorTextColor();
            BYTE txtAlpha = (BYTE)roundf(255.0f * groupBadgeAlpha);
            Gdiplus::SolidBrush textBrush(Gdiplus::Color(txtAlpha, GetRValue(txtC), GetGValue(txtC), GetBValue(txtC)));
            Gdiplus::RectF pillRect((Gdiplus::REAL)badgeX, (Gdiplus::REAL)badgeY,
                                    (Gdiplus::REAL)badgeW, (Gdiplus::REAL)badgeH);
            gfx.DrawString(countText, -1, &badgeFont, pillRect, &sf, &textBrush);
        }
    }
}

static void DrawSwitcherOverlayDynamicContent(HDC hdc, HWND hWnd) {
    if (g_windows.empty()) return;
    int cornerRadius = GetThumbnailCornerRadiusPx();

    // 1. Zoomed thumbnail override (if hover zoom is active for a specific entry)
    if (ThumbnailHoverIsZoom() && g_hoverThumbIndex >= 0 && g_hoverThumbIndex < (int)g_windows.size() && !IsWindowTruncated(g_hoverThumbIndex)) {
        const auto& e = g_windows[g_hoverThumbIndex];
        if (e.hoverScale > 1.0001f) {
            RECT rcThumbActual = GetScaledThumbRect(e);
            if (g_settings.showThumbnails && cornerRadius > 0 && ThemeIs(L"none") && g_settings.opacity >= 99) {
                COLORREF maskColor = GetBgColor();
                if (g_hoverThumbIndex == g_selectedIndex && HighlightHasFill() && !DockLayoutActive()) {
                    maskColor = GetHighlightFillColor();
                }
                MaskRectCorners(hdc, rcThumbActual, cornerRadius, true, maskColor);
            }
            int drawnIconX = e.drawnIconX;
            int drawnIconY = e.drawnIconY;
            int drawnIconSz = e.drawnIconSz;
            if (!DockLayoutActive()) {
                DrawBadgeIconOverlay(hdc, rcThumbActual, e.hIcon, &drawnIconX, &drawnIconY, &drawnIconSz, IsEntryMinimized(e), e.hWnd,
                                     (g_layoutTransition.active && e.isNewEntry) ? e.enterAlpha : 1.0f,
                                     e.iconCell ? e.iconCell->icon : nullptr);
            }
        }
    }

    // 2. Close buttons for any entry animating/showing close button
    if (g_settings.showCloseButton) {
        for (int i = 0; i < (int)g_windows.size(); i++) {
            const auto& e = g_windows[i];
            if (IsWindowTruncated(i)) continue;
            if (e.closeBtnAlpha > 0.01f) {
                if (!DockLayoutActive() || !DockCloseButtonIsHidden()) {
                    RECT rcThumbActual = (ThumbnailHoverIsZoom() && e.hoverScale > 1.0001f) ? GetScaledThumbRect(e) : e.rcThumbActual;
                    RECT btnRc = GetCloseButtonRect(e.rcCell, rcThumbActual, e.rcThumbSlot);
                    if (btnRc.right > btnRc.left && btnRc.bottom > btnRc.top) {
                        float hoverPlateAlpha = (i == g_hoverIndex && g_hoverWnd == hWnd && g_isCloseHovered) ? g_animCloseBtnHoverAlpha : 0.0f;
                        bool isBtnPressed = (i == g_hoverIndex && g_hoverWnd == hWnd && g_isClosePressed);
                        DrawCloseButton(hdc, btnRc, e.closeBtnAlpha, hoverPlateAlpha, isBtnPressed, e.closeBtnScale);
                    }
                }
            }
        }
    }

    // 3. Animated hover focus highlight
    if (g_animHoverAlphaCurrent > 0.01f && g_settings.showThumbnails && g_settings.showHoverBorder && !ThumbnailHoverIsZoom() && !DockLayoutActive()) {
        bool isValidHover = (g_hoverThumbIndex >= 0 && g_hoverThumbIndex < (int)g_windows.size() &&
                             !IsWindowTruncated(g_hoverThumbIndex));
        if (isValidHover || (g_animHoverActive && g_animHoverAlphaTarget == 0.0f)) {
            RectF hRc = (g_animHoverActive && AreAnimationsGloballyEnabled() && g_settings.enableHoverAnimation)
                        ? g_animHoverCurrent
                        : (isValidHover ? ToRectF(g_windows[g_hoverThumbIndex].rcThumbActual) : g_animHoverCurrent);
            BYTE hoverAlpha = (BYTE)(g_animHoverAlphaCurrent * 255.0f);
            float hoverRadius = (float)GetThumbnailCornerRadiusPx();
            DrawContourF(hdc, hRc, 1.0f, 1, hoverRadius, hoverAlpha);
        } else {
            g_animHoverAlphaCurrent = 0.0f;
            g_animHoverActive = false;
        }
    }

    // 4. Active selection focus border (rendered on top of DWM thumbnails)
    if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() && !IsWindowTruncated(g_selectedIndex)) {
        if (HighlightHasBorder()) {
            RectF selRc = SelectionRectWithViewport();
            DrawContourF(hdc, selRc, (float)SWS_CONTOUR_SIZE, 1, (float)GetTaskUiCornerRadiusPx());
        }
    }

    // 5. Modern Fluent vector overflow chevrons
    if (g_settings.showOverflowIndicator && !g_windows.empty()) {
        UpdateChevronLayout(hWnd);

        auto DrawChevronGlyph = [&](int dir, const RECT& baseRc, float alpha) {
            if (alpha <= 0.01f) return;
            Gdiplus::Graphics gfx(hdc);
            gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            gfx.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

            bool verticalLayout = LayoutIsVertical() || DockLayoutActive();
            float motion = dir < 0 ? g_animChevronTransformPrev : g_animChevronTransformNext;
            float slideDist = (float)DpiScale(4, verticalLayout ? g_dpiX : g_dpiY) * (1.0f - motion);
            float offsetX = 0.0f;
            float offsetY = 0.0f;

            if (dir < 0) {
                if (verticalLayout) offsetX = -slideDist;
                else offsetY = -slideDist;
            } else {
                if (verticalLayout) offsetX = slideDist;
                else offsetY = slideDist;
            }

            if (g_pressedChevron == dir) {
                float pressDist = 1.5f * (float)DpiScale(1, verticalLayout ? g_dpiX : g_dpiY);
                if (verticalLayout) offsetX += (dir < 0) ? -pressDist : pressDist;
                else offsetY += (dir < 0) ? -pressDist : pressDist;
            }

            Gdiplus::REAL px = (Gdiplus::REAL)baseRc.left + offsetX;
            Gdiplus::REAL py = (Gdiplus::REAL)baseRc.top + offsetY;
            Gdiplus::REAL pw = (Gdiplus::REAL)(baseRc.right - baseRc.left);
            Gdiplus::REAL ph = (Gdiplus::REAL)(baseRc.bottom - baseRc.top);

            Gdiplus::REAL cx = px + pw * 0.5f;
            Gdiplus::REAL cy = py + ph * 0.5f;

            Gdiplus::REAL wingAcross = 5.5f * (float)(verticalLayout ? g_dpiY : g_dpiX) / 96.0f;
            Gdiplus::REAL wingAlong = 3.0f * (float)(verticalLayout ? g_dpiX : g_dpiY) / 96.0f;

            float hoverA = (dir < 0) ? g_animChevronHoverAlphaPrev : g_animChevronHoverAlphaNext;
            bool isPressed = (g_pressedChevron == dir);
            float idleA = 178.0f;
            float activeA = 255.0f;
            float effectiveA = isPressed ? activeA : (idleA + (activeA - idleA) * hoverA);
            BYTE glyphAlpha = (BYTE)roundf(effectiveA * alpha);
            BYTE gR, gG, gB;
            if (g_isDarkMode) {
                gR = 255; gG = 255; gB = 255;
            } else {
                BYTE baseC = (BYTE)roundf(32.0f * (1.0f - hoverA));
                gR = baseC; gG = baseC; gB = baseC;
            }

            Gdiplus::REAL penThickness = 2.25f * (float)g_dpiX / 96.0f;
            if (penThickness < 1.5f) penThickness = 1.5f;

            Gdiplus::Pen glyphPen(Gdiplus::Color(glyphAlpha, gR, gG, gB), penThickness);
            glyphPen.SetLineCap(Gdiplus::LineCapRound, Gdiplus::LineCapRound, Gdiplus::DashCapRound);
            glyphPen.SetLineJoin(Gdiplus::LineJoinRound);

            Gdiplus::GraphicsPath glyphPath;
            if (!verticalLayout) {
                if (dir < 0) {
                    glyphPath.AddLine(cx - wingAcross, cy + wingAlong, cx, cy - wingAlong);
                    glyphPath.AddLine(cx, cy - wingAlong, cx + wingAcross, cy + wingAlong);
                } else {
                    glyphPath.AddLine(cx - wingAcross, cy - wingAlong, cx, cy + wingAlong);
                    glyphPath.AddLine(cx, cy + wingAlong, cx + wingAcross, cy - wingAlong);
                }
            } else {
                if (dir < 0) {
                    glyphPath.AddLine(cx + wingAlong, cy - wingAcross, cx - wingAlong, cy);
                    glyphPath.AddLine(cx - wingAlong, cy, cx + wingAlong, cy + wingAcross);
                } else {
                    glyphPath.AddLine(cx - wingAlong, cy - wingAcross, cx + wingAlong, cy);
                    glyphPath.AddLine(cx + wingAlong, cy, cx - wingAlong, cy + wingAcross);
                }
            }
            gfx.DrawPath(&glyphPen, &glyphPath);
        };

        if (g_animChevronAlphaPrev > 0.01f) {
            DrawChevronGlyph(-1, g_rcChevronPrev, g_animChevronAlphaPrev);
        }
        if (g_animChevronAlphaNext > 0.01f) {
            DrawChevronGlyph(1, g_rcChevronNext, g_animChevronAlphaNext);
        }
    }

    if (g_settings.showSwitcherBorder) {
        SelectClipRgn(hdc, NULL);
        RECT wRc; GetClientRect(hWnd, &wRc);
        int winRadius = GetWindowCornerRadiusPx();
        DrawSwitcherOuterBorder(hdc, wRc.right, wRc.bottom, winRadius);
    }
}

static void PaintSwitcherOverlay() {
    if (!g_hCloseBtnWnd || !g_isVisible || g_backdropExitFadeActive) return;
    if (g_animActive && !g_animTickInProgress && !g_animEntranceFrameZeroPending) return;
    AnchorPresentationOverlay();
    PositionPresentationWindow(g_hCloseBtnWnd);
    HWND targetWnd = g_hoverWnd ? g_hoverWnd : g_hSwitcher;
    RECT rc; GetClientRect(targetWnd, &rc);
    int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0) return;

    if (!s_cachedOverlayDC || s_cachedOverlayW != w || s_cachedOverlayH != h) {
        if (s_cachedOverlayDC) {
            if (s_cachedOverlayOldBitmap) SelectObject(s_cachedOverlayDC, s_cachedOverlayOldBitmap);
            if (s_cachedOverlayBitmap) DeleteObject(s_cachedOverlayBitmap);
            DeleteDC(s_cachedOverlayDC);
        }
        HDC hdcScreen = GetDC(NULL);
        s_cachedOverlayDC = CreateCompatibleDC(hdcScreen);
        BITMAPINFO bmi = {}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
        s_cachedOverlayBitmap = CreateDIBSection(s_cachedOverlayDC, &bmi, DIB_RGB_COLORS, &s_cachedOverlayBits, NULL, 0);
        s_cachedOverlayOldBitmap = (HBITMAP)SelectObject(s_cachedOverlayDC, s_cachedOverlayBitmap);
        s_cachedOverlayW = w;
        s_cachedOverlayH = h;
        ReleaseDC(NULL, hdcScreen);
    }

    int radius = GetWindowCornerRadiusPx();

    bool animatedDecoration = DockLayoutActive() && g_settings.showGroupIndicator && g_settings.showCloseButton;
    for (const auto& entry : g_windows) {
        if (entry.hoverScale != 1.0f) { animatedDecoration = true; break; }
    }
    if (!g_scrollTransition.active && !g_layoutTransition.active && !animatedDecoration) {
        // Ensure static overlay cache
        if (!s_cachedOverlayStaticDC || s_cachedOverlayStaticW != w || s_cachedOverlayStaticH != h) {
            if (s_cachedOverlayStaticDC) {
                if (s_cachedOverlayStaticOldBitmap) SelectObject(s_cachedOverlayStaticDC, s_cachedOverlayStaticOldBitmap);
                if (s_cachedOverlayStaticBitmap) DeleteObject(s_cachedOverlayStaticBitmap);
                DeleteDC(s_cachedOverlayStaticDC);
            }
            HDC hdcScreen = GetDC(NULL);
            s_cachedOverlayStaticDC = CreateCompatibleDC(hdcScreen);
            BITMAPINFO bmi = {}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
            bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
            s_cachedOverlayStaticBitmap = CreateDIBSection(s_cachedOverlayStaticDC, &bmi, DIB_RGB_COLORS, &s_cachedOverlayStaticBits, NULL, 0);
            s_cachedOverlayStaticOldBitmap = (HBITMAP)SelectObject(s_cachedOverlayStaticDC, s_cachedOverlayStaticBitmap);
            s_cachedOverlayStaticW = w;
            s_cachedOverlayStaticH = h;
            ReleaseDC(NULL, hdcScreen);
            g_overlayStaticDirty = true;
        }

        if (g_overlayStaticDirty) {
            if (s_cachedOverlayStaticBits) {
                memset(s_cachedOverlayStaticBits, 0, (size_t)w * h * sizeof(DWORD));
            }
            HRGN hClip = GetCachedRoundRectRgn(w, h, radius);
            SelectClipRgn(s_cachedOverlayStaticDC, hClip);
            DrawSwitcherOverlayStaticContent(s_cachedOverlayStaticDC, targetWnd);
            g_overlayStaticDirty = false;
        }

        // Fast blit pre-rendered static overlay (<0.05ms)
        BitBlt(s_cachedOverlayDC, 0, 0, w, h, s_cachedOverlayStaticDC, 0, 0, SRCCOPY);

        HRGN hClip = GetCachedRoundRectRgn(w, h, radius);
        SelectClipRgn(s_cachedOverlayDC, hClip);
        DrawSwitcherOverlayDynamicContent(s_cachedOverlayDC, targetWnd);
    } else {
        g_overlayStaticDirty = true;
        if (s_cachedOverlayBits) {
            memset(s_cachedOverlayBits, 0, (size_t)w * h * sizeof(DWORD));
        }
        HRGN hClip = GetCachedRoundRectRgn(w, h, radius);
        SelectClipRgn(s_cachedOverlayDC, hClip);
        DrawSwitcherOverlay(s_cachedOverlayDC, targetWnd);
    }

    POINT ptSrc = {0,0}; SIZE sz = {w, h};
    RECT presentationRect = {};
    GetPresentationWindowRect(g_hCloseBtnWnd, &presentationRect);
    POINT ptDst = { presentationRect.left, presentationRect.top };
    BYTE finalAlpha = CurrentPresentationAlphaByte();
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, finalAlpha, AC_SRC_ALPHA};
    HDC hdcScreen = GetDC(NULL);
    UpdateLayeredWindow(g_hCloseBtnWnd, hdcScreen, &ptDst, &sz, s_cachedOverlayDC, &ptSrc, 0, &bf, ULW_ALPHA);
    ReleaseDC(NULL, hdcScreen);
}

// One compositor for layered/native presentation and interrupted-scroll capture.
// The background is stationary; only the padded content viewport moves. Do not
// round source canvases: their translated corner alpha would flicker in the UI.
static void DrawScrollTransitionFrame(HDC hdc, int w, int h, bool includeSelection) {
    int saved = SaveDC(hdc);
    SelectClipRgn(hdc, NULL);
    RECT full = { 0, 0, w, h };
    FillSwitcherBackground(hdc, full, ShouldFillBackground());
    bool dock = DockLayoutActive();
    if (dock && s_cachedScrollToDC) {
        BitBlt(hdc, 0, 0, w, h, s_cachedScrollToDC, 0, 0, SRCCOPY);
        if (includeSelection && g_settings.showThumbnailShadow && DockShowPreview() &&
            (g_layoutTransition.active || g_dockPreviewSlide.active)) {
            RECT shadow = g_rcCentralPreview;
            float alpha = 1.0f;
            if (g_dockPreviewSlide.active) {
                OffsetRect(&shadow, (int)roundf(g_dockPreviewSlide.currentOffset), 0);
                alpha = g_dockPreviewSlide.currentAlpha;
            }
            DrawThumbnailShadow(hdc, shadow, GetThumbnailCornerRadiusPx(), alpha);
        }
    }
    int padX = DpiScale(g_settings.switcherPadding, g_dpiX);
    int padY = DpiScale(g_settings.switcherPadding, g_dpiY);
    RECT content = dock ? g_rcDockIconStrip : RECT{ padX, padY, w - padX, h - padY };
    IntersectClipRect(hdc, content.left, content.top, content.right, content.bottom);
    if (dock) FillSwitcherBackground(hdc, content, ShouldFillBackground());

    int offX = (int)roundf(g_scrollTransition.offsetCurrentX);
    int offY = dock ? 0 : (int)roundf(g_scrollTransition.offsetCurrentY);
    if (s_cachedScrollFromDC) {
        BitBlt(hdc, offX - g_scrollTransition.travelDistanceX,
               offY - (dock ? 0 : g_scrollTransition.travelDistanceY),
               w, h, s_cachedScrollFromDC, 0, 0, SRCCOPY);
    }
    if (s_cachedScrollToDC) {
        BitBlt(hdc, offX, offY, w, h, s_cachedScrollToDC, 0, 0, SRCCOPY);
    }
    if (includeSelection && dock) {
        DrawDockIconStrip(hdc);
    } else if (includeSelection && HighlightHasFill() && g_selectedIndex >= 0 &&
        g_selectedIndex < (int)g_windows.size()) {
        RectF selection = SelectionRectWithViewport();
        DrawSelectionFillF(hdc, selection);
    }
    RestoreDC(hdc, saved);
}

static void PaintSwitcher() {
    if (!g_hSwitcher || !g_isVisible || g_backdropExitFadeActive) return;
    // Input handlers publish state immediately; an active ticker presents it at
    // the next deadline. Keep the initial entrance paint before first visibility.
    if (g_animActive && !g_animTickInProgress && !g_animEntranceFrameZeroPending) return;
    struct PaintScope {
        bool wasInProgress = g_animTickInProgress;
        PaintScope() { g_animTickInProgress = true; }
        ~PaintScope() { g_animTickInProgress = wasInProgress; }
    } paintScope;
    g_animEntranceFrameZeroPending = false;
    s_iconRepaintPending = false;
    SweepWindowIconShadows();
    if (g_scrollTransition.active) RefreshScrollIconCanvas();
    ApplyPresentationWindowOffset();
    if (ThemeIs(L"none")) {
        // Layered window path: draw to off-screen DIB, UpdateLayeredWindow
        RECT rc; GetClientRect(g_hSwitcher, &rc);
        int w = rc.right, h = rc.bottom;
        if (w <= 0 || h <= 0) return;

        if (!s_cachedMemDC || s_cachedW != w || s_cachedH != h) {
            if (s_cachedMemDC) {
                if (s_cachedOldBitmap) SelectObject(s_cachedMemDC, s_cachedOldBitmap);
                if (s_cachedBitmap) DeleteObject(s_cachedBitmap);
                DeleteDC(s_cachedMemDC);
            }
            HDC hdcScreen = GetDC(g_hSwitcher);
            s_cachedMemDC = CreateCompatibleDC(hdcScreen);
            BITMAPINFO bmi = {}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
            bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
            s_cachedBitmap = CreateDIBSection(s_cachedMemDC, &bmi, DIB_RGB_COLORS, &s_cachedMemBits, NULL, 0);
            s_cachedOldBitmap = (HBITMAP)SelectObject(s_cachedMemDC, s_cachedBitmap);
            s_cachedW = w;
            s_cachedH = h;
            ReleaseDC(g_hSwitcher, hdcScreen);
        }

        int radius = GetWindowCornerRadiusPx();

        // The final alpha mask owns rounding. A retained GDI region here would
        // clip settled frames differently from scroll frames at the corners.
        SelectClipRgn(s_cachedMemDC, NULL);
        if (!g_scrollTransition.active) {
            if (!s_cachedStaticDC || s_cachedStaticW != w || s_cachedStaticH != h) {
                if (s_cachedStaticDC) {
                    if (s_cachedStaticOldBitmap) SelectObject(s_cachedStaticDC, s_cachedStaticOldBitmap);
                    if (s_cachedStaticBitmap) DeleteObject(s_cachedStaticBitmap);
                    DeleteDC(s_cachedStaticDC);
                }
                HDC hdcScreen = GetDC(g_hSwitcher);
                s_cachedStaticDC = CreateCompatibleDC(hdcScreen);
                BITMAPINFO bmi = {}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
                bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
                s_cachedStaticBitmap = CreateDIBSection(s_cachedStaticDC, &bmi, DIB_RGB_COLORS, &s_cachedStaticBits, NULL, 0);
                s_cachedStaticOldBitmap = (HBITMAP)SelectObject(s_cachedStaticDC, s_cachedStaticBitmap);
                s_cachedStaticW = w;
                s_cachedStaticH = h;
                ReleaseDC(g_hSwitcher, hdcScreen);
                g_staticContentDirty = true;
            }

            if (g_staticContentDirty) {
                if (s_cachedStaticBits) {
                    memset(s_cachedStaticBits, 0, (size_t)w * h * sizeof(DWORD));
                }
                SelectClipRgn(s_cachedStaticDC, NULL);
                DrawSwitcherStaticContent(s_cachedStaticDC, true, g_hSwitcher);
                g_staticContentDirty = false;
            }

            // Blit pre-rendered static content in <0.05ms
            BitBlt(s_cachedMemDC, 0, 0, w, h, s_cachedStaticDC, 0, 0, SRCCOPY);

            // Dynamic thumbnail drop shadow for any zoomed/animating thumbnails
            if (g_settings.showThumbnails && g_settings.showThumbnailShadow) {
                if (DockLayoutActive() && DockShowPreview()) {
                    if (g_layoutTransition.active || g_dockPreviewSlide.active) {
                        RECT shadowRc = g_rcCentralPreview;
                        float shadowAlphaMult = 1.0f;
                        if (g_dockPreviewSlide.active) {
                            int offX = (int)roundf(g_dockPreviewSlide.currentOffset);
                            shadowRc.left += offX;
                            shadowRc.right += offX;
                            shadowAlphaMult = g_dockPreviewSlide.currentAlpha;
                        }
                        DrawThumbnailShadow(s_cachedMemDC, shadowRc, GetThumbnailCornerRadiusPx(), shadowAlphaMult);
                    }
                } else {
                    if (g_layoutTransition.active) {
                        for (size_t i = 0; i < g_windows.size(); ++i) {
                            const auto& e = g_windows[i];
                            if (e.rcThumbActual.right > e.rcThumbActual.left && !IsWindowTruncated((int)i)) {
                                float itemAlpha = e.isNewEntry ? e.enterAlpha : 1.0f;
                                DrawThumbnailShadow(s_cachedMemDC, e.rcThumbActual, GetThumbnailCornerRadiusPx(), itemAlpha);
                            }
                        }
                    } else if (ThumbnailHoverIsZoom()) {
                        for (size_t i = 0; i < g_windows.size(); ++i) {
                            const auto& e = g_windows[i];
                            if (e.hoverScale > 1.0001f && !IsWindowTruncated((int)i)) {
                                RECT scaledRc = GetScaledThumbRect(e);
                                float shadowAlphaMult = (e.hoverScale - 1.0f) / SWS_HOVER_ZOOM_DELTA;
                                if (shadowAlphaMult < 0.0f) shadowAlphaMult = 0.0f;
                                if (shadowAlphaMult > 1.0f) shadowAlphaMult = 1.0f;
                                DrawThumbnailShadow(s_cachedMemDC, scaledRc, GetThumbnailCornerRadiusPx(), shadowAlphaMult, e.hoverScale);
                            }
                        }
                    }
                }
            }

            // Draw moving selection highlight fill on top of background
            if (DockLayoutActive()) {
                DrawDockIconStrip(s_cachedMemDC);
            } else if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() && HighlightHasFill()) {
                RectF fillRc = SelectionRectWithViewport();
                DrawSelectionFillF(s_cachedMemDC, fillRc);
            }
        } else {
            // Composite the same stationary background used by settled frames.
            if (s_cachedMemBits) {
                memset(s_cachedMemBits, 0, (size_t)w * h * sizeof(DWORD));
            }
            DrawScrollTransitionFrame(s_cachedMemDC, w, h, true);
        }
        if (radius > 0) {
            RECT rcFull = { 0, 0, w, h };
            MaskRectCorners(s_cachedMemDC, rcFull, radius);
        }

        POINT ptSrc = {0,0}; SIZE sz = {w, h};
        RECT presentationRect = {};
        GetPresentationWindowRect(g_hSwitcher, &presentationRect);
        POINT ptDst = { presentationRect.left, presentationRect.top };
        BYTE finalAlpha = CurrentPresentationAlphaByte();
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, finalAlpha, AC_SRC_ALPHA};
        HDC hdcScreen = GetDC(NULL);
        UpdateLayeredWindow(g_hSwitcher, hdcScreen, &ptDst, &sz, s_cachedMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
        for (HWND hMirror : g_hMirrorSwitchers) {
            if (IsWindow(hMirror)) {
                // The compositor's viewport and presentation timeline are
                // shared. Redrawing a settled mirror during a slide makes its
                // cards jump to the target page while its thumbnails still move.
                if (!g_scrollTransition.active) {
                    DrawSwitcherContent(s_cachedMemDC, true, hMirror);
                    if (radius > 0) {
                        RECT full = { 0, 0, w, h };
                        MaskRectCorners(s_cachedMemDC, full, radius);
                    }
                }
                RECT mwr = {}; GetPresentationWindowRect(hMirror, &mwr);
                POINT mPtDst = { mwr.left, mwr.top };
                UpdateLayeredWindow(hMirror, hdcScreen, &mPtDst, &sz, s_cachedMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
            }
        }
        ReleaseDC(NULL, hdcScreen);
        PaintSwitcherOverlay();
    } else {
        InvalidateRect(g_hSwitcher, NULL, FALSE);
        UpdateWindow(g_hSwitcher);
        for (HWND hMirror : g_hMirrorSwitchers) {
            if (IsWindow(hMirror)) {
                InvalidateRect(hMirror, NULL, FALSE);
                UpdateWindow(hMirror);
            }
        }
        PaintSwitcherOverlay();
    }
}

// Switcher Show / Hide / Switch

static void CancelPendingShow() {
    if (g_hSwitcher) {
        KillTimer(g_hSwitcher, SWS_SHOW_DELAY_TIMER_ID);
        KillTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID);
        DWMNCRENDERINGPOLICY enabled = DWMNCRP_ENABLED;
        DwmSetWindowAttribute(g_hSwitcher, DWMWA_NCRENDERING_POLICY, &enabled, sizeof(enabled));
        if (IsWin11OrGreater()) {
            COLORREF colorNone = 0xFFFFFFFE; // DWMWA_COLOR_NONE
            DwmSetWindowAttribute(g_hSwitcher, 34 /* DWMWA_BORDER_COLOR */, &colorNone, sizeof(colorNone));
        }
    }
    // The WinEvent hook stays installed for the switcher thread's lifetime: the raw-HID
    // path needs it while the switcher is hidden, to hide a Task View the OS shows for its
    // own 3-finger up gesture. It is unhooked when the switcher window is destroyed.
    g_isPendingShow = false;
    g_pendingSwitcherRect = { 0, 0, 0, 0 };
}

// Precision Touchpad & Mouse Wheel Sub-notch Accumulators
static int s_wheelDeltaAccum = 0;
static int s_hwheelDeltaAccum = 0;
static ULONGLONG s_lastTouchpadScrollTick = 0;

static void ResetScrollWheelAccumulators() {
    s_wheelDeltaAccum = 0;
    s_hwheelDeltaAccum = 0;
    s_lastTouchpadScrollTick = 0;
}

// Stable TOUCHPAD_PARAMETERS_V1 layout from the Windows SDK. The compiler's
// SDK predates these declarations; keep the numbered version rather than the
// latest-version alias. SPI_GET/SETTOUCHPADPARAMETERS require Windows 11 24H2.
struct SwsTouchpadParametersV1 {
    UINT versionNumber;
    UINT maxSupportedContacts;
    UINT legacyTouchpadFeatures;
    BOOL touchpadPresent : 1;
    BOOL legacyTouchpadPresent : 1;
    BOOL externalMousePresent : 1;
    BOOL touchpadEnabled : 1;
    BOOL touchpadActive : 1;
    BOOL feedbackSupported : 1;
    BOOL clickForceSupported : 1;
    BOOL reserved1 : 25;
    BOOL allowActiveWhenMousePresent : 1;
    BOOL feedbackEnabled : 1;
    BOOL tapEnabled : 1;
    BOOL tapAndDragEnabled : 1;
    BOOL twoFingerTapEnabled : 1;
    BOOL rightClickZoneEnabled : 1;
    BOOL mouseAccelSettingHonored : 1;
    BOOL panEnabled : 1;
    BOOL zoomEnabled : 1;
    BOOL scrollDirectionReversed : 1;
    BOOL reserved2 : 22;
    UINT sensitivityLevel;
    UINT cursorSpeed;
    UINT feedbackIntensity;
    UINT clickForceSensitivity;
    UINT rightClickZoneWidth;
    UINT rightClickZoneHeight;
};
static_assert(sizeof(SwsTouchpadParametersV1) == 44);
static constexpr UINT SWS_SPI_GETTOUCHPADPARAMETERS = 0x00AE;
static constexpr UINT SWS_SPI_SETTOUCHPADPARAMETERS = 0x00AF;

// Switcher-thread only. We own a restore only after changing an enabled tap
// action. If Windows already had it disabled, leave that preference disabled.
static bool s_twoFingerTapRestoreOwned = false;
static bool s_twoFingerTapParametersUnavailable = false;
static DWORD s_twoFingerTapParameterError = ERROR_SUCCESS;

static void UpdateTwoFingerTapOverride(bool want) {
    if (want && g_touchpadTwoFingerTapSuppressed.load()) {
        if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_TWO_FINGER_TAP_RESTORE_TIMER_ID);
        return;
    }
    if (!want && !s_twoFingerTapRestoreOwned) {
        g_touchpadTwoFingerTapSuppressed.store(false);
        if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_TWO_FINGER_TAP_RESTORE_TIMER_ID);
        return;
    }
    if (want && s_twoFingerTapParametersUnavailable) return;

    SwsTouchpadParametersV1 parameters{};
    parameters.versionNumber = 1;
    SetLastError(ERROR_SUCCESS);
    BOOL read = SystemParametersInfoW(SWS_SPI_GETTOUCHPADPARAMETERS,
                                     sizeof(parameters), &parameters, 0);
    DWORD error = read ? ERROR_SUCCESS : GetLastError();
    if (read && want && !parameters.touchpadPresent) return;
    if (read && (!want || parameters.twoFingerTapEnabled)) {
        // Read afresh on release, and change only our bit. Never restore a
        // snapshot of unrelated settings the user may have changed meanwhile.
        parameters.twoFingerTapEnabled = want ? FALSE : TRUE;
        SetLastError(ERROR_SUCCESS);
        BOOL set = SystemParametersInfoW(SWS_SPI_SETTOUCHPADPARAMETERS,
                                        sizeof(parameters), &parameters, 0);
        error = set ? ERROR_SUCCESS : GetLastError();
        if (set) s_twoFingerTapRestoreOwned = want;
        read = set;
    }
    if (read) {
        s_twoFingerTapParameterError = ERROR_SUCCESS;
        g_touchpadTwoFingerTapSuppressed.store(want);
        if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_TWO_FINGER_TAP_RESTORE_TIMER_ID);
        if (want) {
            // There is no promoted tap pair to intercept on this path. Retire
            // fallback evidence so a physical mouse click cannot claim it.
            g_touchpadRawTwoFingerTapMouseState.store(0, std::memory_order_release);
        }
        Wh_Log(L"SWS: two-finger Windows tap override active=%d restoreOwned=%d (live only)",
               want, s_twoFingerTapRestoreOwned);
        return;
    }
    if (want && (error == ERROR_INVALID_PARAMETER || error == ERROR_INVALID_SPI_VALUE)) {
        s_twoFingerTapParametersUnavailable = true;
    }
    if (s_twoFingerTapParameterError != error) {
        s_twoFingerTapParameterError = error;
        Wh_Log(L"SWS: two-finger Windows tap %s failed (error=%u)",
               want ? L"override" : L"restore", error);
    }
    // A failed restore must retain ownership and retry, including while hidden.
    if (!want && g_hSwitcher) {
        SetTimer(g_hSwitcher, SWS_TWO_FINGER_TAP_RESTORE_TIMER_ID, 100, nullptr);
    }
}

static ULONGLONG EncodeRawTwoFingerTapMouseState(DWORD serial,
                                                ULONGLONG deadline) {
    return ((ULONGLONG)serial << 32) | (DWORD)deadline;
}

static DWORD RawTwoFingerTapMouseRemainingMs(ULONGLONG state, ULONGLONG now) {
    LONG remaining = (LONG)((DWORD)state - (DWORD)now);
    return state && remaining > 0 ? (DWORD)remaining : 0;
}

// Switcher-thread only. Once a down is consumed, its matching up must also be
// consumed, even if the raw lift closes the last entry or settings disable input.
static DWORD s_rawTwoFingerMouseButtonSerial = 0;
static ULONGLONG s_rawTwoFingerMouseButtonDeadline = 0;

static void UpdateTwoFingerTapMouseHookLifetime() {
    if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_RAW_TWO_TAP_MOUSE_TIMER_ID);
    if (g_isVisible || g_isPendingShow) return;
    ULONGLONG now = GetTickCount64();
    DWORD remaining = 0;
    if (TouchpadHandlingEnabled() && g_touchpadReaderAvailable.load()) {
        remaining = RawTwoFingerTapMouseRemainingMs(
            g_touchpadRawTwoFingerTapMouseState.load(std::memory_order_acquire), now);
    }
    if (s_rawTwoFingerMouseButtonDeadline > now) {
        remaining = (std::max)(remaining,
            (DWORD)(s_rawTwoFingerMouseButtonDeadline - now));
    } else {
        s_rawTwoFingerMouseButtonSerial = 0;
        s_rawTwoFingerMouseButtonDeadline = 0;
    }
    if (g_hMouseHook && remaining && g_hSwitcher &&
        SetTimer(g_hSwitcher, SWS_RAW_TWO_TAP_MOUSE_TIMER_ID, remaining, nullptr)) {
        return;
    }
    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = nullptr;
    }
}

static bool SuppressTwoFingerTapMouse(WPARAM message) {
    if (message != WM_RBUTTONDOWN && message != WM_RBUTTONUP) return false;
    ULONGLONG now = GetTickCount64();
    if (message == WM_RBUTTONUP) {
        bool paired = s_rawTwoFingerMouseButtonSerial &&
                      now < s_rawTwoFingerMouseButtonDeadline;
        DWORD serial = s_rawTwoFingerMouseButtonSerial;
        s_rawTwoFingerMouseButtonSerial = 0;
        s_rawTwoFingerMouseButtonDeadline = 0;
        if (!paired) return false; // Never consume an up whose down passed through.
        ULONGLONG state = g_touchpadRawTwoFingerTapMouseState.load(
            std::memory_order_acquire);
        while ((DWORD)(state >> 32) == serial &&
               !g_touchpadRawTwoFingerTapMouseState.compare_exchange_weak(
                   state, 0, std::memory_order_acq_rel)) {}
        UpdateTwoFingerTapMouseHookLifetime();
        return true;
    }
    if (!TouchpadHandlingEnabled() || !g_touchpadReaderAvailable.load()) return false;
    if (g_touchpadTwoFingerTapSuppressed.load()) return false;
    ULONGLONG state = g_touchpadRawTwoFingerTapMouseState.load(
        std::memory_order_acquire);
    if (!RawTwoFingerTapMouseRemainingMs(state, now)) return false;
    s_rawTwoFingerMouseButtonSerial = (DWORD)(state >> 32);
    s_rawTwoFingerMouseButtonDeadline = now + SWS_RAW_TWO_TAP_MOUSE_PAIR_MS;
    UpdateTwoFingerTapMouseHookLifetime();
    return true;
}

static long long TouchpadTraceAgeMs(ULONGLONG now, ULONGLONG tick) {
    return tick ? (tick <= now ? (long long)(now - tick) : 0) : -1;
}

static void LogTwoFingerTapMouseEvent(WPARAM message,
                                      const MSLLHOOKSTRUCT& mouse,
                                      ULONGLONG stateBefore,
                                      DWORD pairSerialBefore,
                                      bool consumed) {
    ULONGLONG now = GetTickCount64();
    HWND underCursor = WindowFromPoint(mouse.pt);
    auto& stats = s_touchpadInputDiagnostics;
    // Diagnostic only: record the hook payload and reader/UI snapshots. Mouse
    // flags/extra-info are not assumed to identify touchpad input, and neither
    // cursor position nor logging changes the ownership decision.
    Wh_Log(L"SWS TAPTRACE mouse tick=%llu eventTick=%u eventAgeMs=%u message=0x%X consumed=%d flags=0x%X extra=0x%llX "
           L"cursor=%d,%d under=%p inside=%d foreground=%p hook=%p visible=%d pending=%d exit=%d hiding=%d "
           L"enabled=%d available=%d stopping=%d closeActive=%d candidateSerial=%u candidateRemainingMs=%u pairSerial=%u "
           L"uiTips=%d uiTap=%d selected=%d raw=%u reports=%u frames=%u posted=%u uiFrames=%u readerTips=%u "
           L"rawAgeMs=%lld frameAgeMs=%lld uiAgeMs=%lld",
           (unsigned long long)now, mouse.time, (DWORD)now - mouse.time,
           (UINT)message, consumed, mouse.flags, (unsigned long long)mouse.dwExtraInfo,
           mouse.pt.x, mouse.pt.y, underCursor, IsSwitcherWindow(underCursor),
           GetForegroundWindow(), g_hMouseHook, g_isVisible, g_isPendingShow,
           g_animExitActive, g_isHidingSwitcher, g_touchpadGesturesEnabled.load(),
           g_touchpadReaderAvailable.load(), g_touchpadReaderStopping.load(),
           g_touchpadTwoFingerCloseActive.load(), (DWORD)(stateBefore >> 32),
           RawTwoFingerTapMouseRemainingMs(stateBefore, now), pairSerialBefore,
           s_rawGestureTips, s_rawTwoFingerTapActive, g_selectedIndex,
           stats.rawMessages.load(), stats.hidReports.load(), stats.frames.load(),
           stats.posted.load(), stats.uiFrames.load(), stats.lastTips.load(),
           TouchpadTraceAgeMs(now, stats.lastRawTick.load()),
           TouchpadTraceAgeMs(now, stats.lastFrameTick.load()),
           TouchpadTraceAgeMs(now, stats.lastUiTick.load()));
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        ULONGLONG stateBefore = g_touchpadRawTwoFingerTapMouseState.load(
            std::memory_order_acquire);
        DWORD pairSerialBefore = s_rawTwoFingerMouseButtonSerial;
        bool consumed = SuppressTwoFingerTapMouse(wParam);
        if (wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP) {
            LogTwoFingerTapMouseEvent(wParam, *(const MSLLHOOKSTRUCT*)lParam,
                                     stateBefore, pairSerialBefore, consumed);
        }
        if (consumed) {
            g_ctrlTapPending = false;
            Wh_Log(L"SWS: consumed owned two-finger tap mouse event (message=0x%X)", (UINT)wParam);
            return 1;
        }
    }
    if (nCode == HC_ACTION && g_isVisible &&
        (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN)) {
        g_ctrlTapPending = false;
        auto* mouse = (MSLLHOOKSTRUCT*)lParam;
        if (!IsSwitcherWindow(WindowFromPoint(mouse->pt))) {
            Wh_Log(L"SWS TAPTRACE click-away tick=%llu message=0x%X cursor=%d,%d uiTips=%d uiTap=%d selected=%d",
                   (unsigned long long)GetTickCount64(), (UINT)wParam,
                   mouse->pt.x, mouse->pt.y, s_rawGestureTips,
                   s_rawTwoFingerTapActive, g_selectedIndex);
            PostMessageW(g_hSwitcher, WM_SWS_CANCEL_INPUT, 0, 0);
        }
    }
    if (nCode == HC_ACTION && g_isVisible && (wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL)) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
        g_ctrlTapPending = false;
        bool ok = ScrollIs(L"always") || (ScrollIs(L"stickyOnly") && g_isSticky);
        if (ok) {
            bool modActive = false;
            if (wcscmp(g_settings.scrollSecondaryModifier, L"shift") == 0) modActive = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            else if (wcscmp(g_settings.scrollSecondaryModifier, L"ctrl") == 0) modActive = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            else if (wcscmp(g_settings.scrollSecondaryModifier, L"alt") == 0) modActive = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
            const WCHAR* actionStr = modActive ? g_settings.scrollSecondaryAction : g_settings.scrollWheelAction;
            int action = 0;
            if (wcscmp(actionStr, L"selection") == 0) action = 1;
            else if (wcscmp(actionStr, L"page") == 0) action = 2;
            if (!action) {
                // None passes every original packet, including sub-notch input.
                // Do not carry partial owned motion across a pass-through action.
                s_wheelDeltaAccum = s_hwheelDeltaAccum = 0;
                return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
            }
            short rawDelta = (short)HIWORD(pMouseStruct->mouseData);
            int notches = 0;
            if (wParam == WM_MOUSEWHEEL) {
                s_wheelDeltaAccum += rawDelta;
                while (s_wheelDeltaAccum >= WHEEL_DELTA) {
                    notches--; // positive wheel = scroll up / previous
                    s_wheelDeltaAccum -= WHEEL_DELTA;
                }
                while (s_wheelDeltaAccum <= -WHEEL_DELTA) {
                    notches++; // negative wheel = scroll down / next
                    s_wheelDeltaAccum += WHEEL_DELTA;
                }
            } else if (wParam == WM_MOUSEHWHEEL) {
                s_hwheelDeltaAccum += rawDelta;
                while (s_hwheelDeltaAccum >= WHEEL_DELTA) {
                    notches++; // positive tilt = scroll right / next
                    s_hwheelDeltaAccum -= WHEEL_DELTA;
                }
                while (s_hwheelDeltaAccum <= -WHEEL_DELTA) {
                    notches--; // negative tilt = scroll left / previous
                    s_hwheelDeltaAccum += WHEEL_DELTA;
                }
            }

            if (notches != 0) {
                if (g_settings.reverseScrollDirection) notches = -notches;
                s_lastTouchpadScrollTick = GetTickCount64();
                PostMessage(g_hSwitcher, WM_SWS_SCROLL, (WPARAM)notches, (LPARAM)action);
                return 1;
            } else {
                // Absorbed sub-notch delta, swallow to prevent background window scrolling
                s_lastTouchpadScrollTick = GetTickCount64();
                return 1;
            }
        } else {
            s_wheelDeltaAccum = s_hwheelDeltaAccum = 0;
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

// Show the switcher, its overlay and the mirrors, then start the entrance animation.
// Kept in one place because both the pending-show reveal and the immediate show use it.
static void PresentSwitcherWindows() {
    // All HWNDs must carry frame zero before any of them becomes visible.
    ApplyPresentationWindowOffset();
    if (g_hCloseBtnWnd) {
        ShowWindow(g_hCloseBtnWnd, SW_SHOWNA);
    }
    ShowWindow(g_hSwitcher, SW_SHOWNA);
    BringWindowToTop(g_hSwitcher);
    if (g_isTouchpadGestureActive) {
        Wh_Log(L"SWS: touchpad presentation context %s",
               FormatTouchpadForegroundDiagnostic(GetForegroundWindow(), g_hSwitcher).c_str());
        // Best-effort legacy foreground request; SendInput's return alone does
        // not prove last-input eligibility or routing across an elevated app.
        if (!s_nativeTouchpadInvocation.token) TapUnassignedKeyForForeground();
    }
    BOOL foreground = SetForegroundWindow(g_hSwitcher);
    if (g_isTouchpadGestureActive && GetForegroundWindow() != g_hSwitcher) {
        SwitchToThisWindow(g_hSwitcher, TRUE);
        foreground = GetForegroundWindow() == g_hSwitcher;
    }
    Wh_Log(L"SWS: presented switcher foreground=%d active=%d", foreground,
           GetForegroundWindow() == g_hSwitcher);
    // Mirrors already carry frame 0 (pushed by PaintSwitcher above); show them
    // now so no unpainted white frame is ever composed.
    ShowMirrorSwitchers();

    if (g_animEntranceActive) {
        StartAnimationTicker();
    } else {
        PaintSwitcher();
        UpdateWindow(g_hSwitcher);
        if (g_hCloseBtnWnd) {
            PaintSwitcherOverlay();
            UpdateWindow(g_hCloseBtnWnd);
        }
    }
}

// A Task View window can briefly win the system-tools z-order after the raw
// reader has already opened SWS. Restore our topmost presentation without
// requiring foreground activation; the latter is unavailable across UIPI.
static void ReassertVisibleSwitcherPresentation() {
    if (!g_hSwitcher || !g_isVisible || !IsWindow(g_hSwitcher)) return;
    RECT rc = {};
    if (!GetWindowRect(g_hSwitcher, &rc) || rc.right <= rc.left || rc.bottom <= rc.top) {
        return;
    }
    PositionPresentationWindow(g_hSwitcher, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    BringWindowToTop(g_hSwitcher);
    if (g_hCloseBtnWnd) {
        AnchorPresentationOverlay();
        PositionPresentationWindow(g_hCloseBtnWnd, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    BOOL foreground = SetForegroundWindow(g_hSwitcher);
    Wh_Log(L"SWS: reasserted visible switcher foreground=%d active=%d",
           foreground, GetForegroundWindow() == g_hSwitcher);
}

static void RevealPendingSwitcher() {
    if (!g_isPendingShow || !g_hSwitcher) {
        return;
    }

    KillTimer(g_hSwitcher, SWS_SHOW_DELAY_TIMER_ID);

    g_isPendingShow = false;
    // The grace-period HWND may be visible off-screen. Hide it before moving
    // native materials to their layout rect, which would otherwise reveal the
    // switcher ahead of ShowBackdropBlur(). No session is visible during this
    // synchronous staging step, so its focus loss cannot cancel the reveal.
    if (BackdropBlurEnabled() && IsWindowVisible(g_hSwitcher)) {
        ShowWindow(g_hSwitcher, SW_HIDE);
    }
    g_isVisible = true;
    RefreshTouchpadGestureKinds();
    if (!g_hMouseHook) {
        g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
        Wh_Log(L"SWS TAPTRACE mouse hook installed (reveal hook=%p error=%u)",
               g_hMouseHook, g_hMouseHook ? ERROR_SUCCESS : GetLastError());
    }

    // Re-resolve the final switcher rect before reading it.
    // CycleLinear (called from the WM_HOTKEY backward path during pending show)
    // may have called ScrollDockWithDynamicResize / UpdateDockSelectionWithDynamicResize
    // which recompute layout while the switcher is still off-screen at (-32000, -32000).
    // That captures rcWndStart from GetWindowRect at the off-screen position, making the
    // entrance animation interpolate from (-32000, -32000) → final position — the
    // "flies from top-left" bug. Refreshing source sizes and recomputing layout here
    // ensures g_winW/g_winH and g_pendingSwitcherRect are at the true final values
    // before SetWindowPos is called, so rcWndStart == rcWndTarget == final position.
    if (g_hCurrentMonitor) {
        for (auto& w : g_windows) {
            RefreshEntrySourceSize(w);
        }
        HMONITOR hMon = g_hCurrentMonitor;
        ComputeLayout(hMon);
        if (DockLayoutActive()) UpdateDockPreviewForSelection();
        MONITORINFO rmi = { sizeof(rmi) };
        GetMonitorInfoW(hMon, &rmi);
        int rcx, rcy;
        GetSwitcherPosition(rmi.rcWork, &rcx, &rcy);
        g_pendingSwitcherRect = { rcx, rcy, rcx + g_winW, rcy + g_winH };
        if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
            RectF rr = ToRectF(g_windows[g_selectedIndex].rcCell);
            SnapSelectionTo(rr);
        }
    }

    int x = g_pendingSwitcherRect.left;
    int y = g_pendingSwitcherRect.top;
    int w = g_pendingSwitcherRect.right - g_pendingSwitcherRect.left;
    int h = g_pendingSwitcherRect.bottom - g_pendingSwitcherRect.top;
    g_switcherBaseX = x;
    g_switcherBaseY = y;
    g_switcherBaseInitialized = true;

    // Restore standard DWM non-client rendering policy
    DWMNCRENDERINGPOLICY enabled = DWMNCRP_ENABLED;
    DwmSetWindowAttribute(g_hSwitcher, DWMWA_NCRENDERING_POLICY, &enabled, sizeof(enabled));

    ApplyThemeToWindow(g_hSwitcher);
    ApplySwitcherRegion();

    StartEntranceAnimation();
    SetPresentationWindowLayout(g_hSwitcher, x, y, w, h, SWP_FRAMECHANGED | SWP_NOACTIVATE);
    CreateMirrorSwitchers();
    if (g_hCloseBtnWnd) {
        SetPresentationWindowLayout(g_hCloseBtnWnd, x, y, w, h);
    }
    g_pendingSwitcherRect = { 0, 0, 0, 0 };

    if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        RectF r = ToRectF(g_windows[g_selectedIndex].rcCell);
        SnapSelectionTo(r);
    }
    g_scrollTransition.active = false;
    g_scrollTransition.offsetCurrentX = 0.0f;
    g_scrollTransition.offsetCurrentY = 0.0f;
    g_scrollTransition.outgoingItems.clear();
    g_scrollTransition.preservingThumbnails = false;
    g_animHoverAlphaCurrent = 0.0f;
    g_animHoverActive = false;

    UpdateChevronLayout(g_hSwitcher);
    UpdateChevronAnimationTargets(true);

    if (DockLayoutActive()) {
        UpdateDockPreviewForSelection();
    }
    RegisterThumbnails();
    // Render initial frame 0 for BOTH switcher and overlay while windows are hidden
    // so no stale borders, contours, or frames flash!
    PaintSwitcher();
    if (g_hCloseBtnWnd) {
        PaintSwitcherOverlay();
    }

    // Backdrop first (opt-in), then the switcher, its overlay and the mirrors in the
    // same message: presentation must never wait on a timer, or a lost tick would leave
    // the full-screen blur plate up with no switcher on top of it.
    ShowBackdropBlur();
    PresentSwitcherWindows();

    if (!g_isSticky && !g_isTouchpadGestureActive) {
        SetTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID, 50, NULL);
    } else {
        KillTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID);
    }
    SetTimer(g_hSwitcher, SWS_DYNAMIC_RESIZE_TIMER_ID, 120, NULL);
    // Deferred auto-drill for Alt+Backtick "sameApp" + grouping (issue #5532). The
    // reveal is complete now, so it is safe to expand the current app's group.
    if (g_drillInAfterReveal) {
        g_drillInAfterReveal = false;
        if (g_settings.showApplications && !g_drilledIn &&
            g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() &&
            g_windows[g_selectedIndex].groupWindows.size() > 1) {
            EnterAppGroup();
        }
    }

}

static void ApplyThemeToWindow(HWND hWnd) {
    if (ThemeIs(L"none")) {
        // 1. Explicitly clear all DWM system backdrops on Windows 11
        if (IsWin11OrGreater()) {
            int noneVal = 1; // DWMSBT_NONE
            DwmSetWindowAttribute(hWnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &noneVal, sizeof(noneVal));
            int disableMica = 0;
            DwmSetWindowAttribute(hWnd, 1029 /* DWMWA_MICA_EFFECT */, &disableMica, sizeof(disableMica));
        }

        // 2. Clear legacy composition accent blur (Windows 10)
        if (g_SetWindowCompositionAttribute) {
            ACCENT_POLICY a = {}; a.AccentState = 0;
            WINDOWCOMPOSITIONATTRIBDATA d = {19, &a, sizeof(a)};
            g_SetWindowCompositionAttribute(hWnd, &d);
        }

        // 3. Reset glass margins to zero
        MARGINS marZero = {0, 0, 0, 0};
        DwmExtendFrameIntoClientArea(hWnd, &marZero);

        // 4. Reset layered style so UpdateLayeredWindow doesn't conflict with previous SetLayeredWindowAttributes
        LONG_PTR exs = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exs & ~WS_EX_LAYERED);
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exs | WS_EX_LAYERED);
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

        // 5. Reset class background brush so GDI doesn't paint black
        SetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)GetStockObject(NULL_BRUSH));

        // 6. Disable DWM hardware corner clipping and hardware border.
        // In 'none' style, MaskRectCorners and DrawSwitcherOuterBorder provide pure 
        // 32-bit layered anti-aliased geometry supporting any custom radius (e.g. 0 to 32px+).
        if (IsWin11OrGreater()) {
            INT donotround = 1; // DWMWCP_DONOTROUND
            DwmSetWindowAttribute(hWnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &donotround, sizeof(donotround));
            COLORREF noneColor = 0xFFFFFFFE; // DWMWA_COLOR_NONE
            DwmSetWindowAttribute(hWnd, 34 /* DWMWA_BORDER_COLOR */, &noneColor, sizeof(noneColor));
        }

        // 7. Flush style and DWM non-client state
        ApplyAcrylicWindowRegion(hWnd);
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        return;
    }

    // --- Non-layered path (Mica / Acrylic) ---
    // Strip WS_EX_LAYERED: Windows DWM never renders hardware system backdrops
    // (Mica DWMSBT_MAINWINDOW) or Windows 10/11 acrylic composition blur on
    // windows with the WS_EX_LAYERED style.
    LONG_PTR exs = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exs & WS_EX_LAYERED) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exs & ~WS_EX_LAYERED);
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    // Reset legacy accent policy by default
    if (g_SetWindowCompositionAttribute) {
        ACCENT_POLICY a = {}; a.AccentState = 0;
        WINDOWCOMPOSITIONATTRIBDATA d = {19, &a, sizeof(a)};
        g_SetWindowCompositionAttribute(hWnd, &d);
    }

    BOOL dark = g_isDarkMode;
    if (IsWin11OrGreater()) {
        DwmSetWindowAttribute(hWnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark, sizeof(dark));
    }

    if (ThemeIs(L"mica")) {
        if (IsWin11OrGreater()) {
            int micaVal = 2; // DWMSBT_MAINWINDOW
            HRESULT hr = DwmSetWindowAttribute(hWnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &micaVal, sizeof(micaVal));
            if (FAILED(hr)) {
                int oldMicaVal = 1;
                hr = DwmSetWindowAttribute(hWnd, 1029 /* DWMWA_MICA_EFFECT */, &oldMicaVal, sizeof(oldMicaVal));
            }
            if (FAILED(hr)) {
                // Switch the renderer as well as the window style. A layered
                // window with the non-layered paint path otherwise stays blank.
                wcsncpy_s(g_settings.theme, L"backdrop", _TRUNCATE);
                UpdateCachedSettings();
                InvalidateStaticCache();
                ApplyThemeToWindow(hWnd);
                return;
            }
            SendMessage(hWnd, WM_NCACTIVATE, TRUE, 0);
        }
    } else if (ThemeIs(L"backdrop")) {
        // Clear Windows 11 hardware system backdrops to prevent Desktop Acrylic fallback interference
        if (IsWin11OrGreater()) {
            int noneVal = 1; // DWMSBT_NONE
            DwmSetWindowAttribute(hWnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &noneVal, sizeof(noneVal));
            int disableMica = 0;
            DwmSetWindowAttribute(hWnd, 1029 /* DWMWA_MICA_EFFECT */, &disableMica, sizeof(disableMica));
        }
        // SetWindowCompositionAttribute Acrylic blur behind (supported across Windows 10 and Windows 11)
        if (g_SetWindowCompositionAttribute) {
            DWORD blur = (DWORD)((g_settings.opacity / 100.0) * 255);
            COLORREF bg = GetBgColor();
            ACCENT_POLICY accent = {};
            accent.AccentState = 4 /* ACCENT_ENABLE_ACRYLICBLURBEHIND */;
            accent.AccentFlags = 0;
            accent.GradientColor = (blur << 24) | (bg & 0x00FFFFFF);
            WINDOWCOMPOSITIONATTRIBDATA data = {19, &accent, sizeof(accent)};
            g_SetWindowCompositionAttribute(hWnd, &data);
        }
    }

    MARGINS marGlassInset = ThemeIs(L"mica") ? MARGINS{-1, -1, -1, -1} : MARGINS{0, 0, 0, 0};
    DwmExtendFrameIntoClientArea(hWnd, &marGlassInset);

    SetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)GetStockObject(BLACK_BRUSH));

    if (IsWin11OrGreater()) {
        INT cp = GetCornerPref();
        DwmSetWindowAttribute(hWnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &cp, sizeof(cp));
        // Always suppress DWM hardware borders. SWS renders its own anti-aliased,
        // alpha-fadeable outer border inside the layered overlay window (g_hCloseBtnWnd).
        COLORREF none = 0xFFFFFFFE; // DWMWA_COLOR_NONE
        DwmSetWindowAttribute(hWnd, 34 /* DWMWA_BORDER_COLOR */, &none, sizeof(none));
    }

    ApplyAcrylicWindowRegion(hWnd);
    SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

static BOOL WINAPI MirrorEnumProc(HMONITOR hM, HDC, LPRECT, LPARAM) {
    if (hM != g_hCurrentMonitor) {
        MONITORINFO mInfo = { sizeof(mInfo) };
        GetMonitorInfoW(hM, &mInfo);
        int mx, my;
        GetSwitcherPosition(mInfo.rcWork, &mx, &my);
        DWORD mirrorExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | (ThemeIs(L"none") ? WS_EX_LAYERED : 0);
        HWND hMirror = CreateSWSWindow(mirrorExStyle, SWS_CLASSNAME, L"", WS_POPUP | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, mx, my, g_winW, g_winH, g_hSwitcher, NULL, GetModuleHandle(NULL), NULL);
        if (hMirror) {
            ApplyThemeToWindow(hMirror);
            g_hMirrorSwitchers.push_back(hMirror);
            SetPresentationWindowLayout(hMirror, mx, my, g_winW, g_winH);
            // NOTE: mirrors stay hidden here on purpose. They are shown by
            // ShowMirrorSwitchers() only after PaintSwitcher() has pushed frame 0
            // into them, so DWM never composes an unpainted (white border) frame.
        }
    }
    return TRUE;
}

static void DestroyMirrorSwitchers() {
    for (HWND hMirror : g_hMirrorSwitchers) {
        g_presentationWindows.erase(hMirror);
        if (IsWindow(hMirror)) {
            if (IsWin11OrGreater()) {
                COLORREF colorNone = 0xFFFFFFFE;
                DwmSetWindowAttribute(hMirror, 34 /* DWMWA_BORDER_COLOR */, &colorNone, sizeof(colorNone));
            }
            ShowWindow(hMirror, SW_HIDE);
            DestroyWindow(hMirror);
        }
    }
    g_hMirrorSwitchers.clear();
}

static void SetSwitcherLayoutBounds(int x, int y, int w, int h, UINT flags) {
    if (g_isPendingShow) {
        g_pendingSwitcherRect = { x, y, x + w, y + h };
        return;
    }
    SetPresentationWindowLayout(g_hSwitcher, x, y, w, h, flags);
    g_switcherBaseX = x;
    g_switcherBaseY = y;
    g_switcherBaseInitialized = true;
    for (HWND hMirror : g_hMirrorSwitchers) {
        if (!IsWindow(hMirror)) continue;
        RECT layout = {};
        if (!GetPresentationLayoutRect(hMirror, &layout)) continue;
        MONITORINFO mi = { sizeof(mi) };
        if (!GetMonitorInfoW(MonitorFromRect(&layout, MONITOR_DEFAULTTONEAREST), &mi)) continue;
        int mx, my;
        GetSwitcherPosition(mi.rcWork, &mx, &my, w, h);
        SetPresentationWindowLayout(hMirror, mx, my, w, h);
    }
    AnchorPresentationOverlay();
    PositionPresentationWindow(g_hCloseBtnWnd);
}

static void CreateMirrorSwitchers() {
    if (wcscmp(g_settings.switcherDisplayBehavior, L"allMonitors") == 0 || g_showAllMonitors) {
        EnumDisplayMonitors(NULL, NULL, MirrorEnumProc, 0);
    }
}

// Shows previously created (hidden) mirrors after their first frame has been
// painted. SW_SHOWNA keeps them non-activated so the main switcher window
// retains foreground ownership established by SetForegroundWindow().
static void ShowMirrorSwitchers() {
    for (HWND hMirror : g_hMirrorSwitchers) {
        if (IsWindow(hMirror) && !IsWindowVisible(hMirror)) {
            ShowWindow(hMirror, SW_SHOWNA);
        }
    }
}


static void ApplyAcrylicWindowRegion(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;

    // The legacy Acrylic accent can bypass DWM's corner-preference hint, even
    // on Windows 11. Clip it to the same resolved system/custom radius as the
    // border. Mica retains DWM clipping; None uses its final per-pixel mask.
    int radiusDIP = 0;
    GetResolvedCornerRadiiDIP(&radiusDIP, nullptr);
    UINT dpi = QueryWindowDpi(hWnd);
    int radius = ThemeIs(L"backdrop") ? MulDiv(radiusDIP, dpi ? dpi : g_dpiX, 96) : 0;
    RECT wr = {};
    if (!GetWindowRect(hWnd, &wr)) return;
    int w = wr.right - wr.left;
    int h = wr.bottom - wr.top;
    if (radius > 0 && w > 0 && h > 0) {
        HRGN region = CreateRoundRectRgn(0, 0, w + 1, h + 1,
                                         radius * 2, radius * 2);
        if (region && !SetWindowRgn(hWnd, region, TRUE)) {
            DeleteObject(region);
        }
    } else {
        // Avoid a needless frame change on every None/Mica resize.
        HRGN existing = CreateRectRgn(0, 0, 0, 0);
        if (existing) {
            if (GetWindowRgn(hWnd, existing) != ERROR) SetWindowRgn(hWnd, NULL, TRUE);
            DeleteObject(existing);
        }
    }
}

static void ApplySwitcherRegion() {
    if (!g_hSwitcher) return;

    ApplyAcrylicWindowRegion(g_hSwitcher);
    if (IsWin11OrGreater()) {
        COLORREF colorNone = 0xFFFFFFFE; // DWMWA_COLOR_NONE
        DwmSetWindowAttribute(g_hSwitcher, 34 /* DWMWA_BORDER_COLOR */, &colorNone, sizeof(colorNone));
    }
}

static void ShowSwitcher(bool sticky, bool immediate = false, HWND invocationSource = nullptr) {
    // A new invocation supersedes a previous denied activation request.
    if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_TOUCHPAD_TARGET_FOCUS_RETRY_TIMER_ID);
    s_touchpadActivationRetryTarget = NULL;
    s_touchpadActivationRetryDeadline = 0;
    s_touchpadCommitActivation = false;
    // Publish intent before layout/icon work so the raw reader can mark a
    // three-finger stroke during this invocation's scheduling window.
    g_touchpadSwitcherActive.store(true);
    PublishNativeSwipePolicy();
    HWND invocationWindow = GetAncestor(
        invocationSource ? invocationSource : GetForegroundWindow(), GA_ROOTOWNER);
    g_sameAppSessionKey[0] = L'\0';
    if (g_isAltBacktickSameApp) {
        GetWindowGroupKey(invocationWindow, g_sameAppSessionKey, ARRAYSIZE(g_sameAppSessionKey));
    }
    int oldDwmRadius = g_systemDwmRadius;
    int oldDwmSmallRadius = g_systemDwmSmallRadius;
    DetectSystemDwmCornerRadius();
    if (g_systemDwmRadius != oldDwmRadius || g_systemDwmSmallRadius != oldDwmSmallRadius) {
        LoadSettings();
        if (g_hSwitcher) {
            ApplyThemeToWindow(g_hSwitcher);
        }
        InvalidateStaticCache();
        FreeCachedBuffers();
    }

    RefreshClientAreaAnimCache();
    DestroyMirrorSwitchers();

    POINT pt; GetCursorPos(&pt);
    HMONITOR hMon = (wcscmp(g_settings.switcherDisplayBehavior, L"primaryOnly") == 0) ?
                    MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY) :
                    MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    g_hCurrentMonitor = hMon;
    UpdateRefreshRateTiming();
    UnregisterThumbnails(); BuildWindowList();
    
    if (g_windows.empty()) {
        HideSwitcher();
        return;
    }

    g_isDarkMode = ShouldUseDarkMode(); g_isSticky = sticky;

    g_layoutStartIndex = 0; // Always start from the first window on initial show
    g_drilledIn = false;
    g_savedAppList.clear();
    g_consumeEscUp = false;
    g_drillInAfterReveal = false;
    g_selectedIndex = 0;
    for (int i = 0; i < (int)g_windows.size(); ++i) {
        const auto& e = g_windows[i];
        if (e.hWnd == invocationWindow ||
            std::find(e.groupWindows.begin(), e.groupWindows.end(), invocationWindow) != e.groupWindows.end()) {
            g_selectedIndex = (i + 1) % (int)g_windows.size();
            break;
        }
    }
    g_hoverIndex = -1;
    g_hoverThumbIndex = -1;
    g_hoverWnd = NULL;
    g_isCloseHovered = false;

    RegisterThumbnailsEarly();
    // DWM often hasn't produced a surface for just-registered thumbnails yet,
    // so the query inside RegisterThumbnailsEarly can leave sourceSize at 0
    // (1x1 placeholder). Fall back to the live window rect for aspect so the
    // FIRST layout — and therefore the initial centered position, which matters
    // most for Dock's single-preview sizing — is already near-final. This avoids
    // the "appears top-right then slides to center" correction right after reveal.
    for (auto& w : g_windows) {
        RefreshEntrySourceSize(w);
    }
    ComputeLayout(hMon);
    if (g_winW <= 0 || g_winH <= 0) {
        // A zero-sized layout means we never reveal. RegisterThumbnailsEarly above
        // already registered DWM thumbnails for the new list; release them so this
        // failed show doesn't leak HTHUMBNAIL handles.
        UnregisterThumbnails();
        g_touchpadSwitcherActive.store(false);
        s_altSessionOwner = false;
        s_altHeld = false;
        s_altRawBaselineSerial = 0;
        s_combinedRawSerial = 0;
        s_combinedRawSeen = false;
        s_combinedRawLiftHandled = false;
        s_commitStarted = false;
        PublishNativeSwipePolicy();
        return;
    }
    if (DockLayoutActive()) {
        UpdateDockPreviewForSelection();
    }
    InvalidateStaticCache();

    if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        RectF r = ToRectF(g_windows[g_selectedIndex].rcCell);
        SnapSelectionTo(r);
    }
    g_scrollTransition.active = false;
    g_scrollTransition.offsetCurrentX = 0.0f;
    g_scrollTransition.offsetCurrentY = 0.0f;
    g_scrollTransition.outgoingItems.clear();
    g_scrollTransition.preservingThumbnails = false;
    g_animHoverAlphaCurrent = 0.0f;
    g_animHoverActive = false;

    // Recreate font for current DPI
    if (g_hFont) { DeleteObject(g_hFont); g_hFont = NULL; }
    g_hFont = CreateScaledFont(g_dpiY);

    CancelPendingShow();

    MONITORINFO mi = { sizeof(mi) }; GetMonitorInfoW(hMon, &mi);
    int cx, cy;
    GetSwitcherPosition(mi.rcWork, &cx, &cy);
    g_switcherBaseX = cx;
    g_switcherBaseY = cy;
    g_switcherBaseInitialized = true;

    g_pendingSwitcherRect = {
        cx,
        cy,
        cx + g_winW,
        cy + g_winH
    };

    if (!s_hWinEventHook) {
        s_hWinEventHook = SetWinEventHook(
            EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE,
            NULL, WinEventShowHideProc,
            0, 0,
            WINEVENT_OUTOFCONTEXT
        );
    }
    if (!s_hForegroundEventHook) {
        s_hForegroundEventHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
            NULL, WinEventShowHideProc,
            0, 0,
            WINEVENT_OUTOFCONTEXT
        );
    }

    // Build the blurred backdrop now, while every SWS window is still hidden: the
    // capture then sees the clean desktop and no SWS window can end up in its own
    // backdrop (see PrepareBackdropBlur for the details).
    PrepareBackdropBlur();

    constexpr int kRapidAltTabGraceThresholdMs = 75;
    int effectiveDelay = (!sticky && !immediate) ? ((g_settings.showDelay > 0) ? std::max(g_settings.showDelay, kRapidAltTabGraceThresholdMs) : kRapidAltTabGraceThresholdMs) : 0;
    if (effectiveDelay > 0) {
        g_isPendingShow = true;
        g_isVisible = false;
        RefreshTouchpadGestureKinds();

        if (ThemeIs(L"none")) {
            LONG_PTR exStyle = GetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE);
            if (!(exStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
            }
            SetLayeredWindowAttributes(g_hSwitcher, 0, 0, LWA_ALPHA);
            SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
        } else {
            // Non-layered (Mica / Acrylic): Keep window off-screen during grace period
            // to avoid corrupting DWM hardware backdrop state with WS_EX_LAYERED.
            SetWindowPos(g_hSwitcher, HWND_TOPMOST, -32000, -32000, g_winW, g_winH, SWP_NOACTIVATE);
        }
        ShowWindow(g_hSwitcher, SW_SHOWNA);
        BringWindowToTop(g_hSwitcher);

        // Establishing foreground ownership ensures UIPI does not block input tracking
        // (WM_KEYUP, GetAsyncKeyState) when invoked over elevated/Admin windows
        SetForegroundWindow(g_hSwitcher);

        if (g_hCloseBtnWnd) {
            ShowWindow(g_hCloseBtnWnd, SW_HIDE);
        }

        if (!sticky && !g_isTouchpadGestureActive) {
            SetTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID, 50, NULL);
        }
        SetTimer(g_hSwitcher, SWS_SHOW_DELAY_TIMER_ID, effectiveDelay, NULL);
        return;
    }

    g_isPendingShow = false;
    g_pendingSwitcherRect = { 0, 0, 0, 0 };
    if (BackdropBlurEnabled() && IsWindowVisible(g_hSwitcher)) {
        ShowWindow(g_hSwitcher, SW_HIDE);
    }
    g_isVisible = true;
    RefreshTouchpadGestureKinds();
    if (!g_hMouseHook) {
        g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
        Wh_Log(L"SWS TAPTRACE mouse hook installed (show hook=%p error=%u)",
               g_hMouseHook, g_hMouseHook ? ERROR_SUCCESS : GetLastError());
    }

    ApplyThemeToWindow(g_hSwitcher);
    ApplySwitcherRegion();

    StartEntranceAnimation();
    SetPresentationWindowLayout(g_hSwitcher, cx, cy, g_winW, g_winH);
    CreateMirrorSwitchers();
    
    if (g_hCloseBtnWnd) {
        SetPresentationWindowLayout(g_hCloseBtnWnd, cx, cy, g_winW, g_winH);
    }

    UpdateChevronLayout(g_hSwitcher);
    UpdateChevronAnimationTargets(true);
    RegisterThumbnails();
    // Render initial frame 0 for BOTH switcher and overlay while windows are hidden
    // so no stale borders, contours, or frames flash!
    PaintSwitcher();
    if (g_hCloseBtnWnd) {
        PaintSwitcherOverlay();
    }

    // Backdrop first (opt-in), then the switcher, its overlay and the mirrors in the
    // same message: presentation must never wait on a timer, or a lost tick would leave
    // the full-screen blur plate up with no switcher on top of it.
    ShowBackdropBlur();
    PresentSwitcherWindows();

    if (!sticky && !g_isTouchpadGestureActive) {
        SetTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID, 50, NULL);
    }
    SetTimer(g_hSwitcher, SWS_DYNAMIC_RESIZE_TIMER_ID, 120, NULL);
}

static void HideSwitcher() {
    if (g_isHidingSwitcher) return;
    g_touchpadTwoFingerCloseActive.store(false);
    g_touchpadSwitcherActive.store(false);
    // A cancelled session must not let the still-held stroke re-enter through
    // the Explorer-side native source gate. The reader will publish the next
    // candidate only after a complete lift and a fresh contact edge.
    ClearRawThreeFingerCandidateProperty();
    PublishNativeSwipePolicy();
    g_isHidingSwitcher = true;
    Wh_Log(L"SWS: hiding session (sticky=%d, raw=%d, tips=%d, foreground=%p)",
           g_isSticky, s_rawSessionOwned, s_rawGestureTips, GetForegroundWindow());
    CancelRawTouchpadStroke();
    RefreshTouchpadGestureKinds();
    // UI dismissal is not a physical lift. The reader owns both markers until
    // this stroke's release grace expires or a new outside stroke replaces it.
    // A cancelled owned stroke must not release its native tap action either.
    UpdateRawSwipeMarkerTimer();
    if (g_hSwitcher) {
        KillTimer(g_hSwitcher, SWS_DYNAMIC_RESIZE_TIMER_ID);
    }
    // Hide every switcher window FIRST, before any teardown or WS_EX_LAYERED /
    // DWM attribute juggling below, so DWM can never compose an intermediate
    // (white border / unpainted / half-torn-down) frame on exit.
    HideSwitcherPresentationWindows();
    DestroyMirrorSwitchers();
    HideBackdropBlur();
    StopAnimationTicker();
    FinishAnimations();
    FreeCachedBuffers();

    g_showAllMonitors = false;
    CancelPendingShow();
    g_switcherBaseInitialized = false;

    UnregisterThumbnails();
    if (g_hCloseBtnWnd) {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 0, AC_SRC_ALPHA };
        UpdateLayeredWindow(g_hCloseBtnWnd, NULL, NULL, NULL, NULL, NULL, 0, &bf, ULW_ALPHA);
        ShowWindow(g_hCloseBtnWnd, SW_HIDE);
    }
    if (g_hSwitcher) {
        if (IsWin11OrGreater()) {
            COLORREF colorNone = 0xFFFFFFFE; // DWMWA_COLOR_NONE
            DwmSetWindowAttribute(g_hSwitcher, 34 /* DWMWA_BORDER_COLOR */, &colorNone, sizeof(colorNone));
        }
        LONG_PTR exStyle = GetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE);
        if (!(exStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        }
        SetLayeredWindowAttributes(g_hSwitcher, 0, 0, LWA_ALPHA);
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 0, AC_SRC_ALPHA };
        UpdateLayeredWindow(g_hSwitcher, NULL, NULL, NULL, NULL, NULL, 0, &bf, ULW_ALPHA);
        ShowWindow(g_hSwitcher, SW_HIDE);
        if (exStyle & WS_EX_TRANSPARENT) {
            SetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
        }
    }

    g_animExitActive = false;
    g_animExitProgress = 1.0f;
    g_animExitCurrentAlpha = 1.0f;

    g_animChevronAlphaPrev = 0.0f;
    g_animChevronAlphaNext = 0.0f;
    g_animChevronTransformPrev = 0.0f;
    g_animChevronTransformNext = 0.0f;
    g_chevronRevealPrev = {};
    g_chevronRevealNext = {};
    g_animChevronAlphaTargetPrev = 0.0f;
    g_animChevronAlphaTargetNext = 0.0f;
    g_animChevronProgressPrev = 1.0f;
    g_animChevronProgressNext = 1.0f;
    g_animChevronHoverAlphaPrev = 0.0f;
    g_animChevronHoverAlphaNext = 0.0f;
    g_animCloseBtnAlpha = 0.0f;
    g_animCloseBtnHoverAlpha = 0.0f;
    for (auto& w : g_windows) {
        SnapCloseButtonMotion(w, 0.0f);
    }
    if (g_hSwitcher && GetCapture() == g_hSwitcher) {
        ReleaseCapture();
    }
    g_isDragging = false;
    g_pressedIndex = -1;
    g_pressedWindow = nullptr;
    g_ctrlTapPending = false;
    g_isClosePressed = false;
    g_hoverChevron = 0;
    g_pressedChevron = 0;
    ResetScrollWheelAccumulators();
    g_hoverIndex = -1;
    g_hoverThumbIndex = -1;
    g_hoverWnd = NULL;

    g_isVisible = false;
    g_isPendingShow = false;
    // UI teardown is not the end of the already-owned tap's mouse pair.
    // The short timer keeps only its input filter alive, never its foreground.
    UpdateTwoFingerTapMouseHookLifetime();
    g_isSticky = false;
    g_isAltBacktickSameApp = false;
    g_sameAppSessionKey[0] = L'\0';
    RefreshTouchpadGestureKinds();
    g_drilledIn = false;
    g_savedAppList.clear();
    g_consumeEscUp = false;
    g_drillInAfterReveal = false;
    g_isPaginatedView = false;
    g_isTouchpadGestureActive = false;
    s_altSessionOwner = false;
    s_altHeld = false;
    s_altRawBaselineSerial = 0;
    s_combinedRawSerial = 0;
    s_combinedRawSeen = false;
    s_combinedRawLiftHandled = false;
    s_commitStarted = false;
    if (g_hSwitcher) {
        KillTimer(g_hSwitcher, SWS_CLOSE_VERIFY_TIMER_ID);
        KillTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID);
        KillTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID);
    }
    s_pendingCloseWindows.clear();
    s_pendingCloseRetries = 0;
    g_isHidingSwitcher = false;
    // Once hidden, a background controller cannot suppress shell gestures.
    // Explorer's source-specific PTP hook covers the reader's release grace.
    RefreshTouchpadGestureKinds();
}

// Never wait synchronously on another application's window procedure. These
// requests remain subject to UIPI; a queued restore is not proof of activation.
static void RestoreWindowIfIconic(HWND hWnd) {
    if (IsIconic(hWnd)) {
        if (!ShowWindowAsync(hWnd, SW_RESTORE)) {
            PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
        }
    }
}

static void ActivateExitedWindow(HWND hTarget, const std::vector<HWND>& restoreWindows) {
    if (!hTarget || !IsWindow(hTarget)) {
        s_touchpadCommitActivation = false;
        return;
    }

    for (HWND hWindow : restoreWindows) {
        if (IsWindow(hWindow) && hWindow != hTarget && IsIconic(hWindow)) {
            ShowWindowAsync(hWindow, SW_SHOWNOACTIVATE);
        }
    }

    const bool retrying = s_touchpadActivationRetryTarget != NULL;
    HWND hPopup = GetLastActivePopup(hTarget);
    HWND hForegroundTarget = IsWindowVisible(hPopup) ? hPopup : hTarget;
    RestoreWindowIfIconic(hTarget);
    if (hForegroundTarget != hTarget) RestoreWindowIfIconic(hForegroundTarget);
    if (s_touchpadCommitActivation && !retrying) {
        Wh_Log(L"SWS: touchpad commit context %s",
               FormatTouchpadForegroundDiagnostic(GetForegroundWindow(), hForegroundTarget).c_str());
        // Make one foreground-eligibility request for this handoff, not one
        // synthetic key for every rejected retry (which can keep stealing
        // last-input eligibility while a different app owns the foreground).
        TapUnassignedKeyForForeground();
    }
    BOOL foregroundOk = SetForegroundWindow(hForegroundTarget);
    if (!foregroundOk) SwitchToThisWindow(hForegroundTarget, TRUE);
    bool active = GetForegroundWindow() == hForegroundTarget;
    if (!retrying || active) {
        Wh_Log(L"SWS: activate %p -> SetForegroundWindow=%d active=%d", hForegroundTarget,
               foregroundOk, active);
    }
    if (active) {
        s_touchpadActivationRetryTarget = NULL;
        s_touchpadActivationRetryDeadline = 0;
        if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_TOUCHPAD_TARGET_FOCUS_RETRY_TIMER_ID);
        UpdateMruWindow(hTarget);
        s_touchpadCommitActivation = false;
    } else if (g_hSwitcher && ((!g_isVisible && !g_isPendingShow) || g_animExitActive)) {
        // Foreground activation may be denied transiently after the raw input
        // process releases the switcher, or while the visual exit is still
        // running. Retry without blocking this message pump; never attach to
        // or send synchronously to the target thread.
        if (!retrying) {
            // The absolute deadline is established once. Resetting it here on
            // each failed attempt made the 600 ms retry run for many seconds.
            s_touchpadActivationRetryDeadline = GetTickCount64() + 600;
            SetTimer(g_hSwitcher, SWS_TOUCHPAD_TARGET_FOCUS_RETRY_TIMER_ID, 40, NULL);
        }
        s_touchpadActivationRetryTarget = hForegroundTarget;
    }
}

static void StartExitAnimation(bool activateSelectedWindow) {
    if ((!g_isVisible && !g_isPendingShow) || g_animExitActive) return;
    g_touchpadTwoFingerCloseActive.store(false);
    UpdateTwoFingerTapOverride(false);
    if (!activateSelectedWindow) s_touchpadCommitActivation = false;
    // A lift during cancellation must not commit; continuing the cancelled
    // stroke must not reopen the switcher before all fingers have lifted.
    CancelRawTouchpadStroke();

    HWND hTarget = NULL;
    std::vector<HWND> restoreWindows;
    if (activateSelectedWindow && g_selectedIndex >= 0 &&
        g_selectedIndex < (int)g_windows.size()) {
        hTarget = g_windows[g_selectedIndex].hWnd;
        if (g_settings.showApplications && g_settings.restoreAllWindows) {
            restoreWindows = g_windows[g_selectedIndex].groupWindows;
        }
    }

    // Pending or accessibility-disabled sessions close immediately. Animated
    // sessions use the shared exit path below for both cancel and selection.
    if (g_isPendingShow || !AreAnimationsGloballyEnabled() ||
        !g_settings.enableEntranceAnimation) {
        // Guard focus-change reentrancy and duplicate release events before
        // handing off. Activation must precede even non-animated teardown.
        g_animExitActive = true;
        if (activateSelectedWindow) ActivateExitedWindow(hTarget, restoreWindows);
        HideSwitcher();
        return;
    }

    // Reverse the shared linear timeline at its current point, before resetting
    // entrance. E(p) and physical position are unchanged at this handoff.
    StartMotionTrack(g_animExitProgress, g_animEntranceProgress);
    g_animExitCurrentAlpha = g_easeEntrance.Solve(g_animExitProgress);
    // Capture the actually presented layered alpha, independently of transform
    // progress, so interruption keeps its frame-zero opacity continuous.
    StartOpacityMotion(g_presentationOpacity, g_presentationOpacityCurrent, 0.0f);
    g_animEntranceActive = false;
    g_animEntranceProgress = 1.0f;
    g_animEntranceCurrentAlpha = 1.0f;

    g_animExitActive = true;
    RefreshTouchpadGestureKinds();
    // Decreasing p at the entrance speed gives a remaining duration of 167ms*p.
    g_animExitDuration = g_animEntranceDuration;

    // Keep the touchpad controller and early marker ownership through the
    // visible exit animation. HideSwitcher releases them at the real close.
    // Native material presentation is motion-only; layered opacity is linear.
    // Make switcher and overlay click-through during dissolve
    if (g_hSwitcher) {
        LONG_PTR ex = GetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE);
        SetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE, ex | WS_EX_TRANSPARENT);
    }
    if (g_hCloseBtnWnd) {
        LONG_PTR ex = GetWindowLongPtrW(g_hCloseBtnWnd, GWL_EXSTYLE);
        SetWindowLongPtrW(g_hCloseBtnWnd, GWL_EXSTYLE, ex | WS_EX_TRANSPARENT);
    }

    StartAnimationTicker();
    // Both release paths hand off before visual teardown. The exit guard and
    // click-through state are established before external calls can reenter.
    if (activateSelectedWindow) ActivateExitedWindow(hTarget, restoreWindows);
}

static void SwitchToSelected() {
    // Idempotence guard: both the Alt-release keyup path and the ALT_POLL timer
    // can trigger a commit. If neither a visible nor a pending switcher remains,
    // the first commit already tore everything down — a second commit must no-op.
    if (!g_isVisible && !g_isPendingShow) return;
    StartExitAnimation(true);
}

static void AdoptRawThreeFingerState(bool allowCurrentLive) {
    if (!s_altSessionOwner) return;
    ULONGLONG state = g_touchpadRawThreeFingerState.load(std::memory_order_acquire);
    ULONGLONG serial = RawThreeFingerStateSerial(state);
    ULONG phase = RawThreeFingerStatePhase(state);
    if (!serial ||
        (serial <= s_altRawBaselineSerial &&
         !(allowCurrentLive && phase == SWS_RAW_THREE_PHASE_LIVE))) {
        return;
    }
    if (!s_combinedRawSeen || serial != s_combinedRawSerial) {
        s_combinedRawSerial = serial;
        s_combinedRawSeen = true;
        s_combinedRawLiftHandled = false;
    }
    if (g_hSwitcher && (g_isVisible || g_isPendingShow) &&
        (phase == SWS_RAW_THREE_PHASE_LIVE ||
         phase == SWS_RAW_THREE_PHASE_LIFTED)) {
        SetTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID,
                 SWS_RAW_SESSION_LOST_TIMEOUT_MS, NULL);
    }
}

static bool CombinedRawReleasePending() {
    AdoptRawThreeFingerState();
    return s_altSessionOwner && s_combinedRawSeen &&
           !s_combinedRawLiftHandled;
}

static void KeepAltReleasePolling() {
    if (g_hSwitcher && s_altSessionOwner &&
        (g_isVisible || g_isPendingShow)) {
        SetTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID, 50, NULL);
    }
}

static bool TryCommitAfterInputRelease() {
    if (!g_isVisible && !g_isPendingShow) return false;
    AdoptRawThreeFingerState();
    if (s_commitStarted) return true;
    if ((s_altSessionOwner && s_altHeld) || g_isTouchpadGestureActive ||
        CombinedRawReleasePending()) {
        KeepAltReleasePolling();
        return false;
    }
    s_commitStarted = true;
    s_touchpadCommitActivation = true;
    SwitchToSelected();
    return true;
}

static void HandleAltRelease() {
    s_altHeld = false;
    AdoptRawThreeFingerState();
    if (g_isSticky) return;
    TryCommitAfterInputRelease();
}

// Helper: recompute layout and reposition switcher window
static void RecomputeAndReposition() {
    // A restore/foreground notification can arrive during activation. Keep the
    // closing frame's membership and geometry; the next invocation enumerates
    // fresh state, while already-owned animation tracks can finish normally.
    if (g_animExitActive) return;
    // Purge any destroyed or non-iconic hidden windows that were closed or hidden silently
    if (!g_windows.empty()) {
        bool removedAny = false;
        for (int i = (int)g_windows.size() - 1; i >= 0; i--) {
            HWND h = g_windows[i].hWnd;
            if (!IsWindow(h) || (!IsWindowVisible(h) && !IsIconic(h))) {
                for (const auto& kv : g_windows[i].hThumbs) {
                    if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
                }
                g_windows[i].hThumbs.clear();
                g_windows.erase(g_windows.begin() + i);
                if (i < g_selectedIndex) {
                    g_selectedIndex--;
                }
                if (i < g_layoutStartIndex) {
                    g_layoutStartIndex--;
                }
                removedAny = true;
            }
        }
        if (removedAny) {
            if (g_windows.empty()) {
                HideSwitcher();
                return;
            }
            if (g_selectedIndex >= (int)g_windows.size()) {
                g_selectedIndex = (int)g_windows.size() - 1;
            }
            if (g_selectedIndex < 0) g_selectedIndex = 0;
            if (g_layoutStartIndex > (int)g_windows.size() - 1) {
                g_layoutStartIndex = std::max(0, (int)g_windows.size() - 1);
            }
            if (g_layoutStartIndex < 0) g_layoutStartIndex = 0;
        }
    }

    bool scrollReflow = g_scrollTransition.preservingThumbnails;
    CaptureLayoutTransitionStart();
    RegisterThumbnailsEarly();
    // Refresh DWM source sizes (GetWindowRect fallback) before layout so that
    // aspect ratios used in ComputeLayout are near-final on the first pass.
    for (auto& w : g_windows) {
        RefreshEntrySourceSize(w);
    }
    HMONITOR hMon = g_hCurrentMonitor
                    ? g_hCurrentMonitor
                    : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
    g_hCurrentMonitor = hMon;
    ComputeTransitionLayout(hMon);
    UpdateChevronLayout(g_hSwitcher);
    UpdateChevronAnimationTargets(false);
    InvalidateStaticCache();
    CommitLayoutTransition(hMon, scrollReflow);
    if (g_isPendingShow) return;
    g_pendingSwitcherRect = { 0, 0, 0, 0 };
    if (IsWin11OrGreater()) {
        COLORREF colorNone = 0xFFFFFFFE;
        DwmSetWindowAttribute(g_hSwitcher, 34 /* DWMWA_BORDER_COLOR */, &colorNone, sizeof(colorNone));
    }
    for (HWND hMirror : g_hMirrorSwitchers) {
        if (!IsWindow(hMirror)) continue;
        if (IsWin11OrGreater()) {
            COLORREF colorNone = 0xFFFFFFFE;
            DwmSetWindowAttribute(hMirror, 34 /* DWMWA_BORDER_COLOR */, &colorNone, sizeof(colorNone));
        }
    }
    ApplySwitcherRegion();
    if (g_hoverThumbIndex >= 0 && g_hoverThumbIndex < (int)g_windows.size() && !IsWindowTruncated(g_hoverThumbIndex)) {
        SnapHoverTo(ToRectF(g_windows[g_hoverThumbIndex].rcThumbActual));
    }
}

// Drill into the selected application's windows: stash the grouped app list and
// replace g_windows with one entry per window of that app.
static void EnterAppGroup() {
    if (!g_settings.showApplications || g_drilledIn) return;
    if (g_selectedIndex < 0 || g_selectedIndex >= (int)g_windows.size()) return;
    std::vector<HWND> members = g_windows[g_selectedIndex].groupWindows;
    if (members.size() <= 1) return;  // nothing to expand

    CaptureGroupTransitionStart();
    if (AreAnimationsGloballyEnabled() && g_settings.enableAnimations) {
        // ── Animated drill-down path ──────────────────────────────────

        // 3. Snapshot visible app-list items as departing entries
        for (auto& w : g_windows) {
            if (w.rcCell.left == 0 && w.rcCell.right == 0 &&
                w.rcCell.top == 0  && w.rcCell.bottom == 0) continue;
            DepartingEntrySnapshot snap = CaptureDepartingEntry(w);
            if (DockLayoutActive()) {
                // Dock: unregister live thumbnails; icon cross-fade via DrawSwitcherStaticContent
                for (const auto& kv : w.hThumbs) {
                    if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
                }
                w.hThumbs.clear();
                snap.hThumbs.clear();
            } else {
                // Grid/Badge: transfer thumbnail handles for smooth DWM cross-fade
                snap.hThumbs = w.hThumbs;
                w.hThumbs.clear();
            }
            g_layoutTransition.departingItems.push_back(std::move(snap));
        }

        // 4. Stash app list and populate drilled-in window entries
        g_savedAppList          = std::move(g_windows);
        g_savedSelectedIndex    = g_selectedIndex;
        g_savedLayoutStartIndex = g_layoutStartIndex;

        g_windows.clear();
        for (HWND hw : members) {
            if (!IsWindow(hw)) continue;
            WindowEntry e = {};
            e.hWnd = hw;
            GetWindowTextW(hw, e.title, 256);
            if (!e.title[0]) InternalGetWindowText(hw, e.title, 256);
            e.hIcon = LoadWindowIcon(hw, &e.iconCell);
            g_windows.push_back(std::move(e));
        }
        if (g_windows.empty()) {  // every window closed in the meantime; abort
            g_windows = std::move(g_savedAppList);
            // Restore thumbnails from departing snapshots
            for (auto& dep : g_layoutTransition.departingItems) {
                for (auto& w : g_windows) {
                    if (w.hWnd == dep.hWnd && !DockLayoutActive()) {
                        w.hThumbs = std::move(dep.hThumbs);
                        break;
                    }
                }
            }
            g_layoutTransition.departingItems.clear();
            RecomputeAndReposition();
            return;
        }
        g_drilledIn             = true;
        g_selectedIndex         = 0;
        g_layoutStartIndex      = 0;
        g_hoverIndex            = -1;
        g_hoverThumbIndex       = -1;
        g_hoverWnd              = NULL;
        g_isCloseHovered        = false;
        g_animHoverActive       = false;
        g_animHoverAlphaCurrent = 0.0f;
        g_animHoverAlphaTarget  = 0.0f;

        // 5. Compute new layout to get target cell rects.
        // Register the fresh per-window entries' DWM thumbnails BEFORE the layout pass so
        // RefreshEntrySourceSize reads DWM's real source size instead of a placeholder
        // fallback; otherwise the layout (and the rcThumbTarget captured below) is computed
        // from a wrong aspect and the transition leaves the entry shrunken. Same order as
        // RecomputeAndReposition.
        RegisterThumbnailsEarly();
        HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
        for (auto& w : g_windows) RefreshEntrySourceSize(w);
        ComputeTransitionLayout(hMon);
        for (auto& w : g_windows) {
            w.rcCellStart = {};
            w.entryMotion = {};
        }
        UpdateChevronLayout(g_hSwitcher);
        UpdateChevronAnimationTargets(false);
        CommitLayoutTransition(hMon);
        PaintSwitcher();
    } else {
        // ── Instant fallback path (animations disabled) ───────────────
        UnregisterThumbnails();
        g_savedAppList = std::move(g_windows);
        g_savedSelectedIndex = g_selectedIndex;
        g_savedLayoutStartIndex = g_layoutStartIndex;

        g_windows.clear();
        for (HWND hw : members) {
            if (!IsWindow(hw)) continue;
            WindowEntry e = {};
            e.hWnd = hw;
            GetWindowTextW(hw, e.title, 256);
            if (!e.title[0]) InternalGetWindowText(hw, e.title, 256);
            e.hIcon = LoadWindowIcon(hw, &e.iconCell);
            g_windows.push_back(std::move(e));
        }
        if (g_windows.empty()) {
            g_windows = std::move(g_savedAppList);
            RecomputeAndReposition();
            return;
        }
        g_drilledIn = true;
        g_selectedIndex = 0;
        g_layoutStartIndex = 0;
        g_hoverIndex = -1;
        g_hoverThumbIndex = -1;
        g_hoverWnd = NULL;
        g_isCloseHovered = false;
        g_animHoverActive = false;
        g_animHoverAlphaCurrent = 0.0f;
        g_animHoverAlphaTarget = 0.0f;
        RecomputeAndReposition();
        if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
            g_animSelectionCurrent = ToRectF(g_windows[g_selectedIndex].rcCell);
            g_animSelectionTarget = g_animSelectionCurrent;
            g_animSelectionActive = false;
        }
        g_scrollTransition.active = false;
        g_scrollTransition.offsetCurrentX = 0.0f;
        g_scrollTransition.offsetCurrentY = 0.0f;
        g_scrollTransition.outgoingItems.clear();
        g_scrollTransition.preservingThumbnails = false;
        PaintSwitcher();
        UpdateHoverFromCursor(false);
    }
}

// Leave the drilled-in window view and restore the grouped application list.
static void ExitAppGroup() {
    if (!g_drilledIn) return;

    CaptureGroupTransitionStart();
    if (AreAnimationsGloballyEnabled() && g_settings.enableAnimations) {
        // ── Animated return path ──────────────────────────────────────

        // 3. Snapshot visible drilled-in items as departing entries
        for (auto& w : g_windows) {
            if (w.rcCell.left == 0 && w.rcCell.right == 0 &&
                w.rcCell.top == 0  && w.rcCell.bottom == 0) continue;
            DepartingEntrySnapshot snap = CaptureDepartingEntry(w);
            if (DockLayoutActive()) {
                for (const auto& kv : w.hThumbs) {
                    if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
                }
                w.hThumbs.clear();
                snap.hThumbs.clear();
            } else {
                snap.hThumbs = w.hThumbs;
                w.hThumbs.clear();
            }
            g_layoutTransition.departingItems.push_back(std::move(snap));
        }

        // 4. Restore app list and selection state
        g_windows          = std::move(g_savedAppList);
        g_savedAppList.clear();
        g_selectedIndex    = g_savedSelectedIndex;
        g_layoutStartIndex = g_savedLayoutStartIndex;
        if (g_selectedIndex >= (int)g_windows.size()) g_selectedIndex = (int)g_windows.size() - 1;
        if (g_selectedIndex < 0) g_selectedIndex = 0;
        g_drilledIn             = false;
        g_hoverIndex            = -1;
        g_hoverThumbIndex       = -1;
        g_hoverWnd              = NULL;
        g_isCloseHovered        = false;
        g_animHoverActive       = false;
        g_animHoverAlphaCurrent = 0.0f;
        g_animHoverAlphaTarget  = 0.0f;

        // 5. Compute restored layout to get target rects.
        // The restored app-list entries lost their DWM thumbnails to the departing
        // snapshots at drill-in, so register fresh ones BEFORE the layout pass:
        // RefreshEntrySourceSize then reads DWM's real source size for every entry
        // (minimized ones included, via the restore rect while DWM has no surface yet)
        // instead of the iconic/GetWindowRect fallback, so the rects captured below as
        // rcThumbTarget are final. Same order as RecomputeAndReposition.
        RegisterThumbnailsEarly();
        HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
        // RefreshEntrySourceSize already covers the no-thumbnail and DWM-query-failed
        // cases (restore rect for minimized windows, live rect otherwise), so no extra
        // zero-size fallback is needed here.
        for (auto& w : g_windows) {
            RefreshEntrySourceSize(w);
        }
        ComputeTransitionLayout(hMon);
        for (auto& w : g_windows) {
            w.rcCellStart = {};
            w.entryMotion = {};
        }
        UpdateChevronLayout(g_hSwitcher);
        UpdateChevronAnimationTargets(false);
        CommitLayoutTransition(hMon);
        PaintSwitcher();
        UpdateHoverFromCursor(false);
    } else {
        // ── Instant fallback path (animations disabled) ───────────────
        UnregisterThumbnails();
        g_windows          = std::move(g_savedAppList);
        g_savedAppList.clear();
        g_selectedIndex    = g_savedSelectedIndex;
        g_layoutStartIndex = g_savedLayoutStartIndex;
        if (g_selectedIndex >= (int)g_windows.size()) g_selectedIndex = (int)g_windows.size() - 1;
        if (g_selectedIndex < 0) g_selectedIndex = 0;
        g_drilledIn = false;
        g_hoverIndex = -1;
        g_hoverThumbIndex = -1;
        g_hoverWnd = NULL;
        g_isCloseHovered = false;
        g_animHoverActive = false;
        g_animHoverAlphaCurrent = 0.0f;
        g_animHoverAlphaTarget = 0.0f;
        RecomputeAndReposition();
        if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
            g_animSelectionCurrent = ToRectF(g_windows[g_selectedIndex].rcCell);
            g_animSelectionTarget = g_animSelectionCurrent;
            g_animSelectionActive = false;
        }
        g_scrollTransition.active = false;
        g_scrollTransition.offsetCurrentX = 0.0f;
        g_scrollTransition.offsetCurrentY = 0.0f;
        g_scrollTransition.outgoingItems.clear();
        g_scrollTransition.preservingThumbnails = false;
        PaintSwitcher();
        UpdateHoverFromCursor(false);
    }
}

static void ToggleAppDrill() {
    if (g_drilledIn) ExitAppGroup();
    else EnterAppGroup();
}

static void ScrollDockWithDynamicResize(int targetStart, int targetSelected, int dir,
                                        ScrollNavType scrollType,
                                        const RectF& selectionFrom) {
    CaptureOutgoingSnapshot();

    CaptureLayoutTransitionStart();

    g_layoutStartIndex = targetStart;
    g_selectedIndex = targetSelected;

    if (!AreAnimationsGloballyEnabled() || !g_settings.enableAnimations) {
        RecomputeAndReposition();
        UpdateDockPreviewForSelection();
        RegisterThumbnails();
        TriggerScrollAnimationEx(dir, scrollType);
        if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
            RetargetSelectionFromPresented(selectionFrom);
        }
        UpdateChevronAnimationTargets(false);
        InvalidateStaticCache();
        PaintSwitcher();
        return;
    }

    HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
    ComputeTransitionLayout(hMon);
    CommitLayoutTransition(hMon, true);

    TriggerScrollAnimationEx(dir, scrollType);
    if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        RetargetSelectionFromPresented(selectionFrom);
    }
    UpdateChevronAnimationTargets(false);
    PaintSwitcher();
    StartAnimationTicker();
}

static void UpdateDockSelectionWithDynamicResize(int prevSelected) {
    if (!DockLayoutActive() || !DockShowPreview()) {
        UpdateDockPreviewForSelection();
        return;
    }
    int n = (int)g_windows.size();
    if (g_selectedIndex < 0 || g_selectedIndex >= n) return;

    if (!AreAnimationsGloballyEnabled() || !g_settings.enableAnimations) {
        RecomputeAndReposition();
        UpdateDockPreviewForSelection();
        RegisterThumbnails();
        PaintSwitcher();
        return;
    }

    (void)prevSelected;
    CaptureLayoutTransitionStart();

    HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
    ComputeTransitionLayout(hMon);
    CommitLayoutTransition(hMon);
}

// Linear navigation: Tab, Shift+Tab, Left, Right, Hotkeys, Scroll
static void CycleLinear(int delta) {
    if (g_windows.empty()) return;
    RectF selectionFrom = SelectionRectWithViewport();
    g_isPaginatedView = false;
    int n = (int)g_windows.size();
    int prevSelected = g_selectedIndex;
    g_selectedIndex = ((g_selectedIndex + delta) % n + n) % n;

    if (DockLayoutActive()) {
        int visibleCount = 0;
        for (int k = 0; k < n; k++) {
            if (!IsWindowTruncated(g_layoutStartIndex + k)) visibleCount++;
            else break;
        }
        if (visibleCount < 1) visibleCount = 1;

        int targetStart = g_layoutStartIndex;
        bool needsScroll = false;

        if (g_selectedIndex < g_layoutStartIndex || g_selectedIndex >= g_layoutStartIndex + visibleCount) {
            needsScroll = true;
            if (delta > 0) {
                if (g_selectedIndex == 0) {
                    targetStart = 0;
                } else {
                    targetStart = g_selectedIndex - visibleCount + 1;
                }
            } else {
                if (g_selectedIndex == n - 1) {
                    targetStart = n - visibleCount;
                } else {
                    targetStart = g_selectedIndex;
                }
            }
            if (targetStart < 0) targetStart = 0;
            if (targetStart > n - visibleCount) targetStart = std::max(0, n - visibleCount);
        }

        if (needsScroll && targetStart != g_layoutStartIndex) {
            int dir = (targetStart > g_layoutStartIndex) ? 1 : -1;
            ScrollDockWithDynamicResize(targetStart, g_selectedIndex, dir, SCROLL_ROW, selectionFrom);
        } else {
            UpdateDockSelectionWithDynamicResize(prevSelected);
            RetargetSelectionFromPresented(selectionFrom);
            UpdateChevronAnimationTargets(false);
            InvalidateStaticCache();
            PaintSwitcher();
        }
        return;
    }

    // If the newly selected window is truncated, recompute layout
    if (IsWindowTruncated(g_selectedIndex)) {
        CaptureOutgoingSnapshot();

        int oldStart = g_layoutStartIndex;
        HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);

        // Dry-run pass to find target line start without churning thumbnails
        g_isDryRunLayout = true;
        int targetStart = 0;
        if (delta > 0 && g_selectedIndex > oldStart) {
            g_layoutStartIndex = oldStart;
            targetStart = oldStart;
        } else {
            g_layoutStartIndex = 0;
            ComputeLayout(hMon);
        }
        int n2 = n;
        while (IsWindowTruncated(g_selectedIndex) && n2-- > 0) {
            int firstIdx = g_layoutStartIndex % n;
            int firstLineCoord = LayoutIsVertical() ? g_windows[firstIdx].rcCell.left : g_windows[firstIdx].rcCell.top;
            int newStart = g_layoutStartIndex;
            for (int k = 0; k < n; k++) {
                int wi = (g_layoutStartIndex + k) % n;
                if (IsWindowTruncated(wi)) break;
                int lineCoord = LayoutIsVertical() ? g_windows[wi].rcCell.left : g_windows[wi].rcCell.top;
                if (lineCoord != firstLineCoord) {
                    newStart = wi;
                    break;
                }
            }
            if (newStart == g_layoutStartIndex) {
                targetStart = g_selectedIndex;
                break;
            } else {
                g_layoutStartIndex = newStart;
                targetStart = newStart;
            }
            ComputeLayout(hMon);
        }
        g_isDryRunLayout = false;

        // Apply single real reflow
        g_layoutStartIndex = targetStart;
        RecomputeAndReposition();

        // Direction logic:
        // If layoutStartIndex decreased (e.g. wrapped from bottom row back to row 0): dir = -1 (scroll back to top).
        // If layoutStartIndex increased (e.g. scrolled forward/down): dir = 1 (scroll down to bottom).
        int dir;
        if (g_layoutStartIndex < oldStart) {
            dir = -1; // Scrolled back to top
        } else if (g_layoutStartIndex > oldStart) {
            dir = 1;  // Scrolled forward/down
        } else {
            dir = (delta >= 0) ? 1 : -1;
        }

        TriggerScrollAnimationEx(dir, SCROLL_ROW);

        if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
            RetargetSelectionFromPresented(selectionFrom);
        }
    } else {
        RetargetSelectionFromPresented(selectionFrom);
    }
    PaintSwitcher();
}

static void CyclePage(int dir) {
    if (g_windows.empty()) return;
    RectF selectionFrom = SelectionRectWithViewport();
    int n = (int)g_windows.size();

    if (DockLayoutActive()) {
        CaptureOutgoingSnapshot();
        int visibleCount = 0;
        for (int k = 0; k < n; k++) {
            if (!IsWindowTruncated(g_layoutStartIndex + k)) visibleCount++;
            else break;
        }
        if (visibleCount < 1) visibleCount = 1;
        int step = (visibleCount > 1) ? (visibleCount - 1) : 1;
        int newStart = g_layoutStartIndex + dir * step;
        if (newStart > n - visibleCount) newStart = n - visibleCount;
        if (newStart < 0) newStart = 0;
        if (newStart != g_layoutStartIndex) {
            int targetSelected = (dir > 0) ? newStart : std::min(n - 1, newStart + visibleCount - 1);
            ScrollDockWithDynamicResize(newStart, targetSelected, dir, SCROLL_PAGE, selectionFrom);
        } else {
            UpdateChevronAnimationTargets(false);
            InvalidateStaticCache();
            PaintSwitcher();
        }
        return;
    }

    // Capture outgoing snapshot BEFORE any dry-run layout modifies g_windows
    CaptureOutgoingSnapshot();

    // ── Step 1: Build a stable page map via a dry-run layout from index 0 ────
    //
    // We temporarily set g_layoutStartIndex = 0 and g_isPaginatedView = false
    // so ComputeLayout places ALL windows with no wrapping suppression.
    // We then read the resulting rcCell coordinates to find where rows/columns
    // are, and record the window index at which each new "page" starts.
    //
    // This is the only reliable approach because ComputeLayout's row/column
    // break conditions depend on many DPI-scaled constants that would be
    // error-prone to replicate here.

    HMONITOR hMon = g_hCurrentMonitor
                    ? g_hCurrentMonitor
                    : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);

    // Save current start index — the dry-run will overwrite g_layoutStartIndex.
    // Also save g_winW/g_winH: the dry-run ComputeLayout calls will overwrite them
    // with intermediate values, which would cause WM_PAINT to draw with wrong dims.
    int savedStart = g_layoutStartIndex;
    int savedWinW  = g_winW;
    int savedWinH  = g_winH;
    bool savedPaginated = g_isPaginatedView;

    // Dry-run: full layout pass from index 0, with wrapping allowed
    g_isDryRunLayout   = true;
    g_layoutStartIndex = 0;
    g_isPaginatedView  = false;
    ComputeLayout(hMon);  // populates rcCell for all windows

    // ── Step 2: Walk rcCell coords to identify page boundaries ───────────────
    //
    // In horizontal mode each page is a group of rows (by rcCell.top).
    // In vertical   mode each page is a group of columns (by rcCell.left).
    //
    // A page boundary occurs at the first row/column whose windows were
    // TRUNCATED by ComputeLayout (rcCell all-zeros), because that is
    // exactly where the layout engine ran out of screen space.
    //
    // pageStarts[p] = the window index (in layout order, which equals the
    // absolute window index when g_layoutStartIndex == 0) at which page p
    // starts.

    std::vector<int> pageStarts;
    pageStarts.push_back(0);

    bool vertical = LayoutIsVertical();
    int  prevLine = -1;   // previous row-top (horiz) or col-left (vert)

    for (int idx = 0; idx < n; idx++) {
        auto& w = g_windows[idx];  // g_layoutStartIndex==0, so idx == window index

        // Truncated window signals the end of what fits on the current page
        if (w.rcCell.left == 0 && w.rcCell.right  == 0 &&
            w.rcCell.top  == 0 && w.rcCell.bottom == 0) {
            // Start a new page here
            pageStarts.push_back(idx);
            prevLine = -1;  // reset for the next page's dry-run (see below)
            break;          // only one overflow region possible per layout pass
        }

        int lineCoord = vertical ? w.rcCell.left : w.rcCell.top;
        if (lineCoord != prevLine) {
            prevLine = lineCoord;
        }
    }

    // If more than one page exists, we need to recursively find further page
    // boundaries by repeating the dry-run from each new page start.
    // We loop until no more pages are detected.
    while (true) {
        int lastPageStart = pageStarts.back();
        if (lastPageStart >= n) break;

        // Dry-run from the last page start
        g_layoutStartIndex = lastPageStart;
        g_isPaginatedView  = true;   // prevent wrapping past end
        ComputeLayout(hMon);

        bool foundTruncation = false;
        prevLine = -1;
        for (int idx = 0; idx < n; idx++) {
            int wi = (lastPageStart + idx) % n;
            // Stop if we've wrapped past the array in paginated mode
            if (idx > 0 && wi < lastPageStart) break;

            auto& w = g_windows[wi];
            if (w.rcCell.left == 0 && w.rcCell.right  == 0 &&
                w.rcCell.top  == 0 && w.rcCell.bottom == 0) {
                // This window starts the next page
                int nextStart = wi;
                if (nextStart <= lastPageStart) break;  // sanity: no progress
                pageStarts.push_back(nextStart);
                foundTruncation = true;
                break;
            }
        }
        if (!foundTruncation) break;  // all remaining windows fit — done
    }

    g_isDryRunLayout = false;

    // ── Step 3: Determine which page we're currently on ──────────────────────
    int currentPage = 0;
    for (int p = (int)pageStarts.size() - 1; p >= 0; p--) {
        if (savedStart >= pageStarts[p]) {
            currentPage = p;
            break;
        }
    }

    // ── Step 4: Navigate to next or previous page with wrap-around ───────────
    int numPages   = (int)pageStarts.size();
    int targetPage = currentPage;

    if (numPages > 1) {
        targetPage = ((currentPage + dir) % numPages + numPages) % numPages;
    }

    // ── Boundary guard: if already on the only page, restore state and exit
    if (targetPage == currentPage) {
        g_winW             = savedWinW;
        g_winH             = savedWinH;
        g_layoutStartIndex = savedStart;
        g_isPaginatedView  = savedPaginated;

        // Run ComputeLayout to restore window coordinates
        g_isDryRunLayout = true;
        ComputeLayout(hMon);
        g_isDryRunLayout = false;

        g_scrollTransition.outgoingItems.clear();
        g_scrollTransition.preservingThumbnails = false;
        return;  // nothing changed — no reflow, no repaint, no flicker
    }

    // ── Step 5: Apply the new page and do the real reflow ────────────────────
    g_winW = savedWinW;
    g_winH = savedWinH;
    g_layoutStartIndex = pageStarts[targetPage];
    g_isPaginatedView  = true;
    RecomputeAndReposition();  // single real reflow — no flicker, no loops

    // ── Step 6: Place selection on the first visible window of the new page ──
    g_selectedIndex = g_layoutStartIndex % n;

    // Direction logic:
    // If targetPage < currentPage (e.g. Page Last -> Page 0 wrap): animDir = -1 (scrolls back to the top).
    // If targetPage > currentPage (e.g. Page 0 -> Page Last wrap): animDir = +1 (scrolls to the bottom).
    int animDir = (targetPage > currentPage) ? 1 : -1;
    TriggerScrollAnimationEx(animDir, SCROLL_PAGE);

    if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        RetargetSelectionFromPresented(selectionFrom);
    }

    PaintSwitcher();
}

// Directional navigation: Up, Down (EP-style row-based with nearest-column match)
// Walks in layout placement order (from g_layoutStartIndex). Keyboard/mouse
// callers retain wrapping; touchpad callers pass wrap=false for hard edges.
static void CycleDirectional(int vertDelta, bool wrap = true) {
    if (g_windows.empty()) return;
    RectF selectionFrom = SelectionRectWithViewport();
    if (DockLayoutActive()) {
        if (wrap) {
            CycleLinear(vertDelta);
        } else {
            int target = std::clamp(g_selectedIndex + vertDelta, 0,
                                    (int)g_windows.size() - 1);
            if (target != g_selectedIndex) CycleLinear(target - g_selectedIndex);
        }
        return;
    }
    int n = (int)g_windows.size();
    if (n <= 1) return;

    bool verticalLayout = LayoutIsVertical();
    HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);

    // Save current selection's perpendicular center for nearest-column matching
    RECT rcPrev = g_windows[g_selectedIndex].rcCell;
    int prevPerpCenter = verticalLayout ? (rcPrev.top + rcPrev.bottom) / 2 : (rcPrev.left + rcPrev.right) / 2;

    int oldStart = g_layoutStartIndex;
    int savedWinW = g_winW;
    int savedWinH = g_winH;
    bool savedPaginated = g_isPaginatedView;

    // ── Step 1: Discover all row starts via dry-run layout passes from 0 ──
    std::vector<int> rowStarts;
    rowStarts.push_back(0);

    g_isDryRunLayout = true;
    int curStart = 0;
    while (curStart < n) {
        g_layoutStartIndex = curStart;
        g_isPaginatedView = false;
        ComputeLayout(hMon);

        int firstIdx = curStart % n;
        int curLine = verticalLayout ? g_windows[firstIdx].rcCell.left : g_windows[firstIdx].rcCell.top;
        int nextStart = -1;

        for (int step = 0; step < n; step++) {
            int wi = (curStart + step) % n;
            if (step > 0 && wi < curStart) break; // wrapped past array end

            if (IsWindowTruncated(wi)) {
                nextStart = wi;
                break;
            }

            int lineCoord = verticalLayout ? g_windows[wi].rcCell.left : g_windows[wi].rcCell.top;
            if (lineCoord != curLine) {
                if (rowStarts.empty() || rowStarts.back() != wi) {
                    rowStarts.push_back(wi);
                }
                curLine = lineCoord;
            }
        }

        if (nextStart > curStart) {
            if (rowStarts.empty() || rowStarts.back() != nextStart) {
                rowStarts.push_back(nextStart);
            }
            curStart = nextStart;
        } else {
            break; // All windows covered
        }
    }

    int numRows = (int)rowStarts.size();
    if (numRows <= 1) {
        // Only one row/column visible; nothing to navigate vertically
        g_layoutStartIndex = oldStart;
        g_winW = savedWinW;
        g_winH = savedWinH;
        g_isPaginatedView = savedPaginated;
        ComputeLayout(hMon);
        g_isDryRunLayout = false;
        return;
    }

    // ── Step 2: Determine which row contains the current selection ──
    int currentRow = 0;
    for (int r = numRows - 1; r >= 0; r--) {
        if (g_selectedIndex >= rowStarts[r]) {
            currentRow = r;
            break;
        }
    }

    // ── Step 3: Determine target row ───────────────────────────────────────
    int targetRow = wrap
        ? ((currentRow + vertDelta) % numRows + numRows) % numRows
        : std::clamp(currentRow + vertDelta, 0, numRows - 1);
    if (targetRow == currentRow) {
        g_layoutStartIndex = oldStart;
        g_winW = savedWinW;
        g_winH = savedWinH;
        g_isPaginatedView = savedPaginated;
        ComputeLayout(hMon);
        g_isDryRunLayout = false;
        return;
    }

    // ── Step 4: Check if targetRow is already visible in current layout (oldStart) ──
    g_layoutStartIndex = oldStart;
    g_winW = savedWinW;
    g_winH = savedWinH;
    g_isPaginatedView = savedPaginated;
    ComputeLayout(hMon);

    bool targetRowVisible = !IsWindowTruncated(rowStarts[targetRow]);
    int targetStart = oldStart;
    int dir = 0;

    if (!targetRowVisible) {
        // Target row is off-screen; compute new g_layoutStartIndex (targetStart) and scroll direction
        if (vertDelta > 0) {
            // Moving Down
            if (targetRow == 0) {
                // Wrap from bottom back to top
                targetStart = rowStarts[0];
                dir = -1; // scroll back up to top
            } else {
                // Scroll down: find row start R_top <= targetRow that keeps targetRow visible
                int bestTop = targetRow;
                for (int r = targetRow; r >= 0; r--) {
                    g_layoutStartIndex = rowStarts[r];
                    ComputeLayout(hMon);
                    if (!IsWindowTruncated(rowStarts[targetRow])) {
                        bestTop = r;
                    } else {
                        break;
                    }
                }
                targetStart = rowStarts[bestTop];
                dir = 1; // scroll down
            }
        } else {
            // Moving Up
            if (targetRow == numRows - 1) {
                // Wrap from top row to the last row at the bottom
                int bestTop = targetRow;
                for (int r = targetRow; r >= 0; r--) {
                    g_layoutStartIndex = rowStarts[r];
                    ComputeLayout(hMon);
                    if (!IsWindowTruncated(rowStarts[targetRow])) {
                        bestTop = r;
                    } else {
                        break;
                    }
                }
                targetStart = rowStarts[bestTop];
                dir = 1; // scroll down to bottom
            } else {
                // Scroll up to reveal targetRow at top of visible view
                targetStart = rowStarts[targetRow];
                dir = -1; // scroll up
            }
        }
    }

    g_isDryRunLayout = false;

    // ── Step 5: Apply reflow if layout start changed ──
    // Step 4's visibility probes may leave geometry from a rejected layout.
    // Restore the real outgoing view before capturing it (even if its start
    // did not change), or navigation can paint/select against probe geometry.
    g_isDryRunLayout = true;
    g_layoutStartIndex = oldStart;
    g_isPaginatedView = savedPaginated;
    ComputeLayout(hMon);
    g_isDryRunLayout = false;
    if (targetStart != oldStart) {
        CaptureOutgoingSnapshot();
        g_winW = savedWinW;
        g_winH = savedWinH;
        g_layoutStartIndex = targetStart;
        g_isPaginatedView = savedPaginated;
        RecomputeAndReposition();
        TriggerScrollAnimationEx(dir, SCROLL_ROW);
    }

    // ── Step 6: Find best column match on targetRow ──
    int targetRowStart = rowStarts[targetRow];
    int targetRowEnd = (targetRow + 1 < numRows) ? rowStarts[targetRow + 1] : n;
    int targetLineCoord = verticalLayout ? g_windows[targetRowStart].rcCell.left : g_windows[targetRowStart].rcCell.top;
    int bestIndex = targetRowStart;
    int bestDist = INT_MAX;

    for (int wi = targetRowStart; wi < targetRowEnd; wi++) {
        if (IsWindowTruncated(wi)) continue;
        int lineCoord = verticalLayout ? g_windows[wi].rcCell.left : g_windows[wi].rcCell.top;
        if (lineCoord != targetLineCoord) continue;

        int perpCenter = verticalLayout ?
            (g_windows[wi].rcCell.top + g_windows[wi].rcCell.bottom) / 2 :
            (g_windows[wi].rcCell.left + g_windows[wi].rcCell.right) / 2;
        int dist = abs(prevPerpCenter - perpCenter);
        if (dist < bestDist) {
            bestDist = dist;
            bestIndex = wi;
        }
    }

    g_selectedIndex = bestIndex;
    RetargetSelectionFromPresented(selectionFrom);
    PaintSwitcher();
}

static int HitTest(int x, int y) {
    x -= (int)roundf(g_scrollTransition.offsetCurrentX);
    y -= (int)roundf(g_scrollTransition.offsetCurrentY);
    for (int i = 0; i < (int)g_windows.size(); i++) {
        RECT r = g_windows[i].rcCell;
        if (x >= r.left && x < r.right && y >= r.top && y < r.bottom) return i;
    }
    return -1;
}
static int HitTestThumb(int x, int y) {
    if (!g_settings.showThumbnails || DockLayoutActive()) return -1;
    x -= (int)roundf(g_scrollTransition.offsetCurrentX);
    y -= (int)roundf(g_scrollTransition.offsetCurrentY);
    for (int i = 0; i < (int)g_windows.size(); i++) {
        RECT r = g_windows[i].rcThumbActual;
        if (x >= r.left && x < r.right && y >= r.top && y < r.bottom) return i;
    }
    return -1;
}

static void GetOverflowState(bool& hasPrev, bool& hasNext) {
    hasPrev = false;
    hasNext = false;
    if (!g_settings.showOverflowIndicator || g_windows.empty()) return;
    int n = (int)g_windows.size();
    int visibleCount = 0;
    for (int i = 0; i < n; i++) {
        if (IsWindowTruncated((g_layoutStartIndex + i) % n)) break;
        visibleCount++;
    }
    bool anyTruncated = visibleCount < n;
    if (!anyTruncated) return;
    hasPrev = (g_layoutStartIndex > 0);
    hasNext = ((g_layoutStartIndex + visibleCount) < n);
}

static void UpdateChevronLayout(HWND hWnd) {
    if (!hWnd) return;
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    if (DockLayoutActive()) {
        int chevW = DpiScale(10, g_dpiX);
        int chevH = DpiScale(28, g_dpiY);
        int midY = g_rcDockIconStrip.top + (g_rcDockIconStrip.bottom - g_rcDockIconStrip.top) / 2;
        int edgeMarginX = DpiScale(6, g_dpiX);
        int leftX = g_rcDockIconStrip.left + edgeMarginX;
        int rightX = g_rcDockIconStrip.right - edgeMarginX - chevW;

        g_rcChevronPrev = { leftX, midY - chevH / 2, leftX + chevW, midY + chevH / 2 };
        g_rcChevronNext = { rightX, midY - chevH / 2, rightX + chevW, midY + chevH / 2 };
        return;
    }
    bool verticalLayout = LayoutIsVertical();
    int chevW, chevH;

    if (!verticalLayout) {
        // Horizontal layout: overflow is vertical (top / bottom)
        // Increased comfortable 7px edge margin from the corners/borders
        chevW = DpiScale(40, g_dpiX);
        chevH = DpiScale(10, g_dpiY);
        int midX = rcClient.left + (rcClient.right - rcClient.left) / 2;
        int edgeMarginY = DpiScale(7, g_dpiY);
        int topY = rcClient.top + edgeMarginY;
        int botY = rcClient.bottom - edgeMarginY - chevH;

        g_rcChevronPrev = { midX - chevW / 2, topY, midX + chevW / 2, topY + chevH };
        g_rcChevronNext = { midX - chevW / 2, botY, midX + chevW / 2, botY + chevH };
    } else {
        // Vertical layout: overflow is horizontal (left / right)
        // Increased comfortable 7px edge margin from the corners/borders
        chevW = DpiScale(10, g_dpiX);
        chevH = DpiScale(40, g_dpiY);
        int midY = rcClient.top + (rcClient.bottom - rcClient.top) / 2;
        int edgeMarginX = DpiScale(7, g_dpiX);
        int leftX = rcClient.left + edgeMarginX;
        int rightX = rcClient.right - edgeMarginX - chevW;

        g_rcChevronPrev = { leftX, midY - chevH / 2, leftX + chevW, midY + chevH / 2 };
        g_rcChevronNext = { rightX, midY - chevH / 2, rightX + chevW, midY + chevH / 2 };
    }
}

static void UpdateChevronAnimationTargets(bool immediate) {
    bool hasPrev = false, hasNext = false;
    GetOverflowState(hasPrev, hasNext);
    float newTargetPrev = hasPrev ? 1.0f : 0.0f;
    float newTargetNext = hasNext ? 1.0f : 0.0f;

    if (newTargetPrev != g_animChevronAlphaTargetPrev) {
        g_animChevronStartAlphaPrev = g_animChevronAlphaPrev;
        g_animChevronTransformStartPrev = g_animChevronTransformPrev;
        StartOpacityMotion(g_chevronRevealPrev, g_animChevronAlphaPrev, newTargetPrev);
        StartMotionTrack(g_animChevronProgressPrev);
        g_animChevronAlphaTargetPrev = newTargetPrev;
    }
    if (newTargetNext != g_animChevronAlphaTargetNext) {
        g_animChevronStartAlphaNext = g_animChevronAlphaNext;
        g_animChevronTransformStartNext = g_animChevronTransformNext;
        StartOpacityMotion(g_chevronRevealNext, g_animChevronAlphaNext, newTargetNext);
        StartMotionTrack(g_animChevronProgressNext);
        g_animChevronAlphaTargetNext = newTargetNext;
    }

    g_animChevronDuration = 0.167f;

    if (immediate || !AreAnimationsGloballyEnabled()) {
        g_animChevronAlphaPrev = g_animChevronAlphaTargetPrev;
        g_animChevronAlphaNext = g_animChevronAlphaTargetNext;
        g_animChevronTransformPrev = g_animChevronAlphaTargetPrev;
        g_animChevronTransformNext = g_animChevronAlphaTargetNext;
        g_chevronRevealPrev = {};
        g_chevronRevealNext = {};
        g_animChevronProgressPrev = 1.0f;
        g_animChevronProgressNext = 1.0f;
    } else if (g_animChevronProgressPrev < 1.0f || g_animChevronProgressNext < 1.0f ||
               fabsf(g_animChevronAlphaPrev - g_animChevronAlphaTargetPrev) > 0.005f ||
               fabsf(g_animChevronAlphaNext - g_animChevronAlphaTargetNext) > 0.005f) {
        StartAnimationTicker();
    }
}

static int HitTestChevron(HWND hWnd, int x, int y) {
    if (!g_settings.showOverflowIndicator || g_windows.empty() || !hWnd) return 0;
    UpdateChevronLayout(hWnd);

    POINT pt = { x, y };
    bool isHorizontalOverflow = LayoutIsVertical() || DockLayoutActive();
    int masterPadX = DpiScale(g_settings.switcherPadding, g_dpiX);
    int masterPadY = DpiScale(g_settings.switcherPadding, g_dpiY);

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    if (g_animChevronAlphaPrev > 0.05f) {
        RECT rcPrevHit = g_rcChevronPrev;
        if (!isHorizontalOverflow) {
            InflateRect(&rcPrevHit, DpiScale(10, g_dpiX), DpiScale(4, g_dpiY));
            // Clamp so it covers margin zone but never encroaches into content items
            if (rcPrevHit.top < rcClient.top) rcPrevHit.top = rcClient.top;
            if (rcPrevHit.bottom > rcClient.top + masterPadY) rcPrevHit.bottom = rcClient.top + masterPadY;
        } else if (DockLayoutActive()) {
            InflateRect(&rcPrevHit, DpiScale(6, g_dpiX), DpiScale(10, g_dpiY));
            if (rcPrevHit.left < g_rcDockIconStrip.left) rcPrevHit.left = g_rcDockIconStrip.left;
        } else {
            InflateRect(&rcPrevHit, DpiScale(4, g_dpiX), DpiScale(10, g_dpiY));
            if (rcPrevHit.left < rcClient.left) rcPrevHit.left = rcClient.left;
            if (rcPrevHit.right > rcClient.left + masterPadX) rcPrevHit.right = rcClient.left + masterPadX;
        }
        if (PtInRect(&rcPrevHit, pt)) return -1;
    }

    if (g_animChevronAlphaNext > 0.05f) {
        RECT rcNextHit = g_rcChevronNext;
        if (!isHorizontalOverflow) {
            InflateRect(&rcNextHit, DpiScale(10, g_dpiX), DpiScale(4, g_dpiY));
            if (rcNextHit.bottom > rcClient.bottom) rcNextHit.bottom = rcClient.bottom;
            if (rcNextHit.top < rcClient.bottom - masterPadY) rcNextHit.top = rcClient.bottom - masterPadY;
        } else if (DockLayoutActive()) {
            InflateRect(&rcNextHit, DpiScale(6, g_dpiX), DpiScale(10, g_dpiY));
            if (rcNextHit.right > g_rcDockIconStrip.right) rcNextHit.right = g_rcDockIconStrip.right;
        } else {
            InflateRect(&rcNextHit, DpiScale(4, g_dpiX), DpiScale(10, g_dpiY));
            if (rcNextHit.right > rcClient.right) rcNextHit.right = rcClient.right;
            if (rcNextHit.left < rcClient.right - masterPadX) rcNextHit.left = rcClient.right - masterPadX;
        }
        if (PtInRect(&rcNextHit, pt)) return 1;
    }

    return 0;
}

static void UpdateHoverAtPoint(HWND hWnd, int x, int y, bool allowAnimation) {
    if (!g_isVisible || !hWnd || g_windows.empty()) {
        if (g_hoverIndex != -1 || g_hoverThumbIndex != -1 || g_hoverWnd != NULL || g_animHoverAlphaCurrent > 0.0f || g_hoverChevron != 0) {
            g_hoverIndex = -1;
            g_hoverThumbIndex = -1;
            g_hoverWnd = NULL;
            g_isCloseHovered = false;
            g_hoverChevron = 0;
            g_animHoverActive = false;
            g_animHoverAlphaCurrent = 0.0f;
            g_animHoverAlphaTarget = 0.0f;
            g_animCloseBtnAlpha = 0.0f;
            g_animCloseBtnHoverAlpha = 0.0f;
            for (auto& w : g_windows) {
                SnapCloseButtonMotion(w, 0.0f);
            }
        }
        return;
    }

    int cDir = HitTestChevron(hWnd, x, y);
    bool chevronHoverChanged = (cDir != g_hoverChevron);
    g_hoverChevron = cDir;

    // Entry hover (for close button, selection, card hover)
    int entryIdx = (cDir != 0) ? -1 : HitTest(x, y);
    if (DockLayoutActive() && entryIdx < 0 && DockShowPreview() && g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        POINT pt = { x, y };
        if (PtInRect(&g_rcCentralPreview, pt)) {
            entryIdx = g_selectedIndex;
        }
    }

    // Thumbnail hover (strictly when cursor is over thumbnail itself, or entire card for zoom)
    int thumbIdx = (cDir != 0 || !g_settings.showThumbnails) ? -1 : HitTestThumb(x, y);
    if (thumbIdx < 0 && entryIdx >= 0 && g_settings.showThumbnails && !DockLayoutActive() && ThumbnailHoverIsZoom()) {
        thumbIdx = entryIdx;
    }

    bool closeHovered = false;
    if (g_settings.showCloseButton && entryIdx >= 0 && entryIdx < (int)g_windows.size() && !IsWindowTruncated(entryIdx)) {
        if (!DockLayoutActive() || !DockCloseButtonIsHidden()) {
            POINT pt = { x, y };
            closeHovered = HitTestCloseButton(g_windows[entryIdx], pt);
        }
    }

    bool entryHoverChanged = (entryIdx != g_hoverIndex || g_hoverWnd != hWnd);
    bool thumbHoverChanged = (thumbIdx != g_hoverThumbIndex || g_hoverWnd != hWnd);
    bool closeHoverChanged = (closeHovered != g_isCloseHovered);

    bool rectChanged = false;
    if (thumbIdx >= 0 && thumbIdx < (int)g_windows.size() && !IsWindowTruncated(thumbIdx)) {
        RectF actual = ToRectF(g_windows[thumbIdx].rcThumbActual);
        if (actual.left != g_animHoverTarget.left || actual.top != g_animHoverTarget.top ||
            actual.right != g_animHoverTarget.right || actual.bottom != g_animHoverTarget.bottom) {
            rectChanged = true;
        }
    }

    if (entryHoverChanged || thumbHoverChanged || closeHoverChanged || rectChanged || chevronHoverChanged) {
        InvalidateStaticCache();
        g_hoverIndex = entryIdx;
        g_hoverThumbIndex = thumbIdx;
        g_hoverWnd = hWnd;
        g_isCloseHovered = closeHovered;

        bool animsGloballyEnabled = AreAnimationsGloballyEnabled() && allowAnimation && g_settings.enableHoverAnimation;
        if (animsGloballyEnabled) {
            if (thumbHoverChanged || rectChanged) {
                TriggerHoverAnimation(thumbIdx);
            }
            UpdateMicrointeractionTargets();
            if (closeHoverChanged || entryHoverChanged || chevronHoverChanged) {
                StartAnimationTicker();
            }
        } else {
            g_animHoverActive = false;
            g_animChevronHoverAlphaPrev = g_hoverChevron < 0 ? 1.0f : 0.0f;
            g_animChevronHoverAlphaNext = g_hoverChevron > 0 ? 1.0f : 0.0f;
            if (thumbIdx >= 0 && thumbIdx < (int)g_windows.size() && !IsWindowTruncated(thumbIdx)) {
                g_animHoverCurrent = ToRectF(g_windows[thumbIdx].rcThumbActual);
                g_animHoverTarget = g_animHoverCurrent;
                g_animHoverAlphaCurrent = 1.0f;
                g_animHoverAlphaTarget = 1.0f;
            } else {
                g_animHoverAlphaCurrent = 0.0f;
                g_animHoverAlphaTarget = 0.0f;
            }
            g_animCloseBtnAlpha = (entryIdx >= 0 && g_settings.showCloseButton && !IsWindowTruncated(entryIdx)) ? 1.0f : 0.0f;
            g_animCloseBtnHoverAlpha = (closeHovered && g_animCloseBtnAlpha > 0.05f) ? 1.0f : 0.0f;
            for (int i = 0; i < (int)g_windows.size(); i++) {
                bool shouldShowClose = (i == entryIdx && g_settings.showCloseButton && !IsWindowTruncated(i));
                if (DockLayoutActive() && DockCloseButtonIsHidden()) shouldShowClose = false;
                SnapCloseButtonMotion(g_windows[i], shouldShowClose ? 1.0f : 0.0f);
                float s = (ThumbnailHoverIsZoom() && i == thumbIdx && !IsWindowTruncated(i)) ? (1.0f + SWS_HOVER_ZOOM_DELTA) : 1.0f;
                g_windows[i].hoverScale = s;
                g_windows[i].hoverScaleStart = s;
                g_windows[i].hoverScaleTarget = s;
                g_windows[i].hoverScaleProgress = 1.0f;
                g_windows[i].hoverScaleDuration = 0.167f;
                g_windows[i].hoverScaleClock = {};
            }
            if (ThumbnailHoverIsZoom()) {
                UpdateThumbnailAnimations();
            }
        }
        PaintSwitcher();
    }
}

static void UpdateHoverFromCursor(bool allowAnimation) {
    if (!g_isVisible || !g_hSwitcher || g_windows.empty()) return;

    POINT pt;
    if (!GetCursorPos(&pt)) return;

    HWND targetWnd = NULL;
    RECT rcWnd;
    if (GetWindowRect(g_hSwitcher, &rcWnd) && PtInRect(&rcWnd, pt)) {
        targetWnd = g_hSwitcher;
    } else {
        for (HWND hMirror : g_hMirrorSwitchers) {
            if (IsWindow(hMirror) && GetWindowRect(hMirror, &rcWnd) && PtInRect(&rcWnd, pt)) {
                targetWnd = hMirror;
                break;
            }
        }
    }

    if (!targetWnd) {
        if (g_hoverIndex != -1 || g_hoverThumbIndex != -1 || g_hoverWnd != NULL || g_animHoverAlphaCurrent > 0.0f || g_hoverChevron != 0) {
            InvalidateStaticCache();
            g_hoverIndex = -1;
            g_hoverThumbIndex = -1;
            g_hoverWnd = NULL;
            g_isCloseHovered = false;
            g_hoverChevron = 0;
            if (AreAnimationsGloballyEnabled() && allowAnimation && g_settings.enableHoverAnimation) {
                TriggerHoverAnimation(-1);
                StartAnimationTicker();
            } else {
                g_animHoverActive = false;
                g_animHoverAlphaCurrent = 0.0f;
                g_animHoverAlphaTarget = 0.0f;
                g_animCloseBtnAlpha = 0.0f;
                g_animCloseBtnHoverAlpha = 0.0f;
                g_animChevronHoverAlphaPrev = g_animChevronHoverAlphaNext = 0.0f;
                for (auto& w : g_windows) {
                    SnapCloseButtonMotion(w, 0.0f);
                    w.hoverScale = w.hoverScaleStart = w.hoverScaleTarget = 1.0f;
                    w.hoverScaleProgress = 1.0f;
                }
                UpdateThumbnailAnimations();
            }
            PaintSwitcher();
        }
        return;
    }

    ScreenToClient(targetWnd, &pt);
    UpdateHoverAtPoint(targetWnd, pt.x, pt.y, allowAnimation);
}


// WndProc

static void SWS_RegisterHotkeys();

static void UpdateEntryForWindow(WindowEntry& e) {
    InvalidateStaticCache();
    GetWindowTextW(e.hWnd, e.title, 256);
    if (!e.title[0]) InternalGetWindowText(e.hWnd, e.title, 256);
    e.hIcon = LoadWindowIcon(e.hWnd, &e.iconCell, true);

    if (g_settings.showApplications && wcscmp(g_settings.showTitles, L"windowTitle") != 0) {
        WCHAR appName[256] = {0};
        GetAppName(e.hWnd, appName, ARRAYSIZE(appName));
        if (appName[0]) {
            if (_wcsicmp(appName, L"Application Frame Host") == 0 && e.title[0]) {
                wcscpy_s(appName, e.title);
            }
            if (wcscmp(g_settings.showTitles, L"appName") == 0) {
                wcscpy_s(e.title, appName);
            } else {  // appNameWindowTitle
                if (e.title[0]) {
                    WCHAR combined[256];
                    _snwprintf_s(combined, ARRAYSIZE(combined), _TRUNCATE,
                                 L"%s - %s", appName, e.title);
                    wcscpy_s(e.title, combined);
                } else {
                    wcscpy_s(e.title, appName);
                }
            }
        }
    }
}

// Keep collapsed and saved groups in the same member-MRU order used by
// BuildWindowList. The app's card/index stays put when its representative changes.
static bool RefreshGroupRepresentative(WindowEntry& entry) {
    auto& members = entry.groupWindows;
    members.erase(std::remove_if(members.begin(), members.end(), [](HWND h) {
        return !IsWindow(h);
    }), members.end());
    if (members.empty()) return false;
    std::stable_sort(members.begin(), members.end(), [](HWND a, HWND b) {
        return GetMruRank(a) < GetMruRank(b);
    });
    if (entry.hWnd == members.front()) return false;
    for (const auto& kv : entry.hThumbs) {
        if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
    }
    entry.hThumbs.clear();
    entry.hWnd = members.front();
    entry.sourceSize = {};
    entry.effectiveSourceSize = {};
    entry.rcSourceCrop = {};
    UpdateEntryForWindow(entry);
    RefreshEntrySourceSize(entry);
    return true;
}

static void RemoveWindowEntryByHwnd(HWND hDestroyed) {
    if (!IsWindow(hDestroyed)) InvalidateWindowIcon(hDestroyed);
    if (g_animExitActive || (!g_isVisible && !g_isPendingShow) || g_windows.empty()) return;
    InvalidateStaticCache();
    // Filtering a minimized drill-in member is not destruction: keep that live
    // member in the saved app unless the entire app is now minimized/hidden.
    bool minimizedOnly = g_settings.hideMinimizedWindows && IsWindow(hDestroyed) &&
                         IsIconic(hDestroyed) && IsEligibleWindow(hDestroyed);

    if (g_drilledIn) {
        for (auto it = g_savedAppList.begin(); it != g_savedAppList.end();) {
            auto& grp = it->groupWindows;
            if (!minimizedOnly) grp.erase(std::remove(grp.begin(), grp.end(), hDestroyed), grp.end());
            bool hideGroup = g_settings.hideMinimizedWindows && IsEntryMinimized(*it);
            if (it->hWnd == hDestroyed || hideGroup) {
                for (const auto& kv : it->hThumbs) {
                    if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
                }
                it->hThumbs.clear();
                if (grp.empty() || hideGroup) {
                    int erased = (int)(it - g_savedAppList.begin());
                    if (erased < g_savedSelectedIndex) --g_savedSelectedIndex;
                    if (erased < g_savedLayoutStartIndex) --g_savedLayoutStartIndex;
                    it = g_savedAppList.erase(it);
                    int last = std::max(0, (int)g_savedAppList.size() - 1);
                    g_savedSelectedIndex = std::clamp(g_savedSelectedIndex, 0, last);
                    g_savedLayoutStartIndex = std::clamp(g_savedLayoutStartIndex, 0, last);
                    continue;
                }
                if (!RefreshGroupRepresentative(*it)) UpdateEntryForWindow(*it);
            }
            ++it;
        }
    }

    // Handle non-representative membership changes before the departing-entry
    // transition code. If only minimized survivors remain, retire the app card.
    if (g_settings.showApplications && !g_drilledIn) {
        for (auto& entry : g_windows) {
            auto& members = entry.groupWindows;
            auto member = std::find(members.begin(), members.end(), hDestroyed);
            if (entry.hWnd == hDestroyed || member == members.end()) continue;
            if (!minimizedOnly) members.erase(member);
            if (g_settings.hideMinimizedWindows && IsEntryMinimized(entry)) {
                HWND representative = entry.hWnd;
                RemoveWindowEntryByHwnd(representative);
                return;
            }
            if (RefreshGroupRepresentative(entry)) RecomputeAndReposition();
            else UpdateEntryForWindow(entry);
            PaintSwitcher();
            return;
        }
    }

    for (int i = 0; i < (int)g_windows.size(); i++) {
        if (g_windows[i].hWnd == hDestroyed) {
            if (g_settings.showApplications && g_windows[i].groupWindows.size() > 1) {
                auto& group = g_windows[i].groupWindows;
                if (!minimizedOnly) group.erase(std::remove(group.begin(), group.end(), hDestroyed), group.end());
                if (!group.empty() && !(g_settings.hideMinimizedWindows && IsEntryMinimized(g_windows[i]))) {
                    if (!RefreshGroupRepresentative(g_windows[i])) UpdateEntryForWindow(g_windows[i]);
                    for (auto& rem : g_windows) {
                        RefreshEntrySourceSize(rem);
                    }
                    RecomputeAndReposition();
                    PaintSwitcher();
                    return;
                }
            }

            if (g_isVisible && AreAnimationsGloballyEnabled() && g_settings.enableAnimations) {
                bool wasFocused = (i == g_selectedIndex);
                int closedIndex = i;
                int oldSize = (int)g_windows.size();

                CaptureLayoutTransitionStart();
                DepartingEntrySnapshot snap = CaptureDepartingEntry(g_windows[i]);

                if (DockLayoutActive()) {
                    for (const auto& kv : g_windows[i].hThumbs) {
                        if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
                    }
                    g_windows[i].hThumbs.clear();
                    snap.hThumbs.clear(); // do not preserve thumbnail for departing in dock layout
                    g_layoutTransition.departingItems.push_back(snap);
                } else {
                    snap.hThumbs = g_windows[i].hThumbs;
                    g_layoutTransition.departingItems.push_back(snap);
                    g_windows[i].hThumbs.clear(); // preserve thumbnails for animation in grid/badge
                }

                g_windows.erase(g_windows.begin() + i);
                if (g_windows.empty()) {
                    HideSwitcher();
                    return;
                }
                if (i < g_selectedIndex) {
                    g_selectedIndex--;
                }
                if (g_selectedIndex >= (int)g_windows.size()) {
                    g_selectedIndex = (int)g_windows.size() - 1;
                }
                if (g_selectedIndex < 0) g_selectedIndex = 0;

                if (i < g_layoutStartIndex) {
                    g_layoutStartIndex--;
                }
                if (g_layoutStartIndex < 0) g_layoutStartIndex = 0;

                if (DockLayoutActive() && wasFocused && g_settings.enableSelectionAnimation) {
                    int slideDir = (closedIndex < oldSize - 1) ? 1 : -1;
                    int travelDist = DpiScale(48, g_dpiX) * slideDir;
                    g_dockPreviewSlide.active = true;
                    StartMotionTrack(g_dockPreviewSlide.progress, 0.0f);
                    g_dockPreviewSlide.duration = 0.250f;
                    g_dockPreviewSlide.travelDistance = (float)travelDist;
                    g_dockPreviewSlide.currentOffset = (float)travelDist;
                    g_dockPreviewSlide.currentAlpha = 0.0f;
                } else {
                    g_dockPreviewSlide.active = false;
                    g_dockPreviewSlide.progress = 1.0f;
                    g_dockPreviewSlide.currentOffset = 0.0f;
                    g_dockPreviewSlide.currentAlpha = 1.0f;
                }

                for (auto& rem : g_windows) {
                    RefreshEntrySourceSize(rem);
                }

                HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
                RegisterThumbnailsEarly();
                ComputeTransitionLayout(hMon);
                CommitLayoutTransition(hMon);
                UpdateChevronLayout(g_hSwitcher);
                UpdateChevronAnimationTargets(false);

                g_hoverIndex = -1;
                g_hoverThumbIndex = -1;
                g_hoverWnd = NULL;
                g_isCloseHovered = false;
                g_animHoverActive = false;
                g_animHoverAlphaCurrent = 0.0f;
                g_animHoverAlphaTarget = 0.0f;
                return;
            } else {
                for (const auto& kv : g_windows[i].hThumbs) {
                    if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
                }
                g_windows[i].hThumbs.clear();

                g_windows.erase(g_windows.begin() + i);

                if (g_windows.empty()) {
                    HideSwitcher();
                    return;
                }

                if (i < g_selectedIndex) {
                    g_selectedIndex--;
                }
                if (g_selectedIndex >= (int)g_windows.size()) {
                    g_selectedIndex = (int)g_windows.size() - 1;
                }
                if (g_selectedIndex < 0) g_selectedIndex = 0;

                if (i < g_layoutStartIndex) {
                    g_layoutStartIndex--;
                }
                g_dockPreviewSlide.active = false;
                g_dockPreviewSlide.progress = 1.0f;
                g_dockPreviewSlide.currentOffset = 0.0f;
                g_dockPreviewSlide.currentAlpha = 1.0f;
                for (auto& rem : g_windows) {
                    RefreshEntrySourceSize(rem);
                }
                RecomputeAndReposition();
                g_hoverIndex = -1;
                g_hoverThumbIndex = -1;
                g_hoverWnd = NULL;
                g_isCloseHovered = false;
                g_animHoverActive = false;
                g_animHoverAlphaCurrent = 0.0f;
                g_animHoverAlphaTarget = 0.0f;
                UpdateHoverFromCursor(false);
                PaintSwitcher();
                return;
            }
        } else if (g_settings.showApplications) {
            auto& group = g_windows[i].groupWindows;
            auto it = std::find(group.begin(), group.end(), hDestroyed);
            if (it != group.end()) {
                group.erase(it);
                UpdateEntryForWindow(g_windows[i]);
                PaintSwitcher();
                return;
            }
        }
    }
}

static void AddWindowEntry(HWND hWnd) {
    if (g_animExitActive || (!g_isVisible && !g_isPendingShow) || g_windows.empty()) return;
    if (!hWnd || !IsWindow(hWnd) || IsSwitcherWindow(hWnd)) return;
    if (!IsEligibleWindow(hWnd)) {
        RemoveWindowEntryByHwnd(hWnd);
        return;
    }

    // A redraw/foreground event can refresh an already visible drill member.
    // Reconcile the saved app before the visible-entry early returns below.
    if (g_drilledIn) {
        for (auto& saved : g_savedAppList) {
            if (std::find(saved.groupWindows.begin(), saved.groupWindows.end(), hWnd) != saved.groupWindows.end()) {
                RefreshGroupRepresentative(saved);
                break;
            }
        }
    }

    // Existing tasks must obey live minimized filtering too, before the early
    // metadata/member returns. A mixed restored/minimized app remains listed.
    for (size_t i = 0; i < g_windows.size(); i++) {
        auto& entry = g_windows[i];
        bool member = g_settings.showApplications && !g_drilledIn &&
                      std::find(entry.groupWindows.begin(), entry.groupWindows.end(), hWnd) != entry.groupWindows.end();
        if ((entry.hWnd == hWnd || member) && g_settings.hideMinimizedWindows && IsEntryMinimized(entry)) {
            RemoveWindowEntryByHwnd(entry.hWnd);
            return;
        }
        if (member && RefreshGroupRepresentative(entry)) {
            RecomputeAndReposition();
            if (g_isVisible) PaintSwitcher();
            return;
        }
        if (g_windows[i].hWnd == hWnd) {
            WCHAR curTitle[256] = {0};
            GetWindowTextW(hWnd, curTitle, 256);
            if (!curTitle[0]) InternalGetWindowText(hWnd, curTitle, 256);
            if (curTitle[0] && wcscmp(g_windows[i].title, curTitle) != 0) {
                UpdateEntryForWindow(g_windows[i]);
                if (g_isVisible) PaintSwitcher();
            }
            return;
        }
        if (g_settings.showApplications) {
            auto& grp = g_windows[i].groupWindows;
            if (std::find(grp.begin(), grp.end(), hWnd) != grp.end()) {
                return;
            }
        }
    }

    if (g_drilledIn) {
        for (auto& w : g_savedAppList) {
            auto& grp = w.groupWindows;
            if (w.hWnd == hWnd || std::find(grp.begin(), grp.end(), hWnd) != grp.end()) {
                if (g_settings.hideMinimizedWindows && IsEntryMinimized(w)) {
                    RemoveWindowEntryByHwnd(w.hWnd);
                    return;
                }
                RefreshGroupRepresentative(w);
                WCHAR drilledKey[MAX_PATH] = {}, memberKey[MAX_PATH] = {};
                GetWindowGroupKey(g_windows.front().hWnd, drilledKey, ARRAYSIZE(drilledKey));
                GetWindowGroupKey(hWnd, memberKey, ARRAYSIZE(memberKey));
                // A restored member of the current drill must continue through
                // insertion; saved membership alone does not make it visible.
                if (!drilledKey[0] || wcscmp(drilledKey, memberKey) != 0) return;
                break;
            }
        }
    }

    WindowEntry e = {};
    if (!IsEligibleWindow(hWnd, &e)) return;

    // Collapsed apps retain minimized members while another member is restored.
    if (g_settings.hideMinimizedWindows && IsIconic(hWnd) &&
        (!g_settings.showApplications || g_drilledIn || g_isAltBacktickSameApp)) return;

    // ── Defensive cleanup: purge pending-close tracking and any departing ghost
    // for this window. With lazy removal (CloseSwitcherEntry no longer removes the
    // entry eagerly), a surviving window never leaves g_windows, so this is normally
    // a no-op. It only matters if an entry was removed while the window turned out
    // to be alive (e.g. an EVENT_OBJECT_DESTROY/HIDE raced a modal dialog).
    {
        auto pcIt = std::find(s_pendingCloseWindows.begin(), s_pendingCloseWindows.end(), hWnd);
        if (pcIt != s_pendingCloseWindows.end()) {
            s_pendingCloseWindows.erase(pcIt);
        }
        auto& deps = g_layoutTransition.departingItems;
        for (auto dit = deps.begin(); dit != deps.end(); ++dit) {
            if (dit->hWnd == hWnd) {
                // Release DWM thumbnail handles held by the snapshot before erasing.
                for (auto& kv : dit->hThumbs) {
                    if (kv.second) SafeDwmUnregisterThumbnail(kv.second);
                }
                deps.erase(dit);
                break;
            }
        }
    }

    // Pre-populate effectiveSourceSize via GetWindowRect fallback so that the
    // first ComputeLayout call (below) uses the correct aspect ratio instead of
    // the 1:1 square fallback triggered by zero sourceSize.
    RefreshEntrySourceSize(e);

    HWND currentSelectedWnd = (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size())
                              ? g_windows[g_selectedIndex].hWnd : NULL;

    if (g_drilledIn) {
        WCHAR drilledKey[MAX_PATH] = {0};
        if (!g_windows.empty()) {
            GetWindowGroupKey(g_windows[0].hWnd, drilledKey, ARRAYSIZE(drilledKey));
        }
        WCHAR newKey[MAX_PATH] = {0};
        GetWindowGroupKey(hWnd, newKey, ARRAYSIZE(newKey));

        if (drilledKey[0] && wcscmp(drilledKey, newKey) == 0) {
            UpdateEntryForWindow(e);
            g_windows.push_back(e);
            for (auto& saved : g_savedAppList) {
                WCHAR savedKey[MAX_PATH] = {0};
                GetWindowGroupKey(saved.hWnd, savedKey, ARRAYSIZE(savedKey));
                if (wcscmp(savedKey, newKey) == 0) {
                    if (std::find(saved.groupWindows.begin(), saved.groupWindows.end(), hWnd) == saved.groupWindows.end()) {
                        saved.groupWindows.push_back(hWnd);
                    }
                    RefreshGroupRepresentative(saved);
                    break;
                }
            }
        } else {
            bool foundInSaved = false;
            for (auto& saved : g_savedAppList) {
                WCHAR savedKey[MAX_PATH] = {0};
                GetWindowGroupKey(saved.hWnd, savedKey, ARRAYSIZE(savedKey));
                if (wcscmp(savedKey, newKey) == 0) {
                    saved.groupWindows.push_back(hWnd);
                    RefreshGroupRepresentative(saved);
                    foundInSaved = true;
                    break;
                }
            }
            if (!foundInSaved) {
                e.groupWindows.push_back(hWnd);
                UpdateEntryForWindow(e);
                g_savedAppList.push_back(e);
            }
            return;
        }
    } else if (g_settings.showApplications && !g_isAltBacktickSameApp) {
        WCHAR newKey[MAX_PATH] = {0};
        GetWindowGroupKey(hWnd, newKey, ARRAYSIZE(newKey));
        for (size_t i = 0; i < g_windows.size(); i++) {
            WCHAR existingKey[MAX_PATH] = {0};
            GetWindowGroupKey(g_windows[i].hWnd, existingKey, ARRAYSIZE(existingKey));
            if (newKey[0] && wcscmp(newKey, existingKey) == 0) {
                g_windows[i].groupWindows.push_back(hWnd);
                if (RefreshGroupRepresentative(g_windows[i])) RecomputeAndReposition();
                InvalidateStaticCache();
                if (g_isVisible) PaintSwitcher();
                return;
            }
        }
        e.groupWindows.push_back(hWnd);
        if (g_settings.hideMinimizedWindows && IsEntryMinimized(e)) return;
        UpdateEntryForWindow(e);

        // MRU-ranked insertion: same comparator as InitWindowList's stable_sort.
        // Minimized entries are segregated to the end (when sortMinimizedWindowsToEnd),
        // and within each tier windows are ordered by g_mruWindows rank (lower = more recent).
        // This ensures a re-added window (e.g. after modal dialog) lands at rank 0 (index 0),
        // not at the minimized boundary where the old IsIconic-only loop would place it.
        {
            bool eMin = g_settings.sortMinimizedWindowsToEnd && IsEntryMinimized(e);
            int  eRank = GetMruRank(e.hWnd);
            auto insertPos = g_windows.end();
            for (auto it = g_windows.begin(); it != g_windows.end(); ++it) {
                bool itMin = g_settings.sortMinimizedWindowsToEnd && IsEntryMinimized(*it);
                if (eMin != itMin) {
                    if (!eMin && itMin) { insertPos = it; break; }
                    continue;
                }
                if (eRank < GetMruRank(it->hWnd)) { insertPos = it; break; }
            }
            g_windows.insert(insertPos, std::move(e));
        }
    } else {
        UpdateEntryForWindow(e);

        {
            bool eMin = g_settings.sortMinimizedWindowsToEnd && IsEntryMinimized(e);
            int  eRank = GetMruRank(e.hWnd);
            auto insertPos = g_windows.end();
            for (auto it = g_windows.begin(); it != g_windows.end(); ++it) {
                bool itMin = g_settings.sortMinimizedWindowsToEnd && IsEntryMinimized(*it);
                if (eMin != itMin) {
                    if (!eMin && itMin) { insertPos = it; break; }
                    continue;
                }
                if (eRank < GetMruRank(it->hWnd)) { insertPos = it; break; }
            }
            g_windows.insert(insertPos, std::move(e));
        }
    }

    if (currentSelectedWnd) {
        for (int idx = 0; idx < (int)g_windows.size(); idx++) {
            if (g_windows[idx].hWnd == currentSelectedWnd) {
                g_selectedIndex = idx;
                break;
            }
        }
    }

    if (g_isVisible) {
        if (AreAnimationsGloballyEnabled() && g_settings.enableAnimations) {
            CaptureLayoutTransitionStart();
            HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
            RegisterThumbnailsEarly();
            for (auto& entry : g_windows) RefreshEntrySourceSize(entry);
            ComputeTransitionLayout(hMon);
            CommitLayoutTransition(hMon);
            UpdateChevronLayout(g_hSwitcher);
            UpdateChevronAnimationTargets(false);
            g_hoverIndex = -1;
            g_hoverThumbIndex = -1;
            g_hoverWnd = NULL;
            g_isCloseHovered = false;
            g_animHoverActive = false;
            g_animHoverAlphaCurrent = 0.0f;
            g_animHoverAlphaTarget = 0.0f;
        } else {
            RecomputeAndReposition();
            g_hoverIndex = -1;
            g_hoverThumbIndex = -1;
            g_hoverWnd = NULL;
            g_isCloseHovered = false;
            g_animHoverActive = false;
            g_animHoverAlphaCurrent = 0.0f;
            g_animHoverAlphaTarget = 0.0f;
            PaintSwitcher();
        }
    }
}

// Task View (the 3-finger up swipe) is an XAML island like the switcher; the OS shows it
// for its own gesture regardless of what the mod does, so it is hidden again while raw
// frames are flowing. Matched by class plus title (an island that has no title yet is
// reported as such in the log, so the match can be tightened from real evidence), and
// Win+Tab or the taskbar button do not come with raw frames and stay untouched.
static bool IsTaskViewWindow(HWND hWnd) {
    WCHAR cls[64] = {0};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    bool isIsland = (wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0);
    bool isMultiView = (wcscmp(cls, L"MultitaskingViewFrame") == 0);
    if (!isIsland && !isMultiView) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    WCHAR path[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(path);
    bool explorer = QueryFullProcessImageNameW(process, 0, path, &size) &&
                    _wcsicmp(PathFindFileNameW(path), L"explorer.exe") == 0;
    CloseHandle(process);
    if (!explorer) return false;

    WCHAR title[64] = {0};
    GetWindowTextW(hWnd, title, ARRAYSIZE(title));
    // Do not classify an arbitrary untitled XAML island as Task View.
    bool match = wcscmp(title, L"Task View") == 0 ||
                 (isMultiView && wcscmp(title, L"Task Switching") != 0);
    if (!match) return false;

    Wh_Log(L"SWS: multitasking island from a swipe: class=%s title='%s'", cls, title);
    return true;
}

static void CALLBACK WinEventShowHideProc(HWINEVENTHOOK hHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event != 0x0003 /* EVENT_SYSTEM_FOREGROUND */ &&
        (idObject != OBJID_WINDOW || idChild != CHILDID_SELF)) return;
    if (!hwnd) return;
    if (event == EVENT_OBJECT_DESTROY) {
        InvalidateWindowIcon(hwnd);
        RemoveWindowEntryByHwnd(hwnd);
        return;
    }
    if (!IsWindow(hwnd)) return;

    if ((event == EVENT_OBJECT_SHOW || event == 0x0003 /* EVENT_SYSTEM_FOREGROUND */) &&
        RawTouchpadShellSuppressionActive() && IsTaskViewWindow(hwnd)) {
        ShellFocusRecoveryScope recovery;
        Wh_Log(L"SWS: hid Task View shown by a 3-finger swipe");
        // Hiding the window alone leaves the shell believing Task View is still open, which
        // swallows the next 3-finger down (show desktop needs a second swipe). Escape is
        // Task View's own dismiss key, so close it through the shell while it owns the
        // foreground; the hide below then only cuts the closing animation short.
        HWND hFg = GetForegroundWindow();
        if (hFg && GetAncestor(hFg, GA_ROOT) == GetAncestor(hwnd, GA_ROOT)) {
            INPUT inputs[2] = {};
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_ESCAPE;
            inputs[1] = inputs[0];
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
            if (SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT))) {
                Wh_Log(L"SWS: dismissed Task View through the shell");
            }
        } else {
            Wh_Log(L"SWS: Task View was not foreground, hid it without dismissing");
        }
        ShowWindow(hwnd, SW_HIDE);
        if (g_hSwitcher && g_isVisible) {
            // Hiding the island can hand the foreground to the shell; keep the switcher
            // visible and topmost even when UIPI prevents reactivation.
            ReassertVisibleSwitcherPresentation();
        } else if (g_hSwitcher && s_rawTouchpadShieldActive) {
            ShowWindow(g_hSwitcher, 1);
            BringWindowToTop(g_hSwitcher);
            SetForegroundWindow(g_hSwitcher);
        }
        return;
    }
    // A hidden sticky shield can still race the native shell switcher. Handle
    // this before the visible-session filter, but never restore a hidden shield
    // as a visible window.
    if (event == EVENT_OBJECT_SHOW && RawTouchpadShellSuppressionActive() &&
        IsNativeSwitcherWindow(hwnd)) {
        ShellFocusRecoveryScope recovery;
        ShowWindow(hwnd, SW_HIDE);
        if (g_isVisible && g_hSwitcher) {
            ReassertVisibleSwitcherPresentation();
        } else if (s_rawTouchpadShieldActive && g_hSwitcher) {
            ShowWindow(g_hSwitcher, 1);
            BringWindowToTop(g_hSwitcher);
            SetForegroundWindow(g_hSwitcher);
        }
        return;
    }

    // Shell activation notifications are not guaranteed for every foreground
    // handoff (especially across integrity boundaries). Reconcile the
    // foreground event directly so a clicked target is in MRU order before the
    // next invocation rebuilds the list. MRU history remains valid while the
    // visual exit is running; live-list mutation still waits for an active
    // session and never changes the closing frame.
    if (event == EVENT_SYSTEM_FOREGROUND) {
        if (!IsSwitcherWindow(hwnd)) {
            UpdateMruWindow(hwnd);
            if (!g_animExitActive && (g_isVisible || g_isPendingShow)) {
                AddWindowEntry(hwnd);
            }
        }
        return;
    }

    if (g_animExitActive || (!g_isVisible && !g_isPendingShow)) return;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (style & WS_CHILD) return;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return;

    if (event == EVENT_OBJECT_HIDE && IsIconic(hwnd)) {
        if (!g_settings.hideMinimizedWindows) return;
        if (g_settings.showApplications && !g_drilledIn && !g_isAltBacktickSameApp) {
            // A minimized member still belongs to its live app. Filter only
            // when every member is minimized; do not report member destruction.
            for (const auto& entry : g_windows) {
                if (entry.hWnd == hwnd ||
                    std::find(entry.groupWindows.begin(), entry.groupWindows.end(), hwnd) != entry.groupWindows.end()) {
                    if (IsEntryMinimized(entry)) RemoveWindowEntryByHwnd(entry.hWnd);
                    return;
                }
            }
            return;
        }

        // The removal helper can regard a hidden HWND as ineligible. Preserve
        // the saved live membership of a mixed app while it filters the drill
        // and performs its existing selection/reflow handling.
        std::vector<HWND> savedMembers;
        HWND survivingMember = NULL;
        if (g_drilledIn) {
            for (const auto& saved : g_savedAppList) {
                if (std::find(saved.groupWindows.begin(), saved.groupWindows.end(), hwnd) == saved.groupWindows.end()) continue;
                for (HWND member : saved.groupWindows) {
                    if (IsWindow(member) && !IsIconic(member)) {
                        survivingMember = member;
                        savedMembers = saved.groupWindows;
                        break;
                    }
                }
                break;
            }
        }
        RemoveWindowEntryByHwnd(hwnd);
        if (g_drilledIn && survivingMember) {
            for (auto& saved : g_savedAppList) {
                if (std::find(saved.groupWindows.begin(), saved.groupWindows.end(), survivingMember) != saved.groupWindows.end()) {
                    saved.groupWindows = std::move(savedMembers);
                    RefreshGroupRepresentative(saved);
                    break;
                }
            }
        }
        return;
    }

    if (event == EVENT_OBJECT_SHOW) {
        // Shell windows are suppression-only; they never navigate or commit.
        AddWindowEntry(hwnd);
    } else if (event == EVENT_OBJECT_HIDE || event == EVENT_OBJECT_DESTROY) {
        if (!IsWindowVisible(hwnd) || event == EVENT_OBJECT_DESTROY) {
            // EVENT_OBJECT_DESTROY always satisfies this guard (a destroyed window is
            // not visible). For a HIDE event, only remove non-minimized windows:
            // minimizing fires HIDE while IsIconic is already true, and minimized
            // windows must keep their switcher entry.
            if (event == EVENT_OBJECT_DESTROY || !IsIconic(hwnd)) {
                RemoveWindowEntryByHwnd(hwnd);
            }
        }
    }
}

// Close the window for the entry at idx (posts SC_CLOSE, same as the close
// button). The entry is kept in the list while the close is pending: actual
// removal only happens once the window is confirmed gone (HSHELL_WINDOWDESTROYED,
// WinEvent HIDE/DESTROY, or the close-verify timer). This lazy removal keeps the
// entry at its exact position and preserves its MRU rank, so a window that
// survives the close attempt (e.g. it showed a modal "save changes?" dialog)
// never moves to the minimized boundary and never overlaps other entries.
// Shared by Q / Ctrl+W / Del and the close button. Posts SC_CLOSE and arms the
// close-verify timer exactly once per window, so rapid double-clicks or repeated
// key presses can't queue duplicate SC_CLOSE messages (which could re-trigger or
// dismiss an app's confirmation dialog).
static void QueueCloseWindow(HWND hw) {
    // Other lifecycle paths can retire the HWND vector while the session closes.
    // Prune those timestamps before recording a new request.
    for (auto it = s_pendingCloseDeadlines.begin(); it != s_pendingCloseDeadlines.end();) {
        if (std::find(s_pendingCloseWindows.begin(), s_pendingCloseWindows.end(), it->first) == s_pendingCloseWindows.end()) {
            it = s_pendingCloseDeadlines.erase(it);
        } else {
            ++it;
        }
    }
    ULONGLONG now = GetTickCount64();
    auto pending = std::find(s_pendingCloseWindows.begin(), s_pendingCloseWindows.end(), hw);
    if (pending != s_pendingCloseWindows.end()) {
        auto deadline = s_pendingCloseDeadlines.find(hw);
        if (deadline != s_pendingCloseDeadlines.end() && now < deadline->second) return;
        s_pendingCloseWindows.erase(pending);
        s_pendingCloseDeadlines.erase(hw);
    }
    // A failed post is not an in-flight close and must remain retryable.
    BOOL posted = PostMessage(hw, WM_SYSCOMMAND, SC_CLOSE, 0);
    DWORD error = posted ? ERROR_SUCCESS : GetLastError();
    Wh_Log(L"SWS TAPTRACE close post tick=%llu window=%p posted=%d error=%u",
           (unsigned long long)now, hw, posted, error);
    if (posted) {
        s_pendingCloseWindows.push_back(hw);
        s_pendingCloseDeadlines[hw] = now + SWS_CLOSE_COOLDOWN_MS;
    }
}

static void CloseSwitcherEntry(int idx) {
    if (idx < 0 || idx >= (int)g_windows.size()) return;

    HWND targetWnd = g_windows[idx].hWnd;
    if (g_settings.showApplications && g_windows[idx].groupWindows.size() > 1) {
        if (wcscmp(g_settings.groupCloseBehavior, L"closeAll") == 0) {
            std::vector<HWND> toClose = g_windows[idx].groupWindows;
            for (HWND hw : toClose) {
                if (CanCloseWindow(hw)) {
                    QueueCloseWindow(hw);
                }
            }
        } else {
            // closeRecent (Default)
            if (CanCloseWindow(targetWnd)) QueueCloseWindow(targetWnd);
        }
    } else {
        if (CanCloseWindow(targetWnd)) QueueCloseWindow(targetWnd);
    }

    if (!s_pendingCloseWindows.empty() && g_hSwitcher) {
        s_pendingCloseRetries = 0;
        SetTimer(g_hSwitcher, SWS_CLOSE_VERIFY_TIMER_ID, 200, NULL);
    }
}

// The raw path has no hotkey or Explorer foreground grant. The legacy
// unassigned-key request is best-effort: UIPI can reject injection and a
// successful SendInput does not establish foreground eligibility. Log both the
// request and the actual activation result rather than assuming it succeeded.
#define SWS_RAW_FOREGROUND_TAP_VK 0xE8
static void TapUnassignedKeyForForeground() {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = SWS_RAW_FOREGROUND_TAP_VK;
    inputs[1] = inputs[0];
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SetLastError(ERROR_SUCCESS);
    UINT sent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    DWORD error = sent == ARRAYSIZE(inputs) ? ERROR_SUCCESS : GetLastError();
    Wh_Log(L"SWS: foreground eligibility tap sent=%u/2 error=%u foreground=%p",
           sent, error, GetForegroundWindow());
}

// SetForegroundWindow is the supported cross-process foreground handoff. Do
// not use AttachThreadInput here: it can fail across integrity boundaries and
// couples this message pump synchronously to the foreground application's
// queue, which is especially unsafe for an elevated or hung application.
static bool ActivateRawTouchpadWindow() {
    if (!g_hSwitcher || !IsWindow(g_hSwitcher)) return false;
    if (GetForegroundWindow() == g_hSwitcher) return true;

    HWND hForeground = GetForegroundWindow();
    Wh_Log(L"SWS: touchpad handoff context %s",
           FormatTouchpadForegroundDiagnostic(hForeground, g_hSwitcher).c_str());
    SetLastError(ERROR_SUCCESS);
    BOOL setForegroundResult = SetForegroundWindow(g_hSwitcher);
    DWORD foregroundError = setForegroundResult ? ERROR_SUCCESS : GetLastError();
    if (GetForegroundWindow() != g_hSwitcher) {
        // This legacy fallback is retained for the shell's asynchronous
        // activation path; it is not used to manipulate the source window.
        SwitchToThisWindow(g_hSwitcher, TRUE);
    }

    bool active = GetForegroundWindow() == g_hSwitcher;
    Wh_Log(L"SWS: touchpad foreground handoff set=%d active=%d source=%p error=%u",
           setForegroundResult, active, hForeground,
           foregroundError);
    return active;
}

static void PromoteRawTouchpadShieldToSession() {
    if (!s_rawTouchpadShieldActive) return;
    s_rawTouchpadShieldActive = false;
    s_rawTouchpadShieldReleasePending = false;
    s_rawTouchpadShieldFocusStartTick = 0;
    s_rawShieldRestoreForeground = nullptr;
    if (g_hSwitcher) {
        KillTimer(g_hSwitcher, SWS_TOUCHPAD_SHIELD_RELEASE_TIMER_ID);
        KillTimer(g_hSwitcher, SWS_TOUCHPAD_SHIELD_FOCUS_RETRY_TIMER_ID);
    }
    if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID);
    // The visible/pending switcher keeps the controller enabled.
    RefreshTouchpadGestureKinds();
}

static void FinishRawTouchpadShield(bool keepMarkerGrace) {
    HWND hRestore = s_rawShieldRestoreForeground;
    bool restoreForeground = hRestore && GetForegroundWindow() == g_hSwitcher;
    if (g_hSwitcher) {
        KillTimer(g_hSwitcher, SWS_TOUCHPAD_SHIELD_RELEASE_TIMER_ID);
        KillTimer(g_hSwitcher, SWS_TOUCHPAD_SHIELD_FOCUS_RETRY_TIMER_ID);
        KillTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID);
    }
    s_rawTouchpadShieldActive = false;
    s_rawTouchpadShieldReleasePending = false;
    s_rawTouchpadShieldFocusStartTick = 0;
    s_rawShieldRestoreForeground = nullptr;

    if (g_hSwitcher && !g_isVisible && !g_isPendingShow && IsWindowVisible(g_hSwitcher)) {
        ShowWindow(g_hSwitcher, SW_HIDE);
    }
    s_rawSwipeMarkerActive = false;
    s_rawStrokeOwned = false;
    if (keepMarkerGrace) {
        // Do not republish here: release latency must not extend lift grace.
        UpdateRawSwipeMarkerTimer();
    } else {
        ClearRawSwipeMarker();
    }
    RefreshTouchpadGestureKinds();
    if (restoreForeground && IsWindow(hRestore)) {
        SetForegroundWindow(hRestore);
    }
}

static void ScheduleRawTouchpadShieldRelease() {
    if ((!s_rawTouchpadShieldActive && !s_rawSwipeMarkerActive) || !g_hSwitcher) return;
    s_rawTouchpadShieldReleasePending = true;
    SetTimer(g_hSwitcher, SWS_TOUCHPAD_SHIELD_RELEASE_TIMER_ID, 220, NULL);
}

// Raw HID controls fallback invocation and navigation. The typed native PTP
// boundaries can also invoke when Windows blocks background HID delivery.
// Shell show/hide notifications are never interpreted as finger movement/lift.
static void BeginTouchpadGesture(int step) {
    // Re-entrant: a new gesture must always be able to (re)open the switcher even if a
    // previous gesture left stale state. Force a clean slate if we are mid-exit.
    if (g_animExitActive) {
        Wh_Log(L"SWS: Touchpad gesture during exit animation -> forcing clean state");
        HideSwitcher();
    }
    g_isTouchpadGestureActive = true;
    s_lastTouchpadScrollTick = GetTickCount64();
    if (!g_isVisible && !g_isPendingShow) {
        ShowSwitcher(false, true);
    } else if (g_isPendingShow) {
        RevealPendingSwitcher();
    }
    if (g_isVisible && !g_windows.empty() && step != 0) {
        CycleLinear(step);
    }
    if (g_hSwitcher) {
        BringWindowToTop(g_hSwitcher);
        TapUnassignedKeyForForeground();
        if (ActivateRawTouchpadWindow()) {
            TapUnassignedKeyForForeground();
        }
        SetTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID, SWS_RAW_SESSION_LOST_TIMEOUT_MS, NULL);
    }
}

static void EndTouchpadGesture() {
    g_isTouchpadGestureActive = false;
    if (g_hSwitcher) {
        KillTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID);
    }
    // A touchpad release is only one half of a combined Alt+Tab session. Do
    // not commit while Alt is still physically held, and do not commit a
    // native handoff until its reader-observed lift reaches the UI thread.
    if (s_altSessionOwner &&
        (s_altHeld || CombinedRawReleasePending())) {
        KeepAltReleasePolling();
        return;
    }
    if (s_commitStarted) return;
    s_commitStarted = true;
    s_touchpadCommitActivation = true;
    if (g_isVisible || g_isPendingShow) {
        SwitchToSelected();
    }
}

// Raw-HID frames drive the switcher: the selection follows the finger position.
// Normal sessions commit on lift; sticky sessions commit only on a fresh tap.
// Sticky launch reserves upward motion outside SWS; downward strokes and taps
// remain Windows-owned. Horizontal motion opens the normal switcher regardless
// of the sticky-launch setting. Only exactly three
// fingers navigate.
// Lift frames can contain bogus edge coordinates, so they never navigate.
#define SWS_RAW_SWIPE_FINGERS 3
#define SWS_RAW_SWIPE_ENTRY_PITCH (65535 / 12)
// The dominant axis has to lead by this much before it navigates, so the sideways drift of
// a vertical swipe (and vice versa) cannot move the selection in the other direction.
#define SWS_RAW_SWIPE_DOMINANCE_NUM 3
#define SWS_RAW_SWIPE_DOMINANCE_DEN 2
// A frame that moves further than this between two reports is a clamped or mirrored
// position the pad emitted at an edge, not a finger: it restarts the gesture window
// instead of navigating, otherwise the selection would fly through the grid.
#define SWS_RAW_SWIPE_JUMP_TRAVEL (65535 / 4)
// While a raw session or sticky shield is open the switcher publishes it on its own window
// (a window property, so the Explorer side can read it without a cross-process call), and
// Explorer drops the shell's 'show desktop' swipe action for that gesture (see RaiseDesktop_Hook).
// The low 31 bits encode the last report's tick; bit 31 distinguishes lift grace.
// Active-report gaps and post-lift shell actions have separate bounded lifetimes.
// Pacing for the vertical axis: one row (or page) per this interval, which keeps a long drag
// from walking the layout through itself while still covering several rows when needed.
#define SWS_RAW_SWIPE_ROW_INTERVAL_MS 140
// A tap must be stationary, not just a drag which failed to change selection.
#define SWS_RAW_DIRECTION_TRAVEL (SWS_RAW_SWIPE_ENTRY_PITCH / 3)

static void PublishRawSwipe(bool liftGrace = false) {
    bool cancelledStroke = s_rawStrokeOwned && (s_rawIgnoreUntilLift || liftGrace);
    // A raw contact outside a visible/pending session is only observed for
    // classification. It must never publish the shell-suppression marker merely
    // because sticky mode is enabled; otherwise Explorer drops Show Desktop and
    // the configured three-finger tap action while the switcher is closed.
    if (!g_hSwitcher || !TouchpadHandlingEnabled() ||
        (!RawTouchpadOwnsInput() && !cancelledStroke)) {
        return;
    }
    // Only the reader timestamps physical contact/lift. A delayed UI message
    // must not renew grace, replace a reader-observed lift with live ownership,
    // or re-own the next outside tap. The UI can promote a live launch candidate
    // when that same contact actually opens a switcher, copying its timestamp.
    AcquireSRWLockExclusive(&s_rawSwipeMarkerLock);
    HANDLE marker = GetPropW(g_hSwitcher, SWS_RAW_SWIPE_PROP);
    HANDLE session = GetPropW(g_hSwitcher, SWS_RAW_SWIPE_SESSION_PROP);
    ULONGLONG now = GetTickCount64();
    bool liveMarker = RawSwipeMarkerRemainingMs(marker, now) != 0;
    if (!liftGrace && !s_rawIgnoreUntilLift && RawTouchpadOwnsInput() &&
        (g_isVisible || g_isPendingShow) && liveMarker && !session &&
        !((DWORD)(ULONG_PTR)marker & SWS_RAW_SWIPE_LIFT_FLAG) &&
        g_touchpadGesturesEnabled.load() && g_touchpadReaderAvailable.load()) {
        SetPropW(g_hSwitcher, SWS_RAW_SWIPE_SESSION_PROP, marker);
    }
    ReleaseSRWLockExclusive(&s_rawSwipeMarkerLock);
    s_rawSwipeMarkerActive = liveMarker;
    s_rawStrokeOwned = liveMarker && !liftGrace;
    s_rawSwipePublishTick = now;
    UpdateRawSwipeMarkerTimer();
}

static bool PreserveRawStrokeOnFocusLoss() {
    // The first shell gesture can activate another window before its native
    // switcher is intercepted. This is not a lift. Explicit clicks/Win-key
    // actions and Esc still cancel; between strokes ordinary focus loss cancels.
    ULONGLONG now = GetTickCount64();
    ULONGLONG readerTick = g_touchpadRawThreeFingerLastTick.load(
        std::memory_order_acquire);
    bool liveUiStroke = g_isTouchpadGestureActive && s_rawGestureTips > 0 &&
                        s_rawTouchpadLastFrameTick &&
                        now - s_rawTouchpadLastFrameTick <
                            SWS_RAW_SESSION_LOST_TIMEOUT_MS;
    bool liveCombinedStroke = CombinedRawReleasePending() && readerTick &&
                              now - readerTick < SWS_RAW_SESSION_LOST_TIMEOUT_MS;
    return RawTouchpadOwnsInput() && (liveUiStroke || liveCombinedStroke) &&
           !(GetAsyncKeyState(VK_LBUTTON) & 0x8000) &&
           !(GetAsyncKeyState(VK_RBUTTON) & 0x8000) &&
           !(GetAsyncKeyState(VK_MBUTTON) & 0x8000) &&
           !(GetAsyncKeyState(VK_LWIN) & 0x8000) &&
           !(GetAsyncKeyState(VK_RWIN) & 0x8000);
}

static int RoundDiv(int value, int divisor) {
    return (value >= 0) ? (value + divisor / 2) / divisor : -((-value + divisor / 2) / divisor);
}

static void CycleLinearBounded(int delta) {
    if (g_windows.empty()) return;
    int target = std::clamp(g_selectedIndex + delta, 0,
                            (int)g_windows.size() - 1);
    if (target != g_selectedIndex) CycleLinear(target - g_selectedIndex);
}

static RECT RawNavigationCell(int index) {
    if (index < 0 || index >= (int)g_windows.size()) return {};
    return g_layoutTransition.active ? g_windows[index].rcCellTarget : g_windows[index].rcCell;
}

// Move only within the current visual row (horizontal layout) or column
// (vertical layout). This intentionally does not advance into the next line;
// that is what makes a touchpad edge behave like the native switcher.
static bool NavigateRawWithinLine(bool horizontalSwipe, int delta) {
    if (g_selectedIndex < 0 || g_selectedIndex >= (int)g_windows.size() || !delta) return false;
    RECT current = RawNavigationCell(g_selectedIndex);
    if (IsRectEmpty(&current)) return false;

    const bool verticalLayout = LayoutIsVertical();
    const int lineCoord = verticalLayout ? current.left : current.top;
    std::vector<int> line;
    for (int i = 0; i < (int)g_windows.size(); ++i) {
        RECT cell = RawNavigationCell(i);
        if (IsRectEmpty(&cell)) continue;
        int coord = verticalLayout ? cell.left : cell.top;
        if (coord == lineCoord) line.push_back(i);
    }
    std::sort(line.begin(), line.end(), [&](int a, int b) {
        RECT ca = RawNavigationCell(a), cb = RawNavigationCell(b);
        int aa = verticalLayout ? (ca.top + ca.bottom) / 2 : (ca.left + ca.right) / 2;
        int ab = verticalLayout ? (cb.top + cb.bottom) / 2 : (cb.left + cb.right) / 2;
        return aa == ab ? a < b : aa < ab;
    });
    auto it = std::find(line.begin(), line.end(), g_selectedIndex);
    if (it == line.end()) return false;
    int pos = (int)(it - line.begin());
    int targetPos = std::clamp(pos + delta, 0, (int)line.size() - 1);
    if (targetPos == pos) return false;
    int target = line[targetPos];
    int previous = g_selectedIndex;
    CycleLinear(target - g_selectedIndex);
    return g_selectedIndex != previous;
}

// Touchpad navigation is spatial and bounded. Keyboard/mouse callers keep
// using the cyclic Cycle* functions above.
static bool TouchpadNavigate(bool horizontalSwipe, int delta) {
    if (g_windows.empty() || !delta) return false;
    const bool verticalLayout = LayoutIsVertical();
    const bool crossLine = (horizontalSwipe == verticalLayout);
    if (DockLayoutActive()) {
        if (!horizontalSwipe) return false;
        int previous = g_selectedIndex;
        CycleLinearBounded(delta);
        return g_selectedIndex != previous;
    }
    if (crossLine) {
        int previous = g_selectedIndex;
        for (int step = 0; step < abs(delta); ++step) {
            CycleDirectional(delta > 0 ? 1 : -1, false);
        }
        return g_selectedIndex != previous;
    }
    return NavigateRawWithinLine(horizontalSwipe, delta);
}

// Whether the layout actually shows more than one line of entries: a single-line task list
// has no rows to move through, and walking them would only churn the layout.
static bool LayoutHasMultipleLines() {
    if (DockLayoutActive()) return false;
    bool haveFirst = false;
    int firstCoord = 0;
    for (int i = 0; i < (int)g_windows.size(); i++) {
        if (IsWindowTruncated(i)) continue;
        const RECT& cell = g_layoutTransition.active ? g_windows[i].rcCellTarget : g_windows[i].rcCell;
        if (IsRectEmpty(&cell)) continue;
        int coord = LayoutIsVertical() ? cell.left : cell.top;
        if (!haveFirst) {
            firstCoord = coord;
            haveFirst = true;
        } else if (coord != firstCoord) {
            return true;
        }
    }
    return false;
}
static bool NavigateRawTouchpad(bool horizontal, int delta, int travel) {
    if (g_windows.empty()) return false;
    const bool verticalLayout = LayoutIsVertical();
    const bool crossLine = DockLayoutActive() ? false : (horizontal == verticalLayout);
    const WCHAR* route = DockLayoutActive() ? L"strip" : crossLine ? L"lines" : L"line";
    int before = g_selectedIndex;
    int startBefore = g_layoutStartIndex;
    int countBefore = (int)g_windows.size();
    bool moved = TouchpadNavigate(horizontal, delta);
    Wh_Log(L"SWS: raw touchpad move (%s=%d, route=%s, delta=%d, moved=%d, lines=%d, paginated=%d, entries=%d->%d, selected=%d->%d, start=%d->%d)",
           horizontal ? L"dx" : L"dy", travel, route, delta, moved,
           LayoutHasMultipleLines(), g_isPaginatedView, countBefore,
           (int)g_windows.size(), before, g_selectedIndex, startBefore,
           g_layoutStartIndex);
    return moved;
}

static bool ActivateNativeTouchpadWindow() {
    if (!g_hSwitcher || !IsWindow(g_hSwitcher)) return false;
    // Consume Explorer's supported grant before expensive list/layout/icon work
    // or another input packet can revoke it. The endpoint is revealed offscreen
    // without activation, never with an old painted frame on the desktop.
    SetWindowPos(g_hSwitcher, nullptr, -32000, -32000, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    ShowWindow(g_hSwitcher, SW_SHOWNA);
    SetLastError(ERROR_SUCCESS);
    BOOL granted = SetForegroundWindow(g_hSwitcher);
    DWORD error = granted ? ERROR_SUCCESS : GetLastError();
    bool active = GetForegroundWindow() == g_hSwitcher;
    Wh_Log(L"SWS: native PTP foreground handoff set=%d active=%d error=%u",
           granted, active, error);
    if (!active) ShowWindow(g_hSwitcher, SW_HIDE);
    return active;
}

static void CompleteNativeTouchpadInvocation() {
    if (!s_nativeTouchpadInvocation.token || !s_nativeTouchpadInvocation.completion) return;
    ULONGLONG now = GetTickCount64();
    if (!s_nativeTouchpadInvocation.rawAdopted &&
        now < s_nativeTouchpadInvocation.completionDeadline &&
        SetTimer(g_hSwitcher, SWS_NATIVE_TOUCHPAD_COMPLETION_TIMER_ID,
                 (UINT)(s_nativeTouchpadInvocation.completionDeadline - now), nullptr)) {
        return; // A queued timer from an old invocation must not shorten this wait.
    }
    UINT event = s_nativeTouchpadInvocation.completion;
    DWORD token = s_nativeTouchpadInvocation.token;
    DWORD tick = s_nativeTouchpadInvocation.completionTick;
    bool valid = s_nativeTouchpadInvocation.epoch == s_nativeTouchpadRelayEpoch.load() &&
                 g_touchpadGesturesEnabled.load() && g_touchpadReaderAvailable.load() &&
                 !g_touchpadReaderStopping.load();
    bool rawOwnsStroke = s_nativeTouchpadInvocation.rawAdopted &&
                        s_rawGestureSawThree && s_rawGestureTips > 0;
    CancelNativeTouchpadInvocation();
    Wh_Log(L"SWS: native PTP completion (event=%u token=%u rawOwnsStroke=%d sticky=%d)",
           event, token, rawOwnsStroke, g_isSticky);
    // Moving focus to SWS can finish/cancel the old native context while reports
    // now arrive in SWS. Once adopted, only the actual HID lift commits it.
    if (!valid || rawOwnsStroke) return;
    s_nativeTouchpadRawDiscardTick = tick;
    s_nativeTouchpadRawDiscardPending = true;
    g_isTouchpadGestureActive = false;
    KillTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID);
    if (!s_rawSessionOwned || (!g_isVisible && !g_isPendingShow)) return;
    if (event == SWS_NATIVE_TOUCHPAD_CANCEL) StartExitAnimation(false);
    else if (!g_isSticky) EndTouchpadGesture();
    // Sticky invocation's original swipe ends without accepting the selection.
}

static void HandleNativeTouchpadBoundary(DWORD packet, DWORD token) {
    DWORD event = packet & 7u;
    DWORD epoch = packet >> 3;
    DWORD policy = (DWORD)(ULONG_PTR)GetPropW(g_hSwitcher, SWS_NATIVE_SWIPE_POLICY_PROP);
    if (!token || !epoch || epoch != s_nativeTouchpadRelayEpoch.load() ||
        !TouchpadHandlingEnabled() ||
        !g_touchpadReaderAvailable.load() || g_touchpadReaderStopping.load() ||
        (policy & 3u) != 3u) {
        return;
    }
    if (event == 1 || event == 2 || event == 3) {
        DWORD age = (DWORD)GetTickCount64() - (DWORD)GetMessageTime();
        if (age > SWS_NATIVE_TOUCHPAD_START_MAX_AGE_MS ||
            (event == 2 && !(policy & 4u)) || !NativeSwipeSourceGateActive() ||
            g_isVisible || g_isPendingShow || g_animExitActive || g_isHidingSwitcher ||
            ExplicitShellInputDown()) {
            return;
        }
        HWND source = GetForegroundWindow();
        s_nativeTouchpadInvocation = {token, epoch,
            s_rawGestureSawThree && s_rawGestureTips > 0 &&
                GetTickCount64() - s_rawTouchpadLastFrameTick < 1000,
            event == 2};
        if (!ActivateNativeTouchpadWindow()) {
            CancelNativeTouchpadInvocation();
            return;
        }
        s_rawSessionOwned = true;
        g_isTouchpadGestureActive = true;
        s_rawIgnoreUntilLift = false;
        s_rawSwipePassedToWindows = false;
        s_rawGestureMoved = true;
        s_rawTapEligible = false;
        s_rawCommandEligible = false;
        s_rawVerticalActionDir = event == 2 ? -1 : 0;
        s_rawGestureAnchorX = s_rawDirectionAnchorX = s_rawLastFrameX;
        s_rawGestureAnchorY = s_rawDirectionAnchorY = s_rawLastFrameY;
        s_rawAppliedX = 0;
        s_rawRowTick = 0;
        if (!s_nativeTouchpadInvocation.rawAdopted) {
            // Discard stale state, not a fabricated HID contact or lift.
            s_rawGestureTips = 0;
            s_rawGestureSawThree = false;
            s_rawGestureArmed = false;
        }
        ShowSwitcher(event == 2, true, source);
        if (!s_nativeTouchpadInvocation.token ||
            s_nativeTouchpadInvocation.epoch != s_nativeTouchpadRelayEpoch.load() ||
            !TouchpadHandlingEnabled() ||
            !g_touchpadReaderAvailable.load() || g_touchpadReaderStopping.load() || !g_isVisible ||
            GetForegroundWindow() != g_hSwitcher) {
            HideSwitcher();
            return;
        }
        PromoteRawTouchpadShieldToSession();
        PublishRawSwipe();
        if (event != 2) {
            int step = event == 3 ? 1 : -1;
            if (g_settings.reverseScrollDirection) step = -step;
            NavigateRawTouchpad(true, step, 0);
        }
        KillTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID);
        SetTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID, SWS_RAW_SESSION_LOST_TIMEOUT_MS, nullptr);
        Wh_Log(L"SWS: native PTP invoked (direction=%u token=%u epoch=%u rawAdopted=%d)",
               event, token, epoch, s_nativeTouchpadInvocation.rawAdopted);
        return;
    }
    if ((event != SWS_NATIVE_TOUCHPAD_END && event != SWS_NATIVE_TOUCHPAD_CANCEL) ||
        s_nativeTouchpadInvocation.token != token || s_nativeTouchpadInvocation.epoch != epoch) {
        return;
    }
    if (s_nativeTouchpadInvocation.completion) return; // Do not extend the wait.
    s_nativeTouchpadInvocation.completion = event;
    s_nativeTouchpadInvocation.completionTick = (DWORD)GetMessageTime();
    s_nativeTouchpadInvocation.completionDeadline = GetTickCount64() + SWS_NATIVE_TOUCHPAD_HANDOFF_MS;
    CompleteNativeTouchpadInvocation();
}

static void HandleRawTouchpadFrame(ULONG tips, ULONG packedPos) {
    const ULONGLONG now = GetTickCount64();
    ++s_touchpadInputDiagnostics.uiFrames;
    s_touchpadInputDiagnostics.lastUiTick.store(now);
    if ((int)tips != s_rawGestureTips) {
        Wh_Log(L"SWS TAPTRACE ui tick=%llu eventTick=%u messageAgeMs=%u tips=%u previous=%d position=%u,%u "
               L"tap=%d visible=%d pending=%d exit=%d ignore=%d nativeDiscard=%d selected=%d",
               (unsigned long long)now, (DWORD)GetMessageTime(),
               (DWORD)now - (DWORD)GetMessageTime(), tips, s_rawGestureTips,
               packedPos & 0xFFFF, (packedPos >> 16) & 0xFFFF,
               s_rawTwoFingerTapActive, g_isVisible, g_isPendingShow,
               g_animExitActive, s_rawIgnoreUntilLift,
               s_nativeTouchpadRawDiscardPending, g_selectedIndex);
    }
    AdoptRawThreeFingerState();
    if (tips == 0 && s_altSessionOwner && s_combinedRawSeen) {
        s_combinedRawLiftHandled = true;
    }
    if (s_nativeTouchpadRawDiscardPending) {
        if ((LONG)((DWORD)GetMessageTime() - s_nativeTouchpadRawDiscardTick) <= 0) return;
        s_nativeTouchpadRawDiscardPending = false;
    }
    const int prevTips = s_rawGestureTips;
    s_rawGestureTips = (int)tips;

    if (!TouchpadHandlingEnabled() ||
        !g_touchpadReaderAvailable.load()) {
        CancelRawTouchpadStroke();
        return;
    }

    // Even an ignored frame can carry an early reader-thread publication.
    // Arrange physical cleanup without depending on UI gesture classification.
    if (!s_rawSwipeMarkerActive) UpdateRawSwipeMarkerTimer();

    if (tips == SWS_RAW_SWIPE_FINGERS && prevTips != SWS_RAW_SWIPE_FINGERS &&
        !s_rawGestureSawThree && !s_rawIgnoreUntilLift &&
        !g_isVisible && !g_isPendingShow) {
        // A stroke which has not opened the switcher belongs to Windows. In
        // particular, an outside downward swipe must remain Show Desktop even
        // when sticky launch is enabled. Do not let a previous session's lift
        // grace consume this new physical stroke.
        ClearRawSwipeMarker();
    }
    if (tips == SWS_RAW_SWIPE_FINGERS || (tips > 0 && s_rawStrokeOwned)) {
        s_rawTouchpadLastFrameTick = now;
        if (prevTips != (int)tips || now - s_rawSwipePublishTick >= 150) PublishRawSwipe();
    } else if (tips == 0 && prevTips > 0 && s_rawStrokeOwned) {
        // Publish before the ignore/exit paths: cancellation clears SawThree,
        // and a partial 3 -> 2 -> 0 lift is still the same owned stroke.
        s_rawTouchpadLastFrameTick = now;
        PublishRawSwipe(true);
    }

    if (g_animExitActive || (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0) {
        CancelRawTouchpadStroke();
        return;
    }
    if (s_rawIgnoreUntilLift) {
        if (tips == 0) {
            s_rawIgnoreUntilLift = false;
            s_rawSwipePassedToWindows = false;
        }
        return;
    }

    const int frameX = (int)(packedPos & 0xFFFF);
    const int frameY = (int)((packedPos >> 16) & 0xFFFF);
    bool closeTwoFingerTap = false;
    if (tips == 0) {
        closeTwoFingerTap = s_rawTwoFingerTapActive && !s_rawTwoFingerTapMoved &&
                            now - s_rawTwoFingerTapStartTick <= SWS_RAW_TAP_MAX_MS &&
                            g_isVisible && !g_animExitActive;
        if (s_rawTwoFingerTapActive) {
            Wh_Log(L"SWS TAPTRACE ui tap release tick=%llu heldMs=%llu close=%d selected=%d",
                   (unsigned long long)now,
                   (unsigned long long)(now - s_rawTwoFingerTapStartTick),
                   closeTwoFingerTap, g_selectedIndex);
        }
        ResetRawTwoFingerTap();
    } else if (s_rawTwoFingerTapActive) {
        if (tips > 2 ||
            abs(frameX - s_rawTwoFingerTapOriginX) > SWS_RAW_TAP_SLOP ||
            abs(frameY - s_rawTwoFingerTapOriginY) > SWS_RAW_TAP_SLOP) {
            Wh_Log(L"SWS TAPTRACE ui tap invalidated tick=%llu tips=%u delta=%d,%d slop=%d",
                   (unsigned long long)now, tips,
                   frameX - s_rawTwoFingerTapOriginX,
                   frameY - s_rawTwoFingerTapOriginY, SWS_RAW_TAP_SLOP);
            ResetRawTwoFingerTap();
        } else if (tips == 2 && prevTips == 0) {
            ResetRawTwoFingerTap();
        }
    } else if (tips == 2 && prevTips == 0 && g_isVisible && !g_isPendingShow) {
        Wh_Log(L"SWS TAPTRACE ui tap armed tick=%llu selected=%d",
               (unsigned long long)now, g_selectedIndex);
        s_rawTwoFingerTapActive = true;
        s_rawTwoFingerTapMoved = false;
        s_rawTwoFingerTapStartTick = now;
        s_rawTwoFingerTapOriginX = frameX;
        s_rawTwoFingerTapOriginY = frameY;
    }

    if (((s_rawSessionOwned && g_isTouchpadGestureActive) || s_rawTouchpadShieldActive) &&
        g_hSwitcher) {
        SetTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID, SWS_RAW_SESSION_LOST_TIMEOUT_MS, NULL);
    }

    if (tips == 0) {
        if (closeTwoFingerTap && g_selectedIndex >= 0 &&
            g_selectedIndex < (int)g_windows.size()) {
            Wh_Log(L"SWS TAPTRACE ui close tick=%llu selected=%d window=%p",
                   (unsigned long long)now, g_selectedIndex,
                   g_windows[g_selectedIndex].hWnd);
            CloseSwitcherEntry(g_selectedIndex);
            return;
        }
        const bool hadThree = s_rawGestureSawThree;
        const ULONGLONG held = now - s_rawGestureStartTick;
        const bool isTap = hadThree && s_rawTapEligible && !s_rawGestureMoved &&
                           held <= SWS_RAW_TAP_MAX_MS;
        s_rawGestureArmed = false;
        s_rawGestureSawThree = false;
        s_rawTapEligible = false;
        s_rawSwipePassedToWindows = false;
        s_rawCommandEligible = false;
        s_rawVerticalActionDir = 0;
        if (hadThree && s_rawSessionOwned && (g_isVisible || g_isPendingShow)) {
            CancelNativeTouchpadInvocation();
            if (g_isSticky && !isTap) {
                // Keep the session, not the physical stroke. Focus loss must still
                // dismiss sticky mode between swipes; no idle commit is needed.
                g_isTouchpadGestureActive = false;
                KillTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID);
                Wh_Log(L"SWS: raw touchpad lift -> sticky, staying open (held=%llu ms, selected=%d)",
                       held, g_selectedIndex);
            } else {
                Wh_Log(L"SWS: raw touchpad %s -> committing (held=%llu ms, selected=%d)",
                       g_isSticky ? L"tap" : L"lift", held, g_selectedIndex);
                // Use the shared activation path, including its popup resolution
                // and foreground fallback. Never reactivate a different target here.
                EndTouchpadGesture();
            }
        } else if (hadThree && (s_rawTouchpadShieldActive || s_rawSwipeMarkerActive)) {
            // No switcher was opened: keep the invisible foreground/controller
            // pair through the release edge, then restore the old application.
            ScheduleRawTouchpadShieldRelease();
        }
        return;
    }

    if (tips != SWS_RAW_SWIPE_FINGERS) {
        s_rawGestureArmed = false;
        if (tips > SWS_RAW_SWIPE_FINGERS) s_rawTapEligible = false;
        return;
    }

    int x = (int)(packedPos & 0xFFFF);
    int y = (int)((packedPos >> 16) & 0xFFFF);
    if (prevTips != SWS_RAW_SWIPE_FINGERS) {
        if (!s_rawGestureSawThree) {
            s_rawGestureSawThree = true;
            s_rawGestureStartTick = now;
            s_rawGestureMoved = false;
            s_rawTapEligible = g_isSticky && (g_isVisible || g_isPendingShow) &&
                               prevTips < SWS_RAW_SWIPE_FINGERS;
            s_rawTapOriginX = x;
            s_rawTapOriginY = y;
            s_rawVerticalActionDir = 0;
            s_rawCommandEligible = true;
        } else {
            // A partial lift/reland in the same stroke is not a fresh tap.
            s_rawTapEligible = false;
            s_rawCommandEligible = false;
        }
        s_rawGestureArmed = true;
        s_rawGestureAnchorX = x;
        s_rawGestureAnchorY = y;
        s_rawDirectionAnchorX = x;
        s_rawDirectionAnchorY = y;
        s_rawGestureAxis = 0;
        s_rawVerticalTravelDir = 0;
        s_rawAppliedX = 0;
        s_rawRowTick = 0;
        s_rawLastFrameX = x;
        s_rawLastFrameY = y;
        if (g_isVisible || g_isPendingShow) {
            s_rawSessionOwned = true;
            g_isTouchpadGestureActive = true;
            if (s_altSessionOwner && !g_isSticky) {
                KeepAltReleasePolling();
            } else {
                KillTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID);
            }
            if (g_isPendingShow) RevealPendingSwitcher();
            SetTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID, SWS_RAW_SESSION_LOST_TIMEOUT_MS, NULL);
        }
        if (s_nativeTouchpadInvocation.token) {
            s_nativeTouchpadInvocation.rawAdopted = true;
            s_rawGestureMoved = true;
            s_rawTapEligible = false;
            s_rawCommandEligible = false;
            s_rawVerticalActionDir = s_nativeTouchpadInvocation.upward ? -1 : 0;
            if (s_nativeTouchpadInvocation.completion) CompleteNativeTouchpadInvocation();
        }
        if (RawTouchpadOwnsInput()) {
            PublishRawSwipe();
        }
        return;
    }

    if (!s_rawGestureArmed) {
        return;
    }

    if (abs(x - s_rawTapOriginX) > SWS_RAW_TAP_SLOP ||
        abs(y - s_rawTapOriginY) > SWS_RAW_TAP_SLOP) {
        s_rawGestureMoved = true;
    }

    // A clamped or mirrored position (padding edges) arrives as a huge move between two
    // reports; treat it as a fresh start so it cannot fly the selection across the grid.
    int stepX = x - s_rawLastFrameX;
    int stepY = y - s_rawLastFrameY;
    s_rawLastFrameX = x;
    s_rawLastFrameY = y;
    if (stepX < 0) stepX = -stepX;
    if (stepY < 0) stepY = -stepY;
    if (stepX > SWS_RAW_SWIPE_JUMP_TRAVEL || stepY > SWS_RAW_SWIPE_JUMP_TRAVEL) {
        s_rawGestureMoved = true;
        s_rawGestureAnchorX = x;
        s_rawGestureAnchorY = y;
        s_rawDirectionAnchorX = x;
        s_rawDirectionAnchorY = y;
        s_rawGestureAxis = 0;
        s_rawVerticalTravelDir = 0;
        s_rawAppliedX = 0;
        s_rawRowTick = 0;
        Wh_Log(L"SWS: raw touchpad re-anchor after a jump (dX=%d, dY=%d)", stepX, stepY);
        return;
    }

    // Detect turns from recent travel, NOT all displacement since the first
    // horizontal movement. Otherwise a long horizontal drag masks a vertical
    // turn until it has travelled even farther vertically. Ignore small jitter.
    int turnX = x - s_rawDirectionAnchorX;
    int turnY = y - s_rawDirectionAnchorY;
    int axis = s_rawGestureAxis;
    if ((std::max)(abs(turnX), abs(turnY)) >= SWS_RAW_DIRECTION_TRAVEL) {
        if (abs(turnX) * SWS_RAW_SWIPE_DOMINANCE_DEN >= abs(turnY) * SWS_RAW_SWIPE_DOMINANCE_NUM) {
            axis = 1;
        } else if (abs(turnY) * SWS_RAW_SWIPE_DOMINANCE_DEN >= abs(turnX) * SWS_RAW_SWIPE_DOMINANCE_NUM) {
            axis = 2;
        } else if (axis == 0 && (std::max)(abs(turnX), abs(turnY)) >= SWS_RAW_SWIPE_ENTRY_PITCH / 2) {
            // A deliberate diagonal start still navigates. Once chosen, retain
            // the axis through the diagonal dead zone rather than oscillating.
            axis = abs(turnX) >= abs(turnY) ? 1 : 2;
        }
    }

    int verticalDir = axis == 2 && abs(turnY) >= SWS_RAW_DIRECTION_TRAVEL ?
                      (turnY > 0 ? 1 : -1) : s_rawVerticalTravelDir;
    bool verticalReversal = axis == 2 && s_rawVerticalTravelDir != 0 &&
                            verticalDir != s_rawVerticalTravelDir;
    if (axis != s_rawGestureAxis || verticalReversal) {
        s_rawGestureAxis = axis;
        s_rawGestureAnchorX = s_rawDirectionAnchorX;
        s_rawGestureAnchorY = s_rawDirectionAnchorY;
        s_rawAppliedX = 0;
        s_rawRowTick = 0;
        if (axis == 1 || verticalReversal) s_rawVerticalActionDir = 0;
        Wh_Log(L"SWS: raw touchpad direction -> %s", axis == 1 ? L"across" : L"rows");
    }
    s_rawVerticalTravelDir = axis == 2 ? verticalDir : 0;
    if (axis != 0 && (std::max)(abs(turnX), abs(turnY)) >= SWS_RAW_DIRECTION_TRAVEL) {
        s_rawDirectionAnchorX = x;
        s_rawDirectionAnchorY = y;
    }
    if (axis == 0) {
        return;
    }

    int dx = x - s_rawGestureAnchorX;
    int dy = y - s_rawGestureAnchorY;
    bool sessionOpen = g_isVisible || g_isPendingShow;
    // With a complete native source gate, only its typed Start opens SWS.
    // Raw reports remain the spatial navigation source; an unavailable gate
    // retains the original raw-only fallback without a second launch race.
    if (!sessionOpen && NativeSwipeSourceGateActive()) return;
    // Foreground recovery for identified native shell windows is handled by the
    // focus/WinEvent paths. Never fight an unrelated app the user switched to.
    if (axis == 1) {
        s_rawCommandEligible = false;
        // Across entries: the selection follows the finger position, one entry per twelfth of
        // the pad, and at most one per report so a fast drag catches up smoothly instead of
        // teleporting through the grid.
        int want = RoundDiv(dx, SWS_RAW_SWIPE_ENTRY_PITCH);
        if (want > s_rawAppliedX + 1) {
            want = s_rawAppliedX + 1;
        } else if (want < s_rawAppliedX - 1) {
            want = s_rawAppliedX - 1;
        }
        int delta = want - s_rawAppliedX;
        if (!delta) {
            return;
        }
        s_rawAppliedX = want;
        if (!sessionOpen) {
            if (s_rawSwipePassedToWindows) return;
            // No hotkey and no Explorer grant here: make this process eligible for the
            // foreground before opening, or the commit could not activate the selection.
            s_rawSessionOwned = true;
            BeginTouchpadGesture(0);
            if (!g_isVisible && !g_isPendingShow) return;
            PromoteRawTouchpadShieldToSession();
            PublishRawSwipe();
            if (g_hSwitcher && GetForegroundWindow() != g_hSwitcher) {
                // The tap did not land (seen in a capture): the switcher has to own the
                // foreground, or the commit cannot activate the selection.
                TapUnassignedKeyForForeground();
                ActivateRawTouchpadWindow();
            }
            Wh_Log(L"SWS: raw gesture opened (switcher foreground=%d)", GetForegroundWindow() == g_hSwitcher);
            // A previous session can end with the fingers still down (idle backstop): the
            // new session maps from where the hand is now.
            s_rawGestureAnchorX = x;
            s_rawGestureAnchorY = y;
            s_rawAppliedX = 0;
        }
        if (g_settings.reverseScrollDirection) {
            delta = -delta;
        }
        s_rawGestureMoved = true;
        if (!NavigateRawTouchpad(true, delta, dx)) {
            // Do not accumulate outward travel while parked on a spatial edge.
            s_rawGestureAnchorX = x;
            s_rawGestureAnchorY = y;
            s_rawAppliedX = 0;
        }
    } else {
        // Require new travel for every vertical step. A stationary heartbeat
        // must never repeat the last row/page move just because time elapsed.
        if (abs(dy) < SWS_RAW_SWIPE_ENTRY_PITCH) {
            return;
        }
        int physicalDir = dy > 0 ? 1 : -1;
        if (!sessionOpen) {
            if (s_rawSwipePassedToWindows) return;
            s_rawGestureMoved = true;
            // The reader normally publishes the upward candidate before this
            // UI message arrives. Repeat it here as a fallback for devices that
            // coalesce HID reports, so the shell's restore-windows action cannot
            // win the scheduling race.
            if (TouchpadStickyLaunchEnabled()) {
                SetRawSwipeUpMarker(physicalDir < 0);
            }
            // Up/down actions retain their physical meaning even when selection
            // scrolling is reversed. Normal horizontal opening stays non-sticky.
            if (!TouchpadStickyLaunchEnabled()) {
                s_rawSwipePassedToWindows = true;
                Wh_Log(L"SWS: raw vertical swipe outside session -> Windows");
            } else if (physicalDir < 0) {
                Wh_Log(L"SWS: raw touchpad upward swipe with no session -> sticky switcher");
                s_rawSessionOwned = true;
                g_isTouchpadGestureActive = true;
                ShowSwitcher(true, true);
                if (!g_isVisible && !g_isPendingShow) {
                    ScheduleRawTouchpadShieldRelease();
                    return;
                }
                PromoteRawTouchpadShieldToSession();
                PublishRawSwipe();
                if (GetForegroundWindow() != g_hSwitcher) {
                    ActivateRawTouchpadWindow();
                }
                // This opening stroke must not also drill into a group.
                s_rawVerticalActionDir = physicalDir;
                s_rawCommandEligible = false;
                s_rawGestureAnchorX = x;
                s_rawGestureAnchorY = y;
                SetTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID, SWS_RAW_SESSION_LOST_TIMEOUT_MS, NULL);
            } else {
                // Sticky mode changes only the upward launch gesture while the
                // switcher is closed. Downward strokes and stationary taps remain
                // Windows' gestures; the raw reader observes them but does not
                // claim them or publish a shell-suppression marker.
                s_rawSwipePassedToWindows = true;
                s_rawVerticalActionDir = physicalDir;
                s_rawCommandEligible = false;
                Wh_Log(L"SWS: raw vertical swipe outside session -> Windows (sticky launch=%d)",
                        TouchpadStickyLaunchEnabled());
            }
            return;
        }
        if (s_rawVerticalActionDir == physicalDir) return;
        if (s_rawRowTick && now - s_rawRowTick < SWS_RAW_SWIPE_ROW_INTERVAL_MS) {
            return;
        }
        s_rawRowTick = now;
        s_rawGestureAnchorX = x;
        s_rawGestureAnchorY = y;
        s_rawGestureMoved = true;
        if (g_isSticky && s_rawCommandEligible) {
            if (physicalDir < 0 && g_settings.showApplications && !g_drilledIn && g_selectedIndex >= 0 &&
                g_selectedIndex < (int)g_windows.size() &&
                g_windows[g_selectedIndex].groupWindows.size() > 1) {
                s_rawVerticalActionDir = physicalDir;
                s_rawCommandEligible = false;
                Wh_Log(L"SWS: raw touchpad up -> entering the application group");
                EnterAppGroup();
                return;
            }
            if (physicalDir > 0) {
                s_rawVerticalActionDir = physicalDir;
                s_rawCommandEligible = false;
                if (g_drilledIn) {
                    Wh_Log(L"SWS: raw touchpad down -> leaving the application group");
                    ExitAppGroup();
                } else {
                    Wh_Log(L"SWS: raw touchpad down -> dismissing sticky switcher");
                    // Use the same reverse reveal animation as every other
                    // non-commit dismissal; HideSwitcher would snap the window
                    // away and release the controller immediately.
                    StartExitAnimation(false);
                }
                return;
            }
        }
        s_rawCommandEligible = false;
        int delta = g_settings.reverseScrollDirection ? -physicalDir : physicalDir;
        if (!NavigateRawTouchpad(false, delta, dy)) {
            s_rawGestureAnchorX = x;
            s_rawGestureAnchorY = y;
            s_rawRowTick = 0;
        }
    }

    s_lastTouchpadScrollTick = GetTickCount64();
    if (g_hSwitcher) {
        SetTimer(g_hSwitcher, SWS_TOUCHPAD_IDLE_TIMER_ID, SWS_RAW_SESSION_LOST_TIMEOUT_MS, NULL);
    }
}

static LRESULT CALLBACK SwitcherWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_SWS_NATIVE_TOUCHPAD) {
        if (hWnd == g_hSwitcher) HandleNativeTouchpadBoundary((DWORD)wParam, (DWORD)lParam);
        return 0;
    }
    if (uMsg == WM_SWS_TOUCHPAD_DIAGNOSTICS) {
        Wh_Log(L"SWS: native input snapshot event=0x%X %s", (UINT)wParam,
               FormatTouchpadInputDiagnostics().c_str());
        // The UI being responsive does not prove the separate reader pump is.
        // Ask that thread to sample itself; a reply with unchanged raw counters
        // distinguishes absent delivery from a stalled reader message pump.
        HWND reader = g_touchpadReaderWindow.load();
        if (reader && !PostMessageW(reader, WM_SWS_TOUCHPAD_READER_DIAGNOSTICS,
                                   wParam, (LPARAM)(DWORD)GetTickCount64())) {
            DWORD error = GetLastError();
            Wh_Log(L"SWS: reader input snapshot post failed (event=0x%X reader=%p error=%u)",
                   (UINT)wParam, reader, error);
        }
        return 0;
    }
    if (uMsg == WM_SWS_ICON_READY) {
        if (hWnd == g_hSwitcher) ConsumeWindowIconResults();
        return 0;
    }
    const bool controlKey = wParam == VK_CONTROL || wParam == VK_LCONTROL || wParam == VK_RCONTROL;
    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
        if (!controlKey) g_ctrlTapPending = false;
    } else if ((uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) && controlKey) {
        bool tap = g_ctrlTapPending;
        g_ctrlTapPending = false;
        if (tap && g_isVisible && !g_animExitActive && g_settings.showApplications) ToggleAppDrill();
        return 0;
    }
    if (g_animExitActive) {
        if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP ||
            uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE ||
            uMsg == WM_HOTKEY || uMsg == WM_SWS_SCROLL) {
            return 0;
        }
    }

    if (uMsg == WM_NCCALCSIZE) {
        return 0; // Remove standard frame for WS_OVERLAPPED and WS_THICKFRAME
    }
    if (uMsg == WM_NCPAINT) {
        return 0; // Suppress default non-client frame/border painting for ALL themes
    }
    if (uMsg == WM_NCACTIVATE) {
        // Pass TRUE for wParam so DWM keeps the Mica backdrop active.
        // Pass -1 for lParam to prevent DefWindowProc from painting the default border (suppressing the white/gray flash).
        return DefWindowProcW(hWnd, uMsg, TRUE, -1);
    }

    if (g_WM_SWS_TOUCHPAD_FRAME && uMsg == g_WM_SWS_TOUCHPAD_FRAME) {
        HandleRawTouchpadFrame((ULONG)wParam, (ULONG)lParam);
        return 0;
    }

    if (uMsg == WM_TIMER) {
        if (wParam == SWS_RAW_TWO_TAP_MOUSE_TIMER_ID) {
            UpdateTwoFingerTapMouseHookLifetime();
            return 0;
        }

        if (wParam == SWS_TWO_FINGER_TAP_RESTORE_TIMER_ID) {
            UpdateTwoFingerTapOverride(g_touchpadTwoFingerCloseActive.load());
            return 0;
        }
        if (wParam == SWS_NATIVE_TOUCHPAD_COMPLETION_TIMER_ID) {
            KillTimer(hWnd, SWS_NATIVE_TOUCHPAD_COMPLETION_TIMER_ID);
            CompleteNativeTouchpadInvocation();
            return 0;
        }
        if (wParam == SWS_HOTKEY_RETRY_TIMER_ID) {
            SWS_RegisterHotkeys();
            return 0;
        }

        if (wParam == SWS_SHOW_DELAY_TIMER_ID) {
            RevealPendingSwitcher();
            return 0;
        }

        if (wParam == SWS_ALT_POLL_TIMER_ID) {
            if (g_isTouchpadGestureActive && !s_altSessionOwner) {
                return 0; // Physical Alt key polling strictly disabled during touchpad gestures
            }
            if (!g_isSticky && (GetAsyncKeyState(VK_MENU) & 0x8000) == 0) {
                if (s_altSessionOwner) {
                    HandleAltRelease();
                    return 0;
                }
                if (GetTickCount64() - s_lastTouchpadScrollTick < 1200) {
                    return 0; // Grace period while user is scrolling/gesturing with touchpad
                }
                KillTimer(hWnd, SWS_ALT_POLL_TIMER_ID);
                SwitchToSelected();
            }
            return 0;
        }

        if (wParam == SWS_TOUCHPAD_TARGET_FOCUS_RETRY_TIMER_ID) {
            if (((g_isVisible || g_isPendingShow) && !g_animExitActive) ||
                !s_touchpadActivationRetryTarget ||
                !IsWindow(s_touchpadActivationRetryTarget) ||
                GetTickCount64() >= s_touchpadActivationRetryDeadline ||
                ExplicitShellInputDown()) {
                if (s_touchpadActivationRetryTarget && !g_isVisible && !g_isPendingShow) {
                    Wh_Log(L"SWS: selected-window activation retry ended");
                }
                KillTimer(hWnd, SWS_TOUCHPAD_TARGET_FOCUS_RETRY_TIMER_ID);
                s_touchpadActivationRetryTarget = NULL;
                s_touchpadActivationRetryDeadline = 0;
                s_touchpadCommitActivation = false;
                return 0;
            }
            HWND target = s_touchpadActivationRetryTarget;
            ActivateExitedWindow(target, {});
            return 0;
        }

        if (wParam == SWS_TOUCHPAD_SHIELD_RELEASE_TIMER_ID) {
            KillTimer(hWnd, SWS_TOUCHPAD_SHIELD_RELEASE_TIMER_ID);
            if ((s_rawTouchpadShieldActive || s_rawSwipeMarkerActive) && !s_rawSessionOwned &&
                !g_isVisible && !g_isPendingShow) {
                if (s_rawTouchpadShieldReleasePending && s_rawGestureTips == 0) {
                    FinishRawTouchpadShield(true);
                } else if (s_rawTouchpadShieldReleasePending) {
                    if (GetTickCount64() - s_rawTouchpadLastFrameTick >=
                        SWS_RAW_SESSION_LOST_TIMEOUT_MS) {
                        // A cancelled, hidden session must not retain a marker
                        // or keep retrying forever when the lift report is lost.
                        FinishRawTouchpadShield();
                    } else {
                        SetTimer(hWnd, SWS_TOUCHPAD_SHIELD_RELEASE_TIMER_ID, 220, NULL);
                    }
                }
            } else {
                s_rawTouchpadShieldReleasePending = false;
            }
            return 0;
        }

        if (wParam == SWS_RAW_SWIPE_MARKER_EXPIRY_TIMER_ID) {
            UpdateRawSwipeMarkerTimer();
            RefreshTouchpadGestureKinds();
            return 0;
        }


        if (wParam == SWS_TOUCHPAD_SHIELD_FOCUS_RETRY_TIMER_ID) {
            KillTimer(hWnd, SWS_TOUCHPAD_SHIELD_FOCUS_RETRY_TIMER_ID);
            if (!s_rawTouchpadShieldActive || s_rawGestureTips == 0) {
                return 0;
            }
            if (ActivateRawTouchpadWindow()) {
                TapUnassignedKeyForForeground();
                s_rawTouchpadShieldFocusStartTick = 0;
                RefreshTouchpadGestureKinds();
                Wh_Log(L"SWS: sticky raw gesture shield foreground retry succeeded");
            } else if (GetTickCount64() - s_rawTouchpadShieldFocusStartTick < 250) {
                SetTimer(hWnd, SWS_TOUCHPAD_SHIELD_FOCUS_RETRY_TIMER_ID, 8, NULL);
            } else {
                s_rawTouchpadShieldFocusStartTick = 0;
                Wh_Log(L"SWS: sticky raw gesture shield foreground retry window expired; shell marker remains active");
            }
            return 0;
        }

        if (wParam == SWS_TOUCHPAD_IDLE_TIMER_ID) {
            KillTimer(hWnd, SWS_TOUCHPAD_IDLE_TIMER_ID);
            AdoptRawThreeFingerState();
            if (s_altSessionOwner && s_combinedRawSeen &&
                !s_combinedRawLiftHandled) {
                ULONGLONG readerTick =
                    g_touchpadRawThreeFingerLastTick.load(
                        std::memory_order_acquire);
                if (readerTick && GetTickCount64() - readerTick <
                                      SWS_RAW_SESSION_LOST_TIMEOUT_MS) {
                    SetTimer(hWnd, SWS_TOUCHPAD_IDLE_TIMER_ID,
                             SWS_RAW_SESSION_LOST_TIMEOUT_MS, NULL);
                    return 0;
                }
                Wh_Log(L"SWS: combined raw input lost -> cancelling without selection");
                HideSwitcher();
                return 0;
            }
            if (s_rawTouchpadShieldActive) {
                if (s_rawTouchpadShieldReleasePending && s_rawGestureTips == 0) {
                    SetTimer(hWnd, SWS_TOUCHPAD_IDLE_TIMER_ID,
                             SWS_RAW_SESSION_LOST_TIMEOUT_MS, NULL);
                    return 0;
                }
                if (s_rawGestureTips > 0 &&
                    GetTickCount64() - s_rawTouchpadLastFrameTick < SWS_RAW_SESSION_LOST_TIMEOUT_MS) {
                    SetTimer(hWnd, SWS_TOUCHPAD_IDLE_TIMER_ID,
                             SWS_RAW_SESSION_LOST_TIMEOUT_MS, NULL);
                    return 0;
                }
                Wh_Log(L"SWS: raw sticky shield lost input -> releasing ownership");
                FinishRawTouchpadShield();
                s_rawGestureTips = 0;
                s_rawGestureSawThree = false;
                s_rawGestureArmed = false;
                s_rawIgnoreUntilLift = false;
                return 0;
            }
            if (s_rawSessionOwned && s_rawGestureTips > 0 &&
                GetTickCount64() - s_rawTouchpadLastFrameTick < SWS_RAW_SESSION_LOST_TIMEOUT_MS) {
                SetTimer(hWnd, SWS_TOUCHPAD_IDLE_TIMER_ID, SWS_RAW_SESSION_LOST_TIMEOUT_MS, NULL);
                return 0; // A stale queued timer is not a finger lift.
            }
            if (s_rawSessionOwned && g_isSticky) {
                CancelRawTouchpadStroke();
                g_isTouchpadGestureActive = false;
                return 0;
            }
            // Missing raw input is not proof of a lift. Cancel rather than activate
            // an unintended window when the reader loses a contact-release report.
            if (g_isTouchpadGestureActive && (g_isVisible || g_isPendingShow) && !g_isSticky) {
                Wh_Log(L"SWS: raw input lost -> cancelling session without selection");
                HideSwitcher();
            }
            return 0;
        }

        if (wParam == SWS_CLOSE_VERIFY_TIMER_ID) {
            KillTimer(hWnd, SWS_CLOSE_VERIFY_TIMER_ID);
            std::vector<HWND> toRemove;
            ULONGLONG now = GetTickCount64();
            for (auto it = s_pendingCloseWindows.begin(); it != s_pendingCloseWindows.end(); ) {
                HWND h = *it;
                auto deadline = s_pendingCloseDeadlines.find(h);
                if (!IsWindow(h) || (!IsWindowVisible(h) && !IsIconic(h))) {
                    // Window truly closed (or hid itself) — remove its entry now.
                    // With lazy removal, the entry was never taken out eagerly, so
                    // this is the single point of removal and the departing fade
                    // starts from the entry's real position.
                    toRemove.push_back(h);
                    s_pendingCloseDeadlines.erase(h);
                    it = s_pendingCloseWindows.erase(it);
                } else if (!CanCloseWindow(h) || deadline == s_pendingCloseDeadlines.end() || now >= deadline->second) {
                    // A modal refusal is protected by CanCloseWindow. Otherwise
                    // retain deduplication for a bounded asynchronous cooldown,
                    // then permit a fresh user request without reposting here.
                    s_pendingCloseDeadlines.erase(h);
                    it = s_pendingCloseWindows.erase(it);
                } else {
                    ++it;
                }
            }
            for (HWND h : toRemove) {
                RemoveMruWindow(h);
                RemoveWindowEntryByHwnd(h);
            }
            if (!s_pendingCloseWindows.empty()) {
                SetTimer(hWnd, SWS_CLOSE_VERIFY_TIMER_ID, 200, NULL);
            } else {
                s_pendingCloseRetries = 0;
                s_pendingCloseDeadlines.clear();
            }
            return 0;
        }

        if (wParam == SWS_DYNAMIC_RESIZE_TIMER_ID) {
            // Fail-safe: the backdrop must never cover the desktop without the switcher
            // on top of it. This timer only runs while a session exists; an exit keeps its
            // own fade-out, so that case is excluded.
            if (g_hBackdropWnd && IsWindowVisible(g_hBackdropWnd) && !g_animExitActive &&
                (!g_hSwitcher || !IsWindowVisible(g_hSwitcher))) {
                HideBackdropBlur();
            }
            // Never recenter/resize mid-entrance-fade: a SetWindowPos here is
            // exactly the visible "drift to center" during reveal. Sizes are
            // refreshed at show/reveal time, and the next tick after the
            // entrance completes applies any genuinely late DWM size in one step.
            if (g_isVisible && !g_windows.empty() && !g_scrollTransition.active && !g_layoutTransition.active && !g_animEntranceActive) {
                bool anyChanged = false;
                for (auto& w : g_windows) {
                    if (RefreshEntrySourceSize(w)) {
                        anyChanged = true;
                    }
                }
                if (anyChanged) {
                    RecomputeAndReposition();
                    if (DockLayoutActive()) {
                        UpdateDockPreviewForSelection();
                    }
                    RegisterThumbnails();
                    PaintSwitcher();
                    if (g_hCloseBtnWnd) {
                        PaintSwitcherOverlay();
                    }
                }
            }
            return 0;
        }
    }
    if (uMsg == WM_HOTKEY) {
        g_ctrlTapPending = false;
        if (g_settings.excludeXboxMode && IsXboxModeOrForeground()) {
            SWS_UnregisterHotkeys();
            INPUT inputs[2] = {};
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_TAB;
            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_TAB;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, inputs, sizeof(INPUT));
            SetTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID, 500, NULL);
            return 0;
        }

        int id = (int)wParam;
        bool isBackward = false;
        bool isCtrl = false;
        bool isAltBacktickTrigger = false;

        switch (id) {
        case SWS_HOTKEY_ALTTAB:
            break;
        case SWS_HOTKEY_WINALTTAB:
            g_showAllMonitors = true;
            break;
        case SWS_HOTKEY_ALTSHIFTTAB:
            if (!UseAltShiftTabBackward()) return 0;
            isBackward = true;
            break;
        case SWS_HOTKEY_WINALTSHIFTTAB:
            if (!UseAltShiftTabBackward()) return 0;
            isBackward = true;
            g_showAllMonitors = true;
            break;
        case SWS_HOTKEY_ALTCTRLTAB:
            isCtrl = true;
            break;
        case SWS_HOTKEY_ALTSHIFTCTRLTAB:
            if (!UseAltShiftTabBackward()) return 0;
            isBackward = true;
            isCtrl = true;
            break;
        case SWS_HOTKEY_ALTBACKTICK:
        case SWS_HOTKEY_ALTBACKTICK_UK:
            if (wcscmp(g_settings.altBacktickBehavior, L"sameApp") == 0) {
                isAltBacktickTrigger = true;
            } else if (wcscmp(g_settings.altBacktickBehavior, L"backward") == 0) {
                isBackward = true;
            } else {
                return 0;
            }
            break;
        default:
            return 0;
        }

        if (!s_altSessionOwner) {
            ULONGLONG rawState = g_touchpadRawThreeFingerState.load(
                std::memory_order_acquire);
            s_altRawBaselineSerial = RawThreeFingerStateSerial(rawState);
            s_combinedRawSerial = 0;
            s_combinedRawSeen = false;
            s_combinedRawLiftHandled = false;
            s_commitStarted = false;
        }
        s_altSessionOwner = true;
        s_altHeld = true;
        AdoptRawThreeFingerState(true);

        if (!g_isVisible && !g_isPendingShow) {
            HWND hFg = GetForegroundWindow();
            if (hFg && !IsSwitcherWindow(hFg) && IsEligibleWindow(hFg)) {
                UpdateMruWindow(hFg);
            }
            if (isAltBacktickTrigger) g_isAltBacktickSameApp = true;
            ShowSwitcher(isCtrl);

            // When "Cycle Between Windows of Current Application" is combined with
            // "Group Windows by Application", skip the collapsed group entry and open
            // the current app's windows directly (issue #5532) — no extra Ctrl tap.
            if (isAltBacktickTrigger && g_settings.showApplications && !g_drilledIn &&
                g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() &&
                g_windows[g_selectedIndex].groupWindows.size() > 1) {
                if (!g_isPendingShow) {
                    // Immediate reveal: drill in now.
                    EnterAppGroup();
                } else {
                    // Show-delay path: defer the drill until the pending reveal runs.
                    // (RevealPendingSwitcher -> g_drillInAfterReveal consumed below.)
                    g_drillInAfterReveal = true;
                }
            }

            if (isBackward && g_windows.size() > 1) {
                g_selectedIndex = (int)g_windows.size() - 1;
                if (IsWindowTruncated(g_selectedIndex)) {
                    CycleLinear(0);
                } else {
                    if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
                        SnapSelectionTo(ToRectF(g_windows[g_selectedIndex].rcCell));
                    }
                }

                if (g_isVisible) {
                    PaintSwitcher();
                }
            }
        } else {
            if (isCtrl) {
                g_isSticky = true;
                KillTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID);
                RefreshTouchpadGestureKinds();
            }

            CycleLinear(isBackward ? -1 : 1);

            if (g_isPendingShow) {
                RevealPendingSwitcher();
            }
        }
        return 0;
    }

    // WM_PAINT for Acrylic (non-layered) path
    if (uMsg == WM_PAINT && !ThemeIs(L"none") && g_isVisible) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);
        int w = rc.right, h = rc.bottom;
        BP_PAINTPARAMS params = { sizeof(params) };
        // NOTE: no BPPF_ERASE here on purpose. Buffered-paint erase fills the
        // buffer with opaque white for one frame (white border/corner flash on
        // reveal). The buffer is explicitly cleared to transparent below and
        // every branch overpaints it fully.
        params.dwFlags = 0;
        HDC hdcBuf = NULL;
        HPAINTBUFFER hBP = BeginBufferedPaint(hdc, &rc, BPBF_TOPDOWNDIB, &params, &hdcBuf);
        if (hBP) {
            if (w > 0 && h > 0) {
                PatBlt(hdcBuf, 0, 0, w, h, BLACKNESS);
            }
            if (!g_scrollTransition.active && w > 0 && h > 0) {
                if (!s_cachedStaticDC || s_cachedStaticW != w || s_cachedStaticH != h) {
                    if (s_cachedStaticDC) {
                        if (s_cachedStaticOldBitmap) SelectObject(s_cachedStaticDC, s_cachedStaticOldBitmap);
                        if (s_cachedStaticBitmap) DeleteObject(s_cachedStaticBitmap);
                        DeleteDC(s_cachedStaticDC);
                    }
                    HDC hdcScreen = GetDC(hWnd);
                    s_cachedStaticDC = CreateCompatibleDC(hdcScreen);
                    BITMAPINFO bmi = {}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
                    bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
                    s_cachedStaticBitmap = CreateDIBSection(s_cachedStaticDC, &bmi, DIB_RGB_COLORS, &s_cachedStaticBits, NULL, 0);
                    s_cachedStaticOldBitmap = (HBITMAP)SelectObject(s_cachedStaticDC, s_cachedStaticBitmap);
                    s_cachedStaticW = w;
                    s_cachedStaticH = h;
                    ReleaseDC(hWnd, hdcScreen);
                    g_staticContentDirty = true;
                }
                if (g_staticContentDirty) {
                    if (s_cachedStaticBits) {
                        memset(s_cachedStaticBits, 0, (size_t)w * h * sizeof(DWORD));
                    }
                    int radius = GetWindowCornerRadiusPx();
                    HRGN hClip = GetCachedRoundRectRgn(w, h, radius);
                    SelectClipRgn(s_cachedStaticDC, hClip);
                    DrawSwitcherStaticContent(s_cachedStaticDC, ShouldFillBackground(), hWnd);
                    g_staticContentDirty = false;
                }
                BitBlt(hdcBuf, 0, 0, w, h, s_cachedStaticDC, 0, 0, SRCCOPY);

                // Dynamic thumbnail drop shadow for any zoomed/animating thumbnails
                if (g_settings.showThumbnails && g_settings.showThumbnailShadow) {
                    if (DockLayoutActive() && DockShowPreview()) {
                        if (g_layoutTransition.active || g_dockPreviewSlide.active) {
                            RECT shadowRc = g_rcCentralPreview;
                            float shadowAlphaMult = 1.0f;
                            if (g_dockPreviewSlide.active) {
                                int offX = (int)roundf(g_dockPreviewSlide.currentOffset);
                                shadowRc.left += offX;
                                shadowRc.right += offX;
                                shadowAlphaMult = g_dockPreviewSlide.currentAlpha;
                            }
                            DrawThumbnailShadow(hdcBuf, shadowRc, GetThumbnailCornerRadiusPx(), shadowAlphaMult);
                        }
                    } else {
                        if (g_layoutTransition.active) {
                            for (size_t i = 0; i < g_windows.size(); ++i) {
                                const auto& e = g_windows[i];
                                if (e.rcThumbActual.right > e.rcThumbActual.left && !IsWindowTruncated((int)i)) {
                                    float itemAlpha = e.isNewEntry ? e.enterAlpha : 1.0f;
                                    DrawThumbnailShadow(hdcBuf, e.rcThumbActual, GetThumbnailCornerRadiusPx(), itemAlpha);
                                }
                            }
                        } else if (ThumbnailHoverIsZoom()) {
                            for (size_t i = 0; i < g_windows.size(); ++i) {
                                const auto& e = g_windows[i];
                                if (e.hoverScale > 1.0001f && !IsWindowTruncated((int)i)) {
                                    RECT scaledRc = GetScaledThumbRect(e);
                                    float shadowAlphaMult = (e.hoverScale - 1.0f) / SWS_HOVER_ZOOM_DELTA;
                                    if (shadowAlphaMult < 0.0f) shadowAlphaMult = 0.0f;
                                    if (shadowAlphaMult > 1.0f) shadowAlphaMult = 1.0f;
                                    DrawThumbnailShadow(hdcBuf, scaledRc, GetThumbnailCornerRadiusPx(), shadowAlphaMult, e.hoverScale);
                                }
                            }
                        }
                    }
                }
                if (DockLayoutActive()) {
                    DrawDockIconStrip(hdcBuf);
                } else if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() && HighlightHasFill()) {
                    RectF fillRc = SelectionRectWithViewport();
                    DrawSelectionFillF(hdcBuf, fillRc);
                }
            } else {
                DrawScrollTransitionFrame(hdcBuf, w, h, true);
            }
            EndBufferedPaint(hBP, TRUE);
        }
        EndPaint(hWnd, &ps);
        return 0;
    }

    switch (uMsg) {
    case WM_KEYUP:
        if (g_isVisible && UseAltShiftBackward() && wParam == VK_TAB) {
            bool altDown = (GetKeyState(VK_MENU) & 0x8000) != 0;
            bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            if (altDown && shiftDown) {
                return 0;
            }
        }

        if (wParam == VK_MENU && (g_isVisible || g_isPendingShow) && !g_isSticky) {
            HandleAltRelease();
            return 0;
        }
        if (wParam == VK_ESCAPE && (g_isVisible || g_isPendingShow)) {
            if (g_consumeEscUp) { g_consumeEscUp = false; return 0; }
            StartExitAnimation(false);
            return 0;
        }
        if (wParam == VK_RETURN && (g_isVisible || g_isPendingShow)) { SwitchToSelected(); return 0; }
        break;
    case WM_SYSKEYUP:
        if (g_isVisible && UseAltShiftBackward() && wParam == VK_TAB) {
            bool altDown = (GetKeyState(VK_MENU) & 0x8000) != 0;
            bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            if (altDown && shiftDown) {
                return 0;
            }
        }

        if (wParam == VK_MENU && (g_isVisible || g_isPendingShow) && !g_isSticky) {
            HandleAltRelease();
            return 0;
        }
        break;
    case WM_SYSKEYDOWN: case WM_KEYDOWN:
        if (wParam == VK_F4) return 0;
        if (wParam == VK_ESCAPE && g_isPendingShow) {
            StartExitAnimation(false);
            return 0;
        }
        if (g_isPendingShow) {
            if (wParam == VK_RETURN || wParam == VK_SPACE) {
                SwitchToSelected();
                return 0;
            }
            RevealPendingSwitcher();
        }
        if (g_isVisible) {
            // Ctrl tap: drill into / out of the selected application's windows.
            if ((wParam == VK_CONTROL || wParam == VK_LCONTROL || wParam == VK_RCONTROL)
                && g_settings.showApplications) {
                bool isRepeat = (lParam & 0x40000000) != 0;
                if (!isRepeat) g_ctrlTapPending = true;
                return 0;
            }
            // Block Alt+Shift+Tab from reaching the system if setting is enabled
            if (UseAltShiftBackward() && wParam == VK_TAB) {
                bool altDown = (GetKeyState(VK_MENU) & 0x8000) != 0;
                bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                if (altDown && shiftDown) {
                    // Suppress native switcher
                    return 0;
                }
            }
            if (UseAltShiftBackward() &&
                (wParam == VK_SHIFT || wParam == VK_LSHIFT || wParam == VK_RSHIFT)) {
                bool isRepeat = (lParam & 0x40000000) != 0;
                bool altDown = (GetKeyState(VK_MENU) & 0x8000) != 0;
                if (!isRepeat && altDown) {
                    CycleLinear(-1);
                    return 0;
                }
            }

            if (wParam == VK_TAB) {
                bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                bool backward = UseAltShiftTabBackward() && shiftDown;
                CycleLinear(backward ? -1 : 1);
                return 0;
            }
            if ((wParam == VK_OEM_3 || wParam == VK_OEM_8) &&
                wcscmp(g_settings.altBacktickBehavior, L"backward") == 0) {
                CycleLinear(-1);
                return 0;
            }
            if (!LayoutIsVertical()) {
                if (wParam == VK_LEFT) { CycleLinear(-1); return 0; }
                if (wParam == VK_RIGHT) { CycleLinear(1); return 0; }
                if (wParam == VK_UP) { CycleDirectional(-1); return 0; }
                if (wParam == VK_DOWN) { CycleDirectional(1); return 0; }
            } else {
                if (wParam == VK_UP) { CycleLinear(-1); return 0; }
                if (wParam == VK_DOWN) { CycleLinear(1); return 0; }
                if (wParam == VK_LEFT) { CycleDirectional(-1); return 0; }
                if (wParam == VK_RIGHT) { CycleDirectional(1); return 0; }
            }
            if (wParam == VK_PRIOR) { CyclePage(-1); return 0; }
            if (wParam == VK_NEXT) { CyclePage(1); return 0; }
            if (wParam == VK_ESCAPE) {
                if (g_drilledIn) { ExitAppGroup(); g_consumeEscUp = true; }
                else StartExitAnimation(false);
                return 0;
            }
            if (wParam == VK_RETURN || wParam == VK_SPACE) { SwitchToSelected(); return 0; }
            bool isCtrlW = (wParam == 'W' && (GetKeyState(VK_CONTROL) & 0x8000) != 0);
            if (wParam == 'Q' || wParam == VK_DELETE || isCtrlW) {
                bool isRepeat = (lParam & 0x40000000) != 0;
                if (!isRepeat) CloseSwitcherEntry(g_selectedIndex);
                return 0;
            }
        }
        break;
    // (Removed duplicate combined case for WM_SYSKEYUP and WM_KEYUP)
    case WM_SWS_SCROLL:
        if (g_isVisible) {
            int dir = (int)wParam;
            int action = (int)lParam;
            if (action == 1) { // selection
                if (dir > 0) {
                    for (int k = 0; k < dir; k++) CycleLinear(1);
                } else if (dir < 0) {
                    for (int k = 0; k < -dir; k++) CycleLinear(-1);
                }
            } else if (action == 2) { // page
                if (dir > 0) {
                    for (int k = 0; k < dir; k++) CyclePage(1);
                } else if (dir < 0) {
                    for (int k = 0; k < -dir; k++) CyclePage(-1);
                }
            }
        }
        return 0;
    case WM_SWS_CANCEL_INPUT:
        Wh_Log(L"SWS TAPTRACE cancellation tick=%llu visible=%d pending=%d exit=%d tips=%d tap=%d selected=%d",
               (unsigned long long)GetTickCount64(), g_isVisible, g_isPendingShow,
               g_animExitActive, s_rawGestureTips, s_rawTwoFingerTapActive,
               g_selectedIndex);
        if (g_isVisible || g_isPendingShow) StartExitAnimation(false);
        return 0;
    case WM_SWS_TOUCHPAD_READER_CHANGED:
        if (!g_touchpadReaderAvailable.load()) {
            if (s_rawSessionOwned || s_combinedRawSeen) HideSwitcher();
            CancelRawTouchpadStroke();
            s_rawTouchpadLastFrameTick = 0;
            FinishRawTouchpadShield();
        }
        RefreshTouchpadGestureKinds();
        return 0;
    case WM_SWS_SETTINGS_CHANGED:
        if (g_isVisible || g_isPendingShow) HideSwitcher();
        CancelRawTouchpadStroke();
        s_rawTouchpadLastFrameTick = 0;
        FinishRawTouchpadShield();
        FreeCachedBuffers();
        g_staticContentDirty = true;
        // Reset the touchpad grace window so a mid-gesture settings reload doesn't
        // inherit stale scroll timing from before the reload.
        ResetScrollWheelAccumulators();
        SWS_UnregisterHotkeys();
        LoadSettings();
        if (!g_settings.handleTouchpadGestures) {
            s_rawGestureTips = 0;
            s_rawIgnoreUntilLift = false;
            FinishRawTouchpadShield();
        }
        g_touchpadGestureTakeoverAvailable.store(true);
        UpdateTouchpadGestureTakeover(TouchpadGestureTakeoverWanted());
        if (g_hSwitcher) ApplyThemeToWindow(g_hSwitcher);
        SWS_RegisterHotkeys();
        return 0;
    case WM_SETCURSOR:
        if (g_hoverChevron != 0 || g_isCloseHovered) {
            SetCursor(LoadCursor(NULL, IDC_HAND));
        } else {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
        }
        return TRUE;
    case WM_MOUSEMOVE: {
        TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
        TrackMouseEvent(&tme);

        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        if (GetCapture() == hWnd && !g_isDragging) {
            int dx = abs(x - g_ptLButtonDown.x);
            int dy = abs(y - g_ptLButtonDown.y);
            int dragX = GetSystemMetrics(SM_CXDRAG);
            int dragY = GetSystemMetrics(SM_CYDRAG);
            if (dragX <= 0) dragX = 4;
            if (dragY <= 0) dragY = 4;
            if (dx >= dragX || dy >= dragY) {
                g_isDragging = true;
                if (g_isClosePressed || g_pressedChevron != 0) {
                    g_isClosePressed = false;
                    g_pressedChevron = 0;
                    PaintSwitcherOverlay();
                }
            }
        }

        if (!g_isDragging) {
            UpdateHoverAtPoint(hWnd, x, y, true);
        }
        return 0;
    }
    case WM_MOUSELEAVE: {
        if (GetCapture() != hWnd) {
            if (g_pressedChevron != 0) {
                g_pressedChevron = 0;
                PaintSwitcherOverlay();
            }
            if (g_hoverWnd == hWnd || g_hoverChevron != 0) {
                UpdateHoverFromCursor(g_settings.enableHoverAnimation);
            }
        }
        return 0;
    }
    case WM_SETTINGCHANGE:
        RefreshClientAreaAnimCache();
        if (!AreAnimationsGloballyEnabled()) {
            if (g_animExitActive) {
                CompleteExitAnimation();
                return 0;
            }
            FinishAnimations();
            StopAnimationTicker();
            if (g_isVisible) RecomputeAndReposition();
            InvalidateStaticCache();
            UpdateThumbnailAnimations();
            PaintSwitcher();
        }
        return 0;
    case WM_DISPLAYCHANGE:
        UpdateRefreshRateTiming(true);
        if (hWnd == g_hSwitcher && (g_isVisible || g_isPendingShow)) {
            PostMessageW(hWnd, WM_SWS_CANCEL_INPUT, 0, 0);
        }
        return 0;
    case WM_DPICHANGED:
        UpdateRefreshRateTiming(true);
        // Dismiss safely instead of retaining stale monitor handles/pixel bounds.
        // The next invocation resolves the monitor, DPI, fonts and backdrop anew.
        if (hWnd == g_hSwitcher && (g_isVisible || g_isPendingShow) &&
            (LOWORD(wParam) != g_dpiX || HIWORD(wParam) != g_dpiY)) {
            PostMessageW(hWnd, WM_SWS_CANCEL_INPUT, 0, 0);
        }
        return 0;
    case WM_LBUTTONDOWN: {
        if (!g_isVisible) return 0;
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        g_ptLButtonDown.x = x;
        g_ptLButtonDown.y = y;
        g_isDragging = false;
        g_pressedWindow = nullptr;
        g_ctrlTapPending = false;
        SetCapture(hWnd);

        int cDir = HitTestChevron(hWnd, x, y);
        if (cDir != 0) {
            g_pressedChevron = cDir;
            g_pressedIndex = -1;
            g_isClosePressed = false;
            PaintSwitcherOverlay();
            return 0;
        }
        if (g_isCloseHovered && g_hoverIndex >= 0) {
            g_isClosePressed = true;
            g_pressedIndex = g_hoverIndex;
            if (g_pressedIndex < (int)g_windows.size()) g_pressedWindow = g_windows[g_pressedIndex].hWnd;
            g_pressedChevron = 0;
            PaintSwitcherOverlay();
            return 0;
        }
        g_isClosePressed = false;
        g_pressedChevron = 0;

        if (DockLayoutActive() && DockShowPreview()) {
            POINT pt = { x, y };
            if (PtInRect(&g_rcCentralPreview, pt)) {
                g_pressedIndex = -2;
                if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
                    g_pressedWindow = g_windows[g_selectedIndex].hWnd;
                }
                return 0;
            }
        }
        g_pressedIndex = HitTest(x, y);
        if (g_pressedIndex >= 0) g_pressedWindow = g_windows[g_pressedIndex].hWnd;
        return 0;
    }
    case WM_CAPTURECHANGED: {
        if (GetCapture() != hWnd) {
            g_isDragging = false;
            g_pressedIndex = -1;
            g_pressedWindow = nullptr;
            if (g_pressedChevron != 0 || g_isClosePressed) {
                g_pressedChevron = 0;
                g_isClosePressed = false;
                PaintSwitcherOverlay();
            }
        }
        return 0;
    }
    case WM_CANCELMODE: {
        if (GetCapture() == hWnd) {
            ReleaseCapture();
        }
        g_isDragging = false;
        g_pressedIndex = -1;
        g_pressedWindow = nullptr;
        g_pressedChevron = 0;
        g_isClosePressed = false;
        return 0;
    }
    case WM_LBUTTONUP: {
        if (!g_isVisible) return 0;
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);

        // Snapshot interaction variables BEFORE ReleaseCapture()
        // because ReleaseCapture() synchronously triggers WM_CAPTURECHANGED!
        bool wasDragging = g_isDragging;
        g_isDragging = false;
        int pressedIdx = g_pressedIndex;
        g_pressedIndex = -1;
        HWND pressedWindow = g_pressedWindow;
        g_pressedWindow = nullptr;
        int pressedChev = g_pressedChevron;
        g_pressedChevron = 0;
        bool wasClose = g_isClosePressed;
        g_isClosePressed = false;

        if (GetCapture() == hWnd) {
            ReleaseCapture();
        }

        if (wasClose || pressedChev != 0) {
            PaintSwitcherOverlay();
        }

        // Drop during drag does nothing (Issue #5488)
        if (wasDragging) {
            return 0;
        }

        // Resolve identity after ReleaseCapture: callbacks or live list updates
        // may have reordered entries since the press. Never close a replacement.
        if (pressedIdx >= 0) {
            pressedIdx = -1;
            for (int i = 0; i < (int)g_windows.size(); ++i) {
                if (g_windows[i].hWnd == pressedWindow) { pressedIdx = i; break; }
            }
            if (pressedIdx < 0) return 0;
        }
        if (wasClose) {
            POINT pt = {x, y};
            if (pressedIdx >= 0 && HitTestCloseButton(g_windows[pressedIdx], pt)) CloseSwitcherEntry(pressedIdx);
            return 0;
        }

        // Chevron page navigation
        if (pressedChev != 0) {
            int cDir = HitTestChevron(hWnd, x, y);
            if (cDir == pressedChev) {
                CyclePage(cDir);
            }
            return 0;
        }

        // Dock central preview click
        if (pressedIdx == -2) {
            POINT pt = { x, y };
            if (DockLayoutActive() && DockShowPreview() && PtInRect(&g_rcCentralPreview, pt) &&
                g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() &&
                g_windows[g_selectedIndex].hWnd == pressedWindow) {
                SwitchToSelected();
            }
            return 0;
        }

        // Card / Thumbnail click
        if (pressedIdx >= 0) {
            int releaseIdx = HitTest(x, y);
            if (releaseIdx == pressedIdx) {
                g_selectedIndex = pressedIdx;
                SwitchToSelected();
            }
            return 0;
        }

        return 0;
    }
    case WM_MBUTTONUP: {
        // Middle-click ends (closes) the task under the cursor.
        if (g_isVisible) {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            int idx = g_settings.showThumbnails ? HitTestThumb(x, y) : HitTest(x, y);
            if (idx < 0) idx = HitTest(x, y);
            if (idx >= 0) CloseSwitcherEntry(idx);
        }
        return 0;
    }
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE && (g_isVisible || g_isPendingShow)) {
            Wh_Log(L"SWS TAPTRACE deactivation tick=%llu newActive=%p foreground=%p tips=%d tap=%d selected=%d",
                   (unsigned long long)GetTickCount64(), (HWND)lParam,
                   GetForegroundWindow(), s_rawGestureTips, s_rawTwoFingerTapActive,
                   g_selectedIndex);
            if (g_animExitActive || g_isHidingSwitcher || g_recoveringShellFocus) return 0;
            HWND hNewActive = (HWND)lParam;
            HWND hCheck = hNewActive ? hNewActive : GetForegroundWindow();
            if (RawTouchpadShellSuppressionActive() && hCheck && IsNativeSwitcherWindow(hCheck)) {
                ShellFocusRecoveryScope recovery;
                ShowWindow(hCheck, SW_HIDE);
                BringWindowToTop(g_hSwitcher);
                SetForegroundWindow(g_hSwitcher);
                return 0;
            }
            if (RawTouchpadShellSuppressionActive() && hCheck && IsTaskViewWindow(hCheck)) {
                // Task View can take focus after physical lift, before its SHOW
                // event is delivered. Preserve only this identified shell race.
                Wh_Log(L"SWS: retaining raw session for Task View recovery (tips=%d)", s_rawGestureTips);
                return 0; // The WinEvent handler dismisses this shell interference.
            }
            if (PreserveRawStrokeOnFocusLoss()) {
                Wh_Log(L"SWS: retaining live raw stroke after deactivation (foreground=%p, tips=%d)",
                       hCheck, s_rawGestureTips);
                return 0;
            }
            if (CombinedRawReleasePending()) {
                Wh_Log(L"SWS: retaining combined Alt/raw session after deactivation (foreground=%p)",
                       hCheck);
                return 0;
            }
            if ((s_rawSessionOwned || g_isSticky) && hCheck && !IsSwitcherWindow(hCheck)) {
                StartExitAnimation(false); // Intentional focus change: cancel with the normal close animation.
                return 0;
            }
            if (g_isTouchpadGestureActive) {
                return 0; // Touchpad gesture in progress; preserve switcher visibility
            }
            if (hNewActive == NULL) {
                HWND hFg = GetForegroundWindow();
                if (hFg == g_hSwitcher || hFg == g_hCloseBtnWnd || (hFg && IsNativeSwitcherWindow(hFg))) {
                    return 0;
                }
                if (GetTickCount64() - s_lastTouchpadScrollTick < 1200) {
                    return 0;
                }
            }
            if (!IsSwitcherWindow(hNewActive)) {
                StartExitAnimation(false);
            }
            return 0;
        }
        break;
    case WM_KILLFOCUS:
        if (g_isVisible || g_isPendingShow) {
            Wh_Log(L"SWS TAPTRACE focus loss tick=%llu newFocus=%p foreground=%p tips=%d tap=%d selected=%d",
                   (unsigned long long)GetTickCount64(), (HWND)wParam,
                   GetForegroundWindow(), s_rawGestureTips, s_rawTwoFingerTapActive,
                   g_selectedIndex);
            if (g_animExitActive || g_isHidingSwitcher || g_recoveringShellFocus) return 0;
            HWND hNewFocus = (HWND)wParam;
            HWND hCheck = hNewFocus ? hNewFocus : GetForegroundWindow();
            if (RawTouchpadShellSuppressionActive() && hCheck && IsNativeSwitcherWindow(hCheck)) {
                ShellFocusRecoveryScope recovery;
                ShowWindow(hCheck, SW_HIDE);
                BringWindowToTop(g_hSwitcher);
                SetForegroundWindow(g_hSwitcher);
                return 0;
            }
            if (RawTouchpadShellSuppressionActive() && hCheck && IsTaskViewWindow(hCheck)) {
                Wh_Log(L"SWS: retaining raw session for Task View recovery (tips=%d)", s_rawGestureTips);
                return 0;
            }
            if (PreserveRawStrokeOnFocusLoss()) {
                Wh_Log(L"SWS: retaining live raw stroke after focus loss (foreground=%p, tips=%d)",
                       hCheck, s_rawGestureTips);
                return 0;
            }
            if (CombinedRawReleasePending()) {
                Wh_Log(L"SWS: retaining combined Alt/raw session after focus loss (foreground=%p)",
                       hCheck);
                return 0;
            }
            if ((s_rawSessionOwned || g_isSticky) && hCheck && !IsSwitcherWindow(hCheck)) {
                StartExitAnimation(false);
                return 0;
            }
            if (g_isTouchpadGestureActive) {
                return 0; // Touchpad gesture in progress; preserve switcher visibility
            }
            if (hNewFocus == NULL) {
                HWND hFg = GetForegroundWindow();
                if (hFg == g_hSwitcher || hFg == g_hCloseBtnWnd || (hFg && IsNativeSwitcherWindow(hFg))) {
                    return 0;
                }
                if (GetTickCount64() - s_lastTouchpadScrollTick < 1200) {
                    return 0;
                }
            }
            if (!IsSwitcherWindow(hNewFocus)) {
                StartExitAnimation(false);
            }
            return 0;
        }
        break;
    case WM_ERASEBKGND: return 1;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_CLOSE || (wParam & 0xFFF0) == SC_KEYMENU) {
            return 0;
        }
        break;
    case WM_CLOSE: return 0;
    case WM_DESTROY:
        if (hWnd != g_hSwitcher) return 0;
        FinishAnimations();
        StopAnimationTicker();
        FreeCachedBuffers();
        if (s_hWinEventHook) {
            UnhookWinEvent(s_hWinEventHook);
            s_hWinEventHook = NULL;
        }
        if (s_hForegroundEventHook) {
            UnhookWinEvent(s_hForegroundEventHook);
            s_hForegroundEventHook = NULL;
        }
        UnregisterThumbnails();
        return 0;
    }

    if (g_shellHookMsg && uMsg == g_shellHookMsg) {
        int code = (int)(wParam & 0x7FFF);
        if (code == HSHELL_WINDOWACTIVATED || code == 4) {
            HWND hAct = (HWND)lParam;
            if (hAct && !IsSwitcherWindow(hAct)) {
                UpdateMruWindow(hAct);
                if (g_isVisible || g_isPendingShow) {
                    AddWindowEntry(hAct);
                }
            }
        } else if (code == HSHELL_WINDOWDESTROYED) {
            HWND hS = (HWND)lParam;
            if (IsWindow(hS)) {
                // Stale/misfired notification: the window is still alive (e.g. it
                // survived a close attempt by showing a modal dialog). Do NOT touch
                // the MRU list or the entry — it must stay exactly where it was.
                return 0;
            }
            RemoveMruWindow(hS);
            auto it = std::find(s_pendingCloseWindows.begin(), s_pendingCloseWindows.end(), hS);
            if (it != s_pendingCloseWindows.end()) {
                s_pendingCloseWindows.erase(it);
            }
            if (g_isVisible || g_isPendingShow) {
                RemoveWindowEntryByHwnd(hS);
            }
        } else if (code == HSHELL_WINDOWCREATED || code == HSHELL_REDRAW) {
            HWND hTarget = (HWND)lParam;
            if (hTarget && (g_isVisible || g_isPendingShow)) {
                AddWindowEntry(hTarget);
            }
        }
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// Hotkey Helpers

static HANDLE g_hHotkeyMutex = NULL;
static bool g_registeredHotkeys[SWS_HOTKEY_ALTBACKTICK_UK + 1] = {};

static void SWS_RegisterHotkeys() {
    if (!g_hSwitcher) return;
    bool wantAltBacktick = (wcscmp(g_settings.altBacktickBehavior, L"none") != 0);
    auto registerOne = [](int id, UINT mods, UINT vk) {
        if (!g_registeredHotkeys[id]) {
            g_registeredHotkeys[id] = RegisterHotKey(g_hSwitcher, id, mods, vk) != FALSE;
        }
        return g_registeredHotkeys[id];
    };
    bool r1 = registerOne(SWS_HOTKEY_ALTTAB, MOD_ALT, VK_TAB);
    bool r2 = registerOne(SWS_HOTKEY_ALTSHIFTTAB, MOD_ALT | MOD_SHIFT, VK_TAB);
    bool r3 = registerOne(SWS_HOTKEY_ALTCTRLTAB, MOD_ALT | MOD_CONTROL, VK_TAB);
    bool r4 = registerOne(SWS_HOTKEY_ALTSHIFTCTRLTAB, MOD_ALT | MOD_SHIFT | MOD_CONTROL, VK_TAB);
    bool r5us = !wantAltBacktick || registerOne(SWS_HOTKEY_ALTBACKTICK, MOD_ALT, VK_OEM_3);
    bool r5uk = !wantAltBacktick || registerOne(SWS_HOTKEY_ALTBACKTICK_UK, MOD_ALT, VK_OEM_8);
    bool r6 = registerOne(SWS_HOTKEY_WINALTTAB, MOD_ALT | MOD_WIN, VK_TAB);
    bool r7 = registerOne(SWS_HOTKEY_WINALTSHIFTTAB, MOD_ALT | MOD_SHIFT | MOD_WIN, VK_TAB);
    g_hotkeysRegistered = r1 && r2 && r3 && r4;
    if (r1) {
        if (!g_hHotkeyMutex) {
            g_hHotkeyMutex = CreateMutexW(NULL, TRUE, L"Windhawk_SWS_HotkeyMutex");
        }
    }
    if (g_hotkeysRegistered && r5us && r5uk && r6 && r7) {
        KillTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID);
    } else {
        // Keep successful registrations. An optional shortcut conflict must not
        // disable ordinary Alt+Tab; retries touch only missing registrations.
        SetTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID, SWS_HOTKEY_RETRY_INTERVAL, NULL);
    }
}
static void SWS_UnregisterHotkeys() {
    KillTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID);
    for (int id = 1; id <= SWS_HOTKEY_ALTBACKTICK_UK; ++id) {
        if (g_registeredHotkeys[id] && g_hSwitcher) UnregisterHotKey(g_hSwitcher, id);
        g_registeredHotkeys[id] = false;
    }
    g_hotkeysRegistered = false;
    if (g_hHotkeyMutex) {
        ReleaseMutex(g_hHotkeyMutex);
        CloseHandle(g_hHotkeyMutex);
        g_hHotkeyMutex = NULL;
    }
    Wh_Log(L"Hotkeys unregistered");
}


// Settings

template <size_t N>
static void LoadStringSetting(LPCWSTR settingName, WCHAR (&dest)[N], LPCWSTR defaultVal) {
    LPCWSTR v = Wh_GetStringSetting(settingName);
    wcsncpy_s(dest, (v && *v) ? v : defaultVal, _TRUNCATE);
    Wh_FreeStringSetting(v);
}

static bool LoadBoolSetting(LPCWSTR settingName, bool defaultVal) {
    PCWSTR s = Wh_GetStringSetting(settingName);
    if (!s || !*s) {
        if (s) Wh_FreeStringSetting(s);
        return defaultVal;
    }
    Wh_FreeStringSetting(s);
    return Wh_GetIntSetting(settingName) != 0;
}

static bool LoadAutoBoolSetting(LPCWSTR settingName, bool autoVal) {
    PCWSTR s = Wh_GetStringSetting(settingName);
    if (!s || !*s || _wcsicmp(s, L"auto") == 0) {
        if (s) Wh_FreeStringSetting(s);
        return autoVal;
    }
    bool result = (_wcsicmp(s, L"true") == 0 || wcscmp(s, L"1") == 0);
    Wh_FreeStringSetting(s);
    return result;
}

static int LoadIntSetting(LPCWSTR settingName, int defaultVal) {
    PCWSTR s = Wh_GetStringSetting(settingName);
    if (!s || !*s) {
        if (s) Wh_FreeStringSetting(s);
        return defaultVal;
    }
    Wh_FreeStringSetting(s);
    return Wh_GetIntSetting(settingName);
}

static void LoadSettings() {
    LPCWSTR v;
    LoadStringSetting(L"Style.theme", g_settings.theme, L"auto");
    if (wcscmp(g_settings.theme, L"auto") == 0) {
        wcsncpy_s(g_settings.theme, IsWin11OrGreater() ? L"mica" : L"backdrop", _TRUNCATE);
    }
    if (wcscmp(g_settings.theme, L"mica") == 0 && !IsWin11OrGreater()) {
        wcsncpy_s(g_settings.theme, L"backdrop", _TRUNCATE);
    }
    DetectSystemDwmCornerRadius();
    LoadStringSetting(L"Style.colorScheme", g_settings.colorScheme, L"system");
    LoadStringSetting(L"Style.backdropBlurEffect", g_settings.backdropBlurEffect, L"off");
    if (wcscmp(g_settings.backdropBlurEffect, L"off") != 0 &&
        wcscmp(g_settings.backdropBlurEffect, L"acrylic") != 0 &&
        wcscmp(g_settings.backdropBlurEffect, L"acrylicWallpaper") != 0) {
        wcsncpy_s(g_settings.backdropBlurEffect, L"off", _TRUNCATE);
    }
    g_settings.backdropBlurOpacity = Wh_GetIntSetting(L"Style.backdropBlurOpacity");
    if (g_settings.backdropBlurOpacity < 0) g_settings.backdropBlurOpacity = 0;
    if (g_settings.backdropBlurOpacity > 100) g_settings.backdropBlurOpacity = 100;
    LoadStringSetting(L"Appearance.Corners.cornerPreference", g_settings.cornerPreference, L"default");
    if (wcscmp(g_settings.cornerPreference, L"auto") == 0) {
        wcsncpy_s(g_settings.cornerPreference, L"default", _TRUNCATE);
    }
    g_settings.customCornerRadius = Wh_GetIntSetting(L"Appearance.Corners.customCornerRadius");
    if (g_settings.customCornerRadius < 0) g_settings.customCornerRadius = 0;
    if (g_settings.customCornerRadius > 32) g_settings.customCornerRadius = 32;
    g_settings.taskRoundedCorners = LoadAutoBoolSetting(L"Appearance.Corners.taskRoundedCorners", g_systemDwmRadius > 0);
    g_settings.roundThumbnailCorners = LoadAutoBoolSetting(L"Appearance.Corners.roundThumbnailCorners", g_systemDwmRadius > 0);
    g_settings.roundGroupIndicator = LoadAutoBoolSetting(L"Appearance.Corners.roundGroupIndicator", g_systemDwmRadius > 0);
    g_settings.roundBadgeIconBackground = LoadAutoBoolSetting(L"Appearance.Corners.roundBadgeIconBackground", g_systemDwmRadius > 0);
    LoadStringSetting(L"Accessibility.scrollWheelBehavior", g_settings.scrollWheelBehavior, L"never");
    LoadStringSetting(L"Accessibility.scrollWheelAction", g_settings.scrollWheelAction, L"selection");
    LoadStringSetting(L"Accessibility.scrollSecondaryAction", g_settings.scrollSecondaryAction, L"page");
    LoadStringSetting(L"Accessibility.scrollSecondaryModifier", g_settings.scrollSecondaryModifier, L"shift");
    LoadStringSetting(L"Appearance.Orientation.taskListOrientation", g_settings.taskListOrientation, L"horizontal");
    LoadStringSetting(L"Appearance.Orientation.headerContentOrientation", g_settings.headerContentOrientation, L"horizontal");
    if (wcscmp(g_settings.headerContentOrientation, L"horizontal") != 0 &&
        wcscmp(g_settings.headerContentOrientation, L"vertical") != 0) {
        wcsncpy_s(g_settings.headerContentOrientation, L"horizontal", _TRUNCATE);
    }
    
    LoadStringSetting(L"Appearance.Position.switcherPosition", g_settings.switcherPosition, L"center");
    if (wcscmp(g_settings.switcherPosition, L"topLeft") != 0 &&
        wcscmp(g_settings.switcherPosition, L"topCenter") != 0 &&
        wcscmp(g_settings.switcherPosition, L"topRight") != 0 &&
        wcscmp(g_settings.switcherPosition, L"centerLeft") != 0 &&
        wcscmp(g_settings.switcherPosition, L"center") != 0 &&
        wcscmp(g_settings.switcherPosition, L"centerRight") != 0 &&
        wcscmp(g_settings.switcherPosition, L"bottomLeft") != 0 &&
        wcscmp(g_settings.switcherPosition, L"bottomCenter") != 0 &&
        wcscmp(g_settings.switcherPosition, L"bottomRight") != 0) {
        wcsncpy_s(g_settings.switcherPosition, L"center", _TRUNCATE);
    }
    g_settings.switcherPositionMargin = Wh_GetIntSetting(L"Appearance.Position.switcherPositionMargin");
    if (g_settings.switcherPositionMargin < 0) g_settings.switcherPositionMargin = 0;
    LoadStringSetting(L"Appearance.HeaderContent.iconSize", g_settings.iconSize, L"small");
    if (wcscmp(g_settings.iconSize, L"small") != 0 &&
        wcscmp(g_settings.iconSize, L"medium") != 0 &&
        wcscmp(g_settings.iconSize, L"large") != 0 &&
        wcscmp(g_settings.iconSize, L"xlarge") != 0) {
        wcsncpy_s(g_settings.iconSize, L"small", _TRUNCATE);
    }
    LoadStringSetting(L"Appearance.Thumbnails.thumbnailPosition", g_settings.thumbnailPosition, L"bottom");
    if (wcscmp(g_settings.thumbnailPosition, L"bottom") != 0 &&
        wcscmp(g_settings.thumbnailPosition, L"top") != 0 &&
        wcscmp(g_settings.thumbnailPosition, L"left") != 0 &&
        wcscmp(g_settings.thumbnailPosition, L"right") != 0) {
        wcsncpy_s(g_settings.thumbnailPosition, L"bottom", _TRUNCATE);
    }
    LoadStringSetting(L"Appearance.Thumbnails.thumbnailAlignment", g_settings.thumbnailAlignment, L"left");
    if (wcscmp(g_settings.thumbnailAlignment, L"left") != 0 &&
        wcscmp(g_settings.thumbnailAlignment, L"centered") != 0 &&
        wcscmp(g_settings.thumbnailAlignment, L"right") != 0) {
        wcsncpy_s(g_settings.thumbnailAlignment, L"left", _TRUNCATE);
    }
    LoadStringSetting(L"Accessibility.backwardShortcut", g_settings.backwardShortcut, L"altShiftTab");
    if (wcscmp(g_settings.backwardShortcut, L"altBacktick") == 0) {
        // Alt+Backtick is no longer an entry of this setting: it is configured on its own
        // (Alt+Backtick Behavior), which cycles backward by default, so a saved selection of
        // the removed entry folds into the default shortcut and keeps behaving the same.
        wcsncpy_s(g_settings.backwardShortcut, L"altShiftTab", _TRUNCATE);
    } else if (wcscmp(g_settings.backwardShortcut, L"altShiftTab") != 0 &&
               wcscmp(g_settings.backwardShortcut, L"altShift") != 0) {
        wcsncpy_s(g_settings.backwardShortcut, L"altShiftTab", _TRUNCATE);
    }
    
    LoadStringSetting(L"Accessibility.altBacktickBehavior", g_settings.altBacktickBehavior, L"backward");
    if (wcscmp(g_settings.altBacktickBehavior, L"none") != 0 &&
        wcscmp(g_settings.altBacktickBehavior, L"backward") != 0 &&
        wcscmp(g_settings.altBacktickBehavior, L"sameApp") != 0) {
        wcsncpy_s(g_settings.altBacktickBehavior, L"backward", _TRUNCATE);
    }

    LoadStringSetting(L"Accessibility.virtualDesktopBehavior", g_settings.virtualDesktopBehavior, L"allDesktops");
    if (wcscmp(g_settings.virtualDesktopBehavior, L"currentOnly") != 0 &&
        wcscmp(g_settings.virtualDesktopBehavior, L"allDesktops") != 0) {
        wcsncpy_s(g_settings.virtualDesktopBehavior, L"allDesktops", _TRUNCATE);
    }

    g_settings.rowHeight = Wh_GetIntSetting(L"Dimensions.rowHeight");
    if (g_settings.rowHeight <= 0) g_settings.rowHeight = 230;
    g_settings.rowWidth = Wh_GetIntSetting(L"Dimensions.rowWidth");
    if (g_settings.rowWidth < 0) g_settings.rowWidth = 0;
    g_settings.stretchThumbnailsToTaskWidth = Wh_GetIntSetting(L"Dimensions.stretchThumbnailsToTaskWidth");
    g_settings.autoFitTasks = Wh_GetIntSetting(L"Dimensions.autoFitTasks");
    g_settings.defaultSwitcherPadding = LoadIntSetting(L"Dimensions.defaultSwitcherPadding", LoadIntSetting(L"Dimensions.switcherPadding", 20));
    if (g_settings.defaultSwitcherPadding < 0) g_settings.defaultSwitcherPadding = 20;
    g_settings.switcherPadding = g_settings.defaultSwitcherPadding;
    g_settings.entryPadding = LoadIntSetting(L"Dimensions.entryPadding", 16);
    if (g_settings.entryPadding < 0) g_settings.entryPadding = 16;
    g_settings.showThumbnails = Wh_GetIntSetting(L"Appearance.Thumbnails.showThumbnails");
    g_settings.showCloseButton = Wh_GetIntSetting(L"Appearance.Thumbnails.showCloseButton");
    g_settings.showCloseButtonBackground = LoadAutoBoolSetting(L"Appearance.Thumbnails.showCloseButtonBackground", IsWin11OrGreater());
    g_settings.showOverflowIndicator = Wh_GetIntSetting(L"Appearance.showOverflowIndicator");
    g_settings.showHoverBorder = Wh_GetIntSetting(L"Appearance.Thumbnails.showHoverBorder");
    LoadStringSetting(L"Appearance.Thumbnails.thumbnailHoverEffect", g_settings.thumbnailHoverEffect, L"auto");
    if (wcscmp(g_settings.thumbnailHoverEffect, L"auto") == 0) {
        wcsncpy_s(g_settings.thumbnailHoverEffect, IsWin11OrGreater() ? L"zoom" : L"border", _TRUNCATE);
    } else if (wcscmp(g_settings.thumbnailHoverEffect, L"border") != 0 &&
               wcscmp(g_settings.thumbnailHoverEffect, L"zoom") != 0) {
        wcsncpy_s(g_settings.thumbnailHoverEffect, IsWin11OrGreater() ? L"zoom" : L"border", _TRUNCATE);
    }
    g_settings.showThumbnailShadow = LoadAutoBoolSetting(L"Appearance.Thumbnails.showThumbnailShadow", IsWin11OrGreater());
    g_settings.showTitle = Wh_GetIntSetting(L"Appearance.HeaderContent.showTitle");
    g_settings.showIcon = Wh_GetIntSetting(L"Appearance.HeaderContent.showIcon");
    if (!g_settings.showThumbnails && !g_settings.showTitle && !g_settings.showIcon) {
        g_settings.showTitle = true;
    }

    // Animations
    g_settings.enableAnimations = LoadAutoBoolSetting(L"Appearance.Animations.enableAnimations", IsWin11OrGreater());
    g_settings.enableEntranceAnimation = LoadBoolSetting(L"Appearance.Animations.enableEntranceAnimation", true);
    g_settings.enableSelectionAnimation = LoadBoolSetting(L"Appearance.Animations.enableSelectionAnimation", true);
    g_settings.enableScrollAnimation = LoadBoolSetting(L"Appearance.Animations.enableScrollAnimation", true);
    g_settings.enableHoverAnimation = LoadBoolSetting(L"Appearance.Animations.enableHoverAnimation", true);

    g_settings.maxWidthPercent = Wh_GetIntSetting(L"Dimensions.maxWidthPercent");
    if (g_settings.maxWidthPercent <= 0 || g_settings.maxWidthPercent > 100) g_settings.maxWidthPercent = 80;
    g_settings.maxHeightPercent = Wh_GetIntSetting(L"Dimensions.maxHeightPercent");
    if (g_settings.maxHeightPercent <= 0 || g_settings.maxHeightPercent > 100) g_settings.maxHeightPercent = 80;

    g_settings.showDelay = Wh_GetIntSetting(L"Accessibility.showDelay");
    if (g_settings.showDelay < 0) g_settings.showDelay = 0;
    g_settings.perMonitorWindows = Wh_GetIntSetting(L"Accessibility.perMonitorWindows");
    g_settings.reverseScrollDirection = Wh_GetIntSetting(L"Accessibility.reverseScrollDirection");
    g_settings.showApplications = Wh_GetIntSetting(L"Grouping.showApplications");
    g_settings.restoreAllWindows = Wh_GetIntSetting(L"Grouping.restoreAllWindows");
    g_settings.hideMinimizedWindows = Wh_GetIntSetting(L"Accessibility.hideMinimizedWindows");
    g_settings.sortMinimizedWindowsToEnd = Wh_GetIntSetting(L"Accessibility.sortMinimizedWindowsToEnd");
    g_settings.showMinimizedIndicator = Wh_GetIntSetting(L"Accessibility.showMinimizedIndicator") != 0;
    LoadStringSetting(L"Accessibility.minimizedIndicatorStyle", g_settings.minimizedIndicatorStyle, L"dimIcon");
    if (wcscmp(g_settings.minimizedIndicatorStyle, L"dimIcon") != 0 &&
        wcscmp(g_settings.minimizedIndicatorStyle, L"dot") != 0 &&
        wcscmp(g_settings.minimizedIndicatorStyle, L"badge") != 0 &&
        wcscmp(g_settings.minimizedIndicatorStyle, L"dimAndDot") != 0 &&
        wcscmp(g_settings.minimizedIndicatorStyle, L"dimAndBadge") != 0) {
        wcsncpy_s(g_settings.minimizedIndicatorStyle, L"dimIcon", _TRUNCATE);
    }
    g_settings.minimizedIconOpacity = Wh_GetIntSetting(L"Accessibility.minimizedIconOpacity");
    if (g_settings.minimizedIconOpacity < 20 || g_settings.minimizedIconOpacity > 90) g_settings.minimizedIconOpacity = 55;
    LoadStringSetting(L"Grouping.showTitles", g_settings.showTitles, L"windowTitle");
    if (wcscmp(g_settings.showTitles, L"windowTitle") != 0 &&
        wcscmp(g_settings.showTitles, L"appName") != 0 &&
        wcscmp(g_settings.showTitles, L"appNameWindowTitle") != 0) {
        wcsncpy_s(g_settings.showTitles, L"windowTitle", _TRUNCATE);
    }
    g_settings.centerTaskContent = Wh_GetIntSetting(L"Appearance.HeaderContent.centerTaskContent");

    // Master layout setting
    LoadStringSetting(L"Appearance.Layout.switcherLayout", g_settings.switcherLayout, L"default");
    if (wcscmp(g_settings.switcherLayout, L"default") != 0 &&
        wcscmp(g_settings.switcherLayout, L"badge") != 0 &&
        wcscmp(g_settings.switcherLayout, L"dock") != 0) {
        wcsncpy_s(g_settings.switcherLayout, L"default", _TRUNCATE);
    }
    // Migrate only absent keys, never an explicitly chosen Default layout.
    PCWSTR savedLayout = Wh_GetStringSetting(L"Appearance.Layout.switcherLayout");
    bool layoutMissing = !savedLayout || !*savedLayout;
    if (savedLayout) Wh_FreeStringSetting(savedLayout);
    if (layoutMissing) {
        if (Wh_GetIntSetting(L"Appearance.DockLayout.enableDockLayout")) {
            wcsncpy_s(g_settings.switcherLayout, L"dock", _TRUNCATE);
        } else if (Wh_GetIntSetting(L"Appearance.BadgeLayout.enableBadgeLayout")) {
            wcsncpy_s(g_settings.switcherLayout, L"badge", _TRUNCATE);
        }
    }

    // Badge layout settings
    LoadStringSetting(L"Appearance.BadgeLayout.badgeIconPosition", g_settings.badgeIconPosition, L"bottomCenter");
    if (wcscmp(g_settings.badgeIconPosition, L"topLeft") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"topCenter") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"topRight") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"centerLeft") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"center") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"centerRight") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"bottomLeft") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"bottomCenter") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"bottomRight") != 0) {
        wcsncpy_s(g_settings.badgeIconPosition, L"bottomCenter", _TRUNCATE);
    }
    LoadStringSetting(L"Appearance.BadgeLayout.badgeTitlePosition", g_settings.badgeTitlePosition, L"bottom");
    if (wcscmp(g_settings.badgeTitlePosition, L"top") != 0 &&
        wcscmp(g_settings.badgeTitlePosition, L"bottom") != 0) {
        wcsncpy_s(g_settings.badgeTitlePosition, L"bottom", _TRUNCATE);
    }
    LoadStringSetting(L"Appearance.BadgeLayout.badgeIconSize", g_settings.badgeIconSize, L"medium");
    if (wcscmp(g_settings.badgeIconSize, L"small") != 0 &&
        wcscmp(g_settings.badgeIconSize, L"medium") != 0 &&
        wcscmp(g_settings.badgeIconSize, L"large") != 0 &&
        wcscmp(g_settings.badgeIconSize, L"xlarge") != 0) {
        wcsncpy_s(g_settings.badgeIconSize, L"medium", _TRUNCATE);
    }
    g_settings.showBadgeIconBackground = LoadAutoBoolSetting(L"Appearance.BadgeLayout.showBadgeIconBackground", !IsWin11OrGreater());
    g_settings.showBadgeIconBackgroundShadow = LoadAutoBoolSetting(L"Appearance.BadgeLayout.showBadgeIconBackgroundShadow", IsWin11OrGreater());
    g_settings.badgeIconPadding = Wh_GetIntSetting(L"Appearance.BadgeLayout.badgeIconPadding");
    g_settings.badgeIconOffsetX = Wh_GetIntSetting(L"Appearance.BadgeLayout.badgeIconOffsetX");
    g_settings.badgeIconOffsetY = Wh_GetIntSetting(L"Appearance.BadgeLayout.badgeIconOffsetY");
    g_settings.badgeSwitcherPadding = LoadIntSetting(L"Appearance.BadgeLayout.badgeSwitcherPadding", LoadIntSetting(L"Dimensions.switcherPadding", 20));
    if (g_settings.badgeSwitcherPadding < 0) g_settings.badgeSwitcherPadding = 20;

    // Dock layout settings
    LoadStringSetting(L"Appearance.DockLayout.dockIconPosition", g_settings.dockIconPosition, L"top");
    if (wcscmp(g_settings.dockIconPosition, L"top") != 0 &&
        wcscmp(g_settings.dockIconPosition, L"bottom") != 0) {
        wcsncpy_s(g_settings.dockIconPosition, L"top", _TRUNCATE);
    }
    g_settings.dockShowPreview = Wh_GetIntSetting(L"Appearance.DockLayout.dockShowPreview");
    g_settings.dockPreviewHeight = Wh_GetIntSetting(L"Appearance.DockLayout.dockPreviewHeight");
    if (g_settings.dockPreviewHeight <= 0) g_settings.dockPreviewHeight = 280;
    PCWSTR dockIconSizeStr = Wh_GetStringSetting(L"Appearance.DockLayout.dockIconSize");
    if (dockIconSizeStr) {
        g_settings.dockIconSize = _wtoi(dockIconSizeStr);
        Wh_FreeStringSetting(dockIconSizeStr);
    } else {
        g_settings.dockIconSize = Wh_GetIntSetting(L"Appearance.DockLayout.dockIconSize");
    }
    if (g_settings.dockIconSize <= 0) g_settings.dockIconSize = 48;
    g_settings.dockIconSpacing = Wh_GetIntSetting(L"Appearance.DockLayout.dockIconSpacing");
    if (g_settings.dockIconSpacing < 0) g_settings.dockIconSpacing = 8;
    LoadStringSetting(L"Appearance.DockLayout.dockHighlightStyle", g_settings.dockHighlightStyle, L"fillOnly");
    if (wcscmp(g_settings.dockHighlightStyle, L"fillOnly") != 0 &&
        wcscmp(g_settings.dockHighlightStyle, L"border") != 0 &&
        wcscmp(g_settings.dockHighlightStyle, L"fillAndBorder") != 0) {
        wcsncpy_s(g_settings.dockHighlightStyle, L"fillOnly", _TRUNCATE);
    }
    PCWSTR dockMaxIconsStr = Wh_GetStringSetting(L"Appearance.DockLayout.dockMaxVisibleIcons");
    if (dockMaxIconsStr) {
        g_settings.dockMaxVisibleIcons = _wtoi(dockMaxIconsStr);
        Wh_FreeStringSetting(dockMaxIconsStr);
    } else {
        g_settings.dockMaxVisibleIcons = Wh_GetIntSetting(L"Appearance.DockLayout.dockMaxVisibleIcons");
    }
    if (g_settings.dockMaxVisibleIcons < 0) g_settings.dockMaxVisibleIcons = 7;
    g_settings.dockSwitcherPadding = LoadIntSetting(L"Appearance.DockLayout.dockSwitcherPadding", 11);
    if (g_settings.dockSwitcherPadding < 0) g_settings.dockSwitcherPadding = 11;

    LoadStringSetting(L"Appearance.DockLayout.dockCloseButtonPosition", g_settings.dockCloseButtonPosition, L"topRight");
    if (wcscmp(g_settings.dockCloseButtonPosition, L"previewTopRight") == 0 ||
        wcscmp(g_settings.dockCloseButtonPosition, L"iconStrip") == 0) {
        wcsncpy_s(g_settings.dockCloseButtonPosition, L"topRight", _TRUNCATE);
    } else if (wcscmp(g_settings.dockCloseButtonPosition, L"previewTopLeft") == 0) {
        wcsncpy_s(g_settings.dockCloseButtonPosition, L"topLeft", _TRUNCATE);
    } else if (wcscmp(g_settings.dockCloseButtonPosition, L"topRight") != 0 &&
               wcscmp(g_settings.dockCloseButtonPosition, L"topLeft") != 0 &&
               wcscmp(g_settings.dockCloseButtonPosition, L"bottomRight") != 0 &&
               wcscmp(g_settings.dockCloseButtonPosition, L"bottomLeft") != 0 &&
               wcscmp(g_settings.dockCloseButtonPosition, L"top") != 0 &&
               wcscmp(g_settings.dockCloseButtonPosition, L"bottom") != 0 &&
               wcscmp(g_settings.dockCloseButtonPosition, L"left") != 0 &&
               wcscmp(g_settings.dockCloseButtonPosition, L"right") != 0 &&
               wcscmp(g_settings.dockCloseButtonPosition, L"hidden") != 0) {
        wcsncpy_s(g_settings.dockCloseButtonPosition, L"topRight", _TRUNCATE);
    }

    LoadStringSetting(L"Appearance.DockLayout.dockGroupIndicatorPosition", g_settings.dockGroupIndicatorPosition, L"bottomRight");
    if (wcscmp(g_settings.dockGroupIndicatorPosition, L"onIconBadge") == 0) {
        wcsncpy_s(g_settings.dockGroupIndicatorPosition, L"bottomRight", _TRUNCATE);
    } else if (wcscmp(g_settings.dockGroupIndicatorPosition, L"belowIcons") == 0) {
        wcsncpy_s(g_settings.dockGroupIndicatorPosition, L"bottom", _TRUNCATE);
    } else if (wcscmp(g_settings.dockGroupIndicatorPosition, L"aboveIcons") == 0) {
        wcsncpy_s(g_settings.dockGroupIndicatorPosition, L"top", _TRUNCATE);
    } else if (wcscmp(g_settings.dockGroupIndicatorPosition, L"insidePreview") == 0) {
        wcsncpy_s(g_settings.dockGroupIndicatorPosition, L"topRight", _TRUNCATE);
    } else if (wcscmp(g_settings.dockGroupIndicatorPosition, L"bottomRight") != 0 &&
               wcscmp(g_settings.dockGroupIndicatorPosition, L"bottomLeft") != 0 &&
               wcscmp(g_settings.dockGroupIndicatorPosition, L"topRight") != 0 &&
               wcscmp(g_settings.dockGroupIndicatorPosition, L"topLeft") != 0 &&
               wcscmp(g_settings.dockGroupIndicatorPosition, L"top") != 0 &&
               wcscmp(g_settings.dockGroupIndicatorPosition, L"bottom") != 0 &&
               wcscmp(g_settings.dockGroupIndicatorPosition, L"left") != 0 &&
               wcscmp(g_settings.dockGroupIndicatorPosition, L"right") != 0 &&
               wcscmp(g_settings.dockGroupIndicatorPosition, L"hidden") != 0) {
        wcsncpy_s(g_settings.dockGroupIndicatorPosition, L"bottomRight", _TRUNCATE);
    }

    // Grouped indicator
    g_settings.showGroupIndicator = Wh_GetIntSetting(L"Grouping.showGroupIndicator");
    g_settings.showGroupIndicatorShadow = LoadAutoBoolSetting(L"Grouping.showGroupIndicatorShadow", IsWin11OrGreater());
    LoadStringSetting(L"Grouping.groupCloseBehavior", g_settings.groupCloseBehavior, L"closeRecent");
    if (wcscmp(g_settings.groupCloseBehavior, L"closeAll") != 0 &&
        wcscmp(g_settings.groupCloseBehavior, L"closeRecent") != 0) {
        wcsncpy_s(g_settings.groupCloseBehavior, L"closeRecent", _TRUNCATE);
    }

    // Global theme settings (apply to both light and dark)
    LoadStringSetting(L"Style.highlightStyle", g_settings.highlightStyle, L"auto");
    if (wcscmp(g_settings.highlightStyle, L"auto") == 0) {
        wcsncpy_s(g_settings.highlightStyle, IsWin11OrGreater() ? L"fillOnly" : L"border", _TRUNCATE);
    } else if (wcscmp(g_settings.highlightStyle, L"border") != 0 &&
               wcscmp(g_settings.highlightStyle, L"fillAndBorder") != 0 &&
               wcscmp(g_settings.highlightStyle, L"fillOnly") != 0) {
        wcsncpy_s(g_settings.highlightStyle, IsWin11OrGreater() ? L"fillOnly" : L"border", _TRUNCATE);
    }
    g_settings.opacity = LoadIntSetting(L"Style.opacity", 65);
    if (g_settings.opacity < 0) g_settings.opacity = 0;
    if (g_settings.opacity > 100) g_settings.opacity = 100;
    g_settings.showSwitcherBorder = LoadBoolSetting(L"Style.showSwitcherBorder", true);

    // Dark Mode color settings
    LoadStringSetting(L"Style.DarkMode.borderColorMode", g_settings.borderColorModeDark, L"default");
    LoadStringSetting(L"Style.DarkMode.highlightFillColorMode", g_settings.highlightFillColorModeDark, L"default");
    LoadStringSetting(L"Style.DarkMode.bgColorMode", g_settings.bgColorModeDark, L"default");
    LoadStringSetting(L"Style.DarkMode.customBorderColor", g_settings.customBorderColorDark, L"#FFFFFF");
    LoadStringSetting(L"Style.DarkMode.customHighlightFillColor", g_settings.customHighlightFillColorDark, L"#FFFFFF");
    LoadStringSetting(L"Style.DarkMode.customBgColor", g_settings.customBgColorDark, L"#202020");
    
    LoadStringSetting(L"Style.DarkMode.iconBgColorMode", g_settings.iconBgColorModeDark, L"default");
    LoadStringSetting(L"Style.DarkMode.customIconBgColor", g_settings.customIconBgColorDark, L"#000000");
    g_settings.iconBgOpacityDark = Wh_GetIntSetting(L"Style.DarkMode.iconBgOpacity");
    if (g_settings.iconBgOpacityDark < 0) g_settings.iconBgOpacityDark = 0;
    if (g_settings.iconBgOpacityDark > 100) g_settings.iconBgOpacityDark = 100;

    LoadStringSetting(L"Style.DarkMode.indicatorBgColorMode", g_settings.indicatorBgColorModeDark, L"accent");
    LoadStringSetting(L"Style.DarkMode.customIndicatorBgColor", g_settings.customIndicatorBgColorDark, L"#333333");
    g_settings.indicatorBgOpacityDark = Wh_GetIntSetting(L"Style.DarkMode.indicatorBgOpacity");
    if (g_settings.indicatorBgOpacityDark < 0) g_settings.indicatorBgOpacityDark = 0;
    if (g_settings.indicatorBgOpacityDark > 100) g_settings.indicatorBgOpacityDark = 100;
    
    LoadStringSetting(L"Style.DarkMode.indicatorTextColorMode", g_settings.indicatorTextColorModeDark, L"default");
    LoadStringSetting(L"Style.DarkMode.customIndicatorTextColor", g_settings.customIndicatorTextColorDark, L"#FFFFFF");

    // Light Mode color settings
    LoadStringSetting(L"Style.LightMode.borderColorMode", g_settings.borderColorModeLight, L"default");
    LoadStringSetting(L"Style.LightMode.highlightFillColorMode", g_settings.highlightFillColorModeLight, L"default");
    LoadStringSetting(L"Style.LightMode.bgColorMode", g_settings.bgColorModeLight, L"default");
    LoadStringSetting(L"Style.LightMode.customBorderColor", g_settings.customBorderColorLight, L"#000000");
    LoadStringSetting(L"Style.LightMode.customHighlightFillColor", g_settings.customHighlightFillColorLight, L"#000000");
    LoadStringSetting(L"Style.LightMode.customBgColor", g_settings.customBgColorLight, L"#F3F3F3");

    LoadStringSetting(L"Style.LightMode.iconBgColorMode", g_settings.iconBgColorModeLight, L"default");
    LoadStringSetting(L"Style.LightMode.customIconBgColor", g_settings.customIconBgColorLight, L"#FFFFFF");
    g_settings.iconBgOpacityLight = Wh_GetIntSetting(L"Style.LightMode.iconBgOpacity");
    if (g_settings.iconBgOpacityLight < 0) g_settings.iconBgOpacityLight = 0;
    if (g_settings.iconBgOpacityLight > 100) g_settings.iconBgOpacityLight = 100;

    LoadStringSetting(L"Style.LightMode.indicatorBgColorMode", g_settings.indicatorBgColorModeLight, L"accent");
    LoadStringSetting(L"Style.LightMode.customIndicatorBgColor", g_settings.customIndicatorBgColorLight, L"#EAEAEA");
    g_settings.indicatorBgOpacityLight = Wh_GetIntSetting(L"Style.LightMode.indicatorBgOpacity");
    if (g_settings.indicatorBgOpacityLight < 0) g_settings.indicatorBgOpacityLight = 0;
    if (g_settings.indicatorBgOpacityLight > 100) g_settings.indicatorBgOpacityLight = 100;
    
    LoadStringSetting(L"Style.LightMode.indicatorTextColorMode", g_settings.indicatorTextColorModeLight, L"default");
    LoadStringSetting(L"Style.LightMode.customIndicatorTextColor", g_settings.customIndicatorTextColorLight, L"#000000");

    LoadStringSetting(L"Appearance.Font.fontFamily", g_settings.fontFamily, L"");
    // Trim leading/trailing whitespace
    WCHAR* fontTrim = g_settings.fontFamily;
    while (*fontTrim == L' ' || *fontTrim == L'\t') fontTrim++;
    if (fontTrim != g_settings.fontFamily) memmove(g_settings.fontFamily, fontTrim, (wcslen(fontTrim) + 1) * sizeof(WCHAR));
    size_t fontLen = wcslen(g_settings.fontFamily);
    while (fontLen > 0 && (g_settings.fontFamily[fontLen - 1] == L' ' || g_settings.fontFamily[fontLen - 1] == L'\t')) {
        g_settings.fontFamily[--fontLen] = L'\0';
    }
    
    g_settings.fontSize = Wh_GetIntSetting(L"Appearance.Font.fontSize");
    if (g_settings.fontSize <= 0) {
        g_settings.fontSize = IsWin11OrGreater() ? 10 : 9;
    }

    LoadStringSetting(L"Appearance.Font.fontStyle", g_settings.fontStyle, L"regular");

    g_settings.applyToGroupIndicator = Wh_GetIntSetting(L"Appearance.Font.applyToGroupIndicator");

    LoadStringSetting(L"Accessibility.switcherDisplayBehavior", g_settings.switcherDisplayBehavior, L"cursorMonitor");

    // Exclusion patterns (newline-delimited text fields)
    g_excludeTitlePatterns.clear();
    g_excludeExePatterns.clear();

    v = Wh_GetStringSetting(L"ExcludedWindows.excludeByTitle");
    if (v && *v) {
        std::wstring valStr(v);
        size_t start = 0;
        while (start < valStr.length()) {
            size_t end = valStr.find(L';', start);
            if (end == std::wstring::npos) end = valStr.length();
            std::wstring token = valStr.substr(start, end - start);
            size_t first = token.find_first_not_of(L" \t\r\n");
            if (first != std::wstring::npos) {
                token = token.substr(first);
                size_t last = token.find_last_not_of(L" \t\r\n");
                token = token.substr(0, last + 1);
                if (!token.empty()) g_excludeTitlePatterns.push_back(token);
            }
            start = end + 1;
        }
    }
    if (v) Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"ExcludedWindows.excludeByExe");
    if (v && *v) {
        std::wstring valStr(v);
        size_t start = 0;
        while (start < valStr.length()) {
            size_t end = valStr.find(L';', start);
            if (end == std::wstring::npos) end = valStr.length();
            std::wstring token = valStr.substr(start, end - start);
            size_t first = token.find_first_not_of(L" \t\r\n");
            if (first != std::wstring::npos) {
                token = token.substr(first);
                size_t last = token.find_last_not_of(L" \t\r\n");
                token = token.substr(0, last + 1);
                if (!token.empty()) g_excludeExePatterns.push_back(token);
            }
            start = end + 1;
        }
    }
    if (v) Wh_FreeStringSetting(v);

    g_settings.excludeXboxMode = LoadBoolSetting(L"ExcludedWindows.excludeXboxMode", false);
    g_settings.handleTouchpadGestures = LoadBoolSetting(L"Touchpad.enabled", true);
    g_settings.stickyTouchpadMode = LoadBoolSetting(L"Touchpad.stickyLaunch", true);
    g_touchpadGesturesEnabled.store(g_settings.handleTouchpadGestures);
    g_touchpadStickyLaunchEnabled.store(g_settings.stickyTouchpadMode);
    PublishNativeSwipePolicy();

    // Custom per-process header (array of { process, iconPath, appName }).
    g_customHeaderRules.clear();
    auto trimWs = [](std::wstring s) -> std::wstring {
        size_t a = s.find_first_not_of(L" \t\r\n");
        if (a == std::wstring::npos) return L"";
        size_t b = s.find_last_not_of(L" \t\r\n");
        return s.substr(a, b - a + 1);
    };
    for (int i = 0; ; i++) {
        PCWSTR proc = Wh_GetStringSetting(L"customHeader[%d].process", i);
        PCWSTR icon = Wh_GetStringSetting(L"customHeader[%d].iconPath", i);
        PCWSTR name = Wh_GetStringSetting(L"customHeader[%d].appName", i);
        bool hasProc = proc && *proc;
        bool hasIcon = icon && *icon;
        bool hasName = name && *name;
        if (hasProc && (hasIcon || hasName)) {
            std::wstring p = trimWs(proc);
            std::wstring ip = hasIcon ? trimWs(icon) : L"";
            std::wstring an = hasName ? trimWs(name) : L"";
            if (!p.empty() && (!ip.empty() || !an.empty()))
                g_customHeaderRules.push_back({p, ip, an});
        }
        bool endOfArray = !hasProc && !hasIcon && !hasName;
        if (proc) Wh_FreeStringSetting(proc);
        if (icon) Wh_FreeStringSetting(icon);
        if (name) Wh_FreeStringSetting(name);
        if (endOfArray) break;
    }
    UpdateCachedSettings();
    g_settings.switcherPadding = GetActiveSwitcherPadding();
    ResetWindowIconSettings();
    InvalidateStaticCache();
}


// Explorer.exe hooks for Alt+Tab hotkey

static bool SWS_IsAltTabHotkey(UINT fsModifiers, UINT vk) {
    UINT baseMods = fsModifiers & ~MOD_NOREPEAT;
    if (vk == VK_TAB && (baseMods & MOD_ALT)) return true;
    return false;
}

typedef BOOL(WINAPI *RegisterHotKey_t)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
static RegisterHotKey_t RegisterHotKey_Original;

static BOOL WINAPI RegisterHotKey_Hook(HWND hWnd, int id, UINT fsModifiers, UINT vk) {
    if (SWS_IsAltTabHotkey(fsModifiers, vk)) {
        Wh_Log(L"Blocked explorer RegisterHotKey for Alt+Tab variant (vk=0x%X, mod=0x%X)", vk, fsModifiers);
        SetLastError(0);
        return TRUE;
    }
    return RegisterHotKey_Original(hWnd, id, fsModifiers, vk);
}

using XamlAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis, void* param1, int param2, void* param3);
static XamlAltTabViewHost_Show_t XamlAltTabViewHost_Show_Original = nullptr;

using CAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis, void* param1, int param2, void* param3);
static CAltTabViewHost_Show_t CAltTabViewHost_Show_Original = nullptr;
static bool s_twinuiAltTabHooksApplied = false;
static bool s_twinuiAltTabHookGaveUp = false;
static int s_twinuiAltTabHookAttempts = 0;

static bool SwitcherOwnsRawSwipe() {
    // The typed dispatcher is authoritative when available. Window-show
    // callbacks arrive later and cannot distinguish three from four fingers.
    if (NativeSwipeSourceGateActive()) return false;
    HWND hSwitcher = FindWindowW(SWS_CLASSNAME, SWS_MAIN_WINDOW_TITLE);
    if (!hSwitcher) return false;
    HANDLE hSession = GetPropW(hSwitcher, SWS_RAW_SWIPE_PROP);
    if (!hSession) return false;
    return RawSwipeMarkerRemainingMs(hSession, GetTickCount64()) != 0;
}

static bool SwitcherOwnsActiveRawSwipe() {
    HWND hSwitcher = FindWindowW(SWS_CLASSNAME, SWS_MAIN_WINDOW_TITLE);
    if (!hSwitcher) return false;
    HANDLE hSession = GetPropW(hSwitcher, SWS_RAW_SWIPE_SESSION_PROP);
    return hSession && RawSwipeMarkerRemainingMs(hSession, GetTickCount64()) != 0;
}

static HRESULT WINAPI XamlAltTabViewHost_Show_Hook(void* pThis, void* param1, int param2, void* param3) {
    if (!ExplicitShellInputDown() && SwitcherOwnsRawSwipe()) {
        Wh_Log(L"SWS: suppressed Win11 native switcher during owned raw stroke");
        return S_OK;
    }
    return XamlAltTabViewHost_Show_Original(pThis, param1, param2, param3);
}

static HRESULT WINAPI CAltTabViewHost_Show_Hook(void* pThis, void* param1, int param2, void* param3) {
    if (!ExplicitShellInputDown() && SwitcherOwnsRawSwipe()) {
        Wh_Log(L"SWS: suppressed Win10 native switcher during owned raw stroke");
        return S_OK;
    }
    return CAltTabViewHost_Show_Original(pThis, param1, param2, param3);
}

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original = nullptr;

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow != SW_HIDE && !ExplicitShellInputDown()) {
        bool genericShell = SwitcherOwnsRawSwipe() &&
                            (IsNativeAltTabWindow(hWnd) || IsTaskViewWindow(hWnd));
        if (genericShell) {
            Wh_Log(L"SWS: suppressed native switcher/Task View ShowWindow call");
            return IsWindowVisible(hWnd); // BOOL reports previous visibility, not success.
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

// twinui.pcshell.dll can still be resolving symbols when Explorer loads the mod. The
// initial ModInit attempt is retained for the normal path, but late retries are required
// because a missed Alt+Tab/Task View hook leaves the raw-swipe property with no consumer.
#if defined(_M_IX86)
static bool TryHookTwinuiAltTab() {
    // The private member-function ABI is intentionally not guessed for x86. The public
    // ShowWindow hook remains available on that architecture.
    return true;
}
#else
static bool QueueTwinuiShellHooks(HMODULE hTwinui) {
    if (XamlAltTabViewHost_Show_Original && CAltTabViewHost_Show_Original) {
        return true;
    }
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK altTabHooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
            &XamlAltTabViewHost_Show_Original, XamlAltTabViewHost_Show_Hook, true,
        },
        {
            {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
            &CAltTabViewHost_Show_Original, CAltTabViewHost_Show_Hook, true,
        },
    };
    // Submit only unresolved paths, so a retry never hooks a trampoline twice.
    size_t first = XamlAltTabViewHost_Show_Original ? 1 : 0;
    size_t count = ARRAYSIZE(altTabHooks) - first -
                   (CAltTabViewHost_Show_Original ? 1 : 0);
    return WindhawkUtils::HookSymbols(hTwinui, altTabHooks + first, count);
}

static bool TryHookTwinuiAltTab() {
    if (s_twinuiAltTabHooksApplied || s_twinuiAltTabHookGaveUp) return true;
    // Native tap filtering has its own independent explorer.exe hook/retry.

    HMODULE hTwinui = GetModuleHandleW(L"twinui.pcshell.dll");
    if (!hTwinui) {
        // The module can be loaded after the Explorer mod itself. Keep the retry alive.
        return false;
    }

    if (++s_twinuiAltTabHookAttempts > 30) {
        s_twinuiAltTabHookGaveUp = true;
        Wh_Log(L"SWS: twinui hook retries exhausted (Alt+Tab=%d)",
               XamlAltTabViewHost_Show_Original || CAltTabViewHost_Show_Original);
        return true;
    }

    if (!QueueTwinuiShellHooks(hTwinui)) return false;

    if (!Wh_ApplyHookOperations()) {
        Wh_Log(L"SWS: applying late twinui Alt+Tab/Task View hooks failed");
        return false;
    }
    s_twinuiAltTabHooksApplied =
        XamlAltTabViewHost_Show_Original || CAltTabViewHost_Show_Original;
    Wh_Log(L"SWS: twinui hooks applied (Alt+Tab=%d)",
           s_twinuiAltTabHooksApplied);
    return s_twinuiAltTabHooksApplied;
}
#endif

// --- Native PTP manipulation source gate (twinui.dll) ----------------------------------
static std::atomic<bool> s_nativeSwipeHooksAvailable{false};
static std::atomic<bool> s_nativeSwipeShutdownRequested{false};
static std::atomic<HWND> s_nativeSwipeStatusWindow{nullptr};

static bool NativeSwipeSourceGateActive() {
    if (s_nativeSwipeShutdownRequested.load()) return false;
    if (s_nativeSwipeHooksAvailable.load()) return true;
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    return taskbar && GetPropW(taskbar, SWS_NATIVE_SWIPE_GATE_PROP) != nullptr;
}

static bool PublishNativeSwipeHookStatus(bool active) {
    if (!active) {
        HWND window = s_nativeSwipeStatusWindow.exchange(nullptr);
        if (window) RemovePropW(window, SWS_NATIVE_SWIPE_GATE_PROP);
        return true;
    }
    if (s_nativeSwipeShutdownRequested.load()) return false;
    HWND published = s_nativeSwipeStatusWindow.load();
    if (published && GetPropW(published, SWS_NATIVE_SWIPE_GATE_PROP)) return true;
    s_nativeSwipeStatusWindow.store(nullptr);
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) return false; // The retry timer or first native report retries.
    DWORD owner = 0;
    GetWindowThreadProcessId(taskbar, &owner);
    if (owner != GetCurrentProcessId()) return true; // Secondary Explorer.
    if (!SetPropW(taskbar, SWS_NATIVE_SWIPE_GATE_PROP, (HANDLE)(ULONG_PTR)1)) return false;
    s_nativeSwipeStatusWindow.store(taskbar);
    if (s_nativeSwipeShutdownRequested.load()) {
        PublishNativeSwipeHookStatus(false);
        return false;
    }
    return true;
}

#if defined(_M_IX86)
static bool TryHookNativeSwipe() {
    return true; // No guessed x86 private member-function ABI.
}
#else
// Exact public symbols and call sites were checked on Windows 26100.9549.
// Private context pointers are opaque: none of their fields/layouts are read.
using NativeSwipeProcess_t = HRESULT(__cdecl*)(void*, const void*, const void*);
using NativeSwipeStart_t = void(__cdecl*)(void*, const void*, int, int);
using NativeSwipeFinish_t = void(__cdecl*)(void*, const void*, int, int, int, int);
using NativeSwipeCancel_t = void(__cdecl*)(void*);
using NativeSwipeTarget_t = HRESULT(__cdecl*)(void*, int, int, void**);
static NativeSwipeProcess_t NativeSwipeProcess_Original = nullptr;
static NativeSwipeStart_t NativeSwipeStart_Original = nullptr;
static NativeSwipeFinish_t NativeSwipeFinish_Original = nullptr;
static NativeSwipeCancel_t NativeSwipeCancel_Original = nullptr;
static NativeSwipeTarget_t NativeSwipeTarget_Original = nullptr;

struct NativeSwipePolicy {
    HWND endpoint = nullptr;
    DWORD value = 0;
    bool active = false;
    bool candidateRequired = false;
    DWORD relayEpoch = 0;
};

static NativeSwipePolicy ReadNativeSwipePolicy() {
    NativeSwipePolicy policy;
    policy.endpoint = FindWindowW(SWS_CLASSNAME, SWS_MAIN_WINDOW_TITLE);
    if (policy.endpoint) {
        policy.value = (DWORD)(ULONG_PTR)GetPropW(
            policy.endpoint, SWS_NATIVE_SWIPE_POLICY_PROP);
        policy.active = GetPropW(policy.endpoint, SWS_NATIVE_SWIPE_ACTIVE_PROP) != nullptr;
        policy.candidateRequired =
            GetPropW(policy.endpoint, SWS_NATIVE_SWIPE_PROFILE_PROP) != nullptr;
        policy.relayEpoch = (DWORD)(ULONG_PTR)GetPropW(
            policy.endpoint, SWS_NATIVE_TOUCHPAD_EPOCH_PROP);
    }
    return policy;
}

static bool NativeTouchpadPolicyReady(const NativeSwipePolicy& policy) {
    return (policy.value & 3u) == 3u && !s_nativeSwipeShutdownRequested.load();
}

// Grant only the current endpoint PID, never ASFW_ANY. This runs independently
// of debug logging; it grants permission but does not itself activate a window.
static BOOL GrantNativeSwipeForeground(const NativeSwipePolicy& expected) {
    NativeSwipePolicy current = ReadNativeSwipePolicy();
    if (!s_nativeSwipeHooksAvailable.load() || !NativeTouchpadPolicyReady(current) ||
        current.active || current.endpoint != expected.endpoint ||
        current.value != expected.value || !current.relayEpoch ||
        current.relayEpoch != expected.relayEpoch) {
        return FALSE;
    }
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(current.endpoint, &pid) || !pid) {
        Wh_Log(L"SWS: native PTP foreground grant not attempted (endpoint=%p pid=%u)",
               current.endpoint, pid);
        return FALSE;
    }
    HWND foreground = GetForegroundWindow();
    SetLastError(ERROR_SUCCESS);
    BOOL granted = AllowSetForegroundWindow(pid);
    DWORD error = granted ? ERROR_SUCCESS : GetLastError();
    Wh_Log(L"SWS: native PTP foreground grant callerPid=%u targetPid=%u granted=%d error=%u foregroundBefore=%p foregroundAfter=%p",
           GetCurrentProcessId(), pid, granted, error, foreground, GetForegroundWindow());
    return granted;
}

enum class NativeSwipeDecision { Undecided, Allowed, Suppressed };

struct NativeSwipeHandlerState {
    void* handler = nullptr;
    ULONGLONG generation = 0;
    NativeSwipeDecision decision = NativeSwipeDecision::Undecided;
    HWND endpoint = nullptr;
    DWORD policyEpoch = 0;
    DWORD relayEpoch = 0;
    DWORD relayToken = 0;
};

static std::atomic<DWORD> s_nativeSwipeNextRelayToken{0};

static DWORD BeginNativeSwipeInvocation(const NativeSwipePolicy& policy, UINT direction) {
    if (!GrantNativeSwipeForeground(policy)) return 0;
    NativeSwipePolicy current = ReadNativeSwipePolicy();
    if (!NativeTouchpadPolicyReady(current) || current.active ||
        current.endpoint != policy.endpoint || current.value != policy.value ||
        current.relayEpoch != policy.relayEpoch) {
        return 0;
    }
    DWORD token = ++s_nativeSwipeNextRelayToken;
    if (!token) token = ++s_nativeSwipeNextRelayToken;
    if (!PostMessageW(policy.endpoint, WM_SWS_NATIVE_TOUCHPAD,
                      (WPARAM)((policy.relayEpoch << 3) | direction), (LPARAM)token)) {
        DWORD error = GetLastError();
        Wh_Log(L"SWS: native PTP invocation post failed (endpoint=%p error=%u)",
               policy.endpoint, error);
        return 0;
    }
    return token;
}

static void PostNativeSwipeCompletion(const NativeSwipeHandlerState& state, UINT event) {
    if (!state.relayToken || !s_nativeSwipeHooksAvailable.load()) return;
    NativeSwipePolicy policy = ReadNativeSwipePolicy();
    if (!NativeTouchpadPolicyReady(policy) || policy.endpoint != state.endpoint ||
        (policy.value >> 3) != state.policyEpoch || policy.relayEpoch != state.relayEpoch) {
        return;
    }
    if (!PostMessageW(state.endpoint, WM_SWS_NATIVE_TOUCHPAD,
                      (WPARAM)((state.relayEpoch << 3) | event), (LPARAM)state.relayToken)) {
        DWORD error = GetLastError();
        Wh_Log(L"SWS: native PTP completion post failed (event=%u token=%u error=%u)",
               event, state.relayToken, error);
        // The existing lost-input timer cancels; a failed post never commits.
    }
}

struct NativeSwipeScope;
struct NativeSwipeThreadState {
    // Fixed storage: stale/missing End callbacks cannot allocate without bound,
    // and no heap allocation or TLS destructor remains on Explorer threads at unload.
    NativeSwipeHandlerState handlers[32]{};
    NativeSwipeScope* current = nullptr;
    ULONGLONG nextGeneration = 0;
};
static_assert(std::is_trivially_destructible_v<NativeSwipeThreadState>);
static thread_local NativeSwipeThreadState s_nativeSwipeThread;
static NativeSwipeHandlerState* FindNativeSwipeState(void* handler);

struct NativeSwipeScope {
    void* handler;
    ULONGLONG generation;
    bool starting;
    NativeSwipeScope* previous;
    NativeSwipeHandlerState state;

    NativeSwipeScope(void* handler, ULONGLONG generation = 0, bool starting = false)
        : handler(handler), generation(generation), starting(starting),
          previous(s_nativeSwipeThread.current) {
        auto* current = FindNativeSwipeState(handler);
        if (current && (!generation || current->generation == generation)) state = *current;
        s_nativeSwipeThread.current = this;
    }
    ~NativeSwipeScope() { s_nativeSwipeThread.current = previous; }
    NativeSwipeScope(const NativeSwipeScope&) = delete;
    NativeSwipeScope& operator=(const NativeSwipeScope&) = delete;
};

static NativeSwipeHandlerState* FindNativeSwipeState(void* handler) {
    if (!handler) return nullptr;
    for (auto& state : s_nativeSwipeThread.handlers) {
        if (state.handler == handler) return &state;
    }
    return nullptr;
}

static NativeSwipeHandlerState* BeginNativeSwipeState(void* handler) {
    if (!handler) return nullptr;
    NativeSwipeHandlerState* state = FindNativeSwipeState(handler);
    if (!state) {
        for (auto& candidate : s_nativeSwipeThread.handlers) {
            if (!candidate.handler) {
                state = &candidate;
                break;
            }
        }
    }
    // Fail open for additional handlers; never evict a live suppressed stroke.
    if (!state) return nullptr;
    *state = {};
    state->handler = handler;
    if (!++s_nativeSwipeThread.nextGeneration) ++s_nativeSwipeThread.nextGeneration;
    state->generation = s_nativeSwipeThread.nextGeneration;
    return state;
}

static void EndNativeSwipeState(void* handler, ULONGLONG generation) {
    auto* state = FindNativeSwipeState(handler);
    // A reentrant Start during original cleanup owns a new generation.
    if (state && state->generation == generation) *state = {};
}

static void CommitNativeSwipeState(const NativeSwipeHandlerState& decision) {
    auto* current = FindNativeSwipeState(decision.handler);
    if (current && current->generation == decision.generation) *current = decision;
}

static HRESULT __cdecl NativeSwipeProcess_Hook(void* pThis, const void* info,
                                               const void* output) {
    if (s_nativeSwipeHooksAvailable.load()) PublishNativeSwipeHookStatus(true);
    NativeSwipeScope scope(pThis);
    return NativeSwipeProcess_Original(pThis, info, output);
}

static void __cdecl NativeSwipeStart_Hook(void* pThis, const void* info, int x, int y) {
    auto* state = s_nativeSwipeHooksAvailable.load() ? BeginNativeSwipeState(pThis) : nullptr;
    NativeSwipeScope scope(pThis, state ? state->generation : 0, true);
    NativeSwipeStart_Original(pThis, info, x, y);
    if (scope.state.endpoint) {
        // Verified ProcessInteractionContextOutput call site passes the public
        // manipulation's cumulative translations here, not raw HID centroids.
        Wh_Log(L"SWS: native PTP start boundary cumulativeX=%d cumulativeY=%d generation=%llu",
               x, y, (unsigned long long)scope.state.generation);
    }
}

static void __cdecl NativeSwipeFinish_Hook(void* pThis, const void* info,
                                         int x, int y, int vx, int vy) {
    auto* state = FindNativeSwipeState(pThis);
    ULONGLONG generation = state ? state->generation : 0;
    NativeSwipeScope scope(pThis, generation);
    // The native End still queries its target and resets its own gesture state.
    NativeSwipeFinish_Original(pThis, info, x, y, vx, vy);
    if (scope.state.endpoint) {
        // End receives the final delta and cached velocity, not cumulative XY.
        Wh_Log(L"SWS: native PTP end boundary deltaX=%d deltaY=%d velocityX=%d velocityY=%d generation=%llu",
               x, y, vx, vy, (unsigned long long)scope.state.generation);
    }
    RequestTouchpadInputDiagnostics(scope.state.endpoint, 0x100);
    PostNativeSwipeCompletion(scope.state, SWS_NATIVE_TOUCHPAD_END);
    EndNativeSwipeState(pThis, generation);
}

static void __cdecl NativeSwipeCancel_Hook(void* pThis) {
    auto* state = FindNativeSwipeState(pThis);
    ULONGLONG generation = state ? state->generation : 0;
    NativeSwipeScope scope(pThis, generation);
    NativeSwipeCancel_Original(pThis);
    RequestTouchpadInputDiagnostics(scope.state.endpoint, 0x200);
    PostNativeSwipeCompletion(scope.state, SWS_NATIVE_TOUCHPAD_CANCEL);
    EndNativeSwipeState(pThis, generation);
}

static bool WaitForFreshRawThreeFingerCandidate(NativeSwipePolicy& policy,
                                                DWORD* waitedMs) {
    ULONGLONG start = GetTickCount64();
    ULONGLONG deadline = start + SWS_NATIVE_TOUCHPAD_CANDIDATE_WAIT_MS;
    bool found = false;
    do {
        policy = ReadNativeSwipePolicy();
        if (policy.active) break;
        if (NativeTouchpadPolicyReady(policy) && policy.endpoint &&
            FreshRawThreeFingerCandidate(policy.endpoint, GetTickCount64())) {
            found = true;
            break;
        }
        if (GetTickCount64() >= deadline) break;
        Sleep(1);
    } while (true);
    if (waitedMs) {
        *waitedMs = (DWORD)(GetTickCount64() - start);
    }
    return found;
}

static HRESULT __cdecl NativeSwipeTarget_Hook(void* pThis, int type, int direction,
                                              void** target) {
    NativeSwipeScope* scope = s_nativeSwipeThread.current;
    auto* state = scope ? &scope->state : nullptr;
    if (s_nativeSwipeShutdownRequested.load() || !s_nativeSwipeHooksAvailable.load() ||
        !target || type != 1 || !state ||
        !state->handler || !state->generation) {
        return NativeSwipeTarget_Original(pThis, type, direction, target);
    }
    if (state->decision == NativeSwipeDecision::Allowed ||
        (state->decision == NativeSwipeDecision::Undecided && !scope->starting)) {
        return NativeSwipeTarget_Original(pThis, type, direction, target);
    }

    bool deciding = state->decision == NativeSwipeDecision::Undecided;
    // Property reads/logging may notify reentrantly. Until this Start's read
    // completes, nested queries for the same generation are conservative.
    if (deciding) {
        state->decision = NativeSwipeDecision::Allowed;
        CommitNativeSwipeState(*state);
    }
    NativeSwipePolicy policy = ReadNativeSwipePolicy();
    // Each scope retains its generation's decision even if a reentrant Start
    // replaces the map entry. In particular, old End must not finish a handler
    // whose old Start was suppressed, or overwrite the new stroke's decision.
    bool ready = NativeTouchpadPolicyReady(policy);
    if (deciding) {
        // The native args already retain input provenance. Explicit Win/mouse
        // state must not veto this PTP source gate. Outside down/tap stay native.
        bool reserved = direction == 1 || direction == 3 ||
                        (direction == 2 && (policy.value & 4u));
        bool candidateRequired = policy.candidateRequired;
        DWORD candidateWaitMs = 0;
        bool candidate = policy.active || !candidateRequired;
        if (!policy.active && reserved && candidateRequired) {
            candidate = WaitForFreshRawThreeFingerCandidate(policy,
                                                            &candidateWaitMs);
            ready = NativeTouchpadPolicyReady(policy);
        }
        bool suppress = ready && direction >= 1 && direction <= 4 &&
                        (policy.active || (reserved && candidate));
        if (suppress && !policy.active) {
            state->relayToken = BeginNativeSwipeInvocation(policy, (UINT)direction);
            state->relayEpoch = policy.relayEpoch;
            if (!state->relayToken) {
                // Fail open outside if either the grant or the queue handoff
                // fails. A concurrently opened session still owns its input.
                policy = ReadNativeSwipePolicy();
                suppress = NativeTouchpadPolicyReady(policy) && policy.active;
            }
        }
        state->decision = suppress ? NativeSwipeDecision::Suppressed : NativeSwipeDecision::Allowed;
        state->endpoint = policy.endpoint;
        state->policyEpoch = policy.value >> 3;
        CommitNativeSwipeState(*state);
        if (suppress) {
            Wh_Log(L"SWS: suppressed native PTP start (direction=%d active=%d candidateRequired=%d candidate=%d waitMs=%u)",
                   direction, policy.active, candidateRequired, candidate,
                   candidateWaitMs);
        } else {
            Wh_Log(L"SWS: native PTP start left to Windows (direction=%d active=%d ready=%d sticky=%d candidateRequired=%d candidate=%d waitMs=%u)",
                   direction, policy.active, ready, (policy.value & 4u) != 0,
                   candidateRequired, candidate, candidateWaitMs);
        }
        Wh_Log(L"SWS: native PTP start context %s",
               FormatTouchpadForegroundDiagnostic(GetForegroundWindow(), policy.endpoint).c_str());
        RequestTouchpadInputDiagnostics(policy.endpoint, (UINT)direction);
    } else if (!ready || state->endpoint != policy.endpoint ||
               state->policyEpoch != (policy.value >> 3)) {
        // Master-off/reader loss is immediate and irreversible for this stroke,
        // including a disable/re-enable interval between native callbacks.
        state->decision = NativeSwipeDecision::Allowed;
        CommitNativeSwipeState(*state);
    }
    if (state->decision == NativeSwipeDecision::Suppressed &&
        !s_nativeSwipeShutdownRequested.load() && s_nativeSwipeHooksAvailable.load()) {
        // Verified native contract: no selected service returns S_OK/null too.
        // Keep it through Update/End/Cancel regardless of raw marker expiry.
        *target = nullptr;
        return S_OK;
    }
    return NativeSwipeTarget_Original(pThis, type, direction, target);
}

static void* s_nativeSwipeTargets[5]{};
static size_t s_nativeSwipeQueuedCount = 0;
static unsigned int s_nativeSwipeRemoveMask = 0;
static bool s_nativeSwipeRollbackPending = false;
static bool s_nativeSwipeHookGaveUp = false;
static int s_nativeSwipeHookAttempts = 0;

static bool RollbackNativeSwipeHooks() {
    bool removed = true;
    for (size_t i = 0; i < s_nativeSwipeQueuedCount; ++i) {
        if (!(s_nativeSwipeRemoveMask & (1u << i))) {
            if (Wh_RemoveFunctionHook(s_nativeSwipeTargets[i])) {
                s_nativeSwipeRemoveMask |= 1u << i;
            } else {
                removed = false;
            }
        }
    }
    if (!removed || !Wh_ApplyHookOperations()) return false;
    s_nativeSwipeQueuedCount = 0;
    s_nativeSwipeRemoveMask = 0;
    s_nativeSwipeRollbackPending = false;
    return true;
}

static bool TryHookNativeSwipe() {
    if (s_nativeSwipeShutdownRequested.load()) return true;
    if (s_nativeSwipeHooksAvailable.load()) return PublishNativeSwipeHookStatus(true);
    if (s_nativeSwipeHookGaveUp) return true;
    if (s_nativeSwipeRollbackPending && !RollbackNativeSwipeHooks()) return false;
    if (!s_nativeSwipeQueuedCount) {
        HMODULE module = GetModuleHandleW(L"twinui.dll");
        if (!module) return false; // Independent late-module retry, no load/reference leak.
        void* targets[5]{};
        // Use the canonical undecorated names returned by Windhawk's symbol
        // provider. All five targets must resolve before any hook is queued.
        WindhawkUtils::SYMBOL_HOOK twinuiDllHooks[] = {
            {{L"public: virtual long __cdecl TouchpadGestureHandler::ProcessInteractionContextOutput(struct InteractionContextOutputInfo const *,struct INTERACTION_CONTEXT_OUTPUT const *)"}, &targets[0], nullptr},
            {{L"private: void __cdecl TouchpadGestureHandler::_StartGesture(struct InteractionContextOutputInfo const *,int,int)"}, &targets[1], nullptr},
            {{L"private: void __cdecl TouchpadGestureHandler::_FinishGesture(struct InteractionContextOutputInfo const *,int,int,int,int)"}, &targets[2], nullptr},
            {{L"private: void __cdecl TouchpadGestureHandler::_CancelGesture(void)"}, &targets[3], nullptr},
            // MSDIA separates the pointer levels as "* *", not "**". Exact
            // matching is required; one missing target disables this whole set.
            {{L"public: virtual long __cdecl TouchpadSettingsManager::GetGestureTarget(enum TOUCHPAD_GESTURE_TYPE,enum TOUCHPAD_GESTURE_DIRECTION,struct ITouchpadGesture * *)"}, &targets[4], nullptr},
        };
        bool resolved = WindhawkUtils::HookSymbols(module, twinuiDllHooks,
                                                  ARRAYSIZE(twinuiDllHooks));
        for (void* target : targets) resolved = resolved && target;
        if (!resolved) {
            ++s_nativeSwipeHookAttempts;
            if (s_nativeSwipeHookAttempts == 1 || s_nativeSwipeHookAttempts % 5 == 0) {
                unsigned int resolvedMask = 0;
                for (size_t i = 0; i < ARRAYSIZE(targets); ++i) {
                    if (targets[i]) resolvedMask |= 1u << i;
                }
                Wh_Log(L"SWS: native PTP swipe symbol resolution failed (attempt=%d resolved=0x%02X expected=0x1F)",
                       s_nativeSwipeHookAttempts, resolvedMask);
            }
            if (s_nativeSwipeHookAttempts >= 30) {
                s_nativeSwipeHookGaveUp = true;
                Wh_Log(L"SWS: native PTP swipe symbols unavailable; source gate stays disabled");
            }
            return s_nativeSwipeHookGaveUp;
        }
        void* hooks[] = {(void*)NativeSwipeProcess_Hook, (void*)NativeSwipeStart_Hook,
                        (void*)NativeSwipeFinish_Hook, (void*)NativeSwipeCancel_Hook,
                        (void*)NativeSwipeTarget_Hook};
        void** originals[] = {(void**)&NativeSwipeProcess_Original, (void**)&NativeSwipeStart_Original,
                              (void**)&NativeSwipeFinish_Original, (void**)&NativeSwipeCancel_Original,
                              (void**)&NativeSwipeTarget_Original};
        for (size_t i = 0; i < ARRAYSIZE(targets); ++i) {
            s_nativeSwipeTargets[i] = targets[i];
            if (!Wh_SetFunctionHook(targets[i], hooks[i], originals[i])) {
                // Other independent installers may apply operations meanwhile.
                // Every wrapper remains passthrough until this whole set is active.
                s_nativeSwipeRollbackPending = true;
                RollbackNativeSwipeHooks();
                return false;
            }
            ++s_nativeSwipeQueuedCount;
        }
    }
    if (!Wh_ApplyHookOperations()) return false; // Retry apply, never queue twice.
    if (s_nativeSwipeShutdownRequested.load()) return true;
    s_nativeSwipeHooksAvailable.store(true);
    if (s_nativeSwipeShutdownRequested.load()) {
        s_nativeSwipeHooksAvailable.store(false);
        return true;
    }
    Wh_Log(L"SWS: native PTP swipe source gate active (all five hooks applied)");
    return PublishNativeSwipeHookStatus(true);
}
#endif

// --- 3-finger 'show desktop' swipe ------------------------------------------------------
// The shell runs the touchpad show-desktop swipe through CTray::_RaiseDesktop, the same
// entry point as Win+D and the taskbar button (the win-d-per-monitor mod hooks it for the
// same reason). The typed twinui source gate owns the decision when installed:
// pcshell posts this action asynchronously with flags 3 and no finger count.
// A late active-session veto would also block an allowed four-finger gesture or
// reclaim an outside stroke which began before SWS opened. Without the complete
// source gate, retain the raw-marker fallback for proven three-finger strokes.
// All policy/evidence is published cross-process, so Explorer never waits on
// the switcher queue or depends on its foreground controller.
#if defined(_M_IX86)
// The receiver of a member function arrives in ECX on x86, which a plain hook signature
// cannot describe, and the 32-bit shell is only reachable on 32-bit Windows: skip the hook
// there and keep the block on the 64-bit architectures.
static bool TryHookRaiseDesktop() {
    static bool s_logged = false;
    if (!s_logged) {
        s_logged = true;
        Wh_Log(L"SWS: show desktop swipe filtering is not available in the 32-bit shell");
    }
    return true;
}
#else
using RaiseDesktop_t = void(__cdecl*)(void* pThis, int flags);
static RaiseDesktop_t RaiseDesktop_Original = nullptr;
static bool s_raiseDesktopHooked = false;
static bool s_raiseDesktopHookQueued = false;
static bool s_raiseDesktopHookGaveUp = false;
static int s_raiseDesktopHookAttempts = 0;

static void __cdecl RaiseDesktop_Hook(void* pThis, int flags) {
    // flags 2 and 3 are the touchpad swipe paths (as documented by the win-d-per-monitor
    // mod, which hooks this function for the same gesture); the Win+D hotkey and the
    // taskbar button use other values and always pass through. The block additionally
    // requires a usable policy and physical-stroke evidence on the fallback
    // path. Keep this flags filter: lift-grace must not turn into a Win+D block.
    bool isTouchpadSwipe = (flags == 2 || flags == 3);
    if (isTouchpadSwipe) {
        NativeSwipePolicy policy = ReadNativeSwipePolicy();
        HWND hSwitcher = policy.endpoint;
        bool ready = NativeTouchpadPolicyReady(policy);
        bool sourceGate = s_nativeSwipeHooksAvailable.load();
        HANDLE marker = hSwitcher ? GetPropW(hSwitcher, SWS_RAW_SWIPE_PROP) : NULL;
        HANDLE session = hSwitcher ? GetPropW(hSwitcher, SWS_RAW_SWIPE_SESSION_PROP) : NULL;
        HANDLE upward = hSwitcher ? GetPropW(hSwitcher, SWS_RAW_SWIPE_UP_PROP) : NULL;
        ULONGLONG now = GetTickCount64();
        DWORD remaining = RawSwipeMarkerRemainingMs(marker, now);
        DWORD sessionRemaining = RawSwipeMarkerRemainingMs(session, now);
        bool liftGrace = marker && ((DWORD)(ULONG_PTR)marker & SWS_RAW_SWIPE_LIFT_FLAG) != 0;
        // Session-owned grace covers this stroke's delayed native finish even
        // after commit/close. The reader clears it at the next outside stroke;
        // a launch-only candidate must not acquire that release ownership.
        bool sessionMarker = session && sessionRemaining != 0;
        bool owns = ready && !sourceGate &&
                    (sessionMarker || (remaining != 0 && !liftGrace && upward));
        // This hook can run for every touchpad report. Log at most once per
        // half-second instead of two OutputDebugString calls per frame.
        static std::atomic<ULONGLONG> lastLogTick{0};
        ULONGLONG previous = lastLogTick.load();
        if ((!previous || now - previous >= 500) &&
            lastLogTick.compare_exchange_strong(previous, now)) {
            Wh_Log(L"SWS: RaiseDesktop touch flags=%d owns=%d active=%d ready=%d swipeGate=%d remaining=%u lift=%d session=%d up=%d",
                   flags, owns, policy.active, ready, sourceGate,
                   remaining, liftGrace, sessionMarker, upward != NULL);
        }
        if (owns) return;
    }
    RaiseDesktop_Original(pThis, flags);
}

// Returns true when the retry timer can stop (hook installed, or given up on).
static bool TryHookRaiseDesktop() {
    if (s_raiseDesktopHooked || s_raiseDesktopHookGaveUp) return true;
    if (s_raiseDesktopHookQueued) {
        // This thread starts in Wh_ModAfterInit. Hooks queued after ModInit
        // must be explicitly applied, including a successful symbol retry.
        if (!Wh_ApplyHookOperations()) return false;
        s_raiseDesktopHooked = true;
        Wh_Log(L"SWS: hooked CTray::_RaiseDesktop (raw touchpad swipe filtering active)");
        return true;
    }
    HMODULE hExplorerModule = GetModuleHandleW(L"explorer.exe");
    if (!hExplorerModule) return true;

    s_raiseDesktopHookAttempts++;
    if (s_raiseDesktopHookAttempts == 1 || (s_raiseDesktopHookAttempts % 5) == 0) {
        Wh_Log(L"SWS: trying CTray::_RaiseDesktop hook attempt=%d",
               s_raiseDesktopHookAttempts);
    }
    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {LR"(protected: void __cdecl CTray::_RaiseDesktop(enum RAISEDESKTOPFLAGS))"},
            (void**)&RaiseDesktop_Original,
            (void*)RaiseDesktop_Hook,
        },
    };
    if (WindhawkUtils::HookSymbols(hExplorerModule, explorerExeHooks,
                                   ARRAYSIZE(explorerExeHooks))) {
        s_raiseDesktopHookQueued = true;
        return TryHookRaiseDesktop();
    }
    if (s_raiseDesktopHookAttempts >= 30) {
        s_raiseDesktopHookGaveUp = true;
        Wh_Log(L"SWS: CTray::_RaiseDesktop not available, the show desktop swipe cannot be filtered");
        return true;
    }
    return false;
}
#endif

// Native PTP actions retain their input source here, before Search/media actions
// or custom keyboard shortcuts are dispatched. Verified against explorer.exe's
// public symbols/call sites: 0xF selects ThreeFingerTapEnabled, 0x10 four-finger.
// Filtering downstream Start requests loses this provenance and can mistake an
// injected Win key for explicit keyboard input (or block a real Start request).
#if defined(_M_IX86)
static bool TryHookTouchpadTap() {
    return true; // Do not guess the private x86 member-function ABI.
}
#else
using HandlePTPTap_t = void(__cdecl*)(void* pThis, int settingIdentifier);
static HandlePTPTap_t HandlePTPTap_Original = nullptr;
static bool s_touchpadTapHooked = false;
static bool s_touchpadTapHookQueued = false;
static bool s_touchpadTapHookGaveUp = false;
static int s_touchpadTapHookAttempts = 0;

static void __cdecl HandlePTPTap_Hook(void* pThis, int settingIdentifier) {
    NativeSwipePolicy policy = ReadNativeSwipePolicy();
    bool ready = NativeTouchpadPolicyReady(policy);
    // A source-specific tap can arrive before the reader or between strokes
    // in an idle sticky session. Session intent does not expire with the last
    // raw marker; that marker is needed only for completion after UI teardown.
    bool owned = settingIdentifier == 0xF && ready &&
                 (policy.active || SwitcherOwnsActiveRawSwipe());
    // Action-edge logging only; no HID-frame or paint-path logging here.
    Wh_Log(L"SWS: native PTP tap setting=0x%X owned=%d active=%d ready=%d swipeGate=%d",
           settingIdentifier, owned, policy.active, ready, s_nativeSwipeHooksAvailable.load());
    if (owned) return;
    HandlePTPTap_Original(pThis, settingIdentifier);
}

static bool TryHookTouchpadTap() {
    if (s_touchpadTapHooked || s_touchpadTapHookGaveUp) return true;
    if (s_touchpadTapHookQueued) {
        if (!Wh_ApplyHookOperations()) return false;
        s_touchpadTapHooked = true;
        Wh_Log(L"SWS: hooked CTray::HandlePTPTap (native three-finger action filtering active)");
        return true;
    }
    HMODULE hExplorerModule = GetModuleHandleW(L"explorer.exe");
    if (!hExplorerModule) return false;
    ++s_touchpadTapHookAttempts;
    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {LR"(protected: void __cdecl CTray::HandlePTPTap(enum SETTING_IDENTIFIER))"},
            &HandlePTPTap_Original, HandlePTPTap_Hook,
        },
    };
    if (WindhawkUtils::HookSymbols(hExplorerModule, explorerExeHooks,
                                   ARRAYSIZE(explorerExeHooks))) {
        s_touchpadTapHookQueued = true;
        return TryHookTouchpadTap();
    }
    if (s_touchpadTapHookAttempts >= 30) {
        s_touchpadTapHookGaveUp = true;
        Wh_Log(L"SWS: CTray::HandlePTPTap unavailable; native tap filtering requires foreground controller routing");
        return true;
    }
    return false;
}
#endif

static bool TryHookExplorerSuppression() {
    // Attempt each path independently. An unresolved swipe/Alt+Tab symbol must
    // not postpone tap filtering while Explorer's symbol cache is loading.
    bool tapReady = TryHookTouchpadTap();
    bool desktopReady = TryHookRaiseDesktop();
    bool altTabReady = TryHookTwinuiAltTab();
    bool swipeReady = TryHookNativeSwipe();
    return tapReady && desktopReady && altTabReady && swipeReady;
}

// Background thread for tool mod process

// --- Taking over the global 3-finger gestures while the switcher is up -------------------
// Windows.UI.Input.TouchpadGesturesController is the documented way for the *foreground*
// application to receive global (three or more finger) touchpad gestures instead of the
// system's own handler. Registered from the switcher process, the system routes 3-finger
// swipes to it while this process owns the foreground - while the switcher is visible or
// closing - so the OS performs none of its own actions for those strokes (Task View, Show
// desktop, Switch apps, or tap actions). Outside a switcher session this process does not
// claim the foreground and Windows keeps its default up/down/tap gestures.
//
// The compiler's copy of windows.ui.input.h predates the class, so the ABI is declared here.
// The layout is not guessed: it was read from the Windows.UI.winmd shipped with the system:
// ITouchpadGesturesControllerStatics is IsSupported, CreateForProcess, and
// ITouchpadGesturesController (IID 28c13cdd-e068-549f-89c6-1a440c6fc327) starts with
// get_Enabled, put_Enabled, get_SupportedGestures, put_SupportedGestures, followed by the
// events this mod does not subscribe to.
static const GUID SWS_IID_TouchpadGesturesControllerStatics =
    {0x207ef171, 0x1a73, 0x51cd, {0xa6, 0x94, 0x88, 0x40, 0xe0, 0x9d, 0xba, 0xfa}};

struct SwsTouchpadGestureStatics : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE IsSupported(bool* supported) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateForProcess(IInspectable** controller) = 0;
};

struct SwsTouchpadGestureController : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Enabled(bool* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Enabled(bool value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SupportedGestures(UINT* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SupportedGestures(UINT value) = 0;
};

// TouchpadGlobalGestureKinds::ThreeFingerManipulations
#define SWS_TOUCHPAD_THREE_FINGER_MANIPULATIONS 0x1
#define SWS_TOUCHPAD_THREE_FINGER_ACTIONS 0x8

static SwsTouchpadGestureController* g_touchpadGestureController = nullptr;

// Whether the current settings want the mod to own three-finger touchpad
// manipulations and actions while the switcher owns the foreground.
static bool TouchpadGestureTakeoverWanted() {
    // Windows ignores background controllers. The reader's cross-process
    // marker and Explorer PTP dispatch hook cover release after UI teardown.
    return TouchpadHandlingEnabled() &&
           g_touchpadReaderAvailable.load() &&
           g_touchpadGestureTakeoverAvailable.load() &&
            (g_isVisible || g_isPendingShow || g_animExitActive);
}

static UINT TouchpadGestureKinds() {
    // Claim both swipe/drag manipulations and stationary actions in every
    // switcher mode. Raw HID supplies the actual gesture state machine, while
    // this mask prevents Windows from independently running Task View, Show
    // desktop, native switching, or a three-finger action.
    return SWS_TOUCHPAD_THREE_FINGER_MANIPULATIONS |
           SWS_TOUCHPAD_THREE_FINGER_ACTIONS;
}

static void ReleaseTouchpadGestureTakeover() {
    if (!g_touchpadGestureController) return;
    g_touchpadGestureController->put_Enabled(false);
    g_touchpadGestureController->Release();
    g_touchpadGestureController = nullptr;
    Wh_Log(L"SWS: released the 3-finger gesture takeover");
}

// Creates the controller on first use, or toggles the one that exists. False means the
// take-over is not possible and Windows keeps handling the swipes.
static bool UpdateTouchpadGestureTakeover(bool want) {
    if (!want) {
        ReleaseTouchpadGestureTakeover();
        return true;
    }
    auto markUnavailable = []() {
        g_touchpadGestureTakeoverAvailable.store(false);
        return false;
    };
    if (g_touchpadGestureController) {
        HRESULT hr = g_touchpadGestureController->put_SupportedGestures(TouchpadGestureKinds());
        if (SUCCEEDED(hr)) hr = g_touchpadGestureController->put_Enabled(true);
        if (SUCCEEDED(hr)) return true;
        Wh_Log(L"SWS: could not update gesture takeover (0x%08X)", hr);
        ReleaseTouchpadGestureTakeover();
        return markUnavailable();
    }

    HMODULE hCombase = GetModuleHandleW(L"combase.dll");
    if (!hCombase) hCombase = LoadLibraryW(L"combase.dll");
    if (!hCombase) {
        Wh_Log(L"SWS: combase.dll not available, 3-finger gestures stay with Windows");
        return markUnavailable();
    }
    using WindowsCreateString_t = HRESULT(WINAPI*)(PCWSTR, UINT32, void**);
    using WindowsDeleteString_t = HRESULT(WINAPI*)(void*);
    using RoGetActivationFactory_t = HRESULT(WINAPI*)(void*, REFIID, void**);
    auto createString = (WindowsCreateString_t)GetProcAddress(hCombase, "WindowsCreateString");
    auto deleteString = (WindowsDeleteString_t)GetProcAddress(hCombase, "WindowsDeleteString");
    auto getActivationFactory =
        (RoGetActivationFactory_t)GetProcAddress(hCombase, "RoGetActivationFactory");
    if (!createString || !deleteString || !getActivationFactory) {
        Wh_Log(L"SWS: WinRT activation is not available, 3-finger gestures stay with Windows");
        return markUnavailable();
    }

    const WCHAR* className = L"Windows.UI.Input.TouchpadGesturesController";
    void* hClassName = nullptr;
    HRESULT hr = createString(className, (UINT32)wcslen(className), &hClassName);
    if (FAILED(hr) || !hClassName) {
        Wh_Log(L"SWS: could not create the WinRT class name (0x%08X)", hr);
        return markUnavailable();
    }
    SwsTouchpadGestureStatics* statics = nullptr;
    hr = getActivationFactory(hClassName, SWS_IID_TouchpadGesturesControllerStatics,
                              (void**)&statics);
    deleteString(hClassName);
    if (FAILED(hr) || !statics) {
        Wh_Log(L"SWS: TouchpadGesturesController is not available on this system (0x%08X)", hr);
        return markUnavailable();
    }

    bool supported = false;
    hr = statics->IsSupported(&supported);
    if (FAILED(hr) || !supported) {
        Wh_Log(L"SWS: TouchpadGesturesController not supported (hr=0x%08X supported=%d)",
               hr, supported);
        statics->Release();
        return markUnavailable();
    }

    IInspectable* controller = nullptr;
    hr = statics->CreateForProcess(&controller);
    statics->Release();
    if (FAILED(hr) || !controller) {
        Wh_Log(L"SWS: could not create the gesture controller (0x%08X)", hr);
        return markUnavailable();
    }
    // CreateForProcess returns ITouchpadGesturesController, whose IInspectable head matches
    // the declaration above (see the layout note); the two methods used are the ones that
    // matter, the events stay unsubscribed.
    auto candidate = (SwsTouchpadGestureController*)controller;
    HRESULT hrGestures = candidate->put_SupportedGestures(TouchpadGestureKinds());
    HRESULT hrEnabled = SUCCEEDED(hrGestures) ? candidate->put_Enabled(true) : hrGestures;
    if (FAILED(hrEnabled)) {
        candidate->put_Enabled(false);
        candidate->Release();
        Wh_Log(L"SWS: gesture takeover configuration failed (0x%08X)", hrEnabled);
        return markUnavailable();
    }
    g_touchpadGestureController = candidate;
    Wh_Log(L"SWS: took over the 3-finger gestures for the switcher (mask=0x%X)", TouchpadGestureKinds());
    return true;
}

static bool RefreshTouchpadGestureKinds() {
    // Only the switcher thread creates, configures and releases this controller.
    bool twoFingerActive =
        TouchpadHandlingEnabled() && g_touchpadReaderAvailable.load() &&
        g_isVisible && !g_animExitActive && !g_isHidingSwitcher;
    if (g_touchpadTwoFingerCloseActive.exchange(twoFingerActive) != twoFingerActive) {
        Wh_Log(L"SWS TAPTRACE lifetime tick=%llu closeActive=%d visible=%d pending=%d exit=%d hiding=%d hook=%p",
               (unsigned long long)GetTickCount64(), twoFingerActive, g_isVisible,
               g_isPendingShow, g_animExitActive, g_isHidingSwitcher, g_hMouseHook);
    }
    UpdateTwoFingerTapOverride(twoFingerActive);
    return UpdateTouchpadGestureTakeover(TouchpadGestureTakeoverWanted());
}

static DWORD WINAPI SwitcherThread(LPVOID lpParam) {
    Wh_Log(L"SwitcherThread starting");
    // The switcher owns top-level windows whose coordinates are physical
    // monitor pixels. Keep this UI thread in PMv2 before creating any HWND so
    // Windows does not virtualize mixed-DPI monitor coordinates.
    using SetThreadDpiAwarenessContext_t = HANDLE(WINAPI*)(HANDLE);
    auto setThreadDpiAwarenessContext = (SetThreadDpiAwarenessContext_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext");
    if (setThreadDpiAwarenessContext) {
        setThreadDpiAwarenessContext((HANDLE)-4); // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    }
    struct RuntimeApartment {
        HRESULT result = RoInitialize(RO_INIT_SINGLETHREADED);
        ~RuntimeApartment() { if (SUCCEEDED(result)) RoUninitialize(); }
    } apartment;
    if (FAILED(apartment.result)) {
        Wh_Log(L"SWS: WinRT apartment initialization failed (0x%08X)", apartment.result);
        return 1;
    }
    g_easeEntrance.Init(0.0f, 0.0f, 0.0f, 1.0f);
    g_easeSlide.Init(0.55f, 0.55f, 0.0f, 1.0f);
    g_easeHover.Init(0.55f, 0.55f, 0.0f, 1.0f);
    g_easeHoverEnter.Init(0.0f, 0.0f, 0.0f, 1.0f);
    QueryPerformanceFrequency(&g_animPerfFreq);
    // Create the virtual desktop manager on this thread so it lives in the same
    // apartment that uses it (EnumWindowsProc runs here). An STA interface
    // pointer is only valid in the apartment that created it.
    CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                     IID_IVirtualDesktopManager, (void**)&g_pVirtualDesktopManager);
    ResolveAPIs();
    LoadSettings();
    g_isDarkMode = ShouldUseDarkMode();

    BufferedPaintInit();

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = SwitcherWndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = SWS_CLASSNAME;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.style = CS_DBLCLKS;
    RegisterClassExW(&wc);

    // Use WS_POPUP | WS_THICKFRAME to get DWM rounded corners and shadows, 
    // without the system caption buttons. We remove the frame via WM_NCCALCSIZE.
    DWORD dwStyle = WS_POPUP | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | (ThemeIs(L"none") ? WS_EX_LAYERED : 0);
    g_hSwitcher = CreateSWSWindow(exStyle, SWS_CLASSNAME, SWS_MAIN_WINDOW_TITLE,
        dwStyle, 0, 0, 0, 0, NULL, NULL, GetModuleHandleW(NULL), NULL);
    if (!g_hSwitcher) { Wh_Log(L"Failed to create switcher window"); return 1; }
    PublishNativeSwipePolicy();
    StartWindowIconWorker();

    // Installed for the thread's lifetime (see CancelPendingShow): the raw-HID path needs it
    // while the switcher is hidden, to hide a Task View the OS shows for its own 3-finger
    // up gesture.
    s_hWinEventHook = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, NULL,
                                      WinEventShowHideProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    s_hForegroundEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL,
        WinEventShowHideProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!s_hWinEventHook || !s_hForegroundEventHook) {
        Wh_Log(L"SWS: SetWinEventHook failed (object=%p foreground=%p error=%u)",
               s_hWinEventHook, s_hForegroundEventHook, GetLastError());
    }

    g_hCloseBtnWnd = CreateSWSWindow(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        SWS_CLASSNAME, L"",
        WS_POPUP, 0, 0, 0, 0, g_hSwitcher, NULL, GetModuleHandleW(NULL), NULL);

    BOOL bExclude = TRUE;
    DwmSetWindowAttribute(g_hSwitcher, DWMWA_EXCLUDED_FROM_PEEK, &bExclude, sizeof(bExclude));

    g_hTheme = OpenThemeData(NULL, L"CompositedWindow::Window");
    g_shellHookMsg = RegisterWindowMessageW(L"SHELLHOOK");
    RegisterShellHookWindow(g_hSwitcher);

    HWND hFgInit = GetForegroundWindow();
    if (hFgInit && !IsSwitcherWindow(hFgInit)) {
        UpdateMruWindow(hFgInit);
    }

    g_hFont = CreateScaledFont(96);

    SWS_RegisterHotkeys();

    timeBeginPeriod(1);
    UpdateRefreshRateTiming();

    #ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
    #define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
    #endif
    HANDLE hAnimTimer = CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_MODIFY_STATE | SYNCHRONIZE);
    if (!hAnimTimer) {
        hAnimTimer = CreateWaitableTimerExW(NULL, NULL, 0, TIMER_MODIFY_STATE | SYNCHRONIZE);
    }

    g_hDwmCornerWatchStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_hDwmCornerWatchStopEvent) {
        g_hDwmCornerWatchThread = CreateThread(NULL, 0, DwmCornerWatchThread, g_hSwitcher, 0, NULL);
    }

    // Own the 3-finger swipes while the mod is set up to handle them: from here on the
    // system routes them to this process whenever the switcher holds the foreground, so the
    // OS runs none of its own swipe actions under a session (see the takeover above).
    UpdateTouchpadGestureTakeover(TouchpadGestureTakeoverWanted());

    Wh_Log(L"Simple Window Switcher initialized, entering message loop");

    MSG msg;
    while (true) {
        // Keep reports and discrete commands in queue order. Bound dispatch by
        // elapsed time, not only count; a single synchronous handler can still
        // overrun this budget, but no additional batch is run before a due frame.
        constexpr int kMaxMessagesBeforeFrame = 128;
        LARGE_INTEGER dispatchStart, now;
        QueryPerformanceCounter(&dispatchStart);
        double dispatchBudget = (std::min)(2.0, s_animTargetIntervalMs * 0.5) *
                                (double)g_animPerfFreq.QuadPart / 1000.0;
        int messagesThisTurn = 0;
        while (messagesThisTurn < kMaxMessagesBeforeFrame &&
               PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto thread_exit;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            messagesThisTurn++;
            QueryPerformanceCounter(&now);
            if ((double)(now.QuadPart - dispatchStart.QuadPart) >= dispatchBudget ||
                (g_animActive && (double)now.QuadPart >= g_animNextFrameDeadline)) break;
        }

        if (g_animActive) {
            QueryPerformanceCounter(&now);
            if ((double)now.QuadPart >= g_animNextFrameDeadline) {
                OnAnimationTick();
                QueryPerformanceCounter(&now);
                double interval = s_animTargetIntervalMs * (double)g_animPerfFreq.QuadPart / 1000.0;
                // Advance from the absolute deadline, skipping missed frames.
                // Never replay a backlog or shift the cadence on an input wake.
                double missed = floor(((double)now.QuadPart - g_animNextFrameDeadline) / interval);
                g_animNextFrameDeadline += ((std::max)(0.0, missed) + 1.0) * interval;
            }

            if (g_animActive) {
                QueryPerformanceCounter(&now);
                double waitMs = (g_animNextFrameDeadline - (double)now.QuadPart) *
                                1000.0 / (double)g_animPerfFreq.QuadPart;
                if (waitMs > 0.0) {
                    BOOL timerArmed = FALSE;
                    if (hAnimTimer) {
                        LARGE_INTEGER dueTime;
                        dueTime.QuadPart = -(LONGLONG)(std::max)(1.0, ceil(waitMs * 10000.0));
                        timerArmed = SetWaitableTimer(hAnimTimer, &dueTime, 0, NULL, NULL, FALSE);
                    }
                    if (timerArmed) {
                        HANDLE handles[1] = { hAnimTimer };
                        MsgWaitForMultipleObjectsEx(
                            1, handles,
                            INFINITE,
                            QS_ALLINPUT,
                            MWMO_ALERTABLE | MWMO_INPUTAVAILABLE
                        );
                    } else {
                        // No waitable timer (or arming failed): fall back to a plain
                        // timeout wait so the animation can't stall indefinitely.
                        MsgWaitForMultipleObjectsEx(
                            0, NULL,
                            (DWORD)ceil(waitMs),
                            QS_ALLINPUT,
                            MWMO_ALERTABLE | MWMO_INPUTAVAILABLE
                        );
                    }
                }
            }
        } else {
            MsgWaitForMultipleObjectsEx(
                0, NULL,
                INFINITE,
                QS_ALLINPUT,
                MWMO_ALERTABLE | MWMO_INPUTAVAILABLE
            );
        }
    }

thread_exit:
    StopWindowIconWorker();
    if (hAnimTimer) {
        CloseHandle(hAnimTimer);
        hAnimTimer = NULL;
    }
    SWS_UnregisterHotkeys();
    // The watcher only posts; join it before destroying its fixed HWND target.
    if (g_hDwmCornerWatchStopEvent) SetEvent(g_hDwmCornerWatchStopEvent);
    if (g_hDwmCornerWatchThread) {
        WaitForSingleObject(g_hDwmCornerWatchThread, INFINITE);
        CloseHandle(g_hDwmCornerWatchThread);
        g_hDwmCornerWatchThread = NULL;
    }
    if (g_hDwmCornerWatchStopEvent) {
        CloseHandle(g_hDwmCornerWatchStopEvent);
        g_hDwmCornerWatchStopEvent = NULL;
    }
    g_touchpadGesturesEnabled.store(false);
    PublishNativeSwipePolicy();
    if (g_isVisible || g_isPendingShow) HideSwitcher();
    g_touchpadTwoFingerCloseActive.store(false);
    UpdateTwoFingerTapOverride(false);
    g_touchpadRawTwoFingerTapMouseState.store(0);
    s_rawTwoFingerMouseButtonSerial = 0;
    s_rawTwoFingerMouseButtonDeadline = 0;
    if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_RAW_TWO_TAP_MOUSE_TIMER_ID);
    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = nullptr;
    }
    FinishRawTouchpadShield();
    ReleaseTouchpadGestureTakeover();
    UnregisterThumbnails();
    g_windows.clear();
    g_mruWindows.clear();
    DestroyBackdropWindow();
    if (g_hCloseBtnWnd) { DestroyWindow(g_hCloseBtnWnd); g_hCloseBtnWnd = NULL; }
    RemoveNativeSwipePolicy();
    if (g_hSwitcher) { DeregisterShellHookWindow(g_hSwitcher); DestroyWindow(g_hSwitcher); g_hSwitcher = NULL; }
    UnregisterClassW(SWS_CLASSNAME, GetModuleHandleW(NULL));
    if (g_hFont) { DeleteObject(g_hFont); g_hFont = NULL; }
    if (g_hTheme) { CloseThemeData(g_hTheme); g_hTheme = NULL; }
    BufferedPaintUnInit();
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }

    if (g_pVirtualDesktopManager) {
        g_pVirtualDesktopManager->Release();
        g_pVirtualDesktopManager = NULL;
    }
    FinishAnimations();
    StopAnimationTicker();
    FreeCachedBuffers();
    timeEndPeriod(1);
    Wh_Log(L"SwitcherThread exiting");
    return 0;
}

// --- Raw-HID touchpad reader (plan stage B2: drives the switcher from raw frames) -----
// The tool-mod process reads the precision touchpad's HID reports directly (same
// mechanism as the Three Finger Drag mod), so the 3-finger swipe drives the switcher
// from the raw frames instead of relying on Explorer's native-switcher interception.
// The reader is passive: it never consumes, blocks or injects input. If reports
// are unavailable, touchpad support is unavailable; keyboard/mouse still work.
#define SWS_TOUCHPAD_READER_CLASSNAME L"WindhawkSWS_TouchpadReader"

#define SWS_HID_PAGE_GENERIC        0x01
#define SWS_HID_USAGE_X             0x30
#define SWS_HID_USAGE_Y             0x31
#define SWS_HID_PAGE_DIGITIZER      0x0D
#define SWS_HID_USAGE_TOUCHPAD      0x05
#define SWS_HID_USAGE_TIP_SWITCH    0x42
#define SWS_HID_USAGE_CONTACT_ID    0x51
#define SWS_HID_USAGE_CONTACT_COUNT 0x54

struct TouchpadContact {
    ULONG id;
    LONG x;
    LONG y;
    bool tip;
};

struct TouchpadDevice {
    std::vector<BYTE> preparsed;
    std::vector<USHORT> fingerCollections;
    USHORT contactCountCollection = 0;
    double rangeX = 0.0;
    double rangeY = 0.0;
    bool hasContactCount = false;
    bool candidateGated = false;
    bool valid = false;
};

static std::map<HANDLE, TouchpadDevice> g_touchpadDevices;
static std::vector<TouchpadContact> g_touchpadFrame;
static ULONG g_touchpadFrameExpected = 0;
static HANDLE g_hTouchpadReaderThread = NULL;
static HANDLE g_hTouchpadReaderStopEvent = NULL;
static HANDLE g_hTouchpadReaderReadyEvent = NULL;

static void UpdateNativeCandidateRequirement() {
    bool required = std::any_of(
        g_touchpadDevices.begin(), g_touchpadDevices.end(),
        [](const auto& entry) {
            return entry.second.valid && entry.second.candidateGated;
        });
    g_touchpadNativeCandidateRequired.store(required);
}

// Some devices declare a maximum which only fits unsigned.
static double TouchpadLogicalRange(const HIDP_VALUE_CAPS& caps) {
    LONGLONG min = caps.LogicalMin;
    LONGLONG max = caps.LogicalMax;
    if (max <= min && caps.BitSize > 0 && caps.BitSize <= 32) {
        min = 0;
        max = (1LL << caps.BitSize) - 1;
    }
    return (double)(max - min);
}

static const TouchpadDevice* TouchpadDeviceFor(HANDLE hDevice) {
    auto it = g_touchpadDevices.find(hDevice);
    if (it != g_touchpadDevices.end()) {
        return it->second.valid ? &it->second : NULL;
    }

    TouchpadDevice& dev = g_touchpadDevices[hDevice];

    UINT size = 0;
    if (GetRawInputDeviceInfoW(hDevice, RIDI_PREPARSEDDATA, NULL, &size) != 0 || !size) {
        return NULL;
    }

    dev.preparsed.resize(size);
    if (GetRawInputDeviceInfoW(hDevice, RIDI_PREPARSEDDATA, dev.preparsed.data(), &size) == (UINT)-1) {
        return NULL;
    }

    PHIDP_PREPARSED_DATA preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();
    HIDP_CAPS caps = {};
    if (HidP_GetCaps(preparsed, &caps) != HIDP_STATUS_SUCCESS) {
        return NULL;
    }

    USHORT valueCapsCount = caps.NumberInputValueCaps;
    std::vector<HIDP_VALUE_CAPS> valueCaps(valueCapsCount);
    if (!valueCapsCount ||
        HidP_GetValueCaps(HidP_Input, valueCaps.data(), &valueCapsCount, preparsed) != HIDP_STATUS_SUCCESS) {
        return NULL;
    }

    for (USHORT i = 0; i < valueCapsCount; i++) {
        const HIDP_VALUE_CAPS& vc = valueCaps[i];
        USAGE usage = vc.IsRange ? vc.Range.UsageMin : vc.NotRange.Usage;

        if (vc.UsagePage == SWS_HID_PAGE_DIGITIZER && usage == SWS_HID_USAGE_CONTACT_COUNT) {
            dev.hasContactCount = true;
            dev.contactCountCollection = vc.LinkCollection;
        } else if (vc.UsagePage == SWS_HID_PAGE_GENERIC && usage == SWS_HID_USAGE_X) {
            if (std::find(dev.fingerCollections.begin(), dev.fingerCollections.end(), vc.LinkCollection) ==
                dev.fingerCollections.end()) {
                dev.fingerCollections.push_back(vc.LinkCollection);
            }
            if (!dev.rangeX) dev.rangeX = TouchpadLogicalRange(vc);
        } else if (vc.UsagePage == SWS_HID_PAGE_GENERIC && usage == SWS_HID_USAGE_Y) {
            if (!dev.rangeY) dev.rangeY = TouchpadLogicalRange(vc);
        }
    }

    dev.valid = !dev.fingerCollections.empty() && dev.rangeX > 0 && dev.rangeY > 0;
    // The reported Apple driver publishes five logical finger collections.
    // Laptop PTP devices observed by this reader use a single hybrid
    // collection and must retain the old native-first timing.
    dev.candidateGated = dev.valid && dev.hasContactCount &&
                         dev.fingerCollections.size() >= 3;
    UpdateNativeCandidateRequirement();
    if (g_touchpadReaderAvailable.load()) {
        PublishNativeSwipePolicy();
    }
    Wh_Log(L"SWS touchpad reader: device %p valid=%d slots=%u contactCount=%d range=%.0fx%.0f", hDevice,
           dev.valid, (UINT)dev.fingerCollections.size(), dev.hasContactCount, dev.rangeX, dev.rangeY);
    return dev.valid ? &dev : NULL;
}

// Reader-thread state, reset after reader shutdown and on device removal. It
// must not survive a disable/re-enable cycle as function-local static contacts.
struct TouchpadReaderStroke {
    ULONG lastTips = 0;
    bool markerStrokeOwned = false;
    bool markerSessionOwned = false;
    bool threeFingerCandidatePublished = false;
    bool threeFingerCandidateBlocked = false;
    ULONGLONG threeFingerSerial = 0;
    DWORD twoFingerTapMouseSerial = 0;
    ULONGLONG twoFingerTapStartTick = 0;
    int twoFingerTapOriginX = 0;
    int twoFingerTapOriginY = 0;
    double lastX = -1.0;
    double lastY = -1.0;
    ULONGLONG lastPostTick = 0;
};
static TouchpadReaderStroke s_touchpadReaderStroke;

static void UpdateRawTwoFingerTapMouseEvidence(ULONG tips, ULONG previousTips,
                                              ULONG packedPos, ULONGLONG now) {
    auto& stroke = s_touchpadReaderStroke;
    if (g_touchpadTwoFingerTapSuppressed.load()) {
        stroke.twoFingerTapMouseSerial = 0;
        g_touchpadRawTwoFingerTapMouseState.store(0, std::memory_order_release);
        return;
    }
    int x = (int)(packedPos & 0xFFFF);
    int y = (int)((packedPos >> 16) & 0xFFFF);
    if (tips > 0 && previousTips == 0) {
        // Every fresh stroke retires the prior release tail, including a new
        // outside tap after dismissal. Windows owns that new stroke normally.
        g_touchpadRawTwoFingerTapMouseState.store(0, std::memory_order_release);
        stroke.twoFingerTapMouseSerial = 0;
        if (tips == 2 && g_touchpadTwoFingerCloseActive.load()) {
            DWORD serial = ++g_touchpadRawTwoFingerNextSerial;
            if (!serial) serial = ++g_touchpadRawTwoFingerNextSerial;
            stroke.twoFingerTapMouseSerial = serial;
            stroke.twoFingerTapStartTick = now;
            stroke.twoFingerTapOriginX = x;
            stroke.twoFingerTapOriginY = y;
            g_touchpadRawTwoFingerTapMouseState.store(
                EncodeRawTwoFingerTapMouseState(serial, now + SWS_RAW_TAP_MAX_MS),
                std::memory_order_release);
        }
    } else if (stroke.twoFingerTapMouseSerial) {
        bool tap = now - stroke.twoFingerTapStartTick <= SWS_RAW_TAP_MAX_MS &&
                   tips <= 2 &&
                   (!tips || (abs(x - stroke.twoFingerTapOriginX) <= SWS_RAW_TAP_SLOP &&
                              abs(y - stroke.twoFingerTapOriginY) <= SWS_RAW_TAP_SLOP));
        if (!tap) {
            stroke.twoFingerTapMouseSerial = 0;
            g_touchpadRawTwoFingerTapMouseState.store(0, std::memory_order_release);
        } else if (tips == 0) {
            // Update only the same live stroke. A consumed mouse up may have
            // retired it already, and must not be re-published by a late lift.
            ULONGLONG state = g_touchpadRawTwoFingerTapMouseState.load(
                std::memory_order_acquire);
            if ((DWORD)(state >> 32) == stroke.twoFingerTapMouseSerial) {
                g_touchpadRawTwoFingerTapMouseState.compare_exchange_strong(
                    state, EncodeRawTwoFingerTapMouseState(stroke.twoFingerTapMouseSerial,
                                                          now + SWS_RAW_TWO_TAP_MOUSE_GRACE_MS),
                    std::memory_order_acq_rel);
            }
            stroke.twoFingerTapMouseSerial = 0;
        }
    }
}

static void TouchpadReaderSetAvailable(bool available) {
    if (available && g_touchpadReaderStopping.load()) return;
    if (!available) {
        // Retire cross-process evidence now, not after a queued UI notification.
        if (s_touchpadReaderStroke.threeFingerSerial) {
            PublishRawThreeFingerState(s_touchpadReaderStroke.threeFingerSerial,
                                       SWS_RAW_THREE_PHASE_LOST,
                                       GetTickCount64());
        }
        ClearRawSwipeMarkerPropertyFromReader();
        ClearRawThreeFingerCandidateProperty();
        g_touchpadRawTwoFingerTapMouseState.store(0, std::memory_order_release);
        s_touchpadReaderStroke = {};
        g_touchpadNativeCandidateRequired.store(false);
    }
    bool changed = g_touchpadReaderAvailable.exchange(available) != available;
    if (changed || !available) PublishNativeSwipePolicy();
    if (changed) {
        Wh_Log(L"SWS touchpad reader: readiness=%d stopping=%d", available,
               g_touchpadReaderStopping.load());
    }
    if (changed && g_hSwitcher) {
        PostMessageW(g_hSwitcher, WM_SWS_TOUCHPAD_READER_CHANGED, 0, 0);
    }
}

// RegisterRawInputDevices does not guarantee that an already-connected
// touchpad generates a device-change notification. Prime the HID descriptors
// immediately after registration so takeover readiness does not wait for the
// first user gesture.
static void TouchpadReaderPrimeDevices() {
    UINT count = 0;
    if (GetRawInputDeviceList(NULL, &count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1 || !count) {
        return;
    }
    std::vector<RAWINPUTDEVICELIST> devices(count);
    UINT actual = GetRawInputDeviceList(devices.data(), &count, sizeof(RAWINPUTDEVICELIST));
    if (actual == (UINT)-1) return;
    for (UINT i = 0; i < actual; ++i) {
        if (devices[i].dwType != RIM_TYPEHID) continue;
        RID_DEVICE_INFO info = {};
        info.cbSize = sizeof(info);
        UINT infoSize = sizeof(info);
        if (GetRawInputDeviceInfoW(devices[i].hDevice, RIDI_DEVICEINFO, &info, &infoSize) == (UINT)-1 ||
            info.dwType != RIM_TYPEHID || info.hid.usUsagePage != SWS_HID_PAGE_DIGITIZER ||
            info.hid.usUsage != SWS_HID_USAGE_TOUCHPAD) {
            continue;
        }
        if (TouchpadDeviceFor(devices[i].hDevice)) {
            TouchpadReaderSetAvailable(true);
            Wh_Log(L"SWS touchpad reader: primed device %p before first report", devices[i].hDevice);
        }
    }
}

static void TouchpadReaderProcessFrame(const TouchpadDevice& dev) {
    ++s_touchpadInputDiagnostics.frames;
    s_touchpadInputDiagnostics.lastFrameTick.store(GetTickCount64());
    if (!g_touchpadGesturesEnabled.load()) {
        if (s_touchpadReaderStroke.threeFingerSerial) {
            PublishRawThreeFingerState(s_touchpadReaderStroke.threeFingerSerial,
                                       SWS_RAW_THREE_PHASE_LOST,
                                       GetTickCount64());
        }
        ClearRawSwipeMarkerPropertyFromReader();
        ClearRawThreeFingerCandidateProperty();
        g_touchpadRawTwoFingerTapMouseState.store(0, std::memory_order_release);
        s_touchpadReaderStroke = {};
        return;
    }
    // Descriptor priming normally establishes readiness before this report.
    // A usable frame also recovers readiness after a device reconnect.
    TouchpadReaderSetAvailable(true);
    ULONG tips = 0;
    double cx = 0.0;
    double cy = 0.0;
    for (const TouchpadContact& c : g_touchpadFrame) {
        if (c.tip) {
            tips++;
            cx += c.x;
            cy += c.y;
        }
    }
    if (tips) {
        cx /= tips;
        cy /= tips;
        ++s_touchpadInputDiagnostics.contactFrames;
    }
    s_touchpadInputDiagnostics.lastTips.store(tips);
    ULONGLONG now = GetTickCount64();
    auto& readerStroke = s_touchpadReaderStroke;
    ULONG previousTips = readerStroke.lastTips;
    if (tips > 0 && previousTips == 0) {
        // If a device skipped its zero-contact report, do not let the prior
        // physical stroke authorize the next native gesture.
        ClearRawThreeFingerCandidateProperty();
        readerStroke.threeFingerCandidatePublished = false;
        readerStroke.threeFingerCandidateBlocked = false;
    }
    if (tips > SWS_RAW_SWIPE_FINGERS) {
        readerStroke.threeFingerCandidateBlocked = true;
        if (readerStroke.threeFingerCandidatePublished) {
            ClearRawThreeFingerCandidateProperty();
        }
    }
    if (tips == SWS_RAW_SWIPE_FINGERS &&
        !readerStroke.threeFingerCandidatePublished &&
        !readerStroke.threeFingerCandidateBlocked) {
        ULONGLONG serial = g_touchpadRawThreeFingerNextSerial.fetch_add(
                                1, std::memory_order_relaxed) + 1;
        if (!serial) {
            serial = g_touchpadRawThreeFingerNextSerial.fetch_add(
                         1, std::memory_order_relaxed) + 1;
        }
        readerStroke.threeFingerSerial = serial;
        readerStroke.threeFingerCandidatePublished = true;
        PublishRawThreeFingerState(serial, SWS_RAW_THREE_PHASE_LIVE, now);
        PublishRawThreeFingerCandidate(now);
        Wh_Log(L"SWS: raw three-finger candidate published (serial=%llu)",
               (unsigned long long)serial);
    } else if (readerStroke.threeFingerCandidatePublished && tips > 0) {
        g_touchpadRawThreeFingerLastTick.store(now, std::memory_order_release);
    }
    if (tips == 0) {
        if (readerStroke.threeFingerCandidatePublished &&
            readerStroke.threeFingerSerial) {
            PublishRawThreeFingerState(readerStroke.threeFingerSerial,
                                       SWS_RAW_THREE_PHASE_LIFTED, now);
        }
        ClearRawThreeFingerCandidateProperty();
        readerStroke.threeFingerCandidatePublished = false;
        readerStroke.threeFingerCandidateBlocked = false;
    }
    double nx = dev.rangeX > 0 ? cx / dev.rangeX : 0.0;
    double ny = dev.rangeY > 0 ? cy / dev.rangeY : 0.0;
    ULONG packedPos = ((ULONG)(nx * 65535.0) & 0xFFFF) | (((ULONG)(ny * 65535.0) & 0xFFFF) << 16);
    DWORD tapSerialBefore = readerStroke.twoFingerTapMouseSerial;
    UpdateRawTwoFingerTapMouseEvidence(tips, previousTips, packedPos, now);
    if (tips != previousTips || tapSerialBefore != readerStroke.twoFingerTapMouseSerial) {
        ULONGLONG tapState = g_touchpadRawTwoFingerTapMouseState.load(std::memory_order_acquire);
        Wh_Log(L"SWS TAPTRACE reader tick=%llu tips=%u previous=%u contacts=%u position=%u,%u closeActive=%d "
               L"serialBefore=%u serialAfter=%u candidateSerial=%u candidateRemainingMs=%u tapAgeMs=%lld "
               L"origin=%d,%d raw=%u reports=%u frames=%u posted=%u",
               (unsigned long long)now, tips, previousTips, (UINT)g_touchpadFrame.size(),
               packedPos & 0xFFFF, (packedPos >> 16) & 0xFFFF,
               g_touchpadTwoFingerCloseActive.load(), tapSerialBefore,
               readerStroke.twoFingerTapMouseSerial, (DWORD)(tapState >> 32),
               RawTwoFingerTapMouseRemainingMs(tapState, now),
               TouchpadTraceAgeMs(now, readerStroke.twoFingerTapStartTick),
               readerStroke.twoFingerTapOriginX, readerStroke.twoFingerTapOriginY,
               s_touchpadInputDiagnostics.rawMessages.load(), s_touchpadInputDiagnostics.hidReports.load(),
               s_touchpadInputDiagnostics.frames.load(), s_touchpadInputDiagnostics.posted.load());
    }

    if (!g_hSwitcher || !g_WM_SWS_TOUCHPAD_FRAME || !IsWindow(g_hSwitcher)) {
        return;
    }
    auto& s_lastTips = s_touchpadReaderStroke.lastTips;
    auto& s_markerStrokeOwned = s_touchpadReaderStroke.markerStrokeOwned;
    auto& s_markerSessionOwned = s_touchpadReaderStroke.markerSessionOwned;
    auto& s_lastX = s_touchpadReaderStroke.lastX;
    auto& s_lastY = s_touchpadReaderStroke.lastY;
    auto& s_lastPostTick = s_touchpadReaderStroke.lastPostTick;
    double signedDx = cx - s_lastX;
    double signedDy = cy - s_lastY;
    double dx = signedDx < 0 ? -signedDx : signedDx;
    double dy = signedDy < 0 ? -signedDy : signedDy;
    // Heartbeat while any finger is still down: the switcher keeps a raw session open
    // until the lift, so resting fingers have to keep producing posts.
    bool heartbeat = tips > 0 && now - s_lastPostTick >= 500;
    if (tips == s_lastTips && s_lastX >= 0.0 && dx < dev.rangeX * 0.01 && dy < dev.rangeY * 0.01 &&
        !heartbeat) {
        return;
    }

    // Publish a marker from the raw-input thread before posting the frame. A
    // sticky outside stroke needs this early candidate marker only to suppress
    // Task View while an upward launch is being classified. It is explicitly
    // marked as non-session, so Show Desktop and Start remain Windows-owned.
    bool sessionOwnsStroke = g_touchpadSwitcherActive.load();
    bool launchOwnsStroke = g_touchpadStickyLaunchEnabled.load() &&
                            (tips == 3 || s_markerStrokeOwned);
    if (tips > 0 && s_lastTips == 0 &&
        !sessionOwnsStroke && !s_markerStrokeOwned) {
        // A fresh outside stroke must not inherit the previous session's
        // post-lift grace, even during the reader/UI scheduling gap.
        ClearRawSwipeMarkerPropertyFromReader();
        s_markerSessionOwned = false;
    }
    if (s_markerStrokeOwned) {
        // The UI may have opened SWS from this live horizontal/upward launch
        // candidate. Remember that promotion before a closing UI clears its
        // active bit, so the reader's eventual lift retains the same owner.
        HANDLE session = GetPropW(g_hSwitcher, SWS_RAW_SWIPE_SESSION_PROP);
        if (RawSwipeMarkerRemainingMs(session, now)) s_markerSessionOwned = true;
    }
    if (!g_touchpadGesturesEnabled.load() ||
        (!sessionOwnsStroke && !launchOwnsStroke && !s_markerStrokeOwned)) {
        s_markerStrokeOwned = false;
        s_markerSessionOwned = false;
    } else {
        bool strokeStart = tips == 3 && !s_markerStrokeOwned;
        // Once a stroke has been marked, retain it through close and partial
        // lift even if the session bit changes before the reader sees the lift.
        // A normal horizontal launch can be opened by the preceding posted
        // three-contact frame while outside candidate marking was disabled.
        // If the very next report is partial/full lift, adopt that same stroke
        // now; requiring another three-contact report leaves its release unowned.
        if ((tips == 3 || s_lastTips == 3) && sessionOwnsStroke) {
            s_markerStrokeOwned = true;
            s_markerSessionOwned = true;
        } else if (tips == 3 && launchOwnsStroke) {
            s_markerStrokeOwned = true;
        }
        if (s_markerStrokeOwned) {
            BOOL markerSet = SetRawSwipeMarker(tips == 0, false,
                                                sessionOwnsStroke ||
                                                s_markerSessionOwned);
            if (tips == SWS_RAW_SWIPE_FINGERS && s_lastTips == SWS_RAW_SWIPE_FINGERS &&
                !sessionOwnsStroke && launchOwnsStroke &&
                dy >= dev.rangeY * 0.01 && dy * 2.0 >= dx * 3.0) {
                // Explorer's RaiseDesktop path can run before the UI thread
                // classifies the vertical direction. Mark only upward sticky
                // candidates so downward Show Desktop remains untouched.
                SetRawSwipeUpMarker(signedDy < 0);
            }
            if (strokeStart || tips == 0 || !markerSet) {
                Wh_Log(L"SWS: early raw swipe marker %s (tips=%u session=%d error=%u)",
                       markerSet ? L"published" : L"failed", tips,
                       sessionOwnsStroke,
                       markerSet ? ERROR_SUCCESS : GetLastError());
            }
        }
        if (tips == 0) {
            s_markerStrokeOwned = false;
            s_markerSessionOwned = false;
        }
    }
    s_lastTips = tips;
    s_lastX = cx;
    s_lastY = cy;
    s_lastPostTick = now;

    if (PostMessageW(g_hSwitcher, g_WM_SWS_TOUCHPAD_FRAME, (WPARAM)tips, (LPARAM)packedPos)) {
        ++s_touchpadInputDiagnostics.posted;
    } else {
        DWORD error = GetLastError();
        s_touchpadInputDiagnostics.lastPostError.store(error);
        ++s_touchpadInputDiagnostics.postFailures;
    }
}

static void TouchpadReaderOnReport(const TouchpadDevice& dev, PCHAR report, ULONG length) {
    PHIDP_PREPARSED_DATA preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();

    if (dev.hasContactCount) {
        ULONG count = 0;
        if (HidP_GetUsageValue(HidP_Input, SWS_HID_PAGE_DIGITIZER, dev.contactCountCollection,
                               SWS_HID_USAGE_CONTACT_COUNT, &count, preparsed, report,
                                length) == HIDP_STATUS_SUCCESS) {
            if (count > 0) {
                // A nonzero count starts a new frame, including resync after
                // a missed report. It counts reported contacts, not tip bits.
                g_touchpadFrame.clear();
                g_touchpadFrameExpected = count;
            } else if (g_touchpadFrameExpected) {
                // Hybrid PTPs put the total only in the first report. A zero
                // while contacts are pending continues that same frame; it
                // is not a lift. Keep its contacts and announced count.
                ++s_touchpadInputDiagnostics.hybridReports;
            } else {
                // No frame is pending: this is a standalone empty report.
                // Do not read unused contact slots as live fingers.
                ++s_touchpadInputDiagnostics.emptyReports;
                g_touchpadFrame.clear();
                TouchpadReaderProcessFrame(dev);
                return;
            }
        }
        if (!g_touchpadFrameExpected) {
            return; // the rest of a frame whose start was missed
        }
    }

    for (USHORT collection : dev.fingerCollections) {
        // Parallel reports and the last hybrid packet can contain unused
        // slots with stale data. Only the announced contacts belong to this
        // frame, including contacts whose tip switch reports their release.
        if (dev.hasContactCount &&
            g_touchpadFrame.size() >= g_touchpadFrameExpected) {
            break;
        }
        ULONG x = 0;
        ULONG y = 0;
        if (HidP_GetUsageValue(HidP_Input, SWS_HID_PAGE_GENERIC, collection, SWS_HID_USAGE_X, &x,
                               preparsed, report, length) != HIDP_STATUS_SUCCESS ||
            HidP_GetUsageValue(HidP_Input, SWS_HID_PAGE_GENERIC, collection, SWS_HID_USAGE_Y, &y,
                               preparsed, report, length) != HIDP_STATUS_SUCCESS) {
            continue;
        }

        ULONG id = collection;
        HidP_GetUsageValue(HidP_Input, SWS_HID_PAGE_DIGITIZER, collection, SWS_HID_USAGE_CONTACT_ID,
                           &id, preparsed, report, length);

        bool tip = false;
        USAGE usages[16];
        ULONG usageCount = ARRAYSIZE(usages);
        if (HidP_GetUsages(HidP_Input, SWS_HID_PAGE_DIGITIZER, collection, usages, &usageCount,
                           preparsed, report, length) == HIDP_STATUS_SUCCESS) {
            tip = std::find(usages, usages + usageCount, SWS_HID_USAGE_TIP_SWITCH) != usages + usageCount;
        }

        g_touchpadFrame.push_back({id, (LONG)x, (LONG)y, tip});
    }

    // Hybrid devices announce the contact count first and spread the contacts over
    // several reports; collect until the announced count is reached.
    if (!dev.hasContactCount || g_touchpadFrame.size() >= g_touchpadFrameExpected) {
        TouchpadReaderProcessFrame(dev);
        g_touchpadFrame.clear();
        g_touchpadFrameExpected = 0;
    }
}

static void TouchpadReaderOnRawInput(HRAWINPUT hRawInput) {
    ++s_touchpadInputDiagnostics.rawMessages;
    s_touchpadInputDiagnostics.lastRawTick.store(GetTickCount64());
    UINT size = 0;
    if (GetRawInputData(hRawInput, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER)) != 0 || !size) {
        DWORD error = GetLastError();
        s_touchpadInputDiagnostics.lastReadError.store(error);
        ++s_touchpadInputDiagnostics.readFailures;
        return;
    }

    static std::vector<BYTE> buffer;
    buffer.resize(size);
    if (GetRawInputData(hRawInput, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER)) == (UINT)-1) {
        DWORD error = GetLastError();
        s_touchpadInputDiagnostics.lastReadError.store(error);
        ++s_touchpadInputDiagnostics.readFailures;
        return;
    }

    const RAWINPUT* raw = (const RAWINPUT*)buffer.data();
    if (raw->header.dwType != RIM_TYPEHID) {
        return;
    }
    s_touchpadInputDiagnostics.hidReports.fetch_add(raw->data.hid.dwCount);

    const TouchpadDevice* dev = TouchpadDeviceFor(raw->header.hDevice);
    if (!dev) {
        ++s_touchpadInputDiagnostics.rejectedDevices;
        return;
    }

    const RAWHID& hid = raw->data.hid;
    for (DWORD i = 0; i < hid.dwCount; i++) {
        TouchpadReaderOnReport(*dev, (PCHAR)hid.bRawData + (size_t)i * hid.dwSizeHid, hid.dwSizeHid);
    }
}

// A handle can be reused by another device, and a device that goes away takes the
// fingers on it with it.
static void TouchpadReaderOnDeviceChange(HANDLE hDevice, WPARAM change) {
    Wh_Log(L"SWS touchpad reader: device change=%u device=%p", (UINT)change, hDevice);
    // WM_INPUT_DEVICE_CHANGE reports both arrivals and removals. An arrival can
    // occur just after registration (and, on some drivers, when the first
    // report is enabled). Treating it as a removal erased the primed touchpad
    // and published available=0 in the gap before that first report.
    //
    // Preserve a primed descriptor on arrival. Parse new devices now so their
    // first swipe need not supply the report that establishes readiness. Retry
    // a descriptor which was unavailable before the arrival notification.
    if (change == GIDC_ARRIVAL) {
        auto device = g_touchpadDevices.find(hDevice);
        if (device != g_touchpadDevices.end() && !device->second.valid) {
            g_touchpadDevices.erase(device);
        }
        if (TouchpadDeviceFor(hDevice)) {
            TouchpadReaderSetAvailable(true);
        } else {
            auto failed = g_touchpadDevices.find(hDevice);
            if (failed != g_touchpadDevices.end() && !failed->second.valid) {
                g_touchpadDevices.erase(failed);
            }
        }
        return;
    }
    if (change != GIDC_REMOVAL) return;
    auto device = g_touchpadDevices.find(hDevice);
    if (device == g_touchpadDevices.end()) return;
    s_touchpadReaderStroke = {};
    ClearRawSwipeMarkerPropertyFromReader();
    ClearRawThreeFingerCandidateProperty();
    g_touchpadRawTwoFingerTapMouseState.store(0, std::memory_order_release);
    g_touchpadDevices.erase(device);
    g_touchpadFrame.clear();
    g_touchpadFrameExpected = 0;
    UpdateNativeCandidateRequirement();
    if (g_touchpadReaderAvailable.load()) {
        PublishNativeSwipePolicy();
    }
    bool anyValid = std::any_of(g_touchpadDevices.begin(), g_touchpadDevices.end(),
        [](const auto& entry) { return entry.second.valid; });
    TouchpadReaderSetAvailable(anyValid);
}

static std::wstring FormatTouchpadInputDiagnostics() {
    auto& stats = s_touchpadInputDiagnostics;
    ULONGLONG now = GetTickCount64();
    auto age = [now](ULONGLONG tick) -> long long {
        // The reader can publish a newer tick while this UI snapshot is read.
        return tick ? (tick <= now ? (long long)(now - tick) : 0) : -1;
    };
    UINT count = 0;
    DWORD registrationError = ERROR_SUCCESS, flags = 0;
    HWND registeredTarget = nullptr;
    if (GetRegisteredRawInputDevices(nullptr, &count, sizeof(RAWINPUTDEVICE)) == (UINT)-1) {
        registrationError = GetLastError();
    } else if (count) {
        std::vector<RAWINPUTDEVICE> devices(count);
        if (GetRegisteredRawInputDevices(devices.data(), &count, sizeof(RAWINPUTDEVICE)) == (UINT)-1) {
            registrationError = GetLastError();
        } else {
            for (const auto& device : devices) {
                if (device.usUsagePage == SWS_HID_PAGE_DIGITIZER &&
                    device.usUsage == SWS_HID_USAGE_TOUCHPAD) {
                    registeredTarget = device.hwndTarget;
                    flags = device.dwFlags;
                }
            }
        }
    }
    WCHAR text[768];
    swprintf_s(text,
        L"raw=%u reports=%u rejectedDevices=%u readFailures=%u frames=%u contactFrames=%u hybridReports=%u emptyReports=%u "
        L"posted=%u postFailures=%u uiFrames=%u "
        L"rawAgeMs=%lld frameAgeMs=%lld uiAgeMs=%lld tips=%u readError=%u postError=%u "
        L"reader=%p registeredTarget=%p registrationFlags=0x%X registrationError=%u enabled=%d available=%d stopping=%d",
        stats.rawMessages.load(), stats.hidReports.load(), stats.rejectedDevices.load(), stats.readFailures.load(),
        stats.frames.load(), stats.contactFrames.load(), stats.hybridReports.load(), stats.emptyReports.load(),
        stats.posted.load(), stats.postFailures.load(), stats.uiFrames.load(),
        age(stats.lastRawTick.load()), age(stats.lastFrameTick.load()), age(stats.lastUiTick.load()),
        stats.lastTips.load(), stats.lastReadError.load(), stats.lastPostError.load(),
        g_touchpadReaderWindow.load(), registeredTarget, flags, registrationError,
        g_touchpadGesturesEnabled.load(), g_touchpadReaderAvailable.load(), g_touchpadReaderStopping.load());
    return text;
}

// "One registration per process" check, so a leftover registration is visible in the
// log before the reader adds its own.
static void TouchpadReaderLogRegistrations(const WCHAR* tag) {
    UINT count = 0;
    GetRegisteredRawInputDevices(NULL, &count, sizeof(RAWINPUTDEVICE));
    if (!count) {
        Wh_Log(L"SWS touchpad reader: %s: no raw input devices registered in this process", tag);
        return;
    }
    std::vector<RAWINPUTDEVICE> devices(count);
    if (GetRegisteredRawInputDevices(devices.data(), &count, sizeof(RAWINPUTDEVICE)) == (UINT)-1) {
        return;
    }
    for (const RAWINPUTDEVICE& rid : devices) {
        Wh_Log(L"SWS touchpad reader: %s: usagePage=0x%X usage=0x%X flags=0x%X target=%p", tag, rid.usUsagePage,
               rid.usUsage, rid.dwFlags, rid.hwndTarget);
    }
}

static LRESULT CALLBACK TouchpadReaderWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SWS_TOUCHPAD_READER_DIAGNOSTICS:
            Wh_Log(L"SWS: reader-thread input snapshot event=0x%X replyMs=%u foreground=%p %s",
                   (UINT)wParam, (DWORD)GetTickCount64() - (DWORD)lParam,
                   GetForegroundWindow(), FormatTouchpadInputDiagnostics().c_str());
            return 0;
        case WM_INPUT:
            TouchpadReaderOnRawInput((HRAWINPUT)lParam);
            break;
        case WM_INPUT_DEVICE_CHANGE:
            TouchpadReaderOnDeviceChange((HANDLE)lParam, wParam);
            return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI TouchpadReaderThread(LPVOID) {
    s_touchpadReaderStroke = {};
    // Same thread setup as the reference mod: PMv2 DPI for the window and high priority
    // so a busy frame cannot delay report parsing. The DPI call is resolved dynamically
    // to stay independent of the SDK's DPI headers.
    using SetThreadDpiAwarenessContext_t = HANDLE(WINAPI*)(HANDLE);
    static auto pSetThreadDpiAwarenessContext = (SetThreadDpiAwarenessContext_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext");
    if (pSetThreadDpiAwarenessContext) {
        pSetThreadDpiAwarenessContext((HANDLE)-4); // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    }
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    HINSTANCE hInstance = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&TouchpadReaderWndProc, &hInstance);

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = TouchpadReaderWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = SWS_TOUCHPAD_READER_CLASSNAME;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"SWS touchpad reader: RegisterClassEx failed (%u)", GetLastError());
        if (g_hTouchpadReaderReadyEvent) SetEvent(g_hTouchpadReaderReadyEvent);
        return 0;
    }

    // A hidden top-level window rather than a message-only one: raw input in the
    // background is not reliably delivered to message-only windows.
    HWND hReaderWnd = CreateWindowExW(WS_EX_TOOLWINDOW, SWS_TOUCHPAD_READER_CLASSNAME, L"", WS_POPUP, 0, 0,
                                      0, 0, NULL, NULL, hInstance, NULL);
    if (!hReaderWnd) {
        Wh_Log(L"SWS touchpad reader: CreateWindowEx failed (%u)", GetLastError());
        UnregisterClassW(SWS_TOUCHPAD_READER_CLASSNAME, hInstance);
        if (g_hTouchpadReaderReadyEvent) SetEvent(g_hTouchpadReaderReadyEvent);
        return 0;
    }

    g_touchpadReaderWindow.store(hReaderWnd);
    Wh_Log(L"SWS touchpad reader: host context %s",
           FormatTouchpadForegroundDiagnostic(GetForegroundWindow(), g_hSwitcher).c_str());

    TouchpadReaderLogRegistrations(L"before register");
    RAWINPUTDEVICE rid = {};
    rid.usUsagePage = SWS_HID_PAGE_DIGITIZER;
    rid.usUsage = SWS_HID_USAGE_TOUCHPAD;
    rid.dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY;
    rid.hwndTarget = hReaderWnd;
    BOOL registered = RegisterRawInputDevices(&rid, 1, sizeof(rid));
    if (registered) {
        TouchpadReaderLogRegistrations(L"after register");
        TouchpadReaderPrimeDevices();
    } else {
        Wh_Log(L"SWS touchpad reader: RegisterRawInputDevices failed (%u)", GetLastError());
    }
    // Signal after registration and descriptor priming, not after the first
    // physical report. Settings changes can then publish a ready policy before
    // the next gesture reaches Explorer.
    if (g_hTouchpadReaderReadyEvent) SetEvent(g_hTouchpadReaderReadyEvent);
    Wh_Log(L"SWS touchpad reader: initial input snapshot %s", FormatTouchpadInputDiagnostics().c_str());

    MSG msg;
    while (registered) {
        DWORD wake = MsgWaitForMultipleObjectsEx(1, &g_hTouchpadReaderStopEvent, INFINITE, QS_ALLINPUT,
                                                 MWMO_INPUTAVAILABLE);
        if (wake != WAIT_OBJECT_0 + 1) {
            break; // stop event signaled, or the wait failed
        }
        bool quit = false;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quit) {
            break;
        }
    }

    if (registered) {
        rid.dwFlags = RIDEV_REMOVE;
        rid.hwndTarget = NULL;
        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }
    g_touchpadDevices.clear();
    TouchpadReaderSetAvailable(false);
    g_touchpadFrame.clear();
    g_touchpadFrameExpected = 0;
    g_touchpadReaderWindow.store(nullptr);
    DestroyWindow(hReaderWnd);
    UnregisterClassW(SWS_TOUCHPAD_READER_CLASSNAME, hInstance);
    Wh_Log(L"SWS touchpad reader: stopped");
    return 0;
}

static bool TouchpadRawInputRequested() {
    return LoadBoolSetting(L"Touchpad.enabled", true);
}

static void StartTouchpadReader() {
    if (g_hTouchpadReaderThread) {
        return;
    }
    g_touchpadReaderStopping.store(false);
    g_hTouchpadReaderStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_hTouchpadReaderStopEvent) {
        return;
    }
    g_hTouchpadReaderReadyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_hTouchpadReaderReadyEvent) {
        CloseHandle(g_hTouchpadReaderStopEvent);
        g_hTouchpadReaderStopEvent = NULL;
        return;
    }
    g_hTouchpadReaderThread = CreateThread(NULL, 0, TouchpadReaderThread, NULL, 0, NULL);
    if (!g_hTouchpadReaderThread) {
        CloseHandle(g_hTouchpadReaderReadyEvent);
        g_hTouchpadReaderReadyEvent = NULL;
        CloseHandle(g_hTouchpadReaderStopEvent);
        g_hTouchpadReaderStopEvent = NULL;
        return;
    }
    Wh_Log(L"SWS touchpad reader: started");
    if (WaitForSingleObject(g_hTouchpadReaderReadyEvent, 1000) == WAIT_TIMEOUT) {
        Wh_Log(L"SWS touchpad reader: startup readiness timed out; keeping native policy fail-open");
    }
}

static void StopTouchpadReader() {
    g_touchpadReaderStopping.store(true);
    bool wasAvailable = g_touchpadReaderAvailable.exchange(false);
    PublishNativeSwipePolicy();
    ClearRawSwipeMarkerPropertyFromReader();
    if (wasAvailable && g_hSwitcher) {
        PostMessageW(g_hSwitcher, WM_SWS_TOUCHPAD_READER_CHANGED, 0, 0);
    }
    // Reader-owned frame/stroke storage is reset only after its thread stops.
    if (!g_hTouchpadReaderThread) {
        TouchpadReaderSetAvailable(false);
        if (g_hTouchpadReaderReadyEvent) {
            CloseHandle(g_hTouchpadReaderReadyEvent);
            g_hTouchpadReaderReadyEvent = NULL;
        }
        return;
    }
    Wh_Log(L"SWS touchpad reader: stopping");
    SetEvent(g_hTouchpadReaderStopEvent);
    WaitForSingleObject(g_hTouchpadReaderThread, INFINITE);
    CloseHandle(g_hTouchpadReaderThread);
    g_hTouchpadReaderThread = NULL;
    CloseHandle(g_hTouchpadReaderStopEvent);
    g_hTouchpadReaderStopEvent = NULL;
    if (g_hTouchpadReaderReadyEvent) {
        CloseHandle(g_hTouchpadReaderReadyEvent);
        g_hTouchpadReaderReadyEvent = NULL;
    }
    Wh_Log(L"SWS touchpad reader: stopped input snapshot %s", FormatTouchpadInputDiagnostics().c_str());
    TouchpadReaderSetAvailable(false);
}

// Tool Mod callbacks

BOOL WhTool_ModInit() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModInit");
    Wh_Log(L"SWS TAPTRACE diagnostics build 2: live two-finger override and mouse routing capture");
    if (!g_WM_SWS_TOUCHPAD_FRAME) {
        g_WM_SWS_TOUCHPAD_FRAME = RegisterWindowMessageW(L"Windhawk_SWS_TouchpadFrame");
    }
    bool touchpadEnabled = TouchpadRawInputRequested();
    g_touchpadGesturesEnabled.store(touchpadEnabled);
    g_touchpadStickyLaunchEnabled.store(
        LoadBoolSetting(L"Touchpad.stickyLaunch", true));
    PublishNativeSwipePolicy();
    // Seed the cross-thread touchpad snapshot before the switcher thread can
    // load its full UI settings. Starting the thread first allowed its initial
    // LoadSettings call to temporarily overwrite stickyLaunch with the old
    // value, producing one native upward swipe that went to Task View after a
    // settings reload.
    g_hSwitcherThread = CreateThread(NULL, 0, SwitcherThread, NULL, 0, &g_dwSwitcherThreadId);
    if (touchpadEnabled) {
        StartTouchpadReader();
    }
    return g_hSwitcherThread != NULL;
}

void WhTool_ModUninit() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModUninit");
    RequestWindowIconWorkerStop();
    // Controller teardown is performed by its owning switcher thread before
    // apartment teardown. Never race startup/settings with a cross-thread release.
    // The reader owns a window class and a raw input registration: stop and join it
    // before anything else is torn down.
    g_touchpadGesturesEnabled.store(false);
    g_touchpadSwitcherActive.store(false);
    PublishNativeSwipePolicy();
    StopTouchpadReader();
    while (g_dwSwitcherThreadId &&
           !PostThreadMessage(g_dwSwitcherThreadId, WM_QUIT, 0, 0)) {
        if (GetLastError() != ERROR_INVALID_THREAD_ID) break;
        if (WaitForSingleObject(g_hSwitcherThread, 10) != WAIT_TIMEOUT) break;
    }
    if (g_hSwitcherThread) {
        WaitForSingleObject(g_hSwitcherThread, INFINITE);
        CloseHandle(g_hSwitcherThread);
        g_hSwitcherThread = NULL;
        g_dwSwitcherThreadId = 0;
    }
    if (g_hDwmCornerWatchStopEvent) {
        SetEvent(g_hDwmCornerWatchStopEvent);
    }
    if (g_hDwmCornerWatchThread) {
        WaitForSingleObject(g_hDwmCornerWatchThread, 1000);
        CloseHandle(g_hDwmCornerWatchThread);
        g_hDwmCornerWatchThread = NULL;
    }
    if (g_hDwmCornerWatchStopEvent) {
        CloseHandle(g_hDwmCornerWatchStopEvent);
        g_hDwmCornerWatchStopEvent = NULL;
    }
    for (auto& pair : g_uwpIconCache) {
        if (pair.second) DestroyIcon(pair.second);
    }
    g_uwpIconCache.clear();
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModSettingsChanged");
    bool touchpadEnabled = TouchpadRawInputRequested();
    // Publish the new cross-thread snapshot before starting/stopping the reader
    // or allowing a native callback to arrive. UI-only settings are reloaded by
    // WM_SWS_SETTINGS_CHANGED on the switcher thread.
    g_touchpadGesturesEnabled.store(touchpadEnabled);
    g_touchpadStickyLaunchEnabled.store(
        LoadBoolSetting(L"Touchpad.stickyLaunch", true));
    PublishNativeSwipePolicy();
    if (touchpadEnabled) {
        StartTouchpadReader();
    } else {
        StopTouchpadReader();
    }
    if (g_hSwitcher) {
        PostMessage(g_hSwitcher, WM_SWS_SETTINGS_CHANGED, 0, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod boilerplate
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk lifecycle

static bool IsMainExplorer() {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTaskbar) {
        DWORD trayPid = 0;
        GetWindowThreadProcessId(hTaskbar, &trayPid);
        if (trayPid != GetCurrentProcessId()) {
            return false;
        }
    }
    return true;
}



BOOL Wh_ModInit() {
    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    WCHAR* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;
    Wh_Log(L"SWS: Wh_ModInit in process: %s", exeName);

    // Cache Windows version once so callers don't need to read the registry repeatedly.
    {
        DWORD sz = 0;
        RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                     L"CurrentBuildNumber", RRF_RT_REG_SZ, NULL, NULL, &sz);
        if (sz > 0) {
            std::wstring buf(sz / sizeof(WCHAR), L'\0');
            if (RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                             L"CurrentBuildNumber", RRF_RT_REG_SZ, NULL, &buf[0], &sz) == ERROR_SUCCESS) {
                g_isWin11OrGreater = (_wtoi(buf.c_str()) >= 22000);
            }
        }
    }

    // --- explorer.exe path: hook RegisterHotKey, ShowWindow, and twinui.pcshell.dll ---
    if (_wcsicmp(exeName, L"explorer.exe") == 0) {
        g_isExplorer = true;
        Wh_Log(L"SWS: Loaded into explorer.exe, setting up hooks");

        if (!g_WM_SWS_GET_UWP_ICON) {
            g_WM_SWS_GET_UWP_ICON = RegisterWindowMessageW(L"Windhawk_SWS_GetUwpIcon");
        }

        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            void* pRegisterHotKey = (void*)GetProcAddress(hUser32, "RegisterHotKey");
            if (pRegisterHotKey) {
                Wh_SetFunctionHook(pRegisterHotKey, (void*)RegisterHotKey_Hook, (void**)&RegisterHotKey_Original);
            }
            void* pShowWindow = (void*)GetProcAddress(hUser32, "ShowWindow");
            if (pShowWindow) {
                Wh_SetFunctionHook(pShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
            }
        }

        // These private member-function signatures are not stdcall on x86.
        // The ShowWindow filter remains available there without an ABI guess.
#if !defined(_M_IX86)
        HMODULE hTwinui = LoadLibraryW(L"twinui.pcshell.dll");
        if (hTwinui) {
            // Queue the same complete hook set on both initial and retry paths.
            // Windhawk applies initial hooks after Wh_ModInit returns.
            if (!QueueTwinuiShellHooks(hTwinui)) {
                Wh_Log(L"SWS: twinui initial symbol lookup failed or still loading");
            }
            Wh_Log(L"SWS: twinui hooks queued (Alt+Tab=%d)",
                   XamlAltTabViewHost_Show_Original || CAltTabViewHost_Show_Original);
        }

#endif
        // Check if Explorer has already registered standard hotkeys.
        // We use Alt+Tab as a probe. If it fails, Explorer is mid-session and already owns it.
        // We only do this for the main Explorer process to avoid false prompts in secondary Explorers.
        if (!GetSystemMetrics(SM_SHUTTINGDOWN) && IsMainExplorer()) {
            Wh_Log(L"SWS: Checking if Explorer is mid-session -> probing Alt+Tab");
            if (!RegisterHotKey(NULL, 0x1337, MOD_ALT, VK_TAB)) {
                HANDLE hMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, L"Windhawk_SWS_HotkeyMutex");
                bool isToolModHolding = false;
                if (hMutex) {
                    if (WaitForSingleObject(hMutex, 0) == WAIT_TIMEOUT) {
                        isToolModHolding = true;
                    } else {
                        ReleaseMutex(hMutex);
                    }
                    CloseHandle(hMutex);
                }

                if (isToolModHolding) {
                    Wh_Log(L"SWS: Alt+Tab failed, but tool mod holds mutex -> skipping prompt");
                } else {
                    Wh_Log(L"SWS: Alt+Tab failed, tool mod does NOT hold mutex -> Explorer is mid-session, prompting");
                    PromptForExplorerRestart();
                }
            } else {
                Wh_Log(L"SWS: Alt+Tab succeeded -> Explorer hasn't registered it, skipping prompt");
                UnregisterHotKey(NULL, 0x1337);
            }
        } else {
            Wh_Log(L"SWS: System shutting down or secondary explorer, skipping prompt");
        }

        return TRUE;
    }

    // --- windhawk.exe path: tool mod boilerplate ---
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isExplorer) {
        g_explorerIpcThread = CreateThread(NULL, 0, ExplorerIpcThread, NULL, 0, &g_explorerIpcThreadId);
        return;
    }
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isExplorer) {
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModBeforeUninit() {
    if (g_isExplorer) {
        s_nativeSwipeShutdownRequested.store(true);
        s_nativeSwipeHooksAvailable.store(false);
        PublishNativeSwipeHookStatus(false);
        // Stop late symbol retries before Windhawk removes hooks. Uninit is too
        // late: its thread must not queue/apply another detour during teardown.
        while (g_explorerIpcThreadId &&
               !PostThreadMessage(g_explorerIpcThreadId, WM_QUIT, 0, 0)) {
            if (GetLastError() != ERROR_INVALID_THREAD_ID) break;
            if (WaitForSingleObject(g_explorerIpcThread, 10) != WAIT_TIMEOUT) break;
        }
        if (g_explorerIpcThread) {
            WaitForSingleObject(g_explorerIpcThread, INFINITE);
            CloseHandle(g_explorerIpcThread);
            g_explorerIpcThread = NULL;
            g_explorerIpcThreadId = 0;
        }
        s_nativeSwipeHooksAvailable.store(false);
        PublishNativeSwipeHookStatus(false);
    }
}

void Wh_ModUninit() {
    if (g_isExplorer) {
        HWND promptWnd = g_restartExplorerPromptWindow;
        if (promptWnd) PostMessage(promptWnd, WM_CLOSE, 0, 0);

        for (auto& pair : g_uwpIconCache) {
            if (pair.second) DestroyIcon(pair.second);
        }
        g_uwpIconCache.clear();

        if (g_isExplorer && IsMainExplorer()) {
            if (!GetSystemMetrics(SM_SHUTTINGDOWN)) {
                PromptForExplorerRestart();
            }
        }

        if (g_restartExplorerPromptThread) {
            if (WaitForSingleObject(g_restartExplorerPromptThread, 30000) == WAIT_TIMEOUT) {
                HWND wnd = g_restartExplorerPromptWindow;
                if (wnd) PostMessage(wnd, WM_CLOSE, 0, 0);
                WaitForSingleObject(g_restartExplorerPromptThread, INFINITE);
            }
            CloseHandle(g_restartExplorerPromptThread);
            g_restartExplorerPromptThread = NULL;
        }
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    // g_pVirtualDesktopManager is created, used, and released on the switcher
    // thread (see SwitcherThread); WhTool_ModUninit joins that thread, which
    // releases it before CoUninitialize.
    WhTool_ModUninit();
    ExitProcess(0);
}
