## 1.1.0 ([Sep 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/14b59f8f635fae78114073adf62be79cf4dddbea/mods/three-finger-drag.wh.cpp))

* Fixed windows falling behind the cursor with native move in programs slow to move them, such as WhatsApp and Task Manager, and replaying the drag in slow motion after the fingers stopped
* Native move starts sooner: the window is asked to get ready during the first part of the motion, and the cursor keeps following the fingers meanwhile
* macOS drag mode is paced the same way when the press lands on a title bar
* A click of your own can't start a drag: only the mod's own press is taken
* More detail in the log for bug reports

## 1.0.0 ([Sep 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3be36182336cc873fa1e06bee5386bb4aa6de93a/mods/three-finger-drag.wh.cpp))

Initial release.
