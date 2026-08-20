## 0.1.28 ([Aug 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/74549269a15a4e540dea200a961aa836b65f3714/mods/cjk-spacer.wh.cpp))

Fix a confirmed startup race in the opt-in modern XAML path. When Explorer, Windhawk, and notification-area applications start concurrently, a diagnostics attempt can run before the corresponding XAML core is ready. The previous one-shot attempt then remains disabled until a mod reload.

The modern diagnostics path now:

- reacts to matching XAML host-window creation through narrowly filtered `CreateWindowExW`, `CreateWindowInBand`, and `CreateWindowInBandEx` hooks instead of DLL-load timing or timed polling;
- scans existing top-level and child windows in the current Explorer process from `Wh_ModAfterInit`, stopping once both XAML flavors are already queued or no longer need a trigger;
- maps `XamlExplorerHostIslandWindow` and a taskbar-owned `Windows.UI.Composition.DesktopWindowContentBridge` to Windows.UI.Xaml, and maps `CabinetWClass` plus `XamlExplorerHostIslandWindow_WASDK` to Microsoft.UI.Xaml;
- only signals a managed worker from window hooks, keeping COM activation and XAML Diagnostics work outside window-creation call stacks;
- coalesces Windows.UI.Xaml and Microsoft.UI.Xaml requests independently;
- allows hard connection failures to be retried by later matching host-window notifications, with at most three attempts per flavor;
- suppresses further attempts when a diagnostics call succeeds without creating a watcher, avoiding repeated conflicts with another Diagnostics consumer;
- resets only the affected flavor's attempt budget when an established connection disconnects;
- stops and joins the worker before unload, preserving the existing thread-lifetime guarantees.

The connection-name walk matches the range the reference mods use (up to 10,000 names), because the endpoint name embeds a process-lifetime core ordinal that is never reused and a live endpoint can sit at a high index. The cost of a failed walk is bounded by the walk count instead: after three empty walks in a session a flavor stops probing until an established connection disconnects. The walk runs only after a matching host-window or framework-module trigger indicates that the corresponding core should be ready.

## 0.1.27 ([Aug 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d021d038b75876dc7345cfe32cd13b079be697a7/mods/cjk-spacer.wh.cpp))

Initial release.
