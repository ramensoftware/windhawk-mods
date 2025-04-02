## 1.1 ([Apr 2, 2025](https://github.com/ramensoftware/windhawk-mods/blob/9d6927c8478d7af98c180d73cbe4fec020df474a/mods/sysdm-general-tab.wh.cpp))

This update makes process creation abide by the contract Windhawk sets.

It is not guaranteed for process-creation hooks to be installed during the execution of Wh_ModInit. Sometimes they are available, but the contract states that they should only be considered as available starting from Wh_ModAfterInit.

https://github.com/ramensoftware/windhawk/wiki/Mod-lifetime

## 1.0 ([Apr 2, 2025](https://github.com/ramensoftware/windhawk-mods/blob/61e7bed5c756991ab03e58a4542c71d8d864b5a4/mods/sysdm-general-tab.wh.cpp))

Initial release.
