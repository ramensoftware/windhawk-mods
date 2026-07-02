## 2.1.1 ([Jul 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/51bb09f1a2f997764267bc016299b00c3bda12cc/mods/macos-minimize-animation.wh.cpp))

Follow-up to #4659 (merged while this final commit was being pushed) - two user-visible timing fixes, found by testing under CPU throttling (battery saver), where both artifacts become clearly visible:

- **Restores are hidden via `DWMWA_CLOAK` instead of `WS_EX_LAYERED` + alpha 0.** Freshly adding the layered style makes DWM rebuild the window's redirection surface, and until that lands the bare window frame is composed for a few frames - a flash on every restore (previously masked by the very style leak #4659 fixed, since windows stayed layered forever). A cloaked window is not rendered at all (frame included), has no rebuild lag, and the restore path no longer touches window styles - structurally removing the layered-leak class there. The launch path keeps the layered mechanism (cloaking would collide with its wait-for-uncloak handling for Store apps), with the true pre-hide style threaded through as per #4659.
- **The real minimize is gated on the genie's first frame being composed.** Previously the hook let the actual minimize run while the animation thread was still starting up, so the window vanished a few frames before the ghost appeared - a background flash in the gap. The hook now waits (capped at 200 ms; duplicated event handles so neither thread can touch a freed handle) until the animation thread has composed its first frame via `DwmFlush`, then performs the minimize invisibly underneath the ghost. Applies to every minimize entry point; on timeout the behavior is simply the previous one.

Verified on a throttled machine: no frame flash on restore, no gap flash on minimize, and the rollback/unload guarantees from #4659 are preserved (all bail paths route through a single undo helper that matches the reveal to the hide).

## 2.1.0 ([Jul 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1f0ba9081e3851d184748875cc71eb5f4d6ec1ae/mods/macos-minimize-animation.wh.cpp))

Fixes the genie animation not playing for apps whose windows bypass the classic `ShowWindow` / `WM_SYSCOMMAND` paths:

- **New minimize hooks**: `ShowWindowAsync`, `SetWindowPlacement`, and `CloseWindow` (which minimizes, despite its name). Apps with custom title bars - e.g. the Zed editor - minimize through these, so their own minimize button previously showed no animation while taskbar minimize worked. Also verified to fix minimize for Store/UWP apps such as the modern Media Player.
- **New launch path**: `SetWindowPos` + `SWP_SHOWWINDOW` for frameworks that show their first window without `ShowWindow`, plus a bounded wait for DWM-cloaked (Store/UWP) windows to uncloak before snapshotting, so the launch genie doesn't warp a black frame.
- **Robustness**: a one-genie-per-window guard - with several minimize entry points hooked, a single gesture could otherwise start two overlapping ghost animations. New hook paths also skip hidden/already-minimized windows (e.g. apps restoring a saved minimized session at startup).

Existing behavior for classic Win32 windows is unchanged.

## 2.0.0 ([Jun 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4ec57cbfabddd7e1259a2f4de6d0cd8d27582ec4/mods/macos-minimize-animation.wh.cpp))

Initial release.
