## 3.1.1 ([Jul 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c022aaa6684072a8feb8ccb7d6d97ae9c293c7c2/mods/macos-minimize-animation.wh.cpp))

- **Translucent windows no longer go grey**: the capture was force-flattening `PrintWindow` output to opaque and re-premultiplying already-premultiplied pixels. It now passes premultiplied pixels through and keeps fully-transparent regions transparent, so acrylic / Mica windows keep their see-through areas during the genie.
- **Window-only capture**: snapshots render just the window's own content, so the taskbar / other windows never bleed into the animation on maximized or fullscreen apps.
- **Renderer quality**: per-primitive mask antialiasing, aliased RT modes set once at creation, larger corner radius, retuned tile bloat — smoother silhouette without seams at higher mesh resolutions.
- **~120fps frame pacer**: frames land at even intervals when per-frame cost varies (no duration change; a no-op at ≤120Hz where the DwmFlush gate already paces slower).
- **Animation tile count setting** (8–96, default 35), replacing the fixed 20x20 mesh.

**The fixes in this release were contributed by @Potassiumuncher** (v1.5 of his genie engine — he's credited in the mod readme as well).

**Mod-side additions:**
- **Excluded programs setting** (fixes the exclusion-list report): Windhawk's per-mod process exclusion only prevents the mod from loading *into* a process, but a window's animation is often driven from a *different* process (Explorer's taskbar, the shell) — so excluded apps still animated (reported with Windows Terminal Quake mode, where the window belongs to WindowsTerminal.exe, not powershell.exe). The new setting matches the process that *owns* the window at the single animation choke point.
- **Non-primary-monitor backdrop fallback**: on secondary displays DWM often returns the whole Mica/acrylic backdrop as fully transparent, which made window backgrounds vanish during the genie. When a majority of captured pixels are fully transparent, the capture now falls back to an opaque backdrop for that snapshot (documented in Known issues; a deeper fix needs investigation).

Readme also documents the reported (not yet reproduced) looping animation on Zen Browser as a known issue, with the exclusion setting as a workaround.

## 3.1.0 ([Jul 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6a8862447e3495d146a8b444be27f71bb46fcaa0/mods/macos-minimize-animation.wh.cpp))

- Adds a new **Animation style** setting with two options: **Modern** (the Direct2D genie engine, default since v3.0.0) and **Classic** (this mod's original v2.2.0 strip-based genie).
- Some users who tried v3.0.0 said they preferred the original look, so this brings it back as an opt-in instead of replacing it.
- Both styles share the exact same setup, capture, taskbar targeting, first-frame sync, and auto-hide teardown path - only the per-frame render thread differs (`MacGenieAnimThreadClassic` vs `MacGenieAnimThread`), so multi-monitor, launch animation, and auto-hide all behave identically under either style.
- Readme updated: new "Animation style" section, Credits clarifies Modern = Potassiumuncher's engine, Classic = the mod's original.

## 3.0.0 ([Jul 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a5349ec77189b66033f27c0e3c0ac7119a2b2545/mods/macos-minimize-animation.wh.cpp))

New Direct2D genie renderer

This release replaces the previous CPU-warp animation with a proper **Direct2D genie renderer** (a 20×20 mesh warp driven by the macOS "lamp" curve). The rendering engine, the UI Automation taskbar-button targeting, and the taskbar auto-hide handling were contributed by **Potassiumuncher** (https://github.com/Potassiumuncher) — huge thanks. The animation that now plays is his engine, integrated into this mod's hardening (six-hook capture, flash-free DWM-cloak restore, worker-drain on unload, first-frame sync).

### What's new / fixed
- **Direct2D genie mesh** — the whole frame necks down and funnels into the taskbar instead of a coarse strip warp.
- **Accurate taskbar targeting** — locates the app's actual taskbar button via UI Automation (with a per-window / per-process cache), falling back to the cursor/center.
- **Pixel-aligned capture** — geometry now comes from `DWMWA_EXTENDED_FRAME_BOUNDS`, so keyboard / AutoHotkey minimizes are no longer spatially shifted (#4670).
- **Translucency fix** — minimize snapshots are taken from the composited screen image, so acrylic/translucent windows no longer flash grey or opaque.
- **Taskbar auto-hide** — optional: briefly reveals an auto-hidden taskbar so the genie has a dock to flow into, then defers the real minimize.

### Preserved hardening
Six minimize/restore entry-point hooks (`ShowWindow`, `ShowWindowAsync`, `SetWindowPlacement`, `CloseWindow`, `SetWindowPos`, `DefWindowProcW`), DWM-cloak flash-free restore, first-frame sync event, `DwmFlush` vsync pacing, one-genie-per-window guard, and worker-thread drain on unload.

### Known issue
On multi-monitor setups (especially the secondary display), the genie can briefly slide sideways / to the left for a frame or two at the start of a minimize or restore before it flows toward the taskbar. The animation still completes correctly. Documented in the readme; a fix is planned for a follow-up.

## 2.2.0 ([Jul 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/01929c0699e86e614c42934ca6aab388e1068647/mods/macos-minimize-animation.wh.cpp))

Adds an experimental **Multi-monitor support** setting (off by default), as requested by a user on the mod page.

When enabled, the genie plays on the monitor the window is on and flows into that monitor's taskbar edge, instead of always targeting the primary monitor. Works with negative virtual-screen coordinates (monitors left of / above the primary).

Also fixes, when the option is enabled, the taskbar-hover detection for icon-position learning: the previous primary-only work-area test matched every cursor position on a secondary monitor, so a plain title-bar minimize there mislearned the cursor X as the taskbar icon position. It now checks the work area of the monitor the cursor is on.

With the setting off, behavior is unchanged from v2.1.1.

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
