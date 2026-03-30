## 3.1 ([Mar 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1987aab8bd5af5b68db23a159698ef6641c072cb/mods/hotcorner-hotkeys.wh.cpp))

v2.4 → v3 — region fallback chain, duplicate-combo guard, dispatch cleanup
Settings schema:
- Replace flat (hotkey, zone) row model with grouped HotkeyActions where each hotkey entry holds a nested Regions list of region→action mappings with  intent to allow users to easily add action under a hotkey. 
- Add Elsewhere region replacing NONE; 9 zones total (4 corners, 4 edges, Elsewhere)
- Duplicate hotkey group (same key combo appearing twice) rejected at LoadSettings time before any regions are read; first group wins
- Duplicate region rows within the same group: last row wins
- modifiers and vk parsed once in LoadSettings and stored on HotkeyBinding, eliminating redundant re-parsing in RegisterHotkeys.

Dispatch:
- Delete GetCursorRegionChain() (returned heap-allocated std::vector on
  every WM_HOTKEY)
- Replace with inline tryFire lambda + || short-circuit fallback chains.
- Fallback chains: TOP_LEFT→TOP→LEFT→Elsewhere, TOP_RIGHT→TOP→RIGHT→
  Elsewhere, BOTTOM_LEFT→BOTTOM→LEFT→Elsewhere, BOTTOM_RIGHT→BOTTOM→
  RIGHT→Elsewhere; edges fall back to Elsewhere; Elsewhere is final catch-all

Struct / registration cleanup:
- RegisterHotkeys uses stored modifiers/vk directly, no re-parse
- Remove affectedMappings/sample-pointer diagnostic block from RegisterHotkeys
- Invalid case removed from RegionNameToString and ActionTypeToString
  (unreachable at dispatch time); Invalid enumerator kept as parse sentinel
- ParseActionSetting: replace named Invalid case with default to silence
  -Wswitch; extract ParseOpacityUpArgs/ParseOpacityDownArgs helpers so
  OpacityUp and OpacityDown cases are single-line like all other actions

Actions added vs v2.4:
- SelectActiveInExplorer: opens Explorer selecting active window's file,
  falls back to process executable path
- clip; prefix for StartProcess: URL-encodes clipboard and appends to
  remainder; built-in aliases: gpt, yt, google, copilot, x, reddit, translate
- uac; and clip; prefixes combinable (uac; must come first)
- OpacityUp/Down simplified to int alpha model (max;step / min;step)
- ToggleAlwaysOnTop added

## 2.4 ([Feb 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2968e3cadbc5e7a693eaea64170812608ff8ecc7/mods/hotcorner-hotkeys.wh.cpp))

add 3 new actions, extended opacity args, cleanup

## 2.0 ([Feb 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8c9ca28522ced27a2e22342a20ed6041f6ec0a47/mods/hotcorner-hotkeys.wh.cpp))

Add @description metadata to HotCorner Hotkeys mod

## 1.9 ([Feb 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/db7dfb73775465f201b6535f9d4e6dbd39e825dd/mods/hotcorner-hotkeys.wh.cpp))

Initial release.
