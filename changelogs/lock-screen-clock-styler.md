## 0.6.0 ([Nov 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7a2157258d4698ee9eb809ea45719051a62ed99a/mods/lock-screen-clock-styler.wh.cpp))

* Added 3 brand-new Vertical Stack Clock styles (Pixel / Nothing OS, Modern Dual Weight, Heavy Poster Impact) with bespoke 2-row line spacing and weight hierarchy.
* Fixed black lock screen background after laptop wakes up from sleep (#6978) via native display and power state callbacks (`PowerSettingRegisterNotification` & `RegisterSuspendResumeNotification`) to pause background polling during quiesce.
* Added a 1000 ms stabilization delay upon wake before resuming clock ticks, preventing contention while LockApp's COM and DXGI pipelines restore wallpaper.
* Added window and element visibility guards (`IsWindowVisible` & `Window::Current().Visible()`) before inspecting or modifying the visual tree.
* Reordered theme selection options across all 14 supported languages to place vertical themes right after Classic Flip Clock (positions 4, 5, 6).
* Closed startup settings synchronization race conditions to ensure preferences apply immediately on shell launch.
* Updated README and mod header previews with new vertical clock screenshots.

## 0.5.0 ([Oct 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2541b69f337db2d7ca6af06371083de0156500f9/mods/lock-screen-clock-styler.wh.cpp))

* Added comprehensive localization for 12 new locales (Simplified Chinese, Traditional Chinese, Japanese, Korean, German, Spanish, Brazilian Portuguese, French, Italian, Polish, Russian, Ukrainian) alongside English and Turkish (14 languages total).
* Increased default digit font size from 92 to 140 for improved readability and prominence on modern displays.
* Enabled live seconds toggle by default across all supported themes.
* Streamlined feature overview and reordered screenshots showcase to feature Retro Pixel.
* Clarified glass opacity setting description specifically for Modern Flip cards.
* Cleaned up unused compiler flags (-lcomctl32).
* Added Community Contributing & Feedback guidelines.

## 0.4.0 ([Oct 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f09550cbeffb51073c224969a2ba706a481d69b6/mods/lock-screen-clock-styler.wh.cpp))

Initial release.
