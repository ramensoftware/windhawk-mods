## 1.1.0 ([Nov 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/814cfdde194e6e74c8f4e97df85af01693fb2284/mods/explorer-fast-navigation.wh.cpp))

- Accelerate grouped folder presentation via confined GroupingWork and CDefCollection::_GroupItem hook
- Prevent tab state collisions using bounded TabTable per CShellBrowser and UIItemsView
- Ensure complete deferred task drops in multiple inheritance by tracking canonical IUnknown pointers
- Offload storage metadata probing to NT threadpool work items with direct queue messaging
- Harden unload safety with uninterrupted subclass lifetime guards and fast-path bailout
- Emit factual transition telemetry directly in Wh_Log for DebugView and mod diagnostics

## 1.0.0 ([Nov 11, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2dcb30a8ddc4a006602cc07657f173ef8ab5bfb6/mods/explorer-fast-navigation.wh.cpp))

Initial release.
