// ==WindhawkMod==
// @id              macos-style-dock
// @name            MacOS Style Dock
// @description     Turns the Windows 11 taskbar into a macOS-style dock with magnifying icons
// @version         1.1
// @author          Zach Vaughn
// @github          https://github.com/zachvdev
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -lgdi32 -lshell32 -lshcore -ladvapi32 -ldwmapi -lgdiplus -ld2d1
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MacOS Style Dock

Turns the Windows 11 taskbar into a macOS-style dock: frosted glass, icons
that magnify under the pointer, running dots, app name labels, Finder and
Trash, and hot corners for Task View, Start and the system tray.

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
  light on a dark one.

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

## Finder and Trash

- **Finder.** File Explorer is moved to the front of your apps for real, so
  it stays there, and can't be dragged out of that spot.
- **Trash.** The Recycle Bin sits at the far end, past a divider. Its icon
  shows when there's something in it.
  - Click it to open it.
  - Drop files on it to delete them. Windows asks first if they're too big
    to recycle.
  - Right-click it to open or empty it.

Each has a choice of icons:

- **File Explorer:** macOS's Finder (the default), Windows 11's own, or your
  own image.
- **Recycle Bin:** macOS's Trash (the default), Windows 11's own, or your own
  images.

With a macOS icon they're named Finder and Trash; otherwise they keep
Windows' names.

## Custom icons

Choose Custom for either icon, then paste your image under "Custom icons" as
Base64 text. You may ask an AI model to resize an image and encode it for
you: 256x256 for File Explorer, and 240x256 for the Recycle Bin, which takes
one image for empty and one for full (or the empty one for both).

- **Margin:** the empty space around the artwork, in pixels of a 256-pixel
  image. It spills past the icon's spot, like the shadow in Apple's icons,
  which have 25.
- **Size:** the artwork's size next to other icons, as a percentage.

PNG with transparency works best, but any format Windows can open works. A
"data:image/png;base64," prefix is fine. Left blank, the default icon shows;
an image that can't be read shows Windows' own icon instead.

## Using the dock

- **App names on hover**, in macOS-style labels, instead of window previews.
- **Rearranging.** Drag apps anywhere between Finder and the Trash. While you
  drag, icons stop magnifying, and the one you're moving fades slightly.
