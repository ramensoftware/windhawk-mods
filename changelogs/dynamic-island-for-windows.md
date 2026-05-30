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
