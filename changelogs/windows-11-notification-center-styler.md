## 1.4 ([Feb 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/33fa04031ed4e201a3f971addf10d06909ba20ca/mods/windows-11-notification-center-styler.wh.cpp))

* Added new themes: TintedGlass, Fluid.
* Updated themes: TranslucentShell, Matter, WindowGlass.
  * TranslucentShell, Matter, and the new TintedGlass theme now allow to customize the media thumbnail size, which may be too large for small screens. Refer to the the theme's readme for details.
  * WindowGlass now allows to select alternative background styles. Refer to the the theme's readme for details.
* Added a way to define theme-aware resources. For example, different colors for an element can be defined for light mode and dark mode.
* Fixed some styles failing to restore properly.

## 1.3.3 ([Oct 29, 2025](https://github.com/ramensoftware/windhawk-mods/blob/eb68fe5aaceca7a2835888d536a43ebed0b86085/mods/windows-11-notification-center-styler.wh.cpp))

* Updated themes: Unified, 10JumpLists.
* Added themes: WindowGlass, WindowGlass (Alternative), Oversimplified&Accentuated.
* Images from online sources are now fetched once internet becomes available. Previously, if images didn't load successfully, disabling and re-enabling the mod was required.
* WindhawkBlur brushes that use ThemeResource colors are now updated automatically on theme change. Previously, disabling and re-enabling the mod was required to apply the new colors.

## 1.3.2 ([Jul 15, 2025](https://github.com/ramensoftware/windhawk-mods/blob/5e1b574b5bdd18af997aaebe764784b9dc6093ef/mods/windows-11-notification-center-styler.wh.cpp))

* Updated TranslucentShell.
  * Fixed notifications causing a crash of the notification center.
  * Fixed buggy menu hover handling.
* Added ThemeResource support to WindhawkBlur.

## 1.3.1 ([Jul 12, 2025](https://github.com/ramensoftware/windhawk-mods/blob/b924b35e8f24e34cf2cb793a4a593f0a1b48057b/mods/windows-11-notification-center-styler.wh.cpp))

* Updated the TranslucentShell theme. Now the translucent effect works even if transparency effects are disabled in the settings or if energy saving mode is enabled.

## 1.3 ([Jul 11, 2025](https://github.com/ramensoftware/windhawk-mods/blob/deda2477f65c93873120051ae41906734197b23e/mods/windows-11-notification-center-styler.wh.cpp))

* Added a built-in blur brush object, `WindhawkBlur`, which supports the `BlurAmount` and `TintColor` properties. For example: `Fill:=<WindhawkBlur BlurAmount="10" TintColor="#80FF00FF"/>`.
* Updated the Matter theme.

## 1.2 ([May 9, 2025](https://github.com/ramensoftware/windhawk-mods/blob/c34430995b5cd76960f30692a94afba87a2f4215/mods/windows-11-notification-center-styler.wh.cpp))

* Added style constants which can be defined once and used in multiple styles.
* Improved handling of commented targets and styles (starting with `//`), and added details about this in the mod's description.
* Added the Matter theme.
* Updated the TranslucentShell theme.

## 1.1.6 ([Feb 24, 2025](https://github.com/ramensoftware/windhawk-mods/blob/e7c61b93d603d644bf3a2263b59f350da851e664/mods/windows-11-notification-center-styler.wh.cpp))

* Updated the 10JumpLists theme.
* Empty visual states can now be targeted, can be useful in cases when there's no active visual state.

## 1.1.5 ([Jan 5, 2025](https://github.com/ramensoftware/windhawk-mods/blob/b0d7a441fde94fac703f55f232efbb0ea674e803/mods/windows-11-notification-center-styler.wh.cpp))

* Added a new theme: 10JumpLists.
* Fixed not being able to style some elements due to error 802B000A (failed to create a 'System.Type' from the text).

## 1.1.4 ([Oct 26, 2024](https://github.com/ramensoftware/windhawk-mods/blob/6f54b0820eb9261a690c18fc396760e82890975b/mods/windows-11-notification-center-styler.wh.cpp))

* Updated the TranslucentShell theme to make the text more readable when on a light background.

## 1.1.3 ([Oct 2, 2024](https://github.com/ramensoftware/windhawk-mods/blob/dd2309b76c8963fe5a2949677725025be7fd0f84/mods/windows-11-notification-center-styler.wh.cpp))

* Added support for the action center (Win+A) in Windows 11 version 24H2.

## 1.1.2 ([Sep 7, 2024](https://github.com/ramensoftware/windhawk-mods/blob/bf5b0746e2cb9d22e109b664ca3a63c430199f18/mods/windows-11-notification-center-styler.wh.cpp))

* Added a new theme: Unified.
* Added a workaround for hanging the notification center process on Windows 10. Although the available themes and example styles are designed for Windows 11, it should now be possible to use the mod on Windows 10 too.

## 1.1.1 ([Jul 28, 2024](https://github.com/ramensoftware/windhawk-mods/blob/931b937d1cfa9782375c939c104de17bf0b5047a/mods/windows-11-notification-center-styler.wh.cpp))

* Updated the TranslucentShell theme.

## 1.1 ([Jun 27, 2024](https://github.com/ramensoftware/windhawk-mods/blob/78d005fa29bcbfaab1f5ea2777f1217468703c9f/mods/windows-11-notification-center-styler.wh.cpp))

* Added themes! Currently, only the TranslucentShell theme is integrated into the mod and can be selected in the settings.

## 1.0.2 ([May 31, 2024](https://github.com/ramensoftware/windhawk-mods/blob/324f683b222562fba882a506476da597fce9fe6f/mods/windows-11-notification-center-styler.wh.cpp))

* The mod now targets all ShellExperienceHost.exe windows, including the notification center, taskbar jump lists and notification popups.

## 1.0.1 ([May 27, 2024](https://github.com/ramensoftware/windhawk-mods/blob/0959f2f57a96dae3785498c063209383ab6b8bdb/mods/windows-11-notification-center-styler.wh.cpp))

* Made styles defined later override styles defined earlier.
* Empty values are now allowed. Setting an empty value with the XAML syntax clears the property value.
* Added a workaround for setting FontWeight which didn't work.
* Fixed the mod not working until some settings are changed.

## 1.0 ([May 11, 2024](https://github.com/ramensoftware/windhawk-mods/blob/3f97fef9c8b453dfee716aab2e50a636f04234fd/mods/windows-11-notification-center-styler.wh.cpp))

Initial release.
