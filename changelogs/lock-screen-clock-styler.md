## 3.0.0 ([Nov 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/01c358fdfa0df94fd9192933fa37190a144f9c08/mods/lock-screen-clock-styler.wh.cpp))

### New Clock Style
* **ANSI Shadow (Terminal ASCII):** Added a new retro terminal ASCII block-art clock style (`ansiShadow`) with 4 phosphor color presets (Matrix Green, Amber CRT, Cyber Cyan, Classic Monochrome).

### Per-Theme Customization Groups
Added dedicated per-theme customization sections with rich new options:
* **Retro Pixel (`pixelSettings`):** LED dot shape (`Circle`, `Square`, `Rounded`), 14 curated LED colors, and frosted backplate toggle & opacity (`0–255`).
* **Vertical Stack Clocks (`verticalSettings`):** Row spacing adjustment, minute digit opacity control, 14 curated text colors, custom font selector (built-in Windows fonts), and drop-shadow toggle.
* **ANSI Shadow (`ansiSettings`):** 4 terminal phosphor color presets (`amber`, `green`, `cyan`, `mono`), terminal prompt header bar toggle, phosphor glow toggle, and card opacity (`0–255`).
* **Flip Clocks (`flipSettings`):** Frosted glass / card opacity, corner radius (`-1` theme default or custom), center split seam toggle, and 14 curated digit colors.
* **Minimalist Sonoma (`minimalistSettings`):** Font weight selector, 14 curated text colors, custom font selector (built-in Windows fonts), and drop-shadow toggle.
* **Cyberpunk Sci-Fi HUD (`cyberpunkSettings`):** 4 HUD color presets (`default`, `matrix`, `crimson`, `violet`), top telemetry header bar toggle, and chassis opacity (`0–255`).

### Breaking & Behavior Changes
* **Minimalist Aura style removed:** Existing `aura` configurations are automatically migrated to **Minimalist Sonoma** (`macos`).
* **`hideDefaultClock` removed:** The stock Windows lock screen clock is now always hidden whenever a custom clock style is active. To keep the stock Windows clock visible instead, select **Default Clock Only** (`defaultOnly`).
* **`showSeconds` changed from a toggle to `auto` / `on` / `off`:** Existing boolean toggle values fall back to `auto` (theme default: enabled on Retro Pixel, ANSI Shadow, and Cyberpunk HUD; disabled on Minimalist Sonoma). Users who previously turned seconds off can set `showSeconds` to **Always Hide** (`off`).
* **`glassOpacity` moved:** The Flip clock glass opacity setting is now located under the **Flip Clock** (`flipSettings.glassOpacity`) group. Users who had a custom value can re-apply it in the new group.

### Improvements & Polish
* **14-Locale Settings UI:** Added emoji prefixes across settings titles and polished translations across all 14 supported languages.
* **Refined Defaults:** Updated default `clockFontSize` to `165` and tuned Retro Pixel & Cyberpunk HUD defaults.
* **Updated Docs & Screenshots:** Refreshed README and mod screenshots for v3.0.0, and updated license to `GPL-3.0-only`.

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
