## 1.0.1 ([Sep 8, 2024](https://github.com/ramensoftware/windhawk-mods/blob/f00161757c50921c354688dd50fd0ef968fe36ac/mods/old-this-pc-commands.wh.cpp))

Codebase cleanup
- Remove unnecessary `#include` and `DEFINE_GUID` statements
- Rely on public `IOpenControlPanel` interface instead of finding `COpenControlPanel::Open` from symbols
- Move symbol hook definitions and update the name to match modern convention
- Properly free memory in `OpenCplPage`

## 1.0.0 ([Dec 9, 2023](https://github.com/ramensoftware/windhawk-mods/blob/90d6ccd363cb9764f081ea1981932b2b3dac68cf/mods/old-this-pc-commands.wh.cpp))

Initial release.
