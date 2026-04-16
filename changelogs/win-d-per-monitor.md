## 1.5.260416 ([Apr 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6f8261d513e2718b604e812c197ca9d384a7d91c/mods/win-d-per-monitor.wh.cpp))

### 2026-04-16 (v1.5.260416)
- Fixed repeated invocation of touchpad gesture by adding debounce logic to ignore redundant calls
- 修复触摸板手势重复进入问题，添加防抖逻辑忽略连续重复调用

Known issues:
- Touchpad swipe-down can minimize all windows and show the desktop, but swipe-up does not trigger this function, so windows cannot be restored via gesture — swipe down again to toggle
- 已知问题：触摸板下滑可最小化窗口显示桌面，但上滑不会进入此函数，无法通过手势还原窗口，需再次下滑切换

Fixes:
- [#3794](https://github.com/ramensoftware/windhawk-mods/issues/3794)

## 1.4.260415 ([Apr 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/100d1ccbed6a49342e5f3d9c1f025a577525c223/mods/win-d-per-monitor.wh.cpp))

* Switched hook target from `_HandleGlobalHotkey` to `_RaiseDesktop`, enabling per-monitor show desktop for both Win+D hotkey and touchpad gestures
* 将 Hook 目标从 `_HandleGlobalHotkey` 改为 `_RaiseDesktop`，同时支持 Win+D 快捷键和触摸板手势的按显示器显示桌面功能
* Fixes: [#3794: Can we add this feature for the touchpad gesture?](https://github.com/ramensoftware/windhawk-mods/issues/3794)

## 1.3.260330 ([Mar 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/df4fa4ff702cbeb34e8ec0eb2e1ae65dd3e26767/mods/win-d-per-monitor.wh.cpp))

### 2026-03-30 (v1.3.260330)
- Improved window control: try ShowWindowAsync first, then fall back to PostMessage (WM_SYSCOMMAND) if it fails
- 改进窗口控制：优先尝试 ShowWindowAsync，失败时回退到 PostMessage（WM_SYSCOMMAND）
- Added custom ignored windows support by class name or window title
- 新增自定义忽略窗口支持（按类名或窗口标题）

## 1.1.20250811 ([Aug 11, 2025](https://github.com/ramensoftware/windhawk-mods/blob/1e6e18f3884de1bcaecd5eeb0db5a4ced8155e93/mods/win-d-per-monitor.wh.cpp))

### New Features
- Add option to ignore topmost tool windows
- Add configurable minimum window size
- Optimize window switching to prevent taskbar flashing

### Fixes
- https://github.com/ramensoftware/windhawk/discussions/597
- https://github.com/ramensoftware/windhawk-mods/issues/2056

## 1.0.20241214 ([Dec 16, 2024](https://github.com/ramensoftware/windhawk-mods/blob/3527a69c3e55a8acdc4e8d4255af0cd3d5e1566a/mods/win-d-per-monitor.wh.cpp))

Initial release.
