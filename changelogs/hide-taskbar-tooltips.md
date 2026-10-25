## 1.3.1 ([Oct 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6774848bf06023622d0918a8b2d2110e315a7c9e/mods/hide-taskbar-tooltips.wh.cpp))

- Add `hideStartButtonTooltip` setting to hide or show the Windows Start button tooltip independently of taskbar applications.
- Accurately identify the Start button by querying `AutomationProperties.AutomationId == L"StartButton"` on the `ExperienceToggleButton` instance, preventing misclassification of the Task View button.
- Add `tooltipHoverDelayMs` setting to configure an extra stationary hover delay (in milliseconds) before showing visible tooltips, while keeping hidden tooltip areas permanently suppressed.
- Fall back to native display behavior if delayed tooltip scheduling fails or when tooltips open on external Explorer threads  to prevent them from being swallowed.
- Disarm delayed tooltip Win32 timers synchronously on the `Shell_TrayWnd` UI thread using `SendMessageW` in `Wh_ModBeforeUninit` to guarantee all timers are killed before the DLL is unloaded (`FreeLibrary`).
- Replace heavy C++/WinRT XAML headers with lightweight ABI COM interfaces, cache `IVisualTreeHelperStatics` once per visual tree walk, and adopt WRL `HString` / `HStringReference` to reduce overhead.
- Clean up unused `-lole32` and `-loleaut32` compiler options and remove obsolete `#undef GetCurrentTime`.

## 1.2.0 ([Oct 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e3976aca5c5f511878e4beefdc2e9ef65cd5565b/mods/hide-taskbar-tooltips.wh.cpp))

* Added `hideTrayOverflowTooltips` setting (enabled by default) to suppress hover tooltips for hidden icons inside the system tray overflow drawer.

## 1.1.0 ([Oct 11, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5fa85c1fab7a650c68755b5859adde091b44d9b4/mods/hide-taskbar-tooltips.wh.cpp))

- Add independent settings for taskbar applications and system tray tooltips
- Fix empty tooltip text bug by removing destructive AddToolTipContent hook

## 1.0.8 ([Oct 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4e4ad92d9b819f521487076cb487b999c176390f/mods/hide-taskbar-tooltips.wh.cpp))

Initial release.
