## 1.8.1 ([Aug 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/35441a23d1531df1b1c756e76b67091a71e3e020/mods/aerexplorer.wh.cpp))

- Hook `ExplorerFrame.dll` symbols *after* checking its version
- Fix the "Open" command for EXE files not having an icon with the Command bar icons option enabled

## 1.8.0 ([Aug 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/6944547fe6da8b6465a3cc6d1d000458e0fe0dd7/mods/aerexplorer.wh.cpp))

- Fixes for classic theme
  - NOTE: Arrows not showing on the navigation pane is a bug in comctl32.dll that will probably be fixed in a separate mod at another time.
- Use `LoadLibraryExW` with `LOAD_LIBRARY_SEARCH_SYSTEM32` (#2063)
- Option for "Views" dropdown on left like Windows Vista
- Option for icons on all command bar items like Windows Vista
- Reject ExplorerFrame.dll with a build number higher than 21332 (#1618)

## 1.7.1 ([Jun 23, 2025](https://github.com/ramensoftware/windhawk-mods/blob/7f4839438dad587631cb7478c5c122417f3a5e95/mods/aerexplorer.wh.cpp))

- Fix a crash caused by a missing null terminator in the navbar theme class name

## 1.7.0 ([Jun 23, 2025](https://github.com/ramensoftware/windhawk-mods/blob/ea7ae610c721354ed1390a538ed1f03d39dd7b5a/mods/aerexplorer.wh.cpp))

- Introduce option for Windows 8 beta navigation bar background
  - The logic to draw the background this way is not present in modern Windows.
    Previously, a hack in themes allowed this navbar background, but also interfered with Internet Explorer.
- Fix a bug where glass would apply on ribbon visibility change with the option disabled
- Change the names of most hook function arguments and class members (macros) to match the names from
  private debug symbols
- Completely rewrite init function to use macros, it is now less repetitive
- Modify some of the option descriptions to be clearer

## 1.6.5 ([Mar 31, 2025](https://github.com/ramensoftware/windhawk-mods/blob/ca9dd8c687a23f9d2ff185604e937009269aea4e/mods/aerexplorer.wh.cpp))

Revise `LoadComCtlModule` to not use registry caching

## 1.6.4 ([Aug 25, 2024](https://github.com/ramensoftware/windhawk-mods/blob/a5291e5c748ccaf5b11b04412683a5d23601b9fc/mods/aerexplorer.wh.cpp))

Seamless CPL transition without forcing separate process mode

## 1.6.3 ([Aug 17, 2024](https://github.com/ramensoftware/windhawk-mods/blob/f463da266f92aea2182a8be649b9b83c6fddbf3a/mods/aerexplorer.wh.cpp))

Make compilable on Windhawk 1.5

## 1.6.2 ([Jun 20, 2024](https://github.com/ramensoftware/windhawk-mods/blob/6c9a8296166542e539dc0166b6310b516b19e39c/mods/aerexplorer.wh.cpp))

Navigation pane accuracy fixes for Windows 7 style, code cleanup

## 1.6.1 ([Jun 8, 2024](https://github.com/ramensoftware/windhawk-mods/blob/3ffd256e00086a152824bfc867257abd31545083/mods/aerexplorer.wh.cpp))

Aerexplorer 1.6.1: Merge isCplHooks into efHooks

## 1.6.0 ([Apr 28, 2024](https://github.com/ramensoftware/windhawk-mods/blob/a7455bd32896c65001b46233741c1d08f9f1f008/mods/aerexplorer.wh.cpp))

Support glass + ribbon

## 1.5.9 ([Apr 15, 2024](https://github.com/ramensoftware/windhawk-mods/blob/3d664dac19baf626c8d0c4f2c87d6622135680de/mods/aerexplorer.wh.cpp))

Fix sub-build collection

## 1.5.8 ([Apr 15, 2024](https://github.com/ramensoftware/windhawk-mods/blob/da930af2ef8190e1b68734fb88e1334b0df12b50/mods/aerexplorer.wh.cpp))

Fix `ModernSearchFeatureEnabled` default case

## 1.5.7 ([Apr 15, 2024](https://github.com/ramensoftware/windhawk-mods/blob/45cdb1c86c8fb0d2356fe4e97974917c22959680/mods/aerexplorer.wh.cpp))

Initial release.
