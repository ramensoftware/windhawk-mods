## 1.3.0 ([Feb 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3c90dd225d03414881f0d9957d66e41bd06081d1/mods/clipboard-history-upgrade.wh.cpp))

The new implementation renames the mod, restructures and expands features (regex replacements, tracking-parameter removal, trimming, unwrap, casing, path escaping, data extraction, Markdown->CF_HTML conversion, force-plain-text), and introduces a TriggerModifierKey setting to require a modifier for processing. It also adds safety checks (clipboard size limit), improved HTML escaping and payload generation, thread-local guards to avoid re-entrancy when injecting formats, and cleaner handling of dropped/non-text formats.

## 1.2.0 ([Feb 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a029fc8cc49ba4ee97412587d7539880ba98edf8/mods/clipboard-history-upgrade.wh.cpp))

Initial release.
