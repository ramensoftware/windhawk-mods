## 1.2 ([Nov 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/cda8376695f4e5390fa76ce4ac7566d9e9db99e2/mods/pointer-afterimage-motion-blur.wh.cpp))

* No more trail across the screen when the pointer jumps across a large part of the screen, such as with PowerToys Cursor Wrap.
* Fixes flickering hover highlights in Office menus (such as Excel's recent files) while the blur is drawn. The overlay window used to move and resize with the blur on every frame, and Windows reports each of those changes to the window under the pointer as a mouse move. Now the window stays put while the pointer is still and only its picture changes.
* The blur is now the same size as the pointer at 125% and 175% scaling; it was drawn up to 25% larger there. Windows scales the pointer in steps (the same size at 100% and 125%), not by the exact scaling.
* A new pointer size is picked up the next time the mouse moves after a pause; before, the blur kept the old size until the mod was restarted.
* A little less CPU for the mod and dwm.exe while the blur is drawn. Memory goes up to 7-8 MB (more on larger screens) for short bursts while the blur is drawn (the window is now larger than the blur) and drops back once the mouse stops.

## 1.1 ([Oct 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ba10ab3e6ea5980b451594f0dc260d9986415f0f/mods/pointer-afterimage-motion-blur.wh.cpp))

* The blur now shows above the Start menu, Search and the notification centre: explorer.exe creates the overlay window in a higher band with `CreateWindowInBand` (as suggested), and the mod's own process draws into it. Nothing else runs in explorer.exe.
* New "Minimum blur speed" setting: slower movements draw no blur and use almost no CPU.
* Trail opacity now means what it says: 100% looks solid (the overlapping copies used to add up to about 60%), and the fade holds near full strength behind the pointer.
* New defaults: opacity 50, minimum blur speed 29 cm/s, trail 25 ms, hiding on above 30 cm/s. Trail opacity is stored under a new name, so everyone starts from 50 (an old 92 would now look nearly solid).
* The blur now matches the pointer size set in Windows; before, it was always drawn at size 1. Large pointers stay sharp.
* The blur now works over administrator windows, such as Task Manager or a terminal run as administrator.
* No CPU at all while idle: window activations come from the shell hook instead of polling.
* README: limitation, resource use with benchmarks, troubleshooting.

## 1.0 ([Oct 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fb5e852a66d521f2f6496b2dfb3d0d8de33e4268/mods/pointer-afterimage-motion-blur.wh.cpp))

Initial release.
