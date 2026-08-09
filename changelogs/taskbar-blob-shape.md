## 1.1.0 ([Aug 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/53c9298760a1b89b1fbf686d7240d29dc65bc566/mods/taskbar-blob-shape.wh.cpp))

* Extends the blob beyond task list buttons, each group behind its own toggle (all default off): Start/Search/Task view, Widgets, date and time, and the other checkable system tray buttons (control center, language, overflow chevron), shown while their flyout is open — on every monitor's instance, matching the native active highlight.
* System buttons are event-driven (ToggleButton Checked/Unchecked, with a CurrentStateChanged fallback) on top of the existing entry/geometry/expression/lifecycle machinery, discovered by sweeping each taskbar's repeater (re-swept on grid SizeChanged) and an optional hook on TaskbarResources::OnExperienceToggleButtonVisualStateChanged for islands without task-button activity. The tray is reached via XamlRoot().Content(), hosts blobs at the bottom of SystemTrayFrameGrid's z-order, and re-discovers elements on the tray grid's SizeChanged. Notification-area app icons and MainStack status indicators are deliberately excluded (no open-state exists for them: Shell_NotifyIcon is a one-way protocol, and the indicators only track hover states).
* Fixes the CustomColor setting description being truncated in the UI (space-before-# triggered YAML's comment rule) and documents the Light | Dark color syntax with concrete examples.

## 1.0.0 ([Aug 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e5d8d9d8d4380fd4f18e08737e3925bc0c48665d/mods/taskbar-blob-shape.wh.cpp))

Initial release.
