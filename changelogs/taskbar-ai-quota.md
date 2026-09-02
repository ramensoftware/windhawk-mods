## 1.6.2 ([Sep 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0a4b26cb2cb709280cbafe70422c4f9aefce092b/mods/taskbar-ai-quota.wh.cpp))

- Add OpenAI credits bar: balance shown against a per-account max, with thresholds and notifications
- Add option to show $ or credit amounts on extra-usage/credits bars instead of percentages
- Fix Anthropic extra-usage bar vanishing at the start of each monthly cycle
- Hide extra OpenAI rate limits (Codex Spark, gpt-reserve) behind one opt-in toggle

## 1.5.8 ([Aug 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d8be922fa99b327d58f6085971c84cfea5863f47/mods/taskbar-ai-quota.wh.cpp))

- Add a new settings system (Right Click taskbar -> Settings) with account management and display controls.
- Add Google Antigravity quota support through its running local app or CLI session.
- Add per-account quota selection, Anthropic Fable and extra-usage bars, pace ticks, compact labels, percentage alignment, and visual previews.
- Improve monitor selection, DPI rendering, tooltip readability, and taskbar layouts.
- Fixed minor bugs and rare crashes (harden OAuth, HTTP retry handling, taskbar lifecycle)

Note: existing Windhawk settings are imported automatically, and stored credentials are preserved.

## 0.10.3 ([Jul 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bb827ab3f6f8cd2dfa0348865e30cb50c3f99fdc/mods/taskbar-ai-quota.wh.cpp))

- Fix tooltip hover being inconsistent (occasionally not showing or not dismissing)

## 0.10.2 ([Jun 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3b5be7d2026357e86981da619e48f0e58af30ecf/mods/taskbar-ai-quota.wh.cpp))

- Built-in OAuth: the mod now signs you in to Anthropic and OpenAI through your browser instead of reading CLI credential files. Tokens are stored encrypted and refreshed automatically. Sign in / Sign out from a column's right-click menu.
- Show/hide individual accounts from the right-click menu.
- Fix: bars no longer overlap the clock/tray on a cold start.
- More robust sign-in and token-refresh handling.

Note: accounts are now just provider + label, so re-check your account settings after updating.

## 0.9.2 ([Jun 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c780b0a668666da572a14c43a7e8f8f22f84cb5d/mods/taskbar-ai-quota.wh.cpp))

Adds a "Bar mode" setting to show remaining quota instead of used

## 0.9.1 ([Jun 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/abc86d1f587043340096e5fafc9019caa29cc2e7/mods/taskbar-ai-quota.wh.cpp))

Initial release.
