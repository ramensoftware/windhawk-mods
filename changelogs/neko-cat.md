## 1.4.0 ([Jul 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5b733b3fa2ccf48f105212d1495530276c87fb51/mods/neko-cat.wh.cpp))

**feat:** Added Neko Dog, context menus, and multi‑monitor improvements.

### Added
- **Neko Dog:** New desktop companion, selectable in settings or mixed with existing litters.  
- **Right‑Click Menu:** Quickly change behaviors (Chase Mouse, Pace, Play With Window).  
- **Save Last Behavior:** Companions now remember their active behavior after restart.  

### Improved
- **Multi‑Monitor Bounds:** Behaviors now align correctly to the monitor edges.  
- **Run Around Physics:** Natural rest cycles (~45s play, ~15s nap) and realistic ball physics with angles/reflections.  
- **Leg Animations:** Walking/running animations scale to actual distance traveled (improved overall animation).
- **Drag & Drop:** Dropping a companion resumes their previous behavior (except Play With Window mode).  
- **Behavior Logic:** Smoother movement and state transitions.  
- **Assets:** Downloads now point to latest commit including Neko Dog assets.  

### Fixed
- **Multi‑Monitor Bugs:** Companions no longer go off‑screen or get stuck.  
- **Window Maximizing:** Double‑clicking companions no longer maximizes underlying windows.  
- **Theme Switcher Leak:** Cleaned up dynamic theme loading to prevent memory leaks.

## 1.3.0 ([Jun 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7afddcbd4e0e0300474719100693301b898f1be3/mods/neko-cat.wh.cpp))

## ✨ New Features
- **Renamed Mod:** From *Neko Cat* → *Desktop Companions*  
- **New Characters:** Sakura and Tomoyo join the lineup 🎉  
- **Diverse Litters:** Option to spawn a randomized mix of all downloaded characters at once  

## ⚡ Performance Improvements
- **High-Performance Texture Slicing:**  
  Replaced 38 separate PNG downloads with a single spritesheet → disk & network checks are **97% faster**!  
- **Dynamic RAM Cloning:**  
  Spritesheets are sliced in memory at startup for smooth, lag-free performance  

## 🛠 Fixes & Enhancements
- **Leak-Free Switcher:** Fixed memory leak when dynamically switching characters in settings  
- **Organized Settings:** Categories are now clean and user-friendly:  
  - Appearance  
  - Behavior  
  - Audio  
  - Advanced  
- **Window Behavior:** Prevent accidental maximization when double-clicking on a desktop companion  

## ⚠️ Important Note
Due to the reorganized settings layout, you may need to **reset or reconfigure your preferences** in the Windhawk settings panel after updating.

---

### 💖 Special Thanks
Big thanks to **@zkyhzxf (quantum)** on Discord for contributing the beautiful Sakura and Tomoyo icons!

## 1.2.1 ([May 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b35660a81b786d8523a45cec049c59d24270b648/mods/neko-cat.wh.cpp))

Initial release.
