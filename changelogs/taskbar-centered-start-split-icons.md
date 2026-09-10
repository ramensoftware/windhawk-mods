## 1.0.0 ([Sep 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/272c8e5c94ca8de5f6cae88ab69698695abb927b/mods/taskbar-centered-start-split-icons.wh.cpp))

* Fixed the `kArmResolveNowMsg` handler in `BackgroundWorkerThreadProc` dropping a `SetTimer` failure. It killed the HWND-resolve timer and then discarded `SetTimer`'s return value, so a failure left no resolve timer running, with nothing logged. It now arms the replacement first and only kills the old one once the new id is known good — the same ordering `SetDragFollowPollInterval` already uses a few lines away, which is where this was noticed. Milder than that sibling case: the resolve timer is re-armed from several independent places on the taskbar thread (a real click, an `ArrangeOverride` count change, a subclass install), so losing it meant "recovers whenever something unrelated nudges it" rather than "never recovers". It now recovers on its own schedule, and a failure is logged rather than swallowed.

## 0.1.0 ([Sep 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/439154bf481990369a27159931010092934dbb06/mods/taskbar-centered-start-split-icons.wh.cpp))

Initial release.
