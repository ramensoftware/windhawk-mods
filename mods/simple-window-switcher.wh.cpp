// ==WindhawkMod==
// @id              simple-window-switcher
// @name            Simple Window Switcher
// @description     Replaces the default Alt+Tab with a lightweight window switcher inspired by ExplorerPatcher's Simple Window Switcher
// @version         3.0
// @author          Lone
// @github          https://github.com/Louis047
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -O2 -ldwmapi -luxtheme -lgdi32 -lshlwapi -loleaut32 -lole32 -lcomctl32 -lgdiplus -lversion -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Simple Window Switcher
A lightweight Alt+Tab replacement for Windows, ported from the [Simple Window Switcher](https://github.com/valinet/sws) project.
Additional improvements made by [Asteski](https://github.com/Asteski) and [bropines](https://github.com/bropines).

## Features
- Grid layout with live DWM thumbnail previews
- Different Task List, Header Content and Thumbnails layouts
- Center align task list content and titles (horizontal and vertical options)
- Keyboard navigation (Tab/Shift+Tab/Shift/Backtick, Arrow keys, Enter, Esc)
- Mouse click to select, scroll wheel to cycle from anywhere
- Scroll wheel page navigation (hold a secondary modifier to switch pages instead of individual windows)
- Virtual Desktop Support (defaults to showing windows from all virtual desktops — change under Accessibility → Virtual Desktop Behavior)
- Group windows by application (macOS Cmd+Tab style, one entry per app)
- Drill into an app's windows with a Ctrl tap to pick a specific window (Esc backs out)
- Win+Alt+Tab override to display windows from all monitors when using Per-Monitor mode
- Alt+Ctrl+Tab sticky mode
- Alt+` shortcut to cycle backwards or filter to windows of the same active application
- Keyboard shortcut to close the selected window (Q, Ctrl+W, or Del)
- Directional overflow chevrons (▲/▼/◀/▶) at the edges when windows extend off-screen
- Show/hide close button toggle under Appearance → Thumbnails
- Theme support (None/Backdrop Acrylic) with fully customizable background opacity
- Works with elevated/admin applications
- Dark/light mode auto-detection
- Custom border colors with optional Windows accent color
- Different item highlight options
- DPI-aware, multi-monitor aware
- Rounded corners for switcher and task thumbnails (optional)
- Dynamic UI adjustments (e.g., intelligent close button placement over thumbnails)
- Highly reliable Explorer restart prompt handling without infinite loops

## Screenshots

| Horizontal squared (default) | Horizontal squared without thumbnails |
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
        - indicatorBgColorMode: default
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
        - indicatorBgColorMode: default
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
        - cornerPreference: auto
          $name: Corner Preference
          $description: Corner radius for the switcher window and its elements.
          $options:
          - auto: Auto (Squared on Windows 10, Rounded on Windows 11)
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
        - showBadgeIconBackground: true
          $name: Show Badge Icon Background
          $description: Draw a backdrop shape behind the badge icon. If off, a drop shadow is drawn instead.
        - showBadgeIconBackgroundShadow: false
          $name: Show Badge Icon Background Shadow
          $description: Draw a soft drop shadow under the badge icon background pill.
        - badgeIconPadding: 4
          $name: Badge Icon Padding (px)
          $description: Extra space between the icon and the edge of its background.
        - badgeIconOffsetX: 0
          $name: Badge Icon Offset X (px)
          $description: Nudge the icon horizontally from its default position.
        - badgeIconOffsetY: 0
          $name: Badge Icon Offset Y (px)
          $description: Nudge the icon vertically from its default position.
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
      $name: Switcher Window Padding (px)
      $description: Padding between the switcher window border and the window entries in pixels (before DPI scaling). Default 20.
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
    - showGroupIndicatorShadow: false
      $name: Show Group Indicator Shadow
      $description: Show a soft drop shadow behind the group indicator badge.
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
      $description: Delay in milliseconds before showing the switcher (0 = instant).
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
      $description: Shortcut used to move backward in the switcher.
      $options:
      - altShiftTab: Alt+Shift+Tab (default)
      - altShift: Alt+Shift
      - altBacktick: Alt+Backtick
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
  $name: Accessibility
- ExcludedWindows:
    - excludeByTitle: ""
      $name: Exclude by Window Title
      $description: "Window title patterns to exclude, separated by ';' (wildcards supported: * matches any characters, ? matches one). Example: *Notepad*;*Chrome*"
    - excludeByExe: ""
      $name: Exclude by Executable Name
      $description: "Executable name patterns to exclude, separated by ';' (wildcards supported: * matches any characters, ? matches one). Example: notepad.exe;chrome.exe"
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
#include <appmodel.h>
#include <vector>
#include <atomic>
#include <map>
#include <string>
#include <algorithm>
#include <gdiplus.h>

#define SWS_CLASSNAME       L"WindhawkSWS_Switcher"
#define SWS_ICON_SIZE       16
// Lower bound (pre-DPI px) for the auto-fit "Shrink tasks to fit" row height so
// thumbnails never collapse to an unusable size.
#define SWS_AUTOFIT_MIN_ROWHEIGHT 90
// EP-style nested padding layers (before DPI scaling)
#define SWS_MASTER_PADDING      20  // Outer margin of the entire switcher window
#define SWS_ELEMENT_PAD_TOP     5   // Vertical margin between cell border and content
#define SWS_ELEMENT_PAD_BOTTOM  5
#define SWS_ELEMENT_PAD_LEFT    2   // Horizontal margin between cell border and content
#define SWS_ELEMENT_PAD_RIGHT   2
#define SWS_PAD_TOP             7   // Inner distance from content area to thumbnail
#define SWS_PAD_BOTTOM          7
#define SWS_PAD_LEFT            7
#define SWS_PAD_RIGHT           7
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
#define SWS_ANIM_TIMER_ID       104
// Posted by the low-level mouse hook so the heavy CycleLinear work runs in the
// wndproc instead of on the synchronous raw-input path. WPARAM is the direction.
#define WM_SWS_SCROLL           (WM_APP + 1)
#define WM_SWS_SETTINGS_CHANGED (WM_APP + 2)

typedef BOOL (WINAPI *IsShellWindow_t)(HWND);
typedef HWND (WINAPI *GhostWindowFromHungWindow_t)(HWND);
struct ACCENT_POLICY { DWORD AccentState; DWORD AccentFlags; DWORD GradientColor; DWORD AnimationId; };
struct WINDOWCOMPOSITIONATTRIBDATA { DWORD dwAttrib; PVOID pvData; SIZE_T cbData; };
typedef BOOL(WINAPI *SetWindowCompositionAttribute_t)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

struct WindowEntry {
    HWND hWnd; HICON hIcon; WCHAR title[256]; std::map<HWND, HTHUMBNAIL> hThumbs;
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
    bool isNewEntry = false;
    float enterAlpha = 1.0f;
    float enterScale = 1.0f;
    float closeBtnAlpha = 0.0f;
    float hoverScale = 1.0f;
    float hoverScaleStart = 1.0f;
    float hoverScaleTarget = 1.0f;
    float hoverScaleProgress = 1.0f;
    float hoverScaleDuration = 0.150f;
};
struct Settings {
    WCHAR theme[32]; WCHAR colorScheme[32]; WCHAR cornerPreference[32]; WCHAR scrollWheelBehavior[32]; WCHAR scrollWheelAction[32]; WCHAR scrollSecondaryAction[32]; WCHAR scrollSecondaryModifier[32]; WCHAR taskListOrientation[32]; WCHAR headerContentOrientation[32]; WCHAR iconSize[32]; WCHAR backwardShortcut[32]; WCHAR altBacktickBehavior[32]; WCHAR thumbnailPosition[32]; WCHAR thumbnailAlignment[32]; WCHAR switcherDisplayBehavior[32];
    WCHAR virtualDesktopBehavior[32];
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
    int entryPadding;
    bool perMonitorWindows; bool taskRoundedCorners; bool roundThumbnailCorners; bool roundGroupIndicator; bool roundBadgeIconBackground; bool reverseScrollDirection;
    bool centerTaskContent;
    bool showApplications;
    WCHAR showTitles[32];
    bool restoreAllWindows;
    bool hideMinimizedWindows;
    bool sortMinimizedWindowsToEnd;
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
};

static std::vector<std::wstring> g_excludeTitlePatterns;
static std::vector<std::wstring> g_excludeExePatterns;
static std::vector<HWND> g_hMirrorSwitchers;

static HWND g_hSwitcher = NULL;
static HWND g_hCloseBtnWnd = NULL;

static bool IsSwitcherWindow(HWND hWnd) {
    if (!hWnd) return false;
    if (hWnd == g_hSwitcher || hWnd == g_hCloseBtnWnd) return true;
    for (HWND h : g_hMirrorSwitchers) {
        if (hWnd == h) return true;
    }
    return false;
}
static IVirtualDesktopManager* g_pVirtualDesktopManager = NULL;
static bool g_showAllMonitors = false;
static HHOOK g_hMouseHook = NULL;
static HWINEVENTHOOK s_hWinEventHook = NULL;
static void AddWindowEntry(HWND hWnd);
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
static bool g_isVisible = false, g_isSticky = false, g_isDarkMode = false;
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
static HMONITOR g_hCurrentMonitor = NULL;
static Settings g_settings;
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
static bool g_nativeBackdropActive = false;
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

// Forward declarations
static void LoadSettings();
static void SWS_RegisterHotkeys();
static void SWS_UnregisterHotkeys();
static void ApplySwitcherRegion();
static void ApplyThemeToWindow(HWND hWnd);
static void CreateMirrorSwitchers();
static void HideSwitcher();
static void PaintSwitcher();
static void PaintSwitcherOverlay();
static INT GetCornerPref();
static int GetWindowCornerRadiusPx();
static void DrawSwitcherStaticContent(HDC hdc, bool fillBg, HWND hWnd);
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
static void UpdateChevronAnimationTargets(bool immediate = false);
static int HitTestChevron(HWND hWnd, int x, int y);

static inline int DpiScale(int val, int dpi) { return MulDiv(val, dpi, 96); }

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
static bool ScrollIs(const WCHAR* v) { return wcscmp(g_settings.scrollWheelBehavior, v) == 0; }
static bool LayoutIsVertical() { return wcscmp(g_settings.taskListOrientation, L"vertical") == 0; }
static bool HeaderOrientationIs(const WCHAR* v) { return wcscmp(g_settings.headerContentOrientation, v) == 0; }
static bool IconSizeIs(const WCHAR* v) { return wcscmp(g_settings.iconSize, v) == 0; }
static bool BackwardShortcutIs(const WCHAR* v) { return wcscmp(g_settings.backwardShortcut, v) == 0; }
static bool UseAltShiftTabBackward() { return BackwardShortcutIs(L"altShiftTab"); }
static bool UseAltShiftBackward() { return BackwardShortcutIs(L"altShift"); }
static bool UseAltBacktickBackward() { return BackwardShortcutIs(L"altBacktick"); }
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
static bool BadgeIconPositionIs(const WCHAR* v) { return wcscmp(g_settings.badgeIconPosition, v) == 0; }
static bool BadgeTitleIsTop() { return wcscmp(g_settings.badgeTitlePosition, L"top") == 0; }
static int GetHeaderIconSizeBase() {
    if (DockLayoutActive()) return g_settings.dockIconSize > 0 ? g_settings.dockIconSize : 48;
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

static CubicBezierEasing g_easeEntrance(0.1f, 0.9f, 0.2f, 1.0f);
static CubicBezierEasing g_easeSlide(0.1f, 0.9f, 0.2f, 1.0f);
static CubicBezierEasing g_easeHover(0.1f, 0.9f, 0.2f, 1.0f);
static CubicBezierEasing g_easeHoverEnter(0.0f, 0.0f, 0.2f, 1.0f);
static CubicBezierEasing g_easeSelection(0.1f, 0.9f, 0.2f, 1.0f);
static CubicBezierEasing g_easeExit(0.7f, 0.0f, 1.0f, 0.5f);

static bool g_animActive = false;
static LARGE_INTEGER g_animPerfFreq = {};
static LARGE_INTEGER g_animLastTickTime = {};

// Exit animation state (160ms WinUI 3 FastOutLinearInKeySpline + 16px sink)
static bool g_animExitActive = false;
static float g_animExitProgress = 1.0f;
static float g_animExitDuration = 0.160f;
static float g_animExitCurrentAlpha = 1.0f;

// Animation target frame interval (updated from DWM composition refresh rate)
static double s_animTargetIntervalMs = 1000.0 / 60.0;

static void UpdateRefreshRateTiming() {
    double hz = 0.0;
    HWND targetWnd = g_hSwitcher ? g_hSwitcher : GetDesktopWindow();
    HMONITOR hMon = MonitorFromWindow(targetWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMon, &mi)) {
        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) && dm.dmDisplayFrequency >= 50) {
            hz = (double)dm.dmDisplayFrequency;
        }
    }
    if (hz <= 0.0) {
        DWM_TIMING_INFO ti = {};
        ti.cbSize = sizeof(ti);
        if (SUCCEEDED(DwmGetCompositionTimingInfo(NULL, &ti)) && ti.rateRefresh.uiDenominator > 0) {
            hz = (double)ti.rateRefresh.uiNumerator / (double)ti.rateRefresh.uiDenominator;
        }
    }
    if (hz >= 50.0 && hz <= 360.0) {
        s_animTargetIntervalMs = 1000.0 / hz;
    }
}

// Entrance animation state (240ms WinUI 3 Control Spline + 28px glide)
static bool g_animEntranceActive = false;
static float g_animEntranceProgress = 1.0f;
static float g_animEntranceDuration = 0.240f;
static float g_animEntranceCurrentAlpha = 1.0f;

// Selection focus animation state (WinUI 3 Decelerate cubic-bezier, consistent with inner hover border)
static bool g_animSelectionActive = false;
static RectF g_animSelectionStart = {};
static RectF g_animSelectionTarget = {};
static RectF g_animSelectionCurrent = {};
static float g_animSelectionProgress = 1.0f;
static float g_animSelectionDuration = 0.110f;

static inline void SnapSelectionTo(const RectF& r) {
    g_animSelectionCurrent = r;
    g_animSelectionTarget = r;
    g_animSelectionStart = r;
    g_animSelectionProgress = 1.0f;
    g_animSelectionActive = false;
}

static inline int GetAnimationOffsetY() {
    // WinUI 3 FadeInThemeAnimation / FadeOutThemeAnimation operates purely via opacity
    // in-place. Eliminating physical vertical window translation prevents coordinate
    // desynchronization between non-layered host windows and the layered overlay.
    return 0;
}

// Hover focus animation state (150ms enter/glide, 120ms exit)
static bool g_animHoverActive = false;
static RectF g_animHoverStart = {};
static RectF g_animHoverTarget = {};
static RectF g_animHoverCurrent = {};
static float g_animHoverAlphaStart = 0.0f;
static float g_animHoverAlphaTarget = 0.0f;
static float g_animHoverAlphaCurrent = 0.0f;
static float g_animHoverProgress = 1.0f;
static float g_animHoverDuration = 0.150f;

static inline void SnapHoverTo(const RectF& r) {
    g_animHoverCurrent = r;
    g_animHoverTarget = r;
    g_animHoverStart = r;
    g_animHoverProgress = 1.0f;
    g_animHoverActive = false;
}

// Hover thumbnail zoom animation state (per-entry concurrent WinUI 3 curves)
static constexpr float SWS_HOVER_ZOOM_DELTA = 0.025f; // 2.5% subtle zoom (1.025x), WinUI 3 Fluent standard
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
static float g_animChevronDuration = 0.200f;

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
    std::map<HWND, HTHUMBNAIL> hThumbs;
    int drawnIconX, drawnIconY, drawnIconSz;
    std::vector<HWND> groupWindows;
    SIZE sourceSize;
    RECT rcSourceCrop;
    SIZE effectiveSourceSize;
};

struct ScrollTransitionState {
    bool active = false;
    bool preservingThumbnails = false;
    float offsetStartX = 0.0f;
    float offsetStartY = 0.0f;
    float offsetCurrentX = 0.0f;
    float offsetCurrentY = 0.0f;
    float progress = 1.0f;
    float duration = 0.170f;
    int travelDistanceX = 0;
    int travelDistanceY = 0;
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
    HICON hIcon;
    WCHAR title[256];
    std::map<HWND, HTHUMBNAIL> hThumbs;
    std::vector<HWND> groupWindows;
};

struct LayoutTransitionState {
    bool active = false;
    float progress = 1.0f;
    float duration = 0.280f; // 280ms WinUI 3 RepositionThemeAnimation standard
    RectF rcWndStart = {};
    RectF rcWndTarget = {};
    std::vector<DepartingEntrySnapshot> departingItems;
};
static LayoutTransitionState g_layoutTransition;
static CubicBezierEasing g_easeLayout(0.4f, 0.0f, 0.2f, 1.0f);

struct DockPreviewSlideTransition {
    bool active = false;
    float progress = 1.0f;
    float duration = 0.220f; // 220ms WinUI 3 SlideNavigationTransitionInfo standard
    float travelDistance = 0.0f;
    float currentOffset = 0.0f;
    float currentAlpha = 1.0f;
};
static DockPreviewSlideTransition g_dockPreviewSlide;

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
    if (!g_animActive) {
        if (g_animPerfFreq.QuadPart == 0) {
            QueryPerformanceFrequency(&g_animPerfFreq);
        }
        QueryPerformanceCounter(&g_animLastTickTime);
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
    g_animEntranceActive = false;
    g_animEntranceProgress = 1.0f;
    g_animEntranceCurrentAlpha = 1.0f;
    if (g_layoutTransition.active) {
        g_layoutTransition.active = false;
        g_layoutTransition.progress = 1.0f;
        for (auto& item : g_layoutTransition.departingItems) {
            for (const auto& kv : item.hThumbs) {
                if (kv.second) DwmUnregisterThumbnail(kv.second);
            }
        }
        g_layoutTransition.departingItems.clear();
        for (auto& w : g_windows) {
            w.rcCell = w.rcCellTarget;
            w.rcThumbActual = w.rcThumbTarget;
            w.isNewEntry = false;
        }
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
                if (kv.second) DwmUnregisterThumbnail(kv.second);
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
    g_animChevronProgressPrev = 1.0f;
    g_animChevronProgressNext = 1.0f;
    g_animHoverScaleActive = false;
    g_animCloseBtnAlpha = (g_hoverIndex >= 0 && g_settings.showCloseButton && !IsWindowTruncated(g_hoverIndex)) ? 1.0f : 0.0f;
    g_animCloseBtnHoverAlpha = g_isCloseHovered ? 1.0f : 0.0f;
    for (int i = 0; i < (int)g_windows.size(); i++) {
        g_windows[i].closeBtnAlpha = (i == g_hoverIndex && g_settings.showCloseButton && !IsWindowTruncated(i)) ? 1.0f : 0.0f;
        g_windows[i].hoverScale = 1.0f;
        g_windows[i].hoverScaleStart = 1.0f;
        g_windows[i].hoverScaleTarget = 1.0f;
        g_windows[i].hoverScaleProgress = 1.0f;
        g_windows[i].hoverScaleDuration = 0.150f;
    }
}

static void CaptureOutgoingSnapshot() {
    if (g_hSwitcher && g_staticContentDirty) {
        RECT rc; GetClientRect(g_hSwitcher, &rc);
        int w = rc.right, h = rc.bottom;
        if (w > 0 && h > 0 && s_cachedStaticDC && s_cachedStaticW == w && s_cachedStaticH == h) {
            int radius = GetWindowCornerRadiusPx();
            if (s_cachedStaticBits) memset(s_cachedStaticBits, 0, (size_t)w * h * sizeof(DWORD));
            HRGN hClip = GetCachedRoundRectRgn(w, h, radius);
            SelectClipRgn(s_cachedStaticDC, hClip);
            DrawSwitcherStaticContent(s_cachedStaticDC, ThemeIs(L"none"), g_hSwitcher);
            g_staticContentDirty = false;
        }
    }
    if (g_scrollTransition.active && !g_scrollTransition.outgoingItems.empty()) {
        for (auto& item : g_scrollTransition.outgoingItems) {
            int curIdx = FindWindowIndexByHwnd(item.hWnd);
            if (curIdx == -1 || IsWindowTruncated(curIdx)) {
                for (const auto& kv : item.hThumbs) {
                    if (kv.second) DwmUnregisterThumbnail(kv.second);
                }
                if (curIdx != -1) {
                    g_windows[curIdx].hThumbs.clear();
                }
            }
        }
    }
    g_scrollTransition.outgoingItems.clear();
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
        wcsncpy_s(snap.title, w.title, _countof(snap.title));
        snap.hIcon = w.hIcon;
        snap.hThumbs = w.hThumbs;
        snap.drawnIconX = w.drawnIconX;
        snap.drawnIconY = w.drawnIconY;
        snap.drawnIconSz = w.drawnIconSz;
        snap.groupWindows = w.groupWindows;
        snap.sourceSize = w.sourceSize;
        snap.rcSourceCrop = w.rcSourceCrop;
        snap.effectiveSourceSize = w.effectiveSourceSize;
        g_scrollTransition.outgoingItems.push_back(snap);
    }
    g_scrollTransition.preservingThumbnails = true;
}

static void TriggerSelectionAnimation(int prevSelected) {
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
    g_animSelectionProgress = 0.0f;
    g_animSelectionDuration = 0.110f;
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
        HRGN hClip = GetCachedRoundRectRgn(w, h, radius);
        SelectClipRgn(s_cachedScrollFromDC, hClip);
        DrawSwitcherStaticContent(s_cachedScrollFromDC, true, g_hSwitcher);
    }

    // 2. Pre-render Incoming Canvas: render incoming layout at rest (offset 0,0)
    if (s_cachedScrollToBits) memset(s_cachedScrollToBits, 0, (size_t)w * h * sizeof(DWORD));
    HRGN hClip = GetCachedRoundRectRgn(w, h, radius);
    SelectClipRgn(s_cachedScrollToDC, hClip);
    DrawSwitcherStaticContent(s_cachedScrollToDC, true, g_hSwitcher);
}

static void TriggerScrollAnimationEx(int dir, ScrollNavType type) {
    if (!AreAnimationsGloballyEnabled() || !g_settings.enableScrollAnimation) {
        g_scrollTransition.active = false;
        g_scrollTransition.offsetCurrentX = 0.0f;
        g_scrollTransition.offsetCurrentY = 0.0f;
        for (auto& item : g_scrollTransition.outgoingItems) {
            int curIdx = FindWindowIndexByHwnd(item.hWnd);
            if (curIdx == -1 || IsWindowTruncated(curIdx)) {
                for (const auto& kv : item.hThumbs) {
                    if (kv.second) DwmUnregisterThumbnail(kv.second);
                }
                if (curIdx != -1) {
                    g_windows[curIdx].hThumbs.clear();
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
                if (diffX != 0) {
                    deltaX = (float)diffX;
                    computedFromShared = true;
                    break;
                }
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
                if (vertical && diffX != 0) {
                    deltaX = (float)diffX;
                    computedFromShared = true;
                    break;
                } else if (!vertical && diffY != 0) {
                    deltaY = (float)diffY;
                    computedFromShared = true;
                    break;
                }
            }
        }
    }

    if (!computedFromShared) {
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

    if (g_scrollTransition.active) {
        // Continuous Retargeting: accumulate offset smoothly
        g_scrollTransition.offsetStartX = g_scrollTransition.offsetCurrentX + deltaX;
        g_scrollTransition.offsetStartY = g_scrollTransition.offsetCurrentY + deltaY;
        g_scrollTransition.offsetCurrentX = g_scrollTransition.offsetStartX;
        g_scrollTransition.offsetCurrentY = g_scrollTransition.offsetStartY;
        g_scrollTransition.progress = 0.0f;
        g_scrollTransition.duration = (type == SCROLL_PAGE) ? 0.200f : 0.120f;
    } else {
        g_scrollTransition.offsetStartX = deltaX;
        g_scrollTransition.offsetStartY = deltaY;
        g_scrollTransition.offsetCurrentX = deltaX;
        g_scrollTransition.offsetCurrentY = deltaY;
        g_scrollTransition.progress = 0.0f;
        g_scrollTransition.duration = (type == SCROLL_PAGE) ? 0.240f : 0.170f;
    }

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
            g_windows[i].closeBtnAlpha = (i == g_hoverIndex && g_settings.showCloseButton && !IsWindowTruncated(i)) ? 1.0f : 0.0f;
            float s = (ThumbnailHoverIsZoom() && i == thumbIdx && !IsWindowTruncated(i)) ? (1.0f + SWS_HOVER_ZOOM_DELTA) : 1.0f;
            g_windows[i].hoverScale = s;
            g_windows[i].hoverScaleStart = s;
            g_windows[i].hoverScaleTarget = s;
            g_windows[i].hoverScaleProgress = 1.0f;
            g_windows[i].hoverScaleDuration = 0.150f;
        }
        if (ThumbnailHoverIsZoom()) {
            UpdateThumbnailAnimations();
            PaintSwitcher();
        }
        return;
    }

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
                g_animHoverProgress = 0.0f;
                g_animHoverDuration = 0.120f;
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
            g_animHoverDuration = 0.150f; // 150ms WinUI 3 Decelerate
        } else {
            // Glide between thumbnails
            g_animHoverStart = g_animHoverCurrent;
            g_animHoverTarget = targetRectF;
            g_animHoverAlphaStart = g_animHoverAlphaCurrent;
            g_animHoverAlphaTarget = 1.0f;
            g_animHoverDuration = 0.150f; // 150ms WinUI 3 Decelerate
        }
        g_animHoverProgress = 0.0f;
        g_animHoverActive = true;
        StartAnimationTicker();
    } else {
        if (g_animHoverAlphaCurrent > 0.01f) {
            g_animHoverStart = g_animHoverCurrent;
            g_animHoverTarget = g_animHoverCurrent;
            g_animHoverAlphaStart = g_animHoverAlphaCurrent;
            g_animHoverAlphaTarget = 0.0f;
            g_animHoverProgress = 0.0f;
            g_animHoverDuration = 0.120f; // 120ms WinUI 3 FastOutLinearIn
            g_animHoverActive = true;
            StartAnimationTicker();
        }
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
    float combinedAlpha = g_animEntranceCurrentAlpha * g_animExitCurrentAlpha;
    if (combinedAlpha < 0.0f) combinedAlpha = 0.0f;
    if (combinedAlpha > 1.0f) combinedAlpha = 1.0f;
    BYTE thumbAlpha = (BYTE)roundf(combinedAlpha * 255.0f);

    RECT rcClient; GetClientRect(g_hSwitcher, &rcClient);
    int masterPadX = DpiScale(g_settings.switcherPadding, g_dpiX);
    int masterPadY = DpiScale(g_settings.switcherPadding, g_dpiY);
    RECT rcContentClip = { masterPadX, masterPadY, rcClient.right - masterPadX, rcClient.bottom - masterPadY };

    struct ThumbCacheState {
        RECT dst;
        BYTE alpha;
        BOOL visible;
    };
    static std::map<HTHUMBNAIL, ThumbCacheState> s_lastThumbState;

    auto updateThumb = [&](HTHUMBNAIL hThumb, const RECT& dst, BYTE alpha) {
        if (!hThumb) return;
        if (alpha <= 0) {
            auto it = s_lastThumbState.find(hThumb);
            if (it == s_lastThumbState.end() || it->second.visible) {
                DWM_THUMBNAIL_PROPERTIES p = {};
                p.dwFlags = DWM_TNP_VISIBLE;
                p.fVisible = FALSE;
                DwmUpdateThumbnailProperties(hThumb, &p);
                s_lastThumbState[hThumb] = { {}, 0, FALSE };
            }
            return;
        }
        RECT rcIntersect;
        if (!IntersectRect(&rcIntersect, &rcContentClip, &dst)) {
            auto it = s_lastThumbState.find(hThumb);
            if (it == s_lastThumbState.end() || it->second.visible) {
                DWM_THUMBNAIL_PROPERTIES p = {};
                p.dwFlags = DWM_TNP_VISIBLE;
                p.fVisible = FALSE;
                DwmUpdateThumbnailProperties(hThumb, &p);
                s_lastThumbState[hThumb] = { {}, 0, FALSE };
            }
            return;
        }
        auto it = s_lastThumbState.find(hThumb);
        if (it != s_lastThumbState.end() && it->second.visible &&
            EqualRect(&it->second.dst, &dst) && it->second.alpha == alpha) {
            return; // Skip redundant IPC
        }
        DWM_THUMBNAIL_PROPERTIES p = {};
        p.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_OPACITY | DWM_TNP_VISIBLE;
        p.rcDestination = dst;
        p.opacity = alpha;
        p.fVisible = TRUE;
        DwmUpdateThumbnailProperties(hThumb, &p);
        s_lastThumbState[hThumb] = { dst, alpha, TRUE };
    };

    // 1. Incoming items
    for (auto& w : g_windows) {
        if (w.rcThumbActual.left == 0 && w.rcThumbActual.right == 0 &&
            w.rcThumbActual.top == 0 && w.rcThumbActual.bottom == 0) {
            for (const auto& kv : w.hThumbs) {
                updateThumb(kv.second, { 0, 0, 0, 0 }, 0);
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
            updateThumb(kv.second, dst, itemAlpha);
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
                        DwmUpdateThumbnailProperties(kv.second, &p);
                    }
                }
                continue;
            }

            for (const auto& kv : snap.hThumbs) {
                updateThumb(kv.second, dst, thumbAlpha);
            }
        }
    }

    // 3. Departing items in layout transition
    if (g_layoutTransition.active && !g_layoutTransition.departingItems.empty()) {
        for (const auto& item : g_layoutTransition.departingItems) {
            BYTE departAlpha = (BYTE)roundf(thumbAlpha * item.alpha);
            RECT dst = item.rcThumbCurrent;
            for (const auto& kv : item.hThumbs) {
                updateThumb(kv.second, dst, departAlpha);
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
        if (measuredDt > 0.002f) {
            dt = measuredDt;
        }
    }
    g_animLastTickTime = now;

    if (dt > 0.1f) dt = 0.1f;
    if (dt <= 0.0f) dt = 0.001f;

    bool anyActive = false;
    bool hadThumbMotion = (g_animEntranceActive || g_scrollTransition.active || g_animExitActive || g_layoutTransition.active || g_dockPreviewSlide.active || g_animHoverScaleActive);
    bool hadScrollOrEntrance = hadThumbMotion;

    // 1. Entrance animation
    if (g_animEntranceActive) {
        g_animEntranceProgress += dt / g_animEntranceDuration;
        if (g_animEntranceProgress >= 1.0f) {
            g_animEntranceProgress = 1.0f;
            g_animEntranceActive = false;
            g_animEntranceCurrentAlpha = 1.0f;
        } else {
            anyActive = true;
            g_animEntranceCurrentAlpha = g_easeEntrance.Solve(g_animEntranceProgress);
        }
    }

    // 2. Selection focus animation (WinUI 3 Decelerate cubic-bezier, consistent with inner hover border)
    if (g_animSelectionActive && !g_layoutTransition.active) {
        g_animSelectionProgress += dt / g_animSelectionDuration;
        if (g_animSelectionProgress >= 1.0f) {
            g_animSelectionProgress = 1.0f;
            g_animSelectionActive = false;
            g_animSelectionCurrent = g_animSelectionTarget;
        } else {
            anyActive = true;
            float e = g_easeSelection.Solve(g_animSelectionProgress);
            g_animSelectionCurrent = LerpRect(g_animSelectionStart, g_animSelectionTarget, e);
        }
    }

    // 3. Scroll / Page Slide animation
    if (g_scrollTransition.active) {
        g_scrollTransition.progress += dt / g_scrollTransition.duration;
        if (g_scrollTransition.progress >= 1.0f) {
            g_scrollTransition.progress = 1.0f;
            g_scrollTransition.active = false;
            g_scrollTransition.offsetCurrentX = 0.0f;
            g_scrollTransition.offsetCurrentY = 0.0f;
            if (s_cachedStaticDC && s_cachedScrollToDC && s_cachedStaticW == s_cachedScrollToW && s_cachedStaticH == s_cachedScrollToH) {
                BitBlt(s_cachedStaticDC, 0, 0, s_cachedStaticW, s_cachedStaticH, s_cachedScrollToDC, 0, 0, SRCCOPY);
                g_staticContentDirty = false;
            }
            for (auto& item : g_scrollTransition.outgoingItems) {
                int curIdx = FindWindowIndexByHwnd(item.hWnd);
                if (curIdx == -1 || IsWindowTruncated(curIdx)) {
                    for (const auto& kv : item.hThumbs) {
                        if (kv.second) DwmUnregisterThumbnail(kv.second);
                    }
                    if (curIdx != -1) {
                        g_windows[curIdx].hThumbs.clear();
                    }
                }
            }
            g_scrollTransition.outgoingItems.clear();
            g_scrollTransition.preservingThumbnails = false;
            if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
                SnapSelectionTo(ToRectF(g_windows[g_selectedIndex].rcCell));
            }
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

    // 4. Hover animation
    if (g_animHoverActive) {
        g_animHoverProgress += dt / g_animHoverDuration;
        if (g_animHoverProgress >= 1.0f) {
            g_animHoverProgress = 1.0f;
            g_animHoverActive = false;
            g_animHoverCurrent = g_animHoverTarget;
            g_animHoverAlphaCurrent = g_animHoverAlphaTarget;
        } else {
            anyActive = true;
            float e = (g_animHoverAlphaTarget >= g_animHoverAlphaStart)
                ? g_easeHoverEnter.Solve(g_animHoverProgress)
                : g_easeExit.Solve(g_animHoverProgress);
            g_animHoverCurrent = LerpRect(g_animHoverStart, g_animHoverTarget, e);
            g_animHoverAlphaCurrent = g_animHoverAlphaStart + (g_animHoverAlphaTarget - g_animHoverAlphaStart) * e;
        }
    }

    // Hover zoom scale animation (per-entry concurrent WinUI 3 curves with C0 state continuity)
    bool hoverScaleAnimActive = false;
    static bool s_hadHoverScaleMotion = false;
    if (ThumbnailHoverIsZoom()) {
        for (int i = 0; i < (int)g_windows.size(); i++) {
            auto& w = g_windows[i];
            float targetScale = (i == g_hoverThumbIndex && !IsWindowTruncated(i)) ? (1.0f + SWS_HOVER_ZOOM_DELTA) : 1.0f;
            if (fabsf(targetScale - w.hoverScaleTarget) > 0.0001f) {
                // Target changed: seamless transition from instantaneous scale
                w.hoverScaleStart = w.hoverScale;
                w.hoverScaleTarget = targetScale;
                w.hoverScaleProgress = 0.0f;
                float dist = fabsf(w.hoverScaleTarget - w.hoverScaleStart);
                float baseDuration = (targetScale > w.hoverScaleStart) ? 0.150f : 0.140f;
                w.hoverScaleDuration = (SWS_HOVER_ZOOM_DELTA > 0.0f) ? (baseDuration * (dist / SWS_HOVER_ZOOM_DELTA)) : baseDuration;
                if (w.hoverScaleDuration < 0.060f) w.hoverScaleDuration = 0.060f;
            }

            if (w.hoverScaleProgress < 1.0f) {
                hoverScaleAnimActive = true;
                w.hoverScaleProgress += dt / w.hoverScaleDuration;
                if (w.hoverScaleProgress >= 1.0f) {
                    w.hoverScaleProgress = 1.0f;
                    w.hoverScale = w.hoverScaleTarget;
                } else {
                    float e = g_easeHoverEnter.Solve(w.hoverScaleProgress);
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
            w.hoverScaleDuration = 0.150f;
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

    // 5. Exit animation (90ms WinUI 3 dissolve)
    if (g_animExitActive) {
        g_animExitProgress += dt / g_animExitDuration;
        if (g_animExitProgress >= 1.0f) {
            g_animExitProgress = 1.0f;
            g_animExitActive = false;
            g_animExitCurrentAlpha = 0.0f;
            HideSwitcher();
            return;
        } else {
            anyActive = true;
            g_animExitCurrentAlpha = 1.0f - g_easeExit.Solve(g_animExitProgress);
        }
    }

    // 6. Chevron reveal / fade animation with cubic-bezier easing & spatial glide
    if (g_animChevronProgressPrev < 1.0f) {
        anyActive = true;
        g_animChevronProgressPrev += dt / g_animChevronDuration;
        if (g_animChevronProgressPrev >= 1.0f) {
            g_animChevronProgressPrev = 1.0f;
            g_animChevronAlphaPrev = g_animChevronAlphaTargetPrev;
        } else {
            float e = (g_animChevronAlphaTargetPrev > g_animChevronStartAlphaPrev)
                ? g_easeEntrance.Solve(g_animChevronProgressPrev)
                : g_easeExit.Solve(g_animChevronProgressPrev);
            g_animChevronAlphaPrev = g_animChevronStartAlphaPrev + (g_animChevronAlphaTargetPrev - g_animChevronStartAlphaPrev) * e;
        }
    } else {
        g_animChevronAlphaPrev = g_animChevronAlphaTargetPrev;
    }

    if (g_animChevronProgressNext < 1.0f) {
        anyActive = true;
        g_animChevronProgressNext += dt / g_animChevronDuration;
        if (g_animChevronProgressNext >= 1.0f) {
            g_animChevronProgressNext = 1.0f;
            g_animChevronAlphaNext = g_animChevronAlphaTargetNext;
        } else {
            float e = (g_animChevronAlphaTargetNext > g_animChevronStartAlphaNext)
                ? g_easeEntrance.Solve(g_animChevronProgressNext)
                : g_easeExit.Solve(g_animChevronProgressNext);
            g_animChevronAlphaNext = g_animChevronStartAlphaNext + (g_animChevronAlphaTargetNext - g_animChevronStartAlphaNext) * e;
        }
    } else {
        g_animChevronAlphaNext = g_animChevronAlphaTargetNext;
    }

    // Hover bloom transitions for chevrons (80ms in / 120ms out)
    float targetHoverPrev = (g_hoverChevron == -1) ? 1.0f : 0.0f;
    float hoverStepPrev = (targetHoverPrev > g_animChevronHoverAlphaPrev) ? (dt / 0.080f) : (dt / 0.120f);
    if (fabsf(g_animChevronHoverAlphaPrev - targetHoverPrev) > 0.01f) {
        anyActive = true;
        if (targetHoverPrev > g_animChevronHoverAlphaPrev) {
            g_animChevronHoverAlphaPrev = (std::min)(1.0f, g_animChevronHoverAlphaPrev + hoverStepPrev);
        } else {
            g_animChevronHoverAlphaPrev = (std::max)(0.0f, g_animChevronHoverAlphaPrev - hoverStepPrev);
        }
    } else {
        g_animChevronHoverAlphaPrev = targetHoverPrev;
    }

    float targetHoverNext = (g_hoverChevron == 1) ? 1.0f : 0.0f;
    float hoverStepNext = (targetHoverNext > g_animChevronHoverAlphaNext) ? (dt / 0.080f) : (dt / 0.120f);
    if (fabsf(g_animChevronHoverAlphaNext - targetHoverNext) > 0.01f) {
        anyActive = true;
        if (targetHoverNext > g_animChevronHoverAlphaNext) {
            g_animChevronHoverAlphaNext = (std::min)(1.0f, g_animChevronHoverAlphaNext + hoverStepNext);
        } else {
            g_animChevronHoverAlphaNext = (std::max)(0.0f, g_animChevronHoverAlphaNext - hoverStepNext);
        }
    } else {
        g_animChevronHoverAlphaNext = targetHoverNext;
    }

    // 7. Dynamic Layout Transition (Window resize + Card rearrange + Add/Remove animation)
    if (g_layoutTransition.active) {
        g_layoutTransition.progress += dt / g_layoutTransition.duration;
        if (g_layoutTransition.progress >= 1.0f) {
            g_layoutTransition.progress = 1.0f;
            g_layoutTransition.active = false;

            for (auto& w : g_windows) {
                w.rcCell = w.rcCellTarget;
                if (!DockLayoutActive()) {
                    w.rcThumbActual = w.rcThumbTarget;
                } else {
                    w.rcThumbActual = (&w == &g_windows[g_selectedIndex] && DockShowPreview()) ? g_rcCentralPreview : RECT{ 0, 0, 0, 0 };
                }
                w.isNewEntry = false;
                w.enterAlpha = 1.0f;
                w.enterScale = 1.0f;
            }

            if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
                g_animSelectionCurrent = ToRectF(g_windows[g_selectedIndex].rcCell);
                g_animSelectionTarget = g_animSelectionCurrent;
                g_animSelectionStart = g_animSelectionCurrent;
                g_animSelectionActive = false;
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
            HDWP hdwp = BeginDeferWindowPos(2);
            if (hdwp) {
                hdwp = DeferWindowPos(hdwp, g_hSwitcher, HWND_TOPMOST, finalCx, finalCy, finalW, finalH, SWP_NOACTIVATE);
                if (hdwp && g_hCloseBtnWnd) {
                    hdwp = DeferWindowPos(hdwp, g_hCloseBtnWnd, HWND_TOPMOST, finalCx, finalCy, finalW, finalH, SWP_NOACTIVATE);
                }
                if (hdwp) EndDeferWindowPos(hdwp);
            }
            g_switcherBaseX = finalCx;
            g_switcherBaseY = finalCy;
            g_switcherBaseInitialized = true;
            ApplySwitcherRegion();

            for (auto& dep : g_layoutTransition.departingItems) {
                for (const auto& kv : dep.hThumbs) {
                    if (kv.second) DwmUnregisterThumbnail(kv.second);
                }
            }
            g_layoutTransition.departingItems.clear();

            InvalidateStaticCache();
            if (DockLayoutActive()) {
                UpdateDockPreviewForSelection();
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
            HDWP hdwp = BeginDeferWindowPos(2);
            if (hdwp) {
                hdwp = DeferWindowPos(hdwp, g_hSwitcher, HWND_TOPMOST, curCx, curCy, curW, curH, SWP_NOACTIVATE);
                if (hdwp && g_hCloseBtnWnd) {
                    hdwp = DeferWindowPos(hdwp, g_hCloseBtnWnd, HWND_TOPMOST, curCx, curCy, curW, curH, SWP_NOACTIVATE);
                }
                if (hdwp) EndDeferWindowPos(hdwp);
            }
            g_switcherBaseX = curCx;
            g_switcherBaseY = curCy;
            g_switcherBaseInitialized = true;

            for (auto& w : g_windows) {
                if (w.isNewEntry) {
                    w.enterAlpha = t;
                    w.enterScale = 0.94f + 0.06f * t;

                    if (!DockLayoutActive()) {
                        int tw = w.rcThumbTarget.right - w.rcThumbTarget.left;
                        int th = w.rcThumbTarget.bottom - w.rcThumbTarget.top;
                        int curW = (int)roundf(tw * w.enterScale);
                        int curH = (int)roundf(th * w.enterScale);
                        int mx = (w.rcThumbTarget.left + w.rcThumbTarget.right) / 2;
                        int my = (w.rcThumbTarget.top + w.rcThumbTarget.bottom) / 2;
                        w.rcThumbActual = { mx - curW / 2, my - curH / 2, mx + curW / 2, my + curH / 2 };
                    } else {
                        w.rcThumbActual = (&w == &g_windows[g_selectedIndex] && DockShowPreview()) ? g_rcCentralPreview : RECT{ 0, 0, 0, 0 };
                    }

                    int cw = w.rcCellTarget.right - w.rcCellTarget.left;
                    int ch = w.rcCellTarget.bottom - w.rcCellTarget.top;
                    int curCw = (int)roundf(cw * w.enterScale);
                    int curCh = (int)roundf(ch * w.enterScale);
                    int mcx = (w.rcCellTarget.left + w.rcCellTarget.right) / 2;
                    int mcy = (w.rcCellTarget.top + w.rcCellTarget.bottom) / 2;
                    w.rcCell = { mcx - curCw / 2, mcy - curCh / 2, mcx + curCw / 2, mcy + curCh / 2 };
                } else {
                    w.enterAlpha = 1.0f;
                    w.enterScale = 1.0f;
                    if (w.rcCellStart.left == 0 && w.rcCellStart.right == 0 &&
                        w.rcCellStart.top == 0 && w.rcCellStart.bottom == 0) {
                        w.rcCellStart = w.rcCellTarget;
                    }
                    if (w.rcCellTarget.left == 0 && w.rcCellTarget.right == 0 &&
                        w.rcCellTarget.top == 0 && w.rcCellTarget.bottom == 0) {
                        w.rcCellTarget = w.rcCellStart;
                    }
                    RectF c = LerpRect(ToRectF(w.rcCellStart), ToRectF(w.rcCellTarget), t);
                    w.rcCell = { (LONG)roundf(c.left), (LONG)roundf(c.top), (LONG)roundf(c.right), (LONG)roundf(c.bottom) };
                    if (!DockLayoutActive()) {
                        if (w.rcThumbStart.left == 0 && w.rcThumbStart.right == 0) w.rcThumbStart = w.rcThumbTarget;
                        if (w.rcThumbTarget.left == 0 && w.rcThumbTarget.right == 0) w.rcThumbTarget = w.rcThumbStart;
                        RectF th = LerpRect(ToRectF(w.rcThumbStart), ToRectF(w.rcThumbTarget), t);
                        w.rcThumbActual = { (LONG)roundf(th.left), (LONG)roundf(th.top), (LONG)roundf(th.right), (LONG)roundf(th.bottom) };
                    } else {
                        w.rcThumbActual = (&w == &g_windows[g_selectedIndex] && DockShowPreview()) ? g_rcCentralPreview : RECT{ 0, 0, 0, 0 };
                    }
                }
            }

            for (auto& dep : g_layoutTransition.departingItems) {
                dep.alpha = 1.0f - t;
                dep.scale = 1.0f - 0.08f * t; // 1.0 -> 0.92 subtle shrink

                int startW = dep.rcThumbStart.right - dep.rcThumbStart.left;
                int startH = dep.rcThumbStart.bottom - dep.rcThumbStart.top;
                int curW = (int)roundf(startW * dep.scale);
                int curH = (int)roundf(startH * dep.scale);
                int midX = (dep.rcThumbStart.left + dep.rcThumbStart.right) / 2;
                int midY = (dep.rcThumbStart.top + dep.rcThumbStart.bottom) / 2;
                dep.rcThumbCurrent = { midX - curW / 2, midY - curH / 2, midX + curW / 2, midY + curH / 2 };

                int cellW = dep.rcCellStart.right - dep.rcCellStart.left;
                int cellH = dep.rcCellStart.bottom - dep.rcCellStart.top;
                int curCellW = (int)roundf(cellW * dep.scale);
                int curCellH = (int)roundf(cellH * dep.scale);
                int midCellX = (dep.rcCellStart.left + dep.rcCellStart.right) / 2;
                int midCellY = (dep.rcCellStart.top + dep.rcCellStart.bottom) / 2;
                dep.rcCellCurrent = { midCellX - curCellW / 2, midCellY - curCellH / 2, midCellX + curCellW / 2, midCellY + curCellH / 2 };
            }

            if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
                g_animSelectionCurrent = ToRectF(g_windows[g_selectedIndex].rcCell);
                g_animSelectionTarget = g_animSelectionCurrent;
                g_animSelectionStart = g_animSelectionCurrent;
                g_animSelectionActive = false;
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

    // 7b. Dock Layout Central Preview Directional Slide Transition (Window close)
    if (g_dockPreviewSlide.active) {
        g_dockPreviewSlide.progress += dt / g_dockPreviewSlide.duration;
        if (g_dockPreviewSlide.progress >= 1.0f) {
            g_dockPreviewSlide.progress = 1.0f;
            g_dockPreviewSlide.active = false;
            g_dockPreviewSlide.currentOffset = 0.0f;
            g_dockPreviewSlide.currentAlpha = 1.0f;
        } else {
            anyActive = true;
            float t = g_easeSlide.Solve(g_dockPreviewSlide.progress);
            g_dockPreviewSlide.currentOffset = (1.0f - t) * g_dockPreviewSlide.travelDistance;
            g_dockPreviewSlide.currentAlpha = t;
        }
        if (DockLayoutActive()) {
            UpdateDockThumbnailDwm();
            g_staticContentDirty = true;
        }
    }

    // 8. Close button fade & hover plate transitions (per-entry WinUI 3 concurrent cross-fade)
    bool closeBtnAnimActive = false;
    for (int i = 0; i < (int)g_windows.size(); i++) {
        float target = (i == g_hoverIndex && g_settings.showCloseButton && !IsWindowTruncated(i)) ? 1.0f : 0.0f;
        if (fabsf(g_windows[i].closeBtnAlpha - target) > 0.01f) {
            closeBtnAnimActive = true;
            float step = (target > g_windows[i].closeBtnAlpha) ? (dt / 0.150f) : (dt / 0.100f);
            if (target > g_windows[i].closeBtnAlpha) {
                g_windows[i].closeBtnAlpha = (std::min)(1.0f, g_windows[i].closeBtnAlpha + step);
            } else {
                g_windows[i].closeBtnAlpha = (std::max)(0.0f, g_windows[i].closeBtnAlpha - step);
            }
        } else {
            g_windows[i].closeBtnAlpha = target;
        }
    }
    if (closeBtnAnimActive) anyActive = true;

    g_animCloseBtnAlpha = (g_hoverIndex >= 0 && g_hoverIndex < (int)g_windows.size()) ? g_windows[g_hoverIndex].closeBtnAlpha : 0.0f;

    float targetHoverAlpha = (g_isCloseHovered && g_hoverIndex >= 0 && g_hoverIndex < (int)g_windows.size() && g_windows[g_hoverIndex].closeBtnAlpha > 0.05f) ? 1.0f : 0.0f;
    float hoverStep = (targetHoverAlpha > g_animCloseBtnHoverAlpha) ? (dt / 0.100f) : (dt / 0.120f);
    if (fabsf(g_animCloseBtnHoverAlpha - targetHoverAlpha) > 0.01f) {
        anyActive = true;
        if (targetHoverAlpha > g_animCloseBtnHoverAlpha) {
            g_animCloseBtnHoverAlpha = (std::min)(1.0f, g_animCloseBtnHoverAlpha + hoverStep);
        } else {
            g_animCloseBtnHoverAlpha = (std::max)(0.0f, g_animCloseBtnHoverAlpha - hoverStep);
        }
    } else {
        g_animCloseBtnHoverAlpha = targetHoverAlpha;
    }

    if (g_settings.showThumbnails && (hadThumbMotion || g_animHoverScaleActive || hadScrollOrEntrance || g_animEntranceActive || g_scrollTransition.active || g_animExitActive || g_layoutTransition.active)) {
        UpdateThumbnailAnimations();
    }

    PaintSwitcher();

    if (!anyActive) {
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
static int GetHeaderTitleHeightPx() {
    return MulDiv(18, g_dpiY, 96);
}
static int GetHeaderRowHeightPx() {
    if (DockLayoutActive()) {
        return MulDiv(30, g_dpiY, 96);
    }
    if (!g_settings.showTitle && !g_settings.showIcon) {
        return 0;
    }

    if (!HeaderIsVertical()) {
        int h = MulDiv(SWS_ROW_TITLE_HEIGHT, g_dpiY, 96);
        if (g_settings.showIcon && GetHeaderIconSizePx() > h) h = GetHeaderIconSizePx();
        return h;
    }

    int gap = MulDiv(4, g_dpiY, 96);
    int h = 0;
    if (g_settings.showIcon) h += GetHeaderIconSizePx();
    if (g_settings.showTitle) h += (h > 0 ? gap : 0) + GetHeaderTitleHeightPx();
    return h;
}
static INT GetCornerPref() {
    if (wcscmp(g_settings.cornerPreference, L"default") == 0) return 0;
    if (wcscmp(g_settings.cornerPreference, L"none") == 0) return 1;
    if (wcscmp(g_settings.cornerPreference, L"roundSmall") == 0) return 3;
    if (wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        if (g_settings.customCornerRadius <= 0) return 1; // DONOTROUND (0px sharp rectangle)
        if (g_settings.customCornerRadius <= 5) return 3; // ROUNDSMALL (~4px)
        return 2; // ROUND (~8px)
    }
    return 2; // Default to round
}

static int GetWindowCornerRadiusPx() {
    if (wcscmp(g_settings.cornerPreference, L"none") == 0) {
        return 0;
    }
    // For Theme: none, 32-bit layered window supports arbitrary custom radius
    if (ThemeIs(L"none")) {
        if (wcscmp(g_settings.cornerPreference, L"custom") == 0) {
            return MulDiv(g_settings.customCornerRadius, g_dpiX, 96);
        }
        if (wcscmp(g_settings.cornerPreference, L"roundSmall") == 0) {
            return MulDiv(4, g_dpiX, 96);
        }
        if (wcscmp(g_settings.cornerPreference, L"default") == 0 || wcscmp(g_settings.cornerPreference, L"auto") == 0) {
            return IsWin11OrGreater() ? MulDiv(8, g_dpiX, 96) : 0;
        }
        return MulDiv(8, g_dpiX, 96);
    }
    // On Windows 10, Acrylic blur is physically a 90° rectangle
    if (!IsWin11OrGreater()) {
        return 0;
    }
    // On Windows 11 with Mica or Desktop Acrylic, outer window matches DWM hardware backdrop geometry
    INT cp = GetCornerPref();
    if (cp == 1) return 0;
    if (cp == 3) return MulDiv(4, g_dpiX, 96);
    return MulDiv(8, g_dpiX, 96);
}

static bool UseTaskRoundedCorners() {
    return g_settings.taskRoundedCorners;
}

static int GetTaskUiCornerRadiusPx() {
    if (!UseTaskRoundedCorners()) {
        return 0;
    }
    if (wcscmp(g_settings.cornerPreference, L"none") == 0) {
        return 0;
    }
    if (wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        return MulDiv(g_settings.customCornerRadius, g_dpiX, 96);
    }
    if (wcscmp(g_settings.cornerPreference, L"roundSmall") == 0) {
        return MulDiv(4, g_dpiX, 96);
    }
    if (wcscmp(g_settings.cornerPreference, L"default") == 0 || wcscmp(g_settings.cornerPreference, L"auto") == 0) {
        return IsWin11OrGreater() ? MulDiv(8, g_dpiX, 96) : 0;
    }
    return MulDiv(8, g_dpiX, 96);
}

// Thumbnail corner rounding is controlled independently from the task border /
// close button rounding, but shares the same radius from Corner Preference.
static int GetThumbnailCornerRadiusPx() {
    if (!g_settings.roundThumbnailCorners) {
        return 0;
    }
    if (wcscmp(g_settings.cornerPreference, L"none") == 0) {
        return 0;
    }
    if (wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        return MulDiv(g_settings.customCornerRadius, g_dpiX, 96);
    }
    if (wcscmp(g_settings.cornerPreference, L"roundSmall") == 0) {
        return MulDiv(4, g_dpiX, 96);
    }
    if (wcscmp(g_settings.cornerPreference, L"default") == 0 || wcscmp(g_settings.cornerPreference, L"auto") == 0) {
        // Windows 11 controls, preview cards, and tiles use 4px rounding at 96 DPI (DWMWCP_ROUNDSMALL)
        return IsWin11OrGreater() ? MulDiv(4, g_dpiX, 96) : 0;
    }
    return MulDiv(8, g_dpiX, 96);
}

static int GetGroupIndicatorCornerRadiusPx(int maxRadius) {
    if (!g_settings.roundGroupIndicator) {
        return 0;
    }
    if (wcscmp(g_settings.cornerPreference, L"none") == 0) {
        return 0;
    }
    int radius = 0;
    if (wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        radius = MulDiv(g_settings.customCornerRadius, g_dpiX, 96);
    } else {
        radius = MulDiv(4, g_dpiX, 96);
    }
    return (radius > maxRadius) ? maxRadius : radius;
}

static int GetBadgeIconBackgroundCornerRadiusPx(int maxRadius) {
    if (!g_settings.roundBadgeIconBackground) {
        return 0;
    }
    if (wcscmp(g_settings.cornerPreference, L"none") == 0) {
        return 0;
    }
    int radius = 0;
    if (wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        radius = MulDiv(g_settings.customCornerRadius, g_dpiX, 96);
    } else {
        radius = MulDiv(4, g_dpiX, 96);
    }
    return (radius > maxRadius) ? maxRadius : radius;
}

static void GetSwitcherPosition(const RECT& workArea, int* outX, int* outY) {
    int w = workArea.right - workArea.left;
    int h = workArea.bottom - workArea.top;
    int m = MulDiv(g_settings.switcherPositionMargin, g_dpiX, 96);
    if (wcscmp(g_settings.switcherPosition, L"topLeft") == 0) {
        *outX = workArea.left + m; *outY = workArea.top + m;
    } else if (wcscmp(g_settings.switcherPosition, L"topCenter") == 0) {
        *outX = workArea.left + (w - g_winW) / 2; *outY = workArea.top + m;
    } else if (wcscmp(g_settings.switcherPosition, L"topRight") == 0) {
        *outX = workArea.right - g_winW - m; *outY = workArea.top + m;
    } else if (wcscmp(g_settings.switcherPosition, L"centerLeft") == 0) {
        *outX = workArea.left + m; *outY = workArea.top + (h - g_winH) / 2;
    } else if (wcscmp(g_settings.switcherPosition, L"centerRight") == 0) {
        *outX = workArea.right - g_winW - m; *outY = workArea.top + (h - g_winH) / 2;
    } else if (wcscmp(g_settings.switcherPosition, L"bottomLeft") == 0) {
        *outX = workArea.left + m; *outY = workArea.bottom - g_winH - m;
    } else if (wcscmp(g_settings.switcherPosition, L"bottomCenter") == 0) {
        *outX = workArea.left + (w - g_winW) / 2; *outY = workArea.bottom - g_winH - m;
    } else if (wcscmp(g_settings.switcherPosition, L"bottomRight") == 0) {
        *outX = workArea.right - g_winW - m; *outY = workArea.bottom - g_winH - m;
    } else { // center
        *outX = workArea.left + (w - g_winW) / 2; *outY = workArea.top + (h - g_winH) / 2;
    }
}

static int GetCloseButtonCornerRadiusPx() {
    return GetTaskUiCornerRadiusPx();
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
    if (!IsWindowEnabled(hWnd)) return false;
    if (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_DISABLED) return false;
    HWND hPopup = GetLastActivePopup(hWnd);
    if (hPopup && hPopup != hWnd && IsWindow(hPopup) && IsWindowVisible(hPopup)) {
        return false;
    }
    return true;
}

static std::vector<HWND> s_pendingCloseWindows;
static int s_pendingCloseRetries = 0;

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


// Window Enumeration

static HICON TryGetUwpIconFromExplorer(HWND hWnd, int desiredSizePx);

// Cache of crisp icons extracted from exe files at a specific pixel size.
// Owned here (DestroyIcon at unload); keyed by "<exePath>_<sizePx>".
static std::map<std::wstring, HICON> g_exeIconCache;

// WM_GETICON returns a fixed (usually 32px) icon that looks blurry when scaled
// up to 48/64. PrivateExtractIconsW pulls the best-matching frame from the
// exe's icon resource at the exact requested size for a crisp result.
static HICON TryGetCrispExeIcon(HWND hWnd, int desiredSizePx) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return NULL;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return NULL;
    WCHAR exePath[MAX_PATH] = {0};
    DWORD size = MAX_PATH;
    BOOL ok = QueryFullProcessImageNameW(hProc, 0, exePath, &size);
    CloseHandle(hProc);
    if (!ok || !exePath[0]) return NULL;

    std::wstring key = std::wstring(exePath) + L"_" + std::to_wstring(desiredSizePx);
    auto it = g_exeIconCache.find(key);
    if (it != g_exeIconCache.end()) return it->second;

    HICON hIcon = NULL;
    if (PrivateExtractIconsW(exePath, 0, desiredSizePx, desiredSizePx,
                             &hIcon, NULL, 1, 0) == 1 && hIcon) {
        g_exeIconCache[key] = hIcon;
        return hIcon;
    }
    return NULL;
}

// === Custom per-process header (icon and/or application name) ===

struct CustomHeaderRule {
    std::wstring pattern;   // executable name pattern; wildcards * and ? supported
    std::wstring iconPath;  // .ico / .exe / .dll to extract the icon from (optional)
    std::wstring appName;   // custom display name overriding the detected one (optional)
};
static std::vector<CustomHeaderRule> g_customHeaderRules;
// Owned cache of icons loaded from custom paths; keyed by "<path>_<sizePx>".
static std::map<std::wstring, HICON> g_customIconCache;

static HICON LoadCustomIconFromPath(const std::wstring& path, int sizePx) {
    if (path.empty() || sizePx <= 0) return NULL;
    std::wstring key = path + L"_" + std::to_wstring(sizePx);
    auto it = g_customIconCache.find(key);
    if (it != g_customIconCache.end()) return it->second;
    HICON hIcon = NULL;
    if (PrivateExtractIconsW(path.c_str(), 0, sizePx, sizePx,
                             &hIcon, NULL, 1, 0) == 1 && hIcon) {
        g_customIconCache[key] = hIcon;
        return hIcon;
    }
    return NULL;
}

// If the window's executable name matches a user-defined rule, return its
// custom icon. The first matching rule wins; this overrides all other sources.
static HICON TryGetCustomIcon(HWND hWnd, int sizePx) {
    if (g_customHeaderRules.empty()) return NULL;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return NULL;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return NULL;
    WCHAR exePath[MAX_PATH] = {0};
    DWORD size = MAX_PATH;
    BOOL ok = QueryFullProcessImageNameW(hProc, 0, exePath, &size);
    CloseHandle(hProc);
    if (!ok || !exePath[0]) return NULL;
    WCHAR* fileName = PathFindFileNameW(exePath);
    for (const auto& rule : g_customHeaderRules) {
        if (PathMatchSpecW(fileName, rule.pattern.c_str())) {
            HICON h = LoadCustomIconFromPath(rule.iconPath, sizePx);
            if (h) return h;
        }
    }
    return NULL;
}

// If the window's executable name matches a user-defined rule with a custom
// application name, copy it into `out` and return true. The first matching rule
// with a non-empty name wins.
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
        if (!rule.appName.empty() && PathMatchSpecW(fileName, rule.pattern.c_str())) {
            wcsncpy_s(out, cch, rule.appName.c_str(), _TRUNCATE);
            return true;
        }
    }
    return false;
}

static HICON LoadWindowIcon(HWND hWnd) {
    // User-assigned custom icon takes priority over everything else.
    HICON hIcon = TryGetCustomIcon(hWnd, GetHeaderIconSizePx());
    if (hIcon) return hIcon;
    if (g_IsShellFrameWindow && g_IsShellFrameWindow(hWnd)) {
        hIcon = TryGetUwpIconFromExplorer(hWnd, GetHeaderIconSizePx());
    }
    // A crisp exe icon extracted at the exact target size beats the WM_GETICON
    // result, which is a fixed ~32px frame: blurry when upscaled to 48/64 and
    // pixelated when downscaled to 16 (GDI does no smoothing in DrawIconEx).
    // PrivateExtractIconsW picks the best-matching frame at the requested size.
    if (!hIcon) {
        hIcon = TryGetCrispExeIcon(hWnd, GetHeaderIconSizePx());
    }
    if (!hIcon) SendMessageTimeoutW(hWnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, (DWORD_PTR*)&hIcon);
    if (!hIcon) SendMessageTimeoutW(hWnd, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, (DWORD_PTR*)&hIcon);
    if (!hIcon) SendMessageTimeoutW(hWnd, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 100, (DWORD_PTR*)&hIcon);
    if (!hIcon) hIcon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICON);
    if (!hIcon) hIcon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
    if (!hIcon) hIcon = LoadIconW(NULL, IDI_APPLICATION);
    return hIcon;
}

static void GetWindowGroupKey(HWND hWnd, WCHAR* out, size_t cch);

static bool IsEligibleWindow(HWND hWnd, WindowEntry* outEntry = nullptr) {
    if (!hWnd || !IsWindow(hWnd) || IsSwitcherWindow(hWnd)) return false;
    if (!IsAltTabWindow(hWnd)) return false;

    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) {
        if (wcscmp(g_settings.virtualDesktopBehavior, L"allDesktops") == 0 && g_pVirtualDesktopManager) {
            BOOL onCurrent = FALSE;
            if (SUCCEEDED(g_pVirtualDesktopManager->IsWindowOnCurrentVirtualDesktop(hWnd, &onCurrent)) && !onCurrent) {
                // allow cloaked window since it's just on another virtual desktop
            } else return false;
        } else return false;
    }

    bool isPrimaryOnly = (wcscmp(g_settings.switcherDisplayBehavior, L"primaryOnly") == 0);
    if (g_settings.perMonitorWindows && !g_showAllMonitors && g_hCurrentMonitor && !isPrimaryOnly) {
        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL) != g_hCurrentMonitor) return false;
    }

    if (g_isAltBacktickSameApp && !g_windows.empty()) {
        WCHAR activeKey[MAX_PATH] = {0};
        GetWindowGroupKey(g_windows[0].hWnd, activeKey, ARRAYSIZE(activeKey));
        if (activeKey[0]) {
            WCHAR targetKey[MAX_PATH] = {0};
            GetWindowGroupKey(hWnd, targetKey, ARRAYSIZE(targetKey));
            if (wcscmp(targetKey, activeKey) != 0) return false;
        }
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
        outEntry->hIcon = LoadWindowIcon(hWnd);
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
    HICON hIcon = NULL;
    Wh_Log(L"ResolveIconFromAumid: aumid=%s, desiredSizePx=%d", aumid, desiredSizePx);
    
    IShellItem* psi = NULL;
    HRESULT hr = SHCreateItemInKnownFolder(
            FOLDERID_AppsFolder, KF_FLAG_DONT_VERIFY,
            aumid, IID_PPV_ARGS(&psi));
    if (SUCCEEDED(hr) && psi) {
        Wh_Log(L"ResolveIconFromAumid: SHCreateItemInKnownFolder succeeded");
        IShellItemImageFactory* psiif = NULL;
        hr = psi->QueryInterface(IID_PPV_ARGS(&psiif));
        if (SUCCEEDED(hr) && psiif) {
            Wh_Log(L"ResolveIconFromAumid: QueryInterface(IShellItemImageFactory) succeeded");
            SIZE sz = { desiredSizePx, desiredSizePx };
            HBITMAP hBitmap = NULL;
            hr = psiif->GetImage(sz, SIIGBF_RESIZETOFIT | SIIGBF_ICONONLY, &hBitmap);
            if (SUCCEEDED(hr) && hBitmap) {
                Wh_Log(L"ResolveIconFromAumid: GetImage succeeded");
                HIMAGELIST hImageList = ImageList_Create(sz.cx, sz.cy, ILC_COLOR32, 1, 0);
                if (hImageList) {
                    if (ImageList_Add(hImageList, hBitmap, NULL) != -1) {
                        hIcon = ImageList_GetIcon(hImageList, 0, 0);
                        if (hIcon) Wh_Log(L"ResolveIconFromAumid: Successfully converted to HICON");
                        else Wh_Log(L"ResolveIconFromAumid: ImageList_GetIcon failed");
                    } else {
                        Wh_Log(L"ResolveIconFromAumid: ImageList_Add failed");
                    }
                    ImageList_Destroy(hImageList);
                } else {
                    Wh_Log(L"ResolveIconFromAumid: ImageList_Create failed");
                }
                DeleteObject(hBitmap);
            } else {
                Wh_Log(L"ResolveIconFromAumid: GetImage failed, hr=0x%08X", hr);
            }
            psiif->Release();
        } else {
            Wh_Log(L"ResolveIconFromAumid: QueryInterface failed, hr=0x%08X", hr);
        }
        psi->Release();
    } else {
        Wh_Log(L"ResolveIconFromAumid: SHCreateItemInKnownFolder failed, hr=0x%08X", hr);
    }
    
    // Fallback: SHParseDisplayName + SHGetFileInfo
    if (!hIcon) {
        Wh_Log(L"ResolveIconFromAumid: Falling back to SHParseDisplayName");
        WCHAR appsFolderPath[768];
        if (swprintf_s(appsFolderPath, L"shell:AppsFolder\\%s", aumid) > 0) {
            PIDLIST_ABSOLUTE pidl = NULL;
            hr = SHParseDisplayName(appsFolderPath, NULL, &pidl, 0, NULL);
            if (SUCCEEDED(hr) && pidl) {
                Wh_Log(L"ResolveIconFromAumid: SHParseDisplayName succeeded");
                SHFILEINFOW sfi = {};
                UINT flags = SHGFI_PIDL | SHGFI_ICON | (desiredSizePx > 24 ? SHGFI_LARGEICON : SHGFI_SMALLICON);
                if (SHGetFileInfoW((LPCWSTR)pidl, 0, &sfi, sizeof(sfi), flags)) {
                    hIcon = sfi.hIcon;
                    if (hIcon) Wh_Log(L"ResolveIconFromAumid: SHGetFileInfoW succeeded");
                    else Wh_Log(L"ResolveIconFromAumid: SHGetFileInfoW returned no icon");
                } else {
                    Wh_Log(L"ResolveIconFromAumid: SHGetFileInfoW failed");
                }
                CoTaskMemFree(pidl);
            } else {
                Wh_Log(L"ResolveIconFromAumid: SHParseDisplayName failed, hr=0x%08X", hr);
            }
        }
    }

    return hIcon;
}

LRESULT CALLBACK ExplorerIpcWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
    if (!g_WM_SWS_GET_UWP_ICON) {
        g_WM_SWS_GET_UWP_ICON = RegisterWindowMessageW(L"Windhawk_SWS_GetUwpIcon");
        Wh_Log(L"TryGetUwpIconFromExplorer: Registered message %u", g_WM_SWS_GET_UWP_ICON);
    }
    HWND hIpc = FindWindowW(L"WindhawkSWS_IpcWindow", NULL);
    if (hIpc) {
        DWORD_PTR res = 0;
        LRESULT sendRes = SendMessageTimeoutW(hIpc, g_WM_SWS_GET_UWP_ICON, (WPARAM)hWnd, desiredSizePx, SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &res);
        Wh_Log(L"TryGetUwpIconFromExplorer: SendMessageTimeoutW to %p returned %ld, res = %p", hIpc, sendRes, res);
        if (res) {
            // On Windows 11 explorer returns usable icon handles via IPC in our environment.
            // Avoid attempting local AUMID->icon resolution on Win11 to preserve that behavior.
            if (g_isWin11OrGreater) {
                return (HICON)res;
            }
            // We got an icon handle from Explorer, but HICON handles are process-local —
            // prefer resolving the AUMID locally and creating an icon in this process.
            // Try to get the AUMID locally using SHGetPropertyStoreForWindow; if available,
            // create a local icon via ResolveIconFromAumid and return it.
            std::wstring aumidLocal;
            IPropertyStore* ps = NULL;
            if (SUCCEEDED(SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&ps))) && ps) {
                PROPVARIANT pv; PropVariantInit(&pv);
                if (SUCCEEDED(ps->GetValue(PKEY_AppUserModel_ID, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal && pv.pwszVal[0]) {
                    aumidLocal = pv.pwszVal;
                    Wh_Log(L"TryGetUwpIconFromExplorer: Got AUMID locally = %s", aumidLocal.c_str());
                }
                PropVariantClear(&pv);
                ps->Release();
            }
            if (!aumidLocal.empty()) {
                std::wstring cacheKey = aumidLocal + L"_" + std::to_wstring(desiredSizePx);
                auto it = g_uwpIconCache.find(cacheKey);
                if (it != g_uwpIconCache.end()) return it->second;
                HICON hLocal = ResolveIconFromAumid(aumidLocal.c_str(), desiredSizePx);
                if (hLocal) {
                    g_uwpIconCache[cacheKey] = hLocal;
                    Wh_Log(L"TryGetUwpIconFromExplorer: Resolved local icon %p from AUMID", hLocal);
                    return hLocal;
                }
                Wh_Log(L"TryGetUwpIconFromExplorer: Local ResolveIconFromAumid failed for %s", aumidLocal.c_str());
            }
            // As a last resort, return the handle from explorer (may not be valid across processes)
            return (HICON)res;
        }
        return NULL;
    } else {
        Wh_Log(L"TryGetUwpIconFromExplorer: WindhawkSWS_IpcWindow not found");
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
        for (const auto& kv : w.hThumbs) { if (kv.second) DwmUnregisterThumbnail(kv.second); }
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
        auto getRank = [](HWND h) -> int {
            for (size_t i = 0; i < g_mruWindows.size(); i++) {
                if (g_mruWindows[i] == h) return (int)i;
            }
            bool isTopmost = (GetWindowLongPtrW(h, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
            return isTopmost ? 20000 : 10000;
        };
        return getRank(a.hWnd) < getRank(b.hWnd);
    });

    // App grouping: keep one entry per application. EnumWindows yields windows in
    // Z-order (top to bottom), so the first window seen for each app is its most
    // recently used one, which becomes the representative entry.
    if (g_settings.showApplications) {
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
                if (g_settings.showApplications) {
                    for (HWND hw : w.groupWindows) {
                        if (!IsIconic(hw)) return false;  // keep: has a visible window
                    }
                    return true;  // all windows minimized: hide
                }
                return IsIconic(w.hWnd) != FALSE;  // hide if minimized
            }), g_windows.end());
    }
    if (g_settings.sortMinimizedWindowsToEnd) {
        std::stable_sort(g_windows.begin(), g_windows.end(), [](const WindowEntry& a, const WindowEntry& b) {
            return IsIconic(a.hWnd) < IsIconic(b.hWnd);
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
        } else if (w.sourceSize.cx <= 0 || w.sourceSize.cy <= 0) {
            SIZE src = {0};
            if (SUCCEEDED(DwmQueryThumbnailSourceSize(w.hThumbs[g_hSwitcher], &src))) {
                w.sourceSize = src;
            }
        }
        for (HWND m : g_hMirrorSwitchers) {
            if (!w.hThumbs.count(m)) {
                HTHUMBNAIL hT = NULL;
                if (SUCCEEDED(DwmRegisterThumbnail(m, w.hWnd, &hT))) w.hThumbs[m] = hT;
            }
        }
        SIZE src = w.sourceSize;

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
                if (wrW > 0 && wrH > 0 && efbW > 0 && efbH > 0 && src.cx > 0 && src.cy > 0) {
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
                } else {
                    w.effectiveSourceSize = src;
                    w.rcSourceCrop = { 0, 0, src.cx, src.cy };
                }
            } else {
                w.effectiveSourceSize = src;
                w.rcSourceCrop = { 0, 0, src.cx, src.cy };
            }
        } else {
            w.effectiveSourceSize = src;
            w.rcSourceCrop = { 0, 0, src.cx, src.cy };
        }
    }
}

static void UpdateDockThumbnailDwm() {
    if (!DockLayoutActive() || !DockShowPreview() || !g_hSwitcher) return;
    int n = (int)g_windows.size();
    if (g_selectedIndex < 0 || g_selectedIndex >= n) return;

    // 1. Hide all non-selected windows' thumbnails
    for (int i = 0; i < n; i++) {
        if (i == g_selectedIndex) continue;
        for (const auto& kv : g_windows[i].hThumbs) {
            if (kv.second) {
                DWM_THUMBNAIL_PROPERTIES p = {};
                p.dwFlags = DWM_TNP_VISIBLE;
                p.fVisible = FALSE;
                DwmUpdateThumbnailProperties(kv.second, &p);
            }
        }
    }

    // 2. Show the selected window's thumbnail in g_rcCentralPreview
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

        BYTE alpha = (BYTE)(g_animEntranceCurrentAlpha * 255.0f);
        if (alpha == 0) alpha = 255;

        RECT curDst = selWnd.rcThumbActual;
        if (g_dockPreviewSlide.active) {
            int offX = (int)roundf(g_dockPreviewSlide.currentOffset);
            curDst.left += offX;
            curDst.right += offX;
            alpha = (BYTE)roundf((float)alpha * g_dockPreviewSlide.currentAlpha);
            if (alpha < 1) alpha = 1;
        }

        for (const auto& kv : selWnd.hThumbs) {
            HTHUMBNAIL hThumb = kv.second;
            if (!hThumb) continue;
            DWM_THUMBNAIL_PROPERTIES p = {};
            p.dwFlags = DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
            p.fSourceClientAreaOnly = FALSE;
            p.rcDestination = curDst;
            p.opacity = alpha;
            p.fVisible = TRUE;

            bool needsCrop = (IsIconic(selWnd.hWnd) ||
                             (selWnd.rcSourceCrop.left != 0 || selWnd.rcSourceCrop.top != 0 ||
                              selWnd.rcSourceCrop.right != selWnd.sourceSize.cx || selWnd.rcSourceCrop.bottom != selWnd.sourceSize.cy))
                             && (selWnd.rcSourceCrop.right > selWnd.rcSourceCrop.left && selWnd.rcSourceCrop.bottom > selWnd.rcSourceCrop.top);
            if (needsCrop) {
                p.dwFlags |= DWM_TNP_RECTSOURCE;
                p.rcSource = selWnd.rcSourceCrop;
            }
            DwmUpdateThumbnailProperties(hThumb, &p);
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

    int masterPadX = DpiScale(g_settings.switcherPadding, dpiX);
    int masterPadY = DpiScale(g_settings.switcherPadding, dpiY);

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
    int spacing = DpiScale(g_settings.dockIconSpacing, dpiX);

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
    int chevReserve = (n > visibleCount) ? DpiScale(26, dpiX) : 0;
    int totalIconsW = visibleCount * cellW + (visibleCount - 1) * spacing;
    int totalStripW = totalIconsW + 2 * chevReserve;

    bool showPreview = DockShowPreview();
    int prevW = 0;
    int prevH = 0;
    int previewSlotH = 0;

    int titleH = GetHeaderRowHeightPx();
    if (titleH < DpiScale(28, dpiY)) titleH = DpiScale(28, dpiY);
    int divPad = DpiScale(6, dpiY);

    if (showPreview) {
        int maxPrevH = DpiScale(g_settings.dockPreviewHeight > 0 ? g_settings.dockPreviewHeight : 280, dpiY);
        int reservedH = 2 * masterPadY + cellH + titleH + (1 + divPad * 2) * 2;
        if (maxPrevH > maxH - reservedH) maxPrevH = maxH - reservedH;
        if (maxPrevH < DpiScale(80, dpiY)) maxPrevH = DpiScale(80, dpiY);

        prevW = maxPrevH;
        prevH = maxPrevH;
        if (g_selectedIndex >= 0 && g_selectedIndex < n) {
            const auto& selWnd = g_windows[g_selectedIndex];
            if (selWnd.effectiveSourceSize.cx > 0 && selWnd.effectiveSourceSize.cy > 0) {
                prevW = (int)((double)selWnd.effectiveSourceSize.cx * maxPrevH / selWnd.effectiveSourceSize.cy);
                int maxPrevW = maxW - 2 * masterPadX - DpiScale(24, dpiX);
                if (maxPrevW > 0 && prevW > maxPrevW) {
                    prevH = (int)((double)maxPrevW * prevH / prevW);
                    prevW = maxPrevW;
                }
            }
        }
        previewSlotH = prevH + DpiScale(12, dpiY);
    }

    int titleSpacing = showPreview ? DpiScale(4, dpiY) : 0;
    int dividerSpace = 1 + divPad * 2; // Only 1 subtle divider between dock strip and content
    int contentH = cellH + previewSlotH + titleH + dividerSpace + titleSpacing;
    g_winH = contentH + 2 * masterPadY;

    int contentW = std::max(totalStripW, prevW + DpiScale(32, dpiX));
    int minW = DpiScale(380, dpiX);
    if (contentW < minW) contentW = minW;
    if (contentW > maxW - 2 * masterPadX) contentW = maxW - 2 * masterPadX;
    g_winW = contentW + 2 * masterPadX;

    int curY = masterPadY;
    if (DockIconIsTop()) {
        // 1. Icon Strip
        g_rcDockIconStrip = { masterPadX, curY, g_winW - masterPadX, curY + cellH };
        curY += cellH + divPad + 1 + divPad;

        // 2. Central Preview
        if (showPreview) {
            g_rcCentralPreviewSlot = { masterPadX, curY, g_winW - masterPadX, curY + previewSlotH };
            int px = masterPadX + (g_winW - 2 * masterPadX - prevW) / 2;
            int py = curY + (previewSlotH - prevH) / 2;
            g_rcCentralPreview = { px, py, px + prevW, py + prevH };
            curY += previewSlotH + titleSpacing;
        } else {
            g_rcCentralPreviewSlot = { 0, 0, 0, 0 };
            g_rcCentralPreview = { 0, 0, 0, 0 };
        }

        // 3. Title Bar
        g_rcDockTitleBar = { masterPadX, curY, g_winW - masterPadX, curY + titleH };
    } else {
        // 1. Title Bar
        g_rcDockTitleBar = { masterPadX, curY, g_winW - masterPadX, curY + titleH };
        curY += titleH + titleSpacing;

        // 2. Central Preview
        if (showPreview) {
            g_rcCentralPreviewSlot = { masterPadX, curY, g_winW - masterPadX, curY + previewSlotH };
            int px = masterPadX + (g_winW - 2 * masterPadX - prevW) / 2;
            int py = curY + (previewSlotH - prevH) / 2;
            g_rcCentralPreview = { px, py, px + prevW, py + prevH };
            curY += previewSlotH + divPad + 1 + divPad;
        } else {
            g_rcCentralPreviewSlot = { 0, 0, 0, 0 };
            g_rcCentralPreview = { 0, 0, 0, 0 };
            curY += divPad + 1 + divPad;
        }

        // 3. Icon Strip
        g_rcDockIconStrip = { masterPadX, curY, g_winW - masterPadX, curY + cellH };
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
    HMODULE hShcore = LoadLibraryW(L"shcore.dll");
    if (hShcore) {
        typedef HRESULT(WINAPI*GDPFM)(HMONITOR,int,UINT*,UINT*);
        auto fn = (GDPFM)GetProcAddress(hShcore, "GetDpiForMonitor");
        if (fn) fn(hMon, 0, &dpiX, &dpiY);
        FreeLibrary(hShcore);
    }
    g_dpiX = dpiX; g_dpiY = dpiY;

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
    int scaledRowH = DpiScale(g_settings.rowHeight, dpiY);
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
    int maxTileW = ScaleAutoFit(DpiScale((int)(g_settings.rowHeight * SWS_MAX_TILE_ASPECT), dpiX));

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
    int thumbTopOffset = padTop + (thumbBottom ? headerAndDividerH : 0);
    int initialTop  = elemPadTop + thumbTopOffset;
    int baseContentH = thumbSide ? std::max(thumbH, rowTitleH) : thumbH;
    int bottomInc   = (baseContentH + padBot) + elemPadBot + initialTop + (thumbTop ? headerAndDividerH : 0);
    int sideHeaderWidth = DpiScale(HeaderIsVertical() ? 96 : 150, dpiX);

    int maxW = monW * g_settings.maxWidthPercent / 100;
    int maxH = monH * g_settings.maxHeightPercent / 100;

    int sideThumbSlotW = 0;
    if (g_settings.showThumbnails && sidePlacement && thumbH > 0) {
        for (const auto& w : g_windows) {
            int slotW = thumbH;
            if (w.effectiveSourceSize.cx > 0 && w.effectiveSourceSize.cy > 0) {
                slotW = (int)((double)w.effectiveSourceSize.cx * thumbH / w.effectiveSourceSize.cy);
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
                        if (kv.second) DwmUnregisterThumbnail(kv.second);
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
                    thumbWidth = (int)((double)w.effectiveSourceSize.cx * thumbH / w.effectiveSourceSize.cy);
                } else {
                    thumbWidth = thumbH;
                }

                int naturalThumbWidth = thumbWidth;
                if (thumbWidth > maxTileW) thumbWidth = maxTileW;
                if (w.effectiveSourceSize.cx > 0 && thumbWidth > w.effectiveSourceSize.cx) thumbWidth = w.effectiveSourceSize.cx;
                if (naturalThumbWidth > 0 && thumbWidth != naturalThumbWidth) {
                    actualThumbH = (int)((double)thumbWidth * thumbH / naturalThumbWidth);
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
                    int contentH = std::max(actualThumbH, rowTitleH);
                    int baseH = std::max(thumbH, rowTitleH);
                    if (contentH < baseH) {
                        w.rcCell.bottom -= (baseH - contentH);
                    }
                } else if (actualThumbH < thumbH) {
                    w.rcCell.bottom -= (thumbH - actualThumbH);
                }
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
                    thumbX += (width - thumbWidth) / 2;
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
                    thumbWidth = (int)((double)w.effectiveSourceSize.cx * thumbH / w.effectiveSourceSize.cy);
                } else {
                    thumbWidth = thumbH;
                }

                int naturalThumbWidth = thumbWidth;
                if (thumbWidth > maxTileW) thumbWidth = maxTileW;
                if (w.effectiveSourceSize.cx > 0 && thumbWidth > w.effectiveSourceSize.cx) thumbWidth = w.effectiveSourceSize.cx;
                if (naturalThumbWidth > 0 && thumbWidth != naturalThumbWidth) {
                    actualThumbH = (int)((double)thumbWidth * thumbH / naturalThumbWidth);
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
                    int contentH = std::max(actualThumbH, rowTitleH);
                    int baseH = std::max(thumbH, rowTitleH);
                    if (contentH < baseH) {
                        w.rcCell.bottom -= (baseH - contentH);
                    }
                } else if (actualThumbH < thumbH) {
                    w.rcCell.bottom -= (thumbH - actualThumbH);
                }
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
                    thumbX += (width - thumbWidth) / 2;
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
        } else if (w.sourceSize.cx <= 0 || w.sourceSize.cy <= 0) {
            SIZE src = {0};
            if (SUCCEEDED(DwmQueryThumbnailSourceSize(w.hThumbs[g_hSwitcher], &src))) {
                w.sourceSize = src;
            }
        }
        if (w.effectiveSourceSize.cx <= 0 || w.effectiveSourceSize.cy <= 0) {
            w.effectiveSourceSize = w.sourceSize;
            w.rcSourceCrop = { 0, 0, w.sourceSize.cx, w.sourceSize.cy };
        }
        for (HWND m : g_hMirrorSwitchers) {
            if (!w.hThumbs.count(m)) {
                HTHUMBNAIL hT = NULL;
                if (SUCCEEDED(DwmRegisterThumbnail(m, w.hWnd, &hT))) w.hThumbs[m] = hT;
            }
        }
        
        for (const auto& kv : w.hThumbs) {
            HTHUMBNAIL hThumb = kv.second;
            if (!hThumb) continue;
            // Skip truncated windows with zero destination rect
            if (w.rcThumbActual.left == 0 && w.rcThumbActual.right == 0 &&
                w.rcThumbActual.top == 0 && w.rcThumbActual.bottom == 0) continue;
            DWM_THUMBNAIL_PROPERTIES p = {};
            p.dwFlags = DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
            p.fSourceClientAreaOnly = FALSE;
            p.rcDestination = w.rcThumbActual;
            int offX = (int)roundf(g_scrollTransition.offsetCurrentX);
            int offY = (int)roundf(g_scrollTransition.offsetCurrentY);
            if (offX != 0 || offY != 0) {
                OffsetRect(&p.rcDestination, offX, offY);
            }
            p.opacity = (BYTE)(g_animEntranceCurrentAlpha * 255.0f);
            p.fVisible = TRUE;
            // Only set DWM_TNP_RECTSOURCE when the crop is non-trivial (e.g. maximized
            // windows with invisible frame borders) or when the window is minimized.
            // For restored non-maximized windows, omitting DWM_TNP_RECTSOURCE preserves
            // the window's visual style including rounded corners on Windows 11.
            // For iconic windows, DWM's default style introduces an extraneous drop shadow
            // halo around the cached thumbnail that spills past rcDestination; setting
            // RECTSOURCE clips it strictly to rcThumbActual.
            bool needsCrop = (IsIconic(w.hWnd) ||
                             (w.rcSourceCrop.left != 0 || w.rcSourceCrop.top != 0 ||
                              w.rcSourceCrop.right != w.sourceSize.cx || w.rcSourceCrop.bottom != w.sourceSize.cy))
                             && (w.rcSourceCrop.right > w.rcSourceCrop.left && w.rcSourceCrop.bottom > w.rcSourceCrop.top);
            if (needsCrop) {
                p.dwFlags |= DWM_TNP_RECTSOURCE;
                p.rcSource = w.rcSourceCrop;
            }
            DwmUpdateThumbnailProperties(hThumb, &p);
        }
    }
}
static void UnregisterThumbnails() {
    for (auto& w : g_windows) {
        for (const auto& kv : w.hThumbs) {
            if (kv.second) DwmUnregisterThumbnail(kv.second);
        }
        w.hThumbs.clear();
    }
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
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdc);
    
    // Compute alpha and create shadow bitmap
    BYTE* shadowBits = new BYTE[width * height * 4];
    BYTE* blackPtr = (BYTE*)pBlackBits;
    BYTE* whitePtr = (BYTE*)pWhiteBits;
    
    for (int i = 0; i < width * height; ++i) {
        int idx = i * 4;
        int whiteB = whitePtr[idx];
        int blackB = blackPtr[idx];
        
        int a = 255 - (whiteB - blackB);
        if (a < 0) a = 0;
        if (a > 255) a = 255;
        
        int finalAlpha = (int)(a * shadowAlphaMult);
        if (finalAlpha > 255) finalAlpha = 255;
        
        shadowBits[idx] = 0;     // B
        shadowBits[idx+1] = 0;   // G
        shadowBits[idx+2] = 0;   // R
        shadowBits[idx+3] = (BYTE)finalAlpha; // A
    }
    
    Gdiplus::Bitmap* shadowBmp = new Gdiplus::Bitmap(width, height, width * 4, PixelFormat32bppARGB, shadowBits);
    Gdiplus::Bitmap* shadowBmpCopy = shadowBmp->Clone(0, 0, width, height, PixelFormat32bppARGB);
    delete shadowBmp;
    delete[] shadowBits;
    DeleteObject(hBmpBlack);
    DeleteObject(hBmpWhite);
    
    return shadowBmpCopy;
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
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);

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
        Gdiplus::SolidBrush brush(Gdiplus::Color(64, r, g, b));

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
    Gdiplus::SolidBrush brush(Gdiplus::Color(64, r, g, b));
    graphics.FillRectangle(&brush, snapLeft, snapTop, snapW, snapH);
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
        int btnSz = DpiScale(16, g_dpiX);
        int btnPadding = DpiScale(2, g_dpiX);
        int bx = rcCell.right - btnSz - btnPadding;
        int by = rcCell.top + btnPadding;
        return { bx, by, bx + btnSz, by + btnSz };
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
        int iconY = rcCell.top + padTop + (rowTitleH - iconSz) / 2;

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
    // Inflate slightly (2px) for comfortable mouse target acquisition
    int pad = DpiScale(2, g_dpiX);
    RECT rcHit = { rc.left - pad, rc.top - pad, rc.right + pad, rc.bottom + pad };
    return PtInRect(&rcHit, ptClient) != FALSE;
}

static void DrawCloseButton(HDC hdc, const RECT& btnRc, float btnAlpha, float hoverAlpha, bool isPressed) {
    if (btnAlpha <= 0.01f) return;

    float origW = (float)(btnRc.right - btnRc.left);
    float origH = (float)(btnRc.bottom - btnRc.top);
    if (origW <= 0.0f || origH <= 0.0f) return;

    // WinUI 3 Fluent scale blossom: gently expand from 85% to 100% as opacity fades in
    float scale = (AreAnimationsGloballyEnabled() && btnAlpha < 1.0f) ? (0.85f + 0.15f * btnAlpha) : 1.0f;
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
        COLORREF plateCol = g_isDarkMode ? RGB(45, 45, 45) : RGB(255, 255, 255);
        Gdiplus::SolidBrush idleBrush(Gdiplus::Color(idlePlateAlpha, GetRValue(plateCol), GetGValue(plateCol), GetBValue(plateCol)));
        if (btnRadius > 0) {
            Gdiplus::GraphicsPath path;
            Gdiplus::REAL d = (Gdiplus::REAL)(btnRadius * 2);
            path.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)by, d, d, 180.0f, 90.0f);
            path.AddArc((Gdiplus::REAL)(bx + btnW) - d, (Gdiplus::REAL)by, d, d, 270.0f, 90.0f);
            path.AddArc((Gdiplus::REAL)(bx + btnW) - d, (Gdiplus::REAL)(by + btnH) - d, d, d, 0.0f, 90.0f);
            path.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)(by + btnH) - d, d, d, 90.0f, 90.0f);
            path.CloseFigure();
            graphics.FillPath(&idleBrush, &path);
        } else {
            graphics.FillRectangle(&idleBrush, (Gdiplus::REAL)bx, (Gdiplus::REAL)by, (Gdiplus::REAL)btnW, (Gdiplus::REAL)btnH);
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
    Gdiplus::LineCap cap = (btnRadius > 0 && IsWin11OrGreater()) ? Gdiplus::LineCapRound : Gdiplus::LineCapFlat;
    xPen.SetStartCap(cap);
    xPen.SetEndCap(cap);

    graphics.DrawLine(&xPen, cx - glyphSpan, cy - glyphSpan, cx + glyphSpan, cy + glyphSpan);
    graphics.DrawLine(&xPen, cx + glyphSpan, cy - glyphSpan, cx - glyphSpan, cy + glyphSpan);
}

// Multi-pass Fluent elevation drop shadow behind a thumbnail. Drawn in the
// content layer below the DWM thumbnail, producing a subtle, soft ambient
// feathering with gentle directional drop (cumulative alpha ~15% dark / ~10% light).
struct ThumbnailShadowPass {
    int baseSpread;     // Outward expansion in base pixels (at 96 DPI)
    int baseYOffset;    // Downward offset in base pixels (at 96 DPI)
    BYTE alphaDark;     // Pass alpha for dark mode (0-255)
    BYTE alphaLight;    // Pass alpha for light mode (0-255)
};

static const ThumbnailShadowPass kThumbnailShadowPasses[5] = {
    { 8, 1,  3, 2 },  // Pass 0: Wide ambient feather
    { 6, 1,  5, 3 },  // Pass 1: Ambient diffusion body
    { 4, 2,  8, 5 },  // Pass 2: Directional key body
    { 2, 2, 11, 7 },  // Pass 3: Directional core
    { 1, 2, 14, 9 }   // Pass 4: Contact occlusion edge
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
    float yOffScale = 1.0f;
    if (elevationScale > 1.0001f) {
        float elevProg = (elevationScale - 1.0f) / SWS_HOVER_ZOOM_DELTA;
        if (elevProg < 0.0f) elevProg = 0.0f;
        if (elevProg > 1.0f) elevProg = 1.0f;
        spreadScale = 1.0f + elevProg * 0.15f;
        yOffScale   = 1.0f + elevProg * 0.25f;
    }

    // During active layout transition (280ms), render a single-pass ambient shadow
    // to maintain 144Hz frame pacing without CPU rasterization stutter.
    if (g_layoutTransition.active) {
        Gdiplus::Graphics gfx(hdc);
        gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        BYTE baseAlpha = g_isDarkMode ? 24 : 16;
        BYTE shadowAlpha = (BYTE)roundf(baseAlpha * alphaMult);
        if (shadowAlpha == 0) return;
        Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(shadowAlpha, 0, 0, 0));
        int sp = DpiScale((int)roundf(3.0f * spreadScale), g_dpiX);
        if (sp < 1) sp = 1;
        int yOff = DpiScale((int)roundf(2.0f * yOffScale), g_dpiY);
        Gdiplus::REAL sx = (Gdiplus::REAL)(rc.left - sp);
        Gdiplus::REAL sy = (Gdiplus::REAL)(rc.top - sp + yOff);
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
        int yOff = DpiScale((int)roundf(pass.baseYOffset * yOffScale), g_dpiY);

        Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(shadowAlpha, 0, 0, 0));
        Gdiplus::REAL sx = (Gdiplus::REAL)(rc.left - sp);
        Gdiplus::REAL sy = (Gdiplus::REAL)(rc.top - sp + yOff);
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

    // Drop shadow behind the thumbnail (below the DWM thumbnail layer).
    if (g_settings.showThumbnails && g_settings.showThumbnailShadow &&
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
            } else if (g_hTheme) {
                DTTOPTS opts = { sizeof(DTTOPTS) };
                opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
                opts.crText = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
                DrawThemeTextEx(g_hTheme, hdc, 0, 0, e.title, -1,
                    DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX, &rcText, &opts);
            } else {
                SetTextColor(hdc, g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT);
                DrawTextW(hdc, e.title, -1, &rcText,
                          DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
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
    int iconY = headerTop + (rowTitleH - iconSz) / 2;

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
        if (alpha < 0.99f) {
            Gdiplus::Graphics gfx(hdc);
            Gdiplus::Bitmap bmp(e.hIcon);
            Gdiplus::ImageAttributes imgAtt;
            Gdiplus::ColorMatrix cm = {{
                { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, alpha, 0.0f },
                { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }
            }};
            imgAtt.SetColorMatrix(&cm);
            gfx.DrawImage(&bmp, Gdiplus::Rect(iconX, iconY, iconSz, iconSz),
                          0, 0, bmp.GetWidth(), bmp.GetHeight(), Gdiplus::UnitPixel, &imgAtt);
        } else {
            DrawIconEx(hdc, iconX, iconY, e.hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
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
        } else if (g_hTheme) {
            DTTOPTS opts = { sizeof(DTTOPTS) };
            opts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
            opts.crText = g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT;
            DrawThemeTextEx(g_hTheme, hdc, 0, 0, e.title, -1,
                DT_SINGLELINE | (HeaderIsVertical() ? DT_CENTER : DT_VCENTER) | DT_END_ELLIPSIS | DT_NOPREFIX, &rcText, &opts);
        } else {
            SetTextColor(hdc, g_isDarkMode ? SWS_TEXT_DARK : SWS_TEXT_LIGHT);
            DrawTextW(hdc, e.title, -1, &rcText,
                      DT_SINGLELINE | (HeaderIsVertical() ? DT_CENTER : DT_VCENTER) | DT_END_ELLIPSIS | DT_NOPREFIX);
        }
    }
}

static void DrawDockContentInner(HDC hdc, bool fillBg, HWND hWnd, bool includeSelectionFill) {
    RECT rcClient; GetClientRect(g_hSwitcher, &rcClient);
    int w = rcClient.right, h = rcClient.bottom;

    if (fillBg) {
        BYTE bgA = (BYTE)(g_settings.opacity * 255 / 100);
        if (bgA == 0) bgA = 1; // Prevent full transparency click-through
        COLORREF bgC = GetBgColor();
        BYTE bgR = GetRValue(bgC), bgG = GetGValue(bgC), bgB = GetBValue(bgC);
        RGBQUAD bgPx = { (BYTE)(bgB*bgA/255), (BYTE)(bgG*bgA/255), (BYTE)(bgR*bgA/255), bgA };
        BITMAPINFO bgBi = {}; bgBi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bgBi.bmiHeader.biWidth = 1; bgBi.bmiHeader.biHeight = 1;
        bgBi.bmiHeader.biPlanes = 1; bgBi.bmiHeader.biBitCount = 32; bgBi.bmiHeader.biCompression = BI_RGB;
        StretchDIBits(hdc, 0, 0, w, h, 0, 0, 1, 1, &bgPx, &bgBi, DIB_RGB_COLORS, SRCCOPY);
    }

    HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFont);
    SetBkMode(hdc, TRANSPARENT);

    // 1. Draw animated selection background fill (underneath icons in the strip)
    if (includeSelectionFill && g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        if (HighlightHasFill()) {
            DrawSelectionFillF(hdc, g_animSelectionCurrent);
        }
    }

    int masterPadX = DpiScale(g_settings.switcherPadding, g_dpiX);
    int cornerRadius = GetThumbnailCornerRadiusPx();

    // 2. Draw Central Preview shadow / card backdrop
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
        if (g_settings.showThumbnailShadow && shadowAlphaMult > 0.01f) {
            DrawThumbnailShadow(hdc, shadowRc, cornerRadius, shadowAlphaMult);
        }
        if (cornerRadius > 0 && ThemeIs(L"none") && g_settings.opacity >= 99) {
            MaskRectCorners(hdc, shadowRc, cornerRadius);
        }
    }

    // 3. Draw 1px subtle divider line between dock icon strip and content area
    {
        Gdiplus::Graphics gfx(hdc);
        gfx.SetSmoothingMode(Gdiplus::SmoothingModeNone);
        COLORREF divCol = g_isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
        BYTE divAlpha = g_isDarkMode ? 24 : 18; // ~9.5% dark / ~7% light subtle Fluent divider
        Gdiplus::SolidBrush divBrush(Gdiplus::Color(divAlpha, GetRValue(divCol), GetGValue(divCol), GetBValue(divCol)));

        int divX = masterPadX + DpiScale(12, g_dpiX);
        int divW = w - 2 * (masterPadX + DpiScale(12, g_dpiX));
        if (divW > 0) {
            if (DockIconIsTop()) {
                int divY = g_rcDockIconStrip.bottom + DpiScale(5, g_dpiY);
                gfx.FillRectangle(&divBrush, divX, divY, divW, 1);
            } else {
                int divY = g_rcDockIconStrip.top - DpiScale(6, g_dpiY);
                gfx.FillRectangle(&divBrush, divX, divY, divW, 1);
            }
        }
    }

    // 4. Draw icons in the horizontal strip
    int iconSz = DpiScale(g_settings.dockIconSize > 0 ? g_settings.dockIconSize : 48, g_dpiX);
    for (int i = 0; i < (int)g_windows.size(); i++) {
        WindowEntry& e = g_windows[i];
        if (IsWindowTruncated(i)) continue;
        if (e.rcCell.right <= e.rcCell.left || e.rcCell.bottom <= e.rcCell.top) continue;

        int cellW = e.rcCell.right - e.rcCell.left;
        int cellH = e.rcCell.bottom - e.rcCell.top;
        int iconX = e.rcCell.left + (cellW - iconSz) / 2;
        int iconY = e.rcCell.top + (cellH - iconSz) / 2;

        e.drawnIconX = iconX;
        e.drawnIconY = iconY;
        e.drawnIconSz = iconSz;

        float itemAlpha = (g_layoutTransition.active && e.isNewEntry) ? e.enterAlpha : 1.0f;
        if (itemAlpha < 0.99f) {
            Gdiplus::Graphics gfx(hdc);
            Gdiplus::Bitmap bmp(e.hIcon);
            Gdiplus::ImageAttributes imgAtt;
            Gdiplus::ColorMatrix cm = {{
                { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, itemAlpha, 0.0f },
                { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }
            }};
            imgAtt.SetColorMatrix(&cm);
            gfx.DrawImage(&bmp, Gdiplus::Rect(iconX, iconY, iconSz, iconSz),
                          0, 0, bmp.GetWidth(), bmp.GetHeight(), Gdiplus::UnitPixel, &imgAtt);
        } else {
            DrawIconEx(hdc, iconX, iconY, e.hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
        }
    }

    // 4b. Draw departing icons in dock strip if layout transition is active
    if (g_layoutTransition.active && !g_layoutTransition.departingItems.empty()) {
        for (const auto& dep : g_layoutTransition.departingItems) {
            if (dep.alpha <= 0.01f || !dep.hIcon) continue;
            int cellW = dep.rcCellCurrent.right - dep.rcCellCurrent.left;
            int cellH = dep.rcCellCurrent.bottom - dep.rcCellCurrent.top;
            if (cellW <= 0 || cellH <= 0) continue;
            int iconX = dep.rcCellCurrent.left + (cellW - iconSz) / 2;
            int iconY = dep.rcCellCurrent.top + (cellH - iconSz) / 2;

            Gdiplus::Graphics gfx(hdc);
            Gdiplus::Bitmap bmp(dep.hIcon);
            Gdiplus::ImageAttributes imgAtt;
            Gdiplus::ColorMatrix cm = {{
                { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, dep.alpha, 0.0f },
                { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }
            }};
            imgAtt.SetColorMatrix(&cm);
            gfx.DrawImage(&bmp, Gdiplus::Rect(iconX, iconY, iconSz, iconSz),
                          0, 0, bmp.GetWidth(), bmp.GetHeight(), Gdiplus::UnitPixel, &imgAtt);
        }
    }

    // 5. Draw centered window title in g_rcDockTitleBar
    if (g_settings.showTitle && g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        const auto& selWnd = g_windows[g_selectedIndex];
        RECT rcText = g_rcDockTitleBar;
        rcText.left += DpiScale(20, g_dpiX);
        rcText.right -= DpiScale(20, g_dpiX);
        float titleAlpha = 1.0f;
        if (g_dockPreviewSlide.active) {
            int offX = (int)roundf(g_dockPreviewSlide.currentOffset);
            rcText.left += offX;
            rcText.right += offX;
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

    if (fillBg) {
        BYTE bgA = (BYTE)(g_settings.opacity * 255 / 100);
        if (bgA == 0) bgA = 1; // Prevent full transparency click-through
        COLORREF bgC = GetBgColor();
        BYTE bgR = GetRValue(bgC), bgG = GetGValue(bgC), bgB = GetBValue(bgC);
        RGBQUAD bgPx = { (BYTE)(bgB*bgA/255), (BYTE)(bgG*bgA/255), (BYTE)(bgR*bgA/255), bgA };
        BITMAPINFO bgBi = {}; bgBi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bgBi.bmiHeader.biWidth = 1; bgBi.bmiHeader.biHeight = 1;
        bgBi.bmiHeader.biPlanes = 1; bgBi.bmiHeader.biBitCount = 32; bgBi.bmiHeader.biCompression = BI_RGB;
        StretchDIBits(hdc, 0, 0, w, h, 0, 0, 1, 1, &bgPx, &bgBi, DIB_RGB_COLORS, SRCCOPY);
    }

    HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFont);
    SetBkMode(hdc, TRANSPARENT);

    // Draw animated selection background fill (underneath thumbnails)
    if (includeSelectionFill && g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        if (HighlightHasFill()) {
            DrawSelectionFillF(hdc, g_animSelectionCurrent);
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

static void DrawBadgeIconOverlay(HDC hdc, const RECT& rcThumbActual, HICON hIcon, int* pOutIconX = NULL, int* pOutIconY = NULL, int* pOutIconSz = NULL) {
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

    Gdiplus::Graphics gfx(hdc);
    gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    if (g_settings.showBadgeIconBackground) {
        int bgSize = iconSz + badgePad * 2;
        int bgX = bIconX - badgePad;
        int bgY = bIconY - badgePad;
        
        COLORREF bgC = GetIconBackgroundColor();
        int op = g_isDarkMode ? g_settings.iconBgOpacityDark : g_settings.iconBgOpacityLight;
        int alpha = (op * 255) / 100;
        Gdiplus::SolidBrush bgBrush(Gdiplus::Color(alpha, GetRValue(bgC), GetGValue(bgC), GetBValue(bgC)));
        
        Gdiplus::REAL r = (Gdiplus::REAL)GetBadgeIconBackgroundCornerRadiusPx(bgSize / 2);
        
        if (g_settings.showBadgeIconBackgroundShadow) {
            for (int pass = 5; pass > 0; --pass) {
                int shadowAlpha = 15 - (pass * 2);
                if (shadowAlpha < 1) shadowAlpha = 1;
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
        DrawIconEx(hdc, bIconX, bIconY, hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
    } else {
        Gdiplus::Bitmap* pBmp = CreateIconShadowBitmap(hIcon, iconSz, iconSz, 0.08f);
        if (pBmp) {
            int dx[] = { 0, 1, 0, -1, 1 };
            int dy[] = { 1, 0, -1, 0, 1 };
            for (int p = 0; p < 5; ++p) {
                gfx.DrawImage(pBmp, bIconX + DpiScale(dx[p], g_dpiX), bIconY + DpiScale(dy[p] + 2, g_dpiY), iconSz, iconSz);
            }
            delete pBmp;
        }
        DrawIconEx(hdc, bIconX, bIconY, hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
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

            DrawBadgeIconOverlay(hdc, snapThumb, snap.hIcon);
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
            DrawBadgeIconOverlay(hdc, depThumb, dep.hIcon);
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
            DrawBadgeIconOverlay(hdc, rcThumbActual, e.hIcon, &drawnIconX, &drawnIconY, &drawnIconSz);
        }

        // Close button (rendered for any entry with closeBtnAlpha > 0.01f, enabling smooth cross-fades between entries)
        if (g_settings.showCloseButton && e.closeBtnAlpha > 0.01f) {
            RECT btnRc = GetCloseButtonRect(rcCell, rcThumbActual, rcThumbSlot);
            float hoverPlateAlpha = (i == g_hoverIndex && g_hoverWnd == hWnd && g_isCloseHovered) ? g_animCloseBtnHoverAlpha : 0.0f;
            bool isBtnPressed = (i == g_hoverIndex && g_hoverWnd == hWnd && g_isClosePressed);
            DrawCloseButton(hdc, btnRc, e.closeBtnAlpha, hoverPlateAlpha, isBtnPressed);
        }

        // Grouped window count badge
        if (g_settings.showGroupIndicator && g_settings.showApplications &&
            e.groupWindows.size() > 1) {
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
            if (g_settings.applyToGroupIndicator && g_settings.fontFamily[0]) {
                family = g_settings.fontFamily;
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

            // Position: top-right area of the icon
            int badgeX, badgeY;
            if (drawnIconSz > 0) {
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
            int alpha = (op * 255) / 100;
            Gdiplus::SolidBrush pillBrush(Gdiplus::Color(alpha, GetRValue(bgC), GetGValue(bgC), GetBValue(bgC)));
            Gdiplus::REAL pillRadius = (Gdiplus::REAL)GetGroupIndicatorCornerRadiusPx(badgeH / 2);
            
            if (g_settings.showGroupIndicatorShadow) {
                for (int pass = 5; pass > 0; --pass) {
                    int shadowAlpha = 15 - (pass * 2);
                    if (shadowAlpha < 1) shadowAlpha = 1;
                    Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(shadowAlpha, 0, 0, 0));
                    int sp = pass;
                    Gdiplus::REAL sx = (Gdiplus::REAL)(badgeX - sp);
                    Gdiplus::REAL sy = (Gdiplus::REAL)(badgeY - sp + 1);
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
            Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, GetRValue(txtC), GetGValue(txtC), GetBValue(txtC)));
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
            RectF selRc = (g_animSelectionActive && AreAnimationsGloballyEnabled() && g_settings.enableSelectionAnimation)
                          ? g_animSelectionCurrent
                          : ToRectF(g_windows[g_selectedIndex].rcCell);
            if (offX != 0 || offY != 0) {
                selRc.left += (float)offX;
                selRc.right += (float)offX;
                selRc.top += (float)offY;
                selRc.bottom += (float)offY;
            }
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
            float slideDist = (float)DpiScale(4, verticalLayout ? g_dpiX : g_dpiY) * (1.0f - alpha);
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
            // Hover: Smooth bloom to primary text opacity (100% alpha) via 80ms/120ms eased transition
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
        bool dwmDrawsBorder = IsWin11OrGreater() && (ThemeIs(L"mica") || g_nativeBackdropActive);

        if (!dwmDrawsBorder) {
            SelectClipRgn(hdc, NULL);
            RECT wRc; GetClientRect(hWnd, &wRc);
            int winRadius = GetWindowCornerRadiusPx();
            DrawSwitcherOuterBorder(hdc, wRc.right, wRc.bottom, winRadius);
        }
    }
}

static void PaintSwitcherOverlay() {
    if (!g_hCloseBtnWnd || !g_isVisible) return;
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

    // Fully clear persistent overlay bitmap so previous hover lines and contours never leave ghost traces
    if (s_cachedOverlayBits) {
        memset(s_cachedOverlayBits, 0, (size_t)w * h * sizeof(DWORD));
    }

    int radius = GetWindowCornerRadiusPx();
    HRGN hClip = GetCachedRoundRectRgn(w, h, radius);
    SelectClipRgn(s_cachedOverlayDC, hClip);

    DrawSwitcherOverlay(s_cachedOverlayDC, targetWnd);

    POINT ptSrc = {0,0}; SIZE sz = {w, h};
    int currentOffsetY = GetAnimationOffsetY();
    int baseX = 0;
    int baseY = 0;
    if (g_isPendingShow && targetWnd == g_hSwitcher && (g_pendingSwitcherRect.right > g_pendingSwitcherRect.left)) {
        baseX = g_pendingSwitcherRect.left;
        baseY = g_pendingSwitcherRect.top;
    } else if (targetWnd) {
        RECT wr; GetWindowRect(targetWnd, &wr);
        baseX = wr.left;
        baseY = wr.top;
        if (targetWnd == g_hSwitcher) {
            g_switcherBaseX = baseX;
            g_switcherBaseY = baseY;
            g_switcherBaseInitialized = true;
        }
    }
    POINT ptDst = { baseX, baseY + currentOffsetY };
    float combinedAlpha = g_animEntranceCurrentAlpha * g_animExitCurrentAlpha;
    if (combinedAlpha < 0.0f) combinedAlpha = 0.0f;
    if (combinedAlpha > 1.0f) combinedAlpha = 1.0f;
    BYTE finalAlpha = (BYTE)roundf(combinedAlpha * 255.0f);
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, finalAlpha, AC_SRC_ALPHA};
    HDC hdcScreen = GetDC(NULL);
    UpdateLayeredWindow(g_hCloseBtnWnd, hdcScreen, &ptDst, &sz, s_cachedOverlayDC, &ptSrc, 0, &bf, ULW_ALPHA);
    ReleaseDC(NULL, hdcScreen);
}

static void PaintSwitcher() {
    if (!g_hSwitcher || !g_isVisible) return;
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
                HRGN hClip = GetCachedRoundRectRgn(w, h, radius);
                SelectClipRgn(s_cachedStaticDC, hClip);
                DrawSwitcherStaticContent(s_cachedStaticDC, true, g_hSwitcher);
                g_staticContentDirty = false;
            }

            // Blit pre-rendered static content in <0.05ms
            BitBlt(s_cachedMemDC, 0, 0, w, h, s_cachedStaticDC, 0, 0, SRCCOPY);

            // Dynamic thumbnail drop shadow for any zoomed/animating thumbnails
            if (g_settings.showThumbnails && g_settings.showThumbnailShadow && ThumbnailHoverIsZoom()) {
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

            // Draw moving selection highlight fill on top of background
            HRGN hClip = GetCachedRoundRectRgn(w, h, radius);
            SelectClipRgn(s_cachedMemDC, hClip);
            if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() && HighlightHasFill()) {
                RectF fillRc = (g_animSelectionActive && AreAnimationsGloballyEnabled() && g_settings.enableSelectionAnimation)
                               ? g_animSelectionCurrent
                               : ToRectF(g_windows[g_selectedIndex].rcCell);
                DrawSelectionFillF(s_cachedMemDC, fillRc);
                if (DockLayoutActive()) {
                    auto& e = g_windows[g_selectedIndex];
                    if (!IsWindowTruncated(g_selectedIndex) && e.drawnIconSz > 0) {
                        DrawIconEx(s_cachedMemDC, e.drawnIconX, e.drawnIconY, e.hIcon, e.drawnIconSz, e.drawnIconSz, 0, NULL, DI_NORMAL);
                    }
                }
            }
        } else {
            // Active scroll/page transition: ultra-fast BitBlt composite from pre-rendered dual canvases (<0.1ms)
            if (s_cachedMemBits) {
                memset(s_cachedMemBits, 0, (size_t)w * h * sizeof(DWORD));
            }
            if (DockLayoutActive()) {
                // 1. In Dock Layout, the switcher background, central preview shadow, divider line,
                // and window title are stationary and must remain 100% visible throughout the transition.
                HRGN hWndClip = GetCachedRoundRectRgn(w, h, radius);
                SelectClipRgn(s_cachedMemDC, hWndClip);
                if (s_cachedScrollToDC) {
                    BitBlt(s_cachedMemDC, 0, 0, w, h, s_cachedScrollToDC, 0, 0, SRCCOPY);
                }

                // 2. Clip strictly to the dock icon strip for sliding the icons
                HRGN hStripClip = CreateRectRgn(g_rcDockIconStrip.left, g_rcDockIconStrip.top, g_rcDockIconStrip.right, g_rcDockIconStrip.bottom);
                SelectClipRgn(s_cachedMemDC, hStripClip);

                // Erase dock strip background with bg color so moving icons blend cleanly
                BYTE bgA = (BYTE)(g_settings.opacity * 255 / 100);
                if (bgA == 0) bgA = 1;
                COLORREF bgC = GetBgColor();
                BYTE bgR = GetRValue(bgC), bgG = GetGValue(bgC), bgB = GetBValue(bgC);
                RGBQUAD bgPx = { (BYTE)(bgB*bgA/255), (BYTE)(bgG*bgA/255), (BYTE)(bgR*bgA/255), bgA };
                BITMAPINFO bgBi = {}; bgBi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bgBi.bmiHeader.biWidth = 1; bgBi.bmiHeader.biHeight = 1;
                bgBi.bmiHeader.biPlanes = 1; bgBi.bmiHeader.biBitCount = 32; bgBi.bmiHeader.biCompression = BI_RGB;
                int stripW = g_rcDockIconStrip.right - g_rcDockIconStrip.left;
                int stripH = g_rcDockIconStrip.bottom - g_rcDockIconStrip.top;
                StretchDIBits(s_cachedMemDC, g_rcDockIconStrip.left, g_rcDockIconStrip.top, stripW, stripH,
                              0, 0, 1, 1, &bgPx, &bgBi, DIB_RGB_COLORS, SRCCOPY);

                int offX = (int)roundf(g_scrollTransition.offsetCurrentX);
                int outOffX = offX - g_scrollTransition.travelDistanceX;

                if (s_cachedScrollFromDC) {
                    BitBlt(s_cachedMemDC, outOffX, 0, w, h, s_cachedScrollFromDC, 0, 0, SRCCOPY);
                }
                if (s_cachedScrollToDC) {
                    BitBlt(s_cachedMemDC, offX, 0, w, h, s_cachedScrollToDC, 0, 0, SRCCOPY);
                }

                if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() && HighlightHasFill()) {
                    RectF selRc = (g_animSelectionActive && AreAnimationsGloballyEnabled() && g_settings.enableSelectionAnimation)
                                  ? g_animSelectionCurrent
                                  : ToRectF(g_windows[g_selectedIndex].rcCell);
                    selRc.left += (float)offX;
                    selRc.right += (float)offX;
                    DrawSelectionFillF(s_cachedMemDC, selRc);
                    auto& e = g_windows[g_selectedIndex];
                    if (!IsWindowTruncated(g_selectedIndex) && e.drawnIconSz > 0) {
                        DrawIconEx(s_cachedMemDC, (int)roundf(e.drawnIconX + offX), (int)roundf(e.drawnIconY),
                                   e.hIcon, e.drawnIconSz, e.drawnIconSz, 0, NULL, DI_NORMAL);
                    }
                }

                SelectClipRgn(s_cachedMemDC, NULL);
                DeleteObject(hStripClip);
            } else {
                int masterPadX = DpiScale(g_settings.switcherPadding, g_dpiX);
                int masterPadY = DpiScale(g_settings.switcherPadding, g_dpiY);
                HRGN hContentClip = CreateRectRgn(masterPadX, masterPadY, w - masterPadX, h - masterPadY);
                if (radius > 0) {
                    HRGN hWndClip = GetCachedRoundRectRgn(w, h, radius);
                    CombineRgn(hContentClip, hContentClip, hWndClip, RGN_AND);
                }
                SelectClipRgn(s_cachedMemDC, hContentClip);

                int offX = (int)roundf(g_scrollTransition.offsetCurrentX);
                int offY = (int)roundf(g_scrollTransition.offsetCurrentY);
                int outOffX = offX - g_scrollTransition.travelDistanceX;
                int outOffY = offY - g_scrollTransition.travelDistanceY;

                if (s_cachedScrollFromDC) {
                    BitBlt(s_cachedMemDC, outOffX, outOffY, w, h, s_cachedScrollFromDC, 0, 0, SRCCOPY);
                }
                if (s_cachedScrollToDC) {
                    BitBlt(s_cachedMemDC, offX, offY, w, h, s_cachedScrollToDC, 0, 0, SRCCOPY);
                }

                if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() && HighlightHasFill()) {
                    RectF selRc = (g_animSelectionActive && AreAnimationsGloballyEnabled() && g_settings.enableSelectionAnimation)
                                  ? g_animSelectionCurrent
                                  : ToRectF(g_windows[g_selectedIndex].rcCell);
                    selRc.left += (float)offX;
                    selRc.right += (float)offX;
                    selRc.top += (float)offY;
                    selRc.bottom += (float)offY;
                    DrawSelectionFillF(s_cachedMemDC, selRc);
                }

                SelectClipRgn(s_cachedMemDC, NULL);
                DeleteObject(hContentClip);
            }
        }
        if (radius > 0) {
            RECT rcFull = { 0, 0, w, h };
            MaskRectCorners(s_cachedMemDC, rcFull, radius);
        }

        POINT ptSrc = {0,0}; SIZE sz = {w, h};
        int currentOffsetY = GetAnimationOffsetY();
        int baseX = g_switcherBaseX;
        int baseY = g_switcherBaseY;
        if (g_isPendingShow && (g_pendingSwitcherRect.right > g_pendingSwitcherRect.left)) {
            baseX = g_pendingSwitcherRect.left;
            baseY = g_pendingSwitcherRect.top;
        } else if (!g_switcherBaseInitialized) {
            RECT wr; GetWindowRect(g_hSwitcher, &wr);
            baseX = wr.left;
            baseY = wr.top;
        }
        POINT ptDst = { baseX, baseY + currentOffsetY };
        float combinedAlpha = g_animEntranceCurrentAlpha * g_animExitCurrentAlpha;
        if (combinedAlpha < 0.0f) combinedAlpha = 0.0f;
        if (combinedAlpha > 1.0f) combinedAlpha = 1.0f;
        BYTE finalAlpha = (BYTE)roundf(combinedAlpha * 255.0f);
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, finalAlpha, AC_SRC_ALPHA};
        HDC hdcScreen = GetDC(NULL);
        UpdateLayeredWindow(g_hSwitcher, hdcScreen, &ptDst, &sz, s_cachedMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
        for (HWND hMirror : g_hMirrorSwitchers) {
            if (IsWindow(hMirror)) {
                DrawSwitcherContent(s_cachedMemDC, true, hMirror);
                RECT mwr; GetWindowRect(hMirror, &mwr);
                POINT mPtDst = { mwr.left, mwr.top + currentOffsetY };
                UpdateLayeredWindow(hMirror, hdcScreen, &mPtDst, &sz, s_cachedMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
            }
        }
        ReleaseDC(NULL, hdcScreen);
        PaintSwitcherOverlay();
    } else {
        // Acrylic / Mica: modulate window alpha via SetLayeredWindowAttributes
        // in lockstep with the layered overlay window and live DWM thumbnails.
        float combinedAlpha = g_animEntranceCurrentAlpha * g_animExitCurrentAlpha;
        if (combinedAlpha < 0.0f) combinedAlpha = 0.0f;
        if (combinedAlpha > 1.0f) combinedAlpha = 1.0f;
        BYTE finalAlpha = (BYTE)roundf(combinedAlpha * 255.0f);

        SetLayeredWindowAttributes(g_hSwitcher, 0, finalAlpha, LWA_ALPHA);
        InvalidateRect(g_hSwitcher, NULL, FALSE);
        UpdateWindow(g_hSwitcher);
        for (HWND hMirror : g_hMirrorSwitchers) {
            if (IsWindow(hMirror)) {
                SetLayeredWindowAttributes(hMirror, 0, finalAlpha, LWA_ALPHA);
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
    }
    if (s_hWinEventHook) {
        UnhookWinEvent(s_hWinEventHook);
        s_hWinEventHook = NULL;
    }

    g_isPendingShow = false;
    g_pendingSwitcherRect = { 0, 0, 0, 0 };
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_isVisible && wParam == WM_MOUSEWHEEL) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
        bool ok = ScrollIs(L"always") || (ScrollIs(L"stickyOnly") && g_isSticky);
        if (ok) {
            int dir = (short)HIWORD(pMouseStruct->mouseData) > 0 ? -1 : 1;
            if (g_settings.reverseScrollDirection) dir = -dir;
            
            bool modActive = false;
            if (wcscmp(g_settings.scrollSecondaryModifier, L"shift") == 0) modActive = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            else if (wcscmp(g_settings.scrollSecondaryModifier, L"ctrl") == 0) modActive = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            else if (wcscmp(g_settings.scrollSecondaryModifier, L"alt") == 0) modActive = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

            const WCHAR* actionStr = modActive ? g_settings.scrollSecondaryAction : g_settings.scrollWheelAction;
            
            int action = 0; // 0 = none, 1 = selection, 2 = page
            if (wcscmp(actionStr, L"selection") == 0) action = 1;
            else if (wcscmp(actionStr, L"page") == 0) action = 2;

            if (action > 0) {
                PostMessage(g_hSwitcher, WM_SWS_SCROLL, (WPARAM)dir, (LPARAM)action);
                return 1;
            }
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

static void RevealPendingSwitcher() {
    if (!g_isPendingShow || !g_hSwitcher) {
        return;
    }

    KillTimer(g_hSwitcher, SWS_SHOW_DELAY_TIMER_ID);

    g_isPendingShow = false;
    g_isVisible = true;
    if (!g_hMouseHook) {
        g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
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

    SetWindowPos(g_hSwitcher, HWND_TOPMOST, x, y, w, h, SWP_FRAMECHANGED | SWP_NOACTIVATE);
    CreateMirrorSwitchers();
    if (g_hCloseBtnWnd) {
        SetWindowPos(g_hCloseBtnWnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);
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

    g_animExitActive = false;
    g_animExitProgress = 1.0f;
    g_animExitCurrentAlpha = 1.0f;

    if (AreAnimationsGloballyEnabled() && g_settings.enableEntranceAnimation) {
        g_animEntranceActive = true;
        g_animEntranceProgress = 0.0f;
        g_animEntranceDuration = 0.240f;
        g_animEntranceCurrentAlpha = 0.0f;
    } else {
        g_animEntranceActive = false;
        g_animEntranceProgress = 1.0f;
    }

    UpdateChevronLayout(g_hSwitcher);
    UpdateChevronAnimationTargets(true);

    RegisterThumbnails();
    // Render initial frame 0 while window is hidden so no previous frame's border flashes
    PaintSwitcher();

    if (g_hCloseBtnWnd) {
        ShowWindow(g_hCloseBtnWnd, SW_SHOWNA);
    }
    ShowWindow(g_hSwitcher, SW_SHOWNA);
    SetForegroundWindow(g_hSwitcher);

    if (g_animEntranceActive) {
        StartAnimationTicker();
    } else {
        PaintSwitcher();
        UpdateWindow(g_hSwitcher);
        if (g_hCloseBtnWnd) UpdateWindow(g_hCloseBtnWnd);
    }

    if (!g_isSticky) {
        SetTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID, 50, NULL);
    } else {
        KillTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID);
    }
}

static void ApplyThemeToWindow(HWND hWnd) {
    g_nativeBackdropActive = false;

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
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        return;
    }

    // --- Non-layered path (Mica / Acrylic) ---
    // Ensure window has WS_EX_LAYERED so SetLayeredWindowAttributes can modulate
    // overall window opacity during WinUI 3 entrance / exit animations in lockstep
    // with the overlay and live DWM thumbnails.
    LONG_PTR exs = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (!(exs & WS_EX_LAYERED)) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exs | WS_EX_LAYERED);
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
    SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

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

    bool useNativeBackdrop = false;

    if (ThemeIs(L"mica")) {
        if (IsWin11OrGreater()) {
            int micaVal = 2; // DWMSBT_MAINWINDOW
            HRESULT hr = DwmSetWindowAttribute(hWnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &micaVal, sizeof(micaVal));
            if (FAILED(hr)) {
                int oldMicaVal = 1;
                hr = DwmSetWindowAttribute(hWnd, 1029 /* DWMWA_MICA_EFFECT */, &oldMicaVal, sizeof(oldMicaVal));
            }
            if (FAILED(hr)) {
                // Fallback to layered window if Mica is unsupported
                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            }
            SendMessage(hWnd, WM_NCACTIVATE, TRUE, 0);
        }
    } else if (ThemeIs(L"backdrop")) {
        if (IsWin11OrGreater()) {
            int backdropVal = 3; // DWMSBT_TRANSIENTWINDOW (Desktop Acrylic)
            HRESULT hr = DwmSetWindowAttribute(hWnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &backdropVal, sizeof(backdropVal));
            if (SUCCEEDED(hr)) {
                useNativeBackdrop = true;
                g_nativeBackdropActive = true;
                int disableOldMica = 0;
                DwmSetWindowAttribute(hWnd, 1029 /* DWMWA_MICA_EFFECT */, &disableOldMica, sizeof(disableOldMica));
                SendMessage(hWnd, WM_NCACTIVATE, TRUE, 0);
            }
        }
        if (!useNativeBackdrop) {
            // Windows 10 or Win11 fallback: SetWindowCompositionAttribute
            if (IsWin11OrGreater()) {
                int noneVal = 1; // DWMSBT_NONE
                DwmSetWindowAttribute(hWnd, 38, &noneVal, sizeof(noneVal));
            }
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
    }

    MARGINS marGlassInset = (ThemeIs(L"mica") || useNativeBackdrop) ? MARGINS{-1, -1, -1, -1} : MARGINS{0, 0, 0, 0};
    DwmExtendFrameIntoClientArea(hWnd, &marGlassInset);

    SetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)GetStockObject(BLACK_BRUSH));

    if (IsWin11OrGreater()) {
        INT cp = GetCornerPref();
        DwmSetWindowAttribute(hWnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &cp, sizeof(cp));
        if (!g_settings.showSwitcherBorder) {
            COLORREF none = 0xFFFFFFFE; // DWMWA_COLOR_NONE
            DwmSetWindowAttribute(hWnd, 34 /* DWMWA_BORDER_COLOR */, &none, sizeof(none));
        } else {
            COLORREF dwmBorderColor = 0xFFFFFFFF; // DWMWA_COLOR_DEFAULT
            const WCHAR* borderMode = g_isDarkMode ? g_settings.borderColorModeDark : g_settings.borderColorModeLight;
            if (wcscmp(borderMode, L"accent") == 0 || wcscmp(borderMode, L"custom") == 0) {
                dwmBorderColor = GetContourColor();
            }
            DwmSetWindowAttribute(hWnd, 34 /* DWMWA_BORDER_COLOR */, &dwmBorderColor, sizeof(dwmBorderColor));
        }
    }

    SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

static BOOL WINAPI MirrorEnumProc(HMONITOR hM, HDC, LPRECT, LPARAM) {
    if (hM != g_hCurrentMonitor) {
        MONITORINFO mInfo = { sizeof(mInfo) };
        GetMonitorInfoW(hM, &mInfo);
        int mx, my;
        GetSwitcherPosition(mInfo.rcWork, &mx, &my);
        HWND hMirror = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED, SWS_CLASSNAME, L"", WS_POPUP | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, mx, my, g_winW, g_winH, g_hSwitcher, NULL, GetModuleHandle(NULL), NULL);
        if (hMirror) {
            ApplyThemeToWindow(hMirror);
            g_hMirrorSwitchers.push_back(hMirror);
            SetWindowPos(hMirror, HWND_TOPMOST, mx, my, g_winW, g_winH, SWP_NOACTIVATE);
            ShowWindow(hMirror, SW_SHOWNA);
            SetActiveWindow(hMirror);
        }
    }
    return TRUE;
}

static void DestroyMirrorSwitchers() {
    for (HWND hMirror : g_hMirrorSwitchers) {
        if (IsWindow(hMirror)) DestroyWindow(hMirror);
    }
    g_hMirrorSwitchers.clear();
}

static void CreateMirrorSwitchers() {
    if (wcscmp(g_settings.switcherDisplayBehavior, L"allMonitors") == 0 || g_showAllMonitors) {
        EnumDisplayMonitors(NULL, NULL, MirrorEnumProc, 0);
    }
}


static void ApplySwitcherRegion() {
    if (!g_hSwitcher) return;
    static bool s_hasActiveRgn = false;

    // Both Windows 11 (DWM hardware rounding) and Theme: none (per-pixel alpha layered window)
    // do not need GDI SetWindowRgn. Windows 10 Acrylic blur is a 90° rectangle, where SetWindowRgn
    // would only conflict with DWM composition.
    if (s_hasActiveRgn) {
        SetWindowRgn(g_hSwitcher, NULL, TRUE);
        s_hasActiveRgn = false;
    }
}

static void ShowSwitcher(bool sticky) {
    RefreshClientAreaAnimCache();
    DestroyMirrorSwitchers();

    POINT pt; GetCursorPos(&pt);
    HMONITOR hMon = (wcscmp(g_settings.switcherDisplayBehavior, L"primaryOnly") == 0) ?
                    MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY) :
                    MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    g_hCurrentMonitor = hMon;
    UpdateRefreshRateTiming();
    UnregisterThumbnails(); BuildWindowList();
    
    if (g_isAltBacktickSameApp) {
        WCHAR activeKey[MAX_PATH] = {0};
        GetWindowGroupKey(GetForegroundWindow(), activeKey, ARRAYSIZE(activeKey));
        if (activeKey[0]) {
            g_windows.erase(std::remove_if(g_windows.begin(), g_windows.end(),
                [&](const WindowEntry& e) {
                    WCHAR key[MAX_PATH] = {0};
                    GetWindowGroupKey(e.hWnd, key, ARRAYSIZE(key));
                    return wcscmp(key, activeKey) != 0;
                }),
                g_windows.end());
        }
        g_isAltBacktickSameApp = false;
    }

    if (g_windows.empty()) {
        HideSwitcher();
        return;
    }

    g_isDarkMode = ShouldUseDarkMode(); g_isSticky = sticky;

    g_layoutStartIndex = 0; // Always start from the first window on initial show
    g_drilledIn = false;
    g_savedAppList.clear();
    g_consumeEscUp = false;
    g_selectedIndex = (g_windows.size() > 1) ? 1 : 0;
    g_hoverIndex = -1;
    g_hoverThumbIndex = -1;
    g_hoverWnd = NULL;
    g_isCloseHovered = false;

    RegisterThumbnailsEarly();
    ComputeLayout(hMon);
    if (g_winW <= 0 || g_winH <= 0) return;
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

    if (g_settings.showDelay > 0 && !sticky) {
        g_isPendingShow = true;
        g_isVisible = false;

        // Ensure WS_EX_LAYERED is active so we can set 100% transparency
        LONG_PTR exStyle = GetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE);
        SetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);

        // Suppress DWM frame/shadow so no visual artifact appears anywhere on screen
        DWMNCRENDERINGPOLICY disabled = DWMNCRP_DISABLED;
        DwmSetWindowAttribute(g_hSwitcher, DWMWA_NCRENDERING_POLICY, &disabled, sizeof(disabled));

        // 100% transparent: zero pixels rendered
        SetLayeredWindowAttributes(g_hSwitcher, 0, 0, LWA_ALPHA);

        // Position directly at target coordinates (no off-screen coordinate guessing)
        SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
        ShowWindow(g_hSwitcher, SW_SHOWNA);

        // Establishing foreground ownership ensures UIPI does not block input tracking
        // (WM_KEYUP, GetAsyncKeyState) when invoked over elevated/Admin windows
        SetForegroundWindow(g_hSwitcher);

        SetTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID, 50, NULL);
        SetTimer(g_hSwitcher, SWS_SHOW_DELAY_TIMER_ID, g_settings.showDelay, NULL);
        return;
    }

    g_isPendingShow = false;
    g_pendingSwitcherRect = { 0, 0, 0, 0 };
    g_isVisible = true;
    if (!g_hMouseHook) {
        g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
    }

    ApplyThemeToWindow(g_hSwitcher);
    ApplySwitcherRegion();

    SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    CreateMirrorSwitchers();
    
    if (g_hCloseBtnWnd) {
        SetWindowPos(g_hCloseBtnWnd, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    }

    g_animExitActive = false;
    g_animExitProgress = 1.0f;
    g_animExitCurrentAlpha = 1.0f;

    if (AreAnimationsGloballyEnabled() && g_settings.enableEntranceAnimation) {
        g_animEntranceActive = true;
        g_animEntranceProgress = 0.0f;
        g_animEntranceDuration = 0.240f;
        g_animEntranceCurrentAlpha = 0.0f;
    } else {
        g_animEntranceActive = false;
        g_animEntranceProgress = 1.0f;
        g_animEntranceCurrentAlpha = 1.0f;
    }

    UpdateChevronLayout(g_hSwitcher);
    UpdateChevronAnimationTargets(true);
    RegisterThumbnails();
    // Render initial frame 0 while window is hidden so no previous frame's border flashes
    PaintSwitcher();

    if (g_hCloseBtnWnd) {
        ShowWindow(g_hCloseBtnWnd, SW_SHOWNA);
    }
    ShowWindow(g_hSwitcher, SW_SHOWNA);
    SetForegroundWindow(g_hSwitcher);

    if (g_animEntranceActive) {
        StartAnimationTicker();
    } else {
        PaintSwitcher();
        UpdateWindow(g_hSwitcher);
        if (g_hCloseBtnWnd) UpdateWindow(g_hCloseBtnWnd);
    }

    if (!sticky) {
        SetTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID, 50, NULL);
    }
}

static void HideSwitcher() {
    StopAnimationTicker();
    FinishAnimations();
    FreeCachedBuffers();

    g_showAllMonitors = false;
    CancelPendingShow();
    g_switcherBaseInitialized = false;

    DestroyMirrorSwitchers();

    UnregisterThumbnails();
    if (g_hCloseBtnWnd) {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 0, AC_SRC_ALPHA };
        UpdateLayeredWindow(g_hCloseBtnWnd, NULL, NULL, NULL, NULL, NULL, 0, &bf, ULW_ALPHA);
        ShowWindow(g_hCloseBtnWnd, SW_HIDE);
    }
    if (g_hSwitcher) {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 0, AC_SRC_ALPHA };
        UpdateLayeredWindow(g_hSwitcher, NULL, NULL, NULL, NULL, NULL, 0, &bf, ULW_ALPHA);
        ShowWindow(g_hSwitcher, SW_HIDE);
        LONG_PTR exStyle = GetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE);
        if (exStyle & WS_EX_TRANSPARENT) {
            SetWindowLongPtrW(g_hSwitcher, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
        }
    }

    g_animExitActive = false;
    g_animExitProgress = 1.0f;
    g_animExitCurrentAlpha = 1.0f;

    g_animChevronAlphaPrev = 0.0f;
    g_animChevronAlphaNext = 0.0f;
    g_animChevronAlphaTargetPrev = 0.0f;
    g_animChevronAlphaTargetNext = 0.0f;
    g_animChevronProgressPrev = 1.0f;
    g_animChevronProgressNext = 1.0f;
    g_animChevronHoverAlphaPrev = 0.0f;
    g_animChevronHoverAlphaNext = 0.0f;
    g_animCloseBtnAlpha = 0.0f;
    g_animCloseBtnHoverAlpha = 0.0f;
    for (auto& w : g_windows) {
        w.closeBtnAlpha = 0.0f;
    }
    g_isClosePressed = false;
    g_hoverChevron = 0;
    g_pressedChevron = 0;
    g_hoverIndex = -1;
    g_hoverThumbIndex = -1;
    g_hoverWnd = NULL;

    g_isVisible = false;
    g_isPendingShow = false;
    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = NULL;
    }
    g_isSticky = false;
    g_drilledIn = false;
    g_savedAppList.clear();
    g_consumeEscUp = false;
    g_isPaginatedView = false;
    if (g_hSwitcher) {
        KillTimer(g_hSwitcher, SWS_CLOSE_VERIFY_TIMER_ID);
        KillTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID);
    }
    s_pendingCloseWindows.clear();
    s_pendingCloseRetries = 0;
}

// Restores a window from iconic (minimized) state.
// ShowWindow(SW_RESTORE) works for standard windows, but UIPI blocks it
// for elevated (admin) windows. If the window remains iconic after ShowWindow,
// PostMessage(WM_SYSCOMMAND, SC_RESTORE) is used as a fallback because
// PostMessage with SC_RESTORE is permitted across integrity boundaries.
static void RestoreWindowIfIconic(HWND hWnd) {
    if (IsIconic(hWnd)) {
        ShowWindow(hWnd, SW_RESTORE);
        if (IsIconic(hWnd)) {
            PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
        }
    }
}

static void StartExitAnimation(bool activateSelectedWindow) {
    if ((!g_isVisible && !g_isPendingShow) || g_animExitActive) return;

    if (activateSelectedWindow && g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        HWND hT = g_windows[g_selectedIndex].hWnd;
        std::vector<HWND> groupWindows;
        if (g_settings.showApplications && g_settings.restoreAllWindows) {
            groupWindows = g_windows[g_selectedIndex].groupWindows;
        }
        
        // Restore sibling windows without stealing activation focus from target
        for (HWND hw : groupWindows) {
            if (IsWindow(hw) && hw != hT && IsIconic(hw)) {
                ShowWindow(hw, SW_SHOWNOACTIVATE);
                if (IsIconic(hw)) {
                    ShowWindowAsync(hw, SW_SHOWNOACTIVATE);
                }
            }
        }
        
        if (IsWindow(hT)) {
            HWND hP = GetLastActivePopup(hT);
            HWND hF = IsWindowVisible(hP) ? hP : hT;
            RestoreWindowIfIconic(hT);
            if (hF != hT) {
                RestoreWindowIfIconic(hF);
            }
            if (!SetForegroundWindow(hF)) SwitchToThisWindow(hF, TRUE);
            UpdateMruWindow(hT);
        }
    }

    if (!AreAnimationsGloballyEnabled()) {
        HideSwitcher();
        return;
    }

    g_animExitActive = true;
    g_animExitProgress = 0.0f;
    g_animExitDuration = 0.160f;
    g_animExitCurrentAlpha = 1.0f;

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
}

static void SwitchToSelected() {
    StartExitAnimation(true);
}

// Helper: recompute layout and reposition switcher window
static void RecomputeAndReposition() {
    // Purge any destroyed or non-iconic hidden windows that were closed or hidden silently
    if (!g_windows.empty()) {
        bool removedAny = false;
        for (int i = (int)g_windows.size() - 1; i >= 0; i--) {
            HWND h = g_windows[i].hWnd;
            if (!IsWindow(h) || (!IsWindowVisible(h) && !IsIconic(h))) {
                for (const auto& kv : g_windows[i].hThumbs) {
                    if (kv.second) DwmUnregisterThumbnail(kv.second);
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

    if (!g_scrollTransition.preservingThumbnails) {
        UnregisterThumbnails();
    }
    RegisterThumbnailsEarly();
    HMONITOR hMon = g_hCurrentMonitor
                    ? g_hCurrentMonitor
                    : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
    g_hCurrentMonitor = hMon;
    ComputeLayout(hMon);
    if (DockLayoutActive()) {
        UpdateDockPreviewForSelection();
    }
    UpdateChevronLayout(g_hSwitcher);
    UpdateChevronAnimationTargets(false);
    InvalidateStaticCache();
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(hMon, &mi);
    int cx, cy;
    GetSwitcherPosition(mi.rcWork, &cx, &cy);
    g_switcherBaseX = cx;
    g_switcherBaseY = cy;
    g_switcherBaseInitialized = true;

    if (g_isPendingShow) {
        g_pendingSwitcherRect = {
            cx,
            cy,
            cx + g_winW,
            cy + g_winH
        };
        return;
    } else {
        g_pendingSwitcherRect = { 0, 0, 0, 0 };
    }

    // Only call SetWindowPos when the size or position has actually changed.
    // An unconditional SetWindowPos sends WM_SIZE which triggers a WM_PAINT
    // before our own rendering pass completes, causing a momentary flicker.
    RECT curWndRect = {};
    GetWindowRect(g_hSwitcher, &curWndRect);
    bool posChanged = (curWndRect.left != cx || curWndRect.top != cy
                       || (curWndRect.right - curWndRect.left) != g_winW
                       || (curWndRect.bottom - curWndRect.top) != g_winH);
    if (posChanged) {
        SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    } else {
        SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH,
                     SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW);
    }
    for (HWND hMirror : g_hMirrorSwitchers) {
        if (!IsWindow(hMirror)) continue;
        MONITORINFO mInfo = { sizeof(mInfo) };
        GetMonitorInfoW(MonitorFromWindow(hMirror, MONITOR_DEFAULTTONEAREST), &mInfo);
        int mx, my; GetSwitcherPosition(mInfo.rcWork, &mx, &my);
        SetWindowPos(hMirror, HWND_TOPMOST, mx, my, g_winW, g_winH, SWP_NOACTIVATE);
    }
    if (g_hCloseBtnWnd && g_isVisible && !g_isPendingShow) {
        SetWindowPos(g_hCloseBtnWnd, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    }
    ApplySwitcherRegion();
    RegisterThumbnails();
    if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
        SnapSelectionTo(ToRectF(g_windows[g_selectedIndex].rcCell));
    }
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

    UnregisterThumbnails();  // release app-list thumbnails before stashing
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
        e.hIcon = LoadWindowIcon(hw);
        g_windows.push_back(std::move(e));
    }
    if (g_windows.empty()) {  // every window closed in the meantime; abort
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

// Leave the drilled-in window view and restore the grouped application list.
static void ExitAppGroup() {
    if (!g_drilledIn) return;
    UnregisterThumbnails();  // release drilled-window thumbnails
    g_windows = std::move(g_savedAppList);
    g_savedAppList.clear();
    g_selectedIndex = g_savedSelectedIndex;
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
    PaintSwitcher();
}

static void ToggleAppDrill() {
    if (g_drilledIn) ExitAppGroup();
    else EnterAppGroup();
}

// Linear navigation: Tab, Shift+Tab, Left, Right, Hotkeys, Scroll
static void CycleLinear(int delta) {
    if (g_windows.empty()) return;
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
            CaptureOutgoingSnapshot();
            int dir = (targetStart > g_layoutStartIndex) ? 1 : -1;
            g_layoutStartIndex = targetStart;
            RecomputeAndReposition();
            UpdateDockPreviewForSelection();
            TriggerScrollAnimationEx(dir, SCROLL_ROW);
            if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
                RectF r = ToRectF(g_windows[g_selectedIndex].rcCell);
                SnapSelectionTo(r);
            }
        } else {
            TriggerSelectionAnimation(prevSelected);
            UpdateDockPreviewForSelection();
        }
        UpdateChevronAnimationTargets(false);
        InvalidateStaticCache();
        PaintSwitcher();
        return;
    }

    // If the newly selected window is truncated, recompute layout
    if (IsWindowTruncated(g_selectedIndex)) {
        CaptureOutgoingSnapshot();

        int oldStart = g_layoutStartIndex;
        HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);

        // Dry-run pass from index 0 to find target line start without churning thumbnails
        g_isDryRunLayout = true;
        g_layoutStartIndex = 0;
        ComputeLayout(hMon);

        int targetStart = 0;
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
            RectF r = ToRectF(g_windows[g_selectedIndex].rcCell);
            SnapSelectionTo(r);
        }
    } else {
        TriggerSelectionAnimation(prevSelected);
    }
    PaintSwitcher();
}

static void CyclePage(int dir) {
    if (g_windows.empty()) return;
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
            g_layoutStartIndex = newStart;
            g_selectedIndex = (dir > 0) ? g_layoutStartIndex : std::min(n - 1, g_layoutStartIndex + visibleCount - 1);
            RecomputeAndReposition();
            UpdateDockPreviewForSelection();
            TriggerScrollAnimationEx(dir, SCROLL_PAGE);
            if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
                RectF r = ToRectF(g_windows[g_selectedIndex].rcCell);
                SnapSelectionTo(r);
            }
        }
        UpdateChevronAnimationTargets(false);
        InvalidateStaticCache();
        PaintSwitcher();
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
        RectF r = ToRectF(g_windows[g_selectedIndex].rcCell);
        SnapSelectionTo(r);
    }

    PaintSwitcher();
}

// Directional navigation: Up, Down (EP-style row-based with nearest-column match)
// Walks in layout placement order (from g_layoutStartIndex, wrapping) instead of raw list index.
static void CycleDirectional(int vertDelta) {
    if (g_windows.empty()) return;
    if (DockLayoutActive()) {
        CycleLinear(vertDelta);
        return;
    }
    int n = (int)g_windows.size();
    int prevSelected = g_selectedIndex;
    bool verticalLayout = LayoutIsVertical();

    // Build layout-order mapping: layoutOrder[0] is the first window placed visually
    auto buildLayoutOrder = [&](std::vector<int>& order) {
        order.resize(n);
        for (int idx = 0; idx < n; idx++)
            order[idx] = (g_layoutStartIndex + idx) % n;
    };

    std::vector<int> layoutOrder;
    buildLayoutOrder(layoutOrder);

    // Find current selection's position in layout order
    int layoutPos = 0;
    for (int idx = 0; idx < n; idx++) {
        if (layoutOrder[idx] == g_selectedIndex) { layoutPos = idx; break; }
    }

    // Save current selection's line anchor and perpendicular center.
    RECT rcPrev = g_windows[g_selectedIndex].rcCell;
    int prevLineCoord = verticalLayout ? rcPrev.left : rcPrev.top;
    int prevPerpCenter = verticalLayout ? (rcPrev.top + rcPrev.bottom) / 2 : (rcPrev.left + rcPrev.right) / 2;

    // Walk direction in layout order: DOWN = +1 (visually next), UP = -1 (visually prev)
    int layoutDelta = vertDelta;
    int current = -1;
    bool foundDifferentRow = false;

    for (int step = 0; step < n; step++) {
        int nextPos = ((layoutPos + (step + 1) * layoutDelta) % n + n) % n;
        int windowIdx = layoutOrder[nextPos];

        if (nextPos == layoutPos) break; // Wrapped all the way around

        // Target window is off-screen — scroll layout to reveal it.
        if (IsWindowTruncated(windowIdx)) {
            CaptureOutgoingSnapshot();

            int oldStart = g_layoutStartIndex;
            HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);

            // If wrapping back to top (vertDelta > 0 but windowIdx < g_layoutStartIndex),
            // start dry-run from index 0
            g_isDryRunLayout = true;
            if (vertDelta > 0 && windowIdx < g_layoutStartIndex) {
                g_layoutStartIndex = 0;
                ComputeLayout(hMon);
            }

            int targetStart = g_layoutStartIndex;
            int attempts = n;
            while (IsWindowTruncated(windowIdx) && attempts-- > 0) {
                int firstIdx = g_layoutStartIndex % n;
                int firstLineCoord2 = verticalLayout ? g_windows[firstIdx].rcCell.left : g_windows[firstIdx].rcCell.top;
                int newStart = g_layoutStartIndex;
                for (int k = 0; k < n; k++) {
                    int wi = (g_layoutStartIndex + k) % n;
                    if (IsWindowTruncated(wi)) break;
                    int lineCoord = verticalLayout ? g_windows[wi].rcCell.left : g_windows[wi].rcCell.top;
                    if (lineCoord != firstLineCoord2) {
                        newStart = wi;
                        break;
                    }
                }
                if (newStart == g_layoutStartIndex) {
                    targetStart = windowIdx;
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
            // If layoutStartIndex increased (e.g. scrolled down to reveal bottom row): dir = 1 (scroll to bottom).
            int dir;
            if (g_layoutStartIndex < oldStart) {
                dir = -1; // Scrolled back to top
            } else if (g_layoutStartIndex > oldStart) {
                dir = 1;  // Scrolled down to bottom
            } else {
                dir = (vertDelta >= 0) ? 1 : -1;
            }

            TriggerScrollAnimationEx(dir, SCROLL_ROW);

            // Rebuild layout order after recompute
            buildLayoutOrder(layoutOrder);
            current = windowIdx;
            foundDifferentRow = true;
            break;
        }

        int lineCoord = verticalLayout ? g_windows[windowIdx].rcCell.left : g_windows[windowIdx].rcCell.top;
        if (lineCoord != prevLineCoord) {
            current = windowIdx;
            foundDifferentRow = true;
            break;
        }
    }

    if (!foundDifferentRow) {
        // Only one line visible; nothing to jump to.
        return;
    }

    // Find current's position in layout order for row scanning
    int currentLayoutPos = 0;
    for (int idx = 0; idx < n; idx++) {
        if (layoutOrder[idx] == current) { currentLayoutPos = idx; break; }
    }

    // Found a window on a different line. Find nearest position match
    // on that line (x-match for horizontal mode, y-match for vertical mode).
    int targetLineCoord = verticalLayout ? g_windows[current].rcCell.left : g_windows[current].rcCell.top;
    int bestIndex = current;
    int bestDist = INT_MAX;

    // Scan forward in layout order from current to find all windows on the target line.
    for (int idx = currentLayoutPos; idx < n; idx++) {
        int wi = layoutOrder[idx];
        if (IsWindowTruncated(wi)) break;
        int lineCoord = verticalLayout ? g_windows[wi].rcCell.left : g_windows[wi].rcCell.top;
        if (lineCoord != targetLineCoord) break;

        int perpCenter = verticalLayout ?
            (g_windows[wi].rcCell.top + g_windows[wi].rcCell.bottom) / 2 :
            (g_windows[wi].rcCell.left + g_windows[wi].rcCell.right) / 2;
        int dist = abs(prevPerpCenter - perpCenter);
        if (dist < bestDist) {
            bestDist = dist;
            bestIndex = wi;
        }
    }

    // Scan backward in layout order from current to cover the full line.
    for (int idx = currentLayoutPos - 1; idx >= 0; idx--) {
        int wi = layoutOrder[idx];
        if (IsWindowTruncated(wi)) break;
        int lineCoord = verticalLayout ? g_windows[wi].rcCell.left : g_windows[wi].rcCell.top;
        if (lineCoord != targetLineCoord) break;

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
    if (g_scrollTransition.active) {
        if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size()) {
            RectF r = ToRectF(g_windows[g_selectedIndex].rcCell);
            SnapSelectionTo(r);
        }
    } else {
        TriggerSelectionAnimation(prevSelected);
    }
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
        g_animChevronProgressPrev = 0.0f;
        g_animChevronAlphaTargetPrev = newTargetPrev;
    }
    if (newTargetNext != g_animChevronAlphaTargetNext) {
        g_animChevronStartAlphaNext = g_animChevronAlphaNext;
        g_animChevronProgressNext = 0.0f;
        g_animChevronAlphaTargetNext = newTargetNext;
    }

    if (g_scrollTransition.active) {
        g_animChevronDuration = g_scrollTransition.duration;
    } else {
        g_animChevronDuration = 0.200f;
    }

    if (immediate || !AreAnimationsGloballyEnabled()) {
        g_animChevronAlphaPrev = g_animChevronAlphaTargetPrev;
        g_animChevronAlphaNext = g_animChevronAlphaTargetNext;
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
                w.closeBtnAlpha = 0.0f;
            }
        }
        return;
    }

    int cDir = HitTestChevron(hWnd, x, y);
    bool chevronHoverChanged = (cDir != g_hoverChevron);
    g_hoverChevron = cDir;

    // Entry hover (for close button, selection, card hover)
    int entryIdx = (cDir != 0) ? -1 : HitTest(x, y);

    // Thumbnail hover (strictly when cursor is over thumbnail itself, or entire card for zoom)
    int thumbIdx = (cDir != 0 || !g_settings.showThumbnails) ? -1 : HitTestThumb(x, y);
    if (thumbIdx < 0 && entryIdx >= 0 && g_settings.showThumbnails && !DockLayoutActive() && ThumbnailHoverIsZoom()) {
        thumbIdx = entryIdx;
    }

    bool closeHovered = false;
    if (g_settings.showCloseButton && entryIdx >= 0 && entryIdx < (int)g_windows.size() && !IsWindowTruncated(entryIdx)) {
        POINT pt = { x, y };
        closeHovered = HitTestCloseButton(g_windows[entryIdx], pt);
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
        g_hoverIndex = entryIdx;
        g_hoverThumbIndex = thumbIdx;
        g_hoverWnd = hWnd;
        g_isCloseHovered = closeHovered;

        bool animsGloballyEnabled = AreAnimationsGloballyEnabled() && allowAnimation;
        if (animsGloballyEnabled) {
            if (thumbHoverChanged || rectChanged) {
                TriggerHoverAnimation(thumbIdx);
            }
            if (closeHoverChanged || entryHoverChanged) {
                StartAnimationTicker();
            }
        } else {
            g_animHoverActive = false;
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
                g_windows[i].closeBtnAlpha = (i == entryIdx && g_settings.showCloseButton && !IsWindowTruncated(i)) ? 1.0f : 0.0f;
                float s = (ThumbnailHoverIsZoom() && i == thumbIdx && !IsWindowTruncated(i)) ? (1.0f + SWS_HOVER_ZOOM_DELTA) : 1.0f;
                g_windows[i].hoverScale = s;
                g_windows[i].hoverScaleStart = s;
                g_windows[i].hoverScaleTarget = s;
                g_windows[i].hoverScaleProgress = 1.0f;
                g_windows[i].hoverScaleDuration = 0.150f;
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
            g_hoverIndex = -1;
            g_hoverThumbIndex = -1;
            g_hoverWnd = NULL;
            g_isCloseHovered = false;
            g_hoverChevron = 0;
            if (AreAnimationsGloballyEnabled() && allowAnimation) {
                TriggerHoverAnimation(-1);
                StartAnimationTicker();
            } else {
                g_animHoverActive = false;
                g_animHoverAlphaCurrent = 0.0f;
                g_animHoverAlphaTarget = 0.0f;
                g_animCloseBtnAlpha = 0.0f;
                g_animCloseBtnHoverAlpha = 0.0f;
                for (auto& w : g_windows) {
                    w.closeBtnAlpha = 0.0f;
                }
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
    GetWindowTextW(e.hWnd, e.title, 256);
    if (!e.title[0]) InternalGetWindowText(e.hWnd, e.title, 256);
    e.hIcon = LoadWindowIcon(e.hWnd);

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

static void RemoveWindowEntryByHwnd(HWND hDestroyed) {
    if (!g_isVisible || g_windows.empty()) return;

    if (g_drilledIn) {
        for (auto& saved : g_savedAppList) {
            auto& grp = saved.groupWindows;
            auto it = std::find(grp.begin(), grp.end(), hDestroyed);
            if (it != grp.end()) {
                grp.erase(it);
                break;
            }
        }
    }

    for (int i = 0; i < (int)g_windows.size(); i++) {
        if (g_windows[i].hWnd == hDestroyed) {
            if (g_settings.showApplications && g_windows[i].groupWindows.size() > 1) {
                auto& group = g_windows[i].groupWindows;
                group.erase(std::remove(group.begin(), group.end(), hDestroyed), group.end());
                if (!group.empty()) {
                    g_windows[i].hWnd = group[0];
                    UpdateEntryForWindow(g_windows[i]);
                    for (const auto& kv : g_windows[i].hThumbs) {
                        if (kv.second) DwmUnregisterThumbnail(kv.second);
                    }
                    g_windows[i].hThumbs.clear();
                    RecomputeAndReposition();
                    PaintSwitcher();
                    return;
                }
            }

            if (AreAnimationsGloballyEnabled() && g_settings.enableAnimations) {
                bool wasFocused = (i == g_selectedIndex);
                int closedIndex = i;
                int oldSize = (int)g_windows.size();

                DepartingEntrySnapshot snap = {};
                snap.hWnd = hDestroyed;
                snap.rcCellStart = g_windows[i].rcCell;
                snap.rcCellCurrent = g_windows[i].rcCell;
                snap.rcThumbStart = g_windows[i].rcThumbActual;
                snap.rcThumbCurrent = g_windows[i].rcThumbActual;
                snap.alpha = 1.0f;
                snap.scale = 1.0f;
                snap.hIcon = g_windows[i].hIcon;
                wcsncpy_s(snap.title, g_windows[i].title, _TRUNCATE);
                snap.groupWindows = g_windows[i].groupWindows;

                if (DockLayoutActive()) {
                    for (const auto& kv : g_windows[i].hThumbs) {
                        if (kv.second) DwmUnregisterThumbnail(kv.second);
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
                    g_dockPreviewSlide.progress = 0.0f;
                    g_dockPreviewSlide.duration = 0.220f;
                    g_dockPreviewSlide.travelDistance = (float)travelDist;
                    g_dockPreviewSlide.currentOffset = (float)travelDist;
                    g_dockPreviewSlide.currentAlpha = 0.0f;
                } else {
                    g_dockPreviewSlide.active = false;
                    g_dockPreviewSlide.progress = 1.0f;
                    g_dockPreviewSlide.currentOffset = 0.0f;
                    g_dockPreviewSlide.currentAlpha = 1.0f;
                }

                RECT curWnd = {};
                GetWindowRect(g_hSwitcher, &curWnd);
                g_layoutTransition.rcWndStart = ToRectF(curWnd);

                for (auto& w : g_windows) {
                    w.rcCellStart = w.rcCell;
                    w.rcThumbStart = w.rcThumbActual;
                    w.isNewEntry = false;
                }

                HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
                ComputeLayout(hMon);
                if (DockLayoutActive()) {
                    UpdateDockPreviewForSelection();
                }
                UpdateChevronLayout(g_hSwitcher);
                UpdateChevronAnimationTargets(false);

                MONITORINFO mi = { sizeof(mi) };
                GetMonitorInfoW(hMon, &mi);
                int cx, cy;
                GetSwitcherPosition(mi.rcWork, &cx, &cy);
                g_layoutTransition.rcWndTarget = { (float)cx, (float)cy, (float)(cx + g_winW), (float)(cy + g_winH) };

                for (auto& w : g_windows) {
                    w.rcCellTarget = w.rcCell;
                    w.rcThumbTarget = w.rcThumbActual;
                    if (DockLayoutActive()) {
                        if (&w == &g_windows[g_selectedIndex] && DockShowPreview()) {
                            w.rcThumbStart = g_rcCentralPreview;
                            w.rcThumbTarget = g_rcCentralPreview;
                            w.rcThumbActual = g_rcCentralPreview;
                        } else {
                            w.rcThumbStart = { 0, 0, 0, 0 };
                            w.rcThumbTarget = { 0, 0, 0, 0 };
                            w.rcThumbActual = { 0, 0, 0, 0 };
                        }
                    }

                    bool wasNotVisible = (w.rcCellStart.left == 0 && w.rcCellStart.right == 0 &&
                                          w.rcCellStart.top == 0 && w.rcCellStart.bottom == 0);
                    bool isNowVisible = (w.rcCellTarget.left != 0 || w.rcCellTarget.right != 0 ||
                                         w.rcCellTarget.top != 0 || w.rcCellTarget.bottom != 0);

                    if (w.isNewEntry || (wasNotVisible && isNowVisible)) {
                        // Newly revealed entry from overflow: blossom in place with smooth fade and subtle scale, never animating from (0,0)
                        w.isNewEntry = true;
                        w.enterAlpha = 0.0f;
                        w.enterScale = 0.94f;
                        w.rcCellStart = w.rcCellTarget;
                        w.rcThumbStart = w.rcThumbTarget;
                        w.rcCell = w.rcCellTarget;
                        w.rcThumbActual = w.rcThumbTarget;
                    } else if (!isNowVisible && !wasNotVisible) {
                        // Was visible, now pushed into overflow: retain position, don't lerp to (0,0)
                        w.rcCellTarget = w.rcCellStart;
                        w.rcThumbTarget = w.rcThumbStart;
                        w.rcCell = w.rcCellStart;
                        w.rcThumbActual = w.rcThumbStart;
                    } else if (wasNotVisible && !isNowVisible) {
                        // Remained overflown / truncated
                        w.rcCellStart = { 0, 0, 0, 0 };
                        w.rcThumbStart = { 0, 0, 0, 0 };
                        w.rcCellTarget = { 0, 0, 0, 0 };
                        w.rcThumbTarget = { 0, 0, 0, 0 };
                        w.rcCell = { 0, 0, 0, 0 };
                        w.rcThumbActual = { 0, 0, 0, 0 };
                    } else {
                        // Surviving visible entry: glides from old cell to new cell
                        w.rcCell = w.rcCellStart;
                        if (!DockLayoutActive()) {
                            w.rcThumbActual = w.rcThumbStart;
                        }
                    }
                }

                g_hoverIndex = -1;
                g_hoverThumbIndex = -1;
                g_hoverWnd = NULL;
                g_isCloseHovered = false;
                g_animHoverActive = false;
                g_animHoverAlphaCurrent = 0.0f;
                g_animHoverAlphaTarget = 0.0f;
                g_layoutTransition.progress = 0.0f;
                g_layoutTransition.duration = 0.280f;
                g_layoutTransition.active = true;
                StartAnimationTicker();
                return;
            } else {
                for (const auto& kv : g_windows[i].hThumbs) {
                    if (kv.second) DwmUnregisterThumbnail(kv.second);
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
    if ((!g_isVisible && !g_isPendingShow) || g_windows.empty()) return;
    if (!hWnd || !IsWindow(hWnd) || IsSwitcherWindow(hWnd)) return;

    // Check if hWnd already exists in g_windows
    for (size_t i = 0; i < g_windows.size(); i++) {
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
        for (const auto& w : g_savedAppList) {
            if (w.hWnd == hWnd) return;
            auto& grp = w.groupWindows;
            if (std::find(grp.begin(), grp.end(), hWnd) != grp.end()) return;
        }
    }

    WindowEntry e = {};
    if (!IsEligibleWindow(hWnd, &e)) return;

    // If hideMinimizedWindows is enabled and window is iconic, ignore
    if (g_settings.hideMinimizedWindows && IsIconic(hWnd)) return;

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
                    saved.groupWindows.push_back(hWnd);
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
    } else if (g_settings.showApplications) {
        WCHAR newKey[MAX_PATH] = {0};
        GetWindowGroupKey(hWnd, newKey, ARRAYSIZE(newKey));
        for (size_t i = 0; i < g_windows.size(); i++) {
            WCHAR existingKey[MAX_PATH] = {0};
            GetWindowGroupKey(g_windows[i].hWnd, existingKey, ARRAYSIZE(existingKey));
            if (newKey[0] && wcscmp(newKey, existingKey) == 0) {
                g_windows[i].groupWindows.push_back(hWnd);
                if (g_isVisible) PaintSwitcher();
                return;
            }
        }
        e.groupWindows.push_back(hWnd);
        UpdateEntryForWindow(e);

        auto insertPos = g_windows.end();
        if (g_settings.sortMinimizedWindowsToEnd && !IsIconic(hWnd)) {
            for (auto it = g_windows.begin(); it != g_windows.end(); ++it) {
                if (IsIconic(it->hWnd)) {
                    insertPos = it;
                    break;
                }
            }
        }
        g_windows.insert(insertPos, std::move(e));
    } else {
        UpdateEntryForWindow(e);

        auto insertPos = g_windows.end();
        if (g_settings.sortMinimizedWindowsToEnd && !IsIconic(hWnd)) {
            for (auto it = g_windows.begin(); it != g_windows.end(); ++it) {
                if (IsIconic(it->hWnd)) {
                    insertPos = it;
                    break;
                }
            }
        }
        g_windows.insert(insertPos, std::move(e));
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
            RECT curWnd = {};
            GetWindowRect(g_hSwitcher, &curWnd);
            g_layoutTransition.rcWndStart = ToRectF(curWnd);

            for (auto& w : g_windows) {
                w.rcCellStart = w.rcCell;
                w.rcThumbStart = w.rcThumbActual;
                w.isNewEntry = (w.hWnd == hWnd);
                if (w.isNewEntry) {
                    w.enterAlpha = 0.0f;
                    w.enterScale = 0.94f;
                } else {
                    w.enterAlpha = 1.0f;
                    w.enterScale = 1.0f;
                }
            }

            HMONITOR hMon = g_hCurrentMonitor ? g_hCurrentMonitor : MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
            ComputeLayout(hMon);
            UpdateChevronLayout(g_hSwitcher);
            UpdateChevronAnimationTargets(false);

            MONITORINFO mi = { sizeof(mi) };
            GetMonitorInfoW(hMon, &mi);
            int cx, cy;
            GetSwitcherPosition(mi.rcWork, &cx, &cy);
            g_layoutTransition.rcWndTarget = { (float)cx, (float)cy, (float)(cx + g_winW), (float)(cy + g_winH) };

            for (auto& w : g_windows) {
                w.rcCellTarget = w.rcCell;
                w.rcThumbTarget = w.rcThumbActual;
                if (DockLayoutActive()) {
                    if (&w == &g_windows[g_selectedIndex] && DockShowPreview()) {
                        w.rcThumbStart = g_rcCentralPreview;
                        w.rcThumbTarget = g_rcCentralPreview;
                        w.rcThumbActual = g_rcCentralPreview;
                    } else {
                        w.rcThumbStart = { 0, 0, 0, 0 };
                        w.rcThumbTarget = { 0, 0, 0, 0 };
                        w.rcThumbActual = { 0, 0, 0, 0 };
                    }
                }

                bool wasNotVisible = (w.rcCellStart.left == 0 && w.rcCellStart.right == 0 &&
                                      w.rcCellStart.top == 0 && w.rcCellStart.bottom == 0);
                bool isNowVisible = (w.rcCellTarget.left != 0 || w.rcCellTarget.right != 0 ||
                                     w.rcCellTarget.top != 0 || w.rcCellTarget.bottom != 0);

                if (w.isNewEntry || (wasNotVisible && isNowVisible)) {
                    // Newly added or newly revealed entry from overflow: blossom in place with smooth fade and subtle scale
                    w.isNewEntry = true;
                    w.enterAlpha = 0.0f;
                    w.enterScale = 0.94f;
                    w.rcCellStart = w.rcCellTarget;
                    w.rcThumbStart = w.rcThumbTarget;
                    w.rcCell = w.rcCellTarget;
                    w.rcThumbActual = w.rcThumbTarget;
                } else if (!isNowVisible && !wasNotVisible) {
                    // Was visible, now pushed into overflow: retain position, don't lerp to (0,0)
                    w.rcCellTarget = w.rcCellStart;
                    w.rcThumbTarget = w.rcThumbStart;
                    w.rcCell = w.rcCellStart;
                    w.rcThumbActual = w.rcThumbStart;
                } else if (wasNotVisible && !isNowVisible) {
                    // Remained overflown / truncated
                    w.rcCellStart = { 0, 0, 0, 0 };
                    w.rcThumbStart = { 0, 0, 0, 0 };
                    w.rcCellTarget = { 0, 0, 0, 0 };
                    w.rcThumbTarget = { 0, 0, 0, 0 };
                    w.rcCell = { 0, 0, 0, 0 };
                    w.rcThumbActual = { 0, 0, 0, 0 };
                } else {
                    // Surviving visible entry: glides from old cell to new cell
                    w.rcCell = w.rcCellStart;
                    if (!DockLayoutActive()) {
                        w.rcThumbActual = w.rcThumbStart;
                    }
                }
            }

            RegisterThumbnails();
            g_hoverIndex = -1;
            g_hoverThumbIndex = -1;
            g_hoverWnd = NULL;
            g_isCloseHovered = false;
            g_animHoverActive = false;
            g_animHoverAlphaCurrent = 0.0f;
            g_animHoverAlphaTarget = 0.0f;
            g_layoutTransition.progress = 0.0f;
            g_layoutTransition.duration = 0.280f;
            g_layoutTransition.active = true;
            StartAnimationTicker();
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

static void CALLBACK WinEventShowHideProc(HWINEVENTHOOK hHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (!hwnd || !IsWindow(hwnd)) return;
    if (!g_isVisible && !g_isPendingShow) return;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (style & WS_CHILD) return;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return;

    if (event == EVENT_OBJECT_SHOW) {
        AddWindowEntry(hwnd);
    } else if (event == EVENT_OBJECT_HIDE || event == EVENT_OBJECT_DESTROY) {
        if (!IsWindowVisible(hwnd) || event == EVENT_OBJECT_DESTROY) {
            if (!IsIconic(hwnd) || event == EVENT_OBJECT_DESTROY) {
                RemoveWindowEntryByHwnd(hwnd);
            }
        }
    }
}

// Close the window for the entry at idx (posts SC_CLOSE, same as the close
// button). Window removal initiates immediately with a smooth shallow fade trick,
// while also tracking via s_pendingCloseWindows and WinEventShowHideProc.
static void CloseSwitcherEntry(int idx) {
    if (idx < 0 || idx >= (int)g_windows.size()) return;
    
    HWND targetWnd = g_windows[idx].hWnd;
    if (!CanCloseWindow(targetWnd)) return;

    if (g_settings.showApplications && g_windows[idx].groupWindows.size() > 1) {
        if (wcscmp(g_settings.groupCloseBehavior, L"closeAll") == 0) {
            std::vector<HWND> toClose = g_windows[idx].groupWindows;
            for (HWND hw : toClose) {
                if (CanCloseWindow(hw)) {
                    PostMessage(hw, WM_SYSCOMMAND, SC_CLOSE, 0);
                    s_pendingCloseWindows.push_back(hw);
                }
            }
            RemoveWindowEntryByHwnd(targetWnd);
        } else {
            // closeRecent (Default)
            PostMessage(targetWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
            s_pendingCloseWindows.push_back(targetWnd);
            RemoveWindowEntryByHwnd(targetWnd);
        }
    } else {
        PostMessage(targetWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        s_pendingCloseWindows.push_back(targetWnd);
        RemoveWindowEntryByHwnd(targetWnd);
    }

    if (!s_pendingCloseWindows.empty() && g_hSwitcher) {
        s_pendingCloseRetries = 0;
        SetTimer(g_hSwitcher, SWS_CLOSE_VERIFY_TIMER_ID, 200, NULL);
    }
}

static LRESULT CALLBACK SwitcherWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_animExitActive) {
        if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP ||
            uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE ||
            uMsg == WM_HOTKEY || uMsg == WM_SWS_SCROLL) {
            return 0;
        }
    }

    if (uMsg == WM_NCCALCSIZE && wParam == TRUE) {
        return 0; // Remove standard frame for WS_OVERLAPPED
    }
    if (uMsg == WM_NCACTIVATE) {
        // Force DWM to keep the active visual state (Mica/Backdrop) even when unfocused
        return DefWindowProcW(hWnd, uMsg, TRUE, lParam);
    }

    if (uMsg == WM_TIMER) {
        if (wParam == SWS_HOTKEY_RETRY_TIMER_ID) {
            SWS_RegisterHotkeys();
            return 0;
        }

        if (wParam == SWS_SHOW_DELAY_TIMER_ID) {
            RevealPendingSwitcher();
            return 0;
        }

        if (wParam == SWS_ALT_POLL_TIMER_ID) {
            if (!g_isSticky && (GetAsyncKeyState(VK_MENU) & 0x8000) == 0) {
                KillTimer(hWnd, SWS_ALT_POLL_TIMER_ID);
                SwitchToSelected();
            }
            return 0;
        }

        if (wParam == SWS_CLOSE_VERIFY_TIMER_ID) {
            KillTimer(hWnd, SWS_CLOSE_VERIFY_TIMER_ID);
            std::vector<HWND> toRemove;
            for (auto it = s_pendingCloseWindows.begin(); it != s_pendingCloseWindows.end(); ) {
                HWND h = *it;
                if (!IsWindow(h) || !IsWindowVisible(h)) {
                    toRemove.push_back(h);
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
                if (++s_pendingCloseRetries < 15) { // Try up to ~3 seconds
                    SetTimer(hWnd, SWS_CLOSE_VERIFY_TIMER_ID, 200, NULL);
                } else {
                    s_pendingCloseWindows.clear();
                    s_pendingCloseRetries = 0;
                }
            } else {
                s_pendingCloseRetries = 0;
            }
            return 0;
        }

        if (wParam == SWS_ANIM_TIMER_ID) {
            OnAnimationTick();
            return 0;
        }
    }
    if (uMsg == WM_HOTKEY) {
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
            } else if (wcscmp(g_settings.altBacktickBehavior, L"backward") == 0 || UseAltBacktickBackward()) {
                isBackward = true;
            } else {
                return 0;
            }
            break;
        default:
            return 0;
        }

        if (!g_isVisible && !g_isPendingShow) {
            HWND hFg = GetForegroundWindow();
            if (hFg && !IsSwitcherWindow(hFg)) {
                UpdateMruWindow(hFg);
            }
            if (isAltBacktickTrigger) g_isAltBacktickSameApp = true;
            ShowSwitcher(isCtrl);

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
            if (g_isPendingShow && isCtrl) {
                g_isSticky = true;
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
        params.dwFlags = BPPF_ERASE;
        HDC hdcBuf = NULL;
        HPAINTBUFFER hBP = BeginBufferedPaint(hdc, &rc, BPBF_TOPDOWNDIB, &params, &hdcBuf);
        if (hBP) {
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
                    DrawSwitcherStaticContent(s_cachedStaticDC, false, hWnd);
                    g_staticContentDirty = false;
                }
                BitBlt(hdcBuf, 0, 0, w, h, s_cachedStaticDC, 0, 0, SRCCOPY);

                // Dynamic thumbnail drop shadow for any zoomed/animating thumbnails
                if (g_settings.showThumbnails && g_settings.showThumbnailShadow && ThumbnailHoverIsZoom()) {
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
                if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() && HighlightHasFill()) {
                    RectF fillRc = (g_animSelectionActive && AreAnimationsGloballyEnabled() && g_settings.enableSelectionAnimation)
                                   ? g_animSelectionCurrent
                                   : ToRectF(g_windows[g_selectedIndex].rcCell);
                    DrawSelectionFillF(hdcBuf, fillRc);
                    if (DockLayoutActive()) {
                        auto& e = g_windows[g_selectedIndex];
                        if (!IsWindowTruncated(g_selectedIndex) && e.drawnIconSz > 0) {
                            DrawIconEx(hdcBuf, e.drawnIconX, e.drawnIconY, e.hIcon, e.drawnIconSz, e.drawnIconSz, 0, NULL, DI_NORMAL);
                        }
                    }
                }
            } else {
                int radius = GetWindowCornerRadiusPx();
                if (DockLayoutActive()) {
                // 1. In Dock Layout, the switcher background, central preview shadow, divider line,
                // and window title are stationary and must remain 100% visible throughout the transition.
                HRGN hWndClip = GetCachedRoundRectRgn(w, h, radius);
                SelectClipRgn(hdcBuf, hWndClip);
                if (s_cachedScrollToDC) {
                    BitBlt(hdcBuf, 0, 0, w, h, s_cachedScrollToDC, 0, 0, SRCCOPY);
                }

                // 2. Clip strictly to the dock icon strip for sliding the icons
                HRGN hStripClip = CreateRectRgn(g_rcDockIconStrip.left, g_rcDockIconStrip.top, g_rcDockIconStrip.right, g_rcDockIconStrip.bottom);
                SelectClipRgn(hdcBuf, hStripClip);

                // Erase dock strip background with bg color so moving icons blend cleanly
                BYTE bgA = (BYTE)(g_settings.opacity * 255 / 100);
                if (bgA == 0) bgA = 1;
                COLORREF bgC = GetBgColor();
                BYTE bgR = GetRValue(bgC), bgG = GetGValue(bgC), bgB = GetBValue(bgC);
                RGBQUAD bgPx = { (BYTE)(bgB*bgA/255), (BYTE)(bgG*bgA/255), (BYTE)(bgR*bgA/255), bgA };
                BITMAPINFO bgBi = {}; bgBi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bgBi.bmiHeader.biWidth = 1; bgBi.bmiHeader.biHeight = 1;
                bgBi.bmiHeader.biPlanes = 1; bgBi.bmiHeader.biBitCount = 32; bgBi.bmiHeader.biCompression = BI_RGB;
                int stripW = g_rcDockIconStrip.right - g_rcDockIconStrip.left;
                int stripH = g_rcDockIconStrip.bottom - g_rcDockIconStrip.top;
                StretchDIBits(hdcBuf, g_rcDockIconStrip.left, g_rcDockIconStrip.top, stripW, stripH,
                              0, 0, 1, 1, &bgPx, &bgBi, DIB_RGB_COLORS, SRCCOPY);

                int offX = (int)roundf(g_scrollTransition.offsetCurrentX);
                int outOffX = offX - g_scrollTransition.travelDistanceX;

                if (s_cachedScrollFromDC) {
                    BitBlt(hdcBuf, outOffX, 0, w, h, s_cachedScrollFromDC, 0, 0, SRCCOPY);
                }
                if (s_cachedScrollToDC) {
                    BitBlt(hdcBuf, offX, 0, w, h, s_cachedScrollToDC, 0, 0, SRCCOPY);
                }

                if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() && HighlightHasFill()) {
                    RectF selRc = (g_animSelectionActive && AreAnimationsGloballyEnabled() && g_settings.enableSelectionAnimation)
                                  ? g_animSelectionCurrent
                                  : ToRectF(g_windows[g_selectedIndex].rcCell);
                    selRc.left += (float)offX;
                    selRc.right += (float)offX;
                    DrawSelectionFillF(hdcBuf, selRc);
                    auto& e = g_windows[g_selectedIndex];
                    if (!IsWindowTruncated(g_selectedIndex) && e.drawnIconSz > 0) {
                        DrawIconEx(hdcBuf, (int)roundf(e.drawnIconX + offX), (int)roundf(e.drawnIconY),
                                   e.hIcon, e.drawnIconSz, e.drawnIconSz, 0, NULL, DI_NORMAL);
                    }
                }

                SelectClipRgn(hdcBuf, NULL);
                DeleteObject(hStripClip);
            } else {
                int masterPadX = DpiScale(g_settings.switcherPadding, g_dpiX);
                int masterPadY = DpiScale(g_settings.switcherPadding, g_dpiY);
                HRGN hContentClip = CreateRectRgn(masterPadX, masterPadY, w - masterPadX, h - masterPadY);
                if (radius > 0) {
                    HRGN hWndClip = GetCachedRoundRectRgn(w, h, radius);
                    CombineRgn(hContentClip, hContentClip, hWndClip, RGN_AND);
                }
                SelectClipRgn(hdcBuf, hContentClip);

                int offX = (int)roundf(g_scrollTransition.offsetCurrentX);
                int offY = (int)roundf(g_scrollTransition.offsetCurrentY);
                int outOffX = offX - g_scrollTransition.travelDistanceX;
                int outOffY = offY - g_scrollTransition.travelDistanceY;

                if (s_cachedScrollFromDC) {
                    BitBlt(hdcBuf, outOffX, outOffY, w, h, s_cachedScrollFromDC, 0, 0, SRCCOPY);
                }
                if (s_cachedScrollToDC) {
                    BitBlt(hdcBuf, offX, offY, w, h, s_cachedScrollToDC, 0, 0, SRCCOPY);
                }

                if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_windows.size() && HighlightHasFill()) {
                    RectF selRc = (g_animSelectionActive && AreAnimationsGloballyEnabled() && g_settings.enableSelectionAnimation)
                                  ? g_animSelectionCurrent
                                  : ToRectF(g_windows[g_selectedIndex].rcCell);
                    selRc.left += (float)offX;
                    selRc.right += (float)offX;
                    selRc.top += (float)offY;
                    selRc.bottom += (float)offY;
                    DrawSelectionFillF(hdcBuf, selRc);
                }

                SelectClipRgn(hdcBuf, NULL);
                DeleteObject(hContentClip);
            }
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
            SwitchToSelected();
            return 0;
        }
        if (wParam == VK_ESCAPE && (g_isVisible || g_isPendingShow)) {
            if (g_consumeEscUp) { g_consumeEscUp = false; return 0; }
            StartExitAnimation(false);
            return 0;
        }
        if (wParam == VK_RETURN && g_isVisible) { SwitchToSelected(); return 0; }
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
            SwitchToSelected();
            return 0;
        }
        break;
    case WM_SYSKEYDOWN: case WM_KEYDOWN:
        if (wParam == VK_F4) return 0;
        if (wParam == VK_ESCAPE && g_isPendingShow) {
            StartExitAnimation(false);
            return 0;
        }
        if (g_isVisible) {
            // Ctrl tap: drill into / out of the selected application's windows.
            if ((wParam == VK_CONTROL || wParam == VK_LCONTROL || wParam == VK_RCONTROL)
                && g_settings.showApplications) {
                bool isRepeat = (lParam & 0x40000000) != 0;
                if (!isRepeat) ToggleAppDrill();
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
                (wcscmp(g_settings.altBacktickBehavior, L"backward") == 0 || UseAltBacktickBackward())) {
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
                CycleLinear(dir);
            } else if (action == 2) { // page
                CyclePage(dir);
            }
        }
        return 0;
    case WM_SWS_SETTINGS_CHANGED:
        if (g_isVisible || g_isPendingShow) HideSwitcher();
        FreeCachedBuffers();
        g_staticContentDirty = true;
        SWS_UnregisterHotkeys();
        LoadSettings();
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
        UpdateHoverAtPoint(hWnd, x, y, true);
        return 0;
    }
    case WM_MOUSELEAVE: {
        if (g_pressedChevron != 0) {
            g_pressedChevron = 0;
            PaintSwitcherOverlay();
        }
        if (g_hoverChevron != 0) {
            g_hoverChevron = 0;
            PaintSwitcherOverlay();
        }
        if (g_hoverWnd == hWnd) {
            g_hoverIndex = -1;
            g_hoverThumbIndex = -1;
            g_hoverWnd = NULL;
            g_isCloseHovered = false;
            TriggerHoverAnimation(-1);
            PaintSwitcher();
        }
        return 0;
    }
    case WM_SETTINGCHANGE:
        RefreshClientAreaAnimCache();
        if (!AreAnimationsGloballyEnabled()) {
            FinishAnimations();
            StopAnimationTicker();
            PaintSwitcher();
        }
        return 0;
    case WM_LBUTTONDOWN: {
        if (!g_isVisible) return 0;
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        int cDir = HitTestChevron(hWnd, x, y);
        if (cDir != 0) {
            g_pressedChevron = cDir;
            SetCapture(hWnd);
            PaintSwitcherOverlay();
            return 0;
        }
        if (g_isCloseHovered && g_hoverIndex >= 0) {
            g_isClosePressed = true;
            SetCapture(hWnd);
            PaintSwitcherOverlay();
            return 0;
        }
        return 0;
    }
    case WM_CAPTURECHANGED: {
        if (g_pressedChevron != 0) {
            g_pressedChevron = 0;
            PaintSwitcherOverlay();
        }
        if (g_isClosePressed) {
            g_isClosePressed = false;
            PaintSwitcherOverlay();
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        if (!g_isVisible) return 0;
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        if (GetCapture() == hWnd) {
            ReleaseCapture();
        }
        if (g_isClosePressed) {
            g_isClosePressed = false;
            PaintSwitcherOverlay();
            if (g_isCloseHovered && g_hoverIndex >= 0) {
                CloseSwitcherEntry(g_hoverIndex);
                return 0;
            }
        }
        if (g_pressedChevron != 0) {
            int wasPressed = g_pressedChevron;
            g_pressedChevron = 0;
            PaintSwitcherOverlay();
            int cDir = HitTestChevron(hWnd, x, y);
            if (cDir == wasPressed) {
                CyclePage(cDir);
                return 0;
            }
        }
        int cDir = HitTestChevron(hWnd, x, y);
        if (cDir != 0) {
            CyclePage(cDir);
            return 0;
        }
        int idx = HitTest(x, y);
        if (idx < 0 && DockLayoutActive() && DockShowPreview()) {
            POINT pt = { x, y };
            if (PtInRect(&g_rcCentralPreview, pt)) {
                idx = g_selectedIndex;
            }
        }
        if (idx >= 0) {
            if (g_isCloseHovered && idx == g_hoverIndex) {
                CloseSwitcherEntry(idx);
            } else {
                g_selectedIndex = idx;
                SwitchToSelected();
            }
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
        if (wParam == WA_INACTIVE && (g_isVisible || g_isPendingShow)) {
            if (g_animExitActive) return 0;
            HWND hNewActive = (HWND)lParam;
            if (!IsSwitcherWindow(hNewActive)) {
                HideSwitcher();
            }
            return 0;
        }
        break;
    case WM_KILLFOCUS:
        if (g_isVisible || g_isPendingShow) {
            if (g_animExitActive) return 0;
            HWND hNewFocus = (HWND)wParam;
            if (!IsSwitcherWindow(hNewFocus)) {
                HideSwitcher();
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
        FinishAnimations();
        StopAnimationTicker();
        FreeCachedBuffers();
        if (s_hWinEventHook) {
            UnhookWinEvent(s_hWinEventHook);
            s_hWinEventHook = NULL;
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
            RemoveMruWindow(hS);
            auto it = std::find(s_pendingCloseWindows.begin(), s_pendingCloseWindows.end(), hS);
            if (it != s_pendingCloseWindows.end()) {
                s_pendingCloseWindows.erase(it);
            }
            if (g_isVisible) {
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
static bool g_altBacktickUsRegistered = false;
static bool g_altBacktickUkRegistered = false;

static void SWS_RegisterHotkeys() {
    if (g_hotkeysRegistered || !g_hSwitcher) return;
    bool wantAltBacktick = (wcscmp(g_settings.altBacktickBehavior, L"none") != 0) || BackwardShortcutIs(L"altBacktick");
    BOOL r1 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTTAB, MOD_ALT, VK_TAB);
    BOOL r2 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTTAB, MOD_ALT | MOD_SHIFT, VK_TAB);
    BOOL r3 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTCTRLTAB, MOD_ALT | MOD_CONTROL, VK_TAB);
    BOOL r4 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTCTRLTAB, MOD_ALT | MOD_SHIFT | MOD_CONTROL, VK_TAB);
    BOOL r5_us = wantAltBacktick ? RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK, MOD_ALT, VK_OEM_3) : TRUE;
    BOOL r5_uk = wantAltBacktick ? RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK_UK, MOD_ALT, VK_OEM_8) : TRUE;
    g_altBacktickUsRegistered = wantAltBacktick && (r5_us != FALSE);
    g_altBacktickUkRegistered = wantAltBacktick && (r5_uk != FALSE);
    BOOL r5 = wantAltBacktick ? (r5_us || r5_uk) : TRUE;
    BOOL r6 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_WINALTTAB, MOD_ALT | MOD_WIN, VK_TAB);
    BOOL r7 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_WINALTSHIFTTAB, MOD_ALT | MOD_SHIFT | MOD_WIN, VK_TAB);
    if (r1 && r2 && r3 && r4 && r5 && r6 && r7) {
        g_hotkeysRegistered = true;
        if (!g_hHotkeyMutex) {
            g_hHotkeyMutex = CreateMutexW(NULL, TRUE, L"Windhawk_SWS_HotkeyMutex");
        }
        KillTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID);
        Wh_Log(L"All hotkeys registered successfully (Alt+Backtick US: %d, UK: %d)", r5_us, r5_uk);
    } else {
        if (r1) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTTAB);
        if (r2) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTTAB);
        if (r3) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTCTRLTAB);
        if (r4) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTCTRLTAB);
        if (g_altBacktickUsRegistered) {
            UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK);
            g_altBacktickUsRegistered = false;
        }
        if (g_altBacktickUkRegistered) {
            UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK_UK);
            g_altBacktickUkRegistered = false;
        }
        if (r6) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_WINALTTAB);
        if (r7) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_WINALTSHIFTTAB);
        SetTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID, SWS_HOTKEY_RETRY_INTERVAL, NULL);
        Wh_Log(L"Hotkey registration incomplete, retrying in %dms", SWS_HOTKEY_RETRY_INTERVAL);
    }
}
static void SWS_UnregisterHotkeys() {
    KillTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID);
    if (!g_hotkeysRegistered || !g_hSwitcher) return;
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTTAB);
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTTAB);
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTCTRLTAB);
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTCTRLTAB);
    if (g_altBacktickUsRegistered) {
        UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK);
        g_altBacktickUsRegistered = false;
    }
    if (g_altBacktickUkRegistered) {
        UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK_UK);
        g_altBacktickUkRegistered = false;
    }
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_WINALTTAB);
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_WINALTSHIFTTAB);
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
    LoadStringSetting(L"Style.colorScheme", g_settings.colorScheme, L"system");
    LoadStringSetting(L"Appearance.Corners.cornerPreference", g_settings.cornerPreference, L"auto");
    if (wcscmp(g_settings.cornerPreference, L"auto") == 0) {
        wcsncpy_s(g_settings.cornerPreference, IsWin11OrGreater() ? L"default" : L"none", _TRUNCATE);
    }
    g_settings.customCornerRadius = Wh_GetIntSetting(L"Appearance.Corners.customCornerRadius");
    if (g_settings.customCornerRadius < 0) g_settings.customCornerRadius = 0;
    if (g_settings.customCornerRadius > 32) g_settings.customCornerRadius = 32;
    g_settings.taskRoundedCorners = LoadAutoBoolSetting(L"Appearance.Corners.taskRoundedCorners", IsWin11OrGreater());
    g_settings.roundThumbnailCorners = LoadAutoBoolSetting(L"Appearance.Corners.roundThumbnailCorners", IsWin11OrGreater());
    g_settings.roundGroupIndicator = LoadAutoBoolSetting(L"Appearance.Corners.roundGroupIndicator", IsWin11OrGreater());
    g_settings.roundBadgeIconBackground = LoadAutoBoolSetting(L"Appearance.Corners.roundBadgeIconBackground", IsWin11OrGreater());
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
    if (wcscmp(g_settings.backwardShortcut, L"altShiftTab") != 0 &&
        wcscmp(g_settings.backwardShortcut, L"altShift") != 0 &&
        wcscmp(g_settings.backwardShortcut, L"altBacktick") != 0) {
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
    g_settings.switcherPadding = LoadIntSetting(L"Dimensions.switcherPadding", 20);
    if (g_settings.switcherPadding < 0) g_settings.switcherPadding = 20;
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
    // Backwards-compatibility migration for pre-master-dropdown configurations
    if (wcscmp(g_settings.switcherLayout, L"default") == 0) {
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
    g_settings.showBadgeIconBackground = Wh_GetIntSetting(L"Appearance.BadgeLayout.showBadgeIconBackground");
    g_settings.showBadgeIconBackgroundShadow = Wh_GetIntSetting(L"Appearance.BadgeLayout.showBadgeIconBackgroundShadow");
    g_settings.badgeIconPadding = Wh_GetIntSetting(L"Appearance.BadgeLayout.badgeIconPadding");
    g_settings.badgeIconOffsetX = Wh_GetIntSetting(L"Appearance.BadgeLayout.badgeIconOffsetX");
    g_settings.badgeIconOffsetY = Wh_GetIntSetting(L"Appearance.BadgeLayout.badgeIconOffsetY");

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

    // Grouped indicator
    g_settings.showGroupIndicator = Wh_GetIntSetting(L"Grouping.showGroupIndicator");
    g_settings.showGroupIndicatorShadow = Wh_GetIntSetting(L"Grouping.showGroupIndicatorShadow");
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

    LoadStringSetting(L"Style.DarkMode.indicatorBgColorMode", g_settings.indicatorBgColorModeDark, L"default");
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

    LoadStringSetting(L"Style.LightMode.indicatorBgColorMode", g_settings.indicatorBgColorModeLight, L"default");
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
    InvalidateStaticCache();
}


// RegisterHotKey hook for explorer.exe

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

// Background thread for tool mod process

static DWORD WINAPI SwitcherThread(LPVOID lpParam) {
    Wh_Log(L"SwitcherThread starting");
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    g_easeEntrance.Init(0.1f, 0.9f, 0.2f, 1.0f);
    g_easeSlide.Init(0.1f, 0.9f, 0.2f, 1.0f);
    g_easeHover.Init(0.1f, 0.9f, 0.2f, 1.0f);
    g_easeHoverEnter.Init(0.0f, 0.0f, 0.2f, 1.0f);
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
    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED;
    g_hSwitcher = CreateWindowExW(exStyle, SWS_CLASSNAME, L"",
        dwStyle, 0, 0, 0, 0, NULL, NULL, GetModuleHandleW(NULL), NULL);
    if (!g_hSwitcher) { Wh_Log(L"Failed to create switcher window"); return 1; }

    g_hCloseBtnWnd = CreateWindowExW(
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

    Wh_Log(L"Simple Window Switcher initialized, entering message loop");

    MSG msg;
    while (true) {
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto thread_exit;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (g_animActive) {
            LARGE_INTEGER frameStart, frameEnd;
            QueryPerformanceCounter(&frameStart);

            OnAnimationTick();

            if (g_animActive) {
                QueryPerformanceCounter(&frameEnd);
                double spentMs = (double)(frameEnd.QuadPart - frameStart.QuadPart) * 1000.0 / (double)g_animPerfFreq.QuadPart;
                double waitMs = s_animTargetIntervalMs - spentMs;
                if (waitMs > 1.0) {
                    MsgWaitForMultipleObjectsEx(
                        0, NULL,
                        (DWORD)floor(waitMs),
                        QS_ALLINPUT,
                        MWMO_ALERTABLE | MWMO_INPUTAVAILABLE
                    );
                } else if (waitMs > 0.0) {
                    SwitchToThread();
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
    SWS_UnregisterHotkeys();
    if (g_isVisible || g_isPendingShow) HideSwitcher();
    UnregisterThumbnails();
    g_windows.clear();
    g_mruWindows.clear();
    if (g_hCloseBtnWnd) { DestroyWindow(g_hCloseBtnWnd); g_hCloseBtnWnd = NULL; }
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
    CoUninitialize();
    Wh_Log(L"SwitcherThread exiting");
    return 0;
}

// Tool Mod callbacks

BOOL WhTool_ModInit() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModInit");
    g_hSwitcherThread = CreateThread(NULL, 0, SwitcherThread, NULL, 0, &g_dwSwitcherThreadId);
    return g_hSwitcherThread != NULL;
}

void WhTool_ModUninit() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModUninit");
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
    for (auto& pair : g_uwpIconCache) {
        if (pair.second) DestroyIcon(pair.second);
    }
    g_uwpIconCache.clear();
    for (auto& pair : g_exeIconCache) {
        if (pair.second) DestroyIcon(pair.second);
    }
    g_exeIconCache.clear();
    for (auto& pair : g_customIconCache) {
        if (pair.second) DestroyIcon(pair.second);
    }
    g_customIconCache.clear();
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModSettingsChanged");
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

    // --- explorer.exe path: hook RegisterHotKey only ---
    if (_wcsicmp(exeName, L"explorer.exe") == 0) {
        g_isExplorer = true;
        Wh_Log(L"SWS: Loaded into explorer.exe, hooking RegisterHotKey");

        if (!g_WM_SWS_GET_UWP_ICON) {
            g_WM_SWS_GET_UWP_ICON = RegisterWindowMessageW(L"Windhawk_SWS_GetUwpIcon");
        }
        g_explorerIpcThread = CreateThread(NULL, 0, ExplorerIpcThread, NULL, 0, &g_explorerIpcThreadId);

        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            void* pRegisterHotKey = (void*)GetProcAddress(hUser32, "RegisterHotKey");
            if (pRegisterHotKey) {
                Wh_SetFunctionHook(pRegisterHotKey, (void*)RegisterHotKey_Hook, (void**)&RegisterHotKey_Original);
            }
        }

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

void Wh_ModUninit() {
    if (g_isExplorer) {
        HWND promptWnd = g_restartExplorerPromptWindow;
        if (promptWnd) PostMessage(promptWnd, WM_CLOSE, 0, 0);

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
        for (auto& pair : g_uwpIconCache) {
            if (pair.second) DestroyIcon(pair.second);
        }
        g_uwpIconCache.clear();
        for (auto& pair : g_exeIconCache) {
            if (pair.second) DestroyIcon(pair.second);
        }
        g_exeIconCache.clear();
        for (auto& pair : g_customIconCache) {
            if (pair.second) DestroyIcon(pair.second);
        }
        g_customIconCache.clear();

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
