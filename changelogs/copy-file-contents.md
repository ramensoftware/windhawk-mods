## 1.4.1 ([Nov 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/50e514baec095cd9fa07354bcefc3b82dc121306/mods/copy-file-contents.wh.cpp))

* Fix SHA-256 read errors being mistaken for end-of-file, which could produce a hash of incomplete data.
* Report complete hash and clipboard-write failures, and identify successful files in joined partial multi-file results.
* Improve unload handling, attempt to cancel tracked synchronous file calls, clean up resources and synchronize settings; correct the initialization version log and documentation.

## 1.4 ([Nov 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0cb1e6720db42338d92199f507ca84c4b23c7fcc/mods/copy-file-contents.wh.cpp))

Initial release.
