## 1.2 ([Nov 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fd84d413a1340b4f4f306423e127368d8518757b/mods/taskbar-live-hud-radar.wh.cpp))

* Added GPU utilization (%) via Windows PDH (`\GPU Engine(*)\Utilization Percentage`).
* Added GPU temperature (NVIDIA NVAPI, AMD ADL, and LibreHardwareMonitor/OpenHardwareMonitor WMI fallback).
* Added CPU temperature (LibreHardwareMonitor/OpenHardwareMonitor WMI and Windows ACPI/ThermalZone WMI fallback).
* Added temperature unit option (`°C` / `°F`).
* Added physical disk Read, Write, and Total speed modes via Windows PDH.
* Added GPU % line to the mini-graph popup when GPU monitoring is enabled.
* Added RGBA and `#RRGGBBAA` opacity support for background, border, and metric colors.
* Added custom border color and per-metric custom colors.
* Added configurable font size, taskbar widget min/max width & height, and mini-graph popup dimensions.
* Added customizable mouse click actions (Left, Ctrl+Left, Right, Ctrl+Right), drag modifier, and mouse-hover activation with configurable delay.

## 1.1.1 ([Nov 11, 2026](https://github.com/ramensoftware/windhawk-mods/blob/074ed83b4b131627bfd23bf64957fbc721906920/mods/taskbar-live-hud-radar.wh.cpp))

* Hides when the taskbar is auto-hidden or made transparent.
* Smoothly follows the taskbar auto-hide animation with zero delay.
* Detects fullscreen windows (F11, media players, games) much faster.
* Fixed an issue where the HUD would remain visible when the taskbar was retracted.

## 1.1 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/df8b56732c81d756c4f32247780be4448a6f11c1/mods/taskbar-live-hud-radar.wh.cpp))

* Added real-time CPU & RAM history mini-graph (Ctrl + Right Click).
* Added default gateway (local router) ping option with zero internet traffic.
* Switched network speed units to Mbps / Kbps.
* Added custom background color setting (HEX).
* Improved background transparency via per-pixel alpha (text and bars remain crisp).
* Added font fallback for Windows 10 support.

## 1.0 ([Nov 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/08e84491fa69a919455b948e06fd1539ca6c8963/mods/taskbar-live-hud-radar.wh.cpp))

Initial release.
