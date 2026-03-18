## 1.2.2 ([Mar 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1c888f9b5404e1f03ca934fce7524bfa68bfb597/mods/perform-speedtest-redirect.wh.cpp))

- Refactors implementation with no functional changes: same 3 hooks, same URL detection and
  replacement logic, same setting key
  - Replaces manual case-insensitive loop in `HasLinkId` with `wcsstr` (LINK_ID is already lowercase)
  - Drops `std::wstring lower` copy and `std::transform` call from `ReplaceUrl`
  - Adds `LOG` macro to eliminate repeated `[speedtest]` prefix literals
  - Removes `#include <algorithm>` (no longer needed)
  - Trims hook log verbosity; removes dead `inApp`-only branch in `CreateProcessW_Hook`

## 1.2.1 ([Mar 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/dbb0144332992d59b8ce96d8a8dafe2a8f84d698/mods/perform-speedtest-redirect.wh.cpp))

Initial release.
