## 1.1.1 ([Jun 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f9abdadf12d052e7120571bdf52f578ad98c4333/mods/dynamic-island-for-windows.wh.cpp))

## Fixes

* Fixed a browser error that could occur when clicking on the Dynamic Island widget.
* Fixed mouse wheel scrolling issues, including delayed responses and unintended looping behavior.
* Fixed an issue where widgets could disappear after clicking on the desktop or using Win + D.

## Improvements

### Weather

* Improved the Weather UI.
* Added support for Imperial units (°F, mph, etc.).
* Added auto-hide functionality.
* Added hover behavior settings.

### Media Controls

* Added click-to-expand functionality for the media widget.
* Added a persistent media pill option.
* Centered media playback controls for better alignment.
* Added click animations to media control buttons for improved feedback.

## 1.1.0 ([Jun 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/55338f1accb10a333568251590275cc3b0e1a77f/mods/dynamic-island-for-windows.wh.cpp))

This release is a big step forward from v1.0.2 — we’ve gone through the code top to bottom and added a ton of polish. Below is the full breakdown of what’s new, what’s improved, and what’s fixed.

---

## ✨ What's New
- **Complete Weather & Calendar UI Overhaul:** A new interactive calendar and a rebuilt weather engine powered by `wttr.in`. Weather now shows detailed info like temperature, wind speed, humidity, and “feels like” values.
- **True GPU Tracking:** The Game Overlay now tracks GPU usage in real time using Windows Performance Counters (PDH) for smooth and accurate metrics.
- **Rich Notification Scraping:** Notifications now display rich text using Windows UI Automation (`uiautomation.h`), not just app icons.
- **Custom Positioning Offsets:** Added `Offset X` and `Offset Y` settings so you can fine‑tune the island’s position to match your setup.
- **Multi‑Monitor & Follow Mouse Mode:** Implemented multi‑monitor support along with a “follow mouse” mode, so the island can adapt seamlessly across different displays.

## 🛠️ Improvements
- **Privacy Indicators:** Replaced old polling logic with native queries to Windows `CapabilityAccessManager` Registry for instant, accurate detection.
- **Smart Alignment:** Media waveform and pagination dots shift automatically when privacy icons appear, keeping the layout clean.
- **Security First:** Even with “Always Show” disabled, the island pops up with clock/battery and privacy dots when your camera or mic is used.
- **UI Polish:** Calendar and weather modules got a full visual refresh — sharper typography, centered layouts, and sleek pagination dots.
- **Docs & Community:** The Readme and setup guides were rewritten to be clearer and more user‑friendly, with a new Feedback section and credits to contributors.

## 🐛 Fixes
- **Media Controls:** Improved spacing and hit‑testing for playback buttons, now responsive across all DPI settings.
- **Direct2D Crashes:** Fixed rendering bugs that caused crashes or flickering during fast expansions or hover states.
- **Hover Logic:** Adjusted idle hover behavior so expansions only trigger when the island is visible, preventing phantom triggers.

---

## 🙌 Special Thanks
Big thanks to **[@ciizerr](https://github.com/ciizerr)** for the layout refinements and bug fixes.

## 1.0.2 ([May 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/63bfb7795abeb4c8d6067b2dccda957b38bfd34e/mods/dynamic-island-for-windows.wh.cpp))

1. Native Windows 11 Fluent Style:** Adds a toggleable mode to render the island as a modern Windows 11 flyout (rounded rectangle with 8px corners and a 1px border highlight) with a new semi-translucent Fluent dark preset.
2. Game Overlay DPI Scaling & Alignment:** Refactored overlay cards, vector icons, and panels to scale proportionally with `settings.sizeScale` and centered the layout inside the expanded pill.
3. Caps Lock & Num Lock Hook:** Implemented a lightweight global hook (`WH_KEYBOARD_LL`) to show real-time lock-key indicator alerts.
4. Dynamic Battery & Power Alerts:** Hardware-polled alert triggers with a dynamic fluid vector battery icon that changes color based on charging state.
5. USB & Device Connection Alerts:** Added a system event listener (`WM_DEVICECHANGE`) to show beautiful plug status alerts with indicator lights.
6. Tactile Asymmetric Spring Animations:** Upgraded motion curves with separate expansion and contraction physics properties for a more organic feel.

## 1.0.1 ([May 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b6e013b94fe87b954b5fa2621b7388d150cfae22/mods/dynamic-island-for-windows.wh.cpp))

Fix: Remove architecture restriction.

## 1.0.0 ([May 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c0b6582ad2161f8406e294b2a74639851923cfac/mods/dynamic-island-for-windows.wh.cpp))

Initial release.
