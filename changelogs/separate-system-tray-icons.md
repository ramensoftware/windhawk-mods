## 1.2.0 ([Nov 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/19a199cce5803268c81d35b593bb0cb0319e58fd/mods/separate-system-tray-icons.wh.cpp))

* Added _**Volume percentage**_ to the Sound context menu, displaying the volume beside the glyph with a `%` sign.
* Added _**Constant width with volume percentage**_, keeping the Sound button width steady and the percentage right-aligned.
* Added middle-click action dropdowns for Bluetooth, Network, and Battery buttons with additional actions:
  - _**Toggle Bluetooth**_
  - _**Toggle Wi-Fi**_
  - _**Toggle Energy saver**_
 * _**Increase chevron size**:_ enlarges the glyph to 16 px to match the tray icons.
* Control Center’s _**Open Project**_ action now replaces _**Taskbar settings**_ with _**Display settings**_ in its context menu.
* _**Sound output**_ flyout is now triggered with command instead of keyboard shortcut

## 1.1.0 ([Oct 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5c80fd1a143de2c86028e8fc91dae73f880ccb51/mods/separate-system-tray-icons.wh.cpp))

# Separate System Tray Icons v1.1.0

* Network, sound, and battery buttons now read Windows native taskbar glyphs and layers. Button and hover-highlight dimensions follow native taskbar sizing and layout, improving consistency across taskbar sizes and orientations. Battery rendering no longer requires manually installing its font.
* Added _Swap troubleshooting position_ for context menus.
* Added _Show master volume slider in Sound output_ flyout.
* Added _Show leaf icon in Energy saver_ mode.
* Added custom middle-click actions across all buttons.
* Trigger _Project_ flyout using the _Control Center_ button.
* Added vertical stacking for the Battery glyph and percentage, adjusted spacing and centering.
* Kept the Battery glyph working through taskbar size and position changes without restarting Explorer.
* Fixed Energy saver and percentage items remaining greyed out when enabling Energy saver.
* Added settings to define horizontal and vertical padding.

## 1.0.0 ([Sep 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/953bdd63123ce7146fb52a71e53e8261369f7a92/mods/separate-system-tray-icons.wh.cpp))

Initial release.
