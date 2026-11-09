// ==WindhawkMod==
// @id              macos-style-dock
// @name            MacOS Style Dock
// @description     Turns the Windows 11 taskbar into a macOS-style dock with magnifying icons
// @version         1.1
// @author          Zach Vaughn
// @github          https://github.com/zachvdev
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -lgdi32 -lshell32 -lshcore -ladvapi32 -ldwmapi -lgdiplus -ld2d1
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MacOS Style Dock

Turns the Windows 11 taskbar into a macOS-style dock: frosted glass, icons
that magnify under the pointer, running dots, app name labels, File Explorer
first and the Recycle Bin last, and hot corners for Task View, Start and the
system tray.

It restyles the real taskbar instead of replacing it, so everything you're
used to keeps working: pinned apps, right-click menus, dropping files onto
apps, and dragging apps to rearrange them.

![The dock](https://i.imgur.com/E1NqVoz.png)

![The dock in action](https://i.imgur.com/eJXsof7.gif)

## Getting started

1. Turn off any other taskbar mods.
2. In Windows' taskbar settings, keep the taskbar centered, at the bottom and
   at its normal size. Hide Task View, unless you want it in the dock.
3. Turn on this mod. If something looks off at first, restart your computer.

Turning the mod off puts the taskbar back to normal.

## The dock

- **Frosted glass.** Light, dark, following Windows' light or dark mode, or a
  color of your choice, with adjustable opacity and blur.
- **Apple's corners.** Not a plain rounded rectangle, but the smooth
  "squircle" corner Apple uses, built from PaintCode's measurements of it.
- **A Mac edge.** A thin light line along the top and bottom, a faint dark
  line along the sides, and a soft glow inside that fades out with no hard
  edge. The glow can come from the sides instead, or from all four. You can
  also pick a plain one-color edge, or none.
- **Running dots.** A small dot under each open app, the same size at any
  dock size, as on a Mac. Dots and dividers turn dark on a light dock and
  light on a dark one. "Running indicators" has two other styles: Hybrid
  dots, Windows' own indicators made round, so the active app's dot takes
  your accent color and an app that needs attention turns orange; or Windows
  11, Windows' own. Progress bars, like a download's, show under the icon
  either way.

## The icons

- **Magnification.** Icons grow as the pointer passes over them, pushing
  their neighbors aside, and the dock stretches to fit. As on a Mac, the end
  farther from the pointer stays put.
- **Sharp at any size.** App icons are loaded at their fully magnified size,
  so they don't blur as they grow.
- **Squircle icons** (on by default). Every app's icon becomes a macOS-style
  tile, so the whole dock matches:
  - Icons that are already square are trimmed to the squircle shape.
  - Anything else, like a round logo, is placed on a tile: light, or dark if
    the logo itself is light. "Squircle color" can make every tile light,
    dark or a color of your choice.
  - Each tile gets a soft shadow.

  Tiles appear a few seconds after the mod starts, once the taskbar has
  finished loading its icons.

## File Explorer and the Recycle Bin

- **File Explorer** is moved to the front of your apps for real, so it stays
  there, and can't be dragged out of that spot. If it's pinned, this changes
  your pinned apps' order, which stays that way after the mod is turned off.
  To leave your order alone, turn off "Keep File Explorer first".
- **The Recycle Bin** sits at the far end, past a divider. Its icon shows
  when there's something in it.
  - Click it to open it.
  - Drop files on it to delete them. Windows asks first if they're too big
    to recycle.
  - Right-click it to open or empty it.

Both keep Windows' icons and names, unless you give them your own icons
(below). Your own icons also name them Finder and Trash.

## Custom icons

Give File Explorer and the Recycle Bin your own icons, from image files on
your PC. Leave a setting blank to keep Windows' icon.

1. **Pick an image.** PNG with transparency works best: 256x256 for File
   Explorer and 240x256 for the Recycle Bin, with 25 pixels of empty space
   around the artwork, as macOS's icons have for their shadow.
2. **Put it somewhere it'll stay**, like `C:\Icons`. The mod loads it from
   there.
3. **Copy its path.** Right-click the file and choose "Copy as path".
4. **Paste it** under "Custom icons": into "File Explorer icon", or "Recycle
   Bin icon, empty" (and, if you like, "full").

A path looks like `C:\Icons\finder.png`. Quotes around it are fine, and so
are variables, like `%USERPROFILE%\Pictures\finder.png`. If there's no image
there, Windows' icon shows. After editing an image, change any setting to
reload it.

If your icon looks too big or small, adjust it:

- **Margin:** the empty space around the artwork in your image, in pixels of
  a 256-pixel image. Space past the icon's spot spills out, like a shadow.
- **Size:** the artwork's size next to other icons, in percent.

## Using the dock

- **App names on hover**, in macOS-style labels. Windows' window previews
  open when you click an app with more than one window. Turn on "Window
  previews" to also see them on hover, just above the label.
- **Rearranging.** Drag apps anywhere between File Explorer and the Recycle
  Bin. While you drag, icons stop magnifying, and the one you're moving fades
  slightly.
- **Right-click menus** (an app's recent files, pinned items and "Close
  window") open right above the icon you right-clicked.
- **Magnified icons** rise over windows, so the dock never needs extra room
  above it.

## Hot corners

The Start button and the system tray are hidden from the dock, and the bottom
corners of the screen take their place. Push the pointer into a corner to use
it. Corners don't react while a mouse button is held or a full screen app is
running.

- **Bottom left** opens Task View. It can open Start instead, or be turned
  off. The Windows key always opens Start.
- **Bottom right** slides the system tray in. It stays while the pointer is
  over it or a menu it opened, and slides away about half a second after.
  Turned off, the tray is either always shown or always hidden.

The tray's icons and clock get a soft shadow, so they're readable over any
background.

![The system tray's hot corner](https://i.imgur.com/qLN8ETY.gif)

## Auto-hide

Only the dock hides; the hot corners keep working.

- **Hybrid** (the default): the dock slides away while a window is maximized.
  Windows always have the whole screen, so the dock floats over their bottom
  edge and nothing moves when it shows or hides.
- **On:** the dock is always hidden.
- **Off:** the dock is always shown, and windows stop at its top.

To bring a hidden dock back, move the pointer to the very bottom of the
screen, right below where the dock sits. It slides away again about half a
second after the pointer leaves it and any menu it opened.

## Settings

Every setting lists its default, since Windhawk has no reset button. Changes
apply right away.

- **Dock:** size (icons from 19 to 77 pixels; the default is 51), color,
  opacity, blur, edge style and auto-hide.
- **Magnification:** how much icons grow (up to double), how far from the
  pointer, and how quickly.
- **Spacing:** padding around the icons and the gaps between them. These
  scale with the dock's size, so it keeps its shape.
- **Dock items:** squircle icons and their tile color, keeping File Explorer
  first, the Recycle Bin, app names, running indicators, window previews, a
  divider after File Explorer, and high resolution icons.
- **Custom icons:** your own File Explorer and Recycle Bin icons, with their
  margin and size.
- **Hot corners:** what each bottom corner does.
- **Advanced adjustments:** the edge glow's direction (top and bottom, sides,
  or all around), size, opacity and color, and the app name labels' color.

## Troubleshooting

If something glitches or looks wrong:

1. Make sure no other taskbar mods are turned on in Windhawk.
2. Restart your computer, with this mod still on.
3. If it's the app icons, try turning off "Squircle icons", or "High
   resolution icons" if apps' custom icons don't show up.

## Compatibility

- Windows 11 on x64 or ARM64, with the taskbar at the bottom. "Small taskbar
  buttons" isn't supported. ARM64 support follows m417z's mods and hasn't been
  tested on an ARM64 PC yet.
- With several monitors, each taskbar becomes its own dock.
- Use the dock's own Auto-hide, not Windows' "Automatically hide the
  taskbar". The mod uses Windows' setting behind the scenes and puts it back
  when it's turned off. If that ever fails (for example, Windhawk was
  uninstalled while Explorer wasn't running), turn it off yourself in
  Windows' taskbar settings.
- High resolution icons need the October 2024 update (KB5044384) or later.
- Don't combine with other taskbar mods, especially "Taskbar height and icon
  size", "Taskbar Dock Animation", "Taskbar Dock Animation Plus", "Mac Island
  & Dock", "Taskbar Multirow", "Start button always on the left" or "Disable
  Taskbar Thumbnails".
- Smart App Control blocks this mod, as it does all Windhawk mods.

## Credits

The taskbar height code is based on m417z's "Taskbar height and icon size".
Icon dragging and right-click menu placement follow his "Taskbar Multirow",
keeping the taskbar shown follows his "Taskbar auto-hide when maximized",
turning off hover previews follows his "Disable Taskbar Thumbnails", and
placing them follows his "Taskbar on top" (all GPL-3.0). Hybrid dots follow
notdominicc's way of reshaping Windows' own indicators. The squircle corners
use PaintCode's measurements of Apple's curve.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dock:
  - size: 70
    $name: Size (0-100)
    $description: Size of the icons and the dock. Default is 70.
  - color: light
    $name: Color
    $description: Default is Light.
    $options:
    - auto: System
    - dark: Dark
    - light: Light
    - custom: Custom
  - customColor: "333333"
    $name: Custom color
    $description: Used when Color is Custom, as 6 hex digits. Default is 333333.
  - opacity: 15
    $name: Opacity (0-100)
    $description: Default is 15.
  - blur: 10
    $name: Blur (0-100)
    $description: Default is 10.
  - border: mac
    $name: Border
    $description: Default is macOS.
    $options:
    - mac: macOS
    - standard: Standard
    - none: None
  - autoHide: hybrid
    $name: Auto-hide
    $description: >-
      On: hidden until the pointer reaches the bottom of the screen below it.
      Hybrid: hidden only while a window is maximized, and windows always get
      the whole screen. Off: windows stop at the dock. Default is Hybrid.
    $options:
    - never: "Off"
    - always: "On"
    - hybrid: Hybrid
  $name: Dock
- magnification:
  - amount: 70
    $name: Amount (0-100)
    $description: How much icons grow. Default is 70.
  - range: 55
    $name: Range (0-100)
    $description: How far from the pointer icons grow. Default is 55.
  - smoothing: 100
    $name: Animation (ms)
    $description: Default is 100.
  $name: Magnification
- spacing:
  - horizontal: 13
    $name: Horizontal padding
    $description: Between the outer icons and the ends of the dock. Default is 13.
  - vertical: 12
    $name: Vertical padding
    $description: Between the icons and the top and bottom of the dock. Default is 12.
  - buttons: 12
    $name: Icon spacing
    $description: Between neighboring icons. Default is 12.
  $name: Spacing
  $description: In pixels at Size 60, scaling with the dock.
- items:
  - squircleIcons: true
    $name: Squircle icons
    $description: >-
      App icons redrawn as macOS-style tiles: square icons fill a squircle, other
      shapes sit on a tile of the Squircle color. May take a few seconds to
      enable. Default is on.
  - squircleColor: auto
    $name: Squircle color
    $description: >-
      The tile behind icons that aren't square. Auto: light, or dark for light
      icons. Default is Auto.
    $options:
    - auto: Auto
    - light: Light
    - dark: Dark
    - custom: Custom
  - squircleCustomColor: FFFFFF
    $name: Custom squircle color
    $description: Used when Squircle color is Custom, as 6 hex digits. Default is FFFFFF.
  - explorerFirst: true
    $name: Keep File Explorer first
    $description: >-
      Moves it to the front of your apps and keeps it there. If it's pinned, this
      changes your pinned apps' order. Turn off to leave your order alone. Default
      is on.
  - recycleBin: true
    $name: Recycle Bin
    $description: At the right end of the dock. Default is on.
  - tooltips: true
    $name: Show tooltips
    $description: App names above icons on hover. Default is on.
  - indicators: dots
    $name: Running indicators
    $description: >-
      Under each open app. Dots: small round dots, as on a Mac. Hybrid dots:
      Windows' own indicators made round, still showing the active app and apps
      that need attention. Windows 11: Windows' own. Progress bars, like a
      download's, show under the icon either way. Default is Dots.
    $options:
    - dots: Dots
    - hybrid: Hybrid dots
    - windows: Windows 11
  - previews: false
    $name: Window previews
    $description: >-
      Windows' previews of an app's windows on hover, just above the app name.
      Off, previews only open on click. Default is off.
  - appsDivider: false
    $name: Divider after File Explorer
    $description: Default is off.
  - highResIcons: true
    $name: High resolution icons
    $description: Keeps icons sharp when magnified. Turn off if apps' custom icons don't show up. Default is on.
  $name: Dock items
- customIcons:
  - finderPath: ""
    $name: File Explorer icon
    $description: >-
      Full path to an image file, like C:\Icons\finder.png. Right-click the file
      and choose "Copy as path" to get it. Also names it Finder. Leave blank for
      Windows' icon.
  - finderMargin: 25
    $name: File Explorer icon margin
    $description: >-
      The empty space around the artwork in your image, in pixels of a 256-pixel
      image. Default is 25.
  - finderSize: 100
    $name: File Explorer icon size
    $description: The artwork's size next to other icons, in percent. Default is 100.
  - trashEmptyPath: ""
    $name: Recycle Bin icon, empty
    $description: >-
      Full path to an image file, like C:\Icons\trash-empty.png. Also names it
      Trash. Leave blank for Windows' icon.
  - trashFullPath: ""
    $name: Recycle Bin icon, full
    $description: >-
      Shown while it has something in it, like C:\Icons\trash-full.png. Leave
      blank to use the empty icon.
  - trashMargin: 25
    $name: Recycle Bin icon margin
    $description: >-
      The empty space around the artwork in your image, in pixels of a 256-pixel
      image. Default is 25.
  - trashSize: 100
    $name: Recycle Bin icon size
    $description: The artwork's size next to other icons, in percent. Default is 100.
  $name: Custom icons
  $description: >-
    Your own icons for File Explorer and the Recycle Bin, from image files on
    your PC. PNGs with transparency work best: 256x256 for File Explorer and
    240x256 for the Recycle Bin, with 25 pixels of empty space around the
    artwork. Keep the files where they are; the mod loads them from there.
- hotCorners:
  - bottomLeft: taskView
    $name: Bottom left
    $description: The Windows key always opens Start. Default is Task View.
    $options:
    - start: Start menu
    - taskView: Task View
    - off: "Off"
  - bottomRight: tray
    $name: Bottom right
    $description: The system tray's clock and icons. Default is System tray.
    $options:
    - tray: System tray
    - hidden: Off, with the tray hidden
    - visible: Off, with the tray visible
  $name: Hot corners
- advanced:
  - glowDirection: horizontal
    $name: Glow direction
    $description: >-
      Horizontal: light top and bottom edges, glowing inward, and dark sides.
      Vertical: the other way around. Both: light edges all around, glowing
      inward. Default is Horizontal.
    $options:
    - horizontal: Horizontal
    - vertical: Vertical
    - both: Both
  - glowSize: 6
    $name: Glow size
    $description: In pixels at Size 60, scaling with the dock. Default is 6.
  - glowOpacity: 60
    $name: Glow opacity
    $description: Relative to the dock's Opacity, so 100 matches it. Default is 60.
  - glowColor: ""
    $name: Glow color
    $description: 6 hex digits, or blank for a lighter shade of the dock's color. Default is blank.
  - tooltipColor: BFBFBF
    $name: Tooltip color
    $description: Behind app names, as 6 hex digits. The text turns light on dark colors. Default is BFBFBF.
  $name: Advanced adjustments
  $description: The inner glow of the macOS border, and the app name tooltips.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellapi.h>
#include <shellscalingapi.h>
#include <sddl.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shlobj.h>
#include <wincodec.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cwctype>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

namespace wadt = winrt::Windows::ApplicationModel::DataTransfer;
namespace wf = winrt::Windows::Foundation;
namespace wss = winrt::Windows::Storage::Streams;
namespace wg = winrt::Windows::Graphics;
namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;

using winrt::Windows::UI::Color;
using wux::Visibility;

// =============================================================================
// SETTINGS
// =============================================================================

enum class Theme { System, Dark, Light, Custom };
enum class Border { Mac, Standard, None };
enum class LeftCorner { Start, TaskView, Off };
enum class Show { Hidden, Visible, HotCorner };  // the system tray
enum class AutoHide { Off, On, Hybrid };
enum class GlowEdges { TopBottom, Sides, All };
enum class TileColor { Auto, Light, Dark, Custom };  // behind squircle icons' shapes
enum class Indicators { Dots, HybridDots, Windows };  // running apps

// The user's own Finder or Trash icon. None while `path` is empty.
struct CustomIcon {
    std::wstring path, fullPath;  // image files; fullPath for a full Recycle Bin
    float size = 1;               // the artwork, of the icon size
    float scale = 1;              // the image, of the icon size, margin included
};

struct Settings {
    int size;
    Theme theme;
    uint8_t customColor[3];
    int opacity;
    int blur;
    Border border;
    AutoHide autoHide;
    GlowEdges glowEdges;
    float glowSize;
    float glowOpacity;
    bool glowCustom;
    uint8_t glowColor[3];
    uint8_t tooltipColor[3];
    int paddingH, paddingV, spacing;
    float scale;  // magnification, 1.0-2.0
    int range;
    int animationMs;
    CustomIcon customFinder, customTrash;
    bool squircleIcons;
    TileColor tileColor;
    uint8_t tileCustomColor[3];
    bool explorerFirst;
    bool recycleBin;
    bool tooltips;
    Indicators indicators;
    bool previews;
    bool appsDivider;
    bool highResIcons;
    LeftCorner leftCorner;
    Show tray;
} g_settings;

// Reads 6 hex digits ("333333"), ignoring '#' and spaces.
bool ParseHexColor(std::wstring_view text, uint8_t rgb[3]) {
    std::wstring digits;
    for (wchar_t c : text) {
        if (c == L'#' || iswspace(c)) continue;
        if (!iswxdigit(c)) return false;
        digits.push_back(c);
    }
    if (digits.size() != 6) return false;
    for (int i = 0; i < 3; i++) {
        rgb[i] = (uint8_t)std::wcstoul(digits.substr(i * 2, 2).c_str(), nullptr, 16);
    }
    return true;
}

// The index of a string setting's value among `options`, or `fallback`.
int ChoiceSetting(PCWSTR name, std::initializer_list<PCWSTR> options, int fallback) {
    auto value = WindhawkUtils::StringSetting::make(name);
    int i = 0;
    for (PCWSTR option : options) {
        if (!wcscmp(value, option)) return i;
        i++;
    }
    return fallback;
}

// A file's path from a string setting, as typed or copied: surrounding quotes
// and %VARIABLES% are fine. Empty if there's no file there.
std::wstring PathSetting(PCWSTR name) {
    auto value = WindhawkUtils::StringSetting::make(name);
    std::wstring_view text = value ? (PCWSTR)value : L"";
    auto trimmed = [](wchar_t c) { return iswspace(c) || c == L'"'; };
    while (!text.empty() && trimmed(text.front())) text.remove_prefix(1);
    while (!text.empty() && trimmed(text.back())) text.remove_suffix(1);
    if (text.empty()) return {};
    std::wstring raw(text);
    std::wstring path(ExpandEnvironmentStringsW(raw.c_str(), nullptr, 0), L'\0');
    DWORD length = ExpandEnvironmentStringsW(raw.c_str(), path.data(), (DWORD)path.size());
    path = length && length <= path.size() ? path.substr(0, length - 1) : raw;
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        Wh_Log(L"No file at %s", path.c_str());
        return {};
    }
    return path;
}

// A custom icon, drawn so its artwork is `size` percent of other icons, with
// `margin` pixels of a 256-pixel image around it.
CustomIcon CustomIconSetting(PCWSTR path, PCWSTR fullPath, PCWSTR margin, PCWSTR size) {
    CustomIcon icon;
    icon.path = PathSetting(path);
    if (fullPath) icon.fullPath = PathSetting(fullPath);
    icon.size = std::clamp(Wh_GetIntSetting(size), 10, 200) / 100.0f;
    icon.scale = icon.size * 256 / (256 - 2.0f * std::clamp(Wh_GetIntSetting(margin), 0, 100));
    return icon;
}

void ComputeSizes();

void LoadSettings() {
    auto& s = g_settings;
    auto percent = [](PCWSTR name) { return std::clamp(Wh_GetIntSetting(name), 0, 100); };
    auto hex = [](PCWSTR name, uint8_t rgb[3]) {
        auto value = WindhawkUtils::StringSetting::make(name);
        PCWSTR text = value ? (PCWSTR)value : L"";
        if (ParseHexColor(text, rgb)) return true;
        if (*text) Wh_Log(L"%s \"%s\" isn't 6 hex digits", name, text);
        return false;
    };

    s.size = percent(L"dock.size");
    s.theme = (Theme)ChoiceSetting(L"dock.color", {L"auto", L"dark", L"light", L"custom"}, 0);
    if (s.theme == Theme::Custom && !hex(L"dock.customColor", s.customColor)) {
        s.theme = Theme::System;
    }
    s.opacity = percent(L"dock.opacity");
    s.blur = percent(L"dock.blur");
    s.border = (Border)ChoiceSetting(L"dock.border", {L"mac", L"standard", L"none"}, 0);
    s.autoHide = (AutoHide)ChoiceSetting(L"dock.autoHide", {L"never", L"always", L"hybrid"}, 0);

    s.paddingH = percent(L"spacing.horizontal");
    s.paddingV = percent(L"spacing.vertical");
    s.spacing = percent(L"spacing.buttons");

    s.scale = 1.0f + percent(L"magnification.amount") / 100.0f;
    s.range = percent(L"magnification.range");
    s.animationMs = std::clamp(Wh_GetIntSetting(L"magnification.smoothing"), 0, 1000);

    s.customFinder = CustomIconSetting(L"customIcons.finderPath", nullptr,
                                       L"customIcons.finderMargin", L"customIcons.finderSize");
    s.customTrash = CustomIconSetting(L"customIcons.trashEmptyPath", L"customIcons.trashFullPath",
                                      L"customIcons.trashMargin", L"customIcons.trashSize");
    s.squircleIcons = Wh_GetIntSetting(L"items.squircleIcons");
    s.tileColor = (TileColor)ChoiceSetting(L"items.squircleColor",
                                           {L"auto", L"light", L"dark", L"custom"}, 0);
    if (s.tileColor == TileColor::Custom && !hex(L"items.squircleCustomColor", s.tileCustomColor)) {
        s.tileColor = TileColor::Auto;
    }
    s.explorerFirst = Wh_GetIntSetting(L"items.explorerFirst");
    s.recycleBin = Wh_GetIntSetting(L"items.recycleBin");
    s.tooltips = Wh_GetIntSetting(L"items.tooltips");
    s.previews = Wh_GetIntSetting(L"items.previews");
    s.indicators = (Indicators)ChoiceSetting(L"items.indicators",
                                             {L"dots", L"hybrid", L"windows"}, 0);
    s.appsDivider = Wh_GetIntSetting(L"items.appsDivider");
    s.highResIcons = Wh_GetIntSetting(L"items.highResIcons");

    s.leftCorner = (LeftCorner)ChoiceSetting(L"hotCorners.bottomLeft",
                                             {L"start", L"taskView", L"off"}, 1);
    s.tray = (Show)ChoiceSetting(L"hotCorners.bottomRight", {L"hidden", L"visible", L"tray"}, 2);

    s.glowEdges = (GlowEdges)ChoiceSetting(L"advanced.glowDirection",
                                           {L"horizontal", L"vertical", L"both"}, 0);
    s.glowSize = std::clamp(Wh_GetIntSetting(L"advanced.glowSize"), 0, 100);
    s.glowOpacity = std::clamp(Wh_GetIntSetting(L"advanced.glowOpacity"), 0, 1000) / 100.0f;
    s.glowCustom = hex(L"advanced.glowColor", s.glowColor);
    if (!hex(L"advanced.tooltipColor", s.tooltipColor)) {
        std::fill(std::begin(s.tooltipColor), std::end(s.tooltipColor), 0xBF);
    }

    ComputeSizes();
}


// =============================================================================
// SIZES
// =============================================================================

constexpr float kStockIconSize = 24;
constexpr float kStockButtonWidth = 44;
constexpr double kStockTaskbarHeight = 48;
constexpr int kMinTaskbarHeight = 40;           // keeps the clock from being squashed
constexpr float kDefaultIconSize = 44;          // at Size 60; spacing settings are pixels here
constexpr float kButtonIconPadding = 8;         // a button is at least its icon + this wide
constexpr float kTrashIconScale = 1.15f;        // Windows' bin icon has empty space around it
constexpr float kMacIconScale = 1024.0f / 824;  // macOS icons fill 824 of 1024px, for shadows
constexpr float kTrashShift = 3;                // the Recycle Bin, moved left, at Size 60
constexpr float kTrashMenuLift = 6;             // its menu's gap above the icon, at Size 60
constexpr float kBounceRoom = 0.25f;            // above a fully magnified icon, of its size
constexpr float kDividerWidth = 1;
constexpr float kDividerGapScale = 1.25f;       // space either side of a divider, in icon gaps
constexpr float kDividerInsetScale = 0.7f;      // divider ends, as a share of the vertical padding

struct Sizes {
    float icon;
    float gap;                // between icons
    float padX, padY;         // dock padding
    float buttonWidth;
    float buttonMargin;       // per side; negative pulls buttons together
    float dockHeight, dockRadius, bottomGap;
    float radius;             // how far from the pointer icons grow
    float iconLoadSize;       // 0 = Windows' own
    float dividerSpace, dividerLeft;
    float trashSlot, trashSpace;
    float bounceRoom;         // how much taller the taskbar is for the bounce
} g_size;

int g_taskbarHeight = 0;      // 0 = the taskbar's normal height (while unloading)

bool DockSizesIcons() { return g_taskbarHeight > 0; }
float DividerInset() { return std::round(g_size.padY * kDividerInsetScale); }

void ComputeSizes() {
    auto& s = g_settings;
    auto& z = g_size;

    // Exponential: 0 -> 19px, 20 -> 25px, 60 -> 44px, 100 -> 77px.
    float icon = std::round(kDefaultIconSize * std::exp2((s.size - 60) / 50.0f));
    float k = icon / kDefaultIconSize;

    z.icon = icon;
    z.gap = std::round(s.spacing * k);
    z.padX = std::round(s.paddingH * k);
    z.padY = std::round(s.paddingV * k);
    // Each button holds its icon and the gap, half on each side, so the taskbar
    // moves apps a whole spot when dragging.
    z.buttonWidth = std::max(kStockButtonWidth, icon + std::max(kButtonIconPadding, z.gap));
    z.buttonMargin = (icon + z.gap - z.buttonWidth) / 2;
    z.dockHeight = icon + 2 * z.padY;
    z.dockRadius = std::round(z.dockHeight * 0.3f);
    z.bottomGap = std::round(icon / 7);
    z.radius = icon * (1 + s.range / 20.0f);
    z.iconLoadSize = s.highResIcons ? std::ceil(icon * s.scale) : 0;

    // Either side of a divider, past the half gap its neighbors hold.
    float room = kDividerGapScale * z.gap;
    z.dividerLeft = room + kDividerWidth / 2;
    z.dividerSpace = room + kDividerWidth + room;
    z.trashSlot = icon + z.gap;
    z.trashSpace = z.dividerSpace + z.trashSlot;

    // Tall enough for the dock and a fully magnified icon, plus 2px, with room
    // above the icon for Windows' launch bounce.
    float dockTop = z.bottomGap + z.dockHeight;
    float iconTop = z.bottomGap + z.padY + s.scale * icon;
    int height = std::max((int)std::ceil(std::max(dockTop, iconTop) + 2), kMinTaskbarHeight);
    g_taskbarHeight = std::max(height, (int)std::ceil(iconTop + s.scale * icon * kBounceRoom));
    z.bounceRoom = (float)(g_taskbarHeight - height);

    Wh_Log(L"Icon %.0f (loaded at %.0f), dock %.0f, taskbar %d, gap %.0f, padding %.0f x %.0f",
           z.icon, z.iconLoadSize, z.dockHeight, g_taskbarHeight, z.gap, z.padX, z.padY);
}

// =============================================================================
// UTILITIES
// =============================================================================

std::atomic<bool> g_unloading = false;

HINSTANCE ModuleInstance() {
    HMODULE module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&ModuleInstance, &module);
    return module;
}

// Calls f(hwnd, primary) for each of Explorer's taskbar windows.
template <typename F>
void ForEachTaskbarWindow(F f) {
    EnumWindows(
        [](HWND w, LPARAM l) -> BOOL {
            DWORD pid;
            WCHAR cls[32];
            if (GetWindowThreadProcessId(w, &pid) && pid == GetCurrentProcessId() &&
                GetClassName(w, cls, ARRAYSIZE(cls))) {
                bool primary = !_wcsicmp(cls, L"Shell_TrayWnd");
                if (primary || !_wcsicmp(cls, L"Shell_SecondaryTrayWnd")) (*(F*)l)(w, primary);
            }
            return TRUE;
        },
        (LPARAM)&f);
}

HWND FindTaskbar() {
    HWND result = nullptr;
    ForEachTaskbarWindow([&result](HWND w, bool primary) {
        if (primary && !result) result = w;
    });
    return result;
}

using RunProc_t = void (*)(void*);

bool RunFromWindowThread(HWND hWnd, RunProc_t proc, void* param) {
    static const UINT msg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param { RunProc_t proc; void* param; };

    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) {
        proc(param);
        return true;
    }
    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int code, WPARAM w, LPARAM l) -> LRESULT {
            auto* cwp = (const CWPSTRUCT*)l;
            if (code == HC_ACTION && cwp->message == msg) {
                auto* p = (Param*)cwp->lParam;
                p->proc(p->param);
            }
            return CallNextHookEx(nullptr, code, w, l);
        },
        nullptr, tid);
    if (!hook) return false;
    Param p{proc, param};
    SendMessage(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

// Calls f(el) for elements under `parent`, `depth` levels down. False skips children.
template <typename F>
void ForEachDescendant(wux::DependencyObject const& parent, int depth, F&& f) {
    int count = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(parent, i);
        auto el = child.try_as<wux::FrameworkElement>();
        if ((!el || f(el)) && depth > 1) ForEachDescendant(child, depth - 1, f);
    }
}

// The taskbar (Taskbar.TaskbarFrame) that `el` is in.
wux::FrameworkElement FindTaskbarFrame(wux::FrameworkElement const& el) {
    wux::DependencyObject current = el;
    for (int depth = 0; current && depth < 20; depth++) {
        if (winrt::get_class_name(current) == L"Taskbar.TaskbarFrame") {
            return current.try_as<wux::FrameworkElement>();
        }
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    return nullptr;
}

bool IsAppButton(wux::FrameworkElement const& el) {
    return winrt::get_class_name(el) == L"Taskbar.TaskListButton";
}

bool IsFileExplorer(wux::FrameworkElement const& button) {
    return wux::Automation::AutomationProperties::GetAutomationId(button) ==
           L"Appid: Microsoft.Windows.Explorer";
}

bool IsStartButton(wux::FrameworkElement const& button) {
    return winrt::get_class_name(button) == L"Taskbar.ExperienceToggleButton" &&
           wux::Automation::AutomationProperties::GetAutomationId(button) == L"StartButton";
}

// Moves an element by its composition visual, outside layout.
void SetTranslation(wux::UIElement const& el, float x, float y = 0) {
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, true);
    wuxh::ElementCompositionPreview::GetElementVisual(el).Properties().InsertVector3(
        L"Translation", {x, y, 0});
}

// Runs f, swallowing (and optionally logging) errors: the taskbar must keep running.
template <typename F>
void Try(F&& f, PCWSTR what = nullptr) {
    try {
        f();
    } catch (...) {
        if (what) Wh_Log(L"%s failed: 0x%08X", what, winrt::to_hresult());
    }
}

// =============================================================================
// SHELL JOBS
// =============================================================================
// Recycling and emptying the Recycle Bin run one at a time on a thread pool
// thread, as they can show Windows' own dialogs.

std::mutex g_shellJobsLock;
std::list<std::function<void()>> g_shellJobs;
PTP_WORK g_shellJobWork = nullptr;

void CALLBACK RunShellJob(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK) {
    std::function<void()> job;
    {
        std::lock_guard lock(g_shellJobsLock);
        if (g_shellJobs.empty()) return;
        job = std::move(g_shellJobs.front());
        g_shellJobs.pop_front();
    }
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    job();
    if (SUCCEEDED(co)) CoUninitialize();
}

void QueueShellJob(std::function<void()> job) {
    if (!g_shellJobWork) return;
    {
        std::lock_guard lock(g_shellJobsLock);
        g_shellJobs.push_back(std::move(job));
    }
    SubmitThreadpoolWork(g_shellJobWork);
}

// =============================================================================
// MOD WINDOW
// =============================================================================
// A message-only window on the taskbar's thread; the app bars share its class.

