## 1.3.1 ([Feb 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f75c1e904f5c57cd06f2c371b06d4d44a98052c2/mods/clipboard-history-upgrade.wh.cpp))

Reorganize Windhawk mod settings into grouped sections (Core, CleanupAndFormatting, DataExtraction, AdvancedConversions) with descriptive labels and emojis. Update LoadSettings to read nested setting keys and move previous flat keys to the new paths. Introduce g_smartCasingExcludeUrls (default true) and g_markdownToHtml (default true), update their defaults and Wh_Get* lookups. Move regex replacement entries to AdvancedConversions.RegexReplacements. Implement URL-aware casing in ApplyCasing so smart casing skips URLs by detecting URL ranges. Gate Markdown-to-HTML clipboard conversion on the new g_markdownToHtml flag.

## 1.3.0 ([Feb 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3c90dd225d03414881f0d9957d66e41bd06081d1/mods/clipboard-history-upgrade.wh.cpp))

The new implementation renames the mod, restructures and expands features (regex replacements, tracking-parameter removal, trimming, unwrap, casing, path escaping, data extraction, Markdown->CF_HTML conversion, force-plain-text), and introduces a TriggerModifierKey setting to require a modifier for processing. It also adds safety checks (clipboard size limit), improved HTML escaping and payload generation, thread-local guards to avoid re-entrancy when injecting formats, and cleaner handling of dropped/non-text formats.

## 1.2.0 ([Feb 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a029fc8cc49ba4ee97412587d7539880ba98edf8/mods/clipboard-history-upgrade.wh.cpp))

Initial release.
