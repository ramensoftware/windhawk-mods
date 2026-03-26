## 1.1 ([Mar 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/abcbd90ab2482b23364890fda64981aab39f5b49/mods/auto-time-sync-on-startup.wh.cpp))

- Fixed the session-run guard logic to use a real volatile registry subkey instead of a persistent value under `HKCU\Software`.
- Added cleanup for the legacy persistent guard value that could make the mod think the sync had already been done after reboot.
- Version bumped to `1.1`.

## 1.0 ([Mar 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c3079a58100cd96c3ec7cda1f12a83dc664fee45/mods/auto-time-sync-on-startup.wh.cpp))

Initial release.
