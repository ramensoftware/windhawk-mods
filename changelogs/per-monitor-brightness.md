## 3.0 ([Oct 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/cbc35e8691aacc9d6809f1cc2318587e284b6c3c/mods/per-monitor-brightness.wh.cpp))

* **New name:** "Per-monitor brightness in Quick Settings" is now **"Per-monitor brightness and more in Quick Settings"**, because it now does more than brightness.
* **Contrast slider** for each monitor that supports it.
* **Volume slider** for monitors with speakers or a headphone jack, with the same animated speaker icon Windows uses.
* **Input buttons** (HDMI 1, DP 1, ...) to switch what a monitor shows.
* **Power button** at the end of a monitor's brightness slider, to turn it off and back on. Only on monitors that support it, and only while another screen is connected.
* **"All displays" sliders** that set brightness or contrast on every monitor at once.
* **Two layouts:** keep each monitor's extra controls in a dropdown, or show everything.
* **Show or hide anything:** every slider and button has its own setting.
* **Per-monitor settings:** give a monitor your own name, hide it, or hide some of its controls. Two monitors of the same model are numbered, and hovering a name shows its ID.
* **Mouse wheel** moves the slider under the pointer, with an adjustable step (#5647).
* **Click a slider's icon** to jump to a level; left, middle and right click each have their own. Off by default (#5697).
* **Titles show every value**, for example "Samsung · 80% · 50%".
* **Fixes:** the brightness icon sometimes not animating, the slider knob sometimes looking tall, and the percentage in titles sometimes getting stuck.

## 2.1 ([Sep 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/325e75600e4ed1367e8d118969097e64fbc3ef72/mods/per-monitor-brightness.wh.cpp))

- **Configurable Slider Placement**: Added an option to place brightness sliders inside the Quick Settings card (above or below native Windows sliders) instead of below the footer, matching native flyout dimensions, margins, and icon alignment.
- **Hide Unsupported Displays**: Added an optional setting to hide monitors that do not support brightness adjustment (DDC/CI).
- **Fixed Phantom Percentage**: Unsupported monitors no longer display a placeholder "50%" text above the "Brightness control not supported" status.
- **Improved Layout Cleanup**: Refined visual tree handling to ensure smooth enable/disable transitions with zero leftover gaps or layout shifts.

## 2.0 ([Sep 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1e9a228c451f2d5ceabd8b412c07fa726a4d583b/mods/per-monitor-brightness.wh.cpp))

Initial release.
