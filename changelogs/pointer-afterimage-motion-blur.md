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
