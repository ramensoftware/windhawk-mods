## 2.0 ([Jul 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/846fb5c2697073198ff251c14fd373f94f16f977/mods/taskbar-start-button-corner-fix.wh.cpp))

Reworked corner-click handling to hook the taskbar’s invisible InputSite window procedure and intercept its `WM_POINTER` messages directly. Existing InputSite windows are found through taskbar scans, while `CreateWindowInBand` covers future taskbar recreation. Runtime hook operations are applied only when necessary, with retry and unload protection.

Improved corner detection by validating the taskbar bounds, monitor ownership, adjacent-monitor geometry, DPI-scaled L-shaped region, pointer lifecycle, and left-aligned taskbar setting. The registry check now runs only after the cheaper geometry checks pass.

UI Automation is now limited to Start-button activation on a dedicated worker thread:

- Uses `CUIAutomation8`, COM RAII, provider timeouts, and per-taskbar element caching.
- Coalesces clicks received during UIA lag to avoid repeatedly toggling Start open and closed.
- Adds the `openStartWhenButtonUnavailable` setting, enabled by default. When UIA successfully searches the taskbar but finds no Start button, the mod falls back to pressing the Windows key.
- Does not trigger the fallback for COM errors, UIA provider failures, activation failures, stale taskbars, or worker-queue failures.

The fallback applies to both primary and secondary taskbars. Since Windows normally opens a keyboard-triggered Start menu on the primary monitor, the documentation recommends the [Start menu open location](https://windhawk.net/mods/start-menu-open-location) mod for cursor-monitor placement.

The UIA bounding rectangle is not queried before interception because that would require provider calls on the taskbar input thread or depend on potentially stale cached geometry.

Also updated the Windhawk requirement and settings descriptions, and removed redundant linker options.

## 1.1 ([Mar 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8e16036ab2ebeffb1e2f456a027a736bc4026aa5/mods/taskbar-start-button-corner-fix.wh.cpp))

When app icons in the taskbar were focused, the corner click would get "eaten" by the focus switch and it would take two clicks to open the start menu.

## 1.0 ([Dec 28, 2025](https://github.com/ramensoftware/windhawk-mods/blob/7a97b948c1747274d849d2c53f6f88be9920f95f/mods/taskbar-start-button-corner-fix.wh.cpp))

Initial release.
