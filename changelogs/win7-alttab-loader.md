## 2.1.1 ([Nov 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/12618343e98bf08a6af6465008bd9eda2cdd1c5a/mods/win7-alttab-loader.wh.cpp))

- Use `LoadLibraryExW` with `LOAD_LIBRARY_SEARCH_SYSTEM32` flag
  - This prevents C:\Windows\AltTab.dll from ancient configurations
    being erroneously loaded

## 2.1.0 ([Apr 15, 2025](https://github.com/ramensoftware/windhawk-mods/blob/c3ac7043865e3f5ae95949ebe5d1b3ce81db8cde/mods/win7-alttab-loader.wh.cpp))

- Remove the `LoadStringW` hook hack. Users must now supply an MUI file.
- Add error message boxes for AltTab.dll or its string resources failing to load.
- Support Windows 8.x's AltTab.dll
  - Change mod name to reflect this
- Slightly revise README to be more clear and accurate

## 2.0.0 ([Aug 26, 2024](https://github.com/ramensoftware/windhawk-mods/blob/a1ef83e2b24d3b147686d269bf92c7c37c7feaa0/mods/win7-alttab-loader.wh.cpp))

- Complete rewrite
- Support for UWP app icons
- Aero Peek now works

## 1.0.3 ([Aug 14, 2024](https://github.com/ramensoftware/windhawk-mods/blob/c541b5650cd974bbbf77bfac89f9f12851d58dc0/mods/win7-alttab-loader.wh.cpp))

Windows 7 Alt+Tab Loader 1.0.3: Remove dead WindhawkUtils namespace

## 1.0.2 ([Jun 24, 2024](https://github.com/ramensoftware/windhawk-mods/blob/5ba4de576191570cfd6748518b3ed427391181d0/mods/win7-alttab-loader.wh.cpp))

- Remove misinformation about Windows 10 AltTab.dll
- Better theming recommendations
  - Link to a theme *not* made by Alcatel
  - Suggest better tools for theme editing; WSB was necessary at the time
  because this was all pretty new, but now most Windows 7 themes will come
  with the AltTab class and can be used as a base.
- I also updated the images (the DWM one now shows glass), but that is on
a seperate repository.

## 1.0.1 ([Aug 8, 2023](https://github.com/ramensoftware/windhawk-mods/blob/f960450eb653d48c9ab3177e706b93b59a4e8394/mods/win7-alttab-loader.wh.cpp))

* Update README to link to more proper theme

## 1.0 ([Jul 29, 2023](https://github.com/ramensoftware/windhawk-mods/blob/5f590e1d63e94d5c7a371e0fb0a9374e757531e7/mods/win7-alttab-loader.wh.cpp))

Initial release.
