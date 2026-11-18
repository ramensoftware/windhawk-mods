## 1.1.0 ([Nov 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d70192267c179a459be7217c1f56d3cc96f94736/mods/winrar-tweaks.wh.cpp))

### ✨ New Features
- **VS Code-Style Quick Filter Bar (`Ctrl+F`):** Added a native bottom filter bar (`quick_search_bar`) with a live match counter, `↑` / `↓` / `✕` buttons, `Enter` / `Shift+Enter` navigation, and automatic Light/Dark theme adaptation. Includes optional settings for **Always Visible** (`always_visible`), **Type to Search** (`type_to_search`), and **Alt+Enter Selects All Matches** (`alt_enter_select_all`).
- **Auto-Dismiss Trial Popup:** Added `auto_dismiss_eval_dialog` to automatically block *"Please purchase WinRAR license"* and *"Trial Expired"* reminder popups on startup before they appear on screen.
- **Optional IEC Binary Units (`KiB` / `MiB`):** Added `use_iec_terms` to switch file size labels between standard (`KB`, `MB`, `GB`, `TB`) and IEC (`KiB`, `MiB`, `GiB`, `TiB`) units on the fly.

### 🛠️ Improvements & Fixes
- **Universal Language Support for File Sizes:** Replaced hardcoded column header keywords with semantic column and byte-format detection (excluding `Name` and `CRC` columns), making human-readable file sizes work automatically across all WinRAR display languages.
- **Search-Aware `Esc` Handling:** Updated `Close on Esc` so pressing `Escape` clears or closes the Quick Filter Bar first (while preserving multi-selections) before closing the main WinRAR window.
- **Live Settings Sync & Thread Safety:** Added custom UI-thread messaging and `std::atomic` settings so toggling options (`KB`/`KiB`, `Always Visible`, etc.) or unloading the mod updates open WinRAR windows immediately without freezing.

## 1.0.0 ([Nov 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b7d208bbef124a2d7be33f9cb2099cd6faf6e33a/mods/winrar-tweaks.wh.cpp))

Initial release.