constexpr WCHAR kWindowClass[] = L"Windhawk_" WH_MOD_ID;
constexpr UINT kAppBarMessage = WM_APP;
constexpr UINT kUpdateOverWindowsMessage = WM_APP + 1;
constexpr UINT kTrashChangedMessage = WM_APP + 2;
constexpr UINT kTrashCheckedMessage = WM_APP + 3;
constexpr UINT kMoveExplorerMessage = WM_APP + 4;
constexpr UINT_PTR kReapplyTimer = 1;
constexpr UINT_PTR kPollTimer = 3;
constexpr UINT_PTR kTrayClipTimer = 4;
constexpr UINT_PTR kSquircleRetryTimer = 5;
constexpr UINT_PTR kMaximizedCheckTimer = 6;
constexpr UINT_PTR kIconsMovingTimer = 7;
constexpr UINT kSquircleRetryMs = 50;

std::atomic<HWND> g_modWindow = nullptr;

LRESULT CALLBACK ModWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool RegisterWindowClass() {
    static bool registered = [] {
        WNDCLASS wc{};
        wc.lpfnWndProc = ModWindowProc;
        wc.hInstance = ModuleInstance();
        wc.lpszClassName = kWindowClass;
        return RegisterClass(&wc) != 0;
    }();
    return registered;
}

// Not again once unloading, as it's destroyed then with anything posted to it.
HWND EnsureModWindow() {
    if (!g_modWindow && !g_unloading && RegisterWindowClass()) {
        g_modWindow = CreateWindowEx(0, kWindowClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                     ModuleInstance(), nullptr);
    }
    return g_modWindow;
}

// =============================================================================
// PROPERTY OVERRIDES
// =============================================================================
// Local values the mod sets on the taskbar's elements, with the originals to
// put back. With `reapply`, the mod's value is set again whenever the taskbar
// changes it.
//
// Containers of XAML objects are no_destroy (XAML shuts down first when
// Explorer exits), and optional so ResetAll can destroy them.

using Reapply = std::function<void(wux::FrameworkElement const&)>;

struct Override {
    winrt::weak_ref<wux::FrameworkElement> element;
    wux::DependencyProperty dp{nullptr};
    wf::IInspectable original{nullptr};
    int64_t token = 0;
};

[[clang::no_destroy]] std::optional<std::list<Override>> g_overrides{std::in_place};

void RestoreLocalValue(wux::DependencyObject const& el, wux::DependencyProperty const& dp,
                       wf::IInspectable const& value) {
    if (!value || value == wux::DependencyProperty::UnsetValue()) el.ClearValue(dp);
    else el.SetValue(dp, value);
}

Override* FindOverride(wux::FrameworkElement const& el, wux::DependencyProperty const& dp) {
    if (!g_overrides) return nullptr;
    for (auto& o : *g_overrides) {
        if (o.dp == dp && o.element.get() == el) return &o;
    }
    return nullptr;
}

Override& Track(wux::FrameworkElement const& el, wux::DependencyProperty const& dp,
                Reapply reapply = nullptr) {
    if (Override* o = FindOverride(el, dp)) return *o;
    auto& o = g_overrides->emplace_back(Override{winrt::make_weak(el), dp, el.ReadLocalValue(dp)});
    if (reapply) {
        o.token = el.RegisterPropertyChangedCallback(
            dp, [reapply](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
                Try([&] { reapply(sender.as<wux::FrameworkElement>()); });
            });
    }
    return o;
}

// The original value back, still tracked.
void PutBack(wux::FrameworkElement const& el, wux::DependencyProperty const& dp) {
    if (Override* o = FindOverride(el, dp)) RestoreLocalValue(el, dp, o->original);
}

void Restore(Override const& o) {
    auto el = o.element.get();
    if (!el) return;
    Try([&] {
        if (o.token) el.UnregisterPropertyChangedCallback(o.dp, o.token);
        RestoreLocalValue(el, o.dp, o.original);
    });
}

// The original value back, no longer tracked.
void Release(wux::FrameworkElement const& el, wux::DependencyProperty const& dp) {
    g_overrides->remove_if([&](Override const& o) {
        if (o.dp != dp || o.element.get() != el) return false;
        Restore(o);
        return true;
    });
}

void RestoreOverrides() {
    for (auto& o : *g_overrides) Restore(o);
    g_overrides->clear();
}

// =============================================================================
// IMAGES
// =============================================================================


MIDL_INTERFACE("905a0fef-bc53-11df-8c49-001e4fc686da")
IDockBufferByteAccess : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE Buffer(BYTE** value) = 0;
};
__CRT_UUID_DECL(IDockBufferByteAccess, 0x905a0fef, 0xbc53, 0x11df, 0x8c, 0x49, 0x00, 0x1e, 0x4f,
                0xc6, 0x86, 0xda)

constexpr GUID kWICImagingFactoryId = {
    0xcacaf262, 0x9370, 0x4615, {0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a}};
constexpr GUID kIWICImagingFactoryId = {
    0xec5ec8a9, 0xc395, 0x4314, {0x9c, 0x77, 0x54, 0xd7, 0xa9, 0x35, 0xff, 0x70}};
constexpr GUID kWICPixelFormat32bppPBGRA = {
    0x6fddc324, 0x4e03, 0x4bfe, {0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10}};
constexpr auto kWICHighQualityCubic = (WICBitmapInterpolationMode)4;

// A XAML bitmap from premultiplied BGRA pixels, top row first.
wuxm::ImageSource BitmapFromPixels(int width, int height, std::vector<BYTE> const& pixels) {
    wuxm::Imaging::WriteableBitmap bitmap(width, height);
    BYTE* data = nullptr;
    winrt::check_hresult(bitmap.PixelBuffer().as<IDockBufferByteAccess>()->Buffer(&data));
    memcpy(data, pixels.data(), pixels.size());
    bitmap.Invalidate();
    return bitmap;
}

// An image file (PNG, or any format Windows reads), scaled to `px` on its
// longer side, decoded synchronously (XAML's own decoding is asynchronous).
wuxm::ImageSource LoadImageFile(std::wstring const& path, int px) {
    if (path.empty() || px <= 0) return nullptr;
    wuxm::ImageSource result{nullptr};
    Try([&] {
        using winrt::check_hresult;
        winrt::com_ptr<IWICImagingFactory> factory;
        winrt::com_ptr<IWICBitmapDecoder> decoder;
        winrt::com_ptr<IWICBitmapFrameDecode> frame;
        winrt::com_ptr<IWICFormatConverter> converter;
        winrt::com_ptr<IWICBitmapScaler> scaler;
        check_hresult(CoCreateInstance(kWICImagingFactoryId, nullptr, CLSCTX_INPROC_SERVER,
                                       kIWICImagingFactoryId, factory.put_void()));
        check_hresult(factory->CreateDecoderFromFilename(
            path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.put()));
        check_hresult(decoder->GetFrame(0, frame.put()));

        UINT width = 0, height = 0;
        check_hresult(frame->GetSize(&width, &height));
        if (!width || !height) winrt::throw_hresult(E_FAIL);
        double k = (double)px / std::max(width, height);
        UINT outWidth = std::max(1u, (UINT)std::lround(width * k));
        UINT outHeight = std::max(1u, (UINT)std::lround(height * k));

        // Premultiplied before scaling, so edges don't get dark fringes.
        check_hresult(factory->CreateFormatConverter(converter.put()));
        check_hresult(converter->Initialize(frame.get(), kWICPixelFormat32bppPBGRA,
                                            WICBitmapDitherTypeNone, nullptr, 0,
                                            WICBitmapPaletteTypeCustom));
        check_hresult(factory->CreateBitmapScaler(scaler.put()));
        if (FAILED(scaler->Initialize(converter.get(), outWidth, outHeight, kWICHighQualityCubic))) {
            check_hresult(scaler->Initialize(converter.get(), outWidth, outHeight,
                                             WICBitmapInterpolationModeFant));
        }
        std::vector<BYTE> pixels((size_t)outWidth * outHeight * 4);
        check_hresult(scaler->CopyPixels(nullptr, outWidth * 4, (UINT)pixels.size(), pixels.data()));
        result = BitmapFromPixels(outWidth, outHeight, pixels);
    }, L"Icon image");
    return result;
}

// Pixels to load an icon drawn `scale` times the icon size at, fully magnified.
int MagnifiedPixels(wux::UIElement const& el, float scale) {
    double dpiScale = 1;
    Try([&] {
        if (auto root = el.XamlRoot()) dpiScale = root.RasterizationScale();
    });
    return (int)std::ceil(std::ceil(g_size.icon * g_settings.scale) * scale * dpiScale);
}

// ---- Finder and Trash ----
// The user's own icons, drawn bigger than the icon's spot so their artwork fills
// it and their margin (a shadow) spills past. They also take the names Finder
// and Trash. Without them, Windows' own.

bool HasFinderIcon() { return !g_settings.customFinder.path.empty(); }
bool HasTrashIcon() { return !g_settings.customTrash.path.empty(); }

float TrashIconScale() {
    return HasTrashIcon() ? g_settings.customTrash.scale : kTrashIconScale;
}

// The Recycle Bin's artwork height, for its label. Windows' bin is drawn bigger
// than its artwork, so its label goes halfway up the difference.
float TrashArtHeight() {
    if (HasTrashIcon()) return g_size.icon * g_settings.customTrash.size;
    return g_size.icon * (1 + kTrashIconScale) / 2;
}

// Empty for Windows' own icon. Without a full image, the empty one.
std::wstring TrashPath(bool full) {
    auto& custom = g_settings.customTrash;
    if (!HasTrashIcon()) return {};
    return full && !custom.fullPath.empty() ? custom.fullPath : custom.path;
}

// =============================================================================
// TASKBAR BUTTONS
// =============================================================================

void MarkPlatesDirty();

struct Button {
    winrt::weak_ref<wux::FrameworkElement> element;
    winrt::weak_ref<wux::FrameworkElement> root;  // template root, widened to fit the icon
    wux::FrameworkElement::SizeChanged_revoker sizeChanged;
    double originalIconColumn = 0, appliedIconColumn = 0;
    float extraLeft = 0, extraRight = 0;  // room for the apps divider
    void* impl = nullptr;                 // the taskbar's object for it, to move it
};

[[clang::no_destroy]] std::optional<std::list<Button>> g_buttons{std::in_place};

Button* FindButton(wux::FrameworkElement const& el) {
    if (!g_buttons) return nullptr;
    for (auto& b : *g_buttons) {
        if (b.element.get() == el) return &b;
    }
    return nullptr;
}

// Side margins: none, as buttons hold the gaps, unless they're too small to; plus
// the apps divider's room.
void ApplySpacing(wux::FrameworkElement const& el) {
    Button* b = FindButton(el);
    wux::Thickness want{g_size.buttonMargin + (b ? b->extraLeft : 0), 0,
                        g_size.buttonMargin + (b ? b->extraRight : 0), 0};
    if (el.Margin() != want) el.Margin(want);
}

Button& TrackButton(wux::FrameworkElement const& el) {
    if (Button* b = FindButton(el)) return *b;
    auto& b = g_buttons->emplace_back();
    b.element = winrt::make_weak(el);
    Track(el, wux::FrameworkElement::MarginProperty(), ApplySpacing);
    b.sizeChanged = el.SizeChanged(
        winrt::auto_revoke,
        [](wf::IInspectable const&, wux::SizeChangedEventArgs const&) { MarkPlatesDirty(); });
    MarkPlatesDirty();
    return b;
}

// The taskbar sizes buttons for a 24px icon; a minimum width makes room.
void ApplyButtonWidth(Button& b, wux::FrameworkElement const& button) {
    auto root = b.root.get();
    if (!root) {
        if (wuxm::VisualTreeHelper::GetChildrenCount(button) == 0) return;
        root = wuxm::VisualTreeHelper::GetChild(button, 0).try_as<wux::FrameworkElement>();
        if (!root) return;
        b.root = winrt::make_weak(root);
        Track(root, wux::FrameworkElement::MinWidthProperty());
    }
    if (root.MinWidth() != g_size.buttonWidth) root.MinWidth(g_size.buttonWidth);
}

// App buttons' first grid column holds the icon, sized for 24px by the taskbar.
void ApplyIconColumn(Button& b) {
    auto root = b.root.get();
    auto grid = root ? root.try_as<wuxc::Grid>() : nullptr;
    if (!grid || grid.ColumnDefinitions().Size() == 0) return;
    auto column = grid.ColumnDefinitions().GetAt(0);
    auto width = column.Width();
    if (width.GridUnitType != wux::GridUnitType::Pixel) return;

    if (width.Value != b.appliedIconColumn) b.originalIconColumn = width.Value;
    auto padding = grid.Padding();
    double target = g_size.buttonWidth - padding.Left - padding.Right;
    if (width.Value != target) column.Width({target, wux::GridUnitType::Pixel});
    b.appliedIconColumn = target;
}

void ReleaseButton(Button& b) {
    b.sizeChanged.revoke();
    auto grid = b.root.get() ? b.root.get().try_as<wuxc::Grid>() : nullptr;
    if (!grid || b.originalIconColumn <= 0 || grid.ColumnDefinitions().Size() == 0) return;
    auto column = grid.ColumnDefinitions().GetAt(0);
    if (column.Width().Value == b.appliedIconColumn) {
        column.Width({b.originalIconColumn, wux::GridUnitType::Pixel});
    }
}

// Visible buttons in `parent`, where the taskbar laid them out.
struct Icon {
    wux::FrameworkElement element{nullptr};
    float left = 0, top = 0, width = 0, height = 0;
    float slotLeft = 0;  // margins included, for the taskbar's order
};

std::vector<Icon> GetIcons(wux::UIElement const& parent) {
    std::vector<Icon> icons;
    for (auto& b : *g_buttons) {
        auto el = b.element.get();
        if (!el || el.Visibility() != Visibility::Visible || el.ActualWidth() <= 0 ||
            wuxm::VisualTreeHelper::GetParent(el) != parent) {
            continue;
        }
        auto slot = wuxc::Primitives::LayoutInformation::GetLayoutSlot(el);
        auto margin = el.Margin();
        float x = slot.X + (float)margin.Left;
        float y = slot.Y + (float)margin.Top;
        // Recycled buttons are parked out of view.
        if (y < -el.ActualHeight() || y > el.ActualHeight()) continue;
        icons.push_back({el, x, y, (float)el.ActualWidth(), (float)el.ActualHeight(), slot.X});
    }
    return icons;
}

// ---- Icon size ----
// Resizing the image, not scaling it, lets Windows draw it from full resolution.

void ApplyIconSize(wux::FrameworkElement const& el) {
    float size = g_size.icon;
    if (el.Width() != size) el.Width(size);  // NaN when unset, so this covers it
    if (el.Height() != size) el.Height(size);
}

void TrackIconImage(wux::FrameworkElement const& el) {
    Track(el, wux::FrameworkElement::WidthProperty(), ApplyIconSize);
    Track(el, wux::FrameworkElement::HeightProperty(), ApplyIconSize);
    ApplyIconSize(el);
}

// ---- Overlay icons ----
// An app's status badge, like a recording dot. Windows puts it at the top of the
// button, far above the icon in the taller dock, so magnifying would carry it
// off the taskbar. It goes on the icon's top-right corner instead, centered in
// the same cell and pushed out by margins.

void ApplyOverlayPlace(wux::FrameworkElement const& el) {
    double size = std::isnan(el.Width()) ? el.ActualWidth() : el.Width();
    double shift = std::max(0.0, (g_size.icon - size) / 2);
    if (el.HorizontalAlignment() != wux::HorizontalAlignment::Center) {
        el.HorizontalAlignment(wux::HorizontalAlignment::Center);
    }
    if (el.VerticalAlignment() != wux::VerticalAlignment::Center) {
        el.VerticalAlignment(wux::VerticalAlignment::Center);
    }
    wux::Thickness margin{2 * shift, 0, 0, 2 * shift};
    auto have = el.Margin();
    if (have.Left != margin.Left || have.Top != margin.Top || have.Right != margin.Right ||
        have.Bottom != margin.Bottom) {
        el.Margin(margin);
    }
}

void TrackOverlayIcon(wux::FrameworkElement const& el) {
    for (auto& dp : {wux::FrameworkElement::MarginProperty(),
                     wux::FrameworkElement::HorizontalAlignmentProperty(),
                     wux::FrameworkElement::VerticalAlignmentProperty()}) {
        Track(el, dp, ApplyOverlayPlace);
    }
    ApplyOverlayPlace(el);
}

// ---- Highlights, Start and the background ----
// Highlights are collapsed and also made transparent, as the taskbar animates
// their opacity.

void ApplyHighlightHidden(wux::FrameworkElement const& el) {
    if (el.Opacity() != 0) el.Opacity(0);
    if (el.Visibility() != Visibility::Collapsed) el.Visibility(Visibility::Collapsed);
}

void HideHighlight(wux::FrameworkElement const& el) {
    Track(el, wux::UIElement::OpacityProperty(), ApplyHighlightHidden);
    Track(el, wux::UIElement::VisibilityProperty(), ApplyHighlightHidden);
    ApplyHighlightHidden(el);
}

void ApplyStartHidden(wux::FrameworkElement const& el) {
    if (el.Visibility() != Visibility::Collapsed) {
        el.Visibility(Visibility::Collapsed);
        MarkPlatesDirty();
    }
}

void UpdateStartHidden(wux::FrameworkElement const& button) {
    if (!IsStartButton(button)) return;
    auto dp = wux::UIElement::VisibilityProperty();
    Track(button, dp, ApplyStartHidden);
    ApplyStartHidden(button);
}

void ApplyCollapsed(wux::FrameworkElement const& el) {
    if (el.Visibility() != Visibility::Collapsed) el.Visibility(Visibility::Collapsed);
}

// Windows' own tooltip, replaced by the dock's labels.
void HideStockToolTip(wux::FrameworkElement const& button) {
    auto value = wuxc::ToolTipService::GetToolTip(button);
    auto tip = value ? value.try_as<wuxc::ToolTip>() : nullptr;
    if (!tip || FindOverride(tip, wux::UIElement::VisibilityProperty())) return;
    Track(tip, wux::UIElement::VisibilityProperty(), ApplyCollapsed);
    ApplyCollapsed(tip);
}

// Opacity rather than Collapsed, so it still catches clicks on empty space.
void HideBackground(wux::FrameworkElement const& el) {
    if (el.Name() != L"BackgroundControl") return;
    Track(el, wux::UIElement::OpacityProperty());
    if (el.Opacity() != 0) el.Opacity(0);
}

// =============================================================================
// DOCK PLATE
// =============================================================================
// Behind each taskbar's icons: blur, color and edge, layered in a grid.

constexpr float kMaxBlur = 60;                  // Blur 50 = 30, like Windows' acrylic
constexpr float kEdgeHighlight = 0.7f;          // top and bottom edge: this far to white
constexpr float kEdgeShadow = 0.7f;             // side edges: this far to black
constexpr float kDividerOpacity = 0.75f;        // of the dots' opacity
constexpr float kEdgeHighlightOpacity = 0.5f;   // top and bottom edge, of the edges' opacity
constexpr float kEdgeMinOpacity = 0.20f;        // edge opacity at dock Opacity 0...
constexpr float kEdgeMaxOpacity = 0.65f;        // ...rising to this at kEdgeMaxOpacityAt
constexpr int kEdgeMaxOpacityAt = 30;
constexpr int kGlowSteps = 10;                  // gradient stops per glowing edge
constexpr float kGlowFalloff = 4.5f;            // Gaussian: e^(-k t^2) over the reach
constexpr float kMarkDarkLightness = 0.05f;   // dividers and dots on a light dock
constexpr float kMarkLightLightness = 0.9f;   // dividers and dots on a dark dock
constexpr uint8_t kMarkAlphaOnLight = 200;
constexpr uint8_t kMarkAlphaOnDark = 125;
constexpr uint8_t kStandardEdgeAlphaOnLight = 135;

// ---- Colors ----

bool IsLightColor(uint8_t r, uint8_t g, uint8_t b) {
    return 0.299f * r + 0.587f * g + 0.114f * b > 140;
}

float Lightness(Color color) {
    int max = std::max({color.R, color.G, color.B});
    int min = std::min({color.R, color.G, color.B});
    return (max + min) / 510.0f;
}

// `color` with its HSL lightness moved by `delta`.
Color ShiftLightness(Color color, float delta);

// The Mac edge's inner glow, lighter than `fill`, at `strength` of its opacity.
Color GlowColor(Color fill, float strength) {
    Color glow = ShiftLightness(fill, (1 - Lightness(fill)) * kEdgeHighlight);
    glow.A = (uint8_t)std::min(255L, std::lround(fill.A * strength));
    return glow;
}

// Gradient stops for an inner glow from both ends, `reach` deep, as (offset,
// strength) in order. Strength falls off as a half Gaussian, reaching 0 at the
// reach, so the glow fades out with no visible edge.
std::array<std::pair<float, float>, kGlowSteps * 2> GlowStops(float reach) {
    std::array<std::pair<float, float>, kGlowSteps * 2> stops;
    float end = std::exp(-kGlowFalloff);
    for (int i = 0; i < kGlowSteps; i++) {
        float t = i / (kGlowSteps - 1.0f);
        float strength = (std::exp(-kGlowFalloff * t * t) - end) / (1 - end);
        stops[i] = {reach * t, strength};
        stops[kGlowSteps * 2 - 1 - i] = {1 - reach * t, strength};
    }
    return stops;
}

// `color` with its HSL lightness moved by `delta`.
Color ShiftLightness(Color color, float delta) {
    float r = color.R / 255.0f, g = color.G / 255.0f, b = color.B / 255.0f;
    float max = std::max({r, g, b}), min = std::min({r, g, b});
    float l = (max + min) / 2, h = 0, s = 0;
    if (max != min) {
        float d = max - min;
        s = l > 0.5f ? d / (2 - max - min) : d / (max + min);
        if (max == r) h = (g - b) / d + (g < b ? 6 : 0);
        else if (max == g) h = (b - r) / d + 2;
        else h = (r - g) / d + 4;
        h /= 6;
    }
    l = std::clamp(l + delta, 0.0f, 1.0f);
    auto channel = [](float p, float q, float t) {
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        if (t < 1.0f / 6) return p + (q - p) * 6 * t;
        if (t < 1.0f / 2) return q;
        if (t < 2.0f / 3) return p + (q - p) * (2.0f / 3 - t) * 6;
        return p;
    };
    if (s == 0) {
        r = g = b = l;
    } else {
        float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        r = channel(p, q, h + 1.0f / 3);
        g = channel(p, q, h);
        b = channel(p, q, h - 1.0f / 3);
    }
    auto byte = [](float v) { return (uint8_t)std::lround(std::clamp(v, 0.0f, 1.0f) * 255); };
    return {color.A, byte(r), byte(g), byte(b)};
}

// ---- Blur ----
// Described directly, as Explorer doesn't ship Win2D.

enum DOCK_EFFECT_PROPERTY_MAPPING : int {
    DOCK_EFFECT_PROPERTY_MAPPING_DIRECT = 1,
};

MIDL_INTERFACE("2FC57384-A068-44D7-A331-30982FCF7177")
IDockGraphicsEffectD2D1Interop : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, DOCK_EFFECT_PROPERTY_MAPPING* mapping) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) = 0;
};
__CRT_UUID_DECL(IDockGraphicsEffectD2D1Interop, 0x2fc57384, 0xa068, 0x44d7, 0xa3, 0x31, 0x30,
                0x98, 0x2f, 0xcf, 0x71, 0x77)

constexpr GUID kGaussianBlurId = {  // CLSID_D2D1GaussianBlur
    0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

class GaussianBlurEffect
    : public winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect,
                               wge::IGraphicsEffectSource, IDockGraphicsEffectD2D1Interop> {
public:
    GaussianBlurEffect(float amount, wge::IGraphicsEffectSource const& source)
        : m_amount(amount), m_source(source) {}

    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring const& name) { m_name = name; }

    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) override {
        *id = kGaussianBlurId;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, DOCK_EFFECT_PROPERTY_MAPPING* mapping) override {
        if (_wcsicmp(name, L"BlurAmount") != 0) return E_INVALIDARG;
        *index = 0;
        *mapping = DOCK_EFFECT_PROPERTY_MAPPING_DIRECT;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) override {
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) override {
        try {
            wf::IInspectable v;
            switch (index) {
                case 0: v = wf::PropertyValue::CreateSingle(m_amount); break;  // std. deviation
                case 1: v = wf::PropertyValue::CreateUInt32(1); break;         // balanced
                case 2: v = wf::PropertyValue::CreateUInt32(1); break;         // hard border
                default: return E_INVALIDARG;
            }
            *value = winrt::detach_abi(v.as<wf::IPropertyValue>());
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) override {
        if (index != 0) return E_INVALIDARG;
        winrt::copy_to_abi(m_source, *source);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) override {
        *count = 1;
        return S_OK;
    }

private:
    float m_amount;
    wge::IGraphicsEffectSource m_source;
    winrt::hstring m_name;
};

// ---- Squircle ----

// Apple's continuous corner, as PaintCode measured it: a near-circular arc eased
// into the straight edges by Béziers. Points are (along the edge in, along the
// edge out) from the corner, in radii. After the first, each three are a cubic
// curve; a straight line repeats its end.
constexpr float kCornerReach = 1.52866f;
constexpr float kCorner[16][2] = {
    {kCornerReach, 0},
    {1.08849f, 0}, {0.86841f, 0}, {0.66993f, 0.06550f},
    {0.66993f, 0.06550f}, {0.63149f, 0.07491f}, {0.63149f, 0.07491f},
    {0.37282f, 0.16906f}, {0.16906f, 0.37282f}, {0.07491f, 0.63149f},
    {0.07491f, 0.63149f}, {0.06550f, 0.66993f}, {0.06550f, 0.66993f},
    {0, 0.86841f}, {0, 1.08849f}, {0, kCornerReach},
};

// The largest corner radius up to `r` that fits a `w` x `h` box.
float SquircleRadius(float w, float h, float r) {
    return std::clamp(std::min(w, h) / 2 / kCornerReach, 0.0f, r);
}

// A closed outline: a start point, then cubic curves as three points each.
std::vector<wf::Point> Squircle(float x, float y, float w, float h, float rx, float ry) {
    struct Corner { float x, y, inX, inY, outX, outY; };
    Corner corners[] = {{x + w, y, -1, 0, 0, 1}, {x + w, y + h, 0, -1, -1, 0},
                        {x, y + h, 1, 0, 0, -1}, {x, y, 0, 1, 1, 0}};
    std::vector<wf::Point> points;
    for (auto& c : corners) {
        for (int i = 0; i < 16; i++) {
            auto [in, out] = kCorner[i];
            wf::Point point{c.x + (in * c.inX + out * c.outX) * rx,
                            c.y + (in * c.inY + out * c.outY) * ry};
            if (i == 0 && !points.empty()) {
                points.insert(points.end(), {points.back(), point});  // the straight edge
            }
            points.push_back(point);
        }
    }
    return points;
}

wuxm::PathGeometry SquircleGeometry(std::initializer_list<std::vector<wf::Point>> outlines) {
    wuxm::PathGeometry geometry;
    geometry.FillRule(wuxm::FillRule::EvenOdd);
    for (auto& points : outlines) {
        wuxm::PolyBezierSegment curves;
        for (size_t i = 1; i < points.size(); i++) curves.Points().Append(points[i]);
        wuxm::PathFigure figure;
        figure.StartPoint(points[0]);
        figure.IsClosed(true);
        figure.Segments().Append(curves);
        geometry.Figures().Append(figure);
    }
    return geometry;
}

MIDL_INTERFACE("0657AF73-53FD-47CF-84FF-C8492D2A80A3")
IDockGeometrySource2DInterop : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetGeometry(ID2D1Geometry** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetGeometryUsingFactory(ID2D1Factory* factory,
                                                                 ID2D1Geometry** value) = 0;
};
__CRT_UUID_DECL(IDockGeometrySource2DInterop, 0x0657af73, 0x53fd, 0x47cf, 0x84, 0xff, 0xc8,
                0x49, 0x2d, 0x2a, 0x80, 0xa3)

winrt::com_ptr<ID2D1Factory> g_d2d;

struct GeometrySource
    : winrt::implements<GeometrySource, wg::IGeometrySource2D, IDockGeometrySource2DInterop> {
    winrt::com_ptr<ID2D1Geometry> geometry;
    HRESULT STDMETHODCALLTYPE GetGeometry(ID2D1Geometry** value) override {
        geometry.copy_to(value);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE TryGetGeometryUsingFactory(ID2D1Factory*,
                                                         ID2D1Geometry** value) override {
        *value = nullptr;
        return E_NOTIMPL;
    }
};

// The same outline for composition, which takes it through Direct2D.
wuc::CompositionPath SquirclePath(std::vector<wf::Point> const& points) {
    if (!g_d2d) {
        winrt::check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
                                               __uuidof(ID2D1Factory), nullptr, g_d2d.put_void()));
    }
    winrt::com_ptr<ID2D1PathGeometry> path;
    winrt::check_hresult(g_d2d->CreatePathGeometry(path.put()));
    winrt::com_ptr<ID2D1GeometrySink> sink;
    winrt::check_hresult(path->Open(sink.put()));
    auto at = [&](size_t i) { return D2D1_POINT_2F{points[i].X, points[i].Y}; };
    sink->BeginFigure(at(0), D2D1_FIGURE_BEGIN_FILLED);
    for (size_t i = 1; i + 2 < points.size(); i += 3) sink->AddBezier({at(i), at(i + 1), at(i + 2)});
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    winrt::check_hresult(sink->Close());
    auto source = winrt::make_self<GeometrySource>();
    source->geometry = path;
    return wuc::CompositionPath(source.as<wg::IGeometrySource2D>());
}

// ---- Blur ----

// What's behind `host`, blurred and clipped to the dock's shape (returned).
wuc::CompositionPathGeometry SetBlurVisual(wux::UIElement const& host, float amount) {
    if (amount <= 0) {
        wuxh::ElementCompositionPreview::SetElementChildVisual(host, nullptr);
        return nullptr;
    }
    auto hostVisual = wuxh::ElementCompositionPreview::GetElementVisual(host);
    auto compositor = hostVisual.Compositor();

    auto effect = winrt::make<GaussianBlurEffect>(
        amount, wuc::CompositionEffectSourceParameter(L"Backdrop"));
    auto brush = compositor.CreateEffectFactory(effect).CreateBrush();
    brush.SetSourceParameter(L"Backdrop", compositor.CreateBackdropBrush());

    auto sprite = compositor.CreateSpriteVisual();
    sprite.Brush(brush);
    auto size = compositor.CreateExpressionAnimation(L"host.Size");
    size.SetReferenceParameter(L"host", hostVisual);
    sprite.StartAnimation(L"Size", size);
    auto shape = compositor.CreatePathGeometry();
    sprite.Clip(compositor.CreateGeometricClip(shape));

    wuxh::ElementCompositionPreview::SetElementChildVisual(host, sprite);
    return shape;
}

// ---- Plate ----

struct Plate {
    winrt::weak_ref<wux::UIElement> row;           // the icons' parent
    winrt::weak_ref<wuxc::Panel> panel;            // the row's parent, which holds the plate
    winrt::weak_ref<wux::FrameworkElement> frame;  // the taskbar
    wuxc::Grid root{nullptr};
    wuxc::Border blurHost{nullptr};
    wuxs::Path fill{nullptr}, edgeInner{nullptr}, edgeOuter{nullptr};
    wuxs::Path topGlow{nullptr}, sideGlow{nullptr};  // in from the top and bottom, and the sides
    wuxm::LinearGradientBrush topGlowBrush{nullptr}, sideGlowBrush{nullptr};
    wuc::CompositionPathGeometry blurShape{nullptr};
    float appliedBlur = -1;
    float baseLeft = 0, baseWidth = 0;  // when nothing is magnified
    float left = -1, width = -1;        // last applied
    float top = 0;
    float offsetY = 0;                  // how far the icons are moved down
    float originX = 0, originY = 0;     // the row's top-left corner in the panel
    size_t iconCount = 0;               // last measured
    Color dotColor{};
    wux::FrameworkElement::SizeChanged_revoker rowSizeChanged, panelSizeChanged;
    wux::FrameworkElement::ActualThemeChanged_revoker panelThemeChanged;

    // The Recycle Bin and its divider, in the panel past the last button.
    wuxc::Grid trash{nullptr};
    wuxc::Image trashImage{nullptr};
    wuxc::Border trashMenuAnchor{nullptr};  // at the taskbar's top edge, where jump lists open
    wuxc::Border divider{nullptr};
    wux::UIElement::Tapped_revoker trashTapped;
    wux::UIElement::RightTapped_revoker trashRightTapped;
    wux::UIElement::DragEnter_revoker trashDragEnter;
    wux::UIElement::DragOver_revoker trashDragOver;
    wux::UIElement::Drop_revoker trashDrop;
    float trashCenter = 0;  // in the row
    wux::Thickness rowMargin{};  // the taskbar's own, before the bin's room
    float rowRoom = 0;
    int trashState = -2;    // icon shown: -1 unknown, 0 empty, 1 full

    // The divider after File Explorer.
    wuxc::Border appsDivider{nullptr};
    winrt::weak_ref<wux::FrameworkElement> beforeAppsDivider;
    float appsDividerShift = 0;

    float hideY = 0;     // auto-hide's slide, applied to the panel
};

[[clang::no_destroy]] std::optional<std::list<Plate>> g_plates{std::in_place};

