// ==WindhawkMod==
// @id              simple-window-switcher
// @name            Simple Window Switcher
// @description     Replaces the default Alt+Tab with a lightweight window switcher inspired by ExplorerPatcher's Simple Window Switcher
// @version         2.0
// @author          Lone
// @github          https://github.com/Louis047
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -ldwmapi -luxtheme -lgdi32 -lshlwapi -loleaut32 -lole32 -lcomctl32 -lgdiplus -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Simple Window Switcher
A lightweight Alt+Tab replacement for Windows, ported from the [Simple Window Switcher](https://github.com/valinet/sws) project.
Additional improvements made by [Asteski](https://github.com/Asteski).

## Features
- Grid layout with live DWM thumbnail previews
- Different Task List, Header Content and Thumbnails layouts
- Center align task list content and titles (horizontal and vertical options)
- Keyboard navigation (Tab/Shift+Tab/Shift/Backtick, Arrow keys, Enter, Esc)
- Mouse click to select, scroll wheel to cycle from anywhere
- Virtual Desktop Support (Show windows across multiple virtual desktops)
- Group windows by application (macOS Cmd+Tab style, one entry per app)
- Drill into an app's windows with a Ctrl tap to pick a specific window (Esc backs out)
- Win+Alt+Tab override to display windows from all monitors when using Per-Monitor mode
- Alt+Ctrl+Tab sticky mode
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
    - theme: none
      $name: Style
      $description: Visual theme style for the switcher background.
      $options:
      - none: None (Transparent)
      - backdrop: Acrylic (Windows 10+)
      - mica: Mica Blur (Windows 11 only)
    - colorScheme: system
      $name: Color Scheme
      $options:
      - system: Follow system setting
      - light: Light
      - dark: Dark
    - highlightStyle: border
      $name: Task Highlight Style
      $description: Style used for the selected task row/tile. Applies to both light and dark themes.
      $options:
      - border: Border only
      - fillAndBorder: Background fill and border
      - fillOnly: Background fill only
    - opacity: 100
      $name: Background Opacity
      $description: Background opacity percentage (0-100), applies to None and Acrylic themes for both light and dark themes. Set value to '80' for Acrylic to see the effect.
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
    - Corners:
        - cornerPreference: round
          $name: Corner Preference
          $description: Corner radius for the switcher window and its elements.
          $options:
          - none: Squared
          - round: Rounded
          - roundSmall: Rounded small
          - custom: Custom
        - customCornerRadius: 8
          $name: Custom Corner Radius (px)
          $description: Corner radius in pixels, used when Corner Preference is set to Custom. Applies to task borders, close buttons, and thumbnails.
        - taskRoundedCorners: false
          $name: Round Task Borders and Close Button
          $description: Apply rounded corners to the selected task border and close button.
        - roundThumbnailCorners: false
          $name: Round Thumbnail Corners
          $description: Round the corners of window thumbnails. Uses the radius from Corner Preference.
        - roundGroupIndicator: false
          $name: Round Group Indicator
          $description: Round the corners of the group indicator. Uses the radius from Corner Preference.
        - roundBadgeIconBackground: false
          $name: Round Icon Background (only for badge-layout)
          $description: Round the corners of the badge icon background pill. Uses the radius from Corner Preference.
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
        - showHoverBorder: true
          $name: Show Hover Border
          $description: Show a colored border around the thumbnail when hovered.
        - showThumbnailShadow: false
          $name: Show Thumbnail Shadow
          $description: Draw a soft drop shadow behind each window thumbnail.
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
        - enableBadgeLayout: false
          $name: Enable Badge Layout (macOS-style)
          $description: Overlay the icon on top of the thumbnail and place the title outside. Overrides normal header positioning when enabled. Requires Show Thumbnails to be on.
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
      $name: Badge Layout
    - Font:
        - fontFamily: Segoe UI
          $name: Font Family
          $description: Font used for window titles (e.g., Segoe UI, Tahoma).
        - fontSize: 9
          $name: Font Size
          $description: Size of the font in points.
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
      $name: Scroll Wheel to Change Selection
      $options:
      - never: Never
      - always: Always
      - stickyOnly: Only in sticky mode
    - reverseScrollDirection: false
      $name: Reverse Scroll Direction
    - backwardShortcut: altShiftTab
      $name: Backward Shortcut
      $description: Shortcut used to move backward in the switcher.
      $options:
      - altShiftTab: Alt+Shift+Tab (default)
      - altShift: Alt+Shift
      - altBacktick: Alt+Backtick
    - switcherDisplayBehavior: cursorMonitor
      $name: Switcher Display Behavior
      $options:
      - primaryOnly: Primary Monitor Only
      - allMonitors: All Monitors
      - cursorMonitor: Monitor Based on Cursor Location
    - perMonitorWindows: false
      $name: Display Windows Only from the Monitor Containing the Cursor
    - virtualDesktopBehavior: currentOnly
      $name: Virtual Desktop Behavior
      $description: Choose which virtual desktops to show windows from.
      $options:
      - currentOnly: Show windows from current virtual desktop only
      - allDesktops: Show windows from all virtual desktops
    - hideMinimizedWindows: false
      $name: Hide Minimized Windows
      $description: Hide minimized windows from the switcher. When "Group Windows by Application" is enabled, an application is only hidden if all of its windows are minimized.
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
      $description: "Custom name to display for matching tasks, replacing the detected application name. Leave empty to keep the default. Shown in the 'App name' and 'App name + Window title' title modes."
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
// Posted by the low-level mouse hook so the heavy CycleLinear work runs in the
// wndproc instead of on the synchronous raw-input path. WPARAM is the direction.
#define WM_SWS_SCROLL           (WM_APP + 1)

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
};
struct Settings {
    WCHAR theme[32]; WCHAR colorScheme[32]; WCHAR cornerPreference[32]; WCHAR scrollWheelBehavior[32]; WCHAR taskListOrientation[32]; WCHAR headerContentOrientation[32]; WCHAR iconSize[32]; WCHAR backwardShortcut[32]; WCHAR thumbnailPosition[32]; WCHAR thumbnailAlignment[32]; WCHAR switcherDisplayBehavior[32];
    WCHAR virtualDesktopBehavior[32];
    // Global theme settings (apply to both light and dark)
    WCHAR highlightStyle[32]; int opacity;
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
    bool showTitle;
    bool showIcon;
    int maxWidthPercent;
    bool autoFitTasks;
    int maxHeightPercent; int showDelay;
    bool perMonitorWindows; bool taskRoundedCorners; bool roundThumbnailCorners; bool roundGroupIndicator; bool roundBadgeIconBackground; bool reverseScrollDirection;
    bool centerTaskContent;
    bool showApplications;
    WCHAR showTitles[32];
    bool restoreAllWindows;
    bool hideMinimizedWindows;
    int customCornerRadius;
    WCHAR switcherPosition[32];
    int switcherPositionMargin;
    bool showHoverBorder;
    bool showThumbnailShadow;
    // Badge layout (macOS-style)
    bool enableBadgeLayout;
    WCHAR badgeIconPosition[32];
    WCHAR badgeTitlePosition[16];
    bool showBadgeIconBackground; bool showBadgeIconBackgroundShadow;
    int badgeIconPadding;
    int badgeIconOffsetX;
    int badgeIconOffsetY;
    // Grouped indicator
    bool showGroupIndicator;
    bool showGroupIndicatorShadow;
    WCHAR groupCloseBehavior[16];
};

static std::vector<std::wstring> g_excludeTitlePatterns;
static std::vector<std::wstring> g_excludeExePatterns;
static std::vector<HWND> g_hMirrorSwitchers;

static HWND g_hSwitcher = NULL;
static HWND g_hCloseBtnWnd = NULL;
static IVirtualDesktopManager* g_pVirtualDesktopManager = NULL;
static bool g_showAllMonitors = false;
static HHOOK g_hMouseHook = NULL;
static std::vector<WindowEntry> g_windows;
static int g_selectedIndex = 0, g_hoverIndex = -1;
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
static HANDLE g_explorerIpcThread = NULL;
static bool g_isPendingShow = false;
static RECT g_pendingSwitcherRect = {0, 0, 0, 0};

// Helpers

static bool ThemeIs(const WCHAR* v) { return wcscmp(g_settings.theme, v) == 0; }
static bool ScrollIs(const WCHAR* v) { return wcscmp(g_settings.scrollWheelBehavior, v) == 0; }
static bool LayoutIsVertical() { return wcscmp(g_settings.taskListOrientation, L"vertical") == 0; }
static bool HeaderOrientationIs(const WCHAR* v) { return wcscmp(g_settings.headerContentOrientation, v) == 0; }
static bool IconSizeIs(const WCHAR* v) { return wcscmp(g_settings.iconSize, v) == 0; }
static bool BackwardShortcutIs(const WCHAR* v) { return wcscmp(g_settings.backwardShortcut, v) == 0; }
static bool ThumbnailPositionIs(const WCHAR* v) { return wcscmp(g_settings.thumbnailPosition, v) == 0; }
static bool HighlightStyleIs(const WCHAR* v) {
    return wcscmp(g_settings.highlightStyle, v) == 0;
}
static bool UseAltShiftTabBackward() { return BackwardShortcutIs(L"altShiftTab"); }
static bool UseAltShiftBackward() { return BackwardShortcutIs(L"altShift"); }
static bool UseAltBacktickBackward() { return BackwardShortcutIs(L"altBacktick"); }
static bool ThumbnailIsBottom() { return ThumbnailPositionIs(L"bottom"); }
static bool ThumbnailIsTop() { return ThumbnailPositionIs(L"top"); }
static bool ThumbnailIsLeft() { return ThumbnailPositionIs(L"left"); }
static bool ThumbnailIsRight() { return ThumbnailPositionIs(L"right"); }
static bool ThumbnailIsSide() { return ThumbnailIsLeft() || ThumbnailIsRight(); }
static bool ThumbnailAlignmentIs(const WCHAR* v) { return wcscmp(g_settings.thumbnailAlignment, v) == 0; }
static bool ThumbnailAlignCentered() { return ThumbnailAlignmentIs(L"centered"); }
static bool ThumbnailAlignRight() { return ThumbnailAlignmentIs(L"right"); }
static bool HighlightHasFill() {
    return HighlightStyleIs(L"fillAndBorder") || HighlightStyleIs(L"fillOnly");
}
static bool HighlightHasBorder() {
    return HighlightStyleIs(L"border") || HighlightStyleIs(L"fillAndBorder");
}
static bool StretchThumbsToTaskWidth() {
    return g_settings.stretchThumbnailsToTaskWidth;
}
static bool HeaderIsVertical() {
    return HeaderOrientationIs(L"vertical");
}
static bool BadgeLayoutActive() {
    return g_settings.enableBadgeLayout && g_settings.showThumbnails;
}
static bool BadgeIconPositionIs(const WCHAR* v) { return wcscmp(g_settings.badgeIconPosition, v) == 0; }
static bool BadgeTitleIsTop() { return wcscmp(g_settings.badgeTitlePosition, L"top") == 0; }
static int GetHeaderIconSizeBase() {
    if (IconSizeIs(L"xlarge")) return 64;
    if (IconSizeIs(L"large")) return 48;
    if (IconSizeIs(L"medium")) return 32;
    return SWS_ICON_SIZE;
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
    if (wcscmp(g_settings.cornerPreference, L"none") == 0) return 1;
    if (wcscmp(g_settings.cornerPreference, L"roundSmall") == 0) return 3;
    if (wcscmp(g_settings.cornerPreference, L"custom") == 0) return 1; // Don't let DWM round, we'll try to manual mask
    return 2; // Default to round
}

static bool UseTaskRoundedCorners() {
    return g_settings.taskRoundedCorners;
}

static int GetTaskUiCornerRadiusPx() {
    if (!UseTaskRoundedCorners()) {
        return 0;
    }
    if (wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        return MulDiv(g_settings.customCornerRadius, g_dpiX, 96);
    }
    return MulDiv(4, g_dpiX, 96);
}

// Thumbnail corner rounding is controlled independently from the task border /
// close button rounding, but shares the same radius from Corner Preference.
static int GetThumbnailCornerRadiusPx() {
    if (!g_settings.roundThumbnailCorners) {
        return 0;
    }
    if (wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        return MulDiv(g_settings.customCornerRadius, g_dpiX, 96);
    }
    return MulDiv(4, g_dpiX, 96);
}

static int GetGroupIndicatorCornerRadiusPx(int maxRadius) {
    if (!g_settings.roundGroupIndicator) {
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

    DWORD ex = (DWORD)GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    // WS_EX_TOOLWINDOW always excludes from alt-tab, regardless of other flags
    if (ex & WS_EX_TOOLWINDOW) return false;

    // WS_EX_NOACTIVATE excludes unless WS_EX_APPWINDOW is also set
    if ((ex & WS_EX_NOACTIVATE) && !(ex & WS_EX_APPWINDOW)) return false;

    // Owner chain: if window has a visible, enabled owner, exclude it
    // unless WS_EX_APPWINDOW forces inclusion
    HWND own = GetWindow(hwnd, GW_OWNER);
    bool ownVis = IsWindow(own) && IsWindowEnabled(own) && IsReallyVisible(own);
    if (ownVis && !(ex & WS_EX_APPWINDOW)) return false;

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

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<WindowEntry>*>(lParam);
    if (hWnd == g_hSwitcher) return TRUE;
    if (!IsAltTabWindow(hWnd)) return TRUE;
    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) {
        if (wcscmp(g_settings.virtualDesktopBehavior, L"allDesktops") == 0 && g_pVirtualDesktopManager) {
            BOOL onCurrent = FALSE;
            if (SUCCEEDED(g_pVirtualDesktopManager->IsWindowOnCurrentVirtualDesktop(hWnd, &onCurrent)) && !onCurrent) {
                // allow cloaked window since it's just on another virtual desktop
            } else return TRUE;
        } else return TRUE;
    }
    bool isPrimaryOnly = (wcscmp(g_settings.switcherDisplayBehavior, L"primaryOnly") == 0);
    if (g_settings.perMonitorWindows && !g_showAllMonitors && g_hCurrentMonitor && !isPrimaryOnly) {
        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL) != g_hCurrentMonitor) return TRUE;
    }
    WindowEntry e = {};
    e.hWnd = hWnd;
    InternalGetWindowText(hWnd, e.title, 256);
    if (!e.title[0]) GetWindowTextW(hWnd, e.title, 256);
    
    bool excluded = false;
    for (const auto& pat : g_excludeTitlePatterns) {
        if (PathMatchSpecW(e.title, pat.c_str())) { excluded = true; break; }
    }
    if (!excluded && !g_excludeExePatterns.empty()) {
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
                        if (PathMatchSpecW(filename, pat.c_str())) { excluded = true; break; }
                    }
                }
                CloseHandle(hProc);
            }
        }
    }
    if (excluded) return TRUE;

    e.hIcon = LoadWindowIcon(hWnd);
    list->push_back(e);
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
                HICON hTmp = (HICON)SendMessageW(hCore, WM_GETICON, ICON_BIG, 0);
                if (!hTmp) hTmp = (HICON)SendMessageW(hCore, WM_GETICON, ICON_SMALL, 0);
                if (!hTmp) hTmp = (HICON)SendMessageW(hCore, WM_GETICON, ICON_SMALL2, 0);
                if (hTmp) hWinIcon = hTmp;
                if (!hWinIcon) {
                    HICON hClassIcon = (HICON)GetClassLongPtrW(hCore, GCLP_HICON);
                    if (!hClassIcon) hClassIcon = (HICON)GetClassLongPtrW(hCore, GCLP_HICONSM);
                    if (hClassIcon) hWinIcon = hClassIcon;
                }
                if (hWinIcon) {
                    Wh_Log(L"Explorer IPC: Returning WM_GETICON/GetClassLongPtr icon %p as Win10 fallback", hWinIcon);
                    g_uwpIconCache[L"wm_icon_" + std::to_wstring((uintptr_t)hWinIcon) + L"_" + std::to_wstring(desiredSizePx)] = hWinIcon;
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
        return NULL;
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
    
    CreateWindowExW(0, L"WindhawkSWS_IpcWindow", L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);
    
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
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
            auto IsWindows11OrGreater = []() -> bool {
                DWORD build = 0; DWORD sz = sizeof(build);
                if (RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"CurrentBuildNumber", RRF_RT_REG_SZ, NULL, NULL, &sz) == ERROR_SUCCESS) {
                    // Read as string
                    std::wstring buf; buf.resize(sz/2);
                    if (RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"CurrentBuildNumber", RRF_RT_REG_SZ, NULL, &buf[0], &sz) == ERROR_SUCCESS) {
                        int buildNum = _wtoi(buf.c_str());
                        return buildNum >= 22000;
                    }
                }
                return false;
            };
            if (IsWindows11OrGreater()) {
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
                HICON hLocal = ResolveIconFromAumid(aumidLocal.c_str(), desiredSizePx);
                if (hLocal) {
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
    std::stable_sort(g_windows.begin(), g_windows.end(), [](const WindowEntry& a, const WindowEntry& b) {
        return IsIconic(a.hWnd) < IsIconic(b.hWnd);
    });
}

// Layout + Thumbnails

static int DpiScale(int val, int dpi) { return MulDiv(val, dpi, 96); }

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
                    if (ml < 0) ml = 0; if (mt < 0) mt = 0;
                    if (mr < 0) mr = 0; if (mb < 0) mb = 0;
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

    // "Shrink tasks to fit": pick a discrete scale from the task count. Must be
    // set before any GetHeaderIconSizePx()/GetHeaderRowHeightPx() call below so
    // icon and row sizes pick it up.
    g_autoFitScalePct = ComputeAutoFitScalePct(n);

    // DPI-scale all EP padding constants
    int masterPad    = DpiScale(SWS_MASTER_PADDING, dpiX);
    int elemPadTop   = DpiScale(SWS_ELEMENT_PAD_TOP, dpiY);
    int elemPadBot   = DpiScale(SWS_ELEMENT_PAD_BOTTOM, dpiY);
    int elemPadLeft  = DpiScale(SWS_ELEMENT_PAD_LEFT, dpiX);
    int elemPadRight = DpiScale(SWS_ELEMENT_PAD_RIGHT, dpiX);
    int padTop       = DpiScale(SWS_PAD_TOP, dpiY);
    int padBot       = DpiScale(SWS_PAD_BOTTOM, dpiY);
    int padLeft      = DpiScale(SWS_PAD_LEFT, dpiX);
    int padRight     = DpiScale(SWS_PAD_RIGHT, dpiX);
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
            g_windows[ji].sourceSize = {0, 0};
            g_windows[ji].rcCell = {0, 0, 0, 0};
            g_windows[ji].rcThumbActual = {0, 0, 0, 0};
            g_windows[ji].rcThumbSlot = {0, 0, 0, 0};
            for (const auto& kv : g_windows[ji].hThumbs) {
                if (kv.second) DwmUnregisterThumbnail(kv.second);
            }
            g_windows[ji].hThumbs.clear();
        }
        placedCount = startIdx;
    };

    if (!LayoutIsVertical()) {
        int maxRowW = 0;

        for (int idx = 0; idx < n; idx++) {
            int i = (g_layoutStartIndex + idx) % n;
            auto& w = g_windows[i];

            if (g_layoutStartIndex > 0 && idx > 0 && i < g_layoutStartIndex
                && ((g_layoutStartIndex + idx - 1) % n) >= g_layoutStartIndex
                && curX > initialLeft + masterPad) {
                if (curX - initialLeft > maxRowW) maxRowW = curX - initialLeft;
                curX = initialLeft + masterPad;
                if (curY + 2 * bottomInc - initialTop > maxH - masterPad) {
                    truncateRemaining(idx);
                    break;
                }
                curY = curY + bottomInc;
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

            if (g_layoutStartIndex > 0 && idx > 0 && i < g_layoutStartIndex
                && ((g_layoutStartIndex + idx - 1) % n) >= g_layoutStartIndex
                && curY > initialTop + masterPad) {
                curY = initialTop + masterPad;
                curX = curX + curColMaxW + rightInc;
                curColMaxW = 0;
                if (curX + rightInc - initialLeft > maxW - masterPad) {
                    truncateRemaining(idx);
                    break;
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
    for (auto& w : g_windows) {
        if (!w.hThumbs.count(g_hSwitcher)) {
            HTHUMBNAIL hT = NULL;
            if (SUCCEEDED(DwmRegisterThumbnail(g_hSwitcher, w.hWnd, &hT))) {
                w.hThumbs[g_hSwitcher] = hT;
                SIZE src = {0}; DwmQueryThumbnailSourceSize(hT, &src);
                w.sourceSize = src;
            }
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
            p.opacity = 255; p.fVisible = TRUE;
            // Only set DWM_TNP_RECTSOURCE when the crop is non-trivial (e.g. maximized
            // windows with invisible frame borders). Without it, DWM preserves the
            // window's visual style including rounded corners on Windows 11.
            bool needsCrop = (w.rcSourceCrop.left != 0 || w.rcSourceCrop.top != 0 ||
                              w.rcSourceCrop.right != w.sourceSize.cx || w.rcSourceCrop.bottom != w.sourceSize.cy);
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

// Draw a rectangular contour around a rect.
// direction: 1 = inner (shrinks inward), -1 = outer (grows outward)
// Uses GDI+ rounded path for contours when cornerRadius > 0.
static void DrawContour(HDC hdc, RECT rc, int contourSize, int direction, int overrideCornerRadius = -1) {
    COLORREF c = GetContourColor();
    BYTE r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);

    int cornerRadius = (overrideCornerRadius != -1) ? overrideCornerRadius : GetTaskUiCornerRadiusPx();
    if (cornerRadius > 0) {
        int penWidth = contourSize * g_dpiX / 96;
        if (penWidth < 1) penWidth = 1;

        RECT drawRc = rc;
        if (direction < 0) {
            InflateRect(&drawRc, 2, 2);
            cornerRadius += 2 + penWidth / 2;
        }

        int width = drawRc.right - drawRc.left - penWidth;
        int height = drawRc.bottom - drawRc.top - penWidth;
        if (width <= 0 || height <= 0) {
            return;
        }

        if (cornerRadius * 2 > width) cornerRadius = width / 2;
        if (cornerRadius * 2 > height) cornerRadius = height / 2;

        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::Pen pen(Gdiplus::Color(255, r, g, b), (Gdiplus::REAL)penWidth);

        Gdiplus::REAL left = (Gdiplus::REAL)drawRc.left + penWidth / 2.0f;
        Gdiplus::REAL top = (Gdiplus::REAL)drawRc.top + penWidth / 2.0f;
        Gdiplus::REAL w = (Gdiplus::REAL)width;
        Gdiplus::REAL h = (Gdiplus::REAL)height;
        Gdiplus::REAL d = (Gdiplus::REAL)(cornerRadius * 2);
        Gdiplus::GraphicsPath path;
        path.AddArc(left, top, d, d, 180, 90);
        path.AddArc(left + w - d, top, d, d, 270, 90);
        path.AddArc(left + w - d, top + h - d, d, d, 0, 90);
        path.AddArc(left, top + h - d, d, d, 90, 90);
        path.CloseFigure();
        graphics.DrawPath(&pen, &path);
        return;
    }

    Gdiplus::Graphics graphics(hdc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
    Gdiplus::SolidBrush solidBrush(Gdiplus::Color(255, r, g, b));

    if (direction < 0) {
        for (int i = 0; i < contourSize; i++) {
            RECT r_rect = rc;
            InflateRect(&r_rect, i + 2, i + 2);
            graphics.FillRectangle(&solidBrush, r_rect.left, r_rect.top, (r_rect.right - r_rect.left), 1);
            graphics.FillRectangle(&solidBrush, r_rect.left, r_rect.bottom - 1, (r_rect.right - r_rect.left), 1);
            graphics.FillRectangle(&solidBrush, r_rect.left, r_rect.top + 1, 1, (r_rect.bottom - r_rect.top) - 2);
            graphics.FillRectangle(&solidBrush, r_rect.right - 1, r_rect.top + 1, 1, (r_rect.bottom - r_rect.top) - 2);
        }
    } else {
        for (int i = 0; i < contourSize; i++) {
            RECT r_rect = rc;
            InflateRect(&r_rect, -i, -i);
            graphics.FillRectangle(&solidBrush, r_rect.left, r_rect.top, (r_rect.right - r_rect.left), 1);
            graphics.FillRectangle(&solidBrush, r_rect.left, r_rect.bottom - 1, (r_rect.right - r_rect.left), 1);
            graphics.FillRectangle(&solidBrush, r_rect.left, r_rect.top + 1, 1, (r_rect.bottom - r_rect.top) - 2);
            graphics.FillRectangle(&solidBrush, r_rect.right - 1, r_rect.top + 1, 1, (r_rect.bottom - r_rect.top) - 2);
        }
    }
}

static void DrawSelectionFill(HDC hdc, RECT rc) {
    RECT fillRc = rc;
    InflateRect(&fillRc, -1, -1);
    int width = fillRc.right - fillRc.left;
    int height = fillRc.bottom - fillRc.top;
    if (width <= 0 || height <= 0) {
        return;
    }

    COLORREF c = GetHighlightFillColor();
    BYTE r = GetRValue(c);
    BYTE g = GetGValue(c);
    BYTE b = GetBValue(c);

    int cornerRadius = GetTaskUiCornerRadiusPx();
    if (cornerRadius > 0) {
        int maxRadius = std::min(width, height) / 2;
        if (cornerRadius > maxRadius) cornerRadius = maxRadius;
    }

    Gdiplus::Graphics graphics(hdc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::SolidBrush brush(Gdiplus::Color(64, r, g, b));

    if (cornerRadius > 0) {
        Gdiplus::REAL left = (Gdiplus::REAL)fillRc.left;
        Gdiplus::REAL top = (Gdiplus::REAL)fillRc.top;
        Gdiplus::REAL w = (Gdiplus::REAL)(fillRc.right - fillRc.left);
        Gdiplus::REAL h = (Gdiplus::REAL)(fillRc.bottom - fillRc.top);
        Gdiplus::REAL d = (Gdiplus::REAL)(cornerRadius * 2);

        Gdiplus::GraphicsPath path;
        path.AddArc(left, top, d, d, 180, 90);
        path.AddArc(left + w - d, top, d, d, 270, 90);
        path.AddArc(left + w - d, top + h - d, d, d, 0, 90);
        path.AddArc(left, top + h - d, d, d, 90, 90);
        path.CloseFigure();
        graphics.FillPath(&brush, &path);
        return;
    }

    graphics.FillRectangle(&brush,
                           (Gdiplus::REAL)fillRc.left,
                           (Gdiplus::REAL)fillRc.top,
                           (Gdiplus::REAL)width,
                           (Gdiplus::REAL)height);
}

static RECT GetHeaderContentRectForEntry(const WindowEntry& e) {
    int padLeft = DpiScale(SWS_PAD_LEFT, g_dpiX);
    int padTop = DpiScale(SWS_PAD_TOP, g_dpiY);
    int padBottom = DpiScale(SWS_PAD_BOTTOM, g_dpiY);
    RECT rc = {
        e.rcCell.left + padLeft,
        e.rcCell.top + padTop,
        e.rcCell.right - padLeft,
        e.rcCell.bottom - padBottom,
    };

    if (!g_settings.showThumbnails) {
        return rc;
    }

    const RECT& rcHeaderSplit = ((e.rcThumbSlot.left != 0 || e.rcThumbSlot.top != 0 ||
                                  e.rcThumbSlot.right != 0 || e.rcThumbSlot.bottom != 0))
                                    ? e.rcThumbSlot
                                    : e.rcThumbActual;

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

static int GetHeaderTopForEntry(const WindowEntry& e) {
    RECT rcHeader = GetHeaderContentRectForEntry(e);
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

// Soft drop shadow behind a thumbnail. Drawn in the content layer, which sits
// below the DWM thumbnail, so only the expanded/offset border peeks out around
// the thumbnail edges, producing a drop-shadow halo.
static void DrawThumbnailShadow(HDC hdc, const RECT& rc, int cornerRadius) {
    int rcw = rc.right - rc.left;
    int rch = rc.bottom - rc.top;
    if (rcw <= 0 || rch <= 0) return;

    Gdiplus::Graphics gfx(hdc);
    gfx.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    int maxPass = DpiScale(6, g_dpiX);
    if (maxPass < 1) maxPass = 1;
    int yOff = DpiScale(2, g_dpiY);  // nudge down for a "drop" feel

    for (int pass = maxPass; pass > 0; --pass) {
        int shadowAlpha = 16 - (pass * 2);
        if (shadowAlpha < 1) shadowAlpha = 1;
        Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(shadowAlpha, 0, 0, 0));
        int sp = pass;
        Gdiplus::REAL sx = (Gdiplus::REAL)(rc.left - sp);
        Gdiplus::REAL sy = (Gdiplus::REAL)(rc.top - sp + yOff);
        Gdiplus::REAL sw = (Gdiplus::REAL)(rcw + sp * 2);
        Gdiplus::REAL sh = (Gdiplus::REAL)(rch + sp * 2);
        if (cornerRadius > 0) {
            Gdiplus::REAL sd = cornerRadius * 2.0f + sp * 2.0f;
            if (sd > sw) sd = sw;
            if (sd > sh) sd = sh;
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

// Shared drawing routine for both layered and buffered paint paths
static void DrawSwitcherContent(HDC hdc, bool fillBg, HWND hWnd) {
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

    // DPI-scale layout constants for drawing
    int padLeft    = DpiScale(SWS_PAD_LEFT, g_dpiX);
    int rowTitleH  = GetHeaderRowHeightPx();
    int iconSz     = GetHeaderIconSizePx();
    int cornerRadius = GetThumbnailCornerRadiusPx();

    for (int i = 0; i < (int)g_windows.size(); i++) {
        auto& e = g_windows[i];

        // Skip truncated (not placed) windows
        if (e.rcCell.left == 0 && e.rcCell.right == 0 &&
            e.rcCell.top == 0 && e.rcCell.bottom == 0) continue;

        // Selection highlight: configurable border/fill combinations on rcCell.
        if (i == g_selectedIndex) {
            if (HighlightHasFill()) {
                DrawSelectionFill(hdc, e.rcCell);
            }
            if (HighlightHasBorder()) {
                DrawContour(hdc, e.rcCell, SWS_CONTOUR_SIZE, 1);
            }
        }

        // Hover thumbnail border is now drawn in DrawSwitcherOverlay so it sits above the thumbnail mask

        // Drop shadow behind the thumbnail (below the DWM thumbnail layer).
        if (g_settings.showThumbnails && g_settings.showThumbnailShadow &&
            !(e.rcThumbActual.left == 0 && e.rcThumbActual.right == 0 &&
              e.rcThumbActual.top == 0 && e.rcThumbActual.bottom == 0)) {
            DrawThumbnailShadow(hdc, e.rcThumbActual, cornerRadius);
        }

        if (g_settings.showThumbnails && cornerRadius > 0 && ThemeIs(L"none")) {
            MaskRectCorners(hdc, e.rcThumbActual, cornerRadius);
            RECT inset = e.rcThumbActual;
            InflateRect(&inset, -1, -1);
            MaskRectCorners(hdc, inset, cornerRadius);
        }

        int closeBtnReserve = DpiScale(24, g_dpiX) + padLeft;

        // ---- Badge layout rendering path ----
        if (BadgeLayoutActive()) {
            // Badge title: draw centered in the header content rect
            if (g_settings.showTitle && e.title[0]) {
                RECT rcHeaderContent = GetHeaderContentRectForEntry(e);
                int titleH = GetHeaderTitleHeightPx();
                int headerTop = GetHeaderTopForEntry(e);
                RECT rcText = { rcHeaderContent.left, headerTop, rcHeaderContent.right, headerTop + titleH };
                if (g_hTheme) {
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

            // Badge icon is drawn in DrawSwitcherOverlay so it appears
            // above the DWM thumbnail (which composites on top of window content).
            continue;  // Skip normal header content rendering
        }

        // ---- Normal (non-badge) header content rendering ----
        // Keep centered header content stable: reserve close-button space consistently.
        // In vertical mode, never reserve space - close button overlays without displacement.
        int btnReserve = 0;
        bool isIconOnly = !g_settings.showThumbnails && !g_settings.showTitle && g_settings.showIcon;
        if (!HeaderIsVertical()) {
            if (!isIconOnly && !(g_settings.showThumbnails && ThumbnailIsSide())) {
                btnReserve = ((g_settings.centerTaskContent) || (i == g_hoverIndex && g_hoverWnd == hWnd))
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
            // Font rendering and icon boundaries scale slightly differently at higher DPIs.
            // At 100% (96 DPI), mathematical centering is visually perfect (shift = 0).
            // At >100% (e.g. 144 DPI), a small nudge downwards perfectly aligns their visual centers.
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
            DrawIconEx(hdc, iconX, iconY, e.hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
        } else {
            e.drawnIconX = contentLeft;
            e.drawnIconY = headerTop;
            e.drawnIconSz = 0;
        }
        // Title text
        if (g_settings.showTitle) {
            RECT rcText = { textLeft, textTop, textRight, textBottom };
            if (rcText.right < rcText.left) rcText.right = rcText.left;
            if (g_hTheme) {
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
    SelectObject(hdc, hOldFont);
}


// Rendering

static bool IsWindowTruncated(int idx);

static void DrawSwitcherOverlay(HDC hdc, HWND hWnd) {
    if (g_windows.empty()) return;

    int rowTitleH = GetHeaderRowHeightPx();
    int cornerRadius = GetThumbnailCornerRadiusPx();

    for (int idx = 0; idx < (int)g_windows.size(); idx++) {
        int i = (g_layoutStartIndex + idx) % g_windows.size();
        auto& e = g_windows[i];
        if (IsWindowTruncated(i)) continue;

        // ALWAYS draw opaque mask on the overlay window to cover the DWM thumbnail's square corners.
        // Even for the transparent theme, DWM on Windows 11 does not clip the thumbnail to the layered window's alpha channel.
        if (g_settings.showThumbnails && cornerRadius > 0) {
            COLORREF maskColor = GetBgColor();
            if (i == g_selectedIndex && HighlightHasFill()) {
                maskColor = GetHighlightFillColor();
            }
            MaskRectCorners(hdc, e.rcThumbActual, cornerRadius, true, maskColor);
            RECT inset = e.rcThumbActual;
            InflateRect(&inset, -1, -1);
            MaskRectCorners(hdc, inset, cornerRadius, true, maskColor);
        }

        // Hover thumbnail border: outer contour on rcThumbActual (drawn here to overlay the mask)
        if (i == g_hoverIndex && g_hoverWnd == hWnd && g_settings.showThumbnails && g_settings.showHoverBorder) {
            DrawContour(hdc, e.rcThumbActual, 1, -1, cornerRadius);
        }

        // Badge layout: draw icon overlay on thumbnail (must be in overlay layer
        // because DWM thumbnails composite on top of window content)
        if (BadgeLayoutActive() && g_settings.showIcon && e.hIcon) {
            int iconSz = GetHeaderIconSizePx();
            int thumbW = e.rcThumbActual.right - e.rcThumbActual.left;
            int thumbHt = e.rcThumbActual.bottom - e.rcThumbActual.top;
            if (thumbW > 0 && thumbHt > 0) {
                int badgePad = DpiScale(g_settings.badgeIconPadding, g_dpiX);
                int bIconX = 0, bIconY = 0;

                // Horizontal positioning
                if (BadgeIconPositionIs(L"topLeft") || BadgeIconPositionIs(L"centerLeft") || BadgeIconPositionIs(L"bottomLeft")) {
                    bIconX = e.rcThumbActual.left + badgePad;
                } else if (BadgeIconPositionIs(L"topRight") || BadgeIconPositionIs(L"centerRight") || BadgeIconPositionIs(L"bottomRight")) {
                    bIconX = e.rcThumbActual.right - iconSz - badgePad;
                } else {
                    bIconX = e.rcThumbActual.left + (thumbW - iconSz) / 2;
                }
                // Vertical positioning
                if (BadgeIconPositionIs(L"topLeft") || BadgeIconPositionIs(L"topCenter") || BadgeIconPositionIs(L"topRight")) {
                    bIconY = e.rcThumbActual.top + badgePad;
                } else if (BadgeIconPositionIs(L"bottomLeft") || BadgeIconPositionIs(L"bottomCenter") || BadgeIconPositionIs(L"bottomRight")) {
                    bIconY = e.rcThumbActual.bottom - iconSz - badgePad;
                } else {
                    bIconY = e.rcThumbActual.top + (thumbHt - iconSz) / 2;
                }

                bIconX += DpiScale(g_settings.badgeIconOffsetX, g_dpiX);
                bIconY += DpiScale(g_settings.badgeIconOffsetY, g_dpiY);

                e.drawnIconX = bIconX;
                e.drawnIconY = bIconY;
                e.drawnIconSz = iconSz;

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
                                if (sd > sw) sd = sw; if (sd > sh) sd = sh;
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
                        if (d > w) d = w; if (d > h) d = h;
                        path.AddArc(x, y, d, d, 180, 90);
                        path.AddArc(x + w - d, y, d, d, 270, 90);
                        path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
                        path.AddArc(x, y + h - d, d, d, 90, 90);
                        path.CloseFigure();
                        gfx.FillPath(&bgBrush, &path);
                    } else {
                        gfx.FillRectangle(&bgBrush, bgX, bgY, bgSize, bgSize);
                    }
                    DrawIconEx(hdc, bIconX, bIconY, e.hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
                } else {
                    Gdiplus::Bitmap* pBmp = CreateIconShadowBitmap(e.hIcon, iconSz, iconSz, 0.08f);
                    if (pBmp) {
                        int dx[] = { 0, 1, 0, -1, 1 };
                        int dy[] = { 1, 0, -1, 0, 1 };
                        for (int p = 0; p < 5; ++p) {
                            gfx.DrawImage(pBmp, bIconX + DpiScale(dx[p], g_dpiX), bIconY + DpiScale(dy[p] + 2, g_dpiY), iconSz, iconSz);
                        }
                        delete pBmp;
                    }
                    DrawIconEx(hdc, bIconX, bIconY, e.hIcon, iconSz, iconSz, 0, NULL, DI_NORMAL);
                }
            }
        }

        // Close button (positioned at top-right of the cell, in title area)
        if (i == g_hoverIndex && g_hoverWnd == hWnd) {
            int btnSz = DpiScale(24, g_dpiX);
            int bx, by;
            if (rowTitleH == 0 || (g_settings.showThumbnails && ThumbnailIsSide()) || BadgeLayoutActive()) {
                int btnPadding = DpiScale(4, g_dpiX);
                bx = e.rcThumbActual.right - btnSz - btnPadding;
                by = e.rcThumbActual.top + btnPadding;
            } else if (!g_settings.showThumbnails && !g_settings.showTitle && g_settings.showIcon) {
                int padLeft = DpiScale(SWS_PAD_LEFT, g_dpiX);
                int padTop = DpiScale(SWS_PAD_TOP, g_dpiY);
                int contentLeft = e.rcCell.left + padLeft;
                int contentRight = e.rcCell.right - padLeft;
                int iconSz = GetHeaderIconSizePx();
                int availableW = contentRight - contentLeft;
                if (availableW < 0) availableW = 0;
                
                int iconX = contentLeft;
                int iconY = e.rcCell.top + padTop + (rowTitleH - iconSz) / 2;
                
                if (g_settings.centerTaskContent) {
                    if (iconSz < availableW) {
                        iconX = contentLeft + (availableW - iconSz) / 2;
                    }
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
                RECT rcHeaderContent = GetHeaderContentRectForEntry(e);
                int headerTop = GetHeaderTopForEntry(e);
                bx = rcHeaderContent.right - btnSz;
                by = HeaderIsVertical() ? headerTop
                                        : (headerTop + (rowTitleH - btnSz) / 2);
            }

            if (g_isCloseHovered) {
                // Red rounded background for close button
                Gdiplus::Graphics graphics(hdc);
                graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                int btnRadius = GetCloseButtonCornerRadiusPx();
                Gdiplus::SolidBrush redBrush(Gdiplus::Color(255, 196, 43, 28));
                if (btnRadius > 0) {
                    if (btnRadius * 2 > btnSz) btnRadius = btnSz / 2;
                    Gdiplus::GraphicsPath path;
                    Gdiplus::REAL d = (Gdiplus::REAL)(btnRadius * 2);
                    path.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)by, d, d, 180, 90);
                    path.AddArc((Gdiplus::REAL)(bx + btnSz) - d, (Gdiplus::REAL)by, d, d, 270, 90);
                    path.AddArc((Gdiplus::REAL)(bx + btnSz) - d, (Gdiplus::REAL)(by + btnSz) - d, d, d, 0, 90);
                    path.AddArc((Gdiplus::REAL)bx, (Gdiplus::REAL)(by + btnSz) - d, d, d, 90, 90);
                    path.CloseFigure();
                    graphics.FillPath(&redBrush, &path);
                } else {
                    graphics.FillRectangle(&redBrush, (Gdiplus::REAL)bx, (Gdiplus::REAL)by,
                                           (Gdiplus::REAL)btnSz, (Gdiplus::REAL)btnSz);
                }
            }

            // Draw X with GDI+ for smooth diagonal lines only.
            Gdiplus::Graphics graphics(hdc);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            COLORREF xc = (g_isCloseHovered || g_isDarkMode) ? RGB(255, 255, 255) : RGB(0, 0, 0);
            Gdiplus::Pen xPen(Gdiplus::Color(255, GetRValue(xc), GetGValue(xc), GetBValue(xc)), 1.5f * g_dpiX / 96.0f);
            int p = (btnSz == DpiScale(16, g_dpiX)) ? DpiScale(4, g_dpiX) : DpiScale(7, g_dpiX);
            graphics.DrawLine(&xPen, bx + p, by + p, bx + btnSz - p, by + btnSz - p);
            graphics.DrawLine(&xPen, bx + btnSz - p, by + p, bx + p, by + btnSz - p);
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
            if (e.drawnIconSz > 0) {
                badgeX = e.drawnIconX + e.drawnIconSz - (badgeW / 2);
                badgeY = e.drawnIconY - (badgeH / 2);
            } else {
                int cellPad = DpiScale(4, g_dpiX);
                badgeX = e.rcCell.right - badgeW - cellPad;
                badgeY = e.rcCell.top + cellPad;
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
                    if (sd > sw) sd = sw; if (sd > sh) sd = sh;
                    
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
}

static void PaintSwitcherOverlay() {
    if (!g_hCloseBtnWnd || !g_isVisible) return;
    HWND targetWnd = g_hoverWnd ? g_hoverWnd : g_hSwitcher;
    RECT rc; GetClientRect(targetWnd, &rc);
    int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0) return;
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    BITMAPINFO bmi = {}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = NULL;
    HBITMAP hBmp = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);

    // Clip the overlay DC to the same rounded rect as the main window.
    // SetWindowRgn is ignored for layered windows using UpdateLayeredWindow,
    // so we must clip manually to prevent corner masks from bleeding outside
    // the main window's rounded boundary.
    INT cp = GetCornerPref();
    if (cp == 1 && wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        int radius = MulDiv(g_settings.customCornerRadius, g_dpiX, 96);
        HRGN hClip = CreateRoundRectRgn(0, 0, w + 1, h + 1, radius * 2, radius * 2);
        SelectClipRgn(hdcMem, hClip);
        DeleteObject(hClip);
    } else if (cp == 2) {
        int radius = MulDiv(8, g_dpiX, 96);
        HRGN hClip = CreateRoundRectRgn(0, 0, w + 1, h + 1, radius * 2, radius * 2);
        SelectClipRgn(hdcMem, hClip);
        DeleteObject(hClip);
    } else if (cp == 3) {
        int radius = MulDiv(4, g_dpiX, 96);
        HRGN hClip = CreateRoundRectRgn(0, 0, w + 1, h + 1, radius * 2, radius * 2);
        SelectClipRgn(hdcMem, hClip);
        DeleteObject(hClip);
    }

    DrawSwitcherOverlay(hdcMem, targetWnd);

    POINT ptSrc = {0,0}; SIZE sz = {w, h};
    RECT wr; GetWindowRect(targetWnd, &wr);
    POINT ptDst = { wr.left, wr.top };
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hCloseBtnWnd, hdcScreen, &ptDst, &sz, hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);
    SelectObject(hdcMem, hOld); DeleteObject(hBmp); DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

static void PaintSwitcher() {
    if (!g_hSwitcher || !g_isVisible) return;
    if (ThemeIs(L"none")) {
        // Layered window path: draw to off-screen DIB, UpdateLayeredWindow
        RECT rc; GetClientRect(g_hSwitcher, &rc);
        int w = rc.right, h = rc.bottom;
        if (w <= 0 || h <= 0) return;
        HDC hdcScreen = GetDC(g_hSwitcher);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        BITMAPINFO bmi = {}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
        void* bits = NULL;
        HBITMAP hBmp = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
        HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);
        DrawSwitcherContent(hdcMem, true, g_hSwitcher);
        POINT ptSrc = {0,0}; SIZE sz = {w, h};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(g_hSwitcher, hdcScreen, NULL, &sz, hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);
        for (HWND hMirror : g_hMirrorSwitchers) {
            if (IsWindow(hMirror)) {
                DrawSwitcherContent(hdcMem, true, hMirror);
                HDC hdcMirrorScreen = GetDC(hMirror);
                UpdateLayeredWindow(hMirror, hdcMirrorScreen, NULL, &sz, hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);
                ReleaseDC(hMirror, hdcMirrorScreen);
            }
        }
        SelectObject(hdcMem, hOld); DeleteObject(hBmp); DeleteDC(hdcMem);
        ReleaseDC(g_hSwitcher, hdcScreen);
        PaintSwitcherOverlay();
    } else {
        // Acrylic: trigger WM_PAINT via InvalidateRect
        InvalidateRect(g_hSwitcher, NULL, TRUE);
        UpdateWindow(g_hSwitcher);
        for (HWND hMirror : g_hMirrorSwitchers) {
            if (IsWindow(hMirror)) {
                InvalidateRect(hMirror, NULL, TRUE);
                UpdateWindow(hMirror);
            }
        }
        PaintSwitcherOverlay();
    }
}

// Switcher Show / Hide / Switch

static void GetOffscreenDelayPosition(int* x, int* y) {
    // Put the 1x1 window just outside the virtual screen bounds.
    // This avoids alpha/layered hacks while keeping the window shown/foreground.
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);

    *x = vx - 2;
    *y = vy - 2;
}

static void CancelPendingShow() {
    if (g_hSwitcher) {
        KillTimer(g_hSwitcher, SWS_SHOW_DELAY_TIMER_ID);
    }

    g_isPendingShow = false;
}

static void ShowPendingOffscreenWindow() {
    int x, y;
    GetOffscreenDelayPosition(&x, &y);

    // Show as 1x1 off-screen. No transparency/style changes needed.
    SetWindowPos(g_hSwitcher, HWND_TOPMOST, x, y, 1, 1, SWP_NOACTIVATE);

    ShowWindow(g_hSwitcher, SW_SHOWNA);
    SetForegroundWindow(g_hSwitcher);
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_isVisible && wParam == WM_MOUSEWHEEL) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
        bool ok = ScrollIs(L"always") || (ScrollIs(L"stickyOnly") && g_isSticky);
        if (ok) {
            int dir = (short)HIWORD(pMouseStruct->mouseData) > 0 ? -1 : 1;
            if (g_settings.reverseScrollDirection) dir = -dir;
            // Defer the heavy CycleLinear work to the wndproc; low-level hook
            // callbacks run synchronously on the raw-input path and must stay
            // trivial or Windows will silently drop the hook.
            PostMessage(g_hSwitcher, WM_SWS_SCROLL, (WPARAM)dir, 0);
            return 1;
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

    SetWindowPos(g_hSwitcher, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);
    if (g_hCloseBtnWnd) {
        SetWindowPos(g_hCloseBtnWnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);
        ShowWindow(g_hCloseBtnWnd, SW_SHOWNA);
    }

    RegisterThumbnails();
    PaintSwitcher();

    if (!g_isSticky) {
        SetTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID, 50, NULL);
    }
}

static void ApplyThemeToWindow(HWND hWnd) {
    if (g_SetWindowCompositionAttribute) {
        ACCENT_POLICY a = {}; a.AccentState = 0;
        WINDOWCOMPOSITIONATTRIBDATA d = {19, &a, sizeof(a)};
        g_SetWindowCompositionAttribute(hWnd, &d);
    }
    MARGINS marGlassInset = ThemeIs(L"mica") ? MARGINS{-1, -1, -1, -1} : MARGINS{0, 0, 0, 0};
    DwmExtendFrameIntoClientArea(hWnd, &marGlassInset);

    LONG_PTR exs = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (ThemeIs(L"none")) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exs | WS_EX_LAYERED);
    } else {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exs & ~WS_EX_LAYERED);
        BOOL dark = g_isDarkMode;
        DwmSetWindowAttribute(hWnd, 20, &dark, sizeof(dark));
        if (ThemeIs(L"mica")) {
            int micaVal = 2; // DWMSBT_MAINWINDOW
            HRESULT hr = DwmSetWindowAttribute(hWnd, 38, &micaVal, sizeof(micaVal));
            if (FAILED(hr)) {
                int oldMicaVal = 1;
                hr = DwmSetWindowAttribute(hWnd, 1029, &oldMicaVal, sizeof(oldMicaVal));
            }
            if (FAILED(hr)) {
                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            }
            // Force the window to evaluate its NCACTIVATE state so Mica stays active
            SendMessage(hWnd, WM_NCACTIVATE, TRUE, 0);
        }
        if (ThemeIs(L"backdrop") && g_SetWindowCompositionAttribute) {
            DWORD blur = (DWORD)((g_settings.opacity / 100.0) * 255);
            COLORREF bg = GetBgColor();
            ACCENT_POLICY accent = {};
            accent.AccentState = 4 /* ACCENT_ENABLE_ACRYLICBLURBEHIND */;
            accent.AccentFlags = 0;
            accent.GradientColor = (blur << 24) | (bg & 0x00FFFFFF);
            WINDOWCOMPOSITIONATTRIBDATA data = {19, &accent, sizeof(accent)};
            g_SetWindowCompositionAttribute(hWnd, &data);
        }
        SetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)GetStockObject(BLACK_BRUSH));
    }
    INT cp = GetCornerPref();
    if (cp == 1 && wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        COLORREF none = 0xFFFFFFFE; // DWMWA_COLOR_NONE
        DwmSetWindowAttribute(hWnd, 34 /* DWMWA_BORDER_COLOR */, &none, sizeof(none));
        
        // Always use DONOTROUND for custom corners. SetWindowRgn defines the
        // window shape; DWM's own rounding (ROUND/ROUNDSMALL) uses a fixed
        // radius that conflicts with our custom radius, creating visible
        // glass artifacts at the corners.
        INT shadowCp = 1; // DWMWCP_DONOTROUND
        DwmSetWindowAttribute(hWnd, 33, &shadowCp, sizeof(shadowCp));
    } else {
        DwmSetWindowAttribute(hWnd, 33, &cp, sizeof(cp));
        COLORREF defaultColor = 0xFFFFFFFF; // DWMWA_COLOR_DEFAULT
        DwmSetWindowAttribute(hWnd, 34, &defaultColor, sizeof(defaultColor));
    }
}

static BOOL WINAPI MirrorEnumProc(HMONITOR hM, HDC, LPRECT, LPARAM) {
    if (hM != g_hCurrentMonitor) {
        MONITORINFO mInfo = { sizeof(mInfo) };
        GetMonitorInfoW(hM, &mInfo);
        int mx, my;
        GetSwitcherPosition(mInfo.rcWork, &mx, &my);
        HWND hMirror = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, SWS_CLASSNAME, L"", WS_POPUP | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, mx, my, g_winW, g_winH, g_hSwitcher, NULL, GetModuleHandle(NULL), NULL);
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

static void ShowSwitcher(bool sticky) {
    POINT pt; GetCursorPos(&pt);
    HMONITOR hMon = (wcscmp(g_settings.switcherDisplayBehavior, L"primaryOnly") == 0) ?
                    MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY) :
                    MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    g_hCurrentMonitor = hMon;
    UnregisterThumbnails(); BuildWindowList();
    if (g_windows.empty()) return;

    g_isDarkMode = ShouldUseDarkMode(); g_isSticky = sticky;

    g_layoutStartIndex = 0; // Always start from the first window on initial show
    g_drilledIn = false;
    g_savedAppList.clear();
    g_consumeEscUp = false;
    g_selectedIndex = (g_windows.size() > 1) ? 1 : 0;
    g_hoverIndex = -1;
    g_hoverWnd = NULL;
    g_isCloseHovered = false;

    RegisterThumbnailsEarly();
    ComputeLayout(hMon);
    if (g_winW <= 0 || g_winH <= 0) return;

    // Recreate font for current DPI
    if (g_hFont) { DeleteObject(g_hFont); g_hFont = NULL; }
    g_hFont = CreateScaledFont(g_dpiY);

    MONITORINFO mi = { sizeof(mi) }; GetMonitorInfoW(hMon, &mi);
    int cx, cy;
    GetSwitcherPosition(mi.rcWork, &cx, &cy);

    ApplyThemeToWindow(g_hSwitcher);

    INT cp = GetCornerPref();
    if (cp == 1 && wcscmp(g_settings.cornerPreference, L"custom") == 0) {
        int radius = MulDiv(g_settings.customCornerRadius, g_dpiX, 96);
        HRGN hRgn1 = CreateRoundRectRgn(0, 0, g_winW + 1, g_winH + 1, radius * 2, radius * 2);
        SetWindowRgn(g_hSwitcher, hRgn1, TRUE);
        // NOTE: Do NOT call SetWindowRgn on g_hCloseBtnWnd — it is a layered
        // window using UpdateLayeredWindow, which ignores SetWindowRgn.
        // Instead, the overlay DC is clipped in PaintSwitcherOverlay.
    } else {
        SetWindowRgn(g_hSwitcher, NULL, TRUE);
    }

    g_pendingSwitcherRect = {
        cx,
        cy,
        cx + g_winW,
        cy + g_winH
    };

    CancelPendingShow();

    if (g_settings.showDelay > 0 && !sticky) {
        g_isPendingShow = true;
        g_isVisible = false;

        ShowPendingOffscreenWindow();

        SetTimer(g_hSwitcher, SWS_SHOW_DELAY_TIMER_ID, g_settings.showDelay, NULL);
        return;
    }

    g_isPendingShow = false;
    g_isVisible = true;
    if (!g_hMouseHook) {
        g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
    }

    SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    ShowWindow(g_hSwitcher, SW_SHOWNA);
    if (wcscmp(g_settings.switcherDisplayBehavior, L"allMonitors") == 0 || g_showAllMonitors) {
        EnumDisplayMonitors(NULL, NULL, MirrorEnumProc, 0);
    }
    
    if (g_hCloseBtnWnd) {
        SetWindowPos(g_hCloseBtnWnd, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
        ShowWindow(g_hCloseBtnWnd, SW_SHOWNA);
    }

    SetForegroundWindow(g_hSwitcher);

    RegisterThumbnails();
    PaintSwitcher();

    if (!sticky) {
        SetTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID, 50, NULL);
    }
}

static void HideSwitcher() {
    g_showAllMonitors = false;
    CancelPendingShow();
    if (g_hSwitcher) KillTimer(g_hSwitcher, SWS_ALT_POLL_TIMER_ID);

    for (HWND hMirror : g_hMirrorSwitchers) {
        if (IsWindow(hMirror)) DestroyWindow(hMirror);
    }
    g_hMirrorSwitchers.clear();

    UnregisterThumbnails();
    if (g_hSwitcher) {
        ShowWindow(g_hSwitcher, SW_HIDE);
    }
    if (g_hCloseBtnWnd) {
        ShowWindow(g_hCloseBtnWnd, SW_HIDE);
    }

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
}

static void SwitchToSelected() {
    if (g_selectedIndex < 0 || g_selectedIndex >= (int)g_windows.size()) { HideSwitcher(); return; }
    HWND hT = g_windows[g_selectedIndex].hWnd;
    std::vector<HWND> groupWindows;
    if (g_settings.showApplications && g_settings.restoreAllWindows) {
        groupWindows = g_windows[g_selectedIndex].groupWindows;
    }
    HideSwitcher();
    for (HWND hw : groupWindows) {
        if (IsWindow(hw) && hw != hT && IsIconic(hw)) ShowWindow(hw, SW_RESTORE);
    }
    if (IsWindow(hT)) {
        HWND hP = GetLastActivePopup(hT);
        HWND hF = IsWindowVisible(hP) ? hP : hT;
        if (IsIconic(hF)) ShowWindow(hF, SW_RESTORE);
        SwitchToThisWindow(hF, TRUE);
    }
}

// Helper: check if a window is truncated (not placed in current layout)
static bool IsWindowTruncated(int idx) {
    auto& w = g_windows[idx];
    return w.rcCell.left == 0 && w.rcCell.right == 0 &&
           w.rcCell.top == 0 && w.rcCell.bottom == 0;
}

// Helper: recompute layout and reposition switcher window
static void RecomputeAndReposition() {
    UnregisterThumbnails();
    RegisterThumbnailsEarly();
    HMONITOR hMon = MonitorFromWindow(g_hSwitcher, MONITOR_DEFAULTTONEAREST);
    ComputeLayout(hMon);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(hMon, &mi);
    int cx, cy;
    GetSwitcherPosition(mi.rcWork, &cx, &cy);

    g_pendingSwitcherRect = {
        cx,
        cy,
        cx + g_winW,
        cy + g_winH
    };

    if (g_isPendingShow) {
        int ox, oy;
        GetOffscreenDelayPosition(&ox, &oy);

        SetWindowPos(g_hSwitcher, HWND_TOPMOST, ox, oy, 1, 1, SWP_NOACTIVATE);
        return;
    }

    SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    if (g_hCloseBtnWnd && g_isVisible && !g_isPendingShow) {
        SetWindowPos(g_hCloseBtnWnd, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    }
    RegisterThumbnails();
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
        InternalGetWindowText(hw, e.title, 256);
        if (!e.title[0]) GetWindowTextW(hw, e.title, 256);
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
    g_hoverWnd = NULL;
    g_isCloseHovered = false;
    RecomputeAndReposition();
    PaintSwitcher();
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
    g_hoverWnd = NULL;
    g_isCloseHovered = false;
    RecomputeAndReposition();
    PaintSwitcher();
}

static void ToggleAppDrill() {
    if (g_drilledIn) ExitAppGroup();
    else EnterAppGroup();
}

// Linear navigation: Tab, Shift+Tab, Left, Right, Hotkeys, Scroll
static void CycleLinear(int delta) {
    if (g_windows.empty()) return;
    int n = (int)g_windows.size();
    g_selectedIndex = ((g_selectedIndex + delta) % n + n) % n;

    // If the newly selected window is truncated, recompute layout
    if (IsWindowTruncated(g_selectedIndex)) {
        // Always try resetting to index 0 first (top-first view)
        g_layoutStartIndex = 0;
        RecomputeAndReposition();

        // If still truncated, scroll forward line-by-line until visible
        // (row in horizontal mode, column in vertical mode).
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
                g_layoutStartIndex = g_selectedIndex;
            } else {
                g_layoutStartIndex = newStart;
            }
            RecomputeAndReposition();
        }
    }

    PaintSwitcher();
}

// Directional navigation: Up, Down (EP-style row-based with nearest-column match)
// Walks in layout placement order (from g_layoutStartIndex, wrapping) instead of raw list index.
static void CycleDirectional(int vertDelta) {
    if (g_windows.empty()) return;
    int n = (int)g_windows.size();
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
            // First try reset to 0 (handles wrap-to-top / DOWN from last row)
            g_layoutStartIndex = 0;
            RecomputeAndReposition();

            // If still truncated, scroll forward line-by-line.
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
                    g_layoutStartIndex = windowIdx;
                } else {
                    g_layoutStartIndex = newStart;
                }
                RecomputeAndReposition();
            }

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
    PaintSwitcher();
}

static int HitTest(int x, int y) {
    for (int i = 0; i < (int)g_windows.size(); i++) {
        RECT r = g_windows[i].rcCell;
        if (x >= r.left && x < r.right && y >= r.top && y < r.bottom) return i;
    }
    return -1;
}
static int HitTestThumb(int x, int y) {
    if (!g_settings.showThumbnails) return -1;
    for (int i = 0; i < (int)g_windows.size(); i++) {
        RECT r = g_windows[i].rcThumbActual;
        if (x >= r.left && x < r.right && y >= r.top && y < r.bottom) return i;
    }
    return -1;
}


// WndProc

static void SWS_RegisterHotkeys();

static void UpdateEntryForWindow(WindowEntry& e) {
    InternalGetWindowText(e.hWnd, e.title, 256);
    if (!e.title[0]) GetWindowTextW(e.hWnd, e.title, 256);
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

// Close the window for the entry at idx (graceful WM_CLOSE, same as the close
// button), remove it from the list and relayout. Shared by the close button,
// middle-click and the Q key.
static void CloseSwitcherEntry(int idx) {
    if (idx < 0 || idx >= (int)g_windows.size()) return;
    
    bool eraseEntry = true;
    if (g_settings.showApplications && g_windows[idx].groupWindows.size() > 1) {
        if (wcscmp(g_settings.groupCloseBehavior, L"closeAll") == 0) {
            for (HWND hw : g_windows[idx].groupWindows) {
                PostMessage(hw, WM_CLOSE, 0, 0);
            }
        } else {
            // closeRecent (Default)
            HWND closedHwnd = g_windows[idx].hWnd;
            PostMessage(closedHwnd, WM_CLOSE, 0, 0);
            
            auto& group = g_windows[idx].groupWindows;
            group.erase(std::remove(group.begin(), group.end(), closedHwnd), group.end());
            
            if (!group.empty()) {
                eraseEntry = false;
                g_windows[idx].hWnd = group[0];
                UpdateEntryForWindow(g_windows[idx]);
                for (const auto& kv : g_windows[idx].hThumbs) {
                    if (kv.second) DwmUnregisterThumbnail(kv.second);
                }
                g_windows[idx].hThumbs.clear();
            }
        }
    } else {
        PostMessage(g_windows[idx].hWnd, WM_CLOSE, 0, 0);
    }

    if (eraseEntry) {
        g_windows.erase(g_windows.begin() + idx);
    }

    if (g_windows.empty()) { HideSwitcher(); return; }
    if (g_selectedIndex >= (int)g_windows.size()) g_selectedIndex = (int)g_windows.size() - 1;
    UnregisterThumbnails();
    RegisterThumbnailsEarly();
    ComputeLayout(g_hCurrentMonitor);
    // Resize and re-center the window to match new layout
    MONITORINFO rmi = { sizeof(rmi) }; GetMonitorInfoW(g_hCurrentMonitor, &rmi);
    int cx, cy;
    GetSwitcherPosition(rmi.rcWork, &cx, &cy);
    SetWindowPos(g_hSwitcher, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    if (g_hCloseBtnWnd) {
        SetWindowPos(g_hCloseBtnWnd, HWND_TOPMOST, cx, cy, g_winW, g_winH, SWP_NOACTIVATE);
    }
    RegisterThumbnails();
    g_hoverIndex = -1;
    g_hoverWnd = NULL;
    g_isCloseHovered = false;
    PaintSwitcher();
}

static bool IsSwitcherWindow(HWND hWnd) {
    if (!hWnd) return false;
    if (hWnd == g_hSwitcher) return true;
    for (HWND h : g_hMirrorSwitchers) {
        if (hWnd == h) return true;
    }
    return false;
}

static LRESULT CALLBACK SwitcherWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
    }
    if (uMsg == WM_HOTKEY) {
        int id = (int)wParam;
        bool isBackward = false;
        bool isCtrl = false;

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
            if (!UseAltBacktickBackward()) return 0;
            isBackward = true;
            break;
        default:
            return 0;
        }

        if (!g_isVisible && !g_isPendingShow) {
            ShowSwitcher(isCtrl);

            if (isBackward && g_windows.size() > 1) {
                g_selectedIndex = (int)g_windows.size() - 1;

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
        BP_PAINTPARAMS params = { sizeof(params) };
        params.dwFlags = BPPF_ERASE;
        HDC hdcBuf = NULL;
        HPAINTBUFFER hBP = BeginBufferedPaint(hdc, &rc, BPBF_TOPDOWNDIB, &params, &hdcBuf);
        if (hBP) {
            DrawSwitcherContent(hdcBuf, false, hWnd);
            // Do NOT call BufferedPaintSetAlpha here — it would force all pixels
            // to opaque (alpha=255), blocking the acrylic blur from showing through.
            // BPPF_ERASE already cleared the buffer to RGBA(0,0,0,0) = transparent.
            // Our contour/icon/text drawing sets correct per-pixel alpha.
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
        if (wParam == VK_ESCAPE && g_isVisible) {
            if (g_consumeEscUp) { g_consumeEscUp = false; return 0; }
            HideSwitcher();
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
            if (wParam == VK_ESCAPE) {
                if (g_drilledIn) { ExitAppGroup(); g_consumeEscUp = true; }
                else HideSwitcher();
                return 0;
            }
            if (wParam == VK_RETURN || wParam == VK_SPACE) { SwitchToSelected(); return 0; }
            if (wParam == 'Q') {
                bool isRepeat = (lParam & 0x40000000) != 0;
                if (!isRepeat) CloseSwitcherEntry(g_selectedIndex);
                return 0;
            }
        }
        break;
    // (Removed duplicate combined case for WM_SYSKEYUP and WM_KEYUP)
    case WM_SWS_SCROLL:
        if (g_isVisible) {
            CycleLinear((int)wParam);
        }
        return 0;
    case WM_SETCURSOR:
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        return TRUE;
    case WM_MOUSEMOVE: {
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        int idx = g_settings.showThumbnails ? HitTestThumb(x, y) : HitTest(x, y);
        if (idx < 0) idx = HitTest(x, y);
        
        bool closeHovered = false;
        if (idx >= 0) {
            auto& e = g_windows[idx];
            int titleH = GetHeaderRowHeightPx();
            int btnSz = DpiScale(24, g_dpiX);
            int bx, by;
            if (titleH == 0 || (g_settings.showThumbnails && ThumbnailIsSide()) || BadgeLayoutActive()) {
                int btnPadding = DpiScale(4, g_dpiX);
                bx = e.rcThumbActual.right - btnSz - btnPadding;
                by = e.rcThumbActual.top + btnPadding;
            } else if (!g_settings.showThumbnails && !g_settings.showTitle && g_settings.showIcon) {
                int padL = DpiScale(SWS_PAD_LEFT, g_dpiX);
                int padT = DpiScale(SWS_PAD_TOP, g_dpiY);
                int contentLeft = e.rcCell.left + padL;
                int contentRight = e.rcCell.right - padL;
                int iconSz = GetHeaderIconSizePx();
                int availableW = contentRight - contentLeft;
                if (availableW < 0) availableW = 0;
                
                int iconX = contentLeft;
                int iconY = e.rcCell.top + padT + (titleH - iconSz) / 2;
                
                if (g_settings.centerTaskContent) {
                    if (iconSz < availableW) {
                        iconX = contentLeft + (availableW - iconSz) / 2;
                    }
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
                RECT rcHeaderContent = GetHeaderContentRectForEntry(e);
                int headerTop = GetHeaderTopForEntry(e);
                bx = rcHeaderContent.right - btnSz;
                by = HeaderIsVertical() ? headerTop
                                        : (headerTop + (titleH - btnSz) / 2);
            }
            if (x >= bx && x <= bx + btnSz && y >= by && y <= by + btnSz) {
                closeHovered = true;
            }
        }
        
        if (idx != g_hoverIndex || closeHovered != g_isCloseHovered || g_hoverWnd != hWnd) {
            g_hoverIndex = idx;
            g_hoverWnd = hWnd;
            g_isCloseHovered = closeHovered;
            PaintSwitcher();
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        int idx = HitTest(x, y);
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
        if (wParam == WA_INACTIVE && g_isVisible) {
            HWND hNewActive = (HWND)lParam;
            if (!IsSwitcherWindow(hNewActive)) {
                HideSwitcher();
            }
            return 0;
        }
        break;
    case WM_KILLFOCUS:
        if (g_isVisible) {
            HWND hNewFocus = (HWND)wParam;
            if (!IsSwitcherWindow(hNewFocus)) {
                HideSwitcher();
            }
            return 0;
        }
        break;
    case WM_ERASEBKGND: return 1;
    case WM_DESTROY: UnregisterThumbnails(); return 0;
    }

    if (g_shellHookMsg && uMsg == g_shellHookMsg && g_isVisible) {
        int code = (int)(wParam & 0x7FFF);
        if (code == HSHELL_WINDOWDESTROYED) {
            HWND hS = (HWND)lParam;
            for (int i = 0; i < (int)g_windows.size(); i++)
                if (g_windows[i].hWnd == hS) { ShowSwitcher(g_isSticky); break; }
        }
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// Hotkey Helpers

static HANDLE g_hHotkeyMutex = NULL;

static void SWS_RegisterHotkeys() {
    if (g_hotkeysRegistered || !g_hSwitcher) return;
    BOOL r1 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTTAB, MOD_ALT, VK_TAB);
    BOOL r2 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTTAB, MOD_ALT | MOD_SHIFT, VK_TAB);
    BOOL r3 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTCTRLTAB, MOD_ALT | MOD_CONTROL, VK_TAB);
    BOOL r4 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTCTRLTAB, MOD_ALT | MOD_SHIFT | MOD_CONTROL, VK_TAB);
    BOOL r5 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK, MOD_ALT, VK_OEM_3);
    BOOL r6 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_WINALTTAB, MOD_ALT | MOD_WIN, VK_TAB);
    BOOL r7 = RegisterHotKey(g_hSwitcher, SWS_HOTKEY_WINALTSHIFTTAB, MOD_ALT | MOD_SHIFT | MOD_WIN, VK_TAB);
    if (r1 && r2 && r3 && r4 && r5 && r6 && r7) {
        g_hotkeysRegistered = true;
        if (!g_hHotkeyMutex) {
            g_hHotkeyMutex = CreateMutexW(NULL, TRUE, L"Windhawk_SWS_HotkeyMutex");
        }
        KillTimer(g_hSwitcher, SWS_HOTKEY_RETRY_TIMER_ID);
        Wh_Log(L"All hotkeys registered successfully");
    } else {
        if (r1) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTTAB);
        if (r2) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTTAB);
        if (r3) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTCTRLTAB);
        if (r4) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTSHIFTCTRLTAB);
        if (r5) UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK);
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
    UnregisterHotKey(g_hSwitcher, SWS_HOTKEY_ALTBACKTICK);
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

static void LoadSettings() {
    LPCWSTR v;
    v = Wh_GetStringSetting(L"Style.theme");
    wcscpy_s(g_settings.theme, v ? v : L"none"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.colorScheme");
    wcscpy_s(g_settings.colorScheme, v ? v : L"system"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Appearance.Corners.cornerPreference");
    wcscpy_s(g_settings.cornerPreference, v ? v : L"round"); Wh_FreeStringSetting(v);
    g_settings.customCornerRadius = Wh_GetIntSetting(L"Appearance.Corners.customCornerRadius");
    if (g_settings.customCornerRadius < 0) g_settings.customCornerRadius = 0;
    if (g_settings.customCornerRadius > 32) g_settings.customCornerRadius = 32;
    g_settings.taskRoundedCorners = Wh_GetIntSetting(L"Appearance.Corners.taskRoundedCorners");
    g_settings.roundThumbnailCorners = Wh_GetIntSetting(L"Appearance.Corners.roundThumbnailCorners");
    g_settings.roundGroupIndicator = Wh_GetIntSetting(L"Appearance.Corners.roundGroupIndicator");
    g_settings.roundBadgeIconBackground = Wh_GetIntSetting(L"Appearance.Corners.roundBadgeIconBackground");
    v = Wh_GetStringSetting(L"Accessibility.scrollWheelBehavior");
    wcscpy_s(g_settings.scrollWheelBehavior, v ? v : L"never"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Appearance.Orientation.taskListOrientation");
    wcscpy_s(g_settings.taskListOrientation, v ? v : L"horizontal"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Appearance.Orientation.headerContentOrientation");
    wcscpy_s(g_settings.headerContentOrientation, v ? v : L"horizontal"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.headerContentOrientation, L"horizontal") != 0 &&
        wcscmp(g_settings.headerContentOrientation, L"vertical") != 0) {
        wcscpy_s(g_settings.headerContentOrientation, L"horizontal");
    }
    
    v = Wh_GetStringSetting(L"Appearance.Position.switcherPosition");
    wcscpy_s(g_settings.switcherPosition, v ? v : L"center"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.switcherPosition, L"topLeft") != 0 &&
        wcscmp(g_settings.switcherPosition, L"topCenter") != 0 &&
        wcscmp(g_settings.switcherPosition, L"topRight") != 0 &&
        wcscmp(g_settings.switcherPosition, L"centerLeft") != 0 &&
        wcscmp(g_settings.switcherPosition, L"center") != 0 &&
        wcscmp(g_settings.switcherPosition, L"centerRight") != 0 &&
        wcscmp(g_settings.switcherPosition, L"bottomLeft") != 0 &&
        wcscmp(g_settings.switcherPosition, L"bottomCenter") != 0 &&
        wcscmp(g_settings.switcherPosition, L"bottomRight") != 0) {
        wcscpy_s(g_settings.switcherPosition, L"center");
    }
    g_settings.switcherPositionMargin = Wh_GetIntSetting(L"Appearance.Position.switcherPositionMargin");
    if (g_settings.switcherPositionMargin < 0) g_settings.switcherPositionMargin = 0;
    v = Wh_GetStringSetting(L"Appearance.HeaderContent.iconSize");
    wcscpy_s(g_settings.iconSize, v ? v : L"small"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.iconSize, L"small") != 0 &&
        wcscmp(g_settings.iconSize, L"medium") != 0 &&
        wcscmp(g_settings.iconSize, L"large") != 0 &&
        wcscmp(g_settings.iconSize, L"xlarge") != 0) {
        wcscpy_s(g_settings.iconSize, L"small");
    }
    v = Wh_GetStringSetting(L"Appearance.Thumbnails.thumbnailPosition");
    wcscpy_s(g_settings.thumbnailPosition, v ? v : L"bottom"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.thumbnailPosition, L"bottom") != 0 &&
        wcscmp(g_settings.thumbnailPosition, L"top") != 0 &&
        wcscmp(g_settings.thumbnailPosition, L"left") != 0 &&
        wcscmp(g_settings.thumbnailPosition, L"right") != 0) {
        wcscpy_s(g_settings.thumbnailPosition, L"bottom");
    }
    v = Wh_GetStringSetting(L"Appearance.Thumbnails.thumbnailAlignment");
    wcscpy_s(g_settings.thumbnailAlignment, v ? v : L"left"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.thumbnailAlignment, L"left") != 0 &&
        wcscmp(g_settings.thumbnailAlignment, L"centered") != 0 &&
        wcscmp(g_settings.thumbnailAlignment, L"right") != 0) {
        wcscpy_s(g_settings.thumbnailAlignment, L"left");
    }
    v = Wh_GetStringSetting(L"Accessibility.backwardShortcut");
    if (v) {
        wcscpy_s(g_settings.backwardShortcut, v);
        Wh_FreeStringSetting(v);
    } else {
        // Backward compatibility with previous boolean setting.
        wcscpy_s(g_settings.backwardShortcut,
                 Wh_GetIntSetting(L"enableAltShiftForBackward") ? L"altShift" : L"altShiftTab");
    }
    if (wcscmp(g_settings.backwardShortcut, L"altShiftTab") != 0 &&
        wcscmp(g_settings.backwardShortcut, L"altShift") != 0 &&
        wcscmp(g_settings.backwardShortcut, L"altBacktick") != 0) {
        wcscpy_s(g_settings.backwardShortcut, L"altShiftTab");
    }

    v = Wh_GetStringSetting(L"Accessibility.virtualDesktopBehavior");
    wcscpy_s(g_settings.virtualDesktopBehavior, v ? v : L"currentOnly");
    Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.virtualDesktopBehavior, L"currentOnly") != 0 &&
        wcscmp(g_settings.virtualDesktopBehavior, L"allDesktops") != 0) {
        wcscpy_s(g_settings.virtualDesktopBehavior, L"currentOnly");
    }

    g_settings.rowHeight = Wh_GetIntSetting(L"Dimensions.rowHeight");
    if (g_settings.rowHeight <= 0) g_settings.rowHeight = 230;
    g_settings.rowWidth = Wh_GetIntSetting(L"Dimensions.rowWidth");
    if (g_settings.rowWidth < 0) g_settings.rowWidth = 0;
    g_settings.stretchThumbnailsToTaskWidth = Wh_GetIntSetting(L"Dimensions.stretchThumbnailsToTaskWidth");
    g_settings.autoFitTasks = Wh_GetIntSetting(L"Dimensions.autoFitTasks");
    g_settings.showThumbnails = Wh_GetIntSetting(L"Appearance.Thumbnails.showThumbnails");
    g_settings.showHoverBorder = Wh_GetIntSetting(L"Appearance.Thumbnails.showHoverBorder");
    g_settings.showThumbnailShadow = Wh_GetIntSetting(L"Appearance.Thumbnails.showThumbnailShadow");
    g_settings.showTitle = Wh_GetIntSetting(L"Appearance.HeaderContent.showTitle");
    g_settings.showIcon = Wh_GetIntSetting(L"Appearance.HeaderContent.showIcon");
    if (!g_settings.showThumbnails && !g_settings.showTitle && !g_settings.showIcon) {
        g_settings.showTitle = true;
    }
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
    v = Wh_GetStringSetting(L"Grouping.showTitles");
    wcscpy_s(g_settings.showTitles, v ? v : L"windowTitle"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.showTitles, L"windowTitle") != 0 &&
        wcscmp(g_settings.showTitles, L"appName") != 0 &&
        wcscmp(g_settings.showTitles, L"appNameWindowTitle") != 0) {
        wcscpy_s(g_settings.showTitles, L"windowTitle");
    }
    g_settings.centerTaskContent = Wh_GetIntSetting(L"Appearance.HeaderContent.centerTaskContent");

    // Badge layout settings
    g_settings.enableBadgeLayout = Wh_GetIntSetting(L"Appearance.BadgeLayout.enableBadgeLayout");
    v = Wh_GetStringSetting(L"Appearance.BadgeLayout.badgeIconPosition");
    wcscpy_s(g_settings.badgeIconPosition, v ? v : L"bottomCenter"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.badgeIconPosition, L"topLeft") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"topCenter") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"topRight") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"centerLeft") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"center") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"centerRight") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"bottomLeft") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"bottomCenter") != 0 &&
        wcscmp(g_settings.badgeIconPosition, L"bottomRight") != 0) {
        wcscpy_s(g_settings.badgeIconPosition, L"bottomCenter");
    }
    v = Wh_GetStringSetting(L"Appearance.BadgeLayout.badgeTitlePosition");
    wcscpy_s(g_settings.badgeTitlePosition, v ? v : L"bottom"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.badgeTitlePosition, L"top") != 0 &&
        wcscmp(g_settings.badgeTitlePosition, L"bottom") != 0) {
        wcscpy_s(g_settings.badgeTitlePosition, L"bottom");
    }
    g_settings.showBadgeIconBackground = Wh_GetIntSetting(L"Appearance.BadgeLayout.showBadgeIconBackground");
    g_settings.showBadgeIconBackgroundShadow = Wh_GetIntSetting(L"Appearance.BadgeLayout.showBadgeIconBackgroundShadow");
    g_settings.badgeIconPadding = Wh_GetIntSetting(L"Appearance.BadgeLayout.badgeIconPadding");
    g_settings.badgeIconOffsetX = Wh_GetIntSetting(L"Appearance.BadgeLayout.badgeIconOffsetX");
    g_settings.badgeIconOffsetY = Wh_GetIntSetting(L"Appearance.BadgeLayout.badgeIconOffsetY");

    // Grouped indicator
    g_settings.showGroupIndicator = Wh_GetIntSetting(L"Grouping.showGroupIndicator");
    g_settings.showGroupIndicatorShadow = Wh_GetIntSetting(L"Grouping.showGroupIndicatorShadow");
    v = Wh_GetStringSetting(L"Grouping.groupCloseBehavior");
    wcscpy_s(g_settings.groupCloseBehavior, v ? v : L"closeRecent"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.groupCloseBehavior, L"closeAll") != 0 &&
        wcscmp(g_settings.groupCloseBehavior, L"closeRecent") != 0) {
        wcscpy_s(g_settings.groupCloseBehavior, L"closeRecent");
    }

    // Global theme settings (apply to both light and dark)
    v = Wh_GetStringSetting(L"Style.highlightStyle");
    wcscpy_s(g_settings.highlightStyle, v ? v : L"border"); Wh_FreeStringSetting(v);
    if (wcscmp(g_settings.highlightStyle, L"border") != 0 &&
        wcscmp(g_settings.highlightStyle, L"fillAndBorder") != 0 &&
        wcscmp(g_settings.highlightStyle, L"fillOnly") != 0) {
        wcscpy_s(g_settings.highlightStyle, L"border");
    }
    g_settings.opacity = Wh_GetIntSetting(L"Style.opacity");
    if (g_settings.opacity < 0) g_settings.opacity = 0;
    if (g_settings.opacity > 100) g_settings.opacity = 100;

    // Dark Mode color settings
    v = Wh_GetStringSetting(L"Style.DarkMode.borderColorMode");
    wcscpy_s(g_settings.borderColorModeDark, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.DarkMode.highlightFillColorMode");
    wcscpy_s(g_settings.highlightFillColorModeDark, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.DarkMode.bgColorMode");
    wcscpy_s(g_settings.bgColorModeDark, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.DarkMode.customBorderColor");
    wcscpy_s(g_settings.customBorderColorDark, v ? v : L"#FFFFFF"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.DarkMode.customHighlightFillColor");
    wcscpy_s(g_settings.customHighlightFillColorDark, v ? v : L"#FFFFFF"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.DarkMode.customBgColor");
    wcscpy_s(g_settings.customBgColorDark, v ? v : L"#202020"); Wh_FreeStringSetting(v);
    
    v = Wh_GetStringSetting(L"Style.DarkMode.iconBgColorMode");
    wcscpy_s(g_settings.iconBgColorModeDark, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.DarkMode.customIconBgColor");
    wcscpy_s(g_settings.customIconBgColorDark, v ? v : L"#000000"); Wh_FreeStringSetting(v);
    g_settings.iconBgOpacityDark = Wh_GetIntSetting(L"Style.DarkMode.iconBgOpacity");
    if (g_settings.iconBgOpacityDark < 0) g_settings.iconBgOpacityDark = 0;
    if (g_settings.iconBgOpacityDark > 100) g_settings.iconBgOpacityDark = 100;

    v = Wh_GetStringSetting(L"Style.DarkMode.indicatorBgColorMode");
    wcscpy_s(g_settings.indicatorBgColorModeDark, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.DarkMode.customIndicatorBgColor");
    wcscpy_s(g_settings.customIndicatorBgColorDark, v ? v : L"#333333"); Wh_FreeStringSetting(v);
    g_settings.indicatorBgOpacityDark = Wh_GetIntSetting(L"Style.DarkMode.indicatorBgOpacity");
    if (g_settings.indicatorBgOpacityDark < 0) g_settings.indicatorBgOpacityDark = 0;
    if (g_settings.indicatorBgOpacityDark > 100) g_settings.indicatorBgOpacityDark = 100;
    
    v = Wh_GetStringSetting(L"Style.DarkMode.indicatorTextColorMode");
    wcscpy_s(g_settings.indicatorTextColorModeDark, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.DarkMode.customIndicatorTextColor");
    wcscpy_s(g_settings.customIndicatorTextColorDark, v ? v : L"#FFFFFF"); Wh_FreeStringSetting(v);

    // Light Mode color settings
    v = Wh_GetStringSetting(L"Style.LightMode.borderColorMode");
    wcscpy_s(g_settings.borderColorModeLight, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.LightMode.highlightFillColorMode");
    wcscpy_s(g_settings.highlightFillColorModeLight, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.LightMode.bgColorMode");
    wcscpy_s(g_settings.bgColorModeLight, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.LightMode.customBorderColor");
    wcscpy_s(g_settings.customBorderColorLight, v ? v : L"#000000"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.LightMode.customHighlightFillColor");
    wcscpy_s(g_settings.customHighlightFillColorLight, v ? v : L"#000000"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.LightMode.customBgColor");
    wcscpy_s(g_settings.customBgColorLight, v ? v : L"#F3F3F3"); Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"Style.LightMode.iconBgColorMode");
    wcscpy_s(g_settings.iconBgColorModeLight, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.LightMode.customIconBgColor");
    wcscpy_s(g_settings.customIconBgColorLight, v ? v : L"#FFFFFF"); Wh_FreeStringSetting(v);
    g_settings.iconBgOpacityLight = Wh_GetIntSetting(L"Style.LightMode.iconBgOpacity");
    if (g_settings.iconBgOpacityLight < 0) g_settings.iconBgOpacityLight = 0;
    if (g_settings.iconBgOpacityLight > 100) g_settings.iconBgOpacityLight = 100;

    v = Wh_GetStringSetting(L"Style.LightMode.indicatorBgColorMode");
    wcscpy_s(g_settings.indicatorBgColorModeLight, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.LightMode.customIndicatorBgColor");
    wcscpy_s(g_settings.customIndicatorBgColorLight, v ? v : L"#EAEAEA"); Wh_FreeStringSetting(v);
    g_settings.indicatorBgOpacityLight = Wh_GetIntSetting(L"Style.LightMode.indicatorBgOpacity");
    if (g_settings.indicatorBgOpacityLight < 0) g_settings.indicatorBgOpacityLight = 0;
    if (g_settings.indicatorBgOpacityLight > 100) g_settings.indicatorBgOpacityLight = 100;
    
    v = Wh_GetStringSetting(L"Style.LightMode.indicatorTextColorMode");
    wcscpy_s(g_settings.indicatorTextColorModeLight, v ? v : L"default"); Wh_FreeStringSetting(v);
    v = Wh_GetStringSetting(L"Style.LightMode.customIndicatorTextColor");
    wcscpy_s(g_settings.customIndicatorTextColorLight, v ? v : L"#000000"); Wh_FreeStringSetting(v);

    v = Wh_GetStringSetting(L"Appearance.Font.fontFamily");
    wcscpy_s(g_settings.fontFamily, v ? v : L"Segoe UI"); Wh_FreeStringSetting(v);
    
    g_settings.fontSize = Wh_GetIntSetting(L"Appearance.Font.fontSize");
    if (g_settings.fontSize <= 0) g_settings.fontSize = 9;

    v = Wh_GetStringSetting(L"Appearance.Font.fontStyle");
    wcscpy_s(g_settings.fontStyle, v ? v : L"regular"); Wh_FreeStringSetting(v);

    g_settings.applyToGroupIndicator = Wh_GetIntSetting(L"Appearance.Font.applyToGroupIndicator");

    v = Wh_GetStringSetting(L"Accessibility.switcherDisplayBehavior");
    wcscpy_s(g_settings.switcherDisplayBehavior, v ? v : L"cursorMonitor"); Wh_FreeStringSetting(v);

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

    g_hFont = CreateScaledFont(96);

    SWS_RegisterHotkeys();

    Wh_Log(L"Simple Window Switcher initialized, entering message loop");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SWS_UnregisterHotkeys();
    if (g_isVisible) HideSwitcher();
    UnregisterThumbnails();
    g_windows.clear();
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
    CoUninitialize();
    Wh_Log(L"SwitcherThread exiting");
    return 0;
}

// Tool Mod callbacks

bool WhTool_ModInit() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModInit");
    g_hSwitcherThread = CreateThread(NULL, 0, SwitcherThread, NULL, 0, &g_dwSwitcherThreadId);
    return g_hSwitcherThread != NULL;
}

void WhTool_ModUninit() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModUninit");
    if (g_dwSwitcherThreadId)
        PostThreadMessage(g_dwSwitcherThreadId, WM_QUIT, 0, 0);
    if (g_hSwitcherThread) {
        WaitForSingleObject(g_hSwitcherThread, INFINITE);
        CloseHandle(g_hSwitcherThread);
        g_hSwitcherThread = NULL;
        g_dwSwitcherThreadId = 0;
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Simple Window Switcher: WhTool_ModSettingsChanged");
    if (g_hSwitcher) {
        LoadSettings();
        if (g_isVisible) {
            ShowSwitcher(g_isSticky);
        }
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

    // --- explorer.exe path: hook RegisterHotKey only ---
    if (_wcsicmp(exeName, L"explorer.exe") == 0) {
        g_isExplorer = true;
        Wh_Log(L"SWS: Loaded into explorer.exe, hooking RegisterHotKey");

        if (!g_WM_SWS_GET_UWP_ICON) {
            g_WM_SWS_GET_UWP_ICON = RegisterWindowMessageW(L"Windhawk_SWS_GetUwpIcon");
        }
        g_explorerIpcThread = CreateThread(NULL, 0, ExplorerIpcThread, NULL, 0, NULL);

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

        HWND hIpc = FindWindowW(L"WindhawkSWS_IpcWindow", NULL);
        if (hIpc) {
            PostMessageW(hIpc, WM_QUIT, 0, 0);
        }
        if (g_explorerIpcThread) {
            WaitForSingleObject(g_explorerIpcThread, 5000);
            CloseHandle(g_explorerIpcThread);
            g_explorerIpcThread = NULL;
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
            WaitForSingleObject(g_restartExplorerPromptThread, INFINITE);
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
