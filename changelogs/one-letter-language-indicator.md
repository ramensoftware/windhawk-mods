## 1.1 ([Nov 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4095aca81f4030254c8f8d87199dd6a37bdb71fb/mods/one-letter-language-indicator.wh.cpp))

* New **Letter weight** setting: font weight of the letter from 1 to 999 (100 Thin … 900 Black). 0 keeps the Windows weight (400).
* Overrides `TextBlock.FontWeight` on `InnerTextBlock` with the same property-callback mechanism as the other settings, and restores the original weight on unload or when set back to 0.
* Fixed: the indicator went back to the full text after switching the taskbar size between small and default.

## 1.0 ([Nov 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b4d2a8ee55b15e05776a0a38fe0c22d62c08a931/mods/one-letter-language-indicator.wh.cpp))

Initial release.