std::array<wux::UIElement, 4> PlateParts(Plate const& p) {
    return {p.root, p.trash, p.divider, p.appsDivider};
}

Plate* GetRowPlate(wux::DependencyObject const& row) {
    if (!row || !g_plates) return nullptr;
    for (auto& p : *g_plates) {
        if (p.row.get() == row) return &p;
    }
    return nullptr;
}

float GetRowOffsetY(wux::DependencyObject const& row) {
    Plate* p = GetRowPlate(row);
    return p ? p->offsetY : 0;
}

void ApplyPlateBlur(Plate& p) {
    float blur = g_settings.blur * kMaxBlur / 100;
    if (blur == p.appliedBlur) return;
    p.appliedBlur = blur;
    p.blurShape = nullptr;
    Try([&] { p.blurShape = SetBlurVisual(p.blurHost, blur); }, L"Dock blur");
}

void ColorDots(Plate& p);

// The squircle at the dock's current width, its edges, and how far the glow
// reaches in from the light edges.
void ShapePlate(Plate& p) {
    float w = p.width > 0 ? p.width : p.baseWidth, h = g_size.dockHeight;
    if (w <= 0 || h <= 0) return;
    float r = SquircleRadius(w, h, g_size.dockRadius), in = std::max(0.0f, r - 1);
    auto outline = Squircle(0, 0, w, h, r, r);

    p.fill.Data(SquircleGeometry({outline}));
    p.topGlow.Data(SquircleGeometry({outline}));
    p.sideGlow.Data(SquircleGeometry({outline}));
    p.edgeInner.Data(nullptr);
    p.edgeOuter.Data(nullptr);
    bool mac = g_settings.border == Border::Mac;
    auto edges = g_settings.glowEdges;
    if (g_settings.border == Border::Standard || (mac && edges == GlowEdges::All)) {
        // A line inside all four sides.
        p.edgeInner.Data(SquircleGeometry({outline, Squircle(1, 1, w - 2, h - 2, in, in)}));
    } else if (mac && edges == GlowEdges::TopBottom) {
        // A light line inside the top and bottom; a dark one outside the sides,
        // laid out 1px wider each side.
        p.edgeInner.Data(SquircleGeometry({outline, Squircle(0, 1, w, h - 2, r, in)}));
        p.edgeOuter.Margin({-1, 0, -1, 0});
        p.edgeOuter.Data(SquircleGeometry({Squircle(0, 0, w + 2, h, r + 1, r),
                                           Squircle(1, 0, w, h, r, r)}));
    } else if (mac) {
        p.edgeInner.Data(SquircleGeometry({outline, Squircle(1, 0, w - 2, h, in, r)}));
        p.edgeOuter.Margin({0, -1, 0, -1});
        p.edgeOuter.Data(SquircleGeometry({Squircle(0, 0, w, h + 2, r, r + 1),
                                           Squircle(0, 1, w, h, r, r)}));
    }
    if (p.blurShape) Try([&] { p.blurShape.Path(SquirclePath(outline)); }, L"Dock blur shape");

    // The same depth in pixels from every glowing edge.
    float size = g_settings.glowSize * g_size.icon / kDefaultIconSize;
    for (auto [brush, extent] : {std::pair{p.topGlowBrush, h}, std::pair{p.sideGlowBrush, w}}) {
        if (!brush) continue;
        auto stops = brush.GradientStops();
        uint32_t i = 0;
        for (auto [offset, strength] : GlowStops(std::clamp(size / extent, 0.0f, 0.5f))) {
            stops.GetAt(i++).Offset(offset);
        }
    }
}

// A glow in `color` fading in from both ends of the dock along `end`: (0, 1)
// for the top and bottom, (1, 0) for the sides. ShapePlate sets its depth.
wuxm::LinearGradientBrush GlowBrush(Color color, wf::Point end) {
    wuxm::LinearGradientBrush brush;
    brush.StartPoint({0, 0});
    brush.EndPoint(end);
    for (auto [offset, strength] : GlowStops(0)) {
        Color stopColor = color;
        stopColor.A = (uint8_t)std::lround(color.A * strength);
        wuxm::GradientStop stop;
        stop.Color(stopColor);
        brush.GradientStops().Append(stop);
    }
    return brush;
}

// Frosted glass. Dividers and dots are dark on a light dock, light on a dark one.
void ApplyPlateLook(Plate& p) {
    auto& s = g_settings;
    uint8_t alpha = (uint8_t)(s.opacity * 255 / 100);
    bool light = s.theme == Theme::Light;
    if (s.theme == Theme::System) {
        auto panel = p.panel.get();
        light = panel && panel.ActualTheme() == wux::ElementTheme::Light;
    }
    Color fill = light ? Color{alpha, 0xCC, 0xCC, 0xCC} : Color{alpha, 0x33, 0x33, 0x33};
    if (s.theme == Theme::Custom) {
        auto [r, g, b] = s.customColor;
        light = IsLightColor(r, g, b);
        fill = Color{alpha, r, g, b};
    }

    Color mark = ShiftLightness(
        fill, (light ? kMarkDarkLightness : kMarkLightLightness) - Lightness(fill));
    mark.A = light ? kMarkAlphaOnLight : kMarkAlphaOnDark;
    Color dividerColor = mark;
    dividerColor.A = (uint8_t)std::lround(mark.A * kDividerOpacity);
    for (auto& divider : {p.divider, p.appsDivider}) {
        if (divider) divider.Background(wuxm::SolidColorBrush(dividerColor));
    }
    p.dotColor = mark;
    ColorDots(p);

    p.fill.Fill(wuxm::SolidColorBrush(fill));
    p.edgeInner.Fill(nullptr);
    p.edgeOuter.Fill(nullptr);
    p.topGlowBrush = nullptr;
    p.sideGlowBrush = nullptr;

    if (s.border == Border::Standard) {
        // A solid edge in the dividers' color, softer on a light dock.
        Color line = mark;
        if (light) line.A = kStandardEdgeAlphaOnLight;
        p.edgeInner.Fill(wuxm::SolidColorBrush(line));
    } else if (s.border == Border::Mac) {
        // Lighter top and bottom (or sides, or all four), glowing inward; any
        // other two darker. Each fades out around the corners.
        Color lighter = ShiftLightness(fill, (1 - Lightness(fill)) * kEdgeHighlight);
        Color darker = ShiftLightness(fill, -Lightness(fill) * kEdgeShadow);
        float t = std::clamp((float)s.opacity / kEdgeMaxOpacityAt, 0.0f, 1.0f);
        uint8_t edgeAlpha =
            (uint8_t)std::lround(255 * (kEdgeMinOpacity + (kEdgeMaxOpacity - kEdgeMinOpacity) * t));
        lighter.A = (uint8_t)std::lround(edgeAlpha * kEdgeHighlightOpacity);
        darker.A = edgeAlpha;
        p.edgeInner.Fill(wuxm::SolidColorBrush(lighter));
        p.edgeOuter.Fill(wuxm::SolidColorBrush(darker));

        Color glowStart = GlowColor(fill, s.glowOpacity);
        if (s.glowCustom) {
            auto [r, g, b] = s.glowColor;
            glowStart = Color{glowStart.A, r, g, b};
        }
        if (s.glowEdges != GlowEdges::Sides) p.topGlowBrush = GlowBrush(glowStart, {0, 1});
        if (s.glowEdges != GlowEdges::TopBottom) p.sideGlowBrush = GlowBrush(glowStart, {1, 0});
    }

    p.topGlow.Fill(p.topGlowBrush);
    p.sideGlow.Fill(p.sideGlowBrush);
    ApplyPlateBlur(p);
    ShapePlate(p);
}

// Lays `el` out from the panel's top-left corner, positioned by its margin.
void PlaceInPanel(wuxc::Panel const& panel, wux::FrameworkElement const& el) {
    el.HorizontalAlignment(wux::HorizontalAlignment::Left);
    el.VerticalAlignment(wux::VerticalAlignment::Top);
    el.Visibility(Visibility::Collapsed);
    if (panel.try_as<wuxc::Grid>()) {
        wuxc::Grid::SetRowSpan(el, 100);
        wuxc::Grid::SetColumnSpan(el, 100);
    }
}

// Adds `el` right after the icon row, so it's drawn above it, or right before
// it, so it's drawn beneath it (still above the plate).
bool InsertBesideRow(wuxc::Panel const& panel, wux::UIElement const& row,
                     wux::UIElement const& el, bool above = true) {
    uint32_t index = 0;
    if (!panel.Children().IndexOf(row, index)) return false;
    panel.Children().InsertAt(above ? index + 1 : index, el);
    return true;
}

wuxc::Border AddDivider(wuxc::Panel const& panel, wux::UIElement const& row) {
    wuxc::Border divider;
    PlaceInPanel(panel, divider);
    divider.IsHitTestVisible(false);
    divider.UseLayoutRounding(false);  // allows a fraction of a pixel
    divider.Width(kDividerWidth);
    divider.Height(std::max(1.0f, g_size.dockHeight - 2 * DividerInset()));
    return InsertBesideRow(panel, row, divider) ? divider : nullptr;
}

// =============================================================================
// APP ICONS
// =============================================================================
// File Explorer can show the user's own icon, and every app a squircle tile,
// as on a Mac. The taskbar reuses buttons and sets new icons, so
// a replacement follows along, keeping the taskbar's own icon to put back. Both
// are drawn bigger than their box, so their artwork matches other icons and
// their shadows spill past.

enum class IconLook { Stock, Finder, Squircle };

struct IconSwap {
    winrt::weak_ref<wuxc::Image> image;
    winrt::weak_ref<wux::FrameworkElement> button;
    IconLook look = IconLook::Stock;
    wuxm::ImageSource stock{nullptr};  // the taskbar's latest
    wuxm::ImageSource shown{nullptr};  // what replaces it, once ready
    wuxm::ScaleTransform transform{nullptr};  // the mod's, while a replacement shows
    wf::IAsyncInfo render{nullptr};           // drawing the icon to make a tile from
    bool rendering = false;
    bool retry = false;  // drawn again on the retry timer
    int retries = 0;
};

[[clang::no_destroy]] std::list<IconSwap> g_iconSwaps;
[[clang::no_destroy]] std::optional<wuxm::ImageSource> g_finderIconSource;  // none if unreadable

IconLook LookFor(wux::FrameworkElement const& button) {
    if (HasFinderIcon() && IsFileExplorer(button)) return IconLook::Finder;
    if (g_settings.squircleIcons && IsAppButton(button)) return IconLook::Squircle;
    return IconLook::Stock;
}

IconSwap* FindSwap(wux::FrameworkElement const& image) {
    for (auto& s : g_iconSwaps) {
        if (s.image.get() == image) return &s;
    }
    return nullptr;
}

// Loaded once. An unreadable custom image leaves Windows' own icon.
wuxm::ImageSource FinderIconSource(wux::UIElement const& el) {
    if (!g_finderIconSource) {
        auto& custom = g_settings.customFinder;
        g_finderIconSource = LoadImageFile(custom.path, MagnifiedPixels(el, custom.scale));
    }
    return *g_finderIconSource;
}

// Bigger while a replacement shows; the taskbar's own icon keeps its size.
void ApplyIconScale(wux::FrameworkElement const& el) {
    IconSwap* s = FindSwap(el);
    if (!s) return;
    auto dp = wux::UIElement::RenderTransformProperty();
    bool replaced = s->shown && el.as<wuxc::Image>().Source() == s->shown;
    bool ours = s->transform && el.RenderTransform() == s->transform;
    if (!replaced) {
        if (ours) PutBack(el, dp);
        return;
    }
    float k = s->look == IconLook::Finder ? g_settings.customFinder.scale : kMacIconScale;
    float center = g_size.icon / 2;
    if (ours && s->transform.ScaleX() == k && s->transform.CenterX() == center) return;
    s->transform = wuxm::ScaleTransform();
    s->transform.ScaleX(k);
    s->transform.ScaleY(k);
    s->transform.CenterX(center);
    s->transform.CenterY(center);
    el.RenderTransform(s->transform);
}

void RequestSquircle(IconSwap& s, wuxc::Image const& image);
wuxm::ImageSource CachedSquircle(wuxm::ImageSource const& stock);
void NoteIconChange();

void ShowSwap(IconSwap& s, wuxc::Image const& image) {
    auto source = s.shown ? s.shown : s.stock;
    if (source && image.Source() != source) image.Source(source);
    ApplyIconSize(image);
    ApplyIconScale(image);
}

void UpdateSwap(IconSwap& s, wuxc::Image const& image) {
    if (s.look == IconLook::Finder) s.shown = FinderIconSource(image);
    if (s.look == IconLook::Squircle && !s.shown) s.shown = CachedSquircle(s.stock);
    ShowSwap(s, image);
    if (s.look == IconLook::Squircle && !s.shown) RequestSquircle(s, image);
}

// The taskbar set an icon: kept to put back, and replaced again.
void OnIconSourceChanged(wux::FrameworkElement const& el) {
    IconSwap* s = FindSwap(el);
    if (!s) return;
    auto image = el.as<wuxc::Image>();
    auto source = image.Source();
    if (source == s->shown || source == s->stock) return;
    NoteIconChange();
    auto dp = wuxc::Image::SourceProperty();
    if (Override* o = FindOverride(el, dp)) o->original = image.ReadLocalValue(dp);
    auto button = s->button.get();
    s->stock = source;
    s->look = button ? LookFor(button) : IconLook::Stock;
    s->shown = nullptr;
    s->retries = 0;
    UpdateSwap(*s, image);
}

void UpdateAppIcon(wux::FrameworkElement const& el, wux::FrameworkElement const& button) {
    auto image = el.try_as<wuxc::Image>();
    if (!image) return;
    auto dp = wuxc::Image::SourceProperty();
    IconLook look = LookFor(button);
    IconSwap* s = FindSwap(el);

    if (look == IconLook::Stock) {
        if (!s) return;
        g_iconSwaps.remove_if([&](IconSwap const& swap) { return &swap == s; });
        Release(el, dp);
        Release(el, wux::UIElement::RenderTransformProperty());
        ApplyIconSize(el);
        return;
    }
    if (!s) {
        // A binding couldn't be put back once replaced.
        auto original = image.ReadLocalValue(dp);
        if (original && original.try_as<wux::Data::BindingExpression>()) return;
        s = &g_iconSwaps.emplace_back();
        s->image = winrt::make_weak(image);
        s->stock = image.Source();
        NoteIconChange();
        Track(el, dp, OnIconSourceChanged);
        Track(el, wux::UIElement::RenderTransformProperty(), ApplyIconScale);
    }
    s->button = winrt::make_weak(button);
    if (s->look != look) {
        s->look = look;
        s->shown = nullptr;
        s->retries = 0;
    }
    UpdateSwap(*s, image);
}

// ---- Squircle tiles ----
// The icon's pixels, read straight from its bitmap, or else from the icon as
// drawn. If it's mostly opaque and square, it's a tile already: cropped to fill
// the squircle, but only if each side is flat between its corners, right up to
// its edge. Otherwise it's a shape, shrunk onto a tile of the Squircle color:
// for Auto, light, or dark if its outline is light.

constexpr int kAlphaShown = 64;              // counts as part of the icon
constexpr int kAlphaSolid = 224;             // counts as solid
constexpr float kTileMinCoverage = 0.84f;    // solid share of its bounds, to be a tile
constexpr float kTileMinAspect = 0.9f;
constexpr float kTileFlatFrom = 0.25f;       // a side's flat middle starts this far along it
constexpr float kTileFlatShare = 0.9f;       // of that middle, solid
constexpr float kTileEdgeDepth = 0.03f;      // how far in from its edge, of the icon's size
constexpr float kTileTrim = 0.015f;          // of a tile's own edge, cut off
constexpr float kShapeSize = 0.72f;          // a shape on its tile, of the tile's size
constexpr float kLongShapeSize = 0.8f;       // a long one, by its longer side
constexpr float kDarkTileFrom = 0.55f;       // a shape's edge lightness that gets a dark tile
constexpr float kTileCorner = 0.225f;        // corner radius, of the tile's size
constexpr float kTileShadowBlur = 0.012f;    // of the tile's size
constexpr float kTileShadowDrop = 0.01f;
constexpr float kTileShadowOpacity = 0.3f;
constexpr std::array<Color, 2> kLightTile{{{255, 0xFF, 0xFF, 0xFF}, {255, 0xE9, 0xE9, 0xEE}}};
constexpr std::array<Color, 2> kDarkTile{{{255, 0x4A, 0x4A, 0x4E}, {255, 0x1F, 0x1F, 0x22}}};
constexpr float kCustomTileShade = 0.08f;    // a custom tile's bottom, darker than its top
constexpr int kSquircleRetries = 30;         // while the icon isn't laid out or drawn yet
constexpr size_t kSquircleCacheSize = 64;

ULONG_PTR g_gdiplus = 0;

bool EnsureGdiplus() {
    Gdiplus::GdiplusStartupInput input;
    if (!g_gdiplus && Gdiplus::GdiplusStartup(&g_gdiplus, &input, nullptr) != Gdiplus::Ok) {
        g_gdiplus = 0;
    }
    return g_gdiplus != 0;
}

void ShutdownGdiplus() {
    if (g_gdiplus) Gdiplus::GdiplusShutdown(g_gdiplus);
    g_gdiplus = 0;
}

Gdiplus::Color GdiColor(Color c) { return Gdiplus::Color(c.A, c.R, c.G, c.B); }

// Three box blurs, close to a Gaussian blur with standard deviation `sigma`.
void Blur(std::vector<float>& v, int w, int h, float sigma) {
    int r = (int)std::lround((std::sqrt(4 * sigma * sigma + 1) - 1) / 2);
    if (r < 1) return;
    std::vector<float> tmp(v.size());
    auto pass = [r](float const* src, float* dst, int lines, int length, int step, int lineStep) {
        for (int line = 0; line < lines; line++) {
            float const* s = src + (size_t)line * lineStep;
            float* d = dst + (size_t)line * lineStep;
            float sum = 0;
            for (int k = -r; k <= r; k++) sum += s[std::clamp(k, 0, length - 1) * step];
            for (int i = 0; i < length; i++) {
                d[i * step] = sum / (2 * r + 1);
                sum += s[std::min(i + r + 1, length - 1) * step] - s[std::max(i - r, 0) * step];
            }
        }
    };
    for (int i = 0; i < 3; i++) {
        pass(v.data(), tmp.data(), h, w, 1, w);  // rows
        pass(tmp.data(), v.data(), w, h, w, 1);  // columns
    }
}

struct IconPixels {
    int size = 0;
    std::vector<BYTE> pixels;  // premultiplied BGRA
};

// A shape's tile colors, top to bottom.
std::array<Color, 2> TileColors(bool lightOutline) {
    switch (g_settings.tileColor) {
        case TileColor::Light: return kLightTile;
        case TileColor::Dark: return kDarkTile;
        case TileColor::Custom: {
            auto [r, g, b] = g_settings.tileCustomColor;
            Color top{255, r, g, b};
            return {top, ShiftLightness(top, -kCustomTileShade)};
        }
        case TileColor::Auto: break;
    }
    return lightOutline ? kDarkTile : kLightTile;
}

// A `w` x `h` icon as a squircle tile, on a canvas kMacIconScale times bigger.
IconPixels MakeSquircleIcon(std::vector<BYTE> source, int w, int h) {
    namespace gp = Gdiplus;
    int x0 = w, y0 = h, x1 = -1, y1 = -1;  // what's shown
    int sx0 = w, sy0 = h, sx1 = -1, sy1 = -1;  // what's solid
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            BYTE alpha = source[((size_t)y * w + x) * 4 + 3];
            if (alpha < kAlphaShown) continue;
            x0 = std::min(x0, x), x1 = std::max(x1, x);
            y0 = std::min(y0, y), y1 = std::max(y1, y);
            if (alpha < kAlphaSolid) continue;
            sx0 = std::min(sx0, x), sx1 = std::max(sx1, x);
            sy0 = std::min(sy0, y), sy1 = std::max(sy1, y);
        }
    }
    if (x1 < 0 || !EnsureGdiplus()) return {};

    // How much of its bounds is solid, and how light its outline is: its pixels
    // next to a transparent one, which are what meet the tile.
    auto shown = [&](int x, int y) {
        return x >= 0 && y >= 0 && x < w && y < h &&
               source[((size_t)y * w + x) * 4 + 3] >= kAlphaShown;
    };
    int bw = x1 - x0 + 1, bh = y1 - y0 + 1, solid = 0;
    double weight = 0, lightness = 0;
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            BYTE const* p = &source[((size_t)y * w + x) * 4];
            if (p[3] >= kAlphaSolid) solid++;
            if (!shown(x, y) ||
                (shown(x - 1, y) && shown(x + 1, y) && shown(x, y - 1) && shown(x, y + 1))) {
                continue;
            }
            lightness += (0.299 * p[2] + 0.587 * p[1] + 0.114 * p[0]) / 255;  // premultiplied
            weight += p[3] / 255.0;
        }
    }
    float aspect = (float)std::min(bw, bh) / std::max(bw, bh);

    // A side is flat if the middle of it, past any rounded corners, is solid
    // within a few pixels of the solid part's edge (soft edges and a baked-in
    // shadow aside).
    int depth = std::max(1, (int)std::round(std::max(bw, bh) * kTileEdgeDepth));
    auto flat = [&](bool vertical, bool end) {
        int length = vertical ? sy1 - sy0 + 1 : sx1 - sx0 + 1;
        int from = (int)(length * kTileFlatFrom), to = length - from, found = 0;
        for (int i = from; i < to; i++) {
            for (int d = 0; d < depth; d++) {
                int x = vertical ? (end ? sx1 - d : sx0 + d) : sx0 + i;
                int y = vertical ? sy0 + i : (end ? sy1 - d : sy0 + d);
                if (source[((size_t)y * w + x) * 4 + 3] >= kAlphaSolid) {
                    found++;
                    break;
                }
            }
        }
        return found >= kTileFlatShare * (to - from);
    };
    bool tile = aspect >= kTileMinAspect && solid >= kTileMinCoverage * bw * bh && sx1 >= 0 &&
                flat(false, false) && flat(false, true) && flat(true, false) && flat(true, true);
    bool light = lightness / weight >= kDarkTileFrom;

    int t = std::max(w, h), size = (int)std::ceil(t * kMacIconScale);
    float o = (size - t) / 2.0f;
    gp::Bitmap icon(w, h, w * 4, PixelFormat32bppPARGB, source.data());
    gp::Bitmap face(t, t, PixelFormat32bppPARGB);
    {
        gp::Graphics g(&face);
        g.SetInterpolationMode(gp::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(gp::PixelOffsetModeHalf);
        gp::ImageAttributes edges;
        edges.SetWrapMode(gp::WrapModeTileFlipXY);
        if (tile) {
            float trim = std::max(bw, bh) * kTileTrim;
            g.DrawImage(&icon, gp::RectF(0, 0, (gp::REAL)t, (gp::REAL)t), x0 + trim, y0 + trim,
                        bw - 2 * trim, bh - 2 * trim, gp::UnitPixel, &edges);
        } else {
            auto colors = TileColors(light);
            gp::LinearGradientBrush back(gp::PointF(0, -1), gp::PointF(0, (gp::REAL)t + 1),
                                         GdiColor(colors[0]), GdiColor(colors[1]));
            g.FillRectangle(&back, 0, 0, t, t);
            float k = t * (aspect >= 0.85f ? kShapeSize : kLongShapeSize) / std::max(bw, bh);
            gp::RectF at((t - bw * k) / 2, (t - bh * k) / 2, bw * k, bh * k);
            g.DrawImage(&icon, at, (gp::REAL)x0, (gp::REAL)y0, (gp::REAL)bw, (gp::REAL)bh,
                        gp::UnitPixel, &edges);
        }
    }

    std::vector<gp::PointF> points;
    for (auto& p : Squircle(o, o, (float)t, (float)t, kTileCorner * t, kTileCorner * t)) {
        points.push_back({p.X, p.Y});
    }
    gp::GraphicsPath path;
    path.AddBeziers(points.data(), (INT)points.size());
    path.CloseFigure();

    // The shadow, from the squircle blurred, then the face cut to it.
    IconPixels out{size, std::vector<BYTE>((size_t)size * size * 4)};
    {
        gp::Bitmap mask(size, size, size * 4, PixelFormat32bppPARGB, out.pixels.data());
        gp::Graphics g(&mask);
        g.SetSmoothingMode(gp::SmoothingModeAntiAlias);
        gp::SolidBrush black(gp::Color(255, 0, 0, 0));
        g.FillPath(&black, &path);
    }
    std::vector<float> shadow((size_t)size * size);
    for (size_t i = 0; i < shadow.size(); i++) shadow[i] = out.pixels[i * 4 + 3] / 255.0f;
    Blur(shadow, size, size, kTileShadowBlur * t);
    int drop = (int)std::lround(kTileShadowDrop * t);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            float a = y >= drop ? shadow[(size_t)(y - drop) * size + x] : 0;
            BYTE* p = &out.pixels[((size_t)y * size + x) * 4];
            p[0] = p[1] = p[2] = 0;
            p[3] = (BYTE)std::lround(a * kTileShadowOpacity * 255);
        }
    }
    {
        gp::Bitmap canvas(size, size, size * 4, PixelFormat32bppPARGB, out.pixels.data());
        gp::Graphics g(&canvas);
        g.SetSmoothingMode(gp::SmoothingModeAntiAlias);
        gp::TextureBrush brush(&face);
        brush.TranslateTransform(o, o);
        g.FillPath(&brush, &path);
    }
    return out;
}

[[clang::no_destroy]] std::list<std::pair<wuxm::ImageSource, wuxm::ImageSource>> g_squircleCache;

wuxm::ImageSource CachedSquircle(wuxm::ImageSource const& stock) {
    for (auto& [icon, tile] : g_squircleCache) {
        if (icon == stock) return tile;
    }
    return nullptr;
}

// Tiles are forgotten when their color changes since `old`, and drawn again as
// the buttons are next processed.
void RecolorSquircles(Settings const& old) {
    auto& s = g_settings;
    bool same = old.tileColor == s.tileColor &&
                (s.tileColor != TileColor::Custom ||
                 std::equal(std::begin(s.tileCustomColor), std::end(s.tileCustomColor),
                            std::begin(old.tileCustomColor)));
    if (same) return;
    g_squircleCache.clear();
    for (auto& swap : g_iconSwaps) {
        if (swap.look != IconLook::Squircle) continue;
        swap.shown = nullptr;
        swap.retries = 0;
    }
}

// ---- Settling ----
// Tiles are made from the icons as drawn, which are in flux while the dock
// starts up and the taskbar reloads its icons. So they wait till no icon has
// changed for a second, or 5 seconds at most. Again after a settings change.

constexpr auto kIconsQuietFor = std::chrono::milliseconds(1000);
constexpr auto kIconsSettleWithin = std::chrono::milliseconds(5000);

bool g_iconsSettling = true;
bool g_iconsSettleStarted = false;
std::chrono::steady_clock::time_point g_iconsSettleStart, g_iconsLastChange;
[[clang::no_destroy]] winrt::Windows::System::DispatcherQueueTimer g_iconsSettleTimer{nullptr};

void NoteIconChange() {
    g_iconsLastChange = std::chrono::steady_clock::now();
}

void SettleIconsAgain() {
    g_iconsSettling = true;
    g_iconsSettleStarted = false;
}

void StopSettlingIcons() {
    if (g_iconsSettleTimer) g_iconsSettleTimer.Stop();
    g_iconsSettleTimer = nullptr;
}

// Checked a few times a second; once settled, the waiting icons are drawn.
void WaitForIconsToSettle() {
    auto now = std::chrono::steady_clock::now();
    if (!g_iconsSettleStarted) {
        g_iconsSettleStarted = true;
        g_iconsSettleStart = g_iconsLastChange = now;
    }
    if (g_iconsSettleTimer) return;
    auto queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (!queue) {
        g_iconsSettling = false;
        return;
    }
    g_iconsSettleTimer = queue.CreateTimer();
    g_iconsSettleTimer.Interval(std::chrono::milliseconds(200));
    g_iconsSettleTimer.IsRepeating(true);
    g_iconsSettleTimer.Tick([](auto&&, auto&&) {
        if (g_unloading) return;
        auto now = std::chrono::steady_clock::now();
        if (now - g_iconsLastChange < kIconsQuietFor &&
            now - g_iconsSettleStart < kIconsSettleWithin) {
            return;
        }
        StopSettlingIcons();
        g_iconsSettling = false;
        for (auto& s : g_iconSwaps) {
            auto image = s.image.get();
            if (image && s.look == IconLook::Squircle && !s.shown) {
                Try([&] { UpdateSwap(s, image); }, L"Squircle icon");
            }
        }
    });
    g_iconsSettleTimer.Start();
}

// Async work's callbacks run code from the mod. Each holds a count, which
// unloading waits on after cancelling the work (see Wh_ModBeforeUninit).
std::atomic<int> g_pendingCallbacks = 0;

std::shared_ptr<void> TrackPendingCallback() {
    g_pendingCallbacks++;
    return std::shared_ptr<void>(nullptr, [](void*) { g_pendingCallbacks--; });
}

void CancelSquircleRenders() {
    for (auto& s : g_iconSwaps) {
        if (s.render) Try([&] { s.render.Cancel(); });
        s.render = nullptr;
    }
}

// Icons not drawn yet, tried again shortly.
void RetrySquircles() {
    for (auto& s : g_iconSwaps) {
        if (!s.retry) continue;
        s.retry = false;
        if (auto image = s.image.get()) Try([&] { UpdateSwap(s, image); }, L"Squircle icon");
    }
}

// Not laid out or drawn yet, as with a button still sliding in: tried again
// shortly, a limited number of times.
void RetrySquircleSoon(IconSwap& s) {
    if (++s.retries > kSquircleRetries) return;
    HWND window = EnsureModWindow();
    if (window && SetTimer(window, kSquircleRetryTimer, kSquircleRetryMs, nullptr)) s.retry = true;
}

void OnStockPixels(winrt::weak_ref<wuxc::Image> const& weak, wuxm::ImageSource const& stock,
                   std::vector<BYTE> pixels, int w, int h) {
    auto image = weak.get();
    IconSwap* s = image ? FindSwap(image) : nullptr;
    if (!s) return;
    s->rendering = false;
    s->render = nullptr;
    if (s->stock != stock || s->look != IconLook::Squircle) return UpdateSwap(*s, image);

    IconPixels tile;
    if (!pixels.empty() && w > 0 && h > 0) tile = MakeSquircleIcon(std::move(pixels), w, h);
    if (!tile.size) return RetrySquircleSoon(*s);
    s->shown = BitmapFromPixels(tile.size, tile.size, tile.pixels);
    g_squircleCache.emplace_front(stock, s->shown);
    if (g_squircleCache.size() > kSquircleCacheSize) g_squircleCache.pop_back();
    ShowSwap(*s, image);
}

std::vector<BYTE> BufferBytes(wss::IBuffer const& buffer) {
    BYTE* data = nullptr;
    winrt::check_hresult(buffer.as<IDockBufferByteAccess>()->Buffer(&data));
    return std::vector<BYTE>(data, data + buffer.Length());
}

// Its own pixels if it's a bitmap the mod can read, or else the icon as drawn,
// at its fully magnified size.
void RequestSquircle(IconSwap& s, wuxc::Image const& image) {
    if (s.rendering || !s.stock || s.retries > kSquircleRetries) return;
    if (g_iconsSettling) return WaitForIconsToSettle();
    if (image.Source() != s.stock) return;
    if (image.ActualWidth() <= 0) return RetrySquircleSoon(s);
    auto weak = winrt::make_weak(image);
    auto stock = s.stock;
    if (auto bitmap = stock.try_as<wuxm::Imaging::WriteableBitmap>()) {
        s.rendering = true;
        return OnStockPixels(weak, stock, BufferBytes(bitmap.PixelBuffer()), bitmap.PixelWidth(),
                             bitmap.PixelHeight());
    }
    auto queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (!queue) return;
    s.rendering = true;
    int px = (int)std::ceil(g_size.icon * g_settings.scale);
    wuxm::Imaging::RenderTargetBitmap target;
    auto render = target.RenderAsync(image, px, px);
    s.render = render;
    // Each callback holds the count till it's done, the next one included.
    render.Completed([=, pending = TrackPendingCallback()](wf::IAsyncAction const&,
                                                           wf::AsyncStatus status) {
        queue.TryEnqueue([=, pending = pending] {
            if (g_unloading) return;
            Try([&] {
                if (status != wf::AsyncStatus::Completed) {
                    return OnStockPixels(weak, stock, {}, 0, 0);
                }
                auto read = target.GetPixelsAsync();
                if (auto image = weak.get()) {
                    if (IconSwap* swap = FindSwap(image)) swap->render = read;
                }
                read.Completed([=, pending = pending](auto const& op, wf::AsyncStatus status) {
                    queue.TryEnqueue([=, pending = pending] {
                        if (g_unloading) return;
                        Try([&] {
                            std::vector<BYTE> pixels;
                            if (status == wf::AsyncStatus::Completed) {
                                pixels = BufferBytes(op.GetResults());
                            }
                            OnStockPixels(weak, stock, std::move(pixels), target.PixelWidth(),
                                          target.PixelHeight());
                        }, L"Squircle icon");
                    });
                });
            }, L"Squircle icon");
        });
    });
}

