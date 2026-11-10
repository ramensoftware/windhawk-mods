## 2.3.19 ([Nov 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a06823eedc04704de05a94fff7778a2410e72068/mods/taskbar-clock-to-left.wh.cpp))

* Support horizontal `StackPanel` tray layouts alongside `Grid`.
* Preserve the clock button's margins when moving it.
* Use the measured bounds of unfamiliar taskbar buttons to avoid overlap.
* Retry placement when the clock text size changes. After a width-related fallback, wait for the clock controls or sizing inputs to change before retrying.
* Track native tray extent, reserved width, and obstacle positions separately when comparing layouts.
* Use the native clock position when the taskbar item list is missing, and reuse a valid list on routine layout updates.
* Update the README.

## 2.3.15 ([Oct 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bfe2a6241a0320f9138140c899b62def49baece7/mods/taskbar-clock-to-left.wh.cpp))

* Place only the native clock/notification button in a dedicated left-side container while preserving its text, calendar and notification interactions.
* Follow readable native system-tray background, border and spacing properties, with asymmetric decoration facing the left screen edge.
* Keep clear of fixed system buttons and application buttons without moving applications or reserving their viewport.
* Restore the native clock when placement is unsafe and retry when effective space, text size or layout changes.
* Support one or two clock text lines; return three or more lines to the native position.
* Limit relocation to single-monitor desktops, keep native waiting observers while multiple monitors are connected, and retry when returning to one monitor even if the layout dimensions are unchanged.
* Remove the internal settings and the earlier experimental tray-group migration and viewport-reservation paths.

## 2.2 ([Sep 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9b682750ef56a668a628b3da682b49100cee3c7d/mods/taskbar-clock-to-left.wh.cpp))

- Place the relocated clock to the right of a Start button pinned to the physical left edge while taskbar apps remain centered.
- Recalculate the clock offset after taskbar layout updates without repeatedly searching the whole visual tree.

## 2.1 ([Sep 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/66142e7d3d98559b85934b8d74a9915b63c8cead/mods/taskbar-clock-to-left.wh.cpp))

Initial release.