- **Right-click menus** (an app's recent files, pinned items and "Close
  window") open right above the icon you right-clicked.
- **Maximized windows** reach down to the top of the dock, with magnified
  icons rising over them.

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

- **Hybrid** (the default): the dock hides while a window is maximized. The
  window gets the whole screen right away, and the dock slides out of sight
  a moment later.
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
- **Dock items:** the File Explorer and Recycle Bin icons, squircle icons and
  their tile color, the Recycle Bin, app names, a divider after Finder, and
  high resolution icons.
- **Custom icons:** your own images, with their margin and size.
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

- Windows 11 on x64, with the taskbar at the bottom. "Small taskbar buttons"
  isn't supported.
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
keeping the taskbar shown follows his "Taskbar auto-hide when maximized", and
removing hover previews follows his "Disable Taskbar Thumbnails" (all
GPL-3.0). The squircle corners use PaintCode's measurements of Apple's curve.
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
      Hybrid: hidden only while a window is maximized. Default is Hybrid.
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
  - finderIcon: mac
    $name: File Explorer icon
    $description: The macOS icon also names it Finder. Default is macOS.
    $options:
    - windows: Windows 11
    - mac: macOS
    - custom: Custom
  - trashIcon: mac
    $name: Recycle Bin icon
    $description: The macOS icon also names it Trash. Default is macOS.
    $options:
    - windows: Windows 11
    - mac: macOS
    - custom: Custom
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
  - recycleBin: true
    $name: Recycle Bin
    $description: At the right end of the dock. Default is on.
  - tooltips: true
    $name: Show tooltips
    $description: App names above icons on hover. Default is on.
  - appsDivider: false
    $name: Divider after File Explorer
    $description: Default is off.
  - highResIcons: true
    $name: High resolution icons
    $description: Keeps icons sharp when magnified. Turn off if apps' custom icons don't show up. Default is on.
  $name: Dock items
- customIcons:
  - finderImage: ""
    $name: Custom File Explorer image (Base64)
    $description: Used when File Explorer icon is Custom.
  - finderMargin: 25
    $name: Custom File Explorer icon margin
    $description: >-
      Empty space around the artwork, in pixels of a 256-pixel image. It spills
      past the icon's spot, like the shadow in Apple's icons. Default is 25.
  - finderSize: 100
    $name: Custom File Explorer icon size
    $description: The artwork's size, as a percentage of other icons. Default is 100.
  - trashEmptyImage: ""
    $name: Custom empty Recycle Bin image (Base64)
    $description: Used when Recycle Bin icon is Custom.
  - trashFullImage: ""
    $name: Custom full Recycle Bin image (Base64)
    $description: Leave blank to use the empty image.
  - trashMargin: 25
    $name: Custom Recycle Bin icon margin
    $description: >-
      Empty space around the artwork, in pixels of a 256-pixel image. It spills
      past the icon's spot, like the shadow in Apple's icons. Default is 25.
  - trashSize: 100
    $name: Custom Recycle Bin icon size
    $description: The artwork's size, as a percentage of other icons. Default is 100.
  $name: Custom icons
  $description: >-
    Your own images for the Custom icon choices. You may ask an AI model to
    resize an image to 256x256 for File Explorer or 240x256 for the Recycle Bin
    and encode it for you in Base64.
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
enum class IconStyle { Windows, Mac, Custom };  // File Explorer's and the Recycle Bin's

// The user's own Finder or Trash icon.
struct CustomIcon {
    std::string image, fullImage;  // Base64; fullImage for a full Recycle Bin
    float size = 1;                // the artwork, of the icon size
    float scale = 1;               // the image, of the icon size, margin included
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
    IconStyle finderIcon, trashIcon;
    CustomIcon customFinder, customTrash;
    bool squircleIcons;
    TileColor tileColor;
    uint8_t tileCustomColor[3];
    bool recycleBin;
    bool tooltips;
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

// Base64 text from a string setting, without a data URI's "data:...;base64,".
std::string Base64Setting(PCWSTR name) {
    auto value = WindhawkUtils::StringSetting::make(name);
    std::wstring_view text = value ? (PCWSTR)value : L"";
    if (size_t comma = text.rfind(L','); comma != text.npos) text.remove_prefix(comma + 1);
    std::string base64;
    for (wchar_t c : text) {
        if (c < 128 && !iswspace(c)) base64.push_back((char)c);
    }
    return base64;
}

// A custom icon, drawn so its artwork is `size` percent of other icons, with
// `margin` pixels of a 256-pixel image around it.
CustomIcon CustomIconSetting(PCWSTR image, PCWSTR fullImage, PCWSTR margin, PCWSTR size) {
    CustomIcon icon;
    icon.image = Base64Setting(image);
    if (fullImage) icon.fullImage = Base64Setting(fullImage);
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

    s.finderIcon =
        (IconStyle)ChoiceSetting(L"items.finderIcon", {L"windows", L"mac", L"custom"}, 1);
    s.customFinder = CustomIconSetting(L"customIcons.finderImage", nullptr,
                                       L"customIcons.finderMargin", L"customIcons.finderSize");
    if (s.finderIcon == IconStyle::Custom && s.customFinder.image.empty()) {
        s.finderIcon = IconStyle::Mac;
    }
    s.trashIcon =
        (IconStyle)ChoiceSetting(L"items.trashIcon", {L"windows", L"mac", L"custom"}, 1);
    s.customTrash = CustomIconSetting(L"customIcons.trashEmptyImage", L"customIcons.trashFullImage",
                                      L"customIcons.trashMargin", L"customIcons.trashSize");
    if (s.trashIcon == IconStyle::Custom && s.customTrash.image.empty()) {
        s.trashIcon = IconStyle::Mac;
    }
    s.squircleIcons = Wh_GetIntSetting(L"items.squircleIcons");
    s.tileColor = (TileColor)ChoiceSetting(L"items.squircleColor",
                                           {L"auto", L"light", L"dark", L"custom"}, 0);
    if (s.tileColor == TileColor::Custom && !hex(L"items.squircleCustomColor", s.tileCustomColor)) {
        s.tileColor = TileColor::Auto;
    }
    s.recycleBin = Wh_GetIntSetting(L"items.recycleBin");
    s.tooltips = Wh_GetIntSetting(L"items.tooltips");
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
constexpr float kMacIconScale = 1024.0f / 824;  // Apple's icons fill 824 of 1024px, for shadows
constexpr float kTrashShift = 3;                // the Recycle Bin, moved left, at Size 60
constexpr float kTrashMenuLift = 6;             // its menu's gap above the icon, at Size 60
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

    // Tall enough for the dock and a fully magnified icon, plus 2px.
    float dockTop = z.bottomGap + z.dockHeight;
    float iconTop = z.bottomGap + z.padY + s.scale * icon;
    g_taskbarHeight = std::max((int)std::ceil(std::max(dockTop, iconTop) + 2), kMinTaskbarHeight);

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
constexpr UINT_PTR kReapplyTimer = 1;
constexpr UINT_PTR kPollTimer = 3;
constexpr UINT_PTR kTrayClipTimer = 4;

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

HWND EnsureModWindow() {
    if (!g_modWindow && RegisterWindowClass()) {
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

extern const char kFinderIconPng[];
extern const char kTrashEmptyIconPng[];
extern const char kTrashFullIconPng[];

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

std::vector<BYTE> DecodeBase64(std::string_view text) {
    std::vector<BYTE> out;
    uint32_t bits = 0;
    int count = 0;
    for (char c : text) {
        int v = c >= 'A' && c <= 'Z'   ? c - 'A'
                : c >= 'a' && c <= 'z' ? c - 'a' + 26
                : c >= '0' && c <= '9' ? c - '0' + 52
                : c == '+'             ? 62
                : c == '/'             ? 63
                                       : -1;
        if (v < 0) continue;
        bits = (bits << 6) | v;
        count += 6;
        if (count >= 8) {
            count -= 8;
            out.push_back((BYTE)(bits >> count));
        }
    }
    return out;
}

// A Base64 image (PNG, or any format Windows reads), scaled to `px` on its
// longer side, decoded synchronously (XAML's own decoding is asynchronous).
wuxm::ImageSource LoadBase64Image(std::string_view base64, int px) {
    std::vector<BYTE> png = DecodeBase64(base64);
    if (png.empty() || px <= 0) return nullptr;
    wuxm::ImageSource result{nullptr};
    Try([&] {
        using winrt::check_hresult;
        winrt::com_ptr<IWICImagingFactory> factory;
        winrt::com_ptr<IWICStream> stream;
        winrt::com_ptr<IWICBitmapDecoder> decoder;
        winrt::com_ptr<IWICBitmapFrameDecode> frame;
        winrt::com_ptr<IWICFormatConverter> converter;
        winrt::com_ptr<IWICBitmapScaler> scaler;
        check_hresult(CoCreateInstance(kWICImagingFactoryId, nullptr, CLSCTX_INPROC_SERVER,
                                       kIWICImagingFactoryId, factory.put_void()));
        check_hresult(factory->CreateStream(stream.put()));
        check_hresult(stream->InitializeFromMemory(png.data(), (DWORD)png.size()));
        check_hresult(factory->CreateDecoderFromStream(
            stream.get(), nullptr, WICDecodeMetadataCacheOnDemand, decoder.put()));
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
// macOS's icons, or the user's, drawn bigger than the icon's spot so their
// artwork fills it and their margin (a shadow) spills past. macOS's icons also
// take the names Finder and Trash.

float FinderIconScale() {
    auto& s = g_settings;
    return s.finderIcon == IconStyle::Custom ? s.customFinder.scale : kMacIconScale;
}

float TrashIconScale() {
    switch (g_settings.trashIcon) {
        case IconStyle::Windows: return kTrashIconScale;
        case IconStyle::Custom: return g_settings.customTrash.scale;
        case IconStyle::Mac: break;
    }
    return kMacIconScale;
}

// The Recycle Bin's artwork height, for its label. Windows' bin is drawn bigger
// than its artwork, so its label goes halfway up the difference.
float TrashArtHeight() {
    switch (g_settings.trashIcon) {
        case IconStyle::Windows: return g_size.icon * (1 + kTrashIconScale) / 2;
        case IconStyle::Custom: return g_size.icon * g_settings.customTrash.size;
        case IconStyle::Mac: break;
    }
    return g_size.icon;
}

// None for Windows' own icon.
std::string_view FinderImage() {
    switch (g_settings.finderIcon) {
        case IconStyle::Windows: return {};
        case IconStyle::Custom: return g_settings.customFinder.image;
        case IconStyle::Mac: break;
    }
    return kFinderIconPng;
}

// None for Windows' own icon. A custom one without a full image uses the empty.
std::string_view TrashImage(bool full) {
    auto& custom = g_settings.customTrash;
    switch (g_settings.trashIcon) {
        case IconStyle::Windows: return {};
        case IconStyle::Custom:
            return full && !custom.fullImage.empty() ? custom.fullImage : custom.image;
        case IconStyle::Mac: break;
    }
    return full ? kTrashFullIconPng : kTrashEmptyIconPng;
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
    float originX = 0;                  // the row's left edge in the panel
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

// Adds `el` right after the icon row, so it's drawn above it.
bool InsertAfterRow(wuxc::Panel const& panel, wux::UIElement const& row,
                    wux::UIElement const& el) {
    uint32_t index = 0;
    if (!panel.Children().IndexOf(row, index)) return false;
    panel.Children().InsertAt(index + 1, el);
    return true;
}

wuxc::Border AddDivider(wuxc::Panel const& panel, wux::UIElement const& row) {
    wuxc::Border divider;
    PlaceInPanel(panel, divider);
    divider.IsHitTestVisible(false);
    divider.UseLayoutRounding(false);  // allows a fraction of a pixel
    divider.Width(kDividerWidth);
    divider.Height(std::max(1.0f, g_size.dockHeight - 2 * DividerInset()));
    return InsertAfterRow(panel, row, divider) ? divider : nullptr;
}

// =============================================================================
// APP ICONS
// =============================================================================
// File Explorer can show a Finder icon, or the user's own, and every app a
// squircle tile, as on a Mac. The taskbar reuses buttons and sets new icons, so
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
    bool rendering = false;
    int retries = 0;
};

[[clang::no_destroy]] std::list<IconSwap> g_iconSwaps;
[[clang::no_destroy]] std::optional<wuxm::ImageSource> g_finderIconSource;  // none if unreadable

IconLook LookFor(wux::FrameworkElement const& button) {
    if (g_settings.finderIcon != IconStyle::Windows && IsFileExplorer(button)) {
        return IconLook::Finder;
    }
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
        g_finderIconSource = LoadBase64Image(FinderImage(), MagnifiedPixels(el, FinderIconScale()));
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
    float k = s->look == IconLook::Finder ? FinderIconScale() : kMacIconScale;
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
// the squircle. Otherwise it's a shape, shrunk onto a tile of the Squircle
// color: for Auto, light, or dark if its outline is light.

constexpr int kAlphaShown = 64;              // counts as part of the icon
constexpr int kAlphaSolid = 224;             // counts as solid
constexpr float kTileMinCoverage = 0.84f;    // solid share of its bounds, to be a tile
constexpr float kTileMinAspect = 0.9f;
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
constexpr int kSquircleRetries = 30;         // while the icon isn't drawn yet
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
    int x0 = w, y0 = h, x1 = -1, y1 = -1;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (source[((size_t)y * w + x) * 4 + 3] < kAlphaShown) continue;
            x0 = std::min(x0, x), x1 = std::max(x1, x);
            y0 = std::min(y0, y), y1 = std::max(y1, y);
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
    bool tile = aspect >= kTileMinAspect && solid >= kTileMinCoverage * bw * bh;
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

std::shared_ptr<void> PinModule();

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
    g_iconsSettleTimer.Tick([pin = PinModule()](auto&&, auto&&) {
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

// Async work's callbacks run code from the mod, so it stays loaded until they're
// done, even if it's turned off meanwhile. Let go of a moment after the last
// one, from a thread pool thread, so that callback has returned by then.
VOID CALLBACK UnpinModule(PTP_CALLBACK_INSTANCE instance, PVOID module) {
    Sleep(1000);
    FreeLibraryWhenCallbackReturns(instance, (HMODULE)module);
}

std::shared_ptr<void> PinModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)&UnpinModule, &module);
    return std::shared_ptr<void>(module, [](void* module) {
        if (module) TrySubmitThreadpoolCallback(UnpinModule, module, nullptr);
    });
}

void OnStockPixels(winrt::weak_ref<wuxc::Image> const& weak, wuxm::ImageSource const& stock,
                   std::vector<BYTE> pixels, int w, int h) {
    auto image = weak.get();
    IconSwap* s = image ? FindSwap(image) : nullptr;
    if (!s) return;
    s->rendering = false;
    if (s->stock != stock || s->look != IconLook::Squircle) return UpdateSwap(*s, image);

    IconPixels tile;
    if (!pixels.empty() && w > 0 && h > 0) tile = MakeSquircleIcon(std::move(pixels), w, h);
    if (!tile.size) {
        // Not drawn yet: tried again shortly.
        if (++s->retries > kSquircleRetries) return;
        auto queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (!queue) return;
        queue.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low,
                         [weak, pin = PinModule()] {
                             if (g_unloading) return;
                             auto image = weak.get();
                             IconSwap* s = image ? FindSwap(image) : nullptr;
                             if (s) Try([&] { UpdateSwap(*s, image); }, L"Squircle icon");
                         });
        return;
    }
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
    if (image.Source() != s.stock || image.ActualWidth() <= 0) return;
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
    auto pin = PinModule();
    target.RenderAsync(image, px, px)
        .Completed([=, pin = pin](wf::IAsyncAction const&, wf::AsyncStatus status) {
            queue.TryEnqueue([=, pin = pin] {
                if (g_unloading) return;
                Try([&] {
                    if (status != wf::AsyncStatus::Completed) {
                        return OnStockPixels(weak, stock, {}, 0, 0);
                    }
                    target.GetPixelsAsync().Completed(
                        [=, pin = pin](wf::IAsyncOperation<wss::IBuffer> const& op,
                                       wf::AsyncStatus status) {
                            queue.TryEnqueue([=, pin = pin] {
                                if (g_unloading) return;
                                Try([&] {
                                    std::vector<BYTE> pixels;
                                    if (status == wf::AsyncStatus::Completed) {
                                        pixels = BufferBytes(op.GetResults());
                                    }
                                    OnStockPixels(weak, stock, std::move(pixels),
                                                  target.PixelWidth(), target.PixelHeight());
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
// icons" is set to. Windows' own, or if a custom image is unreadable.
wuxm::ImageSource TrashIcon(bool full, wux::UIElement const& el) {
    auto& icon = g_trashIcons[full];
    if (icon) return icon;
    int px = MagnifiedPixels(el, TrashIconScale());
    icon = LoadBase64Image(TrashImage(full), px);
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
    bool mac = g_settings.trashIcon == IconStyle::Mac;
    add(mac ? L"Empty Trash" : L"Empty Recycle Bin", g_trashFull != 0,
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
        bool mac = g_settings.trashIcon == IconStyle::Mac;
        ui.Caption(mac ? L"Move to Trash" : L"Move to Recycle Bin");
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
    if (!InsertAfterRow(panel, row, trash)) return;

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
// button, so it follows its icon sideways without growing.

struct Indicator {
    winrt::weak_ref<wux::FrameworkElement> button;
    winrt::weak_ref<wux::FrameworkElement> element;  // Windows' indicator
    winrt::weak_ref<wux::VisualStateGroup> states;
    wux::VisualStateGroup::CurrentStateChanged_revoker stateChanged;
    wux::FrameworkElement::SizeChanged_revoker sizeChanged;
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

// Centered under the icon, in the dock's bottom padding. `tx` is how far
// magnification moves the icon sideways.
void PlaceDot(wux::FrameworkElement const& button, float tx) {
    Indicator* i = FindIndicator(button);
    if (!i) return;
    Plate* p = GetRowPlate(wuxm::VisualTreeHelper::GetParent(button));
    auto panel = p ? p->panel.get() : nullptr;
    auto row = p ? p->row.get() : nullptr;
    if (!panel || !row || p->baseWidth <= 0 || button.Visibility() != Visibility::Visible ||
        button.ActualWidth() <= 0 || !IsRunning(*i) || g_dragging) {
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
        if (!InsertAfterRow(panel, row, dot)) return;
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
    float centerY = p->top + g_size.dockHeight - g_size.padY * kDotRise;
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

// Hides Windows' indicator and follows its state. The taskbar can replace it.
void TrackIndicator(wux::FrameworkElement const& el, wux::FrameworkElement const& button) {
    wuxh::ElementCompositionPreview::GetElementVisual(el).Opacity(0);
    Indicator* i = FindIndicator(button);
    if (i && i->element.get() == el) return;
    if (!i) {
        i = &g_indicators->emplace_back();
        i->button = winrt::make_weak(button);
    }
    i->element = winrt::make_weak(el);
    i->stateChanged.revoke();
    i->states = nullptr;
    auto refresh = [weakButton = i->button] {
        Try([&] {
            if (auto b = weakButton.get()) ReapplyButton(b);
        });
    };
    if (auto group = FindRunningStates(el, button)) {
        i->states = winrt::make_weak(group);
        i->stateChanged = group.CurrentStateChanged(
            winrt::auto_revoke,
            [refresh](wf::IInspectable const&, wux::VisualStateChangedEventArgs const&) {
                refresh();
            });
    }
    i->sizeChanged = el.SizeChanged(
        winrt::auto_revoke,
        [refresh](wf::IInspectable const&, wux::SizeChangedEventArgs const&) { refresh(); });
    MarkPlatesDirty();
}

void ForgetGoneIndicators() {
    g_indicators->remove_if([](Indicator& i) {
        if (i.button.get()) return false;
        RemoveDot(i);
        return true;
    });
}

void RestoreIndicators() {
    for (auto& i : *g_indicators) {
        i.stateChanged.revoke();
        i.sizeChanged.revoke();
        RemoveDot(i);
        if (auto el = i.element.get()) {
            Try([&] { wuxh::ElementCompositionPreview::GetElementVisual(el).Opacity(1); });
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
    if (explorer != kNone && explorer != firstApp) {
        if (!g_dragging) MoveExplorerFirst(icons[explorer].element);
    } else if (explorer != kNone) {
        NoteExplorerFirst();
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

// Clearing the list also revokes the plates' event handlers.
void RemovePlates() {
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
// to the dock. A window merely touching the dock doesn't count.

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

// The dock gives up its space at once, so the window takes it right away, but
// stays on show as if hovered, sliding away once the pointer's been gone for
// half a second.
void SetWindowMaximized(bool maximized) {
    if (maximized == g_windowMaximized) return;
    g_windowMaximized = maximized;
    g_dockRevealed = maximized;
    g_dockAwayTicks = 0;
    ScheduleOverWindows(false);  // app bars and clip
    StartFrames();
}

// ---- Polling ----

void UpdateAutoHide(POINT p, RECT const& m, UINT dpi, bool held, bool fullscreen) {
    if (g_settings.autoHide == AutoHide::Hybrid) SetWindowMaximized(IsWindowMaximized());
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
// of Windows' window previews and tooltips. It rises above the taskbar, so it's
// a click-through window of its own: a solid light pill with a soft glow.

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

// With its arrow's tip at (`x`, `tipY`) in the plate's panel. The taskbar's
// content starts at its window's corner.
void ShowLabel(Plate& p, std::wstring const& name, float x, float tipY) {
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
    RECT window;
    if (name.empty() || !taskbar || !GetWindowRect(taskbar, &window) || !EnsureLabelWindow()) {
        return HideLabel();
    }
    float scale = (float)root.RasterizationScale();
    auto at = panel.TransformToVisual(content).TransformPoint({x, tipY});
    POINT tip{window.left + std::lround(at.X * scale), window.top + std::lround(at.Y * scale)};
    DrawLabel(name, tip, scale);
}

// Over the icon under the pointer, as magnified (icons grow from their bottom),
// and never lower than the dock's top edge.
void UpdateLabel(Plate* p) {
    if (!g_settings.tooltips || !p || !g_pointerOver || g_dragging || g_labelSuppressed ||
        DockHidden()) {
        return HideLabel();
    }
    float bottom = p->top + g_size.padY + g_size.icon;
    auto above = [p, bottom](float height) {
        return std::min(bottom - height - kLabelGap, p->top - kLabelDockGap);
    };
    for (auto& item : g_row) {
        float x = item.center + item.shift;
        if (std::fabs(g_pointerX - x) > item.width * item.scale / 2) continue;
        if (item.trash) {
            PCWSTR name = g_settings.trashIcon == IconStyle::Mac ? L"Trash" : L"Recycle Bin";
            return ShowLabel(*p, name, p->originX + x, above(TrashArtHeight() * item.scale));
        }
        auto el = item.element.get();
        if (!el) break;
        HideStockToolTip(el);
        bool finder = g_settings.finderIcon == IconStyle::Mac && IsFileExplorer(el);
        auto name = finder ? L"Finder" : AppName(el);
        return ShowLabel(*p, name, p->originX + x, above(g_size.icon * item.scale));
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
    if (s.autoHide != AutoHide::Hybrid) g_windowMaximized = false;
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
        } else if (name == L"RunningIndicator") {
            TrackIndicator(el, button);
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

std::list<TrayShadows> g_trayShadows;

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
    g_trayShadows.remove_if([](TrayShadows const& t) { return !t.grid.get(); });
    for (auto& t : g_trayShadows) {
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

    auto& t = g_trayShadows.emplace_back();
    t.grid = winrt::make_weak(grid);
    t.layer = layer;
    t.container = wuxh::ElementCompositionPreview::GetElementVisual(layer).Compositor()
                      .CreateContainerVisual();
    wuxh::ElementCompositionPreview::SetElementChildVisual(layer, t.container);
    t.layoutUpdated = layer.LayoutUpdated(
        winrt::auto_revoke, [weak = winrt::make_weak(layer)](auto&&, auto&&) {
            for (auto& t : g_trayShadows) {
                if (t.layer == weak.get()) Try([&] { UpdateTrayShadows(t); }, L"Tray shadows");
            }
        });
}

void RemoveTrayShadows() {
    for (auto& t : g_trayShadows) {
        t.layoutUpdated.revoke();
        wuxh::ElementCompositionPreview::SetElementChildVisual(t.layer, nullptr);
        uint32_t index;
        auto grid = t.grid.get();
        if (grid && grid.Children().IndexOf(t.layer, index)) grid.Children().RemoveAt(index);
    }
    g_trayShadows.clear();
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
// Centered on where the button is, not where layout put it (File Explorer).

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
    return ToRawPixelsInDesktopCoordinates_Original(moved, dpi, hWnd);
}

// ---- Dragging (as in "Taskbar Multirow") ----
// File Explorer keeps its spot, so its drag never starts. Other apps go no
// further than the first and last spots after it, where they'd land if dropped
// there, so nothing jumps on release. Points are in the button row's coordinates.

winrt::weak_ref<wux::FrameworkElement> g_blockedDrag;
float g_dragMinX = 0, g_dragMaxX = 0;
bool g_dragLimited = false;

// The pointer's range that keeps the dragged icon within those spots: the icons
// of the second and the last app, as they are when the drag starts.
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
    g_dragMinX = start.X + iconLeft(apps[1]) - iconLeft(*dragged);
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
        explorer = button && IsFileExplorer(button);
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

void MoveExplorerFirst(wux::FrameworkElement const& explorer) {
    Button* b = FindButton(explorer);
    auto name = UserValueName(L"explorerMoves");
    int tries = Wh_GetIntValue(name.c_str(), 0);
    if (g_explorerMoved.get() == explorer || !b || !b->impl || tries >= 3 ||
        !TaskListButton_OnDragStartedGesture_Original || !TaskListButton_UpdateDrag_Original ||
        !TaskListButton_OnDragCompletedGesture_Original ||
        !TaskListDragOperation_GetDropPlaceholder_Original || ElementFromThis(b->impl) != explorer) {
        return;
    }
    auto queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (!queue) return;
    g_explorerMoved = winrt::make_weak(explorer);
    void* impl = b->impl;
    queue.TryEnqueue([weak = winrt::make_weak(explorer), impl, name, tries, pin = PinModule()] {
        auto explorer = weak.get();
        if (g_unloading || g_dragging || !explorer) return;
        Try([&] {
            if (ElementFromThis(impl) != explorer) return;
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
    });
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

// ---- The taskbar window ----

using TaskbarConfiguration_GetFrameSize_t = double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    return OverrideHeight(enumTaskbarSize)
               ? g_taskbarHeight
               : TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}

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
// size"): ucomisd xmmN, [reg + offset], then jp, then je or jne.
LONG TrayLastHeightOffset() {
    static LONG offset = [] {
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
// the start of OnGroupingModeChanged: sub rsp, 28h / mov rax|r8, [rcx+offset]
void* TaskbarController_OnGroupingModeChanged_Address;

LONG GetTaskbarFrameOffset() {
    static LONG offset = []() -> LONG {
        const BYTE* p = (const BYTE*)TaskbarController_OnGroupingModeChanged_Address;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B && (p[6] & 0xC0) == 0x80) {
            LONG o = *(LONG*)(p + 7);
            if (o > 0 && o <= 0xFFFF) return o;
        }
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

// Read from the IconHeight setter: movsd xmmN, qword ptr [rcx+offset]
void* TaskListButton_IconHeight_Address;

size_t GetIconHeightOffset() {
    static size_t offset = []() -> size_t {
        BYTE* p = (BYTE*)TaskListButton_IconHeight_Address;
        std::regex regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                         std::regex_constants::icase);
        for (int i = 0; p && i < 30; i++) {
            WH_DISASM_RESULT result;
            if (!Wh_Disasm(p, &result)) break;
            p += result.length;
            std::string_view text = result.text;
            if (text == "ret") break;
            std::match_results<std::string_view::const_iterator> match;
            if (std::regex_match(text.begin(), text.end(), match, regex)) {
                size_t o = 0;
                std::from_chars(&*match[1].first, &*match[1].first + match[1].length(), o, 16);
                Wh_Log(L"Icon height offset: 0x%zX", o);
                return o <= 0xFFFF ? o : 0;
            }
        }
        Wh_Log(L"Icon height offset not found; icon resolution unchanged");
        return 0;
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
// Previews only open on click; hovering shows the dock's labels instead. As in
// m417z's "Disable Taskbar Thumbnails".

// ---- Older builds ----

using CTaskListWnd__DisplayExtendedUI_t = HRESULT(WINAPI*)(void* pThis, void* taskBtnGroup,
                                                           int param2, DWORD flags, int param4);
CTaskListWnd__DisplayExtendedUI_t CTaskListWnd__DisplayExtendedUI_Original;
HRESULT WINAPI CTaskListWnd__DisplayExtendedUI_Hook(void* pThis, void* taskBtnGroup, int param2,
                                                    DWORD flags, int param4) {
    bool click = flags & 2;
    if (!click && !g_unloading) return S_OK;
    return CTaskListWnd__DisplayExtendedUI_Original(pThis, taskBtnGroup, param2, flags, param4);
}

using CTaskListWnd__ShowToolTip_t = void(WINAPI*)(void* pThis, DWORD flags);
CTaskListWnd__ShowToolTip_t CTaskListWnd__ShowToolTip_Original;
void WINAPI CTaskListWnd__ShowToolTip_Hook(void* pThis, DWORD flags) {
    if (g_unloading) CTaskListWnd__ShowToolTip_Original(pThis, flags);
}

// ---- Newer builds ----
// A XAML flyout. While it opens on hover its window is kept hidden, and it's
// shown after all if it then turns into a click (sticky) flyout.

struct HoverFlyout {
    HWND hwnd = nullptr;
    int show = SW_HIDE;
    BOOL enable = FALSE;
};

std::atomic<DWORD> g_hoverFlyoutThread = 0;
bool g_inStickyFlyout = false;
HoverFlyout g_hoverFlyout;

bool InHoverFlyout() {
    return g_hoverFlyoutThread == GetCurrentThreadId() && !g_inStickyFlyout;
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

// =============================================================================
// HOOKS: MAXIMIZED WINDOWS
// =============================================================================
// Maximized windows reach down to the dock, and magnified icons rise over them:
// - Auto-hide is turned on, so Windows keeps no space for the taskbar, which
//   is kept from hiding (as in "Taskbar auto-hide when maximized").
// - An app bar keeps the dock's strip free on each screen, unless the dock
//   hides itself, when windows fill the screen.
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

// No region (null) shows the whole window.
HRGN TaskbarRegion(HWND tb) {
    RECT window;
    if (!KeepTaskbarShown() || g_dockExpanded || !GetWindowRect(tb, &window)) return nullptr;
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
    if (KeepTaskbarShown() && !DockHides()) {
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
        // Window previews (gone in 10.0.26100.8313)
        {{LR"(protected: long __cdecl CTaskListWnd::_DisplayExtendedUI(struct ITaskBtnGroup *,int,unsigned long,int))"},
         &CTaskListWnd__DisplayExtendedUI_Original, CTaskListWnd__DisplayExtendedUI_Hook, true},
        {{LR"(protected: void __cdecl CTaskListWnd::_ShowToolTip(enum ShowToolTipFlags))"},
         &CTaskListWnd__ShowToolTip_Original, CTaskListWnd__ShowToolTip_Hook, true},
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
          // Older builds lack the last parameter; passing it anyway is harmless on x64.
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
    // Window previews, on newer builds.
    if (HMODULE win32u = GetModuleHandle(L"win32u.dll")) {
        if (auto p = GetProcAddress(win32u, "NtUserShowWindow")) {
            WindhawkUtils::SetFunctionHook((ShowWindow_t)p, ShowWindow_Hook, &ShowWindow_Original);
        }
        if (auto p = GetProcAddress(win32u, "NtUserEnableWindow")) {
            WindhawkUtils::SetFunctionHook((EnableWindow_t)p, EnableWindow_Hook,
                                           &EnableWindow_Original);
        }
    }
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

    // The elements first, so the taskbar sizes its icons itself below.
    if (HWND tb = FindTaskbar()) RunFromWindowThread(tb, [](void*) { ResetAll(); }, nullptr);
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
        g_finderIconSource.reset();  // reloaded at the new size and style
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

// =============================================================================
// EMBEDDED ICONS
// =============================================================================
// macOS's Finder and Trash icons as base64 PNGs, exactly as supplied.

const char kFinderIconPng[] =
    "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";

const char kTrashEmptyIconPng[] =
    "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"
    "TwPLO7tkwEFtA9BH7vC8uSgK1Go1G/RRLnBQGPE9YAOcmFxpIZTFNO6kmlJwueAWx2c8PaF86p1akJICSpYxL4T/KINvbEGj"
    "1U3RLQrA3IsKQX5TIKvyNpX0h8Y7xjCk3jAuKgZAY8tTpmw0Go0Gg0EPQLG8vHzlAx/4wPufeOKJJ9rtduP4+Hg8HA5HrVar"
    "ceXKFffCCy+89vu///t/eO3atcatW7eW/+AP/mDfe3+EyRtJfJ70Ow7m+wlgb/5hc3PzsF6v+2azmT/wwAPXvvKVr3x1f3//"
    "+C/9pb/03O3bt5f29/fP9vf3TxuNRv3pp59+4vHHH390dXV1fW1tbWtnZ6e7v79/1O/3z5xzeW2S8izLMtlpRC6PDwLEgjch"
    "ysdlEwcgnu4ociDglvsoXVzrTlZeSCfh91mWOXmVkayvRFBTEfbYFreRaofLSt0W8BbQ9kUGuS90AnG3mQqasQcRrhX9BKC4"
    "XCOgIF7fa6rBU4x4BJKsKMiaN42t2ulm+WgViQCao/ykjEVOMRqNxv1+fzQajUb1ej1bWlpqP/LIIw/dvn37gYceeuih5eXl"
    "Vq/Xw8HBwRkAd/369eZgMCi+8IUv/PGLL7741Rs3bixcuXKllmWZ6/f7xwAGmAD4vqbzsMARwKenp/tFUQyzLPONRiN78MEH"
    "r62trW1+/vOf/8Jzzz33/u/4ju948urVq42Dg4Oz3d3d03q9Xn/yySe/5Vu/9Vu/5eDgoNftdg+2t7d3Dw4ODg8ODo4PDg5O"
    "T09P++Fc6DEFKESbOgAI3ydiNy9q5kIfz+pIAIDSfebD1SqAFuEWIaN3lksmBGERMLGQhyWWKNVFUbDbLgARYMU2EspGWRZ2"
    "xa3lZ0sLApGxnBH89JogL+copUav8YlSiMrFvrPtXPnCvyi2SfwyKhz2IArxTlgRSB/FixBaSXnIIHhQnFA8kFqtltfr9bxW"
    "q+WLi4sLDzzwwI3l5eUr169fv37z5s0by8vLi7VaDb1eD3t7e2fOObe0tNTI8xyvvfbaxh/90R+9uL29vXP79u3lhYUF55wb"
    "1+v1bG1trZOQ//viSp+XC+0BYGVlZbff75/V6/Xs7OzMO+f8gw8+eO3k5GTw27/921965ZVX7n7gAx944sknn3zk2rVr7V6v"
    "5w8PDwdFURSNRqPx6KOPvufJJ598z2g0Qq/XOzs8PDzp9Xr94XA4Pjk5OTk9PT0bDoejfr/fHwwGowDs4XA4HI9Go/FoNCpE"
    "mCQVsqu+tDwxqBLA5bMsQxCuCErr3oW6Kq6ZCBLfB/ThAmxtxaJZ10GEn4HJz4SYlGtZGQwzX2YFwfWF6zjfpGWlyrx5SlNx"
    "zs2WnNtxVBHRojyFoCAlKCbeknfOubCF1bnJB+Pi6kEIXmX55CT3er1ez8MrrI3mJNUXFhYWWq1Wo16v15eWlhbb7Xar0WjA"
    "e4/BYICTk5NRURS+0WjUr1+/3iiKAisrK1svvfTSG1/72tdWms1m/vDDD990zo0AFM45X6vViv39/UPhWYop72S63wBW2uf4"
    "+PhwNBr1g+s7yrLMj8fj8eLiYrPVajU7nc7+r/3ar/3Be97znhvf/u3f/uijjz56++bNm1fb7Xbz7OwM/X5/PJycbudqtVp+"
    "69at67VaTQUmRMDG47FYjiIc9F6ICzgej8UiRACbbYRyPwLUbswgzR7frCFX1+ZTtJGyiPcJd56KqYCQterynKxr+WA6qCQp"
    "AIKmCvJQ6qA1dw4YJV1ajtxSG6mdWxzYci6cOEpl1TZSaZOVkvwkAMuOKhdoyfJJiidOSkRaFKJMv0S+Tk9PizzPs0ajkV+7"
    "dq3mvcfBwcHJSy+9tPvyyy/ffeONNzYB+Nu3by81Go0MwDjLMp/nua/Vaq7f7w86nY58W+m+WV5J5xnEyjc3N/e63e7RwsLC"
    "0snJydCVcxafZRlu3LixeOXKlYVut3v8m7/5m88vLCy0HnzwwWvf8i3fcvuhhx66fuPGjeWrV69eaTQamYB0OBxiNBoVIYmE"
    "88C6LMtqk/cm9BZHQEco+a9cy0Cnnqd+z9PXl4zeqdxjJTftOeex1957jEYjTwlAVDZ5lmWo1+tot9tw4ZOyvV5vtLe3t7ez"
    "s7N/9+7drbW1tc7+/v6Jcw63bt1aXlhYyIIi8FmWxSWwWq2WhU+e7gt55u87ns5rDlwAqO/u7h4dHx8fv+c977mZZdlpOL/X"
    "+8kxOc5Nghf+6tWrrStXrrT6/f7w9ddf37lz5856q9WqLy8vtx944IHlmzdvLi8tLbWvXLnSunr16uLi4mKr2Ww2Wq1WvVar"
    "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"
    "Wd7zHJ94Ej9XIzLC3pIFrvym/qjpDuXnqZqVNQ9APujdAXCICZbOdQ0YuFgL7IHJW0l5nlcAIEwTARC3JzyLjBbhZPeQ2lKB"
    "FSOUqn4gDTor3HIaheSXiiywGGAWKHx/mhJKzZXts0BPpW+p/tp67LNp1ylQT/sLY1EFoADUvJ6svPe+atFtX2XpyMz7kzRw"
    "3Ui44gRmrk8Fuag+lhG1HgwA9Xq9Fk6ijG79eacLO5FDrjc3N7d7vV4/K9eClECw+pOBY0aDhILAJwOumDot4khJnSgp9HAZ"
    "Bl5CgK3FiHVIfgb5veqVM5gD3aottsQpl14Um5TlfjCP7PyV4wTyT4RbABlokDmNk2uerjNPxGLJMCbm4eUglm2Cr80YKDeY"
    "5cDkF/AlT2qB8ZoSHpp92b8yP67Vam51ddUeJcuG6r6niwxiCYA7vV7vNM/zGjMS0MehWO0Hco1FsDhiza4l1RXrsIe2k/BG"
    "jezLZRJRGEnXTNqktoTOCAIj5PEeH2VL/HFFUcR5Ic0P2aqpYBwJrSghBUSmhYDI9SOjYINVZNRPvcsF5VKN8FkUikTHpSn2"
    "YOQ+8V/uK2vGCosVnVXYnJfHhMeIk4yxPJ4yt4+JywmvBoNBf2trays84p0n7+ooNHeutrm5ubO7u3vQarWaMACBcVt4EOTa"
    "ubj9UQUsRKjYArHcsTWW+sJ8SpZyprlcUejlNwMs9IGBGds35SIdFnTsGjM/ElaA15wdtcN9itdGEap8wQPyQDziFcxL7oMa"
    "zMQEVujl+Smgt4NyZF6AbpRockwTUw1l8QTkqfE1yqvCFxrjCF5AnwIq7eV5nvd6vf76+vqFrQEDF2+B61tbW/t7e3v7rVar"
    "lQCq43vCUzt/BNSB3I4Hh62yK3fQKMVK1lkpEONK2wPGY1spwbKCyImEN1bHtOZ57mq1muoHMcTem6bY+J5SGim6RBGQlbS8"
    "Zz44ppcCZGwJmUcCBEe/WRZglQbTaMfdejLihcm0AjT3NfUoK878I2/PySd/5LlhsnMufk50P3wPifO/a4NYNnlMtnLub2xs"
    "dNrt9gKA1ADKIIkgxcFh4edkBK5Sl7TPc0BdPLaZcsM8BbKU1mdh4z6YAFx0QWm6oGiSeWOiL+rgN+ELE85TAikv7TEimY8g"
    "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";

const char kTrashFullIconPng[] =
    "iVBORw0KGgoAAAANSUhEUgAAAPAAAAEACAYAAACNsPs/AADSZUlEQVR4nOz9d6Bkx1knDP+eqhM63Dz33smj0QRlyXKSZMtG"
    "smxhMDYy2OwaE4zBxh+wLBiWDbCAeNkEmH2XBXsJa/zCsn4x3y6YuLZssGVbkpVzGIVJd24O3bfj6RPqef+ocKrvjBwWzYzM"
    "qqSe7tt9Qp2qJ/yeUE8RM+Ol9lJ7qX1jNnG+O/BSe6m91P7320sM/FJ7qX0Dt5cY+KX2UvsGbi8x8EvtpfYN3F5i4JfaS+0b"
    "uL3EwC+1l9o3cHuJgV9qL7Vv4PYSA7/UXmrfwO0lBn6pvdS+gdv/EQz8+7//+5idnQURvfT6Ol7vfe97z+U0kXmdkzYH0Jx3"
    "vwUAT/SAbgNoFsA6gCaAF32eIjP/g33ddtttuOaaa873EH9Dt2/+5m/G2traeZ/LF5w2zMv+vcGM9RdBv77eFzG/6GXM19z6"
    "/T4eeughfPKTn8QXvvAFfPnLXz7fXfoH0bZt24bf/u3fxjvf+c6veFyz2cRDDz2EY8eO4d5778WFF16In/mZnzkrfUqSBHNz"
    "czh8+PD/1vl3mvfXmvc1AAMAu1+Avp3Tdr4lyN/3tbi4iI997GN473vfi9nZ2fM9nP+g2wc/+EE37pubm/jMZz6DX/7lX8Y7"
    "3vEOXHLJJajX66edc8011+C22257Qef8ox/9KPbu3QsA+MEf/EE88cQTX/c17jAv+/caM+ZfBPT89b7Oewf+d19/+qd/iu/9"
    "3u9FpVL5SjRH5+H1D7pde+21eNvb3oaxsbGv67ybb77578XIjUYDv/d7v4dLL730jNf/ehn5dmbcd4bvv/z5z+ODH/wgHn/8"
    "8fNO4/8gGThNU7zjHe94PjrZykzCvM42Y22914uZob/SGJ31fn+9jNxoNHDrrbdiYmLia7r+z//8z6PRaHzV624y4wlmPPv4"
    "4/jJn/xJvP+Hfxhvfdvbhq71rne9Cz/yIz+Cn/mZn8HP/MzPYDAYnHf6/4Zm4Lvvvhsve9nLts7ZVkJ8Xs+6lBK1Wg1xHCOK"
    "IlSr1efV4EII1Go11Go1VKtVjI6OYHR0FPV6HbVaDaOjo1+NliTOjfD4Wps/TvIrHOf3+6wx81dj5Gaz+XUxrt8mJiZw6623"
    "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";