// =============================================================================
// RECYCLE BIN
// =============================================================================

std::atomic<int> g_trashFull = -1;  // -1 unknown, 0 empty, 1 full
std::atomic<bool> g_trashChecking = false;
PTP_WORK g_trashWork = nullptr;
ULONG g_trashNotify = 0;
[[clang::no_destroy]] wuxm::ImageSource g_trashIcons[2] = {nullptr, nullptr};

wuxm::ImageSource LoadStockTrashIcon(bool full, int px) {
    SHSTOCKICONINFO info{sizeof(info)};
    HICON icon = nullptr;
    if (FAILED(SHGetStockIconInfo(full ? SIID_RECYCLERFULL : SIID_RECYCLER, SHGSI_ICONLOCATION,
                                  &info)) ||
        SHDefExtractIconW(info.szPath, info.iIcon, 0, &icon, nullptr, px) != S_OK || !icon) {
        Wh_Log(L"Couldn't load Windows' Recycle Bin icon");
        return nullptr;
    }

    ICONINFO ii{};
    BITMAP bm{};
    std::vector<BYTE> pixels;
    if (GetIconInfo(icon, &ii)) {
        if (ii.hbmColor && GetObject(ii.hbmColor, sizeof(bm), &bm)) {
            BITMAPINFO bi{};
            bi.bmiHeader = {sizeof(BITMAPINFOHEADER), bm.bmWidth, -bm.bmHeight, 1, 32, BI_RGB};
            pixels.resize((size_t)bm.bmWidth * bm.bmHeight * 4);
            HDC dc = GetDC(nullptr);
            if (GetDIBits(dc, ii.hbmColor, 0, bm.bmHeight, pixels.data(), &bi, DIB_RGB_COLORS) !=
                bm.bmHeight) {
                pixels.clear();
            }
            ReleaseDC(nullptr, dc);
        }
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
        if (ii.hbmMask) DeleteObject(ii.hbmMask);
    }
    DestroyIcon(icon);
    if (pixels.empty()) return nullptr;

    for (size_t i = 0; i < pixels.size(); i += 4) {  // premultiply
        BYTE a = pixels[i + 3];
        for (int c = 0; c < 3; c++) pixels[i + c] = (BYTE)(pixels[i + c] * a / 255);
    }
    return BitmapFromPixels(bm.bmWidth, bm.bmHeight, pixels);
}

// Loaded once, at the size shown fully magnified, whatever "High resolution
// icons" is set to. Windows' own without a custom image, or if it's unreadable.
wuxm::ImageSource TrashIcon(bool full, wux::UIElement const& el) {
    auto& icon = g_trashIcons[full];
    if (icon) return icon;
    int px = MagnifiedPixels(el, TrashIconScale());
    icon = LoadImageFile(TrashPath(full), px);
    if (!icon) icon = LoadStockTrashIcon(full, px);
    return icon;
}

void UpdateTrashIcons() {
    int full = g_trashFull;
    for (auto& p : *g_plates) {
        if (!p.trashImage || p.trashState == full) continue;
        p.trashState = full;
        Try([&] { p.trashImage.Source(TrashIcon(full == 1, p.trashImage)); }, L"Recycle Bin icon");
    }
}

// ---- Watching it ----

void CALLBACK CheckRecycleBin(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK) {
    HRESULT co = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    SHQUERYRBINFO info{sizeof(info)};
    if (SUCCEEDED(SHQueryRecycleBinW(nullptr, &info))) g_trashFull = info.i64NumItems > 0;
    if (SUCCEEDED(co)) CoUninitialize();
    g_trashChecking = false;
    if (HWND w = g_modWindow) PostMessage(w, kTrashCheckedMessage, 0, 0);
}

void CheckRecycleBinSoon() {
    if (g_trashWork && !g_trashChecking.exchange(true)) SubmitThreadpoolWork(g_trashWork);
}

void StartTrash() {
    if (!g_trashWork) g_trashWork = CreateThreadpoolWork(CheckRecycleBin, nullptr, nullptr);
    HWND w = EnsureModWindow();
    CheckRecycleBinSoon();
    if (g_trashNotify) return;
    PIDLIST_ABSOLUTE bin;
    if (!w || FAILED(SHGetFolderLocation(nullptr, CSIDL_BITBUCKET, nullptr, 0, &bin))) return;
    SHChangeNotifyEntry entry{bin, TRUE};
    g_trashNotify = SHChangeNotifyRegister(w, SHCNRF_ShellLevel, SHCNE_ALLEVENTS,
                                           kTrashChangedMessage, 1, &entry);
    CoTaskMemFree(bin);
    if (!g_trashNotify) Wh_Log(L"Can't watch the Recycle Bin for changes");
}

// A check still running is waited for in Wh_ModBeforeUninit.
void StopTrash() {
    if (g_trashNotify) {
        SHChangeNotifyDeregister(g_trashNotify);
        g_trashNotify = 0;
    }
    g_trashIcons[0] = g_trashIcons[1] = nullptr;
}

// ---- Using it ----

void OpenRecycleBin() {
    ShellExecuteW(nullptr, L"open", L"shell:RecycleBinFolder", nullptr, nullptr, SW_SHOWNORMAL);
}

// `paths` are null-terminated, one after another. Run as a shell job.
void RecycleFiles(std::wstring const& paths) {
    SHFILEOPSTRUCTW op{};
    op.wFunc = FO_DELETE;
    op.pFrom = paths.c_str();
    op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_WANTNUKEWARNING;
    if (int error = SHFileOperationW(&op)) Wh_Log(L"Recycling failed: %d", error);
    CheckRecycleBinSoon();
}

void EmptyRecycleBin() {
    SHEmptyRecycleBinW(nullptr, nullptr, 0);
    CheckRecycleBinSoon();
}

void SuppressLabel();
void StartFrames();

// The Recycle Bin whose menu is open. The menu takes the pointer from the
// taskbar, so the frame loop follows it instead.
winrt::weak_ref<wux::UIElement> g_trashMenuFor;

void ShowTrashMenu(wux::FrameworkElement const& anchor) {
    SuppressLabel();
    wuxc::MenuFlyout menu;
    auto add = [&](PCWSTR text, bool enabled, void (*action)()) {
        wuxc::MenuFlyoutItem item;
        item.Text(text);
        item.IsEnabled(enabled);
        item.Click([action](wf::IInspectable const&, wux::RoutedEventArgs const&) { action(); });
        menu.Items().Append(item);
    };
    add(L"Open", true, OpenRecycleBin);
    add(HasTrashIcon() ? L"Empty Trash" : L"Empty Recycle Bin", g_trashFull != 0,
        [] { QueueShellJob(EmptyRecycleBin); });
    menu.Placement(wuxc::Primitives::FlyoutPlacementMode::Top);
    menu.Opened([trash = winrt::make_weak(wuxm::VisualTreeHelper::GetParent(anchor)
                                              .as<wux::UIElement>())](auto&&, auto&&) {
        g_trashMenuFor = trash;
        StartFrames();
    });
    menu.Closed([](auto&&, auto&&) { g_trashMenuFor = nullptr; });
    menu.ShowAt(anchor);
}

void AcceptTrashDrag(wux::DragEventArgs const& e) {
    if (!e.DataView().Contains(wadt::StandardDataFormats::StorageItems())) return;
    e.AcceptedOperation(wadt::DataPackageOperation::Move);
    if (auto ui = e.DragUIOverride()) {
        ui.Caption(HasTrashIcon() ? L"Move to Trash" : L"Move to Recycle Bin");
    }
    e.Handled(true);
}

// Finishes right away: the source window waits for the drop, and the paths are
// read from it, so waiting would freeze both. Reporting no operation keeps the
// source from deleting the files itself.
void OnTrashDrop(wux::DragEventArgs const& e) {
    e.Handled(true);
    e.AcceptedOperation(wadt::DataPackageOperation::None);
    if (!e.DataView().Contains(wadt::StandardDataFormats::StorageItems())) return;
    QueueShellJob([items = winrt::make_agile(e.DataView().GetStorageItemsAsync())] {
        std::wstring paths;
        try {
            for (auto const& item : items.get().get()) {
                auto path = item.Path();
                if (path.empty()) continue;
                paths.append(path.c_str(), path.size());
                paths.push_back(L'\0');
            }
        } catch (...) {
            Wh_Log(L"Reading the dropped files failed: 0x%08X", winrt::to_hresult());
            return;
        }
        if (!paths.empty()) RecycleFiles(paths);
    });
}

void HookUpTrash(Plate& p) {
    p.trash.AllowDrop(true);
    p.trashTapped = p.trash.Tapped(
        winrt::auto_revoke, [](wf::IInspectable const&, wuxi::TappedRoutedEventArgs const& e) {
            e.Handled(true);
            OpenRecycleBin();
        });
    p.trashRightTapped = p.trash.RightTapped(
        winrt::auto_revoke,
        [anchor = winrt::make_weak(p.trashMenuAnchor)](
            wf::IInspectable const&, wuxi::RightTappedRoutedEventArgs const& e) {
            e.Handled(true);
            if (auto el = anchor.get()) Try([&] { ShowTrashMenu(el); });
        });
    auto accept = [](wf::IInspectable const&, wux::DragEventArgs const& e) {
        Try([&] { AcceptTrashDrag(e); });
    };
    p.trashDragEnter = p.trash.DragEnter(winrt::auto_revoke, accept);
    p.trashDragOver = p.trash.DragOver(winrt::auto_revoke, accept);
    p.trashDrop = p.trash.Drop(
        winrt::auto_revoke, [](wf::IInspectable const&, wux::DragEventArgs const& e) {
            Try([&] { OnTrashDrop(e); }, L"Recycle Bin drop");
        });
}

void AddTrash(Plate& p, wuxc::Panel const& panel, wux::UIElement const& row) {
    float size = g_size.icon * TrashIconScale();
    wuxc::Grid trash;
    PlaceInPanel(panel, trash);
    trash.Width(size);
    trash.Height(size);
    trash.Background(wuxm::SolidColorBrush(Color{0, 0, 0, 0}));  // clickable everywhere
    wuxc::Image image;
    image.Stretch(wuxm::Stretch::Uniform);
    trash.Children().Append(image);

    // Its menu opens above this, the width of the icon.
    wuxc::Border anchor;
    anchor.VerticalAlignment(wux::VerticalAlignment::Top);
    anchor.Height(0);
    anchor.Margin({0, -kTrashMenuLift * g_size.icon / kDefaultIconSize, 0, 0});
    anchor.IsHitTestVisible(false);
    trash.Children().Append(anchor);
    if (!InsertBesideRow(panel, row, trash)) return;

    p.trash = trash;
    p.trashImage = image;
    p.trashMenuAnchor = anchor;
    p.divider = AddDivider(panel, row);
    HookUpTrash(p);
    StartTrash();
}

// Grows from its bottom edge like a button, and shifts with its divider.
void SetTrashScale(Plate& p, float s, float tx, float dividerTx) {
    if (!p.trash) return;
    float size = g_size.icon * TrashIconScale();
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(p.trash);
    visual.CenterPoint({size / 2, size / 2 + g_size.icon / 2, 0});
    visual.Scale({s, s, 1});
    SetTranslation(p.trash, tx);
    SetTranslation(p.divider, dividerTx);
}

// =============================================================================
// RUNNING INDICATORS
// =============================================================================
// A round dot in place of Windows' pill. It lives beside the dock, not in the
// button, so it follows its icon sideways without growing. With "Running
// indicators" set to Windows 11, Windows' own pill shows instead, or with
// Hybrid dots, that pill made round. Its progress bar always shows. Windows'
// own are moved up under the icon.

struct Indicator {
    winrt::weak_ref<wux::FrameworkElement> button;
    winrt::weak_ref<wux::FrameworkElement> element;  // Windows' indicator
    winrt::weak_ref<wux::VisualStateGroup> states;
    wux::VisualStateGroup::CurrentStateChanged_revoker stateChanged;
    wux::FrameworkElement::SizeChanged_revoker sizeChanged;
    winrt::weak_ref<wux::FrameworkElement> progress;  // Windows' progress bar
    wux::FrameworkElement::SizeChanged_revoker progressSizeChanged;
    int64_t progressVisibilityToken = 0;
    wuxc::Border dot{nullptr};
    winrt::weak_ref<wuxc::Panel> dotPanel;
    float dotLeft = NAN, dotTop = NAN, dotShift = NAN;  // last applied
};

[[clang::no_destroy]] std::optional<std::list<Indicator>> g_indicators{std::in_place};

// Set while the plates are measured. Dots are only added then, as the taskbar
// doesn't like its tree changing while it updates a button.
bool g_measuringPlates = false;

// While an app's dragged (the taskbar's own drag), nothing's magnified and the
// dots are hidden.
bool g_dragging = false;

void ReapplyButton(wux::FrameworkElement const& button);

Indicator* FindIndicator(wux::FrameworkElement const& button) {
    if (!g_indicators) return nullptr;
    for (auto& i : *g_indicators) {
        if (i.button.get() == button) return &i;
    }
    return nullptr;
}

Indicator& IndicatorFor(wux::FrameworkElement const& button) {
    if (Indicator* i = FindIndicator(button)) return *i;
    auto& i = g_indicators->emplace_back();
    i.button = winrt::make_weak(button);
    return i;
}

void RefreshButton(winrt::weak_ref<wux::FrameworkElement> const& weak) {
    Try([&] {
        if (auto button = weak.get()) ReapplyButton(button);
    });
}

bool ProgressShown(Indicator const& i) {
    auto el = i.progress.get();
    return el && el.Visibility() == Visibility::Visible && el.ActualWidth() > 0;
}

constexpr float kDotSize = 4;     // the same at every Size, as on a Mac
constexpr float kDotRise = 0.4f;  // its center, up from the dock's bottom, of the padding

wux::VisualStateGroup FindRunningStates(wux::FrameworkElement const& indicator,
                                        wux::FrameworkElement const& button) {
    for (auto current = wuxm::VisualTreeHelper::GetParent(indicator); current;
         current = wuxm::VisualTreeHelper::GetParent(current)) {
        if (auto fe = current.try_as<wux::FrameworkElement>()) {
            for (auto group : wux::VisualStateManager::GetVisualStateGroups(fe)) {
                if (group.Name() == L"RunningIndicatorStates") return group;
            }
        }
        if (current == button) break;
    }
    return nullptr;
}

bool IsRunning(Indicator& i) {
    auto group = i.states.get();
    if (auto state = group ? group.CurrentState() : nullptr) {
        winrt::hstring name = state.Name();
        if (name == L"ActiveRunningIndicator" || name == L"InactiveRunningIndicator" ||
            name == L"RequestingAttentionRunningIndicator") {
            return true;
        }
        if (std::wstring_view(name).find(L"RunningIndicator") != std::wstring_view::npos) {
            return false;  // another of its states
        }
    }
    // No states to go by: whether Windows shows its indicator.
    auto el = i.element.get();
    return el && el.Visibility() == Visibility::Visible && el.Opacity() > 0.01 &&
           el.ActualWidth() > 0 && el.ActualHeight() > 0;
}

void HideDot(Indicator& i) {
    if (i.dot && i.dot.Visibility() != Visibility::Collapsed) {
        i.dot.Visibility(Visibility::Collapsed);
    }
}

void RemoveDot(Indicator& i) {
    if (!i.dot) return;
    Try([&] {
        uint32_t index;
        auto panel = i.dotPanel.get();
        if (panel && panel.Children().IndexOf(i.dot, index)) panel.Children().RemoveAt(index);
    });
    i.dot = nullptr;
    i.dotPanel = nullptr;
}

// Where indicators are centered: under the icon, in the dock's bottom padding.
float IndicatorCenterY(Plate const& p) {
    return p.top + g_size.dockHeight - g_size.padY * kDotRise;
}

