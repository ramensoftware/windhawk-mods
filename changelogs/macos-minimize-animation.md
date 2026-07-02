## 2.1.0 ([Jul 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1f0ba9081e3851d184748875cc71eb5f4d6ec1ae/mods/macos-minimize-animation.wh.cpp))

Fixes the genie animation not playing for apps whose windows bypass the classic `ShowWindow` / `WM_SYSCOMMAND` paths:

- **New minimize hooks**: `ShowWindowAsync`, `SetWindowPlacement`, and `CloseWindow` (which minimizes, despite its name). Apps with custom title bars - e.g. the Zed editor - minimize through these, so their own minimize button previously showed no animation while taskbar minimize worked. Also verified to fix minimize for Store/UWP apps such as the modern Media Player.
- **New launch path**: `SetWindowPos` + `SWP_SHOWWINDOW` for frameworks that show their first window without `ShowWindow`, plus a bounded wait for DWM-cloaked (Store/UWP) windows to uncloak before snapshotting, so the launch genie doesn't warp a black frame.
- **Robustness**: a one-genie-per-window guard - with several minimize entry points hooked, a single gesture could otherwise start two overlapping ghost animations. New hook paths also skip hidden/already-minimized windows (e.g. apps restoring a saved minimized session at startup).

Existing behavior for classic Win32 windows is unchanged.

## 2.0.0 ([Jun 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4ec57cbfabddd7e1259a2f4de6d0cd8d27582ec4/mods/macos-minimize-animation.wh.cpp))

Initial release.
