## 1.16.1 ([Nov 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a0a4a5b54e1b5e87f118e46f69058c0b699343cb/mods/multiwm.wh.cpp))

**Functionality note**
- **The `bsp` layout setting now means the new BSP layout.** Configurations
  saved before 1.13.29 used `bsp` for Dwindle; those now open in BSP. Choose
  Dwindle again in the layout settings to keep the old behaviour.

**New**
- **BSP layout.** New windows split the focused or biggest tile. Drag a window near a tile's edge to split it there, or drop it in the center to swap.
- **Resize hotkeys.** Alt+= grows the focused window, Alt+- shrinks it and Alt+0 resets it. You can hold a key to repeat, and set the step size.
- **Drag preview.** While you drag a tiled window, MultiWM shows where it will land. You can change the opacity, outline, corner radius, app icon size and fade time.
- **Clearer hotkey feedback.** If another program already uses one of MultiWM's shortcuts, a toast names that program. A hotkey that can't do anything now tells you why.
- **Support for the MultiWM Companion mod.**

**Changes**
- "Set Master Window" (Alt+M) is now **Layout Action**. It rotates the focused split in BSP and still promotes the focused window in other layouts.
- "Adaptive Dwindle" settings are now **Adaptive Split** settings and apply to BSP as well. Splits go back to normal when the extra space is no longer needed, unless you've resized a window.
- If you change the management mode with the hotkey or tray menu, it stays that way until MultiWM restarts or you change the setting.
- The Floating default size now applies whenever a window floats for the first time, not only in Automatic mode.

**Fixes and improvements**
- Windows of apps that stop responding are handled better.
- Minor bug fixes and improvements.

## 1.13.59 ([Oct 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b039ab01c4b034c0adea8c12b213192e8085bec9/mods/multiwm.wh.cpp))

- added contributing section 
- added WHAM delimiters for multi-file workflows

## 1.13.58 ([Oct 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9195db122f6e8340b19d69aa1b776a49930d8434/mods/multiwm.wh.cpp))

- Updated BSP to Dwindle layout with adaptive splits and reversible growth direction.
- Starting-layout rules based on desktop name, desktop number, or monitor.
- Number / Constraint-based tiled-window limits, including separate limits for each monitor.
- Overflow handling: float the window or move it to a new desktop with an animated switch. Optionally try another monitor first.
- Window rules to open apps on a chosen monitor or virtual desktop.
- Configurable tray-click action and an option to disable toasts.
- Improved settings with conditional fields, sliders, and file pickers on Windhawk 2.0, while remaining compatible with older versions.
- Bug fixes and improvements

## 1.13.20 ([Sep 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/de3547deee4febfd63a689621a62b6167c23de5f/mods/multiwm.wh.cpp))

* Updated the author's GitHub profile URL after the username changed from `Meteony` to `Meteoni`.
* Updated screenshot and GIF URLs to use the renamed `Meteoni/meteoni-assets` repository.

## 1.13.19 ([Aug 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/128aed34e3f45734daf90ba9c1ef8923c42f61e4/mods/multiwm.wh.cpp))

Initial release.