// Windows' pill or progress bar, in its button: moved to the dots' spot at its
// own size, undoing the button's magnification `s` (see SetScale).
void PlaceStockIndicator(wux::FrameworkElement const& el, wux::FrameworkElement const& button,
                         float s) {
    Plate* p = GetRowPlate(wuxm::VisualTreeHelper::GetParent(button));
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
    float width = (float)el.ActualWidth(), height = (float)el.ActualHeight();
    float centerY = height / 2;  // in the button, as laid out
    wux::DependencyObject current = el;
    while (current && current != button) {
        if (auto ui = current.try_as<wux::UIElement>()) centerY += ui.ActualOffset().y;
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    if (!p || !current || s <= 0) {
        visual.Scale({1, 1, 1});
        return SetTranslation(el, 0, 0);
    }
    auto slot = wuxc::Primitives::LayoutInformation::GetLayoutSlot(button);
    float buttonTop = p->originY + slot.Y + (float)button.Margin().Top;
    float pivotY = (float)button.ActualHeight() / 2 + g_size.icon / 2;
    float y = pivotY + (IndicatorCenterY(*p) - p->offsetY - buttonTop - pivotY) / s;
    visual.CenterPoint({width / 2, height / 2, 0});
    visual.Scale({1 / s, 1 / s, 1});
    SetTranslation(el, 0, y - centerY);
}

// ---- Hybrid dots ----
// Windows' own pill made round (as in notdominicc's version), so its states
// still show: the active app in the accent color, an app asking for attention
// in orange. Those states only set its fill; otherwise it's the dots' color.

void ApplyHybridDotShape(wux::FrameworkElement const& el) {
    if (el.Width() != kDotSize) el.Width(kDotSize);
    if (el.Height() != kDotSize) el.Height(kDotSize);
    if (auto r = el.try_as<wuxs::Rectangle>()) {
        if (r.RadiusX() != kDotSize / 2) r.RadiusX(kDotSize / 2);
        if (r.RadiusY() != kDotSize / 2) r.RadiusY(kDotSize / 2);
    }
}

void ShapeHybridDot(wux::FrameworkElement const& el) {
    if (el.try_as<wuxs::Shape>()) Track(el, wuxs::Shape::FillProperty());  // see ColorHybridDot
    Track(el, wux::FrameworkElement::WidthProperty(), ApplyHybridDotShape);
    Track(el, wux::FrameworkElement::HeightProperty(), ApplyHybridDotShape);
    if (el.try_as<wuxs::Rectangle>()) {
        Track(el, wuxs::Rectangle::RadiusXProperty(), ApplyHybridDotShape);
        Track(el, wuxs::Rectangle::RadiusYProperty(), ApplyHybridDotShape);
    }
    ApplyHybridDotShape(el);
}

void UnshapeHybridDot(wux::FrameworkElement const& el) {
    if (!FindOverride(el, wux::FrameworkElement::WidthProperty())) return;
    for (auto& dp : {wux::FrameworkElement::WidthProperty(),
                     wux::FrameworkElement::HeightProperty(), wuxs::Rectangle::RadiusXProperty(),
                     wuxs::Rectangle::RadiusYProperty(), wuxs::Shape::FillProperty()}) {
        Release(el, dp);
    }
}

// Its own fill, under the states' colors. Tracked by ShapeHybridDot.
void ColorHybridDot(wux::FrameworkElement const& el, wux::FrameworkElement const& button) {
    auto shape = el.try_as<wuxs::Shape>();
    Plate* p = GetRowPlate(wuxm::VisualTreeHelper::GetParent(button));
    if (!shape || !p) return;
    auto local = el.ReadLocalValue(wuxs::Shape::FillProperty()).try_as<wuxm::SolidColorBrush>();
    if (!local || local.Color() != p->dotColor) shape.Fill(wuxm::SolidColorBrush(p->dotColor));
}

void PlaceStockIndicators(wux::FrameworkElement const& button, float s) {
    Indicator* i = FindIndicator(button);
    if (!i) return;
    if (auto progress = i->progress.get()) {
        Try([&] { PlaceStockIndicator(progress, button, s); });
    }
    auto el = i->element.get();
    if (!el || g_settings.indicators == Indicators::Dots) return;
    Try([&] {
        PlaceStockIndicator(el, button, s);
        if (g_settings.indicators == Indicators::HybridDots) ColorHybridDot(el, button);
    });
}

// Centered under the icon. `tx` is how far magnification moves the icon
// sideways. Not while Windows' own pill or progress bar shows there.
void PlaceDot(wux::FrameworkElement const& button, float tx) {
    Indicator* i = FindIndicator(button);
    if (!i) return;
    Plate* p = GetRowPlate(wuxm::VisualTreeHelper::GetParent(button));
    auto panel = p ? p->panel.get() : nullptr;
    auto row = p ? p->row.get() : nullptr;
    if (!panel || !row || p->baseWidth <= 0 || button.Visibility() != Visibility::Visible ||
        button.ActualWidth() <= 0 || !IsRunning(*i) || g_dragging ||
        g_settings.indicators != Indicators::Dots || ProgressShown(*i)) {
        return HideDot(*i);
    }
    auto slot = wuxc::Primitives::LayoutInformation::GetLayoutSlot(button);
    auto margin = button.Margin();
    float y = slot.Y + (float)margin.Top;
    if (y < -button.ActualHeight() || y > button.ActualHeight()) return HideDot(*i);

    if (i->dot && i->dotPanel.get() != panel) RemoveDot(*i);
    if (!i->dot) {
        if (!g_measuringPlates) return MarkPlatesDirty();
        wuxc::Border dot;
        PlaceInPanel(panel, dot);
        dot.IsHitTestVisible(false);
        dot.UseLayoutRounding(false);
        dot.Background(wuxm::SolidColorBrush(p->dotColor));
        if (!InsertBesideRow(panel, row, dot, false)) return;  // under icons sliding by
        i->dot = dot;
        i->dotPanel = winrt::make_weak(panel);
        i->dotLeft = i->dotTop = i->dotShift = NAN;
    }

    float size = kDotSize;
    if (i->dot.Width() != size) {
        i->dot.Width(size);
        i->dot.Height(size);
        i->dot.CornerRadius({size / 2, size / 2, size / 2, size / 2});
    }
    float centerX = p->originX + slot.X + (float)margin.Left + (float)button.ActualWidth() / 2;
    float centerY = IndicatorCenterY(*p);
    float left = std::round(centerX - size / 2);
    float top = std::round(centerY - size / 2);
    if (left != i->dotLeft || top != i->dotTop) {
        i->dot.Margin({left, top, 0, 0});
        i->dotLeft = left;
        i->dotTop = top;
    }
    if (tx != i->dotShift) {
        SetTranslation(i->dot, tx);
        i->dotShift = tx;
    }
    if (i->dot.Visibility() != Visibility::Visible) i->dot.Visibility(Visibility::Visible);
}

void ColorDots(Plate& p) {
    auto panel = p.panel.get();
    if (!panel || !g_indicators) return;
    for (auto& i : *g_indicators) {
        if (i.dot && i.dotPanel.get() == panel) i.dot.Background(wuxm::SolidColorBrush(p.dotColor));
        auto el = i.element.get();
        auto button = i.button.get();
        if (el && button && g_settings.indicators == Indicators::HybridDots) {
            Try([&] { ColorHybridDot(el, button); });
        }
    }
}

// Hides dots in `panel` whose buttons aren't among `icons`.
void HideDots() {
    for (auto& i : *g_indicators) HideDot(i);
}

// The taskbar lifts a dragged app by growing it; it's kept at its size and
// dimmed instead.
constexpr double kDraggedOpacity = 0.7;

winrt::weak_ref<wux::FrameworkElement> g_draggedButton, g_draggedIcon;
double g_draggedIconOpacity = 1;

void FlattenDragged() {
    auto button = g_draggedButton.get();
    if (!button) return;
    std::vector<wux::UIElement> parts{button};
    if (wuxm::VisualTreeHelper::GetChildrenCount(button) > 0) {
        if (auto root = wuxm::VisualTreeHelper::GetChild(button, 0).try_as<wux::UIElement>()) {
            parts.push_back(root);
        }
    }
    if (auto icon = g_draggedIcon.get()) parts.push_back(icon);
    for (auto& part : parts) {
        auto visual = wuxh::ElementCompositionPreview::GetElementVisual(part);
        for (PCWSTR property : {L"Scale", L"Scale.X", L"Scale.Y"}) visual.StopAnimation(property);
        if (visual.Scale() != wf::Numerics::float3{1, 1, 1}) visual.Scale({1, 1, 1});
    }
}

void DimDragged(wux::FrameworkElement const& button) {
    g_draggedButton = winrt::make_weak(button);
    g_draggedIcon = nullptr;
    ForEachDescendant(button, 6, [](wux::FrameworkElement const& el) {
        if (g_draggedIcon.get() || el.Name() != L"Icon") return true;
        g_draggedIcon = winrt::make_weak(el);
        g_draggedIconOpacity = el.Opacity();
        el.Opacity(kDraggedOpacity);
        return false;
    });
    FlattenDragged();
}

void UndimDragged() {
    if (auto icon = g_draggedIcon.get()) icon.Opacity(g_draggedIconOpacity);
    g_draggedButton = g_draggedIcon = nullptr;
}

void EndDragging() {
    UndimDragged();
    if (!g_dragging) return;
    g_dragging = false;
    for (auto& b : *g_buttons) {
        if (auto el = b.element.get()) ReapplyButton(el);  // its dot back
    }
    MarkPlatesDirty();
}

// Ended without word from the taskbar, once the button's up.
void CheckDragging() {
    int primary = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
    if (g_dragging && !(GetAsyncKeyState(primary) & 0x8000)) EndDragging();
}

void HideStrayDots(wuxc::Panel const& panel, std::vector<Icon> const& icons) {
    for (auto& i : *g_indicators) {
        if (!i.dot || i.dotPanel.get() != panel) continue;
        auto button = i.button.get();
        if (!button || std::none_of(icons.begin(), icons.end(),
                                    [&](Icon const& icon) { return icon.element == button; })) {
            HideDot(i);
        }
    }
}

// Hides Windows' indicator (unless it's the one shown) and follows its state.
// The taskbar can replace it.
void TrackIndicator(wux::FrameworkElement const& el, wux::FrameworkElement const& button) {
    float opacity = g_settings.indicators == Indicators::Dots ? 0.0f : 1.0f;
    wuxh::ElementCompositionPreview::GetElementVisual(el).Opacity(opacity);
    if (g_settings.indicators == Indicators::HybridDots) ShapeHybridDot(el);
    else UnshapeHybridDot(el);
    Indicator& i = IndicatorFor(button);
    if (i.element.get() == el) return;
    i.element = winrt::make_weak(el);
    i.stateChanged.revoke();
    i.states = nullptr;
    if (auto group = FindRunningStates(el, button)) {
        i.states = winrt::make_weak(group);
        i.stateChanged = group.CurrentStateChanged(
            winrt::auto_revoke,
            [weak = i.button](wf::IInspectable const&, wux::VisualStateChangedEventArgs const&) {
                RefreshButton(weak);
            });
    }
    i.sizeChanged = el.SizeChanged(
        winrt::auto_revoke, [weak = i.button](wf::IInspectable const&,
                                              wux::SizeChangedEventArgs const&) {
            RefreshButton(weak);
        });
    MarkPlatesDirty();
}

// Followed as it appears, disappears and resizes.
void TrackProgress(wux::FrameworkElement const& el, wux::FrameworkElement const& button) {
    Indicator& i = IndicatorFor(button);
    if (i.progress.get() == el) return;
    if (auto old = i.progress.get(); old && i.progressVisibilityToken) {
        old.UnregisterPropertyChangedCallback(wux::UIElement::VisibilityProperty(),
                                              i.progressVisibilityToken);
    }
    i.progress = winrt::make_weak(el);
    i.progressSizeChanged = el.SizeChanged(
        winrt::auto_revoke, [weak = i.button](wf::IInspectable const&,
                                              wux::SizeChangedEventArgs const&) {
            RefreshButton(weak);
        });
    i.progressVisibilityToken = el.RegisterPropertyChangedCallback(
        wux::UIElement::VisibilityProperty(),
        [weak = i.button](wux::DependencyObject const&, wux::DependencyProperty const&) {
            RefreshButton(weak);
        });
    MarkPlatesDirty();
}

void ForgetGoneIndicators() {
    g_indicators->remove_if([](Indicator& i) {
        if (i.button.get()) return false;
        RemoveDot(i);
        return true;
    });
}

void RestoreStockIndicator(wux::FrameworkElement const& el) {
    Try([&] {
        auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
        visual.Opacity(1);
        visual.Scale({1, 1, 1});
        SetTranslation(el, 0, 0);
    });
}

void RestoreIndicators() {
    for (auto& i : *g_indicators) {
        i.stateChanged.revoke();
        i.sizeChanged.revoke();
        i.progressSizeChanged.revoke();
        RemoveDot(i);
        if (auto el = i.element.get()) RestoreStockIndicator(el);
        if (auto el = i.progress.get()) {
            Try([&] {
                el.UnregisterPropertyChangedCallback(wux::UIElement::VisibilityProperty(),
                                                     i.progressVisibilityToken);
            });
            RestoreStockIndicator(el);
        }
    }
    g_indicators->clear();
}

// =============================================================================
// MAGNIFICATION
// =============================================================================

// The row under the pointer, re-read every frame.
struct RowItem {
    winrt::weak_ref<wux::FrameworkElement> element;
    float center = 0;  // in the row
    float width = 0;
    float scale = 1.0f;
    float shift = 0;
    bool trash = false;
};

std::vector<RowItem> g_row;
winrt::weak_ref<wux::UIElement> g_rowParent;

bool g_pointerOver = false;
float g_pointerX = 0;
bool g_dockExpanded = false;  // magnified, or about to be
bool g_anchorLeftEnd = true;  // which end stays put as the dock grows
float g_maxGrowth = 0, g_lastGrowth = 0;

// Scales the icon from its bottom edge, shifts it by `tx`, and moves it down
// onto the dock.
void SetScale(wux::FrameworkElement const& el, float s, float tx = 0) {
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
    float pivotY = (float)el.ActualHeight() / 2 + g_size.icon / 2;
    visual.CenterPoint({(float)el.ActualWidth() / 2, pivotY, 0});
    visual.Scale({s, s, 1});
    SetTranslation(el, tx, GetRowOffsetY(wuxm::VisualTreeHelper::GetParent(el)));
    PlaceDot(el, tx);
    PlaceStockIndicators(el, s);
}

void ReapplyButton(wux::FrameworkElement const& button) {
    for (auto& item : g_row) {
        if (item.element.get() == button) return SetScale(button, item.scale, item.shift);
    }
    SetScale(button, 1.0f);
}

void SetAppsDividerShift(Plate& p, float tx) {
    if (!p.appsDivider || std::fabs(tx - p.appsDividerShift) < 0.01f) return;
    p.appsDividerShift = tx;
    SetTranslation(p.appsDivider, tx);
}

// (Re)builds the row in `parent`, keeping sizes in progress. Icons that left
// it go back to normal size.
void BuildRow(wux::UIElement const& parent) {
    std::vector<RowItem> old = std::move(g_row);
    Plate* oldPlate = GetRowPlate(g_rowParent.get());
    if (g_rowParent.get() != parent) g_maxGrowth = g_lastGrowth = 0;
    g_row.clear();
    g_rowParent = nullptr;

    // Carries over an item's size in progress, if it was in the row.
    auto carryOver = [&old](RowItem& item, auto matches) {
        for (auto& o : old) {
            if (!matches(o)) continue;
            item.scale = o.scale;
            item.shift = o.shift;
            o = RowItem{};  // still in the row; don't reset it below
            return;
        }
    };

    if (parent) {
        for (auto& icon : GetIcons(parent)) {
            float pitch = std::max(1.0f, icon.width + 2 * g_size.buttonMargin);
            RowItem item{winrt::make_weak(icon.element), icon.left + icon.width / 2, pitch};
            carryOver(item, [&](RowItem const& o) { return o.element.get() == icon.element; });
            g_row.push_back(item);
        }
        std::sort(g_row.begin(), g_row.end(),
                  [](RowItem const& a, RowItem const& b) { return a.center < b.center; });
        g_rowParent = winrt::make_weak(parent);

        Plate* p = GetRowPlate(parent);
        if (p && p->trash && !g_row.empty() && p->trash.Visibility() == Visibility::Visible) {
            RowItem item{nullptr, p->trashCenter, g_size.trashSlot};
            item.trash = true;
            if (oldPlate == p) carryOver(item, [](RowItem const& o) { return o.trash; });
            g_row.push_back(item);
        }
    }

    for (auto& o : old) {
        if (auto icon = o.element.get()) SetScale(icon, 1.0f);
        if (o.trash && oldPlate) SetTrashScale(*oldPlate, 1.0f, 0, 0);
    }
    if (oldPlate && oldPlate != GetRowPlate(parent)) SetAppsDividerShift(*oldPlate, 0);
}

// =============================================================================
// LAYOUT
// =============================================================================

void MoveExplorerFirst(wux::FrameworkElement const& explorer);
void NoteExplorerFirst();

// The bin's room past the last app, as the row's margin, not the last button's,
// so the taskbar sees each app at its real size when dragging. Kept on top of
// any margin the taskbar sets.
void ApplyRowMargin(wux::FrameworkElement const& row) {
    Plate* p = GetRowPlate(row);
    if (!p) return;
    auto current = row.Margin();
    auto want = p->rowMargin;
    want.Right += p->rowRoom;
    if (current == want) return;
    if (current != p->rowMargin) p->rowMargin = current;  // the taskbar set its own
    want = p->rowMargin;
    want.Right += p->rowRoom;
    row.Margin(want);
}

void ApplyRowRoom(Plate& p, float room) {
    auto row = p.row.get().try_as<wux::FrameworkElement>();
    if (!row) return;
    auto dp = wux::FrameworkElement::MarginProperty();
    if (!FindOverride(row, dp)) {
        p.rowMargin = row.Margin();
        Track(row, dp, ApplyRowMargin);
    }
    p.rowRoom = room;
    auto want = p.rowMargin;
    want.Right += room;
    if (row.Margin() != want) row.Margin(want);
}

// Room for the apps divider, as extra margin. File Explorer is moved first for
// real, if it isn't. Returns the buttons the bin and the apps divider follow.
std::pair<wux::FrameworkElement, wux::FrameworkElement> LayOutButtons(
    Plate& p, std::vector<Icon> const& icons) {
    // The taskbar's own order.
    std::vector<size_t> order(icons.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(),
              [&](size_t a, size_t b) { return icons[a].slotLeft < icons[b].slotLeft; });

    constexpr size_t kNone = SIZE_MAX;
    size_t firstApp = kNone, explorer = kNone;
    for (size_t i : order) {
        if (!IsAppButton(icons[i].element)) continue;
        if (firstApp == kNone) firstApp = i;
        if (explorer == kNone && IsFileExplorer(icons[i].element)) explorer = i;
    }
    if (g_settings.explorerFirst && explorer != kNone) {
        if (explorer != firstApp) {
            if (!g_dragging) MoveExplorerFirst(icons[explorer].element);
        } else {
            NoteExplorerFirst();
        }
    }

    std::vector<float> left(icons.size()), right(icons.size());
    size_t last = order.back();
    ApplyRowRoom(p, p.trash ? g_size.trashSpace : 0);

    // After Start, Task View and a leading File Explorer, with apps after it.
    wux::FrameworkElement beforeAppsDivider{nullptr};
    if (p.appsDivider) {
        size_t i = 0;
        while (i < order.size() && !IsAppButton(icons[order[i]].element)) i++;
        if (i < order.size() && IsFileExplorer(icons[order[i]].element)) i++;
        if (i > 0 && i < order.size()) {
            beforeAppsDivider = icons[order[i - 1]].element;
            right[order[i - 1]] += g_size.dividerSpace;
        }
    }

    bool changed = false;
    for (size_t i = 0; i < icons.size(); i++) {
        Button* b = FindButton(icons[i].element);
        if (!b || (std::fabs(b->extraLeft - left[i]) < 0.5f &&
                   std::fabs(b->extraRight - right[i]) < 0.5f)) {
            continue;
        }
        b->extraLeft = left[i];
        b->extraRight = right[i];
        ApplySpacing(icons[i].element);
        changed = true;
    }
    if (changed) MarkPlatesDirty();
    return {icons[last].element, beforeAppsDivider};
}

void NoteIconsMoving();

// Re-measures the plate around the icons, as if nothing were magnified.
void MeasurePlate(Plate& p) {
    auto row = p.row.get();
    auto panel = p.panel.get();
    if (!row || !panel || !p.root) return;
    if (!p.frame.get()) {
        if (auto frame = FindTaskbarFrame(row.as<wux::FrameworkElement>())) {
            p.frame = winrt::make_weak(frame);
        }
    }

    auto icons = GetIcons(row);
    if (icons.size() != p.iconCount) {
        if (p.iconCount) NoteIconsMoving();  // an app opened or closed
        p.iconCount = icons.size();
    }
    if (icons.empty()) {
        for (auto& el : PlateParts(p)) {
            if (el) el.Visibility(Visibility::Collapsed);
        }
        HideStrayDots(panel, icons);
        return;
    }

    constexpr float kMax = std::numeric_limits<float>::max();
    float minLeft = kMax, maxRight = -kMax, minTop = kMax, maxBottom = -kMax, widths = 0;
    for (auto& icon : icons) {
        minLeft = std::min(minLeft, icon.left);
        maxRight = std::max(maxRight, icon.left + icon.width);
        minTop = std::min(minTop, icon.top);
        maxBottom = std::max(maxBottom, icon.top + icon.height);
        widths += icon.width;
    }

    // Buttons are wider than their icons; the padding is from the icons.
    float empty = std::max(0.0f, (widths / icons.size() - g_size.icon) / 2);
    float pad = g_size.padX - empty;
    auto origin = row.TransformToVisual(panel).TransformPoint({0, 0});
    p.baseLeft = origin.X + minLeft - pad;
    p.baseWidth = maxRight - minLeft + 2 * pad;

    // The taskbar centers icons in its full height; they move down onto the dock.
    float height = g_size.dockHeight;
    p.top = (float)panel.ActualHeight() - g_size.bottomGap - height;
    float centerY = p.top + g_size.padY + g_size.icon / 2;
    float offsetY = centerY - (origin.Y + (minTop + maxBottom) / 2);

    double current = p.root.Height();  // NaN when new
    if (std::isnan(current) || std::fabs(current - height) > 0.5) p.root.Height(height);
    p.left = p.width = -1;  // ApplyPlate writes again
    p.root.Visibility(Visibility::Visible);

    auto [beforeTrash, beforeAppsDivider] = LayOutButtons(p, icons);
    p.beforeAppsDivider = beforeAppsDivider ? winrt::make_weak(beforeAppsDivider) : nullptr;
    auto iconOf = [&](wux::FrameworkElement const& el) -> Icon const& {
        for (auto& icon : icons) {
            if (icon.element == el) return icon;
        }
        return icons.back();
    };
    // `offset` into the space after `icon`.
    auto placeDivider = [&](wuxc::Border const& divider, Icon const& icon, float offset) {
        float x = origin.X + icon.left + icon.width + g_size.buttonMargin + offset;
        divider.Margin({x - kDividerWidth / 2, p.top + DividerInset(), 0, 0});
        divider.Visibility(Visibility::Visible);
    };

    if (p.trash) {
        auto& last = iconOf(beforeTrash);
        float size = g_size.icon * TrashIconScale();
        p.trashCenter = last.left + last.width + g_size.buttonMargin + g_size.dividerSpace +
                        g_size.trashSlot / 2;
        p.trashCenter -= kTrashShift * g_size.icon / kDefaultIconSize;
        p.baseWidth += g_size.trashSpace;
        p.trash.Margin({origin.X + p.trashCenter - size / 2, centerY - size / 2, 0, 0});
        p.trash.Visibility(Visibility::Visible);
        placeDivider(p.divider, last, g_size.dividerLeft);
        UpdateTrashIcons();
    }
    if (p.appsDivider) {
        p.appsDivider.Visibility(Visibility::Collapsed);
        if (beforeAppsDivider) {
            placeDivider(p.appsDivider, iconOf(beforeAppsDivider), g_size.dividerLeft);
        }
    }

    p.offsetY = offsetY;
    p.originX = origin.X;
    p.originY = origin.Y;
    for (auto& icon : icons) ReapplyButton(icon.element);
    HideStrayDots(panel, icons);
}

// Positions the plate, widened by `extra` in all, `extraLeft` of it leftward.
void ApplyPlate(Plate& p, float extraLeft, float extra) {
    if (!p.root) return;
    float left = p.baseLeft - extraLeft;
    float width = p.baseWidth + extra;
    if (std::fabs(left - p.left) > 0.25f || std::fabs(width - p.width) > 0.25f) {
        p.root.Margin({left, p.top, 0, 0});
        p.root.Width(width);
        p.left = left;
        p.width = width;
        ShapePlate(p);
    }
}

void EnsurePlate(wux::FrameworkElement const& button) {
    auto row = wuxm::VisualTreeHelper::GetParent(button).try_as<wux::FrameworkElement>();
    if (!row || GetRowPlate(row) || !FindTaskbarFrame(button)) return;
    auto panel = wuxm::VisualTreeHelper::GetParent(row).try_as<wuxc::Panel>();
    if (!panel) return;

    // Layers in drawing order, unrounded so the right edge doesn't wobble.
    wuxc::Grid root;
    wuxc::Border blurHost;
    wuxs::Path fill, topGlow, sideGlow, edgeInner, edgeOuter;
    PlaceInPanel(panel, root);
    root.IsHitTestVisible(false);
    root.UseLayoutRounding(false);
    for (wux::FrameworkElement layer : std::initializer_list<wux::FrameworkElement>{
             blurHost, fill, topGlow, sideGlow, edgeInner, edgeOuter}) {
        layer.UseLayoutRounding(false);
        root.Children().Append(layer);
    }
    // Just before the icon row: above the taskbar's background, below the icons.
    uint32_t index = 0;
    if (!panel.Children().IndexOf(row, index)) index = 0;
    panel.Children().InsertAt(index, root);

    auto& p = g_plates->emplace_back();
    p.row = winrt::make_weak(row.as<wux::UIElement>());
    p.panel = winrt::make_weak(panel);
    p.root = root;
    p.blurHost = blurHost;
    p.fill = fill;
    p.topGlow = topGlow;
    p.sideGlow = sideGlow;
    p.edgeInner = edgeInner;
    p.edgeOuter = edgeOuter;

    auto onSize = [](wf::IInspectable const&, wux::SizeChangedEventArgs const&) {
        MarkPlatesDirty();
    };
    p.rowSizeChanged = row.SizeChanged(winrt::auto_revoke, onSize);
    p.panelSizeChanged = panel.SizeChanged(winrt::auto_revoke, onSize);
    p.panelThemeChanged = panel.ActualThemeChanged(
        winrt::auto_revoke, [](wux::FrameworkElement const&, wf::IInspectable const&) {
            for (auto& plate : *g_plates) Try([&] { ApplyPlateLook(plate); });
        });
    if (g_settings.recycleBin) AddTrash(p, panel, row);
    if (g_settings.appsDivider) p.appsDivider = AddDivider(panel, row);
    ApplyPlateLook(p);
}

// Clearing the list also revokes the plates' event handlers. Dots go too, and
// come back over the new plates when they're measured.
void RemovePlates() {
    for (auto& i : *g_indicators) RemoveDot(i);
    for (auto& p : *g_plates) {
        if (p.blurHost) {
            Try([&] { wuxh::ElementCompositionPreview::SetElementChildVisual(p.blurHost, nullptr); });
        }
        if (auto row = p.row.get().try_as<wux::FrameworkElement>()) {
            Try([&] { Release(row, wux::FrameworkElement::MarginProperty()); });
        }
        auto panel = p.panel.get();
        if (!panel) continue;
        if (p.hideY != 0) Try([&] { SetTranslation(panel, 0, 0); });
        for (auto& el : PlateParts(p)) {
            uint32_t index;
            if (el && panel.Children().IndexOf(el, index)) panel.Children().RemoveAt(index);
        }
    }
    g_plates->clear();
}

// =============================================================================
// AUTO-HIDE
// =============================================================================
// The dock slides down out of view; the taskbar window stays, so the hot
// corners keep working. It comes back when the pointer reaches the bottom edge
// below it, and slides away once the pointer and any menu it opened are gone.
// Hybrid does this only while a window is maximized.

constexpr float kDockSlideMs = 250;
constexpr int kDockHideTicks = 6;  // polls, about half a second

bool g_dockRevealed = false;
float g_dockShown = 1;  // 0 hidden to 1 shown, animated
int g_dockAwayTicks = 0;
bool g_windowMaximized = false;  // for Hybrid

void StartFrames();
void ScheduleOverWindows(bool autoHide);
bool IsFlyout(HWND hwnd, bool popups);
int StripHeight(UINT dpi);

bool DockHides() {
    return g_settings.autoHide == AutoHide::On ||
           (g_settings.autoHide == AutoHide::Hybrid && g_windowMaximized);
}
bool DockHidden() { return DockHides() && !g_dockRevealed; }
bool DockFullyHidden() { return DockHidden() && g_dockShown <= 0; }

void RevealDock(bool reveal) {
    g_dockAwayTicks = 0;
    if (reveal == g_dockRevealed) return;
    g_dockRevealed = reveal;
    if (!reveal) g_pointerOver = false;
    ScheduleOverWindows(false);  // the window is clipped to what's shown
    StartFrames();
}

// The dock's span along the bottom of monitor `m`, in pixels.
bool DockSpanOn(RECT const& m, LONG& left, LONG& right) {
    bool found = false;
    for (auto& p : *g_plates) {
        auto panel = p.panel.get();
        auto root = panel ? panel.XamlRoot() : nullptr;
        auto content = root ? root.Content().try_as<wux::FrameworkElement>() : nullptr;
        if (!content || p.baseWidth <= 0) continue;
        double scale = root.RasterizationScale();
        bool otherMonitor = std::fabs(content.ActualWidth() * scale - (m.right - m.left)) > 2;
        if (otherMonitor && g_plates->size() > 1) continue;
        float x = panel.TransformToVisual(content).TransformPoint({0, 0}).X + p.baseLeft;
        LONG l = m.left + std::lround(x * scale);
        LONG r = m.left + std::lround((x + p.baseWidth) * scale);
        left = found ? std::min(left, l) : l;
        right = found ? std::max(right, r) : r;
        found = true;
    }
    return found;
}

// ---- Hybrid ----
// Maximized means spanning a dock's screen from edge to edge and reaching down
// to the dock. A window merely touching the dock doesn't count. Checked when a
// window moves, shows, hides, closes or comes to the front, at most every
// kMaximizedCheckMs, rather than polled.

constexpr UINT kMaximizedCheckMs = 50;
std::vector<HWINEVENTHOOK> g_windowEventHooks;
bool g_maximizedCheckPending = false;

bool IsAppWindow(HWND w) {
    if (!IsWindowVisible(w) || IsIconic(w)) return false;
    if (GetWindowLongPtr(w, GWL_EXSTYLE) & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
    BOOL cloaked = FALSE;  // on another desktop, or a closed shell panel
    if (SUCCEEDED(DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked) {
        return false;
    }
    WCHAR cls[64];
    if (!GetClassName(w, cls, ARRAYSIZE(cls))) return false;
    for (PCWSTR shell : {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
                         L"Windows.UI.Core.CoreWindow", L"XamlExplorerHostIslandWindow",
                         kWindowClass}) {
        if (!wcscmp(cls, shell)) return false;
    }
    return true;
}

bool IsWindowMaximized() {
    struct Screen {
        RECT rect;
        LONG dockTop;  // where the dock's strip starts
    };
    std::vector<Screen> screens;
    ForEachTaskbarWindow([&screens](HWND tb, bool) {
        HMONITOR monitor = MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{sizeof(mi)};
        UINT dpiX, dpiY;
        if (GetMonitorInfo(monitor, &mi) &&
            SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
            screens.push_back({mi.rcMonitor, mi.rcMonitor.bottom - StripHeight(dpiY)});
        }
    });
    struct Search {
        std::vector<Screen>* screens;
        bool found = false;
    } search{&screens};
    EnumWindows(
        [](HWND w, LPARAM l) -> BOOL {
            auto& search = *(Search*)l;
            RECT r;
            if (!IsAppWindow(w) ||
                (FAILED(DwmGetWindowAttribute(w, DWMWA_EXTENDED_FRAME_BOUNDS, &r, sizeof(r))) &&
                 !GetWindowRect(w, &r))) {
                return TRUE;
            }
            for (auto& s : *search.screens) {
                if (r.left <= s.rect.left + 1 && r.right >= s.rect.right - 1 &&
                    r.top < s.dockTop && r.bottom >= s.dockTop - 1) {
                    search.found = true;
                    return FALSE;
                }
            }
            return TRUE;
        },
        (LPARAM)&search);
    return search.found;
}

// Windows already have the whole screen in Hybrid, so the dock just slides
// away, unless the pointer's on it, when it waits until it's been gone for half
// a second.
void SetWindowMaximized(bool maximized) {
    if (maximized == g_windowMaximized) return;
    g_windowMaximized = maximized;
    g_dockRevealed = maximized && g_pointerOver;
    g_dockAwayTicks = 0;
    ScheduleOverWindows(false);  // the clip
    StartFrames();
}

void ScheduleMaximizedCheck() {
    if (g_maximizedCheckPending || g_windowEventHooks.empty()) return;
    HWND w = g_modWindow;
    g_maximizedCheckPending = w && SetTimer(w, kMaximizedCheckTimer, kMaximizedCheckMs, nullptr);
}

void CheckWindowMaximized() {
    g_maximizedCheckPending = false;
    if (!g_unloading && g_settings.autoHide == AutoHide::Hybrid) {
        SetWindowMaximized(IsWindowMaximized());
    }
}

void CALLBACK OnWindowEvent(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG idChild, DWORD,
                            DWORD) {
    if (!g_unloading && hwnd && idObject == OBJID_WINDOW && idChild == CHILDID_SELF) {
        ScheduleMaximizedCheck();
    }
}

// On the taskbar's thread, whose message loop delivers the events.
void WatchWindows(bool watch) {
    if (watch == !g_windowEventHooks.empty()) return;
    if (!watch) {
        for (HWINEVENTHOOK hook : g_windowEventHooks) UnhookWinEvent(hook);
        g_windowEventHooks.clear();
        return;
    }
    constexpr std::pair<DWORD, DWORD> kEvents[] = {
        {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
        {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE},  // and EVENT_OBJECT_SHOW
        {EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE},
        {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
    };
    for (auto [first, last] : kEvents) {
        HWINEVENTHOOK hook =
            SetWinEventHook(first, last, nullptr, OnWindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
        if (hook) g_windowEventHooks.push_back(hook);
    }
}

// ---- Polling ----

void UpdateAutoHide(POINT p, RECT const& m, UINT dpi, bool held, bool fullscreen) {
    if (!DockHides()) return;
    LONG left = 0, right = 0;
    bool inSpan = DockSpanOn(m, left, right) && p.x >= left && p.x <= right;
    if (!g_dockRevealed) {
        if (inSpan && p.y >= m.bottom - 3 && !fullscreen) RevealDock(true);
        return;
    }
    bool nearDock = inSpan && p.y >= m.bottom - StripHeight(dpi);
    if (nearDock || g_pointerOver || held || IsFlyout(WindowFromPoint(p), true) ||
        IsFlyout(GetForegroundWindow(), false)) {
        g_dockAwayTicks = 0;
    } else if (++g_dockAwayTicks >= kDockHideTicks) {
        RevealDock(false);
    }
}

// =============================================================================
// LABELS
// =============================================================================
// The name of the app under the pointer, above its icon as on a Mac, in place
// of Windows' tooltips. It rises above the taskbar, so it's a click-through
// window of its own: a solid light pill with a soft glow.

constexpr float kLabelHeight = 30;       // the box, without its arrow
constexpr float kLabelPadX = 11;
constexpr float kLabelFontSize = 14;
constexpr float kLabelGlowReach = 0.3f;     // inner glow, as a share of the height
constexpr float kLabelGlowStrength = 0.24f; // inner glow, as a share of the fill's opacity
constexpr float kLabelArrow = 7.5f;      // arrow height
constexpr float kLabelArrowHalf = 8.5f;  // half the arrow's width
constexpr float kLabelArrowTip = 2;      // rounding at its tip
constexpr float kLabelArrowBase = 4.5f;  // rounding where it meets the pill
constexpr float kLabelGap = 6;           // between the arrow and a magnified icon
constexpr float kLabelDockGap = 3;       // between the arrow and the dock's top edge
constexpr Color kLabelInkOnLight{255, 0x0A, 0x0A, 0x0A};
constexpr Color kLabelInkOnDark{255, 0xF5, 0xF5, 0xF5};

struct Label {
    HWND hwnd = nullptr;
    std::wstring text;
    RECT rect{};  // on screen, as drawn
    bool shown = false;
};

Label g_label;
bool g_labelSuppressed = false;  // by a menu, until the pointer leaves the dock

// ---- Name ----

// The first text in Windows' own tooltip: the app's name.
std::wstring FirstText(wf::IInspectable const& value, int depth = 0) {
    if (!value || depth > 4) return {};
    if (auto s = value.try_as<wf::IPropertyValue>(); s && s.Type() == wf::PropertyType::String) {
        return std::wstring(s.GetString());
    }
    if (auto text = value.try_as<wuxc::TextBlock>()) return std::wstring(text.Text());
    if (auto control = value.try_as<wuxc::ContentControl>()) {
        return FirstText(control.Content(), depth + 1);
    }
    if (auto panel = value.try_as<wuxc::Panel>()) {
        for (auto const& child : panel.Children()) {
            if (auto text = FirstText(child, depth + 1); !text.empty()) return text;
        }
    }
    return {};
}

std::wstring AppName(wux::FrameworkElement const& button) {
    std::wstring name = FirstText(wuxc::ToolTipService::GetToolTip(button));
    if (name.empty()) {
        // "App - 2 running windows", without the count.
        name = wux::Automation::AutomationProperties::GetName(button);
        size_t dash = name.rfind(L" - ");
        if (dash != name.npos && std::any_of(name.begin() + dash, name.end(),
                                             [](wchar_t c) { return iswdigit(c); })) {
            name.resize(dash);
        }
    }
    return name.substr(0, name.find(L'\n'));
}

// ---- Window ----

void HideLabel() {
    if (g_label.shown) ShowWindow(g_label.hwnd, SW_HIDE);
    g_label.shown = false;
}

void SuppressLabel() {
    g_labelSuppressed = true;
    HideLabel();
}

HWND EnsureLabelWindow() {
    if (g_label.hwnd) return g_label.hwnd;
    if (!EnsureGdiplus() || !RegisterWindowClass()) return nullptr;
    g_label.hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        kWindowClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, ModuleInstance(), nullptr);
    return g_label.hwnd;
}

void DestroyLabel() {
    if (g_label.hwnd) DestroyWindow(g_label.hwnd);
    g_label = {};
}

// ---- Drawing ----

// A pill and its arrow as one shape. The arrow's three corners are rounded.
void LabelShape(Gdiplus::GraphicsPath& path, float w, float h, float scale) {
    using Gdiplus::PointF;
    float i = 0.5f, d = h - 2 * i, b = h - i;  // fully round ends; b is the pill's bottom
    float cx = w / 2, tipRound = kLabelArrowTip * scale, baseRound = kLabelArrowBase * scale;
    PointF right{cx + kLabelArrowHalf * scale, b}, tip{cx, b + kLabelArrow * scale};
    PointF left{cx - kLabelArrowHalf * scale, b};

    // `distance` from `from` toward `to`.
    auto toward = [](PointF from, PointF to, float distance) {
        float dx = to.X - from.X, dy = to.Y - from.Y, length = std::hypot(dx, dy);
        return PointF{from.X + dx / length * distance, from.Y + dy / length * distance};
    };
    // Rounds the corner at `c`, from `a` to `b`, as a quadratic curve.
    auto corner = [&path](PointF a, PointF c, PointF b) {
        path.AddBezier(a, PointF{a.X + (c.X - a.X) * 2 / 3, a.Y + (c.Y - a.Y) * 2 / 3},
                       PointF{b.X + (c.X - b.X) * 2 / 3, b.Y + (c.Y - b.Y) * 2 / 3}, b);
    };

    path.AddArc(i, i, d, d, 180, 90);
    path.AddArc(w - i - d, i, d, d, 270, 90);
    path.AddArc(w - i - d, i, d, d, 0, 90);
    corner(PointF{right.X + baseRound, b}, right, toward(right, tip, baseRound));
    corner(toward(tip, right, tipRound), tip, toward(tip, left, tipRound));
    corner(toward(left, tip, baseRound), left, PointF{left.X - baseRound, b});
    path.AddArc(i, i, d, d, 90, 90);
    path.CloseFigure();
}

// With its arrow's tip at `tip`, on screen.
void DrawLabel(std::wstring const& text, POINT tip, float scale) {
    namespace gp = Gdiplus;
    gp::FontFamily family(L"Segoe UI");
    gp::Font font(&family, kLabelFontSize * scale, gp::FontStyleRegular, gp::UnitPixel);
    gp::StringFormat format;
    format.SetAlignment(gp::StringAlignmentCenter);
    format.SetLineAlignment(gp::StringAlignmentCenter);
    format.SetFormatFlags(gp::StringFormatFlagsNoWrap);

    gp::RectF bounds;
    {
        gp::Bitmap probe(1, 1, PixelFormat32bppPARGB);
        gp::Graphics g(&probe);
        g.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
        g.MeasureString(text.c_str(), (INT)text.size(), &font, gp::PointF(0, 0), &format, &bounds);
    }
    int w = (int)std::ceil(bounds.Width + 2 * kLabelPadX * scale);
    int h = (int)std::round(kLabelHeight * scale);
    int height = h + (int)std::ceil(kLabelArrow * scale) + 1;
    RECT rect{tip.x - w / 2, tip.y - height, tip.x - w / 2 + w, tip.y};
    if (g_label.shown && text == g_label.text && EqualRect(&rect, &g_label.rect)) return;

    BITMAPINFO bi{};
    bi.bmiHeader = {sizeof(BITMAPINFOHEADER), w, -height, 1, 32, BI_RGB};
    void* bits = nullptr;
    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    if (HBITMAP bmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0)) {
        {
            gp::Bitmap canvas(w, height, w * 4, PixelFormat32bppPARGB, (BYTE*)bits);
            gp::Graphics g(&canvas);
            g.Clear(gp::Color(0, 0, 0, 0));
            g.SetSmoothingMode(gp::SmoothingModeAntiAlias);
            gp::GraphicsPath path;
            LabelShape(path, (float)w, (float)h, scale);
            auto [r, gr, b] = g_settings.tooltipColor;
            Color fillColor{255, r, gr, b};
            gp::SolidBrush fill(GdiColor(fillColor));
            g.FillPath(&fill, &path);

            // The dock's inner glow, flipped past the box's bottom into the arrow.
            Color glow = GlowColor(fillColor, kLabelGlowStrength);
            gp::Color colors[kGlowSteps * 2];
            gp::REAL positions[kGlowSteps * 2];
            int i = 0;
            for (auto [offset, strength] : GlowStops(kLabelGlowReach)) {
                Color color = glow;
                color.A = (uint8_t)std::lround(glow.A * strength);
                colors[i] = GdiColor(color);
                positions[i++] = offset;
            }
            gp::LinearGradientBrush glowBrush(gp::PointF(0, 0), gp::PointF(0, (gp::REAL)h),
                                              GdiColor(glow), GdiColor(glow));
            glowBrush.SetInterpolationColors(colors, positions, kGlowSteps * 2);
            glowBrush.SetWrapMode(gp::WrapModeTileFlipY);
            g.FillPath(&glowBrush, &path);

            Color ink = IsLightColor(r, gr, b) ? kLabelInkOnLight : kLabelInkOnDark;
            gp::SolidBrush inkBrush(GdiColor(ink));
            g.SetTextRenderingHint(gp::TextRenderingHintAntiAliasGridFit);
            g.DrawString(text.c_str(), (INT)text.size(), &font,
                         gp::RectF(0, 0, (gp::REAL)w, (gp::REAL)h), &format, &inkBrush);
        }
        HGDIOBJ old = SelectObject(dc, bmp);
        POINT pos{rect.left, rect.top}, zero{};
        SIZE size{w, height};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(g_label.hwnd, screen, &pos, &size, dc, &zero, 0, &blend, ULW_ALPHA);
        SelectObject(dc, old);
        DeleteObject(bmp);
    }
    DeleteDC(dc);
    ReleaseDC(nullptr, screen);

    if (!g_label.shown) {
        SetWindowPos(g_label.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    g_label = {g_label.hwnd, text, rect, true};
}

// A point in the plate's panel, on screen, with the window of the taskbar under
// the pointer (its content starts at the window's corner). Returns the screen's
// scale, or 0 when it can't tell.
float PanelToScreen(Plate& p, wf::Point point, POINT& out, RECT& window) {
    auto panel = p.panel.get();
    auto root = panel ? panel.XamlRoot() : nullptr;
    auto content = root ? root.Content() : nullptr;
    POINT cursor;
    HWND taskbar = nullptr;
    if (content && GetCursorPos(&cursor)) {
        HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
        ForEachTaskbarWindow([&](HWND tb, bool) {
            if (!taskbar && MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST) == monitor) taskbar = tb;
        });
    }
    if (!taskbar || !GetWindowRect(taskbar, &window)) return 0;
    float scale = (float)root.RasterizationScale();
    auto at = panel.TransformToVisual(content).TransformPoint(point);
    out = {window.left + std::lround(at.X * scale), window.top + std::lround(at.Y * scale)};
    return scale;
}

// With its arrow's tip at (`x`, `tipY`) in the plate's panel.
void ShowLabel(Plate& p, std::wstring const& name, float x, float tipY) {
    POINT tip;
    RECT window;
    float scale = name.empty() ? 0 : PanelToScreen(p, {x, tipY}, tip, window);
    if (scale <= 0 || !EnsureLabelWindow()) return HideLabel();
    DrawLabel(name, tip, scale);
}

// Where the label's arrow tip goes over an icon `height` tall, as magnified
// (icons grow from their bottom): never lower than the dock's top edge.
float LabelTipY(Plate const& p, float height) {
    float bottom = p.top + g_size.padY + g_size.icon;
    return std::min(bottom - height - kLabelGap, p.top - kLabelDockGap);
}

// Over the icon under the pointer.
void UpdateLabel(Plate* p) {
    if (!g_settings.tooltips || !p || !g_pointerOver || g_dragging || g_labelSuppressed ||
        DockHidden()) {
        return HideLabel();
    }
    for (auto& item : g_row) {
        float x = item.center + item.shift;
        if (std::fabs(g_pointerX - x) > item.width * item.scale / 2) continue;
        if (item.trash) {
            PCWSTR name = HasTrashIcon() ? L"Trash" : L"Recycle Bin";
            float tipY = LabelTipY(*p, TrashArtHeight() * item.scale);
            return ShowLabel(*p, name, p->originX + x, tipY);
        }
        auto el = item.element.get();
        if (!el) break;
        HideStockToolTip(el);
        auto name = HasFinderIcon() && IsFileExplorer(el) ? L"Finder" : AppName(el);
        return ShowLabel(*p, name, p->originX + x, LabelTipY(*p, g_size.icon * item.scale));
    }
    HideLabel();
}

// =============================================================================
// FRAME LOOP
// =============================================================================
// Runs once per screen refresh, only while something is moving.

winrt::event_token g_frameToken{};
bool g_frameRunning = false;
std::chrono::steady_clock::time_point g_lastFrame;
int g_platesDirtyFrames = 0;  // frames left in which to re-measure the plates

void ScanTaskbars();
bool FollowPointerUnderTrashMenu();

void StopFrames() {
    if (!g_frameRunning) return;
    wuxm::CompositionTarget::Rendering(g_frameToken);
    g_frameRunning = false;
}

void OnFrame();

void StartFrames() {
    if (g_frameRunning) return;
    g_lastFrame = std::chrono::steady_clock::now();
    g_frameToken = wuxm::CompositionTarget::Rendering(
        [](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                OnFrame();
            } catch (...) {
                Wh_Log(L"OnFrame error: 0x%08X %s", winrt::to_hresult(),
                       winrt::to_message().c_str());
                StopFrames();
            }
        });
    g_frameRunning = true;
}

// Re-measured over a few frames, so the taskbar has laid buttons out first.
void MarkPlatesDirty() {
    g_platesDirtyFrames = 3;
    StartFrames();
}

// Finds new buttons, adds missing plates and re-measures them. Not done in the
// hooks, as the taskbar doesn't like its tree changing while it updates.
void RefreshPlates() {
    Try(ScanTaskbars, L"ScanTaskbars");
    for (auto& b : *g_buttons) {
        if (auto el = b.element.get()) Try([&] { EnsurePlate(el); }, L"EnsurePlate");
    }
    g_measuringPlates = true;
    for (auto& p : *g_plates) Try([&] { MeasurePlate(p); }, L"MeasurePlate");
    g_measuringPlates = false;
    ScheduleOverWindows(false);  // the taskbar may have changed size
}

void OnFrame() {
    CheckDragging();
    auto now = std::chrono::steady_clock::now();
    float dt = std::min(std::chrono::duration<float>(now - g_lastFrame).count(), 0.1f);
    g_lastFrame = now;
    float alpha = 1.0f;  // how far toward the target this frame
    if (g_settings.animationMs > 0) alpha = 1.0f - std::exp(-dt / (g_settings.animationMs / 3000.0f));

    if (g_platesDirtyFrames > 0) {
        g_platesDirtyFrames--;
        RefreshPlates();
    }
    bool menuOpen = FollowPointerUnderTrashMenu();
    auto rowParent = g_rowParent.get();
    if (rowParent) BuildRow(rowParent);

    // ---- 1. Scale: full size under the pointer, falling off to the radius ----
    bool settled = g_platesDirtyFrames == 0 && !menuOpen;
    bool over = g_pointerOver && !g_dragging;
    std::vector<float> scales(g_row.size(), 1.0f);
    for (size_t i = 0; i < g_row.size(); i++) {
        auto& item = g_row[i];
        float d = std::fabs(g_pointerX - item.center);
        float target = 1.0f;
        if (over && d < g_size.radius) {
            float k = (std::cos(d / g_size.radius * 3.14159265f) + 1.0f) / 2.0f;
            target = 1.0f + (g_settings.scale - 1.0f) * k;
        }
        float s = item.scale + (target - item.scale) * alpha;
        if (std::fabs(target - s) < 0.001f) s = target;
        else settled = false;
        scales[i] = s;
    }

    bool expanded = over ||
                    std::any_of(scales.begin(), scales.end(), [](float s) { return s > 1.0f; });
    if (expanded != g_dockExpanded) {
        g_dockExpanded = expanded;
        ScheduleOverWindows(false);
    }

    // ---- 2. Spread ----
    // Icons are pushed right by the growth before them, then pulled left by
    // `growLeft`. As on a Mac, the end away from the pointer stays put.
    float total = 0;
    for (size_t i = 0; i < g_row.size(); i++) total += (scales[i] - 1.0f) * g_row[i].width;
    if (over) {
        if (!g_row.empty()) {
            auto& first = g_row.front();
            auto& last = g_row.back();
            float middle = (first.center - first.width / 2 + last.center + last.width / 2) / 2;
            g_anchorLeftEnd = g_pointerX > middle;
        }
        g_maxGrowth = std::max(g_maxGrowth, total);
    } else if (g_lastGrowth > 0) {
        g_maxGrowth *= total / g_lastGrowth;  // shrinks back keeping its shape
    } else {
        g_maxGrowth = total;
    }
    g_lastGrowth = total;
    float growLeft = g_anchorLeftEnd ? g_maxGrowth / 2 : total - g_maxGrowth / 2;

    Plate* plate = GetRowPlate(rowParent);
    auto beforeAppsDivider = plate ? plate->beforeAppsDivider.get() : nullptr;
    float appsDividerShift = 0, before = 0;
    for (size_t i = 0; i < g_row.size(); i++) {
        auto& item = g_row[i];
        float extra = (scales[i] - 1.0f) * item.width;
        float shift = before + extra / 2 - growLeft;
        float gapShift = before - growLeft;  // of the space just before it
        before += extra;
        if (beforeAppsDivider && item.element.get() == beforeAppsDivider) {
            appsDividerShift = before - growLeft;
        }

        auto icon = item.element.get();
        bool changed = scales[i] != item.scale || std::fabs(shift - item.shift) > 0.01f;
        if (!changed || (item.trash ? !plate : !icon)) continue;
        if (item.trash) SetTrashScale(*plate, scales[i], shift, gapShift);
        else SetScale(icon, scales[i], shift);
        item.scale = scales[i];
        item.shift = shift;
    }
    if (plate) SetAppsDividerShift(*plate, appsDividerShift);

    // ---- 3. The plate widens the same way ----
    for (auto& p : *g_plates) {
        bool current = rowParent && p.row.get() == rowParent;
        ApplyPlate(p, current ? growLeft : 0, current ? total : 0);
    }

    // ---- 4. Auto-hide: the whole panel slides down ----
    float shownTarget = DockHidden() ? 0.0f : 1.0f;
    if (g_dockShown != shownTarget) {
        bool wasHidden = DockFullyHidden();
        float step = dt * 1000 / kDockSlideMs;
        g_dockShown = std::clamp(g_dockShown + (shownTarget > g_dockShown ? step : -step), 0.0f, 1.0f);
        if (DockFullyHidden() != wasHidden) ScheduleOverWindows(false);
        if (g_dockShown != shownTarget) settled = false;
    }
    float eased = g_dockShown * g_dockShown * (3 - 2 * g_dockShown);
    float hideY = std::round((1 - eased) * (g_size.bottomGap + g_size.dockHeight + 2));
    for (auto& p : *g_plates) {
        auto panel = p.panel.get();
        if (!panel || p.hideY == hideY) continue;
        SetTranslation(panel, 0, hideY);
        p.hideY = hideY;
    }

    // ---- 5. Label ----
    UpdateLabel(plate);

    if (settled) StopFrames();
}

// =============================================================================
// SYSTEM TRAY
// =============================================================================
// Kept at Windows' usual height, at the bottom of the taller taskbar. The hot
// corner slides it in from past the screen's right edge.

constexpr int kTraySlideInMs = 280;
constexpr int kTraySlideOutMs = 320;

struct TrayFrame {
    winrt::weak_ref<wux::FrameworkElement> element;
    bool hidden = false;        // collapsed for Hidden
    bool corner = false;        // set up for its hot corner
    bool cornerHidden = false;  // slid out of view
};

std::list<TrayFrame> g_trayFrames;
bool g_trayRevealed = false;
int g_trayWidth = 0;  // in pixels, for the hot corner

bool TrayShown() {
    return g_settings.tray == Show::Visible || (g_settings.tray == Show::HotCorner && g_trayRevealed);
}

void FixTrayRow(wux::FrameworkElement const& row) {
    Track(row, wux::FrameworkElement::HeightProperty());
    Track(row, wux::FrameworkElement::VerticalAlignmentProperty());
    if (row.Height() != kStockTaskbarHeight) row.Height(kStockTaskbarHeight);
    if (row.VerticalAlignment() != wux::VerticalAlignment::Bottom) {
        row.VerticalAlignment(wux::VerticalAlignment::Bottom);
    }
}

// In: eases to a stop. Out: picks up speed, fading in the second half.
void SlideTray(wux::FrameworkElement const& el, bool hidden, bool animate) {
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, true);
    float x = hidden ? (float)el.ActualWidth() + 24 : 0;
    float opacity = hidden ? 0.0f : 1.0f;
    if (!animate) {
        visual.StopAnimation(L"Translation.X");
        visual.StopAnimation(L"Opacity");
        SetTranslation(el, x);
        visual.Opacity(opacity);
        return;
    }
    auto compositor = visual.Compositor();
    auto slide = compositor.CreateScalarKeyFrameAnimation();
    auto fade = compositor.CreateScalarKeyFrameAnimation();
    auto ease = hidden ? compositor.CreateCubicBezierEasingFunction({0.45f, 0.0f}, {0.6f, 1.0f})
                       : compositor.CreateCubicBezierEasingFunction({0.2f, 0.9f}, {0.3f, 1.0f});
    if (hidden) fade.InsertKeyFrame(0.5f, 1.0f);
    slide.InsertKeyFrame(1.0f, x, ease);
    fade.InsertKeyFrame(1.0f, opacity, ease);
    auto duration = std::chrono::milliseconds(hidden ? kTraySlideOutMs : kTraySlideInMs);
    slide.Duration(duration);
    fade.Duration(duration);
    visual.StartAnimation(L"Translation.X", slide);
    visual.StartAnimation(L"Opacity", fade);
}

void ResetTraySlide(wux::FrameworkElement const& el) {
    SlideTray(el, false, false);
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, false);
}

void ApplyTrayVisibility(wux::FrameworkElement const& el, TrayFrame& t) {
    auto show = g_settings.tray;
    auto hitTest = wux::UIElement::IsHitTestVisibleProperty();

    if (show == Show::Hidden) {
        t.hidden = true;
        if (el.Visibility() != Visibility::Collapsed) el.Visibility(Visibility::Collapsed);
    } else if (t.hidden) {
        t.hidden = false;
        PutBack(el, wux::UIElement::VisibilityProperty());
    }

    if (show == Show::HotCorner) {
        bool hide = !g_trayRevealed;
        if (!t.corner || hide != t.cornerHidden) {
            SlideTray(el, hide, t.corner);  // starts out of view without sliding
            t.corner = true;
            t.cornerHidden = hide;
        }
        if (!hide) PutBack(el, hitTest);
        else if (el.IsHitTestVisible()) el.IsHitTestVisible(false);  // clicks go through
    } else if (t.corner) {
        t.corner = false;
        ResetTraySlide(el);
        PutBack(el, hitTest);
    }

    if (show != Show::Hidden && el.ActualWidth() > 0) {
        auto root = el.XamlRoot();
        double scale = root ? root.RasterizationScale() : 1;
        g_trayWidth = (int)std::lround(el.ActualWidth() * scale);
    }
}

void ApplyTrayFrame(wux::FrameworkElement const& el) {
    for (auto& t : g_trayFrames) {
        if (t.element.get() == el) return ApplyTrayVisibility(el, t);
    }
}

void ApplyTrayFrames() {
    for (auto& t : g_trayFrames) {
        if (auto el = t.element.get()) Try([&] { ApplyTrayVisibility(el, t); });
    }
}

void FixTrayFrame(wux::FrameworkElement const& el) {
    g_trayFrames.remove_if([](TrayFrame const& t) { return !t.element.get(); });
    if (std::none_of(g_trayFrames.begin(), g_trayFrames.end(),
                     [&](TrayFrame const& t) { return t.element.get() == el; })) {
        g_trayFrames.push_back({winrt::make_weak(el)});
        Track(el, wux::UIElement::VisibilityProperty(), ApplyTrayFrame);
        Track(el, wux::UIElement::IsHitTestVisibleProperty());
    }
    ApplyTrayFrame(el);
}

// =============================================================================
// POLLING: HOT CORNERS AND AUTO-HIDE
// =============================================================================
// The pointer is checked a dozen times a second. Neither corner fires over
// full screen apps or while dragging.

bool g_leftCornerArmed = true;
int g_trayAwayTicks = 0;

// Whether `hwnd` belongs to a menu or panel, like those the dock and tray
// open. With `popups`, an app's own pop-ups count too.
bool IsFlyout(HWND hwnd, bool popups) {
    HWND root = hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
    WCHAR cls[96];
    if (!root || !GetClassNameW(root, cls, ARRAYSIZE(cls))) return false;
    for (PCWSTR notFlyout : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", L"Progman", L"WorkerW"}) {
        if (!wcscmp(cls, notFlyout)) return false;
    }
    for (PCWSTR flyout :
         {L"#32768" /* menus */, L"Xaml_WindowedPopupClass", L"XamlExplorerHostIslandWindow",
          L"TopLevelWindowForOverflowXamlIsland", L"NotifyIconOverflowWindow",
          L"Windows.UI.Core.CoreWindow", L"ControlCenterWindow"}) {
        if (!wcscmp(cls, flyout)) return true;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(root, &pid);
    if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
        WCHAR path[MAX_PATH];
        DWORD size = ARRAYSIZE(path);
        bool shell = false;
        if (QueryFullProcessImageNameW(process, 0, path, &size)) {
            PCWSTR file = wcsrchr(path, L'\\');
            file = file ? file + 1 : path;
            shell = !_wcsicmp(file, L"ShellExperienceHost.exe") ||
                    !_wcsicmp(file, L"ShellHost.exe");
        }
        CloseHandle(process);
        if (shell) return true;
    }

    if (!popups) return false;
    LONG_PTR style = GetWindowLongPtrW(root, GWL_STYLE);
    LONG_PTR ex = GetWindowLongPtrW(root, GWL_EXSTYLE);
    return (ex & WS_EX_TOPMOST) && (style & WS_POPUP) && !(style & WS_CAPTION);
}

void UpdatePolling() {
    HWND w = EnsureModWindow();
    if (!w) return;
    auto& s = g_settings;
    bool autoHide = s.autoHide != AutoHide::Off;
    if (!g_unloading &&
        (s.leftCorner != LeftCorner::Off || s.tray == Show::HotCorner || autoHide)) {
        SetTimer(w, kPollTimer, 80, nullptr);
    } else {
        KillTimer(w, kPollTimer);
    }
    if (s.tray != Show::HotCorner && g_trayRevealed) {
        g_trayRevealed = false;
        ApplyTrayFrames();
    }
    if (!autoHide) g_dockRevealed = false;  // starts hidden when turned on
    bool hybrid = s.autoHide == AutoHide::Hybrid && !g_unloading;
    if (!hybrid) g_windowMaximized = false;
    WatchWindows(hybrid);
    if (hybrid) ScheduleMaximizedCheck();  // as things are now, at the new dock size
}

// Windows drops keypresses we send while an administrator window is in front.
bool AdminWindowInFront() {
    DWORD pid = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    bool elevated = true;  // a token we can't read is above us too
    HANDLE token;
    if (OpenProcessToken(process, TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD size;
        elevated = GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size) &&
                   elevation.TokenIsElevated;
        CloseHandle(token);
    }
    CloseHandle(process);
    return elevated;
}

// Presses the keys in order, then lets go of them in reverse.
bool PressKeys(std::initializer_list<WORD> held) {
    std::vector<INPUT> keys;
    for (WORD key : held) keys.push_back({INPUT_KEYBOARD, {.ki = {key}}});
    for (auto key = std::rbegin(held); key != std::rend(held); ++key)
        keys.push_back({INPUT_KEYBOARD, {.ki = {*key, 0, KEYEVENTF_KEYUP}}});
    return SendInput((UINT)keys.size(), keys.data(), sizeof(INPUT)) == keys.size();
}

// The Windows key gives Start the focus. When it can't be pressed, the taskbar is asked.
void OpenStart() {
    if (!AdminWindowInFront() && PressKeys({VK_LWIN})) return;
    if (HWND tb = FindTaskbar()) PostMessage(tb, WM_SYSCOMMAND, SC_TASKLIST, 0);
}

// Windows key + Tab, or Task View's own shortcut when the keys can't be pressed.
void OpenTaskView() {
    if (!AdminWindowInFront() && PressKeys({VK_LWIN, VK_TAB})) return;
    ShellExecuteW(nullptr, nullptr, L"explorer.exe", L"shell:::{3080F90E-D7AD-11D9-BD98-0000947B0257}",
                  nullptr, SW_SHOWNORMAL);
}

void OnPollTimer() {
    POINT p;
    if (g_unloading || !GetCursorPos(&p)) return;
    HMONITOR monitor = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) return;
    RECT const& m = mi.rcMonitor;
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    float scale = dpiY / 96.0f;

    bool held = (GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON)) & 0x8000;
    QUERY_USER_NOTIFICATION_STATE state;
    bool fullscreen = SUCCEEDED(SHQueryUserNotificationState(&state)) &&
                      (state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_BUSY ||
                       state == QUNS_PRESENTATION_MODE);
    bool blocked = held || fullscreen;
    constexpr int edge = 2;
    bool bottom = p.y >= m.bottom - 1 - edge;
    bool inLeft = bottom && p.x <= m.left + edge;
    bool inRight = bottom && p.x >= m.right - 1 - edge;

    if (g_settings.leftCorner != LeftCorner::Off) {
        int away = (int)(40 * scale);
        if (inLeft && g_leftCornerArmed && !blocked) {
            g_leftCornerArmed = false;
            if (g_settings.leftCorner == LeftCorner::Start) OpenStart();
            else OpenTaskView();
        } else if (p.x > m.left + away || p.y < m.bottom - away) {
            g_leftCornerArmed = true;  // once it's left the corner
        }
    }

    if (g_settings.tray == Show::HotCorner) {
        int zoneW = std::max(g_trayWidth, (int)(200 * scale)) + (int)(16 * scale);
        int zoneH = (int)((kStockTaskbarHeight + 8) * scale);
        bool overTray = p.x >= m.right - zoneW && p.y >= m.bottom - zoneH;
        bool reveal = g_trayRevealed;
        if (inRight && !blocked) {
            reveal = true;
            g_trayAwayTicks = 0;
        } else if (g_trayRevealed) {
            if (overTray || IsFlyout(WindowFromPoint(p), true)) g_trayAwayTicks = 0;
            else if (++g_trayAwayTicks >= 6) reveal = false;  // about half a second
        }
        if (reveal != g_trayRevealed) {
            g_trayRevealed = reveal;
            ApplyTrayFrames();
            // The window's clip follows the tray, once it's slid out of view.
            if (reveal) ScheduleOverWindows(false);
            else if (HWND w = g_modWindow) SetTimer(w, kTrayClipTimer, kTraySlideOutMs, nullptr);
        }
    }

    if (g_settings.autoHide != AutoHide::Off) UpdateAutoHide(p, m, dpiY, held, fullscreen);
}

