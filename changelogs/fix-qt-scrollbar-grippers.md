## 1.1 ([Oct 25, 2024](https://github.com/ramensoftware/windhawk-mods/blob/32c17fb5a213cf117d1cb8d3722d01ec18c9a04d/mods/fix-qt-scrollbar-grippers.wh.cpp))

* Fix control detection. \
Previously, we attempted to draw the gripper overlay for any control that existed, not just scrollbars. This is because the control theme opened by the HTHEME was not checked. Now, we only try drawing scrollbar grippers if we know the control itself is a scrollbar.

## 1.0 ([Aug 3, 2024](https://github.com/ramensoftware/windhawk-mods/blob/ad2ad2e2f07630fcdefb96a16bcc53695bb97490/mods/fix-qt-scrollbar-grippers.wh.cpp))

Initial release.
