## 1.5.0 ([Sep 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e81aaa5fd7a81fb12eb0ebde2ce65a435be0d936/mods/taskbar-system-info.wh.cpp))

- Add a **Taskbar monitor** setting to place the widget on the primary or a secondary display. If the selected display is unavailable, use the primary taskbar and retry the selected display automatically.
- Add **adaptive colors** for light, dark and Windows high-contrast themes, while retaining manual color settings.
- Scale the widget down on short taskbars so its two rows fit without clipping.
- Use the Windows Processor Utility counter for CPU usage when available, aligning the reading with Task Manager's frequency-aware measurement. Keep the existing CPU counter as a fallback.
- Add **Automatic / Dedicated / Shared** GPU-memory selection and improve automatic handling of integrated GPUs with a small dedicated-memory reservation.
- Improve recovery when GPU/VRAM counters stop returning data, while keeping CPU and RAM readings visible. Show unavailable GPU readings as `--%` instead of treating them as idle.
- Keep CPU/GPU graph history when samples are missing, show gaps for unavailable data, and use measurement timestamps to preserve the selected history duration.
- Improve HWiNFO temperature-sensor matching when sensor records change, and fix temperature parsing with comma decimal separators.
- Expand the built-in guide with a quick start, explanations of each setting, HWiNFO setup instructions and troubleshooting for missing temperatures.

## 1.3.3 ([Aug 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5305a79fdc7c0a10a0a04af1dcddf2578dd59545/mods/taskbar-system-info.wh.cpp))

Initial release.