// =============================================================================
// SCANNING
// =============================================================================
// The hooks report each taskbar (one per monitor), whose tree is then walked.

std::vector<winrt::weak_ref<wux::FrameworkElement>> g_frames;
int g_reapplyCount = 0;

// Everything the mod changes in one button. Safe to call again.
void ProcessButton(wux::FrameworkElement const& button) {
    UpdateStartHidden(button);
    Button& b = TrackButton(button);
    ApplySpacing(button);
    ApplyButtonWidth(b, button);
    ApplyIconColumn(b);
    HideStockToolTip(button);
    ForEachDescendant(button, 6, [&](wux::FrameworkElement const& el) {
        auto name = el.Name();
        if (name == L"Icon") {
            TrackIconImage(el);
            UpdateAppIcon(el, button);
        } else if (name == L"DefaultIcon") {
            TrackIconImage(el);  // shown in place of Icon for some apps
        } else if (name == L"OverlayIcon") {
            TrackOverlayIcon(el);
        } else if (name == L"RunningIndicator") {
            TrackIndicator(el, button);
        } else if (name == L"ProgressIndicator") {
            TrackProgress(el, button);
        } else if ((name == L"BackgroundElement" || name == L"MultiWindowElement") &&
                   el.try_as<wuxc::Border>()) {
            HideHighlight(el);
        }
        return true;
    });
}

void NoteTaskbarFrame(wux::FrameworkElement const& frame) {
    if (!frame) return;
    for (auto& f : g_frames) {
        if (f.get() == frame) return;
    }
    g_frames.push_back(winrt::make_weak(frame));
    MarkPlatesDirty();
    ScheduleOverWindows(true);
    UpdatePolling();

    // While Windows starts, Explorer can leave the taskbar behind windows.
    // Applying auto-hide and the app bars again over the next seconds fixes it.
    static bool reapplying = false;
    if (!reapplying) {
        if (HWND w = EnsureModWindow()) reapplying = SetTimer(w, kReapplyTimer, 4000, nullptr);
    }
}

// ---- Shadows ----
// A soft shadow under each icon and line of text, on a layer at the back of the
// tray, so it stays readable over light windows and wallpapers. Light text gets
// a dark shadow, dark text a light one.

constexpr float kTrayShadowBlur = 6;
constexpr float kTrayShadowOpacity = 0.6f;

struct TrayShadow {
    winrt::weak_ref<wux::FrameworkElement> target;
    wuc::SpriteVisual sprite{nullptr};
};

struct TrayShadows {
    winrt::weak_ref<wuxc::Panel> grid;
    wuxc::Canvas layer{nullptr};
    wuc::ContainerVisual container{nullptr};
    std::vector<TrayShadow> shadows;
    wux::FrameworkElement::LayoutUpdated_revoker layoutUpdated;
};

[[clang::no_destroy]] std::optional<std::list<TrayShadows>> g_trayShadows{std::in_place};

// The shape of `el`'s text or image, and the shadow color that sets it off.
wuc::CompositionBrush AlphaMask(wux::FrameworkElement const& el, Color& shadow) {
    shadow = Color{255, 0, 0, 0};
    if (auto image = el.try_as<wuxc::Image>()) return image.GetAlphaMask();
    auto text = el.try_as<wuxc::TextBlock>();
    if (!text) return nullptr;
    if (auto ink = text.Foreground().try_as<wuxm::SolidColorBrush>()) {
        auto c = ink.Color();
        if (!IsLightColor(c.R, c.G, c.B)) shadow = Color{255, 255, 255, 255};
    }
    return text.GetAlphaMask();
}

// Follows what's in the tray: called whenever its layout changes.
void UpdateTrayShadows(TrayShadows& t) {
    auto grid = t.grid.get();
    if (!grid) return;
    std::vector<wux::FrameworkElement> targets;
    ForEachDescendant(grid, 30, [&](wux::FrameworkElement const& el) {
        if (el == t.layer || el.Opacity() == 0 || el.ActualWidth() <= 0 || el.ActualHeight() <= 0) {
            return false;
        }
        if (!el.try_as<wuxc::Image>() && !el.try_as<wuxc::TextBlock>()) return true;
        targets.push_back(el);
        return false;
    });

    auto children = t.container.Children();
    std::erase_if(t.shadows, [&](TrayShadow const& s) {
        auto target = s.target.get();
        if (target && std::find(targets.begin(), targets.end(), target) != targets.end()) return false;
        children.Remove(s.sprite);
        return true;
    });
    auto compositor = t.container.Compositor();
    for (auto& el : targets) {
        auto it = std::find_if(t.shadows.begin(), t.shadows.end(),
                               [&](TrayShadow const& s) { return s.target.get() == el; });
        if (it == t.shadows.end()) {
            Color color;
            auto mask = AlphaMask(el, color);
            if (!mask) continue;
            auto shadow = compositor.CreateDropShadow();
            shadow.Mask(mask);
            shadow.Color(color);
            shadow.BlurRadius(kTrayShadowBlur);
            shadow.Opacity(kTrayShadowOpacity);
            shadow.Offset({0, 1, 0});
            auto sprite = compositor.CreateSpriteVisual();
            sprite.Shadow(shadow);
            children.InsertAtTop(sprite);
            it = t.shadows.insert(t.shadows.end(), {winrt::make_weak(el), sprite});
        }
        auto at = el.TransformToVisual(t.layer).TransformPoint({0, 0});
        wf::Numerics::float3 offset{at.X, at.Y, 0};
        wf::Numerics::float2 size{(float)el.ActualWidth(), (float)el.ActualHeight()};
        if (it->sprite.Offset() != offset) it->sprite.Offset(offset);
        if (it->sprite.Size() != size) it->sprite.Size(size);
    }
}

void AddTrayShadows(wux::FrameworkElement const& row) {
    auto grid = row.try_as<wuxc::Panel>();
    if (!grid) return;
    g_trayShadows->remove_if([](TrayShadows const& t) { return !t.grid.get(); });
    for (auto& t : *g_trayShadows) {
        if (t.grid.get() == grid) return;
    }
    wuxc::Canvas layer;
    layer.IsHitTestVisible(false);
    wuxc::Canvas::SetZIndex(layer, -1000);  // behind everything else in the tray
    if (auto g = grid.try_as<wuxc::Grid>()) {
        wuxc::Grid::SetRowSpan(layer, std::max(1, (int)g.RowDefinitions().Size()));
        wuxc::Grid::SetColumnSpan(layer, std::max(1, (int)g.ColumnDefinitions().Size()));
    }
    grid.Children().Append(layer);

    auto& t = g_trayShadows->emplace_back();
    t.grid = winrt::make_weak(grid);
    t.layer = layer;
    t.container = wuxh::ElementCompositionPreview::GetElementVisual(layer).Compositor()
                      .CreateContainerVisual();
    wuxh::ElementCompositionPreview::SetElementChildVisual(layer, t.container);
    t.layoutUpdated = layer.LayoutUpdated(
        winrt::auto_revoke, [weak = winrt::make_weak(layer)](auto&&, auto&&) {
            for (auto& t : *g_trayShadows) {
                if (t.layer == weak.get()) Try([&] { UpdateTrayShadows(t); }, L"Tray shadows");
            }
        });
}

void RemoveTrayShadows() {
    for (auto& t : *g_trayShadows) {
        t.layoutUpdated.revoke();
        wuxh::ElementCompositionPreview::SetElementChildVisual(t.layer, nullptr);
        uint32_t index;
        auto grid = t.grid.get();
        if (grid && grid.Children().IndexOf(t.layer, index)) grid.Children().RemoveAt(index);
    }
    g_trayShadows.reset();
}

// The system tray is elsewhere in the same tree as `frame`.
void ScanTray(wux::FrameworkElement const& frame) {
    auto root = frame.XamlRoot();
    auto content = root ? root.Content() : nullptr;
    if (!content) return;
    ForEachDescendant(content, 10, [](wux::FrameworkElement const& el) {
        auto type = winrt::get_class_name(el);
        if (type == L"Taskbar.TaskbarFrame") return false;
        if (type != L"SystemTray.SystemTrayFrame") return true;
        FixTrayFrame(el);
        ForEachDescendant(el, 1, [](wux::FrameworkElement const& child) {
            if (child.Name() == L"SystemTrayFrameGrid") {
                FixTrayRow(child);
                Try([&] { AddTrayShadows(child); }, L"Tray shadows");
            }
            return false;
        });
        return false;
    });
}

void ScanTaskbars() {
    std::erase_if(g_frames, [](auto& f) { return !f.get(); });
    g_buttons->remove_if([](Button const& b) { return !b.element.get(); });
    g_overrides->remove_if([](Override const& o) { return !o.element.get(); });
    ForgetGoneIndicators();

    for (auto& f : g_frames) {
        auto frame = f.get();
        if (!frame) continue;
        ForEachDescendant(frame, 12, [](wux::FrameworkElement const& el) {
            auto type = winrt::get_class_name(el);
            if (type == L"Taskbar.TaskListButton" || type == L"Taskbar.ExperienceToggleButton") {
                ProcessButton(el);
                return false;
            }
            if (type == L"Taskbar.TaskbarBackground") {
                HideBackground(el);
                return false;
            }
            return true;
        });
        ScanTray(frame);
    }
}

// =============================================================================
// POINTER
// =============================================================================

// Over the dock means within the plate's width, gaps included. `pointIn` gives
// the pointer in an element's coordinates.
template <typename F>
void OnDockPointer(Plate* plate, F pointIn) {
    wux::UIElement row{nullptr};
    bool inSpan = false, over = false;
    if (plate && plate->width > 0 && plate->root.Visibility() == Visibility::Visible) {
        row = plate->row.get();
        auto panel = plate->panel.get();
        if (row && panel) {
            // Never less than unmagnified, so a shrinking edge doesn't regrow
            // under a still pointer. Above the dock counts only while magnifying.
            auto point = pointIn(panel);
            inSpan = point.X >= std::min(plate->left, plate->baseLeft) &&
                     point.X <= std::max(plate->left + plate->width,
                                         plate->baseLeft + plate->baseWidth);
            over = inSpan &&
                   (point.Y >= plate->top || (g_pointerOver && g_rowParent.get() == row));
        }
    }
    if (DockHidden()) {
        if (inSpan) RevealDock(true);  // back over it as it slides away
        return;
    }
    if (!over) {
        g_labelSuppressed = false;
        if (g_pointerOver) {
            g_pointerOver = false;
            StartFrames();
        }
        return;
    }
    if (g_rowParent.get() != row) BuildRow(row);
    g_pointerX = pointIn(row).X;
    g_pointerOver = true;
    StartFrames();
}

void OnTaskbarPointerMoved(wux::FrameworkElement const& frame,
                           wuxi::PointerRoutedEventArgs const& e) {
    CheckDragging();
    NoteTaskbarFrame(frame);
    for (auto& p : *g_plates) {
        if (p.frame.get() != frame) continue;
        return OnDockPointer(&p, [&](wux::UIElement const& el) {
            return e.GetCurrentPoint(el).Position();
        });
    }
    OnDockPointer(nullptr, [](wux::UIElement const&) { return wf::Point{}; });
}

void OnTaskbarPointerExited();

// While the Recycle Bin's menu is open, from the cursor's position on screen,
// over the taskbar the menu was opened on.
bool FollowPointerUnderTrashMenu() {
    static HWND taskbar = nullptr;
    auto trash = g_trashMenuFor.get();
    if (!trash) {
        taskbar = nullptr;
        return false;
    }
    for (auto& p : *g_plates) {
        if (p.trash != trash) continue;
        auto panel = p.panel.get();
        auto root = panel ? panel.XamlRoot() : nullptr;
        auto content = root ? root.Content() : nullptr;
        POINT cursor;
        RECT window;
        if (!content || !GetCursorPos(&cursor)) return true;
        if (!taskbar) {
            HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
            ForEachTaskbarWindow([&](HWND tb, bool) {
                if (!taskbar && MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST) == monitor) {
                    taskbar = tb;
                }
            });
        }
        if (!taskbar || !GetWindowRect(taskbar, &window) || !PtInRect(&window, cursor)) {
            if (g_pointerOver) OnTaskbarPointerExited();
            return true;
        }
        float scale = (float)root.RasterizationScale();
        wf::Point at{(cursor.x - window.left) / scale, (cursor.y - window.top) / scale};
        OnDockPointer(&p, [&](wux::UIElement const& el) {
            return content.TransformToVisual(el).TransformPoint(at);
        });
        return true;
    }
    return false;
}

void OnTaskbarPointerExited() {
    g_pointerOver = false;
    g_labelSuppressed = false;
    StartFrames();
}

// =============================================================================
// RESET
// =============================================================================

void RemoveAppBars();
void ClipTaskbars();

void ResetAll() {
    StopFrames();
    WatchWindows(false);
    Try(DestroyLabel);
    Try(RemovePlates);
    g_d2d = nullptr;
    Try(StopTrash);
    for (auto& b : *g_buttons) {
        auto el = b.element.get();
        if (!el) continue;
        Try([&] {
            ReleaseButton(b);
            SetScale(el, 1.0f);
            wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, false);
        });
    }
    for (auto& t : g_trayFrames) {
        auto el = t.element.get();
        if (el && t.corner) Try([&] { ResetTraySlide(el); });
    }
    Try(RemoveTrayShadows);
    Try(UndimDragged);
    Try(StopSettlingIcons);
    RestoreIndicators();
    RestoreOverrides();
    RemoveAppBars();
    ClipTaskbars();  // unclipped, as g_unloading is set
    if (HWND w = g_modWindow.exchange(nullptr)) DestroyWindow(w);

    g_finderIconSource.reset();
    CancelSquircleRenders();
    g_iconSwaps.clear();
    g_squircleCache.clear();
    ShutdownGdiplus();
    g_row.clear();
    g_rowParent = nullptr;
    g_frames.clear();
    g_trayFrames.clear();
    // Last, as everything above reads them, and nothing does after.
    g_buttons.reset();
    g_plates.reset();
    g_indicators.reset();
    g_overrides.reset();
}

// =============================================================================
// HOOKS: TASKBAR ELEMENTS
// =============================================================================

using VoidMethod_t = void(WINAPI*)(void* pThis);
using GetDouble_t = double(WINAPI*)(void* pThis);
using SetDouble_t = void(WINAPI*)(void* pThis, double value);

// In a hook on a taskbar element's method, the element is the interface at
// pThis + 3 pointers (as in "Taskbar Labels").
wux::FrameworkElement ElementFromThis(void* pThis) {
    winrt::Windows::Foundation::IUnknown unknown;
    winrt::copy_from_abi(unknown, (void**)pThis + 3);
    return unknown.try_as<wux::FrameworkElement>();
}

// An app button updated itself: the dock's changes go in before it's drawn.
void OnTaskListButtonUpdated(void* pThis) {
    if (g_unloading) return;
    Try([&] {
        if (auto button = ElementFromThis(pThis)) {
            ProcessButton(button);
            if (Button* b = FindButton(button)) b->impl = pThis;
            NoteTaskbarFrame(FindTaskbarFrame(button));
        }
    }, L"Button update");
}

// ---- Pointer (as in "Taskbar Dock Animation") ----

using TaskbarFrame_OnPointer_t = int(WINAPI*)(void* pThis, void* pArgs);

wux::FrameworkElement TaskbarFrameFromThis(void* pThis) {
    wux::FrameworkElement element{nullptr};
    ((::IUnknown*)pThis)->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                         winrt::put_abi(element));
    if (!element || winrt::get_class_name(element) != L"Taskbar.TaskbarFrame") return nullptr;
    return element;
}

TaskbarFrame_OnPointer_t TaskbarFrame_OnPointerMoved_Original;
int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    int result = TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    if (g_unloading) return result;
    Try([&] {
        if (auto frame = TaskbarFrameFromThis(pThis)) {
            wuxi::PointerRoutedEventArgs args{nullptr};
            ((::IUnknown*)pArgs)->QueryInterface(winrt::guid_of<wuxi::PointerRoutedEventArgs>(),
                                                 winrt::put_abi(args));
            if (args) OnTaskbarPointerMoved(frame, args);
        }
    });
    return result;
}

TaskbarFrame_OnPointer_t TaskbarFrame_OnPointerExited_Original;
int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    int result = TaskbarFrame_OnPointerExited_Original(pThis, pArgs);
    if (!g_unloading) Try([&] {
        if (TaskbarFrameFromThis(pThis)) OnTaskbarPointerExited();
    });
    return result;
}

// ---- Jump list placement (as in "Taskbar Multirow") ----
// Centered on where the button is, not where layout put it (File Explorer), and
// as high as without the room left for the icons' bounce.

bool g_placingJumpList = false;
bool g_hasJumpListButton = false;
wf::Rect g_jumpListButton{};  // in its taskbar's XAML content

template <typename F>
void OpenJumpListFor(wux::UIElement const& sender, F original) {
    SuppressLabel();
    g_hasJumpListButton = false;
    if (!g_unloading) Try([&] {
        auto button = sender.try_as<wux::FrameworkElement>();
        auto root = button ? button.XamlRoot() : nullptr;
        auto content = root ? root.Content() : nullptr;
        if (content && FindButton(button)) {
            g_jumpListButton = button.TransformToVisual(content).TransformBounds(
                {0, 0, (float)button.ActualWidth(), (float)button.ActualHeight()});
            g_hasJumpListButton = true;
        }
    });
    original();
    g_hasJumpListButton = false;
}

using TaskbarResources_OnTaskListButtonContextRequested_t =
    void(WINAPI*)(void* pThis, wux::UIElement const& sender, void* args);
