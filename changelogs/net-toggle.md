## 2.1.0 ([Jun 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/962908f1a96bfd70d2e01f45873eaef3117aeef4/mods/net-toggle.wh.cpp))

- Add secondary (fallback) DNS server support
- New default check: real DNS query (fixes false "unreachable" on networks that block TCP/53) — fixes #4288
- New per-server check methods: UDP DNS / TCP 53 / DoT 853 / DoH 443
- New Orange icon state: primary down, fallback still up
- Tooltip shows per-server status
- Minor reliability fixes (DNS state reads, shutdown handle cleanup)

## 2.0.0 ([Jun 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c750ebdd82eef330bdfe48831fa1b5fda89c2192/mods/net-toggle.wh.cpp))

- **Complete rebuild.** Mod renamed to Net-Toggle.
- New: **DNS Monitoring** — The icon monitors your connection and changes color if your internet drops out (configurable in Mod Settings).
- New: **WiFi-style tray icon** with 5 color states (Red / Yellow / Blue / Green / Grey).
- New: Middle-click → **Full Network Reset** (flush DNS, release/renew, winsock + IP reset).
- New: Right-click context menu — toggle network, open Network Settings, or exit.
- New: Donate button on the mod page.
- Improved: Tray icon is now independent from the Windhawk app and no longer groups with it in the taskbar.
- Improved: Tray icon persists reliably across Explorer restarts.
- Improved: Icon stays in sync even if you disable your adapter directly in Windows Settings.
- Fixed: Occasional hangs during network toggles or timeouts.
- Fixed: Tray icon could show the wrong state if a toggle failed.

## 1.0 ([Apr 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0874a2e6656b36f8a8834879ee568d80f39d0ed7/mods/net-toggle.wh.cpp))

Initial release.
