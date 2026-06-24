## 1.2.1 ([Jun 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f1df18956cac038a20d04e9d08f371c6121e90ed/mods/disk-usage-bar-in-drive-properties.wh.cpp))

* General code optimizations.
* Removed the supported OS versions list from the Readme, as it is unnecessary; the mod will function as long as the required debug symbols exist on Microsoft's servers.

## 1.2 ([Mar 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7749d8190cab3e0f243aed47c317204e64bf21e9/mods/disk-usage-bar-in-drive-properties.wh.cpp))

Added an option to hide the "Details" (Windows 11) or "Disk Clean-up" (Windows 8.1/10) button, which is recommended for localized systems to prevent a UI collision with a long "Space used" string for the disk usage percentage text.

## 1.1 ([Mar 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ded365a21ed5b60cb63a37e7cf9f23f5e2ba194f/mods/disk-usage-bar-in-drive-properties.wh.cpp))

* Added an option to display the disk usage percentage text with one decimal place (e.g., `64.1%`).
* Refined disk usage percentage rounding logic to cap values between 99.5% and 99.9% at 99% when displayed as whole numbers, ensuring "100%" is shown only when the disk is completely full.

## 1.0 ([Feb 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c2e624b0ebeb3c649073ec5656655b1bebc65089/mods/disk-usage-bar-in-drive-properties.wh.cpp))

Initial release.