TaskbarResources_OnTaskListButtonContextRequested_t
    TaskbarResources_OnTaskListButtonContextRequested_Original;
void WINAPI TaskbarResources_OnTaskListButtonContextRequested_Hook(
    void* pThis, wux::UIElement const& sender, void* args) {
    OpenJumpListFor(sender, [&] {
        TaskbarResources_OnTaskListButtonContextRequested_Original(pThis, sender, args);
    });
}

// The same, on builds that skip the one above.
using TaskListButtonHandlers_HandleContextRequested_t =
    void(WINAPI*)(wux::UIElement const& sender, void* args);
TaskListButtonHandlers_HandleContextRequested_t
    TaskListButtonHandlers_HandleContextRequested_Original;
void WINAPI TaskListButtonHandlers_HandleContextRequested_Hook(wux::UIElement const& sender,
                                                              void* args) {
    OpenJumpListFor(sender, [&] {
        TaskListButtonHandlers_HandleContextRequested_Original(sender, args);
    });
}

using CTaskListWnd_ComputeJumpViewPosition_t =
    HRESULT(WINAPI*)(void* pThis, void* taskBtnGroup, int param2, wf::Point* point,
                     wux::HorizontalAlignment* horizontal, wux::VerticalAlignment* vertical);
CTaskListWnd_ComputeJumpViewPosition_t CTaskListWnd_ComputeJumpViewPosition_Original;
HRESULT WINAPI CTaskListWnd_ComputeJumpViewPosition_Hook(void* pThis, void* taskBtnGroup,
                                                         int param2, wf::Point* point,
                                                         wux::HorizontalAlignment* horizontal,
                                                         wux::VerticalAlignment* vertical) {
    g_placingJumpList = true;
    HRESULT hr = CTaskListWnd_ComputeJumpViewPosition_Original(pThis, taskBtnGroup, param2,
                                                               point, horizontal, vertical);
    g_placingJumpList = false;
    return hr;
}

using ToRawPixelsInDesktopCoordinates_t = RECT(WINAPI*)(wf::Rect const& rect, UINT dpi,
                                                         HWND hWnd);
ToRawPixelsInDesktopCoordinates_t ToRawPixelsInDesktopCoordinates_Original;
RECT WINAPI ToRawPixelsInDesktopCoordinates_Hook(wf::Rect const& rect, UINT dpi, HWND hWnd) {
    if (!g_placingJumpList || !g_hasJumpListButton || g_unloading) {
        return ToRawPixelsInDesktopCoordinates_Original(rect, dpi, hWnd);
    }
    wf::Rect moved = rect;
    moved.X = g_jumpListButton.X;
    moved.Width = g_jumpListButton.Width;
    float room = std::min(g_size.bounceRoom, moved.Height);
    moved.Y += room;
    moved.Height -= room;
    return ToRawPixelsInDesktopCoordinates_Original(moved, dpi, hWnd);
}

// ---- Dragging (as in "Taskbar Multirow") ----
// While File Explorer is kept first, its drag never starts. Other apps go no
// further than the first and last spots they can take, where they'd land if
// dropped there, so nothing jumps on release. Points are in the button row's
// coordinates.

winrt::weak_ref<wux::FrameworkElement> g_blockedDrag;
float g_dragMinX = 0, g_dragMaxX = 0;
bool g_dragLimited = false;

// The pointer's range that keeps the dragged icon within those spots: the icons
// of the first app (the second, after a File Explorer kept first) and the last,
// as they are when the drag starts.
void LimitDrag(wux::FrameworkElement const& button, wf::Point start) {
    g_dragLimited = false;
    auto row = wuxm::VisualTreeHelper::GetParent(button).try_as<wux::UIElement>();
    if (!row) return;
    std::vector<Icon> apps;
    const Icon* dragged = nullptr;
    auto icons = GetIcons(row);
    for (auto& icon : icons) {
        if (IsAppButton(icon.element)) apps.push_back(icon);
    }
    std::sort(apps.begin(), apps.end(), [](Icon const& a, Icon const& b) { return a.left < b.left; });
    for (auto& icon : apps) {
        if (icon.element == button) dragged = &icon;
    }
    if (apps.size() < 2 || !dragged) return;
    auto iconLeft = [](Icon const& i) { return i.left + (i.width - g_size.icon) / 2; };
    bool pastExplorer = g_settings.explorerFirst && IsFileExplorer(apps[0].element);
    g_dragMinX = start.X + iconLeft(apps[pastExplorer ? 1 : 0]) - iconLeft(*dragged);
    g_dragMaxX = start.X + iconLeft(apps.back()) - iconLeft(*dragged);
    g_dragLimited = g_dragMinX <= g_dragMaxX;
}

bool IsBlockedDrag(void* pThis) {
    bool blocked = false;
    Try([&] {
        auto button = g_blockedDrag.get();
        blocked = button && ElementFromThis(pThis) == button;
    });
    return blocked;
}

using TaskListButton_DragPoint_t = void(WINAPI*)(void* pThis, wf::Point point);

TaskListButton_DragPoint_t TaskListButton_OnDragStartedGesture_Original;
void WINAPI TaskListButton_OnDragStartedGesture_Hook(void* pThis, wf::Point point) {
    bool explorer = false;
    if (!g_unloading) Try([&] {
        auto button = ElementFromThis(pThis);
        explorer = g_settings.explorerFirst && button && IsFileExplorer(button);
        if (explorer) g_blockedDrag = winrt::make_weak(button);
    });
    if (explorer) return;
    TaskListButton_OnDragStartedGesture_Original(pThis, point);
    if (g_unloading) return;
    g_dragging = true;
    Try([&] {
        if (auto button = ElementFromThis(pThis)) {
            LimitDrag(button, point);
            DimDragged(button);
        }
        HideDots();
        StartFrames();
    }, L"Drag start");
}

TaskListButton_DragPoint_t TaskListButton_UpdateDrag_Original;
void WINAPI TaskListButton_UpdateDrag_Hook(void* pThis, wf::Point point) {
    if (IsBlockedDrag(pThis)) return;
    if (g_dragLimited && g_dragging && !g_unloading) {
        point.X = std::clamp(point.X, g_dragMinX, g_dragMaxX);
    }
    TaskListButton_UpdateDrag_Original(pThis, point);
    if (g_dragging && !g_unloading) Try(FlattenDragged);
}

VoidMethod_t TaskListButton_OnDragCompletedGesture_Original;
void WINAPI TaskListButton_OnDragCompletedGesture_Hook(void* pThis) {
    if (IsBlockedDrag(pThis)) {
        g_blockedDrag = nullptr;
        return;
    }
    TaskListButton_OnDragCompletedGesture_Original(pThis);
    if (!g_unloading) Try(EndDragging, L"Drag end");
}

// ---- File Explorer first ----
// Moved there for real by the taskbar's own drag, its drop aimed before every
// app. Again whenever an app's dropped before it. Given up after a few tries in
// a row, in case it ever goes wrong.

bool g_movingExplorer = false;
winrt::weak_ref<wux::FrameworkElement> g_explorerMoved;  // this time round
std::wstring UserValueName(PCWSTR name);

using TaskListDragOperation_GetDropPlaceholder_t =
    void*(WINAPI*)(void* pThis, void* result, void* itemBounds, UINT64 itemRange,
                   wf::Rect dragBounds, int rearrangeDirection, void* groupBoundsCalculator,
                   bool param7);
TaskListDragOperation_GetDropPlaceholder_t TaskListDragOperation_GetDropPlaceholder_Original;
void* WINAPI TaskListDragOperation_GetDropPlaceholder_Hook(void* pThis, void* result,
                                                           void* itemBounds, UINT64 itemRange,
                                                           wf::Rect dragBounds,
                                                           int rearrangeDirection,
                                                           void* groupBoundsCalculator,
                                                           bool param7) {
    // Left of the leftmost app (a std::vector's first two members are its begin
    // and end), narrowed to a sliver.
    auto bounds = (wf::Rect* const*)itemBounds;
    if (g_movingExplorer && bounds && bounds[0] != bounds[1]) {
        float x = std::numeric_limits<float>::max();
        for (auto* r = bounds[0]; r != bounds[1]; r++) x = std::min(x, r->X);
        dragBounds.X = x - 2;
        dragBounds.Width = 2;
    }
    return TaskListDragOperation_GetDropPlaceholder_Original(pThis, result, itemBounds, itemRange,
                                                             dragBounds, rearrangeDirection,
                                                             groupBoundsCalculator, param7);
}

bool CanMoveExplorer(wux::FrameworkElement const& explorer, int tries) {
    Button* b = FindButton(explorer);
    return b && b->impl && tries < 3 && TaskListButton_OnDragStartedGesture_Original &&
           TaskListButton_UpdateDrag_Original && TaskListButton_OnDragCompletedGesture_Original &&
           TaskListDragOperation_GetDropPlaceholder_Original &&
           ElementFromThis(b->impl) == explorer;
}

// Once layout is done, from the mod window's message.
void MoveExplorerFirst(wux::FrameworkElement const& explorer) {
    int tries = Wh_GetIntValue(UserValueName(L"explorerMoves").c_str(), 0);
    if (g_explorerMoved.get() == explorer || !CanMoveExplorer(explorer, tries)) return;
    HWND w = EnsureModWindow();
    if (w && PostMessage(w, kMoveExplorerMessage, 0, 0)) {
        g_explorerMoved = winrt::make_weak(explorer);
    }
}

void MoveExplorerNow() {
    auto explorer = g_explorerMoved.get();
    auto name = UserValueName(L"explorerMoves");
    int tries = Wh_GetIntValue(name.c_str(), 0);
    if (!g_settings.explorerFirst || g_dragging || !explorer || !CanMoveExplorer(explorer, tries)) {
        return;
    }
    Try([&] {
        void* impl = FindButton(explorer)->impl;
        Wh_SetIntValue(name.c_str(), tries + 1);
        auto offset = explorer.ActualOffset();
        wf::Point at{offset.x + (float)explorer.ActualWidth() / 2,
                     offset.y + (float)explorer.ActualHeight() / 2};
        g_movingExplorer = true;
        TaskListButton_OnDragStartedGesture_Original(impl, at);
        TaskListButton_UpdateDrag_Original(impl, {at.X - 1, at.Y});
        TaskListButton_OnDragCompletedGesture_Original(impl);
    }, L"Moving File Explorer");
    g_movingExplorer = false;
    MarkPlatesDirty();
}

void NoteExplorerFirst() {
    if (!g_explorerMoved.get()) return;
    g_explorerMoved = nullptr;
    Wh_SetIntValue(UserValueName(L"explorerMoves").c_str(), 0);
}

// =============================================================================
// HOOKS: TASKBAR HEIGHT
// =============================================================================
// Based on m417z's "Taskbar height and icon size", trimmed to the height.
// Everything passes straight through when g_taskbarHeight is 0.

std::atomic<bool> g_applyingHeight = false;
std::atomic<bool> g_trayResized = false;  // since the last height change
thread_local bool g_inTrayUpdateFrameSize = false;

// enumTaskbarSize: 0 = small, 1 = regular, 2 = large.
bool OverrideHeight(int enumTaskbarSize) {
    return g_taskbarHeight > 0 && (enumTaskbarSize == 1 || enumTaskbarSize == 2);
}

// The offset `regex` captures (in hex) from the first of the `limit`
// instructions from `code` it matches, or 0. Stops at a ret.
LONG OffsetFromDisasm(const void* code, int limit, std::regex const& regex) {
    auto p = (BYTE*)code;
    for (int i = 0; p && i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) break;
        p += result.length;
        std::string_view text = result.text;
        if (text == "ret") break;
        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(text.begin(), text.end(), match, regex)) {
            size_t offset = 0;
            std::from_chars(&*match[1].first, &*match[1].first + match[1].length(), offset, 16);
            return offset > 0 && offset <= 0xFFFF ? (LONG)offset : 0;
        }
    }
    return 0;
}

// ---- The taskbar window ----

using TaskbarConfiguration_GetFrameSize_t = double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    return OverrideHeight(enumTaskbarSize)
               ? g_taskbarHeight
               : TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}

#ifdef _M_ARM64
// On ARM64, GetFrameSize is inlined into UpdateFrameSize, so the size it
// stores is replaced just before its change event fires. Its offset is read
// from str dN, [xN, #offset].
thread_local double* g_updatingFrameSize = nullptr;
VoidMethod_t TaskbarConfiguration_UpdateFrameSize_Address;  // hooked by address, below
VoidMethod_t TaskbarConfiguration_UpdateFrameSize_Original;

LONG FrameSizeOffset() {
    static LONG offset = [] {
        LONG found = OffsetFromDisasm((void*)TaskbarConfiguration_UpdateFrameSize_Address, 0x80,
                                      std::regex(R"(str\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])"));
        if (found) Wh_Log(L"Frame size offset: 0x%X", found);
        else Wh_Log(L"Frame size offset not found");
        return found;
    }();
    return offset;
}

void WINAPI TaskbarConfiguration_UpdateFrameSize_Hook(void* pThis) {
    LONG offset = FrameSizeOffset();
    g_updatingFrameSize = offset ? (double*)((BYTE*)pThis + offset) : nullptr;
    TaskbarConfiguration_UpdateFrameSize_Original(pThis);
    g_updatingFrameSize = nullptr;
}

VoidMethod_t Event_operator_call_Original;
void WINAPI Event_operator_call_Hook(void* pThis) {
    if (g_updatingFrameSize && g_taskbarHeight > 0) *g_updatingFrameSize = g_taskbarHeight;
    Event_operator_call_Original(pThis);
}

void HookFrameSizeUpdate() {
    if (!TaskbarConfiguration_UpdateFrameSize_Address ||
        TaskbarConfiguration_UpdateFrameSize_Original) {
        return;
    }
    FrameSizeOffset();  // found up front
    WindhawkUtils::SetFunctionHook(TaskbarConfiguration_UpdateFrameSize_Address,
                                   TaskbarConfiguration_UpdateFrameSize_Hook,
                                   &TaskbarConfiguration_UpdateFrameSize_Original);
}
#endif

// ---- The system tray, on the main taskbar and the others ----

using SystemTray_GetFrameSize_t = double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTray_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
SystemTray_GetFrameSize_t SystemTraySecondaryController_GetFrameSize_Original;

template <auto& Original>
double WINAPI SystemTray_GetFrameSize_Hook(void* pThis, int enumTaskbarSize) {
    return OverrideHeight(enumTaskbarSize) ? g_taskbarHeight : Original(pThis, enumTaskbarSize);
}

// While the tray resizes itself, its frame gets the taskbar's height.
VoidMethod_t SystemTrayController_UpdateFrameSize_Address;  // hooked by address, below
VoidMethod_t SystemTrayController_UpdateFrameSize_Original;
VoidMethod_t SystemTraySecondaryController_UpdateFrameSize_Original;

template <auto& Original>
void WINAPI SystemTray_UpdateFrameSize_Hook(void* pThis) {
    g_inTrayUpdateFrameSize = true;
    Original(pThis);
    g_inTrayUpdateFrameSize = false;
    g_trayResized = true;
}

// The main tray skips resizing when the height is the one it last set, and
// would keep the dock's when the mod unloads. That's forgotten first, so any new
// height applies. Found in the function's code (as in "Taskbar height and icon
// size"): on x64, ucomisd xmmN, [reg + offset], then jp, then je or jne; on
// ARM64, ldr dN, [xN, #offset], then fcmp, then b.eq.
LONG TrayLastHeightOffset() {
    static LONG offset = [] {
#if defined(_M_X64)
        auto start = (BYTE const*)SystemTrayController_UpdateFrameSize_Address;
        for (auto p = start; start && p != start + 0x400; p++) {
            if (p[0] == 0x66 && p[1] == 0x0F && p[2] == 0x2E && (p[3] & 0xC0) == 0x80 &&
                p[8] == 0x7A &&
                (p[10] == 0x74 || p[10] == 0x75 ||
                 (p[10] == 0x0F && (p[11] == 0x84 || p[11] == 0x85)))) {
                LONG found = *(LONG const*)(p + 4);
                Wh_Log(L"Tray height offset: 0x%X", found);
                return found > 0 && found <= 0xFFFF ? found : 0L;
            }
        }
#elif defined(_M_ARM64)
        std::regex load(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        std::regex compare(R"(fcmp\s+d\d+, d\d+)");
        std::regex branch(R"(b\.eq\s+0x[0-9a-f]+)");
        auto start = (DWORD const*)SystemTrayController_UpdateFrameSize_Address;
        for (auto p = start; start && p != start + 0x100; p++) {
            WH_DISASM_RESULT r[3];
            if (!Wh_Disasm((void*)p, &r[0]) || !strcmp(r[0].text, "ret")) break;
            std::cmatch match;
            if (std::regex_match(r[0].text, match, load) && Wh_Disasm((void*)(p + 1), &r[1]) &&
                std::regex_match(r[1].text, compare) && Wh_Disasm((void*)(p + 2), &r[2]) &&
                std::regex_match(r[2].text, branch)) {
                LONG found = std::stol(match[1], nullptr, 16);
                Wh_Log(L"Tray height offset: 0x%X", found);
                return found > 0 && found <= 0xFFFF ? found : 0L;
            }
        }
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"Tray height offset not found");
        return 0L;
    }();
    return offset;
}

void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    if (LONG offset = TrayLastHeightOffset()) *(double*)((BYTE*)pThis + offset) = 0;
    SystemTray_UpdateFrameSize_Hook<SystemTrayController_UpdateFrameSize_Original>(pThis);
}

void HookTrayUpdateFrameSize() {
    if (!SystemTrayController_UpdateFrameSize_Address || SystemTrayController_UpdateFrameSize_Original) {
        return;
    }
    TrayLastHeightOffset();  // found up front
    WindhawkUtils::SetFunctionHook(SystemTrayController_UpdateFrameSize_Address,
                                   SystemTrayController_UpdateFrameSize_Hook,
                                   &SystemTrayController_UpdateFrameSize_Original);
}

SetDouble_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    if (g_inTrayUpdateFrameSize && g_taskbarHeight > 0) value = g_taskbarHeight;
    SystemTrayFrame_Height_Original(pThis, value);
}

// ---- The XAML frame ----
// Without lifting MaxHeight, the frame stays at the stock height.

SetDouble_t TaskbarFrame_MaxHeight_Original;

// Windows 11 22H2/23H2.
SetDouble_t TaskbarFrame_Height_Original;
void WINAPI TaskbarFrame_Height_Hook(void* pThis, double value) {
    if (TaskbarFrame_MaxHeight_Original) {
        TaskbarFrame_MaxHeight_Original(pThis, std::numeric_limits<double>::infinity());
    }
    TaskbarFrame_Height_Original(pThis, value);
}

// Windows 11 24H2 and newer. The controller's TaskbarFrame offset is read from
// the start of OnGroupingModeChanged: on x64, sub rsp, 28h / mov rax|r8,
// [rcx+offset]; on ARM64, the first ldr xN, [xN, #offset].
void* TaskbarController_OnGroupingModeChanged_Address;

LONG GetTaskbarFrameOffset() {
    static LONG offset = []() -> LONG {
#if defined(_M_X64)
        const BYTE* p = (const BYTE*)TaskbarController_OnGroupingModeChanged_Address;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B && (p[6] & 0xC0) == 0x80) {
            LONG o = *(LONG*)(p + 7);
            if (o > 0 && o <= 0xFFFF) return o;
        }
#elif defined(_M_ARM64)
        LONG o = OffsetFromDisasm(TaskbarController_OnGroupingModeChanged_Address, 10,
                                  std::regex(R"(ldr\s+x\d+, \[x\d+, #0x([0-9a-f]+)\])"));
        if (o) return o;
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"Taskbar frame offset not found");
        return 0;
    }();
    return offset;
}

VoidMethod_t TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    wux::FrameworkElement frame{nullptr};
    Try([&] {
        LONG offset = GetTaskbarFrameOffset();
        void* taskbarFrame = offset ? *(void**)((BYTE*)pThis + offset) : nullptr;
        if (taskbarFrame) {
            ((::IUnknown**)taskbarFrame)[1]->QueryInterface(
                winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(frame));
        }
        if (frame) frame.MaxHeight(std::numeric_limits<double>::infinity());
    });

    TaskbarController_UpdateFrameHeight_Original(pThis);
    if (!frame) return;

    if (!g_unloading) Try([&] { NoteTaskbarFrame(frame); });  // found at startup too
    Try([&] {
        // The grid holding the frame has a height of its own.
        auto grid = wuxm::VisualTreeHelper::GetParent(frame).try_as<wux::FrameworkElement>();
        if (grid && grid.Height() > 0 && grid.Height() != frame.Height()) {
            grid.Height(frame.Height());
        }
    });
}

// ---- Applying a new height ----
// Explorer re-reads it on a settings change; this also resizes the window.

VoidMethod_t TrayUI__StuckTrayChange_Original;

using TrayUI__HandleSettingChange_t = void(WINAPI*)(void*, void*, void*, void*, void*);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis, void* p1, void* p2, void* p3,
                                             void* p4) {
    TrayUI__HandleSettingChange_Original(pThis, p1, p2, p3, p4);
    if (g_applyingHeight) TrayUI__StuckTrayChange_Original(pThis);
}

void ApplyTaskbarHeight() {
    HWND tb = FindTaskbar();
    if (!tb) return;
    g_applyingHeight = true;
    SendMessage(tb, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    g_applyingHeight = false;

    // Reloads the icons, at the new resolution.
    HWND rebar = FindWindowEx(tb, nullptr, L"ReBarWindow32", nullptr);
    HWND taskSw = rebar ? FindWindowEx(rebar, nullptr, L"MSTaskSwWClass", nullptr) : nullptr;
    if (taskSw) SendMessage(taskSw, 0x452, 3, 0);
}

// =============================================================================
// HOOKS: ICON RESOLUTION
// =============================================================================
// Icons load at the fully magnified size, while layout code still sees the
// stock 24px. Adapted from m417z's "Taskbar height and icon size".

thread_local bool g_inTaskbarFrameGetMetrics = false;  // picks the button width

GetDouble_t TaskbarConfiguration_GetIconHeightInViewPixels_Original;
double WINAPI TaskbarConfiguration_GetIconHeightInViewPixels_Hook(void* pThis) {
    double height = TaskbarConfiguration_GetIconHeightInViewPixels_Original(pThis);
    // 16 = "small taskbar buttons", which isn't supported.
    if (g_size.iconLoadSize <= 0 || g_inTaskbarFrameGetMetrics || height <= 16) return height;
    return g_size.iconLoadSize;
}

using TaskbarFrame_GetMetrics_t = void*(WINAPI*)(void* pThis, void* metrics);
TaskbarFrame_GetMetrics_t TaskbarFrame_GetMetrics_Original;
void* WINAPI TaskbarFrame_GetMetrics_Hook(void* pThis, void* metrics) {
    g_inTaskbarFrameGetMetrics = true;
    void* ret = TaskbarFrame_GetMetrics_Original(pThis, metrics);
    g_inTaskbarFrameGetMetrics = false;
    return ret;
}

// ---- App buttons: layout keeps seeing 24px ----

// Read from the IconHeight setter: movsd xmmN, qword ptr [rcx+offset] on x64,
// ldr dN, [xN, #offset] on ARM64.
void* TaskListButton_IconHeight_Address;

size_t GetIconHeightOffset() {
    static size_t offset = []() -> size_t {
#if defined(_M_X64)
        std::regex regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                         std::regex_constants::icase);
#elif defined(_M_ARM64)
        std::regex regex(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])", std::regex_constants::icase);
#else
#error "Unsupported architecture"
#endif
        LONG o = OffsetFromDisasm(TaskListButton_IconHeight_Address, 30, regex);
        if (o) Wh_Log(L"Icon height offset: 0x%X", o);
        else Wh_Log(L"Icon height offset not found; icon resolution unchanged");
        return o;
    }();
    return offset;
}

// Runs `original` with the button's icon height at the stock 24px.
template <typename F>
void WithStockIconHeight(void* pThis, F original) {
    size_t offset = g_size.iconLoadSize > 0 ? GetIconHeightOffset() : 0;
    if (!offset) return original();
    double* iconHeight = (double*)((BYTE*)pThis + offset);
    double saved = *iconHeight;
    *iconHeight = kStockIconSize;
    original();
    *iconHeight = saved;
}

template <auto& Original>
void WINAPI StockIconHeight_Hook(void* pThis) {
    WithStockIconHeight(pThis, [=] { Original(pThis); });
}

// These also run whenever a button is set up or changes state.
template <auto& Original>
void WINAPI ButtonUpdate_Hook(void* pThis) {
    WithStockIconHeight(pThis, [=] { Original(pThis); });
    OnTaskListButtonUpdated(pThis);
}

VoidMethod_t TaskListButton_UpdateMultiWindowClip_Original;
VoidMethod_t TaskListButton_CreateMultiWindowClip_Original;
VoidMethod_t TaskListButton_UpdateBadge_Original;
VoidMethod_t TaskListButton_UpdateButtonPadding_Original;
VoidMethod_t TaskListButton_UpdateVisualStates_Original;

// Puts the icon column back to its stock width; it's widened again after.
VoidMethod_t TaskListButton_UpdateIconColumnDefinition_Original;
void WINAPI TaskListButton_UpdateIconColumnDefinition_Hook(void* pThis) {
    TaskListButton_UpdateIconColumnDefinition_Original(pThis);
    OnTaskListButtonUpdated(pThis);
}

using TaskListButton_OverlayIcon_t = void(WINAPI*)(void* pThis, void* stream);
TaskListButton_OverlayIcon_t TaskListButton_OverlayIcon_Original;
void WINAPI TaskListButton_OverlayIcon_Hook(void* pThis, void* stream) {
    WithStockIconHeight(pThis, [=] { TaskListButton_OverlayIcon_Original(pThis, stream); });
    if (!g_unloading) Try([&] {
        if (auto button = ElementFromThis(pThis)) ProcessButton(button);  // places a new badge
    });
}

// ---- Start, Task View, widgets and other system buttons ----
// Shown at the dock's icon size, whatever "High resolution icons" is set to.

template <auto& Original>
void WINAPI DockIconHeight_Hook(void* pThis, double height) {
    if (DockSizesIcons() && height > 16) height = g_size.icon;
    Original(pThis, height);
}

SetDouble_t ExperienceToggleButton_IconHeight_Original;
SetDouble_t TaskbarComponentHost_IconHeight_Original;
GetDouble_t ExperienceToggleButton_IconHeight_get;

// Padding is worked out for the stock 24px. Setting the height calls
// UpdateButtonPadding again, which the flag skips.
thread_local bool g_settingToggleIconHeight = false;

void SetToggleIconHeight(void* pThis, double height) {
    g_settingToggleIconHeight = true;
    ExperienceToggleButton_IconHeight_Original(pThis, height);
    g_settingToggleIconHeight = false;
}

VoidMethod_t ExperienceToggleButton_UpdateButtonPadding_Original;
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    if (g_settingToggleIconHeight) return;
    double height = kStockIconSize;
    if (DockSizesIcons() && ExperienceToggleButton_IconHeight_get &&
        ExperienceToggleButton_IconHeight_Original) {
        height = ExperienceToggleButton_IconHeight_get(pThis);
    }
    bool swap = height > 16 && height != kStockIconSize;
    if (swap) SetToggleIconHeight(pThis, kStockIconSize);
    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    if (swap) SetToggleIconHeight(pThis, height);
}

// =============================================================================
// HOOKS: WINDOW PREVIEWS
// =============================================================================
// With "Window previews" on, hovering opens Windows' preview, just above the
// dock's label. Off, previews only open on click, as in m417z's "Disable
// Taskbar Thumbnails".

// ---- Older builds ----

using CTaskListWnd__DisplayExtendedUI_t = HRESULT(WINAPI*)(void* pThis, void* taskBtnGroup,
                                                           int param2, DWORD flags, int param4);
CTaskListWnd__DisplayExtendedUI_t CTaskListWnd__DisplayExtendedUI_Original;
HRESULT WINAPI CTaskListWnd__DisplayExtendedUI_Hook(void* pThis, void* taskBtnGroup, int param2,
                                                    DWORD flags, int param4) {
    bool click = flags & 2;
    if (!click && !g_unloading && !g_settings.previews) return S_OK;
    return CTaskListWnd__DisplayExtendedUI_Original(pThis, taskBtnGroup, param2, flags, param4);
}

using CTaskListWnd__ShowToolTip_t = void(WINAPI*)(void* pThis, DWORD flags);
CTaskListWnd__ShowToolTip_t CTaskListWnd__ShowToolTip_Original;
void WINAPI CTaskListWnd__ShowToolTip_Hook(void* pThis, DWORD flags) {
    if (g_unloading) CTaskListWnd__ShowToolTip_Original(pThis, flags);
}

// ---- Newer builds ----
// A XAML flyout. Without previews, while it opens on hover its window is kept
// hidden, and it's shown after all if it then turns into a click (sticky) flyout.

struct HoverFlyout {
    HWND hwnd = nullptr;
    int show = SW_HIDE;
    BOOL enable = FALSE;
};

std::atomic<DWORD> g_hoverFlyoutThread = 0;
bool g_inStickyFlyout = false;
HoverFlyout g_hoverFlyout;

