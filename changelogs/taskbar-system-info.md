## 1.7.1 ([Oct 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/05168884e6fe6c3cbfa16abec0f2ccd6ab5e79cd/mods/taskbar-system-info.wh.cpp))

* Add automatic placement around visible taskbar controls, with readable shrinking, collision-safe reservation, and hide/restore when free space changes. Remove the 1,000-pixel left-offset cap.
* Add four automatic layouts for available taskbar width and height: full, graphless, compact two-row and compact one-row. Keep essential readings visible and restore hidden details when space returns.
* Add opt-in live dragging through a configurable shortcut, disabled by default, with Enter to save, Esc to cancel and Home to reset. Save positions separately for each display and revalidate before applying them.
* Stop displaying stale collector readings as current; age graph history during a stall and recover on fresh publication.
* Reject stale/frozen HWiNFO Shared Memory and Gadget Registry temperatures, with automatic fallback to remaining providers.
* Isolate native GPU-temperature failures from other GPU metrics, retry with fresh handles, and recover persistently invalid GPU engine samples through a separately primed query.
* Contain move-editor and display-enumeration exceptions, reject stale window classes and release XAML references on their owning thread. Reuse unchanged placement/font data and avoid unrelated settings cancellations.
* Refresh the built-in guide, layout examples and English/Ukrainian setting descriptions. Preserve existing setting defaults; add only the optional move shortcut.

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
