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