bool InHoverFlyout() {
    return !g_settings.previews && g_hoverFlyoutThread == GetCurrentThreadId() &&
           !g_inStickyFlyout;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (InHoverFlyout()) {
        if (nCmdShow != SW_HIDE) {
            if (hWnd != g_hoverFlyout.hwnd) g_hoverFlyout = {hWnd};
            g_hoverFlyout.show = nCmdShow;
            return TRUE;
        }
        if (hWnd == g_hoverFlyout.hwnd) g_hoverFlyout.show = SW_HIDE;
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

using EnableWindow_t = decltype(&EnableWindow);
EnableWindow_t EnableWindow_Original;
BOOL WINAPI EnableWindow_Hook(HWND hWnd, BOOL bEnable) {
    if (InHoverFlyout()) {
        if (bEnable) {
            if (hWnd != g_hoverFlyout.hwnd) g_hoverFlyout = {hWnd};
            g_hoverFlyout.enable = TRUE;
            return TRUE;
        }
        if (hWnd == g_hoverFlyout.hwnd) g_hoverFlyout.enable = FALSE;
    }
    return EnableWindow_Original(hWnd, bEnable);
}

using HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_t = void(WINAPI*)(void* pThis,
                                                                               void* param1);
HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_t
    HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Original;
void WINAPI HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Hook(void* pThis,
                                                                      void* param1) {
    g_inStickyFlyout = true;
    HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Original(pThis, param1);
    g_inStickyFlyout = false;
    if (HWND w = g_hoverFlyout.hwnd) {
        if (g_hoverFlyout.show != SW_HIDE) ShowWindow_Original(w, g_hoverFlyout.show);
        if (g_hoverFlyout.enable) EnableWindow_Original(w, TRUE);
    }
    g_hoverFlyout = {};
}

// The current signature, and the one before March 2025.
using HoverFlyoutController_ShowTaskListButtonHoverFlyout_t =
    void(WINAPI*)(void* pThis, void* param1, void* param2, int param3, int param4);
HoverFlyoutController_ShowTaskListButtonHoverFlyout_t
    HoverFlyoutController_ShowTaskListButtonHoverFlyout_Original;
void WINAPI HoverFlyoutController_ShowTaskListButtonHoverFlyout_Hook(void* pThis, void* param1,
                                                                    void* param2, int param3,
                                                                    int param4) {
    g_hoverFlyoutThread = g_unloading ? 0 : GetCurrentThreadId();
    HoverFlyoutController_ShowTaskListButtonHoverFlyout_Original(pThis, param1, param2, param3,
                                                                 param4);
    g_hoverFlyoutThread = 0;
}

using HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old_t =
    void(WINAPI*)(void* pThis, void* param1, void* param2, int param3);
HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old_t
    HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old_Original;
void WINAPI HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old_Hook(void* pThis, void* param1,
                                                                        void* param2,
                                                                        int param3) {
    g_hoverFlyoutThread = g_unloading ? 0 : GetCurrentThreadId();
    HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old_Original(pThis, param1, param2, param3);
    g_hoverFlyoutThread = 0;
}

// ---- Placement ----
// Windows puts a preview above its app's button, which reaches well past the
// icons. It's moved to just above the label at its highest, and sideways onto
// the magnified icon under the pointer (as "Taskbar on top" moves them).

constexpr float kPreviewGap = 6;        // between it and the label
constexpr float kFlyoutStockGap = 12;   // under a flyout, above its app's button

// How far to move a preview: a window Windows put at `rect` on screen, or a
// flyout (null), which it puts kFlyoutStockGap above its app's button. Buttons
// reach above the taskbar window.
POINT PreviewOffset(RECT const* rect) {
    HWND tb = FindTaskbar();  // the dock is only read on its own thread
    if (!tb || GetWindowThreadProcessId(tb, nullptr) != GetCurrentThreadId()) return {};
    Plate* p = GetRowPlate(g_rowParent.get());
    if (!p) return {};

    float iconTop = p->top + g_size.padY + g_size.icon * (1 - g_settings.scale);
    float top = g_settings.tooltips
                    ? LabelTipY(*p, g_size.icon * g_settings.scale) - kLabelArrow - kLabelHeight
                    : std::min(iconTop, p->top);
    float shift = 0;
    wux::FrameworkElement button{nullptr};
    for (auto& item : g_row) {
        auto el = item.trash ? nullptr : item.element.get();
        if (!el) continue;
        if (!button) button = el;
        float d = std::fabs(g_pointerX - item.center - item.shift);
        if (g_pointerOver && d <= item.width * item.scale / 2) {
            shift = item.shift;
            button = el;
        }
    }
    auto panel = p->panel.get();
    if (!button || !panel) return {};
    float buttonTop = button.TransformToVisual(panel).TransformPoint({0, 0}).Y;

    POINT bottom, stock;
    RECT window;
    float scale = PanelToScreen(*p, {0, top - kPreviewGap}, bottom, window);
    if (scale <= 0 || !PanelToScreen(*p, {0, buttonTop - kFlyoutStockGap}, stock, window)) {
        return {};
    }
    LONG stockBottom = rect ? rect->bottom : stock.y;
    POINT offset{std::lround(shift * scale), bottom.y - stockBottom};

    // A window is kept on the screen.
    MONITORINFO info{sizeof(info)};
    if (rect && GetMonitorInfo(MonitorFromRect(&window, MONITOR_DEFAULTTONEAREST), &info)) {
        offset.x = std::max(info.rcMonitor.left - rect->left,
                            std::min(offset.x, info.rcMonitor.right - rect->right));
        offset.y = std::max(offset.y, info.rcMonitor.top - rect->top);
    }
    return offset;
}

// Newer builds: a XAML flyout. FlyoutFrame::UpdateFlyoutPosition maps where
// it goes to the screen, and every point it maps is moved.

thread_local void* g_placingFlyout = nullptr;

VoidMethod_t FlyoutFrame_UpdateFlyoutPosition_Original;
void WINAPI FlyoutFrame_UpdateFlyoutPosition_Hook(void* pThis) {
    void* outer = std::exchange(g_placingFlyout, pThis);
    FlyoutFrame_UpdateFlyoutPosition_Original(pThis);
    g_placingFlyout = outer;
}

// Whether a flyout frame holds a preview, not another flyout.
bool IsPreview(void* flyoutFrame) {
    wux::FrameworkElement frame{nullptr};
    ((::IUnknown*)flyoutFrame)
        ->QueryInterface(winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(frame));
    if (!frame) {  // before build 26120.4733
        ((::IUnknown**)flyoutFrame)[1]->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                                       winrt::put_abi(frame));
    }
    bool found = false;
    if (frame) {
        ForEachDescendant(frame, 2, [&found](wux::FrameworkElement const& el) {
            found = found || el.Name() == L"HoverFlyoutGrid";
            return !found;
        });
    }
    return found;
}

using MapWindowPoints_t = decltype(&MapWindowPoints);
MapWindowPoints_t MapWindowPoints_Original;
int WINAPI MapWindowPoints_Hook(HWND hWndFrom, HWND hWndTo, LPPOINT lpPoints, UINT cPoints) {
    int result = MapWindowPoints_Original(hWndFrom, hWndTo, lpPoints, cPoints);
    if (!g_placingFlyout || !lpPoints || !cPoints || g_unloading) return result;
    void* flyout = std::exchange(g_placingFlyout, nullptr);  // not again from in here
    Try([&] {
        if (!IsPreview(flyout)) return;
        POINT offset = PreviewOffset(nullptr);
        for (UINT i = 0; i < cPoints; i++) {
            lpPoints[i].x += offset.x;
            lpPoints[i].y += offset.y;
        }
    });
    g_placingFlyout = flyout;
    return result;
}

// Older builds: a window, placed while CTaskListThumbnailWnd shows it or lays
// out its thumbnails.

thread_local bool g_placingThumbnails = false;

using CTaskListThumbnailWnd_DisplayUI_t = int(WINAPI*)(void* pThis, void* taskBtnGroup,
                                                       void* item1, void* item2, DWORD flags);
CTaskListThumbnailWnd_DisplayUI_t CTaskListThumbnailWnd_DisplayUI_Original;
int WINAPI CTaskListThumbnailWnd_DisplayUI_Hook(void* pThis, void* taskBtnGroup, void* item1,
                                                void* item2, DWORD flags) {
    bool outer = std::exchange(g_placingThumbnails, true);
    int result = CTaskListThumbnailWnd_DisplayUI_Original(pThis, taskBtnGroup, item1, item2, flags);
    g_placingThumbnails = outer;
    return result;
}

VoidMethod_t CTaskListThumbnailWnd_LayoutThumbnails_Original;
void WINAPI CTaskListThumbnailWnd_LayoutThumbnails_Hook(void* pThis) {
    bool outer = std::exchange(g_placingThumbnails, true);
    CTaskListThumbnailWnd_LayoutThumbnails_Original(pThis);
    g_placingThumbnails = outer;
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND after, int X, int Y, int cx, int cy, UINT flags) {
    WCHAR cls[32];
    RECT rect;
    if (g_placingThumbnails && !g_unloading && !(flags & SWP_NOMOVE) &&
        GetClassName(hWnd, cls, ARRAYSIZE(cls)) && !_wcsicmp(cls, L"TaskListThumbnailWnd") &&
        GetWindowRect(hWnd, &rect)) {
        if (!(flags & SWP_NOSIZE)) rect = {X, Y, X + cx, Y + cy};
        else OffsetRect(&rect, X - rect.left, Y - rect.top);
        Try([&] {
            POINT offset = PreviewOffset(&rect);
            X += offset.x;
            Y += offset.y;
        });
    }
    return SetWindowPos_Original(hWnd, after, X, Y, cx, cy, flags);
}

// =============================================================================
// HOOKS: MAXIMIZED WINDOWS
// =============================================================================
// Maximized windows reach down to the dock, and magnified icons rise over them:
// - Auto-hide is turned on, so Windows keeps no space for the taskbar, which
//   is kept from hiding (as in "Taskbar auto-hide when maximized").
// - With Auto-hide Off, an app bar keeps the dock's strip free on each screen.
//   Otherwise windows always have the whole screen, so none is moved when the
//   dock shows or hides.
// - The taller taskbar window is clipped to that strip while not magnifying,
//   and to the tray (if shown) while the dock is hidden.

constexpr UINT kTraySettingMessage = WM_USER + 0x1CA;  // TrayUI::_HandleTrayPrivateSettingMessage
constexpr WPARAM kTraySettingAutoHideGet = 3;
constexpr WPARAM kTraySettingAutoHideSet = 4;
constexpr UINT_PTR kTrayHideTimer = 2;

// Saved per user, so auto-hide is put back even after Explorer restarts.
bool g_managingAutoHide = false;
bool g_autoHideBefore = false;

std::wstring UserValueName(PCWSTR name) {
    static std::wstring sid = [] {
        std::wstring result;
        HANDLE token;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return result;
        alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
        DWORD size;
        PWSTR text;
        if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size) &&
            ConvertSidToStringSidW(((TOKEN_USER*)buffer)->User.Sid, &text)) {
            result = text;
            LocalFree(text);
        }
        CloseHandle(token);
        return result;
    }();
    return std::wstring(name) + L"_" + sid;
}

bool KeepTaskbarShown() { return !g_unloading; }

void ApplyAutoHide() {
    HWND tb = FindTaskbar();
    if (!tb) return;
    if (KeepTaskbarShown()) {
        if (!g_managingAutoHide) {
            g_autoHideBefore = SendMessage(tb, kTraySettingMessage, kTraySettingAutoHideGet, 0);
            g_managingAutoHide = true;
        }
        SendMessage(tb, kTraySettingMessage, kTraySettingAutoHideSet, TRUE);
    } else if (g_managingAutoHide) {
        SendMessage(tb, kTraySettingMessage, kTraySettingAutoHideSet, g_autoHideBefore);
        g_managingAutoHide = false;
    }
    Wh_SetIntValue(UserValueName(L"managingAutoHide").c_str(), g_managingAutoHide);
    Wh_SetIntValue(UserValueName(L"autoHideBefore").c_str(), g_autoHideBefore);
}

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    if (!KeepTaskbarShown()) return TrayUI__Hide_Original(pThis);
    if (HWND tb = FindTaskbar()) KillTimer(tb, kTrayHideTimer);
}

using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
    if (!KeepTaskbarShown()) CSecondaryTray__AutoHide_Original(pThis, param1);
}

// The strip kept free: the dock and the gap under it, or the tray if taller.
int StripHeight(UINT dpi) {
    float height = std::max(g_size.bottomGap + g_size.dockHeight, (float)kStockTaskbarHeight);
    return MulDiv((int)std::ceil(height), dpi, 96);
}

// While an app's icon slides in or out of a shown dock, Windows' animation can
// rise past the strip, so the whole window shows for a moment.
constexpr UINT kIconsMovingMs = 800;
ULONGLONG g_iconsMovingUntil = 0;

bool IconsMoving() { return GetTickCount64() < g_iconsMovingUntil && !DockHidden(); }

void NoteIconsMoving() {
    HWND w = g_modWindow;
    if (!w || !SetTimer(w, kIconsMovingTimer, kIconsMovingMs, nullptr)) return;
    g_iconsMovingUntil = GetTickCount64() + kIconsMovingMs;
    ScheduleOverWindows(false);
}

// No region (null) shows the whole window.
HRGN TaskbarRegion(HWND tb) {
    RECT window;
    if (!KeepTaskbarShown() || g_dockExpanded || IconsMoving() || !GetWindowRect(tb, &window)) {
        return nullptr;
    }
    int width = window.right - window.left, height = window.bottom - window.top;
    UINT dpi = GetDpiForWindow(tb);
    if (!DockFullyHidden()) {
        return CreateRectRgn(0, std::max(0, height - StripHeight(dpi)), width, height);
    }
    if (!TrayShown()) return CreateRectRgn(0, 0, 0, 0);
    int trayWidth = g_trayWidth > 0 ? g_trayWidth : MulDiv(200, dpi, 96);
    int trayHeight = MulDiv((int)kStockTaskbarHeight, dpi, 96);
    return CreateRectRgn(std::max(0, width - trayWidth), height - trayHeight, width, height);
}

void ClipTaskbars() {
    ForEachTaskbarWindow([](HWND tb, bool) {
        HRGN want = TaskbarRegion(tb);
        HRGN have = CreateRectRgn(0, 0, 0, 0);
        bool hasRegion = GetWindowRgn(tb, have) != ERROR;
        bool same = want ? hasRegion && EqualRgn(have, want) : !hasRegion;
        DeleteObject(have);
        if (!same) SetWindowRgn(tb, want, TRUE);  // the window owns it now
        else if (want) DeleteObject(want);
    });
}

struct AppBar {
    HWND hwnd;
    RECT rect;
};
std::vector<AppBar> g_appBars;

void RemoveAppBars() {
    for (auto& bar : g_appBars) {
        APPBARDATA abd{sizeof(abd)};
        abd.hWnd = bar.hwnd;
        SHAppBarMessage(ABM_REMOVE, &abd);
        DestroyWindow(bar.hwnd);
    }
    g_appBars.clear();
}

void AddAppBar(RECT const& rect) {
    HWND w = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWindowClass, L"", WS_POPUP, 0, 0,
                            0, 0, nullptr, nullptr, ModuleInstance(), nullptr);
    if (!w) return;
    g_appBars.push_back({w, rect});

    APPBARDATA abd{sizeof(abd)};
    abd.hWnd = w;
    abd.uCallbackMessage = kAppBarMessage;
    SHAppBarMessage(ABM_NEW, &abd);
    abd.uEdge = ABE_BOTTOM;
    abd.rc = rect;
    SHAppBarMessage(ABM_QUERYPOS, &abd);
    abd.rc.top = abd.rc.bottom - (rect.bottom - rect.top);
    SHAppBarMessage(ABM_SETPOS, &abd);
}

// Only changed when they differ: changes are announced to all app bars, these
// included, which would otherwise loop.
void UpdateAppBars() {
    std::vector<RECT> want;
    if (KeepTaskbarShown() && g_settings.autoHide == AutoHide::Off) {
        ForEachTaskbarWindow([&want](HWND tb, bool) {
            HMONITOR monitor = MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST);
            MONITORINFO info{sizeof(info)};
            UINT dpiX, dpiY;
            if (!GetMonitorInfo(monitor, &info) ||
                FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
                return;
            }
            RECT rect = info.rcMonitor;
            rect.top = rect.bottom - StripHeight(dpiY);
            want.push_back(rect);
        });
    }
    bool same = want.size() == g_appBars.size();
    for (size_t i = 0; same && i < want.size(); i++) same = EqualRect(&want[i], &g_appBars[i].rect);
    if (same) return;
    RemoveAppBars();
    if (want.empty() || !RegisterWindowClass()) return;
    for (auto& rect : want) AddAppBar(rect);
}

// Run once from the message loop for any number of requests.
// 1 = update, 2 = and auto-hide.
int g_overWindowsPending = 0;

void ScheduleOverWindows(bool autoHide) {
    bool post = g_overWindowsPending == 0;
    g_overWindowsPending = std::max(g_overWindowsPending, autoHide ? 2 : 1);
    if (!post) return;
    HWND w = EnsureModWindow();
    if (!w || !PostMessage(w, kUpdateOverWindowsMessage, 0, 0)) g_overWindowsPending = 0;
}

void UpdateOverWindows() {
    int pending = std::exchange(g_overWindowsPending, 0);
    if (g_unloading || !pending) return;
    if (pending == 2) ApplyAutoHide();
    UpdateAppBars();
    ClipTaskbars();
}

// Also done whenever a taskbar appears, as the mod often loads first.
void ApplyMagnifyOverWindows() {
    if (HWND tb = FindTaskbar()) {
        RunFromWindowThread(tb, [](void*) { ScheduleOverWindows(true); }, nullptr);
    }
}

LRESULT CALLBACK ModWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case kAppBarMessage:
            if (wParam == ABN_POSCHANGED) ScheduleOverWindows(false);
            return 0;
        case WM_DISPLAYCHANGE:
            ScheduleOverWindows(false);
            ScheduleMaximizedCheck();
            break;
        case kUpdateOverWindowsMessage:
            UpdateOverWindows();
            return 0;
        case kTrashChangedMessage:
            CheckRecycleBinSoon();
            return 0;
        case kTrashCheckedMessage:
            if (!g_unloading) Try(UpdateTrashIcons);
            return 0;
        case kMoveExplorerMessage:
            if (!g_unloading) MoveExplorerNow();
            return 0;
        case WM_TIMER:
            if (wParam == kPollTimer) {
                Try(OnPollTimer);
                return 0;
            }
            if (wParam == kTrayClipTimer) {
                KillTimer(hWnd, kTrayClipTimer);
                ScheduleOverWindows(false);
                return 0;
            }
            if (wParam == kIconsMovingTimer) {
                KillTimer(hWnd, kIconsMovingTimer);
                ScheduleOverWindows(false);  // clipped again
                return 0;
            }
            if (wParam == kMaximizedCheckTimer) {
                KillTimer(hWnd, kMaximizedCheckTimer);
                CheckWindowMaximized();
                return 0;
            }
            if (wParam == kSquircleRetryTimer) {
                KillTimer(hWnd, kSquircleRetryTimer);
                if (!g_unloading) RetrySquircles();
                return 0;
            }
            if (wParam == kReapplyTimer) {
                // 4, 8 and 12 seconds after the taskbar was found.
                if (++g_reapplyCount >= 3) KillTimer(hWnd, kReapplyTimer);
                if (!g_unloading) ScheduleOverWindows(true);
                return 0;
            }
            break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// =============================================================================
// HOOK INSTALLATION
// =============================================================================

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
         &TrayUI__StuckTrayChange_Original},
        {{LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
         &TrayUI__HandleSettingChange_Original, TrayUI__HandleSettingChange_Hook},
        // Maximized windows
        {{LR"(public: void __cdecl TrayUI::_Hide(void))"},
         &TrayUI__Hide_Original, TrayUI__Hide_Hook, true},
        {{LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
         &CSecondaryTray__AutoHide_Original, CSecondaryTray__AutoHide_Hook, true},
        // Jump list placement
        {{LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
         &CTaskListWnd_ComputeJumpViewPosition_Original, CTaskListWnd_ComputeJumpViewPosition_Hook, true},
        {{LR"(struct tagRECT __cdecl ToRawPixelsInDesktopCoordinates(struct winrt::Windows::Foundation::Rect const &,unsigned int,struct HWND__ *))"},
         &ToRawPixelsInDesktopCoordinates_Original, ToRawPixelsInDesktopCoordinates_Hook, true},
        // Window previews, older builds (gone in 10.0.26100.8313 and later)
        {{LR"(protected: long __cdecl CTaskListWnd::_DisplayExtendedUI(struct ITaskBtnGroup *,int,unsigned long,int))"},
         &CTaskListWnd__DisplayExtendedUI_Original, CTaskListWnd__DisplayExtendedUI_Hook, true},
        {{LR"(protected: void __cdecl CTaskListWnd::_ShowToolTip(enum ShowToolTipFlags))"},
         &CTaskListWnd__ShowToolTip_Original, CTaskListWnd__ShowToolTip_Hook, true},
        {{LR"(public: virtual int __cdecl CTaskListThumbnailWnd::DisplayUI(struct ITaskBtnGroup *,struct ITaskItem *,struct ITaskItem *,unsigned long))"},
         &CTaskListThumbnailWnd_DisplayUI_Original, CTaskListThumbnailWnd_DisplayUI_Hook, true},
        {{LR"(public: virtual void __cdecl CTaskListThumbnailWnd::LayoutThumbnails(void))"},
         &CTaskListThumbnailWnd_LayoutThumbnails_Original, CTaskListThumbnailWnd_LayoutThumbnails_Hook, true},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// The name the symbol extraction script expects for a list of hooks that
// isn't one of the hook arrays itself.
using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;

// Newer builds keep the tray in SystemTray.dll, older ones in Taskbar.View.dll.
std::vector<COMBINED_SH> GetTrayHooks() {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK trayHooks[] = {
        {{LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
         &SystemTrayController_GetFrameSize_Original, SystemTray_GetFrameSize_Hook<SystemTrayController_GetFrameSize_Original>, true},
        {{LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
         &SystemTraySecondaryController_GetFrameSize_Original, SystemTray_GetFrameSize_Hook<SystemTraySecondaryController_GetFrameSize_Original>, true},
        {{LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
         &SystemTrayController_UpdateFrameSize_Address, nullptr, true},
        {{LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
         &SystemTraySecondaryController_UpdateFrameSize_Original, SystemTray_UpdateFrameSize_Hook<SystemTraySecondaryController_UpdateFrameSize_Original>, true},
        {{LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
         &SystemTrayFrame_Height_Original, SystemTrayFrame_Height_Hook, true},
    };
    return {std::make_move_iterator(std::begin(trayHooks)),
            std::make_move_iterator(std::end(trayHooks))};
}

bool HookTaskbarViewSymbols(HMODULE module, bool withTray) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {{LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
         &TaskbarConfiguration_GetFrameSize_Original, TaskbarConfiguration_GetFrameSize_Hook, true},
#ifdef _M_ARM64
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::UpdateFrameSize(void))"},
         &TaskbarConfiguration_UpdateFrameSize_Address, nullptr, true},
        {{LR"(public: void __cdecl winrt::event<struct winrt::delegate<> >::operator()<>(void))"},
         &Event_operator_call_Original, Event_operator_call_Hook, true},
#endif
        {{LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )"},
         &TaskbarFrame_MaxHeight_Original, nullptr, true},
        {{LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )"},
         &TaskbarFrame_Height_Original, TaskbarFrame_Height_Hook, true},  // gone in 24H2
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
         &TaskbarController_OnGroupingModeChanged_Address, nullptr, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
         &TaskbarController_UpdateFrameHeight_Original, TaskbarController_UpdateFrameHeight_Hook, true},
        // Pointer
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
         &TaskbarFrame_OnPointerMoved_Original, TaskbarFrame_OnPointerMoved_Hook, true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
         &TaskbarFrame_OnPointerExited_Original, TaskbarFrame_OnPointerExited_Hook, true},
        // Window previews
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TransitionToFlyoutVisibleStickyState(struct winrt::hstring))"},
         &HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Original, HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Hook, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::ShowTaskListButtonHoverFlyout(class std::vector<struct winrt::weak_ref<struct winrt::Windows::UI::Xaml::FrameworkElement>,class std::allocator<struct winrt::weak_ref<struct winrt::Windows::UI::Xaml::FrameworkElement> > >,struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::Foundation::IInspectable> const &,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind,enum winrt::WindowsUdk::UI::Shell::TaskbarFlyoutKind))"},
         &HoverFlyoutController_ShowTaskListButtonHoverFlyout_Original, HoverFlyoutController_ShowTaskListButtonHoverFlyout_Hook, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::ShowTaskListButtonHoverFlyout(class std::vector<struct winrt::weak_ref<struct winrt::Windows::UI::Xaml::FrameworkElement>,class std::allocator<struct winrt::weak_ref<struct winrt::Windows::UI::Xaml::FrameworkElement> > >,struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::Foundation::IInspectable> const &,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
         &HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old_Original, HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old_Hook, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::FlyoutFrame::UpdateFlyoutPosition(void))"},
         &FlyoutFrame_UpdateFlyoutPosition_Original, FlyoutFrame_UpdateFlyoutPosition_Hook, true},
        // Jump list placement
        {{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnTaskListButtonContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
         &TaskbarResources_OnTaskListButtonContextRequested_Original, TaskbarResources_OnTaskListButtonContextRequested_Hook, true},
        {{LR"(public: static void __cdecl winrt::Taskbar::implementation::TaskListButtonHandlers::HandleContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
         &TaskListButtonHandlers_HandleContextRequested_Original, TaskListButtonHandlers_HandleContextRequested_Hook, true},
        // Dragging
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragStartedGesture(struct winrt::Windows::Foundation::Point))"},
         &TaskListButton_OnDragStartedGesture_Original, TaskListButton_OnDragStartedGesture_Hook, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateDrag(struct winrt::Windows::Foundation::Point))"},
         &TaskListButton_UpdateDrag_Original, TaskListButton_UpdateDrag_Hook, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragCompletedGesture(void))"},
         &TaskListButton_OnDragCompletedGesture_Original, TaskListButton_OnDragCompletedGesture_Hook, true},
        {{LR"(public: struct winrt::Taskbar::implementation::DropPlaceholder __cdecl winrt::Taskbar::implementation::TaskListDragOperation::GetDropPlaceholder(class std::vector<struct winrt::Windows::Foundation::Rect,class std::allocator<struct winrt::Windows::Foundation::Rect> > const &,struct std::pair<unsigned int,unsigned int>,struct winrt::Windows::Foundation::Rect,enum winrt::Taskbar::implementation::RearrangeDirection,struct winrt::Taskbar::implementation::GroupBoundsCalculator const *,bool))",
          // Older builds lack the last parameter; passing it anyway is harmless.
          LR"(public: struct winrt::Taskbar::implementation::DropPlaceholder __cdecl winrt::Taskbar::implementation::TaskListDragOperation::GetDropPlaceholder(class std::vector<struct winrt::Windows::Foundation::Rect,class std::allocator<struct winrt::Windows::Foundation::Rect> > const &,struct std::pair<unsigned int,unsigned int>,struct winrt::Windows::Foundation::Rect,enum winrt::Taskbar::implementation::RearrangeDirection,struct winrt::Taskbar::implementation::GroupBoundsCalculator const *))"},
         &TaskListDragOperation_GetDropPlaceholder_Original, TaskListDragOperation_GetDropPlaceholder_Hook, true},
        // Icon resolution
        {{LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"},
         &TaskbarConfiguration_GetIconHeightInViewPixels_Original, TaskbarConfiguration_GetIconHeightInViewPixels_Hook, true},
        {{LR"(public: struct winrt::Taskbar::implementation::TaskbarFrameMetrics __cdecl winrt::Taskbar::implementation::TaskbarFrame::GetMetrics(void)const )"},
         &TaskbarFrame_GetMetrics_Original, TaskbarFrame_GetMetrics_Hook, true},
        {{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::IconHeight(double))"},
         &TaskListButton_IconHeight_Address, nullptr, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
         &TaskListButton_UpdateButtonPadding_Original, ButtonUpdate_Hook<TaskListButton_UpdateButtonPadding_Original>, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
         &TaskListButton_UpdateVisualStates_Original, ButtonUpdate_Hook<TaskListButton_UpdateVisualStates_Original>, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"},
         &TaskListButton_UpdateIconColumnDefinition_Original, TaskListButton_UpdateIconColumnDefinition_Hook, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateMultiWindowClip(void))"},
         &TaskListButton_UpdateMultiWindowClip_Original, StockIconHeight_Hook<TaskListButton_UpdateMultiWindowClip_Original>, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::CreateMultiWindowClip(void))"},
         &TaskListButton_CreateMultiWindowClip_Original, StockIconHeight_Hook<TaskListButton_CreateMultiWindowClip_Original>, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadge(void))"},
         &TaskListButton_UpdateBadge_Original, StockIconHeight_Hook<TaskListButton_UpdateBadge_Original>, true},
        {{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
         &TaskListButton_OverlayIcon_Original, TaskListButton_OverlayIcon_Hook, true},
        {{LR"(public: double __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(void)const )"},
         &ExperienceToggleButton_IconHeight_get, nullptr, true},
        {{LR"(public: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(double))"},
         &ExperienceToggleButton_IconHeight_Original, DockIconHeight_Hook<ExperienceToggleButton_IconHeight_Original>, true},
        {{LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
         &ExperienceToggleButton_UpdateButtonPadding_Original, ExperienceToggleButton_UpdateButtonPadding_Hook, true},
        {{LR"(public: void __cdecl winrt::Microsoft::Windows::Taskbar::implementation::TaskbarComponentHost::IconHeight(double))"},
         &TaskbarComponentHost_IconHeight_Original, DockIconHeight_Hook<TaskbarComponentHost_IconHeight_Original>, true},
    };
    // On builds with the tray here too, its symbols are looked up in the same call.
    std::vector<COMBINED_SH> allHooks(std::make_move_iterator(std::begin(taskbarViewHooks)),
                                      std::make_move_iterator(std::end(taskbarViewHooks)));
    if (withTray) {
        for (auto& hook : GetTrayHooks()) allHooks.push_back(std::move(hook));
    }
    if (!WindhawkUtils::HookSymbols(module, allHooks.data(), allHooks.size())) return false;
    if (TaskListButton_IconHeight_Address) GetIconHeightOffset();  // found up front
    if (withTray) HookTrayUpdateFrameSize();
#ifdef _M_ARM64
    HookFrameSizeUpdate();
#endif
    return true;
}

bool HookTraySymbols(HMODULE module) {
    auto systemTrayHooks = GetTrayHooks();
    if (!WindhawkUtils::HookSymbols(module, systemTrayHooks.data(), systemTrayHooks.size())) {
        return false;
    }
    HookTrayUpdateFrameSize();
    return true;
}

// Taskbar.View.dll before version 2604 contains the system tray too.
bool TaskbarViewHasTray(HMODULE module) {
    HRSRC res = FindResource(module, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    HGLOBAL global = res ? LoadResource(module, res) : nullptr;
    void* data = global ? LockResource(global) : nullptr;
    VS_FIXEDFILEINFO* info = nullptr;
    UINT size = 0;
    if (!data || !VerQueryValue(data, L"\\", (void**)&info, &size) || !size) return false;
    return HIWORD(info->dwFileVersionMS) < 2604;
}

std::atomic<bool> g_taskbarViewHooked = false;
std::atomic<bool> g_trayHooked = false;

// Hooks whichever taskbar DLLs are loaded and not hooked yet. Returns whether
// anything new was hooked.
bool HookTaskbarModules() {
    bool hooked = false;
    if (HMODULE view = GetModuleHandle(L"Taskbar.View.dll");
        view && !g_taskbarViewHooked.exchange(true)) {
        bool withTray = !GetModuleHandle(L"SystemTray.dll") && TaskbarViewHasTray(view) &&
                        !g_trayHooked.exchange(true);
        HookTaskbarViewSymbols(view, withTray);
        hooked = true;
    }
    if (HMODULE tray = GetModuleHandle(L"SystemTray.dll"); tray && !g_trayHooked.exchange(true)) {
        HookTraySymbols(tray);
        hooked = true;
    }
    return hooked;
}

// The taskbar's DLLs load late when Explorer starts after the mod.
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module && (!g_taskbarViewHooked || !g_trayHooked) && HookTaskbarModules()) {
        Wh_ApplyHookOperations();
    }
    return module;
}

// =============================================================================
// WINDHAWK ENTRY POINTS
// =============================================================================

BOOL Wh_ModInit() {
    LoadSettings();
    g_managingAutoHide = Wh_GetIntValue(UserValueName(L"managingAutoHide").c_str(), 0);
    g_autoHideBefore = Wh_GetIntValue(UserValueName(L"autoHideBefore").c_str(), 0);
    g_shellJobWork = CreateThreadpoolWork(RunShellJob, nullptr, nullptr);

    if (!HookTaskbarDllSymbols()) Wh_Log(L"taskbar.dll hooks failed; height won't change");
    HookTaskbarModules();

    if (HMODULE kb = GetModuleHandle(L"kernelbase.dll")) {
        if (auto p = GetProcAddress(kb, "LoadLibraryExW")) {
            WindhawkUtils::SetFunctionHook((LoadLibraryExW_t)p, LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
        }
    }
    // Window previews.
    if (HMODULE win32u = GetModuleHandle(L"win32u.dll")) {
        if (auto p = GetProcAddress(win32u, "NtUserShowWindow")) {
            WindhawkUtils::SetFunctionHook((ShowWindow_t)p, ShowWindow_Hook, &ShowWindow_Original);
        }
        if (auto p = GetProcAddress(win32u, "NtUserEnableWindow")) {
            WindhawkUtils::SetFunctionHook((EnableWindow_t)p, EnableWindow_Hook,
                                           &EnableWindow_Original);
        }
    }
    WindhawkUtils::SetFunctionHook(MapWindowPoints, MapWindowPoints_Hook,
                                   &MapWindowPoints_Original);
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original);
    g_dockShown = g_settings.autoHide == AutoHide::On ? 0 : 1;  // starts hidden
    return TRUE;
}

// Applying the height also makes the taskbar update its buttons, which runs
// the hooks that find them.
void Wh_ModAfterInit() {
    if (g_taskbarHeight > 0) ApplyTaskbarHeight();
    ApplyMagnifyOverWindows();
}

// Runs before the hooks are removed, which the taskbar needs to go back to its
// normal height and icon size.
void Wh_ModBeforeUninit() {
    g_unloading = true;

    // The elements first, so the taskbar sizes its icons itself below. Then any
    // cancelled async work's callbacks, which the taskbar's thread runs meanwhile.
    if (HWND tb = FindTaskbar()) RunFromWindowThread(tb, [](void*) { ResetAll(); }, nullptr);
    for (int i = 0; i < 100 && g_pendingCallbacks > 0; i++) Sleep(20);
    ApplyAutoHide();
    UnregisterClass(kWindowClass, ModuleInstance());

    // Shell jobs first: they end by checking the Recycle Bin.
    for (PTP_WORK* work : {&g_shellJobWork, &g_trashWork}) {
        if (!*work) continue;
        WaitForThreadpoolWorkCallbacks(*work, FALSE);
        CloseThreadpoolWork(*work);
        *work = nullptr;
    }

    // Waits for the tray to take the stock height while the hooks are still in,
    // as it may do so after this message is handled.
    if (g_taskbarHeight > 0) {
        g_taskbarHeight = 0;
        g_size.iconLoadSize = 0;
        g_trayResized = false;
        ApplyTaskbarHeight();
        for (int i = 0; i < 40 && !g_trayResized; i++) Sleep(50);
        Wh_Log(L"Tray %s its height back", g_trayResized ? L"set" : L"didn't set");
    }
}

// Settings are read on the taskbar's thread, where the frame loop and hooks
// read them. Plates are rebuilt on the next frame.
void Wh_ModSettingsChanged() {
    int oldTaskbarHeight = g_taskbarHeight;
    float oldIconLoadSize = g_size.iconLoadSize;

    HWND tb = FindTaskbar();
    bool ran = tb && RunFromWindowThread(tb, [](void*) {
        Settings old = g_settings;
        LoadSettings();
        RecolorSquircles(old);
        Try(HideLabel);  // redrawn in the new color
        Try(RemovePlates);
        Try(StopTrash);
        Try(UpdatePolling);
        g_finderIconSource.reset();  // reloaded at the new size and image
        SettleIconsAgain();
        for (auto& b : *g_buttons) {
            if (auto el = b.element.get()) Try([&] { ProcessButton(el); });
        }
        g_row.clear();
        g_rowParent = nullptr;
        MarkPlatesDirty();
    }, nullptr);
    if (!ran) LoadSettings();

    // A new height or icon resolution: the taskbar resizes and reloads icons.
    if (g_taskbarHeight != oldTaskbarHeight || g_size.iconLoadSize != oldIconLoadSize) {
        ApplyTaskbarHeight();
    }
    ApplyMagnifyOverWindows();
}
