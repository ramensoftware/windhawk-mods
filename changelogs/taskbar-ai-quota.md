## 2.18.1 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ad20837f5bc018842c571112b1e60019e831e4a4/mods/taskbar-ai-quota.wh.cpp))

**Taskbar**
- Add eight tile styles: usage graph, segmented ring, concentric rings, gauge, clock pies, rounded square, headline % and quota matrix.
- Add provider logos as account identity, with number badges for accounts that share a provider.
- Add a Position setting for where the bars sit on the taskbar, and support for left and right taskbars.
- Add warning marks on accounts while their provider's status page reports a problem.
- Improve clicks: click a column to refresh it, and right-click it to hide the account or open its history.

**Details flyout**
- Add a details flyout on hover, replacing the tooltip, with usage charts, reset times, pace and when each limit is projected to run out.
- Add model and extra limits, the Claude plan, free resets and Antigravity's other quotas.

**Usage history**
- Add usage history, recorded on this PC and on by default.
- Add a history window that charts accounts together, overlays the previous week and exports CSV.

**Providers**
- Add GitHub Copilot support for its monthly AI credits.
- Add OpenCode Go support with its 5-hour, weekly and monthly limits.
- Add Antigravity's Gemini Argon weekly quota.

**Sign-in**
- Improve sign-in with a window to open or copy the link, Anthropic sign-in without pasting a code, and one-button Copilot sign-in.
- Improve sign-in security by never following HTTP redirects.
- Fix a new sign-in being overwritten by a token refresh that was already running.

**Settings and notifications**
- Improve settings with a redesigned window and a live taskbar preview.
- Add a custom usage color palette and a choice of window material.
- Add notification levels; notifications now show under the mod's name, include the projected limit time and open the provider's dashboard.

**Languages**
- Add German, Spanish, French, Japanese, Portuguese (Brazil), Russian and Chinese translations.
- Improve numbers to follow your Windows regional format.

**Fixes**
- Fix duplicate polls, duplicate notifications and lost sign-ins when Explorer runs extra processes.
- Fix a possible Explorer crash when the mod is disabled while a link opens.

Note: settings and sign-ins carry over, and the mod is renamed Taskbar AI Quota. Usage history recording is on by default; samples stay on this PC, and Settings > History turns it off.

## 1.6.5 ([Sep 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c037cba16d2734813aab18acabc5302060439454/mods/taskbar-ai-quota.wh.cpp))

- Fix quota bars not appearing on newer Windows 11 taskbars.
- Fix Anthropic extra-usage amounts and unlimited detection.
- Fix OpenAI credits-only quota reporting.

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
